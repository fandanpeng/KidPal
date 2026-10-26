# PROPOSAL · 261026-gov-review-merge-gate-cluster-fix

> **状态：FROZEN v1.0 · 2026-10-26 · 主线程凭「human 立项 ack + 四个 OQ 已裁 + architect-reviewer R3 GREEN」冻结（R1 BLOCK(F1,F2,F3)+NIT → R2 补获 F4 → R3 GREEN，评审记录 §7）**
> **承载 ARQ**：`ARQ-261017-merge-gate-contract-evidence-ref`（根）、`ARQ-261021-merge-gate-first-merge-blindspot`（与上一条**同根合并消费**——human 2026-10-26 已裁「一份改法覆盖两根，不双头修」：两根 = 取件 ref + verdict 首匹配）、`ARQ-261017-merge-gate-scope-unstated`、`ARQ-261014-merge-gate-tooling-hotfix-batch`、`ARQ-261011-contract-gate-literal-selfquote-false-positive`、`ARQ-261018-enforce-scope-worktree-anchor-offbyone`（MAJOR，human 已裁搭载本批）。**留根不关**：`ARQ-260924-landing-gate-dir-anchor-gap` 残余②（landing 门目录前缀展开）已由 human 裁出留后批。
> **review 轨**：Track B 主路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。**不判轻**：本批全部触机器面（`.claude/scripts/**`、`.claude/hooks/**`、`.gitattributes`）及 G1/G2 联动文面——按 gov-change 判档「路径即判据」，机器面恒 Track B。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T8）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：三处合并链机械判据 + 一处钩子锚定改动，行为面全部向准或归因修正；但改的是**每次合并必经**的闸与**每次写入必经**的钩子，影响面覆盖全仓，故不按 low 报）。
> **review profile**：G1+G2+G3（`AGENTS.md` / `references`+`skills` / `scripts`+`hooks`+`.gitattributes`）；**契约面零触碰**、**agent 定义零改动**、无新增门。

```landing-manifest
AGENTS.md
.claude/hooks/enforce-scope.mjs
.claude/references/pod-coordination.md
.claude/scripts/merge-driver.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/skills/merge-to-main/SKILL.md
.gitattributes
```

---

## 0. 想实现的效果与差距

**目标效果**：六个在册的「合并闸 / 强制执行层」机器面缺陷一次收口，使三道机械设施（前置升级闸、merge-driver、enforce-scope 钩子）在真实拓扑与真实产物形态下**判得准、说得清、不误伤**：

1. **契约凭据对首合并可达**：走 `/arch-change` 触契约分支的批次，三条凭据齐备时合并闸能机械放行，不再「三件齐全仍报缺凭据、只能升级人手工 ff」。
2. **verdict 锚取终态**：landing-verdict 按 append-only 纪律保留 FAIL→PASS 轨迹时，闸与 INDEX 都读得到最终判定。
- **非 lane 制品不再撞「不认识你」**：跨面 intake spine 与 debt 账本四域（gov / tech / requirement / contract）条目的合并，闸报「不适用」而不是「不得自动合并」；混入实现面 / 契约面、或落在账本拓扑之外的变更集仍拦。
4. **契约类 findings 只认真 finding**：review-packet 里「本轮无 hard-violation」这类自述行不再被当成 finding 拦合并。
5. **生成件相撞不升人**：各 lane 的 `INDEX.md` 与 `current/` 投影的派生态冲突，走渲染器 / merge-driver 的既有权威消解，不再升级 human 消解评审。
6. **跨树锚定判对拓扑**：worktree 内子 agent 写同仓合法路径不再被误拦，也不再静默降级 legacy（绕行 Python 直写 = 护栏失效且无人察觉）。

**现状差距（全部为 2026-10-26 起草期在 HEAD 上的实测，见 §2 各 T 证据）**：

- 契约凭据从 `main` 取 PROPOSAL，而候选 PROPOSAL 由构造必然只在源分支 → 首合并契约批恒 BLOCK（复现：fixture 输出 `{"frozen":false,"verdict":true,"covered":false}`）。
- verdict 取**第一条**匹配行 → FAIL→PASS 轨迹恒判非 PASS（复现：`{"frozen":true,"verdict":false,"covered":true}`）。
- kind 探测只有 dev / gov 两 lane；intake / debt 制品 `kind=unknown` 恒 BLOCK，且 `--human-approval-ref` 对 unknown / gov 直接 FATAL exit 2（复现逐字见 T3）。
- findings 扫描 = 行含 `hard-violation` 且行含契约字面 → 89 份 review-packet 中 11 行命中，其中 **3 行是自述「无 finding」**（误报面）。
- `delivery/review/INDEX.md` 双分支尾追加 → merge-tree 实撞 → 闸报 `parallel-overlap-tier3`；`current/` 投影相撞同样报 tier3（两份 fixture 各自复现）。
- worktree 的 `.git` 指向 gitdir **目录**，钩子按「gitdir 是文件」假定 `dirname^3`，多剥一层致 commonDir 落仓根 → 跨树合法写被拦（真实树双例复现，退出码均为 2）。

**非目标（本批不做，防 scope creep）**：

- 不新增任何工程门、不放宽 dev / gov 两条 lane 的既有判据、不动契约面与人类签字的既有防线（Bash 重定向写恒拦 / 实现面 agent 契约禁写）。
- 不做 `ARQ-260924-landing-gate-dir-anchor-gap` 残余②（landing 门的目录前缀展开）——human 已裁拆出。
- 不在本批落 `ARQ-261014` 附带教育面 ④（final 相位派发契约，跨 8 包复现）——**human 已裁拆出独立批**（§4 OQ3），已由主线程登记为 `ARQ-261026-final-phase-dispatch-contract-unwritten`；本批范围明确划出 ④，§6 留根指向该条。
- 不新增 INDEX 陈旧检测器（见 §6 留根：本批以「重跑渲染」形态承载，理由与复议触发写在留根节）。

## 2. 问题、根因与改法（T1…T8）

### 2.0 复证结论与分项表态（六条逐条，起草期实测 2026-10-26）

> 纪律：ARQ 行是待验证假设。以下「结论」列的每一条都对应 §2 各 T 证据段里的一条实跑命令与输出；无实测的推断显式标「未实测」。

**复证 1 · `ARQ-261017-merge-gate-contract-evidence-ref`** —— 处方仍有效，落点未漂移（行号有漂）。

- `:143` 仍是从 **target**（调用方硬绑字面量 `main`）取件 PROPOSAL（完整原文见 T1 证据的 fenced 块；现文件 790 行；ARQ 正文引的 `:425` / `:431` 已漂至 `:443` / `:449`，`:143` 行号未漂）。
- 复现成立：fixture（PROPOSAL 只在源分支 + 契约 diff + 三件凭据齐备）→ BLOCK，逐字含 `已检查 = [{"dir":"delivery/review/261026-arch-review-fx","frozen":false,"verdict":true,"covered":false}]`，同时 `git show main:<PROPOSAL>` 失败、`git show feat:<PROPOSAL>` 成功。
- **与 ARQ 正文的一处不符**：ARQ 说 `--human-approval-ref`「走的是另一套核验（人批事件），不解决本判据」；实况更强硬——`kind=gov` 带该 flag 直接 `FATAL: --human-approval-ref 仅适用于 dev-loop 档`、`exit 2`（`:765-767`）。即对 arch-lane 契约批，人批 flag 不是「帮不上忙」而是「被拒收」。
- 处置：建议 1（取件 ref）→ T1；建议 2（归因自检）→ T1；建议 3（处置路径成文）→ T8；建议 4 → 见复证 4 结论（(a)(b) 已消化，只剩 (c)）。

**复证 2 · `ARQ-261021-merge-gate-first-merge-blindspot`** —— 两根均仍有效；human 已裁合并消费。

- 根 1 = 复证 1 的同一处代码（`:143` + `:449` 调用点），一份改法覆盖。
- 根 2：`:149` 逐字 `const vLine = vText ? vText.split('\n').find((l) => VERDICT_STATUS_RE.test(l)) : null;` ——**首匹配**。同文件 `readGovAnchors` 的 green 锚（`:83-90`）已是「逐行扫、取最后一个命中」；`changes-index.mjs:202-231` 的 `deriveLanded` 也是「后出者胜」。即同仓已有一致的终态语义先例，仅契约凭据这处漏了。
- 复现成立：PROPOSAL 在 main（可读）+ 源分支追加 `verdict: FAIL` → `verdict: PASS` 轨迹 → BLOCK，逐字 `{"frozen":true,"verdict":false,"covered":true}`。
- 方向 ③（明文承认首合并走人批链、改 `arch-change.md` 叙事）：**不建议单取**——它与 261015 批「无人签门、合并过机械凭据闸」的裁定方向相反，且会让「机器可放行」永久退化为纸面。③ 里真正有价值的部分是「闸自身假阴性时人怎么走」，那是一条**兜底通道**，独立于「首合并常态」——归 T8。取舍见 §4 OQ2。

**复证 3 · `ARQ-261017-merge-gate-scope-unstated`** —— 处方仍有效，三项候选未落。

- `:441` 逐字 `const kind = hasArch ? 'gov' : hasDev ? 'devloop' : 'unknown';`；`:416` 的 unknown 分支输出逐字为「unknown change kind（changeId 既无 delivery/taskpackages 也无 delivery/review）」（见 T3 证据实跑输出）；`:765-767` FATAL 分支未动。
- 复现成立：intake-only fixture → `BLOCK (changeId=261026-cross-face-x, kind=unknown)` + `unknown change kind（…）`；带 `--human-approval-ref evt-1` → `exit 2`，逐字 `FATAL: --human-approval-ref 仅适用于 dev-loop 档（changeId=261026-cross-face-x kind=unknown，无人批豁免概念）`。
- 处置：OQ1 已裁 = **候选 2 形态**，且允许集**从当前账本拓扑正推枚举**（`docs/debt/{gov,tech,requirement,contract}/` 四域 + `delivery/intake/`，锚 `repo-structure.md` 的账本拓扑行，拓扑扩域时随其更新）——**不按历史 lane 假设硬编码**（起草 v0.1 曾用 `docs/debt/` 整前缀 + retro/archive，已按裁定收窄，见 T3）；建议 3（补写适用范围）→ T3 两处文面；建议 4（如实记 convention-only）→ §6 留根。

**复证 4 · `ARQ-261014-merge-gate-tooling-hotfix-batch`** —— ①已消化、②仍有效、③仍有效、④建议拆出。

- **①「两热修未入账留痕」= 已消化**：两条热修代码均在树（`merge-driver.mjs:172-183` prose-shape 降级；`:1423-1427` target 声明上提），且 `ARQ-261014` 本条目就是它们的入账载体。真残件 = 两处代码注释仍写「gov 侧待追认」（`:173`、`:1426`）——由 T7 改为追认并引本批。
- **② 为两处热修补防回归验证 = 仍有效**：`--self-check-detect-changes` 16 例全部注入 `runCli`，从未测到 `defaultDetectChangesCli` 的解析层；`--self-check-rebase-target` 7 例测的是解析函数本身，不覆盖「声明被挪进 `if (!DRY_RUN)` 块」这类 TDZ 形态。两条热修至今**零防回归断言**。→ T7。
- **③「生成件与手写相撞」= 仍有效**：无 union 属性时 fixture 复现 `BLOCK … parallel-overlap-tier3 … 实撞 1 件（delivery/review/INDEX.md）`；加一行 `merge=union` 后闸直接 `PASS`（`git merge-tree` 不再产冲突行）。`current/` 投影相撞独立复现（另一份 fixture，同样 tier3）。→ T5。
- **④「教育面三例」（含 2026-10-25 retro 的八包复现）= human 已裁拆出独立批**：它已长出「final 相位三份契约（谁跑 / scope-file 谁产 / round 序号怎么算）」的完整主题，落点是 dev-loop 派发面而非合并闸机器面；本批范围明确划出 ④，由主线程另行登记 governance debt 新条目（占位见 §6 留根）；新条目 = `ARQ-261026-final-phase-dispatch-contract-unwritten`（主线程 2026-10-26 登记）。

**复证 5 · `ARQ-261011-contract-gate-literal-selfquote-false-positive`** —— 处方仍有效；扫描面已全量 census。

