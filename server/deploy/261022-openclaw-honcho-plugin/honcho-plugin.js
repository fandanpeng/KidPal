/**
 * KidPal Honcho memory plugin — gate carrier（261022-m5-k3-honcho-memory-production）
 *
 * 职责（arch §6.7 资格门 + K3-E5 降级护栏 + ARQ-260907 同族报警锚）：
 *   1. 资格门（读写同门）：仅请求携 user=<device_id> 的 kidpal 语音回合可读写
 *      记忆；user 缺席即非资格（front 一次性回合 / main agent / cron 结构性
 *      不入画像——白名单入册，不做黑名单剔除）。
 *   2. peer 映射（不另造键形）：user=<device_id> → agent:kidpal:openai-user:<device_id>
 *      （消费 P2 持久会话键族；device 级 peer 先行）。
 *   3. 可达性探针门控：只有 Honcho /health 探针通过才打 marker
 *      「kidpal honcho memory patch active」；探针失败 = error 级报警 + 降级
 *      （不打 marker——插件装了但 Honcho 不可达时静默照打 = 假绿报警锚，K2b 门控先例）。
 *   4. 降级护栏（K3-E5）：persist / inject 路径 bounded 失败（try/catch，单次
 *      尝试、无重试循环）——Honcho 不可达 → 本轮无画像注入、对话照常、不阻塞、
 *      记忆失败不进用户可见兜底。
 *
 * 形态注记（冻结契约 §B4「形态自由」）：本件为插件包装补丁 JS，随
 * honcho-plugin-declaration.json 的 agents.entries.kidpal.plugins.honcho 声明
 * 经 apply_honcho_plugin.py 定点注入 openclaw.json（baseUrl / peerPrefix /
 * gate 三键为声明面，运行时键形以部署轮插件加载观测为准）。
 */

"use strict";

const HONCHO_BASE_URL = "http://127.0.0.1:18100";
const PEER_PREFIX = "agent:kidpal:openai-user:";
const GATE_MARKER = "kidpal honcho memory patch active";
const PROBE_TIMEOUT_MS = 3000;

/** Honcho 可达性探针（/health）；探针失败只降级报警，不抛出。 */
async function honchoReachable() {
  try {
    const res = await fetch(HONCHO_BASE_URL + "/health", {
      signal: AbortSignal.timeout(PROBE_TIMEOUT_MS),
    });
    return res.status < 500;
  } catch (err) {
    console.error(
      "[kidpal-honcho] ERROR honcho unreachable (" +
        (err && err.message ? err.message : err) +
        ") — degrade: no persist, no inject, marker withheld"
    );
    return false;
  }
}

/**
 * 资格门（唯一判据）：请求携 user=<device_id> 才有资格；缺席 → null。
 * 读写同门：非资格回合既不写 observation 也不注入画像。
 */
function qualifiedPeer(session) {
  const user = session && session.user;
  if (!user) {
    return null; // front 一次性回合 / main / cron：结构性不入画像
  }
  return PEER_PREFIX + user; // agent:kidpal:openai-user:<device_id>
}

/** 启动自检：探针通过才打 marker（机械报警锚，缺失 = 部署轮 grep 可见）。 */
async function init() {
  const ok = await honchoReachable();
  if (ok) {
    console.log(GATE_MARKER + " baseUrl=" + HONCHO_BASE_URL + " peerPrefix=" + PEER_PREFIX);
  }
  // 探针失败：error 已在 honchoReachable 内打出，marker 不打（降级运行）。
}

/**
 * persist 腿（openai-http 钩子，hooks.allowConversationAccess=true 后触发）：
 * 资格回合才入库；bounded 失败——单次尝试，异常只报警、不影响对话主腿。
 */
async function persist(session, messages) {
  const peer = qualifiedPeer(session);
  if (!peer) {
    return; // 非资格：零写入
  }
  try {
    await fetch(HONCHO_BASE_URL + "/v1/openclaw/persist", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ peer, messages }),
      signal: AbortSignal.timeout(PROBE_TIMEOUT_MS),
    });
  } catch (err) {
    console.error(
      "[kidpal-honcho] ERROR persist failed (" +
        (err && err.message ? err.message : err) +
        ") — bounded: single attempt, conversation continues"
    );
  }
}

/**
 * inject 腿（before_prompt_build 钩子）：仅资格回合注入画像；失败 → 返回空
 * 注入（本轮无画像、回复照常），不进用户可见兜底。
 */
async function inject(session) {
  const peer = qualifiedPeer(session);
  if (!peer) {
    return null; // 非资格：零注入（读侧同门）
  }
  try {
    const res = await fetch(HONCHO_BASE_URL + "/v1/openclaw/inject", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ peer }),
      signal: AbortSignal.timeout(PROBE_TIMEOUT_MS),
    });
    if (!res.ok) {
      console.error("[kidpal-honcho] ERROR inject http " + res.status + " — degrade to empty injection");
      return null;
    }
    return await res.json();
  } catch (err) {
    console.error(
      "[kidpal-honcho] ERROR inject failed (" +
        (err && err.message ? err.message : err) +
        ") — degrade to empty injection, reply unaffected"
    );
    return null;
  }
}

module.exports = { init, persist, inject, qualifiedPeer, honchoReachable };
