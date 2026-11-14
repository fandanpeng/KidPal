"""KidPal 星星积分确定性执行器 — 唯一 writer / 原子写与重算 / 两类幂等 / 目标存储 / keyset 分页
(TaskPackage 261104-k1-points-ledger-core；在同一文件上正式化 K0 最小路径)。

语义权威 = 冻结验收 server/acceptance/test_points_ledger_core.acceptance.py 文件头
FROZEN OFFLINE CONTRACT 节（A 段）——本模块的模块级常量、函数名、参数形状与拒绝语义
逐条对齐该节，偏离即红；K0 冻结件 server/acceptance/test_points_e2e_spike.acceptance.py
（14 断言）保持全绿（add/redeem 既有调用形态零放宽，command_id 为 keyword-only 可选）。

D-10（模型只产薄意图，确定性模块写账）的本包落地：LLM 只发
`control/add_points {points, reason}` / `control/redeem_points {points}`；余额、分录 id、
前后余额、revision、激活目标、游标全部由本模块计算并持久化，模型 API 形状上不存在这些入口。

归属边界（K1/K2/K3/K4）：
  - K1（本包）在同一 `points.py` 上正式化单 writer——进程间 `.lock` 文件锁、tmp+os.replace
    原子替换、余额与逐条前后余额全量重算、删除重推、两类幂等、目标配置存储、keyset 分页。
    不另起第二账本、第二余额算法。
  - K2 = 完整积分 UI / 详细账本交互；K3 = 正式 QQ 家长通道（CLI / 稳定 command_id）；
    K4 = 目标配置的 QQ 交互与设备刻度联动。本包只提供执行器面与回执形状。
  - 本模块**零 fastapi / 零 gateway-front 依赖**：纯 stdlib，任何位置可离线导入
    （story.py / schedule.py / display_schema.py 同款先例）。

state 文件形状（本包正式形态；K0 三键形态为其子集，向后兼容）：
    {"revision": int, "balance": int, "entries": [<六键分录>, ...]
     [, "targets": [<四键自定义目标>, ...]      # 仅当存在自定义目标时落盘
      , "active_target": "<目标 id>"            # 仅当非出厂默认时落盘
      , "receipts": {"<command_id>": <六键分录>        # add / redeem 的传输幂等回执
                     "delete:<command_id>": {"id", "deleted": True,
                                             "revision", "balance"}}]}  # delete 回执
`entries` 恒为 (occurred_at, id) 时间倒序；`balance` 恒等于全部 delta 之和
（read_state 从分录重算，不信任磁盘上的 balance 副本）；逐条 balance_before/
balance_after 由写入路径按**时间序**全量重算（删除亦然，允许负余额 —— OQ-1）。
`receipts` 两族共表、键前缀隔离：add/redeem 走裸 command_id，delete 走
`delete:<command_id>`——`_replay_receipt`（只看裸键）永远看不到删除回执，反之
本模块读删除回执时也校验 `deleted` 键在场，两族不串味。
"""

from __future__ import annotations

import base64
import contextlib
import json
import logging
import os
import re
import sys
import threading
import unicodedata
import uuid
from datetime import datetime, timezone

# 进程间文件锁：POSIX = fcntl.flock，Windows = msvcrt.locking（stdlib，无第三方位依赖）。
try:  # pragma: no cover — 平台分支
    import fcntl
except ImportError:  # pragma: no cover — Windows
    fcntl = None
try:  # pragma: no cover — 平台分支
    import msvcrt
except ImportError:  # pragma: no cover — POSIX
    msvcrt = None

log = logging.getLogger("kidpal-gateway.points")

# --------------------------------------------------------------------------- #
# 冻结常量（FROZEN OFFLINE CONTRACT A 段；K1 正式化面）                          #
# --------------------------------------------------------------------------- #

# 短窗语义去重窗口（S-F6.3-AC5：起步 10 分钟；真机调优定数后由后续包回填）。
DEDUP_WINDOW_SECONDS = 600

# 四预置奖励场景（requirements S-F6.5-AC1 / arch §5.3.2「预置四场景为实现内不可删常量」；
# id 命名沿契约 §1.3⑦ 示例 "preset-playground" 的 preset-* 形态扩展冻结）。
PRESET_TARGETS = (
    {"id": "preset-choco", "name": "巧克力", "points": 20, "asset": "sc-choco"},
    {"id": "preset-movie", "name": "看电影", "points": 30, "asset": "sc-movie"},
    {"id": "preset-playground", "name": "游乐场", "points": 40, "asset": "sc-wheel"},
    {"id": "preset-gift", "name": "礼物", "points": 50, "asset": "sc-gift"},
)

# 出厂激活目标（出厂默认 = 游乐场 40，requirements S-F6.5 AC1）。形状与值冻结
# （K0 冻结件依赖）；激活目标可经 set_active_target 切换，本常量仍是出厂态事实。
DEFAULT_TARGET = {
    "id": "preset-playground",
    "name": "游乐场",
    "points": 40,
    "asset": "sc-wheel",
}

# 自定义目标边界（OQ-3：5–200 正整数，含端点）与通用标记图（契约 §1.3⑦ asset 闭集）。
CUSTOM_TARGET_MIN_POINTS = 5
CUSTOM_TARGET_MAX_POINTS = 200
CUSTOM_TARGET_ASSET = "sc-flag"

# state 文件路径 env 名单源（front_app.POINTS_STATE_PATH 与 CLI 共用同一个名字）。
POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"

# points_home.recent 时间倒序最多三条（契约 §1.3⑦）。
HOME_RECENT_MAX = 3

# 分页 limit 允许区间（契约 §四 kidpal_points_page：缺省 6、允许 1–50）。
PAGE_LIMIT_DEFAULT = 6
PAGE_LIMIT_MIN = 1
PAGE_LIMIT_MAX = 50

# QQ 家长通道 recent 回执行数（261104-k3；缺省 5、区间与分页同一把尺 1–50）。
RECENT_LIMIT_DEFAULT = 5

# 分录六键统一形状（契约 §1.3⑦⑧ 共用；写盘/校验/形状自检同一处单源）。
ENTRY_KEYS = frozenset(
    {"id", "delta", "reason", "occurred_at", "balance_before", "balance_after"})
# 目标四键统一形状（契约 §1.3⑦ target 形状）。
TARGET_KEYS = ("id", "name", "points", "asset")

# 进程内串行化（跨进程互斥另由 state 同目录 `.lock` 文件锁承载——两层都在，见 _critical）。
_STATE_LOCK = threading.Lock()

