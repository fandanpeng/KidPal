# Review Packet · 261025-kidpal-tools-profile-fix

> integrator 合并前审查包，2026-10-25，final round 2（invocationId `a978bda7-cb8e-4139-b977-48be0a3fc768`）。等待 supervisor 裁断（由 planner 跑 `supervisor-verdict-gate.mjs` 机械闸）；不 push、不 merge、不碰 ECS。上游 = `delivery/taskpackages/261024-memory-recall-tool-enable/taskpackage.json` notes ⑦ 跨包陈旧基线 follow-up + ECS live 实证（provenance：`systemPromptReport.tools.entries` 仅 `[read, write, apply_patch, exec, web_search]`；per-agent `agents.entries.kidpal.tools.profile="full"` 覆盖全局 `coding` 后 `honcho_search_conclusions` 可达、答出 9-30 事实「王一诺」）+ human 2026-10-25 指示「tools.profile full 修复落到库里」。无 upstream PROPOSAL；技术权威 = 本包 plan.md 记载的 live 实证。story 语义权威 = `docs/requirements.md` 现文 `S-F5.1`（AC2 跨会话回忆）。

## 1. Change Size

| 文件 | +/− | 内容 |
|---|---|---|
| `server/deploy/261022-openclaw-kidpal-tools/kidpal-agent-tools-declaration.json` | +22/−7 | 写入面单键 `change{path,allow}` → 双叶键 `changes[{path,value}]`（`tools.profile="full"` + `tools.allow` 恰五项）；`drift_guard` 由单条 object 候选 → 三条叶路径 AND（`tools.deny` expect_any `["unset",[]]` / `tools.profile` `["unset","full"]` / `tools.allow` `["unset",[四项],[五项]]`）；`_note` 显式换代留痕（含放宽项申报） |
| `server/deploy/261022-openclaw-kidpal-tools/apply_agent_tools.py` | +94/−44 | `apply_change` → `apply_changes`（多叶写入 + `_deep_copy`）；`_json_get` 拆 `_MALFORMED` 哨兵（父键在场但非对象 ≠ 键缺席）；幂等预检与 drift 联合（两叶达标 **且** 基线无漂移才 SKIP）；`_validate_changes` 声明形状校验；`--gen-patch` 产物两叶键；头注换代 |
| `server/deploy/261022-openclaw-kidpal-tools/rollback_agent_tools.py` | **0/0** | **核验零改申报**（§4 第 4 条）：备份文件整档还原机制与键数无关，profile 键自动覆盖；仍在 changeManifest 内以承载 manifest 三件齐全断言（`git diff` 空，见 §2 事实行） |
| `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json` | +2/−2 | `drift_guard.expect` 四项 → 五项（新 P1 已应用态，顺序与 P1 声明 allow 叶值逐字一致）；`_note` 同步换代 + 顺序纪律写清 |
| `server/deploy/261022-openclaw-honcho-plugin/apply_honcho_plugin.py` | +3/−1 | **仅头注换代**（`drift_problems` 代码零改，方案 A）：现状锚「tools.allow 四项在位」→「五项在位，2026-10-25 随 261025 显式换代，代码零改」 |
| `server/deploy/manifest.yaml` | +4/−4 | L251 kidpal-tools 声明 note「恰四项」→「两叶写入面（profile=full + 恰五项…）」；L255 apply note「只改 …tools.allow」→「只改 …tools.{profile,allow}」；L313 注释「四项」→「五项」；L317 honcho 声明 note「四项已应用态」→「五项已应用态」。**实现选择（须人读）**：L255 note 含 `{profile,allow}` 花括号，YAML flow mapping（`- {src: …, note: …}`）里裸花括号会被解析成嵌套 flow mapping，故该条 note **必须整条加双引号**——这是 YAML 语法约束，不是文案风格。已由 `yaml.safe_load` 实测解析（74 条 entries，本目录 3 条；§2）|
| `server/acceptance/test_261022_openclaw_kidpal_tools.acceptance.py`（冻结件） | +865/−350 | K1-E4 显式换代（单键→双叶、drift 三叶基准、gen-patch 两叶、`_MALFORMED`）；新增 `test_k1_e4_declaration_note_records_drift_upgrade_scope` / `test_k1_e4_apply_skips_live_upgraded_state` / `test_k1_e4_apply_deny_nonempty_is_drift_exit3` / `test_k1_e4_gen_patch_product_exactly_target_leaves_and_zero_config_write`；新增 `ToolsProfileReachabilityAcceptance` 四条（K1-E4-C 防回归锚）；K1-E4-B（MemoryRecallRoundAcceptance）**语义零改**（仅类 docstring 加一句「本包（261025）零改动本类」留痕） |
| `server/acceptance/test_261022_memory_isolation.acceptance.py`（冻结件） | +123/−26 | K3-E6 跨包锚**显式换代**：`K3_P1_TOOLS_EXPECT` 四→五项 + `K3_P1_TOOLS_PROFILE="full"` + `K3_P1_TOOLS_LEGACY_FOUR`；`_fixture_p1_applied` 组合态改 `{profile:"full", allow:五项, deny:[]}`；`test_k3_e6_apply_drift_guard_exit3` 追加第三条变异「allow 仍四项 → exit 3」；manifest note 负锚（不得再写「四项」）+ 五项语义锚；头注 / docstring 四处留痕 |

