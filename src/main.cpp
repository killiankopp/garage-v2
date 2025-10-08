#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "REPLACE_ME";
const char* password = "REPLACE_ME";

#define RELAY_1_PIN 16
#define SENSOR_CLOSED_PIN 18  // Sensor for "closed" position
#define SENSOR_OPEN_PIN 19    // Sensor for "open" position

// Timing configuration (in milliseconds)
const unsigned long OPENING_TIMEOUT = 15000;  // 15 seconds to open
const unsigned long CLOSING_TIMEOUT = 15000;  // 15 seconds to close
const unsigned long AUTO_CLOSE_DELAY = 180000;  // 3 minutes auto-close delay

// Create server on port 80
WebServer server(80);

// Possible gate states
enum GateState {
  CLOSED,
  OPEN,
  UNKNOWN
};

// Operation states for timeout monitoring
enum OperationState {
  IDLE,
  OPENING,
  CLOSING
};

GateState lastState = UNKNOWN;
OperationState currentOperation = IDLE;
unsigned long operationStartTime = 0;
GateState expectedState = UNKNOWN;
bool alertTriggered = false;

// Auto-close mechanism
unsigned long gateOpenedTime = 0;
bool autoCloseEnabled = false;

void handleRoot() {
  server.send(200, "text/plain", "ESP32 garage online!");
}

void handleHealth() {
  server.send(200, "text/plain", "OK");
}

void handleRelay1Impulse() {
  digitalWrite(RELAY_1_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_1_PIN, LOW);
}

// Route to open the gate
void handleGateOpen() {
  GateState currentState = readGateState();
  
  if (currentState != OPEN) {
    handleRelay1Impulse();
    
    // Start timeout monitoring
    currentOperation = OPENING;
    operationStartTime = millis();
    expectedState = OPEN;
    alertTriggered = false;
    
    Serial.println("Gate opening initiated - timeout monitoring started");
  }
  
  // Return current status in JSON format
  handleGateStatus();
}

// Route to close the gate
void handleGateClose() {
  GateState currentState = readGateState();
  
  if (currentState != CLOSED) {
    handleRelay1Impulse();
    
    // Start timeout monitoring
    currentOperation = CLOSING;
    operationStartTime = millis();
    expectedState = CLOSED;
    alertTriggered = false;
    
    Serial.println("Gate closing initiated - timeout monitoring started");
  }
  
  // Return current status in JSON format
  handleGateStatus();
}

// Function to read gate position
GateState readGateState() {
  // When gate is NEAR sensor: circuit closes -> digitalRead() = LOW -> sensor active = true
  // When gate is FAR from sensor: circuit opens -> digitalRead() = HIGH (pull-up) -> sensor active = false
  bool sensorClosed = !digitalRead(SENSOR_CLOSED_PIN);  // true when gate is near "closed" sensor
  bool sensorOpen = !digitalRead(SENSOR_OPEN_PIN);      // true when gate is near "open" sensor
  
  if (sensorClosed && !sensorOpen) {
    return CLOSED;
  } else if (!sensorClosed && sensorOpen) {
    return OPEN;
  } else {
    return UNKNOWN;  // None or both sensors activated
  }
}

// Function to trigger alert
void triggerAlert(const String& message) {
  Serial.println("ALERT: " + message);
  alertTriggered = true;
  // Here you could add other alert mechanisms (email, push notification, etc.)
}

