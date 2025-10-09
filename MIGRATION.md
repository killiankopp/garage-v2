# 🔄 Migration Guide - Nouveaux Scripts

## 📅 Changements effectués

Réorganisation complète du projet pour une meilleure lisibilité et maintenance.

## 🚚 Fichiers déplacés

### ✅ **PROBLÈME RÉSOLU : PlatformIO CLI**
**Erreur** : `./vscode_build.sh: line 54: pio: command not found`
**Solution** : PlatformIO installé + scripts corrigés avec détection automatique

### Scripts → `./scripts/`
- `build_vscode.sh` → `scripts/old_build_vscode.sh` (ancien, conservé pour référence)
- `build.sh` → `scripts/build.sh` (ancien script)
- `load_env.sh` → `scripts/load_env.sh`
- `run_tests.sh` → `scripts/run_tests.sh`
- `simple_test.sh` → `scripts/simple_test.sh`
- `test_kafka_logging.sh` → `scripts/test_kafka_logging.sh`

### Documentation → `./docs/`
- `ARCHITECTURE.md` → `docs/ARCHITECTURE.md`
- `AUTHENTICATION.md` → `docs/AUTHENTICATION.md`
- `KAFKA_IMPLEMENTATION_SUMMARY.md` → `docs/KAFKA_IMPLEMENTATION_SUMMARY.md`
- `KAFKA_LOGGING.md` → `docs/KAFKA_LOGGING.md`
- `REFACTORING.md` → `docs/REFACTORING.md`

## 🆕 Nouveaux scripts

### `./scripts/build.sh` - Script principal amélioré
✅ **Fonctionnalités corrigées :**
- Monitoring qui fonctionne vraiment avec `monitor`
- Upload + monitoring automatique avec `flash`
- Meilleure gestion des erreurs
- Support complet de PlatformIO CLI

### `./build.sh` - Raccourci à la racine
Pour faciliter la transition, un script de redirection a été créé à la racine.

## 🎯 Migration de vos habitudes

### Avant (problématique)
```bash
./build_vscode.sh monitor  # ❌ Ne fonctionnait pas
```

### Maintenant (solutions)
```bash
# Option 1: Script principal (recommandé)
./scripts/build.sh flash   # ✅ Upload + monitoring automatique

# Option 2: Raccourci racine
./build.sh flash          # ✅ Redirige vers le script principal

# Option 3: Monitoring seul
./scripts/build.sh monitor # ✅ Monitoring qui fonctionne
```

## 🔧 Avantages de la nouvelle organisation

1. **Monitoring fonctionnel** : Utilisation directe de PlatformIO CLI
2. **Meilleure organisation** : Fichiers groupés par type
3. **Documentation claire** : README dédié aux scripts
4. **Compatibilité** : Scripts pour VS Code ET ligne de commande
5. **Maintenance** : Code plus maintenable et évolutif

## 🚀 Pour commencer maintenant

```bash
# 1. Vérifier que le fichier .env existe
ls -la .env

# 2. Utiliser le nouveau système
./scripts/build.sh flash

# 3. Pour VS Code (optionnel)
./scripts/vscode_build.sh flash
```

## 📖 Documentation complète

Consultez `./scripts/README.md` pour le guide détaillé d'utilisation des nouveaux scripts.