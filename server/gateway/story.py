"""KidPal P3 故事云端面 — 整段转写归档 / story_list·story_text 下发 / V31 四条回退 /
有界累积缓冲 (TaskPackage 260829-story-p3, 契约 §1.3 ③/④ / §1.4 / spec S-F2.2/S-F2.5)。

语义权威 = 冻结验收 server/acceptance/test_story_p3.acceptance.py 文件头
(frozen offline contract)：archive_story / read_story_file / list_stories / story_text /
StoryAccumulator。纯 python、import 期不碰 fastapi/gateway-front —— offline 任何位置可
导入（同 gateway/schedule.py、gateway/display_schema.py 纯模块先例）；下发的 story_list /
story_text 块可原样过 gateway.display_schema.validate_display 机械门。

归属边界：归档落盘 stories/*.md 由本模块（云侧网关）承载；设备只消费下发的 story_list /
story_text（哑终端纯洁性）。S-F2.5「起标题」归 OpenClaw（一回合不读写文件），本模块只在
停录后把整段转写落盘并对缺标题做非空回退。契约对齐（2026-09-24 契约门，CDQ-260924 三行）：story_text 含可选 seq（入册
流水号账本归本模块，只增不减）；story_list items = 最新 ≤16 条窗口 + total =
现存篇数（盖章 N 不再取 total）；不新增 view（生字反白高亮 P3-E4 已裁、不消费）。

内存叠压红线 (notes.blockingDecision_memoryPressure)：StoryAccumulator 的每会话近句环 +
累积缓冲双上限、会话数 max_sessions + LRU，绝不无界增长（char-stroke §8.3 19MB 教训）。
"""

from __future__ import annotations

import base64
import hashlib
import json
import logging
import os
import re
import unicodedata
from collections import OrderedDict, deque
from datetime import datetime, timedelta, timezone
from pathlib import Path

log = logging.getLogger("kidpal-gateway.story")

# 归档记录头哨兵 + 字段标记（list_stories 据此区分「真故事」与杂物文件 README/.txt）。
_SENTINEL = "<!-- kidpal-story:1 -->"
_BODY_START = "<!-- kidpal-body-start -->"
_ID_RE = re.compile(r"<!-- kidpal-id:(.*?) -->")
_DATE_RE = re.compile(r"<!-- kidpal-date:(.*?) -->")
_TITLE_RE = re.compile(r"<!-- kidpal-title:(.*?) -->")

# 云端部署在中国时区；无 tz 库依赖，用固定 +08:00 偏移取当日 (仅当调用方未显式传 date)。
_CN_TZ = timezone(timedelta(hours=8))

# 标题回退切分用标点集（中英文常见句读）。
_PUNCT_SPLIT_RE = re.compile(r"[。，、；：！？,.;:!?…\n\r]")
_NONWORD_STRIP_RE = re.compile(r"[^\w]+")

# 契约 §1.3③ Q2 丙案（2026-09-24 契约门）：story_list items = 最新 ≤16 条窗口
# （= 板端 KSTORY_LIST_MAX，firmware/main/display/story_page.h:40；超限整单 -1 降级
# 维持为防御面）；total 恒全量现存篇数，绝不以 len(items) 顶替（load-bearing）。
STORY_LIST_WINDOW = 16

# requirements §2.1 录音态超时口径：累积态无语音 >= AUTO_STOP_SECONDS 秒自动收尾归档。
AUTO_STOP_SECONDS = 120

# 累积态收尾词（S-F2.2 收尾，front 机械扫描 stt 文本命中）——精确短语 + 末尾标点不
# 阻断（见 is_finish_phrase 子串语义）。
_FINISH_PHRASES = ("我说完了", "讲完啦")

# 入册流水号持久账本（plan 取舍点 B 甲案）：<stories_dir>/.seq.json，原子写、front 单写者。
_LEDGER_NAME = ".seq.json"
# 账本缺失/损坏重建时的安全边际（重建后新 id 的 seq 严格 > 重建前最大 seq，绝不回退/复用）。
_LEDGER_REBUILD_MARGIN = 1

# 非法 id 判据（路径穿越 / 分隔符）——delete_story 拒绝路径。
_ID_UNSAFE_RE = re.compile(r"[/\\]")


