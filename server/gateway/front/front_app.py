"""KidPal direction-B wss TLS gateway FRONT (assembles server/gateway/*).

Composes the frozen adaptation modules into a runnable TLS-terminating gateway front
that the live acceptance (test_gateway_directionB.acceptance.py, LIVE face) probes:

  * LISTEN 0.0.0.0:8000 with TLS (self-signed >=10y cert, GWA-A9).
  * WS handshake: forced device auth AT THE HTTP UPGRADE layer — no token / wrong
    token => reject before accept() => Starlette answers an HTTP 403 (4xx) the live
    probe reads. token 未设 = refuse-service (auth.authorize never true for empty),
    constant-time compare (gateway.auth). No validation-mode backdoor.
  * POST /command: Bearer auth (+ 401), empty text => 400 (never silent 200).
  * Normalised stream/wss path: accepted authenticated WS -> forwarded to the engine
    (plain ws://127.0.0.1:8010/xiaozhi/v1/, the official container's ws leg) so the
    front is the device-facing wss entry and the engine stays plain ws inside.
  * Adaptation assembly demonstrated on the text/command + dryrun paths:
    display_schema mechanical check, mcp_relay tools/call envelope, fallback
    (4s soothing / error fallback text view / V32 truncate), provider stream shape.

Run (uvicorn, TLS on 0.0.0.0:8000):
  cd server/gateway/front && \
  /opt/kidpal/venv/bin/uvicorn front_app:app \
    --host 0.0.0.0 --port 8000 \
    --ssl-certfile /opt/kidpal/xiaozhi-esp32-server/cert/server.pem \
    --ssl-keyfile  /opt/kidpal/xiaozhi-esp32-server/cert/server.key
env: XIAOZHI_DEVICE_TOKEN (>=22 chars; unset => refuse-service), KIDPAL_ENGINE_WS.
"""

from __future__ import annotations

import asyncio
import collections
import json
import logging
import os
import re
import threading
import time
import uuid
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Optional

from fastapi import FastAPI, Header, Request, WebSocket, WebSocketDisconnect, HTTPException
from fastapi.responses import JSONResponse, PlainTextResponse

# --- compose the frozen gateway building blocks (absolute package import) -----
from gateway import auth as gw_auth
from gateway import display_schema as gw_display
from gateway import mcp_relay as gw_mcp
from gateway import fallback as gw_fallback
from gateway import provider as gw_provider
from gateway import reminders as gw_reminders
from gateway import schedule as gw_schedule
from gateway import schedule_push as gw_schedule_push
from gateway import story as gw_story


app = FastAPI(title="KidPal direction-B streaming gateway front")

log = logging.getLogger("kidpal-front")
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(name)s %(message)s")

DEVICE_TOKEN = os.environ.get("XIAOZHI_DEVICE_TOKEN", "")
ENGINE_WS = os.environ.get("KIDPAL_ENGINE_WS", "ws://127.0.0.1:8010/xiaozhi/v1/")
# Engine side-channel message type (pod-events engine-patch-approval): the patched engine
# emits the stripped display blocks on this type; front is the validation/relay AUTHORITY
# (drives them through display_schema + kidpal/display tools/call) and never forwards the
# engine-internal raw frame to the device.
SIDE_CHANNEL_TYPE = "kidpal_display_blocks"

# Q7 relay 收紧（契约 §1.5 step 3 / 2026-09-24 契约门裁定）：story_list/story_text 的
# 数据是 front 机械推送单源（story.py producer），引擎侧（relay 路径）来源的这两个 view
# 块一律降级为友好 text——绝不把 story 载荷透传给设备。front 自推（_push_story_list /
# 归档下发）不经 relay、天然豁免。非 story view 零变化。
_STORY_SINGLE_SOURCE_VIEWS = frozenset({"story_list", "story_text"})

# --- P3 故事 manual 聆听累积态 + 整段转写归档（契约 §1.4 start_recording / P3-A4）------
# front 见 relay 的 control/start_recording -> 把该连接会话切累积态（不按句送对话）；
# 停录整段转写经 gateway.story.archive_story 落盘。真实采音殿后 9b (P3-E2)，此处承载
# 云侧累积/归档的接线点。StoryAccumulator 构造不触碰文件系统（import 期零磁盘副作用）。
STORIES_DIR = os.environ.get("KIDPAL_STORIES_DIR", "/opt/kidpal/stories")
_story_acc = None

# --- P3 记录语境对话式引导（261019-p3-record-dialogue-guide）---------------------------
# ADR-013 Package B 等价信号（服务端内部链路、形状不入契约；协调单源 = 冻结验收
# test_record_dialogue_guide.acceptance.py 文件头）：type=="kidpal_record_context" +
# 布尔 active，经**引擎腿**下发（上行泵 upstream / sweep 的 _alert_legs 引擎腿），
# ADR-003 kidpal_speak 同型内部消息。engine-patch 认领置/清记录语境标志 → chat()
# depth==0 第三守卫抑制 LLM 派发（孩子讲着时 AI 默认闭嘴，切句只回显）。
RECORD_CTX_SIGNAL_TYPE = "kidpal_record_context"
# 收尾统一下行信号（q2 = ⓒ 裁定；契约 §1.4 start_organizing 表行——front 机械下发
# 单源经自推通路 self_push=True 放行，relay 路径恒拒）。
START_ORGANIZING_PAYLOAD = {"view": "control", "data": {"action": "start_organizing"}}
# 固定安抚句（R1-N4 丢稿口径：整理失败 = 当轮素材不归档 + 固定安抚句，不发明新语义；
# PROPOSAL §6.3）。
ORGANIZING_FAIL_SOOTHING = "刚才的话我没能记下来，我们再讲一次好吗？"

# 整理回合进程内单飞锁（261020 D4-1 线程化配套）：_organize_and_archive 移出事件
# 循环（asyncio.to_thread）后，两个并发收尾（多腿/多连接）可能在两线程同时跑
# archive_story——seq 持久账本（.seq.json read-modify-write）的「front 单写者」纪律
# 靠事件循环天然串行的旧形态承载，线程化后须显式串行化还原同语义。
_ORGANIZE_ARCHIVE_LOCK = threading.Lock()

# 归档前内容守卫（261020 D4-2 ①，fixer 裁定；brief 三选组合取 ①，不做 ③）：
# 现场实害 = 空内容 + 幻听「Thank you.」被整理归档成 `2026-10-20-no-reply` 垃圾篇
# （seq=9 占用、SD 落卡 bytes=8）。守卫落点 = _story_finish_session 归档边界
# （strip_trailing_finish_words 之后、organize LLM 回合之前）——**不在**装配层
# （RDG-A3 冻结：build_organizing_input 不代行滤除；本守卫整串判定，混排内容
# 「真话+幻听串」照常放行进整理，只有**纯**幻听/近无内容拒归档）。
# 表与 engine-patch connection.py 幻听过滤同族模式（归一化后整串相等、绝不子串——
# 「谢谢你，这个字怎么写」类真实用户话不得误杀）；不 import engine-patch（部署面
# 隔离：connection.py 属引擎容器部署件，front 侧自有同族实现）。种子集 = 引擎侧
# 幻听表全量（CRQ TECH-261014 真机逐字命中 + ECS 24h 日志证据）+ 中文语气词重复族
# （同 ASR 家族低能量段已知模式）。最小有意义内容阈值 = 归一化去空白标点后 <2 字
# （单语气词/纯标点不归档——孩子单字残余无整理价值，旧形态会归档成垃圾篇）。
_FRONT_ARCHIVE_HALLUCINATIONS = frozenset({
    "thank you",
    "thanks for watching",
    "thank you for watching",
    "谢谢观看",
    "请不吝点赞",
    "嗯嗯",
    "哦哦",
    "呵呵",
    "哈哈",
    "哈哈哈",
    "嘿嘿",
    "嘻嘻",
})
_FRONT_GUARD_STRIP_RE = re.compile(r"[^\w\s]")


def _archive_material_reject_reason(material: str) -> "str | None":
    """归档边界内容守卫：返回拒绝原因串（留痕用）或 None（放行归档）。

    判序：幻听整串命中优先（归一化保空格、整串相等——engine-patch 同族语义，种子
    内含词间空格）→ 命中即 pure_hallucination_material；归一化去空白后 <2 字 =
    insignificant_material（纯标点/单语气词）。绝不子串：只对**整段** material 判定，
    混排内容（真话+幻听串）照常放行。"""
    spaced = _FRONT_GUARD_STRIP_RE.sub("", material or "").strip().lower()
    if spaced in _FRONT_ARCHIVE_HALLUCINATIONS:
        return "pure_hallucination_material"
    compact = re.sub(r"\s+", "", spaced)
    if len(compact) < 2:
        return "insignificant_material"
    return None
# 两枚 front 侧版本化 prompt 制品（home = server/gateway/front/prompts/，目录名冻结；
# 不走 OpenClaw 主 agent 共享 system prompt——request 级 system 消息承载，种子 7）。
PROMPTS_DIR = Path(__file__).resolve().parent / "prompts"
_GUIDE_PROMPT_FILE = "guide-round.v1.md"
_ORGANIZE_PROMPT_FILE = "faithful-organize.v1.md"
# 整理回合 LLM 超时（261021 真机轮 R7 修复）：OpenClaw embedded run 的 prep 阶段实测
# 7.05s（journalctl 09:06:06 trace:embedded-run totalMs=7052，其中 bootstrap-context
# ~6.5s），整理回合长输入 + 长输出还需数秒——原缺省 10s 在模型刚吐 ~1.5s 时即被掐断
# （ClientDisconnectError @ elapsed 1497ms，真机每次必现：10s 对整理回合结构性不够）。
# 60s = prep ~7s + 整理生成长尾留足余量。**引导回合调用点维持缺省 10s 不动**（短 prompt
# 实测可通过，本次不扩大改动面）。
_ORGANIZE_LLM_TIMEOUT_S = 60.0

# --- 记录会话引导节奏三参数（261022-p3-record-front-polish-fix；debt 缺陷③④）-----------
# 名字冻结（FROZEN OFFLINE CONTRACT A 节）、**数值不冻结**：三者都是手感参数，初值取
# 现网实测的合理中位，终判归 human 真机轮（FP-A7），数值不进 spec 文本（FP-X2）。
# 三个都是**模块级单源**：所有消费点每次现读模块全局（不在任何调用点内联字面量），
# 故验收可 monkeypatch 该常量做确定性快进 / 夹紧（内联硬编码 = 单源被破 = 验收红）。
#
# 应答窗（缺陷③：KPLOG005 全天四笔「说完了」笔笔迟到 5~14s 成孤儿帧——末段引导结束与
# control start_organizing 同毫秒下发，会话在 ASR 收尾词链路到达前就被收走）。语义 =
# 末段引导轮发出后，自动收尾不得早于「旧触发点（+STUCK_GUIDE_SECONDS）+ 本窗」，窗内
# 留给收尾词到达并经 is_finish_phrase 走正常收尾（轮数语义 AC5 零触碰，只延后计时起点）。
# 初值 8.0s：与 story.STUCK_GUIDE_SECONDS(8s) 叠加共 16s，覆盖实测 14s 上界。
GUIDE_RESPONSE_WINDOW_SECONDS = 8.0
# 单段引导播报篇幅上限（字符数；缺陷③：39s 会话里引导独播 ~28s，孩子只说了 4 句）。
# 引导制品自身已要求「不超过 20 个字」（prompt 面第一道防线）；本上限是硬护栏——超限
# 按上限截断（截断产物恒为原文前缀，不另编内容）。初值 30 字（≈ 语音 6~8s），留出
# prompt 指令的容差余量而不让回合跑成独白。
GUIDE_SPEECH_MAX_CHARS = 30
# 引导播报窗口 STT 素材过滤时长（秒；缺陷④：KPLOG005 13:02:34「Okay」幻听——human
# 实证非本人声音——直进故事素材缓冲）。锚 = 该会话最近一次引导**播报注入**时刻（被
# 抑制轮无播报、不开窗）；窗口内 stt 只丢素材面，收尾词判定面不在窗内（FP-A1 本体）。
# 初值 8.0s：≈ GUIDE_SPEECH_MAX_CHARS(30 字) 的语音时长上界；窗口过期后孩子接话照常
# 进素材（反向 case 由 FP-A3 兜底，防过滤窗过宽误吞孩子接话）。
GUIDE_STT_GUARD_SECONDS = 8.0
# 故事卡壳 sweep 独立节拍（秒；261022-p3-record-front-polish-fix 真机轮追补）。原 sweep
# 搭 reminder_poll_loop 整分钟节拍：卡壳阈值 STUCK_GUIDE_SECONDS(8s) 却最坏 60s+ 才生效
# （KPLOG006 实测末次 STT 22:28:02 -> 引导 22:29:06，迟到 66s），且整分钟齐射会在孩子
# 正在说话时插话（引导播报 barge-in -> 引擎 abort 竞态 -> 设备键帽卡打断）。固定短间隔
# 独立轮询：4s = 阈值 8s 的一半，引导/收尾判定最坏迟到 4s。**提醒/课表节拍零改动**。
_STORY_SWEEP_INTERVAL_SECONDS = 4.0

# 敷衍标题启发式单源词表（缺陷⑤b；FROZEN OFFLINE CONTRACT E 节）。整理 LLM 偶尔给
# 「万能名」标题——KPLOG005 实证「我的故事」——列表里零可识别性（human：「标题叫啥，
# 我咋找不到呢」）。命中 -> _organize_and_archive 兜底为正文派生标题（story.py
# _fallback_title 单源）；未命中 -> 原样保留。is_generic_story_title 只消费本常量
# （临时换表判定随动 = 实现期可调，RDG-A1 STUCK_PHRASES 先例同款）。
GENERIC_STORY_TITLES = frozenset({
    "我的故事",
    "我的故事呀",
    "我的小故事",
    "我讲的故事",
    "一个故事",
    "小故事",
    "故事",
    "好听的故事",
    "今天的故事",
})
# 标题归一化的首尾剥离字符（标点/空白）：判「我的故事。」与「我的故事」同族。
_GENERIC_TITLE_STRIP_CHARS = "。．.！!？?，,、；;：:～~…· \t\r\n　"


