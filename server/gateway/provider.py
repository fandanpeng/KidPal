"""Pluggable gateway provider adapters (决策18 / 决策19 / 决策23 / GWA-A4 / GWA-E4).

Frozen acceptance GWA-A4 + GWA-E4. Pure-python, no fastapi/edge_tts/httpx at import
time — network deps are imported lazily inside functions so the offline contract stays
runnable anywhere.

Frozen offline contract:
    FEATURE_STREAMING_FORBIDS_MAX_TOKENS = True
    openclaw_stream_request(messages) -> dict payload that NEVER sets "max_tokens"

Hard constraint (contract §四 / 决策18 / V9): the streaming request to OpenClaw MUST
NOT carry max_tokens —实测触发 agent incomplete-turn 报错. openclaw_stream_request
therefore never adds that key.

This module is also the additive provider base (GWA-E4): a small registry + dispatch
switch so 包7/(提醒投递) and 包8/(网关累积) can register new adapters without editing the
core three. The deployed ECS xiaozhi config (server/deploy/) maps these to the
official config.yaml provider blocks: LLM type:openai->OpenClaw, TTS=edge-tts,
ASR=阿里云云端(aliyun streaming).
"""

from __future__ import annotations

import os
import uuid
from enum import Enum

# 决策18/决策23: OpenClaw loopback base (V28 锁 127.0.0.1 —— 公网入口收敛). The
# OFFICIAL xiaozhi-esp32-server lives at /opt/kidpal/xiaozhi-esp32-server; the OpenClaw
# brain is an OpenAI-compatible endpoint bound to 127.0.0.1:18789.
OPENCLAW_BASE_URL = os.environ.get("OPENCLAW_BASE_URL", "http://127.0.0.1:18789")
OPENCLAW_MODEL = os.environ.get("OPENCLAW_MODEL", "openclaw/default")

# 契约 §四 / V9 硬约束 —— 流式请求禁带 max_tokens.
FEATURE_STREAMING_FORBIDS_MAX_TOKENS = True

# Required provider family (决策18 / 决策19 / GWA-A4). Declared here so the pluggable
# registry is the single additive surface 包7/包8 extend.
REQUIRED_PROVIDER_FAMILIES = ("llm", "tts", "asr")


class ProviderKind(Enum):
    """Additive provider kind registry (GWA-E4: 新增 provider 不改核心)."""

    LLM_OPENCLAW_OPENAI = "llm.openclaw.openai"   # type:openai 流式接 OpenClaw
    TTS_EDGE_TTS = "tts.edge-tts"                 # edge-tts 下行
    ASR_ALIYUN_CLOUD = "asr.aliyun"               # 阿里云云 ASR (流式 + VAD 切句)


# additive registry: name -> ProviderKind. 包7/包8 只 append 新条目, 核心 dispatch
# switch 不因新增 provider 改写.
PROVIDER_REGISTRY = {
    "llm": ProviderKind.LLM_OPENCLAW_OPENAI,
    "tts": ProviderKind.TTS_EDGE_TTS,
    "asr": ProviderKind.ASR_ALIYUN_CLOUD,
}


def corr_id() -> str:
    """A Correlation-Id for the request path (K6 structured logging)."""
    return uuid.uuid4().hex


def provider_config(family: str) -> dict:
    """Declared provider config for one family (provisioner shim).

    Consumed by the deploy layer to render the xiaozhi config.yaml provider blocks.
    Credential-bearing fields are filled from env at runtime, never hard-coded here.
    """
    kind = PROVIDER_REGISTRY[family]
    if kind is ProviderKind.LLM_OPENCLAW_OPENAI:
        return {
            "type": "openai",
            "base_url": OPENCLAW_BASE_URL,          # http://127.0.0.1:18789
            "model": OPENCLAW_MODEL,
            "api_key": os.environ.get("OPENCLAW_TOKEN", ""),
            "stream": True,
            "forbid_max_tokens": FEATURE_STREAMING_FORBIDS_MAX_TOKENS,
        }
    if kind is ProviderKind.TTS_EDGE_TTS:
        return {"type": "edge-tts", "voice": os.environ.get("TTS_VOICE", "zh-CN-XiaoxiaoNeural")}
    if kind is ProviderKind.ASR_ALIYUN_CLOUD:
        # 阿里云云 ASR (决策19 云优先). Credentials (appkey/token/api_key) come from
        # env at runtime; unset => live ASR pending human credential provision.
        return {
            "type": "aliyun",
            "credential": {
                "appkey": os.environ.get("ALIYUN_ASR_APPKEY", ""),
                "token": os.environ.get("ALIYUN_ASR_TOKEN", ""),
                "api_key": os.environ.get("DASHSCOPE_API_KEY", ""),
            },
            "vad_compat_required": True,  # V23: 候选流式 + VAD 切句语义兼容性实测
        }
    return {}


def openclaw_stream_request(messages) -> dict:
    """Build the OpenClaw streaming request payload (OpenAI-compatible).

    NEVER sets "max_tokens" (contract §四 / V9): that key is known to trigger the
    OpenClaw agent incomplete-turn error. The gateway sends the full message history
    to the loopback OpenClaw brain and lets the agent steer its own output length.
    """
    return {
        "model": OPENCLAW_MODEL,
        "messages": list(messages or []),
        "stream": True,
    }