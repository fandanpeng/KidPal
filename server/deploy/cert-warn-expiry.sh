#!/usr/bin/env bash
# KidPal direction-B gateway cert expiry warning (V25 / GWA-A9 到期前告警).
#
# Scans the gateway server cert and WARNS (non-zero exit / alert line) once it is
# within the expiry window of the notAfter date. Compose with a cron/systemd on the
# ECS to page an operator before the wss leg drops. The cert is rotated by the sibling
# server/deploy/cert-renew.sh (>=10y validity, 决策22).
#
# Exit semantics (machinable for cron):
#   0  = cert OK, far from expiry (no alert)
#   1  = expiry warning triggered (到期前告警) — operator should rotate
#   2  = cert unreadable/absent

set -uo pipefail

CERT="${XIAOZHI_CERT:-/opt/kidpal/xiaozhi-esp32-server/cert/server.pem}"
# 到期前告警窗口: warn when expiring within this many days.
WARN_DAYS="${XIAOZHI_CERT_WARN_DAYS:-30}"

if [ ! -f "$CERT" ]; then
  echo "ALERT: no gateway cert at $CERT (absent) — 到期前告警 V25"
  exit 2
fi

notafter=$(openssl x509 -in "$CERT" -noout -enddate 2>/dev/null | cut -d= -f2- || true)
if [ -z "$notafter" ]; then
  echo "ALERT: could not read notAfter from $CERT"
  exit 2
fi
# epoch of notAfter
exp_epoch=$(openssl x509 -in "$CERT" -noout -enddate 2>/dev/null \
            | cut -d= -f2- | xargs -0 date -d "$(echo "$notafter" | tr -d '\r')" +%s 2>/dev/null || echo "")
if [ -z "$exp_epoch" ]; then
  echo "ALERT: cannot parse notAfter '$notafter' (expiry warning inconclusive)"
  exit 2
fi
now_epoch=$(date +%s)
# seconds until expiry
left=$(( exp_epoch - now_epoch ))
left_days=$(( left / 86400 ))
if [ "$left" -le 0 ]; then
  echo "ALERT: gateway cert EXPIRED at $notafter (notAfter) — 必须立即轮换 (cert-renew.sh)"
  exit 1
fi
if [ "$left_days" -le "$WARN_DAYS" ]; then
  echo "ALERT: gateway cert expires in ${left_days} days (notAfter $notafter) — 到期前告警, 请轮换"
  exit 1
fi
echo "OK: gateway cert notAfter=$notafter expires in ${left_days} days (>${WARN_DAYS}-day warn window)"
exit 0