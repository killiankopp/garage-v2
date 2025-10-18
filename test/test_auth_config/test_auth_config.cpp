#define KEYCLOAK_SERVER_URL "garage.example.com"
#define KEYCLOAK_REALM "garage"
#define KEYCLOAK_CLIENT_ID "garage-client"
#define KEYCLOAK_CLIENT_SECRET "super-secret"

#include <iostream>
#include "../../src/components/AuthConfig.h"

// Inclure l'implémentation pour que les macros de configuration s'appliquent
#include "../../src/components/AuthConfig.cpp"

int tests_run = 0;
int tests_passed = 0;

void assertTrue(bool condition, const char* message) {
    tests_run++;
    if (condition) {
        tests_passed++;
        std::cout << "✅ PASS: " << message << std::endl;
    } else {
        std::cout << "❌ FAIL: " << message << std::endl;
    }
}

void assertStringEquals(const char* expected, const String& actual, const char* message) {
    tests_run++;
    if (actual == expected) {
        tests_passed++;
        std::cout << "✅ PASS: " << message << std::endl;
    } else {
        std::cout << "❌ FAIL: " << message << " (" << actual.c_str() << " != " << expected << ")" << std::endl;
    }
}

int main() {
    std::cout << "🧪 Test AuthConfig - Démarrage" << std::endl;

    AuthConfig& config = AuthConfig::getInstance();
    config.setAuthEnabled(false);
    config.loadFromEnvironment();

    assertTrue(config.isAuthEnabled(), "Auth should be enabled when server URL is provided");
    assertTrue(config.hasKeycloakClientSecret(), "Client secret should be detected");
    assertStringEquals("super-secret", config.getKeycloakClientSecret(), "Client secret should match");

    std::cout << "\n📊 Résultats:" << std::endl;
    std::cout << "Tests exécutés: " << tests_run << std::endl;
    std::cout << "Tests réussis: " << tests_passed << std::endl;
    std::cout << "Tests échoués: " << (tests_run - tests_passed) << std::endl;

    if (tests_run == tests_passed) {
        std::cout << "🎉 Test AuthConfig réussi !" << std::endl;
        return 0;
    }

    std::cout << "❌ Test AuthConfig en échec" << std::endl;
    return 1;
}
