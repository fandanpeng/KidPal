---
id: ARQ-260924-landing-summary-template-pointer-gap
title: landing-summary 模板指针在 landing 相位文面缺位——协议七步第 4 步只说「写 architect-landing-summary.md」不指模板，主线程手写漏 landing-manifest 围栏块致核验 FAIL（unit-gate F4 同根第二实例，模板早已固化该教训）
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 已结（2026-09-25，260924-gov-review-gate-parse-teeth T8 全落，含可选加固句）
opened: 2026-09-24
closed: "2026-09-25"
closedBy: 260924-gov-review-gate-parse-teeth
paths: [.claude/references/arch-review-protocol.md]
source: 260924-gov-review-planner-orchestration-align（landing-verdict.md 首轮 FAIL + ADVISORY-1；retro §3-F2/§4-2）
---

**证据**（2026-09-24，260924-gov-review-planner-orchestration-align 批 landing 相位实测）：① 本批主线程**手写** landing-summary（未复制模板），缺 ```landing-manifest 围栏块——landing-verifier 首轮 verdict **FAIL**（failingFile = architect-landing-summary.md；category = structural-format-missing-manifest；SSOT = `.claude/references/architect-operating-model.md:122`「frozen PROPOSAL 必含…；architect-landing-summary 同含其块」）。② **同根第一实例早已在案且教训已固化进模板**：`docs/templates/architect-landing-summary.template.md:6` 形制来源自注「unit-gate F4（缺 `landing-manifest` 围栏块 → B2 RED…）教训固化」，模板 :30 即含该块——本批是**模板已修好的坑再踩一次**，因为 landing 时没人看模板。③ 指针缺位实测：`.claude/references/arch-review-protocol.md:58`（Track B 七步第 4 步）逐字「主线程 **packet-bound 落地**（G1/G2/G3 全落，landing-manifest 绑定，落地后不可回改 PROPOSAL）+ 写 `architect-landing-summary.md`；」——**无模板路径指针**（对照：第 1/6 步对 PROPOSAL 与 retro 均带「骨架见 docs/templates/…」指针）。④ 评审期无门可兜：`track-b-judge-brief.md:33` 第 7 条只核 **PROPOSAL** 侧 manifest 块（summary 在评审期尚不存在，结构上不可能核）；landing-fidelity-check 主模式才查双侧——即该判据的第一道覆盖在核验期，天然晚一个相位。

**影响面**：凡 Track B 批 landing 相位主线程手写 summary 的场合均可能复现（本批 = 同根第二实例）；代价 = 一次核验返修往返（本批实测 ≈7 min + 132K tokens + 一轮 verifier resume），结构门 fail-loud 兜底故不致静默放行，但 FAIL→修→复验的往返本可零成本避免。

**建议改动**（候选，交 review，不自批）：① 最小修 = `arch-review-protocol.md` 双轨路由第 4 步文面补模板指针——「写 `architect-landing-summary.md`（**骨架 = `docs/templates/architect-landing-summary.template.md`，复制勿手写；五件组件缺一即结构门/核验官合法打回**）」，与第 1/6 步的 PROPOSAL/retro 指针同款形态；② 可选加固 = `track-b-verifier-brief.md` 或 landing 相位 checklist 文面同句提醒（防协议文面被跳读）；③ 不新增门（结构门已在且 fail-loud 正确工作，方向是防错前移不是加闸）。

**风险**：riskTier: low（G2 文面一句指针，零门判定语义变更）。**review profile**：Track B 三要件逐条 = ① 执行测出 yes（本批 landing-verdict FAIL 实录 + unit-gate F4 先例）；② 问题清晰且修复明确 yes（补指针句，与既有第 1/6 步形态对齐、无真设计分叉）；③ 半径已被执行揭示 yes（一次返修往返实测在案）。G 层事实：G2 = `.claude/references/arch-review-protocol.md`（可选并 `track-b-verifier-brief.md`）。机检硬排除逐项：不触契约面、非 gate 变更（③ 明文不新增门）、不触 self-def / review-loop agent def（brief 属 references 非 agents）、不改 frontmatter。便宜验证计划：落地后 `landing-fidelity-check.mjs` 对本批复跑仍绿（指针句不触任何解析面）+ 下一 Track B 批 summary 以模板复制起步的结构门首跑绿为实效证。owner=architect。

**批 C 起草期实测复证（2026-09-24，260924-gov-review-gate-parse-teeth 起草前）**：处方落点仍在——arch-review-protocol.md:58 第 4 步现文逐字「4. 主线程 **packet-bound 落地**（G1/G2/G3 全落，landing-manifest 绑定，落地后不可回改 PROPOSAL）+ 写 `architect-landing-summary.md`；」无模板指针；对照 :55 第 1 步（「骨架见 `docs/templates/PROPOSAL.template.md`」）与 :60 第 6 步（「骨架见 `docs/templates/retro.template.md`」）确认缺位形态未变。批 A/批 B 两批 landing 均按纪律复制模板起步（批 B 起草纪律第 2 条已入账），未再炸第三实例；未被后续批消化，本条仍成立。

**结项（2026-09-25，260924-gov-review-gate-parse-teeth T8）**：① arch-review-protocol.md 第 4 步补模板指针句（「骨架见 `docs/templates/architect-landing-summary.template.md`，复制勿手写——缺 landing-manifest 围栏块即结构门 / landing-verifier 合法打回；unit-gate F4 + 批 A 两实例已在册，同根本条目」）；② 可选加固落地——track-b-verifier-brief.md 核验清单第 2 项同句提醒（防 protocol 被跳读）。零门判定语义变更（G2 文面，属新增句非回改）。证据：本批 PROPOSAL §2 T8 + §5 V8 + landing-summary。