_CLI_INT_RE = re.compile(r"^[0-9]+$")
# 分录 id 形状（执行器生成 = "pt_" + uuid4().hex；QQ 通道回执的稳定定位把据此校验）。
_POINT_ENTRY_ID_RE = re.compile(r"^pt_[0-9a-f]+$")


# --------------------------------------------------------------------------- #
# 基础工具                                                                      #
# --------------------------------------------------------------------------- #


def _is_int(value) -> bool:
    """整数判定——bool 是 int 子类（JSON 布尔不得冒充整数余额/版本号/分值）。"""
    return isinstance(value, int) and not isinstance(value, bool)


def _is_positive_int(value) -> bool:
    """正整数判定——bool 是 int 子类，必须显式排除（契约 §1.4：points 为正整数）。"""
    return _is_int(value) and value > 0


def _occurred_seconds(value) -> float:
    """occurred_at -> epoch 秒（keyset 排序键的时间半）。

    生产侧只写 `datetime.isoformat()`（带时区）；解析失败保守取 0.0（退化为按 id 排序），
    绝不让一条脏时间炸掉分页。
    """
    dt = _occurred_datetime(value)
    return 0.0 if dt is None else dt.timestamp()


def _occurred_datetime(value):
    """occurred_at -> 带时区 datetime；不可解析 -> None。"""
    if not isinstance(value, str) or not value:
        return None
    try:
        dt = datetime.fromisoformat(value.replace("Z", "+00:00"))
    except ValueError:
        return None
    if dt.tzinfo is None:
        dt = dt.replace(tzinfo=timezone.utc)
    return dt


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
        if not _is_int(entry[key]):
            return f"entry.{key} must be an integer"
    if not isinstance(entry["reason"], str):
        return "entry.reason must be a string"
    if not isinstance(entry["occurred_at"], str) or not entry["occurred_at"]:
        return "entry.occurred_at must be a non-empty string"
    return None


def _target_shape_error(target) -> "str | None":
    """校验一个目标是否满足四键冻结形状（契约 §1.3⑦ target 形状）。"""
    if not isinstance(target, dict):
        return "target must be an object"
    if set(target.keys()) != set(TARGET_KEYS):
        return f"target keys must be exactly {list(TARGET_KEYS)}"
    for key in ("id", "name", "asset"):
        if not isinstance(target[key], str) or not target[key].strip():
            return f"target.{key} must be a non-empty string"
    if not _is_int(target["points"]) or target["points"] <= 0:
        return "target.points must be a positive integer"
    return None


def _new_entry_id() -> str:
    """分录 id 唯一由执行器生成（模型不产——D-10）。"""
    return "pt_" + uuid.uuid4().hex


def _new_target_id() -> str:
    """自定义目标 id 由执行器生成（模型不产 id —— FROZEN CONTRACT A 段）。"""
    return "target-" + uuid.uuid4().hex[:16]


def _coerce_now(now) -> datetime:
    """注入时钟归一：None = UTC 墙钟；naive 视为 UTC；非 datetime 拒绝（不写账）。"""
    if now is None:
        return datetime.now(timezone.utc)
    if not isinstance(now, datetime):
        raise ValueError("now must be a datetime when provided")
    if now.tzinfo is None:
        return now.replace(tzinfo=timezone.utc)
    return now


def _coerce_command_id(command_id):
    """传输幂等载体：None = 无传输幂等语义（K0 兼容）；在场须非空 str。"""
    if command_id is None:
        return None
    if not isinstance(command_id, str) or not command_id.strip():
        raise ValueError("command_id must be a non-empty string when provided")
    return command_id


def _normalize_reason(reason: str) -> str:
    """短窗去重的保守归一化（arch §6.5-2）：NFKC 全半角统一 + 去全部空白 + 去标点。

    不改时间前缀、不引入模型相似分或模糊阈值——归一化后**逐字相等**才算同事由。
    """
    text = unicodedata.normalize("NFKC", reason)
    return "".join(
        ch for ch in text
        if not ch.isspace() and not unicodedata.category(ch).startswith("P"))


# --------------------------------------------------------------------------- #
# 进程间锁（state 同目录 `.lock`；arch §5.1.2 / §5.3.2 / §6.5）                  #
# --------------------------------------------------------------------------- #


def _lock_path(path: str) -> str:
    """锁文件恒在 state 同目录（部署形态 = /opt/kidpal/points/{state.json,.lock}）。"""
    parent = os.path.dirname(os.path.abspath(path))
    return os.path.join(parent, ".lock") if parent else ".lock"


@contextlib.contextmanager
def _file_lock(path: str):
    """跨进程互斥：`<state 同目录>/.lock` 上的排他文件锁（POSIX flock / Windows locking）。

    锁文件常驻不删（unlink-while-held 是经典竞态；K1L-A1 亦以「.lock 在位」作机制锚）。
    本进程内层仍由 `_STATE_LOCK` 串行化——文件锁只兜跨进程覆盖面（front API 与 QQ CLI）。
    """
    lock_path = _lock_path(path)
    parent = os.path.dirname(lock_path)
    if parent:
        os.makedirs(parent, exist_ok=True)
    fd = os.open(lock_path, os.O_RDWR | os.O_CREAT, 0o644)
    try:
        if fcntl is not None:  # POSIX
            fcntl.flock(fd, fcntl.LOCK_EX)
        elif msvcrt is not None:  # Windows
            os.lseek(fd, 0, os.SEEK_SET)
            msvcrt.locking(fd, msvcrt.LK_LOCK, 1)
        yield
    finally:
        try:
            if fcntl is not None:
                fcntl.flock(fd, fcntl.LOCK_UN)
            elif msvcrt is not None:
                os.lseek(fd, 0, os.SEEK_SET)
                msvcrt.locking(fd, msvcrt.LK_UNLCK, 1)
        except OSError:  # pragma: no cover — 解锁失败不掩盖主写入结果
            pass
        finally:
            os.close(fd)


@contextlib.contextmanager
def _critical(path: str):
    """写入临界区：进程内锁 + 跨进程文件锁（读最新状态→校验→重算→原子替换→存回执）。"""
    with _STATE_LOCK:
        with _file_lock(path):
            yield


# --------------------------------------------------------------------------- #
# state 持久化                                                                  #
# --------------------------------------------------------------------------- #


def _default_active_id() -> str:
    return DEFAULT_TARGET["id"]


