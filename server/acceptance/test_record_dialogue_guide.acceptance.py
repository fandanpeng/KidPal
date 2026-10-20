"""Frozen acceptance tests — TaskPackage 261019-p3-record-dialogue-guide（server 面）。

承载 story = S-F2.2（对话式引导记录，v1.65 五 AC 的 server 承载半）+ NFR-PERF-3
（卡壳引导时机五条边界）+ S-F1.4-AC6（录音期提醒排队延后不回归——host 可测半）。
上游冻结 = delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md
（FROZEN v1.2，T1–T7 + ADR-013）+ reviewed plan
delivery/taskpackages/261019-p3-record-dialogue-guide/plan.md（S1–S5 分段 + 验收
种子 1–8 + decisionCoverage 11 项）。

Scope of record（taskpackage acceptance RDG-A1..A7）:
  RDG-A1 明示卡壳词表纯函数（is_stuck_phrase 同构子串扫描；词表 = 实现期可调常量）。
  RDG-A2 StoryAccumulator 卡壳计时与引导轮次纯决策（时钟注入快进）+ NFR-PERF-3
        五条边界（阈值常量不落验收值；轮数 2 已拍）。
  RDG-A3 忠实整理输入装配纯函数（只取孩子的话）+ 两枚 front 侧版本化 prompt 制品
        （忠实约束 + 引导护栏 + 该回合不触发工具调用）。
  RDG-A4 Package B 等价信号注入引擎腿（进/出语境）+ engine-patch 认领/chat 第三守卫/
        stop-truncation 豁免重绑/引导注入不尾随（验收种子 1 host 半）。
  RDG-A5 display_schema 自推通路分叉（start_organizing 网关决定类 relay 拒转 +
        front 自推放行；end_session/start_recording 既有语义零偏移）。
  RDG-A6 卡壳计时 sweep（stuck-due 引导轮不归档；2 轮无回应自动收尾 → 三式收尾
        统一下发 start_organizing、重复幂等；pending-stop 10s 挂起窗口保留）。
  RDG-A7 S-F1.4-AC6 host 半：引导/整理 speak 注入 = kidpal_speak 内部指令同型、
        不产生提醒帧；设备侧录音排队例外（RM-A3）既有齿 must-stay-green 引用。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — S1 纯决策面可离线 import，
front_app.py 面离线装载（fastapi 缺席本地 stub，M4K1/SC 先例），engine-patch
面 AST 抽取真实方法体（connection.py 不可离线 import，ET 先例）。

  A) server/gateway/story.py（纯模块增量；120s 语义退役见 REVISION-NOTE）:

    STUCK_PHRASES（模块级 tuple 常量）
        明示卡壳词表。**内容 = 实现期可调**（S-F2.2-AC2「如『卡住了』『想不起来』」
        为示例起点，验收不冻结成员集）；约束 = 非空 tuple、元素全 str；
        is_stuck_phrase 以它为单源。

    def is_stuck_phrase(text) -> bool
        与 is_finish_phrase 同构（E3 检出器形态）：text（str）剔除首尾空白后**包含**
        STUCK_PHRASES 任一精确短语 -> True；空串/None/非 str -> False。子串匹配、
        末尾标点不阻断。

    STUCK_GUIDE_SECONDS（模块级数值常量）
        卡壳引导阈值缺省单源。**名字冻结、数值不冻结**（NFR-PERF-3 待定——真机调优
        后立项拍数，不得写死为验收值；验收只断言正数）。

    GUIDE_ROUND_LIMIT = 2（模块级常量，**数值冻结**）
        引导连续无回应自动收尾轮数（NFR-PERF-3「轮数已拍」，S-F2.2-AC5）。

    class StoryAccumulator（既有 start/feed/stop/cancel/redo/recover/routing/
    pending-stop 家族零偏移；增量 = 会话态新增引导轮次 guide_rounds(int, 初 0) 与
    guide_since(datetime|None, 初 None) 两字段）:

      mark_guide_round(session_id, now=None, *, suppressed=False) -> None
          计一轮引导：guide_rounds += 1（**suppressed=True 亦计**——q6 收声不停摆：
          收声抑制/LLM 返回空 = 本轮不播报仍计轮）；guide_since = now（None -> 墙钟，
          作为该轮无回应计时锚）；**不改** last_activity（孩子活动权威不被引导污染）。
      guide_rounds(session_id) -> int
          该会话已计引导轮次；无会话 -> 0。查询不改状态。
      feed(session_id, text, now=None) -> int（既有签名/语义零偏移 + additive）
          孩子开口 = 回应：清零该会话 guide_rounds 与 guide_since（连续无回应 streak
          重置），并刷新 last_activity（既有）。
      stuck_due_sessions(now, *, timeout_s=None) -> list[str]
          卡壳计时纯决策（NFR-PERF-3 边界①②）：累积态（routing=="accumulate"）会话
          中，等待锚 t = guide_since（该轮引导已发出则等回应）否则 last_activity；
          now - t >= timeout_s（缺省 STUCK_GUIDE_SECONDS）-> 计入（>= 边界，与既有
          pending/timed_out 决策同缝）。未记录活动 / 非累积态 -> 不返回；查询不改状态。
      auto_finish_due_sessions(now, *, timeout_s=None, rounds=None) -> list[str]
          自动收尾纯决策（NFR-PERF-3 边界④⑤）：累积态会话中 guide_rounds >= rounds
          （缺省 GUIDE_ROUND_LIMIT=2）**且** now - guide_since >= timeout_s（缺省
          STUCK_GUIDE_SECONDS）——第 2 轮计时到仍无回应即判收尾。查询不改状态。
      build_organizing_input(session_id) -> str | None
          忠实整理输入装配纯函数（只取孩子的话）：累积非空 -> "\n".join(accum)
          （**只读**——不消费、不改状态，消费归 stop()）；空/无会话 -> None。
          不滤任何内容（静音段幻听串、孩子中途提问句原样在输入内——滤除职责全压
          整理 LLM 指令，q7-E4/验收种子 2·3 口径）。

    REVISION-NOTE（120s 语义退役）：AUTO_STOP_SECONDS 常量与
    StoryAccumulator.timed_out_sessions() **退役（删除）**——语义换代为「卡壳引导
    2 轮无回应自动收尾」（requirements v1.65 S-F2.2-AC5 / NFR-PERF-3，§2.1 120s
    录音态超时口径作废）。改名/复活 = 语义回退，验收红。builder 须同步更新
    server/tests/test_story_units.py 的 120s 单测（changeManifest server/tests/**
    在册；既有断言 AUTO_STOP_SECONDS==120 与 timed_out_sessions 边界随语义退役
    重写为新决策族，suite 计数变化不记新增红）。M4K1 冻结件 A5 超时齿同步重钉
    （test_m4k1_story_cloud_link.acceptance.py REVISION LOG 2026-10-19 条）。

  B) server/gateway/display_schema.py（自推通路分叉，结构性风险 1）:

    GATEWAY_DECISION_CONTROL_ACTIONS 增 "start_organizing"（契约 §1.4：网关决定类、
        引擎 display 流拒转，与 end_session 同型）。
    def validate_display(payload, *, self_push=False) -> (ok, reason)
        **keyword-only 形参 self_push，缺省 False**（既有全部调用点零偏移）：
        self_push=False（relay/引擎侧路径）：control/start_organizing 一律拒绝
        （伪造/注入的网关决定类不得经 display 流透传）；其余既有行为一字不动
        （end_session 拒转、start_recording/stop_speaking 可透传、story floor 等）。
        self_push=True（front 自推专用通路）：网关决定类中 **start_organizing**
        放行（front 机械下发单源；end_session 即便自推仍拒绝——会话结束决定不因
        通路放行）；其余 floor 照跑。
    FORWARDABLE_CONTROL_ACTIONS 不含 start_organizing（决定类与可透传类不相交）。

  C) server/gateway/front/front_app.py（接线面；离线可装载纪律不变）:

    等价信号注入缝（ADR-013 Package B；服务端内部链路、形状不入契约——本冻结契约
    即 front 与 engine-patch 的协调单源）:
      信号 wire 字面（两侧同名冻结）: type == "kidpal_record_context"，布尔字段
      active（true=进入记录语境、false=退出）。经**引擎腿**下发（上行泵的 upstream
      参数 / sweep 的 _alert_legs 引擎腿），ADR-003 kidpal_speak 同型内部消息。
      进语境 = session_ctx(story) 与 control/start_recording 两收敛入口（幂等——
      已在记录语境重复触发不重复注入）；出语境 = 三式收尾（收尾词归档 / 自动收尾
      start_organizing 下发同拍）/ 取消（cancel）/ 断连。
      引导回合注入不尾随 auto-listen（种子 1 host 半）：引导 kidpal_speak 注入
      **不清**记录语境标志、不触发任何聆听形态/会话路由变化（真机半 = 引导播完
      继续录音，归 RDG-F6 live）。
    引导/整理 LLM 回合（S2）:
      sweep（_story_timeout_sweep 重写为卡壳计时 sweep，签名 (now=None) 不变）：
      **sweep 以注入 now 驱动全部纯决策与轮次记账（stuck_due_sessions /
      auto_finish_due_sessions / mark_guide_round 均传该 now——时钟注入纪律，
      不落墙钟；否则离线快进不可测）**：
      ① pending-stop 10s 挂起窗口**保留**（SAO 家族齿 must-stay-green）；
      ② stuck-due（stuck_due_sessions 命中）会话执行引导回合：引导制品 prompt →
      openclaw_stream_request（R1-F4 provider 出口，loopback 18789）→ 有文本 =
      kidpal_speak 注入播报；**返回空/异常 = 本轮不播报仍计轮**（mark_guide_round
      suppressed 等价）；引导轮**不归档、不下发 start_organizing**；
      ③ auto-finish（auto_finish_due_sessions 命中）→ 收尾：下发
      control/start_organizing 恰一帧（**重复幂等**——同一收尾沿重复 sweep 不重复
      下发）+ 注入 active=False 出语境信号 + 走整理回合；
      ④ 整理回合：一次性、忠实指令、标题 ≤12 字同回合产出 → archive_story（既有
      零改）→ story_text(seq) + story_list 三时点②；失败 = 当轮素材不归档 +
      固定安抚句 kidpal_speak（R1-N4，不发明新语义）。
      收尾词路径（_story_finish_on_phrase 既有）归档成功同拍下发
      start_organizing + 出语境信号（三式收尾统一）。
    两枚 front 侧版本化 prompt 制品（home = server/gateway/front/prompts/，目录
    名冻结；文件名与版本号包内定）: 引导制品 + 整理制品；**不走 OpenClaw 主 agent
    共享 system prompt**（request 级 system 消息承载）；ADR-001 三面相等接线
    不变式（制品文件 ↔ 加载代码 ↔ 回合指令同源）。

  D) server/deploy/engine-patch/connection.py（AST 抽取面，ET 先例）:

    实例标志 self._kidpal_record_context（bool；镜像 _kidpal_listen_closed 命名族）。
    认领缝 _kidpal_on_record_context(self, message)（方法名冻结）：认领 type==
    "kidpal_record_context" 帧——active 真 → 置标志、假 → 清标志；非本类型静默
    不动。_route_message 拦截顺序：_kidpal_try_speak 之后、handleTextMessage 之前
    （先认领后转，_kidpal_on_listen_message 同缝）。
    chat() depth==0 守卫块**第三守卫**：记录语境标志置位 → return（self.dialogue.put
    与 TTS FIRST 入队之前——跳过即零 LLM 零 TTS；守卫内不置 client_abort，避免波及
    front 注入的合法 speak/story 下发）。depth>0 递归子轮不拦。
    stop-truncation 豁免重绑（结构性风险 2，additive 不改既有判别机制）：截断豁免
    判据 = 记录语境标志 **或** 既有 client_listen_mode != "manual"——记录语境即使
    切非 manual 聆听形态，stop 触发的末句转写仍不截断（KEY 收尾承重通道）；非记录
    语境 auto 模式照常截断（261014 既有齿保绿）。
    引导注入不尾随：_kidpal_try_speak 体内不改 _kidpal_record_context、不置
    _kidpal_listen_closed（speak 注入零会话路由副作用）。

Shape authority = docs/contracts/interface-contract.md §1.4（start_organizing 表行
+ start_recording 措辞对账，2026-10-19 LANDED）+ docs/requirements.md v1.65
S-F2.2 / NFR-PERF-3 / S-F1.4-AC6。

跨端 wire 形状锚（对照对端实际接受什么；brief ② 必读面已核）:
  firmware/main/display/view_router.c kidpal_display_control_action —— action 为
    字符串精确比较，"start_organizing" -> VRT_CTRL_START_ORGANIZING(3)；未知 ->
    VRT_CTRL_UNKNOWN(-1) 降级（本包固件齿 RDG-F1 钉）。
  firmware/main/mcp_server.cc 控制 switch case 2 区 —— case 3 消费位 additive。
  firmware/main/display/story_page.c kidpal_story_on_control —— ctrl_action 整数
    数值码消费；case 3 -> 整理过渡态（RDG-F2 钉）。

契约条目 ↔ 齿 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:

| 契约 / 决策条目 | 承载断言 | 性质档 |
|---|---|---|
| S-F2.2-AC2 明示卡壳检出（E3 词表形态） | test_rdg_a1（is_stuck_phrase 真跑 + 词表单源联动） | 行为级 |
| NFR-PERF-3 五条边界（①-⑤） | test_rdg_a2（纯决策时钟注入真跑） | 行为级 |
| S-F2.2-AC4 忠实整理输入（只用孩子原话） | test_rdg_a3（装配真跑 + 制品内容锚） | 行为级 + 源锚代理（制品） |
| 验收种子 7 provider 出口（制品级 system prompt / 不触发工具调用） | test_rdg_a3（制品内容锚 + front 接线锚） | 源锚代理（live 行为归真机轮） |
| ADR-013 Package B（引擎侧抑制 LLM） | test_rdg_a4（front 注入真跑 + patch AST 行为/结构齿） | 行为级（front 半）+ 结构齿（patch 半） |
| 结构性风险 2（stop-truncation 豁免重绑） | test_rdg_a4（提取方法体真执行） | 行为级 |
| 契约 §1.4 start_organizing 网关决定类拒转 | test_rdg_a5（真门真跑 + relay 降级） | 行为级 |
| 结构性风险 1（自推通路分叉） | test_rdg_a5（self_push 真门 + front 自推真跑） | 行为级 |
| q2 = ⓒ 三式收尾统一 start_organizing 下发 | test_rdg_a6（收尾词路径 + sweep 路径真跑） | 行为级 |
| q6 收声不停摆计轮 | test_rdg_a2 边界⑤ + test_rdg_a6 引导轮不归档 | 行为级 |
| S-F2.2-AC5 自动收尾归档不丢弃 | test_rdg_a2 边界④ + test_rdg_a6（sweep 收尾链） | 行为级 |
| S-F1.4-AC6 录音期提醒排队延后不回归（host 半） | test_rdg_a7（注入通道形状锚 + RM-A3 must-stay-green 引用） | 源锚代理（端到端归真机轮） |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-1 引导/整理 LLM 回合的真实行为（引导文本质量、忠实整理产出、标题 ≤12 字、
        R1-N4 安抚句下发）= live-manual（真 OpenClaw loopback 18789 + 假 stt 序列
        驱动，plan S2 live 面），本文件不造 offline 假齿；offline 只钉机械触发/
        计轮/下发/幂等与制品内容锚。
  GAP-2 「引导注入不尾随 auto-listen」的真机半（引导播完继续录音、设备不离记录
        会话）= 真机轮（RDG-F6 structural/live-manual 申报）；host 半只钉引擎 patch
        标志不被注入清位（结构齿）。
  GAP-3 卡壳阈值数值：NFR-PERF-3 待定——本文件全部边界断言用注入 timeout_s 承载，
        绝不断言 STUCK_GUIDE_SECONDS 的取值（只断言正数存在）。
  GAP-4 「KEY 兜底收尾」与「BOOT/讲错啦取消丢弃」的 wire 半（设备上行帧形态）：
        KEY 收尾 = 设备本地 listen stop（既有真机序链路承载，SAO 齿保绿）+ 本包
        additive start_organizing/出语境信号（test_rdg_a6 收尾词路径承载收尾词一式；
        KEY/取消两式的 wire 观察归真机轮 S5）。
  GAP-5 整理回合的 system prompt 是否真的 request 级生效（不走共享 prompt）：
        offline 只钉制品文件在世与内容约束 + front 引用 openclaw_stream_request；
        运行时生效归 live（种子 7 真机半）。
  GAP-6 断连出语境（ws close → active=false 注入）：pump 层不可稳定观察（连接
        生命周期在 ws_gateway），不落 offline 齿；语义由出语境集合的设计约束承载。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 「pump_client_to_upstream 真路径 = 现场帧消费路径」（M4K1-A4/SC EQ-1 同款，
        已复证）——等价信号经该泵的引擎腿注入可观察。
  EQ-2 「_push_display_payload / _alert_legs 推送 = 设备实际收帧形状」（M4K1-A3
        同一重建器口径）。
  EQ-3 「AST 抽取执行 = 真实方法体控制流」（ET 先例）：connection.py 不可离线
        import，抽取失败（方法改名/结构漂移/未落地）以 infra 红报出，返
        acceptance-author 重钉，不由 builder/fixer 改齿。
  EQ-4 「LLM 不可达时快失败」：引导/整理回合的 provider 调用离线快失败（连接拒绝
        级），sweep 不挂起——本文件 sweep 齿依赖该假设（离线环境 18789 不在位）。

Coverage map (acceptanceRef -> test):
  RDG-A1 -> test_rdg_a1_stuck_phrase_substring_scan_and_wordlist_single_source
  RDG-A2 -> test_rdg_a2_nfr_boundary_1_speech_resets_stuck_timer
           -> test_rdg_a2_nfr_boundary_2_threshold_elapsed_triggers_guide
           -> test_rdg_a2_nfr_boundary_3_explicit_stuck_phrase_immediate
           -> test_rdg_a2_nfr_boundary_4_two_rounds_no_response_auto_finish
           -> test_rdg_a2_nfr_boundary_5_suppressed_rounds_still_count
  RDG-A3 -> test_rdg_a3_organizing_input_assembles_child_words_only
           -> test_rdg_a3_versioned_prompt_artifacts_carry_constraints
  RDG-A4 -> test_rdg_a4_front_injects_record_context_enter_exit_on_engine_leg
           -> test_rdg_a4_patch_claims_signal_and_chat_third_guard
           -> test_rdg_a4_patch_stop_truncation_exempt_record_context
           -> test_rdg_a4_guide_speak_injection_does_not_clear_context_flag
  RDG-A5 -> test_rdg_a5_start_organizing_gateway_decision_rejected_on_relay
           -> test_rdg_a5_self_push_fork_admits_front_start_organizing
  RDG-A6 -> test_rdg_a6_finish_word_path_issues_start_organizing_and_exit_signal
           -> test_rdg_a6_sweep_guide_round_no_archive_then_auto_finish_idempotent
  RDG-A7 -> test_rdg_a7_guide_organizing_injection_uses_kidpal_speak_channel

Run protocol（offline；-m "not live" 可跑；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_record_dialogue_guide.acceptance.py

REVISION LOG
  2026-10-19 初版冻结（acceptance-author；RDG-A1..A7；S1 纯决策面 + S2 host 齿 +
             NFR-PERF-3 五边界 + 120s 语义退役配套；live/真机面如实申报不造
             offline 假齿——261018 SLP-A6 同款口径）。
  2026-10-20 RDG-A6 收尾词 case 补 organize 成功桩（test_rdg_a6_finish_word_path_
             issues_start_organizing_and_exit_signal；断言零改，非语义重钉）。
             理由 = SAO-A1 族重钉（261019 同包 fixer R2 真机轮 + human 裁定「SAO-A1
             族重钉 + builder 接线三式收尾 organize」）使 organize 失败 = 不归档，
             离线环境下收尾词路径的「恰一篇归档」前提失真。修订 = 驱动收尾词路径前
             补 _organize_stub（SAO 件同型桩法，monkeypatch front._openclaw_chat_once
             返回罐头忠实整理输出）；三条断言（归档恰一篇 + start_organizing 恰一帧 +
             出语境信号在世）一字未动。授权 = pod-events da35cae4 波及面裁定
             （planner 转达 human）——波及面内最小兼容修订。
  2026-10-20 RDG-A4 收尾沿 case 补 organize 成功桩（test_rdg_a4_front_injects_
             record_context_enter_exit_on_engine_leg；断言零改，非语义重钉）。
             理由 = 同 RDG-A6 条：SAO-A1 族重钉 + builder 接线三式收尾 organize
             后，收尾词路径归档改经整理回合、organize 失败 = 零归档，离线环境下
             本 case 收尾沿的「恰一篇归档」前提同形失真（实测 files=0）。修订 =
             驱动收尾沿的 _engine_to_device 调用包进 _organize_stub（与 RDG-A6
             同款罐头忠实整理输出）；三条断言（归档恰一篇 + 出语境 active=false
             在世 + 注入不尾随）一字未动。授权同上（pod-events da35cae4 波及面
             裁定）——波及面内最小兼容修订。
"""

