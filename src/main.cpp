#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "REPLACE_ME";
const char* password = "REPLACE_ME";

#define RELAY_PIN 16
#define SENSOR_CLOSED_PIN 18  // Sensor for "closed" position
#define SENSOR_OPEN_PIN 19    // Sensor for "open" position

// Create server on port 80
WebServer server(80);

// Possible gate states
enum GateState {
  CLOSED,
  OPEN,
  UNKNOWN
};

GateState lastState = UNKNOWN;

void handleRoot() {
  server.send(200, "text/plain", "ESP32 online!");
}

void handleRelayImpulse() {
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
  server.send(200, "text/plain", "Relay activated");
}

// Function to read gate position
GateState readGateState() {
  bool sensorClosed = !digitalRead(SENSOR_CLOSED_PIN);  // Inverted due to pull-up
  bool sensorOpen = !digitalRead(SENSOR_OPEN_PIN);      // Inverted due to pull-up
  
  if (sensorClosed && !sensorOpen) {
    return CLOSED;
  } else if (!sensorClosed && sensorOpen) {
    return OPEN;
  } else {
    return UNKNOWN;  // None or both sensors activated
  }
}

// Route to get gate state (simple text)
void handleGateState() {
  GateState state = readGateState();
  String response;
  
  switch(state) {
    case CLOSED:
      response = "closed";
      break;
    case OPEN:
      response = "open";
      break;
    case UNKNOWN:
      response = "unknown";
      break;
  }
  
  server.send(200, "text/plain", response);
}

// Route to get detailed state (JSON)
void handleGateStatus() {
  bool sensorClosed = !digitalRead(SENSOR_CLOSED_PIN);
  bool sensorOpen = !digitalRead(SENSOR_OPEN_PIN);
  GateState state = readGateState();
  
  String json = "{";
  json += "\"status\":\"";
  switch(state) {
    case CLOSED: json += "closed"; break;
    case OPEN: json += "open"; break;
    case UNKNOWN: json += "unknown"; break;
  }
  json += "\",";
  json += "\"sensor_closed\":" + String(sensorClosed ? "true" : "false") + ",";
  json += "\"sensor_open\":" + String(sensorOpen ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  
  // Relay configuration
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relay off by default
  
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
  server.on("/relay/1/impulse", handleRelayImpulse);
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
  
  // Monitor state changes (optional)
  GateState currentState = readGateState();
  if (currentState != lastState) {
    Serial.print("State change detected: ");
    switch(currentState) {
      case CLOSED: Serial.println("CLOSED"); break;
      case OPEN: Serial.println("OPEN"); break;
      case UNKNOWN: Serial.println("UNKNOWN"); break;
    }
    lastState = currentState;
  }
  
  delay(100); // Small pause to avoid spam
}