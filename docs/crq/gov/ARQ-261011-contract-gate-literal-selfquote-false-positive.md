---
id: ARQ-261011-contract-gate-literal-selfquote-false-positive
title: 前置升级闸 contract-findings 字面扫描在「自述无 finding」行上误中——review-packet 复述闸字面集的合法行被当成 finding 拦合并
domain: gov
status: deferred
severity: COULD
owner: architect（闸判据修订）+ dev-loop（脚本落地）
trigger: 下次治理批 / 下个动 pre-merge-escalation-gate.mjs 的批；两次合并（261010 两包）各拦一次，人批凭据已留 pod-events
opened: 2026-10-11
closed: ""
closedBy: ""
paths: [".claude/scripts/pre-merge-escalation-gate.mjs"]
source: 261010-device-reconnect 合并链实测（2026-10-11，主线程）：review-packet :47「**无** hard-violation finding——如出现将以 docs/contracts/** / docs/requirements.md / docs/architecture.md 字面前缀起引 Basis；本轮 none」被行级扫描误中为 1 条 hard-violation，BLOCK 拦合并，经 human accept-risk 锚放行（pod-events 13baa956）
---

**定性：治理机检误报（闸自身缺陷），非流程违规**。261010 批两次合并两次触闸：第一次（prompt-v9）是真 finding（arch 指针漂移）缺记账——闸正确；第二次（device-reconnect）是 packet 自述「无 finding」并顺带复述闸的字面集写法（`CONTRACT_SPEC_LITERALS` 起引格式说明），行同时含 `hard-violation` 字样与字面集 → `contractFindingsAccounted` 行级过滤（pre-merge-escalation-gate.mjs:146-149）无否定/引语判别 → 误中。代价 = 多一道 human 门 + pod-events 记账噪声（两次 accept-risk 锚里有一次是为误报落的）。

**修法候选**：① 行级过滤加否定判别（同一行含「无 hard-violation」「none」等自述标记 → 跳过）；② 或契约面 Basis 判据改为「行含 hard-violation 且字面集出现在 `Basis` 列位（管道表第 5 列）」列位级解析，而非整行任意位置；③ 或约定侧收紧——review-packet 模板禁止在非 finding 行复述字面集（改 integrator 写作约定，联动 `.claude/agents/integrator.md` Basis 前缀写作约定同批）。①/② 为机械根治、③ 为约定规避，倾向 ①+③ 双保险。**riskTier: low**。
