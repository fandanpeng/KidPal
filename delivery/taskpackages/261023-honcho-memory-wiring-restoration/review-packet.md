# Review Packet · 261023-honcho-memory-wiring-restoration

> integrator 收拢制品（2026-10-23，dev/pod-m2）。rigor=lite / riskTier=medium / sourceType=arch-review。待 supervisor 机械闸（`supervisor-verdict-gate.mjs`，由 planner 跑）复核；本 packet 不自行判定通过、不执行 merge。

## Change Size

- **Files changed**：changeManifest 六件（5 改 1 删）+ 本包 TaskPackage 制品面（plan/launch-card/brief/taskpackage/pod-events/verifier-evidence/verify-runs + 本 packet/docs-delta/test-scope）+ `delivery/taskpackages/current/*` 投影三件。
- **Net diff**：changeManifest 六件 + current 投影 = 1415 insertions / 553 deletions（`git diff --stat`，含 acceptance 件 1336 行换代与 apply 重做 364 行）。
- **Scope**：server/deploy/261022-openclaw-honcho-plugin 工作台三件 + manifest 工作台条目 + server/acceptance 冻结锚换代件——纯 server 侧部署声明与验收锚换代，零 wire 形状改动（crossEnd exempt，豁免理由 ≥12 字已留痕 taskpackage）。
- **Split recommendation**：不需要拆——单子系统、六件 manifest、验收承载单件；批 A（compose）已先行回流 f4d5e647 解耦。

## 验收对账（exitCriteria ↔ 承载断言 ↔ 实跑证据）

| Exit criterion | 承载断言（test_261022_memory_isolation.acceptance.py） | 强度 | 实跑证据 |
|---|---|---|---|
| A1 声明 5 键逐字 / 负锚 / workspaceId=kidpal / pin 1.7.0 / 运行时证据字面 / session 家族 | `test_k3_e2_verified_wiring_declaration_anchors` + `test_k3_e6_declaration_schema_and_p1_drift_anchor` | 机制级 | 本件实跑绿（12 passed / 3 deselected，2026-10-23 integrator 复跑） |
| A2 apply 双形态安全化（零直写 / patch 生成器 / §9 编排 / 桩真跑调用序 / 幂等 SKIP / drift-guard exit 3 / 时间戳备份） | `test_k3_e6_patch_generator_product_five_keys` / `test_k3_e6_apply_safe_orchestration_call_order` / `test_k3_e6_apply_failure_stops_zero_write` / `test_k3_e6_apply_idempotent_and_preserves_p1_state` / `test_k3_e6_apply_drift_guard_exit3` / `test_k3_e6_apply_timestamped_backup` | 行为级（fixture 真跑） | 同上 |
| A2 rollback 对应改 + 三级回滚序申报 | `test_k3_e6_rollback_dry_run_and_restore` | 行为级（真跑）+ 源锚 | 同上 |
| A3 carrier 撤净（js 删除 / manifest 4→3 / 零 marker / 零 /v1/openclaw） | `test_k3_e2_verified_wiring_declaration_anchors`（负锚半）+ `test_k3_e6_manifest_registers_the_workbench` + `test_k3_e6_no_openclaw_json_in_repo_and_no_secret` | 机制级 | 同上 |
| A4 锚换代九处（marker→两字面 / peer→session 家族 / hooks 全路径 / 探针锚删 / gate_carrier_anchors 重写 / degradation 降 live / manifest 数 3 / 三证 schema / 头注重写） | 上述各件 + 头注（对账见 docs-delta §2） | 机制级 + live 申报 | 同上；EVIDENCE_DIR 已指本包 memory-probes/ |
| LOOP-X 既有冻结件零新增红 | 姊妹件 test_261022_honcho_memory / test_261023_agents_md_static_semantics_alignment 零触碰 | 观察位 | 见下「红基线↔转绿对账」；全量 final verify 归 planner |

## 红基线 ↔ 转绿对账

- **换代前基线**（acceptance-author 冻结相位实跑，2026-10-23 本树）：316 passed / 6 deselected / 33 subtests（批 A+B 后基线确认）。
- **红基线**（换代后全量实跑）：310 passed / 8 failed / 7 deselected / 33 subtests——**8 红全部在本件被换代断言**（设计态红：旧声明 2 键 ≠ 5 键 / 旧 apply 不识别 `--gen-patch` 与 `--openclaw-bin` / rollback 头注缺三级回滚序 / manifest 4 ≠ 3 / hooks 全路径缺失 / wiring 锚缺失）；既有其它件零新增红（全量排除本件 = 306 passed / 4 deselected / 33 subtests 零 failure，同相位实跑确认）。
- **转绿实态**：integrator 收拢相位复跑本件（offline，非全量）= **12 passed / 3 deselected in 13.65s**——8 设计态红对应断言全部转绿（红 8 = 旧断言，绿 12 = 换代断言本件 host 全集）。
- **全量预言**：306（排除本件基线）+ 12（本件）= **318 passed / 7 deselected（4+3）/ 33 subtests 零 failure**——与 taskpackage redBaselineResult 预言算术一致。**全量实跑与 final bearing verification 归 planner**（verify.mjs --phase=final --scope-file delivery/taskpackages/261023-honcho-memory-wiring-restoration/test-scope-final.txt；人工触发位，integrator 不跑全量回归）。
- **verifier scoped r1**：factOk:true（`verifier-evidence/rounds/r1.json`，invocationId 3188dd8c-e98a-4ed7-9f72-d9daac7eeb4d，7 gates run / firmware-build lite 后置 / observedGateExit 0）。

