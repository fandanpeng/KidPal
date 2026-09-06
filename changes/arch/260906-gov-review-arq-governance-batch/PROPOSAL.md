# PROPOSAL · 260906-gov-review-arq-governance-batch

> **状态：FROZEN v1.0 · 2026-09-06 · 凭据 = human 立项 ack（「立项 review吧」2026-09-06）+ architect R1 NEEDS-REVISION → 主线程修订 → R2 GREEN（§7）+ OQ 四题全裁（OQ1=(a) / OQ2=(a) / OQ3=(b) / OQ4=(a)，human 2026-09-06）· R2 后 F13/F14/F15 三条 NIT 由主线程标 FROZEN 时顺手落字（判官授权，机械修订）**（前端对齐：为什么改 = 治理 ARQ 积压十项、其中三项直接挡下一个 dev-loop 包；往哪改 = 治理/工具卫生面（G1+G2+G3，R1-F10 标签修正），不触契约与实现面；改动多大 = 十 T、以「追认热修 + 补自检/提醒 + 两处流程纪律成文」为主，无新承重机制）。
> **承载 ARQ**：`ARQ-260904-gate-regex-uppercase-changeid`、`ARQ-260902-acceptance-coverage-python-assert`、`ARQ-260901-g3-server-live-acceptance`、`ARQ-260905-worktree-graph-index-rules`、`ARQ-260905-merge-driver-worktree-topology`、`ARQ-260906-live-graph-dirty-blocks-merge`、`ARQ-260906-shell-scripts-lf-gitattributes`、`ARQ-260906-live-acceptance-real-path`、`ARQ-260902-frozen-acceptance-never-run`、`ARQ-260902-shim-401-gate-bypass`（十个全结；不关的根见 §6 留根清单）。
> **review 轨**：Track B 常驻轻路径。Track A 判据核查：① 不触 `docs/contracts/**`；② 非跨系统结构性变更（纯治理/工具卫生面）——两判据均否，维持 Track B。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T10）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low——全部触点为治理流程/脚本卫生面；T6/T7 触 merge-driver 与构建入口但属修复型、行为向严；T3 明示行为不变只成文；无新承重闸、无数据迁移、无不可逆操作。
> **review profile**：G1+G2+G3——G1 = `docs/DEV-GOVERNANCE.md`（指令根成文节；R1-F10 标签修正，实质批准载体已在，仅成文）；G2 = `.gitattributes`（新增）；G3 = `.claude/scripts/**`（含 lib 新增、merge-driver、worktree-bootstrap、unit/acceptance-run-check、acceptance-coverage-check）、`.claude/hooks/supervisor-gate-check.mjs`、`.claude/agents/acceptance-author.md`（OQ3=(b) 触 agent def，行内引验收纪律两条 + host/live 约定节名指针；只加引用、不改其既有职责结构）。G0 契约面 = 零；spec 实现面 = 零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行已按硬项 2a 在场。

```landing-manifest
.claude/scripts/lib/changeid-shared.mjs（新增，T1）
.claude/scripts/closeout-check.mjs（改，T1）
.claude/scripts/devloop-artifact-check.mjs（改，T1）
.claude/scripts/dispatch-stamp.mjs（改，T1）
.claude/scripts/journal-append.mjs（改，T1）
.claude/scripts/loop-health-report.mjs（改，T1）
.claude/scripts/loop-metrics.mjs（改，T1）
.claude/scripts/taskpackage-required-check.mjs（改，T1）
.claude/hooks/supervisor-gate-check.mjs（改，T1）
.claude/scripts/acceptance-coverage-check.mjs（改，T2）
.claude/scripts/unit-run-check.mjs（改，T3）
.claude/scripts/acceptance-run-check.mjs（改，T3）
.claude/scripts/worktree-bootstrap.mjs（改，T4）
.claude/scripts/merge-driver.mjs（改，T5+T6）
.gitattributes（新增，T7）
docs/DEV-GOVERNANCE.md（改，T3+T8）
.claude/agents/acceptance-author.md（改，T8）
changes/dev/260831-cloud-link-bringup/review-packet.md（改，T9 append-only）
docs/ARCH-REVIEW-QUEUE.md（改，T10：十项移 §2 已结索引，landing 后随 verdict 执行）
```

---

## 0. 想实现的效果与差距

**目标效果**：在下一个 dev-loop 包 **包 2（`260829-voice-roundtrip`，语音回环 + display 消费，mergeOrder 5，hybrid host+PC+live）** launch 之前，把 `docs/ARCH-REVIEW-QUEUE.md` 当前工作队列中处理时机已到「下次 architect 治理变更批 / 下个包立项前」的十项 governance ARQ 全部清结，使包 2 的 launch 面（worktree 准备、验收起草、合并通道）不踩已知病灶。

**现状差距**：ARCH-REVIEW-QUEUE 现存十项 governance 相关未结项（实况分布 = §1.2 七行 + §1.3 watchlist 三行，R1-F2 已修正）：四项是 human 已指示落地、待 architect 后审的「一次性热修追认」（正则大写 / coverage Python 断言 / 图索引 rules / merge-driver 拓扑），两项待根治或新修（活图脏树撞 merge 通道 / WSL CRLF），四项流程待成文或复核（host/live 约定正式化 / probe≠exercise / 冻结即冒烟 / TP-A 复核注）。其中三项与包 2 直接相碰：T6（活图脏树撞断 merge 通道，包 2 收尾合并必踩）、T7（WSL host 验收跑 `.sh`，包 2 验收面含 host linux target）、T8（包 2 的 live 验收起草纪律）。

**非目标**：① 不改 `docs/contracts/**`、不改 `docs/spec.md`、不改任何自有实现面（firmware/server）；② 不处理 watchlist 其余触发式保留项（§1.3 现十行、本批结其中三项、余七项，触发条件未到）；③ 不处理外因阻塞项（ARQ-260904-v23-asr-swap-quota-recheck，token-plan 配额 2026-09-07 重置 + human 拍板，非本批）；④ 不新立承重闸（verify-gates.json 两门案不动）。

## 1. 问题与根因（地面真值）

**问题类 A：热修已落地、缺追认与固化（T1/T2/T4/T5）**。四个 ARQ 均系 human 指示「直接热修、登记 ARQ、下次治理批追认」形态（先例 ARQ-260904-gate-regex-uppercase-changeid 同款），热修代码已随各包合并入 main，残余风险是「约定只存在于本次补丁、无单源/无自检/无提醒，下次编辑可静默回腐」。

