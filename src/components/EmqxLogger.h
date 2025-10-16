#ifndef EMQX_LOGGER_H
#define EMQX_LOGGER_H

#ifdef UNIT_TEST
#include "../../test/mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#endif

struct GateActionLog {
    String action;      // "open" ou "close"
    String sub;         // Subject du token JWT
    String name;        // Nom de l'utilisateur
    bool authorized;    // Si l'action était autorisée
    String token;       // Token complet (seulement pour les actions non autorisées)
};

#ifdef UNIT_TEST

class EmqxLogger {
public:
    EmqxLogger(const String&, int, const String&, const String&,
               const String&, const String&, const String&) {}

    bool begin() { return true; }
    void loop() {}
    void logAuthorizedAction(const String&, const String&, const String&) {}
    void logUnauthorizedAction(const String&, const String&, const String&, const String&) {}
    bool isConnected() { return true; }
};

#else

class EmqxLogger {
public:
    EmqxLogger(const String& brokerHost, int brokerPort, const String& username, const String& password,
               const String& clientId, const String& topic, const String& unauthorizedTopic);
    
    // Initialize MQTT connection
    bool begin();
    
    // Maintain MQTT connection
    void loop();
    
    // Log une action de porte autorisée
    void logAuthorizedAction(const String& action, const String& sub, const String& name);
    
    // Log une action de porte non autorisée
    void logUnauthorizedAction(const String& action, const String& sub, const String& name, const String& token);
    
    // Check if connected
    bool isConnected();

private:
    String _brokerHost;
    int _brokerPort;
    String _username;
    String _password;
    String _clientId;
    String _topic;
    String _unauthorizedTopic;
    
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    
    unsigned long _lastReconnectAttempt;
    static const unsigned long RECONNECT_INTERVAL = 5000; // 5 seconds
    
    // Connect to MQTT broker
    bool connectMqtt();
    
    // Publish a message to a topic
    bool publishMessage(const String& topic, const String& message);
    
    // Build JSON message
    String buildMessage(const GateActionLog& log);
    
    // MQTT callback (if needed for future features)
    static void mqttCallback(char* topic, byte* payload, unsigned int length);
};

#endif // UNIT_TEST

#endif // EMQX_LOGGER_H