# Review Packet · 260924-perf-k1-roundtrip-latency

> integrator 收拢制品（2026-09-24），供 supervisor 机械合并门复核、高风险/critical 时供人类 review。本包 = server 单端运维/配置型变更（ECS openclaw.json 模型/thinking 声明面 + 声明工件 + 幂等 apply/回滚 + 探针工具），firmware 零触碰、引擎 `.config.yaml` LLM 段零改动。闭环口径：`PERF-A1` 探针门 + `PERF-A2` 质量守卫 + `PERF-A3` 晨间真机轮 + `PERF-A4` 回滚与五项核验 / `PERF-E1`~`E3` exitCriteria。

## Change Size

- **Files changed**：Pod 分支自 `aa5ec4b`（池树 rebase 基线）起 8 个 commit；`git diff --stat aa5ec4b..HEAD` = **22 files, +3122 / −4**（另 integrator 本次新增 `docs/ADR.md` 段 + `delivery/changes/dev/<id>/` 两制品 + 机器证据件）。实现/验收/证据面构成：
  - **实现面（新增 5 件，零既有实现改动）** `server/deploy/perf-k1/`：`openclaw-model-declaration.json`（B 案声明工件）、`apply_model_declaration.py`（幂等 apply + drift-guard STOP-on-mismatch exit 3 + 时间戳备份）、`rollback_model_declaration.py`（一步还原 + dry-run）、`probe_gate.py`（纯判定器，两层门 + 质量判定）、`probe_batch.py`（live 探针批执行器，v6 注入 + hash 断言）。
  - **冻结验收面** `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`（27 齿 = 23 offline + 4 live，1076 行）+ `PERF-K1-ACCEPTANCE-NOTES.md`（设计说明/逐齿映射/mutation 原始输出）。
  - **证据面** `delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/latency-baseline-260924.md`（§7~§12 追加）+ `evidence/probe-batches/*.json`（8 批结构化批次）+ `evidence/serial-baseline-pre-p1-260924.log`。
  - **TaskPackage 面** `taskpackage.json` + `delivery/taskpackages/current/taskpackage.json`（docRefs 改指 `docs/ADR.md`、`mutationEvidence` 四条）。
  - **文档面（docs-sync）** `docs/ADR.md` 新增 `ADR-006`（append-only）。
- **Net diff**：`+3122 / −4`（22 文件）；server 面**全为新增（`git diff --name-status` 全 `A`，零 M/D）**——见「Detect Changes / 影响面核对」节。
- **Scope**：单一子系统（server）+ 单变量（`agents.defaults.thinkingDefault: off`），firmware / 契约 / 引擎 LLM 段零触碰。
- **Split recommendation**：无需拆分。变更是「声明工件 + 幂等 apply/回滚 + 探针工具」一条内聚链，切开会破坏「声明面 == 消费面」的接线不变式。

## Verification Evidence

- **Structural gates**：`contract-lint` **exit 0**（契约 `docs/contracts/interface-contract.md` 未漂移）；`firmware-build` **exit 0**（真编译，`xiaozhi.bin` 0x30e840B / app 分区余 11%——本包 firmware 零触碰，编译为回归护栏）。
- **Acceptance/unit（全量）**：
  - `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/` → **80 passed / 4 deselected（exit 0）**。
  - `python -m pytest -q --import-mode=importlib server/acceptance/`（含 live）→ **80 passed / 4 skipped（exit 0）**（4 live 齿由 conftest 机械 skip，ARQ-260901-g3；live 面走证据读形态，见「未覆盖面申报」）。
  - 红基线 → 绿：`18 failed / 62 passed / 4 deselected`（设计态红）→ `80 passed / 4 deselected`，18 条设计态红全转绿，5 条 must-stay-green 与既有 57 条保持绿。
