#ifdef UNIT_TEST
#include "../mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#endif

#include "../src/components/Config.h"
#include "../src/components/GateTypes.h"
#include "../src/components/GateController.h"
#include "../src/components/GateMonitor.h"

#include <unity.h>

// Integration test fixtures
GateController* gateController;
GateMonitor* gateMonitor;

void setUp(void) {
#ifdef UNIT_TEST
    resetMockState();
#endif
    gateController = new GateController();
    gateMonitor = new GateMonitor(gateController);
    
    gateController->begin();
    gateMonitor->begin();
}

void tearDown(void) {
    delete gateMonitor;
    delete gateController;
    gateMonitor = nullptr;
    gateController = nullptr;
}

// Test complete opening cycle
void test_integration_complete_opening_cycle() {
#ifdef UNIT_TEST
    setMockMillis(0);
    
    // Initial state: gate closed
    setMockPinValue(SENSOR_CLOSED_PIN, LOW);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    gateMonitor->update();
    
    TEST_ASSERT_EQUAL(CLOSED, gateController->readState());
    TEST_ASSERT_FALSE(gateMonitor->isAutoCloseEnabled());
    
    // Start opening operation
    gateController->triggerRelay();
    gateMonitor->startOperation(OPENING, OPEN);
    
    TEST_ASSERT_TRUE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(OPENING, gateMonitor->getCurrentOperation());
    
    // Advance time (simulate gate moving)
    setMockMillis(5000);
    
    // Gate reaches open position
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    gateMonitor->update();
    
    // Operation should complete and auto-close should activate
    TEST_ASSERT_FALSE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(OPEN, gateController->readState());
    TEST_ASSERT_TRUE(gateMonitor->isAutoCloseEnabled());
#endif
}

// Test complete closing cycle
void test_integration_complete_closing_cycle() {
#ifdef UNIT_TEST
    setMockMillis(0);
    
    // Initial state: gate open with auto-close enabled
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    gateMonitor->enableAutoClose();
    gateMonitor->update();
    
    TEST_ASSERT_EQUAL(OPEN, gateController->readState());
    TEST_ASSERT_TRUE(gateMonitor->isAutoCloseEnabled());
    
    // Start closing operation
    gateController->triggerRelay();
    gateMonitor->startOperation(CLOSING, CLOSED);
    
    TEST_ASSERT_TRUE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(CLOSING, gateMonitor->getCurrentOperation());
    
    // Advance time (simulate gate moving)
    setMockMillis(8000);
    
    // Gate reaches closed position
    setMockPinValue(SENSOR_CLOSED_PIN, LOW);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    gateMonitor->update();
    
    // Operation should complete and auto-close should be disabled
    TEST_ASSERT_FALSE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(CLOSED, gateController->readState());
    TEST_ASSERT_FALSE(gateMonitor->isAutoCloseEnabled());
#endif
}

// Test auto-close full cycle
void test_integration_auto_close_full_cycle() {
#ifdef UNIT_TEST
    setMockMillis(0);
    
    // Gate becomes open
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    gateMonitor->update();
    
    TEST_ASSERT_TRUE(gateMonitor->isAutoCloseEnabled());
    
    // Wait for auto-close trigger
    setMockMillis(AUTO_CLOSE_DELAY + 1000);
    gateMonitor->update();
    
    // Should start closing automatically
    TEST_ASSERT_TRUE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(CLOSING, gateMonitor->getCurrentOperation());
    TEST_ASSERT_FALSE(gateMonitor->isAutoCloseEnabled());
    
    // Gate completes closing
    setMockPinValue(SENSOR_CLOSED_PIN, LOW);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    gateMonitor->update();
    
    TEST_ASSERT_FALSE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(CLOSED, gateController->readState());
#endif
}

// Test error scenarios
void test_integration_timeout_scenario() {
#ifdef UNIT_TEST
    setMockMillis(0);
    
    // Start opening operation
    gateMonitor->startOperation(OPENING, OPEN);
    
    // Gate gets stuck (sensors don't change)
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    
    // Wait beyond timeout
    setMockMillis(OPENING_TIMEOUT + 5000);
    gateMonitor->update();
    
    // Alert should be triggered
    TEST_ASSERT_TRUE(gateMonitor->isAlertActive());
    TEST_ASSERT_TRUE(gateMonitor->isOperationInProgress()); // Operation continues
#endif
}

// Test state consistency
void test_integration_state_consistency() {
#ifdef UNIT_TEST
    setMockMillis(0);
    
    // Test all possible sensor combinations
    struct TestCase {
        int closedPin;
        int openPin;
        GateState expectedState;
    } testCases[] = {
        {LOW, HIGH, CLOSED},
        {HIGH, LOW, OPEN},
        {HIGH, HIGH, UNKNOWN},
        {LOW, LOW, UNKNOWN}
    };
    
    for (auto& testCase : testCases) {
        setMockPinValue(SENSOR_CLOSED_PIN, testCase.closedPin);
        setMockPinValue(SENSOR_OPEN_PIN, testCase.openPin);
        
        GateState state = gateController->readState();
        TEST_ASSERT_EQUAL(testCase.expectedState, state);
        
        gateMonitor->update();
        // Monitor should handle all states gracefully
    }
#endif
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_integration_complete_opening_cycle);
    RUN_TEST(test_integration_complete_closing_cycle);
    RUN_TEST(test_integration_auto_close_full_cycle);
    RUN_TEST(test_integration_timeout_scenario);
    RUN_TEST(test_integration_state_consistency);
    
    return UNITY_END();
}