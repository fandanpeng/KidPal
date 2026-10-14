"""Frozen acceptance tests — TaskPackage 261014-engine-stop-truncation-asr-filter
(subsystem: server/deploy/engine-patch; anchors: reviewed plan
delivery/taskpackages/261014-engine-stop-truncation-asr-filter/plan.md
[architect /triage-change 2026-10-14] + CRQ
docs/crq/tech/TECH-261014-engine-asr-hallucination-after-stop.md).

ET-a..g — engine host 断言：listen stop 收尾截断 + ASR 幻听串过滤。驱动
``server/deploy/engine-patch/connection.py`` 的**真实方法体**（AST 抽取）：
``_kidpal_on_listen_message``（行为齿 ET-a..d）、模块级
``_KIDPAL_ASR_HALLUCINATIONS`` + ``_kidpal_is_asr_hallucination``（ET-e）、
``chat`` / ``_route_message`` 的结构形状（结构齿 ET-f/ET-g）。

Defect of record（TECH-261014-engine-asr-hallucination-after-stop，2026-10-14
真机两轮逐字命中，设备串口与 ECS 引擎容器日志同刻）:
    14:38:43 收到listen消息：{"type":"listen","state":"stop"}   ← 设备正常收尾
    14:38:44 识别文本: Thank you.                               ← stop 之后 1 秒引擎仍产出 ASR
    14:38:44 大模型收到用户消息: Thank you.
    14:38:48 发送第一段语音: 不客气呀                              ← LLM 回复被推成 TTS
根因 = listenMessageHandler stop 分支在非流式 ASR 下直接触发缓冲尾音整段识别
（stop 分支无 auto 路径的最短音频门），stop 未截断识别链、未丢弃产出；静音段
ASR 幻听恒产 "Thank you." 被当用户话送 LLM。修法（plan 改动点 1-3）：
listen 帧旁路截断（非 manual 模式 stop → 置 _kidpal_listen_closed + 清
asr_audio + 非阻塞排空 asr_audio_queue，先清后转）+ chat 入口守卫（depth==0
丢弃 flag 置位轮与幻听串轮）+ 幻听串表（归一化整串相等，绝不子串）。

===========================================================================
齿清单（七条，ET-a..g == TaskPackage acceptance[] 逐条）:

  ET-a （行为齿，红基线齿）: stop 截断（auto 模式）——client_listen_mode !=
          manual 下 listen state=stop 后：_kidpal_listen_closed 置 True、
          asr_audio 清空、asr_audio_queue 排空（非阻塞 get_nowait 循环）、
          记 info「kidpal stop-truncation: …丢弃在途音频」。
  ET-b （承重齿，manual 豁免，毁稿级）: client_listen_mode="manual" 下同一
          stop 消息 → flag 不置、asr_audio/asr_audio_queue 零扰动。断言方向
          钉死「不动」：漏掉即 SAO-A2 故事归档转写被丢弃回归。
  ET-c: start 复位——listen state=start 后 _kidpal_listen_closed 复位 False
          （新聆听窗，正常对话轮不被上一轮截断态误杀）。
  ET-d: 非 listen / 畸形消息零扰动零异常——非 listen type、畸形 JSON、listen
          未知 state：静默返回、状态与 buffer/queue 零扰动、不抛异常。
  ET-e: 幻听串表——归一化（strip/lower/去中英标点）后**整串相等**命中
          {"thank you","thanks for watching","thank you for watching",
          "谢谢观看","请不吝点赞"}（含大小写/标点变体）；负锚（整串保护，
          绝不子串）：「谢谢你，这个字怎么写」「让我们一起说thank you吧」
          「thank you so much」（含 "thank you" 子串但整串不等）不命中。
  ET-f （结构齿）: chat 入口守卫——depth==0 的守卫（_kidpal_listen_closed 丢弃
          + 幻听串过滤，各自带 return）位于 self.dialogue.put 与 TTS FIRST
          入队之前；守卫仅 depth==0（_kidpal_listen_closed 引用不出 depth==0
          块）、守卫内不置 client_abort。HEAD 两处结构均不存在 → 天然红。
  ET-g （结构齿）: _route_message 拦截顺序——_kidpal_on_listen_message 调用
          位于 kidpal_speak 认领（_kidpal_try_speak）之后、handleTextMessage
          之前（先清后转承重顺序）。HEAD 结构不存在 → 天然红。

harness 模式: 循 ``server/acceptance/test_kidpal_speak_sync.acceptance.py``
先例（GRF/VR-A6 系）。connection.py 自带引擎依赖（websockets / opuslib_next /
core.* 等）**不可离线 import**；本件 AST 抽取目标方法体/模块级定义 ``exec``
进受控命名空间（真实控制流），self = SimpleNamespace 轻量替身（recording
logger.bind(tag=…).info/error + 引擎同形成员：asr_audio=list、
asr_audio_queue=queue.Queue）——离线、零 socket、零引擎依赖。结构齿对
``chat`` / ``_route_message`` 做纯 AST 形状检查（不执行）。

强度申报（缺口，逐条）:
  · 不覆盖真实 ASR provider 识别、TTS 出声、WS 接收链路——由 live-manual
    真机轮承载（human 手工部署 ECS 后按 plan 真机五项验，本 Pod 不跑）。
  · ET-a 的「先清后转」末端效果（handleTextMessage stop 分支读到空 asr_audio
    经 len()>0 门不进、stop 触发的 ASR 整体不发生）不直接断言（handleTextMessage
    是引擎上游文件、不在本包改动面）——由 ET-g 结构齿钉住调用顺序 + plan 部署
    前置机械核对（容器内 grep len(conn.asr_audio) 门）双重承载。行为等价假设
    显式化：len()>0 门存在于 0.9.6 参考树 listenMessageHandler（plan 已核验）。
  · 日志断言钉可观测前缀「kidpal stop-truncation」（部署/运维 grep 锚）而非
    全文逐字——防日志措辞微调造成假红；「丢弃在途音频」语义词同钉。
  · ET-a 例外面（stop 且 client_listen_mode 为空/其它值）：plan 语义为
    != "manual" 即截断，本件以 "auto" 钉正面、"manual" 钉豁免，其余值不另设齿。
  · AST 抽取执行，非整模块 import（与「connection.py 不可离线 import」既定
    事实同构，GRF-A2 / VR-A6 先例）；抽取失败（方法改名/结构漂移/未落地）以
    infra 红报出，返 acceptance-author 重钉，不由 builder/fixer 改齿。
  · conn 替身成员形状 == 引擎真实形状（connection.py __init__: asr_audio=[]
    list.clear、asr_audio_queue=queue.Queue.get_nowait、clear_queues 先例同款
    get_nowait 循环）；builder 若换容器类型须保持 clear()/get_nowait() 接口，
    否则本件红（属实现违约而非齿漂移）。

Run protocol（文件名两点须 importlib 模式；live 件不 auto-run 由 conftest 机械闸兜）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py \\
      server/acceptance/test_kidpal_speak_sync.acceptance.py
"""

