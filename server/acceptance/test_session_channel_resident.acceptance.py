"""Frozen acceptance tests — TaskPackage 261116-channel-resident-core（server 面 / front）。

承载 story = S-F1.4（提醒必达：常驻通道下会话间隙仍能出音）+ requirements §2.1（结束
三通道）+ S-F2.2-AC4（取消与三式收尾回归）。语义权威 = docs/architecture.md §5.1.2 /
§6.4 + ADR-019 + 契约 §四 listen / kidpal_keepalive / kidpal_session_ctx。技术方案 =
delivery/taskpackages/261116-channel-resident-core/plan.md「技术方案」T5–T9 与「验收方向
修订建议」。

Scope of record（SCRF-B1..B6）:
  SCRF-B1 心跳 producer（front）：env 默认开 / =0 关（回滚开关，极性反于
          KIDPAL_STORY_RESTORE_PUSH）；一个周期设备腿收恰一帧 {"type":"kidpal_keepalive"}
          且 sleep-first（建连当刻零帧）；发送抛异常 → task 静默退出、双泵 finally 三件
          仍执行。
  SCRF-B2 listen stop 分层收尾：① chat（问答）不动作、标记假零注入；② accumulate +
          accum 非空 → 已被 _story_finish_session 消费、收尾三件不得再动（恰一篇归档、
          恰一帧 start_organizing）；③ accumulate + accum 空 → pending_stop 禁止 cancel /
          禁止释放（protected state）；④ pending_stop 重复 stop → mark_pending 幂等、
          不动作；⑤ accumulate 未被归档路径消费（结构上不可达，防御性）→ 不 cancel。
  SCRF-B2b pending 保护 + sweep 释放：+9s 零释放、+10s 由 sweep 释放且 active=false 恰
          一帧入归属腿；先投 stt 走恢复归档则 sweep 不重复释放。
  SCRF-B3 discard→stop 有序不归档（承重齿）：同会话先 discard 后 listen stop → routing
          回 chat、_story_finish_session 未被调、STORIES_DIR 文件数不变。
  SCRF-B4 ctx 边沿兜底分态：accumulate → cancel(reason="session_end_stale") + 释放；
          pending_stop → 不动作；chat 但标记真 → 只释放；重复 ctx(非 story) → 零新增注入。
  SCRF-B4b pending_stop 分支不动作（F-01 ctx 边沿复发防线）：ctx(story)→stop(空)→
          ctx(home) 零 cancel / 零释放 / routing 仍 pending_stop；随后 stt 仍恰一篇归档。
  SCRF-B5 连接级 teardown 保绿（must-stay-green）：断开 → 腿双摘除 +
          acc.cancel(reason="ws_disconnect")。
  SCRF-B6 既有冻结 oracle 保绿回归网（presence 守卫 + 全量 suite 绿留痕）。

真机轮 A 修复轮追加（2026-11-17，human「三个问题都修」；planner/architect 修复轮
技术方案 = plan.md「真机轮 A 修复轮（2026-11-17）技术方案」D1/D2/D3）:
  SCRF-B7（D1）记录语境注入去边缘化：ctx(source_page=story) 每会话**无条件**注入
          kidpal_record_context active=true——routing 已 accumulate（泄漏会话态 / 重复
          ctx / 引擎腿重建）时仍恰一帧；control/start_recording 消费沿同口径
          （_story_route_display_payload 返回语义 enter_edge -> consumed）。front 侧
          审计 token story_record_ctx_enter 单一 emit 点（源文本恰 1 处）+ 真跑泵可
          观测（src=ctx_story|start_recording / edge= / injected=）。story_list 仍守
          ctx_push_edge（重复 ctx 零新增）。
  SCRF-B8（D2）丢弃终止在途回合（**不发 abort**）：kidpal_story_discard 分支内、返回前
          经泵内本连接引擎腿直发 active=false——同一协程同一 upstream 顺序 await ⇒
          active=false 严格先于随后转发的 listen stop；恰一帧（重复 stop 零第二次注入）；
          腿集不得回退 _alert_legs（源文本负锚）；全程零 abort 帧（锁死本轮决策）。
          must-stay-green 对照：ctx+feed+stop（不发 discard）仍恰一篇归档且 active=false
          出现在 stop **之后**（防修过头毁 KEY 尾音归档）。
  SCRF-B9（D2 迟到帧）会话关闭门：_release_record_context 单点 arm；下一次 listen start /
          ctx(story) disarm；门内丢弃引擎腿→设备腿的二进制音频帧与 type in
          ("tts","llm") 文本帧，不拦 type=="mcp" 与 stt。front 自注 kidpal_speak 令牌
          登记后有界放行整轮（含其 tts stop；先判令牌后清令牌——tts stop 帧本身放行，
          stop 之后清位）；令牌登记点覆盖提醒 speech（_deliver_alert_now，reminders.py
          引擎腿注入不经 _speak_to_legs）。
  SCRF-B10（D3 front 半）空素材收尾=释放口径：空素材 stop（accum 空）→ pending_stop →
          10s sweep 释放 + active=false 恰一帧；_story_finish_session 零调用、STORIES_DIR
          零新增、零 start_organizing、零新增 story_list；尾音幻听（「嗯。」）走
          _archive_material_reject_reason 拒绝路径同口径。**预期设计态绿**（must-stay-green
          + 轮 B 离线代理，不造红）。

修复轮强度申报（契约条目 -> 承载断言 -> 性质档；acceptance-evidence T2 纪律）:
  契约 §四 kidpal_session_ctx「每会话进入记录语境」语义 -> B7 -> 行为级（真泵 + 真
    StoryAccumulator + 假引擎腿收帧计数）+ 源锚代理（审计 token 单点计数，辅助信号）。
  device-round-a-evidence §一（丢弃必须使引擎不再产出该回合 TTS）-> B8 -> 行为级（真泵
    帧序 + 腿集归属实证：另一条全局提醒腿零帧）+ 源锚代理（discard 分支 _alert_legs 负锚）。
  契约 §四 kidpal_story_discard + ADR-013 -> B8 对照齿 -> 行为级（真归档落盘）。
  device-round-a-evidence §一（迟到 TTS 拽回 Speaking）-> B9 -> 行为级（真泵逐帧转发
    判决：二进制 / tts / llm 丢弃，mcp / stt 放行；令牌放行整轮）。
  S-F1.4 + ADR-003 -> B9 提醒 speech 令牌登记点 -> 行为级（真 _deliver_alert_now 两腿
    投递 + 随后 tts 帧放行；提醒弹窗+提示音半直发设备腿 = 结构性免疫，本断言只证语音
    令牌面不误丢）。
  requirements §2.1 / §2.3（D3 设备半）+ S-F2.2-AC4 -> B10 -> 行为级（pending 释放 +
    零归档/零推送/零演出；拒绝路径同口径）；设备侧空素材收尾 producer 本体 = firmware
    件 SCR-A8（源锚代理），真机 60s 自动回列表 = 轮 B B-D3。
  弱于契约处（显式缺口，不得当已兑现）:
    GAP-B7-1 引擎侧「收到 active=true 后抑制 LLM」行为不在本侧断言（引擎代码零改动
     铁律）——front 只证注入帧送达与帧数；引擎侧抑制 = engine-patch 既有冻结面 + 轮 B
     三源对账（B-D1：问句零 TTS、设备不 idle->speaking）。
    GAP-B9-1 kidpal_speak 令牌 **deadline 30s 到期**路径未离线断言（无冻结时钟注入缝；
      令牌上限是安全网，主限界 = tts stop 清位，已断言）。到期行为归真机轮 B-D2。
    GAP-B9-2 「令牌在场时 display 块仍可能闪屏」列 residual（B8 修好后该回合不应存在）。
    GAP-B10-1 D3 设备侧 60s 自动收尾的运行时行为 = 真机轮 B-D3；本件只钉 front 释放
      口径（离线 half）。
  行为等价假设（显式；不写即默认不成立）:
    EQ-B9-1 「同 device websocket 对象 ≙ 同连接」：会话关闭门 / 令牌按 device ws 对象
      建键，离线用同一 _FakeDeviceSocket 跨多次 pump 承载（与 EQ-3 同款）。
    EQ-B9-2 「二进制帧在 gate 内被丢弃 ≙ 设备不再播该段音频」：本件只证前件；后件归真机。
  夹具自含性（显式声明）: B7–B10 只用 tempfile STORIES_DIR + 假双腿 + 假时钟注入
    （sweep now）；无网络、无真实墙钟、无外部资产。唯一的模块级跨调用状态
    （_RECORD_CTX_SEEN / 会话关闭门 / speak 令牌）按 device ws 对象建键，test 间零共享。

===========================================================================
FROZEN OFFLINE CONTRACT（builder implements；fastapi 缺席本地 stub，261019/RCL 先例；
story.py 三缝 release_pending / cancel / PENDING_STOP_SECONDS 零改动）：

  T5（front_app.py ws_gateway）：每连接心跳 task，落 await _push_schedule_connect 之后、
  asyncio.wait({pump_up, pump_dn}, FIRST_COMPLETED) 之前创建；发送形态 = 裸
  websocket.send_text(json.dumps({"type":"kidpal_keepalive"}))（非 display 帧）；循环体
  sleep-first 并整体 try/except Exception: return；finally 内首行 hb.cancel() +
  await asyncio.gather(hb, return_exceptions=True)；绝不进 FIRST_COMPLETED 集合。开关
  KIDPAL_CHANNEL_HEARTBEAT（代码默认开；0/false/off/no 关 = 回滚开关）与
  KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S（默认 30）；模块级常量与 channel_heartbeat_enabled()
  helper 定义在 ws_gateway 之前。

  T6（pump_client_to_upstream，_story_stop_on_listen_stop 之后）：listen/stop 门控后按
  acc.routing 分层——chat + 标记假 → 零动作；accumulate + accum 非空 → 不动作（已被归档
  路径消费）；accumulate + accum 空（pending_stop）与 pending_stop 重复 → 禁止 cancel /
  禁止释放（protected）；未消费兜底分支不 cancel。每连接「记录语境曾置位」标记
  _record_ctx_seen（WeakKeyDictionary 形），置位点 = ctx(story) ctx_push_edge +
  _inject_record_context_enter 调用点；清位单点 = _release_record_context 体内按 legs
  的 device_ws 逐个清。审计一行含冻结 token session_end_boundary。

  T7（_story_timeout_sweep_body）：release_pending 返 True 的分支追加
  await _release_record_context(sid, _session_legs(sid) or list(_alert_legs))。

  T8（_consume_kidpal_control ctx 分支）：收到非 story 合法 ctx 且标记真时——pending_stop
  → 不动作；accumulate → acc.cancel(sid, reason="session_end_stale") + 释放；chat +
  标记真 → 只释放。重复 ctx(非 story) → 零新增注入（幂等）。未知 source_page 仍拒绝 +
  warning 且不释放（既有 elif 行为逐字保留）。

  T9（ws_gateway finally）：腿双摘除 + acc.cancel(reason="ws_disconnect") 逐字保留。

强度申报（契约条目 -> 承载断言 -> 性质档）:
  契约 §四 kidpal_keepalive / 部署极性 -> B1 -> 行为级（真跑 ws_gateway + 真设备腿收帧）。
  F-01 承重单点（记录语境释放） -> B2/B2b/B4/B4b -> 行为级（真泵 + 真 StoryAccumulator +
  真时钟注入；释放经 spy 与真引擎腿帧双承载）。
  S-F2.2-AC4 discard→stop 不归档 -> B3 -> 行为级（真泵 + 真归档落盘计数）。
  OQ2 cancel-on-disconnect -> B5 -> 行为级（真跑 ws_gateway）。
  弱于契约处（显式缺口）:
  GAP-1 心跳「假时钟」以注入短 interval（env KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S）近似
        ——心跳循环用 asyncio.sleep，无注入 now 缝；等价断言 = sleep-first 零前置帧 +
        一个周期恰一帧 + 帧形状。真 30s 节奏归真机轮 A3。
  GAP-2 心跳三方并发写同一 device websocket 的竞态（无发送锁 residual）= 真机轮观测。
  GAP-3 设备 → front → 引擎三源对齐 = 真机轮 E5。
行为等价假设（显式；不写即默认不成立）:
  EQ-1 「pump_client_to_upstream 真路径 = 现场帧消费路径」（RCL EQ-1 同款，已复证）。
  EQ-2 「fake websockets 连接 ≈ 真连接上下文」（RCL EQ-5 同款）。
  EQ-3 「同 device websocket 对象 ≙ 同连接」（_record_ctx_seen 按 device ws 建键）——
        离线真泵跨多次调用复用同一 _FakeDeviceSocket 承载。
===========================================================================

轮 C 修复轮追加 REVISION LOG（2026-11-18；human「现在就修」+ 三源在线证据
delivery/taskpackages/261116-channel-resident-core/device-evidence/round-c/；技术方案 =
plan.md「真机轮 C 修复轮（2026-11-18）技术方案」节）：
  本轮只 **additive** 追加两个 TestCase 类，既有 B1..B10 断言行零改动（含 SCRF-B10
  case 2 的「嗯。」收尾沿消费）。新增 acceptance id = SCRF-C1..C4（TaskPackage 已落盘）：
  SCRF-C1 段级静音 ASR 幻觉过滤 + pending_stop 不归档（端到端主防线）。
  SCRF-C2 整理产物占位守卫（后防线，承重）：R1–R4 机械判据 + 页面退出帧 + reason 分档。
  SCRF-C3 A5 正常路径回归（must-stay-green）：正常两行整理结果仍归档；丢稿类 reason
          （llm_unavailable）仍安抚 + 页面退出帧。
  SCRF-C4 素材侧过滤保守性 + 集合不变式 + 判定单源（判据判别力）。
  真机缺陷面（round-c 证据）：① 静音 ASR 幻觉 `Okay.`（engine-asr 21:22:06）喂进
  pending_stop 累积器 → 过素材守卫（compact=4）→ 整理 LLM 返回占位串「（没有可整理的
  内容）」→ 被当正文归档成垃圾篇 2026-11-18-s33477cee（ecs-story-*.md 逐字为该串）。

强度申报（契约 / story 条目 -> 承载断言 -> 性质档）:
  S-F2.2-AC4（三式收尾：归档正文 == 整理输出、不写垃圾篇）-> C2 -> 行为级（真泵 +
    真 StoryAccumulator + 真落盘/seq 账本零增 + 真下行帧序列）+ 源锚代理（R1–R4 直调
    `_organize_and_archive` 的返回/日志；LLM 桩替身 = 外部不可控边界的替身，非被测面）。
  S-F2.2-AC4 正常路径 -> C3 -> 行为级（真归档落盘 + 真 story_text/story_list 帧 + seq）。
  requirements §2.1（记录会话收尾）-> C1 -> 行为级（真泵 + 注入时钟 sweep；零归档 /
    零演出 / 恰一帧 active=false / routing 回 chat）。
  human 2026-11-18「白名单/归一化要保守，不得误伤孩子真说的话」-> C4 -> 行为级（混排
    真话入累积器、集合不变式遍历实际表、换表判定随动）。
  弱于契约处（显式缺口，不得当已兑现）:
    GAP-C1 引擎侧「收到 active=false 后不再产尾音 ASR」不在本侧断言（引擎零改动铁律）；
      本件只证 front 侧段级过滤与释放链。真机轮 C 端到端归 human 持板复测。
    GAP-C2 设备侧 ORGANIZING 过渡页真实退出（页面真的回到 P3 列表）归真机；离线只证
      「页面退出帧恰一帧 + 列表内容零新增」（261021 R6 先例同一口径）。
    GAP-C4 长尾占位（措辞无关以外的语义重叠检查）本轮不做（planner 裁定 7：误杀面
      不可忽略），登记 follow-up 候选。
  行为等价假设（显式；不写即默认不成立）:
    EQ-C1 「离线 LLM 桩返回占位串 ≙ 真实整理 LLM 的占位回复」——桩替身只承载回复文本，
      判据本体（R1–R4 归一化整串判定）在真实代码路径上执行。
    EQ-C2 「真泵帧序 ≙ 设备侧可见序列」——离线只证 front 下发帧序与内容，设备渲染归真机。
  口径偏差留痕（与 human「不推 story_list」原话）：C2 的整理回合已发生路径断言
    「页面退出帧恰一帧且 total==0 / items==[] / restore 缺省或 false」——该帧是
    ORGANIZING 非交互页的唯一机械退出载体（261021 真机 R6 困页实证，SAO-A1 同因重钉）；
    「列表不虚增」由不归档保证（total 由 list_stories 现算）。会话从未进整理回合的
    路径（C1）断言严格零 story_list。planner 裁定 5 已把该分法写入 review-packet。
  新态 chrome 三件自查：本批 uiux block 不在场（input-brief ①c = none），无 UI 设计面。
  device-outcome / host-uncompilable-path：不适用（C1..C4 实现落点全部 = server/
    gateway/front/front_app.py，host 可跑；无固件落点）。
  夹具自含性（显式声明）: 每组用独立 tempfile STORIES_DIR + 假双腿 + 注入时钟（sweep now）
    + LLM 桩；无网络（C1 未打桩路径的 LLM 出口在连接拒绝级快失败）、无真实墙钟依赖、
    无外部资产。唯一的模块级跨调用状态（_story_acc / 记录语境标记 / 会话关闭门）由
    `_front_with_stories` 在用例内重置，test 间零共享。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import logging
import os
import re
import sys
import tempfile
import types
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"

KEEPALIVE_TYPE = "kidpal_keepalive"
RECORD_CTX_TYPE = "kidpal_record_context"
START_ORGANIZING_ACTION = "start_organizing"
CTX_TYPE = "kidpal_session_ctx"
HEARTBEAT_ENV = "KIDPAL_CHANNEL_HEARTBEAT"
HEARTBEAT_INTERVAL_ENV = "KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S"
# 注入短周期（GAP-1）：sleep-first；一个周期恰一帧的离线近似。
_TEST_INTERVAL_S = "0.2"

# 冻结 reason 值（T6/T8）。
END_STALE_REASON = "session_end_stale"
WS_DISCONNECT_REASON = "ws_disconnect"

# 契约 §四 wire 形态（对端实际接受形状）。
def _ctx_msg(source_page: str) -> str:
    return json.dumps({"type": CTX_TYPE, "source_page": source_page}, ensure_ascii=False)


def _listen_stop_msg() -> str:
    return json.dumps({"type": "listen", "state": "stop"}, ensure_ascii=False)


def _discard_msg() -> str:
    return json.dumps({"type": "kidpal_story_discard"}, ensure_ascii=False)


def _stt_frame(text: str) -> dict:
    return {"type": "stt", "text": text}


# --------------------------------------------------------------------------- #
# Module loading（missing/unimportable = RED for missing behaviour）           #
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
    # 注入短心跳周期（GAP-1）：env 在 import 前落位，模块级常量读到此值。
    os.environ[HEARTBEAT_INTERVAL_ENV] = _TEST_INTERVAL_S
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_scr", FRONT_APP)
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
        raise AssertionError("SCR RED: " + err)
    return mod


@contextlib.contextmanager
def _front_with_stories(stories_dir):
    front = _front()
    old_dir, old_acc = front.STORIES_DIR, front._story_acc
    front.STORIES_DIR = str(stories_dir)
    front._story_acc = None
    try:
        yield front
    finally:
        front.STORIES_DIR = old_dir
        front._story_acc = old_acc


def _t0():
    return datetime(2026, 11, 16, 12, 0, 0, tzinfo=dt_timezone.utc)


# --------------------------------------------------------------------------- #
# I/O doubles                                                                  #
# --------------------------------------------------------------------------- #


class _FakeUpstream:
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


class _SilentUpstream:
    def __init__(self):
        self.sent = []

    def __aiter__(self):
        return self

    async def __anext__(self):
        await asyncio.Event().wait()

    async def send(self, message):
        self.sent.append(message)


class _FakeDeviceSocket:
    """starlette-style receive() dicts + send_text/send_bytes；可跨多次 pump 复用
    （_incoming 顺序消费，_pos 持续）——同对象身份 ≙ 同连接（EQ-3）。"""

    def __init__(self, incoming=(), disconnect_exc=None):
        self._incoming = list(incoming)
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.sent_text = []
        self.sent_bytes = []

    def append_text(self, text):
        self._incoming.append({"type": "websocket.receive", "text": text})

    async def receive(self):
        if self._pos >= len(self._incoming):
            raise self._disconnect_exc()
        msg = self._incoming[self._pos]
        self._pos += 1
        return msg

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(bytes(data))


class _BlockingDeviceSocket:
    """ws_gateway 面设备腿（B1 心跳）：receive() 永不返回（连接常驻），send_text 记录；
    可配 keepalive 帧发送抛异常（发送失败分支）。"""

    def __init__(self, auth_header="", raise_on_keepalive=False):
        self.headers = {"authorization": auth_header}
        self.sent_text = []
        self.sent_bytes = []
        self.accepted = False
        self._raise_on_keepalive = raise_on_keepalive

    async def accept(self):
        self.accepted = True

    async def close(self, code=1000):
        self.closed_with = code

    async def receive(self):
        await asyncio.Event().wait()

    async def send_text(self, text):
        if self._raise_on_keepalive and KEEPALIVE_TYPE in text:
            raise RuntimeError("injected keepalive send failure")
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(bytes(data))


class _GatewayDeviceSocket:
    """ws_gateway 面设备腿（B5 teardown）：receive() 立即断连。"""

    def __init__(self, auth_header=""):
        self.headers = {"authorization": auth_header}
        self.sent_text = []
        self.sent_bytes = []
        self.accepted = False
        self.closed_with = None

    async def accept(self):
        self.accepted = True

    async def close(self, code=1000):
        self.closed_with = code

    async def receive(self):
        raise RuntimeError("device leg disconnected before any frame")

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(bytes(data))


def _device_to_engine(front, sid, texts):
    """跑 device→engine 上泵（一次性注入 frames）; 返回 (upstream, device)。"""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in texts])
    up = _FakeUpstream()
    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
    return up, dev


def _engine_to_device(front, sid, frames):
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, sid))
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


def _record_ctx_frames(sent):
    return [o for o in _forwarded_objects(sent) if o.get("type") == RECORD_CTX_TYPE]


def _release_frames(sent):
    return [o for o in _record_ctx_frames(sent) if o.get("active") is False]


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
            if payload.get("method") == "tools/call":
                out.append(payload)
    return out


def _display_payloads(sent_text):
    out = []
    for env in _mcp_envelopes(sent_text):
        params = env.get("params") or {}
        if params.get("name") != "kidpal/display":
            continue
        args = params.get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _start_organizing_frames(sent_text):
    out = []
    for p in _display_payloads(sent_text):
        if p.get("view") == "control" and (p.get("data") or {}).get("action") == START_ORGANIZING_ACTION:
            out.append(p)
    return out


def _keepalive_frames(sent_text):
    out = []
    for text in sent_text:
        try:
            obj = json.loads(text)
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict) and obj.get("type") == KEEPALIVE_TYPE:
            out.append(obj)
    return out


def _story_md_files(stories_dir):
    return list(Path(stories_dir).glob("*.md"))


# --------------------------------------------------------------------------- #
# 真机轮 A 修复轮（2026-11-17）断言 helpers（SCRF-B7..B10）                        #
# --------------------------------------------------------------------------- #

# 二进制音频帧替身（引擎下行 Opus 包形态不必真解码；本组只断言「是否转发」）。
AUDIO_BYTES = b"\x00\x01\x02opus-audio-frame"


def _frames_of_type(sent_text, *types):
    """设备腿已转发文本帧中按 type 过滤（返回 dict 列表）。"""
    wanted = set(types)
    return [o for o in _forwarded_objects(sent_text) if o.get("type") in wanted]


def _frame_indices(sent, predicate):
    """腿序帧下标：sent 为上行/下行腿原始帧列表（str|bytes），返回满足谓词的下标。"""
    out = []
    for idx, item in enumerate(sent):
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict) and predicate(obj):
            out.append(idx)
    return out


def _is_release_frame(obj):
    return obj.get("type") == RECORD_CTX_TYPE and obj.get("active") is False


def _is_listen_stop_frame(obj):
    return obj.get("type") == "listen" and obj.get("state") == "stop"


def _is_abort_frame(obj):
    return obj.get("type") == "abort"


def _tts_frame(state, text=None):
    """引擎 tts 播报状态机帧（start / sentence_start（带同步文字）/ stop）。"""
    frame = {"type": "tts", "state": state}
    if text is not None:
        frame["text"] = text
    return frame


def _llm_frame():
    """引擎 llm 情绪帧（无 text 字段 -> 泵逐字转发路径）。"""
    return {"type": "llm", "emotion": "happy"}


def _mcp_probe_frame():
    """设备只认 type=mcp 进 McpServer；B9 用它证明迟到帧门不拦 mcp。"""
    return {"type": "mcp", "payload": {"jsonrpc": "2.0", "id": 1,
                                       "result": {"ack": True}}}


def _display_blocks_frame(block):
    """引擎侧 side-channel：display 块以字符串数组下发（front 消费后 relay）。"""
    return {"type": "kidpal_display_blocks",
            "blocks": [json.dumps(block, ensure_ascii=False)]}


def _start_recording_block():
    return {"view": "control", "data": {"action": "start_recording"}}


def _log_lines_with(logs, *needles):
    """捕获行中同时含全部 needle 者（大小写不敏感——布尔字段真值形态 edge=true /
    edge=True 皆认，防实现用 %s 打印 Python bool 造成假红）。"""
    out = []
    for line in logs:
        low = line.lower()
        if all(needle.lower() in low for needle in needles):
            out.append(line)
    return out


@contextlib.contextmanager
def _log_capture(name="kidpal-front"):
    """采集 logger 的 INFO+ 消息（不要求「至少一条」—— 设计态红须落在断言本体，
    不能靠 assertLogs 的零记录异常误报）。"""
    records = []
    logger = logging.getLogger(name)

    class _Capture(logging.Handler):
        def emit(self, record):  # noqa: D105
            records.append(self.format(record))

    handler = _Capture(level=logging.INFO)
    old_level = logger.level
    if logger.getEffectiveLevel() > logging.INFO:
        logger.setLevel(logging.INFO)
    logger.addHandler(handler)
    try:
        yield records
    finally:
        logger.removeHandler(handler)
        logger.setLevel(old_level)


# --------------------------------------------------------------------------- #
# Spies / stubs                                                                #
# --------------------------------------------------------------------------- #


def _spy_cancel(acc):
    calls = []
    orig = acc.cancel

    def _spy(session_id, now=None, *, reason=""):
        calls.append((session_id, reason))
        return orig(session_id, now=now, reason=reason)

    acc.cancel = _spy
    return calls


def _spy_release(front):
    """包装 front._release_record_context 记录 (session_id)（T7/T8 释放承载）。"""
    calls = []
    orig = getattr(front, "_release_record_context", None)
    if not callable(orig):
        return None, calls

    async def _spy(session_id, legs):
        calls.append(session_id)
        return await orig(session_id, legs)

    front._release_record_context = _spy
    return orig, calls


@contextlib.contextmanager
def _release_spy(front):
    calls = []
    orig = getattr(front, "_release_record_context", None)
    if not callable(orig):
        yield calls
        return

    async def _spy(session_id, legs):
        calls.append(session_id)
        return await orig(session_id, legs)

    front._release_record_context = _spy
    try:
        yield calls
    finally:
        front._release_record_context = orig


@contextlib.contextmanager
def _finish_spy(front):
    calls = []
    orig = getattr(front, "_story_finish_session", None)
    if not callable(orig):
        yield calls
        return

    async def _spy(session_id, acc, legs):
        calls.append(session_id)
        return await orig(session_id, acc, legs)

    front._story_finish_session = _spy
    try:
        yield calls
    finally:
        front._story_finish_session = orig


class _LLMStub:
    def __init__(self, reply):
        self.calls = []
        self.reply = reply

    def __call__(self, system_text, user_text, timeout_s=10.0, **kwargs):  # noqa: ANN001
        self.calls.append((system_text, user_text))
        return self.reply


@contextlib.contextmanager
def _llm_stub(front, stub):
    old = getattr(front, "_openclaw_chat_once", None)
    front._openclaw_chat_once = stub
    try:
        yield stub
    finally:
        if old is None:
            front.__dict__.pop("_openclaw_chat_once", None)
        else:
            front._openclaw_chat_once = old


@contextlib.contextmanager
def _guide_legs(front, session_id, device_ws, engine_ws):
    register = getattr(front, "_register_session_legs", None)
    if callable(register):
        register(session_id, device_ws, engine_ws)
        try:
            yield
        finally:
            front._unregister_session_legs(device_ws)
    else:  # mapping seam absent -> fall back to global alert legs
        old = list(getattr(front, "_alert_legs", []))
        front._register_alert_legs(device_ws, engine_ws)
        try:
            yield
        finally:
            front._alert_legs[:] = old


@contextlib.contextmanager
def _env_var(name, value):
    saved = os.environ.get(name)
    if value is None:
        os.environ.pop(name, None)
    else:
        os.environ[name] = value
    try:
        yield
    finally:
        if saved is None:
            os.environ.pop(name, None)
        else:
            os.environ[name] = saved


@contextlib.contextmanager
def _fake_engine_connect(upstream):
    names = ("websockets", "websockets.asyncio", "websockets.asyncio.client")
    saved = {k: sys.modules.get(k) for k in names}
    mod_ws = types.ModuleType("websockets")
    mod_asyncio = types.ModuleType("websockets.asyncio")
    mod_client = types.ModuleType("websockets.asyncio.client")

    class _ConnectCM:
        def __init__(self, *_a, **_k):
            pass

        async def __aenter__(self):
            return upstream

        async def __aexit__(self, *_exc):
            return False

    mod_client.connect = lambda *_a, **_k: _ConnectCM()
    mod_asyncio.client = mod_client
    mod_ws.asyncio = mod_asyncio
    sys.modules["websockets"] = mod_ws
    sys.modules["websockets.asyncio"] = mod_asyncio
    sys.modules["websockets.asyncio.client"] = mod_client
    try:
        yield
    finally:
        for k, v in saved.items():
            if v is None:
                sys.modules.pop(k, None)
            else:
                sys.modules[k] = v


@contextlib.contextmanager
def _gateway_env(front, token, tmp, *, fake_uuid_hex=None, quiet_loop=True):
    """ws_gateway 离线跑的前置收窄：DEVICE_TOKEN / SCHEDULE_MD / _ensure_alert_loop /
    uuid（B1/B5 共用）。"""
    old_token = front.DEVICE_TOKEN
    old_schedule = getattr(front, "SCHEDULE_MD", None)
    old_loop = getattr(front, "_ensure_alert_loop", None)
    old_uuid = getattr(front, "uuid", None)
    front.DEVICE_TOKEN = token
    if old_schedule is not None:
        front.SCHEDULE_MD = str(Path(tmp) / "no-schedule.md")
    if quiet_loop and old_loop is not None:
        front._ensure_alert_loop = lambda: None
    if fake_uuid_hex is not None:
        fake_uuid = types.SimpleNamespace()
        fake_uuid.uuid4 = lambda: types.SimpleNamespace(hex=fake_uuid_hex)
        front.uuid = fake_uuid
    try:
        yield
    finally:
        front.DEVICE_TOKEN = old_token
        if old_schedule is not None:
            front.SCHEDULE_MD = old_schedule
        if old_loop is not None:
            front._ensure_alert_loop = old_loop
        if fake_uuid_hex is not None:
            front.uuid = old_uuid


def _token():
    return "tok-scr-acceptance-0123456789ab"


async def _drive_gateway(front, dev, up, *, max_seconds, stop_when):
    """以 fake 引擎腿跑 ws_gateway 至 stop_when(dev) 或 max_seconds，然后 cancel 收尾。"""
    task = asyncio.create_task(front.ws_gateway(dev))
    waited = 0.0
    try:
        while waited < max_seconds:
            await asyncio.sleep(0.02)
            waited += 0.02
            if stop_when(dev):
                break
    finally:
        task.cancel()
        with contextlib.suppress(asyncio.CancelledError, Exception):
            await task


# =========================================================================== #
# SCRF-B1 — 心跳 producer（极性 / 一个周期恰一帧 / sleep-first / 发送失败静默）      #
# =========================================================================== #


class ScrfHeartbeatAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B1 -- 心跳开关极性：channel_heartbeat_enabled() 代码默认开；0/false/
    #            off/no 关（回滚开关，反于 KIDPAL_STORY_RESTORE_PUSH）
    def test_scr_b1_heartbeat_gate_default_on_and_rollback_off(self):
        front = _front()
        helper = getattr(front, "channel_heartbeat_enabled", None)
        self.assertTrue(callable(helper),
                        "SCRF-B1 RED: front_app 无 channel_heartbeat_enabled() helper —— "
                        "心跳部署极性决策面未落（plan T5 冻结名）。")
        # 显式 raw（纯函数，不落墙钟/环境）。
        self.assertTrue(helper(None) is True or helper("") is True,
                        "SCRF-B1 RED: channel_heartbeat_enabled 默认非开 —— 代码默认开"
                        "极性破（会退化成每 ~120s 判死 + 立即重连 churn）。")
        for off in ("0", "false", "off", "no", " FALSE ", "Off"):
            self.assertFalse(helper(off),
                             f"SCRF-B1 RED: channel_heartbeat_enabled({off!r}) 未关 —— "
                             "回滚开关识别集不全（{0,false,off,no}，strip().lower()）。")
        self.assertTrue(helper("1"),
                        "SCRF-B1 RED: channel_heartbeat_enabled('1') 非开 —— 非识别集值"
                        "应一律开（不误关）。")
        # env 缺省（None = 读 os.environ）→ 未设 → 开。
        with _env_var(HEARTBEAT_ENV, None):
            self.assertTrue(helper(),
                            "SCRF-B1 RED: env 未设时 channel_heartbeat_enabled() 非开 —— "
                            "代码默认开偏置破。")

    # coverage: SCRF-B1 -- 一个周期设备腿恰收一帧 {"type":"kidpal_keepalive"}；sleep-first
    #            （建连当刻零帧）
    def test_scr_b1_heartbeat_emits_one_frame_after_interval(self):
        with tempfile.TemporaryDirectory(prefix="scr_b1_emit_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                with _gateway_env(front, token, tmp):
                    dev = _BlockingDeviceSocket(auth_header="Bearer " + token)
                    up = _SilentUpstream()
                    with _fake_engine_connect(up):
                        # sleep-first：不足一个 interval 的短窗 → 零 keepalive。
                        asyncio.run(_drive_gateway(
                            front, dev, up, max_seconds=0.12,
                            stop_when=lambda d: False))
                        self.assertTrue(dev.accepted,
                                        "SCRF-B1 RED(infra): ws_gateway 未 accept 设备腿"
                                        " —— 鉴权门把连接拒之门外，本 case 驱动路径失真。")
                        self.assertEqual(
                            len(_keepalive_frames(dev.sent_text)), 0,
                            "SCRF-B1 RED: 建连当刻即发心跳（间隔窗内零帧被破）—— 心跳须"
                            " sleep-first（建连即发帧会往假设备腿注入额外帧，test_m4k1/"
                            "m4k3/RCL 三件真跑 ws_gateway 的零偏移前提翻车）。")
                        # 一个周期（>interval）→ 恰一帧。
                        dev2 = _BlockingDeviceSocket(auth_header="Bearer " + token)
                        asyncio.run(_drive_gateway(
                            front, dev2, up, max_seconds=1.0,
                            stop_when=lambda d: len(_keepalive_frames(d.sent_text)) >= 1))
                        ka = _keepalive_frames(dev2.sent_text)
                        self.assertEqual(
                            len(ka), 1,
                            f"SCRF-B1 RED: 一个周期内 keepalive 帧数 = {len(ka)}（冻结"
                            "恰 1）—— 心跳 producer 缺席（0）或非单帧（多帧/重复）。"
                            f" 实收 {dev2.sent_text!r}")
                        self.assertEqual(
                            ka[0], {"type": KEEPALIVE_TYPE},
                            f"SCRF-B1 RED: keepalive 帧形状 = {ka[0]!r}（契约 §四冻结"
                            ' {"type":"kidpal_keepalive"}，裸 send_text 非 display 帧）'
                            " —— 心跳被套 display/JSON-RPC 封套或字段漂移。")

    # coverage: SCRF-B1 -- env KIDPAL_CHANNEL_HEARTBEAT=0 → 零帧（回滚开关有效）
    def test_scr_b1_heartbeat_disabled_sends_zero_frames(self):
        with tempfile.TemporaryDirectory(prefix="scr_b1_off_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                with _env_var(HEARTBEAT_ENV, "0"):
                    with _gateway_env(front, token, tmp):
                        dev = _BlockingDeviceSocket(auth_header="Bearer " + token)
                        up = _SilentUpstream()
                        with _fake_engine_connect(up):
                            asyncio.run(_drive_gateway(
                                front, dev, up, max_seconds=0.7,
                                stop_when=lambda d: False))
                        self.assertEqual(
                            len(_keepalive_frames(dev.sent_text)), 0,
                            "SCRF-B1 RED: KIDPAL_CHANNEL_HEARTBEAT=0 仍发心跳 —— 回滚"
                            "开关失效（关不掉 = 旧固件收到 keepalive 落 WARN 噪声）。")

    # coverage: SCRF-B1 -- 发送抛异常 → task 静默退出、双泵 finally 三件仍执行
    #            （cancel(ws_disconnect) + 腿摘除；连接收尾不被心跳异常拖垮）
    def test_scr_b1_heartbeat_send_error_is_silent_teardown_still_runs(self):
        with tempfile.TemporaryDirectory(prefix="scr_b1_err_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                hexval = "e1e1e1e1e1e1e1e1e1e1e1e1e1e1e1e1"
                sid = "sess-" + hexval
                with _gateway_env(front, token, tmp, fake_uuid_hex=hexval):
                    acc = front._get_story_acc()
                    acc.start(sid, now=_t0())
                    cancel_calls = _spy_cancel(acc)
                    dev = _BlockingDeviceSocket(auth_header="Bearer " + token,
                                                raise_on_keepalive=True)
                    up = _SilentUpstream()
                    with _fake_engine_connect(up):
                        # 跑过 >1 个周期（心跳发送必抛）；不因异常外泄崩溃。
                        asyncio.run(_drive_gateway(
                            front, dev, up, max_seconds=0.7,
                            stop_when=lambda d: False))
                    # 双泵 finally 三件：cancel(reason=ws_disconnect) 落（腿摘除同块）。
                    self.assertIn(
                        (sid, WS_DISCONNECT_REASON),
                        [(s, r) for s, r in cancel_calls],
                        f"SCRF-B1 RED: 心跳发送异常后 finally 未落 cancel(ws_disconnect)"
                        f"（实记 {cancel_calls}）—— 心跳异常拖垮收尾链（双泵 finally 三件"
                        "被吞）。")


# =========================================================================== #
# SCRF-B2 — listen stop 分层收尾                                                  #
# =========================================================================== #


class ScrfListenStopLayeredAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B2 ①+② -- chat 问答会话 stop 零动作零注入；accumulate + accum 非空
    #            已被 _story_finish_session 消费、收尾三件不得再动（恰一篇归档 + 恰一帧
    #            start_organizing）
    def test_scr_b2_chat_noop_and_accumulate_consumed_once(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2_a_") as tmp:
            with _front_with_stories(tmp) as front:
                # ① chat 会话：无 ctx，直接 listen stop -> 零动作。
                sid = "sess-b2-chat"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _listen_stop_msg()}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(front._get_story_acc().routing(sid), "chat",
                                 "SCRF-B2 RED: chat 问答会话 stop 后被误切记录态。")
                self.assertEqual(_release_frames(up.sent), [],
                                 "SCRF-B2 RED: chat 会话 stop 触发了记录语境释放注入"
                                 "（标记假时须零注入——防每轮问答打一帧无用 active=false）。")
                self.assertEqual(_start_organizing_frames(dev.sent_text), [],
                                 "SCRF-B2 RED: chat 会话 stop 触发收尾演出。")

        with tempfile.TemporaryDirectory(prefix="scr_b2_b_") as tmp:
            with _front_with_stories(tmp) as front:
                # ② accumulate + accum 非空：ctx(story) + stt 喂入 -> listen stop -> 归档一次。
                sid = "sess-b2-acc"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                _engine_to_device(front, sid, [_stt_frame("从前有一只小猫")])
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SCRF-B2 RED(前置): ctx(story)+stt 后未进累积态。")
                dev.append_text(_listen_stop_msg())
                with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                files = _story_md_files(tmp)
                self.assertEqual(len(files), 1,
                                 f"SCRF-B2 RED: accumulate 非空 stop 归档篇数 = "
                                 f"{len(files)}（冻结恰 1）—— 收尾三件在归档路径已消费后"
                                 "二次动作（双取消/双收尾 = 0 篇或重复）。")
                org = _start_organizing_frames(dev.sent_text)
                self.assertEqual(len(org), 1,
                                 f"SCRF-B2 RED: start_organizing 帧数 = {len(org)}（冻结"
                                 "恰 1）—— 收尾演出被收尾三件重复下发。")

    # coverage: SCRF-B2 ③+④ -- accumulate + accum 空 → pending_stop：禁止 cancel / 禁止
    #            释放（protected）；重复 stop → mark_pending 幂等、不动作
    def test_scr_b2_pending_protected_and_repeated_stop_idempotent(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2_pend_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b2-pend"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                cancel_calls = _spy_cancel(acc)
                with _release_spy(front) as rel:
                    dev.append_text(_listen_stop_msg())  # accum 空 -> pending_stop
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B2 RED: accum 空 stop 后未落 pending_stop —— "
                                     "SAO-A1/A2 挂起态破（立即 cancel = 0 篇归档）。")
                    self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                     "SCRF-B2 RED: accum 空 stop 触发 acc.cancel —— "
                                     "protected pending 被当刻取消（F-01 承重破: 在途转写"
                                     "被毁 = 0 篇归档）。")
                    self.assertEqual(rel, [],
                                     "SCRF-B2 RED: accum 空 stop 触发记录语境释放 —— "
                                     "pending_stop 受保护, 当刻禁止释放（交 10s sweep）。")
                    # ④ 重复 stop：幂等 no-op。
                    dev.append_text(_listen_stop_msg())
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B2 RED: 重复 stop 后 pending_stop 态漂移 —— "
                                 "mark_pending 幂等破。")
                self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                 "SCRF-B2 RED: 重复 stop 触发了 cancel —— 幂等 no-op 破。")
                self.assertEqual(_story_md_files(tmp), [],
                                 "SCRF-B2 RED: pending 态产生了归档 —— 未到达 stt 不该成篇。")


# =========================================================================== #
# SCRF-B2b — pending 保护与 sweep 释放                                            #
# =========================================================================== #


class ScrfPendingSweepReleaseAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B2b -- listen stop 后 +9s 零释放、+10s 由 sweep 释放且 active=false
    #            恰一帧入归属腿（腿集 = _session_legs(sid) or list(_alert_legs)）
    def test_scr_b2b_sweep_releases_at_ten_seconds(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2b_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b2b"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                dev.append_text(_listen_stop_msg())  # accum 空 -> pending_stop
                t0 = datetime.now(dt_timezone.utc)
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B2b RED(前置): 未落 pending_stop。")
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    # +9s：仍在 10s 窗内 -> 零释放。
                    with _release_spy(front) as rel9:
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=9)))
                    self.assertEqual(rel9, [],
                                     "SCRF-B2b RED: +9s 就释放了 pending —— 10s 窗口"
                                     "上界（PENDING_STOP_SECONDS=10.0）破。")
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B2b RED: +9s pending 态被提前释放。")
                    # +10.5s：过窗 -> sweep 释放 + 恰一帧 active=false。
                    with _release_spy(front) as rel10:
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=10.5)))
                    self.assertEqual(rel10, [sid],
                                     f"SCRF-B2b RED: +10s sweep 未释放记录语境（实记 "
                                     f"{rel10}）—— pending 超时分支只 release_pending 不调"
                                     "_release_record_context（常驻后设备永久哑火 = 承重"
                                     "缺口; plan T7）。")
                    self.assertEqual(acc.routing(sid), "chat",
                                     "SCRF-B2b RED: sweep 释放后 routing 未回 chat。")
                    rel_frames = _release_frames(up_leg.sent)
                    self.assertEqual(
                        len(rel_frames), 1,
                        f"SCRF-B2b RED: sweep 释放 active=false 帧数 = {len(rel_frames)}"
                        "（冻结恰 1）—— 释放未注入归属引擎腿（腿集须 = _session_legs(sid) "
                        "or list(_alert_legs); 漏发 = 引擎对该连接永久抑制）。")

    # coverage: SCRF-B2b -- 先投 stt 走恢复归档路径则 sweep 不重复释放
    def test_scr_b2b_stt_recovery_prevents_double_release(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2b_rec_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b2b-rec"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                dev.append_text(_listen_stop_msg())
                t0 = datetime.now(dt_timezone.utc)
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B2b RED(前置): 未落 pending_stop。")
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    with _release_spy(front) as rel:
                        # stt 到达 -> pending_stop 恢复归档单源（_story_finish_session 内释放一次）。
                        with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                            up_e = _FakeUpstream(frames=[json.dumps(_stt_frame("从前有一只小猫"),
                                                                    ensure_ascii=False)])
                            dev_e = _FakeDeviceSocket()
                            asyncio.run(front.pump_upstream_to_client(up_e, dev_e, sid))
                        self.assertEqual(len(_story_md_files(tmp)), 1,
                                         "SCRF-B2b RED: pending 恢复路径未成一篇归档。")
                        first = list(rel)
                        # +10s sweep：会话已 release -> pending_timed_out 不再命中 -> 不重复释放。
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=10.5)))
                        self.assertEqual(list(rel), first,
                                         "SCRF-B2b RED: 恢复归档后 sweep 重复释放记录语境"
                                         " —— 双重 active=false（幂等破）。")


# =========================================================================== #
# SCRF-B3 — discard→stop 有序不归档（承重齿）                                      #
# =========================================================================== #


class ScrfDiscardThenStopAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B3 -- 同一 session 依次 discard + listen stop → routing 回 chat、
    #            _story_finish_session 未被调、STORIES_DIR 文件数不变（承重：已丢弃素材
    #            不得被复活成归档）
    def test_scr_b3_discard_then_stop_no_archive(self):
        with tempfile.TemporaryDirectory(prefix="scr_b3_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b3"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                _engine_to_device(front, sid, [_stt_frame("从前有一只小猫")])
                acc = front._get_story_acc()
                self.assertTrue(acc.has_accum(sid),
                                "SCRF-B3 RED(前置): 累积缓冲为空（stt 喂入链回退）。")
                # 同连接同会话：discard 先、listen stop 后（契约 §四顺序约束）。
                dev.append_text(_discard_msg())
                dev.append_text(_listen_stop_msg())
                with _finish_spy(front) as fin:
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-B3 RED: discard 后 routing 未回 chat —— 丢弃未落"
                                 "（丢弃不入册的反面：素材滞留累积态, 稍后被 sweep 归档）。")
                self.assertEqual(fin, [],
                                 f"SCRF-B3 RED: discard→stop 触发了 _story_finish_session"
                                 f"（实记 {fin}）—— 丢弃素材被复活成归档（契约 §四顺序"
                                 "约束破; S-F2.2-AC4 毁稿）。")
                self.assertEqual(_story_md_files(tmp), [],
                                 "SCRF-B3 RED: discard→stop 落盘了故事 —— 「素材不入册」"
                                 "被破（0 篇归档为承重断言）。")


# =========================================================================== #
# SCRF-B4 / B4b — ctx 边沿兜底分态                                                #
# =========================================================================== #


class ScrfCtxEdgeFallbackAcceptance(unittest.TestCase):
    maxDiff = None

    def _seed_marked(self, front, sid, dev, up):
        """同 device websocket 投 ctx(story)：routing accumulate + _record_ctx_seen 置位。"""
        dev.append_text(_ctx_msg("story"))
        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
        self.assertEqual(front._get_story_acc().routing(sid), "accumulate",
                         "SCRF-B4 RED(前置): ctx(story) 后未进累积态。")

    # coverage: SCRF-B4 -- accumulate + 标记真 → cancel(reason=session_end_stale) + 释放；
    #            重复 ctx(非 story) → 零新增注入（幂等）
    def test_scr_b4_accumulate_cancels_and_releases(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4_acc_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4-acc"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    self._seed_marked(front, sid, dev, up)
                    acc = front._get_story_acc()
                    cancel_calls = _spy_cancel(acc)
                    with _release_spy(front) as rel:
                        dev.append_text(_ctx_msg("home"))
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                        self.assertIn((sid, END_STALE_REASON), cancel_calls,
                                      f"SCRF-B4 RED: 非 story ctx 后未落 "
                                      f"cancel(reason={END_STALE_REASON!r})（实记 "
                                      f"{cancel_calls}）—— ctx 边沿兜底未按 accumulate 分态"
                                      "取消（常驻时代 cancel-on-disconnect 同族）。")
                        self.assertEqual(rel, [sid],
                                         f"SCRF-B4 RED: 非 story ctx 后未释放记录语境"
                                         f"（实记 {rel}）—— 引擎对该连接永久抑制（哑火）。")
                        self.assertEqual(acc.routing(sid), "chat",
                                         "SCRF-B4 RED: accumulate 兜底后 routing 未回 chat。")
                        # 重复 ctx(非 story)：标记已清（_release_record_context 单源清位）
                        # -> 零新增注入。
                        rel_after = list(rel)
                        dev.append_text(_ctx_msg("home"))
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                        self.assertEqual(list(rel), rel_after,
                                         "SCRF-B4 RED: 重复 ctx(非 story) 再次释放 —— "
                                         "幂等破（重复 ctx 零新增注入）。")

    # coverage: SCRF-B4 -- pending_stop + 标记真 → 不动作（保护在途转写）
    def test_scr_b4_pending_stop_no_action(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4_pend_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4-pend"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_marked(front, sid, dev, up)
                dev.append_text(_listen_stop_msg())  # accum 空 -> pending_stop
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B4 RED(前置): 未落 pending_stop。")
                cancel_calls = _spy_cancel(acc)
                with _release_spy(front) as rel:
                    dev.append_text(_ctx_msg("home"))
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                    self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                     "SCRF-B4 RED: pending_stop 收到非 story ctx 被 cancel "
                                     "—— 在途转写被毁（T8 保护态破）。")
                    self.assertEqual(rel, [],
                                     "SCRF-B4 RED: pending_stop 收到非 story ctx 被释放 —— "
                                     "当刻禁释放（≤10s 上界交 sweep）。")
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B4 RED: pending_stop 收到非 story ctx 后态漂移。")

    # coverage: SCRF-B4 -- chat 但标记真 → 只释放（不 cancel、不归档）
    def test_scr_b4_chat_marked_only_release(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4_chat_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4-chat"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    self._seed_marked(front, sid, dev, up)
                    acc = front._get_story_acc()
                    # 让 routing 回 chat 而标记仍真：直接 cancel（pop 会话，不触
                    # _release_record_context -> 标记保持真）。
                    acc.cancel(sid)
                    self.assertEqual(acc.routing(sid), "chat",
                                     "SCRF-B4 RED(前置): cancel 后 routing 非 chat。")
                    cancel_calls = _spy_cancel(acc)
                    with _release_spy(front) as rel:
                        dev.append_text(_ctx_msg("home"))
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                        self.assertEqual(rel, [sid],
                                         f"SCRF-B4 RED: chat+标记真未只释放（实记 {rel}）"
                                         " —— 记录语境残活 = 引擎永久抑制。")
                        self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                         "SCRF-B4 RED: chat+标记真分支误 cancel —— "
                                         "只释放不 cancel（会话已不存在）。")

    # coverage: SCRF-B4b -- pending_stop 分支不动作（F-01 ctx 边沿复发防线）+ 随后 stt
    #            仍恰一篇归档
    def test_scr_b4b_pending_stop_edge_then_stt_archives_once(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4b_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4b"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_marked(front, sid, dev, up)
                dev.append_text(_listen_stop_msg())
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B4b RED(前置): 未落 pending_stop。")
                cancel_calls = _spy_cancel(acc)
                with _release_spy(front) as rel:
                    dev.append_text(_ctx_msg("home"))
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                    self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                     "SCRF-B4b RED: ctx 边沿把 pending_stop 取消了 —— F-01"
                                     " 从 ctx 边沿复发（在途转写被毁）。")
                    self.assertEqual(rel, [],
                                     "SCRF-B4b RED: pending_stop 分支触发了释放 —— 保护态破。")
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B4b RED: pending_stop 态被非 story ctx 改掉。")
                # 随后 stt 到达 -> 恢复归档恰一篇。
                with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                    up_e = _FakeUpstream(frames=[json.dumps(_stt_frame("从前有一只小猫"),
                                                            ensure_ascii=False)])
                    dev_e = _FakeDeviceSocket()
                    asyncio.run(front.pump_upstream_to_client(up_e, dev_e, sid))
                files = _story_md_files(tmp)
                self.assertEqual(len(files), 1,
                                 f"SCRF-B4b RED: pending_stop 恢复路径归档篇数 = "
                                 f"{len(files)}（冻结恰 1）—— 保护态守住了但收尾链断（0 篇）"
                                 "或重复成篇。")


# =========================================================================== #
# SCRF-B5 — 连接级 teardown 保绿（must-stay-green）                                #
# =========================================================================== #


class ScrfTeardownAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B5 -- 设备腿断连 + 引擎腿静默 → 限时收尾完成 + 腿双摘除 +
    #            acc.cancel(reason="ws_disconnect") 逐字不变（OQ2 保绿）
    def test_scr_b5_teardown_completes_cancel_and_legs_removed(self):
        with tempfile.TemporaryDirectory(prefix="scr_b5_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                hexval = "b5b5b5b5b5b5b5b5b5b5b5b5b5b5b5b5"
                sid = "sess-" + hexval
                with _gateway_env(front, token, tmp, fake_uuid_hex=hexval):
                    acc = front._get_story_acc()
                    acc.start(sid, now=_t0())
                    acc.feed(sid, "从前有一只小猫", now=_t0())
                    cancel_calls = _spy_cancel(acc)
                    dev = _GatewayDeviceSocket(auth_header="Bearer " + token)
                    up = _SilentUpstream()
                    with _fake_engine_connect(up):
                        try:
                            asyncio.run(asyncio.wait_for(
                                front.ws_gateway(dev), timeout=6.0))
                        except (asyncio.TimeoutError, TimeoutError):
                            self.fail(
                                "SCRF-B5 RED: 设备腿已断连且引擎腿静默, ws_gateway 收尾未"
                                "在限时内完成 —— 心跳 task 进了 FIRST_COMPLETED 集合（无限"
                                "任务拖住收尾; plan T5 承重约束）。")
                    self.assertIn(
                        (sid, WS_DISCONNECT_REASON),
                        [(s, r) for s, r in cancel_calls],
                        f"SCRF-B5 RED: 断连收尾 cancel(ws_disconnect) 未落（实记 "
                        f"{cancel_calls}）—— OQ2 cancel-on-disconnect 维持被破。")
                    self.assertNotIn(sid, acc.active_sessions(),
                                     "SCRF-B5 RED: 断连收尾后会话未 pop —— 累积缓冲泄漏。")
                    self.assertFalse(
                        any(pair[0] is dev for pair in list(front._alert_legs)),
                        "SCRF-B5 RED: 断连收尾后 _alert_legs 仍含本连接腿 —— zombie 腿泄漏。")


# =========================================================================== #
# SCRF-B6 — 既有冻结 oracle 保绿回归网（presence 守卫 + suite 绿留痕）              #
# =========================================================================== #


class ScrfOracleRegressionAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B6 -- 既有冻结 oracle 文件与关键 frozen 测试函数在世（presence 守卫；
    #            行为绿 = 全量 pytest server/acceptance exit 0 留痕，非本单齿）
    def test_scr_b6_frozen_oracles_present(self):
        oracles = {
            "test_m4k1_story_cloud_link.acceptance.py": (
                "server/acceptance/test_m4k1_story_cloud_link.acceptance.py",
                ("sao", "fix_a1"),),
            "test_m4k3_record_session_live.acceptance.py": (
                "server/acceptance/test_m4k3_record_session_live.acceptance.py",
                ("test_a7_ctx_non_story_pushes_zero_frames",
                 "test_a7_push_seam_lands_in_ctx_consume_not_connect"),),
            "test_record_cancel_lifecycle.acceptance.py": (
                "server/acceptance/test_record_cancel_lifecycle.acceptance.py",
                ("first_completed",),),
            "test_record_front_polish.acceptance.py": (
                "server/acceptance/test_record_front_polish.acceptance.py",
                (),),
        }
        for label, (rel, needles) in oracles.items():
            path = REPO_ROOT / rel
            self.assertTrue(path.is_file(),
                            f"SCRF-B6 RED: 既有冻结 oracle {label} 丢失 —— 心跳/收尾三件"
                            "若改了其断言不得静默删件（须走 planner 扩单通道）。")
            text = path.read_text(encoding="utf-8").lower() if path.is_file() else ""
            for needle in needles:
                self.assertIn(needle, text,
                              f"SCRF-B6 RED: {label} 缺关键 frozen 锚 {needle!r} —— "
                              "既有 oracle 被改弱/删除（回归网破; 任一红不得进 integrator）。")


# =========================================================================== #
# SCRF-B7 — 记录语境注入去边缘化（D1）                                            #
# =========================================================================== #


class ScrfRecordCtxUnconditionalAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B7 -- ctx(story) 无条件注入 active=true：routing 已 accumulate
    #            （重复 ctx）时仍恰一帧；story_list 仍守 ctx_push_edge（重复零新增）；
    #            审计 token story_record_ctx_enter 可观测（edge=true / edge=false,
    #            injected=true, src=ctx_story）
    def test_scr_b7_repeat_ctx_story_injects_again_story_list_stays_edge_only(self):
        with tempfile.TemporaryDirectory(prefix="scr_b7_ctx_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b7-ctx"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                with _log_capture() as first_logs:
                    dev.append_text(_ctx_msg("story"))
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                enters = [f for f in _record_ctx_frames(up.sent) if f.get("active") is True]
                self.assertEqual(
                    len(enters), 1,
                    f"SCRF-B7 RED(前置): 首次 ctx(story) 注入 active=true 帧数 = "
                    f"{len(enters)}（冻结恰 1）—— 进语境注入链回退。")
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SCRF-B7 RED(前置): ctx(story) 后未进累积态。")
                story_list_after_first = len(_story_list_frames(dev.sent_text))
                self.assertGreaterEqual(
                    story_list_after_first, 1,
                    "SCRF-B7 RED(前置): 首帧 ctx 边缘未推 story_list（恢复推送缺失——"
                    "本 case 的「重复零新增」前提失真）。")

                # 重复 ctx(story)：routing 已 accumulate（无 push edge）。
                with _log_capture() as second_logs:
                    dev.append_text(_ctx_msg("story"))
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                enters_all = [f for f in _record_ctx_frames(up.sent)
                              if f.get("active") is True]
                self.assertEqual(
                    len(enters_all), 2,
                    f"SCRF-B7 RED: 已在累积态重复 ctx(story) 后 active=true 帧数 = "
                    f"{len(enters_all)}（冻结 2 = 每会话每次 ctx 无条件恰一帧）—— "
                    "注入仍依赖 ctx_push_edge：泄漏会话态 / 引擎腿重建时语境失同步"
                    "（device-round-a-evidence §三 真实缺陷面）。")
                self.assertEqual(
                    len(_story_list_frames(dev.sent_text)), story_list_after_first,
                    "SCRF-B7 RED: 重复 ctx(story) 多推了 story_list —— 边缘推送须仍守 "
                    "ctx_push_edge（契约 §四「重复发送安全」破）。")
                self.assertTrue(
                    _log_lines_with(first_logs, "story_record_ctx_enter",
                                    "src=ctx_story", "edge=true", "injected=true"),
                    "SCRF-B7 RED: 首帧 ctx 注入无可判别审计行 —— 缺 story_record_ctx_enter"
                    "（src=ctx_story / edge=true / injected=true）：三源对账只剩引擎日志，"
                    "误判成本高（device-round-a-evidence §三.2）。")
                self.assertTrue(
                    _log_lines_with(second_logs, "story_record_ctx_enter",
                                    "src=ctx_story", "edge=false", "injected=true"),
                    "SCRF-B7 RED: 重复 ctx 注入的审计行未标 edge=false / injected=true"
                    " —— 审计字段不能区分边缘与幂等重注入（同一 token 单点 emit）。")

    # coverage: SCRF-B7 -- control/start_recording 消费沿同口径（返回语义
    #            enter_edge -> consumed）：已 accumulate 时仍恰一帧 active=true +
    #            src=start_recording 审计行
    def test_scr_b7_start_recording_consumption_injects_when_accumulating(self):
        with tempfile.TemporaryDirectory(prefix="scr_b7_rec_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b7-rec"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SCRF-B7 RED(前置): ctx(story) 后未进累积态。")
                # 语义面：消费即 consumed（不再是「仅 enter_edge」）。
                self.assertTrue(
                    front._story_route_display_payload(sid, _start_recording_block()),
                    "SCRF-B7 RED: _story_route_display_payload 在 routing 已 accumulate "
                    "时仍返回 False —— 返回语义须由 enter_edge 改为 consumed（命中即 "
                    "True；唯一消费者 = 泵内 mark/inject 两处）。")
                with _log_capture() as logs:
                    up2, _dev2 = _engine_to_device(
                        front, sid, [_display_blocks_frame(_start_recording_block())])
                enters = [f for f in _record_ctx_frames(up2.sent)
                          if f.get("active") is True]
                self.assertEqual(
                    len(enters), 1,
                    f"SCRF-B7 RED: 已累积态下 control/start_recording 注入 active=true "
                    f"帧数 = {len(enters)}（冻结恰 1）—— 第二收敛入口仍按边缘注入"
                    "（plan D1：两入口同口径去边缘化）。")
                self.assertTrue(
                    _log_lines_with(logs, "story_record_ctx_enter",
                                    "src=start_recording", "injected=true"),
                    "SCRF-B7 RED: start_recording 注入缺 story_record_ctx_enter 审计行"
                    "（src=start_recording）—— 两收敛入口须共用单点 emit。")

    # coverage: SCRF-B7 -- 审计 token story_record_ctx_enter 单一 emit 点（front_app.py
    #            源文本恰 1 处；辅助信号，行为面由前两 case 承载）
    def test_scr_b7_audit_token_single_emit_point(self):
        src = FRONT_APP.read_text(encoding="utf-8")
        n = src.count("story_record_ctx_enter")
        self.assertEqual(
            n, 1,
            f"SCRF-B7 RED: front_app.py 内 story_record_ctx_enter 出现 {n} 次（冻结恰 1）"
            " —— 多 emit 点会让三源对账字段漂移（单点 emit = 三处调用点共用）。")


# =========================================================================== #
# SCRF-B8 — 丢弃终止在途回合（D2；不发 abort）                                     #
# =========================================================================== #


class ScrfDiscardTerminatesRoundAcceptance(unittest.TestCase):
    maxDiff = None

    def _seed_recording(self, front, sid, dev, up):
        """ctx(story) + stt feed：routing accumulate，累积非空。"""
        dev.append_text(_ctx_msg("story"))
        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
        _engine_to_device(front, sid, [_stt_frame("从前有一只小猫")])
        acc = front._get_story_acc()
        self.assertEqual(acc.routing(sid), "accumulate",
                         "SCRF-B8 RED(前置): ctx(story)+stt 后未进累积态。")
        self.assertTrue(acc.has_accum(sid),
                        "SCRF-B8 RED(前置): 累积缓冲为空（stt 喂入链回退）。")
        return acc

    # coverage: SCRF-B8 -- discard 分支内、返回前经泵内本连接引擎腿直发 active=false：
    #            恰一帧且严格先于随后转发的 listen stop；腿集 = (websocket, upstream)
    #            不回退 _alert_legs；全程零 abort 帧
    def test_scr_b8_discard_releases_context_before_forwarded_stop(self):
        with tempfile.TemporaryDirectory(prefix="scr_b8_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b8"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_recording(front, sid, dev, up)
                # 另一条全局提醒腿：discard 若误用 _alert_legs，active=false 会打到它。
                other_dev, other_up = _FakeDeviceSocket(), _FakeUpstream()
                front._register_alert_legs(other_dev, other_up)
                try:
                    dev.append_text(_discard_msg())
                    dev.append_text(_listen_stop_msg())
                    with _finish_spy(front) as fin:
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                finally:
                    front._unregister_alert_legs(other_dev)

                releases = _frame_indices(up.sent, _is_release_frame)
                stops = _frame_indices(up.sent, _is_listen_stop_frame)
                self.assertEqual(
                    len(releases), 1,
                    f"SCRF-B8 RED: discard 后本连接引擎腿 active=false 帧数 = "
                    f"{len(releases)}（冻结恰 1）—— 丢弃不释放记录语境：引擎在 stop "
                    "触发尾音 ASR 时仍闭锁撤销（device-round-a-evidence §一 22:46:50"
                    "「嗯。」→ TTS 的本体）。")
                self.assertEqual(
                    len(stops), 1,
                    f"SCRF-B8 RED: 经泵转发的 listen stop 帧数 = {len(stops)}（冻结恰 1）。")
                self.assertLess(
                    releases[-1], stops[-1],
                    "SCRF-B8 RED: active=false 未严格先于转发的 listen stop —— 引擎收到 "
                    "stop 时语境仍真（_kidpal_listen_closed 不置位、asr_audio 不截断）→ "
                    "尾音 ASR 成立、在途回合不被终止。")
                self.assertEqual(
                    _frame_indices(other_up.sent, _is_release_frame), [],
                    f"SCRF-B8 RED: active=false 打到了全局 _alert_legs 提醒腿（实记 "
                    f"{other_up.sent!r}）—— discard 释放腿集须 = 泵内本连接 "
                    "[(websocket, upstream)]，多连接时会误放别的引擎腿。")
                abort_frames = [o for o in _forwarded_objects(up.sent) if _is_abort_frame(o)]
                self.assertEqual(
                    abort_frames, [],
                    f"SCRF-B8 RED: 泵发出 abort 帧 {abort_frames!r} —— 本轮决策 = 不发 "
                    "abort（轮 A 实证 abort 已收仍产 TTS；引擎冻结纪律四处禁置 "
                    "client_abort，防波及 front 合法 speak 注入）。")
                self.assertEqual(fin, [],
                                 f"SCRF-B8 RED: discard→stop 触发了 _story_finish_session"
                                 f"（实记 {fin}）—— 已丢弃素材被复活成归档。")
                self.assertEqual(_story_md_files(tmp), [],
                                 "SCRF-B8 RED: discard→stop 落盘了故事（0 篇为承重）。")

    # coverage: SCRF-B8 -- 重复 stop 零第二次注入（清标记副作用承载幂等）；零 abort；
    #            全链零归档
    def test_scr_b8_repeated_stop_injects_no_second_release(self):
        with tempfile.TemporaryDirectory(prefix="scr_b8_idem_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b8-idem"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_recording(front, sid, dev, up)
                dev.append_text(_discard_msg())
                dev.append_text(_listen_stop_msg())
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                releases_after_first = len(_frame_indices(up.sent, _is_release_frame))
                self.assertEqual(
                    releases_after_first, 1,
                    f"SCRF-B8 RED(前置): discard 后本连接引擎腿 active=false 帧数 = "
                    f"{releases_after_first}（冻结恰 1）—— 首次释放缺失时「重复 stop 零"
                    "第二次注入」不可判（幂等前提失真）。")
                dev.append_text(_listen_stop_msg())   # 重复 stop
                with _finish_spy(front) as fin:
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(
                    len(_frame_indices(up.sent, _is_release_frame)), releases_after_first,
                    "SCRF-B8 RED: 重复 listen stop 又注入了一次 active=false —— 幂等破"
                    "（清标记单点 _release_record_context 未生效 / 另写了计数器）。")
                self.assertEqual(fin, [],
                                 f"SCRF-B8 RED: 重复 stop 触发了 _story_finish_session"
                                 f"（实记 {fin}）。")
                self.assertEqual(
                    [o for o in _forwarded_objects(up.sent) if _is_abort_frame(o)], [],
                    "SCRF-B8 RED: 重复 stop 路径发出 abort 帧。")

    # coverage: SCRF-B8 -- discard 分支腿集不得回退 _alert_legs（源文本负锚；
    #            行为面由 discard_releases_context_before_forwarded_stop 承载）
    def test_scr_b8_discard_branch_has_no_global_alert_legs(self):
        src = FRONT_APP.read_text(encoding="utf-8")
        start = src.find('if mtype == "kidpal_story_discard":')
        self.assertGreaterEqual(
            start, 0,
            "SCRF-B8 RED(infra): front_app.py 无 kidpal_story_discard 分支——消费面丢失。")
        nxt = src.find("\n    if mtype == ", start + 1)
        region = src[start:] if nxt == -1 else src[start:nxt]
        self.assertNotIn(
            "_alert_legs", region,
            "SCRF-B8 RED: discard 分支出现 _alert_legs —— 释放腿集回退成全局提醒腿"
            "（多连接时误放别的引擎腿；冻结形态 = 泵内本连接 [(websocket, upstream)]）。")

    # coverage: SCRF-B8 对照齿（must-stay-green）-- ctx+feed+stop（不发 discard）→ 恰一篇
    #            归档 + start_organizing 恰一帧，且 active=false 出现在 stop **之后**
    #            （KEY 收尾「stop 时语境仍真」是 pending-stop 归档承重通道；防修过头）
    def test_scr_b8_key_stop_still_archives_once_and_releases_after_stop(self):
        with tempfile.TemporaryDirectory(prefix="scr_b8_key_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b8-key"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_recording(front, sid, dev, up)
                dev.append_text(_listen_stop_msg())   # 不发 discard
                with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                files = _story_md_files(tmp)
                self.assertEqual(
                    len(files), 1,
                    f"SCRF-B8 RED(对照齿): KEY 收尾（ctx+feed+stop）归档篇数 = "
                    f"{len(files)}（must-stay-green 恰 1）—— D2 修过头毁掉了 KEY 尾音"
                    "归档（S-F2.2-AC4 三式收尾承重通道）。")
                org = _start_organizing_frames(dev.sent_text)
                self.assertEqual(
                    len(org), 1,
                    f"SCRF-B8 RED(对照齿): start_organizing 帧数 = {len(org)}（must-stay-"
                    "green 恰 1）。")
                releases = _frame_indices(up.sent, _is_release_frame)
                stops = _frame_indices(up.sent, _is_listen_stop_frame)
                self.assertEqual(
                    len(releases), 1,
                    f"SCRF-B8 RED(对照齿): KEY 收尾 active=false 帧数 = {len(releases)}"
                    "（must-stay-green 恰 1）。")
                self.assertEqual(len(stops), 1,
                                 f"SCRF-B8 RED(对照齿): listen stop 转发帧数 = {len(stops)}。")
                self.assertGreater(
                    releases[-1], stops[-1],
                    "SCRF-B8 RED(对照齿): KEY 收尾的 active=false 早于 listen stop —— "
                    "注入被提前到 stop 之前 = 引擎在尾音 ASR 阶段已解除闭锁，KEY 收尾"
                    "归档路径被改坏（对照齿防「一律先释放」的过度修复）。")


# =========================================================================== #
# SCRF-B9 — 会话关闭门：迟到 tts/llm/音频丢弃（D2 迟到帧）                          #
# =========================================================================== #


class ScrfClosedSessionFrameGateAcceptance(unittest.TestCase):
    maxDiff = None

    def _arm_gate(self, front, sid, dev, up):
        """经出语境单源 arm（生产路径 = discard / 收尾 / sweep 释放同一单点）。"""
        asyncio.run(front._release_record_context(sid, [(dev, up)]))

    def _pump_down(self, front, sid, dev, frames):
        """引擎腿 → 设备腿跑一次泵；返回新转发文本帧与二进制帧（按基线切片）。"""
        base_text, base_bytes = len(dev.sent_text), len(dev.sent_bytes)
        up_e = _FakeUpstream(frames=frames)
        asyncio.run(front.pump_upstream_to_client(up_e, dev, sid))
        return dev.sent_text[base_text:], dev.sent_bytes[base_bytes:]

    # coverage: SCRF-B9 -- 门内丢弃引擎腿→设备腿的二进制音频帧 + type in ("tts","llm")
    #            文本帧；不拦 type=="mcp" 与 stt（arm 点 = discard 的真实生产路径）
    def test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt(self):
        with tempfile.TemporaryDirectory(prefix="scr_b9_drop_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b9-drop"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                _engine_to_device(front, sid, [_stt_frame("从前有一只小猫")])
                dev.append_text(_discard_msg())      # discard -> _release_record_context
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))

                new_text, new_bytes = self._pump_down(front, sid, dev, [
                    AUDIO_BYTES,
                    json.dumps(_tts_frame("sentence_start", "嗯～"), ensure_ascii=False),
                    json.dumps(_tts_frame("stop"), ensure_ascii=False),
                    json.dumps(_llm_frame(), ensure_ascii=False),
                    json.dumps(_stt_frame("嗯。"), ensure_ascii=False),
                    json.dumps(_mcp_probe_frame(), ensure_ascii=False),
                ])
                self.assertEqual(
                    new_bytes, [],
                    "SCRF-B9 RED: 会话关闭门内仍转发了引擎腿二进制音频帧 —— 迟到回合"
                    "的 TTS 音频把已回 Idle 的设备拽回 Speaking（device-round-a-evidence "
                    "§一 22:46:53 `<< 嗯～` + idle->speaking 同族）。")
                tts_fwd = _frames_of_type(new_text, "tts")
                self.assertEqual(
                    tts_fwd, [],
                    f"SCRF-B9 RED: 门内转发了 tts 文本帧 {tts_fwd!r} —— start/"
                    "sentence_start/stop 皆须丢弃（否则设备进 Speaking 态、页面被迟到"
                    "回合顶掉）。")
                self.assertEqual(
                    _frames_of_type(new_text, "llm"), [],
                    "SCRF-B9 RED: 门内转发了 llm 帧 —— 情绪/同步文字迟到达（同族漏网）。")
                self.assertEqual(
                    len(_frames_of_type(new_text, "stt")), 1,
                    "SCRF-B9 RED: 门把 stt 也拦了 —— 转写须照常上屏/喂入（拒绝路径"
                    "「嗯。」样本的可见性也靠它）。")
                self.assertEqual(
                    len(_frames_of_type(new_text, "mcp")), 1,
                    "SCRF-B9 RED: 门把 type=mcp 也拦了 —— alert/story_text/story_list "
                    "等机械下发不得被迟到帧门误伤（S-F1.4 必达面）。")

    # coverage: SCRF-B9 -- 下一次 listen start / ctx(story) disarm（门只关本轮；
    #            下一会话的 TTS 必须恢复转发）
    def test_scr_b9_gate_disarms_on_next_listen_start_and_ctx_story(self):
        # ① listen start 解除
        with tempfile.TemporaryDirectory(prefix="scr_b9_disarm_l_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b9-disarm-listen"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                self._arm_gate(front, sid, dev, up)
                text0, _b0 = self._pump_down(front, sid, dev, [
                    json.dumps(_tts_frame("sentence_start", "嗯。"), ensure_ascii=False)])
                self.assertEqual(
                    _frames_of_type(text0, "tts"), [],
                    "SCRF-B9 RED(前置): 门未 arm —— 门内 tts 仍被转发"
                    "（arm 点 = _release_record_context 单点）。")
                dev.append_text(json.dumps({"type": "listen", "state": "start"},
                                           ensure_ascii=False))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                text1, _b1 = self._pump_down(front, sid, dev, [
                    json.dumps(_tts_frame("sentence_start", "第二句。"), ensure_ascii=False)])
                self.assertEqual(
                    len(_frames_of_type(text1, "tts")), 1,
                    "SCRF-B9 RED: 下一次 listen start 未解除会话关闭门 —— 新会话的 TTS "
                    "被永久丢弃（设备永久哑火，比迟到帧更重）。")
        # ② ctx(story) 解除
        with tempfile.TemporaryDirectory(prefix="scr_b9_disarm_ctx_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b9-disarm-ctx"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                self._arm_gate(front, sid, dev, up)
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                text2, _b2 = self._pump_down(front, sid, dev, [
                    json.dumps(_tts_frame("sentence_start", "引导句。"), ensure_ascii=False)])
                self.assertEqual(
                    len(_frames_of_type(text2, "tts")), 1,
                    "SCRF-B9 RED: 下一次 ctx(story) 未解除会话关闭门 —— 新记录会话的"
                    "引导播报被丢弃（引导链哑掉）。")

    # coverage: SCRF-B9 -- front 自注 kidpal_speak 令牌：有界放行整轮（含二进制音频与
    #            tts stop；先判令牌后清令牌——stop 帧本身放行，之后 tts 重新被丢弃）
    def test_scr_b9_speak_token_allows_round_then_tts_stop_clears(self):
        with tempfile.TemporaryDirectory(prefix="scr_b9_token_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b9-token"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                self._arm_gate(front, sid, dev, up)
                # front 自注 kidpal_speak（引导/安抚通道）-> 该连接登记「预期出音」令牌。
                sent = asyncio.run(front._speak_to_legs("该讲故事啦。", [(dev, up)]))
                self.assertEqual(
                    sent, 1,
                    f"SCRF-B9 RED(前置): kidpal_speak 注入帧数 = {sent}（冻结恰 1）"
                    " —— 令牌登记点的前置注入链回退。")
                text1, bytes1 = self._pump_down(front, sid, dev, [
                    json.dumps(_tts_frame("sentence_start", "该讲故事啦。"),
                               ensure_ascii=False),
                    AUDIO_BYTES,
                    json.dumps(_tts_frame("stop"), ensure_ascii=False),
                ])
                self.assertEqual(
                    len(_frames_of_type(text1, "tts")), 2,
                    f"SCRF-B9 RED: 令牌在场时本轮 tts 帧放行数 = "
                    f"{len(_frames_of_type(text1, 'tts'))}（冻结 2 = sentence_start + "
                    "stop）—— 令牌未登记或「先清后判」把 stop 帧吞掉（设备 Speaking "
                    "悬挂）。")
                self.assertEqual(
                    len(bytes1), 1,
                    f"SCRF-B9 RED: 令牌在场时本轮二进制音频帧放行数 = {len(bytes1)}"
                    "（冻结 1）—— 整轮不放行 = 提醒/安抚语音听不见。")
                # stop 已清令牌：随后的 tts 重新按关门丢弃。
                text2, bytes2 = self._pump_down(front, sid, dev, [
                    json.dumps(_tts_frame("sentence_start", "漏网句。"),
                               ensure_ascii=False),
                    AUDIO_BYTES,
                ])
                self.assertEqual(
                    _frames_of_type(text2, "tts"), [],
                    "SCRF-B9 RED: tts stop 后令牌未清 —— 迟到回合的后续 tts 继续放行"
                    "（「有界」破）。")
                self.assertEqual(
                    bytes2, [],
                    "SCRF-B9 RED: tts stop 后令牌未清 —— 后续二进制音频继续放行。")

    # coverage: SCRF-B9 -- 令牌登记点覆盖提醒 speech（_deliver_alert_now；
    #            reminders.py 引擎腿注入不经 _speak_to_legs）：提醒弹窗必达 + 语音帧
    #            在门内不被误丢
    def test_scr_b9_alert_speech_registers_expected_round_token(self):
        with tempfile.TemporaryDirectory(prefix="scr_b9_alert_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b9-alert"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                self._arm_gate(front, sid, dev, up)
                front._register_alert_legs(dev, up)
                try:
                    entry = {"id": "biz-b9", "fire_at": "2026-11-17T08:00:00+00:00",
                             "title": "到点提醒", "speech": "时间到了哦",
                             "view": {"view": "text", "data": {"text": "到点提醒"}}}
                    envelope = front.gw_reminders.build_alert_envelope(entry)

                    async def _deliver():
                        front._deliver_alert_now("biz-b9", envelope)
                        pending = list(getattr(front, "_alert_delivery_tasks", ()))
                        if pending:
                            await asyncio.gather(*pending, return_exceptions=True)

                    asyncio.run(_deliver())
                finally:
                    front._unregister_alert_legs(dev)
                self.assertTrue(
                    _frames_of_type(dev.sent_text, "mcp"),
                    "SCRF-B9 RED(前置): 提醒 mcp 帧未达设备腿 —— 两腿投递链回退"
                    "（S-F1.4 必达面本 case 的前提）。")
                text, _b = self._pump_down(front, sid, dev, [
                    json.dumps(_tts_frame("sentence_start", "时间到了哦"),
                               ensure_ascii=False),
                ])
                self.assertEqual(
                    len(_frames_of_type(text, "tts")), 1,
                    "SCRF-B9 RED: 提醒 speech 登记点未覆盖（_deliver_alert_now）—— "
                    "会话关闭门把提醒语音 TTS 当迟到回合丢弃（语音丢失）。reminders.py "
                    "引擎腿注入不经 _speak_to_legs，须在 _deliver_alert_now 另行登记令牌。")


# =========================================================================== #
# SCRF-B10 — 空素材收尾=释放口径（D3 front 半；预期设计态绿 = must-stay-green）      #
# =========================================================================== #


class ScrfEmptyMaterialReleaseAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B10 -- 空素材 stop（accum 空）→ pending_stop → 10s sweep 释放 +
    #            active=false 恰一帧；_story_finish_session 零调用、STORIES_DIR 零新增、
    #            零 start_organizing、零新增 story_list
    def test_scr_b10_empty_material_stop_releases_without_archive(self):
        with tempfile.TemporaryDirectory(prefix="scr_b10_empty_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b10-empty"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                story_list_after_ctx = len(_story_list_frames(dev.sent_text))
                dev.append_text(_listen_stop_msg())   # accum 空 -> pending_stop
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B10 RED(前置): 空素材 stop 后未落 pending_stop。")
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                story_list_after_ctx_leg = len(_story_list_frames(dev_leg.sent_text))
                t0 = datetime.now(dt_timezone.utc)
                with _guide_legs(front, sid, dev_leg, up_leg):
                    with _finish_spy(front) as fin:
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=10.5)))
                self.assertEqual(
                    fin, [],
                    f"SCRF-B10 RED: 空素材 pending 超时走了 _story_finish_session"
                    f"（实记 {fin}）—— 空素材收尾 = 释放、不归档、不演出、不盖章"
                    "（device-round-a-evidence §二 判据）。")
                releases = _release_frames(up_leg.sent)
                self.assertEqual(
                    len(releases), 1,
                    f"SCRF-B10 RED: 空素材 sweep 释放 active=false 帧数 = {len(releases)}"
                    "（冻结恰 1）—— 释放缺失 = 引擎对该连接永久抑制（后续问答哑火）。")
                self.assertEqual(
                    _story_md_files(tmp), [],
                    "SCRF-B10 RED: 空素材收尾落盘了故事 —— 空素材零归档破。")
                self.assertEqual(
                    _start_organizing_frames(dev.sent_text), [],
                    "SCRF-B10 RED: 空素材收尾下发了 start_organizing —— 无内容不该演出"
                    "（设备困在整合过渡页）。")
                self.assertEqual(
                    len(_story_list_frames(dev.sent_text)), story_list_after_ctx,
                    "SCRF-B10 RED: 空素材收尾多推了 story_list —— 列表对账帧不该在"
                    "「没有新篇」时出现。")
                self.assertEqual(
                    len(_story_list_frames(dev_leg.sent_text)),
                    story_list_after_ctx_leg,
                    "SCRF-B10 RED: 空素材收尾向已登记会话设备腿多推了 story_list —— "
                    "列表对账帧不该在「没有新篇」时出现（sweep 释放按 "
                    "_session_legs(sid) or _alert_legs 投递，帧落会话腿而非外层设备；"
                    "仅观测外层会漏掉会话腿侧的新增）。")
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-B10 RED: sweep 释放后 routing 未回 chat。")

    # coverage: SCRF-B10 -- 尾音幻听（「嗯。」）走 _archive_material_reject_reason 拒绝
    #            路径同口径：finish 路径被消费但零归档 / 零演出 / 恰一帧 active=false
    def test_scr_b10_tail_hallucination_rejected_same_release_contract(self):
        with tempfile.TemporaryDirectory(prefix="scr_b10_halluc_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b10-halluc"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                dev.append_text(_listen_stop_msg())   # accum 空 -> pending_stop
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B10 RED(前置): 未落 pending_stop。")
                with _finish_spy(front) as fin:
                    up_e, dev_e = _engine_to_device(front, sid, [_stt_frame("嗯。")])
                self.assertEqual(
                    len(fin), 1,
                    f"SCRF-B10 RED: 尾音幻听未进入收尾单源（实记 {fin}）—— 拒绝路径"
                    "须与空素材同口径（收尾链被消费 + 内容守卫拒绝）。")
                releases = _release_frames(up_e.sent)
                self.assertEqual(
                    len(releases), 1,
                    f"SCRF-B10 RED: 拒绝路径 active=false 帧数 = {len(releases)}（冻结"
                    "恰 1）—— 会话已释放但引擎语境标志尚活（后续对话被抑制）。")
                self.assertEqual(
                    _story_md_files(tmp), [],
                    "SCRF-B10 RED: 尾音幻听成篇 —— _archive_material_reject_reason"
                    "（insignificant_material / pure_hallucination_material）破。")
                self.assertEqual(
                    _start_organizing_frames(dev_e.sent_text), [],
                    "SCRF-B10 RED: 拒绝路径下发了 start_organizing —— 纯幻听不该演出。")
                self.assertEqual(
                    _story_list_frames(dev_e.sent_text), [],
                    "SCRF-B10 RED: 拒绝路径推送了 story_list —— 无新篇时列表不应变。")
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-B10 RED: 拒绝路径后 routing 未回 chat。")


# --------------------------------------------------------------------------- #
# 真机轮 C 修复轮（2026-11-18）追加 helpers（SCRF-C1..C4）                        #
# --------------------------------------------------------------------------- #

# 冻结 token（plan「真机轮 C 修复轮」三节 + TaskPackage SCRF-C1..C4）。
SILENT_ASR_DROP_TOKEN = "silent_asr_hallucination"
ORGANIZE_PLACEHOLDER_REASON = "organize_placeholder"

_GUARD_STRIP_RE_TEST = re.compile(r"[^\w\s]")


def _guard_compact(text):
    """素材守卫同族归一化（去标点 -> 去空白 -> 小写）；集合不变式的度量面。

    与 front `_archive_material_reject_reason` 的 compact 形态同构：段级集合若收进
    compact 后 <2 字的元素（如「嗯。」->「嗯」），ingest 会把它吞掉，SCRF-B10 case 2
    的「尾音幻听仍进收尾单源被素材守卫拒绝」随之翻红——故不变式必须按本度量钉。"""
    spaced = _GUARD_STRIP_RE_TEST.sub("", text or "").strip().lower()
    return re.sub(r"\s+", "", spaced)


def _story_text_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_text"]


def _kidpal_speak_frames(sent):
    return [o for o in _forwarded_objects(sent) if o.get("type") == "kidpal_speak"]


def _ledger_snapshot(stories_dir):
    """<STORIES_DIR>/.seq.json 读回（缺失/坏 JSON -> {}，供「seq 不占号」断言）。"""
    path = Path(stories_dir) / ".seq.json"
    try:
        raw = path.read_text(encoding="utf-8")
    except OSError:
        return {}
    try:
        return json.loads(raw)
    except (TypeError, ValueError):
        return {}


# =========================================================================== #
# SCRF-C1 + SCRF-C4 — 段级静音 ASR 幻觉过滤（主防线）+ 判据判别力/集合不变式        #
# =========================================================================== #


class ScrfSilentAsrSegmentFilterAcceptance(unittest.TestCase):
    maxDiff = None

    # 命中族（整段归一化后与集合整串相等才 drop；绝不子串）。
    HIT_SAMPLES = ("Okay.", "okay", "OKAY！", "ok", "Thank you.",
                   "うん。", "そう。", "はい。", "いいえ。")
    # 混排真话（孩子真会说的话）——必须照常入累积器。
    KEEP_SAMPLES = ("嗯，然后我们去公园", "Okay，我知道了",
                    "谢谢你，这个字怎么写", "让我们一起说thank you吧")

    # coverage: SCRF-C1 -- 记录语境 ctx(story) -> listen stop（累积空 -> pending_stop，
    #            分层保护零回退）-> 引擎静音幻觉 stt=Okay. 被段级过滤：_story_finish_session
    #            零调用、累积器仍空、routing 仍 pending_stop；+10.5s 既有 sweep 释放
    #            （active=false 恰一帧、零归档 / 零 start_organizing / 零 story_text /
    #            story_list 零新增 / seq 不占号）；审计 dropped=silent_asr_hallucination
    #            恰 1；stt 转发不受过滤影响（恰 1 帧——SCRF-B9 旁路消费语义延伸）
    def test_scr_c1_pending_stop_silent_asr_dropped_then_sweep_releases(self):
        with tempfile.TemporaryDirectory(prefix="scr_c1_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-c1"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SCRF-C1 RED(前置): ctx(story) 后未进累积态 —— 本 case "
                                 "的 pending_stop 起点不可达（夹具失真）。")
                story_list_base = len(_story_list_frames(dev.sent_text))
                t0 = datetime.now(dt_timezone.utc)
                with _log_capture() as stop_logs:
                    dev.append_text(_listen_stop_msg())   # 累积空 -> pending_stop
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(
                    acc.routing(sid), "pending_stop",
                    "SCRF-C1 RED(前置): 空累积 stop 后未落 pending_stop —— 分层保护"
                    "（story_stop pending_stop / session_end_boundary protected）回退。")
                self.assertTrue(
                    _log_lines_with(stop_logs, "story_stop", "archived=false",
                                    "reason=pending_stop"),
                    "SCRF-C1 RED(前置): 缺 story_stop archived=false reason=pending_stop "
                    "审计行 —— 真机轮 C 证据（21:22:05）的挂起留痕链断了。")
                self.assertTrue(
                    _log_lines_with(stop_logs, "session_end_boundary",
                                    "routing=pending_stop", "action=protected"),
                    "SCRF-C1 RED(前置): 缺 session_end_boundary routing=pending_stop "
                    "action=protected 审计行 —— F-01 分层保护留痕链断了。")

                # 引擎静音 ASR 幻觉到达（真机轮 C 21:22:06 逐字样本）。
                with _log_capture() as stt_logs:
                    with _finish_spy(front) as fin:
                        up_e, dev_e = _engine_to_device(front, sid, [_stt_frame("Okay.")])
                self.assertEqual(
                    fin, [],
                    f"SCRF-C1 RED: 静音幻觉 stt=Okay. 触发了 _story_finish_session"
                    f"（实记 {fin}）—— 段级过滤缺席：幻觉喂进 pending_stop 累积器、过"
                    "素材守卫（compact=4）、进整理回合成垃圾篇（round-c "
                    "ecs-story-2026-11-18-s33477cee.md 逐字「（没有可整理的内容）」）。")
                self.assertIsNone(
                    acc.build_organizing_input(sid),
                    "SCRF-C1 RED: 幻觉 stt 仍进了累积缓冲（build_organizing_input 非 "
                    "None）—— 段级过滤须在 feed 之前，整段命中即 drop（绝不子串）。")
                self.assertEqual(
                    acc.routing(sid), "pending_stop",
                    "SCRF-C1 RED: 幻觉 stt 后 routing 漂移（非 pending_stop）—— 被过滤"
                    "不得改变挂起态（交既有 10s sweep 释放，防吞 1.7s 后才到的真转写）。")
                self.assertEqual(
                    len(_frames_of_type(dev_e.sent_text, "stt")), 1,
                    "SCRF-C1 RED: stt 转发帧数 != 1 —— 段级过滤须只作用于旁路素材消费，"
                    "不得挪到转发路径之前或 continue（SCRF-B9 语义延伸）。")
                silent_lines = _log_lines_with(stt_logs, "story_feed_guard",
                                               f"dropped={SILENT_ASR_DROP_TOKEN}")
                self.assertEqual(
                    len(silent_lines), 1,
                    f"SCRF-C1 RED: 审计 story_feed_guard dropped="
                    f"{SILENT_ASR_DROP_TOKEN} 行数 = {len(silent_lines)}（冻结恰 1）"
                    f"—— 命中不可归因（实记 {stt_logs!r}）。")

                # 既有 10s sweep 释放链（不新增即时释放路径）。
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    with _finish_spy(front) as fin2:
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=10.5)))
                self.assertEqual(
                    fin2, [],
                    f"SCRF-C1 RED: sweep 释放走了 _story_finish_session（实记 {fin2}）"
                    "—— 空素材收尾 = 释放、不归档、不演出、不盖章。")
                releases = _release_frames(up_leg.sent)
                self.assertEqual(
                    len(releases), 1,
                    f"SCRF-C1 RED: sweep 释放 active=false 帧数 = {len(releases)}（冻结"
                    "恰 1）—— 释放缺失 = 引擎对该连接永久抑制（后续问答哑火）。")
                self.assertEqual(
                    _story_md_files(tmp), [],
                    "SCRF-C1 RED: 幻觉素材落盘成篇 —— STORIES_DIR 须零新增。")
                self.assertEqual(
                    _ledger_snapshot(tmp), {},
                    "SCRF-C1 RED: 幻觉素材占了 seq 账本号 —— 零盖章（不归档则不分配"
                    "流水号）被破。")
                self.assertEqual(
                    _start_organizing_frames(dev.sent_text), [],
                    "SCRF-C1 RED: 过滤路径下发了 start_organizing —— 会话从未进整理"
                    "回合，不该演出（设备困在整合过渡页）。")
                self.assertEqual(
                    len(_story_list_frames(dev.sent_text)), story_list_base,
                    "SCRF-C1 RED: 过滤路径多推了 story_list —— 该路径精确兑现 human"
                    "「不推 story_list」原话（列表total 由 list_stories 现算、零归档即"
                    "零变化）。")
                self.assertEqual(
                    _story_list_frames(dev_leg.sent_text), [],
                    "SCRF-C1 RED: sweep 释放向会话腿推了 story_list —— 释放不是数据"
                    "更新，不该有列表帧。")
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-C1 RED: sweep 释放后 routing 未回 chat。")

    # coverage: SCRF-C4 -- 命中族（段级 drop）与混排真话放行成对：Okay. / okay /
    #            OKAY！ / ok / Thank you. / うん。 / そう。 / はい。 / いいえ。 整段
    #            命中即 drop（不进累积器）；「嗯。」不命中（compact=1，留给素材守卫）；
    #            混排真话四条全部入累积器；三段式（真话 -> Okay. -> 真话）恰含两句真话
    def test_scr_c4_hit_family_dropped_and_mixed_speech_kept(self):
        with tempfile.TemporaryDirectory(prefix="scr_c4_keep_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                # ① 命中族：逐样本独立会话，routing 必须真在累积态（否则喂入链不达，
                #    断言会空过 —— 先钉前置）。
                with _log_capture() as hit_logs:
                    for idx, sample in enumerate(self.HIT_SAMPLES):
                        sid = f"sess-c4-hit-{idx}"
                        acc.start(sid, now=_t0())
                        self.assertEqual(acc.routing(sid), "accumulate",
                                         f"SCRF-C4 RED(前置): 样本 {sample!r} 会话未进"
                                         "累积态（喂入链不达 = 断言空过）。")
                        front._story_feed_from_stt(sid, _stt_frame(sample), now=_t0())
                        self.assertIsNone(
                            acc.build_organizing_input(sid),
                            f"SCRF-C4 RED: 静音幻觉样本 {sample!r} 未被段级过滤 —— "
                            "整段归一化后与集合整串相等即须 drop（绝不子串）。")
                hit_lines = _log_lines_with(hit_logs, "story_feed_guard",
                                            f"dropped={SILENT_ASR_DROP_TOKEN}")
                self.assertEqual(
                    len(hit_lines), len(self.HIT_SAMPLES),
                    f"SCRF-C4 RED: 命中族审计行数 = {len(hit_lines)}（冻结 "
                    f"{len(self.HIT_SAMPLES)} = 每样本恰 1）—— 命中不可逐条归因。")

                # ② 「嗯。」不命中：仍入累积器（SCRF-B10 case 2 保绿的机械依据）。
                sid = "sess-c4-en"
                acc.start(sid, now=_t0())
                with _log_capture() as en_logs:
                    ret = front._story_feed_from_stt(sid, _stt_frame("嗯。"), now=_t0())
                self.assertFalse(ret,
                                 "SCRF-C4 RED: 「嗯。」在累积态被当作收尾沿返回 True —— "
                                 "该样本的拒绝归素材守卫（insignificant_material），归"
                                 "段级过滤会毁 SCRF-B10 case 2。")
                self.assertIn(
                    "嗯。", acc.build_organizing_input(sid) or "",
                    "SCRF-C4 RED: 「嗯。」被段级集合吞掉 —— compact=1 <2 的元素不得入"
                    "段级集合（B10 case 2「尾音仍进收尾单源」的机械依据）。")
                self.assertEqual(
                    _log_lines_with(en_logs, "story_feed_guard",
                                    f"dropped={SILENT_ASR_DROP_TOKEN}"), [],
                    "SCRF-C4 RED: 「嗯。」命中了段级过滤 —— 见集合不变式"
                    "（compact 后长度 >= 2）。")

                # ③ 混排真话放行：四条全部入累积器（绝不子串）。
                sid = "sess-c4-mixed"
                acc.start(sid, now=_t0())
                for sample in self.KEEP_SAMPLES:
                    front._story_feed_from_stt(sid, _stt_frame(sample), now=_t0())
                built = acc.build_organizing_input(sid) or ""
                for sample in self.KEEP_SAMPLES:
                    self.assertIn(
                        sample, built,
                        f"SCRF-C4 RED: 混排真话 {sample!r} 被段级过滤误杀 —— 过滤只在"
                        "整段相等时 drop（human 2026-11-18：不得误伤孩子真说的话）。")

                # ④ 三段式：真话 -> Okay. -> 真话；恰含两句真话、不含 Okay。
                sid = "sess-c4-triple"
                acc.start(sid, now=_t0())
                front._story_feed_from_stt(sid, _stt_frame("今天我去公园了。"), now=_t0())
                front._story_feed_from_stt(sid, _stt_frame("Okay."), now=_t0())
                front._story_feed_from_stt(sid, _stt_frame("我们玩了滑梯。"), now=_t0())
                built = acc.build_organizing_input(sid) or ""
                self.assertIn("今天我去公园了。", built,
                              "SCRF-C4 RED: 三段式第一句真话丢失。")
                self.assertIn("我们玩了滑梯。", built,
                              "SCRF-C4 RED: 三段式第三句真话丢失。")
                self.assertNotIn(
                    "Okay", built,
                    "SCRF-C4 RED: 三段式中间段的静音幻觉「Okay.」进了整理输入 —— "
                    "段级过滤须在 feed 之前逐段判定。")

    # coverage: SCRF-C4 -- 集合不变式（_FRONT_SILENT_ASR_HALLUCINATIONS 在世、非空、
    #            set/frozenset；每元素归一化去空白后 >= 2 且守卫 compact 后 >= 2）+
    #            表内每个元素行为上确实被 drop + 判定单源（monkeypatch 模块常量后判定随动）
    def test_scr_c4_segment_set_invariant_and_single_source(self):
        with tempfile.TemporaryDirectory(prefix="scr_c4_set_") as tmp:
            with _front_with_stories(tmp) as front:
                table = getattr(front, "_FRONT_SILENT_ASR_HALLUCINATIONS", None)
                self.assertIsNotNone(
                    table,
                    "SCRF-C4 RED: front_app 无模块级 _FRONT_SILENT_ASR_HALLUCINATIONS"
                    " —— 段级静音幻觉集合未落（与 _FRONT_ARCHIVE_HALLUCINATIONS 独立"
                    "成表，不得 union：union 会改变素材守卫单字语气词语义）。")
                self.assertIsInstance(
                    table, (set, frozenset),
                    f"SCRF-C4 RED: 段级集合类型 = {type(table)!r}（冻结 set/frozenset）"
                    " —— 列表/元组形态会让「判定单源换表」语义失真。")
                self.assertTrue(table, "SCRF-C4 RED: 段级集合为空 —— 过滤面恒不命中。")
                for element in table:
                    self.assertIsInstance(
                        element, str,
                        f"SCRF-C4 RED: 段级集合元素非 str：{element!r}。")
                    self.assertGreaterEqual(
                        len(re.sub(r"\s+", "", element)), 2,
                        f"SCRF-C4 RED: 段级集合元素 {element!r} 归一化去空白后长度 < 2"
                        " —— 冻结不变式（集合 = 素材守卫 <2 覆盖不到的精确补集）。")
                    self.assertGreaterEqual(
                        len(_guard_compact(element)), 2,
                        f"SCRF-C4 RED: 段级集合元素 {element!r} 守卫 compact 后长度 < 2"
                        " —— 该类元素（如「嗯。」->「嗯」）会被 ingest 吞掉、使 SCRF-B10 "
                        "case 2「尾音幻听仍进收尾单源被素材守卫拒绝」翻红。")

                # 表内每个元素行为上确实被 drop（表与判定同源，防「表装饰、判定硬编码」）。
                acc = front._get_story_acc()
                for idx, element in enumerate(sorted(table)):
                    sid = f"sess-c4-table-{idx}"
                    acc.start(sid, now=_t0())
                    self.assertEqual(acc.routing(sid), "accumulate",
                                     f"SCRF-C4 RED(前置): 表元素 {element!r} 会话未进"
                                     "累积态（断言会空过）。")
                    front._story_feed_from_stt(sid, _stt_frame(element), now=_t0())
                    self.assertIsNone(
                        acc.build_organizing_input(sid),
                        f"SCRF-C4 RED: 段级集合自报元素 {element!r} 未被判定命中 —— "
                        "表与判定不是同源（或归一化口径不一致）。")

                # 判定单源：临时换表后判定随动（防判定硬编码在函数体内）。
                old_table = front._FRONT_SILENT_ASR_HALLUCINATIONS
                try:
                    front._FRONT_SILENT_ASR_HALLUCINATIONS = frozenset({"测试幻觉句"})
                    sid_new = "sess-c4-swap-new"
                    acc.start(sid_new, now=_t0())
                    front._story_feed_from_stt(sid_new, _stt_frame("测试幻觉句"),
                                               now=_t0())
                    self.assertIsNone(
                        acc.build_organizing_input(sid_new),
                        "SCRF-C4 RED: monkeypatch 段级集合后新值未被判定 —— 判定没有读"
                        "模块级常量（单源被破：临时换表必须随动）。")
                    sid_old = "sess-c4-swap-old"
                    acc.start(sid_old, now=_t0())
                    front._story_feed_from_stt(sid_old, _stt_frame("Okay."), now=_t0())
                    self.assertIn(
                        "Okay.", acc.build_organizing_input(sid_old) or "",
                        "SCRF-C4 RED: 换表后旧值仍被 drop —— 判定源里存在硬编码表"
                        "（单源被破）。")
                finally:
                    front._FRONT_SILENT_ASR_HALLUCINATIONS = old_table

    # coverage: SCRF-C4 -- pending_stop 下被过滤**不即时释放**（routing 仍 pending_stop、
    #            零 active=false）——防吞 1.7s 后才到的真实转写；释放交既有 10s sweep
    def test_scr_c4_pending_stop_filter_does_not_release_immediately(self):
        with tempfile.TemporaryDirectory(prefix="scr_c4_pend_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-c4-pend"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                dev.append_text(_ctx_msg("story"))
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                dev.append_text(_listen_stop_msg())   # 累积空 -> pending_stop
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-C4 RED(前置): 未落 pending_stop。")
                with _release_spy(front) as rel:
                    up_e, _dev_e = _engine_to_device(front, sid, [_stt_frame("Okay.")])
                self.assertEqual(
                    rel, [],
                    f"SCRF-C4 RED: 过滤命中触发了记录语境释放（实记 {rel}）—— 不得新增"
                    "即时释放路径（会吞掉 1.7s 后才到的真实转写；交 10s sweep）。")
                self.assertEqual(
                    _release_frames(up_e.sent), [],
                    "SCRF-C4 RED: 过滤命中向引擎腿注入了 active=false —— 同款即时释放"
                    "破（pending 受保护）。")
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-C4 RED: 过滤命中后挂起态漂移。")
                self.assertIsNone(acc.build_organizing_input(sid),
                                  "SCRF-C4 RED: 过滤命中仍喂进了累积器。")

    # coverage: SCRF-C4 -- 收尾词面零影响：_story_finish_on_phrase(说完了) 仍 True
    #            （段级过滤只作用于素材面，收尾词判定不经该缝）
    def test_scr_c4_finish_phrase_surface_unaffected(self):
        with tempfile.TemporaryDirectory(prefix="scr_c4_phrase_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-c4-phrase"
                acc = front._get_story_acc()
                acc.start(sid, now=_t0())
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SCRF-C4 RED(前置): 未进累积态（收尾词判定仅累积态"
                                 "消费）。")
                self.assertTrue(
                    front._story_finish_on_phrase(sid, _stt_frame("说完了")),
                    "SCRF-C4 RED: 累积态「说完了」不再命中收尾沿 —— 段级过滤误伤了"
                    "收尾词判定面（FP-A1 语义：只滤素材面）。")
                self.assertFalse(
                    front._story_finish_on_phrase(sid, _stt_frame("Okay.")),
                    "SCRF-C4 RED: 普通句「Okay.」被收尾词判定命中 —— 判定面与非判定面"
                    "混淆（过滤不得接到收尾沿）。")


# =========================================================================== #
# SCRF-C2 + SCRF-C3 — 整理产物占位守卫（后防线，承重）+ A5 正常路径回归             #
# =========================================================================== #


class ScrfOrganizePlaceholderGuardAcceptance(unittest.TestCase):
    maxDiff = None

    def _seed_material(self, front, sid, dev, up, text="好的。"):
        """ctx(story) -> 真素材 feed（routing accumulate、累积非空）。"""
        dev.append_text(_ctx_msg("story"))
        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
        _engine_to_device(front, sid, [_stt_frame(text)])
        acc = front._get_story_acc()
        self.assertEqual(acc.routing(sid), "accumulate",
                         "SCRF-C2/C3 RED(前置): ctx(story)+stt 后未进累积态。")
        self.assertTrue(acc.has_accum(sid),
                        "SCRF-C2/C3 RED(前置): 累积缓冲为空（素材喂入链回退）。")
        return acc

    def _organize_once(self, front, reply, raw="孩子讲了一段话。"):
        """直调整理回合单源（LLM 桩注入回复）；返回 (record, 日志行)。"""
        with _llm_stub(front, _LLMStub(reply)):
            with _log_capture() as logs:
                record = front._organize_and_archive(raw, corr_id="scrfc2")
        return record, logs

    # coverage: SCRF-C2 -- 整理产物占位守卫（端到端承重）：素材过段级过滤与素材守卫
    #            （好的。 compact=2）-> listen stop 走收尾单源 -> 整理 LLM 返回占位
    #            （没有可整理的内容）-> 零归档（MD / seq / story_text / story_saved_stamp）、
    #            零安抚、start_organizing 恰 1（守卫不得前移到演出之前）、story_list
    #            页面退出恰 1 且 total==0 / items==[] / restore 缺省或假、active=false
    #            恰 1、routing 回 chat、审计 reason=organize_placeholder 恰 1 且与
    #            organize_failed 分档
    def test_scr_c2_organize_placeholder_guard_releases_with_page_exit(self):
        with tempfile.TemporaryDirectory(prefix="scr_c2_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-c2"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                acc = self._seed_material(front, sid, dev, up)
                self.assertIn(
                    "好的。", acc.build_organizing_input(sid) or "",
                    "SCRF-C2 RED(前置): 素材「好的。」未进累积器（compact=2 = 段级集合"
                    "收不到、素材守卫 <2 也拦不住的精确样本）。")
                story_list_base = len(_story_list_frames(dev.sent_text))
                self.assertEqual(
                    _start_organizing_frames(dev.sent_text), [],
                    "SCRF-C2 RED(前置): 收尾前已有 start_organizing 帧（演出前提失真）。")

                dev.append_text(_listen_stop_msg())   # 累积非空 -> 收尾单源
                with _llm_stub(front, _LLMStub("（没有可整理的内容）")):
                    with _log_capture() as finish_logs:
                        with _finish_spy(front) as fin:
                            asyncio.run(front.pump_client_to_upstream(dev, up, sid))

                self.assertEqual(
                    len(fin), 1,
                    f"SCRF-C2 RED: 收尾单源调用次数 = {len(fin)}（冻结恰 1）—— 占位"
                    "守卫的输入（整理 LLM 回复）只在回合后才存在；守卫前移避免演出"
                    "已被否决（S-F2.2-AC4 演出须盖住延迟 + 261020 D4-1）。")
                self.assertEqual(
                    _story_md_files(tmp), [],
                    "SCRF-C2 RED: 占位回复被当正文归档成篇 —— 垃圾篇（round-c "
                    "2026-11-18-s33477cee「（没有可整理的内容）」）复现；守卫须在 "
                    "archive_story 之前（先 _assign_seq 后写 MD = 删档也留 seq 缺口）。")
                self.assertEqual(
                    _ledger_snapshot(tmp), {},
                    "SCRF-C2 RED: 占位路径占了 seq 账本号 —— 零盖章被破（账本 +1 只"
                    "属真归档路径）。")
                organizing = _start_organizing_frames(dev.sent_text)
                self.assertEqual(
                    len(organizing), 1,
                    f"SCRF-C2 RED: start_organizing 帧数 = {len(organizing)}（冻结恰 1）"
                    "—— 守卫不得前移到演出之前（演出先行是延迟遮挡与过渡态可见的"
                    "承重面）。")
                lists = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(lists), story_list_base + 1,
                    f"SCRF-C2 RED: story_list 帧数增量 = "
                    f"{len(lists) - story_list_base}（冻结恰 1）—— ORGANIZING 非交互页"
                    "的唯一机械退出载体（261021 真机 R6 困页实证 + SAO-A1 同因重钉）；"
                    "该帧是「页面退出」不是「列表新增」。")
                exit_data = lists[-1]["data"] or {}
                self.assertEqual(
                    exit_data.get("total"), 0,
                    f"SCRF-C2 RED: 页面退出帧 total = {exit_data.get('total')!r}（冻结 "
                    "0）—— 不归档则 total 现算不变（列表不虚增）。")
                self.assertEqual(
                    exit_data.get("items"), [],
                    f"SCRF-C2 RED: 页面退出帧 items = {exit_data.get('items')!r}（冻结"
                    " []）—— 不归档则不新增条目。")
                self.assertFalse(
                    exit_data.get("restore"),
                    f"SCRF-C2 RED: 页面退出帧带 restore = "
                    f"{exit_data.get('restore')!r} —— 离页导航帧例外须 restore=False"
                    "（翻 true 设备只填缓存不接管前台 -> 困死复现）。")
                self.assertEqual(
                    _story_text_frames(dev.sent_text), [],
                    "SCRF-C2 RED: 占位路径下发了 story_text —— 设备侧 story_saved_stamp "
                    "盖章不得发生（静默释放，零盖章）。")
                self.assertEqual(
                    _kidpal_speak_frames(up.sent), [],
                    "SCRF-C2 RED: 占位路径下发了 kidpal_speak 安抚 —— 模型明确声明无"
                    "内容 = 静默释放类（与 R1-N4「纯幻听不安抚」同语义；丢稿类才安抚）。")
                releases = _release_frames(up.sent)
                self.assertEqual(
                    len(releases), 1,
                    f"SCRF-C2 RED: 占位路径 active=false 帧数 = {len(releases)}（冻结"
                    "恰 1）—— 会话已释放而引擎语境标志尚活 = 后续对话永久抑制。")
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-C2 RED: 占位路径收尾后 routing 未回 chat。")
                ph_lines = _log_lines_with(
                    finish_logs, "story_finish", "archived=false",
                    f"reason={ORGANIZE_PLACEHOLDER_REASON}")
                self.assertEqual(
                    len(ph_lines), 1,
                    f"SCRF-C2 RED: story_finish archived=false "
                    f"reason={ORGANIZE_PLACEHOLDER_REASON} 行数 = {len(ph_lines)}"
                    f"（冻结恰 1）—— 与 organize_failed / empty_material 分档（复用"
                    f"同 token 会让日志不可分；实记 {finish_logs!r}）。")
                self.assertEqual(
                    _log_lines_with(finish_logs, "reason=organize_failed"), [],
                    "SCRF-C2 RED: 占位路径打成了 organize_failed —— 两条下行序列不同"
                    "（丢稿类安抚 + 退出帧 vs 静默类只退出帧），reason 必须分档。")
                self.assertTrue(
                    _log_lines_with(finish_logs, "story_organize_round", "ok=false"),
                    "SCRF-C2 RED: 缺 story_organize_round ok=false 行（body 截断留痕）"
                    "—— 占位命中不可归因到整理回合。")

    # coverage: SCRF-C2 -- 判据机械判别力（R1 空正文 / R2 去空白 <2 字 / R3 占位族整串 /
    #            R4 括号族）各自拒；真实短故事两条放行（含 FP-A5 body compact=5 上界）、
    #            引号族不被 R4 误伤；否决项负锚 title==body 照常归档（SAO 三桩保绿）
    def test_scr_c2_output_guard_mechanical_discrimination(self):
        rejects = (
            ("。", "empty_body"),                          # R1
            ("嗯", "insignificant_body"),                  # R2
            ("没有可整理的内容", "placeholder_body"),        # R3
            ("无内容", "placeholder_body"),                  # R3
            ("Nothing to organize.", "placeholder_body"),   # R3
            ("【无】", "placeholder_body"),                  # R4（方尖括号族）
            ("(no content)", "placeholder_body"),           # R4（半角括号族）
            ("（本轮素材不足）", "placeholder_body"),          # R4（全角括号族）
        )
        for reply, reason in rejects:
            with self.subTest(reply=reply):
                with tempfile.TemporaryDirectory(prefix="scr_c2_rej_") as tmp:
                    with _front_with_stories(tmp) as front:
                        record, logs = self._organize_once(front, reply)
                        self.assertIsNone(
                            record,
                            f"SCRF-C2 RED: 整理回复 {reply!r} 未被守卫拒绝（record 非 "
                            "None）—— R1–R4 机械判据须整串判定（措辞无关的结构判据 "
                            "R4 是长尾主力）。")
                        self.assertEqual(
                            _story_md_files(tmp), [],
                            f"SCRF-C2 RED: 整理回复 {reply!r} 落盘成篇 —— 零归档。")
                        self.assertEqual(
                            _ledger_snapshot(tmp), {},
                            f"SCRF-C2 RED: 整理回复 {reply!r} 占了 seq 账本号 —— "
                            "零盖章。")
                        self.assertTrue(
                            any(reason in line for line in logs),
                            f"SCRF-C2 RED: 整理回复 {reply!r} 的拒绝原因未留痕"
                            f"（期望细分类 {reason!r}；实记 {logs!r}）—— 判据分档"
                            "（empty_body / insignificant_body / placeholder_body）"
                            "须可归因。")

        passes = (
            ("冰淇淋\n想吃冰淇淋。", "冰淇淋", "想吃冰淇淋。"),
            ("糖葫芦\n正文一句话。", "糖葫芦", "正文一句话。"),   # FP-A5 body compact=5
        )
        for reply, title, body in passes:
            with self.subTest(reply=reply):
                with tempfile.TemporaryDirectory(prefix="scr_c2_pass_") as tmp:
                    with _front_with_stories(tmp) as front:
                        record, _logs = self._organize_once(front, reply)
                        self.assertIsNotNone(
                            record,
                            f"SCRF-C2 RED: 真实短故事 {reply!r} 被占位守卫误杀 —— "
                            "过滤须整串判定（真实短故事是放行面的上界证据）。")
                        self.assertEqual(len(_story_md_files(tmp)), 1,
                                         f"SCRF-C2 RED: 放行样本 {reply!r} 未落盘恰 1 篇。")
                        self.assertEqual(record.get("title"), title,
                                         f"SCRF-C2 RED: 放行样本标题 = "
                                         f"{record.get('title')!r}（期望 {title!r}）。")
                        self.assertEqual(record.get("text"), body,
                                         f"SCRF-C2 RED: 放行样本正文 = "
                                         f"{record.get('text')!r}（期望 {body!r}）。")

        # 引号族不被 R4 误伤（引用孩子原话是合法正文）。
        with tempfile.TemporaryDirectory(prefix="scr_c2_quote_") as tmp:
            with _front_with_stories(tmp) as front:
                record, _logs = self._organize_once(front, "他说了「你好」")
                self.assertIsNotNone(
                    record,
                    "SCRF-C2 RED: 引号族「他说了「你好」」被 R4 括号判据误伤 —— R4 "
                    "明确排除引号族（\"\" '' 「」 『』 《》），防误伤引用原话。")
                self.assertEqual(len(_story_md_files(tmp)), 1,
                                 "SCRF-C2 RED: 引号族放行样本未落盘恰 1 篇。")

        # 否决项负锚：title == body 必须照常归档（SAO 三条冻结桩保绿）。
        with tempfile.TemporaryDirectory(prefix="scr_c2_titleeq_") as tmp:
            with _front_with_stories(tmp) as front:
                record, _logs = self._organize_once(front, "一段故事\n一段故事")
                self.assertIsNotNone(
                    record,
                    "SCRF-C2 RED: title==body 被误判为占位 —— 否决「title == body -> "
                    "判占位」规则（会一次红 SAO 三条冻结桩）。")
                self.assertEqual(len(_story_md_files(tmp)), 1,
                                 "SCRF-C2 RED: title==body 样本未落盘恰 1 篇。")

    # coverage: SCRF-C3 -- A5 正常路径回归（must-stay-green）：真实素材 + 正常两行整理
    #            输出 -> archived=true、MD 恰 1 篇且标题/正文 == 整理结果、story_text
    #            恰 1、story_list 含新篇、seq 账本 +1、routing 回 chat、active=false 恰 1
    def test_scr_c3_normal_archive_path_keeps_working(self):
        with tempfile.TemporaryDirectory(prefix="scr_c3_ok_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-c3-ok"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                acc = self._seed_material(front, sid, dev, up, text="我想讲冰淇淋。")
                story_list_base = len(_story_list_frames(dev.sent_text))
                dev.append_text(_listen_stop_msg())
                with _llm_stub(front, _LLMStub("冰淇淋\n想吃冰淇淋。")):
                    with _log_capture() as logs:
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))

                files = _story_md_files(tmp)
                self.assertEqual(
                    len(files), 1,
                    f"SCRF-C3 RED: 正常整理路径归档篇数 = {len(files)}（must-stay-green"
                    " 恰 1）—— 轮 C 两层守卫吞没了正常素材。")
                record = front.gw_story.read_story_file(str(files[0]))
                self.assertIsNotNone(record, "SCRF-C3 RED: 归档文件读回失败。")
                self.assertEqual(record.get("title"), "冰淇淋",
                                 f"SCRF-C3 RED: 归档标题 = {record.get('title')!r}"
                                 "（冻结 == 整理结果首行）。")
                self.assertEqual(record.get("text"), "想吃冰淇淋。",
                                 f"SCRF-C3 RED: 归档正文 = {record.get('text')!r}"
                                 "（冻结 == 整理结果其余行）。")

                texts = _story_text_frames(dev.sent_text)
                self.assertEqual(
                    len(texts), 1,
                    f"SCRF-C3 RED: story_text 帧数 = {len(texts)}（冻结恰 1）—— "
                    "设备侧盖章（story_saved_stamp）载体缺席。")
                data = texts[0]["data"] or {}
                self.assertEqual(data.get("id"), record.get("id"),
                                 "SCRF-C3 RED: story_text id 与归档记录不一致。")
                self.assertEqual(data.get("title"), "冰淇淋",
                                 "SCRF-C3 RED: story_text 标题与整理结果不一致。")
                self.assertEqual(data.get("text"), "想吃冰淇淋。",
                                 "SCRF-C3 RED: story_text 正文与整理结果不一致。")
                self.assertIsInstance(data.get("seq"), int,
                                      "SCRF-C3 RED: story_text 缺整数 seq（盖章载体）。")

                lists = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(lists), story_list_base + 1,
                    f"SCRF-C3 RED: story_list 帧数增量 = {len(lists) - story_list_base}"
                    "（冻结恰 1 = 归档成功三时点②）。")
                list_data = lists[-1]["data"] or {}
                self.assertEqual(list_data.get("total"), 1,
                                 f"SCRF-C3 RED: 归档后列表 total = "
                                 f"{list_data.get('total')!r}（冻结 1）。")
                self.assertIn(
                    record.get("id"),
                    [item.get("id") for item in list_data.get("items") or []],
                    "SCRF-C3 RED: 归档后 story_list 未含新篇。")

                ledger = _ledger_snapshot(tmp)
                seq = data.get("seq")   # read_story_file 不带 seq；盖章载体 = story_text
                self.assertEqual(
                    (ledger.get("by_id") or {}).get(record.get("id")), seq,
                    f"SCRF-C3 RED: seq 账本 by_id = "
                    f"{(ledger.get('by_id') or {}).get(record.get('id'))!r} 与 story_text "
                    f"seq = {seq!r} 不一致 —— 盖章形态与账本不是同源。")
                self.assertEqual(seq, 1,
                                 f"SCRF-C3 RED: 首篇 seq = {seq!r}"
                                 "（冻结 1 = 账本从 1 起）。")
                self.assertEqual(ledger.get("next_seq"), 2,
                                 f"SCRF-C3 RED: 账本 next_seq = "
                                 f"{ledger.get('next_seq')!r}（冻结 2 = seq 账本 +1）。")
                self.assertEqual(
                    len(_release_frames(up.sent)), 1,
                    "SCRF-C3 RED: 正常归档路径 active=false 帧数 != 1（恰一帧）。")
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-C3 RED: 正常归档后 routing 未回 chat。")
                self.assertEqual(
                    len(_log_lines_with(logs, "story_finish", "archived=true")), 1,
                    "SCRF-C3 RED: 缺 story_finish archived=true 审计行（恰 1）。")

    # coverage: SCRF-C3 -- reason 分档未吞丢稿类：整理 LLM 返回空（llm_unavailable）仍走
    #            安抚 + 页面退出帧（安抚经 kidpal_speak 恰 1、story_text 零、story_list
    #            恰 1 且 total==0 / items==[]、零归档、routing 回 chat）
    def test_scr_c3_llm_unavailable_still_soothes_and_exits(self):
        with tempfile.TemporaryDirectory(prefix="scr_c3_llm_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-c3-llm"
                dev, up = _FakeDeviceSocket(), _FakeUpstream()
                self._seed_material(front, sid, dev, up, text="我想讲冰淇淋。")
                story_list_base = len(_story_list_frames(dev.sent_text))
                dev.append_text(_listen_stop_msg())
                with _llm_stub(front, _LLMStub("")):   # 空返回 = llm_unavailable
                    with _log_capture() as logs:
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))

                self.assertEqual(
                    _story_md_files(tmp), [],
                    "SCRF-C3 RED: 丢稿类（llm_unavailable）落盘成篇 —— 当轮素材不归档"
                    "（R1-N4）。")
                self.assertEqual(
                    _story_text_frames(dev.sent_text), [],
                    "SCRF-C3 RED: 丢稿类下发了 story_text —— 无归档不得盖章。")
                speak = _kidpal_speak_frames(up.sent)
                self.assertEqual(
                    len(speak), 1,
                    f"SCRF-C3 RED: 丢稿类安抚 kidpal_speak 帧数 = {len(speak)}（冻结"
                    "恰 1）—— reason 分档不得把 llm_unavailable 并入静默释放类"
                    "（SAO-A1 四件保绿）。")
                lists = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(lists), story_list_base + 1,
                    f"SCRF-C3 RED: 丢稿类 story_list 帧数增量 = "
                    f"{len(lists) - story_list_base}（冻结恰 1 = 页面退出帧）。")
                list_data = lists[-1]["data"] or {}
                self.assertEqual(list_data.get("total"), 0,
                                 "SCRF-C3 RED: 丢稿类退出帧 total != 0。")
                self.assertEqual(list_data.get("items"), [],
                                 "SCRF-C3 RED: 丢稿类退出帧 items != []。")
                self.assertEqual(
                    len(_release_frames(up.sent)), 1,
                    "SCRF-C3 RED: 丢稿类 active=false 帧数 != 1。")
                self.assertEqual(
                    front._get_story_acc().routing(sid), "chat",
                    "SCRF-C3 RED: 丢稿类收尾后 routing 未回 chat。")
                self.assertTrue(
                    any("llm_unavailable" in line for line in logs),
                    f"SCRF-C3 RED: 丢稿类原因未留痕（期望 llm_unavailable；实记 "
                    f"{logs!r}）—— reason 分档须可归因。")


if __name__ == "__main__":
    unittest.main()