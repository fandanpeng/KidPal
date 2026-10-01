# Docs Delta · 261001-openclaw-model-name-overlay-drift

> integrator 收拢制品（2026-10-01）。本件回答「本包文档面义务是否已全清」，逐面核对与事实锚如下。TaskPackage `docsSyncRequired=true`，`docRefs = [docs/ADR.md#ADR-006]`。

## 结论

- **docsSync 义务已履行，落点 = `docs/ADR.md` 新增一条 `ADR-008`**（引擎 `.config.yaml` LLM 段 `model_name` 定为 `openclaw/kidpal`；**amend ADR-006** 的「model_name 零改 / 对引擎不可见」支撑句——双 agent 下对引擎**可见**〔须点名 agent〕，「对契约 §四不可见」仍成立；ADR-006 核心决策〔降 thinking 档、模型 primary 不变、出网记账型号名零变动〕不受影响）。append-only：ADR-001~007 逐字未动（`git diff docs/ADR.md` 为纯新增 8 行）。`docs-sync` 闸实测 **exit 0**。
- **`docs/contracts/**` 零触碰**（铁律 MNF-E2）：契约 §四对 `model_name` / `openclaw` 值**零命中**（只钉 base_url/loopback + 禁 max_tokens），本包从未把 model_name 引入契约面；`contract-lint` 闸绿。
- **`docs/requirements.md` / `docs/architecture.md` 零改**：两文件对 `model_name` / `openclaw` 值零命中；本包不触需求面与架构面本体（型号名未变、依赖表无漂移对象）。
- **ADR 号订正留痕**：plan/启动卡/ARQ 原写「新增 ADR-007」，但 ADR-007 已被 `260924-m4-k1-story-cloud-link` 占用（`docs/ADR.md`「星星记录本数据面收口」）→ 起包 1f 核验改判 **ADR-008**（plan §8.1）。

## 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（`contractRefs` 唯一条目） | **零触碰**（铁律 MNF-E2；只读消费） | 只读 |
| spec 面·需求 | `docs/requirements.md` | 零触碰（`model_name` 零命中，无漂移对象） | 只读 |
| spec 面·架构 | `docs/architecture.md` | 零触碰（无型号名漂移对象） | 只读 |
| ADR | `docs/ADR.md` | **新增 `ADR-008`（amend ADR-006）**（append-only，不改既有 ADR-001~007） | docs-sync 落点 |
| 历史记录（不改写） | `server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md` | 零改（承载包 260924 的冻结设计/红基线/mutation 记录，改写会伪造证据） | carve-out |
| 历史记录（不改写） | `docs/ADR.md` ADR-006 正文 / `docs/crq/**` / `.claude/skills/kidpal-ecs-gateway/SKILL.md` | 零改（amend 由 ADR-008 承载；crq 台账 / skill 历史解释保持原貌） | carve-out |
| closeout 制品 | `delivery/changes/dev/261001-openclaw-model-name-overlay-drift/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |

## 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出；`contract-lint` 闸绿（final 相位 exit 0）；契约 §四对 `model_name`/`openclaw` 值零命中（grep 实测） |
| 需求面零写入 | `git status --porcelain docs/requirements.md` 零输出；`model_name`/`openclaw` 值零命中 |
| 架构面零写入 | `git status --porcelain docs/architecture.md` 零输出；无型号名漂移对象（型号未变，ADR-006 出网记账声明链完整） |
| ADR append-only | `docs/ADR.md` 仅文件尾追加 `ADR-008`（+8 行），ADR-001~007 逐字未动（`git diff` 为纯新增段） |
| 退役面/历史记录零改 | `PERF-K1-ACCEPTANCE-NOTES.md:63-64` 陈旧逐齿映射、`shim.py`/`kidpal-shim.service`/`acceptance/archive/*` 旧值字面均保持原貌（`plan §5` carve-out 两类） |

## docs-sync 机械判据落实（逐字）

- 判据来源 = `.claude/scripts/docs-sync-check.mjs`：对 `docRefs` 每个文档，`containsAny(content, acceptance)`（整 id 子串）或 `docMentionsAcceptance`（短后缀 `\bA<N>\b`）命中即过。
- **防短后缀假绿**：`docs/ADR.md` 内本已有 `PERF-A1`~`A4`，其短后缀 `A1`/`A2`/`A3` 会被 `\bA<N>\b` 命中——故 ADR-008 正文**显式写全 id** `MNF-A1` / `MNF-A2`（不依赖短后缀假绿），本包自身验收 id 在文档面有真承载体。
- 实测回执：`docs-sync` 闸 stdout `{ "ok": true, "gate": "docs-sync", "docRefs": ["docs/ADR.md#ADR-006"], "acceptance": ["MNF-A1","MNF-A2","MNF-A3"], "docsNoChangeReason": "..." }`，**exit 0**（final r3 实跑）。

## 型号名 / 依赖面漂移声明（如实）

- 本包改的是**部署配置面**（引擎 `.config.yaml` LLM 段 `model_name` 值 = `openclaw/default` → `openclaw/kidpal`），**非模型切换**：模型 primary 恒 `deepseek/deepseek-v4-flash`（ADR-006 核心决策不受影响）。故 `docs/architecture.md` §2 依赖表与决策 19 出网记账的**型号名无漂移对象**——非「漏同步」而是「无漂移」。
- 出网记账语义说明：`openclaw/kidpal` 是 OpenClaw 网关侧的 **agent 定位名**（用于引擎→网关 loopback 请求的模型路由），非出网调用上游模型的型号名；引擎仍以同一 primary 模型出网，记账型号名零变动。
