#ifndef UNIT_TEST

#include "EmqxLogger.h"
#include <WiFi.h>

namespace {
    // Truncate a potentially long token to first 20 + "..." + last 20 characters
    String truncateToken(const String& token) {
        const int keep = 20;
        int len = token.length();
        if (len <= keep * 2) {
            return token;
        }
        String head = token.substring(0, keep);
        String tail = token.substring(len - keep);
        return head + String("...") + tail;
    }
}

EmqxLogger::EmqxLogger(const String& brokerHost, int brokerPort, const String& username, const String& password,
                       const String& clientId, const String& topic, const String& unauthorizedTopic)
    : _brokerHost(brokerHost), _brokerPort(brokerPort), _username(username), _password(password),
      _clientId(clientId), _topic(topic), _unauthorizedTopic(unauthorizedTopic),
      _mqttClient(_wifiClient), _lastReconnectAttempt(0) {
    
    _mqttClient.setServer(_brokerHost.c_str(), _brokerPort);
    _mqttClient.setCallback(mqttCallback);
}

bool EmqxLogger::begin() {
    Serial.println("Initializing EMQX MQTT logger...");
    return connectMqtt();
}

void EmqxLogger::loop() {
    if (!_mqttClient.connected()) {
        unsigned long now = millis();
        if (now - _lastReconnectAttempt > RECONNECT_INTERVAL) {
            _lastReconnectAttempt = now;
            if (connectMqtt()) {
                _lastReconnectAttempt = 0;
            }
        }
    } else {
        _mqttClient.loop();
    }
}

bool EmqxLogger::connectMqtt() {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(_brokerHost);
    Serial.print(":");
    Serial.print(_brokerPort);
    Serial.print(" as ");
    Serial.print(_clientId);
    Serial.print("...");
    
    bool connected;
    if (_username.isEmpty()) {
        // Connect without authentication
        connected = _mqttClient.connect(_clientId.c_str());
    } else {
        // Connect with authentication
        connected = _mqttClient.connect(_clientId.c_str(), _username.c_str(), _password.c_str());
    }
    
    if (connected) {
        Serial.println(" connected!");
        return true;
    } else {
        Serial.print(" failed, rc=");
        Serial.print(_mqttClient.state());
        Serial.println(" retrying in 5 seconds");
        return false;
    }
}

bool EmqxLogger::isConnected() {
    return _mqttClient.connected();
}

void EmqxLogger::logAuthorizedAction(const String& action, const String& sub, const String& name) {
    GateActionLog log = {
        .action = action,
        .sub = sub,
        .name = name,
        .authorized = true,
        .token = ""
    };
    
    String message = buildMessage(log);
    
    Serial.println("Logging authorized action: " + action + " by user: " + name + " (sub: " + sub + ")");
    
    if (!publishMessage(_topic, message)) {
        Serial.println("Failed to send authorized action log to EMQX");
    }
}

void EmqxLogger::logUnauthorizedAction(const String& action, const String& sub, const String& name, const String& token) {
    // Masquer la majorité du token pour réduire la taille et améliorer la sécurité
    String maskedToken = truncateToken(token);

    GateActionLog log = {
        .action = action,
        .sub = sub,
        .name = name,
        .authorized = false,
        .token = maskedToken
    };
    
    String message = buildMessage(log);
    
    Serial.println("Logging unauthorized action: " + action + " - sub: " + sub + ", name: " + name);
    
    if (!publishMessage(_unauthorizedTopic, message)) {
        Serial.println("Failed to send unauthorized action log to EMQX");
    }
}

bool EmqxLogger::publishMessage(const String& topic, const String& message) {
    if (!_mqttClient.connected()) {
        Serial.println("MQTT not connected, cannot publish message");
        return false;
    }
    
    Serial.println("Publishing to topic: " + topic);
    Serial.println("Message: " + message);
    
    bool success = _mqttClient.publish(topic.c_str(), message.c_str());
    
    if (success) {
        Serial.println("Message published successfully to EMQX");
    } else {
        Serial.println("Failed to publish message to EMQX");
    }
    
    return success;
}

String EmqxLogger::buildMessage(const GateActionLog& log) {
    size_t capacity = 256 + log.action.length() + log.sub.length() + log.name.length() + log.token.length();
    DynamicJsonDocument doc(capacity);
    
    doc["timestamp"] = millis();
    doc["action"] = log.action;
    doc["authorized"] = log.authorized;
    doc["device_id"] = _clientId;
    
    // Ajouter sub et name s'ils existent
    if (!log.sub.isEmpty()) {
        doc["sub"] = log.sub;
    }
    
    if (!log.name.isEmpty()) {
        doc["name"] = log.name;
    }
    
    // Pour les actions non autorisées, inclure le token complet
    if (!log.authorized && !log.token.isEmpty()) {
        doc["token"] = log.token;
    }
    
    String message;
    serializeJson(doc, message);
    
    return message;
}

void EmqxLogger::mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Callback for received messages - not currently used but available for future features
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

#endif // UNIT_TEST