- `:195-198` 逐字仍是「行含 `hard-violation` 且行含字面集任一字面 → 计为 finding」的行级过滤，无否定 / 列位判别。
- 89 份 review-packet 全量扫描（命令见 T4 证据）：旧判据 11 行命中 = **8 行真 finding 表行 + 3 行自述「无 hard-violation finding」散文**（`261010-device-reconnect-on-server-close:47`、`261010-p3-storylist-cold-start-restore:60`、`261011-p1-home-v5-rework:51`）。新判据（只认 Findings 表 **Kind 列**）→ 命中 8 行、散文 3 行全消。8 行里有一行 Kind 写作 `**hard-violation**（原判）`（`261016-p3-story-body-sd-cache:71`），故判据必须用**前缀**匹配而非等值。
- **与 ARQ 倾向的一处偏离**：ARQ 倾向「①否定判别 + ③写作约定」。本批建议 **②列位判据 + ③**：①是自然语言启发式（「无」以外的措辞 —— 如「非 finding」「不适用」——仍会绕过，且无法区分「复述」与「声称」）；②直接锚 `integrator.md:40` 已写死的六列表（`Severity | Kind | Area | Finding | Basis | Required action`），是机械可判的形态事实。judge 可就此 challenge（见 §4 无 OQ，属技术取舍，本批建议已给理由）。

**复证 6 · `ARQ-261018-enforce-scope-worktree-anchor-offbyone`** —— 处方仍有效，二次命中后第三次实跑仍复现。

- `enforce-scope.mjs:141-146` 逐字 `const gitdir = resolve(cur, m[1].trim()); // <commonDir>/worktrees/<name>/gitdir` + `return { treeRoot: cur, commonDir: dirname(dirname(dirname(gitdir))) };`；夹具 `:277-279` 按「gitdir 是文件」构造，故自检 6 臂全绿却测不到真实拓扑。
- 真实树复现（2026-10-26，本 gov worktree）：① gov 树内 builder 写主树 `firmware/probe.c` → `exit 2`，逐字 `scope: agent "builder" 只能写 …；../KidPal/firmware/probe.c 超出边界。`；② 主树会话 landing-verifier 写 gov 树 review 目录 → `exit 2`，逐字 `../KidPal-gov/delivery/review/261019-gov-review-local-memory-extraction/landing-verdict.md 超出边界。`（与 2026-10-19 事故同形）。
- 处方「`dirname^3` → `dirname^2`」方向正确，但**需按拓扑分支**（不能一律 `^2`：旧式 gitdir-file 拓扑仍是 `^3`），且夹具必须换成真实拓扑才能有判别力。→ T6。

### T1 · 契约凭据取件 ref：source 优先、target 兜底，取件全失败点名 ref

**问题**：`hasArchLoopContractEvidence` 用 `git show main:<proposalPath>` 读候选 PROPOSAL，而候选 PROPOSAL 之所以进候选，正因它出现在 `source..main` 的 diff 里——按定义它**不在 main 上**。于是 `frozen` 恒 false → 三件联立恒不成立 → 每个走 `/arch-change` 触契约分支的批**首合并必 BLOCK**，且报的是「缺凭据」（读起来像落地方的过失），把注意力引向错误方向。

**证据**（2026-10-26 起草期，`c:\tmp\mgfx` 下构造 fixture 仓 + 实跑 HEAD 闸）：

- 源码落点（逐字，`.claude/scripts/pre-merge-escalation-gate.mjs`）：

```js
// :139-144
for (const proposalPath of proposalPaths) {
  const dir = proposalPath.slice(0, proposalPath.lastIndexOf('/'));
  let proposalText = null;
  try {
    proposalText = execFn(`git show ${target}:${proposalPath}`);
  } catch { /* PROPOSAL 不在 target ref → frozen 判 false */ }
```

- 调用点（逐字）：`:449` `const hasContractGateEvidence = hasArchLoopContractEvidence(execFn, source, 'main', projectDir);` —— `target` 恒为字面量 `'main'`。
- fixture 实跑（git init → main 一条 init commit → 分支 feat 加契约文件 + PROPOSAL（FROZEN + manifest 含契约路径 + `verdict: GREEN`）+ `landing-verdict.md`（`verdict: PASS`）→ `node .claude/scripts/pre-merge-escalation-gate.mjs 261026-arch-review-fx --source-ref feat`，cwd = fixture 仓）：

```
[pre-merge-escalation-gate] BLOCK (changeId=261026-arch-review-fx, kind=gov):
  - docs/contracts/** diff 且同批无契约合并凭据（contract-evidence: 无候选满足三件联立（FROZEN + PASS verdict + manifest 覆盖）；契约路径 = docs/contracts/interface-contract.md；已检查 = [{"dir":"delivery/review/261026-arch-review-fx","frozen":false,"verdict":true,"covered":false}]）——…
R1 git show main: FAILED
R1 git show feat: ok
```

- 反向对照（同一 fixture，改法打补丁后的原型实跑）：`PASS (changeId=261026-arch-review-fx, kind=gov) — 无例外命中，可交给 merge-driver.`（原型路径 `c:\tmp\mgfx\proto\gate.mjs`，非落地件）。

**根因**：取件 ref 与候选来源方向相反。候选集来自 `source..target` 差集（`:137`），取件却定向到 target。「首次合并」不是特例，而是这条路径的**唯一**新批形态——PROPOSAL 随批在源分支首次出现。同根的第二表现是归因：三件塌了以后只报「缺凭据」，不说「取件失败」。

**改法**：

1. 取件改为 **source 优先、target 兜底**（兜底服务于「PROPOSAL 已随前次合并落到 main、本批再改」的复用形态），并把实际生效的 ref 记进 `checked` 明细，便于事后归因。
2. 全部候选在两 ref 都读不到时，报「**候选 PROPOSAL 在 source/target 两 ref 均不可读（疑似取件 ref 或路径错，非缺凭据）**」，把误导性归因消掉（ARQ-261017 建议 2）。
3. 自检新增两条判别夹具（构造见 §5 V1；反向实测：把实现改回 target-only → 第一条夹具红，`pass:61,fail:1`）：source-only 可读 → 三件齐备应 `ok:true`；两 ref 均不可读 → `ok:false` 且 reason 含「两 ref 均不可读」。

替换后代码（逐字，落 `:139-146` 区段）：

```js
    // 261026 批 T1（ARQ-261017-merge-gate-contract-evidence-ref）：候选 PROPOSAL 由构造必在 source 侧
    // （proposalPaths 来自 source..target diff）——source 优先、target 兜底（复用/已落批场景）。
    let proposalText = null;
    let refUsed = null;
    for (const ref of [source, target]) {
      try { proposalText = execFn(`git show ${ref}:${proposalPath}`); refUsed = ref; break; } catch { /* 下一 ref */ }
    }
    if (proposalText === null) { checked.push({ dir, refError: true, frozen: false, verdict: false, covered: false }); continue; }
```

替换后 reason（逐字）：

```js
    reason: checked.length && checked.every((c) => c.refError)
      ? `contract-evidence: 候选 PROPOSAL 在 source/target 两 ref 均不可读（疑似取件 ref 或路径错，非“缺凭据”）；契约路径 = ${contractPaths.join(', ')}`
      : `contract-evidence: 无候选满足三件联立（FROZEN + PASS verdict + manifest 覆盖）；契约路径 = ${contractPaths.join(', ')}；已检查 = ${checked.length ? JSON.stringify(checked) : '无候选 PROPOSAL（同批无 arch-review 批）'}`,
```

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T2 · verdict 锚取终态 + 形态缺行点名

**问题**：契约凭据的 verdict 位取 `VERDICT_STATUS_RE` 命中的**第一行**。landing-verdict 按 append-only 纪律保留 FAIL →（修复后）PASS 的翻正轨迹时，首行恒为 FAIL → 永判非 PASS。这是「诚实留痕」与「单源首匹配」的语义相撞，不是落地方的失误。

**证据**：

- 源码落点（逐字）：`:149` `const vLine = vText ? vText.split('\n').find((l) => VERDICT_STATUS_RE.test(l)) : null;`。
- 仓内已有两项同向先例（起草期读盘确认）：`pre-merge-escalation-gate.mjs:83-90` 的 `isGreenVerdict` 逐行扫**取最后一个**命中；`changes-index.mjs:202-231` 的 `deriveLanded` 同样是「后出者胜」并叠加 JSON 行锚。
- fixture 实跑（PROPOSAL 在 main 可读、源分支追加 FAIL→PASS 轨迹；隔离取件 ref 变量）：

```
[pre-merge-escalation-gate] BLOCK (changeId=261026-arch-review-fy, kind=gov):
  - …已检查 = [{"dir":"delivery/review/261026-arch-review-fy","frozen":true,"verdict":false,"covered":true}]…
```

- 补丁后原型实跑：`PASS (changeId=261026-arch-review-fy, kind=gov) — 无例外命中，可交给 merge-driver.`。

**根因**：判定语义（终态）与解析实现（首匹配）不一致；同仓两处相邻判据已用终态语义，唯独这处没跟上。

**改法**：

1. 新增 `lastVerdictLine(text)` 纯函数（逐行扫、取最后一个命中），`:149` 改调它；文件级语义与 `isGreenVerdict` / `deriveLanded` 对齐。
2. **形态缺行点名**（`ARQ-261017` 建议 4 的落点 (c)，见 §4 OQ4）：`landing-verdict.md` 在场但 `VERDICT_STATUS_RE` 命中 0 行时，`checked` 明细带 `verdictFormatMiss: true`；同时明细带生效的 `refUsed`（`source` / `target`）。两者一起出现在 BLOCK reason 的 `已检查` JSON 里，读的人能直接看出「三件里哪件不成立、因为取件 ref 还是因为 verdict 形态」。当前产出侧义务已双落点成文（`track-b-verifier-brief.md:33` 2026-09-20、`landing-verifier.md:34` 2026-10-19），本条只补「闸说得清哪一件、错在哪」。原型实测（261017 原形 verdict：`## 判定` + `**ok: true — PASS**`、无行首 verdict 行）：改前 `"frozen":true,"verdict":false,"covered":true`；改后 `"refUsed":"feat","frozen":true,"verdict":false,"covered":true,"verdictFormatMiss":true`——判定结果不变（该文件确实没有机器可读 verdict 行，BLOCK 正确），变的是归因可说。
3. 自检新增 2 条夹具（`FAIL → PASS` → pass；`PASS → FAIL` 重开 → block）；取件侧的两条夹具归 T1，不重复计。

替换后代码（逐字）：

```js
function lastVerdictLine(text) {
  let last = null;
  for (const line of text.split('\n')) if (VERDICT_STATUS_RE.test(line)) last = line;
  return last;
}
// …
    const vLine = vText ? lastVerdictLine(vText) : null;
```

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T3 · 非 lane 制品的适用面：ledger 变更集报 NOT_APPLICABLE，其余 unknown 仍拦

**问题**：闸的 kind 探测只有两条 lane（dev / gov）。跨面 intake spine（`delivery/intake/<changeId>.md`）与 debt 账本条目（`docs/debt/**`）的合并恒落 `kind=unknown` → 恒 BLOCK；而 BLOCK 的措辞「不得自动 merge、升级 human」与「人批豁免口」对 unknown 又是 FATAL——**人的批准落在真空里**。结果是一次「照章办事的主线程每次都会撞到无法清除的红灯」，再靠当场解释决定「这是不适用还是真拦」，两次不同会话可能给出不同行为。

**证据**（2026-10-26 起草期实跑，fixture 仓：main 一条 init commit、feat 只加 `delivery/intake/261026-cross-face-x.md`）：

```
R3 unknown status= 1
[pre-merge-escalation-gate] BLOCK (changeId=261026-cross-face-x, kind=unknown):
  - unknown change kind（changeId 既无 delivery/taskpackages 也无 delivery/review）
  → 升级 human，不得自动 merge。
R3 human-approval-ref status= 2 | stderr= [pre-merge-escalation-gate] FATAL: --human-approval-ref 仅适用于 dev-loop 档（changeId=261026-cross-face-x kind=unknown，无人批豁免概念）
```

补充实跑（2026-10-26，OQ1 裁定后重跑：要求「账本四域」逐一验证，不能只验 gov 域或 intake）：

```
R8 requirement-only（feat 只加 docs/debt/requirement/ARQ-…md）
  HEAD  : BLOCK (changeId=261026-debt-req, kind=unknown): unknown change kind… exit 1
  proto : NOT_APPLICABLE (changeId=261026-debt-req, kind=unknown): 变更集仅含 ledger 类制品（delivery/intake/、docs/debt/gov/、docs/debt/tech/、docs/debt/requirement/、docs/debt/contract/）——… exit 0
R9 contract-only（feat 只加 docs/debt/contract/CDQ-…md）
  HEAD  : BLOCK (changeId=261026-debt-con, kind=unknown): unknown change kind… exit 1
  proto : NOT_APPLICABLE (changeId=261026-debt-con, kind=unknown): 变更集仅含 ledger 类制品（同上）… exit 0
R10 retro-only（边界，feat 只加 delivery/retro/…md）
  HEAD  : BLOCK (changeId=261026-retro-only, kind=unknown): unknown change kind… exit 1
  proto : BLOCK（同形）——retro 不在账本枚举内，边界保持保守
```


