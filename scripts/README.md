# 🔧 Scripts de Build - Guide d'utilisation

Ce dossier contient tous les scripts pour compiler, uploader et tester le projet ESP32 Garage Controller.

## 📜 Scripts disponibles

### `build.sh` - Script principal

Script principal utilisant PlatformIO CLI directement, recommandé pour la plupart des cas d'usage.

```bash
# Compilation seulement
./scripts/build.sh build

# Upload vers l'ESP32
./scripts/build.sh upload

# Monitoring série
./scripts/build.sh monitor

# Upload + monitoring automatique (recommandé)
./scripts/build.sh flash

# Nettoyage des fichiers de build
./scripts/build.sh clean

# Tests unitaires
./scripts/build.sh test

# Aide
./scripts/build.sh help
```

### `vscode_build.sh` - Pour VS Code

Script optimisé pour une utilisation avec VS Code et l'extension PlatformIO IDE.

```bash
# Utilisation identique au script principal
./scripts/vscode_build.sh flash
```

## 🔄 Migration depuis les anciens scripts

### Ancien usage → Nouveau usage

```bash
# AVANT
./build_vscode.sh monitor    # ❌ Ne fonctionnait pas bien

# MAINTENANT  
./scripts/build.sh flash     # ✅ Upload + monitoring automatique
# ou
./scripts/build.sh monitor   # ✅ Monitoring seulement
```

## 🐛 Résolution des problèmes

### Le monitoring ne fonctionne pas
1. Vérifiez que l'ESP32 est bien connecté
2. Vérifiez les permissions du port série :
   ```bash
   ls -la /dev/cu.usbserial*
   # ou
   ls -la /dev/tty.usbserial*
   ```
3. Essayez de spécifier le port manuellement :
   ```bash
   pio device monitor --port /dev/cu.usbserial-0001 --baud 115200
   ```

### PlatformIO non trouvé
```bash
# Installation via pip
pip install platformio

# Ou via Homebrew sur macOS
brew install platformio
```

### Variables d'environnement non trouvées
Vérifiez que votre fichier `.env` existe et contient :
```env
WIFI_SSID=VotreSSID
WIFI_PASSWORD=VotreMotDePasse
# ... autres variables
```

## 🎯 Workflow recommandé

1. **Première utilisation** :
   ```bash
   # Créer le fichier .env
   cp .env.example .env
   # Éditer avec vos valeurs
   
   # Build et flash
   ./scripts/build.sh flash
   ```

2. **Développement** :
   ```bash
   # Cycle de développement rapide
   ./scripts/build.sh flash    # Compile + upload + monitor
   ```

3. **Tests** :
   ```bash
   ./scripts/build.sh test     # Tests unitaires
   ```

4. **Debug** :
   ```bash
   ./scripts/build.sh monitor  # Monitoring seulement
   ```

## ⚙️ Configuration avancée

### Modifier le port série
Éditez `platformio.ini` :
```ini
[env:esp32doit-devkit-v1]
monitor_port = /dev/cu.usbserial-0001
monitor_speed = 115200
```

### Monitoring avec filtres
```bash
# Filtrer les logs par niveau
pio device monitor --filter esp32_exception_decoder

# Monitoring en couleur
pio device monitor --filter colorize
```