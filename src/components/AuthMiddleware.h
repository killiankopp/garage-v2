#ifndef AUTH_MIDDLEWARE_H
#define AUTH_MIDDLEWARE_H

#include <Arduino.h>
#include <WebServer.h>
#include "JwtValidator.h"
#include "AuthConfig.h"

class AuthMiddleware {
public:
    explicit AuthMiddleware(AuthConfig* authConfig);
    ~AuthMiddleware();
    
    bool authenticateRequest(WebServer* server);
    void sendUnauthorizedResponse(WebServer* server, const String& error = "");
    
    // Getters for last validation result
    const ValidationResult& getLastValidationResult() const { return _lastValidationResult; }
    
private:
    AuthConfig* _authConfig;
    JwtValidator* _jwtValidator;
    ValidationResult _lastValidationResult;
    
    String extractAuthorizationHeader(WebServer* server);
    void logAuthenticationAttempt(const String& clientIP, bool success, const String& error = "");
};

#endif // AUTH_MIDDLEWARE_H