#ifndef KAFKA_LOGGER_H
#define KAFKA_LOGGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct GateActionLog {
    String action;      // "open" ou "close"
    String sub;         // Subject du token JWT
    String name;        // Nom de l'utilisateur
    bool authorized;    // Si l'action était autorisée
    String token;       // Token complet (seulement pour les actions non autorisées)
};

class KafkaLogger {
public:
    KafkaLogger(const String& brokerUrl, const String& topic, const String& unauthorizedTopic);
    
    // Log une action de porte autorisée
    void logAuthorizedAction(const String& action, const String& sub, const String& name);
    
    // Log une action de porte non autorisée
    void logUnauthorizedAction(const String& action, const String& sub, const String& name, const String& token);
    
    // Test de connectivité Kafka
    bool testConnection();

private:
    String _brokerUrl;
    String _topic;
    String _unauthorizedTopic;
    HTTPClient _httpClient;
    
    // Envoie un message sur un topic Kafka
    bool sendMessage(const String& topic, const String& message);
    
    // Construit le JSON du message
    String buildMessage(const GateActionLog& log);
    
    // Construit l'URL complète pour Kafka
    String buildKafkaUrl(const String& topic);
};

#endif // KAFKA_LOGGER_H