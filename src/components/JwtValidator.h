#ifndef JWT_VALIDATOR_H
#define JWT_VALIDATOR_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "AuthConfig.h"

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
    
    String buildIntrospectionUrl() const;
    bool parseTokenResponse(const String& response, ValidationResult& result);
    String extractBearerToken(const String& authHeader);
};

#endif // JWT_VALIDATOR_H