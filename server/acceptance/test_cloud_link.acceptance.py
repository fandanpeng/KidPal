"""Frozen acceptance tests for TaskPackage 260831-cloud-link-bringup (subsystem: server).

SUPERSEDED (2026-09-05, acceptance-author): superseded-by = 260903-device-ws-switch
(firmware/main/test/device_ws_switch.acceptance.c, coverage WSA-A7). Reason = spec
docs/spec.md §3.0.0「迁移与既有资产」台阶退役条款 — 方向 B（WS+Opus 流式）落地后，
HTTP 台阶（shim 四端点）退役。断言语义零变更、测试 id 与 coverage map 不动，仅本
归属状态注记为新增内容。

Nature: LIVE production-ECS acceptance. These tests run on the author/build machine,
SSH into the KidPal OpenClaw box (8.137.91.16) via /c/tmp/ssh_helper.py, and assert
REAL remote results — HTTP status codes, JSON parseability, non-empty audio, display-block
`view` values. Passing "because nothing errored" is explicitly forbidden: every acceptance
below must hold a concrete real-side-effect assertion.

Coverage map (acceptanceRef -> test method):
  CLB-A2  (OC-5)  -> test_chat_completions_endpoint_open
  CLB-A3  (OC-6)  -> test_display_block_format_compliance
  CLB-A4  (OC-10) -> test_ecs_compute_profile_gives_whisper_tier
  CLB-A5  (OC-8)  -> test_faster_whisper_chinese_transcription
  CLB-A6  (OC-9)  -> test_edge_tts_generates_playable_mp3
  CLB-A7  (OC-7)  -> test_shim_chat_full_path_returns_audio_and_display
  CLB-A8  (OC-11) -> test_openclaw_consumes_schedule_md
  boundary       -> test_shim_chat_empty_text_rejected
  precondition   -> test_ssh_reachable

Error / boundary assertions (acceptance-evidence.md "happy-path only" guard):
  - CLB-A2 hard-fails while /v1/chat/completions is still 404 (endpoint-not-enabled = red).
  - CLB-A3 downgrades the score when any display block is absent or its JSON unparseable;
    both display-block occurrence rate and in-block parse rate must be >= 0.9, and every
    failing sample is persisted to disk (ARTIFACT_DIR/display_failures).
  - CLB-A6 / CLB-A7 hard-fail on empty audio (short file or empty audio field).
  - boundary test asserts empty-text chat request is rejected, not silently 200-d.

Red baseline (vehicle state, 2026-08-31): server/ shim does not exist; OpenAI endpoint 404s;
faster-whisper and edge-tts are not installed; system Python is 3.6.8 (whisper needs >=3.8);
kidpal workspace holds only chardata/ (no schedule.md). Every story test is therefore RED
right now for its own missing-behaviour reason. This module must NOT turn green until the
builder's shim + ASR + TTS + endpoint are live and asserted here.

Config via environment (defaults are the freeze decisions this package pins):
  OPENCLAW_BASE      default http://127.0.0.1:18789   (loopback OpenClaw gateway)
  OPENCLAW_TOKEN     optional; Bearer token added to Authorization header when set
  OPENCLAW_MODEL     optional; added to payload when set (OpenClaw agent designator)
  SHIM_BASE          default http://127.0.0.1:8080    (builder must bind shim here)
  OPENCLAW_WORKSPACE default /root/.openclaw/workspace/kidpal  (where schedule.md lives)
  CLB_WHISPER_PYTHON default python3  (>=3.8 venv interpreter that imports faster_whisper)
  CLB_WHISPER_MODEL  default base      (tiny|base|small; tier is derived in CLB-A4)
  CLB_ARTIFACT_DIR   default <this dir>/_artifacts
"""

import base64
import datetime
import json
import os
import re
import subprocess
import sys
import unittest

try:
    import pytest
    pytestmark = pytest.mark.live  # live on-target：需 SSH 到 ECS，merge/CI 环境不可 auto-run
except ImportError:
    pytestmark = None  # 无 pytest 环境下 standalone unittest 直跑不受影响

from pathlib import Path

