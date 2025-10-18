#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#ifdef UNIT_TEST

#include <iostream>
#include <string>

class String {
public:
    String() = default;
    String(const char* value) : _value(value ? value : "") {}
    String(const std::string& value) : _value(value) {}

    bool isEmpty() const { return _value.empty(); }
    const char* c_str() const { return _value.c_str(); }
    std::size_t length() const { return _value.length(); }

    String operator+(const String& other) const {
        return String(_value + other._value);
    }

    String& operator=(const String& other) {
        _value = other._value;
        return *this;
    }

    String& operator=(const char* value) {
        _value = value ? value : "";
        return *this;
    }

    bool operator==(const String& other) const { return _value == other._value; }
    bool operator!=(const String& other) const { return _value != other._value; }

    bool operator==(const char* other) const {
        return _value == (other ? std::string(other) : std::string());
    }

    bool operator!=(const char* other) const { return !(*this == other); }

private:
    std::string _value;
};

inline String operator+(const char* lhs, const String& rhs) {
    return String(lhs) + rhs;
}

class SerialStub {
public:
    template <typename T>
    void println(const T& value) {
        std::cout << value << std::endl;
    }

    template <typename T>
    void print(const T& value) {
        std::cout << value;
    }

    void println() {
        std::cout << std::endl;
    }
};

inline SerialStub Serial;

#endif // UNIT_TEST

#endif // ARDUINO_STUB_H
