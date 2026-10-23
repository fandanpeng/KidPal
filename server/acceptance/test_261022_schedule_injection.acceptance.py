"""Frozen acceptance tests — TaskPackage 261022-m5-n2a-schedule-mechanical-injection
（K5-E1..E6 host 面；rigor=lite；subsystem = server/deploy/engine-patch 动态 context
装配 + server/gateway/schedule.py 只读复用单源）。

上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md
T5 FROZEN v1.1（注入语义四要点 + 候选甲通道形态）+ docs/contracts/
interface-contract.md §1.5（schedule.py 唯一解析器单源红线，只读）+
delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/plan.md（含
2026-10-23 planner 注记：注入落点 = server/deploy/kidpal-agent-prompt.txt 的
{{schedule_summary}} 占位——P1 已定形该文件为动态内容源；openclaw-agents/
kidpal-dynamic-context.v1.txt 是装配壳无内容占位、零改动）。

背景（TECH-261018 第 3 层修复面）：课表回合「该读未读、凭印象编造」是模型服从度
长尾——纯 prompt 纪律把「是否读文件」留给模型裁量，是不可靠层。本包把课表摘要
机械注入每轮动态 context 块：front 经 schedule.py 单源派生 → 会话建立时内部通道
送入引擎 → 引擎拼入 {{schedule_summary}} 占位。读取动作从「模型决定」变成
「装配决定」。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；offline 可跑 / 导入期零网络 I/O）

  派生接缝（K5-E1/E2/E3/E4 承重面）：
    server/gateway/front/front_app.py 新增模块级纯函数
      build_schedule_summary(model: dict, today_iso: str) -> str
    - model = gateway.schedule.parse_schedule_md 的 B1 模型（单源产物，本函数
      不自行解析 MD——负向：函数体内零 B1 区块名常量、零表格行切分逻辑）；
    - 窗口定稿（K5-E2 本包冻结值）= 今日 + 后续 2 日（3 个日历日：today_iso ..
      today_iso+2）；输出 ≤2048 字节（UTF-8 encoded 长度）；
    - 内容 = 窗口内逐日：items_for_date(model, date) 全部事项（time/name/location
      逐字保真）+ 该日备注列文本（S-F1.3 补充规则「备注参与语音答复」——TECH-
      261018 编造证据恰含带物）+ 该日学校课程表区块的节次课程名（模型问在校课
      的唯一事实来源——TECH-261018 编造证据为周一学校课）；
    - 逐轮在场（不按意图门控）：函数签名零意图参数；front 会话建立路径无条件
      注入（结构性断言，宽松度与 T5 一致）。

  接线面（结构断言，通道实现细节归 builder）：
    - 内部通道帧类型 token = "kidpal_schedule_summary"（kidpal_speak /
      kidpal_record_context 同型先例命名），front_app.py 注入侧与
      engine-patch/connection.py 认领侧同 token；
    - connection.py 含 {{schedule_summary}} 占位填充接缝（源码含
      "schedule_summary" 字面——占位替换发生在引擎部署副本补丁内，引擎上游
      prompt_manager 不在 changeManifest）；
    - 引擎零解析（契约 §1.5）：connection.py 零 B1 区块名常量、零
      gateway.schedule import、零自建解析实现（候选乙/丙 T5 已否，负向恒绿）。

  K5-E5 = integrator 收口动作（TECH-261018 status: closed + consumed-by，debt
    结项走 integrator 收口路径，Pod 内不手改账本）——本件不落断言，仅此申报。
  K5-E6 = 既有冻结验收零新增红：本件不含对 test_m3k1_schedule_producer.
    acceptance.py / test_m3k3_voice_schedule_entry.acceptance.py 的任何断言改动
    （纪律钉行：builder/fixer 不得触碰这两件；server 全量基线由 redBaseline
    命令守——既有 268 passed 不得降）。
  K5-E4 live 段（≥20 轮真机问答零工具调用零编造、答案与 schedule.md 逐日一致）
    = live-manual 层诚实申报：挂 P5 收口轮 human 部署轮复证（launch-card
    awaiting-human:live-deploy），本件只落 host 面可闭环部分（注入块含课表
    事实时的受控问答事实在位断言）；模型行为本身 offline 不可机判，不造假绿。
  窗口外诚实语义（T5 ④）：本件 host 面钉「窗口外事实不进注入块」（注入块不可
    成为编造来源）；模型对窗口外日期的实际应答（走读文件或诚实说查不到）归
    live-manual。静态纪律半（读不到/查不到锚）已由 K1 冻结件承载（AGENTS.md
    课表问答回合行），本件零重复。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · E1 单源派生（正）    → test_k5_e1_derivation_seam_single_source_symbols → 源锚代理
    （函数体引用 schedule.py 符号 = 派生路由的源锚代理，非行为本体；行为本体
    由 E3 输出等价断言承重）
  · E1 引擎零解析（负）  → test_k5_e1_engine_face_no_second_parser → 机制级
    （connection.py 零区块名 / 零 gateway import / 零解析标记）
  · E1 占位填充接缝      → test_k5_e1_placeholder_fill_seam_in_engine_patch → 源锚代理
  · E2 有界（窗口/字节） → test_k5_e2_summary_bounded_3day_2kb → 行为级（真跑派生）
  · E2 无意图门控        → test_k5_e2_no_intent_gating_seam → 源锚代理
  · E2 会话建立时注入    → test_k5_e2_session_establishment_injection_wiring → 源锚代理
  · E3 同日派生等价      → test_k5_e3_same_day_derivation_equivalence → 行为级（真跑派生）
  · E3 窗口外不进注入块  → test_k5_e3_out_of_window_facts_absent → 行为级（真跑派生）
  · E4 受控 ≥20 轮事实在位 → test_k5_e4_twenty_round_controlled_qa_facts_in_summary
    → 行为级（真跑派生；模型应答行为归 live-manual，显式申报不 offline 钉）

缺口（强度低于契约要求处，逐条）：
  · 接线三断言（占位填充 / 无门控 / 会话建立注入）均为源锚代理——「源码含 token /
    调用点在场」≠「运行时逐轮无条件送达模型」；运行时行为归 K5-E4 live 段
    （真机 ≥20 轮收口轮目击）。行为等价假设显式化：① 源码注入调用点在场 ≈ 运行
    时逐轮在场（无条件性由签名零意图参数 + live 复证兜底，offline 无运行时引擎
    可证）；② connection.py 源码含占位字面 ≈ 占位真被替换（引擎渲染链运行时序
    offline 不可证）；③ front_app 派生函数体引用单源符号 ≈ 派生真经单源（别名
    引用形态已容忍）。
  · 输出渲染格式零钉（行序 / 标题措辞 / 分隔符不判）——等价断言只钉事实 token
    在位；时间/日期须以 parse_schedule_md 产出的原形（HH:MM / YYYY-MM-DD）出现
    （渲染须逐字保真单源字符串，不重排时刻格式）。
  · E4 的「答案与 kidpal/schedule.md 逐日一致」在本件降为「注入块事实在位 +
    问答所需事实均在注入块内」——「模型答对」本体 = live 段。

fixture 自含性声明：全部断言自含 fixture（本文件内嵌 B1 MD 字符串，三区块齐备
  + 备注列）+ 直读 repo 内制品（front_app.py / connection.py /
  kidpal-agent-prompt.txt）；日期真值 = 固定锚日 2026-10-23（周五）经函数入参
  传入，零 datetime.now 依赖、零外部资产、零环境前提（front_app 离线装载走
  fastapi stub + env 钉位，test_261013_qq_schedule_write_closure 同款先例）。
行为 seam 驱动方对账：派生函数（front 侧）驱动方 = 本件 host 断言；通道帧
  front→engine 的运行时驱动方 = 设备 WS 建连（offline 无驱动方 = 已申报盲区，
  live 复证归 K5-E4 live 段）；模型应答行为驱动方 = 真机轮（live-manual）。
host/真机近似假设：host 面证明「注入块 == 单源同日派生且含全部问答所需事实」，
  是真机行为的差集覆盖（真机另证「模型确实依据注入块作答」）——差集已显式申报。

Run protocol（两点文件名须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-23，builder 未动工）：
  设计态红（真实缺失行为）= 派生接缝不在世（front_app.build_schedule_summary
  缺失）→ E1 派生符号齿 / E2 有界·无门控·接线齿 / E3 等价·窗口外齿 / E4 受控
  轮齿全部红（同因红，映射 = 接缝缺失这一个缺失行为；E3/E4 红不另指内容缺失）；
  E1 占位填充接缝齿红（connection.py 零 schedule_summary 字面）。
  must-stay-green = E1 引擎零解析负向齿（connection.py 现状零解析实现）、
  kidpal-agent-prompt.txt 占位锚（P1 已落）。
  既有套件（冻结前实跑）= 268 passed / 4 deselected / 33 subtests passed 全绿；
  本件只新增 failure（设计态红）、零新增 error。
  冻结时点实跑（2026-10-23，本树）：本件 8 failed / 2 passed（must-stay-green
  = 引擎零解析负向 + E5/E6 申报自检）；全量 = 270 passed（268 既有 + 2 新增
  must-stay-green）/ 8 failed（全为本件设计态红，成因单一 = 派生接缝与接线面
  缺失）/ 4 deselected / 33 subtests，零 error（§3 冻结即冒烟 + §4 C 类实编
  判据达成：收集零 error、执行通道 error 为零）。
  转绿路径已推演（§6b 断言设计透镜）：以符合冻结契约的实现模拟（派生函数按
  items_for_date + 学校节次 + 备注逐字渲染、恰 3 日窗）——行为面断言（E2 有界 /
  E3 等价·窗口外 / E4 受控轮 / E5E6 申报）全绿，仅 4 条源锚接线断言须真实接线
  后转绿；变异自检（窗口越 4 日 / 内容抽空）分别新爆 6 / 7 条红，断言敏感性
  在案。
"""

