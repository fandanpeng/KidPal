# PROPOSAL · 260924-gov-review-planner-orchestration-align

> **状态：FROZEN v0.3 · 2026-09-24 · 内容冻结，凭据齐备**——① human 立项 ack：2026-09-24 本会话（前端对齐「为什么改/往哪改/改动多大」三段 + OQ-1/OQ-2/OQ-3 均裁 (a) 按推荐位：范围 = 纯 G2 六条承载、uiux 件延批 D、噪声件与三件机械齿留批 C）；② architect 评审循环收敛：R1 BLOCK（F1–F4 + N1–N4，零项影响目标效果、零升级 human）→ v0.3 修订 → **R2 GREEN**（§7，两轮收敛未触熔断）；③ OQ 无未决项。主线程凭「立项 ack + GREEN」自冻，授权记本行。packet-bound landing 基准 = 本文件 §2 改法集 T1–T7 + §5 验证集（landing-verifier 比对基准 = v0.3 §2，与脚手架同形——R2 随行提醒收讫）。
> **版本沿革**：v0.1 起草（脚手架 + §5 全量实跑）→ v0.2 立项 ack + OQ 裁定记录 → **v0.3 = R1 findings 修订**（F1 T2 锚文「对端条目」→「对端面」· F2 T5① 锚文加粗→反引号 code-span · F3 T6b Step-8 指令改整句替换形与脚手架现文对齐〔判官推荐方向，脚手架零改动〕· F4 §5 工作树实况补认第 6 个 M〔changes/arch/INDEX.md，changes-index 过程制品〕· N1 T7 摘要行引文补全 + V5 改精确子串口径 · N2 crq-doctor 输出改标「摘要（数值逐字）」· N3 行号坐标系声明入 §2 硬项段 · N4 → 留根 9）。全部修订落 PROPOSAL 文本面，脚手架工作树零改动。
> **承载 ARQ**：`ARQ-260922-buildfeature-launch-prep-skip`（T1 全承载）· `ARQ-260922-pod-requirements-not-authority`（T2 全承载）· `ARQ-260923-dispatch-prompt-hard-terms`（T3 全承载）· `ARQ-260923-dispatch-oversight-uncodified`（T4 全承载）· `ARQ-260923-portfolio-decomposer-drift`（T5 承载①②，③留根→OQ-3）· `ARQ-260913-current-pointer-implicit-global`（T6 承载残余①③+brief 投影+调用侧带牙文面，17 解析点常驻报警机械齿留根→OQ-3）。不承载：`ARQ-260923-planner-roster-reference-drift`（→OQ-1）、`ARQ-260923-gate-tooling-noise-batch`（→OQ-2）、`ARQ-260918-pod-session-settings-identity-unobservable` 残余后半（VS Code 扩展实测前置未做，留根）。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。不触 `docs/contracts/**`、非跨系统结构性变更 ⇒ 不升 Track A。逐条 ARQ 登记时已标 Track B 三要件全 yes（buildfeature / pod-requirements 两条明文逐条；hard-terms / oversight / decomposer / current-pointer 四条纯编排文面同判）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（证成：全部改法为 G2 文面纪律步与命令模板改注，零机械闸判定语义变更、零 frontmatter 变更、零脚本变更，可整批回滚；不取 low 因触 planner 主编排文多处正文、语义面大，且承载 ARQ 中两条自标 riskTier medium——那是问题面风险，改法面如前述）。
> **review profile**：G2（触面枚举：`.claude/commands/build-feature.md` · `.claude/agents/planner.md` · `.claude/agents/portfolio-planner.md` · `.claude/references/task-intake-and-slicing.md` · `.claude/skills/kidpal-pod-launch-prep/SKILL.md`）+ 账本写回面（`docs/crq/gov/**`，非 G 层分类面）。**G1 触点为零、G3 触点为零、契约面触点为零、不新增任何门、不改任何 agent frontmatter**（OQ-1 裁 (b)/(c) 时此声明按裁定范围修订）。
>
> **起草期脚手架在场声明（诚实声明，循 260923-gov-review-arq-ssot-restructure 先例）**：为履行模板【硬项 · 起草期实证义务】，起草期已把 T1–T6 的全部文面改动写入工作树（未 commit）；T7（账本写回）**不在**脚手架内——账本状态转移只在 landing 相位发生，防「评审未绿先闭账」。§5 的 V4 断言已对脚手架态实跑（输出见 §5 记录）。**这不等于 landed**：若评审 BLOCK 或 human 否决立项，工作树按 findings 修订或整体回滚（`git checkout -- <file>`），不以已落为由倒逼通过。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/commands/build-feature.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/references/task-intake-and-slicing.md
docs/crq/gov/ARQ-260923-portfolio-decomposer-drift.md
docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md
docs/crq/gov/_archive/2026/ARQ-260922-buildfeature-launch-prep-skip.md
docs/crq/gov/_archive/2026/ARQ-260922-pod-requirements-not-authority.md
docs/crq/gov/_archive/2026/ARQ-260923-dispatch-prompt-hard-terms.md
docs/crq/gov/_archive/2026/ARQ-260923-dispatch-oversight-uncodified.md
docs/crq/gov/INDEX.md
docs/crq/gov/_archive/2026/INDEX.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：planner / portfolio-planner / build-feature 的编排文面把 18 包复盘连续五批实测出的六组跨棒复现摩擦（launch 面绕过、需求权威被替代、派发书缺硬约定、过目口径不成文、拆包快照腐坏、current 指针病）一次承接到 canonical——下一个 Pod 从入口到收尾只读文面即得完整纪律，不再靠翻 retro 与口头授权解释重学。

**现状差距**：六组处方全部活在 ARQ 条目与 pod-retro 记忆里，canonical 文面零承载（逐条证据见 §2 各 T）。其中两组已有 human 明确裁定待成文（oversight 口径 2026-09-23「不用每次都请示」；current 指针 checklist 为 planner private 习惯升级条文），两组已跨三包以上复现（decomposer 同颗雷三包各修一遍；指针病 3+3 包两代实证）。

**非目标（本批不做，防 scope creep）**：
- 不动任何文件 frontmatter（description / tools / name / model）——uiux 编制漂移件（planner.md:4 tools 行含 `uiux-author` 悬空引用）整件延后，见 OQ-1。
- 不动任何 `.claude/scripts/**`、`.claude/hooks/**`（G3 零触碰）——decomposer ③ warn 齿、current 指针 17 解析点常驻报警、`--expect-change-id` 闸级强制均留根，见 OQ-3。本批只做**调用侧文面**：把 planner.md 命令模板里的可选旗标改为必带（脚本参数面已存在，零脚本改动）。
- 不动 G1（AGENTS.md / CLAUDE.md / DEV-GOVERNANCE.md / README.md）、不触契约面。
- 不改任何机械闸判定语义；不新增 gate；不动 planner.md Step 1①「启动卡缺失→如实记录进 pod-events 并继续，不设门」既有裁定（Pod 内侧语义不变，T1 治主线程入口外侧）。
- 不承载 `ARQ-260918-pod-session-settings-identity-unobservable` 残余后半（非 CLI 形态降级面显式标注）——其前置实测（VS Code 扩展会话能否传 `--settings`/`--agent`）两批均未做，本批仍不做，留根。
- 不动 `kidpal-pod-launch-prep/SKILL.md:18` 的「moderate + persistence」字样——那是 `ARQ-260923-codemem-mode-doc-drift` 的目标（批 D 候补）；本批对同文件只加一句对偶触发源注记（T1），两批改动面不同行，合并序无冲突。

## 2. 问题、根因与改法（T1…T7）

**【硬项 · 起草期实证义务】** 本节断言纪律照模板执行；本批全部机械事实断言（闸值域 / 脚本输出路径 / 解析默认位 / 文面命中计数）已于 2026-09-24 起草期实测，命令与关键输出逐条记录于各 T 证据段与 §5。**行号坐标系（R1-N3）**：全文 file:line 引用一律以 2026-09-24 起草期坐标系为准（HEAD `5cf067f` + 脚手架工作树）；landing 后行号将随插入漂移（实例：task-intake :233→:245、planner taskpackage 示例 :177→:183），后续读者按文本字节内容定位、不按行号扑空。

### T1 · /build-feature changeId 直启绕过 launch 面预检

**问题**：`/build-feature <changeId>` 直启形状下，DEV-GOVERNANCE §4 图的 ② launch 面准备阶段（`--check-launch` 门 / worktree 等价化 / 专属图索引 / 已知红基线锚 / 启动卡 / board 预填）整段被绕过——命令正文从 Step 1 直接进 planner intake，全文不提 launch-prep；planner 侧「启动卡缺失不设门」裁定又使 Pod 裸起步无人引回。凡 changeId 直启的 portfolio 包必现。

**证据**（ARQ-260922-buildfeature-launch-prep-skip 登记实证 + 本批起草期复核）：① 文面锚——`.claude/commands/build-feature.md` 正文（2026-09-24 全文核读）Step 1 即「Source intake」，无 launch 面预检段；`.claude/agents/planner.md:35` 启动卡缺失处置逐字「缺失则**如实记录进 pod-events 并继续**，**不设门**」。② 执行锚——260921-launch-pipeline 落地当批即被绕过：m3 全批五包 board 在册（merged×4 / running×1）而五包 `launch-card.md` 全部缺失，旧路径 launch prompt 文件止于 `260917-m2-k3`。③ 专属图索引为 launch 面必办有漏建事故先例（ARQ-260905，2026-09-05）。

