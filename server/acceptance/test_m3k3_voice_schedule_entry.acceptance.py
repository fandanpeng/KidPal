"""Frozen acceptance tests — TaskPackage 260921-m3-k3-voice-schedule-entry-live
(subsystem: server; workstream m3-schedule; mergeOrder 35; rigor full; riskTier medium).

Story: S-F1.3 · 语音问课表（AC1–AC4）+ S-F1.5 · 家长录入课表（AC1–AC2 live 复证）
+ S-F1.2 · 课表页看「一周总览」（真机半）+ S-F1.1 · 常显首页看「接下来干嘛」（三触发
live）——prompt v6 问课表口径 + 「帮我显示课表」意图链 offline 齿 + 真机确认轮制品面。
技术方案 = delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/plan.md；
契约形状权威 = docs/contracts/interface-contract.md §1.3②/§1.3⑥（2026-09-21 契约门
HUMAN_APPROVED，K1 CONTRACT-CHANGE-PACKET §5：Q2 甲案薄载荷 / Q4 front 机械算 /
Q5 收紧）。

Scope of record (taskpackage acceptance M3K3-A1..A3):

  M3K3-A1 prompt v6 问课表口径齿——
    ① v6 新文件落盘锚齿（openclaw-system-prompt.v6.md，版本化演进）；
    ② 课表 producer 段口径齿（答复走问答框文本 AC1、答复含备注内容 AC4、
       「帮我显示课表」意图 → schedule 薄载荷 data:{} AC2、网格数据 LLM 不生成
       ——意图信号化，薄载荷形态按 K1 契约门 Q2 甲案）；
    ③ 接线齿：OPENCLAW_SYSTEM_PROMPT_CARRIER 声明切换 + apply-xiaozhi-overlay.sh
       渲染引用改 v6 + provider.config.yaml 声明面同步（接线不变式：渲染面 ==
       声明面 == 被测面，ADR-001；**版本无关解析两面得版本再断言**，ARQ-260908
       versioned-guard-version-literal-coupling 纪律——本件接线类齿零版本字面）；
    ④ v5 基座逐字保留可 diff（diff 界面齿：v6 相对 v5 只动课表段 + 版本沿革前言）；
    ⑤ v5 历史文件零回改（E2 铁律，sha256 落地守卫）。

  M3K3-A2 「帮我显示课表」意图齿——offline 机械验证链路各环节：
    意图信号（= M3K3-A1 的 v6 prompt 措辞齿承载，此处不重复）→ front 截获展开
    （frozen 纯函数 expand_schedule_intent_payload 单元齿）→ view=schedule 下发
    （展开载荷过真实 validate_display 门 + wire 形状 data 字符串化往返）→ 设备切页
    （K2 冻结件 firmware/main/test/schedule_grid.acceptance.c 已钉 view 路由 +
    渲染 + 薄载荷泄漏 -1 降级，本件零重复）。
    **LLM 意图判定本身不可 offline 机判——归 M3K3-A3 真机确认轮目击，不造假绿。**

  M3K3-A3 live 真机确认轮（human 持板）——offline 只钉可机判的制品/清单结构面：
    设备轮记录落地齿 + 清单覆盖条目对账齿（guard-class: landing）。真机目击项
    （S-F1.2 AC1–AC4 / S-F1.3 AC1–AC4 / S-F1.5 AC1–AC2 / 决策 26 三触发 /
    NFR-READ-1 目测）**不 offline 钉**；K1/K2 deferred 真机半（M3K1-A5 / M3K2-A7）
    的闭环对账锚也在记录对账齿内。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement）:

  1) server/deploy/openclaw-system-prompt.v6.md 新文件落盘（v6 槽位 2026-09-21 核空，
     ARQ-260908 槽位预约规则）。v6 = v5 + 本包课表段重写，diff 界面冻结为：
       可改区（v6 相对 v5 的差异只允许落在）：
         a. 文件首行标题行与首个 `## ` 标题之前的**版本沿革前言**（blockquote/标题，
            changelog 性质、非行为语义）；
         b. 标题含「课表」的 `## ` 级节全部内容（课表 producer 扩展段重写 + 意图
            路由可落此处或同族新节）；
         c. 任何节内以竖线加反引号 schedule（display 用法表 schedule 行）起头的行；
         d. 其它节内含「课表」或 "schedule" 字样的行（课表相邻行，v6 口径重写可及）。
       冻结区（逐字保留，diff 界面齿逐行比对）：其余全部——「角色」「每个回复的两
       部分结构（除 schedule 表行）」「F4/S-F1.3 答案纪律」「故事 producer 扩展」
       「增量扩展区（包 4 char / 包 7 提醒）」「你要记住的硬约束（课表相邻行除外）」
       的正文与标题。历史版本 v1..v5 文件零回改（本件另钉 v5 sha256）。

  2) 接线三面同批指 v6 且指向同一在场非空文件（先例 = 260920-m2-charwindow 包
     test_mcd_prompt_wired_two_faces_version_agnostic 形态扩三面）：
       - 渲染面 apply-xiaozhi-overlay.sh：PROMPT_SRC 与 PROMPT_DST；
       - 声明面 provider.config.yaml：system_prompt 的 artifact/source/deploy_target；
       - 载体面 OPENCLAW_SYSTEM_PROMPT_CARRIER：`artifact（生成唯一来源）` 声明行
         与「渲染面」声明行。
     被接线版本文件须携带 ② 的新课表段语义锚（防未来版丢内容，acceptance-evidence
     接线守卫三步）。

  3) server/gateway/schedule.py 增量纯函数（既有 parse_schedule_md / week_grid_payload
     / home_upcoming_payload / group_upcoming 语义零偏移；纯 python、import 期无
     fastapi/gateway 依赖，offline-importable）：

       expand_schedule_intent_payload(payload, model, week_date_iso,
                                      now_hhmm=None, school_start=None) -> dict | None

       甲案 front 截获展开**决策面**（纯函数；front 在 relay 链上的调用时机/参数装配
       ——schedule.md 读取、week_date_iso/now 装配——属实现细节）：
       - payload 为 dict 且 view == "schedule" 且 data == {}（恰薄载荷）→
         返回 week_grid_payload(model, week_date_iso, now_hhmm, school_start)
         （**单源同算**，不得另起一套网格计算）；
       - 其余一切 payload（非 dict / view 非 schedule / data 缺失 / data 非空 dict /
         data 非 dict）→ 返回 None = 不截获，front 原样走既有 relay 链
         （薄载荷之外形状由 display_schema.validate_display 机械门裁决：新周网格
         放行、旧 {title,items} 拒绝降级——Q5，K1 已冻结，本件不重复）；
       - 脏 B1（model 解析自坏 MD）→ 沿 week_grid_payload 的 ValueError 大声传播，
         绝不静默产空网格（P2-A2 先例）。

  4) front relay 链接线（源锚代理承载，见强度申报）：front_app.py 的 display relay
     路径须引用 expand_schedule_intent_payload（薄载荷在 validate_display/下发之前
     被截获替换为展开网格）。

  跨端对端事实（wire 断言按对端实际接受什么钉；SSOT = K2 冻结件
  firmware/main/test/schedule_grid.acceptance.c 文件头 + front_app.py
  _display_wire_arguments 实测）：设备收到的 `data` 参数 = front
  json.dumps(data, ensure_ascii=False) 字符串（默认分隔符含空格）；展开网格
  data.week.courses 条目 {day:int 0 基, slot:int, name:str, flags:int}；薄载荷
  {view:schedule,data:{}} 若泄漏直达设备 → data="{}" → 设备解析 -1 降级——
  故截获展开必须发生在 front（下发之前），展开产物必须通过 validate_display 真门。

===========================================================================
契约条目 ↔ 齿 强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」体例）:

  契约 §1.3② 生产纪律（LLM 只发意图薄载荷 data:{}、front 截获展开）→ A2 齿
    （test_a2_thin_intent_payload_expands_to_week_grid_single_source 等）→ 行为级
    （真实纯函数 + 真实 validate_display 门）。
  契约 §1.3② wire 形状（data 字符串化、条目键集）→ A2 wire 齿 → 行为级
    （json.dumps/loads 往返 + 对端 K2 冻结件头注口径）。
  K1 契约门 Q2 甲案「front 截获」在 relay 链上的**真实接线** → A2 结构齿
    test_a2_front_relay_references_interceptor → **源锚代理**（钉 front_app.py 源码
    引用，非行为本体——front_app 依赖 fastapi 本机不可 offline 导入，行为级
    end-to-end 截获齿 offline 钉不到；缺口显式化：代理齿只证「引用在场」，调用时机
    正确性由真机确认轮（AC2 目击：说「帮我显示课表」屏幕真切页）兜底）。
  S-F1.3 AC1/AC2/AC4 口径 → A1 v6 措辞齿 → 源锚代理（prompt 文本措辞 = 该验收的
    本体工件——v6 的产出物就是措辞本身，故「源锚」即工件面；LLM 实际遵循度不可
    offline 机判，归 M3K3-A3 真机目击，**行为等价假设显式化：「prompt 写明 ≠ LLM
    遵循」，后者只有真机轮能证**）。
  接线不变式（渲染面==声明面==被测面，ADR-001）→ A1 三面接线齿 → 机制级
    （解析三面结构化指向 + 落盘非空 + 语义锚在场）。
  E2 历史版本零回改 → A1 v5 sha256 齿 → 机制级（内容 hash，换行归一后）。
  M3K3-A3 真机目击面 → A3 落地/对账齿 → landing 守卫（只钉制品存在 + 清单条目
    在场，目击真伪 = human 判定，offline 齿自陈不承载目击真伪）。
  弱于契约处（缺口逐条显式化）：
    ① diff 界面齿的免钉豁免按行内「课表/schedule」字样判——提醒段/角色段中含
      「课程表」「每周固定课表」字样的行因此免逐字钉（builder 误改这些行本齿不红）；
      缓解 = v5 文件 sha256 齿（v5 本体被钉，v6 侧被改坏会在三面接线/措辞锚层暴露）。
    ② 版本沿革前言整段免钉（changelog 非行为语义）——前言若误写行为语义本齿不红。
    ③ 措辞锚取并集式同义词组（问答框/备注/薄载荷等），措辞换写不被误红、语义被删
      才红——「措辞存在」不等于「LLM 遵循」，后者归真机轮。
    ④ 三触发/录入链路的 live 目击（M3K1-A4 声明模拟面之外的真机半）本件不钉。
  行为等价假设显式化：无（除上述 ②③ 自陈项外，未引入 BOOT≈KEY 类等价假设）。

Coverage map (acceptanceRef -> test):
  M3K3-A1 -> test_a1_v6_prompt_file_lands
           -> test_a1_v6_schedule_qa_box_answer_and_note_readback
           -> test_a1_v6_show_schedule_intent_thin_payload_no_llm_grid
           -> test_a1_v6_schedule_table_row_not_legacy_shape
           -> test_a1_v5_base_verbatim_diff_confinement
           -> test_a1_v5_prompt_file_immutable
           -> test_a1_prompt_wired_three_faces_version_agnostic
           -> test_a1_wired_prompt_carries_v6_schedule_anchors
  M3K3-A2 -> test_a2_thin_intent_payload_expands_to_week_grid_single_source
           -> test_a2_expanded_payload_wire_shape_and_schema_gate
           -> test_a2_interception_pass_through_and_dirty_boundary
           -> test_a2_front_relay_references_interceptor
  M3K3-A3 -> test_a3_device_round_record_lands
           -> test_a3_device_round_checklist_reconciliation
  （M3K3-E1 = 全件零新增红；M3K3-E2 = test_a1_v5_prompt_file_immutable +
   test_a1_v5_base_verbatim_diff_confinement；M3K3-E3 = test_a3_* 两齿。）

RED baseline（现状树 @ 91b3b5c + 本冻结件，acceptance-author 2026-09-21 实跑，
命令与摘要见 taskpackage redBaselineCommand / 交付报告）：A1 落盘/措辞/diff 界面/
接线锚齿红（v6 未落盘 = 真实缺失行为；v5 无课表段新锚）；A2 四齿红（schedule.py 无
expand_schedule_intent_payload = 意图截获链路未实现；front 未引用 = 接线缺失）；
A3 两齿红（设备轮记录未落 = 轮未发生的设计态红）；must-stay-green = 三面接线一致性
齿 + v5 不可回改齿（现树即绿，builder bump 不得打破）。收集零错误（冻结即冒烟，
DEV-GOVERNANCE 验收测试纪律 §3）。

Run protocol（G 包/K1 先例；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance
  （standalone: python 本文件）
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import re
import sys
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

# --------------------------------------------------------------------------- #
# Paths（仓根由 __file__ 显式推导，不依赖调用点 cwd——acceptance-evidence root 注入纪律）
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
GATEWAY_DIR = SERVER / "gateway"
SCHEDULE_MODULE = GATEWAY_DIR / "schedule.py"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"

PROMPT_RENDER_SH = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
PROMPT_DECL_YAML = DEPLOY_DIR / "provider.config.yaml"
PROMPT_CARRIER = DEPLOY_DIR / "OPENCLAW_SYSTEM_PROMPT_CARRIER"

V6_NAME = "openclaw-system-prompt.v6.md"
DEVICE_ROUND_RECORD = (REPO_ROOT / "delivery" / "taskpackages"
                       / "260921-m3-k3-voice-schedule-entry-live"
                       / "device-round-record.md")

# 冻结时点 v5 内容 hash（换行归一后；E2「prompt v5 及以下历史版本零回改」落地守卫）。
# 计算式 = sha256(read_text.replace("\r\n", "\n"))。v6 bump 与一切后续包不得改 v5 本体。
V5_SHA256 = "318be2ee4ce44ff0938466fe2b5cc452c86de22d69d6be90474d22a908992f8d"

INTENT_FN = "expand_schedule_intent_payload"

# --------------------------------------------------------------------------- #
# A2 fixtures —— B1 课表 MD（2026-09-21 = 周一；与 K1 冻结件同源口径，缩样）
# --------------------------------------------------------------------------- #

B1_MD = """---
child: 小明
timezone: Asia/Shanghai
---

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琼行 | 带琴谱 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-09-21 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 |
|------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 |
"""

MON = "2026-09-21"   # 周一（day 0）


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


# --------------------------------------------------------------------------- #
# schedule.py offline loader（K1 冻结件同形态；缺符号 = RED 而非 crash）
# --------------------------------------------------------------------------- #

_module_cache: dict = {}


def _load_schedule_module():
    cached = _module_cache.get("mod")
    if cached is not None:
        return cached
    if not SCHEDULE_MODULE.is_file():
        return None, (f"{SCHEDULE_MODULE} missing — gateway schedule 纯模块不在位。")
    try:
        spec = importlib.util.spec_from_file_location(
            "kidpal_gateway_schedule_m3k3", SCHEDULE_MODULE)
        if spec is None or spec.loader is None:
            return None, f"{SCHEDULE_MODULE} not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _module_cache["mod"] = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error is a RED, not a crash
        return None, (f"{SCHEDULE_MODULE} failed to import: {e!r}（frozen contract "
                      "module MUST be pure python — no fastapi/gateway imports at "
                      "import time）.")


def _schedule_callable(name: str):
    mod, err = _load_schedule_module()
    if mod is None:
        return None, err
    fn = getattr(mod, name, None)
    if not callable(fn):
        return None, (f"schedule.py has no module-level callable {name}() — frozen "
                      "M3K3-A2 offline contract (front 截获展开决策面) not implemented.")
    return fn, None


def _display_schema():
    """真实冻结机械门（V27/Q5）——shape gate of record。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415 — offline import, cached
    return ds


