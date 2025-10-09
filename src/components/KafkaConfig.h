#ifndef KAFKA_CONFIG_H
#define KAFKA_CONFIG_H

#include <Arduino.h>

class KafkaConfig {
public:
    KafkaConfig();
    
    // Initialise la configuration depuis les variables d'environnement
    void initialize();
    
    // Getters
    String getBrokerUrl() const { return _brokerUrl; }
    String getTopic() const { return _topic; }
    String getUnauthorizedTopic() const { return _unauthorizedTopic; }
    bool isKafkaEnabled() const { return _enabled; }
    
    // Validation
    bool isValid() const;
    void printConfig() const;

private:
    String _brokerUrl;
    String _topic;
    String _unauthorizedTopic;
    bool _enabled;
    
    // Charge une variable d'environnement
    String loadEnvVar(const String& varName, const String& defaultValue = "");
};

#endif // KAFKA_CONFIG_H