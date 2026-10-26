# retro · 261026-gov-review-final-phase-dispatch-contract（Track B 全环：P1.5 final 相位派发契约批）

> 主线程第一人称热记忆 · 2026-10-26 · 单批（P1.5，三 ARQ 并批）· harness 形态：typed architect / architect-reviewer / landing-verifier 子 agent（各 resume 语境延续）· 对照基线 = 同 workstream P1 批（261026-gov-review-merge-gate-cluster-fix）。

**【硬项 R1 · 环后趁热、第一人称】** landing-verdict PASS（D1/D2 两 ADVISORY 当轮记入 §3/§6），本 retro 于合并 main 前趁热写就。

**【硬项 R2 · 计量诚实】** 子 agent 用量来源 = harness task notification 的 usage 字段；architect / reviewer 数字为**同 agent resume 累计值**（单轮增量不可拆）；主线程无计量，仅记工具调用次数级体感。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 均带证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议标载体。

**【硬项 R5 · 保险丝判定】** §5 明确判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens（子 agent） |
|---|---|---|---|---|---|
| 立项 | human ack（三 ARQ 并批口径）+ 主线程事实核实（verify.mjs 定位 / planner 路径存疑） | 2026-10-26 | 同日 | <5 min | — |
| 起草 | architect v0.1（三条 ARQ 复证 + 影响面 sweep + 验证集） | 同日 | 同日 | ~7.4 min | 233,915（97 调用） |
| OQ 拍板 + 修订 | human 裁 OQ1=(a)；architect v0.2（F1/F2/NIT 修订） | 同日 | 同日 | ~14.1 min | 254,815（累计，115 调用） |
| 评审 R1 | 判官 BLOCK(F1,F2)+NIT（全量亲核 + 五臂探针复现） | 同日 | 同日 | ~13.9 min | 100,540（44 调用） |
| 评审 R2 | 判官聚焦 GREEN（单点） | 同日 | 同日 | ~0.9 min | 109,184（累计，50 调用） |
| FROZEN → landing | 主线程逐 T 落 7 文件（13 处替换/插入）+ landing-summary | 同日 | 同日 | ~20 min（含 V1–V9 实跑） | 主线程（无计量） |
| 核验 | landing-verifier PASS + D1/D2 偏差记录 | 同日 | 同日 | ~5.0 min | 63,922（25 调用） |

对照 P1：起草显著更轻（本批纯文面 + 一条 stderr + scope-map 数据，无原型无 fixture），评审 R1 成本相当（全量亲核纪律恒定），收敛轮少一轮（R2 即 GREEN）；核验成本约为 P1 一半。**收口时序与 P1 同构：评审成本集中在第一轮。**

## 2. Token 账

计量方式诚实声明：子 agent 数值来自 harness 通知字段，architect / reviewer 为 **resume 累计**（误差界 = 最后两轮差值量级）；主线程无计量。

| 载体 | tokens | 备注 |
|---|---|---|
| 主线程（编排 + 落地 + 本文） | 无计量 | 约 30 次工具调用（锚点定位 + 13 处 Edit + 8 组实跑） |
| architect（累计两轮） | ~254,815 | 复证 + 影响面 sweep + 一次修订 |
| architect-reviewer（累计两轮） | ~109,184 | R1 全量亲核 + R2 聚焦 |
| landing-verifier | ~63,922 | 独立字节级核验 + 偏差记录 |

