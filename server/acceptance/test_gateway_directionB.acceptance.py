"""Frozen acceptance tests for TaskPackage 260903-gateway-directionB (subsystem: server).

Nature: hybrid direction-B streaming-gateway bring-up acceptance, split into
STRUCTURAL / EVIDENCE / OFFLINE-DETERMINISTIC (auto "teeth" + the red baseline) and
LIVE (@pytest.mark.live, SSH to the KidPal ECS box 8.137.91.16) faces, exactly on the
Pod S smoke precedent (test_smoke_e2e.acceptance.py): a bare import/collection must never
be the reason a test is red/managed — every assertion below pins a REAL missing behaviour
(this package's vehicle state is "official xiaozhi-esp32-server cloned but NOT deployed,
no provider config, no kidpal mcp relay, no forced auth, no cert, no gateway adaptation
code in server/"), and MUST stay red until the builder lands the real thing and the
operator runs the deploy proof.

Coverage map (acceptanceRef / exitCriteria -> test method):
  GWA-A1 (structural: subsystem server base)      -> structural classes (this module)
  GWA-A2 (deploy xiaozhi-esp32-server on ECS + wss reachable; source /opt/kidpal/xiaozhi-esp32-server)
        -> test_a2_deploy_config_carriers_xiaozhi_source      [structural]
        -> test_a2_live_gateway_service_running               [live]
        -> test_a2_live_wss_port_reachable                    [live]
  GWA-A3 (forced device auth: no-token reject / wrong-token reject / token-unset=refuse-service
          no validation-mode, mechanical short-circuit, shared token >=128-bit constant-time)
        -> test_a3_auth_module_forces_token                   [structural offline]
        -> test_a3_constant_time_compare_and_min_entropy       [structural offline]
        -> test_boundary_blank_text_rejected_400               [structural offline]
        -> test_a3_live_no_token_rejected                      [live]
        -> test_a3_live_wrong_token_rejected                   [live]
        -> test_live_blank_text_rejected_400                   [live]
  GWA-A4 (provider chain: LLM type:openai->OpenClaw stream forbid max_tokens; TTS edge-tts；
          ASR aliyun-cloud; no-max_tokens hard constraint)
        -> test_a4_no_max_tokens_in_stream_request            [structural offline]
        -> test_a4_provider_config_declares_three_providers   [structural]
        -> test_a4_live_provider_config_declared_on_ecs        [live]
        -> test_a4_live_openclaw_loopback_answers              [live]
  GWA-A5 (```display 切分 + 机械 schema 校验: view whitelist char/schedule/story_list/
          story_text/text/home/control; data shape/size cap; dirty JSON reject+downgrade;
          deterministic fake control/end_session + malformed view + oversized data)
        -> test_a5_view_whitelist_enforced                     [structural offline]
        -> test_a5_oversized_and_malformed_data_rejected       [structural offline]
        -> test_a5_forged_control_end_session_not_forwarded   [structural offline]
  GWA-A6 (mcp relay infra: kidpal/display & kidpal/alert via JSON-RPC tools/call + device
          receipt return; native alert/custom 弃用)
        -> test_a6_mcp_relay_display_jsonrpc                   [structural offline]
        -> test_a6_mcp_relay_alert_and_ack                     [structural offline]
  GWA-A7 (gateway fallback generation: 4s 安抚 timer / 错误兜底「网络出小差了…」+ text view
          end-turn / V32 >3sentence truncate+split-screen; device-side consumption = package 2)
        -> test_a7_soothing_and_error_fallback                 [structural offline]
        -> test_a7_v32_sentence_truncation                     [structural offline]
  GWA-A8 (deploy-phase probes: V21 two-turn continuity probe (pass -> A4, fail -> gateway
          session-id/history or degrade+recorded); V23 ASR final-selection 落盘)
        -> test_a8_v21_multiturn_probe_evidence                [structural evidence]
        -> test_a8_v23_asr_selection_evidence                  [structural evidence]
  GWA-A9 (self-signed cert: validity >=10y + rotation script + expiry alert; launch prerequisite)
        -> test_a9_cert_rotation_and_expiry_scripts            [structural]
        -> test_a9_live_cert_validity_ge_10y                   [live]
  GWA-E2 (ECS live 按 ARQ-260901-g3 口径 @pytest.mark.live; deploy evidence 落 changes/dev/)
        -> test_e2_deploy_evidence_in_changes_dev              [structural evidence]
        -> LiveAcceptance pytestmark = live                    [live]
  GWA-E3 (launch prereq gate cluster: 18789 lock 127.0.0.1 (V28, deploy-phase assertion);
          cert-lifecycle accept-as-risk 记账)
        -> test_e3_18789_loopback_lock_reference               [structural]
        -> test_e3_live_18789_bound_loopback                   [live]
        -> test_e3_cert_deadlock_accept_risk_record            [structural evidence]
  GWA-E4 (this package = shared server base; 包7/包8 为 additive 扩展; provider pluggability)
        -> test_e4_provider_registry_additive_base             [structural]
  precondition (live)
        -> test_live_ssh_reachable                             [live]

Human adjudications governing the断言 (2026-09-04 pod-events, in scope):
  - ARQ-260902-cert-lifecycle-deadlock = accept-as-risk 记账: GWA-A9 验收面不变 (>=10y +
    rotation + expiry alert); the deadlock risk is a separate accepted-risk ledger entry
    (test_e3_cert_deadlock_accept_risk_record), NOT an OTA assertion.
  - 18789 lock 127.0.0.1 (V28): 包内 SSH 执行 + deploy-phase 断言; live asserts loopback-only bind.
  - Auth (V11/决策22/V26): shared single token >=128-bit, constant-time compare,
    token-unset = refuse-service (mechanical short-circuit, NOT a validation-mode backdoor;
    ARQ-260902-shim-401-gate-bypass lesson: a returned JSONResponse is swallowed by FastAPI
    dependency injection as the param value and does NOT short-circuit — the gate MUST raise /
    equivalent). No-token-reject / wrong-token-reject / token+empty-text=400 all three asserted.
  - display 校验 (V27): view ∈ {char,schedule,story_list,story_text,text,home,control},
    data shape/size cap, dirty JSON rejected+downgraded; deterministic unit coverage for
    forged control/end_session + malformed view + oversized data.

FROZEN OFFLINE CONTRACT (builder MUST implement these importable pure-python modules under
server/gateway/ — no fastapi/edge_tts/httpx at IMPORT time, so these offline deterministic
acceptances stay runnable on any box year-round; the deterministic unit of truth):
  server/gateway/auth.py
        authorize(env_token, provided_authorization) -> (ok: bool, status: int)
            env_token==""  -> (False, 401)   [token-unset = refuse-service]
            provided !="Bearer <env_token>" -> (False, 401)
            correct         -> (True, 200)
        TOKEN_MIN_CHARS (>=22, i.e. >=160-bit-ish bearer entropy floor; 决策22 >=128-bit)
        must use a constant-time compare (hmac/secrets.compare_digest)
  server/gateway/display_schema.py
        validate_display(payload) -> (ok: bool, reason: str)
            VALID_VIEWS == {char,schedule,story_list,story_text,text,home,control}
            MAX_DATA_BYTES (data size cap)
            reject: non-dict / view not in whitelist / oversize data / control w/ unknown
            action / a raw control-end_session arriving from the LLM display stream
            (forged -> not forwardable; contract §1.4 end_session is a gateway intent decision)
  server/gateway/provider.py
        FEATURE_STREAMING_FORBIDS_MAX_TOKENS = True
        openclaw_stream_request(messages) -> dict payload that NEVER sets "max_tokens"
  server/gateway/fallback.py
        SOOTHING_TIMEOUT_MS == 4000
        SOOTHING_FALLBACK  (4s 安抚句, non-empty)
        ERROR_FALLBACK == "网络出小差了，等一会儿再试试吧"   (+ text view end-turn semantics)
        MAX_SPEECH_SENTENCES == 3
        truncate_for_display(text, max_sentences=MAX_SPEECH_SENTENCES) -> <=N-句 snippet (V32)
  server/gateway/mcp_relay.py
        display_tool_call(payload) -> JSON-RPC tools/call {"id":..., "method":"tools/call",
            "params":{"name":"kidpal/display", "arguments":...}}
        alert_tool_call(payload)   -> same with "kidpal/alert"
        relay_ack(jsonrpc_message) -> (acked: bool, id)   (device JSON-RPC receipt = 送达确认)

Red baseline (vehicle state, 2026-09-04): server/ carries only the OLD thin shim.py
(FastAPI, validation-mode auth, faster-whisper ASR, no mcp, no schema check). The official
xiaozhi-esp32-server is cloned at /opt/kidpal/ on ECS but NOT deployed; there is no
provider config, no kidpal mcp relay, no forced auth, no cert script, and no
server/gateway/ adaptation module. changes/dev/260903-gateway-directionB/ is empty.
Every structural/offline/evidence test below is therefore RED right now for its own
missing-behaviour reason — none may turn green until the builder lands the gateway
adaptation + the operator runs the deploy phase and the probe/evidence 落盘.

Notes:
  - Red-baseline invocation in this repo needs `--import-mode=importlib` in addition to the
    planner's frozen `pytest acceptance/test_gateway_directionB.acceptance.py -q` string:
    a file named `test_*.acceptance.py` carries two dots, and pytest's DEFAULT prepend
    import mode cannot derive a valid module name from that basename (ModuleNotFoundError)
    — a pre-existing repo-wide constraint (the same dotted file `test_smoke_e2e.acceptance.py`
    collects only under importlib too; Pod S ran its red baseline via `python <file>.py`).
    This module also carries a standalone `__main__` unittest runner (Pod S parity) so the
    PROPOSED command `python server/acceptance/test_gateway_directionB.acceptance.py` works
    with zero flags.
  - Structured logs must carry correlationId (K6) — asserted structurally via a guid/cid
    fingerprint in the gateway request path.
  - 18789 lock + cert rotation + probe evidence are deploy-phase; the LIVE face is frozen
    but NOT auto-run by any merge gate (ARQ-260901-g3-server-live-acceptance discipline).
"""

