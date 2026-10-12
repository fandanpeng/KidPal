"""Frozen acceptance tests — TaskPackage 260924-m4-k1-story-cloud-link
(subsystem: server; workstream m4-star-notebook; mergeOrder 38; rigor full; riskTier medium).

承载 story = S-F2.1（看「星星记录本」列表：AC1 数据源半 + AC4 云端同步半）
+ S-F2.2（开录双入口：AC1–AC2 云端累积/收尾半）+ S-F2.4（星星保存成就盖章：AC2 流水号账本半）。

Scope of record（taskpackage acceptance M4K1-A1..A6 + exitCriteria E1–E3 + gate evidence §6 条件 C1）：
  M4K1-A1 契约门齿 —— gate evidence §6 C1（Q7 relay 收紧 + Q8 story floor）的机械执行面
          + 契约三触点落文锚（§1.3③ total 语义 + items 窗口 / §1.3④ data.seq / §四 delete）。
  M4K1-A2 session_ctx 拦截齿（front 上行泵整条消费、不下透引擎；source_page=story 切累积态）。
  M4K1-A3 story_list 推送齿（producer 窗口/计数语义 + 三时点下发；≤16 窗口 + total 恒全量）。
  M4K1-A4 删除执行齿（kidpal_story_delete 消费：哨兵头校验 + MD 硬删 + 幂等 no-op + 重推对账
          + 非法 id 拒绝审计）。
  M4K1-A5 收尾与超时齿（收尾词机械检测 + 120s 无语音自动收尾的纯决策面 + BOOT 丢弃零偏移）。
  M4K1-A6 流水号账本齿（归档分配 seq、删除不回退、重启存活、账本缺失重建不倒退）。
  M4K1-A7 live 全链复证 = structural deferred 挂 M4-K3 设备轮 —— 本文件**无 live 面**（不冻 live 齿）。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — pure python 面可离线 import，
front_app.py 面离线装载（本文件在 fastapi 缺席时用本地 stub，g-relay-fix 先例），
两侧都真跑运行时行为（非源码 grep）。

  A) server/gateway/story.py（纯模块，import 期零 fastapi/gateway-front 依赖；增量扩展）：

    STORY_LIST_WINDOW = 16
        story_list items 窗口上限（契约 §1.3③ Q2 丙案；= 板端 KSTORY_LIST_MAX）。
    AUTO_STOP_SECONDS = 120
        累积态无语音自动收尾阈值（秒；requirements §2.1 录音态超时口径）。

    is_finish_phrase(text) -> bool
        累积态收尾词机械检测：text（str）剔除首尾空白后**包含**精确短语「我说完了」
        或「讲完啦」-> True；其余（含空串/None/非 str）-> False。可读文本子串匹配，
        末尾标点不阻断（「我说完了。」含「我说完了」）。

    list_stories(stories_dir, *, limit=None, offset=0) -> dict
        契约 §1.3③ producer 下发块：
          {"view":"story_list","data":{"total":<现存篇数>,"items":[{id,title,date},…]}}
        - items = **最新 ≤ STORY_LIST_WINDOW 条窗口**（完整日期倒序，同日按 id 倒序）；
          显式 limit 给定则用 limit（既有分页语义保留）。
        - **total = 全量现存篇数**（目录内全部可解析故事记录数），与 limit/offset/窗口无关，
          绝不以 len(items) 顶替（>16 篇时窗口条数 ≠ 星星计数 = load-bearing 警示句）。
        - item keys 恰为 {id,title,date}（**不得**塞 seq）；date 显示 "MM-DD"。
        - 空目录/只杂物 -> total 0、items []。整包过 validate_display。

    story_text(record) -> dict
        契约 §1.3④：{"view":"story_text","data":{"id","title","text"[,"seq"]}}——
        **seq 仅当 record 携带整数 seq 时出现**（additive；缺省时设备行为不变，
        缺省则 data keys 恰为 {id,title,text}）。整包过 validate_display。

    archive_story(stories_dir, transcript, *, title=None, slug=None, date=None) -> dict
        既有语义零偏移（见 E1），**additive**：返回 record 增 "seq"（int）= 该故事入册
        流水号；持久账本 <stories_dir>/.seq.json（原子写、front 单写者）承载：
        - 新 id 归档 -> seq = (已分配过的最大 seq) + 1（首篇 = 1）；
        - **同 id 再归档（幂等重写）-> seq 不变、不涨计数**；
        - 删除故事文件**不回退** seq（删除后新 id 仍得更高的 seq）；
        - 进程/实例重启后继续（账本落盘存活，不靠扫描现存文件回推）；
        - 账本缺失/损坏 -> 从现存文件可恢复 seq + 安全边际**重建**并告警，
          **绝不把 seq 回退/复用当可接受降级**（重建后新 id 的 seq > 重建前最大 seq）。

    delete_story(stories_dir, story_id) -> bool
        契约 §四 kidpal_story_delete 的机械执行面（硬删、无恢复；Q6）：
        - story_id 为非空 str、不含路径分隔符/「..」；解析 <stories_dir>/<story_id>.md；
        - 必须经哨兵头校验（read_story_file(path) is not None）——路径穿越 / 无哨兵头
          杂物文件（README.md 等）/ 不存在的 id 一律**拒绝**（返回 False + 审计日志留痕），
          **绝不**删除 stories_dir 以外的文件；
        - 合法 -> 真删该 MD 文件（硬删）-> 返回 True + 审计日志；
        - 流水号账本不变（seq 不回退）；删除的产品边界 = 仅 story MD（Q6）。
        纯机械、绝不抛异常（畸形 id 归 False）。

    class StoryAccumulator（既有 start/feed/stop/cancel/redo/recover/routing 零偏移；增量）：
        timed_out_sessions(now, *, timeout_s=None) -> list[str]
            now: datetime；timeout_s: float|None（缺省 AUTO_STOP_SECONDS）。
            返回**累积态（routing=="accumulate"）**中「最近活动时刻」（= start()/feed()
            显式传入的 now）距 now **>= timeout_s** 的会话 id 列表。
            - 恰为 timeout_s -> 计入（>=）；未记录活动的会话、非累积态会话、
              已 stop/cancel 的会话 -> 不返回。查询不改状态。

  B) server/gateway/display_schema.py（Q8 story 轻量形状 floor；schedule floor 先例对称）：
        validate_display 对 story_list / story_text 新增**形状 floor**：
        - story_list：data.total（若在场）必为 **int 且 >= 0**（字符串/负数/浮点 -> 拒绝）；
          data.items 必为数组，每个 item 必为对象且**三键 {id,title,date} 齐**（缺键 -> 拒绝）；
        - story_text：data.seq（若在场）必为 **int**（字符串/浮点 -> 拒绝）；
        - 其余 view 行为不变（schedule floor 不动，白名单零扩 —— kidpal_story_delete
          为 §四消息、不进 display view 白名单）。

  C) server/gateway/front/front_app.py（接线面；g-relay-fix 先例 = 模块必须离线可装载）：
        build_display_tool_calls：**Q7 relay 收紧** —— 拒绝引擎侧来源的 story_list /
          story_text display 块：该 view 的块降级为友好 text 块（绝不把 story 载荷透传给设备），
          与其它 view 的既有行为（合法块中继 / 脏块降级）零偏移。
        _push_story_list(websocket) -> None（async，新增）
          front 机械推送 story_list 的下行缝：把 list_stories(<STORIES_DIR>) 以 kidpal/display
          mcp 帧下发**恰一帧**（设备 wire 形状 {"view":<str>,"data":<json 串>}）；
          **front 自推不经 relay**（Q7 天然豁免）。三时点调用：WS 建连 / 归档成功后 / 删除执行后。
        pump_client_to_upstream（设备→引擎上行泵）机械消费并**拦截、不下透引擎**：
          - 收到 {"type":"kidpal_session_ctx","source_page":<任意>} -> 整条消息**不转发**给引擎；
            source_page=="story" -> 该会话切累积态（收敛 StoryAccumulator.start 同一幂等入口）；
            其余取值无行为（闭集预留）；省略整条消息 = 普通会话（向后兼容）。
          - 收到 {"type":"kidpal_story_delete","id":<str>} -> 整条消息**不转发**给引擎；
            front 机械执行 delete_story + 重推 story_list 对账（隐式 ack：不存在 id 亦重推、
            total 不变；合法删除 -> total −1 可见）。畸形/非法 id 拒绝、不崩、审计留痕。
        pump_upstream_to_client / _story_feed_from_stt（引擎→设备下行泵）：
          累积态收到 stt 文本命中收尾词（story.is_finish_phrase）-> 走 StoryAccumulator.stop
          正常归档 + 把归档 story_text 下发给设备（**不丢弃**）；非累积态（问答会话）零影响。
          停录（listen/state=stop）归档成功后**追加** story_list 推送（三时点②）。

Shape authority = docs/contracts/interface-contract.md §1.3③/§1.3④/§1.5(3)(6)/§四
（2026-09-24 契约门 HUMAN_APPROVED，landing commit 5e862d0，已计入本包基线）。

跨端 wire 形状锚（对照对端实际接受什么，非按本侧契约想象；brief ② 必读面）：
  firmware/main/display/story_page.c:494 kidpal_story_list_parse —— data.total 在场须为
    非负**整数**（字符串 total -> -1 降级）；items 超 KSTORY_LIST_MAX(16) -> 整单 -1 降级
    （story_page.h:40）；item 缺 id/title -> -1；date 若在场须为 string；未知键忽略。
  firmware/main/display/story_page.c:645 kidpal_story_text_parse —— id/text 必为 string；
    **未知键（含 seq）一律忽略**（:707-712）-> seq 为 additive、旧板端安全忽略。
  firmware/main/display/story_page.c:615 kidpal_story_counter —— 优先 total、缺省回退
    items.length（>16 时窗口条数 ≠ 计数 = 本包警示句的板端依据）。
  firmware/main/mcp_server.cc:752 / :793 路由调用点（解析失败 -> 显式降级提示、绝不盲渲染）。

契约条目 ↔ 齿 的强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」T2 体例）：
  契约 §1.3③ total 语义 + items ≤16 窗口 → A3 齿（list_stories 真跑 + pump wire 帧）→ 行为级
  契约 §1.3④ data.seq 可选整数        → A6 齿（账本真跑）+ A1 floor 齿（真门判定）→ 行为级/机制级
  契约 §四 kidpal_story_delete         → A4 齿（pump 真消费 + delete_story 真落盘）→ 行为级
  契约 §四 kidpal_session_ctx(已 LANDED 零改) → A2 齿（pump 真拦截 + 真归档链）→ 行为级
  契约 §1.5-3 Q7 relay 收紧           → A1 齿（build_display_tool_calls 真跑降级）→ 行为级
  契约 §1.5-3 Q8 story floor          → A1 齿（validate_display 真门拒绝/放行）→ 行为级
  契约 §1.3③④/§1.6 story 段落文        → A1 契约锚齿（读契约正文钉前锚点串）→ 源锚代理
  arch §5.2④ 三时点推送（行为策略；2026-09-28 修订 = 时点①建连即推摘除、归 M4-K3 session_ctx；
  2026-10-12 v1.63 赎回 = 时点①重立为 WS 建连恢复推送（data.restore 标记只填缓存不抢前台），
  ctx(story) 同拍推送保留由 M4K3-A7 冻结齿承载）
                                        → A3 齿（时点②③ = pump wire + restore 缺省零偏移）+
                                        FIX-A1 翻正齿（建连恰一帧 data.restore==true story_list）
                                        + SL-A6 齿（恢复推送失败不阻断建连）
  requirements §2.1 120s 录音态超时     → A5 齿（纯决策面真跑）+ 前端接线缺口（见下）
  spec S-F2.4 AC2 流水号只增不减        → A6 齿（删除不回退/重启存活真跑）→ 行为级

  SAO 承接（261001-m4-k3-story-archive-order；归档时序缺陷冻结 oracle）：
    requirements S-F2.2-AC2（录音中收尾 -> 保存、新篇入列表、AI 不逐句回复）
                                        → SAO-A1/A2/A3/A4 齿（真机序 pump + 真落盘 + 真帧）→ 行为级
    requirements S-F2.2-AC4（KEY 兜底收尾 = 停录并保存）
                                        → SAO-A2/A3 齿（pending-stop 挂起真跑）→ 行为级
    arch §5.2④ 归档时点（触发仍是 listen-stop、归档动作等转写到达）
                                        → SAO-A1/A2 齿（真机序 pump wire）→ 行为级
    plan §2 正文剔尾随收尾词             → SAO-A4 齿（归档正文真读）→ 行为级

  弱于契约处（显式缺口，逐条）：
    ⓢ SAO-A3 pending 超时牙 = 行为级钉子（**线束假设显式化**）：offline 无真计时，本齿以
       「驱动 front 超时轮询 `_story_timeout_sweep(now=...)` 越过窗口」的注入时刻承载——
       假设 = front 把 pending 超时纳入该轮询（与既有 120s 自动收尾同缝，plan §2「front
       轮询」口径）+ pending 会话经 `timed_out_sessions` 族可被该巡检发现。若 builder 以
       其它缝（如 stt 到达时惰性判定）落 10s 超时，本齿须按实落缝重钉（如实申报，非门）。
    ⓣ SAO-A2 的「pending-stop 态」判据 = `routing(sid) != "chat"`（plan §2「不得把会话打回
       chat」的字面面）；不钉 pending 态的内部取值名（"accumulate" 保留 vs 新枚举值），
       二者皆满足 != "chat"，留 builder 实现自由。
    ① **120s 前端计时循环接线 offline 不可验**——只冻纯决策（timed_out_sessions）+ 源锚代理
       （front 引用该决策）。行为等价假设「front 轮询每 tick 调 timed_out_sessions 并对命中
       会话 stop + 下发」**显式化、未验** → live 全链复证归 M4-K3（M4K1-A7 deferred）。
    ② **WS 建连时点（三时点①）= 2026-09-28 真机缺陷止血摘除**（human 方向锚 b036f554，包
       260928-story-connect-push-defer）：设备每会话重开 WS（会话结束关 audio channel、按 KEY
       重开）→ 建连即推 story_list 会盖掉聆听态、旧固件 story_page.c 零 CJK 渲染半截页 ——
       ①摘除、归 M4-K3 session_ctx 承接；FIX-A1 负锚齿以**真实连接处理段流程**（离线装机 +
       假引擎腿 + pump I/O 双桩）钉「建连路径零 story_list 下行」，无源锚代理缺口。
       【2026-10-12 翻正】v1.63（requirements §9 frozen 2026-10-12）赎回 260928 挂起的时点①
       重裁：建连恢复推送回接，data.restore 标记 + 板端 restore 分流（只填缓存不抢前台、P3
       在屏仅重绘本页）消解旧缺陷复现路径（restore 分流 = schedule :926-991 在役形态复刻）；
       FIX-A1 齿翻正为「建连恰一帧 data.restore==true story_list」，授权链（CCP 261010-
       p3-storylist-cold-start HUMAN_APPROVED 2026-10-12 + §9 v1.63 行 + human 方向锚留痕）
       逐字见 REVISION LOG 2026-10-12 条。
    ③ **Q7「引擎侧来源」判据**：offline 只钉 relay 路径对 story_* 降级；不断言真实引擎帧的
       来源标记（该标记在 side-channel 类型）。行为等价假设「relay 路径（build_display_tool_calls）
       = 引擎侧来源面」显式化——front 自推走 _push_story_list、不经 relay（天然豁免）。
    ④ **floor 深度 = 形状 floor**：钉 total 整数 ≥0 / items 三键在场 / seq 整数；不钉逐字段深度
       （item 额外键放行与否、id 非空、date 格式）——契约未定义处，留 builder 实现自由。
    ⑤ **删除审计日志**：断言拒绝路径发出日志记录含 offending id（或拒绝类 token）；审计正文格式不钉。
    ⑥ **账本文件名 `.seq.json`** = 机制级冻结（plan 取舍点 B 甲案）；账本内部 JSON 结构不钉
       （行为级断言 seq 语义）。
    ⑦ **窗口双层钉**：producer（list_stories）+ wire（pump 帧）——若仅在 front 层窗口化、
       producer 仍返全量，则 producer 齿红（frozen seam 已明示）。

Coverage map (acceptanceRef -> test):
  M4K1-A1 -> test_a1_story_list_new_shape_passes_floor
           -> test_a1_story_text_seq_optional_passes_floor
           -> test_a1_story_list_total_must_be_nonneg_int
           -> test_a1_story_list_items_three_keys_required
           -> test_a1_story_text_seq_must_be_int_when_present
           -> test_a1_relay_rejects_engine_side_story_blocks_q7
           -> test_a1_relay_keeps_other_views_unchanged
           -> test_a1_contract_carries_three_touchpoints
  M4K1-A2 -> test_a2_session_ctx_intercepted_not_forwarded
           -> test_a2_source_page_values_and_backward_compat
           -> test_a2_idempotent_convergence_with_start_recording
  M4K1-A3 -> test_a3_list_stories_window_and_total_independent
           -> test_a3_push_seam_and_wire_timepoints
  FIX-A1 -> test_fix_a1_connect_path_emits_exactly_one_restore_story_list（2026-10-12 翻正：
           v1.63 冷启动恢复赎回——建连恰一帧 data.restore==true story_list；原「建连零帧」
           负锚形态沿革与授权链见 REVISION LOG 2026-10-12 条）
  FIX-A2 -> （a3 齿时机语义修订，承载于上两齿；见 REVISION LOG v3；2026-10-12 时点①建连
           恢复推送回接由 FIX-A1 翻正齿承载）
  SL-A1  -> test_fix_a1_connect_path_emits_exactly_one_restore_story_list（同 FIX-A1 翻正齿）
  SL-A6  -> test_sla6_connect_push_failure_does_not_block_connect
  M4K1-A4 -> test_a4_delete_executes_and_repushes
           -> test_a4_delete_nonexistent_id_idempotent_repush
           -> test_a4_delete_rejects_traversal_and_no_sentinel
  M4K1-A5 -> test_a5_finish_phrase_detection
           -> test_a5_finish_phrase_triggers_stop_in_accumulate_only
           -> test_a5_timeout_120s_decision
           -> test_a5_front_wiring_references_timeout_decision
  M4K1-A6 -> test_a6_archive_assigns_persistent_monotonic_seq
           -> test_a6_rearchive_same_id_keeps_seq
           -> test_a6_restart_survival
           -> test_a6_delete_does_not_rollback_seq
           -> test_a6_ledger_missing_rebuilds_without_rollback
  M4K1-E1 -> test_e1_existing_story_semantics_zero_offset

SAO 承接（2026-10-01 · 包 261001-m4-k3-story-archive-order · 归档时序缺陷冻结 oracle）：
  sourceRefs = S-F2.2-AC2 / S-F2.2-AC4（记录会话归档时序；本缺陷 = 归档早于转写）。
  SAO-A1 -> test_sao_a1_stop_before_stt_archives_transcript
          -> test_a2_session_ctx_intercepted_not_forwarded（归档链改真机序）
          -> test_a2_idempotent_convergence_with_start_recording（归档链改真机序）
          -> test_a3_push_seam_and_wire_timepoints（时点②改真机序）
          -> test_a5_finish_phrase_triggers_stop_in_accumulate_only（改真机序）
  SAO-A2 -> test_sao_a2_empty_stop_holds_pending_not_bail_to_chat（本缺陷冻结 oracle/负锚）
  SAO-A3 -> test_sao_a3_repeated_stop_no_double_archive
          -> test_sao_a3_stop_cancel_interleave_no_leak
          -> test_sao_a3_pending_window_boundary
  SAO-A4 -> test_sao_a4_body_excludes_trailing_finish_words
          -> test_a5_finish_phrase_triggers_stop_in_accumulate_only（正文剔词半）

Red baseline (现状树 @5e862d0+契约落地批, acceptance-author 2026-09-24 实跑；命令见 taskpackage
redBaselineCommand): 设计态红 = 未实现行为（无 is_finish_phrase/delete_story/timed_out_sessions、
list_stories 无窗口、story_text 无 seq、archive_story 无 seq 账本、display_schema 无 story floor、
build_display_tool_calls 未收紧 story_*、front 泵未拦截 session_ctx/story_delete、front 无
_push_story_list 且停录只推 story_text 不推 story_list），逐齿映射见交付报告；契约锚齿 + 零偏移
守卫齿现状绿（must-stay-green）。收集零错误（冻结即冒烟，DEV-GOVERNANCE §3）。

mutation 自证（mutationRequired=true）：逐齿变异设计 + expectedRed 见
server/acceptance/M4K1-ACCEPTANCE-NOTES.md §4（需 GREEN 后重放的变异显式声明）。

Run protocol:
  python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance
  (两点文件名须 importlib 模式；standalone: python 本文件)
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import logging
import sys
import tempfile
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import pytest  # noqa: F401  (markers only; standalone unittest runner also works)
    _HAS_PYTEST = True
except ImportError:  # pragma: no cover
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
CONTRACT = REPO_ROOT / "docs" / "contracts" / "interface-contract.md"


# --------------------------------------------------------------------------- #
# Module loading (missing/unimportable = RED for missing behaviour)           #
# --------------------------------------------------------------------------- #

_story_module_cache = None
_front_module_cache = None


def _story_module():
    """server/gateway/story.py 纯模块（module-level callable 缺失 = 设计态红）。"""
    global _story_module_cache
    if _story_module_cache is not None:
        return _story_module_cache
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.story as st  # noqa: PLC0415 — offline import, cached
    _story_module_cache = st
    return st


def _story_attr(name: str):
    """(value|None, err) —— story.py 缺失该模块级属性 = 设计态红（缺行为，非收集错）。"""
    st = _story_module()
    val = getattr(st, name, None)
    if val is None:
        return None, (f"server/gateway/story.py 无模块级 {name} —— frozen M4K1 offline "
                      "contract（story 面增量：窗口/账本/删除/收尾词/超时决策）未实现。")
    return val, None


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（g-relay-fix / RMD 验收件同款先例）。

    FIX-A1（260928-story-connect-push-defer）追加：路由装饰器用**恒等装饰器**（返回原函数，
    与真 fastapi 的 `@app.websocket(...)` 返回原函数同形）——建连路径负锚齿需直调
    `front.ws_gateway(...)` 走真实连接处理段流程，Mock 装饰会把该名字吞成 Mock。
    跨件 stub 共存（2026-09-28 实测）：他件（server/tests/test_reminder_units.py 等）用
    `sys.modules.setdefault("fastapi", permissive)` 先装**无 FastAPI 属性**的宽松 stub，
    本件在其后运行时必须**覆盖**之（否则 `from fastapi import FastAPI` 得 Mock、ws_gateway
    被装饰吞名）。宽松 stub 的消费方只取 WebSocketDisconnect/HTTPException/未知属性 Mock，
    本 stub 为其超集（同款宽松 getattr + 真异常类 + 恒等路由装饰）→ 覆盖向后兼容。"""
    try:
        import fastapi  # noqa: F401
        # 仅真 fastapi（或等价 stub）在场才复用：他件 permissive stub 的 __getattr__ 对任何
        # 属性都返 Mock（FastAPI「存在」但是 Mock 实例、非类），本件必须覆盖之。
        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return  # real fastapi（或带可用 FastAPI 类的 stub）在场 — use it
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    class _StubFastAPI:
        """路由装饰器恒等（get/post/websocket 均返回原函数；与真 fastapi 装饰语义同形）。"""

        def __init__(self, *args, **kwargs):  # noqa: D107
            pass

        def _identity(self, *args, **kwargs):  # noqa: ANN202
            def deco(fn):
                return fn
            return deco

        get = post = websocket = _identity

    stub = _PermissiveModule("fastapi")
    stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
    stub.HTTPException = type("HTTPException", (Exception,), {})
    stub.FastAPI = _StubFastAPI
    sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)."""
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_m4k1", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线可装载"
                      "（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("M4K1 RED: " + err)
    return mod


def _display_schema():
    """真实冻结机械门 gateway.display_schema（V27）—— Q8 floor 判定面。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415
    return ds


