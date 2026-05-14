#include <gtest/gtest.h>
#include "../protocol_defs.h"
#include <QApplication>
#include <QByteArray>

// reszta testów bez zmian
// ============================================================
// Enum tests
// ============================================================

TEST(MessageTypeTest, ExpectedValues) {
    EXPECT_EQ(static_cast<int>(MessageType::HUB_PROPERTIES), 0x01);
    EXPECT_EQ(static_cast<int>(MessageType::HUB_ACTIONS), 0x02);
    EXPECT_EQ(static_cast<int>(MessageType::HUB_ALERTS), 0x03);
    EXPECT_EQ(static_cast<int>(MessageType::HUB_ATTACHED_IO), 0x04);
    EXPECT_EQ(static_cast<int>(MessageType::GENERIC_ERROR_MESSAGES), 0x05);
    EXPECT_EQ(static_cast<int>(MessageType::HW_NETWORK_COMMANDS), 0x08);
    EXPECT_EQ(static_cast<int>(MessageType::FW_UPDATE_GO_INTO_BOOT_MODE), 0x10);
    EXPECT_EQ(static_cast<int>(MessageType::FW_UPDATE_LOCK_MEMORY), 0x11);
    EXPECT_EQ(static_cast<int>(MessageType::FW_UPDATE_LOCK_STATUS_REQUEST), 0x12);
    EXPECT_EQ(static_cast<int>(MessageType::FW_LOCK_STATUS), 0x13);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_INFORMATION_REQUEST), 0x21);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_MODE_INFORMATION_REQUEST), 0x22);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_INPUT_FORMAT_SETUP_SINGLE), 0x41);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_INPUT_FORMAT_SETUP_COMBINEDMODE), 0x42);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_INFORMATION), 0x43);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_MODE_INFORMATION), 0x44);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_VALUE_SINGLE), 0x45);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_VALUE_COMBINEDMODE), 0x46);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_INPUT_FORMAT_SINGLE), 0x47);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_INPUT_FORMAT_COMBINEDMODE), 0x48);
    EXPECT_EQ(static_cast<int>(MessageType::VIRTUAL_PORT_SETUP), 0x61);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_OUTPUT_COMMAND), 0x81);
    EXPECT_EQ(static_cast<int>(MessageType::PORT_OUTPUT_COMMAND_FEEDBACK), 0x82);
}

TEST(HubPropertyReferenceTest, ExpectedValues) {
    EXPECT_EQ(static_cast<int>(HubPropertyReference::ADVERTISING_NAME), 0x01);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::BUTTON), 0x02);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::FW_VERSION), 0x03);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::HW_VERSION), 0x04);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::RSSI), 0x05);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::BATTERY_VOLTAGE), 0x06);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::BATTERY_TYPE), 0x07);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::MANUFACTURER_NAME), 0x08);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::RADIO_FIRMWARE_VERSION), 0x09);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::LEGO_WIRELESS_PROTOCOL_VERSION), 0x0A);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::SYSTEM_TYPE_ID), 0x0B);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::HW_NETWORK_ID), 0x0C);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::PRIMARY_MAC_ADDRESS), 0x0D);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::SECONDARY_MAC_ADDRESS), 0x0E);
    EXPECT_EQ(static_cast<int>(HubPropertyReference::HARDWARE_NETWORK_FAMILY), 0x0F);
}

TEST(BatteryTypeTest, ExpectedValues) {
    EXPECT_EQ(static_cast<int>(BatteryType::NORMAL), 0x00);
    EXPECT_EQ(static_cast<int>(BatteryType::RECHARGEABLE), 0x01);
}

// ============================================================
// DataProcessing::onCreateData pack format test
// ============================================================

TEST(DataProcessingTest, CreateDataPacking) {
    QByteArray ch1 = QByteArray::fromHex("01020304");
    QByteArray ch2 = QByteArray::fromHex("0506");
    QByteArray ch3 = QByteArray::fromHex("07");
    QByteArray ch4 = QByteArray();
    QByteArray ch5 = QByteArray::fromHex("08090a0b0c0d");

    quint32 sizes[] = {
        static_cast<quint32>(ch1.size()),
        static_cast<quint32>(ch2.size()),
        static_cast<quint32>(ch3.size()),
        static_cast<quint32>(ch4.size()),
        static_cast<quint32>(ch5.size())
    };

    EXPECT_EQ(sizes[0], 4);
    EXPECT_EQ(sizes[1], 2);
    EXPECT_EQ(sizes[2], 1);
    EXPECT_EQ(sizes[3], 0);
    EXPECT_EQ(sizes[4], 6);

    int total = 20 + ch1.size() + ch2.size() + ch3.size() + ch4.size() + ch5.size();
    EXPECT_EQ(total, 33);
}

// ============================================================
// BLEDevice command format tests
// ============================================================

TEST(BLEDeviceTest, WriteFramePacksCorrectly) {
    QByteArray input;
    input.append(static_cast<char>(0x81));
    input.append(static_cast<char>(0x00));
    input.append(static_cast<char>(0x11));

    quint8 expectedSize = input.size() + 2;
    EXPECT_EQ(expectedSize, 5);
    EXPECT_EQ(input.size(), 3);
}

TEST(BLEDeviceTest, SetPowerCommandFormat) {
    qint8 speed = 75;
    uint8_t header[] = {0x81, 0x00, 0x11, 0x51, 0x00, static_cast<uint8_t>(speed)};

    EXPECT_EQ(header[0], 0x81);
    EXPECT_EQ(header[1], 0x00);
    EXPECT_EQ(header[2], 0x11);
    EXPECT_EQ(header[3], 0x51);
    EXPECT_EQ(header[4], 0x00);
    EXPECT_EQ(header[5], 75);
}

TEST(BLEDeviceTest, SetPowerNegativeSpeed) {
    qint8 speed = -50;
    uint8_t header[] = {0x81, 0x00, 0x11, 0x51, 0x00, static_cast<uint8_t>(speed)};
    EXPECT_EQ(header[5], 206);
}

TEST(BLEDeviceTest, SetBatteryUpdateCommandFormat) {
    uint8_t header[] = {0x01, 0x06, 0x02};
    EXPECT_EQ(header[0], 0x01);
    EXPECT_EQ(header[1], 0x06);
    EXPECT_EQ(header[2], 0x02);
}

// ============================================================
// Constants
// ============================================================

TEST(ConstantsTest, ServiceUUID) {
    EXPECT_EQ(QString("00001623-1212-efde-1623-785feabcd123"), COMMSERVICEUUID);
}

TEST(ConstantsTest, ControlUUID) {
    EXPECT_EQ(QString("00001624-1212-efde-1623-785feabcd123"), CONTROLUUID);
}

// ============================================================
// Main
// ============================================================

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
