"""Frozen acceptance tests for TaskPackage 260829-voice-roundtrip
(subsystem: firmware+server; 方向 B 语音回环 + display 消费链路; mergeOrder 5;
本包 = 包 4/5/6/8 的设备 display 路由共享底座, PROPOSAL §2 包 2 / §4).

PATH 归位注记（acceptance-author, 2026-09-07）: TaskPackage
`acceptanceTestsFile` 钉的 python 侧路径是 `server/tests/
test_voice_roundtrip_b.acceptance.py`，但 acceptance-author 的机械写边界
（enforce-scope）只放行 `server/**/acceptance/**`，`server/tests/**` 被拦。
故本文件落盘于 scope 内的 `server/acceptance/`（与既有冻结验收同目录、同收集
通路），planner / 有写权的收口方二选一归位： (a) 更新 taskpackage 的
acceptanceTestsFile 为本路径； 或 (b) 将本文件原样移至 server/tests/ 并在
server/pytest.ini 加 `addopts = --import-mode=importlib`。两方案收集口径见下
「Collection discipline」，断言零变更。

Nature: hybrid acceptance — OFFLINE-DETERMINISTIC / STRUCTURAL teeth (auto-run;
the red baseline) + LIVE faces (@pytest.mark.live, SSH to the KidPal ECS box
8.137.91.16; NOT auto-run by any merge gate, ARQ-260901-g3-server-live-acceptance
discipline). A bare import/collection error must never be the reason a test is
red — every assertion below pins a REAL missing behaviour or a landed face it
guards. The device-face twin is firmware/main/test/voice_roundtrip_b.acceptance.c
(hybrid co-check tooth below pins the twin stays aligned).

Coverage map (acceptanceRef / exitCriteria -> test method):
  VRT-A2 (流式语音回环, cloud cooperation) -> test_a2_protocol_profile_twin_aligned
          [offline; §四 stt/tts 状态机协议形状的 C/Python 孪生对齐守卫; 设备行为
          断言在 C 侧, live 链路由 WS 包冒烟 + 本包 A2/E2 证据承载]
  VRT-A3 (display mcp 消费端, cloud cooperation) -> test_a3_cloud_display_envelope
          [offline green-guard of the landed G face: kidpal/display JSON-RPC
          tools/call 信封 (mcp_relay) + display_schema 白名单 = 契约 §1.3/§1.4;
          设备侧消费断言在 C 侧 VRT-A3]
  VRT-A4 (display 格式遵循度复验, 契约待确认点 1 — 方向 B 网关切分路径不复用
          HTTP 期 12/12 旧结论, 统计性口径; intake 裁定: 遵循度走 fence 路径,
          不依赖 OpenClaw tools)
        -> test_a4_adherence_recheck_evidence_record      [offline; RED today]
        -> test_a4_live_display_adherence_recheck         [live]
  VRT-A5 (网关兜底设备侧消费; 生成逻辑归 G 已 landed, 本包验消费链路)
        -> test_a5_cloud_fallback_alignment               [offline green-guard:
          fallback.py 生成面形状 + 与 C 侧 text-view 消费面对齐]
  VRT-A6 (abort: 设备发 abort + 云端取消生成; 本地 AEC+VAD 即时停播主地位不变)
        -> test_a6_abort_cloud_cancel_evidence            [offline; RED today]
  VRT-A7 (F4/S-F4.1 通用问答) -> test_a7_system_prompt_f4_discipline_offline
          [offline; RED today] -> test_a7_live_f4_general_qa_chain [live]
  VRT-E4 (OpenClaw 基础 system prompt 版本化落盘, 供包 4/包 7 增量扩展)
        -> test_e4_system_prompt_versioned_landed         [offline; RED today]
        -> test_e4_system_prompt_wired_into_deploy_overlay [offline; RED today]
  live precondition -> test_live_ssh_reachable             [live]

FROZEN artifact — OpenClaw 基础 system prompt 落盘路径（钉死）:
  server/deploy/openclaw-system-prompt.v1.md
  Rationale: (1) 该 prompt 是 OpenClaw 大脑的配置/声明面（契约 §1.6「通过 system
  prompt 让 DeepSeek 始终按此格式回复」），与 provider.config.yaml 同属
  server/deploy/ 的网关/大脑声明制品族，随 deploy overlay 上 ECS，不是 python
  运行时代码； (2) 版本化文件名（.v1）+ 独立声明目录 = 「版本化落盘 + 可被包
  4（char 扩展）/包 7（提醒备注解析约定）增量扩展」的最直接承载——后续版本以
  .v2 新文件落盘、overlay 渲染切换，历史版本可 diff； (3) live 遵循度/问答探针
  （A4/A7 live）以本文件内容为唯一 prompt 源，保证「验收所测 = 生产所用」。

FROZEN device-face offline contract (cross-reference; asserted in the C twin):
  kidpal_display_route(view) / kidpal_display_control_action(action) /
  kidpal_display_text_page_count(text, screen_lines) — numeric codes and
  semantics are frozen in firmware/main/test/voice_roundtrip_b.acceptance.c.
  This file does NOT re-assert them; the twin-alignment tooth guards the twin
  file exists and keeps carrying the §四 profile.

Evidence dir: delivery/changes/dev/260829-voice-roundtrip/ (G 包 delivery/changes/dev/ 先例).
  - live 冒烟证据（设备侧, C 证据验收消费）: *roundtrip*/*evidence* 记录件。
  - a4-adherence-samples.md: A4 live 统计样本 + 失败样本处置记录（live 面写）。
  - abort 取消生成观察记录（A6 offline 验收消费）。

Red baseline (vehicle state, 2026-09-07 acceptance-author 实测):
  - server/deploy/ 无任何 system prompt 版本化文件, deploy overlay 不引用之
    （E4 双齿红）; delivery/changes/dev/260829-voice-roundtrip/ 不存在（A4/A6 证据齿红）。
  - G 包已 landed 的 gateway 面（mcp_relay/display_schema/fallback）按冻结
    offline contract 断言为绿色守卫（对齐齿, 防漂移）。
  - live 面（A4 统计复验 / A7 问答链路）不在红基线 auto-run 范围（@live）。
  See delivery/taskpackages/260829-voice-roundtrip/red-baseline-summary.md.

Collection discipline:
  - 本文件名含两个点（test_*.acceptance.py），pytest 默认 prepend import 模式
    无法派生合法模块名（G 包先例, ModuleNotFoundError），须 importlib 模式：
      cd server && python -m pytest acceptance/test_voice_roundtrip_b.acceptance.py -q --import-mode=importlib
    offline 类收集 + 运行；live 类被 @pytest.mark.live marker 收集但不 auto-run
    （merge 环境跑法 = `-m "not live"` 或显式 `-m live`）。若收口方按归位注记
    (b) 把文件移去 server/tests/，则在 server/pytest.ini 加
    `addopts = --import-mode=importlib` 使裸 pytest 命令即可收集。
  - 本模块带 standalone __main__ runner（Pod S / G 包先例），默认只跑 offline
    类；VRT_RUN_LIVE=1 时追加 live 类（需 SSH + ECS 环境）。
"""

