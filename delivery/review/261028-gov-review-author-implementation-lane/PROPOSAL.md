# PROPOSAL · 261028-gov-review-author-implementation-lane

> **状态：FROZEN v1.0 · 2026-10-28 · human 最新指示主线程直接实施；R1–R5 设计评审记录保留，实施后由 architect-reviewer 审真实 diff、landing-verifier 独立验证**
> **承载 ARQ**：吸收 `ARQ-281028-mutation-fixture-consumer-layer` 的层别义务但留待 P9 实证后结项；`ARQ-281028-assembler-import-closure-and-root-param` 本批不消化、不结项，旧通道仍需要其处方便利。
> **review 轨**：Track B 主路径自评。理由是触 G1、agent 定义、hooks、scripts、scope-map 与合并闸，但 human 已批准的设计记录已收敛方向；是否升 Track A 由 architect-reviewer 只升不降判定。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **landing-lane**：`packet-bound`（仅本批）。human 于 R5 后明确指示主线程直接实施，跳过继续起草循环；scope-map 直接落稳定终态，不再执行临时授权→作者实现→终态恢复的三态链。本批建立的新 author-implementation 通道只由后续合格批消费。
> **one-time-bootstrap（已被后续 human 指令取代，不再执行）**：2026-10-28 human 显式授权（设计记录 §9，主线程落地前可行性核后拍板）。理由是现状为 6 组逐字块 + 20 件行为规格，严格 packet-bound 会迫使主线程按 prose 代写或补齐逐字块；授权范围仅本批一次，主线程先机械落 scope-map 临时授权 edit，architect 再实现其余 25 件，不构成后续批先例。本批为混合面批，human 路线 B 授权即 T1 判据③与混合面绝对句的一次性例外；绝对句自本批 canonical 落地起对新批生效。
> **bootstrap-write-scope（已被后续 human 指令取代，不再执行）**：2026-10-28 human 经 AskUserQuestion 显式确认临时扩为十件：`AGENTS.md`、`docs/DEV-GOVERNANCE.md`、`.claude/agents/architect.md`、`.claude/agents/architect-reviewer.md`、`.claude/agents/review-moderator.md`、`.claude/agents/adjudicator.md`、`.claude/agents/landing-verifier.md`、`docs/templates/PROPOSAL.template.md`、`docs/templates/architect-landing-summary.template.md`、`.claude/route-fixtures/surface-alignment.json`。其中五件 agent 定义属自改递归高敏面，已获 human 显式确认；scripts/hooks 基础授权不变，批终态必须恢复稳定 scope-map。
> **riskTier**：medium。开放受限作者写权并改合并凭据，爆炸半径在治理执行层；以路径白名单、非 main 拦截、独立审稿、同树验证和合并时 Git 重算收口。
> **review profile**：G1+G2+G3；零契约、零 spec、零 firmware/server。
> **意图基线**：`delivery/workstreams/gov-debt-reduction/261028-author-implementation-lane.design.md`（human 已批准的整体设计与补充裁定）。

```landing-manifest
AGENTS.md
docs/DEV-GOVERNANCE.md
.claude/commands/gov-change.md
.claude/references/gov-change.md
.claude/references/arch-change.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/agents/review-moderator.md
.claude/agents/adjudicator.md
.claude/skills/merge-to-main/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/agents/architect.md
.claude/scope-map.json
.claude/route-fixtures/surface-alignment.json
.claude/hooks/enforce-scope.mjs
.claude/scripts/release-pr.mjs
docs/templates/PROPOSAL.template.md
docs/templates/architect-landing-summary.template.md
.claude/scripts/assemble-edits.mjs
.claude/agents/architect-reviewer.md
.claude/agents/landing-verifier.md
.claude/references/track-b-judge-brief.md
.claude/references/track-b-verifier-brief.md
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
```

---

## 0. 想实现的效果与差距

目标效果是：对纯 `.claude/scripts/**` / `.claude/hooks/**` 治理批，允许 architect 在 FROZEN 设计 manifest 内直接实现并提交，由独立 reviewer 审真实 diff、独立 verifier 验同一 commit/tree，主线程不再逐字代写；其它治理面继续使用现行主线程 packet-bound 通道。新通道证明的是「审、验、合并为同一实现」，不是降低检查。

现状差距由 P8 实测给出：42 处编辑、15 件 manifest 形成 1434 行 PROPOSAL，并产生 4 个一次性仓内脚本、18 个仓外装配或 overlay 副本、至少 6 遍完整装配。根因不是缺少独立判官，而是 author 不能提交实现、主线程必须代写、PROPOSAL 必须逐字承诺未来代码三者叠加。

非目标：不开放契约、spec、G1、settings、agent 定义、scope-map、verify-gates 或 firmware/server 的常态作者直达；不授予 merge、push、自批或自签 verdict；不把 P9 的 scripts 试点宣称为 hooks 新通道实证；不把本批 human 路线 B 的一次性自用解释为后续批可自行 bootstrap。

本批不引入产品新子系统、外部依赖或数据迁移，不命中新机制 spike 的三类判据；治理机制本身以 P8 事故证据和本批临时 Git 仓夹具作先导验证。

## 2. 问题、根因与改法（T1–T6）

### T1 · 把单一 packet-bound 后端改成显式双通道，不扩大适用面

**问题**：现行字面规则把「主线程代写」当成唯一 landing 形态。必须同步修改的规则不是只有 architect 自身定义：`AGENTS.md:19-20` 规定主线程落地、核验后提交，`AGENTS.md:33` 规定唯一通道；`docs/DEV-GOVERNANCE.md:147` 与 `:210-214` 规定 G1/G2/G3 全由主线程落地；`.claude/references/arch-review-protocol.md:48-56` 的七步与 `:68` 的通道声明、`.claude/references/gov-change.md:29-38`、`.claude/references/arch-change.md:31-40`、`.claude/references/architect-operating-model.md:56`、`:75`、`:119`、`.claude/commands/gov-change.md:18`、`.claude/agents/review-moderator.md:48` 与 `:77`、`.claude/agents/adjudicator.md:31` 与 R1 补捕的 `.claude/skills/gov-arq/SKILL.md:12` 都把后端写成主线程 packet-bound。

**证据**：上述文件于 2026-10-28 逐处 Read/Grep；P8 retro 的时间账与摩擦记录见 `delivery/review/261028-gov-review-p8-gate-semantics-cluster/retro.md:44-53`。另发现 `.claude/skills/merge-to-main/SKILL.md:12-23` 要求净树并直接 ff-only，未表达「verified commit 之后仅有证据尾」的新凭据关系。

**根因**：通道选择没有机读字段，角色规则也没有按「实现面路径」区分；因此只要开一个写权例外，就会与多份「恒 / 永不 / 一条」字面冲突。

**改法**：在 PROPOSAL 头部新增单值字段 `landing-lane: packet-bound | author-implementation`，缺省或缺失一律解释为 `packet-bound`，历史批零迁移。`/gov-change` 为作者通道唯一流程 SSOT；`arch-review-protocol.md` 只保留双轨后端指针，不复制字段算法。

新通道必须同时满足：① human 立项 ack 确认触发后的默认通道或显式选择新通道；② FROZEN PROPOSAL 写明 `author-implementation`；③ manifest 每个路径都位于 `.claude/scripts/**` 或 `.claude/hooks/**`；④ architect 在当前 gov worktree 的非 main 批准分支实现；⑤ reviewer 与 verifier 凭据齐备。任一不满足，整批走 `packet-bound`，不得把越界文件拆进新通道夹带。契约、spec、G1、settings、agent 定义、scope-map、verify-gates、混合面和 `/arch-change` 永远走旧通道。Track A 只管方向收敛，收敛后仍按同一路径判据选后端。

初始启用采用显式 opt-in。P9 同时满足以下三项后，纯 scripts/hooks 的 `/gov-change` 批自动默认 `author-implementation`，无需另开默认翻转批：① `delivery/review/INDEX.md` 的 P9 行 `状态=landed`；② 同行 `已合main=yes`；③ 主线程人读 P9 retro，确认无未闭合 BLOCK。前两项有机读锚，第三项是 convention-only 核对，不由闸解析或消费；主线程必须在每个拟默认新通道批的 PROPOSAL 头部、`landing-lane` 行旁追加 `default-trigger-evidence` 行，记录 P9 changeId、INDEX 两项实值、P9 retro 路径、人读结论与核对日期，供后续审计。该残留面按 architect-operating-model §5 的 convention-only 分层兜底明文保留，不伪装成机械门。human 在每次立项 ack 时始终可显式改选 `packet-bound`，该选择权不受默认翻转影响。首个 hooks 批即使命中默认，也仍须全强度评审并单独记录，不以 P9 的 scripts 实证替代。新通道七步变为「设计冻结 → architect 实现提交 → reviewer 真实 diff GREEN → verifier 同树 PASS → retro → 升级闸 → 合并」，旧七步原样保留。

`AGENTS.md` 的承重总声明替换为：

原文（逐字节）：
```text
**landing 通道（常态规则）**：恒 = 主线程 packet-bound 一条；architect 恒起草、architect-reviewer 恒审稿、两者永不 landing。
```

改后（逐字节）：
```text
**landing 通道（双 lane，按面选择）**：初始缺省 = 主线程 packet-bound；P9 达到 INDEX `landed + 已合main=yes` 且 retro 无未闭合 BLOCK 后，纯 scripts/hooks 的 `/gov-change` 批默认 `author-implementation`，human 立项仍可改选旧通道。该通道由 architect 在当前 gov worktree 非 main 分支实现并提交，architect-reviewer 审真实 diff，landing-verifier 对同一 commit/tree 独立验证；契约、spec、G1、settings、agent 定义、scope-map 与任何混合面仍走旧通道。首个 hooks 批仍全强度评审并单独记录；两条通道都不改 author≠judge、合并闸和 human 立项 ack。
```

`gov-arq` 的下游路由注记同步改为双 lane 指针：

原文（逐字节）：
```text
主线程派 architect 起草 PROPOSAL → human 立项 ack → architect-reviewer 评审循环 → human 裁 OQ → FROZEN → packet-bound landing → landing-verifier → retro。
```

改后（逐字节）：
```text
主线程派 architect 起草 PROPOSAL → human 立项 ack → architect-reviewer 评审循环 → human 裁 OQ → FROZEN → 按 gov-change 的 landing-lane 分流（P9 触发后纯 scripts/hooks 默认 author-implementation，human 仍可改选 packet-bound）→ landing-verifier → retro。
```

`.claude/references/gov-change.md` 的 Track B 后端替换如下。

原文（逐字节）：
```text
= 现行 Track B 七步（AGENTS.md「治理路由」节），cast 已统一：主线程派 architect 起草 PROPOSAL → human 立项 ack → architect-reviewer 评审循环至 GREEN → 主线程标 FROZEN → packet-bound landing → landing-verifier → retro → 升级闸 → 合并本地 main。 落地制品义务（landing-summary 结构两件 + 验证集实跑回填）见 `arch-change.md`「Track B 制品义务」节（单源，本文件不复制）。
```

