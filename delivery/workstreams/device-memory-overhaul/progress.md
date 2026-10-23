# progress · device-memory-overhaul

> 执行日志（append-only）：改了什么、测了什么、失败/修复。行不覆盖。

## 2026-10-22 · 拆包（portfolio-planner，同 changeId 第二轮）

- **status：`awaiting-human:slicing-review`** —— 四件套已齐备并落盘，停于「门 1 + human 过目切法」之前；不拉子会话、不 launch Pod。
- **上游复核（intake）**：workstream PROPOSAL FROZEN v1.0（pin `sha1:b03fdcbb2150cbfa`）；arch PROPOSAL FROZEN v1.1（pin `sha1:68254183b7f6e6ab`，commit `0411e957` landing PASS）；review.md 契约四分类闭合（处 A `NO_CONTRACT_CHANGE` / 处 B `ACCOMPANIES_SPEC_CHANGE` 已随批原子落）；requirements v1.67 frozen；task_plan.md P1–P5 phases；milestones.jsonl `M5-device-memory`（state=planning）。
- **上轮停等两件复核**：① 里程碑登记（`M5-device-memory` 行在场，`delivery-status.mjs` 已重跑）✅；② `task_plan.md`（phases P1–P5 + merge order + launch 前置 + 已知窗口）✅；连同 PROPOSAL（③ 判据）三件齐全，拆包顺序链停等解除。
- **拆 5 包（单 Pod 串行，merge order 50–54）**：
  - `261022-m5-k1-prompt-carrier-migration`（P1，mo50，dependsOn []，medium/full/proposal）
  - `261022-m5-n2a-schedule-mechanical-injection`（P5，mo51，dependsOn K1，medium/lite/regression）
  - `261022-m5-k2b-persistent-session`（P2，mo52，dependsOn K1，high/full/proposal）
  - `261022-m5-k3-honcho-memory-production`（P3，mo53，dependsOn K2+K1，high/full/proposal）
  - `261022-m5-k4-history-corpus-backfill`（P4，mo54，dependsOn K3，high/full/proposal）
- **切片内容与 changeManifest 种子**：以 PROPOSAL §4 + task_plan.md + review.md §5 逐文件清单为准；P1 的 `openclaw-system-prompt.v12.md` 新建项已按 arch §6.1 / OQ-8 撤销，改为新增 `server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`；既有 `kidpal-agent-prompt.txt` 去留/改写归 P1 定形（二选一、不得并存）。
- **story 机查**：S-F5.1（requirements v1.67 新建）不在 §8 已砍清单；可行性 AC1/AC2 ✅、AC3/AC4/AC5 🟡，无 🔴，无需立项处置记录。S-F1.3（课表问答）沿用。
- **跨包共享常量 / 冻结面交叉扫描**：常量互斥型未发现；行为碰撞型四处已登记处置——(a) manifest `.config.yaml` marker 锚「实时事实回合」P1 必须同批换成动态块锚；(b) 冻结验收 harness（test_prompt_v7_round_routing / test_261010 / test_261017）P1 经 acceptance-author 留痕 supersede；(c) `openclaw.json` 被 P1（tools）与 P3（Honcho 插件/hooks）两次声明式 patch，须顺序 apply / 幂等 / drift_guard；(d) `workspace-kidpal/AGENTS.md` 被 P1（v11 迁移）与 P3（v12 守则）串行编辑。详见 `decision-coverage.json` `_doc`。
- **落盘制品**：
  - `roadmap.md`（① 六目的投影 / ② render-roadmap.mjs 机械渲染 5 包列车图 / ③ 每包效果与回指 / ④ 非目标）
  - `delivery/taskpackages/<changeId>/taskpackage.json` ×5（acceptance 为空骨架 `[]`，acceptanceTestsFile 空——Draft 态，验收唯一写点归 Pod 内 acceptance-author）
  - `delivery/taskpackages/<changeId>/plan.md` ×5（背景摘录锚 workType 分流 / 可行性判据 / 跨包事实与共享冻结面约束 / 铁律 / demo 证据形态）
  - `decision-coverage.json`（R1–R9 + OQ-1–OQ-10 + 切片 + 非目标全覆盖；`crossPackageBehaviors` 8 条；`launchPreconditions` LP1–LP5）
- **机检**：5 包 JSON 均通过 `taskpackage-required-check.mjs` 除 Draft 态两字段（`acceptance` / `acceptanceTestsFile`）外零错误、零 warning；`render-roadmap.mjs` 渲染成功且编号与 changeId token 一致（M5-K1 / M5-N2a / M5-K2b / M5-K3 / M5-K4）。
- **board**：5 pod 预填 `queued` + merge order 50–54（`portfolio-reconcile.mjs --set-pod` 单一写者；worktree-path 留 null，由 launch 侧预填）。
- **待办（交给 human / 门 1）**：① human 发起门 1（architect 轻量 checklist 五项 + workType 分流），裁决 append `review.md` 门 1 节；② 门 1 GREEN + human 过目切法；③ commit-before-launch（四件套 commit 到专用 base ref，非 main；board 不入 commit）；④ 逐包 launch（P1 先，P5/P2 次，P3、P4 串行）。