- T1 证据（2026-09-06 实测）：`grep -rln "CHANGE_ID_PREFIX_RE" .claude/scripts/` 命中 7 脚本（closeout-check / devloop-artifact-check / dispatch-stamp / journal-append / loop-health-report / loop-metrics / taskpackage-required-check），各文件内正则均为 `/^(\d{6})-[a-zA-Z0-9-]+$/` 的**本地拷贝**（如 `closeout-check.mjs:89`、`dispatch-stamp.mjs:31`）；`.claude/hooks/supervisor-gate-check.mjs:58` 亦有一份本地拷贝（ARQ 原文记「8 个闸脚本」，精确实况 = 7 脚本 + 1 钩子）。四笔热修 commit `fee5627`/`ccf5a12`/`b7aa3df`/`fdbe8c2` 经 `git cat-file -t` 核实存在（2026-09-06）。根因 = 正则 8 处拷贝无单源，本次大写误判（`260903-gateway-directionB` 被误判 pre-cutoff）正是拷贝漂移的实例。
- T2 证据（2026-09-06 实测）：`acceptance-coverage-check.mjs:73-81` 的 `hasRealAssertion` 已含 Python unittest 分支（`:81` `\.assert[A-Z]\w*\s*\(`），系 2026-09-02 human 批准的一次性追加，无正反例自检兜底。
- T4 证据（2026-09-06 实测）：`worktree-bootstrap.mjs:13-15` 的 worktree 专属图索引约定仅存源码注释，脚本无任何运行时输出提醒；rules 行消歧热修（2026-09-05）已落 `repo-structure.md`，但「launch 面漏建专属索引无门报警」的残余未解。
- T5 证据（2026-09-06 实测）：`merge-driver.mjs` 现有自检入口 `--self-check-retro-advisory`（`:820`）/`--self-check-quick-checks`（`:824`）/`--self-check-detect-changes`（`:828`）+ rebase-target 自检（`:556-607`），**无**覆盖「无 origin 跳过 fetch」与「定位持有 main 的 worktree 执行合并」两步（拓扑分支无用例，ARQ 原文记，本批 grep 复核在册）。

**问题类 B：活图持续写脏被跟踪文件，merge 通道 fail-closed（T6，包 2 合并必经）**。证据（2026-09-06 实测）：`git ls-files .codebase-memory/` = `.gitattributes`/`artifact.json`/`graph.db.zst` 三件被跟踪；`git ls-files -v .codebase-memory/` = `S .codebase-memory/artifact.json` / `S .codebase-memory/graph.db.zst`（S = skip-worktree 已设，即 2026-09-06 WS 包合并时的人工权宜）；`.gitattributes` 内 `binary merge=ours` 只决冲突内容、管不到 rebase/checkout 的净树要求。根因 = CodebaseMemory 索引器（主 checkout 与各 worktree 专属进程）持续重写这两件被跟踪文件，git 树常驻脏，机械合并的 rebase/merge 步无条件 fail-closed；每个后续 Pod（含包 2）合并都会复发，且「合并前人工对两个 worktree 各打一次 skip-worktree」靠人记得。

**问题类 C：WSL 读 `.sh` 撞 CRLF（T7，包 2 host 验收面）**。证据（2026-09-06 实测，R1 判官复核五行在册）：`git ls-files --eol '*.sh'` 五件（`firmware/main/test/host_app/espidf_wsl.sh`、`firmware/scripts/mp3_to_ogg.sh`、`server/deploy/apply-xiaozhi-overlay.sh`、`server/deploy/cert-renew.sh`、`server/deploy/cert-warn-expiry.sh`）全部 `i/lf w/crlf`——**入库侧已是 LF**，病灶在 Windows checkout 换行转换（autocrlf）把工作树文件转成 CRLF、WSL bash 读到即 `$'\r': command not found`（WS 包 2026-09-06 实测，绕行 = `sed 's/\r$//'` 生成 LF 临时副本）；根级 `.gitattributes` 不存在（`cat .gitattributes` 空输出实测）。根因 = 无 `eol=lf` 属性钉死，换行行为随提交者机器配置漂移。注：ARQ 原文「以 CRLF 入库」表述与实况有出入（实况 = 入库 LF、工作树 CRLF），按硬项 1a 以本批实测为准修正，不改 ARQ 行事实结论（修复动作一致）。

**问题类 D：验收纪律两处未成文（T3/T8，包 2 验收起草面）**。

- T3 证据（2026-09-06 实测）：`@pytest.mark.live` 跳过逻辑已落在执行器——`unit-run-check.mjs:184-189` 与 `acceptance-run-check.mjs:180-185` 的 `executor-skipped` 路径（「live on-target 手动触发；对偶固件 on-target 结构闸」），系 2026-09-01 human 授权快速补的**暂用约定**；host/live 判定约定（标记 vs 目录、pytest-not-installed fail-open 语义）无成文，触发条件「第二个 live 验收需求」已出现（G 包 ECS 部署面 / WS 真机冒烟 / 包 2 遵循度复验均用暂用约定）。
- T8 证据：G 包 live 验收当年只「探测可达」未「真跑全链」，六个真实链路缺陷（websockets v17 不兼容、缺 device-id 头透传、TTS type 拼写、OpenClaw base_url `/v1`、容器 loopback、Intent 拒绝）全留到 WS 包真机冒烟连环爆（ARQ-260906-live-acceptance-real-path）；另「冻结后从未执行」形态两度致缺陷——TP-A 先例 + smoke-e2e 包 live 线束 3 缺陷（ARQ-260902-frozen-acceptance-never-run）。

**问题类 E：TP-A 制品结论与实况不符（T9）**。证据：`changes/dev/260831-cloud-link-bringup/` 在册（`ls` 实测：cloud-link-validation-report.md / docs-delta.md / pod-retro.md / review-packet.md）；其「无 token → 401」实测结论被 WS 包证伪（`require_device` 的 `return JSONResponse` 不短路缺陷，已修并复验），根因 = TP-A 验证批未跑机械轮、该「实测」未经真机复验，制品缺真实性复核注。

## 2. 改法（T1…T10）

**T1 · changeId 正则收敛单源（ARQ-260904-gate-regex-uppercase-changeid）**。新建 `.claude/scripts/lib/changeid-shared.mjs`，导出 `CHANGE_ID_PREFIX_RE` 与 changeId 前缀校验函数（承 ARQ-097「短 id 提取收敛单源」先例：`lib/acceptance-id-shared.mjs` / `lib/governance-gate-shared.mjs` 同款形态）；8 处本地拷贝全部改为 import、本地 const 删除；同步改写 `loop-metrics.mjs:17` 注释——现称「复用 closeout-check.mjs 的 CHANGE_ID_PREFIX_RE / stripPrefix / validDatePrefix」与实况不符（三者皆本地拷贝，R1-F7），收敛后指向 `lib/changeid-shared.mjs` 单源，并注明 `stripPrefix`/`validDatePrefix` 仍留本地拷贝、是否二轮收敛留后续（本批不收，防范围蠕变）。为什么这样改：直接消灭「8 拷贝漂移」根因，本次大写误判即该类漂移的实例；不新立闸（verify-gates 两门案不动），固化靠单源 + 下述自检。
目标文件：`.claude/scripts/lib/changeid-shared.mjs`（新增）；`.claude/scripts/closeout-check.mjs`、`.claude/scripts/devloop-artifact-check.mjs`、`.claude/scripts/dispatch-stamp.mjs`、`.claude/scripts/journal-append.mjs`、`.claude/scripts/loop-health-report.mjs`、`.claude/scripts/loop-metrics.mjs`、`.claude/scripts/taskpackage-required-check.mjs`、`.claude/hooks/supervisor-gate-check.mjs`（各删本地 const、改 import；loop-health-report 的 `CHANGE_ID_PREFIX_RE_LHR` 别名一并消除）。