已跟踪实现面合计 **+1113 / −434（7 文件）**；`rollback_agent_tools.py` 登记但零 diff。另有 `delivery/taskpackages/current/` 三件投影由 verify.mjs 程序 fs-write（`input-brief.md` +39/−9、`taskpackage.json` +45/−38、`verdict.json` +27/−27），与本包目录新增制品（`taskpackage.json` / `plan.md` / `input-brief.md` / `pod-events.jsonl` / `verify-runs.jsonl` / `test-scope-final-r2.txt` / `verifier-evidence/rounds/r1.json`、`r2.json` / 本 packet / `docs-delta.md` / `verdict.json` / `verification.json`）。

零新增第三方依赖（apply / rollback 纯 stdlib；验收件用 PyYAML，属既有 manifest 断言的同一解析器）；零固件 diff（crossEnd exempt：设备端零感知）；零数据迁移；零 ECS 动作。

## 2. Verification Evidence

- **scoped round 1**（invocation `c2dd97d2-a609-45e2-bdaa-6e133c9a53a0`，10:49:17.695Z–10:49:20.035Z）：`ok:true`、`observedGateExit=0`、8 闸绿——`firmware-build` scoped 相位后置（`rigor=lite`，261015-gov-review-lite-fast-path；闸自报 `executor-skipped` 且被机器判为 `notExecuted`，非假绿），fact = `verifier-evidence/rounds/r1.json`。
- **final bearing round 2**（invocation `a978bda7-cb8e-4139-b977-48be0a3fc768`，10:51:12.248Z–10:54:30.209Z）：`ok:true`、`observedGateExit=0`、**10 闸全绿**（taskpackage-required / change-manifest〔pre-cutoff 存量放行〕/ firmware-build〔真编译，工具链可用〕/ contract-lint〔仅既有历史断链告警〕/ acceptance-coverage〔acceptance = K1-E4 / K1-E4-C / K3-E6，两文件〕/ mutation-check〔豁免声明在册：`mutationSkipReason` + `mutationEvidence.perAcceptance` 空〕/ docs-sync / security / unit / acceptance），fact = `verifier-evidence/rounds/r2.json`。
- **fact 校验**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261025-kidpal-tools-profile-fix --phase final --round 2 --invocation-id a978bda7-cb8e-4139-b977-48be0a3fc768` → `factOk:true`、`observedGateExit:0`、exit 0（integrator 实跑）。
- **test-scope 清单落盘路径（Step 4 ① 圈定锚行）**：`delivery/taskpackages/261025-kidpal-tools-profile-fix/test-scope-final-r2.txt`，**10 行** = 本包 `acceptanceTestsFile` 2 件（`test_261022_openclaw_kidpal_tools` / `test_261022_memory_isolation`）∪ `impactedTests` 种子新增 2 件（`test_261022_honcho_memory` / `test_261017_openclaw_kidpal_skills`——同一运行态 `openclaw.json` 组合面）∪ 共享 seam 6 件（`manifest.yaml` 是部署单一事实源，被 `test_261010_engine_decision21_overlay_persist` / `test_261013_qq_schedule_write_closure` / `test_261022_honcho_backfill` / `test_261022_m5_k1_prompt_carrier_migration` / `test_261022_persistent_session` / `test_prompt_v7_round_routing` 各自 `yaml.safe_load` 解析——本包改 manifest 4 行且其中一条须加引号，YAML 一旦解析破损这 6 件同红，属共享 seam 安全余量）。**留根注记**：r2 fact 的 `results` 元数据未含 scope 路径行（`verify.mjs` 未把 `--scope-file` 透传落 fact；实测 r2.json 内无 scope 字段），路径以本 packet 与 `verification.json` 的 `finalScopeFile / finalScopeNote` 承载——与上一号包同一观察差集锚。
- **圈定交集实测（r2 final）**：unit 闸 **155 passed / 3 deselected（18.93s）**、acceptance 闸 **155 passed / 3 deselected（18.23s）**，`serverAcceptanceFiles=10`、`filesWithLiveMark="2/10"`（live 标记件 = memory_isolation + honcho_memory，`-m "not live"` 下 3 条 live 用例 deselect）。其中两份被改的冻结件单独跑 = **35 passed / 3 deselected**（`git diff` 面全覆盖）。
- **全量口径（integrator 独立复跑，2026-10-25）**：`cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance` = **353 passed / 7 deselected / 33 subtests passed（29.36s）**——与 builder 全量实测、planner 独立复跑同值。对照冻结基线：冻结前同树 346 passed / 7 deselected / 33 subtests（353 收集）；冻结后设计态红 = 21 failed / 332 passed / 7 deselected / 33 subtests（360 收集，+7 = 本批新增 case）。现 21 条设计态红全转绿、冻结前已通过的 346 条零回退、净增 7 条通过。
- **gen-patch 产物独立复核（integrator 实跑）**：`python server/deploy/261022-openclaw-kidpal-tools/apply_agent_tools.py --gen-patch <tmp>/kidpal-tools.patch.json` → rc 0；产物展平 **恰 2 个叶键**：`agents.entries.kidpal.tools.profile = "full"`、`agents.entries.kidpal.tools.allow = [read, write, exec, web_search, honcho_search_conclusions]`；零 config 读写（冻结断言另有字节相等 + config 缺失仍可生成两态）。
- **`rollback_agent_tools.py` 零 diff 事实行**：`git diff --stat -- server/deploy/261022-openclaw-kidpal-tools/rollback_agent_tools.py` 为空（§1 表同步）；其行为仍由冻结件 `test_k1_e4_rollback_dry_run_and_restore` 真跑承载（在新目标态 fixture 上 dry-run 零写 / 一步还原）。
- **`docs/` 零 diff 事实行**：`git status --porcelain docs/` 为空；`git status --porcelain firmware/` 为空（final 相位真编译重生成的 `firmware/sdkconfig` 空块副作用已还原，见 docs-delta §1 附注）。
- **round 历史**：r1（scoped，ok:true，8 闸）→ r2（final，ok:true，10 闸全绿）。无红轮、无 fixer 返工轮（iterationBudget 2 / fixBudget 3 未消耗）。

## 3. demo 对账行（workstream 包增锚 ①）

plan.md「demo 证据形态」声明 =「host：验收换代后全量套件绿（基线 = 本包冻结前实跑值）+ gen-patch 产物形状断言（展平两叶键）+ drift 四态真跑；live：**零 host 断言**——`openclaw config patch --dry-run` 接受产物 / `openclaw.json` 实升级态 / 真机「王一诺是谁」问答归 human 部署轮」。逐项对账：

| plan 声明 | 本次实证形态 | 一致性结论 |
|---|---|---|
| host：冻结断言真跑 | r2 unit 闸与 acceptance 闸各真跑同一 10 件圈定集，各 **155 passed / 3 deselected**；两份被改冻结件单独 35 passed / 3 deselected（`executor-run`，子进程真跑 apply / gen-patch / rollback） | 一致 |
| host：全量套件绿（基线 = 冻结前实跑值） | integrator 独立复跑全量 `-m "not live"` = **353 passed / 7 deselected / 33 subtests（29.36s）**；冻结前 346 passed 基线、冻结后 21 条设计态红，现全转绿且 346 条零回退、净增 7 条 | 一致 |
| host：gen-patch 产物形状（展平两叶键） | integrator 实跑 rc 0，展平恰 2 叶键（`tools.profile="full"` / `tools.allow` 五项）；冻结断言 `test_k1_e4_gen_patch_product_exactly_target_leaves_and_zero_config_write` 另以 config 字节相等与「config 缺失仍可生成」两态承载零 config 读写 | 一致 |
| host：drift 四态真跑 | 四态接受 = fresh 全缺席（`test_k1_e4_apply_accepts_legacy_four_baseline` 的 unset 分支 + `test_k1_e4_drift_guard_upgrade_baselines` 基线集断言）/ 旧四项 live（同前件 legacy 分支）/ P0 中间态（`test_k1_e4_c_allow_five_with_profile_absent_is_upgradable`，rc 0 补 profile）/ live 升级态（`test_k1_e4_apply_skips_live_upgraded_state`，SKIP 零写零备份）；拒态 = deny 非空（`test_k1_e4_apply_deny_nonempty_is_drift_exit3`）/ allow 项数不符与 profile 非 full（`test_k1_e4_apply_drift_guard_exit3`、`test_k1_e4_c_profile_not_full_is_drift_exit3`）/ 父键畸形（`_MALFORMED` 哨兵 + drift 断言） | 一致 |
| live：`openclaw config patch --dry-run` 接受两叶产物 | **未发生（按设计）**——OpenClaw 自有 schema 校验只能在 ECS 跑；归 human 部署轮，命令单见 §5 | 一致（申报为未覆盖面，§4 第 1 条） |
| live：`openclaw.json` 实升级态 | **未发生（按设计）**——live 现状已由主线程实读为升级态（本包 provenance），仓内不重复取证；offline 只证声明接受四态基线 | 一致（§4 第 2 条） |
| live：真机「王一诺是谁」问答 | **未发生（按设计）**——行为观测归部署轮 | 一致（§4 第 3 条） |
| 锚兼容红线（既有冻结件继续绿） | K1-E4-B 三条（AGENTS.md 记忆回忆回合静态语义 + 锚兼容）零语义改动、r2 全绿；`test_261017_openclaw_kidpal_skills` / `test_261022_honcho_memory` / `test_261022_m5_k1_prompt_carrier_migration` 全绿；K3-E2 两条 + K3-E6 其余用例全绿（`_fixture_p1_applied` 四处同批核对：常量 / fixture / 头注 FROZEN CONTRACT §B1 / docstring） | 一致（零回退） |
| 宽容/宽松项显式申报 | 声明 `_note` 与验收件头注两处均写明「叶路径容忍 tools 未知同级键（旧多键即拒语义消失）」与「allow 顺序敏感相等（顺序不同 = drift）」 | 一致 |

## 4. 未覆盖面申报（workstream 包增锚 ②）

对照本包 `acceptance[]`（K1-E4 / K1-E4-C / K3-E6）、`exitCriteria`、引用的契约/文档条目（`docs/contracts/interface-contract.md#1.6`、`docs/requirements.md#S-F5.1`、`TECH-261023` / `TECH-261022` 债账本）与 plan 目标态（1–6 条），逐条列本次无验收断言 / 未跑的设计性质：

