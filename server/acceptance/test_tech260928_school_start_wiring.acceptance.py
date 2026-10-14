"""Frozen acceptance tests — TaskPackage 260928-tech-260928-school-start-upcoming-empty
(subsystem: server/gateway/front; sourceType: crq-tech-entry; riskTier low; gateProfile test-quality).

Story 语义权威 = docs/crq/tech/TECH-260928-school-start-upcoming-empty.md：周中（在校日）
P1 首页「接下来」恒空 —— front_app.py 建连推（:685 附近 _push_schedule_connect）与
零点/即时推（:708 附近 _push_schedule_refresh_to_legs）两处 home_upcoming_payload 调用
漏传 school_start，与「帮我显示课表」意图展开路径（:642 附近，唯一正确参照）不同参；
school_start 只在 ECS 配置落位（运维半，本包范围外）后由 _schedule_school_start() 读入。

Scope of record（taskpackage acceptance TECH-A1..A3）：
  TECH-A1 建连推同参齿 —— _push_schedule_connect 内 home_upcoming_payload 调用传
          school_start=_schedule_school_start()（与意图展开路径同参对齐）；
          含边界齿：配置缺失 -> None 优雅降级不炸中继（ECS 现状即此形态）。
  TECH-A2 refresh 推同参齿 —— _push_schedule_refresh_to_legs 内 home_upcoming_payload
          调用同参对齐。
  TECH-A3 grid restore 零偏移守卫齿 —— front_app.py 两处 grid_restore_payload 调用
          （connect piggyback :694 附近 + refresh piggyback :712 附近）保持
          now_hhmm/school_start 双 None（ADR-005 恢复性载荷 = 静态周网格，防 builder
          「顺手修复」破坏恢复语义）。

===========================================================================
技术路线 = **行为级**（M4K1 先例：fastapi stub + front_app.py 离线装载，g-relay-fix
先例），非源锚代理。理由：三条推送缝（_push_schedule_connect /
_push_schedule_refresh_to_legs）是具名 async helper、可离线直调；producer
（home_upcoming_payload / grid_restore_payload）与开关（grid_restore_enabled）是模块级
属性、可在调用点捕获。故装载 front_app 后把捕获 spy 织到 gateway.schedule /
gateway.schedule_push 模块属性上、把中继缝 _relay_schedule_payload 换成录制器，
真跑两条推送路径，断言**设备可见的载荷内容**（合并「上学」项在场/缺席）+ 传参本身
——比 grep 源码强一档（源码写了 school_start 但接错变量/传错值也逃不掉）。
M4K1 的 _load_front_app 模式逐件复用（stub / spec_from_file_location / 独立模块名）。

Coverage map (acceptanceRef -> test):
  TECH-A1 -> test_a1_connect_push_passes_school_start
          -> test_a1_connect_push_missing_config_degrades_to_none_school_start（边界）
  TECH-A2 -> test_a2_refresh_push_passes_school_start
  TECH-A3 -> test_a3_grid_restore_stays_double_none

Red baseline 判读（acceptance-author 2026-09-28 实跑，命令逐字见 taskpackage
redBaselineCommand / 交付报告）：
  - 未修补树：TECH-A1 主齿 RED（school_start 缺省 None -> 合并「上学」项缺席 =
    bug 症状本身）、TECH-A2 RED（同因）；TECH-A1 边界齿绿（配置缺失降级路径现状即
    正确，must-stay-green）；TECH-A3 绿（守卫齿正样本——现状双 None，must-stay-green）。
  - 既有面（M3K1 producer / M3K3 / M4K1 / units）零新增红。
  - 收集零错误（冻结即冒烟，DEV-GOVERNANCE §3；C 类验收文件实编 = 本文件被 pytest
    收集且 0 error，DEV-GOVERNANCE §4）。

跨端 wire 形状锚（brief ② 必读面——对端实际接受什么）：firmware/main/display/
home_upcoming.c kidpal_home_upcoming_apply 消费 {"view":"home","data":{"upcoming":
{day_key,items[{time,name}],preview}}}；school_start 只影响 server 侧 upcoming 分组
内容（周中合并「上学」项恢复出现），载荷结构与固件解析逻辑零改动（taskpackage
crossEnd 已申报，wire-shape-change=false）。本文件不钉 wire 帧形状（M3K1-A3 已钉
producer 形状），只钉分组内容随 school_start 接线恢复。

契约条目 ↔ 齿 的强度申报（acceptance-evidence T2 体例）：
  story 代码半（TECH-260928 建议改动·接线补传）→ A1/A2 齿（front 推送缝真跑 +
  spy 捕获传参 + 载荷内容断言）→ 行为级
  story 零偏移面（grid restore 双 None = ADR-005 恢复语义）→ A3 齿（两调用点真跑
  + spy 捕获 + data.restore=True 正样本）→ 行为级
  弱于契约处（显式缺口，逐条）：
    ① 真 WS 建连/真零点调度 offline 不可验——本齿直调具名推送缝并断言其行为；
      「ws_gateway 建连点 / run_schedule_poll_once 轮询恰调一次对应缝」的行为等价
      假设显式化、本件未验（M3K1-A5 / M3K3-A2 live 面已按各自挂账复证，本轮不新增
      live 齿——bug 修复不改触发拓扑，只改缝内传参）。
    ② _schedule_school_start() 的文件解析正则属 front 内部实现，本件不钉（只经
      SCHEDULE_CFG 指向的临时配置文件端到端喂值）；配置格式锚点已由运维半承载。
    ③ spy 断言「与意图展开路径同参对齐」钉到**等值**（school_start == 配置文件值），
      不钉调用形态（位置参 vs 关键字参均可）——修复实现自由。

Run protocol:
  PYTHONIOENCODING=utf-8 python -m pytest -q --import-mode=importlib -m "not live" \
      server/acceptance/test_tech260928_school_start_wiring.acceptance.py
  (standalone: python 本文件)
"""

