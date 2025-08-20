#include <Arduino.h>
#include <unity.h>
#include <vector>
#include "iec104/transport/IEC104Communicator.h"
#include "comm/CommInterface.h"

class MockComm : public CommInterface {
public:
    std::vector<uint8_t> lastWrite;

    void setupConnection() override {}
    void restart() override {}
    void sendData(const uint8_t* data, size_t len) override { write(data, len); }
    int available() override { return 0; }
    int read() override { return -1; }
    void write(const uint8_t* data, size_t len) override {
        lastWrite.assign(data, data + len);
    }
    void flush() override {}
};

static MockComm mockComm;
static IEC104Communicator* comm = nullptr;

void setUp() {}
void tearDown() {}

void test_startdt_act_response() {
    comm = new IEC104Communicator(&mockComm);

    // U-Frame STARTDT_ACT: 0x68 0x04 0x07 0x00 0x00 0x00
    const uint8_t frame[] = {0x68, 0x04, 0x07, 0x00, 0x00, 0x00};
    comm->handleUFrame(frame, sizeof(frame));

    const uint8_t expected[] = {0x68, 0x04, 0x0B, 0x00, 0x00, 0x00}; // STARTDT_CON
    TEST_ASSERT_EQUAL_UINT32(sizeof(expected), mockComm.lastWrite.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, mockComm.lastWrite.data(), sizeof(expected));

    delete comm;
}

void test_testfr_act_response() {
    comm = new IEC104Communicator(&mockComm);

    // U-Frame TESTFR_ACT: 0x68 0x04 0x43 0x00 0x00 0x00
    const uint8_t frame[] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};
    comm->handleUFrame(frame, sizeof(frame));

    const uint8_t expected[] = {0x68, 0x04, 0x83, 0x00, 0x00, 0x00}; // TESTFR_CON
    TEST_ASSERT_EQUAL_UINT32(sizeof(expected), mockComm.lastWrite.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, mockComm.lastWrite.data(), sizeof(expected));

    delete comm;
}

void setup() {
    delay(2000); // give time for serial
    UNITY_BEGIN();
    RUN_TEST(test_startdt_act_response);
    RUN_TEST(test_testfr_act_response);
    UNITY_END();
}

void loop() {}