from __future__ import annotations

import ast
import asyncio
import json
import queue
import re
import sys
import textwrap
import types
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
ENGINE_PATCH_CONNECTION = (
    REPO_ROOT / "server" / "deploy" / "engine-patch" / "connection.py"
)

# Frozen literals（plan 改动点 1-3 钉死的目标符号名）。
LISTEN_METHOD = "_kidpal_on_listen_message"
HALLUC_TABLE = "_KIDPAL_ASR_HALLUCINATIONS"
HALLUC_FN = "_kidpal_is_asr_hallucination"
CHAT_METHOD = "chat"
ROUTE_METHOD = "_route_message"
TRY_SPEAK_METHOD = "_kidpal_try_speak"
HANDLE_TEXT_FN = "handleTextMessage"
FLAG = "_kidpal_listen_closed"
CLIENT_ABORT = "client_abort"
LOG_MARKER = "kidpal stop-truncation"
LOG_MARKER_SEMANTIC = "丢弃在途音频"
MANUAL_MODE = "manual"

# 幻听串表种子（plan 改动点 3）与整串保护负锚。
HALLUC_SEEDS = (
    "thank you",
    "thanks for watching",
    "thank you for watching",
    "谢谢观看",
    "请不吝点赞",
)
# 大小写/标点/空白变体（归一化后须整串命中）。
HALLUC_VARIANTS = {
    "thank you": ("Thank you.", "  THANK YOU!  ", "thank you"),
    "thanks for watching": ("Thanks for watching!!", "THANKS FOR WATCHING."),
    "thank you for watching": ("Thank You For Watching!",),
    "谢谢观看": ("谢谢观看。", " 谢谢观看！"),
    "请不吝点赞": ("请不吝点赞。", "请不吝点赞，"),
}
# 整串保护负锚：绝不子串——含种子子串但整串不等，必须不命中。
HALLUC_NEGATIVES = (
    "谢谢你，这个字怎么写",
    "让我们一起说thank you吧",
    "thank you so much",  # 含 "thank you" 子串，整串不等 → 必须不命中
    "thanks for watching my video",  # 同理
)


# --------------------------------------------------------------------------- #
# connection.py 载入 + AST 抽取（不可离线 import → 抽真实定义 exec）             #
# --------------------------------------------------------------------------- #

