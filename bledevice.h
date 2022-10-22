#ifndef BLEDEVICE_H
#define BLEDEVICE_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QStandardPaths>
#include <QFile>
#include <QFileDialog>

#include "deviceinfo.h"
#include "dataprocessing.h"
#include "stdint.h"

#define COMMSERVICEUUID    "00001623-1212-efde-1623-785feabcd123"
#define CONTROLUUID        "00001624-1212-efde-1623-785feabcd123"
//#define CONFIGUUID        "d44bc439-abfd-45a2-b575-925416129600"

//#define DEVICENAME1      "Smart Hub 1"
//#define DEVICENAME2      "Smart Hub 2"

//#define COMMSERVICEUUID "00001623-1212-efde-1623-785feabcd123"
//#define CXUUID          "00001624-1212-efde-1623-785feabcd123"


class BLEDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList deviceListModel READ deviceListModel WRITE setDeviceListModel RESET resetDeviceListModel NOTIFY deviceListModelChanged)

public:
    explicit BLEDevice(QObject *parent = nullptr);
    ~BLEDevice();

    QStringList deviceListModel();

private:
    DeviceInfo currentDevice;
    QBluetoothDeviceDiscoveryAgent *DiscoveryAgent;
    QList<QObject*> qlDevices;
    uint32_t controllerIndex=0;
    QList<QLowEnergyController*> controllers;
    QList<QLowEnergyService*> services;
    QLowEnergyDescriptor notificationDesc;
    QList<bool> bFoundSensorService;
    QStringList m_foundDevices;
    QStringList m_deviceListModel;

    quint8 accRange;

    bool bRun=0;

    DataProcessing dataProcessing;

private slots:
    /* Slots for QBluetothDeviceDiscoveryAgent */
    void addDevice(const QBluetoothDeviceInfo &);
    void scanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    /* Slots for QLowEnergyController */
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error);
    void deviceConnected();
    void deviceDisconnected();

    /* Slotes for QLowEnergyService */
    void serviceStateChanged(QLowEnergyService::ServiceState);
    void updateData(const QLowEnergyCharacteristic &, const QByteArray &);
    void updateData2(const QLowEnergyCharacteristic &, const QByteArray &);
    void updateData3(const QLowEnergyCharacteristic &, const QByteArray &);
    void updateData4(const QLowEnergyCharacteristic &, const QByteArray &);
    void updateData5(const QLowEnergyCharacteristic &, const QByteArray &);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &, const QByteArray &);

public slots:
    /* Slots for user */
    void startScan();
    void startConnect(int);
    void disconnectFromDevice();
    void writeData(quint8, QByteArray);
    void setDeviceListModel(QStringList);
    void resetDeviceListModel();
    void setPower(quint8, qint8);
    void setLedColor(quint8, quint8, quint8, quint8);
    void setBatteryUpdate(quint8);

    void startAcquisition(bool);
    void saveFile();
    void getStatus(QString);
    void writeFrame(quint8, QByteArray);

signals:
    /* Signals for user */
    void newData(QVector<qreal>, quint8);
    void scanningFinished();
    void connectionStart();
    void connectionEnd();
    void deviceListModelChanged(QStringList);
    void devicesReady();
    void batteryLevel(quint8, quint8);
    void statusChange(QString);

};

#endif // BLEDEVICE_H
