#include <iostream>
#include <cassert>
#include <string>

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

// Test functions
void test_basic_functionality() {
    std::cout << "\n🔍 Test: Basic Functionality" << std::endl;
    TEST_ASSERT_EQUAL(1, 1);
    TEST_ASSERT_TRUE(true);
    TEST_ASSERT_FALSE(false);
}

void test_math_operations() {
    std::cout << "\n🔍 Test: Math Operations" << std::endl;
    int a = 5;
    int b = 3;
    TEST_ASSERT_EQUAL(8, a + b);
    TEST_ASSERT_EQUAL(2, a - b);
    TEST_ASSERT_EQUAL(15, a * b);
}

void test_string_operations() {
    std::cout << "\n🔍 Test: String Operations" << std::endl;
    std::string hello = "Hello";
    std::string world = "World";
    std::string result = hello + " " + world;
    TEST_ASSERT_TRUE(result == "Hello World");
}

int main() {
    std::cout << "🧪 Tests de Base - Démarrage" << std::endl;
    
    test_basic_functionality();
    test_math_operations();
    test_string_operations();
    
    std::cout << "\n📊 Résultats:" << std::endl;
    std::cout << "Tests exécutés: " << tests_run << std::endl;
    std::cout << "Tests réussis: " << tests_passed << std::endl;
    std::cout << "Tests échoués: " << (tests_run - tests_passed) << std::endl;
    
    if (tests_passed == tests_run) {
        std::cout << "🎉 Tous les tests de base sont passés !" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Certains tests ont échoué" << std::endl;
        return 1;
    }
}