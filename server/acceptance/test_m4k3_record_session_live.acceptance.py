"""Frozen acceptance tests — TaskPackage 260924-m4-k3-record-session-live
(subsystem: server 半; workstream m4-star-notebook; mergeOrder 40; rigor full;
riskTier high; dependsOn 260924-m4-k1-story-cloud-link + 260924-m4-k2-p3-star-notebook-ui
均已 merged)。

承载 story = S-F2.2（开录双入口：AC1 技术信号 front 半——ctx 条件推送①恢复重裁 /
AC4 丢弃与引导态结束两条 front 路径守卫）+ prompt v8 双指令批（AC1 触发词入口 v8
承载 + notes ⑦ 意图边界 + E2 版本化铁律）+ FIX-A1 语义保持负锚。

Scope of record（taskpackage acceptance M4K3-A4/A7/A8/A9-server 半）：
  M4K3-A7 session_ctx 条件推送齿（defer 包 260928-story-connect-push-defer 摘除 K1
          建连即推后，三时点①恢复重裁 = plan T5 推荐形态：front 收 ctx(story) 切累积态
          同帧条件推送——routing 沿触发，routing != "accumulate" 时才推；story.py 零改动；
          重复 ctx = 零重复帧，契约「重复发送安全」语义保持；ctx(非 story) = 零帧）。
  M4K3-A4 引导态结束 ≠ 录音态丢弃 front 双路径守卫齿（notes 边角⑪：引导态结束未开口
          零累积零副作用 / 录音态丢弃有累积但 acc.cancel 弹缓冲不落盘——两路径分别
          行为承载；K1 front 冻结语义零触碰，本类全为 must-stay-green 守卫）。
  M4K3-A8 prompt v8 齿（ADR-001 无版本字面断言：解析 carrier/声明/渲染三面得版本再
          断言一致；内容锚钉当前版本文件故事段双指令 + 意图边界负例 + 影子口径/标题
          规则零偏移；v7 本体 sha256 零回改 = E2「新文件不回改 v7」铁律齿）。
  M4K3-A9 server 半零偏移守卫（条件推送落点 = _consume_kidpal_control ctx 分支; 建连
          恢复推送落点 = ws_gateway 建连段——2026-10-12 翻正（v1.63 时点①回接）后两落点
          并存, 结构守卫双钉; K1 27 齿零偏移由套件全量运行承载, 不在本文件重复断言）。

跨包边界（2026-10-12 翻正, 包 261010-p3-storylist-cold-start-restore）：FIX-A1 原为
「建连路径零 story_list 下行帧」负锚（human 方向锚 b036f554）；v1.63（requirements §9
frozen 2026-10-12）赎回时点①重裁——建连恢复推送回接（data.restore 标记、只填缓存不抢
前台），翻正齿 = test_m4k1_story_cloud_link.acceptance.py::
test_fix_a1_connect_path_emits_exactly_one_restore_story_list（授权链三件逐字见该件
REVISION LOG 2026-10-12 条）。本文件条件推送行为齿全部钉 ctx 触发时点、零触碰（ctx
同拍推送保留 = M4K3-A7 冻结齿对 builder 归并处置的硬约束）；落点结构守卫
test_a7_push_seam_lands_in_ctx_consume_not_connect 同批翻正：建连段**含**建连恢复
推送调用（>=1 处 _push_story_list）+ ctx 分支仍在（双落点并存）。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — front_app.py 面（接线半增量）：

  _consume_kidpal_control（既有拦截函数增量扩展；K1 冻结消费语义零偏移）：
    kidpal_session_ctx 分支在切累积态（acc.start）同帧条件推送 story_list：
      - 推送条件 = **routing 沿触发**：消费 ctx 时 acc.routing(session_id) !=
        "accumulate"（即本帧 ctx 把会话从 chat 切入累积态）才推——在 acc.start 之前
        自查 routing（story.py 零改动；routing() 查询不改状态，K1 冻结）。
      - 首帧 ctx(story)（chat→accumulate 沿）→ 恰一帧 story_list 下行（设备腿
        kidpal/display mcp 帧，producer 语义 = list_stories 单源，total = 全量现存
        篇数——plan T5「①恢复 = 引导态会话建立同拍」的 front 半）。
      - 重复 ctx(story)（已 accumulate）→ 零新增帧（契约 §四「重复发送安全」+
        零重复推送；板端 gating 半 = M4K3-A6 host 齿）。
      - ctx(home/schedule/未知)（不切累积态）→ 零帧（闭集预留取值现版本无行为，
        K1 冻结语义保持）。
      - 推送失败不阻断 ctx 消费（ctx 整条拦截语义优先; 推送为增强, K1 建连推送
        try/except 同款先例）。
    kidpal_story_delete 分支零变化（删除后重推③既有）。

Shape authority = docs/contracts/interface-contract.md §四（kidpal_session_ctx 已
LANDED 零改动——本包只落实现与推送重裁行为面）+ arch §5.2 场景④（三时点推送行为
策略权威；①恢复形态 = plan T5 重裁结论，acceptance 冻齿承载）。

prompt v8 面（server/deploy 部署制品模式，E2 铁律）：
  - 版本化演进：新文件 openclaw-system-prompt.v8.md（以 v7 为基只动故事段——加
    「帮我记下来」第二指令 + 意图边界措辞）；v7 本体零回改（sha256 冻结齿）；
    OPENCLAW_SYSTEM_PROMPT_CARRIER 声明翻转（artifact 行 v7→v8 + 渲染面行 +
    历史版本行 append v7 沿革）+ provider.config.yaml 三键同步（artifact/source/
    deploy_target）+ apply-xiaozhi-overlay.sh PROMPT_SRC/PROMPT_DST 版本行翻转。
  - ADR-001 纪律：三面接线一致性按「解析 carrier 得当前版本 V → 断言声明/渲染两面
    与 V 一致 + V 文件在世」版本无关形态承载（bump 不假红、漏翻必红）；本文件零
    "当前 = vN" 字面断言。
  - 内容锚（钉当前版本文件故事段，版本无关读取）：双指令「我要讲故事」+「帮我
    记下来」在世；意图边界负例「帮我记下来明天带跳绳」（= 课表录入, 非开录——
    notes ⑦ 不得越界误触课表录入意图）显式写进故事段；影子口径（禁产 story_list/
    story_text）与标题 ≤12 字规则零偏移继承（v7:34/:52-53 现文）。
  - LLM 真判定（「帮我记下来」真机开录、误触课表真行为）= live 轮目击项
    （notes ⑩，M3-K3 同款诚实声明）——offline 不造 LLM 意图假齿。

契约条目 ↔ 齿 的强度申报：
  契约 §四 kidpal_session_ctx（已 LANDED 零改）→ M4K3-A7 行为级（真 pump + 真帧
        计数 + 真归档链）——front 条件推送半全承载；板端发送半 = host 齿（RSL-A1..A3,
        机制级）。
  arch §5.2④ 三时点①恢复（行为策略, plan T5 重裁）→ M4K3-A7 行为级 + M4K3-A9
        结构级（落点守卫）。
  requirements S-F2.2-AC4 丢弃/引导态结束 front 半 → M4K3-A4 行为级守卫（K1 cancel
        语义零偏移面——弱于 AC 处：设备侧按键触发半 = host 结构齿 RSL-A5 + 设备轮
        目击，本文件只承载 front 侧副作用差分）。
  E2 prompt 版本化铁律 → M4K3-A8 结构级（三面解析一致 + sha256 零回改）+ 行为级
        （内容锚真读当前版本文件）。
  弱于契约/显式缺口（逐条）：
    ① **P3 冷浏览（无会话翻页）无列表数据源** = 显式取舍 dropped（taskpackage
       decisionCoverage M4K3-DC-1；补偿 = 首次开录/归档/删除后可见 + K2 影子缓存跨
       会话留存）——本文件不为其造断言（契约零改约束下无干净缝, plan T5 残余缺口
       如实申报, human 过目项）。
    ② **ctx 推送失败的降级路径**（推送异常不阻断消费）：行为等价假设 = try/except
       包裹, offline 以「ctx 消费语义优先」结构承载（落点守卫）, 异常注入半不钉
       （front 建连推送 try/except 同款先例, K1 同口径未钉）。
    ③ **routing 沿判定的实现位置**（消费分支内 acc.start 前自查 vs acc.start 返回
       沿信号）：行为齿只钉帧数可观测结果, 实现位置 = 源结构守卫（落点 needle）,
       不钉内部行序。
    ④ **prompt 内容锚 = 并集窗口选节**（v7 齿同款内容驱动, 措辞换写不误红、语义
       被删才红）; 标题规则钉「12 字」锚不钉整句。

Coverage map (acceptanceRef -> test):
  M4K3-A7 -> test_a7_ctx_story_first_pushes_exactly_one_story_list
           -> test_a7_ctx_duplicate_pushes_no_extra_frame
           -> test_a7_ctx_non_story_pushes_zero_frames（守卫）
           -> test_a7_plain_chat_session_pushes_zero_frames（守卫）
           -> test_a7_push_seam_lands_in_ctx_consume_not_connect（结构守卫）
  M4K3-A4 -> test_a4_guiding_end_zero_side_effect（守卫）
           -> test_a4_discard_after_speech_drops_content（守卫）
  M4K3-A8 -> test_a8_prompt_three_faces_version_agnostic（守卫）
           -> test_a8_current_prompt_dual_trigger_and_boundary
           -> test_a8_v7_prompt_immutable（守卫）
  M4K3-A9 -> server 半 = 套件全量零偏移（K1 27 齿 + 本文件守卫齿）+ 上述结构守卫;
             firmware 半 = record_session_live.acceptance.c RSL-C1。

Red baseline (acceptance-author 2026-09-30 实跑；命令 = taskpackage redBaselineCommand
通道②; 现状树 @5d58eb9):
  实跑 = 4 failed / 199 passed / 4 deselected / 2 subtests passed。设计态红 4 齿 =
  未实现行为: test_a7_ctx_story_first_pushes_exactly_one_story_list（首帧 ctx 产 0 帧
  story_list）、test_a7_ctx_duplicate_pushes_no_extra_frame（重复 ctx 共 0 帧）、
  test_a7_push_seam_lands_in_ctx_consume_not_connect（ctx 分支无 _push_story_list）、
  test_a8_current_prompt_dual_trigger_and_boundary（故事段无「帮我记下来」第二指令/
  无「明天带跳绳」边界负例）; 红名 verbatim 见 taskpackage redBaselineResult。守卫齿
  （a7 负锚两条 / a4 两条 / a8 三面一致与 v7 零回改）现状绿（must-stay-green）。
  收集零错误（冻结即冒烟, DEV-GOVERNANCE §3）。

mutation 自证（mutationRequired=true）：逐齿变异设计 + expectedRed 见 taskpackage
mutationEvidence.perAcceptance（设计态红齿 = counterEvidence; 守卫齿 sensitive=false
显式申报）。

Run protocol:
  python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance
  (standalone: python 本文件)

REVISION LOG（冻结期修订窗; 只记 fixture/线束自身缺陷闭环, 零断言语义变化）:
  2026-09-30  R1 test_a4 两齿驱动通道修正（fixture 缺陷: 冻结即冒烟首跑暴露）——
              首版以泵直调（pump_client_to_upstream 空入列）模拟设备断连, 实测
              「丢弃后重开停录产生归档」红: 断连 cancel 钩子住 ws_gateway 连接
              处理段 finally（front_app.py :970-979）, 泵直调不可达该钩子 → 首版
              红是线束假象非被测语义。修正 = 两齿改走真实连接流程（ws_gateway
              全程离线: 鉴权 + accept + 假引擎腿 + 两泵 + 断连收尾, K1 FIX-A1
              _FakeDeviceSession/_fake_engine_ws 同款）+ _fixed_session_id 钉连接
              会话键供「缓冲被清」同键复核。断言语义零变化（同 zero-archive /
              zero-story_text / buffer-cleared 三断言面）。
  2026-09-30  R2 test_a7_push_seam_lands 切片收紧（fixture 缺陷: 首跑假绿暴露）——
              首版整函数切片被 :966 注释块「_push_story_list…保留不动」字面假绿;
              二版函数体切片仍假绿（delete 分支 ③重推既有调用在函数体内）。修正 =
              切片收紧到 kidpal_session_ctx 分支行 → delete 分支行（①条件推送的
              真实落点）; 断言语义零变化（落点 = ctx 分支）。
  2026-10-12  R3 test_a7_push_seam_lands 建连段断言翻正（正常改验收通道, 非 §6
              tighten-only; 包 261010-p3-storylist-cold-start-restore）——FIX-A1
              负锚「建连零 story_list 帧」由 v1.63（requirements §9 frozen
              2026-10-12）赎回翻正为「建连恰一帧 data.restore==true 恢复推送」
              （授权链三件 = CCP 261010-p3-storylist-cold-start HUMAN_APPROVED
              2026-10-12 + §9 v1.63 行 + human 方向锚留痕, 逐字见 K1 件 REVISION
              LOG 2026-10-12 条）。本件动作 = test_a7_push_seam_lands_in_ctx_
              consume_not_connect 建连段断言 == 0 → >= 1（落点在场）+ 头注跨包
              边界段 / M4K3-A9 描述 / 类 docstring / 守卫齿措辞同步翻正; ctx 分支
              落点断言与全部行为齿零触碰（M4K3-A7 ctx 条件推送保留 = 本件冻结齿
              对 builder 归并处置的硬约束——ctx 同拍推送不得摘除）。红基线恒现跑
              （2026-10-12 实跑, 见 taskpackage redBaselineResult）: 本齿设计态红
              （建连段现 0 命中）+ 其余齿零新增红。
"""

