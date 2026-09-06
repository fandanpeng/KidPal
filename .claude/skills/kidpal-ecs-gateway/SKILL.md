---
name: kidpal-ecs-gateway
description: KidPal ECS gateway server operations — which engine config is actually live (the docker bind-mount trap), how to read/edit/restart, the direction-B topology and ports, and the LLM/ASR/TTS config keys that bit us. Use for any server-side query, config change, or engine log read on the ECS host.
---

# KidPal ECS Gateway Ops

## Access
- All ECS commands go through **`c:/tmp/ssh_helper.py "<remote cmd>"`** (wraps SSH to the ECS host). Never print/persist secrets in transcripts.

## Direction-B topology (live)
- Device ⇄ **`wss://8.137.91.16:8000`** = `kidpal-front.service` (TLS + Bearer auth, the public edge). SG 8000 open.
- front ⇄ engine `xiaozhi-esp32-server` docker, **network_mode: host**, listens **`127.0.0.1:8010`** (ws) and `127.0.0.1:8003`.
- engine ⇄ **OpenClaw `127.0.0.1:18789`** (LLM brain, loopback-only — V28 invariant: never publicly reachable; SG closed).
- Retired: `kidpal-shim` (stopped+disabled), port 8080 (SG closed).

## THE config trap (biggest time sink)
The engine container bind-mounts `deploy/xiaozhi-docker-data` → container `/opt/xiaozhi-esp32-server/data`. So the **LIVE config is:**
```
/opt/kidpal/xiaozhi-esp32-server/deploy/xiaozhi-docker-data/.config.yaml
```
NOT `/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server/data/.config.yaml` (that one is a stale decoy with empty ASR creds). Confirm the real mount with:
`docker inspect xiaozhi-esp32-server --format '{{range .Mounts}}{{.Source}} -> {{.Destination}}{{println}}{{end}}'`
If the config you're reading contradicts observed behavior (e.g. shows Aliyun ASR but ASR works), you're reading the decoy.

## Read / edit / apply
```
python c:/tmp/ssh_helper.py "sed -n '1,40p' /opt/kidpal/xiaozhi-esp32-server/deploy/xiaozhi-docker-data/.config.yaml"
# edit: cp to .bak-<date>-<why> first, sed -i, then:
python c:/tmp/ssh_helper.py "docker restart xiaozhi-esp32-server"
```
A config edit needs `docker restart` to take effect. Verify reload via startup logs (`docker logs --since 15m ... | grep 初始化组件`).
`docker ps` "(unhealthy)" is a cosmetic healthcheck-definition artifact (exit 1, empty output) — engine is fine if it's listening + initializing providers.

## Config keys that bit us (live values)
- `selected_module.LLM: OpenClawOpenAI`, `type: openai`, `base_url: http://127.0.0.1:18789/v1` (**`/v1` is required** — without it the openai SDK hits 404 "Not Found"), `model_name: openclaw/default`.
- `selected_module.Intent: nointent` — **must be nointent, not function_call.** function_call sends the tools array (device MCP tools + server tools) to OpenClaw, which rejects it: `LLM stream processing error: invalid tool configuration` → device shows fallback「小智现在有点忙」. nointent = plain streaming chat (correct for the smoke).
- `selected_module.ASR: Qwen3ASRFlash` (`type: qwen3_asr_flash`, dashscope key); `TTS: EdgeTTS` (`type: edge`, NOT `edge-tts` — the value must match the engine provider module filename).

## Reading the engine log for a round
```
python c:/tmp/ssh_helper.py "docker logs --since 6m xiaozhi-esp32-server 2>&1 | grep -iE '识别文本|大模型|发送第一段语音|语音生成成功|invalid tool|error' | tail -40"
```
Good round = `识别文本:` (ASR) → `大模型收到用户消息` → `发送第一段语音` + `语音生成成功` (TTS), no `invalid tool configuration`.

Related: [[kidpal-serial-monitor]] [[kidpal-nvs-ops]] [[kidpal-live-smoke-debug]]
