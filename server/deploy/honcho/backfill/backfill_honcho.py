#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""backfill_honcho.py — 历史 corpus → Honcho 记忆库照记全量迁移 runner。

TaskPackage: 261022-m5-k4-history-corpus-backfill（M5 列车 P4/K4）。
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
  · 目标 peer 恒 = 家庭单 owner（--peer 缺省 "owner"）；零新 peer 创建
    （零设备名 peer、零多设备多孩子身份）；换设备 = 新 session 家族、
    画像不变（arch §1.3 已知后果，K4-E6 live 轮知悉项）。

幂等与 live 共生（K4-E3 · notes ④ 判据）:
  · 每条灌入消息 metadata 携带 kidpalBackfillFingerprint =
    v1:<source-kind>:<sha256hex>；指纹 = 源锚身份（源件 + 行/事件序 +
    内容），同源重跑确定不变；重复内容消息不互吞（同源内零碰撞）。
  · 自建 session metadata 携带 kidpalBackfillBatch，值 = 该 session 自身
    键（「标记即键」形态——值形态契约自由；回滚 runner 经
    list_backfill_batches 取回的集合即恰被标记 session 的键集合，无需
    额外枚举面）。
  · 写入前先取既有指纹集合，命中即跳过——二次 apply 恰 ingested=0、
    skipped=全量、零写调用。
  · live 插件在写数据无 backfill 标记 → 永不进指纹集合、永不被误判
    已灌、永不被本工具写入、永不被回滚触碰。

源（只读零改写；plan/apply 全程字节不变，K4-E4）:
  · --corpus-dir  session-corpus 源语料目录：逐非空行一条消息，行格式
    `[<role>] <content>`（任何方括号角色前缀皆识别透传，含旧设备流程
    回合行——照记不滤）；未识别行（无前缀残行）兜底收编为一条消息
    （role=user、内容=整行）并在计数报告 backfill.unparsed 披露——
    照记全量、零丢弃。
  · --openclaw-db  OpenClaw kidpal agent sqlite：表 transcript_events，
    至少列 id / role / content（列超集容忍；缺表缺列 = 红，exit 1）。
    只读使用（全部语句为 SELECT，零写语句）。

出网（NFR-SEC-2 ⑤）: 数据面只经 --api（缺省 http://127.0.0.1:18100，
Honcho loopback）；deriver→DeepSeek / embedding→DashScope 两条出网记账
在 Honcho 服务端既有体内，本工具零新增出网路径、零内联凭据。
零过滤类旗标: redact / mask / anonymize / pii / filter- 族一律不在场
（human 终拍照记全量）。

exit 码: 0 = 成功（含 plan 干跑）；1 = 运行/数据错误（源不在世、缺表
缺列、API 不可达、报告落盘失败——先于或隔离于写入显式失败，不裸崩）；
2 = 用法错（缺 --device-id、零数据源、未知旗标——argparse 同码）。

计数报告（--report-out，K4-E3 · M5 收口轮 workstream 对账件）: plan/apply
两态都落盘；顶层键 mode + peers/sessions/messages/observations 四计数器
before/after + backfill.ingested/skipped/unparsed/perSource；apply 态
after = 写后快照，plan 态 after = 预测（messages.after = before + ingested；
observations 的 deriver 异步增量在 live 轮对账——K4-E6）。

执行面: human 在 ECS 全量跑（K4-E6 live structural 项 = ECS 全量跑 +
计数报告过目 + 幂等二次跑零增量 + 清库回滚演练 + 画像抽查；挂
awaiting-human:live-deploy，Pod 不自行部署）。

minimal: 真 REST 适配（HonchoRestClient）的端点与 wire 形状按 Honcho
REST 最佳知识落笔——metadata 逐消息可达性 = live 全量跑首验点（冻结件
缺口申报）；端点路径集中类常量，live 轮一次校准。若 message metadata
不可达，按冻结件缺口申报换 session 侧指纹清单方案并走 tighten-only
反审。升级路径：本类恰一 _request 出入口。

