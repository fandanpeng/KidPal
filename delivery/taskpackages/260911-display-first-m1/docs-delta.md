# docs-delta — 260911-display-first-m1（PKG-4 显示先行 M1 · 围栏闭合即发）

> integrator docs-sync 结果清单 · 2026-09-12 · 分支 `dev/260911-display-first-m1`（base = main `438f85a`）。
> 性质：引擎补丁内部面（`server/deploy/engine-patch`）+ 冻结验收件修订包（subsystem=server+firmware，rigor=full，riskTier=high，gateProfile=frozen-acceptance-revision/device-round/contract-drift，mergeOrder 20）。
> `docsSyncRequired=false`。**核心结论：canonical 面（`docs/contracts/**`、`docs/spec.md`）与全部 docs 面零改动**；
> 本包实现侧注记随代码注释与 changes/dev closeout 制品承载。

## A. docsSyncRequired=false 判定（integrator，承 taskpackage.docsSyncReason）

本包为引擎侧「块闭合从 finish() 提前到 feed() 内」的发射时机改动，零 docs 面改动：

- **spec §4.0/§5.3 零修订**：OQ2=M1 只改引擎内部 block 发射时机，不触 spec 页面/交互语义（`.planning/260911-display-interaction-sovereignty/PROPOSAL.md §2.2/§3/§7——SPEC-1 本批不触发，spec 零修订走 OQ4=A / OQ3=c 已裁）。
- **契约面零触碰**：`docs/contracts/**` 只读、未改。开工前 reviewer-first 契约复核结论 = `CONFIRM_NO_CONTRACT_CHANGE`（`.claude/taskpackages/260911-display-first-m1/evidence/DF-E1-contract-recheck.md` 总裁决）：M1 不改 E1 契约设备面的 view schema／mcp 信封／工具白名单／校验权威四个面；`kidpal_display_blocks` 侧通道为引擎→front 内部实现面（契约全库 grep 零命中）。
- **无新增约定文档面**：无新 view、无字段增删、无协议/页面语义变化——本包只改「引擎剥离出块后何时发射」，块仍位于回复文本末尾（§1.1「末尾插入」约束对象 = LLM 回复文本物理位置，非网关两路下发墙钟时序），每块仍经 front 逐块机械校验 + `kidpal/display` mcp 信封下发，设备 wire 三面不变。

## B. docsSyncReason 逐字（taskpackage.json）

> `docsSyncRequired=false`，`docsSyncReason`：「无实现侧文档同步需求：spec 零修订（OQ2=M1 只改引擎内部 block 发射时机，不触 spec §4.0/§5.3）、docs/contracts/** 零触碰（DF-E1 契约复核 CONFIRM_NO_CONTRACT_CHANGE）；实现仅改 server/deploy/engine-patch 内部面（strip machine 闭合回调 API + connection 接线），front/固件复核零代码变更，不产生 docs 面变更。」

## C. 实现侧注记承载处（本包未改 docs 面，注记随实现代码注释）

| 承载处 | 注记 | 消费方 |
|---|---|---|
| `server/deploy/engine-patch/kidpal_display_strip.py` 头注释 | frozen offline contract 增补 `__init__(on_block_closed=None)` 语义段（optional closure callback / 不变量「回调载荷序列 == finish().blocks」/ 发射信道唯一） | 冻结离线契约（实现面自注释，与 acceptance 件头同义） |
| `server/deploy/engine-patch/kidpal_display_strip.py:54-61` | `_on_block_closed` 成员 + `None = pre-M1 behaviour, byte-identical` 向后兼容注释 | 实现面自注释 |
| `server/deploy/engine-patch/connection.py:1168-1178` | `_emit_display_block` M1 注释（closed fence 在 feed 内 closer 被消费瞬间发射、单块包装 `[block]`、`finish().blocks` 只作观察镜像不重复发射） | 实现面自注释 |
| `server/acceptance/test_g_relay_fix.acceptance.py:185-243` | REVISION LOG [1]-[3]（additive 语义扩展边界 + 语义不放松自证 + DF-A4 承载映射） | 冻结验收件（REVISION LOG 留痕） |

## D. 明确没动的面（及为什么）

1. **`docs/spec.md` 零改动**：OQ2=M1 裁定 spec 零修订（`taskpackage.decisionCoverage` 条目「SPEC-1 本批不触发」由 PROPOSAL §7 承载，未在本包 decisionCoverage 落点——spec 面本批恒不触发）。实现中若发现与 spec 冲突走逃生门（Track B spec-lane），本包全程零触发。
2. **`docs/contracts/**` 零改动**：只源锚引用 `interface-contract.md`，不改契约文字。DF-E1 reviewer-first 复核 `CONFIRM_NO_CONTRACT_CHANGE` 在案，无四分类 + human 契约门需要。`git diff --name-only -- docs/` 全空（见 §E 机器核据）。
3. **front / 固件零改动留根**：front 逐块中继（`server/gateway/mcp_relay.py` `kidpal/display` relay）已是现状、M1 只改引擎侧发射次数与时机（DF-E1 边界注记 3：幂等/顺序/回执属实现面自检、每块已带独立数字 id）；设备侧「语音中渲染」由已并入 base 的 PKG-2 layer 仲裁规则承载（`firmware/main/test/screen_unification.acceptance.c` b1/b3/s2）。两者均零代码变更，不产生 docs 面。

## E. 机器核据（integrator 本相位实跑）

```
git diff --name-only -- docs/                 → 空输出（canonical docs 零触碰，实测）
git diff --name-only -- server/gateway/       → 空输出（front 零触碰，实测）
git diff --name-only -- firmware/             → 空输出（固件零触碰，实测）
contract-lint 闸 r5 ok:true                   → 闸对象 docs/contracts/interface-contract.md
docs-sync 闸 r5 ok:true                       → docsSyncRequired=false + docsSyncReason 在场
node .claude/scripts/closeout-check.mjs       → 见 review-packet §Closeout（本相位实跑）
```

> 本包 docs-delta 结论 = **零 docs 面改动**，与 `docsSyncRequired=false` 一致，无遗留同步项。
> DF-E1 边界注记 2 提出的 §1.1「『末尾』指块在回复文本中的物理位置、不约束墙钟时序」措辞澄清建议为 INFO 级、**不在本包落地**（契约面零触碰铁律），留给未来独立契约变更走 architect-reviewer 四分类 + human 契约门。