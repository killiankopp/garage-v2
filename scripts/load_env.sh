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
echo "- KEYCLOAK_CLIENT_SECRET: [masqué]"
echo "- EMQX_BROKER_HOST: ${EMQX_BROKER_HOST}"
echo "- EMQX_BROKER_PORT: ${EMQX_BROKER_PORT}"
echo "- EMQX_USERNAME: ${EMQX_USERNAME}"
echo "- EMQX_PASSWORD: [masqué]"
echo "- EMQX_TOPIC: ${EMQX_TOPIC}"
echo "- EMQX_UNAUTHORIZED_TOPIC: ${EMQX_UNAUTHORIZED_TOPIC}"

echo ""
echo "Vous pouvez maintenant lancer : pio run"