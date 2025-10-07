#!/bin/bash

# Script pour compiler avec les variables d'environnement WiFi
# Usage: ./build.sh

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}🔧 Configuration WiFi ESP32${NC}"

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
    
    # Demander à l'utilisateur
    read -p "WIFI_SSID: " WIFI_SSID
    read -s -p "WIFI_PASSWORD: " WIFI_PASSWORD
    echo
    
    export WIFI_SSID
    export WIFI_PASSWORD
fi

# Vérifier que les variables sont définies
if [ -z "$WIFI_SSID" ] || [ -z "$WIFI_PASSWORD" ]; then
    echo -e "${RED}❌ Variables WiFi non définies !${NC}"
    exit 1
fi

echo -e "${GREEN}📡 SSID configuré: $WIFI_SSID${NC}"
echo -e "${GREEN}🔒 Mot de passe: ${WIFI_PASSWORD:0:3}***${NC}"

# Détecter PlatformIO CLI
PIO_CMD=""
if command -v pio &> /dev/null; then
    PIO_CMD="pio"
elif command -v platformio &> /dev/null; then
    PIO_CMD="platformio"
elif [ -f ~/.platformio/penv/bin/pio ]; then
    PIO_CMD="~/.platformio/penv/bin/pio"
elif [ -f ~/.platformio/penv/bin/platformio ]; then
    PIO_CMD="~/.platformio/penv/bin/platformio"
else
    echo -e "${RED}❌ PlatformIO CLI non trouvé !${NC}"
    echo "Installez PlatformIO :"
    echo "  curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py"
    echo "  python3 get-platformio.py"
    echo ""
    echo "Ou utilisez VS Code avec l'extension PlatformIO"
    exit 1
fi

echo -e "${GREEN}🔧 PlatformIO trouvé: $PIO_CMD${NC}"

# Exécuter la commande demandée
case "${1:-build}" in
    "build"|"")
        echo -e "${YELLOW}🔨 Compilation...${NC}"
        $PIO_CMD run
        ;;
    "upload")
        echo -e "${YELLOW}📤 Upload vers ESP32...${NC}"
        $PIO_CMD run --target upload
        ;;
    "monitor")
        echo -e "${YELLOW}📺 Monitoring...${NC}"
        $PIO_CMD run --target upload --target monitor
        ;;
    "clean")
        echo -e "${YELLOW}🧹 Nettoyage...${NC}"
        $PIO_CMD run --target clean
        ;;
    *)
        echo "Usage: $0 [build|upload|monitor|clean]"
        exit 1
        ;;
esac