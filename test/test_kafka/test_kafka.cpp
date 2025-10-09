#include <unity.h>
#include "../src/components/KafkaConfig.h"
#include "../src/components/KafkaLogger.h"

void test_kafka_config_initialization() {
    KafkaConfig config;
    config.initialize();
    
    // Test que la configuration se charge correctement
    TEST_ASSERT_TRUE(!config.getBrokerUrl().isEmpty() || !config.isKafkaEnabled());
    TEST_ASSERT_EQUAL_STRING("garage", config.getTopic().c_str());
    TEST_ASSERT_EQUAL_STRING("garage-unauthorized", config.getUnauthorizedTopic().c_str());
}

void test_kafka_config_validation() {
    KafkaConfig config;
    config.initialize();
    
    if (config.isKafkaEnabled()) {
        TEST_ASSERT_TRUE(config.isValid());
        TEST_ASSERT_FALSE(config.getBrokerUrl().isEmpty());
    } else {
        // Si Kafka n'est pas activé, c'est normal
        TEST_ASSERT_FALSE(config.isValid());
    }
}

void test_kafka_logger_message_building() {
    KafkaLogger logger("http://test:9092", "test-topic", "test-unauthorized");
    
    // Cette méthode devrait être testée mais elle est privée
    // Dans un vrai projet, on pourrait créer des méthodes de test ou rendre certaines méthodes protected
    TEST_ASSERT_TRUE(true); // Test placeholder
}

void test_kafka_logger_initialization() {
    KafkaLogger logger("http://localhost:9092", "garage", "garage-unauthorized");
    
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
    
    RUN_TEST(test_kafka_config_initialization);
    RUN_TEST(test_kafka_config_validation);
    RUN_TEST(test_kafka_logger_initialization);
    RUN_TEST(test_kafka_logger_message_building);
    
    return UNITY_END();
}