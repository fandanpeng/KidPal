# Review Packet · 261025-openclaw-dreaming-disable

> integrator 合并前审查包，2026-10-25，final round 2（invocationId `77ed0122-68e1-47a2-a732-eae6708c90b3`）。等待 supervisor 裁断（由 planner 跑 `supervisor-verdict-gate.mjs` 机械闸）；不 push、不 merge、不碰 ECS。
>
> 上游 = `delivery/taskpackages/261025-openclaw-dreaming-disable/taskpackage.json` 的 `origin.upstreamRef`（`delivery/review/261022-arch-review-device-memory-prompt-overhaul/dreaming-capture-noise-20261025.md`：诊断全文 + §4 architect 裁决「方案 A：全局关闭 memory-core dreaming」+ §5 human 2026-10-25 四裁决）；debt 条目 = `docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md`。前账依赖 = `261025-kidpal-tools-profile-fix`（live 工具面升级态已部署，本包 drift_guard 锚其形态）。无 upstream PROPOSAL（human-ad-hoc 技术/运维包，非 story 来源）。

## 1. Change Size

| 文件 | +/− | 内容 |
|---|---|---|
| `server/deploy/261025-openclaw-dreaming-disable/dreaming-disable-declaration.json`（新建） | +35/−0 | 声明工件：`changes` 恰一条 `{path: plugins.entries.memory-core.config.dreaming.enabled, value: false}`；`drift_guard` 四条叶路径 AND（dreaming 叶 `expect_any ["unset", false]` / tools.deny `["unset", []]` / tools.profile 精确 `"full"` / tools.allow 五项顺序敏感）；`backup.pattern = openclaw.json.bak-kidpal-dreaming-*`；`_note` 记机制、无 per-agent 作用域、零 MEMORY.md 旁证与重评触发；零 secret、无顶层 `plugins` 镜像 |
| `server/deploy/261025-openclaw-dreaming-disable/apply_dreaming_disable.py`（新建） | +475/−0 | 双形态：① `--gen-patch` 从声明生成 patch（零 config 读写、零外部命令、零备份）；② 安全编排 = 幂等预检（SKIP exit 0 + 工具面漂移打 WARN）→ drift-guard（exit 3 零写零备份零外部命令）→ 写前 UTC 时间戳备份 → `config patch --dry-run --json` 核 `checks.schema === true` → 正式 `config patch --file` → `config validate` → `systemctl restart openclaw-gateway` → `openclaw health` → `systemctl show -p NRestarts` 崩循环核对；零 plugins install/inspect/doctor（memory-core bundled）；零「直写 openclaw.json」路径；`--openclaw-bin` 单桩兼收两类 argv |
| `server/deploy/261025-openclaw-dreaming-disable/rollback_dreaming_disable.py`（新建） | +164/−0 | 一步回滚：默认取 config 旁最新声明 pattern 匹配备份 → 整档还原 → `openclaw config validate` 通过才 `systemctl restart openclaw-gateway`（validate 失败不重启并报错）；`--dry-run` 只打印零写零外部命令；`--systemctl-bin` / `--openclaw-bin` 单桩；头注显式区分「还原已知合法快照」与「禁拼装新目标态绕过 zod」 |
| `server/deploy/manifest.yaml` | +18/−0 | 新增 3 条登记（声明 / apply / rollback）：`dst` 落 `/opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/`，`owner: deploy.sh, verify: md5, restart: none`；前置注释写清「openclaw.json 整文件不入仓」「写入面恰一布尔叶键」「与 261017 skills / 261022 tools / honcho 声明作用同一运行态文件、组合共存」 |
| `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` | +12/−0 | 新增 `## 9. 升级检查清单`：`dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 四家族必须为空，违反 = 采集策略漂移；解释 dreaming 子回合落 chat 分支的机制；引 debt 条目并指路重跑本包 apply |
| `server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py`（新建冻结件） | +1652/−0 | 17 条：声明 shape / 严格布尔 false（DD-A1）、gen-patch 单叶 + 零 config I/O（DD-A2）、drift 判定面（DD-A3）、fresh 接受（DD-A3）、六态拒绝 exit 3（DD-A3）、幂等 SKIP 逐字节无副作用（DD-A4）、SKIP 先于 drift 且打 WARN（DD-A4 子 case）、负锚 memory-core 其它键（DD-A5）、编排调用序 + 逐调用 config-hash 零直写、失败即停零写、源锚零直写、rollback dry-run、rollback 先 validate 后 restart、rollback 头注区分、manifest 三件登记、runbook 语义锚、openclaw.json 零入仓 + 零 secret |

已跟踪实现面合计 **+2356 / −0（6 文件，4 新增 2 修改）**。另有簿记面改动：`delivery/taskpackages/current/` 三件投影由 verify.mjs 程序 fs-write（`input-brief.md` +9/−39、`taskpackage.json` +47/−47、`verdict.json` +26/−26）；`delivery/workstreams/device-memory-overhaul/{progress.md +7, roadmap.md +8}`（planner 起包记账 + 本包在册条目）、`delivery/workstreams/INDEX.md`（hook 生成，最后更新日期 +1/−1）；本包目录新增制品（`taskpackage.json` / `plan.md` / `input-brief.md` / `pod-events.jsonl` / `verify-runs.jsonl` / `test-scope-final-r2.txt` / `verifier-evidence/rounds/r1.json`、`r2.json` / 本 packet / `docs-delta.md` / `verdict.json` / `verification.json`）。

零新增第三方依赖（apply / rollback 纯 stdlib；验收件用 PyYAML，属既有 manifest 断言的同一解析器）；零固件 diff（crossEnd exempt：设备端零感知）；零数据迁移 / 零库写；零 ECS 动作、零 push、零 merge。

## 2. Verification Evidence

- **scoped round 1**（invocation `ba3ed9f1-b627-4e55-b1e7-fa1fa8c1543a`，11:36:47.543Z–11:36:49.667Z）：`ok:true`、`observedGateExit=0`、8 闸绿——`firmware-build` scoped 相位后置（`rigor=lite`，261015-gov-review-lite-fast-path；闸自报 `executor-skipped` 且被机器判为 `notExecuted`，非假绿），fact = `verifier-evidence/rounds/r1.json`。
- **final bearing round 2**（invocation `77ed0122-68e1-47a2-a732-eae6708c90b3`，11:38:35.343Z–11:41:25.910Z）：`ok:true`、`observedGateExit=0`、**10 闸全绿**：

  | 闸 | exitCode | ms | 事实要点 |
  |---|---|---|---|
  | `taskpackage-required` | 0 | 693 | changeId 解析一致、零 warning |
  | `change-manifest` | 0 | 555 | `skipped: pre-cutoff`（gitFirstCommitDate 2026-09-11 < CUTOFF 2026-09-28，存量放行） |
  | `firmware-build` | 0 | 100759 | **真编译**（工具链可用，venv `idf5.5_py3.10_env` + `c:/tmp/idf_run.py`），`Executing action: all` |
  | `contract-lint` | 0 | 144 | 仅既有历史断链告警（`260923-arch-review-schedule-grid-cold-boot` 相对链接），与本包无关 |
  | `acceptance-coverage` | 0 | 146 | acceptance = DD-A1..DD-A5，files = 本包冻结件 1 件 |
  | `mutation-check` | 0 | 152 | 豁免在册（`mutationSkipReason` + `mutationEvidence.perAcceptance` 空） |
  | `docs-sync` | 0 | 158 | `docsSyncRequired:false`（理由见 docs-delta） |
  | `security` | 0 | 141 | `securityReviewRequired:true`，`securityAxes: []` |
  | `unit` | 0 | 33880 | **162 passed / 3 deselected**（31.53s），按 10 件 scope 交集跑 |
  | `acceptance` | 0 | 33808 | **162 passed / 3 deselected**（31.51s）；`serverAcceptanceFiles=10`、`filesWithLiveMark="2/10"` |

  fact = `verifier-evidence/rounds/r2.json`。
- **fact 校验**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261025-openclaw-dreaming-disable --phase final --round 2 --invocation-id 77ed0122-68e1-47a2-a732-eae6708c90b3` → `factOk:true`、`observedGateExit:0`、**exit 0**（integrator 实跑）。
- **test-scope 清单落盘路径（Step 4 ① 圈定锚行）**：`delivery/taskpackages/261025-openclaw-dreaming-disable/test-scope-final-r2.txt`，**10 行** = 本包 `acceptanceTestsFile` 1 件（`test_261025_openclaw_dreaming_disable`）∪ manifest 共享 seam 9 件（`test_prompt_v7_round_routing` / `test_261022_memory_isolation` / `test_261022_openclaw_kidpal_tools` / `test_261022_honcho_memory` / `test_261022_m5_k1_prompt_carrier_migration` / `test_261022_persistent_session` / `test_261022_honcho_backfill` / `test_261010_engine_decision21_overlay_persist` / `test_261013_qq_schedule_write_closure`——均在 `server/acceptance/` 各自 `yaml.safe_load` 或字面依赖 `server/deploy/manifest.yaml`；本包向 manifest 增 3 条 src/dst 登记，YAML 一旦解析破损这批同红，属共享 seam 安全余量）。圈定口径已用 grep 独立复核：`server/acceptance` 下命中 `safe_load|manifest\.yaml` 的文件**恰这 10 件**，无遗漏。
  - **留根注记**：r2 fact 的 `results` 元数据未含 scope 路径行（`verify.mjs` 未把 `--scope-file` 透传落 fact；实测 r2.json 内仅 `timing: "scoped"` 字段、无 scope 字段），路径以本 packet 与 `verification.json` 的 `finalScopeFile / finalScopeNote` 承载——与上一号包（`261025-kidpal-tools-profile-fix`）同一观察差集锚，供「圈定 vs 事后全 blast radius」差集观察。
