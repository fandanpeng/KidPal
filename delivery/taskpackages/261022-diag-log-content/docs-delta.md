# docs-delta — 261022-diag-log-content

> 本件 = integrator 制品面（delivery/taskpackages/** 授权内）。**spec 面（docs/architecture.md）integrator 无写权**（scope-map `agents.integrator.allow` 不含两份 spec 文件；human 裁定 ARQ-260929-docs-sync-spec-handoff：spec 面变更交持写权者落盘、integrator 零自改）——本件 §2 为**待落盘 verbatim 句**，落盘主体 = 持 `docs/architecture.md` 写权者（architect 派单或主线程 packet-bound 落地），落盘后 docs-sync 闸即绿。

## 1. docs-sync 状态

- verify 链七闸中唯一红 = **docs-sync**（`delivery/taskpackages/current/verdict.json` 2026-10-22T03:37:41Z round：`docRefs do not mention any declared acceptance ids`，docsWithoutAcceptance = `["docs/architecture.md"]`）。
- taskpackage `docRefs` = `docs/architecture.md` + `firmware/main/test/sd_log.acceptance.c`；机检要求 docRefs 文档内出现声明的验收 id（DLG-A1..A6）。
- `docsSyncRequired: true`、`docsOwner: "integrator"`（taskpackage 冻结字段）与 scope-map integrator 无 spec 写权存在职责冲突——按 ARQ-260929 人类裁定从严：integrator 不自改 spec 文件，本件交回 planner 落盘。这是本包 closeout 前唯一未闭合项。

## 2. 待落盘 delta（verbatim）

**落点**：`docs/architecture.md` §6.6 可观测性（**第 334 行**段落末尾，紧跟「业务日志不落板、语义不变。」之后，同段句接）。架构语义零变更（决策 24 + §6.6 已覆盖日志内容增强，triage architect 已裁），只补内容增强一句 + 验收锚回引；锚批注形态照 docRefs 文件 `firmware/main/test/sd_log.acceptance.c:254` 的 `[C1]-[C5]` 连写先例。

**补入句子（verbatim，只加此一句，其余零改动）**：

> 日志内容增强（`261022-diag-log-content`，2026-10-22）：每行落卡日志带墙上时间前缀（SNTP/RTC 已同步 → `YYYY-MM-DD HH:MM:SS`，未同步降级 `BOOT+<ms>ms` 标注；低频任务预格式化缓存 + 钩子只拼接），并覆盖五类语义记录面——页面跳转（PAGE_SWITCH from→to + trigger）、设备状态机跳转（含被拒转移 state_reject 补打）、按键事件语义名、SystemInfo 周期噪声降频（10s→60s）——落卡管线口径（8.3 名 / 512KB × 32 / 16MB 封顶轮转）不变，验收锚 `[DLG-A1]-[DLG-A6]`。

**落盘后动作**：跑 `node .claude/hooks/verify.mjs` 复跑（期望七闸全 ok——docs-sync 检查 docRefs 文件内出现 DLG id，句尾 `[DLG-A1]-[DLG-A6]` 即满足）。

## 3. 零触碰面

- `docs/contracts/**`（零 wire 形状改动，triage 四面探测裁 NO_CONTRACT_CHANGE）；
- `docs/requirements.md`（需求面零变更）;
- 落卡管线口径（分区表 / sdkconfig / 16MB 封顶 / 轮转数量）——实现面已保证零 diff（DLG-X3）。
