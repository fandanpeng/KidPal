#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""backfill_honcho.py — 历史 corpus → Honcho 记忆库照记全量迁移 runner。

TaskPackage: 261022-m5-k4-history-corpus-backfill（M5 列车 P4/K4）。
解析器 v2（2026-10-24，review 修正案）+ created_at 回填 + toolResult 路由：
TaskPackage 261024-m5-k4b-corpus-parser-v2（K4 归因 bug 修复轮，rigor=lite），
技术方案权威 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
k4-attribution-fix-review-20261024.md §1.5（双层识别补丁裁定 KEEP）+ §2.1
（解析器 v2 逐条）+ human D1-D5 五项裁决。v2 = 双层识别（KEEP 基底）+ 续行
归组 + 内嵌 turn 拆分 + created_at 回填 + toolResult 路由 + additive 报告扩展。
上游: docs/requirements.md S-F5.1-AC5 + docs/architecture.md §6.7 +
workstream PROPOSAL §3.1 R6 / §3.2 OQ-3 + arch review PROPOSAL T4 +
ADR-016 + OQ-10 + NFR-SEC-2 ⑤ + NFR-PRIV-1。

豁免声明（K4-E5 · T4「记忆口径分层」）:
  本工具属「历史照记」显式豁免——AC5：孩子以前聊过的历史内容（含同学
  姓名等第三方姓名）不分回合性质全量照记，旧内容与新聊天一视同仁
  （human 2026-10-22 终拍 R6/OQ-3：不带 PII 过滤器；不得以「隐私」为由
  自行缩圈）。本豁免只作用于本工具消费的历史源；新增回合的记忆写入
  资格门不因本工具存在而改变，两条口径不互串——本树零资格门配置引用，
  P3 插件工作台（server/deploy/261022-openclaw-honcho-plugin/）单源不动。

身份口径（K4-E2 · ADR-016 现行口径）:
  · 灌入 session 键全部落既有设备 session 键族：前缀恰
    agent:kidpal:openai-user:<device_id>（后缀 = :backfill:<batch>:<source-kind>）。
    canonical 键形态在鸭型协议层、metadata batch 标记值与计数报告中保真
    不变；仅出网 wire 层经 HonchoRestClient 确定性可逆编码
    （kidpal-bf- + base64url——Honcho 资源名 pattern ^[a-zA-Z0-9_-]+$
    不容 ':'，schemas/api.py:46 实测），同键跨进程编码确定一致。
  · 目标 peer 恒 = 家庭单 owner（--peer 缺省 "owner"）；零新 peer 创建
    （零设备名 peer、零多设备多孩子身份）；换设备 = 新 session 家族、
    画像不变（arch §1.3 已知后果，K4-E6 live 轮知悉项）。

幂等与 live 共生（K4-E3 · notes ④ 判据）:
  · 每条灌入消息 metadata 携带 kidpalBackfillFingerprint =
    v1:<source-kind>:<sha256hex>；指纹 = 源锚身份（源件 + 行/事件序 + 内容），
    同源重跑确定不变；重复内容消息不互吞（同源内零碰撞）。corpus 指纹公式
    v2 换代（review §2.1 ⑤）：锚 = 源文件 | 首行行号 | 段序 | role | 段文本
    （段级身份——归组/拆分后每段独立锚定）；transcript 指纹公式不动；旧批次
    旧指纹随 session 回滚删除自然消失，与新公式零撞车面。
  · 自建 session metadata 携带 kidpalBackfillBatch，值 = 该 session 自身
    键（「标记即键」形态——值形态契约自由；回滚 runner 经
    list_backfill_batches 取回的集合即恰被标记 session 的键集合，无需
    额外枚举面）。
  · 写入前先取既有指纹集合，命中即跳过——二次 apply 恰 ingested=0、
    skipped=全量、零写调用。
  · live 插件在写数据无 backfill 标记 → 永不进指纹集合、永不被误判
    已灌、永不被本工具写入、永不被回滚触碰。

源（只读零改写；plan/apply 全程字节不变，K4-E4）:
  · --corpus-dir  session-corpus 源语料目录。解析器 v2（2026-10-24 review
    修正案 §2.1，human D1 拍板「归组+内嵌拆分」）按四层还原真实回合序列，
    全程无损、零丢弃：
      1. 逐行双层识别（fixer 补丁 KEEP 基底）：行格式两种形态——
         - 单层旧格式 `[<role>] <content>`：任何方括号角色前缀皆识别透传，
           含旧设备流程回合行（如 `[front] ...`）——照记不滤、逐行一条、
           不参与归组与拆分（冻结 K4 最小格式行为逐字保持）；
         - 双层机器格式 `[<会话源引用>] <Role>: <content>`：第一层方括号是
           会话源引用（剥掉，不进 role 也不进 content），角色取第二层
           `User:` / `Assistant:` 前缀（恰一单词 token + 冒号，大小写不敏感；
           归一键 = 小写，确保 assistant 回合路由到 agent peer）。理论上的
           多层源引用按循环剥除。content 自带的 `词:` 或另一角色名不会被
           误判为角色前缀（保守：只认 user/assistant 两 token）。
      2. 续行归组（仅双层行）：同文件 + 同首层 `[<ref>]` + 同第二层 role 的
         连续行合并为一条逻辑消息（换行保留），首行行号作锚——修复 F2 续行
         碎片化（display JSON / 笔画路径不再被拆成多条独立消息）。
      3. 内嵌 turn 拆分（仅双层组）：逻辑消息体按 `(?<=\\s)(User|Assistant): `
         切段；首段沿用外层 role，其后各段按标记定 role。包装标记
         `[Chat messages since your last reply - for context]` /
         `[Current message - respond to this]` 作为协议标签计数进
         wrapperMarkersStripped（review §2.1 ③ 保留变体「不剥、只计数」：
         真语料 243 个组首段恰为包装标记载体——若剥离，这些段变空、冻结
         锚定数 2879 不可达且产生空消息；故标记保留在段文本中、只计数，
         内容零丢弃，段计数与冻结锚逐值等——对账基准 =
         review 实读探针 k4-scripts/split_probe2.py 的 1007→2879 模拟）
         ——修复 F1 主症状（user 消息上下文块里嵌的小伴话术改归 assistant
         段）。assistant 组实读零标记；若出现标记按同规则处理并如实计数，
         不得丢内容。
      4. created_at 回填（human D3）：文件名 `YYYY-MM-DD.txt` → 该日 12:00
         Asia/Shanghai = 04:00 UTC（ISO 8601 带 +00:00 后缀）经
         MessageCreate.created_at 出网；非日期文件名 → created_at=None 走
         服务端写入时刻缺省——修复 F3 时间线被盖成写入日。
    未识别行（无前缀残行）兜底收编为一条消息（role=user、内容=整行）并在
    计数报告 backfill.unparsed 披露——照记全量、零丢弃。
  · --openclaw-db  OpenClaw kidpal agent sqlite：表 transcript_events，
    至少列 id / role / content（列超集容忍；缺表缺列 = 红，exit 1）。
    只读使用（全部语句为 SELECT，零写语句）。

