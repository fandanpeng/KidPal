"""Frozen acceptance tests — 261117-session-end-intent-longretry（SEI-B1..B4 host 面）。

TaskPackage 261117-session-end-intent-longretry（P2，workstream
261115-session-channel-resident）；acceptance-author 冻结，builder/fixer 只读。

================================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期零网络 I/O）

  SEI-B1 · 门禁分叉（display_schema）：end_session 属「截获转发类」网关决定类——
    relay 缺省路径恒拒（must-stay-green，与 test_points_e2e_spike K0PTS-A3 同判据）;
    front 自推通路 self_push=True 放行（与 start_organizing 同型）;
    add_points / redeem_points 在 self_push 下仍恒拒（正锚，防豁免口误扩）;
    restart_organizing / stop_speaking / start_recording 既有语义零偏移。
    回引：契约 §1.4 / §1.5。

  SEI-B2 · 截获-自推（front_app）：引擎 display 流（side-channel 与 fence 两 relay 路径
    收敛于 _relay_display_blocks_to_device）投一块 control/end_session ->
    设备腿恰收一帧自推 control/end_session（_push_display_payload 单源，过 display_schema
    self_push=True）+ 零帧友好降级 text + 不再走降级/透传。回引：契约 §1.4 截获转发类。

  SEI-B3 · 回环/重复防护：每个被截获薄意图恰一帧（两条 relay 入口路径各自恰 1、
    不叠加「降级 text + 自推」两帧）；同一批两块同型薄意图 -> 恰 2 帧
    （契约语义 = 每个被截获薄意图一帧，不加每回合去重）。回引：契约 §1.4、PROPOSAL T5。

  SEI-B4 · prompt 承载三面相等（ADR-001 / arch §5.5.5）：workspace-kidpal AGENTS.md
    回合路由表含告别回合纪律行（拜拜 / 不用了 -> control/end_session）+ home 行语义
    已收窄（不再承载告别）; carrier 静态承载声明指向 AGENTS.md; provider.config.yaml
    的 static_prompt 三键指向 AGENTS.md; 契约 §1.6 告别回合纪律段在场。
    回引：契约 §1.6、arch §5.5.5。

================================================================================
强度申报（契约条目 / 决策 -> 承载断言 -> 性质档）:

  · 契约 §1.4 end_session 截获转发类 / §1.5-3 控制类分流
        -> SEI-B1（真门真跑）+ SEI-B2（两 relay 入口真跑）+ SEI-B3（帧数不变量）
        -> **行为级**（不是文字比对，是 drive 真实 pump + 真实 validate_display 的无副作用
           I/O double）。端到端「说拜拜」实际收会话 = 真机轮 B B1。
  · 契约 §1.4 伪造/注入防护（relay 恒拒）
        -> SEI-B1 relay 缺省 assertFalse（must-stay-green，防反向破口）
        -> 行为级。引擎流注入伪造 = 真机轮 B B3。
  · 契约 §1.6 告别回合纪律 + arch §5.5.5 静态单源 + ADR-001 三面相等
        -> SEI-B4 **源锚代理**（prompt 措辞本身即交付物；「prompt 写明 ≠ LLM 遵循」行为
           等价假设显式化——LLM 遵循度 offline 不可机判，归真机轮 B B1/B2 目击）。

  弱于契约处（显式缺口，逐条）:
    GAP-B1 SEI-B1/B2/B3 证明 front 侧 wire 分发正确；但「设备真收到后 EndVoiceSession
      事件位收口、麦克风关、页回落、通道常驻」= 真机轮 B B1（front 侧无驱动方可量化）。
    GAP-B2 SEI-B4 只证 prompt 制品措辞在场；「孩子真说拜拜 -> 模型真产 control/end_session
      薄意图」的生成行为 = 真机轮 B B1/B2。
    GAP-B3 「告别语不误伤」（「拜拜，明天再聊」只结束会话不误伤）的语义 = LLM 判据 +
      统一处理器，offline 不可机判，归真机轮 B B2。

  fixture 自含性声明:
    - 纯 python / 零第三方 / 零网络；front_app 离线装载（fastapi 缺席走本地 stub，
      与 M4K1 / RDG 冻结件同款）。
    - I/O double 只替代 WS 两端（_FakeUpstream / _FakeDeviceSocket），不 mock front 内部
      逻辑——validate_display / _relay_display_blocks_to_device / pump 全部真跑。
    - 行为 seam 驱动方缺口见上（设备侧与 LLM 遵循归真机轮）。
    - 依赖被测文件在场非空 / AGENTS.md 非空 = infra 红（不静默跳过）。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_session_end_intent.acceptance.py
================================================================================
"""