- **圈定交集实测（integrator 独立复跑，2026-10-25）**：10 件 scope 全跑 = **162 passed / 3 deselected（31.17s）**，与 r2 unit / acceptance 两闸同值（两闸 `filesWithLiveMark="2/10"`，live 标记件 = `memory_isolation` + `honcho_memory`，`-m "not live"` 下 3 条 live 用例 deselect）。本包冻结件单独跑 = **17 passed（13.82s）**。
- **全量口径（integrator 独立复跑，2026-10-25）**：`cd server && PYTHONIOENCODING=utf-8 python -m pytest -q --import-mode=importlib -m "not live" acceptance` = **370 passed / 7 deselected / 33 subtests passed（43.56s）**——与 builder 全量实测、planner 独立复跑同值。对照红基线：冻结时同树 = **17 failed / 353 passed / 7 deselected / 33 subtests**（17 条全指向未实现，零环境错误）；冻结前同树基线 = 353 passed。现 17 条设计态红全转绿、既有 353 条零回退、净增 17 条通过。
- **gen-patch 产物独立复核（integrator 实跑）**：`python server/deploy/261025-openclaw-dreaming-disable/apply_dreaming_disable.py --gen-patch <tmp>/dd.patch.json` → rc 0；产物 stdlib json 可解析，展平**恰 1 个叶键** = `plugins.entries.memory-core.config.dreaming.enabled: false`，嵌套树 `plugins.entries.memory-core.config.dreaming.enabled`，零额外键。
- **manifest 解析独立复核（integrator 实跑）**：`yaml.safe_load` → 77 条 entries（本包前 74 条，+3 为本包）；本包 3 条逐项核对通过（`src` 在仓内、`dst` 全部落 `/opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/`、`owner: deploy.sh` / `verify: md5` / `restart: none`）。另有 1 条无 `src` 键的 entry（`dst = …/xiaozhi-docker-data/.config.yaml`、`owner: overlay`、`verify: marker`）为既有形态（secret 承载渲染件，禁 md5），与本包无关。
- **`docs/` 与 `firmware/` 零 diff 事实行**：`git status --porcelain docs/`、`git status --porcelain docs/contracts/`、`git status --porcelain firmware/` 均为空。final 相位真编译重生成的 `firmware/sdkconfig`（+5 行 Image Effects Configuration 空块）已 `git checkout` 还原（闸副作用，非本包改动）。
- **round 历史**：r1（scoped，ok:true，8 闸）→ r2（final，ok:true，10 闸全绿）。无红轮、无 fixer 返工轮（iterationBudget 2 / fixBudget 3 均未消耗）。