def _parse_b1():
    parse, err = _schedule_callable("parse_schedule_md")
    if parse is None:
        raise AssertionError("M3K3-A2 RED: " + err)
    return parse(B1_MD)


# --------------------------------------------------------------------------- #
# prompt 版本无关接线解析（三面：渲染面 / 声明面 / 载体面）——MCD 包先例形态
# --------------------------------------------------------------------------- #

_PROMPT_FILE_RE = re.compile(r"openclaw-system-prompt\.v\d+\.md")


def _wired_prompt_faces():
    """三面解析当前被接线的版本化 prompt 文件名（不硬写版本字面，ARQ-260908 纪律）。

    SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E3：vN 三面接线判法随
    承载反转退役，face-equality 由该包新验收的静态面/动态面集合相等不变式取代；
    本解析器现存留作历史口径参照、当前无调用方（M3K3-A1 两个接线齿已 supersede
    改钉 AGENTS.md 承载面）。

    返回 (rendered, declared, carried)：各自为从该面解析出的文件名列表。"""
    render_src = _read_text(PROMPT_RENDER_SH)
    rendered = re.findall(
        r"PROMPT_(?:SRC|DST)\s*=.*?(openclaw-system-prompt\.v\d+\.md)", render_src)
    decl_src = _read_text(PROMPT_DECL_YAML)
    declared = re.findall(
        r"(?:artifact|source|deploy_target)\s*:\s*\"[^\"]*"
        r"(openclaw-system-prompt\.v\d+\.md)", decl_src)
    carried = []
    carrier = _read_text(PROMPT_CARRIER)
    for line in carrier.splitlines():
        if "artifact（生成唯一来源）" in line or (
                line.strip().startswith("- 渲染面") and "openclaw-system-prompt" in line):
            m = _PROMPT_FILE_RE.search(line)
            if m:
                carried.append(m.group(0))
    return rendered, declared, carried


