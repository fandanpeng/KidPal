"""Frozen acceptance tests — TaskPackage 261022-m5-k3-honcho-memory-production
（K3-E2 / K3-E5 / K3-E6 host 面 + live 证据判定齿；rigor=full；riskTier=high；
subsystem = 记忆写入资格门（user 缺席即非资格、读写同门）+ front 一次性回合
隔离 + openclaw.json 声明式 patch 组合 + 记忆降级护栏。栈生产化与 v12 守则见
姊妹件 test_261022_honcho_memory.acceptance.py。）

上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
PROPOSAL.md T4（资格门唯一判据 = 请求是否携带 user；白名单入册不做黑名单剔除；
验收方式①②③④）+ ADR-014 + workstream PROPOSAL §3.1 R7 / §3.2 OQ-4 + arch
§6.7 六行表（front 引导/整理回合 = 非资格第 1/2 行）+ docs/requirements.md
S-F5.1-AC3/AC5 + NFR-PRIV-1 + docs/debt/tech/TECH-261022-device-memory-prompt-
overhaul.md 证据 7（hooks.allowConversationAccess=true 后一次性路径钩子即入库；
peer 键 agent:kidpal:openai-user:<device_id>）+ launch-card 纪律红线（openclaw.json
双声明式 patch 顺序 / peer 键不另造 / A2 归属 261022-openclaw-honcho-plugin）。

背景（TECH-261022 第 3 包）：debt 证据 7 实证 `hooks.allowConversationAccess=true`
后 front 一次性路径钩子即正常触发 → 消息入库——**隔离判据必须先于插件启用量产
验证**（plan.md 可行性判据）。资格门 = 仅 kidpal 语音链路且请求携
`user=<device_id>` 的回合可读写记忆（读写同门：非资格回合不写 observation、
不注入画像）；main agent / cron / front 一次性回合一律不入库。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O）

  A) front 一次性回合 payload 零 user（arch T4 验收方式③ front 侧结构断言；
     现状已合规 = must-stay-green 回归护栏）：`_openclaw_chat_once` 经
     openclaw_stream_request 单源构造 payload，**不得**携带 `user` 键——
     front 引导/整理回合无设备身份（arch §6.7 六行表第 1/2 行），user 缺席
     即非资格（资格门唯一判据；main 链路恰一条 user message 且 user=device_id
     的正证已由 K2 冻结件 test_261022_persistent_session 承载，本件不复钉）。

  B) server/deploy/261022-openclaw-honcho-plugin/（A2 归属：human 2026-10-22
     确认归本包；种子外扩）—— 资格门 + 插件授权 + 降级护栏三件合一：

       1) honcho-plugin-declaration.json —— openclaw.json 声明工件（**JSON**，
          stdlib json 解析）。必需顶层键：case / changes / drift_guard /
          backup / rollback。
            · changes : 非空 list[{path: <点分路径>, value: <json>}]，≥2 条，
              且必须含：
                - 恰一条 path 以 "hooks.allowConversationAccess" 结尾、
                  value === true（debt 证据 7 钩子授权字面）；
                - ≥一条 path 含 "honcho"（ci）且 value 子树含 baseUrl 字面
                  "127.0.0.1:18100"（baseUrl 指本机 loopback）。
          其余键（gateway / agents.entries 下 P1 tools.allow 与 261017 skills
          白名单 / 全局 tools.profile）原样保留——**双声明式 patch 组合**：
          P1 tools allowlist 已应用在先（live 态 allow=[read,write,exec,
          web_search]），本包顺序 apply、幂等、不整文件改写。
            · drift_guard : 非空 list[{path, expect}]，必须含
              {"path": "agents.entries.kidpal.tools.allow",
               "expect": ["read","write","exec","web_search"]}
              —— **现状锚 = P1 已应用态**（launch-card 红线：本包 drift_guard
              以 P1 已应用态为现状锚，非 K1 的 unset 形态）。expect == "unset"
              判键缺席，否则 json 相等判。
            · backup / rollback : 261017/261022 同形制（backup.pattern 含
              "bak"、config 同目录 glob、无路径分隔符；rollback.script =
              rollback_honcho_plugin.py 在同目录）。
       2) apply_honcho_plugin.py —— 幂等 apply（**Python**，stdlib json 零
          额外依赖，不依赖 jq）。语义序（261017 PV-A6 / K1-E4 同形制）：
            ① 幂等优先：config 现状已等于全部声明目标 → 打印 SKIP、exit 0
              （不写、不再备份）；
            ② drift-guard：逐条核对 drift_guard，任一不中 → 打印 STOP、
              **exit 3**（不写、不备份）；
            ③ 写前时间戳备份：config 同目录、basename + ".bak-..." + UTC
              yyyymmddTHHMMSS、内容 = 原始 config；
            ④ 应用：逐条把 value 写到 path（每条只改自己的键）；成功 exit 0。
          CLI：--config（默认 /root/.openclaw/openclaw.json）、--decl（默认
          同目录声明工件）。缺 config / 缺声明 / 非法 JSON → exit 2。
       3) rollback_honcho_plugin.py —— 一步回滚（--config、--backup、
          --dry-run；非 dry-run 还原 backup 内容 exit 0）。
       4) 资格门承载件（gate carrier，形态自由：插件包装补丁 JS / 插件配置
          声明均可，但必须落在本工作台内）冻结锚集：
            · 启动标记行（ARQ-260907 同族机械报警锚）字面
              "kidpal honcho memory patch active"，且须由 **Honcho 可达性
              探针门控**（探针通过才打 marker；探针失败 = error 级报警 + 降级
              不打 marker——插件装了但 Honcho 不可达时静默照打 marker =
              假绿报警锚，K2b 门控先例同族）；
            · peer 键锚 "agent:kidpal:openai-user:"（peer 键不另造红线：消费
              P2 的 user=<device_id> 与该键族；工作台内 "agent:kidpal:" 前缀
              出现形态唯一 = "agent:kidpal:openai-user:"，任何变体 = 红）；
            · 钩子授权锚 "allowConversationAccess" 与 baseUrl 锚
              "127.0.0.1:18100"（可与声明件分载，本判据看工作台并集）。

  C) K3-E5 降级护栏（gate carrier 内）：persist / inject 路径 bounded 失败
     （catch/except 形态在场）；零无界重试风暴（while(true)/while True = 红）；
     Honcho 不可达 → 本轮无画像注入、对话照常、记忆失败不进用户可见兜底。
     文本锚为**源锚代理**（行为本体 = live 真栈实证，见分工边界）。

  D) K3-E2 / K3-E3 live 证据判定齿（@pytest.mark.live，host 不 auto-run，
     conftest 机械 skip）：live/部署轮把结构化探针证据落
     delivery/taskpackages/261022-m5-k3-honcho-memory-production/memory-probes/
     *.json，本件 live 齿判定：
       · marker 探针（DB+日志双证）文件 schema：
           {"main_link": [{"marker": str, "db_hit": bool, "log_hit": bool}],
            "one_shot":  [{"round": "guide"|"organize", "marker": str,
                           "db_hit": bool, "log_hit": bool}],
            "read_side": {"non_qualified_injection": bool}}
         判定 = main_link ≥1 且逐条 db_hit && log_hit（主链路正证）；
         one_shot 覆盖 {"guide","organize"} 各 ≥1 且逐条零命中（一次性回合
         负证，arch T4 验收方式①）；read_side.non_qualified_injection ==
         false（读侧同门）。
       · seeded 负向探针集文件 schema：
           {"probes": [{"category": "姓名"|"电话"|"家庭安排", "seed": str,
                        "query": str, "profile_hit": bool, "reply_hit": bool}]}
         判定 = category 覆盖三档各 ≥1、全部 probes profile_hit == false 且
         reply_hit == false（S-F5.1-AC3 / NFR-PRIV-1 量化口径：注入式探针
         100% 不回忆）。
     证据缺位 = live 相位设计态红（不 auto-run，ARQ-260901-g3 同纪律）。

===========================================================================
host/live 分工边界（按 riskTier=high + Honcho DB 与 OpenClaw 插件运行时均在
ECS、host 面不可直达，如实申报）：

  · host 面（本件）= front payload 结构断言（行为级，离线真跑）+ 声明式 patch
    四态行为断言（fixture 真跑）+ 资格门/降级**源锚代理**断言。
  · live/部署轮面 = arch T4 验收方式①②④（marker 探针 DB+日志双证 / 主链路
    正证 / seeded 负向探针集）+ 降级真栈实证 + 资格门运行时行为——由本件
    live 齿（D 节）对结构化证据判定 + K3-E7 live 观测项承载；Pod 不自行上
    ECS，host 面转绿后 status awaiting-human:live-deploy。

强度申报（契约条目 → 承载断言 → 性质档）：

  · E2 资格门唯一判据（front 半：一次性回合 payload 零 user）
                                       → test_k3_e2_front_oneshot_payload_no_user → 行为级
  · E2 资格门承载件 / peer 键不另造 / marker 探针门控
                                       → test_k3_e2_gate_carrier_anchors → 源锚代理
  · E2 主链路正证 / 一次性回合负证 / 读侧同门（marker DB+日志双证）
                                       → live 齿 test_live_k3_e2_... → 行为级（live 证据判定）
  · E3 seeded 负向探针集 100% 零命中   → live 齿 test_live_k3_e3_... → 行为级（live 证据判定）
  · E5 bounded 失败 / 零重试风暴        → test_k3_e5_degradation_guards → 源锚代理
  · E6 声明工件 schema + drift_guard P1 已应用态锚
                                       → test_k3_e6_declaration_schema → 机制级
  · E6 apply 幂等 / P1 组合态保留      → test_k3_e6_apply_idempotent_... → 行为级（真跑）
  · E6 drift 现状锚不中 exit 3 零写    → test_k3_e6_apply_drift_guard_exit3 → 行为级（真跑）
  · E6 时间戳备份 / 回滚 dry-run+还原  → test_k3_e6_apply_backup / rollback → 行为级（真跑）
  · E6 openclaw.json 零入仓 + 零 secret → test_k3_e6_no_openclaw_json_... → 机制级
  · E6 manifest 工作台登记             → test_k3_e6_manifest_registers → 机制级

缺口（承载断言强度低于契约要求处，逐条）：

  · 资格门运行时行为（携 user 回合入库、缺 user 回合零入库零注入）不可离线
    实证——gate carrier 锚集是**源锚代理**：「工作台并集含 user 门控锚」≠
    「运行时按 user 缺席过滤」。行为本体 = live marker 探针 DB+日志双证
    （arch T4 验收方式①②）+ 读侧同门证据。行为等价假设显式化：① 插件侧
    按 openclawSessionKey 映射 peer（证据 7 实测键形）≈ 运行时以 user 键族
    过滤（若部署轮实测键形不符，属验收判法升级，走 tighten-only 窗口经
    planner 调整，不得静默绕过）；② marker 同文件含探针调用 ≈ 门控成立
    （分支级判定超离线文本判法能力，live 轮 marker 有无 + DB 双证兜底）。
  · E5 降级：文本锚（catch 在场 / 零 while(true)）是弱代理；「不阻塞、不
    静默重试风暴、失败不进用户可见兜底」的运行时证明 = 部署轮真栈实证
    （停 Honcho 容器 → 对话照常）——K3-E7 live 观测项申报面，host 不造假绿。
  · E6 drift_guard「现状锚 = P1 已应用态」offline 只证声明形态与 fixture
    行为；ECS 实况是否真为 P1 已应用态由 apply 时 STOP / SKIP 回执承载
    （261017 PV-A6 同款缺口）。
  · declaration 的插件注册 path 形态（openclaw.json 运行时 schema）不作冻结
    ——只钉两条判据字面（hooks.allowConversationAccess=true / baseUrl
    127.0.0.1:18100）；运行时键形以部署轮插件加载观测为准。

fixture 自含性声明：全部断言直读 repo 内制品 + tempfile fixture（P1 已应用态
组合态内嵌本文件）；零网络 I/O、零外部资产、零环境前提（PyYAML 仅 manifest
验收测试 case 用）。evidence 类断言仓根由 __file__ 显式推导，不依赖调用点
cwd。行为 seam 驱动方对账：声明 → OpenClaw 运行时插件加载的驱动方 = 部署轮
（offline 无驱动方 = 已申报盲区）；探针证据 → live 齿判定闭环；「模型真的
记 / 真的拒」驱动方 = 真机轮（K3-E7）。

must-stay-green（冻结时点即绿，builder 不得回退）：
  · test_k3_e2_front_oneshot_payload_no_user（front 现状合规 = 回归护栏；
    若 builder 为隔离改动 front payload 装配面，此断言红 = 越权）。
  · K1 冻结件（test_261022_openclaw_kidpal_tools / test_261017_openclaw_kidpal_skills
    / test_261022_m5_k1_prompt_carrier_migration）零触碰——本件对其零断言改动，
    server 全量基线由 redBaseline 命令守（292 passed 不降）。
  · 姊妹件 K3-E1/E3 结构面（栈落盘 / v12 守则）零交叉触碰。

Run protocol（文件名两点须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
  （standalone: python 本文件）
"""