# --------------------------------------------------------------------------- #
# Freeze-decisions & config
# --------------------------------------------------------------------------- #
SSH_HELPER = os.environ.get("SSH_HELPER", "C:/tmp/ssh_helper.py")
OPENCLAW_BASE = os.environ.get("OPENCLAW_BASE", "http://127.0.0.1:18789")
OPENCLAW_TOKEN = os.environ.get("OPENCLAW_TOKEN", "")
OPENCLAW_MODEL = os.environ.get("OPENCLAW_MODEL", "")
OPENCLAW_WORKSPACE = os.environ.get("OPENCLAW_WORKSPACE", "/root/.openclaw/workspace/kidpal")
SHIM_BASE = os.environ.get("SHIM_BASE", "http://127.0.0.1:8080")
CLB_WHISPER_PYTHON = os.environ.get("CLB_WHISPER_PYTHON", "python3")
CLB_WHISPER_MODEL = os.environ.get("CLB_WHISPER_MODEL", "base")
ARTIFACT_DIR = Path(
    os.environ.get("CLB_ARTIFACT_DIR", str(Path(__file__).resolve().parent / "_artifacts"))
)

# Contract §1.6 system prompt — must be sent verbatim so DeepSeek always emits ```display.
SYSTEM_PROMPT = (
    "你是儿童学习伙伴。每次回复分两部分：先用口语化、适合8-12岁孩子的话回答"
    "（这部分会被念出来）；然后在末尾附一个 ```display 代码块，内含屏幕显示指令 JSON，"
    "view 取 char/schedule/story_list/story_text/text/home/control 之一。"
)

VALID_VIEWS = {"char", "schedule", "story_list", "story_text", "text", "home", "control"}

# ```display fenced block: content between the ```display line and closing ```.
DISPLAY_RE = re.compile(r"```\s*display\s*\n(.*?)```", re.DOTALL)

# --------------------------------------------------------------------------- #
# SSH / remote helpers
# --------------------------------------------------------------------------- #
def ssh(cmd: str, timeout: int = 300):
    """Run a command on the ECS via ssh_helper.py. Returns (stdout, stderr, rc)."""
    proc = subprocess.run(
        [sys.executable, SSH_HELPER, cmd],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout,
    )
    return proc.stdout, proc.stderr, proc.returncode


def ssh_assert_ok(cmd: str, what: str, timeout: int = 300):
    """Run a remote command and fail with a structured message on any transport/RC problem."""
    out, err, rc = ssh(cmd, timeout=timeout)
    if rc != 0:
        raise AssertionError(
            f"{what}: remote command exited rc={rc}. stdout={out!r} stderr={err!r}"
        )
    return out, err


def remote_write(path: str, content: str) -> str:
    """Return a bash fragment that writes `content` to `path` on the ECS (base64-safe)."""
    b64 = base64.b64encode(content.encode("utf-8")).decode("ascii")
    return f"echo {b64} | base64 -d > {path}"


def run_curl(url: str, body=None, method: str = "POST", timeout: int = 90) -> str:
    """Build the bash command that POSTs `body` (dict) to `url` and echoes 'HTTP:<code>' + body."""
    parts = ["rm -f /tmp/clb_resp.json"]
    if body is not None:
        parts.append(remote_write("/tmp/clb_payload.json", json.dumps(body, ensure_ascii=False)))
    curl = f"curl -s -m {timeout} -o /tmp/clb_resp.json -w 'HTTP:%{{http_code}}'"
    if method == "POST":
        curl += " -X POST -H 'Content-Type: application/json'"
        if OPENCLAW_TOKEN and "18789" in url:
            curl += f" -H 'Authorization: Bearer {OPENCLAW_TOKEN}'"
        if body is not None:
            curl += " -d @/tmp/clb_payload.json"
    curl += f" '{url}'"
    parts.append(curl + "; echo; cat /tmp/clb_resp.json 2>/dev/null; true")
    return "; ".join(parts)


def parse_curl_stdout(out: str):
    """Split 'HTTP:<code>\\n<body>' into (code, body)."""
    m = re.search(r"HTTP:(\d+)", out)
    code = int(m.group(1)) if m else None
    body = out.split("\n", 1)[1] if "\n" in out else ""
    return code, body


