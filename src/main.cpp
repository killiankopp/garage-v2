#include <Arduino.h>
#include "components/Config.h"
#include "components/WiFiManager.h"
#include "components/GateController.h"
#include "components/GateMonitor.h"
#include "components/WebServerHandler.h"

#ifndef WIFI_SSID
#define WIFI_SSID "REPLACE_ME"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "REPLACE_ME"
#endif

// Component instances
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD);
GateController gateController;
GateMonitor gateMonitor(&gateController);
WebServerHandler webServer(&gateController, &gateMonitor);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    
    // Initialize components
    gateController.begin();
    wifiManager.begin();
    gateMonitor.begin();
    webServer.begin();
    
    Serial.println("System initialization complete");
}

void loop() {
    // Handle web server requests
    webServer.handleClient();
    
    // Update gate monitoring
    gateMonitor.update();
    
    // Small delay to avoid excessive CPU usage
    delay(MAIN_LOOP_DELAY);
}