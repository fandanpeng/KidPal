# review-r1-architect · 260915-gov-review-worktree-pool-reuse

> 判官 = architect（Track B 常驻轻路径，G2 + spec 面连带）。评审对象 = PROPOSAL.md DRAFT v0.4。
> 评审日期 2026-09-15。本件为 R1 全面审；R2+ 聚焦本件 findings 的修订到位性。
> 纪律声明：canonical 全程只读；唯一可写面 = 本 change 目录评审记录（本件）。一切事实断言均亲核（Read/Grep/Bash 实测），未采信 PROPOSAL 转述。

## Verdict

**BLOCK（F1 / F2 / F3 三条 BLOCKING；另有 F4–F6 SHOULD、N1–N2 NIT）** —— 方向不否：树池复用 + 一树一常驻分支 + 基线锚点判停的总体设计与 merge-driver / reconcile 的机械现实相容（问题 5 的分支非依赖断言逐条属实），但换包协议与 prompt 收缩各有一处会按字面执行出错 / 静默回退已落地门默认值的缺口，须修订后再审。无「影响目标效果」级方向性错误，无契约面 / 跨系统触面，不升级 human。

## 一、机械断言复核留痕（PROPOSAL §1 承重声明逐条）

| # | 断言 | 亲核结果 |
|---|---|---|
| 1 | `.gitignore:2` 含 `build/` | ✅ Read 实核：第 2 行 = `build/`。 |
| 2 | 主 checkout 有上批遗留热 build | ✅ `ls` 实测：`firmware/main/test/host_app/build/host_infra_test.elf` 存在（6,070,776 字节，2026-09-15 12:04）。 |
| 3 | `SKILL.md:56` / `:59` / `:14` verbatim | ✅ 逐字吻合（`git worktree add C:/AI/KidPal-pod-<suffix> -b dev/<changeId> main` 等）。另注意 `:60` = 「base = **当前 main HEAD**」同族过时措辞（验证集 V2 的 grep 能覆盖，不另立 finding）。 |
| 4 | `SKILL.md:43` 两态真值纪律措辞 | ✅ 逐字吻合；260909-gov-review-gate-truthfulness PROPOSAL T5 原文亲核确认只有「现跑 / 手写散文」两态、记录三要素 = 计数 + 命令 + 日期。 |
| 5 | `merge-driver.mjs:1411-1413` 读当前分支不认名 | ✅ 逐字吻合：1412 行 `const branch = execSync('git rev-parse --abbrev-ref HEAD', { cwd: wt, ... })`；1413 行 `if (branch === 'main' \|\| branch === 'master')` 仅跳过已在 main/master 情形。 |
| 6 | 升级闸 / reconcile 无 `dev/` 分支名依赖 | ✅ `grep -n 'dev/'` 两脚本均零命中（exit 1）。merge-driver.mjs 的 `dev/` 命中全部是 `delivery/changes/dev/` 路径语义。 |
| 7 | `planner.md` 的 `dev/<changeId>` 命中全是路径 | ✅ grep 命中 `:80/:85/:91` 三处，均为 `delivery/changes/dev/<changeId>/` 路径。 |
| 8 | `integrator.md:64/68` 为分支惯例唯一行为承载 | ✅ 全 `.claude` 扫 `dev/<changeId>`：除 integrator.md:64/68 与 launch-prep SKILL.md:14/56（T1/T4 改动面自身）外零命中；docs/ 命中仅在 ARCH-REVIEW-QUEUE 历史行（append-only 史料，非活承载）。 |
| 9 | `planner.md:35` changeId 裸起步自探测三处 | ✅ 亲核：taskpackage.json / devloop pods json / upstreamRef→PROPOSAL 三处探测在位。 |
| 10 | `portfolio-planner.md:50` launch 侧预填语义 | ✅ 逐字吻合（「worktree 路径记录由 **launch 侧预填**……launch 方是唯一知道 worktree 绝对路径的实体」）。F-05 / Step 6 ② 专用 base 分支措辞同文件 `:49` 亲核吻合；本 workstream 实作 `base/m1-home-qa` 与主 checkout 现状（`git worktree list` / 当前分支）一致。 |
| 11 | `pod-coordination.md:86` merge-driver 经 worktreePath 定位 | ✅ 逐字吻合。 |
| 12 | 260911-render-pages 先例（human 2026-09-12 不重跑裁定） | ✅ `.git/devloop/launch/prompt-260911-render-pages.txt:22` 逐字含所引裁定全文；`git rev-parse 0f9895d` 解析成功（0f9895d5d73bcde05642e21c1df907c580d07347）。 |
| 13 | `260829-story-p3` knownRedFloor 手写反例 | ✅ `delivery/taskpackages/260829-story-p3/taskpackage.json:76` 手写散文段在案。 |
| 14 | `architecture.md` §5.5.6（298–302 行）无开发拓扑承载；`:300` 有 skill 指针先例 | ✅ 亲核：该节仅构建 / 测试四面 / 结构化日志三样；300 行含「WSL CRLF 坑见 kidpal-idf-env skill」。 |
| 15 | acceptance-author 步骤 6 红基线恒现跑（T3 边界锚） | ✅ `.claude/agents/acceptance-author.md:36` = 「6. 运行红基线；失败必须对应当前 story 缺失的真实行为。」 |
| 16 | K1a launch prompt 拆段（问题 6） | ✅ `prompt-260915-m1-k1a-key-layer-dispatch.txt` 亲读：身份/探测、包事实、环境事实（IDF/WSL/COM5/ECS）、基线数字、上批留根五段俱全，与 PROPOSAL 拆段描述一致。 |
| 17 | espidf_wsl.sh 头注释 `-T main` 示例在案 | ✅ `firmware/main/test/host_app/espidf_wsl.sh:13` = `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh -T main build`；kidpal-idf-env SKILL.md 现文两坑（`MSYS_NO_PATHCONV` / `-T` 失效警示）确不在册（T5 为纯增）。`idf.py` 无 `-T` 选项的本体实测未由判官复跑（成本高），但 K1a prompt 环境段同日记载「idf.py 无 -T 选项——脚本头注释示例过时，260915 实测」佐证，采信并见 F6 证据落盘 finding。 |

