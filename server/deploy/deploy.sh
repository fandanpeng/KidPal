#!/usr/bin/env bash
# deploy.sh — KidPal ECS 部署入口（manifest 驱动，单一写者）
# 261013-gov-review-deploy-single-source-loop 落地件（PROPOSAL T2）。
#
# 在 ECS deploy clone（/opt/kidpal/deploy-clone，见 ecs-bootstrap.md）内执行：
#   git fetch + checkout 指定 ref → 按 manifest 铺 owner=deploy.sh 条目 →
#   按变更集决定重启面（front / engine 分开判，引擎重启 = 设备全断，谨慎）→
#   引擎面有变更时跑 apply-xiaozhi-overlay.sh 渲染 → 落 .deployed-commit 戳 →
#   自动跑 verify-deploy.sh 自检。
#
# 用法：
#   bash server/deploy/deploy.sh [--ref REF] [--dry-run] [--first-run]
#     --ref REF     部署到的 git ref（默认 origin/main）
#     --dry-run    只报告将要铺放/重启的清单，不写不重启
#     --first-run  首次对账铺放：跳过「部署前漂移拦截」（首铺按定义必有漂移——
#                  2026-10-13 基线 = PROPOSAL §2 实测的 6 处漂移 + 2 处配置缺位）；
#                  铺完后的 verify 自检仍须绿
#
# 设计约束（PROPOSAL 冻结面）：
#   - secrets 豁免：secrets.env 仅 ECS，manifest 不含、本脚本不碰；
#   - owner=overlay 条目本脚本不铺（单一写者——overlay 渲染面由
#     apply-xiaozhi-overlay.sh 负责，本脚本在引擎面变更时调用它）；
#   - live 紧急热修允许存在，但部署前 verify 拦截未回流漂移（fail-loud）。
set -euo pipefail

REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null)" || {
  echo "ERROR: 不在 git clone 内运行（deploy clone = /opt/kidpal/deploy-clone，见 ecs-bootstrap.md）" >&2
  exit 2
}
cd "$REPO_ROOT"
MANIFEST="$REPO_ROOT/server/deploy/manifest.yaml"
[ -f "$MANIFEST" ] || { echo "ERROR: manifest 不存在：$MANIFEST" >&2; exit 2; }

if [ -x /opt/kidpal/venv/bin/python3 ]; then
  PY=/opt/kidpal/venv/bin/python3
else
  PY=python3
fi

REF="origin/main"
DRY_RUN=0
FIRST_RUN=0
while [ $# -gt 0 ]; do
  case "$1" in
    --ref)       REF="$2"; shift 2 ;;
    --dry-run)   DRY_RUN=1; shift ;;
    --first-run) FIRST_RUN=1; shift ;;
    *) echo "ERROR: 未知参数 $1" >&2; exit 2 ;;
  esac
done

# ── 0. 部署前漂移拦截（fail-loud；--first-run 跳过：首铺对账基线；
#      --dry-run 也跳过——预览不该被漂移拦死，真部署才拦）──
if [ "$FIRST_RUN" -ne 1 ] && [ "$DRY_RUN" -ne 1 ]; then
  echo "== 部署前漂移核验 =="
  if ! bash server/deploy/verify-deploy.sh --no-alerts; then
    echo "REFUSE: live 存在未对账漂移（上面清单）——先回流仓或按 manifest 对账，再部署。" >&2
    echo "（首铺对账场景用 --first-run；漂移详情已由 verify 输出。）" >&2
    exit 1
  fi
fi

# ── 1. 取代码 ──
echo "== git fetch + checkout $REF =="
git fetch origin
git checkout "$REF"
COMMIT="$(git rev-parse HEAD)"
echo "deploying commit $COMMIT"

# ── 2. 按 manifest 铺放（owner=deploy.sh）+ 收集重启面 ──
# 备份策略：铺放前把旧 live 快照进 /opt/kidpal/deploy-backups/<commit-ts>/，
# 替代历史手工 .bak 习惯；回滚 = 从快照目录拷回 + 按重启面重启。
BACKUP_DIR="/opt/kidpal/deploy-backups/$(date +%Y%m%d-%H%M%S)"
STAMP="/opt/kidpal/.deployed-commit"

PLAN="$("$PY" - "$MANIFEST" <<'PYEOF'
import json, sys
import yaml
manifest = yaml.safe_load(open(sys.argv[1], encoding="utf-8"))
plan = []
for e in manifest.get("entries", []):
    if e.get("owner") != "deploy.sh" or not e.get("src"):
        continue
    plan.append({"src": e["src"], "dst": e["dst"], "restart": e.get("restart", "none")})
