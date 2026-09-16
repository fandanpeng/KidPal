"""Frozen acceptance tests for TaskPackage 260829-char-stroke
(subsystem: firmware+server; 云侧 C1: OpenClaw 侧静态字库查表拼装 D3 块 +
目标字表预热 + LRU 缓存 + system prompt v2; mergeOrder 7).

Device-face twin: firmware/main/test/char_stroke.acceptance.c (CHR-A1~A7 +
闸 A 缺字段防御降级, weak-symbol frozen contract). This file carries the CLOUD
half: CHR-A8 (查表拼装, 逐字对照数据源正确, LLM 只出意图＋字不复述坐标 JSON)、
CHR-A9 (预热 + LRU + char 查询端到端 5 秒档 live)、CHR-E3 (静态字库挂 OpenClaw
侧绝不入 shim/gateway 薄桥红线 + system prompt v2 结构齿).

Nature: OFFLINE-DETERMINISTIC / STRUCTURAL teeth (auto-run; the red baseline)
+ LIVE faces (@pytest.mark.live, SSH to the KidPal ECS box; NOT auto-run by any
merge gate — server/acceptance/conftest.py 机械 skip 闸, ARQ-260901-g3 纪律).
A bare import/collection error must never be the reason a test is red — module
absence surfaces as a missing-behaviour RED (self.fail), not an import crash.

Coverage map (acceptanceRef / exitCriteria -> test method):
  CHR-A8 (静态字库查表拼装, 断言打在拼装产物上非 LLM 输出)
        -> test_a8_char_lib_module_landed              [offline; RED today]
        -> test_a8_assembly_product_matches_source     [offline; RED today]
        -> test_a8_minimal_block_extract_and_replace   [offline; RED today]
        -> test_a8_live_block_matches_makemeahanzi     [live]
  CHR-A9 (目标字表预热 + LRU 生效; 5 秒档 live 不 auto-run)
        -> test_a9_lru_and_warmup_semantics            [offline; RED today]
        -> test_a9_live_char_latency_5s_band           [live]
  CHR-E3 (静态字库挂 OpenClaw 侧绝不入 shim; §1.5 薄桥; prompt v2)
        -> test_e3_prompt_v2_landed_with_discipline    [offline; RED today]
        -> test_e3_current_prompt_wired_and_carries_char_c1
                                                       [offline green-guard; 原
                                                       test_e3_prompt_v2_wired_into_deploy,
                                                       2026-09-08 版本字面 -> 版本无关
                                                       接线+内容守卫, human 决策 c54bbcde
                                                       / 升级单 80deedcb]
        -> test_e3_gateway_thin_bridge_untouched       [offline green-guard]
  闸 A additive 纪律 (MVP 不动网关 display_schema, 板子侧防御降级)
        -> test_gate_a_display_schema_untouched        [offline green-guard]
  live precondition -> test_live_ssh_reachable          [live]

FROZEN OFFLINE CONTRACT (builder MUST implement importable pure-python modules
under server/deploy/openclaw-patch/ — 机制 B = OpenClaw 回复出口补丁的本地可测
纯逻辑核; 模块文件名 builder 可自定, 但函数/类契约不得偏移; discovery 按契约
标记文本扫装, 禁 fastapi/httpx 等 import-time 依赖, 任何机器常年可跑):

  assemble_char(graphics: dict, dictionary: dict, words=None) -> dict
      graphics   = makemeahanzi graphics.txt 记录 (parsed):
                   {"character","strokes":[str,...],"medians":[[[x,y],...],...]}
      dictionary = makemeahanzi dictionary.txt 记录 (parsed):
                   {"character","pinyin":[...],"radical","strokes":int,
                    "structure","definition"}
      words      = 可选组词列表 (契约 §1.3① words; makemeahanzi 不含组词,
                   由部署面补充源提供)
      返回契约 §1.3① 全量 D3 data dict, set(keys) == FULL_DATA_KEYS, 逐字段
      与数据源对照: char=graphics["character"](与 dictionary["character"] 必须
      一致否则 ValueError), strokes/medians 原样 (逐字对照数据源), pinyin=
      "、".join(dictionary["pinyin"]) (多音字无损), radical/dictionary["radical"],
      strokeCount=dictionary["strokes"] 有则用之、否则 len(graphics["strokes"])
      (数据集实况回退, 见 REVISION LOG), structure=dictionary["structure"] 空时
      回退 dictionary["decomposition"] (同 IDC 格式), meaning=
      dictionary["definition"] (兼容 "meaning" 键), words=参数 or []。

  extract_minimal_char_block(text: str) -> dict | None
      从 OpenClaw 回复正文提取第一个 ```display fenced 且 view=="char" 的块并
      json 解析; 无 char 块 -> None (机制 B 的提取语义: LLM 只出意图＋字,
      最小块 {"view":"char","data":{"char":"勇"}} 形态或已全量块都返回)。

  replace_minimal_block(text: str, full_data: dict) -> str
      把 text 中的 char display 块 JSON 替换为拼装后的全量 full_data
      (json.dumps ensure_ascii=False), 语音部分原样保留; 无 char 块时原样返回。
      (机制 B 的替换语义: 网关永不见最小块成品。)

  class CharLruCache
      __init__(capacity: int); get(key) -> value|None (命中晋升 LRU);
      put(key, value) — 超容量淘汰最久未用项; 同 key 重 put 刷新新近度与值。

  def warmup_cache(cache, target_chars, loader) -> int
      目标字表预热: 对 dedup(保持首次出现序) 的 target_chars 逐字 loader(char)
      装入 cache, 返回预热条数 (dedup 后)。

Constants frozen: FULL_DATA_KEYS (契约 §1.3① 九键) / REQUIRED_DATA_KEYS
(渲染关键六件, 设备侧 kidpal_char_data_validate 降级门同一集合).

system prompt v2: server/deploy/openclaw-system-prompt.v2.md (版本化落盘,
v1 保持共存可 diff). 断言措辞: 「禁自生成/复述 strokes/medians」+ data 原样用
查字工具结果 (闸 A 预审第四节建议措辞) + 三句纪律收紧 (包 2 收口留根: A4
sample 1 口语 4 句超三句纪律, 本包顺带收紧, v2 必须比 v1 更硬)。「验收所测 =
生产所用」: live 探针以 v2 文件为唯一 prompt 源 (VRT-A4/E4 先例)。

Red baseline (vehicle state, 2026-09-07 acceptance-author 实测):
  - server/deploy/openclaw-patch/ 不存在, 契约模块全缺 (A8/A9 offline 红);
  - openclaw-system-prompt.v2.md 不存在、无任何 deploy 载体引用 (E3 双齿红);
  - gateway 面零 char 组装引用 (E3 薄桥守卫绿) + display_schema 未动 (闸 A 守卫绿);
  - live 面被 conftest 机械 skip, 不 auto-run.
  See delivery/taskpackages/260829-char-stroke/red-baseline-summary.md.

Collection discipline: 文件名两点 (test_*.acceptance.py) 须 importlib 模式 (G 包先例):
  cd server && python -m pytest acceptance/test_char_stroke.acceptance.py -q --import-mode=importlib
live 类被 conftest 默认 skip; RUN_LIVE=1 或 -m live 显式执行。本模块带 standalone
__main__ runner (Pod S / G / VRT 先例), 默认只跑 offline 类。

REVISION LOG (acceptance-author only; tighten-only discipline):
  2026-09-07  initial freeze (acceptance-author).
  2026-09-07  CHR-E3 prompt-v2 wiring tooth tightened: 首版扫全 deploy corpus 被
              OPENCLAW_SYSTEM_PROMPT_CARRIER 前瞻注记打出假绿, 收窄到渲染面
              (apply-xiaozhi-overlay.sh) / 声明面 (provider.config.yaml)。
  2026-09-07  数据集实况 vs 冻结假设处置 (live 阶段实测, ECS 9574 字全量:
              dictionary.txt 实际字段 = character/definition/pinyin/decomposition/
              radical/matches, 无 strokes/无 structure; 与 spec §3.0.2 记载及冻结
              假设不符): (a) live A8 strokeCount 对照改打数据源真值
              len(gfx["strokes"])（笔画数 ≡ 笔画 path 条数）; (b) 新增 offline 齿
              钉 assemble_char 回退语义（无 strokes 键 -> len(strokes); structure
              空 -> decomposition），带键 fixture 行为零变更; (c) 对应实现面回退
              （char_stroke_lib.py / warm_char_cache.py 守卫）由 builder 落
              （acceptance-author 被 enforce-scope 拦, diff 见 red-baseline-summary
              修复记录）——本齿在其落地前对该未修实现为红, 落地即绿。断言语义零放宽
              （笔画数真值比原 None 对照更严）。
"""