源码落点（逐字）：`:441` 的 kind 三值枚举；`:416` 的 unknown 分支；`:765-767` 的 FATAL 分支。

**根因**：闸的**覆盖面**（哪些合并在它的判域内）从未成文，实现上表现为「kind 枚举 = 判域」。于是「不在判域」被编码成「判定为拦」，而闸自己的措辞又把「不适用」说成「不得自动 merge」——两件事对读者的行动指引完全相反。

**改法**（**OQ1 human 已裁（2026-10-26）**：取候选 2 形态，允许集按当前账本拓扑正推（四域 + intake）；原型已按此实现并实测）：

1. 新增账本类制品允许集 `LEDGER_ARTIFACT_PREFIXES`，**从当前账本拓扑正推枚举**：`docs/debt/{gov,tech,requirement,contract}/` 四域（锚 `repo-structure.md` 的「docs/debt/」行——四域文件夹 + 各自生成件 INDEX）加 `delivery/intake/`；`kind === 'unknown'` 时若变更集（`git diff --name-only <source>..main`）**非空且每一项**都落在该前缀集内 → 不 push 任何理由、置 `notApplicable: true`。**拓扑扩域时随 `repo-structure.md` 同批更新本常量**——不按历史 lane 假设硬编码（v0.1 曾用 `docs/debt/` 整前缀 + `delivery/retro/` + `delivery/archive/`，按 OQ1 裁定收窄）。 **该字段由 `buildCtx` 组装**（下述替换段 A）：`evaluate` 只消费 `ctx.diffPaths`；缺 A 时 `NOT_APPLICABLE` 永不触发（§7 R1-F1）。 **`notApplicable` 由 `evaluate` 序言的 `let notApplicable = false;` 声明**（下述替换段 C）：B 只赋值、`return` 只引用；ESM 严格模式下缺此声明，裸赋值与简写引用都会抛 `ReferenceError`（§7 R2-F4）。
2. CLI 分支：`!blocked && notApplicable` → 打一行 `NOT_APPLICABLE (changeId=…, kind=unknown): 变更集仅含 ledger 类制品（…）——本闸只管 dev/gov 两条 lane，此类合并按 merge-to-main skill 走 human 授权通道。`，`exit 0`。
3. 边界（刻意保守，写入代码注释）：允许集之外的任何路径（含固件、shim、`.claude/**`、`docs/contracts/**`、`delivery/retro/**`、`delivery/archive/**`、`docs/debt/README.md`——最后三处不在 human 裁的账本枚举内）都使 `onlyLedger` 不成立 → 仍 BLOCK；`delivery/workstreams/**` 亦不入集（多为伴随面，单列会扩大绕行面）。**通用 1（契约 diff 凭据）与 off-main 等检查独立于 kind 照常生效**——起草期实测：`kind=unknown` + ledger-only + 契约 diff → `blocked: true`（契约理由在场）。
4. 口径成文两处（承重文本，原文引用钉死 + 替换后完整句）：

`AGENTS.md:22` 原文起头逐字：`7. 主线程跑前置升级闸（含 off-main 校验），放行 → 自动合并本地 main（可回滚）；升级闸拦 → 升级人。`

替换后（原句逐字保留、其后追加一句；下列围栏内文本按字节落地，反引号随行）：

```markdown
7. 主线程跑前置升级闸（含 off-main 校验），放行 → 自动合并本地 main（可回滚）；升级闸拦 → 升级人。**适用面**：本闸管 dev（`delivery/taskpackages/<changeId>`）与 gov（`delivery/review/<changeId>`）两条 lane；变更集**全部**落在账本类制品（`delivery/intake/`、`docs/debt/gov/`、`docs/debt/tech/`、`docs/debt/requirement/`、`docs/debt/contract/`）时闸报 `NOT_APPLICABLE`（exit 0——不适用，不是放行），此类合并按 merge-to-main skill 的 human 授权通道执行；其余无法归 lane 的变更集仍 BLOCK 交人。
```

`.claude/skills/merge-to-main/SKILL.md`「前置」节第二条（原文逐字：`- 合并方向与授权明确：本 skill 只执行**已被批准的**本地合并（supervisor APPROVE / architect GREEN 或 human 批准后的本地合并动作面）；未批准的合并不做。`）之后追加一条（围栏内文本按字节落地）：

```markdown
- **适用面（261026 批）**：前置升级闸只管 dev / gov 两条 lane 的变更批；变更集全部落在账本类制品（`delivery/intake/`、`docs/debt/gov/`、`docs/debt/tech/`、`docs/debt/requirement/`、`docs/debt/contract/`）时闸报 `NOT_APPLICABLE`，走本 skill 的 human 授权通道即可——不需要人批豁免口，也不要把它读成「闸放行了」。
```

5. 自检新增 6 条夹具（判据 = human 裁的「四域各一条 + intake 一条 → 不适用；ledger + 固件 → 仍 BLOCK」）：`intake-only` / `requirement-only` / `contract-only` / `四域 + intake 合并` 四条 → 均 `notApplicable:true` 且不 blocked；`ledger + firmware` → blocked 且 `notApplicable:false`；`retro-only` → 仍 blocked（边界：retro / archive 不在账本枚举内）。反向实测：从允许集删 `docs/debt/requirement/` → 2 条红（`requirement-only` + `四域合并`）；关掉允许集判断 → 4 条红。

替换后代码（逐字；A = `buildCtx` 组装（`:447-449` 区段）与 ctx 字段，B = `evaluate` 的 unknown 分支（`:415-419` 区段），C = `evaluate` 序言里的 `notApplicable` 声明（`:333-334` 区段）。**A、B、C 缺一即 `NOT_APPLICABLE` 永不触发或直接抛 ReferenceError**，见 §7 R1-F1 / R2-F4）：

```js
// 允许集：从当前账本拓扑正推（.claude/rules/repo-structure.md「docs/debt/」行四域 + intake）
export const LEDGER_ARTIFACT_PREFIXES = ['delivery/intake/', 'docs/debt/gov/', 'docs/debt/tech/', 'docs/debt/requirement/', 'docs/debt/contract/'];

// ---- A. buildCtx 组装变更集（:447-449 区段；值为 ctx.diffPaths）----
  const contractsDiff = hasContractsDiff(execFn, source, 'main');
  const diffPaths = (() => { try { return execFn(`git diff --name-only ${source}..main`).split('\n').map((l) => l.trim()).filter(Boolean); } catch { return []; } })(); // 261026 批 T3：unknown 档 applicability 判据
  // 同函数 return 的 ctx 对象内补一行：
    diffPaths, // 261026 批 T3：变更集路径（unknown 档 applicability 判据）

// ---- C. evaluate 序言（:333-334 区段；notApplicable 必须先声明）----
export function evaluate(ctx) {
  const reasons = [];
  let notApplicable = false;


// …（off-main / 契约凭据 / blocked / contract-findings 等既有理由检查不变，略）…
// ---- B. evaluate 的 unknown 分支（:415-419 区段；只消费 ctx.diffPaths）----
  } else {
    // 261026 批 T3（ARQ-261017-merge-gate-scope-unstated，OQ1 裁候选 2）：变更集全部落在账本类制品 →
    // 本闸不适用（NOT_APPLICABLE），交 merge-to-main skill 的 human 授权通道；否则仍 BLOCK。
    const diffPaths = ctx.diffPaths || [];
    const onlyLedger = diffPaths.length > 0 && diffPaths.every((p) => LEDGER_ARTIFACT_PREFIXES.some((pre) => p.startsWith(pre)));
    if (onlyLedger) notApplicable = true;
    else push(`unknown change kind（changeId 既无 delivery/taskpackages 也无 delivery/review）`);
  }

  return { blocked: reasons.length > 0, reasons, notApplicable };
```

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）· `AGENTS.md`（改）· `.claude/skills/merge-to-main/SKILL.md`（改）

### T4 · 契约类 findings 改列位判据：只认 Findings 表 Kind 列

**问题**：`contractFindingsAccounted` 的行级过滤是「行含 `hard-violation` 且行含字面集任一字面」。review-packet 的常规样板句「**无** hard-violation finding——如出现将以 `docs/contracts/**` / … 字面前缀起引 Basis」同时含两样，于是**自述无 finding 的行被当成 finding**，拦合并、逼人批。已记录四例真实拦截（261001 / 261010 / 261013 三批），代价是每批至少一次无谓人门 + pod-events 噪声。

**证据**：

- 源码落点（逐字，`:195-198`）：

```js
  const hits = packetText
    .split('\n')
    .filter((l) => l.includes('hard-violation') && CONTRACT_SPEC_LITERALS.some((lit) => l.includes(lit)));
```

- 全量 census（起草期命令 + 输出；`delivery/taskpackages/**/review-packet.md`，89 份）：

```
旧判据命中 11 行 = 8 行真 finding 表行 + 3 行自述散文
散文 3 处：260831 之前的 261010-device-reconnect-on-server-close:47 / 261010-p3-storylist-cold-start-restore:60 / 261011-p1-home-v5-rework:51（均为「无 hard-violation finding——如出现将以 … 字面前缀起引」样板）
8 行真 finding 的 Kind 列实测：7 行 == 'hard-violation'，1 行 == '**hard-violation**（原判）'（261016-p3-story-body-sd-cache:71）
新判据（行首 `|` + Kind 列前缀匹配 + 字面在行内）实跑：hits=8，packetsWithHits=8
```

- 判定形态单源：`integrator.md:40` 已写死六列 `Severity | Kind | Area | Finding | Basis | Required action`，并写明「Kind=hard-violation 且 Basis 涉契约/spec 时 Basis 须以字面前缀起引」。

**根因**：判据锚在「行文本巧合」而不是「finding 的结构位」。写作侧与机械侧的契约是**列位**，判据却按自由文本匹配，于是任何复述、模板句、示例都被误判。

**改法**：

1. 判据改为：`trim` 后以 `|` 起始的**表行**、且第 3 个单元格（Kind 列）去 `*` 后**前缀**为 `hard-violation`、且该行仍含字面集任一字面（保留「契约相关」这一层收缩，同时保住历史行）。
2. 写作侧同批收紧（③）：`integrator.md:40` 增一句「非 finding 行不得复述守门字面集」？——**本批不动 `integrator.md`**（当前非 finding 行已被列位判据自动免疫，再加写作禁令属冗余约束）；写作约定侧只留现状，理由记 §6 留根。
3. 自检新增 3 条夹具：散文行（非表行）→ 不命中；「核销说明」行（Kind 列非 hard-violation）→ 不命中；真 finding 行 → 命中 1 条。

替换后代码（逐字）：

```js
  const hits = packetText
    .split('\n')
    .filter((l) => {
      const t = l.trim();
      if (!t.startsWith('|')) return false;
      const kind = String(t.split('|').map((s) => s.trim())[2] || '').replace(/\*/g, '').toLowerCase();
      return kind.startsWith('hard-violation') && CONTRACT_SPEC_LITERALS.some((lit) => t.includes(lit));
    });
```

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T5 · 生成件与派生态相撞：INDEX 走 union + 重渲染，current/ 投影走闸侧 ②

**问题**：三条 lane 的 `INDEX.md` 是渲染器（`changes-index.mjs`）生成件，`current/` 两件是「最后写者赢」派生态。双 Pod 近距合并时两侧各自追加/改写 → merge-tree 实撞 → 闸报 `parallel-overlap-tier3`（交 architect 消解评审），而实质是**零语义冲突**：INDEX 重跑渲染即归位，投影由 merge-driver 既有特判消解。每撞一次烧一次人工消解评审。

**证据**（2026-10-26 起草期，三份 fixture 实跑）：