## 3. demo 对账行（workstream 包增锚 ①）

plan.md「demo 证据形态」逐项对账（host 侧 = 冻结件真跑承载；live 侧 = 零 host 断言、归 human 部署轮）：

| plan 声明 | 本次实证形态 | 一致性结论 |
|---|---|---|
| host：冻结件真跑（声明形状） | `test_dd_a1_declaration_exactly_one_leaf_and_strict_false`（changes 恰一条、value 严格 `is False`、无顶层 plugins 镜像）+ `test_dd_a5_negative_anchor_no_other_memory_core_keys`（负锚：memory-core 路径只允许 dreaming.enabled 一叶）真跑绿 | 一致 |
| host：gen-patch 单叶 | `test_dd_a2_gen_patch_single_leaf_zero_config_io` 真跑（子进程 `--gen-patch`）+ integrator 独立实跑复算：展平恰 1 叶键、值 `false`、零 config 读写（config 路径不存在仍可生成、config 字节不变、零备份） | 一致 |
| host：drift 两态接受 | `test_dd_a3_accepts_fresh_with_tool_face_upgraded`（dreaming 叶 unset = fresh 机 + 工具面升级态 → 接受并进入编排）；`test_dd_a3_drift_guard_acceptance_surface` 另钉判定面恰四条叶路径与候选集逐字（目标态 false 由 DD-A4 SKIP 路径承载） | 一致 |
| host：drift 两态拒绝 | `test_dd_a3_rejects_true_and_tool_face_regressions` 真跑六态：dreaming 显式 `true` / allow 回退四项 / profile 缺席 / profile 非 full / deny 非空 / 父键畸形（`_MALFORMED`）→ 一律 exit 3 零写零备份零外部命令（原 plan 措辞为「两态」，实现扩为六态，强于声明） | 一致（强于声明） |
| host：幂等 SKIP | `test_dd_a4_target_state_skips_bytewise_noop`：config 已达目标 → rc 0、打印 SKIP、config 逐字节不变、零备份、注入桩零记录 | 一致 |
| host：幂等 SKIP + WARN（architect 1g 裁决 ②） | `test_dd_a4_skip_before_drift_warns_on_tool_face_drift`：dreaming 已 false + 工具面 allow 回退四项 → 仍 rc 0 SKIP、零写零外部命令，但 stdout 出现 WARN 且携带漂移锚点（tools / allow）——非静默、exit 0 不变 | 一致 |
| host：注入桩编排序 | `test_dd_apply_safe_orchestration_call_order`：首个外部命令 = `config patch --dry-run`；序链 dry-run → 正式 patch（`--file`、无 `--dry-run`）→ validate → `systemctl restart openclaw-gateway` → `openclaw health` → NRestarts；零 plugins install/inspect/doctor；**逐调用 config-sha256 协议**证明正式 patch 及其前零改写、其后无额外改写；终态 == 声明目标态且工具面升级态原样保留；备份内容 == 变更前 config 字节、名含 UTC 时间戳。另 `test_dd_apply_failure_stops_and_writes_nothing`（dry-run 失败 / 正式 patch 失败两序：其后步骤零调用、config 全程不变）与 `test_dd_apply_source_has_patch_entry_and_no_direct_config_write`（源锚：含 `config patch` 字面、无 config 直写路径） | 一致 |
| host：回滚 | `test_dd_rollback_dry_run_writes_nothing`（`--dry-run` rc 0、config 字节不变、不注入桩即证零外部命令）+ `test_dd_rollback_restores_and_validates_before_restart`（整档还原 → validate → 通过才 restart；validate 失败不重启并 exit 非 0）+ `test_dd_rollback_source_mentions_validate_and_distinction`（头注区分「还原合法快照」与「禁拼写新目标态」） | 一致 |
| host：manifest 三件登记 | `test_dd_manifest_registers_three_assets` 真跑 PyYAML 解析（src 在场 / dst 消费位 / owner·verify·restart 三字段）；integrator 另独立解析复核 77 条 entries、本包 3 条逐项通过 | 一致 |
| host：零 secret / openclaw.json 不入仓 | `test_dd_no_openclaw_json_in_repo_and_no_secret`（仓内零 `openclaw.json`、声明无 `gateway.token` / token 键 / `sk-` 长键）；integrator `git ls-files | grep -c "openclaw.json$"` = 0 | 一致 |
| live：`openclaw config patch --dry-run --json` 接受产物 | **未发生（按设计）**——OpenClaw 自有 zod schema 校验只能在 ECS 跑；offline 只钉 JSON 子集形状与叶键集恰一。归 human 部署轮，命令单 §5 第 ② 步 | 一致（申报为未覆盖面，§4 第 1 条） |
| live：`openclaw.json` 实落态 | **未发生（按设计）**——仓内不重复取证；另申报合并语义残余风险（整对象替换 vs 逐叶合并仓内无取证），命令单 §5 第 ④ 步以 `config get plugins.entries.memory-core` 前后比对承载 | 一致（§4 第 2 条） |
| live：重启后 `openclaw health` | **未发生（按设计）**——health 由 apply 编排第 ⑥ 步在 live 执行，host 侧只以注入桩证「调用序正确且在 NRestarts 前」；NRestarts 值判定亦只在 live 有意义 | 一致（§4 第 4 条） |
| live：一个调度周期无新增 `dreaming-narrative-*` 会话 | **未发生（按设计）**——`0 3 * * *` 周期观察归 human 部署轮；runbook §9 把四家族盘点固化为升级后例行检查 | 一致（§4 第 5 条） |
| 停等位 | `awaiting-human:live-deploy`——live 四项归 human 部署轮，命令单见 §5；Pod 不自行上 ECS | 一致 |

