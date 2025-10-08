#ifndef GATE_CONTROLLER_H
#define GATE_CONTROLLER_H

#ifdef UNIT_TEST
#include "../../test/mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#endif

#include "GateTypes.h"

class GateController {
public:
    GateController();
    void begin();
    void triggerRelay();
    GateState readState();
    bool isClosedSensorActive();
    bool isOpenSensorActive();

private:
    void initializePins();
    void printInitialState();
};

#endif // GATE_CONTROLLER_H