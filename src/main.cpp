#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "REPLACE_ME";
const char* password = "REPLACE_ME";

#define RELAY_PIN 16

// Création du serveur sur le port 80
WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "ESP32 en ligne !");
}

void handleRelayImpulse() {
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
  server.send(200, "text/plain", "Relais impulsé");
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // relais éteint par défaut

  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connecté, IP: ");
  Serial.println(WiFi.localIP());

  // Routes HTTP
  server.on("/", handleRoot);
  server.on("/relay/1/impulse", handleRelayImpulse);

  server.begin();
  Serial.println("Serveur HTTP démarré");
}

void loop() {
  server.handleClient();
}