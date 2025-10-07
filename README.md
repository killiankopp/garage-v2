# Garage V2 - Configuration

## Configuration WiFi Sécurisée avec fichier .env

Pour protéger vos identifiants WiFi, ce projet utilise les **variables d'environnement système**.

### ⚠️ Important 
Le fichier `platformio.ini` est versionné sur Git, donc **JAMAIS de secrets dedans !**

### Configuration

1. **Créez le fichier `.env`** à la racine du projet (ignoré par Git) :
```env
WIFI_SSID=VotreSSID
WIFI_PASSWORD=VotreMotDePasse
```

2. **Utilisez le script de build** qui charge automatiquement le `.env` :
```bash
# Compilation
./build.sh

# Upload vers ESP32
./build.sh upload

# Upload + monitoring
./build.sh monitor
```

### Alternative manuelle

Si vous préférez définir les variables manuellement :
```bash
export WIFI_SSID="VotreSSID"
export WIFI_PASSWORD="VotreMotDePasse"
pio run --target upload
```

## Sécurité

- ✅ Le fichier `.env` est dans `.gitignore`
- ✅ Aucun secret dans le code versionné
- ✅ Variables d'environnement chargées dynamiquement
- ✅ Vos secrets ne seront jamais exposés sur GitHub

## Build et Upload

```bash
# Compiler
pio run

# Uploader sur l'ESP32
pio run --target upload

# Monitorer la sortie série
pio device monitor
```