1. **live：`openclaw config patch --file <gen-patch 产物> --dry-run --json` 接受两叶产物**——零 host 断言（OpenClaw 自有 zod strictObject 校验只能在 ECS 跑；offline 只证 JSON 子集形状与叶键集恰二）。归 human 部署轮命令单 §5 第 2 步；判据 = schema 接受且 `tools` 子树未丢 `deny`。
2. **live：真实 `openclaw.json` 实升级态（profile full + allow 五项 + deny []）**——零 host 断言；本包 provenance 已由主线程 live 实读，仓内不重复取证。若部署轮实读与仓内假设不符（例如 deny 非空），按命令单停行回报，不在仓内改配置凑数。
3. **live：真机问「王一诺是谁」答出 2026-09-30 事实**——零 host 断言（行为观测；本包只是把 live 已验证形态固化，不再重复真机轮）。
4. **rollback 零 diff 的核验方式**——无「回滚处理 profile 键」的专项断言，因为该脚本按 backup 文件**整档还原**、语义与键数无关；零 diff 由 `git diff` 事实（§2 事实行）+ landing diff 人读核验承载，行为由 `test_k1_e4_rollback_dry_run_and_restore` 在新目标态 fixture 上真跑覆盖（dry-run 零写 / 一步还原）。
5. **honcho drift 锚换代在 live 不可观测**——插件 4 键已在目标态 → `apply_honcho_plugin.py` 幂等 `SKIP` 先于 drift 判定，五项锚不参与 live 判定；本包如实申报为**离线证书**（`test_k3_e6_declaration_schema_and_p1_drift_anchor` 逐字核 expect 五项 + `test_k3_e6_apply_drift_guard_exit3` 第三条变异「allow 仍四项 → exit 3」机检顺序纪律）。
6. **docs 面零 diff**——由 `git status --porcelain docs/` 事实承载，无测试断言；另申报一处**已知残留陈旧行**：`docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md:27` 仍写「P1 已应用态 tools.allow 四项」（同文件 `:20` 亦为历史 live 实况描述）——append-only 债账本，本包不改写；换代权威 = 本包 honcho 声明 `_note` + K3 验收件头注（详见 `docs-delta.md`「已知残留陈旧行申报」）。
7. **`manifest.yaml` L255 引号形态本身无断言**——更换代的是语义（note 不得再出现「恰四项」、须出现 profile / 五项口径），YAML 双引号是 flow mapping 的语法必需而非语义；解析正确性由 7 件解析 `manifest.yaml` 的验收件（含本包 K1 manifest case）全绿承载，引号写法刻意不钉（排版约定不钉，防脆性）。
8. **`deny` 非空 live 态**——offline 只以 fixture 证 exit 3 零写零备份；live `deny: []` 为当前实况，非空态不会在 live 出现（若出现即 drift 停行）。
9. **无独立单测文件（builder 有意选择）**——本包 `changeManifest` 内测试面**只有两份冻结验收件**（归 acceptance-author，builder 不另建单测文件）；行为级真跑（子进程 apply / gen-patch / rollback 四态）承载分支覆盖，`unit` 闸结构收集即该 10 件圈定集。若 supervisor 认为静态函数面（如 `_alt_hits` / `_validate_changes` / `_json_set`）需要更细的分支级单测，由 planner 走 architect 扩单（不在本包自行补造）。
10. **K1-E4-B 零语义改动的留痕句**——类 docstring 新增一句「本包（261025）零改动本类」属留痕，不产生新断言；行为等价（AGENTS.md 本包零 diff）由该三条用例全绿承载。

