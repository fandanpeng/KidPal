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


# --------------------------------------------------------------------------- #
# 261116-channel-resident-core 真机轮 C 修复轮（SCRF-C1..C4）追加：
#   产物侧占位守卫（_organize_body_reject_reason R1-R4）+ 段级静音 ASR 幻觉集合。
#   纯 additive：上方 _load_guard / ArchiveMaterialGuardUnits 逐字不动。
# --------------------------------------------------------------------------- #


def _load_output_guard():
    """AST 抽取轮 C 输出守卫判定核（常量 + 纯函数）在受限命名空间 exec。

    与 `_load_guard()` 同一技法、独立抽取器（互不影响）。wanted 比派发清单多列
    `_FRONT_GUARD_STRIP_RE`：`_guard_compact` 的函数体直接引用该模块级正则，不抽取
    会在 exec 期 NameError（假红）；它只是既有常量，不动 `_load_guard` 的白名单。"""
    import ast
    import textwrap
    src = (SERVER / "gateway" / "front" / "front_app.py").read_text(
        encoding="utf-8")
    tree = ast.parse(src)
    wanted = {
        "_FRONT_GUARD_STRIP_RE",
        "_FRONT_SILENT_ASR_HALLUCINATIONS",
        "_FRONT_ORGANIZE_PLACEHOLDERS",
        "_ORGANIZE_BODY_MIN_CHARS",
        "_ORGANIZE_BRACKETED_MAX_CHARS",
        "_guard_compact",
        "_is_silent_asr_hallucination",
        "_organize_body_reject_reason",
    }
    parts = [n for n in tree.body
             if ((isinstance(n, ast.Assign) and any(
                 isinstance(t, ast.Name) and t.id in wanted for t in n.targets))
                 or (isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef))
                     and n.name in wanted))]
    got = {getattr(n, "name", None) or getattr(
        (n.targets if isinstance(n, ast.Assign) else [None])[0], "id", None)
        for n in parts}
    missing = wanted - got
    assert not missing, f"output guard 判定核抽取失败: {missing}"
    ns = {"re": __import__("re")}
    exec(compile(textwrap.dedent("\n\n".join(
        ast.get_source_segment(src, n) for n in parts)),
        "front_app_output_guard_extract", "exec"), ns)
    return ns


_OUTPUT_GUARD_NS = _load_output_guard()
_organize_guard = _OUTPUT_GUARD_NS["_organize_body_reject_reason"]
_is_silent_hit = _OUTPUT_GUARD_NS["_is_silent_asr_hallucination"]
_output_compact = _OUTPUT_GUARD_NS["_guard_compact"]
_SILENT_TABLE = _OUTPUT_GUARD_NS["_FRONT_SILENT_ASR_HALLUCINATIONS"]
_PLACEHOLDER_TABLE = _OUTPUT_GUARD_NS["_FRONT_ORGANIZE_PLACEHOLDERS"]