from __future__ import annotations

import base64
import importlib.util
import json
import os
import re
import subprocess
import sys
import time
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
GATEWAY_DIR = SERVER / "gateway"
CHAR_PATCH_DIR = DEPLOY_DIR / "openclaw-patch"

# FROZEN artifacts / constants.
PROMPT_V2_PATH = DEPLOY_DIR / "openclaw-system-prompt.v2.md"
PROMPT_V1_PATH = DEPLOY_DIR / "openclaw-system-prompt.v1.md"

FULL_DATA_KEYS = {"char", "pinyin", "radical", "strokeCount", "strokes",
                  "medians", "structure", "meaning", "words"}          # 契约 §1.3①
REQUIRED_DATA_KEYS = {"char", "pinyin", "radical", "strokeCount",
                      "strokes", "medians"}                             # 渲染关键 (设备降级门)
assert REQUIRED_DATA_KEYS <= FULL_DATA_KEYS  # logic guard: frozen key sets must nest

# Contract-marker text used to DISCOVER the builder's module (name-agnostic;
# the function/class contract itself is frozen in the docstring above).
CONTRACT_MARKERS_CORE = ("def assemble_char(", "def extract_minimal_char_block(",
                         "def replace_minimal_block(")
CONTRACT_MARKERS_CACHE = ("class CharLruCache", "def warmup_cache(")

# Live probe config (env-overridable; defaults are the frozen decisions).
SSH_HELPER = os.environ.get("SSH_HELPER", "C:/tmp/ssh_helper.py")
ECS_HOST = os.environ.get("ECS_HOST", "8.137.91.16")
OPENCLAW_LOOPBACK = os.environ.get("OPENCLAW_LOOPBACK", "http://127.0.0.1:18789")
OPENCLAW_MODEL = os.environ.get("OPENCLAW_MODEL", "openclaw/default")
OPENCLAW_TOKEN = os.environ.get("OPENCLAW_TOKEN", "")
CHAR_DATA_DIR = os.environ.get("CHAR_DATA_DIR", "/opt/kidpal/chardata")
# CHR-A9「char 查询 109s -> 5 秒档」: 装配+推理 ~5s + 网络余量 = 8s 预算口径.
CHAR_E2E_BUDGET_S = float(os.environ.get("CHAR_E2E_BUDGET_S", "8"))
CHAR_LIVE_QUESTION = "勇敢的『勇』字怎么写？"