**零缺口项：none**——除上述已逐条申报的设计面差集外，`acceptance[]` 三项（K1-E4 / K1-E4-C / K3-E6）的 host 断言全部在冻结件内真跑转绿，无其它未覆盖面。

## 5. live 部署轮命令单（**只列不跑**；Pod 不执行，human 授权轮照敲）

> 前置事实：三件工件由 deploy.sh 按 manifest md5 铺放（本包不改登记结构，只改 note 文案；md5 随内容自然更新）。live 已是升级态（profile full + allow 五项），本轮**只验 schema 接受性，零写零重启**。

```bash
# ① 生成 patch（host 已验形状；产物展平叶键恰两个）
python /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/apply_agent_tools.py \
  --gen-patch /tmp/kidpal-tools.patch.json
#    判据：rc 0；产物 stdlib json 可解析；展平叶键恰
#          {agents.entries.kidpal.tools.profile: "full",
#           agents.entries.kidpal.tools.allow: [五项]}
#    停行：rc != 0 或叶键集不符 → 停，回报

# ② dry-run schema 校验（OpenClaw 自有写入口校验；零写）
openclaw config patch --file /tmp/kidpal-tools.patch.json --dry-run --json
#    判据：回执 schema = true（接受两叶产物），且核对 tools 子树未丢 deny
#          （dry-run 回执 / 随后 `openclaw config get agents.entries.kidpal.tools` 只读实读）
#    停行：schema 非 true / 非零退出 → 不改配置，回报

# ③ 顺序纪律（本轮不写，仅为后续任何重跑定序）
#    P1 tools applier 先（tools.profile=full + tools.allow 五项落库），
#    honcho plugin applier 后（其 drift_guard 现状锚 = 五项）。
#    四项机器上先跑 honcho applier 必 exit 3（零写）——这是顺序红线的机械承载，
#    不得把 exit 3 当缺陷修掉声明锚。
```