**T1b · 收敛自检**。`lib/changeid-shared.mjs` 自带 `--self-check`：混合大小写正例（`260903-gateway-directionB`）必命中、纯小写正例必命中、反例（`260903-Gateway_DirectionB` 含下划线 / 无日期前缀）必不命中。为什么：把「门须接受混合大小写 changeId」从热修追认固化为可机判断言。
目标文件：同 T1（自检内嵌新模块）。

**T2 · acceptance-coverage Python 断言识别固化（ARQ-260902-acceptance-coverage-python-assert）**。给 `acceptance-coverage-check.mjs` 新增 `--self-check`（该脚本现无自检入口，R1 实证）：正例两支——Python unittest 风格（`self.assertIn(`）必识别、行首裸 `assert` 必识别；反例两支钉死 **prose 形态**——`# 断言风格说明：assertEqual 须在测试内调用`（注释 prose）、`"本次 assert 校验未通过"`（字符串 prose，均不含调用形态 assert 子串）必不识别；空文件必报「无可识别断言」。**已知边界显式声明（R1-F4 收束，不扩面）**：现行 `hasRealAssertion` 是纯正则、不剥注释/字符串，凡调用形态字样（如字符串 `"assert(x)"`、注释 `# self.assertIn(a, b)`）会被命中为「有真断言」——这是既有实现语义，本批不改（要剥注释 = 门语义变更，须另行走治理批 + human）；自检反例只覆盖 prose 形态，如实标注边界防误判「自检万能」。行为零变更，只加自检。为什么：该热修同样是无单源断言识别的补丁形态，自检防回腐。
目标文件：`.claude/scripts/acceptance-coverage-check.mjs`（改）。

**T3 · server 验收 host/live 约定正式化（ARQ-260901-g3-server-live-acceptance）**。行为零变更，两处成文：① `docs/DEV-GOVERNANCE.md` 新增 **`## 验收测试纪律`** 节（**节名 FROZEN 前钉死**，T8 与 acceptance-author def 引用同一名，R1-F11）：server 面验收分 host-fast 与 live on-target 两态；live 态以 `@pytest.mark.live` 标记、unit/acceptance 两闸执行器只结构闸不 auto-run（对偶固件 on-target）；pytest-not-installed 在 host-fast 路径按 fail-open 跳过并 warn（明示语义，当前行为即此，成文防误「修复」）；② `unit-run-check.mjs:184-185` / `acceptance-run-check.mjs:180-181` 行内注释补「`## 验收测试纪律`（docs/DEV-GOVERNANCE.md）」节名指针（R1-F5 修正：脚本无 `_doc` 字段——`_doc` 在 verify-gates.json、本批不动——注释中亦无「暂用约定」字样，改法 = 补指针非改措辞）。为什么：触发条件已出现（第三个 live 验收需求在用行内注释级约定），约定此前只活在两闸脚本的行内注释里。
目标文件：`.claude/scripts/unit-run-check.mjs`（改）、`.claude/scripts/acceptance-run-check.mjs`（改）、`docs/DEV-GOVERNANCE.md`（改）。

**T4 · worktree-bootstrap 图索引运行时提醒（ARQ-260905-worktree-graph-index-rules 残余）**。`worktree-bootstrap.mjs` 完成输出末尾追加一行提醒：「下一步（launch 面必办）：为 worktree 建专属图索引 `index_repository(repo_path=<worktree>)`；先 `list_projects` 核既有项目，不得降级读文件（规则见 .claude/rules/repo-structure.md `.codebase-memory/` 行）」；顺手刷新 `:14-17` 头注释——「本仓尚未 commit 快照」句已陈旧（快照 b7aa3df 2026-09-05 已入仓，R1-F12），按实况改为「快照已入仓，bootstrap 后跑一次 `index_repository(repo_path=<worktree>)` 即从快照导入 + 增量补差」。为什么：约定本体在 rules 已消歧，但 bootstrap 是 launch 面必经动作、却无产出面提醒——加一行输出是最便宜的机械提醒，不新立闸。旧 launch prompt 文件的过期降级注记不在本批清扫（属 `.git/devloop/launch/` 历史件，随下次 launch prompt 重写自然更替，留根登记）。
目标文件：`.claude/scripts/worktree-bootstrap.mjs`（改）。

**T5 · merge-driver 自检补 worktree 拓扑覆盖（ARQ-260905-merge-driver-worktree-topology 残余）**。merge-driver 新增 `--self-check-worktree-topology`：以临时 git 仓库夹具（脚本自建、跑完即删）断言两步语义——① 无 origin remote 时 fetch 步跳过、rebase 目标解析仍落本地 main（不 fail-closed）；② `git worktree list --porcelain` 解析能正确定位持有 main 的 worktree。为什么：这两步是 2026-09-05 human 裁定热修的核心语义，现拓扑分支无用例，self-check 假绿空间属实（本次 fail 模式已有两次执行实证）。
目标文件：`.claude/scripts/merge-driver.mjs`（改）。

**T6 · merge-driver 对 `.codebase-memory/` 漂移自动 skip-worktree（ARQ-260906-live-graph-dirty-blocks-merge 根治）**。merge-driver 在 rebase 步（Pod worktree 内）与 merge 步（main 持有者 worktree 内）之前，对 `CODEBASE_MEMORY_DIRTY_PATHS = ['.codebase-memory/graph.db.zst', '.codebase-memory/artifact.json']` 幂等执行 `git update-index --skip-worktree`（先 `ls-files -v` 查标记、已设则跳过），成功路径 stderr 留痕一行；不 unset（活索引器持续重写，unset 即复发）。**DRY_RUN 交互钉死（R1-F9）**：该步只动本地 index 标记、不改历史、幂等可重入，故**置于 `if (!DRY_RUN)` 守卫之外、DRY_RUN 下照常执行并留痕**——§5.8 的 dry-run 验证路径因此可真实覆盖该步，不空转。为什么选此形态承 OQ1。根治后人工权宜撤除：`git update-index --no-skip-worktree` 两文件后重设一次（由本批验证步骤执行，验证自动重设生效）。`.gitattributes` 的 `binary merge=ours` 保留不动（仍消二进制冲突）。
目标文件：`.claude/scripts/merge-driver.mjs`（改）。

**T7 · 根级 `.gitattributes` 钉 `*.sh text eol=lf`（ARQ-260906-shell-scripts-lf-gitattributes）**。新建根级 `.gitattributes`（首行 `*.sh text eol=lf`，唯一规则）；步骤序列按 git 机制钉死（R1-F1 修正——`git add --renormalize` 只按新属性重算 index、**不改工作树字节**，必须补显式刷新步）：① 属性文件入库 → ② `git add --renormalize .`（重算 index，五件 `.sh` index 实测已是 LF、此步幂等无 diff）→ ③ `git checkout -- '*.sh'`（按新 index 把工作树五件从 CRLF 刷新回 LF）。为什么：从 checkout 侧消灭 CRLF，WSL bash 直读直跑；属性入库后对全部克隆者/提交者一致生效，不依赖个人 autocrlf 配置。范围克制在 `*.sh`（仓内 Python 走 ECS/SSH 执行不踩本地工作树，不扩面）。附记（R1 已排除两疑点）：根级属性只含 `*.sh` 一行时 renormalize 对其他后缀零效果、不触发无关 diff；`.codebase-memory/.gitattributes`（merge=ours）作用于其子树路径，与根级 `*.sh` 规则无路径交集、共存无冲突。
目标文件：`.gitattributes`（新增）。

