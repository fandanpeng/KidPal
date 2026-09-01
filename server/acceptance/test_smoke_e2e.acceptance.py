"""Frozen acceptance tests for TaskPackage 260829-smoke-e2e (subsystem: firmware+server).

Nature: hybrid tracer-bullet smoke acceptance.
  - STRUCTURAL / FILE / EVIDENCE assertions run on the author/build machine with no
    SSH and no device — they are the red baseline's load-bearing teeth ("auto" face).
  - LIVE assertions (class SmokeE2ELiveAcceptance, @pytest.mark.live) SSH into the ECS
    box (8.137.91.16) via /c/tmp/ssh_helper.py; they are frozen but NOT auto-run by the
    merge gate (ARQ-260901-g3-server-live-acceptance discipline).
  - DEVICE assertions are evidence-based: the physical smoke (flash / KEY press / listen
    to speaker / read screen) is manual and cannot be scripted, so the acceptance asserts
    that REAL evidence files exist and their content is合格 — never "no error = pass".

Coverage map (acceptanceRef / exitCriteria -> test method):
  SMK-A1 (structural: device HTTP shim protocol exists)
        -> test_a1_device_http_shim_protocol_exists
  SMK-A2 (structural: 配网指向 shim, 非 OpenClaw 18789)
        -> test_a2_provision_points_to_shim_base_not_openclaw
  SMK-A2 (live: shim 收到设备来源请求 + 访问日志)
        -> test_a2_live_shim_receives_device_request          [live]
  SMK-A3 (structural device leg: 下行 MP3 可解码)
        -> test_a3_mp3_decode_path_exists
  SMK-A3 (live server leg: /v1/asr -> /v1/chat 一次往返)
        -> test_a3_live_shim_asr_then_chat_roundtrip          [live]
  SMK-A3 (device evidence: 一次往返请求/响应证据落盘)
        -> test_a3_device_roundtrip_evidence
  SMK-A4 (屏显一行 evidence)
        -> test_a4_display_line_evidence
  SMK-E1 (structural: 目标板 build/flash 结构 + shim 协议进 build)
        -> test_e1_build_structure
  SMK-E2 (exitCriteria: 烧录日志 + 往返证据落盘, 证据目录结构承接)
        -> test_e2_evidence_directory_structure
  SMK-E3 (exitCriteria: 零业务、只做联通)
        -> test_e3_zero_business_logic
  boundary / error / 权限路径
        -> test_boundary_shim_rejects_empty_text_statically
        -> test_security_no_hardcoded_device_token_in_firmware
        -> test_live_shim_unauthorized_without_token          [live]
        -> test_live_shim_empty_text_rejected                 [live]
  precondition (live)
        -> test_live_shim_healthz_reachable                   [live]

Red baseline (vehicle state, 2026-09-01): server/shim.py is already merged (TP-A proved the
cloud legs), but the DEVICE side does not exist — firmware/main/protocols/ carries only
mqtt/websocket, application.cc instantiates only Mqtt/WebsocketProtocol, the audio pipeline
decodes Opus only (no mp3), and no shim base/token provisioning config exists anywhere in
firmware. The physical smoke evidence dir does not exist yet. So every structural/evidence
test below is RED right now for its own missing-behaviour reason, and MUST NOT turn green
until the builder lands the device-side HTTP shim protocol + mp3 play path + provisioning,
the operator runs the physical smoke, and the evidence files are落盘.

Audio format freeze (taskpackage note ⑤): device uplink WAV PCM16 16kHz mono (faster-whisper
可解), downlink MP3 (edge-tts output). SMK-A3's "TTS 合成" is carried by /v1/chat's internal
TTS leg (contract §1.1: 正文 -> shim 送 TTS -> mp3 -> 设备); the device does NOT separately
call /v1/tts in the smoke path. Assertions pin this semantic.

Config via environment (defaults are the freeze decisions this package pins):
  SMOKE_ARTIFACT_DIR  default <this dir>/_artifacts/smoke-e2e  (where the manual-smoke
                      evidence MUST land; E2 asserts its structure)
  SSH_HELPER          default C:/tmp/ssh_helper.py
  SHIM_BASE           default http://127.0.0.1:8080  (loopback shim on ECS, live tests)
  SHIM_DEVICE_TOKEN   optional; when set, live tests PROVE it gates /v1/* (401 without)
  TTS_VOICE           default zh-CN-XiaoxiaoNeural (reference-audio fixture for live ASR leg)
"""

