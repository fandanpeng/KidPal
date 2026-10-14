---
name: "qqbot-reply-diagnosis"
description: "Diagnose QQ Bot messages with no reply when channel shows connected: dm_policy drops, plugin/core mismatch, billing cooldown."
---

# Diagnose QQ Bot "no reply" failures

The QQ channel shows `connected` but user messages get no reply. Locate which layer dropped the message by reading the gateway log, then apply only the matching fix. Do not change configuration before identifying the layer.

## Steps

1. Confirm channel state: run `openclaw channels status`. It must report `connected`. If it does not, fix connection first (appId/clientSecret, token, WebSocket link) before anything else.

2. Find the message's fate in the gateway log:
   `grep -E "Processing message|Blocked|Dropped|Message processor error|channel exited|model-fetch.*status=" /tmp/openclaw/openclaw-*.log | tail -20`
   The sender's message appears as `Processing message from <openid>`. The lines right after it name the failing layer.

3. `Blocked qqbot inbound: ... dm_policy_not_allowlisted` / `Dropped inbound qqbot message` means the DM policy rejected the sender. The QQBot plugin schema forbids `"*"` inside `allowFrom` (only openids or `openclaw:approval-disabled` are accepted), while `dmPolicy: "open"` requires `"*"` — an unconfigurable combination that drops every DM. Fix: set `channels.qqbot.dmPolicy` to `allowlist` and add the sender's openid to `channels.qqbot.allowFrom`, then restart the gateway (`systemctl restart openclaw-gateway`).

4. `Message processor error: ... formatInboundEnvelope is not a function` or `channel exited: ... ingress queue is unavailable ... origin "global"` means the qqbot plugin version does not match the core runtime, or the plugin came from an untrusted source. Fix: find the official source with `openclaw plugins search qqbot`, reinstall it (`openclaw plugins install clawhub:@openclaw/qqbot --accept-capabilities` or the provider's official package), disable any older conflicting plugin so exactly one owns the `qqbot` channel id (`openclaw plugins disable qqbot`), then restart the gateway.

5. `Provider <name> is in cooldown (billing)` or model-fetch responses that are not `status=200` mean the model provider is out of credit — not a channel fault. Tell the user to top up; replies resume once the provider accepts calls again. Verify with a fresh model call returning `status=200`.

6. Repeated `stalled session: ... sessionKey=agent:main:qqbot:direct:<openid> ... reason=blocked_tool_call ... activeTool=ask_user` with no `Sent markdown chunk` in the same window means the agent asked a clarifying question through `ask_user`, but this plugin cannot deliver that prompt: the run blocks until the 900 s timeout and every later user message queues behind it. The user sees silence, not an error. Fix: get the answer from another surface (the main/webchat session), apply it to the artifact yourself, then hand the decision to the blocked session (`sessions_send` with the clarification and "do not call ask_user again"); restarting the gateway also clears the stuck run. Because the prompt never arrives, the user cannot answer on the QQ side — resolve the ambiguity from the main session instead of waiting.

7. Verify end to end: ask the user to send a new message, then confirm the log shows `Processing message` → a successful model call (`status=200`) → `Sent markdown chunk` / `onMessageSent` for that openid. Only then report success.

## Notes

- Restarting the gateway interrupts the current session; expect the tool call to be aborted and resume after restart completes.
- Keep `channels.qqbot.groupAllowFrom` as-is when only DMs are affected.
