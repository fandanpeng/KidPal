# landing-verdict · 260908-gov-review-merge-chain-hardening

> 判定者 = landing-verifier（独立核验，与落地线程分离）；判据 = FROZEN v1.0 PROPOSAL（`af0ac8e`）+ 附录 A 逐字正文；被审 diff = `git show b89bf33`（10 文件 +345/−55）；不采信 landing-summary 自述，全部结论出自本 verifier 独立复跑与逐行 diff 对读。

## 0. Verdict：**RED（有条件退回——修完下述五项即可转绿，无需重开设计）**

**根因一句**：canonical 落地本体忠实（8 manifest 路径改法全部落位、无私货、无破坏性改动、验证集独立复跑全绿），但 (a) 结构门 landing-fidelity-check.mjs 实跑为 RED（落地线程只跑了 proposal-consolidation-check，从未实跑本门；两处证据包装偏差 + 冻结稿一处锚行格式缺陷），且 (b) 冻结验证集有两条未落地且未在 1b 申报（V1-② 行尾 sha256 夹具、V2-④ N5 背书夹具——后者是 decision-packet 明列的 human 拍板落地点）。

**GREEN 条件（全部满足后由主线程重跑结构门 + verifier 复核，merge 仍归 human 门）**：

1. 证据制品归位：`landing-summary.md` 改名/补位为 `architect-landing-summary.md`（260906/260907 两先例批均用此名，结构门硬依赖该文件名），并补 ```landing-manifest``` 围栏块（8 路径）与字面 `packet-id` token（现有正文只写「packet-bound」「蓝本 = FROZEN v1.0 PROPOSAL（af0ac8e）」，缺门判据要求的 token）。
2. 冻结层定夺 T3 锚行：PROPOSAL `§2 T3` 的「目标文件：」行括注内嵌两个 backtick 路径（`docs/contracts/**` 或 `docs/spec.md`）夹「或」，锚清单解析器整行 fail-loud（或锚未决）。修法 = 把两路径移出「目标文件：」行（如去 backtick 化为纯 prose），属非语义格式修正，但改冻结文本须 human/architect 追认留痕。
3. 补 V1-② 冻结夹具：`merge-driver.mjs` 自检 T7 段补「投影消解后 canonical ↔ current sha256 相等 + autocrlf 场景」文件级夹具（现 T7e 仅字符串级 `normalizeLf` 断言，merge-driver.mjs:1190）。
4. 补 V2-④ N5 背书夹具：decision-packet N5 节明列「落地点：… + V2 增背书夹具『unlock 收窄形态下，off-main / provenance 违例 Pod 解锁后仍在同 invocation 被 Step 1b BLOCK 并回落 blocked』」——未落地且 1b 未申报。须补夹具或经 human 追认的偏差申报。
5. 修两处落地文本缺陷（见 §3 表 T7/簿记行）：acceptance-evidence.md:12 掉字补回；ARCH-REVIEW-QUEUE.md §2 结移行移到分隔行之后。

## 1. 结构门输出（原样记录）

**实跑一（按脚本用法对证据目录本体）**：

```
$ node .claude/scripts/landing-fidelity-check.mjs changes/arch/260908-gov-review-merge-chain-hardening
{
  "ok": false,
  "gate": "landing-fidelity",
  "message": "architect-landing-summary.md is required（landing 声称 + packet-binding）",
  "reviewDir": "changes/arch/260908-gov-review-merge-chain-hardening"
}
---exit=1
```

**实跑二（verifier 诊断复跑：在 c:\tmp/lfc-recheck 建临时核对目录——只复制、不改证据本体——把 summary 按「落点对照」表原样补 manifest 围栏块后，以 baseRef=af0ac8e 从仓根重跑）**：

```
{
  "ok": false,
  "gate": "landing-fidelity",
  "message": "governance landing 保真度核验未过",
  "reviewDir": "changes/arch/lfc-recheck",
  "errors": [
    "architect-landing-summary.md 缺 packet-binding（须引 packet-id + 被批 review id，A9）",
    "PROPOSAL §2 锚清单：或锚未决：T3 · integrator.md「merge-chain execution」节（G2，A-1 已裁：agent def 执行语义变更、恒 human-approved，human 2026-09-08 已批） 两路径间出现「或」——须 FROZEN/manifest 层定夺后重跑（不静默拆解、不吞过）"
  ],
  "gitDegraded": false
}
---exit=1
```

