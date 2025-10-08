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

// Test fixtures
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

// Test initial state
void test_gate_monitor_initial_state() {
    TEST_ASSERT_FALSE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(IDLE, gateMonitor->getCurrentOperation());
    TEST_ASSERT_FALSE(gateMonitor->isAutoCloseEnabled());
    TEST_ASSERT_FALSE(gateMonitor->isAlertActive());
}

// Test starting an operation
void test_gate_monitor_start_operation() {
#ifdef UNIT_TEST
    setMockMillis(1000);
    
    gateMonitor->startOperation(OPENING, OPEN);
    
    TEST_ASSERT_TRUE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(OPENING, gateMonitor->getCurrentOperation());
    TEST_ASSERT_FALSE(gateMonitor->isAlertActive());
#endif
}

// Test operation completion
void test_gate_monitor_operation_completion() {
#ifdef UNIT_TEST
    setMockMillis(1000);
    gateMonitor->startOperation(OPENING, OPEN);
    
    // Simulate gate reaching open position
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    
    gateMonitor->update();
    
    // Operation should complete
    TEST_ASSERT_FALSE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(IDLE, gateMonitor->getCurrentOperation());
#endif
}

// Test operation timeout
void test_gate_monitor_operation_timeout() {
#ifdef UNIT_TEST
    setMockMillis(1000);
    gateMonitor->startOperation(OPENING, OPEN);
    
    // Advance time beyond timeout
    setMockMillis(1000 + OPENING_TIMEOUT + 1000);
    
    // Keep gate in unknown state
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, HIGH);
    
    gateMonitor->update();
    
    // Alert should be triggered
    TEST_ASSERT_TRUE(gateMonitor->isAlertActive());
#endif
}

// Test auto-close activation
void test_gate_monitor_auto_close_activation() {
#ifdef UNIT_TEST
    setMockMillis(1000);
    
    // Simulate gate becoming open
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    
    gateMonitor->update();
    
    // Auto-close should be enabled
    TEST_ASSERT_TRUE(gateMonitor->isAutoCloseEnabled());
#endif
}

// Test auto-close trigger
void test_gate_monitor_auto_close_trigger() {
#ifdef UNIT_TEST
    setMockMillis(1000);
    
    // Enable auto-close
    gateMonitor->enableAutoClose();
    
    // Simulate gate is open
    setMockPinValue(SENSOR_CLOSED_PIN, HIGH);
    setMockPinValue(SENSOR_OPEN_PIN, LOW);
    
    // Advance time beyond auto-close delay
    setMockMillis(1000 + AUTO_CLOSE_DELAY + 1000);
    
    gateMonitor->update();
    
    // Should start closing operation
    TEST_ASSERT_TRUE(gateMonitor->isOperationInProgress());
    TEST_ASSERT_EQUAL(CLOSING, gateMonitor->getCurrentOperation());
    TEST_ASSERT_FALSE(gateMonitor->isAutoCloseEnabled());
#endif
}

// Test remaining time calculations
void test_gate_monitor_remaining_times() {
#ifdef UNIT_TEST
    setMockMillis(1000);
    gateMonitor->startOperation(OPENING, OPEN);
    
    // Advance time partially
    setMockMillis(5000);
    
    unsigned long elapsed = gateMonitor->getOperationElapsedTime();
    unsigned long remaining = gateMonitor->getOperationRemainingTime();
    
    TEST_ASSERT_EQUAL(4000, elapsed);
    TEST_ASSERT_EQUAL(OPENING_TIMEOUT - 4000, remaining);
#endif
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_gate_monitor_initial_state);
    RUN_TEST(test_gate_monitor_start_operation);
    RUN_TEST(test_gate_monitor_operation_completion);
    RUN_TEST(test_gate_monitor_operation_timeout);
    RUN_TEST(test_gate_monitor_auto_close_activation);
    RUN_TEST(test_gate_monitor_auto_close_trigger);
    RUN_TEST(test_gate_monitor_remaining_times);
    
    return UNITY_END();
}