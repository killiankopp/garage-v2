#ifdef UNIT_TEST
#include "../mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#endif

// Include our components
#include "../src/components/Config.h"
#include "../src/components/GateTypes.h"
#include "../src/components/GateController.h"

#include <unity.h>

// Test fixtures
GateController* gateController;

void setUp(void) {
#ifdef UNIT_TEST
    resetMockState();
#endif
    gateController = new GateController();
}

void tearDown(void) {
    delete gateController;
    gateController = nullptr;
}

// Test GateController initialization
void test_gate_controller_initialization() {
    gateController->begin();
    
#ifdef UNIT_TEST
    // Verify pins were configured correctly
    TEST_ASSERT_EQUAL(OUTPUT, pinModes[RELAY_1_PIN]);
    TEST_ASSERT_EQUAL(INPUT_PULLUP, pinModes[SENSOR_CLOSED_PIN]);
    TEST_ASSERT_EQUAL(INPUT_PULLUP, pinModes[SENSOR_OPEN_PIN]);
    
    // Verify relay is initially off
    TEST_ASSERT_EQUAL(LOW, pinValues[RELAY_1_PIN]);
#endif
}

// Test sensor reading when gate is closed
void test_gate_controller_read_state_closed() {
#ifdef UNIT_TEST
    gateController->begin();
    
    // Simulate closed sensor active (LOW because of pull-up logic)
    setMockPinValue(SENSOR_CLOSED_PIN, LOW);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    
    GateState state = gateController->readState();
    TEST_ASSERT_EQUAL(CLOSED, state);
    
    TEST_ASSERT_TRUE(gateController->isClosedSensorActive());
    TEST_ASSERT_FALSE(gateController->isOpenSensorActive());
#endif
}

// Test sensor reading when gate is open
void test_gate_controller_read_state_open() {
#ifdef UNIT_TEST
    gateController->begin();
    
    // Simulate open sensor active (LOW because of pull-up logic)
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    
    GateState state = gateController->readState();
    TEST_ASSERT_EQUAL(OPEN, state);
    
    TEST_ASSERT_FALSE(gateController->isClosedSensorActive());
    TEST_ASSERT_TRUE(gateController->isOpenSensorActive());
#endif
}

// Test sensor reading when gate is in unknown state
void test_gate_controller_read_state_unknown() {
#ifdef UNIT_TEST
    gateController->begin();
    
    // Simulate both sensors active (should not happen in reality)
    setMockPinValue(SENSOR_CLOSED_PIN, LOW);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    
    GateState state = gateController->readState();
    TEST_ASSERT_EQUAL(UNKNOWN, state);
    
    // Test neither sensor active
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    
    state = gateController->readState();
    TEST_ASSERT_EQUAL(UNKNOWN, state);
#endif
}

// Test relay trigger
void test_gate_controller_trigger_relay() {
#ifdef UNIT_TEST
    gateController->begin();
    
    // Initial state should be LOW
    TEST_ASSERT_EQUAL(LOW, pinValues[RELAY_1_PIN]);
    
    unsigned long initialMillis = mockMillis;
    gateController->triggerRelay();
    
    // After trigger, time should have advanced and relay should be back to LOW
    TEST_ASSERT_EQUAL(LOW, pinValues[RELAY_1_PIN]);
    TEST_ASSERT_EQUAL(initialMillis + 1000, mockMillis);
#endif
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_gate_controller_initialization);
    RUN_TEST(test_gate_controller_read_state_closed);
    RUN_TEST(test_gate_controller_read_state_open);
    RUN_TEST(test_gate_controller_read_state_unknown);
    RUN_TEST(test_gate_controller_trigger_relay);
    
    return UNITY_END();
}