## 2026-10-22 · 门 1 GREEN + 判官 A1–A5 补正落盘（portfolio-planner）

- **门 1**：human 确认 verdict GREEN、过目切法；判官 A1–A5 五项补正经 human 终裁，均不构成否回、不改切片结构。裁决已耐久记录 `review.md`「门 1」节（GREEN 不留聊天）。
- **A1（K1 · SOUL/USER 清理）**：落 K1 `plan.md`「跨包事实」节 + taskpackage `K1-E8`（structural / live 处置项）——live 核实 `workspace-kidpal/SOUL.md` 与 `USER.md`：存在则清空/瘦身并登记为 P1 launch card 人工步骤（随 deploy.sh 铺放批次一并处理），不存在则显式记 moot；不扩 changeManifest。
- **A2（K3 种子外扩项确认）**：落 `decision-coverage.json` `_doc`——`server/deploy/261022-openclaw-honcho-plugin/**` 属种子外扩，依据 = P3 范围「生产 profile 插件+hooks 授权」+ K3-E6 + 261017 声明式 patch 先例，无他包认领（human 2026-10-22 确认）。
- **A3（K4 出处指针改正）**：K4 `plan.md`「arch PROPOSAL §4」→「workstream PROPOSAL.md:99 · §4 P4 行」。
- **A4（glob 重叠注记）**：落 K3 `plan.md`「跨包事实」节——`server/deploy/honcho/backfill/**` 子树归 P4，K3 只建栈本体、不预建/不改动 backfill 目录。
- **A5（launch brief 提醒）**：落 `roadmap.md` ④ 节 + `decision-coverage.json` `_doc`——OQ 编号两套不同源（workstream vs arch PROPOSAL），实现侧按包内 `sourceRefs` 读、勿裸引 OQ 号。
- **LP4 状态翻转**：`decision-coverage.json` `launchPreconditions` LP4 → `verified`（evidence = review.md 门 1 节 + 本节点补正）。
- **机检**：5 包 `taskpackage-required-check.mjs` 重跑（见同日验证输出）；JSON 语法与 Draft 态豁免字段核对。
- **status：`ready-for-commit-before-launch`** —— 四件套就绪；LP5 commit-before-launch（专用 base ref，非 main）与逐包 launch 交主线程（human）。本轮不拉子会话、不 launch Pod。

## 2026-10-23 · P1（K1）launch + 冻结相位起跑（主线程 = Pod m2 本体）

- **launch 形态**：human 指示「开始开发第一个包」——本树（`C:/AI/KidPal-pod-m2`，`dev/pod-m2`）即 Pod 本体就地起 loop（m2-char-window / 261010-p3-storylist 先例），不另建树、不派 headless。K1 为 server/deploy 纯云端包，本树环境可用，无固件等价化需求。
- **launch 面动作**：① 五包 board worktreePath 预填 `C:/AI/KidPal-pod-m2`（`portfolio-reconcile.mjs --set-pod` ×5，status=queued、mergeOrder 50–54 维持）；② K1 启动卡落盘 `delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/launch-card.md`（已知窗口判红豁免 / live 门前停等 `awaiting-human:live-deploy` / 共享冻结面纪律 / medium 合并门预期）。
- **loop 进度**：acceptance-author 冻结相位已派（红基线实跑回填 + K1-E1..E6 host 面验收冻结 + 三件冻结验收 supersede 换代 + `test_261022_openclaw_kidpal_tools.acceptance.py` + taskpackage.json 回填）。后续 builder → verifier↔fixer → integrator → supervisor 按 full rigor 串行。

## 2026-10-23 · K1 冻结相位收口 + 扩单两项（planner）

- **冻结完成**：红基线终态 = 冻结前既有套件 246 passed 全绿无先存红；冻结后 242 passed / 26 failed（全设计态红：新件 18〔承载迁移 8 + tools 覆写 10〕+ supersede 换代红 8），零 infra 红。新验收两件：`test_261022_m5_k1_prompt_carrier_migration.acceptance.py`（12 齿，K1-E1/E2/E3/E5/E6 + README 指针节齿）+ `test_261022_openclaw_kidpal_tools.acceptance.py`（10 齿，K1-E4 四态契约仿 261017 先例）。K1-E7/E8 live 项零自动断言、docstring 申报。
- **planner 裁定 A(a)**：supersede 扩面授权——m3k3（2 齿）/ m4k3（2 齿）/ perf-k1（4 齿）承载拓扑守卫与已换代三件同类，K1-E3 落地必转红；选项 (b)「保留 v11 指针线」与 K1-E1 冲突且留死指针，不取。扩面后六件全部 SUPERSEDED-BY 留痕、与承载无关断言零回改、perf 数值断言零放松。m3k3 一齿（`test_a1_wired_prompt_carries_v6_schedule_anchors`）在 planner 点名清单外、与点名齿共用帮手同属承载拓扑断言，acceptance-author 按同授权一并换代，planner 追认。
- **扩单两项（planner 决定，record 于 taskpackage.json changeManifest）**：① `server/deploy/perf-k1/probe_gate.py`（acceptance-author 申报——resolver 改指 AGENTS.md 是 perf 三齿转绿前置，prompt_sha256 口径不动）；② `server/deploy/README.md`（prompt 指针节散文随承载反转改写）。impactedTests 补 m3k3/m4k3/perf-k1 三件。
- **taskpackage.json 回填完成（planner 代写，acceptance-author 写权被 scope 层拦属预期防线）**：acceptance 8 条目（E1–E6 + E7/E8 structural）+ acceptanceTestsFile 两件 + redBaselineResult 终态 + 扩单；`taskpackage-required-check.mjs` 零错误零 warning；`brief-inputs.mjs` 生成 input-brief.md（0 warnings / 14 advisories）。
- **TODO-BUILDER 锚（冻结相位移交，随 builder brief 下发）**：E2 二选一定形推荐「新件取代」（OQ-8 单源）；E2 课表摘要占位措辞建议「课表摘要」；E3 静态承载键键形不钉名（key: value 行且值含 AGENTS.md 路径）；E4 运行时键形假设 `agents.entries.kidpal.tools.allow`（部署轮不符走 tighten-only、不得静默凑合）；渲染注入形态假设 `.config.yaml prompt:` 块 2 空格缩进现行 sed 口径。另：m4k3 换代新增「≤12 字标题」AGENTS.md 迁入要求，builder 迁移时须逐条承载。

