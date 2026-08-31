"""KidPal cloud shim — thin bridge over OpenClaw (FastAPI).

Responsibilities (contract E1 §1.5, kept intentionally thin):
  1. POST /v1/chat  : device text -> OpenClaw /v1/chat/completions (loopback)
                      -> split 正文/display block -> 正文 to TTS -> {audio, display}
  2. POST /v1/asr   : audio (base64 mp3/wav) -> faster-whisper -> {text}
  3. POST /v1/tts   : text -> edge-tts -> {audio}
  4. GET  /v1/alerts: poll reminder file (stub-backed, reads alerts/pending.json)

Deliberately NOT here (per decision spec-§3.0.0 薄桥): business gateway, CRUD,
intent layer. This shim only carries ASR/TTS HTTP ports + device auth + alert
poll-forward. All intelligence stays in OpenClaw.

Boundary controls (security-hardening): device bearer token (weak, env-gated),
loopback-only bind, real OpenClaw token injected ONLY on loopback forward.
"""

from __future__ import annotations

import base64
import json
import logging
import os
import re
import tempfile
import time
import uuid
from typing import Optional

import edge_tts
import httpx
from fastapi import Depends, FastAPI, Header, Request
from fastapi.responses import JSONResponse
from pydantic import BaseModel

# --------------------------------------------------------------------------- #
# Config (env-driven; freeze-friendly)
# --------------------------------------------------------------------------- #
OPENCLAW_BASE = os.environ.get("OPENCLAW_BASE", "http://127.0.0.1:18789")
OPENCLAW_TOKEN = os.environ.get("OPENCLAW_TOKEN", "")
OPENCLAW_MODEL = os.environ.get("OPENCLAW_MODEL", "openclaw/default")
OPENCLAW_WORKSPACE = os.environ.get("OPENCLAW_WORKSPACE", "/root/.openclaw/workspace/kidpal")
# Device-facing weak token. Empty => auth disabled (validation mode). Set to a
# secure random value in production to require Authorization: Bearer on /v1/*.
SHIM_DEVICE_TOKEN = os.environ.get("SHIM_DEVICE_TOKEN", "")
# Local pre-downloaded CTranslate2 model dir (ECS can't reach huggingface.co).
CLB_WHISPER_MODEL = os.environ.get(
    "CLB_WHISPER_MODEL", "/opt/kidpal/models/faster-whisper-tiny"
)
TTS_VOICE = os.environ.get("TTS_VOICE", "zh-CN-XiaoxiaoNeural")

# Contract §1.6 system prompt — sent verbatim so DeepSeek always emits ```display.
SYSTEM_PROMPT = (
    "你是儿童学习伙伴。每次回复分两部分：先用口语化、适合8-12岁孩子的话回答"
    "（这部分会被念出来）；然后在末尾附一个 ```display 代码块，内含屏幕显示指令 JSON，"
    "view 取 char/schedule/story_list/story_text/text/home/control 之一。"
)

# Same fence splitter as contract E1 / frozen acceptance.
DISPLAY_RE = re.compile(r"```\s*display\s*\n(.*?)```", re.DOTALL)

log = logging.getLogger("kidpal-shim")
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(name)s %(message)s")

app = FastAPI(title="KidPal cloud shim", version="0.1.0")


# --------------------------------------------------------------------------- #
# helpers
# --------------------------------------------------------------------------- #
def err_env(category: str, fingerprint: str, step: str, message: str, tail: str = ""):
    """ErrorEnvelope shape aligned to observability-k6.md; never logs secrets."""
    return {
        "error": {
            "category": category,
            "fingerprint": fingerprint,
            "failingStep": step,
            "message": message,
            "logTail": tail[-500:],
        }
    }


