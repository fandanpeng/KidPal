"""Frozen acceptance tests — TaskPackage 261022-p3-record-front-polish-fix（server 面）。

承载 debt TECH-261022-p3-record-session-state-churn 缺陷③④⑤（KPLOG005 真机轮三源
取证：全天四笔「说完了」笔笔迟到 5~14s 成孤儿帧 / 引导独播 28s + 末段引导与
start_organizing 同毫秒 / 「Okay」幻听直进素材缓冲 / 同日随机 id 排序 + 敷衍标题
「我的故事」）+ 冻结 plan delivery/taskpackages/261022-p3-record-front-polish-fix/
plan.md（验收方向 1①②④⑤a⑤b；②单包 Lite，riskTier=medium，sourceType=regression）。

Scope of record（taskpackage acceptance FP-A1..A7；本文件承载 FP-A1..A5）:
  FP-A1 应答窗收尾通路（缺陷③）：末段引导播报结束与 start_organizing 之间存在应答窗
        （时长单源常量 + 注入时钟快进）；应答窗内「说完了」stt 经 is_finish_phrase 走
        正常收尾整理归档（不成孤儿帧）；现状「末段引导后 STUCK_GUIDE_SECONDS 即收会话」
        形态不再出现。AC5「2 轮无回应自动收尾」轮数语义零触碰（负锚由 RDG-A2 既有齿
        must-stay-green 承载）；is_finish_phrase 词表零触碰（v1.66 需求面已冻）。
  FP-A2 引导篇幅上限（缺陷③）：单段引导播报篇幅超上限被截断/抑制（上限 = 单源常量、
        数值不冻结）；上限内引导原样保真（防上限过紧误伤正常引导）。
  FP-A3 引导窗口素材过滤（缺陷④）：引导播报窗口内到达的 stt 不进素材缓冲；窗口过期
        与从未播报引导的会话正常进素材（反向 case 必载——防过滤窗过宽误吞孩子接话）。
  FP-A4 列表同日 seq tiebreak（缺陷⑤a）：同日多篇按 seq 倒序（新篇在上）；缺 seq 回退
        id 倒序不崩且账本纯读；跨日日期倒序不变；items 字段形状零偏移（零 wire 变更，
        crossEnd 豁免预核面）。
  FP-A5 标题敷衍兜底（缺陷⑤b）：整理输出标题命中敷衍启发式（单源可测）时兜底替换；
        正常标题原样保留。faithful-organize.v1.md 制品语义零触碰（不做清单）。
  FP-A6 回归网 = 套件级 suiteGate（test_record_dialogue_guide / test_record_cancel_
        lifecycle / test_m4k1_story_cloud_link 三族全绿 + server 全量零新增红 vs 基线
        238 passed 锚 64d54eba）——不入本文件，由 verifier 套件面承载。
  FP-A7 live-manual 真机目检（structural 口径，CG-A5 先例同款；不计入 runner 通过面）：
        human 持板——① 引导不再独播垄断（篇幅受限、末段引导后有应答窗）；② 说「说完了」
        正常收尾归档（不再孤儿帧）；③ 新篇在列表最上且标题可识别；④ 录音回显区无幻听
        句（GAP-1 真机半）。前置 = front 重部署 ECS；Pod 到真机门前停等 human
        （status=awaiting-human:real-device），不自行部署。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) —

  A) server/gateway/front/front_app.py 模块级单源常量（名字冻结、数值不冻结——
     手感参数归真机轮终判，数值不进 spec 文本，FP-X2）:

    GUIDE_RESPONSE_WINDOW_SECONDS   应答窗时长（正数；int/float 非 bool）。
        语义：末段引导轮发出后，自动收尾（start_organizing 下发）不得早于「旧触发点」
        = 该轮发出时刻 + STUCK_GUIDE_SECONDS；实现形态不钉（延后计时起点 / 阈值叠加
        窗口等均可），以注入时钟行为断言承载。**经 front 模块全局单源读取**（验收
        monkeypatch 该常量做确定性快进；内联硬编码 = 单源被破，验收红）。
    GUIDE_SPEECH_MAX_CHARS          单段引导播报篇幅上限（正 int，字符数）。
        _story_guide_round 有文本产出时，kidpal_speak 注入文本长度 ≤ 上限（超限截断
        或整体抑制，二义由实现裁；截断产物须为原文前缀、不得另编内容）。**经 front
        模块全局单源读取**（同上，monkeypatch 注入）。
    GUIDE_STT_GUARD_SECONDS         引导播报窗口 STT 过滤时长（正数）。
        语义：该会话引导播报注入沿起 GUIDE_STT_GUARD_SECONDS 内（以调用方注入 now
        判定），_story_feed_from_stt 不把该 stt 喂进素材缓冲。**经 front 模块全局
        单源读取**（同上）。

  B) _story_feed_from_stt(session_id, obj, now=None)（additive keyword-only 形参 now；
     缺省 None = 墙钟，既有调用点零偏移——RDG-A5 self_push keyword-only 先例同缝）:
        引导播报窗口内（窗口锚 = 引导播报注入时刻；**时钟注入纪律：窗口锚取 sweep
        注入 now、过期判定消费调用方注入 now，不落墙钟——否则离线快进不可测**）到达
        的 stt：不 feed 进累积缓冲（素材面丢弃，返回 False、不改累积缓冲）；窗口外 /
        从未播报引导的会话：既有语义零偏移（accumulate 照常 feed、pending_stop 照常
        接通收尾、chat 不入库）。
        **过滤窗只滤素材面**：收尾词判定面（_story_finish_on_phrase -> is_finish_phrase）
        不在过滤窗内——应答窗/播报窗内孩子真说了「说完了」收尾沿照常可达（FP-A1 的
        本体；过滤窗误吞收尾沿 = 验收红）。

  C) 应答窗收尾通路（缺陷③行为钉）:
        sweep 注入时钟驱动下，末段引导轮（guide_rounds 达 GUIDE_ROUND_LIMIT=2 的那一
        轮）发出后：旧触发点（+STUCK_GUIDE_SECONDS）不自动收尾、不下发 start_organizing；
        应答窗内到达的「说完了」stt 经 is_finish_phrase（词表零触碰）走三式收尾单源
        _story_finish_session（start_organizing 恰一帧 + 出语境信号 active=false +
        忠实整理归档）；应答窗耗尽后自动收尾照常（恰一帧、重复幂等——既有 RDG-A6 语义
        零偏移）。GUIDE_ROUND_LIMIT=2 轮数语义零触碰（RDG-A2 既有齿 must-stay-green）。

  D) server/gateway/story.py list_stories 同日 tiebreak（缺陷⑤a）:
        同日多篇按 seq 倒序（seq = .seq.json 账本 by_id；list 对账本**纯读**——不重建、
        不落盘，fetch_story 先例；「front 单写者」纪律不受 list 扰动）；缺 seq 记录
        回退 id 倒序、绝不崩；跨日仍按完整日期倒序（契约 §1.3③ 只钉日期倒序，tiebreak
        是实现自由度）；items 元素字段恰为 {id,title,date}、total 语义零偏移（零 wire
        形状变更——设备端按 items 到达序消费，固件全树无重排消费已预核）。

  E) 标题敷衍兜底（缺陷⑤b，front 侧启发式单源）:
        is_generic_story_title(title) -> bool 纯函数 + GENERIC_STORY_TITLES 模块级
        常量（非空、元素全 str；is_generic_story_title 以它为单源——临时换表判定随动，
        RDG-A1 STUCK_PHRASES 词表先例同款）。_organize_and_archive 标题命中启发式 ->
        兜底替换/后缀化（替换形态实现自由，须非空、≠敷衍原词、≤12 字）；未命中 ->
        原样保留（≤12 字既有截断零偏移）。faithful-organize.v1.md 制品语义零触碰
        （不做清单：不重写整理 LLM prompt 语义）。

Shape authority = docs/debt/tech/TECH-261022-p3-record-session-state-churn.md（缺陷③④⑤
机制事实三源取证）+ delivery/taskpackages/261022-p3-record-front-polish-fix/plan.md
（验收方向 + 不做清单）+ 既有冻结件 test_record_dialogue_guide.acceptance.py（FROZEN
OFFLINE CONTRACT A–D 节——本包零偏移消费其 StoryAccumulator / sweep / 三式收尾单源 /
kidpal_speak 通道语义，零新增帧类型）。

跨端 wire 形状锚（crossEnd 豁免预核面，负锚守住）:
  story_list items 字段形状零 diff（FP-A4 断言 item keys 恰 {id,title,date}）；tiebreak
  只变同日到达序；start_organizing / kidpal_speak / kidpal_record_context 帧形状零 diff
  （既有 RDG 齿 must-stay-green + 本文件 FP-A1 断言沿用既有帧形状解析）；本包零新增
  wire 类型、零契约 §1.3③ / §1.4 文本触碰。

契约条目 ↔ 验收断言强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:

| 来源条目 | 承载断言 | 性质档 |
|---|---|---|
| debt 缺陷③ 应答窗（引导节奏） | test_fp_a1_response_window_*（sweep 注入时钟真跑） | 行为级 |
| debt 缺陷③「说完了」孤儿帧 | test_fp_a1_finish_phrase_within_*（泵路径真跑 + 整理桩） | 行为级 |
| debt 缺陷③ 引导篇幅垄断 | test_fp_a2_*（引导回合真跑 + speak 帧取证） | 行为级 |
| debt 缺陷④ 素材面设防 | test_fp_a3_*（_story_feed_from_stt 真跑 + 累积取证） | 行为级 |
| debt 缺陷⑤a 同日排序 | test_fp_a4_same_day_*（archive_story 真落盘 + list 真跑） | 行为级 |
| debt 缺陷⑤a 缺 seq 回退 | test_fp_a4_missing_seq_*（真删账本 + 不崩 + 账本纯读） | 行为级 |
| debt 缺陷⑤b 标题兜底 | test_fp_a5_*（整理回合真跑 + 归档取证） | 行为级 |
| FP-X2 负锚（轮数 2 / 词表 / 契约 §1.3③ 不动） | RDG-A1/A2 既有齿 must-stay-green 引用 + 本文件零触碰 | 引用既有齿 |
| FP-A6 回归网 | 套件级 suiteGate（verifier 面，不入本文件） | 套件级 |
| FP-A7 真机目检 | 不落 offline 齿（live-manual 申报） | live-manual |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-1 「不进录音回显旁路」（设备 kidpal_story_echo_push 消费 stt wire 帧的设备半）：
        由设备侧承载，front 转发冻结（决策23：代理语义 front 只消费不重发）不可承载、
        引擎侧候选排除已裁（第三方部署树）——offline 不造假齿；真机半归 FP-A7 真机
        目检（录音回显区无幻听句）。front 侧可承载半（素材缓冲）由 FP-A3 全额承载。
  GAP-2 三参数数值（应答窗 / 篇幅上限 / 过滤窗时长）：手感参数（应答窗参考 ASR 实测
        5~14s 定初值），初值归 builder 实现、human 真机轮终判——本文件全部经
        monkeypatch 注入承载，绝不断言缺省取值（只断言正数在世 + 单源可消费）。
  GAP-3 敷衍启发式的替换文案形态（替换词 / 后缀化）：实现自由，验收只钉「非空、
        ≠敷衍原词、≤12 字、正常标题原样保留」。
  GAP-4 缺 seq 记录与有 seq 记录混排的相对序：实现自由度，不钉（只钉全缺序不崩 +
        回退 id 倒序 + 账本纯读不重建）。
  GAP-5 引导播报窗口与③应答窗共享「引导播报起止沿」状态源的内部形态：plan 建议项，
        验收以各自行为断言承载，不钉共享实现。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 「sweep 注入 now 驱动 = 真实轮询路径」（RDG-A6 既有冻结纪律延续）——应答窗 /
        引导轮计时全部以注入 now 判定，离线快进可测；窗口锚不落墙钟（B 节时钟注入
        纪律），落墙钟 = 快进不可测 = 验收红。
  EQ-2 「_openclaw_chat_once 模块全局查找 = 桩可注入缝」（RDG _organize_stub 既有
        先例）；本文件引导/整理双路桩按调用超时分派（整理回合 timeout = 60s =
        _ORGANIZE_LLM_TIMEOUT_S、引导回合缺省 10s——双常数均被既有冻结面钉住）。
  EQ-3 「LLM 不可达时快失败」（RDG EQ-4 既有环境假设）：不打桩路径离线连接拒绝级
        失败，sweep 不挂起。
  EQ-4 「list_stories 真 tempdir 落盘 = 生产 stories 目录读序」（story.py producer
        纯模块先例）——seq 账本由 archive_story 真实分配，非 fixture 伪造。

Coverage map (acceptanceRef -> test):
  FP-A1 -> test_fp_a1_response_window_delays_auto_finish_after_final_guide_round
        -> test_fp_a1_finish_phrase_within_response_window_archives_normally
  FP-A2 -> test_fp_a2_guide_speech_cap_truncates_overlong_and_preserves_within_limit
  FP-A3 -> test_fp_a3_guide_broadcast_window_drops_stt_and_expires_to_normal_feed
  FP-A4 -> test_fp_a4_same_day_seq_tiebreak_desc_and_cross_day_unchanged
        -> test_fp_a4_missing_seq_falls_back_id_desc_without_crash
  FP-A5 -> test_fp_a5_generic_title_heuristic_single_source
        -> test_fp_a5_organize_generic_title_fallback_and_normal_preserved
  FP-A6 -> 套件级 suiteGate（不入本文件）
  FP-A7 -> live-manual（不入 runner 通过面）

Run protocol（offline；-m "not live" 可跑；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_record_front_polish.acceptance.py

builder FIRST ACTION 注记：本件落位 server/acceptance/ 即被全量套件自动收集（pytest
无登记机制——与固件 CMakeLists HOST_ACCEPTANCE_SRCS 登记先例不同源）。冻结态 = 设计态
红（红基线通道 B verbatim 见 taskpackage redBaselineResult 回填）；FROZEN OFFLINE
CONTRACT 落地后全量套件 = 238 基线 + 本件 8 齿全绿（FP-X1 判据：零新增红 vs 既有齿）。
本件对 builder/fixer 只读冻结；修订走 tighten-only 窗口（DEV-GOVERNANCE 验收纪律 §6）。

REVISION LOG
  2026-10-22 初版冻结（acceptance-author；FP-A1..A5 八齿；FP-A6 套件级 / FP-A7
             live-manual 如实申报不造 offline 假齿——RDG/SLP/CG 先例同款口径）。
"""