## 2026-10-23 · K1 builder 全绿 + verifier 双轮绿 + 进 integrator 收口（planner）

- **builder 实现（未 commit）**：26 设计态红全部转绿——acceptance `268 passed / 4 deselected / 33 subtests`、unit `115 passed`、契约零 diff。逐 K1-E：E1 v11 全量静态语义迁 AGENTS.md（路由五行/回复格式/各域节/硬约束/底线三不变量 + m4k3 换代新增「≤12 字标题」锚；v11 本体零触碰 sha256 仍绿）；E2 定形 **「原地改写」**（判法约束下的唯一自洽解，见下）；E3 三面接线翻转（carrier 静态/动态面声明、provider static_prompt/dynamic_context 键、overlay 改注入动态内容块、manifest 锚集撤「实时事实回合」换「本回合时间/课表摘要」、verify-deploy md5/marker 路径原样、README 指针节改写、perf probe resolver 改指 AGENTS.md）；E4 `261022-openclaw-kidpal-tools/` 四态工作台（apply 幂等 SKIP / drift exit3 / UTC 备份 / rollback，与 261017 skills 组合共存真跑，openclaw.json 零入仓）。
- **E2 定形裁定（planner 追认「原地改写」）**：冻结判法组合（内容源齿读 `DEPLOY_DIR/<裸名>` × 存在性齿读 `openclaw-agents/` × PV-A4 身份齿要 `{{base_prompt}}`）下唯一自洽解 = 内容源 `kidpal-agent-prompt.txt` + 装配壳 `kidpal-dynamic-context.v1.txt`；验收头注明列该分支合法。推荐「新件取代」形态若要取需扩单根名新件或 tighten-only 调验收路径，不值。
- **canonical docs 窄漂移（声明性，本包不改 docs）**：arch §5.5.5 把装配壳描述为渲染内容源的动态模板——实现为「内容源 + 装配壳」两件分工。处置：docs-delta.md 登记 + 建议随 P5 小批措辞修正；**P5 注入落点注记已落 `261022-m5-n2a plan.md` 跨包事实节**（真实占位 = `kidpal-agent-prompt.txt` 的 `{{schedule_summary}}`；装配壳无内容占位勿改）。
- **v11 声明面保留（刻意，非漏改）**：`test_char_meaning_merge` 守卫仍解析 provider 三键/overlay PROMPT_SRC 指 v11——保留为「冻结终点版/data 存档」声明面（不再注入 prompt 键），后续包勿当废弃清理。
- **verifier 双轮**：scoped r1（invocation `aa4e8924`）`ok:true`，FS-EVIDENCE-2 过；final r2 由 integrator Step 4 携 scope-file 跑（invocation `28f4ca43`，round 2——首个 final 尝试因缺 `--scope-file` 被 verify.mjs fail-closed 举手、未产证据、轮号回收复用）。FS-EVIDENCE-1 过（19 tracked 改动全部在 changeManifest 面）；current/ 投影已字节级刷新；dispatch-stamp ×2 + over-scan friction ×2 落账。
- **board**：K1 `running`；integrator 转绿后置 `verifier-green`。

## 2026-10-23 · K1 closeout + supervisor 闸 ESCALATE 停人类门（planner）