def _front_source() -> str:
    try:
        return FRONT_APP.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


@contextlib.contextmanager
def _front_with_stories(stories_dir):
    """把 front 的 STORIES_DIR 指向 tempdir（隔离真实 /opt/kidpal/stories 路径）。"""
    front = _front()
    old_dir, old_acc = front.STORIES_DIR, front._story_acc
    front.STORIES_DIR = str(stories_dir)
    front._story_acc = None
    try:
        yield front
    finally:
        front.STORIES_DIR = old_dir
        front._story_acc = old_acc


# --------------------------------------------------------------------------- #
# I/O doubles for the pump teeth (g-relay-fix 冻结 seam)                       #
# --------------------------------------------------------------------------- #

class _FakeUpstream:
    """Engine ws leg double: async-iterable of preloaded frames, records .send()."""

    def __init__(self, frames=()):
        self._frames = list(frames)
        self._pos = 0
        self.sent = []

    def __aiter__(self):
        return self

    async def __anext__(self):
        if self._pos >= len(self._frames):
            raise StopAsyncIteration
        frame = self._frames[self._pos]
        self._pos += 1
        return frame

    async def send(self, message):
        self.sent.append(message)


class _FakeDeviceSocket:
    """Device ws leg double: starlette-style .receive() dicts + send_text/send_bytes."""

    def __init__(self, incoming=(), disconnect_exc=None):
        self._incoming = list(incoming)
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.sent_text = []
        self.sent_bytes = []

    async def receive(self):
        if self._pos >= len(self._incoming):
            raise self._disconnect_exc()
        msg = self._incoming[self._pos]
        self._pos += 1
        return msg

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(data)


def _device_to_engine(front, session_id, incoming_texts):
    """Run the device->engine uplink pump; returns (upstream, device_socket)."""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in incoming_texts])
    up = _FakeUpstream()
    asyncio.run(front.pump_client_to_upstream(dev, up, session_id))
    return up, dev


def _engine_to_device(front, session_id, frames):
    """Run the engine->device downlink pump; returns (upstream, device_socket)."""
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, session_id))
    return up, dev


def _forwarded_frame_types(sent):
    """Frame types the front forwarded up to the engine (text frames parsed)."""
    out = []
    for item in sent:
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            out.append("<non-json>")
            continue
        out.append(obj.get("type") if isinstance(obj, dict) else "<non-object>")
    return out


def _mcp_envelopes(sent_text):
    """kidpal/display tools/call envelopes as the device accepts them (type=mcp frames)."""
    out = []
    for text in sent_text:
        try:
            frame = json.loads(text)
        except (TypeError, ValueError):
            continue
        if (isinstance(frame, dict) and frame.get("type") == "mcp"
                and isinstance(frame.get("payload"), dict)):
            payload = frame["payload"]
            params = payload.get("params") or {}
            if payload.get("method") == "tools/call" and params.get("name") == "kidpal/display":
                out.append(payload)
    return out


def _wire_arguments(envelope) -> dict:
    return ((envelope or {}).get("params") or {}).get("arguments") or {}


def _display_payloads(sent_text):
    """Reconstruct [{"view","data"}] from kidpal/display envelopes on the device leg."""
    out = []
    for env in _mcp_envelopes(sent_text):
        args = _wire_arguments(env)
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _story_text_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_text"]


@contextlib.contextmanager
def _captured_logs():
    """Capture every log record (audit-log assertions)."""
    records = []

    class _H(logging.Handler):
        def emit(self, record):
            records.append(record)

    handler = _H(level=logging.DEBUG)
    root = logging.getLogger()
    prev_level = root.level
    root.addHandler(handler)
    if root.level > logging.DEBUG or root.level == 0:
        root.setLevel(logging.DEBUG)
    try:
        yield records
    finally:
        root.removeHandler(handler)
        root.setLevel(prev_level)


