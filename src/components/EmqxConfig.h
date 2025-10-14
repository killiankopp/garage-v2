#ifndef EMQX_CONFIG_H
#define EMQX_CONFIG_H

#include <Arduino.h>

class EmqxConfig {
public:
    EmqxConfig();
    
    // Initialise la configuration depuis les variables d'environnement
    void initialize();
    
    // Getters
    String getBrokerHost() const { return _brokerHost; }
    int getBrokerPort() const { return _brokerPort; }
    String getUsername() const { return _username; }
    String getPassword() const { return _password; }
    String getTopic() const { return _topic; }
    String getUnauthorizedTopic() const { return _unauthorizedTopic; }
    String getClientId() const { return _clientId; }
    bool isEmqxEnabled() const { return _enabled; }
    
    // Validation
    bool isValid() const;
    void printConfig() const;

private:
    String _brokerHost;
    int _brokerPort;
    String _username;
    String _password;
    String _topic;
    String _unauthorizedTopic;
    String _clientId;
    bool _enabled;
    
    // Charge une variable d'environnement
    String loadEnvVar(const String& varName, const String& defaultValue = "");
};

#endif // EMQX_CONFIG_H