def is_generic_story_title(title) -> bool:
    """敷衍标题判定纯函数（缺陷⑤b；单源 = GENERIC_STORY_TITLES）。

    非 str / 空白 -> False（安全拒判）；剔除首尾空白与尾随标点后**整串**命中词表 ->
    True（绝不子串——「风里的故事」这类具体题名不误杀）。判定只读模块常量，
    临时换表即随动。"""
    if not isinstance(title, str):
        return False
    candidate = title.strip().strip(_GENERIC_TITLE_STRIP_CHARS).strip()
    if not candidate:
        return False
    return candidate in GENERIC_STORY_TITLES


def _load_prompt_artifact(filename: str) -> str:
    """读 prompt 制品全文；缺失/不可读返回 ""（回合退化为无 system 指令，不炸 sweep）。"""
    try:
        return (PROMPTS_DIR / filename).read_text(encoding="utf-8")
    except OSError:
        log.warning("story_prompt_artifact_missing corr_id=story file=%s", filename)
        return ""


def _openclaw_chat_once(system_text: str, user_text: str, timeout_s: float = 10.0):
    """引导/整理回合的 OpenClaw OpenAI 兼容端点一次性调用（R1-F4 provider 出口 (a)：
    loopback 18789，出网沿契约 §1.2 路径②零新增）。

    request 级 system 消息承载制品指令（不走主 agent 共享 prompt）；payload 经
    openclaw_stream_request 单源构造（禁 max_tokens 硬约束继承）。返回助手文本
    （strip 后非空 str）或 None（空返回/异常——EQ-4 离线快失败：连接拒绝级异常直接
    归 None，sweep 不挂起）。urllib 惰性 import（离线装载纪律）。"""
    import urllib.error  # noqa: PLC0415 — lazy import (offline load discipline)
    import urllib.request  # noqa: PLC0415

    messages = []
    if system_text.strip():
        messages.append({"role": "system", "content": system_text})
    messages.append({"role": "user", "content": user_text})
    payload = gw_provider.openclaw_stream_request(messages)
    req = urllib.request.Request(
        gw_provider.OPENCLAW_BASE_URL.rstrip("/") + "/v1/chat/completions",
        data=json.dumps(payload, ensure_ascii=False).encode("utf-8"),
        headers={"Content-Type": "application/json",
                 "Authorization": "Bearer " + os.environ.get("OPENCLAW_TOKEN", "")},
        method="POST")
    try:
        with urllib.request.urlopen(req, timeout=timeout_s) as resp:
            body = resp.read().decode("utf-8", "replace")
    except (urllib.error.URLError, OSError, ValueError) as e:
        log.info("story_llm_round_unreachable corr_id=story err=%s", type(e).__name__)
        return None
    # stream:True（单源 payload）-> SSE data: 行；兼容非流 JSON 双形态。
    text_parts = []
    for line in body.splitlines():
        line = line.strip()
        if not line.startswith("data:"):
            continue
        chunk = line[len("data:"):].strip()
        if chunk in ("", "[DONE]"):
            continue
        try:
            obj = json.loads(chunk)
        except (TypeError, ValueError):
            continue
        for choice in (obj.get("choices") or []):
            delta = (choice.get("delta") or {}).get("content")
            if isinstance(delta, str):
                text_parts.append(delta)
            msg = (choice.get("message") or {}).get("content")
            if isinstance(msg, str):
                text_parts.append(msg)
    if not text_parts:
        # 非流式 JSON 兜底（端点可能忽略 stream）。
        try:
            obj = json.loads(body)
            for choice in (obj.get("choices") or []):
                msg = (choice.get("message") or {}).get("content")
                if isinstance(msg, str):
                    text_parts.append(msg)
        except (TypeError, ValueError):
            return None
    text = "".join(text_parts).strip()
    return text or None


async def _send_engine_leg_frames(upstream, frames) -> int:
    """引擎腿逐帧注入（kidpal_speak / kidpal_record_context 等内部指令同型通道）。

    单腿失败不外抛（引导注入不得拖垮泵/sweep），返回成功帧数供 K6 记账。"""
    sent = 0
    for frame in frames:
        try:
            await upstream.send(json.dumps(frame, ensure_ascii=False, default=str))
            sent += 1
        except Exception as e:  # noqa: BLE001 — 注入失败降级留痕，不连坐
            log.warning("story_engine_leg_inject_err err=%s", type(e).__name__)
    return sent


async def _speak_to_engine_legs(text: str, legs=None) -> int:
    """ADR-003 kidpal_speak 通道播报注入（reminders.build_speech_injection 先例复用；
    S-F1.4-AC6：注入不以提醒帧形态下发、不产生 alert 载荷——引导/整理播报与提醒
    弹窗链路分离）。空文本零注入。

    legs（additive，261021 F3⑤）：显式腿集（sweep 引导注入 = 会话归属连接腿）；
    缺省 None = 全局 _alert_legs（261019 现状零偏移）。"""
    return await _speak_to_legs(text, list(_alert_legs) if legs is None else legs)


async def _speak_to_legs(text: str, legs) -> int:
    """kidpal_speak 注入到指定腿集（_speak_to_engine_legs / 收尾安抚句共用单源）。

    引导回合走 _alert_legs（全局登记腿）；收尾安抚句走调用方腿快照（泵路径 = 本连接
    双腿、sweep 路径 = _alert_legs），两条通道同型、不产生提醒帧。空文本零注入。"""
    if not isinstance(text, str) or not text.strip():
        return 0
    injection = gw_reminders.build_speech_injection({"speech": text.strip()})
    if injection is None:
        return 0
    sent = 0
    for _device_ws, engine_ws in list(legs):
        sent += await _send_engine_leg_frames(engine_ws, [injection])
    return sent


def _organize_and_archive(raw, corr_id=None) -> "dict | None":
    """忠实整理回合 + 归档单源（三式收尾共用；S-F2.2-AC4 v1.65 / R1-N4）。

    孩子原话素材 -> 去掉**尾随**收尾词（story.py 既有 strip_trailing_finish_words，
    防收尾词进整理输入）-> 整理制品 + 一次性 LLM 回合 -> 标题 = 输出首行 strip 后
    ≤12 字、正文 = 其余行 -> archive_story（既有零改）。标题命中敷衍启发式
    （is_generic_story_title）-> 兜底为正文派生标题（缺陷⑤b）。

    任一失败（空素材 / LLM 不可达或空返回 / 空正文）返回 None = **当轮素材不归档**
    （R1-N4；调用方补固定安抚句），不退回原始转写、不写空故事。
    `_openclaw_chat_once` 经模块全局查找调用（冻结桩 monkeypatch front._openclaw_chat_once）。"""
    material = gw_story.strip_trailing_finish_words(raw or "").strip()
    if not material:
        return None
    prompt = _load_prompt_artifact(_ORGANIZE_PROMPT_FILE)
    reply = _openclaw_chat_once(
        prompt, f"孩子讲的内容：\n{material}\n\n请按指令把这段话忠实整理成一篇。",
        timeout_s=_ORGANIZE_LLM_TIMEOUT_S)
    if not reply:
        log.info("story_organize_round corr_id=%s ok=false reason=llm_unavailable",
                 corr_id)
        return None
    lines = [ln for ln in reply.splitlines() if ln.strip()]
    if not lines:
        return None
    title = lines[0].strip()[:12]
    body = "\n".join(lines[1:]).strip() or reply.strip()
    if is_generic_story_title(title):
        # 缺陷⑤b 兜底（CONTRACT E 节）：整理 LLM 起的「万能名」不成题名（KPLOG005 实证
        # 「我的故事」，列表里零可识别性 = human 找不到刚录篇目）。交给 archive_story 的
        # 内容派生标题单源（story._fallback_title：正文首个标点前片段 ≤12 字）——替换形态
        # 不另编内容；未命中即原样保留（≤12 字既有截断零偏移，faithful-organize 制品
        # 语义零触碰）。
        log.info("story_organize_round corr_id=%s generic_title=%r fallback=content", corr_id,
                 title)
        title = None
    try:
        record = gw_story.archive_story(STORIES_DIR, body, title=title)
    except ValueError:
        return None  # 空正文不归档（不写空故事）
    log.info("story_organize_round corr_id=%s ok=true id=%s", corr_id, record["id"])
    return record


def _organize_and_archive_offloop(raw, corr_id=None) -> "dict | None":
    """整理回合同步体的线程化执行壳（261020 D4-1）。

    病灶（真机 R3/R4 四轮收尾逐拍实证）：_openclaw_chat_once 是同步 urllib LLM 回合
    （~2-4s），在事件循环上内联执行把整个 front 冻结到回合完成——收尾沿先行下发的
    start_organizing 帧、期间到达的 stt 转发全部压到解冻后才冲刷，与 story_text 同拍
    到设备，整理过渡页（S-F2.2 AC4「演出盖住延迟」）实际从未可见（设备日志：每轮
    收尾 start_organizing 与 story_saved_stamp 同拍 10-20ms 到达）。移出事件循环后
    收尾沿下发即冲刷、循环在 LLM 回合期间保持响应。单飞锁还原 seq 账本「front
    单写者」串行语义（同步时代由事件循环天然承载）。"""
    with _ORGANIZE_ARCHIVE_LOCK:
        return _organize_and_archive(raw, corr_id)


def _get_story_acc():
    global _story_acc
    if _story_acc is None:
        _story_acc = gw_story.StoryAccumulator(STORIES_DIR)
    return _story_acc


def _story_route_display_payload(session_id, payload) -> bool:
    """旁路消费（不改 wire 输出）：control/start_recording -> 该会话切累积态。

    停录 -> 整段归档由上层（停录事件 / 真 ASR 上行殿后 9b）驱动
    _get_story_acc().stop(session_id, ...) -> archive_story 落盘 stories/<id>.md。

    返回 True = 本次消费是「进入记录语境」沿（routing 此前非 accumulate——收敛入口
    首次触发；261019-p3-record-dialogue-guide：调用方沿此注入 kidpal_record_context
    active=true 等价信号，重复触发幂等不重复注入）。"""
    if not session_id or not isinstance(payload, dict):
        return False
    data = payload.get("data")
    if (payload.get("view") == "control" and isinstance(data, dict)
            and data.get("action") == "start_recording"):
        acc = _get_story_acc()
        enter_edge = acc.routing(session_id) != "accumulate"
        acc.start(session_id)
        return enter_edge
    return False


def _story_guide_window_active(acc, session_id, now=None) -> bool:
    """引导播报窗口判定（缺陷④；FROZEN OFFLINE CONTRACT A/B 节）。

    窗口锚 = 该会话最近一次引导**播报注入**时刻（StoryAccumulator.guide_broadcast_anchor，
    被抑制轮无播报 = 不开窗）；`now`（= 调用方注入时刻）落在 [锚, 锚 +
    GUIDE_STT_GUARD_SECONDS) 内 -> True。时钟注入纪律：锚与 now 都不落墙钟（缺省 None
    = 墙钟，仅生产泵路径如此），否则离线快进不可测。从未播报引导的会话 -> False
    （不得全局误伤——FP-A3 反向 case②）。"""
    anchor = acc.guide_broadcast_anchor(session_id)
    if anchor is None:
        return False
    now_dt = now if now is not None else datetime.now(timezone.utc)
    try:
        elapsed = (now_dt - anchor).total_seconds()
    except TypeError:
        return False  # 不可比时刻（注入时钟类型失真）-> 不误判过滤
    if elapsed < 0:  # 时钟回拨/注入时刻早于锚：不当作窗口内（宁可放行素材）
        return False
    return elapsed < float(GUIDE_STT_GUARD_SECONDS)


def _story_feed_from_stt(session_id, obj, *, now=None) -> bool:
    """云侧累积接线 (包 9b HWV-A10 / story-p3 §9 F7 / 261001-m4-k3-story-archive-order)：
    引擎回带的 ASR 上行 stt 帧 (= 孩子逐句被识别的叙述) 在累积态喂进 StoryAccumulator.feed，
    供停录时整段归档。仅旁路消费（wire 转发不变，决策23）；非累积态 (普通闲聊) 不入库。

    真机序挂起态接通（261001）：routing == "pending_stop"（listen/state=stop 先到、accum 空）
    时，本条 stt（~1.7s 后到）即转写到达 -> feed 并返回 True，上层走忠实整理收尾单源
    `_story_finish_session`（S-F2.2-AC4 v1.65：归档正文 == 整理输出，不再原始转写直归档）；
    routing == "accumulate" -> 只 feed、返回 False；聊态 -> 无行为返回 False。

    引导播报窗口过滤（261022-p3-record-front-polish-fix 缺陷④；FROZEN CONTRACT B 节）：
    窗口内（`_story_guide_window_active`）到达的 stt 不喂素材缓冲、返回 False、不改累积
    缓冲——KPLOG005「Okay」幻听（human 实证非本人声音）直进素材即此缺口。**只滤素材面**：
    收尾词判定面（`_story_finish_on_phrase` -> story.is_finish_phrase）不经本函数，窗内
    孩子真说「说完了」收尾沿照常可达（FP-A1 本体）。now（additive keyword-only；缺省 None
    = 墙钟）供注入时钟快进（RDG-A5 self_push keyword-only 先例同缝），窗口过期判定与
    feed 的 last_activity 都消费注入 now，不落墙钟。"""
    if not session_id or not isinstance(obj, dict):
        return False
    if obj.get("type") != "stt":
        return False
    text = obj.get("text")
    if not isinstance(text, str) or not text.strip():
        return False
    acc = _get_story_acc()
    if _story_guide_window_active(acc, session_id, now):
        log.info("story_feed_guard corr_id=%s dropped=guide_broadcast_window", session_id)
        return False
    now_dt = now if now is not None else datetime.now(timezone.utc)
    routing = acc.routing(session_id)
    if routing == "pending_stop":
        acc.feed(session_id, text, now=now_dt)
        return True
    if routing == "accumulate":
        acc.feed(session_id, text, now=now_dt)
    return False