**根因**：launch-pipeline 批把 ② 阶段建给了「主线程手工调用 kidpal-pod-launch-prep skill」路径，而默认实现入口 `/build-feature` 的 changeId 直启形状没有任何文面把执行者路由回 ② 阶段手册——两入口之间缺一条路由。

**改法**：`build-feature.md` 在「逐步执行，并在每一步显式说明…」行之后、Step 1 之前插入一段非编号的「launch 面预检」段（不动既有 1–10 编号，不新增机械闸）。承重 landing 指令——锚定原文（逐字，现文件 :14）：

> 逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

在该行之后、`1. **Source intake…` 行之前插入以下完整段（插入后全文即此段 + 原 Step 1 起不动）：

> **launch 面预检（Step 1 之前执行；仅 changeId 直启形状触发，260924-gov-review-planner-orchestration-align T1）**：输入为 launch-argument 形状（`/build-feature <changeId>`，即 planner.md Step 1① 的 launch-argument 形状）时，先核 launch 面**四件**——① board 条目（`$(git rev-parse --git-common-dir)/devloop/pods/<changeId>.json` 在场）；② 启动卡（`delivery/taskpackages/<changeId>/launch-card.md` 在场）；③ 运行时 worktree（board 条目 worktreePath 已记录且该路径实为 git worktree）；④ 专属图索引（`list_projects` 在册含该 worktree 路径派生的项目条目，ARQ-260905 launch 面必办）。**四件齐 → 原样进 Step 1 intake；任一缺 → 不进 planner intake**，先按 `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 把 ② launch 面准备阶段补齐（该 skill「执行主体 = 主线程（human 会话）」条款与本命令运行位同口径）再进 Step 1。raw / human-ad-hoc 无 changeId 形状不触本预检。本预检是 **command 文面路由、非机械闸**：不改 planner.md「启动卡缺失不设门」既有裁定（那治 Pod 内侧交接断裂保护，本段治主线程入口外侧）；若未来把本预检升为 verify-gate / hook 判据即构成新增 gate，须按硬排除走 full 评审，非本条预批范围。

同批对偶注记：`kidpal-pod-launch-prep/SKILL.md` 的「执行主体」blockquote（逐字，现文件 :10）：

> **执行主体 = 主线程（human 会话）**；本手册 = [`docs/DEV-GOVERNANCE.md`](../../../docs/DEV-GOVERNANCE.md) §4 Dev Loop 图的 **② launch 面准备** 阶段操作手册（260921-gov-review-launch-pipeline T1 立此坐标）。

在该句末尾追加一句（同 blockquote 内）：

> 对偶触发源：`/build-feature` 的 launch 面预检（changeId 直启形状核 board 条目 / 启动卡 / 运行时 worktree / 专属图索引四件，任一缺即路由到本手册补齐再回 intake，260924-gov-review-planner-orchestration-align T1）。

**为什么这样改**：对根因「两入口缺路由」补最小文面路由；四件判据直接取自 ARQ 条目登记的原处方（board 条目 / launch-card.md / 运行时 worktree / 专属图索引），不发明新判据；「缺→不进 planner」的方向对严格于「裸起步」，且与 F-09 fail-closed 精神一致。

目标文件：`.claude/commands/build-feature.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改，仅追加对偶一句、不动其余行）

### T2 · 需求权威纪律步（sourceRefs 命中 requirements story 时先读原文、已定不问）

**问题**：Pod 不先读 requirements story 原文与 AC 就把已定项当开放问题问 human——需求已写明的行为仍给替代推荐让 human 拍板，需求权威被「human 临场偏好」静默替代；更危险的反向面 = human 临场裁定实际放宽了 story 口径而 requirements 未随裁回写，需求面与实现面即刻静默漂移。

**证据**（ARQ-260922-pod-requirements-not-authority 登记实证）：260920-m2-charwindow-card-fix pod-events 两条 human-decision（「右栏内容判据」「板端定位」）与 `docs/requirements.md` S-F3.2（:497-506，AC1 + 补充规则本已写明右栏内容权威口径）对读——至少部分被请示项需求已答；human 被连拉两次、planner 解读还被推翻造成返工轮。机制面：planner.md intake 链对 sourceRefs 命中 requirements story 只要求锚点引用形态（taskpackage 示例 :177「story 语义权威 = docs/requirements.md 现文」），无一步硬性要求通读全节并当行为权威；Step 5b 三触发形状（return / propose-drop / notify）覆盖「钉不成断言」，不覆盖「需求已写却推荐替代」。

**根因**：intake 链把 story 当**引用对象**（锚点形态合规即可）而非**行为权威**（须通读并蒸馏），升级判据里「需求已写明」不构成阻却事由。

**改法**：planner.md 在 1d 行（跨端判断）之后新增 1e 纪律步。承重 landing 指令——锚定 1d 行结尾原文（逐字，现文件 :42）：

> **判断不了按存疑向严填 counterparts，不填 exempt**（填 exempt 是把闸变成台账）。Step 7 前跑 `brief-inputs.mjs` 时该字段必须已定（脚本据此展开对端面）。

在该行之后插入以下完整行（新 1e，编号形态与 1c/1d 同款）：

> 1e. **需求权威（sourceRefs 命中 `docs/requirements.md` story 时，260924-gov-review-planner-orchestration-align T2）**：本包 `sourceRefs[]` 任一命中 requirements story（`S-Fx.y`）时，**finalize TaskPackage 与任何 AskUserQuestion 之前**必须通读该 story 全节（AC + 补充规则 + 线框行），把它当**行为权威**蒸馏进 acceptance/decisionCoverage——需求已写明即已答，**禁止「需求已写明但给替代推荐让 human 拍板」形态**。向上升级仅限三类：① 需求确实未定；② 需求内部矛盾；③ 需求与契约/实况冲突（冲突走 Step 5b 既有 return 形状；requirements 回写走 write-story 面，按 human-correction 留痕）。**同族边界**：human 主动给出偏离需求的指令属 human authority（F-08 面），按 human-correction 留痕并触发 requirements 回写，不算违规请示。本条不动 Step 5b 既有三形状语义，只收窄「把已定项当未定项」的误用面。

**为什么这样改**：对根因补「先读原文、已定不问」的硬性时序（finalize 与提问之前）；三类升级白名单 + human authority 边界逐字承接 ARQ 条目原处方，不扩不缩。

目标文件：`.claude/agents/planner.md`（改，正文 intake 节，不动 frontmatter）

### T3 · 派发书三条硬约定（Write 被拦回执 / scope 写权前置核对 / mutation 证据落盘责任人）

**问题**：派发书模板缺三条硬约定，全活在各棒 retro 的「下次改进」里、每棒重学一遍：① 子代理 Write 被环境拦后无标准回执姿势（K7 一棒三撞，前两次白跑转录）；② scope 写权无前置核对（builder 无 taskpackages 写权致 mutation 证据落盘改派/代落，K6 撞一次、K7 复现，其 retro 自判「同一坑跨棒复现 = 记账没升级成机制」）；③ mutation 执行证据落盘责任人无指定（K5 跨三棒只做到设计态对账，K6 写进派单才销账走通，K7 又回代落转述差）。

**证据**（ARQ-260923-dispatch-prompt-hard-terms，源 = loop-retro-2026-09-23 报告，delivery/changes/retro/2026-09-23-loop-retro.md）：K7 卡点 3/4（challenger / defender / adjudicator / acceptance-author 均中 Write 拦）、K6 卡点 3、K5 追加轮 4、display-fix 钩子节（enforce-scope 把正确活挡门外第二例）、m3-k1 卡点 6（第三例）；K7 retro 原话「这个约定应该在派单模板里，而不是让每个 agent 撞一次再学」。

**根因**：三条约定的载体是各棒 retro 记忆与 planner 临场想起来，派发步文面无模板条目——记账从未升级成机制。

**改法**：planner.md 起包 checklist blockquote（:31）之后、Step 1 之前插入一个新 blockquote 块。承重 landing 指令——锚定起包 checklist blockquote 的替换（T6 同时改该块，两 T 合并为一次整块替换，替换后全文见 T6 改法段；本 T 的新块插入在该整块之后）。新块全文：

> **派发书三条硬约定（每次 `Agent()` 派发适用，260924-gov-review-planner-orchestration-align T3；K5–K7 跨棒实证成文）**：① **Write 被拦回执姿势**——派单写明「若 Write 工具被环境拦截（『Subagents should return findings as text』类回执），不重试、把拟写制品**全文**作为回执返回，由 planner 代存到目标路径」；② **scope 写权前置核对**——派单前列写权表：谁落盘、落哪个目录、`.claude/scope-map.json` 对该角色是否放写；**无权即在派单时预指派代落人**（planner 或有权限角色），不让子 agent 撞 enforce-scope 钩子再学；③ **mutation 执行证据落盘责任人**——包含 mutation 取证要求的包，派单写明证据落仓内 evidence 文件（如 mutation-evidence/）并**指名落盘责任人**（第一选择 = 有写权的执行角色本人，无权则 planner 代落），不接受跨棒转述（K6「六条变异第一方实跑 + 原始输出落盘」已验证走通，K7 回落代落即产转述差）。三条均为派单模板条目：零新门、不改任何闸判据。

**为什么这样改**：把三条已验证有效的做法（K7 第三撞后的止损姿势 / K6 写权表 / K6 第一方落盘）从 retro 记忆升级为派发步 canonical 模板条目，对根因「载体错位」直接补位。

目标文件：`.claude/agents/planner.md`（改，新增 blockquote 块，与 T4/T6 同区）

### T4 · 分层出场口径成文（human 2026-09-23 裁定「不用每次都请示」的 canonical 承载）

