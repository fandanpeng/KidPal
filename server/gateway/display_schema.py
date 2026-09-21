"""Mechanical ```display schema validation (V27 / contract §1.3 / §1.4 / §1.5).

Frozen acceptance GWA-A5. Pure-python, no fastapi at import time.

Frozen offline contract:
    validate_display(payload) -> (ok: bool, reason: str)
        VALID_VIEWS == {char, schedule, story_list, story_text, text, home, control}
        MAX_DATA_BYTES  (data size cap)
        reject: non-dict /
                view not in whitelist /
                oversize data /
                control w/ unknown action /
                a raw control-end_session arriving from the LLM display stream
                (forged -> not forwardable; contract §1.4 end_session is a gateway
                 intent decision, NEVER blindly forwarded to the board render path)

The gateway runs this on every ```display block split out of the LLM reply (契约 §1.5
step 3): a rejected payload is downgraded to a friendly text view (or dropped), never
sent to the board's blind-render path.
"""

from __future__ import annotations

import json

# 契约 §1.3 view 白名单.
VALID_VIEWS = {
    "char",
    "schedule",
    "story_list",
    "story_text",
    "text",
    "home",
    "control",
}

# data 大小上限 (契约 §1.5 / V27). 64 KB serialised JSON is far beyond any real
# display payload; a 100k-char data blob (≈300KB UTF-8) stays well above the cap.
MAX_DATA_BYTES = 65536

# 契约 §1.4 控制 action 表. end_session 是网关切意图后的决定, 不是可被 LLM 流
# 盲转发的 display 意图 —— 伪造/注入的 control-end_session 必须在机械层拒绝.
GATEWAY_DECISION_CONTROL_ACTIONS = frozenset({"end_session"})
# 可由 display 流透传的 control action.
FORWARDABLE_CONTROL_ACTIONS = frozenset({"stop_speaking", "start_recording"})


def _data_bytes(data) -> int:
    try:
        return len(json.dumps(data, ensure_ascii=False).encode("utf-8"))
    except (TypeError, ValueError):
        return -1  # not JSON-serialisable -> caller rejects


def validate_display(payload) -> tuple[bool, str]:
    """Validate a ```display JSON payload for forwardability to the board.

    Returns (ok: bool, reason: str). (True, "ok") means the payload passed the
    mechanical schema check and is safe to relay; anything else must be downgraded.
    """
    if not isinstance(payload, dict):
        return (False, "display payload must be a JSON object, not a raw/dirty value")

    view = payload.get("view")
    if view not in VALID_VIEWS:
        return (False, f"view {view!r} is not in whitelist {sorted(VALID_VIEWS)}")

    data = payload.get("data")

    # control view: action must be a known *forwardable* control intent.
    if view == "control":
        if not isinstance(data, dict):
            return (False, "control view requires data.action map")
        action = data.get("action")
        if action in GATEWAY_DECISION_CONTROL_ACTIONS:
            # 契约 §1.4: end_session 是网关切意图后的决定, 伪造/注入的必须拒绝.
            return (False, "control/end_session is a gateway intent decision, not forwardable")
        if action not in FORWARDABLE_CONTROL_ACTIONS:
            return (False, f"control action {action!r} unknown / not forwardable")

    # data must be an object for data-carrying views; home carries none.
    if data is not None and not isinstance(data, dict):
        return (False, "display data must be a JSON object, not a scalar/array")
    if data is None and view != "home":
        return (False, f"view {view!r} requires a data object")

    # Q5 收紧（契约 §1.5 step 3 / M3-K1 契约门裁定）：schedule view data 只放行
    # 意图薄载荷 {}（取舍点 A 甲案）或新周网格形状 {week:{courses:[{day,slot,name,flags}]}}；
    # 旧 {title, items} 列表形与缺 courses 的畸形周网格一律拒绝并降级提示。网格由 front
    # 机械计算（schedule.py 单源）——此校验只把关 shape floor，不作逐字段深度类型校验。
    if view == "schedule" and data != {}:
        week = data.get("week") if isinstance(data, dict) else None
        if not isinstance(week, dict):
            return (False, "schedule view data must be {} (thin) or a week-grid "
                           "{week:{courses:[{day,slot,name,flags}]}}")
        courses = week.get("courses")
        if not isinstance(courses, list):
            return (False, "schedule week-grid requires a courses list "
                           "({week:{courses:[...]}})")
        for course in courses:
            if not isinstance(course, dict):
                return (False, "schedule week-grid course entry must be an object")
            for key in ("day", "slot", "name", "flags"):
                if key not in course:
                    return (False, f"schedule week-grid course entry missing key {key!r}")

    size = _data_bytes(data if isinstance(data, dict) else {})
    if size < 0:
        return (False, "data is not JSON-serialisable")
    if size > MAX_DATA_BYTES:
        return (False, f"data size {size}B exceeds MAX_DATA_BYTES {MAX_DATA_BYTES}B")

    return (True, "ok")