def _story_stop_on_listen_stop(session_id, obj) -> bool:
    """云侧停录归档接线 (包 9b HWV-A10/A12 / 261001-m4-k3-story-archive-order)：
    设备「聆听停止」上行帧 (listen/state=stop) 命中累积态时按下法处置：

    - 累积**非空** -> 返回 True，上层走忠实整理收尾单源 `_story_finish_session`
      (兼容 B2/B3 auto 模式：录音期间 stt 已喂入 accum；2026-10-20 三式收尾统一后
      **不再**原始转写直归档——S-F2.2-AC4 v1.65)；
    - 累积**空** -> 真机序（manual 模式 ASR 由 listen stop 触发，转写 ~1.7s 后才经下行到达）
      -> 挂起 pending-stop（acc.mark_pending，routing="pending_stop" != "chat"），返回 False；
      下一条 stt 到达时由 _story_feed_from_stt 接通收尾（SAO-A1/A2）。
    非累积态 / 已挂起 / 累积为空且已 pending -> False（不编造归档、不写空故事）。"""
    if not session_id or not isinstance(obj, dict):
        return False
    if not (obj.get("type") == "listen" and obj.get("state") == "stop"):
        return False
    acc = _get_story_acc()
    if acc.routing(session_id) != "accumulate":
        return False
    if not acc.has_accum(session_id):
        # 累积空 -> 真机序：转写尚未到达，挂起 pending-stop 等它（旧实现此处立即 stop()
        # 返 None 并把会话打回 chat -> 转写被丢弃 = 本缺陷，SAO-A2 冻结 oracle）。
        acc.mark_pending(session_id)
        log.info("story_stop corr_id=%s archived=false reason=pending_stop", session_id)
        return False
    return True


async def _push_display_payload(websocket, payload: dict) -> None:
    """front 机械自推单帧（不经 relay；Q7 天然豁免——自推不属引擎侧来源面）。

    把 payload 以设备 wire 形状的 kidpal/display mcp 帧下发恰一帧；过 display_schema
    机械门（自推通路 self_push=True——结构性风险 1 分叉：start_organizing 机械下发
    单源放行、end_session 即便自推仍拒；畸形不推记日志、零帧、不裸送）。"""
    ok, reason = gw_display.validate_display(payload, self_push=True)
    if not ok:
        log.warning("story_display_push skipped view=%s reason=%s",
                    (payload or {}).get("view"), reason)
        return
    envelope = gw_mcp.display_tool_call(_display_wire_arguments(payload))
    await websocket.send_text(
        json.dumps({"type": "mcp", "payload": envelope}, ensure_ascii=False, default=str))


# ECS 故事列表恢复性推送开关（ADR-010 / architecture-gate-evidence C1：部署面默认关、
# 固件烧板验证（SL-A5）后 human 人工开闸）。schedule grid_restore_enabled 先例形态
# （env 名 + strip().lower() 识别集）；差异仅在缺省极性 = 代码默认**开**（SL-A1 建连
# 恢复推送齿无 env 驱动形态、须默认可驱动）——部署面默认关由 ECS 配置层显式置
# KIDPAL_STORY_RESTORE_PUSH=0/false/off/no 承载（「不得为过齿改部署默认开」= 不动
# ECS 部署配置）。开闸留痕/部署文档按本 env 名单源。
STORY_RESTORE_ENV = "KIDPAL_STORY_RESTORE_PUSH"
# story restore 开关识别集（strip().lower() ∈ 本集 -> 关；其余一律开——代码默认开偏置）。
_STORY_RESTORE_FALSE_SET = {"0", "false", "off", "no"}


def story_restore_enabled(raw: "str | None" = None) -> bool:
    """故事列表建连恢复推送开关决策面（纯函数；default None = 读 os.environ）。

    代码默认开：env unset -> True（SL-A1 建连恢复推送齿可驱动，无 env 驱动形态）；
    strip().lower() ∈ {"0","false","off","no"} -> False（部署侧 ECS 显式关，C1
    closeout：开关默认关交付、烧板验证后人工开闸）。与 schedule grid_restore_enabled
    同形（env 名常量 + 纯函数 + strip().lower() 识别集），差异仅在缺省极性。
    """
    if raw is None:
        raw = os.environ.get(STORY_RESTORE_ENV)
    if not isinstance(raw, str):
        return True
    return raw.strip().lower() not in _STORY_RESTORE_FALSE_SET


async def _push_story_list(websocket, restore: bool = False) -> None:
    """front 机械推送 story_list 的下行缝（三时点：WS 建连恢复 / 归档成功后 / 删除执行后）。

    把 list_stories(<STORIES_DIR>) 以 kidpal/display mcp 帧下发**恰一帧**（设备 wire
    形状 {"view":<str>,"data":<json 串>}）。计数/窗口语义由 story.py producer 单源承载
    （total=现存篇数 + items 最新 ≤16 窗口），本缝只做接线、不重算。front 自推不经 relay
    （Q7 天然豁免——直调 _push_display_payload，与引擎侧 build_display_tool_calls 无关）。

    restore（契约 §1.3③ D1 生产纪律）：仅 WS 建连恢复推送路径传 restore=True——下行
    data 带 `restore: true`（板端 restore 分流载体：只填 RAM 列表缓存、不接管前台、P3
    在屏仅重绘本页）；②归档后（:573 一带）/③删除后（:223）的主动推送缺省 restore=False
    ——下行 data **不带** restore 键（契约缺省语义：absent 或显式 false 设备行为完全
    一致，主动展示接管前台既有行为零偏移）。"""
    payload = gw_story.list_stories(STORIES_DIR)
    if restore:
        payload["data"]["restore"] = True
    await _push_display_payload(websocket, payload)


async def _consume_kidpal_control(session_id, obj, websocket, upstream=None) -> bool:
    """拦截设备→引擎的 KidPal 私有控制帧（契约 §四：网关消费并拦截、不下透引擎）。

    - kidpal_session_ctx：整条消费（无论取值）；source_page=="story" 时把该会话切累积态
      （收敛 _get_story_acc().start 同一幂等入口——与 §1.4 control/start_recording 同入口，
      重复触发不重复开会话）；其余取值现版本无行为（闭集预留）。
      三时点①恢复重裁（TaskPackage 260924-m4-k3-record-session-live / plan T5）：
      routing 沿触发条件推送——本帧 ctx 把会话从 chat 切入累积态（acc.start 之前自查
      routing != "accumulate"，story.py 零改动）才推恰一帧 story_list；重复 ctx(story)
      （已 accumulate）零新增帧（契约 §四「重复发送安全」）；推送失败不阻断 ctx 消费
      （ctx 整条拦截语义优先, 推送为增强, K1 建连推送 try/except 同款先例）。
      Package B 等价信号注入（261019-p3-record-dialogue-guide）：进入记录语境沿
      （ctx_push_edge == True）同拍向**引擎腿**（additive 形参 upstream，缺省 None =
      直调/离线装载零注入）注入 kidpal_record_context active=true——幂等（已
      accumulate 的重复 ctx 不重复注入；注入失败不阻断拦截消费）。
    - kidpal_story_delete：整条消费；机械执行 delete_story（哨兵头校验 + MD 硬删 + 拒绝
      审计）→ 重推 story_list 对账（隐式 ack：不存在 id 亦重推、total 不变；合法删除
      total −1 可见）。畸形/非法 id 由 delete_story 内部拒绝、不崩、审计留痕。
    - kidpal_story_fetch（TaskPackage 261016-p3-story-body-sd-cache; 契约 §四）：
      整条消费并拦截；逐 id 校验存在于 story 账本（哨兵头 + 文件存在）→ 读 MD → 以
      **既有 story_text 帧形状**逐篇应答（每篇一次、零新字段；顺序 == ids 序）。
      非法 / 不存在 id 由 fetch_story 内部拒绝 + 审计、不回帧（混合请求逐条跳过——
      契约「逐 id 校验 + 逐篇应答」直读）；空 ids / 畸形载荷 = no-op（零帧、零下透）。
      纯只读：不重归档、不动 seq 账本、不推 story_list（契约 §1.3④ fetch 应答不触发
      盖章 / 无归档侧效应）。
    - kidpal_story_discard（TaskPackage 261021-p3-record-cancel-lifecycle F2; 契约 §四
      kidpal_story_discard 2026-10-21 LANDED——本件只消费冻结契约、零契约改动）：
      整条消费并拦截、不下透引擎；**连接权威**——按本连接 session_id 取消
      （acc.cancel(reason="discard")，载荷 obj["session_id"] 仅作审计参考、绝不作会话键,
      防跨会话伪造取消）；累积缓冲销毁、不归档、不推 story_list；幂等 no-op（会话已
      释放/非累积态/未知会话 cancel 返 False 即空转）；审计日志一行（logger kidpal-front
      含冻结 token story_discard + 本连接 session_id）。
    返回 True = 已消费（调用方不得转发引擎）。"""
    if not isinstance(obj, dict):
        return False
    mtype = obj.get("type")
    if mtype == "kidpal_session_ctx":
        if session_id and obj.get("source_page") == "story":
            acc = _get_story_acc()
            # routing 沿自查在 acc.start 之前（routing() 查询不改状态, K1 冻结;
            # story.py 零改动）：chat -> accumulate 沿 = 首帧 ctx, 才条件推送①。
            ctx_push_edge = acc.routing(session_id) != "accumulate"
            acc.start(session_id)
            if ctx_push_edge and upstream is not None:
                # ADR-013 Package B：进语境等价信号注入引擎腿（幂等沿；失败不阻断拦截消费）。
                try:
                    await _send_engine_leg_frames(upstream, [
                        {"type": RECORD_CTX_SIGNAL_TYPE, "active": True}])
                except Exception as e:  # noqa: BLE001 — 注入失败不阻断 ctx 拦截语义
                    log.warning("story_record_ctx_inject_err err=%s", type(e).__name__)
            if ctx_push_edge:
                try:
                    await _push_story_list(websocket)  # ①恢复 = 引导态会话建立同拍
                except Exception:  # noqa: BLE001 — 推送失败不阻断 ctx 拦截消费
                    log.warning("story_ctx_push failed session_id=%s (ctx 拦截语义优先)",
                                session_id)
        return True
    if mtype == "kidpal_story_delete":
        gw_story.delete_story(STORIES_DIR, obj.get("id"), corr_id=session_id)
        await _push_story_list(websocket)  # 删除执行后重推对账（三时点③）
        return True
    if mtype == "kidpal_story_fetch":
        ids = obj.get("ids")
        if not isinstance(ids, list):
            # 畸形载荷（缺 ids / ids 非数组 / null）：no-op + 审计；整条拦截不下透。
            log.warning("story_fetch rejected corr_id=%s id=<none> reason=invalid_ids",
                        session_id)
            return True
        for raw in ids:
            if not isinstance(raw, str):
                # 元素非字符串：逐条跳过（契约「逐 id 校验」；不崩、不回帧）。
                log.warning("story_fetch rejected corr_id=%s id=%r reason=invalid_type",
                            session_id, raw)
                continue
            rec = gw_story.fetch_story(STORIES_DIR, raw, corr_id=session_id)
            if rec is None:
                continue  # 拒绝审计已在 fetch_story 内留痕；不回帧
            await _push_display_payload(websocket, gw_story.story_text(rec))
        return True
    if mtype == "kidpal_story_discard":
        # F2 显式丢弃通知（契约 §四）：会话绑定以**连接权威**——按本连接 session_id
        # 取消，载荷 session_id 只进审计、绝不作取消键（防跨会话伪造取消）。
        payload_sid = obj.get("session_id")  # 审计参考，不信任为会话键
        dropped = False
        try:
            dropped = _get_story_acc().cancel(session_id, reason="discard")
        except Exception as e:  # noqa: BLE001 — 丢弃消费绝不外抛（幂等 no-op 语义）
            log.warning("story_discard corr_id=%s err=%s", session_id, type(e).__name__)
        # 审计一行（冻结 token story_discard + 本连接 session_id；载荷键仅参照）。
        log.info("story_discard corr_id=%s consumed=true cancelled=%s payload_session_id=%s",
                 session_id, dropped, payload_sid)
        return True
    return False


def _story_finish_on_phrase(session_id, obj) -> bool:
    """累积态 stt 命中收尾词 -> 收尾沿（S-F2.2 收尾；M4K1-A5）。

    仅累积态消费（问答会话零影响）；命中 -> 返回 True，上层走忠实整理收尾单源
    `_story_finish_session`（S-F2.2-AC4 v1.65：三式收尾均走整理回合成篇）；
    未命中 / 非累积态 -> False。触发句本身不入素材由整理输入装配前剔尾随收尾词承载
    （逐句 feed 语义不动）。"""
    if not session_id or not isinstance(obj, dict):
        return False
    if obj.get("type") != "stt":
        return False
    text = obj.get("text")
    if not isinstance(text, str) or not text.strip():
        return False
    if not gw_story.is_finish_phrase(text):
        return False
    acc = _get_story_acc()
    return acc.routing(session_id) == "accumulate"


