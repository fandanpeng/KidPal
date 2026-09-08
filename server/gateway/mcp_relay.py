"""mcp relay infrastructure (决策23 / GWA-A6).

Frozen acceptance GWA-A6. Pure-python, no fastapi/httpx at import time.

Decision 23: display 与提醒统一走 mcp 工具调用 (kidpal/display, kidpal/alert), 经
JSON-RPC tools/call 下发; 设备 JSON-RPC 回执 = 送达确认 (渲染回执出队). 小智原生
alert/custom 通道弃用.

Frozen offline contract:
    display_tool_call(payload) -> JSON-RPC tools/call {"id":…,
        "method":"tools/call", "params":{"name":"kidpal/display", "arguments":…}}
    alert_tool_call(payload)   -> same with "kidpal/alert"
    relay_ack(jsonrpc_message) -> (acked: bool, id)  (设备 JSON-RPC 回执 = 送达确认)

Aligns with contract §四 mcp 工具调用 + §三 (提醒 JSON-RPC 回执 = 渲染即确认、即出队).
"""

from __future__ import annotations

import importlib.util
import json  # noqa: F401
import logging
import os
import time
import uuid

log = logging.getLogger("kidpal-gateway.mcp-relay")

TOOL_DISPLAY = "kidpal/display"
TOOL_ALERT = "kidpal/alert"
JSONRPC = "2.0"
METHOD_TOOLS_CALL = "tools/call"


_reminders_cache = None


def _reminders_module():
    """懒取提醒投递模块（gateway/reminders.py）—— 信封形状与传输 id 算法的单一事实源。

    先按包导入（生产：front 以 server/ 为工作目录跑 uvicorn），退回按本文件同级路径加载，
    这样本模块被离线按路径单独导入时（GWA-A6 的装载方式）也不依赖包上下文；两条路加载的
    都是同一份 reminders.py，不分叉第二套算法。加载不了返回 None（调用方退回通用信封）。
    """
    global _reminders_cache
    if _reminders_cache is not None:
        return _reminders_cache
    try:
        from gateway import reminders as rm
        _reminders_cache = rm
        return rm
    except ImportError:
        pass
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "reminders.py")
    if not os.path.isfile(path):
        return None
    try:
        spec = importlib.util.spec_from_file_location("kidpal_gateway_reminders", path)
        if spec is None or spec.loader is None:
            return None
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
    except Exception:  # noqa: BLE001 — 可选依赖缺失时退回通用信封，绝不让 relay 层崩
        return None
    _reminders_cache = module
    return module


def corr_id() -> str:
    """A Correlation-Id for the request path (K6 structured logging)."""
    return uuid.uuid4().hex


# Device wire fact (firmware mcp_server.cc ParseMessage / cJSON_IsNumber): the JSON-RPC
# `id` MUST be a NUMBER — uuid/hex strings are rejected "Invalid id".  Display relay ids are
# numeric (ms-timestamp + monotonic tag).  alert_tool_call delegates to reminders'
# build_alert_envelope (包 260829-schedule-reminder 已落地该数字化: 确定性派生 +
# arguments.id 保留 §三 字符串业务 id)。
_display_id_seq = 0


def _next_numeric_id() -> int:
    """A unique numeric JSON-RPC id (ms timestamp + in-process monotonic tag)."""
    global _display_id_seq
    _display_id_seq += 1
    return int(time.time() * 1000) * 1000 + (_display_id_seq % 1000)


def _tool_call(tool: str, arguments) -> dict:
    """Build one JSON-RPC 2.0 tools/call envelope for the device leg."""
    return {
        "id": _next_numeric_id(),
        "jsonrpc": JSONRPC,
        "method": METHOD_TOOLS_CALL,
        "params": {"name": tool, "arguments": arguments},
    }


def display_tool_call(payload) -> dict:
    """Relay an E1 display payload to the board via kidpal/display tools/call."""
    return _tool_call(TOOL_DISPLAY, payload)


def alert_tool_call(payload) -> dict:
    """Relay a §三 提醒条目 to the board via kidpal/alert tools/call.

    本包的传输面收口（上游明文把「alert 传输 id 数字化」划归包 7）：§三 业务幂等 id 是
    **字符串**、必须留在 arguments.id 供设备易失去重，而设备 mcp 层只认**数字** JSON-RPC
    id（mcp_server.cc ParseMessage 的 cJSON_IsNumber 判据，字符串 id 被整条按 Invalid id
    拒收 —— g-relay-fix 二轮冒烟实证过的断点）。信封形状与派生算法的单一事实源 =
    gateway/reminders.build_alert_envelope（同机同源，不分叉第二套）；reminders 不可达的
    极端离线装载场合退回通用 _tool_call（display 路径与 relay_ack 冻结语义一律不动）。
    """
    reminders = _reminders_module()
    entry = dict(payload) if isinstance(payload, dict) else {}
    if reminders is not None:
        return reminders.build_alert_envelope(entry)
    return _tool_call(TOOL_ALERT, entry)


def relay_ack(jsonrpc_message) -> tuple[bool, object]:
    """Interpret a device JSON-RPC receipt as a delivery acknowledgement (送达确认).

    A device receipt with an `id` and a `result` (success ack) means the board rendered
    the alert/display => 渲染即确认、即出队 (contract §三). Returns (acked: bool, id)
    so the gateway can dequeue and, on no-ack within the window, re-push (上限 3 次).
    """
    if not isinstance(jsonrpc_message, dict):
        return (False, None)
    rid = jsonrpc_message.get("id")
    result = jsonrpc_message.get("result")
    is_ack = isinstance(result, dict) and result.get("ack") is True
    # tolerate either an explicit {"ack":true} result or any non-error result on a
    # known id (idempotent dedupe treats a rendered receipt as delivery).
    if isinstance(rid, (str, int)) and (is_ack or jsonrpc_message.get("result") is not None):
        return (True, rid)
    return (False, rid)