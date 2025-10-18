#define KEYCLOAK_SERVER_URL "REPLACE_ME_SERVER" // can be overridden via build flags if needed
#define KEYCLOAK_REALM "REPLACE_ME_REALM"
#define KEYCLOAK_CLIENT_ID "REPLACE_ME_CLIENT"
#define KEYCLOAK_CLIENT_SECRET "REPLACE_ME_SECRET"

#include <iostream>
#include "../../src/components/JwtValidator.h"
#include "../../src/components/AuthConfig.h"

// Minimal stub for HTTPClient on host won't work; this test serves as compile guard and runtime will be covered by Python.
// We'll compile the validator and ensure validateToken can be invoked symbolically, but won't actually run network I/O here.

int main() {
    std::cout << "🧪 Test JwtValidator - Compilation guard" << std::endl;
    AuthConfig &cfg = AuthConfig::getInstance();
    cfg.loadFromEnvironment();

    JwtValidator validator(&cfg);
    ValidationResult res = validator.validateToken("dummy");
    // We don't assert network behavior here since HTTPClient depends on Arduino stack.
    std::cout << "ℹ️ Appel de validateToken effectué (résultat.isValid=" << (res.isValid ? "true" : "false") << ")" << std::endl;
    return 0; // compile-only smoke test
}