from __future__ import annotations

import asyncio
import importlib.util
import json
import sys
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
DEPLOY_DIR = SERVER / "deploy"
AGENTS_MD = DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
CARRIER = DEPLOY_DIR / "OPENCLAW_SYSTEM_PROMPT_CARRIER"
PROVIDER_CONFIG = DEPLOY_DIR / "provider.config.yaml"
CONTRACT_MD = REPO_ROOT / "docs" / "contracts" / "interface-contract.md"

END_SESSION_ACTION = "end_session"
START_ORGANIZING_ACTION = "start_organizing"
POINTS_ACTIONS = ("add_points", "redeem_points")
FRIENDLY_DOWNGRADE_MARKER = "没显示好"


# --------------------------------------------------------------------------- #
# Module loading（缺失/不可装载 = 设计态红，不静默跳过）                       #
# --------------------------------------------------------------------------- #

_front_module_cache = None


def _ensure_fastapi_stub() -> None:
    try:
        import fastapi  # noqa: F401

        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    class _StubFastAPI:
        def __init__(self, *args, **kwargs):  # noqa: D107
            pass

        def _identity(self, *args, **kwargs):
            def deco(fn):
                return fn

            return deco

        get = post = websocket = _identity

    stub = _PermissiveModule("fastapi")
    stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
    stub.HTTPException = type("HTTPException", (Exception,), {})
    stub.FastAPI = _StubFastAPI
    sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")


def _load_front_app():
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location(
            "kidpal_front_app_sei", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线"
                      "可装载（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("SEI RED: " + err)
    return mod


def _display_schema():
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415

    return ds


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


# --------------------------------------------------------------------------- #
# I/O doubles（M4K1 / RDG / points-e2e 冻结 seam 同款形态：只替代 WebSocket 两端）  #
# --------------------------------------------------------------------------- #


class _FakeUpstream:
    """Engine ws leg double: async-iterable of preloaded frames, records .send()."""

    def __init__(self, frames=()):
        self._frames = list(frames)
        self._pos = 0
        self.sent = []

    def __aiter__(self):
        return self

    async def __anext__(self):
        if self._pos >= len(self._frames):
            raise StopAsyncIteration
        frame = self._frames[self._pos]
        self._pos += 1
        return frame

    async def send(self, message):
        self.sent.append(message)


class _FakeDeviceSocket:
    """Device ws leg double: starlette-style .receive() dicts + send_text/send_bytes."""

    def __init__(self, incoming=(), disconnect_exc=None):
        self._incoming = list(incoming)
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.sent_text = []
        self.sent_bytes = []

    async def receive(self):
        if self._pos >= len(self._incoming):
            raise self._disconnect_exc()
        msg = self._incoming[self._pos]
        self._pos += 1
        return msg

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(data)


def _engine_to_device(front, frames, session_id=None):
    """Run the engine->device downlink pump; returns (upstream, device_socket)."""
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, session_id))
    return up, dev


