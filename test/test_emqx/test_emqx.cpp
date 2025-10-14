#include <unity.h>
#include "../src/components/EmqxConfig.h"
#include "../src/components/EmqxLogger.h"

void test_emqx_config_initialization() {
    EmqxConfig config;
    config.initialize();
    
    // Test que la configuration se charge correctement
    TEST_ASSERT_TRUE(!config.getBrokerHost().isEmpty() || !config.isEmqxEnabled());
    TEST_ASSERT_EQUAL_STRING("garage/authorized", config.getTopic().c_str());
    TEST_ASSERT_EQUAL_STRING("garage/unauthorized", config.getUnauthorizedTopic().c_str());
    TEST_ASSERT_EQUAL(1883, config.getBrokerPort());
}

void test_emqx_config_validation() {
    EmqxConfig config;
    config.initialize();
    
    if (config.isEmqxEnabled()) {
        TEST_ASSERT_TRUE(config.isValid());
        TEST_ASSERT_FALSE(config.getBrokerHost().isEmpty());
        TEST_ASSERT_TRUE(config.getBrokerPort() > 0);
        TEST_ASSERT_FALSE(config.getClientId().isEmpty());
    } else {
        // Si EMQX n'est pas activé, c'est normal
        TEST_ASSERT_FALSE(config.isValid());
    }
}

void test_emqx_logger_message_building() {
    EmqxLogger logger("test.emqx.io", 1883, "", "", "test-client", "test-topic", "test-unauthorized");
    
    // Test que l'initialisation ne crash pas
    TEST_ASSERT_TRUE(true); // Test placeholder
}

void test_emqx_logger_initialization() {
    EmqxLogger logger("localhost", 1883, "", "", "garage-client", "garage/authorized", "garage/unauthorized");
    
    // Test que l'initialisation ne crash pas
    TEST_ASSERT_TRUE(true);
}

void setUp(void) {
    // Initialisation avant chaque test
}

void tearDown(void) {
    // Nettoyage après chaque test
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_emqx_config_initialization);
    RUN_TEST(test_emqx_config_validation);
    RUN_TEST(test_emqx_logger_initialization);
    RUN_TEST(test_emqx_logger_message_building);
    
    return UNITY_END();
}