# 重入守卫（进程级 in-flight 标志；261022-p3-record-front-polish-fix）：独立节拍缩短到
# 4s 后，引导/整理回合的 LLM 在途（asyncio.to_thread 6~7s）期间下一拍会重入 -> 同会话
# 双引导轮。不用 asyncio.Lock：冻结齿逐次 asyncio.run 直调本函数（同一 Lock 跨事件循环
# 复用会绑死首个 loop 并抛 RuntimeError），布尔标志等价且跨循环安全。sweep 任务无 stop
# 语义（与 _alert_loop_task 对齐、从不 cancel），不存在「取消后 to_thread 仍在跑」窗口。
_story_sweep_inflight = False


async def _story_timeout_sweep(now=None) -> int:
    """卡壳计时 sweep 单源入口（签名 (now=None) 不变；语义说明见
    `_story_timeout_sweep_body`）。

    重入守卫：上一拍仍在途 -> 本拍跳过（skip，返回 0），锁在 finally 释放。守卫只拦
    并发重入，不触单轮内路径——「空/异常 = 本轮不播报仍计轮」（q6/E3）语义零偏移。"""
    global _story_sweep_inflight
    if _story_sweep_inflight:
        log.debug("story_timeout_sweep skipped=inflight")
        return 0
    _story_sweep_inflight = True
    try:
        return await _story_timeout_sweep_body(now)
    finally:
        _story_sweep_inflight = False


async def _story_timeout_sweep_body(now=None) -> int:
    """卡壳计时 sweep（261019-p3-record-dialogue-guide 重写；签名 (now=None) 不变）+
    pending-stop 10s 挂起窗口保留（SAO 家族齿 must-stay-green）。

    每 tick 先巡检 pending-stop 挂起超时：纯决策 pending_timed_out_sessions 取挂起
    >=10s（PENDING_STOP_SECONDS）的会话，release_pending 释放回闲聊态（不写空故事）。
    再走卡壳引导/自动收尾（120s 纯静默语义已退役——requirements v1.65 S-F2.2-AC5）：

      ② stuck-due（stuck_due_sessions 命中）会话执行引导回合：引导制品 prompt →
         _openclaw_chat_once（R1-F4 provider 出口，loopback 18789）→ 有文本 =
         kidpal_speak 注入播报（ADR-003 通道，不以提醒帧形态下发；播报文本受
         GUIDE_SPEECH_MAX_CHARS 篇幅上限夹紧）；**返回空/异常 = 本轮不播报仍计轮**
         （mark_guide_round，q6/E3 收声不停摆）。引导轮不归档、不下发 start_organizing
         （引导不是收尾）。末段引导轮（GUIDE_ROUND_LIMIT）已发的会话不再发第 3 轮。
      ③ auto-finish（auto_finish_due_sessions 命中，先于引导判定——第 2 轮计时到仍
         无回应即收尾，不再发第 3 轮引导）→ 三式收尾统一下发：control/start_organizing
         恰一帧（重复幂等——收尾后 routing 回 chat，重复 sweep 不再命中）+ 出语境信号
         active=false + 走整理回合。计时阈值 = STUCK_GUIDE_SECONDS +
         GUIDE_RESPONSE_WINDOW_SECONDS（末段引导后的应答窗：窗内留给 ASR 收尾词链路，
         窗耗尽才自动收尾——缺陷③「说完了」孤儿帧竞态）。
      ④ 整理回合：一次性、忠实指令（整理制品）、标题 ≤12 字同回合产出 → archive_story
         （既有零改）→ story_text(seq) + story_list 三时点②；失败 = 当轮素材不归档 +
         固定安抚句 kidpal_speak（R1-N4，不发明新语义）。

    sweep 以注入 now 驱动全部纯决策与轮次记账（stuck_due_sessions /
    auto_finish_due_sessions / mark_guide_round 均传该 now——时钟注入纪律，不落墙钟；
    否则离线快进不可测）。返回自动收尾会话数（K6 归因）。"""
    acc = _get_story_acc()
    now_dt = now if now is not None else datetime.now(timezone.utc)
    # pending-stop 挂起超时（10s 无转写）-> 释放挂起态、不写空故事（SAO-A3②）
    for sid in acc.pending_timed_out_sessions(now_dt):
        if acc.release_pending(sid):
            log.info("story_pending_timeout corr_id=%s released=true", sid)
    finished = 0
    # ③ 自动收尾先于引导判定：第 2 轮计时到仍无回应即收尾（不再发第 3 轮引导）。
    # 应答窗（261022-p3-record-front-polish-fix 缺陷③；FROZEN CONTRACT A/C 节）：自动
    # 收尾阈值 = 旧触发点（STUCK_GUIDE_SECONDS）**叠加**应答窗——末段引导后先留
    # GUIDE_RESPONSE_WINDOW_SECONDS 给 ASR 收尾词链路（实测 5~14s 恒迟到），窗内「说完了」
    # 经 is_finish_phrase 走正常收尾（不成孤儿帧），窗耗尽后自动收尾照常（恰一帧 + 幂等
    # 既有语义零偏移）。常量现读模块全局（monkeypatch 可调，FP-X2 单源）。
    finish_timeout_s = gw_story.STUCK_GUIDE_SECONDS + GUIDE_RESPONSE_WINDOW_SECONDS
    for sid in acc.auto_finish_due_sessions(now_dt, timeout_s=finish_timeout_s):
        await _story_auto_finish(sid, now_dt)
        finished += 1
    # ② 卡壳引导回合（stuck-due 命中；LLM 失败/空 = 抑制计轮仍走，收声不停摆）。
    # 末段引导轮（GUIDE_ROUND_LIMIT）已发的会话不再发第 3 轮——那段时长正是应答窗，
    # 播放第三段引导会把孩子正说的收尾话盖过去（缺陷③本体）；等 ③ 的自动收尾接位。
    for sid in acc.stuck_due_sessions(now_dt):
        if acc.guide_rounds(sid) >= gw_story.GUIDE_ROUND_LIMIT:
            continue
        await _story_guide_round(sid, now_dt)
    return finished


async def _story_guide_round(sid, now_dt) -> None:
    """卡壳引导回合（S-F2.2-AC2）：引导制品指令 + 已讲内容 -> 引导 LLM -> 有文本 =
    kidpal_speak 播报注入；空/异常 = 本轮不播报仍计轮（q6/E3）。引导轮不归档、
    不下发 start_organizing（引导不是收尾）。mark_guide_round 恒传注入 now（时钟
    注入纪律，不落墙钟）。

    在途双点存续守卫（261021-p3-record-cancel-lifecycle F3①②，[SC-B]）：
      ① 入口守卫：build_organizing_input 返 None（会话已释放 / 累积为空）-> 直接返回，
         零 LLM、零播报、零计轮、零会话壳（mark_guide_round 内部 _ensure 会重建会话壳,
         连 mark 都不得触达）；
      ② 播报前复查：LLM 在途期间 discard / 断连 teardown / 收尾词路径可能已 cancel 该
         会话 -> routing 不再 "accumulate" 则丢弃产出（不播报、不计轮、不留壳）。"""
    acc = _get_story_acc()
    spoken = acc.build_organizing_input(sid)
    if spoken is None:
        log.info("story_guide_round corr_id=%s skipped=no_session", sid)
        return
    prompt = _load_prompt_artifact(_GUIDE_PROMPT_FILE)
    # 261020 D4-1 同族：引导回合的同步 LLM 调用同样移出事件循环（sweep 路径上一轮
    # 冻结会吃掉下一轮收尾沿的及时性，与整理回合同根因）。
    reply = await asyncio.to_thread(
        _openclaw_chat_once,
        prompt,
        f"孩子到目前为止讲的内容：\n{spoken}\n\n孩子停顿了，请按指令给一句轻声引导。")
    if acc.routing(sid) != "accumulate":
        # 在途期间会话已释放（丢弃 / 断连 / 收尾）——产出丢弃是正确行为（F3④）。
        log.info("story_guide_round corr_id=%s dropped=session_released", sid)
        return
    if reply:
        # 腿集会话归口（[SC-D]）：只打该会话归属连接的引擎腿；映射缺席（无登记连接,
        # 离线/既有直调形态）= 退回全局腿，与 261019 现状等价。
        # 单段引导播报篇幅上限（261022-p3-record-front-polish-fix 缺陷③；CONTRACT A 节）：
        # 超限按 GUIDE_SPEECH_MAX_CHARS 截断——截断产物恒为原文前缀（不另编内容），
        # 上限内原样保真；常量现读模块全局（monkeypatch 可调）。
        spoken_text = reply[:GUIDE_SPEECH_MAX_CHARS]
        await _speak_to_engine_legs(spoken_text, _session_legs(sid) or list(_alert_legs))
        log.info("story_guide_round corr_id=%s spoken=yes rounds=%d chars=%d%s",
                 sid, acc.guide_rounds(sid) + 1, len(spoken_text),
                 "" if spoken_text == reply else " truncated=true")
    else:
        log.info("story_guide_round corr_id=%s spoken=no (suppressed, still counts)",
                 sid)
    acc.mark_guide_round(sid, now=now_dt, suppressed=not reply)


async def _story_auto_finish(sid, now_dt) -> None:
    """自动收尾（S-F2.2-AC5）：委托三式收尾单源 `_story_finish_session`。

    腿集 = **该会话归属连接**的腿（`_session_legs(sid)`，[SC-D] 会话归口；261021 F3⑤
    ——zombie 会话的收尾帧族不得打进新连接）。映射缺席（无登记连接 = 离线/既有直调
    形态）退回 `_alert_legs` 当前在线腿快照，与 261019 现状等价。start_organizing 恰
    一帧 + 出语境信号 -> 素材装配 -> 会话释放 -> 整理回合归档 -> story_text + story_list
    三时点②；失败 = 当轮素材不归档 + 固定安抚句 kidpal_speak（R1-N4）。重复幂等由
    会话状态沿承载（收尾后会话已释放，重复 sweep 不再命中）。now_dt 保留签名兼容
    （sweep 时钟注入纪律；本函数不另取墙钟）。"""
    await _story_finish_session(sid, _get_story_acc(),
                                _session_legs(sid) or list(_alert_legs))


def _bearer(header: Optional[str]) -> str:
    """Pick the raw Authorization value (or ''), ready for constant-time compare."""
    return header or ""


def _require_device(authorization: Optional[str]) -> tuple[bool, int]:
    """Mechanical forced-auth gate (mechanical short-circuit like ARQ-260902 lesson)."""
    return gw_auth.authorize(DEVICE_TOKEN, _bearer(authorization))


def _unauthorized() -> JSONResponse:
    return JSONResponse(status_code=401, content={
        "error": {"category": "auth", "fingerprint": "front.bad-device-token",
                  "failingStep": "require_device", "message": "invalid/missing device bearer token"}})


@app.get("/healthz")
async def healthz():
    return {"status": "ok", "gateway_modules": [
        gw_auth.TOKEN_MIN_CHARS, bool(gw_provider.FEATURE_STREAMING_FORBIDS_MAX_TOKENS),
        sorted(gw_display.VALID_VIEWS), gw_fallback.SOOTHING_TIMEOUT_MS, gw_mcp.TOOL_DISPLAY]}


@app.post("/command")
async def command(request: Request, authorization: Optional[str] = Header(default=None)):
    """Boundary: Bearer auth then empty-text -> 400 (never silently 200). Non-empty -> ok."""
    ok, status = _require_device(authorization)
    if not ok:
        return _unauthorized()
    try:
        body = await request.json()
    except Exception:  # noqa: BLE001
        return JSONResponse(status_code=400, content={"error": {"message": "body must be JSON"}})
    text = (body.get("text") or "").strip() if isinstance(body, dict) else ""
    if not text:
        # same empty-input boundary as the ws/shim path: reject 400.
        return JSONResponse(status_code=400, content={"error": {
            "category": "validation", "fingerprint": "front.empty-text",
            "failingStep": "command.validate", "message": "empty text rejected"}})
    # non-empty: assemble a kidpal/display mcp relay as a minimal adaptation smoke.
    display = {"view": "text", "data": {"text": text}}
    ok_disp, reason = gw_display.validate_display(display)
    envelope = gw_mcp.display_tool_call(display) if ok_disp else {"error": "invalid-display:" + reason,
                                                                  "fallback": gw_fallback.ERROR_FALLBACK}
    return JSONResponse(status_code=202, content={"accepted": True, "mcp": envelope,
                                                  "correlation_id": gw_provider.corr_id()})


@app.post("/adaptation/dryrun")
async def adaptation_dryrun(request: Request, authorization: Optional[str] = Header(default=None)):
    """Probe the gateway adaptation assembly (validate_display + truncate + relay)."""
    ok, status = _require_device(authorization)
    if not ok:
        return _unauthorized()
    try:
        payload = await request.json()
    except Exception:  # noqa: BLE001
        return JSONResponse(status_code=400, content={"error": {"message": "body must be JSON"}})
    view = (payload or {}).get("view", "text") if isinstance(payload, dict) else "text"
    data = (payload or {}).get("data", {"text": ""}) if isinstance(payload, dict) else {"text": ""}
    ok_disp, reason = gw_display.validate_display({"view": view, "data": data})
    truncated = gw_fallback.truncate_for_display((data.get("text") or ""))
    return JSONResponse(content={
        "slide": ok_disp, "reason": reason,
        "v32_truncated": truncated,
        "soothing_timeout_ms": gw_fallback.SOOTHING_TIMEOUT_MS,
        "relay": gw_mcp.display_tool_call({"view": "text", "data": {"text": truncated}}) if ok_disp else None})