def _mcp_display_envelopes(sent_text):
    out = []
    for text in sent_text:
        try:
            frame = json.loads(text)
        except (TypeError, ValueError):
            continue
        if (isinstance(frame, dict) and frame.get("type") == "mcp"
                and isinstance(frame.get("payload"), dict)):
            payload = frame["payload"]
            params = payload.get("params") or {}
            if payload.get("method") == "tools/call" and params.get("name") == "kidpal/display":
                out.append(payload)
    return out


def _display_payloads(sent_text):
    """Reconstruct [{"view","data"}] from kidpal/display envelopes on the device leg."""
    out = []
    for env in _mcp_display_envelopes(sent_text):
        args = (env.get("params") or {}).get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _control_end_session_frames(payloads):
    return [
        p for p in payloads
        if p.get("view") == "control"
        and isinstance(p.get("data"), dict)
        and p["data"].get("action") == END_SESSION_ACTION
    ]


def _friendly_downgrade_frames(payloads):
    return [
        p for p in payloads
        if p.get("view") == "text"
        and isinstance(p.get("data"), dict)
        and FRIENDLY_DOWNGRADE_MARKER in (p["data"].get("text") or "")
    ]


def _side_channel(block_strs):
    return {"type": "kidpal_display_blocks", "blocks": list(block_strs)}


def _fence_frame(block_strs):
    body = "\n".join(["好的。", "```display", *block_strs, "```"])
    return {"type": "tts", "state": "sentence_end", "text": body}


def _end_session_block():
    return json.dumps({"view": "control", "data": {"action": END_SESSION_ACTION}},
                      ensure_ascii=False)


# --------------------------------------------------------------------------- #
# SD-SCHEMA / 承载面读出（root 注入纪律：路径由 __file__ 推导，不依赖 cwd）     #
# --------------------------------------------------------------------------- #


def _md_section(src: str, heading_needle: str, level: str = "## ") -> str:
    """取标题含 heading_needle 的节窗口：自该标题行起、至同/更高层级下一标题行或 EOF。"""
    lines = src.splitlines()
    start = None
    for i, line in enumerate(lines):
        if line.startswith(level) and heading_needle in line:
            start = i
            break
    if start is None:
        return ""
    rest = lines[start + 1:]
    end = len(rest)
    for j, line in enumerate(rest):
        if line.startswith(level):
            end = j
            break
    return "\n".join(rest[:end])


# =========================================================================== #
# SEI-B1 — 门禁分叉（self_push 放行 end_session；relay 恒拒；points 双路径恒拒）  #
# =========================================================================== #