改后（逐字节）：
```text
= Track B 七步的公共前端不变；FROZEN 后按 `landing-lane` 分流。P9 默认翻转前缺省 `packet-bound`；P9 达到 INDEX `landed + 已合main=yes` 且 retro 无未闭合 BLOCK 后，纯 scripts/hooks 批缺省 `author-implementation`，human 立项仍可改选旧通道。前两项机读，retro 项由主线程人读、属 convention-only；主线程须在新批 PROPOSAL 的 `landing-lane` 行旁落 `default-trigger-evidence`，记录 P9 changeId、INDEX 实值、retro 路径、人读结论与日期。作者通道由 architect 在当前 gov worktree 非 main 分支实现并提交，reviewer 审真实 diff，landing-verifier 验同一 commit/tree，再经 retro、升级闸与本地 main 合并。契约、spec、G1、settings、agent 定义、scope-map、verify-gates 与混合面恒走旧通道。落地制品义务见 `arch-change.md`「Track B 制品义务」节（单源，本文件不复制）；作者通道新增的 commit/tree 凭据以本文件为唯一流程 SSOT。
```

`arch-change.md` 保留架构与契约旧通道，只把共用制品义务中的绝对句改为「本面恒 packet-bound；gov 的受限作者通道见 gov-change」。`merge-to-main` 增加新通道前置：只有升级闸重算通过才能合并；verified commit 后允许的证据尾不等于可带实现改动，rebase / 回同步后必须重新审验。

目标文件：`AGENTS.md`（改）· `docs/DEV-GOVERNANCE.md`（改）· `.claude/commands/gov-change.md`（改）· `.claude/references/gov-change.md`（改）· `.claude/references/arch-change.md`（改）· `.claude/references/arch-review-protocol.md`（改）· `.claude/references/architect-operating-model.md`（改）· `.claude/agents/review-moderator.md`（改）· `.claude/agents/adjudicator.md`（改）· `.claude/skills/merge-to-main/SKILL.md`（改）· `.claude/skills/gov-arq/SKILL.md`（改）

### T2 · 最小开放 scripts/hooks 写权，并以同树非 main 检查收口

**问题**：`.claude/scope-map.json:161-192` 的 architect allow 没有 scripts/hooks，deny 明确包含两者；只删 deny 会因 allow 白名单仍不命中而继续拦截，也会遗漏分支边界。`.claude/hooks/enforce-scope.mjs:228-268` 只按路径判权，没有当前分支判定。

**证据**：scope-map 与 hook 于 2026-10-28 Read；`enforce-scope.mjs --self-check` 基线为 8 个具名结果全绿。`.claude/route-fixtures/surface-alignment.json:6-10` 仍写 architect「Draft/judge only」；`.claude/scripts/release-pr.mjs:21` 注释仍写「scripts 属 G3（agent 不可写）」，两者都需同步而非留成反事实说明。

**根因**：路径矩阵与角色说明按「所有 canonical 都只能主线程落地」建模，且 worktree 重锚只解决「哪棵树的 scope-map」，未解决「该树是否 main」。

**改法**：scope-map 的 architect allow 只追加 `.claude/scripts/**`、`.claude/hooks/**`，deny 只移除这两项。继续 deny 自身与 review-loop agent 定义、scope-map、settings、G1；contracts、firmware/server 不新增 allow。另 deny architect 写 `delivery/review/**/implementation-review.json` 与 `implementation-verification.json`；`enforce-scope.mjs` 的 architect 专用 Bash 检查同步拦截重定向写这两类凭据文件，防止作者用受控工具自造独立凭据。主线程作为既有可信编排者的写权不变；这不是密码学身份证明，verifier 是否真实独立仍由独立会话与 dispatch 保证，合并闸只重算其声明的 Git 事实。

`enforce-scope.mjs` 对 architect 写 scripts/hooks 增加专用判定：目标树必须是本会话 `CLAUDE_PROJECT_DIR` 所在树，拒绝借跨树重锚写兄弟 worktree；读取目标树 git HEAD，main/master、detached、HEAD 不可读均 fail-closed，只有非 main 分支放行。对 architect 直接 `git add` / `git commit` 增加同树非 main 检查，拒绝用 `-C` 跨树提交。同步修订 `enforce-scope.mjs` 的 Bash 分支头注，改为「universal.bashDenyWrite + architect 专用直接 git add/commit 分支检查」，不再保留「Bash 不套 per-agent」的反事实绝对句。该钩子是 PreToolUse 热路径，沿用纯文件系统读 `.git` / HEAD，不新增每次调用 spawn git。Bash 不是完整 shell 沙箱，子程序内部写文件仍属既有残余面；本批如实标注为「Write/Edit + 直接 git add/commit 机械拦截 + 合并闸兜底」，不宣称能拦所有进程内写入。

`architect.md` 同步修 Role、Boundaries、Write Scope 与 Landing 四处：只有 FROZEN manifest 内的 scripts/hooks 作者通道可实现和提交；其余 canonical 恒由主线程 packet-bound。作者可以写常驻测试、按 findings 修实现，但不得扩 manifest、改权限、合并 main 或写两份独立凭据。其机械兜底说明不再称 scripts/hooks 为保留 deny。

原文（architect.md :62-64，逐字节）：
```text
## Landing / 落地（常态规则：architect 恒不 landing）

**landing 通道恒 = 主线程 packet-bound**（G1/G2/G3 全落，landing-manifest 绑定 + landing-verifier 核验）：**architect 恒纯判官 / 起草，永不 landing**——approved-landing mode 已退役。Track A 触发时落地通道亦同（不因轨道而变）。
```

改后（逐字节）：
```text
## Landing / 落地（双 lane：默认主线程，受限作者实现）

初始默认通道 = 主线程 packet-bound（G1/G2/G3，landing-manifest 绑定 + landing-verifier 核验）。唯一例外 = `/gov-change` 的 FROZEN `author-implementation` 批，且 manifest 全部位于 `.claude/scripts/**` / `.claude/hooks/**`：P9 达到 INDEX `landed + 已合main=yes` 且 retro 无未闭合 BLOCK 后，这类批默认走作者通道；此前须显式选择，human 立项始终可改选旧通道。architect 可在当前 gov worktree 非 main 分支实现、写常驻测试并提交，但不得自批、自签 verdict、扩 manifest、写独立评审/验证凭据或合并 main。契约、spec、G1、settings、agent 定义、scope-map、verify-gates 与混合面不在例外内；Track A 收敛后也按同一路径边界选择后端。
```

路线 B 的唯一前置授权 edit 如下。临时写权集在 scripts/hooks 与凭据禁写之外，再覆盖授权注记点名的十件；五件 agent 定义虽已有 `.claude/agents/**` allow，但各自 deny 优先，故必须同步移除 deny，单纯追加 allow 压不过 deny。主线程只机械落「临时授权改后」块；architect 完成其余 25 件后，主线程必须再按「终态恢复改后」块恢复稳定 scope-map。

原文（scope-map architect 条目，逐字节）：
```json
    "architect": {
      "allow": [
        "docs/requirements.md",
        "docs/architecture.md",
        "docs/contracts/**",
        "delivery/review/*arch-review-*/**",
        "delivery/review/*gov-review-*/**",
        ".claude/agents/**",
        ".claude/commands/**",
        ".claude/references/**",
        ".claude/rules/**",
        ".claude/skills/**"
      ],
      "deny": [
        "ESP32-S3-RLCD-4.2-Demo/**",
        ".claude/agents/architect.md",
        ".claude/agents/architect-reviewer.md",
        ".claude/agents/review-moderator.md",
        ".claude/agents/scenario-author.md",
        ".claude/agents/challenger.md",
        ".claude/agents/defender.md",
        ".claude/agents/adjudicator.md",
        ".claude/agents/landing-verifier.md",
        ".claude/scope-map.json",
        ".claude/*.settings.json",
        ".claude/hooks/**",
        ".claude/scripts/**",
        ".claude/settings.json",
        "AGENTS.md",
        "CLAUDE.md",
        "docs/DEV-GOVERNANCE.md"
      ]
    },
```

临时授权改后（scope-map architect 条目，逐字节）：
```json
    "architect": {
      "allow": [
        "docs/requirements.md",
        "docs/architecture.md",
        "docs/contracts/**",
        "delivery/review/*arch-review-*/**",
        "delivery/review/*gov-review-*/**",
        ".claude/agents/**",
        ".claude/commands/**",
        ".claude/references/**",
        ".claude/rules/**",
        ".claude/skills/**",
        ".claude/scripts/**",
        ".claude/hooks/**",
        "docs/templates/**",
        ".claude/route-fixtures/**"
      ],
      "deny": [
        "ESP32-S3-RLCD-4.2-Demo/**",
        ".claude/agents/scenario-author.md",
        ".claude/agents/challenger.md",
        ".claude/agents/defender.md",
        ".claude/scope-map.json",
        ".claude/*.settings.json",
        ".claude/settings.json",
        "CLAUDE.md",
        "delivery/review/**/implementation-review.json",
        "delivery/review/**/implementation-verification.json"
      ]
    },
```

终态恢复改后（scope-map architect 条目，逐字节）：
```json
    "architect": {
      "allow": [
        "docs/requirements.md",
        "docs/architecture.md",
        "docs/contracts/**",
        "delivery/review/*arch-review-*/**",
        "delivery/review/*gov-review-*/**",
        ".claude/agents/**",
        ".claude/commands/**",
        ".claude/references/**",
        ".claude/rules/**",
        ".claude/skills/**",
        ".claude/scripts/**",
        ".claude/hooks/**"
      ],
      "deny": [
        "ESP32-S3-RLCD-4.2-Demo/**",
        ".claude/agents/architect.md",
        ".claude/agents/architect-reviewer.md",
        ".claude/agents/review-moderator.md",
        ".claude/agents/scenario-author.md",
        ".claude/agents/challenger.md",
        ".claude/agents/defender.md",
        ".claude/agents/adjudicator.md",
        ".claude/agents/landing-verifier.md",
        ".claude/scope-map.json",
        ".claude/*.settings.json",
        ".claude/settings.json",
        "AGENTS.md",
        "CLAUDE.md",
        "docs/DEV-GOVERNANCE.md",
        "delivery/review/**/implementation-review.json",
        "delivery/review/**/implementation-verification.json"
      ]
    },
