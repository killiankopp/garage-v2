#include "EmqxConfig.h"
#include <WiFi.h>
#include <cstdlib>

EmqxConfig::EmqxConfig() : _brokerPort(1883), _enabled(false) {
}

void EmqxConfig::initialize() {
    _brokerHost = loadEnvVar("EMQX_BROKER_HOST");
    String portStr = loadEnvVar("EMQX_BROKER_PORT", "1883");
    _brokerPort = portStr.toInt();
    if (_brokerPort <= 0) {
        _brokerPort = 1883; // default MQTT port
    }
    
    _username = loadEnvVar("EMQX_USERNAME");
    _password = loadEnvVar("EMQX_PASSWORD");
    _topic = loadEnvVar("EMQX_TOPIC", "garage/authorized");
    _unauthorizedTopic = loadEnvVar("EMQX_UNAUTHORIZED_TOPIC", "garage/unauthorized");
    
    // Generate a unique client ID based on ESP32 MAC address
    _clientId = "ESP32_" + WiFi.macAddress();
    _clientId.replace(":", "");
    
    _enabled = !_brokerHost.isEmpty();
    
    if (_enabled) {
        Serial.println("EMQX configuration loaded successfully");
        printConfig();
    } else {
        Serial.println("EMQX disabled - no broker host configured");
    }
}

bool EmqxConfig::isValid() const {
    return _enabled && 
           !_brokerHost.isEmpty() && 
           _brokerPort > 0 &&
           !_topic.isEmpty() && 
           !_unauthorizedTopic.isEmpty() &&
           !_clientId.isEmpty();
}

void EmqxConfig::printConfig() const {
    if (!_enabled) {
        Serial.println("EMQX: Disabled");
        return;
    }
    
    Serial.println("EMQX Configuration:");
    Serial.println("  Broker Host: " + _brokerHost);
    Serial.println("  Broker Port: " + String(_brokerPort));
    Serial.println("  Username: " + (_username.isEmpty() ? "(none)" : _username));
    Serial.println("  Password: " + (_password.isEmpty() ? "(none)" : "***"));
    Serial.println("  Topic: " + _topic);
    Serial.println("  Unauthorized Topic: " + _unauthorizedTopic);
    Serial.println("  Client ID: " + _clientId);
    Serial.println("  Status: " + String(_enabled ? "Enabled" : "Disabled"));
}

String EmqxConfig::loadEnvVar(const String& varName, const String& defaultValue) {
    // Les variables d'environnement sont définies à la compilation via les build_flags de PlatformIO
    
    if (varName == "EMQX_BROKER_HOST") {
        #ifdef EMQX_BROKER_HOST
            return String(EMQX_BROKER_HOST);
        #else
            return defaultValue;
        #endif
    } else if (varName == "EMQX_BROKER_PORT") {
        #ifdef EMQX_BROKER_PORT
            return String(EMQX_BROKER_PORT);
        #else
            return defaultValue.isEmpty() ? "1883" : defaultValue;
        #endif
    } else if (varName == "EMQX_USERNAME") {
        #ifdef EMQX_USERNAME
            return String(EMQX_USERNAME);
        #else
            return defaultValue;
        #endif
    } else if (varName == "EMQX_PASSWORD") {
        #ifdef EMQX_PASSWORD
            return String(EMQX_PASSWORD);
        #else
            return defaultValue;
        #endif
    } else if (varName == "EMQX_TOPIC") {
        #ifdef EMQX_TOPIC
            return String(EMQX_TOPIC);
        #else
            return defaultValue.isEmpty() ? "garage/authorized" : defaultValue;
        #endif
    } else if (varName == "EMQX_UNAUTHORIZED_TOPIC") {
        #ifdef EMQX_UNAUTHORIZED_TOPIC
            return String(EMQX_UNAUTHORIZED_TOPIC);
        #else
            return defaultValue.isEmpty() ? "garage/unauthorized" : defaultValue;
        #endif
    }
    
    return defaultValue;
}