# --------------------------------------------------------------------------- #
# 小工具（id / slug / title 回退 / 日期）
# --------------------------------------------------------------------------- #


def _today_cn() -> str:
    return datetime.now(_CN_TZ).strftime("%Y-%m-%d")


def _slugify(text: str) -> str:
    """把可读文本转为 ASCII 安全 slug（小写 / 短横 / 去非字母数字）。

    纯 CJK 等无 ASCII 可转写时返回 ""（调用方回退到稳定散列）。"""
    if not text:
        return ""
    norm = unicodedata.normalize("NFKD", text)
    ascii_bytes = norm.encode("ascii", "ignore").decode("ascii")
    lowered = ascii_bytes.lower()
    slug = re.sub(r"[^a-z0-9]+", "-", lowered).strip("-")
    return slug


def _hash_slug(source: str) -> str:
    """无可用 ASCII slug 时的稳定散列回退（由入参决定、可复现）。"""
    digest = hashlib.sha1(source.encode("utf-8", "replace")).hexdigest()
    return "s" + digest[:8]


def _fallback_title(text: str) -> str:
    """title 缺省时的**非空**回退：首个标点前的片段，截断至 <=12 字符（列表行显示位）。

    整段都是标点/无实义片段时，剥离非词字符再取；仍空则回退到截断原文（保证非空）。"""
    stripped = (text or "").strip()
    seg = _PUNCT_SPLIT_RE.split(stripped, maxsplit=1)[0].strip()
    if not seg:
        seg = _NONWORD_STRIP_RE.sub("", stripped)
    seg = seg[:12].strip()
    if not seg:
        seg = stripped[:12].strip() or "故事"
    return seg[:12]


def is_finish_phrase(text) -> bool:
    """累积态收尾词机械检测（S-F2.2 收尾；M4K1-A5 冻结口径）。

    text（str）剔除首尾空白后**包含**精确短语「我说完了」或「讲完啦」-> True；其余
    （空串/None/非 str/无关文本）-> False。可读文本子串匹配，末尾标点不阻断
    （「我说完了。」含「我说完了」）。仅累积态消费（问答会话零影响由 front 调用点保证）。"""
    if not isinstance(text, str):
        return False
    stripped = text.strip()
    if not stripped:
        return False
    return any(phrase in stripped for phrase in _FINISH_PHRASES)


# --------------------------------------------------------------------------- #
# 入册流水号持久账本（契约 §1.3④ data.seq；plan 取舍点 B 甲案）
# --------------------------------------------------------------------------- #


def _ledger_path(stories_dir) -> Path:
    return Path(stories_dir) / _LEDGER_NAME


def _load_ledger(stories_dir) -> "tuple[dict | None, str]":
    """读 <stories_dir>/.seq.json -> (ledger | None, state)。

    state ∈ {"ok","missing","corrupt"}。ledger 形状 {"next_seq": int>=1, "by_id": {id: seq}}；
    缺失/不可读/坏 JSON/形状失真一律返回 (None, 原因)，交调用方重建（绝不静默当空账本
    从 1 重发——那会把 seq 回退当可接受降级）。"""
    p = _ledger_path(stories_dir)
    try:
        text = p.read_text(encoding="utf-8")
    except OSError:
        return None, "missing"
    try:
        raw = json.loads(text)
    except (ValueError, TypeError):
        return None, "corrupt"
    if not isinstance(raw, dict):
        return None, "corrupt"
    by_id = raw.get("by_id")
    next_seq = raw.get("next_seq")
    if (not isinstance(by_id, dict)
            or not isinstance(next_seq, int) or isinstance(next_seq, bool) or next_seq < 1):
        return None, "corrupt"
    clean = {k: v for k, v in by_id.items()
             if isinstance(k, str) and isinstance(v, int) and not isinstance(v, bool)}
    return {"next_seq": next_seq, "by_id": clean}, "ok"


def _rebuild_ledger(stories_dir) -> dict:
    """账本缺失/损坏时的重建：按现存可解析故事记录分配 id->seq（决定序：日期+id 升序），
    下一条 next_seq = 现存篇数 + 安全边际。

    安全边际保证「重建后新 id 的 seq > 重建前最大 seq」（不把 seq 回退/复用当可接受降级）。
    空目录 -> {next_seq: 1, by_id: {}}（首篇 seq=1）。"""
    records = _collect_records(stories_dir)
    ordered = sorted(records, key=lambda r: (r["date"], r["id"]))
    by_id = {r["id"]: i for i, r in enumerate(ordered, start=1)}
    next_seq = (len(ordered) + _LEDGER_REBUILD_MARGIN) if ordered else 1
    return {"next_seq": next_seq, "by_id": by_id}