_conn_cache = None


def _load_connection():
    """Return (src, tree, err). Parse error / missing file = infra RED."""
    global _conn_cache
    if _conn_cache is None:
        if not ENGINE_PATCH_CONNECTION.is_file():
            return None, None, (
                "server/deploy/engine-patch/connection.py missing — the engine "
                "deploy-copy of record is not landed in the repo."
            )
        src = ENGINE_PATCH_CONNECTION.read_text(encoding="utf-8", errors="replace")
        try:
            tree = ast.parse(src)
        except SyntaxError as e:  # noqa: PERF203 — a parse error is a RED, not a crash
            return None, None, f"connection.py failed to parse: {e!r}"
        _conn_cache = (src, tree, None)
    src, tree, err = _conn_cache
    return src, tree, err


def _find_func(tree, name):
    return next(
        (n for n in ast.walk(tree)
         if isinstance(n, (ast.AsyncFunctionDef, ast.FunctionDef))
         and n.name == name),
        None,
    )


def _load_listen_handler():
    """Extract the real ``_kidpal_on_listen_message`` (async method) body.

    Returns (async_callable|None, err).  找不到目标方法 = infra RED（方法未落地
    / 改名 / 结构漂移）。namespace 只供 stdlib 与 TAG——方法体引用引擎内部
    成员一律经 self，由替身提供。
    """
    src, tree, err = _load_connection()
    if err:
        return None, err
    node = _find_func(tree, LISTEN_METHOD)
    if node is None:
        return None, (
            f"connection.py has no method {LISTEN_METHOD} — HEAD 设计态红基线（改动点 1 "
            "listen 帧旁路截断入口未落地）或实现后 anchor drift（方法改名/删除 → re-pin "
            "with acceptance-author）。"
        )
    segment = ast.get_source_segment(src, node)
    if segment is None:
        return None, f"could not slice the source of {LISTEN_METHOD} — anchor drift."
    ns = {
        "json": json,
        "queue": queue,
        "asyncio": asyncio,
        "re": re,
        "TAG": "kidpal_stop_truncation_acceptance",
    }
    try:
        exec(compile(textwrap.dedent(segment), str(ENGINE_PATCH_CONNECTION), "exec"), ns)
    except Exception as e:  # noqa: BLE001 — an exec error here is a RED, not a crash
        return None, f"failed to exec the extracted {LISTEN_METHOD} body: {e!r}"
    fn = ns.get(LISTEN_METHOD)
    if not callable(fn):
        return None, f"extracted {LISTEN_METHOD} is not callable — anchor drift."
    return fn, None


def _load_hallucination_table():
    """Extract module-level ``_KIDPAL_ASR_HALLUCINATIONS`` + ``_kidpal_is_asr_hallucination``.

    Returns (namespace|None, err). namespace 内含表对象与判定函数（以及可能被
    判定函数引用的其它模块级 ``_kidpal/_KIDPAL`` 辅助定义，一并 exec 兜自包含）。
    """
    src, tree, err = _load_connection()
    if err:
        return None, err
    fn_node = _find_func(tree, HALLUC_FN)
    table_node = None
    for node in tree.body:  # top-level only（plan 改动点 3：模块级）
        if isinstance(node, ast.Assign) and any(
            isinstance(t, ast.Name) and t.id == HALLUC_TABLE for t in node.targets
        ):
            table_node = node
    if fn_node is None or table_node is None:
        missing = [n for n, has in (
            (HALLUC_TABLE, table_node is not None),
            (HALLUC_FN, fn_node is not None),
        ) if not has]
        return None, (
            f"connection.py top-level missing {missing} — HEAD 设计态红基线（改动点 3 "
            "幻听串表未落地）或实现后 anchor drift（定义改名/移位 → re-pin with "
            "acceptance-author）。"
        )
    ns = {"re": re, "json": json, "queue": queue, "asyncio": asyncio}
    # 判定函数可能引用的其它 _kidpal 模块级辅助（自包含兜底；plan 要求单收口在
    # patch 文件内）。收集所有顶层 _kidpal* / _KIDPAL* 定义一并 exec。
    extra_defs = [
        n for n in tree.body
        if isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef, ast.Assign))
        and any(
            (isinstance(t, ast.Name) and t.id.startswith(("_kidpal", "_KIDPAL")))
            for t in (n.targets if isinstance(n, ast.Assign) else [])
        )
        or (isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef))
            and n.name.startswith(("_kidpal", "_KIDPAL"))
            and n.name != HALLUC_FN)
    ]
    segments = [ast.get_source_segment(src, table_node),
                ast.get_source_segment(src, fn_node)]
    segments += [s for s in (ast.get_source_segment(src, n) for n in extra_defs)
                 if s]
    for seg in segments:
        try:
            exec(compile(textwrap.dedent(seg), str(ENGINE_PATCH_CONNECTION), "exec"), ns)
        except Exception as e:  # noqa: BLE001 — RED, not a crash
            return None, f"failed to exec extracted hallucination-table defs: {e!r}"
    fn = ns.get(HALLUC_FN)
    table = ns.get(HALLUC_TABLE)
    if not callable(fn) or table is None:
        return None, "extracted hallucination defs not callable/bound — anchor drift."
    return ns, None


