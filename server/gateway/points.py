"""KidPal 星星积分确定性执行器 — 最小写账 / 权威载荷生产 / keyset 分页
(TaskPackage 261104-k0-points-e2e-spike；契约 §1.3⑦⑧ / §1.4 add_points·redeem_points /
§1.5-7 积分面机械职责 / §四 kidpal_points_page)。

语义权威 = 冻结验收 server/acceptance/test_points_e2e_spike.acceptance.py 文件头
FROZEN OFFLINE CONTRACT 节（A 段）——本模块的模块级常量、函数名、参数形状与拒绝语义
逐条对齐该节，偏离即红。

D-10（模型只产薄意图，确定性模块写账）的本包落地：LLM 只发
`control/add_points {points, reason}` / `control/redeem_points {points}`；余额、分录 id、
前后余额与 revision 全部由本模块计算并持久化，模型 API 形状上不存在这些入口。

归属边界（K1/K2/K3）：
  - K1（261104-k1-points-ledger-core）在同一 `points.py` 上正式化单 writer——跨进程锁、
    幂等、余额重算与删除重推。本包只做最小可靠写（进程内锁 + tmp+replace），不另起
    第二账本、第二余额算法。
  - K2 = 完整积分 UI / 详细账本交互；K3 = 正式 QQ 家长通道（skill / 稳定定位）。
  - 本模块**零 fastapi / 零 gateway-front 依赖**：纯 stdlib，任何位置可离线导入
    （story.py / schedule.py / display_schema.py 同款先例）。

state 文件形状（本包最小形态；K1 在同一形状上补齐不变量）：
    {"revision": int, "balance": int, "entries": [<六键分录>, ...]}
`entries` 恒为 (occurred_at, id) 时间倒序；`balance` 恒等于全部 delta 之和
（read_state 从分录重算，不信任磁盘上的 balance 副本）。
"""

from __future__ import annotations

import base64
import json
import logging
import os
import re
import sys
import threading
import uuid
from datetime import datetime, timezone

log = logging.getLogger("kidpal-gateway.points")

# --------------------------------------------------------------------------- #
# 冻结常量（FROZEN OFFLINE CONTRACT A 段；spike 只读，不提供配置入口）          #
# --------------------------------------------------------------------------- #

# 当前唯一激活目标（出厂默认 = 游乐场 40，requirements S-F6.5 AC1）。
# 形状与值冻结；target 配置管理归 K3/K4，spike 只读此常量。
DEFAULT_TARGET = {
    "id": "preset-playground",
    "name": "游乐场",
    "points": 40,
    "asset": "sc-wheel",
}

# state 文件路径 env 名单源（front_app.POINTS_STATE_PATH 与 CLI 共用同一个名字）。
POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"

# points_home.recent 时间倒序最多三条（契约 §1.3⑦）。
HOME_RECENT_MAX = 3

# 分页 limit 允许区间（契约 §四 kidpal_points_page：缺省 6、允许 1–50）。
PAGE_LIMIT_DEFAULT = 6
PAGE_LIMIT_MIN = 1
PAGE_LIMIT_MAX = 50

# 分录六键统一形状（契约 §1.3⑦⑧ 共用；写盘/校验/形状自检同一处单源）。
ENTRY_KEYS = frozenset(
    {"id", "delta", "reason", "occurred_at", "balance_before", "balance_after"})

# 进程内串行化（最小形态；跨进程锁 / 幂等 / 删后重算归 K1）。
_STATE_LOCK = threading.Lock()

_CLI_INT_RE = re.compile(r"^[0-9]+$")


# --------------------------------------------------------------------------- #
# 基础工具                                                                      #
# --------------------------------------------------------------------------- #


def _is_positive_int(value) -> bool:
    """正整数判定——bool 是 int 子类，必须显式排除（契约 §1.4：points 为正整数）。"""
    return isinstance(value, int) and not isinstance(value, bool) and value > 0


def _occurred_seconds(value) -> float:
    """occurred_at -> epoch 秒（keyset 排序键的时间半）。

    生产侧只写 `datetime.isoformat()`（带时区）；解析失败保守取 0.0（退化为按 id 排序），
    绝不让一条脏时间炸掉分页。
    """
    if isinstance(value, str):
        try:
            dt = datetime.fromisoformat(value.replace("Z", "+00:00"))
        except ValueError:
            return 0.0
        if dt.tzinfo is None:
            dt = dt.replace(tzinfo=timezone.utc)
        return dt.timestamp()
    return 0.0


def _entry_sort_key(entry: dict) -> tuple:
    """(occurred_at, id) 时间倒序键（契约 §四：keyset 定位，不用 offset）。"""
    return (_occurred_seconds(entry.get("occurred_at")), str(entry.get("id") or ""))