模板硬项核对：§1 实证纪律总体达标（各问题带 file:line / 实测 / hedged 体感声明）；§2 六个 T 均有「目标文件」锚清单且 `### T<n> ·` 标题形态合规；landing-manifest 围栏块五路径 ≡ 各 T 锚清单并集（T1/T3 同文件去重）✅；packet-id 行在场 ✅；§3 sweep 表 UNCLASSIFIED=0、硬项 1b「无新门无判定 pattern」声明与本批内容相符 ✅。

## 二、Findings

### F1 · BLOCKING · abandoned 复位目标 `git reset --hard main` 与 F-05 专用 base 分支结构性冲突

T1/T2 换包协议第二条：「上一包 **abandoned** → 先 `git reset --hard main` 复位清半成品再续」。而同批 T1 ③ 自己对齐的 F-05 现实是：**拆包产物 commit 在专用 base 分支、不在 main**。实测（2026-09-15）：`git log --oneline main..base/m1-home-qa` = 6 个 commit，含「M1-home-qa 开工批拆包四件套落盘（门 1 GREEN 后 commit-before-launch）」（68b2360）与后续五包 taskpackage 修订——main（0551bf0）**不含**这些拆包产物。按字面执行 `reset --hard main` 会把未 launch 包的 taskpackage / plan.md 从树里抹掉，下一个包 intake 即触发 planner F-09 fail-closed「交接断裂」（fail-loud 可恢复，但协议第一步就自破，且与 T1 ③ 在同一 T 内自相矛盾）。

修法方向：复位目标改为「**下一包的基准 ref**」（= 专用 base 分支当时 HEAD，或已含所需拆包产物的 ref）；merged 续跑路径不受影响。T1 与 T2 两处同文概要同步改。

### F2 · BLOCKING · SKILL.md:38 `--check-launch --base-ref main` 漏出 T1 ③ 改动面，且验证集 V2 抓不住

问题 3 的证据只引 `:56` / `:14`，但同一 skill 的步骤 2 机械门命令（`SKILL.md:38`）= `node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref main`——与 pod-coordination §2b canonical「校验**基准 ref（专用 base ref，非 main）**」（`:32/:43` 亲核）直接矛盾，同为 F-05 漂移面。验证集 V2 只 grep `'当前 main'`，对 `--base-ref main` 零覆盖——landing 按现验证集执行会留下这处行为承重残留（launch 前置门的基准 ref 参数），形成「看起来验过、实则证不了」。

修法方向：T1 ③ 显式把 `:38` 列入改动锚（`--base-ref main` → `--base-ref <专用 base ref>`），验证集补一条 `grep -n '\-\-base-ref main' SKILL.md` → 0 命中。

### F3 · BLOCKING · prompt 收缩吞掉固件严格模式默认开的声明载体，静默回退 260909 落地裁定

T1 ⑤ 把 launch prompt 收缩为「数行启动卡」，环境事实一律归 skills。但 `VERIFY_FW_BUILD_REQUIRED=1`（260909 T3/T5，OQ1=(b)「固件严格模式默认开」）不是知识、是**会话级默认声明**：`firmware-build-check.mjs:108` 实测 = `process.env.VERIFY_FW_BUILD_REQUIRED === '1'`，未设即非严格（工具链缺失降级放行）；其成文载体 = SKILL.md §7「launch prompt 环境段注入」（`:100` ①，亲核）+ K1a prompt 实物同款行。skills 是按需读的知识件、不会设进程环境变量——启动卡若不承载这一行，「默认开」退化为「默认降级」，恰是 260909 批要消灭的静默假绿面，且 riskTier 证成句「行为面收敛为少建树、少重跑」随之失真。

