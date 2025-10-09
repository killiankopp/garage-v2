# Authentification JWT avec Keycloak

Ce projet implémente un système d'authentification basé sur les tokens JWT validés par Keycloak pour protéger les routes sensibles du système de contrôle de garage.

## Architecture

L'authentification suit le principe de responsabilité unique (SRP) avec les composants suivants :

### 1. AuthConfig
- **Responsabilité** : Gestion de la configuration d'authentification
- **Fichiers** : `AuthConfig.h`, `AuthConfig.cpp`
- Pattern Singleton pour un accès global
- Charge la configuration depuis les variables d'environnement

### 2. JwtValidator  
- **Responsabilité** : Validation des tokens JWT via l'API d'introspection Keycloak
- **Fichiers** : `JwtValidator.h`, `JwtValidator.cpp`
- Communique avec l'endpoint `/realms/{realm}/protocol/openid-connect/token/introspect`
- Parse et valide la réponse d'introspection

### 3. AuthMiddleware
- **Responsabilité** : Middleware d'authentification pour les requêtes HTTP
- **Fichiers** : `AuthMiddleware.h`, `AuthMiddleware.cpp`
- Extrait et valide les headers Authorization
- Gère les réponses d'erreur HTTP 401
- Log des tentatives d'authentification

### 4. WebServerHandler (modifié)
- **Responsabilité** : Serveur HTTP avec protection des routes sensibles
- Intègre l'authentification pour les routes `/gate/open` et `/gate/close`
- Nouvelle route `/auth/info` pour vérifier la configuration

## Configuration

### Variables d'environnement

Ajoutez ces variables à votre fichier `.env` :

```bash
# Configuration Keycloak
KEYCLOAK_SERVER_URL=10.0.0.48:5990
KEYCLOAK_REALM=garage
KEYCLOAK_CLIENT_ID=garage-client
```

### Build avec PlatformIO

1. Chargez les variables d'environnement :
```bash
source load_env.sh
```

2. Compilez le projet :
```bash
pio run
```

## Utilisation

### Routes publiques (non protégées)
- `GET /` - Page d'accueil
- `GET /health` - Status de santé
- `GET /auth/info` - Information sur la configuration d'authentification
- `GET /gate/status` - Status du garage

### Routes protégées (nécessitent un token JWT)
- `POST /gate/open` - Ouvrir le garage
- `POST /gate/close` - Fermer le garage

### Format des requêtes authentifiées

```bash
curl -H "Authorization: Bearer YOUR_JWT_TOKEN" \
     http://esp32-ip/gate/open
```

### Exemples de réponses

**Succès** :
```json
{
  "status": "opening",
  "sensor_closed": false,
  "sensor_open": false,
  "operation_time": 1500,
  "timeout_remaining": 13500,
  "alert_active": false,
  "auto_close_enabled": true
}
```

**Erreur d'authentification** :
```json
{
  "error": "Unauthorized",
  "message": "Token is not active",
  "code": 401
}
```

**Information sur l'authentification** :
```json
{
  "auth_enabled": true,
  "keycloak_server": "10.0.0.48:5990",
  "realm": "garage",
  "client_id": "garage-client",
  "protected_routes": ["/gate/open", "/gate/close"]
}
```

## Sécurité

### Activation/Désactivation
- L'authentification peut être désactivée en définissant `KEYCLOAK_SERVER_URL=disabled`
- Si désactivée, toutes les routes sont accessibles sans token

### Validation des tokens
- Les tokens sont validés en temps réel via l'API d'introspection Keycloak
- Aucun cache de token côté ESP32 pour des raisons de sécurité
- Chaque requête déclenche une validation

### Logging
- Toutes les tentatives d'authentification sont loggées
- Affichage de l'IP client et du résultat (succès/échec)
- Les erreurs détaillées sont loggées pour le debugging

## Dépendances

- `ArduinoJson` : Parsing des réponses JSON de Keycloak
- `HTTPClient` : Communication HTTP avec Keycloak  
- `WebServer` : Serveur HTTP ESP32

## Configuration Keycloak

Votre serveur Keycloak doit :
1. Avoir un realm configuré (par défaut : "garage")
2. Avoir un client configuré (par défaut : "garage-client")
3. Permettre l'introspection des tokens sur l'endpoint public

Exemple d'URL d'introspection :
```
http://10.0.0.48:5990/realms/garage/protocol/openid-connect/token/introspect
```