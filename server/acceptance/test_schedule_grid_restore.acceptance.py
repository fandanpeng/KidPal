"""Frozen acceptance tests — TaskPackage 260923-m3-k7-schedule-grid-restore
(subsystem: server; workstream m3-schedule; rigor full; riskTier medium;
sourceType = arch-review, upstream frozen 2026-09-23 commit 565cb2d).

Scope of record (taskpackage acceptance K7-A1..A4 + K7-A7 server 半; K7-A5/A6/A8
归 firmware/main/test/schedule_grid_restore.acceptance.c + 真机窗):
K7-A1  server 建连帧序——home upcoming 先、grid restore 后（fake websocket 注入，
       K1 三触发模拟齿先例 / 归档 RMD 假腿先例合体）；schedule.md 缺失/坏不阻断
       建连（try/except 语义保持）。
K7-A2  薄载荷展开恒不置 restore（机械齿，守契约 §1.3② 生产纪律）——
       expand_schedule_intent_payload 输出断言（absent/false 同判 = 输出不得携带
       truthy restore，R7 口径）；语音接管路径零偏移。
K7-A3  ECS 开关默认关——默认配置下 grid restore relay 不发出（C1 挂账机判齿）；
       开关开 = relay 发出（正例齿）。
K7-A4  alert 起轮不回归——_register_alert_legs 注册时序零偏移（pytest 时序断言 +
       行为级投递闭环零偏移）。
K7-A7  每日 / marker 触发附带——piggyback 路径 grid 同型附带（同一 due 判定点、
       不新增第二套进程内日账）；录入 marker 附带 = SHOULD 级（R8 分层口径）。

上游锚：docs/ADR.md ADR-005（restore 语义 + 部署时序 + 触发必要性分层 R8）+
docs/contracts/interface-contract.md §1.3②（data.restore additive L85 / 生产纪律
L86 / 板子行为句 L87，2026-09-23 契约门批准）+ arch §5.2 场景①（三触发附带推）。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python、导入期零网络 I/O、
离线可装载。建议家 = schedule_push.py 纯决策面 + front_app.py 接线；先例 =
M3K1 三触发决策面 + 归档 RMD 假腿行为齿）:

  server/gateway/schedule_push.py — 增量三件（既有 connect_push_payload /
  daily_push_due / refresh_requested / consume_refresh_marker 语义零偏移）:

    GRID_RESTORE_ENV = "KIDPAL_GRID_RESTORE_PUSH"
      ECS 推送开关 env 名常量（跨组件配置协议，同 MARKER_FILENAME 钉法）。
      C1 closeout：默认关交付；开闸 = 真机验证后 human/integrator 人工动作
      （ADR-005 部署时序）。

    grid_restore_enabled(raw: str | None = None) -> bool
      开关决策面（纯函数，default None = 读 os.environ.get(GRID_RESTORE_ENV)）。
      strip().lower() ∈ {"1","true","on","yes"} -> True；unset/""/其它任何值
      （含 "0"/"false"）-> False（默认关偏置：误配永不误开闸）。

    grid_restore_payload(model, week_date_iso, now_hhmm=None,
                         school_start=None) -> dict
      grid restore 变体 producer（单源委托 schedule.week_grid_payload 同参，
      绝不另起第二套网格计算）：== week_grid_payload(同参) 且
      data["restore"] is True、data 键集恰 {"week","restore"}（契约 §1.3②
      data.restore additive 布尔，data 内）。

  server/gateway/front/front_app.py — 增量接线（既有 home upcoming 建连推送 /
  run_schedule_poll_once 日账 / _register_alert_legs 时序零偏移）:

    async def _push_schedule_connect(websocket) -> None
      建连推送缝（ws_gateway 建连点调用的 async helper；把既有 L762-773 内联
      home 推送收编 + 追加 grid restore relay）：
      - _load_schedule_model() None（缺失/坏 schedule.md）-> 静默 return
        （零帧、零异常——建连不阻断，try/except 语义保持）；
      - 推 1 = home upcoming（home_upcoming_payload 同参，与现状零偏移）；
      - 推 2 = grid_restore_enabled() 为真时追加 grid restore
        （grid_restore_payload(model, today_iso, now, school_start)）；
      - 两推各自独立 try/except（单推失败记日志不连坐另一推、不外抛）。

    ws_gateway：_register_alert_legs -> _ensure_alert_loop -> 建连推送调用
      （时序零偏移；建连推送调用点保留既有 try/except 不阻断建连——
      schedule_connect_push_err 日志锚在世）。

    run_schedule_poll_once：触发②③ piggyback 同型追加 grid（同一 due 判定点、
      同一 model 快照；开关门控；**不新增第二套进程内日账**——既有
      _daily_push_local_date 恰一次语义承载 grid 与 home 同批）。

跨端 wire 事实（本文件断言按对端实际接受什么钉，260907-g-relay-fix 教训）：
grid restore 帧 = 既有 kidpal/display tools/call 单帧（{"type":"mcp","payload":…}），
arguments = {"view":"schedule","data":"<json.dumps(data) 字符串>"}；data 内
restore 是 JSON 布尔 true。设备解析面 = 冻结 kidpal_schedule_grid_parse（K2）——
数据对象级未知键走 kspj_skip_value（true/false literal 已在世），firmware 侧
容忍锚钉在 schedule_grid_restore.acceptance.c TEST 4（weak 哨兵）。

契约条目 ↔ 齿 的强度申报（acceptance-evidence T2 体例）：
  契约 §1.3② 生产纪律「展开恒不置」→ A2 齿 → 行为级（expand 真输出断言）
  ADR-005/plan 部署纪律「开关默认关」→ A3 决策面齿 + front relay 行为齿 → 行为级
  plan 建连帧序 → A1 帧序齿 → 行为级（假腿真帧序，非源锚）
  plan「alert 起轮零回归」→ A4 源锚代理（注册时序位次）+ 行为级（投递闭环真跑）
  plan「piggyback 同型 + 不新增第二套日账」→ A7 行为级（帧序 + 恰一次）+
    源锚代理（_daily_push_local_date 计数冻结基线）
  弱于契约处（显式缺口）：① 真实 TLS/WS 建连握手时 front 真发两帧 offline 钉不
    到（ws_gateway 网络面）——行为等价假设「ws_gateway 调 _push_schedule_connect ≙
    建连帧序」，由 A4 时序源锚 + K7-A8 真机锚①兜底；② 开关开闸的真机端到端
    （烧板 + human 开闸）归 K7-A8④/C1 closeout，offline 不冒充已验；③ 每日零点
    真节拍（asyncio 调度）offline 钉不到——恰一次语义经注入 now_utc 行为钉，
    真节拍归 M3K1-A5 遗留 live 面。

Coverage map (acceptanceRef -> test):
  K7-A1 -> test_a1_connect_frames_home_first_grid_restore_second
         -> test_a1_missing_schedule_md_silent_no_block
         -> test_a1_dirty_schedule_md_silent_no_block
         -> test_a1_connect_push_err_guard_kept
  K7-A2 -> test_a2_expand_output_never_carries_restore
         -> test_a2_week_grid_payload_producer_never_restore
         -> test_a2_full_restore_payload_not_intercepted
  K7-A3 -> test_a3_switch_decision_face_default_off
         -> test_a3_switch_on_positive
         -> test_a1_connect_frames…（开关开 = relay 发出正例，A1 正锚承载）
         -> test_a7_piggyback_gated_by_switch_default_off（三触发默认关全盖）
  K7-A4 -> test_a4_legs_registered_before_loop_before_connect_push
         -> test_a4_poll_loop_alert_first_schedule_second
         -> test_a4_alert_delivery_cycle_zero_offset
  K7-A7 -> test_a7_daily_trigger_piggyback_grid_same_frames
         -> test_a7_marker_trigger_piggyback_grid
         -> test_a7_no_second_daily_ledger_structural

Red baseline（现状树, acceptance-author 2026-09-23 实跑）: A1 四齿 RED
（_push_schedule_connect 全仓零命中 = frozen seam not implemented，grep 实证）；
A2 三齿现状绿（expand 既有语义本就不置 restore——must-stay-green 回归齿）；
A3 两齿 RED（grid_restore_enabled / GRID_RESTORE_ENV 不存在 = 真实缺失）；
A4 时序两齿 RED（_push_schedule_connect 缺）、投递闭环齿现状绿（must-stay）；
A7 帧序两齿 RED（relay 不存在）、结构齿 RED、默认关门齿现状绿（现状无 grid
relay = 默认关行为天然成立，开关落地后承载门控）。收集零错误（冻结即冒烟，
DEV-GOVERNANCE §3）。

Run protocol:
  python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance
  （两点文件名须 importlib 模式，G 包先例; standalone: python 本文件）

清单外阅读 + 原因（input-brief-acceptance-author.md 缺失 = manifest-missing，
降级自扫申报，planner 据此落 over-scan friction 记账）:
  - server/acceptance/archive/test_schedule_reminder.acceptance.py —— 假腿注入
    （_FakeDevice/_register_alert_legs/asyncio.run 行为齿）与 front_app 离线装载
    （fastapi stub）的仓内唯一先例，A1/A4/A7 行为齿直接对齐其形态。
  - server/gateway/mcp_relay.py / display_schema.py / reminders.py —— 对端 wire
    信封形状（tools/call arguments）与机械门对 data.restore 的容忍度实测
    （validate_display({"restore":true,…}) = 放行），跨端断言必须对照对端实际。
  - firmware/main/display/schedule_page.c —— 对端解析面 kspj_skip_value 的
    true/false literal 容忍实证（restore 键值不会被 -1），跨端 wire 断言依据。
"""

