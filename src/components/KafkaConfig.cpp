#include "KafkaConfig.h"
#include <cstdlib>

KafkaConfig::KafkaConfig() : _enabled(false) {
}

void KafkaConfig::initialize() {
    _brokerUrl = loadEnvVar("KAFKA_BROKER_URL");
    _topic = loadEnvVar("KAFKA_TOPIC", "REPLACE_ME");
    _unauthorizedTopic = loadEnvVar("KAFKA_UNAUTHORIZED_TOPIC", "REPLACE_ME");
    
    _enabled = !_brokerUrl.isEmpty();
    
    if (_enabled) {
        Serial.println("Kafka configuration loaded successfully");
        printConfig();
    } else {
        Serial.println("Kafka disabled - no broker URL configured");
    }
}

bool KafkaConfig::isValid() const {
    return _enabled && 
           !_brokerUrl.isEmpty() && 
           !_topic.isEmpty() && 
           !_unauthorizedTopic.isEmpty();
}

void KafkaConfig::printConfig() const {
    if (!_enabled) {
        Serial.println("Kafka: Disabled");
        return;
    }
    
    Serial.println("Kafka Configuration:");
    Serial.println("  Broker URL: " + _brokerUrl);
    Serial.println("  Topic: " + _topic);
    Serial.println("  Unauthorized Topic: " + _unauthorizedTopic);
    Serial.println("  Status: " + String(_enabled ? "Enabled" : "Disabled"));
}

String KafkaConfig::loadEnvVar(const String& varName, const String& defaultValue) {
    // Les variables d'environnement sont définies à la compilation via les build_flags de PlatformIO
    
    if (varName == "KAFKA_BROKER_URL") {
        #ifdef KAFKA_BROKER_URL
            return String(KAFKA_BROKER_URL);
        #else
            return defaultValue;
        #endif
    } else if (varName == "KAFKA_TOPIC") {
        #ifdef KAFKA_TOPIC
            return String(KAFKA_TOPIC);
        #else
            return defaultValue.isEmpty() ? "topic" : defaultValue;
        #endif
    } else if (varName == "KAFKA_UNAUTHORIZED_TOPIC") {
        #ifdef KAFKA_UNAUTHORIZED_TOPIC
            return String(KAFKA_UNAUTHORIZED_TOPIC);
        #else
            return defaultValue.isEmpty() ? "topic-unauthorized" : defaultValue;
        #endif
    }
    
    return defaultValue;
}