def require_device(authorization: Optional[str] = Header(default=None)):
    """Device-facing weak-token gate. Disabled unless SHIM_DEVICE_TOKEN is set."""
    if not SHIM_DEVICE_TOKEN:
        return  # validation mode (frozen acceptance has no shim-auth header path)
    if not authorization or not authorization.startswith("Bearer "):
        return JSONResponse(status_code=401, content=err_env(
            "auth", "shim.missing-device-token", "require_device",
            "missing device bearer token"))
    if authorization[len("Bearer "):] != SHIM_DEVICE_TOKEN:
        return JSONResponse(status_code=401, content=err_env(
            "auth", "shim.bad-device-token", "require_device",
            "invalid device bearer token"))
    return None


async def openclaw_completion(text: str) -> str:
    """Forward one turn to OpenClaw loopback; returns raw completion content."""
    async with httpx.AsyncClient(timeout=120.0) as client:
        headers = {"Content-Type": "application/json"}
        if OPENCLAW_TOKEN:
            headers["Authorization"] = f"Bearer {OPENCLAW_TOKEN}"
        payload = {
            "model": OPENCLAW_MODEL,
            "messages": [
                {"role": "system", "content": SYSTEM_PROMPT},
                {"role": "user", "content": text},
            ],
        }
        r = await client.post(f"{OPENCLAW_BASE}/v1/chat/completions",
                              headers=headers, json=payload)
        r.raise_for_status()
        return r.json()["choices"][0]["message"]["content"]


def split_display(content: str):
    """Return (speech, display_json_or_None)."""
    m = DISPLAY_RE.search(content or "")
    if not m:
        return (content or "").strip(), None
    speech = content[: m.start()].strip()
    block = m.group(1).strip()
    try:
        display = json.loads(block)
    except json.JSONDecodeError:
        display = None
    return speech, display


async def tts_mp3(text: str) -> bytes:
    """Synthesize Chinese speech via edge-tts; returns mp3 bytes."""
    communicate = edge_tts.Communicate(text, TTS_VOICE)
    chunks = []
    async for chunk in communicate.stream():
        if chunk["type"] == "audio":
            chunks.append(chunk["data"])
    return b"".join(chunks)


# Lazy singleton: faster-whisper model is heavy; load only when ASR is used.
_whisper = None


def _whisper_model():
    global _whisper
    if _whisper is None:
        from faster_whisper import WhisperModel
        _whisper = WhisperModel(CLB_WHISPER_MODEL, device="cpu", compute_type="int8")
    return _whisper


def asr_transcribe(audio: bytes) -> str:
    """Transcribe a Chinese utterance. initial_prompt keeps output simplified (zh)."""
    with tempfile.NamedTemporaryFile(suffix=".mp3", delete=False) as f:
        f.write(audio)
        path = f.name
    try:
        segments, _info = _whisper_model().transcribe(
            path, language="zh", beam_size=1, initial_prompt="以下是普通话的句子。"
        )
        return "".join(s.text for s in segments)
    finally:
        try:
            os.unlink(path)
        except OSError:
            pass


# --------------------------------------------------------------------------- #
# request/response models
# --------------------------------------------------------------------------- #
class ChatIn(BaseModel):
    text: str


class AudioIn(BaseModel):
    audio: str  # base64 mp3/wav


class TtsIn(BaseModel):
    text: str


def b64(raw: bytes) -> str:
    return base64.b64encode(raw).decode("ascii")


