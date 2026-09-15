# architect-landing-summary · 260906-gov-review-arq-governance-batch

packet-id：FROZEN PROPOSAL §2 改法集（T1–T10）+ §5 验证集 · 评审轮次 R1 NEEDS-REVISION → 主线程修订 → R2 GREEN · change-id 260906-gov-review-arq-governance-batch

> 落地人：主线程（Track B 步骤 4，packet-bound）· 日期：2026-09-06
> packet = FROZEN PROPOSAL + landing-manifest + 本 summary（architect-landing-summary）
> 凭据 = human 立项 ack（「立项 review吧」2026-09-06）+ OQ 四题全裁（OQ1=(a) / OQ2=(a) / OQ3=(b) / OQ4=(a)）+ architect R2 GREEN

```landing-manifest
.claude/scripts/lib/changeid-shared.mjs
.claude/scripts/closeout-check.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/journal-append.mjs
.claude/scripts/loop-health-report.mjs
.claude/scripts/loop-metrics.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/scripts/acceptance-coverage-check.mjs
.claude/scripts/unit-run-check.mjs
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/worktree-bootstrap.mjs
.claude/scripts/merge-driver.mjs
.gitattributes
docs/DEV-GOVERNANCE.md
.claude/agents/acceptance-author.md
changes/dev/260831-cloud-link-bringup/review-packet.md
docs/ARCH-REVIEW-QUEUE.md
```

> manifest 注：`docs/ARCH-REVIEW-QUEUE.md` 的 T10（十项移 §2 已结索引）按 PROPOSAL 约定留待 landing-verifier 绿后执行，属预期状态不算缺口（承 260905 批同约定先例）。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 changeId 正则收敛 lib 单源 | `.claude/scripts/lib/changeid-shared.mjs`（新增）+ 8 消费方（closeout-check / devloop-artifact-check / dispatch-stamp / journal-append / loop-health-report / loop-metrics / taskpackage-required-check / supervisor-gate-check） | ✅ 已落：本地常量删净、import 单源、自检 4/4、census 全仓仅剩 lib 一处定义 |
| T2 acceptance-coverage Python 断言自检 | `.claude/scripts/acceptance-coverage-check.mjs` | ✅ 已落：runSelfCheck 5 用例（2 正 3 负）、已知边界声明成文（R1-F4 收束：不剥注释/字符串、调用形态 assert 在注释/字符串中命中 = 既有语义，本批不改） |
| T3 host/live 约定成文 + 行内指针 | `docs/DEV-GOVERNANCE.md`（「## 验收测试纪律」§1，文件尾追加不扰既有节号）+ unit-run-check / acceptance-run-check 两闸行内注释指针 | ✅ 已落：行为零变更，只成文 + 指针 |
| T4 worktree 图索引提醒 | `.claude/scripts/worktree-bootstrap.mjs` | ✅ 已落：头注释刷新（快照 b7aa3df 已入仓实况）+ 完成输出后追加 launch 面必办提醒行 |
| T5 merge-driver 拓扑自检 | `.claude/scripts/merge-driver.mjs` | ✅ 已落：`--self-check-worktree-topology` 新入口（临时 git 夹具 5 断言）；`hasOriginRemote` / `findMainHolderWorktree` 提取为具名实现、主流程与自检共用同一逻辑（self-check 不复述、防假绿）；既有四自检零回退 |
| T6 活图漂移自动 skip-worktree | `.claude/scripts/merge-driver.mjs` | ✅ 已落：`CODEBASE_MEMORY_DIRTY_PATHS` 白名单两件 + `ensureSkipWorktree` 幂等实现（ls-files -v 查标记、已设跳过、成功路径 stderr 留痕一行、永不 unset）；rebase 步（Pod worktree）与 merge 步（main 持有者 worktree）两个调用点均置于 `if (!DRY_RUN)` 守卫之外（R1-F9）；`.codebase-memory/.gitattributes` 的 `binary merge=ours` 未动 |
| T7 根级 .gitattributes 钉 LF | `.gitattributes`（新增） | ✅ 已落：`*.sh text eol=lf` 唯一规则 + 五件 `.sh` 全部刷新为工作树 LF（步骤③等效替换见 §3 插曲 1） |
| T8 验收纪律两条成文 + agent def 引用 | `docs/DEV-GOVERNANCE.md`（「## 验收测试纪律」§2 probe≠exercise + §3 冻结即冒烟）+ `.claude/agents/acceptance-author.md`（References 节加一行行内引用） | ✅ 已落：agent def 只加引用、职责/工具面结构零改动 |
| T9 TP-A 复核注 | `changes/dev/260831-cloud-link-bringup/review-packet.md` | ✅ 已落：append-only 尾部追加「真实性复核注」节、既有行零改动 |
| T10 ARQ 十项移 §2 已结 | `docs/ARCH-REVIEW-QUEUE.md` | ⏳ 待 verifier 绿（PROPOSAL 约定时序） |