- **Security gate**：`security` 闸未在 scoped 相位执行（scoped 遇红即停在 `docs-sync`）；final 相位由 `verify.mjs --phase=final` 全覆盖（见下）。人工安全轴 review 见「Five-Axis Review」第 4 轴。
- **Mutation check**：`mutation-check` **exit 0**（4 条 functional acceptance `PERF-A1`~`A4` 各有 `mutationEvidence`；`structural:[]`、`suiteGates:[]`——**零豁免**，无需「为何豁免」枚举）。mutation 实测打在配置接线（M1 删声明文件 / M2 thinking 改 `high` / M3 删 allow / M4 reasoning 翻真 / M5 两轴混声明 / M6 改错 model id → 对应齿红），原始输出见 `PERF-K1-ACCEPTANCE-NOTES.md` §6。
- **Docs sync**：`docs-sync-check` **exit 0**（`docRefs` 三锚全提及验收 id；落点 = `docs/ADR.md` ADR-006，详见 `docs-delta.md`）。
- **Bearing final fact**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id 260924-perf-k1-roundtrip-latency --round 3 --invocation-id <uuid>` 产 `verifier-evidence/rounds/r3.json`；`verifier-evidence-check.mjs --phase final --round 3` 复核（本节末「Final 相位全闸」实跑数字）。

## Acceptance 逐锚结果（PERF-A1~A4）

判定器 = 冻结件 `server/deploy/perf-k1/probe_gate.py::evaluate_batch`（offline 纯逻辑齿 + live 证据读齿同源）。integrator 对**全部 8 个批次 JSON** 重跑判定器复现（命令 = 载入 `probe_gate` 逐批 `evaluate_batch`），读数如下（各题 N=3 中位；a1 保底 6/9/14s、目标 4/6/10s）：

| 批次 segment | 闲聊 | 课表 | 天气 | 保底 | 目标 | verdict | display 出现率 | schema 合法率 | quality_pass |
|---|---|---|---|---|---|---|---|---|---|
| `night-baseline-remeasure`（重建件） | 13.42 | 11.43 | 9.54 | ✗ | ✗ | FAIL | 0.667 | 0.667 | False |
| `post-p0-restart`（重建件） | 6.64 | 11.21 | 11.68 | ✗ | ✗ | FAIL | 0.333 | 0.333 | False |
| `pre-p1-post-p0`（实跑对照锚） | 4.492 | 10.707 | 8.773 | ✗ | ✗ | FAIL | 0.444 | 0.444 | False |
| `p1-b-inline-thinkoff-trial1`（内联 `/think:off` 试验） | 4.401 | 10.49 | 11.207 | ✗ | ✗ | FAIL | 0.333 | 0.222 | False |
| **`post-p1-b-final`（权威）** | **4.135** | **5.369** | **8.054** | **✓** | ✗ | **BOTTOM_ONLY** | 0.222 | 0.222 | False |
| `post-p1-b-batch2` | 4.784 | 5.992 | 9.553 | ✓ | ✗ | BOTTOM_ONLY | 0.444 | 0.222 | False |
| `post-p1-b-norestart` | 4.258 | 5.274 | 5.971 | ✓ | ✗ | BOTTOM_ONLY | 0.333 | 0.333 | False |
| `post-p1-b-low`（`thinking=low` 对照） | 5.4 | 11.868 | 9.951 | ✗ | ✗ | FAIL | 0.333 | 0.333 | False |

- **PERF-A1（探针门）**：**B（off）三批保底门全绿**（`post-p1-b-final`/`batch2`/`norestart` 全 `BOTTOM_ONLY`：闲聊 4.14~4.78 ≤ 6、课表 5.27~5.99 ≤ 9、天气 5.97~9.55 ≤ 14）。**目标门**：课表（≤6s，5.27~5.99 ✓）与天气（≤10s，5.97~9.55 ✓）达标，**闲聊（≤4s）差 0.1~0.8s 未达**——plan 原文「目标门未达 = k2 立项依据、不算当晚失败」，非本 Pod 判红项。天气题附加条件（须含真实检索数据）逐批 `weather_retrieval_ok=True` ✓。**A1 保底门结论 = 绿**（P1 主案 B 达标）。判定器另对 `low` 档 / 内联 `/think:off` 对照给出 FAIL，坐实「配置档 `off` 才是真杠杆、`low` 无益、内联无效」。
- **PERF-A2（质量守卫）**：**P1 前后均不达标**——`display_rate` / `schema_rate` 全程 < 1.0（pre-P1 0.444 → post-P1 0.222~0.444），`quality_pass=False` 覆盖**全部 8 批**（含基线）。根因 = 基线固有的 v6 遵循度 / 「David」人设渗漏（`tools.profile="coding"` + workspace bootstrap 注入），**非本 Pod 引入**——见下「A2 破线如实载明」节。**此为真实验收缺口，见 Findings #1。**
- **PERF-A3（晨间真机轮）**：P1 后正式轮待晨间 human 持板（本 Pod 不 auto-run）；同日对照锚 = §10 pre-P1 真机基线轮（闲聊 17.22s / 课表 13.64s / 天气 15.05s，超 A3 保底门 1.7~2.2 倍）——该轮不替代 A3（门判定对象是 P1 后配置）。见「未覆盖面申报」。
- **PERF-A4（回滚与核验）**：回滚脚本 dry-run + 一步还原行为齿在档（offline 行为级，临时参考实现反证非空过）；P0 五项核验 live 证据齐（§7.3 重启后 + §12 改配置后复跑全绿：cron `kidpal-reminder-plan` 在册 / qqbot READY 重连 / 18789 锁 loopback / char-stroke `KidPal C1` marker 在位 / 引擎→18789 可达）。**绿**。

## A2 破线如实载明（最高优先晨间决策点）

> 本节按 planner 派单要求「不得修复、必须如实载明」落笔；integrator **不自裁**处置，合并门判定归 supervisor / human。

- **现状数字**：`PERF-A2` 要求 display 块出现率与 schema 合法率**均 100%**。实测（判定器全量判定，非抽检）pre-P1 `display_rate=0.444`、post-P1 `0.222~0.444`，`quality_pass=False` 贯穿全 8 批；课表备注读回在部分批次亦 `note_readback_ok=False`（`pre-p1`/`batch2`/`norestart`/`low`）。**A2 门未达标**。
- **基线对照**：带 v6 的**基线探针同样复现**（`night-baseline-remeasure` display 0.667 → 仍 <1）——判据对新旧配置同等严苛，非 P1 引入。
- **根因**：基线固有的 v6 system prompt 遵循度不稳 / workspace「David」人设渗漏（泄漏形态实测含「你好呀,David」称呼、工具前导 `I'll check the schedule file`、内部注释 `<!-- project: path:/root/.openclaw/workspace -->`、v6 明禁的 `<details>` 块）；来源 = `tools.profile="coding"` + `DREAMS.md` 42117B 等 bootstrap 注入（§12 k2 输入清单）。**根因归 k2**，不在本 Pod 范围。
- **planner 裁定（§12.1-1，逐字要旨）**：plan.md `A2` 回滚触发线原文为「**切 A 后**任一批 display 合法率 <100% → 当晚回滚」，本 Pod 走 **B 案**（human `B2` 裁「A2 照 plan.md 原文执行」，故触发线为 **A 案作用域**，B 案路径不触发）；且 A2 未达标为基线固有，回滚既不能修复质量、又退回更慢状态（v4-flash + high，课表 10.7s）。→ **暂留 B 已应用，A2 破线作最高优先晨间决策点上浮 human**。
- **一条命令可回滚**（若不接受现状）：`python server/deploy/perf-k1/rollback_model_declaration.py --config /root/.openclaw/openclaw.json`（默认取最新备份 `openclaw.json.bak-perf-k1-20260924T032223`）。回滚后退回 v4-flash + `thinking=high`（课表 10.7s），**质量面不回改善**（A2 基线同样红）。

