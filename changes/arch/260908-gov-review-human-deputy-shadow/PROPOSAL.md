# PROPOSAL · 260908-gov-review-human-deputy-shadow

> **状态：DRAFT v0.1 · 2026-09-08 · 主线程起草完毕，待 human 立项 ack（ack 后送 architect 评审循环；T2 agent def 正文按 DEV-GOVERNANCE:222 硬条款 §2 归 architect 起草、human 批准定稿）**
> **承载 ARQ**：无既有 ARQ。来源 = human 2026-09-08 指示链：「总结决策习惯 → 定义 human deputy agent（model fable）→ 先讨论」；决策习惯挖掘批已执行完毕，证据面 = 仓外 scratch `C:\AI\_human-deputy-mining\`（profile-v2.md 画像 12+9 条 / canon-draft-v0.1.md 规则书草案 17+2 条款 / report.md 审计，本批起草输入，语料本体**不入库**见 §0 非目标）。姊妹批 `260908-gov-review-input-surface-frontload`（dev-loop 输入面批）与本批文件交集仅 DEV-GOVERNANCE 一处、不同节不共锚（见 §3 ⚠️ 行）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。升级判据核对：不触 `docs/contracts/**`（契约面为零）、非跨系统结构性变更（治理/流程面单侧）→ 按判据不升 Track A。**诚实申报一处判据适用存疑点供判官复核改判**：本批是新设「决策辅助角色」的编制级动作——虽零权力转移（shadow 预裁不约束任何状态转移）、可回滚、全新增面，但涉及 human 判断点的**行为面**新增与 scope-map 新写槽；「governance 类变更不强制升级」与「存疑向严」在此相交，主线程自评 low-medium 走 Track B，判官若认定属「角色边界变更 → 全套」（DEV-GOVERNANCE:160 阈值行）请打回升 Track A。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：零权力转移、闸与合并链零改动、不启用即无影响故 downside bounded；但改六件承重/编制面 + 给全会话治理触点加可选行为，非纯成文。）
> **review profile**：G1+G2+G3（触面 = `docs/DEV-GOVERNANCE.md` 编制注与 Track B 条款 G1 成文（按 §6 表字面，姊妹批 260908 同型先例，归类请判官复核）+ 两份新 references、新 agent def、retro 模板 G2 + `.claude/scope-map.json` 新编制条目 G3；契约面为零。）
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/human-decision-preferences.md
.claude/agents/human-deputy.md
.claude/references/human-deputy-shadow-protocol.md
docs/DEV-GOVERNANCE.md
docs/templates/retro.template.md
.claude/scope-map.json
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：给治理环的三个人类判断触点（立项对齐包 / 批内 OQ 裁断 / 例外升级拍板）装一个 fable 档的**影子预裁官**——`human-deputy` agent 依据入仓的偏好 canon 逐点出「条款号 + 选项 + 一句话理由」的预裁，human 照单快扫或实裁，**同场对比数据逐批沉淀**，让将来「哪一类判断可以托付」从感觉问题变成命中率实测问题。本批**零权力转移**：预裁不约束任何状态转移、不是任何闸的凭据，human 两个判断点（AGENTS.md「前端对齐 + 例外反选」条款）原样不动。

**现状差距**（三节 §1 P1–P3 逐一实证）：human 是决策吞吐的唯一串行点且已被观察到在为此调度时间；deputy 类辅助能力没有任何零风险试床可测；决策偏好从未被成文为 agent 可用的单一事实源（976 episode 语料与 17 条 canon 现在只活在仓外 scratch，仓内零痕迹——本批就是把它制度化的第一批）。

**非目标（本批不做）**：
- **不改 `AGENTS.md` / `CLAUDE.md`**——「human 只在两个真正判断点出场」铁律字面不动（shadow 不占该位；V4 含「deputy」零命中的负断言）；
- **不预授权生效升级**——deputy 任何裁断即时生效、白名单权力转移，恒归后续独立批凭实测数据另行提案（OQ4；P-2.7「裁断权归属变更永不豁免」的直接应用）；
- 不给 deputy 任何执行权：不 landing、不 merge、不 push、不豁免、不 accept-as-risk、不供凭据（canon §0 P-2 九项 never 清单进 def 正文）；
- 不动前置升级闸五要素核验、不动 `human-approval-ref` 机制、不动 pod-events schema（deputy 事件型留生效批）；
- 不加新 hook/脚本——违禁语自检先 prose + 人核（C-06 节奏：执行测出价值再机检）；
- 不动 dev-loop 侧触点（planner 升级件的 deputy 影子留后续批）；
- **聊天语料不入库**（隐私面：含个人事务段与亲贴口令痕迹；体积：976 条全文）——仓内只入 canon 与画像结论，chat 锚点以「profile-v2 条款号」形式引用、如实注「语料本机保留不保证复现」（§6 留根）。

## 1. 问题与根因（地面真值）

### P1 · human 是决策吞吐的唯一串行点，且已观察到「围绕人做调度」的形态

- 触点计数实况（2026-09-08 本批起草现跑：`grep -h "human 触点" changes/arch/*/retro.md`）：已闭环治理批每批 human 触点 **1–6 次**（260829-spec-stories 6 次；260904/260906-arq 各 2 次；260907-batch-2 3 次——retro 逐行在册）。
- 围绕人的调度（chat 语料 verbatim，episode 锚见仓外 profile-v2）：「**先不立项，等晚上10点 token 5折的时候再跑review流程**」（e253，ainative 2026-08-25）；「还不能收官……你启动review流程，**我去睡觉**」（e274，同夜）；等待-恢复句式在 approve-go 桶高频（「继续，有token了」「分类器好了，你继续跑吧」）。
- **根因**：立项 ack / OQ / 升级拍板三个触点全部等人在场；人的响应节奏成了流水线节拍器。委托是自然出路，但委托前提是可测——见 P2/P3。

### P2 · 「判断能力」没有零风险试床：现有制度里没有任何影子对比面

arch 侧早就有对偶机制：charter 冻结 → challenger **盲攻**（`scenario-author.md:64` verbatim「单列才暴露留空的断链点(毛利缺口教训)」——先钉冻结判据再让攻方独立发挥）；dev 侧同理：验收冻结 → 红基线独立复验。**决策面是唯一没有这种试床的环**：deputy 合不合格，今天只能靠 human 在场时顺带感觉，没有「同题双裁、事后对比」的数据面。

**根因**：影子预裁制度缺位——不是能力缺位（模型档、canon 素材都已备齐，见 P3），是**承载件与协议**没有。

### P3 · 决策偏好已有实证画像与规则书草案，但全部在仓外、仓内零承载

本轮专项实测（2026-09-08 现跑，脚本 `extract-decision-episodes.mjs` + `make-views.mjs`，输出 `C:\AI\_human-deputy-mining\`）：三仓组 112 个会话 transcript 抽出 **976 条决策 episode**（752 typed 原话 + 205 条 AskUserQuestion 裁断 + 8 拒答）；带推荐位问题 215 题中直采推荐位 **155 题 = 72.1%**，偏离分三档（澄清反问挂起 / 实质换选 33 / 自造第三案）且**框架定义面偏离密度远高于运行面**（esp32 需求 brainstorm 期实证）——「哪些判断可托、哪些不可托」第一次有了分相数据。规则书草案 `canon-draft-v0.1.md`：17+2 条款，每条挂 ≥2 证据锚点，含永不豁免九项。仓内现状：`grep -ri "deputy" .claude/ docs/ changes/` **零命中**（2026-09-08 现跑）——全部工作成果尚未制度化，一次会话外即消失。

**根因**：偏好与协议未成文入仓 = 没有事实源，任何后续动作（含「不干了」这个选项本身）都无承重点。

## 2. 改法（T1…T6）

### T1 · 偏好 canon 入仓（SSOT，新文件）

新增 `.claude/references/human-decision-preferences.md`：decision-preferences（人类决策偏好规则书）**单一事实源**。结构 = 草案五段：§0 职权总则（P-0 框架面/运行面划界 + P-1 白名单生效 + P-2 永不豁免九项）、§1 程序条款 C-01..C-05（理解门/事实核证/建议即地板/偏离三不/改判常态化）、§2 实质条款 C-06..C-12（机械化节奏/严松分面/复用优先/修设计不绕行/精确命中/缓议带出口/例外特权归 human）、§3 运行留痕 C-13..C-17（攒批择时/留痕自含/专家先路/不创先例/委托件五段式）、§4 域偏好、§5 弱信号观察名单。

**定稿机制（OQ2 裁断对象）**：human 逐条审核（批注/改词/删条），定稿版 = landing 正文；判官评审期复核 canon 结构面（条款间冲突、证据锚缺失、与既有 DEV-GOVERNANCE 条文矛盾——如 P-2.1/2 与 §5 人类门定义的一致性），**内容取舍权在 human 不在判官**。证据锚格式：每条款双轨——①仓内锚（`changes/**` file:line，永久可核）；②chat 锚（`profile-v2 §M1/N-3 · e803` 形式，注「语料本机保留、不保证复现」）。目标文件：`.claude/references/human-decision-preferences.md`（新增）

### T2 · `human-deputy` agent 定义（新编制，正文归 architect 起草）

新增 `.claude/agents/human-deputy.md`。按 DEV-GOVERNANCE:222 硬条款 §2 verbatim「**agent 定义只归 architect 起草**：`.claude/agents/**` 的职责 / 边界 / 工具 / write scope 变更只能 architect 起草（**landing 恒 = 主线程 packet-bound**）+ human 审核批准」——本节只钉 def 骨架契约（①–⑥），正文由 architect 在评审期出候选稿、随 R1/R2 修订、FROZEN 前回灌 PROPOSAL §7 附件供 human 批准定稿：

- **① 档位**：frontmatter `model: fable`（同型先例：`architect.md:5` verbatim「`model: fable`」= FROZEN 默认档机制）；派发恒不带 model 覆盖——MODEL-TIER（`planner.md:57`）「升档覆盖仅限 builder/fixer」条款不冲突（frontmatter 默认档非派发覆盖；agent-model-guard 现仅拦 architect，本批**不扩**，理由见 §6 留根——无已证实违例面，C-10 精确命中原则）。
- **② 角色定性**：决策辅助位（影子预裁官），**非判官、非执行位**——与 author≠judge 原则（DEV-GOVERNANCE:28）无冲突：deputy 不判定任何制品合格性，只产出建议性预裁；判定权仍恒归 human/各独立判官。
- **③ 输入面**：决策包路径指针（PROPOSAL §0/§4 或升级件全文）+ canon SSOT（`human-decision-preferences.md`）——**不转抄 canon 正文进派发 prompt**（防双源漂移，260908 姊妹批 T4 自律件同款纪律）。
- **④ 输出面**：按 T3 协议落 `changes/arch/<id>/deputy-shadow.md`（append-only）；每预裁五要素 = 裁断点 id / 选项全文 / 预裁 / canon 条款号 / 一句话理由 + 偏离标记；裁不动或不合规（命中 P-2 九项、或依赖无机械证据的现状叙述）→ 输出 `refuse` 条目 + 一句「缺什么证据」，**不猜不造**（canon C-02）。
- **⑤ 工具面**：Read/Grep/Glob/Bash（只读）+ Write（write scope 见 T6 scope-map 槽，物理上只能写各批 deputy-shadow.md）。
- **⑥ 边界恒条款**：def 正文含逐字「永不 landing / 永不批准 / 永不放行 / P-2 九项永不裁 / 裁断不构成先例（canon C-16）/ 自身权限变更永不参与（P-2.7）」——自指红线成文进 def 与 canon 双承载。

目标文件：`.claude/agents/human-deputy.md`（新增）

### T3 · shadow 协议成文（SSOT，新文件）

新增 `.claude/references/human-deputy-shadow-protocol.md`，要素：

1. **触发相位**：主线程在决策包组装完成、呈 human 之前派 deputy（默认同步派、失败降级「无影子」不阻塞呈包——shadow 非任何转移前置）。触发触点范围 = OQ3 裁断值；
2. **制品面**：`changes/arch/<change-id>/deputy-shadow.md`，一批一文件、append-only；预裁条目结构如 T2④；
3. **对比回写**：human 实裁落定后，主线程在同条目 append 终局行：`HIT`（同项）/ `MISS`（异项）/ `DIFF-CREATED`（human 自造第三案——单列，这是 deputy 最不可代的能力信号）/ `REFUSE-MATCHED`（deputy 拒裁且 human 也未直裁）；
4. **分层计量**（report §5 偏差的直接条款化）：命中率按 ①触点类（立项/OQ/升级）②推荐源（真 Claude 推荐 vs 映射档期间推荐）③riskTier 三维分桶——**只记录不评价，本批不设及格线**；
5. **违禁语自检（prose 级）**：deputy 输出若出现「批准合并 / accept-as-risk / push / 豁免 / 授权部署」类执行语义 = 协议违规，主线程回写时点名、记 retro friction；机检化（脚本扫描）留执行测出需要再上（C-06 节奏，本批不造闸）；
6. **晋升观察集**：生效批的立项输入 = ≥3 个自然治理批的 deputy-shadow.md 汇总 + /loop-retro 消费行（T5）；晋升条件、白名单半径、五要素兼容设计**全部归晋升批，本批零预设**（防「先造机制后验需求」倒置）。

目标文件：`.claude/references/human-deputy-shadow-protocol.md`（新增）

### T4 · DEV-GOVERNANCE 两处投影（编制注 + Track B 触点句）

**承重 landing 指令（原文引用 + 替换后完整句）**：

1. `docs/DEV-GOVERNANCE.md:60` 现行编制注整行 verbatim 为「现行编制注：`supervisor` / `verifier` 不设独立 agent 定义（定义已于 2026-07-25 退役），以 **stage 身份**存续，由 planner Bash 直跑机械闸承载（`verify.mjs` round 证据 + `verifier-evidence-check.mjs` + `supervisor-verdict-gate.mjs`，**不派 agent**）。」——在其后追加独立一句：「辅助位编制注（260908 批）：`human-deputy` 为决策辅助 agent（影子预裁，协议 SSOT：`.claude/references/human-deputy-shadow-protocol.md`，偏好 SSOT：`.claude/references/human-decision-preferences.md`），非判官非执行位——human 两判断点（立项 ack + 例外升级拍板）语义零变更，预裁不约束任何状态转移、不构成任何闸的放行凭据。」
2. `:157`「3. **architect 评审循环**……影响目标效果的打回项升级 human；**3 轮未 GREEN → human 三选一**……」与 `:158`「4. **落地序列**（状态不可合并：立项 ack ≠ FROZEN ≠ landing 授权）……」两 bullet 内，凡呈 human 触点句后各加同款一句（完整文本见协议件，正文不重复）：「呈包可附 deputy shadow 预裁（可选辅助，见协议件；非状态转移前置）。」

理由：编制与触点行为须在册否则后人无从发现 deputy 存在（「期望只活在判定代码里、起草侧零成文」老路——DEV-GOVERNANCE:291 头注教训的镜像面）；**不新增规则语义**，纯登记指针。目标文件：`docs/DEV-GOVERNANCE.md`

### T5 · retro 模板触点槽加影子行

`docs/templates/retro.template.md:64` 现文 verbatim 片段「`<成本（tokens / 时长）/ 触点（human 几次）/ 制品数 / 对照前几乘客>`」——「触点」槽扩为「触点（human 几次；deputy shadow：预裁 n / HIT n / MISS n / DIFF-CREATED n / refuse n，见 deputy-shadow.md，无则注'无影子'）」。消费面：/loop-retro cadence 汇总各批行成命中率视图（P-4 观察集输入）。理由：留痕不模板化的东西三批后就没人写（五条→六条清单同源教训）。目标文件：`docs/templates/retro.template.md`

### T6 · scope-map 编制条目（G3 结构边界动作，随批呈 human）

`.claude/scope-map.json` agents 段新增 `"human-deputy"` 条目，形态照 `landing-verifier` 条最小化：allow = `["changes/arch/*gov-review-*/deputy-shadow.md", "changes/arch/*arch-review-*/deputy-shadow.md"]`，deny = `["ESP32-S3-RLCD-4.2-Demo/**", "firmware/**", "server/**", "tools/**", "docs/**", ".claude/**", "AGENTS.md", "CLAUDE.md", ".planning/**"]`（`.claude/**` 已在 universal bashDenyWrite 双保险）。注册语义如实声明：scope-map 对**未登记** agent 不设 per-agent 约束（scope-map `agents._doc` verbatim「未登记 agent 不受 per-agent 约束」）——登记本身是把 deputy 从「无边界」收进「窄槽」的动作，属结构边界编制（先例 260907-batch-2：编制与写槽登记归 human 裁），随本批立项 ack 覆盖 + 判官复核最小性。目标文件：`.claude/scope-map.json`

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 偏好 SSOT | `.claude/references/human-decision-preferences.md` | G2 新增 | human-deputy def（T2 ③ 输入面）、shadow 协议（T3）、后续晋升批 |
| agent 编制 | `.claude/agents/human-deputy.md` | G2 新增（正文 architect 起草 + human 批准，DEV-GOVERNANCE:222 §2） | 主线程三触点派发；Agent 工具编制可见 |
| 协议 SSOT | `.claude/references/human-deputy-shadow-protocol.md` | G2 新增 | 主线程/retro 模板（T5）/晋升批 |
| 治理成文 | `docs/DEV-GOVERNANCE.md` | G1 成文（编制注 + 触点指针两句，零规则语义变更；G1/G2 归类请判官复核） | 全员常读面；姊妹批同文件不同节（见下行 ⚠️） |
| 模板 | `docs/templates/retro.template.md` | G2 触点槽扩写 | 各批 retro 作者（主线程）；/loop-retro |
| 强制层 | `.claude/scope-map.json` | G3 新增编制条目（最小写槽） | enforce-scope hook（消费方零代码改动——agents 段数据驱动） |
| 姊妹批冲突 | `changes/arch/260908-gov-review-input-surface-frontload/` | ⚠️ 同文件不同节 | DEV-GOVERNANCE：该批动「起包格式期望前置」节 `:289-297`，本批动 `:60/:157-158`——零共锚；两批并跑时后合批 rebase 消文本冲突，无语义碰撞；已在两批 INDEX 行可见 |
| AGENTS.md / CLAUDE.md | — | **未触（零）** | 两判断点铁律字面不动（V4 负断言守护） |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| hooks / settings / verify-gates / pod-events / dev-loop 四 agent def | — | **未触** | model-guard 不扩（无实测违例面）；事件型归晋升批 |

**【硬项 1b · sweep 透镜义务】** 新判定 pattern 全仓预跑（2026-09-08 现跑）：①「deputy」全仓 census：`grep -ri "deputy" .claude/ docs/ changes/` → **零命中**（全新增面、无双源残留；命令输出留痕于本会话）；②触点覆盖 census：`grep -h "human 触点" changes/arch/*/retro.md` → 已闭环批 1–6 次/批、10 行在册（§1 P1 引用）；③写槽 pattern：`deputy-shadow.md` 全仓零命中，与既有 verdict/review-rN 等批内制品名无碰撞（landing-verifier 槽位 glob 复核）。

**【硬项 1c · 复扫模式完备性先验】** 不适用：本批无退役语面/残留清扫（全新增面，无「旧形态改完复扫」需求）。

## 4. human 取舍点（OQ）

- **OQ1 · 形态**：(a) **影子预裁（推荐）**——deputy 只出预裁、human 恒实裁，零权力转移，命中率攒数据；(b) 直接有限生效——白名单（riskTier low-medium、非契约、非升级）内 deputy 裁断即时生效、human 事后翻案，省 70%+ 触点但把权力转移跳过实测直接落闸（五要素 `actor:"human"` 兼容问题当场就得裁）；(c) 完全代理（含升级拍板位）。推荐一句话：canon C-06 自己写的——机制先跑实证再上承重；且 M1 分相数据显示**框架面偏离高发**，生效白名单没影子数据支撑就是裸赌。
- **OQ2 · canon 定稿批准方式**：(a) **human 逐条审定（推荐）**——立项 ack 前把 canon-draft 批注完，定稿版即 landing 正文，判官只复核结构面；(b) 整包批准 + 评审期反修——快但偏好文过判官手会漂味；(c) 先 land 标 DRAFT、生效前再逐条——渐进路径，260906 OQ3 有明确否先例（「先成文跑一阵、漏了再升」被 human 否掉），不推荐。
- **OQ3 · 影子触发触点范围**：(a) **三类触点全影（推荐）**——含例外升级拍板包：升级面样本最稀缺而托付价值判断最需要它，shadow 无约束力故敏感面照影零风险；(b) 仅立项 + OQ，升级包不影；(c) 仅批内 OQ（最省 token，但立项对齐面=「委托权演练位」恰是最该测的）。增量成本：每触点一次 fable 派发（读一个决策包 + canon，估 20–40k tokens/触点），按 14 批/半月 × 触点 1–3 个估算为治理环现有成本的个位数百分比。
- **OQ4 · 晋升是否预授权**：(a) **不预授权（推荐）**——本批只建数据面，晋升批凭 ≥3 批影子数据另行提案（连及格线本身都归那批定）；(b) 本批预写晋升判据（如「连续 3 批 HIT≥90% 自动升白名单」）。(b) = 给未验证的能力预签授权书，违反 canon C-16 不创先例 + P-2.7，不推荐。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · 结构门**：`node .claude/scripts/landing-fidelity-check.mjs`（landing-verifier 例行）+ manifest ≡ §2 各 T 锚清单并集（6 路径逐字等）。
- **V2 · def 装载与格式自检（真空绿规约：先断言盲测题非空）**：以 **历史批盲测**跑首次派发——题库 = `260907-gov-review-merge-friction` §4 三题 + `260904-gov-review-escalate-merge-approval` §4 三题（两批 OQ 选项全文在册、human 终裁 verbatim 在册于各自 PROPOSAL/retro）。派发 `human-deputy` 只读两批 PROPOSAL §4（**不给 §7 评审记录与终裁段**），按协议出预裁落本批 `deputy-shadow.md`。断言：①6 题预裁条目全在场、五要素齐（条款号非空）；②refuse 型允许存在不判错；③格式 parse 通过。
- **V3 · 首基线对比**：主线程按 V2 预裁 vs 在册终裁逐题回写 HIT/MISS/DIFF-CREATED——**无及格线**（本批验证「机械可行 + 格式合规」，能力评价归观察集；结果行如实进本批 retro 触点槽，预期量级参考：全库 72.1% 直采）。
- **V4 · 投影 grep（正负双断言）**：正：`grep -c "human-deputy" docs/DEV-GOVERNANCE.md` ≥1；`grep -c "deputy" docs/templates/retro.template.md` ≥1；`.claude/scope-map.json` JSON parse 通过且 `agents.human-deputy.allow` 恰等于 T6 两路径。负（边界字面守护）：`grep -ri "deputy" AGENTS.md CLAUDE.md .claude/agents/planner.md .claude/agents/portfolio-planner.md .claude/hooks/` → **零命中**（两判断点铁律不动、dev-loop 不触、无新闸）。
- **V5 · 违禁语自检（首跑人工）**：V2/V3 产物 `deputy-shadow.md` grep `批准合并|accept-as-risk|push|豁免|授权部署` 零命中；命中即协议违规记 friction（不阻断本批——机制首跑诊断）。
- **V6 · 交观察（非门）**：≥3 个自然治理批后 /loop-retro 汇总命中率三维分桶进晋升决策材料（OQ4=a 的数据腿）。

## 6. 留根清单

- **晋升批（本批的全部意义所在）**：有限生效白名单设计、五要素/`human-approval-ref` 兼容方案（deputy 永远造不出 `actor:"human"` 凭据——生效批必须正面回答 ESCALATE 合并批准怎么处）、pod-events `deputy-decision` 事件型、晋升判据与及格线——凭影子数据另立，本批零预设。
- **dev-loop 侧触点**：planner 升级件（supervisor ESCALATE 等）的 deputy 影子——架构同型但触点面在 dev-loop，单独批。
- **违禁语机检化**：V5 若三批内零违规保持 prose；若出现违规再评估扫描脚本（C-06）。
- **agent-model-guard 扩展**：deputy 派发被带 model 覆盖的风险现无实测违例——不泛化扩拦（C-10）；若晋升批改派发链一并评。
- **证据可复现性**：chat 语料与 profile-v2 留本机 scratch，不入库；canon 内 chat 锚的复现策略（脱敏导出 docs/notes/ vs 仅仓内锚）归晋升批定。
- **human 操作面**：轮换 ECS root 口令（transcript 明文历史存在，report §3）；与姊妹批的 DEV-GOVERNANCE 合流顺序（谁后合谁 rebase）。
- **自律件（不进模板）**：主线程呈包时必须**先影后呈**不偷序；对比回写不修饰（MISS 照记）；deputy 预裁永不出现在给 landing-verifier 的证据链里（防影子升格为准据）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
