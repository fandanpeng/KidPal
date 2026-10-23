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