## E1 / E2 / E3 exitCriteria 铁律逐项复核

- **PERF-E1（offline 机械面齿全绿）**：`taskpackage-required` / `acceptance-coverage` / `mutation-check` / `docs-sync` 四闸 exit 0；声明工件在位且字段正确；apply 幂等 + drift-guard（锚点不中 → exit 3）+ 时间戳备份行为齿绿；探针脚本 v6 hash 断言逻辑齿绿（`--check-prompt` sha256 == `425d293d…cc62`）；回滚 dry-run 可跑；must-stay-green（引擎 LLM 段声明面与 v6 文件零回改）绿。**时延数字不做 CI 齿**（timing flaky），门 = live 证据 + 冻结阈值判定——**遵**。
- **PERF-E2（铁律）**：`docs/contracts/**` 零改（contract-lint 绿 + `git status --porcelain docs/contracts/` 零输出）；引擎 `.config.yaml` LLM 段零改（base_url `/v1`、model_name `openclaw/default`、流式禁 max_tokens——`e1_provider_llm_segment_unchanged` / `e1_overlay_llm_segment_unchanged` 齿绿）；prompt v6 零回改（`e1_prompt_v6_byte_hash_unchanged` 齿绿）；不动 qqbot 配置 / 不升 OpenClaw / 不加 agent（**B 案零结构隔离**）；ECS 运维逐步留痕（evidence append-only）、凭证不入档（probe_batch 提示 token 经 `--token-file` 且不落档）、动 openclaw.json 前必备份（写前时间戳备份 + `openclaw backup create` 双锚）、清理类动作先 backup（archive 前 backup 在先）；B 案 timebox ≤30min（实用约 16 分钟）；contingency 预写。**遵**。
- **PERF-E3（三段归因证据齐 + 记账 + 晨间包）**：三段归因（当晚基线重测 / P0 后 / P1 后——各 N=3 中位 + 主机快照 + session 计数，分开记不混算）落 §7.2/§7.4/§12；探针批次后探针新建 session 已 archive（backup 在先，§12 步⑦/⑧）；记账义务 = 「模型未换 → 无 architecture.md 型号名漂移」如实登记（`docs-delta.md` + ADR-006）；`morning-decision-package.md` 成文（晨间 6 项 + A2 破线置首位）。**遵**（记：步⑦仪表化未执行，见 Findings #3）。

