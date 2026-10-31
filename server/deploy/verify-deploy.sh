#!/usr/bin/env bash
# verify-deploy.sh — KidPal ECS 部署漂移机检（manifest 驱动）
# 261013-gov-review-deploy-single-source-loop 落地件（PROPOSAL T3）。
#
# 读 server/deploy/manifest.yaml，逐条核验 live 与仓内源一致：
#   - verify=md5    → 行尾归一 md5 比对（src vs dst）
#   - verify=marker → 逐锚 grep dst（secret 承载件；禁 md5 禁期望值）
#   - cron_jobs     → openclaw cron show <name> --json 与期望命令串/归属比对
#
# 三个时机共用本脚本：部署前前置（deploy.sh 调）、部署后自检（deploy.sh 调）、
# 每日 systemd timer（kidpal-deploy-drift.timer）。
# 261022-m5-k1-prompt-carrier-migration：渲染件 marker 锚集随 prompt 承载反转同批换
# （v11 静态内容锚退役、换动态 context 块锚）；本脚本恒 manifest 驱动、对全部条目
# 一视同仁执行 md5 / marker 两路径核验——不为任何单文件设特例减免。
# 漂移时：写 /opt/kidpal/alerts/deploy-drift.json（机器可读告警面）+ exit 1。
#
# 用法：bash server/deploy/verify-deploy.sh [--alerts-dir DIR] [--no-alerts]
# 退出码：0 = 全绿；1 = 有漂移；2 = 脚本自身错误（manifest 缺失/解析失败等）。
set -uo pipefail

REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null)" || {
  echo "ERROR: 不在 git clone 内运行（deploy clone = /opt/kidpal/deploy-clone，见 ecs-bootstrap.md）" >&2
  exit 2
}
MANIFEST="$REPO_ROOT/server/deploy/manifest.yaml"
[ -f "$MANIFEST" ] || { echo "ERROR: manifest 不存在：$MANIFEST" >&2; exit 2; }

# python 解释器：ECS venv 优先（3.11 + PyYAML），退化裸 python3
if [ -x /opt/kidpal/venv/bin/python3 ]; then
  PY=/opt/kidpal/venv/bin/python3
else
  PY=python3
fi

# openclaw CLI 可达性（cron 核验用）；PATH 兜底 npm-global
export PATH="/root/.npm-global/bin:$PATH"

ALERTS_DIR="${KIDPAL_ALERTS_DIR:-/opt/kidpal/alerts}"
NO_ALERTS=0
while [ $# -gt 0 ]; do
  case "$1" in
    --alerts-dir) ALERTS_DIR="$2"; shift 2 ;;
    --no-alerts)  NO_ALERTS=1; shift ;;
    *) echo "ERROR: 未知参数 $1" >&2; exit 2 ;;
  esac
done

"$PY" - "$MANIFEST" "$ALERTS_DIR" "$NO_ALERTS" <<'PYEOF'
import hashlib
import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

import yaml

manifest_path, alerts_dir, no_alerts = sys.argv[1], sys.argv[2], int(sys.argv[3])
repo = Path(manifest_path).resolve().parents[2]

try:
    manifest = yaml.safe_load(Path(manifest_path).read_text(encoding="utf-8"))
except Exception as exc:  # noqa: BLE001
    print(f"ERROR: manifest 解析失败: {exc}", file=sys.stderr)
    sys.exit(2)

def norm_md5(path: Path) -> str:
    """行尾归一 md5——repo（Windows CRLF）与 ECS（LF）两侧可比。"""
    raw = path.read_bytes()
    return hashlib.md5(raw.replace(b"\r\n", b"\n").replace(b"\r", b"\n")).hexdigest()

findings = []

def drift(kind: str, ident: str, detail: str) -> None:
    findings.append({"kind": kind, "id": ident, "detail": detail})
    print(f"DRIFT [{kind}] {ident}: {detail}")

for entry in manifest.get("entries", []):
    dst = Path(entry["dst"])
    src = entry.get("src")
    ident = entry["dst"]
    if not dst.exists():
        drift("missing", ident, "live 缺文件（src=%s）" % src)
        continue
    if entry.get("verify") == "marker":
        for anchor in entry.get("anchors", []):
            if anchor not in dst.read_text(encoding="utf-8", errors="replace"):
                drift("anchor", ident, f"锚未命中: {anchor!r}")
    else:  # md5（须有 src）
        if not src:
            drift("config", ident, "verify=md5 但无 src——manifest 配置错误")
            continue
        src_path = repo / src
        if not src_path.exists():
            drift("config", ident, f"仓内源缺失: {src}")
            continue
        want, got = norm_md5(src_path), norm_md5(dst)
        if want != got:
            drift("md5", ident, f"src={src} 仓内 {want[:8]}… != live {got[:8]}…")