- **integrator 收口（commit `f08d7d28`，40 files，未 push）**：final bearing r2（invocation `28f4ca43`）`ok:true` 10 闸全绿；test-scope-final.txt 10 行无降级（detect_changes 可用，impacted 2 全在种子内）；closeout 两 blocking 制品落盘、closeout-check PASS；demo 对账行 + 未覆盖面申报六行如实（K1-E7/E8 标「待 human 部署轮」）；两条非 BLOCKING findings（SHOULD：§5.5.5 措辞漂移随 P5；NIT：carrier 历史段时态混排顺手改）。FS-EVIDENCE-3 过。
- **supervisor 机械闸 = ESCALATE（停人类合并门）**：diff 重算 tier=`high`（判据 `all-mock: test-files-no-real-db/event-assertion`——本包 diff 全为验收/声明/部署脚本面、无真实 DB/事件断言，启发式上浮），声明的 medium 向上取效为 high → high 判 ESCALATE。provenance v2 全过（`supervisor-gate-check` exit 0）、evidenceComplete、熔断轮 2。非缺陷路径：medium 包被 diff 启发式上浮属闸的诚实行为。board 置 `blocked`；pod-retro.md 已落（closeout advisory 消）。
- **待 human 裁**：合并放行（人批 → pod-events `human-decision` → `merge-driver --pod` 本地合并）或维持停等。**部署轮提醒**：合并后 live 项仍待 human——K1-E7（prompt_tokens ≤11,000 / cache hit / 工具 4 项）+ K1-E8（SOUL/USER live 清理，launch card 人工步骤）；**AGENTS.md 不得单独铺放**（v11 语义已在仓内镜像就位，走 deploy.sh 整批铺放即安全）。另：loop-retro 触发阈值已到（30 pending，advisory），归主线程排期。

## 2026-10-23 · K1 合并 DONE（planner 收口）

- **人批 → 合并**：human 会话内批准；human-decision 凭据两落（首条 `3a031d29` actor 误落 planner 未过五要素核验 fail-closed，修正条 `00e18128` actor=human 过验——**教训：pod-events 人批事件 actor 字段恒 = human**，planner 是记录者不是决策者）；`pre-merge-escalation-gate` PASS；merge-driver 本地合并 DONE（merge OK，quick-check 2 闸 OK，merge token 串行化走完）。rebase 期间工作树未提交簿记被 fail-closed 拦一轮，commit `fc75c7ee→67ee0e3f` 后放行。
- **收口状态**：board K1 = **merged**（merge order 50）；`delivery/taskpackages/INDEX.md` dev 段 K1 = closed（全量重建收敛；`--upsert` 子命令对本包撞 naming 检查误报，全量重建绕开）；P1→P5/P2→P3→P4 链下一包 = **P5 `261022-m5-n2a-schedule-mechanical-injection`**（mo51，dependsOn K1 已满足）。
- **停等位：`awaiting-human:live-deploy`**——live 观测项移交 human 部署轮：① K1-E7 复测单轮 prompt_tokens ≤11,000（基线 22,116）+ cache 命中覆盖稳定前缀 + `openclaw` 工具审计 = 4 项、57KB schema 消失；② K1-E8 live 核实 `workspace-kidpal/SOUL.md`/`USER.md` 存在则清空/瘦身、不存在记 moot；③ **AGENTS.md 走 deploy.sh 整批铺放、不得单独铺放**（仓内镜像已反转，单独铺 = 护栏真空）；④ 部署后 verify-deploy.sh 对 AGENTS.md 的 md5 漂移报属预期已知窗口不判红。
- **移交下一包的实现面事实**：动态内容源 = `kidpal-agent-prompt.txt`（`{{schedule_summary}}` 占位），装配壳 = `openclaw-agents/kidpal-dynamic-context.v1.txt`；v11 在 provider/overlay 的「冻结终点版/data 存档」声明面刻意保留（test_char_meaning_merge 守卫仍在解析）——两注记均已落 P5 plan.md 跨包事实节。

## 2026-10-23 · K1 ECS 部署轮（human 授权主线程执行，两轮收口）

- **部署序**：PR #18/#19/#20 三次 push（K1 实现 + SKILL.md 热修回流 + manifest 补登）；`deploy.sh --first-run` 两轮（预检按 10-14 旧基线拦对账铺放，`--first-run` 跳预检、铺后自检须绿；全量快照 `/opt/kidpal/deploy-backups/20261023-124026`）。**终态 drift_count=0，DEPLOY OK @ 462d0cfe**。
- **部署实证缺口两个（已修）**：① `kidpal-reminder-semantic/SKILL.md` live 热修未回流（10-21 三条陷阱实证）→ 原样回流仓（88e8bdb3）；② **apply-xiaozhi-overlay.sh 自身不在 manifest**（ECS 停旧版致渲染链走旧布局：装配壳缺失、prompt 块为旧 v11 内容、内容源被旧脚本回写）+ 新脚本 `$HERE` 消费位无输入铺放条目 → manifest 补三条（脚本自身 + 内容源/装配壳输入副本，fabc6e4c；输入行 restart:engine 保证同轮触发渲染与引擎重启）。教训进 pod-retro 语境：K1 离线验收 `$HERE`=repo 布局成立、ECS `$HERE` 布局不成立，渲染脚本的输入面必须在 manifest 成对登记。
- **openclaw 运行态补丁**：`apply_agent_tools.py` APPLIED（allow=[read,write,exec,web_search]，备份 `openclaw.json.bak-kidpal-tools-20261023T044104`；复跑 SKIP 幂等 ✓）——注意 ECS 默认 python3=3.6 需 `/usr/bin/python3.11`（SKILL.md 热修同款坑）。
- **K1-E8 执行**：`workspace-kidpal/SOUL.md`(1518B)/`USER.md`(871B) 在场 → 备份 `/opt/kidpal/deploy-backups/20261023-k1-soul-user/` 后瘦身为指针壳（228B/224B）。
- **live 核验全绿**：engine boot 干净（ASR/组件初始化成功）；设备 12:40:44 重连，`kidpal display-strip patch active` 标记行在场；**`快速初始化组件: prompt成功 <context>`** = 引擎装配壳 + 动态块生效（v11 注入退役）；`.config.yaml` 锚集 本回合时间/课表摘要 命中；`prompt_template` → 装配壳路径；front/openclaw-gateway active；tools allow 四项读回一致。
- **K1-E7 余项（待语音回合）**：真机语音对话单轮 prompt_tokens ≤11,000 + cache 命中——设备在连但未开嗓，等 human 安排一轮回读数。

