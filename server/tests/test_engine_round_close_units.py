"""KidPal 261019-p3-record-dialogue-guide fixer R4 (D4-3) 服务端自带测 —
engine-patch connection.py 丢弃轮收尾（round-close）guard。

背景（真机 R3 实害）：chat depth==0 幻听过滤分支静默丢弃幻听轮，但 ASR 路径的 stt
回显帧先于 chat 已下发、设备已进思考态等回复——静默丢弃 = 设备悬思考态直到连接
超时（R3 目击挂 20+ 分钟）。修法 = 丢弃时走引擎既有回合收尾机制（speak_txt 完整
TTS 轮）关轮；记录语境不出音（抑制链冻结语义）。

与冻结验收分工：冻结件（server/acceptance/test_engine_stop_truncation_asr_filter.
acceptance.py ET-e/f、test_record_dialogue_guide.acceptance.py RDG-A4）是规格、
一字不改；本文件是 D4-3 增量面的自带测（round-close 挂点、记录语境豁免、
client_abort 红线、speak_txt 注入行为与降级）。

纯 stdlib + AST 按路径加载（connection.py 依赖引擎运行时、不可直接 import）：
  cd server && python -m pytest -q tests/test_engine_round_close_units.py
"""

from __future__ import annotations

import ast
import json
import sys
import textwrap
import types
import unittest
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
CONNECTION = SERVER / "deploy" / "engine-patch" / "connection.py"

HALLUC_FN = "_kidpal_is_asr_hallucination"
ROUND_CLOSE_METHOD = "_kidpal_close_dropped_round"
ROUND_CLOSE_LINE_CONST = "_KIDPAL_ROUND_CLOSE_LINE"


def _load_connection():
    if not CONNECTION.is_file():
        return None, None, f"engine-patch connection.py 不存在: {CONNECTION}"
    src = CONNECTION.read_text(encoding="utf-8", errors="replace")
    try:
        tree = ast.parse(src)
    except SyntaxError as exc:
        return src, None, f"connection.py 解析失败: {exc}"
    return src, tree, None


def _find_func(tree, name):
    for node in ast.walk(tree):
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)) \
                and node.name == name:
            return node
    return None


def _load_round_close_fn():
    """AST 抽取 _kidpal_close_dropped_round 方法体在受限命名空间 exec，返回
    (fn, captured, err)——captured 是桩 speak_txt 收到的调用记录。桩模块安装进
    sys.modules 并保持到 restore 回调执行（helper 的 import 发生在调用时）。"""
    src, tree, err = _load_connection()
    if err:
        return None, None, err
    node = _find_func(tree, ROUND_CLOSE_METHOD)
    if node is None:
        return None, None, f"connection.py 无方法 {ROUND_CLOSE_METHOD}"
    seg = ast.get_source_segment(src, node)
    if seg is None:
        return None, None, "round-close 方法源切片失败"

    captured = []

    stub_module = types.ModuleType("core.handle.intentHandler")
    stub_module.speak_txt = lambda conn, text: captured.append((conn, text))
    pkg = types.ModuleType("core.handle")
    pkg.intentHandler = stub_module
    core_pkg = types.ModuleType("core")
    core_pkg.handle = pkg

    _ROUND_CLOSE_STUBS.clear()
    _ROUND_CLOSE_STUBS.update({
        "core": core_pkg, "core.handle": pkg,
        "core.handle.intentHandler": stub_module,
    })

    ns = {
        "json": json, "sys": sys, "TAG": "round_close_ut",
        ROUND_CLOSE_LINE_CONST: _extract_const(src),
    }
    exec(compile(textwrap.dedent(seg), str(CONNECTION), "exec"), ns)
    fn = ns.get(ROUND_CLOSE_METHOD)
    if fn is None:
        return None, captured, "exec 后未取到 round-close 方法"
    return fn, captured, None


_ROUND_CLOSE_STUBS = {}


