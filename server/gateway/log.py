"""Shared structured-log helper for the gateway runtime (K6 / observability-k6.md).

The frozen, offline-importable modules (auth / display_schema / provider / fallback /
mcp_relay) stay fully self-contained (no cross-imports) so the acceptance can import
each in isolation. This module is the *runtime* shared logger the deployed gateway
adapter uses to emit structured JSON log lines carrying a Correlation-Id.

Every request path picks up a cid (X-Correlation-Id header on the HTTP shim, or a
fresh gateway-generated cid on the wss leg) and threads it through provider / fallback /
mcp_relay so an operator can correlate one child turn end-to-end. ErrorEnvelope follows
observability-k6.md (category / fingerprint / failingStep / message / logTail),
and secrets are never logged.
"""

from __future__ import annotations

import json
import logging
import os
import time
import uuid

log = logging.getLogger("kidpal-gateway")
_default_handler: logging.Handler | None = None


def ensure_structured() -> None:
    """Attach a JSON-line handler once (idempotent) so runtime logs are structured."""
    global _default_handler
    if _default_handler is None:
        _default_handler = logging.StreamHandler()
        _default_handler.setFormatter(logging.Formatter("%(message)s"))
        log.setLevel(os.environ.get("KIDPAL_LOG_LEVEL", "INFO").upper())
        log.addHandler(_default_handler)
        log.propagate = False


def new_cid() -> str:
    """A fresh Correlation-Id (32-hex, uuid4)."""
    return uuid.uuid4().hex


def event(cid: str, ev: str, **fields) -> None:
    """Emit one structured log event with the Correlation-Id (K6)."""
    ensure_structured()
    row = {"cid": cid, "event": ev, "ts_ms": int(time.time() * 1000)}
    row.update(fields)
    log.info(json.dumps(row, ensure_ascii=False, default=str))


def envelope(category: str, fingerprint: str, step: str, message: str,
             tail: str = "", cid: str = "") -> dict:
    """ErrorEnvelope shape per observability-k6.md (never logs secrets)."""
    return {
        "cid": cid,
        "error": {
            "category": category,
            "fingerprint": fingerprint,
            "failingStep": step,
            "message": message,
            "logTail": (tail or "")[-500:],
        },
    }