from __future__ import annotations

import ast
import importlib.util
import os
import re
import sys
import tempfile
import unittest
from datetime import date, timedelta
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import pytest  # noqa: F401  (markers only; standalone runner also works)
except ImportError:  # pragma: no cover
    pytest = None

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"
CONNECTION_PY = SERVER / "deploy" / "engine-patch" / "connection.py"
AGENT_PROMPT_TXT = SERVER / "deploy" / "kidpal-agent-prompt.txt"

# --------------------------------------------------------------------------- #
# 窗口 / 有界定稿值（K5-E2 本包冻结；变更 = 语义放宽，须走 tighten-only 纪律反审）
# --------------------------------------------------------------------------- #

K5_WINDOW_DAYS = 3          # 今日 + 后续 2 日
K5_SUMMARY_MAX_BYTES = 2048  # ≤2KB（UTF-8）

K5_TODAY = "2026-10-23"  # 固定锚日（周五）；窗口 = 10-23(五) / 10-24(六) / 10-25(日)
K5_TODAY_D = date(2026, 10, 23)
K5_WINDOW_DATES = [(K5_TODAY_D + timedelta(days=i)).isoformat()
                   for i in range(K5_WINDOW_DAYS)]
K5_OUT_DATES = [(K5_TODAY_D + timedelta(days=i)).isoformat()
                for i in range(K5_WINDOW_DAYS, K5_WINDOW_DAYS + 3)]