# --------------------------------------------------------------------------- #
# v6 课表段语义锚（内容驱动选节：课表节正文 + schedule 表行 + 硬约束课表相邻行的并集）
# --------------------------------------------------------------------------- #

def _schedule_face_window(text: str) -> str:
    """课表相关措辞面并集：标题含「课表」的 `## ` 节全文 + `| \\`schedule\\`` 表行 +
    其它节内含「课表」/"schedule" 的行。"""
    lines = text.splitlines()
    heads = [i for i, l in enumerate(lines) if l.startswith("## ")]
    out: list[str] = []
    for k, i in enumerate(heads):
        j = heads[k + 1] if k + 1 < len(heads) else len(lines)
        sec = lines[i:j]
        if "课表" in lines[i]:
            out.extend(sec)
        else:
            out.extend(l for l in sec if "课表" in l or "schedule" in l)
    if not heads:  # 无节结构（异常文件）——退化为全文
        out = lines
    return "\n".join(out) + "\n" + "\n".join(
        l for l in lines if l.startswith("| `schedule`"))


def _assert_v6_qa_note_anchors(testcase, body: str, label: str):
    """课表问答口径锚（AC1 问答框 + AC4 备注读回）。措辞锚取并集式同义词组
    （见文件头强度申报③）：语义被删才红、同义换写不误红。"""
    window = _schedule_face_window(body)

    # AC1：课表问答答复走问答框（与百科问答同一问答框，不切课表页不滚动定位）。
    testcase.assertTrue(
        "问答框" in window,
        f"M3K3-A1 RED({label}): 课表段无「问答框」口径锚——S-F1.3 AC1 答复与百科问答"
        "同一问答框（不切课表页）的生成端约定缺失。")
    testcase.assertTrue(
        any(w in window for w in ("不切", "不滚动", "不定位", "不切课表页")),
        f"M3K3-A1 RED({label}): 课表段无「不切页/不定位」口径锚——AC1 问答不嵌课表页"
        "（2026-09-14 human 裁）的生成端约定缺失。")

    # AC4：答复包含备注内容（带物读回）。
    testcase.assertTrue(
        "备注" in window,
        f"M3K3-A1 RED({label}): 课表段无「备注」锚——S-F1.3 AC4 答复包含备注内容"
        "（带物读回）口径缺失。")
    testcase.assertTrue(
        any(w in window for w in ("答复", "读回", "念出", "读出")),
        f"M3K3-A1 RED({label}): 课表段无「答复/读回」锚——备注读进口头答复的口径缺失。")