from __future__ import annotations

import importlib.util
import json
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import yaml  # manifest 验收测试 case 前置；缺席 = infra 红（K1 同口径）
    _HAS_YAML = True
except ImportError:  # pragma: no cover
    _HAS_YAML = False

try:
    import pytest
    _HAS_PYTEST = True
except ImportError:  # pragma: no cover
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"
WORKBENCH = DEPLOY / "261022-openclaw-honcho-plugin"
DECL_PATH = WORKBENCH / "honcho-plugin-declaration.json"
APPLY_SCRIPT = WORKBENCH / "apply_honcho_plugin.py"
ROLLBACK_SCRIPT = WORKBENCH / "rollback_honcho_plugin.py"
MANIFEST_YAML = DEPLOY / "manifest.yaml"
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"
EVIDENCE_DIR = (REPO_ROOT / "delivery" / "taskpackages"
                / "261022-m5-k3-honcho-memory-production" / "memory-probes")

# --------------------------------------------------------------------------- #
# 冻结字面（变更 = 语义放宽/收紧，走 tighten-only 纪律反审）
# --------------------------------------------------------------------------- #

K3_GATE_MARKER = "kidpal honcho memory patch active"
K3_PEER_KEY_PREFIX = "agent:kidpal:openai-user:"
K3_PEER_KEY_VARIANT_RE = re.compile(r"agent:kidpal:(?!openai-user:)")
K3_HOOKS_PATH_SUFFIX = "hooks.allowConversationAccess"
K3_HONCHO_BASE_URL = "127.0.0.1:18100"
K3_P1_TOOLS_PATH = "agents.entries.kidpal.tools.allow"
K3_P1_TOOLS_EXPECT = ["read", "write", "exec", "web_search"]
K3_PROBE_ANCHORS = ("healthz", "/health", "readyz")
K3_DECL_NAME = "honcho-plugin-declaration.json"
K3_WORKBENCH_TOKEN = "261022-openclaw-honcho-plugin"
K3_SECRET_KEY_RE = re.compile(r"sk-[A-Za-z0-9]{16,}")
K3_TIMESTAMP_RE = re.compile(r"\d{8}T\d{6}")

