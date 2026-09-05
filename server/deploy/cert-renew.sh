#!/usr/bin/env bash
# KidPal direction-B gateway self-signed cert issuance + rotation (GWA-A9 / 决策22 / V25).
#
# Generates a self-signed ECDSA/RSA server cert with a >=10-year validity (we use 20y
# = 7300 days, well above the 3650-day floor) and rewrites it into place for the
# xiaozhi gateway wss leg. Idempotent: a later run *rotates* (regenerates) the cert.
#
# Deploy-time usage:
#   server/deploy/cert-renew.sh [renew|force]
#   (default): emit only if the on-disk cert has no valid HTTPS server cert yet.
#   renew/force: regenerate unconditionally (rotation per V25).
#
# The rotated key/cert land at the frozen paths the acceptance reads:
#   /opt/kidpal/xiaozhi-esp32-server/cert/server.{key,pem}
# Expiry warning lives in the sibling server/deploy/cert-warn-expiry.sh (到期前告警 V25).
#
# ARQ-260902-cert-lifecycle-deadlock = accept-as-risk (human 2026-09-04): 验收面不变
# (>=10y + rotation + expiry alert). OTA does NOT move earlier.

set -euo pipefail

CERT_DIR="${XIAOZHI_CERT_DIR:-/opt/kidpal/xiaozhi-esp32-server/cert}"
CERT="$CERT_DIR/server.pem"
KEY="$CERT_DIR/server.key"
# 决策22 「有效期 >= 10 年」— 20 years (7300 days) as the script literal.
VALID_DAYS="${XIAOZHI_CERT_VALID_DAYS:-7300}"
CN="${XIAOZHI_CERT_CN:-kidpal-gateway}"
MODE="${1:-auto}"

mkdir -p "$CERT_DIR"

already_valid() {
  # A server.pem that is itself a valid HTTPS server cert is left alone on 'auto'.
  [ -f "$CERT" ] && openssl x509 -in "$CERT" -noout -checkend 0 >/dev/null 2>&1 \
    && openssl x509 -in "$CERT" -noout -ext extendedKeyUsage 2>/dev/null | grep -q "TLS Web Server Authentication"
}

if [ "$MODE" = "renew" ] || [ "$MODE" = "force" ] || ! already_valid; then
  openssl req -x509 -newkey rsa:3072 -nodes \
    -keyout "$KEY" -out "$CERT" \
    -days "$VALID_DAYS" \
    -subj "/CN=$CN" \
    -addext "extendedKeyUsage=serverAuth" \
    -addext "subjectAltName=DNS:$CN,IP:127.0.0.1,IP:8.137.91.16" \
    >/dev/null 2>&1
  chmod 640 "$KEY"; chmod 644 "$CERT"
  echo "renewed $CERT valid_days=$VALID_DAYS"
else
  echo "cert $CERT already valid (>=10y unexpired) — no rotation needed (auto)"
fi

openssl x509 -in "$CERT" -noout -dates 2>/dev/null || true
echo "rotation done: $CERT"