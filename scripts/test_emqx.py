#!/usr/bin/env python3
"""Publie un message de test sur EMQX avec les variables du .env.

- Utilise les variables suivantes (chargées via scripts/load_env.sh):
  EMQX_BROKER_HOST, EMQX_BROKER_PORT, EMQX_USERNAME, EMQX_PASSWORD,
  EMQX_TOPIC (topic de succès), EMQX_UNAUTHORIZED_TOPIC (optionnel)

- Par défaut, publie un message JSON sur EMQX_TOPIC et tente une souscription
  temporaire pour vérifier la réception si --verify est activé.

Dépendances: paho-mqtt (pip install paho-mqtt)
"""

import argparse
import json
import os
import sys
import time
from datetime import datetime

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("Ce script nécessite paho-mqtt. Installez-le: pip install paho-mqtt", file=sys.stderr)
    sys.exit(2)


def get_env(name, default=None, required=False):
    val = os.getenv(name, default)
    if required and not val:
        print(f"❌ Variable d'environnement requise manquante: {name}", file=sys.stderr)
        sys.exit(1)
    return val


def main():
    parser = argparse.ArgumentParser(description="Test de publication EMQX")
    parser.add_argument("--verify", action="store_true", help="Souscrire et vérifier la réception du message publié")
    parser.add_argument("--topic", help="Topic à utiliser (outrepasse EMQX_TOPIC)")
    parser.add_argument("--payload", help="Payload JSON à publier (sinon généré)")
    parser.add_argument("--qos", type=int, default=1, help="QoS à utiliser (0/1/2)")
    parser.add_argument("--retain", action="store_true", help="Publier en mode retain")
    parser.add_argument("--host", help="Hôte du broker (outrepasse EMQX_BROKER_HOST)")
    parser.add_argument("--port", type=int, help="Port du broker (outrepasse EMQX_BROKER_PORT)")
    parser.add_argument("--tls", action="store_true", help="Activer TLS (port par défaut 8883 si non précisé)")
    parser.add_argument("--cafile", help="Chemin vers le certificat CA (si TLS)")
    parser.add_argument("--insecure", action="store_true", help="TLS sans vérification des certificats (dev seulement)")
    args = parser.parse_args()

    host = args.host or get_env("EMQX_BROKER_HOST", required=True)
    port_env = int(get_env("EMQX_BROKER_PORT", "1883"))
    port = args.port if args.port is not None else (8883 if args.tls and port_env == 1883 else port_env)
    username = get_env("EMQX_USERNAME", "")
    password = get_env("EMQX_PASSWORD", "")
    topic = args.topic or get_env("EMQX_TOPIC", required=True)

    payload = args.payload
    if not payload:
        payload = json.dumps({
            "type": "garage_v2_test",
            "message": "Hello EMQX",
            "timestamp": datetime.utcnow().isoformat() + "Z",
        })

    client_id = f"garage-v2-test-{int(time.time())}"
    received_ok = False

    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print(f"✅ Connecté à EMQX {host}:{port}")
            if args.verify:
                client.subscribe(topic, qos=args.qos)
        else:
            print(f"❌ Connexion EMQX échouée (rc={rc})")

    def on_message(client, userdata, msg):
        nonlocal received_ok
        print(f"📩 Message reçu sur {msg.topic}: {msg.payload.decode('utf-8', 'ignore')}")
        # Vérif simple: même topic
        received_ok = True

    client = mqtt.Client(client_id=client_id, clean_session=True)
    if args.tls:
        # Configure TLS; cafile optional, insecure allowed for dev
        try:
            client.tls_set(ca_certs=args.cafile if args.cafile else None)
        except Exception as e:
            print(f"⚠️ TLS configuration error: {e}")
        if args.insecure:
            client.tls_insecure_set(True)
    if username:
        client.username_pw_set(username, password)

    client.on_connect = on_connect
    client.on_message = on_message if args.verify else None

    try:
        client.connect(host, port, keepalive=20)
    except Exception as e:
        print(f"❌ Échec de connexion à EMQX: {e}")
        return 1

    client.loop_start()

    # petite attente pour la connexion et la souscription
    time.sleep(0.5)

    print(f"🚀 Publication sur {topic} (qos={args.qos}, retain={args.retain})")
    result = client.publish(topic, payload, qos=args.qos, retain=args.retain)
    result.wait_for_publish()

    if result.rc != mqtt.MQTT_ERR_SUCCESS:
        print(f"❌ Publication échouée (rc={result.rc})")
        client.loop_stop()
        client.disconnect()
        return 1

    print("✅ Publication OK")

    if args.verify:
        # attendre la réception
        deadline = time.time() + 3.0
        while time.time() < deadline and not received_ok:
            time.sleep(0.1)
        client.loop_stop()
        client.disconnect()
        if received_ok:
            print("✅ Vérification de réception OK")
            return 0
        else:
            print("⚠️ Pas de message reçu en retour (selon les ACL, le broker peut ne pas renvoyer nos propres publications)")
            return 0

    client.loop_stop()
    client.disconnect()
    return 0


if __name__ == "__main__":
    sys.exit(main())