_FRONT_MODULE_CACHE = None


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _iter_tree_text(root: Path):
    if not root.is_dir():
        return
    for p in sorted(root.rglob("*")):
        if p.is_file():
            txt = _read_text(p)
            if "\x00" in txt:
                continue
            yield p, txt


def _run_py(script: Path, args, timeout: int = 120):
    """用当前解释器跑 python 脚本（离线可跑，不经 bash/ssh）；脚本缺失 → rc 127。"""
    if not script.is_file():
        return 127, "", f"{script} 不在位（冻结契约工件缺失）"
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout)
    return proc.returncode, proc.stdout, proc.stderr


_MISSING = object()


def _json_get(obj, dotted: str):
    cur = obj
    for part in dotted.split("."):
        if not isinstance(cur, dict) or part not in cur:
            return _MISSING
        cur = cur[part]
    return cur


def _json_set(obj: dict, dotted: str, value) -> None:
    parts = dotted.split(".")
    cur = obj
    for p in parts[:-1]:
        nxt = cur.get(p)
        if not isinstance(nxt, dict):
            nxt = {}
            cur[p] = nxt
        cur = nxt
    cur[parts[-1]] = value


# --------------------------------------------------------------------------- #
# fixture（P1 已应用态组合态：K1 tools.allow 四项在位 + 261017 skills 白名单
# 在场 + hooks/plugin 键未设 = 本包 drift_guard 现状锚，launch-card 双声明式
# patch 顺序红线）
# --------------------------------------------------------------------------- #

