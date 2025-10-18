# Migration de Kafka vers EMQX

Ce document décrit la migration du système de logging de Kafka vers EMQX MQTT.

## Changements principaux

### 1. Remplacement des composants

- `KafkaConfig` → `EmqxConfig`
- `KafkaLogger` → `EmqxLogger`

### 2. Protocole de communication

- **Avant**: HTTP REST vers Kafka REST Proxy
- **Après**: MQTT natif vers broker EMQX

### 3. Configuration

#### Anciennes variables d'environnement (supprimées)

```bash
KAFKA_BROKER_URL=http://kafka.example.com:8082
KAFKA_TOPIC=garage-events
KAFKA_UNAUTHORIZED_TOPIC=garage-unauthorized
```

#### Nouvelles variables d'environnement

```bash
EMQX_BROKER_HOST=emqx.amazone.lan
EMQX_BROKER_PORT=1883
EMQX_USERNAME=                    # Optionnel
EMQX_PASSWORD=                    # Optionnel
EMQX_TOPIC=garage/authorized
EMQX_UNAUTHORIZED_TOPIC=garage/unauthorized
```

## Architecture EMQX

### Topics MQTT

1. **`garage/authorized`** - Actions autorisées
   - Messages des utilisateurs authentifiés
   - Contient: action, timestamp, user_id, username

2. **`garage/unauthorized`** - Tentatives non autorisées
   - Messages des tentatives d'accès refusées
   - Contient: action, timestamp, token JWT complet (pour audit)

### Format des messages

```json
{
  "timestamp": 1234567890,
  "action": "open|close",
  "authorized": true|false,
  "device_id": "ESP32_AABBCCDDEEFF",
  "sub": "user-uuid",           // Si authentifié
  "name": "John Doe",           // Si authentifié
  "token": "jwt.token.here"     // Seulement pour unauthorized
}
```

## Avantages de la migration

1. **Protocole natif MQTT**: Plus léger que HTTP REST
2. **Meilleure connectivité**: Reconnexion automatique
3. **Moins de dépendances**: Pas besoin de Kafka REST Proxy
4. **Simplicité**: Configuration plus simple pour l'ESP32
5. **Fiabilité**: QoS MQTT garantit la livraison des messages

## Dépendances

- **Ajouté**: `knolleary/PubSubClient@^2.8` - Client MQTT pour ESP32
- **Supprimé**: Dépendances HTTP pour Kafka REST API

## Tests

Les tests ont été mis à jour pour valider :

- Configuration EMQX
- Connexion MQTT
- Publication des messages
- Format JSON des logs

## Serveur EMQX

Le serveur EMQX est accessible sur le réseau local à l'adresse `emqx.amazone.lan` sur le port standard MQTT 1883.
