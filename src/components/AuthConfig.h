#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>

class AuthConfig {
public:
    static AuthConfig& getInstance();
    
    void loadFromEnvironment();
    
    const String& getKeycloakServerUrl() const { return _keycloakServerUrl; }
    const String& getKeycloakRealm() const { return _keycloakRealm; }
    const String& getKeycloakClientId() const { return _keycloakClientId; }
    
    bool isAuthEnabled() const { return _authEnabled; }
    void setAuthEnabled(bool enabled) { _authEnabled = enabled; }

private:
    AuthConfig() = default;
    ~AuthConfig() = default;
    AuthConfig(const AuthConfig&) = delete;
    AuthConfig& operator=(const AuthConfig&) = delete;
    
    String _keycloakServerUrl;
    String _keycloakRealm;
    String _keycloakClientId;
    bool _authEnabled = false;
    
    void loadEnvVar(const char* varName, String& target, const String& defaultValue = "");
};

#endif // AUTH_CONFIG_H