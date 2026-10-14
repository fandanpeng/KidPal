"""Frozen acceptance tests — TaskPackage 260920-m2-charwindow-display-fix
(subsystem: firmware+server; workstream m2-char-window; mergeOrder 32; rigor lite;
 riskTier medium; **跨两端**).

云侧 MCD-A5（网关双出口 merge：LLM 最小块 `meaning` 覆盖 cache 值 / 缺键回落 /
替换后 JSON 合法 / 无最小块泄漏；**流式与非流式两条出口路径一致**）+ MCD-A7
（播报不夹带非口语内容：`<details>…</details>` 附录在两条出口路径上均被剥离、
正常口语正文不被误剥）+ prompt 锚齿（char 段 meaning 新约 + 严禁自生成 strokes/
medians 约束 + ≤3 句 + 只含口语正文与一个 display 块、禁自检/来源/附录/明细/HTML）。

Device-face twin: firmware/main/test/char_window_overlay.acceptance.c test 10/11
（MCD-A1/A2/A3/A4 板端展示面让位 + 空行不显示 + 组词多词 join）。

Nature: OFFLINE-DETERMINISTIC（python 镜像 `char_stroke_lib.py` merge 语义）
 + REAL-EXECUTION（node 真跑 `make_openclaw_patch._helper_ts()` 的**两条 TS 出口
   路径**：`k1cRewriteText`（非流式）/ `k1cLookupBlock`·`k1cStreamStep`（流式）——
   跨端 wire 形状断言须对照**对端实际接受什么**，不得按本侧契约想象；
   260907-g-relay-fix 同族教训：跨端不申报 = 离线全绿而真机被对端一句拒收）
 + STRUCTURAL（TS helper 两函数体 merge/strip 在场；prompt 版本无关锚）。

RED baseline（现状树，acceptance-author 2026-09-20 实跑）：本件 A5/A7 行为齿 +
prompt 锚齿**红**（现行 `k1cRewriteText`/`k1cLookupBlock` 无 meaning merge、无
`<details>` 剥离 = 真实缺陷信号；prompt v4 char 段无 meaning 新约、无附录禁令）；
prompt 接线两面守卫 + A7 正向 case 现状**绿**（must-stay-green）。

Collection discipline（G 包先例）：文件名两点须 importlib 模式
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance
"""

from __future__ import annotations

import importlib.util
import json
import os
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
PATCH_DIR = DEPLOY_DIR / "openclaw-patch"

MAKE_PATCH_PY = PATCH_DIR / "make_openclaw_patch.py"
CHAR_LIB_PY = PATCH_DIR / "char_stroke_lib.py"
PROMPT_RENDER_SH = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
PROMPT_DECL_YAML = DEPLOY_DIR / "provider.config.yaml"

# 冻结 wire 形状：cache 值 = D3 data 层（九键；不含 view/data 信封）。
CACHE_CHAR = "勇"
CACHE_MEANING = "brave; courageous"        # makemeahanzi definition（英文）——旧 cache 值
MINIMAL_MEANING = "勇敢；有胆量"             # LLM 最小块携带的中文释义（同回复播报同源）
CACHE_ROWS = {
    CACHE_CHAR: {
        "char": CACHE_CHAR,
        "pinyin": "yǒng",
        "radical": "力",
        "strokeCount": 9,
        "strokes": ["M 100 100 L 900 100", "M 200 400 Q 500 900 800 400"],
        "medians": [[[100, 200], [900, 200]], [[200, 500], [500, 700], [800, 500]]],
        "structure": "⿱甬力",
        "meaning": CACHE_MEANING,
        "words": ["勇敢", "勇气"],
    }
}

# 附录样本（human finding③ 实测逐字形状：来源与自检 + 数据入口 + 视觉参考注 + svg 字段注）。
APPENDIX = (
    "<details><summary>来源与自检</summary>\n"
    "数据入口：char_cache.json 命中勇；笔画名称来源：bishun.hao86.com、bishun.fjnews.com。\n"
    "另注：勇_stroke_order.svg 只是视觉参考，C1 契约里没有 svg 字段。\n"
    "</details>"
)
APPENDIX_TOKENS = ("<details", "</details", "来源与自检", "数据入口", "stroke_order.svg")