## Detect Changes / 影响面核对

- **detect_changes**（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 池树专属索引，`since=aa5ec4b`）：changed_files=**28**、seed_symbols=**74**、**impacted_total=0**——改动符号的传递调用方集合为空。**与 change surface 自洽**：server 面改动**全为新增文件**（`git diff --name-status aa5ec4b..HEAD -- server` 全 `A`，零 M/D），无既有符号被改，故无入向影响面。**非** `N/A (worktree)`——池树索引可见本次 diff，核对有效。
- **关键共享符号 trace_path 抽查**：`probe_gate._payload_valid` 惰性 import 的真门 `server.gateway.display_schema.validate_display`——inbound 调用方 **8 个**（`front_app._relay_display_blocks_to_device` / `adaptation_dryrun` / `build_display_tool_calls` / `command`、`AlertDispatcher.tick`、`reminders.due_entries` / `read_pending` / `validate_alert_entry`），**全部在 `server/gateway/**`、本 Pod 零改动**。probe_gate 只读消费该符号（不改、不接线生产路径），既有消费方行为不变——**跨层/回归风险为零**。
- 结论：本包对既有实现面**零改动**，blast radius = 0；新增件间自洽（声明面 `openclaw-model-declaration.json` == 消费面 `apply_model_declaration.py` 默认 decl 路径；探针渲染面 == 声明面，ADR-001 不变式）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记） | — | — | no-trigger |

