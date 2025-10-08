#include "WebServerHandler.h"
#include "Config.h"

WebServerHandler::WebServerHandler(GateController* gateController, GateMonitor* gateMonitor) 
    : _server(SERVER_PORT), _gateController(gateController), _gateMonitor(gateMonitor) {
}

void WebServerHandler::begin() {
    setupRoutes();
    _server.begin();
    Serial.println("HTTP server started");
}

void WebServerHandler::handleClient() {
    _server.handleClient();
}

void WebServerHandler::setupRoutes() {
    // Bind methods to this instance
    _server.on("/", [this]() { handleRoot(); });
    _server.on("/health", [this]() { handleHealth(); });
    _server.on("/gate/open", [this]() { handleGateOpen(); });
    _server.on("/gate/close", [this]() { handleGateClose(); });
    _server.on("/gate/status", [this]() { handleGateStatus(); });
}

void WebServerHandler::handleRoot() {
    _server.send(200, "text/plain", "ESP32 garage online!");
}

void WebServerHandler::handleHealth() {
    _server.send(200, "text/plain", "OK");
}

void WebServerHandler::handleGateOpen() {
    GateState currentState = _gateController->readState();
    
    if (currentState != OPEN) {
        _gateController->triggerRelay();
        _gateMonitor->startOperation(OPENING, OPEN);
    }
    
    // Return current status in JSON format
    handleGateStatus();
}

void WebServerHandler::handleGateClose() {
    GateState currentState = _gateController->readState();
    
    if (currentState != CLOSED) {
        _gateController->triggerRelay();
        _gateMonitor->startOperation(CLOSING, CLOSED);
    }
    
    // Return current status in JSON format
    handleGateStatus();
}

void WebServerHandler::handleGateStatus() {
    String json = buildStatusJson();
    _server.send(200, "application/json", json);
}

String WebServerHandler::buildStatusJson() {
    bool sensorClosed = _gateController->isClosedSensorActive();
    bool sensorOpen = _gateController->isOpenSensorActive();
    GateState state = _gateController->readState();
    OperationState currentOperation = _gateMonitor->getCurrentOperation();
    
    String json = "{";
    json += "\"status\":\"";
    
    // Priority: ongoing operations override physical state
    if (currentOperation == OPENING) {
        json += "opening";
    } else if (currentOperation == CLOSING) {
        json += "closing";
    } else {
        // No operation, return physical state
        switch(state) {
            case CLOSED: json += "closed"; break;
            case OPEN: json += "open"; break;
            case UNKNOWN: json += "unknown"; break;
        }
    }
    
    json += "\",";
    json += "\"sensor_closed\":" + String(sensorClosed ? "true" : "false") + ",";
    json += "\"sensor_open\":" + String(sensorOpen ? "true" : "false") + ",";
    
    if (_gateMonitor->isOperationInProgress()) {
        json += "\"operation_time\":" + String(_gateMonitor->getOperationElapsedTime()) + ",";
        json += "\"timeout_remaining\":" + String(_gateMonitor->getOperationRemainingTime()) + ",";
    }
    
    json += "\"alert_active\":" + String(_gateMonitor->isAlertActive() ? "true" : "false") + ",";
    json += "\"auto_close_enabled\":" + String(_gateMonitor->isAutoCloseEnabled() ? "true" : "false");
    
    if (_gateMonitor->isAutoCloseEnabled() && state == OPEN && currentOperation == IDLE) {
        json += ",\"auto_close_remaining\":" + String(_gateMonitor->getAutoCloseRemainingTime());
    }
    
    json += "}";
    
    return json;
}