print(json.dumps(plan))
PYEOF
)"

echo "== 铺放清单（manifest owner=deploy.sh，共 $(echo "$PLAN" | "$PY" -c 'import json,sys;print(len(json.load(sys.stdin)))') 条）=="
CHANGED_ANY=0
CHANGED_FRONT=0
CHANGED_ENGINE=0
CHANGED_LIST=""

while IFS= read -r line; do
  SRC_REL="$(echo "$line" | cut -f1)"
  DST="$(echo "$line" | cut -f2)"
  RESTART="$(echo "$line" | cut -f3)"
  SRC="$REPO_ROOT/$SRC_REL"
  [ -f "$SRC" ] || { echo "ERROR: 仓内源缺失 $SRC" >&2; exit 2; }

  # 行尾归一比对：一致则跳过（md5 判同与 verify-deploy.sh 同口径）
  SAME="$("$PY" -c "
import hashlib,sys
def m(p):
    return hashlib.md5(open(p,'rb').read().replace(b'\r\n',b'\n')).hexdigest()
import os
d='$DST'
print('same' if os.path.exists(d) and m('$SRC')==m(d) else 'diff')
")"
  if [ "$SAME" = "same" ]; then
    continue
  fi
  CHANGED_ANY=1
  CHANGED_LIST="$CHANGED_LIST$SRC_REL -> $DST"$'\n'
  case "$RESTART" in
    front)  CHANGED_FRONT=1 ;;
    engine) CHANGED_ENGINE=1 ;;
  esac
  echo "  $SRC_REL -> $DST  [restart:$RESTART]"
  if [ "$DRY_RUN" -eq 0 ]; then
    mkdir -p "$BACKUP_DIR$(dirname "$DST")"
    if [ -f "$DST" ]; then cp "$DST" "$BACKUP_DIR$DST"; fi
    mkdir -p "$(dirname "$DST")"
    cp "$SRC" "$DST"
  fi
done < <(echo "$PLAN" | "$PY" -c '
import json, sys
for e in json.load(sys.stdin):
    print("\t".join((e["src"], e["dst"], e["restart"])))
')

if [ "$CHANGED_ANY" -eq 0 ]; then
  echo "无变更条目（live 已与 $COMMIT 一致）。"
fi

# systemd timer 单元铺放后需 reload（幂等）
if [ "$DRY_RUN" -eq 0 ]; then
  if systemctl list-unit-files 2>/dev/null | grep -q kidpal-deploy-drift; then
    systemctl daemon-reload
    systemctl enable --now kidpal-deploy-drift.timer 2>/dev/null || true
  fi
fi

# ── 3. 引擎面变更 → overlay 渲染（.config.yaml 是 overlay 的渲染产物）──
if [ "$CHANGED_ENGINE" -eq 1 ]; then
  echo "== 引擎面有变更：跑 apply-xiaozhi-overlay.sh 渲染 =="
  if [ "$DRY_RUN" -eq 0 ]; then
    bash /opt/kidpal/xiaozhi-esp32-server/deploy/apply-xiaozhi-overlay.sh
  fi
else
  echo "引擎面无变更——不跑 overlay、不重启引擎。"
fi

# ── 4. 重启（按变更集；dry-run 只报告）──
if [ "$CHANGED_FRONT" -eq 1 ]; then
  echo "== restart kidpal-front =="
  [ "$DRY_RUN" -eq 0 ] && systemctl restart kidpal-front
fi
if [ "$CHANGED_ENGINE" -eq 1 ]; then
  echo "== docker restart xiaozhi-esp32-server（设备将全断重连）=="
  [ "$DRY_RUN" -eq 0 ] && docker restart xiaozhi-esp32-server
fi

# ── 5. 落戳 + 自检 ──
if [ "$DRY_RUN" -eq 0 ]; then
  {
    echo "commit=$COMMIT"
    echo "deployed_at=$(date -Is)"
    echo "backup_dir=$BACKUP_DIR"
    printf '%s' "$CHANGED_LIST"
  } > "$STAMP"
  echo "== 部署后自检 =="
  bash server/deploy/verify-deploy.sh
  echo "DEPLOY OK @ $COMMIT（快照 $BACKUP_DIR）"
else
  echo "== dry-run 完成（未写未重启）=="
  [ "$CHANGED_ANY" -eq 1 ] && echo "将要变更的条目："$'\n'"$CHANGED_LIST"
fi
