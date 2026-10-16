# docs-delta · 261016-sd-card-mount

> integrator closeout 裁定 · 2026-10-16 · `docsSyncRequired=true`（taskpackage.json docsSyncReason 同源）· 事实态盘点同步，无设计决策变更，不升级架构面。

## 1. `docs/architecture.md` —— 已落盘（architect 代写），integrator 抽查

- **改动 = §5.3.1 存储行盘点（:232）单行**：`**SD 槽固件未接入**` → `**SD 槽固件已接入**（SDMMC 1-bit 挂载 \`/sdcard\`，能力就绪 2026-10-16，261016-sd-card-mount；业务数据不落板、决策 24 介质禁令不变）`。
- **抽查结论**（`git diff -- docs/architecture.md` = 单行，实证）：行文与事实态一致（SDMMC 1-bit `/sdcard` 能力就绪，业务数据不落板、决策 24 不变），与 plan §随批 doc-sync 义务逐字对应；无额外行、无设计决策引入。
- **落点通道** = architect allow 覆盖 `docs/architecture.md`（261010-device-reconnect-on-server-close 先例）→ integrator 只抽查不重写（scope-map 实证 integrator 无该文件写权）。

## 2. `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:61-62` —— **待主线程落笔（docs/notes，无 agent 写权）**

**本项不由 integrator 落笔**：scope-map 实证 integrator 无 `docs/notes/**` 写权，且仓内**无任何 agent** 的 allow 覆盖 `docs/notes/**`（planner 亦被 `deny: docs/**` 拦截，本 Pod 实测 pod-events friction `26636485`）。故本 Pod 以「待主线程（human 主会话，无 agent_type）落笔」移交，exact text 存本节。

改前（当前 :61-62 逐字）：

```
未接入（官方在板、固件未用）：**KEY 键（GPIO 18）**、Micro SD 卡槽（注意：art 资产
用的是内部 Flash 的 SPIFFS art 分区，非 SD 卡）。
```

改后（ready-to-apply 逐字）：

```
接入变更（原「未接入」两件均已接）：**KEY 键（GPIO 18）** 已随 260911-key-route-a1 接线（三手势）；
**Micro SD 卡槽** 已随 261016-sd-card-mount 接入（SDMMC 1-bit 挂载 `/sdcard` 能力就绪，2026-10-16；业务数据不落板、决策 24 介质禁令不变）。art 资产用的是内部 Flash 的 SPIFFS art 分区，非 SD 卡。
```

注：该文件行尾 **CRLF**（保持原样，勿 EOL 归一）。KEY 键 stale 行的修正是 plan §随批 doc-sync 明列项。

## 3. 契约面 / 其它 doc 面

- `docs/contracts/**` 全程只读、零 diff（plan 四面探测实证 `NO_CONTRACT_CHANGE`）；契约零触，无需 `architect-reviewer` 四分类、无 `/arch-change` loop 触发。
- `docs/requirements.md` / `docs/architecture.md`（除上述单行）/ ADR：零附加事项（`adrRequired=false`）。
- `docs/requirements.md`（需求面 spec）与 `docs/architecture.md`（架构面 spec，除本行）**无漂移**——本包为事实态盘点同步，非 spec 面变更，无 architect 更新派发（`ARQ-260929-docs-sync-spec-handoff` 触发条件未命中）。

## 附注

- 本包实现未引入新契约暴露面、新依赖、跨层调用。
- §2 待落笔项 = `exitCriteria` 未闭项之一（见 `review-packet.md` 未覆盖面申报），待主线程（human 主会话）落笔后闭合；pod-events friction `26636485` 已记通道脱节（plan【doc 行落点】通道假设与 enforce-scope 实况脱节）。
