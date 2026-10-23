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