# --------------------------------------------------------------------------- #
# 结构齿 AST 形状检查（chat 守卫 / _route_message 拦截顺序）                     #
# --------------------------------------------------------------------------- #

def _call_func_name(call_node):
    func = call_node.func
    if isinstance(func, ast.Name):
        return func.id
    if isinstance(func, ast.Attribute):
        return func.attr
    return None


def _test_references(test_node, name):
    return any(
        (isinstance(n, ast.Attribute) and n.attr == name)
        or (isinstance(n, ast.Name) and n.id == name)
        for n in ast.walk(test_node)
    )


def _has_return_in_body(if_node):
    return any(
        isinstance(n, ast.Return)
        for stmt in if_node.body
        for n in ast.walk(stmt)
    )


def _is_depth0_test(test_node):
    return (
        isinstance(test_node, ast.Compare)
        and len(test_node.ops) == 1
        and isinstance(test_node.ops[0], ast.Eq)
        and isinstance(test_node.left, ast.Name)
        and test_node.left.id == "depth"
        and isinstance(test_node.comparators[0], ast.Constant)
        and test_node.comparators[0].value == 0
    )


def _chat_guard_findings(tree):
    """Inspect ``chat`` for the depth==0 guard shape (plan 改动点 2).

    Returns (findings|None, err). findings keys:
      depth0_if, guard_flag, guard_halluc, first_dialogue_put, first_tts_put,
      flag_refs_outside (lineno list), client_abort_assigns (lineno list).
    """
    chat = _find_func(tree, CHAT_METHOD)
    if chat is None:
        return None, f"connection.py has no method {CHAT_METHOD} — anchor drift."
    depth0 = None
    for node in ast.walk(chat):
        if isinstance(node, ast.If) and _is_depth0_test(node.test):
            if depth0 is None or node.lineno < depth0.lineno:
                depth0 = node
    if depth0 is None:
        return None, "chat 中无 depth == 0 分支 — 结构漂移。"

    sub_ifs = [n for n in ast.walk(depth0)
               if isinstance(n, ast.If) and n is not depth0]
    guard_flag = next((n for n in sub_ifs
                       if _test_references(n.test, FLAG)
                       and _has_return_in_body(n)), None)
    guard_halluc = next((n for n in sub_ifs
                         if any(isinstance(x, ast.Call)
                                and _call_func_name(x) == HALLUC_FN
                                for x in ast.walk(n.test))
                         and _has_return_in_body(n)), None)

    def _first_put_lineno(receiver_attr):
        linenos = [
            n.lineno for n in ast.walk(chat)
            if isinstance(n, ast.Call)
            and isinstance(n.func, ast.Attribute) and n.func.attr == "put"
            and isinstance(n.func.value, ast.Attribute)
            and n.func.value.attr == receiver_attr
        ]
        return min(linenos) if linenos else None

    flag_refs_outside = sorted({
        n.lineno for n in ast.walk(chat)
        if isinstance(n, ast.Attribute) and n.attr == FLAG
        and not (depth0.lineno <= n.lineno <= depth0.end_lineno)
    })
    client_abort_assigns = sorted({
        n.lineno for n in ast.walk(depth0)
        if isinstance(n, (ast.Assign, ast.AugAssign))
        and any(isinstance(t, ast.Attribute) and t.attr == CLIENT_ABORT
                for t in ast.walk(n))
    })
    return {
        "depth0_if": depth0,
        "guard_flag": guard_flag,
        "guard_halluc": guard_halluc,
        "first_dialogue_put": _first_put_lineno("dialogue"),
        "first_tts_put": _first_put_lineno("tts_text_queue"),
        "flag_refs_outside": flag_refs_outside,
        "client_abort_assigns": client_abort_assigns,
    }, None


def _route_findings(tree):
    """Inspect ``_route_message`` for the interception ordering (plan 改动点 1)."""
    route = _find_func(tree, ROUTE_METHOD)
    if route is None:
        return None, f"connection.py has no method {ROUTE_METHOD} — anchor drift."
    linenos = {}
    for target in (LISTEN_METHOD, TRY_SPEAK_METHOD, HANDLE_TEXT_FN):
        linenos[target] = sorted({
            n.lineno for n in ast.walk(route)
            if isinstance(n, ast.Call) and _call_func_name(n) == target
        })
    return linenos, None


