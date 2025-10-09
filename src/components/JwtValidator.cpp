#include "JwtValidator.h"

JwtValidator::JwtValidator(AuthConfig* authConfig) : _authConfig(authConfig) {
}

ValidationResult JwtValidator::validateToken(const String& token) {
    ValidationResult result = {false, "", "", "", ""};
    
    if (token.isEmpty()) {
        result.error = "Token is empty";
        return result;
    }
    
    if (!_authConfig || !_authConfig->isAuthEnabled()) {
        result.error = "Authentication is disabled";
        return result;
    }
    
    _httpClient.begin(buildIntrospectionUrl());
    _httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Préparer la requête d'introspection
    String postData = "token=" + token;
    postData += "&client_id=" + _authConfig->getKeycloakClientId();
    
    int httpCode = _httpClient.POST(postData);
    
    if (httpCode == HTTP_CODE_OK) {
        String response = _httpClient.getString();
        if (!parseTokenResponse(response, result)) {
            result.error = "Failed to parse token response";
        }
    } else if (httpCode > 0) {
        result.error = "HTTP error: " + String(httpCode);
        Serial.println("JWT validation failed with HTTP code: " + String(httpCode));
    } else {
        result.error = "Connection failed: " + String(_httpClient.errorToString(httpCode));
        Serial.println("JWT validation connection failed: " + _httpClient.errorToString(httpCode));
    }
    
    _httpClient.end();
    return result;
}

String JwtValidator::buildIntrospectionUrl() const {
    String url = "http://" + _authConfig->getKeycloakServerUrl();
    url += "/realms/" + _authConfig->getKeycloakRealm();
    url += "/protocol/openid-connect/token/introspect";
    return url;
}

bool JwtValidator::parseTokenResponse(const String& response, ValidationResult& result) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        Serial.println("Failed to parse JSON response: " + String(error.c_str()));
        return false;
    }
    
    result.isValid = doc["active"] | false;
    
    if (result.isValid) {
        result.userId = doc["sub"] | "";
        result.username = doc["username"] | "";
        result.realm = doc["iss"] | "";
        
        Serial.println("Token validated successfully for user: " + result.username);
    } else {
        result.error = "Token is not active";
        Serial.println("Token validation failed: token is not active");
    }
    
    return true;
}

String JwtValidator::extractBearerToken(const String& authHeader) {
    if (authHeader.startsWith("Bearer ")) {
        return authHeader.substring(7);
    }
    return "";
}