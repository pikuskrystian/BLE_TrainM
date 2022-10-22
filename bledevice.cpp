#include "bledevice.h"

BLEDevice::BLEDevice(QObject *parent) : QObject(parent),
    currentDevice(QBluetoothDeviceInfo())
{

    DiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    DiscoveryAgent->setLowEnergyDiscoveryTimeout(10000);


    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLEDevice::addDevice);
    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &BLEDevice::deviceScanError);
    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLEDevice::scanFinished);
    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BLEDevice::scanFinished);

    accRange = 1;

    connect(&dataProcessing, &DataProcessing::statusChange, this, &BLEDevice::getStatus);
}

BLEDevice::~BLEDevice()
{
    delete DiscoveryAgent;

    qDeleteAll(controllers);
    controllers.clear();

    qDeleteAll(services);
    services.clear();


    bFoundSensorService.clear();

}

QStringList BLEDevice::deviceListModel()
{
    return m_deviceListModel;
}

void BLEDevice::setDeviceListModel(QStringList deviceListModel)
{
    if (m_deviceListModel == deviceListModel)
        return;

    m_deviceListModel = deviceListModel;
    emit deviceListModelChanged(m_deviceListModel);
}

void BLEDevice::resetDeviceListModel()
{
    m_deviceListModel.clear();
    emit deviceListModelChanged(m_deviceListModel);
}

void BLEDevice::addDevice(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qDebug()<<"Discovered Device:"<<device.name()<<"Address: "<<device.address().toString()<<"RSSI:"<< device.rssi()<<"dBm";

        if(!m_foundDevices.contains(device.name(), Qt::CaseSensitive) && device.name().contains("Train", Qt::CaseSensitive)) {
            m_foundDevices.append(device.name());

            DeviceInfo *dev = new DeviceInfo(device);
            qlDevices.append(dev);



            QList <quint16> mid = device.manufacturerIds();

            qDebug()<<"------------------------------";

            for(int i=0;i<mid.size();i++) {
                QByteArray mdata=device.manufacturerData(mid[i]);

                qDebug()<<mdata.toHex('-');

                //      auto frame = reinterpret_cast<const quint8 *>(mdata.constData());
                //                quint8 BeaconType=static_cast<quint8>(frame[0]);
                //                quint8 BeaconLen=static_cast<quint8>(frame[1]);

                //    qDebug()<<frame;
                qDebug()<<"------------------------------";
            }
            // startConnect(qlDevices.size()-1);
            // startConnect(controllerIndex);
            // DiscoveryAgent->stop();
        }
    }
}

void BLEDevice::scanFinished()
{
    setDeviceListModel(m_foundDevices);
    emit scanningFinished();
    emit statusChange(QString("Devices found: %1").arg(m_foundDevices.size()));
}

void BLEDevice::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError) {

        qDebug() << "The Bluetooth adaptor is powered off.";
        emit statusChange("The Bluetooth adaptor is powered off.");
    } else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError) {

        qDebug() << "Writing or reading from the device resulted in an error.";
        emit statusChange("Writing or reading from the device resulted in an error.");
    } else {

        qDebug() << "An unknown error has occurred.";
        emit statusChange("An unknown error has occurred.");
    }
}

void BLEDevice::startScan()
{
    qDeleteAll(qlDevices);
    qlDevices.clear();
    m_foundDevices.clear();
    resetDeviceListModel();

    DiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    qDebug()<< "Searching for BLE devices..." ;
    emit statusChange("Searching for BLE devices...");
}

void BLEDevice::startConnect(int i)
{
    currentDevice.setDevice(((DeviceInfo*)qlDevices.at(i))->getDevice());

    QLowEnergyController *controller = 0;
    controller = QLowEnergyController::createCentral(currentDevice.getDevice());
    controller->setRemoteAddressType(QLowEnergyController::RandomAddress);

    controllers.append(controller);
    controllerIndex=controllers.size()-1;

    QLowEnergyService *service = 0;
    services.append(service);
    bFoundSensorService.append(0);

    connect(controllers.last(), &QLowEnergyController::serviceDiscovered, this, &BLEDevice::serviceDiscovered);
    connect(controllers.last(), &QLowEnergyController::discoveryFinished, this, &BLEDevice::serviceScanDone);
    connect(controllers.last(), &QLowEnergyController::errorOccurred,  this, &BLEDevice::controllerError);
    connect(controllers.last(), &QLowEnergyController::connected, this, &BLEDevice::deviceConnected);
    connect(controllers.last(), &QLowEnergyController::disconnected, this, &BLEDevice::deviceDisconnected);

    controllers.last()->connectToDevice();

    qDebug()<<controllerIndex<<controllers.size()<<services.size()<<bFoundSensorService.size();

    emit statusChange(QString("Controller state: [%1 %2 %3 %4 %5]").arg(controllers.size()).arg(services.size()).arg(bFoundSensorService.size()));
}

