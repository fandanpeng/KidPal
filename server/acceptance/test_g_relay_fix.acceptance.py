"""Frozen acceptance tests for TaskPackage 260907-g-relay-fix (subsystem: server).

Defect of record (changes/dev/260829-voice-roundtrip/review-packet.md SHOULD #1 +
roundtrip-live-evidence.md「新发现（G 包 live 面缺陷）」段, session 95ccf407): the
direction-B gateway front's REAL ws proxy path forwarded engine frames to the device with
ZERO text takeover — the `` ```display `` fence rode through into the TTS/文字同步 text and
no ``kidpal/display`` JSON-RPC tools/call was ever relayed, so the display text view face
(P3 翻屏语义) never triggered.

ARCHITECTURE OF RECORD (pod-events decision `engine-patch-approval`, event 37469170 —
human 批「方案 A + 首例引擎部署副本代码补丁」; 二轮真机冒烟已证引擎剥离 + front 链路全通):
the fence strip lives PRIMARILY in the ENGINE deploy copy (a patched connection.py
bind-mounted into the xiaozhi container, fence state machine + holdback at the LLM
streaming accumulation loop / tts_text_queue enqueue point); the FRONT is the
mechanical-validation + relay AUTHORITY: it consumes the engine's side-channel blocks,
runs display_schema (V27) and relays kidpal/display tools/call (决策23).  The front's
per-text-face strip stays as DEFENSE-IN-DEPTH only — cross-frame reassembly buffering is
explicitly NOT required on the front (the engine state machine owns streaming).

TRANSPORT OF RECORD (device wire facts, pinned against firmware/main/application.cc +
mcp_server.cc — 二轮冒烟暴露的最后一环断点):
  - application.cc:492: the device routes ONLY ``{"type":"mcp","payload":<JSON-RPC 对象>}``
    frames into McpServer::ParseMessage; application.cc:532 drops every other type — the
    front's former ``display_relay`` wrapper frame ended exactly there (串口实证
    ``Unknown message type: display_relay``).  display_relay is DEPRECATED: every relay
    envelope must go to the device as ONE ``{"type":"mcp","payload":<envelope>}`` frame
    (多块多帧, no batching).
  - mcp_server.cc:473: the JSON-RPC ``id`` MUST be a NUMBER (cJSON_IsNumber) — uuid hex
    strings are rejected with "Invalid id".  display relay ids are numeric (millisecond
    timestamp / counter).
  - mcp_server.cc:448: jsonrpc must be "2.0"; method tools/call / params.name /
    params.arguments shapes otherwise satisfied.
  - mcp_server.cc:105: the kidpal/display tool declares ``Property("view", String)`` +
    ``Property("data", String)`` — the envelope arguments are therefore
    ``{"view": <str>, "data": <json.dumps(data) STRING>}`` (the device parses data itself);
    passing data as an object is rejected.
  - Device receipt: ``{"type":"mcp","payload":{"jsonrpc":"2.0","id":<number>,"result":...}}``
    (result = the tool callback's ack object, e.g. {"route":"text"}) — relay_ack's
    「非 error result 即确认」semantics already accepts this; the FRONT must unwrap the
    {"type":"mcp","payload":...} frame before relay_ack (relay_ack 本体语义不动).
  - alert_tool_call rides the same frame type; the numeric-id migration of its
    deterministic reminder id belongs to the reminder-side package — NOT pinned here
    (注记, per decision engine-patch-approval follow-up).

FROZEN OFFLINE CONTRACT — part 1, engine deploy-copy patch (builder MUST land):
  server/deploy/engine-patch/connection.py
      The patched engine file (0.9.6-based deploy copy, full file or clean patch form),
      bind-mounted into the xiaozhi container.  Mechanical carrier teeth (source-text):
        - startup marker literal ``kidpal display-strip patch active`` — the engine MUST log
          this explicit marker at startup (live evidence + drift alarm anchor, GRF-A5);
        - the side-channel message type literal ``kidpal_display_blocks`` (emitted on the
          device ws toward the front);
        - the ``tts_text_queue`` enqueue point wired to the strip state machine (the patch
          references the frozen pure module ``kidpal_display_strip``);
        - ``dialogue.put`` receives the PRE-STRIP original reply text (对话历史保留 fence
          原文) — the patched call's arguments must reference an identifier containing
          ``original`` (frozen naming, e.g. ``original_text`` / ``original_reply``).
      Semantics the patch must implement (advisory, live-verified): 普通 delta 照常入队保延迟;
      delta 遇 ``` 转累积态直到闭合 fence; 闭合后整块从播报面摘除、原始载荷收进 blocks;
      流结束时未闭合 fence 不入 TTS（宁静默不念 JSON）、整段原文投侧通道; TTS delta 与
      response_message 全文共用同一剥离后输出流（单点剥离，防两处状态机漂移）.
  server/deploy/engine-patch/kidpal_display_strip.py
      The fence state machine as a PURE-python module (no engine/config imports at import
      time — it is bind-mounted next to connection.py and importable offline by this test):
        class DisplayFenceStateMachine:
            feed(delta: str) -> str            # speech-safe text to enqueue NOW ("" while
                                               # accumulating inside a fence — 延迟不损)
            finish() -> (tail_speech: str,     # end-of-stream speech tail; a still-unclosed
                         blocks: list[str])    # fence NEVER lands here（宁静默不念 JSON）—
                                               # its raw body lands in blocks instead
                                               # blocks: ALL raw fence payload strings, in
                                               # order, WITHOUT the ``` markers (closed ones
                                               # and the unclosed raw body alike)
        display_blocks_message(blocks) -> dict  # {"type": "kidpal_display_blocks",
                                                #  "blocks": [...raw strings, 哑管道透传,
                                                #  引擎零校验...]}
  server/deploy/ wiring: the compose/deploy config must bind-mount the engine-patch into the
  container (mechanical tooth: ``engine-patch`` referenced in the server/deploy corpus).

FROZEN OFFLINE CONTRACT — part 2, front (validation/relay authority, in
server/gateway/front/front_app.py — module must stay offline-importable, no network I/O at
import time; this acceptance loads it behind a local fastapi stub when fastapi is absent):

  strip_display_fences(text: str) -> (speech_text: str, display_blocks: list[str])
      DEFENSE-IN-DEPTH per-text-face split (contract §1.5 step 3): every `` ```display ``
      fenced block removed with ZERO residue, raw bodies returned verbatim in order (NOT
      parsed — dirty bodies go to the mechanical check to reject).  No fence -> (text, []).
      TOLERANCE (live session 95ccf407): inline form `` ```display{...}``` `` on ONE line
      must split cleanly — do not rely on the opener being followed by a newline.
  build_display_tool_calls(blocks: list[str]) -> list[dict]
      One kidpal/display JSON-RPC tools/call envelope per block, in order, NEVER silently
      dropped (len(result) == len(blocks)); envelope ids are NUMBERS (device
      cJSON_IsNumber); arguments are the device wire shape
      ``{"view": <str>, "data": <json.dumps(data) STRING>}`` (round-trip fidelity holds via
      json.loads(arguments["data"]) == the validated payload's data):
        - block parses as JSON and passes gateway.display_schema.validate_display
          -> relayed with that payload's view/data;
        - otherwise -> DOWNGRADED, not silent: arguments carry a FRIENDLY text view payload
          that itself passes validate_display with non-empty text, never the raw dirty
          payload (contract §1.5: 脏 JSON 拒绝并降级友好提示; V27).
      Every envelope: {"jsonrpc": "2.0", "id": <number>, "method": "tools/call",
      "params": {"name": "kidpal/display", "arguments": {"view": <str>, "data": <str>}}}.
  async pump_upstream_to_client(upstream, websocket)
      Engine->device leg WITH side-channel handler: binary (audio) frames forward unchanged
      as bytes.  Text frames that are JSON objects of type ``kidpal_display_blocks`` are
      INTERCEPTED (never forwarded verbatim to the device — engine-internal transport): the
      raw block strings feed build_display_tool_calls and EACH envelope goes toward the
      device as its own ``{"type":"mcp","payload":<envelope>}`` frame.  Text frames
      carrying a string "text" field are fence-stripped via strip_display_fences before
      forwarding (defense-in-depth) with any extracted blocks relayed the same way.
      Frames without a text field (hello, state, ...) forward verbatim.  The deprecated
      ``display_relay`` wrapper type must NOT appear on the wire.
  async pump_client_to_upstream(websocket, upstream) -> list
      Device->engine leg: reads starlette-style dict messages via await websocket.receive()
      ({"type": "websocket.receive", "text"/"bytes": ...}), forwards every frame to
      upstream.send() UNCHANGED (front 只消费回执、不重发 — 决策23), consumes device JSON-RPC
      render receipts via on_device_receipt, returns the list of receipt ids consumed as
      delivery acks.  Client disconnect (fastapi WebSocketDisconnect / RuntimeError) ends.
  on_device_receipt(message) -> (acked: bool, id)
      Unwraps a ``{"type":"mcp","payload":{...}}`` receipt frame (dict or JSON str) and
      delegates the payload to gateway.mcp_relay.relay_ack (渲染即确认、即出队语义由设备侧
      承载 — the front must NOT reimplement divergent semantics; relay_ack 本体语义不动).
  ws_gateway (the proxy path) MUST wire both pumps (structural source assertion below).

I/O doubles used by the pump teeth (frozen seam): _FakeUpstream (async-iterable + .send)
and _FakeDeviceSocket (.receive() starlette dicts + send_text/send_bytes), so the whole
proxy path is unit-exercised offline with zero sockets.

Live face (@pytest.mark.live, NOT auto-run — conftest mechanical skip, ARQ-260901-g3):
GRF-A5 真机一轮会话的日志证据断言, evidence lands per GRF-E2 under
changes/dev/260907-g-relay-fix/ (evidence form = what the stack naturally persists:
设备串口摘录 / 引擎 docker logs 摘录 / 证据 md — NOT wire literals).  Four facts:
  ⓪ engine patch marker line ``kidpal display-strip patch active`` present in the engine
     startup log excerpt (补丁真在跑, not a stale container);
  ① fence 残留清零, 播报+屏幕双面: 设备串口/文字同步文本 AND 屏幕渲染面均无 `` ```display ``
     字样（evidence must carry the session id + explicit clean statements for BOTH faces）;
  ② kidpal/display tools/call 实际下发且设备侧 McpServer 接受 (无 Unknown type / Invalid id
     拒收记录 — 二轮冒烟的 display_relay 拒收断点必须在三轮证据中证消);
  ③ display text 视图触发 (翻屏语义生效, 「上一页看不到」根因消除).

Coverage map (acceptanceRef -> test method):
  GRF-A1 (structural: 代理路径装配 + 侧通道 handler 接线)
        -> test_a1_front_app_loads_and_ws_path_wires_takeover
  GRF-A2 (fence 剥离——引擎侧为主: 部署副本补丁 + 状态机 + 装载形态; front 纵深防御)
        -> test_a2_engine_patch_carrier_landed
        -> test_a2_engine_strip_state_machine_semantics
        -> test_a2_engine_patch_deploy_wiring
        -> test_a2_strip_display_fences_canonical_block            (front 纵深防御)
        -> test_a2_strip_display_fences_no_fence_passthrough
        -> test_a2_strip_display_fences_multiple_and_inline_opener
        -> test_a2_strip_display_fences_dirty_bodies_still_stripped
        -> test_a3_pump_upstream_takes_over_text_stream            (front 文本面 wiring)
  GRF-A3 (display 机械校验 + kidpal/display relay 下发; 设备 wire 形态: mcp 帧载 / 数字 id /
          data 字符串化; 侧通道 blocks → 校验 → 信封; 降级不静默)
        -> test_a3_display_tool_calls_envelope_for_valid_block
        -> test_a3_display_tool_calls_downgrades_invalid_not_silent
        -> test_a3_pump_side_channel_blocks_relayed
        -> test_a3_pump_upstream_takes_over_text_stream
  GRF-A4 (回执闭环——设备渲染回执 mcp 帧解包后经 relay_ack 消费, front 不重发)
        -> test_a4_device_receipt_relay_ack_consumption
  GRF-A5 (live 实证——真机一轮会话四事实: 补丁标记行 + fence 双面清零 + relay 下发被设备接受
          + text 视图)
        -> test_a5_live_engine_patch_marker_in_log
        -> test_a5_live_fence_not_in_broadcast
        -> test_a5_live_kidpal_display_tools_call_dispatched
        -> test_a5_live_display_text_view_triggered
  GRF-E1 (既有 G 包验收不回归)
        -> test_e1_existing_directionb_acceptance_structural_green
  GRF-E2 (live 证据落 changes/dev/260907-g-relay-fix/) — the live teeth's evidence contract

Red baseline (transport-revision round, 2026-09-07): the front five functions and the
defense-in-depth strip are GREEN; the DEVICE wire form is NOT — envelopes still ride the
deprecated display_relay wrapper with uuid-string ids and object-typed data, and receipts
are not unwrapped, so the transport teeth are RED for exactly those missing behaviours.
test_e1 is the regression guard (must stay green through the fix).

decisionCoverage landing (TaskPackage 260907-g-relay-fix):
  contract-§1.5-网关职责收敛 -> GRF-A2 teeth (engine-side primary strip + front 校验/relay 权威)
  contract-V27-display机械校验 -> GRF-A3 teeth
  spec-决策23-mcp载体      -> GRF-A3 transport teeth + GRF-A4 teeth (设备 mcp 帧型/回执解包)
  (no dropped / risk-accepted entries)
"""