## 2026-10-23 · K1-E7 live 实测达标——P1 全 exitCriteria 闭合（planner）

- **观测回合**：human 让设备问「今天西安天气怎么样」（12:44:57，session 5cbd765f）。链路全通：ASR → kidpal agent 调 `web_search`（四项收窄工具真实工作）→ 真实数据播报（晴间多云 9-17℃，有查询支撑、非编造——底线不变量行为级验证）。
- **token 实测**（openclaw transcript usage）：首轮 input 7,761 / cacheRead 0（新会话冷启动，≤11,000 目标 ✓，基线 22,116 → -65%）；二轮 input 3,949 + cacheRead 7,808 = 11,757（含 web_search 结果 ~3k 回合事实）——**cache hit 66% 覆盖稳定前缀（基线 32%，翻倍达成）**，每轮真成本（未缓存）14,948 → 3,949（**-74%**）。
- **P1 收口**：K1-E1..E8 全部闭合（E7 实测达标、E8 已执行）。board merged / INDEX closed 不变；部署缺口两修复（SKILL 回流 + manifest 三补登）随 PR #19/#20 在 origin/main。
- **下一包** = P5 `261022-m5-n2a-schedule-mechanical-injection`（mo51；消费本包动态块 `{{schedule_summary}}` 占位）。

## 2026-10-23 · P5（N2a 课表机械注入）launch + 冻结相位起跑（planner，K1 同会话续跑）

- **launch 面**：human 指示起包；board `running`（mo51）；current/ 投影切 P5；`brief-inputs.mjs` 产单（0 warnings / 11 advisories）；启动卡落盘（基线锚 = K1 后 268 passed；T5 + 契约 §1.5 为架构权威；通道形态若需改 front 侧文件而 changeManifest 未含 → 停报扩单）。
- **S-F1.3 权威通读**（1e 硬前置）：AC1 共享问答框口径 / AC4 备注参与答复——本包锚 AC1（注入使课表回合零工具调用也答对）；补充规则「备注列参与语音答复」落 K5-E3 受控集的备注断言。
- **loop 进度**：acceptance-author 冻结相位已派（K5-E1..E6 冻结 + 红基线回填 + 通道形态现状排查——front_app.py 不在 changeManifest，是否需要扩单由排查结论定）。

## 2026-10-23 · P5 冻结收口 + 扩单一项 + builder 起跑（planner）

- **冻结完成**：红基线 = 冻结前 268 全绿无先存红；冻结后 270 passed / 8 failed（全设计态红，成因单一：派生接缝 `build_schedule_summary` 不在世 + 接线面未落）+ 2 must-stay-green；零 error。变异自检在案（窗口越界 / 内容抽空分别新爆 6/7 红）。新验收件 = `test_261022_schedule_injection.acceptance.py`（10 断言，K5-E1..E6）。
- **冻结契约定形**：派生接缝 = `front_app.py` 模块级纯函数 `build_schedule_summary(model, today_iso)`；内部通道帧 token = `kidpal_schedule_summary`（kidpal_speak 同型先例）；填充接缝 = connection.py 内 `{{schedule_summary}}` 替换、引擎零解析；定稿窗口 = 3 日历日 / ≤2048 字节；**注入内容含学校节次名 + 备注列逐字**（TECH-261018 编造证据恰是学校课与带物）。
- **扩单一项（planner 决定）**：`server/gateway/front/front_app.py` 入 changeManifest——T5 候选甲通道原点必在 front 侧派生，manifest 内自足落法只剩 connection.py 自解析 MD = T5 已否决 + 违反契约 §1.5。launch-card 预设停等判据命中，扩单后放行 builder。
- **簿记**：dispatch-stamp ×2（补实测值）+ over-scan ×1；taskpackage.json 回填（acceptance 6 条目 + acceptanceTestsFile + redBaselineResult + 扩单）；brief 重生成（refs 改动）；current/ 投影刷新；required check 零错误。
- **builder 已派**（例行形态默认档）：8 红转绿目标 = acceptance 278 passed。

## 2026-10-23 · P5 builder 全绿 + verifier scoped 绿 + 进 integrator（planner）

