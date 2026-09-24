# Docs Delta · 260924-perf-k1-roundtrip-latency

> integrator 收拢制品（2026-09-24）。本件回答「本包文档面义务是否已全清」，逐面核对与事实锚如下。TaskPackage `docsSyncRequired=true`，`docRefs = [docs/requirements.md#NFR-PERF-1, docs/requirements.md#NFR-PERF-2, docs/ADR.md]`。

## 结论

- **docsSync 义务已履行，落点 = `docs/ADR.md` 新增一条 `ADR-006`（P1 取「降 thinking 档」而非「换模型」的决策、新增部署工件、型号名零漂移声明）**。`node .claude/scripts/docs-sync-check.mjs` 实测 **exit 0**（判据：`docRefs` 每个文档至少提及一个验收 id；`docs/ADR.md` 正文已出现 `PERF-A1`~`PERF-A4`）。
- **本包实况 = 未换模型**（human 五项裁定 `B1` 裁 B 案 = 只改 `agents.defaults.thinkingDefault` 为 `off`，模型 primary 恒 `deepseek/deepseek-v4-flash`）——故 plan.md 阶段划分 4 里「模型切换成为生产常态 → `docs/architecture.md` §2 依赖表 + 决策 19 出网记账型号名同步」的**前提未成立**：**型号未变 → 无 architecture.md 型号名漂移，无需同步 architecture.md**。此项如实写明即算履行 docsSync 义务（对偶 plan.md「回滚路径 → 无漂移、如实记录」口径）。
- **`docs/requirements.md` 零改**：`NFR-PERF-1`（思考响应不冷场）与 `NFR-PERF-2`（查字端到端 5 秒档）为本包的两条需求锚、既有冻结需求面；本包只**消费**不改，需求面零漂移。
- **`docs/architecture.md` 零改**：型号名不带入 → 依赖表（§2）与决策 19 的型号名无需改。
- **`docs/contracts/**` 零触碰**：本包不动引擎 `.config.yaml` LLM 段（base_url `/v1`、model_name `openclaw/default`、流式禁 max_tokens），契约 §四语义不变；`contract-lint` 闸绿。铁律 E2 逐项复核见 `review-packet.md`。

## 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（`contractRefs` 唯一条目） | **零触碰**（铁律 E2；只读消费） | 只读 |
| spec 面·需求 | `docs/requirements.md`（docRefs `#NFR-PERF-1`、`#NFR-PERF-2`） | 零触碰（既有冻结需求，本包消费） | 只读 |
| spec 面·架构 | `docs/architecture.md`（§2 依赖表 / 决策 19） | 零触碰（型号未变，无漂移） | 只读 |
| ADR | `docs/ADR.md` | **新增 `ADR-006`**（append-only，不改既有 ADR-001~005） | docs-sync 落点 |
| closeout 制品 | `delivery/changes/dev/260924-perf-k1-roundtrip-latency/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |
| 决策包 | `delivery/taskpackages/260924-perf-k1-roundtrip-latency/morning-decision-package.md` | integrator 扩写成文（A2 破线置首位） | 晨间制品 |

## 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出；`contract-lint` 闸绿（final 相位 exit 0） |
| 需求面零写入 | 同上零输出；`docs-sync` 闸绿（final 相位 exit 0，`docRefs` 三锚全提及验收 id） |
| 架构面零写入 | `git status --porcelain docs/architecture.md` 零输出；ADR-006 内「型号未变 → 无型号名漂移」声明确立无 sync 义务 |
| ADR append-only | `docs/ADR.md` 仅文件尾追加 `ADR-006`，ADR-001~005 逐字未动（`git diff docs/ADR.md` 为纯新增段） |
| 引擎 LLM 段零改 | `provider.config.yaml` 与 overlay 引擎 LLM 段 `must-stay-green` 设计态即绿齿在档；契约 §四不变（见 review-packet E2 复核） |
| v6 prompt 零回改 | `e1_prompt_v6_byte_hash_unchanged` 齿在档：v6 归一 sha256 == `425d293d…cc62`，本包不动 prompt |

## docs-sync 机械判据落实（逐字）

- 判据来源 = `.claude/scripts/docs-sync-check.mjs`：对 `docRefs` 每个文档，`containsAny(content, acceptance)`（整 id 子串）或 `docMentionsAcceptance`（短后缀 `\bA<N>\b`）命中即过。
- `docs/ADR.md` 可能因 `ADR-00N` 编号中的 `N`（1~5）与短后缀 `A1`~`A4` **不碰撞**而原本不命中——故 `ADR-006` 正文**显式写全 id**（`PERF-A1` / `PERF-A2` / `PERF-A3` / `PERF-A4`），不依赖短后缀假绿。
- 实测回执：`docs-sync-check.mjs` stdout `{ "ok": true, "gate": "docs-sync", "docRefs": [...三锚...], "acceptance": ["PERF-A1","PERF-A2","PERF-A3","PERF-A4"] }`，**exit 0**。

## 型号名漂移声明（如实）

- 本包生产态 = v4-flash + `thinkingDefault: off`（B 案，模型未换）；qqbot 与 voice 共用同一 main agent（共 agent 实证），故模型面零接线变动。
- 因此 **architecture.md §2 依赖表与决策 19 的型号名既无新增、也无改指**，docsSync 无需触碰该文件——非「漏同步」而是「无漂移对象」，与 plan.md 阶段划分 4「回滚路径 → 无漂移不 sync，如实记录」同口径。
- 若晨间 human 拍板改走 A 案（换 `deepseek-chat`/`deepseek-flash` 为生产常态），则须**另起** architecture.md 型号名 docsSync 或 ARQ 记账（二选一不许静默漂移）——该项已列 `morning-decision-package.md` 决策点 3。