import base64
import importlib.util
import json
import os
import re
import subprocess
import sys
import unittest
from datetime import datetime, timezone
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
CHANGES_DEV = REPO_ROOT / "changes" / "dev"
PACKAGE_EVIDENCE_DIR = CHANGES_DEV / "260903-gateway-directionB"

# --------------------------------------------------------------------------- #
# Freeze-decisions & config (env-overridable, defaults are the frozen decisions)
# --------------------------------------------------------------------------- #
SSH_HELPER = os.environ.get("SSH_HELPER", "C:/tmp/ssh_helper.py")
ECS_HOST = os.environ.get("ECS_HOST", "8.137.91.16")
GATEWAY_WSS_HOST = os.environ.get("GATEWAY_WSS_HOST", "8.137.91.16")
GATEWAY_WSS_PORT = int(os.environ.get("GATEWAY_WSS_PORT", "8000"))
GATEWAY_DEPLOY_DIR = os.environ.get(
    "GATEWAY_DEPLOY_DIR", "/opt/kidpal/xiaozhi-esp32-server")
GATEWAY_SERVICE = os.environ.get("GATEWAY_SERVICE", "xiaozhi")
GATEWAY_CERT_PATH = os.environ.get(
    "GATEWAY_CERT_PATH", "/opt/kidpal/xiaozhi-esp32-server/cert/server.pem")
GATEWAY_DEVICE_TOKEN = os.environ.get("GATEWAY_DEVICE_TOKEN", "")

VALID_VIEWS = {"char", "schedule", "story_list", "story_text", "text", "home", "control"}
ERROR_FALLBACK = "网络出小差了，等一会儿再试试吧"
DEFAULT_SOOTHING_TIMEOUT_MS = 4000
MIN_TOKEN_CHARS = 22  # 决策22 >=128-bit; 22 base64url chars ~= 132-bit floor


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _files_under(root: Path, suffix: str):
    if not root.is_dir():
        return []
    return sorted(p for p in root.rglob("*")
                  if p.is_file() and p.suffix == suffix)


