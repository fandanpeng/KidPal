# architect-landing-summary · 261026-gov-review-merge-gate-cluster-fix

packet-id：FROZEN PROPOSAL §2 改法集（T1–T8）+ §5 验证集（V0–V15） · 评审轮次 R1 BLOCK(F1,F2,F3)+NIT → R2 补获 F4 → R3 GREEN（GREEN 轮 = R3） · change-id 261026-gov-review-merge-gate-cluster-fix

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 契约凭据取件 source 优先 / target 兜底 + 两 ref 均不可读归因点名 | .claude/scripts/pre-merge-escalation-gate.mjs | hasArchLoopContractEvidence 取件循环改为 [source, target] 有序回退，refUsed / refError 进 checked 明细；reason 二分支（全 refError → 「两 ref 均不可读」） |
| T2 verdict 锚取终态（lastVerdictLine）+ verdictFormatMiss / refUsed 点名 | .claude/scripts/pre-merge-escalation-gate.mjs | 新增 lastVerdictLine 纯函数（后出者胜）；:149 首匹配改调它；checked 明细带 refUsed 恒在、verdictFormatMiss 条件在场 |
| T3 ledger 允许集 NOT_APPLICABLE（A/B/C 三段）+ AGENTS.md / SKILL.md 适用面句 | .claude/scripts/pre-merge-escalation-gate.mjs · AGENTS.md · .claude/skills/merge-to-main/SKILL.md | LEDGER_ARTIFACT_PREFIXES 五前缀导出；buildCtx 组装 diffPaths（段 A）；evaluate 序言声明 notApplicable（段 C）；unknown 分支 onlyLedger 判定（段 B）；return 带 notApplicable；CLI NOT_APPLICABLE 分支 exit 0；AGENTS.md step 7 与 SKILL.md 前置节各加一句适用面 |
| T4 契约 findings 列位判据（Findings 表 Kind 列前缀匹配） | .claude/scripts/pre-merge-escalation-gate.mjs | contractFindingsAccounted 行过滤改「表行 + Kind 列去 * 前缀 hard-violation + 字面在行内」 |
| T5 三条 lane INDEX union + DERIVED_STATE_CARVE_OUT + pod-coordination §10d 句 + SKILL.md 步骤 2/8 | .gitattributes · .claude/scripts/pre-merge-escalation-gate.mjs · .claude/references/pod-coordination.md · .claude/skills/merge-to-main/SKILL.md | .gitattributes 追加三条 merge=union；闸加 DERIVED_STATE_CARVE_OUT 并入 :318 carve-out；§10d 追加派生态句；SKILL.md 步骤 2 括号串追加 + 步骤 8 增全量重建行 |
| T6 enforce-scope 拓扑感知 + 夹具双拓扑 | .claude/hooks/enforce-scope.mjs | locateGitTree 按 gitdir 是目录/文件分支回推 commonDir（目录 ^2、文件先升父再 ^2）；夹具 pod-x 换真实拓扑、pod-y 旧拓扑兼容；新增 fixture-real-topology 与 sibling-legacy-file-topology-allowed 两臂 |
| T7 merge-driver 解析纯函数化 + 8 case 自检入口 + 注释追认 + UUID 注释 | .claude/scripts/merge-driver.mjs | normalizeDetectChangesOutput 导出纯函数、defaultDetectChangesCli 退化调用；新增 --self-check-merge-tooling-hotfixes（8 case，exit 1 传播）；两处「gov 侧待追认」改「追认入档 = ARQ-261014」；eventId 上方加 Windows UUID 教育注释（F2 承接，落 T7——merge-driver.mjs 是 T7 目标文件，FROZEN §2 与 §7 R2 已确认口径） |
| T8 闸假阴性兜底通道成文 | .claude/references/pod-coordination.md | §10e 追加四步受限兜底段（开放条件仅限假阴性） |

落地文件并集 = 7 项，与下方 landing-manifest 及 FROZEN PROPOSAL §2 锚并集逐条一致（V13 preflight 复核 anchorCount:7）。

## 2. 验证证据

主线程在落地盘上（gov worktree，改动在文件、未 commit）逐条实跑，输出关键行如下：

