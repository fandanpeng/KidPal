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

import json  # noqa: F401
import logging
import time
import uuid

log = logging.getLogger("kidpal-gateway.mcp-relay")

TOOL_DISPLAY = "kidpal/display"
TOOL_ALERT = "kidpal/alert"
JSONRPC = "2.0"
METHOD_TOOLS_CALL = "tools/call"


def corr_id() -> str:
    """A Correlation-Id for the request path (K6 structured logging)."""
    return uuid.uuid4().hex


# Device wire fact (firmware mcp_server.cc:473): the JSON-RPC `id` MUST be a NUMBER
# (cJSON_IsNumber) — uuid hex strings are rejected "Invalid id".  Display relay ids are
# numeric (ms-timestamp + monotonic tag).  (alert_tool_call keeps its own deterministic
# reminder-id override below — the numeric-ification of that id belongs to the reminder-side
# package, NOT pinned in this transport round.)
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

    Reuses the reminder's deterministic `id` (contract §三: id = 确定性散列, 跨重推
    不变) as the JSON-RPC id so the device receipt maps back to the same reminder for
    iddempotent dedupe + 渲染回执出队. Falls back to a fresh uuid if payload carries none.
    """
    rid = (payload or {}).get("id")
    call = _tool_call(TOOL_ALERT, payload)
    if rid:
        call["id"] = rid
    return call


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