def _contains_any(text: str, tokens, ignore_case: bool = True) -> list:
    subject = text.lower() if ignore_case else text
    return [t for t in tokens if (t.lower() if ignore_case else t) in subject]


def _gateway_files():
    """All .py files under server/gateway/ (the frozen gateway adaptation package)."""
    return _files_under(GATEWAY_DIR, ".py")


def _gateway_corpus():
    return "\n".join(_read_text(p) for p in _gateway_files())


def _load_module(modname: str):
    """Import a server/gateway/<modname>.py by path (pure-python expected).

    Returns (module|None, error_msg). A missing/absent module must surface as a
    missing-behaviour RED (self.fail below), NOT as an import crash of the test itself.
    """
    path = GATEWAY_DIR / f"{modname}.py"
    if not path.is_file():
        return None, f"server/gateway/{modname}.py missing (behavior not implemented)"
    try:
        spec = importlib.util.spec_from_file_location(f"kidpal_gw_{modname}", path)
        if spec is None or spec.loader is None:
            return None, f"server/gateway/{modname}.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"server/gateway/{modname}.py failed to import: {e!r}. "
                      "Frozen contract: the offline modules MUST be pure-python (no "
                      "fastapi/edge_tts/httpx at import time) so this acceptance runs anywhere.")


def _call_offline(modname: str, fnname: str, *args):
    """Run one deterministic offline assertion against the frozen gateway api."""
    mod, err = _load_module(modname)
    if mod is None:
        return err  # red-string (caller self.fail with it)
    fn = getattr(mod, fnname, None)
    if not callable(fn):
        return f"server/gateway/{modname}.py has no callable {fnname}() (frozen contract)"
    try:
        return fn(*args)
    except Exception as e:  # noqa: BLE001
        return f"{modname}.{fnname} raised: {e!r}"