- `grep -rn "minimal:" server/deploy/perf-k1/` 零命中——本包无 `minimal:` 简化债务标记。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| **BLOCKING** | hard-violation | Acceptance（PERF-A2） | `PERF-A2` 质量守卫（display 块出现率 + schema 合法率 100%）**未达标**：全 8 批 `quality_pass=False`，post-P1 `display_rate=0.222~0.444`。 | taskpackage `acceptance[1]`（`PERF-A2` 要求 100%）+ 判定器实跑读数（见「Acceptance 逐锚结果」表）；根因 = 基线固有 v6 遵循度 / 「David」人设渗漏（`tools.profile="coding"` + bootstrap 注入，`docs/requirements.md` 无关此轴） | **交人类**：planner 已据 plan「**切 A 后**破线才回滚」A 案作用域 + 基线固有性裁「暂留 B、上浮 human」（§12.1-1）。**本 Pod 不修**（根因归 k2）；晨间拍板 = 接受现状 / 回滚 B（一条命令）/ 升 k2 首要。integrator 不自裁合并门处置 |
| SHOULD | judgement-call | Test/verification surface | A 案 `allow` 路径形状不一致：冻结齿 fixture 用**顶层** `modelPolicy.allow`，生产 `openclaw.json` 实为 `agents.defaults.modelPolicy.allow`。 | changed-code hunk `server/deploy/perf-k1/apply_model_declaration.py:70-82`（apply 对两路径**双写**兼容）+ 已登记缺口 `evidence/latency-baseline-260924.md` §12.1-2；heuristic = verification-surface ≠ production-surface 的潜在假绿 | 已如实登记，暂不重冻（重冻触发 verifier 全量重跑）。交 **k2 / A-fallback 处置时收口**（planner 已登记；本 Pod B 案不触该键、当前齿不受影响） |
| SHOULD | judgement-call | Evidence hygiene | `night-baseline-remeasure.json` / `post-p0-restart.json` 的 `_provenance.reconstructed=true`——总耗时逐字取自 §7.2/§7.4 记载，**reply 字段为重建成占位文本**（原始回复正文未留存）。 | 两 JSON 的 `_provenance.reconstructed` 字段 + `evidence/…md` §12「重建件标注」 | **勿当原始采样读**（仅作三段归因对照锚）；已在 §12 明标。无额外动作 |
| OPTIONAL | judgement-call | Operability | 步⑦「仅测量面仪表化」**未执行**（触发条件均未成立：A1 保底已绿 / 网关日志不暴露 preflight 字段 / slow SQLite 属探针自污染且随 archive 消解）。 | `evidence/…md` §12 步⑦ + §12.1-4（触发判据逐条）；`probe_batch.py:104` `_count_pressure_warnings` 显示网关日志面未暴露 preflight 分段 | 如实记「未做 + 原因」，未自裁达标。晨间若仍需 preflight 三段时戳 → **另批回派**（网关当前不暴露该字段） |
| NIT | judgement-call | Maintainability | `probe_batch.py:120-132` `_session_count` 以正则解析 `openclaw sessions list` 文本（`Sessions listed: N of M`），输出格式漂移即静默退化为 −1。 | changed-code hunk `probe_batch.py:120-132`；heuristic = 文本解析脆性（K3 先例读 YAML 亦走 regex，同族） | 可另开后续；当前 live 执行器由人工执行、退化可见，不阻塞 |

## Five-Axis Review

1. **Correctness（是否满足声明 `Sx-Ay`，边界条件）**：`PERF-A1` 保底门达标（B 案三批全绿）、`PERF-A4` 回滚 + 五项核验齐、`E1`~`E3` 铁律逐项遵——见上逐锚节。边界条件：drift-guard `expect="unset"` 一次性锚（B 案落 `off` 后即消耗，再改档需新锚，**符合设计**非缺陷）；apply 幂等优先（现状已等于目标 → SKIP 不重备份）；N<3 → `n_ok=False` 判 FAIL（门 = N≥3 中位）。`PERF-A2` 未达标为唯一正确性缺口（Findings #1）。
2. **Test quality（是否断言真实行为）**：offline 齿断言**真实结果**（声明 schema 校验器真判违规、apply 真跑幂等/drift-guard exit 3/时间戳备份内容、回滚真还原、判定器喂 fixture 真断言 verdict）——非「没报错就算过」；mutation 打在**配置接线**（M1~M6 逐条转红 + GREEN 对照 0 violation + apply/rollback 由临时参考实现反证非空过）。`live` 齿取 evidence-read 形态（读真实批次 JSON 跑判定器），不 auto-run。**强**。
3. **Contract/boundary safety（是否漂移契约/spec）**：`docs/contracts/**` 零触碰（contract-lint 绿 + git 零输出）；引擎 LLM 段与 v6 prompt must-stay-green 齿绿；`docs/architecture.md` 型号名零漂移（型号未变，`docs-delta.md` 明载）；ADR-006 append-only 不改既有条目。**无漂移**。
4. **Operability/security（ErrorEnvelope/correlationId/权限/输入/secret/日志）**：apply/rollback 退出码契约清晰（0 成功 /2 环境错 /3 drift-guard STOP）；probe_batch 凭证经 `--token-file`、明示「凭证不入档」，请求头 Bearer 仅内存；`subprocess` 调用为**固定字符串**（`free -m` / `journalctl -u openclaw-gateway …`）无注入面；写 ECS openclaw.json 前双备份（时间戳 backup + `openclaw backup create`）。回滚单命令可达。**无安全残余需人类判断**（见 Residual Risk）。
5. **Maintainability/simplicity（局部/易懂/无过早抽象/无重复实现）**：5 件实现相互解耦（纯判定器 `probe_gate` 无网络无 fastapi、执行器 `probe_batch` 独立）；`probe_gate._payload_valid` **复用**真门 `validate_display` 而非重造校验（single source）；`probe_batch` 按路径 import 同目录 `probe_gate`（不依赖 sys.path）。Findings #2（allow 双路径）与 #5（文本解析）为可维护性 minor。