for job in manifest.get("cron_jobs", []):
    name = job["name"]
    proc = subprocess.run(
        ["openclaw", "cron", "show", name, "--json"],
        capture_output=True, text=True, timeout=120,
    )
    if proc.returncode != 0:
        drift("cron", name, f"openclaw cron show 失败: {proc.stderr.strip()[:200]}")
        continue
    try:
        payload = json.loads(proc.stdout)
    except json.JSONDecodeError:
        drift("cron", name, "cron JSON 解析失败")
        continue
    if "expect_argv" in job:
        argv = " ".join(payload.get("payload", {}).get("argv", []))
        # argv[2] 是 sh -lc 的整条命令串；与期望串做去空白比对
        cmd = payload.get("payload", {}).get("argv", ["", "", ""])[2] if len(
            payload.get("payload", {}).get("argv", [])) >= 3 else ""
        joined = (cmd or argv).split()
        expect = job["expect_argv"].split()
        if joined != expect:
            drift("cron", name, "命令串漂移（期望 %s）" % job["expect_argv"][:80])
    if "expect_agent" in job and payload.get("agentId") != job["expect_agent"]:
        drift("cron", name, f"agent 漂移: live={payload.get('agentId')!r} 期望={job['expect_agent']!r}")
    if "expect_schedule" in job:
        live_sched = payload.get("schedule", {})
        every_min = live_sched.get("everyMs", 0) // 60000
        if live_sched.get("kind") == "every" and every_min != 15:
            drift("cron", name, f"调度漂移: live=every {every_min}min 期望={job['expect_schedule']}")
    # ── trigger 核验（261031-openclaw-cron-trigger-gating）──
    # ① expect_trigger_file：job.trigger.script 与盘上文件行尾归一后逐字相等
    #    （抓「盘上变了但 job 内联没更新」）；② once 不得为 true（变更驱动须周期复判）；
    # ③ 全局 cron.triggers.enabled 不得为 false（带 trigger 的 job 会每轮 status=error）。
    trigger = payload.get("trigger")
    if isinstance(trigger, dict) and trigger.get("once") is True:
        drift("cron", name, "trigger.once=true —— 必须 false（变更驱动需每周期复判）")
    if "expect_trigger_file" in job:
        rel = job["expect_trigger_file"]
        src_path = repo / rel
        if not isinstance(trigger, dict) or not str(trigger.get("script") or "").strip():
            drift("cron", name, "缺 trigger.script（manifest 已登记 expect_trigger_file）")
        elif not src_path.is_file():
            drift("config", name, f"仓内 trigger 源缺失: {rel}")
        else:
            want = src_path.read_bytes().replace(b"\r\n", b"\n").replace(b"\r", b"\n") \
                .decode("utf-8", "replace").strip()
            got = str(trigger.get("script")).replace("\r\n", "\n").replace("\r", "\n").strip()
            if want != got:
                drift("cron", name,
                      f"内联 trigger 文本与仓内 {rel} 不一致（重跑 cron_triggers.py apply）")

# 全局 trigger 开关守卫：只在能确证 false 时报警（不可读 → 不判漂移，L1 live 观测兜底）。
def _trigger_global_enabled():
    try:
        p = subprocess.run(
            ["openclaw", "config", "get", "cron.triggers.enabled", "--json"],
            capture_output=True, text=True, timeout=60)
    except Exception:  # noqa: BLE001
        p = None
    if p is not None and p.returncode == 0:
        txt = (p.stdout or "").strip()
        if txt.lower() in ("true", "false"):
            return txt.lower() == "true"
        try:
            val = json.loads(txt)
            if isinstance(val, dict):
                val = val.get("value", val.get("cron.triggers.enabled"))
            if isinstance(val, bool):
                return val
        except ValueError:
            pass
    for cand in (os.environ.get("OPENCLAW_HOME"), "/root/.openclaw"):
        if not cand:
            continue
        cfg = Path(cand) / "openclaw.json"
        if not cfg.is_file():
            continue
        try:
            doc = json.loads(cfg.read_text(encoding="utf-8", errors="replace"))
            val = doc.get("cron", {}).get("triggers", {}).get("enabled")
            if isinstance(val, bool):
                return val
        except Exception:  # noqa: BLE001
            pass
    return None

if _trigger_global_enabled() is False:
    drift("config", "cron.triggers.enabled",
          "全局 trigger 开关为 false —— 带 trigger 的 job 每轮 status=error（功能停摆）")


