#!/bin/bash#!/bin/bash



# Script pour compiler, uploader et monitorer l'ESP32 avec PlatformIO# Script pour compiler avec les variables d'environnement WiFi

# Usage: ./scripts/build.sh [build|upload|monitor|clean|test]# Usage: ./build.sh



# Se déplacer vers le répertoire du projet# Couleurs pour l'affichage

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"RED='\033[0;31m'

PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"GREEN='\033[0;32m'

cd "$PROJECT_ROOT"YELLOW='\033[1;33m'

NC='\033[0m' # No Color

# Couleurs pour l'affichage

RED='\033[0;31m'echo -e "${YELLOW}🔧 Configuration WiFi ESP32${NC}"

GREEN='\033[0;32m'

YELLOW='\033[1;33m'# Vérifier si le fichier .env existe

BLUE='\033[0;34m'if [ -f ".env" ]; then

NC='\033[0m' # No Color    echo -e "${GREEN}📄 Chargement des variables depuis .env${NC}"

    # Charger les variables du fichier .env

# Fonction pour afficher l'aide    export $(cat .env | grep -v '^#' | xargs)

show_help() {else

    echo -e "${BLUE}🔧 ESP32 Garage Controller - Script de build${NC}"    echo -e "${YELLOW}⚠️  Fichier .env non trouvé${NC}"

    echo ""    echo "Créez un fichier .env avec :"

    echo "Usage: $0 [COMMAND]"    echo "WIFI_SSID=VotreSSID"

    echo ""    echo "WIFI_PASSWORD=VotrePassword"

    echo "Commands:"    

    echo "  build     - Compile le projet"    # Demander à l'utilisateur

    echo "  upload    - Compile et upload vers l'ESP32"    read -p "WIFI_SSID: " WIFI_SSID

    echo "  monitor   - Lance le moniteur série"    read -s -p "WIFI_PASSWORD: " WIFI_PASSWORD

    echo "  flash     - Upload et lance immédiatement le moniteur"    echo

    echo "  clean     - Nettoie les fichiers de build"    

    echo "  test      - Lance les tests unitaires"    export WIFI_SSID

    echo "  help      - Affiche cette aide"    export WIFI_PASSWORD

    echo ""fi

}

# Vérifier que les variables sont définies

# Fonction pour détecter et configurer PlatformIOif [ -z "$WIFI_SSID" ] || [ -z "$WIFI_PASSWORD" ]; then

detect_platformio() {    echo -e "${RED}❌ Variables WiFi non définies !${NC}"

    # Activer l'environnement virtuel si disponible    exit 1

    if [ -f ".venv/bin/activate" ]; thenfi

        echo -e "${BLUE}🐍 Activation de l'environnement virtuel...${NC}"

        source .venv/bin/activateecho -e "${GREEN}📡 SSID configuré: $WIFI_SSID${NC}"

    fiecho -e "${GREEN}🔒 Mot de passe: ${WIFI_PASSWORD:0:3}***${NC}"



    # Essayer différentes méthodes pour trouver PlatformIO# Détecter PlatformIO CLI

    if command -v pio &> /dev/null; thenPIO_CMD=""

        PIO_CMD="pio"if command -v pio &> /dev/null; then

        echo -e "${GREEN}✅ PlatformIO CLI trouvé: $PIO_CMD${NC}"    PIO_CMD="pio"

        return 0elif command -v platformio &> /dev/null; then

    elif command -v platformio &> /dev/null; then    PIO_CMD="platformio"

        PIO_CMD="platformio"elif [ -f ~/.platformio/penv/bin/pio ]; then

        echo -e "${GREEN}✅ PlatformIO CLI trouvé: $PIO_CMD${NC}"    PIO_CMD="~/.platformio/penv/bin/pio"

        return 0elif [ -f ~/.platformio/penv/bin/platformio ]; then

    elif python3 -m platformio --help &> /dev/null; then    PIO_CMD="~/.platformio/penv/bin/platformio"

        PIO_CMD="python3 -m platformio"else

        echo -e "${GREEN}✅ PlatformIO CLI trouvé: $PIO_CMD${NC}"    echo -e "${RED}❌ PlatformIO CLI non trouvé !${NC}"

        return 0    echo "Installez PlatformIO :"

    else    echo "  curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py"

        echo -e "${RED}❌ PlatformIO CLI n'est pas installé !${NC}"    echo "  python3 get-platformio.py"

        echo ""    echo ""

        echo "Solutions possibles :"    echo "Ou utilisez VS Code avec l'extension PlatformIO"

        echo "1. Installer via pip : pip3 install platformio"    exit 1

        echo "2. Installer via brew : brew install platformio"fi

        echo ""

        echo "Le script va tenter d'installer PlatformIO via pip3..."echo -e "${GREEN}🔧 PlatformIO trouvé: $PIO_CMD${NC}"

        if command -v pip3 &> /dev/null; then

            pip3 install platformio# Exécuter la commande demandée

            if [ $? -eq 0 ]; thencase "${1:-build}" in

                echo -e "${GREEN}✅ PlatformIO installé avec succès${NC}"    "build"|"")

                PIO_CMD="pio"        echo -e "${YELLOW}🔨 Compilation...${NC}"

                return 0        $PIO_CMD run

            fi        ;;

        fi    "upload")

        echo -e "${RED}❌ Échec de l'installation automatique${NC}"        echo -e "${YELLOW}📤 Upload vers ESP32...${NC}"

        return 1        $PIO_CMD run --target upload

    fi        ;;

}    "monitor")

        echo -e "${YELLOW}📺 Monitoring...${NC}"