def _audit_hit(records, needle: str) -> bool:
    tokens = (needle, "evil", "reject", "拒绝", "invalid", "story_delete")
    for rec in records:
        try:
            msg = rec.getMessage()
        except Exception:  # noqa: BLE001
            continue
        if any(tok and tok in msg for tok in tokens):
            return True
    return False


# --------------------------------------------------------------------------- #
# Fixtures                                                                     #
# --------------------------------------------------------------------------- #

def _seed(stories_dir: Path, n: int, *, start_day: int = 1, slug_prefix: str = "s"):
    """Seed n deterministic stories (date desc ordering: newest = highest day)."""
    st = _story_module()
    made = []
    for i in range(n):
        day = start_day + i
        rec = st.archive_story(
            str(stories_dir), f"正文-{i}", title=f"故事{i}", slug=f"{slug_prefix}{i:02d}",
            date=f"2026-09-{day:02d}")
        made.append(rec)
    return made


# =========================================================================== #
# M4K1-A1 — 契约门齿（gate evidence §6 C1：Q7 relay 收紧 + Q8 story floor）        #
# =========================================================================== #

class M4K1ContractGateAcceptance(unittest.TestCase):
    """M4K1-A1: Q8 story floor（真门）+ Q7 relay 收紧（真跑）+ 契约落文锚（源锚代理）。"""

    maxDiff = None

    # --- Q8 story floor ---------------------------------------------------- #

    def test_a1_story_list_new_shape_passes_floor(self):
        """coverage: M4K1-A1 / 契约 §1.5-3 Q8 — 冻结新形状 story_list 必须过真门
        （floor 不得误杀机械推送的单源载荷）。"""
        sample = {"view": "story_list", "data": {
            "total": 5,
            "items": [{"id": "2026-09-20-xiaotuzi", "title": "我和小兔子的冒险",
                       "date": "09-20"}]}}
        ok, reason = _display_schema().validate_display(sample)
        self.assertTrue(ok, f"M4K1-A1 RED: 冻结 story_list 新形状被真门拒绝 ({reason}) —— "
                            "Q8 floor 不得误杀机械推送载荷（契约 §1.5-3）。")

    def test_a1_story_text_seq_optional_passes_floor(self):
        """coverage: M4K1-A1 / 契约 §1.3④ — story_text 带 seq / 缺 seq 均过门（additive）。"""
        ds = _display_schema()
        with_seq = {"view": "story_text", "data": {
            "id": "2026-09-24-maoxian", "title": "我的大冒险", "text": "……", "seq": 6}}
        ok, reason = ds.validate_display(with_seq)
        self.assertTrue(ok, f"M4K1-A1 RED: story_text 带 seq 被真门拒绝 ({reason})。")
        no_seq = {"view": "story_text", "data": {
            "id": "2026-09-24-maoxian", "title": "我的大冒险", "text": "……"}}
        ok2, reason2 = ds.validate_display(no_seq)
        self.assertTrue(ok2, f"M4K1-A1 RED: story_text 缺 seq 被真门拒绝 ({reason2})——"
                             "seq 为可选 additive，缺省时设备行为不变。")

    def test_a1_story_list_total_must_be_nonneg_int(self):
        """coverage: M4K1-A1 / 契约 §1.5-3 Q8 + story_page.c:494 — total 在场须为非负整数
        （字符串/负数/浮点拒收，对端 story_list_parse 同判据）。"""
        ds = _display_schema()
        base_items = [{"id": "a", "title": "t", "date": "09-20"}]
        for bad, label in (("5", "字符串 total"), (-1, "负数 total"), (2.5, "浮点 total")):
            payload = {"view": "story_list", "data": {"total": bad, "items": base_items}}
            ok, reason = ds.validate_display(payload)
            self.assertFalse(
                ok, f"M4K1-A1 RED: {label} 通过了 Q8 story floor ({reason}) —— total 必为"
                    "整数且 >= 0（板端 kidpal_story_list_parse 对非整数 total -> -1 整单降级）。")

    def test_a1_story_list_items_three_keys_required(self):
        """coverage: M4K1-A1 / 契约 §1.5-3 Q8 — items 条目三键 {id,title,date} 齐；
        缺键 / 非对象 -> 拒绝（脏块降级不盲渲染）。"""
        ds = _display_schema()
        missing = [
            {"title": "t", "date": "09-20"},
            {"id": "a", "date": "09-20"},
            {"id": "a", "title": "t"},
        ]
        for item in missing:
            payload = {"view": "story_list", "data": {"total": 1, "items": [item]}}
            ok, reason = ds.validate_display(payload)
            self.assertFalse(
                ok, f"M4K1-A1 RED: 缺键 item {item!r} 通过 floor ({reason}) —— items 条目"
                    "三键 {id,title,date} 齐是形状 floor（契约 §1.5-3 Q8）。")
        nonobj = {"view": "story_list", "data": {"total": 1, "items": ["not-an-object"]}}
        ok2, reason2 = ds.validate_display(nonobj)
        self.assertFalse(ok2, f"M4K1-A1 RED: 非对象 item 通过 floor ({reason2})。")

    def test_a1_story_text_seq_must_be_int_when_present(self):
        """coverage: M4K1-A1 / 契约 §1.5-3 Q8 — story_text seq 在场须为整数。"""
        ds = _display_schema()
        for bad, label in (("6", "字符串 seq"), (6.5, "浮点 seq")):
            payload = {"view": "story_text", "data": {
                "id": "a", "title": "t", "text": "x", "seq": bad}}
            ok, reason = ds.validate_display(payload)
            self.assertFalse(
                ok, f"M4K1-A1 RED: {label} 通过 floor ({reason}) —— seq 必须为整数"
                    "（入册流水号 N 是整数；契约 §1.3④/§1.5-3 Q8）。")

    # --- Q7 relay 收紧 ---------------------------------------------------- #

    def _downgraded(self, envelope) -> bool:
        """The relayed envelope is a friendly DOWNGRADE (never the story payload):
        a schema-legal non-empty text view, not the story view."""
        payload = _display_payloads([json.dumps({"type": "mcp", "payload": envelope})])
        if not payload:
            return False
        view = payload[0].get("view")
        data = payload[0].get("data")
        return (view == "text" and isinstance(data, dict)
                and isinstance(data.get("text"), str) and data["text"].strip() != "")

    def test_a1_relay_rejects_engine_side_story_blocks_q7(self):
        """coverage: M4K1-A1 / 契约 §1.5-3 Q7 — relay 收紧：引擎侧来源的 story_list /
        story_text display 块必须降级（绝不把 story 载荷透传给设备），story 数据只走
        front 机械推送单源。"""
        front = _front()
        story_list_block = json.dumps({"view": "story_list", "data": {
            "total": 1, "items": [{"id": "a", "title": "t", "date": "09-20"}]}},
            ensure_ascii=False)
        story_text_block = json.dumps({"view": "story_text", "data": {
            "id": "a", "title": "t", "text": "从前有一只小猫"}}, ensure_ascii=False)
        for block, label in ((story_list_block, "story_list"), (story_text_block, "story_text")):
            calls = front.build_display_tool_calls([block])
            self.assertEqual(len(calls), 1, f"M4K1-A1 RED: {label} 块中继数 != 1（不得静默丢块）。")
            payload = _display_payloads(
                [json.dumps({"type": "mcp", "payload": calls[0]})])[0]
            self.assertNotEqual(
                payload.get("view"), label,
                f"M4K1-A1 RED: relay 仍把 {label} 载荷透传给设备 —— Q7 收紧（拒绝引擎侧"
                "story_* 块、降级友好提示）未实现（契约 §1.5-3）。")
            self.assertTrue(
                self._downgraded(calls[0]),
                f"M4K1-A1 RED: {label} 块未被降级为合法友好 text 块（脏块降级语义）。")

    def test_a1_relay_keeps_other_views_unchanged(self):
        """coverage: M4K1-A1 / 契约 §1.5-3 — Q7 收紧不得过宽：非 story view 的合法块
        仍按既有语义中继（回归守卫）；脏块降级语义保持。"""
        front = _front()
        text_block = json.dumps({"view": "text", "data": {"text": "你好呀"}}, ensure_ascii=False)
        calls = front.build_display_tool_calls([text_block])
        payload = _display_payloads([json.dumps({"type": "mcp", "payload": calls[0]})])[0]
        self.assertEqual(payload.get("view"), "text",
                         "M4K1-A1 RED: 合法 text 块被 Q7 收紧误杀 —— 收紧必须只作用于 story_*。")
        self.assertEqual(payload.get("data"), {"text": "你好呀"},
                         "M4K1-A1 RED: text 块 data 语义偏移（中继链零偏移被破）。")

    # --- 契约落文锚（源锚代理）-------------------------------------------- #

    def test_a1_contract_carries_three_touchpoints(self):
        """coverage: M4K1-A1（源锚代理）— 契约三触点落文在案（总语义/窗口 + seq + delete
        + §1.6 story 段），防落地批被回退而无机械兜底（pass-oracle 第 4 关无脚本，
        ARQ-260827-contract-mech-gate-gap）。"""
        text = CONTRACT.read_text(encoding="utf-8", errors="replace")
        anchors = {
            "§1.3③ total=现存篇数": "现存篇数 = 星星计数",
            "§1.3③ items ≤16 窗口": "最新 ≤16 条窗口",
            "§1.3④ data.seq 只增不减": "入册流水号，只增不减",
            "§四 kidpal_story_delete": "kidpal_story_delete",
            "§1.6 story 段": "涉及星星记录本时：只用文字答复",
        }
        for label, needle in anchors.items():
            self.assertIn(
                needle, text,
                f"M4K1-A1 源锚代理 RED: 契约正文缺 {label} 锚点串 {needle!r} —— 2026-09-24 "
                "契约门三触点落文被回退/未落地。")


# =========================================================================== #
# M4K1-A2 — session_ctx 拦截齿（front 上行泵）                                   #
# =========================================================================== #