class OrganizeOutputGuardUnits(unittest.TestCase):
    """SCRF-C2：整理产物占位守卫 R1-R4 的机械判别力（拒占位、放真实短故事）。"""

    maxDiff = None

    def test_r1_r2_r3_r4_rejects_with_reason(self):
        """冻结验收 C2 的 8 条拒绝样本逐条对齐（细分类 reason 可归因）。"""
        cases = (
            ("。", "empty_body"),                          # R1
            ("", "empty_body"),                            # R1
            ("嗯", "insignificant_body"),                  # R2
            ("没有可整理的内容", "placeholder_body"),        # R3（本轮真机实证）
            ("无内容", "placeholder_body"),                  # R3
            ("Nothing to organize.", "placeholder_body"),   # R3
            ("【无】", "placeholder_body"),                  # R4（方尖括号）
            ("(no content)", "placeholder_body"),           # R4（半角括号）
            ("（本轮素材不足）", "placeholder_body"),          # R4（全角括号）
        )
        for body, reason in cases:
            with self.subTest(body=body):
                self.assertEqual(
                    _organize_guard(body), reason,
                    f"UT RED: 整理产物 {body!r} 拒绝原因 != {reason!r}——R1-R4 机械"
                    "判据分档破坏（垃圾篇回归面）。")

    def test_r4_two_layer_shell_and_length_upper_bound(self):
        """R4：至多剥 2 层、剥壳后 <=16 字拒；超上限按真实正文放行。"""
        self.assertEqual(_organize_guard("【（这里没东西）】"), "placeholder_body",
                         "UT RED: 双层括号壳未剥到内层（R4 至多 2 层）。")
        inner16 = "一" * 16
        self.assertEqual(_organize_guard(f"（{inner16}）"), "placeholder_body",
                         "UT RED: 剥壳后 16 字边界未被拒（<= 上限语义）。")
        inner17 = "一" * 17
        self.assertIsNone(_organize_guard(f"（{inner17}）"),
                          "UT RED: 剥壳后 17 字被误拒——上限只罩占位措辞，长正文"
                          "必须放行。")
        self.assertEqual(_organize_guard("（ ）"), "empty_body",
                         "UT RED: 空括号壳未归 empty_body（R1 优先）。")

    def test_real_bodies_pass(self):
        """放行面：真实短故事、引号族引用原话、title==body（否决项负锚）。"""
        for body in ("冰淇淋\n想吃冰淇淋。",
                     "糖葫芦\n正文一句话。",     # FP-A5 body compact=5 上界证据
                     "他说了「你好」",           # 引号族不是 R4 括号族
                     "「你好」",
                     "一段故事\n一段故事",       # 否决「title == body -> 占位」
                     "小猫",
                     "让我们一起说thank you吧"):
            with self.subTest(body=body):
                self.assertIsNone(
                    _organize_guard(body),
                    f"UT RED: 真实/合法正文 {body!r} 被占位守卫误杀——R4 必须排除"
                    "引号族、R2/R3 只做整串判定。")

    def test_silent_asr_hit_family_and_misses(self):
        """段级静音幻觉：命中族 drop、素材守卫覆盖不到的串才入表。"""
        for text in ("Okay.", "okay", "OKAY！", "ok", "Thank you.",
                     "うん。", "そう。", "はい。", "いいえ。",
                     "谢谢观看", "请不吝点赞", "嗯嗯", "哦哦"):
            with self.subTest(text=text):
                self.assertTrue(
                    _is_silent_hit(text),
                    f"UT RED: 静音幻觉 {text!r} 未被段级集合命中。")
        for text in ("嗯。", "好的。", "嗯，然后我们去公园", "Okay，我知道了",
                     "谢谢你，这个字怎么写", "让我们一起说thank you吧",
                     "", "   ", "。"):
            with self.subTest(text=text):
                self.assertFalse(
                    _is_silent_hit(text),
                    f"UT RED: {text!r} 被段级过滤误杀——中文应答词/单字/混排真话"
                    "不得入表（SCRF-B10 case 2 与 human「不误伤真话」）。")

    def test_silent_set_invariant(self):
        """集合不变式：非空 set/frozenset、元素为 compact 形态且长度 >=2。

        单字（「嗯」->「嗯」）入表会让 ingest 吞掉「嗯。」收尾沿，SCRF-B10 case 2
        翻红；空格形态（「thank you」）入表则与 compact 判定不同源、恒不命中。"""
        self.assertIsInstance(_SILENT_TABLE, (set, frozenset),
                              "UT RED: 段级集合类型不是 set/frozenset。")
        self.assertTrue(_SILENT_TABLE, "UT RED: 段级集合为空——过滤面恒不命中。")
        for element in _SILENT_TABLE:
            with self.subTest(element=element):
                self.assertIsInstance(element, str,
                                      f"UT RED: 段级集合元素非 str：{element!r}。")
                self.assertGreaterEqual(
                    len(_output_compact(element)), 2,
                    f"UT RED: 元素 {element!r} compact 后长度 < 2——会被 ingest "
                    "吞掉并红 SCRF-B10 case 2。")
                self.assertEqual(
                    _output_compact(element), element,
                    f"UT RED: 元素 {element!r} 非 compact 形态（判定面恒不命中）。")

    def test_placeholder_table_is_compact_and_disjoint(self):
        """占位表不变式：compact 形态（R3 直接成员判定）+ 与段级表不重叠。"""
        self.assertIsInstance(_PLACEHOLDER_TABLE, (set, frozenset),
                              "UT RED: 占位集合类型不是 set/frozenset。")
        self.assertTrue(_PLACEHOLDER_TABLE, "UT RED: 占位集合为空。")
        for element in _PLACEHOLDER_TABLE:
            with self.subTest(element=element):
                self.assertEqual(
                    _output_compact(element), element,
                    f"UT RED: 占位元素 {element!r} 非 compact 形态——R3 成员判定"
                    "恒不命中（如 n/a 必须存为 na）。")
        self.assertFalse(
            _PLACEHOLDER_TABLE & _SILENT_TABLE,
            "UT RED: 段级表与整理占位表重叠——两层守卫的证据档位混同。")


if __name__ == "__main__":
    unittest.main()
