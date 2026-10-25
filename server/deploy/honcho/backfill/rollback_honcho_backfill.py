#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""rollback_honcho_backfill.py — backfill 清库回滚 runner（K4-E4 / OQ-10 / NFR-PRIV-1）。

TaskPackage: 261022-m5-k4-history-corpus-backfill。
选择性删除扩展（K4B-E2 · 261024-m5-k4b-corpus-parser-v2，review §2.1 ⑧ /
human D2 承载）：--source-kind {session-corpus, openclaw-transcript} 按
session 键后缀过滤删除面（batch 值 = session 自身键，末段即 source-kind）；
缺省 = 全部（旧行为逐字保持）；plan 输出列出 matched/skipped 键——「只回滚
corpus、transcript 批次保留/另裁」在工具契约下由此成为可表达操作。

职责: 删除范围 = 恰携带 kidpalBackfillBatch 标记的 session（连带其消息与
派生 observation——cascade 归 Honcho 服务端语义）；无标记数据（live 在用
session / 消息）零触碰。回滚面只在记忆库侧——零源侧旗标（不删 OpenClaw
完整日志、不删 session-corpus 源语料、不删设备诊断日志，各按既有接受口径
保留；NFR-PRIV-1 三保留）；对源语料的任何回读都不需要。

标记即键: backfill runner 自建 session 的 kidpalBackfillBatch 值 = 该
session 自身键（值形态契约自由）——本 runner 经 list_backfill_batches
取回的集合即恰被标记 session 的键集合，逐键删除；无标记数据天然不在
集合内，零误删面。

CLI: --api / --peer / --report-out / --apply（缺省 = plan 预演零删；显式
--apply 才删——回滚路径先于全量执行验证的迁移铁律）。报告同 backfill
计数 schema，backfill 块含 deleted 计数；plan 态 after = before（逐
session 消息量不可经客户端协议预知——预测半留 live 演练对账，K4-E6 ③）。

exit 码 / stdlib-only / 模块顶层零副作用零网络 I/O —— 同 backfill_honcho.py
（真实现经同目录 backfill_honcho.py 共享 HonchoRestClient，回滚与迁移对
REST 的认知同源；离线验收以记录桩注入 build_client 接缝）。
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import sys
from pathlib import Path

DEFAULT_API = "http://127.0.0.1:18100"
SESSION_BATCH_KEY = "kidpalBackfillBatch"
DEFAULT_PEER = "owner"

_BACKFILL_MODULE_NAME = "kidpal_backfill_honcho_shared"
_backfill_module_cache = None


class UsageError(Exception):
    """用法错（exit 2）。"""


def backfill_module():
    """懒加载同目录 backfill_honcho.py（共享真 REST 适配；顶层零副作用）。"""
    global _backfill_module_cache
    if _backfill_module_cache is None:
        path = Path(__file__).resolve().with_name("backfill_honcho.py")
        spec = importlib.util.spec_from_file_location(
            _BACKFILL_MODULE_NAME, path)
        if spec is None or spec.loader is None:
            raise RuntimeError(f"无法定位同目录 backfill_honcho.py: {path}")
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        _backfill_module_cache = module
    return _backfill_module_cache


def build_client(api: str):
    """恰一位置参 → 鸭型协议客户端（离线验收以记录桩注入本接缝）。"""
    return backfill_module().HonchoRestClient(api)


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="rollback_honcho_backfill.py",
        description="backfill 清库回滚（恰删携带 "
                    f"{SESSION_BATCH_KEY} 标记的 session；plan 预演缺省，"
                    "--apply 显式删）")
    parser.add_argument("--api", default=DEFAULT_API,
                        help=f"Honcho API base（缺省 {DEFAULT_API} loopback）")
    parser.add_argument("--peer", default=DEFAULT_PEER,
                        help=f"目标 peer（缺省 {DEFAULT_PEER}，家庭单 owner；报告注记用）")
    parser.add_argument("--source-kind", default=None,
                        choices=backfill_module().SOURCE_KINDS,
                        help="选择性删除：只删该 source-kind 的标记 session"
                             "（session 键末段判别）；缺省 = 全部（旧行为）")
    parser.add_argument("--report-out", default=None,
                        help="回滚报告 JSON 落盘路径（缺省打印 stdout）")
    parser.add_argument("--apply", action="store_true",
                        help="显式删除；缺省 = plan 预演（零删除）")
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


def run_rollback(args: argparse.Namespace) -> int:
    client = build_client(args.api)
    before = client.snapshot_counts()
    marked_keys = sorted(str(key) for key in client.list_backfill_batches())
    mode = "apply" if args.apply else "plan"

    # 选择性删除（K4B-E2）：batch 值 = session 自身键，键末段（最后一个
    # ':' 之后）即 source-kind——后缀判别，零额外枚举面。缺省全量 = 旧行为。
    if args.source_kind:
        matched_keys = [k for k in marked_keys
                        if k.rsplit(":", 1)[-1] == args.source_kind]
        skipped_keys = [k for k in marked_keys
                        if k.rsplit(":", 1)[-1] != args.source_kind]
    else:
        matched_keys = list(marked_keys)
        skipped_keys = []

    if mode == "apply":
        # 零标记 = 零删除调用（live 在用数据零触碰的最小作用面）。
        deleted = int(client.delete_sessions(matched_keys) or 0) \
            if matched_keys else 0
        after = client.snapshot_counts()
    else:
        deleted = len(matched_keys)
        after = dict(before)

    report = {
        "mode": mode,
        "peers": {"before": before["peers"], "after": after["peers"]},
        "sessions": {"before": before["sessions"], "after": after["sessions"]},
        "messages": {"before": before["messages"], "after": after["messages"]},
        "observations": {"before": before["observations"],
                         "after": after["observations"]},
        "backfill": {
            "deleted": deleted,
            "markedSessions": marked_keys,
            "sourceKind": args.source_kind,      # additive（K4B-E2）；null = 全量
            "matchedSessions": matched_keys,     # additive：本次删除面逐键
            "skippedSessions": skipped_keys,     # additive：保留面逐键（plan 必列）
            "targetPeer": args.peer,
        },
    }
    emit_report(report, args.report_out)
    print(f"[kidpal-backfill-rollback] mode={mode} "
          f"marked={len(marked_keys)} matched={len(matched_keys)} "
          f"skipped={len(skipped_keys)} deleted={deleted}", flush=True)
    return 0


def main(argv: list | None = None) -> int:
    """exit 0 = 成功（含 plan 预演）；2 = 用法错；1 = 运行/数据错误。不裸崩。"""
    try:
        return run_rollback(build_arg_parser().parse_args(
            list(sys.argv[1:] if argv is None else argv)))
    except UsageError as exc:
        print(f"USAGE ERROR: {exc}", file=sys.stderr, flush=True)
        return 2
    except Exception as exc:  # noqa: BLE001 —— 兜底：回滚工具不得裸异常崩溃
        print(f"ERROR: {exc!r}", file=sys.stderr, flush=True)
        return 1


if __name__ == "__main__":
    sys.exit(main())