**T8 · 验收纪律两条成文（ARQ-260906-live-acceptance-real-path + ARQ-260902-frozen-acceptance-never-run）**。`docs/DEV-GOVERNANCE.md` 的 **`## 验收测试纪律`** 节（节名同 T3 钉死）新增两条分目，并在 `acceptance-author.md` agent def 行内引用（OQ3=(b)）：① **probe ≠ exercise**——跨进程包的 live 验收必须真 exercise 一次真实端到端路径（最小 tracer-bullet 即可），只探测可达不算 live 验收；② **冻结即冒烟**——冻结验收落盘时 acceptance-author 当场执行一次线束冒烟：live 面真连一次 SSH 可达性、线束助手（连接/读写/payload）真调一次、pytest 收集零错误。判据 = **执行通道 error 为零**（error ≠ failure：收集错/夹具崩/线束异常是线束缺陷，当场修）；断言 failure 是冻结相位的设计态（实现未建、红基线），不阻塞冻结。为什么这样定界：历史病灶（`_ssh` 解包、payload 上限）全部落在线束层、**不需要被测实现存在**就会炸，冒烟对象 = 线束通道而非断言转绿；「跑必然失败」的误解由 E/F 二分消解。两形态各有两度实证（见 §1 问题类 D），包 2 起草验收前必须成文；agent def 引用让起草 agent 不经 planner 中转也可见。
目标文件：`docs/DEV-GOVERNANCE.md`（改）、`.claude/agents/acceptance-author.md`（改，只加两条纪律引用 + T3 host/live 节名指针，不改其既有职责/工具面结构）。

**T9 · TP-A 收口制品补真实性复核注（ARQ-260902-shim-401-gate-bypass）**。`changes/dev/260831-cloud-link-bringup/review-packet.md` **append-only** 追加一段复核注：原「无 token → 401」结论系验证批未跑机械轮、未经真机复验，已被 WS 包证伪并修复（`require_device` 短路缺陷），以 WS 包复验结论（无 token→401 零转发 / 错 token→401 / 带 token 空文本→400）为准。为什么：历史制品的失效结论要原位标注，防下游引用错源；append-only 不动既有行。
目标文件：`changes/dev/260831-cloud-link-bringup/review-packet.md`（改，append-only）。

**T10 · ARQ 队列结项（T1–T9 landing 后随 verdict 执行）**。`docs/ARCH-REVIEW-QUEUE.md`：本批十项移 §2 已结索引（**实况分布 R1-F2 修正**：§1.2 移 7 行 = gate-regex-uppercase / worktree-graph-index-rules / merge-driver-worktree-topology / live-graph-dirty-blocks-merge / shell-scripts-lf-gitattributes / live-acceptance-real-path / frozen-acceptance-never-run；§1.3 移 3 行 = g3-server-live-acceptance / acceptance-coverage-python-assert / shim-401-gate-bypass），§2 按统一格式各加一行（ID / title / landing 日期 / evidence 指向本 change 目录）。为什么：Completion 定义 = landing 完成 + landing-verifier 绿 + retro 齐（AGENTS.md）；ARQ 已结规则 = human 批准且 landing 完成即移 §2。
目标文件：`docs/ARCH-REVIEW-QUEUE.md`（改）。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 闸/脚本 | `lib/changeid-shared.mjs` + 8 消费方（T1/T1b） | 新增 + 改 | 消费方 = closeout-check / devloop-artifact-check / dispatch-stamp / journal-append / loop-health-report / loop-metrics / taskpackage-required-check / supervisor-gate-check 钩子；均只换 const 来源、判定语义不变；消费方均带 `--self-check` 或主流程可冒烟 |
| G3 闸/脚本 | `acceptance-coverage-check.mjs`（T2） | 改 | 消费方 = acceptance 闸 + docs-sync-check（import 同 lib）；只加自检、判定语义不变 |
| G3 闸/脚本 | `unit-run-check.mjs` / `acceptance-run-check.mjs`（T3） | 改 | 消费方 = unit/acceptance 两闸执行器（merge-driver quick-checks 调 contract-lint/acceptance 两门案调 acceptance-run-check）；行内注释补「`## 验收测试纪律`」节名指针（脚本无 `_doc` 字段、verify-gates.json 不动），执行路径零变更 |
| G3 闸/脚本 | `worktree-bootstrap.mjs`（T4） | 改 | 消费方 = launch 面（主线程/Pod 准备）；追加完成提醒行 + 刷新 `:14-17` 陈旧头注释（快照已入仓），bootstrap 行为不变 |
| G3 闸/脚本 | `merge-driver.mjs`（T5/T6） | 改 | 消费方 = supervisor 合并门（merge-driver 由 supervisor 收口链路调用；merge-driver 内部 Step 1b 调前置升级闸，merge-driver.mjs:633，R1-F6 方向修正）；T5 只加自检入口；T6 在两 git 步前加幂等 skip-worktree（置于 DRY_RUN 守卫外、本地索引标记、非门语义变更）；rebase 目标解析 / 升级闸 / quick-checks 清单均未动 |
| G2 仓库卫生 | `.gitattributes`（T7） | 新增 | 消费方 = 全仓克隆者/提交者的 checkout 换行行为；只钉 `*.sh`，不动其他后缀（五件在册） |
| G1 指令根 | `docs/DEV-GOVERNANCE.md`（T3/T8） | 改 | G 层注册表 G1 = AGENTS.md / CLAUDE.md / docs/DEV-GOVERNANCE.md（R2-F15 核正，原误标 G2）；纯成文 |
| G2 角色定义 | `.claude/agents/acceptance-author.md`（T8） | 改 | G 层注册表 G2 = `.claude/agents/**`（R2-F15 核正）；OQ3=(b)：行内加两条验收纪律引用 + 「`## 验收测试纪律`」节名指针；只加引用、不改职责/工具面结构；消费方 = 全部 Pod 的 acceptance-author 派生实例（经 planner 派生时继承 def） |
| G4 证据与队列 | `docs/ARCH-REVIEW-QUEUE.md`（T10） | 改 | G 层注册表 G4 = 证据与队列；ARQ 登记动作归主线程（human-approved 2026-07-28，ARQ-260728 收口）；十项移 §2 |
| G2 归档制品 | `changes/dev/260831-cloud-link-bringup/review-packet.md`（T9） | 改 | changes/dev 证据归档面（不在 G 层注册表，主线程登记收口同 ARQ 例）；append-only 复核注 |
| 契约面 / spec / 实现面 | — | 未触 | 零触点 |

