#include "WiFiManager.h"
#include <Arduino.h>

WiFiManager::WiFiManager(const char* ssid, const char* password) 
    : _ssid(ssid), _password(password) {
}

void WiFiManager::begin() {
    WiFi.begin(_ssid, _password);
    Serial.print("WiFi connecting");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    printConnectionStatus();
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getLocalIP() {
    return WiFi.localIP().toString();
}

void WiFiManager::printConnectionStatus() {
    Serial.println("");
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
}