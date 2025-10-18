#!/bin/bash

# Framework de test simple pour C++
# Usage: ./simple_test.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🧪 Tests Unitaires - Garage V2${NC}"
echo ""

# Créer un répertoire temporaire pour les tests
TEST_DIR="./test_build"
mkdir -p $TEST_DIR

# Fonction pour compiler et exécuter un test
run_test() {
    local test_name=$1
    local test_file=$2
    local extra_sources=${3:-}
    
    echo -e "${YELLOW}Compilation de $test_name...${NC}"
    
    # Compiler le test
    g++ -std=c++17 -I./src -I./test -I./test/mocks -DUNIT_TEST \
        $test_file $extra_sources \
        -o $TEST_DIR/$test_name
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Échec de compilation pour $test_name${NC}"
        return 1
    fi
    
    echo -e "${YELLOW}Exécution de $test_name...${NC}"
    
    # Exécuter le test
    $TEST_DIR/$test_name
    local result=$?
    
    if [ $result -eq 0 ]; then
        echo -e "${GREEN}✅ $test_name : SUCCÈS${NC}"
        return 0
    else
        echo -e "${RED}❌ $test_name : ÉCHEC${NC}"
        return 1
    fi
}

# Liste des tests
total_tests=0
failed_tests=0

# Test de base
echo "==============================================="
((total_tests++))
if ! run_test "test_basic" "./test/test_basic_simple/test_basic_simple.cpp"; then
    ((failed_tests++))
fi
echo ""

# Test de logique
echo "==============================================="
((total_tests++))
if ! run_test "test_logic" "./test/test_logic_simple/test_logic_simple.cpp"; then
    ((failed_tests++))
fi
echo ""

# Test de compilation JwtValidator (smoke)
echo "==============================================="
((total_tests++))
if ! run_test "test_jwt_validation" "./test/test_jwt_validation/test_jwt_validation.cpp" "./src/components/AuthConfig.cpp ./src/components/JwtValidator.cpp"; then
     ((failed_tests++))
fi
echo ""
# Test AuthConfig
echo "==============================================="
((total_tests++))
if ! run_test "test_auth_config" "./test/test_auth_config/test_auth_config.cpp" "./src/components/AuthConfig.cpp"; then
    ((failed_tests++))
fi
echo ""

# Test de configuration d'authentification
echo "==============================================="
((total_tests++))
if ! run_test "test_auth_config" "./test/test_auth_config/test_auth_config.cpp"; then
    ((failed_tests++))
fi
echo ""

# Nettoyage
rm -rf $TEST_DIR

# Résumé
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