"""KidPal 三触发 producer 决策面（TaskPackage 260921-m3-k1-cloud-schedule-producer）。

本模块 = 纯决策面（触发①②③的可观测判定 + 单源推送内容），纯 python、import 时不依赖
fastapi/gateway —— offline 任何位置可导入。触发调度 / 接线（ws 建连握手、每日零点节拍、
轮询循环 piggyback）属前端 front_app 接线实现细节，不落本模块（可观测决策在此钉住）。

冻结契约（server/acceptance/test_m3k1_schedule_producer.acceptance.py 文件头）：

  MARKER_FILENAME = "schedule.refresh"   # 跨组件协议常量：OpenClaw 主通道改写
                                           # schedule.md 后落既有 alerts 交接目录
                                           # (pending.json 同目录) 的 refresh marker，
                                           # front 轮询 piggyback 消费（取舍点 C 甲案）。
  connect_push_payload(model, today_iso, now_hhmm, tomorrow_iso, school_start=None)
      触发① WS 建连即推的推送内容：必须 == home_upcoming_payload(同参)（三触发单源，
      防每触发各算一套漂移）。
  daily_push_due(timezone, last_push_local_date, now_utc) -> bool
      触发② 每日零点判定。timezone 取 B1 frontmatter；语义 = now_utc 在该时区的本地
      日历日 != last_push_local_date 即 due（本地零点后第一 tick 触发；调用方记下已推
      本地日 -> 每 local 日恰一次）。last_push=None -> True（尚未推过）。
      "Asia/Shanghai" = UTC+8（house 先例即用固定 +8 偏移，见展示下方说明）；timezone
      为空 -> 默认按 Asia/Shanghai。其它时区不在役、行为不钉 -> ValueError 大声拒绝
      （绝不静默吞）。
  refresh_requested(alerts_dir) -> bool
      触发③ 主通道即时请求是否 pending（alerts_dir 下 MARKER_FILENAME 在场）。副通道
      直改 schedule.md 不落 marker -> 恒 False（主/副通道判别不被 mtime 抹平）。
  consume_refresh_marker(alerts_dir) -> bool
      消费一个 pending 请求：True + marker 移除（恰一次）；无 marker -> False。

时区换算约束：host（Windows）无 tzdata，禁用 zoneinfo —— 全部按 house 先例固定偏移
UTC+8（reminder_schedule_lib._TZ8 / story._CN_TZ，B1 唯一在役时区 Asia/Shanghai）。
"""

from __future__ import annotations

import importlib.util
import os
from datetime import datetime, timedelta, timezone
from pathlib import Path

# 固定 UTC+8 偏移（house 先例：reminder_schedule_lib._TZ8 / story._CN_TZ；B1 唯一在役
# 时区 Asia/Shanghai）。host(Windows) 无 tzdata -> 不用 zoneinfo，偏移直算。
_TZ8 = timezone(timedelta(hours=8))
_CN_TZ_NAME = "Asia/Shanghai"

# schedule.py 单源 producer 的定位基准（本模块旁、同 gateway 目录）——按文件路径加载，
# 不依赖调用方 sys.path，保证「离线任意位置可导入」可独立成立。
_SCHEDULE_PY = Path(__file__).resolve().parent / "schedule.py"
_schedule_src = None


# OpenClaw 主通道改写 schedule.md 后落刷新 marker 的跨组件协议常量（靠既有 alerts
# 交接目录 pending.json 同目录承载）。front 轮询循环 piggyback 检查并消费。
MARKER_FILENAME = "schedule.refresh"

# ---- ECS 网格恢复性推送开关（ADR-005 部署时序：默认关交付；开闸 = 真机验证后
# ---- human/integrator 人工动作，C1 closeout 职责）。跨组件配置协议同 MARKER_FILENAME
# ---- 钉法——开关 env 名是开闸留痕/部署文档按此名的单源，改名 = 部署面断线（server
# ---- pytest K7-A3 断言 GRID_RESTORE_ENV == "KIDPAL_GRID_RESTORE_PUSH" 逐字冻结）。
GRID_RESTORE_ENV = "KIDPAL_GRID_RESTORE_PUSH"

# grid restore 开关识别集（strip().lower() ∈ 本集 -> True；其余一律 False——默认关
# 偏置：误配/陈旧环境值永不误开闸，符合 C1 挂账「只填缓存不接管前台」的门控前提）。
_GRID_RESTORE_TRUE_SET = {"1", "true", "on", "yes"}


