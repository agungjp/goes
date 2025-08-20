#include <unity.h>
#include <vector>
#include <cstdint>
#include "iec104/transport/IEC104Communicator.h"
#include "comm/CommInterface.h"

// Mock CommInterface for host (no Arduino)
class MockCommHost : public CommInterface {
public:
    std::vector<uint8_t> writes;
    int available() override { return 0; }
    int read() override { return -1; }
    void write(const uint8_t* data, size_t len) override { writes.assign(data, data + len); }
    void sendData(const uint8_t* data, size_t len) override { write(data, len); }
    void setupConnection() override {}
    void restart() override {}
    void flush() override {}
};

static MockCommHost mock;
static IEC104Communicator* comm = nullptr;

void setUp() {}
void tearDown() { if (comm) { delete comm; comm = nullptr; } }

void test_u_startdt_con() {
    comm = new IEC104Communicator(&mock);
    const uint8_t startdt_act[] = {0x68, 0x04, 0x07, 0x00, 0x00, 0x00};
    comm->handleUFrame(startdt_act, sizeof(startdt_act));
    const uint8_t expected[]    = {0x68, 0x04, 0x0B, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT32(sizeof(expected), mock.writes.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, mock.writes.data(), sizeof(expected));
}

void test_u_testfr_con() {
    comm = new IEC104Communicator(&mock);
    const uint8_t testfr_act[]  = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};
    comm->handleUFrame(testfr_act, sizeof(testfr_act));
    const uint8_t expected[]    = {0x68, 0x04, 0x83, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT32(sizeof(expected), mock.writes.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, mock.writes.data(), sizeof(expected));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_u_startdt_con);
    RUN_TEST(test_u_testfr_con);
    return UNITY_END();
}


