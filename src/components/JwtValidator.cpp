#include "JwtValidator.h"

#include <cstdio>
#if defined(ARDUINO) && !defined(UNIT_TEST)
#include <WiFiClientSecure.h>
#endif

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
    
    const String clientId = _authConfig->getKeycloakClientId();
    const bool hasClientSecret = _authConfig->hasKeycloakClientSecret();
    const String clientSecret = _authConfig->getKeycloakClientSecret();
    const String introspectionUrl = buildIntrospectionUrl();

    // Initialize HTTP client with proper transport (HTTP or HTTPS)
    if (introspectionUrl.startsWith("https://")) {
#if defined(ARDUINO) && !defined(UNIT_TEST)
    WiFiClientSecure secure;
#if KEYCLOAK_TLS_INSECURE
    secure.setInsecure(); // Dev mode: disable certificate validation
#endif
    _httpClient.begin(secure, introspectionUrl);
#else
    _httpClient.begin(introspectionUrl);
#endif
    } else {
    _httpClient.begin(introspectionUrl);
    }

    if (hasClientSecret) {
        _httpClient.setAuthorization(clientId.c_str(), clientSecret.c_str());
    }

    _httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Préparer la requête d'introspection
    String postData = "token=" + urlEncode(token);
    postData += "&client_id=" + urlEncode(clientId);
    
    if (hasClientSecret) {
        postData += "&client_secret=" + urlEncode(clientSecret);
    }
    
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
    String base = _authConfig->getKeycloakServerUrl();
    // If user provided full URL with scheme, keep it; otherwise default to http://
    if (!(base.startsWith("http://") || base.startsWith("https://"))) {
        base = String("http://") + base;
    }
    // Trim trailing slashes
    while (base.endsWith("/")) {
        base.remove(base.length() - 1);
    }

    String url = base;
    url += "/realms/";
    url += _authConfig->getKeycloakRealm();
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

String JwtValidator::urlEncode(const String& value) const {
    String encoded;
    encoded.reserve(value.length() * 3);
    for (size_t i = 0; i < value.length(); ++i) {
        const unsigned char c = static_cast<unsigned char>(value.charAt(i));
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += static_cast<char>(c);
        } else if (c == ' ') {
            encoded += '+';
        } else {
            char buf[4];
            snprintf(buf, sizeof(buf), "%%%02X", c);
            encoded += buf;
        }
    }
    return encoded;
}