# --- ws-proxy text takeover (TaskPackage 260907-g-relay-fix / 契约 §1.5 step 3) -----
# FROZEN OFFLINE CONTRACT — see server/acceptance/test_g_relay_fix.acceptance.py. These
# four callables + the wired pumps in ws_gateway below are the defect body fix: the real
# proxy path now takes over the engine text stream (the /command & /dryrun demo paths were
# never the defect). 决策23: 设备 JSON-RPC 回执 = 送达确认, front 只消费不重发.

# Fence marker as produced by OpenClaw (contract §1.4). The engine's sentence splitter can
# strip the newline after the opener (live session 95ccf407), so the opener is matched in
# BOTH canonical (```display \n <json> \n ```) and inline (```display<json>```) forms.
_DISPLAY_FENCE_RE = re.compile(r"```display(.*?)```", flags=re.DOTALL)


def strip_display_fences(text: str) -> tuple[str, "list[str]"]:
    """Pure syntactic split of engine LLM text (contract §1.5 step 3 is the gateway's job).

    Returns (speech_text, display_blocks). Every ```display fenced block is removed from
    the speech track with ZERO residue (no ``` and no fence-body chars leak into speech),
    and the raw fence bodies are returned verbatim in order as a list of str — NOT parsed,
    so dirty bodies are still handed to the downstream mechanical check (display_schema) to
    reject. Text outside fences is preserved verbatim, in order. No fence -> (text, []).
    """
    blocks = [m.group(1) for m in _DISPLAY_FENCE_RE.finditer(text)]
    speech = _DISPLAY_FENCE_RE.sub("", text)
    return speech, blocks


def _friendly_downgrade_view(_reason: str) -> dict:
    """A non-empty, schema-legal friendly text view to downgrade a dirty/supersized display
    block to — never the raw dirty payload (contract §1.5: 脏 JSON 拒绝并降级友好提示; V27)."""
    return {"view": "text", "data": {"text": "这张小卡片没显示好，先专心听我说话吧。"}}


def _display_wire_arguments(payload: dict) -> dict:
    """The device wire shape for kidpal/display arguments (firmware mcp_server.cc:105 — BOTH
    the view and data Properties are String-typed; the device parses the data string itself,
    an object-typed data is rejected)."""
    return {"view": payload.get("view"), "data": json.dumps(payload.get("data"), ensure_ascii=False)}


def build_display_tool_calls(blocks: list) -> list:
    """One kidpal/display JSON-RPC tools/call envelope per block, in order, NEVER silently
    dropped (len(result) == len(blocks)).

    A block that parses as JSON and passes display_schema.validate_display is relayed with
    arguments at the DEVICE wire shape {"view": <str>, "data": <json.dumps(data) STRING>}
    (data round-trips via json.loads(arguments["data"]) == the validated payload's data).
    Anything else (dirty / schema reject) is DOWNGRADED, not silent: a friendly text view
    that itself passes validate_display with non-empty text, never the raw dirty payload.
    """
    calls = []
    for block in blocks:
        payload = None
        if isinstance(block, str):
            try:
                payload = json.loads(block)
            except (TypeError, ValueError):
                payload = None
        # 「帮我显示课表」意图薄载荷截获展开（M3K3-A2）：LLM 只发 {view:schedule,
        # data:{}}，在 validate_display/下发之前被替换为新鲜周网格（单源复用
        # schedule.expand_schedule_intent_payload -> week_grid_payload）。恰薄载荷
        # 才截获；读不到 schedule.md / 其它形状 -> _expand_schedule_intent 返回
        # None，原样走既有 relay 链交 display_schema 机械门裁决（Q5）。
        if payload is not None:
            expanded = _expand_schedule_intent(payload)
            if expanded is not None:
                payload = expanded
        if payload is not None:
            ok, reason = gw_display.validate_display(payload)
        else:
            ok, reason = False, "display block is not JSON-object parseable"
        if ok:
            # Q7 收紧（story 单源）：引擎侧来源的 story_list/story_text 块降级为友好 text
            # （与脏块降级同语义），绝不把 story 载荷透传给设备；其它 view 照既有语义中继。
            if payload.get("view") in _STORY_SINGLE_SOURCE_VIEWS:
                display = _display_wire_arguments(
                    _friendly_downgrade_view("story_single_source"))
            else:
                display = _display_wire_arguments(payload)
        else:
            display = _display_wire_arguments(_friendly_downgrade_view(reason))
        calls.append(gw_mcp.display_tool_call(display))
    return calls


async def _relay_display_blocks_to_device(blocks: list, websocket, session_id=None) -> bool:
    """Send each display relay envelope toward the device as its OWN
    {"type":"mcp","payload":<envelope>} frame (firmware application.cc:492 routes ONLY
    type=mcp into McpServer::ParseMessage; :532 drops every other type — the deprecated
    display_relay wrapper ended exactly there).  多块多帧, no batching.

    session_id (additive, P3-A4): 若给定，旁路消费 control/start_recording 把该会话切
    累积态（契约 §1.4）；wire 输出不变（决策23: front 只消费不重发）。
    返回 True = 本批块中有「进入记录语境」沿（control/start_recording 首次消费，
    261019-p3-record-dialogue-guide：调用方沿此向引擎腿注入 active=true 等价信号）。"""
    enter_edge = False
    for block in blocks:
        if session_id is not None:
            try:
                if _story_route_display_payload(
                        session_id, json.loads(block) if isinstance(block, str) else block):
                    enter_edge = True
            except (TypeError, ValueError):
                pass  # 脏块仍照常经 build_display_tool_calls 降级，旁路消费忽略
        envelope = build_display_tool_calls([block])[0]
        await websocket.send_text(
            json.dumps({"type": "mcp", "payload": envelope},
                       ensure_ascii=False, default=str)
        )
    return enter_edge


async def pump_upstream_to_client(upstream, websocket, session_id=None) -> None:
    """Engine->device leg of the ws proxy WITH takeover (this is the defect body).

    upstream is an async-iterable of frames (str|bytes, websockets-client shape); the device
    websocket has send_text()/send_bytes(). Binary (audio) frames forward unchanged. Text
    frames that are JSON objects carrying a string "text" field (the llm/tts 文字同步 stream)
    are fence-stripped via strip_display_fences before forwarding, and each extracted display
    block is relayed toward the device as a kidpal/display tools/call envelope from
    build_display_tool_calls (a small wrapper text frame makes the envelopes discoverable).
    Frames without a text field (hello, state, ...) forward verbatim.

    session_id 由 ws_gateway 生成（每连接一个不可复用的 uuid 键；P3-A4 累积态路由 + 包 9b
    F6 弃用 websocket 对象地址（CPython id）复用），两条腿共享同一键以贯穿 start/feed/stop/cancel。
    默认 None = 无连接会话（旁路故事 feed/stop 接线不触发），保 g-relay-fix 冻结的 2-arg 调用契约不破。

    Package B 等价信号注入（261019-p3-record-dialogue-guide，ADR-013）：
      进语境 = control/start_recording 消费沿（side-channel / fence 两 relay 路径收敛）
      同拍向**引擎腿**注入 kidpal_record_context active=true（幂等沿）；
      出语境 = 三式收尾（收尾词归档 / pending 真机序归档同拍）向引擎腿注入 active=false
      + 设备腿统一下发 control/start_organizing 恰一帧（q2 = ⓒ；自推通路 self_push=True）。
    """
    try:
        async for frame in upstream:
            if isinstance(frame, bytes):
                await websocket.send_bytes(frame)
                continue
            try:
                obj = json.loads(frame)
            except (TypeError, ValueError):
                await websocket.send_text(frame)  # non-JSON text: forward verbatim
                continue
            if not isinstance(obj, dict):
                await websocket.send_text(frame)  # non-object JSON: forward verbatim
                continue
            # 云侧累积接线 (包 9b HWV-A10 / 261001)：ASR 上行 stt 帧命中累积态则喂缓冲
            # （旁路消费，转发不变，决策23）；pending-stop 态（listen/state=stop 先到、转写后到
            # 的真机序 SAO-A1）的转写到达 = 收尾沿 -> 忠实整理收尾单源（S-F2.2-AC4 v1.65）。
            if _story_feed_from_stt(session_id, obj):
                await _story_finish_session(session_id, _get_story_acc(),
                                            [(websocket, upstream)])
            # 累积态收尾词收尾（S-F2.2 收尾）：stt 命中「我说完了/讲完啦/说完了」-> 同一
            # 整理收尾单源成篇（三式收尾统一）；问答会话零影响（检测仅累积态消费）。
            if _story_finish_on_phrase(session_id, obj):
                await _story_finish_session(session_id, _get_story_acc(),
                                            [(websocket, upstream)])
            if obj.get("type") == SIDE_CHANNEL_TYPE:
                # engine-internal display-blocks side channel: INTERCEPT (never forwarded
                # verbatim — the device must not see the engine transport frame), drive the
                # raw blocks through the same relay chain.
                if await _relay_display_blocks_to_device(obj.get("blocks") or [], websocket,
                                                         session_id):
                    await _inject_record_context_enter(upstream)
                continue
            text_field = obj.get("text") if isinstance(obj, dict) else None
            if isinstance(text_field, str):
                speech, blocks = strip_display_fences(text_field)
                if not blocks:
                    await websocket.send_text(frame)  # no display fence: verbatim
                    continue
                # forward the fence-stripped speech text on the original carrier frame…
                relay = dict(obj)
                relay["text"] = speech
                await websocket.send_text(json.dumps(relay, ensure_ascii=False))
                # …and each display envelope to the device as its own mcp frame.
                if await _relay_display_blocks_to_device(blocks, websocket, session_id):
                    await _inject_record_context_enter(upstream)
            else:
                await websocket.send_text(frame)  # no text field (hello/state): verbatim
    except Exception:  # noqa: BLE001 — proxy teardown ends the pump gracefully.
        pass


async def _inject_record_context_enter(upstream) -> None:
    """进语境等价信号注入引擎腿（active=true；幂等沿由消费侧保证，注入失败降级留痕）。"""
    try:
        await _send_engine_leg_frames(
            upstream, [{"type": RECORD_CTX_SIGNAL_TYPE, "active": True}])
    except Exception as e:  # noqa: BLE001 — 注入失败不阻断泵转发
        log.warning("story_record_ctx_inject_err err=%s", type(e).__name__)


async def _issue_finish_downlink(session_id, upstream, websocket) -> None:
    """三式收尾统一下发（q2 = ⓒ 裁定；261019-p3-record-dialogue-guide RDG-A6）：

    归档成功同拍向本连接设备腿下发 control/start_organizing 恰一帧（front 机械
    自推单源, self_push=True 放行）。腿 = 泵自身双腿（本连接权威对，不经
    _alert_legs——避免与登记腿重复下发双帧）。幂等由会话状态沿承载：归档后 routing
    回 chat, 同一收尾沿的重复消费（feed_rec / finish_rec 双路径互斥）与重复 sweep
    不再命中。单腿失败降级留痕, 不连坐另一腿、不阻断泵。

    261022 真机轮修订：kidpal_record_context active=false 出语境信号**不再在此处
    发**——原形态在整理回合开始前就解除引擎抑制，收尾后迟到的幻听 STT（KPLOG006
    23:59:30 「Thank you」）被引擎当普通对话派发 LLM 并播报「我没听清/再说一遍」，
    盖过整理演出。出语境改由 _story_finish_session 在整理回合**完成后**发
    （_release_record_context 单源）——整理期间孩子的后续话语走闭嘴回显，不进对话。"""
    try:
        await _push_display_payload(websocket, dict(START_ORGANIZING_PAYLOAD))
    except Exception as e:  # noqa: BLE001 — 单腿失败不连坐
        log.warning("story_start_organizing_push_err corr_id=%s err=%s",
                    session_id, type(e).__name__)


async def _release_record_context(session_id, legs) -> None:
    """出语境单源（261022 真机轮修订）：kidpal_record_context active=false 逐引擎腿
    注入（引擎抑制解除）。收尾路径统一在**整理回合完成后**调用——迟到 STT 在整理
    期间走闭嘴回显不进对话（孤儿幻听不再触发 LLM 播报）。空素材/纯幻听拒绝路径
    同样必须调用（会话已释放但引擎语境标志尚活——不解除 = 后续正常对话被抑制）。"""
    for _device_ws, engine_ws in list(legs):
        try:
            await _send_engine_leg_frames(
                engine_ws, [{"type": RECORD_CTX_SIGNAL_TYPE, "active": False}])
        except Exception as e:  # noqa: BLE001 — 单腿失败降级留痕不连坐
            log.warning("story_record_ctx_exit_err corr_id=%s err=%s",
                        session_id, type(e).__name__)


