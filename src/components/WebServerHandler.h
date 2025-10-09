#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include <WebServer.h>
#include "GateController.h"
#include "GateMonitor.h"
#include "AuthConfig.h"
#include "AuthMiddleware.h"
#include "KafkaConfig.h"
#include "KafkaLogger.h"

class WebServerHandler {
public:
    WebServerHandler(GateController* gateController, GateMonitor* gateMonitor);
    ~WebServerHandler();
    void begin();
    void handleClient();

private:
    WebServer _server;
    GateController* _gateController;
    GateMonitor* _gateMonitor;
    AuthConfig* _authConfig;
    AuthMiddleware* _authMiddleware;
    KafkaConfig* _kafkaConfig;
    KafkaLogger* _kafkaLogger;
    
    // Route handlers
    void handleRoot();
    void handleHealth();
    void handleAuthInfo();
    void handleGateOpen();
    void handleGateClose();
    void handleGateStatus();
    
    // Helper methods
    String buildStatusJson();
    void setupRoutes();
    
    // Authentication helpers
    bool requireAuthentication();
    void initializeAuth();
    
    // Kafka logging helpers
    void initializeKafka();
    void logGateAction(const String& action, bool authorized);
};

#endif // WEB_SERVER_HANDLER_H