def _preset_targets() -> list:
    """四预置的防御性副本（返回值可被调用方改写而不污染模块常量）。"""
    return [dict(t) for t in PRESET_TARGETS]


def _all_targets(full: dict) -> list:
    """预置四项恒在 + 自定义目标（顺序：预置按冻结序、自定义按登记序）。"""
    return _preset_targets() + [dict(t) for t in full["custom_targets"]]


def _resolve_active(full: dict) -> dict:
    """当前激活目标（四键副本）；state 校验期已保证 id 落在预置/自定义集合内。"""
    active_id = full["active_target"]
    for target in _all_targets(full):
        if target["id"] == active_id:
            return target
    # _load_full 已拒绝未知 id；此处为不可能分支的保守回退（出厂默认恒在）。
    return dict(DEFAULT_TARGET)  # pragma: no cover


def _empty_full() -> dict:
    return {
        "revision": 0,
        "balance": 0,
        "entries": [],
        "custom_targets": [],
        "active_target": _default_active_id(),
        "receipts": {},
    }


def _load_custom_targets(raw: dict) -> list:
    """校验并返回自定义目标列表（缺省 = 无自定义）。"""
    targets = raw.get("targets", [])
    if targets is None:
        targets = []
    if not isinstance(targets, list):
        raise ValueError("points state targets must be a list")
    preset_ids = {t["id"] for t in PRESET_TARGETS}
    seen = set()
    out = []
    for target in targets:
        problem = _target_shape_error(target)
        if problem is not None:
            raise ValueError(f"points state corrupt target: {problem}")
        if target["id"] in preset_ids or target["id"] in seen:
            raise ValueError(f"points state duplicate/reserved target id {target['id']!r}")
        seen.add(target["id"])
        out.append(dict(target))
    return out


def _load_receipts(raw: dict) -> dict:
    """命令回执（传输幂等，arch §6.5-1）：缺省空表；形状违约 -> ValueError。"""
    receipts = raw.get("receipts", {})
    if receipts is None:
        receipts = {}
    if not isinstance(receipts, dict):
        raise ValueError("points state receipts must be an object")
    for key, value in receipts.items():
        if not isinstance(key, str) or not key:
            raise ValueError("points state receipt key must be a non-empty string")
        if not isinstance(value, dict) or not isinstance(value.get("id"), str):
            raise ValueError("points state receipt value must carry an entry id")
    return {key: dict(value) for key, value in receipts.items()}


def _load_full(path) -> dict:
    """读取并校验完整状态（含目标/回执）；文件不存在 = 出厂零状态。

    文件存在但不可解析 / 形状违约 -> ValueError（绝不静默当空账覆盖真账本——数据丢失面）。
    """
    if not isinstance(path, str) or not path:
        raise ValueError("points state path must be a non-empty string")
    try:
        with open(path, "r", encoding="utf-8") as handle:
            raw = json.load(handle)
    except FileNotFoundError:
        return _empty_full()
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
    if not _is_int(revision) or revision < 0:
        raise ValueError("points state revision must be a non-negative integer")

    custom_targets = _load_custom_targets(raw)
    active_target = raw.get("active_target", _default_active_id())
    if not isinstance(active_target, str) or not active_target:
        raise ValueError("points state active_target must be a non-empty string")
    known_ids = {t["id"] for t in PRESET_TARGETS} | {t["id"] for t in custom_targets}
    if active_target not in known_ids:
        raise ValueError(f"points state active_target {active_target!r} is unknown")
    receipts = _load_receipts(raw)

    ordered = _sorted_desc(entries)
    return {
        "revision": int(revision),
        "balance": sum(int(entry["delta"]) for entry in ordered),
        "entries": ordered,
        "custom_targets": custom_targets,
        "active_target": active_target,
        "receipts": receipts,
    }


def read_state(path) -> dict:
    """读取账本状态：{"revision", "balance", "entries"}（entries 时间倒序）。

    文件不存在 -> 零状态（revision 0 / balance 0 / entries []）。
    文件存在但不可解析 / 形状违约 -> ValueError（绝不静默当空账覆盖真账本——数据丢失面）。
    `balance` 恒由分录重算（磁盘上的 balance 只是副本，不是权威）。逐条前后余额按持久值
    返回（写入路径负责重算；读取侧不在读时「修正」陈旧链，以免掩盖写入缺陷）。
    """
    full = _load_full(path)
    return {
        "revision": full["revision"],
        "balance": full["balance"],
        "entries": full["entries"],
    }


def _write_state(path: str, full: dict) -> None:
    """落盘：先写同目录临时文件再 os.replace（原子替换；失败上抛、旧状态完整）。"""
    ordered = _sorted_desc(full["entries"])
    payload = {
        "revision": int(full["revision"]),
        "balance": sum(int(entry["delta"]) for entry in ordered),
        "entries": ordered,
    }
    # 可选键只在非出厂默认时落盘（K0 三键形态是子集，旧 state 继续可读）。
    if full["custom_targets"]:
        payload["targets"] = [dict(t) for t in full["custom_targets"]]
    if full["active_target"] != _default_active_id():
        payload["active_target"] = full["active_target"]
    if full["receipts"]:
        payload["receipts"] = {key: dict(value) for key, value in full["receipts"].items()}

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


def _recompute_chain(entries) -> list:
    """按**时间序**全量重算逐条 balance_before / balance_after（允许负余额，OQ-1）。

    返回时间倒序的新列表（每条都是新 dict，不就地改写入参）。余额 = 剩余分录之和：
    写入路径（追加 / 删除 / 兑换）恒过本函数，因此删除后可负、其余分录保留不清零。
    """
    running = 0
    out = []
    for entry in sorted(entries, key=_entry_sort_key):
        delta = int(entry["delta"])
        out.append({
            "id": entry["id"],
            "delta": delta,
            "reason": entry["reason"],
            "occurred_at": entry["occurred_at"],
            "balance_before": running,
            "balance_after": running + delta,
        })
        running += delta
    return _sorted_desc(out)


def _find_entry(entries, entry_id):
    for entry in entries:
        if entry["id"] == entry_id:
            return entry
    return None


def _replay_receipt(full: dict, command_id: str):
    """同 command_id 重放 -> 首次结果（优先返账本里该分录的当前派生值）。"""
    stored = full["receipts"].get(command_id)
    if not isinstance(stored, dict):
        return None
    live = _find_entry(full["entries"], stored.get("id"))
    return dict(live if live is not None else stored)


