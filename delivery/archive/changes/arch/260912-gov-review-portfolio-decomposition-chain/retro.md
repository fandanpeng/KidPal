# retro · 260912-gov-review-portfolio-decomposition-chain（Track B 全环：拆包链治理 PROPOSAL 起草→三轮评审→FROZEN v1.0→9 文件落地→核验 GREEN）

> 主线程第一人称热记忆 · 2026-09-14 · 乘客 #18（治理档，第 18 件 retro）· harness 形态：判官 = 独立 architect 子 agent（R1 17 条打回 / R2 6 条打回 / R3 GREEN，全程零上下文 resumable）；核验官 = landing-verifier 子 agent；主线程起草 + 落地 + 编排，中途因 token 耗尽换模型一次（F4）。· 跨乘客对照：本仓首个「拆包链」型治理批（前几乘客多为闸/流程件）；量级与 260906 十项批（乘客 #8）同档偏上——9 manifest 文件 + 三轮评审 + 两段 RED 收口。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写（01:00 簿记 commit ab357ea 后、合并前），第一人称热记忆。

**【硬项 R2 · 计量诚实】** 本环无 harness token / 时长计量。时长按 branch commit 时间戳锚定（git log，见 §1 表）；tokens 按各 agent 轮次 × 会话规模粗估，误差界 ±50%，不装精确。

**【硬项 R3 · 摩擦带根因 + 证据锚】** 见 §3，每条 F 带 现象 → 根因 → 证据锚。

**【硬项 R4 · 建议必标载体】** 见 §4，逐条标载体与去向。

**【硬项 R5 · 保险丝判定】** 见 §5。

## 1. 时间账

硬数据锚点 = feature 分支六次 commit 时间戳（git log --date=format:"%m-%d %H:%M"）：

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL v0.1→v0.6（拆包链四件套设计） | 09-13 白天（无 commit 锚，估） | 21:54 | 估 3-4h | 估 ~400k ±50% |
| 评审 R1 | 判官打回 17 条（B-1 幻影路径等） | 21:54 | 22:32 | 38 min | 估 ~150k ±50% |
| 评审 R2 | 判官打回 6 条（M-1 闭集碰撞等） | 22:32 | 22:49 | 17 min | 估 ~120k ±50% |
| R3 + FROZEN | R3 GREEN（23 条全闭合 + 2 NOTE 折入）→ v1.0 FROZEN | 22:49 | 22:57 | 8 min | 估 ~100k ±50% |
| landing | T1–T4 共 9 文件落地 + summary | 22:57 | 00:45 | 108 min（含 compaction + F4 换模型中断） | 估 ~500k ±50% |
| 核验 + 收口 | landing-verifier GREEN（D1–D5）+ D1–D4 处置 | 00:45 | 01:00 | 15 min | 估 ~150k ±50% |
| **合计** | | | | 实锚 21:54→01:00 = 3h06m，起草另计 | 估 ~1.4M ±50% |

对照：260913 spec-split 批（同日乘客，六轮判官）本环评审轮次更少（三轮）但单轮 finding 密度高（R1 17 条）；landing 相位因模型中断拉长，非节奏劣化。

## 2. Token 账

计量方式诚实声明：无 harness 计量，按轮次 × 会话规模粗估，±50%。

| 载体 | tokens（估） | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 落地 + 编排） | ~900k | ~64% | 含两次 compaction 后的上下文重建；landing 相位为主（9 文件逐件写） |
| 判官（R1+R2+R3） | ~370k | ~26% | R1 最重（17 条逐条引证）；R3 含全仓机检复核 |
| 核验官（landing-verifier） | ~130k | ~10% | 结构门 + 9 文件逐件人读 diff |

花费最大处：主线程 landing 相位。可压缩分析：PROPOSAL 修订史（⑩–㉘ 逐版留痕）占起草成本约三成——留痕是治理要求不可省，但「v0.6→v0.7 对齐 write-story」类外部口径变化若能早一个相位对齐（见 F5），可省一轮修订往返。

## 3. 摩擦与根因（按痛感排序）