from __future__ import annotations

import ast
import asyncio
import contextlib
import importlib.util
import json
import logging
import re
import sys
import tempfile
import textwrap
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
PROMPTS_DIR = GATEWAY_DIR / "front" / "prompts"
ENGINE_PATCH_CONNECTION = SERVER / "deploy" / "engine-patch" / "connection.py"

# 冻结协调字面（front 与 engine-patch 的内部信号单源；形状不入契约、入本冻结契约）。
RECORD_CTX_TYPE = "kidpal_record_context"
RECORD_CTX_FLAG = "_kidpal_record_context"
RECORD_CTX_METHOD = "_kidpal_on_record_context"
START_ORGANIZING_ACTION = "start_organizing"
SPEAK_TYPE = "kidpal_speak"

# NFR-PERF-3 已拍轮数（requirements v1.65：轮数已拍；阈值待定不落验收值）。
GUIDE_ROUND_LIMIT_FROZEN = 2


# --------------------------------------------------------------------------- #
# Module loading (missing/unimportable = RED for missing behaviour)           #
# --------------------------------------------------------------------------- #

_story_module_cache = None
_front_module_cache = None


def _story_module():
    """server/gateway/story.py 纯模块。"""
    global _story_module_cache
    if _story_module_cache is not None:
        return _story_module_cache
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.story as st  # noqa: PLC0415 — offline import, cached
    _story_module_cache = st
    return st