**问题**：「派发书过目门」口径未成文——planner 每棒靠解释 human 一句口头授权判断是否免过目，m2-k2 / m2-k3 两棒如实标注「这是我的解释、可能超出授权」；口径不成文则要么逐次请示（纯摩擦）、要么自行解释授权（越权风险），且解释无法跨棒继承。human 2026-09-23 已当场裁定「不用每次都请示」，但裁定只活在会话记忆与 ARQ 条目里。

**证据**（ARQ-260923-dispatch-oversight-uncodified）：GC1 实核 planner.md 全文无「过目」字样（起草期 2026-09-24 复跑 `grep -c "过目" .claude/agents/planner.md` = **0**，输出见 §5 记录）；仓内「过目」成文点仅 portfolio-planner.md:46 门 1 与 task-intake-and-slicing.md:233，属拆包切法面非派发书面。正向实证：m3-k1 retro「分层是对的」、k5/k6/k7 三棒 human 只在真判断点出场（launch 授权 / 真机持板 / 部署与合并批准）、loop 中途零打断。

**根因**：分层出场纪律的载体是会话记忆 + retro 先例沿用，缺 canonical 条文；免除动作也无标准留痕形状，「谁授的权、免到哪」不可审计。

**改法**：紧接 T3 新块之后再插一个 blockquote 块（同区连排）。新块全文：

> **分层出场口径（派发书过目面，260924-gov-review-planner-orchestration-align T4；human 2026-09-23 裁定「不用每次都请示」成文化）**：① **常规派发不逐次请示**——acceptance-author / builder / fixer / integrator 等任务书派发前无需 human 过目，human 要看时才看。② **必升面逐条钉死、向严不减**——以下六类不在常规免除面内，仍按既有门与口径出场：合并门（supervisor ESCALATE 与人批后合并全路径）、契约门（`docs/contracts/**` 改动，architect-reviewer + human 签字制不动）、生产写入（ECS 部署与开闸）、high riskTier 的 supervisor ESCALATE、需求未定或与实况冲突（Step 5b 三形状）、F-08 人类显式覆盖留痕。③ **免除动作仍留痕**——human 给出概括性免除口径（「少请示」类）时，落 pod-events `human-decision` 事件，payload 记 human 原文与适用范围，使「谁授的权、免到哪」可审计（m2-k2/m2-k3 留痕先例沿用）。本条为口径成文：零新门、不改任何闸判据，必升面清单只收既有门、不新设。

**为什么这样改**：human 裁定已给（2026-09-23），本 T 是纯成文承载；必升面六类逐条取自 ARQ 条目原处方（向严不减），免除留痕沿用 pod-events 既有闭集（human-decision），零机制扩张。

目标文件：`.claude/agents/planner.md`（改，新增 blockquote 块，与 T3 同区）

### T5 · 拆包快照保鲜：milestone origin 填法成文 + intake 逐字段对磁盘核验

**问题**：portfolio-planner 拆包产物是「有保质期的快照」，两类腐坏无闸可拦：① origin 两键写出闸值域外的 `milestone` 值——同一颗雷 m2-k1 r1 红、m2-k3 intake 裸跑报红、m3-k1 r1 红，三包 planner 各自在 finalize 权内改成 `delivery-story` + milestone 溯源走 upstreamRef，各自重学一遍；② 引用腐坏（验收文件路径磁盘不存在 / 冻结钉点文件已进归档区 / 红基线命令 cwd 基准错 / sourceRefs 漏真实改动面）——机械闸只查字段形态，查不出「引用指向的东西还在不在」，m2-k2 四处引用腐坏、m2-k3 漏 4 处（最重 = 本包存在理由的承接面）、m3-k4 contractRefs 空数组被判缺。

**证据**（ARQ-260923-portfolio-decomposer-drift，源 = loop-retro-2026-09-23，5 包实证）+ 起草期机械实测（2026-09-24）：闸值域闭集逐字（`.claude/scripts/taskpackage-required-check.mjs:93`）`const ORIGIN_TYPES = [...SOURCE_TYPES, 'human-confirmed-plan']; // origin.type 值域 = sourceType 枚举 + 受控扩展`，:197 出集即红 `missing.push(\`origin.type(valid: ${ORIGIN_TYPES.join('|')})\`)`——`milestone` 不在集内；三包收敛先例实读（`delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/taskpackage.json` 与 `260917-m2-k1-char-link-defect-fix/taskpackage.json` 的 origin 块均为 `type/upstreamType = delivery-story` + `upstreamRef = delivery/milestones/milestones.jsonl#<milestone-id>` + `workstream`，upstreamFrozen 空）。m3 批起「intake 裸跑形态闸 + 逐字段亲核」已成 planner 自发惯例（连续多棒净赚）但未成文。

**根因**：① milestone 来源包的 origin 填法无 canonical 条文——task-intake origin 节有小 bug 包、hybrid 包填法段，独缺 milestone 来源包段，producer 只能猜；② 拆包→接棒的时滞里磁盘现实会漂，而接棒侧「逐字段对磁盘核验」是未成文的个人惯例，新棒靠翻 retro 自觉继承。

**改法**（两件；ARQ 条目 ③「引用字段磁盘存在性 warn 齿」为 G3 脚本面，留根→OQ-3，不在本 T）：

**T5①** `task-intake-and-slicing.md` origin 节，在「小 bug 包 origin 填法」段（锚定其结尾原文，逐字，现文件 :56）：

> `origin` 仍是一等强键，只是 type 非 proposal。

之后插入新段全文：

> **milestone 来源包 origin 填法（260924-gov-review-planner-orchestration-align T5，三包收敛先例成文）**：portfolio-planner 拆包以 milestone 为上游来源（`delivery/milestones/**`）时，`origin.type`/`origin.upstreamType` **必须取闸值域内枚举值**——`milestone` 不在 `ORIGIN_TYPES` 闭集（= sourceType 9 枚举 + `human-confirmed-plan`），写出必红（m2-k1 r1 / m2-k3 intake 裸跑 / m3-k1 r1 三包同雷各修一遍的先例）。收敛填法 = `upstreamType` 取 `delivery-story`，milestone 溯源走 `upstreamRef`（指 `delivery/milestones/milestones.jsonl#<milestone-id>`），`workstream` 照常：
>
> ```json
> "origin": {
>   "type": "delivery-story",
>   "upstreamType": "delivery-story",
>   "upstreamRef": "delivery/milestones/milestones.jsonl#M3-schedule",
>   "upstreamFrozen": "",
>   "workstream": "m3-schedule"
> }
> ```

同批 producer 侧指针一句：`portfolio-planner.md` Step 1（锚定其结尾原文，逐字，现文件 :35 末尾）：

> ……（建包方职责经 reference 下达、经既有委派链承接；gov-review-devloop-artifact-lifecycle DP-5）。

在该行句号后追加：

> **milestone 来源包**：origin 两键（`type`/`upstreamType`）取闸值域内枚举值、milestone 溯源走 `upstreamRef`——填法见 task-intake-and-slicing.md「milestone 来源包 origin 填法」节（`milestone` 不在闸值域，写出必红）。

**T5②** planner.md 在 T2 新增的 1e 行之后插入 1f 行，全文：

> 1f. **拆包快照保鲜核验（portfolio handoff 消费期，260924-gov-review-planner-orchestration-align T5）**：拆包产物是**有保质期的快照**——拆包到起包间隔越长，元数据与磁盘现实越可能腐坏，而机械闸只查字段形态、查不出「引用指向的东西还在不在」（m2-k2 四处引用腐坏 / m2-k3 漏 4 处真实改动面 / m3-k4 contractRefs 空数组，5 包实证）。Step 1① 命中 portfolio taskpackage 后、进 ② 归一前，**逐字段对磁盘核验**四问（m2-k2 棒总结）：① `acceptance[]`/`acceptanceTestsFile[]` 引用的文件是否存在；② 冻结件/红基线引用的对象是否仍在跑（未进归档区）；③ 红基线与验证命令的 cwd 基准是否正确（elf / host runner 路径基准）；④ `sourceRefs[]`/`docRefs[]` 是否覆盖真实改动面（上包标注「保留不动、由本包统一收」的承接面必须出现在 refs——那是本包存在的理由）。发现漂移在 finalize 权内修正并记 notes（`origin` 不可改——F-08 字段白名单）；修不了 → needs-triage 照旧。本条为 m3 批起 planner 自发惯例（连续多棒净赚）的成文化，纯纪律步、零新门。

**为什么这样改**：① 对根因「filling 口径缺 canonical 段」在 SSOT（task-intake origin 节）补段 + producer 指令处加指针，上游修一次消全批雷（ARQ 原处方「成本低可先行」）；② 对根因「惯例未成文」把四问升级进 intake 纪律步。ARQ 条目 ①② 原样承载，③（warn 齿）留根 OQ-3。

目标文件：`.claude/references/task-intake-and-slicing.md`（改，origin 节插一段）· `.claude/agents/portfolio-planner.md`（改，Step 1 追加一句）· `.claude/agents/planner.md`（改，新增 1f 行）

### T6 · current 指针面三时点成文 + 调用侧带牙（起包核指针 / 原子刷新 / 字节级复制 / brief 双份投影 / 命令模板必带旗标）

