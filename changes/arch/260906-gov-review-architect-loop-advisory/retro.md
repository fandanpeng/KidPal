# retro · 260906-gov-review-architect-loop-advisory（Track B 全环：单轮 GREEN 一次过）

> 主线程第一人称热记忆 · 2026-09-06 · 乘客 #11（同日第二班，承 260906-gov-review-arq-governance-batch 十项批后）· harness 形态 = 判官 / 核验官均 architect/landing-verifier 全档子 agent（非降级形态）· 跨乘客对照：与上一批同享「小 G2 批、单轮过」形态。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL v0.1（设计讨论结晶，OQ 三题） | 2026-09-06 对话期 | 同 | ~15 min（估） | 未计量 |
| 评审 R1 | 判官 architect · GREEN + 4 NIT | — | — | 532 s（harness 实测） | 67,365（harness 实测） |
| FROZEN | 凭据齐（ack + OQ 全裁 (a) + R1 GREEN）+ 4 NIT 机械落字 | — | — | <5 min | 未计量 |
| landing | T1–T4 四文件落地 + V1–V7 全绿 | — | — | ~10 min（估） | 未计量 |
| 核验 | landing-verifier · GREEN（D1 披露 + INDEX 簿记 advisory） | — | — | 235 s（harness 实测） | 43,606（harness 实测） |
| **合计** | | | | ~40 min（估） | ~111k（子 agent 实测）+ 主线程未计量 |

对照：上一批（arq-governance-batch）双轮 R1 NEEDS-REVISION → R2 GREEN，本批单轮过——原因是本批为「纯新增型」prose 修订（没有机制改法要亲核），证据断言少而直。

## 2. Token 账

计量方式诚实声明：子 agent tokens/duration 为 harness 回传实测；主线程（起草 + 落地 + 评审 prompt 工程）无 harness 分相计量，按对话复杂度估 ~60–90k ±50%，不装精确。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + FROZEN + landing） | ~60–90k（估） | ~45% | 起草期跑了 6 组实证命令（grep/闸/结构） |
| 判官 R1 | 67,365（实测） | ~40% | 23 tool-uses，行号级引文全量亲核 |
| 核验官 | 43,606（实测） | ~15% | 13 tool-uses，独立复跑结构门 + 逐文件 diff |

花费最大处：判官行号级逐字核对（planner.md 57/58/66、architect.md 53/66、operating-model 等 10 条引文）。可压缩分析：本批引文密度高是小 G2 prose 批的固有成本，判官不虚核则 GREEN 含金量下降——不压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 两条编辑撞重复匹配**：标 FROZEN 落 NIT 时，「十条→八条」与「≥4→≥7」两处 old_string 在 R1 评审记录里有逐字引述（append-only 区不能回改），Edit 报 2 matches。→ 根因 = 评审记录天然复述正文措辞，机械修订必须带更长上下文定位。→ 证据锚：本会话 Edit 工具双报错输出。化解 = 各加长 ~20 字上下文消歧，零成本。
- **F2 · landing-fidelity 参数形态**：第一跑传 `--change-id <id>`（dev-loop 习惯形态）被当路径解析报 review directory not exist。→ 根因 = 该闸取**位置参数** reviewDir（changes/arch/... 相对路径），与 dev-loop 闸的 flag 形态不一致。→ 证据锚：脚本 `landing-fidelity-check.mjs:50`（`process.argv[2]` 位置参数）。第二跑改位置参数即过。
- **near-miss（最险一刻）**：advisory `changedCanonicalNotInProposal` 报出 `.planning/kidpal-mvp/progress.md` 与 `.claude/skills/kidpal-pod-launch-prep/` 两件脏文件——若 staging 时一把 `git add -A` 会把**包 2 launch 准备面的未提交工作**混进本批 commit。没出事的因为：AGENTS.md 纪律「`git add` 后必 `git diff --cached --stat` 核对」+ verifier 范围断言先行归账这两件为「另一工作面」。教训：多工作面并行期，staging 必须逐件显式路径。

## 4. 优化建议（按 ROI 排序）

1. **landing-fidelity-check 统一参数形态或报错提示**：治 F2 · 载体 = ARQ 收件箱（低优先，一次报错即学会，但下个主线程/包会再踩）· 预期收益 = 省一轮失败调用。
2. **多工作面并行期 staging 纪律成文**：治 near-miss · 载体 = 登记不硬治（既有 `git diff --cached --stat` 纪律已够，本次它接住了）· 仅在 retro 留痕提醒。
3. **T1 引文体例（D1）**：landing 把两处行号指针（planner.md:58/:66）规范化为「（Step 9）」结构化指针——verifier 判语义零变更、非私货。载体 = 固化候选已随本批落地（verifier D1 建议 retro 留一笔，此笔即落实）· 后续 prose 引用优先结构化锚（节名/步骤号）少行号锚，降行号漂移面。

## 5. 一句总结

成本 ~40 min / 子 agent ~111k tokens（主线程未计量）/ human 触点 1 次（立项 ack + OQ 三裁一并）/ 制品 4 件齐 / 对照上一批：单轮过 vs 双轮，纯新增型 prose 批的轻量形态成立 · 最险一刻 = 脏文件混 staging 风险（被既有纪律接住）· **保险丝判定：两方独立结构接住了作者偏差**——判官抓出 4 条证据精度 NIT（含一条真空绿验证集条目 F4：基线已 4、阈值 ≥4 不改也过，主线程起草时确实没跑基线），核验官独立复跑结构门 + 逐文件 diff 并披露 D1 引文体例差异；作者（主线程）的盲点在 R1 全部现形 · 最值得带走的一条 = **验证集阈值类断言必须亲跑基线**（≥N 型断言不跑基线 = 真空绿，F4 是活例）。

## 6. 本环遗留

- changes/INDEX.md 本 change 行状态随合并后 upsert 更新（verifier advisory 已提，簿记面）。
- D1（引文体例规范化）已留痕，不另立项。
- 首次实际使用观察项（PROPOSAL §6 自律件）：planner 首次真派 architect 档时 pod-retro 记使用体感；retro 核对「意见 advisory 不腐蚀验证链」。