def grid_restore_enabled(raw: "str | None" = None) -> bool:
    """ECS 网格恢复性推送开关决策面（纯函数；default None = 读 os.environ）。

    strip().lower() ∈ {"1","true","on","yes"} -> True；unset/""/其它任何值（含
    "0"/"false"/"off"/"no"）-> False。默认关偏置：不可识别的陈旧环境值绝不误开闸
    （C1 closeout：开关默认关交付，开闸 = 真机验证后 human 人工动作）。
    """
    if raw is None:
        raw = os.environ.get(GRID_RESTORE_ENV)
    if not isinstance(raw, str):
        return False
    return raw.strip().lower() in _GRID_RESTORE_TRUE_SET


def grid_restore_payload(model: dict, week_date_iso: str,
                         now_hhmm: "str | None" = None,
                         school_start: "str | None" = None) -> dict:
    """grid restore 变体 producer（契约 §1.3② data.restore additive 布尔 L85）。

    单源委托 schedule.py week_grid_payload（同参——绝不另起第二套网格计算），在产出
    上把 data["restore"] 置 True 并保持 data 键集恰 {"week","restore"}（L87 板子行为
    句：true = 只填缓存不接管前台）。生产纪律：只有本变体（front 后台附带推送路径）可
    置 restore；薄载荷展开路径（expand 意图）恒不置——两者键集互为反证。
    """
    payload = _schedule_callable("week_grid_payload")(
        model, week_date_iso, now_hhmm, school_start)
    payload["data"]["restore"] = True
    return payload


def _alerts_marker(alerts_dir) -> Path:
    return Path(alerts_dir) / MARKER_FILENAME


def _schedule_callable(name):
    """按文件路径加载 schedule.py 单源 producer 模块，并取指定 callable。"""
    global _schedule_src
    if _schedule_src is None:
        spec = importlib.util.spec_from_file_location("kidpal_gateway_schedule_single_source",
                                                      _SCHEDULE_PY)
        if spec is None or spec.loader is None:
            raise ImportError(f"cannot load {_SCHEDULE_PY}")
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _schedule_src = mod
    return getattr(_schedule_src, name)


def connect_push_payload(model: dict, today_iso: str, now_hhmm: str,
                         tomorrow_iso: str,
                         school_start: str | None = None) -> dict:
    """触发① WS 建连即推的推送内容 = home_upcoming_payload 同参单源，防各触发漂移。

    委托 schedule.py 同一 producer（按文件路径加载，offline 独立可导入），三触发共用
    一套计算，不得每触发各产一套。"""
    return _schedule_callable("home_upcoming_payload")(
        model, today_iso, now_hhmm, tomorrow_iso, school_start)


def _resolve_tz(cls_tz: str):
    name = (cls_tz or "").strip()
    if not name:                      # 空 timezone 缺省 = Asia/Shanghai（house 先例兜底）
        name = _CN_TZ_NAME
    if name != _CN_TZ_NAME:
        raise ValueError(
            f"timezone {name!r} not pinned (only B1 唯一在役时区 {_CN_TZ_NAME} 支撑); "
            "其它时区行为不钉、绝不静默吞。")
    return _TZ8


def daily_push_due(timezone: str, last_push_local_date: str | None,
                   now_utc: datetime) -> bool:
    """触发② 每日零点判定：now_utc 在该时区（UTC+8）的本地日历日 != 已推本地日即 due。

    last_push_local_date=None（尚未推过）-> True；本地零点前（同一本地日）不 due。
    """
    tz = _resolve_tz(timezone)
    local_date = now_utc.astimezone(tz).date().isoformat()
    if last_push_local_date is None:
        return True
    return local_date != str(last_push_local_date)


def refresh_requested(alerts_dir) -> bool:
    """触发③ 主通道即时请求是否 pending（refresh marker 在场）。

    副通道直改 schedule.md 不落 marker -> 恒 False（AC2 副通道无即时推送，靠①②兜底；
    主/副判别靠 marker 存在性，非 mtime）。"""
    return _alerts_marker(alerts_dir).exists()


def consume_refresh_marker(alerts_dir) -> bool:
    """消费一个 pending 即时请求：有 marker 则移除并返回 True（恰一次）；无则 False。"""
    marker = _alerts_marker(alerts_dir)
    if not marker.exists():
        return False
    try:
        marker.unlink()
    except FileNotFoundError:  # 竞态下已被消费 —— 视为无请求
        return False
    return True