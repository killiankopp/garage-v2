#!/bin/bash

# Script pour compiler avec VS Code et PlatformIO IDE
# Usage: ./build_vscode.sh

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}🔧 Configuration WiFi ESP32 (VS Code)${NC}"

# Vérifier si le fichier .env existe
if [ -f ".env" ]; then
    echo -e "${GREEN}📄 Chargement des variables depuis .env${NC}"
    # Charger les variables du fichier .env
    export $(cat .env | grep -v '^#' | xargs)
else
    echo -e "${YELLOW}⚠️  Fichier .env non trouvé${NC}"
    echo "Créez un fichier .env avec :"
    echo "WIFI_SSID=VotreSSID"
    echo "WIFI_PASSWORD=VotrePassword"
    exit 1
fi

# Vérifier que les variables sont définies
if [ -z "$WIFI_SSID" ] || [ -z "$WIFI_PASSWORD" ]; then
    echo -e "${RED}❌ Variables WiFi non définies !${NC}"
    exit 1
fi

echo -e "${GREEN}📡 SSID configuré: $WIFI_SSID${NC}"
echo -e "${GREEN}🔒 Mot de passe: ${WIFI_PASSWORD:0:3}***${NC}"

# Exécuter la commande demandée via VS Code
case "${1:-build}" in
    "build"|"")
        echo -e "${YELLOW}🔨 Compilation avec VS Code...${NC}"
        code --command "platformio-ide.build"
        ;;
    "upload")
        echo -e "${YELLOW}📤 Upload vers ESP32 avec VS Code...${NC}"
        code --command "platformio-ide.upload"
        ;;
    "monitor")
        echo -e "${YELLOW}📺 Monitoring avec VS Code...${NC}"
        code --command "platformio-ide.upload-and-monitor"
        ;;
    "clean")
        echo -e "${YELLOW}🧹 Nettoyage avec VS Code...${NC}"
        code --command "platformio-ide.clean"
        ;;
    *)
        echo "Usage: $0 [build|upload|monitor|clean]"
        echo "Note: Ce script utilise VS Code avec l'extension PlatformIO IDE"
        exit 1
        ;;
esac