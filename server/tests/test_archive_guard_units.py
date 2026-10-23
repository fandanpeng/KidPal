"""KidPal 261019-p3-record-dialogue-guide fixer R4 (D4-2) 服务端自带测 —
front 归档边界内容守卫（_archive_material_reject_reason）。

背景（真机 R3 实害）：空内容 + 幻听「Thank you.」被整理归档成 `2026-10-20-no-reply`
垃圾篇（seq 占用、SD 落卡 bytes=8）。守卫 = _story_finish_session 归档边界
（strip_trailing_finish_words 后、organize LLM 前）：纯幻听 / 近无内容（单语气词、
纯标点）不归档不演出不安抚仅释放；混排内容（真话+幻听串）照常放行——RDG-A3 冻结
「装配层不代行滤除」不被触碰（守卫不在 build_organizing_input）。

与冻结验收分工：RDG-A3/M4K1 冻结件是规格、一字不改；本文件是守卫判定核的自带测。

纯 stdlib + 按路径加载（front_app 依赖 fastapi，可 import）：
  cd server && python -m pytest -q tests/test_archive_guard_units.py
"""

from __future__ import annotations

import sys
import unittest
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))


def _load_guard():
    """AST 抽取归档守卫判定核（函数 + 两个模块级常量）在受限命名空间 exec——
    front_app.py 依赖 fastapi、本地不可整模块 import（引擎侧 round-close 测试同技法）。"""
    import ast
    import textwrap
    src = (SERVER / "gateway" / "front" / "front_app.py").read_text(
        encoding="utf-8")
    tree = ast.parse(src)
    wanted = {"_FRONT_ARCHIVE_HALLUCINATIONS", "_FRONT_GUARD_STRIP_RE",
              "_archive_material_reject_reason"}
    parts = [n for n in tree.body
             if ((isinstance(n, ast.Assign) and any(
                 isinstance(t, ast.Name) and t.id in wanted for t in n.targets))
                 or (isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef))
                     and n.name in wanted))]
    got = {getattr(n, "name", None) or getattr(
        (n.targets if isinstance(n, ast.Assign) else [None])[0], "id", None)
        for n in parts}
    missing = wanted - got
    assert not missing, f"guard 判定核抽取失败: {missing}"
    ns = {"re": __import__("re")}
    exec(compile(textwrap.dedent("\n\n".join(
        ast.get_source_segment(src, n) for n in parts)),
        "front_app_guard_extract", "exec"), ns)
    return ns["_archive_material_reject_reason"], ns


guard, _ns = _load_guard()


class ArchiveMaterialGuardUnits(unittest.TestCase):
    """D4-2 归档边界内容守卫判定核：拒幻听/近无内容、放混排与真实内容。"""

    maxDiff = None

    def test_pure_hallucination_rejected(self):
        """纯幻听串（含变体：大小写/标点）整串命中即拒——no-reply 实害本体。"""
        for text in ("Thank you.", "thank you", "THANK YOU!",
                     "谢谢观看。", "请不吝点赞", "哈哈", "哈哈哈", "嗯嗯"):
            with self.subTest(text=text):
                self.assertEqual(
                    guard(text), "pure_hallucination_material",
                    f"UT RED: 纯幻听 {text!r} 未拒——垃圾篇归档回归（D4-2）。")

    def test_insignificant_material_rejected(self):
        """近无内容：单语气词 / 纯标点 / 空白 → insignificant_material。"""
        for text in ("嗯", "啊。", "呃……", "。。。", "  ？？  ", ""):
            with self.subTest(text=text):
                self.assertEqual(
                    guard(text), "insignificant_material",
                    f"UT RED: 近无内容 {text!r} 未拒——垃圾篇归档回归（D4-2）。")

    def test_real_and_mixed_material_pass(self):
        """放行面：真实内容、混排（真话+幻听串，RDG-A3 口径）、两字以上残句。"""
        for text in ("我去了游乐场，然后吃了冰淇淋",
                     "从前有一只小猫\nThank you.\n然后它回家了",
                     "谢谢你，这个字怎么写",   # 整串保护负锚（ET-e 同款）：绝不误杀
                     "让我们一起说thank you吧",
                     "小猫"):
            with self.subTest(text=text):
                self.assertIsNone(
                    guard(text),
                    f"UT RED: 合法/混排内容 {text!r} 被误拒——真实故事归档回归。")

    def test_guard_sits_after_empty_check_before_downlink(self):
        """结构锚：守卫在 _story_finish_session 内位于 empty_material 释放之后、
        _issue_finish_downlink（演出下发）之前——拒归档即不演出。"""
        import ast
        src = (SERVER / "gateway" / "front" / "front_app.py").read_text(
            encoding="utf-8")
        tree = ast.parse(src)
        fn = next(n for n in ast.walk(tree)
                  if isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef))
                  and n.name == "_story_finish_session")
        seg = ast.get_source_segment(src, fn)
        assert seg is not None
        empty_ln = seg.index("reason=empty_material")
        guard_ln = seg.index("_archive_material_reject_reason(material)")
        downlink_ln = seg.index("_issue_finish_downlink(session_id")
        self.assertLess(
            empty_ln, guard_ln,
            "UT RED(infra): 守卫不在空素材释放之后——判定序漂移。")
        self.assertLess(
            guard_ln, downlink_ln,
            "UT RED: 内容守卫晚于演出下发——拒归档仍下发 start_organizing，"
            "整理过渡页对垃圾轮可见（D4-2 守卫语义破坏）。")


if __name__ == "__main__":
    unittest.main()
