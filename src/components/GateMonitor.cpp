#include "GateMonitor.h"
#include "Config.h"

#ifdef UNIT_TEST
#include "../../test/mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#endif

GateMonitor::GateMonitor(GateController* gateController) 
    : _gateController(gateController), _lastState(UNKNOWN), _currentOperation(IDLE),
      _operationStartTime(0), _expectedState(UNKNOWN), _alertTriggered(false),
      _gateOpenedTime(0), _autoCloseEnabled(false) {
}

void GateMonitor::begin() {
    _lastState = _gateController->readState();
}

void GateMonitor::update() {
    GateState currentState = _gateController->readState();
    
    // Monitor state changes
    if (currentState != _lastState) {
        handleStateChange(currentState);
        _lastState = currentState;
    }
    
    // Check for timeout during operations
    checkOperationTimeout();
    
    // Check for auto-close
    checkAutoClose();
}

void GateMonitor::startOperation(OperationState operation, GateState expectedState) {
    _currentOperation = operation;
    _operationStartTime = millis();
    _expectedState = expectedState;
    _alertTriggered = false;
    
    String operationName = (operation == OPENING) ? "opening" : "closing";
    Serial.println("Gate " + operationName + " initiated - timeout monitoring started");
}

bool GateMonitor::isOperationInProgress() {
    return _currentOperation != IDLE;
}

OperationState GateMonitor::getCurrentOperation() {
    return _currentOperation;
}

void GateMonitor::enableAutoClose() {
    _gateOpenedTime = millis();
    _autoCloseEnabled = true;
    Serial.println("Auto-close timer started - gate will close in " + String(AUTO_CLOSE_DELAY/1000) + " seconds");
}

void GateMonitor::disableAutoClose() {
    _autoCloseEnabled = false;
}

bool GateMonitor::isAutoCloseEnabled() {
    return _autoCloseEnabled;
}

unsigned long GateMonitor::getAutoCloseRemainingTime() {
    if (!_autoCloseEnabled) {
        return 0;
    }
    unsigned long elapsed = millis() - _gateOpenedTime;
    return AUTO_CLOSE_DELAY > elapsed ? AUTO_CLOSE_DELAY - elapsed : 0;
}

bool GateMonitor::isAlertActive() {
    return _alertTriggered;
}

void GateMonitor::clearAlert() {
    _alertTriggered = false;
}

unsigned long GateMonitor::getOperationElapsedTime() {
    if (_currentOperation == IDLE) {
        return 0;
    }
    return millis() - _operationStartTime;
}

unsigned long GateMonitor::getOperationRemainingTime() {
    if (_currentOperation == IDLE) {
        return 0;
    }
    unsigned long elapsed = getOperationElapsedTime();
    unsigned long timeout = (_currentOperation == OPENING) ? OPENING_TIMEOUT : CLOSING_TIMEOUT;
    return timeout > elapsed ? timeout - elapsed : 0;
}

void GateMonitor::handleStateChange(GateState currentState) {
    printStateChange(currentState);
    
    switch(currentState) {
        case CLOSED: 
            // Reset auto-close when gate is closed
            disableAutoClose();
            break;
        case OPEN: 
            // Start auto-close timer when gate becomes open
            enableAutoClose();
            break;
        case UNKNOWN: 
            break;
    }
    
    // Check if operation completed successfully
    if (_currentOperation != IDLE && currentState == _expectedState) {
        Serial.println("Operation completed successfully");
        _currentOperation = IDLE;
        _alertTriggered = false;
    }
}

void GateMonitor::checkOperationTimeout() {
    if (_currentOperation != IDLE) {
        unsigned long elapsed = getOperationElapsedTime();
        unsigned long timeout = (_currentOperation == OPENING) ? OPENING_TIMEOUT : CLOSING_TIMEOUT;
        
        if (elapsed >= timeout && !_alertTriggered) {
            String operationName = (_currentOperation == OPENING) ? "opening" : "closing";
            String alertMessage = "Timeout: Gate " + operationName + " did not complete within " + String(timeout/1000) + " seconds";
            triggerAlert(alertMessage);
        }
    }
}

void GateMonitor::checkAutoClose() {
    if (_autoCloseEnabled && _gateController->readState() == OPEN && _currentOperation == IDLE) {
        unsigned long elapsed = millis() - _gateOpenedTime;
        if (elapsed >= AUTO_CLOSE_DELAY) {
            Serial.println("Auto-close triggered - closing gate after " + String(AUTO_CLOSE_DELAY/1000) + " seconds");
            
            // Initiate closing
            _gateController->triggerRelay();
            startOperation(CLOSING, CLOSED);
            disableAutoClose();  // Disable auto-close to prevent repeated triggers
        }
    }
}

void GateMonitor::triggerAlert(const String& message) {
    Serial.println("ALERT: " + message);
    _alertTriggered = true;
    // Here you could add other alert mechanisms (email, push notification, etc.)
}

void GateMonitor::printStateChange(GateState state) {
    Serial.print("State change detected: ");
    switch(state) {
        case CLOSED: Serial.println("CLOSED"); break;
        case OPEN: Serial.println("OPEN"); break;
        case UNKNOWN: Serial.println("UNKNOWN"); break;
    }
}