"""Frozen acceptance tests — TaskPackage 261014-voice-round-entry-exit-fixes
(subsystem: server/deploy/engine-patch; anchor:
docs/crq/tech/TECH-261014-kidpal-speak-await-nonetype.md).

VR-A6 — engine host 断言：以 stub ``speak_txt``（**同步、返回 None**）驱动
``server/deploy/engine-patch/connection.py`` 的 ``_kidpal_try_speak``，断言
「无异常 + 返回 True + 认领路径不回归（kidpal_speak 认领 + 空文案认领两形态）」。

Defect of record（TECH-261014-kidpal-speak-await-nonetype，2026-10-14 ECS 引擎容器 24h
日志三轮提醒逐条命中）:
    [core.connection]-ERROR-kidpal_speak 播报注入失败: object NoneType can't be used
    in 'await' expression (提醒降级：弹窗+提示音仍必达)
根因 = ``connection.py`` 的 ``_kidpal_try_speak`` 内 ``await speak_txt(self, text.strip())``
—— ``speak_txt``（引擎原生 ``core/handle/intentHandler.py``）是**同步函数**：调用先行完成
（语音发送任务已排队）、返回 None，随后对 None 求 await 即抛 TypeError 进 except 记 ERROR。
主链不受影响（语音仍照常合成下发），但 ERROR 文案自称「播报注入失败」与实际相反，每次运维
对账提醒链路都被误导一次。

修法（plan 改动点 3）: ``await speak_txt(...)`` → 同步调用 ``speak_txt(...)``（或按返回值
是否 coroutine 判别以兼容引擎未来改异步）；外层 try/except 降级语义（V20「弹窗+提示音必达，
语音为可选增强」）**保持不变**。

===========================================================================
齿清单（三条，行为级驱动真实方法体）：

  VR-A6-a （正钉，红基线齿）: 同步 stub speak_txt（返回 None）→ 无异常、返回 True、
          speak_txt 被调用且收到 strip 后文案、**零 ERROR 日志**（defect of record 本体）。
          HEAD 设计态红 = 现行 ``await speak_txt(...)`` 对 None 求值抛 TypeError 进 except 记
          ERROR（断言「零 ERROR」即红）。
  VR-A6-b （认领路径不回归）: kidpal_speak 空文案认领（返回 True、不调 speak_txt、记 info
          「无文案」）+ 非认领形态（非 kidpal_speak type / JSON 非 dict / 文本非 str /
          畸形 JSON）返回 False——引擎内部指令认领面零回归。
  VR-A6-c （V20 降级守卫）: speak_txt 抛异常 → 仍返回 True 且记 ERROR（提醒投递不得被语音
          拖住，弹窗+提示音必达）——外层 try/except 语义不放松。

harness 模式: 循 ``server/acceptance/archive/test_g_relay_fix.acceptance.py``（G 包先例）。
``connection.py`` 自带引擎依赖（websockets / opuslib_next / core.* 等）**不可离线 import**
（GRF-A2 carrier 齿同判）。本件据此 **AST 抽出 ``_kidpal_try_speak`` 真实方法体**、
``exec`` 进受控命名空间运行（真实控制流、真实 except 分支），并注入 stub 模块
``core.handle.intentHandler`` 提供同步 ``speak_txt``——离线、零 socket、零引擎依赖。
self = 轻量替身（仅 ``.logger.bind(tag=...).info/error`` 记录面）；方法体不触及其它 self 成员。

强度申报（缺口，逐条）:
  · 本件钉的是 ``_kidpal_try_speak`` 的**出口可观测面**（返回值 / speak_txt 调用 / 日志级别）；
    不覆盖真实 edge-tts provider 出声（归 live 轮——human 人工部署 ECS 后同轮闭账）。
  · 方法体经 AST 抽取执行（非整模块 import）——与「connection.py 不可离线 import」的既定
    事实同构（GRF-A2 / DF-A1 先例）；抽取失败（方法改名/结构漂移）以 infra 红报出，返
    acceptance-author 重钉，不由 builder/fixer 改齿。
  · 行为等价假设显式化: 同步 stub 的调用形状（``callable(conn, text)`` + 返回 None）== 引擎
    原生 ``intentHandler.speak_txt`` 的实测形状（callsite ``speak_txt(self, text)`` 同步返回
    None，CRQ 日志时序双证）；若引擎侧改异步，plan 允许「按返回值是否 coroutine 判别」的兼容
    形，本件同步 stub 仍成立（不应被误红）。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_kidpal_speak_sync.acceptance.py
"""