async def _story_finish_session(session_id, acc, legs) -> "dict | None":
    """三式收尾单源（261019-p3-record-dialogue-guide fixer R3；S-F2.2-AC4 v1.65）：

    语音收尾词路径（_story_finish_on_phrase）、KEY listen-stop 立即归档半
    （_story_stop_on_listen_stop）、pending-stop 恢复归档半（_story_feed_from_stt）、
    自动收尾（_story_auto_finish）全部委托本函数——整理回合范式单源。

    序：
      ① 素材装配（build_organizing_input 只读）+ 会话释放（cancel 终态统一，任一分支
         不挂死；先于演出 await 释放，防并发 sweep 重复命中同一收尾沿）；
      ② 演出先行：逐腿 control/start_organizing 恰一帧 + 出语境 kidpal_record_context
         active=false（_issue_finish_downlink 同源）；
      ③ 忠实整理回合 + 归档（_organize_and_archive）；
      ④ 成功 -> story_text 正文 + story_list 三时点②（逐设备腿）；
         失败 -> 当轮素材不归档 + 固定安抚句 ORGANIZING_FAIL_SOOTHING 经 kidpal_speak
         下发（R1-N4；逐引擎腿，与引导回合同型通道、不产生提醒帧）+ **补 story_list
         页面退出帧**（逐设备腿；设备已进 ORGANIZING 过渡态，无下行帧即困死整合页，
         261021 真机轮 R6 修复——列表 total 不虚增，仅缺 story_text 正文帧）；
      空素材（只有收尾词/无累积）-> 不归档、不演出、不安抚（无可丢内容），仅释放。

    legs = [(device_ws, engine_ws)] 当前在线腿快照（泵路径 = 本连接双腿；sweep 路径 =
    _alert_legs）。单腿失败降级留痕、不连坐其它腿。返回归档记录或 None（K6 归因）。"""
    raw = acc.build_organizing_input(session_id) or ""
    material = gw_story.strip_trailing_finish_words(raw).strip()
    acc.cancel(session_id, reason="finish")
    if not material:
        log.info("story_finish corr_id=%s archived=false reason=empty_material", session_id)
        # 261022 真机轮修订：空素材路径此前零下行帧 = 引擎语境标志永不解除（后续
        # 正常对话被抑制的潜伏缺陷）——同样必须出语境。
        await _release_record_context(session_id, legs)
        return None
    # 261020 D4-2 ①归档边界内容守卫：纯幻听/近无内容（单语气词、纯标点）不归档
    # （不占 seq、不落 SD）、不演出（不下发 start_organizing）、不安抚——与空素材
    # 同款仅释放。R1-N4 口径裁决：安抚句语义是「有稿丢了」，纯幻听 = 孩子本无内容，
    # 安抚反而误导（留注记，归 acceptance-author 后续通道复裁）。
    reject = _archive_material_reject_reason(material)
    if reject:
        log.info("story_finish corr_id=%s archived=false reason=%s material=%r",
                 session_id, reject, material)
        # 261022 真机轮修订：拒绝路径与空素材同款，必须出语境（见 empty 分支注记）。
        await _release_record_context(session_id, legs)
        return None
    for device_ws, engine_ws in list(legs):
        await _issue_finish_downlink(session_id, engine_ws, device_ws)
    # 261020 D4-1: 整理回合移出事件循环（同步 urllib LLM 回合 ~2-4s 曾整冻 front，
    # 把收尾沿先行的 start_organizing 压到与 story_text 同拍——过渡页不可见本体）。
    record = await asyncio.to_thread(_organize_and_archive_offloop,
                                     material, session_id)
    # 261022 真机轮修订：出语境移到整理回合**完成后**（原在 ② 演出先行时提前解除，
    # 整理期间迟到的幻听 STT 被引擎当普通对话派发 LLM 播报「我没听清」——KPLOG006
    # 23:59:30-36 实证）。整理期间孩子话语走闭嘴回显（会话已 cancel，不进素材）。
    await _release_record_context(session_id, legs)
    if record is None:
        # R1-N4 丢稿口径：当轮素材不归档 + 固定安抚句（不发明新语义）。
        await _speak_to_legs(ORGANIZING_FAIL_SOOTHING, legs)
        # 261021 真机轮 R6 缺陷修复（KPLOG005 实证）：失败分支**必须补页面退出帧**——
        # 设备已在 ② 按 start_organizing 进 ORGANIZING 非交互过渡态（substate=6，
        # KEY/BOOT 全吞 = 261019 有意的过渡态）。成功路径靠 story_text + story_list
        # 出页；失败路径原先零下行帧 -> 安抚句 TTS 播完进 Idle 后，[R2]
        # kidpal_story_session_end 只复位 RECORDING 页（ORGANIZING 命中 no-op）-> 设备
        # 困死整合页、重启是唯一出口。补 story_list 推送 = 「收尾后回 P3 列表、当轮不
        # 归档」：列表里没有这篇 = 准确反映未归档（total 不虚增），与成功路径的唯一差别
        # 是没有 story_text 正文帧。复用既有 _push_story_list 通道与错误处理形态
        # （单腿失败不连坐、不发明新帧）。
        pushed = 0
        for device_ws, _engine_ws in list(legs):
            try:
                await _push_story_list(device_ws)
                pushed += 1
            except Exception as e:  # noqa: BLE001 — 单腿失败不连坐其它腿
                log.warning("story_finish_push_err corr_id=%s err=%s",
                            session_id, type(e).__name__)
        log.info("story_finish corr_id=%s archived=false reason=organize_failed "
                 "story_list_pushed=%d", session_id, pushed)
        return None
    for device_ws, _engine_ws in list(legs):
        try:
            await _push_display_payload(device_ws, gw_story.story_text(record))
            await _push_story_list(device_ws)
        except Exception as e:  # noqa: BLE001 — 单腿失败不连坐其它腿
            log.warning("story_finish_push_err corr_id=%s err=%s",
                        session_id, type(e).__name__)
    log.info("story_finish corr_id=%s archived=true id=%s", session_id, record["id"])
    return record


async def pump_client_to_upstream(websocket, upstream, session_id=None) -> list:
    """Device->engine leg: reads starlette-style dict messages via await websocket.receive()
    ({"type": "websocket.receive", "text"/"bytes": ...}), forwards every frame to
    upstream.send() UNCHANGED (决策23: 代理语义, front 只消费回执、不重发), consumes device
    JSON-RPC render receipts via on_device_receipt, and returns the list of receipt ids
    consumed as delivery acks. Client disconnect (WebSocketDisconnect / RuntimeError) ends.

    ⚠ 语义钉行（260829-schedule-reminder 收口，review-packet Findings「acked_ids 含受理帧」）：
    本列表由 gateway.mcp_relay.relay_ack 的**宽容**判据产生（result 非空即算 acked，该函数语义
    是 display/通用回执的冻结面、本包一字未动），因此 `kidpal/alert` 工具回调的**同步受理帧**
    （设备回 "ack": false，只代表「已收到/已入队」）也会被计进 acked_ids —— **acked_ids 不等于
    送达确认**。提醒档位的送达确认只有一条路：reminders.alert_receipt_id（严格判据 result.ack
    is True，设备面唯一来源是渲染完成后的 McpServer::EmitRenderReceipt）→ _queue_alert_receipt
    → run_alert_poll_once 里 consume_receipts 出队。日后若要消费「已送达」语义，MUST 走
    drain_alert_receipts()/alert_dispatcher()，不得改读本列表（当前本列表无消费方）。
    """
    acked_ids = []
    try:
        while True:
            msg = await websocket.receive()
            if msg.get("type") == "websocket.disconnect":
                break
            if msg.get("type") != "websocket.receive":
                continue
            if isinstance(msg.get("bytes"), bytes):
                await upstream.send(msg["bytes"])
            else:
                text = msg.get("text", "")
                try:
                    up_obj = json.loads(text)
                except (TypeError, ValueError):
                    up_obj = None
                # 契约 §四：KidPal 私有控制帧（kidpal_session_ctx / kidpal_story_delete）
                # 由网关整条消费并拦截、不下透引擎（无论取值）；ctx 进入记录语境沿同拍
                # 向引擎腿注入 Package B 等价信号（261019, upstream additive 形参）。
                if await _consume_kidpal_control(session_id, up_obj, websocket,
                                                 upstream=upstream):
                    continue
                await upstream.send(text)
                # 云侧停录归档接线 (包 9b HWV-A10/A12)：旁路消费设备 listen/stop 上行帧
                # （转发不变，决策23）——命中累积态则走忠实整理收尾单源（KEY 收尾式；
                # S-F2.2-AC4 v1.65：整理输出成篇，不再原始转写直归档）。front 自推
                # 不经 relay（Q7 天然豁免）；演出先行序与另两式同构
                # （downlink -> story_text -> story_list）。单帧单发、不拆帧。
                if _story_stop_on_listen_stop(session_id, up_obj):
                    await _story_finish_session(session_id, _get_story_acc(),
                                                [(websocket, upstream)])
                acked, rid = on_device_receipt(text)
                if acked:
                    acked_ids.append(rid)
                # additive (包 260829-schedule-reminder): 提醒档位只认**渲染回执**
                # (result.ack === true)，设备对 kidpal/alert 的同步 accepted 受理帧不算
                # 送达确认 —— 渲染即确认、即出队（契约 §三 / RMD-A6）。队列有界。
                alert_receipt = gw_reminders.alert_receipt_id(text)
                if alert_receipt is not None:
                    _queue_alert_receipt(alert_receipt)
    except (WebSocketDisconnect, RuntimeError):  # device leg closed — normal teardown.
        pass
    return acked_ids


def on_device_receipt(message) -> tuple[bool, object]:
    """Delegate the device JSON-RPC receipt to gateway.mcp_relay.relay_ack (决策23: 渲染即
    确认、即出队语义由设备侧承载 — front 不得自造分叉实现).  The device's receipt rides the
    wire frame {"type":"mcp","payload":<JSON-RPC>} (firmware mcp_server.cc:473 + 回执形态
    pinned in the acceptance) — the front unwraps payload before relay_ack (relay_ack 本体
    语义不动). Accepts a dict or a JSON string. Returns (acked: bool, id)."""
    if isinstance(message, str):
        try:
            message = json.loads(message)
        except (TypeError, ValueError):
            pass  # non-JSON — relay_ack rejects the non-dict below
    if (isinstance(message, dict) and message.get("type") == "mcp"
            and isinstance(message.get("payload"), dict)):
        message = message["payload"]  # unwrap the device wire frame before relay_ack
    return gw_mcp.relay_ack(message)


# --- 提醒投递面接线 (TaskPackage 260829-schedule-reminder / RMD-A4 A6 A7) ----------
# 网关 push helper：每分钟读 alerts/pending.json -> 机械校验 + 到点判定 -> 经 mcp
# kidpal/alert 推给当前在线设备腿；60 秒未收渲染回执重推、单档上限 3 次、不做重连即重推、
# 超出重试窗口静默作废不补发（状态机在 gateway/reminders.AlertDispatcher，本文件只接线）。
# 设备渲染回执经 consume_receipts 接回出队方 —— 这条线补的就是 g-relay-fix 留下的
# 「acked_ids 目前无人消费、未接出队方」缺口。语音按裁定二向引擎腿注入 kidpal_speak，
# front 不自建合成、不做 mp3→opus 编码、不注入二进制音频帧（设备腿 send_bytes 恒空）。
# 有界性（ECS available 941MB / Swap used 535MB 同机还有引擎与模型）：交接路径 env 可配、
# 回执缓冲与投递记账都被 MAX_* 卡住、无在线设备时不排队攒历史。

ALERT_LEG_CAP = 8                       # 同时登记的（设备腿, 引擎腿）对数上界
_alert_dispatcher = None
_alert_receipts = collections.deque(maxlen=gw_reminders.MAX_TRACKED)
_alert_legs: list = []
_alert_loop_task = None
# 故事卡壳 sweep 独立任务句柄（261022-p3-record-front-polish-fix）：与 _alert_loop_task
# 同位起停、同生命周期语义（进程内一份；连接全断后不停转——不做额外 stop 语义）。
_story_sweep_task = None
_alert_delivery_tasks: set = set()


# --- 课表三触发 producer 接线 (M3-K1 / 决策 26) -----------------------------------
# 契约 §1.3⑥ home 可选 data.upcoming 承载：建连 / 每日零点 / 录入即时三时点把「接下来」
# 推向设备。触发① 直接（ws 建连后即推）；触发② 每日零点 + 触发③ 录入即时 marker 走
# reminder 轮询循环 piggyback（复用 alerts 交接目录模式）。决策面（daily_push_due /
# refresh_requested / consume_refresh_marker / connect_push_payload 单源）在
# gateway.schedule_push；本处只做脏活接线。副通道（家长直改 schedule.md 不落 marker）
# 无即时推、靠①+②兜底（S-F1.5 AC2）。
# 时区固定 UTC+8（house 先例：B1 frontmatter timezone 唯一在役 Asia/Shanghai；
# host 无 tzdata，禁用 zoneinfo 直算偏移）。
_T8 = timezone(timedelta(hours=8))
SCHEDULE_MD = os.environ.get("KIDPAL_SCHEDULE_MD", "/opt/kidpal/schedule.md")
ALERTS_DIR = os.environ.get("KIDPAL_ALERTS_DIR", "/opt/kidpal/alerts")
# 学校课程表首节时刻（合并「上学」项的显式入参 school_start，决策 26）——repo 侧载体
# = openclaw-schedule.config.yaml，env 可配（同三档提醒 KIDPAL_* 口径，禁止硬编绝对路径）。
_DEPLOY_DIR = Path(__file__).resolve().parents[2] / "deploy"
SCHEDULE_CFG = os.environ.get(
    "KIDPAL_SCHEDULE_CONFIG", str(_DEPLOY_DIR / "openclaw-schedule.config.yaml"))
_daily_push_local_date: "str | None" = None   # 触发②记账：已推本地日（每 local 日恰一次）


def _write_schedule_parse_alert(err_type: str, detail: str) -> None:
    """把 schedule.md 解析失败落 alerts/schedule-parse-error.json（QSW-A3 / TECH-261012 建议④）。

    与 deploy-drift.json 同族机器可读告警件（checked_at / error / detail 三字段；checked_at
    口径对齐 verify-deploy.sh 的 UTC ISO8601）。front 每分钟轮询命中解析失败即覆写一次 ->
    checked_at 持续刷新；解析恢复后不再写 -> checked_at 冻结（kidpal-schedule-health skill
    据此判「已恢复」，front 无需在成功路径删件）。**自身全吞写失败**（至多 debug 日志）——
    _load_schedule_model 契约是「返回 None、不炸轮询循环」，写告警件失败不得反向上抛。
    不加 ack 字段：本件每轮被覆写，ack 归 health skill 自有状态件（plan §4.2）。"""
    try:
        os.makedirs(ALERTS_DIR, exist_ok=True)
        target = os.path.join(ALERTS_DIR, "schedule-parse-error.json")
        tmp = f"{target}.tmp"
        payload = {
            "checked_at": datetime.now(tz=timezone.utc).isoformat(timespec="seconds"),
            "error": err_type,
            "detail": detail,
        }
        with open(tmp, "w", encoding="utf-8") as fh:
            json.dump(payload, fh, ensure_ascii=False)
        os.replace(tmp, target)
    except Exception:  # noqa: BLE001 — 写告警件失败静默吞，不炸轮询循环
        log.debug("schedule_parse_alert_write_err", exc_info=True)