# --------------------------------------------------------------------------- #
# routes
# --------------------------------------------------------------------------- #
@app.post("/v1/chat")
async def chat(body: ChatIn, request: Request, _auth=Depends(require_device)):
    cid = request.headers.get("X-Correlation-Id") or uuid.uuid4().hex
    text = (body.text or "").strip()
    if not text:
        # boundary: reject empty input, never silently 200.
        return JSONResponse(status_code=400, content=err_env(
            "validation", "shim.empty-text", "chat.validate",
            "empty text rejected"), headers={"X-Correlation-Id": cid})
    t0 = time.time()
    try:
        content = await openclaw_completion(text)
    except Exception as e:  # noqa: BLE001
        log.error("chat openclaw fail cid=%s err=%s", cid, str(e))
        return JSONResponse(status_code=502, content=err_env(
            "upstream", "shim.openclaw-failed", "chat.forward",
            "OpenClaw completion failed", str(e)), headers={"X-Correlation-Id": cid})
    speech, display = split_display(content)
    if not speech:
        speech = text  # never hand TTS an empty string
    if display is None:
        # fallback so the board still has a valid view even on a split miss.
        display = {"view": "text", "data": {"text": speech}}
    try:
        mp3 = await tts_mp3(speech)
    except Exception as e:  # noqa: BLE001
        log.error("chat tts fail cid=%s err=%s", cid, str(e))
        return JSONResponse(status_code=502, content=err_env(
            "tts", "shim.tts-failed", "chat.tts",
            "edge-tts synthesis failed", str(e)), headers={"X-Correlation-Id": cid})
    log.info("chat ok cid=%s view=%s speech_chars=%d audio_bytes=%d dt=%.1fs",
             cid, display.get("view"), len(speech), len(mp3), time.time() - t0)
    return JSONResponse(content={"audio": b64(mp3), "display": display},
                        headers={"X-Correlation-Id": cid})


@app.post("/v1/asr")
async def asr(body: AudioIn, _auth=Depends(require_device)):
    cid = uuid.uuid4().hex
    try:
        audio = base64.b64decode(body.audio)
    except Exception:  # noqa: BLE001
        return JSONResponse(status_code=400, content=err_env(
            "validation", "shim.bad-base64", "asr.decode", "audio not valid base64"),
            headers={"X-Correlation-Id": cid})
    if not audio:
        return JSONResponse(status_code=400, content=err_env(
            "validation", "shim.empty-audio", "asr.validate", "empty audio"),
            headers={"X-Correlation-Id": cid})
    t0 = time.time()
    try:
        text = asr_transcribe(audio)
    except Exception as e:  # noqa: BLE001
        log.error("asr fail cid=%s err=%s", cid, str(e))
        return JSONResponse(status_code=502, content=err_env(
            "asr", "shim.whisper-failed", "asr.transcribe", "whisper failed", str(e)),
            headers={"X-Correlation-Id": cid})
    log.info("asr ok cid=%s chars=%d dt=%.1fs", cid, len(text), time.time() - t0)
    return JSONResponse(content={"text": text}, headers={"X-Correlation-Id": cid})


@app.post("/v1/tts")
async def tts(body: TtsIn, _auth=Depends(require_device)):
    cid = uuid.uuid4().hex
    text = (body.text or "").strip()
    if not text:
        return JSONResponse(status_code=400, content=err_env(
            "validation", "shim.empty-text", "tts.validate", "empty text"),
            headers={"X-Correlation-Id": cid})
    try:
        mp3 = await tts_mp3(text)
    except Exception as e:  # noqa: BLE001
        log.error("tts fail cid=%s err=%s", cid, str(e))
        return JSONResponse(status_code=502, content=err_env(
            "tts", "shim.tts-failed", "tts.synth", "edge-tts failed", str(e)),
            headers={"X-Correlation-Id": cid})
    return JSONResponse(content={"audio": b64(mp3)}, headers={"X-Correlation-Id": cid})


@app.get("/v1/alerts")
async def alerts(_auth=Depends(require_device)):
    cid = uuid.uuid4().hex
    # minimal: read OpenClaw cron's pending.json if present; else empty.
    pending = os.path.join(OPENCLAW_WORKSPACE, "alerts", "pending.json")
    if os.path.isfile(pending):
        try:
            with open(pending, encoding="utf-8") as f:
                return JSONResponse(content=json.load(f),
                                    headers={"X-Correlation-Id": cid})
        except (OSError, json.JSONDecodeError):
            pass
    return JSONResponse(content={"alerts": []}, headers={"X-Correlation-Id": cid})


@app.get("/healthz")
async def healthz():
    return JSONResponse(content={"status": "ok"})