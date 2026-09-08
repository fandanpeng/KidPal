"""Frozen acceptance tests for TaskPackage 260829-schedule-reminder (subsystem: server).

Scope of record (taskpackage acceptance/exitCriteria + human 裁定一/裁定二 + 上游实测面):
RMD-A2 OpenClaw 侧三档排程算术（按区块范围 + 已过去档位跳过）+ 幂等 id 确定性散列 +
alerts/pending.json 原子写；RMD-A3 speech 面（借引擎既有音频链的**指令构造** +
合成缺省/失败时**降级仍投递**）；RMD-A4 网关提醒投递（每分钟轮询到点判定、kidpal/alert
载荷形状、JSON-RPC id 数字化、重试到确认 60s×3、过期静默作废、同 id 重推幂等、回执接出队）；
RMD-A7 网关侧一半（离线期间未送达档位在推送时点判定作废、不做重连即重推）；
RMD-A8 携带内容走备注列 + 语义产物 schema + 约定文本承载。

两条 human 裁定（pod-events 留痕，本验收据此钉，不得越界）：
  裁定一 eventId acaef302-9ddc-4508-b5d3-94023e050f43 = 生产侧混合分层。
    可离线真跑、必须硬断言的：时间串解析、跨天映射、事前 N 分钟减法、已过去档位跳过、
    幂等 id 确定性散列、pending.json 原子写、机械 schema 校验、降级行为。
    只能钉「产物 schema + 约定文本存在性 + 脚本对坏/缺语义输入的降级」的：备注里哪部分算
    要带的（语义判断）与给孩子的文案（OpenClaw agent 回合产出）——**不假装能离线验大模型输出**。
  裁定二 eventId 09359e74-005b-45bb-9535-69e19c14ee04 = 提醒语音借引擎既有音频链。
    front 到点向**引擎腿注入内部指令**（core/handle/intentHandler.py:233 speak_txt 走既有
    edge-tts provider + 已 live 实证的 opus 下行链出音）；front 侧不自建合成、不做
    mp3→opus 编码、不自己注入二进制音频帧（设备 send_bytes 必须恒空 = 可机判红线）。
    真机音频时序（弹窗与 TTS 并发）殿后 9b，不在本包离线验收面。

FROZEN OFFLINE CONTRACT — module 1 (OpenClaw 生产侧, 随 server/deploy/ 上 ECS 的确定性
python 脚本，纯 stdlib、离线可导入；先例同目录 warm_char_cache.py；建议家
server/deploy/openclaw-patch/reminder_schedule_lib.py):

  LADDER_NIGHT = "night"; LADDER_MORNING = "morning"; LADDER_SOON = "soon"
  DEFAULT_LADDER_CONFIG = {"night_at": "20:30", "morning_at": "07:45", "before_min": 15}
      三档时刻是**纯服务端配置**（spec S-F1.4 三段阶梯，改时刻即时生效、永不需烧板子），
      不进契约 B1、不进固件（固件侧零引用由 C 孪生档 RMD-A7 齿负向钉死）。

  load_ladder_config(path: str|Path) -> dict
      Read the deployed reminder config carrier (YAML-lite `key: value` lines, '#'
      comments and unknown keys ignored).  Missing/unreadable file -> a copy of
      DEFAULT_LADDER_CONFIG (排程不得因缺配置而停摆).  Values normalised:
      night_at/morning_at -> "HH:MM" str, before_min -> int.

  entry_key(section: str, row: dict) -> str
      Stable identity of a schedule row: "weekly|<weekday>|<start>-<end>|<name>|<location>"
      (end omitted when None) / "onetime|<date>|<start>-<end>|<name>|<location>".
      section ∈ {"weekly","onetime"}; the 学校课程表 section has NO reminder plan at all.

  reminder_id(key: str, ladder: str, event_date: str) -> str
      确定性散列(条目 + 档位 + 事件日期)，sha256 派生、"rmd-" 前缀、长度 <= 40（设备侧
      id[40] 上界）、仅 [0-9A-Za-z._-] 字符。**跨重推/重算不变**：输入只有 (key, ladder,
      event_date) —— 不得掺 now/随机数/进程状态/pending.json 既有内容（V15）。

  load_semantic(path) -> (mapping|None, err:str|None)
      Read the OpenClaw agent 语义中间产物 (备注里哪部分算要带的 + 给孩子的文案):
      {"<entry_key>": {"title"?: str, "speech"?: str, "bring"?: [str]}}.
      Missing file / bad JSON / non-object root -> (None, reason) — 脚本必须能降级继续排程，
      不得抛异常。个别条目形状不合法 -> 该条目被丢弃（不影响其它条目），不崩。

  bring_items(note: str, semantic_entry: dict|None) -> list[str]
      携带内容（RMD-A8）：语义产物给了 bring 列表就用它（原样、去空串）；否则**机械兜底**
      = 备注非空时整条备注作单项 ["<note>"]，备注空 -> []。绝不凭空发明要带物。

  plan_reminders(model: dict, now_iso: str, config=None, semantic=None,
                 horizon_days: int = 7) -> list[dict]
      契约 §三 提醒条目列表（按 id 升序），规则（全部可离线机判）：
        范围（按区块判）：model["weekly"] 在 horizon 内按星期重复生成（周末班同样生成、
          每周重复）；model["onetime"] 只在 date 精确匹配当日/未来时生成（**过期一次性安排
          不生成**，契约 §2.3）；model["school"] **永不生成任何档位**（在校例行不提醒）。
        三档：night = (事件日期 - 1 天) 的 config["night_at"]；morning = 事件日期当天的
          config["morning_at"]；soon = 事件开始时刻 - config["before_min"] 分钟。
        已过去档位自动跳过：fire_at <= now 的档位不产出（如当晚才录入「明天」的事，
          前晚档不发）。
        时刻来源：全部从 config 读；改 config 只改 fire_at，**id 不变**（幂等复用）。
      每项形状恰为 {"id","fire_at","title","speech","view"}（契约 §三 零增减）：
        fire_at = "YYYY-MM-DDTHH:MM:SS+08:00";
        title/speech 优先取语义产物、缺省时机械兜底文案（night 有备注时兜底文案**必须带上
        备注原文**——这是「携带内容走备注列承载」的可机判落点）；
        view = {"view": "text", "data": {"text": ...}} —— 内嵌 display 块只能用契约 §1.3
        白名单，不得发明提醒专用 view（state-reminder 是 UI 状态名，不是契约 view）。
      缺区块不崩：真实 ECS schedule.md 只有「每周固定课表」一节、备注列全空、frontmatter
      仍带已废止的 remind_before_min —— 该形状必须正常产出、且 remind_before_min 的值
      **不参与任何计算**（提前量只从 config 读）。

  write_pending_atomic(path, entries: list[dict]) -> None
      原子写同机交接文件：容器 {"version": 1, "generated_at": <iso>, "alerts": [...]}
      （alerts 按 id 升序）。写临时文件 + os.replace；**完成后目录内不得残留 .tmp**；
      两次写入同一 plan 的 alerts 数组必须逐字节相同（generated_at 可不同，且它不被消费）。

FROZEN OFFLINE CONTRACT — module 2 (网关提醒投递面，server/gateway/ 的 additive 扩展；
纯 python、导入期零网络 I/O、离线可导入；建议家 server/gateway/reminders.py):

  POLL_INTERVAL_SEC = 60        RETRY_TIMEOUT_SEC = 60      MAX_ATTEMPTS = 3
  MAX_PENDING = 64              MAX_TRACKED = 256           VOID_WINDOW_SEC = 180
      （VOID_WINDOW_SEC = RETRY_TIMEOUT_SEC * MAX_ATTEMPTS，重试预算耗尽即过期；ECS
      实测 Mem available 941MB / Swap used 535MB → 投递面零大常驻，缓存必须有界。）

  next_poll_delay(now_epoch: float) -> float
      每分钟轮询的等待秒数：对齐到下一个整分钟（0 < r <= 60），now_epoch <= 0 防御回 60。

  to_epoch_min(iso: str) -> int|None   /   parse_iso(iso) -> datetime|None
      ISO8601 带偏移解析（接受 Z / ±HH:MM）；非法 -> None（不得当成 0）。

  validate_alert_entry(entry) -> (ok: bool, reason: str)
      机械 schema 校验（V27 纪律在提醒载荷上）：dict；id 非空 str 且 <= 40 且字符集
      [0-9A-Za-z._-]；fire_at 可解析且带偏移；title 非空 str；speech 为 str（可为 ""）；
      view 为 dict 且**过真实 gateway.display_schema.validate_display**。畸形/缺必需字段
      -> (False, reason) —— 拒绝并给原因（审计面），**绝不盲转发**。

  read_pending(path, now_iso=None) -> (entries: list[dict], rejected: list[str])
      缺文件 -> ([], [])；整档非 JSON / alerts 非数组 -> ([], [reason])（不抛异常）；
      逐条校验，合法的进 entries、畸形的进 rejected（含 id 或原因）；超过 MAX_PENDING 的
      部分进 rejected（有界，不静默吃进内存）。

  due_entries(entries: list[dict], now_iso: str) -> list[dict]
      到点判定（机械校验通过且 fire_at <= now 的条目；闭区间 = 恰好的那一刻算到点）。
      非法载荷一律不算到点（校验前置于推送，不盲推）。

  alert_jsonrpc_id(business_id: str) -> int
      §三 业务字符串 id → 设备可接受的 JSON-RPC **数字** id（mcp_server.cc:546
      cJSON_IsNumber；1 <= v <= 2**31-1 以适配设备 int 解析）；确定性派生（同业务 id 跨
      重推/跨进程重启得同一数字），不同业务 id 不得碰撞（抽样断言）。

  build_alert_envelope(entry) -> dict
      {"jsonrpc":"2.0","id":<int>,"method":"tools/call",
       "params":{"name":"kidpal/alert","arguments":{"id":<str 业务 id>,"fire_at","title",
                                                   "speech","view":<json.dumps(view) 字符串>}}}
      —— 设备侧 mcp Property 只有 bool/int/str 三型，嵌套 view 必须字符串化承载（与
      kidpal/display 的 data 字符串化同纪律）；arguments.id 保留**字符串**业务 id 供易失
      去重；传输 id 数字化（本包从 mcp_relay.alert_tool_call 接管这件事）。

  build_speech_injection(entry) -> dict|None
      {"type": "kidpal_speak", "text": <speech>}；speech 缺失/空 -> None（降级=不注入，
      弹窗+提示音仍必达）。kidpal/alert 路径**永不**产生二进制帧。

  build_alert_frames(entry) -> (device_frames: list[dict], upstream_frames: list[dict])
      device_frames = [{"type":"mcp","payload":<envelope>}]（设备只把 type=mcp 帧送进
      McpServer，application.cc:492；多帧不批量）；upstream_frames = 语音注入指令（走
      **引擎腿**，front 不合成、不编码）。语音缺省时 upstream_frames 必须为 []。

  async deliver_alert(entry, device, upstream) -> dict
      真跑两腿投递：device.send_text(json) 逐帧下发 device_frames；upstream.send(json)
      注入 upstream_frames。返回 {"delivered": bool, "device_frames": n, "speech": bool}。
      **device.send_bytes 必须恒空**（裁定二红线）；speech 缺失时仍 delivered=True。

  class AlertDispatcher
      重试到确认 + 过期静默作废 + 幂等 + 有界 + 审计轨迹。
        __init__(sink: callable(id, envelope), max_attempts=MAX_ATTEMPTS,
                 retry_timeout_sec=RETRY_TIMEOUT_SEC, void_window_sec=VOID_WINDOW_SEC)
        tick(entries: list[dict], now_iso: str) -> list[str]      # 本分钟推送的 id
        on_receipt(rid) -> str|None                               # 业务 id 或数字 id 都收
        on_ws_connected() -> list[str]                            # 必须恒 []（不做重连即重推）
        state_of(bid) -> "unseen"|"pushing"|"acked"|"void"
        attempts(bid) -> int
        events -> list[dict]  {"id","action":push|ack|void|reject,"attempt","now"}
        tracked_count() -> int
      冻结语义：
        - 未到点不推；到点推第一次并把 attempt=1、next_retry_at = now + 60s；
        - 同一 now 再 tick 同一 id 不重复推（幂等，重算复用 id 不重开预算）；
        - 未收回执且 now >= next_retry_at 且 attempts < 3 → 重推（同一业务 id、同一数字 id）；
        - attempts 到 3 仍无回执 → state "void"，**静默作废不补发**（儿童产品不追催）；
        - 首次见到就已过 fire_at + VOID_WINDOW_SEC 的条目 → 直接 "void"，一次都不推
          （= 断网期间未送达档位在**网关推送时点**判定作废，V16/RMD-A7）；
        - 收到回执 → "acked" 出队，后续 tick 不再推；重复回执幂等无副作用；
        - 非法载荷（validate 失败）→ reject 事件 + 不推；
        - 内部记账有界：tracked_count() <= MAX_TRACKED（先淘汰已 acked/void 的）。

  consume_receipts(dispatcher, receipt_ids) -> list[str]
      把 front 泵收集到的设备回执喂给 dispatcher（**这条就是 g-relay-fix 留下的「acked_ids
      目前无人消费、未接出队方」缺口的接法**），返回被确认出队的业务 id。

Assembly teeth (structural, paired with the behavioral teeth above — 源码存在性本身不算
验收，这里只钉「装配到达」，行为由上面的纯逻辑齿承载):
  server/gateway/front/front_app.py 必须接线投递面：引用 deliver_alert( 、read_pending( 、
  consume_receipts( 与 next_poll_delay(（每分钟轮询到点推送的真循环落点）。
  引擎部署副本（server/deploy/engine-patch/connection.py）必须认得语音注入指令：引用
  kidpal_speak 与既有出音入口 speak_txt —— 真机出音由 live/9b 承载，此处只钉载体在世。

Live face (@pytest.mark.live, NOT auto-run — conftest 机械 skip 闸, ARQ-260901-g3；
且按 planner 2026-09-08 更正：文件内出现任一 live 标记即被 acceptance-run-check /
unit-run-check 的 fileHasLiveMark 整档排除出闸内 auto-run，所以**本包 server 侧「真跑过」
的证据只能由显式点名命令承载**：
  cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
）：真 cron + 真推送 + 真回执 + 真语义文案一轮的落盘证据断言，证据下
changes/dev/260829-schedule-reminder/（probe ≠ exercise —— 只探测 openclaw 可达不算 live
验收；必须真走一轮到点投递闭环）。五事实见类内 docstring。

Coverage map (acceptanceRef / exitCriteria / decisionCoverage -> test method):
  RMD-A1 structural (生产/投递两面装配 + 交接路径可配)
        -> test_a1_modules_loadable_and_handoff_path_configurable
  RMD-A2 (三档算术 + 区块范围 + 已过档跳过 + 幂等 id + pending.json)
        -> test_a2_three_ladder_arithmetic
        -> test_a2_block_scope_school_block_never_reminds
        -> test_a2_weekly_repeats_and_onetime_scope
        -> test_a2_past_ladders_skipped
        -> test_a2_idempotent_id_stable_across_recompute
        -> test_a2_ladder_times_come_from_config_not_code
        -> test_a2_pending_json_atomic_write
        -> test_a2_real_ecs_schedule_shape
        -> test_a2_live_cron_roundtrip_evidence (live)
  RMD-A3 (speech 指令构造 + 降级仍投递 + 不自建合成)
        -> test_a3_speech_injection_frame
        -> test_a3_speech_missing_still_delivers
        -> test_a3_alert_leg_emits_no_binary_frames
        -> test_a3_engine_patch_carrier_accepts_injection (structural carrier)
        -> test_a3_front_poll_injects_speech_into_engine_leg (tighten ADD 2026-09-08:
           离线跑 front 一轮投递周期，两腿条数对账钉「引擎腿注入真发生」+ 无 speech 对照；
           补 mutation sensitive:false 缺口 —— 详见文件尾 REVISION LOG)
        -> test_a3_live_tts_and_popup_timing_evidence (live, 真机音频时序殿后 9b)
  RMD-A4 (载荷形状 + 数字 id + 轮询到点 + 重试到确认 60s×3 + 过期作废 + 回执接出队)
        -> test_a4_alert_envelope_wire_shape
        -> test_a4_numeric_jsonrpc_id_deterministic_and_in_int_range
        -> test_a4_poll_alignment_and_due_predicate
        -> test_a4_retry_to_ack_60s_x3_then_void
        -> test_a4_receipt_dequeues_and_is_idempotent
        -> test_a4_reconnect_does_not_repump
        -> test_a4_same_id_repush_reuses_ids_and_budget
        -> test_a4_front_wires_delivery_and_receipt_dequeue (structural)
        -> test_a4_live_push_reaches_device_and_acks (live)
  RMD-A6 网关侧一半 (reject/审计事件 + 有界记账)
        -> test_a6_gateway_dedupe_audit_and_bounded_cache
  RMD-A7 (离线未送达档位推送时点作废 + 不做重连即重推)
        -> test_a4_retry_to_ack_60s_x3_then_void (void 分支)
        -> test_a7_expired_while_offline_is_voided_not_backfilled
        -> test_a4_reconnect_does_not_repump
        -> test_a7_live_offline_gap_voids_slot (live)
  RMD-A8 (携带内容走备注列 + 语义产物 schema + 约定文本承载 + B1 表体零变更)
        -> test_a8_bring_from_note_and_semantic_override
        -> test_a8_semantic_artifact_schema_and_degrade
        -> test_a8_prompt_carrier_v3_landed_and_wired (约定文本存在性, carrier 齿)
        -> test_a8_prompt_carrier_v3_reminder_section_anchors (tighten ADD 2026-09-08:
           carrier 齿收紧为「只认提醒面专有锚 + 段落级结构锚」，闭合 .v3 版本化槽位
           add/add 冲突下单边丢弃本包约定时的假绿 —— 详见文件尾 REVISION LOG)
        -> test_a8_b1_table_body_zero_change_reuses_package6_parser
  RMD-E2 (两面证据 + 既有冻结验收不回归)
        -> test_e2_existing_frozen_acceptance_suites_green
  RMD-E3 (范围红线：additive 扩展、消费包 6 数据链路、不重写 B1 解析)
        -> test_a8_b1_table_body_zero_change_reuses_package6_parser
        -> test_e3_no_http_alerts_endpoint_revival_and_no_data_duplication
  RMD-E4 (contractBlocked 解除记录)
        -> test_e4_contract_point3_evidenced_and_poll_form_retired
  boundary (错误/边界/权限路径 + 防假绿)
        -> test_boundary_malformed_payloads_rejected_not_forwarded
        -> test_boundary_bad_semantic_and_bad_config_inputs_degrade

decisionCoverage landing (taskpackage 260829-schedule-reminder):
  spec-决策20-提醒模型        -> RMD-A2 teeth (三档算术/区块范围/已过档跳过/幂等 id/配置来源)
  spec-决策27-提醒主动播报    -> RMD-A3 teeth (speech 指令构造 + 降级必达 + 引擎载体)
  contract-§三-推送语义       -> RMD-A4 teeth (载荷形状/数字 id/60s×3/静默作废/回执出队)
  spec-决策24-离线边界        -> RMD-A7 teeth + RMD-A6/A7 设备面 (C 孪生档 schedule_reminder_b)
  spec-S-F1.4-V18-滞留弹窗顶替 -> C 孪生档 RMD-A5 queue/顶替齿 (设备面行为；本档不重复钉)
  无 dropped / risk-accepted 条目。

Red baseline (freeze state, 2026-09-08 acceptance-author 实测): server/gateway 无
reminders 投递模块、server/deploy/openclaw-patch 无排程脚本、front 未接轮询投递与回执出队、
prompt 载体无 v3 —— 所有离线行为齿因「frozen offline contract 尚不存在」而红；收集零错误
（§3 冻结即冒烟 / §4 C 类实编：本档可编译可收集，红在行为缺失不在语法）。
既有 main 基线 = 8 failed / 64 passed / 35 skipped（7 条 smoke_e2e + 1 条 vrt 证据齿，
全落在 live-marked 文件内，属包 S 退役架构面）。本包判读口径 = 「8 条既有红不变 +
本包新增齿全红」。

Run protocol:
  cd server && python -m pytest -q --import-mode=importlib acceptance/test_schedule_reminder.acceptance.py
  （两点文件名需 --import-mode=importlib，G 包先例；live 类默认被 conftest 机械 skip，
   RUN_LIVE=1 / KRM_RUN_LIVE=1 或 -m live 显式点名才跑）
  standalone: python acceptance/test_schedule_reminder.acceptance.py
"""