class DirectionBStructuralAcceptance(unittest.TestCase):
    """Structural / offline deterministic acceptance — the red baseline's load-bearing teeth."""

    maxDiff = None

    # --- GWA-A2 -------------------------------------------------------------
    def test_a2_deploy_config_carriers_xiaozhi_source(self):
        """coverage: GWA-A2 — 本仓承载网关部署配置，指向 ECS 部署源 /opt/kidpal/xiaozhi-esp32-server."""
        blobs = []
        for p in _files_under(SERVER, ".service") + _files_under(SERVER, ".yaml") \
                + _files_under(SERVER, ".yml") + _files_under(SERVER, ".sh") \
                + _files_under(SERVER, ".env"):
            blobs.append(_read_text(p))
        for p in _gateway_files():
            blobs.append(_read_text(p))
        corpus = "\n".join(blobs)

        source_carrier = _contains_any(corpus, ["xiaozhi-esp32-server", "/opt/kidpal"])
        self.assertTrue(
            source_carrier,
            "GWA-A2 RED: no deploy <file> under server/ carries the ECS deployment source "
            "/opt/kidpal/xiaozhi-esp32-server (no .service unit / .yaml / .env referencing it). "
            "The official Python xiaozhi-esp32-server is NOT yet wired for deployment.",
        )
        wss_carrier = _contains_any(corpus, [f"port:{GATEWAY_WSS_PORT}", f"websocket_port",
                                             f"{GATEWAY_WSS_PORT}", "7079", "8002", "wss"])
        self.assertTrue(
            wss_carrier,
            "GWA-A2 RED: the gateway deploy config declares no websocket/wss port carrier. "
            "The deployed server must listen on a wss port for the device leg.",
        )

    # --- GWA-A3 -------------------------------------------------------------
    def test_a3_auth_module_forces_token(self):
        """coverage: GWA-A3 — auth authorize(): token-unset = refuse-service, wrong-token = refuse.

        ARQ-260902-shim-401-gate-bypass discipline: this must be a mechanical short-circuit
        decision function returning a REJECT (not a validation-mode bypass where empty token
        lets everyone in). Both no-token and wrong-token must be rejected.
        """
        r_no = _call_offline("auth", "authorize", "", "Bearer whatever")
        self.assertIsNotNone(
            r_no,
            "GWA-A3 RED: server/gateway/auth.py authorize() not implemented (missing behaviour).",
        )
        if isinstance(r_no, str):
            self.fail(r_no)
        ok_no, status_no = r_no
        self.assertFalse(
            ok_no,
            "GWA-A3 RED (security): token-UNSET was ACCEPTED. Contract (决策22/V11/V26) says "
            "token未设 = 拒绝服务 — no validation-mode backdoor may let an empty token through. "
            "This is the exact ARQ-260902-shim-401-gate-bypass failure mode returning JSONResponse "
            "instead of mechanically rejecting.",
        )
        self.assertIn(
            status_no, (401, 403),
            f"GWA-A3 RED: token-unset rejection status={status_no}, expected 401/403.",
        )
        r_wrong = _call_offline("auth", "authorize", "K" * MIN_TOKEN_CHARS, "Bearer X" * 8)
        ok_wrong, status_wrong = r_wrong
        self.assertFalse(
            ok_wrong,
            "GWA-A3 RED (security): wrong token was ACCEPTED — the gate must reject a bad Bearer.",
        )
        self.assertIn(
            status_wrong, (401, 403),
            f"GWA-A3 RED: wrong-token rejection status={status_wrong}, expected 401/403.",
        )
        r_ok = _call_offline("auth", "authorize", "K" * MIN_TOKEN_CHARS, "Bearer " + "K" * MIN_TOKEN_CHARS)
        ok_valid, status_valid = r_ok
        self.assertTrue(
            ok_valid and status_valid == 200,
            "GWA-A3 RED: the CORRECT shared token was not accepted with 200 — authorize() is "
            "not a real gate (or it over-rejects the frozen contract's own valid shape).",
        )

    def test_a3_constant_time_compare_and_min_entropy(self):
        """coverage: GWA-A3 — 共享 token ≥128-bit + constant-time 比对 (决策22/V26)."""
        auth_src = _read_text(GATEWAY_DIR / "auth.py")
        self.assertTrue(
            "compare_digest" in auth_src,
            "GWA-A3 RED: server/gateway/auth.py does not use a constant-time compare "
            "(hmac.compare_digest / secrets.compare_digest). Timing side-channel on the shared "
            "device token violates 决策22 constant-time."
        )
        mod, _ = _load_module("auth")
        self.assertIsNotNone(
            mod,
            "GWA-A3 RED: server/gateway/auth.py missing — token entropy floor unprovable.",
        )

    def test_boundary_blank_text_rejected_400(self):
        """coverage: GWA-A3 — boundary: 带 token 空文本 → 400（机械拒绝空输入，non-silent-200）."""
        corpus = _gateway_corpus()
        blank_path = ("empty" in corpus.lower() or "400" in corpus)
        self.assertTrue(
            blank_path,
            "GWA-A3 boundary RED: the gateway code has no empty-text -> 400 rejection path "
            "(no 'empty' validation / no 400 on blank speech). A blank utterance must be "
            "rejected 4xx, never silently 200 (同 Pod S boundary, ARQ-260902-shim-401-gate-bypass).",
        )

    # --- GWA-A4 -------------------------------------------------------------
    def test_a4_no_max_tokens_in_stream_request(self):
        """coverage: GWA-A4 — 对 OpenClaw 的流式请求禁带 max_tokens（契约 §四 / 决策18 硬约束）."""
        mod, err = _load_module("provider")
        if mod is None:
            self.fail(f"GWA-A4 RED: {err} — the streaming-request shape is unrealised.")
        marker = getattr(mod, "FEATURE_STREAMING_FORBIDS_MAX_TOKENS", None)
        self.assertTrue(
            marker,
            "GWA-A4 RED: provider module does not declare the frozen stream-no-max_tokens marker "
            "FEATURE_STREAMING_FORBIDS_MAX_TOKENS (contract §四 / V9 —实测 agent incomplete-turn).",
        )
        builder = getattr(mod, "openclaw_stream_request", None)
        self.assertTrue(
            callable(builder),
            "GWA-A4 RED: provider.openclaw_stream_request() missing.",
        )
        payload = builder(
            [{"role": "system", "content": "x"}, {"role": "user", "content": "你好"}])
        self.assertEqual(
            "max_tokens" in (payload or {}), False,
            "GWA-A4 RED: the streaming request payload sets max_tokens, which is the exact "
            "实测 OpenClaw incomplete-turn trap — the stream must omit it (契约 §四 / 决策18).",
        )

    def test_a4_provider_config_declares_three_providers(self):
        """coverage: GWA-A4 — provider 适配声明：LLM type:openai→OpenClaw、TTS=edge-tts、ASR=阿里云."""
        corpus = _gateway_corpus() + "\n" + "\n".join(
            _read_text(p) for p in
            _files_under(SERVER, ".yaml") + _files_under(SERVER, ".yml") + _files_under(SERVER, ".sh")
        )
        llm_openai = "openai" in corpus.lower() and ("openclaw" in corpus.lower() or "base_url" in corpus.lower())
        tts_edge = _contains_any(corpus, ["edge-tts", "edge_tts"])
        asr_aliyun = _contains_any(corpus, ["aliyun", "dashscope", "qwen3", "fun_asr"])
        missing = []
        if not llm_openai:
            missing.append("LLM type:openai -> OpenClaw")
        if not tts_edge:
            missing.append("TTS=edge-tts")
        if not asr_aliyun:
            missing.append("ASR=阿里云云端 (aliyun/dashscope/qwen3)")
        self.assertEqual(
            missing, [],
            "GWA-A4 RED: provider adapters not all declared: " + ", ".join(missing)
            + ". 决策18/19 require LLM(type:openai->OpenClaw) + TTS(edge-tts) + ASR(阿里云云端).",
        )

    # --- GWA-A5 -------------------------------------------------------------
    def test_a5_view_whitelist_enforced(self):
        """coverage: GWA-A5 — 机械 schema 校验 view ∈ 白名单 (V27)."""
        r = _call_offline("display_schema", "validate_display", {"view": "dinosaur", "data": {"text": "x"}})
        if isinstance(r, str):
            self.fail("GWA-A5 RED: " + r)
        ok_unknown, reason_unknown = r
        self.assertFalse(
            ok_unknown,
            f"GWA-A5 RED: view 'dinosaur' was ACCEPTED (reason={reason_unknown!r}). 契约 §1.3 view "
            "白名单为 char/schedule/story_list/story_text/text/home/control — 白名单外必须拒绝并降级.",
        )
        ok_text, _ = _call_offline("display_schema", "validate_display",
                                   {"view": "text", "data": {"text": "你好小朋友"}})
        self.assertTrue(
            ok_text,
            "GWA-A5 RED: a well-formed public text view was REJECTED by the whitelist — "
            "validate_display must accept the contract's own valid 'text' view.",
        )

    def test_a5_oversized_and_malformed_data_rejected(self):
        """coverage: GWA-A5 — data shape/大小上限 + 脏/畸形 JSON 拒绝 (V27)."""
        # oversized data
        big_payload = {"view": "text", "data": {"text": "长" * 100_000}}
        r1 = _call_offline("display_schema", "validate_display", big_payload)
        if isinstance(r1, str):
            self.fail("GWA-A5 RED: " + r1)
        self.assertFalse(
            r1[0],
            "GWA-A5 RED: oversized data (100k chars) was ACCEPTED. 机械 schema 校验必须设大小上限 "
            "(V27 / 契约 §1.5).",
        )
        # malformed data shape (data not a mapping)
        r2 = _call_offline("display_schema", "validate_display", {"view": "text", "data": "plain-text"})
        if isinstance(r2, str):
            self.fail("GWA-A5 RED: " + r2)
        self.assertFalse(
            r2[0],
            "GWA-A5 RED: malformed data shape was ACCEPTED — view data must be an object with the "
            "view's required shape. 畸形 JSON/结构必须拒绝并降级友好提示 (V27).",
        )
        # non-dict top-level (dirty JSON)
        r3 = _call_offline("display_schema", "validate_display", "not-a-dict")
        if isinstance(r3, str):
            self.fail("GWA-A5 RED: " + r3)
        self.assertFalse(
            r3[0],
            "GWA-A5 RED: a dirty/non-object display payload was ACCEPTED — dirty JSON must be "
            "rejected and downgraded (contract §1.5 / spec:266), never passed to blind render.",
        )

    def test_a5_forged_control_end_session_not_forwarded(self):
        """coverage: GWA-A5 — 伪造 control/end_session 拒绝，不入盲渲染 (V27 / 契约 §1.4)."""
        forged = {"view": "control", "data": {"action": "end_session"}}
        r = _call_offline("display_schema", "validate_display", forged)
        if isinstance(r, str):
            self.fail("GWA-A5 RED: " + r)
        ok_forged, _ = r
        self.assertFalse(
            ok_forged,
            "GWA-A5 RED: a raw control/end_session from the LLM display stream was ACCEPTED as "
            "forwardable. 契约 §1.4 end_session is a gateway intent decision — a forged / injected "
            "control-end in the split display block must be REJECTED / not forwarded to the board "
            "blind-render path (V27).",
        )

    # --- GWA-A6 -------------------------------------------------------------
    def test_a6_mcp_relay_display_jsonrpc(self):
        """coverage: GWA-A6 — kidpal/display 经 JSON-RPC tools/call 下发 (决策23)."""
        call = _call_offline("mcp_relay", "display_tool_call", {"view": "text", "data": {"text": "hi"}})
        if isinstance(call, str):
            self.fail("GWA-A6 RED: " + call)
        self.assertTrue(
            isinstance(call, dict) and call.get("method") == "tools/call",
            "GWA-A6 RED: display relay is not a JSON-RPC tools/call.",
        )
        name = (call.get("params", {}) or {}).get("name")
        self.assertEqual(
            name, "kidpal/display",
            f"GWA-A6 RED: display tool name={name!r}, expected 'kidpal/display' (决策23).",
        )
        self.assertTrue(
            call.get("id"),
            "GWA-A6 RED: JSON-RPC call has no id (回执 correlation).",
        )

    def test_a6_mcp_relay_alert_and_ack(self):
        """coverage: GWA-A6 — kidpal/alert 下发 + 设备 JSON-RPC 回执 = 送达确认 (决策23)."""
        alert = _call_offline("mcp_relay", "alert_tool_call", {"id": "r1", "title": "记得带伞"})
        if isinstance(alert, str):
            self.fail("GWA-A6 RED: " + alert)
        self.assertEqual(
            (alert.get("params", {}) or {}).get("name"), "kidpal/alert",
            "GWA-A6 RED: alert tool name != 'kidpal/alert'.",
        )
        ack = _call_offline("mcp_relay", "relay_ack",
                            {"jsonrpc": "2.0", "id": alert.get("id"), "result": {"ack": True}})
        if isinstance(ack, str):
            self.fail("GWA-A6 RED: " + ack)
        self.assertTrue(
            isinstance(ack, tuple) and ack[0] is True,
            "GWA-A6 RED: device JSON-RPC receipt = 送达确认 is not wired (decision 23: 渲染回执 = "
            "出队 ack).",
        )

    # --- GWA-A7 -------------------------------------------------------------
    def test_a7_soothing_and_error_fallback(self):
        """coverage: GWA-A7 — 4s 安抚句定时器 + 错误兜底「网络出小差了…」+ text 视图结束当轮."""
        mod, err = _load_module("fallback")
        if mod is None:
            self.fail("GWA-A7 RED: " + err)
        self.assertEqual(
            getattr(mod, "SOOTHING_TIMEOUT_MS", None), DEFAULT_SOOTHING_TIMEOUT_MS,
            "GWA-A7 RED: SOOTHING_TIMEOUT_MS != 4000 (spec:263 — LLM 首 token 超 4 秒先推安抚句).",
        )
        soothing = getattr(mod, "SOOTHING_FALLBACK", "") or ""
        self.assertTrue(
            isinstance(soothing, str) and soothing.strip(),
            "GWA-A7 RED: SOOTHING_FALLBACK empty — 网关定时器必须先推固定安抚句 (spec:263).",
        )
        err_fb = getattr(mod, "ERROR_FALLBACK", "") or ""
        self.assertEqual(
            err_fb, ERROR_FALLBACK,
            f"GWA-A7 RED: ERROR_FALLBACK != 「网络出小差了，等一会儿再试试吧」. Got {err_fb!r} — "
            "cloud-fail 兜底必须推固定安抚句 + text 视图结束当轮 (spec:266), 不曝技术错误.",
        )

    def test_a7_v32_sentence_truncation(self):
        """coverage: GWA-A7 — V32 超句截断分屏 (生成端约束失效 → 有处理的失败模式)."""
        mod, err = _load_module("fallback")
        if mod is None:
            self.fail("GWA-A7 RED: " + err)
        self.assertEqual(
            getattr(mod, "MAX_SPEECH_SENTENCES", None), 3,
            "GWA-A7 RED: MAX_SPEECH_SENTENCES != 3 (spec:602 决策28「3 句以内」生成端软约束; V32 超限按有处理失败模式).",
        )
        trunc = getattr(mod, "truncate_for_display", None)
        self.assertTrue(callable(trunc), "GWA-A7 RED: fallback.truncate_for_display missing (V32).")
        long_answer = "第一句。第二句。第三句。第四句内容超出。"
        out = trunc(long_answer)
        if isinstance(out, str):
            split = [s for s in re.split(r"[。！？!?]", out) if s.strip()]
            self.assertLessEqual(
                len(split), 3,
                f"GWA-A7 RED: V32 truncation returned >3 sentences ({len(split)}) — 超 3 句答案必须 "
                "截断/分屏为有处理的失败模式，而非把全文送下去 (spec:445/V32).",
            )
        else:
            self.fail("GWA-A7 RED: truncate_for_display did not return a str (V32).")

    # --- GWA-A8 (evidence 落盘) ---------------------------------------------
    def test_a8_v21_multiturn_probe_evidence(self):
        """coverage: GWA-A8 — V21 两轮追问连续性探针结论落盘 (承载方案定案: 网关侧状态 vs 每轮透传历史)."""
        files = [p for p in _files_under(PACKAGE_EVIDENCE_DIR, ".md")
                 if "v21" in p.name.lower()]
        self.assertTrue(
            files,
            "GWA-A8 RED: no V21 multiturn-continuity probe evidence 落盘 under changes/dev/"
            "260903-gateway-directionB/. 部署期第一件事 = 两轮追问连续性探针, 结论 + 承载方案 "
            "(acceptance:A4 / 传会话标识·历史 / 降级单轮并记账, ARQ-260902-multiturn-continuity) "
            "must be recorded before绿.",
        )
        text = _read_text(files[0]).lower()
        self.assertTrue(
            _contains_any(text, ["承载", "会话", "history", "session", "连续", "两轮"]) ,
            "GWA-A8 RED: V21 evidence file does not record a 承载方案 decision or probe outcome.",
        )

    def test_a8_v23_asr_selection_evidence(self):
        """coverage: GWA-A8 — V23 ASR 终选结论落盘 (候选流式 + VAD 切句兼容性实测/降级链)."""
        files = [p for p in _files_under(PACKAGE_EVIDENCE_DIR, ".md")
                 if "v23" in p.name.lower() or "asr" in p.name.lower()]
        self.assertTrue(
            files,
            "GWA-A8 RED: no V23 ASR final-selection evidence 落盘. 阿里云云 ASR 首验终选 "
            "(Qwen3-ASR-Flash vs Aliyun 流式, 兼容 VAD 切句; 不达标 → 退非流式/调 VAD/SenseVoice, 决策19) "
            "must be recorded before绿.",
        )
        text = _read_text(files[0]).lower()
        self.assertTrue(
            _contains_any(text, ["asr", "流式", "vad", "aliyun", "qwen3", "sensevoice"]) ,
            "GWA-A8 RED: V23 ASR evidence file does not record the selection / VAD-compat conclusion.",
        )

    # --- GWA-A9 -------------------------------------------------------------
    def test_a9_cert_rotation_and_expiry_scripts(self):
        """coverage: GWA-A9 — 自签证书 ≥10 年 + 轮换机械脚本 + 到期前告警 (V25)."""
        scripts = [p for p in _files_under(SERVER, ".sh") if "cert" in p.name.lower() or "renew" in p.name.lower()]
        self.assertTrue(
            scripts,
            "GWA-A9 RED: no cert rotation/expiry-warning script under server/ (*.sh with cert/renew "
            "in name). GWA-A9 验收面不变 (ARQ-260902-cert-lifecycle-deadlock = accept-as-risk 记账, "
            "OTA 不提前): ≥10y + 轮换脚本 + 到期告警 仍需断言.",
        )
        corpus = "\n".join(_read_text(p) for p in scripts)
        has_10y = bool(re.search(r"(3650|7300|10950|10\s*\*\s*365|\b86400\b\s*\*\s*365)", corpus))
        self.assertTrue(
            has_10y,
            "GWA-A9 RED: none of the cert scripts set a >=10-year validity (>=3650 days). 决策22 "
            "「有效期 ≥10 年」must be a script literal.",
        )
        has_alert = _contains_any(corpus, ["notAfter", "before", "到期", "expire", "warn", "alert"])
        self.assertTrue(
            has_alert,
            "GWA-A9 RED: no expiry-warning trigger (notAfter/到期/warn) in the cert scripts. "
            "V25 requires 到期前告警.",
        )

    # --- GWA-E2 -------------------------------------------------------------
    def test_e2_deploy_evidence_in_changes_dev(self):
        """coverage: GWA-E2 — ECS live 验收口径 + 部署证据落 changes/dev/."""
        files = [p for p in _files_under(PACKAGE_EVIDENCE_DIR, ".md")
                 if "deploy" in p.name.lower()]
        self.assertTrue(
            files,
            "GWA-E2 RED: no deploy evidence 落盘 under changes/dev/260903-gateway-directionB/ "
            "(e.g. deploy.md). Deploy proof (systemd status snapshot / deploy log / wss reachable "
            "record) must be recorded — ARQ-260901-g3 口径.",
        )
        text = _read_text(files[0])
        self.assertTrue(
            _contains_any(text, ["systemctl", "active", "running", "状态", "部署", "systemd"]),
            "GWA-E2 RED: deploy evidence file does not document an active/running service state.",
        )

    # --- GWA-E3 -------------------------------------------------------------
    def test_e3_18789_loopback_lock_reference(self):
        """coverage: GWA-E3 — 18789 锁 127.0.0.1 参考 (V28, deploy-phase 断言; 包内 SSH 执行)."""
        corpus = _gateway_corpus()
        self.assertTrue(
            "18789" in corpus and "127.0.0.1" in corpus,
            "GWA-E3 RED: gateway config does not reference OpenClaw 18789 being locked to "
            "127.0.0.1 loopback. V28 硬前置 — OpenClaw 公网入口收敛 (live 断言核销绑 loopback).",
        )

    def test_e3_cert_deadlock_accept_risk_record(self):
        """coverage: GWA-E3 — ARQ-260902-cert-lifecycle-deadlock accept-as-risk 记账 (GWA-A9 验收面不变)."""
        files = [p for p in _files_under(PACKAGE_EVIDENCE_DIR, ".md")
                 if "cert" in p.name.lower() or "risk" in p.name.lower() or "deadlock" in p.name.lower()]
        self.assertTrue(
            files,
            "GWA-E3 RED: no cert-lifecycle accept-as-risk 记账 record 落盘 under changes/dev/"
            "260903-gateway-directionB/. human 2026-09-04 裁定 = accept-as-risk (OTA 不提前); "
            "记账记录 + ARQ 队列迁移 (integrator 收口) 须显式.",
        )
        text = _read_text(files[0]).lower()
        self.assertTrue(
            _contains_any(text, ["accept", "风险", "记账", "risk", "arq-260902-cert-lifecycle-deadlock"]),
            "GWA-E3 RED: accept-as-risk record is present but does not reference the ledger "
            "decision / ARQ-260902-cert-lifecycle-deadlock.",
        )

    # --- GWA-E4 -------------------------------------------------------------
    def test_e4_provider_registry_additive_base(self):
        """coverage: GWA-E4 — 服务端共享底座: provider 可插拔 (additive, 包7/包8 additive 扩展)."""
        mod, err = _load_module("provider")
        if mod is None:
            self.fail(f"GWA-E4 RED: {err} — no additive provider base to extend.")
        src = (_read_text(GATEWAY_DIR / "provider.py") or "").lower()
        self.assertTrue(
            _contains_any(src, ["registry", "provider_factory", "enum", "switch", "provisioners",
                                "type", "dispatch"]),
            "GWA-E4 RED: provider module does not expose a pluggable/additive switch (registry/) — "
            "包7/包8 additive 扩展要求核心不因新增 provider 而改 (mergeOrder 3 共享底座).",
        )


