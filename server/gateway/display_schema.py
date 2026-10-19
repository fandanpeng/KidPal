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
# start_organizing（契约 §1.4, 2026-10-19 CDQ 条目 LANDED）同型: 三式收尾统一由
# front 机械下发单源承载（q2 = ⓒ 裁定）, 引擎 display 流来源一律拒转.
GATEWAY_DECISION_CONTROL_ACTIONS = frozenset({"end_session", "start_organizing"})
# 可由 display 流透传的 control action（决定类与可透传类不相交——start_organizing
# 只走 front 自推通路 self_push=True 放行, relay 路径恒拒）.
FORWARDABLE_CONTROL_ACTIONS = frozenset({"stop_speaking", "start_recording"})


def _data_bytes(data) -> int:
    try:
        return len(json.dumps(data, ensure_ascii=False).encode("utf-8"))
    except (TypeError, ValueError):
        return -1  # not JSON-serialisable -> caller rejects


def validate_display(payload, *, self_push=False) -> tuple[bool, str]:
    """Validate a ```display JSON payload for forwardability to the board.

    Returns (ok: bool, reason: str). (True, "ok") means the payload passed the
    mechanical schema check and is safe to relay; anything else must be downgraded.

    self_push（keyword-only, 缺省 False; 既有全部调用点零偏移——结构性风险 1 自推通路
    分叉, 261019-p3-record-dialogue-guide）:
      False（relay/引擎侧路径, 既有语义一字不动）: 网关决定类（GATEWAY_DECISION_
        CONTROL_ACTIONS）一律拒绝——伪造/注入的网关决定不得经 display 流透传。
      True（front 机械自推专用通路 _push_display_payload）: 网关决定类中仅
        start_organizing 放行（front 机械下发单源, q2 = ⓒ）; end_session 即便自推仍
        拒绝（会话结束决定不因通路放行）。其余 floor 照跑。
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
            # 契约 §1.4: 网关决定类不是可被 display 流盲转发的意图. start_organizing
            # 唯一豁免口 = front 自推通路（self_push=True）——机械下发单源放行;
            # end_session 即便自推仍拒（会话结束决定不因通路放行）.
            if self_push and action == "start_organizing":
                pass  # front 自推放行（落位到下方 forwardable 分支外的放行通道）
            else:
                return (False, f"control/{action} is a gateway intent decision, not forwardable")
        elif action not in FORWARDABLE_CONTROL_ACTIONS:
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

    # Q8 story 轻量形状 floor（契约 §1.5 step 3 / 2026-09-24 契约门裁定；schedule floor
    # 先例对称——同为 shape floor，不作逐字段深度类型校验）。白名单零扩：kidpal_story_delete
    # 是 §四端云消息、非 display view，不进本表。
    if view == "story_list":
        total = data.get("total")
        if total is not None and (not isinstance(total, int) or isinstance(total, bool)
                                  or total < 0):
            # total 在场须为非负整数（对端 kidpal_story_list_parse 非整数 -> -1 整单降级）。
            return (False, "story_list total must be a non-negative integer")
        # data.restore 可选布尔（additive；契约 §1.3③ D1 / §1.5-3 floor 联动，2026-10-12
        # 契约门）——true = 恢复性载荷（WS 建连附带重推，只填缓存不接管前台）。缺省（absent）
        # 或显式 false 设备行为完全不变；在场须为 JSON 布尔（非布尔字面 -> 拒绝并降级，
        # 畸形 restore 帧不入板盲渲染）。
        restore = data.get("restore")
        if restore is not None and not isinstance(restore, bool):
            return (False, "story_list restore must be a boolean when present")
        items = data.get("items")
        if not isinstance(items, list):
            return (False, "story_list items must be an array")
        for item in items:
            if not isinstance(item, dict):
                return (False, "story_list item must be an object")
            for key in ("id", "title", "date"):
                if key not in item:
                    return (False, f"story_list item missing key {key!r}")
    elif view == "story_text":
        seq = data.get("seq")
        if seq is not None and (not isinstance(seq, int) or isinstance(seq, bool)):
            # seq 可选（additive）；在场须为整数（入册流水号 N 是整数，契约 §1.3④/Q8）。
            return (False, "story_text seq must be an integer when present")

    size = _data_bytes(data if isinstance(data, dict) else {})
    if size < 0:
        return (False, "data is not JSON-serialisable")
    if size > MAX_DATA_BYTES:
        return (False, f"data size {size}B exceeds MAX_DATA_BYTES {MAX_DATA_BYTES}B")

    return (True, "ok")