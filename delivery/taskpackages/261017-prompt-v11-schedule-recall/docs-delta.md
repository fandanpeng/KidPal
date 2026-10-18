# docs-delta · 261017-prompt-v11-schedule-recall

> integrator · 2026-10-18 · 分支 `dev/pod-m2`。本文件是 closeout 的 docs-sync 记账与承接声明：哪些文档在本包改了、哪些**明确不在本包**、以及未改的理由与去向。

## 1. 本包已落盘的文档改动

### 1.1 两条 TECH 债务结项（`docs/debt/tech/**`）

| 条目 | 改动 | 依据 |
|---|---|---|
| `TECH-261017-schedule-answer-skip-read-hallucination`（MAJOR） | frontmatter `status: deferred → closed`、`closed: 2026-10-18`、`closedBy: 261017-prompt-v11-schedule-recall`；正文末尾 append 一行 `consumed-by: 261017-prompt-v11-schedule-recall`（含 v11 落盘、四面接线、v10 零回改、host 验收绿与 PV-A8 真机门状态） | 本包 v11 识别特征扩列落地 + 四面接线同批翻（PV-A1/A2/A3 host 绿） |
| `TECH-261017-xiaozhi-template-residue-deploy-ssot-gap`（SHOULD） | 同上三字段翻转 + 正文末尾 append `consumed-by:` 行（含模板/两键/skills 声明/manifest 四件事与 live 落地待 human 部署轮的状态） | 本包 PV-A4/A5/A6/A7 host 绿 |

- **归档归位**：按 `debt-render.mjs` 的「位置 = 状态的纯函数」纪律，两条目由渲染器自动搬入 `docs/debt/tech/_archive/2026/`（**人不搬文件**；这是机制正常行为，不是手工移动）。原顶层路径已不存在，故 TaskPackage 的 `sourceRefs` / `docRefs` / `changeManifest` 同步改指归档现址，并补 `docs/debt/tech/_archive/2026/INDEX.md` 制件行；`origin.upstreamRef` 保留 intake 时点事实不动。此修正为引用精度修正、非语义变更（同 `261013-lang-config-thinking-fix` 先例），已在 taskpackage `notes` 留痕。
- **INDEX 刷新**：`node .claude/scripts/debt-render.mjs` 实跑（exit 0）；`docs/debt/tech/INDEX.md` 摘要从「未结 27 / 已结 19」变为「未结 25 / 已结 21」，`docs/debt/tech/_archive/2026/INDEX.md` 同步追加两条薄行。`node .claude/scripts/debt-render.mjs --check` 复跑 exit 0（盘上 == 应渲染）。

### 1.2 `server/deploy/README.md` prompt 指针节

- 现状核对：指针节已由 builder 翻为 **「现状 = v11」** 并持有 `openclaw-system-prompt.v11.md` 指针 + 一句 v11 增量说明；写法维持「指针 + 不复述沿革」（沿革唯一全量副本在 `OPENCLAW_SYSTEM_PROMPT_CARRIER`）。integrator 核证**与当前接线版一致**（`OPENCLAW_SYSTEM_PROMPT_CARRIER` / `provider.config.yaml` / `apply-xiaozhi-overlay.sh` / `manifest.yaml` 四面均指 v11），**未再修改**——无新增 delta。
- 观察记账（不在本包改）：同文件「已知版本字面漂移（本包未修，归 ARQ-260908）」小节仍以 v3 时代口径叙述 live 探针钉旧版字面。该节属 `ARQ-260908-versioned-guard-version-literal-coupling` 的既有记账（明确标注「本包未修 / 改本节或 bump v4 前须知道」），是历史漂移说明而非当前指针；本包不改写它以免把历史观测改写成伪现状。去向 = 该 ARQ 收口时统一重写。

## 2. 明确不在本包：`docs/architecture.md` §5.5.5「现状 = v10」与 §5.1 :197 指针

- **状态**：未改，且**本包任何角色都无写权**——scope-map 中 `integrator.allow` 不含 `docs/architecture.md`、`planner.deny` 含 `docs/**`，dev-loop 无角色可写该文件。architect 2026-10-18 重裁已把它迁出 dev 包（plan「架构面不在本包」段 + spine facePlan）。
- **顺序理由（禁「指针先于实物」）**：arch 面必须等 dev 面 v11 实物与四面接线**已落地**后才翻指针。反序会在落地窗口内制造「文档说 v11、仓内无 v11」或「文档说 v11、接线仍 v10」的假声明。本包现在已完成实物落地（r3 final 全绿），满足时间条件。
- **承接会话形态**：merge 后由主线程另起 `--agent architect` 会话（`/arch-change` 轻路径，文档面措辞级批）翻两处：§5.5.5「现状 = v10」→ v11 并追加 v11 沿革一句、§5.1 :197「system prompt 版本化 v10」→ v11。ADR-001 版本无关守卫从三面结构化指向解析当前版、不读 `docs/architecture.md`，落地窗口内 arch 短暂落后**不会假红任何既有守卫**。
- **不漂移的担保**：`docs-delta` 显式声明 + spine `anchors` 记录 arch 面待办；在 arch 会话完成前，`docs/architecture.md` 与实现存在**已知且被记账**的指针滞后，不得据 README/CARRIER 已 v11 反推 arch 已同步。

## 3. 本包零改动面（显式声明）

| 面 | 状态 | 理由 |
|---|---|---|
| `docs/requirements.md` | 零 diff | 行为语义权威 = S-F1.3 AC1「课表问题永不编造」与 human 2026-10-17 裁「只要提到课表就去读」；本包是识别特征扩列的实现细化，需求文本无需变 |
| `docs/contracts/**` | 零 diff | 零 wire 形状改动（display E1 / schedule view / 课表 MD B1 全不变）；契约面 `NO_CONTRACT_CHANGE` |
| `docs/debt/tech/INDEX.md` 以外 debt 域 | 零 diff | 仅两条 TECH 结项（gov / requirement 两域未触） |
| `openclaw.json` | 零入仓 | 含 `gateway.token` secret；本包只入无 secret 的 skills 声明工件 + 定点 apply/rollback 脚本 |
| 固件面 / front 面 | 零 diff | 部署面 prompt/配置/声明工件批，设备与网关除「课表必读」与「兜底语改口径」两条运行时行为外无代码变化 |

## 4. 未随本包完成、需承接的项（不表述为已完成）

1. **PV-A8 live-manual 真机轮**——human 持板四项观测（问「后天都有什么课程」见 `read` 动作且答周一课表 / loopback 复验 / `openclaw skills list --agent kidpal` ready = 3 / 兜底语无「小智」）。本 Pod 零部署、零烧录，host 面转绿后**停门等 human**。
2. **ECS 部署轮**——`apply-xiaozhi-overlay.sh` 真再生 `.config.yaml`（两键落盘）、超集模板拷入 `data/`、`verify-deploy.sh` 三时机核验、skills `apply_agent_skills.py` 首次真 apply。本包只交付仓内制品与 host 静态齿；真再生与首次 apply 归 human 部署后部署轮（plan 风险段「三保险」口径）。
3. **`docs/architecture.md` 两处指针**——见 §2，merge 后独立 arch 会话承接。
