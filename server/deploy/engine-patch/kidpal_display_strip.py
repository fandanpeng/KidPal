"""Engine-side ```display fence state machine (pod-events decision `engine-patch-approval`,
event 37469170).  Bind-mounted into the xiaozhi container next to connection.py (方案 A:
engine 为主剥离, front 为校验/relay 权威).

PURE python — NO engine/config imports at import time (this module must be importable
offline by the acceptance, and it is the single-point strip owner so the TTS delta stream and
response_message fulltext share ONE machine — no two-machine drift).

Frozen offline contract (see server/acceptance/test_g_relay_fix.acceptance.py):
    class DisplayFenceStateMachine:
        feed(delta: str) -> str          # speech-safe text to enqueue NOW ("" while
                                         # accumulating inside a fence — 延迟不损)
        finish() -> (tail_speech: str,   #   end-of-stream tail; a still-unclosed fence
                     blocks: list[str])  #   NEVER lands here (宁静默不念 JSON) — its raw
                                         #   body lands in blocks instead; blocks carry ALL
                                         #   raw fence payload strings, in order, WITHOUT the
                                         #   ``` markers (closed and unclosed alike)
    display_blocks_message(blocks) -> dict  # {"type": "kidpal_display_blocks",
                                            #  "blocks": [...raw strings...]} 哑管道零校验
"""

from __future__ import annotations

# The side-channel message type (matches front_app.SIDE_CHANNEL_TYPE).
SIDE_CHANNEL_TYPE = "kidpal_display_blocks"

_OPEN = "```"
_CLOSE = "```"
# Known language tag immediately after the opener (OpenClaw emits ` ```display `); the tag
# and one optional trailing newline are stripped so the block carries the raw payload body.
_TAG = "display"


def display_blocks_message(blocks: "list[str]") -> dict:
    """Side-channel carrier toward the front: raw pass-through, engine is a dumb pipe
    (zero parsing/validation here — the front's display_schema owns the mechanical check)."""
    return {"type": SIDE_CHANNEL_TYPE, "blocks": list(blocks)}


class DisplayFenceStateMachine:
    """Streaming fence holder.  feed() returns only speech-safe text (delay-preserving:
    fence-free deltas flow immediately); while a fence is accumulating inside the podcast
    stream it is withheld.  finish() drains a still-unclosed fence into blocks, never into
    the speech tail (宁静默不念 JSON).
    """

    def __init__(self) -> None:
        self._state = "normal"   # normal | tag | fence
        self._blocks: "list[str]" = []
        self._body: "list[str]" = []
        self._pending = ""       # unresolved suffix carried across feed() calls
        self._tag_partial = ""

    # -- public API -----------------------------------------------------------
    def feed(self, delta: str) -> str:
        """Consume one LLM stream delta; return the speech-safe text to enqueue now."""
        text = self._pending + (delta or "")
        self._pending = ""
        self._tag_partial = ""
        out: "list[str]" = []
        while True:
            idx = text.find(_OPEN)
            if self._state == "normal":
                if idx < 0:
                    # no marker yet — emit now except a possibly-partial backtick opener.
                    keep = self._trailing_backticks(text)
                    if 1 <= keep < len(_OPEN):
                        out.append(text[: -keep])
                        self._pending = text[-keep:]
                        text = ""
                        break
                    out.append(text)
                    text = ""
                    break
                out.append(text[:idx])               # non-fence speech before the opener
                self._state = "tag"                   # skip the language tag next
                text = text[idx + len(_OPEN):]
            elif self._state == "tag":
                # consume the language tag word + one optional newline; body starts after.
                if not text:
                    break                            # tag bytes still streaming — wait
                i = 0
                while i < len(text) and (text[i].isalnum() or text[i] == "_"):
                    i += 1
                text = text[i:]                       # drop the tag (e.g. "display")
                if text.startswith("\n"):
                    text = text[1:]
                self._state = "fence"
            else:  # fence — accumulate until a closer
                if idx < 0:
                    keep = self._trailing_backticks(text)
                    if 1 <= keep < len(_CLOSE):
                        self._body.append(text[: -keep])
                        self._pending = text[-keep:]
                        text = ""
                        break
                    self._body.append(text)
                    text = ""
                    break
                self._body.append(text[:idx])         # raw body up to the closer
                self._blocks.append("".join(self._body))  # close the fence block
                self._body = []
                self._state = "normal"
                text = text[idx + len(_CLOSE):]
        return "".join(out)

    def finish(self) -> "tuple[str, list[str]]":
        """End of stream: return (tail_speech, blocks).  A still-unclosed fence's raw body
        goes to blocks (NEVER read aloud); trailing partial opener backticks are dropped."""
        tail = ""
        if self._state == "fence" and self._pending:
            keep = self._trailing_backticks(self._pending)
            body = self._pending[: -keep] if (1 <= keep < len(_CLOSE)) else self._pending
            self._blocks.append("".join(self._body) + body)
            self._body = []
            self._pending = ""
        elif self._state == "fence" and not self._pending and self._body:
            # unclosed body already parked in self._body (no pending tail)
            self._blocks.append("".join(self._body))
            self._body = []
        self._state = "normal"
        return tail, self._blocks

    # -- helpers ---------------------------------------------------------------
    @staticmethod
    def _trailing_backticks(s: str) -> int:
        n = 0
        for ch in reversed(s):
            if ch == "`":
                n += 1
            else:
                break
        return n