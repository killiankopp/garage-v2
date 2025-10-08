#ifndef GATE_MONITOR_H
#define GATE_MONITOR_H

#ifdef UNIT_TEST
#include "../../test/mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#endif

#include "GateTypes.h"
#include "GateController.h"

class GateMonitor {
public:
    GateMonitor(GateController* gateController);
    void begin();
    void update();
    
    // Operation management
    void startOperation(OperationState operation, GateState expectedState);
    bool isOperationInProgress();
    OperationState getCurrentOperation();
    
    // Auto-close management
    void enableAutoClose();
    void disableAutoClose();
    bool isAutoCloseEnabled();
    unsigned long getAutoCloseRemainingTime();
    
    // Alert management
    bool isAlertActive();
    void clearAlert();
    
    // Status information
    unsigned long getOperationElapsedTime();
    unsigned long getOperationRemainingTime();

private:
    GateController* _gateController;
    GateState _lastState;
    OperationState _currentOperation;
    unsigned long _operationStartTime;
    GateState _expectedState;
    bool _alertTriggered;
    
    // Auto-close mechanism
    unsigned long _gateOpenedTime;
    bool _autoCloseEnabled;
    
    void handleStateChange(GateState currentState);
    void checkOperationTimeout();
    void checkAutoClose();
    void triggerAlert(const String& message);
    void printStateChange(GateState state);
};

#endif // GATE_MONITOR_H