def _save_ledger(stories_dir, ledger) -> None:
    """原子写账本（临时文件 + os.replace），front 单写者。写失败记日志、不抛（归档不因
    账本 I/O 失败而崩——已分配 seq 仍随 record 返回）。"""
    d = Path(stories_dir)
    try:
        d.mkdir(parents=True, exist_ok=True)
    except OSError as e:  # noqa: BLE001
        log.warning("story_seq_ledger_mkdir_err err=%s", type(e).__name__)
        return
    p = d / _LEDGER_NAME
    tmp = p.with_name(f"{p.name}.tmp.{os.getpid()}")
    payload = json.dumps(
        {"version": 1, "next_seq": ledger["next_seq"], "by_id": ledger["by_id"]},
        ensure_ascii=False, sort_keys=True)
    try:
        with open(tmp, "w", encoding="utf-8") as f:
            f.write(payload)
            f.flush()
            try:
                os.fsync(f.fileno())
            except OSError:
                pass
        os.replace(str(tmp), str(p))
    except OSError as e:  # noqa: BLE001
        log.warning("story_seq_ledger_write_err err=%s", type(e).__name__)
        try:
            if tmp.exists():
                tmp.unlink()
        except OSError:
            pass


def _assign_seq(stories_dir, story_id) -> int:
    """为 story_id 分配/复用入册流水号（max+1 语义，只增不减）。

    - 同 id 再归档（幂等重写）-> 复用账本既有 seq（不变、不涨计数）；
    - 新 id -> next_seq（首篇 = 1）+ 账本进位；
    - 账本缺失/损坏 -> 从现存文件重建 + 告警留痕（producer doc 强制「不把 seq 回退当
      可接受降级」）。"""
    ledger, state = _load_ledger(stories_dir)
    if ledger is None:
        ledger = _rebuild_ledger(stories_dir)
        if ledger["by_id"]:
            log.warning(
                "story_seq_ledger_rebuild corr_id=story state=%s dir=%s recovered_ids=%d "
                "next_seq=%d — 账本缺失/损坏，从现存文件重建（seq 不回退/不复用）",
                state, stories_dir, len(ledger["by_id"]), ledger["next_seq"])
        _save_ledger(stories_dir, ledger)
    existing = ledger["by_id"].get(story_id)
    if existing is not None:
        return existing
    seq = ledger["next_seq"]
    ledger["by_id"][story_id] = seq
    ledger["next_seq"] = seq + 1
    _save_ledger(stories_dir, ledger)
    return seq


# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — 归档 / 读回 / 下发
# --------------------------------------------------------------------------- #


def archive_story(stories_dir, transcript, *, title=None, slug=None, date=None) -> dict:
    """把整段转写真实落盘为 <stories_dir>/<id>.md，返回记录 {id,title,date,text,path}。

    transcript 为 None / 空 / 仅空白 -> 抛 ValueError（空故事不入库、不写文件）。
    同 id 再归档 -> 覆盖同一文件（幂等重写，不产生第二颗、不涨计数）。

    additive（2026-09-24 契约门 §1.3④）：返回 record 增 "seq"（int）= 该故事入册流水号；
    持久账本 <stories_dir>/.seq.json（原子写、front 单写者）承载，只增不减（删除不回退、
    同 id 重写不变、重启存活、账本缺失重建不倒退）。"""
    if transcript is None or not str(transcript).strip():
        raise ValueError("空转写不归档：archive_story 需要非空 transcript（空故事不得入册）。")
    text = str(transcript).strip()

    date = (date or "").strip() or _today_cn()
    title = (title or "").strip() or _fallback_title(text)

    base_slug = _slugify(slug) if (slug and str(slug).strip()) else ""
    if not base_slug:
        base_slug = _slugify(title)
    if not base_slug:
        base_slug = _hash_slug(f"{title}|{date}")
    story_id = f"{date}-{base_slug}"

    d = Path(stories_dir)
    d.mkdir(parents=True, exist_ok=True)
    path = d / f"{story_id}.md"
    # 先分配流水号（账本为 seq 权威）再落 MD；分配后落盘失败只留 seq 缺口（只增不减允许）。
    seq = _assign_seq(d, story_id)
    path.write_text(_render_record(story_id, title, date, text), encoding="utf-8")
    return {"id": story_id, "title": title, "date": date, "text": text,
            "seq": seq, "path": str(path)}