# ---------------------------------------------------------------------------
# Fixtures — self-made makemeahanzi-format records (红基线不需要真数据; 真数据
# 对照留 live: test_a8_live_block_matches_makemeahanzi 对 ECS 数据集逐字对照).
GFX_FIXTURE = {
    "character": "测",
    "strokes": [
        "M 100 100 L 900 100 L 900 300 L 100 300 Z",
        "M 200 400 Q 500 900 800 400",
    ],
    "medians": [
        [[100, 200], [900, 200]],
        [[200, 500], [500, 700], [800, 500]],
    ],
}
DICT_FIXTURE = {
    "character": "测",
    "pinyin": ["cè"],
    "radical": "氵",
    "strokes": 2,
    "structure": "⿰氵刂",
    "definition": "测定；测量",
}
WORDS_FIXTURE = ["测试", "测验", "测控"]


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _files_under(root: Path, suffix: str):
    if not root.is_dir():
        return []
    return sorted(p for p in root.rglob("*")
                  if p.is_file() and (p.suffix == suffix or (suffix == "" and not p.suffix)))


def _contains_any(text: str, tokens) -> list:
    low = text.lower()
    return [t for t in tokens if t.lower() in low]


def _load_contract_module(markers, role: str):
    """Discover + load the builder's pure module under server/deploy/openclaw-patch/.

    Returns (module|None, error_string). A missing module must surface as a
    missing-behaviour RED (self.fail), NOT as an import crash of THIS file.
    """
    if not CHAR_PATCH_DIR.is_dir():
        return None, (f"server/deploy/openclaw-patch/ missing entirely — the {role} "
                      "frozen contract module is not implemented")
    hits = []
    for path in sorted(CHAR_PATCH_DIR.rglob("*.py")):
        src = _read_text(path)
        if src and all(m in src for m in markers):
            hits.append(path)
    if not hits:
        return None, (f"no module under server/deploy/openclaw-patch/ carries the {role} "
                      f"frozen contract markers {markers} — 模块可自定名但契约不得偏移")
    path = hits[0]
    try:
        spec = importlib.util.spec_from_file_location(f"kidpal_cs_{path.stem}", path)
        if spec is None or spec.loader is None:
            return None, f"{path} not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"{path} failed to import: {e!r}. C1 frozen contract: 字库/拼装"
                      "模块必须是纯 python (import 期禁 fastapi/httpx/LLM 依赖).")


def _extract_display_block(content: str):
    """Extract the first ```display fenced JSON from a reply (VRT fence 路径口径)."""
    m = re.search(r"```display\s*(\{.*?\})\s*```", content or "", re.DOTALL)
    if not m:
        return False, None
    try:
        return True, json.loads(m.group(1))
    except (ValueError, TypeError):
        return True, None