import base64
import json
import os
import re
import subprocess
import sys
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers; pytest optional — standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
FIRMWARE = REPO_ROOT / "firmware"
FIRMWARE_MAIN = FIRMWARE / "main"
PROTOCOLS_DIR = FIRMWARE_MAIN / "protocols"
SERVER_SHIM = REPO_ROOT / "server" / "shim.py"
BOARD_OVERLAY = FIRMWARE_MAIN / "boards" / "waveshare-s3-rlcd-4.2"

SSH_HELPER = os.environ.get("SSH_HELPER", "C:/tmp/ssh_helper.py")
SHIM_BASE = os.environ.get("SHIM_BASE", "http://127.0.0.1:8080")
SHIM_DEVICE_TOKEN = os.environ.get("SHIM_DEVICE_TOKEN", "")
TTS_VOICE = os.environ.get("TTS_VOICE", "zh-CN-XiaoxiaoNeural")
SMOKE_ARTIFACT_DIR = Path(
    os.environ.get(
        "SMOKE_ARTIFACT_DIR",
        str(Path(__file__).resolve().parent / "_artifacts" / "smoke-e2e"),
    )
)

EVIDENCE_FILES = ["burn.log", "shim.access.log", "roundtrip.json", "screen.md"]
MP3_MAGIC = (b"\xff\xfb", b"\xff\xf3", b"ID3", b"\xff\xf2")
VALID_VIEWS = {"char", "schedule", "story_list", "story_text", "text", "home", "control"}


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _files_under(root: Path, suffix: str):
    if not root.is_dir():
        return []
    return sorted(p for p in root.rglob("*") if p.is_file() and p.suffix == suffix)


def _contains_any(text: str, tokens, ignore_case: bool = True) -> list:
    subject = text.lower() if ignore_case else text
    found = []
    for tok in tokens:
        t = tok.lower() if ignore_case else tok
        if t in subject:
            found.append(tok)
    return found


def _shq(s: str) -> str:
    """Single-quote a string for POSIX shell safety (no subshell/expansion)."""
    return "'" + s.replace("'", "'\\''") + "'"