from __future__ import annotations

import asyncio
import importlib.util
import json
import os
import sys
import tempfile
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
SCHEDULE_MODULE = GATEWAY_DIR / "schedule.py"
PUSH_MODULE = GATEWAY_DIR / "schedule_push.py"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"

# --------------------------------------------------------------------------- #
# Offline module loading (missing/unimportable = RED for missing behaviour)   #
# --------------------------------------------------------------------------- #

_module_cache: dict = {}


def _load_module(path: Path, modname: str, missing_msg: str):
    cached = _module_cache.get(modname)
    if cached is not None:
        return cached
    if not path.is_file():
        return None, missing_msg
    try:
        spec = importlib.util.spec_from_file_location(modname, path)
        if spec is None or spec.loader is None:
            return None, f"{path} not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _module_cache[modname] = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error is a RED, not a crash
        return None, (f"{path} failed to import: {e!r} (frozen contract module MUST "
                      "be pure python — no network I/O at import time).")


def _schedule_callable(name: str):
    mod, err = _load_module(
        SCHEDULE_MODULE, "kidpal_gateway_schedule_k7",
        "server/gateway/schedule.py missing — B1 解析/producer 纯模块不在位。")
    if mod is None:
        return None, err
    fn = getattr(mod, name, None)
    if not callable(fn):
        return None, f"schedule.py has no module-level callable {name}()."
    return fn, None