- **builder 实现（未 commit，改动 3 件全在 changeManifest 内）**：`front_app.py` 新增 `build_schedule_summary(model, today_iso)` 纯函数（只调 `items_for_date` 单源符号、学校节次+备注从单源模型行回查、窗口 3 日历日/≤2048 字节整日粒度截断）+ 会话建立时经 `_send_engine_leg_frames` 发 `kidpal_schedule_summary` 帧；`connection.py` 占位填充单收口落 `change_system_prompt`（三时序安全：帧先到/后到/二次到达均正确，缺席填「（本次会话未提供）」不编造）+ 启动标记 `kidpal schedule-summary patch active`；manifest 仅补 note 标记载行。**schedule.py 零改动**；`kidpal-agent-prompt.txt`/装配壳零改动；m3k1/m3k3 冻结件零触碰。
- **验证**：acceptance **278 passed / 4 deselected / 33 subtests**（逐字达标）、unit 115 passed、契约零 diff。分支/边界由一次性探针覆盖（未持久化——unit 层按 K1 先例以既有套件零新增红承载，integrator docs-delta 诚实申报）。
- **P1 遗留观察项（builder 遗留②，跨包）**：装配壳 jinja2 单遍渲染下 `{{today_date}}` 等日期/天气占位可能字面进 live prompt（`{{schedule_summary}}` 本包已填不受影响）——K5-E4 live 轮顺带实证，若确认归 P1 收尾小批（候选落点 connection.py `change_system_prompt` 同缝），本包不扩。
- **verifier**：scoped r1（invocation `86204f35`）`ok:true`、FS-EVIDENCE-2 过；final r2 由 integrator 携 scope-file 跑（invocation `becb85f0`）。FS-EVIDENCE-1 过（3 件改动全在 manifest 面）；dispatch-stamp ×2 + over-scan ×1 落账。

## 2026-10-23 · P5 integrator 收口（未 commit→commit，未合并）

- **final bearing r2 绿**（invocation `becb85f0-afdb-4d4d-a281-a40b99fb0325`，evidence r2.json，fact ok:true，10 闸全绿）：acceptance 圈定跑 160 passed / 5 subtests（scope 清单 10 件恰合）+ unit 220 passed / 5 subtests + firmware 真编译绿；verifier-evidence-check exit 0。scoped r1（`86204f35`）此前已绿。
- **范围圈定**：detect_changes（C-AI-KidPal-pod-m2，base=main，merge_base fdd643b3，impacted_total=66）+ impactedTests 4 件种子 + 共享 seam 4 件合成 test-scope 清单落盘 `test-scope-final.txt`（正常圈定、无降级）；路径一行已记 review-packet。
- **制品**：docs-delta.md（两声明注记：P1 遗留占位观察 + unit 层口径）+ review-packet.md（findings 仅 2 NIT judgement-call，无 BLOCKING/SHOULD）+ verdict.json / verification.json 晋升 + closeout-check --update-index 绿。
- **TECH-261018 结项（K5-E5）**：status closed + closedBy = 本包 + 结项记 append；debt-render 跑过、归位 `docs/debt/tech/_archive/2026/`。
- **停等位**：`awaiting-human:live-deploy`——K5-E4 live 段（真机 ≥20 轮零工具调用零编造）挂 human 部署轮；connection.py engine-face 重启 = 设备断连重连（K1 同款）；manifest note 三条生效标记行核验锚。

## 2026-10-23 · P5 supervisor 闸 ESCALATE → 人批 → 合并 DONE（planner）

- **supervisor 机械闸 = ESCALATE（与 K1 同形）**：diff 启发式（all-mock: test-files-no-real-db/event-assertion）把装配/声明面上浮 high，声明的 medium 向上取效 → 人批放行。provenance v2 全过（gate-check exit 0）；board 置 blocked → merge-driver approvalRefUnlock 消解。
- **人批 → 合并**：human-decision 凭据 `5f792801`（actor=human 一次到位，K1 教训已吸收）；merge-driver 本地合并 **DONE**（quick-check 2 闸 OK；Step 0 治理面收口自动 commit 了在途 pod-events）。board = **merged**（mo51）；INDEX dev 段 P5 = closed（全量重建收敛）。
- **M5 五包列车进度**：P1 ✓（merged + 部署实测达标）→ **P5 ✓（merged，待部署）** → P2/K2b（mo52）→ P3/K3（mo53）→ P4/K4（mo54）。本地 main 领先 origin/main（P5 合并 + K1 记账数 commit），push 归 human。

## 2026-10-23 · P5 部署 + live 实测全绿——P5 完全收口（planner）

- **部署**：PR #21（release/261023-5）push 后 `deploy.sh` **常规闸直接过**（预检 drift 0——K1 manifest 补登后对账稳态恢复，无需 --first-run）；铺 front_app.py（front 重启）+ connection.py（引擎重启）；DEPLOY OK @ 2d338dd1，快照 `20261023-134048`。
- **live 实测（human 持板 4 轮 + transcript 机验）**：
  - 三 patch 标记行全在场（display-strip / stop-truncation / **schedule-summary** 新标记）；
  - **窗口内零工具调用**：「明天有什么课」5 秒直答（周六两节课：新东方英语 8 点 / 街舞 17 点，含地点与提前 15 分钟——注入块事实，transcript 无 toolResult）；「大后天」= 10月26日星期一在窗口外，模型**正确转 read 文件**（窗口外行为 = 读文件不编造，K5-E3 live 证）；
  - 「哪天有美术课」跨窗口泛问 → read 工具查全表（周三第五节/周五第三节）诚实回答；天气混合轮 web 工具正常且**顺带交叉引用了课表**（「早上八点上英语课记得穿外套」）——注入无污染反成加分；
  - **cache 实测**：cacheRead 6,144→9,600 递增（含课表摘要的稳定前缀全命中），每轮 input 仅 341-2,188；
  - transcript 全量 `{{` 零命中——P1 遗留占位（today_date 等）**无行为级泄漏**（回答日期全部正确），降级为外观级清理候选、不另起批。