def openai_payload(messages):
    payload = {"messages": messages}
    if OPENCLAW_MODEL:
        payload["model"] = OPENCLAW_MODEL
    return payload


def extract_display_block(text: str):
    m = DISPLAY_RE.search(text or "")
    if not m:
        return None
    return m.group(1).strip()


def now_stamp() -> str:
    return datetime.datetime.now().strftime("%Y%m%d-%H%M%S")


# --------------------------------------------------------------------------- #
# Tests
# --------------------------------------------------------------------------- #
class CloudLinkAcceptance(unittest.TestCase):
    """Each method maps 1:1 to a CLB acceptance or the boundary/precondition guard."""

    maxDiff = None

    # ---- precondition ------------------------------------------------------ #
    def test_ssh_reachable(self):
        """Environment precondition: ECS must be reachable, else every ruling below is void."""
        out, err, rc = ssh("echo ok")
        if "ok" not in out:
            self.fail(
                f"SSH precondition failed (not a story red): rc={rc} out={out!r} err={err!r}. "
                "Fix ssh_helper.py / credentials before judging this package."
            )

    # ---- CLB-A2 (OC-5) ----------------------------------------------------- #
    def test_chat_completions_endpoint_open(self):
        """coverage: CLB-A2(OC-5) — /v1/chat/completions returns a normal completion."""
        url = f"{OPENCLAW_BASE}/v1/chat/completions"
        cmd = run_curl(url, body=openai_payload([{"role": "user", "content": "你好"}]))
        out, err = ssh_assert_ok(cmd, "CLB-A2 chat/completions probe")
        code, body = parse_curl_stdout(out)

        self.assertIsNotNone(code, f"CLB-A2 no HTTP code captured. raw={out!r} err={err!r}")
        self.assertNotEqual(
            code, 404,
            "CLB-A2 RED: OpenAI-compatible endpoint is still 404 (not enabled). "
            "This is the missing behaviour this package must close.",
        )
        self.assertEqual(code, 200, f"CLB-A2 expected 200, got {code} body={body[:500]!r}")

        resp = json.loads(body)
        choices = resp.get("choices")
        self.assertTrue(choices, f"CLB-A2 got no choices[] body={body[:500]!r}")
        content = choices[0].get("message", {}).get("content")
        self.assertIsInstance(content, str, "CLB-A2 choices[0].message.content is not a string")
        self.assertTrue(content.strip(), "CLB-A2 choices[0].message.content is empty")

    # ---- CLB-A3 (OC-6) ----------------------------------------------------- #
    def test_display_block_format_compliance(self):
        """coverage: CLB-A3(OC-6) — DeepSeek ```display occurrence + in-block JSON parse >= 0.9."""
        queries = [
            ("char", "勇字怎么写"),
            ("char", "『明』这个字怎么写"),
            ("char", "『家』字怎么写"),
            ("schedule", "我今天下午有什么课"),
            ("schedule", "明天有什么课"),
            ("schedule", "这周有哪些课"),
            ("story_list", "我讲过的故事有哪些"),
            ("story_list", "我的故事列表"),
            ("story_text", "给我读一读上次那个冒险故事"),
            ("text", "现在几点了"),
            ("text", "今天天气怎么样"),
            ("text", "你叫什么名字"),
        ]
        fails = []
        samples = []
        n_blocks = 0
        n_parsed = 0

        for label, q in queries:
            url = f"{OPENCLAW_BASE}/v1/chat/completions"
            messages = [
                {"role": "system", "content": SYSTEM_PROMPT},
                {"role": "user", "content": q},
            ]
            cmd = run_curl(url, body=openai_payload(messages))
            out, err = ssh_assert_ok(cmd, f"CLB-A3 query {label!r}")
            code, body = parse_curl_stdout(out)
            self.assertEqual(code, 200, f"CLB-A3 query {label!r} HTTP {code} body={body[:300]!r}")

            try:
                resp = json.loads(body)
                content = resp["choices"][0]["message"]["content"]
            except (KeyError, IndexError, json.JSONDecodeError) as e:
                fails.append((label, q, body, f"no choices[].message.content: {e}"))
                samples.append({"label": label, "query": q, "ok": False,
                                "reason": "bad completion schema", "raw": body})
                continue

            block = extract_display_block(content)
            sample = {"label": label, "query": q, "ok": True, "content": content}
            if block is None:
                fails.append((label, q, content, "no ```display block"))
                sample["ok"] = False
                sample["reason"] = "no display block"
            else:
                n_blocks += 1
                try:
                    parsed = json.loads(block)
                    view = parsed.get("view")
                    if view in VALID_VIEWS:
                        n_parsed += 1
                        sample["view"] = view
                    else:
                        fails.append((label, q, content, f"invalid view {view!r}"))
                        sample["ok"] = False
                        sample["reason"] = f"invalid view {view!r}"
                        sample["block"] = block
                except json.JSONDecodeError as e:
                    fails.append((label, q, content, f"unparseable display JSON: {e}"))
                    sample["ok"] = False
                    sample["reason"] = f"unparseable JSON: {e}"
                    sample["block"] = block
            samples.append(sample)

        # Persist failure samples (per-sample drop) + full summary for CLB-E4 report.
        fail_dir = ARTIFACT_DIR / "display_failures"
        fail_dir.mkdir(parents=True, exist_ok=True)
        ts = now_stamp()
        for i, (label, q, raw, why) in enumerate(fails):
            (fail_dir / f"{ts}_{i:02d}_{label}.md").write_text(
                f"# display failure sample\n\nlabel={label}\nquery={q}\nreason={why}\n\n"
                f"```\n{raw}\n```\n",
                encoding="utf-8",
            )
        summary = {"ts": ts, "total": len(queries), "n_blocks": n_blocks, "n_parsed": n_parsed,
                   "occurrence_rate": n_blocks / len(queries),
                   "parse_rate": n_parsed / max(n_blocks, 1),
                   "failures": len(fails), "samples": samples}
        (ARTIFACT_DIR / f"display_compliance_{ts}.json").write_text(
            json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8"
        )

        occ_rate = n_blocks / len(queries)
        parse_rate = n_parsed / max(n_blocks, 1)
        self.assertGreaterEqual(
            occ_rate, 0.9,
            f"CLB-A3 display-block occurrence {occ_rate:.2f} < 0.9 ({n_blocks}/{len(queries)}). "
            f"Failing samples dropped to {fail_dir}.",
        )
        self.assertGreaterEqual(
            parse_rate, 0.9,
            f"CLB-A3 in-block JSON parse {parse_rate:.2f} < 0.9 ({n_parsed}/{n_blocks}). "
            f"Failing samples dropped to {fail_dir}.",
        )

    # ---- CLB-A4 (OC-10) ---------------------------------------------------- #
    def test_ecs_compute_profile_gives_whisper_tier(self):
        """coverage: CLB-A4(OC-10) — ECS CPU/mem occupancy -> faster-whisper model tier."""
        cmd = (
            "nproc; "
            "free -m | awk 'NR==2{print $2, $3, $7}'; "
            "grep -m1 'model name' /proc/cpuinfo; "
            "uptime"
        )
        out, err = ssh_assert_ok(cmd, "CLB-A4 resource probe")
        lines = [ln.strip() for ln in out.splitlines() if ln.strip()]

        cores = int(lines[0])
        total_mb, used_mb, avail_mb = (int(x) for x in lines[1].split())
        cpu_model = lines[2].split(":", 1)[1].strip() if len(lines) > 2 and ":" in lines[2] else "unknown"
        load = lines[3] if len(lines) > 3 else ""

        # Real assertions: sane ECS, and it must at least carry the tiny tier or the cloud plan
        # is infeasible on this box (a genuine "不可行" conclusion, not a vacuous pass).
        self.assertGreaterEqual(cores, 1, "CLB-A4 cores must be >= 1")
        self.assertGreaterEqual(total_mb, 1024, "CLB-A4 total RAM < 1GB -> faster-whisper tiny infeasible")

        # Documented heuristic for model tier (faster-whisper CPU rough needs).
        if total_mb < 2048 or cores < 2:
            tier = "tiny"
        elif total_mb < 4096 or cores < 4:
            tier = "base"
        else:
            tier = "small"
        self.assertIn(tier, {"tiny", "base", "small"})

        ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
        (ARTIFACT_DIR / f"ecs_compute_profile_{now_stamp()}.json").write_text(
            json.dumps({"cores": cores, "total_mb": total_mb, "used_mb": used_mb,
                        "avail_mb": avail_mb, "cpu_model": cpu_model, "load": load,
                        "recommended_whisper_tier": tier}, ensure_ascii=False, indent=2),
            encoding="utf-8",
        )

    # ---- CLB-A5 (OC-8) ----------------------------------------------------- #
    def test_faster_whisper_chinese_transcription(self):
        """coverage: CLB-A5(OC-8) — faster-whisper transcribes a known Chinese sentence + latency."""
        check = f"{CLB_WHISPER_PYTHON} -c 'import faster_whisper; print(\"ok\")'"
        out, err, rc = ssh(check)
        if "ok" not in out:
            self.fail(
                "CLB-A5 RED: faster-whisper not importable. "
                f"probe={check!r} out={out!r} err={err!r}. "
                "System python is 3.6.8 (<3.8); install a >=3.8 venv and point "
                "CLB_WHISPER_PYTHON at it."
            )

        # Fixture: synthesise a known Chinese sentence with edge-tts, then transcribe it back.
        ref_text = "今天天气真不错，我们一起去公园玩吧。"
        tts_cmd = (
            f"edge-tts --voice zh-CN-XiaoxiaoNeural --text '{ref_text}' "
            "--write-media /tmp/clb_asr_ref.mp3 >/dev/null 2>&1; "
            "stat -c %s /tmp/clb_asr_ref.mp3"
        )
        size_out, size_err = ssh_assert_ok(tts_cmd, "CLB-A5 reference audio synth (edge-tts)")
        self.assertGreater(int(size_out.strip()), 256, "CLB-A5 reference audio empty -> cannot test ASR")

        script = (
            "import sys, time, json\n"
            "from faster_whisper import WhisperModel\n"
            "t0 = time.time()\n"
            "model = WhisperModel(sys.argv[1], device='cpu', compute_type='int8')\n"
            "segments, info = model.transcribe(sys.argv[2], language='zh', beam_size=1, initial_prompt='以下是普通话的句子。')\n"
            "text = ''.join(s.text for s in segments)\n"
            "latency = time.time() - t0\n"
            "print(json.dumps({'text': text, 'latency': latency}, ensure_ascii=False))\n"
        )
        wh_cmd = (
            remote_write("/tmp/clb_whisper.py", script) + "; "
            f"{CLB_WHISPER_PYTHON} /tmp/clb_whisper.py {CLB_WHISPER_MODEL} /tmp/clb_asr_ref.mp3"
        )
        out, err = ssh_assert_ok(wh_cmd, "CLB-A5 whisper transcription", timeout=600)
        result = json.loads(out.strip().splitlines()[-1])

        transcribed = result["text"].replace(" ", "").replace(",", "").replace("，", "")
        latency = result["latency"]

        ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
        (ARTIFACT_DIR / f"whisper_transcription_{now_stamp()}.json").write_text(
            json.dumps({"model": CLB_WHISPER_MODEL, "reference": ref_text,
                        "transcription": result["text"], "latency_s": latency},
                       ensure_ascii=False, indent=2),
            encoding="utf-8",
        )

        # Distinctive token must survive: "公园" is the load-bearing content word.
        self.assertIn("公园", transcribed,
                      f"CLB-A5 transcription wrong: got {result['text']!r}, expected to contain 公园")
        self.assertGreater(latency, 0.0, "CLB-A5 latency must be recorded (>0)")
        self.assertLess(latency, 120.0, "CLB-A5 latency implausibly high; check model/freeze")

    # ---- CLB-A6 (OC-9) ----------------------------------------------------- #
    def test_edge_tts_generates_playable_mp3(self):
        """coverage: CLB-A6(OC-9) — edge-tts produces a non-empty, playable Chinese mp3."""
        probe = "which edge-tts"
        out, err, rc = ssh(probe)
        if not out.strip():
            self.fail(
                f"CLB-A6 RED: edge-tts not installed. which={out!r} err={err!r}. "
                "Install edge-tts (>=3.8 venv) to close OC-9."
            )

        tts_cmd = (
            "edge-tts --voice zh-CN-XiaoxiaoNeural --text '今天天气真好，我们去公园玩。' "
            "--write-media /tmp/clb_tts.mp3 >/dev/null 2>&1; "
            "stat -c %s /tmp/clb_tts.mp3; echo; xxd -l 3 -p /tmp/clb_tts.mp3"
        )
        out, err = ssh_assert_ok(tts_cmd, "CLB-A6 edge-tts synth")
        lines = [ln.strip() for ln in out.splitlines() if ln.strip()]
        size = int(lines[0])
        magic = lines[1] if len(lines) > 1 else ""

        # A real spoken-sentence mp3 is far above 1KB; an error page / 0-byte file is not.
        self.assertGreater(size, 1024, f"CLB-A6 mp3 too small ({size} bytes) -> empty/error audio")
        ok_magic = magic == "494433" or magic.startswith("fffb") or magic.startswith("fff3")
        self.assertTrue(ok_magic, f"CLB-A6 not a valid mp3 header, first bytes {magic}")

        ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
        (ARTIFACT_DIR / f"edge_tts_mp3_{now_stamp()}.json").write_text(
            json.dumps({"size_bytes": size, "magic": magic}, indent=2), encoding="utf-8"
        )

    # ---- CLB-A7 (OC-7) ----------------------------------------------------- #
    def test_shim_chat_full_path_returns_audio_and_display(self):
        """coverage: CLB-A7(OC-7) — shim POST /v1/chat returns both audio and display."""
        url = f"{SHIM_BASE}/v1/chat"
        body = {"text": "你好，陪我聊聊天吧"}
        cmd = run_curl(url, body=body)
        out, err = ssh_assert_ok(cmd, "CLB-A7 shim /v1/chat probe")
        code, resp_body = parse_curl_stdout(out)
        self.assertIsNotNone(code, "CLB-A7 no HTTP code")
        self.assertNotIn(
            code, (None, 0),
            "CLB-A7 RED: shim /v1/chat not reachable (connection refused). "
            "The server/ shim is not running.",
        )
        self.assertNotEqual(code, 404,
                            "CLB-A7 RED: shim /v1/chat route absent (server/ shim not running)")
        self.assertEqual(code, 200, f"CLB-A7 shim returned {code} body={resp_body[:400]!r}")

        resp = json.loads(resp_body)
        audio = resp.get("audio")
        display = resp.get("display")
        self.assertTrue(audio, "CLB-A7 missing/non-empty audio field failed")
        if isinstance(audio, (str, bytes)):
            self.assertGreater(len(audio), 100, "CLB-A7 audio field is a near-empty string")
        self.assertIsNotNone(display, "CLB-A7 missing display field")

        # Accept display as an object or a JSON string (shim field shape not yet in contract).
        if isinstance(display, str):
            display = json.loads(display)
        view = display.get("view")
        self.assertIn(view, VALID_VIEWS, f"CLB-A7 display view {view!r} not a contract view")

    # ---- CLB-A8 (OC-11) ---------------------------------------------------- #
    def test_openclaw_consumes_schedule_md(self):
        """coverage: CLB-A8(OC-11) — a valid B1 schedule.md yields a correct answer + view:schedule."""
        # What weekday is it on the ECS right now? (1=Mon..7=Sun)
        date_out, date_err = ssh_assert_ok("date '+%u|%Y-%m-%d|%H:%M'", "CLB-A8 date probe")
        u, _, _ = date_out.strip().split("|")
        weekday_names = ["周一", "周二", "周三", "周四", "周五", "周六", "周日"]
        today_wd = weekday_names[int(u) - 1]

        generic = {
            "周一": ("19:00-20:00", "英语培训", "新东方"),
            "周二": ("18:30-19:30", "书法班", "文化宫"),
            "周三": ("17:00-18:00", "游泳课", "游泳馆"),
            "周四": ("18:00-19:00", "奥数班", "辅导中心"),
            "周五": ("17:30-18:30", "美术班", "少年宫"),
            "周六": ("10:00-11:30", "武术班", "武馆"),
            "周日": ("09:30-11:00", "围棋班", "棋院"),
        }
        TODAY_COURSE = "钢琴课"

        rows = []
        for wd in weekday_names:
            if wd == today_wd:
                time_slot, course, loc = "16:30-17:30", TODAY_COURSE, "琴行"
            else:
                time_slot, course, loc = generic[wd]
            rows.append(f"| {wd} | {time_slot} | {course} | {loc} | |")

        schedule_md = "\n".join([
            "---",
            "child: 小明",
            "timezone: Asia/Shanghai",
            "remind_before_min: 15",
            "---",
            "",
            "# 小明的课表",
            "",
            "## 每周固定课表",
            "| 星期 | 时间 | 课程 | 地点 | 备注 |",
            "|------|------|------|------|------|",
            *rows,
            "",
        ]) + "\n"

        write_cmd = remote_write(f"{OPENCLAW_WORKSPACE}/schedule.md", schedule_md)
        ssh_assert_ok(write_cmd, "CLB-A8 write schedule.md")

        url = f"{OPENCLAW_BASE}/v1/chat/completions"
        messages = [
            {"role": "system", "content": SYSTEM_PROMPT},
            {"role": "user", "content": "今天下午有什么课"},
        ]
        cmd = run_curl(url, body=openai_payload(messages))
        out, err = ssh_assert_ok(cmd, "CLB-A8 schedule query")
        code, body = parse_curl_stdout(out)
        self.assertEqual(code, 200, f"CLB-A8 query HTTP {code} body={body[:400]!r}")
        resp = json.loads(body)
        content = resp["choices"][0]["message"]["content"]

        block = extract_display_block(content)
        self.assertIsNotNone(block, f"CLB-A8 no ```display block in reply: {content[:400]!r}")
        display = json.loads(block)
        self.assertEqual(display.get("view"), "schedule",
                         f"CLB-A8 display view is {display.get('view')!r}, expected 'schedule'")
        items = display.get("data", {}).get("items")
        self.assertTrue(items, "CLB-A8 schedule display has empty items")
        names = [it.get("name") for it in items]
        self.assertIn(TODAY_COURSE, names,
                      f"CLB-A8 wrong answer: items={names!r}, expected to include {TODAY_COURSE!r}")
        self.assertIn(TODAY_COURSE, content,
                      f"CLB-A8 spoken answer does not mention the course: {content[:300]!r}")

        ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
        (ARTIFACT_DIR / f"schedule_md_consumption_{now_stamp()}.json").write_text(
            json.dumps({"today_weekday": today_wd, "query": "今天下午有什么课",
                        "answer": content, "display": display}, ensure_ascii=False, indent=2),
            encoding="utf-8",
        )

    # ---- boundary ---------------------------------------------------------- #
    def test_shim_chat_empty_text_rejected(self):
        """boundary: empty-text chat is rejected (4xx), not silently 200 with garbage."""
        url = f"{SHIM_BASE}/v1/chat"
        for empty in [{}, {"text": ""}, {"text": "   "}]:
            cmd = run_curl(url, body=empty)
            out, err = ssh_assert_ok(cmd, "CLB boundary empty-text probe")
            code, body = parse_curl_stdout(out)
            # code 0/None = curl could not connect -> shim absent (missing behaviour, red),
            # not a legitimate "rejection". A rejection must be a real HTTP 4xx response.
            self.assertNotIn(
                code, (None, 0),
                "CLB boundary RED: shim /v1/chat not reachable (connection refused). "
                "The shim must be running before empty-text rejection can be judged.",
            )
            self.assertIn(
                code, range(400, 500),
                f"CLB boundary RED: empty text {empty!r} returned HTTP {code} ({body[:200]!r}), "
                "expected a 4xx rejection. A text->OpenClaw->TTS pipeline must reject empty input.",
            )


if __name__ == "__main__":
    unittest.main(verbosity=2)