class SmokeE2EStructuralAcceptance(unittest.TestCase):
    """Machine-runnable assertions. These are the merge-gate teeth + the red baseline."""

    maxDiff = None

    def test_a1_device_http_shim_protocol_exists(self):
        """coverage: SMK-A1 — the firmware carries a device-side HTTP shim protocol."""
        has_shim_file = False
        has_http_file = False
        for name in ("shim", "http"):
            for p in _files_under(PROTOCOLS_DIR, ".cc"):
                if name in p.name.lower():
                    if name == "shim":
                        has_shim_file = True
                    else:
                        has_http_file = True
        self.assertTrue(
            has_shim_file or has_http_file,
            "SMK-A1 RED: firmware/main/protocols/ has no device-side HTTP shim protocol "
            "(only mqtt/websocket). This is the missing firmware leg this package must close.",
        )
        cmake = _read_text(FIRMWARE_MAIN / "CMakeLists.txt")
        wired = ("protocols/shim" in cmake) or ("protocols/http" in cmake)
        self.assertTrue(
            wired,
            "SMK-A1 RED: the device HTTP shim protocol is not referenced in "
            "firmware/main/CMakeLists.txt SOURCES — it is not part of the build yet.",
        )

    def test_a2_provision_points_to_shim_base_not_openclaw(self):
        """coverage: SMK-A2 — 配网指向 shim（复用 server/shim.py），不直连 OpenClaw 18789."""
        blobs = [_read_text(FIRMWARE_MAIN / "ota.cc"),
                 _read_text(FIRMWARE_MAIN / "Kconfig.projbuild"),
                 _read_text(FIRMWARE_MAIN / "CMakeLists.txt")]
        for p in _files_under(PROTOCOLS_DIR, ".cc"):
            blobs.append(_read_text(p))
        for p in _files_under(PROTOCOLS_DIR, ".h"):
            blobs.append(_read_text(p))
        for p in _files_under(FIRMWARE_MAIN / "boards" / "waveshare-s3-rlcd-4.2", ".cc"):
            blobs.append(_read_text(p))
        corpus = "\n".join(blobs)

        shim_tokens = ["shim", "shim_base", "shim_url", "SHIM_BASE", "device_token"]
        found_shim = _contains_any(corpus, shim_tokens)
        self.assertTrue(
            found_shim,
            "SMK-A2 RED: no shim-endpoint provisioning carrier found in firmware "
            "(no `shim` NVS namespace, no CONFIG_SHIM_BASE kconfig, no shim URL literal). "
            "The device is not yet pointed at the shim; it only knows mqtt/websocket.",
        )
        forbidden = _contains_any(corpus, ["18789", "openclaw"])
        self.assertEqual(
            forbidden, [],
            "SMK-A2 RED: firmware references OpenClaw directly "
            f"(tokens found: {forbidden}). The device must reach the shim, not OpenClaw 18789.",
        )

    def test_a3_mp3_decode_path_exists(self):
        """coverage: SMK-A3 (device leg) — 下行 MP3 必须可解码再送喇叭."""
        blobs = [_read_text(FIRMWARE_MAIN / "CMakeLists.txt"),
                 _read_text(FIRMWARE_MAIN / "idf_component.yml")]
        for p in _files_under(FIRMWARE_MAIN / "audio", ".cc"):
            blobs.append(_read_text(p))
        for p in _files_under(FIRMWARE_MAIN / "audio", ".h"):
            blobs.append(_read_text(p))
        corpus = "\n".join(blobs)
        mp3_tokens = ["mp3", "helix", "mp3_decoder", "libhelix", "Mp3Decoder"]
        found = _contains_any(corpus, mp3_tokens)
        self.assertTrue(
            found,
            "SMK-A3 RED: the firmware audio pipeline has no MP3 decode path (Opus-only). "
            "The downlink mp3 from /v1/chat cannot reach the speaker — builder must add an "
            "mp3 play path (intake note ⑤).",
        )
        shim_src = _read_text(SERVER_SHIM)
        self.assertIn(
            "mp3", shim_src.lower(),
            "SMK-A3 RED (seam drift): server/shim.py no longer synthesises mp3 for the "
            "downlink; the frozen downlink format is MP3.",
        )

    def test_e1_build_structure(self):
        """coverage: SMK-E1 — 冒烟固件的 build/flash 结构针对目标硬件 (esp32s3/16MB/8MB PSRAM)."""
        cfg = _read_text(BOARD_OVERLAY / "config.json")
        self.assertIn("esp32s3", cfg,
                      "SMK-E1 RED: board overlay target is not esp32s3.")
        part_dirs = [FIRMWARE / "partitions" / "v1", FIRMWARE / "partitions" / "v2"]
        has_16m = any((d / "16m.csv").is_file() for d in part_dirs if d.is_dir())
        self.assertTrue(has_16m,
                        "SMK-E1 RED: no 16m.csv partition table for the 16MB-flash board.")
        cmake = _read_text(FIRMWARE_MAIN / "CMakeLists.txt")
        wired = ("protocols/shim" in cmake) or ("protocols/http" in cmake)
        self.assertTrue(wired,
                        "SMK-E1 RED: the smoke firmware build does not yet include the "
                        "device HTTP shim protocol (not in CMakeLists.txt SOURCES).")

    def test_e3_zero_business_logic(self):
        """coverage: SMK-E3 — 本包只做联通、零业务（课表/故事/查字显式不在范围）."""
        blobs = [p.read_text(encoding="utf-8", errors="replace")
                 for p in _files_under(PROTOCOLS_DIR, ".cc")]
        blobs += [p.read_text(encoding="utf-8", errors="replace")
                  for p in _files_under(PROTOCOLS_DIR, ".h")]
        corpus = "\n".join(blobs)
        business_tokens = ["story_list", "story_text", "课表", "查字", "笔顺",
                           "故事", '"schedule"', '"char"']
        found = _contains_any(corpus, business_tokens)
        self.assertEqual(
            found, [],
            "SMK-E3 RED: business logic tokens leaked into the smoke protocol layer "
            f"(found: {found}). This package is tracer-bullet connectivity only.",
        )

    def test_e2_evidence_directory_structure(self):
        """coverage: SMK-E2 — 烧录日志 + 一次往返证据落盘，证据目录结构承接 closeout."""
        missing, empty = [], []
        for name in EVIDENCE_FILES:
            p = SMOKE_ARTIFACT_DIR / name
            if not p.is_file():
                missing.append(name)
                continue
            if p.stat().st_size == 0:
                empty.append(name)
        if missing:
            self.fail(f"SMK-E2 RED: missing smoke evidence file(s) {missing} under "
                      f"{SMOKE_ARTIFACT_DIR}. The physical device smoke has not been run "
                      "and its artifacts落盘, or SMOKE_ARTIFACT_DIR is wrong.")
        if empty:
            self.fail(f"SMK-E2 RED: evidence file(s) empty: {empty}.")
        burn = _read_text(SMOKE_ARTIFACT_DIR / "burn.log")
        burn_tokens = _contains_any(burn, ["hash of data verified", "flash", "writing at",
                                           "compressed", "idf.py", "esptool", "leaving"])
        self.assertTrue(burn_tokens,
                        "SMK-E2 RED: burn.log has no flash/serial activity marker.")
        access = _read_text(SMOKE_ARTIFACT_DIR / "shim.access.log")
        routes = _contains_any(access, ["/v1/chat", "/v1/asr", "/v1/tts"])
        self.assertTrue(routes,
                        "SMK-E2 RED: shim.access.log does not record any /v1/* route.")

    def test_a3_device_roundtrip_evidence(self):
        """coverage: SMK-A3 — 一次往返（KEY→麦→/v1/asr→/v1/chat→TTS→喇叭）证据合格."""
        path = SMOKE_ARTIFACT_DIR / "roundtrip.json"
        if not path.is_file():
            self.fail(f"SMK-A3 RED: {path} missing — no one-roundtrip evidence落盘.")
        try:
            data = json.loads(_read_text(path))
        except json.JSONDecodeError as e:
            self.fail(f"SMK-A3 RED: roundtrip.json is not valid JSON: {e}")
        asr_text = (data.get("asr", {}) or {}).get("text", "")
        self.assertTrue(isinstance(asr_text, str) and asr_text.strip(),
                        "SMK-A3 RED: roundtrip.json asr.text is empty — /v1/asr produced no transcript.")
        display = (data.get("chat", {}) or {}).get("display")
        self.assertIsNotNone(display, "SMK-A3 RED: roundtrip.json chat.display missing")
        view = (display or {}).get("view")
        self.assertIn(view, VALID_VIEWS,
                      f"SMK-A3 RED: chat.display.view={view!r} not a contract view.")
        tts_bytes = (data.get("tts", {}) or {}).get("audio_bytes", 0)
        self.assertGreater(tts_bytes, 1024,
                           f"SMK-A3 RED: tts.audio_bytes={tts_bytes} too small (TTS leg not proven).")
        audio_b64 = (data.get("tts", {}) or {}).get("audio_b64")
        if audio_b64:
            raw = base64.b64decode(audio_b64)
            self.assertTrue(raw.startswith(MP3_MAGIC),
                            "SMK-A3 RED: decoded tts.audio_b64 is not a valid mp3 header.")

    def test_a4_display_line_evidence(self):
        """coverage: SMK-A4 — 回复文字或占位串在屏上显示至少一行（evidence）."""
        path = SMOKE_ARTIFACT_DIR / "screen.md"
        if not path.is_file():
            self.fail(f"SMK-A4 RED: {path} missing — no on-screen line evidence.")
        text = _read_text(path).strip()
        self.assertTrue(text, "SMK-A4 RED: screen.md is empty.")
        lines = [ln.strip() for ln in text.splitlines() if ln.strip()]
        self.assertTrue(lines, "SMK-A4 RED: screen.md has no non-empty line.")

    def test_boundary_shim_rejects_empty_text_statically(self):
        """boundary: server/shim.py rejects empty chat text with 400 (static seam-guard)."""
        src = _read_text(SERVER_SHIM)
        self.assertIn("shim.empty-text", src,
                      "boundary RED: server/shim.py lost the empty-text 400 rejection path.")

    def test_security_no_hardcoded_device_token_in_firmware(self):
        """权限/安全轴（secrets-not-in-firmware）: 固件不得硬编码 shim 设备 token."""
        blobs = []
        for p in _files_under(FIRMWARE_MAIN / "protocols", ".cc"):
            blobs.append(_read_text(p))
        for p in _files_under(FIRMWARE_MAIN / "protocols", ".h"):
            blobs.append(_read_text(p))
        blobs.append(_read_text(FIRMWARE_MAIN / "ota.cc"))
        corpus = "\n".join(blobs)
        hardcoded = re.findall(r"token\s*=\s*\"[A-Za-z0-9_\-]{16,}\"", corpus, re.IGNORECASE)
        self.assertEqual(hardcoded, [],
                         f"security RED: firmware hardcodes a device token literal: {hardcoded}.")