## 4. 未覆盖面申报（workstream 包增锚 ②）

对照本包 `acceptance[]`（DD-A1..DD-A5）、`exitCriteria`（11 条）、`contractRefs`（`docs/contracts/interface-contract.md`，本包零字段变更）与 plan 目标态（1–6 条），逐条列本次无验收断言 / 未跑的设计性质：

1. **`config patch` 对 `plugins.entries.memory-core` 的合并语义（整对象替换 vs 逐叶合并）——仓内无取证**。offline 只能证 patch 产物的**形状**（单叶嵌套树）与本地 fixture 上的写入口模拟；真实 OpenClaw 是「逐叶合并」还是「整对象替换」决定会不会挤掉 memory-core 的其它 config 键。**去向**：部署轮 `config get plugins.entries.memory-core` patch 前后比对，差异只允许 `dreaming.enabled`（命令单 §5 第 ④ 步）；若发现额外键丢失，停行回滚并回报，不得在仓内放宽。
2. **live `dreaming.enabled` 实值形态**——若显式 `true`（非缺省），apply 会 fail-closed exit 3（这是设计：DD-A3 拒态）。**真值是否为显式 true 仓内不可知**。**去向**：部署轮以 exit 3 报文 + `config get` 现值确认来源，若确为显式态须人工裁决后再经显式换代纳入接受集，不得静默放宽。
3. **SKIP 分支「dreaming 已关 + 工具面回退」只 WARN 不阻断**——**设计决定**（architect 1g 裁决 ②；本包写目标仅 dreaming 一叶，工具面三叶是前置基线而非写目标；与 `apply_agent_tools.py` 的「联合幂等」有意不同）。验收只钉「SKIP 仍 exit 0 + stdout 出现 WARN + 可辨漂移落在工具面」，不钉句式与完整 drift_problems 文本。**现状**：`test_dd_a4_skip_before_drift_warns_on_tool_face_drift` 承载。**去向**：无（这是接受的行为，不是缺口）；工具面回退的第二道防线 = 261025 自身验收 + honcho 声明 drift_guard。
4. **`--settle-seconds`（默认 5 秒）的 live 观察窗**——apply 在 restart 后 sleep 再跑 `openclaw health`；5 秒是否足够让网关完成加载在仓内不可证（host 侧注入桩是瞬时返回，sleep 只在 live 有意义）。**去向**：部署轮若 health 偶发红而网关日志显示「尚在启动」，把 settle 窗口作为运维参数上调（`--settle-seconds`），不改代码；这也是为什么参数做成 CLI 而非常量。
5. **NRestarts 崩循环核对的判定强度**——脚本解析 `systemctl show -p NRestarts`，**> 0 即报崩循环**；但 NRestarts 是累计值（非本重启窗口增量），长期运行的网关历史重启会积累。**现状**：值不可解析时打印 note 并以 health 回执 + 网关日志为准（fail-soft，非静默）。**去向**：部署轮以「重启前后 NRestarts 差值」人工判读；若要机械钉差值须改实现，属新行为须新验收断言。
6. **rollback 直写还原快照 vs「禁脚本直写新目标态」的区分**——rollback **确实直写 config 文件**（`write_text(backup 内容)`），但写的是 apply 前被网关正常加载过的**已知合法快照**，不拼装新目标态、不绕过 zod；禁令针对的正是「拼装新内容绕过校验」（K3 exit 78 崩因）。**承载**：头注显式区分（`test_dd_rollback_source_mentions_validate_and_distinction` 源锚）+ 还原后先 `config validate` 再 restart（行为）。**残余**：还原动作本身无 zod 前置校验，靠「快照来源合法 + validate 后置」兜底；这是分级的第 ② 级回滚，与 plugins disable / uninstall 同族。
7. **「零直写 openclaw.json」的行为证明存在离线残余盲区**（冻结契约已申报，与 K3-E6 同款）：若编排器在桩的正式 `config patch` **之后**再做冗余直写，逐调用 hash 协议不可分辨（终态相同）。缓解 = 两个失败序场景（dry-run 失败 / 正式 patch 失败）下任何直写都会被终态字节 + 逐调用 hash 双查捕获，且源锚（无 config `write_text` 直写路径）为第二道。**去向**：部署轮可对 apply 前后 `config get` 全量 diff 旁证。
8. **runbook 升级清单的「钉一行」不钉物理行数**——冻结契约只钉语义锚组（四家族 token + 「必须为空」或等价口径 + 引 debt 条目）与一处 ±900 字符窗口内聚，不钉行数、不钉清单位置。**理由**：物理行数不是可机判语义，钉死会制造脆性。**去向**：人工 review（本 packet §3 已逐项对账内容）。
9. **声明 `_note` 只钉「非空字符串」**，不钉「全局关闭 / 无 per-agent 作用域 / 重评触发」三句的句式——避免脆性句式断言；内容口径由本 packet 与 docs-delta 人读承载。
10. **注入桩旗标拼写自由度**——rollback 接受 `--systemctl-bin`（plan 口径）或 `--openclaw-bin`（与 apply 同形单桩）二者其一；钉的是「注入通道存在 + 调用序 + 还原语义」，不钉旗标拼写。二者都缺则红（离线不可跑）。
11. **live 面零 host 断言**（显式申报）——`config patch --dry-run` 接受产物 / `openclaw.json` 实落态 / 重启后 health / 一个调度周期无新增 `dreaming-narrative-*` 会话 / 真机旧事抽问不劣化，均归 human 部署轮，本件不设 live 用例。
12. **数据清理不在本包**——human 2026-10-25 裁决 ② 明确归运维轮（先按 provenance 导出 → 等 deriver 队列排空 → 删结论 → 删会话 → 计数归零 + 真机复测）。本包零库写、零数据迁移；debt 条目 `status: deferred` 保持，不预关。