# 内部通道帧类型 token（kidpal_speak / kidpal_record_context 同型先例命名）。
K5_FRAME_TYPE = "kidpal_schedule_summary"

# B1 区块名（契约 §二；第二解析器负向锚——单源解析器 schedule.py 的私有定位常量）。
K5_B1_BLOCK_NAMES = ("每周固定课表", "一次性安排", "学校课程表")


# --------------------------------------------------------------------------- #
# fixture：B1 课表 MD（三区块齐备 + 备注列；日期以 K5_TODAY 为锚全受控）
# --------------------------------------------------------------------------- #

K5_FIXTURE_MD = """---
child: 小伴
timezone: Asia/Shanghai
---

# 每周固定课表

| 星期 | 时间 | 课程 | 地点 | 备注 |
| --- | --- | --- | --- | --- |
| 周五 | 16:30-17:30 | 美术 | 画室 | 带水彩笔 |
| 周五 | 19:00-20:00 | 编程 | 线上 | 记得带电脑 |
| 周六 | 10:00-11:30 | 足球 | 操场 |  |
| 周日 | 15:00-16:00 | 钢琴 | 琴行 | 带琴谱 |
| 周一 | 17:00-18:00 | 围棋 | 棋院 | 窗口外事项 |

# 一次性安排

| 日期 | 时间 | 事项 | 地点 | 备注 |
| --- | --- | --- | --- | --- |
| 2026-10-20 | 10:00-11:00 | 手工课 | 教室 | 已过期事项 |
| 2026-10-23 | 18:30-19:30 | 牙医复诊 | 门诊楼 | 带上次的片子 |
| 2026-10-24 | 09:00-10:00 | 秋游集合 | 学校门口 | 带水壶 |
| 2026-10-25 | 14:00-15:00 | 科学实验课 | 科技馆 | 带预约码 |
| 2026-10-26 | 16:00-17:00 | 窗口外科博课 | 展馆 | 窗口外事项 |

# 学校课程表

| 星期 | 第1节 | 第2节 | 第3节 | 第4节 | 第5节 | 第6节 |
| --- | --- | --- | --- | --- | --- | --- |
| 周五 | 语文 | 数学 | 英语 | 体育 | 音乐 | 班会 |
| 周一 | 窗口外科学 | 窗口外语文 | 窗口外英语 | 窗口外体育 | 窗口外音乐 | 窗口外班会 |
"""

# 窗口外 / 过期事实（负向唯一 token；不得出现在注入块——注入块不可成为编造来源）。
K5_OUT_FACTS = ("围棋", "手工课", "窗口外科博课", "窗口外科学", "已过期事项")

# --------------------------------------------------------------------------- #
# 派生接缝装载（front_app.py 离线装载：fastapi stub + env 钉位，261013 先例同款；
# 跨冻结件零 import——本装载器自含复制，不 import 既有冻结件）
# --------------------------------------------------------------------------- #

_front_stub_done = False
_front_module_cache: dict = {}


def _ensure_fastapi_stub() -> None:
    global _front_stub_done
    if _front_stub_done:
        return
    try:
        import fastapi  # noqa: F401
        _front_stub_done = True
        return  # 真 fastapi 在位更佳
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    if "fastapi" not in sys.modules:
        stub = _PermissiveModule("fastapi")
        stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
        stub.HTTPException = type("HTTPException", (Exception,), {})
        sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")
    _front_stub_done = True