# --------------------------------------------------------------------------- #
# I/O doubles — recording logger + 引擎同形 conn 替身                           #
# --------------------------------------------------------------------------- #

class _RecordingLogger:
    """Records .bind(tag=...).info/error calls exactly as the method body uses them."""

    def __init__(self):
        self._entries = []  # list[(level, message)]

    def bind(self, **_kw):
        return self

    def info(self, message):
        self._entries.append(("info", message))

    def error(self, message):
        self._entries.append(("error", message))

    def at_level(self, level):
        return [m for lvl, m in self._entries if lvl == level]


def _make_conn(mode="auto", flag=False, buffer=("chunk-1", "chunk-2"),
               queue_items=("q-1", "q-2")):
    """conn 替身：成员形状 == 引擎真实形状（connection.py __init__ :136/:177/:178）。

    asr_audio = list（.clear() 语义）、asr_audio_queue = queue.Queue
    （.get_nowait() 语义，clear_queues 先例同款）。flag 缺省 False == plan
    「__init__ 状态区初值 False」。
    """
    q = queue.Queue()
    for item in queue_items:
        q.put(item)
    return types.SimpleNamespace(
        logger=_RecordingLogger(),
        client_listen_mode=mode,
        _kidpal_listen_closed=flag,
        asr_audio=list(buffer),
        asr_audio_queue=q,
    )


def _invoke_conn(fn, conn, message):
    """Drive the extracted async method against a prebuilt conn double."""
    try:
        returned = asyncio.run(fn(conn, message))
    except Exception as e:  # noqa: BLE001 — surfaced to the tooth as a failure
        return conn, None, e
    return conn, returned, None


def _stop_msg():
    return json.dumps({"type": "listen", "state": "stop"}, ensure_ascii=False)


def _start_msg():
    return json.dumps({"type": "listen", "state": "start"}, ensure_ascii=False)


# --------------------------------------------------------------------------- #
# ET-a..d 行为齿（AST 抽取 _kidpal_on_listen_message 真实方法体）                #
# --------------------------------------------------------------------------- #