from __future__ import annotations

import ast
import asyncio
import contextlib
import json
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

# Frozen literals (defect of record + engine-side kidpal_speak contract).
KIDPAL_SPEAK_TYPE = "kidpal_speak"
METHOD_NAME = "_kidpal_try_speak"

# The engine ``core.handle.intentHandler`` import target is stubbed (模块不在本仓, 引擎侧
# 依赖); the method body does ``from core.handle.intentHandler import speak_txt`` at call
# time — installing these three module names in sys.modules satisfies it offline.
_CORE_STUB_NAMES = ("core", "core.handle", "core.handle.intentHandler")

# A reminder speech text exactly as the front injects it (§三 speech 原文形态)；断言 strip。
_REMINDER_TEXT_RAW = "  明天要带画具哦，美术课要用  "
_REMINDER_TEXT_STRIPPED = "明天要带画具哦，美术课要用"


# --------------------------------------------------------------------------- #
# AST extraction of the real method body (connection.py is not importable)     #
# --------------------------------------------------------------------------- #

_method_cache = None


def _load_kidpal_try_speak():
    """Extract the real ``_kidpal_try_speak`` body from connection.py via AST and exec it.

    Returns (async_callable|None, err).  connection.py carries engine dependencies
    (websockets / opuslib_next / core.* …) that are NOT importable offline (GRF-A2
    precedent), so the module cannot be ``import``-ed; the method body itself only
    references ``json`` / ``KIDPAL_SPEAK_TYPE`` / ``TAG`` / ``self.logger``, all of
    which are provided here.  An extraction failure is an infra RED for a drifted
    anchor (method renamed/moved) — re-pin with acceptance-author, never edit the tooth.
    """
    global _method_cache
    if _method_cache is not None:
        return _method_cache
    if not ENGINE_PATCH_CONNECTION.is_file():
        return None, (
            "server/deploy/engine-patch/connection.py missing — the engine deploy-copy "
            "of record is not landed in the repo."
        )
    src = ENGINE_PATCH_CONNECTION.read_text(encoding="utf-8", errors="replace")
    try:
        tree = ast.parse(src)
    except SyntaxError as e:  # noqa: PERF203 — a parse error is a RED, not a crash
        return None, f"connection.py failed to parse: {e!r}"
    node = next(
        (n for n in ast.walk(tree)
         if isinstance(n, ast.AsyncFunctionDef) and n.name == METHOD_NAME),
        None,
    )
    if node is None:
        return None, (
            f"connection.py has no async method {METHOD_NAME} — anchor drift (the "
            "kidpal_speak claim path was renamed/removed); re-pin with acceptance-author."
        )
    segment = ast.get_source_segment(src, node)
    if segment is None:
        return None, (
            f"could not slice the source of {METHOD_NAME} (ast.get_source_segment "
            "returned None) — anchor drift."
        )
    ns = {
        "json": json,
        "KIDPAL_SPEAK_TYPE": KIDPAL_SPEAK_TYPE,
        "TAG": "kidpal_speak_sync_acceptance",
    }
    try:
        exec(compile(textwrap.dedent(segment), str(ENGINE_PATCH_CONNECTION), "exec"), ns)
    except Exception as e:  # noqa: BLE001 — an exec error here is a RED, not a crash
        return None, f"failed to exec the extracted {METHOD_NAME} body: {e!r}"
    fn = ns.get(METHOD_NAME)
    if not callable(fn):
        return None, f"extracted {METHOD_NAME} is not callable — anchor drift."
    _method_cache = (fn, None)
    return _method_cache


