#include "KafkaLogger.h"

KafkaLogger::KafkaLogger(const String& brokerUrl, const String& topic, const String& unauthorizedTopic)
    : _brokerUrl(brokerUrl), _topic(topic), _unauthorizedTopic(unauthorizedTopic) {
}

void KafkaLogger::logAuthorizedAction(const String& action, const String& sub, const String& name) {
    GateActionLog log = {
        .action = action,
        .sub = sub,
        .name = name,
        .authorized = true,
        .token = ""
    };
    
    String message = buildMessage(log);
    
    Serial.println("Logging authorized action: " + action + " by user: " + name + " (sub: " + sub + ")");
    
    if (!sendMessage(_topic, message)) {
        Serial.println("Failed to send authorized action log to Kafka");
    }
}

void KafkaLogger::logUnauthorizedAction(const String& action, const String& sub, const String& name, const String& token) {
    GateActionLog log = {
        .action = action,
        .sub = sub,
        .name = name,
        .authorized = false,
        .token = token
    };
    
    String message = buildMessage(log);
    
    Serial.println("Logging unauthorized action: " + action + " - sub: " + sub + ", name: " + name);
    
    if (!sendMessage(_unauthorizedTopic, message)) {
        Serial.println("Failed to send unauthorized action log to Kafka");
    }
}

bool KafkaLogger::testConnection() {
    _httpClient.begin(_brokerUrl + "/health");
    _httpClient.setTimeout(5000);
    
    int httpCode = _httpClient.GET();
    _httpClient.end();
    
    return (httpCode == 200);
}

bool KafkaLogger::sendMessage(const String& topic, const String& message) {
    String url = buildKafkaUrl(topic);
    
    _httpClient.begin(url);
    _httpClient.addHeader("Content-Type", "application/json");
    _httpClient.setTimeout(10000);
    
    int httpCode = _httpClient.POST(message);
    String response = _httpClient.getString();
    _httpClient.end();
    
    if (httpCode == 200 || httpCode == 201) {
        Serial.println("Message sent to Kafka successfully");
        return true;
    } else {
        Serial.println("Failed to send message to Kafka. HTTP code: " + String(httpCode));
        Serial.println("Response: " + response);
        return false;
    }
}

String KafkaLogger::buildMessage(const GateActionLog& log) {
    DynamicJsonDocument doc(1024);
    
    doc["timestamp"] = millis();
    doc["action"] = log.action;
    doc["authorized"] = log.authorized;
    
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

String KafkaLogger::buildKafkaUrl(const String& topic) {
    // Assume une API REST Kafka standard
    // L'URL exacte peut dépendre de votre setup Kafka
    return _brokerUrl + "/topics/" + topic + "/messages";
}