class EngineStopTruncationAcceptance(unittest.TestCase):

    maxDiff = None

    @staticmethod
    def _method():
        """Fetch the extracted _kidpal_on_listen_message (cached in loader)."""
        fn, err = _load_listen_handler()
        return fn, err

    def _red(self, prefix, err):
        self.fail(f"{prefix} RED: " + (err or "connection.py method not loadable"))

    # ------------------------------------------------------------------ #
    # ET-a · stop 截断（auto 模式）                                        #
    # ------------------------------------------------------------------ #

    def test_et_a_auto_mode_stop_truncates(self):
        """coverage: ET-a — auto 模式下 listen state=stop：_kidpal_listen_closed 置
        True、asr_audio 清空、asr_audio_queue 排空（非阻塞 get_nowait 循环）、记
        info「kidpal stop-truncation …丢弃在途音频」。

        HEAD 设计态红 = 方法不存在（改动点 1 未落地），infra 红。"""
        fn, err = self._method()
        if fn is None:
            self._red("ET-a", err)

        conn = _make_conn(mode="auto", flag=False,
                          buffer=("pcm-1", "pcm-2", "pcm-3"),
                          queue_items=("frame-1", "frame-2"))
        _conn, returned, raised = _invoke_conn(fn, conn, _stop_msg())
        self.assertIsNone(
            raised,
            f"ET-a RED: listen stop 在 auto 模式抛异常 {raised!r} —— 截断路径不得抛。",
        )
        self.assertIs(
            conn._kidpal_listen_closed, True,
            f"ET-a RED: stop 后 _kidpal_listen_closed = "
            f"{conn._kidpal_listen_closed!r}（期望 True）—— 截断标志未置位，stop 收尾后"
            "在途音频照旧进识别链（TECH-261014 defect of record 不消）。",
        )
        self.assertEqual(
            conn.asr_audio, [],
            f"ET-a RED: stop 后 asr_audio = {conn.asr_audio!r}（期望空）—— 缓冲尾音未被"
            "丢弃，handleTextMessage stop 分支仍会读到音频触发整段识别。",
        )
        self.assertTrue(
            conn.asr_audio_queue.empty(),
            "ET-a RED: stop 后 asr_audio_queue 未排空 —— 在途 PCM 帧残留，stop 触发的 "
            "ASR 仍会发生。",
        )
        infos = conn.logger.at_level("info")
        self.assertTrue(
            any(LOG_MARKER in m for m in infos),
            f"ET-a RED: 未记 info「{LOG_MARKER}…」（info={infos!r}）—— 截断动作不可观测，"
            "部署/运维 grep 锚缺失。",
        )
        self.assertTrue(
            any(LOG_MARKER_SEMANTIC in m for m in infos),
            f"ET-a RED: info 缺「{LOG_MARKER_SEMANTIC}」语义词（info={infos!r}）。",
        )

    # ------------------------------------------------------------------ #
    # ET-b · manual 豁免（承重齿，毁稿级——断言方向钉死「不动」）             #
    # ------------------------------------------------------------------ #

    def test_et_b_manual_mode_stop_is_exempt(self):
        """coverage: ET-b — manual 模式下同一 stop 消息：flag 不置、asr_audio /
        asr_audio_queue 零扰动（manual 的 stop 触发 ASR 是 front pending-stop
        归档的承重通道；漏掉即 SAO-A2「转写被丢弃 = 毁稿」回归）。"""
        fn, err = self._method()
        if fn is None:
            self._red("ET-b", err)

        conn = _make_conn(mode=MANUAL_MODE, flag=False,
                          buffer=("story-pcm-1", "story-pcm-2"),
                          queue_items=("story-frame-1",))
        _conn, returned, raised = _invoke_conn(fn, conn, _stop_msg())
        self.assertIsNone(
            raised,
            f"ET-b RED: manual 模式下 listen stop 抛异常 {raised!r} —— 豁免路径不得抛。",
        )
        self.assertIs(
            conn._kidpal_listen_closed, False,
            f"ET-b RED: manual 模式下 _kidpal_listen_closed 被置为 "
            f"{conn._kidpal_listen_closed!r}（必须保持 False）—— 毁稿级回归：故事归档"
            "转写通道（front pending-stop 经 stop 触发 ASR 下行）被截断，SAO-A2 "
            "「转写被丢弃」。",
        )
        self.assertEqual(
            conn.asr_audio, ["story-pcm-1", "story-pcm-2"],
            f"ET-b RED: manual 模式下 asr_audio 被清成 {conn.asr_audio!r} —— 故事尾音"
            "缓冲被毁，归档转写丢失。",
        )
        self.assertFalse(
            conn.asr_audio_queue.empty(),
            "ET-b RED: manual 模式下 asr_audio_queue 被排空 —— 在途故事音频帧被毁。",
        )
        self.assertEqual(
            [m for m in conn.logger.at_level("info") if LOG_MARKER in m],
            [],
            f"ET-b RED: manual 模式下仍记「{LOG_MARKER}」info —— 截断动作在豁免模式被"
            "触发（状态未动但日志谎报）。",
        )

    # ------------------------------------------------------------------ #
    # ET-c · start 复位（新聆听窗）                                        #
    # ------------------------------------------------------------------ #

    def test_et_c_start_resets_flag(self):
        """coverage: ET-c — listen state=start 后 _kidpal_listen_closed 复位 False
        （新聆听窗：正常对话轮不被上一轮截断态误杀）。auto 与 manual 两模式都复位
        （start 复位不设豁免——plan 改动点 1 仅 stop 分支带 manual 条件）。"""
        fn, err = self._method()
        if fn is None:
            self._red("ET-c", err)

        for mode in ("auto", MANUAL_MODE):
            conn = _make_conn(mode=mode, flag=True, buffer=(), queue_items=())
            _conn, _returned, raised = _invoke_conn(fn, conn, _start_msg())
            self.assertIsNone(
                raised,
                f"ET-c RED: {mode} 模式 listen start 抛异常 {raised!r}。",
            )
            self.assertIs(
                conn._kidpal_listen_closed, False,
                f"ET-c RED: {mode} 模式 listen start 后 _kidpal_listen_closed = "
                f"{conn._kidpal_listen_closed!r}（期望 False）—— 新聆听窗未复位，上一轮"
                "截断态会误杀下一轮正常对话（plan 回归风险 1）。",
            )

    # ------------------------------------------------------------------ #
    # ET-d · 非 listen / 畸形消息零扰动零异常                               #
    # ------------------------------------------------------------------ #

    def test_et_d_non_listen_and_malformed_zero_disturbance(self):
        """coverage: ET-d — 非 listen type、畸形 JSON、listen 未知 state：静默返回、
        状态与 buffer/queue 零扰动、不抛异常（旁路钩子绝不能扰动上游路由）。"""
        fn, err = self._method()
        if fn is None:
            self._red("ET-d", err)

        cases = [
            # (名称, 消息, 预置 flag)
            ("非 listen type（stt）", json.dumps({"type": "stt", "text": "你好"},
                                                ensure_ascii=False), False),
            ("非 listen type（kidpal_speak）",
             json.dumps({"type": "kidpal_speak", "text": "提醒文案"},
                        ensure_ascii=False), True),
            ("畸形 JSON", "{ this is not json", True),
            ("JSON 非 dict", "[1, 2]", False),
            ("listen 未知 state", json.dumps({"type": "listen", "state": "detect"},
                                             ensure_ascii=False), True),
        ]
        for name, message, preset_flag in cases:
            with self.subTest(case=name):
                conn = _make_conn(mode="auto", flag=preset_flag,
                                  buffer=("keep-1",), queue_items=("keep-q-1",))
                _conn, _returned, raised = _invoke_conn(fn, conn, message)
                self.assertIsNone(
                    raised,
                    f"ET-d RED[{name}]: 抛异常 {raised!r} —— 旁路钩子必须静默返回。",
                )
                self.assertIs(
                    conn._kidpal_listen_closed, preset_flag,
                    f"ET-d RED[{name}]: _kidpal_listen_closed 被扰动为 "
                    f"{conn._kidpal_listen_closed!r}（预置 {preset_flag!r}）。",
                )
                self.assertEqual(
                    conn.asr_audio, ["keep-1"],
                    f"ET-d RED[{name}]: asr_audio 被扰动为 {conn.asr_audio!r}。",
                )
                self.assertFalse(
                    conn.asr_audio_queue.empty(),
                    f"ET-d RED[{name}]: asr_audio_queue 被扰动（排空）。",
                )