# --------------------------------------------------------------------------- #
# REVISION LOG — test_g_relay_fix.acceptance.py（本文件首次修订）               #
# --------------------------------------------------------------------------- #
# 2026-09-12 · changeId=260911-display-first-m1（PKG-4 显示先行 M1）
# 修订者 = acceptance-author。本文件由 260907-g-relay-fix 包冻结，本次为 M1「围栏
# 闭合即发」做 additive 时序扩展修订。逐条修订声明 + 语义不放松自证：
#
#  [1] additive——新增 class DisplayFirstM1Acceptance（DF-A1/DF-A2 新齿），钉
#      kidpal_display_strip.DisplayFenceStateMachine 的闭合回调 API + connection.py
#      接线 + finish() 语义保全。既有 GRelayFixStructuralAcceptance /
#      GRelayFixLiveAcceptance 全部齿一字未动（语义零放松）。
#      语义不放松自证：下列既有齿断言语义逐字保持——test_a2_engine_strip_state_
#      machine_semantics 的 feed/finish 语义（feed 直通保延迟、闭合摘除、未闭合宁静默
#      不念 JSON、多块顺序/裸载荷/无 ``` 标记、display_blocks_message 哑管道成形）、
#      test_a2_strip_display_fences_* 四条（front 纵深防御）、GRF-A3 信封/wire/降级、
#      GRF-A4 回执、GRF-E1 回归。M1 后 finish() 仍返回 (tail_speech, blocks)、blocks
#      仍按序承载全部裸载荷字符串（含未闭合尾块）、不含 ``` 标记；on_block_closed 是
#      新增发射信道，不改 feed()/finish() 签名与返回语义。
#
#  [2] kidpal_display_strip.py 冻结离线契约的 additive 语义扩展边界（builder 按此物理
#      实现并落码；语义裁决与 REVISION LOG 归 acceptance-author，实现落码面
#      server/deploy/engine-patch/** 归 builder——两者不交叉，见 taskpackage notes
#      DF-E3 scope-map 落地解释）：
#        class DisplayFenceStateMachine:
#            def __init__(self, on_block_closed=None) -> None:
#                on_block_closed: Optional[Callable[[str], None]]，默认 None（无回调 =
#                与 M1 前完全一致，向后兼容；既有齿 sm_cls() 零参数构造继续成立）。
#                事件边界：
#                  - feed() 内 fence 闭合（closer ``` 被消费）的瞬间，同步调用一次
#                    on_block_closed(block)；block = 裸载荷字符串（无 ``` 标记；
#                    language tag + 一个前导换行已剥除，与 finish().blocks 每条 body
#                    逐字节同形）；
#                  - 流末仍未闭合的 fence：在 finish() 内同步调用一次 on_block_closed
#                    一次（仍宁静默不念，其裸 body 照旧进 finish().blocks）；
#                  - 不变量：一次完整流（finish() 已调）后，on_block_closed 触发的载荷
#                    序列 == finish().blocks（同序同集）——发射信道唯一（回调），
#                    finish().blocks 只作观察镜像，实现不得双发（closed 块逐个在 feed
#                    内早发、unclosed 尾在 finish 发，每条块恰发射一次）。
#            feed(delta: str) -> str              # 签名/语义不改
#            finish() -> (tail_speech, blocks)    # 签名/语义不改
#        单点原则保持：feed/finish/回调同属一个 DisplayFenceStateMachine 实例（不建
#        第二状态机）；模块仍纯 Python、离线可 import、零引擎依赖。
#
#  [3] DF-A4（设备侧语音中渲染）承载映射——判断：已承载于 PKG-2 冻结齿，本包不新钉
#      host 齿。设备侧「display 块中途到达按 PKG-2 layer 仲裁规则渲染」由已并入 base
#      的 260911-screen-unification 冻结齿承载：
#        - firmware/main/test/screen_unification.acceptance.c test b1（标签 [SU-A2]
#          [SU-A4]，L616）钉「SERVER_PAGE(4) 盖过 VOICE(3)——回答期间服务端页前台可见、
#          语音不拦内容页（SU-RULING-01 方案A）+ 聊天 layer 常驻」；
#        - 同文件 test b3（[SU-A5][SU-A7]，L787）钉「语音轮→回答驻留→回首页前台归属」；
#        - 同文件 test s2（[SU-A1]，L893）钉「kidpal_layer_ 前缀 >= 3 文件消费者 wiring
#          + kidpal_layer.c 编入设备构建」。
#        仲裁规则与到达早晚无关（M1 只改引擎侧发射时机，设备规则同一套），「中途到达」
#        =「提前到达」，无新行为面需钉。设备侧落地接线已随 PKG-2 并入 base：
#        kidpal_render_node.cc:358-371（begin_page 推 SERVER_PAGE，前台盖过 VOICE）、
#        :302-326（voice_begin 清上轮驻留后推 VOICE、聊天 chrome 恒可见）。DF-A4 后半
#        「与 PKG-2 合并基线复跑其冻结齿绿」= 阶段 3 跨包回归动作（builder/verifier 按
#        screen_unification.acceptance.c 头注 run protocol 复跑全绿），非本文件新齿。
# --------------------------------------------------------------------------- #

from __future__ import annotations

import asyncio
import importlib.util
import inspect
import json
import os
import re
import subprocess
import sys
import types
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import pytest  # noqa: F401  (markers only; the standalone runner also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"
GATEWAY_DIR = SERVER / "gateway"
DEPLOY_DIR = SERVER / "deploy"
ENGINE_PATCH_DIR = DEPLOY_DIR / "engine-patch"
ENGINE_PATCH_CONNECTION = ENGINE_PATCH_DIR / "connection.py"
ENGINE_PATCH_STRIP = ENGINE_PATCH_DIR / "kidpal_display_strip.py"
CHANGES_DEV = REPO_ROOT / "changes" / "dev"
LIVE_EVIDENCE_DIR = CHANGES_DEV / "260907-g-relay-fix"

# Frozen literals (engine patch of record — pod-events decision engine-patch-approval).
ENGINE_PATCH_MARKER = "kidpal display-strip patch active"
SIDE_CHANNEL_TYPE = "kidpal_display_blocks"
DEPRECATED_WRAPPER_TYPE = "display_relay"

# --------------------------------------------------------------------------- #
# Frozen display samples (real fence shapes, incl. the live-evidence forms)    #
# --------------------------------------------------------------------------- #