class SeiDisplaySchemaForkAcceptance(unittest.TestCase):
    """SEI-B1: end_session 截获转发类门禁分叉。"""

    maxDiff = None

    # coverage: SEI-B1 -- GATEWAY_DECISION_CONTROL_ACTIONS 仍含 end_session（不得从集合摘出，K0PTS-A3 四值逐个 assertIn）+ validate_display(end_session, self_push=True) 放行 + validate_display(end_session) relay 缺省恒拒（must-stay-green）+ add_points/redeem_points 在 self_push 下仍恒拒（防豁免口误扩）+ start_organizing self_push 仍放行 / stop_speaking / start_recording 双路径可透传（零偏移）。回引契约 §1.4/§1.5
    def test_sei_b1_self_push_admits_end_session_relay_still_rejects(self):
        ds = _display_schema()
        actions = getattr(ds, "GATEWAY_DECISION_CONTROL_ACTIONS", None)
        self.assertIsInstance(
            actions, (set, frozenset),
            "SEI-B1 RED(infra): GATEWAY_DECISION_CONTROL_ACTIONS 缺失。")
        self.assertIn(
            END_SESSION_ACTION, actions,
            "SEI-B1 RED: GATEWAY_DECISION_CONTROL_ACTIONS 不含 end_session —— 决定类"
            "被摘出集合（K0PTS-A3 四值 assertIn 破；只许扩豁免口、不许摘集合）。")

        payload = {"view": "control", "data": {"action": END_SESSION_ACTION}}

        # self_push 放行（红→绿核心）。
        try:
            ok_push, reason_push = ds.validate_display(payload, self_push=True)
        except TypeError as e:
            self.fail("SEI-B1 RED: validate_display 无 keyword-only self_push 形参 —— "
                      f"自推通路分叉未按冻结 seam 落地（{e}）。")
        self.assertTrue(
            ok_push,
            f"SEI-B1 RED: 自推通路仍拒 end_session ({reason_push}) —— front 机械下发"
            "被自拒（截获转发类终端帧下发不出去）。")

        # relay 缺省恒拒（must-stay-green：防伪造/注入）。
        ok_relay, _ = ds.validate_display(payload)
        self.assertFalse(
            ok_relay,
            "SEI-B1 RED: relay 缺省路径放行 end_session —— 伪造/注入破口（契约 §1.4："
            "relay 直转对网关决定类恒拒；must-stay-green 不得被顺手翻）。")

        # 正锚：add_points / redeem_points 在 self_push 下仍恒拒（豁免口不许误扩）。
        for action in POINTS_ACTIONS:
            ok_pts, reason_pts = ds.validate_display(
                {"view": "control", "data": {"action": action}}, self_push=True)
            self.assertFalse(
                ok_pts,
                f"SEI-B1 RED: self_push 放行了 {action} ({reason_pts}) —— 豁免口误扩"
                "（积分控制帧 relay 与 self_push 双路径恒拒；契约 §1.4 截获执行类）。")

        # 零偏移：start_organizing self_push 仍放行；stop_speaking/start_recording 双路径可透传。
        ok_org, reason_org = ds.validate_display(
            {"view": "control", "data": {"action": START_ORGANIZING_ACTION}}, self_push=True)
        self.assertTrue(
            ok_org,
            f"SEI-B1 RED: start_organizing self_push 被误拒 ({reason_org}) —— 既有豁免口"
            "被 end_session 扩口带破（must-stay-green）。")
        for action in ("stop_speaking", "start_recording"):
            ok_push_fwd, r1 = ds.validate_display(
                {"view": "control", "data": {"action": action}}, self_push=True)
            ok_relay_fwd, r2 = ds.validate_display(
                {"view": "control", "data": {"action": action}})
            self.assertTrue(ok_push_fwd, f"SEI-B1 RED: {action} self_push 被误拒 ({r1})。")
            self.assertTrue(ok_relay_fwd, f"SEI-B1 RED: {action} relay 被误拒 ({r2})。")


# =========================================================================== #
# SEI-B2 — 截获-自推（引擎流投 end_session -> 设备恰一帧自推 + 零降级）          #
# =========================================================================== #