# --------------------------------------------------------------------------- #
# ET-e 行为齿（模块级幻听串表，整串相等，绝不子串）                              #
# --------------------------------------------------------------------------- #

class AsrHallucinationFilterAcceptance(unittest.TestCase):

    maxDiff = None

    @staticmethod
    def _load():
        return _load_hallucination_table()

    def _red(self, err):
        self.fail("ET-e RED: " + (err or "hallucination table not loadable"))

    def test_et_e_hallucination_table_whole_string_match(self):
        """coverage: ET-e — 归一化（strip/lower/去中英标点）后整串相等：五个种子串
        命中（含大小写/标点/空白变体）；整串保护负锚（含「谢谢你，这个字怎么写」
        「让我们一起说thank you吧」，以及含种子子串但整串不等的
        「thank you so much」等）不命中——绝不子串。"""
        ns, err = self._load()
        if ns is None:
            self._red(err)
        is_halluc = ns[HALLUC_FN]

        # 正面：种子串 + 变体（归一化后整串相等 → 命中）。
        for seed, variants in HALLUC_VARIANTS.items():
            for text in (seed,) + variants:
                with self.subTest(kind="positive", text=text):
                    self.assertTrue(
                        is_halluc(text),
                        f"ET-e RED: 幻听串 {text!r} 未命中 —— 种子 {seed!r} 的变体须在"
                        "归一化（strip/lower/去中英标点）后整串相等命中。",
                    )

        # 负面：整串保护（绝不子串）——含种子子串但整串不等的都必须放行。
        for text in HALLUC_NEGATIVES:
            with self.subTest(kind="negative", text=text):
                self.assertFalse(
                    is_halluc(text),
                    f"ET-e RED: 合法语句 {text!r} 被误杀 —— 整串保护被破坏（匹配退化成"
                    "子串）。「谢谢你，这个字怎么写」「让我们一起说thank you吧」是查字/"
                    "对话轮的真实用户话，误杀即正常功能回归。",
                )


# --------------------------------------------------------------------------- #
# ET-f / ET-g 结构齿（AST 形状检查，不执行）                                    #
# --------------------------------------------------------------------------- #