def _render_record(story_id: str, title: str, date: str, text: str) -> str:
    """落盘 MD：机器可读头（哨兵 + id/date/title-b64 标记）+ 人类可读标题 + 正文。

    标题以 base64 存头（可含冒号/换行不破坏标记），同时以 `# title` 明文出现在正文区，
    供「归档文件可读、含标题与整段转写」断言。"""
    title_b64 = base64.b64encode(title.encode("utf-8")).decode("ascii")
    return (
        f"{_SENTINEL}\n"
        f"<!-- kidpal-id:{story_id} -->\n"
        f"<!-- kidpal-date:{date} -->\n"
        f"<!-- kidpal-title:{title_b64} -->\n"
        f"\n# {title}\n\n"
        f"{_BODY_START}\n"
        f"{text}\n"
    )


def read_story_file(path) -> dict | None:
    """读回一个归档文件为 record；非故事记录文件（无本模块记录头）-> None。"""
    if path is None:
        return None
    p = Path(path)
    if not p.is_file():
        return None
    try:
        content = p.read_text(encoding="utf-8")
    except OSError:
        return None
    if _SENTINEL not in content:
        return None  # 杂物文件（README/.txt）不是本模块记录

    id_m = _ID_RE.search(content)
    date_m = _DATE_RE.search(content)
    title_m = _TITLE_RE.search(content)
    story_id = id_m.group(1).strip() if id_m else p.stem
    date = date_m.group(1).strip() if date_m else ""
    title = ""
    if title_m:
        try:
            title = base64.b64decode(title_m.group(1).strip()).decode("utf-8")
        except Exception:  # noqa: BLE001 — 头损坏按无标题回退，不崩
            title = ""

    idx = content.find(_BODY_START)
    if idx >= 0:
        body = content[idx + len(_BODY_START):].strip()
    else:
        body = ""
    return {"id": story_id, "title": title, "date": date, "text": body, "path": str(p)}


def _display_date(date_iso: str) -> str:
    """完整日期 'YYYY-MM-DD' -> 显示 'MM-DD'（跨年比较仍按完整日期）。"""
    s = (date_iso or "").strip()
    if re.match(r"^\d{4}-\d{2}-\d{2}$", s):
        return s[5:]
    return s


def list_stories(stories_dir, *, limit=None, offset=0) -> dict:
    """契约 §1.3 ③ story_list producer 下发块：total = 全集计数（与分页无关），
    items 按完整日期倒序（同日按 id 倒序）、date 显示 'MM-DD'、杂物文件不计入。

    limit/offset 只切 items 页、绝不动 total（契约 §1.3③：total = 现存篇数 = 星星
    计数 ×N；盖章 N 取 story_text.data.seq——2026-09-24 契约门拆分）；offset 超界
    -> items [] 而 total 原值。整包过
    validate_display（验收机械断言）。"""
    records = _collect_records(stories_dir)
    total = len(records)  # 全量现存篇数，绝不以 len(items) 顶替
    ordered = sorted(records, key=lambda r: (r["date"], r["id"]), reverse=True)

    start = offset if (offset and offset > 0) else 0
    # 窗口上限：显式 limit 覆盖（既有分页语义保留），否则恒取最新 ≤STORY_LIST_WINDOW 条
    # （契约 §1.3③ Q2 丙案——items 是显示窗口，total 才是计数，二者独立）。
    window = STORY_LIST_WINDOW if limit is None else limit
    page = ordered[start:start + window]
    items = [{"id": r["id"], "title": r["title"], "date": _display_date(r["date"])}
             for r in page]
    return {"view": "story_list", "data": {"total": total, "items": items}}