出网（NFR-SEC-2 ⑤）: 数据面只经 --api（缺省 http://127.0.0.1:18100，
Honcho loopback）；deriver→DeepSeek / embedding→DashScope 两条出网记账
在 Honcho 服务端既有体内，本工具零新增出网路径、零内联凭据。
凭据: 可选 HONCHO_JWT 环境变量名字引用（Authorization: Bearer <token>；
token 由 ECS 侧注入，不入仓；服务端 settings.AUTH.USE_AUTH=false 时
零凭据放行 admin 语义——security.py auth() 实测）。
零过滤类旗标: redact / mask / anonymize / pii / filter- 族一律不在场
（human 终拍照记全量）。

exit 码: 0 = 成功（含 plan 干跑）；1 = 运行/数据错误（源不在世、缺表
缺列、API 不可达、报告落盘失败——先于或隔离于写入显式失败，不裸崩）；
2 = 用法错（缺 --device-id、零数据源、未知旗标——argparse 同码）。

计数报告（--report-out，K4-E3 · M5 收口轮 workstream 对账件）: plan/apply
两态都落盘；顶层键 mode + peers/sessions/messages/observations 四计数器
before/after + backfill.ingested/skipped/unparsed/perSource；apply 态
after = 写后快照，plan 态 after = 预测（messages.after = before + ingested；
observations 的 deriver 异步增量在 live 轮对账——K4-E6）。解析器 v2 报告
扩展（K4B-E3，additive、顶层键零破坏）：perSource[kind] 增 perRole
{user, assistant, other} / groups / segments / wrapperMarkersStripped /
corpusFiles[{name, sha256, lines}]；backfill 顶层增 perRoleTotal。

执行面: human 在 ECS 全量跑（K4-E6 live structural 项 = ECS 全量跑 +
计数报告过目 + 幂等二次跑零增量 + 清库回滚演练 + 画像抽查；挂
awaiting-human:live-deploy，Pod 不自行部署）。

minimal: 真 REST 适配（HonchoRestClient）的端点与 wire 形状已按 Honcho
upstream 源码对读实测落笔（对读源 = plastic-labs/honcho main@7478be29
本地克隆 C:/tmp/honcho-upstream-ref/honcho + 协调者 ECS /opt/honcho 现读
特征逐条吻合：全 router prefix /v3、routers 全 workspace 作用域）；端点
路径集中类常量。逐端点源码行锚：sessions routers/sessions.py:40-43
（prefix）+ :305（POST "" = get-or-create）+ :418（DELETE 202 异步级联）、
peers routers/peers.py:87（POST /list）+ :124（POST "" get-or-create）、
messages routers/messages.py:38（prefix）+ :102（批量上限 100，201）、
conclusions routers/conclusions.py:22（prefix）+ :61（POST /list 分页）、
schemas/api.py:46（RESOURCE_NAME_PATTERN）+ :342（MessageCreate alias
peer_id）+ :394（MessageBatchCreate ≤100）+ :420（SessionCreate alias
id/peers）、utils/pagination.py（POST /list 分页 Page.items，size ≤ 100）、
security.py:260-274（Bearer；USE_AUTH=false 放行）。——metadata 逐消息
可达性 = live 全量跑首验点（冻结件缺口申报保留）。若 message metadata
不可达，按冻结件缺口申报换 session 侧指纹清单方案并走 tighten-only
反审。升级路径：本类恰一 _request 出入口；_add_messages 已按 100 条
分块建模，runner 逐条调用可在不破冻结鸭型协议下升级为批量。

