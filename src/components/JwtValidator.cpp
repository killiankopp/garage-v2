#include "JwtValidator.h"

#include <cstdio>
#include <time.h>
#if defined(ARDUINO) && !defined(UNIT_TEST)
#include <WiFiClientSecure.h>
#include <WiFi.h>
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
    
    // Log current time for JWT timestamp comparison
    time_t now = time(nullptr);
    Serial.println("[Auth] Current ESP32 time: " + String(now) + " (" + String(ctime(&now)).substring(0, 24) + ")");
    
    // Decode and display JWT claims for diagnostics
    decodeAndLogJwtClaims(token);
    
    const String clientId = _authConfig->getKeycloakClientId();
    const bool hasClientSecret = _authConfig->hasKeycloakClientSecret();
    const String clientSecret = _authConfig->getKeycloakClientSecret();
    const String introspectionUrl = buildIntrospectionUrl();

    Serial.println("[Auth] Introspection URL: " + introspectionUrl);
    if (introspectionUrl.indexOf("://") < 0) {
        Serial.println("[Auth][Warning] KEYCLOAK_SERVER_URL semble invalide. Exemple: https://host:port");
    }

    // Extract host/port for diagnostics
    String host;
    uint16_t port = 0;
    bool isHttps = false;
    {
        int schemeSep = introspectionUrl.indexOf("://");
        String rest = schemeSep > 0 ? introspectionUrl.substring(schemeSep + 3) : introspectionUrl;
        isHttps = introspectionUrl.startsWith("https://");
        int pathStart = rest.indexOf('/');
        String hostPort = pathStart >= 0 ? rest.substring(0, pathStart) : rest;
        int colon = hostPort.indexOf(':');
        if (colon >= 0) {
            host = hostPort.substring(0, colon);
            port = hostPort.substring(colon + 1).toInt();
        } else {
            host = hostPort;
            port = isHttps ? 443 : 80;
        }
    }
#if defined(ARDUINO) && !defined(UNIT_TEST)
    IPAddress ip;
    if (WiFi.hostByName(host.c_str(), ip)) {
        Serial.println("[Auth] Résolution DNS: " + host + " -> " + ip.toString() + ":" + String(port));
    } else {
        Serial.println("[Auth][Error] Échec de résolution DNS pour: " + host);
    }
#endif

    // Initialize HTTP client with proper transport (HTTP or HTTPS)
    if (introspectionUrl.startsWith("https://")) {
#if defined(ARDUINO) && !defined(UNIT_TEST)
#if KEYCLOAK_TLS_INSECURE
    _secureClient.setInsecure(); // Dev mode: disable certificate validation
#endif
    // CRITICAL for Kubernetes ingress: SNI hostname is automatically sent by HTTPClient
    // but we can verify TLS connectivity first
    _secureClient.setHandshakeTimeout(15); // seconds
    Serial.println("[Auth] Configuring TLS for SNI hostname: " + host);
    
    _httpClient.begin(_secureClient, introspectionUrl);
#else
    _httpClient.begin(introspectionUrl);
#endif
    } else {
    _httpClient.begin(introspectionUrl);
    }

    // Harden HTTP behavior: avoid keep-alive reuse and use explicit timeouts
    _httpClient.setReuse(false);
    _httpClient.setTimeout(12000); // ms
#ifdef HTTPC_STRICT_FOLLOW_REDIRECTS
    _httpClient.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