- **P5 = 全 exitCriteria 闭合**（E4 host 21 轮冻结断言 + live 抽样 4 轮全绿）。M5 列车：P1 ✓ P5 ✓ → 下包 **P2/K2b 持久会话**（mo52）。

## 2026-10-23 · P2（K2b 持久会话）launch + 冻结相位起跑（planner，K1/P5 同会话续跑）

- **launch 面**：human 指示起包；board `running`（mo52）；current/ 投影切 K2b（stale P5 verdict 移除）；`brief-inputs.mjs` 产单（0 warnings / 12 advisories——「TECH-261022 引用腐坏」经核实为裸 token 形态探针误报，`docs/debt/tech/` 下两件 debt 文件均在场）；启动卡落盘（纪律红线四条 / bind-mount 人工生效步 / riskTier=high → ESCALATE 人批为预期路径）。
- **架构权威**：arch T2 + ADR-015 双历史单一权威 + 契约 §四 :287 定案；基线锚 = acceptance 278 passed（P5 后本树实测）。
- **loop 进度**：acceptance-author 冻结相位已派（K2-E1..E5 冻结 + K2-E6 live 项零自动断言申报 + 红基线实跑回填 + taskpackage.json 回填）。

## 2026-10-23 · P2 冻结收口 + builder 起跑（planner）

- **冻结完成**：红基线 = 冻结前 278 passed / 4 deselected / 33 subtests（P5 后锚逐字命中、无偏离）；冻结后 **281 passed / 11 failed**（全设计态红，三源同因：openai.py 补丁未建 4 / connection.py 接线未落 5 / compose+manifest 登记 2）+ must-stay-green 3 条冻结时点即绿；零 error。新验收件 = `server/acceptance/test_261022_persistent_session.acceptance.py`（14 断言，K2-E1..E6；connection.py/openai.py 不可离线 import，走 AST 抽取 + 受限 exec 行为驱动）。变异自检 9 项全抓（双写禁区 / 切片丢史 / 丢 user 注入 / max_tokens 禁带 / marker 门控），自检纠出 harness 两缺陷（exec 命名空间回传 / E4 fixture RAM 历史）。
- **判法定形（acceptance-author，planner 追认）**：connection.py 模块级纯函数 `_kidpal_turn_messages(assembled, actual_start, keep_tail_count)` + provider 探针 `_kidpal_provider_supports_user(llm)`（反射失败 False 不抛）；marker 字面 `kidpal persistent-session patch active` 须探针门控（openai.py bind-mount 丢失时 stock provider 静默吞 user kwargs，无门控 marker = 假绿报警锚）；openai.py 两腿命名参数 `user`（缺省 None）、零 max_tokens（契约 §四 :288 禁带）。
- **簿记**：dispatch-stamp 落账（189k tokens / 66 tool-uses / 23.7min）；over-scan friction 1 笔（同族验收先例两件 + 参考树 05 openai.py/dialogue.py 源码——低风险同向消费）；taskpackage.json 回填（acceptance 6 条目 + acceptanceTestsFile + redBaselineResult 终态 + verify-deploy.sh 零 diff 预期注记）；brief 重生成（refs 改动）；required check 零错误零 warning；current/ 投影刷新。
- **builder 已派**：11 设计态红转绿目标 = acceptance 281 passed / 0 failed。

## 2026-10-23 · P2 builder 全绿 + verifier scoped r1 全闸绿 + 进 integrator（planner）

- **builder 实现（未 commit，4 件改动全在 changeManifest 内）**：`engine-patch/openai.py` 新增（参考树 05 stock 基线整文件复制 + keyword-only `user=None` 两腿 / `if user:` 注入 / 零 max_tokens 语义保持 / thinking 钩子原样）；`connection.py` 切片接线（`_kidpal_turn_messages` 纯函数 + `_kidpal_provider_supports_user` 探针 + depth==0 记 turn_start + 两发送腿内联切片携 `user=self.device_id` + 探针门控第四条 marker，探针失败打 error 不打 marker——假绿锚防护；`dialogue.put` 记账照旧；P5 schedule-summary 段与既有三 marker 零回退）；compose 新增 openai.py bind-mount 行（注释含 compose up -d 人工生效步）；manifest 新资产条目（md5/restart=engine/note 含人工生效步）+ connection.py 条目 marker 清单补第四条。
- **验证实测**：acceptance **292 passed / 0 failed / 4 deselected / 33 subtests**（= 278 既有 + 冻结件 14 全绿）；unit 115 passed 零新增红；契约零 diff；verify-deploy.sh 零改动（预期内，如实申报）。builder 另以真实 Dialogue + 抽取切片函数做四场景离线仿真旁证（未落文件、不计判据）。
- **形态申报**：① 判法目标数 281 是冻结时点值，11 红转绿后实数 292（判据全达成）；② unit 专用补件因 manifest 边界未做（清单外停手，行为级证据由冻结件 ET 断言承载）；③ `_kidpal_prefix_count` few-shot 补位假设 minimal: 注记（nointent 部署面该段为空）。
- **verifier scoped r1**（invocation `7322233d`，round 1）：`ok:true`、factOk true、8 闸全 PASS（taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）。verifier-evidence-check 过。over-scan friction 1 笔落账（builder 超读申报 5 行）。board 置 `verifier-green`。
- **integrator 已派**（final r2 由其携 scope-file 跑，full bearing 模式）。