**sweep 透镜义务核对**：本批新立判定 pattern 两处——T1 的 lib 单源正则（消费方 8 处已机械枚举，见 §1 证据 grep 全输出）与 T7 的 `*.sh` 属性（覆盖集合 = `git ls-files '*.sh'` **五件**，R1 复核五行全 `i/lf w/crlf`，已枚举）。T6 的白名单路径固定两件（`.codebase-memory/graph.db.zst` / `artifact.json`），非模式匹配、无完备性问题。

## 4. human 取舍点（OQ）

**OQ1 · T6 根治形态**：human 2026-09-06 裁 **(a)** merge-driver 自动 skip-worktree 白名单（承 2026-09-06 已验证的人工权宜，两 worktree 实测放行合并，改为机械幂等执行；不动 b7aa3df 快照引导收益）；(b) 活图移出被跟踪面（`.gitignore` 两文件，丢仓内快照引导、worktree 退回全量建图）与 (c) 双管为落选项。

**OQ2 · T1 固化形态**：human 2026-09-06 裁 **(a)** 收敛单源 lib + 8 消费方 import（ARQ-097 同款先例在册，直接消灭 8 拷贝漂移根因）；(b) 仅加 census 自检脚本、不动 8 拷贝（漂移面保留）为落选项。

**OQ3 · T8（连带 T3）落点**：human 2026-09-06 裁 **(b)**——`docs/DEV-GOVERNANCE.md` 成文 + `acceptance-author.md` agent def 同步引用改（起草 agent 不经 planner 中转直接可见两条纪律 + host/live 节名指针；只加引用、不改 agent def 既有职责结构，触及面在 §3 sweep 与 review profile 显式列出）；(a) 只写成文为落选项（先成文跑一阵子、漏了再升的渐进路径被 human 否掉，直接要起草侧强制可见）。

**OQ4 · 批边界**：human 2026-09-06 裁 **(a)** 一批十项全做（全为 low riskTier 小件、owner 同为 architect、共享同一次 review/landing/verifier 开销）；(b) 拆两批为落选项。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**landing 前（执行侧自检，真空绿规约：自检断言输出含新增自检名）**：

1. `node .claude/scripts/lib/changeid-shared.mjs --self-check` → ok:true，输出含新增自检名 `changeid-prefix`（T1b 正反例）。
2. `node .claude/scripts/acceptance-coverage-check.mjs --self-check` → ok:true，输出含 Python 断言识别新用例名（T2；若该脚本自检形态为内嵌用例，断言其输出列名含新增用例）。
3. `node .claude/scripts/merge-driver.mjs --self-check-retro-advisory && node .claude/scripts/merge-driver.mjs --self-check-quick-checks && node .claude/scripts/merge-driver.mjs --self-check-detect-changes && node .claude/scripts/merge-driver.mjs --self-check-rebase-target && node .claude/scripts/merge-driver.mjs --self-check-worktree-topology` → 五个全 ok:true（T5 新入口在场；既有四自检〔R1-F3④ 补列 `--self-check-rebase-target`，恰 T5 邻域〕不回退）。
4. `grep -c "process.argv.includes('--self-check" .claude/scripts/merge-driver.mjs` → 预期 5（五个具名自检入口；R2-F14 收窄模式——裸 `grep "self-check"` 会命中函数定义/注释、无法机判「恰好五个」）。裸 `--self-check` 总入口不存在（裸旗标落入主流程真合并），禁入验证集。
5. `node .claude/scripts/closeout-check.mjs`（本仓现有包）→ ok:true 不崩（T1 换源后主流程回归）。
6. 消费方语法冒烟：`node --check` 对 T1 八件 + T2/T3/T4 各件 → 全过。
7. `grep -rn "CHANGE_ID_PREFIX_RE = " .claude/scripts/ .claude/hooks/` → 只剩 `lib/changeid-shared.mjs` 一处定义（八处本地拷贝清零，census 断言）。

**landing 后（核验侧）**：

8. T6 实证：主 checkout `git update-index --no-skip-worktree .codebase-memory/graph.db.zst .codebase-memory/artifact.json` 撤权宜后，跑一次 merge-driver **dry-run**（T6 的 skip-worktree 步置于 DRY_RUN 守卫外，R1-F9）→ `git ls-files -v .codebase-memory/` 两文件重回 S 标记、stderr 含 skip-worktree 留痕行。
9. T7 实证：`git ls-files --eol '*.sh'` **五件**全 `w/lf`（renormalize + `git checkout -- '*.sh'` 刷新步后）；WSL `bash firmware/main/test/host_app/espidf_wsl.sh --help`（或等效只读入口）→ 无 `$'\r'` 报错（live 验证；环境红先立基线如实记录）。
10. T4 实证：`node .claude/scripts/worktree-bootstrap.mjs`（任一既有 worktree）→ 完成输出含图索引提醒行。
11. T3/T8 实证：人读 DEV-GOVERNANCE「`## 验收测试纪律`」节 + 两闸行内注释节名指针指认一致 + `acceptance-author.md` diff 只增引用（两条纪律 + 节名指针），职责/工具面结构零改动。
12. T9 实证：人读 review-packet.md 复核注在尾部、既有行零改动（diff 只增不删）。
13. 结构门：`node .claude/scripts/landing-fidelity-check.mjs`（本 change）→ 绿（landing-manifest ≡ T 目标并集）。
14. T10 后：ARCH-REVIEW-QUEUE.md §1.2 余 5 行（multiturn-continuity / p4-stroke-dasharray / v23-asr-swap / protocol-version-pinned-constant / nvs-csv-wifi-injection）、§1.3 余 7 行；本批十项在 §2 已结索引各加一行（R1-F2 实况修正）。

## 6. 留根清单

**不关的根（触发式 / 外因 / 归口他批）**：

- `ARQ-260902-multiturn-continuity`——G 包 GWA-A8 部署期首验双探针（mergeOrder 3 已重钉），非治理批。
- `ARQ-260831-p4-stroke-dasharray-visual-check`——P4 包验收阶段触发。
- `ARQ-260904-v23-asr-swap-quota-recheck`——token-plan 配额 **2026-09-07 14:19 UTC** 重置后补 live 实测 + human 拍板引擎 ASR provider 换型；**时序提醒：距触发不足一日**，与包 2 launch 并行安排。
- `ARQ-260906-protocol-version-pinned-constant`——固件自有实现面设计变更（websocket_protocol/ws_provision），归下个动该面的 dev-loop 包。
- `ARQ-260906-nvs-csv-wifi-injection`——下个涉及配网/NVS 直注的包顺手查根因。
- watchlist 其余七项（contract-mech-gate-gap / simulator-lvgl-drift / simulator-truecolor / i1-indexed / small-cjk / openclaw-public / cert-lifecycle）——触发条件未到，维持 watchlist（本批从 §1.3 移 §2 的三项不在这七 ID 内，R2-F13 括注理顺）；其中 openclaw-public 的销账条件（18789 锁执行完成移已结）归 human/ECS 操作面，提醒跟进。