from __future__ import annotations

import asyncio
import importlib.util
import json
import os
import re
import subprocess
import sys
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; the standalone runner also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
DEPLOY_DIR = SERVER / "deploy"
OPENCLAW_PATCH_DIR = DEPLOY_DIR / "openclaw-patch"
ENGINE_PATCH_DIR = DEPLOY_DIR / "engine-patch"

REMINDER_LIB = OPENCLAW_PATCH_DIR / "reminder_schedule_lib.py"
REMINDERS_MODULE = GATEWAY_DIR / "reminders.py"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
MCP_RELAY = GATEWAY_DIR / "mcp_relay.py"
SCHEDULE_MODULE = GATEWAY_DIR / "schedule.py"          # 包 6 producer（本包只消费不改）
LADDER_CONFIG = DEPLOY_DIR / "openclaw-reminder.config.yaml"
PROMPT_CARRIER_V3 = DEPLOY_DIR / "openclaw-system-prompt.v3.md"
LIVE_EVIDENCE_DIR = REPO_ROOT / "changes" / "dev" / "260829-schedule-reminder"

SPEECH_INJECTION_TYPE = "kidpal_speak"       # 裁定二冻结的内部指令类型字面
PENDING_NAME = "pending.json"

# --------------------------------------------------------------------------- #
# Frozen fixtures — 契约 §2.2 B1 定稿形状 + 外部世界真记录抽样（planner 2026-09-08     #
# ECS 只读实测：真 schedule.md 只有「每周固定课表」一节、时间列 19:00-20:00 形态、     #
# 备注列 7 行全空、frontmatter 仍带已废止 remind_before_min、无「学校课程表」也无      #
# 「一次性安排」区块）。两个 fixture 一个钉契约全形、一个钉实况缺区块。               #
# --------------------------------------------------------------------------- #

B1_CANONICAL = """---
child: 小明
timezone: Asia/Shanghai
remind_before_min: 15
---

# 小明的课表

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周三 | 19:00-20:00 | 英语培训 | 新东方 | |
| 周六 | 10:00-11:30 | 美术班 | 少年宫 | 带画笔 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-08-30 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |
| 2026-09-16 | 09:00 | 开学报到 | 学校 | 带暑假作业 |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 |
|------|-------|-------|-------|
| 周一 | 语文 | 数学 | 自习 |
| 周三 | 数学 | 语文 | 自习 |
"""

# 真 ECS 实况形状（549B 摘要的等价体）：只有每周固定课表、备注全空、废止字段仍在。
B1_REAL_ECS_SHAPE = """---
child: 小伴
timezone: Asia/Shanghai
remind_before_min: 15
---

# 小伴的课表

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 编程 |  |  |
| 周二 | 19:00-20:00 | 篮球 | 体育中心 |  |
| 周三 | 19:00-20:00 | 书法 |  |  |
| 周四 | 17:00-18:00 | 围棋 | 少年宫 |  |
| 周五 | 19:00-20:00 | 英语 |  |  |
| 周六 | 09:00-10:00 | 游泳 | 泳馆 |  |
| 周日 | 09:30-10:30 | 钢琴 | 家 |  |
"""

# 2026-09 星期对照（2026-09-07=周一 … 09-13=周日, 09-16=周三; datetime 实测核对）
MON, TUE, WED, FRI, SAT, SUN = (
    "2026-09-07", "2026-09-08", "2026-09-09", "2026-09-11", "2026-09-12", "2026-09-13")
NEXT_WED = "2026-09-16"

# 一个 §三 合法提醒条目（设备/网关两侧共用）
ALERT_CANONICAL = {
    "id": "rmd-3f2a1b9c4d5e6f70",
    "fire_at": "2026-09-09T16:15:00+08:00",
    "title": "钢琴课 16:30 开始前",
    "speech": "16:30 有钢琴课，别忘了带琴谱！",
    "view": {"view": "text", "data": {"text": "16:30 钢琴课 @琴行 · 带琴谱"}},
}


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _load_module_by_path(name: str, path: Path, label: str):
    """Offline import by path.  A missing/unimportable module is a RED for missing
    behaviour (self.fail at the call site), never a collection crash."""
    if not path.is_file():
        return None, (f"{path.relative_to(REPO_ROOT)} missing — {label} is not "
                      "implemented (frozen offline contract).")
    try:
        spec = importlib.util.spec_from_file_location(name, path)
        if spec is None or spec.loader is None:
            return None, f"{path} not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"{path.name} failed to import: {e!r}. Frozen contract: the module "
                      "MUST be pure python (no fastapi/gateway/network imports at import "
                      "time) so this acceptance runs offline anywhere.")


_rem_lib_cache: object = None
_gw_cache: object = None


def _rem_lib():
    global _rem_lib_cache
    if _rem_lib_cache is None:
        _rem_lib_cache = _load_module_by_path(
            "kidpal_reminder_schedule_lib", REMINDER_LIB,
            "OpenClaw 侧三档排程算术/幂等 id/pending.json 原子写脚本")
    return _rem_lib_cache


def _gw():
    global _gw_cache
    if _gw_cache is None:
        _gw_cache = _load_module_by_path(
            "kidpal_gateway_reminders", REMINDERS_MODULE,
            "网关提醒投递模块（每分钟轮询/重试到确认/过期静默作废/回执出队）")
    return _gw_cache


def _callable(mod, name: str):
    fn = getattr(mod, name, None)
    return fn if callable(fn) else None