class SeiRelayInterceptAcceptance(unittest.TestCase):
    """SEI-B2: _relay_display_blocks_to_device 截获 end_session 并自推恰一帧。"""

    maxDiff = None

    # coverage: SEI-B2 -- 引擎 side-channel display 流投一块 control/end_session -> 设备腿恰收一帧自推 control/end_session（_push_display_payload 单源、过 display_schema self_push=True、arguments.data 为 JSON 字符串）+ 零帧友好降级 text（不降级不透传）+ 原始块不被 verbatim 转发。回引契约 §1.4 截获转发类 + PROPOSAL T5
    def test_sei_b2_relay_intercepts_end_session_and_self_pushes_one_frame(self):
        front = _front()
        block = _end_session_block()
        frames = [_side_channel([block])]

        up, dev = _engine_to_device(front, frames, session_id=None)

        payloads = _display_payloads(dev.sent_text)
        control = _control_end_session_frames(payloads)
        self.assertEqual(
            len(control), 1,
            f"SEI-B2 RED: 引擎流投 end_session 后设备腿 control/end_session 帧数 = "
            f"{len(control)}（应恰一帧自推）—— 截获-自推单源未落（0 = 逐块 relay 降级；"
            ">1 = 双发）")

        downgrades = _friendly_downgrade_frames(payloads)
        self.assertEqual(
            downgrades, [],
            "SEI-B2 RED: 设备腿收到友好降级 text 帧 —— end_session 被 build_display_"
            "tool_calls 当作 schema 拒收块降级（截获落点须在 _relay_display_blocks_to_device"
            " 的 build_display_tool_calls 之前、consumed 即 continue）。")

        # 自推帧是设备 wire 形状：arguments.data 为 JSON 字符串（mcp_server String 属性）。
        envs = [
            env for env in _mcp_display_envelopes(dev.sent_text)
            if ((env.get("params") or {}).get("arguments") or {}).get("view") == "control"
        ]
        self.assertEqual(len(envs), 1, "SEI-B2 RED: 设备腿 control 信封数 != 1。")
        args = (envs[0].get("params") or {}).get("arguments") or {}
        self.assertIsInstance(
            args.get("data"), str,
            "SEI-B2 RED: 自推帧 arguments.data 非 JSON 字符串 —— 设备 wire 形状错"
            "（mcp_server Property 是 String 类型，object 型 data 被拒）。")
        self.assertEqual(
            json.loads(args["data"]), {"action": END_SESSION_ACTION},
            "SEI-B2 RED: 自推帧 data 内容 != {action:end_session}。")

        # 引擎流未被 verbatim 转发（side-channel 永不直转；且原始 display 块不出现为独立帧）。
        self.assertFalse(
            any(block in t for t in up.sent),
            "SEI-B2 RED: 引擎腿收到 display 块回写 —— 截获缝误向引擎回灌。")

    # coverage: SEI-B2 -- fence 文本路径同判据（两 relay 入口收敛于 _relay_display_blocks_to_device）：tts 文本携带 ```display 块投 control/end_session -> 设备腿恰一帧自推 + 零降级
    def test_sei_b2_fence_path_also_intercepts_end_session(self):
        front = _front()
        block = _end_session_block()
        frames = [_fence_frame([block])]

        _up, dev = _engine_to_device(front, frames, session_id=None)

        payloads = _display_payloads(dev.sent_text)
        control = _control_end_session_frames(payloads)
        self.assertEqual(
            len(control), 1,
            f"SEI-B2 RED: fence 路径 end_session 设备帧数 = {len(control)}（应恰一帧）—— "
            "两 relay 入口未同口径截获（_relay_display_blocks_to_device 唯一收敛点）。")
        self.assertEqual(
            _friendly_downgrade_frames(payloads), [],
            "SEI-B2 RED: fence 路径 end_session 被降级（截获落点错误）。")


# =========================================================================== #
# SEI-B3 — 回环/重复防护（每意图恰一帧；不叠加降级；不加每回合去重）             #
# =========================================================================== #


