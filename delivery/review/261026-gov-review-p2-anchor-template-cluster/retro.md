# retro · 261026-gov-review-p2-anchor-template-cluster（Track B 全环：P2 锚解析门 + summary 模板簇，六 ARQ 并批 + P3 搭载）

> 主线程第一人称热记忆 · 2026-10-26 · workstream gov-debt-reduction 第 3 波 · harness 形态 = typed agent 全 cast（architect / architect-reviewer / landing-verifier 均本仓 typed agent，无降级形态）· 对照：P1 同日六条并批三轮 GREEN、P1.5 三条两轮 GREEN，本批单轮 GREEN。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写，第一人称热记忆；本篇为 landing-verifier 到场后、verdict 未回时的趁热稿，核验段结论以 verdict 实文为准补记。

**【硬项 R2 · 计量诚实】** 无逐相位 token 计量；§2 按子 agent 用量回执（起草 338k / R1 评审 130k 级）与主线程体感估计，误差界 ±30%。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 带 现象 → 根因 → 证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议标载体。

**【硬项 R5 · 保险丝判定】** §5 给明确判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 立项 | 六 ARQ 处方复证 + human 三取舍 ack | 2026-10-26 | 同日 | ~10 min（人侧仅一次问答） | 主线程 ~20k |
| 起草 | PROPOSAL v0.1（七 T + preflight 自检） | 同日 | 同日 | ~17 min | 338k |
| 评审 R1 | 四查 + 透镜全过，GREEN（0 BLOCKING / 3 NIT） | 同日 | 同日 | ~27 min | ~130k |
| FROZEN | 状态行翻转 + `--upsert` pin | 同日 | 同日 | <5 min | 主线程 |
| landing | T1–T7 七文件 packet-bound + V1–V9 全绿 | 同日 | 同日 | ~30 min | 主线程 |
| 核验 | landing-verifier 六项清单（后台） | 同日 | — | 待回 | 待回 |

对照：P1（六条，三轮 GREEN）→ P1.5（三条，两轮 GREEN）→ 本批（七条，一轮 GREEN）——ARQ 预消化 + 立项期把 human 三取舍前置拍板（P1.5 retro 教训①的执行）后，评审轮次收敛到 1。

## 2. Token 账

计量方式：子 agent 用量取任务回执（起草 338,413 / 评审 129,526 级），主线程与核验官为估计，误差 ±30%。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 起草（architect） | ~338k | ~55% | 含复证 + 装配实跑 + preflight，占大头但换来了单轮 GREEN |
| 判官（R1） | ~130k | ~21% | 含 B6/B10 独立装配复现——评审重活换 pass 可信度 |
| 主线程（立项 + 落地 + 记账） | ~140k（估） | ~23% | landing 为逐字节转写 + V1–V9 实跑 |
| 核验官 | 待回执 | — | — |

花费最大处 = 起草期复证与装配验证。可压缩分析：处方复证（三处事实修正全部在起草期抓到）是单轮 GREEN 的直接原因，不该压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · V7 白名单漏计夹具行**：现象 = V7 第 4 条落地实跑 2、起草期预期 1 → 根因 = 白名单只从「实现行」正推、漏了 §2 处方新文里的自检夹具行也逐字含该字面 → 证据锚 = `grep -cF '（制品见 delivery/review/' .claude/scripts/release-pr.mjs` = 2（buildPrBody 行 + `gov-lane-body-pointer` 夹具行）；FROZEN §5 V7 行 + §7 Landing 留痕已覆写。**反讽点：这正是本批 T4 立的规则该抓的形态——起草期 architect 自己没把夹具行算进处方新文。**
- **F2 · ARQ 前提过时（260928/261021「无模板」）**：现象 = 两条 ARQ 的核心前提在起草期复证时被推翻（模板 260802 已存在） → 根因 = ARQ 登记（09-28 / 10-21）晚于模板落地（08-02）且登记时未查模板面 → 证据锚 = `docs/templates/architect-landing-summary.template.md` 文件头自注 + §2.0 复证 3。幸有「ARQ 处方须实测复证」纪律，起草期消解、未带进改法。
- **F3 · verbatim 观察列在结构门的形态噪声**：现象 = 结构门 verbatim-quote advisory 观察 24 条 missing（exit 0 不阻断） → 根因 = PROPOSAL §2 的 js 夹具串内含 ``` 字面（如 `'/^\s*```/.test(l)'`），围栏提取正则把代码内 ``` 当块边界错切 → 证据锚 = 结构门输出 verbatimQuoteAdvisory.missing（`let end = lines.length…` 被腰斩的条目）。**本批修的就是这个检查的假阴性族，但「夹具串内含 ``` 错切」是相邻新形态**——已在 §6 留根口径外、本 retro 记为观察。
- **near-miss（最险一刻）**：T2 夹具插入差点落错自检段——`rmSync(fixtureDir, …)` 在 landing-fidelity-check.mjs 出现两处，若以它为锚夹具会落进 citation 段且 `block` 未定义、`--self-check` 运行期崩。为什么没出：起草期装配实跑先踩过一次、把「正确锚 = `verbatim-crlf-checked` 行之后」写死进 FROZEN §2，落地照抄锚行。教训：多宿主文件（多自检段）的插入锚必须在起草期实测钉死，不能写「插入自检段末」这类模糊位。

## 4. 优化建议（按 ROI 排序）

1. **verbatim-quote 块提取对「块内容含 ``` 字面」的错切修一下**：治 F3 · 载体 = ARQ 收件箱（登记执行测出的新形态，建议修法 = 提取时要求闭合围栏行独立成行）· 预期收益 = 结构门观察列噪声再降一档。
2. **「处方新文」白名单正推时把自检夹具行显式纳入清单**：治 F1 · 载体 = 登记不硬治（本批 T4 三规则第 2 条已含「处方文本自带的字面也计入」，本次偏差属执行遗漏而非规则缺口；再犯一次再升格）· 预期收益 = V 类断言零覆写。
3. **ARQ 登记时顺手查「承接件是否已存在」**：治 F2 · 载体 = 登记不硬治（gov-arq skill 的登记口径已有「先问是否被后续批消化」，本次是反向：登记晚于承接件落地；执行层面 reminder 即可）· 预期收益 = 复证期少一次前提推翻。

