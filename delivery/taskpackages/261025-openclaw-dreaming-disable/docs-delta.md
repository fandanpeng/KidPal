# docs-delta · 261025-openclaw-dreaming-disable

> closeout 制品（blocking 必交）。生成：integrator，2026-10-25，final round 2 收拢相位。
> TaskPackage `docsSyncRequired=false`；本文件记录零 diff 的核实事实、部署 face 变更去向、以及与 spec 面相关的在途归口（面 C）。

## 结论：`docs/**` 零 diff 声明（含契约面）

1. **git 事实**：`git status --porcelain docs/` 为空、`git status --porcelain docs/contracts/` 为空、`git status --porcelain firmware/` 为空——工作树对 spec 面（`docs/requirements.md` / `docs/architecture.md`）、契约面（`docs/contracts/**`）、ADR（`docs/decisions/ADR.md`）与固件面全部零改动。
   - 附注（闸副作用已还原）：final 相位 `firmware-build` 闸真编译（工具链可用，100.76s）由 CMake 重生成 `firmware/sdkconfig`（+5 行 Image Effects Configuration 空块）。该 diff 是闸副作用而非本包改动，收口 commit 前已 `git checkout -- firmware/sdkconfig` 还原，固件面维持零 diff。
2. **本包实际改动面**（`changeManifest` 四件全在 `server/` 部署与验收面）：
   - `server/deploy/261025-openclaw-dreaming-disable/`（新建三件：声明 / apply / rollback）
   - `server/deploy/manifest.yaml`（+18/−0，三件登记）
   - `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（+12/−0，§9 升级检查清单）
   - `server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py`（新建冻结件，DD-A1..DD-A5 + 编排 / 回滚 / 部署面共 17 条）
3. **声明工件属部署资产而非契约**：`dreaming-disable-declaration.json` 是无 secret 的 OpenClaw 部署声明（`openclaw.json` 含 gateway 凭据、整文件不入仓、仓内零命中），不与 `docs/contracts/interface-contract.md` 的端云接口契约同面；写入面 = 恰一个布尔叶键 `plugins.entries.memory-core.config.dreaming.enabled=false`。
4. **`docsSyncRequired=false` 的成立方式**：本批是「云端部署声明工件 + apply/drift/rollback 脚本 + 部署 runbook 一行升级清单 + 新建冻结验收件」，无 requirements 承诺变更、无架构结构变更、无契约字段变更；`docs-sync` 闸在 r2 final 相位绿（`docsSyncRequired:false`，理由逐字见 fact）。

## 部署 face 变更记录（`server/deploy` 运维面，非 `docs/` 树）

- `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` 新增 **`## 9. 升级检查清单（OpenClaw 或记忆类插件升级后必跑）`**：钉一条盘点口径——`dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 四家族必须为空，违反即采集策略漂移，并引 `docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md`；同时写明发现漂移后应先盘点、再决定是否重跑本包 apply。
  - 去向理由：runbook 是 `server/deploy` 运维面文档（与 `docs/` 白名单无关），由本 `docs-delta.md` 记录变更即可，不触发 docs-sync。
  - 升级清单的语义锚（四家族 token + 「必须为空」口径 + 引 debt 条目）由冻结件 `test_dd_runbook_upgrade_checklist_anchors` 机械承载（源锚代理：钉语义锚组 + 窗口内聚，不钉行数与措辞）。

## debt 条目状态：本包**不改**

- `docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md` 保持 `status: deferred`、`closed: ""` 原样。
  - 该条目的覆盖范围 = **配置包（本包）+ human 数据清理运维轮 + 面 C spec 修订**三段，其 `owner` 写明「dev-loop（配置包）+ 主线程（清理运维轮）+ architect（面 C spec 修订）」。状态流转（partial / closed）应在清理与 spec 修订都完成后由对应动作方推进，本包单独提前关账会掩盖后两段在办事实。
  - 本包对该条目的义务在别处兑现：runbook 升级清单（上节）+ 冻结件（本包验收面）+ 声明 `_note` 的机制与重评触发口径。

## spec 面已知在途（**归面 C，非本包未跟踪漂移**）

历史事实需要如实申报，不因 `docs-sync` 绿而隐去：

- `docs/architecture.md` §6.7 现行文本第 360 行写「② 系统回合（cron / 心跳）由 `captureSystemRuns=false`（默认值）排除」。本次诊断实证了该表述的**覆盖不全**：memory-core dreaming 子回合的 session key 前缀 `dreaming-narrative-memory-core-v2-*` 在 honcho 插件 `dist/helpers.js` 里既不入 `isSystemRun()` 三条件、也不被 `classifySession()` 判为 `subagent:` / `cron:`，故落入 chat 分支被当 owner 话语抽取——`captureSystemRuns=false` 对该类会话无效。本包的对策是在配置面**全局关闭 dreaming**（方案 A），不改 spec 文本。
- 该修订**已由 human 2026-10-25 裁决明确归入在途「面 C」**，并由 debt 条目在册登记（该条目 `paths` 已含 `docs/architecture.md` 与 `docs/decisions/ADR.md`，正文「建议改动 · spec 修订」节写明：§6.7 记忆写入范围由逐行枚举升级为类级规则〔平台内部系统回合不入画像〕、ADR-016 补带日期注记；并附硬约束「不允许先合实现、spec 永远不补」）。
- 因此本包的处理 = **零自改 + 不在本批派 architect**（spec 修订的派发主体是面 C 的 arch 批，不是本包的 docs-sync 动作）；integrator 对 `docs/requirements.md` / `docs/architecture.md` 无写权，本批亦未触发。

## 固件面 / 跨端

- 零固件 diff（`git status --porcelain firmware/` 为空）。`crossEnd.exempt` 成立：本包只改云端 OpenClaw 部署声明 / 脚本与部署 runbook，设备端零感知，不新增或改变端→网关消息、帧类型、字段承载与序列化形态。

## 后续 docs / spec 触碰触发位（声明性注记，非本包动作）

- **面 C spec 修订**（§6.7 类级规则 + ADR-016 日期注记）：按 `/arch-change` loop 走，入口 = debt 条目 `TECH-261025`；本包不合、不写。
- **live 部署轮若发现 `config patch` 对 `plugins.entries.memory-core` 是整对象替换语义**：属部署面事实校正，先停行回报，不在 `docs/` 补写；若结论影响架构对「插件配置合并语义」的表述，再走面 C / arch 批。
- **debt 条目关账**：清理运维轮 + 面 C 修订都完成后，由条目自身生命周期推进（`debt-render.mjs` 归位），本包不预关。