class M4K1SessionCtxAcceptance(unittest.TestCase):
    """M4K1-A2: 上行泵整条消费 kidpal_session_ctx、不下透引擎；story 切累积态。"""

    maxDiff = None

    def test_a2_session_ctx_intercepted_not_forwarded(self):
        """coverage: M4K1-A2 + SAO-A1 / 契约 §四 — session_ctx 整条拦截（无论取值，不转发
        引擎）；source_page=story 切累积态；随后真机序（listen-stop 先、stt 后）归档链可证。"""
        with tempfile.TemporaryDirectory(prefix="m4k1_a2_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-a2-story"
                msg = json.dumps({"type": "kidpal_session_ctx", "source_page": "story"},
                                 ensure_ascii=False)
                up, _dev = _device_to_engine(front, sid, [msg])
                self.assertNotIn(
                    "kidpal_session_ctx", _forwarded_frame_types(up.sent),
                    "M4K1-A2 RED: kidpal_session_ctx 被透传给引擎 —— 契约 §四「网关消费并"
                    "拦截、不下透引擎」未实现。")
                self.assertEqual(
                    front._get_story_acc().routing(sid), "accumulate",
                    "M4K1-A2 RED: source_page=story 未把该会话切累积态（§四/§1.4 收敛入口）。")
                # 真机序归档链可证（SAO-A1，261001 r2 实录序）：先 listen-stop（累积空 ->
                # 挂起 pending-stop）后转写 stt 到 -> 恰一篇归档。旧序（stt -> listen-stop）
                # 固化了错误时序假设、不得再过闸（261001-m4-k3-story-archive-order）。
                _device_to_engine(
                    front, sid, [json.dumps({"type": "listen", "state": "stop"})])
                _up2, dev2 = _engine_to_device(
                    front, sid, [{"type": "stt", "text": "从前有一只小猫"}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(len(files), 1,
                                 f"M4K1-A2 RED: session_ctx->listen-stop->stt（真机序）未产出恰"
                                 f"一篇归档（得 {len(files)}）—— 归档链未接通（旧序 stt->stop 不"
                                 "得过闸；见 SAO-A1）。")
                self.assertTrue(
                    _story_text_frames(dev2.sent_text),
                    "M4K1-A2 RED: 停录归档后未下发 story_text（既有链路偏移）。")

    def test_a2_source_page_values_and_backward_compat(self):
        """coverage: M4K1-A2 / 契约 §四 — 其余取值（home/schedule/未知）仍整条拦截但无
        行为（不切累积态）；省略整条消息 = 普通会话（向后兼容，其它帧照常转发）。"""
        with tempfile.TemporaryDirectory(prefix="m4k1_a2b_") as tmp:
            with _front_with_stories(tmp) as front:
                for value in ("home", "schedule", "whatever"):
                    sid = f"sess-a2-{value}"
                    msg = json.dumps({"type": "kidpal_session_ctx", "source_page": value},
                                     ensure_ascii=False)
                    up, _ = _device_to_engine(front, sid, [msg])
                    self.assertNotIn(
                        "kidpal_session_ctx", _forwarded_frame_types(up.sent),
                        f"M4K1-A2 RED: source_page={value} 的 session_ctx 被透传 —— "
                        "契约 §四「无论取值一律拦截」。")
                    self.assertEqual(
                        front._get_story_acc().routing(sid), "chat",
                        f"M4K1-A2 RED: source_page={value} 误切累积态 —— 其余取值现版本无行为。")
                # 向后兼容负锚：无 session_ctx 时普通帧照常转发、保持闲聊态。
                sid = "sess-a2-plain"
                plain = json.dumps({"type": "listen", "state": "start"}, ensure_ascii=False)
                up2, _ = _device_to_engine(front, sid, [plain])
                self.assertIn("listen", _forwarded_frame_types(up2.sent),
                              "M4K1-A2 RED: 普通帧被拦截 —— 省略 session_ctx 必须向后兼容。")
                self.assertEqual(front._get_story_acc().routing(sid), "chat",
                                 "M4K1-A2 RED: 无 session_ctx 时误入累积态。")

    def test_a2_idempotent_convergence_with_start_recording(self):
        """coverage: M4K1-A2 + SAO-A1 / 契约 §四+§1.4 — session_ctx(story) 与
        control/start_recording 收敛同一幂等入口：各自单独可达累积态、重复触发不重复开会话、
        不丢已累积内容。归档链走真机序（listen-stop 先、stt 后；SAO-A1）。"""
        with tempfile.TemporaryDirectory(prefix="m4k1_a2c_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-a2-idem"
                ctx = json.dumps({"type": "kidpal_session_ctx", "source_page": "story"},
                                 ensure_ascii=False)
                # 1) session_ctx(story) 单独 -> 累积态（收敛 §1.4 同一入口）。
                _device_to_engine(front, sid, [ctx])
                self.assertEqual(
                    front._get_story_acc().routing(sid), "accumulate",
                    "M4K1-A2 RED: session_ctx(story) 单独未进入累积态 —— 未收敛到 §1.4 幂等入口。")
                # 2) 引擎侧 control/start_recording 再触发 -> 仍累积态（既有接线零偏移）。
                _engine_to_device(front, sid, [{
                    "type": "kidpal_display_blocks",
                    "blocks": [json.dumps({"view": "control",
                                           "data": {"action": "start_recording"}},
                                          ensure_ascii=False)]}])
                self.assertEqual(front._get_story_acc().routing(sid), "accumulate",
                                 "M4K1-A2 RED: control/start_recording 既有累积接线偏移。")
                # 3) 重复两次 session_ctx / 一次 start_recording -> 仍幂等（不退出累积态）。
                _device_to_engine(front, sid, [ctx, ctx])
                _engine_to_device(front, sid, [{
                    "type": "kidpal_display_blocks",
                    "blocks": [json.dumps({"view": "control",
                                           "data": {"action": "start_recording"}},
                                          ensure_ascii=False)]}])
                self.assertEqual(front._get_story_acc().routing(sid), "accumulate",
                                 "M4K1-A2 RED: 重复触发退出累积态 —— 幂等入口被破。")
                # 4) 真机序归档（SAO-A1）：listen-stop 先（累积空 -> 挂起）-> stt 后 -> 恰一篇。
                _device_to_engine(
                    front, sid, [json.dumps({"type": "listen", "state": "stop"})])
                _up, dev = _engine_to_device(front, sid, [{"type": "stt", "text": "第一句"}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(len(files), 1,
                                 f"M4K1-A2 RED: 重复触发产出 {len(files)} 篇归档 —— 幂等入口"
                                 "被破（重复 session_ctx 不得重复开会话/重复归档），或真机序归档"
                                 "链未接通（旧序不得过闸）。")
                st = _story_module()
                rec = st.read_story_file(str(files[0]))
                self.assertIn("第一句", (rec or {}).get("text", ""),
                              "M4K1-A2 RED: 真机序归档丢失转写内容 —— 内容未进归档正文。")


# =========================================================================== #
# M4K1-A3 — story_list 推送齿                                                   #
# =========================================================================== #

class M4K1StoryListPushAcceptance(unittest.TestCase):
    """M4K1-A3: producer 窗口/计数语义 + 三时点推送（建连命名缝 + pump wire 时点②③）。"""

    maxDiff = None

    def test_a3_list_stories_window_and_total_independent(self):
        """coverage: M4K1-A3 / 契约 §1.3③ Q2 丙案 — items = 最新 ≤16 条窗口（日期倒序），
        total = 全量现存篇数，二者独立（>16 时窗口条数 ≠ 星星计数 = load-bearing）。"""
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_a3_") as tmp:
            _seed(Path(tmp), 20)  # 2026-09-01..2026-09-20
            payload = st.list_stories(str(tmp))
            self.assertEqual(payload.get("view"), "story_list",
                             "M4K1-A3 RED: list_stories view != story_list。")
            data = payload.get("data") or {}
            items = data.get("items")
            self.assertIsInstance(items, list, "M4K1-A3 RED: items 非列表。")
            self.assertEqual(
                len(items), 16,
                f"M4K1-A3 RED: items 窗口 = {len(items)} != 16 —— front 恒推最新 ≤16 条窗口"
                "（契约 §1.3③ Q2 丙案；板端 KSTORY_LIST_MAX=16，超限整单 -1 降级）。")
            self.assertEqual(
                data.get("total"), 20,
                f"M4K1-A3 RED: total = {data.get('total')!r} != 20（全量现存篇数）—— "
                "total 绝不以 len(items) 顶替（>16 篇时窗口条数 ≠ 计数）。")
            self.assertNotEqual(len(items), data.get("total"),
                                "M4K1-A3 RED: 窗口条数与 total 相等（无法区分窗口/计数）。")
            # 最新窗口 = 最高日期那 16 条（2026-09-20..05），日期倒序、显示 MM-DD。
            dates = [it.get("date") for it in items]
            self.assertEqual(dates[0], "09-20", f"M4K1-A3 RED: items 未按日期倒序: {dates[:3]}。")
            self.assertEqual(dates[-1], "09-05",
                             f"M4K1-A3 RED: 窗口未取最新 16 条: 末条 date={dates[-1]!r}。")
            for it in items:
                self.assertEqual(
                    set(it), {"id", "title", "date"},
                    f"M4K1-A3 RED: item keys {sorted(it)} != {{id,title,date}} —— items 形状"
                    "零改动、不得塞 seq（契约 §1.3③）。")
            ok, reason = _display_schema().validate_display(payload)
            self.assertTrue(ok, f"M4K1-A3 RED: producer 载荷未过真门 ({reason})。")
            # 空目录 -> total 0、items []（非假装有故事）。
            empty = st.list_stories(str(Path(tmp) / "nope"))
            self.assertEqual(empty["data"], {"total": 0, "items": []},
                             "M4K1-A3 RED: 空目录语义偏移。")

    def test_a3_push_seam_and_wire_timepoints(self):
        """coverage: M4K1-A3 / FIX-A2（260928-story-connect-push-defer）/ arch §5.2④（修订）—
        时机语义修订（2026-09-28 真机缺陷止血，human 方向锚 b036f554）：三时点① WS 建连即推
        **摘除**（设备每会话重开 WS → 建连即推盖掉聆听态、旧固件 story_page.c 零 CJK 渲染半截
        页；①归 M4-K3 session_ctx 承接）。本齿保留 _push_story_list 命名缝行为断言 + 三时点②
        （归档成功后）③（删除执行后）wire 断言；建连路径零 story_list 下行由 FIX-A1 负锚齿
        承载（不双断言同一面）。wire 时点②③ 走真 pump（层无关）。"""
        front = _front()
        push = getattr(front, "_push_story_list", None)
        self.assertTrue(
            callable(push),
            "M4K1-A3 RED: front_app.py 无 async _push_story_list(websocket) —— 二/三时点推送"
            "的下行缝未实现（frozen seam；先例 _push_schedule_connect）。")
        with tempfile.TemporaryDirectory(prefix="m4k1_a3w_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                dev = _FakeDeviceSocket()
                asyncio.run(front._push_story_list(dev))
                frames = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(frames), 1,
                    f"M4K1-A3 RED: _push_story_list 下发 {len(frames)} 帧 story_list "
                    "（应恰一帧）。")
                self.assertEqual(frames[0]["data"]["total"], 2,
                                 "M4K1-A3 RED: 缝直调下发 total != 现存篇数。")
                self.assertLessEqual(len(frames[0]["data"]["items"]), 16,
                                     "M4K1-A3 RED: 缝直调下发 items 超窗口。")
                # 时点② 归档成功后：session_ctx -> listen-stop -> stt（真机序，SAO-A1）后除
                # story_text 外必须**追加** story_list 推送。
                sid = "sess-a3-tp2"
                _device_to_engine(front, sid, [json.dumps(
                    {"type": "kidpal_session_ctx", "source_page": "story"})])
                _device_to_engine(
                    front, sid, [json.dumps({"type": "listen", "state": "stop"})])
                _up, dev2 = _engine_to_device(
                    front, sid, [{"type": "stt", "text": "从前有一只小猫"}])
                self.assertTrue(
                    _story_text_frames(dev2.sent_text),
                    "M4K1-A3 RED: 归档成功后未下发 story_text（既有链路偏移）。")
                pushes = _story_list_frames(dev2.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"M4K1-A3 RED: 归档成功后 story_list 推送 {len(pushes)} 帧（应恰一帧）"
                    " —— 三时点② 未接线/非幂等不重复。")
                self.assertEqual(pushes[0]["data"]["total"], 3,
                                 "M4K1-A3 RED: 归档后推送 total != 更新后现存篇数。")
                # 2026-10-12 additive（SL-A1 同批）：时点②主动推送 restore 缺省零偏移
                # （契约 §1.3③ D1 生产纪律——仅 WS 建连恢复推送可置 data.restore；缺省
                # absent/显式 false 同判，②③调用点零偏移）。
                self.assertFalse(
                    pushes[0]["data"].get("restore"),
                    "M4K1-A3 RED: 归档后（时点②）推送带 data.restore —— 生产纪律破"
                    "（契约 §1.3③ D1: 仅 WS 建连恢复推送可置 restore; 主动推送缺省零偏移）。")


# =========================================================================== #
# FIX-A1（260928-story-connect-push-defer）— 建连即推摘除负锚                    #
# =========================================================================== #

class _FakeDeviceSession:
    """ws_gateway 形状的设备腿双桩（headers/accept/receive/send_text/send_bytes/close）。"""

    def __init__(self, incoming=(), token=None, disconnect_exc=None):
        self.headers = {"authorization": f"Bearer {token}"} if token else {}
        self._incoming = [{"type": "websocket.receive", "text": t} for t in incoming]
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.accepted = False
        self.closed = None
        self.sent_text = []
        self.sent_bytes = []

    async def accept(self):
        self.accepted = True

    async def receive(self):
        if self._pos >= len(self._incoming):
            raise self._disconnect_exc()
        msg = self._incoming[self._pos]
        self._pos += 1
        return msg

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(data)

    async def close(self, code=1000):
        self.closed = code


@contextlib.contextmanager
def _fake_engine_ws(upstream):
    """假引擎腿：注入 websockets.asyncio.client 假模块（ws_gateway 在连接处理段内联 import
    `from websockets.asyncio.client import connect`），使 `async with ws_connect(ENGINE_WS,…)`
    离线可达 —— 建连流程真实走完。这是 FIX-A1 行为级负锚的前提：否则「连接早退 -> 零帧」
    会让负锚空过（vacuous）。"""

    class _Conn:
        def __init__(self, up):
            self._up = up

        async def __aenter__(self):
            return self._up

        async def __aexit__(self, *exc):
            return False

    def _connect(*args, **kwargs):
        return _Conn(upstream)

    mod_client = ModuleType("websockets.asyncio.client")
    mod_client.connect = _connect
    mod_asyncio = ModuleType("websockets.asyncio")
    mod_asyncio.client = mod_client
    mod_ws = ModuleType("websockets")
    mod_ws.asyncio = mod_asyncio
    wanted = (("websockets", mod_ws), ("websockets.asyncio", mod_asyncio),
              ("websockets.asyncio.client", mod_client))
    added = [name for name, _mod in wanted if name not in sys.modules]
    for name, mod in wanted:
        sys.modules[name] = mod
    try:
        yield
    finally:
        for name in added:
            sys.modules.pop(name, None)


class M4K1ConnectPushDeferAcceptance(unittest.TestCase):
    """FIX-A1（260928-story-connect-push-defer → 2026-10-12 翻正, 见 REVISION LOG）:
    建连恢复推送齿（原「建连即推摘除负锚」）。

    行为级（非源锚代理）：真实连接处理段流程 —— `ws_gateway` 全程离线走完（鉴权通过 +
    accept + 假引擎腿 + 两条 pump），断言设备腿收到的 story_list 下行帧数 = 恰 1 且
    data.restore 字面 true（v1.63 冷启动恢复：恢复性推送只填缓存不抢前台——restore 标记
    是板端 restore 分流的 wire 载体, 缺它 = 板端按主动展示接管前台, 旧缺陷复现）；同时钉
    「连接真实走通」（设备帧被转发引擎）防「流程早退 -> 零帧/单帧」空过。三时点②（归档
    成功后）③（删除执行后）在场由修订后 a3/a4 齿承载（restore 缺省零偏移断言随批
    additive）——本齿不双断言同一面。SL-A6 = 恢复推送失败不阻断建连（try/except 同
    schedule connect push 先例）。"""

    maxDiff = None

    def test_fix_a1_connect_path_emits_exactly_one_restore_story_list(self):
        """coverage: SL-A1 + FIX-A1 — 建连路径（ws_gateway 连接处理段）恰一帧 story_list
        下行且 data.restore == true（2026-10-12 翻正; 原「零 story_list 下行」负锚形态
        沿革与授权链见 REVISION LOG 2026-10-12 条）。"""
        front = _front()
        push = getattr(front, "_push_story_list", None)
        self.assertTrue(
            callable(push),
            "FIX-A1 RED: front_app.py 无 async _push_story_list(websocket) —— 建连恢复"
            "推送与时点②③共用该下行缝, 缝不在世。")
        token = "F" * 24
        old_token = front.DEVICE_TOKEN
        with tempfile.TemporaryDirectory(prefix="m4k1_fixa1_") as tmp:
            with _front_with_stories(tmp) as front:
                front.DEVICE_TOKEN = token
                # 预置 2 篇故事：建连恢复推送必下发且 total = 现存篇数 —— 断言非空过。
                _seed(Path(tmp), 2)
                up = _FakeUpstream()
                dev = _FakeDeviceSession(
                    incoming=[json.dumps({"type": "listen", "state": "start"})],
                    token=token)
                with _fake_engine_ws(up):
                    asyncio.run(front.ws_gateway(dev))
                # 连接真实走通（防「流程早退 -> 零帧」空过）。
                self.assertTrue(
                    dev.accepted,
                    "FIX-A1 RED: 设备腿未被 accept —— 连接处理段流程未走通（鉴权/装机问题），"
                    "建连推送断言失效。")
                self.assertIn(
                    "listen", _forwarded_frame_types(up.sent),
                    "FIX-A1 RED: 设备帧未被转发引擎 —— 连接处理段流程未走通（pump 未跑），"
                    "建连推送断言失效。")
                # 翻正本体：建连路径恰一帧 story_list 下行（三时点① WS 建连恢复推送,
                # requirements v1.63 S-F2.1 补充规则 + arch §5.2④ 三时点①）。
                frames = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(frames), 1,
                    f"FIX-A1 RED: 建连路径产生 {len(frames)} 帧 story_list 下行（应恰"
                    "一帧恢复推送）—— 时点①建连恢复推送未回接（v1.63 赎回缺口）或非单帧。")
                # restore 标记 = 板端 restore 分流的 wire 载体（契约 §1.3③ data.restore
                # additive, CCP D1——缺它板端按主动展示确定性接管前台顶到 P3, 260928 旧
                # 缺陷形态复现; 板端 mcp_json_get_bool absent/false 同判 false, 须字面 true）。
                self.assertIs(
                    frames[0]["data"].get("restore"), True,
                    f"FIX-A1 RED: 建连恢复推送 data.restore = "
                    f"{frames[0]['data'].get('restore')!r}（应字面 true）—— 恢复性标记缺失"
                    "时板端 restore 分流无载体（旧固件确定性接管前台 = 旧缺陷复现）。")
                self.assertEqual(
                    frames[0]["data"]["total"], 2,
                    "FIX-A1 RED: 恢复推送 total != 现存篇数 —— producer 语义偏移。")
                ok, reason = _display_schema().validate_display(frames[0])
                self.assertTrue(
                    ok,
                    f"FIX-A1 RED: 带 data.restore 的恢复推送被 V27 真门拒绝 ({reason}) —— "
                    "story_list floor 未随契约门 D3 联动扩 restore 可选布尔（CCP D3）。")

    def test_sla6_connect_push_failure_does_not_block_connect(self):
        """coverage: SL-A6 — 建连恢复推送失败（_push_story_list 抛异常）不阻断建连：
        try/except 同 schedule connect push（front_app.py 建连段 :1009-1012 先例）——
        鉴权/accept/pump/连接生命周期照常。must-stay-green 守卫（现状树无建连推送 =
        平凡绿; builder 落裸调（无 try/except）即红——变异敏感面, REVISION LOG
        2026-10-12 条显式申报）。"""
        front = _front()
        token = "F" * 24
        with tempfile.TemporaryDirectory(prefix="m4k1_sla6_") as tmp:
            with _front_with_stories(tmp) as front:
                front.DEVICE_TOKEN = token
                _seed(Path(tmp), 2)

                async def _boom(websocket, *args, **kwargs):
                    raise RuntimeError("restore push boom")

                original = front._push_story_list
                front._push_story_list = _boom
                try:
                    up = _FakeUpstream()
                    dev = _FakeDeviceSession(
                        incoming=[json.dumps({"type": "listen", "state": "start"})],
                        token=token)
                    with _fake_engine_ws(up):
                        asyncio.run(front.ws_gateway(dev))
                finally:
                    front._push_story_list = original
                self.assertTrue(
                    dev.accepted,
                    "SL-A6 RED: 恢复推送异常时设备腿未 accept —— 建连被推送失败阻断"
                    "（try/except 先例缺失; 建连推送是增强, 不因它拒服务——schedule "
                    "connect push :1009-1012 同款先例）。")
                self.assertIn(
                    "listen", _forwarded_frame_types(up.sent),
                    "SL-A6 RED: 恢复推送异常时设备帧未转发引擎 —— 泵未跑（建连被推送"
                    "失败阻断; 连接语义优先于增强推送）。")


# =========================================================================== #
# M4K1-A4 — 删除执行齿                                                          #
# =========================================================================== #

class M4K1DeleteAcceptance(unittest.TestCase):
    """M4K1-A4: kidpal_story_delete 消费（拦截 + 硬删 + 幂等 + 重推对账 + 拒绝审计）。"""

    maxDiff = None

    def _seed_titled(self, tmp: Path):
        st = _story_module()
        a = st.archive_story(str(tmp), "故事A正文", title="故事A", slug="alpha",
                             date="2026-09-20")
        b = st.archive_story(str(tmp), "故事B正文", title="故事B", slug="beta",
                             date="2026-09-21")
        c = st.archive_story(str(tmp), "故事C正文", title="故事C", slug="gamma",
                             date="2026-09-22")
        return a, b, c

    def test_a4_delete_executes_and_repushes(self):
        """coverage: M4K1-A4 / 契约 §四 Q5/Q6 — 合法 delete：拦截不下透引擎 + MD 硬删 +
        story_list 重推对账（total −1 可见 = 隐式 ack 载体）。"""
        with tempfile.TemporaryDirectory(prefix="m4k1_a4_") as tmp:
            with _front_with_stories(tmp) as front:
                _a, b, _c = self._seed_titled(Path(tmp))
                sid = "sess-a4-del"
                frame = json.dumps({"type": "kidpal_story_delete", "id": b["id"]},
                                   ensure_ascii=False)
                up, dev = _device_to_engine(front, sid, [frame])
                self.assertNotIn(
                    "kidpal_story_delete", _forwarded_frame_types(up.sent),
                    "M4K1-A4 RED: kidpal_story_delete 被透传给引擎 —— 契约 §四「网关消费并"
                    "拦截、不下透引擎」未实现。")
                self.assertFalse(
                    (Path(tmp) / f"{b['id']}.md").exists(),
                    "M4K1-A4 RED: 合法 delete 未真删 MD（硬删，Q6）。")
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"M4K1-A4 RED: 删除后 story_list 重推 {len(pushes)} 帧（应恰一帧对账）。")
                self.assertEqual(
                    pushes[0]["data"]["total"], 2,
                    "M4K1-A4 RED: 删除后重推 total != 现存篇数（删除即 −1 可见 = 隐式 ack）。")
                # 2026-10-12 additive（SL-A1 同批）：时点③主动重推 restore 缺省零偏移
                # （契约 §1.3③ D1 生产纪律——仅 WS 建连恢复推送可置 data.restore）。
                self.assertFalse(
                    pushes[0]["data"].get("restore"),
                    "M4K1-A4 RED: 删除后（时点③）重推带 data.restore —— 生产纪律破"
                    "（契约 §1.3③ D1: 仅 WS 建连恢复推送可置 restore; ③缺省 False 零偏移）。")
                ids = {it["id"] for it in pushes[0]["data"]["items"]}
                self.assertNotIn(b["id"], ids,
                                 "M4K1-A4 RED: 重推列表仍含被删 id。")

    def test_a4_delete_nonexistent_id_idempotent_repush(self):
        """coverage: M4K1-A4 / 契约 §四 Q5 — 不存在 id = 幂等 no-op（不报错）+ 仍重推
        列表对账（隐式 ack 重发承接面）。"""
        with tempfile.TemporaryDirectory(prefix="m4k1_a4n_") as tmp:
            with _front_with_stories(tmp) as front:
                self._seed_titled(Path(tmp))
                sid = "sess-a4-noop"
                before = sorted(p.name for p in Path(tmp).glob("*.md"))
                frame = json.dumps({"type": "kidpal_story_delete", "id": "2099-01-01-ghost"},
                                   ensure_ascii=False)
                up, dev = _device_to_engine(front, sid, [frame])
                self.assertNotIn("kidpal_story_delete", _forwarded_frame_types(up.sent),
                                 "M4K1-A4 RED: 不存在 id 的 delete 被透传。")
                self.assertEqual(
                    before, sorted(p.name for p in Path(tmp).glob("*.md")),
                    "M4K1-A4 RED: 不存在 id 误删了某文件 —— 幂等 no-op 被破。")
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"M4K1-A4 RED: 不存在 id 未重推 story_list（{len(pushes)} 帧）—— "
                    "隐式 ack「不存在 id 亦重推列表」未实现（Q5）。")
                self.assertEqual(pushes[0]["data"]["total"], 3,
                                 "M4K1-A4 RED: no-op 后 total 被改动。")

    def test_a4_delete_rejects_traversal_and_no_sentinel(self):
        """coverage: M4K1-A4 / 契约 §四 Q6 — 非法 id（路径穿越 / 无哨兵头文件）拒绝、
        绝不删 stories_dir 以外文件 + 审计日志留痕；畸形 id 不崩。"""
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_a4r_") as tmp:
            root = Path(tmp)
            stories = root / "stories"
            stories.mkdir()
            outside = root / "outside.md"
            outside.write_text("不应被删\n", encoding="utf-8")
            (stories / "notes.md").write_text("普通杂物，无哨兵头\n", encoding="utf-8")
            with _front_with_stories(stories) as front:
                # 路径穿越
                with _captured_logs() as recs:
                    frame = json.dumps({"type": "kidpal_story_delete", "id": "../outside"},
                                       ensure_ascii=False)
                    up, dev = _device_to_engine(front, "sess-a4-trav", [frame])
                self.assertTrue(outside.exists(),
                                "M4K1-A4 RED: 路径穿越 id 删掉了 stories 目录外的文件 —— "
                                "删除边界（仅 story MD）被破。")
                self.assertNotIn("kidpal_story_delete", _forwarded_frame_types(up.sent),
                                 "M4K1-A4 RED: 非法 delete 被透传。")
                self.assertTrue(
                    _audit_hit(recs, "../outside"),
                    "M4K1-A4 RED: 路径穿越拒绝未留审计日志（拒绝必须留痕，Q6）。")
                # 无哨兵头文件
                with _captured_logs() as recs2:
                    frame2 = json.dumps({"type": "kidpal_story_delete", "id": "notes"},
                                        ensure_ascii=False)
                    _device_to_engine(front, "sess-a4-nosent", [frame2])
                self.assertTrue((stories / "notes.md").exists(),
                                "M4K1-A4 RED: 无哨兵头文件被删 —— 哨兵头校验缺失"
                                "（read_story_file 判据）。")
                self.assertTrue(_audit_hit(recs2, "notes"),
                                "M4K1-A4 RED: 无哨兵头拒绝未留审计日志。")
                # 畸形 id（非字符串 / 空串）不崩。
                for bad_frame in ('{"type":"kidpal_story_delete","id":123}',
                                  '{"type":"kidpal_story_delete","id":""}',
                                  '{"type":"kidpal_story_delete"}'):
                    _device_to_engine(front, "sess-a4-bad", [bad_frame])
                self.assertTrue((stories / "notes.md").exists(),
                                "M4K1-A4 RED: 畸形 id 导致误删/崩溃。")
        # 纯面：delete_story 直接可调（缺 = 设计态红）。
        fn, err = _story_attr("delete_story")
        if not callable(fn):
            self.fail("M4K1-A4 RED: " + (err or "delete_story 非可调用。"))
        with tempfile.TemporaryDirectory(prefix="m4k1_a4p_") as tmp2:
            rec = st.archive_story(tmp2, "正文", title="T", slug="x", date="2026-09-20")
            self.assertTrue(fn(tmp2, rec["id"]), "M4K1-A4 RED: delete_story 合法 id 未返回 True。")
            self.assertFalse((Path(tmp2) / f"{rec['id']}.md").exists(),
                             "M4K1-A4 RED: delete_story 未真删文件。")
            self.assertFalse(fn(tmp2, "../evil"), "M4K1-A4 RED: delete_story 路径穿越未拒绝。")
            self.assertFalse(fn(tmp2, rec["id"]),
                             "M4K1-A4 RED: delete_story 重复删未幂等（应 False + 不抛）。")


# =========================================================================== #
# M4K1-A5 — 收尾与超时齿                                                        #
# =========================================================================== #

class M4K1FinishTimeoutAcceptance(unittest.TestCase):
    """M4K1-A5: 收尾词机械检测 + 120s 超时纯决策 + 前端接线源锚代理 + 丢弃零偏移。"""

    maxDiff = None

    def test_a5_finish_phrase_detection(self):
        """coverage: M4K1-A5 / S-F2.2 收尾 — is_finish_phrase 精确短语「我说完了」/
        「讲完啦」子串命中；其余不误判（空/None/无关文本）。"""
        fn, err = _story_attr("is_finish_phrase")
        if not callable(fn):
            self.fail("M4K1-A5 RED: " + (err or "is_finish_phrase 非可调用。"))
        for text in ("我说完了", "好啦，讲完啦", "我说的故事讲完啦。"):
            self.assertTrue(fn(text), f"M4K1-A5 RED: 收尾词未命中: {text!r}。")
        for text in ("我今天去公园玩了", "", "   ", None, "讲完了"):
            self.assertFalse(fn(text), f"M4K1-A5 RED: 非收尾词误判为收尾: {text!r}。")

    def test_a5_finish_phrase_triggers_stop_in_accumulate_only(self):
        """coverage: M4K1-A5 + SAO-A1 + SAO-A4 / S-F2.2 收尾 — 累积态真机序（listen-stop
        先、转写后到）收到含收尾词的转写 -> 自动归档 + 下发正文（不丢弃）+ 正文剔尾随收尾词；
        非累积态（问答会话）零影响。"""
        with tempfile.TemporaryDirectory(prefix="m4k1_a5_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-a5-finish"
                _device_to_engine(front, sid, [json.dumps(
                    {"type": "kidpal_session_ctx", "source_page": "story"})])
                # 真机序（SAO-A1）：listen-stop 先（累积空 -> 挂起 pending-stop）-> 转写后到
                # （manual 模式 ASR 由 listen stop 触发，整段含尾随「我说完了」）。
                _device_to_engine(
                    front, sid, [json.dumps({"type": "listen", "state": "stop"})])
                _up, dev = _engine_to_device(front, sid, [
                    {"type": "stt", "text": "从前有一只小猫。我说完了"}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(
                    len(files), 1,
                    f"M4K1-A5 RED: 累积态真机序未触发归档（得 {len(files)} 篇）—— 收尾/挂起"
                    "归档链缺失（S-F2.2 收尾 + SAO-A1 真机序；旧序 stt->stop 不得过闸）。")
                self.assertTrue(_story_text_frames(dev.sent_text),
                                "M4K1-A5 RED: 收尾词归档后未下发 story_text。")
                st = _story_module()
                rec = st.read_story_file(str(files[0]))
                body = (rec or {}).get("text", "")
                self.assertIn("从前有一只小猫", body,
                              "M4K1-A5 RED: 收尾归档未包含已累积正文（内容被丢弃）。")
                self.assertNotIn(
                    "我说完了", body,
                    "M4K1-A5 RED: 归档正文含尾随收尾词「我说完了」—— 正文剔词未落（SAO-A4）。")
                # 问答会话零影响：无 session_ctx 时同一句收尾词不得归档。
                sid2 = "sess-a5-chat"
                _engine_to_device(front, sid2, [{"type": "stt", "text": "我说完了"}])
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 1,
                    "M4K1-A5 RED: 问答会话被收尾词误归档 —— 收尾词仅累积态消费。")

    def test_a5_timeout_120s_decision(self):
        """coverage: M4K1-A5 / requirements §2.1 — AUTO_STOP_SECONDS==120；timed_out_sessions
        边界（>=120s 计入 / <120s 不计 / 非累积态与无活动会话排除）。"""
        const, err = _story_attr("AUTO_STOP_SECONDS")
        self.assertEqual(const, 120,
                         f"M4K1-A5 RED: AUTO_STOP_SECONDS = {const!r} != 120（录音态超时口径，"
                         "requirements §2.1）。")
        st = _story_module()
        acc = st.StoryAccumulator(str(Path(tempfile.gettempdir()) / "m4k1_a5_acc"))
        self.assertTrue(hasattr(acc, "timed_out_sessions"),
                        "M4K1-A5 RED: StoryAccumulator 无 timed_out_sessions(now) —— 120s "
                        "自动收尾的纯决策面未实现。")
        t0 = datetime(2026, 9, 24, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s-old", now=t0)
        acc.feed("s-old", "内容", now=t0)
        acc.start("s-fresh", now=t0 + timedelta(seconds=125))
        acc.feed("s-fresh", "内容", now=t0 + timedelta(seconds=125))
        acc.feed("s-chat", "闲聊", now=t0)  # 无 start -> chat 态
        at_119 = acc.timed_out_sessions(t0 + timedelta(seconds=119))
        at_120 = acc.timed_out_sessions(t0 + timedelta(seconds=120))
        at_130 = acc.timed_out_sessions(t0 + timedelta(seconds=130))
        self.assertNotIn("s-old", at_119, "M4K1-A5 RED: 119s 误判超时（阈值必须 >=120s）。")
        self.assertIn("s-old", at_120, "M4K1-A5 RED: 恰 120s 未计入（边界 >= 语义）。")
        self.assertIn("s-old", at_130, "M4K1-A5 RED: 130s 未计超时。")
        self.assertNotIn("s-fresh", at_130, "M4K1-A5 RED: 活动仅 5s 前的会话误判超时。")
        self.assertNotIn("s-chat", at_130, "M4K1-A5 RED: 非累积态会话误判超时。")

    def test_a5_front_wiring_references_timeout_decision(self):
        """coverage: M4K1-A5（源锚代理）— front 轮询接线引用 120s 纯决策
        （timed_out_sessions）；行为等价假设「每 tick 调该决策并对命中会话 stop+下发」
        本齿未验（offline 不可验，归 M4-K3 live）。"""
        self.assertIn(
            "timed_out_sessions", _front_source(),
            "M4K1-A5 源锚代理 RED: front_app.py 未引用 timed_out_sessions —— 120s 自动收尾的"
            "前端计时接线未落地（纯决策在而接线不在 = 超时永不触发）。")


# =========================================================================== #
# M4K1-A6 — 流水号账本齿                                                        #
# =========================================================================== #

class M4K1LedgerAcceptance(unittest.TestCase):
    """M4K1-A6: seq 持久账本（分配 max+1 / 删除不回退 / 重启存活 / 缺失重建不倒退）。"""

    maxDiff = None

    def test_a6_archive_assigns_persistent_monotonic_seq(self):
        """coverage: M4K1-A6 / 契约 §1.3④ — 归档分配 seq（首篇 1 递增）+ 账本落盘
        <stories_dir>/.seq.json + story_text 携带 seq。"""
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_a6_") as tmp:
            made = _seed(Path(tmp), 3)
            seqs = [r.get("seq") for r in made]
            self.assertEqual(seqs, [1, 2, 3],
                             f"M4K1-A6 RED: 归档 seq = {seqs!r} != [1,2,3] —— 入册流水号"
                             "账本（max+1）未实现。")
            self.assertTrue((Path(tmp) / ".seq.json").is_file(),
                            "M4K1-A6 RED: 账本文件 <stories_dir>/.seq.json 不在位 —— "
                            "持久账本未落盘（plan 取舍点 B 甲案）。")
            block = st.story_text(made[2])
            self.assertEqual(block["data"].get("seq"), 3,
                             "M4K1-A6 RED: story_text 未携带 data.seq（契约 §1.3④）。")
            ok, reason = _display_schema().validate_display(block)
            self.assertTrue(ok, f"M4K1-A6 RED: 带 seq 的 story_text 未过真门 ({reason})。")

    def test_a6_rearchive_same_id_keeps_seq(self):
        """coverage: M4K1-A6 — 同 id 幂等重写不涨计数、seq 不变（不产生第二颗）。"""
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_a6i_") as tmp:
            r1 = st.archive_story(str(tmp), "第一版", title="T", slug="same", date="2026-09-20")
            r2 = st.archive_story(str(tmp), "第二版", title="T", slug="same", date="2026-09-20")
            self.assertEqual(r1["id"], r2["id"], "M4K1-A6 RED: 同 id 语义偏移。")
            self.assertIsInstance(r1.get("seq"), int,
                                  "M4K1-A6 RED: 归档 record 未携带整数 seq —— 入册流水号账本"
                                  "（max+1）未实现。")
            self.assertEqual(r1.get("seq"), r2.get("seq"),
                             f"M4K1-A6 RED: 同 id 重写 seq 变化 {r1.get('seq')}->{r2.get('seq')} "
                             "—— 幂等重写不得分配新流水号。")
            self.assertEqual(st.list_stories(str(tmp))["data"]["total"], 1,
                             "M4K1-A6 RED: 同 id 重写涨了计数。")

    def test_a6_restart_survival(self):
        """coverage: M4K1-A6 — 进程/实例重启后账本存活（新实例续分配，不靠扫描回推）。"""
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_a6r_") as tmp:
            _seed(Path(tmp), 3)  # seq 1..3
            # 修订（tighten-only §6，2026-09-24）：纳入删除步，使本齿真正考到 docstring 自陈
            # 意图「不靠扫描回推（删除后会回退）」——seed 3 篇后删中间 1 篇 -> 现存 2 篇；
            # 计数派生 = 3，账本续分配 = 4（原变异「去持久账本、按现存 MD 计数分配、不回写
            # .seq.json」下本齿可判别：seed-only 无删除时两派生同为 4、不可判别）。
            _del, _del_err = _story_attr("delete_story")
            if not callable(_del):
                self.fail("M4K1-A6 RED: " + (_del_err
                          or "delete_story 非可调用（restart fixture 删除步前置）。"))
            _before_restart = sorted(Path(tmp).glob("*.md"))
            _del(str(tmp), _before_restart[1].stem)   # 删中间 1 篇（现存 2 篇）
            acc2 = st.StoryAccumulator(str(tmp))  # 模拟重启：新实例、同目录
            acc2.start("s-restart")
            acc2.feed("s-restart", "重启后新篇")  # 非空累积（空累积 stop 依既有语义返 None）
            rec = acc2.stop("s-restart")
            self.assertIsNotNone(rec, "M4K1-A6 RED: 重启后 stop 未产归档。")
            self.assertEqual(
                rec.get("seq"), 4,
                f"M4K1-A6 RED: 重启后 seq = {rec.get('seq')!r} != 4 —— 账本未存活、靠扫描"
                "回推（删除后会回退）。")

    def test_a6_delete_does_not_rollback_seq(self):
        """coverage: M4K1-A6 / S-F2.4 AC2 — 存 3 删 1 -> total=2 且下一篇 seq=4（删除不
        回退流水号；计数与流水号两数独立）。"""
        st = _story_module()
        delete, err = _story_attr("delete_story")
        if not callable(delete):
            self.fail("M4K1-A6 RED: " + (err or "delete_story 非可调用。"))
        with tempfile.TemporaryDirectory(prefix="m4k1_a6d_") as tmp:
            a, b, c = st.archive_story(str(tmp), "A", title="A", slug="a", date="2026-09-20"), \
                st.archive_story(str(tmp), "B", title="B", slug="b", date="2026-09-21"), \
                st.archive_story(str(tmp), "C", title="C", slug="c", date="2026-09-22")
            self.assertEqual([a["seq"], b["seq"], c["seq"]], [1, 2, 3],
                             "M4K1-A6 RED: 前置 seq 偏移。")
            self.assertTrue(delete(str(tmp), b["id"]), "M4K1-A6 RED: 删除 B 失败。")
            self.assertEqual(st.list_stories(str(tmp))["data"]["total"], 2,
                             "M4K1-A6 RED: 删 1 后 total != 2（现存篇数口径）。")
            d = st.archive_story(str(tmp), "D", title="D", slug="d", date="2026-09-23")
            self.assertEqual(
                d.get("seq"), 4,
                f"M4K1-A6 RED: 删 1 后下一篇 seq = {d.get('seq')!r} != 4 —— 删除回退了流水号"
                "（只增不减被破，S-F2.4 AC2 v1.47）。")

    def test_a6_ledger_missing_rebuilds_without_rollback(self):
        """coverage: M4K1-A6 — 账本缺失/损坏 -> 重建且不倒退（新 id seq > 重建前最大 seq）
        + 告警留痕（不把 seq 回退当可接受降级）。"""
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_a6m_") as tmp:
            made = _seed(Path(tmp), 3)  # seq 1..3
            self.assertEqual(made[-1].get("seq"), 3, "M4K1-A6 RED: 前置 seq 偏移。")
            (Path(tmp) / ".seq.json").unlink()  # 账本损坏/缺失
            with _captured_logs() as recs:
                nxt = st.archive_story(str(tmp), "新篇", title="N", slug="new",
                                       date="2026-09-25")
            self.assertGreater(
                nxt.get("seq") or 0, 3,
                f"M4K1-A6 RED: 账本缺失重建后 seq = {nxt.get('seq')!r} <= 3 —— seq 回退/复用"
                "（不得把回退当可接受降级）。")
            self.assertTrue(
                any("seq" in r.getMessage().lower() or "账本" in r.getMessage()
                    or "ledger" in r.getMessage().lower() for r in recs),
                "M4K1-A6 RED: 账本缺失重建未告警留痕（静默丢账本 = 证据丢失）。")


# =========================================================================== #
# M4K1-E1 — 既有 story 冻结语义零偏移守卫（must-stay-green）                     #
# =========================================================================== #

class M4K1ZeroOffsetGuard(unittest.TestCase):
    """coverage: M4K1-E1 — archive_story / read_story_file / list_stories / story_text /
    StoryAccumulator（start/feed/stop/cancel/redo/recover V31 四回退）冻结语义回归守卫
    （builder 增量扩展不得偏移既有行为；先例 = 已归档 story_p3 冻结件）。"""

    maxDiff = None

    def test_e1_existing_story_semantics_zero_offset(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="m4k1_e1_") as tmp:
            # archive_story：空转写大声拒绝；id 形态；真落盘；哨兵头。
            with self.assertRaises(ValueError,
                                   msg="M4K1-E1 RED: 空转写未 ValueError（空故事不得入册）。"):
                st.archive_story(str(tmp), "   ")
            rec = st.archive_story(str(tmp), "从前有一只小猫。后来它回家了。",
                                   title="小猫回家", slug="xiaomao", date="2026-09-20")
            self.assertEqual(rec["id"], "2026-09-20-xiaomao",
                             "M4K1-E1 RED: id 形态偏移（<date>-<slug>）。")
            self.assertEqual(rec["date"], "2026-09-20", "M4K1-E1 RED: date 偏移。")
            self.assertEqual(rec["title"], "小猫回家", "M4K1-E1 RED: title 偏移。")
            path = Path(rec["path"])
            self.assertTrue(path.is_file(), "M4K1-E1 RED: 归档未真落盘。")
            content = path.read_text(encoding="utf-8")
            self.assertIn("<!-- kidpal-story:1 -->", content,
                          "M4K1-E1 RED: 哨兵头偏移（list 区分真故事的判据）。")
            # read_story_file：非记录文件 -> None。
            junk = Path(tmp) / "README.txt"
            junk.write_text("杂物\n", encoding="utf-8")
            self.assertIsNone(st.read_story_file(str(junk)),
                              "M4K1-E1 RED: 杂物文件未被 read_story_file 判 None。")
            self.assertEqual(st.read_story_file(str(path))["text"], "从前有一只小猫。后来它回家了。",
                             "M4K1-E1 RED: read_story_file 正文偏移。")
            # story_text：无 seq 的 record -> data keys 恰 {id,title,text}（seq additive）。
            block = st.story_text({"id": "x", "title": "t", "text": "body"})
            self.assertEqual(
                set(block["data"]), {"id", "title", "text"},
                f"M4K1-E1 RED: 无 seq record 的 story_text keys {sorted(block['data'])} != "
                "{id,title,text} —— seq 必须仅当 record 携带时才出现（additive 红线）。")
            ok, reason = _display_schema().validate_display(block)
            self.assertTrue(ok, f"M4K1-E1 RED: 既有 story_text 形状未过真门 ({reason})。")
            # list_stories：杂物不计、日期倒序、MM-DD、total 全集计数、offset 超界。
            st.archive_story(str(tmp), "第二篇", title="第二", slug="two", date="2026-09-22")
            lst = st.list_stories(str(tmp))
            self.assertEqual(lst["data"]["total"], 2,
                             "M4K1-E1 RED: list total 全集计数偏移（杂物文件不得计入）。")
            self.assertEqual([it["date"] for it in lst["data"]["items"]], ["09-22", "09-20"],
                             "M4K1-E1 RED: items 日期倒序/MM-DD 显示偏移。")
            out_of_range = st.list_stories(str(tmp), offset=99)
            self.assertEqual(out_of_range["data"]["items"], [],
                             "M4K1-E1 RED: offset 超界应 items []。")
            self.assertEqual(out_of_range["data"]["total"], 2,
                             "M4K1-E1 RED: offset 超界动了 total（total 与分页无关）。")
            # StoryAccumulator V31 四回退 + 有界（先例 story_p3）。
            acc = st.StoryAccumulator(str(tmp))
            acc.start("s1")
            self.assertEqual(acc.routing("s1"), "accumulate", "M4K1-E1 RED: start 路由偏移。")
            acc.feed("s1", "第一句")
            acc.feed("s1", "第二句")
            r = acc.stop("s1")
            self.assertIsNotNone(r, "M4K1-E1 RED: stop 空/满语义偏移。")
            self.assertEqual(acc.routing("s1"), "chat", "M4K1-E1 RED: stop 后未回闲聊态。")
            # cancel（discard）：丢弃不归档。
            acc.start("s2")
            acc.feed("s2", "丢弃内容")
            self.assertTrue(acc.cancel("s2"), "M4K1-E1 RED: cancel 未上报已丢弃。")
            self.assertFalse(acc.cancel("s2"), "M4K1-E1 RED: cancel 非幂等。")
            # recover：环空不编造归档。
            self.assertIsNone(acc.recover("nonexistent"),
                              "M4K1-E1 RED: recover 会话缺失应 None。")
            # redo：窗口内撤销上一归档（删文件 + 回累积态）。
            acc.start("s3")
            acc.feed("s3", "待撤销")
            t_stop = datetime(2026, 9, 24, tzinfo=dt_timezone.utc)
            rec3 = acc.stop("s3", now=t_stop)
            undone = acc.redo("s3", now=t_stop + timedelta(seconds=5))
            self.assertIsNotNone(undone, "M4K1-E1 RED: redo 窗口内未撤销。")
            self.assertFalse(Path(rec3["path"]).exists(),
                             "M4K1-E1 RED: redo 未真删刚写文件。")
            self.assertEqual(acc.routing("s3"), "accumulate",
                             "M4K1-E1 RED: redo 未回累积态。")
            # 有界：char_window 触顶 FIFO 不无界增长。
            acc_bounded = st.StoryAccumulator(str(tmp), sentence_window=3, char_window=1000)
            acc_bounded.start("b")
            for i in range(50):
                acc_bounded.feed("b", f"句{i:03d}")
            self.assertLessEqual(acc_bounded.recent_depth("b"), 3,
                                 "M4K1-E1 RED: 近句环超 sentence_window —— 缓冲无界（内存红线）。")


# =========================================================================== #
# SAO-A1..A4 — 归档时序缺陷冻结 oracle（261001-m4-k3-story-archive-order）        #
# =========================================================================== #

class SAOArchiveOrderAcceptance(unittest.TestCase):
    """SAO-A1..A4: 记录会话归档时序缺陷（261001 真机 r2 实录）冻结 oracle。

    真机序 = 设备 stop_rec -> 上行 listen/state=stop 先到（此时 accum 必空）-> 转写 stt
    1.7s 后经下行才到。旧实现于该 stop 帧上立即 acc.stop()：空累积 -> 返回 None 且把
    会话打回 chat -> 随后 stt 被 _story_feed_from_stt 丢弃 -> 内容彻底丢失。逃逸本源 =
    K1 冻结件把喂数序固化为 stt -> listen-stop（与真机相反；本类 SAO-A1 承接改序）。

    修法口径（plan §2）：stop 时 accum **非空** -> 立即归档（B2/B3 auto 模式兼容分支，
    本类不将其判错——既有 E1/a6 齿沿该分支覆盖）；accum **空** -> 挂起 pending-stop，
    下一条 stt 到达时 feed -> 归档 -> 下发 story_text + story_list；pending 超时 10s ->
    释放挂起态、不写空故事、迟到 stt 不污染；正文剔尾随收尾词（SAO-A4）。

    sourceRefs = S-F2.2-AC2（录音中收尾 -> 保存、新篇入列表、AI 不逐句回复）、
    S-F2.2-AC4（KEY 兜底收尾 = 停录并保存）。"""

    maxDiff = None

    _CTX = json.dumps({"type": "kidpal_session_ctx", "source_page": "story"},
                      ensure_ascii=False)
    _STOP = json.dumps({"type": "listen", "state": "stop"}, ensure_ascii=False)

    def test_sao_a1_stop_before_stt_archives_transcript(self):
        """coverage: SAO-A1 / S-F2.2-AC2 / S-F2.2-AC4 — 真机序（listen/state=stop 先、
        转写 stt 后）-> 归档恰一篇、正文 = 转写内容、下发 story_text + story_list。"""
        with tempfile.TemporaryDirectory(prefix="sao_a1_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-sao-a1"
                _device_to_engine(front, sid, [self._CTX])
                # 1) 停录上行帧先到（累积必空）。
                _device_to_engine(front, sid, [self._STOP])
                # 2) 转写 stt 后到（真机序）。
                transcript = "从前有一只小猫，它去公园玩了"
                _up, dev = _engine_to_device(front, sid, [{"type": "stt", "text": transcript}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(
                    len(files), 1,
                    f"SAO-A1 RED: 真机序（listen-stop 先、stt 后）未产出恰一篇归档"
                    f"（得 {len(files)}）—— 归档早于转写（旧缺陷：stop 帧上立即 acc.stop()）。")
                st = _story_module()
                rec = st.read_story_file(str(files[0]))
                self.assertEqual(
                    (rec or {}).get("text"), transcript,
                    f"SAO-A1 RED: 归档正文 {((rec or {}).get('text'))!r} != 转写内容 "
                    f"{transcript!r} —— 正文须为转写结果。")
                self.assertTrue(
                    _story_text_frames(dev.sent_text),
                    "SAO-A1 RED: 归档后未下发 story_text（设备侧无法盖章/回看）。")
                self.assertTrue(
                    _story_list_frames(dev.sent_text),
                    "SAO-A1 RED: 归档后未追加 story_list 推送（三时点②；新篇应出现在列表）。")

    def test_sao_a2_empty_stop_holds_pending_not_bail_to_chat(self):
        """coverage: SAO-A2 / S-F2.2-AC2 — 本缺陷冻结 oracle（防复发）：listen/state=stop
        先到且 accum 空时，会话**不得**立即被打回 chat（旧实现 stop() 空累积 -> None +
        routing 回 chat）；须挂起 pending-stop 态，后续 stt 到达才归档。

        对旧实现变红 = routing(sid) == "chat"（缺陷复现）。"""
        with tempfile.TemporaryDirectory(prefix="sao_a2_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-sao-a2"
                acc = front._get_story_acc()
                _device_to_engine(front, sid, [self._CTX])
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SAO-A2 RED(infra): ctx(story) 未切累积态（前置齿红）。")
                # listen-stop 先到（accum 空）——不得立即回 chat（pending-stop 态须存在）。
                _device_to_engine(front, sid, [self._STOP])
                self.assertNotEqual(
                    acc.routing(sid), "chat",
                    "SAO-A2 RED: listen-stop 空累积时会话被打回 chat —— 旧缺陷复现"
                    "（stop() 立即返 None 并把会话回闲聊态）；须挂起 pending-stop 等转写到达"
                    "（本缺陷冻结 oracle）。")
                # 后续 stt 到达 -> 挂起态解除 + 归档恰一篇。
                _up, dev = _engine_to_device(front, sid, [{"type": "stt", "text": "转写内容"}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(
                    len(files), 1,
                    f"SAO-A2 RED: pending-stop 后转写到达未归档（得 {len(files)} 篇）—— "
                    "挂起态未接通恢复归档。")
                self.assertTrue(
                    _story_text_frames(dev.sent_text),
                    "SAO-A2 RED: pending 恢复归档后未下发 story_text。")

    def test_sao_a3_repeated_stop_no_double_archive(self):
        """coverage: SAO-A3 / S-F2.2-AC4 — 重复 listen/state=stop 不双归档：真机序下
        stop -> stop -> stt 恰产一篇（pending 去重，不留第二挂起）。"""
        with tempfile.TemporaryDirectory(prefix="sao_a3r_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-sao-a3-dup"
                _device_to_engine(front, sid, [self._CTX])
                _device_to_engine(front, sid, [self._STOP, self._STOP])
                _up, dev = _engine_to_device(front, sid, [{"type": "stt", "text": "一段故事"}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(
                    len(files), 1,
                    f"SAO-A3 RED: 重复 stop 产出 {len(files)} 篇归档（应恰一篇）—— 双归档"
                    "（重复 stop 未去重）或真机序归档链未接通。")

    def test_sao_a3_stop_cancel_interleave_no_leak(self):
        """coverage: SAO-A3 / S-F2.2-AC4 — pending 中 cancel（V31 丢弃）不归档、不泄漏
        会话态；释放后新会话真机序仍可归档（不串味）。"""
        with tempfile.TemporaryDirectory(prefix="sao_a3c_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                sid_a = "sess-sao-a3-cancel"
                _device_to_engine(front, sid_a, [self._CTX])
                _device_to_engine(front, sid_a, [self._STOP])   # pending-stop
                acc.cancel(sid_a, reason="test_discard")         # V31 丢弃
                self.assertNotIn(
                    sid_a, acc.active_sessions(),
                    "SAO-A3 RED: cancel 后 pending 会话态未释放（泄漏挂起态）。")
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 0,
                    "SAO-A3 RED: pending 中 cancel 仍产出归档 —— 丢弃即丢弃（V31）。")
                # 迟到 stt 不复活已丢弃会话。
                _up_a, dev_a = _engine_to_device(front, sid_a, [{"type": "stt", "text": "迟到转写"}])
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 0,
                    "SAO-A3 RED: cancel 后迟到 stt 复活了已丢弃内容（串味）。")
                # 新会话真机序仍可归档（正齿，证本件非恒绿）。
                sid_b = "sess-sao-a3-cancel-pos"
                _device_to_engine(front, sid_b, [self._CTX])
                _device_to_engine(front, sid_b, [self._STOP])
                _up_b, dev_b = _engine_to_device(front, sid_b, [{"type": "stt", "text": "新故事"}])
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 1,
                    "SAO-A3 RED: 释放后新会话真机序未归档 —— 归档链被前一会话串味/状态未清。")

    def test_sao_a3_pending_window_boundary(self):
        """coverage: SAO-A3 / S-F2.2-AC2 — pending 窗口两侧：① 窗口内（转写及时到）仍归档
        （超时不得过早触发）；② 无转写 -> 驱动 front 超时轮询越过窗口 -> 释放挂起态回闲聊、
        不写空故事；③ 超时后迟到 stt 不污染、无假归档演出。"""
        with tempfile.TemporaryDirectory(prefix="sao_a3t_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                # ① 窗口内：stop -> stt（真机序）-> 恰一篇（防超时过早释放）。
                sid_in = "sess-sao-a3-win"
                _device_to_engine(front, sid_in, [self._CTX])
                _device_to_engine(front, sid_in, [self._STOP])
                _up, dev = _engine_to_device(
                    front, sid_in, [{"type": "stt", "text": "窗口内转写"}])
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 1,
                    "SAO-A3 RED: 窗口内转写未归档 —— pending 被过早释放（或归档链未接通）。")
                # ② 窗口外：stop 后无转写 -> 驱动超时轮询越过窗口 -> 释放、零归档。
                sid_out = "sess-sao-a3-out"
                _device_to_engine(front, sid_out, [self._CTX])
                t0 = datetime.now(dt_timezone.utc)
                _device_to_engine(front, sid_out, [self._STOP])
                # +30s：越过 10s pending 窗口、且未到 120s 录音态静默收尾（隔离 pending 超时半，
                # 不撞 120s sweep 语义）。
                asyncio.run(front._story_timeout_sweep(now=t0 + timedelta(seconds=30)))
                self.assertEqual(
                    acc.routing(sid_out), "chat",
                    "SAO-A3 RED: pending 超时后会话未释放回闲聊态（泄漏挂起态）。")
                # ③ 超时后迟到 stt 不污染（仍为窗口内那 1 篇，无新增、无假演出）。
                _up2, dev2 = _engine_to_device(front, sid_out, [{"type": "stt", "text": "迟到的转写"}])
                self.assertEqual(
                    len(list(Path(tmp).glob("*.md"))), 1,
                    "SAO-A3 RED: 超时后迟到 stt 产出归档 —— 挂起态未真正释放（污染）。")
                self.assertEqual(
                    len(_story_text_frames(dev2.sent_text)), 0,
                    "SAO-A3 RED: 超时后迟到 stt 触发 story_text 下行（假归档演出）。")

    def test_sao_a4_body_excludes_trailing_finish_words(self):
        """coverage: SAO-A4 / S-F2.2-AC2 — 归档正文剔除尾随收尾词：「我说完了」/「讲完啦」
        + 子串「说完了」（真机转写尾带「说完了」）。"""
        narration = "从前有一只小猫去公园玩"
        for tail in ("我说完了", "讲完啦", "说完了"):
            with self.subTest(tail=tail):
                with tempfile.TemporaryDirectory(prefix="sao_a4_") as tmp:
                    with _front_with_stories(tmp) as front:
                        sid = f"sess-sao-a4-{tail}"
                        _device_to_engine(front, sid, [self._CTX])
                        _device_to_engine(front, sid, [self._STOP])
                        _up, dev = _engine_to_device(
                            front, sid, [{"type": "stt", "text": f"{narration}。{tail}"}])
                        files = list(Path(tmp).glob("*.md"))
                        self.assertEqual(
                            len(files), 1,
                            f"SAO-A4 RED({tail}): 真机序未产出归档（得 {len(files)} 篇）。")
                        st = _story_module()
                        body = (st.read_story_file(str(files[0])) or {}).get("text", "")
                        self.assertIn(
                            narration, body,
                            f"SAO-A4 RED({tail}): 归档正文丢失叙述内容 {narration!r}。")
                        self.assertNotIn(
                            tail, body,
                            f"SAO-A4 RED({tail}): 归档正文含尾随收尾词 —— 正文剔词未落"
                            "（plan §2 正文剔尾随收尾词）。")


# --------------------------------------------------------------------------- #
# REVISION LOG (冻结件修订记录；append-only —— 本块之前/之后均不得删除既有行)
# --------------------------------------------------------------------------- #
# [2026-09-24 · tighten-only（DEV-GOVERNANCE §6）· 同 Pod / 同件 / 不新建文件 · 范围帽内]
#   Reason: `test_a6_restart_survival` fixture 缺陷（builder 实现期实证 + planner 第二方复核）
#   ——原 fixture `start("s-restart")` 后缺 `feed()`，空累积下 `stop` 依既有冻结语义返 None
#   （先例 = 已归档 story_p3 冻结件逐字断言 empty-stop -> None；本件 E1 齿断言空转写
#   ValueError）→ `assertIsNotNone` 必红，与账本实现无关（两向皆红 = 齿不可满足）。
#   修订 = 纯 additive 补 fixture：① 纳入删除步（seed 3 篇后删中间 1 篇 -> 现存 2 篇），使本齿
#   真正考到 docstring 自陈意图「不靠扫描回推（删除后会回退）」；② 补一行 `feed` 使累积非空。
#   **断言语义零改动**（`assertIsNotNone` + `seq == 4` 逐字不动）= 钉 M4K1-A6 原意图（账本存活 /
#   续分配）；无新 acceptance id、未改行为定义 ⇒ 非 re-spec。
#   原变异重放（= taskpackage mutationEvidence 的 A6 条 mutation：去持久账本、按现存 MD 计数
#   分配、不回写 .seq.json）= 运行期扰动 `story._assign_seq` -> 本齿转红（`3 != 4`，原始输出见
#   server/acceptance/M4K1-ACCEPTANCE-NOTES.md §9）；还原复绿。修订前 fixture 若仅补 `feed` 而
#   无删除步，该变异下账本派生与计数派生同为 4、不可判别 —— 故删除步为达标本件之必要形态。
#   机判纯 additive: `git diff -U0`（HEAD v1 -> 本修订）**删除行数 = 0**（全 hunk 皆 `+`）。
#   sha256 核销: 61c0501742eda85c50a6c67b0ea399a12c8b273da56daee778e89eb3eb49cc26 → 1be76af9f3f1076987f44caeb8334688019fae8a191eead57bf62de04a57c2a8
#   （「后」= 修订后**正文态** sha256（不含本 REVISION LOG 块；去块后复算即得，逐字可验）；
#    含本块之交付形态最终 sha256 另记于 notes §9.7 —— 块内置自身之 sha 自指不可解，据实分记。）
# 本块刻意置于 `if __name__ == "__main__":` **之前**（非文件尾）——使 v1 末行保持逐字不变，
# 纯 additive 机判无 EOF 换行正规化假象（删除行数真为 0）。
# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #
# [2026-09-28 · 正常改验收通道（非 §6 tighten-only）· 包 260928-story-connect-push-defer
#  · human 方向锚 pod-events b036f554 · 范围：本冻结件内 a3 齿时机语义修订 + FIX-A1 负锚新增]
#   修订理由（真机缺陷 + 设备拓扑）：M4-K1 三时点①「WS 建连即推 story_list」与设备拓扑冲突
#   —— 设备**每会话重开 WS**（会话结束关 audio channel、按 KEY 重开），front 建连即推会在
#   每次会话建立时无条件下发 story_list，设备渲染 P3 页盖掉聆听态；且旧固件 story_page.c
#   零 CJK → 半截页。根因三方核实记录 = 2026-09-28 会话 + 包 plan.md。
#   human 裁定止血：**摘除建连即推**；三时点①时机归 M4-K3 session_ctx 承接；②③保留。
#   修订动作：FIX-A2 = `test_a3_push_seam_connect_and_wire_timepoints` 更名
#   `test_a3_push_seam_and_wire_timepoints`，移除建连时点断言（含「ws_gateway 引用」源锚
#   代理——判据随语义作废），保留 _push_story_list 命名缝行为断言 + ②③ wire 断言；
#   FIX-A1 = 新增负锚齿 `test_fix_a1_connect_path_emits_zero_story_list`（**行为级**：
#   ws_gateway 真实连接处理段流程离线走完——鉴权 + accept + 假引擎腿（websockets 假模块
#   注入 `_fake_engine_ws`）+ 两条 pump，断言设备腿 story_list 帧数 = 0 + 连接活性防空过；
#   时点②③在场由修订后 a3 齿承载，不双断言同一面）；配套 `_ensure_fastapi_stub` 升级
#   （恒等路由装饰器与真 fastapi 同形 + 覆盖他件无 FastAPI 属性的 permissive stub）。
#   通道声明：本修订为**正常改验收通道**的语义修订（human 方向授权的行为策略变更），
#   **非 §6 tighten-only** —— 建连时点断言的移除属语义修订本体（既有行删改在案）。
#   红基线恒现跑（2026-09-28 实测）：FIX-A1 设计态红（建连推仍在树）+ 其余 26 齿全绿零
#   新增红；尾行与红名见 notes §10。mutation：恢复建连即推（连接处理段同位注入）→ 负锚
#   齿转红；摘除态（模拟修订后）→ 绿（可满足性），原始输出见 notes §10。
#   sha256 核销链: 5d4ebbf28f13cf325483738430f62a60105f54176eba1d85fc15a355c625a287（v2 交付
#   形态）→ 2e33ee943872f5781ab40eb54a58c790004291134a1dffdf6f1ec799516f71dd（本次修订后
#   正文态，剥离全部 REVISION LOG 注释块后复算可验）→ 交付形态（含本条）见 notes §10
#   —— 本条自身计入正文、自指不可解，据实分记（同上条口径）。
# --------------------------------------------------------------------------- #
# [2026-10-01 · 正常改验收通道（非 §6 tighten-only）· 包 261001-m4-k3-story-archive-order
#  · sourceType=regression · riskTier=high · 范围：本冻结件内归档链齿时序改真机序 +
#  SAO-A1..A4 新增]
#   修订理由（261001 真机 r2 实录 + spine q8 复核订正口径）：时序缺陷，非竞态。旧实现于
#   上行 listen/state=stop 帧上立即 acc.stop()，但 manual 模式 ASR 由 listen stop 才触发、
#   转写 stt 1.7s 后才经下行到 -> 归档瞬间 accum 必空 -> stop() 返 None 并把会话打回 chat
#   -> 随后 stt 被 _story_feed_from_stt 丢弃 -> 内容彻底丢失。逃逸本源 = 本件全部归档链齿
#   喂数序固化为 stt -> listen-stop（与真机相反）。根因三方记录 = spine q8 + 本包 plan §1。
#   修订动作：① 既有归档链齿改真机序（listen-stop 先、stt 后）——test_a2_session_ctx_
#   intercepted_not_forwarded / test_a2_idempotent_convergence_with_start_recording /
#   test_a3_push_seam_and_wire_timepoints（时点②）/ test_a5_finish_phrase_triggers_stop_in_
#   accumulate_only（a5 真机序形态 = 转写整段含尾随「我说完了」；正文剔词断言并入）；
#   ② 新增 SAOArchiveOrderAcceptance（SAO-A1 正齿 / SAO-A2 负锚 / SAO-A3 边界三齿 /
#   SAO-A4 正文齿）。旧序 stt -> listen-stop **不得再过闸**（改后旧实现上转红 = 设计态红）。
#   通道声明：本修订为**正常改验收通道**的语义修订（真机序 = 已被 261001 r2 证据定死的
#   行为），**非 §6 tighten-only**（既有归档链齿喂数序属语义本体改写）。改写已 merged 包
#   （260924-m4-k1）冻结 oracle 的签字成本经 closeout + supervisor riskTier=high 承载
#   （plan §5）。兼容形态（stop 时 accum 非空 -> 立即归档，B2/B3 auto 模式不返工）= 本件
#   不判错（既有 E1/a6 齿沿该分支覆盖）。
#   红基线恒现跑（2026-10-01 实跑，见交付报告 / taskpackage redBaselineResult）：
#   SAO 新齿 + 改序齿在未修实现上转红（旧缺陷 = 归档早于转写）；其余齿零新增红。
#   sha256 核销链: 2e33ee943872f5781ab40eb54a58c790004291134a1dffdf6f1ec799516f71dd（v3 交付
#   形态）→ 本次修订后正文态 sha256 见 notes（剥离全部 REVISION LOG 注释块后复算可验）→
#   交付形态（含本条）自身计入正文、自指不可解，据实分记（同上两条口径）。
# --------------------------------------------------------------------------- #
# [2026-10-12 · 正常改验收通道（非 §6 tighten-only）· 包 261010-p3-storylist-cold-start-restore
#  · 范围：本冻结件内 FIX-A1 负锚齿翻正 + SL-A1/SL-A6 齿 + a3/a4 齿 restore 缺省零偏移
#  断言 additive]
#   修订授权链（三件齐备, 逐字留痕——260928 FIX-A2 先例形态）:
#     1) 契约门 = CONTRACT-CHANGE-PACKET 261010-p3-storylist-cold-start（delivery/
#        taskpackages/261010-p3-storylist-cold-start-restore/contract-gate/
#        CONTRACT-CHANGE-PACKET.md）HUMAN_APPROVED 2026-10-12——story_list data.restore
#        additive（D1 字段定义 + 生产纪律「仅 front 后台附带推送路径（WS 建连恢复推送）
#        可置 restore」/ D2 板子行为句例外子句 / D3 §1.5-3 floor 联动 / D4 变更记录行）。
#     2) 需求面 = docs/requirements.md §9 v1.63 行（frozen 2026-10-12）——S-F2.1 AC1
#        「正常接收与冷启动恢复推送均算」+ 补充规则「冷启动恢复 = WS 建连附带重推（恢复性
#        推送、只填缓存不抢前台、P3 在屏时刷新本页）」+ AC5/AC6 空态两态定稿。
#     3) human 方向锚留痕：v1.63 赎回 260928-story-connect-push-defer 挂起的时点①重裁
#        （spine delivery/changes/intake/261010-p3-storylist-cold-start.md OQ-1 human 裁
#        restore 标记方案）——260928 摘除的止血前提（旧固件无条件渲染 P3 盖聆听态）已被
#        「data.restore 标记 + 板端 restore 分流（只填缓存不抢前台、P3 在屏仅重绘本页,
#        复刻 schedule mcp_server.cc :926-991 在役形态）」消解, 摘除理由不再成立。三方
#        核实记录 = spine + contract-gate/architecture-gate-evidence.md §4（HUMAN_APPROVED
#        问答原文留痕于主线程会话）。
#   修订动作:
#     ① FIX-A1 翻正 = test_fix_a1_connect_path_emits_zero_story_list 更名
#       test_fix_a1_connect_path_emits_exactly_one_restore_story_list, 「建连路径零
#       story_list 下行」负锚断言改为「恰一帧 + data.restore 字面 true + total 语义 +
#       V27 真门放行（D3 floor 联动）」; 类 docstring、coverage map、强度申报行、
#       弱于契约② 同步翻正注记（历史沿革原文保留）。连接活性防空过双断言（accepted +
#       listen 转发）逐字保留。
#     ② SL-A6 新增 = test_sla6_connect_push_failure_does_not_block_connect（行为级守卫:
#       monkeypatch _push_story_list 抛异常 -> 建连照常完成; 现状树无建连推送 = 平凡绿,
#       builder 落裸调（无 try/except）即红——变异敏感面显式申报, 非设计态红）。
#     ③ a3/a4 齿 additive: 时点②③推送帧 restore 缺省零偏移断言（assertFalse(data.get
#       ("restore"))——D1 生产纪律「仅建连恢复推送可置 restore」; absent/显式 false 同判,
#       既有断言零触碰）。
#   红基线恒现跑（2026-10-12 实跑, 逐字结果见 taskpackage redBaselineResult）: FIX-A1
#   翻正齿设计态红（建连现零帧）+ 其余齿全绿零新增红; 收集零错误（冻结即冒烟,
#   DEV-GOVERNANCE §3）。
#   mutation: 摘除建连段 _push_story_list(restore=True) 调用 -> 翻正齿转红（0 帧）; 置
#   restore=False -> restore 字面断言红; ②③调用点误置 restore -> a3/a4 零偏移断言红;
#   建连推送裸调（摘 try/except）-> SL-A6 齿红。
#   sha256 核销链: v3 正文态 -> 本次修订后正文态 sha256 见 notes（剥离全部 REVISION LOG
#   注释块后复算可验）——本条自身计入正文、自指不可解, 据实分记（同前三条口径）。
#   追记（同日, verifier 内循环 scoped r2）: 翻正齿 docstring coverage 标记
#   `coverage: FIX-A1 + SL-A1` 调序为 `coverage: SL-A1 + FIX-A1`——acceptance-coverage
#   闸（acceptance-coverage-check.mjs hasCoverageMarker 正则 `coverage\s*:?\s*SL-A1\b`）
#   要求 coverage 与验收 ID 紧邻, `FIX-A1 + ` 隔中不命中致 SL-A1 误报未覆盖; 纯
#   cosmetic coverage-marker 修正（闸红 ad 实证驱动）, 断言零触碰。
# --------------------------------------------------------------------------- #
if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    for cls in (M4K1ContractGateAcceptance, M4K1SessionCtxAcceptance,
                M4K1StoryListPushAcceptance, M4K1ConnectPushDeferAcceptance,
                M4K1DeleteAcceptance, M4K1FinishTimeoutAcceptance,
                M4K1LedgerAcceptance, M4K1ZeroOffsetGuard,
                SAOArchiveOrderAcceptance):
        suite.addTests(loader.loadTestsFromTestCase(cls))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)