def _find_duplicate(full: dict, delta: int, reason: str, now_dt: datetime):
    """短窗语义去重（arch §6.5-2）：同正负号分值 + 归一化事由相等 + 窗口内。

    匹配对象 = 当前账本**现存**分录（已删除分录不参与——D-03「分录为事实」）。
    窗口以注入 now 判定（禁 wall-clock 依赖验收）。
    """
    if DEDUP_WINDOW_SECONDS <= 0:  # 退化配置 = 关闭去重（变异敏感点）
        return None
    target_norm = _normalize_reason(reason)
    for entry in full["entries"]:
        if int(entry["delta"]) != int(delta):
            continue
        if _normalize_reason(str(entry["reason"])) != target_norm:
            continue
        occurred = _occurred_datetime(entry["occurred_at"])
        if occurred is None:
            continue
        if abs((now_dt - occurred).total_seconds()) <= DEDUP_WINDOW_SECONDS:
            return entry
    return None


def _duplicate_receipt(full: dict, hit: dict) -> dict:
    """去重命中回执（契约 A 段：{"duplicate": True, "balance": <当前权威余额>}）。"""
    return {
        "duplicate": True,
        "balance": int(full["balance"]),
        "entry_id": hit["id"],
        "revision": int(full["revision"]),
    }


# --------------------------------------------------------------------------- #
# 执行器 API（FROZEN OFFLINE CONTRACT A 段）                                     #
# --------------------------------------------------------------------------- #


def add_points(path, points, reason, *, now=None, command_id=None) -> dict:
    """追加一条加分分录并持久化，返回新分录（六键冻结形状）。

    拒绝（ValueError，**不写账**）：points 非正整数（bool 不算整数）/ reason 非非空 str。
    command_id（keyword-only, 缺省 None = 无传输幂等语义，K0 兼容）：同 id 重放返首次
    结果——不新增分录、不递增 revision（arch §6.5-1）；传输幂等检查先于短窗语义去重。
    短窗去重命中 -> 不落账，返回 {"duplicate": True, "balance": <当前权威余额>}（+附加键）。
    """
    if not _is_positive_int(points):
        raise ValueError(
            f"add_points points must be a positive integer, got {points!r} (bool excluded)")
    if not isinstance(reason, str) or not reason.strip():
        raise ValueError("add_points reason must be a non-empty string")
    now_dt = _coerce_now(now)
    command_id = _coerce_command_id(command_id)

    with _critical(path):
        full = _load_full(path)
        if command_id is not None:
            replayed = _replay_receipt(full, command_id)
            if replayed is not None:
                log.info(
                    "points_replay corr_id=%s command_id=%s entry_id=%s revision=%d",
                    uuid.uuid4().hex, command_id, replayed.get("id"), full["revision"])
                return replayed
        hit = _find_duplicate(full, int(points), reason, now_dt)
        if hit is not None:
            receipt = _duplicate_receipt(full, hit)
            log.info(
                "points_dedup corr_id=%s delta=%+d balance=%d revision=%d hit=%s",
                uuid.uuid4().hex, int(points), receipt["balance"],
                receipt["revision"], hit["id"])
            return receipt
        entry = {
            "id": _new_entry_id(),
            "delta": int(points),
            "reason": reason,
            "occurred_at": now_dt.isoformat(),
            "balance_before": 0,
            "balance_after": int(points),
        }
        full["entries"] = _recompute_chain(full["entries"] + [entry])
        stored = _find_entry(full["entries"], entry["id"])
        full["revision"] = int(full["revision"]) + 1
        if command_id is not None:
            full["receipts"][command_id] = dict(stored)
        _write_state(path, full)
        revision = full["revision"]

    log.info(
        "points_entry corr_id=%s entry_id=%s delta=%+d balance_after=%d revision=%d",
        uuid.uuid4().hex, stored["id"], stored["delta"], stored["balance_after"], revision)
    return dict(stored)


def redeem_points(path, points, *, now=None, command_id=None) -> dict:
    """兑换落一条负向分录，返回新分录。

    事由统一取 `兑换<当前激活目标名>`（模型不提供 target id / 负 delta / 事由）；
    拒绝（ValueError，**不写账**）：points != 当前激活目标分值（动态资格门，
    S-F6.3-AC3），或当前 balance < points。command_id / 短窗去重语义与 add_points 同型
    （同一次兑换不重复扣分——S-F6.3-AC5 兑换侧同此）。
    """
    if not _is_positive_int(points):
        raise ValueError(
            f"redeem_points points must be a positive integer, got {points!r} (bool excluded)")
    now_dt = _coerce_now(now)
    command_id = _coerce_command_id(command_id)

    with _critical(path):
        full = _load_full(path)
        if command_id is not None:
            replayed = _replay_receipt(full, command_id)
            if replayed is not None:
                log.info(
                    "points_replay corr_id=%s command_id=%s entry_id=%s revision=%d",
                    uuid.uuid4().hex, command_id, replayed.get("id"), full["revision"])
                return replayed
        active = _resolve_active(full)
        target_points = int(active["points"])
        if int(points) != target_points:
            raise ValueError(
                f"redeem_points only accepts the active target value {target_points}, "
                f"got {points}")
        reason = f"兑换{active['name']}"
        hit = _find_duplicate(full, -int(points), reason, now_dt)
        if hit is not None:
            receipt = _duplicate_receipt(full, hit)
            log.info(
                "points_dedup corr_id=%s delta=%+d balance=%d revision=%d hit=%s",
                uuid.uuid4().hex, -int(points), receipt["balance"],
                receipt["revision"], hit["id"])
            return receipt
        if int(full["balance"]) < int(points):
            raise ValueError(
                f"redeem_points insufficient balance {full['balance']} < {points}")
        entry = {
            "id": _new_entry_id(),
            "delta": -int(points),
            "reason": reason,
            "occurred_at": now_dt.isoformat(),
            "balance_before": 0,
            "balance_after": -int(points),
        }
        full["entries"] = _recompute_chain(full["entries"] + [entry])
        stored = _find_entry(full["entries"], entry["id"])
        full["revision"] = int(full["revision"]) + 1
        if command_id is not None:
            full["receipts"][command_id] = dict(stored)
        _write_state(path, full)
        revision = full["revision"]

    log.info(
        "points_entry corr_id=%s entry_id=%s delta=%+d balance_after=%d revision=%d "
        "kind=redeem",
        uuid.uuid4().hex, stored["id"], stored["delta"], stored["balance_after"], revision)
    return dict(stored)


