#!/usr/bin/env bash
# KidPal C1: apply the char-stroke hold-and-replace patch to the DEPLOYMENT OpenClaw
# 2026.8.1 dist bundle (mechanism B). 见 DEPLOY-RUNBOOK.md（planner 逐条执行）。
#
# Reality (coordinator/planner 2026-09 ECS 实测):
#   - OpenClaw = npm 全局装 2026.8.1, 无 src 源码 checkout; 补丁目标是 dist ESM chunk:
#       /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-<hash>.js
#   - 数据集实际在 /root/.openclaw/workspace/kidpal/chardata/ (graphics.txt + dictionary.txt)
#   - char_cache.json 需先由 warm_char_cache.py 生成
#   - /opt/kidpal/chardata 不存在 -> 需 ln -s 软链 (部署动作, planner 照 runbook 做)
#
# This script: patches dist (drift-guarded) + warms cache. Idempotent + backup + restore.
set -euo pipefail

PATCH_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MARKER="[KidPal C1] char-backed hold-and-replace"

NM_ROOT="${OPENCLAW_NM_ROOT:-/root/.npm-global/lib/node_modules/openclaw}"
DIST_DIR="$NM_ROOT/dist"
DATA_DIR="${CHAR_DATA_DIR:-/root/.openclaw/workspace/kidpal/chardata}"
CACHE_JSON="${CHAR_CACHE_JSON:-$DATA_DIR/char_cache.json}"

# ---- 0. 定位 dist chunk（含 2026.8.1 锚点的那个；多 hash chunk 里选真网关） --------
if [ ! -d "$DIST_DIR" ]; then
  echo "ERR: dist dir not found: $DIST_DIR (OpenClaw 没装在 $NM_ROOT?)" >&2
  exit 2
fi
mapfile -t CANDIDATES < <(ls "$DIST_DIR"/openai-http-*.js 2>/dev/null || true)
TARGET=""
for c in "${CANDIDATES[@]:-}"; do
  if grep -q "resolveAgentResponseCommentary" "$c" 2>/dev/null; then
    TARGET="$c"; break
  fi
done
if [ -z "$TARGET" ]; then
  echo "ERR: no dist chunk under $DIST_DIR contains the 2026.8.1 anchor "
  echo "     'resolveAgentResponseCommentary'. 版本漂移 → 停止 (runbook: 对不上就停)." >&2
  exit 3
fi
echo "dist chunk: $TARGET"

# ---- 1. marker 幂等 --------------------------------------------------------------
if grep -q "$MARKER" "$TARGET"; then
  echo "SKIP: $TARGET already patched (marker present)."
else
  cp "$TARGET" "$TARGET.c1.bak"
  echo "backed up $TARGET -> $TARGET.c1.bak"
  python3 "$PATCH_DIR/make_openclaw_patch.py" "$TARGET" || {
    echo "ERR: make_openclaw_patch.py failed (anchor drift?). Restoring backup." >&2
    cp "$TARGET.c1.bak" "$TARGET"
    exit 3
  }
  echo "patched $TARGET (marker: $MARKER)"
fi

# ---- 2. warm char_cache.json -----------------------------------------------------
if [ ! -f "$DATA_DIR/graphics.txt" ] || [ ! -f "$DATA_DIR/dictionary.txt" ]; then
  echo "ERR: dataset not found under $DATA_DIR (graphics.txt/dictionary.txt). "
  echo "     runbook: CHAR_DATA_DIR 软链/指向 /root/.openclaw/workspace/kidpal/chardata" >&2
  exit 4
fi
PYTHONPATH="$PATCH_DIR" python3 "$PATCH_DIR/warm_char_cache.py" "$DATA_DIR" "$CACHE_JSON"

echo "done. Patch marker + warm cache applied."
echo "NEXT (部署动作, planner): ln -s 数据集到 /opt/kidpal/chardata, 并 systemctl restart "
echo "     openclaw-gateway 后按 DEPLOY-RUNBOOK.md 校验点复核。"