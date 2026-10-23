#!/usr/bin/env bash
# healthcheck-honcho.sh — Honcho 五容器健康检查（K3-E1 · 261022-m5-k3）
#
# 核验两件（arch §5.3.3 ③ 机检承载）：
#   ① api loopback 探针：127.0.0.1:18100 仅本机可达（ss 监听面 + HTTP 探针）；
#   ② 五容器健康门：docker compose ps 全部 running/healthy。
#
# 用法：bash healthcheck-honcho.sh [--dir /opt/honcho]
# 退出码：0 = 全绿；1 = 有漂移。
set -uo pipefail

HONCHO_DIR="${KIDPAL_HONCHO_DIR:-/opt/honcho}"
while [ $# -gt 0 ]; do
  case "$1" in
    --dir) HONCHO_DIR="$2"; shift 2 ;;
    *) echo "ERROR: 未知参数 $1" >&2; exit 2 ;;
  esac
done

FAIL=0

# ① loopback 探针：18100 在监听且绑定面仅为 127.0.0.1（公网不可达红线）
if command -v ss >/dev/null 2>&1 && ss -tln 2>/dev/null | grep -q ':18100 '; then
  if ! ss -tln 2>/dev/null | grep ':18100 ' | grep -q '127.0.0.1:18100'; then
    echo "DRIFT [honcho-health] 18100 存在非 127.0.0.1 监听——api 暴露公网（出网红线失守）"
    FAIL=1
  fi
fi
if command -v curl >/dev/null 2>&1; then
  if ! curl -sS -m 5 -o /dev/null http://127.0.0.1:18100/ 2>/dev/null; then
    echo "DRIFT [honcho-health] 127.0.0.1:18100 探针失败——api 未就绪"
    FAIL=1
  fi
fi

# ② 五容器健康门（compose ps 路径）
if command -v docker >/dev/null 2>&1; then
  PS_LINES="$(docker compose --project-directory "$HONCHO_DIR" ps --format '{{.Service}} {{.State}}' 2>/dev/null || true)"
  TOTAL="$(printf '%s\n' "$PS_LINES" | grep -c . || true)"
  BAD="$(printf '%s\n' "$PS_LINES" | grep -cv 'running\|healthy' || true)"
  if [ "${TOTAL:-0}" -ne 5 ]; then
    echo "DRIFT [honcho-health] 五容器计数 = ${TOTAL:-0}/5（compose ps）"
    FAIL=1
  fi
  if [ "${BAD:-0}" -ne 0 ]; then
    echo "DRIFT [honcho-health] 有容器非 running/healthy："
    printf '%s\n' "$PS_LINES" | grep -v 'running\|healthy' || true
    FAIL=1
  fi
else
  echo "DRIFT [honcho-health] docker 不在位——健康门不可核"
  FAIL=1
fi

if [ "$FAIL" -eq 0 ]; then
  echo "HEALTHY: Honcho 五容器全绿（api=127.0.0.1:18100 loopback）"
fi
exit "$FAIL"
