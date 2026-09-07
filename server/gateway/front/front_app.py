"""KidPal direction-B wss TLS gateway FRONT (assembles server/gateway/*).

Composes the frozen adaptation modules into a runnable TLS-terminating gateway front
that the live acceptance (test_gateway_directionB.acceptance.py, LIVE face) probes:

  * LISTEN 0.0.0.0:8000 with TLS (self-signed >=10y cert, GWA-A9).
  * WS handshake: forced device auth AT THE HTTP UPGRADE layer — no token / wrong
    token => reject before accept() => Starlette answers an HTTP 403 (4xx) the live
    probe reads. token 未设 = refuse-service (auth.authorize never true for empty),
    constant-time compare (gateway.auth). No validation-mode backdoor.
  * POST /command: Bearer auth (+ 401), empty text => 400 (never silent 200).
  * Normalised stream/wss path: accepted authenticated WS -> forwarded to the engine
    (plain ws://127.0.0.1:8010/xiaozhi/v1/, the official container's ws leg) so the
    front is the device-facing wss entry and the engine stays plain ws inside.
  * Adaptation assembly demonstrated on the text/command + dryrun paths:
    display_schema mechanical check, mcp_relay tools/call envelope, fallback
    (4s soothing / error fallback text view / V32 truncate), provider stream shape.

Run (uvicorn, TLS on 0.0.0.0:8000):
  cd server/gateway/front && \
  /opt/kidpal/venv/bin/uvicorn front_app:app \
    --host 0.0.0.0 --port 8000 \
    --ssl-certfile /opt/kidpal/xiaozhi-esp32-server/cert/server.pem \
    --ssl-keyfile  /opt/kidpal/xiaozhi-esp32-server/cert/server.key
env: XIAOZHI_DEVICE_TOKEN (>=22 chars; unset => refuse-service), KIDPAL_ENGINE_WS.
"""

from __future__ import annotations

import asyncio
import json
import logging
import os
import re
from typing import Optional

from fastapi import FastAPI, Header, Request, WebSocket, WebSocketDisconnect, HTTPException
from fastapi.responses import JSONResponse, PlainTextResponse

# --- compose the frozen gateway building blocks (absolute package import) -----
from gateway import auth as gw_auth
from gateway import display_schema as gw_display
from gateway import mcp_relay as gw_mcp
from gateway import fallback as gw_fallback
from gateway import provider as gw_provider

app = FastAPI(title="KidPal direction-B streaming gateway front")

log = logging.getLogger("kidpal-front")
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(name)s %(message)s")

DEVICE_TOKEN = os.environ.get("XIAOZHI_DEVICE_TOKEN", "")
ENGINE_WS = os.environ.get("KIDPAL_ENGINE_WS", "ws://127.0.0.1:8010/xiaozhi/v1/")
# Engine side-channel message type (pod-events engine-patch-approval): the patched engine
# emits the stripped display blocks on this type; front is the validation/relay AUTHORITY
# (drives them through display_schema + kidpal/display tools/call) and never forwards the
# engine-internal raw frame to the device.
SIDE_CHANNEL_TYPE = "kidpal_display_blocks"


def _bearer(header: Optional[str]) -> str:
    """Pick the raw Authorization value (or ''), ready for constant-time compare."""
    return header or ""


def _require_device(authorization: Optional[str]) -> tuple[bool, int]:
    """Mechanical forced-auth gate (mechanical short-circuit like ARQ-260902 lesson)."""
    return gw_auth.authorize(DEVICE_TOKEN, _bearer(authorization))


def _unauthorized() -> JSONResponse:
    return JSONResponse(status_code=401, content={
        "error": {"category": "auth", "fingerprint": "front.bad-device-token",
                  "failingStep": "require_device", "message": "invalid/missing device bearer token"}})


@app.get("/healthz")
async def healthz():
    return {"status": "ok", "gateway_modules": [
        gw_auth.TOKEN_MIN_CHARS, bool(gw_provider.FEATURE_STREAMING_FORBIDS_MAX_TOKENS),
        sorted(gw_display.VALID_VIEWS), gw_fallback.SOOTHING_TIMEOUT_MS, gw_mcp.TOOL_DISPLAY]}


