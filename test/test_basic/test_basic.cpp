#include <unity.h>
#include <iostream>

// Test simple pour vérifier la configuration
void test_basic_functionality() {
    TEST_ASSERT_EQUAL(1, 1);
    TEST_ASSERT_TRUE(true);
    TEST_ASSERT_FALSE(false);
}

void test_math_operations() {
    int a = 5;
    int b = 3;
    TEST_ASSERT_EQUAL(8, a + b);
    TEST_ASSERT_EQUAL(2, a - b);
    TEST_ASSERT_EQUAL(15, a * b);
}

void test_string_operations() {
    std::string hello = "Hello";
    std::string world = "World";
    std::string result = hello + " " + world;
    TEST_ASSERT_EQUAL_STRING("Hello World", result.c_str());
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_basic_functionality);
    RUN_TEST(test_math_operations);
    RUN_TEST(test_string_operations);
    
    return UNITY_END();
}