# --------------------------------------------------------------------------- #
# I/O doubles — a recording logger and a stub speak_txt target                 #
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


@contextlib.contextmanager
def _speak_txt_stub(speak_fn, call_log):
    """Install a stub ``core.handle.intentHandler.speak_txt`` for the duration.

    The extracted method body does ``from core.handle.intentHandler import speak_txt``
    at call time; installing the three dotted module names in sys.modules satisfies it
    with zero engine imports.  Restored on exit (no cross-test pollution).
    """
    saved = {name: sys.modules.get(name) for name in _CORE_STUB_NAMES}
    core = types.ModuleType("core")
    core_handle = types.ModuleType("core.handle")
    core_intent = types.ModuleType("core.handle.intentHandler")

    def speak_txt(conn, text):
        call_log.append((conn, text))
        return speak_fn(conn, text)

    core_intent.speak_txt = speak_txt
    core.handle = core_handle
    core_handle.intentHandler = core_intent
    sys.modules["core"] = core
    sys.modules["core.handle"] = core_handle
    sys.modules["core.handle.intentHandler"] = core_intent
    try:
        yield
    finally:
        for name, prev in saved.items():
            if prev is None:
                sys.modules.pop(name, None)
            else:
                sys.modules[name] = prev


def _invoke(fn, message):
    """Drive the extracted method; return (returned, raised_exception_or_None)."""
    conn = types.SimpleNamespace(logger=_RecordingLogger())
    try:
        returned = asyncio.run(fn(conn, message))
    except Exception as e:  # noqa: BLE001 — surfaced to the tooth as a failure, not a crash
        return conn, None, e
    return conn, returned, None


# --------------------------------------------------------------------------- #
# VR-A6 teeth                                                                   #
# --------------------------------------------------------------------------- #