def _ensure_fastapi_stub() -> None:
    """让 front_app 在无 fastapi 的机器上仍可离线装载（tighten ADD 2026-09-08 配套）。

    异常类必须是**真类**，否则 front 里的 `except (WebSocketDisconnect, RuntimeError)`
    在桩下失效；其余属性给宽容 Mock。（与 test_g_relay_fix.acceptance.py 同款桩，
    本档自带一份，不跨档依赖。）"""
    from types import ModuleType  # noqa: PLC0415
    from unittest.mock import Mock  # noqa: PLC0415
    try:
        import fastapi  # noqa: F401
        return
    except ImportError:
        pass

    class _Permissive(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    if "fastapi" not in sys.modules:
        stub = _Permissive("fastapi")
        stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
        stub.HTTPException = type("HTTPException", (Exception,), {})
        sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _Permissive("fastapi.responses")


_front_cache: object = None


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline.

    Returns (module|None, err).  Missing/unimportable = a RED for missing behaviour
    (self.fail at the call site), never a collection crash."""
    global _front_cache
    if _front_cache is not None:
        return _front_cache
    if not FRONT_APP.is_file():
        return None, ("server/gateway/front/front_app.py missing — 网关 ws 代理/提醒投递"
                      "接线面不在世。")
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_rmd", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_cache = (mod, None)
        return _front_cache
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须"
                      "导入期零网络 I/O，验收可离线装载。")


def _sched():
    """包 6 的真实 B1 解析器（本包的课表数据 producer 消费面）。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.schedule as sc  # noqa: PLC0415
    return sc


def _display_schema():
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415
    return ds


class _TmpDir:
    def __enter__(self):
        import tempfile  # noqa: PLC0415
        self._path = tempfile.mkdtemp(prefix="krm_rmd_")
        return Path(self._path)

    def __exit__(self, *exc):
        import shutil  # noqa: PLC0415
        shutil.rmtree(self._path, ignore_errors=True)
        return False


class _FakeDevice:
    """Device ws leg double — send_text/send_bytes both recorded (the send_bytes
    emptiness is the 裁定二 machine-judgable red line)."""

    def __init__(self):
        self.sent_text = []
        self.sent_bytes = []

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(data)


class _FakeUpstream:
    """Engine ws leg double (the injection target of 裁定二)."""

    def __init__(self):
        self.sent = []

    async def send(self, message):
        self.sent.append(message)


def _parsed(frames):
    out = []
    for f in frames:
        try:
            out.append(json.loads(f) if isinstance(f, str) else f)
        except (TypeError, ValueError):
            continue
    return out


# --------------------------------------------------------------------------- #
# Offline structural / behavioral acceptance — the red baseline teeth           #
# --------------------------------------------------------------------------- #

class ScheduleReminderOfflineAcceptance(unittest.TestCase):
    """Offline teeth on the two frozen modules + assembly guards + regression guards."""

    maxDiff = None

    # --- RMD-A1 --------------------------------------------------------------
    def test_a1_modules_loadable_and_handoff_path_configurable(self):
        """coverage: RMD-A1 — 生产/投递两模块可离线装载；交接路径可 env 配置（ECS 实况：
        alerts/ 目录当前不存在，路径由部署配置给）。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A1 RED: " + err)
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A1 RED: " + err)
        for name in ("plan_reminders", "reminder_id", "write_pending_atomic",
                     "load_ladder_config", "load_semantic", "bring_items"):
            self.assertTrue(callable(_callable(lib, name)),
                            f"RMD-A1 RED: reminder_schedule_lib.{name}() missing (frozen contract).")
        for name in ("read_pending", "validate_alert_entry", "build_alert_envelope",
                     "build_alert_frames", "deliver_alert", "consume_receipts",
                     "next_poll_delay", "alert_jsonrpc_id"):
            self.assertTrue(callable(_callable(gw, name)),
                            f"RMD-A1 RED: gateway/reminders.py.{name}() missing (frozen contract).")
        self.assertTrue(hasattr(gw, "AlertDispatcher"),
                        "RMD-A1 RED: gateway/reminders.py has no AlertDispatcher — 重试到确认"
                        "与过期作废的状态机没有承载体。")
        # 交接路径可配置（不得把 /root/... 硬编死到不可改）
        pending_path_fn = _callable(gw, "pending_path")
        self.assertTrue(callable(pending_path_fn),
                        "RMD-A1 RED: gateway/reminders.py has no pending_path() — "
                        "alerts/pending.json 交接路径必须由环境变量/配置给（ECS 上该目录"
                        "当前不存在，硬编绝对路径 = 部署即崩）。")
        with_env = {
            "KIDPAL_ALERTS_DIR": "/tmp/krm-handoff",
            "KIDPAL_PENDING_JSON": "/tmp/krm-handoff/pending.json",
        }
        old = {k: os.environ.get(k) for k in with_env}
        try:
            os.environ.update(with_env)
            p = str(pending_path_fn())
            self.assertTrue(p.endswith(PENDING_NAME) or PENDING_NAME in p,
                            f"RMD-A1 RED: pending_path() must point at {PENDING_NAME}: {p!r}")
            self.assertIn("krm-handoff", p,
                          f"RMD-A1 RED: pending_path() ignores the configured alerts dir: {p!r}")
        finally:
            for k, v in old.items():
                if v is None:
                    os.environ.pop(k, None)
                else:
                    os.environ[k] = v

    # --- RMD-A2: 三档算术 ----------------------------------------------------
    def test_a2_three_ladder_arithmetic(self):
        """coverage: RMD-A2 / spec-决策20 — 时间串解析、跨天映射（前晚档 = 事件日前一天）、
        事前 15 分钟减法；三档时刻与 fire_at 逐档精确。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        # 参考点 now = 2026-09-08T07:00+08（**周二**，日历真值：09-07=周一、09-08=周二）。
        # 钢琴课是「周一 16:30」，其 09-07 那一周次的前晚/早/临期三档相对 now 全部已过去，
        # 按 spec §4.1「已过去的档位自动跳过」+ 决策 20 每周重复，本齿期望的是**下一次
        # occurrence（周一 2026-09-14）**的三档簇。★不要把它改回 09-06/09-07：那会与同档
        # test_a2_past_ladders_skipped（已过档位一个都不发）直接互斥，两齿无共同解。
        # 本齿真正钉的三条关系与周次无关：前晚档 = **事件日前一天** 20:30（09-13 是 09-14 的
        # 前一天）、早档 = 事件日当天 07:45、临期档 = 事件开始 16:30 - 15min = 16:15。
        entries = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")
        self.assertIsInstance(entries, list, "RMD-A2 RED: plan_reminders must return a list.")
        by = {}
        for e in entries:
            self.assertEqual(
                set(e), {"id", "fire_at", "title", "speech", "view"},
                f"RMD-A2 RED: 契约 §三 载荷键漂移: {sorted(e)} — 零增减红线。")
            by[e["id"]] = e
        self.assertEqual(len(by), len(entries),
                         "RMD-A2 RED: 一次排程内出现重复 id — 三档/多条目的散列维度不足，"
                         "设备幂等去重会把不同提醒合并成一条。")
        flat = sorted(e["fire_at"] for e in entries)
        self.assertTrue(flat, "RMD-A2 RED: no reminder planned for a week that has 钢琴课(周一) "
                              "and 开学报到(周三 09-16)? — 见逐档断言。")
        # 周一 16:30-17:30 钢琴课 · 下一次 occurrence(09-14)：三档 = 09-13 20:30 /
        # 09-14 07:45 / 09-14 16:15
        piano = [e for e in entries if "钢琴课" in e["title"]]
        self.assertEqual(
            sorted(e["fire_at"] for e in piano),
            ["2026-09-13T20:30:00+08:00", "2026-09-14T07:45:00+08:00",
             "2026-09-14T16:15:00+08:00"],
            f"RMD-A2 RED: 钢琴课(周一 16:30 · next occurrence 09-14) 三档时刻错: "
            f"{[e['fire_at'] for e in piano]!r} — "
            "前晚档必须映射到**事件日前一天** 20:30、早档当天 07:45、临期档 16:30-15min=16:15。")
        # 周三 19:00 英语培训：soon 档 = 18:45（跨小时的减法）
        english = [e for e in entries if "英语培训" in e["title"]]
        self.assertIn("2026-09-09T18:45:00+08:00", [e["fire_at"] for e in english],
                      f"RMD-A2 RED: 英语培训(周三 19:00) 缺 18:45 临期档: "
                      f"{[e['fire_at'] for e in english]!r} — 事前 15 分钟减法错。")
        self.assertIn("2026-09-08T20:30:00+08:00", [e["fire_at"] for e in english],
                      "RMD-A2 RED: 英语培训缺前一天(09-08 周二) 20:30 前晚档 — 跨天映射缺失。")
        # 周六 10:00 美术班：soon = 09:45，前晚 = 周五 20:30（周末班同样每周重复生成）
        art = [e for e in entries if "美术班" in e["title"]]
        self.assertEqual(
            sorted(e["fire_at"] for e in art),
            ["2026-09-11T20:30:00+08:00", "2026-09-12T07:45:00+08:00",
             "2026-09-12T09:45:00+08:00"],
            f"RMD-A2 RED: 周六美术班三档错: {[e['fire_at'] for e in art]!r} — 周末培训班"
            "必须照三段阶梯生成（决策 20：周末班打破平环节奏最容易漏）。")

    def test_a2_block_scope_school_block_never_reminds(self):
        """coverage: RMD-A2 / spec-决策20 — 按区块判范围：「学校课程表」区块**零提醒**。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        self.assertTrue(model.get("school"), "RMD-A2 RED fixture: 学校课程表区块未被解析出来")
        entries = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")
        corpus = json.dumps(entries, ensure_ascii=False)
        for course in ("语文", "数学", "自习"):
            self.assertNotIn(
                course, corpus,
                f"RMD-A2 RED: 学校课程表科目「{course}」出现在提醒计划里 — 在校例行课不提醒"
                "（决策 20 第三轮 grilling 按区块判范围）。")
        self.assertTrue(any("钢琴课" in e["title"] for e in entries),
                        "RMD-A2 RED: 负向齿过宽——培训班区块被一起屏蔽了（应只屏蔽学校区块）。")

    def test_a2_weekly_repeats_and_onetime_scope(self):
        """coverage: RMD-A2 — 每周固定课表按周重复生成；一次性安排只在 date 精确匹配、
        过期一次性安排不生成（契约 §2.3）。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        # horizon 14 天：钢琴课(周一) 应出现 >= 2 次事件日（每周重复），不是只一次
        entries = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00", horizon_days=14)
        dates = sorted({e["fire_at"][:10] for e in entries if "钢琴课" in e["title"]})
        self.assertGreaterEqual(
            len({d for d in dates if d in ("2026-09-06", "2026-09-07", "2026-09-13", "2026-09-14")}),
            2,
            f"RMD-A2 RED: 每周固定课表没有按周重复生成: {dates!r} — 下一周(09-13/09-14) 也应有档。")
        # 一次性安排：09-16 开学报到 有档；已过去的 08-30 牙医复诊 永不出现
        names = {e["title"] for e in entries}
        self.assertTrue(any("开学报到" in t for t in names),
                        f"RMD-A2 RED: 未来一次性安排(2026-09-16 开学报到) 未生成提醒: {sorted(names)!r}")
        self.assertFalse(any("牙医" in t for t in names),
                         "RMD-A2 RED: 过期一次性安排(2026-08-30 牙医复诊) 仍在生成提醒 — "
                         "契约 §2.3 过期自动忽略缺失（会给孩子推半年前的旧事）。")

    def test_a2_past_ladders_skipped(self):
        """coverage: RMD-A2 — 已过去档位自动跳过（当晚才录入「明天」的事，前晚档不发）。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        # 周一 16:30 钢琴课：now = 09-07T19:00（当天三档全过）-> 钢琴课零档
        late = [e for e in lib.plan_reminders(model, "2026-09-07T19:00:00+08:00")
                if "钢琴课" in e["title"]]
        self.assertEqual(
            [e["fire_at"] for e in late if e["fire_at"] <= "2026-09-07T19:30"], [],
            f"RMD-A2 RED: 已过去的档位没有跳过: {[e['fire_at'] for e in late]!r} — "
            "spec 明写「已过去的档位自动跳过」（如当晚才录入明天的事，前晚档不发）。")
        # now = 09-07T07:50（早档已过 07:45、临期档 16:15 未到）-> 只剩临期档
        midday = [e for e in lib.plan_reminders(model, "2026-09-07T07:50:00+08:00")
                  if "钢琴课" in e["title"] and e["fire_at"].startswith("2026-09-07")]
        self.assertEqual([e["fire_at"] for e in midday], ["2026-09-07T16:15:00+08:00"],
                         f"RMD-A2 RED: 部分已过时应只留未过的档: {[e['fire_at'] for e in midday]!r}")

    def test_a2_idempotent_id_stable_across_recompute(self):
        """coverage: RMD-A2 / V15 — 幂等 id = 确定性散列(条目+档位+事件日期)，跨重算/重推
        不变；重算复用既有 id 不重生成。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        # 同一 plan 在不同 now / 不同进程时刻下重算 -> id 集合完全一致
        a = {e["id"] for e in lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")}
        b = {e["id"] for e in lib.plan_reminders(model, "2026-09-08T07:01:00+08:00")}
        self.assertTrue(a, "RMD-A2 RED: empty plan in the id-stability tooth.")
        self.assertEqual(a, b, "RMD-A2 RED: 重算生成了不同的 id 集合 — 幂等 id 掺入了 now/"
                               "随机数，网关重推与设备去重会双双失效。")
        # 三个维度各自独立：同一条目不同档位 id 不同；不同日期同档位不同；不同条目不同
        cfg = lib.DEFAULT_LADDER_CONFIG
        k1 = lib.entry_key("weekly", {"weekday": 3, "start": "19:00", "end": "20:00",
                                      "name": "英语培训", "location": "新东方"})
        k2 = lib.entry_key("weekly", {"weekday": 1, "start": "16:30", "end": "17:30",
                                      "name": "钢琴课", "location": "琴行"})
        i_night = lib.reminder_id(k1, lib.LADDER_NIGHT, WED)
        i_morn = lib.reminder_id(k1, lib.LADDER_MORNING, WED)
        i_other = lib.reminder_id(k2, lib.LADDER_NIGHT, WED)
        i_next = lib.reminder_id(k1, lib.LADDER_NIGHT, NEXT_WED)
        self.assertEqual({i_night, i_morn, i_other, i_next},
                         {i_night, i_morn, i_other, i_next},
                         "RMD-A2 RED: id 维度塌缩（条目/档位/事件日期必须共同参与散列）。")
        for v in (i_night, i_morn, i_other, i_next):
            self.assertTrue(re.fullmatch(r"[0-9A-Za-z._-]{1,40}", v),
                            f"RMD-A2 RED: id {v!r} 不是设备侧可承载的 ASCII token 或超出 40 字节")
        self.assertEqual(len({i_night, i_morn, i_other, i_next}), 4,
                         "RMD-A2 RED: 条目/档位/日期三维度中至少一个没有进入散列 — "
                         "碰撞会让第二条提醒被第一条的去重屏蔽吃掉（丢提醒）。")
        # 配置改动只改时刻、不改 id（重算复用）
        c2 = dict(cfg)
        c2["night_at"] = "21:00"
        p1 = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00", config=cfg)
        p2 = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00", config=c2)
        m1 = {e["id"]: e["fire_at"] for e in p1}
        m2 = {e["id"]: e["fire_at"] for e in p2}
        self.assertEqual(set(m1), set(m2), "RMD-A2 RED: 改配置时刻导致 id 集合变化 — "
                                           "改时刻即时生效但幂等 id 不得重生成。")
        self.assertTrue(any(m1[i] != m2[i] for i in m1),
                        "RMD-A2 RED: 改 night_at 没有改变任何 fire_at — 时刻没有从配置读。")

    def test_a2_ladder_times_come_from_config_not_code(self):
        """coverage: RMD-A2 / spec-决策20 — 三档时刻从 OpenClaw 侧提醒配置读、随配置变。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        with _TmpDir() as d:
            cfgp = d / "openclaw-reminder.config.yaml"
            cfgp.write_text("# 三档提醒时刻（纯服务端配置）\nnight_at: 19:45\n"
                            "morning_at: 06:30\nbefore_min: 30\n", encoding="utf-8")
            cfg = lib.load_ladder_config(cfgp)
            self.assertEqual(cfg.get("night_at"), "19:45",
                             f"RMD-A2 RED: 配置未生效: {cfg!r} — night_at 必须从载体读。")
            self.assertEqual(cfg.get("before_min"), 30,
                             f"RMD-A2 RED: before_min 必须是 int 且从载体读: {cfg!r}")
            piano = [e for e in lib.plan_reminders(model, "2026-09-08T07:00:00+08:00", config=cfg)
                     if "钢琴课" in e["title"]]
            # 钢琴课 next occurrence = 周一 09-14（09-07 那一周次三档全已过并跳过，理由见
            # test_a2_three_ladder_arithmetic 注记）：前晚 09-13T19:45 / 当天 06:30 /
            # 临期 16:30-30min = 16:00。
            self.assertEqual(
                sorted(e["fire_at"] for e in piano),
                ["2026-09-13T19:45:00+08:00", "2026-09-14T06:30:00+08:00",
                 "2026-09-14T16:00:00+08:00"],
                f"RMD-A2 RED: 配置改成 19:45/06:30/30min 后排程没跟着变: "
                f"{[e['fire_at'] for e in piano]!r} — 「改时刻即时生效」不成立。")
        # 缺配置载体不得停摆（回退默认，与 spec 常量一致）
        missing = lib.load_ladder_config(REPO_ROOT / "no" / "such" / "cfg.yaml")
        self.assertEqual(missing.get("night_at"), "20:30")
        self.assertEqual(missing.get("morning_at"), "07:45")
        self.assertEqual(missing.get("before_min"), 15)
        # 随包部署的载体本身必须是三档常量的家（验收所测 = 生产所用）
        self.assertTrue(LADDER_CONFIG.is_file(),
                        "RMD-A2 RED: server/deploy/openclaw-reminder.config.yaml missing — "
                        "三档时刻没有随 server/deploy 上 ECS 的配置载体（spec：配在 OpenClaw 侧）。")
        shipped = lib.load_ladder_config(LADDER_CONFIG)
        self.assertEqual((shipped.get("night_at"), shipped.get("morning_at"),
                          shipped.get("before_min")), ("20:30", "07:45", 15),
                         f"RMD-A2 RED: 部署载体默认三档 != spec 常量: {shipped!r}")
        wiring = "\n".join(p.read_text(encoding="utf-8", errors="replace")
                           for p in sorted(DEPLOY_DIR.rglob("*")) if p.is_file())
        self.assertIn("openclaw-reminder.config.yaml", wiring,
                      "RMD-A2 RED: server/deploy 的渲染/装载脚本从未引用 openclaw-reminder.config.yaml"
                      " — 配置载体进了仓但没上 ECS（验收所测 != 生产所用）。")

    def test_a2_pending_json_atomic_write(self):
        """coverage: RMD-A2 / contract-§三 — pending.json 真落盘（真实持久化副作用）、
        原子写无 .tmp 残留、容器 schema 稳定、写后读回一致、重算 alerts 逐字节相同。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        plan = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")
        self.assertTrue(plan, "RMD-A2 RED fixture: empty plan.")
        with _TmpDir() as d:
            sub = d / "alerts"
            sub.mkdir()
            path = sub / PENDING_NAME
            lib.write_pending_atomic(path, plan)
            self.assertTrue(path.is_file() and path.stat().st_size > 0,
                            "RMD-A2 RED: pending.json 没有真落盘（原子写未执行）。")
            residue = [p.name for p in d.rglob("*") if p.is_file() and p.suffix == ".tmp"]
            self.assertEqual(residue, [],
                             f"RMD-A2 RED: 原子写后残留临时文件 {residue!r} — 网关可能读到半成品。")
            raw = json.loads(path.read_text(encoding="utf-8"))
            self.assertIsInstance(raw, dict, "RMD-A2 RED: pending.json 顶层必须是对象容器。")
            self.assertIsInstance(raw.get("alerts"), list,
                                  "RMD-A2 RED: pending.json 缺 alerts 数组容器。")
            self.assertEqual([e["id"] for e in raw["alerts"]],
                             sorted(e["id"] for e in raw["alerts"]),
                             "RMD-A2 RED: alerts 未按 id 升序 — 重算写入不稳定会制造无意义 diff。")
            # 网关读回：全部合法、零拒绝，且与生产侧 plan 等价（交接两侧对齐）
            got, rejected = gw.read_pending(path)
            self.assertEqual(rejected, [],
                             f"RMD-A2 RED: 生产侧写出的条目被网关机械校验拒绝: {rejected!r}")
            self.assertEqual(sorted(e["id"] for e in got), sorted(e["id"] for e in plan),
                             "RMD-A2 RED: 写后读回条目集不一致（交接面丢项）。")
            # 重算再写：alerts 数组逐字节相同（generated_at 不参与比较、也不被消费）
            lib.write_pending_atomic(path, lib.plan_reminders(model, "2026-09-08T07:05:00+08:00"))
            raw2 = json.loads(path.read_text(encoding="utf-8"))
            self.assertEqual(json.dumps(raw["alerts"], sort_keys=True, ensure_ascii=False),
                             json.dumps(raw2["alerts"], sort_keys=True, ensure_ascii=False),
                             "RMD-A2 RED: 同一周重算写出的 alerts 不稳定 — 幂等复用未贯穿到文件面。")

    def test_a2_real_ecs_schedule_shape(self):
        """coverage: RMD-A2 / RMD-A8 — 外部世界真形状（planner 2026-09-08 ECS 只读实测）：
        只有「每周固定课表」区块、备注列全空、frontmatter 仍带已废止 remind_before_min、
        无一次性/学校区块 -> 缺区块不崩、废止字段不消费、三档只从配置读。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A2 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_REAL_ECS_SHAPE)
        self.assertEqual(model.get("onetime"), [], "RMD-A2 RED fixture: 缺区块应解析为空列表")
        self.assertEqual(model.get("school"), [], "RMD-A2 RED fixture: 缺学校区块应为空列表")
        try:
            entries = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")
        except Exception as e:  # noqa: BLE001
            self.fail(f"RMD-A2 RED: 真 schedule.md 形状（缺两个区块）导致排程抛异常: {e!r} — "
                      "实况文件就是这个形状，缺区块必须正常产出。")
        self.assertTrue(entries, "RMD-A2 RED: 真 ECS 形状（7 行每周课表）没排出任一提醒。")
        self.assertTrue(all("带" not in e["title"] for e in entries),
                        "RMD-A2 RED: 备注全空却被编出「带 X」文案 — 语义不得凭空发明（RMD-A8）。")
        # remind_before_min 已废止：改它的值不得改变任何 fire_at（提前量只从配置读）
        mutated = sc.parse_schedule_md(B1_REAL_ECS_SHAPE.replace("remind_before_min: 15",
                                                                 "remind_before_min: 9999"))
        base = {e["id"]: e["fire_at"] for e in lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")}
        alt = {e["id"]: e["fire_at"] for e in lib.plan_reminders(mutated, "2026-09-08T07:00:00+08:00")}
        self.assertEqual(base, alt,
                         "RMD-A8 RED: remind_before_min 参与了排程计算 — 该字段已废止不消费"
                         "（契约 §2.3，2026-09-03 契约门裁定），提前量必须来自 OpenClaw 配置。")

    # --- RMD-A3 --------------------------------------------------------------
    def test_a3_speech_injection_frame(self):
        """coverage: RMD-A3 / spec-决策27 — 语音借引擎既有音频链：到点向**引擎腿**注入
        kidpal_speak 内部指令，文案逐字进指令、设备腿只见 alert mcp 帧。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A3 RED: " + err)
        frame = gw.build_speech_injection(ALERT_CANONICAL)
        self.assertIsInstance(frame, dict, "RMD-A3 RED: 有 speech 时必须给出注入指令。")
        self.assertEqual(frame.get("type"), SPEECH_INJECTION_TYPE,
                         f"RMD-A3 RED: 注入指令类型 {frame.get('type')!r} != {SPEECH_INJECTION_TYPE!r}"
                         "（裁定二冻结字面：引擎部署副本按该类型识别并调 speak_txt）。")
        self.assertEqual(frame.get("text"), ALERT_CANONICAL["speech"],
                         "RMD-A3 RED: 注入指令没带上 §三 speech 原文 — TTS 读文案的来源丢失。")
        device_frames, upstream_frames = gw.build_alert_frames(ALERT_CANONICAL)
        self.assertTrue(any(str(f.get("text", "")) == ALERT_CANONICAL["speech"]
                            for f in upstream_frames),
                        "RMD-A3 RED: 语音指令没有走向**引擎腿**（upstream）— 裁定二：借引擎"
                        "现成入口出音，front 不自建合成。")
        self.assertFalse(any(f.get("type") == SPEECH_INJECTION_TYPE for f in device_frames),
                         f"RMD-A3 RED: {SPEECH_INJECTION_TYPE} 内部指令被发到设备腿 — 设备会按"
                         " Unknown message type 丢弃（application.cc:532），且引擎内部面不得外泄。")

    def test_a3_speech_missing_still_delivers(self):
        """coverage: RMD-A3 / V20 — 合成失败/缺文案 -> 无注入指令，但弹窗投递照常（必达面）。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A3 RED: " + err)
        empty_speech = {k: v for k, v in ALERT_CANONICAL.items() if k != "speech"}
        empty_speech["speech"] = ""
        no_speech_key = {k: v for k, v in ALERT_CANONICAL.items() if k != "speech"}
        for entry, label in ((empty_speech, "空 speech"), (no_speech_key, "缺 speech 键")):
            self.assertIsNone(gw.build_speech_injection(entry),
                              f"RMD-A3 RED: {label} 仍生成注入指令 — 应降级为「仅弹窗+提示音」。")
            device_frames, upstream_frames = gw.build_alert_frames(entry)
            self.assertEqual(len(device_frames), 1,
                             f"RMD-A3 RED: {label} 时弹窗投递帧数 {len(device_frames)} != 1 — "
                             "语音失败不得阻塞弹窗必达。")
            self.assertEqual(upstream_frames, [],
                             f"RMD-A3 RED: {label} 仍向引擎腿注入 — 不阻塞不等于照常注入空文案。")

    def test_a3_alert_leg_emits_no_binary_frames(self):
        """coverage: RMD-A3 / 裁定二 — front 不自己注入二进制音频帧：投递一轮真跑双假 socket，
        设备侧 send_bytes 必须恒空，文本帧恰为 mcp 帧 + 引擎侧恰一条注入。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A3 RED: " + err)
        device, engine = _FakeDevice(), _FakeUpstream()
        result = asyncio.run(gw.deliver_alert(ALERT_CANONICAL, device, engine))
        self.assertEqual(device.sent_bytes, [],
                         "RMD-A3 RED: 设备腿出现 send_bytes —— 裁定二红线：front 不做 mp3→opus"
                         " 编码、不自己注入二进制音频帧，出音走引擎既有下行链。")
        df = _parsed(device.sent_text)
        mcp_alerts = [f for f in df if isinstance(f, dict) and f.get("type") == "mcp"
                      and isinstance(f.get("payload"), dict)
                      and (f["payload"].get("params") or {}).get("name") == "kidpal/alert"]
        self.assertEqual(len(mcp_alerts), 1,
                         f"RMD-A3 RED: 设备侧 kidpal/alert mcp 帧应恰 1 条, got {df!r} — "
                         "载荷必须以 {\"type\":\"mcp\",\"payload\":…} 承载（application.cc:492）。")
        uf = _parsed([m if isinstance(m, str) else json.dumps(m, ensure_ascii=False)
                      for m in engine.sent])
        self.assertEqual(len([f for f in uf if isinstance(f, dict)
                              and f.get("type") == SPEECH_INJECTION_TYPE]), 1,
                         f"RMD-A3 RED: 引擎腿注入指令应恰 1 条, got {uf!r}")
        self.assertTrue(isinstance(result, dict) and result.get("delivered") is True,
                        f"RMD-A3 RED: deliver_alert 返回值缺 delivered 事实: {result!r} — "
                        "投递结果必须可被上层判定（否则重试逻辑无处记账）。")

    def test_a3_engine_patch_carrier_accepts_injection(self):
        """coverage: RMD-A3 structural carrier — 引擎部署副本认得语音注入指令并接既有出音入口。
        只钉载体在世（行为面 = 真机出音，由 live 齿与 9b 殿后承载，不在离线冒充已验）。"""
        conn = _read_text(ENGINE_PATCH_DIR / "connection.py")
        self.assertIn(SPEECH_INJECTION_TYPE, conn,
                      f"RMD-A3 RED: server/deploy/engine-patch/connection.py 未引用 "
                      f"{SPEECH_INJECTION_TYPE!r} — front 注入的内部指令在引擎侧无人认领"
                      "（裁定二：借引擎既有音频链，需在 _route_message 一类的入口接住）。")
        self.assertIn("speak_txt", conn,
                      "RMD-A3 RED: 引擎补丁未引用既有出音入口 speak_txt"
                      "（core/handle/intentHandler.py:233 speak_txt(conn, text)）——"
                      "「复用现成 edge-tts + opus 下行链」没有落点。")

    # --- RMD-A3 tighten ADD: front→引擎腿注入的离线行为齿（mutation 暴露的覆盖缺口） ----
    def test_a3_front_poll_injects_speech_into_engine_leg(self):
        """coverage: RMD-A3 — 真跑 front 的**一轮投递周期**（run_alert_poll_once）+ 假设备腿
        与假引擎腿双 sink，断言「有 speech 的档位 → 引擎腿收到恰一条 kidpal_speak 注入、
        text 非空且等于载荷 speech、注入条数与本周期推送条数相等」；对照「无 speech 的档位 →
        设备腿仍必达一条 mcp 帧、引擎腿零注入」。

        tighten ADD 理由（2026-09-08 mutation 证据 sensitive:false）：变异把
        front_app._deliver_alert_now 的 deliver_alert(entry, device_ws, engine_ws) 改成
        (…, device_ws, None)（语音永不外发）时，本冻结档原先只有数源码字符串的 needle 齿
        （test_a4_front_wires_delivery_and_receipt_dequeue）不敏感 —— 只有从**两腿条数对账**
        才看得见这条腿静默断开。钉的仍是 RMD-A3 与裁定二已声明的意图（借引擎既有音频链、
        弹窗必达、语音是引擎腿注入），不新增行为定义。
        边界：不断言任何音频/真机时序（那是 test_a3_live_tts_* + 9b 殿后面）；不断言 front
        内部私有符号形状 —— 断言全部落在我方假腿收到的**对端可观察帧**上（_register_alert_legs
        / _unregister_alert_legs 只用作接线 setup，并在 finally 里注销避免污染兄弟齿）。
        """
        from datetime import datetime  # noqa: PLC0415  局部导入，不改本档既有 import 面
        front, err = _load_front_app()
        if front is None:
            self.fail("RMD-A3 RED: " + err)
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A3 RED: " + err)

        epoch = 1788941700                        # 2026-09-09T16:15:00+08:00
        # 用与 front 内部同一个表达式生成 now_iso，使 fire_at == now（闭区间算到点、
        # 且绝不落在 VOID_WINDOW 之外），任何本机时区下都成立。
        now_iso = datetime.fromtimestamp(epoch).astimezone().isoformat(timespec="seconds")
        speech_text = "16:30 有钢琴课，别忘了带琴谱！"
        with_speech = {"id": "rmd-front-a", "fire_at": now_iso, "title": "钢琴课 16:30 开始前",
                       "speech": speech_text,
                       "view": {"view": "text", "data": {"text": "16:30 钢琴课 @琴行"}}}
        no_speech = {"id": "rmd-front-b", "fire_at": now_iso, "title": "美术班 10:00 开始前",
                     "speech": "",
                     "view": {"view": "text", "data": {"text": "10:00 美术班 @少年宫"}}}
        for entry in (with_speech, no_speech):
            ok, reason = gw.validate_alert_entry(entry)
            self.assertTrue(ok, f"RMD-A3 RED fixture: 探针条目未过本包 §三 机械校验 ({reason})")

        def _mcp_alert_frames(sent_text):
            out = []
            for f in _parsed(sent_text):
                payload = f.get("payload") if isinstance(f, dict) else None
                if (isinstance(f, dict) and f.get("type") == "mcp" and isinstance(payload, dict)
                        and (payload.get("params") or {}).get("name") == "kidpal/alert"):
                    out.append(payload)
            return out

        def _injection_frames(sent):
            return [f for f in _parsed(sent) if isinstance(f, dict)
                    and f.get("type") == SPEECH_INJECTION_TYPE]

        async def _one_cycle(entry):
            """一轮周期：写 pending -> 登记两条假腿 -> 跑 run_alert_poll_once -> 收帧。"""
            with _TmpDir() as d:
                path = d / PENDING_NAME
                path.write_text(json.dumps({"version": 1, "generated_at": now_iso,
                                            "alerts": [entry]}, ensure_ascii=False),
                                encoding="utf-8")
                device, engine = _FakeDevice(), _FakeUpstream()
                old = os.environ.get("KIDPAL_PENDING_JSON")
                os.environ["KIDPAL_PENDING_JSON"] = str(path)
                front._register_alert_legs(device, engine)
                try:
                    res = await front.run_alert_poll_once(now_epoch=epoch)
                    for _ in range(24):   # 放 sink 内 create_task 的投递任务跑完（无睡眠、确定性）
                        await asyncio.sleep(0)
                finally:
                    front._unregister_alert_legs(device)
                    if old is None:
                        os.environ.pop("KIDPAL_PENDING_JSON", None)
                    else:
                        os.environ["KIDPAL_PENDING_JSON"] = old
                return res, device, engine

        # ============ 有 speech：引擎腿必须真收到注入（断这条腿 = 本齿的存在理由）========
        res, device, engine = asyncio.run(_one_cycle(with_speech))
        self.assertEqual(res["pushed"], [with_speech["id"]],
                         f"RMD-A3 RED: 本轮未把到点档位推出去 (pushed={res['pushed']!r}) — "
                         "两腿对账的前置不成立。")
        alerts = _mcp_alert_frames(device.sent_text)
        injections = _injection_frames(engine.sent)
        self.assertEqual(len(alerts), 1,
                         f"RMD-A3 RED: 设备腿 kidpal/alert mcp 帧数 {len(alerts)} != 1 — "
                         "弹窗必达面（RMD-A3/V20）不成立。")
        self.assertEqual(len(injections), len(res["pushed"]),
         "RMD-A3 RED: 引擎腿注入条数 != 本周期推送条数 —— front→引擎腿这一腿静默断开：语音"
                         "永不外发，孩子不看屏也能听得到的那条链就断了（裁定二「借引擎既有"
                         "音频链」的落点）。needle 型齿看不见这个缺陷，只有两腿条数对账能看见。")
        self.assertEqual(len(injections), 1,
                         f"RMD-A3 RED: 引擎腿注入帧数 {len(injections)} != 1: {engine.sent!r}")
        self.assertEqual(injections[0].get("text"), speech_text,
                         f"RMD-A3 RED: 注入指令的 text 不等于载荷 speech 原文: "
                         f"{injections[0]!r} — TTS 读文案的来源被改写。")
        self.assertTrue(str(injections[0].get("text", "")).strip(),
                        "RMD-A3 RED: 注入指令 text 为空（空文案外发 = 播报面静默失效）。")
        self.assertEqual(alerts[0]["params"]["arguments"]["id"], with_speech["id"],
                         "RMD-A3 RED(infra): 设备帧的 §三 业务 id 不符（去重锚漂移）。")
        self.assertEqual(device.sent_bytes, [],
                         "RMD-A3 RED: 走 front 投递周期后设备腿出现二进制帧 — 裁定二红线"
                         "（front 不自己注入音频帧），本齿从端到端再钉一次。")
        self.assertFalse([f for f in _parsed(device.sent_text) if isinstance(f, dict)
                          and f.get("type") == SPEECH_INJECTION_TYPE],
                         f"RMD-A3 RED: {SPEECH_INJECTION_TYPE} 内部指令漏进设备腿（引擎内部"
                         "传输面不得外泄，设备按 Unknown type 丢弃）。")

        # ============ 对照：无 speech 的设备腿必达 + 引擎腿零注入 =========================
        res2, device2, engine2 = asyncio.run(_one_cycle(no_speech))
        self.assertEqual(res2["pushed"], [no_speech["id"]],
                         "RMD-A3 RED: 无 speech 的档位被拒绝投递 — 语音是可选增强。")
        self.assertEqual(len(_mcp_alert_frames(device2.sent_text)), 1,
                         "RMD-A3 RED: speech 缺省时弹窗投递不再必达（V20 降级不阻塞）。")
        self.assertEqual(_injection_frames(engine2.sent), [],
                         f"RMD-A3 RED: speech 为空仍向引擎腿注入 — 降级应是「仅弹窗+提示音」，"
                         f"而不是外发一条空 {SPEECH_INJECTION_TYPE}。")

    # --- RMD-A4 --------------------------------------------------------------
    def test_a4_alert_envelope_wire_shape(self):
        """coverage: RMD-A4 / contract-§三-推送语义 — 载荷 {id,fire_at,title,speech,view}
        逐字段到端，view 字符串化（设备 Property 三型限制），arguments.id 保留字符串业务 id。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        env = gw.build_alert_envelope(ALERT_CANONICAL)
        self.assertEqual(env.get("jsonrpc"), "2.0",
                         "RMD-A4 RED: 信封不是 JSON-RPC 2.0（mcp_server.cc:448 区段）。")
        self.assertEqual(env.get("method"), "tools/call",
                         "RMD-A4 RED: 提醒不是 tools/call 承载（契约 §四 mcp 工具调用 / 决策23）。")
        params = env.get("params") or {}
        self.assertEqual(params.get("name"), "kidpal/alert",
                         f"RMD-A4 RED: 工具名 {params.get('name')!r} != kidpal/alert")
        args = params.get("arguments") or {}
        self.assertEqual(set(args), {"id", "fire_at", "title", "speech", "view"},
                         f"RMD-A4 RED: arguments 键集漂移 {sorted(args)} — 契约 §三 零增减。")
        self.assertEqual(args.get("id"), ALERT_CANONICAL["id"],
                         "RMD-A4 RED: 字符串业务 id 必须留在 arguments.id（设备易失去重与"
                         "回执关联全靠它），不得被数字传输 id 顶替。")
        self.assertEqual(args.get("fire_at"), ALERT_CANONICAL["fire_at"])
        self.assertEqual(args.get("title"), ALERT_CANONICAL["title"])
        self.assertEqual(args.get("speech"), ALERT_CANONICAL["speech"])
        self.assertIsInstance(args.get("view"), str,
                              "RMD-A4 RED: 嵌套 view 必须字符串化承载（设备侧 Property 只有 "
                              "bool/int/str 三型，对象型会被拒；与 kidpal/display data 同纪律）。")
        self.assertEqual(json.loads(args["view"]), ALERT_CANONICAL["view"],
                         "RMD-A4 RED: view 字符串化后载荷内容不保真（round-trip 失败）。")

    def test_a4_numeric_jsonrpc_id_deterministic_and_in_int_range(self):
        """coverage: RMD-A4 — JSON-RPC 传输 id 数字化（本包从 mcp_relay 接管这件事）：
        设备 cJSON_IsNumber 只收数字；确定性派生、落在 int32 正区间的 200 抽样无碰撞。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        env = gw.build_alert_envelope(ALERT_CANONICAL)
        self.assertIsInstance(env.get("id"), int,
                              f"RMD-A4 RED: 信封 id {env.get('id')!r} 不是 JSON number — "
                              "设备 mcp_server.cc:546 cJSON_IsNumber 会按 Invalid id 拒收"
                              "（g-relay-fix 二轮冒烟实证过的断点）。")
        self.assertNotIsInstance(env.get("id"), bool)
        self.assertEqual(gw.build_alert_envelope(ALERT_CANONICAL)["id"], env["id"],
                         "RMD-A4 RED: 同一业务 id 两次构造得到不同传输 id — 重推必须复用同一"
                         "数字 id，否则设备侧「补回回执」关联不上。")
        ids = {gw.alert_jsonrpc_id(f"rmd-{i:06d}") for i in range(200)}
        self.assertEqual(len(ids), 200,
                         "RMD-A4 RED: 200 个不同业务 id 派生出碰撞的传输 id — 两档提醒会被"
                         "认成同一条回执（出队错档）。")
        self.assertTrue(all(isinstance(v, int) and 1 <= v <= 2 ** 31 - 1 for v in ids),
                        "RMD-A4 RED: 传输 id 越出设备可解析的正 int32 区间（valueint 截断）。")

    def test_a4_relay_alert_tool_call_numeric_id(self):
        """coverage: RMD-A4 — 上游明文把「alert 传输 id 数字化」划归本包：mcp_relay
        .alert_tool_call 曾把 §三 字符串业务 id 直接当 JSON-RPC id 用（设备按 Invalid id
        拒收）。独立成齿，避免被同档其它缺失掩盖、诊断不唯一。"""
        if str(SERVER) not in sys.path:
            sys.path.insert(0, str(SERVER))
        import gateway.mcp_relay as relay  # noqa: PLC0415
        relay_env = relay.alert_tool_call(ALERT_CANONICAL)
        self.assertIsInstance(
            relay_env.get("id"), int,
            f"RMD-A4 RED: mcp_relay.alert_tool_call 仍把字符串业务 id 当 JSON-RPC 传输 id 用"
            f"（id={relay_env.get('id')!r}）— 设备 mcp_server.cc:546 cJSON_IsNumber 会按 "
            "Invalid id 拒收整条提醒（g-relay-fix 二轮冒烟同款断点）。")
        self.assertEqual((relay_env.get("params") or {}).get("arguments", {}).get("id"),
                         ALERT_CANONICAL["id"],
                         "RMD-A4 RED: 数字化传输 id 时把 §三 业务 id 挤掉了 — 设备侧幂等去重"
                         "与回执关联全靠 arguments.id。")
        self.assertEqual(relay_env.get("method"), "tools/call",
                         "RMD-A4 RED(infra): alert 不再走 tools/call — 决策23 载体被改坏。")

    def test_a4_poll_alignment_and_due_predicate(self):
        """coverage: RMD-A4 — 每分钟轮询的节拍与到点判定（分钟对齐、边界含等号、非法不给真）。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        self.assertEqual(gw.POLL_INTERVAL_SEC, 60, "RMD-A4 RED: 轮询节拍必须 60 秒（spec）。")
        self.assertEqual(gw.next_poll_delay(0.0), 60.0,
                         f"RMD-A4 RED: next_poll_delay(0)=={gw.next_poll_delay(0.0)} 应为 60")
        self.assertEqual(gw.next_poll_delay(60.0), 60.0,
                         "RMD-A4 RED: 整分钟点应等下一整分钟（60s），不得 0 秒忙轮询。")
        self.assertEqual(gw.next_poll_delay(90.0), 30.0,
                         "RMD-A4 RED: 分钟对齐算法错（t=90 -> 应剩 30 秒）。")
        self.assertEqual(gw.next_poll_delay(-5.0), 60.0,
                         "RMD-A4 RED: 非法时钟必须防御回默认节拍。")
        due = gw.due_entries([ALERT_CANONICAL], "2026-09-09T16:15:00+08:00")
        self.assertEqual([e["id"] for e in due], [ALERT_CANONICAL["id"]],
                         "RMD-A4 RED: fire_at 恰等于 now 时必须判为到点（闭区间）。")
        self.assertEqual(gw.due_entries([ALERT_CANONICAL], "2026-09-09T16:14:00+08:00"), [],
                         "RMD-A4 RED: 未到点的条目被判为到点（会提前推）。")
        self.assertEqual(gw.due_entries([{"id": "bad"}], "2026-09-09T16:15:00+08:00"), [],
                         "RMD-A4 RED: 畸形条目不得被判为到点（校验前置，不盲推）。")

    def test_a4_retry_to_ack_60s_x3_then_void(self):
        """coverage: RMD-A4 / contract-§三-推送语义 — 重试到确认：60 秒超时未收回执重推、
        单档上限 3 次；上限后静默作废不补发（儿童产品不追催）。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        # sink 契约 = (业务 id, 传输信封)；本齿分别采集两者，避免用同一个列表既装 id 又装元组。
        pushed = []          # 业务 id 序列（与同文件其它 a4 齿同形）
        envs = []            # 每次推送实际下发的 JSON-RPC 信封序列
        def _sink(bid, envelope):
            pushed.append(bid)
            envs.append(envelope)

        d = gw.AlertDispatcher(sink=_sink)
        t0 = ALERT_CANONICAL["fire_at"]                      # 16:15 到点
        d.tick([ALERT_CANONICAL], t0)
        self.assertEqual(pushed, [ALERT_CANONICAL["id"]],
                         "RMD-A4 RED: 到点未推送。")
        self.assertEqual(d.attempts(ALERT_CANONICAL["id"]), 1)
        d.tick([ALERT_CANONICAL], "2026-09-09T16:15:30+08:00")
        self.assertEqual(len(pushed), 1,
                         "RMD-A4 RED: 60 秒超时未到就重推了（节拍错）。")
        d.tick([ALERT_CANONICAL], "2026-09-09T16:16:00+08:00")
        d.tick([ALERT_CANONICAL], "2026-09-09T16:17:00+08:00")
        self.assertEqual(len(pushed), 3, f"RMD-A4 RED: 3 次重试预算未打满: {len(pushed)}")
        self.assertEqual(d.attempts(ALERT_CANONICAL["id"]), 3)
        d.tick([ALERT_CANONICAL], "2026-09-09T16:18:00+08:00")
        self.assertEqual(len(pushed), 3,
                         "RMD-A4 RED: 超过单档 3 次上限仍在重推（上限失效=追催）。")
        self.assertEqual(d.state_of(ALERT_CANONICAL["id"]), "void",
                         "RMD-A4 RED: 预算耗尽后未静默作废。")
        acts = [ev["action"] for ev in d.events]
        self.assertEqual(acts, ["push", "push", "push", "void"],
                         f"RMD-A4 RED: 投递审计轨迹错: {acts!r} — 每次推送/作废必须留痕。")
        # 重推必须复用同一业务 id 与同一传输 id（幂等）—— envs 由 _sink 直接采集
        self.assertEqual(len(envs), 3,
                         f"RMD-A4 RED: 三次推送的信封采集数 {len(envs)} != 3 — 采集自洽性。")
        self.assertEqual(len({e["id"] for e in envs}), 1,
                         "RMD-A4 RED: 重推换了传输 id — 设备侧无法把它认成同一条。")
        args_ids = {e["params"]["arguments"]["id"] for e in envs}
        self.assertEqual(args_ids, {ALERT_CANONICAL["id"]})

    def test_a4_receipt_dequeues_and_is_idempotent(self):
        """coverage: RMD-A4 / RMD-A6 — 收回执即出队；数字 id 与业务 id 都收；重复回执幂等；
        「目前无人消费、未接出队方」的缺口由 consume_receipts 接上。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        pushed = []
        d = gw.AlertDispatcher(sink=lambda i, e: pushed.append(i))
        d.tick([ALERT_CANONICAL], ALERT_CANONICAL["fire_at"])
        numeric = gw.build_alert_envelope(ALERT_CANONICAL)["id"]
        out = gw.consume_receipts(d, [numeric])          # 设备回执带的是数字传输 id
        self.assertEqual(out, [ALERT_CANONICAL["id"]],
                         f"RMD-A4 RED: 数字传输 id 的回执没有被翻译成业务 id 出队: {out!r} — "
                         "front 泵收集到的 acked_ids 必须能接回投递档位。")
        self.assertEqual(d.state_of(ALERT_CANONICAL["id"]), "acked")
        d.tick([ALERT_CANONICAL], "2026-09-09T16:16:00+08:00")
        d.tick([ALERT_CANONICAL], "2026-09-09T16:17:00+08:00")
        self.assertEqual(pushed, [ALERT_CANONICAL["id"]],
                         "RMD-A4 RED: 已确认出队的档位又被推送（重复弹窗）。")
        again = gw.consume_receipts(d, [ALERT_CANONICAL["id"]])
        self.assertEqual(again, [],
                         "RMD-A4 RED: 重复回执又产生了一次出队效果（幂等缺失）。")
        self.assertEqual(d.state_of(ALERT_CANONICAL["id"]), "acked")
        self.assertIsNone(d.on_receipt("rmd-never-seen"),
                          "RMD-A6 RED: 未知 id 的回执被凭空记账成 acked。")

    def test_a4_reconnect_does_not_repump(self):
        """coverage: RMD-A4 — 不做「重连即重推」：设备连上不得触发补推。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        pushed = []
        d = gw.AlertDispatcher(sink=lambda i, e: pushed.append(i))
        d.tick([ALERT_CANONICAL], ALERT_CANONICAL["fire_at"])
        self.assertEqual(pushed, [ALERT_CANONICAL["id"]])
        self.assertEqual(d.on_ws_connected(), [],
                         "RMD-A4 RED: 重连返回了补推列表 — spec 明写「不做重连即重推」"
                         "（否则设备一醒就被历史提醒轰炸）。")
        self.assertEqual(len(pushed), 1, "RMD-A4 RED: 重连真的又推了一次。")

    def test_a4_same_id_repush_reuses_ids_and_budget(self):
        """coverage: RMD-A4 / V15 — cron 每 15 分钟重算把同 id 再交一份：不重开重试预算、
        不重复推送（同 id 重推两态分处的网关侧）。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A4 RED: " + err)
        pushed = []
        d = gw.AlertDispatcher(sink=lambda i, e: pushed.append(i))
        d.tick([ALERT_CANONICAL], ALERT_CANONICAL["fire_at"])
        d.tick([ALERT_CANONICAL, dict(ALERT_CANONICAL)], ALERT_CANONICAL["fire_at"])
        self.assertEqual(pushed, [ALERT_CANONICAL["id"]],
                         f"RMD-A4 RED: 重算交来的同 id 条目被重复推送: {pushed!r} — "
                         "重算复用既有 id 必须在网关侧也幂等。")
        d.tick([ALERT_CANONICAL], "2026-09-09T16:16:00+08:00")
        self.assertEqual(d.attempts(ALERT_CANONICAL["id"]), 2,
                         f"RMD-A4 RED: 重试预算被重算重置/多吃: attempts={d.attempts(ALERT_CANONICAL['id'])}")

    def test_a4_front_wires_delivery_and_receipt_dequeue(self):
        """coverage: RMD-A4 structural — front 真接投递面（每分钟轮询到点推送 + 回执出队）。
        行为半由上面 reminders.py 的纯逻辑齿承载；此处只钉装配到达（同 GRF-A1 先例口径）。"""
        src = _read_text(FRONT_APP)
        self.assertTrue(src, "RMD-A4 RED(infra): server/gateway/front/front_app.py unreadable")
        for needle, why in (
            ("read_pending(", "每分钟读 alerts/pending.json 的轮询落点"),
            ("next_poll_delay(", "对齐到整分钟的节拍落点"),
            ("deliver_alert(", "到点经 mcp kidpal/alert 推送的落点"),
            ("consume_receipts(", "把设备回执接出去队方的落点（g-relay-fix 遗留缺口）"),
        ):
            self.assertGreaterEqual(
                src.count(needle), 1,
                f"RMD-A4 RED: front_app.py never calls {needle} — {why}缺失；提醒投递仍停在"
                "「模块在世但无人驱动」的状态。")
        relay = _read_text(MCP_RELAY)
        self.assertIn("kidpal/alert", relay,
                      "RMD-A4 RED(infra): mcp_relay 不再定义 kidpal/alert — 决策23 载体被改坏")

    # --- RMD-A6 网关侧 -------------------------------------------------------
    def test_a6_gateway_dedupe_audit_and_bounded_cache(self):
        """coverage: RMD-A6 网关侧 — 非法载荷 reject 留痕不转发 + 记账有界（ECS 实测
        Swap used 535MB → 投递面零大常驻）。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A6 RED: " + err)
        pushed = []
        d = gw.AlertDispatcher(sink=lambda i, e: pushed.append(i))
        bad = {"id": "rmd-bad", "fire_at": "昨天", "title": "", "speech": None, "view": "x"}
        d.tick([bad], "2026-09-09T16:15:00+08:00")
        self.assertEqual(pushed, [], "RMD-A6 RED: 非法载荷被推送给设备（不盲转发红线）。")
        self.assertIn("reject", [ev["action"] for ev in d.events],
                      "RMD-A6 RED: 非法载荷被静默吞掉、审计轨迹无 reject 事件（故障不可归因）。")
        many = []
        for i in range(gw.MAX_TRACKED + 60):
            e = dict(ALERT_CANONICAL)
            e["id"] = f"rmd-bnd-{i:05d}"
            e["title"] = f"条目{i}"
            many.append(e)
        d2 = gw.AlertDispatcher(sink=lambda i, e: None)
        d2.tick(many, "2026-09-09T16:15:00+08:00")
        d2.tick(many, "2026-09-09T16:16:00+08:00")
        self.assertLessEqual(
            d2.tracked_count(), gw.MAX_TRACKED,
            f"RMD-A6 RED: 投递记账无界增长 tracked={d2.tracked_count()} > {gw.MAX_TRACKED} — "
            "ECS 上该进程与引擎/模型同机且 Swap 已用 535MB，投递面必须有界。")

    # --- RMD-A7 网关侧 -------------------------------------------------------
    def test_a7_expired_while_offline_is_voided_not_backfilled(self):
        """coverage: RMD-A7 / spec-决策24 — 断网期间未送达的档位，在网关推送时点判定作废、
        不补发（V16 口径：到点设备离线即进入重推窗口，上限后静默作废）。"""
        gw, err = _gw()
        if gw is None:
            self.fail("RMD-A7 RED: " + err)
        pushed = []
        d = gw.AlertDispatcher(sink=lambda i, e: pushed.append(i))
        stale = dict(ALERT_CANONICAL)
        stale["fire_at"] = "2026-09-09T16:15:00+08:00"
        # 设备离线 3 小时后网关才看到这个档位
        d.tick([stale], "2026-09-09T19:15:00+08:00")
        self.assertEqual(pushed, [],
                         f"RMD-A7 RED: 离线期间已过重试窗口的档位被补发 {pushed!r} — "
                         "过期静默作废不补发（儿童产品不追催）。")
        self.assertEqual(d.state_of(stale["id"]), "void",
                         "RMD-A7 RED: 推送时点判定作废未落账（state 应直接 void）。")
        self.assertEqual([ev["action"] for ev in d.events], ["void"],
                         f"RMD-A7 RED: 作废审计轨迹错: {[e['action'] for e in d.events]!r}")

    # --- RMD-A8 --------------------------------------------------------------
    def test_a8_bring_from_note_and_semantic_override(self):
        """coverage: RMD-A8 — 携带内容走课表**备注列**承载：备注非空时前晚档兜底文案必须带
        备注原文；语义产物给了 bring 就以它为准。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A8 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        entries = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00")
        # 钢琴课 = 周一 16:30；now = 周二 09-08T07:00 → 09-07 那一周次三档全已过并跳过，
        # 前晚档落在**下一次 occurrence（周一 09-14）的前一天** = 2026-09-13T20:30。
        # （与 test_a2_three_ladder_arithmetic 同口径，理由见彼处注记。）
        night = [e for e in entries
                 if "钢琴课" in e["title"] and e["fire_at"] == "2026-09-13T20:30:00+08:00"]
        self.assertTrue(night, "RMD-A8 RED: 没排到钢琴课的前晚档。")
        self.assertIn("带琴谱", night[0]["speech"] + night[0]["title"],
                      "RMD-A8 RED: 前晚（带物）档的兜底文案没有携带备注内容 — 携带内容由备注"
                      "列承载这条链在缺语义产物时断了（契约 §2.3 备注列口径）。")
        # 语义产物覆盖机械兜底
        key = lib.entry_key("weekly", model["weekly"][0])
        sem = {key: {"speech": "明天要上钢琴课啦，记得把琴谱和压泥板放进书包！",
                     "bring": ["琴谱", "压泥板"]}}
        e2 = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00", semantic=sem)
        got = [e for e in e2 if "钢琴课" in e["title"] and e["fire_at"].endswith("20:30:00+08:00")]
        self.assertTrue(got, "RMD-A8 RED: 带语义产物后排程丢了钢琴课前晚档。")
        self.assertIn("压泥板", got[0]["speech"],
                      "RMD-A8 RED: 语义产物给的 speech 没有被采用（仍用机械兜底 = 云侧语义"
                      "判断白做）。")
        self.assertEqual(lib.bring_items("带琴谱", sem.get(key)), ["琴谱", "压泥板"],
                         "RMD-A8 RED: 语义 bring 优先于机械兜底这条优先级没落。")
        self.assertEqual(lib.bring_items("带琴谱", None), ["带琴谱"],
                         "RMD-A8 RED: 无语义输入时机械兜底必须整条备注作单项（不得凭空发明，"
                         "也不得丢内容）。")
        self.assertEqual(lib.bring_items("", None), [],
                         "RMD-A8 RED: 空备注被编出了要带物 — 不得凭空发明。")

    def test_a8_semantic_artifact_schema_and_degrade(self):
        """coverage: RMD-A8 / 裁定一 — 语义中间产物的 schema 与「坏/缺输入」的降级行为
        （离线只能钉这些，大模型文案质量由 live 齿承载，不在此冒充）。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A8 RED: " + err)
        with _TmpDir() as d:
            good = d / "semantic.json"
            good.write_text(json.dumps({"weekly|x|y": {"speech": "文案", "bring": ["画笔"]}},
                                       ensure_ascii=False), encoding="utf-8")
            sem, e = lib.load_semantic(good)
            self.assertIsNone(e, f"RMD-A8 RED: 合法语义产物被判坏: {e!r}")
            self.assertIsInstance(sem, dict)
            missing, err_missing = lib.load_semantic(d / "nope.json")
            self.assertIsNotNone(err_missing,
                                 "RMD-A8 RED: 缺文件必须给出降级原因（可归因），不得静默。")
            self.assertIsNone(missing)
            broken = d / "broken.json"
            broken.write_text("{not json at all", encoding="utf-8")
            bad, err_bad = lib.load_semantic(broken)
            self.assertIsNone(bad, "RMD-A8 RED: 坏 JSON 语义产物被当成可用输入。")
            self.assertIsNotNone(err_bad)
            partial = d / "partial.json"
            partial.write_text(json.dumps({"k1": {"speech": "好文案"},
                                           "k2": "not-an-object",
                                           "k3": {"bring": [None, "", "画笔"]}}),
                               encoding="utf-8")
            sem2, err2 = lib.load_semantic(partial)
            self.assertIsNone(err2, "RMD-A8 RED: 个别条目不合法不该整档作废。")
            self.assertIn("k1", sem2 or {},
                          "RMD-A8 RED: 合法条目被坏邻居连坐丢弃（降级粒度过粗）。")
            # 降级后排程照常产出（缺语义 = 走机械兜底，不是不出提醒）
            sc = _sched()
            model = sc.parse_schedule_md(B1_CANONICAL)
            plan = lib.plan_reminders(model, "2026-09-08T07:00:00+08:00", semantic=None)
            self.assertTrue(plan, "RMD-A8 RED: 没有语义产物就完全不排提醒 — 降级必须是继续投递。")

    def test_a8_prompt_carrier_v3_landed_and_wired(self):
        """coverage: RMD-A8 structural — OpenClaw system prompt 约定的承载体按版本化惯例
        以 v3 新文件落盘并接渲染（「备注里哪部分算要带的」+ 三档阶梯 + 学校区块不提醒）。
        只钉约定文本存在性与装载接线（裁定一授权的离线面），语义正确性由 live 齿承载。"""
        self.assertTrue(PROMPT_CARRIER_V3.is_file(),
                        "RMD-A8 RED: server/deploy/openclaw-system-prompt.v3.md missing — "
                        "提醒语义/文案约定的载体未落盘（包 6 先例 = 版本化新文件，不覆盖 v2）。")
        txt = _read_text(PROMPT_CARRIER_V3)
        self.assertTrue(txt.strip(), "RMD-A8 RED: v3 载体是空文件。")
        self.assertIn("备注", txt, "RMD-A8 RED: v3 未写明「备注列承载携带内容」的约定。")
        self.assertIn("学校课程表", txt,
                      "RMD-A8 RED: v3 未写明「学校课程表区块不提醒」的范围约定。")
        wiring = "\n".join(p.read_text(encoding="utf-8", errors="replace")
                           for p in sorted(DEPLOY_DIR.rglob("*")) if p.is_file())
        self.assertIn(PROMPT_CARRIER_V3.name, wiring,
                      "RMD-A8 RED: server/deploy 从未引用 v3 载体 — overlay 渲染未切到 v3"
                      "（验收所测 != 生产所用）。")

    def test_a8_prompt_carrier_v3_reminder_section_anchors(self):
        """coverage: RMD-A8 / 裁定一 — carrier 齿收紧：只认**提醒面专有锚**，且锚必须落在
        「提醒…约定」这一节内部（段落存在 + 段内含语义规则），不认与课表面共享的宽词。

        tighten ADD 理由（2026-09-08 integrator 跨包合并探测发现；planner 亲跑
        `git merge-tree --write-tree HEAD main` 复核）：并行包 story-p3 与本包抢同一个
        `openclaw-system-prompt.`**`v3`** 槽位 = add/add 冲突（本包侧 181 行提醒约定 vs
        main 侧 160 行故事 producer 扩展）。main 那份只把「包 7 — 提醒备注解析约定」写成
        增量扩展区里的 2 行**占位**（约定正文缺席），而旧 carrier 齿
        (test_a8_prompt_carrier_v3_landed_and_wired) 的 needle「备注」「学校课程表」是
        课表面与提醒面**共享的宽词**（main 版分别命中 3 / 2 次）—— 于是「合并取 main 版 =
        本包提醒约定静默消失」这个最可能的失败模式下旧齿仍 PASS = 假绿。
        本齿改为**内容驱动**：把载体按 ##/###/#### 切成小节，对每一节检查必需项，要求
        至少有一节全项齐备（缺哪一项都写进红因）——必需项 = (a) 实质篇幅（占位标题级过不了）、
        (b) 三个与确定性脚本**有契约关系**的硬锚 entry_key / bring / speech（改了直接断链，
        不会被散文润色弄红）、(c) 四条语义规则组各自命中其一（判定口径 / 区块范围 / 反发明 /
        语义产物，用 OR 组而非整句措辞以抗日后修订）、(d) 三档档位名 night+前晚 /
        morning+早档 / soon+临期 **三条同时**在场、(e) 三个区块名都出现在**同一节内**
        （不再依赖全文宽词命中）。选节按内容而非标题措辞，故标题改名或换级别都不会误红。
        合并三种消解的期望：取本包版 / 取并集 -> 绿；取 main 版（单边丢弃）-> 红；
        另测「本包版但提醒小节标题被改名」-> 绿（脆断言自查）。
        钉的是 RMD-A8 已声明的原意图（「备注里哪部分算要带的」由 OpenClaw system prompt 约定），
        不新增 acceptance id、不改行为定义。
        """
        import re  # noqa: PLC0415  局部导入，不改本档既有 import 面
        txt = _read_text(PROMPT_CARRIER_V3)
        self.assertTrue(txt.strip(),
                        "RMD-A8 RED: v3 载体不可读或为空 — 提醒约定正文无从定位。")
        lines = txt.split("\n")

        def _level(ln: str) -> int:
            m = re.match(r"^(#+)\s", ln)
            return len(m.group(1)) if m else 0

        def _sections():
            """每个 ##/###/#### 标题 -> (标题行, 该节正文直到下一个同级或更高级标题)。"""
            for i, ln in enumerate(lines):
                lvl = _level(ln)
                if not 2 <= lvl <= 4:
                    continue
                j = i + 1
                while j < len(lines):
                    l2 = _level(lines[j])
                    if l2 and l2 <= lvl:
                        break
                    j += 1
                yield ln.strip(), "\n".join(lines[i + 1:j])

        LADDER_GROUPS = (("night/前晚", ("night", "前晚")),
                         ("morning/早档", ("morning", "早档")),
                         ("soon/临期", ("soon", "临期")))
        BLOCKS = ("学校课程表", "每周固定课表", "一次性安排")
        HARD_ANCHORS = (
            ("entry_key", "语义产物的键必须与脚本 entry_key 算法逐字对齐（裁定一分层锚）"),
            ("bring", "备注判定结果必须以 bring 数组交给脚本（RMD-A8 携带内容的承载点）"),
            ("speech", "给孩子的文案必须以 speech 字段产出（契约 §三 speech 字段的来源）"),
        )
        RULE_GROUPS = (
            (("要带", "携带"), "「备注里哪部分算要带的」判定口径"),
            (("不提醒", "不生成"), "按区块判范围（学校课程表区块不提醒）"),
            (("凭空", "严禁", "不得编造", "不要自己发明"), "备注为空时不得凭空发明要带物"),
            (("语义",), "语义中间产物是 agent 的产出物（裁定一混合分层）"),
        )

        def _unmet(body: str) -> list:
            """该节正文缺哪些必需项 —— 缺一即计入，最终必须为空。"""
            u = []
            if len(body.strip()) < 400:
                u.append(f"实质篇幅（现 {len(body.strip())} 字符 < 400 = 占位标题级，"
                         "约定正文缺席）")
            for anchor, why in HARD_ANCHORS:
                if anchor not in body:
                    u.append(f"硬锚 {anchor!r}（{why}）")
            for alts, rule in RULE_GROUPS:
                if not any(a in body for a in alts):
                    u.append(f"「{rule}」规则（候选措辞 {alts} 在小节内全部未命中）")
            miss = [n for n, keys in LADDER_GROUPS if not any(k in body for k in keys)]
            if miss:
                u.append(f"档位取向说明 {miss}（三段阶梯决策 20）")
            for block in BLOCKS:
                if block not in body:
                    u.append(f"本节内的区块范围「{block}」")
            return u

        scored = sorted(((len(_unmet(b)), h, b) for h, b in _sections()), key=lambda t: t[0])
        if not scored:
            self.fail("RMD-A8 RED: v3 载体内没有任何 ##/###/#### 小节 — 承载体结构不可识别，"
                      "提醒备注解析约定更无从谈起。")
        n_unmet, best_head, best_body = scored[0]
        self.assertEqual(
            n_unmet, 0,
            "RMD-A8 RED: v3 载体内**没有任何一节**齐备提醒备注解析约定的必需项 —— 最接近的"
            f"一节是 {best_head!r}，仍缺：{ _unmet(best_body) }。典型失败形态 = 跨包合并取了"
            "兄弟包的 .v3 版（提醒约定只在增量扩展区留一行占位标题），本包 181 行约定正文"
            "静默消失；旧 carrier 齿靠「备注」「学校课程表」这类课表面共享宽词命中，看不见"
            "这种缺失（本齿改为只认提醒面专有锚 + 段落级结构锚，标题改名/换级别都不误红）。")

    def test_a8_b1_table_body_zero_change_reuses_package6_parser(self):
        """coverage: RMD-A8 / RMD-E3 — B1 表格主体零变更、课表解析复用包 6 不重写。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("RMD-A8 RED: " + err)
        sc = _sched()
        model = sc.parse_schedule_md(B1_CANONICAL)
        self.assertEqual(
            set(model["weekly"][0]), {"weekday", "start", "end", "name", "location", "note"},
            "RMD-A8 RED: 包 6 B1 行模型键集漂移 — 契约 §2.2 表格主体零变更红线被破。")
        src = _read_text(REMINDER_LIB)
        self.assertIn("parse_schedule_md", src,
                      "RMD-A8 RED: 排程脚本没有复用包 6 的 parse_schedule_md，另起一套 B1 "
                      "解析（RMD-E3 红线：消费包 6 的课表数据链路，不重复造数据）。")
        self.assertEqual(model["school"][0]["periods"], ["语文", "数学", "自习"],
                         "RMD-A8 RED(infra): 包 6 解析器被本包改坏（学校区块不可读）。")

    # --- RMD-E3 / E4 ---------------------------------------------------------
    def test_e3_no_http_alerts_endpoint_revival_and_no_data_duplication(self):
        """coverage: RMD-E3 / RMD-E4 — 原 HTTP 轮询形态已废止，不得复活；提醒走 mcp 推送。"""
        corpus = "\n".join(_read_text(p) for p in sorted(GATEWAY_DIR.rglob("*.py")))
        self.assertNotIn(
            "/v1/alerts", corpus,
            "RMD-E4 RED: gateway 里再现 /v1/alerts HTTP 轮询端点 — 契约 §三 明写该形态废止"
            "（HTTP 轮询端点随 Pod S 台阶退役），提醒一律走 mcp kidpal/alert 推送。")
        self.assertIn("kidpal/alert", corpus,
                      "RMD-E3 RED: 网关投递面没有 kidpal/alert 载体（决策23 统一 mcp 工具调用）。")

    def test_e4_contract_point3_evidenced_and_poll_form_retired(self):
        """coverage: RMD-E4 — contractBlocked 解除记录在案（契约待确认点 3 已实证）。"""
        contract = _read_text(REPO_ROOT / "docs" / "contracts" / "interface-contract.md")
        self.assertIn("已实证", contract,
                      "RMD-E4 RED(infra): 契约里找不到「已实证」记录（待确认点 3 解除依据）。")
        self.assertIn("kidpal/alert", contract,
                      "RMD-E4 RED(infra): 契约 §三/§四 不再描述 kidpal/alert 推送语义。")
        spec = _read_text(REPO_ROOT / "docs" / "spec.md")
        self.assertIn("重试到确认", spec,
                      "RMD-E4 RED(infra): spec 的送达语义记录被改动（重试到确认口径是包 7 的"
                      "验收依据）。")

    # --- boundary: 错误/边界/权限路径 + 防假绿 --------------------------------
    def test_boundary_malformed_payloads_rejected_not_forwarded(self):
        """coverage: boundary (错误路径) — §三 机械校验逐形拒因 + 读侧坏文件不崩。"""
        gw, err = _gw()
        if gw is None:
            self.fail("boundary RED: " + err)
        ok, _ = gw.validate_alert_entry(ALERT_CANONICAL)
        self.assertTrue(ok, f"boundary RED: 规范 §三 条目被自己拒绝。")
        cases = [
            (None, "非对象"),
            ({}, "空对象"),
            ({**ALERT_CANONICAL, "id": ""}, "空 id"),
            ({**ALERT_CANONICAL, "id": 123}, "id 非字符串"),
            ({**ALERT_CANONICAL, "id": "rmd 带空格"}, "id 含非法字符"),
            ({**ALERT_CANONICAL, "id": "x" * 60}, "id 超设备承载上界"),
            ({k: v for k, v in ALERT_CANONICAL.items() if k != "title"}, "缺 title"),
            ({**ALERT_CANONICAL, "fire_at": "2026-09-09 16:15"}, "fire_at 非 ISO"),
            ({**ALERT_CANONICAL, "fire_at": "2026-09-09T16:15:00"}, "fire_at 无偏移"),
            ({**ALERT_CANONICAL, "speech": 7}, "speech 非字符串"),
            ({**ALERT_CANONICAL, "view": {"view": "reminder", "data": {}}}, "内嵌 view 非白名单"),
        ]
        for entry, why in cases:
            good, reason = gw.validate_alert_entry(entry)
            self.assertFalse(good, f"boundary RED: {why} 竟然通过机械校验（会盲转发给孩子）。")
            self.assertTrue(isinstance(reason, str) and reason.strip(),
                            f"boundary RED: {why} 被拒但没给原因（不可归因）。")
        no_speech = {k: v for k, v in ALERT_CANONICAL.items() if k != "speech"}
        ok_ns, r_ns = gw.validate_alert_entry(no_speech)
        self.assertTrue(ok_ns, f"boundary RED: speech 缺省应是合法的降级形态 ({r_ns})。")
        with _TmpDir() as d:
            e, rej = gw.read_pending(d / "absent.json")
            self.assertEqual((e, rej), ([], []),
                             "RMD-A4 RED: 交接文件还不存在时必须安静地返回空（cron 首次写入前"
                             "文件确实不存在 —— planner ECS 实测）。")
            junk = d / "junk.json"
            junk.write_text("{{{ half written", encoding="utf-8")
            e2, rej2 = gw.read_pending(junk)
            self.assertEqual(e2, [], "RMD-A2 RED: 半截/坏 JSON 的 pending.json 被当成可用条目。")
            self.assertTrue(rej2, "RMD-A2 RED: 整档坏但没有拒绝原因（读侧坏输入不可归因）。")

    def test_boundary_bad_semantic_and_bad_config_inputs_degrade(self):
        """coverage: boundary (错误路径) — 配置/语义载体的坏输入一律降级不崩、不瞎猜默认。"""
        lib, err = _rem_lib()
        if lib is None:
            self.fail("boundary RED: " + err)
        with _TmpDir() as d:
            weird = d / "weird.yaml"
            weird.write_text("# 只有注释\n\nnot_a_key_line\nnight_at: \"  21:05 \"\n"
                             "unknown_key: 42\n", encoding="utf-8")
            cfg = lib.load_ladder_config(weird)
            self.assertEqual(cfg.get("night_at"), "21:05",
                             f"boundary RED: 配置值未清洗/未读到: {cfg!r}")
            self.assertEqual(cfg.get("morning_at"), "07:45",
                             "boundary RED: 缺项应回退默认而不是丢空（排程不得停摆）。")
            self.assertNotIn("unknown_key", cfg,
                             "boundary RED: 未知配置键被原样塞进生效配置（配置面失控）。")
            empty = d / "empty.yaml"
            empty.write_text("", encoding="utf-8")
            cfg2 = lib.load_ladder_config(empty)
            self.assertEqual((cfg2.get("night_at"), cfg2.get("before_min")),
                             ("20:30", 15), f"boundary RED: 空载体未回退默认: {cfg2!r}")

    # --- RMD-E2 回归守卫 -----------------------------------------------------
    def test_e2_existing_frozen_acceptance_suites_green(self):
        """coverage: RMD-E2 — 既有冻结验收不回归（G 网关 / g-relay-fix 回执帧 / 包 6 课表
        三档离线套件独立复跑 exit 0；本包是它们的 additive 消费方）。"""
        targets = ["test_gateway_directionB.acceptance.py",
                   "test_g_relay_fix.acceptance.py",
                   "test_schedule_p2.acceptance.py"]
        for name in targets:
            proc = subprocess.run([sys.executable, str(SERVER / "acceptance" / name)],
                                  capture_output=True, text=True, encoding="utf-8",
                                  errors="replace", timeout=600, cwd=str(SERVER))
            self.assertEqual(
                proc.returncode, 0,
                f"RMD-E2 RED: 既有冻结验收 {name} 回归。\nstdout tail: "
                f"{proc.stdout[-1200:]!r}\nstderr tail: {proc.stderr[-600:]!r}")


# --------------------------------------------------------------------------- #
# Live face — 真 cron + 真到点推送 + 真回执 + 真语义文案（不 auto-run）           #
# --------------------------------------------------------------------------- #

class ScheduleReminderLiveAcceptance(unittest.TestCase):
    """live teeth: 真跑一轮三档排程→到点推送→渲染回执出队的落盘证据（planner 已在 ECS
    只读实测 cron 在册：4 条全 status=ok，含先例 class-reminder-0529；本包必须真 exercise
    一次端到端，只探测可达不算 —— probe ≠ exercise）。证据下
    changes/dev/260829-schedule-reminder/。

    五事实：
      L1 cron 任务真在册并真跑过（openclaw cron list 摘录：name/enabled/schedule.expr/
          payload.kind/state.lastRunStatus）；
      L2 alerts/pending.json 真身落地且条目形状合契约 §三（逐字段摘录，非自造 fixture）；
      L3 到点真经 mcp kidpal/alert 推到设备（设备串口或引擎/front 日志摘录）；
      L4 渲染回执真回来且档位出队（回执摘录 + 无重复推送计数）；
      L5 真大模型语义产物（备注→带物判断 + 给孩子的文案）落在语义中间产物里，且与机械兜底
         文案可区分（证明确实过了 agent 回合）。
    另 RMD-A3 的真机音频时序（弹窗与 TTS 并发/提示音）按 RMD-E2 口径殿后 9b —— 本类只
    核验「引擎注入指令真被认领并出了音」的日志事实，不核验听感。
    """

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _corpus(self):
        if not LIVE_EVIDENCE_DIR.is_dir():
            return None
        parts = []
        for p in sorted(LIVE_EVIDENCE_DIR.rglob("*")):
            if p.is_file() and p.suffix in (".md", ".log", ".txt", ".json"):
                try:
                    parts.append(p.read_text(encoding="utf-8", errors="replace"))
                except OSError:
                    continue
        return "\n".join(parts) if parts else None

    def _require(self):
        corpus = self._corpus()
        self.assertTrue(
            corpus,
            "RMD-E2 live RED: changes/dev/260829-schedule-reminder/ 下无落盘证据 — 真 cron"
            " 排程 + 真到点投递 + 真回执闭环必须先真跑一轮并留证（probe ≠ exercise）。")
        return corpus

    def test_a2_live_cron_roundtrip_evidence(self):
        """coverage: RMD-A2 live — L1 cron 在册且真跑过 + L2 pending.json 真形状。"""
        corpus = self._require()
        self.assertTrue(
            ("openclaw cron" in corpus) or ("cron list" in corpus) or ("lastRunStatus" in corpus),
            "RMD-A2 live RED: 证据未摘录 openclaw cron 在册/执行事实（L1）。")
        self.assertIn("pending.json", corpus,
                      "RMD-A2 live RED: 证据未摘录 alerts/pending.json 真身（L2）。")
        for field in ("fire_at", "speech", "title"):
            self.assertIn(field, corpus,
                          f"RMD-A2 live RED: 交接条目摘录缺 §三 字段 {field}（L2）。")
        self.assertTrue(("class-reminder" in corpus) or ("enabled" in corpus)
                        or ("schedule" in corpus),
                      "RMD-A2 live RED: 无 cron 任务形状摘录（name/enabled/schedule）。")
        for bad in ("lastRunStatus\": \"error", "排程失败", "cron 未注册"):
            self.assertNotIn(bad, corpus, f"RMD-A2 live RED: 证据记录了失败结果 {bad!r}")

    def test_a4_live_push_reaches_device_and_acks(self):
        """coverage: RMD-A4 live — L3 真推送到设备 + L4 渲染回执真回来并出队。"""
        corpus = self._require()
        self.assertIn("kidpal/alert", corpus,
                      "RMD-A4 live RED: 证据未摘录 kidpal/alert 实际下发事实（L3）。")
        self.assertTrue(("回执" in corpus) or ("receipt" in corpus) or ("\"result\"" in corpus)
                        or ("relay_ack" in corpus),
                      "RMD-A4 live RED: 证据未摘录设备渲染回执（L4）。")
        self.assertTrue(("出队" in corpus) or ("acked" in corpus) or ("dequeu" in corpus.lower())
                        or ("void" in corpus),
                      "RMD-A4 live RED: 证据未摘录回执后档位状态变化（L4 出队事实）。")
        for bad in ("Invalid id", "Unknown message type", "推送失败", "未收回执后放弃"):
            self.assertNotIn(bad, corpus,
                             f"RMD-A4 live RED: 证据里存在真实拒收/失败 {bad!r} —— 不得按通过交付。")

    def test_a8_live_semantic_is_agent_produced(self):
        """coverage: RMD-A8 live — L5 真大模型语义产物（备注→带物 + 文案）确实过了 agent 回合。"""
        corpus = self._require()
        self.assertTrue(("system prompt" in corpus) or ("system_prompt" in corpus)
                        or ("语义" in corpus) or ("带物" in corpus) or ("semantic" in corpus),
                      "RMD-A8 live RED: 证据未摘录语义判断/文案由 OpenClaw agent 回合产出的事实（L5）。")
        self.assertTrue(("备注" in corpus) or ("note" in corpus),
                        "RMD-A8 live RED: 证据未摘录「备注里哪部分算要带的」判断结果（L5）。")

    def test_a3_live_tts_and_popup_timing_evidence(self):
        """coverage: RMD-A3 live — 引擎注入指令被认领并出音（真机音频时序/听感殿后 9b）。"""
        corpus = self._require()
        self.assertTrue((SPEECH_INJECTION_TYPE in corpus) or ("speak_txt" in corpus)
                        or ("播报" in corpus) or ("TTS" in corpus),
                        "RMD-A3 live RED: 证据未摘录语音注入指令被引擎认领/出音的事实 — "
                        "裁定二「借引擎既有音频链」必须有一次真跑留证（听感与并发时序另记 9b）。")
        self.assertTrue(("降级" in corpus) or ("弹窗" in corpus) or ("popup" in corpus.lower()),
                        "RMD-A3 live RED: 证据未摘录弹窗/降级投递事实（必达面必须与语音面分开核）。")

    def test_a7_live_offline_gap_voids_slot(self):
        """coverage: RMD-A7 live — 断网一轮：设备侧无提醒 + 网关侧档位作废未补发。"""
        corpus = self._require()
        self.assertTrue(("断网" in corpus) or ("离线" in corpus) or ("offline" in corpus.lower()),
                        "RMD-A7 live RED: 证据未摘录一轮真实离线（决策 24 断网无提醒必须实测一次）。")
        self.assertTrue(("作废" in corpus) or ("void" in corpus) or ("无提醒" in corpus),
                        "RMD-A7 live RED: 证据未摘录离线期间的作废/无提醒结果。")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(ScheduleReminderOfflineAcceptance)
    if os.environ.get("KRM_RUN_LIVE") == "1" or os.environ.get("RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(ScheduleReminderLiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)


# --------------------------------------------------------------------------- #
# REVISION LOG (acceptance-author only; tighten-only discipline):
#   2026-09-08  initial freeze (acceptance-author, TaskPackage 260829-schedule-reminder).
#   2026-09-08  冻结件自身两处缺陷修复（planner 回派、断言语义零变更）：钢琴课三档期望簇
#               的参考周次（now 是周二，09-07 周次三档全已过并与 past-skip 齿互斥，按
#               spec §4.1 + 决策 20 取 next occurrence 09-13/09-14，牵动 3 齿）；
#               test_a4_retry_to_ack_* 一齿内 sink 采集形状与自身解包冲突（拆 pushed/envs
#               两个采集器）。Reason: fixture/collection 缺陷由 planner 独立核实，非实现
#               缺陷；未删齿、未放宽、被钉关系（前一天 20:30 / 当天 07:45 / 事前 15 分；
#               60s×3 后 void 与审计轨迹）逐字不变。
#   2026-09-08  tighten-only ADD: one extra offline test method
#               test_a3_front_poll_injects_speech_into_engine_leg（配套 _load_front_app
#               + _ensure_fastapi_stub 装载器）—— 真跑 front 一轮投递周期，用假设备腿/
#               假引擎腿对账「推送条数 == 引擎腿注入条数」并加无 speech 对照。
#               Reason: mutation evidence sensitive:false —— 变异把 _deliver_alert_now 的
#               deliver_alert(entry, device_ws, engine_ws) 改成 (…, device_ws, None)
#               （语音永不外发）时本档不敏感，原先只有数源码字符串的 needle 齿守着这条腿，
#               且仅 builder 的非冻结单测（commit 503e022）在挡，无冻结保护。钉的是 RMD-A3
#               与裁定二已声明的同一意图，不新增 acceptance id、不改行为定义；不碰音频/
#               真机时序（属已冻结的 test_a3_live_* + 9b 面）。无既有断言被删除或修改。
#   2026-09-08  tighten-only ADD: one extra carrier-conformance test method
#               test_a8_prompt_carrier_v3_reminder_section_anchors — 按内容切节，要求
#               至少一节齐备提醒面专有锚（entry_key / bring / speech 三硬锚 + 四条语义
#               规则组 + 三档取向 + 三个区块名同节内 + 实质篇幅），缺一即红。
#               Reason: integrator 跨包合并探测发现的第二例 oracle 缺陷（触发源非 mutation
#               批次）—— 并行包 story-p3 与本包抢同一个 openclaw-system-prompt.v3 版本化
#               槽位 = add/add 冲突，main 侧那份只把「包 7 — 提醒备注解析约定」写成 2 行
#               占位；旧 carrier 齿的 needle「备注」「学校课程表」是与课表面共享的宽词
#               （main 版命中 3 / 2 次），故「合并取 main 版 = 本包 181 行约定静默消失」
#               这一最可能失败模式下旧齿仍 PASS = 假绿，保护不了 RMD-A8 所钉决策。
#               自证：仓外临时副本重放四种消解 + 八个逐锚/逐组删除变体（共 12 例）——
#               main-only 旧 PASS/新 FAIL（假绿复现并闭合）、union 双绿（不惩罚正确消解）、
#               标题改名绿（抗润色）、单删任一硬锚或整组删任一规则措辞均红、仅留一个同义词
#               绿。不新增 acceptance id、不改行为定义；旧 carrier 齿一字未动（保留作
#               「载体在世 + 装载接线」的既有半）。
# --------------------------------------------------------------------------- #
