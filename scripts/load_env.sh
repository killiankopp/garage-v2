#!/bin/bash

# Script pour charger les variables d'environnement depuis le fichier .env
# Usage: source load_env.sh && pio run

# Vérifier si le fichier .env existe
if [ ! -f .env ]; then
    echo "Erreur: Le fichier .env n'existe pas"
    echo "Copiez .env.example vers .env et configurez vos valeurs"
    exit 1
fi

# Charger les variables depuis .env, en ignorant les commentaires et lignes vides
export $(grep -v '^#' .env | grep -v '^$' | xargs)

echo "Variables d'environnement chargées depuis .env:"
echo "- WIFI_SSID: ${WIFI_SSID}"
echo "- WIFI_PASSWORD: [masqué]"
echo "- KEYCLOAK_SERVER_URL: ${KEYCLOAK_SERVER_URL}"
echo "- KEYCLOAK_REALM: ${KEYCLOAK_REALM}"
echo "- KEYCLOAK_CLIENT_ID: ${KEYCLOAK_CLIENT_ID}"
echo "- KAFKA_BROKER_URL: ${KAFKA_BROKER_URL}"
echo "- KAFKA_TOPIC: ${KAFKA_TOPIC}"
echo "- KAFKA_UNAUTHORIZED_TOPIC: ${KAFKA_UNAUTHORIZED_TOPIC}"

echo ""
echo "Vous pouvez maintenant lancer : pio run"