## changeManifest 六件逐件 diff 摘要

1. `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json`（+41/-16）：changes 换代为恰 5 条点分键（plugins.entries.openclaw-honcho.enabled / .hooks.allowConversationAccess / .config.baseUrl / .config.workspaceId="kidpal" / .config.noisePatterns=["^孩子(到目前为止)?讲的内容："]）；顶层 `hooks.*` 与 `agents.entries.*.plugins` 零出现（负锚）；新增顶层 plugins 嵌套镜像块（人读 + 验收读取）；drift_guard P1 现状锚（tools.allow 四项）原样保留；`_note` 重写为甲案口径（pin 1.7.0 / session key 家族 / §9 铁律）；零 secret。
2. `server/deploy/261022-openclaw-honcho-plugin/apply_honcho_plugin.py`（+364 级重做）：点分写入器直写路径撤销；双形态——`--gen-patch` patch 生成器（declaration → config patch 文件，离线可测）+ §9 序安全应用编排（备份→dry-run schema:true→install --pin→inspect/doctor→正式 patch→validate→restart+health，任一步失败即停）；`--openclaw-bin` 注入桩离线真跑调用序；幂等 SKIP / drift-guard STOP exit 3 / 写前时间戳备份语义保留；退出码 0/1/2/3。
3. `server/deploy/261022-openclaw-honcho-plugin/rollback_honcho_plugin.py`（+18/-4）：头注重写申报三级回滚序（① plugins disable + restart 低危首选 / ② 备份还原本脚本承载 / ③ plugins uninstall）；CLI 与还原语义不变。
4. `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin.js`（**删除**）：自研 carrier 弃用（三层失效：配置形状 schema 拒收 / 不是插件 / 端点不存在）。
5. `server/deploy/manifest.yaml`（+17/-12）：插件工作台条目 4→3（decl+apply+rollback）；carrier 条目删除；三件 note 换代为甲案口径（5 键 / 安全 apply 序 / 三级回滚序）；dst 落点 openclaw-patch 消费位不变。
6. `server/acceptance/test_261022_memory_isolation.acceptance.py`（1336 行级换代）：头注重写（已验证接线 + R1/R2 残余申报）；存活 4 组 + 15 个 test（12 host + 3 live）；锚换代九处按 advisory §7 逐条；EVIDENCE_DIR 指本包 memory-probes/；三证 schema + 甲案 one_shot 口径写死。

## Live 面 pending 申报（status: awaiting-human:live-deploy）

Pod 停等，以下全部归 human 部署轮（或主线程经 human 授权），Pod 不自行上 ECS：

1. **插件安装**：`openclaw plugins install @honcho-ai/openclaw-honcho --pin`（1.7.0），`plugins inspect --runtime` 核对 Status: loaded + Version: 1.7.0。
2. **4+1 键 apply**：按 advisory §9 序执行（备份→dry-run schema:true→…→restart+health/NRestarts 核对）；apply 脚本已交付工具与序，真实执行 = 部署轮。
3. **三证采集**（live 证据落本包 `memory-probes/*.json`）：① session metadata `openclawSessionKey`（前缀 `agent:kidpal:openai-user:`）② DB 消息条数增量 ③ 网关日志行；one_shot 判定口径 = 甲案 DB 增量中 user 原文零出现（guide/organize 各 ≥1）。
4. **seeded 负向探针集**：三档（姓名/电话/家庭安排）profile_hit 与 reply_hit 全 false（S-F5.1-AC3 / NFR-PRIV-1 量化口径不变）。
5. **降级实测**：停 Honcho 容器 → 对话照常、无重试风暴、无用户可见报错。
6. **OQ-3 实测**：`honcho_*` 工具是否对 kidpal agent 暴露 / `honcho_search_messages` 可搜性（R2 残余）——实测后另裁挡置。
7. **真机抽检**：整理回合画像注入对「忠实」的影响（R1 残余，抽 3-5 例）；ECS 2C/4G 内存复测回填 `TECH-261022-openclaw-prep-tax-memory-pressure`。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无） | 本包 changeManifest 六件 grep `minimal:` 零命中 | — | — |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | live 编排 | apply 编排 ⑦ 步 restart 后 NRestarts 核对是「5 秒级重启报警」启发式，非硬闸 | changed-code hunk（apply_honcho_plugin.py 编排段）+ heuristic「ECS 实况漂移兜底」 | 部署轮核对回执如实入 memory-probes；异常即升级 |
| NIT | judgement-call | 文档 | 声明顶层 plugins 嵌套镜像块与 changes 扁平键为双份同值，靠 `_note` 约束一致 | changed-code hunk（declaration 镜像段）+ heuristic「单源消费 = changes，镜像仅人读/验收读」 | 无（验收断言已钉两处一致；后续维护者改任一侧须同步） |

