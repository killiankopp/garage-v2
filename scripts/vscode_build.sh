#!/bin/bash

# Script pour VS Code avec PlatformIO IDE
# Usage: ./scripts/vscode_build.sh [command]

# Se déplacer vers le répertoire du script pour éviter les problèmes de chemin
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔧 ESP32 Garage Controller (VS Code)${NC}"

# Fonction pour détecter et configurer PlatformIO
detect_platformio() {
    # Détecter si on est dans le dossier scripts
    if [ -f "../.venv/bin/activate" ]; then
        VENV_PATH="../.venv/bin/activate"
    elif [ -f ".venv/bin/activate" ]; then
        VENV_PATH=".venv/bin/activate"
    fi

    # Activer l'environnement virtuel si disponible
    if [ -n "$VENV_PATH" ] && [ -f "$VENV_PATH" ]; then
        echo -e "${BLUE}🐍 Activation de l'environnement virtuel...${NC}"
        source $VENV_PATH
    fi

    # Essayer différentes méthodes pour trouver PlatformIO
    if command -v pio &> /dev/null; then
        PIO_CMD="pio"
        echo -e "${GREEN}✅ PlatformIO CLI trouvé: $PIO_CMD${NC}"
        return 0
    elif command -v platformio &> /dev/null; then
        PIO_CMD="platformio"
        echo -e "${GREEN}✅ PlatformIO CLI trouvé: $PIO_CMD${NC}"
        return 0
    elif python3 -m platformio --help &> /dev/null; then
        PIO_CMD="python3 -m platformio"
        echo -e "${GREEN}✅ PlatformIO CLI trouvé: $PIO_CMD${NC}"
        return 0
    else
        echo -e "${RED}❌ PlatformIO CLI n'est pas installé !${NC}"
        echo ""
        echo "Solutions possibles :"
        echo "1. Installer via pip : pip3 install platformio"
        echo "2. Installer via brew : brew install platformio"
        echo "3. Utiliser VS Code avec l'extension PlatformIO"
        echo ""
        echo "Pour VS Code, ouvrez le projet et utilisez les commandes de l'extension."
        return 1
    fi
}

# Fonction pour charger les variables d'environnement
load_env() {
    # Détecter si on est dans le dossier scripts ou à la racine
    if [ -f ".env" ]; then
        ENV_FILE=".env"
    elif [ -f "../.env" ]; then
        ENV_FILE="../.env"
    else
        echo -e "${YELLOW}⚠️  Fichier .env non trouvé${NC}"
        echo "Créez un fichier .env à la racine du projet avec les variables nécessaires"
        return 1
    fi

    echo -e "${GREEN}📄 Chargement des variables depuis $ENV_FILE${NC}"
    export $(cat $ENV_FILE | grep -v '^#' | xargs)

    if [ -z "$WIFI_SSID" ] || [ -z "$WIFI_PASSWORD" ]; then
        echo -e "${RED}❌ Variables WiFi non définies !${NC}"
        return 1
    fi

    echo -e "${GREEN}📡 Configuration WiFi chargée${NC}"
    return 0
}

# Détecter PlatformIO
if ! detect_platformio; then
    exit 1
fi

# Charger l'environnement
if ! load_env; then
    exit 1
fi

# Naviguer vers le dossier racine du projet si nécessaire
if [ ! -f "platformio.ini" ] && [ -f "../platformio.ini" ]; then
    cd ..
fi

# Pour VS Code, on utilise directement PlatformIO CLI
# car les commandes VS Code via CLI ne sont pas fiables
case "${1:-build}" in
    "build"|"")
        echo -e "${YELLOW}🔨 Compilation...${NC}"
        $PIO_CMD run
        ;;
    "upload")
        echo -e "${YELLOW}📤 Upload...${NC}"
        $PIO_CMD run --target upload
        ;;
    "monitor")
        echo -e "${YELLOW}📺 Monitoring...${NC}"
        # Utiliser PlatformIO monitor au lieu des commandes VS Code
        $PIO_CMD device monitor --baud 115200
        ;;
    "flash")
        echo -e "${YELLOW}🚀 Upload et monitoring...${NC}"
        $PIO_CMD run --target upload && sleep 2 && $PIO_CMD device monitor --baud 115200
        ;;
    "clean")
        echo -e "${YELLOW}🧹 Nettoyage...${NC}"
        $PIO_CMD run --target clean
        ;;
    *)
        echo "Usage: $0 [build|upload|monitor|flash|clean]"
        echo ""
        echo "Note: Ce script utilise PlatformIO CLI directement"
        echo "pour une meilleure compatibilité avec VS Code"
        echo ""
        echo "PlatformIO détecté: $PIO_CMD"
        exit 1
        ;;
esac