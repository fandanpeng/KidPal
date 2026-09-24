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
import os
import re
import unicodedata
from collections import OrderedDict, deque
from datetime import datetime, timedelta, timezone
from pathlib import Path

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


# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — 归档 / 读回 / 下发
# --------------------------------------------------------------------------- #


def archive_story(stories_dir, transcript, *, title=None, slug=None, date=None) -> dict:
    """把整段转写真实落盘为 <stories_dir>/<id>.md，返回记录 {id,title,date,text,path}。

    transcript 为 None / 空 / 仅空白 -> 抛 ValueError（空故事不入库、不写文件）。
    同 id 再归档 -> 覆盖同一文件（幂等重写，不产生第二颗、不涨计数）。"""
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
    path.write_text(_render_record(story_id, title, date, text), encoding="utf-8")
    return {"id": story_id, "title": title, "date": date, "text": text, "path": str(path)}


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
    total = len(records)  # 全集计数，绝不以 len(items) 顶替
    ordered = sorted(records, key=lambda r: (r["date"], r["id"]), reverse=True)

    start = offset if (offset and offset > 0) else 0
    page = ordered[start:] if limit is None else ordered[start:start + limit]
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
    return {
        "view": "story_text",
        "data": {
            "id": record.get("id", ""),
            "title": record.get("title", ""),
            "text": record.get("text", ""),
        },
    }


# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — StoryAccumulator (manual 聆听累积态 + 归档 + V31 回退)
# --------------------------------------------------------------------------- #


class _SessionState:
    __slots__ = ("routing", "accum", "accum_chars", "ring", "ring_chars",
                 "last_record", "last_stop_now", "last_transcript")

    def __init__(self) -> None:
        self.routing = "chat"
        self.accum: list[str] = []
        self.accum_chars = 0
        self.ring: deque[str] = deque()
        self.ring_chars = 0
        self.last_record: dict | None = None
        self.last_stop_now = None
        self.last_transcript: str | None = None


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

    def start(self, session_id, now=None) -> None:
        st = self._ensure(session_id)
        # 幂等：重复 start 不重复开会话、不清空已在累积的内容
        st.routing = "accumulate"

    def feed(self, session_id, text, now=None) -> int:
        st = self._ensure(session_id)
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