stdlib-only；模块顶层零副作用、零网络 I/O；可被 importlib 按路径导入。
"""

from __future__ import annotations

import argparse
import hashlib
import json
import sqlite3
import sys
import urllib.error
import urllib.parse
import urllib.request
from datetime import datetime, timezone
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


def split_corpus_line(line: str) -> tuple[str, str, bool]:
    """`[<role>] <content>` → (role, content, recognized)。

    任何方括号角色前缀皆识别透传（含旧设备流程回合行——照记不滤）；
    未识别行返回 (user, 整行, False)——兜底收编，不静默丢弃。
    """
    if line.startswith("["):
        end = line.find("]")
        if end > 1:
            rest = line[end + 1:]
            if rest.startswith(" "):
                rest = rest[1:]
            role = line[1:end].strip()
            return (role or "user"), rest, True
    return "user", line, False


def parse_corpus_dir(corpus_dir: Path) -> tuple[list[dict], int]:
    """session-corpus 源语料 → 计划消息列表。

    逐非空行一条消息；未识别行兜底收编并计数（零丢弃）；只读——不写
    不改不删源。返回 (messages, unparsed_count)，每条 message 含
    role / content / fingerprint / unparsed 四键。
    """
    messages: list[dict] = []
    unparsed = 0
    for path in sorted(p for p in corpus_dir.rglob("*") if p.is_file()):
        rel = path.relative_to(corpus_dir).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        for line_no, raw in enumerate(text.splitlines(), start=1):
            line = raw.strip()
            if not line:
                continue
            role, content, recognized = split_corpus_line(line)
            if not recognized:
                unparsed += 1
            messages.append({
                "role": role,
                "content": content,
                "fingerprint": fingerprint("session-corpus", rel, str(line_no), line),
                "unparsed": not recognized,
            })
    return messages, unparsed


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
            })
    finally:
        con.close()
    return messages


def plan_sources(args: argparse.Namespace) -> dict[str, list[dict]]:
    """双源解析（两源至少其一已在用法校验保证）；返回按 source-kind 分组。"""
    plan: dict[str, list[dict]] = {}
    if args.corpus_dir:
        corpus_dir = Path(args.corpus_dir)
        if not corpus_dir.is_dir():
            raise BackfillError(f"--corpus-dir 不在世或非目录: {corpus_dir}")
        plan["session-corpus"], _ = parse_corpus_dir(corpus_dir)
    if args.openclaw_db:
        plan["openclaw-transcript"] = parse_transcript_db(Path(args.openclaw_db))
    return plan


# --------------------------------------------------------------------------- #
# 真 REST 适配（离线验收以记录桩注入 build_client 接缝，不经本类）
# --------------------------------------------------------------------------- #


class HonchoRestClient:
    """Honcho REST 适配（真实现；stdlib urllib）。

    minimal: 端点与 wire 形状按 Honcho REST 最佳知识落笔——metadata 逐
    消息可达性 = live 全量跑首验点（冻结件缺口申报）；端点集中类常量，
    live 轮一次校准。升级路径：若 message metadata 不可达，按缺口申报
    换 session 侧指纹清单方案（tighten-only 反审回改冻结件）。
    快照/指纹枚举按全量拉取建模（现网量级小）；量级增大后换聚合端点。
    """

    EP_PEERS = "/v2/peers"
    EP_SESSIONS = "/v2/sessions"
    EP_SESSION_MESSAGES = "/v2/sessions/{sid}/messages"
    EP_PEER_REPRESENTATIONS = "/v2/peers/{pid}/representations"

    def __init__(self, api: str):
        self._base = api.rstrip("/")
        self._sid_by_key: dict[str, str] = {}
        self._peer_by_key: dict[str, str] = {}

    # -- HTTP 出入口（恰一处；出网目的地 = 构造参数 api，即 --api loopback）-- #
    def _request(self, method: str, path: str, *, query=None, body=None):
        url = self._base + path
        if query:
            url += "?" + urllib.parse.urlencode(query)
        data = None
        headers = {"Accept": "application/json"}
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
        if payload is None:
            return []
        if isinstance(payload, list):
            return payload
        if isinstance(payload, dict):
            for key in ("items", "data", "results"):
                if isinstance(payload.get(key), list):
                    return payload[key]
        return []

    def snapshot_counts(self) -> dict:
        peers = self._items(self._request("GET", self.EP_PEERS))
        sessions = self._items(self._request("GET", self.EP_SESSIONS))
        messages = 0
        for session in sessions:
            sid = urllib.parse.quote(str(
                session.get("id") or session.get("sessionId") or ""))
            msgs = self._items(self._request(
                "GET", self.EP_SESSION_MESSAGES.format(sid=sid)))
            messages += len(msgs)
        observations = 0
        for peer in peers:
            pid = urllib.parse.quote(str(
                peer.get("id") or peer.get("peerId") or ""))
            reps = self._items(self._request(
                "GET", self.EP_PEER_REPRESENTATIONS.format(pid=pid),
                query={"type": "worked"}))
            observations += len(reps)
        return {
            "peers": len(peers),
            "sessions": len(sessions),
            "messages": messages,
            "observations": observations,
        }

    def list_backfill_batches(self) -> set:
        """既有 backfill batch 标记集合——标记即键形态下 = 被标记 session 键集。"""
        batches = set()
        for session in self._items(self._request("GET", self.EP_SESSIONS)):
            metadata = session.get("metadata") or {}
            if SESSION_BATCH_KEY in metadata:
                batches.add(str(metadata[SESSION_BATCH_KEY]))
        return batches

    def list_fingerprints(self) -> set:
        """既有消息指纹集合（live 数据无标记 → 天然不在集合内）。"""
        fingerprints = set()
        for session in self._items(self._request("GET", self.EP_SESSIONS)):
            sid = urllib.parse.quote(str(
                session.get("id") or session.get("sessionId") or ""))
            for message in self._items(self._request(
                    "GET", self.EP_SESSION_MESSAGES.format(sid=sid))):
                metadata = message.get("metadata") or {}
                if MESSAGE_FP_KEY in metadata:
                    fingerprints.add(str(metadata[MESSAGE_FP_KEY]))
        return fingerprints

    def _resolve_session(self, session_key: str) -> str:
        sid = self._sid_by_key.get(session_key)
        if sid:
            return sid
        for session in self._items(self._request("GET", self.EP_SESSIONS)):
            name = str(session.get("name") or session.get("id") or "")
            if name == session_key:
                sid = str(session.get("id") or "")
                self._sid_by_key[session_key] = sid
                return sid
        raise BackfillError(f"Honcho session 不可解析: {session_key}")

    def create_session(self, peer, session_key, metadata) -> None:
        self._peer_by_key[session_key] = str(peer)
        body = {"peerId": str(peer), "name": str(session_key),
                "metadata": dict(metadata)}
        try:
            payload = self._request("POST", self.EP_SESSIONS, body=body)
        except BackfillError as exc:
            if exc.status == 409:
                return  # session 已在世（重复 create）——幂等容忍
            raise
        if isinstance(payload, dict):
            sid = payload.get("id") or payload.get("sessionId")
            if sid:
                self._sid_by_key[session_key] = str(sid)

    def add_message(self, session_key, role, content, metadata) -> None:
        sid = urllib.parse.quote(self._resolve_session(str(session_key)))
        peer = (AGENT_PEER if str(role) == "assistant"
                else self._peer_by_key.get(str(session_key), DEFAULT_PEER))
        self._request(
            "POST", self.EP_SESSION_MESSAGES.format(sid=sid),
            body=[{"content": str(content), "peerId": peer,
                   "metadata": dict(metadata)}])

    def delete_sessions(self, session_keys) -> int:
        deleted = 0
        for key in session_keys:
            try:
                sid = urllib.parse.quote(self._resolve_session(str(key)))
            except BackfillError:
                continue  # 不在世即视作已删
            self._request("DELETE", self.EP_SESSIONS + "/" + sid)
            deleted += 1
        return deleted


def build_client(api: str):
    """恰一位置参 → 鸭型协议客户端（离线验收以记录桩注入本接缝）。"""
    return HonchoRestClient(api)


# --------------------------------------------------------------------------- #
# CLI 与主流程
# --------------------------------------------------------------------------- #


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="backfill_honcho.py",
        description="历史 corpus → Honcho 记忆库照记全量迁移"
                    "（plan 干跑缺省，--apply 显式写）")
    parser.add_argument("--corpus-dir", default=None,
                        help="session-corpus 源语料目录（只读；逐非空行一条消息）")
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
    plan = plan_sources(args)

    client = build_client(args.api)
    before = client.snapshot_counts()
    existing_fps = client.list_fingerprints()

    ingested = 0
    skipped = 0
    per_source: dict[str, dict] = {}
    write_sessions: list[tuple[str, list[dict]]] = []

    for kind in SOURCE_KINDS:
        msgs = plan.get(kind)
        if msgs is None:
            continue  # 该源未提供（两源至少其一已校验）
        stat = {
            "planned": len(msgs),
            "ingested": 0,
            "skipped": 0,
            "unparsed": sum(1 for m in msgs if m["unparsed"]),
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
                client.add_message(
                    key, message["role"], message["content"],
                    {MESSAGE_FP_KEY: message["fingerprint"]})
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