def _story_attr(name: str):
    """(value|None, err) —— story.py 缺失模块级属性 = 设计态红。"""
    st = _story_module()
    val = getattr(st, name, None)
    if val is None:
        return None, (f"server/gateway/story.py 无模块级 {name} —— frozen RDG offline "
                      "contract（S1 卡壳词表/轮次纯决策/整理输入装配）未实现。")
    return val, None


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（M4K1 / SC 冻结件同款先例）。"""
    try:
        import fastapi  # noqa: F401

        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return  # real fastapi（或带可用 FastAPI 类的 stub）在场 —— use it
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    class _StubFastAPI:
        def __init__(self, *args, **kwargs):  # noqa: D107
            pass

        def _identity(self, *args, **kwargs):
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
        spec = importlib.util.spec_from_file_location("kidpal_front_app_rdg", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线"
                      "可装载（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("RDG RED: " + err)
    return mod


def _display_schema():
    """真实冻结机械门 gateway.display_schema。"""
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
# I/O doubles（M4K1 / SC 冻结 seam 同款形态：只替代 WebSocket 两端）             #
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


def _forwarded_objects(sent):
    out = []
    for item in sent:
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict):
            out.append(obj)
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


def _display_payloads(sent_text):
    """Reconstruct [{"view","data"}] from kidpal/display envelopes on the device leg."""
    out = []
    for env in _mcp_envelopes(sent_text):
        args = (env.get("params") or {}).get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_text_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_text"]


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _control_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "control"]


def _start_organizing_frames(sent_text):
    return [p for p in _control_frames(sent_text)
            if (p.get("data") or {}).get("action") == START_ORGANIZING_ACTION]


def _speak_frames(sent):
    """引擎腿上 type==kidpal_speak 的内部注入帧（dict 形态）。"""
    return [o for o in _forwarded_objects(sent) if o.get("type") == SPEAK_TYPE]


def _record_ctx_frames(sent):
    """引擎腿上 type==kidpal_record_context 的等价信号帧（dict 形态）。"""
    return [o for o in _forwarded_objects(sent) if o.get("type") == RECORD_CTX_TYPE]


_CTX = json.dumps({"type": "kidpal_session_ctx", "source_page": "story"},
                  ensure_ascii=False)
_STOP = json.dumps({"type": "listen", "state": "stop"}, ensure_ascii=False)


@contextlib.contextmanager
def _alert_legs(front, device_ws, engine_ws):
    """临时登记 sweep 推送腿（_register_alert_legs 既有缝），退出时清理。"""
    old = list(getattr(front, "_alert_legs", []))
    front._register_alert_legs(device_ws, engine_ws)
    try:
        yield
    finally:
        front._alert_legs[:] = old


@contextlib.contextmanager
def _organize_stub(front, reply):
    """organize 成功桩（SAO 件 test_m4k1_story_cloud_link.acceptance.py _organize_stub
    同型桩法，2026-10-20 波及面修订）：monkeypatch front 侧整理回合入口
    _openclaw_chat_once（front_app 冻结 seam）返回罐头忠实整理输出（标题行 + 正文行）。
    失败路径**不打桩**（离线连接拒绝快失败 = EQ-4 既有环境假设）。
    授权凭据：pod-events da35cae4 波及面裁定（planner）——SAO-A1 族重钉 + builder
    接线三式收尾 organize 后，收尾词路径归档改经整理回合，organize 失败 = 不归档；
    本桩保证 RDG-A6 收尾词 case 在重钉后的接线形态下仍钉机械下发/出语境信号语义
    （断言零改、非语义重钉）。"""
    calls = []

    def _fake(system_text, user_text, timeout_s=10.0):  # noqa: ANN001, ANN202
        calls.append((system_text, user_text))
        return reply

    old = getattr(front, "_openclaw_chat_once", None)
    front._openclaw_chat_once = _fake
    try:
        yield calls
    finally:
        if old is None:
            front.__dict__.pop("_openclaw_chat_once", None)
        else:
            front._openclaw_chat_once = old


# =========================================================================== #
# RDG-A1 — 明示卡壳词表纯函数（E3 检出器形态）                                  #
# =========================================================================== #


class RdgStuckPhraseAcceptance(unittest.TestCase):
    """RDG-A1: is_stuck_phrase 同构子串扫描 + STUCK_PHRASES 词表单源可调。"""

    maxDiff = None

    def test_rdg_a1_stuck_phrase_substring_scan_and_wordlist_single_source(self):
        """coverage: RDG-A1 / S-F2.2-AC2 + E3 — is_stuck_phrase 与 is_finish_phrase
        同构（strip 后子串包含、末尾标点不阻断、空/None/非 str -> False）；
        词表 STUCK_PHRASES 为单源（临时改词表 -> 判定随动，实现期可调证明）。"""
        st = _story_module()
        fn = getattr(st, "is_stuck_phrase", None)
        self.assertTrue(callable(fn),
                        "RDG-A1 RED: story.py 无 is_stuck_phrase —— 明示卡壳词检测"
                        "纯函数未实现（E3 检出器形态，S-F2.2-AC2）。")
        words, err = _story_attr("STUCK_PHRASES")
        if err:
            self.fail("RDG-A1 RED: " + err)
        self.assertIsInstance(words, tuple, "RDG-A1 RED: STUCK_PHRASES 非 tuple 常量。")
        self.assertTrue(words, "RDG-A1 RED: STUCK_PHRASES 为空 —— 词表不可为空表。")
        for w in words:
            self.assertIsInstance(w, str, f"RDG-A1 RED: 词表元素非 str: {w!r}。")
        # S-F2.2-AC2 示例起点词必须命中（「如」= 示例非穷尽；起点词缺席 = 词表未按
        # story 语义起步）。
        for text in ("我卡住了", "我想了一下，卡住了。", "想不起来了", "嗯……想不起来"):
            self.assertTrue(fn(text), f"RDG-A1 RED: AC2 示例卡壳话未命中: {text!r}。")
        for text in ("我今天去公园玩了", "从前有一只小猫", "", "   ", None, 123):
            self.assertFalse(fn(text), f"RDG-A1 RED: 非卡壳话误判: {text!r}。")
        # 末尾标点不阻断（与 is_finish_phrase 同构语义）。
        self.assertTrue(fn("真的卡住了。"), "RDG-A1 RED: 末尾标点阻断了子串匹配。")
        # 词表单源：临时换词 -> 判定随动（实现期可调；finally 还原）。
        old = st.STUCK_PHRASES
        try:
            st.STUCK_PHRASES = ("接不下去",)
            self.assertFalse(fn("我卡住了"),
                             "RDG-A1 RED: is_stuck_phrase 未消费 STUCK_PHRASES 词表"
                             "（判定与词表双源 = 词表不可调）。")
            self.assertTrue(fn("接不下去了"), "RDG-A1 RED: 换表后新词未命中。")
        finally:
            st.STUCK_PHRASES = old
        # 收尾词检测既有函数零偏移（must-stay-green：卡壳词不得吃掉收尾词语义）。
        self.assertTrue(callable(getattr(st, "is_finish_phrase", None)),
                        "RDG-A1 RED: is_finish_phrase 在世性被破（既有收尾词检测缺失）。")


# =========================================================================== #
# RDG-A2 — 卡壳计时与引导轮次纯决策（NFR-PERF-3 五条边界）                       #
# =========================================================================== #


class RdgStuckTimerAcceptance(unittest.TestCase):
    """RDG-A2: StoryAccumulator 卡壳计时/引导轮次/自动收尾纯决策，时钟注入快进。"""

    maxDiff = None

    def _acc(self, st, tmp):
        acc = st.StoryAccumulator(str(tmp))
        return acc

    def _check_constants(self, st):
        """阈值常量不落验收值（GAP-3）：只断言名字在世 + 正数；轮数 2 已拍冻结。"""
        limit, err = _story_attr("GUIDE_ROUND_LIMIT")
        if err:
            self.fail("RDG-A2 RED: " + err)
        self.assertEqual(limit, GUIDE_ROUND_LIMIT_FROZEN,
                         f"RDG-A2 RED: GUIDE_ROUND_LIMIT = {limit!r} != 2 —— 自动收尾"
                         "轮数已拍（NFR-PERF-3 / S-F2.2-AC5），不得改。")
        secs, err = _story_attr("STUCK_GUIDE_SECONDS")
        if err:
            self.fail("RDG-A2 RED: " + err)
        self.assertIsInstance(secs, (int, float),
                              "RDG-A2 RED: STUCK_GUIDE_SECONDS 非数值常量。")
        self.assertGreater(secs, 0,
                           "RDG-A2 RED: STUCK_GUIDE_SECONDS 非正数（阈值缺省须为正）。")
        # 阈值待定红线：验收绝不把它写死为 8（或任何具体值）——本断言显式记录该纪律。
        self.assertNotIsInstance(secs, bool, "RDG-A2 RED: 阈值常量为布尔（类型失真）。")

    # coverage: RDG-A2 -- NFR-PERF-3 边界①：孩子接着说即重置卡壳计时
    def test_rdg_a2_nfr_boundary_1_speech_resets_stuck_timer(self):
        st = _story_module()
        self._check_constants(st)
        with tempfile.TemporaryDirectory(prefix="rdg_a2_b1_") as tmp:
            acc = self._acc(st, tmp)
            t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
            acc.start("s1", now=t0)
            acc.feed("s1", "从前有一只小猫", now=t0)
            # t0+5s 已超注入阈值(3s) -> due；t0+4s 孩子接着说 -> 计时重置。
            self.assertIn("s1", acc.stuck_due_sessions(t0 + timedelta(seconds=5),
                                                       timeout_s=3),
                          "RDG-A2 RED(前置): 注入阈值 3s 未判定 stuck-due —— 纯决策面缺失。")
            acc.feed("s1", "它去公园玩", now=t0 + timedelta(seconds=4))
            self.assertNotIn("s1", acc.stuck_due_sessions(t0 + timedelta(seconds=7),
                                                          timeout_s=3),
                             "RDG-A2 RED: 接着说后 3s（距上句仅 3s）误判 stuck-due —— "
                             "NFR-PERF-3 边界①「孩子接着说即重置」被破。")
            self.assertIn("s1", acc.stuck_due_sessions(t0 + timedelta(seconds=8),
                                                       timeout_s=3),
                          "RDG-A2 RED: 重置后超阈值未再判 due —— 计时锚未随 feed 前移。")

    # coverage: RDG-A2 -- NFR-PERF-3 边界②：停顿超阈值触发引导（>= 边界语义）
    def test_rdg_a2_nfr_boundary_2_threshold_elapsed_triggers_guide(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a2_b2_") as tmp:
            acc = self._acc(st, tmp)
            t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
            acc.start("s2", now=t0)
            acc.feed("s2", "然后呢……", now=t0)
            self.assertNotIn("s2", acc.stuck_due_sessions(t0 + timedelta(seconds=5),
                                                          timeout_s=5),
                             "RDG-A2 RED: 未到注入阈值(5s)误判 due —— 过早引导打断孩子。")
            self.assertIn("s2", acc.stuck_due_sessions(t0 + timedelta(seconds=5.5),
                                                       timeout_s=5),
                          "RDG-A2 RED: 超过注入阈值未判 due —— 超阈值触发引导缺失。")
            # 非累积态会话不受卡壳计时约束（问答会话零影响）。
            acc.feed("s-chat", "闲聊", now=t0)
            self.assertNotIn("s-chat", acc.stuck_due_sessions(t0 + timedelta(seconds=99),
                                                              timeout_s=5),
                             "RDG-A2 RED: 非累积态会话被误判 stuck-due。")

    # coverage: RDG-A2 -- NFR-PERF-3 边界③：明示卡壳词立即触发（不等计时）
    def test_rdg_a2_nfr_boundary_3_explicit_stuck_phrase_immediate(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a2_b3_") as tmp:
            acc = self._acc(st, tmp)
            t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
            acc.start("s3", now=t0)
            acc.feed("s3", "我今天……", now=t0)
            acc.feed("s3", "我卡住了", now=t0 + timedelta(seconds=1))
            # 明示卡壳 = 立即（计时刚 1s、远未到阈值）；判定面 = is_stuck_phrase 命中
            # 即引导（front 接线即时消费），纯决策半钉：命中后 mark_guide_round 立即
            # 计轮且计时锚 = 该轮发出时刻（不等卡壳计时）。
            self.assertTrue(st.is_stuck_phrase("我卡住了"),
                            "RDG-A2 RED(前置): 明示卡壳词未命中（边界③判据面）。")
            acc.mark_guide_round("s3", now=t0 + timedelta(seconds=1))
            self.assertEqual(acc.guide_rounds("s3"), 1,
                             "RDG-A2 RED: 明示卡壳后引导轮未立即计（边界③「立即触发」）。")
            self.assertIn("s3", acc.auto_finish_due_sessions(
                t0 + timedelta(seconds=1) + timedelta(seconds=3),
                timeout_s=3, rounds=1),
                "RDG-A2 RED: 引导轮计时锚未取轮发出时刻（guide_since）—— 明示路径的"
                "无回应计时被卡壳计时污染。")

    # coverage: RDG-A2 -- NFR-PERF-3 边界④：连续 2 轮无回应自动收尾（轮数冻结 2）
    def test_rdg_a2_nfr_boundary_4_two_rounds_no_response_auto_finish(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a2_b4_") as tmp:
            acc = self._acc(st, tmp)
            t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
            acc.start("s4", now=t0)
            acc.feed("s4", "从前有一只小猫", now=t0)
            acc.mark_guide_round("s4", now=t0 + timedelta(seconds=5))     # 引导轮 1
            self.assertNotIn("s4", acc.auto_finish_due_sessions(
                t0 + timedelta(seconds=10), timeout_s=5),
                "RDG-A2 RED: 仅 1 轮无回应即判自动收尾 —— 轮数 2 已拍（NFR-PERF-3），"
                "提前收尾会截断孩子思考。")
            acc.mark_guide_round("s4", now=t0 + timedelta(seconds=10))    # 引导轮 2
            self.assertNotIn("s4", acc.auto_finish_due_sessions(
                t0 + timedelta(seconds=12), timeout_s=5),
                "RDG-A2 RED: 第 2 轮计时未到即判收尾。")
            self.assertIn("s4", acc.auto_finish_due_sessions(
                t0 + timedelta(seconds=15), timeout_s=5),
                "RDG-A2 RED: 连续 2 轮无回应且第 2 轮计时到未判自动收尾 —— "
                "NFR-PERF-3 边界④ / S-F2.2-AC5 被破。")
            # 自动收尾 = 归档不丢弃（断言方向钉死）。
            rec = acc.stop("s4", now=t0 + timedelta(seconds=15))
            self.assertIsNotNone(rec,
                                 "RDG-A2 RED: 自动收尾后 stop 未归档 —— 自动收尾丢弃"
                                 "已讲内容 = 语义回退（S-F2.2-AC5「自动收尾走忠实整理"
                                 "→归档」）。")
            self.assertIn("从前有一只小猫", rec.get("text", ""),
                          "RDG-A2 RED: 自动收尾归档丢失孩子原话。")
            # 孩子中途回应 -> streak 重置：1 轮 + 回应 + 1 轮 不得收尾。
            acc2 = self._acc(st, tmp)
            acc2.start("s4b", now=t0)
            acc2.feed("s4b", "开头", now=t0)
            acc2.mark_guide_round("s4b", now=t0 + timedelta(seconds=5))
            acc2.feed("s4b", "然后它回家了", now=t0 + timedelta(seconds=6))  # 回应
            acc2.mark_guide_round("s4b", now=t0 + timedelta(seconds=11))
            self.assertNotIn("s4b", acc2.auto_finish_due_sessions(
                t0 + timedelta(seconds=16), timeout_s=5),
                "RDG-A2 RED: 两轮之间孩子回应过仍判「连续 2 轮无回应」—— streak 未随"
                "回应重置（连续性语义被破）。")

    # coverage: RDG-A2 -- NFR-PERF-3 边界⑤：收声抑制/返回空轮照常计数至自动收尾
    def test_rdg_a2_nfr_boundary_5_suppressed_rounds_still_count(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a2_b5_") as tmp:
            acc = self._acc(st, tmp)
            t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
            acc.start("s5", now=t0)
            acc.feed("s5", "小猫走到门口", now=t0)
            # 两轮均为收声抑制轮（AI 收声只听 / 引导 LLM 返回空）：照常计轮。
            acc.mark_guide_round("s5", now=t0 + timedelta(seconds=5), suppressed=True)
            self.assertEqual(acc.guide_rounds("s5"), 1,
                             "RDG-A2 RED: 被抑制引导轮未计轮 —— q6 收声不停摆被破"
                             "（会话将挂死不归档）。")
            acc.mark_guide_round("s5", now=t0 + timedelta(seconds=10), suppressed=True)
            self.assertEqual(acc.guide_rounds("s5"), 2,
                             "RDG-A2 RED: 第二个被抑制轮未计。")
            self.assertIn("s5", acc.auto_finish_due_sessions(
                t0 + timedelta(seconds=15), timeout_s=5),
                "RDG-A2 RED: 被抑制轮不计入自动收尾轮次 —— 收声不停摆语义被破"
                "（NFR-PERF-3 边界⑤ / S-F2.2-AC5「引导轮含被抑制轮」）。")
            # 查询不改状态（与 pending/timed_out 决策同缝纪律）。
            self.assertEqual(acc.guide_rounds("s5"), 2, "RDG-A2 RED: 查询改了轮次。")


# =========================================================================== #
# RDG-A3 — 忠实整理输入装配纯函数 + 两枚版本化 prompt 制品                       #
# =========================================================================== #


class RdgOrganizingInputAcceptance(unittest.TestCase):
    """RDG-A3: build_organizing_input 只取孩子的话（只读）+ prompt 制品内容锚。"""

    maxDiff = None

    # coverage: RDG-A3 -- 装配 = accum 拼接（孩子原话），含提问句与静音段幻听串
    #                   （滤除职责全压整理 LLM 指令，不滤装配）；只读不改状态
    def test_rdg_a3_organizing_input_assembles_child_words_only(self):
        st = _story_module()
        acc = st.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_a3_acc"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s-a3", now=t0)
        self.assertIsNone(acc.build_organizing_input("s-a3"),
                          "RDG-A3 RED: 空累积装配非 None（不得编造整理输入）。")
        self.assertIsNone(acc.build_organizing_input("nope"),
                          "RDG-A3 RED: 无会话装配非 None。")
        # 种子 2 + 种子 3 fixture：静音段幻听产物 + 孩子中途提问句显式在输入内。
        acc.feed("s-a3", "从前有一只小猫", now=t0)
        acc.feed("s-a3", "Thank you.", now=t0 + timedelta(seconds=1))     # 幻听串
        acc.feed("s-a3", "小猫后来去哪了？", now=t0 + timedelta(seconds=2))  # 中途提问
        acc.feed("s-a3", "然后它回家了", now=t0 + timedelta(seconds=3))
        got = acc.build_organizing_input("s-a3")
        self.assertIsInstance(got, str, "RDG-A3 RED: 非空累积装配未返回 str。")
        for needle in ("从前有一只小猫", "Thank you.", "小猫后来去哪了？", "然后它回家了"):
            self.assertIn(needle, got,
                          f"RDG-A3 RED: 装配输入丢失 {needle!r} —— 装配层不得代行滤除"
                          "（滤除职责全压整理 LLM 指令，q7-E4/种子 2·3；装配滤了 = "
                          "双源滤除，忠实性责任面被稀释）。")
        before = acc.guide_rounds("s-a3")
        acc.build_organizing_input("s-a3")
        self.assertEqual(acc.guide_rounds("s-a3"), before,
                         "RDG-A3 RED: 装配改了状态（只读义务）——装配消费会破坏归档。")

    # coverage: RDG-A3 -- 两枚版本化 prompt 制品在场 + 忠实/护栏/工具调用禁令约束
    def test_rdg_a3_versioned_prompt_artifacts_carry_constraints(self):
        """coverage: RDG-A3 / S-F2.2-AC2·AC4 + 种子 7（PROPOSAL T3 第 5 条）。
        制品 = front 侧版本化 prompt 文件（home = server/gateway/front/prompts/）；
        引导制品含 AC2 护栏逐字、整理制品含 AC4 忠实义务逐字 + 该回合不触发工具
        调用（源锚代理；运行时生效归 live，GAP-5）。"""
        self.assertTrue(
            PROMPTS_DIR.is_dir(),
            f"RDG-A3 RED: server/gateway/front/prompts/ 目录不存在 —— 两枚 front 侧"
            "版本化 prompt 制品无承载目录（frozen home）。")
        arts = sorted(p for p in PROMPTS_DIR.iterdir()
                      if p.is_file() and p.suffix in (".md", ".txt"))
        self.assertGreaterEqual(
            len(arts), 2,
            "RDG-A3 RED: prompts/ 目录内制品不足 2 枚（引导 + 整理）。")
        blob = "\n".join(p.read_text(encoding="utf-8", errors="replace") for p in arts)
        # 整理制品：AC4 忠实义务逐字片段（只用孩子原话 / 去口水词 / 分段 / 起标题 /
        # 不补情节 / 不换表达）。
        for needle in ("只用", "原话", "口水", "分段", "标题", "不补情节", "不换表达"):
            self.assertIn(needle, blob,
                          f"RDG-A3 RED: 整理制品缺忠实义务锚 {needle!r}（AC4 逐字）。")
        # 引导制品：AC2 护栏逐字（只提问推动 / 不提供情节选项 / 不代编情节）。
        for needle in ("不提供情节选项", "不代编"):
            self.assertIn(needle, blob,
                          f"RDG-A3 RED: 引导制品缺护栏锚 {needle!r}（AC2 逐字）。")
        # 种子 7：该回合不触发工具调用（两枚制品都须约束）。
        self.assertGreaterEqual(
            blob.count("工具调用"), 2,
            "RDG-A3 RED: prompt 制品的「工具调用」禁令约束不足两处 —— 引导与整理"
            "回合都必须约束该回合不触发工具调用（种子 7 / PROPOSAL T3 第 5 条）。")
        # front 接线锚（ADR-001 三面相等接线不变式；R1-F4 provider 出口）。
        src = _front_source()
        self.assertIn("openclaw_stream_request", src,
                      "RDG-A3 RED: front_app.py 未引用 openclaw_stream_request —— "
                      "引导/整理回合未走既有 OpenClaw OpenAI 兼容端点（R1-F4 出口钉；"
                      "直连 provider = 新增出网路径，禁）。")
        self.assertIn("prompts", src,
                      "RDG-A3 RED: front_app.py 未引用 prompts 制品面 —— 制品与回合"
                      "指令脱钩（ADR-001 三面相等接线不变式被破）。")


# =========================================================================== #
# RDG-A4 — 等价信号注入 + engine-patch（Package B / 种子 1 host 半）             #
# =========================================================================== #


class RdgRecordContextWiringAcceptance(unittest.TestCase):
    """RDG-A4: front 等价信号注入引擎腿（进/出语境）+ patch 认领/守卫/豁免重绑。"""

    maxDiff = None

    # coverage: RDG-A4 -- 进语境两收敛入口幂等注入 active=true；收尾归档出语境
    #                   active=false（行为级，真泵真跑）
    def test_rdg_a4_front_injects_record_context_enter_exit_on_engine_leg(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a4_ctx_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-rdg-a4"
                up, _dev = _device_to_engine(front, sid, [_CTX])
                enters = [f for f in _record_ctx_frames(up.sent) if f.get("active")]
                self.assertTrue(
                    enters,
                    f"RDG-A4 RED: session_ctx(story) 后引擎腿无 {RECORD_CTX_TYPE} "
                    "active=true 注入 —— Package B 等价信号进语境注入缺失（ADR-013；"
                    "引擎侧抑制 LLM 无信号载体 = 抑制不发生）。")
                self.assertNotIn("kidpal_session_ctx", _forwarded_frame_types(up.sent),
                                 "RDG-A4 RED: session_ctx 被透传（既有拦截语义回退）。")
                # 幂等：重复触发不重复注入（进语境收敛入口幂等）。
                up2, _ = _device_to_engine(front, sid, [_CTX])
                self.assertEqual(
                    len([f for f in _record_ctx_frames(up2.sent) if f.get("active")]), 0,
                    "RDG-A4 RED: 已在记录语境重复 ctx 重复注入 enter —— 收敛入口非幂等。")
                # control/start_recording 收敛入口（引擎侧）同样注入。
                sid2 = "sess-rdg-a4b"
                up3, _ = _engine_to_device(front, sid2, [{
                    "type": "kidpal_display_blocks",
                    "blocks": [json.dumps({"view": "control",
                                           "data": {"action": "start_recording"}},
                                          ensure_ascii=False)]}])
                self.assertTrue(
                    [f for f in _record_ctx_frames(up3.sent) if f.get("active")],
                    "RDG-A4 RED: control/start_recording 收敛入口未注入 enter 信号 "
                    "（两入口必须收敛同一等价信号注入）。")
                # 出语境：收尾词归档同拍注入 active=false。
                _device_to_engine(front, sid, [_STOP])
                # organize 成功桩（2026-10-20 波及面修订，与 RDG-A6 同款；pod-events
                # da35cae4 波及面裁定 + SAO-A1 族重钉后收尾词路径归档经整理回合，
                # organize 失败 = 不归档——离线环境下不补桩则「恰一篇归档」前提
                # 失真；断言零改，非语义重钉）。
                with _organize_stub(front, "小猫回家了\n小猫回家了，它玩得很开心。"):
                    up4, dev4 = _engine_to_device(
                        front, sid, [{"type": "stt", "text": "小猫回家了。我说完了"}])
                self.assertEqual(len(list(Path(tmp).glob("*.md"))), 1,
                                 "RDG-A4 RED(前置): 收尾词路径未归档（既有链路回退）。")
                exits = [f for f in _record_ctx_frames(up4.sent)
                         if f.get("active") is False]
                self.assertTrue(
                    exits,
                    "RDG-A4 RED: 收尾归档后引擎腿无 active=false 出语境信号 —— 出语境"
                    "集合缺「三式收尾」成员（引擎抑制不解除 = 会话后 LLM 永久哑）。")
                # 引导注入不尾随（种子 1 host 半）：归档后的 speak/organizing 链不产生
                # 新的 enter 信号（不回记录语境）。
                self.assertFalse(
                    [f for f in _record_ctx_frames(up4.sent) if f.get("active")],
                    "RDG-A4 RED: 收尾后出现新的 enter 注入 —— 出语境被回滚（种子 1 "
                    "「注入不尾随」host 半被破）。")

    # coverage: RDG-A4 -- patch 认领 + chat 第三守卫（AST 行为/结构齿，ET 先例）
    def test_rdg_a4_patch_claims_signal_and_chat_third_guard(self):
        self._patch_claims_and_guard()

    def _patch_claims_and_guard(self):
        src, tree, err = _load_connection()
        if err:
            self.fail("RDG-A4 RED(infra): " + err)
        # 认领方法在世（方法名冻结）。
        node = _find_func(tree, RECORD_CTX_METHOD)
        self.assertIsNotNone(
            node,
            f"RDG-A4 RED: connection.py 无方法 {RECORD_CTX_METHOD} —— 等价信号认领缝"
            "未落地（Package B；设计态红 = 冻结契约未实现，非收集错）。")
        seg = ast.get_source_segment(src, node)
        self.assertIsNotNone(seg, "RDG-A4 RED(infra): 认领方法源切片失败。")
        ns = {"json": json, "re": re, "queue": __import__("queue"), "asyncio": asyncio,
              "TAG": "rdg_acceptance"}
        exec(compile(textwrap.dedent(seg), str(ENGINE_PATCH_CONNECTION), "exec"), ns)
        fn = ns.get(RECORD_CTX_METHOD)
        self.assertTrue(callable(fn), "RDG-A4 RED(infra): 认领方法不可调用。")

        class _Log:
            def bind(self, **_kw):
                return self

            def info(self, message):
                pass

            def error(self, message):
                pass

        # 行为：active=true 置标志 / false 清标志 / 非本类型零扰动。
        conn = Mock()
        conn.logger = _Log()
        conn._kidpal_record_context = False
        asyncio.run(fn(conn, json.dumps({"type": RECORD_CTX_TYPE, "active": True})))
        self.assertTrue(conn._kidpal_record_context,
                        "RDG-A4 RED: 认领 active=true 未置记录语境标志。")
        asyncio.run(fn(conn, json.dumps({"type": RECORD_CTX_TYPE, "active": False})))
        self.assertFalse(conn._kidpal_record_context,
                         "RDG-A4 RED: 认领 active=false 未清记录语境标志。")
        before = conn._kidpal_record_context
        asyncio.run(fn(conn, json.dumps({"type": "listen", "state": "stop"})))
        self.assertEqual(conn._kidpal_record_context, before,
                         "RDG-A4 RED: 非等价信号帧被认领扰动（认领必须只认本类型）。")

        # chat() 第三守卫：depth==0 块内记录语境守卫 + return，先于 dialogue.put /
        # TTS FIRST；不置 client_abort；depth>0 不拦。
        chat = _find_func(tree, "chat")
        self.assertIsNotNone(chat, "RDG-A4 RED(infra): connection.py 无 chat 方法。")
        depth0 = None
        for n in ast.walk(chat):
            if (isinstance(n, ast.If) and isinstance(n.test, ast.Compare)
                    and len(n.test.ops) == 1 and isinstance(n.test.ops[0], ast.Eq)
                    and isinstance(n.test.left, ast.Name) and n.test.left.id == "depth"
                    and isinstance(n.test.comparators[0], ast.Constant)
                    and n.test.comparators[0].value == 0):
                if depth0 is None or n.lineno < depth0.lineno:
                    depth0 = n
        self.assertIsNotNone(depth0, "RDG-A4 RED(infra): chat 无 depth==0 分支。")

        def refs(n, name):
            return any((isinstance(x, ast.Attribute) and x.attr == name)
                       or (isinstance(x, ast.Name) and x.id == name)
                       for x in ast.walk(n))

        def has_return(n):
            return any(isinstance(x, ast.Return) for x in ast.walk(n))

        guard = next((g for g in ast.walk(depth0)
                      if isinstance(g, ast.If) and g is not depth0
                      and refs(g.test, RECORD_CTX_FLAG) and has_return(g)), None)
        self.assertIsNotNone(
            guard,
            f"RDG-A4 RED: chat depth==0 块内无「{RECORD_CTX_FLAG} 守卫 + return」第三守卫"
            " —— 记录语境中 LLM 派发未抑制（Package B 读法乙本体；设计态红）。")
        first_dialogue_put = min(
            (n.lineno for n in ast.walk(chat)
             if isinstance(n, ast.Call) and isinstance(n.func, ast.Attribute)
             and n.func.attr == "put" and isinstance(n.func.value, ast.Attribute)
             and n.func.value.attr == "dialogue"),
            default=None)
        first_tts_put = min(
            (n.lineno for n in ast.walk(chat)
             if isinstance(n, ast.Call) and isinstance(n.func, ast.Attribute)
             and n.func.attr == "put" and isinstance(n.func.value, ast.Attribute)
             and n.func.value.attr == "tts_text_queue"),
            default=None)
        self.assertIsNotNone(first_dialogue_put, "RDG-A4 RED(infra): chat 无 dialogue.put。")
        self.assertIsNotNone(first_tts_put, "RDG-A4 RED(infra): chat 无 TTS FIRST 入队。")
        self.assertLess(
            guard.lineno, first_dialogue_put,
            "RDG-A4 RED: 第三守卫晚于 dialogue.put —— 守卫必须在 LLM 派发与 TTS FIRST"
            "入队之前 return（跳过即零 LLM 零 TTS）。")
        self.assertLess(
            guard.lineno, first_tts_put,
            "RDG-A4 RED: 第三守卫晚于 TTS FIRST 入队 —— 守卫内不得先入队。")
        abort_assigns = [
            n for n in ast.walk(guard)
            if isinstance(n, (ast.Assign, ast.AugAssign))
            and any(isinstance(t, ast.Attribute) and t.attr == "client_abort"
                    for t in ast.walk(n))]
        self.assertFalse(
            abort_assigns,
            "RDG-A4 RED: 第三守卫内置 client_abort —— 会波及 front 注入的合法 "
            "speak/story 下发（ET 同款纪律）。")
        # _route_message 拦截顺序：认领在 _kidpal_try_speak 之后、handleTextMessage 之前。
        route = _find_func(tree, "_route_message")
        self.assertIsNotNone(route, "RDG-A4 RED(infra): 无 _route_message。")

        def call_linenos(fn_node, name):
            return sorted({n.lineno for n in ast.walk(fn_node)
                           if isinstance(n, ast.Call)
                           and ((isinstance(n.func, ast.Name) and n.func.id == name)
                                or (isinstance(n.func, ast.Attribute)
                                    and n.func.attr == name))})

        claim_ln = call_linenos(route, RECORD_CTX_METHOD)
        speak_ln = call_linenos(route, "_kidpal_try_speak")
        handle_ln = call_linenos(route, "handleTextMessage")
        self.assertTrue(
            claim_ln and speak_ln and handle_ln,
            f"RDG-A4 RED(infra): _route_message 缺关键调用（claim={claim_ln}, "
            f"speak={speak_ln}, handle={handle_ln})。")
        self.assertLess(
            max(claim_ln), min(handle_ln),
            "RDG-A4 RED: 等价信号认领晚于 handleTextMessage —— 信号会被当普通文本"
            "送 LLM（先认领后转纪律）。")

    # coverage: RDG-A4 -- stop-truncation 豁免重绑（结构性风险 2，KEY 收尾末句承重）
    def test_rdg_a4_patch_stop_truncation_exempt_record_context(self):
        src, tree, err = _load_connection()
        if err:
            self.fail("RDG-A4 RED(infra): " + err)
        node = _find_func(tree, "_kidpal_on_listen_message")
        self.assertIsNotNone(
            node,
            "RDG-A4 RED(infra): connection.py 无 _kidpal_on_listen_message —— 261014 "
            "截断面被移除（既有齿面回退，须复核）。")
        seg = ast.get_source_segment(src, node)
        ns = {"json": json, "re": re, "queue": __import__("queue"), "asyncio": asyncio,
              "TAG": "rdg_acceptance"}
        exec(compile(textwrap.dedent(seg), str(ENGINE_PATCH_CONNECTION), "exec"), ns)
        fn = ns.get("_kidpal_on_listen_message")

        class _Log:
            def bind(self, **_kw):
                return self

            def info(self, message):
                pass

            def error(self, message):
                pass

        # 记录语境 + 非 manual 聆听形态（auto）+ listen stop：豁免——不截断、不排空
        # （KEY 收尾末句转写承重通道；重绑判据 = 记录语境标志 OR 既有 manual）。
        conn = Mock()
        conn.logger = _Log()
        conn._kidpal_record_context = True
        conn.client_listen_mode = "auto"
        conn.asr_audio = ["tail-chunk"]
        q = __import__("queue").Queue()
        q.put("q-1")
        conn.asr_audio_queue = q
        conn._kidpal_listen_closed = False
        asyncio.run(fn(conn, json.dumps({"type": "listen", "state": "stop"})))
        self.assertFalse(
            conn._kidpal_listen_closed,
            "RDG-A4 RED: 记录语境（auto 形态）stop 被截断 —— 豁免重绑缺失（结构性"
            "风险 2：KEY 收尾路径末句转写被截断清空 = 毁稿）。")
        self.assertEqual(conn.asr_audio, ["tail-chunk"],
                         "RDG-A4 RED: 记录语境 stop 排空了在途音频缓冲。")
        self.assertEqual(q.qsize(), 1,
                         "RDG-A4 RED: 记录语境 stop 排空了在途 PCM 队列。")
        # 非记录语境 auto 照常截断（261014 既有语义零偏移）。
        conn2 = Mock()
        conn2.logger = _Log()
        conn2._kidpal_record_context = False
        conn2.client_listen_mode = "auto"
        conn2.asr_audio = ["tail-chunk"]
        q2 = __import__("queue").Queue()
        q2.put("q-1")
        conn2.asr_audio_queue = q2
        conn2._kidpal_listen_closed = False
        asyncio.run(fn(conn2, json.dumps({"type": "listen", "state": "stop"})))
        self.assertTrue(
            conn2._kidpal_listen_closed,
            "RDG-A4 RED: 非记录语境 auto 模式 stop 未截断 —— 261014 既有语义零偏移"
            "被破（既有截断齿保绿义务）。")
        self.assertEqual(conn2.asr_audio, [],
                         "RDG-A4 RED: 非记录语境 stop 未清音频缓冲（截断不完整）。")

    # coverage: RDG-A4 -- 引导 speak 注入零会话路由副作用（种子 1 host 半，结构齿）
    def test_rdg_a4_guide_speak_injection_does_not_clear_context_flag(self):
        src, tree, err = _load_connection()
        if err:
            self.fail("RDG-A4 RED(infra): " + err)
        node = _find_func(tree, "_kidpal_try_speak")
        self.assertIsNotNone(node, "RDG-A4 RED(infra): 无 _kidpal_try_speak。")
        refs_closed = [n for n in ast.walk(node)
                       if isinstance(n, ast.Attribute)
                       and n.attr == "_kidpal_listen_closed"]
        self.assertFalse(
            refs_closed,
            "RDG-A4 RED: _kidpal_try_speak 体内引用 _kidpal_listen_closed —— speak "
            "注入触发截断态 = 注入尾随会话路由变化（种子 1 host 半）。")
        # 负向断言：speak 注入不得对记录语境标志赋值（读引用允许——豁免判据重绑后
        # speak 路径可能需要读标志）。
        assigns = [
            n for n in ast.walk(node)
            if isinstance(n, (ast.Assign, ast.AugAssign))
            and any(isinstance(t, ast.Attribute) and t.attr == RECORD_CTX_FLAG
                    for t in ast.walk(n))]
        self.assertFalse(
            assigns,
            "RDG-A4 RED: _kidpal_try_speak 体内对记录语境标志赋值 —— 引导注入不得清"
            "记录语境标志（注入不尾随 auto-listen / 不离记录会话，种子 1 host 半）。")


# =========================================================================== #
# RDG-A5 — display_schema 自推通路分叉（结构性风险 1）                           #
# =========================================================================== #


class RdgDisplaySchemaForkAcceptance(unittest.TestCase):
    """RDG-A5: start_organizing 网关决定类 relay 拒转 + front 自推放行 + 零偏移。"""

    maxDiff = None

    # coverage: RDG-A5 -- relay/引擎侧路径拒转 start_organizing；决定类集合在世；
    #                   end_session/start_recording 既有语义零偏移（must-stay-green）
    def test_rdg_a5_start_organizing_gateway_decision_rejected_on_relay(self):
        ds = _display_schema()
        actions = getattr(ds, "GATEWAY_DECISION_CONTROL_ACTIONS", None)
        self.assertIsInstance(actions, (set, frozenset),
                              "RDG-A5 RED(infra): GATEWAY_DECISION_CONTROL_ACTIONS 缺失。")
        self.assertIn(START_ORGANIZING_ACTION, actions,
                      "RDG-A5 RED: GATEWAY_DECISION_CONTROL_ACTIONS 不含 "
                      "start_organizing —— 网关决定类未登记（契约 §1.4 与 end_session "
                      "同型）。")
        fwd = getattr(ds, "FORWARDABLE_CONTROL_ACTIONS", frozenset())
        self.assertNotIn(
            START_ORGANIZING_ACTION, fwd,
            "RDG-A5 RED: start_organizing 进了可透传集合 —— 决定类不得经引擎 display "
            "流转发（伪造/注入面）。")
        payload = {"view": "control", "data": {"action": START_ORGANIZING_ACTION}}
        ok, reason = ds.validate_display(payload)
        self.assertFalse(
            ok,
            f"RDG-A5 RED: 引擎侧 control/start_organizing 通过了真门 ({reason}) —— "
            "网关决定类拒转缺失（结构性风险 1：自推会被自拒的对偶 = relay 必须仍拒）。")
        # 既有语义零偏移。
        ok_end, _ = ds.validate_display(
            {"view": "control", "data": {"action": "end_session"}})
        self.assertFalse(ok_end, "RDG-A5 RED: end_session 拒转语义偏移（must-stay-green）。")
        ok_rec, reason_rec = ds.validate_display(
            {"view": "control", "data": {"action": "start_recording"}})
        self.assertTrue(ok_rec,
                        f"RDG-A5 RED: start_recording 被误拒 ({reason_rec}) —— 既有可透"
                        "传语义零偏移被破。")
        # relay 层（build_display_tool_calls）对 start_organizing 块降级、不透传。
        front = _front()
        block = json.dumps(payload, ensure_ascii=False)
        calls = front.build_display_tool_calls([block])
        self.assertEqual(len(calls), 1, "RDG-A5 RED: start_organizing 块中继数 != 1。")
        payloads = _display_payloads(
            [json.dumps({"type": "mcp", "payload": calls[0]})])
        self.assertTrue(payloads, "RDG-A5 RED: relay 输出不可重建。")
        relayed = payloads[0]
        self.assertFalse(
            relayed.get("view") == "control"
            and (relayed.get("data") or {}).get("action") == START_ORGANIZING_ACTION,
            "RDG-A5 RED: relay 把 control/start_organizing 透传给设备 —— 决定类只能走 "
            "front 机械下发单源（契约 §1.4）。")

    # coverage: RDG-A5 -- 自推通路分叉：self_push 放行 start_organizing；front 自推缝
    #                   真跑恰一帧；end_session 即便自推仍拒
    def test_rdg_a5_self_push_fork_admits_front_start_organizing(self):
        ds = _display_schema()
        payload = {"view": "control", "data": {"action": START_ORGANIZING_ACTION}}
        try:
            ok, reason = ds.validate_display(payload, self_push=True)
        except TypeError as e:
            self.fail("RDG-A5 RED: validate_display 无 keyword-only self_push 形参 —— "
                      f"自推通路分叉未按冻结 seam 落地（{e}；结构性风险 1）。")
        self.assertTrue(
            ok,
            f"RDG-A5 RED: 自推通路仍拒 start_organizing ({reason}) —— front 机械下发"
            "被自拒（结构性风险 1：start_organizing 入网关决定类后自推通路不分叉 = "
            "收尾永远下发不出去）。")
        ok_end, _ = ds.validate_display(
            {"view": "control", "data": {"action": "end_session"}}, self_push=True)
        self.assertFalse(
            ok_end,
            "RDG-A5 RED: end_session 借 self_push 放行 —— 自推豁免只许 start_organizing"
            "（会话结束决定不因通路放行）。")
        # front 自推缝真跑：恰一帧下发给设备腿。
        with tempfile.TemporaryDirectory(prefix="rdg_a5_push_") as tmp:
            with _front_with_stories(tmp) as front:
                dev = _FakeDeviceSocket()
                asyncio.run(front._push_display_payload(dev, payload))
                frames = _start_organizing_frames(dev.sent_text)
                self.assertEqual(
                    len(frames), 1,
                    f"RDG-A5 RED: front 自推 start_organizing 下发 {len(frames)} 帧"
                    "（应恰一帧）—— 自推缝未接分叉或分叉误杀机械下发。")


# =========================================================================== #
# RDG-A6 — 卡壳计时 sweep + 三式收尾统一 start_organizing 下发                    #
# =========================================================================== #


class RdgSweepOrganizingAcceptance(unittest.TestCase):
    """RDG-A6: sweep 重写（引导轮不归档；2 轮无回应自动收尾 → start_organizing 幂等）
    + 收尾词路径统一下发 + 出语境信号。"""

    maxDiff = None

    # coverage: RDG-A6 -- 收尾词路径：归档同拍下发 start_organizing + 出语境信号
    def test_rdg_a6_finish_word_path_issues_start_organizing_and_exit_signal(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a6_fin_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-rdg-a6-fin"
                up, dev = _FakeUpstream(), _FakeDeviceSocket()
                _device_to_engine(front, sid, [_CTX])
                _device_to_engine(front, sid, [_STOP])
                # organize 成功桩（2026-10-20 波及面修订，pod-events da35cae4；SAO-A1
                # 族重钉后收尾词路径归档经整理回合，organize 失败 = 不归档——离线
                # 环境下不补桩则「恰一篇归档」前提失真；断言零改，非语义重钉）。
                with _organize_stub(front, "小猫回家了\n小猫回家了，它玩得很开心。"):
                    up2, dev2 = _engine_to_device(
                        front, sid, [{"type": "stt", "text": "小猫回家了。我说完了"}])
                files = list(Path(tmp).glob("*.md"))
                self.assertEqual(len(files), 1,
                                 "RDG-A6 RED(前置): 收尾词路径未归档（既有链路回退）。")
                org = _start_organizing_frames(dev2.sent_text)
                self.assertEqual(
                    len(org), 1,
                    f"RDG-A6 RED: 收尾词归档后 start_organizing 下发 {len(org)} 帧"
                    "（应恰一帧）—— 三式收尾统一下发缺失（q2 = ⓒ 裁定；wire 形状 "
                    "view=control + data.action=start_organizing）。")
                exits = [f for f in _record_ctx_frames(up2.sent)
                         if f.get("active") is False]
                self.assertTrue(
                    exits,
                    "RDG-A6 RED: 收尾词路径未注入出语境信号（出语境集合缺收尾成员）。")

    # coverage: RDG-A6 -- sweep：stuck-due 引导轮（LLM 不可达 = 抑制计轮）不归档、
    #                   不下发；2 轮无回应自动收尾 → start_organizing 恰一帧幂等
    def test_rdg_a6_sweep_guide_round_no_archive_then_auto_finish_idempotent(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="rdg_a6_sweep_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-rdg-a6-sweep"
                acc = front._get_story_acc()
                t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
                _device_to_engine(front, sid, [_CTX])
                acc.feed(sid, "从前有一只小猫", now=t0)
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                with _alert_legs(front, dev, up):
                    # sweep ①：stuck-due（10 分钟 >> 任何合理阈值）-> 引导轮。
                    # LLM 不可达（离线）= 本轮不播报仍计轮（E3/q6）。
                    asyncio.run(front._story_timeout_sweep(
                        now=t0 + timedelta(seconds=600)))
                    self.assertGreaterEqual(
                        acc.guide_rounds(sid), 1,
                        "RDG-A6 RED: stuck-due 后引导轮未计（引导回合未接线或未计轮"
                        "——LLM 失败/返回空也必须计轮，q6/E3）。")
                    self.assertEqual(
                        _start_organizing_frames(dev.sent_text), [],
                        "RDG-A6 RED: 引导轮即下发 start_organizing —— 引导不是收尾"
                        "（提前收尾截断孩子思考，S-F2.2-AC2）。")
                    self.assertEqual(
                        len(list(Path(tmp).glob("*.md"))), 0,
                        "RDG-A6 RED: 引导轮即归档 —— 引导轮不得触发整理归档。")
                    self.assertEqual(
                        len(_story_text_frames(dev.sent_text)), 0,
                        "RDG-A6 RED: 引导轮下发 story_text。")
                    # sweep ②：第 2 轮引导（仍无回应）。
                    asyncio.run(front._story_timeout_sweep(
                        now=t0 + timedelta(seconds=1200)))
                    self.assertGreaterEqual(
                        acc.guide_rounds(sid), 2,
                        "RDG-A6 RED: 第二轮引导未计轮。")
                    self.assertEqual(
                        _start_organizing_frames(dev.sent_text), [],
                        "RDG-A6 RED: 第 2 轮发出即收尾 —— 自动收尾须等第 2 轮计时到"
                        "仍无回应（NFR-PERF-3 边界④ 的接线半）。")
                    # sweep ③：第 2 轮计时到仍无回应 -> 自动收尾。
                    asyncio.run(front._story_timeout_sweep(
                        now=t0 + timedelta(seconds=1800)))
                    org = _start_organizing_frames(dev.sent_text)
                    self.assertEqual(
                        len(org), 1,
                        f"RDG-A6 RED: 2 轮无回应计时到后 start_organizing 下发 "
                        f"{len(org)} 帧（应恰一帧）—— S-F2.2-AC5 自动收尾接线缺失。")
                    exits = [f for f in _record_ctx_frames(up.sent)
                             if f.get("active") is False]
                    self.assertTrue(
                        exits,
                        "RDG-A6 RED: 自动收尾未注入出语境信号（引擎抑制不解除）。")
                    # 重复幂等：再次 sweep 不重复下发。
                    asyncio.run(front._story_timeout_sweep(
                        now=t0 + timedelta(seconds=2400)))
                    self.assertEqual(
                        len(_start_organizing_frames(dev.sent_text)), 1,
                        "RDG-A6 RED: 重复 sweep 重复下发 start_organizing —— 三式收尾"
                        "重复幂等被破（契约 §1.4）。")


# =========================================================================== #
# RDG-A7 — S-F1.4-AC6 host 半（注入通道形状 + RM-A3 must-stay-green 引用）        #
# =========================================================================== #


class RdgReminderDeferralAcceptance(unittest.TestCase):
    """RDG-A7: 引导/整理注入 = kidpal_speak 内部指令同型、不产生提醒帧。"""

    maxDiff = None

    # coverage: RDG-A7 -- 引导注入通道 = reminders build_speech_injection 同型
    #                   （{"type":"kidpal_speak","text":...}）；录音期提醒排队延后
    #                   的设备半（RM-A3）既有齿 must-stay-green 引用
    def test_rdg_a7_guide_organizing_injection_uses_kidpal_speak_channel(self):
        """coverage: RDG-A7 / S-F1.4-AC6 host 半 —— front 引导/整理播报注入复用
        ADR-003 kidpal_speak 通道（reminders.py build_speech_injection 同型：只有
        {"type":"kidpal_speak","text":...}，无弹窗/alert 载荷），引导注入不得以
        提醒链路形态下发（防引导把提醒提前弹/弹窗混入记录会话）；设备侧录音排队
        例外（RM-A3，reminder_overlay_unify.acceptance.c tooth 2）must-stay-green
        ——本包引导注入不得破坏「录音中提醒到点不 raise、会话结束立刻弹」。
        端到端（提醒 TTS 不混进录音内容）归真机轮 S5（live-manual，GAP-1）。"""
        src = _front_source()
        self.assertTrue(
            ("kidpal_speak" in src) or ("build_speech_injection" in src),
            "RDG-A7 RED: front_app.py 未引用 kidpal_speak 注入通道 —— 引导/整理播报"
            "未走 ADR-003 同型内部指令（reminders.py build_speech_injection 先例）。")
        # 引导/整理注入不得产生 alert/提醒帧形状（提醒帧 = kidpal/alert mcp 信封，
        # 只归 reminders 投递链路；front story 面不引用 alert 信封构造）。
        import gateway.reminders as gw_reminders  # noqa: PLC0415
        self.assertTrue(
            callable(getattr(gw_reminders, "build_speech_injection", None)),
            "RDG-A7 RED(infra): reminders.build_speech_injection 缺失（先例通道不在世）。")
        inj = gw_reminders.build_speech_injection({"speech": "那后来呢？"})
        self.assertEqual(inj, {"type": SPEAK_TYPE, "text": "那后来呢？"},
                         "RDG-A7 RED: kidpal_speak 注入形状偏移（先例通道被改）。")
        self.assertIsNone(
            gw_reminders.build_speech_injection({"speech": ""}),
            "RDG-A7 RED: 空 speech 注入未按降级返回 None（先例语义偏移）。")
        # 设备半（firmware）RM-A3 门控以页面态为权威——锚在 firmware 冻结件
        # RDG-F5（mcp_server.cc RaiseReminderQueue rec_now 按 g_story_view.page 判定），
        # 既有 reminder_overlay_unify.acceptance.c tooth 2（RM-A3 决策表）must-stay-green
        # 继续承载录音排队例外语义。本文件不重钉设备半（固件冻结件边界）。


# --------------------------------------------------------------------------- #
# connection.py 载入 + AST 抽取（不可离线 import → 抽真实定义 exec；ET 先例）     #
# --------------------------------------------------------------------------- #

_conn_cache = None


def _load_connection():
    """Return (src, tree, err). Parse error / missing file = infra RED."""
    global _conn_cache
    if _conn_cache is None:
        if not ENGINE_PATCH_CONNECTION.is_file():
            return None, None, ("server/deploy/engine-patch/connection.py missing — "
                                "engine-patch 面不在世。")
        src = ENGINE_PATCH_CONNECTION.read_text(encoding="utf-8", errors="replace")
        try:
            tree = ast.parse(src)
        except SyntaxError as e:  # noqa: PERF203
            return None, None, f"connection.py failed to parse: {e!r}"
        _conn_cache = (src, tree, None)
    return _conn_cache


def _find_func(tree, name):
    return next(
        (n for n in ast.walk(tree)
         if isinstance(n, (ast.AsyncFunctionDef, ast.FunctionDef)) and n.name == name),
        None,
    )


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
