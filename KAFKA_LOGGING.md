# Kafka Logging

Ce document décrit la fonctionnalité de logging des actions de porte via Kafka.

## Fonctionnalités

Le système log automatiquement toutes les demandes d'ouverture et de fermeture de la porte sur un serveur Kafka, qu'elles soient autorisées ou non.

### Messages d'actions autorisées

Pour les actions autorisées, les messages sont envoyés sur le topic `garage` avec la structure suivante :

```json
{
    "timestamp": 1234567890,
    "action": "open|close",
    "authorized": true,
    "sub": "user-id-from-jwt",
    "name": "User Display Name"
}
```

### Messages d'actions non autorisées

Pour les actions non autorisées, les messages sont envoyés sur le topic `garage-unauthorized` avec la structure suivante :

```json
{
    "timestamp": 1234567890,
    "action": "open|close", 
    "authorized": false,
    "sub": "user-id-from-jwt-if-available",
    "name": "User Display Name if available",
    "token": "complete-jwt-token-for-analysis"
}
```

## Configuration

La configuration Kafka se fait via les variables d'environnement suivantes dans le fichier `.env` :

```bash
KAFKA_BROKER_URL=http://10.0.0.48:5996
KAFKA_TOPIC=garage
KAFKA_UNAUTHORIZED_TOPIC=garage-unauthorized
```

## Architecture

Le système respecte le principe de responsabilité unique (SRP) avec les composants suivants :

### KafkaConfig
- **Responsabilité** : Gestion de la configuration Kafka
- **Fichiers** : `KafkaConfig.h`, `KafkaConfig.cpp`
- Charge les variables d'environnement
- Valide la configuration
- Fournit un point d'accès unique aux paramètres Kafka

### KafkaLogger
- **Responsabilité** : Envoi des messages vers Kafka
- **Fichiers** : `KafkaLogger.h`, `KafkaLogger.cpp`
- Construit les messages JSON
- Gère la communication HTTP avec Kafka
- Sépare les actions autorisées et non autorisées

### WebServerHandler (modifié)
- **Responsabilité étendue** : Intégration du logging dans les handlers de routes
- Log automatiquement toutes les actions de porte
- Extrait les informations du token JWT
- Gère les cas d'authentification activée/désactivée

## Tests

Des tests unitaires sont disponibles dans :
- `test/test_kafka/test_kafka.cpp` : Tests des composants Kafka
- `test/test_kafka_integration/test_kafka_integration.cpp` : Tests d'intégration

## Compilation

Pour compiler avec la configuration Kafka :

```bash
source load_env.sh && pio run
```

## Sécurité

- Les URLs Kafka ne sont **jamais** hardcodées dans le code source
- La configuration se fait uniquement via des variables d'environnement
- Le fichier `.env` est exclu du versioning (`.gitignore`)
- Les tokens complets ne sont envoyés que pour les tentatives non autorisées à des fins d'analyse de sécurité

## Dépannage

### Kafka désactivé
Si les variables d'environnement Kafka ne sont pas définies, le système continue de fonctionner normalement sans logging.

### Erreurs de connexion
Les erreurs de connexion Kafka sont loggées sur la console série mais n'interrompent pas le fonctionnement normal du système.

### Test de connectivité
Un test de connectivité Kafka est effectué au démarrage pour valider la configuration.