_DISPLAY_PAYLOAD = {
    "view": "text",
    "data": {"text": "月亮盈亏小知识：月亮自己不会发光，太阳照到哪里哪里亮，我们看到的亮面每天都不一样哦。"},
}
_FENCE_BODY = json.dumps(_DISPLAY_PAYLOAD, ensure_ascii=False, indent=2)
_SP_BEFORE = "月亮为什么会变弯呢？让我来告诉你这个小秘密呀。"
_SP_AFTER = "今晚也抬头看看月亮吧。"

# canonical OpenClaw reply shape: speech + fenced multiline display JSON + speech.
_LLM_TEXT_WITH_FENCE = _SP_BEFORE + "\n```display\n" + _FENCE_BODY + "\n```\n" + _SP_AFTER

_PAYLOAD_2 = {"view": "text", "data": {"text": "第二块显示内容。"}}
_TEXT_TWO_FENCES = (
    "开头一句。```display\n" + json.dumps(_DISPLAY_PAYLOAD, ensure_ascii=False) + "\n```"
    + "中间一句。```display\n" + json.dumps(_PAYLOAD_2, ensure_ascii=False) + "\n```"
    + "结尾一句。"
)

# live-evidence form (session 95ccf407): the engine's sentence splitter ate the newline
# after the opener, so the whole fence sits on ONE line: ```display{...}```
_INLINE_JSON = '{"view": "text", "data": {"text": "小卡片内容"}}'
_TEXT_INLINE_OPENER = "好的呀。\n```display" + _INLINE_JSON + "```\n结束啦。"

# dirty bodies the mechanical check must reject (V27) — but the SPLITTER/STATE MACHINE must
# still extract them so nothing leaks into the speech face and the check (not the child)
# sees them.
_MALFORMED_BLOCK = '{"view": "text", "data": {"text": "被截断的回答'
_OVERSIZED_BLOCK = json.dumps({"view": "text", "data": {"text": "长" * 100_000}}, ensure_ascii=False)

# Streaming deltas for the engine state machine teeth (fence split across deltas, exactly
# how the LLM stream arrives at the patched accumulation loop).
_SM_DELTAS_CLOSED = [
    "今天讲讲",
    "月亮呀。\n```",
    'display\n{"view": "te',
    'xt", "data": {"text": "卡片内容"}}\n',
    "```",
    "再见啦。",
]
_SM_EXPECTED_SPEECH_CLOSED = "今天讲讲月亮呀。\n" + "再见啦。"
_SM_DELTAS_UNCLOSED = [
    "开始讲。",
    "```display\n" + '{"view": "text", "data": {"text": "没讲完的卡',
    "片内容",
]

# Device receipt samples (wire facts: numeric id per mcp_server.cc:473; the tool callback's
# result object rides the mcp frame — relay_ack's non-error-result semantics accept it).
_ACK_RECEIPT = {"jsonrpc": "2.0", "id": 172001, "result": {"ack": True}}
_ACK_RECEIPT_WRAPPED = {"type": "mcp", "payload": _ACK_RECEIPT}


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _files_under(root: Path, suffixes):
    if not root.is_dir():
        return []
    return sorted(p for p in root.rglob("*")
                  if p.is_file() and p.suffix in suffixes)


# --------------------------------------------------------------------------- #
# Offline loading of front_app.py (fastapi stubbed when absent — deterministic) #
# --------------------------------------------------------------------------- #

_FRONT_MODULE_NAME = "kidpal_front_app_gfix"
_front_module_cache = None


def _ensure_fastapi_stub() -> None:
    """Make front_app importable on a bare box: stub fastapi unless really installed.

    Exception classes must be REAL classes so `except (WebSocketDisconnect, ...)` in the
    module keeps working behind the stub; everything else is a permissive Mock.
    """
    try:
        import fastapi  # noqa: F401
        return  # real fastapi present — even better, use it
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    if "fastapi" not in sys.modules:
        stub = _PermissiveModule("fastapi")
        stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
        stub.HTTPException = type("HTTPException", (Exception,), {})
        sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline.

    Returns (module|None, err).  A missing/unimportable module is a RED for missing
    behaviour (self.fail below), never a collection crash.
    """
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing (ws proxy path not implemented)"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location(_FRONT_MODULE_NAME, FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "server/gateway/front/front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"server/gateway/front/front_app.py failed to import: {e!r}. "
                      "The ws-proxy module of record must stay offline-importable "
                      "(no network I/O at import time).")


def _load_engine_strip_module():
    """Import server/deploy/engine-patch/kidpal_display_strip.py by path (pure python).

    Returns (module|None, err).
    """
    if not ENGINE_PATCH_STRIP.is_file():
        return None, ("server/deploy/engine-patch/kidpal_display_strip.py missing — the engine "
                      "deploy-copy fence state machine is not landed (方案 A 未落地).")
    try:
        spec = importlib.util.spec_from_file_location("kidpal_engine_display_strip", ENGINE_PATCH_STRIP)
        if spec is None or spec.loader is None:
            return None, "kidpal_display_strip.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"kidpal_display_strip.py failed to import: {e!r}. Frozen contract: the "
                      "engine state-machine module MUST be pure python (no engine/config imports "
                      "at import time) so this acceptance runs offline anywhere.")


def _front_source() -> str:
    return _read_text(FRONT_APP)


def _front_callable(name: str):
    """Fetch one frozen-contract callable from front_app; (callable|None, err)."""
    mod, err = _load_front_app()
    if mod is None:
        return None, err
    fn = getattr(mod, name, None)
    if not callable(fn):
        return None, (f"front_app.py has no module-level callable {name}() — the ws proxy "
                      "path takeover is not implemented (frozen contract).")
    return fn, None


# --------------------------------------------------------------------------- #
# I/O doubles for the pump teeth (frozen seam — see module docstring)          #
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


def _parsed_outbound_text_frames(sent_text):
    frames = []
    for text in sent_text:
        try:
            frames.append(json.loads(text))
        except (TypeError, ValueError):
            continue  # non-JSON outbound text
    return frames


def _extract_mcp_envelopes(parsed_frames):
    """kidpal/display tools/call envelopes as the DEVICE accepts them: ONLY inside
    {"type":"mcp","payload":<JSON-RPC>} frames (application.cc:492 routes type mcp into
    McpServer::ParseMessage; :532 drops every other type — the deprecated display_relay
    wrapper ended exactly there)."""
    out = []
    for frame in parsed_frames:
        if (isinstance(frame, dict) and frame.get("type") == "mcp"
                and isinstance(frame.get("payload"), dict)):
            payload = frame["payload"]
            params = payload.get("params") or {}
            if payload.get("method") == "tools/call" and params.get("name") == "kidpal/display":
                out.append(payload)
    return out


def _wire_arguments_match(arguments, expected_payload):
    """True iff arguments is the device wire shape {"view": <str>, "data": <json STRING>}
    (mcp_server.cc:105: both Properties are String-typed; the device parses data itself)
    and the data string round-trips to the expected payload's data."""
    if not isinstance(arguments, dict) or not isinstance(arguments.get("view"), str):
        return False
    data = arguments.get("data")
    if not isinstance(data, str):
        return False
    try:
        return json.loads(data) == expected_payload.get("data")
    except ValueError:
        return False


def _wire_payload_of(arguments):
    """Reconstruct the display payload from the device wire shape for validate_display."""
    if not isinstance(arguments, dict) or not isinstance(arguments.get("data"), str):
        return None
    try:
        return {"view": arguments.get("view"), "data": json.loads(arguments["data"])}
    except ValueError:
        return None


# --------------------------------------------------------------------------- #
# Structural / offline deterministic acceptance — the red baseline teeth       #
# --------------------------------------------------------------------------- #

