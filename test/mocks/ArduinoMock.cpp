#ifdef UNIT_TEST

#include "ArduinoMock.h"
#include <iostream>
#include <map>

// Global state
std::map<int, int> pinModes;
std::map<int, int> pinValues;
unsigned long mockMillis = 0;

SerialMock Serial;

// Mock Arduino functions
void pinMode(int pin, int mode) {
    pinModes[pin] = mode;
}

void digitalWrite(int pin, int value) {
    pinValues[pin] = value;
}

int digitalRead(int pin) {
    auto it = pinValues.find(pin);
    if (it != pinValues.end()) {
        return it->second;
    }
    return HIGH; // Default pull-up value
}

void delay(unsigned long ms) {
    // In tests, we don't actually delay
    mockMillis += ms;
}

unsigned long millis() {
    return mockMillis;
}

// Serial mock implementation
void SerialMock::begin(unsigned long baud) {
    std::cout << "[Serial] Begin with baud: " << baud << std::endl;
}

void SerialMock::print(const std::string& str) {
    std::cout << str;
}

void SerialMock::println(const std::string& str) {
    std::cout << str << std::endl;
}

void SerialMock::println() {
    std::cout << std::endl;
}

// Helper functions
void resetMockState() {
    pinModes.clear();
    pinValues.clear();
    mockMillis = 0;
}

void setMockMillis(unsigned long ms) {
    mockMillis = ms;
}

void setMockPinValue(int pin, int value) {
    pinValues[pin] = value;
}

#endif // UNIT_TEST