## 6. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 写入面 / 哨兵语义对称性 | `_json_set` 对「父键在场但非对象」的中间节点静默替换为 `{}`（fail-open），与 `_json_get` 的 `_MALFORMED` fail-closed 语义不对称。当前 apply 路径不可达：`drift_guard` 三条叶路径（deny / profile / allow）已覆盖 `tools` 子树，畸形 `tools` 必先 exit 3 | changed-code hunk：`apply_agent_tools.py:80-89`（`_json_set`）对照同文件 `:65-77`（`_json_get` 的 `_MALFORMED` 哨兵 docstring 明写「不得与键缺席混同、须 fail-closed」）；judgement heuristic（当前无外部可达路径，非规则违反） | 不改（现状不可达、改动面最小）；若未来声明新增未被 drift_guard 覆盖的叶路径，须同批补对应叶谓词，或让 `_json_set` 遇非 dict 中间节点 fail-closed |
| FYI | judgement-call | 声明 path 面 | `_validate_changes` 只校验 `{path, value}` 形状，不把 path 限定在 `agents.entries.kidpal.tools.{profile,allow}` 枚举内 | changed-code hunk：`apply_agent_tools.py:186-203`；judgement heuristic（声明是仓内受审工件，风险封闭在代码评审 diff 面） | 不改；声明任何改动走仓内评审 |
| FYI | judgement-call | patch 形态与 drift | `--gen-patch` 按冻结契约不读 config，`drift_guard` 在 patch 路径不参与判定；升级合法性由 OpenClaw `config patch --dry-run` schema 校验承担 | changed-code hunk：`apply_agent_tools.py:241-252`；plan T7 / 声明 `_note` 已申报；judgement heuristic | 命令单第 ①/② 步以产物形状 + dry-run + 只读实读补现状核对；不改代码 |