manifest 外零落地：契约面（`docs/contracts/**`）、`docs/spec.md`、自有实现面（firmware/server）均未触碰（PROPOSAL §0 非目标①）；verify-gates.json 未动（非目标④）。

## 2. 验证证据（§5 重放，2026-09-06）

landing 前（执行侧自检）：

- **V1** `node .claude/scripts/lib/changeid-shared.mjs --self-check` → `{"selfCheck":"changeid-prefix","pass":4,"fail":0,"total":4,"ok":true}` ✅（输出含新增自检名 changeid-prefix）
- **V2** `node .claude/scripts/acceptance-coverage-check.mjs --self-check` → `pass:5 fail:0 ok:true`（python-unittest-positive / python-bare-assert-positive / prose-comment-negative / prose-string-negative / empty-negative 全过）✅
- **V3** merge-driver 五自检：--self-check-worktree-topology `pass:5 fail:0 ok:true` + 既有四自检（retro-advisory / quick-checks / detect-changes / rebase-target）全 ok ✅（新入口在场、既有不回退）
- **V4** `grep -c "process.argv.includes('--self-check" .claude/scripts/merge-driver.mjs` → `5`（恰五个具名自检入口）✅；裸 `--self-check` 未入验证集 ✅
- **V5** `node .claude/scripts/closeout-check.mjs` → `ok:true` 不崩（T1 换源后主流程回归）✅（2 条 advisory 见 §3 插曲 4）
- **V6** `node --check` 对 T1 八件 + T2/T3/T4/T5/T6 各件（共 14 件）全过 ✅
- **V7** `grep -rn "CHANGE_ID_PREFIX_RE = " .claude/scripts/ .claude/hooks/` → 只剩 `.claude/scripts/lib/changeid-shared.mjs:7` 一处定义 ✅

landing 后（核验侧）：