def _load_schedule_model() -> "dict | None":
    """读 schedule.md -> B1 模型；读/解析失败返回 None（产出可归因，不炸轮询循环）。"""
    try:
        text = Path(SCHEDULE_MD).read_text(encoding="utf-8")
    except OSError:
        return None
    try:
        return gw_schedule.parse_schedule_md(text)
    except ValueError as e:  # noqa: BLE001 — 脏 B1 大声拒绝传日志，轮询照走
        detail = str(e)[:120]
        log.warning("schedule_md_err err=%s detail=%s", type(e).__name__, detail)
        _write_schedule_parse_alert(type(e).__name__, detail)
        return None


def _local_now_hhmm() -> str:
    return datetime.now(tz=_T8).strftime("%H:%M")


def _today_tomorrow_iso() -> tuple[str, str]:
    today = datetime.now(tz=_T8).date()
    return today.isoformat(), (today + timedelta(days=1)).isoformat()


def _schedule_school_start() -> "str | None":
    """读 openclaw-schedule.config.yaml 的 school_start（决策 26 合并「上学」项首节时刻）。

    读/解析失败 -> None（可归因、不炸中继，与 _load_schedule_model 同口径）。"""
    try:
        text = Path(SCHEDULE_CFG).read_text(encoding="utf-8")
    except OSError:
        return None
    m = re.search(r"^school_start:\s*[\"']?([0-2]\d:[0-5]\d)", text, re.MULTILINE)
    return m.group(1) if m else None


def _expand_schedule_intent(payload) -> "dict | None":
    """「帮我显示课表」意图薄载荷 -> front 截获展开为新鲜周网格（M3K3-A2 接线。

    relay 链上的参数装配（实现细节，frozen contract §3 自陈）：读 schedule.md 取 B1
    模型、week_date_iso = 本地今天、now_hhmm = 当前时刻、school_start 取配置文件首节
    时刻；模型读/解析失败 -> 不截获（None，薄载荷原样走 relay 链交 display_schema
    机械门裁决，不炸中继）。截获判据 = schedule.expand_schedule_intent_payload（恰
    薄载荷才展开、其余 None）；脏 B1 的 ValueError 沿 producer 大声拒绝（记日志，
    不静默产空网格——P2-A2）。"""
    if not isinstance(payload, dict) or payload.get("view") != "schedule":
        return None
    model = _load_schedule_model()
    if model is None:
        return None
    week_date_iso, _tomorrow = _today_tomorrow_iso()
    try:
        return gw_schedule.expand_schedule_intent_payload(
            payload, model, week_date_iso, _local_now_hhmm(), _schedule_school_start())
    except ValueError as e:  # noqa: BLE001 — 脏 B1 大声拒绝传日志，中继照走
        log.warning("schedule_intent_expand_err err=%s detail=%s",
                    type(e).__name__, str(e)[:120])
        return None


async def _relay_schedule_payload(payload: dict, websocket) -> None:
    """把 home upcoming payload 以设备 wire 单帧下发（复用既有 display 中继链，
    home 形状必过 display_schema 机械门；down 即降级不裸送）。

    中继链的冻结入口 build_display_tool_calls 只对 str 块做 json.loads 再校验，
    dict 块会被判 "not JSON-object parseable" 一律降级成 text 兜底（2026-09-21
    线上实测：建连推送全部变成「这张小卡片没显示好」）——本函数必须 dumps 成 str
    再入中继。"""
    await _relay_display_blocks_to_device(
        [json.dumps(payload, ensure_ascii=False)], websocket)


async def _push_schedule_connect(websocket) -> None:
    """建连推送缝（ws_gateway 建连点调用的 async helper；把 home 推送收编 + 追加
    grid restore relay）。

    契约 §1.3② / ADR-005（server schedule_grid_restore.acceptance.py A1/A3 齿）：
      1) _load_schedule_model() None（缺失/坏 schedule.md）-> 静默 return——零帧、
         零异常（建连不被课表推送阻断，try/except 语义保持）；
      2) 推 1 = home upcoming（home_upcoming_payload 同参，与既有建连推送零偏移；
         K6 锚① home upcoming 零回归面）；
      3) 推 2 = grid_restore_enabled() 为真时追加 grid restore（grid_restore_payload
         model/today/now/school_start——单源委托 week_grid_payload，开关门控）。
      两推各自独立 try/except：单推失败记日志不连坐另一推、不外抛（建连不被推送阻断）。

    grid_restore_payload 的 now_hhmm / school_start 传 None/None——恢复性载荷 = 静态
    周网格（不含当前时刻星标与合并「上学」实时态，ADR-005「只填缓存」恢复语义），且与
    frozen A1 齿的参照 face week_grid_payload(model, today)（双 None）逐字段同算。
    """
    model = _load_schedule_model()
    if model is None:
        return
    today_iso, tomorrow_iso = _today_tomorrow_iso()
    # 推 1 = home upcoming（与既有建连推送完全同参，退化环境与现状零偏移）
    try:
        await _relay_schedule_payload(
            gw_schedule.home_upcoming_payload(model, today_iso, _local_now_hhmm(),
                                              tomorrow_iso, _schedule_school_start()),
            websocket)
    except Exception as e:  # noqa: BLE001 — 单推失败记日志、不连坐 grid 推/不阻断建连
        log.warning("schedule_connect_home_push_err err=%s", type(e).__name__)
    # 推 2 = grid restore（ECS 开关默认关门控；开闸前不发出——C1 挂账）
    if gw_schedule_push.grid_restore_enabled():
        try:
            await _relay_schedule_payload(
                gw_schedule_push.grid_restore_payload(model, today_iso),
                websocket)
        except Exception as e:  # noqa: BLE001 — grid 单推失败不连坐建连
            log.warning("schedule_connect_grid_push_err err=%s", type(e).__name__)


async def _push_schedule_refresh_to_legs(model: dict) -> int:
    """触发①③共用：把 home upcoming 推给当前在线设备腿，返回投递腿数。

    grid restore piggyback：开关开（grid_restore_enabled）时每腿在 home 之后追加
    grid restore 帧（home 先 grid 后——本函数单源承载、不新增第二套进程内日账，
    F3 纪律；grid 走 grid_restore_payload 单源 producer，now/school 双 None = 静态
    周网格恢复性载荷）。"""
    today_iso, tomorrow_iso = _today_tomorrow_iso()
    home_payload = gw_schedule.home_upcoming_payload(model, today_iso,
                                                     _local_now_hhmm(), tomorrow_iso,
                                                     _schedule_school_start())
    grid_payload = None
    if gw_schedule_push.grid_restore_enabled():
        grid_payload = gw_schedule_push.grid_restore_payload(model, today_iso)
    sent = 0
    for device_ws, _engine_ws in list(_alert_legs):
        try:
            await _relay_schedule_payload(home_payload, device_ws)
            sent += 1
        except Exception as e:  # noqa: BLE001 — 单腿失败不连坐其它腿
            log.warning("schedule_push_leg_err err=%s", type(e).__name__)
        if grid_payload is not None:
            try:
                await _relay_schedule_payload(grid_payload, device_ws)
                sent += 1
            except Exception as e:  # noqa: BLE001 — grid 单腿失败不连坐 home/其它腿
                log.warning("schedule_grid_leg_err err=%s", type(e).__name__)
    return sent


async def run_schedule_poll_once(now_utc=None) -> dict:
    """piggyback：触发② 每日零点 + 触发③ 录入即时 marker，任一到点即推一批。

    单批语义：本轮内触发器（每日跨天 / 录入 marker）任一命中，就把 home upcoming +
    (开关开时) grid restore 以**同一 model 快照**合成一批下发给在线设备腿——不因每日
    与 marker 同一轮同时到点而重复推多批（frozen K7-A7 marker 齿：重置日账 + 置 marker
    的同一轮只期望 [home, schedule] 一批）。既有日账恰一次语义承载 grid 与 home
    同批（不新增第二套进程内日账，F3 纪律——日账冻结基线恒 4 处）；marker 消费恰一次。

    返回可观测计数（K6 correlationId = 触发类别 记账）供日志归因。"""
    global _daily_push_local_date
    model = _load_schedule_model()
    if model is None:
        return {"trigger2": False, "immediate": 0, "sent": 0, "reason": "no_model"}
    now_utc_dt = datetime.now(tz=timezone.utc) if now_utc is None else now_utc
    corr = gw_provider.corr_id()
    sent = 0
    tz_name = model.get("timezone") or ""
    trigger = "none"
    local_date = None
    # 触发② 每日零点：本地日历日跨天 -> 只推恰一次（记账进位）
    if gw_schedule_push.daily_push_due(tz_name, _daily_push_local_date, now_utc_dt):
        local_date = now_utc_dt.astimezone(_T8).date().isoformat()
        _daily_push_local_date = local_date
        trigger = "midnight"
        log.info("schedule_push trigger=midnight corr=%s local=%s", corr, local_date)
    # 触发③ 录入即时：轮询 piggyback 消费 refresh marker（副通道无 marker -> 不触发）
    immediate = 0
    while gw_schedule_push.consume_refresh_marker(ALERTS_DIR):
        immediate += 1
        if trigger == "none":
            trigger = "immediate"
    # 任一到点：单批下发（home 先 + grid 附随后；grid 开关门控）。标记 trigger 类别记账。
    if trigger != "none":
        sent += await _push_schedule_refresh_to_legs(model)
        if trigger == "midnight":
            log.info("schedule_push sent corr=%s trigger=midnight local=%s sent=%d",
                     corr, local_date, sent)
        else:
            log.info("schedule_push sent corr=%s trigger=immediate markers=%d sent=%d",
                     corr, immediate, sent)
    return {"trigger2": True, "immediate": immediate, "sent": sent, "corr": corr}


def _entry_from_envelope(envelope, business_id) -> dict:
    """从 kidpal/alert 信封还原 §三 条目（wire 上 view 是 JSON 字符串 -> 解回对象）。

    只依赖信封本身，不在模块里另建条目缓存（少一处会漂移的状态）；view 解不动时退回
    title 的 text 视图，仍过 display_schema 白名单。"""
    args = ((envelope or {}).get("params") or {}).get("arguments") or {}
    view = args.get("view")
    if isinstance(view, str):
        try:
            view = json.loads(view)
        except (TypeError, ValueError):
            view = None
    if not isinstance(view, dict):
        view = {"view": "text", "data": {"text": str(args.get("title") or "")}}
    return {
        "id": args.get("id") or business_id,
        "fire_at": args.get("fire_at"),
        "title": args.get("title"),
        "speech": args.get("speech") or "",
        "view": view,
    }


def _deliver_alert_now(business_id, envelope) -> None:
    """AlertDispatcher 的 sink：把一档提醒真投到当前在线的两腿（设备腿 mcp 帧 + 引擎腿注入）。

    无在线设备腿 = 本轮不投递（不在 front 里排队攒历史）：dispatcher 按 60s×3 重试窗口
    继续尝试，窗口耗尽即静默作废（决策 24 断网无提醒；不追催）。"""
    legs = list(_alert_legs)
    if not legs:
        log.warning("alert_skip corr_id=%s reason=no_online_device pushed=false", business_id)
        return
    entry = _entry_from_envelope(envelope, business_id)
    for device_ws, engine_ws in legs:
        try:
            task = asyncio.create_task(
                gw_reminders.deliver_alert(entry, device_ws, engine_ws))
        except RuntimeError:  # 无运行中的事件循环（离线装载场合）—— 记账仍在，投递跳过
            log.warning("alert_defer corr_id=%s reason=no_running_loop", business_id)
            return
        _alert_delivery_tasks.add(task)
        task.add_done_callback(_alert_delivery_tasks.discard)


def alert_dispatcher():
    """提醒投递状态机单例（进程内一份，记账有界）。"""
    global _alert_dispatcher
    if _alert_dispatcher is None:
        _alert_dispatcher = gw_reminders.AlertDispatcher(sink=_deliver_alert_now)
    return _alert_dispatcher


def _queue_alert_receipt(rid) -> None:
    """设备渲染回执入队（deque(maxlen=MAX_TRACKED)：满则丢最旧，绝不无界增长）。"""
    _alert_receipts.append(rid)


def drain_alert_receipts() -> list:
    out = list(_alert_receipts)
    _alert_receipts.clear()
    return out


def _register_alert_legs(device_ws, engine_ws) -> None:
    while len(_alert_legs) >= ALERT_LEG_CAP:
        _alert_legs.pop(0)      # 超出上界丢最旧一条腿（同机单设备实态；有界性优先）
        log.warning("alert_leg_overflow cap=%d dropped=oldest_leg", ALERT_LEG_CAP)
    _alert_legs.append((device_ws, engine_ws))


def _unregister_alert_legs(device_ws) -> None:
    for pair in list(_alert_legs):
        if pair[0] is device_ws:
            _alert_legs.remove(pair)