from __future__ import annotations

import asyncio
import contextlib
import hashlib
import importlib.util
import json
import re
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
DEPLOY_DIR = SERVER / "deploy"
CARRIER = DEPLOY_DIR / "OPENCLAW_SYSTEM_PROMPT_CARRIER"
# SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E3：两常量服务过的
# provider 三键 / overlay PROMPT_SRC/DST vN 指针断言已随承载反转退役（faces 判法
# 移交该包 K1-E3 静态面/动态面集合相等不变式），现存留作历史口径参照。
PROVIDER_YAML = DEPLOY_DIR / "provider.config.yaml"
OVERLAY_SH = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
V7_NAME = "openclaw-system-prompt.v7.md"

# 冻结时点 v7 内容 hash（raw bytes 口径; v8 bump 与一切后续包不得改 v7 本体——
# E2「prompt v8 新文件不回改 v7」铁律齿; 计算式 = sha256(read_bytes())）。
V7_SHA256 = "bfcbaa05e4a7642304d10a38ccfe027ea9b23a879ae172a282ee3e2b9cc51935"

STORY_MARKER = "kidpal_session_ctx"


# --------------------------------------------------------------------------- #
# Module loading (K1 同款先例; module name 独立防跨件缓存串味)                  #
# --------------------------------------------------------------------------- #