**问题**：`delivery/taskpackages/current/` 是隐性全局指针，指错包即静默假绿或簿记污染。两代实证：一代（home-p1 verify 九闸跑错包上下文全绿 / display-first-m1 簿记写进上一批 / schedule-reminder intake 投影指上一包）；二代（2026-09-24 追注：k4-reminder 漏刷 current 致两条 dispatch-completed 误落 k3 pod-events、m3-k1 漏传旗标致 telemetry 误投上一包「工具牙在但调用侧没带」、m3-k2 中途回填后忘刷 current 副本致 round 1 闸红）。同族两面：指针副本行尾字节差破 sha256 比较（screen-unification）；brief 双份维护——k5 起跨三棒被同一闸咬、k6 r1 再栽，「intake cp 两份」是各棒自发对策未成文。已有半张牙（dispatch-stamp 回显 + opt-in `--expect-change-id`，260918-closeout T2 落地），残余全在人面纪律与调用侧。

**证据**（ARQ-260913-current-pointer-implicit-global 正文 + main 侧追注，源 = loop-retro-2026-09-13 / 2026-09-23 第 3 轮）+ 起草期机械实测（2026-09-24）：① brief 双份投影的机械成因——`brief-inputs.mjs:605` 逐字 `const dir = resolve(projectDir, 'delivery/taskpackages', args.changeId);`（只写 changeId 目录），而 `taskpackage-required-check.mjs:26-30` 默认解析 `delivery/taskpackages/current/taskpackage.json`、:389 `const briefPath = join(tpDir, \`input-brief-${role}.md\`);`（在 current/ 目录找 brief）——两处合成「brief 必须双份在场」的机械事实，无任何文面承载；② 调用侧旗标面——`dispatch-stamp.mjs:71` 支持 `--task-package-id`、:220 支持 `--expect-change-id`（在场即硬校验 fail-loud），`journal-append.mjs:48` 支持 `--task-package-id`，而 planner.md [DISPATCH-STAMP]/[JOURNAL-APPEND] 两处命令模板均**不带**这些旗标（2026-09-24 全文核读）。

**根因**：指针的三时点（起包 / 中途回填 / brief 投影）都没有 checklist 硬条目或原子绑定条文；且已有的机械牙是 opt-in，命令模板不带 = 调用侧永远不会带上。

**改法**（四处；「17 解析点常驻报警」机械齿留根→OQ-3）：

**T6a 起包 checklist 增第⓪条**——承重 landing 指令：planner.md :31 起包 checklist blockquote **整行替换**。替换前原文（逐字）：

> **起包 checklist（条文，gov-review-devloop-closeout-hardening §3.2）**：起包即钉 retro 相位两条（从个人习惯升为条文）——① **pod-events 全程实时 append**（dispatch / escalation / human-decision / human-correction / friction 随发生随 append，**非收尾补记**；backfill 须标 `source=<actor>-backfill` + `payload.approx`，见 operating-system §5d）；② **closeout 前写 `pod-retro.md`**（planner 收尾相位，integrator closeout 之后、supervisor gate 之前，第一人称热记忆；post-cutoff closeout 完整性条件，Q5=warn）。

替换后完整行（既有①②逐字保留，只增⓪与引导句）：

> **起包 checklist（条文，gov-review-devloop-closeout-hardening §3.2；第⓪条 260924-gov-review-planner-orchestration-align T6 增）**：起包先核指针，再钉 retro 相位两条（从个人习惯升为条文）——⓪ **核 `delivery/taskpackages/current/` 指向本包**：读 `current/taskpackage.json` 的 `changeId` == 本包 changeId；不符或缺失 → 先刷新投影再开工（current 是隐性全局指针，指错包即静默假绿或簿记污染——home-p1 / display-first-m1 / schedule-reminder 三包一代实证 + k4-reminder / m3-k1 / m3-k2 二代复现）；① **pod-events 全程实时 append**（dispatch / escalation / human-decision / human-correction / friction 随发生随 append，**非收尾补记**；backfill 须标 `source=<actor>-backfill` + `payload.approx`，见 operating-system §5d）；② **closeout 前写 `pod-retro.md`**（planner 收尾相位，integrator closeout 之后、supervisor gate 之前，第一人称热记忆；post-cutoff closeout 完整性条件，Q5=warn）。

**T6b current/ 投影纪律条文**——锚定 Step 5 结尾原文（逐字，现文件 :46 末句）：

> 立即写入 `delivery/taskpackages/<change-id>/taskpackage.json`，同步刷新 `delivery/taskpackages/current/taskpackage.json`。TaskPackage 不能只留在聊天里。

在「TaskPackage 不能只留在聊天里。」之后追加：

> **current/ 投影纪律（260924-gov-review-planner-orchestration-align T6）**：① 投影刷新 = **字节级复制**（`cp` / fs copyFile 类），**禁 Write 重打全文**——Write 产 LF、拆包侧产 CRLF，行尾字节差会破 sha256 比较（screen-unification 实证）；② **canonical 改动与投影刷新原子绑定**——taskpackage.json 任何改动（含中途回填）后立即刷新 current/ 投影，不得「回填后忘刷」（m3-k2 round 1 taskpackage-required 红实证）；③ **brief 双份投影**——两份 `input-brief-*.md` 生成或重跑覆盖后，同步字节级复制到 current/（机械事实：`brief-inputs.mjs` 只写 changeId 目录，而 `taskpackage-required-check.mjs` 默认按 current/ 目录找 brief——k5 起跨三棒被同一闸咬、k6 r1 再栽，各棒「intake cp 两份」自发对策自此成文）。

同批 Step 8 补一处指针——锚定原文（逐字，现文件 :56 中段）：

> 把最终版重新写回 `<change-id>/taskpackage.json` 与 `current/taskpackage.json`。缺落盘记录不得派 builder。

替换为（括注嵌句内、句号移括号外——R1-F3 对齐：§2 文本与脚手架现文取同形，语义全同）：

> 把最终版重新写回 `<change-id>/taskpackage.json` 与 `current/taskpackage.json`（current/ 投影纪律见 Step 5 条文：字节级复制、原子刷新、brief 双份投影）。缺落盘记录不得派 builder。

**T6c [DISPATCH-STAMP] 命令模板必带旗标**——锚定 Step 9 [DISPATCH-STAMP] 块内命令模板原文（逐字，现文件 :60 Post-cutoff 命令行）：

> `node .claude/scripts/dispatch-stamp.mjs --dispatch-id <id> --actor <name> --phase <phase> --duration-ms <ms> --tool-uses <n> --terminal-status <status> --token-status exact|unavailable [--token-input <n>] [--token-output <n>] [--token-cache-read <n>] [--token-cache-write <n>] [--token-total <n>] [--token-source harness] [--token-reason <reason>]`

替换为（增两旗标 + 块尾增一句；其余逐字不动）：

> `node .claude/scripts/dispatch-stamp.mjs --dispatch-id <id> --actor <name> --phase <phase> --duration-ms <ms> --tool-uses <n> --terminal-status <status> --task-package-id <changeId> --expect-change-id <changeId> --token-status exact|unavailable [--token-input <n>] [--token-output <n>] [--token-cache-read <n>] [--token-cache-write <n>] [--token-total <n>] [--token-source harness] [--token-reason <reason>]`
>
> （块尾追加句）**〔T6，260924-gov-review-planner-orchestration-align〕`--task-package-id` 与 `--expect-change-id` 双必带**：前者免走 current/ 回退解析，后者在场即硬校验（解析值 ≠ 期望值 fail-loud）——「工具牙在、调用侧没带」的 m3-k1 telemetry 误投上一包实证自此闭合；旗标面脚本早已支持（`dispatch-stamp.mjs:71/:220`），本条只改调用模板、零脚本改动。

**T6d [JOURNAL-APPEND] 命令模板必带旗标**——锚定块内两条命令模板原文（逐字，现文件 :67）：

> `node .claude/scripts/journal-append.mjs --event-type round-outcome --actor planner --round <n> --role verifier|fixer --ok true|false [--caught "<bounded 一句抓到啥>"]`

与

> `node .claude/scripts/journal-append.mjs --event-type friction --actor planner --friction-type stall|scope-drift|scan-miss|degraded|over-scan --summary "<bounded 一句>"`

两条均在 `--actor planner` 之后插入 `--task-package-id <changeId>`（脚本既有旗标，`journal-append.mjs:48`，免走 current/ 回退解析），其余逐字不动；块尾追加一句：

> **〔T6〕`--task-package-id <changeId>` 必带**——与 [DISPATCH-STAMP] 同理，簿记不经 current/ 回退解析（k4-reminder 两条 dispatch-completed 误落上一包实证的同族面）。

**为什么这样改**：三时点各补其位——起包时点进 checklist 硬条目（ARQ 残余①原处方）；回填时点绑成原子步骤（m3-k2 retro 自判处方）；brief 投影把各棒自发对策成文（ARQ 同族第三面）；字节级复制是 ARQ 残余③原处方成文推广。调用侧带牙（T6c/T6d）是文面半张——把 opt-in 旗标改进命令模板使「必带」成为读文即得的默认姿势；闸级强制（不带即红）属 G3 机械齿，留根 OQ-3。

目标文件：`.claude/agents/planner.md`（改，起包 checklist / Step 5 / Step 8 / Step 9 两块命令模板，共五处，不动 frontmatter）

### T7 · 账本写回与状态转移（landing 相位执行，不在起草脚手架内）

**问题**：本批承载 6 条 ARQ，landing 后账本须如实反映承载结果，否则账实漂移（账本 SSOT 纪律：改状态去改条目文件再重跑渲染）。

**证据**：`docs/crq/gov/INDEX.md` 现摘要行逐字（整行，R1-N1 补全行尾注）「**摘要**：未结 21（待拍板 0 / 待触发 19 / 已接受风险 2）；已结 64（归档索引见下）。」。

**根因**：（簿记义务，无病灶。）