修法方向：T1 ⑤ 启动卡要素补一项严格模式声明行（或在「launch 侧动作三件」外补第四件 = launch shell 设 `VERIFY_FW_BUILD_REQUIRED=1` 并在卡上留痕；显式关闭仍须留痕理由），§5 验证集同步。

### F4 · SHOULD · sweep 漏一类消费方：`dev-loop-operating-system.md:50` 的「每个 ready TaskPackage: git worktree add」

`.claude/references/dev-loop-operating-system.md:50`（流程总览 text 块内）= 「每个 ready TaskPackage: git worktree add（自含拆包产物的专用 base ref）+ 起 planner 会话」——与问题 2 同族的「每包一树」固化措辞，住在另一份 reference。本批落地后该行与本批新协议矛盾，重演本批自己要治的「成文漂移」。修法方向：要么入 landing-manifest 一行对齐（「每个 ready 包：运行时 worktree（复用 ∨ 按需新建）」），要么 sweep 表显式分类「未触 + 一句为何可不改」。

### F5 · SHOULD · 验证集 V3 关键词「切新分支」与协议正文「一树一常驻分支」不一致

§5 验证 3 要求新节含四条协议关键词「空闲判读/切新分支/交接卫生/按需扩容」，但 T1/T2 正文四条 = 空闲判读 / **一树一常驻分支** / 交接卫生 / 按需扩容。「切新分支」是 v0.2（一树一分支换包）年代的遗留词形，v0.3 改一树一常驻分支后未同步——landing 若按 V3 字面 grep 会误导措辞回退。修法方向：V3 关键词改「一树一常驻分支」。

### F6 · SHOULD · T5 两坑实测输出未落耐久制品，违反硬项 1a 的留痕形态

T5 两坑的行为断言（`Error: No such option: -T`、MSYS 改写 `C:/Program Files/Git/mnt/c/...`）证据写「实测输出 verbatim 见本批会话记录」——会话记录不是 repo-tracked 耐久制品，硬项 1a 要求「命令 + 关键输出 verbatim + 日期」进草稿。佐证面（K1a prompt 环境段同日记载两坑、espidf_wsl.sh:13 头注释在案）足以采信断言为真，但证据应自持。修法方向：把两条错误输出 verbatim 补进 PROPOSAL §1（或锚到某耐久制品路径）。

### N1 · NIT · Status 行仍写「待 human 立项 ack」而 ack 已给（2026-09-15，OQ1–4 收口）——主线程标 FROZEN 时更新凭据即可，不阻 GREEN 后续轮。

### N2 · NIT · T3 ④ 措辞「launch prompt 模板要素（skill §7）同步——**环境段**标基线时附 anchor SHA」与 T1 ⑤ 收缩后的启动卡形态需在 landing 文本里统一（卡上 = 基线锚指针 + SHA + 面域），避免两 T 各写一种载体。

## 三、聚焦要点结论（任务指定六项）

1. **模板硬项**：§1 实证纪律合格（17 项承重断言全部亲核为真，无一虚构）；§2 锚清单与 landing-manifest 并集一致；§3 UNCLASSIFIED=0。本批失分不在实证、在改动面完备性（F2/F4）与协议内部一致性（F1）。
2. **机械断言复核**：全部属实（见上表）；尤其问题 5 的「分支惯例无机械依赖」三链证据（merge-driver 读当前分支 / 两闸零 dev/ 命中 / integrator 唯一 prose 承载）逐条实测成立——T4 的方向因此成立。
3. **T3 真值纪律**：第三态定义不放松 260909 T5——三件齐（量测 SHA + 命令 + 落盘锚 verbatim）缺一视同手写散文，且比 2026-09-12 一次性裁定更严（增了 `git diff --quiet` 机械前置）；边界（本包验收红基线恒现跑）锚 acceptance-author.md:36 属实、防误伤成立。通过。
4. **T6 spec 连带**：§5.5.6 增行纯记账（现状亲核为三样、零拓扑承载），指针关系（「操作协议见 kidpal-pod-launch-prep skill」）与 architecture.md:300 既有指针先例同构。通过。
5. **行为影响面**：六 T 落地后 planner/builder 行为零改（prompt 载体不变、planner.md:35 自探测本已在位）、integrator 一处口径放宽（非 main/master 即过，与 merge-driver 同口径、驱动契约句保留）。漏的消费方 = F3（严格模式声明）与 F4（operating-system 流程行）。
6. **换包协议完备性**：merged 续跑 / 白名单（build/ 与 managed_components 软链保热）成立；abandoned 复位时机无窗口问题（复位在下一包 launch 前、树空闲判读之后），但**复位目标错**（F1）；board worktreePath「记录不是锁」与 OQ4 裁定自洽，无升级必要。

## 四、结构字段核对（GREEN 前置项）

landing-manifest 围栏块在场、五路径与 §2 并集一致 ✅；packet-id 行在场 ✅。本轮 BLOCK 不因结构字段，全部因 findings F1–F3。