# 删除回执在 state.receipts 里的键命名空间（add/redeem 用裸 command_id；前缀隔离
# 保证 `_replay_receipt` 永远看不到删除回执——两族互不串味）。
_DELETE_RECEIPT_PREFIX = "delete:"


def _delete_receipt_key(command_id: str) -> str:
    return _DELETE_RECEIPT_PREFIX + command_id


def _replay_delete_receipt(full: dict, command_id):
    """同 command_id 的删除重放 -> 首次结果；无回执 / 形状不符 -> None。

    形状不符（缺 `deleted` 键）= 该裸键是 add/redeem 的分录回执，绝不能当删除结果读
    （command_id 字面量恰好以命名空间前缀开头时的隔离守卫）。
    """
    stored = full["receipts"].get(_delete_receipt_key(command_id))
    if not isinstance(stored, dict) or "deleted" not in stored:
        return None
    return dict(stored)


def _validate_delete_locator(entry_id, index) -> None:
    """定位符的 state-free 校验（互斥 + 形状 + 正整数）。

    挡在 `_critical` 之前：语义拒绝路径零文件副作用（不建目录、不建 `.lock`）。
    越界（需要账本长度）留给 `_delete_entry_locked` 在临界区内判。
    """
    if (entry_id is None) == (index is None):
        raise ValueError(
            "delete needs exactly one locator: --entry-id pt_<hex> 或 --index <序号>")
    if index is not None:
        if not _is_positive_int(index):
            raise ValueError(f"delete index must be a positive integer, got {index!r}")
        return
    if not isinstance(entry_id, str) or not entry_id.strip():
        raise ValueError(
            f"delete_entry entry_id must be a non-empty string, got {entry_id!r}")


def _delete_entry_locked(path, entry_id=None, index=None, command_id=None) -> dict:
    """**只许在 `_critical` 内调用**：序号→id 换算与删除在同一临界区一次做完。

    排序权威 = `_entry_sort_key` / `_sorted_desc`（与设备 ledger 分页、recent 回执
    同一把尺）——若「先读序号、再进锁删」，两次操作之间的新写入会让序号静默漂移、
    删错账（TOCTOU）。index 为 1 起的时间倒序序号（= recent 回执的 index）。

    command_id（keyword-only，缺省 None = 无传输幂等语义）：同 id 重放 -> **直接返回
    首次结果**、不再删除、不递增 revision。重放判定在**序号换算之前**——首删后序号已
    漂移，若先换算就会删到另一条（`--index 2` 重放的真实误删面）。`deleted=false` 的
    幂等 no-op **不记回执**（no-op 恒零写入，冻结契约与文档同钉）。
    """
    _validate_delete_locator(entry_id, index)
    full = _load_full(path)
    if command_id is not None:
        replayed = _replay_delete_receipt(full, command_id)
        if replayed is not None:
            log.info(
                "points_delete_replay corr_id=%s command_id=%s entry_id=%s revision=%d",
                uuid.uuid4().hex, command_id, replayed.get("id"), full["revision"])
            return replayed
    target_id = entry_id
    if index is not None:
        entries = full["entries"]          # `_load_full` 已按 (occurred_at, id) 倒序
        if index > len(entries):
            raise ValueError(
                f"delete index {index} out of range "
                f"(ledger holds {len(entries)} entries)")
        target_id = entries[index - 1]["id"]
    if _find_entry(full["entries"], target_id) is None:
        log.info(
            "points_delete corr_id=%s entry_id=%s deleted=false revision=%d balance=%d",
            uuid.uuid4().hex, target_id, full["revision"], full["balance"])
        return {
            "id": target_id,
            "deleted": False,
            "revision": int(full["revision"]),
            "balance": int(full["balance"]),
        }
    full["entries"] = _recompute_chain(
        [entry for entry in full["entries"] if entry["id"] != target_id])
    full["revision"] = int(full["revision"]) + 1
    receipt = {
        "id": target_id,
        "deleted": True,
        "revision": int(full["revision"]),
        "balance": sum(int(entry["delta"]) for entry in full["entries"]),
    }
    if command_id is not None:
        key = _delete_receipt_key(command_id)
        existing = full["receipts"].get(key)
        if isinstance(existing, dict) and "deleted" not in existing:
            # 病态撞键（调用方的 command_id 字面量恰好以命名空间前缀开头，且该裸键已
            # 被 add/redeem 的分录回执占用）：绝不覆盖它——add/redeem 的传输幂等优先，
            # 本条删除回执降级为不持久（下次重放即普通删除，退回无重放守卫生效前的行为）。
            log.warning(
                "points_delete_receipt_degraded corr_id=%s command_id=%s "
                "reason=namespace_collision",
                uuid.uuid4().hex, command_id)
        else:
            full["receipts"][key] = dict(receipt)
    _write_state(path, full)
    revision = receipt["revision"]
    balance = receipt["balance"]

    log.info(
        "points_delete corr_id=%s entry_id=%s deleted=true revision=%d balance=%d",
        uuid.uuid4().hex, target_id, revision, balance)
    return receipt


def delete_entry(path, entry_id=None, *, index=None, command_id=None) -> dict:
    """删除一条误记分录并按时间序重算全部前后余额（契约 §四 kidpal_points_delete）。

    - 形状合法且存在 -> 移除该分录、全量重算、revision +1，返回回执
      {"id", "deleted": True, "revision", "balance"}（允许负余额 —— OQ-1）；
    - 形状合法但不存在 -> 幂等 no-op：**不写账**、revision 不变、回执 "deleted": False
      （front 据此仍重推对账帧）；
    - 非法 id（非 str / 空串 / 纯空白）/ 定位符缺失或重复 / index 非正整数或越界
      -> ValueError（拒绝并审计，**零写入**）。

    定位符二选一：`entry_id`（稳定 id，QQ 通道首选）或 keyword-only `index`（1 起
    时间倒序序号，= recent 回执的 index）。command_id（keyword-only，缺省 None）作
    传输幂等：同 id 重放返首次结果、不二次删除（`delete:<command_id>` 命名空间回执，
    与 add/redeem 的裸 command_id 回执同表隔离）。换算与删除同在一个 `_critical`
    临界区内（`_delete_entry_locked`），防 TOCTOU 删错账。
    """
    _validate_delete_locator(entry_id, index)   # 拒绝路径零文件副作用（锁外挡）
    with _critical(path):
        return _delete_entry_locked(
            path, entry_id=entry_id, index=index, command_id=command_id)