def _push_module():
    return _load_module(
        PUSH_MODULE, "kidpal_gateway_schedule_push_k7",
        "server/gateway/schedule_push.py missing — 三触发 producer 决策面不在世。")


def _push_attr(name: str):
    mod, err = _push_module()
    if mod is None:
        return None, err
    val = getattr(mod, name, None)
    if val is None:
        return None, (f"schedule_push.py has no {name} — frozen K7 seam "
                      "(ECS switch / grid restore producer) not implemented.")
    return val, None


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（归档 RMD 验收件同款先例）。"""
    try:
        import fastapi  # noqa: F401
        return
    except ImportError:
        pass

    class _Permissive(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    if "fastapi" not in sys.modules:
        stub = _Permissive("fastapi")
        stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
        stub.HTTPException = type("HTTPException", (Exception,), {})
        sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _Permissive("fastapi.responses")


_front_cache = None


def _front_module():
    """Import server/gateway/front/front_app.py by path, offline. 缺失/装载失败 =
    RED（frozen seam 不在世），绝不 collection 崩。"""
    global _front_cache
    if _front_cache is not None:
        return _front_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k7", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_cache = (mod, None)
        return _front_cache
    except Exception as e:  # noqa: BLE001
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须"
                      "导入期零网络 I/O，验收可离线装载。")


def _front():
    mod, err = _front_module()
    if mod is None:
        raise AssertionError("K7 RED: " + err)
    return mod


def _parse(md: str):
    parse, err = _schedule_callable("parse_schedule_md")
    if parse is None:
        raise AssertionError("K7 RED(infra): " + err)
    return parse(md)


def _grid(model, week_date_iso, now_hhmm=None, school_start=None):
    fn, err = _schedule_callable("week_grid_payload")
    if fn is None:
        raise AssertionError("K7 RED(infra): " + err)
    return fn(model, week_date_iso, now_hhmm, school_start)


# --------------------------------------------------------------------------- #
# Frozen fixtures. 周内行只用「每周固定课表 + 学校课程表」（weekday 基，日期无关
# ——grid 内容对任意 week_date_iso 确定性，测试不因跑测日漂移）。
# --------------------------------------------------------------------------- #

B1_MD = """---
child: 小明
timezone: Asia/Shanghai
---

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周日 | 10:00-11:00 | 美术班 | 少年宫 | 带画笔 |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 |
|------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 |
"""

B1_DIRTY = B1_MD.replace("| 周一 | 16:30", "| 周九 | 16:30")

_T8 = dt_timezone(timedelta(hours=8))
SWITCH_ENV = "KIDPAL_GRID_RESTORE_PUSH"


def _today_iso() -> str:
    return datetime.now(tz=_T8).date().isoformat()


class _FakeDevice:
    """Device ws leg double（归档 RMD 同款）：记录 send_text 帧。"""

    def __init__(self):
        self.sent_text = []

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes = list(getattr(self, "sent_bytes", [])) + [data]


def _display_frames(sent_text) -> list:
    """设备腿收到的 kidpal/display 帧序列 [(view, data_dict), …]（保序）。

    wire 事实 = {"type":"mcp","payload":{params:{name:"kidpal/display",
    arguments:{view, data:<json str>}}}}
    （mcp_relay.display_tool_call + _display_wire_arguments 实测形状）；
    按 params.name 过滤——kidpal/alert 信封的 arguments 也有 view 键（§三 条目
    内嵌视图 JSON 字符串），不按工具名过滤会误计入帧序。"""
    out = []
    for f in sent_text:
        try:
            obj = json.loads(f) if isinstance(f, str) else f
        except (TypeError, ValueError):
            continue
        if not isinstance(obj, dict) or obj.get("type") != "mcp":
            continue
        payload = obj.get("payload") or {}
        params = payload.get("params") or {}
        if params.get("name") != "kidpal/display":
            continue
        args = params.get("arguments") or {}
        data = args.get("data")
        if isinstance(data, str):
            try:
                data = json.loads(data)
            except (TypeError, ValueError):
                data = None
        out.append((args.get("view"), data))
    return out


class _TempMd:
    """KIDPAL_SCHEDULE_MD/ALERTS_DIR 指向临时 fixture（front 模块属性运行期替换，
    import 期常量不重载；finally 还原，兄弟齿零污染）。"""

    def __init__(self, md_text: "str | None"):
        self._dir = tempfile.mkdtemp(prefix="k7_a1_")
        self.md_path = Path(self._dir) / "schedule.md"
        if md_text is not None:
            self.md_path.write_text(md_text, encoding="utf-8")

    def __enter__(self):
        front = _front()
        self._old_md = front.SCHEDULE_MD
        self._old_alerts = front.ALERTS_DIR
        front.SCHEDULE_MD = str(self.md_path)
        front.ALERTS_DIR = str(Path(self._dir) / "alerts")
        return self

    def __exit__(self, *exc):
        front = _front()
        front.SCHEDULE_MD = self._old_md
        front.ALERTS_DIR = self._old_alerts
        return False


class _Switch:
    """KIDPAL_GRID_RESTORE_PUSH env 保存/还原。"""

    def __init__(self, value: "str | None"):
        self._value = value

    def __enter__(self):
        self._old = os.environ.get(SWITCH_ENV)
        if self._value is None:
            os.environ.pop(SWITCH_ENV, None)
        else:
            os.environ[SWITCH_ENV] = self._value
        return self

    def __exit__(self, *exc):
        if self._old is None:
            os.environ.pop(SWITCH_ENV, None)
        else:
            os.environ[SWITCH_ENV] = self._old
        return False


async def _run_connect(front, device) -> None:
    """跑一次建连推送缝（frozen seam；seam 缺失 = RED 已由调用方 fail）。"""
    helper = getattr(front, "_push_schedule_connect", None)
    if not callable(helper):
        raise AssertionError(
            "K7-A1 RED: front_app.py 无 async _push_schedule_connect(websocket) "
            "——建连推送缝未落（frozen seam not implemented：home 先/grid 后、"
            "缺失/坏 schedule.md 不阻断建连的机判面不存在）。")
    await helper(device)


# --------------------------------------------------------------------------- #
# K7-A1 — 建连帧序（fake websocket 注入，K1 三触发模拟齿先例）
# --------------------------------------------------------------------------- #

class K7A1ConnectFrameOrderAcceptance(unittest.TestCase):
    """K7-A1: home upcoming 先、grid restore 后；schedule.md 缺失/坏不阻断建连。"""

    maxDiff = None

    def test_a1_connect_frames_home_first_grid_restore_second(self):
        """coverage: K7-A1 + K7-A3 正例半 — 开关开：建连恰两帧，home 先、
        schedule+restore 后；grid 帧与 week_grid_payload 同参单源同算。"""
        front = _front()
        model = _parse(B1_MD)
        today = _today_iso()
        with _TempMd(B1_MD), _Switch("1"):
            device = _FakeDevice()
            asyncio.run(_run_connect(front, device))
        frames = _display_frames(device.sent_text)
        self.assertEqual(
            [v for v, _d in frames], ["home", "schedule"],
            f"K7-A1 RED: 建连帧序 {[(v, (d or {}).get('restore')) for v, d in frames]} "
            "!= [home, schedule] —— upcoming 先、grid restore 后的帧序破（对调/缺"
            "推/多推都是真机可观测回归）。")
        home_view, home_data = frames[0]
        self.assertIsInstance(home_data, dict, "K7-A1 RED(infra): home 帧 data 非对象。")
        self.assertEqual(
            home_data.get("upcoming", {}).get("day_key"), "今天",
            "K7-A1 RED: home 帧不是 upcoming 载荷（day_key != 今天）——建连第一推"
            "内容偏移（K6 锚① home upcoming 零回归面）。")
        grid_view, grid_data = frames[1]
        self.assertIsInstance(grid_data, dict, "K7-A1 RED(infra): grid 帧 data 非对象。")
        self.assertIs(
            grid_data.get("restore"), True,
            "K7-A1 RED: grid 帧 data.restore 非 true —— 恢复性载荷未置位（设备将按"
            "现行为接管前台，restore 语义整条失效）。")
        self.assertEqual(
            set(grid_data), {"week", "restore"},
            f"K7-A1 RED: grid 帧 data 键集 {sorted(grid_data)} != {{week,restore}} "
            "—— restore 必须在 data 内（契约 §1.3② additive 布尔），不得另立信封键。")
        self.assertEqual(
            {(c["day"], c["slot"], c["name"], c["flags"])
             for c in grid_data["week"]["courses"]},
            {(c["day"], c["slot"], c["name"], c["flags"])
             for c in _grid(model, today)["data"]["week"]["courses"]},
            "K7-A1 RED: grid 帧 courses != week_grid_payload 同参产出 —— 三触发/两"
            "载荷必须单源同算（第二套网格计算 = 漂移入口）。")

    def test_a1_missing_schedule_md_silent_no_block(self):
        """coverage: K7-A1 — schedule.md 缺失：零帧、零异常（建连不被推送阻断）。"""
        front = _front()
        with _TempMd(None), _Switch("1"):
            device = _FakeDevice()
            asyncio.run(_run_connect(front, device))
        self.assertEqual(
            _display_frames(device.sent_text), [],
            "K7-A1 RED: schedule.md 缺失仍发出推送帧 —— _load_schedule_model None "
            "必须静默 return（可归因不炸建连）。")

    def test_a1_dirty_schedule_md_silent_no_block(self):
        """coverage: K7-A1 — schedule.md 坏（脏 B1 周九行）：零帧、零异常。"""
        front = _front()
        with _TempMd(B1_DIRTY), _Switch("1"):
            device = _FakeDevice()
            asyncio.run(_run_connect(front, device))
        self.assertEqual(
            _display_frames(device.sent_text), [],
            "K7-A1 RED: 坏 schedule.md（parse ValueError）仍外抛或发帧 —— try/"
            "except 语义保持破（建连被课表推送阻断 = 回潮）。")

    def test_a1_connect_push_err_guard_kept(self):
        """coverage: K7-A1（源锚代理，must-stay-green 半）— ws_gateway 建连推送调用
        点的既有 try/except 不阻断语义保持（schedule_connect_push_err 日志锚在世），
        且建连点确实调用 _push_schedule_connect。"""
        src = FRONT_APP.read_text(encoding="utf-8")
        self.assertIn("schedule_connect_push_err", src,
                      "K7-A1 RED: ws_gateway 建连推送 try/except 锚（"
                      "schedule_connect_push_err）丢失 —— 不阻断建连语义被拆。")
        self.assertIn("_push_schedule_connect(websocket)", src,
                      "K7-A1 RED: ws_gateway 未调用 _push_schedule_connect(websocket)"
                      " —— 建连推送缝无调用点（helper 是孤岛）。")


# --------------------------------------------------------------------------- #
# K7-A2 — 薄载荷展开恒不置 restore（机械齿；absent/false 同判 R7 口径）
# --------------------------------------------------------------------------- #

class K7A2ExpandNeverRestoreAcceptance(unittest.TestCase):
    """K7-A2: expand_schedule_intent_payload 输出恒无 truthy restore（语音接管
    路径零偏移——展开产物 = 主动展示、无条件接管前台，M3-K3 冻结行为）。"""

    maxDiff = None

    def test_a2_expand_output_never_carries_restore(self):
        """coverage: K7-A2 — 薄载荷展开输出 == week_grid_payload 同参且 data 键集
        恰 {"week"}；data.get("restore", False) 非 truthy（absent/false 同判——
        absent 或显式 false 都不构成「置位」，唯一红态 = truthy restore）。"""
        expand, err = _schedule_callable("expand_schedule_intent_payload")
        if expand is None:
            self.fail("K7-A2 RED(infra): " + err)
        model = _parse(B1_MD)
        today = _today_iso()
        for now_hhmm, school in (("17:00", "08:00"), (None, None)):
            expanded = expand({"view": "schedule", "data": {}},
                              model, today, now_hhmm, school)
            self.assertEqual(
                expanded, _grid(model, today, now_hhmm, school),
                "K7-A2 RED: 展开输出 != week_grid_payload 同参 —— 单源复用偏移"
                "（M3K3 冻结行为回归面）。")
            self.assertEqual(
                set(expanded["data"]), {"week"},
                f"K7-A2 RED: 展开输出 data 键集 {sorted(expanded['data'])} != "
                "{{week}} —— 意图展开（主动展示路径）不得置 data.restore（契约 "
                "§1.3② 生产纪律：仅 front 后台附带推送路径可置）。")
            self.assertFalse(
                bool(expanded["data"].get("restore", False)),
                "K7-A2 RED: 展开输出携带 truthy restore —— 语音「帮我显示课表」"
                "会变成只填缓存不接管（M3-K3 冻结行为回归 + R7 false 态倒置）。")

    def test_a2_week_grid_payload_producer_never_restore(self):
        """coverage: K7-A2 — 单源 producer 本身恒不置 restore（restore 只能由
        grid_restore_payload 变体置位，键集冻结）。"""
        model = _parse(B1_MD)
        plain = _grid(model, _today_iso())
        self.assertEqual(
            set(plain["data"]), {"week"},
            f"K7-A2 RED: week_grid_payload data 键集 {sorted(plain['data'])} != "
            "{{week}} —— 单源 producer 被顺手加 restore（置位权收口破）。")

    def test_a2_full_restore_payload_not_intercepted(self):
        """coverage: K7-A2 边界 — 带完整 data（含 restore）的 schedule 载荷不是
        意图薄载荷：expand 恒返回 None（原样走 relay 链交机械门；F-C5 幻觉路径
        accepted-as-risk 不设闸，但展开面只认恰薄载荷的冻结判据不得放宽）。"""
        expand, err = _schedule_callable("expand_schedule_intent_payload")
        if expand is None:
            self.fail("K7-A2 RED(infra): " + err)
        model = _parse(B1_MD)
        full = _grid(model, _today_iso())
        full["data"]["restore"] = True
        self.assertIsNone(
            expand(full, model, _today_iso()),
            "K7-A2 RED: 完整网格+restore 载荷被截获展开 —— 截获判据必须恰为 "
            "data=={}，放宽即薄载荷生产纪律洞。")
        self.assertIsNone(
            expand({"view": "schedule", "data": {"restore": True}},
                   model, _today_iso()),
            "K7-A2 RED: data={restore:true} 非空 dict 被误截获 —— 恰薄载荷判据破。")


# --------------------------------------------------------------------------- #
# K7-A3 — ECS 开关默认关（C1 挂账机判齿）
# --------------------------------------------------------------------------- #

class K7A3SwitchAcceptance(unittest.TestCase):
    """K7-A3: 默认配置 grid restore relay 不发出；开关开 = 发出（正例归 A1/A7 齿）。"""

    maxDiff = None

    def test_a3_switch_decision_face_default_off(self):
        """coverage: K7-A3 — 决策面默认关偏置：unset/空串/不可识别值一律 False；
        env 名常量逐字冻结（跨组件配置协议，同 MARKER_FILENAME 钉法）。"""
        enabled, err = _push_attr("grid_restore_enabled")
        if enabled is None:
            self.fail("K7-A3 RED: " + err)
        env_name, err2 = _push_attr("GRID_RESTORE_ENV")
        if env_name is None:
            self.fail("K7-A3 RED: " + err2)
        self.assertEqual(
            env_name, SWITCH_ENV,
            f"K7-A3 RED: GRID_RESTORE_ENV = {env_name!r} != {SWITCH_ENV!r} —— "
            "开关 env 名是 C1 closeout 配置协议（开闸留痕/文档按此名），改名 = "
            "部署面断线。")
        for raw in (None, "", "   ", "0", "false", "off", "no", "junk"):
            with _Switch(raw if raw is not None else None):
                self.assertFalse(
                    enabled(),
                    f"K7-A3 RED: grid_restore_enabled({raw!r}) = True —— 默认关"
                    "偏置破（C1 挂账：开关默认关交付，误配永不误开闸）。")

    def test_a3_switch_on_positive(self):
        """coverage: K7-A3 — 开关开正例：识别集 {"1","true","on","yes"}（大小写/
        空白容忍）→ True；no-arg 形态读 env（front 接线用）。"""
        enabled, err = _push_attr("grid_restore_enabled")
        if enabled is None:
            self.fail("K7-A3 RED: " + err)
        for raw in ("1", "true", "ON", "Yes", " true "):
            with _Switch(raw):
                self.assertTrue(
                    enabled(),
                    f"K7-A3 RED: grid_restore_enabled({raw!r}) = False —— 开关开"
                    "正例失真（正例齿无真开态 = 开闸验证无从机判）。")


# --------------------------------------------------------------------------- #
# K7-A4 — alert 起轮不回归（_register_alert_legs 时序零偏移）
# --------------------------------------------------------------------------- #

def _front_region(src: str, start: str, end: "str | None") -> str:
    s = src.find(start)
    if s < 0:
        return ""
    e = src.find(end, s + len(start)) if end else -1
    return src[s:e] if e > 0 else src[s:]


class K7A4AlertTimingAcceptance(unittest.TestCase):
    """K7-A4: _register_alert_legs 注册时序零偏移（提醒链路与课表推送共用建连
    线路段——plan §5 风险 1 的钉死齿）。"""

    maxDiff = None

    def test_a4_legs_registered_before_loop_before_connect_push(self):
        """coverage: K7-A4 — ws_gateway 区内时序：登记提醒腿 -> 起轮询 -> 建连
        推送（位次零偏移；建连推送提前或腿登记被挪后 = 投递/推送竞态面）。"""
        src = FRONT_APP.read_text(encoding="utf-8")
        region = _front_region(src, "async def ws_gateway", None)
        self.assertTrue(region, "K7-A4 RED(infra): ws_gateway 区切取失败——锚漂移。")
        p_reg = region.find("_register_alert_legs(websocket")
        p_loop = region.find("_ensure_alert_loop()")
        p_push = region.find("_push_schedule_connect(websocket)")
        self.assertTrue(p_reg >= 0, "K7-A4 RED: ws_gateway 区 _register_alert_legs "
                                    "丢失——提醒投递腿登记被拆（K7-A4 时序零偏移）。")
        self.assertTrue(p_loop >= 0, "K7-A4 RED: ws_gateway 区 _ensure_alert_loop "
                                     "丢失——轮询起轮点被拆。")
        self.assertTrue(p_push >= 0, "K7-A4 RED: ws_gateway 区无 _push_schedule_"
                                     "connect 调用——建连推送缝未接线（K7-A1）。")
        self.assertTrue(
            p_reg < p_loop < p_push,
            f"K7-A4 RED: ws_gateway 时序偏移 reg@{p_reg} loop@{p_loop} push@{p_push} "
            "—— 必须 登记腿 -> 起轮询 -> 建连推送（同序起点先跑一轮的语义保持）。")

    def test_a4_poll_loop_alert_first_schedule_second(self):
        """coverage: K7-A4 — reminder_poll_loop：起轮与循环体都是 alert 先、
        schedule 后（课表 piggyback 不得反插到提醒投递之前）。"""
        src = FRONT_APP.read_text(encoding="utf-8")
        region = _front_region(src, "async def reminder_poll_loop",
                               "def _ensure_alert_loop")
        self.assertTrue(region, "K7-A4 RED(infra): reminder_poll_loop 区切取失败。")
        first_alert = region.find("await run_alert_poll_once()")
        first_sched = region.find("await run_schedule_poll_once()")
        self.assertTrue(0 <= first_alert < first_sched,
                        "K7-A4 RED: 起轮序非 alert 先 schedule 后（plan §5 提醒"
                        "链路零回归面）。")
        body_alert = region.find("await run_alert_poll_once()", first_alert + 1)
        body_sched = region.find("await run_schedule_poll_once()", first_sched + 1)
        self.assertTrue(0 <= body_alert < body_sched,
                        "K7-A4 RED: 循环体序非 alert 先 schedule 后。")

    def test_a4_alert_delivery_cycle_zero_offset(self):
        """coverage: K7-A4（行为级）— 真跑一轮 run_alert_poll_once（假腿注入）：
        到点档位仍投到设备腿恰一帧（既有投递闭环零偏移）。"""
        front = _front()
        gw_reminders = front.gw_reminders
        now_iso = datetime.now().astimezone().isoformat(timespec="seconds")
        entry = {"id": "k7-a4-zero-offset", "fire_at": now_iso,
                 "title": "钢琴课 16:30 开始前", "speech": "",
                 "view": {"view": "text", "data": {"text": "16:30 钢琴课 @琴行"}}}
        ok, reason = gw_reminders.validate_alert_entry(entry)
        self.assertTrue(ok, f"K7-A4 RED(infra): 探针条目未过机械校验 ({reason})")

        async def _one_cycle():
            with tempfile.TemporaryDirectory(prefix="k7_a4_") as tmp:
                pending = Path(tmp) / "pending.json"
                pending.write_text(json.dumps(
                    {"version": 1, "generated_at": now_iso, "alerts": [entry]},
                    ensure_ascii=False), encoding="utf-8")
                old = os.environ.get("KIDPAL_PENDING_JSON")
                os.environ["KIDPAL_PENDING_JSON"] = str(pending)
                device = _FakeDevice()
                front._alert_dispatcher = None
                front._register_alert_legs(device, object())
                try:
                    res = await front.run_alert_poll_once()
                    for _ in range(24):  # 放 sink create_task 的投递任务跑完
                        await asyncio.sleep(0)
                finally:
                    front._unregister_alert_legs(device)
                    front._alert_dispatcher = None
                    if old is None:
                        os.environ.pop("KIDPAL_PENDING_JSON", None)
                    else:
                        os.environ["KIDPAL_PENDING_JSON"] = old
                return res, device

        res, device = asyncio.run(_one_cycle())
        self.assertEqual(res["pushed"], [entry["id"]],
                         f"K7-A4 RED: 到点档位未推出（pushed={res['pushed']!r}）——"
                         "投递闭环前置不成立。")
        alerts = [f for f in _display_frames(device.sent_text)]
        self.assertEqual(len(alerts), 0,
                         "K7-A4 RED(infra): 提醒走的是 kidpal/alert 信封而非 display"
                         "帧——探针形状漂移，须复核。")
        mcp_alerts = 0
        for f in device.sent_text:
            obj = json.loads(f)
            if isinstance(obj, dict) and obj.get("type") == "mcp" and \
                    ((obj.get("payload") or {}).get("params") or {}).get("name") \
                    == "kidpal/alert":
                mcp_alerts += 1
        self.assertEqual(mcp_alerts, 1,
                         f"K7-A4 RED: 设备腿 kidpal/alert 帧数 {mcp_alerts} != 1 ——"
                         "alert 投递面被 K7 改动连带偏移（时序零回归义务）。")


# --------------------------------------------------------------------------- #
# K7-A7 — 每日 / marker 触发附带（piggyback 同型 + 不新增第二套日账）
# --------------------------------------------------------------------------- #

class K7A7PiggybackAcceptance(unittest.TestCase):
    """K7-A7: run_schedule_poll_once 两触发 piggyback grid 同型（home 先 grid 后）、
    开关门控、恰一次（不新增第二套进程内日账）。marker 附带 = SHOULD 级（R8）。"""

    maxDiff = None

    def _reset_ledger(self, front) -> None:
        front._daily_push_local_date = None

    def _run_poll(self, front, now_utc):
        return asyncio.run(front.run_schedule_poll_once(now_utc=now_utc))

    def test_a7_daily_trigger_piggyback_grid_same_frames(self):
        """coverage: K7-A7 — 触发②（本地零点跨天）piggyback：设备腿收到 [home,
        schedule+restore] 同型帧序；同本地日二轮恰零新帧（既有日账恰一次语义
        承载 grid，不新增第二套日账的行为级证明）。"""
        front = _front()
        self._reset_ledger(front)
        crossed = datetime(2026, 9, 21, 16, 5, tzinfo=dt_timezone.utc)  # 沪 09-22 00:05
        with _TempMd(B1_MD), _Switch("1"):
            device = _FakeDevice()
            front._register_alert_legs(device, object())
            try:
                self._run_poll(front, crossed)
                first = _display_frames(device.sent_text)
                n_after_first = len(device.sent_text)
                self._run_poll(front, crossed)          # 同本地日二轮
                second = _display_frames(device.sent_text[n_after_first:])
            finally:
                front._unregister_alert_legs(device)
        self.assertEqual(
            [v for v, _d in first], ["home", "schedule"],
            f"K7-A7 RED: 触发② piggyback 帧序 {[v for v, _ in first]} != "
            "[home, schedule] —— grid 未同型附带或顺序偏移。")
        self.assertIs(first[1][1].get("restore"), True,
                      "K7-A7 RED: piggyback grid 帧 restore 未置 true。")
        self.assertEqual(
            second, [],
            f"K7-A7 RED: 同本地日二轮仍发 {[v for v, _ in second]} —— 恰一次破"
            "（第二套日账/无账重复推）。")

    def test_a7_marker_trigger_piggyback_grid(self):
        """coverage: K7-A7 — 触发③（录入 marker，SHOULD 级 R8）：marker 消费恰
        一次、帧序同型；消费后零残留。"""
        front = _front()
        self._reset_ledger(front)
        with _TempMd(B1_MD) as t, _Switch("1"):
            alerts_dir = Path(front.ALERTS_DIR)
            alerts_dir.mkdir(parents=True, exist_ok=True)
            marker = alerts_dir / "schedule.refresh"
            marker.write_text("", encoding="utf-8")
            device = _FakeDevice()
            front._register_alert_legs(device, object())
            try:
                self._run_poll(front, datetime.now(tz=dt_timezone.utc))
                frames = _display_frames(device.sent_text)
                n_after_first = len(device.sent_text)
                self._run_poll(front, datetime.now(tz=dt_timezone.utc))
                second = _display_frames(device.sent_text[n_after_first:])
            finally:
                front._unregister_alert_legs(device)
        self.assertEqual(
            [v for v, _d in frames], ["home", "schedule"],
            f"K7-A7 RED: 触发③ piggyback 帧序 {[v for v, _ in frames]} != "
            "[home, schedule]（marker 附带 = SHOULD 级，但既然附带必须同型）。")
        self.assertFalse(
            marker.exists(),
            "K7-A7 RED: marker 未被消费移除 —— 下轮重复推送。")
        self.assertEqual(second, [], "K7-A7 RED: marker 已消费仍二轮推送 —— 恰一次破。")

    def test_a7_piggyback_gated_by_switch_default_off(self):
        """coverage: K7-A3 + K7-A7 — 默认关：三触发 piggyback 也不发 grid 帧
        （C1 默认关在全触发面生效，不只建连点）。"""
        front = _front()
        self._reset_ledger(front)
        crossed = datetime(2026, 9, 21, 16, 5, tzinfo=dt_timezone.utc)
        with _TempMd(B1_MD), _Switch(None):
            alerts_dir = Path(front.ALERTS_DIR)
            alerts_dir.mkdir(parents=True, exist_ok=True)
            (alerts_dir / "schedule.refresh").write_text("", encoding="utf-8")
            device = _FakeDevice()
            front._register_alert_legs(device, object())
            try:
                self._run_poll(front, crossed)
            finally:
                front._unregister_alert_legs(device)
        views = [v for v, _d in _display_frames(device.sent_text)]
        self.assertNotIn(
            "schedule", views,
            f"K7-A7 RED: 开关默认关仍发出 grid 帧（views={views}）—— C1 默认关"
            "在 piggyback 路径失效。")

    def test_a7_no_second_daily_ledger_structural(self):
        """coverage: K7-A7（源锚代理）— 既有日账 _daily_push_local_date 恰 4 处
        （声明 + global + 读 + 写，2026-09-23 冻结基线 grep 实证）；grid 附带经
        grid_restore_payload 单源（front 引用 >= 1 处），无第二套记账形态。"""
        front_src = FRONT_APP.read_text(encoding="utf-8")
        n = front_src.count("_daily_push_local_date")
        self.assertEqual(
            n, 4,
            f"K7-A7 RED: front_app.py _daily_push_local_date = {n} 处（冻结基线 4）"
            "—— 日账被改名/复制（第二套进程内日账入口，F3 纪律破）。")
        self.assertGreaterEqual(
            front_src.count("grid_restore_payload"), 1,
            "K7-A7 RED: front_app.py 未引用 grid_restore_payload —— piggyback "
            "grid 未走单源 producer。")
        push_src = PUSH_MODULE.read_text(encoding="utf-8")
        self.assertIn("def grid_restore_payload", push_src,
                      "K7-A7 RED: schedule_push.py 无 grid_restore_payload 定义 —— "
                      "grid 变体 producer 未落单源决策面。")


# --------------------------------------------------------------------------- #

if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    for cls in (K7A1ConnectFrameOrderAcceptance, K7A2ExpandNeverRestoreAcceptance,
                K7A3SwitchAcceptance, K7A4AlertTimingAcceptance,
                K7A7PiggybackAcceptance):
        suite.addTests(loader.loadTestsFromTestCase(cls))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