**改法**（landing 相位、landing-verifier 核验前完成）：
1. **4 条全承载结项**（buildfeature-launch-prep-skip / pod-requirements-not-authority / dispatch-prompt-hard-terms / dispatch-oversight-uncodified）：各条目正文 append 承载段（本批 changeId + T 编号 + landing 凭据），frontmatter `status: closed` + `closed: 2026-09-24`（实际 landing 日）+ `closedBy: 260924-gov-review-planner-orchestration-align T<n>`；由 `crq-render.mjs` 自动归位 `_archive/2026/`（人不搬文件）。
2. **2 条部分承载不结**（portfolio-decomposer-drift：①②落、③留根；current-pointer-implicit-global：残余①③+brief 投影+调用侧文面落、17 解析点常驻报警与闸级强制留根）：正文 append 部分承载段，`status` 保持 deferred，`trigger` 字段更新为留根去向（指向 OQ-3 裁定结果：批 C 或新登条目）。
3. 重跑 `node .claude/scripts/crq-render.mjs`（INDEX 再生 + 归档归位）；`node .claude/scripts/crq-doctor.mjs` 复核 red=0。
4. 状态转移各跑 `node .claude/scripts/changes-index.mjs --upsert 260924-gov-review-planner-orchestration-align` 单行钉进 `delivery/changes/arch/INDEX.md`。

**为什么这样改**：账本写回是 Track B 制品链义务；结项/部分结判定逐条对照 §2 承载面，不虚报（③与机械齿未落即不结）。

目标文件：`docs/crq/gov/ARQ-260923-portfolio-decomposer-drift.md`（改，部分承载追注）· `docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md`（改，部分承载追注）· `docs/crq/gov/_archive/2026/ARQ-260922-buildfeature-launch-prep-skip.md`（改后归档迁入）· `docs/crq/gov/_archive/2026/ARQ-260922-pod-requirements-not-authority.md`（改后归档迁入）· `docs/crq/gov/_archive/2026/ARQ-260923-dispatch-prompt-hard-terms.md`（改后归档迁入）· `docs/crq/gov/_archive/2026/ARQ-260923-dispatch-oversight-uncodified.md`（改后归档迁入）· `docs/crq/gov/INDEX.md`（渲染再生）· `docs/crq/gov/_archive/2026/INDEX.md`（渲染再生）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| dev-loop 入口 | `.claude/commands/build-feature.md` | G2 改 | T1 预检段；frontmatter description 不动；消费方 = /build-feature 调用者（human/主线程） |
| launch 手册 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | G2 改 | T1 对偶一句（:10 blockquote 内）；**:18「moderate + persistence」为 ARQ-260923-codemem-mode-doc-drift 目标、本批不动该行**；消费方 = launch 执行者 |
| 单 Pod 编排 | `.claude/agents/planner.md` | G2 改 | T2 1e / T3+T4 两个 blockquote / T5② 1f / T6 五处；**frontmatter（含 tools 行 uiux-author 悬空引用）不动**（OQ-1 推荐位前提）；消费方 = planner 会话 |
| 跨 Pod 编排 | `.claude/agents/portfolio-planner.md` | G2 改 | T5① Step 1 追加一句；frontmatter 不动；消费方 = portfolio-planner 会话 |
| intake SSOT | `.claude/references/task-intake-and-slicing.md` | G2 改 | T5① origin 节插一段；消费方 = planner / portfolio-planner / 拆包链文面 |
| 账本 | `docs/crq/gov/**`（2 改 + 4 迁 + 2 INDEX 再生） | 账本写回面 | T7；消费方 = crq-render / crq-doctor / 全仓 ARQ 引用 |
| 机械闸（读证据） | `.claude/scripts/taskpackage-required-check.mjs` · `brief-inputs.mjs` · `dispatch-stamp.mjs` · `journal-append.mjs` · `lib/governance-gate-shared.mjs` | 未触（G3 零改动） | 本批只读取其行为作证据（§2 各 T 实测），不改任何一行 |
| G1 指令根 | AGENTS.md / CLAUDE.md / DEV-GOVERNANCE.md / README.md | 未触 | 零改动（uiux 引用的 DEV-GOVERNANCE.md 四处随 OQ-1 延后） |
| 契约面 | `docs/contracts/**` | 未触 | 零改动 |
| 过程制品 | `delivery/changes/arch/260924-gov-review-planner-orchestration-align/**` | 新增 | 本批四件套（PROPOSAL / landing-summary / verdict / retro），Track B 义务制品，不入 landing-manifest（过程面非 canonical 改法集） |

**【硬项 · sweep 透镜义务】声明**：本批不落地任何新门/新断言齿（纯文面纪律步 + 命令模板旗标 + 账本写回），无判定 pattern 可全仓预跑——透镜义务不适用，如实声明。census 类清单仅一处（uiux 引用面计数），已机械枚举（Grep 全仓，命令与命中清单见 §0-E 记录与 OQ-1 背景）。

**【硬项 · 复扫模式完备性先验】声明**：本批不涉退役语面/残留清扫（uiux 引用摘除随 OQ-1 延后；legacy Sx-Ay 批量对齐不在本批），先验义务不适用，如实声明。

## 4. human 取舍点（OQ）

> **裁定记录（2026-09-24，human 立项 ack 同场）**：OQ-1 = **(a) 延后批 D**；OQ-2 = **(a) 留批 C**；OQ-3 = **(a) 全部留根批 C**。三项均按推荐位裁定，圈定范围与 v0.1 草案一致，manifest 零变化；留根清单 1/2/3 的「前提」自此为裁定事实。

### OQ-1 · uiux 编制漂移件（ARQ-260923-planner-roster-reference-drift）本批处置

**背景**：planner.md frontmatter tools 行与 Step 6b 引用的 `uiux-author`/`uiux-designer` 编制文件不存在（`.claude/agents/` 实况 14 个 def，无此二者）。起草期全仓机械枚举（Grep，2026-09-24）实测引用面**宽于 ARQ 登记面**：planner.md 2 处之外，另有 acceptance-author.md:23、dev-loop.md:55/:68、arch-change.md:26、**DEV-GOVERNANCE.md:54/:55/:245/:264（G1 面）**。且条目本身是二选一未决：补编制（按 6b 职责面起草 uiux-author.md）vs 摘引用（UIUX L2 面降级为「直翻 + human design review」成文，k3 实走形态）。
**为什么该 human 定**：补 vs 摘是编制设计判断（要不要为一个暂无在跑需求的角色建 agent def），非机械可判；且引用面含 G1，圈定范围直接决定本批 review 轨。
- **(a) 延后批 D（文档对齐批，推荐）**——理由：全写面 fan-out（12+ 处、含 G1）天然是文档对齐批的事，二选一可在批 D 起草期带着完整引用面清单拍；本批保持纯 G2 轻路径。代价：planner.md tools 行悬空引用存续至批 D（派发失败→降级的形态 k3 已实走验证，风险面 = 零行为变更，纯账面漂移）。
- **(b) 本批只做 planner.md 局部摘引用**（frontmatter tools + Step 6b 二选一摘除）——便宜，但制造「planner 干净、其余 6+ 处照旧」的新漂移，违反全写面纪律（memory 教训：别把看到的一处当全部）。
- **(c) 全引用面入本批**——12+ 处 + 触 G1（DEV-GOVERNANCE.md）→ 本批升 Track A full-adversarial，流程最重、与本批「轻路径止血」定位冲突。

### OQ-2 · gate-tooling-noise 四小修（ARQ-260923-gate-tooling-noise-batch）是否搭车

**背景**：四处一行级小修（commit-safety 提示口径 / verify.mjs 未知 phase 静默回落 / changes-index+mutation-check 传参形态 / enforce-scope-canary 去留），账本 trigger 明文「可搭任意批」。但四处落点全在 `.claude/hooks/**` 与 `.claude/scripts/**`（G3）。
**为什么该 human 定**：搭车与否改变本批 review profile（G2 → G2+G3），是流程重量与一次做完的取舍。
- **(a) 留批 C 门/解析面批（推荐）**——理由：与其主体（verify.mjs / 闸脚本解析面）同批，本批保持纯 G2；K7 实烧的 phase 静默回落随批 C 一并治。代价：四处噪声存续至批 C（均为非阻断消耗型）。
- **(b) 搭本批**——一次做完、账本销一条；代价：profile 变 G2+G3，评审面与验证集扩，且 hooks 改动与本批「编排文面」主题异质。

### OQ-3 · G3 机械齿类残余（decomposer ③ warn 齿 + current 指针常驻报警 + 旗标闸级强制）处置