- **F1 · landing 结构门两段 RED（D1–D3 + D4）**：现象 = 核验官判 GREEN 后复跑 `landing-fidelity-check.mjs` 先报 3 错（summary 文件名 / packet-id / manifest fence 缺）后报 4 错（「T1…T4 无『目标文件：』行」）→ 根因 = landing 制品格式约定（文件名 `architect-landing-summary.md`、packet-id 行、```landing-manifest fence、锚行裸形态）散在 parser 实现里，起草侧凭记忆写、无起草期自检 → 证据锚 = `.claude/scripts/lib/governance-gate-shared.mjs` `parseSection2AnchorLists`（regex `/^目标文件\s*[:：]/` 拒加粗形态）；实跑输出两段 RED（会话记录，exit 1）。
- **F2 · R2-M1 闭集碰撞**：现象 = v0.8 首钉新事件型 `anchor3-deviation`，判官亲核发现会被 `pod-events-append.mjs` 六型闭集 throw → 根因 = 先钉句法后查机器载体，顺序颠倒 → 证据锚 = `.claude/references/dev-loop-operating-system.md:130`（eventType 闭集六型）；PROPOSAL §7 R2 打回记录。修复 = 换既有 `human-decision` + `payload.decision="anchor3-deviation-ruling"` 区分值 + `--actor human`（planner.md `approve-escalated-merge` 先例）。
- **F3 · R1 B-1 幻影 manifest 目标**：现象 = `.claude/skills/triage-change/SKILL.md` 不存在却被列为 landing 目标 → 根因 = 起草时凭印象引路径、未 ls 验证 → 证据锚 = PROPOSAL §7 R1 B-1；修复 = 换真身 `change-triage.md` + `triage-change.md`。**次生**：T1 锚行注记逐字回引该幻影路径，修 D4 加粗形态后它会进 parser 锚并集、造成 §2↔manifest 假失配（见 near-miss）。
- **F4 · token 耗尽换模型**：现象 = 4 次 `portfolio-planner.md` 编辑被上游拒（rate-limit），中断后用户换模型续跑 → 根因 = 上游限流，非本批缺陷 → 证据锚 = 会话记录（用户「没token了，换了一个模型，你继续」）。重试同内容全成功，零返工。
- **F5 · 跨批悬空指针**：现象 = write-story（a118f75）把转写规则委托给本 PROPOSAL，v0.6 时未承载 → 根因 = 两批并行起草、依赖时序错位；用户另否决「内嵌 story 原文」（仅允许序号+Title 引用），方向修正后 v0.7 落锚点引用口径 → 证据锚 = v0.7 commit ed89771 修订行⑩–⑮。

- **near-miss（最险一刻）**：D4 收口时若只改加粗形态、不撤 T1 锚行注记里的幻影路径 token，parser 会把 `.claude/skills/triage-change/SKILL.md` 提进锚并集 → 结构门 fail-loud 卡死（好情况）或 manifest 偶然含它时静默放行（坏情况）。没出事是因为核验官 D4 处置意见里已点名「锚行裸写、注记不引路径 token」纪律，主线程照做并逐 token 排查。教训：锚行注记引用任何路径都会被机器当成锚，注记只写人读语义、路径 token 只出现在真锚列。

## 4. 优化建议（按 ROI 排序）

1. **FROZEN 前结构门干跑**：PROPOSAL 标 FROZEN 前对 §2 锚清单跑一次 `parseSection2AnchorLists` 干跑（只解析不比 manifest），把 D4 类格式错前移两个相位。治 F1 · 载体 = ARQ 收件箱（登记 `landing-preflight-anchor-parse` 候选，不硬治） · 预期收益：本批 D4 那类 4 错全消，收口相位缩短约 10 min/批。
2. **起草侧「路径先验存在」自检**：PROPOSAL 列 manifest 目标前逐路径 ls 验证（本批 B-1 + T1 注记两次同根）。治 F3 · 载体 = 留批次（并入建议 1 的干跑，或 PROPOSAL.template.md 起草 checklist 一句话） · 预期收益：幻影路径在 R1 前拦截。
3. **机器载体先查后钉句法**：新事件型 / 新字段先读消费方脚本（闭集、schema）再写 PROPOSAL 条文。治 F2 · 载体 = 留批次（task-intake 门 1 checklist 已有「裁决耐久记录」项，可补一句「先查 pod-events-append 闭集」） · 预期收益：省一轮 R2 打回。
4. **citationAnchorAdvisory 短式引用解析局限**：判官记录里 `dev-loop-operating-system.md:130` 相对仓根解析不到（实文件在 `.claude/references/` 下，行内容本批已核对相符；260908 verdict 已归因同型）。载体 = ARQ 收件箱（parser 的 `resolveByPrefix` 扩 `.claude/references/` 前缀） · 预期收益：advisory 噪音减半。

## 5. 一句总结

成本估 ~1.4M tokens ±50% / 实锚 3h06m（起草另计）/ human 触点 4 次（立项 ack、M-1 三选一、OQ1–OQ4 批裁定【OQ3 先解释再定】、token 耗尽换模型）/ 制品 4 件 + manifest 9 文件 / 对照 260906 十项批量级偏上。最险一刻 = T1 锚行注记回引幻影路径 token（near-miss 详 §3）。**保险丝判定：接住**——判官独立抓住 B-1 幻影路径与 M-1 闭集碰撞（设计偏差），核验官独立抓住 D1–D4（判官未看的 landing 制品格式面）；两方抓的是不同类作者偏差，互补成立。最值得带走的一条：**锚行是机器接口不是散文——注记里的路径 token 会被当锚提取，起草时锚行只列真锚、语义注记另起一行裸写。**

## 6. 本环遗留

- **ARQ 留根（PROPOSAL §6 + summary 诚实记录已落）**：① M-1 机械齿——`portfolio-reconcile.mjs` 读 launchPreconditions（260913 human 裁定本批不做、convention + 人读）；② `Sx-Ay` 老仓遗产字样 manifest 外命中面批量对齐（integrator.md / build-feature.md / minimal-implementation.md / acceptance-evidence.md / code-review-gate.md / builder.md / dev-loop.md / acceptance-author description / acceptance-id-shared.mjs / mutation-check.mjs 等）。
- **建议 1 / 4 的 ARQ 登记**：本 retro §4 所列两条 ARQ 候选待登 `docs/ARCH-REVIEW-QUEUE.md` 收件箱。
- **`landing-verifier.md` lint over-length（363 > 320）**：既有 warn、与本批无关，未处理。
- **requirements.md 同期重 review（human 主导 phase 1/2）**：本批机查口径按「节名定位、位置无关」落（m-5），后续变化由该批自洽，不属本环遗留动作。