# 正向 case 正文（普通括号 / 中文标点；剥离规则若过宽会误伤——A7 正向钉此）。
NORMAL_SPEECH = (
    "今天天气真好，我们来认识「勇」字（就是勇敢的那个勇）。"
    "它一共有九笔（横、竖、撇、捺），跟着我一笔一画写写看吧！")


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _load_ts_helper() -> str:
    """渲染出 make_openclaw_patch._helper_ts() 的 TS 源码（marker 替换后）。

    make_openclaw_patch 只 import sys（纯 python），可直接加载。返回的 TS 是**部署
    实际注入 dist chunk 的那一段**（网关出口 helper 的唯一生成来源）。
    """
    spec = importlib.util.spec_from_file_location("kidpal_mcd_mop", MAKE_PATCH_PY)
    if spec is None or spec.loader is None:
        raise AssertionError(f"make_openclaw_patch.py not loadable: {MAKE_PATCH_PY}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod._helper_ts().replace("$MARKER", getattr(mod, "MARKER", ""))


def _load_char_lib():
    spec = importlib.util.spec_from_file_location("kidpal_mcd_char_lib", CHAR_LIB_PY)
    if spec is None or spec.loader is None:
        raise AssertionError(f"char_stroke_lib.py not loadable: {CHAR_LIB_PY}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def _ts_func_region(src: str, name: str):
    """TS helper 内 `function <name>(` 起始到下一个顶层 `function ` 之间的区段（含注释）。"""
    i = src.find("function " + name + "(")
    if i < 0:
        return None
    j = src.find("\nfunction ", i + 1)
    return src[i:] if j < 0 else src[i:j]


def _display_block(text: str):
    m = re.search(r"```display\s*(\{.*?\})\s*```", text or "", re.DOTALL)
    if not m:
        return None
    try:
        return json.loads(m.group(1))
    except (ValueError, TypeError):
        return None


def _block_data(blk):
    """解析出的 display 块的 data 层——兼容 view/data 信封（TS 出口）与裸 data 层
    （python 镜像既有冻结契约 `replace_minimal_block` 逐字 emit full_data）。"""
    if not isinstance(blk, dict):
        return {}
    inner = blk.get("data")
    return inner if isinstance(inner, dict) else blk


class _TSRunner(unittest.TestCase):
    """Shared node execution harness for the two gateway exit paths.

    node 是仓库工具链既有依赖（`.claude/scripts/*.mjs`）；本件用真实 node 执行
    **部署实际注入的 TS helper**（不是 python 摹本），以钉「跨端对端形状」。
    node 缺席 = 执行通道缺失：行为齿显式 skip（不静默假绿），源头覆盖由同件
    STRUCTURAL 齿（两函数体 merge/strip 在场）承载。
    """

    def _run_ts(self, driver_body: str):
        node = shutil.which("node")
        if node is None:
            self.skipTest(
                "node 不可用 = 执行通道缺失；本件 TS 两条出口路径的行为齿需真实 node "
                "执行（结构性两函数体 merge/strip 齿仍独立覆盖同一需求）")
        ts = _load_ts_helper()
        d = tempfile.mkdtemp(prefix="mcd_ts_")
        try:
            cache_path = os.path.join(d, "cache.json")
            with open(cache_path, "w", encoding="utf-8") as f:
                json.dump(CACHE_ROWS, f, ensure_ascii=False)
            mjs = os.path.join(d, "driver.mjs")
            with open(mjs, "w", encoding="utf-8") as f:
                f.write(ts + "\n" + driver_body)
            env = dict(os.environ)
            env["CHAR_CACHE_JSON"] = cache_path
            proc = subprocess.run(
                [node, mjs], capture_output=True, encoding="utf-8", errors="replace",
                timeout=120, env=env)
            out = (proc.stdout or "").strip().splitlines()
            if proc.returncode != 0 or not out:
                self.fail(
                    "MCD RED(infra): TS helper 未产出结果（node rc=%s）。stdout=%r "
                    "stderr=%r" % (proc.returncode, proc.stdout, proc.stderr))
            try:
                return json.loads(out[-1])
            except (ValueError, IndexError):
                self.fail(f"MCD RED(infra): TS driver 输出非 JSON。raw={out[-1][:400]!r}")
        finally:
            shutil.rmtree(d, ignore_errors=True)


class CharMeaningMergeAcceptance(_TSRunner):
    """MCD-A5 / MCD-A7 云面 merge + 播报剥离齿。"""

    maxDiff = None

    # --- MCD-A5: python 镜像 merge 语义 -----------------------------------------
    def test_mcd_a5_python_mirror_meaning_merge_semantics(self):
        """coverage: MCD-A5 — python 镜像（`char_stroke_lib.py`）的
        `replace_minimal_block(text, full_data)` 语义：块内最小块携带 `meaning` ->
        覆盖 cache 值；缺键 -> 回落 cache 原值；替换后 JSON 合法；最小块成品不外泄
        （输出 = 拼装后全量 D3，非 LLM 复述形态）。TS 侧同语义由 node 行为齿钉
        （两路径一致），本齿钉「TS = python 冻结语义镜像」纪律。"""
        mod = _load_char_lib()
        replace = getattr(mod, "replace_minimal_block", None)
        self.assertIsNotNone(
            replace,
            "MCD-A5 RED: char_stroke_lib.replace_minimal_block 缺失——C1 冻结契约函数被拆")

        speech = "勇字这样写，一共九笔哦。"
        full = json.loads(json.dumps(CACHE_ROWS[CACHE_CHAR]))

        # (1) 块内携带 meaning -> 覆盖 cache 值（LLM 已播报的中文释义同源文本）。
        with_m = '{"view":"char","data":{"char":"%s","meaning":"%s"}}' % (
            CACHE_CHAR, MINIMAL_MEANING)
        reply = f"{speech}\n```display\n{with_m}\n```\n"
        out = replace(reply, full)
        blk = _display_block(out)
        self.assertIsNotNone(
            blk, "MCD-A5 RED: 替换后无合法 ```display 块（JSON 非法或块丢失）")
        self.assertEqual(
            _block_data(blk).get("meaning"), MINIMAL_MEANING,
            "MCD-A5 RED: 块内最小块 meaning 未覆盖 cache 值——播报的中文解释没写进 "
            "display 块既有的 meaning 字段（human 触点①决策 2：同一回合同源）。")
        self.assertEqual(
            set(_block_data(blk).keys()), set(CACHE_ROWS[CACHE_CHAR].keys()),
            "MCD-A5 RED: 输出块 data 键集不等于 cache 全量 D3——最小块成品泄漏或键缺失。")
        self.assertNotIn(
            with_m, out, "MCD-A5 RED: 最小块成品未替换（HOLD-AND-REPLACE 失守）。")
        self.assertTrue(
            out.split("```display")[0].strip() == speech,
            "MCD-A5 RED: 替换动了语音正文（语音部分必须原样保留）。")
        # strokes/medians 仍逐字来自静态字库（坐标绝不来自 LLM）。
        self.assertEqual(
            _block_data(blk).get("strokes"), CACHE_ROWS[CACHE_CHAR]["strokes"],
            "MCD-A5 RED: 坐标未逐字来自静态字库（LLM 复述坐标回归）。")

        # (2) 缺键 -> 回落 cache 原值（英文 meaning；prompt 钉「必带」+ 板端空行兜底）。
        no_m = '{"view":"char","data":{"char":"%s"}}' % CACHE_CHAR
        reply2 = f"{speech}\n```display\n{no_m}\n```\n"
        blk2 = _display_block(replace(reply2, full))
        self.assertIsNotNone(blk2, "MCD-A5 RED: 缺 meaning 时替换产不出合法块。")
        self.assertEqual(
            _block_data(blk2).get("meaning"), CACHE_MEANING,
            "MCD-A5 RED: 最小块缺 meaning 时未回落 cache 原值（缺键回落语义失守）。")

    # --- MCD-A5: char_stroke_lib docstring 冻结验收指针（现役件）-----------------
    def test_mcd_a5_char_lib_docstring_pointer_is_live(self):
        """coverage: MCD-A5（留痕）— `char_stroke_lib.py` 模块 docstring 的「冻结验收
        指针」必须指向**现役**验收件（server/acceptance/test_char_meaning_merge.
        acceptance.py），不得再指已归档的 `test_char_stroke.acceptance.py`（落盘 ≠
        孤儿；discovery 按契约标记扫装，指针 = 契约单源声明）。"""
        doc = _read_text(CHAR_LIB_PY)
        self.assertNotIn(
            "test_char_stroke.acceptance.py", doc,
            "MCD-A5 RED: char_stroke_lib.py docstring 仍指已归档的 "
            "test_char_stroke.acceptance.py——冻结验收指针未收编到现役件（交错引用会让"
            "契约单源声明指向不存在/退役的验收文件）。")
        self.assertIn(
            "test_char_meaning_merge.acceptance.py", doc,
            "MCD-A5 RED: char_stroke_lib.py docstring 冻结验收指针未改指本轮现役件 "
            "test_char_meaning_merge.acceptance.py（禁落 archive 名目录，验收纪律 §5）。")

    # --- MCD-A5: TS 两条出口路径 merge（node 真跑）------------------------------
    def test_mcd_a5_ts_two_exit_paths_meaning_merge(self):
        """coverage: MCD-A5 — **流式与非流式两条出口路径一致**：`k1cRewriteText`
        （非流式）+ `k1cLookupBlock`（流式，经 `k1cStreamStep`）两条路径都取最小块
        `meaning` 覆盖 cache 值再 emit；缺键回落 cache 原值；替换后 JSON 合法；无最小
        块泄漏。node 真跑部署实际注入的 TS helper。"""
        driver = r"""
const withM = JSON.stringify({view:"char",data:{char:"勇",meaning:"勇敢；有胆量"}});
const noM   = JSON.stringify({view:"char",data:{char:"勇"}});
function blk(t){ const m = /```display\s*(\{.*?\})\s*```/s.exec(t||""); return m?JSON.parse(m[1]):null; }
const T1 = "勇字这样写，一共九笔哦。\n```display\n"+withM+"\n```\n";
const T2 = "勇字这样写，一共九笔哦。\n```display\n"+noM+"\n```\n";
const ns_ov = blk(k1cRewriteText(T1));
const ns_fb = blk(k1cRewriteText(T2));
const st_ov = blk(k1cStreamStep({id:"ns"}, T1));
const st_fb = blk(k1cStreamStep({id:"sf"}, T2));
console.log(JSON.stringify({
  ns_merge: ns_ov && ns_ov.data && ns_ov.data.meaning,
  ns_fallback: ns_fb && ns_fb.data && ns_fb.data.meaning,
  st_merge: st_ov && st_ov.data && st_ov.data.meaning,
  st_fallback: st_fb && st_fb.data && st_fb.data.meaning,
  ns_keys: ns_ov && ns_ov.data ? Object.keys(ns_ov.data).sort() : null,
  st_keys: st_ov && st_ov.data ? Object.keys(st_ov.data).sort() : null,
  ns_leak: k1cRewriteText(T1).indexOf(withM) >= 0,
  st_leak: k1cStreamStep({id:"lk"}, T1).indexOf(withM) >= 0
}));
"""
        r = self._run_ts(driver)
        exp_keys = sorted(CACHE_ROWS[CACHE_CHAR].keys())
        self.assertEqual(
            r.get("ns_merge"), MINIMAL_MEANING,
            "MCD-A5 RED(非流式 k1cRewriteText): 最小块 meaning 未覆盖 cache 值——"
            "播报的中文解释没写进 display 块 meaning 字段。")
        self.assertEqual(
            r.get("st_merge"), MINIMAL_MEANING,
            "MCD-A5 RED(流式 k1cStreamStep→k1cLookupBlock): 最小块 meaning 未覆盖 cache "
            "值——两条出口路径不一致（260907-g-relay-fix 同族：离线绿、真机一红）。")
        self.assertEqual(
            r.get("ns_fallback"), CACHE_MEANING,
            "MCD-A5 RED(非流式): 最小块缺 meaning 时未回落 cache 原值。")
        self.assertEqual(
            r.get("st_fallback"), CACHE_MEANING,
            "MCD-A5 RED(流式): 最小块缺 meaning 时未回落 cache 原值（两路径须一致）。")
        self.assertEqual(
            r.get("ns_keys"), exp_keys,
            "MCD-A5 RED(非流式): emit 块 data 键集非 cache 全量 D3——最小块成品泄漏/缺键。")
        self.assertEqual(
            r.get("st_keys"), exp_keys,
            "MCD-A5 RED(流式): emit 块 data 键集非 cache 全量 D3。")
        self.assertFalse(r.get("ns_leak"),
                         "MCD-A5 RED(非流式): 最小块成品外泄（HOLD-AND-REPLACE 失守）。")
        self.assertFalse(r.get("st_leak"),
                         "MCD-A5 RED(流式): 最小块成品外泄（HOLD-AND-REPLACE 失守）。")

    # --- MCD-A5: TS 两函数体 merge 在场（STRUCTURAL）---------------------------
    def test_mcd_a5_ts_helper_structural_two_paths_carry_merge(self):
        """coverage: MCD-A5 structural — `make_openclaw_patch._helper_ts()` 内含
        meaning merge 逻辑，且 `k1cRewriteText` 与 `k1cLookupBlock` **两函数体**均触及
        （`meaning` 或共享 merge 助手调用）——两出口路径一致的结构半；node 行为齿的
        真实执行可能因 node 缺席 skip，本齿独立承载「两路径都覆盖」。"""
        ts = _load_ts_helper()
        self.assertIn(
            "meaning", ts,
            "MCD-A5 RED: TS helper 全域零 meaning——最小块 meaning 覆盖逻辑无落点。")
        for fn in ("k1cRewriteText", "k1cLookupBlock"):
            region = _ts_func_region(ts, fn)
            self.assertIsNotNone(
                region, f"MCD-A5 RED(infra): TS helper 找不到 {fn}（出口路径被拆）。")
            self.assertTrue(
                "meaning" in region or "merge" in region.lower(),
                f"MCD-A5 RED: {fn} 函数体未触及 meaning 覆盖（meaning / merge 助手"
                "调用均未命中）——该出口路径未做最小块 meaning 覆盖（两路径不一致）。")

    # --- MCD-A7: TS 两出口路径剥离 <details> 附录（node 真跑）-------------------
    def test_mcd_a7_ts_two_exit_paths_strip_non_speech_appendix(self):
        """coverage: MCD-A7 — 含 `<details>…</details>` 附录的回复经 `k1cRewriteText`
        （非流式）+ `k1cStreamStep`→`k1cLookupBlock`（流式）**两条出口路径**处理后，
        下发内容（及由其派生的 TTS 文本）均不含该附录（human finding③ 实测：网关
        display-strip 只剥 ```display 围栏、未剥该附录 -> 整块进 TTS 被念出）。"""
        driver = r"""
const minimal = JSON.stringify({view:"char",data:{char:"勇",meaning:"勇敢；有胆量"}});
const appx = "<details><summary>来源与自检</summary>数据入口：char_cache.json 命中勇；笔画名称来源：bishun.hao86.com、bishun.fjnews.com。另注：勇_stroke_order.svg 只是视觉参考，C1 契约里没有 svg 字段。</details>";
const T = "勇字这样写，一共九笔哦。\n```display\n"+minimal+"\n```\n"+appx;
const ns = k1cRewriteText(T);
const st = k1cStreamStep({id:"s"}, T);
console.log(JSON.stringify({ns, st}));
"""
        r = self._run_ts(driver)
        for path in ("ns", "st"):
            body = r.get(path) or ""
            for tok in APPENDIX_TOKENS:
                self.assertNotIn(
                    tok, body,
                    f"MCD-A7 RED({path}): 出口文本仍含附录片段 {tok!r}——非口语内容"
                    "（自检/来源/明细/HTML 结构）夹带进 TTS 被念出（两路径都要剥）。")

    # --- MCD-A7: 正向 case（防误伤）-------------------------------------------
    def test_mcd_a7_ts_positive_normal_speech_not_stripped(self):
        """coverage: MCD-A7（正向 / must-stay-green）— 正常口语正文（含普通括号/说明/
        中文标点）不被误剥：剥离规则过宽会伤正文。两条出口路径均原样保留纯口语文本
        （无 display 块 -> 原样透传）。"""
        driver = (
            "const P = " + json.dumps(NORMAL_SPEECH, ensure_ascii=False) + ";\n"
            "console.log(JSON.stringify({ns: k1cRewriteText(P), "
            "st: k1cStreamStep({id:\"p\"}, P)}));\n"
        )
        r = self._run_ts(driver)
        self.assertEqual(
            r.get("ns"), NORMAL_SPEECH,
            "MCD-A7 RED(正向): 非流式路径误改了正常口语正文——剥离规则过宽（伤正文）。")
        self.assertEqual(
            r.get("st"), NORMAL_SPEECH,
            "MCD-A7 RED(正向): 流式路径误改了正常口语正文——剥离规则过宽（伤正文）。")

    # --- MCD-A7: TS helper 结构性剥离在场 --------------------------------------
    def test_mcd_a7_ts_helper_structural_strip_present(self):
        """coverage: MCD-A7 structural — `make_openclaw_patch._helper_ts()` 内含
        `<details>` 附录剥离逻辑（`<details` 与 `</details>` 锚），且**两条出口路径**
        函数体（`k1cRewriteText` 非流式 / `k1cStreamStep` 流式出口）均触及剥离
        （`strip` 助手调用或 `details` 在场）；node 行为齿的真实执行可能因 node 缺席
        skip，本齿独立承载「剥离逻辑在场」。"""
        ts = _load_ts_helper()
        self.assertIn("<details", ts,
                      "MCD-A7 RED: TS helper 零 `<details` 剥离锚——附录（自检/来源/明细/"
                      "HTML 结构）无剥离逻辑（human finding③ 真实缺陷信号）。")
        self.assertIn("</details", ts, "MCD-A7 RED: TS helper 零 `</details>` 闭合锚。")
        for fn in ("k1cRewriteText", "k1cStreamStep"):
            region = _ts_func_region(ts, fn)
            self.assertIsNotNone(
                region, f"MCD-A7 RED(infra): TS helper 找不到 {fn}。")
            self.assertTrue(
                "strip" in region.lower() or "details" in region,
                f"MCD-A7 RED: {fn} 函数体未触及 <details> 剥离（strip 助手调用 / "
                "details 均未命中）——该出口路径仍夹带附录（两路径都要剥）。")

    # --- prompt 锚齿：版本无关接线解析 ------------------------------------------
    def _wired_prompt_files(self):
        render_src = _read_text(PROMPT_RENDER_SH)
        decl_src = _read_text(PROMPT_DECL_YAML)
        rendered = re.findall(
            r"PROMPT_SRC\s*=.*?(openclaw-system-prompt\.v\d+\.md)", render_src)
        declared = re.findall(
            r"(?:artifact|source|deploy_target)\s*:\s*\"[^\"]*"
            r"(openclaw-system-prompt\.v\d+\.md)", decl_src)
        return rendered, declared

    def test_mcd_prompt_wired_two_faces_version_agnostic(self):
        """coverage: prompt（接线守卫 / must-stay-green，版本无关）— 从渲染面
        （apply-xiaozhi-overlay.sh 的 PROMPT_SRC）与声明面（provider.config.yaml
        system_prompt 三键）解析出**当前真正被接线的那一版** prompt 文件名，断言两面
        一致且该文件落盘非空。**避开版本字面耦合**（不钉 v5 字面——builder bump 后
        本齿不误红；两面分叉 = 生产所用与被测所载不是同一块 = 红）。"""
        rendered, declared = self._wired_prompt_files()
        self.assertTrue(
            rendered and declared,
            "MCD-prompt RED: 渲染面/声明面未解析出版本化 prompt 文件名——prompt 落盘即"
            "孤儿（apply-xiaozhi-overlay.sh PROMPT_SRC / provider.config.yaml 未指向"
            "openclaw-system-prompt.vN.md）。")
        self.assertEqual(
            set(rendered), set(declared),
            f"MCD-prompt RED: 渲染面 {sorted(rendered)} 与声明面 {sorted(declared)} 指向"
            "不同版本——「验收所测 = 生产所用」断裂（overlay 渲染的 prompt 与声明不符）。")
        for name in sorted(set(rendered)):
            path = DEPLOY_DIR / name
            self.assertTrue(path.is_file(),
                            f"MCD-prompt RED: 被接线的 {name} 未落盘（孤儿声明）。")
            self.assertTrue(_read_text(path).strip(),
                            f"MCD-prompt RED: 被接线的 {name} 是空文件。")

    def test_mcd_prompt_char_section_meaning_and_discipline(self):
        """coverage: MCD-A5/A7（prompt 锚齿，版本无关）— 当前接线的 prompt 正文内，
        **char 段**（含「最小意图块」的段落及其相邻段）携带：meaning 新约（data 含
        `meaning` = 中文释义，同回复播报同源）+「严禁自生成/复述 strokes/medians」
        约束仍在 + 语音纪律锚（v7 起为「简洁不啰嗦」；原「≤3 句」锚随 human 2026-09-25
        裁定删除硬性句数上限而废止，NFR-SAFE-1 同批修订——supersession，非丢锚）。
        沿 a8 载体齿「解析渲染/声明两面得版本再断言」形态，避开版本字面耦合。"""
        rendered, _ = self._wired_prompt_files()
        self.assertTrue(rendered, "MCD-prompt RED(infra): 无接线 prompt 版本可解析。")
        body = _read_text(DEPLOY_DIR / sorted(set(rendered))[0])
        paras = re.split(r"\n\s*\n", body)
        idx = [i for i, p in enumerate(paras) if "最小意图块" in p]
        self.assertTrue(
            idx,
            "MCD-prompt RED: 接线 prompt 找不到 char C1「最小意图块」段——查字最小块机制"
            "措辞被删。")
        # char C1 约定可能落在「增量扩展区 包4」段与「硬约束」段——取两者并集
        # （内容驱动选节，不钉标题措辞；版本无关）。
        window = "\n".join(paras[i] for i in idx)

        self.assertIn(
            "meaning", window,
            "MCD-prompt RED: char 段无 meaning 新约——最小块 data 未增中文释义键"
            "（human 触点①决策 2：把已播报的中文解释写进 display 块既有 meaning 字段）。")
        self.assertTrue(
            "char" in window and "meaning" in window,
            "MCD-prompt RED: char 段未同时含 char 与 meaning——最小块形状未含释义键。")
        self.assertTrue(
            "strokes" in window and "medians" in window and
            any(w in window for w in ("禁止", "不得", "严禁", "不要自己", "禁自生成")),
            "MCD-prompt RED: char 段丢「严禁自生成/复述 strokes/medians」约束——坐标"
            "数据禁令（C1 立论前提）必须逐字保留。")
        # 语音纪律锚（supersession, human 2026-09-25 裁 + 260925-prompt-v7-round-routing）：
        # v6 及以下 = 「三句/3 句/句以内」硬上限；v7 起 = 「简洁不啰嗦」软纪律（NFR-SAFE-1
        # 同批修订）。锚取两代口径并集，任一代在场即绿、两代语义全删才红。
        self.assertTrue(
            any(w in window for w in ("三句", "3 句", "句以内", "简洁", "不啰嗦")),
            "MCD-prompt RED: char 段丢语音纪律锚——v6 口径（≤3 句）与 v7 口径（简洁"
            "不啰嗦）至少一代必须在场，语音长度纪律整体消失不可接受。")

    def test_mcd_prompt_no_appendix_anchor(self):
        """coverage: MCD-A7（prompt 锚齿，版本无关）— 当前接线的 prompt 正文携带新约：
        **面向设备回复只含口语正文 + 一个 display 块、禁任何自检/来源/附录/明细/HTML
        结构**（human finding③：附录进 TTS 的生成端约束）。"""
        rendered, _ = self._wired_prompt_files()
        self.assertTrue(rendered, "MCD-prompt RED(infra): 无接线 prompt 版本可解析。")
        body = _read_text(DEPLOY_DIR / sorted(set(rendered))[0])
        has_ban_word = any(
            w in body for w in ("禁止", "不得", "严禁", "不要", "禁"))
        has_appendix = any(
            w in body for w in ("附录", "自检", "明细"))
        has_html = any(
            w in body for w in ("HTML", "html", "details", "<details"))
        self.assertTrue(
            has_appendix and has_html and has_ban_word,
            "MCD-A7 RED: prompt 缺「禁自检/来源/附录/明细/HTML 结构」新约——生成端未约束"
            "非口语附录（附录会夹带进 TTS 被念出；现正文检测 appendix=%s html=%s "
            "banword=%s）。" % (has_appendix, has_html, has_ban_word))


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(CharMeaningMergeAcceptance)
    runner = unittest.TextTestRunner(verbosity=2)
    raise SystemExit(0 if runner.run(suite).wasSuccessful() else 1)