def _collect_records(stories_dir) -> list:
    """扫描目录内所有真实故事记录（有本模块记录头），返回 record 列表（未排序）。

    目录不存在 / 空 -> []（total 0，非假装有故事）。"""
    d = Path(stories_dir)
    if not d.is_dir():
        return []
    records = []
    for p in sorted(d.glob("*.md")):
        rec = read_story_file(str(p))
        if rec is not None:
            records.append(rec)
    return records


def story_text(record) -> dict:
    """契约 §1.3 ④ story_text 下发块：data keys {id,title,text} + 可选 seq（入册
    流水号、只增不减——2026-09-24 契约门 additive），无生字标注/高亮字段（P3-E4）。整包过 validate_display（验收机械断言）。"""
    data = {
        "id": record.get("id", ""),
        "title": record.get("title", ""),
        "text": record.get("text", ""),
    }
    seq = record.get("seq")
    if isinstance(seq, int) and not isinstance(seq, bool):
        # additive（契约 §1.3④ / 2026-09-24 契约门）：seq 仅当 record 携带整数 seq 时出现
        # （缺省则 data keys 恰为 {id,title,text}，设备行为不变）。
        data["seq"] = seq
    return {"view": "story_text", "data": data}


# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — 删除执行面（契约 §四 kidpal_story_delete / Q6 硬删）
# --------------------------------------------------------------------------- #


def delete_story(stories_dir, story_id, *, corr_id=None) -> bool:
    """契约 §四 kidpal_story_delete 的机械执行面（硬删、无恢复；Q6 裁定）。

    - story_id 必须为非空 str、不含路径分隔符 /「..」；解析 <stories_dir>/<story_id>.md；
    - 必须经哨兵头校验（read_story_file(path) is not None）——路径穿越 / 无哨兵头杂物文件
      （README.md 等）/ 不存在的 id 一律**拒绝**（返回 False + 审计日志留痕），绝不删除
      stories_dir 以外的文件；
    - 合法 -> 真删该 MD 文件（硬删）-> True + 审计日志；
    - 流水号账本不变（seq 不回退；删除的产品边界 = 仅 story MD，Q6）；
    - 纯机械、绝不抛异常（畸形 id 归 False，幂等 no-op 不报错）。"""
    if not isinstance(story_id, str):
        log.warning("story_delete rejected corr_id=%s id=%r reason=invalid_type",
                    corr_id, story_id)
        return False
    sid = story_id.strip()
    if not sid or ".." in sid or _ID_UNSAFE_RE.search(sid):
        log.warning("story_delete rejected corr_id=%s id=%s reason=unsafe_id", corr_id, sid)
        return False
    d = Path(stories_dir)
    path = d / f"{sid}.md"
    # 双保险：解析后必须落在 stories_dir 内（拒绝任何逃逸）。
    try:
        if path.resolve().parent != d.resolve():
            log.warning("story_delete rejected corr_id=%s id=%s reason=outside_dir",
                        corr_id, sid)
            return False
    except OSError:
        log.warning("story_delete rejected corr_id=%s id=%s reason=resolve_error",
                    corr_id, sid)
        return False
    if read_story_file(str(path)) is None:
        # 不存在 / 无哨兵头杂物 -> 拒绝（幂等 no-op），审计留痕（绝不删非故事 MD）。
        log.warning("story_delete rejected corr_id=%s id=%s reason=no_sentinel_or_missing",
                    corr_id, sid)
        return False
    try:
        path.unlink()
    except OSError as e:  # noqa: BLE001 — 删除失败按拒绝处理，不抛
        log.warning("story_delete rejected corr_id=%s id=%s reason=unlink_failed err=%s",
                    corr_id, sid, type(e).__name__)
        return False
    log.info("story_delete ok corr_id=%s id=%s path=%s", corr_id, sid, path)
    return True


# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — StoryAccumulator (manual 聆听累积态 + 归档 + V31 回退)
# --------------------------------------------------------------------------- #


class _SessionState:
    __slots__ = ("routing", "accum", "accum_chars", "ring", "ring_chars",
                 "last_record", "last_stop_now", "last_transcript", "last_activity")

    def __init__(self) -> None:
        self.routing = "chat"
        self.accum: list[str] = []
        self.accum_chars = 0
        self.ring: deque[str] = deque()
        self.ring_chars = 0
        self.last_record: dict | None = None
        self.last_stop_now = None
        self.last_transcript: str | None = None
        # 最近活动时刻（start()/feed() 传入 now 或墙钟）——timed_out_sessions 的纯决策输入。
        self.last_activity = None


