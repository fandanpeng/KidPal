#!/usr/bin/env bash
# rollback-honcho-stack.sh — Honcho 自托管记忆栈一步回滚（K3-E1 · 261022-m5-k3）
#
# 语义：把栈回退到上一份备份的 compose 声明（若无备份则停栈）；数据卷
# honcho-pgdata 保留——记忆数据生命周期（NFR-PRIV-1）归运维面手工动作，
# 本脚本不删数据。回滚 ≠ 删除画像。
#
# 用法：bash rollback-honcho-stack.sh [--dir /opt/honcho]
set -euo pipefail

HONCHO_DIR="${KIDPAL_HONCHO_DIR:-/opt/honcho}"
while [ $# -gt 0 ]; do
  case "$1" in
    --dir) HONCHO_DIR="$2"; shift 2 ;;
    *) echo "ERROR: 未知参数 $1" >&2; exit 2 ;;
  esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

LATEST_BAK="$(ls -1 "$HONCHO_DIR"/docker-compose.yml.bak-* 2>/dev/null | sort | tail -n 1 || true)"

if [ -n "$LATEST_BAK" ] && [ -f "$LATEST_BAK" ]; then
  TS="$(date -u +%Y%m%dT%H%M%S)"
  cp "$HONCHO_DIR/docker-compose.yml" "$HONCHO_DIR/docker-compose.yml.pre-rollback-$TS"
  cp "$LATEST_BAK" "$HONCHO_DIR/docker-compose.yml"
  echo "RESTORED: $LATEST_BAK -> $HONCHO_DIR/docker-compose.yml"
  docker compose --project-directory "$HONCHO_DIR" up -d
else
  echo "NO-BACKUP: 无 compose 备份——回退为停栈（数据卷 honcho-pgdata 保留）"
  docker compose --project-directory "$HONCHO_DIR" down || true
  echo "STOPPED: 栈已停（可用 apply-honcho-stack.sh 重新收敛声明态）"
fi