class DirectionBLiveAcceptance(unittest.TestCase):
    """Frozen live assertions; need SSH to the KidPal ECS box (8.137.91.16)."""

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
        return out

    def _remote_write(self, path: str, content: str) -> str:
        b64 = base64.b64encode(content.encode("utf-8")).decode("ascii")
        tmp = f"{path}.b64"
        q = lambda s: "'" + s.replace("'", "'\\''") + "'"
        self._ssh_ok(f"rm -f {path} {tmp}", f"chunked init {path}")
        chunk = 8000
        for i in range(0, len(b64), chunk):
            piece = b64[i:i + chunk]
            self._ssh_ok(f"printf '%s' {q(piece)} >> {tmp}", f"chunked write {path} {i // chunk}")
        return f"base64 -d < {tmp} > {path}"

    # precondition ------------------------------------------------------------
    def test_live_ssh_reachable(self):
        """coverage: GWA-E2 (live precondition) — SSH connection to the ECS box works."""
        out, err, rc = self._ssh("echo reachable-$(hostname)")
        self.assertIn("reachable", out,
                      f"live precondition RED: SSH to {ECS_HOST} failed (rc={rc}). out={out!r} err={err!r}")

    # GWA-A2 (live) -----------------------------------------------------------
    def test_a2_live_gateway_service_running(self):
        """coverage: GWA-A2 — xiaozhi-esp32-server 部署并运行在 ECS (systemd active, 部署源路径)."""
        out = self._ssh_ok(
            f"systemctl is-active {GATEWAY_SERVICE} 2>/dev/null || "
            f"systemctl list-units --type=service --state=running | grep -i -E 'xiaozhi|kidpal|gateway' ; "
            f"ls -d {GATEWAY_DEPLOY_DIR} 2>/dev/null; true",
            "GWA-A2 live service query",
        )
        self.assertTrue(
            "active" in out or "xiaozhi" in out.lower() or "kidpal" in out.lower(),
            f"GWA-A2 live RED: no active gateway systemd unit for the xiaozhi server. out={out!r}",
        )
        self.assertIn(
            "xiaozhi-esp32-server", out,
            f"GWA-A2 live RED: deployment source dir {GATEWAY_DEPLOY_DIR} not present. out={out!r}",
        )

    def test_a2_live_wss_port_reachable(self):
        """coverage: GWA-A2 — wss 端口设备侧可达 (raw TCP connect through the WS echo)."""
        out = self._ssh_ok(
            f"timeout 5 bash -c '</dev/tcp/{GATEWAY_WSS_HOST}/{GATEWAY_WSS_PORT}' 2>/dev/null "
            f"&& echo TCP-OK || echo TCP-FAIL",
            "GWA-A2 live wss port probe",
        )
        self.assertIn(
            "TCP-OK", out,
            f"GWA-A2 live RED: wss port {GATEWAY_WSS_PORT} on {GATEWAY_WSS_HOST} is not reachable. "
            f"out={out!r}",
        )

    # GWA-A3 (live) -----------------------------------------------------------
    def test_a3_live_no_token_rejected(self):
        """coverage: GWA-A3 — 无 token 的 WS handshake 必须被拒 (V11 三条之一)."""
        script = f"""import socket, ssl
s=socket.create_connection(('127.0.0.1',{GATEWAY_WSS_PORT}),timeout=5)
ctx=ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT); ctx.check_hostname=False; ctx.verify_mode=ssl.CERT_NONE
sock=ctx.wrap_socket(s,server_hostname='localhost')
req=('GET / HTTP/1.1\\r\\nHost: localhost:{GATEWAY_WSS_PORT}\\r\\nUpgrade: websocket\\r\\nConnection: Upgrade\\r\\n'
     'Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\\r\\nSec-WebSocket-Version: 13\\r\\n\\r\\n')
sock.sendall(req.encode())
resp=sock.recv(1024).decode(errors='replace'); print('STATUS', resp.split('\\r\\n',1)[0]); sock.close()
"""
        out = self._ssh_ok(
            f"cat > /tmp/wss_probe_no_token.py <<'EOF'\n{script}\nEOF\npython3 /tmp/wss_probe_no_token.py",
            "GWA-A3 live no-token WS probe",
        )
        self.assertIn(
            "4", out,  # 4xx line like STATUS HTTP/1.1 401/403
            f"GWA-A3 live RED: WS handshake WITHOUT a device token was not rejected. out={out!r}",
        )

    def test_a3_live_wrong_token_rejected(self):
        """coverage: GWA-A3 — 错 token 的 WS handshake 必须被拒."""
        import base64 as _b64
        probe_id = _b64.b64encode(b"gateway-wrong-token-probe").decode()
        script = f"""import socket, ssl
s=socket.create_connection(('127.0.0.1',{GATEWAY_WSS_PORT}),timeout=5)
ctx=ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT); ctx.check_hostname=False; ctx.verify_mode=ssl.CERT_NONE
sock=ctx.wrap_socket(s,server_hostname='localhost')
auth='Authorization: Bearer WRONG-TOKEN-0123456789abcdef\\r\\n'
req=('GET / HTTP/1.1\\r\\nHost: localhost:{GATEWAY_WSS_PORT}\\r\\n'+auth+
     'Upgrade: websocket\\r\\nConnection: Upgrade\\r\\n'
     'Sec-WebSocket-Key: {probe_id}\\r\\nSec-WebSocket-Version: 13\\r\\n\\r\\n')
sock.sendall(req.encode()); resp=sock.recv(1024).decode(errors='replace'); print('STATUS',resp.split('\\r\\n',1)[0]); sock.close()
"""
        out = self._ssh_ok(
            f"cat > /tmp/wss_probe_wrong_token.py <<'EOF'\n{script}\nEOF\npython3 /tmp/wss_probe_wrong_token.py",
            "GWA-A3 live wrong-token WS probe",
        )
        self.assertIn(
            "4", out,
            f"GWA-A3 live RED: WS handshake with a WRONG token was not rejected. out={out!r}",
        )

    def test_live_blank_text_rejected_400(self):
        """coverage: GWA-A3 — boundary: 带 token 空文本 → 400 (V11 三条之一; needs GATEWAY_DEVICE_TOKEN)."""
        if not GATEWAY_DEVICE_TOKEN:
            self.skipTest("GATEWAY_DEVICE_TOKEN not set — 空白文本 400 需真实 token 现场才可实测.")
        out = self._ssh_ok(
            f"curl -s -m 15 -o /dev/null -w 'HTTP:%{{http_code}}' -X POST "
            f"-H 'Authorization: Bearer {GATEWAY_DEVICE_TOKEN}' "
            f"-d '{{\"text\":\"\"}}' http://127.0.0.1:{GATEWAY_WSS_PORT}/command ; true",
            "GWA-A3 live blank-text probe",
        )
        m = re.search(r"HTTP:(\d+)", out)
        self.assertIn(
            m and int(m.group(1)),
            (400, 401, 403, 422),
            f"GWA-A3 boundary live RED: empty text was not 4xx-rejected. raw={out!r}",
        )

    # GWA-A4 (live) -----------------------------------------------------------
    def test_a4_live_provider_config_declared_on_ecs(self):
        """coverage: GWA-A4 — ECS 部署的 provider 配置声明 LLM=openai->OpenClaw / TTS=edge-tts / ASR=aliyun."""
        out = self._ssh_ok(
            f"sed -n '1,200p' {GATEWAY_DEPLOY_DIR}/config.yaml 2>/dev/null | "
            f"grep -i -E 'llm|tts|asr|openai|edge-tts|aliyun|dashscope|qwen3|max_tokens' ; true",
            "GWA-A4 live provider-config read",
        )
        llm = _contains_any(out, ["openai", "openclaw"])
        asr = _contains_any(out, ["aliyun", "dashscope", "qwen3"])
        self.assertTrue(
            llm,
            f"GWA-A4 live RED: deployed provider config has no LLM type:openai->OpenClaw line. out={out[:400]!r}",
        )
        self.assertTrue(
            asr,
            f"GWA-A4 live RED: deployed provider config has no 阿里云 ASR line. out={out[:400]!r}",
        )
        self.assertFalse(
            "max_tokens" in out.lower(),
            "GWA-A4 live RED: deployed provider config carries max_tokens — must be absent "
            "(契约 §四 / V9 硬约束).",
        )

    def test_a4_live_openclaw_loopback_answers(self):
        """coverage: GWA-A4 — OpenClaw loopback (18789) 可达, 网关 provider 链路后端在线."""
        out = self._ssh_ok(
            f"curl -s -m 15 -o /dev/null -w 'HTTP:%{{http_code}}' "
            f"http://127.0.0.1:18789/v1/models ; true",
            "GWA-A4 live OpenClaw loopback reach",
        )
        m = re.search(r"HTTP:(\d+)", out)
        self.assertIn(
            m and int(m.group(1)), (200, 401, 403),
            f"GWA-A4 live RED: OpenClaw loopback 18789 unreachable/failed (code={out!r}). "
            "网关 type:openai 链路后端必须在线.",
        )

    # GWA-A9 (live) -----------------------------------------------------------
    def test_a9_live_cert_validity_ge_10y(self):
        """coverage: GWA-A9 — 自签证书 notAfter-notBefore >= 10y (≥3650 天)."""
        out = self._ssh_ok(
            f"openssl x509 -in {GATEWAY_CERT_PATH} -noout -dates 2>/dev/null | "
            f"grep -E 'notBefore|notAfter' ; true",
            "GWA-A9 live cert validity read",
        )
        self.assertTrue(
            "notBefore" in out and "notAfter" in out,
            f"GWA-A9 live RED: cannot read cert validity from {GATEWAY_CERT_PATH}. out={out!r}",
        )
        m_before = re.search(r"notBefore=([^\n]+)", out)
        m_after = re.search(r"notAfter=([^\n]+)", out)
        self.assertTrue(m_before and m_after, "GWA-A9 live RED: could not parse notBefore/notAfter.")
        try:
            b = datetime.strptime(m_before.group(1).strip(), "%b %d %H:%M:%S %Y %Z").replace(tzinfo=timezone.utc)
            a = datetime.strptime(m_after.group(1).strip(), "%b %d %H:%M:%S %Y %Z").replace(tzinfo=timezone.utc)
        except ValueError:
            self.fail(f"GWA-A9 live RED: unrecognised openssl date format. out={out!r}")
        days = (a - b).days
        self.assertGreaterEqual(
            days, 3650,
            f"GWA-A9 live RED: cert validity {days} days < 10 years (3650). 决策22「≥10 年」.",
        )

    # GWA-E3 (live) -----------------------------------------------------------
    def test_e3_live_18789_bound_loopback(self):
        """coverage: GWA-E3 — OpenClaw 18789 只绑 127.0.0.1, 公网不可达 (V28 硬前置)."""
        out = self._ssh_ok(
            "ss -ltnp 2>/dev/null | grep 18789 ; true", "GWA-E3 live 18789 socket query")
        self.assertTrue(
            "127.0.0.1:18789" in out,
            f"GWA-E3 live RED: 18789 not bound to 127.0.0.1 loopback. sock={out!r} — V28 硬前置要求 "
            "OpenClaw 公网入口收敛.",
        )
        self.assertFalse(
            "*:18789" in out or "0.0.0.0:18789" in out or ":::18789" in out,
            f"GWA-E3 live RED: 18789 leaks to 0.0.0.0/* (public). sock={out!r} — V28 硬前置.",
        )


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(DirectionBStructuralAcceptance)
    if os.environ.get("DIRB_RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(DirectionBLiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    sys.exit(0 if result.wasSuccessful() else 1)