from __future__ import annotations

import contextlib
import importlib.util
import json
import os
import sys
import tempfile
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"

# 冻结 seam 名（FROZEN OFFLINE CONTRACT A/E 节；名字冻结、数值不冻结）。
GUIDE_RESPONSE_WINDOW_SECONDS = "GUIDE_RESPONSE_WINDOW_SECONDS"
GUIDE_SPEECH_MAX_CHARS = "GUIDE_SPEECH_MAX_CHARS"
GUIDE_STT_GUARD_SECONDS = "GUIDE_STT_GUARD_SECONDS"
GENERIC_TITLE_FN = "is_generic_story_title"
GENERIC_TITLE_SET = "GENERIC_STORY_TITLES"

SPEAK_TYPE = "kidpal_speak"
RECORD_CTX_TYPE = "kidpal_record_context"
START_ORGANIZING_ACTION = "start_organizing"

# FP-A2 引导篇幅桩文本（> 任意合理小上限；截断相用）。
LONG_GUIDE_REPLY = "小猫后来遇到了一只大狗，它吓了一跳，赶紧躲到了树后面去了"


# --------------------------------------------------------------------------- #
# Module loading（RDG 冻结件同款；缺失/不可装载 = RED）                          #
# --------------------------------------------------------------------------- #

_story_module_cache = None
_front_module_cache = None