from __future__ import annotations

import asyncio
import contextlib
import copy
import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import pytest  # noqa: F401  (markers only; standalone unittest runner also works)
    _HAS_PYTEST = True
except ImportError:  # pragma: no cover
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"

# ---- 确定性时间/配置锚（2026-09-28 = 周一，B1 weekday 1；实测见冻结记录） -------- #
TODAY_ISO = "2026-09-28"     # 周一（在校日，合并「上学」项的判据日）
TOMORROW_ISO = "2026-09-29"  # 周二
NOW_HHMM = "07:30"           # 上学前（08:00 前野 -> 合并项 start-only 规则在场）
SCHOOL_START = "08:00"
CFG_TEXT = '# openclaw-schedule.config.yaml（测试夹具）\nschool_start: "08:00"\n'

# 最小 B1 模型：仅周一学校课程表行（合并项判据面），无课外课/临时安排 —— 让
# 「接下来」组里唯一可能的项就是合并「上学」项，偏移一目了然。
MODEL = {
    "child": "测试娃",
    "timezone": "+08:00",
    "weekly": [],
    "onetime": [],
    "school": [{"weekday": 1, "periods": ["语文", "数学", "英语", "体育", "音乐", "美术"]}],
}

_MERGED_SCHOOL_ITEM = {"time": "08:00", "name": "上学"}


# --------------------------------------------------------------------------- #
# Module loading（M4K1 先例：fastapi stub + front_app.py 离线装载）              #
# --------------------------------------------------------------------------- #

