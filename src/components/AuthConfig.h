#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H

#ifdef UNIT_TEST
#include "../../test/mocks/Arduino.h"
#else
#include <Arduino.h>
#include <WiFi.h>
#endif

class AuthConfig {
public:
    static AuthConfig& getInstance();
    
    void loadFromEnvironment();
    
    const String& getKeycloakServerUrl() const { return _keycloakServerUrl; }
    const String& getKeycloakRealm() const { return _keycloakRealm; }
    const String& getKeycloakClientId() const { return _keycloakClientId; }
    const String& getKeycloakClientSecret() const { return _keycloakClientSecret; }
    bool hasKeycloakClientSecret() const { return !_keycloakClientSecret.isEmpty(); }
    
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
    String _keycloakClientSecret;
    bool _authEnabled = false;
    
    void loadEnvVar(const char* varName, String& target, const String& defaultValue = "");
};

#endif // AUTH_CONFIG_H