**零缺口项：`none`**——除上述已逐条申报的设计面差集（1、2、4、5、6、7 为真实未知或强度边界；3、8、9、10 为有意不钉的措辞/形态自由度；11、12 为跨面归口）外，`acceptance[]` 五项（DD-A1..DD-A5）与全部 `exitCriteria` 的 host 断言均在冻结件内真跑转绿，无其它未覆盖面。

## 5. live 部署轮命令单（**只列不跑**；Pod 不执行，human 授权轮照敲）

> 前置事实：三件工件先由 deploy.sh 按 manifest md5 铺放到 `/opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/`。当前 live 工具面已是升级态（`profile:"full"` / `allow` 五项 / `deny:[]`，本包 drift_guard 锚其形态）。

```bash
# ① 生成 patch（host 已验形状：展平恰一个叶键）
python /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/apply_dreaming_disable.py \
  --gen-patch /tmp/dreaming-disable.patch.json
#    判据：rc 0；产物 stdlib json 可解析；展平叶键恰
#          {plugins.entries.memory-core.config.dreaming.enabled: false}
#    停行：rc != 0 或叶键集不符 → 停，回报

# ② dry-run schema 校验（OpenClaw 自有写入口校验；零写）
openclaw config patch --file /tmp/dreaming-disable.patch.json --dry-run --json
#    判据：回执 checks.schema = true
#    停行：schema 非 true / 非零退出 → 不改配置，回报

# ③ 快照 memory-core 现值（合并语义比对基线，只读）
openclaw config get plugins.entries.memory-core > /tmp/memory-core.before.json

# ④ 正式应用（脚本内自带：写前时间戳备份 → dry-run 核 schema → 正式 patch
#    → config validate → systemctl restart → openclaw health → NRestarts 核对）
python /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/apply_dreaming_disable.py
#    判据：rc 0；末行 APPLIED；备份名含 openclaw.json.bak-kidpal-dreaming-<UTC>
#    若 exit 3（drift STOP）：按报文核对是哪条叶路径不中——dreaming 显式 true 须
#      人工确认来源后再裁；工具面三叶不中说明 261025 工具面回退，先修工具面
#    停行：任何非 0 → 不进入观察期

# ⑤ 合并语义核对（patch 前后比对，差异只允许 dreaming.enabled）
openclaw config get plugins.entries.memory-core > /tmp/memory-core.after.json
diff <(python -m json.tool /tmp/memory-core.before.json) \
     <(python -m json.tool /tmp/memory-core.after.json)
#    判据：差异只允许 config.dreaming.enabled = false
#    停行：其它键丢失/改动 → 立即回滚（第 ⑦ 步）并回报

# ⑥ 观察一个调度周期（cron 0 3 * * *）
#    · 次日 03:0x 后跑 Honcho 会话家族盘点：dreaming-narrative-* 必须为空
#    · openclaw health 保持绿；journal 无新 event_loop_delay 存活告警
#    · 真机旧事抽问（如「王一诺是谁」）不劣化（画像窗口释放 13% 的收益验证）

# ⑦ 回滚（如需；一步还原 apply 前时间戳快照）
python /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/rollback_dreaming_disable.py --dry-run
#    先看意图（rc 0、零写零重启）
python /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/rollback_dreaming_disable.py
#    还原 → config validate 通过才 restart；validate 失败则不重启并报错

# ⑧ 本包之后的重复运行是安全且预期的
python /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/apply_dreaming_disable.py
#    dreaming 已 false → SKIP exit 0，零写零备份零外部命令；若工具面此时已漂移
#    → 仍 exit 0 但打印 WARN（非静默）；确认工具面后再跑 261025 工具面包
```