# --------------------------------------------------------------------------- #
# 目标配置存储（S-F6.5 AC1/AC2/AC4 存储半；OQ-2/OQ-3）                           #
# --------------------------------------------------------------------------- #


def get_active_target(path) -> dict:
    """当前激活目标（id/name/points/asset 四键）；出厂态（无 state 文件）= 游乐场 40。"""
    full = _load_full(path)
    return _resolve_active(full)


def list_targets(path) -> list:
    """预置四项恒在 + 自定义目标（元素为含 id/name/points/asset 的 dict）。"""
    full = _load_full(path)
    return _all_targets(full)


def add_custom_target(path, name, points, *, now=None) -> dict:
    """新增自定义奖励目标（OQ-3：name 非空 str；points 为 5–200 正整数）。

    id 由执行器生成（模型不产）、asset 恒 "sc-flag"（通用标记图）、revision +1。
    越界 / 畸形 raise ValueError 且**不写账**。
    """
    if not isinstance(name, str) or not name.strip():
        raise ValueError("add_custom_target name must be a non-empty string")
    if not _is_int(points):
        raise ValueError(
            f"add_custom_target points must be an integer, got {points!r} (bool excluded)")
    if points < CUSTOM_TARGET_MIN_POINTS or points > CUSTOM_TARGET_MAX_POINTS:
        raise ValueError(
            f"add_custom_target points must be within "
            f"{CUSTOM_TARGET_MIN_POINTS}-{CUSTOM_TARGET_MAX_POINTS}, got {points}")
    _coerce_now(now)

    with _critical(path):
        full = _load_full(path)
        target = {
            "id": _new_target_id(),
            "name": name.strip(),
            "points": int(points),
            "asset": CUSTOM_TARGET_ASSET,
        }
        full["custom_targets"] = list(full["custom_targets"]) + [target]
        full["revision"] = int(full["revision"]) + 1
        _write_state(path, full)
        revision = int(full["revision"])

    log.info(
        "points_target_added corr_id=%s target_id=%s points=%d revision=%d",
        uuid.uuid4().hex, target["id"], target["points"], revision)
    return dict(target)


def set_active_target(path, target_id, *, now=None) -> dict:
    """切换激活目标：revision +1、不产生任何分录（切目标不自动扣分）；未知 id 拒绝。

    历史兑换分录的 reason 快照是既有事实，切换不改写（arch 场景⑥E / K1L-E4）。
    """
    if not isinstance(target_id, str) or not target_id.strip():
        raise ValueError(
            f"set_active_target target_id must be a non-empty string, got {target_id!r}")
    _coerce_now(now)

    with _critical(path):
        full = _load_full(path)
        known_ids = {t["id"] for t in _all_targets(full)}
        if target_id not in known_ids:
            raise ValueError(f"set_active_target unknown target id {target_id!r}")
        full["active_target"] = target_id
        full["revision"] = int(full["revision"]) + 1
        _write_state(path, full)
        revision = int(full["revision"])
        active = _resolve_active(full)

    log.info(
        "points_target_active corr_id=%s target_id=%s revision=%d",
        uuid.uuid4().hex, target_id, revision)
    return active


def delete_target(path, target_id, *, now=None) -> dict:
    """删除自定义目标（非激活）：revision +1；预置 / 激活中 / 未知 id 一律拒绝（OQ-2）。"""
    if not isinstance(target_id, str) or not target_id.strip():
        raise ValueError(
            f"delete_target target_id must be a non-empty string, got {target_id!r}")
    _coerce_now(now)

    with _critical(path):
        full = _load_full(path)
        if target_id in {t["id"] for t in PRESET_TARGETS}:
            raise ValueError(
                f"delete_target refuses preset target {target_id!r} (presets are constants)")
        if target_id == full["active_target"]:
            raise ValueError(
                f"delete_target refuses active target {target_id!r} "
                "(switch to another target first)")
        remaining = [t for t in full["custom_targets"] if t["id"] != target_id]
        if len(remaining) == len(full["custom_targets"]):
            raise ValueError(f"delete_target unknown target id {target_id!r}")
        full["custom_targets"] = remaining
        full["revision"] = int(full["revision"]) + 1
        _write_state(path, full)
        revision = int(full["revision"])

    log.info(
        "points_target_deleted corr_id=%s target_id=%s revision=%d",
        uuid.uuid4().hex, target_id, revision)
    return {"id": target_id, "deleted": True, "revision": revision}


# --------------------------------------------------------------------------- #
# 权威载荷生产（完整载荷只由本模块产生；restore 由 front 按通道追加）              #
# --------------------------------------------------------------------------- #