- **V8** T6 实证：主 checkout `--no-skip-worktree` 撤权宜（两文件 S→H 实测）→ merge-driver `--dry-run --pod 260903-device-ws-switch`（该 pod 临时置 ready-to-merge、跑毕即恢复 merged）→ stderr 两行留痕：rebase 步 `2 set, 0 already flagged`（Pod worktree 内自动重设生效）、merge 步 `0 set, 2 already flagged`（幂等跳过实证）；Pod worktree `ls-files -v` 两文件回 S ✅。**拓扑插曲**：当前主仓无任一 worktree 持有 main（本批治理分支占主 checkout、KidPal-gov 在 gov/kidpal），merge 步按 Hot-fix #2 回退语义落在 Pod worktree，主 checkout 两文件标记由验证末段按 T6 同款命令手工恢复（S 在册）——T6 在「有 main 持有者」的真实合并拓扑下自动覆盖主 checkout，留待包 2 合并实证（§3 插曲 2）
- **V9** T7 实证：`git ls-files --eol '*.sh'` 五件全 `i/lf w/lf attr/text eol=lf` ✅；WSL live：`wsl -e bash -c "head -c 200 .../espidf_wsl.sh | cat -A"` → 行尾 `$` 无 `^M`，无 `$'\r'` 报错 ✅
- **V10** T4 实证：`node .claude/scripts/worktree-bootstrap.mjs --dry-run`（C:/AI/KidPal-pod-device-ws-switch 内）→ 完成输出含「下一步（launch 面必办）：为 worktree 建专属图索引 …」提醒行 ✅
- **V11** T3/T8 实证：DEV-GOVERNANCE「## 验收测试纪律」节在场（§1 host/live + §2 probe≠exercise + §3 冻结即冒烟）；两闸行内注释节名指针与之一致；`acceptance-author.md` diff 仅 +1 行（References 节引用行），职责/工具面结构零改动 ✅
- **V12** T9 实证：review-packet.md diff 只增不删（+6/-1，其中 -1 = 原文件末行「无 trailing newline」因追加换行而重新发射、行内容零改动，既有行零改动），复核注节在尾部 ✅
- **V13** 结构门：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260906-gov-review-arq-governance-batch` → ok:false，**残 3 条 error 全部属已声明的可宽免类**（承 260905 批 landing-verifier「脚本已知局限，非真实失配」判例）：
  1. `§2 锚清单 ≢ manifest（仅 manifest 有：.gitattributes, changes/dev/.../review-packet.md）`——机械门 `CANONICAL_TEST` 只认 `.claude/`/`docs/`/`.planning/`/根三件套，不识仓根点文件与 `changes/**` 形态（本批两文件类首见）；PROPOSAL §2 T7/T9 锚行已按整裁腿形态如实登记缺口原因，两文件真实落点直列 manifest、git 改动集在册（staged A / modified），**人读 diff 担纲**（verifier §3 逐文件核对项）。
  2./3. `docs/ARCH-REVIEW-QUEUE.md` claimed/planned-not-landed——**T10 预期时序**（PROPOSAL 钉死 landing-verifier 绿后执行，同 260905 批 T3 判例），非缺口。
  其余结构判据全过：FROZEN 状态行在场 / 双侧 manifest 围栏块在场（19 路径逐字一致）/ packet-binding 行在场 / summary ⊆ PROPOSAL 零 scope creep / `gitDegraded:false`。
- **V14** ⏳ T10 后执行（verifier 绿后）：§1.2 余 5 行、§1.3 余 7 行、十项进 §2 已结索引

## 3. 诚实注记

1. **T7 步骤③机械等效替换（如实登记）**：钉死序列③ `git checkout -- '*.sh'` 在本机为空转——renormalize 后五件 index 与工作树经规范化比较被判 clean，checkout 跳过重写（实测字节仍 CRLF）。等效生效路径 = 逐件 `rm` + `git checkout -- <file>` 强制重写（五件全回 LF，V9 实证）。**终端状态与 PROPOSAL 钉死终端状态逐字一致**（`w/lf` + `attr/text eol=lf`），仅到达路径因 git stat 优化改用强制重写；判定「落法 == PROPOSAL」以终端状态 + 机制等价为准，请 landing-verifier 逐字复核。
2. **V8 拓扑插曲**：见 §2 V8。主 checkout 两文件标记当前为手工恢复（S 在册）——这不是 T6 缺口：T6 的自动化在「存在 main 持有者」的合并拓扑下覆盖主 checkout（merge 步调用点即为此设），当前 transient 拓扑（main 无持有者）下 merge 步按 2026-09-05 human 裁定回退语义落 Pod worktree，与热修前行为一致。包 2 真实合并时拓扑必有 main 持有者，T6 将自动覆盖。
3. **T2 已知边界**：hasRealAssertion 为纯正则、不剥注释/字符串——调用形态 assert 字样在注释/字符串中被命中为「有真断言」系既有实现语义，本批未改（要剥注释 = 门语义变更，须另行走治理批 + human，R1-F4 已声明）。自检反例因此只钉 prose 形态。
4. **V5 advisory 枚举**（closeout-check 输出，2 条，均 advisory-only 不产 fail）：① ws-switch 包 machine evidence missing（verdict.json / verification.json）；② loop-retro 触发阈值达（pending 6 + waiting 2 = 8 ≥ 3），主线程应发起 /loop-retro。与本批改法无因果（均为在册既有态），如实登记不代处理。
5. **FROZEN PROPOSAL 格式修复 + 机械门缺口登记（如实登记，语义零变更）**：① PROPOSAL §2 十个 T 标题由 `**T<n> · …**` 粗体段形态改为 `### T<n> · …` 闸可机辨形态（T1b 折入 T1 节内为粗体子项、锚行改整裁腿「目标文件：无（…）」），正文零字改动——landing-fidelity 闸 `parseSection2AnchorLists` 只认 `### T<n>` 标题，冻结时 R2 评审未跑此闸（闸属 landing 相位机械门），格式与闸契约不符属 drafting 缺陷，修复使其符合 FROZEN 文档自己声称的「结构字段硬条款：…已按硬项 2a 在场」；② 双侧 manifest 围栏块行由带括注形态（`.gitattributes（新增，T7）`）改为裸路径形态——`extractManifestBlock` 逐字取行不识括注；③ **机械门真缺口**：`CANONICAL_TEST` 不识仓根点文件（`.gitattributes`）与 `changes/**` 路径，首见（既往批无此类落点），T7/T9 锚行以整裁腿 + 原因声明登记、人读 diff 担纲（V13 第 1 条）；缺口根治（拓宽 CANONICAL_TEST/TARGET_PATTERNS 或闸增白名单参数）**超本批 frozen 范围，另行登记 ARQ**（主线程登记职责，ARQ-260728）——登记动作排入 T10 执行后同一收尾段、单独 commit 如实留痕，retro 留根清单同步记录。

## 4. 状态转移记录

- 2026-09-06 human 立项 ack（「立项 review吧」）+ OQ 四题裁断（OQ1=(a) / OQ2=(a) / OQ3=(b) / OQ4=(a)）。
- 2026-09-06 architect R1 NEEDS-REVISION（F1/F2/F3/F4 BLOCK + F9 NIT 等）→ 主线程修订 → R2 GREEN（F13/F14/F15 NIT 修订在册，PROPOSAL §7）→ 主线程凭「立项 ack + GREEN」标 **FROZEN v1.0**（commit 9e71bed）；`changes-index.mjs --upsert` 钉 frozen。
- 2026-09-06 主线程 packet-bound 落地 T1–T9（§1 清单）→ §5 V1–V13 全绿（V14 待 T10）→ 本 summary 写就，交 landing-verifier 独立核验（Track B 步骤 5）。
- 待办（verifier 绿后）：T10 ARQ 十项移 §2 已结 → retro（步骤 6，含 PROPOSAL §6 交日项「包 2 是否踩 T6/T7/T8 病灶」核对义务）→ 前置升级闸（gov 档机锚 + off-main 校验）→ 自动合并本地 main（步骤 7；remote push 留 human）。