**背景**：本批把三件机械齿的**人面/文面半张**全部落了（T5② 四问成文、T6 checklist⓪+命令模板必带旗标）， остав三件的**闸级半张**：① taskpackage-required / brief-inputs 对引用字段加磁盘存在性 warn（advisory 不拦门）；② 17 个经 current/ 解析点的常驻「指错包报警」（含 verify.mjs:123 病例承载者）；③ `--expect-change-id` 从「模板必带」升为「缺旗标即闸红」。三件全在 G3 脚本面。
**为什么该 human 定**：与 OQ-2 同轴（G3 是否入本批），但这三件是「文面已落、机械牙要不要现在咬」的向严节奏判断。
- **(a) 全部留根批 C（推荐）**——理由：本批文面纪律先跑一段，实测「文面成文后是否仍复现」再定机械齿的形态与力度（先例：dispatch-stamp 回显牙落地后，二代实证显示残余在调用侧而非解析侧——机械齿打点需要实况喂）；ARQ 两条目按部分承载追注、trigger 指向批 C。代价：三件闸级保护缺位期靠 T6 checklist⓪ 人面兜底。
- **(b) 升入本批**——机械牙一次咬全；代价：profile G2+G3、触承重闸邻接面（taskpackage-required 是起包结构闸），评审轮次预期增加，且 ② 的 17 点改造半径未经文面期实测校准。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**起草期实跑记录（2026-09-24，脚手架态；全部命令已实跑，关键输出逐字）**：
- **V1 preflight**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260924-gov-review-planner-orchestration-align/PROPOSAL.md` → `"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 13, "findings": []`；`anchorUnion` 13 成员与 landing-manifest 13 行**逐一对上**（5 个 G2 文面 + 2 个 deferred 条目 + 4 个归档迁入路径 + 2 个 INDEX），目录 token 零出现、注记零夹带。
- **V2 route-regression**：`node .claude/scripts/route-regression-check.mjs` → **exit 0**，尾行逐字「route-regression-check: 28 ok, 0 fail」。
- **V3 lint**：`node .claude/scripts/lint-descriptions.mjs` → **exit 0**（告警级），输出逐字「scanned 13, exempt 0, excluded 1 (architect.md), flagged 1 · ⚠ landing-verifier.md: over-length (363 chars > 320)」——该 flag 为**基线既有**（非本批目标文件、本批未触），如实记录不处置。frontmatter 零命中断言：`git diff -U0 -- <五个 G2 文件> | grep -cE '^[+-](description|tools|name|model):'` → **0**（实测）。
- **V4 全量 grep 已对脚手架态实跑**（2026-09-24，逐项命中数）：launch 面预检=1 · 对偶触发源=1 · 需求权威=1 · 「禁止「需求已写明但给替代推荐让 human 拍板」形态」=1 · 派发书三条硬约定=1 · 写权表=1 · 分层出场口径=1 · 必升面=1 · 过目=1（**基线 0**，T4 使该词首次入 planner.md）· milestone 来源包 origin 填法（task-intake）=1 · milestone 来源包（portfolio-planner）=1 · 拆包快照保鲜核验=1 · 指向本包=1 · 字节级复制=2 · brief 双份投影=2 · `--expect-change-id <changeId>` 出现次数=1 · `--task-package-id <changeId>` 出现次数=4。
- **V4-保留面实测**：uiux（planner.md，行数）=**2**（基线值：tools 行 + Step 6b 行；OQ-1(a) 前提下 landing 后不变）；moderate + persistence（SKILL.md）=1 不变；不设门（planner.md）=1 不变；`git diff --name-only -- .claude/scripts/ .claude/hooks/` → **空输出**（G3 零触碰实测）。
- **V5 crq-doctor 基线**：`node .claude/scripts/crq-doctor.mjs` → **exit 0**，输出摘要（三行压缩为一行、数值逐字——R1-N2 口径更正）「条目 103 条：gov=85 tech=17 requirement=1 / advisory 142 条（不阻断）/ RED 零命中 —— ok」（脚手架期账本未动；计数断言在 landing 后核）。
- **工作树实况**：`git status --porcelain`（除 .codebase-memory 常驻脏）为 **6 个 M** + 本批新增目录：5 个 canonical 面（planner.md / portfolio-planner.md / build-feature.md / task-intake-and-slicing.md / SKILL.md）+ `delivery/changes/arch/INDEX.md`（+1 行 = 本批 draft 状态行，`changes-index.mjs --upsert` 过程制品、AGENTS.md 状态转移钉行纪律的正当产物——非 canonical 越界改动、不入 landing-manifest），无其它改动。（R1-F4 补认：原申报「恰为 5 个 M」不实。）