class SeiFrameCountInvariantAcceptance(unittest.TestCase):
    """SEI-B3: 帧数不变量（恰一帧自推，双入口独立，无二次截获）。"""

    maxDiff = None

    # coverage: SEI-B3 -- 两条 relay 入口（side-channel / fence）各自单块 end_session：设备腿 kidpal/display 显示帧总数恰 1（不叠加「降级 text + 自推」两帧 = 无二次截获 / 无回环双发）。回引契约 §1.4 截获转发类 + PROPOSAL T5
    def test_sei_b3_one_frame_per_intent_both_relay_paths(self):
        front = _front()
        block = _end_session_block()
        for label, frames in (
            ("side_channel", [_side_channel([block])]),
            ("fence", [_fence_frame([block])]),
        ):
            _up, dev = _engine_to_device(front, frames, session_id=None)
            payloads = _display_payloads(dev.sent_text)
            self.assertEqual(
                len(payloads), 1,
                f"SEI-B3 RED[{label}]: 单块 end_session 的设备显示帧总数 = "
                f"{len(payloads)}（应恰 1）—— 判据 = 自推那一帧，任何额外帧（降级 text / "
                "二次截获 / 回环）都会让计数 > 1。")
            self.assertEqual(
                payloads[0].get("view"), "control",
                f"SEI-B3 RED[{label}]: 唯一帧不是 control view（= {payloads[0].get('view')}）"
                "—— 帧数虽 1 但内容是降级 text。")

    # coverage: SEI-B3 -- 同一 side-channel 批两块同型薄意图 -> 设备腿恰 2 帧（每个被截获薄意图一帧、不加每回合去重；契约语义）。回引契约 §1.4 + PROPOSAL T5
    def test_sei_b3_no_per_turn_dedupe_two_intents_two_frames(self):
        front = _front()
        block = _end_session_block()
        frames = [_side_channel([block, block])]

        _up, dev = _engine_to_device(front, frames, session_id=None)
        payloads = _display_payloads(dev.sent_text)
        control = _control_end_session_frames(payloads)
        self.assertEqual(
            len(control), 2,
            f"SEI-B3 RED: 同一批两块同型 end_session 的设备帧数 = {len(control)}（应恰 2）"
            "—— 契约语义 = 每个被截获薄意图恰一帧，不加每回合去重（1 = 误加去重；"
            ">2 = 双发）。")


# =========================================================================== #
# SEI-B4 — prompt 承载三面相等（AGENTS.md 纪律 + home 收窄 + carrier/provider）    #
# =========================================================================== #


