# retro · 261027-gov-review-p75-closeout-shoot-cluster（Track B 全环：R1 BLOCK 文本级 → v0.2 → R2 GREEN → 落地 → PASS+1 ADVISORY）

> 主线程第一人称热记忆 · 2026-10-27 · gov-debt-reduction workstream P7.5 波（两条，恒 Track B）· harness 形态：architect / 判官 / 核验官零上下文子 agent，判官两轮同实例续跑（SendMessage 带上下文）· 同日第三批（P7 / P7.5 连跑）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 落盘后随即撰写，全相位本会话亲历，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens = harness usage 报告（architect 起草 278,445 + 修订轮 303,515；判官 R1 134,195 + R2 167,177；verifier 见 §2 如实声明）。主线程无计量给估计。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 逐条带证据锚。

**【硬项 R4 · 建议必标载体】** S1 已条目化（ARQ-261027-draft-toolface-blind）；S2 登记不硬治。

**【硬项 R5 · 保险丝判定】** §5 明确判定：接住。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | DRAFT v0.1（536 行，T1–T6 17 处） | 本会话 | 本会话 | ~13 min（778s） | 278,445 |
| 评审 R1 | 四查 BLOCK(F1,F2,F3)+2NIT | 本会话 | 本会话 | ~7.5 min（453s） | 134,195 |
| 修订 | v0.2（566 行，五处文本级修订） | 本会话 | 本会话 | ~2 min（120s） | 303,515 |
| 评审 R2 | 聚焦复验 GREEN | 本会话 | 本会话 | ~2 min（114s） | 167,177 |
| FROZEN + landing | 复跑装配 → apply 17 处 → render → cite-check → 清 _scratch | 本会话 | 本会话 | ~8 min | 主线程 |
| 核验 | landing-verifier PASS + 1 ADVISORY | 本会话 | 本会话 | 235s | 82,489 |
| **合计** | 立项到 verdict | — | — | ~50 min | 子 agent ~917k（主线程另计） |

对照：P7 批同日 ~30–35 min / ~389k——本批翻倍，主因 = 恒 Track B 两条件 + agents 定义改写触发判官深核（scope-map 逐行亲读、消费者全仓普查）+ 一轮 BLOCK 返工；且同日三批连跑主线程编排面复用，无额外启动成本。

## 2. Token 账

计量方式诚实声明：子 agent 五段 usage 全部实测（verifier 82,489 在初稿后到达，已覆写初稿估计值——原估 ~55k±30% 被突破，如实记录）；主线程无计量给估计。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（编排 + 落地 + 记账） | ~60–90k（估） | ~12% | 含两轮 OQ/记账插话 |
| architect（起草 + 修订） | 581,960 | ~65% | 修订轮 303k 反常高——五处文本级修订烧出 drafting 轮最高值，疑 SendMessage 续跑重载全上下文所致，待 cadence 校准 |
| 判官（R1+R2） | 301,372 | ~34% | 两轮续跑；R1 深核（scope-map 亲读 + 全仓普查）是 Track B 应有强度 |
| 核验官 | 82,489 | ~9% | usage 实测（初稿低估 33%，覆写留痕） |