def _sorted_desc(entries) -> list:
    return sorted(entries, key=_entry_sort_key, reverse=True)


def _entry_shape_error(entry) -> "str | None":
    """校验一条分录是否满足六键冻结形状；通过 -> None，否则 -> 原因串。"""
    if not isinstance(entry, dict):
        return "entry must be an object"
    if set(entry.keys()) != ENTRY_KEYS:
        return f"entry keys must be exactly {sorted(ENTRY_KEYS)}"
    if not isinstance(entry["id"], str) or not entry["id"]:
        return "entry.id must be a non-empty string"
    for key in ("delta", "balance_before", "balance_after"):
        if not isinstance(entry[key], int) or isinstance(entry[key], bool):
            return f"entry.{key} must be an integer"
    if not isinstance(entry["reason"], str):
        return "entry.reason must be a string"
    if not isinstance(entry["occurred_at"], str) or not entry["occurred_at"]:
        return "entry.occurred_at must be a non-empty string"
    return None


def _new_entry_id() -> str:
    """分录 id 唯一由执行器生成（模型不产——D-10）。"""
    return "pt_" + uuid.uuid4().hex


def _now_iso(now) -> str:
    """occurred_at 生成：带时区 ISO8601（注入 now 供验收；None = UTC 墙钟）。"""
    if now is None:
        now = datetime.now(timezone.utc)
    if not isinstance(now, datetime):
        raise ValueError("now must be a datetime when provided")
    if now.tzinfo is None:
        now = now.replace(tzinfo=timezone.utc)
    return now.isoformat()


# --------------------------------------------------------------------------- #
# state 持久化                                                                  #
# --------------------------------------------------------------------------- #


def _empty_state() -> dict:
    return {"revision": 0, "balance": 0, "entries": []}


def read_state(path) -> dict:
    """读取账本状态：{"revision", "balance", "entries"}（entries 时间倒序）。

    文件不存在 -> 零状态（revision 0 / balance 0 / entries []）。
    文件存在但不可解析 / 形状违约 -> ValueError（绝不静默当空账覆盖真账本——数据丢失面）。
    `balance` 恒由分录重算（磁盘上的 balance 只是副本，不是权威）。
    """
    if not isinstance(path, str) or not path:
        raise ValueError("points state path must be a non-empty string")
    try:
        with open(path, "r", encoding="utf-8") as handle:
            raw = json.load(handle)
    except FileNotFoundError:
        return _empty_state()
    except (OSError, ValueError) as exc:
        raise ValueError(f"points state file unreadable/corrupt: {exc!r}") from exc

    if not isinstance(raw, dict):
        raise ValueError("points state root must be a JSON object")
    entries = raw.get("entries")
    if not isinstance(entries, list):
        raise ValueError("points state entries must be a list")
    for entry in entries:
        problem = _entry_shape_error(entry)
        if problem is not None:
            raise ValueError(f"points state corrupt entry: {problem}")
    revision = raw.get("revision", 0)
    if not isinstance(revision, int) or isinstance(revision, bool) or revision < 0:
        raise ValueError("points state revision must be a non-negative integer")

    ordered = _sorted_desc(entries)
    return {
        "revision": revision,
        "balance": sum(int(entry["delta"]) for entry in ordered),
        "entries": ordered,
    }


def _write_state(path: str, revision: int, entries) -> None:
    """落盘最小可靠写：先写同目录临时文件再 os.replace（跨进程锁 / 幂等归 K1）。

    `minimal:` 本包只保证「不写半截账本」；K1 在同一文件上补跨进程锁、原子语义与
    余额重算不变量，不另起第二账本。
    """
    ordered = _sorted_desc(entries)
    payload = {
        "revision": int(revision),
        "balance": sum(int(entry["delta"]) for entry in ordered),
        "entries": ordered,
    }
    parent = os.path.dirname(os.path.abspath(path))
    if parent:
        os.makedirs(parent, exist_ok=True)
    tmp_path = f"{path}.tmp-{os.getpid()}-{uuid.uuid4().hex[:8]}"
    try:
        with open(tmp_path, "w", encoding="utf-8") as handle:
            json.dump(payload, handle, ensure_ascii=False, indent=2)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(tmp_path, path)
    finally:
        try:
            if os.path.exists(tmp_path):
                os.remove(tmp_path)
        except OSError:  # pragma: no cover — 清理失败不掩盖主写入结果
            pass