## 6. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 幂等判与 drift 严格布尔判的语义不对称 | `main()` 的幂等预检用 `target_state(config, changes) == config`（Python dict 相等），而 `_alt_hits` 对布尔候选用 `cur is False` 严格判。若 live `dreaming.enabled` 是整数 `0`，Python 下 `0 == False` 使预检判为「已达目标」→ 走 SKIP 分支（此时 `drift_problems` 会把该叶列为 WARN，但 exit 0），而非 DD-A3 语义下的 exit 3。结果 = 零写零外部命令 + WARN 可见，属非破坏性 fail-open | changed-code hunk：`apply_dreaming_disable.py:448`（`if target_state(config, changes) == config:`）对照同文件 `:146-155`（`_alt_hits` 的 `cur is False`）与 `:19-28` 头注（architect 1g ① 要求布尔叶严格判）；judgement heuristic（`0` 只能来自畸形 live 配置，且 SKIP 已打 WARN 零写） | 不改（严格化幂等预检会打破「SKIP 先于 drift」的既定设计取舍，属行为换代须新验收断言）；部署轮若真见 `dreaming.enabled: 0`，按「显式态需人工确认」口径处理（命令单 §5 第 ④ 步停行条款） |
| FYI | judgement-call | 路径写入/读取的畸形语义不对称 | `_json_set` 对「父键在场但非对象」的中间节点静默替换为 `{}`（fail-open），与 `_json_get` 的 `_MALFORMED` fail-closed 哨兵语义不对称。apply 路径当前不可达：幂等预检先判不等 → drift_guard 对 dreaming 叶路径必得 `_MALFORMED` → exit 3 零写 | changed-code hunk：`apply_dreaming_disable.py:107-116`（`_json_set`）对照 `:92-104`（`_json_get` 与 `_MALFORMED`）；judgement heuristic（无外部可达路径） | 不改（与同族 `apply_agent_tools.py` 同形）；若未来声明新增未被 drift_guard 覆盖的叶路径，须同批补对应叶谓词或让 `_json_set` 遇非 dict 中间节点 fail-closed |
| FYI | judgement-call | 声明写入面校验粒度 | `_validate_changes` 只校验 `{path, value}` 形状，不把 path 限定在 `plugins.entries.memory-core.config.dreaming.enabled` 枚举内 | changed-code hunk：`apply_dreaming_disable.py:350-363`；judgement heuristic（声明是仓内受审工件，DD-A5 负锚在验收侧机械承载：changes 路径集恰一条 + memory-core 路径只允许 dreaming 叶） | 不改；声明任何改动走仓内评审 diff |
| NIT | judgement-call | rollback dry-run 的备份前置依赖 | `rollback_dreaming_disable.py` 在进入 `--dry-run` 分支**之前**解析备份路径，备份缺失即 return 2；即在没有备份的机器上 dry-run 报错，而非打印「无可还原稿」。对照 apply 的 `--gen-patch` 是零依赖形态 | changed-code hunk：`rollback_dreaming_disable.py:122-134`（备份解析早于 `if args.dry_run`）对照 `apply_dreaming_disable.py:424-435`（gen-patch 零 config 依赖） | 不改（「无备份即无法回滚」本身是事实；dry-run 的典型用法是在已有备份的机器上预演，命令单 §5 第 ⑦ 步即如此）；若运维希望在无备份机上预演，另起行为换代 |
| FYI | judgement-call | dry-run 回执的判定强度 | `_schema_ok` 只核回执中 `checks.schema is True`，不解析回执其余字段（是否提示叶覆盖 / 丢弃键）。合并语义风险不由此处兜底 | changed-code hunk：`apply_dreaming_disable.py:248-266`；plan.md「部署轮残余风险」已申报 | 不改；命令单 §5 第 ⑤ 步以 `config get plugins.entries.memory-core` 前后比对承载 |

无 BLOCKING、无 SHOULD、无 hard-violation。结构闸（verify.mjs 10 闸）的 PASS/FAIL 不复述为 finding；gate 全绿仍完整执行五轴语义 review（§7）。

## 7. Five-Axis Review