@app.post("/command")
async def command(request: Request, authorization: Optional[str] = Header(default=None)):
    """Boundary: Bearer auth then empty-text -> 400 (never silently 200). Non-empty -> ok."""
    ok, status = _require_device(authorization)
    if not ok:
        return _unauthorized()
    try:
        body = await request.json()
    except Exception:  # noqa: BLE001
        return JSONResponse(status_code=400, content={"error": {"message": "body must be JSON"}})
    text = (body.get("text") or "").strip() if isinstance(body, dict) else ""
    if not text:
        # same empty-input boundary as the ws/shim path: reject 400.
        return JSONResponse(status_code=400, content={"error": {
            "category": "validation", "fingerprint": "front.empty-text",
            "failingStep": "command.validate", "message": "empty text rejected"}})
    # non-empty: assemble a kidpal/display mcp relay as a minimal adaptation smoke.
    display = {"view": "text", "data": {"text": text}}
    ok_disp, reason = gw_display.validate_display(display)
    envelope = gw_mcp.display_tool_call(display) if ok_disp else {"error": "invalid-display:" + reason,
                                                                  "fallback": gw_fallback.ERROR_FALLBACK}
    return JSONResponse(status_code=202, content={"accepted": True, "mcp": envelope,
                                                  "correlation_id": gw_provider.corr_id()})


@app.post("/adaptation/dryrun")
async def adaptation_dryrun(request: Request, authorization: Optional[str] = Header(default=None)):
    """Probe the gateway adaptation assembly (validate_display + truncate + relay)."""
    ok, status = _require_device(authorization)
    if not ok:
        return _unauthorized()
    try:
        payload = await request.json()
    except Exception:  # noqa: BLE001
        return JSONResponse(status_code=400, content={"error": {"message": "body must be JSON"}})
    view = (payload or {}).get("view", "text") if isinstance(payload, dict) else "text"
    data = (payload or {}).get("data", {"text": ""}) if isinstance(payload, dict) else {"text": ""}
    ok_disp, reason = gw_display.validate_display({"view": view, "data": data})
    truncated = gw_fallback.truncate_for_display((data.get("text") or ""))
    return JSONResponse(content={
        "slide": ok_disp, "reason": reason,
        "v32_truncated": truncated,
        "soothing_timeout_ms": gw_fallback.SOOTHING_TIMEOUT_MS,
        "relay": gw_mcp.display_tool_call({"view": "text", "data": {"text": truncated}}) if ok_disp else None})


# --- ws-proxy text takeover (TaskPackage 260907-g-relay-fix / 契约 §1.5 step 3) -----
# FROZEN OFFLINE CONTRACT — see server/acceptance/test_g_relay_fix.acceptance.py. These
# four callables + the wired pumps in ws_gateway below are the defect body fix: the real
# proxy path now takes over the engine text stream (the /command & /dryrun demo paths were
# never the defect). 决策23: 设备 JSON-RPC 回执 = 送达确认, front 只消费不重发.

# Fence marker as produced by OpenClaw (contract §1.4). The engine's sentence splitter can
# strip the newline after the opener (live session 95ccf407), so the opener is matched in
# BOTH canonical (```display \n <json> \n ```) and inline (```display<json>```) forms.
_DISPLAY_FENCE_RE = re.compile(r"```display(.*?)```", flags=re.DOTALL)


def strip_display_fences(text: str) -> tuple[str, "list[str]"]:
    """Pure syntactic split of engine LLM text (contract §1.5 step 3 is the gateway's job).

    Returns (speech_text, display_blocks). Every ```display fenced block is removed from
    the speech track with ZERO residue (no ``` and no fence-body chars leak into speech),
    and the raw fence bodies are returned verbatim in order as a list of str — NOT parsed,
    so dirty bodies are still handed to the downstream mechanical check (display_schema) to
    reject. Text outside fences is preserved verbatim, in order. No fence -> (text, []).
    """
    blocks = [m.group(1) for m in _DISPLAY_FENCE_RE.finditer(text)]
    speech = _DISPLAY_FENCE_RE.sub("", text)
    return speech, blocks


def _friendly_downgrade_view(_reason: str) -> dict:
    """A non-empty, schema-legal friendly text view to downgrade a dirty/supersized display
    block to — never the raw dirty payload (contract §1.5: 脏 JSON 拒绝并降级友好提示; V27)."""
    return {"view": "text", "data": {"text": "这张小卡片没显示好，先专心听我说话吧。"}}


