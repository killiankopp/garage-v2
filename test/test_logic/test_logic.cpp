#include <unity.h>

// Include only the enum definitions for testing
enum GateState {
    CLOSED,
    OPEN,
    UNKNOWN
};

enum OperationState {
    IDLE,
    OPENING,
    CLOSING
};

// Test logic functions that can be extracted from our classes
GateState determineGateState(bool sensorClosed, bool sensorOpen) {
    if (sensorClosed && !sensorOpen) {
        return CLOSED;
    } else if (!sensorClosed && sensorOpen) {
        return OPEN;
    } else {
        return UNKNOWN;
    }
}

bool shouldTriggerTimeout(unsigned long elapsed, OperationState operation, 
                         unsigned long openingTimeout, unsigned long closingTimeout) {
    if (operation == IDLE) return false;
    
    unsigned long timeout = (operation == OPENING) ? openingTimeout : closingTimeout;
    return elapsed >= timeout;
}

bool shouldTriggerAutoClose(unsigned long elapsed, bool autoCloseEnabled, 
                           GateState state, OperationState operation, 
                           unsigned long autoCloseDelay) {
    return autoCloseEnabled && 
           state == OPEN && 
           operation == IDLE && 
           elapsed >= autoCloseDelay;
}

// Tests for gate state logic
void test_gate_state_closed() {
    GateState state = determineGateState(true, false);
    TEST_ASSERT_EQUAL(CLOSED, state);
}

void test_gate_state_open() {
    GateState state = determineGateState(false, true);
    TEST_ASSERT_EQUAL(OPEN, state);
}

void test_gate_state_unknown_both_sensors() {
    GateState state = determineGateState(true, true);
    TEST_ASSERT_EQUAL(UNKNOWN, state);
}

void test_gate_state_unknown_no_sensors() {
    GateState state = determineGateState(false, false);
    TEST_ASSERT_EQUAL(UNKNOWN, state);
}

// Tests for timeout logic
void test_timeout_opening_within_limit() {
    bool timeout = shouldTriggerTimeout(10000, OPENING, 15000, 20000);
    TEST_ASSERT_FALSE(timeout);
}

void test_timeout_opening_exceeded() {
    bool timeout = shouldTriggerTimeout(16000, OPENING, 15000, 20000);
    TEST_ASSERT_TRUE(timeout);
}

void test_timeout_closing_within_limit() {
    bool timeout = shouldTriggerTimeout(15000, CLOSING, 15000, 20000);
    TEST_ASSERT_FALSE(timeout);
}

void test_timeout_closing_exceeded() {
    bool timeout = shouldTriggerTimeout(21000, CLOSING, 15000, 20000);
    TEST_ASSERT_TRUE(timeout);
}

void test_timeout_idle_never_triggers() {
    bool timeout = shouldTriggerTimeout(999999, IDLE, 15000, 20000);
    TEST_ASSERT_FALSE(timeout);
}

// Tests for auto-close logic
void test_auto_close_should_trigger() {
    bool shouldTrigger = shouldTriggerAutoClose(200000, true, OPEN, IDLE, 180000);
    TEST_ASSERT_TRUE(shouldTrigger);
}

void test_auto_close_not_enabled() {
    bool shouldTrigger = shouldTriggerAutoClose(200000, false, OPEN, IDLE, 180000);
    TEST_ASSERT_FALSE(shouldTrigger);
}

void test_auto_close_gate_not_open() {
    bool shouldTrigger = shouldTriggerAutoClose(200000, true, CLOSED, IDLE, 180000);
    TEST_ASSERT_FALSE(shouldTrigger);
}

void test_auto_close_operation_in_progress() {
    bool shouldTrigger = shouldTriggerAutoClose(200000, true, OPEN, OPENING, 180000);
    TEST_ASSERT_FALSE(shouldTrigger);
}

void test_auto_close_time_not_elapsed() {
    bool shouldTrigger = shouldTriggerAutoClose(100000, true, OPEN, IDLE, 180000);
    TEST_ASSERT_FALSE(shouldTrigger);
}

int main() {
    UNITY_BEGIN();
    
    // Gate state tests
    RUN_TEST(test_gate_state_closed);
    RUN_TEST(test_gate_state_open);
    RUN_TEST(test_gate_state_unknown_both_sensors);
    RUN_TEST(test_gate_state_unknown_no_sensors);
    
    // Timeout logic tests
    RUN_TEST(test_timeout_opening_within_limit);
    RUN_TEST(test_timeout_opening_exceeded);
    RUN_TEST(test_timeout_closing_within_limit);
    RUN_TEST(test_timeout_closing_exceeded);
    RUN_TEST(test_timeout_idle_never_triggers);
    
    // Auto-close logic tests
    RUN_TEST(test_auto_close_should_trigger);
    RUN_TEST(test_auto_close_not_enabled);
    RUN_TEST(test_auto_close_gate_not_open);
    RUN_TEST(test_auto_close_operation_in_progress);
    RUN_TEST(test_auto_close_time_not_elapsed);
    
    return UNITY_END();
}