#endif
    // Some proxies/servers behave better with HTTP/1.0 (no chunked encoding)
    _httpClient.useHTTP10(true);

    if (hasClientSecret) {
        _httpClient.setAuthorization(clientId.c_str(), clientSecret.c_str());
    } else {
        Serial.println("[Auth][Warning] Aucun client_secret. L'introspection nécessite un client confidentiel.");
    }

    _httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    _httpClient.addHeader("Connection", "close");
    
    // Préparer la requête d'introspection
    String postData = "token=" + urlEncode(token);
    postData += "&client_id=" + urlEncode(clientId);
    
    if (hasClientSecret) {
        postData += "&client_secret=" + urlEncode(clientSecret);
    }
    
    // Add token_type_hint for better compatibility
    postData += "&token_type_hint=access_token";
    
    Serial.println("[Auth] POST data length: " + String(postData.length()));
    
    int httpCode = _httpClient.POST(postData);
    
    if (httpCode == HTTP_CODE_OK) {
        String response = _httpClient.getString();
        Serial.println("[Auth] Introspection response: " + response);
        if (!parseTokenResponse(response, result)) {
            result.error = "Failed to parse token response";
        }
    } else if (httpCode > 0) {
        result.error = "HTTP error: " + String(httpCode);
        Serial.println("[Auth] Introspection failed, HTTP code: " + String(httpCode));
        Serial.println("[Auth] Response: " + _httpClient.getString());
    } else {
        result.error = "Connection failed: " + String(_httpClient.errorToString(httpCode));
        Serial.println("[Auth] Connection failed: " + _httpClient.errorToString(httpCode));
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
    JsonDocument doc;
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
        
        // Log additional fields that might explain why
        if (doc["error"].is<const char*>()) {
            Serial.println("[Auth] Error from Keycloak: " + String(doc["error"].as<const char*>()));
        }
        if (doc["error_description"].is<const char*>()) {
            Serial.println("[Auth] Error description: " + String(doc["error_description"].as<const char*>()));
        }
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

void JwtValidator::decodeAndLogJwtClaims(const String& token) {
    // JWT format: header.payload.signature
    int firstDot = token.indexOf('.');
    int secondDot = token.indexOf('.', firstDot + 1);
    
    if (firstDot < 0 || secondDot < 0) {
        Serial.println("[Auth] Warning: Token doesn't appear to be a valid JWT");
        return;
    }
    
    String payload = token.substring(firstDot + 1, secondDot);
    
    // Base64 decode the payload
    String decodedPayload = base64Decode(payload);
    
    if (decodedPayload.isEmpty()) {
        Serial.println("[Auth] Warning: Failed to decode JWT payload");
        return;
    }
    
    // Parse JSON claims
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, decodedPayload);
    
    if (error) {
        Serial.println("[Auth] Warning: Failed to parse JWT claims JSON");
        return;
    }
    
    Serial.println("[Auth] JWT Claims:");
    
    if (doc["exp"].is<long>()) {
        time_t exp = doc["exp"].as<time_t>();
        time_t now = time(nullptr);
        long diff = exp - now;
        Serial.println("  exp: " + String(exp) + " (expires in " + String(diff) + " seconds)");
        if (diff < 0) {
            Serial.println("  ⚠️  Token is EXPIRED (exp < now)");
        }
    }
    
    if (doc["iat"].is<long>()) {
        Serial.println("  iat: " + String(doc["iat"].as<long>()));
    }
    
    if (doc["iss"].is<const char*>()) {
        Serial.println("  iss: " + String(doc["iss"].as<const char*>()));
    }
    
    if (doc["aud"].is<JsonArray>()) {
        Serial.print("  aud: [");
        JsonArray aud = doc["aud"].as<JsonArray>();
        for (size_t i = 0; i < aud.size(); i++) {
            if (i > 0) Serial.print(", ");
            Serial.print("\"" + String(aud[i].as<const char*>()) + "\"");
        }
        Serial.println("]");
    } else if (doc["aud"].is<const char*>()) {
        Serial.println("  aud: \"" + String(doc["aud"].as<const char*>()) + "\"");
    }
    
    if (doc["sub"].is<const char*>()) {
        Serial.println("  sub: " + String(doc["sub"].as<const char*>()));
    }
    
    if (doc["azp"].is<const char*>()) {
        Serial.println("  azp: " + String(doc["azp"].as<const char*>()));
    }
    
    if (doc["preferred_username"].is<const char*>()) {
        Serial.println("  username: " + String(doc["preferred_username"].as<const char*>()));
    }
}

String JwtValidator::base64Decode(const String& input) {
    // Add padding if needed
    String padded = input;
    while (padded.length() % 4 != 0) {
        padded += '=';
    }
    
    // Replace URL-safe characters
    padded.replace('-', '+');
    padded.replace('_', '/');
    
    // Simple base64 decode
    const char* b64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String decoded;
    decoded.reserve((padded.length() * 3) / 4);
    
    for (size_t i = 0; i < padded.length(); i += 4) {
        uint32_t val = 0;
        int validChars = 0;
        
        for (int j = 0; j < 4 && (i + j) < padded.length(); j++) {
            char c = padded.charAt(i + j);
            if (c == '=') break;
            
            const char* pos = strchr(b64chars, c);
            if (!pos) continue;
            
            val = (val << 6) | (pos - b64chars);
            validChars++;
        }
        
        if (validChars >= 2) decoded += (char)((val >> ((validChars - 2) * 6 + 2)) & 0xFF);
        if (validChars >= 3) decoded += (char)((val >> ((validChars - 3) * 6 + 2)) & 0xFF);
        if (validChars >= 4) decoded += (char)((val >> 2) & 0xFF);
    }
    
    return decoded;
}