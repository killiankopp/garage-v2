#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include <WebServer.h>
#include "GateController.h"
#include "GateMonitor.h"

class WebServerHandler {
public:
    WebServerHandler(GateController* gateController, GateMonitor* gateMonitor);
    void begin();
    void handleClient();

private:
    WebServer _server;
    GateController* _gateController;
    GateMonitor* _gateMonitor;
    
    // Route handlers
    void handleRoot();
    void handleHealth();
    void handleGateOpen();
    void handleGateClose();
    void handleGateStatus();
    
    // Helper methods
    String buildStatusJson();
    void setupRoutes();
};

#endif // WEB_SERVER_HANDLER_H