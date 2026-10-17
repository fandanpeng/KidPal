# retro · 261016-gov-review-triage-entry-identity（Track B 全环：立项 → R1 GREEN → 修订 → R2 GREEN → FROZEN → 落地 → PASS + 3 ADVISORY）

> 主线程第一人称热记忆 · 2026-10-17 · 乘客 #1（本类「常驻指令面身份陈述显式化」首个；`delivery/review/` 在册仅 3 批，无同款乘客对照）· harness 形态 = 起草 / 判官 / 核验官均为 typed 子 agent（architect / architect-reviewer / landing-verifier），本仓有 typed landing-verifier，非 Kimi 降级形态。

**【硬项 R1 · 环后趁热、第一人称】** 本 retro 于 landing-verdict PASS 后立即撰写，未隔场补记。

## 1. 时间账

| 相位 | 内容 | 日期 | 子 agent 挂钟 / tokens（通知载荷） |
|---|---|---|---|
| 立项 | ARQ 消费 + 主线程查证（触发本批的两处 ARQ 更正） | 2026-10-16 | 主线程自办 |
| 起草 v0.1 | T1–T5 | 2026-10-16 | 9 min 45 s / 138,865 |
| OQ 裁定 → 修订 v0.2 | OQ-4=(a) 破界后新增 T5/T6 | 2026-10-16 | 5 min 16 s / 181,209 |
| R1 评审 | GREEN（1 SHOULD + 4 NIT，无 BLOCKING） | 2026-10-17 | 3 min 18 s / 108,356 |
| 修订 v0.3 | F1–F5 逐条处置 | 2026-10-17 | 5 min 20 s / 230,489 |
| R2 评审 | GREEN 维持（F1–F5 全到位） | 2026-10-17 | 52 s / 131,554 |
| FROZEN | 立项 ack + R2 GREEN + OQ 裁毕，主线程凭据自冻 | 2026-10-17 | — |
| landing | T1–T7 落地 8 文件（V1–V6 全绿、结构门 exit=0） | 2026-10-17 | 主线程自办 |
| 核验 | landing-verifier PASS + 3 ADVISORY | 2026-10-17 | 2 min 16 s / 68,560 |
| **合计** | | | **约 26 min 47 s / 859,033** |

对照：`261016-gov-review-nano-tier`（同前序批）走 R1–R5 五轮评审、挂钟约 66 min + 核验 18 min；本批两轮收敛、量级小一个数量级——差别在于本批是**纯措辞显式化、零门权/零流程语义**，改法集无设计分叉。

## 2. Token 账

**计量方式诚实声明**：上表 token 为各子 agent 每次「停止」时 harness 通知载荷里的 `subagent_tokens`，**逐次照抄、未做归一**。我无法确认该字段是「本次运行」还是「该 agent 累计」，故按每次通知原样并列（三次 architect 通知 138,865 / 181,209 / 230,489 单调递增但**非**累计关系——若为累计，第三次数值应 ≥ 前两次之和 320,074，实测 230,489 小于该值，故此判为逐次量、误差界 ±10%）。**主线程自身 token 无计量面**，不在表内。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 起草者 architect（三次运行） | 550,563 | 64% | 含两次修订（OQ-4 增项 / F1–F5） |
| 判官 architect-reviewer（R1+R2） | 239,910 | 28% | R2 只做聚焦复核，52 s |
| 核验官 landing-verifier | 68,560 | 8% | 六项清单 + 两点追加亲核 |

花费最大处：**architect 的第二次修订（230,489，占单次最高）**——F1–F5 全是措辞/计数/引据级修订，却要重读 PROPOSAL 全文并复跑四条命令。可压缩分析：若起草期把「同一事实多处一致」「description 守 authoring 规则」「引据引对权威」三件事做进自检三句，F1–F5 五条中至少三条不会产生，可省约一轮修订（≈23 万 token、5 min）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 承载 ARQ 的成因断言与 git 事实相反**：现象 = ARQ 证据 5 断言「前身批改了 triage 身份却没同步 AGENTS.md，其 `paths` 未含 AGENTS.md 即直接实证」；实测 `git show 2e43fc59 -- AGENTS.md` 显示该批**确实改了 AGENTS.md**（+2 行），真正的漂移点是后一批 `68ab0579`（新立「四面入口」框架、把前批身份陈述收编成并列项）。→ 根因 = 该 ARQ 由主线程在**同一场误读之后**所写，成因分析建立在 `paths` 字段的**间接证据**（路径清单缺失）上，未直接看 diff；`paths` 未列 AGENTS.md 是「列举遗漏」，推不出「未同步」。→ 证据锚：`git show 2e43fc59 -- AGENTS.md`（+2 行）、`git show 68ab0579 -- AGENTS.md`（新立节头 + 三新行）；判官 R1 独立复核确认（PROPOSAL §7 `:328`–`:329`）。

- **F2 · PROPOSAL 自身同一事实两处不一致**：现象 = §2 谓 `grep "triage 实现面"` = 1 处、§3 同形 = 3 处，自相矛盾（实测 3，§3 对）。→ 根因 = 起草期只做了「每条断言有实测」，**没做「同一事实多处出现须两两一致」**的横向自检；census 那行在 v0.2 修订时被漏改。→ 证据锚：PROPOSAL R1-F1；R2 复跑四形实测 `4 / 1 / 2 / 3` 三方吻合。