花费最大处：architect。可压缩分析：判官 findings 全部文本级、一次修订收敛——若起草期把 §4 OQ 拍板记录与 agents 工具面核对前置，BLOCK 轮可免（见 S1）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 起草对 agents 落点的「工具面盲区」**：现象 = T4 单写者约定指示「做最小 Edit」，但 uiux-author frontmatter `tools: Read, Write, Grep, Glob` 无 Edit——指令对执行者不可执行 → 根因 = 起草期只核语义不核执行者工具面，prompt-authoring rule 管文风不管工具面对账 → 证据锚 = R1 F2（PROPOSAL §7）；判官抓住，改 Write 语义判据收敛。同类坑上批无先例，属 agents 落点批的新暴露面。
- **F2 · 验证集摘要「报喜式转写」**：现象 = B10 写「17 条 matched 各指向对应目标文件」，实跑 16+1 归属歧义 → 根因 = 起草期把「ok:true」笼统转写成全称断言，未逐条看 matched 分布 → 证据锚 = R1 F3；「Track B 制品义务②如实回填」被判官逐字核对兜住。教训：matched 分布要逐条抄，不摘要。
- **F3 · cite-check 对同串翻转编辑的归属歧义**：现象 = ARQ-261019 frontmatter 翻转块 oldStr（`status: deferred`）在两件 ARQ 各恰 1 命中，checker 归到另一件 → 根因 = checker 按「旧串在哪个文件命中」归属，同串多文件即歧义；装配器「恰 1 命中」断言与落地相位「新文唯一」兜住，无实险 → 证据锚 = R1 F3 / landing-summary 诚实注记。教训：翻转类编辑的 oldStr 尽量带 ID 行唯一化。
- **near-miss（最险一刻）**：起草期工作树出现两条「已接受风险」ARQ 裸删（contract-mech-gate-gap / crossend-consumer-tiers，无归位痕迹、非本会话产物）——若未察觉直接进入 P7.5 落地 commit，裸删会被顺手 staged 进批 commit，账本 append-only 纪律破防。拦截 = 落地前 `git status` 全量过 + architect 交接报告主动声明「他方在途改动」；处置 = 停流程 → 问 human → 确认有意删 → 还原 + 翻 closed + 追注 + 归档正规补记（commit `8c262d4c`）。教训：多会话同仓时代，「工作树里不是我改的」必须逐件定性后才准进批 commit。
- **F4 · INDEX 域外来历重排**：delivery/review/INDEX.md 出现非本会话的重排（行数 23→24、P7 行移位）→ human 裁「不管，随本批落地重建」——生成件幂等重建兜住，未升级。与 near-miss 同源（多会话并发写生成件）。

## 4. 优化建议（按 ROI 排序）

1. **agents 落点批起草期加「工具面对账」前置**：治 F1 · 载体 = **已条目化 → `docs/debt/gov/ARQ-261027-draft-toolface-blind.md`**（SHOULD；处方 = prompt-authoring rule 或 PROPOSAL.template 自检句加一句「改 agents 正文时先读该 agent frontmatter tools，禁指涉工具面不存在的操作」）· 预期收益 = 免一轮 BLOCK 返工（本批 F2 型）。
2. **验证集 matched 分布逐条抄**：治 F2 · 载体 = **登记不硬治**——「Track B 制品义务②」原文明文「禁止推算、如实回填」，本批是执行走样非规则缺口；判官逐字核对已兜住，先观察复发。
3. **翻转类编辑 oldStr 带 ID 唯一化**：治 F3 · 载体 = **留批次**——下次遇 frontmatter 翻转编辑时在改法集里用含 ID 的多行锚，届时顺手，不单独立规。

## 5. 一句总结

成本子 agent ~917k / ~50 min / human 触点 3 次（OQ-1 拍板 + 裸删确认 + 结项理由）· 制品 4 件齐 · 对照 P7 同日批成本翻倍，主因恒 Track B 深核 + 一轮 BLOCK · 最险一刻 = 两条账本条目裸删差点混进批 commit（status 全量过 + architect 主动声明接住）· **保险丝判定：接住**——判官 R1 独立抓出工具面盲区与摘要失实（作者偏差两处均非 verifier 才暴露），verifier PASS 零新增差异 · 最值得带走的一条：多会话同仓时代，落地前的「全量 status 逐件定性」是账本纪律的最后一道闸，谁改的都得问清。

## 6. 本环遗留（条件节）

- FROZEN §6 留根 7 条照录（per-session cast 可用性 / 机械通道观察 / SKILL description 死指针 / 8c 无机械闸 / 生成件位移 / _scratch 清理〔已执行〕 / P5 留根交接）。
- ARQ-261014 维持 deferred（②③ 收编、① 半收编）；ARQ-261019 整条结项归档。
- ADVISORY-1（--upsert 补钉）= 主线程落地 commit 前动作，本 retro 撰写时点未执行、不预写结果。
