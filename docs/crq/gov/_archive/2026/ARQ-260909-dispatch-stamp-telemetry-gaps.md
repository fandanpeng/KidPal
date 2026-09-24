---
id: ARQ-260909-dispatch-stamp-telemetry-gaps
title: dispatch-stamp 缺「实际生效模型 + token 分项」且主线程自身零观测，致 retro 硬项 R2 无法诚实完成
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-09
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-dispatch-telemetry-source **三条子项 a / b / c 全收**——取件复证实测发现三者由**同一个机制**解决：读 Claude Code 会话 transcript（`<projects>/<slug>/<sessionId>/subagents/agent-<agentId>.jsonl` 逐消息 `message.usage` 四路 + `message.model` 生效模型）。**逐子项实况（两条与原描述不符，如实记录）**：a = 批① 落的只是「请求模型」旗标、全量 199 条事件里 `model` 字段使用 **0 次**；**b = 按 ARQ 原建议「分项入 payload」改是改不出来的**——旗标面（`--token-input`/`--token-output`/`--token-cache-read`/`--token-cache-write`/`--token-total`）、`planner.md` 调用行、脚本校验**三处早已在位**，真缺口在**上游数据源**（harness 只回单值），脚本侧再改也补不出分项；c = 原「估算 vs 不采」的真设计分叉**被第三条路溶解**（主会话 transcript 可直接读，既非估算也非放弃）⇒ ARQ 对 c 的「转 full-adversarial + human」触发前提不再成立，**human 立项时据此改判全批走 Track B**。落点：T1 新增 `.claude/scripts/lib/transcript-telemetry.mjs`（去重按 `message.id`——同一次 API 响应的 usage 按 content block 落多遍；谓词钉死顶层 `o.type`，`message.type` 恒为 `"message"` 误取即静默得 0；四条硬约束 = 降级不抛 / 路径可覆盖 / 无模型白名单 / 失败不阻断）、T2 `dispatch-stamp.mjs` 接入（命中置 `status:'exact'` + `source:'transcript'`——`status` 是既有消费方唯一分桶键，缺则覆盖度静默不涨且无门报红）、T3 `planner.md` 钉死 `--dispatch-id` 必传 harness agentId（连接键）、T4 token 政策按来源分档（原句四条实质约束逐字保留）、T5 `pod-events-aggregate.mjs` 增 `models` / `mainThread` 两列（只增键）。**评审 R1 打回（3 SHOULD + 3 COULD + 2 NIT）→ v0.2 全折入 → R2 GREEN**；**核验首轮 FAIL（1 RED：主线程侧去重夹具为空壳，最小复现 = 移除去重仍 15/15 全绿）→ 补夹具 + 复证移除即变红 → 复核 PASS**。**本批首次给出真实 token 分项，并立即暴露通道自身一处限制**（typed agent 下 `output_tokens` 恒 0 且 `stop_reason` 恒 null ⇒ `total` 退化为 input 却仍写作完整值）——该限制**另立 ARQ 待批，本批不热修**。landing-verdict PASS（含 4 ADVISORY + 1 RED 闭环）、commit `25a3340`。retro 见同目录（**本仓首个带真实 token 分项的 retro**）。证据：[260920-gov-review-dispatch-telemetry-source](../changes/arch/260920-gov-review-dispatch-telemetry-source/)。