report = {
    "checked_at": datetime.now(timezone.utc).isoformat(timespec="seconds"),
    "repo_commit": subprocess.run(
        ["git", "-C", str(repo), "rev-parse", "HEAD"],
        capture_output=True, text=True).stdout.strip(),
    "drift_count": len(findings),
    "findings": findings,
}

print(json.dumps({"drift_count": len(findings)}, ensure_ascii=False))
for f in findings:
    print(f"  [{f['kind']}] {f['id']}")

if not no_alerts:
    alerts = Path(alerts_dir)
    alerts.mkdir(parents=True, exist_ok=True)
    (alerts / "deploy-drift.json").write_text(
        json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")

sys.exit(1 if findings else 0)
PYEOF
VERIFY_RC=$?

# ── Honcho 记忆子系统核验段（261022-m5-k3 / K3-E1；arch §5.3.3 ③「记忆子系统
#    api 18100 仅本机可达、五容器其余端口公网不可达」的机检承载）──
# compose 声明面（published 端口全显式 127.0.0.1）+ 运行态双核验：
#   ① loopback 探针：127.0.0.1:18100 本机可达且监听面无非 loopback 绑定；
#   ② 五容器健康门：docker compose ps 全部 running/healthy（compose ps 路径，
#      端口探针不能替代容器健康）。
# 漂移同样并入口径：任何一段失败 → 本脚本 exit 1（每日 drift timer 消费同脚本）。
HONCHO_DIR_LIVE="${KIDPAL_HONCHO_DIR:-/opt/honcho}"
HONCHO_FAIL=0
if [ ! -d "$HONCHO_DIR_LIVE" ]; then
  echo "DRIFT [honcho] $HONCHO_DIR_LIVE 不在世——记忆子系统栈未部署（K3-E1 消费位缺失）"
  HONCHO_FAIL=1
else
  if command -v ss >/dev/null 2>&1 && ss -tln 2>/dev/null | grep -q ':18100 '; then
    if ! ss -tln 2>/dev/null | grep ':18100 ' | grep -q '127.0.0.1:18100'; then
      echo "DRIFT [honcho] 18100 存在非 127.0.0.1 监听——api 暴露公网（出网红线失守）"
      HONCHO_FAIL=1
    fi
  fi
  if command -v curl >/dev/null 2>&1; then
    if ! curl -sS -m 5 -o /dev/null http://127.0.0.1:18100/ 2>/dev/null; then
      echo "DRIFT [honcho] 127.0.0.1:18100 loopback 探针失败——api 未就绪"
      HONCHO_FAIL=1
    fi
  fi
  if command -v docker >/dev/null 2>&1; then
    HONCHO_PS="$(docker compose --project-directory "$HONCHO_DIR_LIVE" ps --format '{{.Service}} {{.State}}' 2>/dev/null || true)"
    HONCHO_TOTAL="$(printf '%s\n' "$HONCHO_PS" | grep -c . || true)"
    HONCHO_BAD="$(printf '%s\n' "$HONCHO_PS" | grep -cv 'running\|healthy' || true)"
    if [ "${HONCHO_TOTAL:-0}" -ne 5 ]; then
      echo "DRIFT [honcho] 五容器健康门不满足：compose ps 计数 ${HONCHO_TOTAL:-0}/5"
      HONCHO_FAIL=1
    fi
    if [ "${HONCHO_BAD:-0}" -ne 0 ]; then
      echo "DRIFT [honcho] 有容器非 running/healthy："
      printf '%s\n' "$HONCHO_PS" | grep -v 'running\|healthy' || true
      HONCHO_FAIL=1
    fi
  else
    echo "DRIFT [honcho] docker 不在位——五容器健康门不可核"
    HONCHO_FAIL=1
  fi
fi
[ "$HONCHO_FAIL" -ne 0 ] && VERIFY_RC=1

# ── Honcho backfill 迁移工具核验段（261022-m5-k4 / K4-E6；261013 纪律）──
# 两工具逐件 md5 比对已由上方 manifest 驱动段覆盖（entries dst =
# /opt/honcho/backfill/*）；本段核消费位目录在世——栈已部署而迁移工具
# 消费位缺位 = drift（执行面 = human ECS 轮 K4-E6）。
BACKFILL_DIR_LIVE="${KIDPAL_HONCHO_DIR:-/opt/honcho}/backfill"
if [ -d "${KIDPAL_HONCHO_DIR:-/opt/honcho}" ] && [ ! -d "$BACKFILL_DIR_LIVE" ]; then
  echo "DRIFT [honcho-backfill] $BACKFILL_DIR_LIVE 不在世——backfill 迁移工具消费位缺失（K4-E6；manifest 已登记两工具）"
  VERIFY_RC=1
fi

exit "$VERIFY_RC"