def _assert_v6_intent_thin_anchors(testcase, body: str, label: str):
    """意图信号化锚（AC2 意图短语 + 薄载荷 data:{} + LLM 不产网格）。"""
    window = _schedule_face_window(body)

    testcase.assertTrue(
        any(w in window for w in ("帮我显示课表", "打开课表")),
        f"M3K3-A1 RED({label}): 课表段无「帮我显示课表/打开课表」意图锚——S-F1.3 AC2 "
        "意图路由的生成端约定缺失。")
    testcase.assertTrue(
        "薄载荷" in window or "{}" in window,
        f"M3K3-A1 RED({label}): 课表段无薄载荷 data:{{}} 锚——意图信号化（甲案）口径"
        "缺失，课表段仍在让 LLM 产课表数据。")

    # 网格数据 LLM 不生成（Q4 front 机械算单源 / 意图信号化）。
    testcase.assertTrue(
        any(w in window for w in ("不生成", "不由你", "不由 LLM", "不产", "不得生成",
                                  "front", "网关", "机械计算")),
        f"M3K3-A1 RED({label}): 课表段无「网格数据不由 LLM 生成」锚——契约 §1.3② 生产"
        "纪律（front 机械计算、LLM 意图信号化）的生成端约定缺失。")


def _assert_v6_schedule_anchors(testcase, body: str, label: str):
    """v6 课表段全组口径锚 = 问答/备注半 + 意图/薄载荷半（接线锚齿用全组）。"""
    _assert_v6_qa_note_anchors(testcase, body, label)
    _assert_v6_intent_thin_anchors(testcase, body, label)


def _schedule_table_rows(text: str) -> list[str]:
    return [l for l in text.splitlines() if l.startswith("| `schedule`")]


