#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager(const char* ssid, const char* password);
    void begin();
    bool isConnected();
    String getLocalIP();

private:
    const char* _ssid;
    const char* _password;
    void printConnectionStatus();
};

#endif // WIFI_MANAGER_H