## 2026-10-23 · P2 integrator 收口（final r2 全绿 + 制品落盘 + commit 冻结 diff）

- **final bearing round 2**（invocation `2b7d9e4a-5c31-4f8a-9e62-d01c4471ab55`，evidence `verifier-evidence/rounds/r2.json`，fact `ok:true`，observedGateExit 0，**10 闸全绿**）；verifier-evidence-check 过（exit 0 / factOk true）。acceptance 闸按 scope-file 圈定 = 89 passed / 4 subtests（7 件恰合清单）；unit 同清单求交 89 passed。
- **范围圈定**：test-scope 清单落 `delivery/taskpackages/261022-m5-k2b-persistent-session/test-scope-final.txt`（acceptanceTestsFile 1 + 种子 3 pytest + 共享 seam 2〔P5 接线守卫 / K1 承载不变式〕+ 安全余量 2〔R9 工具 / 模型名漂移〕；detect_changes impacted_total=7 全为 builtins/usage 边噪声，正常圈定无降级）。
- **制品**：review-packet.md（findings 2 NIT judgement-call，无 BLOCKING/SHOULD；K2-E6 live 项挂 human 部署轮）+ docs-delta.md（四条声明性注记）+ verdict.json/verification.json 晋升 + closeout-check 绿（--update-index inserted，INDEX 收敛）。
- **commit**：feat(m5-k2b) 冻结 diff 收口（实现 4 件 + 冻结验收件 + 簿记面），本地 `dev/pod-m2`、不 push、不合并；待 supervisor 闸（riskTier=high → ESCALATE 人批为预期路径）。

## 2026-10-23 · P2 supervisor 闸 ESCALATE → 停人类合并门（planner）

- **supervisor 机械闸 = ESCALATE（预期路径，launch-card 钉死）**：riskTier=high（`all-mock:test-files-no-real-db/event-assertion` 启发式 + adjudicate critical/high）；provenance v2 全过（`supervisor-gate-check` exit 0）；evidenceComplete、轮 2。verdict 落 `supervisor-verdict.json`；board 置 `blocked`。
- **收尾簿记**：integrator dispatch-stamp ×2（0 值条 + 实测修正条）+ over-scan 落账；pod-retro.md 落（closeout advisory Q5 消）。
- **待 human 裁**：合并放行（人批 → pod-events `human-decision` → `merge-driver --pod` 本地合并）或维持停等。**部署轮提前提醒（比 K1/P5 多一步）**：合并后 ① 新增 bind-mount 首次生效须人工 `docker compose up -d` 重建容器（deploy.sh 只 restart 不重建；容器重建 = 设备全断，需安排窗口）；② K2-E6 真机跨 WS 新会话回忆复证；③ 持久会话增长 / auto-compaction 成本只记观察（arch §8.1 风险 14）。

## 2026-10-23 · P2 人批 → 合并 DONE（planner 收口）

- **人批 → 合并**：human 会话内 AskUserQuestion 批准；human-decision 凭据 `596028f3`（actor=human 一次到位）；`pre-merge-escalation-gate` PASS；merge-driver 本地合并 **DONE**（escalation gate PASS → rebase OK → quick-check 2 闸 OK → merge OK，merge token 串行化走完）。首跑被未提交簿记 fail-closed 拦一轮（K1 同款），收尾簿记 commit `f0856754` 后放行。
- **收口状态**：board K2b = **merged**（mo52）；`delivery/taskpackages/INDEX.md` dev 段 = closed（行 98）。merge commit `a62321cf`；Step 5b 簿记落 main（`190b1a28`）。本地 main 领先 origin/main（P2 合并 + 簿记），push 归 human。
- **停等位：`awaiting-human:live-deploy`**——live 项移交 human 部署轮：① **先 `docker compose up -d` 重建容器**（新增 bind-mount 首次生效 = 唯一路径，deploy.sh 只 restart 不重建；容器重建 = 设备全断，需安排窗口）；② K2-E6 真机跨 WS 新会话问此前讲过的事实能答出（真机半）；③ 引擎日志见 `kidpal persistent-session patch active` 标记行（探针门控——若见 `INACTIVE` error 行 = bind-mount 未生效，即回查）；④ 持久会话增长 / auto-compaction 成本只记观察，不在本包定阈值。
- **M5 五包列车进度**：P1 ✓ → P5 ✓ → **P2 ✓（merged，待部署）** → P3/K3（mo53，dependsOn K2 已满足）→ P4/K4（mo54）。
- **部署核对锚（可机判）**：`docker compose config` 应见 `./engine-patch/openai.py:/opt/xiaozhi-esp32-server/core/providers/llm/openai/openai.py` 挂载；manifest md5 核验过 verify-deploy；两轮追问「我刚才说的第二件事是什么」跨新 WS 会话仍记得 = 持久会话生效。