**交日落复盘项**：本批 retro 里核对「包 2 launch 是否确实未踩 T6/T7/T8 三病灶」（验证治理批的时序价值）；若踩，复盘病灶为何未在本批前暴露。

**登记不硬治项**：旧 launch prompt 文件（`.git/devloop/launch/`）中的过期降级注记不清扫，随下次 launch prompt 重写自然更替（T4 提醒 + rules 消歧已双保险）。

**自律件（不进 landing）**：包 2 acceptance-author 起草时引用 DEV-GOVERNANCE 新增两条纪律；包 2 launch 前 human 自跑 launch 命令（既有交付形态）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-06 · architect 判官 · 结论：NEEDS-REVISION

评审范围：硬项 1a/1b/1c/2a 逐项核对 + Track B 三要件与机检硬排除 + 技术合理性抽查 + §5 验证集逐条亲核 + §6 留根对照全队列。以下实证均为本判官 2026-09-06 亲跑/亲读。

**核对通过项（实证留痕，供主线程修订时对照）**：

- T1 拷贝枚举实测吻合：`grep -rn CHANGE_ID_PREFIX_RE .claude/` = 7 脚本本地拷贝（closeout-check.mjs:89 / devloop-artifact-check.mjs:70 / dispatch-stamp.mjs:31 / journal-append.mjs:32 / loop-health-report.mjs:165〔`_LHR` 别名在册〕/ loop-metrics.mjs:18 / taskpackage-required-check.mjs:95）+ 1 钩子（supervisor-gate-check.mjs:58），正则均为 `/^(\d{6})-[a-zA-Z0-9-]+$/`；无任何脚本 import 该符号（全为拷贝）。「7 脚本 + 1 钩子」精确。四笔热修 commit `git cat-file -t` 全存（fee5627/ccf5a12/b7aa3df/fdbe8c2）。
- T2 证据：`acceptance-coverage-check.mjs` `hasRealAssertion` 在 :73-84，`:81` `\.assert[A-Z]\w*\s*\(` 在场（另有 :82 裸 assert、:83 `.fail(`，共三支 Python 热修补丁，与 ARQ 原文「三支」一致）；该脚本**无** `--self-check`（grep 零命中），T2「既有自检（若在场）或新增」的条件分支取「新增」，可行。
- T5 证据：`merge-driver.mjs` 自检入口 :820/:824/:828 行号全对；rebase-target 自检函数 :548-609（PROPOSAL 记 :556-607，微差不影响结论）；「无 origin 跳过 fetch」（:661-679）与「定位持有 main 的 worktree 执行合并」（:761-770）两步骤无 self-check 用例——声明确认。
- T6 证据：`git ls-files .codebase-memory/` = 三件被跟踪；`git ls-files -v` = `H .gitattributes` / `S artifact.json` / `S graph.db.zst`（两文件 skip-worktree 已设在册）；`.codebase-memory/.gitattributes` 内容实测仅 `graph.db.zst binary merge=ours` 一行。双 worktree 覆盖设计正确：rebase 步在 Pod worktree（cwd=wt）、merge 步在 main 持有者 worktree（cwd=mergeCwd，:761-770），git 各 worktree 独立 index，两处分别幂等设置 = 两边都覆盖。
- T7 证据：`git ls-files --eol '*.sh'` 实测五行全 `i/lf w/crlf`；根级 `.gitattributes` 不存在（cat 空）。「入库 LF、工作树 CRLF」修正属实。
- T3 证据：`unit-run-check.mjs:184-189` 与 `acceptance-run-check.mjs:180-185` executor-skipped 路径在行（live-only 跳过 + pytest 未装 fail-open 两分支）。
- T9 证据：`changes/dev/260831-cloud-link-bringup/` 四件在册；`review-packet.md:69` 被证伪的「实测：无 token → 401（Bearer 已生效）」结论原文在场，append-only 复核注落点成立。
- 硬项 2a：每 T 末尾锚清单齐备；landing-manifest 19 行 ≡ 各 T 目标文件并集（逐一比对通过，含 T1b 内嵌自检不另占文件、T9 append-only、T10 随 verdict 执行均已分注）。
- 硬项 1b/1c：新立判定 pattern 两处（lib 单源正则、`*.sh` 属性）均有 census 枚举；T6 白名单固定两件非模式匹配。枚举可机械复跑（V7 grep census 断言设计成立）。
- Track B 三要件全 yes；机检硬排除逐项 no（不触 contracts、不改 verify-gates 门语义〔manifest 不含 verify-gates.json，实测两门案 cmd 未入锚〕、非 mixed lane、不触 L0、无 gate 新增删改）。lib 先例两件在册（`lib/acceptance-id-shared.mjs`、`lib/governance-gate-shared.mjs`，均带 `--self-check` JSON 输出形态，T1b 同款形态成立）；hooks import scripts 先例在册（planning-sync.mjs:12、supervisor-verdict-gate.mjs:57），supervisor-gate-check 改 import 不破坏独立可跑。

**Findings**：