结构闸（verifier scoped r1 七闸）PASS/FAIL 不复述为 finding；gate 全绿仍完成五轴语义 review（下节）。

## Five-Axis Review

- **安全**：openclaw.json 零入仓 + 零 secret 负锚在位（`test_k3_e6_no_openclaw_json_in_repo_and_no_secret`）；直写路径撤销后配置唯一写入口 = OpenClaw `config patch`（写前 --dry-run schema 校验），逐调用 sha256 协议承载「失败序零写 / 正式 patch 前零直写」；backup/rollback 形制不变、三级回滚序申报。残余：桩后冗余直写离线盲区（头注缺口段已申报，live NRestarts 兜底）——非 BLOCKING。
- **契约**：`docs/contracts/**` 零触碰、零 diff；本包零契约面改动需求（甲案复用上游插件、配置键非跨端协议）；契约面只读红线全程遵守。
- **边界**：manifest/工作台/验收件全部在 changeManifest 六件内；姊妹件（test_261022_honcho_memory、test_261023_agents_md_static_semantics_alignment）与 compose、spec、contracts 零触碰（零越权声明见下）；跨层调用无新增（front payload seam 只读消费不变）。
- **可运维性**：apply 幂等 SKIP / drift-guard STOP exit 3 / 失败即停非零退出 / 退出码 0-3 明确；rollback 三级序低危首选；patch 生成器离线可预演；部署轮 runbook = advisory §9 序 + apply 头注。
- **文档**：docsSyncRequired=false（零 canonical spec/contracts 改动）；两处残余偏差 R1/R2 经 acceptance 头注 + docs-delta.md 逐条申报，spec 口径修正归面 C /arch-change（PROPOSAL 261023-arch-review-memory-wiring-verified-behavior 在途，integrator 零自改 spec）；acceptance 头注重写对账见 docs-delta §2。

## 零越权声明

- 验收件（本件）改动 = **显式治理动作**（advisory §10.4 human 已裁：批 B 锚换代承载，非静默改冻结件），由 acceptance-author/builder 执行、integrator 零实现改动。
- 姊妹件 test_261022_honcho_memory.acceptance.py / test_261023_agents_md_static_semantics_alignment.acceptance.py / K1/K2/261017 冻结件：**零触碰**。
- `server/deploy/honcho/docker-compose.yml`：零触碰（批 A 已回流 f4d5e647）。
- `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`：**零 diff、零写**。
- integrator 本人：只收拢 + 三制品（test-scope/docs-delta/review-packet）+ commit，零实现代码改动。

## Residual Risk

- **decisionCoverage risk-accepted 条目**：taskpackage 无 `decisionCoverage` 字段 → 无 `landsIn: dropped` + `risk-accepted` 条目可列（空）。
- **Known limitation**（head注缺口段同源申报）：① 桩后冗余直写离线盲区（live NRestarts 兜底）；② 插件运行时行为（loaded 日志 / Policy 行 / noisePatterns 挡 user 消息）不可离线实证——静态锚为源锚代理，行为本体 = live 三证；③ 幂等/drift-guard「先于任何外部命令」离线只证 fixture 行为，ECS 实况由 STOP/SKIP 回执承载；④ R1/R2 残余偏差（见 docs-delta §1）。
- **Follow-up recommended**：面 C /arch-change 批（spec §6.7/§1.3 口径修正）落地后回写本包 docs-delta 状态；OQ-3 实测结论回填。
- **Human decision needed**：live 部署轮排期（install+apply+重启+三证+降级+OQ-3）；R1 读侧注入对「忠实」影响的真机抽检判读。

## 回滚路径

- 代码面：本 commit 单 commit 于 dev/pod-m2，`git revert <sha>` 一键回退（六件 manifest + 制品同 revert）；carrier 弃用为删除件，revert 即恢复。
- 部署面：三级回滚序（rollback_honcho_plugin.py 头注申报）——① `plugins disable openclaw-honcho` + restart（低危首选）；② 时间戳备份还原（脚本承载）；③ `plugins uninstall openclaw-honcho`。