- **F3 · 引据引到对本文件硬排除的规则**：现象 = T5 称守 `prompt-authoring.md` 的 HQ-1，但该 rule 首行明写**不适用于 `architect.md`**。→ 根因 = 起草者凭「同构 → 同规则」类推、未读该 rule 的硬排除首行；真依据是 `docs/DEV-GOVERNANCE.md:212` §6.3「自改递归」。→ 证据锚：PROPOSAL R1-F3；R2 亲核 `:212` 逐字吻合。

- **near-miss（最险一刻）**：`architect.md` 同时是**自改递归边界 + 第五阀 + `prompt-authoring.md` 硬排除**三重重约束的文件——OQ-4 若被按推荐位 (b) 静默处理本无风险，但 human 改裁 (a)「本批顺带改」。**险点在于：一旦 (a) 被当作普通改动执行，本批就会在不经人类明批的情况下改掉 architect 的自身定义**。没出事的原因 = 主线程在呈 OQ 时把「需你此刻显式批准破自改递归边界」写进选项描述、且把该前提逐字留在 PROPOSAL §4；判官 R1 的焦点 1 又把「批准留痕是否在场、是否与裁断一致」列为必核项（R1 记 F4 建议逐字留痕，R2 核到位）。**教训：破界类选项必须把「本选项需要什么额外授权」写在选项里，而不是写在正文注解里——human 读的是选项。**

## 4. 优化建议（按 ROI 排序）

1. **debt 条目的因果断言禁以路径清单反推、须直接看 diff**：治 F1 · 载体 = **ARQ 收件箱**（拟固化进 `gov-arq` skill 或 debt 条目模板的起草纪律：「凡断言『某批没做 X』→ 先 `git show <sha> -- <file>` 实测，不得以 `paths`/清单缺失反推」）· 预期收益 = 免去外部查证返工；本批若不带该更正，一条失实成因会进入 FROZEN 治理记录。
2. **PROPOSAL 起草期自检加第四句「同一事实多处出现须两两一致」**：治 F2 · 载体 = **ARQ 收件箱**（拟扩 `docs/templates/PROPOSAL.template.md`「起草期自检」段）· 预期收益 = 消掉「SHOULD 级事实矛盾靠判官抓」这一整类；本批 F2 是判官发现的、非自检发现的。
3. **改任何 `description` 前先读 `authoring-agents-skills.md`**：治 F3 的姊妹类（description 落点规则）· 载体 = **登记不硬治**（该规则已单源在册，属执行面纪律；重复违反再升 ARQ）· 预期收益 = 免 RE 轮。

## 5. 一句总结

成本 = 子 agent 约 86 万 tokens / 挂钟约 27 min / 主线程一次性自办落地**；触点 = human 三次（立项 ack「同意立项」一次、OQ-1~4 裁定一次含破界明批、此前「紧急止血」路径选型一次）；制品 = 4 件（PROPOSAL + landing-summary + landing-verdict + retro）+ 8 个 canonical 落地文件。最险一刻 = `architect.md` 破自改递归边界若被当作普通改动执行——靠「授权写在选项里 + 判官焦点必核 + 逐字留痕」三处兜住。**保险丝判定：两方独立结构（判官独立 + verifier 独立）接住作者偏差**——判官 R1 独立抓到 F1（事实矛盾）/F2（规则违反）/F3（误引权威）三条起草者自检未发现的缺陷，核验官独立把结构门 advisory 归因到 main 合并而非本批私货（A3）；若只有作者自检，F1（SHOULD）会带病 FROZEN。最值得带走的一条 = **承重身份句必须正面写进常驻指令面**——本批的存在本身就是反例：一句已存在于 FROZEN PROPOSAL 的定义，只因没进 AGENTS.md，就让编排者本人连错两次。

## 6. 本环遗留

- **留根（FROZEN §6，去向已定）**：① check 齿（ARQ 建议 4）按 human 裁 OQ-3=(b) convention-only 留根——**去向 = 待将来有机制需求时另立 `/gov-change` 批（触 G3 恒 Track B）**；② `change-triage.md` §6 逃生门为显式豁免、未碰——**去向 = 无需处置（零门权纪律的显式不动项）**；③ ARQ-261016 结项评估——**去向 = landing-verifier 绿 + 本 retro 齐后由主线程裁**。
- **ADVISORY 处置（landing-verdict 三条，全非阻断）**：A1 = ARQ append 前多一枚 `---` 分隔符（该文件既有节间约定，正文逐字命中）→ **接受，不返修**；A2 = AGENTS.md 跨面 intake 行删了 `delivery/intake/<changeId>.md` 路径 token——**此为 FROZEN §2 T1 替换文本本身所授权**（冻结句即未含该 token），落地与冻结逐字一致；该路径 token 在 `change-intake.md` §2 仍在册，**接受，记为冻结文本编辑副作用、登记于本 retro**；A3 = 结构门三件 advisory 的合并基线归因（`docs/debt/tech/**` 随 main 合并 `8885bcea` 带入）→ **确认非本批，不处置**。
- **R2-N1（NIT，未落）**：§2 census 四形未统一标 scope——属 PROPOSAL 文本层打磨，**去向 = 不另立批；若同类 census 口径漂移复发，并入建议 2 的自检项**。