_story_module_cache = None
_front_module_cache = None


def _story_module():
    """server/gateway/story.py 纯模块（模块级 callable 缺失 = 设计态红）。"""
    global _story_module_cache
    if _story_module_cache is not None:
        return _story_module_cache
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.story as st  # noqa: PLC0415 — offline import, cached
    _story_module_cache = st
    return st


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（K1 FIX-A1 增强版：恒等路由装饰，
    ws_gateway 名字不被吞——K1 同款, 本文件复用其形态）。"""
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

        def _identity(self, *args, **kwargs):  # noqa: ANN202
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
        spec = importlib.util.spec_from_file_location("kidpal_front_app_m4k3", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须"
                      "离线可装载（import 期零网络 I/O）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("M4K3 RED: " + err)
    return mod


def _front_source() -> str:
    try:
        return FRONT_APP.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


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


# --------------------------------------------------------------------------- #
# I/O doubles for the pump teeth（K1 同款先例形态）                             #
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
    asyncio.run(front.pump_client_to_upstream(dev, up, session_id))
    return up, dev


def _engine_to_device(front, session_id, frames):
    """Run the engine->device downlink pump; returns (upstream, device_socket)."""
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, session_id))
    return up, dev


def _forwarded_frame_types(sent):
    """Frame types the front forwarded up to the engine (text frames parsed)."""
    out = []
    for item in sent:
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            out.append("<non-json>")
            continue
        out.append(obj.get("type") if isinstance(obj, dict) else "<non-object>")
    return out


def _mcp_envelopes(sent_text):
    """kidpal/display tools/call envelopes as the device accepts them (type=mcp frames)."""
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
    for env in _mcp_envelopes(sent_text):
        args = ((env or {}).get("params") or {}).get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _story_text_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_text"]


def _seed(stories_dir: Path, n: int, *, start_day: int = 1, slug_prefix: str = "k3"):
    """Seed n deterministic stories (date desc ordering: newest = highest day)."""
    st = _story_module()
    made = []
    for i in range(n):
        day = start_day + i
        rec = st.archive_story(
            str(stories_dir), f"正文-{i}", title=f"故事{i}", slug=f"{slug_prefix}{i:02d}",
            date=f"2026-09-{day:02d}")
        made.append(rec)
    return made


def _ctx_msg(source_page: str) -> str:
    return json.dumps({"type": STORY_MARKER, "source_page": source_page},
                      ensure_ascii=False)


# =========================================================================== #
# M4K3-A7 — session_ctx 条件推送齿（defer ①恢复重裁 front 半; plan T5）           #
# =========================================================================== #

class M4K3CtxConditionalPushAcceptance(unittest.TestCase):
    """M4K3-A7: front 收 kidpal_session_ctx(story) 切累积态同帧 routing 沿条件推送。

    行为级（真 pump + 真帧计数 + 真归档链 producer）; 负锚齿（非 story / 普通会话
    泵级零帧）= 守卫, 防实现过宽（每帧都推 / 泵级无 ctx 推送）。FIX-A1 翻正齿（建连
    恰一帧 restore, K1 文件）与 ctx 条件推送两落点并存（2026-10-12 翻正, 见 REVISION
    LOG R3）; 落点边界由 test_a7_push_seam_lands_in_ctx_consume_not_connect 结构
    守卫承载。"""

    maxDiff = None

    def test_a7_ctx_story_first_pushes_exactly_one_story_list(self):
        """coverage: M4K3-A7 — 首帧 ctx(story)（chat→accumulate 沿）→ 恰一帧
        story_list 下行（defer ①恢复: 引导态会话建立同拍见列表, plan T5 推荐形态）。"""
        with tempfile.TemporaryDirectory(prefix="m4k3_a7a_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                sid = "sess-a7-first"
                up, dev = _device_to_engine(front, sid, [_ctx_msg("story")])
                self.assertNotIn(
                    STORY_MARKER, _forwarded_frame_types(up.sent),
                    "M4K3-A7 RED: kidpal_session_ctx 被透传引擎 —— K1 拦截语义偏移"
                    "（契约 §四: 网关消费并拦截、不下透引擎; 本包不得破）。")
                self.assertEqual(
                    front._get_story_acc().routing(sid), "accumulate",
                    "M4K3-A7 RED: ctx(story) 未切累积态 —— K1 收敛入口偏移。")
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"M4K3-A7 RED: 首帧 ctx(story) 产生 {len(pushes)} 帧 story_list "
                    "下行（应恰一帧）—— ctx 条件推送未实现（defer ①恢复重裁 front 半"
                    "缺失; 引导态会话建立同拍无列表数据源）或非单帧（重复推送）。")
                self.assertEqual(
                    pushes[0]["data"]["total"], 2,
                    "M4K3-A7 RED: 条件推送 total != 现存篇数 —— producer 语义偏移"
                    "（story.py 单源零改红线）。")

    def test_a7_ctx_duplicate_pushes_no_extra_frame(self):
        """coverage: M4K3-A7 — 重复 ctx(story)（已 accumulate）→ 零新增帧（契约 §四
        「重复发送安全」语义保持: 板端重复发 ctx, front 不重复推列表）。"""
        with tempfile.TemporaryDirectory(prefix="m4k3_a7b_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 1)
                sid = "sess-a7-dup"
                _up1, dev1 = _device_to_engine(front, sid, [_ctx_msg("story")])
                _up2, dev2 = _device_to_engine(front, sid, [_ctx_msg("story")])
                total = (len(_story_list_frames(dev1.sent_text)) +
                         len(_story_list_frames(dev2.sent_text)))
                self.assertEqual(
                    total, 1,
                    f"M4K3-A7 RED: 重复 ctx(story) 共产生 {total} 帧 story_list（应"
                    "恰 1: 首沿 1 帧 + 重复 0 帧）—— routing 沿条件缺失（已累积态"
                    "重复触发 = 重复推送, 板端每会话重复帧 = defer 缺陷形态复潮）或"
                    "条件推送未实现（0 帧）。")

    def test_a7_ctx_non_story_pushes_zero_frames(self):
        """coverage: M4K3-A7 守卫 — ctx(home/schedule/未知) 不切累积态且零帧
        （闭集预留取值现版本无行为, K1 冻结语义保持; 防实现过宽）。"""
        with tempfile.TemporaryDirectory(prefix="m4k3_a7c_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 1)
                for value in ("home", "schedule", "whatever"):
                    sid = f"sess-a7-{value}"
                    up, dev = _device_to_engine(front, sid, [_ctx_msg(value)])
                    self.assertNotIn(
                        STORY_MARKER, _forwarded_frame_types(up.sent),
                        f"M4K3-A7 RED: source_page={value} 被透传 —— K1 拦截偏移。")
                    self.assertEqual(
                        front._get_story_acc().routing(sid), "chat",
                        f"M4K3-A7 RED: source_page={value} 误切累积态 —— K1 闭集"
                        "语义偏移。")
                    self.assertEqual(
                        len(_story_list_frames(dev.sent_text)), 0,
                        f"M4K3-A7 RED: source_page={value} 产生 story_list 下行 —— "
                        "非 story 取值零行为被破（过宽实现: 无累积态切换不推送）。")

    def test_a7_plain_chat_session_pushes_zero_frames(self):
        """coverage: M4K3-A7 守卫 — 省略 ctx = 普通会话（向后兼容）泵级零 story_list
        下行（防「每会话都推」回潮——推送只许 ctx(story) 沿触发与建连恢复两落点,
        2026-10-12 翻正后口径; 建连段断言归结构守卫齿, 本齿钉泵级零帧）。"""
        with tempfile.TemporaryDirectory(prefix="m4k3_a7d_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                sid = "sess-a7-plain"
                up, dev = _device_to_engine(
                    front, sid, [json.dumps({"type": "listen", "state": "start"})])
                self.assertIn(
                    "listen", _forwarded_frame_types(up.sent),
                    "M4K3-A7 RED: 普通帧被拦截 —— 向后兼容负锚破坏（省略 ctx = "
                    "普通会话）。")
                self.assertEqual(
                    len(_story_list_frames(dev.sent_text)), 0,
                    "M4K3-A7 RED: 无 ctx 普通会话泵级产生 story_list 下行 —— 推送"
                    "过宽（只许 ctx(story) 沿触发与建连恢复两落点, 2026-10-12 翻正后"
                    "口径; 泵级无 ctx 推送 = 守卫）。")

    def test_a7_push_seam_lands_in_ctx_consume_not_connect(self):
        """coverage: M4K3-A7 / M4K3-A9（结构守卫）— 条件推送落点 = _consume_kidpal_
        control ctx 分支; ws_gateway 建连段含建连恢复推送调用（2026-10-12 翻正:
        v1.63 三时点①回接——原「零调用」负锚随 FIX-A1 翻正同步翻正, 授权链见
        REVISION LOG R3; 建连推送的 wire 形态断言归 K1 翻正齿, 本齿只钉落点在场）。"""
        src = _front_source()
        self.assertTrue(
            bool(src), "M4K3-A7 RED(infra): front_app.py 源码读取失败。")
        accept_pos = src.find("await websocket.accept()")
        pump_pos = src.find("pump_up = asyncio.create_task")
        consume_pos = src.find("async def _consume_kidpal_control")
        self.assertTrue(
            accept_pos != -1 and pump_pos != -1 and consume_pos != -1,
            "M4K3-A7 RED(infra): front_app.py 结构锚漂移（accept/pump/消费函数锚"
            "缺失）—— 须复核本齿锚。")
        # 消费函数体切片 = 函数定义行到下一个顶层 def/async def（防函数区外注释/
        # 其它函数的命中假绿——冻结期实测教训, 见 REVISION LOG R2）。
        next_def_candidates = [
            p for p in (src.find("\ndef ", consume_pos + 1),
                        src.find("\nasync def ", consume_pos + 1)) if p != -1]
        consume_end = min(next_def_candidates) if next_def_candidates else len(src)
        connect_seg = src[accept_pos:pump_pos]
        # ctx 分支切片 = ctx 分支行到 delete 分支行（③删除后重推已在 delete 分支,
        # 整函数切片会假绿——冻结期实测教训 R2）; ①条件推送须落本分支。
        ctx_pos = src.find('if mtype == "kidpal_session_ctx":', consume_pos)
        delete_pos = src.find('if mtype == "kidpal_story_delete":', consume_pos)
        self.assertTrue(
            ctx_pos != -1 and delete_pos != -1 and ctx_pos < delete_pos,
            "M4K3-A7 RED(infra): _consume_kidpal_control ctx/delete 分支锚漂移"
            "—— 须复核本齿锚。")
        ctx_branch = src[ctx_pos:delete_pos]
        consume_seg = src[consume_pos:consume_end]
        # 2026-10-12 翻正（v1.63 三时点①回接, 授权链见 REVISION LOG R3 与 K1 件
        # REVISION LOG 2026-10-12 条）：建连段**含**建连恢复推送调用。
        self.assertGreaterEqual(
            connect_seg.count("_push_story_list"), 1,
            "M4K3-A7 RED: ws_gateway 建连段无 _push_story_list 调用 —— 建连恢复推送"
            "（三时点①, requirements v1.63 赎回）未回接（K1 FIX-A1 翻正齿必红; 授权链"
            "见该件 REVISION LOG 2026-10-12 条）。")
        self.assertIn(
            "_push_story_list", ctx_branch,
            "M4K3-A7 RED: _consume_kidpal_control 的 kidpal_session_ctx 分支无 "
            "_push_story_list —— ctx 条件推送未落 ctx 消费缝（defer ①恢复落点 = "
            "plan T5; ③删除后重推在 delete 分支不算数, 落到别处 = 缝分裂）。")


# =========================================================================== #
# M4K3-A4 — 引导态结束 ≠ 录音态丢弃 front 双路径守卫（notes 边角⑪）              #
# =========================================================================== #

class _FakeDeviceSession:
    """ws_gateway 形状的设备腿双桩（headers/accept/receive/send_text/send_bytes/close;
    K1 FIX-A1 同款——断连 cancel 钩子住 ws_gateway 连接处理段 finally, 本类使真实
    连接流程离线可达, 泵直调不可达该钩子）。"""

    def __init__(self, incoming=(), token=None, disconnect_exc=None):
        self.headers = {"authorization": f"Bearer {token}"} if token else {}
        self._incoming = [{"type": "websocket.receive", "text": t} for t in incoming]
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.accepted = False
        self.closed = None
        self.sent_text = []
        self.sent_bytes = []

    async def accept(self):
        self.accepted = True

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

    async def close(self, code=1000):
        self.closed = code


@contextlib.contextmanager
def _fake_engine_ws(upstream):
    """假引擎腿：注入 websockets.asyncio.client 假模块（K1 FIX-A1 同款——建连流程
    真实走完, 连接处理段两泵 + 断连收尾全部执行）。"""

    class _Conn:
        def __init__(self, up):
            self._up = up

        async def __aenter__(self):
            return self._up

        async def __aexit__(self, *exc):
            return False

    def _connect(*args, **kwargs):
        return _Conn(upstream)

    mod_client = ModuleType("websockets.asyncio.client")
    mod_client.connect = _connect
    mod_asyncio = ModuleType("websockets.asyncio")
    mod_asyncio.client = mod_client
    mod_ws = ModuleType("websockets")
    mod_ws.asyncio = mod_asyncio
    wanted = (("websockets", mod_ws), ("websockets.asyncio", mod_asyncio),
              ("websockets.asyncio.client", mod_client))
    added = [name for name, _mod in wanted if name not in sys.modules]
    for name, mod in wanted:
        sys.modules[name] = mod
    try:
        yield
    finally:
        for name in added:
            sys.modules.pop(name, None)


@contextlib.contextmanager
def _front_token(front, token):
    old = front.DEVICE_TOKEN
    front.DEVICE_TOKEN = token
    try:
        yield
    finally:
        front.DEVICE_TOKEN = old


@contextlib.contextmanager
def _fixed_session_id(front, hex_str):
    """钉住 ws_gateway 的连接会话键（HWV-A11: session_id = f"sess-{uuid4().hex}"）——
    断连 cancel 弹掉的会话可在泵直调通道以同键复核「缓冲被清」。patch 域内自动还原。"""
    import uuid as _uuid_mod
    from unittest.mock import patch as _patch

    fake = type("_FixedUuid4", (), {"hex": hex_str})()
    with _patch.object(_uuid_mod, "uuid4", lambda: fake):
        yield f"sess-{hex_str}"


class M4K3FrontSessionPathsAcceptance(unittest.TestCase):
    """M4K3-A4: 两条 front 路径分别行为承载（K1 cancel 语义零偏移, 守卫齿——K3 设备
    半接线后两路径语义不得混源）:

      引导态结束（未开口, 零累积）→ 断连 cancel → 零归档零 story_text 下行（无归档
      无 cancel 副作用 = 空累积 cancel 自然 no-op, front 零新分支——plan T3）。
      录音态丢弃（已录内容）→ 断连 cancel → 弹缓冲不落盘 → 零归档, 且缓冲被清
      （同会话键重开 + listen-stop 空累积不归档 = 已丢弃内容不复活——「不设重录
      恢复」v1.49 的 front 侧后果）。

    两齿走真实连接流程（ws_gateway 全程离线: 鉴权 + accept + 假引擎腿 + 两泵 +
    断连收尾）——cancel 钩子住连接处理段 finally, 泵直调不可达（冻结期实测教训,
    见 REVISION LOG R1）。"""

    maxDiff = None

    def _run_connection(self, front, incoming_texts, upstream_frames=(), hex_str="a" * 12):
        token = "F" * 24
        up = _FakeUpstream(frames=[
            f if isinstance(f, str) else json.dumps(f, ensure_ascii=False)
            for f in upstream_frames])
        dev = _FakeDeviceSession(incoming=incoming_texts, token=token)
        with _front_token(front, token):
            with _fake_engine_ws(up):
                with _fixed_session_id(front, hex_str) as sid:
                    asyncio.run(front.ws_gateway(dev))
        return up, dev, sid

    def test_a4_guiding_end_zero_side_effect(self):
        """coverage: M4K3-A4 — 引导态结束（ctx 后未开口即断开）零归档零 story_text。"""
        with tempfile.TemporaryDirectory(prefix="m4k3_a4a_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 1)
                up, dev, sid = self._run_connection(front, [_ctx_msg("story")])
                self.assertTrue(
                    dev.accepted,
                    "M4K3-A4 RED: 设备腿未被 accept —— 连接流程未走通（负锚空过）。")
                self.assertNotIn(
                    STORY_MARKER, _forwarded_frame_types(up.sent),
                    "M4K3-A4 RED: ctx 被透传引擎 —— K1 拦截语义偏移。")
                files_after = len(list(Path(tmp).glob("*.md")))
                self.assertEqual(
                    files_after, 1,
                    f"M4K3-A4 RED: 引导态结束后 stories 目录 {files_after} 篇（应仍"
                    "为预置 1 篇）—— 空累积产生归档（V31 边界: 不编造归档、不写空"
                    "故事; notes 边角⑪ 前半: 引导态结束无归档无 cancel 副作用）。")
                self.assertEqual(
                    len(_story_text_frames(dev.sent_text)), 0,
                    "M4K3-A4 RED: 引导态结束下发 story_text —— 空收尾假演出（孩子"
                    "没开口却看到盖章 = 假归档演出）。")

    def test_a4_discard_after_speech_drops_content(self):
        """coverage: M4K3-A4 — 录音态丢弃（有累积后断开）零归档 + 缓冲被清（同会话
        键重开不复活——「不设重录恢复」v1.49 的 front 侧后果; notes 边角⑪ 后半）。"""
        with tempfile.TemporaryDirectory(prefix="m4k3_a4b_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 1)
                stt_frames = [{"type": "stt", "text": "从前有一只小猫"},
                              {"type": "stt", "text": "它住在森林里"}]
                _up, dev, sid = self._run_connection(
                    front, [_ctx_msg("story")], upstream_frames=stt_frames)
                files_after = len(list(Path(tmp).glob("*.md")))
                self.assertEqual(
                    files_after, 1,
                    f"M4K3-A4 RED: 丢弃后 stories 目录 {files_after} 篇（应仍为预置 "
                    "1 篇）—— 已录内容被归档落盘（丢弃不是归档; S-F2.2-AC4 丢弃退回"
                    "列表不保存; front cancel 弹缓冲不落盘语义破）。")
                self.assertEqual(
                    len(_story_text_frames(dev.sent_text)), 0,
                    "M4K3-A4 RED: 丢弃路径下发 story_text —— 丢弃不该有归档演出。")
                # 缓冲被清复核：同会话键重开（cancel 已 pop 会话态 -> start 起新态;
                # 若 cancel 未弹缓冲, 此处 stop 会把已丢弃内容归档 = 复活）。
                st = _story_module()
                acc = front._get_story_acc()
                self.assertNotIn(
                    sid, acc.active_sessions(),
                    "M4K3-A4 RED: 断连后会话态未释放 —— ws_disconnect cancel 钩子"
                    "未弹会话（HWV-A11 泄漏面; 内容滞留累积态）。")
                _device_to_engine(front, sid, [_ctx_msg("story")])
                _up2, dev2 = _device_to_engine(
                    front, sid, [json.dumps({"type": "listen", "state": "stop"})])
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 1,
                    "M4K3-A4 RED: 丢弃后同会话键重开停录产生归档 —— 已丢弃内容复活"
                    "（「不设重录恢复」v1.49 破; cancel 未清缓冲 = 串味）。")
                self.assertEqual(
                    len(_story_text_frames(dev2.sent_text)), 0,
                    "M4K3-A4 RED: 空累积 stop 下发 story_text —— 假归档演出。")


# =========================================================================== #
# M4K3-A8 — prompt v8 齿（双指令 + 意图边界 + 版本化铁律; ADR-001 无字面断言）    #
# =========================================================================== #

_CARRIER_ARTIFACT_RE = re.compile(
    r"artifact（生成唯一来源）：`openclaw-system-prompt\.(v\d+)\.md`")


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _sha256_raw(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _section_window(body: str, title_keywords: tuple[str, ...],
                    extra_line_keywords: tuple[str, ...] = ()) -> str:
    """内容驱动选节：标题含任一关键词的 `## ` 节全文 + 其它节内含 extra 行的并集
    （v7 齿同款——措辞换写不误红、语义被删才红）。"""
    lines = body.splitlines()
    heads = [i for i, l in enumerate(lines) if l.startswith("## ")]
    out: list[str] = []
    for k, i in enumerate(heads):
        j = heads[k + 1] if k + 1 < len(heads) else len(lines)
        if any(w in lines[i] for w in title_keywords):
            out.extend(lines[i:j])
        else:
            out.extend(l for l in lines[i:j]
                       if any(w in l for w in extra_line_keywords))
    if not heads:  # 无节结构（异常文件）——退化为全文
        out = lines
    return "\n".join(out)


def _current_prompt_version() -> str | None:
    """解析 carrier 声明面 artifact 行得当前版本号（如 "v8"）——ADR-001: 三面一致性
    按解析值断言, 零版本字面断言（bump 不假红、漏翻必红）。"""
    m = _CARRIER_ARTIFACT_RE.search(_read_text(CARRIER))
    return m.group(1) if m else None


class M4K3PromptV8Acceptance(unittest.TestCase):
    """M4K3-A8: prompt v8 双指令批（E2 铁律 = 版本化演进 + 三面接线一致 + v7 零回改;
    内容锚钉当前版本文件故事段）。"""

    maxDiff = None

    def test_a8_prompt_three_faces_version_agnostic(self):
        """coverage: M4K3-A8 守卫 — 终点版冻结声明与沿革完整（261022 语义换代）。

        SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1/K1-E3：
        旧语义 = carrier artifact 行解析当前版 + provider.config.yaml 三键与
        apply-xiaozhi-overlay.sh PROMPT_SRC/DST 同指该版 + 版本文件在世 + carrier
        沿革保留 v7 条目（vN 三面接线守卫）；新语义 = 承载反转（v11 = 引擎注入
        承载的**终点版**，不再 bump；静态语义单一事实源 = workspace AGENTS.md），
        vN 三面 face-equality 由该包新验收
        test_261022_m5_k1_prompt_carrier_migration.acceptance.py 的 K1-E3 静态面/
        动态面集合相等不变式取代。本齿改钉（must-stay-green）：① carrier artifact
        行仍解析出终点版（声明形被改破 = 红）；② 终点版文件在世非空；③ carrier
        沿革保留 v7 条目（历史行不删铁律不变）。"""
        version = _current_prompt_version()
        self.assertTrue(
            version is not None,
            "M4K3-A8 RED: OPENCLAW_SYSTEM_PROMPT_CARRIER artifact 行无法解析出终点"
            "版本（声明面被改形——版本化管线锚破；261022-m5-k1 承载反转不删终点版"
            "声明）。")
        fname = f"openclaw-system-prompt.{version}.md"
        # SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1：provider 三键
        # 与 PROMPT_SRC/DST 同指断言退役（face-equality 移交该包 K1-E3）；保留
        # 终点版文件在世 + carrier 沿革两半。
        self.assertTrue(
            (DEPLOY_DIR / fname).is_file() and _read_text(DEPLOY_DIR / fname).strip(),
            f"M4K3-A8 RED: {fname} 缺失或为空 —— 终点版冻结件被删（历史版本共存可"
            "diff 铁律；261022-m5-k1 OQ-8：v11 原地冻结，删除 ≠ 迁移）。")
        self.assertIn(
            "openclaw-system-prompt.v7.md",
            _read_text(CARRIER),
            "M4K3-A8 RED: carrier 丢失 v7 沿革条目 —— 版本演进史唯一全量副本被删"
            "（历史版本可 diff 铁律; v8 后 v7 行须移入历史版本段保留）。")

    def test_a8_current_prompt_dual_trigger_and_boundary(self):
        """coverage: M4K3-A8 — 故事段双指令与意图边界（261022 语义换代）。

        SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1：旧语义 = 当前
        接线版 vN 文件故事段承载双指令 + 边界负例 + 影子口径 + 标题 ≤12 字；新语义
        = 静态语义单一事实源 workspace AGENTS.md 承载同组锚（§「故事」节窗口，
        内容驱动选节判法不变；「帮我记下来明天带跳绳」边界负例与 ≤12 字标题锚须
        逐条迁入——迁移只许换承载不许丢语义）。现树未迁入 = 设计态红。"""
        # SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1：承载面从「当前
        # 接线版 vN 文件」换为 workspace AGENTS.md（§ 故事节窗口）。
        agents_path = DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
        self.assertTrue(
            agents_path.is_file(),
            "M4K3-A8 RED(superseded): workspace-kidpal/AGENTS.md 不在场——静态语义"
            "单一事实源镜像缺失（261022-m5-k1 承载反转 K1-E1）。")
        body = _read_text(agents_path)
        self.assertTrue(body.strip(), "M4K3-A8 RED(superseded): AGENTS.md 为空（前置）。")
        window = _section_window(body, ("故事",), ("story",))
        self.assertIn(
            "我要讲故事", window,
            "M4K3-A8 RED(superseded): AGENTS.md 故事段丢失「我要讲故事」—— 既有指令"
            "零偏移被破（迁移逐条承载，261022-m5-k1 K1-E1）。")
        self.assertIn(
            "帮我记下来", window,
            "M4K3-A8 RED(superseded): AGENTS.md 故事段无「帮我记下来」第二指令 —— "
            "双指令批未迁入（S-F2.2 双入口触发词；261022-m5-k1 K1-E1）。")
        self.assertIn(
            "明天带跳绳", window,
            "M4K3-A8 RED(superseded): AGENTS.md 故事段无「帮我记下来明天带跳绳」"
            "边界负例 —— 意图边界措辞未迁入（notes ⑦: 该句 = 课表录入, 不得判开录; "
            "LLM 真判定归 live 目击, prompt 文本负例 = offline 可机判的生成端锚）。")
        self.assertIn(
            "story_list", window,
            "M4K3-A8 RED(superseded): AGENTS.md 故事段丢失 story_list 措辞 —— 影子"
            "口径零偏移被破（Q7: story 数据 front 机械推送单源, prompt 不教模型产 "
            "story 块）。")
        self.assertTrue(
            any(w in window for w in ("严禁生成", "不要生成", "禁生成", "不生成",
                                      "严禁产", "禁产")),
            "M4K3-A8 RED(superseded): AGENTS.md 故事段无「禁产 story JSON」禁令 —— "
            "影子口径被稀释。")
        self.assertIn(
            "12 字", window,
            "M4K3-A8 RED(superseded): AGENTS.md 故事段丢失标题 ≤12 字锚 —— 归档起"
            "标题规则零偏移被破（notes ⑧: v7 故事段既有, v8 零偏移继承；迁移逐条"
            "承载）。")

    def test_a8_v7_prompt_immutable(self):
        """coverage: M4K3-A8 守卫 — v7 本体零回改（E2「新文件不回改 v7」铁律齿;
        v6 零回改由 v7 齿承载, v7 零回改自本批起由本齿承载）。"""
        v7_path = DEPLOY_DIR / V7_NAME
        self.assertTrue(
            v7_path.is_file(),
            "M4K3-A8 RED: v7 文件被删除 —— 历史版本共存可 diff 铁律被破坏。")
        self.assertEqual(
            _sha256_raw(v7_path), V7_SHA256,
            "M4K3-A8 RED: v7 本体被回改 —— prompt v8 新文件不回改 v7（版本化演进; "
            "sha256 = 冻结时点 v7 内容）。")


if __name__ == "__main__":
    unittest.main(verbosity=2)
