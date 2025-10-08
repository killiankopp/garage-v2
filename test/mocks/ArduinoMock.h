#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

#ifdef UNIT_TEST

#include <iostream>
#include <string>
#include <map>

// Mock Arduino types and constants
typedef bool boolean;
typedef unsigned char byte;
typedef std::string String;

#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// Mock functions
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
int digitalRead(int pin);
void delay(unsigned long ms);
unsigned long millis();

// Mock Serial
class SerialMock {
public:
    void begin(unsigned long baud);
    void print(const std::string& str);
    void println(const std::string& str);
    void println();
};

extern SerialMock Serial;

// Pin state tracking for tests
extern std::map<int, int> pinModes;
extern std::map<int, int> pinValues;
extern unsigned long mockMillis;

// Helper functions for tests
void resetMockState();
void setMockMillis(unsigned long ms);
void setMockPinValue(int pin, int value);

#endif // UNIT_TEST

#endif // ARDUINO_MOCK_H