_front_module_cache = None


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（g-relay-fix / M4K1 验收件同款先例）。"""
    try:
        import fastapi  # noqa: F401
        return  # real fastapi present — even better, use it
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    if "fastapi" not in sys.modules:
        stub = _PermissiveModule("fastapi")
        stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
        stub.HTTPException = type("HTTPException", (Exception,), {})
        sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)."""
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location(
            "kidpal_front_app_tech260928", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线可装载"
                      "（import 期零网络 I/O；fastapi 缺席走本地 stub，M4K1 先例）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("TECH-260928 RED: " + err)
    return mod


def _gateway_modules():
    """front 组装的两个冻结 producer 模块（与其 front 内引用同对象，patch 即生效）。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.schedule as sched
    import gateway.schedule_push as sched_push
    return sched, sched_push


# --------------------------------------------------------------------------- #
# Wiring fixtures（织 spy / 录制器，全部 try/finally 还原 —— 不污染同进程其它测试） #
# --------------------------------------------------------------------------- #

@contextlib.contextmanager
def _front_with_school_cfg(cfg_text: "str | None"):
    """装载 front 并钉死确定性环境：SCHEDULE_CFG -> 临时配置文件（cfg_text None =
    配置缺失形态）、模型/今天明天/当前时刻全部注入。"""
    front = _front()
    with tempfile.TemporaryDirectory(prefix="tech260928_") as tmp:
        cfg = Path(tmp) / "openclaw-schedule.config.yaml"
        if cfg_text is not None:
            cfg.write_text(cfg_text, encoding="utf-8")
        saved = (front.SCHEDULE_CFG, front._load_schedule_model,
                 front._today_tomorrow_iso, front._local_now_hhmm)
        front.SCHEDULE_CFG = str(cfg)
        front._load_schedule_model = lambda: copy.deepcopy(MODEL)
        front._today_tomorrow_iso = lambda: (TODAY_ISO, TOMORROW_ISO)
        front._local_now_hhmm = lambda: NOW_HHMM
        try:
            yield front
        finally:
            (front.SCHEDULE_CFG, front._load_schedule_model,
             front._today_tomorrow_iso, front._local_now_hhmm) = saved


@contextlib.contextmanager
def _spy_home_payload():
    """捕获 front 对 gateway.schedule.home_upcoming_payload 的全部调用（真函数照跑，
    载荷照产 —— 只旁路记账，不改变被测路径行为）。"""
    sched, _ = _gateway_modules()
    orig = sched.home_upcoming_payload
    calls = []

    def spy(model, today_iso, now_hhmm, tomorrow_iso, school_start=None, **kw):
        payload = orig(model, today_iso, now_hhmm, tomorrow_iso, school_start, **kw)
        calls.append({"model": model, "today_iso": today_iso, "now_hhmm": now_hhmm,
                      "tomorrow_iso": tomorrow_iso, "school_start": school_start,
                      "payload": payload})
        return payload

    sched.home_upcoming_payload = spy
    try:
        yield calls
    finally:
        sched.home_upcoming_payload = orig


@contextlib.contextmanager
def _spy_grid_payload():
    """捕获 front 对 gateway.schedule_push.grid_restore_payload 的全部调用（真函数
    照跑；捕获 now_hhmm/school_start 是否被「顺手修复」塞了值）。"""
    _, sched_push = _gateway_modules()
    orig = sched_push.grid_restore_payload
    calls = []

    def spy(model, week_date_iso, now_hhmm=None, school_start=None, **kw):
        payload = orig(model, week_date_iso, now_hhmm, school_start, **kw)
        calls.append({"model": model, "week_date_iso": week_date_iso,
                      "now_hhmm": now_hhmm, "school_start": school_start,
                      "payload": payload})
        return payload

    sched_push.grid_restore_payload = spy
    try:
        yield calls
    finally:
        sched_push.grid_restore_payload = orig


@contextlib.contextmanager
def _grid_restore_enabled(enabled: bool):
    """钉 grid restore 开关（默认读 env —— 显式钉死，测试不依赖机器环境）。"""
    _, sched_push = _gateway_modules()
    orig = sched_push.grid_restore_enabled
    sched_push.grid_restore_enabled = lambda raw=None: enabled
    try:
        yield
    finally:
        sched_push.grid_restore_enabled = orig


@contextlib.contextmanager
def _relay_recorder(front):
    """把 front 的下行中继缝换成录制器（设备可见面的观测点）。"""
    sent = []

    async def record(payload, websocket):
        sent.append((websocket, payload))

    old = front._relay_schedule_payload
    front._relay_schedule_payload = record
    try:
        yield sent
    finally:
        front._relay_schedule_payload = old


@contextlib.contextmanager
def _alert_legs(front, legs):
    """替换/还原建连腿表（_push_schedule_refresh_to_legs 的投递对象）。"""
    old = front._alert_legs
    front._alert_legs = list(legs)
    try:
        yield
    finally:
        front._alert_legs = old


def _upcoming_of(home_payload: dict) -> dict:
    return ((home_payload or {}).get("data") or {}).get("upcoming") or {}


def _grid_payloads(sent) -> list:
    return [p for _ws, p in sent if (p or {}).get("view") == "schedule"
            and isinstance((p.get("data") or {}), dict)
            and p["data"].get("restore") is True]


# =========================================================================== #
# TECH-A1 — 建连推同参齿（_push_schedule_connect）                                #
# =========================================================================== #

class TechA1ConnectPushWiring(unittest.TestCase):
    """TECH-A1: 建连推 home_upcoming_payload 必须传 school_start（与意图展开路径
    :642 同参对齐）；设备可见效果 = 周中在校日「接下来」组合并「上学」项恢复在场。"""

    maxDiff = None

    def test_a1_connect_push_passes_school_start(self):
        """coverage: TECH-A1 — 建连推真跑：传参 school_start == 配置文件值，且中继给
        建连 socket 的 home 载荷 items 含合并「上学」项（bug 症状直接断言）；
        同参对齐钉**等值**不钉调用形态（位置/关键字均可）。"""
        front = _front()
        connect_ws = object()  # 建连 socket 哨兵（中继已录制，不需真 ws）
        with _front_with_school_cfg(CFG_TEXT) as front:
            with _relay_recorder(front) as sent, _spy_home_payload() as calls:
                asyncio.run(front._push_schedule_connect(connect_ws))
        self.assertEqual(
            len(calls), 1,
            f"TECH-A1 RED: _push_schedule_connect 调 home_upcoming_payload {len(calls)} 次"
            "（应恰 1 次）—— 建连推内容偏移。")
        call = calls[0]
        self.assertEqual(
            call["school_start"], SCHOOL_START,
            f"TECH-A1 RED: 建连推 school_start = {call['school_start']!r} != "
            f"{SCHOOL_START!r} —— _push_schedule_connect 漏传 school_start（与意图展开"
            "路径 :642 不同参），周中在校日 upcoming 恒空（TECH-260928 代码半）。")
        self.assertEqual(call["today_iso"], TODAY_ISO, "TECH-A1 RED: today_iso 偏移。")
        self.assertEqual(call["now_hhmm"], NOW_HHMM, "TECH-A1 RED: now_hhmm 偏移。")
        self.assertEqual(call["tomorrow_iso"], TOMORROW_ISO, "TECH-A1 RED: tomorrow_iso 偏移。")
        self.assertEqual(call["model"], MODEL, "TECH-A1 RED: model 非 _load_schedule_model 产物。")
        # 设备可见面：恰一帧、发给建连 socket、items 含合并「上学」项。
        self.assertEqual(
            len(sent), 1, f"TECH-A1 RED: 建连推下发 {len(sent)} 帧（应恰 1 帧 home）。")
        relay_ws, payload = sent[0]
        self.assertIs(relay_ws, connect_ws, "TECH-A1 RED: home 帧未发到建连 socket。")
        self.assertEqual(payload.get("view"), "home", "TECH-A1 RED: 建连推 view 偏移。")
        upcoming = _upcoming_of(payload)
        self.assertEqual(
            upcoming.get("items"), [_MERGED_SCHOOL_ITEM],
            f"TECH-A1 RED: 周一 {NOW_HHMM} 建连推 items = {upcoming.get('items')!r} != "
            "[{'time':'08:00','name':'上学'}] —— school_start 未接线则合并项缺席，"
            "设备 P1「接下来」恒空（TECH-260928 真机症状）。")

    def test_a1_connect_push_missing_config_degrades_to_none_school_start(self):
        """coverage: TECH-A1（边界齿）— 配置文件缺失（ECS 运维半未落位的现状形态）->
        school_start 传 None、不炸中继、载荷仍合法下发（优雅降级语义，与
        _load_schedule_model 同口径）。配置落位前后行为差异 = 有无合并项。"""
        front = _front()
        connect_ws = object()
        with _front_with_school_cfg(None) as front:
            self.assertIsNone(
                front._schedule_school_start(),
                "TECH-A1 前置 RED: 配置缺失时 _schedule_school_start() 应返 None。")
            with _relay_recorder(front) as sent, _spy_home_payload() as calls:
                asyncio.run(front._push_schedule_connect(connect_ws))  # 不得抛
        self.assertEqual(len(calls), 1, "TECH-A1 RED: 缺配置时建连推调用次数偏移。")
        self.assertIsNone(
            calls[0]["school_start"],
            "TECH-A1 RED: 配置缺失时 school_start 未传 None —— 降级口径偏移。")
        self.assertEqual(len(sent), 1, "TECH-A1 RED: 缺配置时建连推未下发（不得炸中继）。")
        self.assertEqual(
            _upcoming_of(sent[0][1]).get("items"), [],
            "TECH-A1 RED: 缺配置时 items 应为空（无配置 -> 无合并项），出现内容 = "
            "配置读取面偏移。")


# =========================================================================== #
# TECH-A2 — refresh 推同参齿（_push_schedule_refresh_to_legs）                    #
# =========================================================================== #

class TechA2RefreshPushWiring(unittest.TestCase):
    """TECH-A2: 零点/即时推（触发②③ 共用缝）home_upcoming_payload 必须传
    school_start；设备可见效果同 A1。"""

    maxDiff = None

    def test_a2_refresh_push_passes_school_start(self):
        """coverage: TECH-A2 — 注入一条在线腿 + 关 grid piggyback，真跑
        _push_schedule_refresh_to_legs：传参 school_start == 配置文件值，载荷发到该腿
        且含合并「上学」项；返回投递腿数 = 1。"""
        front = _front()
        leg_ws = object()
        with _front_with_school_cfg(CFG_TEXT) as front:
            with _alert_legs(front, [(leg_ws, object())]):
                with _relay_recorder(front) as sent, _spy_home_payload() as calls:
                    with _grid_restore_enabled(False):
                        sent_count = asyncio.run(
                            front._push_schedule_refresh_to_legs(copy.deepcopy(MODEL)))
        self.assertEqual(
            sent_count, 1,
            f"TECH-A2 RED: refresh 推返回投递腿数 {sent_count} != 1 —— 推送缝行为偏移。")
        self.assertEqual(
            len(calls), 1,
            f"TECH-A2 RED: _push_schedule_refresh_to_legs 调 home_upcoming_payload "
            f"{len(calls)} 次（应恰 1 次）。")
        call = calls[0]
        self.assertEqual(
            call["school_start"], SCHOOL_START,
            f"TECH-A2 RED: refresh 推 school_start = {call['school_start']!r} != "
            f"{SCHOOL_START!r} —— _push_schedule_refresh_to_legs 漏传 school_start"
            "（与意图展开路径 :642 不同参），零点/即时推产空 upcoming（TECH-260928）。")
        self.assertEqual(call["today_iso"], TODAY_ISO, "TECH-A2 RED: today_iso 偏移。")
        self.assertEqual(call["now_hhmm"], NOW_HHMM, "TECH-A2 RED: now_hhmm 偏移。")
        self.assertEqual(call["tomorrow_iso"], TOMORROW_ISO, "TECH-A2 RED: tomorrow_iso 偏移。")
        self.assertEqual(
            len(sent), 1, f"TECH-A2 RED: 该腿收到 {len(sent)} 帧（grid 关时应恰 1 帧 home）。")
        relay_ws, payload = sent[0]
        self.assertIs(relay_ws, leg_ws, "TECH-A2 RED: home 帧未发到在线腿。")
        self.assertEqual(
            _upcoming_of(payload).get("items"), [_MERGED_SCHOOL_ITEM],
            f"TECH-A2 RED: refresh 推 items = {_upcoming_of(payload).get('items')!r} != "
            "[{'time':'08:00','name':'上学'}] —— school_start 未接线，零点/即时推恒空。")


# =========================================================================== #
# TECH-A3 — grid restore 零偏移守卫齿（双 None = ADR-005 恢复语义）               #
# =========================================================================== #

class TechA3GridRestoreZeroOffsetGuard(unittest.TestCase):
    """TECH-A3: front 两处 grid_restore_payload 调用保持 now_hhmm/school_start 双 None
    （恢复性载荷 = 静态周网格，不含实时态）。守卫齿 = 正样本：现状绿；
    builder「顺手修复」把 school_start 也传进 grid 路径 -> 本齿转红。"""

    maxDiff = None

    def test_a3_grid_restore_stays_double_none(self):
        """coverage: TECH-A3 — connect piggyback 与 refresh piggyback 两条路径真跑：
        grid_restore_payload 恰以 (model, today_iso) 两参调用（now/school 缺省 None），
        且中继载荷 data.restore == True（恢复语义在场）。"""
        front = _front()
        with _front_with_school_cfg(CFG_TEXT) as front:
            # 路径 1：建连 piggyback（home 先 grid 后）。
            with _relay_recorder(front) as sent1, _spy_grid_payload() as grid_calls1:
                with _grid_restore_enabled(True):
                    asyncio.run(front._push_schedule_connect(object()))
            # 路径 2：refresh piggyback（每腿 home 之后追加 grid）。
            with _alert_legs(front, [(object(), object())]):
                with _relay_recorder(front) as sent2, _spy_grid_payload() as grid_calls2:
                    with _grid_restore_enabled(True):
                        asyncio.run(
                            front._push_schedule_refresh_to_legs(copy.deepcopy(MODEL)))
        for label, calls, sent in (("connect piggyback", grid_calls1, sent1),
                                   ("refresh piggyback", grid_calls2, sent2)):
            with self.subTest(path=label):
                self.assertEqual(
                    len(calls), 1,
                    f"TECH-A3 RED: {label} 调 grid_restore_payload {len(calls)} 次（应恰 1 次）。")
                call = calls[0]
                self.assertEqual(
                    call["week_date_iso"], TODAY_ISO,
                    f"TECH-A3 RED: {label} 的 week_date_iso 偏移。")
                self.assertIsNone(
                    call["now_hhmm"],
                    f"TECH-A3 RED: {label} 给 grid_restore_payload 传了 now_hhmm="
                    f"{call['now_hhmm']!r} —— 恢复性载荷必须是静态周网格（双 None，"
                    "ADR-005「只填缓存」恢复语义），不得携带实时态。")
                self.assertIsNone(
                    call["school_start"],
                    f"TECH-A3 RED: {label} 给 grid_restore_payload 传了 school_start="
                    f"{call['school_start']!r} —— grid restore 双 None 是故意语义"
                    "（ADR-005），顺手修复禁止把同参对齐外溢到恢复路径。")
                grids = _grid_payloads(sent)
                self.assertEqual(
                    len(grids), 1,
                    f"TECH-A3 RED: {label} 下发 {len(grids)} 帧 grid restore（应恰 1 帧）")
                # 双 None 下网格为纯静态：模型周一学校行 6 节 -> 上午 4 节 slot0 +
                # 下午 2 节 slot1，flags 全 0（无星标 bit1 —— now=None 不做星标呼应）。
                expected_courses = (
                    [{"day": 0, "slot": 0, "name": p, "flags": 0}
                     for p in ("语文", "数学", "英语", "体育")]
                    + [{"day": 0, "slot": 1, "name": p, "flags": 0}
                       for p in ("音乐", "美术")])
                self.assertEqual(
                    grids[0]["data"].get("week", {}).get("courses"), expected_courses,
                    f"TECH-A3 RED: {label} 的 grid 载荷 courses 偏移（静态周网格 = 学校行"
                    "逐节 4+2、flags 全 0 —— 双 None 无星标无合并项，ADR-005 恢复语义）。")


# --------------------------------------------------------------------------- #
if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    for cls in (TechA1ConnectPushWiring, TechA2RefreshPushWiring,
                TechA3GridRestoreZeroOffsetGuard):
        suite.addTests(loader.loadTestsFromTestCase(cls))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