def _install_round_close_stubs():
    """把 core.handle.intentHandler 桩装进 sys.modules（调用期窗口），返回恢复函数。"""
    saved = {k: sys.modules.get(k) for k in _ROUND_CLOSE_STUBS}
    sys.modules.update(_ROUND_CLOSE_STUBS)

    def _restore():
        for k, v in saved.items():
            if v is None:
                sys.modules.pop(k, None)
            else:
                sys.modules[k] = v
    return _restore


_CONST_RE = None


def _extract_const(src):
    """从模块源码取 _KIDPAL_ROUND_CLOSE_LINE 字面值（避免 import 整个模块）。"""
    import re as _re
    global _CONST_RE
    if _CONST_RE is None:
        _CONST_RE = _re.compile(
            rf"^{ROUND_CLOSE_LINE_CONST}\s*=\s*[ru]?[\"'](.+?)[\"']", _re.M)
    m = _CONST_RE.search(src)
    return m.group(1) if m else ""


class _Log:
    def bind(self, **_kw):
        return self

    def info(self, message):
        pass

    def error(self, message):
        pass


class _Conn:
    def __init__(self):
        self.logger = _Log()
        self.client_abort = False
        self._kidpal_record_context = False
        self.touched = []


class EngineRoundCloseUnits(unittest.TestCase):
    """D4-3 自带测：丢弃轮收尾的挂点、豁免与红线。"""

    maxDiff = None

    def test_round_close_helper_exists_and_nonempty_line(self):
        """守卫件在世：helper 方法 + 固定收尾文案非空。"""
        src, tree, err = _load_connection()
        self.assertIsNone(err, f"UT RED(infra): {err}")
        self.assertIsNotNone(
            _find_func(tree, ROUND_CLOSE_METHOD),
            f"UT RED: connection.py 无 {ROUND_CLOSE_METHOD} —— D4-3 丢弃轮收尾"
            "补丁被移除（真机 R3 思考态悬挂回归）。")
        line = _extract_const(src)
        self.assertTrue(
            line and line.strip(),
            f"UT RED: {ROUND_CLOSE_LINE_CONST} 空——丢弃轮收尾无可播文案。")

    def test_hallucination_guard_closes_round_and_skips_record_context(self):
        """挂点 + 豁免：幻听过滤守卫体内调 _kidpal_close_dropped_round，且调用被
        「not self._kidpal_record_context」条件包裹（记录语境抑制链不出音）。"""
        src, tree, err = _load_connection()
        self.assertIsNone(err, f"UT RED(infra): {err}")
        chat = _find_func(tree, "chat")
        self.assertIsNotNone(chat, "UT RED(infra): connection.py 无 chat 方法。")
        depth0 = None
        for n in ast.walk(chat):
            if (isinstance(n, ast.If) and isinstance(n.test, ast.Compare)
                    and isinstance(n.test.left, ast.Name)
                    and n.test.left.id == "depth"
                    and any(isinstance(o, ast.Eq) for o in n.test.ops)
                    and any(isinstance(c, ast.Constant) and c.value == 0
                            for c in n.test.comparators)):
                if depth0 is None or n.lineno < depth0.lineno:
                    depth0 = n
        self.assertIsNotNone(depth0, "UT RED(infra): chat 无 depth==0 分支。")

        guard = None
        for n in ast.walk(depth0):
            if (isinstance(n, ast.If) and n is not depth0
                    and any(isinstance(x, ast.Call)
                            and isinstance(x.func, ast.Name)
                            and x.func.id == HALLUC_FN for x in ast.walk(n.test))
                    and any(isinstance(x, ast.Return) for x in ast.walk(n))):
                guard = n
                break
        self.assertIsNotNone(
            guard, "UT RED(infra): chat depth==0 无幻听过滤守卫。")

        def calls_in(node, name):
            return [x for x in ast.walk(node)
                    if isinstance(x, ast.Call)
                    and ((isinstance(x.func, ast.Name) and x.func.id == name)
                         or (isinstance(x.func, ast.Attribute)
                             and x.func.attr == name))]

        closes = calls_in(guard, ROUND_CLOSE_METHOD)
        self.assertTrue(
            closes,
            "UT RED: 幻听过滤守卫不再调 _kidpal_close_dropped_round —— 丢弃不关轮，"
            "设备思考态悬挂回归（D4-3）。")
        # 豁免：调用必须落在 not _kidpal_record_context 条件之内（守卫体内某 If 的
        # test 引用 _kidpal_record_context，且该 If 内含 close 调用）。
        cond_blocks = [x for x in ast.walk(guard)
                       if isinstance(x, ast.If) and x is not guard
                       and any((isinstance(t, ast.Name)
                                and t.id == "_kidpal_record_context")
                               or (isinstance(t, ast.Attribute)
                                   and t.attr == "_kidpal_record_context")
                               for t in ast.walk(x.test))]
        self.assertTrue(
            any(calls_in(b, ROUND_CLOSE_METHOD) for b in cond_blocks),
            "UT RED: round-close 调用未受记录语境豁免包裹 —— 记录语境幻听轮出音"
            "将打断抑制链（冻结语义回退）。")

    def test_no_client_abort_anywhere_in_round_close_paths(self):
        """红线：round-close helper 与幻听守卫体内均不得赋值 client_abort。"""
        src, tree, err = _load_connection()
        self.assertIsNone(err, f"UT RED(infra): {err}")

        def abort_assigns(node):
            return [n.lineno for n in ast.walk(node)
                    if isinstance(n, (ast.Assign, ast.AugAssign))
                    and any(isinstance(t, ast.Attribute)
                            and t.attr == "client_abort" for t in ast.walk(n))]

        helper = _find_func(tree, ROUND_CLOSE_METHOD)
        self.assertIsNotNone(helper, "UT RED(infra): 无 round-close helper。")
        self.assertEqual(
            abort_assigns(helper), [],
            "UT RED: round-close helper 内赋值 client_abort —— 波及 stop 后 front "
            "注入的合法 speak/story 下发（ET/RDG 同款纪律）。")

    def test_round_close_behavior_calls_speak_txt_and_swallows_errors(self):
        """行为：helper 经 speak_txt（引擎既有 TTS 轮收尾原语）出音；speak_txt 抛错
        只记日志不外抛（降级同 _kidpal_try_speak）；client_abort 不被触碰。"""
        fn, captured, err = _load_round_close_fn()
        self.assertIsNone(err, f"UT RED(infra): {err}")

        restore = _install_round_close_stubs()
        try:
            conn = _Conn()
            fn(conn, "asr-hallucination")
        finally:
            restore()
        self.assertEqual(len(captured), 1,
                         "UT RED: round-close 未出音（speak_txt 未被调）。")
        spoke_conn, spoke_text = captured[0]
        self.assertIs(spoke_conn, conn, "UT RED: speak_txt 未收到本连接。")
        self.assertEqual(spoke_text, _extract_const(
            CONNECTION.read_text(encoding="utf-8", errors="replace")),
            "UT RED: 收尾播报文案与冻结常量不一致。")
        self.assertFalse(conn.client_abort, "UT RED: client_abort 被置位。")

        # 降级：speak_txt 抛错不外抛（丢弃语义不受通知失败拖垮）。
        def boom(conn, text):
            raise RuntimeError("tts down")

        stub = _ROUND_CLOSE_STUBS["core.handle.intentHandler"]
        saved_speak = stub.speak_txt
        stub.speak_txt = boom
        restore = _install_round_close_stubs()
        try:
            fn(_Conn(), "asr-hallucination")  # 不应 raise
        finally:
            restore()
            stub.speak_txt = saved_speak


if __name__ == "__main__":
    unittest.main()