class CharStrokeOfflineAcceptance(unittest.TestCase):
    """Offline-deterministic / structural acceptance — the red baseline's teeth."""

    maxDiff = None

    # --- CHR-A8 (module landed) -----------------------------------------------
    def test_a8_char_lib_module_landed(self):
        """coverage: CHR-A8 — 静态字库查表拼装纯模块在树 (server/deploy/openclaw-patch/,
        挂 OpenClaw 侧, 机制 B 的本地可测核): assemble_char + extract_minimal_char_block +
        replace_minimal_block 三件契约齐全且可独立 import (纯 python, 无 import-time
        LLM/网关依赖)."""
        mod, err = _load_contract_module(CONTRACT_MARKERS_CORE, "CHR-A8 拼装")
        if mod is None:
            self.fail("CHR-A8 RED: " + err)

    # --- CHR-A8 (assembly product vs data source) ------------------------------
    def test_a8_assembly_product_matches_source(self):
        """coverage: CHR-A8 — 拼装产物逐字段对照数据源正确 (断言打在拼装产物上,
        工具输出 = makemeahanzi 原文, 非 LLM 输出; intake gate-a 待办 3):
        九键全量 + strokes/medians 原样 + pinyin 顿号无损连接 + strokeCount 与
        strokes 长度自洽."""
        mod, err = _load_contract_module(CONTRACT_MARKERS_CORE, "CHR-A8 拼装")
        if mod is None:
            self.fail("CHR-A8 RED: " + err)

        data = mod.assemble_char(GFX_FIXTURE, DICT_FIXTURE, WORDS_FIXTURE)
        self.assertEqual(
            set(data.keys()), FULL_DATA_KEYS,
            f"CHR-A8 RED: assembled keys {sorted(data.keys())} != 契约 §1.3① 全量 "
            f"{sorted(FULL_DATA_KEYS)} — 拼装产物不是全量 D3 块.")
        self.assertEqual(data["char"], "测", "CHR-A8 RED: char 字段漂移.")
        self.assertEqual(
            data["strokes"], GFX_FIXTURE["strokes"],
            "CHR-A8 RED: strokes 不是 makemeahanzi 原文 — 坐标 JSON 必须逐字对照"
            "数据源 (CHR-A8: LLM 只出意图＋字, 坐标只出自静态字库).")
        self.assertEqual(
            data["medians"], GFX_FIXTURE["medians"],
            "CHR-A8 RED: medians 不是数据源原文 — 骨架点漂移 (笔顺动画/握笔小手"
            "会走错路).")
        self.assertEqual(
            data["pinyin"], "cè",
            "CHR-A8 RED: pinyin 带声调原样要求失守 (多音字以「、」无损连接, 多音"
            "字丢失 = 拼装缺陷).")
        self.assertEqual(data["radical"], "氵", "CHR-A8 RED: radical 漂移.")
        self.assertEqual(data["strokeCount"], 2, "CHR-A8 RED: strokeCount 漂移.")
        self.assertEqual(
            data["strokeCount"], len(data["strokes"]),
            "CHR-A8 RED: strokeCount 与 strokes 条数不自洽 — 数据完整性缺陷.")
        self.assertEqual(data["structure"], "⿰氵刂", "CHR-A8 RED: structure 漂移.")
        self.assertEqual(data["meaning"], "测定；测量", "CHR-A8 RED: meaning 漂移.")
        self.assertEqual(data["words"], WORDS_FIXTURE, "CHR-A8 RED: words 漂移.")

        # 不一致字符必须显式拒绝（防 graphics/dictionary 错行拼装）
        mismatched = dict(DICT_FIXTURE, character="勇")
        with self.assertRaises(ValueError,
                msg="CHR-A8 RED: graphics/dictionary 记录字符不一致必须拒绝 — 错行"
                    "拼装会把 A 字的笔顺发给孩子"):
            mod.assemble_char(GFX_FIXTURE, mismatched, WORDS_FIXTURE)

        # words 缺省容忍（可选补充源未提供时 words=[] 而非崩溃）
        data_nowords = mod.assemble_char(GFX_FIXTURE, DICT_FIXTURE)
        self.assertEqual(data_nowords["words"], [],
                         "CHR-A8 RED: words 缺省必须为空列表而非 KeyError/崩溃.")

        # 数据集实况回退语义（2026-09-07 live 实测: ECS dictionary.txt 实际字段 =
        # character/definition/pinyin/decomposition/radical/matches —— 无 strokes、
        # 无 structure）: 无 strokes 键 -> strokeCount = len(graphics["strokes"])
        # （笔画数 ≡ 笔画 path 条数）; structure 空时回退 decomposition（同 IDC 格式）。
        # 上面已有对照钉死 offline fixture 带 strokes/structure 键的行为零变更。
        dict_live_shape = {"character": "测", "pinyin": ["cè"], "radical": "氵",
                           "decomposition": "⿰氵刂", "definition": "测定；测量"}
        data_live = mod.assemble_char(GFX_FIXTURE, dict_live_shape, WORDS_FIXTURE)
        self.assertEqual(
            data_live["strokeCount"], 2,
            "CHR-A8 RED: dictionary 无 strokes 键时 strokeCount 必须回退为笔画 path "
            "条数 — 恒 0 会让「共 N 画」与进度珠全错 (数据集实况回退语义).")
        self.assertEqual(
            data_live["structure"], "⿰氵刂",
            "CHR-A8 RED: dictionary 无 structure 键时必须回退 decomposition — "
            "结构字段不得为空串 (数据集实况回退语义).")

    # --- CHR-A8 (minimal-block extract + replace) -------------------------------
    def test_a8_minimal_block_extract_and_replace(self):
        """coverage: CHR-A8 — 「LLM 只出意图＋字、不复述坐标 JSON」的机械承载:
        最小块提取语义 (```display fence 内 view=="char") + 拼装替换语义
        (最小块 -> 全量块, 语音部分原样, 网关永不见最小块成品)."""
        mod, err = _load_contract_module(CONTRACT_MARKERS_CORE, "CHR-A8 拼装")
        if mod is None:
            self.fail("CHR-A8 RED: " + err)

        speech = "勇字这样写，一共 9 笔，跟着我一笔一画写哦！"
        minimal = '{"view":"char","data":{"char":"勇"}}'
        reply = f"{speech}\n```display\n{minimal}\n```\n"

        got = mod.extract_minimal_char_block(reply)
        self.assertEqual(
            got, {"view": "char", "data": {"char": "勇"}},
            "CHR-A8 RED: 最小块提取语义失守 — 机制 B 无法从回复中拿到 LLM 意图＋字.")
        self.assertIsNone(
            mod.extract_minimal_char_block("没有显示指令的普通回复"),
            "CHR-A8 RED: 无 char 块必须返回 None 而非误提取.")
        self.assertIsNone(
            mod.extract_minimal_char_block('```display\n{"view":"text","data":{"text":"hi"}}\n```'),
            "CHR-A8 RED: 非 char view 的块不得被 char 提取语义吞掉.")

        full = mod.assemble_char(GFX_FIXTURE, DICT_FIXTURE, WORDS_FIXTURE)
        replaced = mod.replace_minimal_block(reply, full)
        self.assertIn("```display", replaced, "CHR-A8 RED: 替换产物丢了 display fence.")
        self.assertNotIn(
            minimal, replaced,
            "CHR-A8 RED: 最小块未被替换 — 网关将看到 LLM 复述形态, 109s 问题回归 "
            "(机制 B: 拼装替换语义).")
        self.assertTrue(
            replaced.startswith(speech) or replaced.split("```display")[0].strip() == speech,
            "CHR-A8 RED: 替换动了语音部分 — 语音正文必须原样保留 (契约 §1.1 两部分).")
        found, payload = _extract_display_block(replaced)
        self.assertTrue(found and payload == full,
                        "CHR-A8 RED: 替换后的块不是拼装的全量 D3 JSON (roundtrip 失败).")
        self.assertIn(
            "strokes", json.dumps(payload, ensure_ascii=False),
            "CHR-A8 RED: 全量块必须带 strokes — 设备侧光栅示范字的数据源 (CHR-A7).")

    # --- CHR-A9 (LRU + warmup, PC-testable half) --------------------------------
    def test_a9_lru_and_warmup_semantics(self):
        """coverage: CHR-A9 (PC 半) — LRU 命中/淘汰/重 put 刷新新近度 + 目标字表
        预热 (dedup 保持首现序, loader 逐字装配入缓存)."""
        mod, err = _load_contract_module(CONTRACT_MARKERS_CACHE, "CHR-A9 LRU/预热")
        if mod is None:
            self.fail("CHR-A9 RED: " + err)

        cache = mod.CharLruCache(capacity=2)
        self.assertIsNone(cache.get("勇"), "CHR-A9 RED: 空缓存 get 必须未命中 (None).")
        cache.put("一", {"char": "一"})
        cache.put("二", {"char": "二"})
        self.assertEqual(cache.get("一"), {"char": "一"}, "CHR-A9 RED: 命中返回错值.")
        cache.get("一")  # promote 一
        cache.put("三", {"char": "三"})  # capacity exceeded -> evict LRU = 二
        self.assertIsNone(cache.get("二"),
                          "CHR-A9 RED: 最久未用项未被淘汰 — LRU 淘汰语义失守 (A9).")
        self.assertIsNotNone(cache.get("一"),
                             "CHR-A9 RED: 刚被访问的一被误淘汰 — 命中未晋升新近度.")
        self.assertIsNotNone(cache.get("三"), "CHR-A9 RED: 新写入项丢失.")

        # 重 put 刷新新近度 + 覆盖值
        cache.put("一", {"char": "一", "v": 2})
        cache.put("三", {"char": "三"})  # 一 是最近使用, 淘汰 三
        self.assertIsNone(cache.get("三"),
                          "CHR-A9 RED: 重 put 未刷新新近度 — 热字被误淘汰.")
        self.assertEqual(cache.get("一"), {"char": "一", "v": 2},
                         "CHR-A9 RED: 重 put 未覆盖值.")

        # 预热: dedup + 保持首现序 + 逐字 loader
        warm = mod.CharLruCache(capacity=10)
        calls = []

        def loader(ch):
            calls.append(ch)
            return {"char": ch, "assembled": True}

        n = mod.warmup_cache(warm, ["勇", "勇", "敢", "星", "勇"], loader)
        self.assertEqual(n, 3, "CHR-A9 RED: 预热未 dedup — 同字重复装配 (浪费/翻倍延迟).")
        self.assertEqual(calls, ["勇", "敢", "星"],
                         "CHR-A9 RED: loader 调用序/次数不对 — 预热必须逐字装配且去重.")
        for ch in ("勇", "敢", "星"):
            self.assertIsNotNone(warm.get(ch),
                                 f"CHR-A9 RED: 预热后 {ch} 不在缓存 — 预热语义失守 (A9).")

    # --- CHR-E3 (system prompt v2) ------------------------------------------------
    def test_e3_prompt_v2_landed_with_discipline(self):
        """coverage: CHR-E3 — system prompt v2 版本化落盘 (机制定稿后的措辞面):
        禁自生成/复述 strokes/medians + data 原样用查字工具结果 + 三句纪律收紧
        (v2 必须比 v1 更硬, 包 2 收口留根) + char view 白名单在场; v1 保持共存."""
        self.assertTrue(
            PROMPT_V1_PATH.is_file(),
            "E3 RED(infra): v1 版本化 prompt 不在了 — 版本化共存纪律被破坏 "
            "(后缀演进: .v2 新落盘、历史可 diff, VRT-E4 先例).")
        self.assertTrue(
            PROMPT_V2_PATH.is_file(),
            f"CHR-E3 RED: system prompt v2 缺失 ({PROMPT_V2_PATH}) — C1 机制的措辞面"
            "无处钉死 (闸 A 预审: v1 第 60-61 行包 4 占位须替换, 版本化落 .v2).")
        text = _read_text(PROMPT_V2_PATH)
        self.assertTrue(
            "```display" in text,
            "CHR-E3 RED: v2 丢了 ```display fenced-block 约定 — E1 结构回退 "
            "(契约 §1.6).")
        for view in ("char", "schedule", "story_list", "story_text", "text",
                     "home", "control"):
            self.assertIn(
                view, text,
                f"CHR-E3 RED: v2 view 白名单缺 {view!r} — 契约 §1.3/§1.4 白名单不全.")

        # 禁自生成/复述坐标 JSON (闸 A 预审第四节建议措辞)
        self.assertTrue(
            "strokes" in text and "medians" in text,
            "CHR-E3 RED: v2 未点名 strokes/medians — 「禁自生成/复述坐标数据」措辞"
            "必须显式钉在 prompt (CHR-A8 的生成端约束).")
        self.assertTrue(
            _contains_any(text, ["禁止", "不得", "严禁", "禁自生成", "不要自己"]),
            "CHR-E3 RED: v2 缺禁止性措辞 — 仅描述机制不构成纪律 (A8 三句收紧同理).")
        self.assertTrue(
            _contains_any(text, ["查字工具", "查库", "拼装", "查表"]),
            "CHR-E3 RED: v2 未对齐查表拼装机制 — LLM 与工具的分工未约定 (机制 B).")

        # 三句纪律收紧: 保留三句上限 + 比 v1 更硬的收紧措辞
        self.assertTrue(
            _contains_any(text, ["三句以内", "3 句以内", "不超过三句", "三句"]),
            "CHR-E3 RED: v2 丢三句纪律 — 包 2 收口留根的纪律上限必须保留.")
        self.assertTrue(
            _contains_any(text, ["严禁", "不得超过", "硬性", "严格", "必须"]),
            "CHR-E3 RED: v2 无收紧措辞 — 包 2 留根 (A4 sample 1 口语 4 句超三句)"
            "要求 v2 比 v1 更硬, 仅重复 v1 措辞不构成收紧.")

    def test_e3_current_prompt_wired_and_carries_char_c1(self):
        """coverage: CHR-E3 — 落盘 ≠ 孤儿文件 + 「验收所测 = 生产所用」的**版本无关**守卫:
        从渲染面 (apply-xiaozhi-overlay.sh 的 PROMPT_SRC) 与声明面
        (provider.config.yaml system_prompt.artifact|source|deploy_target) 解析出
        **当前真正被接线的那一版** prompt 文件名, 然后三重断言:
          ① 两面至少其一解析出版本化文件名 (整条接线被删 = 孤儿 = 红);
          ② 该版本文件在 server/deploy/ **确实落盘** (声明了却没落盘 = 红 — 原缺陷
             捕获力完整保留), 且渲染面与声明面指向**同一版** (两面分叉 = 生产所用
             与被测所载不是同一块 = 红);
          ③ 该**当前接线版文件的正文**逐字携带 char C1 段落锚点
             (「最小意图块」/「禁自生成」+strokes+medians/「查字工具」) —— 未来任何
             一版把 char C1 弄丢, 本守卫照样转红 (这才是 CHR-E3 真正要防的东西)。
        OPENCLAW_SYSTEM_PROMPT_CARRIER 声明文件里的前瞻注记不算接线 (red baseline
        假绿实证, 故齿仍只认渲染面/声明面两处结构化指向)。

        改判缘由 (2026-09-08; human 决策 c54bbcde-582f-42c2-a337-5260f890a799 /
        升级单 80deedcb-1ea8-4550-b1fa-f20c2f0376d7): 原齿断言「deploy 两面引用字面
        openclaw-system-prompt.v2」, 与后继包把 carrier 正常 forward-bump 到 vN
        (260829-story-p3 指 v3) 必然撞出**假红**。CHR-E3 的语义保重面从来不是「版本号
        必须等于 2」, 而是「被接线的那一版存在、两面一致、且内含本包 char C1 纪律」,
        故把版本字面替换为版本无关的接线解析 + 内容锚点。v2 自身的落盘与措辞纪律仍由
        姊妹齿 test_e3_prompt_v2_landed_with_discipline 原样钉死, 未放宽。"""
        render_src = _read_text(DEPLOY_DIR / "apply-xiaozhi-overlay.sh")
        decl_src = _read_text(DEPLOY_DIR / "provider.config.yaml")

        # ① 解析两面各自指向的版本化文件名 (只认结构化指向, 不认散文提及)
        rendered = re.findall(
            r'PROMPT_SRC\s*=.*?(openclaw-system-prompt\.v\d+\.md)', render_src)
        declared = re.findall(
            r'(?:artifact|source|deploy_target)\s*:\s*"[^"]*'
            r'(openclaw-system-prompt\.v\d+\.md)', decl_src)
        wired = rendered + declared
        self.assertTrue(
            wired,
            "CHR-E3 RED: 没有任何渲染/声明面结构化指向版本化 prompt — "
            "apply-xiaozhi-overlay.sh 无 PROMPT_SRC=openclaw-system-prompt.vN.md, "
            "provider.config.yaml system_prompt 三键也未指向版本化文件; prompt 落盘即"
            "孤儿文件, live 探针所测 ≠ 生产所用 (E4 先例; 真接线 = overlay 渲染切换或"
            "声明面更新).")

        # ② 两面一致 + 被指向的那一版确实落盘 (声明了却没有 = 孤儿, 原缺陷捕获力)
        self.assertEqual(
            set(rendered), set(declared),
            f"CHR-E3 RED: 渲染面 {sorted(rendered)} 与声明面 {sorted(declared)} 指向"
            "不同版本 — 两面分叉时 ECS 实际渲染的 prompt 与声明/被测的不是同一块 "
            "(「验收所测 = 生产所用」断裂).")
        for name in sorted(set(wired)):
            path = DEPLOY_DIR / name
            self.assertTrue(
                path.is_file(),
                f"CHR-E3 RED: 被接线的 {name} 在 server/deploy/ 未落盘 — 两面都指它, "
                "overlay 渲染时静默 WARN, 生产用的是旧版/空版 (孤儿声明).")
            self.assertTrue(
                _read_text(path).strip(),
                f"CHR-E3 RED: 被接线的 {name} 是空文件 — 接线到空 prompt 等于没接线.")

        # ③ 当前接线版正文必须逐字携带 char C1 锚点 (版本无关的内容承载守卫)
        for name in sorted(set(wired)):
            body = _read_text(DEPLOY_DIR / name)
            self.assertIn(
                "最小意图块", body,
                f"CHR-E3 RED: 当前接线的 {name} 丢了「最小意图块」 — char C1 机制"
                "(LLM 只出最小意图块、不整块复述 D3) 的措辞承载被删, 生产端会退回"
                "自生成坐标 (CHR-A8 生成端约束失守).")
            self.assertTrue(
                _contains_any(body, ["禁自生成", "禁止", "不得", "严禁", "不要自己"]) and
                "strokes" in body and "medians" in body,
                f"CHR-E3 RED: 当前接线的 {name} 丢了「禁自生成/复述 strokes·medians」"
                "纪律 — v2 定的坐标数据禁令必须被后续每一版逐字继承, 丢了即 LLM 复述"
                "9574 字坐标的回归通道 (机制 B 的立论前提).")
            self.assertTrue(
                _contains_any(body, ["查字工具", "查库", "拼装", "查表"]),
                f"CHR-E3 RED: 当前接线的 {name} 未对齐查表拼装机制 — 完整 D3 由谁产出"
                "未约定 (工具侧拼装是 C1 的另一半).")

    # --- CHR-E3 / 闸 A green-guards (must stay green) ------------------------------
    def test_e3_gateway_thin_bridge_untouched(self):
        """coverage: CHR-E3 — 薄桥红线 (契约 §1.5: 网关薄, 零自研业务逻辑): 静态
        字库/拼装/LRU 挂 OpenClaw 侧, server/gateway/** 零引用 — 本包 additive
        纪律的机械守卫 (机制 A 被否决的理由即此)."""
        corpus = "\n".join(_read_text(p) for p in _files_under(GATEWAY_DIR, ".py"))
        for banned in ("makemeahanzi", "assemble_char", "CharLruCache", "warmup_cache",
                       "openclaw-patch", "graphics.txt", "dictionary.txt"):
            self.assertNotIn(
                banned, corpus,
                f"CHR-E3 RED: server/gateway/ 引用了 {banned!r} — 字库/拼装面渗入网关"
                "违反薄桥红线 (§1.5; human 2026-09-01 冻结: 挂 OpenClaw 侧、绝不入 shim).")

    def test_gate_a_display_schema_untouched(self):
        """coverage: 闸 A 包内裁定 — MVP 不动网关 display_schema (不触 GWA-A5 冻结
        验收), 缺字段 char 块的防御降级归设备侧 (C 孪生 degrade TEST_CASE). 本齿
        守卫 display_schema 对 char 仍只是白名单级校验、未被本包顺手改坏."""
        path = GATEWAY_DIR / "display_schema.py"
        self.assertTrue(path.is_file(), "gate-A RED(infra): display_schema.py 消失了.")
        try:
            spec = importlib.util.spec_from_file_location("kidpal_cs_display_schema", path)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
        except Exception as e:  # noqa: BLE001
            self.fail(f"gate-A RED(infra): display_schema.py failed to import: {e!r}")
        self.assertEqual(
            set(mod.VALID_VIEWS),
            {"char", "schedule", "story_list", "story_text", "text", "home", "control"},
            "gate-A RED: display_schema.VALID_VIEWS 漂移 — view 白名单是包 G 冻结面 "
            "(GWA-A5), 本包零变更.")