# --- [SC-D] 腿集会话归口映射（TaskPackage 261021-p3-record-cancel-lifecycle F3⑤）----
# 会话 -> 归属连接双腿 [(device_ws, engine_ws)] 的连接级映射：ws_gateway 双腿在拍
# 登记（与 _register_alert_legs 同位）、断连 finally 摘除（同生命周期）。sweep 引导
# 注入（_story_guide_round）与自动收尾（_story_auto_finish）只打该会话归属连接的腿
# ——zombie 会话跨重连把引导注入/收尾帧族打进新连接 = 症状②⑤的根因；**提醒
# （_deliver_alert_now）与课表（_push_schedule_refresh_to_legs）两消费点维持全局
# _alert_legs 不动**（F3⑤ must-stay-green 行为锚）。
_session_legs_map: dict = {}


def _register_session_legs(session_id, device_ws, engine_ws) -> None:
    """登记会话 -> 归属连接腿对（ws_gateway 双腿在拍调用；与 _register_alert_legs 同位）。

    session_id 为每连接 uuid4 键（HWV-A11：SF6 弃用对象地址），故一次登记即该会话的
    唯一归属；重复登记（幂等重入）覆盖为最新腿对。"""
    if not session_id:
        return
    _session_legs_map[session_id] = [(device_ws, engine_ws)]


def _unregister_session_legs(device_ws) -> None:
    """按 device_ws **对象身份**摘除该连接的全部会话登记（_unregister_alert_legs
    同款身份判据）；摘空的会话键一并移除（查询面随之返 []）。"""
    for sid in list(_session_legs_map.keys()):
        kept = [pair for pair in _session_legs_map.get(sid, []) if pair[0] is not device_ws]
        if kept:
            _session_legs_map[sid] = kept
        else:
            _session_legs_map.pop(sid, None)


def _session_legs(session_id) -> list:
    """该会话归属连接的腿 [(device_ws, engine_ws)]；未知/已摘除 -> []。

    纯查询：返回副本、不改状态（[SC-D] 冻结语义）。"""
    return list(_session_legs_map.get(session_id, ()))


async def run_alert_poll_once(now_epoch=None) -> dict:
    """一轮投递闭环：读 pending（按分钟）-> 先接出队回执 -> 到点判定并推送。

    先消费回执再 tick：上一分钟内渲染完成的档位不再被本轮重推（重复弹窗根除）。
    返回可观测计数供探针/日志归因（K6 correlationId = 业务 id / batch）。"""
    stamp = datetime.fromtimestamp(
        time.time() if now_epoch is None else float(now_epoch)).astimezone()
    now_iso = stamp.isoformat(timespec="seconds")
    entries, rejected = gw_reminders.read_pending(gw_reminders.pending_path(), now_iso)
    dispatcher = alert_dispatcher()
    acked = gw_reminders.consume_receipts(dispatcher, drain_alert_receipts())
    pushed = dispatcher.tick(entries, now_iso)
    if rejected:
        log.warning("alert_pending_rejected corr_id=batch count=%d first=%s",
                    len(rejected), rejected[0])
    log.info("alert_poll corr_id=batch pending=%d rejected=%d pushed=%d acked=%d tracked=%d",
             len(entries), len(rejected), len(pushed), len(acked), dispatcher.tracked_count())
    return {"pending": len(entries), "rejected": len(rejected), "pushed": pushed,
            "acked": acked, "tracked": dispatcher.tracked_count()}


async def reminder_poll_loop() -> None:
    """每分钟轮询到点推送 + 课表 piggyback（触发②每日零点/③录入即时 marker）。

    节拍对齐整分钟；单轮异常绝不杀死循环。提醒投递与课表推送同节拍，两路各自容错。
    故事卡壳 sweep 已摘出到独立短节拍（_story_sweep_loop；261022-p3-record-front-polish-fix
    ——原同节拍使 8s 阈值最坏 60s+ 迟到，且 sweep 的 LLM 在途会阻塞提醒投递）。"""
    try:
        await run_alert_poll_once()          # 起轮先跑一次：避免刚连上就错过整分钟点
    except Exception as e:  # noqa: BLE001
        log.warning("alert_poll_error err=%s", type(e).__name__)
    try:
        await run_schedule_poll_once()       # 课表 piggyback：同一循环，容错不连坐
    except Exception as e:  # noqa: BLE001
        log.warning("schedule_poll_error err=%s", type(e).__name__)
    while True:
        try:
            await asyncio.sleep(gw_reminders.next_poll_delay(time.time()))
            await run_alert_poll_once()
            await run_schedule_poll_once()
        except asyncio.CancelledError:
            raise
        except Exception as e:  # noqa: BLE001 — 提醒投递不得因单轮异常停摆
            log.warning("alert_poll_error err=%s", type(e).__name__)


async def _story_sweep_loop() -> None:
    """故事卡壳 sweep 独立短节拍轮询（261022-p3-record-front-polish-fix 真机轮追补）。

    原 sweep 挂 reminder_poll_loop 整分钟齐射：卡壳阈值 8s 却最坏 60s+ 才生效
    （KPLOG006 实测 66s 迟到），且齐射会在孩子说话中插话（引导播报 barge-in -> 引擎
    abort 竞态）。固定 _STORY_SWEEP_INTERVAL_SECONDS 间隔独立轮询；单轮异常不连坐循环。
    最小实现：固定间隔、无抖动/无自适应（minimal: 回退或调频只动该常量；若日后确需
    抖动防多进程齐射，在此循环内加 jitter，升级路径不改调用面）。"""
    while True:
        try:
            await _story_timeout_sweep()
        except asyncio.CancelledError:
            raise
        except Exception as e:  # noqa: BLE001 — 卡壳 sweep 不得因单轮异常停摆
            log.warning("story_timeout_sweep_err err=%s", type(e).__name__)
        await asyncio.sleep(_STORY_SWEEP_INTERVAL_SECONDS)


def _ensure_alert_loop() -> None:
    """首个已鉴权设备连接建立时起轮询任务（进程内只跑一份；无设备时不空转）。
    故事卡壳 sweep 同位起独立节拍任务（261022-p3-record-front-polish-fix：生命周期与
    提醒轮询对齐，各自一份、互不连坐）。"""
    global _alert_loop_task, _story_sweep_task
    try:
        if _alert_loop_task is None or _alert_loop_task.done():
            _alert_loop_task = asyncio.create_task(reminder_poll_loop())
        if _story_sweep_task is None or _story_sweep_task.done():
            _story_sweep_task = asyncio.create_task(_story_sweep_loop())
    except RuntimeError:  # 无运行中事件循环（离线装载）
        pass


@app.websocket("/{path:path}")
async def ws_gateway(websocket: WebSocket):
    """Device leg: forced Bearer auth at HTTP upgrade; reject (403) before accept().

    A forwarded authenticated stream goes to the plain-ws engine (loopback). The live
    probes hit the auth-reject path; the engine forward is best-effort auxiliary.
    """
    # mechanical auth gate — no accept() on failure => Starlette answers 403 on the upgrade.
    provided = (websocket.headers.get("authorization") or "")
    ok, _status = gw_auth.authorize(DEVICE_TOKEN, provided)
    if not ok:
        log.warning("ws handshake rejected (auth) — no accept")
        return
    await websocket.accept()
    # 包 9b HWV-A11 (F6): 每连接一个**不可复用**会话键（uuid4）——弃用 websocket 对象地址作键
    # （CPython id, 连接关闭后地址可复用 -> StoryAccumulator 会话跨连接串味）。
    # 两条腿共享此键，贯穿 start/feed/stop/cancel 的整条故事累积链路。
    session_id = f"sess-{uuid.uuid4().hex}"
    # best-effort forward to the engine's plain ws (loopback). If the engine is not up,
    # we close the client gracefully rather than error-spam.
    try:
        # pass the device's whitelist headers to the engine: it requires device-id
        # (websocket_server.py:83) and honours authorization / protocol-version / client-id.
        # case-insensitive match, preserve original value, token erased from logs.
        _ALLOWED_HEADERS = ("authorization", "device-id", "client-id", "protocol-version")
        hdrs = {h: v for h, v in websocket.headers.items() if h.lower() in _ALLOWED_HEADERS}
        try:
            from websockets.asyncio.client import connect as ws_connect  # websockets >= 13
            headers_kwarg = "additional_headers"
        except ImportError:  # legacy websockets (< 13)
            from websockets import client as _ws_legacy_client
            ws_connect = _ws_legacy_client.connect
            headers_kwarg = "extra_headers"
        async with ws_connect(ENGINE_WS, open_timeout=10, **{headers_kwarg: hdrs}) as upstream:
            # 故事三时点 ①：WS 建连恢复推送（requirements v1.63 S-F2.1 补充规则 / arch
            # §5.2 场景④ 三时点①——2026-10-12 赎回 260928-story-connect-push-defer 挂起
            # 的时点①重裁）。data.restore 标记 = 板端 restore 分流载体（只填 RAM 列表缓存、
            # 不接管前台、P3 在屏仅重绘本页），消解 260928 摘除理由（旧固件无条件渲染 P3
            # 盖聆听态）。②归档后 / ③删除后的主动推送缺省零偏移。开关
            # story_restore_enabled()（ECS 配置层默认关由部署侧显式置 0 承载，C1）。
            # 失败不阻断建连（SL-A6，schedule connect push 同位 try/except 先例）。
            if story_restore_enabled():
                try:
                    await _push_story_list(websocket, restore=True)
                except Exception as e:  # noqa: BLE001 — 建连推送增强，不因它拒服务
                    log.warning("story_connect_push_err err=%s", type(e).__name__)
            # both proxy legs now run through the takeover pumps (text-face interception):
            # engine->device strips ```display fences + relays kidpal/display tools/call;
            # device->engine forwards receipts unchanged and consumes them as delivery acks.
            pump_up = asyncio.create_task(pump_upstream_to_client(upstream, websocket, session_id))
            pump_dn = asyncio.create_task(pump_client_to_upstream(websocket, upstream, session_id))
            # 提醒投递面（包 260829-schedule-reminder）：两条腿都在的这段时间登记为投递目标并
            # 起每分钟轮询（到点经 kidpal/alert 推送 + 渲染回执接出队方）；断开即摘除、不攒历史。
            _register_alert_legs(websocket, upstream)
            # [SC-D] 会话归口：同一双腿按会话键登记（sweep 引导/收尾只打归属连接腿,
            # 261021 F3⑤）；与 _register_alert_legs 同生命周期（finally 双摘除）。
            _register_session_legs(session_id, websocket, upstream)
            _ensure_alert_loop()
            # 课表三触发 ①：WS 建连即推（home upcoming 先 + grid restore 附随后；
            # 触发②每日零点/③录入即时 marker 走轮询 piggyback）。失败不阻断建连
            # （读不见 schedule.md 等 -> 静默跳过，_push_schedule_connect 内两推各自
            # try/except）。
            try:
                await _push_schedule_connect(websocket)
            except Exception as e:  # noqa: BLE001 — 建连推送增强，不因它拒服务
                log.warning("schedule_connect_push_err err=%s", type(e).__name__)
            # 故事三时点 ①「建连即推」沿革（260928-story-connect-push-defer 摘除 ->
            # 261010-p3-storylist-cold-start-restore v1.63 赎回）：旧摘除理由 = 设备每会话
            # 重开 WS、建连即推无条件给设备打 story_list 帧，旧固件无条件渲染 P3 页盖掉
            # 聆听态（story_page 零 CJK -> 半截页）。赎回 = data.restore 标记（板端 restore
            # 分流：只填 RAM 列表缓存不接管前台、P3 在屏仅重绘本页）消解该复现路径——见上方
            # ws_connect 块内的建连恢复推送调用（story_restore_enabled 门控）。ctx(story)
            # 同拍条件推送（_consume_kidpal_control 内）保留 = M4K3-A7 冻结齿硬约束。
            # ②（停录归档后）/③（删除执行后）调用点缺省 restore=False 零偏移。
            try:
                # [SC-C] teardown 收尾不悬挂（261021-p3-record-cancel-lifecycle F1）：
                # 原 `await asyncio.gather(pump_up, pump_dn)` 等**双泵**——记录语境下引擎腿
                # 被抑制、无流量，pump_up 阻塞在引擎腿读帧上永不返回；设备丢弃关通道后
                # pump_dn 已结束也不收尾 -> finally 的 cancel / 腿摘除永不执行（zombie
                # 会话 + zombie 腿跨重连泄漏，未停录断开的累积缓冲不释放 = HWV-A11 意图
                # 失效）。改为任一泵先结束即收尾：另一泵取消后限时回收（先完成者即收尾沿）。
                _done, _pending = await asyncio.wait(
                    {pump_up, pump_dn}, return_when=asyncio.FIRST_COMPLETED)
                for _task in _pending:
                    _task.cancel()
                if _pending:
                    await asyncio.gather(*_pending, return_exceptions=True)
                for _task in _done:
                    if _task.cancelled():
                        continue
                    _exc = _task.exception()
                    if _exc is not None:
                        log.warning("ws_pump_ended corr_id=%s err=%s",
                                    session_id, type(_exc).__name__)
            finally:
                _unregister_alert_legs(websocket)
                # [SC-D] 会话腿映射摘除（与 _unregister_alert_legs 同生命周期；按
                # device_ws 身份摘除本连接全部会话登记）。
                _unregister_session_legs(websocket)
                # 包 9b HWV-A11 (F6): 断开钩子释放该连接的累积会话（StoryAccumulator.cancel
                # pop 会话态）——常驻网关下「未停录即断开」的会话不得泄漏累积缓冲。cancel 对
                # 不存在 / 已停录会话幂等返回 False，无副作用。
                try:
                    _get_story_acc().cancel(session_id, reason="ws_disconnect")
                except Exception as e:  # noqa: BLE001 — 清理钩子绝不影响断开收尾
                    log.warning("story_session_teardown corr_id=%s err=%s",
                                session_id, type(e).__name__)
    except Exception as e:  # noqa: BLE001
        log.info("ws forward not available (engine down?): %s", type(e).__name__)
        await websocket.close(code=1011)