无 BLOCKING、无 SHOULD、无 hard-violation。结构闸（verify.mjs 10 闸）的 PASS/FAIL 不复述为 finding；gate 全绿仍完整执行五轴语义 review（§7）。

## 7. Five-Axis Review

1. **Correctness**：写入面从单键扩为两叶键，直接对应「profile 先行」根因——`tools.profile="full"` 覆盖全局 `coding`，`allow` 仍恰五项（加名 ≠ 可达的教训被 K1-E4-C 机械钉死）。drift_guard 三叶 AND 把四态接受集（fresh / 旧四项 / P0 中间态 / live 升级态）与四类拒态（deny 非空 / allow 项数不符 / profile 非 full / 父键畸形）逐项分开；`_MALFORMED` 哨兵消除了旧 `_json_get` 把「父键非对象」误当「键缺席」按 unset 放行的静默覆盖风险。幂等预检与 drift 联合是本次唯一定向收紧（`profile:full + allow 五项 + deny 非空` 不再裸 SKIP），与姊妹 honcho 脚本「SKIP 先于 drift」的有意差异已双向留痕（脚本头注 + 验收件头注）。honcho 声明锚四→五项同批落地，新机按 manifest 顺序（P1 先）不再被旧锚拦死；四项机器上仍 exit 3，顺序纪律保留。
2. **Test quality**：全部断言直读仓内制品 + tempfile fixture，子进程真跑 apply（首次应用 / 幂等二跑 / 旧四项升级 / P0 中间态升级 / live 升级态 SKIP / deny 非空 exit 3 / 时间戳备份 / 只改两叶与 gateway.token、skills、其它 agent、`deny`、未知同级键零触碰）、`--gen-patch`（两叶键集、字节相等零写、config 缺失仍可生成）与 `rollback`（dry-run 零写 / 一步还原）。负向断言齐（`honcho_search_messages` 不进 allow、`deny` 不进写入面、profile 非 full 即拒、父键畸形即拒、旧单键 `change` 退役负锚、manifest「恰四项」负锚）。K3 侧第三条变异（allow 仍四项 → exit 3）把跨包顺序纪律钉进机械判。mutation 豁免在本包成立：声明 / 配置脚本 / 验收锚换代，冻结断言已把形状与语义逐项钉死（`mutationSkipReason` 在册、`mutationEvidence.perAcceptance` 空）。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零 diff（`git status docs/` 空）；两函数 spec 面零触碰；`AGENTS.md` 本包零 diff（K1-E4-B 三条继续绿作锚兼容旁证）；跨端消息、帧类型、字段与序列化形态零变化（crossEnd exempt）。声明工件是部署配置（无 secret），不是接口类型定义；旧单键 `change` 顶层键退役有负锚防回流。
4. **Operability/security**：工具面未净放宽——`profile=full` 只解除 profile 级过滤，`allow` 仍恰五项且 `honcho_search_messages` 明确不放行（原始消息面含家长代述 / 流程内容）；零新增出网（检索走本机 loopback Honcho `127.0.0.1:18100`）；drift 面 fail-closed 加固（deny 非空 / 项数不符 / profile 非 full / 父键畸形一律 exit 3 零写零备份，不静默覆盖畸形配置）；零数据迁移 / 零库写 / 零密钥入仓；live 写入口纪律不变（只经 `openclaw config patch`，直写形态保留离线核对与回退路径）；输出 UTF-8 兜底避免 Windows 捕获干扰。
5. **Maintainability/simplicity**：改动局部（apply 脚本 +94/−44，四段函数职责单一；声明与 manifest 是文案换代；两份冻结件扩行主要是显式换代留痕与新增 case，属冻结件信息密度而非实现复杂度）；无新依赖、无新抽象层、无跨模块引入；`build_patch_tree` / `apply_changes` 复用 `_json_set` 与 `_deep_copy`，没有为「以后可能的形态」预造配置项。无 `minimal:` 标记新增（§8）。

