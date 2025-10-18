#ifndef JWT_VALIDATOR_H
#define JWT_VALIDATOR_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "AuthConfig.h"

#if defined(ARDUINO) && !defined(UNIT_TEST)
#include <WiFiClientSecure.h>
#endif

struct ValidationResult {
    bool isValid;
    String error;
    String userId;
    String username;
    String realm;
};

class JwtValidator {
public:
    explicit JwtValidator(AuthConfig* authConfig);
    
    ValidationResult validateToken(const String& token);
    
private:
    AuthConfig* _authConfig;
    HTTPClient _httpClient;
#if defined(ARDUINO) && !defined(UNIT_TEST)
    WiFiClientSecure _secureClient;
#endif
    
    String buildIntrospectionUrl() const;
    bool parseTokenResponse(const String& response, ValidationResult& result);
    String extractBearerToken(const String& authHeader);
    String urlEncode(const String& value) const;
    void decodeAndLogJwtClaims(const String& token);
    String base64Decode(const String& input);
};

#endif // JWT_VALIDATOR_H