## 5. 一句总结

成本 = 主线程 + 两 agent 约 608k tokens（±30%）、human 触点 1 次（立项问答，三取舍一次拍完）、制品 4 件 + 七文件落地，单轮 GREEN 对照前两波（三轮 / 两轮）持续收敛 · 最险一刻 = T2 夹具插入锚（起草期装配预踩 + FROZEN 钉死化解）· **保险丝判定：两方独立结构接住作者偏差**——判官 R1 独立复现 B6/B10 装配（非转述采信）并抓出 3 条 census 口径 NIT；主线程 V7 实跑偏差由 FROZEN 自带覆写纪律接住、留待核验官交叉 · 最值得带走的一条 = **ARQ 处方复证 + human 取舍前置 = 单轮 GREEN 的两个直接因**（本波与 P1.5 retro 教训互证）。

## 6. 本环遗留（批次型变更特有）

- 留根 1–8 逐条随 FROZEN §6（orphan-T 复议触发 / fence 奇偶例外 / spec-anchor-check·contract-lint 不动 / T7 检测面 / ARQ-261021 机判口径改写 / 五处义务句漂移观察 / T5 硬项标签 / 两线皆空分支）。
- R1 三条 NIT：F1/F2 census 口径在**结项订正 ARQ 条目**时改写（prose 层，不改制品）；F3 已由留根 2 覆盖。
- 七条 ARQ 结项登记（260928/261021 并条带「前提已过时」注记）= 主线程结项动作，随本 retro 之后执行。
- `ARQ-260924` 残余②独立批待开（human 2026-10-26 裁）。
- verbatim 观察列「夹具串内含 ``` 错切」新形态 → §4 建议 1 的 ARQ 收件箱条目。

## 7. verdict 回执补记（同日，趁热）

landing-verifier 回执 = **PASS + 3 ADVISORY**（verdict 行机器锚在案）。六项清单全过：逐文件 diff 对照 7/7 语义匹配、packet-binding 三方一致、验证集复跑全绿（V7 申报「预期 1 → 实跑 2」被核实为真、覆写合规）、结构门 ok:true、真空绿规约无违、诚实披露一致。三条 ADVISORY：① verbatim 观察列 missing = 夹具串含 ``` 字面致围栏错切（本 retro F3，同因）；② `governance-gate-shared.mjs:380` 注释「fence 定界行为奇数」vs 处方「行总数」byte 偏差——仅注释零语义、problem 串逐字一致，核验官裁不要求返工，挂遗留（结项后顺手批对齐或并入下个动该文件的批）；③ 非 manifest 工作树改动四类（workstreams 记账 / debt 结项 / review INDEX / .codebase-memory 再生）性质核为记账与再生类，commit 按此切分、.codebase-memory 两件不追交。

**硬项 R5 修订**：核验官独立复核把 V7 主线程自报偏差核实为真——两方独立结构（判官 + verifier）均接住作者偏差，判定升级为「双接住」。