**landing 前机械前置**：
- **V1**（FROZEN 前义务）：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260924-gov-review-planner-orchestration-align/PROPOSAL.md` → 预期：形态错 0 条；`anchorUnion` 成员名单 == landing-manifest 13 行**逐一对上**（多一个或少一个都是起草缺陷；目录 token 零出现）。
- **V2**：`node .claude/scripts/route-regression-check.mjs` → 预期 exit 0（planner.md / portfolio-planner.md / build-feature.md 的路由信号全保留——本批不动 description 与 tools 行，断言与改前基线一致）。
- **V3**：`node .claude/scripts/lint-descriptions.mjs` → 预期 exit 0；**且** frontmatter 零命中 grep（命令见上）预期输出 `0`。**零命中断言对账**（模板交叉一致性核义务）：本断言的保留面 = §0 非目标第 1 条 + §3 sweep 表「frontmatter 不动」声明 + OQ-1 推荐位 (a)；**条件联动**：若 OQ-1 裁 (b)/(c)，planner.md tools 行将变，本断言相应改为「除 OQ-1 裁定行外零命中」。

**landing 后核验（逐条可判，命中数为预期下限）**：
- **V4-T1**：`grep -c "launch 面预检" .claude/commands/build-feature.md` → ≥1；`grep -c "对偶触发源" .claude/skills/kidpal-pod-launch-prep/SKILL.md` → ≥1。
- **V4-T2**：`grep -c "需求权威" .claude/agents/planner.md` → ≥1；`grep -c "禁止「需求已写明但给替代推荐让 human 拍板」形态" .claude/agents/planner.md` → =1。
- **V4-T3**：`grep -c "派发书三条硬约定" .claude/agents/planner.md` → =1；`grep -c "写权表" .claude/agents/planner.md` → ≥1。
- **V4-T4**：`grep -c "分层出场口径" .claude/agents/planner.md` → =1；`grep -c "必升面" .claude/agents/planner.md` → ≥1；`grep -c "过目" .claude/agents/planner.md` → ≥1（改前基线 = 0，本批使该词首次入 planner.md——T4 成文的可判指征）。
- **V4-T5**：`grep -c "milestone 来源包 origin 填法" .claude/references/task-intake-and-slicing.md` → ≥1；`grep -c "milestone 来源包" .claude/agents/portfolio-planner.md` → ≥1；`grep -c "拆包快照保鲜核验" .claude/agents/planner.md` → =1。
- **V4-T6**：`grep -c "指向本包" .claude/agents/planner.md` → ≥1；`grep -c "字节级复制" .claude/agents/planner.md` → ≥2；`grep -c "brief 双份投影" .claude/agents/planner.md` → ≥1；**旗标断言用出现次数口径**（`grep -o … | wc -l`；`grep -c` 按行计数会少算——JOURNAL-APPEND 两条命令模板同行，起草期实跑对账揭示并更正本口径）：`grep -o -e '--expect-change-id <changeId>' .claude/agents/planner.md | wc -l` → **=1**（DISPATCH-STAMP 命令模板）；`grep -o -e '--task-package-id <changeId>' .claude/agents/planner.md | wc -l` → **=4**（DISPATCH-STAMP 模板 1 + JOURNAL-APPEND 两模板 2 + T6d 注记句 1）。
- **V4-保留面（零命中/不变断言，逐条已对账）**：① `grep -c "uiux" .claude/agents/planner.md` → landing 后 == 起草期基线值（OQ-1(a) 前提；对账保留面 = §0 非目标第 1 条）；② `grep -c "moderate + persistence" .claude/skills/kidpal-pod-launch-prep/SKILL.md` → ≥1 不变（对账保留面 = §0 非目标末条：codemem 件属批 D，本批不动该行）；③ `grep -c "不设门" .claude/agents/planner.md` → ≥1 不变（对账保留面 = §0 非目标：T1 明文不改「启动卡缺失不设门」裁定）；④ `.claude/scripts/**` 与 `.claude/hooks/**` 零 diff：`git diff --name-only -- .claude/scripts/ .claude/hooks/` → 预期**空输出**（对账保留面 = §3 sweep「机械闸（读证据）未触」行）。
- **V5（T7 后）**：`node .claude/scripts/crq-doctor.mjs` → red=0；`node .claude/scripts/crq-render.mjs` 复跑后 `git diff --exit-code -- docs/crq/` → exit 0（幂等）；`docs/crq/gov/_archive/2026/` 下出现 4 个本批结项条目文件、顶层不再有同名文件；`docs/crq/gov/INDEX.md` 摘要行**含精确子串**（`grep -F -c` → 1；子串包含口径，消「整行==」歧义——R1-N1）`未结 17（待拍板 0 / 待触发 15 / 已接受风险 2）；已结 68（归档索引见下）。`（算术对账：21−4=17、19−4=15、64+4=68；部分承载 2 条留在待触发不减）。
- **V6**：`node .claude/scripts/changes-index.mjs --upsert 260924-gov-review-planner-orchestration-align` → `delivery/changes/arch/INDEX.md` 该 changeId 单行在场且状态列 == 当前相位（draft/frozen/landed 按转移时点）。

**真空绿规约遵守声明**：V4 各断言均为「具体字符串 ≥/= 具体数」形态，反转实现（漏落任一 T）必红；V3/V4-④ 为零命中断言且逐条对账保留面（260921 批 §5 断言过宽教训——「零命中」断言的保留面清单在本 PROPOSAL 明文要求保留的项已逐一列出对账，无「全/均/无」裸断言）。

## 6. 留根清单

1. **decomposer ③ warn 齿 / current 指针 17 解析点常驻报警 / `--expect-change-id` 闸级强制**（OQ-3(a) 前提）→ 批 C（门/解析面）；T7 写回时两条目 trigger 更新为指向批 C。
2. **uiux 编制漂移件**（OQ-1(a) 前提）→ 批 D（文档对齐批），随带起草期实测的完整引用面清单（12+ 处、含 DEV-GOVERNANCE.md G1 四处）；「补 vs 摘」二选一在批 D 起草期拍。
3. **gate-tooling-noise 四小修**（OQ-2(a) 前提）→ 批 C。
4. **ARQ-260918 残余后半**（非 CLI 形态降级面显式标注）→ 前置实测（VS Code 扩展会话能否传 `--settings`/`--agent`）仍欠，留根不催；实测做成后随批落。
5. **T1 预检的机械化升格触发器**（自律件）：若文面路由落地后仍出现 changeId 直启绕过 ② 阶段的实例 ≥1，升格为机械闸的议题重开（届时构成新增 gate，须按硬排除走 full）——登记不硬治。
6. **T4 必升面六类的复评触发器**（自律件）：任何新门/新生产写入面设立时，同批核必升面清单是否需增补（向严不减的维护义务）。
7. **current/ 指针存疑样本——已核证非指针病（2026-09-24 起草期，本项闭合）**：核验撞见的 `current/verdict.json` 生产路径（m1-k1a Pod worktree）与承载 changeId（perf-k1）解耦样本，经 board 实读取证：`devloop/pods/260924-perf-k1-roundtrip-latency.json` 的 `worktreePath = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`，与 m1-k1a 条目同值（两条均 merged 态）——即 perf-k1 经 **worktree 池复用**（pod-coordination §2c 一树多包的常态形态）跑在 m1-k1a 树内，路径与包名解耦属无害形态，**非指针病新实例**，无需追注 ARQ-260913。
8. **settings Write deny 件**（互指，非本批）：human 终裁已移交 gov 批处置 → 批 B（机械强制层快批）候补；本批不触 `.claude/**settings.json`。
9. **[DISPATCH-STAMP] 旧句与必带旗标的口径注**（R1-N4 观察，自律件）：块内既有句「脚本无 TaskPackage 上下文时自动跳过（exit 0）」在模板必带 `--expect-change-id` 后仅对不带旗标的调用形态成立（260918 T2② 语义：旗标在场则无上下文亦 fail-loud）。本批按「其余逐字不动」保持；未来批次触碰该块时顺手补半句口径注（「跳过语义仅对不带 `--expect-change-id` 的调用形态成立」）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 判官 = architect（Track B 评审循环）· 2026-09-24 · 结论：**打回（BLOCK）**

**亲跑复核命令清单与关键输出**（全部本轮实测，非采信申报）：
- `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok:true, section2BodyFound:true, anchorCount:13, findings:[]`；anchorUnion 13 成员与 landing-manifest 13 行逐一对上（我逐一比对）。
- `node .claude/scripts/route-regression-check.mjs` → exit 0，尾行「route-regression-check: 28 ok, 0 fail」（== V2 申报）。
- `node .claude/scripts/lint-descriptions.mjs` → exit 0，「scanned 13, exempt 0, excluded 1 (architect.md), flagged 1 · ⚠ landing-verifier.md: over-length (363 chars > 320)」（== V3 申报，flag 为基线既有、非本批目标文件）。
- frontmatter 零命中：`git diff -U0 -- <五个 G2 文件> | grep -cE '^[+-](description|tools|name|model):'` → **0**（亲跑）。
- G3 零触碰：`git diff --name-only -- .claude/scripts/ .claude/hooks/` → **空输出**（亲跑）。
- `node .claude/scripts/crq-doctor.mjs` → exit 0，「条目 103 条：gov=85 tech=17 requirement=1 / advisory 142 条（不阻断）/ RED 零命中 —— ok」（数值与 V5 申报全对；引用形态见 N2）。
- V4 全部计数亲跑复核（不止抽查 3 项）：需求权威=1、禁止形态=1、派发书三条硬约定=1、写权表=1、分层出场口径=1、必升面=1、过目(planner)=1（HEAD 基线=0 亲核）、拆包快照保鲜核验=1、指向本包=1、字节级复制=2、brief 双份投影=2、`--expect-change-id <changeId>` 出现次数=1、`--task-package-id <changeId>` 出现次数=4、launch 面预检(build-feature)=1、对偶触发源(SKILL)=1；保留面 uiux(planner)=2、不设门=1、moderate + persistence(SKILL)=1——**与 §5 申报逐项一致**。
- 锚文与插入文本 byte 级机器比对（node 脚本，PROPOSAL 引文 vs `git show HEAD:` 原文 / 工作树现文）：build-feature :14 ✓、SKILL :10 全行 ✓、planner HEAD :31（T6a 替换前原文全行）✓、:46 末句 ✓、:56 中段 ✓、:60 DISPATCH-STAMP 模板 ✓、:67 JOURNAL-APPEND 两模板 ✓、portfolio HEAD :35 末句 ✓、taskpackage-required-check :26-30/:93/:197/:389 ✓、brief-inputs :605 ✓、dispatch-stamp :71（`--task-package-id` 显式旗标返回行）/:220（`--expect-change-id` flagVal 行）✓、journal-append :48 ✓；14 处插入/替换文本 13 处 byte 级一致（T6a 替换行既有①②byte 级保留亲证），**1 处偏差见 F3**。
- 先例 origin 块亲读：`260921-m3-k1-cloud-schedule-producer` 与 `260917-m2-k1-char-link-defect-fix` 的 taskpackage.json origin 均为 `type/upstreamType=delivery-story` + `upstreamRef=delivery/milestones/milestones.jsonl#<id>` + `workstream`、upstreamFrozen 空 ✓（== T5 证据申报）；SOURCE_TYPES 9 枚举亲数 ✓、`milestone` 不在 ORIGIN_TYPES 闭集 ✓。
- `docs/crq/gov/INDEX.md:10` 现摘要行亲核：「**摘要**：未结 21（待拍板 0 / 待触发 19 / 已接受风险 2）；已结 64（归档索引见下）。」；6 条承载 ARQ frontmatter `status: deferred` 亲核 → V5 算术 21−4=17、19−4=15、64+4=68 **成立**；crq-render.mjs 归档机制亲核（「位置 = 状态的纯函数」，closed+年份 → `_archive/<年>/`，人不搬文件）✓。

**Eligibility 独立核（作者自评不作数）**：触面 = 5 个 G2 文件 + 账本写回面，G1 零 / G3 零（机械实测）/ 契约面零 / frontmatter 零（机械实测）；planner.md 与 portfolio-planner.md 均非 self-def（architect.md）、非 review-loop def（hardExclusionsOf 闭集七件）；T1 明文自限「command 文面路由、非机械闸」且自带升格触发器（留根 5），不构成新增 gate。`hardExclusionsOf`（实际位置 `.claude/scripts/crq-ledger-shared.mjs:68`，任务书所引 `lib/` 子路径有误）的 `agentFrontmatter` 判据为**路径式**（`.claude/agents/*.md` 即命中），唯一消费方 = crq-doctor 对 ARQ 条目 paths[] 的 advisory（6 条承载 ARQ 均 `paths: []`，advisory 不阻断）——不构成路由硬排除。protocol「Eligibility 判据」的机检硬排除针对 frontmatter tools/name/model、write scope、authority 语义、self-def、review-loop def、gate 变更：本批正文节改动零命中（T4 是 human 既有裁定的成文化，必升面只收既有门、免除面仅限任务书派发前过目，未改任何 authority 语义）。Track B 升级判据（契约面 / 跨系统结构性变更）零命中。**pod-requirements 条目内「agent def 新节是否触硬排除请判官核」→ 裁定：不构成。Track B eligibility 成立。**

**实质面核对**：T1 与 planner「启动卡缺失不设门」裁定（HEAD :35，保留面亲测不变）外侧/内侧分界明文、与 F-02/F-09 互补无缝（四件核 launch 面就绪、F-09 核交接完整，覆盖无漏）；T2 不动 Step 5b 三形状、human authority 同族边界与 F-08（:42）一致；T5 三段填法（小 bug / milestone / hybrid）按上游来源类型互斥、无矛盾；1f「`origin` 不可改」与 F-08 字段白名单逐字呼应（历史上三包 planner 曾在 finalize 权内改 origin，系 producer 侧填法未成文时期的临场修，1f 将该面 fail-closed 交 needs-triage，向严、非矛盾）；T4 六类 == ARQ 原处方逐条，对照 Boundaries（合并门/契约门/升级闸）与 Red Flags 节核过：免除面严格限定「任务书派发前过目」，Red Flags 各停止条件（needs-triage、handoff 断裂、budget 红等）不受免除面波及，无既有门遗漏；T6 旗标脚本参数面亲证支持，DISPATCH-STAMP「无上下文自动跳过 exit 0」旧句与必带 `--expect-change-id` 的 fail-loud 并存见 N4（观察级）。T7 4 结 2 追注与 §2 承载面逐条一致，不虚报。脚手架 diff 范围恰 == §2 改法集（planner 11+/5−、pp 1+/1−、bf 2+、ti 12+、SKILL 1+/1−），无多改。

**Findings**（F1–F4 须修，N1–N4 记录级）：

- **F1（BLOCKING）T2 锚定原文非逐字**：PROPOSAL:88 引「…该字段必须已定（脚本据此展开**对端条目**）。」并标「逐字，现文件 :42」；磁盘实际（HEAD :42 == 工作树 :46，该行未被脚手架触碰）为「…该字段必须已定（脚本据此展开**对端面**）。」。承重 landing 指令的锚文失逐字 → 冻结后 landing-verifier 按 §2 锚文对磁盘将找不到该串。修法：§2 T2 锚文改「对端面」（两字）。不影响目标效果，无须升级 human。
- **F2（BLOCKING）T5① 锚定原文非逐字**：PROPOSAL:142 引「**origin** 仍是一等强键，只是 type 非 proposal。」并标「逐字，现文件 :56」；磁盘实际（task-intake-and-slicing.md:56，未被脚手架触碰）行尾为「`origin` 仍是一等强键，只是 type 非 proposal。」——code-span 格式被引成加粗格式。修法：§2 T5① 锚文改为反引号形态。同上，无须升级 human。
- **F3（SHOULD）T6b Step-8 括注：§2 文本与脚手架不一致**：§2（PROPOSAL:204-206）指令为在「…与 `current/taskpackage.json`。」该句**之后**插入「（current/ 投影纪律见 Step 5 条文：字节级复制、原子刷新、brief 双份投影**。**）」（句号在括号内）；脚手架实况（planner.md:62）为「…与 `current/taskpackage.json`（current/ 投影纪律见 Step 5 条文：字节级复制、原子刷新、brief 双份投影）**。**缺落盘…」——括注嵌入句内、句号移至括号外。语义全同，但 §2↔脚手架 byte 级比对是 landing-verifier 的核验基准，FROZEN 前必须二选一对齐（建议：§2 措辞改为「把该句句末句号替换为括注（…投影）+ 句号」以匹配脚手架现文；或按 §2 字面回调脚手架）。不影响目标效果，无须升级 human。
- **F4（SHOULD）§5「工作树实况」申报与实况不符**：PROPOSAL:306 申报「（除 .codebase-memory 常驻脏）恰为 5 个 M …+ 本批新增目录，无越界改动」；实况 `git status --porcelain` 另有第 6 个 M：`delivery/changes/arch/INDEX.md`（+1 行 = 本批 draft 状态行，`changes-index.mjs --upsert` 过程制品，属 AGENTS.md 状态转移钉行纪律的正当产物、非越界改动，但申报未如实带出）。修法：§5 实况行补认该 M 及其性质（过程制品面、不入 landing-manifest）。
- **N1（NIT）摘要行「逐字」引用与「==」断言口径**：T7 证据（PROPOSAL:238）称摘要行逐字而省略行尾「（归档索引见下）。」；V5（PROPOSAL:321）「摘要行 == `未结 17…已结 68`」存在整行相等 vs 子串包含的读法歧义（crq-render 渲染的整行含「**摘要**：」前缀与「（归档索引见下）。」尾注，整行相等读法必假红）。建议 V5 改「含精确子串」或写整行。算术本身无误。
- **N2（NIT）crq-doctor 输出「逐字」为压缩拼接**：实际输出三行（含 advisory 分项明细），§5 V5 以「·」拼成单行；数值 103/85/17/1、142、RED 零命中全对，仅引用形态非逐字。
- **N3（NIT）起草期坐标行号 landing 后漂移**：T4 证据「task-intake-and-slicing.md:233」（HEAD 坐标，T5① 插入 12 行后现文 :245）、T2 证据「taskpackage 示例 :177」（HEAD 坐标，现文 :183）。属 as-of 引用的常态，建议注明坐标系或改节锚，防后续读者按新坐标扑空。
- **N4（NIT·观察，不要求本批修）**：[DISPATCH-STAMP] 块内既有句「脚本无 TaskPackage 上下文时自动跳过（exit 0）」在模板必带 `--expect-change-id` 后，仅对不带旗标的调用形态成立（260918 T2② 语义：传了则无上下文亦 fail-loud）。「其余逐字不动」是本批已批准的设计决定，不构成缺陷；建议未来批次顺手补半句口径注。

**结论**：**打回（BLOCK）**——F1/F2（BLOCKING，承重 landing 指令锚文失逐字）+ F3/F4（SHOULD，§2↔脚手架一致性、§5 申报真实性）须在标 FROZEN 前修复；四项均为机械文面修复、修法方向已逐条给出，**零项影响目标效果，无须升级 human**。N1–N4 可随 F 项修订顺带处置或如实保留。R2 复核范围 = 四处修复点 + 修复未引入新偏差的回归确认（锚文重新 byte 比对 + preflight 复跑）。

### R2 · 判官 = architect（Track B 评审循环，同判官 resume）· 2026-09-24 · 结论：**GREEN**

**复核对象**：DRAFT v0.3（R1 findings F1–F4 + N1–N4 修订，全部落 PROPOSAL 文本面、申报脚手架零改动）。

**亲跑复核命令清单与关键输出**（全部本轮独立实测）：
- `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok:true, section2BodyFound:true, anchorCount:13, findings:[]`，anchorUnion 13 成员 ≡ landing-manifest 13 行（独立复跑，与主线程申报一致）。
- `node .claude/scripts/route-regression-check.mjs` → exit 0，「route-regression-check: 28 ok, 0 fail」（复跑）。
- `node .claude/scripts/lint-descriptions.mjs` → exit 0，输出与 R1 逐字相同（复跑；landing-verifier.md over-length 为基线既有 flag）。
- frontmatter 零命中 grep → **0**（复跑）；`git diff --name-only -- .claude/scripts/ .claude/hooks/` → **空输出**（复跑）。
- **脚手架零改动实证**：`git status --porcelain` 与 `git diff --stat` 与 R1 记录**逐项相同**（6 M + codebase-memory 常驻脏 + 本批新增目录；planner 16/pp 2/bf 2/ti 12/SKILL 2/INDEX 1）——v0.3 修订确未波及脚手架，§5「工作树实况」新申报（6 个 M + 第 6 个 M 性质标注）与实况相符。
- **修复点 byte 级比对**（node 脚本，引文 vs `git show HEAD:` / 工作树现文）：
  - F1：§2 T2 锚文（v0.3 :89「…（脚本据此展开**对端面**）。」）== HEAD planner :42 行尾 且 == 工作树 :46 行尾，byte 级 ✓。
  - F2：§2 T5① 锚文（v0.3 :143「`origin` 仍是一等强键，只是 type 非 proposal。」code-span 形态）== task-intake-and-slicing.md :56 行尾（endsWith 精确比对）✓。
  - F3：§2 T6b Step-8 改**整句替换形**——锚定原文（v0.3 :203「把最终版重新写回 `…json` 与 `current/taskpackage.json`。缺落盘记录不得派 builder。」）在 HEAD :56 byte 级在场 ✓；替换后整句（v0.3 :207，括注嵌句内、句号在括号外）== 工作树 planner.md :62 子串 byte 级 ✓——§2 与脚手架现文**取同形**，R1 偏差消除，landing-verifier 比对基准恢复无歧义。
  - F4：§5 实况行（v0.3 :307）补认第 6 个 M 并如实标注原申报不实 ✓（与本轮 git status 实测相符）。
- **R1 全套 byte 比对回归重跑**（防修订波及）：T1 build-feature 段 / SKILL 对偶句 / T2 1e / T3 块 / T4 块 / T5① 段首 + json 块（CRLF）+ portfolio 指针句 / T5② 1f / T6a 替换前后两行 / T6b Step 5 追加 / T6c 旧新模板 + 尾句 / T6d 两旧模板 + 尾句——**全部 PASS**。其中 T6a 两行按**原始行**比对（PROPOSAL 引文行与 canonical blockquote 行首「> 」为融合形态，引文原样即全行）：旧行 == HEAD :31 byte 级 ✓、新行 == 工作树 :31 byte 级 ✓。本轮比对脚本首跑曾对该两行误剥「> 」前缀报 FAIL，经逐字符 debug 确认为**判官侧脚本伪差**、非 PROPOSAL 缺陷，特此如实记录。
- N1：T7 证据引文（v0.3 :239）== `docs/crq/gov/INDEX.md:10` **整行** byte 级 ✓；V5 断言改「含精确子串（grep -F -c → 1）」且子串含行尾「（归档索引见下）。」，与 `crq-render.mjs:65` 渲染格式核对**吻合**（landing 后整行 = 「**摘要**：未结 17（待拍板 0 / 待触发 15 / 已接受风险 2）；已结 68（归档索引见下）。」，子串包含关系成立）✓；算术复认 21−4=17、19−4=15、64+4=68 ✓。
- N2：§5 V5 已改标「输出摘要（三行压缩为一行、数值逐字）」✓。N3：§2 硬项段已增「行号坐标系」声明（HEAD `5cf067f` 与实际 HEAD 相符；漂移实例 :233→:245、:177→:183 与 R1 实测一致）✓。N4：留根 9 已增（自律件）✓。
- Status 行 = 「DRAFT v0.3 …待 R2 复核至 GREEN 后标 FROZEN」+ 版本沿革 v0.1→v0.3 逐版枚举 ✓；v0.3 全部改动点与主线程申报清单一一对应，通读未发现申报之外的文本变化；R1 记录完整未被覆盖（append-only 保全）✓。

**结构字段核对（GREEN 结论随行义务）**：Status 行 / packet-binding 行 / landing-manifest 围栏块三件在场；manifest 13 行 ≡ §2 锚清单并集（preflight 机证）✓。

**结论**：**GREEN**——R1 四处修复点（F1–F4）全部落实且 byte 级复核通过，N1–N4 处置到位，回归重跑零新偏差，脚手架零改动实证成立。**无任何项须升级 human。** 主线程可凭「立项 ack（2026-09-24）+ 本 GREEN」标 FROZEN（协议 arch-review-protocol「Track B 七步流程」第 3 步）。随行提醒（非 finding）：标 FROZEN 时 Status 行机锚形态须为「FROZEN v0.3」样式（`FROZEN v<大>.<小>`，勿写成「FROZEN（v0.3）」——升级闸 isFrozen 判读形态，先例教训在案）；landing 相位按 §5 V1–V6 与 T7 顺序执行，landing-verifier 比对基准 = 本 v0.3 §2（F3 对齐后与脚手架同形，无歧义残留）。