from __future__ import annotations

import base64
import importlib.util
import json
import os
import re
import subprocess
import sys
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
DEPLOY_DIR = SERVER / "deploy"
EVIDENCE_DIR = REPO_ROOT / "delivery" / "changes" / "dev" / "260829-voice-roundtrip"
C_TWIN = REPO_ROOT / "firmware" / "main" / "test" / "voice_roundtrip_b.acceptance.c"

# FROZEN artifact path (钉死; rationale in module docstring).
SYSTEM_PROMPT_PATH = DEPLOY_DIR / "openclaw-system-prompt.v1.md"
SYSTEM_PROMPT_NAME = "openclaw-system-prompt"

VALID_VIEWS = {"char", "schedule", "story_list", "story_text", "text", "home", "control"}
ERROR_FALLBACK = "网络出小差了，等一会儿再试试吧"
SOOTHING_TIMEOUT_MS = 4000
MAX_SPEECH_SENTENCES = 3

# Live probe config (env-overridable; defaults are the frozen decisions).
SSH_HELPER = os.environ.get("SSH_HELPER", "C:/tmp/ssh_helper.py")
ECS_HOST = os.environ.get("ECS_HOST", "8.137.91.16")
OPENCLAW_LOOPBACK = os.environ.get("OPENCLAW_LOOPBACK", "http://127.0.0.1:18789")
OPENCLAW_MODEL = os.environ.get("OPENCLAW_MODEL", "openclaw/default")
OPENCLAW_TOKEN = os.environ.get("OPENCLAW_TOKEN", "")
VRT_A4_SAMPLES = int(os.environ.get("VRT_A4_SAMPLES", "8"))