- **F1（BLOCK）** T7 改法的 `git add --renormalize .` 机制错误：`--renormalize` 只按新属性重算 **index**（clean filter），**不改工作树字节**；而属性变更后工作树 CRLF 文件经 clean 得 LF 与 index 一致，git 视其为净、不会自发重写。按现改法 landing 后五件 `.sh` 在工作树仍是 CRLF，§5.9（`w/lf` + WSL 直跑）必红，目标效果（WSL 直读直跑）不达成。依据：git 属性/renormalize 语义 + 实测五件现 `w/crlf`。修订要求：T7 改法补显式工作树刷新步（如属性入库后 `git checkout -- '*.sh'`，或 `git rm -r --cached . && git reset --hard` 等价路径），并把该步写进 §2 T7 步骤序列；§5.9 保留作终验。附记（解答抽查疑点）：根级 `.gitattributes` 只含 `*.sh` 一行时 renormalize 对其他后缀零效果，不会触发无关 diff；`.codebase-memory/.gitattributes`（merge=ours）作用于其子树路径，与根级 `*.sh` 规则无路径交集，共存无冲突——此两点成立，仅刷新机制需修。
- **F2（BLOCK）** ARQ 队列区归属与实况不符：十项中三项（`ARQ-260901-g3-server-live-acceptance`、`ARQ-260902-acceptance-coverage-python-assert`、`ARQ-260902-shim-401-gate-bypass`）现位于 §1.3 watchlist，不在 §1.2（§1.2 现共 12 行，本批仅涉其中 7 行，结项后 §1.2 余 5 行）。故 T10「§1.2 十行移除」与 §5.14「§1.2 十行清零」均不可能成立，头部「承载 ARQ：…§1.2 十项」同误。修订要求：T10 改为「§1.2 七行 + §1.3 三行移 §2 已结索引」；§5.14 改为「§1.2 余 multiturn-continuity / p4-stroke-dasharray / v23-asr-swap / protocol-version-pinned-constant / nvs-csv-wifi-injection 五行、§1.3 余七行，十项在 §2 各加一行」；头部表述同步修正。
- **F3（BLOCK）** 硬项 1a 数字实证四处与实测不符：① `.sh`「四件」实为**五件**（§1 问题类 C、§3 sweep 透镜义务、§5.9 三处；实测输出五行：espidf_wsl.sh / mp3_to_ogg.sh / apply-xiaozhi-overlay.sh / cert-renew.sh / cert-warn-expiry.sh——§1 括号内本就列了五个路径却称四件）；② watchlist「六项」实为**七项**（§0 非目标②与 §6 均称六项，但 §6 括号列了七个 ID，§1.3 结项三项后实测余七行）；③ §0「另五项是流程纪律待成文」括号内只列四项（host/live 约定 / probe≠exercise / 冻结即冒烟 / TP-A 复核注），且 T7（shell-scripts LF）既不属「热修追认五件」也不属该括号，归类悬空；④ §5.3「既有三自检不回退」实为**四个**既有自检——`--self-check-rebase-target`（merge-driver.mjs:832）漏列，而它恰是 T5 改动邻域、最该纳入回归。修订要求：四处数字逐项改正并在 §1/§3/§5 留实测出处（命令 + 日期）。
- **F4（BLOCK）** T2 自检反例规格与「行为零变更」约束冲突：现行 `hasRealAssertion` 为纯正则、不剥注释与字符串字面量；凡**调用形态**的 assert 字样——如字符串 `"assert(x)"`（命中 :75 `\bassert\s*[.(]`）、注释 `# self.assertIn(a, b)`（命中 :81 `\.assert[A-Z]\w*\s*\(`）——即判「有真断言」。T2 现文「纯注释/打印文本（含 assert 字样的字符串与 # 注释）必不识别」在这类形状下不可能成立；要普遍成立就得给实现加注释/字符串剥离 = 行为变更，与同句「行为零变更，只加自检」自相矛盾。修订要求（二选一，建议前者）：把反例 fixture 钉死为 prose 形态（不含调用形态 assert 子串的具体字符串/注释样本，如 `# 断言风格说明：assertEqual 须在测试内调用`、字符串 `"本次 assert 校验未通过"`），并在 T2 显式声明反例只覆盖 prose 形态、调用形态字样在注释/字符串中的识别为既有实现语义不在本批改动；或改口承认实现微调并重新声明影响面与门语义变化。
- **F5（NIT）** T3 描述与实况两处偏差：① 两闸脚本内无 `_doc` 字段（`_doc` 在 verify-gates.json，而本批声明「verify-gates 两门案不动」、manifest 亦不含它）；② 脚本行内注释（unit-run-check.mjs:184-185 / acceptance-run-check.mjs:180-181）中并无「暂用约定」措辞（该词组只出现在 ARQ 队列文本）。修订要求：T3 改法表述改为「行内注释补 DEV-GOVERNANCE 新增节名指针」，删去「`_doc`」「从暂用约定措辞改为」两类失实指称；若本意确含 verify-gates.json `_doc`，则须补 manifest 锚并撤回「两门案不动」声明（建议不含）。
- **F6（NIT）** §3 sweep merge-driver 行消费方方向写反：实况是 merge-driver 在 processPod Step 1b 调 pre-merge-escalation-gate（merge-driver.mjs:633），非「pre-merge-escalation-gate 调 merge-driver」；闸自身注释 :8 亦云「它放行 merge-driver 才跑」。修订要求：改为「消费方 = supervisor 合并门（merge-driver 由 supervisor 收口链路调用）；merge-driver 内部调前置升级闸」。
- **F7（NIT）** T1 收敛后 `loop-metrics.mjs:17` 注释将再度失真：该注释现称「复用 closeout-check.mjs 的 CHANGE_ID_PREFIX_RE / stripPrefix / validDatePrefix」，实况三者皆为本地拷贝（注释今日已不准确）；T1 落地后正则来自 lib 单源，而 stripPrefix/validDatePrefix 仍留本地拷贝（T1 范围只收正则）。修订要求：T1 改法显式包含同步改写该注释——指向 `lib/changeid-shared.mjs` 单源，并注明 stripPrefix/validDatePrefix 仍本地、是否收敛留后续；防止「单源收敛」完成后注释与实现新一轮漂移。
- **F8（NIT）** §5.4「`--self-check`（若存在总入口）」实测总入口不存在：merge-driver main() 只认四个具名旗标（:820/:824/:828/:832），裸 `--self-check` 不命中任何分支、会落入主流程对 live board 执行真实 driver 逻辑（有 ready-to-merge Pod 时即真合并）。修订要求：删 §5.4，或改为「grep 断言四个具名自检入口在场」，避免 landing 期误跑。
- **F9（NIT）** T6 与 §5.8 的 DRY_RUN 交互未钉死：merge-driver 的 rebase/merge 步均裹在 `if (!DRY_RUN)` 内（:657/:751）；若 T6 的幂等 skip-worktree 步同样置于 DRY_RUN 守卫内，则 §5.8 的 dry-run 验证路径执行不到该步、验证空转。修订要求：T6 改法明示「幂等 skip-worktree 步不改历史、DRY_RUN 下也执行（或至少输出将执行项）」，或把 §5.8 改为真实触发路径（撤权宜后跑一次真实合并/dry-run-with-index-ops），二者选一钉死。
- **F10（NIT）** review profile 标签失真：`docs/DEV-GOVERNANCE.md` 在 G 层注册表属 G1 指令根（`.claude/agents/architect.md` Write Scope「G1 = AGENTS.md / CLAUDE.md / docs/DEV-GOVERNANCE.md」+ arch-review-protocol E1 同款列举），头部「G2 = .gitattributes、docs/DEV-GOVERNANCE.md」「G1/契约面 = 零」两处与之不符。实质 human 批准载体已在（立项 ack + OQ3=(b) 明示 DEV-GOVERNANCE 落点 + FROZEN 前人工复核），Track B 判据亦不因此升级，仅标签需与注册表一致。修订要求：profile 改标 G1+G2+G3，G1 触点附一句批准载体说明。
- **F11（NIT）** T3「落点节名 landing 时定」不宜悬空：T8 的 acceptance-author def 指针与 DEV-GOVERNANCE 节名必须同名，而 packet-binding 下 landing 只许落 FROZEN 内容——节名留到 landing 期定 = landing 期自由发挥 + 两处引用对不齐的风险。修订要求：FROZEN 前把节名钉死进 §2 T3（T8 与 agent def 引用同一名）。
- **F12（NIT）** `worktree-bootstrap.mjs:14-17` 注释已陈旧：「本仓尚未 commit 快照时，工作树索引仍由 launch 面同名建」——快照已入仓（b7aa3df，§1 T4 证据自引）。T4 追加提醒行后，完成输出（:146）与头注释口径一新一旧并存。修订要求：T4 顺手把该段注释按「快照已入仓、worktree bootstrap 后跑一次 `index_repository(repo_path=<worktree>)` 即从快照导入 + 增量补差」实况刷新。