- 无属性 fixture（两分支各自尾追加 `delivery/review/INDEX.md` 一行；cwd = 源分支工作树）→ `BLOCK (changeId=261026-fx-p, kind=devloop): parallel-overlap-tier3：…实撞 1 件（delivery/review/INDEX.md）`。
- 加一行 `delivery/review/INDEX.md merge=union` 的同构 fixture → `PASS (changeId=261026-fx-p, kind=devloop) — 无例外命中`；`git merge-tree --write-tree feat main` 输出首行仅树 oid、无冲突清单；结果树内 INDEX 内容 = 两行都在（`a / c / b`，次序由重渲染归位）。
- 投影 fixture（两分支各自改 `delivery/taskpackages/current/taskpackage.json`）→ `BLOCK … parallel-overlap-tier3：…实撞 1 件（delivery/taskpackages/current/taskpackage.json）`；补丁后原型 → `PASS`。
- 现状缺口：`.gitattributes` 现有 union 仅四处（`:12` progress.md、`:19` pod-events.jsonl、`:28` 与 `:29` 的 debt INDEX 两条）；`delivery/review/` / `delivery/taskpackages/` / `delivery/intake/` 三份 lane INDEX **无** union。闸的分级判据 `:310-321` 的 carve-out（`:318`）只认 `.vN.md` 形态，不含派生态（`merge-driver.mjs:943-946` 的 `PROJECTION_CONFLICT_FILES` 两件）。

**根因**：合并语义层（git 属性 / 闸分级）不认「这份文件的权威不是它的内容，而是生成它的程序」。debt 账本 INDEX 已用「union + 渲染自愈 + doctor 报待重渲染」解决过一次（`260923-gov-review-arq-ssot-restructure` T2 件 2/3），本批把同一口径推广到三条 lane INDEX，并把派生态并入既有 ② 级。

**改法**：

1. `.gitattributes` 追加三条（紧随现有 debt INDEX 段之后，附一行理由注释）：

```gitattributes
# 三条 lane INDEX 生成件双追加（261026-gov-review-merge-gate-cluster-fix T5，ARQ-261014 ③）：
# delivery/{review,taskpackages,intake}/INDEX.md 由 changes-index.mjs 幂等全量重建，双分支尾追加是结构性
# 双追加；union 双侧保留 + 合并后重跑渲染即归位（同 docs/debt/*/INDEX.md 先例）。
delivery/review/INDEX.md merge=union
delivery/taskpackages/INDEX.md merge=union
delivery/intake/INDEX.md merge=union
```

2. 闸侧：新增 `DERIVED_STATE_CARVE_OUT`（恰 = merge-driver 的两个投影路径，注释标「与 merge-driver.mjs:943-946 同形、lib 收编留根」，对齐 `VERSIONED_ARTIFACT_RE_LOCAL` 既有先例注记），并入 `:318` 的 carve-out 条件：

```js
export const DERIVED_STATE_CARVE_OUT = new Set([
  'delivery/taskpackages/current/taskpackage.json',
  'delivery/taskpackages/current/verdict.json',
]);
// …
      if (conflictedFiles.every((f) => VERSIONED_ARTIFACT_RE_LOCAL.test(f) || DERIVED_STATE_CARVE_OUT.has(f))) return 'union-safe';
```

3. 分级口径成文（`pod-coordination.md` §10d「取舍机判判据」末尾追加，原文末句逐字 `判不了 = 存疑向严升 ③。`）：

> 追加句（承重文本）：`**派生态与生成件相撞恒归 ②**：current/ 投影（两件，merge-driver 的 PROJECTION_CONFLICT_FILES）与三条 lane 的 INDEX.md（渲染器 changes-index.mjs 全量重建为权威）无论哪侧先并都按机械消解——生成件由 union 免冲突 + 重跑渲染归位、投影由 merge-driver 特判（被合并包赢）；不升级 human 消解评审（261026 批 T5）。`

4. 合并后必做动作成文（`merge-to-main` skill）：

- 步骤 2（`SKILL.md:18`）括号串末尾追加。原文逐字：

```markdown
2. **预览 overlap** — `git diff --stat <target>...<source>`。终点：列出改动文件；**标记双侧都改的文件**（INDEX 已按 lane 分文件——`delivery/review/INDEX.md` 与 `delivery/taskpackages/INDEX.md` 各自只在自己的 lane 合并流里变；debt 账本 `docs/debt/*/INDEX.md` 已配 `merge=union` + 渲染自愈——合并后重跑 `node .claude/scripts/debt-render.mjs` 即归位，doctor 会报「待重渲染」；旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕）——这些是冲突高发点，先看清再动。
```

追加后完整行（围栏内按字节落地；插入点在原文「旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕」之后、右括号之前）：

```markdown
2. **预览 overlap** — `git diff --stat <target>...<source>`。终点：列出改动文件；**标记双侧都改的文件**（INDEX 已按 lane 分文件——`delivery/review/INDEX.md` 与 `delivery/taskpackages/INDEX.md` 各自只在自己的 lane 合并流里变；debt 账本 `docs/debt/*/INDEX.md` 已配 `merge=union` + 渲染自愈——合并后重跑 `node .claude/scripts/debt-render.mjs` 即归位，doctor 会报「待重渲染」；旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕；三条 lane INDEX 已配 merge=union——合并后重跑 `node .claude/scripts/changes-index.mjs` 全量重建即归位（幂等））——这些是冲突高发点，先看清再动。
```
- 步骤 8 的 `--upsert` 行（原文逐字 `node .claude/scripts/changes-index.mjs --upsert <本批 changeId>   # 逐个`）之后追加一行：`node .claude/scripts/changes-index.mjs   # 全量重建（union 合并后行序/计数归位；幂等）`。

5. 自检新增 2 条夹具，**函数级**（直接调 `parallelOverlapProbe` 并注入 execFn，不手注 `overlap.tier`）：投影派生态冲突 → `tier: union-safe`；投影 + 其他文件混撞 → `tier: tradeoff`。

   > 起草期自曝（值得留痕）：「手注 `tier` 的 evaluate 夹具」在删掉 carve-out 后**仍全绿**——这类夹具只测了 `evaluate` 的分支消费、没测探测器的产出，属于无判别力的假绿。已改为函数级夹具并实测：删掉 carve-out → `overlap-projection-derived-state-pass` 红。

**为什么不只在闸里加白名单**：只改闸，`parallel-overlap-tier3` 是不报了，但 rebase 阶段 merge-driver 仍会在 INDEX 上实撞（`tryResolveProjectionConflicts` 只认两个投影路径），从「闸前升级人」变成「rebase 后 blocked」——把摩擦挪了个位置。union 属性在 git 层直接免冲突，才是根上。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）· `.gitattributes`（改）· `.claude/references/pod-coordination.md`（改）· `.claude/skills/merge-to-main/SKILL.md`（改）

### T6 · enforce-scope 跨树锚定：按 gitdir 拓扑回推 commonDir，夹具换成真实拓扑

**问题**：worktree 的 `.git` 文件内容是 `gitdir: <commonDir>/worktrees/<name>`——指向一个**目录**（该目录内另有 `gitdir` 文件回指 worktree 的 `.git`）。钩子 `locateGitTree` 的注释与实现假定它指向「`<commonDir>/worktrees/<name>/gitdir` 这个**文件**」，用 `dirname^3` 回推 commonDir，于是 worktree 分支多剥一层：commonDir 落到仓根，而主 checkout 分支算出的是 `<repo>/.git` → `samePath` 恒不等 → 静默降级 legacy → 目标路径相对误算成 `../<sibling>/…` → 同仓合法写被判定越界。

**证据**（2026-10-26 起草期）：

- 源码落点（逐字，`.claude/hooks/enforce-scope.mjs:141-146`）：

```js
      const m = /^gitdir:\s*(.+)\s*$/.exec(readFileSync(dotGit, 'utf8'));
      if (m) {
        const gitdir = resolve(cur, m[1].trim()); // <commonDir>/worktrees/<name>/gitdir
        // commonDir = gitdir 上三级（…/worktrees/<name>/gitdir → …/worktrees/<name> → …/worktrees → <commonDir>）
        return { treeRoot: cur, commonDir: dirname(dirname(dirname(gitdir))) };
      }
```

- 真实树实跑（本 gov worktree，2026-10-26）：

```
A_siblingWriteFromGovBuilder（原钩子）: {"status":2,"stderr":"scope: agent \"builder\" 只能写 firmware/**, server/**, tools/**, delivery/workstreams/**；../KidPal/firmware/probe.c 超出边界。"}
B_govVerdictWriteFromMainVerifier（原钩子）: {"status":2,"stderr":"…；../KidPal-gov/delivery/review/261019-gov-review-local-memory-extraction/landing-verdict.md 超出边界。"}
```

  第二条与 2026-10-19 landing-verifier 被误拦的事故逐字同形（见 `ARQ-261018` 追注）。
- 夹具无判别力（逐字，`:277-279`）：先取 `const gitdirFile = join(repo, '.git', 'worktrees', 'pod-x', 'gitdir');`，再把 pod-x 的 `.git` 内容写成「`gitdir: ` + 该文件路径」——**按文件拓扑构造**，故 `--self-check` 6 臂全绿（起草期实跑 `ok: true`）却测不到真实拓扑。
- 判别对照：把同一真实拓扑夹具分别喂给原钩子与补丁原型 → 原钩子 `exit 2`（`../trees/pod-x/firmware/x.c 超出边界`）、原型 `exit 0`。

**根因**：实现按「单一拓扑」写死；`git worktree add` 的实际产物是另一种拓扑。夹具复刻了实现的错误假定，于是自检成了「实现照镜子」。

**改法**：

1. `locateGitTree` 改为**拓扑感知**：解析出 gitdir 后，若该路径是目录 → 上两级即 commonDir；若是文件（旧拓扑，或按文件回退）→ 先升到其父目录再上两级。两分支统一到同一层级。

```js
        const gitdir = resolve(cur, m[1].trim());
        // 真实拓扑（git worktree add）：gitdir 指向 <commonDir>/worktrees/<name> **目录**；
        // 兼容旧拓扑：gitdir 指向该目录内的 gitdir 文件。统一先归到 worktrees/<name>，再上两级 = <commonDir>
        // （261026 批 T6，ARQ-261018：原 dirname^3 多剥一层）。
        let wt = gitdir;
        try {
          if (!statSync(wt).isDirectory()) wt = dirname(wt);
        } catch {
          wt = dirname(wt); // 目标不存在时按文件形态回退（等价原语义）
        }
        return { treeRoot: cur, commonDir: dirname(dirname(wt)) };
```

2. 夹具改为**双拓扑**：pod-x 用真实拓扑（`.git` → `worktrees/pod-x` 目录，目录内 `gitdir` 文件回指），pod-y 保留旧式文件拓扑（兼容臂）；新增臂 `fixture-real-topology`（断言 pod-x 的 gitdir 是目录、其内 `gitdir` 是文件——自检自身先断夹具形状，防再次照镜子）与 `sibling-legacy-file-topology-allowed`。
3. 判别力反向验证：把原型夹具的 pod-x 换回文件拓扑时新臂即失去意义；而「原钩子 + 真实拓扑夹具 → exit 2」已在起草期实测（见上）。

**不做的边界（留根）**：不改「非本仓路径」判定与 fail-closed 语义；不动 Bash 分支与 per-agent allow/deny；`.git` 指向 `<superproject>/.git/modules/<name>` 的子模块拓扑本仓不使用，不在本批覆盖（§6 留根）。

目标文件：`.claude/hooks/enforce-scope.mjs`（改）

### T7 · merge-driver 两热修的防回归断言 + 注释追认

**问题**：两处热修（`ReferenceError: target is not defined` 的声明作用域；0.10.8 detect-changes prose 输出形态失配）已在树，但**零防回归断言**，且代码注释仍写「gov 侧待追认」。同类崩溃每复发一次就是整批 merge 链卡死（pod 卡 merging 态）。

**证据**：

- 两热修在树（逐字）：`:172-183` 的 0.10.8 prose 形态降级（含 `warn('detect_changes cli: success output not JSON-parseable (mcp 0.10.x prose shape) — degrade to empty enrichment (escalation 9b1181bc)')`）；`:1423-1427` 的 `const target = resolveRebaseTargetLive(wt);` 声明上提（注释逐字「Hot-fix（human 侧主线程落笔 2026-09-30，gate-tooling-bug escalation a8a3b44c）」）。
- 断言空档（起草期实跑）：`--self-check-detect-changes` 16 例（`pass:16,fail:0`）全部经 `runCli` 注入，**从不触达** `defaultDetectChangesCli` 的解析层；`--self-check-rebase-target` 7 例（`pass:7,fail:0`）测解析函数本身，不覆盖声明位置。
- 「待追认」注释两处（逐字）：`:173` 「…先例 0303e98 / 85667b2a，gov 侧待追认）：codebase-memory-mcp 0.10.8」；`:1426` 「（85667b2a 热修先例同形态，gov 侧待追认入档）。」

**根因**：热修在压力下由人直接落笔、走的是「先止血」通道；止血后没人回头补断言，而现有自检的注入面恰好绕过了故障层（解析器 / 声明位置）。

**改法**：

