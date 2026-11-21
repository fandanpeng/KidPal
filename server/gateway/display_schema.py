"""Mechanical ```display schema validation (V27 / contract §1.3 / §1.4 / §1.5).

Frozen acceptance GWA-A5. Pure-python, no fastapi at import time.

Frozen offline contract:
    validate_display(payload) -> (ok: bool, reason: str)
        VALID_VIEWS == {char, schedule, story_list, story_text, text, home, control,
                        points_home, points_ledger}   （2026-11-04 additive 7→9）
        MAX_DATA_BYTES  (data size cap)
        reject: non-dict /
                view not in whitelist /
                oversize data /
                control w/ unknown action /
                a raw control-end_session arriving from the LLM display stream
                (forged -> not forwardable; contract §1.4 end_session is a gateway
                 intent decision, NEVER blindly forwarded to the board render path) /
                control/add_points|redeem_points from ANY path (front 截获执行，控制帧
                 不发任何一端；契约 §1.4) /
                完整 points_home|points_ledger 帧的引擎侧来源（积分来源门禁，契约 §1.5；
                 仅 front 自推 self_push=True 可下发）

The gateway runs this on every ```display block split out of the LLM reply (契约 §1.5
step 3): a rejected payload is downgraded to a friendly text view (or dropped), never
sent to the board's blind-render path.
"""

from __future__ import annotations

import json

# 契约 §1.3 view 白名单. 2026-11-04（261103-star-points-story-cover）additive 7→9：
# 尾部追加 points_home / points_ledger，既有 7 个 view 零偏移（FROZEN K0PTS-A3）。
VALID_VIEWS = {
    "char",
    "schedule",
    "story_list",
    "story_text",
    "text",
    "home",
    "control",
    "points_home",
    "points_ledger",
}

# data 大小上限 (契约 §1.5 / V27). 64 KB serialised JSON is far beyond any real
# display payload; a 100k-char data blob (≈300KB UTF-8) stays well above the cap.
MAX_DATA_BYTES = 65536

# 契约 §1.4 控制 action 表. end_session 是网关切意图后的决定, 不是可被 LLM 流
# 盲转发的 display 意图 —— 伪造/注入的 control-end_session 必须在机械层拒绝.
# start_organizing（契约 §1.4, 2026-10-19 CDQ 条目 LANDED）同型: 三式收尾统一由
# front 机械下发单源承载（q2 = ⓒ 裁定）, 引擎 display 流来源一律拒转.
# add_points / redeem_points（契约 §1.4, 2026-11-04 LANDED；K0PTS-A3）：front 截获
# 薄意图交 points.py 执行, **不把控制帧发给任何一端**——relay 与 self_push 双路径恒拒
# （与 start_organizing 唯一豁免口不同；设备只消费执行后的权威积分视图）。
GATEWAY_DECISION_CONTROL_ACTIONS = frozenset(
    {"end_session", "start_organizing", "add_points", "redeem_points"})
# self_push 豁免口（契约 §1.4「截获转发类」 / §1.5-3；261117-session-end-intent-
# longretry 2026-11-19 由 {start_organizing} 扩为 {start_organizing, end_session}）：
# 截获转发类两个动作 = front 从引擎 display 流截获模型薄意图（end_session）或自行检出
# 收尾（start_organizing）后，以 front 机械自推单源恰一帧下发设备（self_push=True）；
# relay 路径对全部网关决定类仍恒拒（防伪造/注入）。add_points/redeem_points（截获执行
# 类）不在本集——relay 与 self_push 双路径恒拒。
SELF_PUSH_EXEMPT_CONTROL_ACTIONS = frozenset({"start_organizing", "end_session"})
# 可由 display 流透传的 control action（决定类与可透传类不相交——start_organizing /
# end_session 只走 front 自推通路 self_push=True 放行, relay 路径恒拒）。
FORWARDABLE_CONTROL_ACTIONS = frozenset({"stop_speaking", "start_recording"})

# 积分来源门禁（契约 §1.5 / §1.3⑦⑧；K0PTS-A3）：完整 points 帧只能由 front 依据
# points.py 当前状态自推下发；引擎侧完整 points 帧在 relay 路径拒转（降级友好 text，
# 与 story 单源先例同型）。空 data 导航薄载荷由 front 展开为完整帧。
POINTS_SOURCE_GATED_VIEWS = frozenset({"points_home", "points_ledger"})

# points_home.target 恒四键（契约 §1.3⑦ target 形状）。
POINTS_TARGET_KEYS = ("id", "name", "points", "asset")
# points_ledger.page 恒四键（契约 §1.3⑧ / §四 kidpal_points_page）。
POINTS_PAGE_KEYS = ("total", "next_cursor", "has_more", "reset")