**结论：NEEDS-REVISION**。F1–F4 为 BLOCK（修订后可由主线程直接回改 PROPOSAL，均无需 human 新裁——F1/F2/F3 是实证纠错、F4 建议取「反例钉 prose 形态」这一不扩面的收束；若主线程对 F4 改走「改实现」分支，则该分支触门语义声明、须回 human）。F5–F12 为 NIT，不升级 human。修订重点：T7 刷新机制、队列区归属与全部计数、T2 反例规格。修订后判官复核上述十二点即可，无需重跑全量实证（F1/F2/F3 的实况数据以本记录留痕为准）。

### R2 · 2026-09-06 · architect 判官 · 结论：GREEN

复核方式：人读 v0.2→v0.3 全量 diff，逐条核对 R1 十二点落位 + 修订面新矛盾扫描（manifest 稳定性、§0/§1/§3/§5/§6 计数与归类一致性、节名引用一致性）。未重跑全量实证（按 R1 约定以 R1 留痕为准）。

**R1 十二点落位核对（全过）**：

- F1 ✅ T7 步骤序列已按 git 机制钉死（§2 T7）：① 属性入库 → ② `git add --renormalize .`（只重算 index、幂等无 diff）→ ③ `git checkout -- '*.sh'`（工作树字节刷新）；R1 排除的两疑点（无无关 diff / `.codebase-memory/.gitattributes` 共存无冲突）作附记保留。§5.9 同步改为「renormalize + checkout 刷新步后五件全 w/lf」。
- F2 ✅ 队列区归属全修正：T10 = §1.2 移 7 行 + §1.3 移 3 行（十项逐名列出，与队列实况逐名比对一致）；§5.14 = §1.2 余 5 行（逐名）+ §1.3 余 7 行；§0 现状差距改「§1.2 七行 + §1.3 三行」；头部「承载 ARQ」行的「§1.2 十项」误述已除。
- F3 ✅ 四处数字全改：① 五件 `.sh`——§1 问题类 C、§3 sweep 透镜、§5.9、§3 T7 行（五件在册）四处一致；② watchlist 七项——§0 非目标②与 §6 均改七（但见 F13 措辞残余）；③ §0 重新分类 = 4 追认 + 2 根治/新修 + 4 流程成文（4+2+4=10，T7 归入「新修」，归类悬空消除），与 §1 问题类 A（T1/T2/T4/T5 四件追认）一致；④ §5.3 补列 `--self-check-rebase-target`，五条命令、「既有四自检不回退」。
- F4 ✅ T2 取收束分支（未走改实现、无需回 human）：反例两支钉死 prose 形态具体样本（`# 断言风格说明：assertEqual 须在测试内调用` / `"本次 assert 校验未通过"`），并显式声明「调用形态字样被命中 = 既有实现语义、本批不改、剥离注释属门语义变更另走治理批 + human」——规格内部自洽，「行为零变更」不再自相矛盾。
- F5 ✅ T3 改法删「`_doc`」「从暂用约定措辞改」两类失实指称，改「行内注释补节名指针」；§3 T3 行同步修正（「脚本无 `_doc` 字段、verify-gates.json 不动」）。
- F6 ✅ §3 merge-driver 行消费方方向改正（supervisor 收口链路调 merge-driver；merge-driver 内部 Step 1b 调前置升级闸 :633）。
- F7 ✅ T1 显式包含 loop-metrics.mjs:17 注释同步改写（指向 lib 单源 + 注明 stripPrefix/validDatePrefix 仍本地、二轮收敛留后续防范围蠕变）。
- F8 ✅ §5.4 改为 grep 断言五个具名自检入口、显式禁裸 `--self-check`（落点精度见 F14）。
- F9 ✅ T6 明示幂等 skip-worktree 步置于 `if (!DRY_RUN)` 守卫之外、DRY_RUN 下照常执行并留痕；§5.8 dry-run 验证路径因此真实覆盖、不空转，两处表述互锁一致。
- F10 ✅ review profile 改标 G1+G2+G3，G1 触点 = DEV-GOVERNANCE 成文节并注「实质批准载体已在」（但 §3 标签未同步，见 F15）。
- F11 ✅ 节名 FROZEN 前钉死 = `## 验收测试纪律`，T3/T8/§5.11/§3 T3 行四处引用同名，无第二形态。
- F12 ✅ T4 含 `:14-17` 陈旧头注释刷新（「快照已入仓，bootstrap 后跑一次 index_repository 即从快照导入 + 增量补差」）；§3 T4 行同步。

**manifest 稳定性**：landing-manifest 19 行与 v0.2 逐行一致、未变；F7/F12 新增动作均落在既有锚内文件（loop-metrics.mjs / worktree-bootstrap.mjs），锚并集不变，硬项 2a 完整性维持。

**R2 新 findings（均 NIT，不升级 human，不阻塞 GREEN；建议主线程标 FROZEN 时顺手落字，不改亦不影响 landing 正确性）**：

- **F13（NIT）** §0 非目标②括号内措辞自相矛盾：「七项，本批结项其中三项后余四项 + cert-lifecycle 等共七项维持」——「余四项」与「共七项」并立（实况 = 现十行、结三行、余七行）；§6 watchlist 行「本批结项**其中** g3-server-live-acceptance…三项后余七项」的「其中」误指（该三项不在所列七个 ID 内，所列七 ID 即结项后余留集合）。计数本身（七项）两处均正确，仅括注逻辑需理顺。修订建议：§0 改「watchlist 现十行、本批结项三行后余七行（ID 见 §6），触发条件未到维持」；§6 删「其中」或改「本批结项 g3-server-live-acceptance / acceptance-coverage-python-assert / shim-401-gate-bypass 三项后，上列七项余留」。
- **F14（NIT）** §5.4 的 grep 模式与预期不匹配：`grep -n "self-check" .claude/scripts/merge-driver.mjs` 会命中函数定义、调用点与注释等远多于五行的输出，「恰好五个具名自检入口」无法从该输出直接机判。修订建议：模式改 `grep -c "process.argv.includes('--self-check" .claude/scripts/merge-driver.mjs` → 预期 = 5（landing 后含新入口），机判精确。
- **F15（NIT）** F10 标签修正未收口到 §3：sweep 表 `docs/DEV-GOVERNANCE.md` 行仍标「G2 治理文档」，与头部 review profile 改标后的 G1 归层不一致（F10 修的正是这类标签失真，宜同批收齐）。附邻接观察（不挡）：同表 `docs/ARCH-REVIEW-QUEUE.md` 行标「G2 治理队列」，与 G4（证据与队列）归层的对齐可顺手一并核正。修订建议：§3 两行归层标签与 G 层注册表对齐。

**结论：GREEN**。R1 四项 BLOCK 全部消解且未引入新的目标效果风险；F13–F15 为措辞/精度/标签级 NIT，授权主线程在标 FROZEN 时顺手落字（属机械修订，不需判官第三轮；若主线程判断任一 NIT 需改设计再回判官）。后续序列：主线程凭「立项 ack + R2 GREEN」标 FROZEN（FROZEN 前人工复核结构字段硬条款：manifest 围栏块 + packet-id 行在场——本判官 R1/R2 两轮均已核在场）→ packet-bound landing → landing-verifier 独立核验 → retro → 前置升级闸放行合并。