# 统计性遵循度复验的问题集（百科/闲聊混合, S-F4.1 口径）。
VRT_A4_QUESTIONS = [
    "天为什么是蓝色的？",
    "恐龙为什么灭绝了？",
    "月亮为什么会变圆变弯？",
    "彩虹是怎么来的？",
    "猫为什么会喵喵叫？",
    "人为什么要睡觉？",
    "给我讲个笑话吧",
    "小企鹅住在哪里？",
]

_SENTENCE_SPLIT = re.compile(r"[。！？!?]")


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _files_under(root: Path, suffix: str):
    if not root.is_dir():
        return []
    return sorted(p for p in root.rglob("*") if p.is_file() and p.suffix == suffix)


def _contains_any(text: str, tokens, ignore_case: bool = True) -> list:
    subject = text.lower() if ignore_case else text
    return [t for t in tokens if (t.lower() if ignore_case else t) in subject]


def _sentence_count(text: str) -> int:
    return len([s for s in _SENTENCE_SPLIT.split(text or "") if s.strip()])


def _extract_display_block(content: str):
    """Extract the ```display fenced block from a chat reply.

    Returns (block_found: bool, payload_or_None). Fence 路径（intake 裁定:
    遵循度/显示指令经 ```display fence, 不依赖 OpenClaw tools）。
    """
    m = re.search(r"```display\s*(\{.*?\})\s*```", content or "", re.DOTALL)
    if not m:
        return False, None
    try:
        return True, json.loads(m.group(1))
    except (ValueError, TypeError):
        return True, None


def _load_gateway_module(modname: str):
    """Import a server/gateway/<modname>.py by path (G frozen offline contract).

    Returns (module|None, error_string). A missing module must surface as a
    missing-behaviour RED (self.fail), NOT as an import crash of the file.
    """
    path = GATEWAY_DIR / f"{modname}.py"
    if not path.is_file():
        return None, f"server/gateway/{modname}.py missing (G frozen contract)"
    try:
        spec = importlib.util.spec_from_file_location(f"kidpal_vrt_{modname}", path)
        if spec is None or spec.loader is None:
            return None, f"server/gateway/{modname}.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"server/gateway/{modname}.py failed to import: {e!r}. "
                      "G frozen contract: gateway modules MUST be pure-python "
                      "(no fastapi/edge_tts/httpx at import time).")