def build_points_home(path) -> dict:
    """契约 §1.3⑦ 完整 points_home 载荷（**不含 restore 键**——restore 归 front 按通道加）。

    target = 当前激活目标（动态；出厂态 == DEFAULT_TARGET 四键，K0 语义零偏移）。
    """
    full = _load_full(path)
    return {
        "view": "points_home",
        "data": {
            "revision": int(full["revision"]),
            "balance": int(full["balance"]),
            "target": _resolve_active(full),
            "recent": [dict(entry) for entry in full["entries"][:HOME_RECENT_MAX]],
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
    if not _is_int(limit):
        raise ValueError(f"points page limit must be an integer, got {limit!r}")
    if limit < PAGE_LIMIT_MIN or limit > PAGE_LIMIT_MAX:
        raise ValueError(
            f"points page limit must be within {PAGE_LIMIT_MIN}-{PAGE_LIMIT_MAX}, got {limit}")
    return limit


def build_points_ledger_page(path, cursor=None, known_revision=None, limit=None) -> dict:
    """契约 §1.3⑧ 完整 points_ledger 载荷（无 restore 键；分页 wire 规则见契约 §四）。

    - cursor 缺省/None = 最新首页（page.reset=True）；
    - 续页按 (occurred_at, id) 时间倒序 keyset 定位，正常续页 page.reset=False；
    - 游标非法/过期，或 known_revision 与当前 revision 不一致 -> 回最新首页且 reset=True
      （插入 / 删除致 revision 变化时设备不得跨版本拼接缓存）；
    - page.total = 全量分录数（不用 items.length 代替）；limit 允许 1–50，越界 ValueError；
    - 空账 -> items=[], page.total=0, balance=0（权威空账，不伪造分录）。
    - 载荷**无 target 字段**（K0 ruling：仅 points_home 强制 target 四键）。
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
    if cursor is not None:
        decoded = _decode_cursor(cursor)
        if decoded is not None:
            boundary = (
                _occurred_seconds(decoded.get("occurred_at")), str(decoded.get("id")))
            version_ok = (known_revision is None
                          or (_is_int(known_revision) and int(known_revision) == revision))
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
# QQ 家长通道 CLI（261104-k3；`python -m gateway.points`，先 cd /opt/kidpal/front）  #
# 六子命令：add | redeem | delete | show | recent | targets                        #
# 回执恒单行 key=value（skill 解析口径与冻结验收同一形态）；拒绝不伪造成功           #
# --------------------------------------------------------------------------- #


class _UsageError(Exception):
    """用法/形状错误（无子命令 / 未知子命令 / 参数缺失或多余 / 非法选项）-> exit 2。

    与语义拒绝（ValueError -> exit 1）分开：exit 2 = 「命令本身没说对，重说一遍」，
    exit 1 = 「命令说清楚了但账本拒绝」（S-F6.4-AC4 一次性可判）。
    """


def _parse_cli_int(raw: str) -> int:
    text = raw.strip() if isinstance(raw, str) else ""
    if not _CLI_INT_RE.match(text):
        raise ValueError(f"points value must be a positive decimal integer, got {raw!r}")
    value = int(text)
    if value <= 0:
        raise ValueError(f"points value must be a positive integer, got {raw!r}")
    return value


def _cli_value(value) -> str:
    """回执值编码：只把空白字符转 `%XX`（中文与标点原样保留）。

    回执是单行 key=value、以空白分词解析（skill 与冻结验收同一口径）；事由含空格时
    只能编码空白位，否则一条回执会被拆成多个 token。值内容不改写、不截断。
    """
    return "".join(f"%{ord(ch):02X}" if ch.isspace() else ch for ch in str(value))


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
        "  python -m gateway.points add --points N --reason TEXT --command-id ID\n"
        "  python -m gateway.points add <points> <reason>          (legacy positional)\n"
        "  python -m gateway.points redeem --points N --command-id ID\n"
        "  python -m gateway.points redeem <points>                (legacy positional)\n"
        "  python -m gateway.points delete --entry-id pt_<hex> --command-id ID\n"
        "  python -m gateway.points delete --index N --command-id ID\n"
        "  python -m gateway.points show\n"
        "  python -m gateway.points recent [--limit N]             (default 5, 1-50)\n"
        "  python -m gateway.points targets\n"
        f"env: {POINTS_STATE_ENV} = points state file path (required)")


def _parse_flags(args, allowed) -> dict:
    """手写旗标解析（不用 argparse：`--points -3` 必须进语义拒绝而非被当未知选项）。

    未登记 token / 重复旗标 / 旗标缺值 -> _UsageError（exit 2）。
    """
    opts = {}
    position = 0
    while position < len(args):
        token = args[position]
        if token not in allowed:
            raise _UsageError(f"unexpected argument {token!r}")
        if token in opts:
            raise _UsageError(f"duplicate option {token}")
        if position + 1 >= len(args):
            raise _UsageError(f"{token} requires a value")
        opts[token] = args[position + 1]
        position += 2
    return opts


def _require(opts: dict, names) -> None:
    missing = [name for name in names if name not in opts]
    if missing:
        raise _UsageError("missing required option(s): " + ", ".join(missing))


def _entry_receipt_line(command: str, entry: dict, *, revision=None,
                        command_id=None) -> str:
    """成功回执行（单行 key=value；skill 按此解析，不另算权威值）。

    字段：entry_id / delta / 权威余额 / 事由 / **revision** / **command_id**
    （revision 与 command_id 是 K3 补的——skill 要把 command_id 念给家长，让重复
    成为可被发现的事实而非被静默吞掉）。短窗去重回执标 `duplicate=true`。
    """
    suffix = f" command_id={_cli_value(command_id)}" if command_id else ""
    if entry.get("duplicate") is True:
        revision = entry.get("revision") if revision is None else revision
        return (f"points {command} duplicate=true balance={entry.get('balance')} "
                f"revision={revision} reason=already_recorded{suffix}")
    return (f"points {command} ok entry_id={entry['id']} delta={int(entry['delta']):+d} "
            f"balance={int(entry['balance_after'])} "
            f"reason={_cli_value(entry['reason'])} revision={revision}{suffix}")


def _emit_entry_receipt(command: str, state_path: str, entry: dict,
                        command_id) -> int:
    """写成功后回执：revision 取写入后权威状态（同一把锁内的版本已落盘）。"""
    if entry.get("duplicate") is True:
        _emit(sys.stdout, _entry_receipt_line(command, entry, command_id=command_id))
        return 0
    revision = int(read_state(state_path)["revision"])
    _emit(sys.stdout, _entry_receipt_line(
        command, entry, revision=revision, command_id=command_id))
    return 0


def _cmd_add(state_path: str, argv) -> int:
    """`add`：旗标形（--points/--reason/--command-id）或既有位置形 `add <points> <reason>`。"""
    if argv and not argv[0].startswith("--"):
        # K0/K1 既有调用形态零放宽（test_points_e2e_spike / 既有 unit 直接这样调）。
        if len(argv) != 2:
            raise _UsageError("positional form is: add <points> <reason>")
        entry = add_points(state_path, _parse_cli_int(argv[0]), argv[1])
        return _emit_entry_receipt("add", state_path, entry, None)
    opts = _parse_flags(argv, ("--points", "--reason", "--command-id"))
    _require(opts, ("--points", "--reason", "--command-id"))
    entry = add_points(
        state_path, _parse_cli_int(opts["--points"]), opts["--reason"],
        command_id=opts["--command-id"])
    return _emit_entry_receipt("add", state_path, entry, opts["--command-id"])


def _cmd_redeem(state_path: str, argv) -> int:
    """`redeem`：旗标形（--points/--command-id）或既有位置形 `redeem <points>`。

    事由由执行器取「兑换<激活目标名>」，模型不提供；只认激活目标分值。
    """
    if argv and not argv[0].startswith("--"):
        if len(argv) != 1:
            raise _UsageError("positional form is: redeem <points>")
        entry = redeem_points(state_path, _parse_cli_int(argv[0]))
        return _emit_entry_receipt("redeem", state_path, entry, None)
    opts = _parse_flags(argv, ("--points", "--command-id"))
    _require(opts, ("--points", "--command-id"))
    entry = redeem_points(
        state_path, _parse_cli_int(opts["--points"]), command_id=opts["--command-id"])
    return _emit_entry_receipt("redeem", state_path, entry, opts["--command-id"])


def _cmd_delete(state_path: str, argv) -> int:
    """`delete`：`--entry-id` 与 `--index` 互斥且恰给一个；序号由执行器锁内换算。"""
    opts = _parse_flags(argv, ("--entry-id", "--index", "--command-id"))
    _require(opts, ("--command-id",))
    raw_id = opts.get("--entry-id")
    raw_index = opts.get("--index")
    command_id = opts["--command-id"]
    entry_id = None
    index = None
    if (raw_id is None) == (raw_index is None):
        raise ValueError(
            "delete needs exactly one locator: --entry-id pt_<hex> 或 --index <序号>"
            "（序号见 recent 回执）")
    if raw_index is not None:
        text = raw_index.strip()
        if not _CLI_INT_RE.match(text) or int(text) <= 0:
            raise ValueError(f"--index must be a positive integer, got {raw_index!r}")
        index = int(text)
    else:
        entry_id = raw_id.strip() if isinstance(raw_id, str) else raw_id
        if not isinstance(entry_id, str) or not _POINT_ENTRY_ID_RE.match(entry_id):
            raise ValueError(
                f"--entry-id must look like pt_<hex>, got {raw_id!r} "
                "（可用 recent 回执里的 entry_id）")
    receipt = delete_entry(state_path, entry_id, index=index, command_id=command_id)
    _emit(sys.stdout,
          f"points delete ok deleted={str(bool(receipt['deleted'])).lower()} "
          f"entry_id={_cli_value(receipt['id'])} revision={int(receipt['revision'])} "
          f"balance={int(receipt['balance'])} "
          f"command_id={_cli_value(command_id)}")
    return 0


def _cmd_show(state_path: str, argv) -> int:
    """`show`：只读单行（余额 / 版本 / 分录数 / 激活目标三键）。不创建 state 文件。"""
    if argv:
        raise _UsageError("show takes no arguments")
    state = read_state(state_path)
    target = get_active_target(state_path)
    _emit(sys.stdout,
          f"points show balance={int(state['balance'])} "
          f"revision={int(state['revision'])} "
          f"entries_total={len(state['entries'])} "
          f"active_target_id={_cli_value(target['id'])} "
          f"active_target_name={_cli_value(target['name'])} "
          f"active_target_points={int(target['points'])}")
    return 0


def _cmd_recent(state_path: str, argv) -> int:
    """`recent [--limit N]`：每条一行，**index 与 entry_id 同行**（index 1 = 最新）。

    序号→id 的换算权收在执行器手里：模型只许把 recent 打出来的 entry_id 回喂
    `delete --entry-id`，不得自己推断或编造 id。
    """
    opts = _parse_flags(argv, ("--limit",))
    limit = RECENT_LIMIT_DEFAULT
    if "--limit" in opts:
        text = opts["--limit"].strip()
        if not _CLI_INT_RE.match(text):
            raise ValueError(f"--limit must be an integer, got {opts['--limit']!r}")
        limit = int(text)
        if limit < PAGE_LIMIT_MIN or limit > PAGE_LIMIT_MAX:
            raise ValueError(
                f"--limit must be within {PAGE_LIMIT_MIN}-{PAGE_LIMIT_MAX}, got {limit}")
    state = read_state(state_path)
    for position, entry in enumerate(state["entries"][:limit], 1):
        _emit(sys.stdout,
              f"recent index={position} entry_id={_cli_value(entry['id'])} "
              f"delta={int(entry['delta']):+d} "
              f"balance_before={int(entry['balance_before'])} "
              f"balance_after={int(entry['balance_after'])} "
              f"reason={_cli_value(entry['reason'])} "
              f"occurred_at={_cli_value(entry['occurred_at'])}")
    return 0


def _cmd_targets(state_path: str, argv) -> int:
    """`targets`：只读列举预置四目标 + 自定义目标（id/name/points/asset）。"""
    if argv:
        raise _UsageError("targets takes no arguments")
    for target in list_targets(state_path):
        _emit(sys.stdout,
              f"target id={_cli_value(target['id'])} "
              f"name={_cli_value(target['name'])} "
              f"points={int(target['points'])} "
              f"asset={_cli_value(target['asset'])}")
    return 0


_CLI_COMMANDS = {
    "add": _cmd_add,
    "redeem": _cmd_redeem,
    "delete": _cmd_delete,
    "show": _cmd_show,
    "recent": _cmd_recent,
    "targets": _cmd_targets,
}


def main(argv=None) -> int:
    """`python -m gateway.points <子命令>` 入口（先 `cd /opt/kidpal/front`）。

    退出码三分：0 = 成功（含短窗去重 / 删除幂等 no-op 回执）；1 = 语义拒绝（非法值、
    歧义定位、越界、兑换门不过——**零写入**、说明走 stderr、stdout 无成功回执）；
    2 = 用法/环境错误（无/未知子命令、参数缺失或多余、`KIDPAL_POINTS_STATE` 未设）。

    state 路径单源 = os.environ[POINTS_STATE_ENV]（与 front 同一 env 名，无第二账本）；
    本模块是唯一 writer——CLI 从不替调用方判断两次命令「像重复就不调」（传输幂等与
    短窗语义去重都由执行器在锁内裁决）。
    """
    args = list(sys.argv[1:] if argv is None else argv)
    state_path = os.environ.get(POINTS_STATE_ENV)
    if not state_path:
        _emit(
            sys.stderr,
            f"points: {POINTS_STATE_ENV} is not set — refusing to write "
            "(no state file configured, no second ledger)")
        return 2
    if not args:
        _cli_usage(sys.stderr)
        return 2
    command = args[0]
    handler = _CLI_COMMANDS.get(command)
    if handler is None:
        _emit(sys.stderr, f"points: unknown subcommand {command!r}")
        _cli_usage(sys.stderr)
        return 2
    try:
        return handler(state_path, args[1:])
    except _UsageError as exc:
        _emit(sys.stderr, f"points {command}: {exc}")
        _cli_usage(sys.stderr)
        return 2
    except ValueError as exc:
        # 语义拒绝：不写账、不伪造成功回执（S-F6.4-AC4 一次性可判）。
        _emit(sys.stderr, f"points {command} rejected: {exc}")
        return 1
    except OSError as exc:
        # 持久化层硬失败（磁盘/权限）：同样不伪造成功，说明写 stderr。
        _emit(sys.stderr, f"points {command} failed: {exc}")
        return 1


if __name__ == "__main__":  # pragma: no cover — module CLI entry
    raise SystemExit(main())
