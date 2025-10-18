#include "AuthConfig.h"

AuthConfig& AuthConfig::getInstance() {
    static AuthConfig instance;
    return instance;
}

void AuthConfig::loadFromEnvironment() {
    // Load configuration from compile-time environment variables
    // In a real embedded system, these would typically be loaded from EEPROM or a config file
    
    #ifdef KEYCLOAK_SERVER_URL
        _keycloakServerUrl = String(KEYCLOAK_SERVER_URL);
    #else
        _keycloakServerUrl = "REPLACE_ME"; // Default dev server
    #endif
    
    #ifdef KEYCLOAK_REALM
        _keycloakRealm = String(KEYCLOAK_REALM);
    #else
        _keycloakRealm = "REPLACE_ME";
    #endif
    
    #ifdef KEYCLOAK_CLIENT_ID
        _keycloakClientId = String(KEYCLOAK_CLIENT_ID);
    #else
        _keycloakClientId = "REPLACE_ME";
    #endif

    #ifdef KEYCLOAK_CLIENT_SECRET
        _keycloakClientSecret = String(KEYCLOAK_CLIENT_SECRET);
    #else
        _keycloakClientSecret = "";
    #endif
    
    // Enable auth only if server URL is configured
    _authEnabled = !_keycloakServerUrl.isEmpty() && _keycloakServerUrl != "disabled";
    
    if (_authEnabled) {
        Serial.println("Auth enabled with Keycloak server: " + _keycloakServerUrl);
        Serial.println("Realm: " + _keycloakRealm);
        Serial.println("Client ID: " + _keycloakClientId);
        Serial.println("Client Secret configured: " + String(_keycloakClientSecret.isEmpty() ? "no" : "yes"));

        if (_keycloakClientSecret.isEmpty()) {
            Serial.println("Warning: KEYCLOAK_CLIENT_SECRET not set. Confidential clients will fail introspection.");
        }
    } else {
        Serial.println("Authentication disabled");
    }
}

void AuthConfig::loadEnvVar(const char* varName, String& target, const String& defaultValue) {
    // Cette méthode pourrait être étendue pour lire depuis EEPROM ou un fichier de config
    target = defaultValue;
}