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

import json
import logging
import os
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
        import websockets
        async with websockets.client.connect(ENGINE_WS, open_timeout=10) as upstream:
            async def client_to_upstream():
                try:
                    while True:
                        msg = await websocket.receive()
                        if msg.get("type") == "websocket.disconnect":
                            break
                        if msg.get("type") == "websocket.receive":
                            if isinstance(msg.get("bytes"), bytes):
                                await upstream.send(msg["bytes"])
                            else:
                                await upstream.send(msg.get("text", ""))
                except (WebSocketDisconnect, RuntimeError):
                    pass
            async def upstream_to_client():
                try:
                    async for frame in upstream:
                        if isinstance(frame, bytes):
                            await websocket.send_bytes(frame)
                        else:
                            await websocket.send_text(frame)
                except Exception:  # noqa: BLE001
                    pass
            import asyncio
            await asyncio.gather(client_to_upstream(), upstream_to_client())
    except Exception as e:  # noqa: BLE001
        log.info("ws forward not available (engine down?): %s", type(e).__name__)
        await websocket.close(code=1011)