class SmokeE2ELiveAcceptance(unittest.TestCase):
    """Frozen live assertions; need SSH to 8.137.91.16 + a running public shim."""

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _ssh(self, cmd: str, timeout: int = 300):
        proc = subprocess.run(
            [sys.executable, SSH_HELPER, cmd],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=timeout,
        )
        return proc.stdout, proc.stderr, proc.returncode

    def _ssh_ok(self, cmd: str, what: str, timeout: int = 300):
        out, err, rc = self._ssh(cmd, timeout=timeout)
        if rc != 0:
            raise AssertionError(f"{what}: remote rc={rc}. stdout={out!r} stderr={err!r}")
        return out, err

    def _remote_write(self, path: str, content: str) -> str:
        """Chunk base64-encode content onto the remote box via repeated appends.

        Each append carries one ~8 KB base64 chunk, keeping every single
        subprocess command line far below the Windows CreateProcess 32767-char
        ceiling (the old single-inline payload triggered WinError 206). Returns a
        shell snippet that decodes the accumulated base64 into `path`; the caller
        runs it as part of its own remote command.
        """
        b64 = base64.b64encode(content.encode("utf-8")).decode("ascii")
        tmp = f"{path}.b64"
        self._ssh_ok(f"rm -f {path} {tmp}", f"chunked write init {path}")
        chunk = 8000
        for i in range(0, len(b64), chunk):
            piece = b64[i:i + chunk]
            self._ssh_ok(f"printf '%s' {_shq(piece)} >> {tmp}",
                         f"chunked write {path} piece {i // chunk}")
        return f"base64 -d < {tmp} > {path}"

    def _curl_json(self, url: str, body: dict) -> tuple:
        parts = ["rm -f /tmp/smoke_resp.json"]
        parts.append(self._remote_write("/tmp/smoke_payload.json",
                                        json.dumps(body, ensure_ascii=False)))
        auth = ""
        if SHIM_DEVICE_TOKEN:
            auth = f" -H 'Authorization: Bearer {SHIM_DEVICE_TOKEN}'"
        curl = (f"curl -s -m 120 -o /tmp/smoke_resp.json -w 'HTTP:%{{http_code}}' "
                f"-X POST -H 'Content-Type: application/json'{auth} -d @/tmp/smoke_payload.json '{url}'")
        parts.append(curl + "; echo; cat /tmp/smoke_resp.json 2>/dev/null; true")
        out, err = self._ssh_ok("; ".join(parts), f"smoke live POST {url}")
        m = re.search(r"HTTP:(\d+)", out)
        code = int(m.group(1)) if m else None
        body = out.split("\n", 1)[1] if "\n" in out else ""
        return code, body

    def test_live_shim_healthz_reachable(self):
        """precondition: shim /healthz returns {status:ok}."""
        out, err, _ = self._ssh(f"curl -s -m 10 '{SHIM_BASE}/healthz'")
        self.assertIn("ok", out, f"live precondition RED: shim /healthz not ok. out={out!r} err={err!r}")

    def test_a2_live_shim_receives_device_request(self):
        """coverage: SMK-A2 — shim 收到设备来源请求，访问日志可查（经 SSH 查 ECS）."""
        out, err = self._ssh_ok(
            "journalctl -u kidpal-shim --since '12 hours ago' 2>/dev/null | "
            "grep -E '/v1/(chat|asr|tts)' | tail -20; true",
            "SMK-A2 live access-log query",
        )
        routes = [ln for ln in out.splitlines() if "/v1/" in ln]
        self.assertTrue(routes,
                        "SMK-A2 live RED: shim access log has no device-sourced /v1/* entry "
                        "in the last 12h (device never hit the shim, or wrong unit/log sink).")

    def test_a3_live_shim_asr_then_chat_roundtrip(self):
        """coverage: SMK-A3 — /v1/asr 转文字 + /v1/chat 到 OpenClaw 拿回复并合成 mp3."""
        ref_text = "今天天气不错，我们一起去公园玩吧。"
        synth = (f"edge-tts --voice {TTS_VOICE} --text '{ref_text}' "
                 "--write-media /tmp/smoke_ref.mp3 >/dev/null 2>&1; "
                 "python3 -c \"import base64;print(base64.b64encode("
                 "open('/tmp/smoke_ref.mp3','rb').read()).decode())\"")
        b64_out, err = self._ssh_ok(synth, "SMK-A3 live reference audio synth")
        audio_b64 = b64_out.strip().splitlines()[-1]
        self.assertGreater(len(audio_b64), 100,
                           "SMK-A3 live RED: reference audio synthesis returned empty base64.")

        code, body = self._curl_json(f"{SHIM_BASE}/v1/asr", {"audio": audio_b64})
        self.assertEqual(code, 200, f"SMK-A3 live /v1/asr HTTP {code} body={body[:400]!r}")
        asr_text = json.loads(body).get("text", "")
        self.assertTrue(asr_text.strip(),
                        f"SMK-A3 live RED: /v1/asr returned empty text. body={body[:400]!r}")

        code, body = self._curl_json(f"{SHIM_BASE}/v1/chat", {"text": asr_text})
        self.assertEqual(code, 200, f"SMK-A3 live /v1/chat HTTP {code} body={body[:400]!r}")
        resp = json.loads(body)
        audio = resp.get("audio", "")
        display = resp.get("display")
        self.assertGreater(len(audio), 100, "SMK-A3 live RED: /v1/chat audio field empty")
        self.assertTrue(base64.b64decode(audio).startswith(MP3_MAGIC),
                        "SMK-A3 live RED: /v1/chat audio is not a valid mp3.")
        if isinstance(display, str):
            display = json.loads(display)
        self.assertIn((display or {}).get("view"), VALID_VIEWS,
                      f"SMK-A3 live RED: display.view invalid. display={display!r}")

    def test_live_shim_empty_text_rejected(self):
        """boundary (live): empty /v1/chat text is rejected 4xx, never silent 200."""
        code, body = self._curl_json(f"{SHIM_BASE}/v1/chat", {"text": ""})
        self.assertIn(code, range(400, 500),
                      f"SMK-A3 boundary RED: empty text returned HTTP {code} ({body[:200]!r}).")

    def test_live_shim_unauthorized_without_token(self):
        """权限 (live): 无 Bearer token 的 /v1/* 请求必须 401（公开面安全轴）."""
        if not SHIM_DEVICE_TOKEN:
            self.skipTest("SHIM_DEVICE_TOKEN not set — shim validation mode; token gate not asserted.")
        out, err, _ = self._ssh(
            "curl -s -m 10 -o /tmp/smoke_resp.json -w 'HTTP:%{http_code}' "
            "-X POST -H 'Content-Type: application/json' -d '{\"text\":\"hi\"}' "
            f"'{SHIM_BASE}/v1/chat'"
        )
        m = re.search(r"HTTP:(\d+)", out)
        self.assertEqual(m and int(m.group(1)), 401,
                         f"security RED: /v1/chat without Bearer token was not rejected 401. raw={out!r}")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(SmokeE2EStructuralAcceptance)
    if os.environ.get("SMOKE_RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(SmokeE2ELiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    sys.exit(0 if result.wasSuccessful() else 1)