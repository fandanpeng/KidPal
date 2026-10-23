#!/usr/bin/env bash
# apply-honcho-stack.sh — Honcho 自托管记忆栈幂等铺放（K3-E1 · 261022-m5-k3）
#
# 语义：声明（本目录 docker-compose.yml + honcho.env.example 模板）→ /opt/honcho
# 消费位；幂等（重复运行收敛同一状态）；不删除数据卷（NFR-PRIV-1：记忆数据
# 本机持久，删除 = 运维面手工、独立动作）。
#
# 用法：bash apply-honcho-stack.sh [--dir /opt/honcho]
# 前置：docker + docker compose 插件在位；/opt/honcho/.env 已按 honcho.env.example
#       填好（缺 .env 直接拒跑——凭据纪律：真实值只存 ECS）。
set -euo pipefail

HONCHO_DIR="${KIDPAL_HONCHO_DIR:-/opt/honcho}"
while [ $# -gt 0 ]; do
  case "$1" in
    --dir) HONCHO_DIR="$2"; shift 2 ;;
    *) echo "ERROR: 未知参数 $1" >&2; exit 2 ;;
  esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if ! command -v docker >/dev/null 2>&1; then
  echo "ERROR: docker 不在位——栈铺放前置缺失" >&2
  exit 2
fi
if [ ! -f "$HONCHO_DIR/.env" ]; then
  echo "ERROR: $HONCHO_DIR/.env 不存在——先复制 honcho.env.example 并填入真实凭据（凭据不入仓）" >&2
  exit 2
fi

mkdir -p "$HONCHO_DIR"
if ! cmp -s "$SCRIPT_DIR/docker-compose.yml" "$HONCHO_DIR/docker-compose.yml"; then
  cp "$SCRIPT_DIR/docker-compose.yml" "$HONCHO_DIR/docker-compose.yml"
  echo "APPLIED: compose -> $HONCHO_DIR/docker-compose.yml"
else
  echo "SKIP: compose 已一致（幂等，不重铺）"
fi
if ! cmp -s "$SCRIPT_DIR/honcho.env.example" "$HONCHO_DIR/honcho.env.example"; then
  cp "$SCRIPT_DIR/honcho.env.example" "$HONCHO_DIR/honcho.env.example"
  echo "APPLIED: env 模板 -> $HONCHO_DIR/honcho.env.example"
else
  echo "SKIP: env 模板已一致（幂等）"
fi

docker compose --project-directory "$HONCHO_DIR" up -d
echo "== 五容器状态 =="
docker compose --project-directory "$HONCHO_DIR" ps
echo "DONE: Honcho 栈已收敛声明态（api = 127.0.0.1:18100；出网仅 deriver→DeepSeek + embedding→DashScope 两条记账管线）"