// Route to get detailed state (JSON)
void handleGateStatus() {
  bool sensorClosed = !digitalRead(SENSOR_CLOSED_PIN);  // true when gate is near "closed" sensor
  bool sensorOpen = !digitalRead(SENSOR_OPEN_PIN);      // true when gate is near "open" sensor
  GateState state = readGateState();
  
  String json = "{";
  json += "\"status\":\"";
  
  // Priority: ongoing operations override physical state
  if (currentOperation == OPENING) {
    json += "opening";
  } else if (currentOperation == CLOSING) {
    json += "closing";
  } else {
    // No operation, return physical state
    switch(state) {
      case CLOSED: json += "closed"; break;
      case OPEN: json += "open"; break;
      case UNKNOWN: json += "unknown"; break;
    }
  }
  
  json += "\",";
  json += "\"sensor_closed\":" + String(sensorClosed ? "true" : "false") + ",";
  json += "\"sensor_open\":" + String(sensorOpen ? "true" : "false") + ",";
  
  if (currentOperation != IDLE) {
    unsigned long elapsed = millis() - operationStartTime;
    json += "\"operation_time\":" + String(elapsed) + ",";
    unsigned long timeout = (currentOperation == OPENING) ? OPENING_TIMEOUT : CLOSING_TIMEOUT;
    json += "\"timeout_remaining\":" + String(timeout > elapsed ? timeout - elapsed : 0) + ",";
  }
  
  json += "\"alert_active\":" + String(alertTriggered ? "true" : "false") + ",";
  json += "\"auto_close_enabled\":" + String(autoCloseEnabled ? "true" : "false");
  
  if (autoCloseEnabled && state == OPEN && currentOperation == IDLE) {
    unsigned long elapsed = millis() - gateOpenedTime;
    json += ",\"auto_close_time\":" + String(elapsed);
    json += ",\"auto_close_remaining\":" + String(AUTO_CLOSE_DELAY > elapsed ? AUTO_CLOSE_DELAY - elapsed : 0);
  }
  
  json += "}";
  
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  
  // Relay configuration
  pinMode(RELAY_1_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW); // Relay off by default
  
  // Sensor configuration with internal pull-up
  pinMode(SENSOR_CLOSED_PIN, INPUT_PULLUP);
  pinMode(SENSOR_OPEN_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected, IP: ");
  Serial.println(WiFi.localIP());

  // HTTP routes
  server.on("/", handleRoot);
  server.on("/health", handleHealth);
  server.on("/gate/open", handleGateOpen);
  server.on("/gate/close", handleGateClose);
  server.on("/gate/status", handleGateStatus);

  server.begin();
  Serial.println("HTTP server started");
  
  // Display initial state
  Serial.print("Initial gate state: ");
  GateState initialState = readGateState();
  switch(initialState) {
    case CLOSED: Serial.println("CLOSED"); break;
    case OPEN: Serial.println("OPEN"); break;
    case UNKNOWN: Serial.println("UNKNOWN"); break;
  }
}

void loop() {
  server.handleClient();
  
  // Monitor state changes
  GateState currentState = readGateState();
  if (currentState != lastState) {
    Serial.print("State change detected: ");
    switch(currentState) {
      case CLOSED: 
        Serial.println("CLOSED"); 
        // Reset auto-close when gate is closed
        autoCloseEnabled = false;
        break;
      case OPEN: 
        Serial.println("OPEN");
        // Start auto-close timer when gate becomes open
        gateOpenedTime = millis();
        autoCloseEnabled = true;
        Serial.println("Auto-close timer started - gate will close in " + String(AUTO_CLOSE_DELAY/1000) + " seconds");
        break;
      case UNKNOWN: 
        Serial.println("UNKNOWN"); 
        break;
    }
    lastState = currentState;
    
    // Check if operation completed successfully
    if (currentOperation != IDLE && currentState == expectedState) {
      Serial.println("Operation completed successfully");
      currentOperation = IDLE;
      alertTriggered = false;
    }
  }
  
  // Check for timeout during operations
  if (currentOperation != IDLE) {
    unsigned long elapsed = millis() - operationStartTime;
    unsigned long timeout = (currentOperation == OPENING) ? OPENING_TIMEOUT : CLOSING_TIMEOUT;
    
    if (elapsed >= timeout && !alertTriggered) {
      String operationName = (currentOperation == OPENING) ? "opening" : "closing";
      String alertMessage = "Timeout: Gate " + operationName + " did not complete within " + String(timeout/1000) + " seconds";
      triggerAlert(alertMessage);
    }
  }
  
  // Check for auto-close
  if (autoCloseEnabled && currentState == OPEN && currentOperation == IDLE) {
    unsigned long elapsed = millis() - gateOpenedTime;
    if (elapsed >= AUTO_CLOSE_DELAY) {
      Serial.println("Auto-close triggered - closing gate after " + String(AUTO_CLOSE_DELAY/1000) + " seconds");
      
      // Initiate closing
      handleRelay1Impulse();
      currentOperation = CLOSING;
      operationStartTime = millis();
      expectedState = CLOSED;
      alertTriggered = false;
      autoCloseEnabled = false;  // Disable auto-close to prevent repeated triggers
    }
  }
  
  delay(100); // Small pause to avoid spam
}