## 8. Simplification Debt（`minimal:` 台账）

grep `minimal:` 在本次全部改动文件（两份声明、apply / rollback、manifest、两份冻结验收件）**零命中**——本包未新增简化债务，无待登记条目。**no-trigger。**

（承接说明：`apply_agent_tools.py` 的直写形态注释自注「保留为离线核对与既有回退路径」，属既有形态选择，不是本包新造简化债。）

## 9. Residual Risk

- **`decisionCoverage` 筛选**（`landsIn === 'dropped' && reason.startsWith('risk-accepted')`）：**none**——本包 `taskpackage.json` 无 `decisionCoverage` 字段；human 2026-10-25 指示与 architect 1g 预评结论全部落进 `exitCriteria` / `notes`，无 risk-accepted dropped 条目。
- **Known limitation**：① live 面三项（dry-run 接受两叶产物 / 实升级态复核 / 真机问答）零 host 断言，只能部署轮首验（§4 第 1–3 条）；② honcho drift 锚换代在 live 不可观测（幂等 SKIP 先于 drift），为离线证书（§4 第 5 条）；③ rollback 零 diff 由 git 事实 + 整档还原机制承载，无 profile 专项断言（§4 第 4 条）；④ docs 面残留一处 append-only 账本陈旧字面（`TECH-261023:27`，及 `:20` 历史实况），本包不改写、换代权威在声明 `_note` 与 K3 头注（§4 第 6 条）；⑤ `_json_set` 父键畸形 fail-open 与 `_json_get` 不对称，当前不可达（finding 1）；⑥ patch 形态不评估 drift，合法性靠 OpenClaw schema（finding 3）；⑦ 直写形态遇 JSON5 注释会 `json.loads` exit 2 零写（fail-closed），本批不在 live 实跑。
- **已知有意选择（供 supervisor 复核）**：builder 未新增独立单测文件——`changeManifest` 内测试面只有两份冻结验收件（归 acceptance-author），行为级真跑承载分支覆盖（§4 第 9 条）。若 supervisor 认为静态函数面需更细分支级单测，由 planner 走 architect 扩单。
- **Follow-up recommended**：`docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md` 的「四项」字面（`:20` / `:27`）属 append-only 账本历史记录，若后续批次给该条目补换代注记，按账本自身纪律处理；本包按 human 口径不改写。
- **Human decision needed**：部署轮授权与执行时机（本包只列命令单、不跑）；「账本陈旧字面保持原样」是否接受（本包按 append-only 纪律默认接受并留痕）。
- **停等位**：`awaiting-human:live-deploy`——live 三项归 human 部署轮，命令单见 §5；Pod 不自行上 ECS。