void BLEDevice::disconnectFromDevice()
{

    for(int i=0; i<controllers.size(); i++) {

        if (controllers[i]->state() != QLowEnergyController::UnconnectedState) {
            controllers[i]->disconnectFromDevice();
        } else {
            deviceDisconnected();
        }
    }

    qDeleteAll(controllers);
    controllers.clear();

    qDeleteAll(services);
    services.clear();


    bFoundSensorService.clear();

    bRun = false;
    emit connectionEnd();
}

void BLEDevice::serviceDiscovered(const QBluetoothUuid &gatt)
{
    qDebug() <<"serviceDiscovered";

    if(gatt==QBluetoothUuid(QUuid(COMMSERVICEUUID))) {
        bFoundSensorService[controllerIndex] =true;
        qDebug() << "Sensor service found";
    }
}

void BLEDevice::serviceScanDone()
{
    qDebug() <<"serviceScanDone";
    emit statusChange("Services Scan Done");

    delete services[controllerIndex];
    services[controllerIndex] = 0;

    if(bFoundSensorService[controllerIndex]) {
        qDebug() << "Connecting to Sensor service...";
        services[controllerIndex] = controllers[controllerIndex]->createServiceObject(QBluetoothUuid(QUuid(COMMSERVICEUUID)),this);
    }

    if(!services[controllerIndex]) {
        qDebug() <<"Sensor service not found";
        disconnectFromDevice();
        return;
    }

    connect(services[controllerIndex], &QLowEnergyService::stateChanged,this, &BLEDevice::serviceStateChanged);
    if(controllerIndex == 0) {
        connect(services[controllerIndex], &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData);
    }
    if(controllerIndex == 1) {
        connect(services[controllerIndex], &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData2);
    }
    if(controllerIndex == 2) {
        connect(services[controllerIndex], &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData3);
    }
    if(controllerIndex == 3) {
        connect(services[controllerIndex], &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData4);
    }
    if(controllerIndex == 4) {
        connect(services[controllerIndex], &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData5);
    }
    connect(services[controllerIndex], &QLowEnergyService::descriptorWritten,this, &BLEDevice::confirmedDescriptorWrite);
    services[controllerIndex]->discoverDetails();
}

void BLEDevice::deviceDisconnected()
{
    qDebug() << "Remote device disconnected";
    emit statusChange("Remote device disconnected");
    //  emit connectionEnd();

}

void BLEDevice::deviceConnected()
{
    qDebug() << "Device connected";
    controllers[controllerIndex]->discoverServices();
}

void BLEDevice::controllerError(QLowEnergyController::Error error)
{
    qDebug() << "Controller Error:" << error;
    emit statusChange(QString("Controller Error: %1").arg(error));
}

void BLEDevice::serviceStateChanged(QLowEnergyService::ServiceState s)
{

    switch (s) {
    case QLowEnergyService::RemoteServiceDiscovered:
    {
        //Sensor characteristic
        const QLowEnergyCharacteristic  sensorChar = services[controllerIndex]->characteristic(QBluetoothUuid(QUuid(CONTROLUUID)));
        if (sensorChar.isValid()) {
            //Change FS and Range
            // setSensorConfig(0, 0);
        } else {
            qDebug() << "Sensor characteristic not found";
            break;
        }

        // Sensor notify enabled
        const QLowEnergyDescriptor m_notificationDesc = sensorChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid()) {
            // enable notification
            services[controllerIndex]->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
            qDebug() << "Notification enabled";
            emit connectionStart();



            //            if(controllerIndex < qlDevices.size()) {
            //                startConnect(controllerIndex);
            //            }

            //            if(controllerIndex == 4) {
            //                emit devicesReady();
            //            }
            if(controllerIndex == (m_foundDevices.size() - 1) &&  (controllerIndex < 5)) {
                emit devicesReady();
            }
        }
        break;
    }
    default:

        break;
    }
}



