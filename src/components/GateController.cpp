#include "GateController.h"
#include "Config.h"

#ifdef UNIT_TEST
#include "../../test/mocks/ArduinoMock.h"
#else
#include <Arduino.h>
#endif

GateController::GateController() {
}

void GateController::begin() {
    initializePins();
    printInitialState();
}

void GateController::triggerRelay() {
    digitalWrite(RELAY_1_PIN, HIGH);
    delay(1000);
    digitalWrite(RELAY_1_PIN, LOW);
}

GateState GateController::readState() {
    // When gate is NEAR sensor: circuit closes -> digitalRead() = LOW -> sensor active = true
    // When gate is FAR from sensor: circuit opens -> digitalRead() = HIGH (pull-up) -> sensor active = false
    bool sensorClosed = isClosedSensorActive();
    bool sensorOpen = isOpenSensorActive();
    
    if (sensorClosed && !sensorOpen) {
        return CLOSED;
    } else if (!sensorClosed && sensorOpen) {
        return OPEN;
    } else {
        return UNKNOWN;  // None or both sensors activated
    }
}

bool GateController::isClosedSensorActive() {
    return !digitalRead(SENSOR_CLOSED_PIN);  // true when gate is near "closed" sensor
}

bool GateController::isOpenSensorActive() {
    return !digitalRead(SENSOR_OPEN_PIN);    // true when gate is near "open" sensor
}

void GateController::initializePins() {
    // Relay configuration
    pinMode(RELAY_1_PIN, OUTPUT);
    digitalWrite(RELAY_1_PIN, LOW); // Relay off by default
    
    // Sensor configuration with internal pull-up
    pinMode(SENSOR_CLOSED_PIN, INPUT_PULLUP);
    pinMode(SENSOR_OPEN_PIN, INPUT_PULLUP);
}

void GateController::printInitialState() {
    Serial.print("Initial gate state: ");
    GateState initialState = readState();
    switch(initialState) {
        case CLOSED: Serial.println("CLOSED"); break;
        case OPEN: Serial.println("OPEN"); break;
        case UNKNOWN: Serial.println("UNKNOWN"); break;
    }
}