## 10. 集成执行回报（Merge-Chain Execution 节）

未触发——本包无 merge-driver 冲突 abort 派单，未执行 rebase / 冲突消解；集成树复跑清单 N/A（本包范围圈定终验已按 §2 跑绿）。

## 11. UI design-review 证据

N/A——本包是云端配置声明 + apply/drift 脚本 + 验收锚换代，无 UIUX 面；仓内不存 screenshot / video / HTML 证据。

## 12. 架构 / 依赖卫生

- **零新增第三方依赖**：apply / rollback 纯 stdlib（json / argparse / pathlib / datetime）；验收件仅用 PyYAML（既有 manifest 断言同一解析器）。
- **无跨层调用 / 无仓内 importer**：`apply_agent_tools.py` 与 `rollback_agent_tools.py` 是独立 CLI，仓内唯一引用者是冻结验收件（按路径以子进程调用），无 Python import 面。graph 复核（`detect_changes`，project `C-AI-KidPal-pod-m2`，base `main`，merge_base `469ca84c`）：changed_files 15、seed_symbols 66、**impacted_total = 4**，四条全部落在被改的两份验收件自身（`server/acceptance` 模块计 4：`test_k1_e4_apply_timestamped_backup` / `K3OpenclawPatchAcceptance._orchestrate` / `test_k3_e6_apply_safe_orchestration_call_order` / `test_k3_e6_apply_timestamped_backup`），production 代码零调用方——爆炸半径自包含，与 test-scope 清单的 10 件圈定一致。changed_files 15 = dev/pod-m2 相对 main 的既有已提交债务/审查 commit（dreaming 污染诊断件、TECH/INDEX 账本、current/ 投影）+ 本包工作树七件 + 本包目录；per-Pod 专属图索引正常圈定、无降级。
- **无循环依赖**：单文件脚本 + 验收件，无环。
- **无绕过 contracts 的本地类型**：零契约面触碰；声明工件是无 secret 的部署配置，不是接口类型定义。
- **`adrRequired=false` 成立**：技术权威 = ECS live 实证（主线程亲核）+ architect 1g 预评档，本包是既有 live 验证形态的仓内固化与锚换代，无新架构裁决需要归档 ADR。

## 13. 制品与 commit 状态

- closeout 制品：`docs-delta.md`（零 diff 声明 + 已知残留陈旧行申报）+ 本 packet + `test-scope-final-r2.txt`（圈定清单）+ `verdict.json`（final round 2 fact 逐字晋升 = `verifier-evidence/rounds/r2.json` 副本，supervisor-compatible shape）+ `verification.json`（advisory 投影：finalScopedRun r2 + priorRounds r1）——均落 `delivery/taskpackages/261025-kidpal-tools-profile-fix/`。
- 本包按 planner 派单边界**不改** `delivery/workstreams/device-memory-overhaul/progress.md`、`delivery/milestones/`、`delivery/taskpackages/INDEX.md`——`deliveryImpact=milestone-affecting` 的 workstream 收口节 / 里程碑记账 / INDEX upsert 归 planner 主线程（本包任务边界 = `server/deploy/**` + 本包目录）；收口 commit 后 integrator 跑了一次默认形态 `closeout-check.mjs`（不传 `--update-index`）作为核验，索引写入不在本包内。
- commit：`dev/pod-m2` 分支（实现面七件 + 两份冻结件 + `delivery/taskpackages/current/` 投影 + 本包全部制品），**不 push**；`.codebase-memory/` 两件不入 commit；final 相位真编译重生成的 `firmware/sdkconfig` 副作用已还原（不入 commit）。
- 后续门：`supervisor-verdict-gate.mjs`（planner 跑）→ `pre-merge-escalation-gate` → `merge-driver --pod`（planner 主线程）；live 部署轮独立于合并门，归 human 授权（命令单 §5）。