1. **Correctness**：写入面是**恰一个布尔叶键**这一最小形态，直接对应根因——dreaming 无 per-agent 作用域，只能全局关；关闭后做梦子回合不再产生，`dreaming-narrative-*` 会话家族自然归零。drift_guard 四叶 AND 把接受集（dreaming unset = fresh 机 / false = 目标态）与拒态（显式 true / allow 四项目回退 / profile 缺席 / profile 非 full / deny 非空 / 父键畸形）逐项分开，且 `_MALFORMED` 哨兵消除了「父键非对象」被误当「键缺席」按 unset 放行的静默覆盖风险；布尔叶 `cur is False` 严格判堵住 `0 == False` 的 Python 陷阱。**SKIP 先于 drift** 是本次有意的设计取舍：写目标只有 dreaming 一叶，工具面三叶是前置基线，达标即零外部命令；为避免「dreaming 已关 + 工具面回退」完全静默，SKIP 分支把 `drift_problems` 打印为 WARN（exit 0 不变）——该差异与 `apply_agent_tools.py` 的「联合幂等」有意不同，已双向留痕（脚本头注 + 验收件头注）。编排序把 `config patch --dry-run --json` 钉为首个外部命令，是 K3 血教训（直写绕过 zod → 网关 exit 78 崩循环）的直接防线；health 之后补 NRestarts 核对，防止「重启成功但进崩循环」这种假绿。rollback 的「先 validate 再 restart」把还原失败收敛为「不重启、报错、网关仍跑内存态」，而不是把坏配置推进去重启。
2. **Test quality**：17 条全部真跑（tempfile fixture + 子进程 `sys.executable <script>` + 内嵌 openclaw 桩），不是读字符串的形式断言。**逐调用 config-sha256 协议**是本件最有力的一条：桩每次被调时记录当时 config 的 sha256，从而证明「正式 patch 及其之前 config 字节不变、其后无额外改写」——这比「终态正确」强得多，因为终态相同无法区分中间是否被直写。失败序两条（dry-run 失败 / 正式 patch 失败）各自断言后续步骤零调用 + 终态字节不变，把「失败即停、失败序零写」钉死。负向断言齐：DD-A1 严格 `is False`（`0`/`"false"` 都不算）、DD-A5 负锚（changes 路径集恰一条 + memory-core 路径只允许 dreaming 叶 + 声明字符串值不得携带第二个 memory-core 路径）、零 secret / openclaw.json 零入仓。DD-A4 的 SKIP 子 case 把「WARN 非静默」也钉进机械判。mutation 豁免在本包成立：声明 / 配置脚本 / 部署工件，冻结断言已把形状与语义逐项钉死（`mutationSkipReason` 在册、`mutationEvidence.perAcceptance` 空），且反向变异已在 acceptance-author 阶段实测（脚手架已删）。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/decisions/ADR.md` 零 diff（`git status docs/` 空）；`AGENTS.md` 零 diff。契约面零字段变更（`contractRefs` 列 `interface-contract.md` 仅为在册回指，本包未触其任一节）。`crossEnd.exempt` 成立：端→网关消息、帧类型、字段承载与序列化形态零变化，设备端零感知、固件零 diff。声明工件是部署配置（零 secret），不是接口类型定义；`openclaw.json`（含 gateway 凭据）整文件零入仓（`git ls-files` 实测 0 命中）。
4. **Operability/security**：工具与运维面**只减不增**——关闭 dreaming 后不再产生后台自回合（省一次每日 LLM 生成，缓解 ECS 2C/4G 与引擎共置的 event_loop_delay 峰值），不再向 Honcho 写幻觉叙事；零新增出网、零权限扩大、零 secret 触碰、零数据迁移、零库写。配置写入口纪律 = 只经 OpenClaw `config patch`（写前 dry-run 核 `checks.schema === true`），脚本零直写路径（源锚 + 逐调用 hash 双证）。drift 面 fail-closed（六态 exit 3 零写零备份零外部命令），幂等面零副作用（SKIP 零写零备份零外部命令）。回滚可操作：一步还原 + validate 后重启 + `--dry-run` 预演。升级清单把「四家族必须为空」固化为升级后例行检查，防止上游升级悄悄恢复采集。残余风险已在 §4/§9 逐条申报（合并语义、显式 true、settle 窗口、NRestarts 累计值）。
5. **Maintainability/simplicity**：改动局部、职责单一——apply 475 行里约 1/3 是冻结契约头注与中文错误文案，实际逻辑是「点分读写 + drift 判定 + 编排」三段；rollback 164 行同族形态、与 apply 共享注入桩约定。没有为「以后可能的形态」预造配置项（无顶层 plugins 镜像、无泛用 rule 引擎、无新增抽象层）；memory-core 为 bundled 插件的判断直接体现为**删掉** plugins install/inspect/doctor 三步（少代码不是多代码）。无新依赖（stdlib），无跨模块引入，无环。`minimal:` 标记零命中（§8）。

## 8. Simplification Debt（`minimal:` 台账）

grep `minimal:` 在本次全部改动文件（三件新建工件、manifest、runbook、冻结验收件）**零命中**——本包未新增简化债务，无待登记条目。**no-trigger。**

（承接说明：apply 头注把「与 `apply_agent_tools.py` 的有意差异」写清而非统一实现，属**刻意的形态分化**、不是简化债——统一反而会把「SKIP 先于 drift」与「联合幂等」两种正确语义混成一种错误语义。）

## 9. Residual Risk

- **`decisionCoverage` 筛选**（`landsIn === 'dropped' && reason.startsWith('risk-accepted')`）：**`none`**——本包 `taskpackage.json` 无 `decisionCoverage` 字段；human 2026-10-25 四裁决与 architect 1g 五条预评结论全部落进 `exitCriteria` / `notes` / plan.md「技术方案」，无 risk-accepted dropped 条目。
  - 相邻申报（供 supervisor 对照，非 dropped 条目）：human 裁决 ① 明确接受两项**已知损失**——「main 侧失去自动巩固（当前零产出）」与「dreaming 开关无 per-agent 作用域」；两者写在 `_note` 的重评触发里（若 main 出现「手动 MEMORY.md 维护不可接受」的实际诉求，须整体重开并重评 kidpal 侧污染），不是丢弃项。
- **Known limitation**：① `config patch` 对 `plugins.entries.memory-core` 的合并语义仓内无取证（§4 第 1 条），部署轮 `config get` 前后比对兜底；② live `dreaming.enabled` 若为显式 `true`，apply 会 exit 3，须人工确认来源（§4 第 2 条）；③ `--settle-seconds` 5 秒窗口是否足够在仓内不可证（§4 第 4 条）；④ NRestarts 是累计值、非本重启窗口增量，判定强度有限（§4 第 5 条）；⑤ 零直写的行为证明存在「正式 patch 之后冗余直写」盲区（§4 第 7 条），由失败序 + 源锚兜底；⑥ 幂等预检与 drift 严格布尔判在 `0` 值上不对称（finding 1），结果非破坏性；⑦ rollback dry-run 依赖备份存在（finding 4）。
- **已知有意选择（供 supervisor 复核）**：① builder 未新增独立单测文件——`changeManifest` 内测试面只有一份冻结验收件（归 acceptance-author），行为级真跑承载分支覆盖；② SKIP 分支只 WARN 不阻断（§4 第 3 条，architect 1g 裁决 ②）；③ 回滚脚本对 config 的直写是「还原合法快照」而非「拼装新目标态」，头注显式区分（§4 第 6 条）。
- **Follow-up recommended**：① 面 C spec 修订（`docs/architecture.md` §6.7 类级规则 + ADR-016 日期注记）按 `TECH-261025` 走 `/arch-change`——本包不合、不写，但「不允许先合实现、spec 永远不补」的约束仍然在账；② 数据清理 = human 运维轮（先导出 → 排空 deriver 队列 → 删结论 → 删会话 → 计数归零 + 真机复测）；③ 未来若起探针包，须把 `subagent:*` 一并列入「必须为空」集合（debt 条目「相邻同类缺口」节）。
- **Human decision needed**：部署轮授权与执行时机（本包只列命令单、不跑）；若部署轮实测合并语义为「整对象替换」且丢失 memory-core 其它键，是否接受「补声明其它键」的换代方案需 human 裁；live 见显式 `true` 时的接受集换代同需 human 裁。
- **停等位**：`awaiting-human:live-deploy`——live 四项归 human 部署轮，命令单见 §5；Pod 不自行上 ECS。

## 10. 集成执行回报（Merge-Chain Execution 节）

未触发——本包无 merge-driver 冲突 abort 派单，未执行 rebase / 冲突消解；集成树复跑清单 N/A（本包范围圈定终验已按 §2 跑绿：10 闸全绿 + 圈定 10 件 162 passed + 全量 370 passed，零回退）。

## 11. UI design-review 证据

N/A——本包是云端配置声明 + apply/drift/rollback 脚本 + 部署 runbook 一行，无 UIUX 面；仓内不存 screenshot / video / HTML 证据。

## 12. 架构 / 依赖卫生

- **零新增第三方依赖**：apply / rollback 纯 stdlib（json / argparse / shutil / subprocess / tempfile / datetime / pathlib / re）；验收件仅用 PyYAML（既有 manifest 断言的同一解析器，`verify-deploy.sh` 同源）。
- **无跨层调用**：三件工件是独立 CLI / 数据文件，仓内唯一引用者是冻结验收件（按路径以子进程调用）与 `deploy.sh`（按 manifest md5 铺放）；无 Python import 面，不进入 front / 引擎 / gateway 的调用链；零端口、零出网、零 DB。
- **无循环依赖**：单文件脚本 + 声明 JSON + 验收件，无环。
- **无绕过 contracts 的本地类型**：零契约面触碰；声明工件是部署配置，不是接口类型定义。写入面是**单个布尔叶键**，没有引入任何新的数据结构或序列化契约。
- **graph 复核（`detect_changes`，project `C-AI-KidPal-pod-m2`，base `main`，merge_base `fa60dd2f`）**：`changed_files` **11**（8 个文件 + 本包目录 + 新建 deploy 目录 + 新建验收件）、`seed_symbols` **88**、**`impacted_total = 2`**：
  - `server/tests/test_story_cancel_lifecycle_units._FrontSandbox.tearDown`（hop 1，module `server/tests`）
  - `server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py.__file__`（hop 1，module `server/acceptance`）
  - 判读：production 代码**零调用方**（本包改动全是 `.md` / `.yaml` / `.json` / CLI 脚本 + 一个新测试文件，仓内无 Python 模块 import 它们）。第一条边是 **heuristic 名称/词元匹配**——该 method 的 body 操作 `f._openclaw_chat_once`，而新增验收件的 stub/docstring 大量出现 `openclaw` / `systemctl` 词元；无 import / call 边（`callers: 0`）。**安全余量实测**：integrator 另单独跑该件 = **11 passed（0.13s）**，零回退。爆炸半径自包含，与 test-scope 清单 10 件圈定一致；per-Pod 专属图索引正常圈定、**无降级**（detect_changes 与 trace 均可用）。
  - **差集留根**：blast radius 里那条 `server/tests/…` heuristic 边**未**加入本轮 scope 清单（scope 由 planner 圈定为「本包 acceptanceTestsFile ∪ manifest 共享 seam」10 件）；按「安全余量可加不可缩」原则，integrator 以**单独跑该件**的方式补证零回退，并把该差集如实记在此处，供「圈定 vs 事后全 blast radius」观察。
- **`adrRequired=false` 成立**：技术权威 = 诊断件 §4 architect 裁决（方案 A）+ architect 1g 五条预评档；本包是把一项已被 human 裁决的配置能力落进仓，无新的架构结构 / 契约裁决需要归档 ADR。与之相邻的 spec 修订（§6.7 / ADR-016）已归面 C，不在本包。

## 13. 制品与 commit 状态

- closeout 制品（均落 `delivery/taskpackages/261025-openclaw-dreaming-disable/`）：`test-scope-final-r2.txt`（圈定清单，10 行）+ `docs-delta.md`（零 diff 声明 + 部署 face 记录 + spec 在途归口）+ 本 packet + `verdict.json`（final round 2 immutable fact 逐字晋升 = `verifier-evidence/rounds/r2.json` 副本，supervisor-compatible shape）+ `verification.json`（advisory 投影：finalScopedRun r2 + priorRounds r1 + scopeDetection + docsSync + provenance）。
- **不产**（post-cutoff）：`pod-retro.md`（planner 收尾相位）、`session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`（EVENTS_SCHEMA_CUTOFF 之后退役；friction 已入 `pod-events.jsonl`）。`pod-events.jsonl` 存在性检查作 advisory，缺失不阻塞 closeout / supervisor / merge。
- 簿记面：本包派单边界**包含** `delivery/workstreams/device-memory-overhaul/{progress.md, roadmap.md}` 与 `delivery/workstreams/INDEX.md`（planner 起包记账与 hook 生成件）以及 `delivery/taskpackages/current/` 三件投影（verify.mjs 程序 fs-write），随本包一并 commit。`changes-index.mjs --upsert 261025-openclaw-dreaming-disable` 写 dev lane INDEX（`delivery/taskpackages/INDEX.md`）。
- commit：`dev/pod-m2` 分支（实现面 6 件 + 簿记面 + 本包全部制品），**不 push**；`.codebase-memory/**` 两件不入 commit（机器再生面）；final 相位真编译重生成的 `firmware/sdkconfig` 副作用已还原。
- **跨面变更回写（spine 在册，261015 批 T5）**：本包不在跨面 intake spine 的 facePlan 内（human-ad-hoc 单面包，`attach-existing` 挂 `device-memory-overhaul` workstream），无 spine anchors / status 需主线程回写；workstream 的「完成后人能看到什么 / 达成状态 pending → done」由 planner 在合并后推进。
- 后续门：`supervisor-verdict-gate.mjs`（planner 跑）→ `pre-merge-escalation-gate` → `merge-driver --pod`（planner 主线程）。high riskTier ⇒ 按规则**升级 human 复核**；live 部署轮独立于合并门，归 human 授权（命令单 §5）。
