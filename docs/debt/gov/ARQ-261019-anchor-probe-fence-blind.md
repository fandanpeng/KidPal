---
id: ARQ-261019-anchor-probe-fence-blind
title: parseSection2AnchorLists 的 §2 边界探测不跳围栏——§2 内围栏块含 `^## ` 行会提前截断 §2，其后锚行与 T 条目全消失且报错误导（T3 误报缺锚、T4 条目静默消失不点名）
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一次动 `.claude/scripts/lib/governance-gate-shared.mjs` 锚解析面的治理批
opened: 2026-10-19
closed: ""
closedBy: ""
paths: [.claude/scripts/lib/governance-gate-shared.mjs]
source: [delivery/review/261019-gov-review-local-memory-extraction/PROPOSAL.md（§6 留根 + §5 V1 首跑 RED 实录）]
---

**证据**：261019-gov-review-local-memory-extraction 批起草期 preflight 首跑实测：T3 落地内容含 `## OpenClaw ops ...` 标题行并置入围栏块，`parseSection2AnchorLists`（`governance-gate-shared.mjs:363`）的 §2 边界探测（`:371-374`，以 `/^##\s/` 找下一节标题）不查 `fenceLineFlags`、把围栏内 `^## ` 行当节边界 → §2 提前截断 → T3 被误报「缺锚清单」、T4 整条目静默消失不点名。判官 R1 亲核源码证实（`:372-373`）。对照：锚行扫描段（`:375-384`）跳围栏属实——**同函数内扫描跳、边界探测不跳，两处行为不一致**（模板读法注第 1 条所称「已修复的跳围栏」仅指前者）。该批以「标题行行内钉死 + 围栏承载正文」形态规避落绿。

**影响面**：凡 PROPOSAL §2 用围栏承载含 markdown 节标题的落地内容（新节插入类改动是常见形态），必触发截断 + 误导性报错；起草者须付一轮形态规避返工（本批 T3 重写一次），且静默消失条目若未被其它检查接住 = 漏锚风险。

**建议改动**：§2 边界探测复用 `fenceLineFlags`（与锚行扫描段同机制），围栏内 `^## ` 行不作节边界；报错侧对「条目在 §2 截断后消失」增显式点名（候选，交 review、不自批）。

**风险**：riskTier: low——边界探测加围栏跳过是判定条件收窄，误改方向只会让少数真边界被多跳一行，既有锚行扫描逻辑不受影响。review profile: full-adversarial + human（触 G3 门脚本，如实标注）。