def _load_front_module():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)。

    装载坑（261013 先例）：SCHEDULE_MD / ALERTS_DIR 是模块级常量、import 时读 env
    ——装载前钉临时默认位，装载后还原（模块常量已捕获安全值）。"""
    if "mod" in _front_module_cache:
        cached = _front_module_cache["mod"]
        return (cached, None) if cached is not None else (None, _front_module_cache["err"])
    if not FRONT_APP.is_file():
        err = "server/gateway/front/front_app.py missing —— 派生接缝承载面不在世。"
        _front_module_cache.update(mod=None, err=err)
        return None, err
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    safe_md = Path(tempfile.gettempdir()) / "kidpal_absent_schedule_k5.md"
    safe_alerts = Path(tempfile.gettempdir()) / "kidpal_absent_alerts_k5"
    keys = ("KIDPAL_SCHEDULE_MD", "KIDPAL_ALERTS_DIR")
    saved = {k: os.environ.get(k) for k in keys}
    os.environ["KIDPAL_SCHEDULE_MD"] = str(safe_md)
    os.environ["KIDPAL_ALERTS_DIR"] = str(safe_alerts)
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k5", FRONT_APP)
        if spec is None or spec.loader is None:
            raise RuntimeError("front_app.py spec not loadable")
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        err = None
    except Exception as e:  # noqa: BLE001 —— import error 是 RED，不是 crash
        mod, err = None, (f"front_app.py failed to import: {e!r}。冻结口径：该模块必须"
                          "离线可装载（fastapi 缺席走本地 stub，261013 先例）。")
    finally:
        for k, v in saved.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v
    _front_module_cache.update(mod=mod, err=err)
    return mod, err


def _front_seam(testcase, tag: str):
    """取派生接缝函数（缺席 = 设计态 RED，非 error）。"""
    mod, err = _load_front_module()
    if mod is None:
        testcase.fail(f"{tag} RED: {err}")
    fn = getattr(mod, "build_schedule_summary", None)
    if fn is None or not callable(fn):
        testcase.fail(
            f"{tag} RED: front_app.build_schedule_summary 不在世——派生接缝缺失"
            "（FROZEN OFFLINE CONTRACT：模块级纯函数 "
            "build_schedule_summary(model: dict, today_iso: str) -> str）。")
    return mod, fn


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _fixture_model():
    """fixture MD -> B1 模型（经唯一解析器 parse_schedule_md——单源反证的预期侧）。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    from gateway import schedule as gw_schedule
    return gw_schedule, gw_schedule.parse_schedule_md(K5_FIXTURE_MD)


def _note_lookup(model: dict, date_iso: str, time_hhmm: str, name: str) -> str:
    """从已解析 B1 模型回查事项备注（weekly 按 weekday+start+name；onetime 按
    date+start+name）——items_for_date 输出不携备注（契约 item 形状三键），备注
    在位断言的预期侧由此同源取得，绝不硬编码第二份事实。"""
    wd = date.fromisoformat(date_iso).weekday() + 1
    for row in model.get("weekly", []):
        if row["weekday"] == wd and row["start"] == time_hhmm and row["name"] == name:
            return row["note"]
    for row in model.get("onetime", []):
        if row["date"] == date_iso and row["start"] == time_hhmm and row["name"] == name:
            return row["note"]
    return ""


def _expected_date_facts_with(gw, model: dict, date_iso: str) -> set:
    facts: set = set()
    for it in gw.items_for_date(model, date_iso):
        facts.add(it["time"])
        facts.add(it["name"])
        if it["location"]:
            facts.add(it["location"])
        note = _note_lookup(model, date_iso, it["time"], it["name"])
        if note:
            facts.add(note)
    wd = date.fromisoformat(date_iso).weekday() + 1
    for row in model.get("school", []):
        if row["weekday"] == wd:
            for period in row["periods"]:
                if period and period.strip():
                    facts.add(period.strip())
    return facts


# --------------------------------------------------------------------------- #
# K5-E1 · 单源（契约 §1.5 红线）
# --------------------------------------------------------------------------- #