class StoryAccumulator:
    """manual 聆听累积态 + 归档 + V31 四条回退（契约 §1.4 start_recording + spec S-F2.2）。

    纯逻辑 + 真实落盘（委托 archive_story）；不 import fastapi/websockets；会话标识由
    调用方（front/engine）传入。缓冲严格有界：每会话近句环句数 + 字符双上限、累积缓冲
    上限、max_sessions + LRU，取消/停录后释放（notes 内存压力硬约束）。
    """

    def __init__(self, stories_dir, *, sentence_window=24, char_window=4000,
                 max_sessions=8, redo_window_s=10.0) -> None:
        # __init__ 不触碰文件系统（import/构造期零副作用；仅在真正归档时由 archive_story
        # 惰性建目录），使 front 消费方在模块加载期实例化也不产生磁盘副作用。
        self._dir = str(stories_dir)
        self._sentence_window = max(1, int(sentence_window))
        self._char_window = max(1, int(char_window))
        self._max_sessions = max(1, int(max_sessions))
        self._redo_window_s = float(redo_window_s)
        self._sessions: "OrderedDict[str, _SessionState]" = OrderedDict()

    # --- 内部：会话获取 / LRU 有界 / 缓冲压入 ----------------------------------

    def _get(self, session_id) -> "_SessionState | None":
        st = self._sessions.get(session_id)
        if st is not None:
            self._sessions.move_to_end(session_id)  # LRU touch（读也算活动）
        return st

    def _ensure(self, session_id) -> _SessionState:
        st = self._sessions.get(session_id)
        if st is None:
            st = _SessionState()
            self._sessions[session_id] = st
        else:
            self._sessions.move_to_end(session_id)
        # LRU 全局界：超 max_sessions 淘汰最久未活动会话（释放其缓冲）
        while len(self._sessions) > self._max_sessions:
            self._sessions.popitem(last=False)
        return st

    def _push_ring(self, st: _SessionState, text: str) -> None:
        st.ring.append(text)
        st.ring_chars += len(text)
        # FIFO 淘汰最旧（保 newest「刚才那段」），句数 + 字符双上限
        while len(st.ring) > 1 and (
                len(st.ring) > self._sentence_window or st.ring_chars > self._char_window):
            old = st.ring.popleft()
            st.ring_chars -= len(old)

    def _push_accum(self, st: _SessionState, text: str) -> None:
        st.accum.append(text)
        st.accum_chars += len(text)
        # 累积缓冲同样有界（内存红线）：极端超长时丢最旧句
        while len(st.accum) > 1 and st.accum_chars > self._char_window:
            old = st.accum.pop(0)
            st.accum_chars -= len(old)

    # --- 路由 / 生命周期 -------------------------------------------------------

    def routing(self, session_id, now=None) -> str:
        st = self._get(session_id)
        return st.routing if st is not None else "chat"

    def timed_out_sessions(self, now, *, timeout_s=None) -> list:
        """累积态无语音自动收尾的**纯决策面**（requirements §2.1 录音态超时口径；M4K1-A5）。

        返回 routing=="accumulate" 且「最近活动时刻」（= start()/feed() 传入的 now 或墙钟）
        距 now >= timeout_s 的会话 id 列表。

        - timeout_s 缺省 = AUTO_STOP_SECONDS（120）；边界语义 >=（恰 timeout_s 计入）；
        - 未记录活动的会话 / 非累积态会话 / 已 stop·cancel 的会话 -> 不返回；
        - 查询不改状态（不触碰 LRU、不动缓冲）。"""
        if now is None:
            now = datetime.now(timezone.utc)
        threshold = AUTO_STOP_SECONDS if timeout_s is None else float(timeout_s)
        out = []
        for sid, st in self._sessions.items():
            if st.routing != "accumulate" or st.last_activity is None:
                continue
            try:
                elapsed = (now - st.last_activity).total_seconds()
            except TypeError:
                continue  # 不可比时刻（时区/类型）-> 不误判超时
            if elapsed >= threshold:
                out.append(sid)
        return out

    def start(self, session_id, now=None) -> None:
        st = self._ensure(session_id)
        # 最近活动时刻（供 120s 无语音超时决策；now 缺省用墙钟）。
        st.last_activity = now if now is not None else datetime.now(timezone.utc)
        # 幂等：重复 start 不重复开会话、不清空已在累积的内容
        st.routing = "accumulate"

    def feed(self, session_id, text, now=None) -> int:
        st = self._ensure(session_id)
        st.last_activity = now if now is not None else datetime.now(timezone.utc)
        if text is None or not str(text).strip():
            return len(st.ring)  # 空句不入库、不计一句（不抛）
        sentence = str(text)
        self._push_ring(st, sentence)
        if st.routing == "accumulate":
            # 累积态：进累积缓冲（供停录整段归档），同时进有界近句环；不按句送对话
            self._push_accum(st, sentence)
        return len(st.ring)

    def stop(self, session_id, now=None, *, title=None, slug=None, date=None):
        st = self._get(session_id)
        if st is None or not st.accum:
            # 累积为空 -> None 且不写文件；会话回闲聊态
            if st is not None:
                st.routing = "chat"
                st.accum = []
                st.accum_chars = 0
            return None
        transcript = "\n".join(st.accum)
        rec = archive_story(self._dir, transcript, title=title, slug=slug, date=date)
        # 记录撤销凭据（供 redo 在窗口内撤销）
        st.last_record = rec
        st.last_stop_now = now
        st.last_transcript = transcript
        # 释放累积缓冲 + 回闲聊态（近句环保留：已入册内容不再 recover 也不重复计，
        # 但 recover 只用于漏判路径；停录内容不再走 recover 见测试语义）
        st.accum = []
        st.accum_chars = 0
        st.ring = deque()
        st.ring_chars = 0
        st.routing = "chat"
        return rec

    def cancel(self, session_id, now=None, *, reason="") -> bool:
        st = self._get(session_id)
        if st is None:
            return False
        dropped = bool(st.accum)
        # V31 discard：丢弃已累积内容、不归档；并删除会话态（释放缓冲、且不可被
        # recover 挽回 —— 「丢弃即丢弃」，与漏判 recover 两条路径可分）
        self._sessions.pop(session_id, None)
        return dropped

    def redo(self, session_id, now=None, *, title=None, slug=None):
        st = self._get(session_id)
        if st is None or st.last_record is None:
            return None  # 无上一归档 -> None，归档保持不动
        # 窗口判定用注入时刻（可测、可复现），不用墙钟
        if now is None or st.last_stop_now is None:
            return None
        elapsed = (now - st.last_stop_now).total_seconds()
        if elapsed > self._redo_window_s:
            return None  # 超窗 -> 拒绝，已入册故事保持
        undone = st.last_record
        try:
            p = Path(undone["path"])
            if p.is_file():
                p.unlink()  # 真删刚写的文件
        except OSError:
            pass  # 删失败仍回滚会话态（归档已不存在则下次 list 自然反映）
        # 转写放回累积缓冲 + 回累积态（孩子重新说一遍即可）
        restored = (st.last_transcript or "").split("\n") if st.last_transcript else []
        st.accum = [s for s in restored if s != ""]
        st.accum_chars = sum(len(s) for s in st.accum)
        st.routing = "accumulate"
        st.last_record = None
        st.last_stop_now = None
        st.last_transcript = None
        return undone

    def recover(self, session_id, now=None, *, title=None, slug=None, date=None):
        st = self._get(session_id)
        if st is None or not st.ring:
            return None  # 环空 / 已被 cancel 删除 -> 不编造归档
        transcript = "\n".join(st.ring)
        if not transcript.strip():
            return None
        rec = archive_story(self._dir, transcript, title=title, slug=slug, date=date)
        # 挽回一次即清环（不重复入册）
        st.ring = deque()
        st.ring_chars = 0
        return rec

    # --- 可测面（内存叠压断言） -------------------------------------------------

    def recent_depth(self, session_id) -> int:
        st = self._get(session_id)
        return len(st.ring) if st is not None else 0

    def recent_chars(self, session_id) -> int:
        st = self._get(session_id)
        return st.ring_chars if st is not None else 0

    def active_sessions(self) -> list:
        return list(self._sessions.keys())

    def total_buffer_bytes(self) -> int:
        return sum(s.accum_chars + s.ring_chars for s in self._sessions.values())