class StructuralOrderAcceptance(unittest.TestCase):

    maxDiff = None

    def _load_tree(self):
        _src, tree, err = _load_connection()
        return tree, err

    # ------------------------------------------------------------------ #
    # ET-f · chat 入口守卫结构齿                                          #
    # ------------------------------------------------------------------ #

    def test_et_f_chat_guard_before_dialogue_put_and_tts_enqueue(self):
        """coverage: ET-f — chat 的 depth==0 守卫（_kidpal_listen_closed 丢弃 +
        幻听串过滤，各自带 return）位于 self.dialogue.put 与 TTS FIRST 入队
        （self.tts.tts_text_queue.put）之前；守卫仅 depth==0（flag 引用不出
        depth==0 块）、守卫内不置 client_abort（避免波及 stop 后 front 注入的
        合法 speak/story 下发）。HEAD 守卫不存在 → 天然红。"""
        tree, err = self._load_tree()
        if err:
            self.fail("ET-f RED: " + err)
        findings, err = _chat_guard_findings(tree)
        if err:
            self.fail("ET-f RED: " + err)

        if findings["guard_flag"] is None:
            self.fail(
                "ET-f RED: chat 的 depth==0 块内无 _kidpal_listen_closed 丢弃守卫"
                "（带 return 的 if）—— 改动点 2 守卫缺失，stop 收尾后已在途的 ASR 轮"
                "（~1s 竞态窗）照进 LLM/TTS。"
            )
        if findings["guard_halluc"] is None:
            self.fail(
                "ET-f RED: chat 的 depth==0 块内无幻听串过滤守卫（调用 "
                f"{HALLUC_FN} 且带 return 的 if）—— 幻听串放行进 LLM/TTS。"
            )

        guard_end = max(findings["guard_flag"].end_lineno,
                        findings["guard_halluc"].end_lineno)
        for label, put_lineno in (
            ("self.dialogue.put（用户话入上下文）", findings["first_dialogue_put"]),
            ("self.tts.tts_text_queue.put（TTS FIRST 入队）", findings["first_tts_put"]),
        ):
            if put_lineno is None:
                self.fail(f"ET-f RED: chat 内找不到 {label} 调用 —— 结构漂移。")
            self.assertLess(
                guard_end, put_lineno,
                f"ET-f RED: depth==0 守卫（end_lineno={guard_end}）不在 {label}"
                f"（lineno={put_lineno}）之前 —— 守卫后置则用户话已入上下文/TTS 已入队，"
                "丢弃失效（plan 改动点 2 承重顺序）。",
            )

        self.assertEqual(
            findings["flag_refs_outside"], [],
            "ET-f RED: _kidpal_listen_closed 在 chat 的 depth==0 块之外被引用"
            f"（lineno={findings['flag_refs_outside']}）—— 守卫须仅 depth==0，递归轮"
            "（工具调用子轮）不得被拦（plan：守卫仅 depth==0）。",
        )
        self.assertEqual(
            findings["client_abort_assigns"], [],
            "ET-f RED: 守卫内赋值 client_abort（lineno="
            f"{findings['client_abort_assigns']}）—— plan 禁止（避免波及 stop 后 front"
            "注入的合法 speak/story 下发），须用自有标志。",
        )

    # ------------------------------------------------------------------ #
    # ET-g · _route_message 拦截顺序结构齿                                 #
    # ------------------------------------------------------------------ #

    def test_et_g_route_interception_between_claim_and_handletext(self):
        """coverage: ET-g — _route_message 中 _kidpal_on_listen_message 调用位于
        kidpal_speak 认领（_kidpal_try_speak）之后、handleTextMessage 之前
        （先清后转承重顺序：认领的内部指令不被截断扰动；handleTextMessage 的
        stop 分支读到已清空的 asr_audio 经 len()>0 门不进）。HEAD 拦截调用不存在
        → 天然红。"""
        tree, err = self._load_tree()
        if err:
            self.fail("ET-g RED: " + err)
        linenos, err = _route_findings(tree)
        if err:
            self.fail("ET-g RED: " + err)

        listen_calls = linenos[LISTEN_METHOD]
        claim_calls = linenos[TRY_SPEAK_METHOD]
        handle_calls = linenos[HANDLE_TEXT_FN]
        if not listen_calls:
            self.fail(
                "ET-g RED: _route_message 中无 _kidpal_on_listen_message 拦截调用 —— "
                "改动点 1 未接线（listen 帧旁路截断不发生），stop 收尾截断整体失效。"
            )
        if not claim_calls:
            self.fail(
                "ET-g RED: _route_message 中无 _kidpal_try_speak 认领调用 —— 结构漂移"
                "（裁定二的提醒语音认领面丢失）。"
            )
        if not handle_calls:
            self.fail(
                "ET-g RED: _route_message 中无 handleTextMessage 调用 —— 结构漂移"
                "（上游消息路由面丢失）。"
            )
        first_listen = min(listen_calls)
        last_claim = max(claim_calls)
        first_handle = min(handle_calls)
        self.assertGreater(
            first_listen, last_claim,
            f"ET-g RED: _kidpal_on_listen_message（lineno={first_listen}）不在 "
            f"_kidpal_try_speak 认领（lineno={last_claim}）之后 —— 认领的内部指令先被"
            "截断逻辑扰动则提醒语音链面被改（plan：认领之后调用）。",
        )
        self.assertLess(
            first_listen, first_handle,
            f"ET-g RED: _kidpal_on_listen_message（lineno={first_listen}）不在 "
            f"handleTextMessage（lineno={first_handle}）之前 —— 先清后转顺序破坏，"
            "stop 分支读到的还是未清空缓冲（TECH-261014 根因不消）。",
        )


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
