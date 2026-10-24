# Review Packet · 261024-oneshot-user-uniformity

> integrator 合并前审查制品 · 2026-10-24 · rigor=lite · 分支 dev/pod-m2 · 模板 = `.claude/references/code-review-gate.md` §Review Packet

## 1. 变更摘要

一次性回合携 user 统一（human 2026-10-24 方向权威，R7/OQ-4 隔离口径退役）：

- **provider 单源扩展**（`server/gateway/provider.py`）：`openclaw_stream_request(messages, user=None)`——真值判据（与 engine-patch openai.py 两腿逐字同源）才加 `"user"` 键；禁 max_tokens 硬约束零触碰；模块头注 Frozen offline contract 行同步。
- **front 调用链**（`server/gateway/front/front_app.py`）：`_openclaw_chat_once` 增 `user` 透传；新增 `_leg_device_id` / `_legs_device_id`（device-id 头原值透传，不 strip 不归一；isinstance str + strip 双保险防御式）；`_organize_and_archive` / `_organize_and_archive_offloop` 增 `device_id` 形参；`_story_finish_session` 身份派生放归档内容守卫后、to_thread 前（AST 结构锚零偏移）；`_story_guide_round` LLM 调用前取一次腿快照仅用于身份（播报腿集判据零偏移）；条件传参 `kwargs={"user": device_id} if device_id else {}`——device-id 缺席 = 诚实降级（payload 缺 user 键 + 调用缝留痕），八处既有桩零改。
- **指令挪 request 级 system**：整理 user 消息 = material 纯文、引导 user 消息 = spoken 纯文；两 prompt 制品登记进 `server/deploy/manifest.yaml`（`restart: none` / `verify: md5`）；空制品回退 = user 消息回填现行完整包装形态 + warning（`story_organize_prompt_fallback` / `story_guide_prompt_fallback`）。
- **声明工件换代**（`honcho-plugin-declaration.json`）：noisePatterns 键从 changes[] 与顶层 plugins 块同步删除（5键→4键）；`_note` 换代重写，保留 `agent:kidpal:openai-user:` 字面、零 `agent:kidpal:` 其它形态；noisePatterns 退役无机械验证面如实申报（不设 `expect:unset` 假牙）；apply/rollback 零代码改动。
- **manifest 口径同步**：`:297`/`:303` 两处 5 键 note 换代为 4 键 + 退役实证注记；新增两条 prompt 条目（first-run 语义见 docs-delta §二）。
- **验收锚显式换代**（`test_261022_memory_isolation.acceptance.py`）：`test_k3_e2_front_oneshot_payload_no_user` → `test_k3_e2_front_oneshot_payload_carries_device_user`（恰 {model,messages,stream,user} 且 user=<device_id> + 降级路径 + 空制品回退路径断言）；`test_k3_e6_patch_generator_product_five_keys` → `..._four_keys`；`K3_DECL_CHANGES` 5→4；头注 B1 挡板措辞退役申报 + 换代事由（human 2026-10-24 裁 R7/OQ-4 隔离口径退役）写进断言 docstring 与头注；旧名映射留痕；live 证据 one_shot 判定口径反转 + 每条证据增 `openclaw_session_key` 前缀断言字段。

## 2. 范围圈定与 final 终验（Step 4 两步走）

