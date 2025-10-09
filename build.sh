#!/bin/bash

# Script de lancement rapide - Garage V2
# Redirige vers le nouveau script organisé

echo "🔄 Redirection vers ./scripts/build.sh..."
echo ""

# Exécuter le nouveau script avec tous les arguments passés
exec ./scripts/build.sh "$@"