def _story_module():
    """server/gateway/story.py 纯模块。"""
    global _story_module_cache
    if _story_module_cache is not None:
        return _story_module_cache
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.story as st  # noqa: PLC0415 — offline import, cached
    _story_module_cache = st
    return st


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（M4K1 / SC / RDG 冻结件同款先例）。"""
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
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)."""
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_fp", FRONT_APP)
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
        raise AssertionError("FP RED: " + err)
    return mod


def _front_num_constant(front, name):
    """(value|None) —— front 模块级数值单源常量（FROZEN CONTRACT A 节）。"""
    return getattr(front, name, None)


@contextlib.contextmanager
def _front_with_stories(stories_dir):
    """把 front 的 STORIES_DIR 指向 tempdir（隔离真实 /opt/kidpal/stories 路径）。"""
    front = _front()
    old_dir, old_acc = front.STORIES_DIR, front._story_acc
    front.STORIES_DIR = str(stories_dir)
    front._story_acc = None
    try:
        yield front
    finally:
        front.STORIES_DIR = old_dir
        front._story_acc = old_acc


@contextlib.contextmanager
def _patch_const(front, name, value):
    """临时改 front 模块级常量（单源可消费性证明缝，RDG-A1 词表先例同款）；退出还原。"""
    had = hasattr(front, name)
    old = getattr(front, name, None)
    setattr(front, name, value)
    try:
        yield
    finally:
        if had:
            setattr(front, name, old)
        else:
            delattr(front, name)


@contextlib.contextmanager
def _llm_round_stub(front, guide_reply=None, organize_reply=None):
    """引导/整理回合双路桩（_openclaw_chat_once 模块全局查找缝，EQ-2）。

    分派判据 = 调用超时：整理回合 timeout_s = 60.0（_ORGANIZE_LLM_TIMEOUT_S 既有常量）、
    引导回合缺省 10.0（_story_guide_round 不传 timeout——既有接线形态）。双常数均被
    既有冻结面钉住，桩不依赖 prompt 文案措辞（措辞归制品面，builder 可调）。"""
    calls = []

    def _fake(system_text, user_text, timeout_s=10.0):  # noqa: ANN001, ANN202
        calls.append({"system": system_text, "user": user_text, "timeout_s": timeout_s})
        if timeout_s >= 30.0:
            return organize_reply
        return guide_reply

    old = getattr(front, "_openclaw_chat_once", None)
    front._openclaw_chat_once = _fake
    try:
        yield calls
    finally:
        if old is None:
            front.__dict__.pop("_openclaw_chat_once", None)
        else:
            front._openclaw_chat_once = old