- **范围圈定**：detect_changes（project=`C-AI-KidPal-pod-m2`，per-Pod 专属图条目，base=main，inbound，depth=2）——changed 11 files（含实现面 5 件 + taskpackages 簿记 + 2 workstream/debt 文档 + 本包目录），seed_symbols 28，**impacted_total 26**（server/acceptance 16 / server/gateway 6 / server/tests 4），落点 = 四姊妹验收件（test_m4k1_story_cloud_link / test_record_cancel_lifecycle / test_record_dialogue_guide / test_record_front_polish）+ test_story_fetch + front_app 内部调用者（_story_auto_finish / _story_timeout_sweep / ws_gateway / pump 双泵）+ test_story_cancel_lifecycle_units 三族——与 TaskPackage 预登记 impactedTests 及 LOOP-X 申报面一致，**零意外扩散**。图可用性 = 正常圈定（per-Pod 专属索引可用，非降级）。
- **test-scope 清单落盘**：`delivery/taskpackages/261024-oneshot-user-uniformity/test-scope-final.txt`（12 条：acceptanceTestsFile ∪ impactedTests 种子 ∪ blast radius 映射 ∪ 共享 seam（test_archive_guard_units AST 锚）∪ LOOP-X 安全余量三姊妹）。范围 = 影响面圈定清单，未默认全量、未缩到只剩本包档。
- **final 终验**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --round 2 --invocation-id 8e0e9ad7-7e96-43eb-a59f-dbf774d47869 --scope-file <test-scope-final.txt>` → **绿**（`ok:true` / `observedGateExit 0`）；evidence = `delivery/taskpackages/261024-oneshot-user-uniformity/verifier-evidence/rounds/r2.json`（bearing 机器件，10 闸全绿：taskpackage-required / change-manifest(pre-cutoff 存量放行) / firmware-build(真编译 build-pass) / contract-lint / acceptance-coverage / mutation-check(降级声明) / docs-sync / security / unit / acceptance）。checker `verifier-evidence-check.mjs` PASS（`factOk:true`）。
- **申报偏差（advisory）**：r2.json 的 results 元数据本版 verify.mjs 未落 scopeFile 字段——test-scope 清单落盘路径改记入本 packet 与 docs-delta 留根，圈定面生效可见（acceptance 收集 128 vs 全量约 340、unit 143）。
- **全量回归** = 人工触发位，本收口不跑（scope 圈定面已绿 + builder 转绿时已跑全量）。

## 3. 验收面与数字轨迹

**换代断言三件**（本包 A1-A3 承载）：
1. `test_k3_e2_front_oneshot_payload_carries_device_user`——spy 捕获 `openclaw_stream_request` payload 恰 {model,messages,stream,user} 且 user=<device_id>；降级路径（device-id 缺席 → 缺 user 键）；空制品回退路径（user 消息回填现行包装形态）。
2. 声明 4 键逐字锚 + patch 生成器恰 4 键 + `_note` session 家族锚（`agent:kidpal:openai-user:` 字面在位、变体负锚）。
3. live 段口径反转 + `openclaw_session_key` 前缀断言字段（live 标记件，`-m "not live"` 可收集、零收集错误）。

**must-stay-green 面**（LOOP-X，零新增红）：四姊妹验收件桩缝条件传参下无感；test_story_cancel_lifecycle_units 四族；test_archive_guard_units AST 结构锚；test_261022_honcho_memory / test_261022_honcho_backfill（K4_GATE_CONFIG_NEEDLES 负锚天然不红）/ test_261023_agents_md_static_semantics_alignment 零触碰。

**红基线→绿数字轨迹**：
| 相位 | acceptance（-m "not live"） | unit |
|---|---|---|
| 红基线（planner 起包前实跑） | 333 passed / 0 failed / 7 deselected / 33 subtests | — |
| acceptance-author 冻结后（设计态红） | 330 passed / **3 failed**（全为本件换代断言红：缺 user 键主锚 / 声明 5 键 / patch 5 键，零 error）/ 7 deselected / 33 subtests；排除本件 330/0 零新增红 | — |
| builder 转绿 | **333 passed / 0 failed / 7 deselected / 33 subtests** | **115 passed** |
| integrator final round 2（scope 圈定面） | 128 passed / 0 failed / 3 deselected / 3 subtests | 143 passed / 3 deselected / 22 subtests（unit 闸 full 计入口径与 builder 单文件申报不同，闸绿为承重事实） |

## 4. 未覆盖面申报（对照 acceptance[] + 契约条目逐条一行）

- **live one_shot 证据采集**（A3 live 段：live 采集 one_shot 证据 + `openclaw_session_key` 前缀核验）——本次无验收断言/未跑（离线桩面不可证）；现状 = 断言 schema 已换代并在位；去向 = 部署轮采集。
- **真机 3–5 例抽检**（指令挪 system 输出质量：整理首行标题/分段/忠实度/不混入既往篇目；引导 ≤20 字/不代编）——本次未跑（离线不可证）；去向 = 真机轮（docs-delta §三 #1）。
- **noisePatterns live 退役实证**（A2 声明侧退役已锚，live 侧无机械验证面）——本次未跑（patch additive 语义证不了键删除，如实申报不设假牙）；去向 = 部署轮实读 openclaw.json（docs-delta §二 2）。
- **firmware-build 闸 lite scoped defer**——final round 2 实跑结果：**真编译 build-pass 绿**（xiaozhi.bin 0x3242f0 / app 分区余 9%，见 r2.json firmware-build 闸 stdout），defer 未发生、闸已实过。
- **跨端 wire 面**——crossEnd exempt 申报成立（user 是云端运行态请求字段，非跨端 wire；固件零 diff；WS device-id 头既有）。
- **mutation 取证**——mutationSkipReason 降级声明前置（薄装配 + 行为断言直接覆盖，ROI 不成立，批 B 同款），final round 2 mutation-check 闸以降级声明 PASS。

## 5. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | server/gateway/provider.py | payload 字典沿用文件末尾无换行符（`No newline at end of file`），diff 显示为整 dict 重写、非本批引入 | changed-code hunk（diff 尾标 `\ No newline at end of file`）；heuristic: 卫生类小疵，冻结面语义零影响 | 不阻塞合并；后续触碰该文件时顺手补换行 |
| NIT | judgement-call | closeout 制品 | final verifier fact（r2.json）results 元数据未含 test-scope 清单路径字段（本版 verify.mjs 不落 scopeFile 键） | changed-artifact hunk（r2.json 全文核对）；heuristic: Step 4「路径一行记入」的本版工具能力边界，非 integrator 漏记 | 路径已记入本 packet §2 与 docs-delta 留根；verify.mjs 增强需求交 planner 记账 |
| — | — | — | 无 hard-violation finding（Basis 涉契约/spec 的 hard-violation 前缀起引规则本轮无适用条目） | — | — |

结构闸：final round 2 十闸全绿（§2），gate 全绿仍完成下方五轴语义 review。

## 6. 五轴语义 review

1. **契约符合性**：零契约动作成立——front 是同一 loopback 客户端沿用契约 §四已定案的 user 语义（`agent:kidpal:openai-user:<device_id>` session 键族与 engine 主链路同源）；禁 max_tokens 约束逐字保（`openclaw_stream_request` diff 中零 max_tokens 出现）；`docs/contracts/**` 零触碰。PASS。
2. **安全**：security 闸绿（securityReviewRequired=false）；声明零 secret（openclaw.json 不入仓注记保留）；device-id 原值透传不 strip 不归一 = 与 engine 同源、防分叉第二 session 键族，isinstance str + strip 双保险防 Mock truthy 炸 json.dumps；新代码无 eval/无外发路径、出网仍沿契约 §1.2 路径②零新增。PASS。
3. **边界与错误处理**：诚实降级链完整——headers 缺属性 / getter 非 callable / 取头异常 / 非 str / 空白五路全归 None → 条件传参空 kwargs → payload 缺 user 键 + 调用缝 log.info 留痕；空制品回退回填现行包装形态 + warning（最坏退回旧 live 行为，不是无指令）；归档内容守卫时序零偏移（身份派生置于守卫后）。PASS。
4. **可运维性**：manifest 两 note 说谎面已同步换代（部署单一事实源不再写 5 键）；两条 prompt 条目登记 + first-run REFUSE 语义注记在位（偿还 261022 欠账）；warning 日志键（story_organize_prompt_fallback / story_guide_prompt_fallback / oneshot_user=absent）可 greppable 供部署轮排查。PASS。
5. **测试与证据**：换代显式留痕（改名 + docstring 换代事由 + 旧名映射）；红基线→设计态红→绿轨迹完整且 3 条设计态红与换代断言一一对应（不是碰巧红）；LOOP-X 姊妹件零触碰由 blast radius 圈定佐证。PASS。

## 7. 简化债务台账（`minimal:` 标记收成）

- 本批 diff **零新增** `minimal:` 标记。存量一处（front_app.py:1909 固定间隔 sweep，minimal: 回退或调频只动该常量）非本批触碰面，不入台账升级，no-trigger。

## 8. Residual Risk（decisionCoverage 筛选）

- 本 TaskPackage **无 `decisionCoverage` 字段**（lite 单面包，未启该结构）——`landsIn: dropped && reason: risk-accepted` 条目为零，无 human 风险接受项需列。
- 残余风险以 docs-delta §三 残余清单承载（真机抽检 / 多腿单设备口径 / 双回合重复入记忆 / noisePatterns live 实证），各有归属轮，非 blocking。

## 9. docs-sync 与架构卫生

- docsSyncRequired=false 成立（零 canonical spec/contracts 改动）；spec 过时面六条 + 部署注记 + 残余清单由 `docs-delta.md` 显式申报，逐条挂「在途 /requirement-change & arch 批」，integrator 对 arch/requirements 本体零自改（无写权）。
- 架构/依赖卫生：零新增依赖；零跨层调用（front→provider 既有单源，loopback 既有）；无循环依赖；无绕过 contracts 的本地类型；重复实现面核对——user 真值判据与 engine-patch openai.py 同源注释在位、非复制实现。

## 10. closeout 与 commit

- closeout-check（只读，未传 --update-index）：见交付回执实测行。
- commit：dev/pod-m2 分支，实现面 5 件 + taskpackages 簿记 + 本包收口制品；不 push。
- 集成执行回报：无（merge-chain 冲突消解未触发，无跨包冲突）。
- UI design-review：N/A（本包零 UI 面）。
- 跨面 spine：非在册 spine 包（human-ad-hoc 单面包，workstream=""），退出报告含包收口实态供主线程按需回写。

## 11. 等待 supervisor 裁断

本 packet 不自行判定合并通过；合并门 = supervisor 复核 + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后 planner 主线程 merge-driver 本地合并。回滚方式：dev/pod-m2 分支 commit 粒度回滚（`git revert <hash>` 或 reset 至本 commit 前），实现面 5 文件均可独立还原，无 schema/数据迁移面。