def _display_wire_arguments(payload: dict) -> dict:
    """The device wire shape for kidpal/display arguments (firmware mcp_server.cc:105 — BOTH
    the view and data Properties are String-typed; the device parses the data string itself,
    an object-typed data is rejected)."""
    return {"view": payload.get("view"), "data": json.dumps(payload.get("data"), ensure_ascii=False)}


def build_display_tool_calls(blocks: list) -> list:
    """One kidpal/display JSON-RPC tools/call envelope per block, in order, NEVER silently
    dropped (len(result) == len(blocks)).

    A block that parses as JSON and passes display_schema.validate_display is relayed with
    arguments at the DEVICE wire shape {"view": <str>, "data": <json.dumps(data) STRING>}
    (data round-trips via json.loads(arguments["data"]) == the validated payload's data).
    Anything else (dirty / schema reject) is DOWNGRADED, not silent: a friendly text view
    that itself passes validate_display with non-empty text, never the raw dirty payload.
    """
    calls = []
    for block in blocks:
        payload = None
        if isinstance(block, str):
            try:
                payload = json.loads(block)
            except (TypeError, ValueError):
                payload = None
        if payload is not None:
            ok, reason = gw_display.validate_display(payload)
        else:
            ok, reason = False, "display block is not JSON-object parseable"
        if ok:
            display = _display_wire_arguments(payload)
        else:
            display = _display_wire_arguments(_friendly_downgrade_view(reason))
        calls.append(gw_mcp.display_tool_call(display))
    return calls


async def _relay_display_blocks_to_device(blocks: list, websocket) -> None:
    """Send each display relay envelope toward the device as its OWN
    {"type":"mcp","payload":<envelope>} frame (firmware application.cc:492 routes ONLY
    type=mcp into McpServer::ParseMessage; :532 drops every other type — the deprecated
    display_relay wrapper ended exactly there).  多块多帧, no batching."""
    for envelope in build_display_tool_calls(blocks):
        await websocket.send_text(
            json.dumps({"type": "mcp", "payload": envelope},
                       ensure_ascii=False, default=str)
        )


async def pump_upstream_to_client(upstream, websocket) -> None:
    """Engine->device leg of the ws proxy WITH takeover (this is the defect body).

    upstream is an async-iterable of frames (str|bytes, websockets-client shape); the device
    websocket has send_text()/send_bytes(). Binary (audio) frames forward unchanged. Text
    frames that are JSON objects carrying a string "text" field (the llm/tts 文字同步 stream)
    are fence-stripped via strip_display_fences before forwarding, and each extracted display
    block is relayed toward the device as a kidpal/display tools/call envelope from
    build_display_tool_calls (a small wrapper text frame makes the envelopes discoverable).
    Frames without a text field (hello, state, ...) forward verbatim.
    """
    try:
        async for frame in upstream:
            if isinstance(frame, bytes):
                await websocket.send_bytes(frame)
                continue
            try:
                obj = json.loads(frame)
            except (TypeError, ValueError):
                await websocket.send_text(frame)  # non-JSON text: forward verbatim
                continue
            if not isinstance(obj, dict):
                await websocket.send_text(frame)  # non-object JSON: forward verbatim
                continue
            if obj.get("type") == SIDE_CHANNEL_TYPE:
                # engine-internal display-blocks side channel: INTERCEPT (never forwarded
                # verbatim — the device must not see the engine transport frame), drive the
                # raw blocks through the same relay chain.
                await _relay_display_blocks_to_device(obj.get("blocks") or [], websocket)
                continue
            text_field = obj.get("text") if isinstance(obj, dict) else None
            if isinstance(text_field, str):
                speech, blocks = strip_display_fences(text_field)
                if not blocks:
                    await websocket.send_text(frame)  # no display fence: verbatim
                    continue
                # forward the fence-stripped speech text on the original carrier frame…
                relay = dict(obj)
                relay["text"] = speech
                await websocket.send_text(json.dumps(relay, ensure_ascii=False))
                # …and each display envelope to the device as its own mcp frame.
                await _relay_display_blocks_to_device(blocks, websocket)
            else:
                await websocket.send_text(frame)  # no text field (hello/state): verbatim
    except Exception:  # noqa: BLE001 — proxy teardown ends the pump gracefully.
        pass