class VoiceRoundtripBOfflineAcceptance(unittest.TestCase):
    """Offline-deterministic / structural acceptance — the red baseline's teeth."""

    maxDiff = None

    # --- VRT-A2 (cloud cooperation, twin alignment) --------------------------
    def test_a2_protocol_profile_twin_aligned(self):
        """coverage: VRT-A2 — §四 stt/tts 状态机协议形状的 C/Python 孪生对齐守卫.

        设备侧行为断言在 C 孪生（voice_roundtrip_b.acceptance.c）: tts
        start/sentence_start(带同步文字)/stop 状态机 + stt 识别文字上屏 +
        句级 SetChatMessage 同步。此处 offline 钉住孪生文件存在且仍承载 §四
        协议剖面标记（hybrid 验收两半不漂移）。
        """
        self.assertTrue(
            C_TWIN.is_file(),
            "VRT-A2 RED: C-side twin frozen acceptance file is missing — hybrid "
            "验收两半失衡 (firmware/main/test/voice_roundtrip_b.acceptance.c)",
        )
        src = _read_text(C_TWIN)
        for marker in ("sentence_start", '"stt"', "abort", "kidpal_display_route"):
            self.assertIn(
                marker, src,
                f"VRT-A2 RED: C twin no longer carries §四 profile marker {marker!r} — "
                "设备面协议剖面漂移",
            )

    # --- VRT-A3 (cloud cooperation) ------------------------------------------
    def test_a3_cloud_display_envelope(self):
        """coverage: VRT-A3 — kidpal/display JSON-RPC tools/call 信封 + 白名单对齐
        （对齐 server/gateway/mcp_relay.py / display_schema.py 已有 offline contract,
        绿色守卫防漂移; 设备侧消费 = C 侧 VRT-A3）."""
        mod, err = _load_gateway_module("mcp_relay")
        if mod is None:
            self.fail("VRT-A3 RED: " + err)
        call = mod.display_tool_call({"view": "text", "data": {"text": "hi"}})
        self.assertEqual(
            call.get("method"), "tools/call",
            "VRT-A3 RED: display relay is not a JSON-RPC tools/call (decision 23).")
        self.assertEqual(
            (call.get("params", {}) or {}).get("name"), "kidpal/display",
            "VRT-A3 RED: display tool name != kidpal/display — 设备消费端契约漂移.")
        schema, err = _load_gateway_module("display_schema")
        if schema is None:
            self.fail("VRT-A3 RED: " + err)
        self.assertEqual(
            set(schema.VALID_VIEWS), VALID_VIEWS,
            "VRT-A3 RED: display_schema.VALID_VIEWS drifted from the contract "
            "§1.3/§1.4 whitelist — 机械校验白名单与契约不一致.")
        ok, _ = schema.validate_display({"view": "text", "data": {"text": "你好"}})
        self.assertTrue(ok, "VRT-A3 RED: a well-formed text view was rejected — "
                            "消费端与机械校验面对不齐.")

    # --- VRT-A5 (cloud-device alignment) --------------------------------------
    def test_a5_cloud_fallback_alignment(self):
        """coverage: VRT-A5 — 网关兜底生成面形状（G 已 landed, 绿色守卫）+ 与设备
        text-view 消费面对齐: error_fallback_text_view() 下发的就是 C 侧 VRT-A3/A5
        消费面的 text view 载荷; 安抚句 = 原生 tts sentence_start 流水线即播
        （设备侧绿色守卫在 C 侧）."""
        mod, err = _load_gateway_module("fallback")
        if mod is None:
            self.fail("VRT-A5 RED: " + err)
        self.assertEqual(
            mod.ERROR_FALLBACK, ERROR_FALLBACK,
            "VRT-A5 RED: ERROR_FALLBACK drifted from the human-fixed friendly "
            "wording — 云端兜底措辞是 human 修正案, 不得漂移 (spec §4.0).")
        self.assertEqual(
            mod.SOOTHING_TIMEOUT_MS, SOOTHING_TIMEOUT_MS,
            "VRT-A5 RED: SOOTHING_TIMEOUT_MS != 4000 — 4s 安抚不冷场漂移 (spec §4.0).")
        self.assertTrue(
            isinstance(mod.SOOTHING_FALLBACK, str) and mod.SOOTHING_FALLBACK.strip(),
            "VRT-A5 RED: SOOTHING_FALLBACK empty — 安抚句必须非空.")
        view = mod.error_fallback_text_view()
        self.assertEqual(
            view,
            {"view": "text", "data": {"text": ERROR_FALLBACK}},
            "VRT-A5 RED: error fallback payload drifted from the text-view shape the "
            "device consumption face consumes — 云端生成面与设备消费面对齐破裂 "
            "(VRT-A3/A5 C 侧).")
        # V32 有处理失败模式的存在性（超 3 句 -> 截断, 非仅记录）
        trunc = mod.truncate_for_display("一。二。三。四。五。")
        self.assertLessEqual(
            _sentence_count(trunc), MAX_SPEECH_SENTENCES,
            "VRT-A5 RED: V32 truncation returned >3 sentences — 有处理的失败模式漂移.")

    # --- VRT-A4 (offline evidence record) --------------------------------------
    def test_a4_adherence_recheck_evidence_record(self):
        """coverage: VRT-A4 — 统计性遵循度复验记录落盘（不复用 HTTP 期 12/12 旧结论）.

        记录件须含: fence 路径声明（intake 裁定: 不依赖 tools）、样本计数、
        失败样本逐条处置（遵循度缺陷 = 失败模式非仅记录, S-F4.1 对齐）、
        「不复用 12/12」声明。
        """
        files = [p for p in _files_under(EVIDENCE_DIR, ".md")
                 if ("a4" in p.name.lower() or "adherence" in p.name.lower()
                     or "遵循" in p.name)]
        self.assertTrue(
            files,
            "VRT-A4 RED: no display-adherence recheck record under "
            "delivery/changes/dev/260829-voice-roundtrip/ — 方向 B 网关切分路径的格式遵循度"
            "统计性复验未执行（契约待确认点 1: 不复用 HTTP 期 12/12 旧结论）.",
        )
        text = _read_text(files[0])
        self.assertTrue(
            _contains_any(text, ["fence", "display 块", "```display"]),
            "VRT-A4 RED: adherence record does not state the fence path — intake "
            "裁定遵循度走 ```display fence 路径、不依赖 OpenClaw tools, 记录未声明.",
        )
        self.assertTrue(
            _contains_any(text, ["12/12"]),
            "VRT-A4 RED: adherence record does not reference the superseded 12/12 "
            "HTTP-era conclusion — 复验口径未声明「不复用旧结论」.",
        )
        self.assertTrue(
            _contains_any(text, ["处置"]),
            "VRT-A4 RED: adherence record has no failure-disposition entries — "
            "遵循度缺陷必须按失败模式处置（网关截断/分屏或 prompt 修正），非仅记录 "
            "(S-F4.1 / 契约待确认点 1).",
        )

    # --- VRT-A6 (cloud cancel evidence) ----------------------------------------
    def test_a6_abort_cloud_cancel_evidence(self):
        """coverage: VRT-A6 — 设备发 abort + 云端取消生成的观察记录（设备发送路径
        的原生绿色守卫在 C 侧 VRT-A6; 本地 AEC+VAD 即时停播主地位不变由 9a 真机
        实测承载, 不在此重复断言）."""
        files = [p for p in _files_under(EVIDENCE_DIR, ".md")
                 if "abort" in p.name.lower()
                 or ("roundtrip" in p.name.lower() or "evidence" in p.name.lower())]
        self.assertTrue(
            files,
            "VRT-A6 RED: no abort/cloud-cancel observation record under "
            "delivery/changes/dev/260829-voice-roundtrip/ — 喊停/打断 -> abort -> 云端取消"
            "生成链路未实测或未记录 (契约 §四 abort 消息).",
        )
        text = _read_text(files[0])
        self.assertTrue(
            "abort" in text.lower(),
            "VRT-A6 RED: abort evidence record does not mention the abort message.")
        self.assertTrue(
            _contains_any(text, ["取消", "cancel", "停止生成", "停"]),
            "VRT-A6 RED: abort evidence does not record the cloud cancel-generation "
            "observation — 只发 abort 不记录云端取消, 链路证据不合格.",
        )

    # --- VRT-A7 (offline: system prompt F4 discipline) -------------------------
    def test_a7_system_prompt_f4_discipline_offline(self):
        """coverage: VRT-A7 — F4/S-F4.1 答案纪律由 system prompt 钉死（offline 半）:
        口语化 + 3 句以内 + 儿童化（8-12 岁）+ E1 遵循（始终附 ```display 块）."""
        self.assertTrue(
            SYSTEM_PROMPT_PATH.is_file(),
            "VRT-A7 RED: the frozen system prompt file is missing "
            f"({SYSTEM_PROMPT_PATH}) — F4 答案纪律的生成端约束无处钉死 "
            "(spec §4.4 / 契约 §1.6).",
        )
        text = _read_text(SYSTEM_PROMPT_PATH)
        self.assertTrue(
            _contains_any(text, ["3 句以内", "三句以内"]),
            "VRT-A7 RED: system prompt does not pin the 3-sentence discipline — "
            "S-F4.1 三句纪律（生成端约束, 非显示端截断）缺失.")
        self.assertTrue(
            _contains_any(text, ["8-12岁", "8-12 岁", "8～12岁", "八到十二岁"]),
            "VRT-A7 RED: system prompt does not pin the 8-12 岁儿童化 audience — "
            "儿童化措辞纪律缺失.")
        self.assertTrue(
            _contains_any(text, ["口语化", "儿童化"]),
            "VRT-A7 RED: system prompt does not pin 口语化/儿童化 wording — F4 答案"
            "纪律不完整.")
        self.assertTrue(
            "```display" in text and _contains_any(text, ["始终", "每次回复"]),
            "VRT-A7 RED: system prompt does not mandate the ```display fenced block "
            "on every reply — E1 遵循约束缺失 (契约 §1.6).")

    # --- VRT-E4 (versioned system prompt landed + wired) ------------------------
    def test_e4_system_prompt_versioned_landed(self):
        """coverage: VRT-E4 — system prompt 版本化落盘于冻结路径
        server/deploy/openclaw-system-prompt.v1.md（路径 rationale 见模块 docstring）:
        两部分结构 + 7 view 白名单 + v1 版本标记 + 供包 4/包 7 增量扩展的扩展区."""
        self.assertTrue(
            SYSTEM_PROMPT_PATH.is_file(),
            "VRT-E4 RED: no versioned system prompt at the frozen path "
            f"{SYSTEM_PROMPT_PATH} — OpenClaw 基础 system prompt 未落盘 (VRT-E4).",
        )
        text = _read_text(SYSTEM_PROMPT_PATH)
        self.assertTrue(
            "```display" in text,
            "VRT-E4 RED: prompt lacks the ```display fenced-block instruction — "
            "E1 显示指令通道未约定 (契约 §1.1).")
        missing_views = [v for v in VALID_VIEWS if v not in text]
        self.assertEqual(
            missing_views, [],
            "VRT-E4 RED: prompt view whitelist incomplete, missing: "
            f"{missing_views} — 契约 §1.3/§1.4 白名单未全量约定.")
        self.assertIn(
            "v1", text,
            "VRT-E4 RED: prompt carries no v1 version marker — 版本化要求缺失.")
        self.assertTrue(
            _contains_any(text, ["增量扩展", "extension"]),
            "VRT-E4 RED: prompt has no incremental-extension section — 包 4（char）/"
            "包 7（提醒备注解析）无法在不动基础结构的情况下扩展 (VRT-E4).")
        self.assertTrue(
            _contains_any(text, ["包 4", "包4", "包 7", "包7", "char", "提醒"]),
            "VRT-E4 RED: extension section does not name its consumers (包 4/包 7) — "
            "扩展契约不明.")

    def test_e4_system_prompt_wired_into_deploy_overlay(self):
        """coverage: VRT-E4 — 落盘 ≠ 孤儿文件: deploy overlay / 网关声明面必须引用
        system prompt 文件（随 overlay 上 ECS 或经网关注入请求）, 否则 E4 只是摆设."""
        corpus = "\n".join(
            _read_text(p) for p in _files_under(DEPLOY_DIR, "")) + "\n" + "\n".join(
            _read_text(p) for p in _files_under(GATEWAY_DIR, ".py"))
        self.assertIn(
            SYSTEM_PROMPT_NAME, corpus,
            "VRT-E4 RED: no deploy/gateway carrier references "
            f"{SYSTEM_PROMPT_NAME} — system prompt 落盘但未接入任何部署/注入面, "
            "孤儿文件不算落盘 (VRT-E4 + A4/A7 live 所测必须 = 生产所用).")


