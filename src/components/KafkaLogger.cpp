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

bool KafkaLogger::sendMessage(const String& topic, const String& message) {
    size_t messageCapacity = message.length() + 256;
    DynamicJsonDocument messageDoc(messageCapacity);
    DeserializationError error = deserializeJson(messageDoc, message);
    
    if (error) {
        Serial.println("Failed to parse Kafka log message: " + String(error.c_str()));
        Serial.println("Payload: " + message);
        return false;
    }

    DynamicJsonDocument payloadDoc(message.length() + 512);
    JsonArray records = payloadDoc.createNestedArray("records");
    JsonObject record = records.createNestedObject();
    record["value"].set(messageDoc.as<JsonVariant>());

    String payload;
    serializeJson(payloadDoc, payload);

    String url = buildKafkaUrl(topic);

    _httpClient.begin(url);
    _httpClient.addHeader("Content-Type", "application/vnd.kafka.json.v2+json");
    _httpClient.addHeader("Accept", "application/vnd.kafka.v2+json, application/vnd.kafka+json, application/json");
    _httpClient.setTimeout(10000);

    int httpCode = _httpClient.POST(payload);
    String response = _httpClient.getString();
    _httpClient.end();

    if (httpCode >= 200 && httpCode < 300) {
        Serial.println("Kafka message accepted (HTTP " + String(httpCode) + ")");
        return true;
    }

    Serial.println("Failed to send message to Kafka. HTTP code: " + String(httpCode));
    if (!response.isEmpty()) {
        Serial.println("Response: " + response);
    }
    Serial.println("Request payload: " + payload);
    return false;
}

String KafkaLogger::buildMessage(const GateActionLog& log) {
    size_t capacity = 256 + log.action.length() + log.sub.length() + log.name.length() + log.token.length();
    DynamicJsonDocument doc(capacity);
    
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
    String baseUrl = _brokerUrl;
    if (baseUrl.endsWith("/")) {
        baseUrl.remove(baseUrl.length() - 1);
    }
    if (baseUrl.endsWith("/topics")) {
        return baseUrl + "/" + topic;
    }
    return baseUrl + "/topics/" + topic;
}