## 未覆盖面申报（对照 `acceptance[]` + 契约条目 + roadmap §3 效果段）

- **PERF-A3 真机轮**：性质 = human 持板 live 轮，非 auto-run；现状 = pre-P1 同日基线对照轮已跑（§10）、P1 后正式轮未跑（门判定对象是 P1 后配置，基线轮不替代）；去向 = **晨间 human 持板**（`morning-decision-package.md` 决策点 1）。
- **live 探针批真机 HTTP（A1 live 面）**：性质 = 需 ECS loopback 127.0.0.1:18789，非 CI；现状 = builder 已跑 8 批结构化 JSON，live 齿 evidence-read 消费；去向 = **已完成**（本 Pod live 相位）。
- **P0 五项核验 live 齿**：性质 = live evidence-read；现状 = §7.3（重启后）+ §12（改配置后复跑）已记录全绿；去向 = **已完成**。
- **PERF-A2 质量门 100%**：性质 = live 全量判定；现状 = **未达标（基线固有）**；去向 = **上浮 human / 根因归 k2**（Findings #1）。
- **步⑦ 仅测量面仪表化**：性质 = 条件步，触发条件未成立；现状 = 未做 + 原因已记；去向 = **明示接受 / 晨间若需另批回派**（Findings #4）。
- **CI 时延齿**：性质 = timing 齿 flaky，**刻意不入 CI**；现状 = 门 = live 证据 + 冻结阈值判定（评审 §3）；去向 = **明示接受不覆盖**。
- **QQ 录入回归 + 提醒 semantic.json 形状冒烟**：性质 = 晨间 human 回归（共 agent 波及面）；现状 = 未跑；去向 = **晨间**（决策点 2）。
- **跨子系统 e2e / correlationId 链路**：性质 = 结构性跨端；现状 = 本包 server 单端、firmware 零触碰、display 消费面（front V27）行为不变，`crossEnd.exempt` 明载无跨包行为对偶面；去向 = **不适用（none）**。

> 零缺口项明写：跨子系统 e2e / correlationId = **none**（本包 server-only、无跨端对偶面）。

## demo 对账结论（锚 3）

- **本包 demo 形态** = **live 探针批（8 批结构化 JSON + 判定器读数）+ 真机基线对照轮（§10 串口三源对齐）**，**非 UI 截图**（本包无 UIUX 改动，`designReviewRequired` 未声明，故无 viewport/a11y/design-drift 证据面）。
- **对账**：offline 机械面 RED→GREEN 全程留痕（18 设计态红 → 全绿）+ pytest 全量零新增红 ✓；live 三段归因探针批（当晚基线重测 / P0 后 / P1 后，各 N≥3 中位 + 主机快照 + session 计数）✓；`A2` 全量判定记录 ✓（**结论 = 未达标**，如实）；P0 五项核验记录 ✓；archive 前后 session 对照 ✓；晨间 A3 真机轮 + QQ 回归待跑（决策点 1/2）。
- **形态差异如实记**：`night-baseline-remeasure` / `post-p0-restart` 为**重建件**（§12「重建件标注」），非原始采样——对账时按对照锚读、不作新采样证据。

## Residual Risk