class SeiPromptCarrierAcceptance(unittest.TestCase):
    """SEI-B4: 告别回合纪律落 AGENTS.md，home 行收窄，三面相等不变式保绿。"""

    maxDiff = None

    def _agents(self):
        src = _read_text(AGENTS_MD)
        self.assertTrue(
            src.strip(),
            f"SEI-B4 RED: {AGENTS_MD} 不在场/为空 —— 静态语义单一事实源镜像缺失。")
        return src

    # coverage: SEI-B4 -- AGENTS.md 回合路由表含告别回合行（拜拜/不用了 + control/end_session）+ home 行语义收窄（不再承载再见/结束/告别，收窄为「无需显示内容的普通答复」）。回引契约 §1.6 + arch §5.5.5
    def test_sei_b4_agents_md_farewell_row_and_home_narrowed(self):
        src = self._agents()
        lines = src.splitlines()

        # ① 告别回合纪律行：含「告别」且同时含同族告别词 + control + end_session。
        farewell_rows = [l for l in lines if "告别" in l]
        self.assertTrue(
            farewell_rows,
            "SEI-B4 RED: AGENTS.md 回合路由表无含「告别」的行 —— 告别回合纪律未落"
            "（契约 §1.6：孩子明确告别 -> 口语一句 + control/end_session）。")
        row = "\n".join(farewell_rows)
        self.assertTrue(
            any(w in row for w in ("拜拜", "不用了", "先到这儿", "不聊了")),
            "SEI-B4 RED: 告别行缺同族告别词（拜拜/不用了/先到这儿/不聊了）—— 识别特征"
            "缺失。")
        self.assertIn("control", row, "SEI-B4 RED: 告别行未指向 control view。")
        self.assertIn(
            END_SESSION_ACTION, row,
            "SEI-B4 RED: 告别行未指向 control/end_session —— 告别仍被指向 home 或其它"
            "（原缺陷：告别被指向 home view，云端结束意图通道断开）。")

        # ② home 行语义收窄：home 行不再承载告别。
        home_rows = [l for l in lines if l.lstrip().startswith("| home")]
        self.assertTrue(
            home_rows,
            "SEI-B4 RED: AGENTS.md 无 `| home` display 白名单行 —— display 表形态漂移"
            "须复核。")
        home_row = "\n".join(home_rows)
        for banned in ("再见", "结束", "告别"):
            self.assertNotIn(
                banned, home_row,
                f"SEI-B4 RED: home 行仍含 {banned!r} —— home 未收窄为「无需显示内容的"
                "普通答复」（契约 §1.6：不得用 home view 承载告别）。")
        self.assertTrue(
            any(w in home_row for w in ("无需显示", "无需", "空")),
            "SEI-B4 RED: home 行未表达「无需显示内容」的收窄语义。")

    # coverage: SEI-B4 -- ADR-001 三面相等：(静态面) AGENTS.md 在场非空 == (声明面) OPENCLAW_SYSTEM_PROMPT_CARRIER 声明 AGENTS.md 为静态语义主承载 == (声明面) provider.config.yaml static_prompt 三键指向 workspace-kidpal/AGENTS.md；另契约 §1.6 告别回合纪律段在场（源锚 canary）。回引契约 §1.6 + arch §5.5.5 / ADR-001
    def test_sei_b4_three_face_static_carrier_invariant(self):
        agents_src = self._agents()

        # 声明面 1：carrier 声明 AGENTS.md 为静态语义主承载。
        carrier = _read_text(CARRIER)
        self.assertTrue(
            carrier.strip(), f"SEI-B4 RED: {CARRIER} 不在场/为空。")
        self.assertIn(
            "AGENTS.md", carrier,
            "SEI-B4 RED: carrier 未声明 AGENTS.md 静态承载 —— 声明面缺位（ADR-001）。")
        self.assertTrue(
            any(w in carrier for w in ("静态语义", "主承载", "静态承载")),
            "SEI-B4 RED: carrier 未声明 AGENTS.md 为静态语义主承载。")

        # 声明面 2：provider.config.yaml static_prompt 三键指向 workspace-kidpal/AGENTS.md。
        provider = _read_text(PROVIDER_CONFIG)
        self.assertTrue(
            provider.strip(), f"SEI-B4 RED: {PROVIDER_CONFIG} 不在场/为空。")
        # YAML 不是 markdown —— 用行块切取：static_prompt: 起到下一个 top-level key。
        lines = provider.splitlines()
        start = next((i for i, l in enumerate(lines) if l.strip() == "static_prompt:"), None)
        self.assertIsNotNone(
            start, "SEI-B4 RED: provider.config.yaml 无 static_prompt: 块 —— 声明面缺位。")
        block_lines = []
        for line in lines[start + 1:]:
            if line and not line.startswith((" ", "\t", "#")):
                break
            block_lines.append(line)
        block = "\n".join(block_lines)
        self.assertIn(
            "AGENTS.md", block,
            "SEI-B4 RED: provider.config.yaml static_prompt 块未指向 AGENTS.md —— "
            "静态承载声明面与 carrier 分叉（ADR-001 三面相等破坏）。")

        # 契约 §1.6 告别回合纪律段在场（源锚：本包实现所兑现的契约条目）。
        contract = _read_text(CONTRACT_MD)
        self.assertTrue(contract.strip(), f"SEI-B4 RED: {CONTRACT_MD} 不在场。")
        s16 = _md_section(contract, "1.6", level="### ")
        self.assertTrue(
            s16.strip(), "SEI-B4 RED: 契约 §1.6 节窗口切取失败（标题漂移须复核）。")
        self.assertIn(
            "告别", s16,
            "SEI-B4 RED: 契约 §1.6 无告别回合纪律段 —— 本包实现的语义源头缺失"
            "（契约 §1.6 应已由 261115 架构批落地的告别回合纪律条）。")
        self.assertIn(END_SESSION_ACTION, s16,
                      "SEI-B4 RED: 契约 §1.6 告别纪律段未指 control/end_session。")

        # 静态面非空（AGENTS.md 即被测面；装载在场即三面之一）。
        self.assertTrue(agents_src.strip(), "SEI-B4 RED: AGENTS.md 静态面为空。")


if __name__ == "__main__":
    unittest.main()