- **V1 闸自检**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `{"pass":66,"fail":0,"total":66,"ok":true}` exit 0（51 基线 + 15 新夹具：T1 2 / T2 2 / T3 6 / T4 3 / T5 2，与 §5 计数一致）。**判别力实例（诚实注记见 §3）**：T5 混撞夹具首轮实跑红（`overlap-projection-plus-other-tradeoff` got union-safe——firmware 件不在双侧改动交集被噪声滤清剔除），修正夹具让双侧同改两件后转绿；夹具在测真判据，不是照镜子。
- **V0b 钩子自检**：`node .claude/hooks/enforce-scope.mjs --self-check` → `ok:true`、**8 臂**（6 基线 + fixture-real-topology + sibling-legacy-file-topology-allowed）。
- **V9 真实树双探针 + 两护栏**：gov 树 builder 写主树 `firmware/probe.c` → exit 0；主树 landing-verifier 写 gov 树 review 目录 → exit 0（原钩子双例 exit 2，2026-10-19 事故同形）；护栏不退化——builder 写 `docs/contracts/**` → exit 2（deny 命中）、写仓外路径 → exit 2。
- **V10 热修自检**：`node .claude/scripts/merge-driver.mjs --self-check-merge-tooling-hotfixes` → `{"pass":8,"fail":0,"total":8,"ok":true}` exit 0（起草期反向已实测：target 声明改四空格 → 7/1 且 exit 1）。
- **V11 既有五入口**：retro-advisory 3/3、detect-changes 16/16、rebase-target 7/7、worktree-topology 9/9、merge-friction 23/23——与 V0c 基线逐一相同。
- **V5 census**：`contractFindingsAccounted` 遍历 89 份 review-packet → `{"packets":89,"hits":8}`（HEAD 基线 11 = 8 真表行 + 3 自述散文；3 散文全消，8 真行全保留）。
- **V6 属性**：`git check-attr merge -- delivery/review/INDEX.md delivery/taskpackages/INDEX.md delivery/intake/INDEX.md` → 三行均 `merge: union`（基线 unspecified）。
- **V2 / V3 / V4 / V4b / V7 / V12 起草期 fixture 仓以落地实现复跑**（cwd = fixture 仓，`--source-ref feat`）：r1-ref `PASS (changeId=261026-arch-review-fx, kind=gov)`；r2-verdict（FAIL→PASS 轨迹）`PASS`；r3-unknown `NOT_APPLICABLE (…kind=unknown): 变更集仅含 ledger 类制品（…）` exit 0；r8-req / r9-con 同 `NOT_APPLICABLE` exit 0；r10-retro 仍 `BLOCK … unknown change kind`（边界保守）；r6-projection `PASS (kind=devloop)`（派生态 carve-out 生效）；r7-fmt（261017 原形 verdict 无行首行）`BLOCK` 判定不变且已检查明细逐字含 `"refUsed":"feat"` 与 `"verdictFormatMiss":true`；r4-selfquote（自述无 finding 散文行）`PASS` 不再误拦。
- **V13 preflight**：`landing-fidelity-check.mjs --preflight` → `ok:true / anchorCount:7 / advisories:[] / findings:[]`，anchorUnion 7 项与 manifest 逐条一致。
- **authoring guards**（碰 SKILL.md 正文联动义务）：lint-descriptions exit 0、route-regression-check exit 0。

真空绿规约：T5 两夹具为函数级（直调 parallelOverlapProbe 注入 execFn），首轮实跑即抓出夹具自身判别力缺口并修正（见 §3）；未出现「断言恒真」形态。

## 3. 诚实注记

1. **V14 / V15 未跑**：两者为 post-commit 项（V14 = 全量复跑、V15 = 本批自身过闸），依赖落地 commit，归 landing-verifier / 合并前执行——本 summary 时点未发生，不预支绿。
2. **落地实现与原型的形态差异（一处）**：T7 8-case 自检的 case 8（退出码传播）落地形态 = 静态不变量断言（源码含 `process.exit(failCount ? 1 : 0)` 行），行为义务由本自检自身运行时兑现；起草期原型的反向实测（恒 exit 0 → 7/1 且 exit 1）覆盖同一风险面。该形态在 FROZEN §2 T7 改法 1 第 8 条的字面范围内（「落地实现须同样 propagate 退出码」），判官 R1–R3 未单独核过 case 8 落地形态（landing 后于核验官 A4 复核）。
3. **T5 混撞夹具修正留痕**：首轮 `overlap-projection-plus-other-tradeoff` 红（混撞件不在交集被滤），修正为双侧同改两件后绿——夹具判别力自证，非预期值放宽。
4. **F2 承接落 T7 非 T5**：Windows UUID 教育注释落在 merge-driver eventId 处（T7 目标文件内），与 FROZEN §2 T7 第 3 条逐字一致；§6 结项行与 ARQ-261014 拆出注记口径一致（本批消费 ①②③ + ④ 残余）。
5. **留根不关**（FROZEN §6 十项）：260924② 拆出留后批；ARQ-261026-final-phase-dispatch-contract-unwritten 已登记（主线程 2026-10-26）；input-brief 不入 carve-out、派生态集双源、INDEX 陈旧检测 convention-only、T4 ③ 未落 integrator.md、闸 cwd 语义、子模块拓扑、`--human-approval-ref` 对 gov/unknown 仍 FATAL（r3 fixture 带 flag 实测 FATAL 保持）、账本枚举边界（retro/archive/README 不入集）——各自复议触发在 FROZEN §6。
6. **ARQ 结项未执行**：六条 ARQ 的 status 翻转 = §6 结项段动作，归 landing-verifier 绿后主线程执行（本 summary 时点六条仍 deferred，防「先结项后核验」倒序）。

```landing-manifest
AGENTS.md
.claude/hooks/enforce-scope.mjs
.claude/references/pod-coordination.md
.claude/scripts/merge-driver.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/skills/merge-to-main/SKILL.md
.gitattributes
```
