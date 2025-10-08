#!/bin/bash

# Script pour lancer les tests unitaires
# Usage: ./run_tests.sh [test_name]

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🧪 Lancement des tests unitaires...${NC}"
echo ""

# Fonction pour afficher le résultat
show_result() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✅ Tests $2 : SUCCÈS${NC}"
    else
        echo -e "${RED}❌ Tests $2 : ÉCHEC${NC}"
        return 1
    fi
}

# Si un test spécifique est demandé
if [ ! -z "$1" ]; then
    echo -e "${YELLOW}Exécution du test spécifique: $1${NC}"
    platformio test -e native -f $1
    exit $?
fi

# Variables pour compter les résultats
total_tests=0
failed_tests=0

# Liste des tests à exécuter
tests=("test_basic" "test_logic")

for test in "${tests[@]}"; do
    echo -e "${YELLOW}Exécution de: $test${NC}"
    platformio test -e native -f $test
    result=$?
    show_result $result $test
    
    ((total_tests++))
    if [ $result -ne 0 ]; then
        ((failed_tests++))
    fi
    echo ""
done

# Résumé final
echo -e "${BLUE}===========================================${NC}"
echo -e "${BLUE}Résumé des tests${NC}"
echo -e "${BLUE}===========================================${NC}"
echo -e "Total des tests: $total_tests"

if [ $failed_tests -eq 0 ]; then
    echo -e "${GREEN}Tests réussis: $total_tests${NC}"
    echo -e "${GREEN}Tests échoués: 0${NC}"
    echo -e "${GREEN}🎉 Tous les tests sont passés avec succès !${NC}"
    exit 0
else
    echo -e "${GREEN}Tests réussis: $((total_tests - failed_tests))${NC}"
    echo -e "${RED}Tests échoués: $failed_tests${NC}"
    echo -e "${RED}❌ Certains tests ont échoué${NC}"
    exit 1
fi