def _append_entry(path: str, delta: int, reason: str, now) -> dict:
    """统一写账路径（add_points / redeem_points 共用；先校验后落账，拒绝零写入）。"""
    occurred_at = _now_iso(now)
    with _STATE_LOCK:
        state = read_state(path)
        balance_before = int(state["balance"])
        entry = {
            "id": _new_entry_id(),
            "delta": int(delta),
            "reason": reason,
            "occurred_at": occurred_at,
            "balance_before": balance_before,
            "balance_after": balance_before + int(delta),
        }
        revision = int(state["revision"]) + 1
        _write_state(path, revision, list(state["entries"]) + [entry])
    log.info(
        "points_entry corr_id=%s entry_id=%s delta=%+d balance_after=%d revision=%d",
        uuid.uuid4().hex, entry["id"], entry["delta"], entry["balance_after"], revision)
    return dict(entry)


# --------------------------------------------------------------------------- #
# 执行器 API（FROZEN OFFLINE CONTRACT A 段）                                     #
# --------------------------------------------------------------------------- #


def add_points(path, points, reason, *, now=None) -> dict:
    """追加一条加分分录并持久化，返回新分录（六键冻结形状）。

    拒绝（ValueError，**不写账**）：points 非正整数（bool 不算整数）/ reason 非非空 str。
    """
    if not _is_positive_int(points):
        raise ValueError(
            f"add_points points must be a positive integer, got {points!r} (bool excluded)")
    if not isinstance(reason, str) or not reason.strip():
        raise ValueError("add_points reason must be a non-empty string")
    return _append_entry(path, int(points), reason, now)


def redeem_points(path, points, *, now=None) -> dict:
    """兑换落一条负向分录，返回新分录。

    事由统一取 `兑换<当前目标名>`（模型不提供 target id / 负 delta）；
    拒绝（ValueError，**不写账**）：points != 当前激活目标分值，或余额不足。
    """
    if not _is_positive_int(points):
        raise ValueError(
            f"redeem_points points must be a positive integer, got {points!r} (bool excluded)")
    target_points = int(DEFAULT_TARGET["points"])
    if int(points) != target_points:
        raise ValueError(
            f"redeem_points only accepts the active target value {target_points}, "
            f"got {points}")
    state = read_state(path)
    if int(state["balance"]) < int(points):
        raise ValueError(
            f"redeem_points insufficient balance {state['balance']} < {points}")
    return _append_entry(
        path, -int(points), f"兑换{DEFAULT_TARGET['name']}", now)


# --------------------------------------------------------------------------- #
# 权威载荷生产（完整载荷只由本模块产生；restore 由 front 按通道追加）              #
# --------------------------------------------------------------------------- #


def build_points_home(path) -> dict:
    """契约 §1.3⑦ 完整 points_home 载荷（**不含 restore 键**——restore 归 front 按通道加）。"""
    state = read_state(path)
    return {
        "view": "points_home",
        "data": {
            "revision": int(state["revision"]),
            "balance": int(state["balance"]),
            "target": dict(DEFAULT_TARGET),
            "recent": [dict(entry) for entry in state["entries"][:HOME_RECENT_MAX]],
        },
    }


def _encode_cursor(entry: dict) -> str:
    """不透明 keyset 游标 = base64url({occurred_at, id})（设备原样回传，不解释内容）。"""
    raw = json.dumps(
        {"occurred_at": entry.get("occurred_at"), "id": entry.get("id")},
        ensure_ascii=False, separators=(",", ":"))
    return base64.urlsafe_b64encode(raw.encode("utf-8")).decode("ascii")


def _decode_cursor(cursor) -> "dict | None":
    """解码游标；非字符串 / 非 base64 / 非 {occurred_at,id} 形状 -> None（= 非法游标）。"""
    if not isinstance(cursor, str) or not cursor:
        return None
    try:
        raw = base64.urlsafe_b64decode(cursor.encode("ascii"))
        decoded = json.loads(raw.decode("utf-8"))
    except (ValueError, UnicodeDecodeError, OSError):
        return None
    if not isinstance(decoded, dict):
        return None
    if not isinstance(decoded.get("occurred_at"), str):
        return None
    if not isinstance(decoded.get("id"), str):
        return None
    return decoded


def _validate_limit(limit) -> int:
    if not isinstance(limit, int) or isinstance(limit, bool):
        raise ValueError(f"points page limit must be an integer, got {limit!r}")
    if limit < PAGE_LIMIT_MIN or limit > PAGE_LIMIT_MAX:
        raise ValueError(
            f"points page limit must be within {PAGE_LIMIT_MIN}-{PAGE_LIMIT_MAX}, got {limit}")
    return limit