def _assert_schedule_table_row_not_legacy(testcase, body: str, label: str):
    """display 用法表的 schedule 行不得再描述旧 {title,items} 形状（Q5 即废）。"""
    rows = _schedule_table_rows(body)
    testcase.assertTrue(
        rows, f"M3K3-A1 RED({label}): display 用法表无 `schedule` 行——view 白名单表"
              "结构被破坏（7 view 表行须在）。")
    for row in rows:
        testcase.assertFalse(
            "title" in row and "items" in row,
            f"M3K3-A1 RED({label}): schedule 表行仍描述旧形状 {row.strip()!r} —— "
            "契约 §1.3② 2026-09-21 起旧 {title,items} 即废（Q5 收紧），LLM 只发薄载荷。")
        testcase.assertTrue(
            ("{}" in row) or ("薄载荷" in row) or ("意图" in row),
            f"M3K3-A1 RED({label}): schedule 表行未改述薄载荷/意图语义：{row.strip()!r}")


# --------------------------------------------------------------------------- #
# M3K3-A1 — prompt v6 落盘 / 措辞 / diff 界面 / 历史零回改齿
# （guard-class: landing / wording —— 落盘与措辞类守卫钉死 v6/v5 字面合理，见
#   acceptance-evidence「必要证据」末条；接线类守卫一律版本无关，见下类）
# --------------------------------------------------------------------------- #

class M3K3PromptV6Acceptance(unittest.TestCase):
    """M3K3-A1: v6 落盘锚齿 + 课表段口径齿 + v5 基座逐字保留 diff 界面齿 + v5 零回改。"""

    maxDiff = None

    @property
    def _v5(self) -> str:
        return _read_text(DEPLOY_DIR / "openclaw-system-prompt.v5.md")

    @property
    def _v6(self) -> str:
        return _read_text(DEPLOY_DIR / V6_NAME)

    # -- 落盘锚齿（guard-class: landing） -----------------------------------
    def test_a1_v6_prompt_file_lands(self):
        """coverage: M3K3-A1 — openclaw-system-prompt.v6.md 落盘且非空（版本化演进，
        v6 槽位 ARQ-260908 预约规则）。"""
        path = DEPLOY_DIR / V6_NAME
        self.assertTrue(
            path.is_file(),
            f"M3K3-A1 RED: {V6_NAME} 未落盘——prompt v6 问课表口径（S-F1.3 AC1/AC2/AC4 "
            "生成端约定 + 意图信号化）无载体。")
        self.assertTrue(
            self._v6.strip(),
            f"M3K3-A1 RED: {V6_NAME} 是空文件。")

    # -- 课表段口径措辞齿（guard-class: wording） ---------------------------
    def test_a1_v6_schedule_qa_box_answer_and_note_readback(self):
        """coverage: M3K3-A1 / S-F1.3 AC1+AC4 — v6 课表段：答复走问答框文本（不切页
        不定位）+ 答复包含备注内容（带物读回）。"""
        self.assertTrue(self._v6.strip(), "M3K3-A1 RED: v6 未落盘（前置）。")
        _assert_v6_qa_note_anchors(self, self._v6, "v6 文件")

    def test_a1_v6_show_schedule_intent_thin_payload_no_llm_grid(self):
        """coverage: M3K3-A1 / S-F1.3 AC2 + 契约 §1.3② 生产纪律 — v6 课表段：
        「帮我显示课表」意图 → schedule 薄载荷 data:{}；网格数据 LLM 不生成。"""
        self.assertTrue(self._v6.strip(), "M3K3-A1 RED: v6 未落盘（前置）。")
        _assert_v6_intent_thin_anchors(self, self._v6, "v6 文件")

    def test_a1_v6_schedule_table_row_not_legacy_shape(self):
        """coverage: M3K3-A1 — v6 display 用法表 schedule 行不再描述旧 {title,items}
        形状（Q5 即废），改述薄载荷/意图语义。"""
        self.assertTrue(self._v6.strip(), "M3K3-A1 RED: v6 未落盘（前置）。")
        _assert_schedule_table_row_not_legacy(self, self._v6, "v6 文件")

    # -- v5 基座逐字保留 diff 界面齿（guard-class: wording） -----------------
    def test_a1_v5_base_verbatim_diff_confinement(self):
        """coverage: M3K3-A1 / M3K3-E2 — v6 相对 v5 的差异只落在冻结声明的可改区
        （版本沿革前言 + 课表节 + schedule 表行 + 课表相邻行）；兄弟段（角色 / 两部分
        结构 / F4 答案纪律 / 故事 producer 扩展 / 增量扩展区包 4 包 7 / 硬约束课表相邻
        行之外的正文）逐字保留，不改坏兄弟包冻结齿（v5 头注版本化纪律）。"""
        v5, v6 = self._v5, self._v6
        self.assertTrue(v5.strip(), "M3K3-A1 RED(infra): v5 不在场。")
        self.assertTrue(v6.strip(), "M3K3-A1 RED: v6 未落盘（前置）。")

        def split(text: str):
            lines = text.splitlines()
            heads = [i for i, l in enumerate(lines) if l.startswith("## ")]
            secs = []
            for k, i in enumerate(heads):
                j = heads[k + 1] if k + 1 < len(heads) else len(lines)
                secs.append((lines[i], lines[i + 1:j]))
            return secs

        v5_secs, v6_secs = split(v5), split(v6)
        # 兄弟节筛选：标题含「课表」的节 = 本包可改区，不参与逐字比对。
        frozen_v5 = [(h, body) for h, body in v5_secs if "课表" not in h]
        frozen_v6 = [(h, body) for h, body in v6_secs if "课表" not in h]
        self.assertEqual(
            [h for h, _ in frozen_v5], [h for h, _ in frozen_v6],
            "M3K3-A1 RED: v6 相对 v5 的 `## ` 级节集发生变化（新增/删除/改名了非课表节）"
            "——版本化演进只许动课表段（文件头 FROZEN OFFLINE CONTRACT 可改区）。")

        def frozen_body(body: list[str]) -> list[str]:
            keep = []
            for l in body:
                if l.startswith("| `schedule`"):
                    continue
                if "课表" in l or "schedule" in l:
                    continue
                keep.append(l)
            return keep

        for (h5, b5), (h6, b6) in zip(frozen_v5, frozen_v6):
            self.assertEqual(
                frozen_body(b5), frozen_body(b6),
                f"M3K3-A1 RED: v6 改动了兄弟节 {h5!r} 的冻结正文（课表相邻行之外的行"
                "被增删改）——v5 基座逐字保留、可 diff，故事/查字/提醒段零触碰 "
                "（plan.md 改法取舍 / M3K3-E2）。")

    # -- v5 历史文件零回改（guard-class: landing，机制级 sha256） ------------
    def test_a1_v5_prompt_file_immutable(self):
        """coverage: M3K3-A1 / M3K3-E2 — v5 本体零回改（内容 hash，换行归一后）。
        版本化演进纪律：历史版本可 diff、不回改；改动 v5 = 铁律违规。"""
        v5 = self._v5.replace("\r\n", "\n")
        self.assertEqual(
            hashlib.sha256(v5.encode("utf-8")).hexdigest(), V5_SHA256,
            "M3K3-A1 RED: openclaw-system-prompt.v5.md 内容 hash 与冻结时点不符——"
            "历史版本文件被回改（M3K3-E2 铁律：prompt v5 及以下零回改；如确需改动"
            "属治理裁决，须 human ruling 后更新本钉）。")