**诊断复跑的实质判据读数**：`claimedNotLanded` 与 `plannedNotLanded` 皆空（8/8 manifest 路径相对 af0ac8e 均有改动）、`unauthorized` 为空（summary 声称集 ⊆ PROPOSAL 清单，无 scope creep）、git 未降级。即：**结构门四条 RED 全部是证据包装/锚行格式问题，无一涉及实质越权或漏落**；但因门字面 RED 且两条冻结验证集漏落（§0），按「红即停」判 RED。

**验证集抽验（verifier 独立复跑，不采信 summary）**：见 §5。

## 2. 逐文件核对表（manifest 8 路径 + 2 簿记文件，全部核对、无抽查跳过）

| # | 文件 | 结构 | 语义 | 冻结依据 → 实际 |
|---|---|---|---|---|
| 1 | `.claude/scripts/merge-driver.mjs` | 动了 | **ok（附 2 条验证缺口）** | T1-1 特判：`detectVersionedAddAdd`（:893-899，AA + `.vN.md$` 正则）+ `VERSIONED_ADD_ADD_GUIDANCE` 并集指引（:929-933，含 §10c 互指）接线 rebase/merge 两冲突点（:1296-1308、:1388-1399，abort 前探测、命中 warn 指引后照常 blocked）——补集不变式由 T7b 机证。T1-2：`normalizeLf/normalizeLfFile`（:906-920）接线投影特判 `git checkout --theirs` 后（:859）。T1-3：`ensureGraphCacheSynced` 命名导出（:741）。T1-4：`resolveUnlockWorktree`（:923-928，`findWorktree || null`）+ `approvalRefUnlock` 内 null → `errorExit`（:982-983，不回退主仓，A-3 已裁）。T6：`approvalRefUnlock` 改 import `resolveHumanApproval`（:36、:985，N5 选项 2 收窄 + Step 1b 背书注释 :970-977）。自检 T7a-g（:1174-1197）。**缺口见 §0 条件 3/§4 观察 3** |
| 2 | `.claude/scripts/lib/governance-gate-shared.mjs` | 动了 | ok | T6/F1：纯新增 B8 段（`resolveHumanApproval` 迁入，与 gate 原函数体逐字一致、仅 `deps.decision` 参数化且缺省 `'approve-escalated-merge'` 保旧语义）+ `existsSync` 补入 fs import；既有导出与函数体零改动（diff 仅两块） |
| 3 | `.claude/scripts/pre-merge-escalation-gate.mjs` | 动了 | ok | T5：`contractFindingsAccounted`（hard-violation + `docs/contracts/**`/`docs/spec.md` 字面前缀、CDQ 含 changeId 或 human-decision defer/accept-risk 锚；present:false 放行 = A-8 只拦可识别形态）+ evaluate 通用 4 分支（`contract-findings-unaccounted`）+ buildCtx/selfCheck 接线 + 自检 +3 例（21/21 复跑绿）。T6：`isDirectRun()` import.meta 守卫包 CLI 入口；`resolveHumanApproval` 本地定义删除改具名导入。既有六条例外清单未动（grep :10 例外清单节在位，diff 无该区 hunk） |
| 4 | `.claude/references/pod-coordination.md` | 动了 | ok | T2：§10c 新节（槽位预约机制 / A-7 双源关系 authority / 登记表实测超集六行含 engine-patch、openclaw-patch、CARRIER、provider.config.yaml、apply-xiaozhi-overlay.sh / B-2 维护三句 / B-0 单一权威互指注记）；§10a 补「消解纪律五条」（形态钉死 / graph 缓存三拍引 `ensureGraphCacheSynced` / 投影归一 LF / 双方冻结件复跑 / 版本化制品禁单边）；第 3 步改素形 `git rebase --continue` + `git config core.editor true` 一次性设置（N3/A-4，含 GIT_EDITOR=true 与 -c 形态不命中前缀的禁用句）。既有 §10a/§10b 行除第 3 步（T2-2⑤ 授权改动）外零改动 |
| 5 | `.claude/references/acceptance-evidence.md` | 动了 | **mismatch（一字）** | T7/A.2：「必要证据」段末（:12）追加版本无关 green-guard 三步条款 + guard-class token + 母版/清点范围句；「反例」节（:41-51）挂硬写版本字面反例 + 正形指针。**掉字**：:12 末句「vs 落盘 / 措辞守（钉死某版合理）」，A.2 冻结稿为「措辞守**卫**（」——逐字取用义务破一字（语义不变，但 1b 声称「其余落点与冻结稿逐字对账」与此矛盾） |
| 6 | `.claude/agents/integrator.md` | 动了 | ok | T3/A.1：「Merge-Chain Execution / 集成链执行」节（:56-86，插入位 = Process 之后、References 之前，符合 A.1 位置钉死）与附录 A.1 五个小节**逐字一致**（本 verifier 逐行比对：触发条件句、三前置、四铁律、消解执行三条、复跑清单、fs-write 收口归属，零偏差）；:42 B-4 句在位（「Kind=hard-violation 且 Basis 涉契约/spec 时，Basis 须以 docs/contracts/** 或 docs/spec.md 字面前缀起引」+ 260908 批注记）。既有边界句（:12/:54/:73/:74 一族）未动 |
| 7 | `.claude/dev-loop.settings.json` | 动了 | ok | T4：`permissions.ask` 两形逐字（`Bash(git merge:*)` / `Bash(git rebase:*)`）；`_doc` 补 N6 批准判据行 + A-4 回填义务①②主张限定句（条件语义 T3 承载 + 三类绕道在册 + hook 留根）。`permissions` 键 = ask/deny，无 allow（V4 断言）；deny 两行逐字在位；JSON parse OK |
| 8 | `docs/ARCH-REVIEW-QUEUE.md` | 动了 | **ok（附格式缺陷）** | V5：两条 260907 merge ARQ 行自 §1 开放表删除、结移 §2 已结索引（含 `260907-gov-review-merge-friction` 承载批引用 + 核销确认 + 2026-09-08 日期）；B-8 排队行 `ARQ-260908-versioned-guard-repoint-execution` 入 §1（待重指 4 件点名 + 发起时机 = 下一批触碰冻结件或 human 专项批，与 T7③/B-8 一致）。**格式缺陷**：desync 结移行插在 §2 表头行与 `\|---\|` 分隔行之间（:b89bf33 后 :74-76），markdown 表头被数据行隔断、表格渲染破损（closeout 行插位正确，两行应同在分隔行后） |
| 9 | `changes/INDEX.md` + `changes/.changes-index-state.json` | 动了 | ok | V5 授权动作：`changes-index --upsert` 单行钉入（状态 draft→frozen、已合main no），状态文件同步；非 manifest 路径但为冻结 V5 明列动作，不属私货 |

## 3. landing-summary 1b 三条偏差裁定

1. **V4 大小写断言（偏差 1）——裁定：落地忠实、断言笔误，成立。** 冻结 V4 写 `grep -c "merge-chain execution"`（小写），同 packet 附录 A.1 题行为「Merge-Chain Execution / 集成链执行」（大写），且 A.1 头注明文「landing 时逐字取用」——冻结 packet 内部冲突以 A.1 为准合理。落地 :56 题行与 A.1 逐字一致；实跑 `grep -c "Merge-Chain Execution"` = 1、小写形态 = 0。判定：不改冻结稿、偏差申报充分，无需回改 canonical。
2. **V6 演练形态适配（偏差 2）——裁定：等价性部分成立，申报充分，可接受。** 前提属实：merge-driver 无 import 守卫（本 verifier grep `import.meta` = 0），V6 冻结形态「driver 跑」在演练会话不可行。适配后实证覆盖 = 真实临时仓 add/add porcelain 与 T7a 输入形状对账 + 检出命中 + 并集指引在案 + reflog 零 ours/theirs。范围收窄处：冻结 V6 的「并集消解后 X 与 Y 双齿绿」未在演练复现——但 driver 侧行为本就是「检出 + 指引 + blocked」（特判只 widen 指引、不自动消解），并集消解与双齿复跑由 §10c/§10a runbook 与 T3 正文承载，故该缺口属演练降档而非改法缺口。保留观察：双齿绿断言建议随 §0 条件 3 的夹具补强一并落。
3. **T7③ 重指不在本批（偏差 3）——裁定：与冻结条款一致，成立。** 冻结 T7③ 原文「本批只登记清单与规约、不预批任何冻结件改动」；b89bf33 diff 不含 `server/acceptance/**` 任何改动（本 verifier 核对 commit 全量 10 文件清单），A.3 八实例零触碰，B-8 排队行承接重指执行。零冻结件改动红线保持。

## 4. 破坏性自检结论

- **投影特判语义**：`tryResolveProjectionConflicts` 既有 `--theirs` + add + continue 循环零改动，仅在其内加 `normalizeLfFile` 一行（:859）——未破坏。
- **Step 0 白名单收口**：本批 diff 无 Step 0 区 hunk——未动。
- **kept-blocked**：`approvalRefUnlock` catch 块仍返回 `'kept-blocked'`（:989-995），T6 重写未改字面与意图——未动。
- **gate 六条例外清单**：例外清单节（:10 起）无 diff hunk，evaluate 既有分支零删改，仅追加 T5 分支——未动。
- **canary 12 条（scope-map 等）**：`.claude/scope-map.json`、`.gitattributes` 不在 b89bf33 文件清单——未动。
- **既有 runbook 行**：pod-coordination §10a 仅第 3 步被授权改写（N3），其余行零改动；§10b 零改动。
- **契约面 / spec / CDQ 写权**：零触碰（diff 无 `docs/contracts/**`、`docs/spec.md`、`docs/CONTRACT-DECISION-QUEUE.md`）——红线保持。
- **越权扫描**：b89bf33 全量 10 文件 = 8 manifest + 2 个 V5 授权簿记文件，`changedNotInProposal` 实质为空——无夹带、无私货、无权限/写域扩张。
- **核验独立性自评**：8 manifest 文件全部逐行对读、无「挑好看的核」；三脚本 diff 全量读、两文档 diff 全量读、两簿记文件 diff 读。

## 5. 抽验记录（verifier 独立复跑，2026-09-08）

| 项 | 冻结要求 | 独立复跑结果 |
|---|---|---|
| V1 六自检 | 全绿 | retro-advisory 3/3、quick-checks ok:true（expectedCmds 全等）、detect-changes 16/16、rebase-target 7/7、worktree-topology 9/9、merge-friction 17/17（11 旧 + T7a-g）——全绿，与 summary 一致 |
| V2 gate 自检 | 21/21 | `--self-check` = pass 21 / fail 0（含 T5 +3 例）；另 changeid-prefix 4/4 |
| V2 lib import 探针 | exit 0 + `resolveHumanApproval` 可调 | `import('<lib>')` 成功、`typeof m.resolveHumanApproval === 'function'`；`grep -c "process.argv"` = 0 |
| V2-③ 七消费方名册 | 全绿零回退 | landing-fidelity-check（isFrozen 19/19）、proposal-consolidation-check（19/19）、closeout-check、taskpackage-required-check、devloop-artifact-check、changes-index、portfolio-reconcile——七套 `--self-check` 全 ok:true |
| V4 投影 grep | 逐条复跑 | `Merge-Chain Execution`=1（大小写裁定见 §3-1）、`10c`=2、`versioned-artifact-claim`=1、`guard-class`=2；deny 两行 `-cF`=1/1；ask 两形 `-cF`=1/1；permissions 无 allow 键（node 解析实证）；「永不 push」=1、「永不 merge 进 main」=1；JSON parse OK |
| V2-① T5 两态 | 真实扫描路径两态 | verifier 独立探针（注入 readText 直测 `contractFindingsAccounted`，c:\tmp/t5probe）：契约类 hard-violation 无 CDQ 无锚 → present:true/accounted:false（evaluate 分支即 BLOCK）；有 CDQ 行 → accounted:true；有 defer-contract-change 锚 → accounted:true；无契约前缀 → present:false（A-8 放行）；packet 不在场 → present:false。五态全符合。**注**：gate 自检 +3 例为 ctx 注入式而非冻结「真空绿规约：fixture 非空断言先行」的真实扫描夹具——helper 行为由本探针补证，夹具弱化不作独立 RED 项，但建议随 §0 条件 4 一并补 |
| V1-② 行尾夹具 | sha256 相等 + autocrlf 场景 | **未按冻结落**：T7e（:1190）仅字符串级 `normalizeLf` 断言，无 sha256、无 autocrlf 场景、`normalizeLfFile` 无夹具——1b 未申报，RED 条件 3 |
| V2-④ N5 背书夹具 | decision-packet 明列落地点 | **未落地**：merge-friction 17 断言 = 11 旧 + T7a-g，无「解锁后 Step 1b 同 invocation BLOCK 回落 blocked」夹具——1b 未申报，RED 条件 4 |
| V1-③ 回退分支断言 | errorExit 而非静默落主仓 | 部分：T7f 钉 `resolveUnlockWorktree` null 返回（:1193-1194）；`approvalRefUnlock` 内 errorExit 分支（:982-983）无直接夹具（T1a/b 走注入 runner）——代码在案、语义正确，作观察项并入条件 3 一并补 |
| V1-④ ensureGraphCacheSynced 导出可 import | 探针 | 未落探针；导出在案（:741）静态可证；merge-driver 无 import 守卫使 import 探针会执行 main()——建议以「grep export」级断言落，并入条件 3 |
| V3 ask 探针 | 前缀命中 | ask 两形逐字在位（-cF=1/1）；弹窗实测未做（summary V3 行如实申报以 V4 断言替代；条件语义机械层不判 = A-4 已裁）——可接受 |

## 6. 判定依据与边界

- 本 verdict 唯一产出物；verifier 未改任何被审文件（c:\tmp 下临时核对目录为只读复制件，用后可弃）。
- 结构门两处证据包装偏差（文件名 / manifest 块 / packet-id token）+ 冻结稿 T3 锚行「或锚未决」为门字面 RED 直接原因；冻结稿锚行修正须冻结层（human/architect）追认，verifier 不代改。
- 语义保真总评：**8 路径改法全部按冻结条款落位，T3 节逐字零偏差，无私货、无破坏性改动**——RED 限于证据包装与验证集补齐，不涉重开设计；五项 GREEN 条件完成后由主线程重跑 `landing-fidelity-check.mjs`（应 ok:true）并交 verifier 复核，merge 决定恒归 human 门。

---

## 7. 终态复核（2026-09-08 · 修复 commit `aa6ffd9` 后）

> 复核对象 = 修复 commit `aa6ffd9`（RED 五前提逐条修复）。本轮 verifier 独立动作：重读修复 diff 全量、重跑结构门与自检、独立核 T8c 结构性前提。verdict 更新为 **GREEN**。

### 7.0 Verdict：**GREEN（放行建议；merge 决定恒归 human 门）**

五条转绿前提逐条复核：

| # | 前提 | 复核实证 | 裁定 |
|---|---|---|---|
| 1 | summary 制品归位 + manifest 块 + packet-id | `landing-summary.md → architect-landing-summary.md`（git mv）；新增 `packet-id = 260908-gov-review-merge-chain-hardening §2 改法集（T1–T7）+ §5 验证集` 行；```landing-manifest``` 围栏块 8 路径与 PROPOSAL 逐字一致 | 满足 |
| 2 | PROPOSAL T3 锚行或锚修正 | 括注内两路径去 backtick 转散文（含「格式注记…verdict 转绿五前提之追认 2026-09-08」留痕）；落地正文零改动；锚并集现 8/8（结构门 proposalTargets = summaryTargets = 8，无锚问题） | 满足（附残余观察 a） |
| 3 | V1-② sha256 夹具 + V2-④ N5 背书夹具 | T8a（:1201-1206）：真实 CRLF 文件经 `normalizeLfFile` 归一后与 LF canonical 做 sha256 逐字节相等断言；T8b/c（:1207-1219）：独立临时仓 unlock→ready-to-merge→BLOCK 回落 blocked 自纠可落。merge-friction 自检 20/20（verifier 独立复跑确认），其余五套自检复跑零回退（3/3、16/16、7/7、9/9） | 满足（附残余观察 b） |
| 4 | 措辞守卫掉字 | acceptance-evidence.md:12 「措辞守（」→「措辞守卫（」，A.2 逐字恢复 | 满足 |
| 5 | 队列 §2 表头换位 | desync 结移行移至 `\|---\|` 分隔行之后，与 closeout 行同侧，markdown 表格结构恢复 | 满足 |

### 7.1 结构门实跑输出（修复态，baseRef=af0ac8e，原样）

```
{
  "ok": true,
  "gate": "landing-fidelity",
  "reviewDir": "changes/arch/260908-gov-review-merge-chain-hardening",
  "baseRef": "af0ac8e",
  "frozen": true,
  "packetBinding": true,
  "proposalTargets": [ 8 路径 = manifest 全集 ],
  "summaryTargets":   [ 同上，8/8 相等 ],
  "unauthorized": [],
  "claimedNotLanded": [],
  "plannedNotLanded": [],
  "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 1, "problems": [], "resolvedByPrefix": ["taskpackage-required-check.mjs:143 → .claude/scripts/…"] },
  "verbatimQuoteAdvisory": { "missing": [ "T4 ask JSON 块 not-found-verbatim（缩进形态差，advisory-only）" ] },
  "advisory": { "changedCanonicalNotInProposal": [] }
}
---exit=0
```

advisory 三条均不阻塞：verbatim missing = 冻结 T4 的 ask 块在 settings 内嵌缩进与 PROPOSAL 顶层缩进不同所致（同族格式 advisory；ask 两形逐字在场已由 `-cF` = 1/1 独立证实）；citation resolvedByPrefix 为消警留痕；lint-descriptions 对 `landing-verifier.md` 的 over-length 警告与本批落地无关（warning-level、agent def 自身）。

### 7.2 T8 夹具定性（verifier 独立判断）

- **T8a 判：满足 V1-② 冻结断言实质**。文件级 `normalizeLfFile` + sha256 逐字节相等两要素齐；autocrlf 场景以「CRLF 物化产物直接入夹具」等价模拟（未真跑 `core.autocrlf=true` 的 checkout 往返），达冻结「canonical ↔ current sha256 相等」断言之实质，可接受。
- **T8b/c 判：机制级夹具成立，背书链由结构性亲核补足**。T8b 以真实临时仓 + pod 文件复证 unlock→ready 机制；T8c 机证 ready→blocked 回落自纠可落。Step 1b 无条件全闸调用（无 unlock 旁路）为代码形状事实——verifier 本轮独立亲核（merge-driver.mjs:1246 Step 1b 注释「合并前必经」+ :1251 全闸 spawn 在 processPod 主流程、approvalRefUnlock 无跳过 Step 1b 的路径），与 decision-packet N5 adjudicator 亲核（:1136-1150）双重背书。端到端「真 gate 对 off-main 夹具 BLOCK」未复现，属机制级与端到端级的残差，不阻塞（结构性不变式已双亲核 + 机制级夹具在案）。

### 7.3 残余观察（不阻塞，留档不入转绿条件）

- **a**：PROPOSAL T3 锚行修正时括注散文把 `docs/contracts/**` 误写为 `docs/contracts**`（少一斜杠，aa6ffd9 引入）。纯 prose 笔误、解析器不提取、权威匹配形态在 integrator.md:42 与 gate 注释（均正确）；建议随下一批治理批顺手订正。
- **b**：T8b 沿 T1a 先例注入 runner，真实 `resolveHumanApproval` 五要素路径由 gate 自检 + verifier 五态探针（§5）覆盖；如后续批动 unlock 路径，建议把真实五要素调用纳入夹具。
- **c**：V1-④ `ensureGraphCacheSynced` 导出可 import 探针未落（导出在案 :741 静态可证）；merge-driver 无 import 守卫使 import 级探针不可行，属门判据与脚本形态的既有张力，留根观测。
- **d**：结构门本轮才首次全绿——本批落地线程当时只跑 proposal-consolidation-check 未跑 landing-fidelity-check 的流程缺口，建议入下批 retro（landing 步骤清单应含结构门实跑）。

### 7.4 终态结论

五前提全部满足、结构门 ok:true（baseRef=af0ac8e）、验证集独立复跑全绿（merge-driver 六套 + T8 = 20/20、gate 21/21、lib 探针、七消费方名册、V4 全套、T5 五态探针）、语义保真复确认（修复 diff 仅触五前提对应面，无新夹带；aa6ffd9 文件清单 = summary 改名 + PROPOSAL 锚行 + merge-driver T8 + 掉字 + 表头 + verdict 归档 + graph 缓存非 canonical 件）。**landing 保真度判定 GREEN，放行建议成立；合并（Track A human 门）与后续 Track A escalation 批启动归 human。**
