#!/usr/bin/env bash
# Valide un token Keycloak via l'endpoint d'introspection en réutilisant la config du projet.
# Usage:
#   source ./scripts/load_env.sh
#   ./scripts/validate_token.sh "<JWT>"
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <JWT> [--insecure]" >&2
  exit 1
fi

TOKEN="$1"
INSECURE="${2:-}"

if [[ -z "${KEYCLOAK_SERVER_URL:-}" || -z "${KEYCLOAK_REALM:-}" || -z "${KEYCLOAK_CLIENT_ID:-}" ]]; then
  echo "❌ Variables d'environnement manquantes. Exécutez: source ./scripts/load_env.sh" >&2
  exit 1
fi

PY_ARGS=("--token" "$TOKEN")
if [[ "$INSECURE" == "--insecure" ]]; then
  PY_ARGS+=("--insecure")
fi

python3 ./scripts/test_keycloak.py "${PY_ARGS[@]}"