def _is_int(value) -> bool:
    """整数判定——bool 是 int 子类（JSON 布尔不得冒充整数余额/版本号）。"""
    return isinstance(value, int) and not isinstance(value, bool)


def _points_floor(view: str, data: dict) -> "str | None":
    """积分视图完整形状 floor（契约 §1.5-3；只把关 shape、不作逐字段深度校验）。

    data == {} = 导航薄载荷（front 展开为完整帧），放行。
    完整形状须含 revision / balance（整数）、target 四键；points_home 另认 recent
    （list）、points_ledger 另认 items（list）与 page 四键；restore 可选布尔。
    返回 None = 通过，否则为拒绝原因。
    """
    if not data:
        return None
    revision = data.get("revision")
    if revision is not None and not _is_int(revision):
        return "points revision must be an integer when present"
    balance = data.get("balance")
    if balance is not None and not _is_int(balance):
        return "points balance must be an integer when present"
    if view == "points_home":
        # target 恒四键（契约 §1.3⑦）；points_ledger §1.3⑧ 载荷无 target 字段。
        target = data.get("target")
        if not isinstance(target, dict):
            return "points target must be an object with id/name/points/asset"
        for key in POINTS_TARGET_KEYS:
            if key not in target:
                return f"points target missing key {key!r}"
        recent = data.get("recent")
        if recent is not None and not isinstance(recent, list):
            return "points_home recent must be an array"
    else:  # points_ledger
        items = data.get("items")
        if items is not None and not isinstance(items, list):
            return "points_ledger items must be an array"
        page = data.get("page")
        if not isinstance(page, dict):
            return "points_ledger page must be an object with total/next_cursor/has_more/reset"
        for key in POINTS_PAGE_KEYS:
            if key not in page:
                return f"points_ledger page missing key {key!r}"
    restore = data.get("restore")
    if restore is not None and not isinstance(restore, bool):
        return "points restore must be a boolean when present"
    return None


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
      True（front 机械自推专用通路 _push_display_payload）: 网关决定类中
        start_organizing 与 end_session 放行（二者同型「截获转发类」，front 机械下发
        单源；2026-11-19 反转——end_session 由「即便自推仍拒」改为放行，见契约 §1.4
        变更记录 + 261117-session-end-intent-longretry）；add_points / redeem_points
        仍拒（截获执行类，控制帧不发任何一端）。其余 floor 照跑。
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
            # 契约 §1.4: 网关决定类不是可被 display 流盲转发的意图. 截获转发类
            # （start_organizing / end_session）唯一豁免口 = front 自推通路
            # （self_push=True）——机械下发单源放行; 截获执行类（add_points /
            # redeem_points）即便 self_push 仍拒（控制帧不发任何一端，只发执行后权威
            # 积分视图）。2026-11-19 end_session 由「即便自推仍拒」反转为放行
            # （截获转发类，front 截获模型薄意图后机械自推单源恰一帧；契约 §1.4/§1.5）。
            if self_push and action in SELF_PUSH_EXEMPT_CONTROL_ACTIONS:
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
        # `data == {}` = 显式「打开故事」导航薄载荷（契约 §1.3③/§1.5 2026-11-15 增认；
        # 对齐 points 空 data 先例）：front 截获展开为完整非 restore 帧（进封皮）——此处
        # 放行；其余畸形（total 非整数 / 缺 items / restore 非布尔 / data 非对象）仍拒。
        if data == {}:
            return (True, "ok")
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

    # 积分来源门禁 + 完整形状 floor（契约 §1.5-3 / §1.3⑦⑧；2026-11-04 LANDED，K0PTS-A3）：
    # `{"view":"points_home","data":{}}` / `{"view":"points_ledger","data":{}}` 空 data
    # 导航薄载荷由 front 展开为完整帧；完整 points 帧仅 front 自推可下发——引擎侧完整帧
    # 在 relay 路径拒转（与 story 单源先例同型；front 侧再降级为友好 text，绝不透传）。
    if view in POINTS_SOURCE_GATED_VIEWS:
        problem = _points_floor(view, data)
        if problem is not None:
            return (False, problem)
        if data and not self_push:
            return (False, f"complete {view} frame from engine source is not forwardable "
                           "(points payloads are front self-push only, contract §1.5)")

    size = _data_bytes(data if isinstance(data, dict) else {})
    if size < 0:
        return (False, "data is not JSON-serialisable")
    if size > MAX_DATA_BYTES:
        return (False, f"data size {size}B exceeds MAX_DATA_BYTES {MAX_DATA_BYTES}B")

    return (True, "ok")