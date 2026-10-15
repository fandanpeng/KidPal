# pod-retro · 261001-openclaw-model-name-overlay-drift（第一人称热记忆）

> planner 全程在场的**过程体感**。非第二事实源：不复述 tokens / gates / rounds / verdict / test 结果——那些以 machine artifacts / pod-events / verifier-evidence 为权威。标签：第一人称热记忆。

## 起包期最大的价值点：1f 快照保鲜核验

plan、启动卡、ARQ **三处**都写「新增 ADR-007」，但 `docs/ADR.md` 里 ADR-007 早已被 `260924-m4-k1-story-cloud-link` 占掉（星星记录本数据面收口）。这是典型的「拆包文稿比磁盘现实老」——拆包到起包间隔并不长，坏的是**跨包共享的序列号**被别人用掉了。如果我照文稿放行，integrator 会照 plan 落一个重号 ADR，收口时才炸。1f 那条「逐字段对磁盘核验」的纪律，本次实打实拦下一次腐坏引用。**下次改进**：1f 核验清单可补一条「跨包共享序列号（ADR 号、changeId 段号、端口号等）是否已被他包占用」。

## architect 1g 预评补捞一处悬点，暴露我的 grep 方法缺陷

我起包时的全仓 grep 带了一串 `--include`（.py/.sh/.yaml/.md/.json…）去找 `openclaw/default` 悬点，**漏了 `.service` 扩展名**，于是没看见退役 shim 的 systemd 单 `server/kidpal-shim.service`。architect 1g 一眼补上。教训很具体：**做「零残留」类扫描时，`--include` 白名单本身就是漏面源头**——应当先无 include 穷尽、再分类 carve-out。architect 还顺手把 carve-out 措辞从我一刀切的「历史变更记录类文本」精修成「退役面」（shim.py / kidpal-shim.service / archive/*）与「历史记录」（PERF-K1-ACCEPTANCE-NOTES.md / ADR-006 正文 / crq 注）两类，比我的分法准。

## 冻结验收件内 plan 未点到的残留

plan 只点了 PERF-K1 验收件的 `:916/:938`（值）与 `:917/:939`（文案），漏了 `:929` 的 docstring 也含旧值。又是同一个病根：**「重钉/改写某文件若干处」的任务书，逐处枚举不能信文稿的下标清单，必须与实文件字面复核**。

## verifier 绕轮：元数据地雷的第二形态

首轮被一条 `mutation-check` 机械牙拦下——`mutationRequired=false` 未带 `mutationSkipReason`。我的既有 memory 只记了「`mutationRequired=true` 须 `mutationEvidence`」这一半，**这是同一族地雷的另一半**（false 侧须 skip 理由）。纯 planner 侧元数据补齐、非代码缺陷，没派 fixer。属于「起包格式期望前置」清单可直接吸收的一行。

## 帮上忙 / 帮倒忙的钩子与规则

- **帮上忙**：`verify.mjs` 的 final 相位缺 `--scope-file` 直接 fail-closed 举手——迫使我正确地把 final 终验交给 integrator 的圈定步骤，而不是在 verifier 段硬跑全量。这个 fail-closed 设计对。
- **帮上忙**：`taskpackage-required-check` 的 canonical ↔ current/ 指针一致性牙——我每次改了 canonical 忘刷 current/，它都会立刻点破，没有静默假绿。
- **略绕**：`current/` 是隐性全局指针，跨包接力时（本树上一包 `261001-m4-k3-story-archive-order` 遗留）要手工字节级刷 taskpackage + plan + 两份 brief 四件，纯手工、易漏。本包刷全了，但这是值得留意的重复劳动面。

## 人工纠偏体感

全程无 human 入场——低风险 Lite、无契约面、无门冲突，符合「loop 内非 blocking 不请示，human 只花时间在升级门/契约/真机/预算」的口径。这也意味着本包没有 human 纠偏体感可记。

## escalation themes

- 无 `agent-escalation` 事件。
- 三条 friction：ADR 号腐坏（`scan-miss`，事件 `169eed22`）、acceptance-author 超读申报（`over-scan`，事件 `886ec498`）、`mutationSkipReason` 缺失（`scan-miss`，事件 `eaf4ec10`）。前两条同源于「拆包文稿 vs 磁盘现实」的老问题族。

## 下次改进（三条）

1. 「零残留」类扫描：先无 include 穷尽 grep，再分类 carve-out；白名单 `--include` 是漏面。
2. 「重钉 N 处」任务书：逐处与实文件字面复核，不信任文稿的「N 处」下标。
3. 起包 1f 核验清单补「跨包共享序列号占用」一项；并把「`mutationRequired=false` 须 `mutationSkipReason`」写进起包格式前置自查。
