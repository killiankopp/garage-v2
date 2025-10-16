#include "EmqxConfig.h"

#ifndef UNIT_TEST
#include <WiFi.h>
#else
#include <algorithm>
#endif

#include <cstdlib>

namespace {
    String sanitizeMac(const String& mac) {
#ifdef UNIT_TEST
        String sanitized = mac;
        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), ':'), sanitized.end());
        return sanitized;
#else
        String sanitized = mac;
        sanitized.replace(":", "");
        return sanitized;
#endif
    }

    String intToString(int value) {
#ifdef UNIT_TEST
        return std::to_string(value);
#else
        return String(value);
#endif
    }
}

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
#ifdef UNIT_TEST
    String mac = "00:00:00:00:TEST";
#else
    String mac = WiFi.macAddress();
#endif
    _clientId = String("ESP32_") + sanitizeMac(mac);
    
    _enabled = _brokerHost.length() > 0;
    
    if (_enabled) {
        Serial.println("EMQX configuration loaded successfully");
        printConfig();
    } else {
        Serial.println("EMQX disabled - no broker host configured");
    }
}

bool EmqxConfig::isValid() const {
    return _enabled && 
        _brokerHost.length() > 0 && 
        _brokerPort > 0 &&
        _topic.length() > 0 && 
        _unauthorizedTopic.length() > 0 &&
        _clientId.length() > 0;
}

void EmqxConfig::printConfig() const {
    if (!_enabled) {
        Serial.println("EMQX: Disabled");
        return;
    }
    
    Serial.println("EMQX Configuration:");
    Serial.print("  Broker Host: ");
    Serial.println(_brokerHost);
    Serial.print("  Broker Port: ");
    Serial.println(intToString(_brokerPort));
    Serial.print("  Username: ");
    Serial.println(_username.length() == 0 ? String("(none)") : _username);
    Serial.print("  Password: ");
    Serial.println(_password.length() == 0 ? String("(none)") : String("***"));
    Serial.print("  Topic: ");
    Serial.println(_topic);
    Serial.print("  Unauthorized Topic: ");
    Serial.println(_unauthorizedTopic);
    Serial.print("  Client ID: ");
    Serial.println(_clientId);
    Serial.print("  Status: ");
    Serial.println(_enabled ? String("Enabled") : String("Disabled"));
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
            return defaultValue.length() == 0 ? "1883" : defaultValue;
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
            return defaultValue.length() == 0 ? "garage/authorized" : defaultValue;
        #endif
    } else if (varName == "EMQX_UNAUTHORIZED_TOPIC") {
        #ifdef EMQX_UNAUTHORIZED_TOPIC
            return String(EMQX_UNAUTHORIZED_TOPIC);
        #else
            return defaultValue.length() == 0 ? "garage/unauthorized" : defaultValue;
        #endif
    }
    
    return defaultValue;
}