花费最大处 = architect 起草期复证（三条 ARQ 逐条对源 + 五臂写权探针 + moderate census）。可压缩分析：R1-F1（OQ 裁定行回填滞后）源自「先起草后裁 OQ1」时序——若 human 拍 OQ1 在起草完成前，F1 整类可消。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · ARQ 处方与实况冲突两连（起草期双抓）**：现象 = ① 主条目 paths 引不存在的 `.claude/references/planner.md`；② 条目三候选修法（子 agent 直写包目录）与 enforce-scope 写权实况冲突（五臂探针 exit 2）→ 根因 = **ARQ 行整条是待验证假设**（登记时未实测写权与路径）→ 证据锚 = PROPOSAL §2.0 复证 1/3 + 起草报告事实修正 1/4。两处都在起草期复证拦下，未流到落地。
- **F2 · OQ 裁定时序交错（R1-F1 根因）**：现象 = human 裁 OQ1 发生在 v0.1 之后，PROPOSAL 止于「推荐 (a)」、三处条件注记未解除，判官判为 BLOCK 项 → 根因 = 主线程把 OQ 拍板排在了起草完成之后；裁定回填靠主线程转发、又漏让 architect 回填裁定行 → 证据锚 = PROPOSAL §7 R1-F1。P1 同日先例是「起草 → 评审 → 四 OQ 一并拍」——本批 OQ 只有 1 个且起草期已知，本可前置。
- **F3 · 誊写位置语义失真（verifier D2）**：现象 = cli/SKILL.md 的追加提醒句落进 bash 围栏内而非「代码块下方」，字节级核验抓出；landing-summary 初稿自述「形态差异一处」与实况两处不符 → 根因 = 13 处誊写中「下方追加」类位置指令靠目视执行、无锚句 grep 自证；summary 自述时点早于 verifier 独立核验，自述计数未回对 → 证据锚 = landing-verdict D1/D2 + summary §3.2 订正行。无语义损失，纯位置与自述精度。
- **near-miss（无大险，一条记档）**：主条目只说「verify.mjs fail-closed 行为」未给路径，主线程 Glob 定位 `.claude/hooks/verify.mjs` 后写进 architect 简报——若当时凭 ARQ 语境默认成 scripts/ 会错落改法②落点；复证纪律（指派单官现读再粘 + 起草期对源）接住了。

## 4. 优化建议（按 ROI 排序）

1. **OQ 前置拍板**：治 F2 · 载体 = 主线程编排习惯（gov-change loop 主线程可判「OQ 已在起草期显形且无新信息时，派起草的同时递 human 拍板」）· 预期收益 = 消 F1 型 BLOCK 轮，每批省 1 轮判官往返。
2. **「位置类」誊写自检**：治 F3 · 载体 = 登记不硬治（若再现一次同根偏差，升级 landing-summary 模板硬项：追加句类改法须带锚句 grep 自证）· 预期收益 = 消「下方/围栏内」型位置偏差。
3. **ARQ 登记时跑最小写权/路径探针**：治 F1 · 载体 = ARQ 收件箱纪律（gov-arq skill 登记时对「写权/路径」类断言附一条实跑命令与输出）· 预期收益 = 起草期复证成本下降，OQ 升级更早显形。
4. **summary 自述计数回对**：治 F3 后半 · 载体 = 已有纪律（carry-canonical-briefs-verbatim 同族，P1 retro 建议 3 复诵）——本批对判官简报做到了，对 summary 自述又漏了一次；P1 已提，无新机制。

## 5. 一句总结

成本 = 子 agent 三角色 ~428k tokens（architect 254,815 + reviewer 109,184 + verifier 63,922，累计口径）+ 主线程半日内闭环；human 触点 2 次（立项 ack + OQ1 一并），制品 4 件齐（PROPOSAL FROZEN v0.2 + landing-summary + landing-verdict PASS + 本文）；最险一刻 = 无（本批无行为面新代码，唯一行为面改动是 stderr 文案与 scope-map 数据，探针与自检双层兜住）；**保险丝判定：两方独立结构接住了作者偏差**——判官 R1 抓出裁定回填缺失与体感升格（均非作者故意、但落地即钉进 canonical），核验官字节级抓出主线程誊写的围栏位置偏差与 summary 自述计数失实；最值得带走的一条 = **ARQ 行是假设、OQ 宜前置：起草期复证拦住两处处方冲突，OQ 时序若前置可省整轮 BLOCK**。

## 6. 本环遗留

- **verifier 偏差 2 条处置**：D1（T1/T3 块间 `>` 空行）→ 接受（FROZEN 未规定块间空行形态）；D2（cli 提醒句落围栏内）→ 接受 + summary §3.2 已订正自述计数（一处 → 两处）。
- **留根 7 项**：去向 = FROZEN §6（launch card 不加 final 契约、scope-file 存量命名不追溯、progress 注记惯例非门、moderate 历史散文不追改、KidPal-claude 建图模式未单列、bearing preflight 文案未含正确序列、窄写权越界复议触发）。
- **三条 ARQ 结项**：verdict 绿后主线程原地翻字段（closed + closedBy = 本批 landing-verdict），已随落地 commit `009eb70f` 入账并归档 `_archive/2026/`。