```

`route-fixtures` 同步 allow 与新 rationale；`release-pr.mjs` 只修注释为「scripts 作者写权限定 gov 新通道，发布防线不变」，不改发布逻辑。

目标文件：`.claude/agents/architect.md`（改）· `.claude/scope-map.json`（改）· `.claude/route-fixtures/surface-alignment.json`（改）· `.claude/hooks/enforce-scope.mjs`（改）· `.claude/scripts/release-pr.mjs`（改）

### T3 · 模板以设计约束替代未来代码，旧 lane 的逐字与装配能力保留

**问题**：`docs/templates/PROPOSAL.template.md:46` 要求承重 landing 指令给原文与替换后完整句，`:69` 要求承重引文 byte 对账，`:87` 规定所有 §5 实跑副本一律由 assemble-edits 装配；这些要求对作者直实现通道会变成重复抄写。summary 模板没有 commit/tree 凭据位置。

**证据**：模板于 2026-10-28 Read；`landing-fidelity-check.mjs:34-37` 的 cite-check 自我声明为只读、不进闸链，`:1100-1128` 的逐字块检查也只是 advisory，但模板措辞使它们在实际流程中成为必做工艺。`assemble-edits.mjs:2-24` 的用途注释同样假定 architect 永不落 canonical。

**根因**：模板只有旧 lane 一种形态，没有按 lane 区分「冻未来文本」与「冻设计 + 后验真实 diff」。

**改法**：PROPOSAL 模板增加 `landing-lane` 头部字段和选择判据；P9 默认翻转后的批次另附 `default-trigger-evidence` 行，字段承载 P9 changeId、INDEX 两项实值、retro 路径、convention-only 人读结论与核对日期。旧 lane 保持五段式、承重原文/改后块、preflight、cite-check 与 assemble-edits；新 lane 仍写 T 条目、manifest、行为约束、正反测试与明确不做事项，但不要求逐字未来代码，不用 make-edits/build-proposal，不生成证明主线程抄录一致的装配副本。新 lane 的验证数字来自真实实现 commit 上的命令，不从「设想的未来文件」代跑。

装配义务改为只约束 `packet-bound`：

原文（PROPOSAL.template.md :87 的装配句，逐字节）：
```text
**装配**：§5 实跑副本一律用共享锚文装配器 `node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>` 装配（261027 批 T1，ARQ-261027）——旧串锚定、逐条断言恰命中 1 次（0 次 / 多次即中止，全部命中后才写盘）、行尾原样不归一、输出根强制在仓外；清单 = §2 编辑的 JSON 形态（file / oldStr / newStr / label），不再每批现写一次性脚手架。
```

改后（逐字节）：
```text
**装配（仅 `packet-bound` lane）**：§5 实跑副本一律用共享锚文装配器 `node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>` 装配（261027 批 T1，ARQ-261027）——旧串锚定、逐条断言恰命中 1 次（0 次 / 多次即中止，全部命中后才写盘）、行尾原样不归一、输出根强制在仓外；清单 = §2 编辑的 JSON 形态（file / oldStr / newStr / label），旧 lane 不再每批现写一次性脚手架。`author-implementation` lane 不装配未来代码，验证命令直接在已提交且待审的实现 tree 上实跑。
```

summary 模板保留 packet-id、双侧 manifest、落地点对应与验证证据；新 lane 追加 `implementation-review.json` 与 `implementation-verification.json` 指针、reviewedCommit/tree 与 verifiedCommit/tree 的人读复述。权威顺序写清：review JSON 是评审钉点，verification JSON 是验证钉点，summary 只是汇总，不能拿 summary 自报替代两份独立凭据。旧 lane 不需要这两件，原模板路径继续可用。

`assemble-edits.mjs` 仅修订头注与仓内输出拒绝消息，把「architect 不落 canonical」收窄为「packet-bound 起草副本不落 canonical」；算法不变。cite-check 与逐字 advisory 在新 lane 标 `NOT_APPLICABLE`，旧 lane 原样执行，避免双头 SSOT。

目标文件：`docs/templates/PROPOSAL.template.md`（改）· `docs/templates/architect-landing-summary.template.md`（改）· `.claude/scripts/assemble-edits.mjs`（改）

### T4 · reviewer 审真实 diff，verifier 独立做同树与消费层验证

**问题**：现行 reviewer 只审 PROPOSAL，landing-verifier 以主线程未提交 landing diff 为对象；两者都没有 author-implementation 所需的机读钉点和独立失败判据义务。

**证据**：`.claude/agents/architect-reviewer.md:21`、`:43-49` 与 `.claude/references/track-b-judge-brief.md:19-37` 限定审 PROPOSAL；`.claude/agents/landing-verifier.md:12-34` 与 verifier brief :19-29 限定「主线程落地后逐文件 diff」。P8 `ARQ-281028-mutation-fixture-consumer-layer` 实测证明，仅在计算层做 mutation 会出现自检全绿而消费层带洞。

**根因**：旧模型的独立性依赖「作者写设计、主线程写实现」；新模型必须把独立性迁移到「实现作者不写评审与验证凭据，评审人与验证人各自从设计和 Git 地面真值重推」。

**改法**：reviewer 在新 lane 中先读 FROZEN 行为约束，独立列出失败判据，再审 `main...reviewedCommit` 的真实实现 diff、常驻测试和文件范围；至少一条判据针对真实 CLI / hook 消费或拦截层，不能只复述作者测试名。GREEN 时写 `implementation-review.json`，字段为 `schemaVersion=1`、`lane`、`changeId`、`reviewedCommit`、`reviewedTree`、`changedFiles`；§7 仍写行首 `verdict: GREEN`。后续实现变化使旧钉点失效。

verifier 不接受作者或 reviewer 的测试结论作唯一证据。它在同一 reviewedCommit 上重算 tree 与 changedFiles，复跑结构门和常驻测试，并独立执行至少一个消费层/黑盒探测；PASS 时写 `implementation-verification.json`，字段为 `schemaVersion=1`、`lane`、`changeId`、`reviewedCommit`、`reviewedTree`、`verifiedCommit`、`verifiedTree`、`changedFiles`，并在 landing-verdict 写行首 PASS。要求 `verifiedCommit == reviewedCommit`、`verifiedTree == reviewedTree`，不一致即 FAIL。

测试最低义务：有长期保护价值的 case 进入被改脚本/ hook 的 `--self-check` 或 manifest 内正式测试文件，批后不得删除；计算层与消费/拦截层分离时，两层各至少一个正反思辨 case，消费层必须证明「反转实现会变红」。一次性环境夹具可放仓外，但不能是唯一判据。该改法吸收 `ARQ-281028-mutation-fixture-consumer-layer` 的建议 1 与 3，并给建议 2 留下 P9 实证义务；本批不提前把条目标 closed。

旧 lane 的 reviewer / verifier 职责与制品不变，仅新增模式分支，不把真实 diff 评审义务反向套到所有历史批。

目标文件：`.claude/agents/architect-reviewer.md`（改）· `.claude/agents/landing-verifier.md`（改）· `.claude/references/track-b-judge-brief.md`（改）· `.claude/references/track-b-verifier-brief.md`（改）

### T5 · landing-fidelity 按 lane 分流，结构门直接核 Git 钉点

**问题**：`landing-fidelity-check.mjs:920-939` 以「工作树 ∪ HEAD~1」为已落地集合，`:996-1020` 只判 summary ⊆ proposal 和文件是否有改动；它既不能表示多个实现 commit，也不能证明 reviewedCommit 是被审的那一版。

**证据**：2026-10-28 Read 上述实现；现行 `--self-check` 全绿，依次为 changeid-prefix 4、extractTargets 6、section2Anchors 29、resolveOrigin 29、computeFrozenPin 4、citation-anchors 19、landing-preflight 20、cite-check 21、lint-descriptions-trigger 2、verbatim-quotes 13、dir-anchor-expansion 7、isFrozen 19，fail 均为 0。

**根因**：旧结构门只需要回答「主线程是否把 manifest 中的文件都改了」，没有作者提交后「哪个 commit 被审」的状态。

**改法**：共享 lib 增加 lane 与 JSON 凭据的 fail-loud 解析，供 landing 与 pre-merge 两方消费，禁止各写一套正则。`landing-lane` 缺失 = 旧 lane；未知值、重复冲突、JSON 缺字段或 SHA 非 40 位均 RED。

旧 lane 保留现有全部检查与默认 baseRef 行为。新 lane 改查：① FROZEN、packet-binding、双侧 manifest 与 §2 锚并集仍一致；② manifest 仅 scripts/hooks；③ `implementation-review.json` 的 changeId/lane 正确；④ `git rev-parse <reviewedCommit>^{tree}` 等于 reviewedTree；⑤ 从 `main...reviewedCommit` 重算的非证据改动文件恰等于 manifest，review JSON 的 changedFiles 只作交叉对账；⑥ 工作树没有未提交的 scripts/hooks 实现漂移。summary 中复述的钉点必须与 review JSON 一致，但不作为独立事实源。

证据路径只允许 `delivery/review/<changeId>/**`、`delivery/review/INDEX.md` 与同生同写的 `delivery/review/.changes-index-state.json`；这不是实现白名单，只是评审、verdict、retro 和索引生成件的尾部提交空间。新 lane 跳过 §2 未来文本的 verbatim advisory，保留引据锚点 advisory；旧 lane 的 cite-check / assemble-edits 行为不变。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）· `.claude/scripts/lib/governance-gate-shared.mjs`（改）

### T6 · pre-merge 闸重算同一实现链，拒绝旧 SHA、作者自报与证据尾夹带

**问题**：`pre-merge-escalation-gate.mjs:104-120` 的 gov 锚只有 FROZEN、GREEN、landing-verdict 是否存在，`:615-621` 也只消费这三项；没有验证 verdict 终态对应哪棵实现 tree，也没有防止验证后再改实现。

**证据**：2026-10-28 Read；`pre-merge-escalation-gate.mjs --self-check` 基线为 88 pass / 0 fail。当前闸的 off-main、契约凭据、废止路径检查保持不动。

**根因**：旧 lane 的落地与验证相邻，凭据模型不需要表达实现 commit；新 lane 有独立提交与后续证据提交，必须把「被审」「被验」「待合并」连成可由 Git 重算的链。

**改法**：新 lane 除 FROZEN + 最终 GREEN + 最终 PASS 外，必须同时有 review / verification 两份 JSON。闸在合并时独立执行 Git 重算，不信任 summary 或作者报告：

1. 两份 JSON 的 lane/changeId 正确，`verifiedCommit == reviewedCommit`，`verifiedTree == reviewedTree == git rev-parse <reviewedCommit>^{tree}`；SHA 不存在或 Git 不可用 fail-closed。
2. reviewedCommit 必须是待合并 source HEAD 的祖先；rebase 后旧 commit 不再是祖先即 BLOCK。
3. `main...reviewedCommit` 与 `main...source` 去除上述两类证据路径后，改动文件集都必须恰等于 manifest；越 manifest、少实现或借早先 commit 藏文件均 BLOCK。
4. `reviewedCommit..source` 只允许本批 review 目录、`delivery/review/INDEX.md` 与 `delivery/review/.changes-index-state.json`。证据尾中的 PROPOSAL、summary、两份 JSON、verdict、retro、INDEX 与状态缓存不需反复自证；任何 scripts/hooks、其它 canonical、debt 或 workstream 尾随改动都要求重新 reviewer GREEN + verifier PASS。回同步 merge 带来非证据尾，同样 BLOCK。
5. source HEAD 仍过现有 off-main 检查；gov 新 lane 不开放 `--human-approval-ref` 豁免，不削弱契约、设备、冲突升级等既有判据。

这一模型故意不要求 source HEAD 的整仓 tree 等于 verifiedTree，否则 verdict 自身提交会使判据永不可满足；等价安全性质由「同一 verified commit + 其后只能为封闭证据路径 + 全合并 diff 仍等于 manifest」给出。任何非证据 tree 变化都必须重新审验，旧 SHA 不能放行。

机器能防的是旧/错 SHA、越界 diff 与作者直接写两份 JSON；机器不能从一份 JSON 证明独立 verifier 真跑过测试，后者由 typed agent、独立会话、只写凭据窄槽和 verdict 人读命令证据共同保证，诚实保留此边界。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

## 3. 影响面 sweep 分类（UNCLASSIFIED=0）

### 3.1 本批落地文件

| 面 | 文件 | 类别 | 消费方与动作 |
|---|---|---|---|
| G1 | `AGENTS.md` | 改 | 七步与双 lane 总路由 |
| G1 | `docs/DEV-GOVERNANCE.md` | 改 | 角色表、治理流程、§6 通道 |
| G2 | `.claude/commands/gov-change.md` | 改 | 主线程选择后端 |
| G2 | `.claude/references/gov-change.md` | 改 | 新 lane 唯一流程 SSOT |
| G2 | `.claude/references/arch-change.md` | 改 | 共用制品义务与旧面边界 |
| G2 | `.claude/references/arch-review-protocol.md` | 改 | Track B 后端与 Track A 回流指针 |
| G2 | `.claude/references/architect-operating-model.md` | 改 | architect 职责与 landing 例外 |
| G2 | `.claude/agents/architect.md` | 改 | 受限实现与禁止项 |
| G2 | `.claude/agents/architect-reviewer.md` | 改 | 真实 diff 与独立风险判据 |
| G2 | `.claude/agents/landing-verifier.md` | 改 | 同树、黑盒与凭据产出 |
| G2 | `.claude/agents/review-moderator.md` | 改 | Track A 后不假定唯一旧通道 |
| G2 | `.claude/agents/adjudicator.md` | 改 | FIX-GOVERNANCE 后端指针 |
| G2 | `.claude/references/track-b-judge-brief.md` | 改 | 新评审派遣形状与消费层层别 |
| G2 | `.claude/references/track-b-verifier-brief.md` | 改 | 新核验派遣形状 |
| G2 | `.claude/skills/merge-to-main/SKILL.md` | 改 | 证据尾与复验后合并 |
| G2 | `.claude/skills/gov-arq/SKILL.md` | 改 | ARQ 下游路由注记同步双 lane |
| G2 | `docs/templates/PROPOSAL.template.md` | 改 | 双 lane 模板与消费层测试义务 |
| G2 | `docs/templates/architect-landing-summary.template.md` | 改 | 新凭据指针，旧字段保留 |
| G3 | `.claude/scope-map.json` | 改 | scripts/hooks 最小 allow 与凭据 deny |
| G3 | `.claude/hooks/enforce-scope.mjs` | 改 | 同树非 main 与直接提交拦截 |
| G3 | `.claude/scripts/landing-fidelity-check.mjs` | 改 | 新结构门，旧门回归 |
| G3 | `.claude/scripts/pre-merge-escalation-gate.mjs` | 改 | 同树合并凭据重算 |
| G3 | `.claude/scripts/lib/governance-gate-shared.mjs` | 改 | lane/JSON 解析单源 |
| G3 | `.claude/scripts/assemble-edits.mjs` | 改 | 仅注释与旧 lane 适用范围 |
| G3 | `.claude/scripts/release-pr.mjs` | 改 | 仅反事实注释修正 |
| 邻接登记 | `.claude/route-fixtures/surface-alignment.json` | 改 | architect writeScope 对齐 |

脚本净增预计超过 150 行且含新凭据判定，按一般拆分纪律本应单独立脚本批；但 human 已批准的 enabling 设计要求权限、模板、闸与角色原子生效，拆开会形成「有写权无同树闸」或「有闸无通道」的半态，故本批作为一次原子 enabling 改造交 reviewer 判档，不把此例推广为以后绕开拆分纪律的许可。

### 3.2 引用普查后的保留与历史面

| 文件 / 面 | 处置 | 理由 |
|---|---|---|
| `.claude/rules/contracts.md` | 保留不改 | 契约恒旧 lane，现有「落地恒主线程」在该面仍正确 |
| `.claude/commands/arch-change.md` | 保留不改 | 只描述架构+契约面，其主路径仍 packet-bound |
| `CLAUDE.md` | 保留不改 | 经 `@AGENTS.md` 单源传导，无独立 landing 规则 |
| `.claude/agents/planner.md` | 保留不改 | 「只 advisory 不落盘」只限 dev-loop advisory，不属新 lane |
| `docs/requirements.md` / `docs/architecture.md` | 保留不改 | spec 不进新 lane |
| `docs/notes/**` 中的 show-me / archify HTML | 历史说明不改 | 记录当时制度，非现行 canonical |
| `delivery/**` 历史 PROPOSAL / verdict / retro | 历史证据不改 | 不回写旧批事实 |
| `docs/debt/gov/_archive/**` | 历史账不改 | 只作根因与处方复证 |
| `.claude/scripts/assemble-edits.mjs` 的装配算法 | 保留 | 旧 lane 继续消费 |
| `landing-fidelity-check --cite-check` | 保留为旧 lane | 不是全局删除，只在新 lane 标不适用 |
| `.claude/route-fixtures/surface-alignment.json` | 必须改 | 虽 cross-surface 脚本未迁，登记内容仍不能反事实 |

复扫模式完备性：以 `packet-bound`、`恒不 landing`、`永不 landing`、`architect 恒`、`assemble-edits`、`cite-check`、`agent 不可写` 七组字面在 `AGENTS.md`、`docs/`、`.claude/` 全仓扫描，再对长行省略处定点 Read；「逐字」另行扫描后只收与 landing/装配相关命中，spec 镜像、验收 needle、历史引文等语义无关项不机械改。覆盖 canonical、模板、agent、command、reference、skill、脚本注释和 route fixture；docs/notes 与 delivery 只分类为历史，不做现行声明。

## 4. human 取舍点（OQ）

### OQ-1 · verified commit 之后的证据尾免复验边界【human 2026-10-28 裁 (a)】

**背景**：verdict 和 verification JSON 必须在验证后产生；若要求最终 source HEAD 整仓 tree 仍等于 verifiedTree，则提交 verdict 本身就会使条件永假。但「任何后续变化都免审」又会给验证后夹带实现留下口子。

**裁定**：采用 (a) 窄例外。只允许 `delivery/review/<changeId>/**` 与 `delivery/review/INDEX.md` 作为证据尾；其它任何路径变更都强制重新 reviewer GREEN + verifier PASS。debt 等其它收尾走既有独立通道，不扩进证据尾。

第三件生成件：`delivery/review/.changes-index-state.json`——**2026-10-28 human 显式确认并入（主线程转呈，R1-F3 增补）**。

**裁定追注（2026-10-28，AskUserQuestion，主线程转呈）**：human 确认将第三件 `.changes-index-state.json` 正式并入证据尾白名单，白名单共三件；前两件属原裁范围，第三件是 R1-F3 的后续增补，不回写成原裁内容。

### OQ-2 · P9 成功后纯 scripts/hooks 批的默认通道【human 2026-10-28 裁：P9 成功即转默认】

**背景**：human 已定 P9 是首个 scripts 试点、hooks 首用另行实证。本拍进一步裁定默认翻转时点，推翻起草者「继续 opt-in」的推荐。

**裁定**：P9 成功即转默认，不另开默认翻转批。触发条件 = `delivery/review/INDEX.md` 的 P9 行同时满足 `状态=landed`、`已合main=yes`，且主线程人读 P9 retro 确认无未闭合 BLOCK。前两项机械可读；retro 项按 architect-operating-model §5 先例明确为 convention-only 残余面，不虚构机读判据。三项齐备后，所有纯 scripts/hooks 的 `/gov-change` 批默认 `author-implementation`；主线程在新批 PROPOSAL 头部 `landing-lane` 行旁落一行 `default-trigger-evidence`，逐记 P9 changeId、INDEX 实值、retro 路径、人读无未闭合 BLOCK 的结论和核对日期。缺任一项则仍须显式 opt-in。

human 在每次立项 ack 时始终保留显式选 `packet-bound` 的权利，默认翻转不取消旧通道。首个 hooks 批仍须全强度评审并单独记录实际加载形态、自动执行副作用和独立复现结果，不能借 P9 的 scripts 成功宣称 hooks 已实证。

## 5. 验证集（landing 前机械前置 + landing 后核验）

### 5.1 起草期已跑基线（2026-10-28）

| 编号 | 命令 | 实测关键输出 |
|---|---|---|
| B1 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | 12 组自检均 `ok:true`；计数依次 4/6/29/29/4/19/20/21/2/13/7/19，fail 全 0 |
| B2 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | `pass:88, fail:0, total:88, ok:true` |
| B3 | `node .claude/hooks/enforce-scope.mjs --self-check` | `ok:true`，8 个具名结果全 true |
| B4 | `node .claude/scripts/assemble-edits.mjs --self-check` | `pass:16, fail:0, total:16, ok:true` |
| B5 | `node .claude/scripts/route-regression-check.mjs --self-check` | `104 ok, 0 fail` |

本稿 §2 的 6 组承重替换已生成 `_scratch/edits.json`（共 7 条 edit，scope-map 为「现盘 → 临时授权 → 终态恢复」两条链式 edit），并于 2026-10-28 以 `node .claude/scripts/assemble-edits.mjs <本批_edits.json> C:/tmp/author-lane-assembly-draft` 实装成功：`ok:true`，输出 6 件（AGENTS.md、gov-change.md、architect.md、PROPOSAL.template.md、gov-arq SKILL.md、scope-map.json），输出根在仓外；scope-map 最终恢复块 afterLines=306。首次以裸数组调用时被装配器以「清单缺 edits 与 copy」拒绝，改为脚本声明的 `{edits:[...]}` 形状后通过；未产生半装配副本。

最终自证回执（2026-10-28，v0.6 临时十件写权与终态恢复修订后复跑）：preflight = `ok:true`、`anchorCount:26`、`findings:[]`、`advisories:[]`，anchorUnion 与头部 26 行 manifest 逐件一致；cite-check draft = `ok:true`、`targetCount:26`、`checkedCount:6`、`missing:[]`、`unverified:[]`，7 个改后块（含 scope-map 临时与终态两块）按相位列入 notChecked。仓外复装为 `ok:true`、6 件共 7 个具名 edit，scope-map 两条链式 edit 均命中且终态 afterLines=306。全稿 549 行，edits.json 46 行。

### 5.2 落地后新机制验证（全部在仓外临时 Git 仓夹具执行）

| 编号 | 验证 | 预期 |
|---|---|---|
| V1 | 两个改动脚本与 hook 的 `--self-check`，先断言新增夹具非空、manifest 非空、SHA 为 40 位 | 新增自检全部具名出现且 fail=0；调用顺序不被终端式自检截断 |
| V2 | 新 lane happy path：真实 temp repo 中实现 commit → reviewer JSON → 同 commit verifier JSON → 证据尾 → 直调 landing-fidelity 与 pre-merge CLI | 两门 exit 0；不是只调内部 assess / parser 函数 |
| V3 | 旧 lane happy path：无 `landing-lane` 的 FROZEN PROPOSAL + 原 summary / verdict 形状 | 原结构门与 gov 闸行为保持，历史缺省仍为 packet-bound |
| V4 | 旧 SHA：PASS 后追加一个 scripts 实现 commit，但保留旧 review / verification JSON | pre-merge exit 1，点名 source 含非证据尾或 reviewedCommit 不是最新实现链；不得放行 |
| V5 | rebase：同一实现补丁重写为新 SHA，沿用旧 reviewedCommit | pre-merge exit 1，旧 commit 非 source 祖先或 tree 钉点失配 |
| V6 | 回同步：PASS 后 merge 含 main 侧非证据改动的 commit | pre-merge exit 1；重新 reviewer + verifier 后新钉点才可通过 |
| V7 | 越 manifest：实现 diff 多一个 `.claude/scripts/extra.mjs`，再做一个少 manifest 成员的反向夹具 | 两者均 exit 1；正向 V2 证明不是「凡新 lane 都红」 |
| V8 | 证据尾边界：只加 PROPOSAL/verdict/retro/INDEX/.changes-index-state.json 与加 debt、workstream、hooks 各一组 | 前者可过，后三者逐一 BLOCK |
| V9 | 作者自报与错 tree：summary 写正确 SHA 但 verification JSON 写错 tree；architect 尝试 Write/Edit 两份凭据 JSON | 闸重算后 BLOCK；hook exit 2；summary 不能补救 |
| V10 | main / 跨树拦截：构造 main、gov 非 main、detached、兄弟 worktree 四树，architect 写 scripts 与 hooks；另试直接 `git add` / `git commit` 和 `-C` | 仅当前 gov 非 main 树放行，其余 exit 2；architect 写 contracts、builder 写 scripts 的既有拒绝均不回归 |
| V11 | 消费层 mutation：分别反转「证据尾检查」与「tree 相等检查」的计算函数，再直调 pre-merge CLI | 每个反转都使对应消费层 case 从绿变红；计算层自检也各至少一例变红 |
| V12 | 常驻性：检查新增测试位于 manifest 内 `--self-check` 或正式测试文件 | 无依赖 `_scratch` 才能通过的常驻防线；一次性 temp repo 仅作夹具 |
| V13 | 全量回归：B1–B5 原命令 + `settings-lint` + `lint-descriptions` | 原自检不回退；route regression 仍 104 或新增断言后全绿，lint 无新增阻断 |
| V14 | 敏感面逐文件人读 | landing-verifier 对 G1、scope-map、architect/reviewer/verifier 定义、hook、两闸逐字核 diff；机器绿不替代 |
| V15 | 默认翻转证据：分别构造 INDEX 两项齐全/缺项与 retro 人读结论，并检查后续批 PROPOSAL 头部 | 仅三项齐全可默认；`default-trigger-evidence` 含 P9 id、INDEX 实值、retro 路径、结论与日期；retro 项明确标 convention-only，缺记录不得宣称默认已触发 |

V2、V4–V11 是消费/拦截层义务，不得用仅调用纯函数的测试替代。无法自动证明的「独立 verifier 真独立思考」由 V14 与 verdict 中独立命令留痕人工核验，不伪称机械已证。

### 5.3 实现与验证序列（human 路线 B · 本批一次性自用）

1. **唯一前置授权 edit**：主线程先按 T2 第 6 组「临时授权改后」块机械落 `.claude/scope-map.json`，只落该 architect 条目的临时授权，不代写其余 25 件。临时面 = scripts/hooks + 授权注记逐一点名的十件；五件 agent 定义的 deny 必须同步移除。落地后立即核对实际 diff 与该块逐字一致。
2. **作者实现**：architect 在当前 gov worktree 的 `gov/kidpal` 非 main 分支实现其余 25 件——既有 5 组逐字块照规格落地，其余文件按行为规格实现，不补未来代码逐字块；运行可跑检查并提交实现 commit，不静默扩 manifest。
3. **批终态恢复（强制前置）**：作者实现完成后、真实 diff 评审前，主线程按第 6 组「终态恢复改后」块恢复 scope-map 稳定态，收回十件临时写权，仅保留常态 scripts/hooks 与凭据禁写。恢复结果必须逐字等于该 after 块；未恢复不得派 reviewer / verifier。
4. **真实 diff 评审**：architect-reviewer 审 `main...HEAD` 的真实 diff、常驻测试与文件范围，专项核对前置授权 commit、终态恢复 commit 与最终 scope-map：临时授权只含获批十件，终态无临时权限残留、无额外权限夹带；GREEN 后钉 reviewedCommit/tree。
5. **独立验证**：landing-verifier 对同一恢复后 commit/tree 独立复验，钉 verifiedTree、核对 `changedFiles == manifest`，逐字确认 scope-map 终态等于「终态恢复改后」块，并执行消费层/黑盒可跑检查；作者测试不是唯一判据。
6. **后置夹具与旧闸合并**：V2、V4–V11、V15 标为**实现落地后、合并前**在仓外临时 Git 仓执行。全部通过且 scope-map 终态核验通过后，本批才过现行旧闸的 FROZEN + GREEN + verdict 锚合并；新 tree 重算闸由本批实现，不能用尚未落地的闸证明自己，路线 B 的「闸后置」仅此一次。
7. **窗口诚实注记**：临时授权落地到 enforce-scope 新代码落地之间，非 main 拦截尚未机械化；该短窗口靠本批 `gov/kidpal` 纪律、主线程前置 diff 核对、reviewer 专项审与极短窗口兜底，不宣称此时已有新 hook 防线。

### 5.4 实际实施序列（human 最新指令，覆盖 §5.3）

R5 后 human 明确指示「主线程直接改，然后 architect review、landing verifier 正常验证」。实际执行据此改为：① 主线程在 gov worktree 直接实现全部 manifest 文件，scope-map 直接落稳定终态，不创建临时写权、不执行三态恢复；② 核心自检与真实 Git 仓夹具先行；③ architect-reviewer 对真实实现 diff 独立评审，PROPOSAL §7 追加最终 verdict；④ landing-verifier 按现行 packet-bound 保真度流程独立验证。§5.3 保留为路线 B 的历史方案记录，不再作为本批执行指令。

## 6. 留根清单

1. **P9 首个 scripts 试点**：硬成功指标 = 主线程实现代写 0 次、逐字未来代码块 0 个、make-edits/build-proposal 类脚手架 0 个、仓外装配副本 0 个；reviewedTree==verifiedTree，旧 SHA / 越 manifest / 非证据尾负例全部能拦，verifier 至少独立发现或复证一个消费层风险。PROPOSAL 行数、临时脚本数、装配次数、评审轮次与验证发现只作实测对照，不设诱导缩文的数字配额。只有 P9 达到 INDEX `landed + 已合main=yes` 且主线程人读本 retro 确认无未闭合 BLOCK，才触发后续纯 scripts/hooks 批默认新通道；retro 项为 convention-only，触发证据须落在后续批 PROPOSAL 的 `default-trigger-evidence` 行，未齐三项不自动翻转。若仍须主线程代写或逐字块才能过闸，或旧 SHA 能放行，立即判设计未闭合；回退 = 新批恢复 scope-map deny 并让新 lane fail-closed，旧 packet-bound 继续服务，不做 main reset。
2. **首个 hooks 新通道实证**：不借 P9 宣称。优先选已注册 hook；若需新增 settings 注册即属混合面，不能走作者通道。验证在当前 gov worktree 以新 reviewer/verifier 会话和直接子进程黑盒两路进行，不假定新 worktree，也不假定当前会话会热重载；记录实际加载形态、会话标识、自动执行副作用与前后 `git status`。PostToolUse 类 hook 对 verdict 写入的副作用必须纳入夹具。首个 hooks 批未独立完成这些记录前，不得以 P9 默认翻转为由免除全强度评审与单独记录；human 立项仍可按既有权利改选旧 lane。
3. **ARQ-281028-mutation-fixture-consumer-layer**：本批把建议 1/3 落进模板与简报，把建议 2 变成 V11 与 P9 必做实证；条目暂留 deferred，P9 证明消费层判别长期在位后由主线程结项，避免「文面一成即宣布根因消失」。不另立同根 ARQ。
4. **ARQ-281028-assembler-import-closure-and-root-param**：本批不消化。assemble-edits 继续服务旧 lane，其 import 闭包与根参数问题仍是有效便利改进；本批只修适用范围注释，不把新 lane 的「不装配」误当旧 lane 问题已解决。
5. **机械边界残余**：enforce-scope 不是完整进程沙箱，不能识别任意子程序内部写 main；直接 Write/Edit、直接 git add/commit、跨树重锚与合并 diff 是本批可机械覆盖的范围。残余靠角色禁令与合并闸，后续若实测绕过再登记，不预先声称已封死。
6. **一次性自举边界**：本批即 human 2026-10-28 显式授权的一次性自举，范围 = 主线程先落第 6 组临时授权 edit、architect 实现其余 25 件、主线程恢复 scope-map 稳定态后再审验；后续任何批不得自行 bootstrap，除非其自身立项获得 human 同等显式授权。本批不构成后续批先例。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-28 - architect-reviewer

**四查结论**：意图基线七点裁定（scripts+hooks 双覆盖 / gov worktree 不新建 / 冻设计不冻代码 / 作者测试非唯一判据 / changedFiles==manifest + reviewedTree==verifiedTree 绑定 / enabling 批不自举 / OQ-1 窄证据尾 / OQ-2 P9 成功即转默认）逐条落成机制，无偷换——OQ-1 证据尾未被扩大（T5/T6 两处均钉死 `delivery/review/<changeId>/**` + `delivery/review/INDEX.md`）、OQ-2 写的是「默认」且保留 human 改选权、非「强制」。判档：**维持 Track B 主路径、riskTier medium**——触 commands/agents/hooks/scripts/scope-map/合并闸，轻路径判据（gov-change.md「轻路径判据」节③）命中恒 Track B；方向已由 human 批准的设计记录收敛、OQ 两拍已裁，Track A 段判据（只看收敛度）不命中，不升级。§3.1 脚本拆分纪律例外申报诚实（原子性理由成立、且明确不推广），接受。行级引证抽查 30+ 处（AGENTS.md:19/:33、DEV-GOVERNANCE.md:147/:210-214、arch-review-protocol.md:48-56/:68、arch-change.md:33、architect-operating-model.md:56/:75/:119、commands/gov-change.md:18、review-moderator.md:48/:77、adjudicator.md:31、merge-to-main SKILL.md:12-23、architect.md:62-64 逐字节、scope-map.json:161-192、enforce-scope.mjs:228-268、surface-alignment.json:6-10、release-pr.mjs:21、PROPOSAL.template.md:46/:69/:87 逐字节、landing-fidelity-check.mjs:34-37/:920-939/:996-1020/:1100-1128、assemble-edits.mjs:2-24、architect-reviewer.md:21/:43-49、landing-verifier.md:12-34、track-b-verifier-brief.md:19-29、pre-merge-escalation-gate.mjs:104-120/:615-621、changes-index.mjs:41、gov-arq SKILL.md:12、P8 retro.md:44-53）全部实测吻合；B1–B5 五项自检基线 2026-10-28 本机复跑计数逐一吻合（4/6/29/29/4/19/20/21/2/13/7/19、88、ok、16、104）。manifest 25 件 == §3.1 表 25 行 == preflight anchorCount:25 自洽。ARQ 处置如实：mutation-fixture-consumer-layer 吸收建议 1/3（T4 + 简报 + V11 层别义务）、建议 2 留 P9 实证、条目不提前结项；assembler-import-closure 不消化、注明旧通道仍需其处方便利——与两条目 deferred 状态一致，无双头。

**findings**：

- **F1（BLOCKING）· 引用普查漏 `.claude/skills/gov-arq/SKILL.md`，§3.2「复扫模式完备性」断言不实**。实测 Grep `packet-bound|永不 landing|恒不 landing|architect 恒` 于 AGENTS.md + docs/ + .claude/：`.claude/skills/gov-arq/SKILL.md:12` 含「→ FROZEN → packet-bound landing → landing-verifier → retro」路由注记，该文件既不在 manifest 25 件内、也不在 §3.2 保留表。默认翻转后此注记对纯 scripts/hooks 批成为反事实投影——与本批自己定的标准冲突（T2 对 route-fixtures 与 release-pr.mjs「两者都需同步而非留成反事实说明」）与 DEV-GOVERNANCE.md:148「改源时同批同步本文件内的全部投影处」硬条款冲突。修法方向：入 manifest 同步（推荐，一句话加 lane 指针）或入 §3.2 保留表给理由；§3.2 复扫声明须相应改写。
- **F2（SHOULD）· OQ-2 三触发条件两真一虚，「P9 retro 无未闭合 BLOCK」无机读锚**。`delivery/review/INDEX.md` 列名实测为 `状态 | 已合main`，前两项机读可得（landed 由 landing-verdict verdict 行现推）；retro 为散文制品、无机器解析的 BLOCK 字段，闸不消费。现文「主线程立项时读取该行与 retro 并记录触发依据」已诚实承认人读，但触发依据的**记录落点未指定**，事后不可审计。修法方向：钉死记录落点（如新批 PROPOSAL 头部 `landing-lane` 行旁附一行 P9 三项核对结果），并按 architect-operating-model.md §5「convention-only 分层兜底」先例明文标注该默认翻转核对为 convention-only 残余面。
- **F3（SHOULD）· 证据尾 allowlist 漏 `delivery/review/.changes-index-state.json`**。实测 `changes-index.mjs:41`：`STATE_CACHE_PATH = resolve(projectDir, 'delivery', 'review', '.changes-index-state.json')`——与 INDEX 同生同写的生成件，既不匹配 `delivery/review/<changeId>/**` 也不匹配 `delivery/review/INDEX.md`。gated 窗口（verifier PASS → 合并）内任何 `--upsert` 重跑或缓存再生都会撞 T6-4 的 BLOCK（fail-closed 方向安全、但为生成件缓存强制全量复验属误伤）。修法方向：把它与 INDEX 并列为证据尾同类生成件，或明文规定 gated 窗口内禁跑 `--upsert` 并接受 fail-closed 代价。
- **F4（NIT）· `enforce-scope.mjs:204-207` 既有注释「Bash 分支只守 universal.bashDenyWrite、不套 per-agent」在 T2 为 architect 加专用 git add/commit 检查后成反事实**——T2 对 release-pr.mjs 注释用了「同步而非留成反事实」标准，同一标准应及于本文件注释。
- **F5（NIT）· T3 装配句改后文本删去「（261027 批 T1，ARQ-261027）」出处括注与句尾「不再每批现写一次性脚手架」**——canonical 出处锚丢失，建议改后句保留出处括注。
- **F6（NIT）· V10 措辞「contracts 与 builder 写 scripts 的既有拒绝」歧义（contracts 非 agent）**，疑为「写 contracts 与 builder 写 scripts」，建议改写消歧。

**亲核留痕**：五项自检复跑（landing-fidelity / pre-merge 88 / enforce-scope / assemble-edits 16 / route-regression 104）全绿且计数与 §5.1 逐一吻合；sweep 独立 Grep 两轮（命中 20 + 9 文件逐一定性）；changes-index state 路径、enforce-scope Bash 分支、INDEX 列名均为本机实测；manifest ↔ §3.1 ↔ preflight anchorCount 三方对账 25=25=25。

verdict: BLOCK(F1)

### R2 - 2026-10-28 - architect-reviewer

**轮次纪律**：聚焦 R1 六条 findings 修订到位性 + 修订引入的新矛盾扫描，未做全量重核（B1–B5 基线所涉脚本本稿未再改动，R1 复跑结论沿用）。

**F1–F6 修订到位性逐条复核（全部亲核实测，不采信回执）**：

- **F1 到位**。manifest 25→26（`.claude/skills/gov-arq/SKILL.md` 入列，头部第 23 行）；§3.1 表增至 26 行（含 gov-arq 行）；T1 问题句补「R1 补捕的 `.claude/skills/gov-arq/SKILL.md:12`」、目标文件列补第 11 件；新增第 5 组逐字块（:83-91）。本机复跑 `--preflight`：`ok:true`、`anchorCount:26`、`findings:[]`，anchorUnion 与 26 行 manifest 逐件一致。原文块逐字性由两路独立机检证实：`--cite-check --phase draft` exit 0（`checkedCount:5`，gov-arq 块 line 84 `matched`）；并以 `_scratch/edits.json`（实测 34 行、5 条 edit）复跑装配器至仓外新根 `C:/tmp/author-lane-r2-verify`——`ok:true`、5 件各恰 1 个具名 edit、全部 oldStr 恰命中 1 次，即现盘 gov-arq 原文与逐字块 byte 吻合。R1 修法中「§3.2 复扫声明须相应改写」为条件项：走 manifest 路线后 census 分类已完备，方法声明段落无需改写，视为解除。
- **F2 到位**。三条件分工如实：前两项机读（INDEX 生成件），retro 项明文「convention-only、不由闸解析或消费、不虚构机读判据」（OQ-2、T1 :67、gov-change 改后 :102、留根 1 四处一致）；记录落点钉死 = 后续批 PROPOSAL 头部 `landing-lane` 行旁 `default-trigger-evidence` 行，五要素（P9 changeId / INDEX 实值 / retro 路径 / 人读结论 / 日期）齐全。**可行性判定**：evidence 行落入 FROZEN PROPOSAL（冻结后不可回改），所引 INDEX 两项任何人可对机器生成件事后复核，retro 为仓内制品可回看，每批 reviewer 四查可复核——人读结论本身不可机判，但已被诚实标注为 convention-only 残余面且控制点在 human 立项 ack，与 human 裁定「立项时读取并记录」忠实。新增 V15 覆盖该机制的夹具验证。观察（非 finding、不构成本批缺口）：T5 结构门可低成本对「翻转后 author-implementation 批缺 default-trigger-evidence 行」fail-loud，属后续可选加固。
- **F3 到位（窄边界守住）**。`delivery/review/.changes-index-state.json` 以单一固定文件（非通配）入 T5 :199、T6-4 :216、OQ-1 :284、V8 :321 四处，口径一致；仍限于 delivery/review 生成面、与 INDEX 同脚本同生同写，不构成对「仅 review 面」边界的实质扩大。**但引入新矛盾 F7（见下）**。
- **F4 到位**：T2 :119 新增「同步修订 enforce-scope.mjs 的 Bash 分支头注……不再保留『Bash 不套 per-agent』的反事实绝对句」。
- **F5 到位**：T3 改后块（:160）保留「（261027 批 T1，ARQ-261027）」出处括注与「旧 lane 不再每批现写一次性脚手架」。
- **F6 到位**：V10 改为「architect 写 contracts、builder 写 scripts 的既有拒绝均不回归」，歧义消除。

**新矛盾扫描**：

- **F7（BLOCKING）· OQ-1 裁定记录保真：增补项被写成 human 原裁**。human 2026-10-28 裁 (a) 的证据尾清单为**两件**（`delivery/review/<changeId>/**` + `delivery/review/INDEX.md`，其它路径变更强制重验）；v0.3 的 OQ-1 段在【human 2026-10-28 裁 (a)】标题下把裁定文本原地改写为三件（新增 `.changes-index-state.json`），以裁定口径呈现未经 human 确认的扩充。增补本身是 R1 F3 提出的工程修法、reviewer 认可其实质（窄、生成件、同 trust class），被否的不是内容而是**记录形态与确认缺失**——OQ 裁定是 human 的裁定，评审循环无权把它改写为已裁事实。修法方向（二选一，均便宜）：① OQ-1 段分层——「原裁定（human 2026-10-28）」保留两件原貌 + 「R1 F3 增补（reviewer 提议）」单列，标明待 FROZEN 前 human 一次性确认；② 维持三件文本不变，FROZEN 标注前主线程以例外反选通道向 human 显式呈报该增补并留痕。两案都要求 human 在 FROZEN 前看到并确认第三件，不得静默并入已裁范围。
- 观察（NIT 级、随 F7 顺手可收）：F7 若走方案①，OQ-2 段的「触发条件」表述同步检查一遍（本稿 OQ-2 三条件表述与 human 裁定逐字一致，无此问题）。

**亲核留痕（R2 新增实测）**：`--preflight` 复跑 `ok:true / anchorCount:26 / findings:[]`；`--cite-check --phase draft` exit 0 / checkedCount:5 / gov-arq 块 matched；装配器复跑仓外新根 `ok:true`、5 件各 1 edit；`_scratch/edits.json` 实测 34 行、5 条 edit 且 newStr 与 §2 五组改后块逐一吻合；PROPOSAL 实测 360 行（申报 360 ✓）；R1 评审记录段逐字未被改动（append-only 守住）。

verdict: BLOCK(F7)

### R3 - 2026-10-28 - architect-reviewer

**轮次纪律**：只核 R2 F7 修订到位性，不做全量重核（manifest 与 edits.json 本稿未变，R2 的装配 / cite-check 复跑结论沿用）。

**F7 修订到位性（逐项亲核实测）**：

- **原裁两件文本逐字保真**：OQ-1 裁定段（:284）与 v0.2 原貌逐字一致——「采用 (a) 窄例外。只允许 `delivery/review/<changeId>/**` 与 `delivery/review/INDEX.md` 作为证据尾；其它任何路径变更都强制重新 reviewer GREEN + verifier PASS。debt 等其它收尾走既有独立通道，不扩进证据尾。」（对照 R1 起草期留档原文，byte 级一致）。
- **第三件不再呈现为原裁范围**：:286 独立段落单列第三件并标「human 显式确认并入（主线程转呈，R1-F3 增补）」；:288 追注明文「前两件属原裁范围，第三件是 R1-F3 的后续增补，不回写成原裁内容」——三层（原裁 / 增补件 / 确认追注）边界清晰，冒充已裁的形态已消除。
- **追注五要素齐**：日期 2026-10-28 ✓、通道 AskUserQuestion ✓、转呈方 主线程 ✓、增补来源 R1-F3 ✓、确认人 human ✓。human 确认本身发生于主线程会话、评审面无法独立重放，其可信度由转呈记录 + human 将在 FROZEN 制品与本 §7 留痕中可见共同承载；评审核的是记录不再误归属——达成。
- **全文残留扫描**：Grep `changes-index-state` 全稿 8 处定性——T5 :199 / T6-4 :216 / V8 :325 为机械路径清单（非裁定记录，无归属问题）；OQ-1 :286/:288 为分层与追注；§7 :357/:374 为评审记录引用；其余无把增补写成 human 原裁的残留。OQ-2 段与 R2 时一致、无同类问题。
- **机检复跑**：`--preflight` = `ok:true / anchorCount:26 / findings:[]`（OQ-1 改写未碰锚集）；全稿实测 386 行、§5.1 回执申报 386 ✓；edits.json 34 行未变。

**收尾核对**：R1 F1–F6 与 R2 F7 全部关闭；landing-manifest 围栏块 26 件与 packet-id 行在场（判官 GREEN 前结构字段在场义务）；OQ-1/OQ-2 裁定记录与 human 逐字裁定及确认通道一致，无偷换；自举禁令、非目标边界、验证集 V1–V15 消费层义务均未被修订削弱。

verdict: GREEN

### R3 后落地前可行性记录（非新评审轮）

- 主线程发现原 `packet-bound` 自声明与「26 件中 21 件仅有行为级规格」不兼容；human 于 2026-10-28 拍路线 B：本批一次性自用 `author-implementation`，主线程仅先机械落 scope-map 第 6 组授权 edit，architect 实现其余 25 件，reviewer 审真实 diff、verifier 钉同树与 manifest，旧闸合并，不构成后续批先例。

### R4 - 2026-10-28 - architect-reviewer

**轮次性质（如实标注）**：R3 GREEN 后主线程落地前可行性核发现 material amendment（lane 翻转 + 路线 B 一次性自用），评审重开；本 R4 是重开后的第 1 轮、全批第 4 轮。原「3 轮未 GREEN」熔断未触发过（R3 已 GREEN），重开周期自本轮重新计；本轮 BLOCK = 重开周期第 1 个未 GREEN。聚焦 v0.5 修订六点，不做全量重核。

**六点复核（①②⑤⑥ 通过，③ 硬缺口 F8，④ 通过但引出 F8，⑥ 一致性残 NIT）**：

- **① 授权范围钉死 ✓**：头部 `landing-lane: author-implementation` + `one-time-bootstrap` 注记（:7-8）、非目标「不把本批一次性自用解释为后续批可自行 bootstrap」（:50）、留根 6 重写（:431）三处一致；范围钉死为「主线程先落 scope-map 授权 edit + architect 实现其余 25 件，仅本批一次」。
- **② 留根 6 无自服务口子 ✓**：「后续任何批不得自行 bootstrap，除非其自身立项获得 human 同等显式授权」——例外权保留在 human 立项门，与 OQ-2 默认翻转（P9 三条件）互不打通；后续批无可自行援引的本批先例。
- **③ 序列可执行性 —— F8（BLOCKING）· bootstrap edit 后 architect 仍有 10/25 件被机械拦截，§5.3 步骤 2 按现文执行必然硬失败**。实测（隔离临时树装入装配产出的改后 scope-map，直调现 enforce-scope.mjs 喂 architect 写各路径）：放行 = scripts 4 件 + hooks 1 件 + commands 1 件 + references 7 件 + skills 2 件（如 `.claude/scripts/landing-fidelity-check.mjs -> exit=0`、`.claude/references/gov-change.md -> exit=0`）；**拦截 10 件**——`AGENTS.md`、`docs/DEV-GOVERNANCE.md`（deny 命中）、`.claude/agents/architect.md` / `architect-reviewer.md` / `review-moderator.md` / `adjudicator.md` / `landing-verifier.md`（deny 命中）、`docs/templates/PROPOSAL.template.md`、`docs/templates/architect-landing-summary.template.md`、`.claude/route-fixtures/surface-alignment.json`（allow 未命中，报「只能写 …」）。步骤 1 的第 6 组块只完成 scripts/hooks allow/deny 与凭据 deny 四项变化，不足以解锁「architect 实现其余 25 件」；执行到第 16 件即被 PreToolUse exit 2 卡死。且本批 manifest 为 26 件混合面，与 T1 新通道判据③「manifest 每个路径都位于 scripts/hooks」及「混合面永远走旧通道」的绝对句直接冲突（详见 F9）。修法方向（任一，均须改 §5.3 与 one-time-bootstrap 注记）：(a) 步骤 1 前置 edit 扩为「本批最小写权集」——临时 allow `docs/templates/**`、`.claude/route-fixtures/**` 并临时移除 AGENTS.md、DEV-GOVERNANCE.md 与五个 agent 定义的 deny（注意 deny 移除非 allow 追加可解：deny 先判）；十件逐一点名进授权注记，批终态由主线程恢复为第 6 组块、verifier 核对终态 scope-map == after 块；agent 定义五件属自改递归高敏面，human 路线 B 授权须显式覆盖「临时写五件 agent 定义」，否则授权范围与实际写权不符——此点建议经主线程向 human 补一句显式确认；(b) architect 只实现可写 15 件、主线程 packet-bound 落 10 件——但 10 件中仅 2 件有逐字块（AGENTS.md、architect.md），8 件回到 prose 代写，违背路线 B 初衷，须 human 重拍；(c) 重切 lane/manifest 结构。推荐 (a)。
- **④ 第 6 组 scope-map 逐字块 ✓**：cite-check draft 实测 `ok:true / checkedCount:6 / missing:[] / unverified:[]`（scope-map 原文块 byte 吻合现盘）；装配器 6/6 复跑（仓外新根 `C:/tmp/author-lane-r4-verify`）恰命中 1 次、scope-map afterLines=306 与回执一致；after 块仅含 architect 条目、只动该条目（allow 末尾 +2、deny 去 hooks/scripts、追加两条凭据 deny）✓。但「只动 architect 条目」的 after 块**不足以承载步骤 2**——这正是 F8。
- **⑤ 空窗诚实注记 ✓**：:422 如实声明 scope-map 授权落地到 enforce-scope 新代码生效之间非 main 拦截未机械化、不宣称已有新 hook 防线。补一条亲核观察（缩小暴露面、非 finding）：旧钩子跨树重锚仍按**目标树** scope-map 判权（:129 注记 + R1 实测），main 树自身 scope-map 未被本批步骤 1 改动，窗口内 architect 跨树写 main 的 scripts/hooks 仍被 main 树旧 map 拦截——空窗暴露面限于 gov 树本分支，与注记「极短窗口兜底」相容。
- **⑥ 计数一致性**：「其余 25 件」口径全文一致（:8 / §5.3 :417-419 / 留根 6 :431，均 = 26 − scope-map）✓。但 **F10（NIT）· 5+21 计数 stale**：头部 :8「26 件中 21 件只有行为级规格」写于第 6 组 scope-map 逐字块加入之前，现为 6 块 + 20 件行为级规格，按现文读与 6 个逐字块并存矛盾；修法 = 改为 20 件或标注「路线 B 决策时点（第 6 组块加入前）状态」。R3 后可行性记录（:494）的 21 件系决策时点历史留痕，不改。

**F9（SHOULD）· one-time-bootstrap 注记未显式声明本批是 T1 判据③与「混合面恒旧通道」绝对句的一次性 human 授权例外**。v0.5 后本批 = 混合面批走 author-implementation；而本批自身要立的 canonical（改后 AGENTS / gov-change / T1 :66）写死「混合面永远走旧通道」。canonical 生效后读回，founding batch 自身无例外注记即违反刚立的规则。修法：one-time-bootstrap 注记加一句「本批为混合面批，human 路线 B 授权即 T1 判据③与混合面绝对句的一次性例外；绝对句自本批 canonical 落地起对新批生效」。与 F8 修法 (a) 的授权注记可合并为一段。

**机检复跑（R4 新增实测，不采信回执）**：preflight `ok:true / anchorCount:26 / findings:[]`；cite-check draft exit 0 / checkedCount:6 / 零缺失；装配器 6/6（scope-map afterLines=306）；edits.json 40 行 ✓、PROPOSAL 实测 494 行 ✓（申报 494 ✓）；隔离树钩子直调 12 路径探针（10 拦 2 放行）如上逐条 verbatim。R1–R3 评审记录段未被改动。

verdict: BLOCK(F8)

### R5 - 2026-10-28 - architect-reviewer

**轮次性质**：重开周期第 2 轮、全批第 5 轮；聚焦 R4 F8/F9/F10 修订到位性，不做全量重核。

**R4 findings 修订到位性**：

- **F8 部分到位，残留 F11（BLOCKING）**。三态链结构 ✓、恢复强制性时序 ✓、终态一致性 ✓、十件点名与 human 确认记录 ✓——但临时授权块对十件中**两件机械无效**（见 F11），「architect 实现其余 25 件」仍会在 AGENTS.md / docs/DEV-GOVERNANCE.md 上硬失败。
- **F9 到位 ✓**：one-time-bootstrap 注记（:8）新增「本批为混合面批，human 路线 B 授权即 T1 判据③与混合面绝对句的一次性例外；绝对句自本批 canonical 落地起对新批生效」——豁免面恰为本批，新批不继承。
- **F10 到位 ✓**：:8 改「6 组逐字块 + 20 件行为规格」，计数一致。

**F11（BLOCKING）· 临时授权块对十件中两件（AGENTS.md、docs/DEV-GOVERNANCE.md）机械无效——deny 移除不等于解锁，allow 白名单仍未命中**。实测：从 PROPOSAL 临时块构建临时态 scope-map、隔离树直调现 enforce-scope.mjs——`.claude/agents/` 五件、两模板、route-fixture、scripts 共 8 件放行（exit=0），但 `AGENTS.md` 与 `docs/DEV-GOVERNANCE.md` 仍 exit=2，报错 verbatim 为 allow 未命中形态（「只能写 …；AGENTS.md 超出边界」「…docs/DEV-GOVERNANCE.md 超出边界」）；`allow has AGENTS.md: false | allow has DEV-GOV: false | deny has AGENTS.md: false`。根因：enforce-scope 判序 = deny 命中先拦、allow 非空且未命中再拦（:260-267）——这两件是仓根/白名单外文件，不在任何 allow pattern 内，仅从 deny 移除后落入 allow-miss 仍被拦（与五件 agent 定义不同：其父 glob `.claude/agents/**` 本在 allow，移除 deny 即通）。修法：临时授权块 allow 追加两件精确路径 `"AGENTS.md"`、`"docs/DEV-GOVERNANCE.md"`（终态恢复块不加——恢复即收回）；链式 edit 2 的 oldStr 须同步为新临时块（装配器恰命中断言对失配 fail-loud 兜底）；修后重跑本隔离树探针，十件必须全 exit=0。

**F12（NIT）· 临时目录 glob 超覆盖披露缺失（实测）**：临时 allow 用 `docs/templates/**` 与 `.claude/route-fixtures/**` 两条目录 glob，机械 grant 面宽于十件点名——探针实测 `docs/templates/retro.template.md -> exit=0`、`.claude/route-fixtures/routing.json -> exit=0`（目录内其余 8 个模板件与 routing.json 同样临时可写）。兜底链真实存在（changedFiles==manifest 对账 + reviewer 专项审 + 终态恢复），不构成放行风险；建议授权注记（:9）补一句「机械 grant 为两条目录 glob、覆盖面略宽于十件点名，超grant成员由 manifest 对账与终态恢复兜底」，保持注记与机械面诚实对齐。

**其六点复核通过项**：

- **三态链与逐字性 ✓**：装配器 6 件 7 edit 复跑（仓外新根 `C:/tmp/r5-tmpverify`）exit 0，scope-map 两条链式 edit 均恰命中 1 次；**终态 scope-map 与 v0.5 稳定态 byte 级一致**（`cmp` 对 R4 装配产物实测全同）；cite-check draft exit 0 / checkedCount:6（现盘原文 6 块 byte 吻合）；preflight `ok:true / anchorCount:26 / findings:[]`；全稿实测 549 行、edits.json 46 行（申报吻合）。
- **临时块十件覆盖对账（8/10 实放行，见 F11）**：五件 agent 定义（父 glob 在 allow + deny 移除）✓、两模板 + route-fixture（allow 追加）✓；保留拦截实测 verbatim：`CLAUDE.md -> exit=2`、`.claude/scope-map.json -> exit=2`、`.claude/agents/scenario-author.md -> exit=2`、凭据 JSON `implementation-review.json -> exit=2`——临时态不放松凭据禁写与 scope-map 自写 ✓。
- **恢复步骤强制性与时序 ✓**：§5.3 步骤 3「强制前置……未恢复不得派 reviewer / verifier」（:454）；步骤 4 reviewer 专项核前置授权 commit + 终态恢复 commit + 最终 scope-map（:455）；步骤 5 verifier 核恢复后同 tree 终态（:456）；步骤 6 合并以终态核验通过为前置（:457）。
- **授权注记 ✓**：新增 `bootstrap-write-scope` 行（:9）十件逐一点名 + human 2026-10-28 AskUserQuestion 确认记录 + 五件 agent 定义自改递归高敏面显式标注 ✓。
- **留根 6 ✓**：更新为「临时授权 → 实现 25 件 → 主线程恢复稳定态后再审验」全链，一次性边界与 human 授权通道保留。

**轮次计数**：重开周期 R4、R5 连续两轮未 GREEN——按熔断条款「评审 3 轮未 GREEN → human 三选一」，重开周期再一轮（R6）仍不绿即触熔断。F11 修法为两条 allow 追加 + 链式块同步，改动极小。

verdict: BLOCK(F11)

### R5 后主线程直接实施记录（非 reviewer verdict）

human 明确指示停止继续打磨 PROPOSAL，由主线程直接实施，再交 architect-reviewer 审真实 diff、landing-verifier 正常验证。主线程据此跳过继续起草循环，未使用 §5.3 的临时写权链；scope-map 直接落稳定终态。该一次性流程跳过已如实记录，后续评审与独立核验不豁免。

### R6 - 2026-10-28 - architect-reviewer

**轮次性质**：human 于 R5 后指示主线程直接实施；本 R6 审磁盘真实实现 diff（26 件 manifest），不重走起草轮次。R1–R5 记录保留不重核；本批头部现为 FROZEN v1.0 + `landing-lane: packet-bound`（仅本批），故本批自身落地走旧通道判据。

**七问逐条实测结论（全部亲核，不采信回执）**：

1. **双通道条件收口 ✓**：`governance-gate-shared.mjs` `parseLandingLane` 缺省/缺失 = packet-bound、未知值与重复冲突 fail-loud（`INVALID_LANDING_LANE`）；landing-fidelity 与 pre-merge 两门在 author lane 均硬查 manifest 全落 `.claude/scripts|hooks/**`，任一越界即 fail；gov 非 main 由 enforce-scope 写权层（见 3）+ 闸既有 off-main 判定承载；human 选择/默认触发为 prose 约定（`landing-lane` 头字段 + `default-trigger-evidence` 行），闸不解析默认翻转——与 OQ-2 一致，无机械翻转口。混合面 / 契约 / spec / G1 / settings / agent 定义 / scope-map 无任何新通道口（manifest 检查两处 + prose 恒旧通道句，diff 已逐一落地）。
2. **scope-map 终态 ✓**：实盘 architect 条目与 PROPOSAL T2「终态恢复改后」块逐字一致——allow 仅追加 `.claude/scripts/**`、`.claude/hooks/**`；deny 移除该两项、追加 `delivery/review/**/implementation-review.json` 与 `…verification.json`，self / review-loop 八 def、G1（AGENTS.md / CLAUDE.md / DEV-GOVERNANCE.md）、scope-map、settings deny 全保留；无临时十件残留、无额外权限夹带。实盘 diff 集合 == manifest 26 件 ∪ 证据尾（INDEX.md 一行 upsert + 新 review 目录）∪ codebase-memory 两件豁免件，零越 manifest 改动。
3. **enforce-scope ✓**：`--self-check` 复跑 20/20 全绿（新增 11 臂：main/detached/跨树 fail-closed、gov 非 main 放行、git add/commit 同树非 main、-C 跨树拦、两凭据 JSON 拦）；另做**真树 14/14 探针**（构造 stdin 直调钩子）：gov 树 scripts/hooks 写放行、main 树与跨树拦（报错 verbatim 命中新分支）、architect-reviewer def / scope-map / 两凭据 JSON 仍拦、landing-verifier 写 verification JSON 放行、builder 与主线程 Bash 行为不回归、universal bashDenyWrite 保留。Bash 分支头注已按 T2 改写，反事实绝对句消除。
4. **凭据链与 Git 重算 ✓**：`readImplementationEvidence` 单源 fail-loud（schemaVersion / lane / changeId / 40 位小写 SHA / changedFiles 形状）；闸侧重算 verifiedCommit==reviewedCommit、verifiedTree==reviewedTree==`git rev-parse <reviewedCommit>^{tree}`、merge-base 祖先、`main...reviewedCommit` 与 `main...source` 非证据改动恰等 manifest、`implementation-review.changedFiles==manifest`、`reviewedCommit..source` 仅允许三件证据尾。**本审自建临时 Git 仓夹具独立复现**：happy 双门 exit 0；debt 尾 BLOCK、rebase 旧 SHA BLOCK（exit 1 点名原因）；pre-merge `--self-check` 复跑 96/96（旧 88 + 新 8，内含真实临时仓双 CLI happy / 旧 SHA / 越 manifest / 错 tree）。fail-closed 边界：git 不可用、JSON 缺字段、SHA 非 40 位、lane 不符均拦。
5. **旧通道回归 ✓**：landing 旧 12 组自检全绿（计数与基线一致）；packet-bound 缺省路径逐字节保留；无双头 SSOT（gov-change.md 为作者通道唯一流程 SSOT，arch-review-protocol 只留指针，lib 单源不复制正则）；消费层测试真实——pre-merge 自检含真实临时 Git 仓 spawn 双 CLI（非纯函数自检），本审另独立复现三路。
6. **prose 一致性（一处残留，见 F13）**：26 件中承重改后文本与 PROPOSAL 改后块逐字一致（AGENTS.md landing 通道句、gov-change Track B 后端、gov-arq 路由注、architect.md Landing / 机械兜底、模板装配句、summary 模板指针段）；Grep sweep 全 canonical，「architect 恒不 landing / 永不 landing / landing 恒 = 主线程」类绝对句在 AGENTS.md / DEV-GOVERNANCE / agents / commands / references 主体已清，**仅 arch-review-protocol.md:191 一处残留**；历史制品（approved-landing 退役注记、delivery/**）不属反事实。
7. **无私货 ✓**：diff 无 PROPOSAL 未授权改动；scope-map 未越「终态恢复改后」块；release-pr 仅注释、assemble-edits 仅头注，算法与发布逻辑零变。

**findings**：

- **F13（SHOULD）· arch-review-protocol.md:191 残留「landing 恒 = 主线程 packet-bound」绝对句**（Governance profile 定稿节；:183/:188 定稿流程段亦假定 packet-bound landing 无条件成立）。首个 author-implementation 批（P9 在即）生效时即为现行反事实——与 T2「同步而非留成反事实」标准及 §3.2 复扫完备性声称（七组字面含 packet-bound、该行 landing 相关）不符；R1-F1 同类先例曾判 BLOCKING，本次降档理由：该行自带「见『双轨路由』节」指针、双轨路由节已承载正确双 lane 规则，无路由误导且无机械消费面，非目标效果级缺口。修法：落 landing commit 前顺手改一句（本文件在 manifest 内、零额外成本），或随 P9 批修并在本批 landing-summary 诚实注记残留。
- **F14（NIT）· landing-verifier.md:18 与 architect.md:69 的 Track A 定稿 / 派遣句仍写「主线程 packet-bound landing 后派 / 落 canonical」无条件成立**——同段 defer 到已更新的双 lane 节，误导面极小；随批顺手可收，不强制。
- **F15（NIT）· architect Bash git 检查三形绕过实测**（构造 stdin 直调钩子）：`git -C<path>`（无空白）、`--work-tree=<path>`（无空白）连「拒绝 -C/--git-dir/--work-tree 跨树」的具体声明也 defeats，`cd <main> && git add` 形态绕过同树判定（钩子校验会话树非命令生效树）。属 T2/§6 已披露的「Bash 非完整沙箱」残余面，合并闸 Git 重算 + 人合门兜底仍在，不构成放行风险；建议后续批把 -C/--git-dir/--work-tree 判定改为「git 子命令前出现即拦（不要求空白）」并对 cd 形态显式注记归残余面。
- 观察（不构 finding）：`collectAuthorImplementationGate` 的 execFn 无 cwd 参数（沿 buildCtx 既有约定 = 进程 cwd），门须从仓根调起；与文件内既有探测同约定，不新引入。

**亲核留痕**：enforce-scope self-check 20/20、landing-fidelity self-check 全绿（12 组旧 + author-lane-structure 11/11，计数与起草基线一致）、pre-merge self-check 96/96、assemble-edits 16/16、route-regression 104/104、release-pr 26/26、settings-lint 6/6 全部本机复跑；preflight `ok:true / anchorCount:26`（26 锚与 manifest 逐件一致）；diff 集合 == manifest ∪ 证据尾 ∪ codebase-memory 豁免件；自建临时 Git 仓三路 gate 探针 + 干净夹具 landing happy exit 0；enforce-scope 真树 14/14 探针；`git -C` 无空白 / --work-tree / cd 三形绕过探针；R1–R5 评审记录段逐字未被改动（append-only 守住）。

verdict: GREEN