class K5ScheduleInjectionAcceptance(unittest.TestCase):

    maxDiff = None

    def test_k5_e1_derivation_seam_single_source_symbols(self):
        """coverage: K5-E1 — 派生接缝函数体引用 schedule.py 单源符号
        （parse_schedule_md / items_for_date / group_upcoming 之一，别名引用形态
        容忍）；函数体零 B1 区块名常量、零表格行切分逻辑（不自行解析 MD——
        模型必须来自 parse_schedule_md 产物）。"""
        tag = "K5-E1"
        mod, fn = _front_seam(self, tag)
        src = _read_text(FRONT_APP)
        tree = ast.parse(src)
        fn_node = None
        for node in ast.walk(tree):
            if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)) \
                    and node.name == "build_schedule_summary":
                fn_node = node
                break
        self.assertIsNotNone(
            fn_node,
            f"{tag} RED: front_app 源码无 build_schedule_summary 函数定义"
            "（接缝须为模块级函数，属性挂载不算）。")
        body_src = ast.get_source_segment(src, fn_node) or ""
        symbol_names = {"parse_schedule_md", "items_for_date", "group_upcoming"}
        refs = set()
        for node in ast.walk(fn_node):
            if isinstance(node, ast.Attribute) and node.attr in symbol_names:
                refs.add(node.attr)
            elif isinstance(node, ast.Name) and node.id in symbol_names:
                refs.add(node.id)
        self.assertTrue(
            refs,
            f"{tag} RED: build_schedule_summary 函数体零 schedule.py 单源符号引用"
            f"（{sorted(symbol_names)} 任一须在场，gw_schedule.X 或 from-import 别名"
            "皆可）——注入内容产生路径未路由单源解析器（契约 §1.5）。")
        for node in ast.walk(fn_node):
            if isinstance(node, ast.Constant) and isinstance(node.value, str):
                for block in K5_B1_BLOCK_NAMES:
                    self.assertNotIn(
                        block, node.value,
                        f"{tag} RED: 派生函数体含 B1 区块名常量 {block!r}——函数自"
                        "行定位/解析 MD 区块 = 第二解析器萌芽（模型应由 "
                        "parse_schedule_md 产物入参传入）。")
        for marker in ('split("|")', "split('|')", 'startswith("|")',
                       "startswith('|')"):
            self.assertNotIn(
                marker, body_src,
                f"{tag} RED: 派生函数体含表格行切分标记 {marker!r}——自建 MD 解析"
                "逻辑（契约 §1.5 单源红线）。")

    def test_k5_e1_engine_face_no_second_parser(self):
        """coverage: K5-E1 — 引擎部署副本补丁零课表解析实现（契约 §1.5 / T5 候选
        乙丙否决的负向恒绿面）：connection.py 零 B1 区块名常量、零 gateway/schedule
        模块 import、零自建解析函数标记。本件不 import connection.py（opuslib 等
        引擎依赖 offline 不可装载）——纯源码/AST 判。"""
        tag = "K5-E1"
        src = _read_text(CONNECTION_PY)
        self.assertTrue(
            src.strip(),
            f"{tag} RED: {CONNECTION_PY} 不在场/为空——引擎部署副本补丁面缺失。")
        offenders = [b for b in K5_B1_BLOCK_NAMES if b in src]
        self.assertEqual(
            offenders, [],
            f"{tag} RED: connection.py 出现 B1 区块名常量 {offenders}——引擎自解析"
            "课表 MD（候选丙，T5 直接否：造第二解析器，违反契约 §1.5）。")
        for pat in (r"^\s*import\s+schedule\b", r"^\s*from\s+gateway\b",
                    r"^\s*import\s+gateway\b", r"^\s*from\s+schedule\b"):
            m = re.search(pat, src, re.MULTILINE)
            self.assertIsNone(
                m,
                f"{tag} RED: connection.py 含 {pat!r} import——引擎绑定 front 单源"
                "模块（候选乙，T5 否决：跨容器双部署副本漂移面）。")
        for marker in ("parse_schedule", "schedule_summary_md", "_parse_b1"):
            self.assertNotIn(
                marker, src,
                f"{tag} RED: connection.py 含解析函数标记 {marker!r}——引擎侧自建"
                "解析实现痕迹（契约 §1.5）。")

    def test_k5_e1_placeholder_fill_seam_in_engine_patch(self):
        """coverage: K5-E1 — {{schedule_summary}} 占位填充接缝在引擎部署副本补丁
        内：connection.py 含 "schedule_summary" 字面（填充侧）+ 内部通道帧 token
        认领侧。源锚代理（占位真被替换的运行时序 offline 不可证——见头注缺口
        申报②）。模板占位本体（kidpal-agent-prompt.txt）为 must-stay-green 前置
        （P1 已落）。"""
        tag = "K5-E1"
        self.assertIn(
            "{{schedule_summary}}", _read_text(AGENT_PROMPT_TXT),
            f"{tag} RED: kidpal-agent-prompt.txt 丢失 {{schedule_summary}} 占位——"
            "P1 动态内容源被回改（must-stay-green 前置；本包零改动该文件）。")
        src = _read_text(CONNECTION_PY)
        self.assertTrue(src.strip(), f"{tag} RED: connection.py 不在场/为空。")
        self.assertIn(
            "schedule_summary", src,
            f"{tag} RED: connection.py 零 schedule_summary 字面——{{schedule_summary}}"
            " 占位填充接缝不在引擎部署副本补丁内（front 注入的课表摘要无消费点，"
            "动态块课表摘要恒空 = 机械注入未落地）。")
        self.assertIn(
            K5_FRAME_TYPE, src,
            f"{tag} RED: connection.py 零内部通道帧 token {K5_FRAME_TYPE!r} 认领——"
            "front 注入帧无人消费（kidpal_speak / kidpal_record_context 同型认领"
            "先例）。")

    # ------------------------------------------------------------------ #
    # K5-E2 · 注入语义（逐轮在场 / 有界 / 会话建立时注入）
    # ------------------------------------------------------------------ #

    def test_k5_e2_summary_bounded_3day_2kb(self):
        """coverage: K5-E2 — 注入有界（本包定稿值）：窗口恰 3 日历日（今日 + 后续
        2 日——输出内出现的全部 ISO 日期 ∈ 窗口集）、输出 ≤2048 UTF-8 字节、
        非空。行为级（真跑派生函数）。"""
        tag = "K5-E2"
        mod, fn = _front_seam(self, tag)
        gw, model = _fixture_model()
        summary = fn(model, K5_TODAY)
        self.assertIsInstance(
            summary, str,
            f"{tag} RED: build_schedule_summary 返回 {type(summary).__name__}——"
            "须为字符串（注入块文本）。")
        self.assertTrue(
            summary.strip(),
            f"{tag} RED: 注入块为空——fixture 含窗口内事项，空块 = 机械注入未产内容。")
        iso_dates = set(re.findall(r"\d{4}-\d{2}-\d{2}", summary))
        self.assertTrue(
            iso_dates,
            f"{tag} RED: 注入块零 ISO 日期——窗口日期头缺失（逐日结构不可核；"
            "渲染须以 parse 侧原形 YYYY-MM-DD 携带日期，见头注渲染假设）。")
        self.assertEqual(
            iso_dates, set(K5_WINDOW_DATES),
            f"{tag} RED: 注入块日期集 {sorted(iso_dates)} ≠ 窗口集 "
            f"{K5_WINDOW_DATES}——窗口越界（≤3 日定稿值）或窗口日缺失。")
        size = len(summary.encode("utf-8"))
        self.assertLessEqual(
            size, K5_SUMMARY_MAX_BYTES,
            f"{tag} RED: 注入块 {size} 字节 > {K5_SUMMARY_MAX_BYTES}——有界定稿值"
            "（≤2KB）被突破（fixture 规模远低于真实课表上界，突破即实现无界）。")

    def test_k5_e2_no_intent_gating_seam(self):
        """coverage: K5-E2 — 逐轮在场（不按意图门控）：派生接缝签名零意图参数
        （intent/query/question/user_say/is_schedule 系）；front_app 内派生接缝
        存在 ≥1 个定义外调用点（注入侧真实调用）。源锚代理（运行时无条件性归
        live 复证，见头注缺口申报①）。"""
        tag = "K5-E2"
        mod, fn = _front_seam(self, tag)
        import inspect
        params = set(inspect.signature(fn).parameters)
        self.assertTrue(
            {"model", "today_iso"} <= params,
            f"{tag} RED: build_schedule_summary 参数 {sorted(params)} 缺 model/"
            "today_iso——冻结签名偏移（FROZEN OFFLINE CONTRACT）。")
        gated = {"intent", "query", "question", "user_say", "is_schedule",
                 "is_schedule_round", "should_inject", "only_if_asked"}
        hit = sorted(gated & params)
        self.assertEqual(
            hit, [],
            f"{tag} RED: 派生接缝签名含意图门控参数 {hit}——是否课表回合的判断"
            "本身就是模型裁量，机械注入不得按意图门控（T5 注入语义①）。")
        src = _read_text(FRONT_APP)
        self.assertGreaterEqual(
            src.count("build_schedule_summary"), 2,
            f"{tag} RED: front_app 零定义外调用点（仅定义无调用 = 注入侧未接线，"
            "逐轮在场无承载）。")

    def test_k5_e2_session_establishment_injection_wiring(self):
        """coverage: K5-E2 — 会话建立时注入（T5 ③）：front_app 会话建立路径上存
        在派生接缝调用点，且调用点所在函数处于引擎腿注入上下文（引用 upstream/
        websocket 标识——kidpal_speak / kidpal_record_context 同型通道先例）；
        内部通道帧 token 在 front 侧在场。源锚代理（通道实现细节归 builder，
        宽松度与 T5 一致；运行时时序归 live 复证）。"""
        tag = "K5-E2"
        mod, fn = _front_seam(self, tag)
        src = _read_text(FRONT_APP)
        self.assertIn(
            K5_FRAME_TYPE, src,
            f"{tag} RED: front_app 零内部通道帧 token {K5_FRAME_TYPE!r}——注入帧"
            "未定义（会话建立时注入无通道承载）。")
        tree = ast.parse(src)
        lines = src.splitlines()
        call_sites = []
        for node in ast.walk(tree):
            if not isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
                continue
            for sub in ast.walk(node):
                if (isinstance(sub, ast.Call)
                        and isinstance(sub.func, (ast.Name, ast.Attribute))
                        and getattr(sub.func, "id", getattr(sub.func, "attr", None))
                        == "build_schedule_summary"):
                    seg = "\n".join(lines[node.lineno - 1:node.end_lineno])
                    call_sites.append((node.name, seg))
                    break
        self.assertTrue(
            call_sites,
            f"{tag} RED: front_app 零定义外 build_schedule_summary 调用点——派生"
            "结果未进入任何注入路径。")
        wired = [name for name, seg in call_sites
                 if "upstream" in seg or "websocket" in seg or "send" in seg]
        self.assertTrue(
            wired,
            f"{tag} RED: 派生接缝调用点所在函数 {[(n for n, _ in call_sites)]} 无"
            "引擎腿注入上下文（upstream/websocket/send 皆缺）——会话建立路径上的"
            "注入接线不成立（帧定义了但没人发）。")

    # ------------------------------------------------------------------ #
    # K5-E3 · 单源反证（核心）
    # ------------------------------------------------------------------ #

    def test_k5_e3_same_day_derivation_equivalence(self):
        """coverage: K5-E3 — 单源反证（受控日期集）：fixture schedule.md 经
        parse_schedule_md / items_for_date 实算窗口内逐日预期事实集
        （事项 time/name/location + 源行备注 + 学校课程表节次），断言注入产物
        逐项覆盖（同日派生等价——注入内容 == 解析器输出，绝不另算一套）。"""
        tag = "K5-E3"
        mod, fn = _front_seam(self, tag)
        gw, model = _fixture_model()
        summary = fn(model, K5_TODAY)
        for date_iso in K5_WINDOW_DATES:
            expected = _expected_date_facts_with(gw, model, date_iso)
            self.assertTrue(
                expected,
                f"{tag} fixture 自检：{date_iso} 预期事实集为空——fixture 与锚日"
                "星期错位（测试自身缺陷，非实现红）。")
            missing = sorted(f for f in expected if f not in summary)
            self.assertEqual(
                missing, [],
                f"{tag} RED: {date_iso} 注入块缺预期事实 {missing}——与 "
                "items_for_date / 学校课程表 同日派生不等价（单源反证失败：注入"
                "内容 ≠ 解析器输出）。")

    def test_k5_e3_out_of_window_facts_absent(self):
        """coverage: K5-E3 — 窗口外行为断言：窗口外日期（今日+3 起）与过期一次性
        安排的事实 token 零出现在注入块（注入块不可成为编造来源；窗口外诚实
        语义的 host 半——模型应答行为归 live-manual）。"""
        tag = "K5-E3"
        mod, fn = _front_seam(self, tag)
        gw, model = _fixture_model()
        summary = fn(model, K5_TODAY)
        # fixture 自检：负向 token 确在窗口外日期的解析产物中（负向断言非空转）。
        out_dates_with_facts = [d for d in K5_OUT_DATES
                                if _expected_date_facts_with(gw, model, d)
                                & set(K5_OUT_FACTS)]
        self.assertTrue(
            out_dates_with_facts,
            f"{tag} fixture 自检：窗口外日期 {K5_OUT_DATES} 无一携带负向 token——"
            "fixture 与锚日星期错位（测试自身缺陷）。")
        leaked = sorted(f for f in K5_OUT_FACTS if f in summary)
        self.assertEqual(
            leaked, [],
            f"{tag} RED: 注入块泄漏窗口外/过期事实 {leaked}——注入块成为编造来"
            "源（T5 注入语义②：超窗日期仍走读文件或诚实说查不到，禁凭印象）。")

    # ------------------------------------------------------------------ #
    # K5-E4 · 受控 ≥20 轮（host 面可闭环部分）
    # ------------------------------------------------------------------ #

    # 受控问答 fixture：question = 孩子问句（live 段原样可用）；date = 事实锚日
    # （窗口内）或 None（窗口外负向轮）；expect = 该轮答案所需事实 token；absent
    # = 该轮答案不得依赖的事实（负向轮）。expect 逐轮经单源派生事实集核对
    # （fixture 漂移守卫：问答所需事实必须确属该日解析产物，问答映射不造假）。
    K5_ROUNDS = [
        # —— 窗口内正证轮（答案事实必须在注入块内）——
        {"q": "今天有什么课？", "date": K5_TODAY,
         "expect": ["美术", "编程", "语文", "数学", "英语", "体育", "音乐",
                     "班会", "牙医复诊", "带上次的片子"]},
        {"q": "今天美术课要带什么？", "date": K5_TODAY, "expect": ["带水彩笔"]},
        {"q": "今天晚上编程课在哪上？", "date": K5_TODAY,
         "expect": ["19:00", "编程", "线上"]},
        {"q": "今天学校第三节是什么课？", "date": K5_TODAY, "expect": ["英语"]},
        {"q": "今天放学后有什么安排？", "date": K5_TODAY,
         "expect": ["18:30", "牙医复诊", "门诊楼"]},
        {"q": "明天有什么课？", "date": K5_WINDOW_DATES[1],
         "expect": ["足球", "秋游集合"]},
        {"q": "明天几点集合？", "date": K5_WINDOW_DATES[1],
         "expect": ["09:00", "秋游集合"]},
        {"q": "明天秋游要带什么？", "date": K5_WINDOW_DATES[1], "expect": ["带水壶"]},
        {"q": "明天足球课在哪？", "date": K5_WINDOW_DATES[1],
         "expect": ["10:00", "足球", "操场"]},
        {"q": "后天有什么课？", "date": K5_WINDOW_DATES[2],
         "expect": ["钢琴", "科学实验课"]},
        {"q": "后天要带什么东西？", "date": K5_WINDOW_DATES[2],
         "expect": ["带琴谱", "带预约码"]},
        {"q": "后天科学实验课几点开始？", "date": K5_WINDOW_DATES[2],
         "expect": ["14:00", "科学实验课"]},
        {"q": "周五美术课在哪儿上？", "date": K5_TODAY, "expect": ["画室"]},
        {"q": "今天语文课的下一节是什么？", "date": K5_TODAY, "expect": ["数学"]},
        {"q": "今天编程课要带什么？", "date": K5_TODAY, "expect": ["记得带电脑"]},
        {"q": "今天最后一节课是什么？", "date": K5_TODAY, "expect": ["班会"]},
        {"q": "后天钢琴课在哪？", "date": K5_WINDOW_DATES[2], "expect": ["琴行"]},
        {"q": "这周末有游泳课吗？", "date": K5_WINDOW_DATES[1],
         "expect": [], "absent": ["游泳"]},
        # —— 窗口外负证轮（事实不得在注入块内；诚实应答归 live-manual）——
        {"q": "大后天有什么课？", "date": None, "expect": [],
         "absent": ["围棋", "窗口外科博课"]},
        {"q": "下周一学校上什么课？", "date": None, "expect": [],
         "absent": ["窗口外科学"]},
        {"q": "上周二的手工课是什么？", "date": None, "expect": [],
         "absent": ["手工课"]},
    ]

    def test_k5_e4_twenty_round_controlled_qa_facts_in_summary(self):
        """coverage: K5-E4 — 受控 ≥20 轮课表提问（host 面可闭环部分）：每轮断言
        答案所需事实全部在注入块内（零工具调用也有料可答）；窗口外轮断言事实
        零泄漏；≥20 轮规模钉行。fixture 漂移守卫：窗口内每轮 expect ⊆ 该日单源
        派生事实集（问答映射不造假）。模型应答行为本身（真的答对、零工具调用、
        零编造）= live-manual 段，挂 P5 收口轮 human 部署轮复证，本件不造假绿。"""
        tag = "K5-E4"
        mod, fn = _front_seam(self, tag)
        gw, model = _fixture_model()
        rounds = self.K5_ROUNDS
        self.assertGreaterEqual(
            len(rounds), 20,
            f"{tag} fixture 自检：受控轮次 {len(rounds)} < 20——规模钉行失守"
            "（测试自身缺陷）。")
        summary = fn(model, K5_TODAY)
        derived = {d: _expected_date_facts_with(gw, model, d)
                   for d in K5_WINDOW_DATES}
        for idx, rd in enumerate(rounds, 1):
            q, d, expect = rd["q"], rd.get("date"), rd.get("expect", [])
            if d is not None:
                # fixture 漂移守卫：expect 必须确属该日解析产物。
                stray = sorted(f for f in expect if f not in derived[d])
                self.assertEqual(
                    stray, [],
                    f"{tag} fixture 自检：第 {idx} 轮 {q!r} expect {stray} 不在"
                    f"{d} 单源派生事实集——问答映射与 fixture 漂移（测试自身缺陷）。")
            missing = sorted(f for f in expect if f not in summary)
            self.assertEqual(
                missing, [],
                f"{tag} RED: 第 {idx} 轮 {q!r} 答案所需事实 {missing} 不在注入块"
                "内——零工具调用时该轮无料可答（机械注入覆盖缺口，K5-E4 host 半）。")
            for absent in rd.get("absent", []):
                self.assertNotIn(
                    absent, summary,
                    f"{tag} RED: 第 {idx} 轮 {q!r} 的不得依赖事实 {absent!r} 出现"
                    "在注入块——注入块成了编造/误答来源（负向轮失守）。")

    # ------------------------------------------------------------------ #
    # K5-E5 / K5-E6 · 申报面（无断言承载，纪律钉行见文件头注）
    # ------------------------------------------------------------------ #

    def test_k5_e5_e6_declarations_exist(self):
        """coverage: K5-E5, K5-E6 — 申报面自检（机制级，防申报段被静默删除）：
        文件头注含 K5-E5 integrator 收口申报（TECH-261018 结项不走 Pod 内断言）
        与 K5-E6 零新增红纪律钉行（不触碰 m3k1 / m3k3 冻结件；既有基线由
        redBaseline 命令守）。本断言只守申报在位，不替 integrator 结项。"""
        tag = "K5-E5/E6"
        src = _read_text(Path(__file__))
        self.assertIn(
            "K5-E5", src,
            f"{tag} RED: 头注 K5-E5 申报段被删——integrator 收口申报失位。")
        self.assertIn(
            "K5-E6", src,
            f"{tag} RED: 头注 K5-E6 纪律钉行被删——零新增红申报失位。")
        self.assertIn(
            "test_m3k1_schedule_producer.acceptance.py", src,
            f"{tag} RED: 头注零触碰钉行丢失 m3k1 冻结件名——K5-E6 纪律对象失指。")
        self.assertIn(
            "test_m3k3_voice_schedule_entry.acceptance.py", src,
            f"{tag} RED: 头注零触碰钉行丢失 m3k3 冻结件名——K5-E6 纪律对象失指。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
