# Review Packet · 261017-prompt-v11-schedule-recall

> integrator · 2026-10-18 · rigor=lite / riskTier=medium / sourceType=regression · 分支 `dev/pod-m2` · **无 BLOCKING finding，可交 planner 跑 supervisor 机械闸；integrator 不 push、不 merge、不自行判定通过**。本包 UI 面 = 零（无 UIUX 工作，design-review 证据节不适用）。

## Change Size

- **Files changed（实现面 + 文档面 16 件）**：`server/deploy/openclaw-system-prompt.v11.md`（新 +80）、`OPENCLAW_SYSTEM_PROMPT_CARRIER`（+23/-9）、`provider.config.yaml`（+15/-9）、`apply-xiaozhi-overlay.sh`（+28/-3）、`manifest.yaml`（+26/-6）、`kidpal-agent-prompt.txt`（新 +13）、`261017-openclaw-kidpal-skills/kidpal-agent-skills-declaration.json`（新 +21）、`apply_agent_skills.py`（新 +158）、`rollback_agent_skills.py`（新 +71）、`README.md`（+8/-3）；`server/acceptance/test_prompt_v7_round_routing.acceptance.py`（+628 冻结件）、`test_261010_engine_decision21_overlay_persist.acceptance.py`（+256 冻结件扩展）、`test_261017_openclaw_kidpal_skills.acceptance.py`（新 +530 冻结件）；`docs/debt/tech/` 两条目结项 + `INDEX.md`。
- **Net diff**：`git diff main...HEAD -- server docs` = **16 files changed, +1903 / -42**（commit 前一刻；不含 delivery 簿记与本 closeout 制品自身）。全分支计入 delivery 簿记 = 26 files +2426/-436。基准 = `main...HEAD`（merge_base `dd8083d2`）。
- **Scope**：部署面 prompt v11 识别特征扩列 + KidPal 极简模板 + overlay 两键 + kidpal skills 收敛声明工件 + manifest 单一事实源；零固件 diff、零 front diff、零 wire 形状 diff。
- **Split recommendation**：**不拆**（plan 单包 Lite；arch 面已迁出，独立 arch 会话后置）。

## Verification Evidence