# --------------------------------------------------------------------------- #
# I/O doubles（RDG 冻结 seam 同款形态：只替代 WebSocket 两端）                   #
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


def _device_to_engine(front, session_id, incoming_texts):
    """Run the device->engine uplink pump; returns (upstream, device_socket)."""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in incoming_texts])
    up = _FakeUpstream()
    import asyncio  # noqa: PLC0415
    asyncio.run(front.pump_client_to_upstream(dev, up, session_id))
    return up, dev


def _engine_to_device(front, session_id, frames):
    """Run the engine->device downlink pump; returns (upstream, device_socket)."""
    import asyncio  # noqa: PLC0415
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, session_id))
    return up, dev


def _forwarded_objects(sent):
    out = []
    for item in sent:
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict):
            out.append(obj)
    return out


def _mcp_envelopes(sent_text):
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
    out = []
    for env in _mcp_envelopes(sent_text):
        args = (env.get("params") or {}).get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _start_organizing_frames(sent_text):
    return [p for p in _display_payloads(sent_text)
            if p.get("view") == "control"
            and isinstance(p.get("data"), dict)
            and p["data"].get("action") == START_ORGANIZING_ACTION]


def _speak_texts(sent):
    """引擎腿上 kidpal_speak 注入帧的文本列表。"""
    return [o.get("text") for o in _forwarded_objects(sent)
            if o.get("type") == SPEAK_TYPE and isinstance(o.get("text"), str)]


def _record_ctx_frames(sent):
    return [o for o in _forwarded_objects(sent) if o.get("type") == RECORD_CTX_TYPE]


_CTX = json.dumps({"type": "kidpal_session_ctx", "source_page": "story"},
                  ensure_ascii=False)


@contextlib.contextmanager
def _alert_legs(front, device_ws, engine_ws):
    """临时登记 sweep 推送腿（_register_alert_legs 既有缝），退出时清理。"""
    old = list(getattr(front, "_alert_legs", []))
    front._register_alert_legs(device_ws, engine_ws)
    try:
        yield
    finally:
        front._alert_legs[:] = old


def _t(sec):
    """fixture 基准时刻 + sec 秒（时钟注入纪律，全文件统一锚）。"""
    base = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
    return base + timedelta(seconds=sec)


# =========================================================================== #
# FP-A1 — 应答窗收尾通路（debt 缺陷③）                                          #
# =========================================================================== #


