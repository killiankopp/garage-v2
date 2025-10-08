#include <iostream>
#include <cassert>

// Simple test framework
int tests_run = 0;
int tests_passed = 0;

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        tests_run++; \
        if ((expected) == (actual)) { \
            tests_passed++; \
            std::cout << "✅ PASS: " << #actual << " == " << (expected) << std::endl; \
        } else { \
            std::cout << "❌ FAIL: " << #actual << " = " << (actual) << ", expected " << (expected) << std::endl; \
        } \
    } while(0)

#define TEST_ASSERT_TRUE(condition) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            std::cout << "✅ PASS: " << #condition << " is true" << std::endl; \
        } else { \
            std::cout << "❌ FAIL: " << #condition << " is false" << std::endl; \
        } \
    } while(0)

#define TEST_ASSERT_FALSE(condition) \
    do { \
        tests_run++; \
        if (!(condition)) { \
            tests_passed++; \
            std::cout << "✅ PASS: " << #condition << " is false" << std::endl; \
        } else { \
            std::cout << "❌ FAIL: " << #condition << " is true" << std::endl; \
        } \
    } while(0)

// Gate state definitions (from our project)
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

// Business logic functions to test
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

// Test functions
void test_gate_state_logic() {
    std::cout << "\n🔍 Test: Gate State Logic" << std::endl;
    
    // Test closed state
    TEST_ASSERT_EQUAL(CLOSED, determineGateState(true, false));
    
    // Test open state
    TEST_ASSERT_EQUAL(OPEN, determineGateState(false, true));
    
    // Test unknown states
    TEST_ASSERT_EQUAL(UNKNOWN, determineGateState(true, true));
    TEST_ASSERT_EQUAL(UNKNOWN, determineGateState(false, false));
}

void test_timeout_logic() {
    std::cout << "\n🔍 Test: Timeout Logic" << std::endl;
    
    const unsigned long OPENING_TIMEOUT = 15000;
    const unsigned long CLOSING_TIMEOUT = 20000;
    
    // Test opening timeouts
    TEST_ASSERT_FALSE(shouldTriggerTimeout(10000, OPENING, OPENING_TIMEOUT, CLOSING_TIMEOUT));
    TEST_ASSERT_TRUE(shouldTriggerTimeout(16000, OPENING, OPENING_TIMEOUT, CLOSING_TIMEOUT));
    
    // Test closing timeouts
    TEST_ASSERT_FALSE(shouldTriggerTimeout(15000, CLOSING, OPENING_TIMEOUT, CLOSING_TIMEOUT));
    TEST_ASSERT_TRUE(shouldTriggerTimeout(21000, CLOSING, OPENING_TIMEOUT, CLOSING_TIMEOUT));
    
    // Test idle never triggers timeout
    TEST_ASSERT_FALSE(shouldTriggerTimeout(999999, IDLE, OPENING_TIMEOUT, CLOSING_TIMEOUT));
}

void test_auto_close_logic() {
    std::cout << "\n🔍 Test: Auto-close Logic" << std::endl;
    
    const unsigned long AUTO_CLOSE_DELAY = 180000;
    
    // Should trigger when all conditions are met
    TEST_ASSERT_TRUE(shouldTriggerAutoClose(200000, true, OPEN, IDLE, AUTO_CLOSE_DELAY));
    
    // Should not trigger when disabled
    TEST_ASSERT_FALSE(shouldTriggerAutoClose(200000, false, OPEN, IDLE, AUTO_CLOSE_DELAY));
    
    // Should not trigger when gate is not open
    TEST_ASSERT_FALSE(shouldTriggerAutoClose(200000, true, CLOSED, IDLE, AUTO_CLOSE_DELAY));
    
    // Should not trigger when operation is in progress
    TEST_ASSERT_FALSE(shouldTriggerAutoClose(200000, true, OPEN, OPENING, AUTO_CLOSE_DELAY));
    
    // Should not trigger when time has not elapsed
    TEST_ASSERT_FALSE(shouldTriggerAutoClose(100000, true, OPEN, IDLE, AUTO_CLOSE_DELAY));
}

void test_edge_cases() {
    std::cout << "\n🔍 Test: Edge Cases" << std::endl;
    
    // Zero timeouts
    TEST_ASSERT_TRUE(shouldTriggerTimeout(1, OPENING, 0, 0));
    TEST_ASSERT_TRUE(shouldTriggerTimeout(1, CLOSING, 0, 0));
    
    // Exact timeout values
    TEST_ASSERT_TRUE(shouldTriggerTimeout(15000, OPENING, 15000, 20000));
    TEST_ASSERT_TRUE(shouldTriggerTimeout(20000, CLOSING, 15000, 20000));
    
    // Zero auto-close delay
    TEST_ASSERT_TRUE(shouldTriggerAutoClose(1, true, OPEN, IDLE, 0));
}

int main() {
    std::cout << "🧪 Tests de Logique Métier - Démarrage" << std::endl;
    
    test_gate_state_logic();
    test_timeout_logic();
    test_auto_close_logic();
    test_edge_cases();
    
    std::cout << "\n📊 Résultats:" << std::endl;
    std::cout << "Tests exécutés: " << tests_run << std::endl;
    std::cout << "Tests réussis: " << tests_passed << std::endl;
    std::cout << "Tests échoués: " << (tests_run - tests_passed) << std::endl;
    
    if (tests_passed == tests_run) {
        std::cout << "🎉 Tous les tests de logique sont passés !" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Certains tests ont échoué" << std::endl;
        return 1;
    }
}