# --------------------------------------------------------------------------- #
# M3K3-A1 — prompt 接线三面一致性齿（guard-class: wiring，版本无关——ARQ-260908
# 纪律：解析两面得出版本再断言，接线类断言零版本字面）
# --------------------------------------------------------------------------- #

class M3K3PromptWiringAcceptance(unittest.TestCase):
    """M3K3-A1 接线不变式：渲染面 == 声明面 == 载体面 == 被测面（ADR-001）。"""

    maxDiff = None

    def test_a1_prompt_wired_three_faces_version_agnostic(self):
        """coverage: M3K3-A1（接线守卫，261022 语义换代）— 静态语义主承载在场。

        SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1/K1-E3：
        旧语义 = 渲染面（apply-xiaozhi-overlay.sh PROMPT_SRC/PROMPT_DST）、声明面
        （provider.config.yaml system_prompt 三键）、载体面（OPENCLAW_SYSTEM_PROMPT_
        CARRIER 声明行）三面解析 vN 接线版并断言三面一致且落盘非空（ADR-001 vN
        形态）；新语义 = 承载反转（v11 静态语义迁 workspace AGENTS.md 为静态语义
        单一事实源、引擎只发有界动态块），vN 三面 face-equality 由该包新验收
        test_261022_m5_k1_prompt_carrier_migration.acceptance.py 的 K1-E3 静态面/
        动态面集合相等不变式取代。本齿改钉（must-stay-green）：AGENTS.md 静态
        主承载镜像在场非空（测试 id 保留以稳定历史 coverage 引用）。"""
        # SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1：被测承载面从
        # 「接线版 vN 文件」换为 workspace AGENTS.md。
        agents_path = DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
        self.assertTrue(
            agents_path.is_file(),
            "M3K3-A1 RED(superseded): workspace-kidpal/AGENTS.md 不在场——静态语义"
            "单一事实源镜像缺失（261022-m5-k1 承载反转 K1-E1）。")
        self.assertTrue(
            _read_text(agents_path).strip(),
            "M3K3-A1 RED(superseded): workspace-kidpal/AGENTS.md 是空文件——"
            "静态语义主承载空载（261022-m5-k1 K1-E1）。")

    def test_a1_wired_prompt_carries_v6_schedule_anchors(self):
        """coverage: M3K3-A1（接线守卫锚半，261022 语义换代）— 课表段语义锚承载。

        SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1：旧语义 = 三面
        解析出的当前接线版 vN 文件正文携带本包课表段新语义锚；新语义 = 静态语义
        单一事实源 AGENTS.md 承载课表段锚（问答框口径 / 备注读回 / 意图薄载荷 /
        LLM 不产网格——`_assert_v6_schedule_anchors` 为正文并集窗口、承载形态
        容忍）；旧 {title,items} schedule 形状负锚同步改为 AGENTS.md 正文级负向
        （不要求 AGENTS.md 复刻 display 用法表行结构）。现树未迁入 = 设计态红；
        builder 完成迁移后转绿。"""
        # SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E1：承载面从「接线版
        # vN 文件」换为 workspace AGENTS.md。
        agents_path = DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
        self.assertTrue(
            agents_path.is_file(),
            "M3K3-A1 RED(superseded): workspace-kidpal/AGENTS.md 不在场（前置）。")
        body = _read_text(agents_path)
        self.assertTrue(body.strip(), "M3K3-A1 RED(superseded): AGENTS.md 为空（前置）。")
        _assert_v6_schedule_anchors(self, body, "workspace-kidpal/AGENTS.md")
        # 旧 {title,items} 形状负锚（正文级，AGENTS.md 不要求表格结构）。
        legacy_rows = [
            l for l in body.splitlines()
            if "schedule" in l.lower() and "title" in l and "items" in l]
        self.assertEqual(
            legacy_rows, [],
            "M3K3-A1 RED(superseded): AGENTS.md 仍描述旧 {title,items} schedule "
            f"形状（{legacy_rows[:1]!r}）——契约 §1.3② 2026-09-21 起旧形状即废"
            "（Q5 收紧），LLM 只发薄载荷；迁移不得把旧形状带进静态语义主承载。")