# Fonction pour charger les variables d'environnement        $PIO_CMD run --target upload --target monitor

load_env() {        ;;

    if [ -f ".env" ]; then    "clean")

        echo -e "${GREEN}📄 Chargement des variables depuis .env${NC}"        echo -e "${YELLOW}🧹 Nettoyage...${NC}"

        # Charger les variables du fichier .env        $PIO_CMD run --target clean

        export $(cat .env | grep -v '^#' | xargs)        ;;

    else    *)

        echo -e "${YELLOW}⚠️  Fichier .env non trouvé${NC}"        echo "Usage: $0 [build|upload|monitor|clean]"

        echo "Créez un fichier .env avec :"        exit 1

        echo "WIFI_SSID=VotreSSID"        ;;

        echo "WIFI_PASSWORD=VotrePassword"esac
        echo "KEYCLOAK_SERVER_URL=https://your-keycloak-server"
        echo "KEYCLOAK_REALM=your-realm"
        echo "KEYCLOAK_CLIENT_ID=your-client-id"
        echo "KAFKA_BROKER_URL=your-kafka-broker:9092"
        echo "KAFKA_TOPIC=garage-events"
        echo "KAFKA_UNAUTHORIZED_TOPIC=garage-unauthorized"
        return 1
    fi

    # Vérifier que les variables essentielles sont définies
    if [ -z "$WIFI_SSID" ] || [ -z "$WIFI_PASSWORD" ]; then
        echo -e "${RED}❌ Variables WiFi non définies !${NC}"
        return 1
    fi

    echo -e "${GREEN}📡 SSID configuré: $WIFI_SSID${NC}"
    echo -e "${GREEN}🔒 Mot de passe: ${WIFI_PASSWORD:0:3}***${NC}"
    return 0
}

# Vérifier que nous sommes dans le bon répertoire
if [ ! -f "platformio.ini" ]; then
    echo -e "${RED}❌ Fichier platformio.ini non trouvé !${NC}"
    echo "Assurez-vous d'être dans le répertoire racine du projet."
    exit 1
fi

# Détecter PlatformIO
if ! detect_platformio; then
    exit 1
fi

# Exécuter la commande demandée
case "${1:-build}" in
    "build"|"")
        echo -e "${YELLOW}🔨 Compilation du projet...${NC}"
        if load_env; then
            $PIO_CMD run
        fi
        ;;
    "upload")
        echo -e "${YELLOW}📤 Upload vers ESP32...${NC}"
        if load_env; then
            $PIO_CMD run --target upload
        fi
        ;;
    "monitor")
        echo -e "${YELLOW}📺 Démarrage du moniteur série...${NC}"
        $PIO_CMD device monitor --baud 115200
        ;;
    "flash")
        echo -e "${YELLOW}🚀 Upload et monitoring...${NC}"
        if load_env; then
            echo -e "${BLUE}Phase 1: Upload...${NC}"
            $PIO_CMD run --target upload
            if [ $? -eq 0 ]; then
                echo -e "${BLUE}Phase 2: Démarrage du moniteur...${NC}"
                sleep 2  # Attendre que l'ESP redémarre
                $PIO_CMD device monitor --baud 115200
            else
                echo -e "${RED}❌ Échec de l'upload !${NC}"
                exit 1
            fi
        fi
        ;;
    "clean")
        echo -e "${YELLOW}🧹 Nettoyage des fichiers de build...${NC}"
        $PIO_CMD run --target clean
        ;;
    "test")
        echo -e "${YELLOW}🧪 Lancement des tests unitaires...${NC}"
        $PIO_CMD test
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        echo -e "${RED}❌ Commande inconnue: $1${NC}"
        show_help
        exit 1
        ;;
esac