void BLEDevice::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == notificationDesc && value == QByteArray("0000")) {
        controllers[controllerIndex]->disconnectFromDevice();
        delete services[controllerIndex];
        services[controllerIndex] = nullptr;
    }
}

void BLEDevice::writeData(quint8 i, QByteArray v)
{
    if(services[i]) {

        const QLowEnergyCharacteristic  configChar = services[i]->characteristic(QBluetoothUuid(QUuid(CONTROLUUID)));
        services[i]->writeCharacteristic(configChar, v, QLowEnergyService::WriteWithoutResponse);
    }
}



void BLEDevice::updateData(const QLowEnergyCharacteristic &c, const QByteArray &v)
{
    if (c.uuid() == QBluetoothUuid(QUuid(CONTROLUUID))) {

        qDebug()<<v.toHex('-');


        switch(v[2]) {

        case (char)MessageType::HUB_ATTACHED_IO:
        {

            uint8_t port = v[3];
            bool isConnected = (v[4] == 1 || v[4] == 2) ? true : false;
            if (isConnected)
            {
                qDebug()<<"port "<< port<< "is connected with device "<<(uint8_t)v[5];

            }
            else
            {
                qDebug()<<"port "<< port<< "is disconnected";

            }
            break;
        }


        case (char)MessageType::HUB_PROPERTIES:
        {
            qDebug()<<"HUB_PROPERTIES ";

            if (v[3] == (uint8_t)HubPropertyReference::ADVERTISING_NAME)
            {
                //  log_d("advertising name: %s", parseHubAdvertisingName(pData).c_str());

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BUTTON)
            {
                //log_d("hub button state: %x", parseHubButton(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::FW_VERSION)
            {
                // Version version = parseVersion(pData);
                //log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::HW_VERSION)
            {
                //Version version = parseVersion(pData);
                // log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::RSSI)
            {
                //log_d("rssi: ", parseRssi(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_VOLTAGE)
            {
                //log_d("battery level: %d %", parseBatteryLevel(pData));
                qDebug()<<"battery level: "<<(uint8_t)v[5]<<"%";
                emit batteryLevel((uint8_t)v[5], 0);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_TYPE)
            {
                //log_d("battery type: %d", parseBatteryType(pData));
                qDebug()<<"battery type: "<<(uint8_t)v[5];

            }
            else if (v[3] == (uint8_t)HubPropertyReference::SYSTEM_TYPE_ID)
            {
                //log_d("system type id: %x", parseSystemTypeId(pData));

            }

            break;
        }

        }

    }
}

void BLEDevice::updateData2(const QLowEnergyCharacteristic &c, const QByteArray &v)
{
    if (c.uuid() == QBluetoothUuid(QUuid(CONTROLUUID))) {

        switch(v[2]) {

        case (char)MessageType::HUB_ATTACHED_IO:
        {

            uint8_t port = v[3];
            bool isConnected = (v[4] == 1 || v[4] == 2) ? true : false;
            if (isConnected)
            {
                qDebug()<<"port "<< port<< "is connected with device "<<(uint8_t)v[5];

            }
            else
            {
                qDebug()<<"port "<< port<< "is disconnected";

            }
            break;
        }


        case (char)MessageType::HUB_PROPERTIES:
        {
            qDebug()<<"HUB_PROPERTIES ";

            if (v[3] == (uint8_t)HubPropertyReference::ADVERTISING_NAME)
            {
                //  log_d("advertising name: %s", parseHubAdvertisingName(pData).c_str());

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BUTTON)
            {
                //log_d("hub button state: %x", parseHubButton(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::FW_VERSION)
            {
                // Version version = parseVersion(pData);
                //log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::HW_VERSION)
            {
                //Version version = parseVersion(pData);
                // log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::RSSI)
            {
                //log_d("rssi: ", parseRssi(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_VOLTAGE)
            {
                //log_d("battery level: %d %", parseBatteryLevel(pData));
                qDebug()<<"battery level: "<<(uint8_t)v[5]<<"%";
                emit batteryLevel((uint8_t)v[5], 1);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_TYPE)
            {
                //log_d("battery type: %d", parseBatteryType(pData));
                qDebug()<<"battery type: "<<(uint8_t)v[5];

            }
            else if (v[3] == (uint8_t)HubPropertyReference::SYSTEM_TYPE_ID)
            {
                //log_d("system type id: %x", parseSystemTypeId(pData));

            }

            break;
        }

        }
    }
}
void BLEDevice::updateData3(const QLowEnergyCharacteristic &c, const QByteArray &v)
{
    if (c.uuid() == QBluetoothUuid(QUuid(CONTROLUUID))) {
        switch(v[2]) {

        case (char)MessageType::HUB_ATTACHED_IO:
        {

            uint8_t port = v[3];
            bool isConnected = (v[4] == 1 || v[4] == 2) ? true : false;
            if (isConnected)
            {
                qDebug()<<"port "<< port<< "is connected with device "<<(uint8_t)v[5];

            }
            else
            {
                qDebug()<<"port "<< port<< "is disconnected";

            }
            break;
        }


        case (char)MessageType::HUB_PROPERTIES:
        {
            qDebug()<<"HUB_PROPERTIES ";

            if (v[3] == (uint8_t)HubPropertyReference::ADVERTISING_NAME)
            {
                //  log_d("advertising name: %s", parseHubAdvertisingName(pData).c_str());

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BUTTON)
            {
                //log_d("hub button state: %x", parseHubButton(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::FW_VERSION)
            {
                // Version version = parseVersion(pData);
                //log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::HW_VERSION)
            {
                //Version version = parseVersion(pData);
                // log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::RSSI)
            {
                //log_d("rssi: ", parseRssi(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_VOLTAGE)
            {
                //log_d("battery level: %d %", parseBatteryLevel(pData));
                qDebug()<<"battery level: "<<(uint8_t)v[5]<<"%";
                emit batteryLevel((uint8_t)v[5], 2);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_TYPE)
            {
                //log_d("battery type: %d", parseBatteryType(pData));
                qDebug()<<"battery type: "<<(uint8_t)v[5];

            }
            else if (v[3] == (uint8_t)HubPropertyReference::SYSTEM_TYPE_ID)
            {
                //log_d("system type id: %x", parseSystemTypeId(pData));

            }

            break;
        }

        }

    }
}
void BLEDevice::updateData4(const QLowEnergyCharacteristic &c, const QByteArray &v)
{
    if (c.uuid() == QBluetoothUuid(QUuid(CONTROLUUID))) {

        switch(v[2]) {

        case (char)MessageType::HUB_ATTACHED_IO:
        {

            uint8_t port = v[3];
            bool isConnected = (v[4] == 1 || v[4] == 2) ? true : false;
            if (isConnected)
            {
                qDebug()<<"port "<< port<< "is connected with device "<<(uint8_t)v[5];

            }
            else
            {
                qDebug()<<"port "<< port<< "is disconnected";

            }
            break;
        }


        case (char)MessageType::HUB_PROPERTIES:
        {
            qDebug()<<"HUB_PROPERTIES ";

            if (v[3] == (uint8_t)HubPropertyReference::ADVERTISING_NAME)
            {
                //  log_d("advertising name: %s", parseHubAdvertisingName(pData).c_str());

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BUTTON)
            {
                //log_d("hub button state: %x", parseHubButton(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::FW_VERSION)
            {
                // Version version = parseVersion(pData);
                //log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::HW_VERSION)
            {
                //Version version = parseVersion(pData);
                // log_d("version: %d-%d-%d (%d)", version.Major, version.Minor, version.Bugfix, version.Build);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::RSSI)
            {
                //log_d("rssi: ", parseRssi(pData));

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_VOLTAGE)
            {
                //log_d("battery level: %d %", parseBatteryLevel(pData));
                qDebug()<<"battery level: "<<(uint8_t)v[5]<<"%";
                emit batteryLevel((uint8_t)v[5], 3);

            }
            else if (v[3] == (uint8_t)HubPropertyReference::BATTERY_TYPE)
            {
                //log_d("battery type: %d", parseBatteryType(pData));
                qDebug()<<"battery type: "<<(uint8_t)v[5];

            }
            else if (v[3] == (uint8_t)HubPropertyReference::SYSTEM_TYPE_ID)
            {
                //log_d("system type id: %x", parseSystemTypeId(pData));

            }

            break;
        }

        }
    }
}

void BLEDevice::updateData5(const QLowEnergyCharacteristic &c, const QByteArray &v)
{
    if (c.uuid() == QBluetoothUuid(QUuid(CONTROLUUID))) {


    }
}

void BLEDevice::writeFrame(quint8 i, QByteArray d)
{

    quint8 size = (d.size() + 2);
    QByteArray frame;
    quint8 header[]={size, 0x00};
    frame =  QByteArray::fromRawData((char*)header, sizeof(header));
    frame.append(d);

    qDebug() << frame.toHex('-');

    if(i < services.size()) {
        writeData(i, frame);
    }
    for(volatile int n=0; n<1000000; n++);
}

void BLEDevice::setPower(quint8 i, qint8 p)
{
    qDebug() << i << p;

    //      (size)
    // DATA: 0x08, 0x00, 0x81, port (0/1), 0x11, 0x51, 0x00, speed (-100 to 100)
    QByteArray senddata;
    uint8_t header[]={0x81, 0x00, 0x11, 0x51, 0x00, static_cast<uint8_t>(p)};
    senddata =  QByteArray::fromRawData((char*)header, sizeof(header));

    writeFrame(i, senddata);
}

void BLEDevice::setLedColor(quint8 i, quint8 r, quint8 g, quint8 b)
{
    uint8_t port = 50; //LED
    QByteArray senddata;
    uint8_t header[]={0x41, port, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
    senddata =  QByteArray::fromRawData((char*)header, sizeof(header));
    writeFrame(i, senddata);
    uint8_t header2[]={0x81, port, 0x11, 0x51, 0x01, static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)};
    senddata =  QByteArray::fromRawData((char*)header2, sizeof(header2));
    writeFrame(i, senddata);
}

void BLEDevice::setBatteryUpdate(quint8 i)
{
    QByteArray senddata;
    uint8_t header[]={0x01, 0x06 , 0x02};
    senddata =  QByteArray::fromRawData((char*)header, sizeof(header));
    writeFrame(i, senddata);
}

void BLEDevice::startAcquisition(bool state)
{

    setLedColor(0, state*255, !state*255, 0);



}

void BLEDevice::saveFile()
{

    //    byte setSoundMode[8] = {0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};
    //        WriteValue(setSoundMode, 8);
    //        byte playSound[6] = {0x81, 0x01, 0x11, 0x51, 0x01, sound};
    //        WriteValue(playSound, 6);


//    uint8_t sound = 3;
//    QByteArray senddata;
//    uint8_t header[]={0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};
//    senddata =  QByteArray::fromRawData((char*)header, sizeof(header));
//    writeFrame(0, senddata);
//    uint8_t header2[]={0x81, 0x01, 0x11, 0x51, 0x01, sound};
//    senddata =  QByteArray::fromRawData((char*)header2, sizeof(header2));
//    writeFrame(0, senddata);

//    byte setToneMode[8] = {0x41, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x01};
//        WriteValue(setToneMode, 8);
//        byte playTone[6] = {0x81, 0x01, 0x11, 0x51, 0x02, number};
//        WriteValue(playTone, 6);


//    uint8_t number = 1;
//    QByteArray senddata;
//    uint8_t header[]={0x41, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x01};
//    senddata =  QByteArray::fromRawData((char*)header, sizeof(header));
//    writeFrame(0, senddata);
//    uint8_t header2[]={0x81, 0x01, 0x11, 0x51, 0x02, number};
//    senddata =  QByteArray::fromRawData((char*)header2, sizeof(header2));
//    writeFrame(0, senddata);

}

void BLEDevice::getStatus(QString status)
{
    //    qDebug()<<status;

    //    if(status == "200") {
    //        emit statusChange("File Created ["+status+"]");
    //    } else {
    //        emit statusChange("Server ERROR! ["+status+"]");
    //    }
}