class KidpalSpeakSyncAcceptance(unittest.TestCase):

    maxDiff = None

    @staticmethod
    def _method():
        """Fetch the extracted method (cached).  Kept a *staticmethod* — storing the plain
        function as a class attribute would make ``self.method`` a BOUND method (the
        function-descriptor protocol pre-fills an instance), corrupting the arity."""
        fn, err = _load_kidpal_try_speak()
        return fn, err

    # ------------------------------------------------------------------ #
    # VR-A6-a · 正钉（红基线齿）: 同步 speak_txt 零 ERROR
    # ------------------------------------------------------------------ #

    def test_vr_a6_sync_speak_txt_takes_no_error_path(self):
        """coverage: VR-A6 — 同步 stub speak_txt（返回 None）驱动 _kidpal_try_speak：无异常、
        返回 True、speak_txt 被调用且收到 strip 后文案、零 ERROR 日志（defect of record 消）。

        HEAD 设计态红 = 现行 ``await speak_txt(...)`` 对 None 求值抛 TypeError 进 except 记
        ERROR（TECH-261014-kidpal-speak-await-nonetype 本体）。"""
        fn, err = self._method()
        if fn is None:
            self.fail("VR-A6 RED: " + (err or "connection.py method not loadable"))

        call_log = []
        conn, returned, raised = None, None, None
        with _speak_txt_stub(lambda _conn, _text: None, call_log):
            conn, returned, raised = _invoke(
                fn,
                json.dumps({"type": KIDPAL_SPEAK_TYPE, "text": _REMINDER_TEXT_RAW},
                           ensure_ascii=False),
            )
        # 无异常（外层 try/except 降级语义不被击穿；同步 speak_txt 返回 None 不得触发 await 求值）。
        self.assertIsNone(
            raised,
            f"VR-A6 RED: _kidpal_try_speak 对同步 speak_txt 抛异常 {raised!r} —— 外层 "
            "try/except 降级语义（V20「弹窗+提示音必达」）被击穿。",
        )
        # 返回 True（该条已由引擎认领，绝不回流 handleTextMessage 当孩子说的话进 ASR/LLM）。
        self.assertIs(
            returned, True,
            f"VR-A6 RED: _kidpal_try_speak 返回 {returned!r}（期望 True）—— kidpal_speak "
            "认领语义断裂（认领 = True；非认领 = False）。",
        )
        # speak_txt 真实被调用且收到 strip 后文案（提醒语音注入链不断）。
        self.assertEqual(
            len(call_log), 1,
            f"VR-A6 RED: speak_txt 被调用 {len(call_log)} 次（期望恰 1）—— 提醒语音注入链断裂。",
        )
        self.assertEqual(
            call_log[0][1], _REMINDER_TEXT_STRIPPED,
            f"VR-A6 RED: speak_txt 收到文案 {call_log[0][1]!r}（期望 strip 后 "
            f"{_REMINDER_TEXT_STRIPPED!r}）—— 提醒文案原文丢失/未 strip。",
        )
        # defect of record 本体: 零 ERROR（同步调用不得走「播报注入失败」的 except 分支）。
        errors = conn.logger.at_level("error")
        self.assertEqual(
            errors, [],
            "VR-A6 RED: 同步 speak_txt（返回 None）触发 except 记 ERROR "
            f"{errors!r} —— 「每轮提醒必报 ERROR『kidpal_speak 播报注入失败: object "
            "NoneType can't be used in 'await' expression』」的 defect of record 未消 "
            "(TECH-261014-kidpal-speak-await-nonetype; 修法 = await speak_txt 改同步调用)。",
        )

    # ------------------------------------------------------------------ #
    # VR-A6-b · 认领路径不回归（kidpal_speak 认领 + 空文案认领两形态）
    # ------------------------------------------------------------------ #

    def test_vr_a6_claim_paths_non_regression(self):
        """coverage: VR-A6 — 认领路径不回归：kidpal_speak 空文案认领（True/不播报/记 info）
        + 非认领形态（非 kidpal_speak / JSON 非 dict / 文本非 str / 畸形 JSON）返回 False。"""
        fn, err = self._method()
        if fn is None:
            self.fail("VR-A6 RED: " + (err or "connection.py method not loadable"))

        # (1) 空文案认领: {"type":"kidpal_speak","text":"   "} -> True、不调 speak_txt、记 info。
        call_log = []
        with _speak_txt_stub(lambda _conn, _text: None, call_log):
            conn, returned, raised = _invoke(
                fn,
                json.dumps({"type": KIDPAL_SPEAK_TYPE, "text": "   "}, ensure_ascii=False),
            )
        self.assertIsNone(raised, f"VR-A6 RED: 空文案认领抛异常 {raised!r}。")
        self.assertIs(
            returned, True,
            f"VR-A6 RED: 空文案认领返回 {returned!r}（期望 True）—— 空文案 = 已认领但不播报"
            "（提醒降级形态：弹窗 + 提示音仍必达）。",
        )
        self.assertEqual(
            call_log, [],
            f"VR-A6 RED: 空文案仍调 speak_txt {call_log!r} —— 空文案不得进播报链。",
        )
        self.assertTrue(
            any("无文案" in m for m in conn.logger.at_level("info")),
            f"VR-A6 RED: 空文案认领未记 info「无文案」—— 降级可观测面缺失"
            f"（info={conn.logger.at_level('info')!r}）。",
        )
        self.assertEqual(
            conn.logger.at_level("error"), [],
            "VR-A6 RED: 空文案认领记了 ERROR —— 降级路径不得报错。",
        )

        # (2) 非 kidpal_speak 类型（如 stt）-> False（绝不吞掉孩子说的话）。
        with _speak_txt_stub(lambda _conn, _text: None, []):
            _conn, returned, raised = _invoke(
                fn, json.dumps({"type": "stt", "text": "你好"},
                                        ensure_ascii=False))
        self.assertIsNone(raised, f"VR-A6 RED: 非认领类型抛异常 {raised!r}。")
        self.assertIs(
            returned, False,
            f"VR-A6 RED: 非 kidpal_speak 类型返回 {returned!r}（期望 False）—— 误认领会把"
            "普通对话文本吞掉不进 handleTextMessage。",
        )

        # (3) JSON 非 dict（数组/数字）-> False。
        for bad in ("[]", "[1, 2]", "123"):
            with _speak_txt_stub(lambda _conn, _text: None, []):
                _conn, returned, raised = _invoke(fn, bad)
            self.assertIsNone(raised, f"VR-A6 RED: JSON {bad!r} 抛异常 {raised!r}。")
            self.assertIs(
                returned, False,
                f"VR-A6 RED: JSON {bad!r} 返回 {returned!r}（期望 False）—— 非 dict 载荷须拒收。",
            )

        # (4) text 非 str（数字）-> 认领但不播报（True，不调 speak_txt）。
        call_log = []
        with _speak_txt_stub(lambda _conn, _text: None, call_log):
            _conn, returned, raised = _invoke(
                fn,
                json.dumps({"type": KIDPAL_SPEAK_TYPE, "text": 123}),
            )
        self.assertIsNone(raised, f"VR-A6 RED: 非 str 文案抛异常 {raised!r}。")
        self.assertIs(
            returned, True,
            f"VR-A6 RED: 非 str 文案返回 {returned!r}（期望 True）—— 已认领（type 命中）但"
            "文案不可播报，仍不得回流。",
        )
        self.assertEqual(call_log, [], f"VR-A6 RED: 非 str 文案仍调 speak_txt {call_log!r}。")

        # (5) 畸形 JSON -> False（解析失败不认领、不抛）。
        with _speak_txt_stub(lambda _conn, _text: None, []):
            _conn, returned, raised = _invoke(fn, "{ this is not json")
        self.assertIsNone(raised, f"VR-A6 RED: 畸形 JSON 抛异常 {raised!r}。")
        self.assertIs(
            returned, False,
            f"VR-A6 RED: 畸形 JSON 返回 {returned!r}（期望 False）—— 解析失败须静默拒收。",
        )

    # ------------------------------------------------------------------ #
    # VR-A6-c · V20 降级守卫: speak_txt 抛异常仍返回 True
    # ------------------------------------------------------------------ #

    def test_vr_a6_speak_failure_degrades_reminder_still_delivered(self):
        """coverage: VR-A6 — speak_txt 抛异常 -> _kidpal_try_speak 仍返回 True 且记 ERROR
        （提醒降级：弹窗+提示音仍必达，语音为可选增强——V20 外层 try/except 语义不放松）。"""
        fn, err = self._method()
        if fn is None:
            self.fail("VR-A6 RED: " + (err or "connection.py method not loadable"))

        def _boom(_conn, _text):
            raise RuntimeError("tts provider down")

        call_log = []
        with _speak_txt_stub(_boom, call_log):
            conn, returned, raised = _invoke(
                fn,
                json.dumps({"type": KIDPAL_SPEAK_TYPE, "text": _REMINDER_TEXT_RAW},
                           ensure_ascii=False),
            )
        self.assertIsNone(
            raised,
            f"VR-A6 RED: speak_txt 抛异常穿透了 _kidpal_try_speak（{raised!r}）—— 外层 "
            "try/except 降级语义被移除（提醒投递不得被语音拖住，V20）。",
        )
        self.assertIs(
            returned, True,
            f"VR-A6 RED: 出音失败时 _kidpal_try_speak 返回 {returned!r}（期望 True）—— "
            "提醒降级形态（弹窗+提示音必达）被破坏。",
        )
        self.assertEqual(
            len(call_log), 1,
            f"VR-A6 RED: speak_txt 被调用 {len(call_log)} 次（期望恰 1）。",
        )
        self.assertTrue(
            conn.logger.at_level("error"),
            "VR-A6 RED: 出音失败未记 ERROR —— 出音失败只记日志不抛（降级可观测面）语义缺失。",
        )


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