def _fixture_p1_applied() -> dict:
    return {
        "gateway": {"token": "FIXTURE-GATEWAY-TOKEN-0123456789"},
        "tools": {"profile": "coding"},
        "agents": {
            "defaults": {"model": {"primary": "openclaw/kidpal"}},
            "entries": {
                "main": {"skills": ["kidpal-schedule-write"]},
                "kidpal": {
                    "prompt": "kidpal/base",
                    "skills": ["kidpal-schedule-answer", "weather", "tavily"],
                    "tools": {"allow": ["read", "write", "exec", "web_search"]},
                },
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _target_state(pre: dict, changes) -> dict:
    state = json.loads(json.dumps(pre))
    for change in changes:
        _json_set(state, change["path"], json.loads(json.dumps(change["value"])))
    return state


def _backups(td: Path, pattern: str) -> list[Path]:
    return sorted(td.glob(pattern))


# --------------------------------------------------------------------------- #
# front_app.py 离线装载（RDG/M4K1/SC 冻结件同款先例）
# --------------------------------------------------------------------------- #

def _ensure_fastapi_stub() -> None:
    try:
        import fastapi  # noqa: F401
        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return  # real fastapi（或带可用 FastAPI 类的 stub）在场 —— use it
    except ImportError:
        pass

    class _StubFastAPI:
        def __init__(self, *args, **kwargs):
            pass

        def _identity(self, *args, **kwargs):
            def deco(fn):
                return fn
            return deco

        get = post = websocket = _identity

    stub = ModuleType("fastapi")

    def _permissive_getattr(name):
        return Mock()

    stub.__getattr__ = _permissive_getattr  # type: ignore[method-assign]
    stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
    stub.HTTPException = type("HTTPException", (Exception,), {})
    stub.FastAPI = _StubFastAPI
    sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        resp = ModuleType("fastapi.responses")
        resp.__getattr__ = _permissive_getattr  # type: ignore[method-assign]
        sys.modules["fastapi.responses"] = resp


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)."""
    global _FRONT_MODULE_CACHE
    if _FRONT_MODULE_CACHE is not None:
        return _FRONT_MODULE_CACHE
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k3", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _FRONT_MODULE_CACHE = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error 是 RED，不是 crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须"
                      "离线可装载（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


class _FakeSseResponse:
    """urlopen 桩：SSE 双形态（delta 流 + [DONE]），context manager。"""

    def __init__(self, body: bytes):
        self._body = body

    def read(self):
        return self._body

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        return False


_SSE_BODY = (b'data: {"choices":[{"delta":{"content":"\xe5\xa5\xbd\xe7\x9a\x84"}}]}\n\n'
             b"data: [DONE]\n\n")


# --------------------------------------------------------------------------- #
# K3-E2 · 资格门（front 半 = 行为级；插件半 = 源锚代理）
# --------------------------------------------------------------------------- #


class K3MemoryIsolationAcceptance(unittest.TestCase):

    maxDiff = None

    def test_k3_e2_front_oneshot_payload_no_user(self):
        """coverage: K3-E2, S-F5.1-AC5 — arch T4 验收方式③ front 侧结构断言
        （行为级，离线真跑）：_openclaw_chat_once 的 payload 经
        openclaw_stream_request 单源构造，**不得**携带 user 键（键集恰
        {model, messages, stream}）——front 引导/整理一次性回合无设备身份
        （arch §6.7 六行表第 1/2 行），user 缺席即非资格（读写同门）。
        现状已合规 = must-stay-green 回归护栏：builder 若为隔离改 payload
        装配面，此断言红 = 越权（隔离判据在插件侧承载，不动 front 单源）。"""
        tag = "K3-E2"
        mod, err = _load_front_app()
        self.assertIsNotNone(mod, f"{tag} RED: front_app.py 离线不可装载 —— {err}")
        front = mod
        captured = []
        real_build = front.gw_provider.openclaw_stream_request

        def spy_build(messages):
            payload = real_build(messages)
            captured.append(payload)
            return payload

        orig_urlopen = None
        import urllib.request as _urlreq
        orig_urlopen = _urlreq.urlopen
        _urlreq.urlopen = lambda *a, **kw: _FakeSseResponse(_SSE_BODY)
        saved_build = front.gw_provider.openclaw_stream_request
        front.gw_provider.openclaw_stream_request = spy_build
        try:
            reply = front._openclaw_chat_once(
                "整理制品指令（request 级 system）",
                "孩子讲的内容：\n我昨天去了动物园。\n\n请按指令把这段话忠实整理成一篇。",
                timeout_s=5.0)
        finally:
            front.gw_provider.openclaw_stream_request = saved_build
            _urlreq.urlopen = orig_urlopen
        self.assertEqual(
            len(captured), 1,
            f"{tag} RED: _openclaw_chat_once 未恰一次经 openclaw_stream_request "
            "构造 payload（单源构造被旁路 = 契约 §四禁 max_tokens 约束失去唯一"
            "承载点）。")
        payload = captured[0]
        self.assertEqual(
            set(payload), {"model", "messages", "stream"},
            f"{tag} RED: front 一次性回合 payload 键集 = {sorted(payload)}，须恰 "
            "{model, messages, stream} —— payload 不得携带 user（资格门唯一判据："
            "user 缺席即非资格，arch §6.7 六行表第 1/2 行）或缺 model/stream。")
        self.assertNotIn(
            "user", payload,
            f"{tag} RED: front 一次性回合 payload 携带 user 键 —— 一次性回合将"
            "被 OpenClaw 当设备身份持久会话并入库（hooks.allowConversationAccess"
            "=true 后一次性路径钩子即触发，debt 证据 7）= AC5「设备流程触发的"
            "回合不入画像」结构性失守。")
        self.assertEqual(
            [m.get("role") for m in payload["messages"]], ["system", "user"],
            f"{tag} RED: 一次性回合 messages 形状 = "
            f"{[m.get('role') for m in payload['messages']]}，须恰 [system, user]"
            "（request 级 system 指令 + 单条本回合输入）。")
        self.assertEqual(
            reply, "好的",
            f"{tag} RED(infra): SSE 解析桩返回不符预期（{reply!r}）—— 测试自身"
            "缺陷，非被测面。")

    def test_k3_e2_gate_carrier_anchors(self):
        """coverage: K3-E2 — 资格门承载件锚集（源锚代理，工作台并集）：marker
        字面（探针门控锚）+ peer 键锚（peer 键不另造）+ 钩子授权锚 + baseUrl
        锚；marker 所在件须含可达性探针锚；工作台内 "agent:kidpal:" 键族唯一
        形态 = agent:kidpal:openai-user:（任何变体 peer 键 = 红）；零内联
        secret。"""
        tag = "K3-E2"
        self.assertTrue(
            WORKBENCH.is_dir(),
            f"{tag} RED: {WORKBENCH} 不在世 —— A2 归属本包的插件工作台缺失"
            "（资格门 / 钩子授权 / 降级护栏无承载面；K3-E2/E5/E6 三条共建此目录）。")
        texts = {str(p.relative_to(WORKBENCH)): txt
                 for p, txt in _iter_tree_text(WORKBENCH)}
        self.assertTrue(texts, f"{tag} RED: 工作台零文件。")
        union = "\n".join(texts.values())
        for anchor in (K3_GATE_MARKER, K3_PEER_KEY_PREFIX,
                       K3_HOOKS_PATH_SUFFIX, K3_HONCHO_BASE_URL):
            self.assertIn(
                anchor, union,
                f"{tag} RED: 工作台并集缺锚 {anchor!r} —— 资格门承载件冻结锚集"
                "缺失（marker = ARQ-260907 同族机械报警锚；peer 键 = P2 键族不"
                "另造；allowConversationAccess = debt 证据 7 钩子授权；baseUrl = "
                "指本机 loopback）。")
        marker_files = [name for name, txt in texts.items()
                        if K3_GATE_MARKER in txt]
        self.assertTrue(
            marker_files, f"{tag} RED: marker 字面无承载件（前置）。")
        for name in marker_files:
            self.assertTrue(
                any(a in texts[name] for a in K3_PROBE_ANCHORS),
                f"{tag} RED: marker 承载件 {name} 零可达性探针锚 "
                f"（{K3_PROBE_ANCHORS} 任一）—— marker 未由 Honcho 可达性探针门控："
                "插件装了但 Honcho 不可达时静默照打 marker = 假绿报警锚"
                "（K2b 门控先例同族；memory 死了还报「patch active」）。")
            self.assertTrue(
                K3_PEER_KEY_PREFIX in texts[name],
                f"{tag} RED: marker 承载件 {name} 不含 peer 键锚 —— 资格门与"
                "报警锚必须同件承载（marker 在、门在别处 = 门可被单独移除）。")
        variants = K3_PEER_KEY_VARIANT_RE.findall(union)
        self.assertEqual(
            variants, [],
            f"{tag} RED: 工作台出现非 openai-user 形态的 agent:kidpal 键族变体 "
            f"{variants} —— peer 键不另造（launch-card 红线：消费 P2 的 "
            "agent:kidpal:openai-user:<device_id> 键，device 级 peer 先行）。")
        leaks = K3_SECRET_KEY_RE.findall(union)
        self.assertEqual(
            leaks, [], f"{tag} RED: 工作台内联 secret（sk- 长键）—— 凭据不入仓。")

    def test_k3_e5_degradation_guards(self):
        """coverage: K3-E5 — 降级护栏（源锚代理）：marker 承载件含 bounded
        失败形态（catch/except）；零无界重试风暴（while(true)/while True）。
        行为本体（不阻塞、不静默重试风暴、失败不进用户可见兜底）= 部署轮
        真栈实证（K3-E7 live 申报面），host 不造假绿。"""
        tag = "K3-E5"
        self.assertTrue(
            WORKBENCH.is_dir(),
            f"{tag} RED: {WORKBENCH} 不在世 —— 降级护栏无承载面。")
        texts = {str(p.relative_to(WORKBENCH)): txt
                 for p, txt in _iter_tree_text(WORKBENCH)}
        marker_files = [name for name, txt in texts.items()
                        if K3_GATE_MARKER in txt]
        self.assertTrue(
            marker_files,
            f"{tag} RED: marker 承载件不在世（K3-E2 未落，先修 E2）。")
        for name in marker_files:
            carrier = texts[name]
            self.assertTrue(
                ("catch" in carrier) or ("except" in carrier),
                f"{tag} RED: 承载件 {name} 零 bounded 失败形态（catch/except）—— "
                "persist/inject 路径无失败护栏，Honcho 不可达 = 整轮对话报错"
                "（K3-E5：记忆失败不阻塞对话、不进用户可见兜底）。")
            self.assertIsNone(
                re.search(r"while\s*\(\s*true\s*\)", carrier, re.IGNORECASE),
                f"{tag} RED: 承载件 {name} 含 while(true) —— 无界重试风暴形态"
                "（K3-E5：不静默重试风暴；重试须有界或直接降级）。")
            self.assertIsNone(
                re.search(r"while\s+True\s*:", carrier),
                f"{tag} RED: 承载件 {name} 含 while True —— 无界重试风暴形态。")


# --------------------------------------------------------------------------- #
# K3-E6 · openclaw.json 声明式 patch 组合（P1 已应用态为现状锚）
# --------------------------------------------------------------------------- #


class K3OpenclawPatchAcceptance(unittest.TestCase):

    maxDiff = None

    def _require_workbench(self, tag):
        self.assertTrue(
            WORKBENCH.is_dir(),
            f"{tag} RED: {WORKBENCH} 不在世 —— 声明式 patch 工作台缺失"
            "（K3-E6 本体：Honcho 插件 + hooks 授权声明 + apply + rollback）。")

    def _decl(self, tag) -> dict:
        self.assertTrue(
            DECL_PATH.is_file(),
            f"{tag} RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway.token 整文件不入仓，只入无 secret 声明"
            "（K3-E6 铁律）。")
        txt = _read_text(DECL_PATH)
        try:
            decl = json.loads(txt)
        except ValueError as e:  # noqa: BLE001
            self.fail(f"{tag} RED: 声明工件不可解析 —— {e!r}")
        self.assertIsInstance(decl, dict, f"{tag} RED: 声明工件顶层须为 object。")
        return decl

    def test_k3_e6_declaration_schema_and_p1_drift_anchor(self):
        """coverage: K3-E6 — 声明工件 schema（case/changes/drift_guard/backup/
        rollback 五键）；changes ≥2 且含 hooks.allowConversationAccess=true 与
        honcho baseUrl 127.0.0.1:18100 两判据字面；drift_guard 必含 P1 已应用
        态现状锚（tools.allow = 契约四项）——launch-card 红线：本包 drift_guard
        以 P1 已应用态为现状锚（非 K1 的 unset 形态）；backup/rollback 同形制。"""
        tag = "K3-E6"
        self._require_workbench(tag)
        decl = self._decl(tag)
        for key in ("case", "changes", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"{tag} RED: 声明工件缺必需顶层键 {key!r}（冻结契约结构 = case / "
                "changes / drift_guard / backup / rollback，261017/K1-E4 同族）。")
        self.assertIsInstance(decl.get("case"), str)
        self.assertTrue(str(decl.get("case", "")).strip(), f"{tag} RED: case 空。")

        changes = decl.get("changes")
        self.assertIsInstance(changes, list, f"{tag} RED: changes 须为 list。")
        self.assertGreaterEqual(
            len(changes), 2,
            f"{tag} RED: changes 条数 = {len(changes)} < 2 —— 本包声明至少两条："
            "钩子授权 + Honcho 插件注册（K3-E6 双声明式 patch）。")
        for i, change in enumerate(changes):
            self.assertIsInstance(
                change, dict, f"{tag} RED: changes[{i}] 须为 object。")
            self.assertTrue(
                isinstance(change.get("path"), str) and change["path"].strip(),
                f"{tag} RED: changes[{i}].path 须为非空点分路径。")
            self.assertIn(
                "value", change,
                f"{tag} RED: changes[{i}] 缺 value（本包声明形状 = {{path, value}}，"
                "非 K1 的 {{path, allow}}——插件配置为任意 json 子树）。")
        hooks = [c for c in changes
                 if str(c.get("path")).endswith(K3_HOOKS_PATH_SUFFIX)]
        self.assertEqual(
            len(hooks), 1,
            f"{tag} RED: 以 {K3_HOOKS_PATH_SUFFIX} 结尾的 change 数 = "
            f"{len(hooks)}，须恰 1（debt 证据 7 钩子授权字面；多次声明 = 双写）。")
        self.assertIs(
            hooks[0].get("value"), True,
            f"{tag} RED: 钩子授权 value = {hooks[0].get('value')!r}，须为 true —— "
            "allowConversationAccess=false 则插件钩子不触发、记忆管线整体不工作；"
            "但无资格门先行的裸 true = debt 证据 7 的一次性回合入库路径。")
        honcho = [c for c in changes
                  if "honcho" in str(c.get("path")).lower()
                  and K3_HONCHO_BASE_URL in json.dumps(c.get("value"), ensure_ascii=False)]
        self.assertTrue(
            honcho,
            f"{tag} RED: 零 change 的 value 携带 baseUrl {K3_HONCHO_BASE_URL} —— "
            "插件 baseUrl 须指本机 loopback（debt 证据 7「baseUrl 指本机」）。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, f"{tag} RED: drift_guard 须为 list。")
        self.assertTrue(dg, f"{tag} RED: drift_guard 不得为空。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict, f"{tag} RED: drift_guard[{i}] 须为 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry["path"].strip(),
                f"{tag} RED: drift_guard[{i}].path 须为非空字符串。")
            self.assertIn(
                "expect", entry, f"{tag} RED: drift_guard[{i}] 缺 expect。")
        p1_anchor = [e for e in dg if e.get("path") == K3_P1_TOOLS_PATH]
        self.assertTrue(
            p1_anchor,
            f"{tag} RED: drift_guard 未锚定 {K3_P1_TOOLS_PATH} —— launch-card "
            "红线：本包 drift_guard 现状锚 = P1 已应用态（tools.allow = 契约四项"
            "在位），P1 漂移时本包必须 STOP 而非叠加写。")
        self.assertEqual(
            p1_anchor[0].get("expect"), K3_P1_TOOLS_EXPECT,
            f"{tag} RED: P1 现状锚 expect = {p1_anchor[0].get('expect')!r}，须恰 "
            f"{K3_P1_TOOLS_EXPECT}（K1-E4 已应用态逐字）。")

        backup = decl.get("backup")
        self.assertIsInstance(backup, dict, f"{tag} RED: backup 须为 object。")
        self.assertTrue(
            isinstance(backup.get("dir"), str) and backup["dir"].strip(),
            f"{tag} RED: backup.dir 须为非空字符串。")
        pattern = backup.get("pattern")
        self.assertTrue(
            isinstance(pattern, str) and "bak" in pattern,
            f"{tag} RED: backup.pattern 须含 'bak'（config 同目录 glob）。")
        self.assertNotIn("/", pattern, f"{tag} RED: backup.pattern 不得含路径分隔符。")
        rollback = decl.get("rollback")
        script = (rollback or {}).get("script")
        self.assertTrue(
            isinstance(script, str) and script.strip(),
            f"{tag} RED: rollback.script 须为非空字符串。")
        self.assertEqual(
            script, "rollback_honcho_plugin.py",
            f"{tag} RED: rollback.script = {script!r}，冻结契约 = "
            "rollback_honcho_plugin.py。")
        self.assertTrue(
            (WORKBENCH / script).is_file(),
            f"{tag} RED: rollback 脚本 {script!r} 不在工作台 —— 回滚不可跑。")
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"{tag} RED: apply_honcho_plugin.py 不在位 —— 冻结契约工件缺失。")
        apply_src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            K3_DECL_NAME, apply_src,
            f"{tag} RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)",
                      apply_src),
            f"{tag} RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖、"
            "不依赖 jq）。")

    def _apply_and_check(self, tag, cfg_path: Path, pre: dict, changes):
        rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg_path, "--decl", DECL_PATH])
        self.assertEqual(
            rc, 0, f"{tag} RED: apply rc={rc}（应 0）。stdout={out!r} stderr={err!r}")
        got, gerr = _load_json_file(cfg_path)
        self.assertIsNone(gerr, f"{tag} RED: apply 后 config 不可解析: {gerr}")
        return got

    def test_k3_e6_apply_idempotent_and_preserves_p1_state(self):
        """coverage: K3-E6 — apply 真跑（P1 已应用态 fixture）：① 首跑 → 全部
        changes 目标态落地，且 P1 组合态零触碰（tools.allow 四项 / 261017
        skills / gateway.token / 全局 tools / modelPolicy 原样）；② 二跑 →
        幂等 SKIP（rc 0、零语义变更、零新备份）——双声明式 patch 顺序组合
        （P1 先、P3 后）互不覆盖。"""
        tag = "K3-E6"
        decl = self._decl(tag)
        changes = decl["changes"]
        pattern = decl["backup"]["pattern"]
        pre = _fixture_p1_applied()
        want = _target_state(pre, changes)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json_file(cfg, pre)

            got = self._apply_and_check(tag, cfg, pre, changes)
            self.assertEqual(
                got, want,
                "K3-E6 RED: apply 未产出全部 changes 的声明目标态（逐键 deep-equal）。")
            self.assertEqual(
                _json_get(got, K3_P1_TOOLS_PATH), K3_P1_TOOLS_EXPECT,
                "K3-E6 RED: apply 改动了 P1 tools.allow —— 双声明式 patch 组合"
                "被破（本包只叠加 hooks/插件声明，P1 已应用态是现状锚与保留面）。")
            self.assertEqual(
                _json_get(got, "gateway.token"), _json_get(pre, "gateway.token"),
                "K3-E6 RED: apply 改动了 gateway.token —— 定点 patch 越界触碰"
                " secret。")
            self.assertEqual(
                _json_get(got, "agents.entries.kidpal.skills"),
                _json_get(pre, "agents.entries.kidpal.skills"),
                "K3-E6 RED: apply 抹掉了 261017 skills 白名单组合态 —— 声明式"
                " patch 组合共存被破（launch-card：两包均幂等 apply、不整文件改写）。")
            self.assertEqual(
                _json_get(got, "agents.entries.main"), _json_get(pre, "agents.entries.main"),
                "K3-E6 RED: apply 改动了其它 agent 条目 —— 定点 patch 越界。")
            backups_after_first = _backups(td, pattern)
            self.assertTrue(
                backups_after_first, "K3-E6 RED: 首次 apply 未建备份（前置）。")

            rc2, out2, err2 = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            got2, _ = _load_json_file(cfg)
            self.assertEqual(
                rc2, 0,
                f"K3-E6 RED: apply 二次运行 rc={rc2}（幂等应 0）。"
                f"stdout={out2!r} stderr={err2!r}")
            self.assertEqual(
                got2, want, "K3-E6 RED: apply 二次运行改动了已达标态（非幂等）。")
            self.assertEqual(
                len(_backups(td, pattern)), len(backups_after_first),
                "K3-E6 RED: apply 二次运行又新建了备份（幂等应不重备份）。")

    def test_k3_e6_apply_drift_guard_exit3(self):
        """coverage: K3-E6 — drift_guard 现状锚（P1 已应用态）不中 → STOP
        exit 3、零写、零备份：P1 tools.allow 被漂移（改白名单 / 键缺席）时
        本包必须拒绝叠加——双声明式 patch 的顺序纪律机械承载。"""
        tag = "K3-E6"
        decl = self._decl(tag)
        pattern = decl["backup"]["pattern"]
        for mutate, label in (
                (lambda s: _json_set(s, K3_P1_TOOLS_PATH, ["bash", "browser"]),
                 "tools.allow 被改成别的白名单"),
                (lambda s: s["agents"]["entries"]["kidpal"].pop("tools"),
                 "tools.allow 键缺席（P1 已应用态丢失）")):
            with tempfile.TemporaryDirectory() as td:
                td = Path(td)
                cfg = td / "openclaw.json"
                drifted = _fixture_p1_applied()
                mutate(drifted)
                _write_json_file(cfg, drifted)
                rc, out, err = _run_py(
                    APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
                self.assertEqual(
                    rc, 3,
                    f"K3-E6 RED: drift-guard 不中（{label}）应 exit 3（实得 "
                    f"rc={rc}）。stdout={out!r} stderr={err!r}")
                after, _ = _load_json_file(cfg)
                self.assertEqual(
                    after, drifted,
                    f"K3-E6 RED: drift-guard 拒绝（{label}）后仍改写了 config。")
                self.assertEqual(
                    _backups(td, pattern), [],
                    f"K3-E6 RED: drift-guard 拒绝（{label}）后仍建了备份"
                    "（拒绝 = 完全 no-op）。")

    def test_k3_e6_apply_timestamped_backup(self):
        """coverage: K3-E6 — 写前时间戳备份：config 同目录、匹配声明 pattern、
        文件名含 UTC yyyymmddTHHMMSS、内容 = 原始 config（P1 已应用态）。"""
        tag = "K3-E6"
        decl = self._decl(tag)
        pattern = decl["backup"]["pattern"]
        pre = _fixture_p1_applied()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json_file(cfg, pre)
            rc, _out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K3-E6 RED: apply rc={rc}。stderr={err!r}")
            backups = _backups(td, pattern)
            self.assertTrue(
                backups, f"{tag} RED: apply 未建匹配 backup.pattern 的备份。")
            self.assertRegex(
                backups[0].name, K3_TIMESTAMP_RE.pattern,
                f"{tag} RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳。")
            saved, _ = _load_json_file(backups[0])
            self.assertEqual(
                saved, pre,
                f"{tag} RED: 备份内容 ≠ 变更前 config（P1 已应用态快照失真）。")

    def test_k3_e6_rollback_dry_run_and_restore(self):
        """coverage: K3-E6 — 回滚脚本 --dry-run 可跑（exit 0）且零写入；非
        dry-run 一步还原 backup 内容（exit 0）。"""
        tag = "K3-E6"
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"{tag} RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-honcho-20261023T000000"
            cur = {"applied": True}
            restored = {"applied": False}
            _write_json_file(cfg, cur)
            _write_json_file(bak, restored)

            rc, _out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0, f"{tag} RED: rollback --dry-run rc={rc}（应 0）。stderr={err!r}")
            after, _ = _load_json_file(cfg)
            self.assertEqual(after, cur, f"{tag} RED: --dry-run 改写了 config。")

            rc2, _out2, err2 = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(rc2, 0, f"{tag} RED: rollback rc={rc2}。stderr={err2!r}")
            after2, _ = _load_json_file(cfg)
            self.assertEqual(
                after2, restored, f"{tag} RED: 回滚未还原 backup 内容。")

    def test_k3_e6_no_openclaw_json_in_repo_and_no_secret(self):
        """coverage: K3-E6 — openclaw.json 整文件不入仓（含 gateway 凭据）；
        声明工件零 secret（gateway.token 字样 / token 键 / sk- 长键零出现）。"""
        tag = "K3-E6"
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            f"{tag} RED: 仓内出现 openclaw.json（含 gateway 凭据）—— 整文件"
            f"零入仓（launch-card 红线）。实得：{leaked}")
        self.assertTrue(
            DECL_PATH.is_file(),
            f"{tag} RED: 声明工件不在位（前置）——负锚无承载面。")
        decl_text = _read_text(DECL_PATH)
        self.assertNotIn(
            "gateway.token", decl_text,
            f"{tag} RED: 声明工件出现 gateway.token 字样。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            f"{tag} RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")
        self.assertEqual(
            K3_SECRET_KEY_RE.findall(decl_text), [],
            f"{tag} RED: 声明工件内联 secret（sk- 长键）。")

    def test_k3_e6_manifest_registers_the_workbench(self):
        """coverage: K3-E6 — manifest 对 261022-openclaw-honcho-plugin 登记 ≥3 条
        （声明 / apply / rollback），src 在仓内存在、dst 在
        /opt/kidpal/openclaw-patch/261022-openclaw-honcho-plugin/ 消费位
        （261013 部署单一事实源纪律）。"""
        tag = "K3-E6"
        self._require_workbench(tag)
        self.assertTrue(
            _HAS_YAML or self.fail(
                f"{tag} RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(MANIFEST_YAML.is_file(), f"{tag} RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001
            self.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
        entries = [
            e for e in (doc or {}).get("entries", []) or []
            if isinstance(e, dict) and K3_WORKBENCH_TOKEN in (
                str(e.get("src") or "") + str(e.get("dst") or ""))]
        self.assertGreaterEqual(
            len(entries), 3,
            f"{tag} RED: manifest 对 {K3_WORKBENCH_TOKEN} 的登记条目 "
            f"{len(entries)} < 3 —— 声明 / apply / rollback 须各入部署单一事实源。")
        self.assertTrue(
            any(str(e.get("src") or "").endswith(K3_DECL_NAME) for e in entries),
            f"{tag} RED: manifest 未登记声明工件 {K3_DECL_NAME}。")
        srcs = [str(e.get("src") or "") for e in entries]
        missing = [s for s in srcs if s and not (REPO_ROOT / s).is_file()]
        self.assertEqual(missing, [], f"{tag} RED: 登记的仓内源缺失: {missing}。")
        bad_dst = [str(e.get("dst") or "") for e in entries
                   if not str(e.get("dst") or "").startswith(
                       "/opt/kidpal/openclaw-patch/" + K3_WORKBENCH_TOKEN)]
        self.assertEqual(
            bad_dst, [], f"{tag} RED: 登记条目 dst 越出 openclaw-patch 消费位: {bad_dst}。")


def _load_json_file(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} 缺失或为空"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} 非法 JSON: {e!r}"


def _write_json_file(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")


# --------------------------------------------------------------------------- #
# LIVE 齿 —— K3-E2 marker 探针 / K3-E3 seeded 负向探针集
# （@pytest.mark.live，不 auto-run；conftest 机械 skip，ARQ-260901-g3）
# --------------------------------------------------------------------------- #

_LIVE_MARKER_SCHEMA_NOTE = (
    "marker 探针证据 schema = {main_link: [{marker, db_hit, log_hit}], "
    "one_shot: [{round, marker, db_hit, log_hit}], "
    "read_side: {non_qualified_injection}}；seeded 证据 schema = "
    "{probes: [{category, seed, query, profile_hit, reply_hit}]}（冻结于本件头注 D 节）")


class K3MemoryProbeLiveAcceptance(unittest.TestCase):
    """live 证据判定齿：arch T4 验收方式①②（marker 探针 DB+日志双证——主链路
    正证 / 一次性回合负证）+ 读侧同门 + T4 方式④（seeded 负向探针集 100% 零
    命中，S-F5.1-AC3 / NFR-PRIV-1 量化口径）。

    probe ≠ exercise 注记（DEV-GOVERNANCE 验收测试纪律 §2）：本类只对 live 相位
    **真实产出**的结构化证据施加判定；真实上 ECS 发探针（含 DB/日志双证采集）
    归部署轮执行（Pod 不自行上 ECS，awaiting-human:live-deploy）。证据缺位 =
    live 相位设计态红（不 auto-run）。
    """

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _evidence_files(self, tag) -> list[Path]:
        files = sorted(EVIDENCE_DIR.glob("*.json")) if EVIDENCE_DIR.is_dir() else []
        self.assertTrue(
            files,
            f"{tag} LIVE RED: {EVIDENCE_DIR} 零结构化探针证据 —— 部署轮须先把 "
            f"marker 探针 / seeded 负向集证据落盘（{_LIVE_MARKER_SCHEMA_NOTE}）。",
        )
        return files

    def test_live_k3_e2_marker_probes_db_log_double_evidence(self):
        """coverage: K3-E2 — marker 探针（DB+日志双证）：主链路正证（携 user
        真实聊天回合 marker 入 DB 且入插件日志）、一次性回合负证（guide 与
        organize 两类回合 marker 零 DB 零日志）、读侧同门（非资格回合零画像
        注入）。逐条判定，任一命中形态不符 = live 红。"""
        tag = "K3-E2(live)"
        docs = []
        for p in self._evidence_files(tag):
            obj, err = _load_json_file(p)
            self.assertIsNotNone(obj, f"{tag} LIVE RED: {p} 不可解析: {err}")
            if "main_link" in obj:
                docs.append((p, obj))
        self.assertTrue(
            docs,
            f"{tag} LIVE RED: 证据文件缺 main_link 段（{_LIVE_MARKER_SCHEMA_NOTE}）。")
        positive = 0
        one_shot_rounds: set = set()
        for p, obj in docs:
            for item in obj.get("main_link") or []:
                positive += 1
                self.assertTrue(
                    item.get("db_hit") and item.get("log_hit"),
                    f"{tag} LIVE RED: {p.name} 主链路正证 marker {item.get('marker')!r} "
                    f"双证不全（db_hit={item.get('db_hit')} log_hit={item.get('log_hit')}）"
                    " —— 携 user 的真实聊天回合须 DB+日志双证命中（arch T4 方式②）。",
                )
            for item in obj.get("one_shot") or []:
                one_shot_rounds.add(item.get("round"))
                self.assertFalse(
                    item.get("db_hit") or item.get("log_hit"),
                    f"{tag} LIVE RED: {p.name} 一次性回合（{item.get('round')}）marker "
                    f"{item.get('marker')!r} 出现命中（db_hit={item.get('db_hit')} "
                    f"log_hit={item.get('log_hit')}）—— 设备流程回合不入画像被破"
                    "（S-F5.1-AC5 / arch §6.7 六行表；零命中 = DB 与日志均不得出现）。",
                )
            read_side = obj.get("read_side") or {}
            if "non_qualified_injection" in read_side:
                self.assertIs(
                    read_side.get("non_qualified_injection"), False,
                    f"{tag} LIVE RED: {p.name} read_side.non_qualified_injection = "
                    "true —— 非资格回合被注入画像（读写同门被破：非资格回合不写"
                    " observation、不注入画像，arch T4）。")
        self.assertGreaterEqual(
            positive, 1,
            f"{tag} LIVE RED: 主链路正证零条 —— 缺携 user 真实聊天回合的 marker "
            "双证（arch T4 方式②）。")
        self.assertTrue(
            {"guide", "organize"} <= one_shot_rounds,
            f"{tag} LIVE RED: 一次性回合负证未覆盖 guide 与 organize 两类"
            f"（实得 {sorted(r or '' for r in one_shot_rounds)}）—— arch §6.7 "
            "六行表第 1/2 行各须一次负证探针。")

    def test_live_k3_e3_seeded_negative_probe_set_zero_hit(self):
        """coverage: K3-E3, S-F5.1-AC3, NFR-PRIV-1 — seeded 负向探针集：注入
        家长姓名 / 电话 / 家庭安排后再问询（含变着问法套问），画像与答复
        100% 零命中。三档 category 各 ≥1、全部 probes 双零——任一命中即 live 红
        （三层纵深任一失守即不达标：来源门 / 抽取策略 / 注入过滤）。"""
        tag = "K3-E3(live)"
        docs = []
        for p in self._evidence_files(tag):
            obj, err = _load_json_file(p)
            self.assertIsNotNone(obj, f"{tag} LIVE RED: {p} 不可解析: {err}")
            if "probes" in obj:
                docs.append((p, obj))
        self.assertTrue(
            docs,
            f"{tag} LIVE RED: 证据文件缺 seeded probes 段"
            f"（{_LIVE_MARKER_SCHEMA_NOTE}）。")
        categories: set = set()
        total = 0
        for p, obj in docs:
            for probe in obj.get("probes") or []:
                total += 1
                categories.add(probe.get("category"))
                self.assertFalse(
                    probe.get("profile_hit") or probe.get("reply_hit"),
                    f"{tag} LIVE RED: {p.name} seeded 探针（{probe.get('category')}："
                    f"{probe.get('seed')!r} → {probe.get('query')!r}）出现命中"
                    f"（profile_hit={probe.get('profile_hit')} "
                    f"reply_hit={probe.get('reply_hit')}）—— 家长隐私被记忆/复述"
                    "（S-F5.1-AC3 / NFR-PRIV-1 ①：注入式探针 100% 不回忆；"
                    "三层纵深任一失守即不达标）。",
                )
        self.assertGreaterEqual(
            total, 3, f"{tag} LIVE RED: seeded 探针总数 = {total} < 3（三档各 ≥1）。")
        self.assertTrue(
            {"姓名", "电话", "家庭安排"} <= categories,
            f"{tag} LIVE RED: seeded 探针 category 未覆盖三档（实得 "
            f"{sorted(c or '' for c in categories)}）—— 家长姓名 / 联系电话 / "
            "家庭安排三类注入缺一不可（S-F5.1-AC3 枚举）。",
        )


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
