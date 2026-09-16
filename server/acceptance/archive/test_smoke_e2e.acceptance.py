"""Frozen acceptance tests for TaskPackage 260829-smoke-e2e (subsystem: firmware+server).

SUPERSEDED (2026-09-05, acceptance-author): superseded-by = 260903-device-ws-switch
(firmware/main/test/device_ws_switch.acceptance.c, coverage WSA-A7). Reason = spec
docs/spec.md §3.0.0「迁移与既有资产」台阶退役条款 — 方向 B（WS+Opus 流式）落地后，
Pod S 同步 HTTP 台阶（四端点 + WAV 上行/MP3 下行）退役；其安全发现（401 门纪律、
设备 token 纪律）沿用。断言语义零变更、测试 id 与 coverage map 不动，仅本归属状态
注记为新增内容。

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

SUPERSEDED-DISPOSAL (2026-09-09 合并链定向处置批, acceptance-author, human 授权
pod-events eventId=2dd4498d-68db-448f-ac72-5e90d1cad0ad): 方向 A（设备 HTTP shim）
架构已由 260903-device-ws-switch 退役取代（spec docs/spec.md §3.0.0「迁移与既有资产」
台阶退役条款 + §7 决策 9/18; 归档记录 delivery/changes/dev/260903-device-ws-switch/
shim-archive-record.md）。据此：架构过时齿（SMK-A1/A2/A3-mp3/E1）标 skip；
证据齿中 SMK-A3-roundtrip / SMK-E2 因 roundtrip.json 从未落盘（设备完整往返未发生
即被方向 B 取代, 不可回补）标 skip；SMK-A4 由回收的 screen.md 实证据承载（不 skip）。
被标 skip 的测试断言本体逐字保留于 skipTest 之后（不可达），测试 id 与 coverage map
不动，供未来重新启用判定。逐字引用见模块常量 _SUPERSEDED_DISPOSAL 注释块。

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

# --------------------------------------------------------------------------- #
# SUPERSEDED 处置常量（2026-09-09 合并链定向处置批, human 授权, pod-events
# eventId=2dd4498d-68db-448f-ac72-5e90d1cad0ad）。
#
# 退役依据（逐字引用）:
#   - spec docs/spec.md §3.0.0「迁移与既有资产」台阶退役条款: 「Pod S
#     （260829-smoke-e2e）的同步 HTTP 链路（四端点 + WAV 上行/MP3 下行）为架构验证
#     台阶（live 5/5 实证），方向 B 落地后退役；其安全发现（401 门穿透与修复、
#     设备 token 纪律）沿用。」
#   - spec docs/spec.md §7 决策 4 注记: 「⚠️ 已被决策 9/18 取代：方向 B 转官方
#     小智服务端网关，HTTP shim 退役为已验证台阶（spec §3.0.0「迁移与既有资产」）。」
#     （决策 9 = 端云通信 WebSocket + Opus 流式（方向 B）；决策 18 = 服务端实现
#     形态 = 官方 Python 版小智服务端网关。）
#   - 归档记录: delivery/changes/dev/260903-device-ws-switch/shim-archive-record.md
#     （「处置：归档（archived）——文件保留在原路径，头部加 ARCHIVED 墓碑注记；
#     代码逻辑零改动」。）
#
# 被 skip 的测试: 断言本体逐字保留于 skipTest 调用之后（运行不可达），零删除；
# 测试 id 与 coverage map 不动；重新启用判定材料 = 本注释块 + 上列三处引用。
# --------------------------------------------------------------------------- #
_SUPERSEDED_DISPOSAL = (
    "SUPERSEDED (2026-09-09 合并链定向处置, human 授权 eventId="
    "2dd4498d-68db-448f-ac72-5e90d1cad0ad): 方向 A（设备 HTTP shim）架构已由 "
    "260903-device-ws-switch 退役取代 — spec docs/spec.md §3.0.0「迁移与既有资产」"
    "台阶退役条款 + §7 决策 9/18（「已被决策 9/18 取代：方向 B 转官方小智服务端网关，"
    "HTTP shim 退役为已验证台阶」）; 归档记录 delivery/changes/dev/260903-device-ws-switch/"
    "shim-archive-record.md。断言本体保留于 skip 之后（不可达），供未来重新启用判定。"
)


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
        """coverage: SMK-A1 — the firmware carries a device-side HTTP shim protocol.

        SKIP (2026-09-09 合并链定向处置): 方向 A 架构齿。设备唯一公网入口已收敛为
        网关 wss（决策 18 + OQ-R4），设备侧 HTTP shim protocol 不再进 build 是
        方向 B 的正确形态而非缺失。
        """
        self.skipTest(
            _SUPERSEDED_DISPOSAL
            + " SMK-A1 特注: 设备侧 HTTP shim protocol 已不存在且不再进 build，"
              "属方向 B 正确形态（设备唯一公网入口 = 网关 wss, 决策 18）。"
        )
        # ---- 断言本体（保留原文, skip 后不可达） ----
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
        """coverage: SMK-A2 — 配网指向 shim（复用 server/shim.py），不直连 OpenClaw 18789.

        SKIP (2026-09-09 合并链定向处置): 方向 A 架构齿。红齿 = 固件 corpus 无
        shim token（配网不再指向 HTTP shim base）；forbidden 齿（18789/openclaw
        直连检查）现行通过。设备配网已改为 WS 命名空间直写 NVS（spec §9 决策 23:
        「配网直写 NVS（websocket 命名空间 url/token/version）+ 短路激活」）。
        """
        self.skipTest(
            _SUPERSEDED_DISPOSAL
            + " SMK-A2 特注: 配网不再指向 HTTP shim base（设备配网 = websocket "
              "命名空间直写 NVS, spec §9 决策 23）; forbidden 齿（18789/openclaw）"
              "现行通过。"
        )
        # ---- 断言本体（保留原文, skip 后不可达） ----
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
        """coverage: SMK-A3 (device leg) — 下行 MP3 必须可解码再送喇叭.

        SKIP (2026-09-09 合并链定向处置): 方向 A 架构齿。方向 B 下行 = WS+Opus
        流式（决策 9），设备音频管线无 MP3 解码路径是正确形态而非缺失。
        seam 齿核实记录: _read_text(SERVER_SHIM) 对归档 shim.py 的现行为 = 文件
        仍原路径存在（shim-archive-record.md: 归档 = 原路径保留 + ARCHIVED 墓碑
        注记、代码逻辑零改动），read 正常、断言现通过——但该通过属空转脆性依赖
        （shim.py 一旦迁移即读空失败），随本 skip 一并解除。
        """
        self.skipTest(
            _SUPERSEDED_DISPOSAL
            + " SMK-A3-mp3 特注: 设备下行 = WS+Opus 流式（决策 9），MP3 解码路径"
              "不存在是方向 B 正确形态; shim.py seam 齿因归档文件原路径保留而空转"
              "通过，随 skip 解除该脆性依赖。"
        )
        # ---- 断言本体（保留原文, skip 后不可达） ----
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
        """coverage: SMK-E1 — 冒烟固件的 build/flash 结构针对目标硬件 (esp32s3/16MB/8MB PSRAM).

        SKIP (2026-09-09 合并链定向处置): 仅 CMakeLists 齿（protocols/shim|http）
        红，属方向 A 架构齿; esp32s3 板目标齿与 16m.csv 分区齿现行通过（本次核实）。
        """
        self.skipTest(
            _SUPERSEDED_DISPOSAL
            + " SMK-E1 特注: 红齿仅 CMakeLists 不含 protocols/shim|http（方向 B "
              "正确形态）; esp32s3/16m.csv 两齿现行通过。"
        )
        # ---- 断言本体（保留原文, skip 后不可达） ----
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
        """coverage: SMK-E2 — 烧录日志 + 一次往返证据落盘，证据目录结构承接 closeout.

        SKIP (2026-09-09 合并链定向处置): 四件证据中 burn.log / shim.access.log /
        screen.md 为 2026-09-02 真实冒烟产物（处置清单: 由 builder 自 pod-smoke-e2e
        worktree 回收至 SMOKE_ARTIFACT_DIR），但 roundtrip.json 从未落盘（不可回补,
        同 SMK-A3-roundtrip 特注）——断言原子性要求四件齐全，故整体 skip。
        """
        self.skipTest(
            _SUPERSEDED_DISPOSAL
            + " SMK-E2 特注: burn.log/shim.access.log/screen.md 为 2026-09-02 "
              "真实冒烟产物（builder 回收至 SMOKE_ARTIFACT_DIR, 处置清单在案），"
              "roundtrip.json 从未落盘不可回补，断言原子性要求四件齐全故整体 skip。"
        )
        # ---- 断言本体（保留原文, skip 后不可达） ----
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
        """coverage: SMK-A3 — 一次往返（KEY→麦→/v1/asr→/v1/chat→TTS→喇叭）证据合格.

        SKIP (2026-09-09 合并链定向处置): 证据不可回补齿。roundtrip.json 从未
        落盘——2026-09-02 物理冒烟（pod-smoke-e2e worktree _artifacts/smoke-e2e）
        中 ASR 对静音输入返回空文本、设备侧按冻结逻辑短路，完整设备往返未发生即被
        方向 B 取代（screen.md 屏显记录在案）。方向 B 的设备语音回环证据由
        260829-voice-roundtrip（roundtrip-live-evidence.md）与 260903-device-ws-switch
        承载。
        """
        self.skipTest(
            _SUPERSEDED_DISPOSAL
            + " SMK-A3-roundtrip 特注: roundtrip.json 从未落盘（2026-09-02 ASR 空"
              "文本短路, 完整设备往返未发生即被方向 B 取代），证据不可回补; 方向 B "
              "回环证据由 260829-voice-roundtrip / 260903-device-ws-switch 承载。"
        )
        # ---- 断言本体（保留原文, skip 后不可达） ----
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