class CharStrokeLiveAcceptance(unittest.TestCase):
    """Frozen live assertions; @pytest.mark.live — NOT auto-run by merge gates
    (server/acceptance/conftest.py 机械 skip 闸; RUN_LIVE=1 或 -m live 显式执行).

    Live faces: CHR-A8 (拼装块对真 makemeahanzi 数据集逐字对照) + CHR-A9 (char
    查询端到端 5 秒档). system prompt 源 = 冻结 v2 文件 (验收所测 = 生产所用).
    """

    maxDiff = None
    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _ssh(self, cmd: str, timeout: int = 300):
        proc = subprocess.run(
            [sys.executable, SSH_HELPER, cmd],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=timeout,
        )
        return proc.stdout, proc.stderr, proc.returncode

    def _ssh_ok(self, cmd: str, what: str, timeout: int = 300):
        out, err, rc = self._ssh(cmd, timeout=timeout)
        if rc != 0:
            raise AssertionError(f"{what}: remote rc={rc}. stdout={out!r} stderr={err!r}")
        return out

    def _chat_once(self, prompt: str, question: str) -> tuple[dict, float]:
        """One NON-streaming chat completion to the OpenClaw loopback via SSH.
        Request carries model/messages/stream ONLY (G/VRT 冻结纪律: 禁 max_tokens,
        不带 tools). Returns (response dict, elapsed_seconds)."""
        body = {
            "model": OPENCLAW_MODEL,
            "messages": [
                {"role": "system", "content": prompt},
                {"role": "user", "content": question},
            ],
            "stream": False,
        }
        self.assertNotIn("tools", body, "live RED(harness): fence 路径纪律被违反")
        self.assertNotIn("max_tokens", body, "live RED(harness): 禁 max_tokens 被违反")
        b64 = base64.b64encode(json.dumps(body, ensure_ascii=False).encode("utf-8")).decode()
        auth = f"-H 'Authorization: Bearer {OPENCLAW_TOKEN}' " if OPENCLAW_TOKEN else ""
        cmd = (
            f"printf '%s' {b64} | base64 -d > /tmp/cs_chat_body.json && "
            f"curl -s -m 120 {auth}-H 'Content-Type: application/json' "
            f"-X POST --data @/tmp/cs_chat_body.json {OPENCLAW_LOOPBACK}/v1/chat/completions"
        )
        t0 = time.monotonic()
        out = self._ssh_ok(cmd, "live OpenClaw char probe", timeout=180)
        elapsed = time.monotonic() - t0
        try:
            return json.loads(out.strip().splitlines()[-1]), elapsed
        except (ValueError, IndexError):
            raise AssertionError(
                f"live RED: OpenClaw reply is not JSON (probe broken). raw={out[:400]!r}")

    @staticmethod
    def _reply_content(resp: dict) -> str:
        try:
            return resp["choices"][0]["message"]["content"] or ""
        except (KeyError, IndexError, TypeError):
            return ""

    def _lookup_dataset(self, char: str) -> tuple[dict, dict]:
        """Pull the char's graphics + dictionary records from the ECS makemeahanzi
        dataset (逐字对照数据源的真数据半)."""
        script = (
            "import json,sys\n"
            "ch=sys.argv[1]; base=sys.argv[2]\n"
            "def find(name):\n"
            "    for line in open(base+'/'+name, encoding='utf-8'):\n"
            "        line=line.strip()\n"
            "        if not line: continue\n"
            "        r=json.loads(line)\n"
            "        if r.get('character')==ch: return r\n"
            "    return None\n"
            "g=find('graphics.txt'); d=find('dictionary.txt')\n"
            "print(json.dumps({'graphics':g,'dictionary':d}, ensure_ascii=False))\n"
        )
        b64 = base64.b64encode(script.encode("utf-8")).decode()
        cmd = (
            f"printf '%s' {b64} | base64 -d > /tmp/cs_lookup.py && "
            f"python3 /tmp/cs_lookup.py '{char}' '{CHAR_DATA_DIR}'"
        )
        out = self._ssh_ok(cmd, f"live makemeahanzi lookup for {char}", timeout=120)
        try:
            rec = json.loads(out.strip().splitlines()[-1])
        except (ValueError, IndexError):
            raise AssertionError(f"live RED: dataset lookup output not JSON. raw={out[:300]!r}")
        if not rec.get("graphics") or not rec.get("dictionary"):
            raise AssertionError(
                f"live RED: {char} not found in {CHAR_DATA_DIR}/graphics.txt|dictionary.txt "
                "— 数据集路径/内容不符 (CHAR_DATA_DIR env 可覆盖; 不允许跳过对照).")
        return rec["graphics"], rec["dictionary"]

    # precondition --------------------------------------------------------------
    def test_live_ssh_reachable(self):
        """coverage: live precondition — SSH to the ECS box works."""
        out, err, rc = self._ssh("echo reachable-$(hostname)")
        self.assertIn(
            "reachable", out,
            f"live precondition RED: SSH to {ECS_HOST} failed (rc={rc}). out={out!r} err={err!r}")

    # CHR-A8 (live) ----------------------------------------------------------------
    def test_a8_live_block_matches_makemeahanzi(self):
        """coverage: CHR-A8 — 对真 ECS/OpenClaw: 查字回复的 display 块 = 拼装全量
        D3, 且 strokes/medians/pinyin/radical/strokeCount 与 makemeahanzi 数据集
        逐字对照 (真数据半; 断言打在拼装产物 = 工具输出上)."""
        self.assertTrue(
            PROMPT_V2_PATH.is_file() and _read_text(PROMPT_V2_PATH).strip(),
            f"CHR-A8 live RED: frozen system prompt v2 missing/empty ({PROMPT_V2_PATH}).")
        prompt = _read_text(PROMPT_V2_PATH)
        resp, _ = self._chat_once(prompt, CHAR_LIVE_QUESTION)
        content = self._reply_content(resp)
        self.assertTrue(content.strip(), "CHR-A8 live RED: empty reply.")
        found, payload = _extract_display_block(content)
        self.assertTrue(found, "CHR-A8 live RED: reply carries no ```display block.")
        self.assertEqual(payload.get("view"), "char",
                         "CHR-A8 live RED: display view != char — 查字意图路由错.")
        data = payload.get("data") or {}
        missing = REQUIRED_DATA_KEYS - set(data)
        self.assertEqual(missing, set(),
                         f"CHR-A8 live RED: 拼装块缺渲染关键键 {missing} — 不是全量 D3 "
                         "(设备侧将防御降级, P4 不可用).")
        gfx, dic = self._lookup_dataset(str(data.get("char", "勇"))[:1] or "勇")
        self.assertEqual(
            data["strokes"], gfx["strokes"],
            "CHR-A8 live RED: 回复块 strokes != makemeahanzi 原文 — 坐标数据不是"
            "查表拼装 (LLM 复述回归, 109s 问题复发).")
        self.assertEqual(
            data["medians"], gfx["medians"],
            "CHR-A8 live RED: 回复块 medians != 数据集原文.")
        self.assertEqual(data["radical"], dic.get("radical"),
                         "CHR-A8 live RED: radical 与数据源不符.")
        # 数据集实况（2026-09-07 live 实测）: dictionary.txt 无 strokes 字段 ——
        # 笔画数真值 = len(graphics["strokes"])（笔画数 ≡ 笔画 path 条数, 数据源）。
        self.assertEqual(data["strokeCount"], len(gfx["strokes"]),
                         "CHR-A8 live RED: strokeCount 与数据源笔画 path 条数不符.")
        self.assertEqual(
            data["pinyin"], "、".join(dic.get("pinyin", [])),
            "CHR-A8 live RED: pinyin 与数据源不符 (拼装契约: 数据源音节表以「、」"
            "无损连接, 带声调原样).")

    # CHR-A9 (live) ----------------------------------------------------------------
    def test_a9_live_char_latency_5s_band(self):
        """coverage: CHR-A9 — char 查询端到端延迟达 5 秒档 (109s -> ~5s, 预算含
        网络余量 = CHAR_E2E_BUDGET_S 默认 8s): 完整块 (渲染关键六键) + 时限."""
        self.assertTrue(
            PROMPT_V2_PATH.is_file() and _read_text(PROMPT_V2_PATH).strip(),
            f"CHR-A9 live RED: frozen system prompt v2 missing ({PROMPT_V2_PATH}).")
        prompt = _read_text(PROMPT_V2_PATH)
        resp, elapsed = self._chat_once(prompt, CHAR_LIVE_QUESTION)
        found, payload = _extract_display_block(self._reply_content(resp))
        self.assertTrue(
            found and (payload or {}).get("view") == "char",
            "CHR-A9 live RED: 查字回复无 char display 块 — 链路或机制 B 失效.")
        missing = REQUIRED_DATA_KEYS - set((payload or {}).get("data") or {})
        self.assertEqual(missing, set(),
                         f"CHR-A9 live RED: 块不完整 ({missing}) — 延迟达标但数据残缺"
                         "不算过.")
        self.assertLessEqual(
            elapsed, CHAR_E2E_BUDGET_S,
            f"CHR-A9 live RED: char 端到端 {elapsed:.1f}s > 预算 {CHAR_E2E_BUDGET_S}s — "
            "未达 5 秒档 (109s 逐 token 复述回归或拼装链路退化).")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(CharStrokeOfflineAcceptance)
    if os.environ.get("CS_RUN_LIVE") == "1" or os.environ.get("RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(CharStrokeLiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