1. 把 0.10.8 解析抽成**纯函数** `normalizeDetectChangesOutput(outer, warnFn)`（导出），`defaultDetectChangesCli` 退化为「spawn + 解析 stdout + 调纯函数」；新增入口 `--self-check-merge-tooling-hotfixes`，8 条 case：

   - `structuredContent` 直通（`changed_count` 保留）；
   - `content[0].text` 为 JSON 串 → 解析成功；
   - prose 形态（`isError:false`、text 非 JSON）→ `result.filePaths` 为空数组 **且** warn 含 `prose shape`（反向：fail-closed 版本会红）；
   - `isError:true` → `{isError}`；
   - `outer` 为 null → `{result:null}`；
   - 声明位置静态不变量：源码里存在**两空格缩进**的 `const target = resolveRebaseTargetLive(wt);` 行（若被挪回 `if (!DRY_RUN)` 块内会变四空格 → 红；兼容 CRLF 行尾）；
   - 夹具非空先行（真空绿规约）；
   - 自检失败必须 **exit 1**（起草期原型一度写成恒 exit 0，属「自检不能失败」的假绿形态，已修正；落地实现须同样 propagate 退出码）。

2. 两处注释改为追认：`:173` 的 `gov 侧待追认` → `追认入档 = ARQ-261014，随 261026-gov-review-merge-gate-cluster-fix 结项`；`:1426` 的 `gov 侧待追认入档` → `追认入档 = ARQ-261014，随 261026 批结项`。
3. **承接 `ARQ-261014` ④ 残余（Windows UUID 一例）**：本仓 `merge-driver.mjs:35` 已 `import { randomUUID } from 'node:crypto'`、`:1062` 的 `eventId: randomUUID()` 即正统做法；在该行上方加一行注释固化这条教育点（**勿从 `/proc` 读 UUID**，Windows 下不可用），使该残余随 261014 结项后仍活在代码里（§7 R1-F2 的最小承接路线）。

```js
    const event = {
      schemaVersion: 1,
      // 261026 批 T7（ARQ-261014 ④ 残余承接）：Windows 上 /proc 不可用，事件 id 一律用 node crypto.randomUUID()，勿从 /proc 读 UUID。
      eventId: randomUUID(),
```

替换后代码（逐字，T7 第 1 条：`defaultDetectChangesCli` 尾部 + 新纯函数）：

```js
  let outer;
  try { outer = JSON.parse(res.stdout); } catch { return { result: null }; }
  return normalizeDetectChangesOutput(outer, warn);
}

// 261026 批 T7（ARQ-261014 ②）：0.10.8 prose-shape 降级解析抽为纯函数（可 self-check，防回归）。
export function normalizeDetectChangesOutput(outer, warnFn = warn) {
  if (!outer || typeof outer !== 'object') return { result: null };
  if (outer.isError) return { isError: 'cli reported isError' };
  let result = outer.structuredContent;
  if (!result && outer.content && outer.content[0] && outer.content[0].text) {
    try { result = JSON.parse(outer.content[0].text); } catch { result = null; }
  }
  // Hot-fix #2（escalation 9b1181bc）：CLI 真成功但输出为纯文本 prose → 空 enrichment + warn，不 fail-closed。
  if (!result && outer.isError === false) {
    warnFn('detect_changes cli: success output not JSON-parseable (mcp 0.10.x prose shape) — degrade to empty enrichment (escalation 9b1181bc)');
    result = { filePaths: [] };
  }
  return { result: result || null };
}
```

**起草期原型实测**：`--self-check-merge-tooling-hotfixes` → `{"pass":8,"fail":0,"total":8,"ok":true}`，exit 0；把声明改成四空格（模拟回退）→ `{"pass":7,"fail":1,…}`，**exit 1**（判别力成立）。既有自检在原型上复跑：retro-advisory 3/3、detect-changes 16/16、rebase-target 7/7、worktree-topology 9/9、merge-friction 23/23（`--self-check-quick-checks` 因原型副本缺 `verify-gates.json` 不可跑，落地后须在仓内复跑）。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T8 · 闸自身假阴性的兜底通道成文（不是「首合并常态」）

**问题**：261017 / 261021 两批的实际处置都是「human 当场授权 + 主线程手工 `git -C <main-worktree> merge --ff-only`」，但这条路径**未成文**：`pod-coordination.md` §10e 写的是「无人合通道」，与实况相反。未成文的后果是同一种局面两次会话可能两种做法（一次直接合、一次无谓升级）。

**证据**：`pod-coordination.md:209` 逐字含「——**无人合通道、无人类签字门**」；闸 `:765-767` 对 gov / unknown 档的 `--human-approval-ref` 直接 FATAL（T1 复证已录）。

**根因**：261015 批撤销人签门时写的是「目标态」，而机械凭据对首合并不可达（T1/T2 的两根）使**实现态**长期退化为人合。叙事与机器现实脱节，兜底路径只能口头传递。

**改法**：T1/T2 修好后，首合并的机械路径可达，叙事回到真值；同时把「闸自身缺陷时的兜底」写成**受限通道**（不是常态路径），追加在 `pod-coordination.md` §10e 原段之后（原文末句逐字 `…该条目已于 2026-09-24 结项归档）。`）：

追加段（围栏内文本按字节落地）：

```markdown
**闸自身假阴性时的兜底通道（261026 批 T8）**：机械凭据闸是契约批默认且唯一的自动放行路径。若闸自身出缺陷（实例：261017 取件 ref、261021 首匹配锚——三件凭据齐备仍 BLOCK），处置固定四步：① 按实况逐件核对三件凭据（PROPOSAL 状态行 / landing-verdict 行首 verdict 行 / landing-manifest 覆盖全部契约 diff 路径）；② 在 `docs/debt/gov/` 立 ARQ 条目，逐字记 BLOCK 输出与凭据实况；③ 由 human 显式授权；④ 主线程在 main worktree 执行 `git -C <main-worktree> merge --ff-only <source>`，并在 pod-events 记人批事件。开放条件仅限「凭据齐备而闸报缺凭据」的假阴性——凭据真缺、off-main、契约 diff 无同批批证据一律不得走本通道。首合并的机械可达性已由本批 T1/T2 修复兑现；再走兜底即等于承认一次未修完的闸缺陷，须与 ARQ 一并处理。
```

