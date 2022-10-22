#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpPart>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

class DataProcessing : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessing(QObject *parent = nullptr);


private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QString answer;
    QString server;
    QVector<uint8_t> control;
    uint deviceid=0;

private slots:
    void managerFinished(QNetworkReply *reply);
    void onRequest();

public slots:
    void onCreateData(QByteArray &, QByteArray &, QByteArray &, QByteArray &, QByteArray &);
    void onSendValue(QByteArray &);

signals:
    void statusChange(QString);
};


enum struct MessageType
{
  HUB_PROPERTIES = 0x01,
  HUB_ACTIONS = 0x02,
  HUB_ALERTS = 0x03,
  HUB_ATTACHED_IO = 0x04,
  GENERIC_ERROR_MESSAGES = 0x05,
  HW_NETWORK_COMMANDS = 0x08,
  FW_UPDATE_GO_INTO_BOOT_MODE = 0x10,
  FW_UPDATE_LOCK_MEMORY = 0x11,
  FW_UPDATE_LOCK_STATUS_REQUEST = 0x12,
  FW_LOCK_STATUS = 0x13,
  PORT_INFORMATION_REQUEST = 0x21,
  PORT_MODE_INFORMATION_REQUEST = 0x22,
  PORT_INPUT_FORMAT_SETUP_SINGLE = 0x41,
  PORT_INPUT_FORMAT_SETUP_COMBINEDMODE = 0x42,
  PORT_INFORMATION = 0x43,
  PORT_MODE_INFORMATION = 0x44,
  PORT_VALUE_SINGLE = 0x45,
  PORT_VALUE_COMBINEDMODE = 0x46,
  PORT_INPUT_FORMAT_SINGLE = 0x47,
  PORT_INPUT_FORMAT_COMBINEDMODE = 0x48,
  VIRTUAL_PORT_SETUP = 0x61,
  PORT_OUTPUT_COMMAND = 0x81,
  PORT_OUTPUT_COMMAND_FEEDBACK = 0x82,
};

enum struct HubPropertyReference
{
  ADVERTISING_NAME = 0x01,
  BUTTON = 0x02,
  FW_VERSION = 0x03,
  HW_VERSION = 0x04,
  RSSI = 0x05,
  BATTERY_VOLTAGE = 0x06,
  BATTERY_TYPE = 0x07,
  MANUFACTURER_NAME = 0x08,
  RADIO_FIRMWARE_VERSION = 0x09,
  LEGO_WIRELESS_PROTOCOL_VERSION = 0x0A,
  SYSTEM_TYPE_ID = 0x0B,
  HW_NETWORK_ID = 0x0C,
  PRIMARY_MAC_ADDRESS = 0x0D,
  SECONDARY_MAC_ADDRESS = 0x0E,
  HARDWARE_NETWORK_FAMILY = 0x0F,
};

enum struct BatteryType
{
  NORMAL = 0x00,
  RECHARGEABLE = 0x01
};

#endif // DATAPROCESSING_H