async def pump_client_to_upstream(websocket, upstream) -> list:
    """Device->engine leg: reads starlette-style dict messages via await websocket.receive()
    ({"type": "websocket.receive", "text"/"bytes": ...}), forwards every frame to
    upstream.send() UNCHANGED (决策23: 代理语义, front 只消费回执、不重发), consumes device
    JSON-RPC render receipts via on_device_receipt, and returns the list of receipt ids
    consumed as delivery acks. Client disconnect (WebSocketDisconnect / RuntimeError) ends.
    """
    acked_ids = []
    try:
        while True:
            msg = await websocket.receive()
            if msg.get("type") == "websocket.disconnect":
                break
            if msg.get("type") != "websocket.receive":
                continue
            if isinstance(msg.get("bytes"), bytes):
                await upstream.send(msg["bytes"])
            else:
                text = msg.get("text", "")
                await upstream.send(text)
                acked, rid = on_device_receipt(text)
                if acked:
                    acked_ids.append(rid)
    except (WebSocketDisconnect, RuntimeError):  # device leg closed — normal teardown.
        pass
    return acked_ids


def on_device_receipt(message) -> tuple[bool, object]:
    """Delegate the device JSON-RPC receipt to gateway.mcp_relay.relay_ack (决策23: 渲染即
    确认、即出队语义由设备侧承载 — front 不得自造分叉实现).  The device's receipt rides the
    wire frame {"type":"mcp","payload":<JSON-RPC>} (firmware mcp_server.cc:473 + 回执形态
    pinned in the acceptance) — the front unwraps payload before relay_ack (relay_ack 本体
    语义不动). Accepts a dict or a JSON string. Returns (acked: bool, id)."""
    if isinstance(message, str):
        try:
            message = json.loads(message)
        except (TypeError, ValueError):
            pass  # non-JSON — relay_ack rejects the non-dict below
    if (isinstance(message, dict) and message.get("type") == "mcp"
            and isinstance(message.get("payload"), dict)):
        message = message["payload"]  # unwrap the device wire frame before relay_ack
    return gw_mcp.relay_ack(message)


@app.websocket("/{path:path}")
async def ws_gateway(websocket: WebSocket):
    """Device leg: forced Bearer auth at HTTP upgrade; reject (403) before accept().

    A forwarded authenticated stream goes to the plain-ws engine (loopback). The live
    probes hit the auth-reject path; the engine forward is best-effort auxiliary.
    """
    # mechanical auth gate — no accept() on failure => Starlette answers 403 on the upgrade.
    provided = (websocket.headers.get("authorization") or "")
    ok, _status = gw_auth.authorize(DEVICE_TOKEN, provided)
    if not ok:
        log.warning("ws handshake rejected (auth) — no accept")
        return
    await websocket.accept()
    # best-effort forward to the engine's plain ws (loopback). If the engine is not up,
    # we close the client gracefully rather than error-spam.
    try:
        # pass the device's whitelist headers to the engine: it requires device-id
        # (websocket_server.py:83) and honours authorization / protocol-version / client-id.
        # case-insensitive match, preserve original value, token erased from logs.
        _ALLOWED_HEADERS = ("authorization", "device-id", "client-id", "protocol-version")
        hdrs = {h: v for h, v in websocket.headers.items() if h.lower() in _ALLOWED_HEADERS}
        try:
            from websockets.asyncio.client import connect as ws_connect  # websockets >= 13
            headers_kwarg = "additional_headers"
        except ImportError:  # legacy websockets (< 13)
            from websockets import client as _ws_legacy_client
            ws_connect = _ws_legacy_client.connect
            headers_kwarg = "extra_headers"
        async with ws_connect(ENGINE_WS, open_timeout=10, **{headers_kwarg: hdrs}) as upstream:
            # both proxy legs now run through the takeover pumps (text-face interception):
            # engine->device strips ```display fences + relays kidpal/display tools/call;
            # device->engine forwards receipts unchanged and consumes them as delivery acks.
            pump_up = asyncio.create_task(pump_upstream_to_client(upstream, websocket))
            pump_dn = asyncio.create_task(pump_client_to_upstream(websocket, upstream))
            await asyncio.gather(pump_up, pump_dn)
    except Exception as e:  # noqa: BLE001
        log.info("ws forward not available (engine down?): %s", type(e).__name__)
        await websocket.close(code=1011)