# --------------------------------------------------------------------------- #
# M3K3-A2 — 「帮我显示课表」意图链 offline 齿（front 截获展开决策面 + wire + 接线代理）
# --------------------------------------------------------------------------- #

class M3K3IntentChainAcceptance(unittest.TestCase):
    """M3K3-A2: 意图信号（= A1 措辞齿）→ front 截获展开 → view=schedule 下发 →
    设备切页（K2 冻结件承载）。LLM 意图判定归真机轮，本类不造假绿。"""

    maxDiff = None

    def _expand(self, payload, model, week, now=None, school=None):
        fn, err = _schedule_callable(INTENT_FN)
        if fn is None:
            raise AssertionError("M3K3-A2 RED: " + err)
        return fn(payload, model, week, now, school)

    def test_a2_thin_intent_payload_expands_to_week_grid_single_source(self):
        """coverage: M3K3-A2 / S-F1.3 AC2 + 契约 §1.3② — 意图薄载荷
        {view:schedule,data:{}} 截获展开 = week_grid_payload 同参产出（**单源同算**，
        不得另起一套网格计算），展开载荷过真实 validate_display 机械门。"""
        grid_fn, err = _schedule_callable("week_grid_payload")
        if grid_fn is None:
            self.fail("M3K3-A2 RED(infra): " + err)
        model = _parse_b1()
        expanded = self._expand({"view": "schedule", "data": {}},
                                model, MON, "17:00", "08:00")
        self.assertEqual(
            expanded, grid_fn(model, MON, "17:00", "08:00"),
            "M3K3-A2 RED: 薄载荷展开结果 != week_grid_payload 同参产出——截获展开必须"
            "单源复用 K1 周网格 producer（防两套网格计算漂移）。")
        ok, reason = _display_schema().validate_display(expanded)
        self.assertTrue(
            ok, f"M3K3-A2 RED: 展开载荷未过真实 validate_display 门 ({reason})——"
                "展开网格必须原样过机械门才能下发设备（Q5 收紧面）。")

    def test_a2_expanded_payload_wire_shape_and_schema_gate(self):
        """coverage: M3K3-A2 — wire 形状：展开载荷的 data 经 front
        json.dumps(ensure_ascii=False) 字符串化后往返保真（对端设备解析面 = K2 冻结件
        kidpal_schedule_grid_parse 接受的 data JSON 字符串；跨端不申报 = 离线绿真机红，
        g-relay-fix 教训）。"""
        expanded = self._expand({"view": "schedule", "data": {}}, _parse_b1(), MON)
        self.assertIsInstance(expanded, dict, "M3K3-A2 RED: 展开产物非 dict 信封。")
        self.assertEqual(expanded.get("view"), "schedule",
                         "M3K3-A2 RED: 展开信封 view != schedule——设备不会切 P2。")
        data = expanded["data"]
        wire = json.dumps(data, ensure_ascii=False)
        self.assertEqual(
            json.loads(wire), data,
            "M3K3-A2 RED: 展开载荷 data 字符串化往返失真——设备解析面（K2 "
            "kidpal_schedule_grid_parse）拿到的 data JSON 字符串必须逐字还原网格。")
        ok, reason = _display_schema().validate_display(expanded)
        self.assertTrue(ok, f"M3K3-A2 RED: 展开载荷被机械门拒绝 ({reason})。")

    def test_a2_interception_pass_through_and_dirty_boundary(self):
        """coverage: M3K3-A2 边界 — 恰薄载荷才截获：非 dict / view 非 schedule /
        data 缺失 / data 非空（新周网格形态）/ data 非 dict → None（原样走既有 relay
        链，形状裁决归 validate_display）；脏 B1 → ValueError 大声传播不静默产空网格。"""
        model = _parse_b1()
        grid_fn, err = _schedule_callable("week_grid_payload")
        if grid_fn is None:
            self.fail("M3K3-A2 RED(infra): " + err)
        full_grid = grid_fn(model, MON)
        cases = [
            ("full week grid payload", full_grid),
            ("legacy {title,items} shape",
             {"view": "schedule", "data": {"title": "今天", "items": []}}),
            ("missing data key", {"view": "schedule"}),
            ("data not empty dict",
             {"view": "schedule", "data": {"week": {"courses": []}}}),
            ("data not a dict", {"view": "schedule", "data": "[]"}),
            ("non-schedule view", {"view": "text", "data": {"text": "hi"}}),
            ("not a dict", "```display\n{view:schedule}\n```"),
        ]
        for label, payload in cases:
            self.assertIsNone(
                self._expand(payload, model, MON),
                f"M3K3-A2 RED: 非薄载荷被误截获（{label}）——截获判据必须恰为 "
                "view==schedule 且 data=={}，其余原样走 relay 链交机械门裁决。")
        dirty = B1_MD.replace("| 周一 | 16:30", "| 周九 | 16:30")
        dirty_model, err2 = _schedule_callable("parse_schedule_md")
        if err2:
            self.fail("M3K3-A2 RED(infra): " + err2)
        with self.assertRaises(
                ValueError,
                msg="M3K3-A2 RED: 脏 B1 经截获展开链路必须 ValueError 大声拒绝，"
                    "绝不静默产网格（P2-A2 先例）。"):
            self._expand({"view": "schedule", "data": {}},
                         dirty_model(dirty), MON)

    def test_a2_front_relay_references_interceptor(self):
        """coverage: M3K3-A2（源锚代理，结构性）— front relay 链引用截获展开函数。
        **源锚代理自陈**：钉 front_app.py 源码对 expand_schedule_intent_payload 的引用
        在场，非行为本体（front_app 依赖 fastapi 本机不可 offline 导入，行为级 end-to-
        end 截获 offline 钉不到）；调用时机正确性 = 真机确认轮 AC2 目击（说「帮我显示
        课表」屏幕真切 P2）兜底。缺口已写入文件头强度申报。"""
        src = _read_text(FRONT_APP)
        self.assertIn(
            INTENT_FN, src,
            f"M3K3-A2 RED: front_app.py 未引用 {INTENT_FN}——薄载荷在 relay 链上无截获"
            "落点，意图薄载荷会直达设备被解析 -1 降级（「帮我显示课表」真机不切页）。")