stdlib-only；模块顶层零副作用、零网络 I/O；可被 importlib 按路径导入。
"""

from __future__ import annotations

import argparse
import base64
import hashlib
import inspect
import json
import os
import re
import sqlite3
import sys
import urllib.error
import urllib.parse
import urllib.request
from datetime import datetime, timedelta, timezone
from pathlib import Path

DEFAULT_API = "http://127.0.0.1:18100"
SESSION_BATCH_KEY = "kidpalBackfillBatch"
MESSAGE_FP_KEY = "kidpalBackfillFingerprint"
SOURCE_KINDS = ("session-corpus", "openclaw-transcript")
DEVICE_FAMILY_PREFIX = "agent:kidpal:openai-user:"
DEFAULT_PEER = "owner"
# assistant 角色消息在 Honcho 侧的 peer 归宿（ADR-016：DB peers = owner /
# agent-kidpal 两员）；其余角色一律归目标 owner peer。
AGENT_PEER = "agent-kidpal"
# 路由 agent peer 的角色集（review §2.1 ⑦ / F4：toolResult 及未来 tool* 一并
# 归 agent——课表文件正文类 toolResult 归 owner 会派生「owner has a 课表」类
# 伪事实）。比较经 role 小写归一。
_AGENT_ROUTE_ROLES = frozenset({"assistant", "toolresult", "tool"})
# 双层组内嵌上下文块的协议包装标记（review §2.1 ③ / F1）——计数进
# wrapperMarkersStripped（保留变体「不剥、只计数」，见 _strip_wrapper_markers
# 注：剥离会使 243 个包装标载体段变空、冻结锚定数 2879 不可达）；协议标签，
# 非孩子/小伴话语。
_WRAPPER_MARKERS = (
    "[Chat messages since your last reply - for context]",
    "[Current message - respond to this]",
)
# 内嵌 turn 切段标记（review §2.1 ③）：前置空白 + User/Assistant token + 冒号
# 空格。(?<=\s) 保段首 token 不误拆（content 自带 `词:` / 行首 `User:` 首段
# 沿用外层 role），capture 组供切段后定 role。
_TURN_MARK_RE = re.compile(r"(?<=\s)(User|Assistant): ")
# wire 层 session 名编码前缀（wire_session_name / canonical_session_name）——
# canonical 键含 ':' 不合 Honcho 资源名 pattern，出网名 = 前缀 + base64url。
WIRE_NAME_PREFIX = "kidpal-bf-"


class BackfillError(Exception):
    """运行/数据错误（exit 1）；status 承载 HTTP 状态码（可缺省）。"""

    def __init__(self, message: str, status: int | None = None):
        super().__init__(message)
        self.status = status


class UsageError(Exception):
    """用法错（exit 2）。"""


# --------------------------------------------------------------------------- #
# 指纹 / session 键（K4-E3 / K4-E2 冻结格式）
# --------------------------------------------------------------------------- #


def fingerprint(source_kind: str, *anchor: str) -> str:
    """v1:<source-kind>:<sha256hex>——源锚身份（源件 + 行/事件序 + 内容）。

    同一源重跑逐位稳定；锚含位置（行号/事件 id），重复内容消息不互吞。
    """
    payload = "\x1f".join((source_kind, *anchor))
    digest = hashlib.sha256(payload.encode("utf-8")).hexdigest()
    return f"v1:{source_kind}:{digest}"


def session_key_for(device_id: str, batch: str, source_kind: str) -> str:
    """灌入 session 键——既有设备 session 键族 + backfill 后缀（K4-E2）。"""
    return f"{DEVICE_FAMILY_PREFIX}{device_id}:backfill:{batch}:{source_kind}"


def utc_batch_tag() -> str:
    """run batch 标记（UTC 秒级）；同时充当本次运行的 correlationId。"""
    return datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")


# --------------------------------------------------------------------------- #
# 源解析（只读；零过滤、零丢弃）
# --------------------------------------------------------------------------- #


# 双层机器格式第二层只认这两个机器角色 token（大小写不敏感）——比
# 「任意 `Word:`」更保守，避免把 content 自带的 `词:` 误判成角色前缀而
# 破坏单层旧格式逐字兼容（2026-10-24 解析归因修复）。
_SECOND_LAYER_ROLES = frozenset({"user", "assistant"})


def _without_leading_bracket_layers(text: str) -> str:
    """剥去行首任意层 `[...]` 源引用层（连同尾随单空格），返回剩余。

    循环剥除处理理论上的多层源引用；遇到 `[]`/未闭合方括号即停手。
    """
    while text.startswith("["):
        end = text.find("]")
        if end <= 1:
            break
        text = text[end + 1:]
        if text.startswith(" "):
            text = text[1:]
    return text


def _second_layer_role(text: str) -> tuple[str, str] | None:
    """`User:` / `Assistant:` 前缀（恰一单词 token + 冒号，大小写不敏感）。

    命中 → (小写角色, 冒号后内容去一个可选空格)；否则 None（保守不剥）。
    只认冒号前的整 token ∈ {user, assistant}，content 内的 `词:` 不误判。
    """
    colon = text.find(":")
    if colon <= 0:
        return None
    head = text[:colon]
    if head.lower() not in _SECOND_LAYER_ROLES:
        return None
    content = text[colon + 1:]
    if content.startswith(" "):
        content = content[1:]
    return head.lower(), content


def split_corpus_line(line: str) -> tuple[str, str, bool]:
    """`[<role>] <content>` 或 `[<源引用>] <Role>: <content>` → (role, content, recognized)。

    解析器 v2 的逐行识别基底（fixer 补丁 KEEP，review §1.5）：归组与拆分
    见 parse_corpus_dir / _classify_corpus_line。单层旧格式 `[<role>]
    <content>`：逐字保持既有行为（任何方括号角色前缀透传，含旧设备流程
    回合行——照记不滤）。双层机器格式 `[<会话源引用>] User:/Assistant:
    <content>`：第一层方括号是源引用（剥掉，不进 role 也不进 content），
    角色取第二层 `User:`/`Assistant:` 前缀（大小写不敏感，归一键 = 小写
    ——确保 assistant 回合路由到 agent peer，修复 live 实证的归因错）。
    未识别行返回 (user, 整行, False)——兜底收编，不静默丢弃。
    """
    _ref, role, content, recognized, _double = _classify_corpus_line(line)
    return role, content, recognized


def _classify_corpus_line(line: str) -> tuple[str | None, str, str, bool, bool]:
    """逐行分类 → (ref, role, content, recognized, double_layer)。

    ref = 双层行首层方括号内容（归组键的组成部分；单层/未识别行 = None）；
    double_layer = 是否双层机器格式（归组与内嵌拆分只对双层行启用——单层
    旧格式逐行一条、逐字保持冻结 K4 行为）。
    """
    if line.startswith("["):
        end = line.find("]")
        if end > 1:
            rest = line[end + 1:]
            if rest.startswith(" "):
                rest = rest[1:]
            ref = line[1:end]
            # 双层：剥去首层及后续方括号源引用层后，剩余以 User:/Assistant:
            # token 起 → 角色取第二层（首层方括号即源引用）。
            second = _second_layer_role(_without_leading_bracket_layers(rest))
            if second is not None:
                return ref, second[0], second[1], True, True
            # 单层旧格式（含 `[user] 内容: 冒号` / `[front] ...`）逐字保持。
            return None, (ref.strip() or "user"), rest, True, False
    return None, "user", line, False, False


def _strip_wrapper_markers(text: str) -> tuple[str, int]:
    """协议包装标记计数（review §2.1 ③ 保留变体「不剥、只计数」）。

    返回 (原文原样, 标记出现次数)——标记保留在文本中不剥离（真语料 243 个
    组首段恰为包装标记载体：剥离会使段变空、冻结锚定数 2879 不可达且产生
    空消息；对账基准 = review 探针 split_probe2.py 保留变体）；计数进
    wrapperMarkersStripped 报告键（键名为冻结面，语义 = 协议标签出现数）。
    """
    count = 0
    for marker in _WRAPPER_MARKERS:
        count += text.count(marker)
    return text, count


def _split_embedded_turns(body: str, outer_role: str) -> list[tuple[str, str]]:
    """逻辑消息体按内嵌 turn 标记切段（review §2.1 ③）→ [(role, 段文本)]。

    首段沿用外层 role；其后各段按 `(?<=\\s)(User|Assistant): ` 标记定 role
    （小写归一）。切段标记本身（token + `: `）是协议成分，不入段文本；
    用户组恒启用，assistant 组实读零标记、若出现按同规则处理并如实分段
    （不得丢内容）。段文本内的 `词:` 等非 User/Assistant token 不误拆。
    """
    parts = _TURN_MARK_RE.split(body)
    segments = [(outer_role, parts[0])]
    for marker, seg in zip(parts[1::2], parts[2::2]):
        segments.append((marker.lower(), seg))
    return segments


def corpus_created_at(file_stem: str) -> str | None:
    """语料文件名 `YYYY-MM-DD` → created_at 回填值（human D3 口径）。

    该日 12:00 Asia/Shanghai = 04:00 UTC，ISO 8601 带 +00:00 后缀
    （如 2026-09-30T04:00:00+00:00，单测钉死）；非日期文件名（含非法日期
    如 2026-13-99）→ None，走服务端写入时刻缺省。
    """
    if not re.fullmatch(r"\d{4}-\d{2}-\d{2}", file_stem or ""):
        return None
    try:
        day = datetime.strptime(file_stem, "%Y-%m-%d").date()
    except ValueError:
        return None
    local_noon = datetime(day.year, day.month, day.day, 12, 0,
                          tzinfo=timezone(timedelta(hours=8)))
    return local_noon.astimezone(timezone.utc).isoformat()


def parse_corpus_dir(corpus_dir: Path) -> tuple[list[dict], int, dict]:
    """session-corpus 源语料 → 计划消息列表（解析器 v2）。

    四层还原（头注「源」节）：逐行双层识别 → 续行归组（同文件 + 同首层
    `[<ref>]` + 同第二层 role 的连续行合并，换行保留，首行行号作锚；空行
    不隔断组——组键连续性只看非空行序列）→ 内嵌 turn 拆分 + 包装标记计数
    （仅双层组；标记保留变体，见 _strip_wrapper_markers）→ created_at 回填。
    单层旧格式与未识别行逐行一条、不归组不拆分（冻结 K4 最小格式行为逐字
    保持）。只读——不写不改不删源。
    返回 (messages, unparsed_count, stats)：每条 message 含 role / content /
    fingerprint / unparsed / created_at / segment_index 六键；stats 含
    groups（逻辑组数）/ wrapperMarkersStripped（包装标记剥离计数）/
    corpusFiles（逐文件 {name, sha256, lines}——lines = 非空行数，只读
    计算，快照审计对账面）。
    """
    messages: list[dict] = []
    unparsed = 0
    stats: dict = {"groups": 0, "wrapperMarkersStripped": 0, "corpusFiles": []}
    for path in sorted(p for p in corpus_dir.rglob("*") if p.is_file()):
        rel = path.relative_to(corpus_dir).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        raw_lines = text.splitlines()
        nonempty = [(no, raw.strip()) for no, raw in
                    enumerate(raw_lines, start=1) if raw.strip()]
        stats["corpusFiles"].append({
            "name": rel,
            "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
            "lines": len(nonempty),
        })
        created_at = corpus_created_at(path.stem)
        # 续行归组：同 (ref, role) 的连续双层行合并；单层/未识别行自成一组。
        groups: list[dict] = []
        for line_no, line in nonempty:
            ref, role, content, recognized, double = _classify_corpus_line(line)
            prev = groups[-1] if groups else None
            if (double and prev is not None and prev["double"]
                    and prev["ref"] == ref and prev["role"] == role):
                prev["contents"].append(content)  # 换行保留，锚 = 首行行号
                continue
            groups.append({
                "ref": ref, "role": role, "anchor": line_no,
                "contents": [content], "recognized": recognized,
                "double": double,
            })
        for group in groups:
            stats["groups"] += 1
            if not group["recognized"]:
                unparsed += 1
            body = "\n".join(group["contents"])
            if group["double"]:
                body, stripped = _strip_wrapper_markers(body)
                stats["wrapperMarkersStripped"] += stripped
                segments = _split_embedded_turns(body, group["role"])
            else:
                segments = [(group["role"], body)]
            seg_index = 0
            for seg_role, seg_text in segments:
                seg_text = seg_text.strip()
                if not seg_text:
                    continue  # 纯标记/空段（真语料实读零空段；不计内容丢失）
                messages.append({
                    "role": seg_role,
                    "content": seg_text,
                    # 指纹公式 v2（review §2.1 ⑤）：段级锚 = 源文件 | 首行
                    # 行号 | 段序 | role | 段文本——逐位稳定、重复内容不互吞。
                    "fingerprint": fingerprint(
                        "session-corpus", rel, str(group["anchor"]),
                        str(seg_index), seg_role, seg_text),
                    "unparsed": not group["recognized"],
                    "created_at": created_at,
                    "segment_index": seg_index,
                })
                seg_index += 1
    return messages, unparsed, stats


def parse_transcript_db(db_path: Path) -> list[dict]:
    """OpenClaw transcript_events → 计划消息列表。

    表 transcript_events 至少列 id / role / content（列超集容忍）；
    缺表缺列 = 红（BackfillError）。只读使用——零写语句。
    """
    if not db_path.is_file():
        raise BackfillError(f"--openclaw-db 不在世: {db_path}")
    con = sqlite3.connect(str(db_path))
    messages: list[dict] = []
    try:
        try:
            cursor = con.execute(
                "SELECT id, role, content FROM transcript_events ORDER BY id")
        except sqlite3.Error as exc:
            raise BackfillError(
                f"transcript_events 表/列缺失或不可读（缺表缺列 = 红）: {exc}") from exc
        for event_id, role, content in cursor:
            role_text = "user" if role is None else str(role)
            content_text = "" if content is None else str(content)
            messages.append({
                "role": role_text,
                "content": content_text,
                "fingerprint": fingerprint(
                    "openclaw-transcript", db_path.name, str(event_id),
                    role_text, content_text),
                "unparsed": False,
                "created_at": None,  # transcript 源零回填（D2 裁不重灌）
                "segment_index": 0,
            })
    finally:
        con.close()
    return messages


def plan_sources(args: argparse.Namespace) -> tuple[dict[str, list[dict]], dict]:
    """双源解析（两源至少其一已在用法校验保证）。

    返回 (plan, parse_stats)：plan 按 source-kind 分组为消息列表；
    parse_stats[kind] = 解析级统计（groups / wrapperMarkersStripped /
    corpusFiles——segments 与 perRole 由消息列表派生，报告扩展 K4B-E3）。
    """
    plan: dict[str, list[dict]] = {}
    parse_stats: dict[str, dict] = {}
    if args.corpus_dir:
        corpus_dir = Path(args.corpus_dir)
        if not corpus_dir.is_dir():
            raise BackfillError(f"--corpus-dir 不在世或非目录: {corpus_dir}")
        msgs, _unparsed, stats = parse_corpus_dir(corpus_dir)
        plan["session-corpus"] = msgs
        parse_stats["session-corpus"] = stats
    if args.openclaw_db:
        msgs = parse_transcript_db(Path(args.openclaw_db))
        plan["openclaw-transcript"] = msgs
        # transcript 源逐事件一条逻辑消息：零归组、零拆分、零包装标记、
        # 零语料文件清单（corpusFiles 恒空表——additive 键形状统一）。
        parse_stats["openclaw-transcript"] = {
            "groups": len(msgs),
            "wrapperMarkersStripped": 0,
            "corpusFiles": [],
        }
    return plan, parse_stats


# --------------------------------------------------------------------------- #
# 真 REST 适配（离线验收以记录桩注入 build_client 接缝，不经本类）
# --------------------------------------------------------------------------- #


def wire_session_name(canonical_key: str) -> str:
    """canonical session 键 → wire 合法资源名。

    Honcho 资源名 pattern = ^[a-zA-Z0-9_-]+$（schemas/api.py:46）——canonical
    键含 ':' 不合 wire；编码 = kidpal-bf- 前缀 + base64url（去 padding），
    可逆且同键确定不变（幂等/回滚跨进程一致）。canonical 键在 metadata
    batch 标记值、消息指纹与计数报告中保真，wire 名仅出网层局部形态。
    """
    encoded = base64.urlsafe_b64encode(canonical_key.encode("utf-8")).decode("ascii")
    return WIRE_NAME_PREFIX + encoded.rstrip("=")


def canonical_session_name(wire_name: str) -> str | None:
    """wire 名反解 canonical 键；非本前缀（live 插件等他源 session）→ None。"""
    if not wire_name.startswith(WIRE_NAME_PREFIX):
        return None
    raw = wire_name[len(WIRE_NAME_PREFIX):]
    return base64.urlsafe_b64decode(
        raw + "=" * (-len(raw) % 4)).decode("utf-8")


class HonchoRestClient:
    """Honcho REST 适配（真实现；stdlib urllib；wire 形状按 upstream 源码实测）。

    对读源：plastic-labs/honcho main@7478be29 本地克隆
    （C:/tmp/honcho-upstream-ref/honcho——src/main.py:216-223 全 router
    prefix /v3；sessions/peers/messages/conclusions 四 routers 全 workspace
    作用域），与协调者 ECS /opt/honcho 现读特征逐条吻合。逐端点源码行锚
    见模块头注 minimal 段。目标 workspace = kidpal（与插件 config.workspaceId
    同源）。

    wire 形态要点（全部源码实测，非「最佳知识」）：
      · 资源名 pattern ^[a-zA-Z0-9_-]+$（schemas/api.py:46）——canonical
        session 键含 ':'，出网名 = wire_session_name() 确定性可逆编码；
      · 建会话 = get-or-create 语义（POST ""，200/201 即得，无 409 分支）；
      · 消息批 {"messages": [...]} 上限 100 条/批（MessageBatchCreate），
        超量自动分块；单条 MessageCreate 的 peer 字段 wire 名 = peer_id
        （MessageCreate.peer_name 的 alias，schemas/api.py:342-347）；
      · 列举 = POST /list 分页（Page.items；size 上限 100 逐页走）；
      · DELETE session 返回 202（标记 inactive 即时、真删异步；list 只回
        active——crud/session.py:183 is_active 过滤实测）；
      · 凭据 = HONCHO_JWT 环境变量名字引用（token 由 ECS 侧注入，不入仓）；
        服务端 USE_AUTH=false 时零凭据放行（security.py:272 实测）。

    minimal: 快照/指纹枚举按全量拉取建模（现网量级小；size=100 逐页）；
    metadata 逐消息可达性 = live 全量跑首验点（冻结件缺口申报保留）；若
    message metadata 不可达，按冻结件缺口申报换 session 侧指纹清单方案并
    走 tighten-only 反审。快照计数对服务端异步派生（observations）含等待
    窗差——live 轮对账见 K4-E6。升级路径：本类恰一 _request 出入口；
    _add_messages 已按 100 条/批分块建模，runner 逐条调用可在不破冻结
    鸭型协议（六方法）下升级为批量调用。
    """

    WS = "kidpal"
    EP_SESSIONS = "/v3/workspaces/{ws}/sessions"                # POST = get-or-create
    EP_SESSIONS_LIST = EP_SESSIONS + "/list"                    # POST 分页列举
    EP_SESSION = "/v3/workspaces/{ws}/sessions/{sid}"           # DELETE（202 异步删）
    EP_SESSION_MESSAGES = "/v3/workspaces/{ws}/sessions/{sid}/messages"  # POST 批量写
    EP_SESSION_MESSAGES_LIST = EP_SESSION_MESSAGES + "/list"    # POST 分页列举
    EP_PEERS_LIST = "/v3/workspaces/{ws}/peers/list"            # POST 分页列举
    EP_CONCLUSIONS_LIST = "/v3/workspaces/{ws}/conclusions/list"  # POST 分页列举

    MESSAGE_BATCH_LIMIT = 100  # MessageBatchCreate max_length=100（api.py:394）
    LIST_PAGE_SIZE = 100       # pagination size le=100（utils/pagination.py）
    JWT_ENV_VAR = "HONCHO_JWT"  # token 名字引用；ECS 侧注入，不入仓

    def __init__(self, api: str):
        self._base = api.rstrip("/")
        self._peer_by_key: dict[str, str] = {}

    # -- HTTP 出入口（恰一处；出网目的地 = 构造参数 api，即 --api loopback）-- #
    def _request(self, method: str, path: str, *, query=None, body=None):
        url = self._base + path
        if query:
            url += "?" + urllib.parse.urlencode(query)
        data = None
        headers = {"Accept": "application/json"}
        token = os.environ.get(self.JWT_ENV_VAR)
        if token:
            headers["Authorization"] = "Bearer " + token
        if body is not None:
            data = json.dumps(body, ensure_ascii=False).encode("utf-8")
            headers["Content-Type"] = "application/json"
        req = urllib.request.Request(url, data=data, headers=headers, method=method)
        try:
            with urllib.request.urlopen(req, timeout=30) as resp:
                raw = resp.read()
        except urllib.error.HTTPError as exc:
            try:
                detail = exc.read(200)
            except Exception:  # noqa: BLE001 —— 响应体读取失败不掩盖状态码
                detail = b""
            raise BackfillError(
                f"Honcho API HTTP {exc.code} {method} {path}: {detail!r}",
                status=exc.code) from exc
        except urllib.error.URLError as exc:
            raise BackfillError(
                f"Honcho API 不可达（{self._base}）: {exc.reason}") from exc
        return json.loads(raw.decode("utf-8")) if raw else None

    @staticmethod
    def _items(payload) -> list:
        """Page 分页响应取 items（Page[...] 响应形状——utils/pagination.py）。"""
        if isinstance(payload, dict) and isinstance(payload.get("items"), list):
            return payload["items"]
        return []

    def _list_all(self, path: str) -> list:
        """POST /list 分页走全量（page/size 偏移翻页，至不足一页或齐 total）。"""
        items: list = []
        page = 1
        while True:
            payload = self._request(
                "POST", path, query={"page": page, "size": self.LIST_PAGE_SIZE})
            batch = self._items(payload)
            items.extend(batch)
            if len(batch) < self.LIST_PAGE_SIZE:
                return items
            total = payload.get("total") if isinstance(payload, dict) else None
            if isinstance(total, int) and len(items) >= total:
                return items
            page += 1

    def snapshot_counts(self) -> dict:
        ws = self.WS
        peers = self._list_all(self.EP_PEERS_LIST.format(ws=ws))
        sessions = self._list_all(self.EP_SESSIONS_LIST.format(ws=ws))
        messages = 0
        for session in sessions:
            sid = urllib.parse.quote(str(session.get("id") or ""))
            messages += len(self._list_all(
                self.EP_SESSION_MESSAGES_LIST.format(ws=ws, sid=sid)))
        observations = len(self._list_all(
            self.EP_CONCLUSIONS_LIST.format(ws=ws)))
        return {
            "peers": len(peers),
            "sessions": len(sessions),
            "messages": messages,
            "observations": observations,
        }

    def list_backfill_batches(self) -> set:
        """既有 backfill batch 标记集合——标记即键形态下 = 被标记 session 键集。"""
        batches = set()
        for session in self._list_all(self.EP_SESSIONS_LIST.format(ws=self.WS)):
            metadata = session.get("metadata") or {}
            if SESSION_BATCH_KEY in metadata:
                batches.add(str(metadata[SESSION_BATCH_KEY]))
        return batches

    def list_fingerprints(self) -> set:
        """既有消息指纹集合（live 数据无标记 → 天然不在集合内）。"""
        fingerprints = set()
        for session in self._list_all(self.EP_SESSIONS_LIST.format(ws=self.WS)):
            sid = urllib.parse.quote(str(session.get("id") or ""))
            for message in self._list_all(
                    self.EP_SESSION_MESSAGES_LIST.format(ws=self.WS, sid=sid)):
                metadata = message.get("metadata") or {}
                if MESSAGE_FP_KEY in metadata:
                    fingerprints.add(str(metadata[MESSAGE_FP_KEY]))
        return fingerprints

    def create_session(self, peer, session_key, metadata) -> None:
        canonical = str(session_key)
        self._peer_by_key[canonical] = str(peer)
        body = {
            "id": wire_session_name(canonical),  # SessionCreate.name 的 alias id
            "metadata": dict(metadata),
            "peers": {str(peer): {}},            # SessionCreate.peer_names 的 alias peers
        }
        # POST sessions = get-or-create 语义（routers/sessions.py:305）——重复
        # create 返回既有 session（200/201），无 409 分支。
        self._request("POST", self.EP_SESSIONS.format(ws=self.WS), body=body)

    def add_message(self, session_key, role, content, metadata,
                    created_at=None) -> None:
        """单条消息写入（鸭型协议；created_at 可选——review §2.1 ④ / human D3）。

        created_at 非 None 时进 MessageCreate.created_at 字段
        （schemas/api.py:347 实测支持回填）；None = 服务端写入时刻缺省
        （旧行为，向后兼容）。peer 路由：role ∈ {assistant, toolResult,
        tool}（小写归一）→ AGENT_PEER；其余 → session 目标 owner peer。
        """
        session_key = str(session_key)
        peer = (AGENT_PEER if str(role).lower() in _AGENT_ROUTE_ROLES
                else self._peer_by_key.get(session_key, DEFAULT_PEER))
        row = {
            "content": str(content),
            "peer_id": peer,  # MessageCreate.peer_name 的 alias peer_id（api.py:342）
            "metadata": dict(metadata),
        }
        if created_at is not None:
            row["created_at"] = str(created_at)
        self._add_messages(session_key, [row])

    def _add_messages(self, session_key: str, rows: list[dict]) -> None:
        """批量写消息（MessageBatchCreate：{"messages":[...]} 上限 100 条/批）。

        超过 MESSAGE_BATCH_LIMIT 自动按 100 分块（api.py:394 实测）；当前
        runner 逐条调用 = 每批恰一条。wire 会话名 = 确定性编码
        （wire_session_name），消息作者 peer 由服务端 get-or-create
        （crud/message.py:434 实测）。
        """
        sid = urllib.parse.quote(wire_session_name(session_key))
        for start in range(0, len(rows), self.MESSAGE_BATCH_LIMIT):
            self._request(
                "POST", self.EP_SESSION_MESSAGES.format(ws=self.WS, sid=sid),
                body={"messages": rows[start:start + self.MESSAGE_BATCH_LIMIT]})

    def delete_sessions(self, session_keys) -> int:
        deleted = 0
        for key in session_keys:
            sid = urllib.parse.quote(wire_session_name(str(key)))
            try:
                # DELETE 返回 202（标记 inactive 即时、真删异步级联）。
                self._request("DELETE", self.EP_SESSION.format(ws=self.WS, sid=sid))
            except BackfillError as exc:
                if exc.status == 404:
                    continue  # 不在世即视作已删（幂等回滚）
                raise
            deleted += 1
        return deleted


def build_client(api: str):
    """恰一位置参 → 鸭型协议客户端（离线验收以记录桩注入本接缝）。"""
    return HonchoRestClient(api)


def client_supports_created_at(add_message) -> bool:
    """鸭型协议兼容探测：add_message 是否接受 created_at 可选参。

    向后兼容面（review §2.1 ④「验收桩同步」）：真客户端与已同步的新桩
    支持该参 → runner 传 created_at 出网；未同步的旧四参桩（冻结 K4 验收
    桩形态 add_message(session_key, role, content, metadata)）不支持 →
    runner 省略该参、行为与旧版逐位一致（离线 fixture 无 created_at 断言
    面——回填断言由同步后的验收桩承载）。**kwargs 桩视作支持。
    """
    try:
        params = inspect.signature(add_message).parameters
    except (TypeError, ValueError):
        return False
    if "created_at" in params:
        return True
    return any(p.kind is inspect.Parameter.VAR_KEYWORD
               for p in params.values())


# --------------------------------------------------------------------------- #
# CLI 与主流程
# --------------------------------------------------------------------------- #


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="backfill_honcho.py",
        description="历史 corpus → Honcho 记忆库照记全量迁移"
                    "（plan 干跑缺省，--apply 显式写）")
    parser.add_argument("--corpus-dir", default=None,
                        help="session-corpus 源语料目录（只读；解析器 v2："
                             "双层识别+续行归组+内嵌拆分+created_at 回填）")
    parser.add_argument("--openclaw-db", default=None,
                        help="OpenClaw kidpal agent sqlite（transcript_events，只读）")
    parser.add_argument("--api", default=DEFAULT_API,
                        help=f"Honcho API base（缺省 {DEFAULT_API} loopback）")
    parser.add_argument("--device-id", default=None,
                        help="设备键族归属（必填；session 键前缀 "
                             f"{DEVICE_FAMILY_PREFIX}<device_id>）")
    parser.add_argument("--peer", default=DEFAULT_PEER,
                        help=f"目标 peer（缺省 {DEFAULT_PEER}，家庭单 owner，ADR-016）")
    parser.add_argument("--report-out", default=None,
                        help="计数报告 JSON 落盘路径（K4-E3 对账件；缺省打印 stdout）")
    parser.add_argument("--apply", action="store_true",
                        help="显式写入；缺省 = plan 干跑（零写副作用）")
    return parser


def emit_report(report: dict, report_out: str | None) -> None:
    text = json.dumps(report, ensure_ascii=False, indent=2)
    if report_out:
        path = Path(report_out)
        if not path.parent.exists():
            path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text + "\n", encoding="utf-8")
    else:
        print(text, flush=True)


def run_backfill(args: argparse.Namespace) -> int:
    # 用法校验先于任何客户端构建与写入（exit 2 = 用法错）。
    problems = []
    if not args.device_id:
        problems.append("缺 --device-id（设备键族归属，必填）")
    if not args.corpus_dir and not args.openclaw_db:
        problems.append("零数据源：--corpus-dir 与 --openclaw-db 至少其一")
    if problems:
        raise UsageError("；".join(problems))

    batch = utc_batch_tag()
    mode = "apply" if args.apply else "plan"
    print(f"[kidpal-backfill] batch={batch} mode={mode} api={args.api}",
          flush=True)

    # 源解析（只读；缺表缺列等数据错在此显式失败，先于任何写入）。
    plan, parse_stats = plan_sources(args)

    client = build_client(args.api)
    before = client.snapshot_counts()
    existing_fps = client.list_fingerprints()
    # 鸭型兼容探测（头注 B 节接缝）：created_at 只对支持的客户端出网
    # （真客户端/已同步桩支持；未同步旧四参桩省略该参，行为不变）。
    send_created_at = client_supports_created_at(client.add_message)

    def per_role_counts(msgs: list[dict]) -> dict:
        counts = {"user": 0, "assistant": 0, "other": 0}
        for m in msgs:
            role = str(m["role"])
            counts[role if role in counts else "other"] += 1
        return counts

    ingested = 0
    skipped = 0
    per_source: dict[str, dict] = {}
    per_role_total = {"user": 0, "assistant": 0, "other": 0}
    write_sessions: list[tuple[str, list[dict]]] = []

    for kind in SOURCE_KINDS:
        msgs = plan.get(kind)
        if msgs is None:
            continue  # 该源未提供（两源至少其一已校验）
        stats = parse_stats.get(kind, {})
        per_role = per_role_counts(msgs)
        for bucket in per_role_total:
            per_role_total[bucket] += per_role[bucket]
        stat = {
            "planned": len(msgs),
            "ingested": 0,
            "skipped": 0,
            "unparsed": sum(1 for m in msgs if m["unparsed"]),
            # additive 报告扩展（K4B-E3，review §2.1 ⑥）：顶层键零破坏。
            "perRole": per_role,
            "groups": int(stats.get("groups", len(msgs))),
            "segments": len(msgs),
            "wrapperMarkersStripped": int(
                stats.get("wrapperMarkersStripped", 0)),
            "corpusFiles": list(stats.get("corpusFiles", [])),
        }
        pending: list[dict] = []
        for message in msgs:
            if message["fingerprint"] in existing_fps:
                skipped += 1
                stat["skipped"] += 1
                continue
            pending.append(message)
            ingested += 1
            stat["ingested"] += 1
        per_source[kind] = stat
        if pending:
            write_sessions.append(
                (session_key_for(args.device_id, batch, kind), pending))

    if mode == "apply":
        for key, msgs in write_sessions:
            client.create_session(args.peer, key, {SESSION_BATCH_KEY: key})
            for message in msgs:
                metadata = {MESSAGE_FP_KEY: message["fingerprint"]}
                if send_created_at:
                    client.add_message(
                        key, message["role"], message["content"], metadata,
                        created_at=message["created_at"])
                else:
                    client.add_message(
                        key, message["role"], message["content"], metadata)
        after = client.snapshot_counts()
    else:
        after = {
            "peers": before["peers"],
            "sessions": before["sessions"] + len(write_sessions),
            "messages": before["messages"] + ingested,
            "observations": before["observations"],
        }

    report = {
        "mode": mode,
        "peers": {"before": before["peers"], "after": after["peers"]},
        "sessions": {"before": before["sessions"], "after": after["sessions"]},
        "messages": {"before": before["messages"], "after": after["messages"]},
        "observations": {"before": before["observations"],
                         "after": after["observations"]},
        "backfill": {
            "ingested": ingested,
            "skipped": skipped,
            "unparsed": sum(s["unparsed"] for s in per_source.values()),
            "perSource": per_source,
            "perRoleTotal": per_role_total,  # additive（K4B-E3）
            "batch": batch,
            "targetPeer": args.peer,
            "deviceSessionFamily": DEVICE_FAMILY_PREFIX + args.device_id,
            "sessionKeys": [key for key, _ in write_sessions],
        },
    }
    emit_report(report, args.report_out)
    print(f"[kidpal-backfill] batch={batch} mode={mode} ingested={ingested} "
          f"skipped={skipped} "
          f"unparsed={report['backfill']['unparsed']}", flush=True)
    return 0


def main(argv: list | None = None) -> int:
    """exit 0 = 成功（含 plan）；2 = 用法错；1 = 运行/数据错误。不裸崩。"""
    try:
        return run_backfill(build_arg_parser().parse_args(
            list(sys.argv[1:] if argv is None else argv)))
    except UsageError as exc:
        print(f"USAGE ERROR: {exc}", file=sys.stderr, flush=True)
        return 2
    except BackfillError as exc:
        print(f"ERROR: {exc}", file=sys.stderr, flush=True)
        return 1
    except Exception as exc:  # noqa: BLE001 —— 兜底：迁移工具不得裸异常崩溃
        print(f"ERROR: 未预期失败: {exc!r}", file=sys.stderr, flush=True)
        return 1


if __name__ == "__main__":
    sys.exit(main())