def build_points_ledger_page(path, cursor=None, known_revision=None, limit=None) -> dict:
    """契约 §1.3⑧ 完整 points_ledger 载荷（无 restore 键；分页 wire 规则见契约 §四）。

    - cursor 缺省/None = 最新首页（page.reset=True）；
    - 续页按 (occurred_at, id) 时间倒序 keyset 定位，正常续页 page.reset=False；
    - 游标非法/过期，或 known_revision 与当前 revision 不一致 -> 回最新首页且 reset=True；
    - page.total = 全量分录数（不用 items.length 代替）；limit 允许 1–50，越界 ValueError；
    - 空账 -> items=[], page.total=0, balance=0（权威空账，不伪造分录）。
    """
    if limit is None:
        limit = PAGE_LIMIT_DEFAULT
    limit = _validate_limit(limit)

    state = read_state(path)
    revision = int(state["revision"])
    balance = int(state["balance"])
    entries = state["entries"]
    total = len(entries)

    reset = True
    start = 0
    boundary = None
    if cursor is not None:
        decoded = _decode_cursor(cursor)
        if decoded is not None:
            boundary = (
                _occurred_seconds(decoded.get("occurred_at")), str(decoded.get("id")))
            version_ok = (known_revision is None
                          or (isinstance(known_revision, int)
                              and not isinstance(known_revision, bool)
                              and int(known_revision) == revision))
            if version_ok:
                reset = False
                # keyset 续页：严格小于游标键（时间倒序）的第一条即起点。
                start = total
                for idx, entry in enumerate(entries):
                    if _entry_sort_key(entry) < boundary:
                        start = idx
                        break

    window = entries[start:start + limit]
    if reset:
        # 重置页恒从最新一条开始（非法游标 / 版本不符都在此收敛）。
        window = entries[:limit]

    has_more = (start + len(window)) < total
    # next_cursor=None 且 has_more=False = 已到最早一页（契约 §四）；仍有更早一页时
    # 游标 = 本页最后一条的 (occurred_at, id) keyset 键（不用 offset）。
    next_cursor = _encode_cursor(window[-1]) if (window and has_more) else None

    return {
        "view": "points_ledger",
        "data": {
            "revision": revision,
            "balance": balance,
            "items": [dict(entry) for entry in window],
            "page": {
                "total": total,
                "next_cursor": next_cursor,
                "has_more": bool(has_more),
                "reset": bool(reset),
            },
        },
    }


# --------------------------------------------------------------------------- #
# 最小 add / redeem CLI（门 1 裁定：真实 QQ 最小 producer 的落点）                 #
# --------------------------------------------------------------------------- #


def _parse_cli_int(raw: str) -> int:
    text = raw.strip() if isinstance(raw, str) else ""
    if not _CLI_INT_RE.match(text):
        raise ValueError(f"points value must be a positive decimal integer, got {raw!r}")
    value = int(text)
    if value <= 0:
        raise ValueError(f"points value must be a positive integer, got {raw!r}")
    return value


def _emit(stream, text: str) -> None:
    """CLI 行输出——终端编码（如 Windows cp1252）不支持中文时退化为 ASCII 转义，
    绝不让编码异常冒充执行失败。"""
    line = text + "\n"
    try:
        stream.write(line)
    except UnicodeEncodeError:
        stream.write(line.encode("ascii", "backslashreplace").decode("ascii"))


def _cli_usage(stream) -> None:
    _emit(
        stream,
        "usage:\n"
        "  python -m gateway.points add <points> <reason>\n"
        "  python -m gateway.points redeem <points>\n"
        f"env: {POINTS_STATE_ENV} = points state file path (required)")


def main(argv=None) -> int:
    """`python -m gateway.points add|redeem` 入口。

    state 路径单源 = os.environ[POINTS_STATE_ENV]（与 front 同一 env 名，无第二账本）。
    成功 exit 0；非法 / 拒绝 exit 非 0、不写账、说明写 stderr、不伪造成功回执。
    """
    args = list(sys.argv[1:] if argv is None else argv)
    state_path = os.environ.get(POINTS_STATE_ENV)
    if not state_path:
        _emit(
            sys.stderr,
            f"points: {POINTS_STATE_ENV} is not set — refusing to write "
            "(no state file configured, no second ledger)")
        return 2
    if not args or args[0] not in ("add", "redeem"):
        _cli_usage(sys.stderr)
        return 2
    command = args[0]
    try:
        if command == "add":
            if len(args) != 3:
                _cli_usage(sys.stderr)
                return 2
            points = _parse_cli_int(args[1])
            entry = add_points(state_path, points, args[2])
        else:
            if len(args) != 2:
                _cli_usage(sys.stderr)
                return 2
            points = _parse_cli_int(args[1])
            entry = redeem_points(state_path, points)
    except ValueError as exc:
        _emit(sys.stderr, f"points {command} rejected: {exc}")
        return 1

    # 成功回执只陈述执行器真实落账结果（不宣称「已告知家长」等未发生的事）。
    _emit(
        sys.stdout,
        f"points {command} ok entry_id={entry['id']} delta={entry['delta']:+d} "
        f"balance={entry['balance_after']} reason={entry['reason']}")
    return 0


if __name__ == "__main__":  # pragma: no cover — module CLI entry
    raise SystemExit(main())
