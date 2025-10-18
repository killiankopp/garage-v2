#include "WiFiManager.h"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <time.h>

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
    
    // Enable mDNS for .local and .lan hostname resolution
    if (MDNS.begin("esp32-garage")) {
        Serial.println("mDNS responder started (esp32-garage.local)");
    } else {
        Serial.println("mDNS setup failed");
    }
    
    // Synchronize time via NTP (critical for JWT validation)
    Serial.println("Synchronizing time with NTP...");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    
    // Wait for time to be set (max 10 seconds)
    int retry = 0;
    const int retry_count = 20;
    while (time(nullptr) < 24 * 3600 && retry < retry_count) {
        Serial.print(".");
        delay(500);
        retry++;
    }
    Serial.println("");
    
    time_t now = time(nullptr);
    if (now > 24 * 3600) {
        struct tm timeinfo;
        gmtime_r(&now, &timeinfo);
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
        Serial.print("Time synchronized: ");
        Serial.print(timeStr);
        Serial.print(" (timestamp: ");
        Serial.print(now);
        Serial.println(")");
    } else {
        Serial.println("Warning: Time synchronization failed. JWT validation may fail!");
    }
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