目标文件：`.claude/references/pod-coordination.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 合并链闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 | 消费方 = `merge-driver.mjs` Step 1b（spawn 同 invocation）、planner / portfolio-planner / build-feature 的合并门（`--source-ref HEAD`）、主线程治理批合并、`arch-review-protocol.md` 治理路由 step 7。全部经 CLI 退出码消费；本批判据为收敛式改动（T1/T2 向准、T3 收窄判域、T4 收窄命中、T5 扩 ② 级） |
| 写边界钩子 | `.claude/hooks/enforce-scope.mjs` | G3 | 注册于 `.claude/settings.json`、`.claude/dev-loop.settings.json`、`.claude/review-loop.settings.json` 三处 PreToolUse；消费方 = 全部会话的 Edit/Write/Bash 写入判定 |
| 合并执行器 | `.claude/scripts/merge-driver.mjs` | G3 | 消费方 = planner 主线程（`--pod`）、6 条既有自检入口 |
| 合并语义配置 | `.gitattributes` | G3 邻接（合并语义） | 新增三条 union；与既有 `*.sh eol=lf`、progress / pod-events / debt INDEX 段无路径交集 |
| 指令根 | `AGENTS.md` | G1 | 治理路由 step 7 追加适用面句（T3） |
| 流程定义 | `.claude/skills/merge-to-main/SKILL.md` | G2 | 前置节适用面句 + 步骤 2 / 8 重渲染句（T3/T5） |
| 流程定义 | `.claude/references/pod-coordination.md` | G2 | §10d 取舍判据追加派生态句（T5）+ §10e 兜底通道段（T8） |
| 契约面 | `docs/contracts/**` | 未触 | 本批不改契约；T1/T2 只改凭据取件与锚点解析 |
| agent 定义 | `.claude/agents/**` | 未触 | 无 frontmatter / 职责变更（OQ3 若取 (b) 才追加 planner 一处文面，另行定稿） |
| spec / 需求面 | `docs/requirements.md` / `docs/architecture.md` | 未触 | — |

**【硬项 · sweep 透镜义务】新判据全仓预跑（起草期 2026-10-26，命令 + 结果）**：

- T4 新判据（Findings 表 Kind 列）对 89 份 review-packet 预跑：旧 11 行 → 新 8 行；8 行真 finding 逐包在册（`260829-hardware-validation` / `260829-schedule-reminder` / `260831-cloud-link-bringup` / `260907-g-relay-fix` / `260924-perf-k1-roundtrip-latency` / `261010-prompt-v9-schedule-anti-narration` / `261016-p3-story-body-sd-cache` / `261022-diag-log-content`，各 1 行）；被剔除的 3 行全部是自述散文（T4 证据逐条点名）。形态完备性：命中集按「表行 + Kind=hard-violation 前缀 + 字面在行内」三条件分类，8 + 3 已是当前全部 11 条的穷举，无第四类。
- T5 派生态集：`delivery/taskpackages/current/` 现 3 件（`taskpackage.json` / `verdict.json` / `input-brief.md`）；前两件与 merge-driver 特判集逐字相同，`input-brief.md` **刻意不入** carve-out（driver 不可解，入闸会让「闸放行、driver blocked」错位；见 §6 留根）。
- T3 允许集现状（`find <dir> -type f | wc -l`，2026-10-26）：`docs/debt/gov/` 130、`docs/debt/tech/` 62、`docs/debt/requirement/` 5、`docs/debt/contract/` 17、`delivery/intake/` 19。五类都是账本 / 路由记录面，不含实现面或 canonical 规则。**不在集内**（按 OQ1 裁定）：`delivery/retro/` 5、`delivery/archive/` 295、`docs/debt/README.md`（域指路件）；纯此类变更集仍 BLOCK，边界实测见 V4b。
- 闸调用点 census（`grep -rn pre-merge-escalation-gate`，排除自检与脚本自身）：planner / portfolio-planner / integrator / build-feature / arch-change / arch-review-protocol / architect-operating-model / dev-loop / pod-coordination / contracts rule / contract-change-evidence / architect 定义 共 12 处文档引用 + 1 处代码调用（merge-driver）。**无一处需随本批改口径**——T3 的适用面语义只影响「不适用」分支的输出文案，调用方一律按退出码 0/1 分流：`NOT_APPLICABLE` 走 0，对调用方等价于放行（区别只在闸自己的文案与 exit 语义留痕）。

**【硬项 · 复扫模式完备性先验】**：本批无退役语面清扫（无重命名、无目录搬家）。唯一「模式集」类改动 = T4 的 Kind 列判据与 T5 的路径集，均已在上表逐形态枚举（表行 / 非表行、七种 Kind 书写变体、两个投影路径 + 一个刻意排除路径）。

## 4. human 取舍点（OQ）

**OQ1 · 非 lane 制品的处置形态（`ARQ-261017-merge-gate-scope-unstated` 的真设计分叉）**

- (a) **闸增第三 kind（`ledger`）**：把 intake/debt/retro/archive 识别为独立类，另定其判定（放行 / 需人授权 / 不适用）。代价 = 新增一个 kind 值 + 一份判定语义 + 调用方文案分流；收益 = 报告里能看出「闸认识你，判过你」。
- (b) **unknown 收窄为 NOT_APPLICABLE（带路径允许集）**：本批原型所取形态——变更集全部落在 ledger 前缀集时报 `NOT_APPLICABLE`（exit 0），其余 unknown 仍 BLOCK。代价 = 允许集是白名单，将来新制品面要登记；收益 = 不动 kind 枚举、默认仍 fail-closed。
- (c) 保持现状 + 只补文面（不推荐：会让「照章办事每次撞红灯」原样保留）。
- **推荐：(b)**。理由：闸要回答的是「这次合并有没有例外」，不是「这是什么 lane」；允许集把「不适用」的证据钉在**变更集内容**上（可机判、可审计），而新增 kind 只是把同一判断换个标签，却引入第三个枚举值要维护。
- **human 已裁（2026-10-26）**：原则「合并闸要跟现在的账本结构一致，不能跟历史硬套」→ 取 (b) 形态；允许集须从账本拓扑正推枚举（`docs/debt/{gov,tech,requirement,contract}/` 四域 + `delivery/intake/`），并在 PROPOSAL 写明「锚 repo-structure.md 账本拓扑、扩域时随其更新」；不接受按历史 lane 硬编码。T3 已据此收窄、补四域夹具并重跑。
- **影响面提醒**：若取 (a)，T3 的代码段与 §5 的预期输出要重写；若取 (c)，T3 的代码段删除、只留文面两处。

**OQ2 · 首合并契约批：修门 vs 改叙事（`ARQ-261021` 方向 ③）**

- (a) **只修门**（T1+T2）：机械凭据对首合并可达，叙事不动——`/arch-change` 与 §10e 的「无人签门、合并过机械凭据闸」回到真值。
- (b) **修门 + 兜底通道成文**（本批建议）：在 (a) 之上加 T8，把「闸自身假阴性时」的四步受限通道写进 §10e。
- (c) **只改叙事**（承认首合并走人批链常态）：与 261015 批裁定方向相反，且会让「机器可放行」永久纸面化。
- **推荐：(b)**。理由：①「首合并走人批链」不是设计选择，是缺陷的症状（三件凭据齐备却不可达），写成常态等于固化假阴性；② 但闸**确实**会出缺陷（本批六条里有三条就是闸自身缺陷），没有成文兜底就会继续靠口头解释；③ 兜底写成受限通道（四步、开放条件仅限假阴性），不构成绕过门。
- **human 已裁（2026-10-26）**：取 (b)，T1/T2 修门 + T8 兜底通道成文。
- **代价对照**：只做 (a) 的代价 = 下次闸缺陷仍临时解释；(c) 的代价 = 契约批合并从此无机械放行，261015 裁定失效。

**OQ3 · `ARQ-261014` 附带教育面 ④（final 相位派发契约）是否并入本批**

- (a) **拆出独立批**（本批建议）：④ 已长出三份契约（谁跑 final / scope-file 谁产 / round 序号怎么算），2026-10-25 retro 记八包复现、每包至少烧半轮到一轮；落点是 dev-loop 派发面（planner / launch-prep），与合并闸机器面不同根。拆出后由主线程在 `docs/debt/gov/` 登记新条目（登记动作归主线程）。
- (b) **本批落最小三句**：在 planner 的 final 相位派发段加三句（谁跑、scope-file 归属、round 序号 = next completed ordinal），其余细节留后续。代价 = 本批追加一个 G2 目标文件、与「机器面簇」主题混装；收益 = 止血更快。
- (c) 本批不动也不拆（不推荐：该条 ④ 会长期悬空）。
- **推荐：(a)**。理由：本批已含 7 个目标文件、3 处机器判据，再加 dev-loop 派发契约会让评审面失焦；④ 的证据强度（跨 8 包）足以支撑单独成批，独立批还能顺带把 retro 里同主题的 scope-file / ordinal 形态一次收口。
- **human 已裁（2026-10-26）**：取 (a) 拆出独立批；由主线程另行登记 governance debt 条目防忘（登记归主线程）——已登记为 `ARQ-261026-final-phase-dispatch-contract-unwritten`（2026-10-26），本批留根指向该条。

**OQ4 · verdict 产出侧格式校验的落点（`ARQ-261017` 建议 4）**

- **复证结论先行**：候选载体的 (a) verifier 简报版 与 (b) 产出模板版 **都已存在**——`track-b-verifier-brief.md:33` 自 2026-09-20（`260920-gov-review-verdict-anchor-align` T1）起就写死「必须含一行行首 `verdict: <TOKEN>`」；`landing-verifier.md:34` 自 2026-10-19 起在 agent 定义里再写一遍。261017 那次是**产出侧一次性未遵守**，事后已由 verifier 补行（该 verdict 现含 `verdict: PASS + 6 ADVISORY`）。
- 因此三选一实际只剩一项有增量：**(c) 闸内「命中 0 行」点名报错**（本批 T2 第 2 条已含）。若取 (a)/(b) = 再抄一遍已有义务（双源漂移风险），不推荐。
- **推荐：(c)**（并入 T2，不单独占目标文件）。若 human 认为仍要「模板化」，请指明落在哪一份载体、以什么形态；那需要新增 §2 锚行、FROZEN 前重定稿。
- **human 已裁（2026-10-26）**：取 (c)，闸内点名，已并入 T2，无新增目标文件。

**另两点非 OQ 但提请评审注意（技术取舍，已给理由，judge 可 challenge）**：

1. T4 偏离 ARQ 倾向（ARQ 倾向 ①否定判别 + ③；本批取 ②列位 + ③），理由见复证 5。
2. T5 对 INDEX 采用「`.gitattributes` union」而非「闸侧白名单」，理由见 T5 末段。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律**：每条 = 命令 + 预期 + 评估时点。**预期值一律来自起草期实跑**——基线跑在 HEAD（2026-10-26），改法预期跑在起草期原型（`c:\tmp\mgfx\proto\{gate.mjs,hook.mjs,merge-driver.mjs}`，**非落地件**；落地实现若夹具条数或命令名不同，须在评审记录里 append 实际值再改预期）。无实跑支撑的项显式标「未实测」。
> **时点定义**：pre-commit = 落地文件在盘、尚未 commit 即可跑（本批绝大多数验证属此类）；post-commit = 须落地提交后才成立。双时点项逐子句标注。
> **真空绿规约**：新增自检 case 逐条配反向例（把实现改回旧形态必红），反向实跑已在 V1 / V10 备注里给出。

**基线（HEAD，pre-commit，起草期实跑，供落差对照）**：

| # | 命令 | 实跑输出（逐字） |
|---|---|---|
| V0 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | `{"selfCheck":"pre-merge-escalation-gate","pass":51,"fail":0,"total":51,"ok":true}`，exit 0 |
| V0b | `node .claude/hooks/enforce-scope.mjs --self-check` | `"ok": true`，6 臂（含 `sibling-worktree-workfile-allowed`），exit 0——**注意：此绿不具判别力**（夹具按错误拓扑构造） |
| V0c | `node .claude/scripts/merge-driver.mjs --self-check-{retro-advisory,detect-changes,rebase-target,worktree-topology,merge-friction}` | 依次 `3/3`、`16/16`、`7/7`、`9/9`、`23/23`，全 `ok:true` |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑值） | 起草期实测留痕 |
|---|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | `ok:true`、`fail:0`、`total = 51 + 新增夹具数`；本批新增 15（T1 2 / T2 2 / T3 6 / T4 3 / T5 2）→ **66** | 原型：`{"pass":66,"fail":0,"total":66,"ok":true}`，exit 0。**反向（七条各自实测，把对应实现改回旧形态）**：T1 target-only → 65/1（`contract-evidence-source-first-pass`）；T2 首匹配 → 64/2（两条 verdict 轨迹）；T3a 允许集删 `docs/debt/requirement/` → 64/2（`requirement-only` 与四域合并两条）；T3b 关掉允许集判断 → 62/4（四条正向全红）；T4 回行级过滤 → 64/2（散文与非 finding 行）；T5 去掉派生态 carve-out → 65/1（`overlap-projection-derived-state-pass`）；F4 删 `let notApplicable = false;` → 自检在 `return { …, notApplicable }` 处 `ReferenceError: notApplicable is not defined`、exit 1。七条反向均非绿 |
| V2 | pre-commit | 契约凭据 fixture（recipe 见 §2 T1 证据：main init + feat 加契约文件/PROPOSAL/verdict）→ `node .claude/scripts/pre-merge-escalation-gate.mjs 261026-arch-review-fx --source-ref feat`（cwd = fixture 仓） | `PASS (changeId=261026-arch-review-fx, kind=gov)`，exit 0 | 改前 BLOCK（`frozen:false`）→ 原型 PASS（§2 T1 逐字） |
| V3 | pre-commit | 同 V2 的轨迹变体（verdict `FAIL → PASS`） | `PASS (changeId=261026-arch-review-fy, kind=gov)` | 改前 BLOCK（`verdict:false`）→ 原型 PASS |
| V4 | pre-commit | 同 V2 的 intake-only 变体（feat 只加 `delivery/intake/261026-cross-face-x.md`） | `NOT_APPLICABLE (changeId=261026-cross-face-x, kind=unknown): 变更集仅含 ledger 类制品（delivery/intake/、docs/debt/gov/、docs/debt/tech/、docs/debt/requirement/、docs/debt/contract/）；…` exit 0 | 改前 BLOCK + `--human-approval-ref` FATAL exit 2（§2 T3 逐字）；原型输出逐字见 §2 T3 |
| V4b | pre-commit | 账本域矩阵（三份 fixture：只加 `docs/debt/requirement/…`；只加 `docs/debt/contract/…`；只加 `delivery/retro/…`） | requirement-only、contract-only → `NOT_APPLICABLE` exit 0；retro-only → 仍 `BLOCK … unknown change kind` exit 1（边界） | HEAD：三者均 BLOCK；原型：前两者 exit 0、retro 仍 exit 1（§2 T3 补充实跑逐字） |
| V5 | pre-commit | `node --input-type=module -e` 调 `contractFindingsAccounted` 遍历 `delivery/taskpackages/**/review-packet.md`（89 份） | `{"packets":89,"hits":8}`；8 包名单 = §3 第一节所列 | 旧判据同遍历 = `hits:11`（8 真 + 3 散文）；原型实测 8/8 |
| V6 | pre-commit（属性可见）+ post-commit（合并行为生效） | `git check-attr merge -- delivery/review/INDEX.md delivery/taskpackages/INDEX.md delivery/intake/INDEX.md` | 三行均 `merge: union` | 现状 `git check-attr` 对这三条输出 `merge: unspecified`；原型 fixture 对比：无属性 `BLOCK … parallel-overlap-tier3`、有属性 `PASS`（§2 T5 逐字） |
| V7 | pre-commit | 投影 fixture（两分支各改 `delivery/taskpackages/current/taskpackage.json`）→ gate | `PASS (changeId=261026-fx-q, kind=devloop)`，exit 0 | 改前 BLOCK（`实撞 1 件（delivery/taskpackages/current/taskpackage.json）`）→ 原型 PASS |
| V8 | pre-commit | `node .claude/hooks/enforce-scope.mjs --self-check` | `ok: true`，**8 臂**（新增 `fixture-real-topology`、`sibling-legacy-file-topology-allowed`） | 原型 8/8 全绿；判别对照：同一真实拓扑夹具喂原钩子 → `exit 2`（`../trees/pod-x/firmware/x.c 超出边界`）、原型 `exit 0` |
| V9 | pre-commit | 真实树双探针（gov 树 builder 写主树 `firmware/probe.c`；主树 landing-verifier 写 gov 树 review 目录）+ 两条护栏探针 | 双探针 `status:0`；护栏：builder 写 `docs/contracts/**` → `status:2`（`deny` 命中）、写仓外路径 → `status:2` | 原钩子双探针均 `status:2`（§2 T6 逐字）；原型双探针 `status:0`、两护栏 `status:2`（逐字已录） |
| V10 | pre-commit | `node .claude/scripts/merge-driver.mjs --self-check-merge-tooling-hotfixes` | `{"pass":8,"fail":0,"total":8,"ok":true}`，exit 0 | 原型实跑；**反向**：把 target 声明改四空格 → `{"pass":7,"fail":1,…}`，**exit 1** |
| V11 | pre-commit | 复跑 V0c 五入口 | 计数与基线一致（3/16/7/9/23 全 ok） | 原型复跑一致；`--self-check-quick-checks` 须在仓内跑（原型副本缺 `verify-gates.json`，未实测） |
| V12 | pre-commit | 261017 原形 verdict fixture（`## 判定` + `**ok: true — PASS**`，无行首 verdict 行） | `BLOCK`（判定不变），`已检查` 明细含 `"refUsed":"feat"` 与 `"verdictFormatMiss":true` | 原型逐字（改前同场景仅 `"frozen":true,"verdict":false,"covered":true`） |
| V13 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-merge-gate-cluster-fix/PROPOSAL.md` | `problems` 空、`anchorUnion` = 7 个文件（与 landing-manifest 逐条一致）、`advisories` 不含路径夹带 | 起草期实跑见 §5 末尾「preflight 实跑」段 |
| V14 | post-commit | 落地后复跑 V0–V13 全部（in-repo；含 V4b） | 全部符合上表 | 落地后由主线程 / landing-verifier 执行；**未实测**（依赖落地） |
| V15 | post-commit | 本批自身合并：FROZEN + GREEN + landing-verdict 齐备后 `node .claude/scripts/pre-merge-escalation-gate.mjs 261026-gov-review-merge-gate-cluster-fix --source-ref HEAD` | `PASS (…, kind=gov)`，exit 0 | **未实测**（依赖 FROZEN）；本批无契约 diff，走 `no-contract-diff` 分支，不触及 T1/T2 的凭据链 |

**preflight 实跑**（V13，起草期 2026-10-26 逐字）：

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-merge-gate-cluster-fix/PROPOSAL.md
{"ok":true,"gate":"landing-preflight","proposal":"delivery/review/261026-gov-review-merge-gate-cluster-fix/PROPOSAL.md",
 "section2BodyFound":true,"anchorCount":7,
 "anchorUnion":[".claude/hooks/enforce-scope.mjs",".claude/references/pod-coordination.md",
   ".claude/scripts/merge-driver.mjs",".claude/scripts/pre-merge-escalation-gate.mjs",
   ".claude/skills/merge-to-main/SKILL.md",".gitattributes","AGENTS.md"],
 "advisories":[],"findings":[]}
exit=0
```

`anchorUnion`（7 项）与头部 landing-manifest 围栏块逐条一致；`advisories` 空 = 锚行未夹带路径 token。

**V6 基线实跑**（同一时点，`git check-attr merge` 三件）：三行均为 `merge: unspecified`（即现状无 union），与 V6 的 post-commit 预期 `merge: union` 构成对照。

**V5 实跑命令与输出**（起草期逐字；落地后以仓内实现复跑）：

```
$ node --input-type=module -e "
import { contractFindingsAccounted } from './.claude/scripts/pre-merge-escalation-gate.mjs';
import fs from 'node:fs'; import path from 'node:path';
const files=[]; (function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=path.join(d,e.name);
  if(e.isDirectory()) walk(p); else if(e.name==='review-packet.md') files.push(p);} })('delivery/taskpackages');
let hits=0; for(const f of files){ const id=path.basename(path.dirname(f));
  const r=contractFindingsAccounted('.', id, { readText:(q)=> String(q).includes('review-packet.md')?fs.readFileSync(f,'utf8'):null, listFiles:()=>[] });
  if(r.present) hits+=r.hits.length; }
console.log(JSON.stringify({packets:files.length,hits}));"
HEAD 实跑（旧判据）: {"packets":89,"hits":11}
```

- **HEAD 实跑** = `hits:11`（上面这段命令的逐字输出，2026-10-26 在本 gov worktree 跑）。
- **落地后预期** = `{"packets":89,"hits":8}`（两字段，与上面所引命令的输出形态一致）：同一命令在原型（改法 patch 后的 `gate.mjs`）上实跑得此值；它与 HEAD 的 `hits:11` 相差 3 = 自述散文三行，包名见 T4 证据。

## 6. 留根清单

**本批不关的根（指路，不硬治）**：

1. `ARQ-260924-landing-gate-dir-anchor-gap` 残余②（landing 门目录前缀展开）——human 已裁出留后批；本批 manifest 不含 landing 门相关文件。
2. `ARQ-261014` ④（final 相位派发契约）**已裁拆出独立批（§4 OQ3）**；主线程已于 2026-10-26 登记新条目 `ARQ-261026-final-phase-dispatch-contract-unwritten`（登记动作归主线程；本批只留指针，不写条目）。
3. **`input-brief.md` 不入派生态 carve-out**：它是 `current/` 下第三件，但 merge-driver 的 `PROJECTION_CONFLICT_FILES` 不含它（两分支同改时 driver 会 abort blocked）。本批刻意让闸保持 tier3——宁可升级人，也不制造「闸放行、driver blocked」的错位。复议触发 = 该文件确发生双分支相撞且被人工判定为零语义冲突时，两处（driver 列表 + 闸常量）同批扩。
4. **派生态集双源**：闸的 `DERIVED_STATE_CARVE_OUT` 与 merge-driver 的 `PROJECTION_CONFLICT_FILES` 同形两份（代码注释已标「lib 收编留根」，对齐 `VERSIONED_ARTIFACT_RE_LOCAL` 先例）。复议触发 = 第三次需要同步这两份清单时，收编进 `lib/`。
5. **INDEX 陈旧检测 = convention-only**：debt 账本侧有 `debt-doctor` 的 `index-stale` 检测，三条 lane INDEX 没有对应检测器；本批以「合并后重跑全量重建」的文面义务承载（skill 步骤 8）。复议触发 = 观察到一次「union 合并后长期未重跑、INDEX 与实际目录不一致」的实例时，按 debt-doctor 先例加只读检测（不新增门）。
6. **T4 ③写作约定未落 `integrator.md`**：列位判据已使非 finding 行天然免疫，再加「禁止复述字面集」的写作禁令属冗余约束（且与 `integrator.md` 现有「Kind=hard-violation 且 Basis 涉契约时字面起引」的义务并列会让人误读为两条独立义务）。复议触发 = 出现「Kind 列被写成 hard-violation 的非 finding 行」的实例时。
7. **闸的 git 命令跑在调用方 cwd、`--project-dir` 只管文件读点**：起草期构造 fixture 时发现（`--project-dir` 指向 fixture 仓但 git 命令仍在调用方仓解析 ref）。本批不改（属既有形态、改动面会扩到所有调用点）；文档面已在 §5 fixture recipe 里写明「cwd 必须是 fixture 仓」。复议触发 = 出现「调用方 cwd ≠ 目标仓」的我方用法时。
8. **子模块拓扑未覆盖**：`.git` 指向 `<superproject>/.git/modules/<name>` 的场景本仓不使用；`locateGitTree` 的拓扑分派对它会回落到「文件形态」分支，行为等价现状（不新增误伤）。复议触发 = 仓内引入 submodule 时。
9. **`--human-approval-ref` 对 gov / unknown 仍 FATAL**：T3 的 NOT_APPLICABLE 不需要人批口，故不动该 FATAL 语义；若将来 ledger 面真需要「人批后放行」语义，另行立项（会碰豁免设计，属真分叉）。

10. **账本枚举边界（OQ1 裁定口径）**：`delivery/retro/**`、`delivery/archive/**`、`docs/debt/README.md` 不在允许集内，纯此类变更集仍 BLOCK（`retro-only` fixture 实测仍 BLOCK）。复议触发 = 这类纯制品合并实际发生且被人工判定为「不适用」时，按账本拓扑扩枚举（同批更新 repo-structure 指针与自检夹具）。

**本批结项（landing 后由主线程执行登记 / 状态变更，architect 不做）**：六条 ARQ 全部结项——`ARQ-261017-merge-gate-contract-evidence-ref`、`ARQ-261021-merge-gate-first-merge-blindspot`（同根合并消费，与上一条同批结）、`ARQ-261017-merge-gate-scope-unstated`、`ARQ-261014-merge-gate-tooling-hotfix-batch`（①②③ 落；④ 已裁拆出独立批，新条目 = ARQ-261026-final-phase-dispatch-contract-unwritten；④ 残余 Windows UUID 一例由 T7 在 merge-driver 的 eventId 处一行注释承接）、`ARQ-261011-contract-gate-literal-selfquote-false-positive`、`ARQ-261018-enforce-scope-worktree-anchor-offbyone`。

**环内自律（不进模板的纪律）**：本批 `§5` 的原型实跑件全在 `c:\tmp\mgfx`（scratch，不入仓）；落地实现与原型若有形态差异，以**落地实跑**为准覆写 §5 预期值，不把原型数字当既有事实转引（本仓「ARQ 处方须实测复证」纪律的对偶）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-26 - architect-reviewer

> 判官形态注记：本环由 harness 降级形态（零上下文 coder 子 agent 承判官）执行；Track B 判官义务（只读 canonical、只写本段、地面真值亲核、verdict 行首机锚形态）同额履行。

**四查结论概要**：

1. **意图保真**：human 2026-10-26 四项裁定在 §4 逐条如实承载。OQ1 的原则句「合并闸要跟现在的账本结构一致，不能跟历史硬套」已落为 T3 允许集正推枚举（四域 + intake；retro / archive / `docs/debt/README.md` 刻意排除且 §6 留根 10 有复议触发）；OQ2 = T1/T2/T8；OQ3 划界见 F2（有一处承接矛盾）；OQ4 = T2 第 2 条闸内点名（产出侧双落点在场我已亲核：`track-b-verifier-brief.md:33`、`landing-verifier.md:34` 逐字确认）。
2. **纪律合规**：manifest 7 文件 == §2 各 T 目标文件并集（preflight 亲跑复证，`anchorCount:7`、`advisories:[]`、exit 0）；零契约触碰、零 agent 定义改动、无新增门成立；「不判轻、恒 Track B 主路径」成立（机器面 G1+G2+G3）。T2 复证否决 ARQ-261021 方向③（改叙事）与 261015 批「无人签门、合并过机械凭据闸」裁定方向一致，正确。
3. **撞车复核**：`.gitattributes` 新增三条 union 与既有五条 union（`:12` / `:19` / `:28` / `:29` 及 codebase-memory）无路径交集（逐行读过）；T8 兜底通道与 pod-coordination §10e「无人合通道」（`:209` 逐字在案）的矛盾被 T8 自身显式调和（开放条件仅限闸假阴性），成立。唯一撞车 = F2。
4. **牵连面**：AGENTS.md / merge-to-main SKILL.md / pod-coordination.md 三处文面联动均已入 §2 与 manifest；反向锚（requirements 侧）无；契约指针零刷新（零契约触碰，成立）。

**两项自报偏离的判官表态**：

- **T4 取「②列位判据 + ③」弃 ARQ 原倾向「①否定判别 + ③」**：**同意本批取法**。亲核依据：`integrator.md:40` 逐字在案（「Findings 表为 `Severity | Kind | Area | Finding | Basis | Required action` 六列」+ hard-violation Basis 字面前缀起引义务）；`261016-p3-story-body-sd-cache/review-packet.md:71` 实测 Kind 列逐字 `**hard-violation**（原判）`——判据必须前缀匹配而非等值，② 成立；① 是自然语言启发式（「非 finding」「不适用」等措辞绕过），判别力弱于结构位。census 亲跑复核：`delivery/taskpackages` 下 review-packet 89 份、旧判据命中 11 行（8 表行 + 3 散文行，三处散文行号与 §2 所引逐字一致）、新判据按三条件推演 = 恰 8 表行 / 8 包，包名单与 §3 所列逐一吻合。判据列位推演：`t.split('|')[2]` 对 `| a | b |` 形态确实取到 Kind 列（split 首元素为空串，偏移正确）。
- **T5 取「`.gitattributes` union」而非闸侧白名单**：**同意**。「为什么不只在闸里加白名单」论证亲核成立：merge-driver 只对两个投影路径特判（`merge-driver.mjs:943-946` 逐字确认），闸单侧放行只把摩擦挪到 rebase 阶段；debt INDEX 先例（union + 渲染自愈，`.gitattributes:21-29`）形态同源。

**ground truth 亲核留痕（判官实跑，2026-10-26，本 gov worktree）**：

- V0 亲跑：`{"selfCheck":"pre-merge-escalation-gate","pass":51,"fail":0,"total":51,"ok":true}` exit 0，与 §5 基线一致。
- V0b 亲跑：`ok:true`、6 臂（fixture-nonempty + 五臂），与基线一致；夹具按「gitdir 是文件」构造（`enforce-scope.mjs:277-279` 逐字确认）、「此绿不具判别力」注记成立。
- V0c 亲跑：retro-advisory 3/3、detect-changes 16/16、rebase-target 7/7、worktree-topology 9/9、merge-friction 23/23，全 ok。
- 本 worktree `.git` 实测逐字 `gitdir: C:/AI/KidPal/.git/worktrees/KidPal-gov`——指向目录（无 `/gitdir` 后缀），T6 拓扑前提成立；`enforce-scope.mjs:141-146` 逐字与 §2 引文一致（`dirname(dirname(dirname(gitdir)))` 三级回推在案）。
- 闸脚本逐字核对：`:137`（proposalPaths 来自 source..main diff）、`:139-144`（target 取件循环）、`:149`（`.find()` 首匹配）、`:83-90`（`isGreenVerdict` 取最后命中——同仓终态语义先例成立）、`:416`、`:441`、`:449`（`hasArchLoopContractEvidence(execFn, source, 'main', projectDir)` 硬绑字面量）、`:765-767`（FATAL 分支）、`:195-198`（行级过滤）、`:310-321` / `:318`（carve-out）、`:158`（现 reason 文案）——全部与 §2 引文逐字一致；现文件 790 行、ARQ 引 `:425`/`:431` 已漂至 `:443`/`:449` 属实。`changes-index.mjs:202-231` `deriveLanded`「后出者胜」逐字确认。`merge-driver.mjs:172-183` / `:1423-1427` / `:173` / `:1426`（两热修 + 两处「gov 侧待追认」）、`:943-946`（两投影路径）逐字确认；`.gitattributes` 现无三条 lane INDEX 的 union（`git check-attr` 基线「unspecified」可信）。
- 文面锚逐字核对：`AGENTS.md:22` step 7 原句、`SKILL.md:13`（前置节第二条）、`SKILL.md:34`（`--upsert <本批 changeId>   # 逐个`）、`pod-coordination.md:205`（§10d 末句 `判不了 = 存疑向严升 ③。`）、`:209`（§10e「无人合通道、无人类签字门」+ 末句「该条目已于 2026-09-24 结项归档）。」）、`integrator.md:40`、`repo-structure.md`「docs/debt/」行四域——全部与 §2 所引一致。`261017-arch-review-p3-story-body-sd-cache/landing-verdict.md:9` 实测 `verdict: PASS + 6 ADVISORY`（OQ4「事后已由 verifier 补行」属实）。
- V13 preflight 亲跑：输出与 §5 所引逐字一致（ok:true、anchorCount:7、advisories 空、exit 0）。
- 计数注记：`docs/debt/gov` 亲测 131 = PROPOSAL 所记 130 + 起草后新登记的 `ARQ-261026-final-phase-dispatch-contract-unwritten.md`（untracked 在盘），非计数错误；其余五处计数（tech 62 / requirement 5 / contract 17 / intake 19 / retro 5 / archive 295）亲测逐一吻合。

**findings**：

- **F1 · BLOCKING · T3 的 `ctx.diffPaths` 无接源——改法规格不完整，逐字落地必使 V4/V4b 与 T3 六条夹具全红**。证据：T3 替换段逐字 `const diffPaths = ctx.diffPaths || [];`，但 `diffPaths` 在 `pre-merge-escalation-gate.mjs` 全文零出现（Grep 实测 No matches found）——`buildCtx`（`:435-458`）从不组装该字段，闸内仅有的 diff 计算点（`:113` / `:130` 的 `hasContractsDiff`、`hasArchLoopContractEvidence`）都只返回布尔或契约路径，不携带全量变更集；§2 T3 通篇未提 `buildCtx` 改动。后果两难：实现者按 §2 逐字落地 → `onlyLedger` 恒 false → `NOT_APPLICABLE` 永不触发，与 §5 V4/V4b 预期及 T3 夹具（4 正向）矛盾；实现者自行补 `buildCtx` → 落地含 frozen PROPOSAL 未描述的改动，landing-verifier 逐文件核对必判漂移。修法方向：§2 T3 补一条 `buildCtx` 组装行（如 `const diffPaths = execFn(`git diff --name-only ${source}..main`).split('\n').map((l) => l.trim()).filter(Boolean)` 并入 ctx），并把该行纳入「替换后代码（逐字）」，同批核对 §5 预期不变。
- **F2 · SHOULD（解后方可冻）· ARQ-261014 ④ 残余（Windows UUID）承接矛盾——按 §6 结项口径将静默蒸发**。证据：`ARQ-261014-merge-gate-tooling-hotfix-batch.md` 拆出注记（2026-10-26）逐字：「本条 ④ 残余仅承载 Windows UUID 一例，本批（261026-gov-review-merge-gate-cluster-fix）消费范围为 ①②③ 与 ④ 残余」；而 PROPOSAL 全文零提 Windows UUID / crypto.randomUUID（Grep 实测 No matches found），§6 结项行只写「①②③ 落；④ 已裁拆出独立批」；新条目 `ARQ-261026-final-phase-dispatch-contract-unwritten.md` 的证据清单只并 required-check 参数形状与 voice-round 两例同根先例，**不含** Windows UUID——三处合读，该残余在 261014 结项后无家。修法方向（二选一，主线程 append-only 承载）：① §6 结项行补「④ 残余（Windows UUID 一例）随结项并入 `ARQ-261026` 条目」+ 主线程向新条目 append 该例；② §2 增最小承接项（同一文面一句话）。不改不影响机器行为，但属 human OQ3 裁定范围的漏承接，须在 FROZEN 前闭合。
- **F3 · SHOULD（解后方可冻）· T5 改法 4 的步骤 2 追加目标缺原文锚且交叉引用指错段落**。证据：T5 写「步骤 2 括号串末尾追加…（原句引文见 T3 的 skill 段，同一段落）」，但 T3 所引是 SKILL.md **前置节第二条**（`:13`「合并方向与授权明确：…」），而步骤 2 是另一段（`:18`「**预览 overlap** — …（INDEX 已按 lane 分文件——…）」，亲读确认）——被追加的括号串原文在 §2 全文无逐字引文，违背本批对承重文本「原文引用钉死」的自身口径。修法方向：§2 T5 补步骤 2 括号串的逐字原文（含结尾「旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕）」处），并把交叉引用改为「SKILL.md 步骤 2（`:18`）」。
- **NIT（随批顺手，不单独设 verdict 障碍）· V5 预期输出形态与所引命令不一致**：§5 V5 预期写 `{"packets":89,"hits":8,"packetsWithHits":8}`，而同节所引 one-liner 只打印 `{packets,hits}` 两字段（§5 末尾「V5 实跑命令与输出」段自证）；落地复跑时以实际命令输出为准即可，建议预期列改为两字段形态或命令补 `packetsWithHits` 统计。

**升级 human 项**：无——三项 findings 修法方向均无设计分叉，属规格补全与账本承接，不触发「影响目标效果」升级线；riskTier medium 判档成立（每次合并必经的闸 + 每次写入必经的钩子，证成合理）；存疑项（T4 / T5 偏离）经亲核表态同意，无保留意见。

verdict: BLOCK(F1,F2,F3)

### R2 - 2026-10-26 - architect-reviewer

> 轮次纪律：聚焦 R1 findings 修订到位性 + 修订是否引入新矛盾，不做全量重核。R1 四项处置逐条亲核如下。

**① 修订到位性（F1/F2/F3/NIT）**：

- **F1 已到位**。T3 改法 1 末尾新增同步句「**该字段由 `buildCtx` 组装**（下述替换段 A）：`evaluate` 只消费 `ctx.diffPaths`；缺 A 时 `NOT_APPLICABLE` 永不触发」；替换段改为 A/B 两段式并显式标注「**A、B 缺一即 `NOT_APPLICABLE` 永不触发**（§7 R1-F1）」。行号锚亲核：A 段锚 `:447-449 区段`——现文 `:447` 逐字 `const contractsDiff = hasContractsDiff(execFn, source, 'main');` 与 A 段首行一致，插入点处 `execFn`（:436）与 `source`（:443）均在作用域内；B 段锚 `:415-419 区段`——现文 `:415-417` 的 else 分支与 `:419` 的 return 与 B 段对应。git diff 失败 → `catch { return []; }` → `onlyLedger` 不成立 → 仍 BLOCK，fail-closed 方向正确。
- **F2 已到位（按主线程口径核：承接项落 T7 而非 T5，不按 R1 修法原文的「T5」字面打回）**。落点自洽性确认：`merge-driver.mjs` 本就是 T7 目标文件且在 manifest 内，注释插在 T7 避免了「§2 目标文件与注释落点」错配，比 R1 建议的 T5 路线更准。锚点亲核：`merge-driver.mjs:35` 逐字 `import { createHash, randomUUID } from 'node:crypto';`、`:1060-1062` 的 `const event = { schemaVersion: 1, eventId: randomUUID(),` 逐字在案——「在该行上方加一行注释」插入点成立（对象字面量内注释行合法）；注释文案（:475）与 §6 结项行「④ 残余 Windows UUID 一例由 T7 在 merge-driver 的 eventId 处一行注释承接」与 `ARQ-261014` 拆出注记「本批消费范围为 ①②③ 与 ④ 残余」三方一致，残余不再蒸发。
- **F3 已到位**。T5 改法 4 现引 `SKILL.md:18` 逐字原文——亲比对盘上 `SKILL.md:18` 全行（含「旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕」结尾）逐字一致；追加后完整行的插入点标注精确（原文该句之后、右括号之前，追加内容语义与 v0.2 版一致）；错误交叉引用（「原句引文见 T3 的 skill 段」）已删除。
- **NIT 已到位**。V5 预期改为两字段 `{"packets":89,"hits":8}`（§5 :613），与所引 one-liner 实际输出形态一致。

**② 修订是否引入新矛盾**：

- T3 两段式行号锚（`:447-449` / `:415-419`）与现源逐字吻合（见上）；V1 夹具计数不受影响（T3 仍 6 条、T7 的 8 条归 V10），66 总数自洽。
- T7 注释插入点 `:1060-1062` 与现源吻合（见上）；T7 改法编号扩为 3 条不改其目标文件集，§3 sweep 表与 manifest 无需变动。
- §6 结项行与 `ARQ-261014` 拆出注记、`ARQ-261026-final-phase-dispatch-contract-unwritten` 条目三方一致性成立（见 F2 项）。
- **新矛盾一处（F4，见下）**——非本轮修订新引入，系 R1 漏检、本轮补获。

**findings**：

- **F4 · BLOCKING · evaluate 的 `notApplicable` 绑定无声明——逐字落地则每次闸调用即崩**。证据：T3 替换段 B 直接 `if (onlyLedger) notApplicable = true;` 且 return 改为 `return { blocked: reasons.length > 0, reasons, notApplicable };`，但 `evaluate` 现文（:332-334 起）无该绑定，替换段与 PROPOSAL 全文均无 `let notApplicable = false;` 声明（Grep 实测 `let notApplicable|notApplicable = false` → No matches found）。本文件是 ESM（import/export，严格模式）：裸赋值与 return 简写引用未声明标识符均抛 `ReferenceError`——且 return 在**每次** evaluate 调用的公共路径上（dev/gov 档同样崩），V1 的 66 夹具将全红。与 R1-F1 同种（规格不完整使逐字落地与 §5 预期互斥），同判 BLOCKING。修法方向（一行）：在 evaluate 序言（现 `:333-334` 的 `const reasons = [];` / `const push = …` 附近）加 `let notApplicable = false;`，并在 T3 替换段标注该声明行（可并入 B 段的区段说明），§5 预期不变。

**升级 human 项**：无——F4 修法为单行声明补全，无设计分叉。

verdict: BLOCK(F4)

### R3 - 2026-10-26 - architect-reviewer

> 轮次纪律：单点复核 F4 修订到位性 + 是否引入新矛盾，不做全量重核。

**F4 修订到位性（亲核）**：

- **C 段已到位**。替换段标题扩为「A = buildCtx（`:447-449` 区段）… B = evaluate 的 unknown 分支（`:415-419` 区段），C = `evaluate` 序言里的 `notApplicable` 声明（`:333-334` 区段）。**A、B、C 缺一即 `NOT_APPLICABLE` 永不触发或直接抛 ReferenceError**（§7 R1-F1 / R2-F4）」——后果表述与两根缺陷形态一一对应（缺 A = 永不触发、缺 C = 抛错），准确。C 段锚亲核：现源 `:332` 逐字 `export function evaluate(ctx) {`、`:333` 逐字 `const reasons = [];`，C 段引用的两行与现源逐字一致，`let notApplicable = false;` 插入序言位置成立；`evaluate` 全函数单一 return（`:419`），C + B 的绑定/赋值/引用三件齐备后无遗漏路径。T3 改法 1 同步句已补：「**`notApplicable` 由 `evaluate` 序言的 `let notApplicable = false;` 声明**（下述替换段 C）：B 只赋值、`return` 只引用；ESM 严格模式下缺此声明，裸赋值与简写引用都会抛 `ReferenceError`（§7 R2-F4）」——机制描述准确。
- **摘录衔接无误读**：C 段与 B 段之间有显式省略注释「…（off-main / 契约凭据 / blocked / contract-findings 等既有理由检查不变，略）…」，且 C 标注「序言（:333-334 区段）」、B 标注「unknown 分支（:415-419 区段）」——两段在真源相隔约 80 行，摘录边界清晰，不存在「声明后直接接 else」的误读面。
- **V1 七条反向自洽**：反向清单扩为七条（T1 / T2 / T3a / T3b / T4 / T5 / F4），新增 F4 句「删 `let notApplicable = false;` → 自检在 `return { …, notApplicable }` 处 `ReferenceError: notApplicable is not defined`、exit 1」——失败形态（崩溃非判红）与措辞已从「ok:false、exit 1」改为「非绿」，口径一致；新增夹具数仍 15（T1 2 / T2 2 / T3 6 / T4 3 / T5 2）、总数仍 66，F4 声明行由既有 T3 六夹具覆盖、反向属变异测试不增夹具，计数逻辑自洽。

**是否引入新矛盾**：无——C 段只增一行声明与摘录注记，不触 A/B 段语义、不触 CLI 分支与文面两处；§3 sweep 表、manifest（仍 7 文件）、§5 其余行均无需联动。

verdict: GREEN
