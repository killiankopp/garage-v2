#!/usr/bin/env python3
"""Utility script to validate Keycloak connectivity before flashing the ESP32."""

import argparse
import base64
import json
import os
import ssl
import sys
import urllib.parse
import urllib.request
from urllib.error import HTTPError, URLError


def build_base_url(server, protocol):
    server = server.strip()
    if server.startswith("http://") or server.startswith("https://"):
        return server.rstrip("/")
    return f"{protocol}://{server}".rstrip("/")


def encode_basic_auth(client_id, client_secret):
    credentials = f"{client_id}:{client_secret}".encode("utf-8")
    return "Basic " + base64.b64encode(credentials).decode("ascii")


def post_form(url, data, *, auth_header=None, context=None):
    payload = urllib.parse.urlencode(data).encode("utf-8")
    request = urllib.request.Request(url, data=payload, method="POST")
    request.add_header("Content-Type", "application/x-www-form-urlencoded")
    if auth_header:
        request.add_header("Authorization", auth_header)

    try:
        if context is None:
            with urllib.request.urlopen(request) as response:
                body = response.read().decode("utf-8")
                return response.status, body
        with urllib.request.urlopen(request, context=context) as response:
            body = response.read().decode("utf-8")
            return response.status, body
    except HTTPError as error:
        body = error.read().decode("utf-8", errors="ignore")
        return error.code, body
    except URLError as error:
        print(f"❌ Impossible de contacter {url}: {error.reason}")
        sys.exit(2)


def parse_json(body: str, *, description: str) -> dict:
    try:
        return json.loads(body)
    except json.JSONDecodeError:
        print(f"❌ Réponse inattendue pour {description} :\n{body}")
        sys.exit(3)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Teste l'obtention d'un token Keycloak et son introspection avant déploiement sur ESP32.",
    )
    parser.add_argument(
        "--token",
        help="Token existant à introspecter. Si non fourni, le script utilise le flux client_credentials.",
    )
    parser.add_argument(
        "--skip-introspection",
        action="store_true",
        help="N'effectue pas l'introspection du token (uniquement la récupération).",
    )
    parser.add_argument(
        "--insecure",
        action="store_true",
        help="Autorise les certificats TLS auto-signés (désactive la vérification SSL).",
    )

    args = parser.parse_args()

    server_url = os.getenv("KEYCLOAK_SERVER_URL")
    realm = os.getenv("KEYCLOAK_REALM")
    client_id = os.getenv("KEYCLOAK_CLIENT_ID")
    client_secret = os.getenv("KEYCLOAK_CLIENT_SECRET")
    protocol = os.getenv("KEYCLOAK_PROTOCOL", "http")

    missing = [name for name, value in (
        ("KEYCLOAK_SERVER_URL", server_url),
        ("KEYCLOAK_REALM", realm),
        ("KEYCLOAK_CLIENT_ID", client_id),
    ) if not value]

    if missing:
        print("❌ Variables d'environnement manquantes : " + ", ".join(missing))
        return 1

    base_url = build_base_url(server_url, protocol)
    token_endpoint = f"{base_url}/realms/{realm}/protocol/openid-connect/token"
    introspection_endpoint = f"{base_url}/realms/{realm}/protocol/openid-connect/token/introspect"

    ssl_context = None
    if base_url.startswith("https://") and args.insecure:
        ssl_context = ssl._create_unverified_context()

    token = args.token
    obtained_via_client_credentials = False

    if not token:
        if not client_secret:
            print("❌ KEYCLOAK_CLIENT_SECRET est requis pour obtenir un token avec le flux client_credentials.")
            print("   Fournissez --token avec un jeton existant si vous utilisez un client public.")
            return 1

        print("🔑 Obtention d'un token via le flux client_credentials...")
        auth_header = encode_basic_auth(client_id, client_secret)
        status, body = post_form(
            token_endpoint,
            {
                "grant_type": "client_credentials",
                "client_id": client_id,
            },
            auth_header=auth_header,
            context=ssl_context,
        )

        if status != 200:
            print(f"❌ Échec de la récupération du token (HTTP {status}) :\n{body}")
            return 1

        token_response = parse_json(body, description="la récupération de token")
        token = token_response.get("access_token")

        if not token:
            print("❌ La réponse ne contient pas de access_token.")
            return 1

        obtained_via_client_credentials = True
        print("✅ Token récupéré avec succès.")

    if args.skip_introspection:
        print("⏭️  Introspection sautée (--skip-introspection).")
        return 0

    print("🔍 Introspection du token via Keycloak...")
    auth_header = encode_basic_auth(client_id, client_secret) if client_secret else None
    status, body = post_form(
        introspection_endpoint,
        {
            "token": token,
            "client_id": client_id,
            **({"client_secret": client_secret} if client_secret else {}),
        },
        auth_header=auth_header,
        context=ssl_context,
    )

    if status != 200:
        print(f"❌ Échec de l'introspection (HTTP {status}) :\n{body}")
        return 1

    introspection = parse_json(body, description="l'introspection")

    active = introspection.get("active", False)
    username = introspection.get("username")
    subject = introspection.get("sub")
    client = introspection.get("client_id") or introspection.get("azp")

    print("✅ Introspection réussie.")
    print(f"   Token actif : {'oui' if active else 'non'}")
    if username:
        print(f"   Utilisateur : {username}")
    if subject:
        print(f"   Subject (sub) : {subject}")
    if client:
        print(f"   Client : {client}")
    if obtained_via_client_credentials:
        print("   Mode : client_credentials")

    return 0 if active else 4


if __name__ == "__main__":
    sys.exit(main())