class FpResponseWindowAcceptance(unittest.TestCase):
    """FP-A1: 末段引导与 start_organizing 之间存在应答窗；窗内「说完了」正常收尾。"""

    maxDiff = None

    def _check_response_window_constant(self, front):
        val = _front_num_constant(front, GUIDE_RESPONSE_WINDOW_SECONDS)
        if val is None:
            self.fail(
                f"FP-A1 RED: front_app.py 无模块级 {GUIDE_RESPONSE_WINDOW_SECONDS} —— "
                "应答窗时长单源常量未实现（FROZEN CONTRACT A 节；末段引导与 "
                "start_organizing 之间留应答窗，缺陷③）。")
        self.assertIsInstance(val, (int, float),
                              f"FP-A1 RED: {GUIDE_RESPONSE_WINDOW_SECONDS} 非数值常量。")
        self.assertNotIsInstance(val, bool,
                                 f"FP-A1 RED: {GUIDE_RESPONSE_WINDOW_SECONDS} 为布尔（类型失真）。")
        self.assertGreater(val, 0,
                           f"FP-A1 RED: {GUIDE_RESPONSE_WINDOW_SECONDS} 非正数（应答窗须为正）。")
        return val

    def _drive_final_guide_round(self, front, sid, t0_sec):
        """驱动到末段引导轮（2 轮卡壳引导，RDG-A6 sweep 先例同款驱动法；speak 桩在世）。

        返回末段引导轮发出时刻 t2（秒偏移）。"""
        import asyncio  # noqa: PLC0415
        acc = front._get_story_acc()
        _device_to_engine(front, sid, [_CTX])
        acc.feed(sid, "从前有一只小猫", now=_t(t0_sec))
        asyncio.run(front._story_timeout_sweep(now=_t(t0_sec + 600)))   # 引导轮 1
        self.assertGreaterEqual(acc.guide_rounds(sid), 1,
                                "FP-A1 RED(前置): 第一轮引导未计（sweep 引导回合接线回退，"
                                "既有 RDG-A6 齿面被破）。")
        asyncio.run(front._story_timeout_sweep(now=_t(t0_sec + 1200)))  # 引导轮 2 = 末段
        self.assertGreaterEqual(acc.guide_rounds(sid), 2,
                                "FP-A1 RED(前置): 末段引导轮未计（GUIDE_ROUND_LIMIT=2 "
                                "接线被破——轮数语义零触碰负锚亮红，须复核而非改判）。")
        return t0_sec + 1200

    # coverage: FP-A1 -- 应答窗存在性：旧触发点（末段引导 + STUCK_GUIDE_SECONDS）不收尾、
    #                   单源常量可消费（monkeypatch 快进）、应答窗耗尽后恰一帧收尾
    def test_fp_a1_response_window_delays_auto_finish_after_final_guide_round(self):
        front = _front()
        self._check_response_window_constant(front)
        with tempfile.TemporaryDirectory(prefix="fp_a1_window_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-fp-a1-win"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                with _alert_legs(front, dev, up), \
                        _llm_round_stub(front, guide_reply="然后小猫遇到了谁呢？"):
                    t2 = self._drive_final_guide_round(front, sid, 0)
                    self.assertEqual(
                        _start_organizing_frames(dev.sent_text), [],
                        "FP-A1 RED(前置): 末段引导轮发出即收尾 —— 引导不是收尾（既有 "
                        "RDG-A6 齿面被破）。")
                    # 旧触发点：现状 = 末段引导 + STUCK_GUIDE_SECONDS(8s) 即自动收局
                    # （KPLOG005 缺陷③本体：ASR 链路 5~14s 恒迟到 -> 孤儿帧）。
                    import asyncio  # noqa: PLC0415
                    asyncio.run(front._story_timeout_sweep(now=_t(t2 + 8)))
                    self.assertEqual(
                        _start_organizing_frames(dev.sent_text), [],
                        "FP-A1 RED: 末段引导后 STUCK_GUIDE_SECONDS 即自动收尾 —— 应答窗"
                        "缺失（缺陷③：收尾词 ASR 链路 5~14s 结构性迟到，窗内会话已清 = "
                        "「说完了」恒成孤儿帧；KPLOG005 全天四笔笔笔实证）。")
                    # 单源可消费性：应答窗常量 monkeypatch 到超大 -> 应答窗耗尽前不收尾
                    # （内联硬编码 = 常量不被消费，此处红）。
                    with _patch_const(front, GUIDE_RESPONSE_WINDOW_SECONDS, 10 ** 6):
                        asyncio.run(front._story_timeout_sweep(now=_t(t2 + 300)))
                        self.assertEqual(
                            _start_organizing_frames(dev.sent_text), [],
                            f"FP-A1 RED: {GUIDE_RESPONSE_WINDOW_SECONDS}=10^6 注入下 "
                            "300s 后仍自动收尾 —— 应答窗时长常量未被单源消费（内联硬编码"
                            " = 手感参数不可调，FP-X2 单源可测被破）。")
                    # 应答窗耗尽（注入 30s）-> 自动收尾照常：恰一帧 + 出语境信号。
                    with _patch_const(front, GUIDE_RESPONSE_WINDOW_SECONDS, 30):
                        asyncio.run(front._story_timeout_sweep(now=_t(t2 + 600)))
                    org = _start_organizing_frames(dev.sent_text)
                    self.assertEqual(
                        len(org), 1,
                        f"FP-A1 RED: 应答窗耗尽后 start_organizing 下发 {len(org)} 帧"
                        "（应恰一帧）—— 自动收尾照常语义被破（AC5 轮数语义不动的前提下"
                        "收尾仍须发生）。")
                    exits = [f for f in _record_ctx_frames(up.sent)
                             if f.get("active") is False]
                    self.assertTrue(
                        exits,
                        "FP-A1 RED: 应答窗耗尽自动收尾未注入出语境信号（既有三式收尾"
                        "语义零偏移被破）。")

    # coverage: FP-A1 -- 应答窗内「说完了」stt 经 is_finish_phrase 走正常收尾整理归档
    #                   （不成孤儿帧）；词表零触碰、帧形状零 diff
    def test_fp_a1_finish_phrase_within_response_window_archives_normally(self):
        front = _front()
        st = _story_module()
        self._check_response_window_constant(front)
        with tempfile.TemporaryDirectory(prefix="fp_a1_fin_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-fp-a1-fin"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                with _alert_legs(front, dev, up), \
                        _llm_round_stub(front, guide_reply="然后小猫遇到了谁呢？",
                                        organize_reply="小猫回家了\n小猫回家了，它玩得很开心。"):
                    t2 = self._drive_final_guide_round(front, sid, 0)
                    # 旧触发点 sweep（应答窗内 = 现状此处已收会话；KPLOG005 缺陷③竞态
                    # 复现驱动）。修后：应答窗内不收局，会话保持累积态。
                    import asyncio  # noqa: PLC0415
                    with _patch_const(front, GUIDE_RESPONSE_WINDOW_SECONDS, 30):
                        asyncio.run(front._story_timeout_sweep(now=_t(t2 + 8)))
                    self.assertEqual(
                        list(Path(tmp).glob("*.md")), [],
                        "FP-A1 RED: 应答窗内旧触发点（末段引导 + STUCK_GUIDE_SECONDS）"
                        "即自动收局归档 ——「说完了」还在 ASR 链路上（实测 5~14s）会话"
                        "已被收，迟到转写到达即孤儿帧（KPLOG005 全天四笔笔笔实证）。")
                    # 应答窗内（t2+10s，ASR 实测迟到区间内）「说完了」转写到达。
                    with _patch_const(front, GUIDE_RESPONSE_WINDOW_SECONDS, 30):
                        up2, dev2 = _engine_to_device(
                            front, sid,
                            [{"type": "stt", "text": "小猫回家了。我说完了"}])
                    files = list(Path(tmp).glob("*.md"))
                    self.assertEqual(
                        len(files), 1,
                        "FP-A1 RED: 应答窗内「说完了」未走收尾整理归档 —— 收尾沿被提前"
                        "收会话吃掉 = 孤儿帧本体（KPLOG005 全天四笔「说完了」笔笔迟到 "
                        "5~14s 成孤儿帧；is_finish_phrase 判定面被应答窗/过滤窗误吞亦"
                        "在此红）。")
                    org = _start_organizing_frames(dev2.sent_text)
                    self.assertEqual(
                        len(org), 1,
                        f"FP-A1 RED: 窗内收尾沿 start_organizing 下发 {len(org)} 帧"
                        "（应恰一帧）—— 三式收尾统一下发缺失（既有 RDG-A6 语义零偏移"
                        "被破）。")
                    exits = [f for f in _record_ctx_frames(up2.sent)
                             if f.get("active") is False]
                    self.assertTrue(
                        exits,
                        "FP-A1 RED: 窗内收尾沿未注入出语境信号（引擎抑制不解除）。")
                    # 归档正文 == 忠实整理输出（S-F2.2-AC4 v1.65 既有语义零偏移）。
                    body = files[0].read_text(encoding="utf-8")
                    self.assertIn("小猫回家了，它玩得很开心。", body,
                                  "FP-A1 RED: 归档正文非整理输出（S-F2.2-AC4 既有语义"
                                  "被破）。")
                    # 词表零触碰负锚：is_finish_phrase 既有词表判定原样（「说完了」在册、
                    # v1.66 需求面已冻——本包只接线不改词）。
                    self.assertTrue(st.is_finish_phrase("我说完了"),
                                    "FP-A1 RED(负锚): is_finish_phrase 词表被触碰"
                                    "（「我说完了」失判 = v1.66 冻结词表被改）。")
                    self.assertFalse(st.is_finish_phrase("然后它回家了"),
                                     "FP-A1 RED(负锚): is_finish_phrase 误判非收尾句"
                                     "（词表面被扩张/污染）。")


# =========================================================================== #
# FP-A2 — 引导篇幅上限（debt 缺陷③）                                            #
# =========================================================================== #


class FpGuideSpeechCapAcceptance(unittest.TestCase):
    """FP-A2: 单段引导播报篇幅超上限被截断/抑制；上限内原样保真。"""

    maxDiff = None

    # coverage: FP-A2 -- 截断/抑制（超限）+ 原样保真（上限内），上限常量单源可消费
    def test_fp_a2_guide_speech_cap_truncates_overlong_and_preserves_within_limit(self):
        import asyncio  # noqa: PLC0415
        front = _front()
        cap = _front_num_constant(front, GUIDE_SPEECH_MAX_CHARS)
        if cap is None:
            self.fail(
                f"FP-A2 RED: front_app.py 无模块级 {GUIDE_SPEECH_MAX_CHARS} —— 引导篇幅"
                "上限单源常量未实现（FROZEN CONTRACT A 节；KPLOG005 缺陷③ 39s 会话引导"
                "独播 ~28s）。")
        self.assertIsInstance(cap, int,
                              f"FP-A2 RED: {GUIDE_SPEECH_MAX_CHARS} 非整数常量。")
        self.assertNotIsInstance(cap, bool,
                                 f"FP-A2 RED: {GUIDE_SPEECH_MAX_CHARS} 为布尔（类型失真）。")
        self.assertGreater(cap, 0,
                           f"FP-A2 RED: {GUIDE_SPEECH_MAX_CHARS} 非正数（篇幅上限须为正）。")

        def drive_one_guide_round(sid):
            with tempfile.TemporaryDirectory(prefix="fp_a2_") as tmp:
                with _front_with_stories(tmp) as f:
                    dev, up = _FakeDeviceSocket(), _FakeUpstream()
                    with _alert_legs(f, dev, up), \
                            _llm_round_stub(f, guide_reply=LONG_GUIDE_REPLY):
                        acc = f._get_story_acc()
                        _device_to_engine(f, sid, [_CTX])
                        acc.feed(sid, "从前有一只小猫", now=_t(0))
                        asyncio.run(f._story_timeout_sweep(now=_t(600)))
                        self.assertGreaterEqual(
                            acc.guide_rounds(sid), 1,
                            "FP-A2 RED(前置): 引导轮未计（sweep 引导回合接线回退）。")
                        return list(_speak_texts(up.sent))

        # 相①：上限 monkeypatch 到 12（< 桩文本长度）-> 截断或抑制。
        with _patch_const(front, GUIDE_SPEECH_MAX_CHARS, 12):
            spoken = drive_one_guide_round("sess-fp-a2-trunc")
        for text in spoken:
            self.assertLessEqual(
                len(text), 12,
                f"FP-A2 RED: 引导播报 {len(text)} 字超上限 12 未截断/抑制 —— 篇幅上限"
                "未实现（KPLOG005 缺陷③：39s 会话引导独播 ~28s，孩子只说了 4 句）。")
            self.assertTrue(
                LONG_GUIDE_REPLY.startswith(text),
                "FP-A2 RED: 截断产物非原文前缀 —— 另编内容不可接受（引导文本须可溯源）。")
        if spoken:
            self.assertTrue(
                any(t != LONG_GUIDE_REPLY for t in spoken),
                "FP-A2 RED: 超限播报全文原样下发 —— 截断未发生（篇幅上限形同虚设）。")
        # 相②：上限 monkeypatch 到远大于桩文本 -> 原样保真（防上限过紧误伤）。
        with _patch_const(front, GUIDE_SPEECH_MAX_CHARS, 2000):
            spoken2 = drive_one_guide_round("sess-fp-a2-keep")
        self.assertTrue(
            spoken2,
            "FP-A2 RED: 上限内正常引导被整体抑制 —— 上限内不受影响被破（防过紧误伤）。")
        for text in spoken2:
            self.assertEqual(
                text, LONG_GUIDE_REPLY,
                "FP-A2 RED: 上限内引导文本被改动 —— 原样保真被破。")


# =========================================================================== #
# FP-A3 — 引导播报窗口素材过滤（debt 缺陷④）                                     #
# =========================================================================== #


class FpGuideSttGuardAcceptance(unittest.TestCase):
    """FP-A3: 引导播报窗口内 STT 不进素材缓冲；窗口过期/未开播会话正常进（反向 case）。"""

    maxDiff = None

    # coverage: FP-A3 -- 窗内丢弃 + 窗口过期反向 case + 从未播报引导会话反向 case
    def test_fp_a3_guide_broadcast_window_drops_stt_and_expires_to_normal_feed(self):
        import asyncio  # noqa: PLC0415
        front = _front()
        guard = _front_num_constant(front, GUIDE_STT_GUARD_SECONDS)
        if guard is None:
            self.fail(
                f"FP-A3 RED: front_app.py 无模块级 {GUIDE_STT_GUARD_SECONDS} —— 引导播报"
                "窗口 STT 过滤时长单源常量未实现（FROZEN CONTRACT A 节；KPLOG005 缺陷④"
                "「Okay」幻听直进素材缓冲）。")
        self.assertIsInstance(guard, (int, float),
                              f"FP-A3 RED: {GUIDE_STT_GUARD_SECONDS} 非数值常量。")
        self.assertNotIsInstance(guard, bool,
                                 f"FP-A3 RED: {GUIDE_STT_GUARD_SECONDS} 为布尔（类型失真）。")
        self.assertGreater(guard, 0,
                           f"FP-A3 RED: {GUIDE_STT_GUARD_SECONDS} 非正数（过滤窗须为正）。")
        with tempfile.TemporaryDirectory(prefix="fp_a3_guard_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                with _alert_legs(front, dev, up), \
                        _llm_round_stub(front, guide_reply="然后小猫遇到了谁呢？"):
                    # 引导播报（t=600）-> 播报窗口开启沿。
                    sid = "sess-fp-a3-win"
                    _device_to_engine(front, sid, [_CTX])
                    acc.feed(sid, "从前有一只小猫", now=_t(0))
                    with _patch_const(front, GUIDE_STT_GUARD_SECONDS, 60):
                        asyncio.run(front._story_timeout_sweep(now=_t(600)))
                    self.assertGreaterEqual(
                        acc.guide_rounds(sid), 1,
                        "FP-A3 RED(前置): 引导轮未计（播报窗口开启沿前提失真）。")
                    # 窗内 stt（幻听「Okay」，KPLOG005 13:02:34 实证）：不进素材缓冲。
                    frame = {"type": "stt", "text": "Okay"}
                    try:
                        r1 = front._story_feed_from_stt(sid, frame, now=_t(600))
                    except TypeError as e:
                        self.fail(
                            "FP-A3 RED: _story_feed_from_stt 无 keyword-only now 注入"
                            f"形参（{e}）—— 引导播报窗口时钟注入缝未落地（FROZEN "
                            "CONTRACT B 节；RDG-A5 self_push keyword-only 先例同缝），"
                            "素材面对引导播报窗口不设防（KPLOG005 缺陷④）。")
                    self.assertFalse(
                        r1, "FP-A3 RED: 窗内幻听 stt 触发了收尾沿（过滤窗语义越界）。")
                    self.assertNotIn(
                        "Okay", acc.build_organizing_input(sid) or "",
                        "FP-A3 RED: 引导播报窗口内 STT 进了素材缓冲 —— 素材面不设防"
                        "（KPLOG005 缺陷④：本次成文干净是整理 LLM 丢噪声的运气非设计）。")
                    # 反向 case①：窗口过期（注入 60s 过滤窗 + 1s）-> 正常进素材
                    # （防过滤窗过宽误吞孩子接话）。
                    try:
                        front._story_feed_from_stt(
                            sid, {"type": "stt", "text": "然后它回家了"}, now=_t(661))
                    except TypeError as e:
                        self.fail("FP-A3 RED: _story_feed_from_stt now 形参缺席（" + repr(e) + "）。")
                    got = acc.build_organizing_input(sid) or ""
                    self.assertIn(
                        "然后它回家了", got,
                        "FP-A3 RED: 窗口过期后孩子接话未进素材 —— 过滤窗过宽误吞孩子"
                        "接话（反向 case 必载被破）。")
                    self.assertNotIn("Okay", got,
                                     "FP-A3 RED: 被丢弃的窗内幻听句滞后回流进素材。")
                    # 反向 case②：从未播报引导的会话不受过滤窗影响（同刻注入 now）。
                    sid2 = "sess-fp-a3-quiet"
                    acc.start(sid2, now=_t(600))
                    try:
                        front._story_feed_from_stt(
                            sid2, {"type": "stt", "text": "从前有一只小猫"}, now=_t(600))
                    except TypeError as e:
                        self.fail("FP-A3 RED: _story_feed_from_stt now 形参缺席（" + repr(e) + "）。")
                    self.assertIn(
                        "从前有一只小猫", acc.build_organizing_input(sid2) or "",
                        "FP-A3 RED: 未播报引导的会话 stt 被过滤 —— 过滤窗必须按会话"
                        "播报沿开启，不得全局误伤。")
                    # 负锚：过滤窗只滤素材面——收尾词判定面不在窗内
                    # （FROZEN CONTRACT B 节；应答窗内「说完了」收尾沿照常可达，FP-A1）。
                    self.assertTrue(
                        callable(getattr(front, "_story_finish_on_phrase", None)),
                        "FP-A3 RED(负锚): _story_finish_on_phrase 在世性被破 —— 收尾词"
                        "判定面不得被过滤窗收编。")


# =========================================================================== #
# FP-A4 — 列表同日 seq tiebreak（debt 缺陷⑤a）                                   #
# =========================================================================== #


class FpStoryListOrderAcceptance(unittest.TestCase):
    """FP-A4: 同日多篇按 seq 倒序；缺 seq 回退 id 倒序不崩；跨日日期倒序不变。"""

    maxDiff = None

    # coverage: FP-A4 -- 同日 seq 倒序（新篇在上）+ 跨日日期倒序 + items 形状零偏移
    def test_fp_a4_same_day_seq_tiebreak_desc_and_cross_day_unchanged(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="fp_a4_seq_") as tmp:
            # 同日两篇：id 序与 seq 序**反排**（zzz 先归档 = 旧篇；aaa 后归档 = 新篇）
            # —— 现状 (date, id) reverse 排序把新篇压在旧篇之下（KPLOG005 缺陷⑤a：
            # 13:03 新篇 s789d9474 排 09:31 旧篇 s9074aa51 之下）。
            st.archive_story(tmp, "上午讲的旧故事正文", title="早晨的旧篇",
                             slug="zzz", date="2026-10-22")
            st.archive_story(tmp, "下午讲的新故事正文", title="下午的新篇",
                             slug="aaa", date="2026-10-22")
            st.archive_story(tmp, "昨天讲的正文", title="昨天的画",
                             slug="mmm", date="2026-10-21")
            payload = st.list_stories(tmp)
            items = payload["data"]["items"]
            self.assertEqual(payload["data"]["total"], 3,
                             "FP-A4 RED: total 语义偏移（= 现存篇数，既有契约面）。")
            # items[0] = 同日 seq 最大（新篇 aaa）；items[1] = 旧篇 zzz；items[2] = 昨日。
            self.assertEqual(
                items[0]["id"], "2026-10-22-aaa",
                "FP-A4 RED: 同日新篇（seq 大者）未排在最上 —— 同日 tiebreak 仍按 id "
                "字符串倒序（随机散列无时间序；KPLOG005 缺陷⑤a「找不到刚录的篇目」"
                "本体）。")
            self.assertEqual(
                items[1]["id"], "2026-10-22-zzz",
                "FP-A4 RED: 同日旧篇排位偏移（seq 倒序应为次位）。")
            self.assertEqual(
                items[2]["id"], "2026-10-21-mmm",
                "FP-A4 RED: 跨日日期倒序被破（昨日篇须排最后——契约 §1.3③ 日期倒序"
                "零偏移）。")
            # items 字段形状零偏移（零 wire 变更负锚：crossEnd 豁免预核面）。
            for item in items:
                self.assertEqual(
                    set(item.keys()), {"id", "title", "date"},
                    f"FP-A4 RED: story_list items 字段形状偏移 {sorted(item.keys())}"
                    " —— tiebreak 只许变同日到达序，零 wire 形状变更（契约 §1.3③）。")

    # coverage: FP-A4 -- 缺 seq 记录回退 id 倒序不崩 + 账本纯读（list 不重建/不落盘）
    def test_fp_a4_missing_seq_falls_back_id_desc_without_crash(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="fp_a4_noseq_") as tmp:
            st.archive_story(tmp, "第一篇正文", title="第一篇", slug="bbb",
                             date="2026-10-22")
            st.archive_story(tmp, "第二篇正文", title="第二篇", slug="aaa",
                             date="2026-10-22")
            ledger = Path(tmp) / getattr(st, "_LEDGER_NAME", ".seq.json")
            self.assertTrue(ledger.is_file(),
                            "FP-A4 RED(前置): seq 账本未落盘（archive_story 既有面回退）。")
            os.remove(ledger)  # 账本缺失（历史脏态）-> 缺 seq 回退路径
            payload = st.list_stories(tmp)  # 绝不崩
            items = payload["data"]["items"]
            self.assertEqual(len(items), 2,
                             "FP-A4 RED: 缺 seq 账本下列表丢篇（不崩之外还须全量）。")
            self.assertEqual(
                [i["id"] for i in items], ["2026-10-22-bbb", "2026-10-22-aaa"],
                "FP-A4 RED: 缺 seq 记录未按 id 倒序回退（同日 tiebreak 回退路径；"
                "id 字符串倒序 bbb > aaa）。")
            self.assertFalse(
                ledger.exists(),
                "FP-A4 RED: list_stories 重建/落盘了 seq 账本 —— 账本对 list 必须"
                "纯读（front 单写者纪律；fetch_story 先例，重建只归 _assign_seq 归档沿）。")


# =========================================================================== #
# FP-A5 — 标题敷衍兜底（debt 缺陷⑤b）                                           #
# =========================================================================== #


class FpGenericTitleAcceptance(unittest.TestCase):
    """FP-A5: 敷衍标题启发式（单源可测）命中兜底；正常标题原样保留。"""

    maxDiff = None

    # coverage: FP-A5 -- is_generic_story_title 纯函数 + GENERIC_STORY_TITLES 单源换表随动
    def test_fp_a5_generic_title_heuristic_single_source(self):
        front = _front()
        fn = getattr(front, GENERIC_TITLE_FN, None)
        if not callable(fn):
            self.fail(
                f"FP-A5 RED: front_app.py 无 {GENERIC_TITLE_FN} 纯函数 —— 标题敷衍兜底"
                "启发式未实现（FROZEN CONTRACT E 节；KPLOG005 缺陷⑤b 整理输出标题"
                "「我的故事」在列表里无可识别性）。")
        titles = getattr(front, GENERIC_TITLE_SET, None)
        if not titles:
            self.fail(
                f"FP-A5 RED: front_app.py 无模块级 {GENERIC_TITLE_SET} 常量（或为空）"
                " —— 敷衍标题集单源缺席（启发式须单源可测，FP-X2）。")
        self.assertTrue(
            all(isinstance(t, str) and t.strip() for t in titles),
            f"FP-A5 RED: {GENERIC_TITLE_SET} 元素须为非空 str。")
        # 实证锚：KPLOG005 该笔 LLM 起的敷衍题（human「标题叫啥，我咋找不到呢」）。
        self.assertTrue(fn("我的故事"),
                        "FP-A5 RED: 实证敷衍标题「我的故事」未命中启发式。")
        for t in ("糖葫芦", "下午的风筝", ""):
            self.assertFalse(
                fn(t), f"FP-A5 RED: 正常标题 {t!r} 被误判敷衍（启发式过宽误杀）。")
        self.assertFalse(fn(None), "FP-A5 RED: None 未安全拒判（非 str -> False）。")
        # 单源：临时换表 -> 判定随动（实现期可调，RDG-A1 STUCK_PHRASES 先例同款）。
        with _patch_const(front, GENERIC_TITLE_SET, ("万能标题",)):
            self.assertFalse(
                fn("我的故事"),
                f"FP-A5 RED: {GENERIC_TITLE_FN} 未消费 {GENERIC_TITLE_SET} 单源"
                "（判定与词表双源 = 启发式不可调）。")
            self.assertTrue(fn("万能标题"),
                            f"FP-A5 RED: 换表后新敷衍词未命中。")

    # coverage: FP-A5 -- _organize_and_archive 标题命中启发式 -> 兜底替换（非空、
    #                   ≠敷衍原词、≤12 字）；正常标题原样保留（≤12 字截断零偏移）
    def test_fp_a5_organize_generic_title_fallback_and_normal_preserved(self):
        front = _front()
        with tempfile.TemporaryDirectory(prefix="fp_a5_title_") as tmp:
            with _front_with_stories(tmp) as front:
                # 相①：敷衍标题 -> 兜底替换/后缀化（替换形态实现自由，GAP-3）。
                with _llm_round_stub(
                        front,
                        organize_reply="我的故事\n小猫出去了，它玩得很开心。"):
                    rec = front._organize_and_archive("小猫出去了，它玩得很开心。")
                self.assertIsNotNone(
                    rec, "FP-A5 RED(前置): 整理回合未归档（既有链路回退）。")
                self.assertNotEqual(
                    rec["title"], "我的故事",
                    "FP-A5 RED: 敷衍标题「我的故事」原样入册 —— 兜底替换缺失"
                    "（KPLOG005 缺陷⑤b：列表里无可识别性，human 找不到刚录篇目）。")
                self.assertTrue(rec["title"], "FP-A5 RED: 兜底替换产出空标题。")
                self.assertLessEqual(
                    len(rec["title"]), 12,
                    "FP-A5 RED: 兜底标题超 12 字（列表行显示位截断口径被破）。")
                # 相②：正常标题原样保留（防兜底过宽误改）。
                with _llm_round_stub(front, organize_reply="糖葫芦\n正文一句话。"):
                    rec2 = front._organize_and_archive("正文一句话。")
                self.assertIsNotNone(rec2, "FP-A5 RED(前置): 整理回合未归档。")
                self.assertEqual(
                    rec2["title"], "糖葫芦",
                    "FP-A5 RED: 正常标题被兜底改动 —— 原样保留被破（启发式过宽）。")


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