# --------------------------------------------------------------------------- #
# M3K3-A3 — 真机确认轮制品面（guard-class: landing；真机目击项不 offline 钉）
# --------------------------------------------------------------------------- #

class M3K3DeviceRoundRecordAcceptance(unittest.TestCase):
    """M3K3-A3 offline 面：设备轮记录落地 + 清单覆盖条目对账。目击真伪 = human
    判定（taskpackage M3K3-A3 / M3K3-E3），本类只钉可机判的制品结构面。"""

    maxDiff = None

    @property
    def _record(self) -> str:
        return _read_text(DEVICE_ROUND_RECORD)

    def test_a3_device_round_record_lands(self):
        """coverage: M3K3-A3 / M3K3-E3 — 设备轮记录文件落地（标准 closeout 制品位，
        先例 = delivery/taskpackages/<changeId>/device-round-record.md）。"""
        self.assertTrue(
            DEVICE_ROUND_RECORD.is_file(),
            f"M3K3-A3 RED: 设备轮记录未落地（{DEVICE_ROUND_RECORD}）——human 持板真机"
            "确认轮（M3K3-A3 清单目击 + 三源日志取证 + K1/K2 deferred 收口）未发生或"
            "未留痕。")
        self.assertTrue(
            self._record.strip(), "M3K3-A3 RED: 设备轮记录是空文件。")

    def test_a3_device_round_checklist_reconciliation(self):
        """coverage: M3K3-A3 / M3K3-E3 — 记录须对账覆盖 taskpackage M3K3-A3 清单各族
        条目（S-F1.2 AC1–AC4 + NFR-READ-1 / S-F1.3 AC1–AC4 / S-F1.5 AC1–AC2 / 决策 26
        三触发 / K1/K2 deferred 真机半 / 三源日志）。同义词组任一命中即算覆盖（并集
        容错）；缺族 = 红并点名缺哪族。"""
        record = self._record
        self.assertTrue(record.strip(), "M3K3-A3 RED: 设备轮记录未落地（前置）。")
        families = [
            ("S-F1.2 网格/周总览", ("S-F1.2", "周总览", "网格")),
            ("S-F1.2 今天列", ("今天列", "今天", "高亮")),
            ("S-F1.2 ※ 临时标记", ("※", "临时")),
            ("S-F1.2 星标", ("星标", "★")),
            ("NFR-READ-1 14px 目测", ("14px", "14 px", "NFR-READ-1")),
            ("S-F1.3 问答框答复", ("S-F1.3", "问答框", "问答")),
            ("S-F1.3 帮我显示课表切页", ("帮我显示课表", "打开课表", "切页", "切到")),
            ("S-F1.3 播报声波", ("声波", "播报")),
            ("S-F1.3 备注读回", ("备注",)),
            ("S-F1.5 QQ 录入三处即时", ("S-F1.5", "QQ", "录入")),
            ("S-F1.5 直改文件/跨天兜底", ("跨天", "副通道", "直改")),
            ("决策 26 触发①联网/建连", ("联网", "建连", "连接")),
            ("决策 26 触发②每日零点", ("零点", "每日")),
            ("决策 26 触发③录入后即时", ("录入后", "即时")),
            ("K1 deferred M3K1-A5", ("M3K1-A5",)),
            ("K2 deferred M3K2-A7", ("M3K2-A7",)),
            ("三源日志·串口", ("串口", "serial")),
            ("三源日志·front/systemd", ("front", "systemd")),
            ("三源日志·引擎 docker logs", ("docker", "引擎", "engine")),
        ]
        missing = [name for name, tokens in families
                   if not any(t in record for t in tokens)]
        self.assertEqual(
            missing, [],
            f"M3K3-A3 RED: 设备轮记录清单对账缺族：{missing} —— M3K3-A3 确认清单"
            "（S-F1.2/S-F1.3/S-F1.5/决策 26 三触发/NFR-READ-1/K1-K2 deferred 真机半/"
            "三源日志）逐项判定须在记录中对账留痕（K1/K2 deferred 未闭环 = E3 缺口）。")


# --------------------------------------------------------------------------- #

if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    for cls in (M3K3PromptV6Acceptance, M3K3PromptWiringAcceptance,
                M3K3IntentChainAcceptance, M3K3DeviceRoundRecordAcceptance):
        suite.addTests(loader.loadTestsFromTestCase(cls))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