class GRelayFixStructuralAcceptance(unittest.TestCase):
    """Structural offline teeth: engine-side primary strip + front validation/relay authority,
    on the device-accepted mcp wire form."""

    maxDiff = None

    # --- GRF-A1 -------------------------------------------------------------
    def test_a1_front_app_loads_and_ws_path_wires_takeover(self):
        """coverage: GRF-A1 — front_app 可离线装载，ws 代理路径接线 takeover 泵 + 侧通道 handler."""
        mod, err = _load_front_app()
        self.assertIsNotNone(
            mod,
            "GRF-A1 RED: " + (err or "front_app.py not loadable"),
        )
        src = _front_source()
        self.assertIn(
            "async def ws_gateway", src,
            "GRF-A1 RED: the device-facing ws proxy path (ws_gateway) is gone from front_app.py.",
        )
        # def + at least one call site inside the proxy path, for BOTH pump directions.
        self.assertGreaterEqual(
            src.count("pump_upstream_to_client"), 2,
            "GRF-A1 RED: front_app.py's ws proxy path never wires the engine->device takeover "
            "pump (pump_upstream_to_client defined but not invoked by the proxy path).",
        )
        self.assertGreaterEqual(
            src.count("pump_client_to_upstream"), 2,
            "GRF-A1 RED: front_app.py's ws proxy path never wires the device->engine pump "
            "(pump_client_to_upstream) — the receipt-consumption leg of the proxy is missing.",
        )
        self.assertIn(
            SIDE_CHANNEL_TYPE, src,
            f"GRF-A1 RED: front_app.py never references '{SIDE_CHANNEL_TYPE}' — the engine "
            "side-channel handler (blocks → 校验 → relay) is not wired into the proxy path "
            "(新架构方案 A: 引擎剥离为主, front 为校验/relay 权威).",
        )

    # --- GRF-A2: engine deploy-copy patch (primary strip) --------------------
    def test_a2_engine_patch_carrier_landed(self):
        """coverage: GRF-A2 — 引擎部署副本补丁入仓：connection.py 补丁 + 状态机纯模块 + 关键接线."""
        self.assertTrue(
            ENGINE_PATCH_CONNECTION.is_file(),
            "GRF-A2 RED: server/deploy/engine-patch/connection.py missing — the engine-side "
            "fence-strip deploy copy (0.9.6-based patch, 方案 A) is not landed in the repo.",
        )
        self.assertTrue(
            ENGINE_PATCH_STRIP.is_file(),
            "GRF-A2 RED: server/deploy/engine-patch/kidpal_display_strip.py missing — the "
            "fence state machine must be a pure module bind-mounted next to connection.py.",
        )
        src = _read_text(ENGINE_PATCH_CONNECTION)
        self.assertIn(
            ENGINE_PATCH_MARKER, src,
            f"GRF-A2 RED: the patched connection.py never logs the startup marker "
            f"'{ENGINE_PATCH_MARKER}' — 无显式标记则 live 无法核验补丁真在跑、无漂移告警锚.",
        )
        self.assertIn(
            SIDE_CHANNEL_TYPE, src,
            f"GRF-A2 RED: patched connection.py never emits the '{SIDE_CHANNEL_TYPE}' "
            "side-channel message — 剥离出的 blocks 无投递通道.",
        )
        self.assertIn(
            "tts_text_queue", src,
            "GRF-A2 RED: the patch does not touch the tts_text_queue enqueue point — fence "
            "state machine must gate the TTS queue (普通文本照常入队保延迟, fence 累积态拦截).",
        )
        self.assertIn(
            "kidpal_display_strip", src,
            "GRF-A2 RED: the patch does not reference the frozen pure module "
            "kidpal_display_strip — 状态机必须单点实现（防 TTS delta 与 response_message "
            "两处状态机漂移）.",
        )
        self.assertIn(
            "response_message", src,
            "GRF-A2 RED: the patch never touches response_message — TTS delta 与 response "
            "全文必须共用同一剥离后输出流（单点剥离）.",
        )
        m = re.search(r"dialogue\.put\(([^)]*)", src)
        self.assertIsNotNone(
            m,
            "GRF-A2 RED: the patch never calls dialogue.put — 对话历史写入面缺失.",
        )
        self.assertIn(
            "original", (m.group(1) or "").lower(),
            f"GRF-A2 RED: dialogue.put({(m.group(1) or '')[:80]!r}…) does not reference the "
            "PRE-STRIP original text (frozen naming: the call args must carry an identifier "
            "containing 'original') — 对话历史必须保留 fence 原文，剥离只作用于播报面.",
        )

    def test_a2_engine_strip_state_machine_semantics(self):
        """coverage: GRF-A2 — 引擎 fence 状态机语义：流式 holdback / 闭合摘除 / 未闭合宁静默 /
        哑管道侧通道消息."""
        mod, err = _load_engine_strip_module()
        if mod is None:
            self.fail("GRF-A2 RED: " + err)
        sm_cls = getattr(mod, "DisplayFenceStateMachine", None)
        self.assertTrue(
            callable(sm_cls),
            "GRF-A2 RED: kidpal_display_strip.DisplayFenceStateMachine missing (frozen "
            "state-machine contract: feed/finish).",
        )

        # (1) plain deltas flow through IMMEDIATELY — 保延迟, no holdback on fence-free text.
        sm = sm_cls()
        self.assertEqual(
            sm.feed("你好呀，"), "你好呀，",
            "GRF-A2 RED: fence-free delta was withheld — 普通文本必须照常入队保延迟.",
        )
        self.assertEqual(sm.feed("讲讲月亮。"), "讲讲月亮。",
                         "GRF-A2 RED: fence-free deltas must pass through verbatim.")

        # (2) fence split across deltas: accumulation until the closer, then clean speech.
        sm = sm_cls()
        speech_parts = []
        for delta in _SM_DELTAS_CLOSED:
            speech_parts.append(sm.feed(delta))
        tail, blocks = sm.finish()
        speech = "".join(speech_parts) + tail
        self.assertEqual(
            speech, _SM_EXPECTED_SPEECH_CLOSED,
            f"GRF-A2 RED: streaming strip mangled the speech face: {speech!r} (expected "
            f"{_SM_EXPECTED_SPEECH_CLOSED!r}) — 非 fence 正文必须原样按序流出.",
        )
        self.assertNotIn("```", speech, "GRF-A2 RED: '```' residue in the streamed speech face.")
        self.assertNotIn("卡片内容", speech,
                         "GRF-A2 RED: fence body leaked into the streamed speech face.")
        self.assertEqual(len(blocks), 1, f"GRF-A2 RED: expected 1 closed block, got {blocks!r}.")
        self.assertEqual(
            json.loads(blocks[0]),
            {"view": "text", "data": {"text": "卡片内容"}},
            "GRF-A2 RED: the closed block's raw payload was not carried verbatim (无标记包裹、"
            "原样载荷，哑管道口径).",
        )

        # (3) unclosed fence at stream end: 宁静默不念 JSON — never reaches the speech face,
        # raw body goes to the side channel instead.
        sm = sm_cls()
        speech_parts = []
        for delta in _SM_DELTAS_UNCLOSED:
            speech_parts.append(sm.feed(delta))
        tail, blocks = sm.finish()
        speech = "".join(speech_parts) + tail
        self.assertIn("开始讲。", speech, "GRF-A2 RED: speech before the unclosed fence was lost.")
        self.assertNotIn("```", speech,
                         "GRF-A2 RED: the unclosed fence leaked into the speech face at stream end.")
        self.assertNotIn("没讲完的卡", speech,
                         "GRF-A2 RED: the unclosed fence BODY leaked into the speech face — "
                         "流结束时未闭合 fence 不入 TTS（宁静默不念 JSON）.")
        self.assertEqual(len(blocks), 1, f"GRF-A2 RED: unclosed fence body missing from blocks: {blocks!r}.")
        self.assertIn("没讲完的卡", blocks[0],
                      "GRF-A2 RED: the unclosed fence's raw body was not preserved for the side channel.")

        # (4) multiple closed fences across the stream, payloads in order.
        sm = sm_cls()
        for delta in (_SM_DELTAS_CLOSED[:-1]
                      + ["再见啦。第二段。```display\n"
                         + json.dumps(_PAYLOAD_2, ensure_ascii=False) + "\n```\n完啦。"]):
            sm.feed(delta)
        _tail, blocks = sm.finish()
        self.assertEqual(len(blocks), 2, f"GRF-A2 RED: expected 2 blocks in order, got {blocks!r}.")
        self.assertEqual(json.loads(blocks[1]), _PAYLOAD_2)

        # (5) side-channel message construction: raw passthrough, engine is a dumb pipe.
        msg_fn = getattr(mod, "display_blocks_message", None)
        self.assertTrue(
            callable(msg_fn),
            "GRF-A2 RED: kidpal_display_strip.display_blocks_message missing — 侧通道消息构造 "
            f"({{'type': '{SIDE_CHANNEL_TYPE}', 'blocks': [...]}}) 未实现.",
        )
        dirty = "not-json-at-all )))"
        msg = msg_fn([blocks[0], dirty])
        self.assertEqual(
            msg, {"type": SIDE_CHANNEL_TYPE, "blocks": [blocks[0], dirty]},
            "GRF-A2 RED: display_blocks_message must be exactly "
            f"{{'type': '{SIDE_CHANNEL_TYPE}', 'blocks': [...]}} with RAW string payloads — "
            "引擎零校验哑管道（校验权威在 front 的 display_schema）.",
        )

    def test_a2_engine_patch_deploy_wiring(self):
        """coverage: GRF-A2 — compose/deploy 装载形态：engine-patch 以 bind-mount 进容器."""
        corpus = "\n".join(_read_text(p) for p in _files_under(DEPLOY_DIR, {".yml", ".yaml", ".sh"}))
        self.assertIn(
            "engine-patch", corpus,
            "GRF-A2 RED: no server/deploy compose/script references 'engine-patch' — 补丁未以 "
            "bind-mount 装载进 xiaozhi 容器（部署副本不进容器 = 补丁不生效）.",
        )

    # --- GRF-A2 (front defense-in-depth) --------------------------------------
    def test_a2_strip_display_fences_canonical_block(self):
        """coverage: GRF-A2 — front 纵深防御：canonical fence（多行 JSON、前后正文）剥离零残留."""
        strip, err = _front_callable("strip_display_fences")
        if strip is None:
            self.fail("GRF-A2 RED: " + err)
        result = strip(_LLM_TEXT_WITH_FENCE)
        self.assertIsInstance(
            result, tuple,
            "GRF-A2 RED: strip_display_fences must return (speech_text, display_blocks).",
        )
        speech, blocks = result
        self.assertNotIn(
            "```", speech,
            f"GRF-A2 RED: fence residue '```' leaked into the speech text: {speech!r}. 设备收到的 "
            "TTS/文字同步文本必须零 fence 残留（live session 95ccf407 缺陷）.",
        )
        self.assertIn(_SP_BEFORE, speech, "GRF-A2 RED: fence 前的正文丢失（非 fence 部分必须原样保留）.")
        self.assertIn(_SP_AFTER, speech, "GRF-A2 RED: fence 后的正文丢失（非 fence 部分必须原样保留）.")
        self.assertLess(
            speech.index(_SP_BEFORE), speech.index(_SP_AFTER),
            "GRF-A2 RED: non-fence text order was scrambled by the strip.",
        )
        self.assertNotIn(
            "月亮盈亏小知识", speech,
            "GRF-A2 RED: the display block BODY leaked into the speech text — display JSON 是屏幕面，"
            "不得混入播报文本.",
        )
        self.assertEqual(len(blocks), 1, f"GRF-A2 RED: expected exactly 1 extracted block, got {blocks!r}.")
        self.assertEqual(
            json.loads(blocks[0]), _DISPLAY_PAYLOAD,
            "GRF-A2 RED: the extracted block is not the raw fence body (must be handed to the "
            "downstream mechanical check verbatim).",
        )

    def test_a2_strip_display_fences_no_fence_passthrough(self):
        """coverage: GRF-A2 — 无 fence 文本必须原样直通（不得误伤普通回复）."""
        strip, err = _front_callable("strip_display_fences")
        if strip is None:
            self.fail("GRF-A2 RED: " + err)
        plain = "天空是什么颜色的？现在嘛，天空大概是那种刚睡醒的淡蓝色。"
        speech, blocks = strip(plain)
        self.assertEqual(
            speech, plain,
            "GRF-A2 RED: fence-free engine text was mutated — 无 fence 时必须原样保留（直通）.",
        )
        self.assertEqual(
            blocks, [],
            f"GRF-A2 RED: fence-free text produced phantom display blocks: {blocks!r}.",
        )

    def test_a2_strip_display_fences_multiple_and_inline_opener(self):
        """coverage: GRF-A2 — 多 fence 块逐一剥离 + live 证据形态（opener 后无换行、整行内联）."""
        strip, err = _front_callable("strip_display_fences")
        if strip is None:
            self.fail("GRF-A2 RED: " + err)
        speech, blocks = strip(_TEXT_TWO_FENCES)
        self.assertNotIn("```", speech,
                         "GRF-A2 RED: '```' residue after stripping two display fences.")
        for marker in ("开头一句。", "中间一句。", "结尾一句。"):
            self.assertIn(marker, speech, f"GRF-A2 RED: 正文 {marker!r} 在剥离后丢失.")
        self.assertNotIn("月亮盈亏小知识", speech, "GRF-A2 RED: 第一个 display 块体漏进播报文本.")
        self.assertNotIn("第二块显示内容", speech, "GRF-A2 RED: 第二个 display 块体漏进播报文本.")
        self.assertEqual(len(blocks), 2, f"GRF-A2 RED: expected 2 blocks, got {blocks!r}.")
        self.assertEqual(json.loads(blocks[0]), _DISPLAY_PAYLOAD)
        self.assertEqual(json.loads(blocks[1]), _PAYLOAD_2)

        # the live-evidence inline form (engine sentence splitter ate the newline):
        speech2, blocks2 = strip(_TEXT_INLINE_OPENER)
        self.assertNotIn(
            "```", speech2,
            "GRF-A2 RED: inline-opener fence (```display{...}``` on ONE line, live session "
            "95ccf407 form) left '```' residue in the speech text.",
        )
        self.assertNotIn(
            "小卡片内容", speech2,
            "GRF-A2 RED: inline-opener fence body leaked into the speech text — 不得依赖 opener "
            "后有换行（引擎切句会吃掉它）.",
        )
        self.assertIn("好的呀。", speech2)
        self.assertIn("结束啦。", speech2)
        self.assertEqual(len(blocks2), 1, f"GRF-A2 RED: inline fence not extracted: {blocks2!r}.")
        self.assertEqual(json.loads(blocks2[0]), {"view": "text", "data": {"text": "小卡片内容"}})

    def test_a2_strip_display_fences_dirty_bodies_still_stripped(self):
        """coverage: GRF-A2 — 畸形/超大 fence 体同样被剥离出播报面，交给下游机械校验处置."""
        strip, err = _front_callable("strip_display_fences")
        if strip is None:
            self.fail("GRF-A2 RED: " + err)
        dirty_text = ("回答前半。" + "```display\n" + _MALFORMED_BLOCK + "\n```"
                      + "回答后半。" + "```display\n" + _OVERSIZED_BLOCK + "\n```")
        speech, blocks = strip(dirty_text)
        self.assertNotIn(
            "```", speech,
            "GRF-A2 RED: a MALFORMED-JSON fence left '```' residue in speech — 剥离是语法层动作，"
            "脏体也要先剥离出播报面再交机械校验拒绝.",
        )
        self.assertNotIn(
            "被截断的回答", speech,
            "GRF-A2 RED: malformed fence body leaked into the speech text.",
        )
        self.assertIn("回答前半。", speech)
        self.assertIn("回答后半。", speech)
        self.assertEqual(len(blocks), 2, f"GRF-A2 RED: dirty fences must still be extracted: {blocks!r}.")
        self.assertIn("被截断的回答", blocks[0], "GRF-A2 RED: malformed raw body not preserved for the check.")
        self.assertIn("长", blocks[1], "GRF-A2 RED: oversized raw body not preserved for the check.")

    # --- GRF-A3 -------------------------------------------------------------
    def test_a3_display_tool_calls_envelope_for_valid_block(self):
        """coverage: GRF-A3 — 合法 display 块 → kidpal/display JSON-RPC tools/call 信封，设备
        wire 形态：数字 id（mcp_server.cc:473 cJSON_IsNumber）+ data 字符串化（:105 String 属性）."""
        build, err = _front_callable("build_display_tool_calls")
        if build is None:
            self.fail("GRF-A3 RED: " + err)
        self.assertEqual(
            build([]), [],
            "GRF-A3 RED: empty block list must yield no envelopes.",
        )
        calls = build([json.dumps(_DISPLAY_PAYLOAD, ensure_ascii=False)])
        self.assertEqual(
            len(calls), 1,
            f"GRF-A3 RED: expected exactly one relay envelope, got {calls!r}.",
        )
        call = calls[0]
        self.assertEqual(
            call.get("method"), "tools/call",
            "GRF-A3 RED: relay is not a JSON-RPC tools/call (决策23 / contract §四).",
        )
        self.assertEqual(
            call.get("jsonrpc"), "2.0",
            "GRF-A3 RED: relay envelope is not JSON-RPC 2.0 (mcp_server.cc:448).",
        )
        cid = call.get("id")
        self.assertIsInstance(
            cid, int,
            f"GRF-A3 RED: envelope id {cid!r} is not a JSON number — 设备 mcp_server.cc:473 "
            "cJSON_IsNumber 拒收字符串 id（二轮冒烟「Invalid id」断点），display relay id 必须"
            "数字化（毫秒时间戳/递增计数器）.",
        )
        self.assertNotIsInstance(cid, bool, "GRF-A3 RED: envelope id must be a real number, not bool.")
        params = call.get("params") or {}
        self.assertEqual(
            params.get("name"), "kidpal/display",
            f"GRF-A3 RED: tool name={params.get('name')!r}, expected 'kidpal/display'.",
        )
        arguments = params.get("arguments")
        self.assertTrue(
            _wire_arguments_match(arguments, _DISPLAY_PAYLOAD),
            f"GRF-A3 RED: arguments {json.dumps(arguments, ensure_ascii=False)[:200]!r} is not "
            "the device wire shape {\"view\": <str>, \"data\": <json.dumps(data) STRING>} — "
            "mcp_server.cc:105 的 kidpal/display 两个 Property 均为 String，data 必须字符串化、"
            "设备端自行解析（对象形态被拒）.",
        )

    def test_a3_display_tool_calls_downgrades_invalid_not_silent(self):
        """coverage: GRF-A3 — 畸形/超大块拒绝且降级友好提示，绝不静默丢弃、绝不盲转发脏载荷 (V27)."""
        build, err = _front_callable("build_display_tool_calls")
        if build is None:
            self.fail("GRF-A3 RED: " + err)
        mod, _ = _load_front_app()
        validate = mod.gw_display.validate_display  # the frozen display_schema module, as composed
        blocks = [_MALFORMED_BLOCK, _OVERSIZED_BLOCK]
        calls = build(blocks)
        self.assertEqual(
            len(calls), len(blocks),
            "GRF-A3 RED: invalid blocks were silently dropped — 每个块必须有交待（降级不静默，"
            "契约 §1.5「脏 JSON 拒绝并降级友好提示」）.",
        )
        for call in calls:
            self.assertEqual(call.get("method"), "tools/call", "GRF-A3 RED: downgrade entry is not a tools/call.")
            self.assertIsInstance(
                call.get("id"), int,
                "GRF-A3 RED: downgraded envelope id is not a JSON number (mcp_server.cc:473).",
            )
            params = call.get("params") or {}
            self.assertEqual(params.get("name"), "kidpal/display",
                             "GRF-A3 RED: downgrade entry is not kidpal/display.")
            arguments = params.get("arguments")
            payload = _wire_payload_of(arguments)
            self.assertIsNotNone(
                payload,
                f"GRF-A3 RED: downgraded arguments {arguments!r} is not the device wire shape "
                "{\"view\": <str>, \"data\": <json STRING>}.",
            )
            ok, reason = validate(payload)
            self.assertTrue(
                ok,
                f"GRF-A3 RED: downgraded payload itself fails display_schema ({reason}) — 降级载荷"
                "必须是合法的友好 text 视图，不能把脏载荷换个信封继续下发.",
            )
            self.assertEqual(payload.get("view"), "text", "GRF-A3 RED: downgrade must be a friendly text view.")
            self.assertTrue(
                ((payload.get("data") or {}).get("text") or "").strip(),
                "GRF-A3 RED: downgraded friendly hint text is empty (不静默要求非空提示).",
            )
            self.assertNotEqual(
                (payload.get("data") or {}).get("text"), "被截断的回答",
                "GRF-A3 RED: the raw malformed payload was forwarded (blind render of dirty JSON).",
            )

    def test_a3_pump_side_channel_blocks_relayed(self):
        """coverage: GRF-A3 — 引擎侧通道 kidpal_display_blocks 被 front handler 截获：blocks 进
        校验+relay 链，且每个信封以设备接受的 {"type":"mcp","payload":…} 单帧下发（display_relay
        废止）；原始侧通道帧不透传设备."""
        pump, err = _front_callable("pump_upstream_to_client")
        if pump is None:
            self.fail("GRF-A3 RED: " + err)
        mod, _ = _load_front_app()
        validate = mod.gw_display.validate_display

        upstream = _FakeUpstream([
            b"\x1a\x2b\x00opus-audio-frame",
            json.dumps({"type": SIDE_CHANNEL_TYPE,
                        "blocks": [json.dumps(_DISPLAY_PAYLOAD, ensure_ascii=False), _MALFORMED_BLOCK]},
                       ensure_ascii=False),
            json.dumps({"type": "tts", "state": "sentence_start", "text": "现在是干净的播报文本。"},
                       ensure_ascii=False),
        ])
        device = _FakeDeviceSocket()
        asyncio.run(pump(upstream, device))

        self.assertEqual(
            device.sent_bytes, [b"\x1a\x2b\x00opus-audio-frame"],
            "GRF-A3 RED: binary (audio) frames must forward unchanged.",
        )
        self.assertIn(
            json.dumps({"type": "tts", "state": "sentence_start", "text": "现在是干净的播报文本。"},
                       ensure_ascii=False),
            device.sent_text,
            "GRF-A3 RED: a clean tts frame must forward verbatim (front 是校验/relay 权威, "
            "不是文本改写者).",
        )
        frames = _parsed_outbound_text_frames(device.sent_text)
        side_channel_forwarded = any(
            isinstance(f, dict) and f.get("type") == SIDE_CHANNEL_TYPE for f in frames
        )
        self.assertFalse(
            side_channel_forwarded,
            f"GRF-A3 RED: the raw '{SIDE_CHANNEL_TYPE}' frame was forwarded verbatim to the "
            "device — handler 必须截获（引擎内部传输面，设备只见 kidpal/display tools/call）.",
        )
        deprecated = [f for f in frames if isinstance(f, dict) and f.get("type") == DEPRECATED_WRAPPER_TYPE]
        self.assertEqual(
            deprecated, [],
            f"GRF-A3 RED: the deprecated '{DEPRECATED_WRAPPER_TYPE}' wrapper frame is still on "
            "the wire — 设备 application.cc:532 丢弃一切非 mcp 类型（二轮冒烟串口实证 "
            "'Unknown message type: display_relay'），该类型废止、不得再发.",
        )
        envelopes = _extract_mcp_envelopes(frames)
        self.assertEqual(
            len(envelopes), 2,
            f"GRF-A3 RED: expected exactly 2 kidpal/display relays carried on "
            f"{{'type':'mcp','payload':…}} frames (1 valid + 1 downgraded), got {envelopes!r} — "
            "设备只认 type=mcp 帧（application.cc:492），信封必须逐帧 mcp 载荷下发.",
        )
        relayed = [e for e in envelopes if _wire_arguments_match(e["params"].get("arguments"), _DISPLAY_PAYLOAD)]
        self.assertTrue(
            relayed,
            "GRF-A3 RED: the valid side-channel block was not relayed verbatim (wire shape "
            "view str + data json-string) via kidpal/display on an mcp frame.",
        )
        downgraded = [e for e in envelopes
                      if not _wire_arguments_match(e["params"].get("arguments"), _DISPLAY_PAYLOAD)]
        for e in downgraded:
            payload = _wire_payload_of(e["params"].get("arguments"))
            ok, reason = validate(payload)
            self.assertTrue(ok, f"GRF-A3 RED: downgraded side-channel payload fails schema ({reason}).")
            self.assertTrue(((payload or {}).get("data") or {}).get("text"),
                            "GRF-A3 RED: downgraded relay carries an empty friendly hint.")
        bare = [f for f in frames if isinstance(f, dict) and f.get("method") == "tools/call"]
        self.assertEqual(
            bare, [],
            "GRF-A3 RED: a bare (non-mcp-wrapped) tools/call frame was sent — 设备会按 Unknown "
            "type 丢弃，信封只能以 {\"type\":\"mcp\",\"payload\":…} 载荷形态出站.",
        )

    def test_a3_pump_upstream_takes_over_text_stream(self):
        """coverage: GRF-A2+A3 — front 文本面纵深防御 + relay：漏网 fence 被剥离、信封以设备
        mcp 帧型逐帧下发，二进制/hello 帧原样直通（引擎侧剥离为主，此为兜底齿——两道防线任一
        失效都不得漏到设备）."""
        pump, err = _front_callable("pump_upstream_to_client")
        if pump is None:
            self.fail("GRF-A2/A3 RED: " + err)
        mod, _ = _load_front_app()
        validate = mod.gw_display.validate_display

        upstream = _FakeUpstream([
            b"\x1a\x2b\x00opus-audio-frame",  # binary audio: verbatim passthrough
            json.dumps({"type": "hello", "version": 1, "features": {"mcp": True}}),
            json.dumps({"type": "tts", "state": "sentence_start", "text": _LLM_TEXT_WITH_FENCE},
                       ensure_ascii=False),
            json.dumps({"type": "tts", "state": "sentence_start",
                        "text": "看这里。\n```display\n" + _MALFORMED_BLOCK + "\n```\n"},
                       ensure_ascii=False),
        ])
        device = _FakeDeviceSocket()
        asyncio.run(pump(upstream, device))

        self.assertEqual(
            device.sent_bytes, [b"\x1a\x2b\x00opus-audio-frame"],
            "GRF-A2 RED: binary (audio) frames must forward to the device unchanged — takeover "
            "is text-face only.",
        )
        self.assertIn(
            json.dumps({"type": "hello", "version": 1, "features": {"mcp": True}}),
            device.sent_text,
            "GRF-A2 RED: text frames WITHOUT a text field (hello) must forward verbatim.",
        )

        frames = _parsed_outbound_text_frames(device.sent_text)
        tts_frames = [f for f in frames if isinstance(f, dict) and f.get("type") == "tts"]
        self.assertTrue(
            tts_frames,
            "GRF-A2 RED: the tts 文字同步 frame never reached the device through the proxy path.",
        )
        cleaned = "\n".join(f.get("text") or "" for f in tts_frames)
        self.assertNotIn(
            "```", cleaned,
            f"GRF-A2 RED: the proxy path forwarded fence residue to the device: {cleaned[:200]!r} "
            "— live session 95ccf407 缺陷本体（```display 混入播报）.",
        )
        self.assertIn(_SP_BEFORE, cleaned, "GRF-A2 RED: non-fence speech lost in the proxy path.")
        self.assertNotIn("月亮盈亏小知识", cleaned, "GRF-A2 RED: display JSON body reached the device speech face.")
        self.assertNotIn("被截断的回答", cleaned, "GRF-A2 RED: malformed fence body reached the device speech face.")

        deprecated = [f for f in frames if isinstance(f, dict) and f.get("type") == DEPRECATED_WRAPPER_TYPE]
        self.assertEqual(
            deprecated, [],
            f"GRF-A3 RED: the deprecated '{DEPRECATED_WRAPPER_TYPE}' wrapper frame is still on "
            "the wire — 设备 application.cc:532 丢弃一切非 mcp 类型，该类型废止.",
        )
        envelopes = _extract_mcp_envelopes(frames)
        self.assertEqual(
            len(envelopes), 2,
            f"GRF-A3 RED: expected exactly 2 kidpal/display relays carried on "
            f"{{'type':'mcp','payload':…}} frames (1 valid + 1 downgraded), got {envelopes!r} — "
            "代理路径必须把剥离出的 display JSON 以设备接受的 mcp 帧型实际下发.",
        )
        relayed = [e for e in envelopes if _wire_arguments_match(e["params"].get("arguments"), _DISPLAY_PAYLOAD)]
        self.assertTrue(
            relayed,
            "GRF-A3 RED: the valid display payload was not relayed verbatim (wire shape) via "
            "kidpal/display on an mcp frame.",
        )
        downgraded = [e for e in envelopes
                      if not _wire_arguments_match(e["params"].get("arguments"), _DISPLAY_PAYLOAD)]
        for e in downgraded:
            payload = _wire_payload_of(e["params"].get("arguments"))
            ok, reason = validate(payload)
            self.assertTrue(ok, f"GRF-A3 RED: downgraded relay payload fails schema ({reason}).")
            self.assertTrue(((payload or {}).get("data") or {}).get("text"),
                            "GRF-A3 RED: downgraded relay carries an empty friendly hint.")
        bare = [f for f in frames if isinstance(f, dict) and f.get("method") == "tools/call"]
        self.assertEqual(
            bare, [],
            "GRF-A3 RED: a bare (non-mcp-wrapped) tools/call frame was sent — 信封只能以 "
            "{\"type\":\"mcp\",\"payload\":…} 载荷形态出站.",
        )

    # --- GRF-A4 -------------------------------------------------------------
    def test_a4_device_receipt_relay_ack_consumption(self):
        """coverage: GRF-A4 — 设备渲染回执 {"type":"mcp","payload":…} 帧解包后经 relay_ack 消费为
        送达确认；代理转发原样（front 不重发）；relay_ack 本体语义不动."""
        on_receipt, err = _front_callable("on_device_receipt")
        if on_receipt is None:
            self.fail("GRF-A4 RED: " + err)
        mod, _ = _load_front_app()
        frozen_relay_ack = mod.gw_mcp.relay_ack  # 决策23 语义的唯一来源——front 必须委托，不自造

        # (1) the wrapped device receipt frame (the real wire form) must UNWRAP to relay_ack.
        self.assertEqual(
            on_receipt(_ACK_RECEIPT_WRAPPED), (True, _ACK_RECEIPT["id"]),
            f"GRF-A4 RED: on_device_receipt({_ACK_RECEIPT_WRAPPED!r}) did not unwrap the mcp "
            "frame and recognise the render receipt as a delivery ack — 设备回执以 "
            '{"type":"mcp","payload":…} 到达，front 必须先解 payload 再进 relay_ack.',
        )
        self.assertEqual(
            on_receipt(json.dumps(_ACK_RECEIPT_WRAPPED, ensure_ascii=False)), (True, _ACK_RECEIPT["id"]),
            "GRF-A4 RED: on_device_receipt must also accept the JSON-string form of the wrapped "
            "receipt (ws receive path yields strings).",
        )
        # (2) semantics still DELEGATE to relay_ack for every other shape (no divergence).
        for sample in (_ACK_RECEIPT,
                       "not-a-dict",
                       {"jsonrpc": "2.0", "id": 7, "result": {"route": "text"}},
                       {"jsonrpc": "2.0", "id": 9, "error": {"code": -1, "message": "no"}}):
            self.assertEqual(
                on_receipt(sample), frozen_relay_ack(sample),
                f"GRF-A4 RED: on_device_receipt({sample!r}) diverges from mcp_relay.relay_ack — "
                "front 必须复用冻结的 relay_ack 语义（渲染即确认、即出队），不得自造分叉实现.",
            )
        self.assertEqual(
            on_receipt(_ACK_RECEIPT), (True, _ACK_RECEIPT["id"]),
            "GRF-A4 RED: a bare {ack:true} device receipt must still be recognised as a "
            "delivery ack with its id (送达确认闭环).",
        )

        # (3) proxy-path wiring: wrapped receipts flow device->engine UNCHANGED, consumed as acks.
        pump, err = _front_callable("pump_client_to_upstream")
        if pump is None:
            self.fail("GRF-A4 RED: " + err)
        device = _FakeDeviceSocket(
            incoming=[
                {"type": "websocket.receive", "text": json.dumps({"type": "abort", "session_id": "s1"})},
                {"type": "websocket.receive", "text": json.dumps(_ACK_RECEIPT_WRAPPED, ensure_ascii=False)},
            ],
            disconnect_exc=mod.WebSocketDisconnect,
        )
        upstream = _FakeUpstream()
        acked = asyncio.run(pump(device, upstream))
        forwarded = "\n".join(str(m) for m in upstream.sent)
        self.assertIn(
            json.dumps(_ACK_RECEIPT_WRAPPED, ensure_ascii=False), forwarded,
            "GRF-A4 RED: the device receipt was not forwarded to the engine unchanged — 代理语义"
            "保持（front 只消费回执，不截流、不重发）.",
        )
        self.assertIn(
            json.dumps({"type": "abort", "session_id": "s1"}), forwarded,
            "GRF-A4 RED: a regular device frame (abort) was not forwarded unchanged.",
        )
        self.assertIsInstance(
            acked, (list, tuple),
            "GRF-A4 RED: pump_client_to_upstream must return the consumed ack receipt ids.",
        )
        self.assertIn(
            str(_ACK_RECEIPT["id"]), [str(x) for x in acked],
            f"GRF-A4 RED: receipt id {_ACK_RECEIPT['id']} was not consumed as a delivery ack via "
            f"relay_ack (acked={acked!r}) — 回执闭环 (决策23) 未落到代理路径.",
        )

    # --- GRF-E1 -------------------------------------------------------------
    def test_e1_existing_directionb_acceptance_structural_green(self):
        """coverage: GRF-E1 — G 包既有验收结构面不回归（独立 runner 复跑，exit 0 = 全绿）."""
        target = SERVER / "acceptance" / "test_gateway_directionB.acceptance.py"
        proc = subprocess.run(
            [sys.executable, str(target)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=600, cwd=str(SERVER),
        )
        self.assertEqual(
            proc.returncode, 0,
            "GRF-E1 RED: the existing 260903-gateway-directionB acceptance regressed under the "
            f"relay fix.\nstdout tail: {proc.stdout[-1500:]!r}\nstderr tail: {proc.stderr[-800:]!r}",
        )


# --------------------------------------------------------------------------- #
# DF-A1 / DF-A2 — M1「围栏闭合即发」additive 时序齿（260911-display-first-m1）  #
# --------------------------------------------------------------------------- #

class DisplayFirstM1Acceptance(unittest.TestCase):
    """M1 (display-first) additive timing teeth: the engine fence state machine emits
    a display block AT its closure inside feed() — not deferred to finish() — via the
    new on_block_closed callback.  Primary evidence = behavioural exercise of the pure
    module (real payloads, order, call count, speech-face cleanliness); the engine
    deploy-copy wiring tooth is auxiliary/structural (same precedent as the GRF-A2
    carrier tooth — connection.py cannot be import-tested offline)."""

    maxDiff = None

    @staticmethod
    def _sm_class():
        mod, err = _load_engine_strip_module()
        if mod is None:
            raise AssertionError("DF-A1 RED: " + err)
        sm_cls = getattr(mod, "DisplayFenceStateMachine", None)
        if not callable(sm_cls):
            raise AssertionError("DF-A1 RED: kidpal_display_strip.DisplayFenceStateMachine missing.")
        return sm_cls

    @staticmethod
    def _accepts_callback(sm_cls):
        try:
            sig = inspect.signature(sm_cls.__init__)
        except (TypeError, ValueError):
            return False
        return "on_block_closed" in sig.parameters

    @staticmethod
    def _make_sm(sm_cls, on_block_closed):
        """Construct the machine with the closure callback; return (sm|None, err)."""
        try:
            return sm_cls(on_block_closed=on_block_closed), None
        except TypeError as e:
            return None, (f"DisplayFenceStateMachine(on_block_closed=...) not accepted: {e!r}. "
                          "DF-A1 要求 feed() 内块级闭合即发射的闭合回调 API（构造注入 "
                          "on_block_closed(block)，additive）——该行为现状不存在（M1 未实现）。")

    def test_m1_closure_callback_api_present(self):
        """coverage: DF-A1 — DisplayFenceStateMachine 暴露 on_block_closed 闭合回调构造参数."""
        sm_cls = self._sm_class()
        self.assertTrue(
            self._accepts_callback(sm_cls),
            "DF-A1 RED: DisplayFenceStateMachine.__init__ 不接受 on_block_closed 参数——"
            "块闭合回调 API 未实现（M1「feed() 内块级闭合即发射」的前提，additive 语义边界见"
            "本文件 REVISION LOG [2]）。",
        )

    def test_m1_feed_emits_closed_block_immediately(self):
        """coverage: DF-A1 — fence 跨 delta 闭合瞬间（closer ``` 到达的那个 feed()），同步
        触发回调恰一次、载荷裸化（无 ``` 标记/无 tag），播报面照旧零 fence 残留."""
        sm_cls = self._sm_class()
        fired = []
        sm, err = self._make_sm(sm_cls, fired.append)
        if sm is None:
            self.fail("DF-A1 RED: " + err)

        speech_parts = []
        for delta in _SM_DELTAS_CLOSED:
            speech_parts.append(sm.feed(delta))
            if delta == "```":
                self.assertEqual(
                    len(fired), 1,
                    "DF-A1 RED: 闭合回调未在 closer ``` 到达的那次 feed() 内同步触发——"
                    "块闭合仍被推迟到 finish()（M1 早发缺失）。",
                )
        tail, blocks = sm.finish()
        speech = "".join(speech_parts) + tail

        self.assertEqual(speech, _SM_EXPECTED_SPEECH_CLOSED,
                         "DF-A1 RED: 加回调后播报面被改动（语义放松）。")
        self.assertNotIn("```", speech, "DF-A1 RED: fence 残留混入播报。")
        self.assertNotIn("卡片内容", speech, "DF-A1 RED: fence body 泄进播报。")
        self.assertEqual(len(fired), 1,
                         f"DF-A1 RED: 期望恰 1 次闭合回调，得 {len(fired)} 次: {fired!r}.")
        payload = fired[0]
        self.assertNotIn("```", payload, "DF-A1 RED: 回调载荷仍带 ``` 标记（未裸化）。")
        self.assertEqual(json.loads(payload), {"view": "text", "data": {"text": "卡片内容"}},
                         "DF-A1 RED: 回调载荷非裸载荷——须与 finish().blocks 同形、可 json.loads.")
        self.assertEqual(fired, blocks,
                         "DF-A1 RED: 回调序列 != finish().blocks（发射信道与观察镜像漂移，"
                         "闭块被双发或乱序）。")

    def test_m1_multiple_blocks_emitted_in_order(self):
        """coverage: DF-A1 — 两个闭合 fence 依文本次序各触发一次回调，载荷逐块对应、裸化."""
        sm_cls = self._sm_class()
        fired = []
        sm, err = self._make_sm(sm_cls, fired.append)
        if sm is None:
            self.fail("DF-A1 RED: " + err)
        for delta in (_SM_DELTAS_CLOSED[:-1]
                      + ["再见啦。第二段。```display\n" + json.dumps(_PAYLOAD_2, ensure_ascii=False)
                         + "\n```\n完啦。"]):
            sm.feed(delta)
        _tail, blocks = sm.finish()
        self.assertEqual(len(fired), 2,
                         f"DF-A1 RED: 两个闭合块应触发 2 次回调，得 {len(fired)} 次: {fired!r}.")
        self.assertNotIn("```", fired[0], "DF-A1 RED: 第一块回调载荷未裸化（带 ``` 标记）.")
        self.assertNotIn("```", fired[1], "DF-A1 RED: 第二块回调载荷未裸化（带 ``` 标记）.")
        self.assertEqual(json.loads(fired[0]), {"view": "text", "data": {"text": "卡片内容"}},
                         "DF-A1 RED: 第一块回调载荷错乱.")
        self.assertEqual(json.loads(fired[1]), _PAYLOAD_2,
                         "DF-A1 RED: 第二块回调载荷错乱.")
        self.assertEqual(fired, blocks,
                         "DF-A1 RED: 多块回调序列与 finish().blocks 不同序（顺序语义破坏）.")

    def test_m1_unclosed_tail_emitted_exactly_once_at_finish(self):
        """coverage: DF-A1 — 流末 unclosed fence 在 feed() 期间绝不发射，finish() 内恰发射一次，
        裸 body 照旧宁静默不念、照旧进 finish().blocks."""
        sm_cls = self._sm_class()
        fired = []
        sm, err = self._make_sm(sm_cls, fired.append)
        if sm is None:
            self.fail("DF-A1 RED: " + err)
        speech_parts = []
        for delta in _SM_DELTAS_UNCLOSED:
            speech_parts.append(sm.feed(delta))
            self.assertEqual(fired, [],
                             "DF-A1 RED: unclosed fence 在 feed() 期间被提前发射——闭合前不得发射。")
        tail, blocks = sm.finish()
        speech = "".join(speech_parts) + tail
        self.assertIn("开始讲。", speech, "DF-A1 RED: fence 前正文丢失.")
        self.assertNotIn("```", speech, "DF-A1 RED: unclosed fence 泄进播报.")
        self.assertNotIn("没讲完的卡", speech, "DF-A1 RED: unclosed body 泄进播报.")
        self.assertEqual(len(fired), 1,
                         f"DF-A1 RED: unclosed tail 应在 finish() 内恰触发一次回调, 得 "
                         f"{len(fired)} 次: {fired!r}.")
        self.assertIn("没讲完的卡", fired[0], "DF-A1 RED: unclosed 裸 body 未进回调载荷.")
        self.assertIn("片内容", fired[0], "DF-A1 RED: unclosed 裸 body 不完整（被截断发射）.")
        self.assertNotIn("```", fired[0], "DF-A1 RED: unclosed 回调载荷仍带 ``` 标记.")
        self.assertEqual(fired, blocks,
                         "DF-A1 RED: unclosed 回调 != finish().blocks（观察镜像漂移）.")

    def test_m1_finish_semantics_preserved_with_callback(self):
        """coverage: DF-A2 — 加回调后 finish() 语义不放松：仍返回 (tail_speech, blocks)、
        blocks 按序承载全部裸载荷（闭合与未闭合同列、无 ``` 标记），与 M1 前（无回调）同形."""
        sm_cls = self._sm_class()
        fired = []
        sm, err = self._make_sm(sm_cls, fired.append)
        if sm is None:
            self.fail("DF-A2 RED: " + err)
        for delta in (_SM_DELTAS_CLOSED[:-1]
                      + ["再见啦。第二段。```display\n" + json.dumps(_PAYLOAD_2, ensure_ascii=False)
                         + "\n```\n完啦。",
                         "结尾有个没收尾的。```display\n" + '{"view":"text","data":{"text":"没讲完',
                         "的尾巴"]):
            sm.feed(delta)
        tail, blocks = sm.finish()
        self.assertEqual(len(blocks), 3,
                         f"DF-A2 RED: finish().blocks 应为 3 条（闭合2 + 未闭合1），得 {blocks!r}.")
        for b in blocks:
            self.assertNotIn("```", b, "DF-A2 RED: blocks 出现 ``` 标记（裸载荷语义放松）.")
        self.assertEqual(json.loads(blocks[0]), {"view": "text", "data": {"text": "卡片内容"}},
                         "DF-A2 RED: blocks[0] 载荷错乱.")
        self.assertEqual(json.loads(blocks[1]), _PAYLOAD_2, "DF-A2 RED: blocks[1] 载荷错乱.")
        self.assertIn("没讲完", blocks[2], "DF-A2 RED: 未闭合尾块未进 finish().blocks.")
        self.assertIn("尾巴", blocks[2], "DF-A2 RED: 未闭合尾块不完整.")
        self.assertEqual(fired, blocks,
                         "DF-A2 RED: 回调序列 != finish().blocks——发射/观察信道不一致"
                         "（闭合块被双发或只发一遍的语义被改）。")

    def test_m1_connection_wires_closure_callback_into_stream(self):
        """coverage: DF-A1 — 引擎部署副本 connection.py 把闭合回调接线进流循环（结构面辅助齿，
        行为主齿在上；connection.py 自带引擎依赖不可离线 import，同 GRF-A2 carrier 先例）."""
        src = _read_text(ENGINE_PATCH_CONNECTION)
        self.assertIn(
            "on_block_closed", src,
            "DF-A1 RED: connection.py 未引用 on_block_closed 闭合回调——feed() 内块闭合即发射"
            "的引擎接线缺失（builder 须把闭合回调接到侧通道 kidpal_display_blocks 发射，使闭合"
            "块不等 finish() 即发）。",
        )


# --------------------------------------------------------------------------- #
# GRF-A5 live face — 真机一轮会话的日志证据断言（不 auto-run，ARQ-260901-g3）      #
# --------------------------------------------------------------------------- #

class GRelayFixLiveAcceptance(unittest.TestCase):
    """live evidence teeth: read the real-device smoke evidence under
    changes/dev/260907-g-relay-fix/ (GRF-E2).  Evidence form = what the stack naturally
    persists (设备串口摘录 / 引擎 docker logs 摘录 / 证据 md)，NOT wire literals
    (permessage-deflate makes wire capture the wrong form — kidpal-live-smoke-debug 口径).
    """

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _evidence_corpus(self):
        if not LIVE_EVIDENCE_DIR.is_dir():
            return None
        parts = []
        for p in sorted(LIVE_EVIDENCE_DIR.rglob("*")):
            if p.is_file() and p.suffix == ".md":
                try:
                    parts.append(p.read_text(encoding="utf-8", errors="replace"))
                except OSError:
                    continue
        return "\n".join(parts) if parts else None

    def _require_evidence(self):
        corpus = self._evidence_corpus()
        self.assertTrue(
            corpus,
            "GRF-A5/E2 RED: no live evidence 落盘 under changes/dev/260907-g-relay-fix/ — "
            "真机一轮冒烟（修复后）的串口/引擎日志证据必须先落盘（GRF-E2），四事实齿据此核销.",
        )
        return corpus

    # --- GRF-A5 fact ⓪ --------------------------------------------------------
    def test_a5_live_engine_patch_marker_in_log(self):
        """coverage: GRF-A5⓪ — 引擎启动日志含补丁标记行（补丁真在跑，防陈旧容器假绿）."""
        corpus = self._require_evidence()
        self.assertIn(
            ENGINE_PATCH_MARKER, corpus,
            f"GRF-A5 live RED: evidence does not carry the engine startup marker line "
            f"'{ENGINE_PATCH_MARKER}' — 补丁是否真在跑（非陈旧容器）必须由引擎日志标记行证明.",
        )

    # --- GRF-A5 fact ① --------------------------------------------------------
    def test_a5_live_fence_not_in_broadcast(self):
        """coverage: GRF-A5① — 真机会话：fence 清零，播报面 + 屏幕面双面（session id + 双面清零声明）."""
        corpus = self._require_evidence()
        session = re.search(r"[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}", corpus)
        self.assertTrue(
            session,
            "GRF-A5 live RED: evidence does not cite the smoke session id (uuid) — 证据须可对回"
            "具体一轮真机会话.",
        )
        lower = corpus.lower()
        self.assertIn(
            "fence", lower,
            "GRF-A5 live RED: evidence never mentions fence 残留核验 — 必须显式记录清零事实.",
        )
        clean_markers = ("零", "无 fence", "无fence", "不再", "未混入", "不混入", "清零", "clean")
        self.assertTrue(
            any(m in corpus for m in clean_markers),
            "GRF-A5 live RED: evidence mentions fence but records no fence-CLEAN outcome "
            "(零/无/不再混入) — GRF-A5① 是「清零」断言，不是「提到」断言.",
        )
        self.assertIn(
            "播报", corpus,
            "GRF-A5 live RED: evidence does not record the 播报面 (TTS/文字同步) fence 核验 — "
            "清零断言必须覆盖播报面.",
        )
        self.assertTrue(
            ("屏幕" in corpus or "显示" in corpus),
            "GRF-A5 live RED: evidence does not record the 屏幕面 (设备渲染) fence 核验 — "
            "清零断言必须覆盖屏幕渲染面（双面口径）.",
        )
        failure_markers = ("仍混入", "仍可见", "仍然混入", "still mixed", "残留仍在")
        for marker in failure_markers:
            self.assertNotIn(
                marker, corpus,
                f"GRF-A5 live RED: evidence records a FAILURE outcome ({marker!r}) — fence 字样"
                "仍混入设备播报/屏幕文本，修复未生效。核验结论与缺陷背景引文须分段、显式标注清零声明.",
            )

    # --- GRF-A5 fact ② --------------------------------------------------------
    def test_a5_live_kidpal_display_tools_call_dispatched(self):
        """coverage: GRF-A5② — 真机会话：kidpal/display tools/call 实际下发且设备侧 McpServer
        接受（无 Unknown type / Invalid id 拒收——display_relay 断点证消）."""
        corpus = self._require_evidence()
        self.assertIn(
            "kidpal/display", corpus,
            "GRF-A5 live RED: evidence never records a kidpal/display tool dispatch — GRF-A5② "
            "要求 relay 实际下发的事实摘录（引擎/front 日志或设备 MCP 到达日志）.",
        )
        self.assertTrue(
            ("tools/call" in corpus or "tool_call" in corpus or "tool call" in corpus.lower()),
            "GRF-A5 live RED: evidence records kidpal/display but not the JSON-RPC tools/call "
            "dispatch shape — 须摘录下发载体事实.",
        )
        rejection_markers = ("Unknown message type", "Invalid id")
        for marker in rejection_markers:
            self.assertNotIn(
                marker, corpus,
                f"GRF-A5 live RED: evidence records a device-side rejection ({marker!r}) — relay "
                "帧仍被固件拒收（mcp 帧型/数字 id/data 字符串化 未按 wire 事实落地）.",
            )

    # --- GRF-A5 fact ③ --------------------------------------------------------
    def test_a5_live_display_text_view_triggered(self):
        """coverage: GRF-A5③ — 真机会话：display text 视图触发（翻屏语义生效，「上一页看不到」根因消除）."""
        corpus = self._require_evidence()
        view_markers = ("text 视图", "text视图", "view=text", "view: text", '"view": "text"', "text 视图面")
        self.assertTrue(
            any(m in corpus for m in view_markers),
            "GRF-A5 live RED: evidence does not record the display TEXT view being triggered — "
            "GRF-A5③ 要求设备侧 text 视图实际渲染的事实.",
        )
        page_markers = ("翻屏", "翻页", "翻回", "上一页", "P3", "分页")
        self.assertTrue(
            any(m in corpus for m in page_markers),
            "GRF-A5 live RED: evidence does not record the 翻屏/paging semantics — 用户「上一页看不到」"
            "根因消除必须由 text 视图 + 翻屏事实共同证明.",
        )


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(GRelayFixStructuralAcceptance)
    suite.addTests(loader.loadTestsFromTestCase(DisplayFirstM1Acceptance))
    if os.environ.get("GRF_RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(GRelayFixLiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    sys.exit(0 if result.wasSuccessful() else 1)