class VoiceRoundtripBLiveAcceptance(unittest.TestCase):
    """Frozen live assertions; @pytest.mark.live — NOT auto-run by merge gates.

    Live faces: VRT-A4 statistical display-adherence recheck against the REAL
    OpenClaw (fence path, system prompt = the frozen versioned file) and
    VRT-A7 F4 general-QA chain. 统计性口径: 遵循度缺陷 = 失败样本逐条处置记录,
    不 gate；探针若被环境卡死（OpenClaw 不可达等）按 launch 纪律 fail-closed
    升级, 不降级为「记录即过」。
    """

    maxDiff = None
    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _ssh(self, cmd: str, timeout: int = 300):
        proc = subprocess.run(
            [sys.executable, SSH_HELPER, cmd],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=timeout,
        )
        return proc.stdout, proc.stderr, proc.returncode

    def _ssh_ok(self, cmd: str, what: str, timeout: int = 300):
        out, err, rc = self._ssh(cmd, timeout=timeout)
        if rc != 0:
            raise AssertionError(f"{what}: remote rc={rc}. stdout={out!r} stderr={err!r}")
        return out

    def _chat_once(self, prompt: str, question: str) -> dict:
        """One NON-streaming chat completion to the OpenClaw loopback via SSH.

        FROZEN fence-path rule (intake 裁定): the request carries model/messages/
        stream ONLY — never a `tools` key (display 指令经 ```display fence, 引擎
        意图分类的 tools 行为与本验收无关).
        """
        body = {
            "model": OPENCLAW_MODEL,
            "messages": [
                {"role": "system", "content": prompt},
                {"role": "user", "content": question},
            ],
            "stream": False,
        }
        self.assertNotIn(
            "tools", body,
            "VRT-A4/A7 live RED(harness): request carries a tools key — fence 路径"
            "纪律被违反 (intake 裁定: 遵循度/显示指令不依赖 OpenClaw tools).")
        b64 = base64.b64encode(json.dumps(body, ensure_ascii=False).encode("utf-8")).decode()
        auth = f"-H 'Authorization: Bearer {OPENCLAW_TOKEN}' " if OPENCLAW_TOKEN else ""
        cmd = (
            f"printf '%s' {b64} | base64 -d > /tmp/vrt_chat_body.json && "
            f"curl -s -m 120 {auth}-H 'Content-Type: application/json' "
            f"-X POST --data @/tmp/vrt_chat_body.json {OPENCLAW_LOOPBACK}/v1/chat/completions"
        )
        out = self._ssh_ok(cmd, "live OpenClaw chat completion probe", timeout=180)
        try:
            return json.loads(out.strip().splitlines()[-1])
        except (ValueError, IndexError):
            raise AssertionError(
                f"live RED: OpenClaw reply is not JSON (fence-path probe broken). raw={out[:400]!r}")

    @staticmethod
    def _reply_content(resp: dict) -> str:
        try:
            return resp["choices"][0]["message"]["content"] or ""
        except (KeyError, IndexError, TypeError):
            return ""

    # precondition --------------------------------------------------------------
    def test_live_ssh_reachable(self):
        """coverage: live precondition — SSH connection to the ECS box works."""
        out, err, rc = self._ssh("echo reachable-$(hostname)")
        self.assertIn(
            "reachable", out,
            f"live precondition RED: SSH to {ECS_HOST} failed (rc={rc}). out={out!r} err={err!r}")

    # VRT-A4 (live, statistical) --------------------------------------------------
    def test_a4_live_display_adherence_recheck(self):
        """coverage: VRT-A4 — 对真 OpenClaw 统计 E1 格式遵循率（fence 路径, system
        prompt = 冻结版本化文件; 失败样本逐条处置落盘; 不复用 12/12 旧结论）.

        判据（统计性, 不设遵循率阈值——契约待确认点 1 = 需实测记录）:
          - 每个样本一条记录（问题 + 是否含合法 display 块 + view）;
          - 每个失败样本必须紧跟一条「处置:」行（失败模式, 非仅记录）;
          - 样本文件声明不复用 12/12。
        """
        prompt = _read_text(SYSTEM_PROMPT_PATH)
        self.assertTrue(
            prompt.strip(),
            f"VRT-A4 live RED: frozen system prompt missing/empty ({SYSTEM_PROMPT_PATH}).")
        # push the prompt so 所测 = 生产所用 (single file, small)
        pb64 = base64.b64encode(prompt.encode("utf-8")).decode()
        self._ssh_ok(
            f"printf '%s' {pb64} | base64 -d > /tmp/kidpal_system_prompt.v1.md",
            "push frozen system prompt to ECS", timeout=60)

        rows = []
        failures = 0
        for i, q in enumerate(VRT_A4_QUESTIONS[:VRT_A4_SAMPLES], start=1):
            resp = self._chat_once(prompt, q)
            content = self._reply_content(resp)
            found, payload = _extract_display_block(content)
            view = (payload or {}).get("view")
            ok = bool(found and isinstance(payload, dict) and view in VALID_VIEWS)
            if not ok:
                failures += 1
            rows.append((i, q, ok, view, content))

        lines = [
            "# VRT-A4 display 格式遵循度复验样本（方向 B fence 路径, 不复用 HTTP 期 12/12 旧结论）",
            f"- 样本数: {len(rows)}；失败样本: {failures}；遵循率: "
            f"{(len(rows) - failures)}/{len(rows)}",
            "- 路径声明: ```display fence 切分（网关正则 + display_schema 机械校验），"
            "不依赖 OpenClaw tools（intake 裁定）。",
            "- system prompt 源: server/deploy/openclaw-system-prompt.v1.md（冻结版本化文件）。",
            "",
        ]
        for i, q, ok, view, content in rows:
            lines.append(f"## sample {i}: {q}")
            lines.append(f"- 结果: {'PASS' if ok else 'FAIL'}；display view: {view!r}")
            lines.append(f"- 回复节选: {(content or '')[:160]!r}")
            if not ok:
                lines.append(
                    "- 处置: 遵循度缺陷登记为失败模式（候选处置 = 网关 V32 截断/分屏 "
                    "或 system prompt 修正），包内 resolve、不 gate（契约待确认点 1）。")
            lines.append("")
        EVIDENCE_DIR.mkdir(parents=True, exist_ok=True)
        out_path = EVIDENCE_DIR / "a4-adherence-samples.md"
        out_path.write_text("\n".join(lines), encoding="utf-8")

        self.assertEqual(
            len(rows), min(VRT_A4_SAMPLES, len(VRT_A4_QUESTIONS)),
            "VRT-A4 live RED: sample count mismatch — 统计口径未跑满.")
        written = _read_text(out_path)
        self.assertEqual(
            written.count("- 结果:"), len(rows),
            "VRT-A4 live RED: sample records not fully written.")
        if failures:
            self.assertGreaterEqual(
                written.count("处置:"), failures,
                "VRT-A4 live RED: failure samples lack 处置 entries — 遵循度缺陷必须"
                "按失败模式处置, 非仅记录 (S-F4.1).")

    # VRT-A7 (live) ----------------------------------------------------------------
    def test_a7_live_f4_general_qa_chain(self):
        """coverage: VRT-A7 — F4/S-F4.1 通用问答 live 链路: 对话态问百科/闲聊 ->
        口语化答案（非空）+ ```display text 视图全文上屏 + 三句以内纪律（超限 =
        system prompt 遵循度缺陷, 指向 V32 有处理失败模式）."""
        prompt = _read_text(SYSTEM_PROMPT_PATH)
        self.assertTrue(
            prompt.strip(),
            f"VRT-A7 live RED: frozen system prompt missing ({SYSTEM_PROMPT_PATH}).")
        resp = self._chat_once(prompt, "天为什么是蓝色的？")
        content = self._reply_content(resp)
        self.assertTrue(
            content.strip(),
            "VRT-A7 live RED: empty reply from OpenClaw — 通用问答链路未跑通.")
        found, payload = _extract_display_block(content)
        self.assertTrue(
            found,
            "VRT-A7 live RED: reply carries no ```display fenced block — E1 遵循失败 "
            "(通用问答的显示载体 = display text view, S-F4.1).")
        view = (payload or {}).get("view")
        self.assertIn(
            view, VALID_VIEWS,
            f"VRT-A7 live RED: display view {view!r} outside the contract whitelist "
            "— 机械校验应在网关拦截 (契约 §1.5).")
        speech = content.split("```")[0].strip()
        self.assertTrue(
            speech,
            "VRT-A7 live RED: no speech part before the display block — 「先口语回答」"
            "两部分结构缺失 (§1.6).")
        n = _sentence_count(speech)
        self.assertLessEqual(
            n, MAX_SPEECH_SENTENCES,
            f"VRT-A7 live RED: speech has {n} sentences > 3 — 三句纪律遵循度缺陷 "
            "(S-F4.1 生成端约束失效; 失败模式处置 = 网关 V32 截断/分屏 + prompt 修正, "
            "登记入 a4-adherence-samples.md 非仅记录).")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(VoiceRoundtripBOfflineAcceptance)
    if os.environ.get("VRT_RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(VoiceRoundtripBLiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