- **前置绿**：builder 实现 commit `0574e9bb` 后 scoped verifier round 1 全闸绿（`r1.json`，invocationId `75014a6e-1171-4ffc-9629-80ace68527a0`，`ok:true`）。
- **Final 相位（bearing）**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261017-prompt-v11-schedule-recall/test-scope.txt --change-id 261017-prompt-v11-schedule-recall --round 3 --invocation-id d55fac5f-35ee-481f-9fe0-48e91dfcd804` → `{"ok":true,"phase":"final","round":3,"evidenceRef":"…/verifier-evidence/rounds/r3.json","observedGateExit":0}`；**10 闸全跑全绿**（taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance）。
- **`verifier-evidence-check`**：`--phase final --round 3 --invocation-id d55fac5f-35ee-481f-9fe0-48e91dfcd804` → `{"ok":true,"gate":"verifier-evidence","evidenceRef":"…/rounds/r3.json","factOk":true,"observedGateExit":0,"phase":"final","round":3}`（exit 0）。
- **中间红点（如实申报，非代码红）**：final **round 2**（invocationId `21b6f0c8-4cc6-4218-81fd-3338c4544795`）红于 `docs-sync`，`missingDocs` = 两条 TECH 条目旧顶层路径——收口后 `debt-render.mjs` 按「位置 = 状态纯函数」自动归位 `_archive/2026/`，TaskPackage `docRefs` 仍指旧址。修正 = `docRefs`/`sourceRefs`/`changeManifest` 改指归档现址（引用精度修正，非语义变更；`origin.upstreamRef` 保留 intake 时点事实），round 3 复跑全绿。r2 事实件保留不删（immutable）。
- **Structural gates**：`firmware-build` **真编译 build-pass**（工具链可用、真编译；`xiaozhi.bin 0x3217f0 bytes`、最小 app 分区余量 9%，本包零固件 diff，编译面与前序包同态）；`contract-lint` PASS（唯一 warning = 上游断链 `delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md`，非本包引入）。
- **Acceptance / unit（按 test-scope 圈定真跑）**：scope 内 9 件 pytest；`unit` 与 `acceptance` 两闸均 `executor-run`：**121 passed / 4 deselected / 4 subtests**。**integrator 独立全量交叉核对（非闸内）**：`server/acceptance/` `-m "not live"` = **210 passed / 4 deselected / 33 subtests**（红基线 210 收集 = 186 passed + 24 设计态红，零残留红；相对上游 181 passed 基线 = +29 条 PV 新齿）；`server/tests/` = **91 passed**。
- **Security gate**：`securityReviewRequired=false`、`security` 闸 PASS；复核补充：`openclaw.json` 零入仓（`test_pv_a6_no_openclaw_json_in_repo` 断言）、仓内 `server/deploy/261017-openclaw-kidpal-skills/**` grep 零 `gateway.token`/`Bearer`/`sk-` 字样、apply 只改写 `agents.entries.kidpal.skills` 一个键。
- **Mutation check**：`mutation-check` PASS / `skipped:true`，理由 = prompt 文本 + 配置声明 + 声明工件批无 DB/事件 mutation 取证对象；apply/rollback 幂等性由冻结件在临时 fixture 上构造真实 pre/post 状态并 subprocess 真跑（本身即状态变异）；live 行为归 PV-A8（structural 豁免）。integrator 复核认可该口径（同 261012 / 261015 先例）。
- **Docs sync**：`docs-sync` 闸 PASS（docRefs 六条全部在盘）。本包 docs-sync 实际落盘 = 两条 TECH 结项 + INDEX 重渲染 + README v11 指针核证（builder 已翻、integrator 核证一致、未再改）。`docs/architecture.md` 两处指针明确不在本包（见 `docs-delta.md` §2）。
- **机器件口径**：承重 = `verifier-evidence/rounds/r3.json`（final，`authority: bearing`、`producedBy: verify.mjs`，immutable fact）；r1（scoped）与 r2（final 红，已修正）留档。`current/verdict.json` = verify.mjs 脚本 fs-write 的 non-bearing projection；包目录 `verdict.json` / `verification.json` = advisory-only 投影，**closeout 判断只认 r3 fact**。

## test-scope 清单（final 相位范围圈定）

- **落盘路径**：`delivery/taskpackages/261017-prompt-v11-schedule-recall/test-scope.txt`（`verify.mjs --phase=final --scope-file` 消费，round 3）。**本行即「清单路径记入 results 元数据必含行」的落点**——r3 fact 的 `results[]` schema 固定（无 scopeFile 字段），scope 路径由本 packet + 各闸 stdout 承载。
- **圈定方式 = 正常路径（非降级）**：`detect_changes`（per-Pod 专属图索引 `C-AI-KidPal-pod-m2`，base=main，merge_base `dd8083d2`，inbound depth=2）读到 changed_files 29 / seed_symbols 87 / **impacted_total 0**。零 impact 的成因 = 本包改动面是 prompt 文本 / YAML 清单 / shell 渲染 / JSON 声明工件，无代码符号调用者——`detect_changes` 可用（非「图不可用」），blast radius 由「谁读这些部署面文件」人工映射补全，清单内逐条注明。
- **范围构成**：① `acceptanceTestsFile` 三件（PV 冻结验收）；② `impactedTestsSeed` 映射测试（`test_m3k3_voice_schedule_entry` 四面版本无关守卫、`test_261001_openclaw_model_name_overlay_drift` overlay 冲刷守卫）；③ 共享 seam = 所有读取 prompt / `manifest.yaml` / `apply-xiaozhi-overlay.sh` / `provider.config.yaml` 的冻结件（`test_char_meaning_merge` / `test_m4k3_record_session_live` / `test_perf_k1_roundtrip_latency` / `test_261013_qq_schedule_write_closure`）；④ 安全余量 = 声明契约先例件（perf-k1）。
- **排除申报**：`server/tests/*.py` 无文件引用本次改动面（unit 闸交集空 = 诚实绿跳过，非缩水）；`test_engine_stop_truncation_asr_filter` / `test_kidpal_speak_sync` 只读 `engine-patch/connection.py`（零 diff）；其余 acceptance 文件不读本包改动面。`server/deploy/verify-deploy.sh` 非测试件，其静态可解析性由 PV-A7 齿覆盖。
- **floor**：本包 `acceptanceTestsFile` ∪ blast radius ∪ 共享 seam（单包，无对端冻结件）。

## 本包效果对账（plan「验收要点」host 七项 + live 四项 ↔ PV-A1..A8）

| plan 验收要点 | 承载 | 实测结果 |
|---|---|---|
| ① v11 落盘 + 四面接线同版 + ADR-001 版本无关守卫绿 | PV-A1（落盘/无前言/扩列/仅课表行 diff）+ PV-A3（四面一致/沿革追加/接线版携带扩列）+ m3k3 A1 | 绿（6 case） |
| ② v10 sha256 零回改（历史不可变） | PV-A2 | 绿（LF 归一 canonical blob 口径） |
| ③ v11 课表行含「后天/大后天/周X/星期X/下周/几号/提到课表」扩列锚 | PV-A1 `schedule_row_expanded_recognition` | 绿 |
| ④ 模板含 `<identity>` + `<context>`，不含通用小智残留段 | PV-A4（两 case，含禁 `{{current_time}}`） | 绿 |
| ⑤ overlay 两键在场、兜底语无「小智」、声明面同步、marker 锚覆盖 | PV-A5（D21 扩展六 case） | 绿 |
| ⑥ skills apply 幂等 / drift_guard 生效 / backup / rollback 可跑 | PV-A6（十 case，subprocess 真跑临时 fixture） | 绿 |
| ⑦ manifest 条目齐 + 可静态解析 + README 指针 v11 | PV-A7（三 case） | 绿 |
| ⑧ `verify-deploy.sh` 三时机核验绿（零未对账漂移） | **host 面只承载静态可解析性**（PV-A7）；三时机真跑 = 部署轮 | 部分（host 静态绿，真跑未跑） |
| live-1 问「后天都有什么课程？」→ 审计出现 read 动作、答复与周一行一致 | PV-A8 ①（structural / live-manual） | **未跑，停门等 human** |
| live-2 loopback 探针同口径复验 | PV-A8 ② | **未跑** |
| live-3 `openclaw skills list --agent kidpal` ready = 3 | PV-A8 ③ | **未跑** |
| live-4 人为触发引擎出错兜底，播报不含「小智」 | PV-A8 ④ | **未跑** |

> PV-A8 = human 持板 live-manual 停门（structural，非自动断言）；**本 Pod 零部署、零烧录**，host 面转绿即为本 Pod 终点。

## 未覆盖面申报（逐条一行）

- ① **PV-A8 四项 live-manual 未跑** —— 现状 = 未部署、未真机；去向 = human 持板轮（部署后按 PV-A8 ①②③④ 逐项观测）。
- ② **`.config.yaml` 真再生未跑** —— 现状 = 生成件只在 ECS 部署轮由 `apply-xiaozhi-overlay.sh` 渲染；host 面只断言 heredoc 源与 marker 锚；去向 = human 部署后 `verify-deploy.sh` 三时机核验（plan 风险段「三保险」）。
- ③ **skills 首次真 apply 未跑** —— 现状 = 冻结件在临时 fixture 上 subprocess 真跑幂等/drift/backup/rollback，但未触碰生产 `openclaw.json`；去向 = 部署轮在 ECS 执行 `apply_agent_skills.py`（drift_guard 现状锚不中即 exit 3 拒绝）。
- ④ **模板渲染为等价代理** —— 现状 = 本机无 jinja2（实测 `import jinja2` 失败），引擎 `build_enhanced_prompt`/`_load_base_template` 的真实渲染未在 host 执行；冻结件以文本锚 + `{{base_prompt}}` / 禁 `{{current_time}}` 静态断言代理；去向 = 部署轮与 PV-A8 真机轮观测实际注入 prompt。
- ⑤ **`system_error_response` 文案为 builder 拟定** —— 现状 = plan 只裁「KidPal 口径、禁小智」，具体措辞「哎呀，我这边卡了一下，稍等一下再问我一次吧。」为实现选择；去向 = PV-A8 ④ 只观测「不含小智」，措辞观感归 human（可随时在 overlay + provider.config 同值改）。
- ⑥ **skills 收敛的实际效果（18 bundled/extra 从注入面消失）未观测** —— 现状 = 声明 + 脚本契约绿；去向 = PV-A8 ③ `skills list` ready = 3 + 真机轮观察注入 prompt 体积/误触发。
- ⑦ **`verify-deploy.sh` 三时机真跑未跑** —— 现状 = 只验静态可解析；去向 = 部署轮（部署前拦截 / 部署后自检 / 每日 timer）。
- ⑧ **README「已知版本字面漂移（归 ARQ-260908）」小节仍 v3 时代叙述** —— 现状 = 既有 ARQ 记账，明确标注「本包未修」；去向 = `ARQ-260908` 收口时统一重写（见 `docs-delta.md` §1.2）。
- ⑨ **`docs/architecture.md` §5.5.5 / §5.1 指针仍 v10** —— 现状 = dev-loop 无写权、顺序要求「实物先于指针」；去向 = merge 后独立 `--agent architect` 会话（见 `docs-delta.md` §2）。
- ⑩ **main agent skills 与 QQ 录入链路未验** —— 声明只作用于 `agents.entries.kidpal`，main 不在断言面；去向 = 本包非目标（plan 非目标段）。
- （零缺口项明写）**none** —— 除上述 ①–⑩ 外无其它未覆盖条目。

## UI design-review 文本证据

- **不适用**：本包零 UIUX 工作（部署面 prompt/配置/声明工件，零设备渲染面改动）。viewport / a11y / design-drift / external artifact 四项均无对象；无 residual UI risk。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包 diff 与新件扫描**零 `minimal:` 标记**） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | `server/deploy/261017-openclaw-kidpal-skills/apply_agent_skills.py:126-143` | apply 语义上只改 `agents.entries.kidpal.skills`，但落盘是对整份 `openclaw.json` 做 `json.dumps(..., indent=2)` 重序列化 + 非原子 `write_text`——其余键的排版会变（无注释的 JSON 语义不变）；写入中途中断可留截断文件 | changed-code hunk（写前时间戳备份 + 整文件重写）；heuristic：destructive write without temp+rename；缓解 = ③ 备份先落盘 + rollback 脚本可字节还原 | **accepted-with-rationale（本批不改）**：目标文件是 ECS 运行态配置、仓外、有备份与回滚。升级触发 = `openclaw.json` 出现格式敏感消费者 / 并发写者，或需要保留人工排版时改「读-改-写 + tempfile + os.replace」 |
| NIT | judgement-call | `apply_agent_skills.py:129` / `rollback_agent_skills.py:38-41` | 备份文件 `openclaw.json.bak-kidpal-skills-<ts>` 每次 apply 新增一份、无保留策略；rollback 默认取字典序最新 | changed-code hunk + heuristic：无界备份积累；量级 = 该键极少变更 | **accepted-with-rationale**：单次手工部署触发的 apply，积累可忽略；部署轮如多次重试可人工清理 |
| FYI | judgement-call | `server/deploy/README.md:79-98` | 「已知版本字面漂移（本包未修，归 ARQ-260908）」小节仍以 v3 时代口径叙述 live 探针钉 v1/v2 字面与「被接线真实渲染源已 v3」。非本包引入、非当前指针（指针节 :54-63 已 v11） | `git diff main...HEAD -- server/deploy/README.md` 仅含指针节 hunk；该小节未被本包触碰（历史记账，文内明写「本包未修」） | 归 `ARQ-260908-versioned-guard-version-literal-coupling` 收口时统一重写；本包不改写以免把历史观测改成伪现状 |
| FYI | judgement-call | `server/deploy/apply-xiaozhi-overlay.sh:88` / `provider.config.yaml:52` | `system_error_response` 具体措辞为 builder 拟定（plan 只裁口径与禁「小智」） | changed-code hunk + plan §B 约束粒度；产品语感属 human 可调面 | 无需动作；human 如要改文案，两处同值改即可（PV-A5 的「渲染面 == 声明面同值」齿会守住一致性） |

> 无 BLOCKING finding；结构闸 PASS 不复述为 finding。**本包无未决 BLOCKING ⇒ 可进入 supervisor 机械闸。**

## Five-Axis Review

1. **Correctness**：v11 = v10 全文仅课表识别特征列扩列（`test_pv_a1_diff_confined_to_schedule_row` 以行级 diff 断言钉死「其余段落零偏移」）；扩列覆盖「提到课表/课程/上课… + 今天/明天/后天/大后天/周X/星期X/周几/这周/下周/某月某号 + 课」，纪律句落在同一行内（先读 `kidpal/schedule.md`、读不到诚实说查不到、禁凭印象）；四面接线一致且 ADR-001 版本无关守卫绿；v10 canonical blob sha256 未变（负锚）；模板 `<identity>` 承接 `{{base_prompt}}`、`<context>` 保留 today_date/today_weekday/农历/位置/天气（课表算星期的输入）、无 `{{current_time}}` 字面量陷阱、无通用小智模板残留段；overlay 两键与声明面同值、兜底语无「小智」、marker 锚覆盖；skills 声明恰三项、apply 语义序（幂等 → drift-guard exit 3 → 时间戳备份 → 只改目标键）与 rollback 均在临时 fixture 上真跑。**未执行面**：引擎真实渲染、live 行为（见未覆盖面 ②③④⑤⑥）。
2. **Test quality**：29 条新冻结 case 断言真实文件与真实子进程行为（apply/rollback subprocess、临时目录 pre/post 状态、备份文件命名、二次 apply 零语义变更、drift_guard 拒绝 exit 3、`skills list` 面不伪造）；负锚（v10 sha256、禁 `{{current_time}}`、禁残留段、兜底语禁「小智」）齐；seam 守卫（m3k3 / 261001 / V7 / PN / RF / D21 旧齿）零新增红。诚实边界 = live 四项目前无自动断言（PV-A8 structural 明示）。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 均零 diff（实测 `git diff main...HEAD -- docs/contracts docs/architecture.md docs/requirements.md` 为空）；零 wire 形状改动（display E1 / schedule view / B1 不变）；零新增依赖；无跨层调用；`openclaw.json` 零入仓。唯一跨面承接 = `docs/architecture.md` 两处指针，已在 `docs-delta.md` §2 显式声明顺序与承接会话形态。
4. **Operability/security**：secret 面零泄漏（apply 只读改写非 secret 字段、仓内 grep 零 token 字样）；写前时间戳备份 + drift_guard 防 clobber + rollback 可字节还原；`.config.yaml` 生成件三保险（host 静态齿 + 部署后 `verify-deploy.sh` + 改动前 `.bak`）；manifest 落位与 `verify-deploy.sh` 静态解析对齐；失败模式 fail-loud（模板缺源 WARN、apply 非零退出码分层 2/3）。残余 = 整文件重序列化与非原子写（NIT，已记账）。
5. **Maintainability/simplicity**：五件全部收敛进 manifest 单一事实源；prompt 沿革只留 carrier 一处（README/provider/overlay 指针化）；skills 工件复用 perf-k1 声明 + apply + rollback 形态，无第二套机制；脚本纯 stdlib（离线可跑、无 jq 依赖）；注释逐处锚 plan/PV id 与冻结契约节。零 `minimal:` 标记、无过早抽象。

## Residual Risk

- **Known limitation**：① PV-A8 真机四项与 ECS 部署轮（`.config.yaml` 真再生、skills 首次真 apply、`verify-deploy.sh` 三时机）均未跑——本 Pod 零部署零烧录；② 模板渲染为静态等价代理（本机无 jinja2，引擎 `build_enhanced_prompt` 未执行）；③ `detect_changes` 对本包返回 impacted_total 0（非代码面），scope 由共享 seam 人工映射补全，属「图可用、符号面无边」而非降级；④ `system_error_response` 文案为 builder 拟定，未与 human 定稿；⑤ README ARQ-260908 小节 v3 时代叙述仍在（既有记账）。
- **Follow-up recommended**：merge 后独立 `--agent architect` 会话翻 `docs/architecture.md` §5.5.5 / §5.1 指针（禁指针先于实物）；human 部署轮按 PV-A8 执行四项观测并跑 `verify-deploy.sh`；`ARQ-260908` 收口时重写 README 字面漂移小节。
- **Human decision needed**：PV-A8 真机轮放行与验收结论；兜底语文案是否采用 builder 拟定措辞（可随时同值改两处）。
