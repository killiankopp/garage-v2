#!/bin/bash

# Script de test pour la fonctionnalité Kafka logging
# Ce script simule des requêtes HTTP vers l'ESP32 pour tester le logging

ESP32_IP="192.168.1.100"  # À ajuster selon votre configuration
ESP32_PORT="80"

echo "Test du logging Kafka - ESP32 Garage"
echo "====================================="

# Test 1: Action autorisée avec token valide
echo "Test 1: Action d'ouverture autorisée"
curl -X GET "http://${ESP32_IP}:${ESP32_PORT}/gate/open" \
  -H "Authorization: Bearer YOUR_VALID_JWT_TOKEN_HERE" \
  -H "Content-Type: application/json"

echo -e "\n\nTest 2: Action de fermeture autorisée"
curl -X GET "http://${ESP32_IP}:${ESP32_PORT}/gate/close" \
  -H "Authorization: Bearer YOUR_VALID_JWT_TOKEN_HERE" \
  -H "Content-Type: application/json"

# Test 3: Action non autorisée sans token
echo -e "\n\nTest 3: Action d'ouverture non autorisée (sans token)"
curl -X GET "http://${ESP32_IP}:${ESP32_PORT}/gate/open"

# Test 4: Action non autorisée avec token invalide
echo -e "\n\nTest 4: Action d'ouverture non autorisée (token invalide)"
curl -X GET "http://${ESP32_IP}:${ESP32_PORT}/gate/open" \
  -H "Authorization: Bearer invalid.jwt.token"

echo -e "\n\nTests terminés. Vérifiez les logs Kafka sur les topics 'garage' et 'garage-unauthorized'"