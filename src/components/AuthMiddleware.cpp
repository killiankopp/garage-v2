#include "AuthMiddleware.h"

AuthMiddleware::AuthMiddleware(AuthConfig* authConfig) 
    : _authConfig(authConfig), _jwtValidator(nullptr) {
    
    if (_authConfig && _authConfig->isAuthEnabled()) {
        _jwtValidator = new JwtValidator(_authConfig);
    }
}

AuthMiddleware::~AuthMiddleware() {
    delete _jwtValidator;
}

bool AuthMiddleware::authenticateRequest(WebServer* server) {
    // Si l'authentification est désactivée, autoriser la requête
    if (!_authConfig || !_authConfig->isAuthEnabled()) {
        _lastValidationResult = {true, "", "", "", ""};
        return true;
    }
    
    String authHeader = extractAuthorizationHeader(server);
    String clientIP = server->client().remoteIP().toString();
    
    if (authHeader.isEmpty()) {
        String error = "Missing Authorization header";
        logAuthenticationAttempt(clientIP, false, error);
        _lastValidationResult = {false, error, "", "", ""};
        return false;
    }
    
    // Extraire le token Bearer
    String token = "";
    if (authHeader.startsWith("Bearer ")) {
        token = authHeader.substring(7);
    }
    
    if (token.isEmpty()) {
        String error = "Invalid Authorization header format. Expected: Bearer <token>";
        logAuthenticationAttempt(clientIP, false, error);
        _lastValidationResult = {false, error, "", "", ""};
        return false;
    }
    
    // Valider le token avec Keycloak
    _lastValidationResult = _jwtValidator->validateToken(token);
    
    logAuthenticationAttempt(clientIP, _lastValidationResult.isValid, _lastValidationResult.error);
    
    return _lastValidationResult.isValid;
}

void AuthMiddleware::sendUnauthorizedResponse(WebServer* server, const String& error) {
    String errorMessage = error.isEmpty() ? _lastValidationResult.error : error;
    
    if (errorMessage.isEmpty()) {
        errorMessage = "Unauthorized";
    }
    
    // Créer une réponse JSON d'erreur
    String jsonResponse = "{";
    jsonResponse += "\"error\":\"Unauthorized\",";
    jsonResponse += "\"message\":\"" + errorMessage + "\",";
    jsonResponse += "\"code\":401";
    jsonResponse += "}";
    
    server->send(401, "application/json", jsonResponse);
}

String AuthMiddleware::extractAuthorizationHeader(WebServer* server) {
    if (server->hasHeader("Authorization")) {
        return server->header("Authorization");
    }
    
    // Vérifier aussi les variantes courantes
    if (server->hasHeader("authorization")) {
        return server->header("authorization");
    }
    
    return "";
}

void AuthMiddleware::logAuthenticationAttempt(const String& clientIP, bool success, const String& error) {
    String logMessage = "Auth attempt from " + clientIP + ": ";
    
    if (success) {
        logMessage += "SUCCESS";
        if (!_lastValidationResult.username.isEmpty()) {
            logMessage += " (user: " + _lastValidationResult.username + ")";
        }
    } else {
        logMessage += "FAILED";
        if (!error.isEmpty()) {
            logMessage += " - " + error;
        }
    }
    
    Serial.println(logMessage);
}