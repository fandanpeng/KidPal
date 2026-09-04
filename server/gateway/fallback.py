"""Gateway-side fallback generation (spec §4.0 / decision 28 / V32 / GWA-A7).

Frozen acceptance GWA-A7. Pure-python, no fastapi at import time.

Frozen offline contract:
    SOOTHING_TIMEOUT_MS == 4000         (LLM 首 token 超 4 秒先推安抚句)
    SOOTHING_FALLBACK                   (4s 安抚句, non-empty)
    ERROR_FALLBACK == "网络出小差了，等一会儿再试试吧"  (+ text view end-turn)
    MAX_SPEECH_SENTENCES == 3           (decision 28「3 句以内」生成端软约束)
    truncate_for_display(text, max_sentences=MAX_SPEECH_SENTENCES) -> <=N-句 snippet (V32)

All three fallback behaviors live here (spec:263 / spec:266 / spec:445 / V32):
  - 4s 安抚句定时器: LLM 首 token 超 4 秒网关先推固定安抚句走 TTS (不冷场).
  - 云端任一环节失败: 固定错误兜底句 + text 视图结束当轮 (不曝技术错误).
  - 答案超 3 句: 按有处理的失败模式截断/分屏, 绝不把全文一次性送下去.
"""

from __future__ import annotations

import json  # noqa: F401  (kept for structured-log envelope docs / future use)
import logging
import re
import time
import uuid
from typing import Optional

log = logging.getLogger("kidpal-gateway.fallback")

# spec:263 —— LLM 首 token 超 4 秒先开口安抚, 不冷场.
SOOTHING_TIMEOUT_MS = 4000

# 4s 安抚句 (non-empty): fixed, kid-friendly.
SOOTHING_FALLBACK = "我在认真想呢，你先听我说，马上就好啦。"

# spec:266 —— 云端任一环节失败 = 固定友好句 + text 视图结束当轮, 不曝技术错误.
ERROR_FALLBACK = "网络出小差了，等一会儿再试试吧"

# decision 28 / V32 —— 生成端「3 句以内」软约束失效时按有处理失败模式截断分屏.
MAX_SPEECH_SENTENCES = 3

# sentence delimiters for V32 truncation (。。！？…) — 中文句末标点.
_SENTENCE_BOUNDARY = re.compile(r"[^。！？!?]*[。！？!?]")


def corr_id() -> str:
    """A Correlation-Id for the request path (K6 structured logging)."""
    return uuid.uuid4().hex


def _log_event(cid: str, event: str, **fields) -> None:
    """Structured log event carrying a Correlation-Id (K6)."""
    row = {"cid": cid, "event": event, "ts_ms": int(time.time() * 1000)}
    row.update(fields)
    log.info(json.dumps(row, ensure_ascii=False, default=str))


def truncate_for_display(text: str, max_sentences: int = MAX_SPEECH_SENTENCES) -> str:
    """V32: clip a speech answer to <= max_sentences full sentences.

    Over-long LLM output (生成端软约束失效) is degraded to a managed failure mode:
    a clipped N-句 snippet + the board shows the split-screen "……" continuation hint.
    Returns a str. If no sentence delimiter is found, falls back to a char cap so the
    snippet never ships the whole oversized blob.
    """
    text = (text or "").strip()
    if not text:
        return ""
    sentences = _SENTENCE_BOUNDARY.findall(text)
    if not sentences:
        return text[: max_sentences * 40].strip()
    clipped = "".join(sentences[:max_sentences]).strip()
    return clipped


def soothing_fallback(cid: Optional[str] = None) -> tuple[str, bool]:
    """Compute the 4s 安抚句 payload. Returns (text, along_with_ok_flags).

    Gateway 定时器语义: LLM 首 token 超过 SOOTHING_TIMEOUT_MS 未到, 网关先推这句 TTS
    (不冷场, spec:263). cid carried for structured logging.
    """
    _ = cid
    return SOOTHING_FALLBACK, True


def error_fallback_text_view(cid: Optional[str] = None) -> dict:
    """错误兜底文本 + text 视图结束当轮 (spec:266).

    Returns the E1 display payload for the board: fixed friendly sentence shown as a
    text view, ending the current turn so the client doesn't keep waiting.
    """
    cid = cid or corr_id()
    _log_event(cid, "error_fallback", message=ERROR_FALLBACK)
    return {"view": "text", "data": {"text": ERROR_FALLBACK}}