- **Known limitation**：① `PERF-A2` 质量门未达标（基线固有，根因归 k2）——见 Findings #1，**须 human 拍板**；② A 案 `allow` 路径形状不一致（潜在假绿，A 案未走故当前无实害）——Findings #2；③ 步⑦仪表化未执行——Findings #4；④ 重建件非原始采样——Findings #3。
- **Follow-up recommended**：k2 立项（携本包 P1 数据）= ①David 人设渗漏治理（`tools.profile="coding"` 与 bootstrap 注入）②静态上下文瘦身（~25k tokens/请求：v6 22.5KB + DREAMS.md 42KB 等）③session store retention（511 会话/93MB）④课表 3 轮工具循环收敛（prompt v7 槽位预约先行）⑤ttfc==total 流式结构调研；P3 = ECS 4GB 升级（human 裁暂缓，B3）；OpenClaw 2.0.4 升级窗口（过 runbook §0 漂移闸）。
- **Human decision needed**：①**A2 破线拍板**（接受现状 / 回滚 B / 升 k2 首要）——最高优先；②模型定案（B 批准为生产常态，或改走 A 并另起 architecture.md 型号名 docsSync / ARQ）；③P3 4GB 与过渡项取舍（B3 裁暂缓）；④OpenClaw 2.0.4 升级窗口（B3/B5）。
- **decisionCoverage dropped/risk-accepted**：本 TaskPackage `decisionCoverage` 字段缺失，**无** `landsIn === 'dropped' && reason.startsWith('risk-accepted')` 条目——**none**。

## 回滚方式

- **一步回滚**（P1 声明面）：`python server/deploy/perf-k1/rollback_model_declaration.py --config /root/.openclaw/openclaw.json`（默认取最新 `.bak-perf-k1-*`，当前 = `openclaw.json.bak-perf-k1-20260924T032223`）→ 还原 `agents.defaults.thinkingDefault` 至 unset（退回运行时默认 high）。
- **SQLite 级回滚锚**：`openclaw backup create` 快照 `/root/2026-09-24T11-14-24.686+08-00-openclaw-backup.tar.gz`（清理类动作专属锚）。
- **P0 前回滚**（如回退重启阈值态）：`/root/.openclaw/openclaw.json.bak-260924-preP0`（7817B）。
- **改配置热加载**：改 thinking 档由网关 `[reload] config hot reload applied` 生效，**无需重启 gateway**（故 P1 段无重启混淆因子）。
- **本仓改动回滚**：本 Pod 分支未 push、未 merge；弃用 = 丢弃分支 commit 即可（新增件全在 `server/deploy/perf-k1/**` 与证据/制品面，零既有实现改）。

## Final 相位全闸（`verify.mjs --phase=final`）

实跑（`round=3`，`invocationId=1b596a7b-97aa-4a7c-952b-403a66cb6702`）：

- `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id 260924-perf-k1-roundtrip-latency --round 3 --invocation-id 1b596a7b-97aa-4a7c-952b-403a66cb6702` → **exit 0**，`{"ok":true,"phase":"final","round":3,"observedGateExit":0}`，产 `verifier-evidence/rounds/r3.json`。
- `node .claude/scripts/verifier-evidence-check.mjs --change-id 260924-perf-k1-roundtrip-latency --phase final --round 3 --invocation-id 1b596a7b-97aa-4a7c-952b-403a66cb6702` → **exit 0**，`{"ok":true,"gate":"verifier-evidence","factOk":true,"observedGateExit":0}`。

**final 相位 9 闸全绿**（r3.json，`observedGateExit=0`）：

| 闸 | kind | exit | ms | 关键读数 |
|---|---|---|---|---|
| `taskpackage-required` | governance | 0 | 235 | ok |
| `firmware-build` | structural | 0 | 28389 | 真编译 build-pass（本包 firmware 零触碰，回归护栏） |
| `contract-lint` | structural | 0 | 182 | 契约未漂移 |
| `acceptance-coverage` | functional | 0 | 191 | `PERF-A1~A4` 全 functional，`structural:[]` / `suiteGates:[]` |
| `mutation-check` | functional | 0 | 223 | 4 条 functional acceptance 各有 mutationEvidence |
| `docs-sync` | governance | 0 | 203 | docRefs 三锚全提及验收 id |
| `security` | security | 0 | 165 | `securityReviewRequired:false`（未声明安全轴） |
| `unit` | functional | 0 | 5853 | **134 passed / 4 deselected** |
| `acceptance` | functional | 0 | 5240 | **80 passed / 4 deselected**（firmware 验收件 14 + server 5；on-target 由实机/台架另触发） |

> 承重判定：validated final fact（r3.json，`ok:true`，checker `factOk:true`）——本包 final 证据有效，可进入 closeout。