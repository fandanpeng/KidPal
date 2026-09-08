"""KidPal direction-B streaming-gateway adaptation package (server side).

This package is the *shared server base* (GWA-E4) for TaskPackage
260903-gateway-directionB. It carries the frozen, pure-python, offline-importable
adaptation modules that the deployed xiaozhi-esp32-server gateway wires in:

  auth.py          forced device auth (决策22 / V11 / V26)
  display_schema.py mechanical ```display schema validation (V27)
  provider.py      pluggable provider adapters (openai->OpenClaw / edge-tts / aliyun)
  fallback.py      gateway-side fallback generation (4s soothing / error / V32 truncate)
  mcp_relay.py     kidpal/display & kidpal/alert JSON-RPC tools/call relay (决策23)
  reminders.py     提醒投递面 (260829-schedule-reminder): pending.json 按分钟读 + 机械校验 +
                   到点经 kidpal/alert 推送 + 重试到确认 60s×3 + 过期静默作废 + 渲染回执出队
                   + kidpal_speak 引擎腿注入 (裁定二)。additive 扩展，不改动上述模块语义。

Contract of record: docs/contracts/interface-contract.md §四 (端云协议剖面).
Every module below is pure-python — no fastapi/edge_tts/httpx at IMPORT time —
so the frozen offline acceptance (server/acceptance/test_gateway_directionB.acceptance.py)
stays runnable on any box year-round.

Structured logging carries Correlation-Id (K6) — see server/gateway/log.py.
"""

__version__ = "0.1.0"