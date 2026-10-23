# 启动卡 · 261023-honcho-memory-wiring-restoration

跑 `261023-honcho-memory-wiring-restoration`（批 B 记忆接线 bring-up，rigor=lite，riskTier=medium，sourceType=arch-review）。本树 C:/AI/KidPal-pod-m2（dev/pod-m2）即 Pod m2 本体，就地起 loop。

- **reviewed plan**：`delivery/taskpackages/261023-honcho-memory-wiring-restoration/plan.md`（冻结语义六条全读：甲案资格门口径 / 两处残余偏差申报 / peer→session 措辞 / OQ-3 只申报 / pin 1.7.0 / advisory §9 安全铁律）。
- **上游面**：设计权威 = `delivery/review/261022-arch-review-device-memory-prompt-overhaul/memory-wiring-advisory-20261023.md`（architect advisory + human 三项裁决：方案甲 / workspaceId=kidpal / 与批 A 解耦）。批 A（compose 回流）已先行合入 commit f4d5e647——**本包不改 compose**。
- **基线锚**：server acceptance 全量 = **316 passed / 6 deselected / 33 subtests**（批 A+B 后，2026-10-23 本树）。本包红基线由 acceptance-author 冻结相位实跑回填（换代断言设计态红，既有其它件零新增红）。
- **铁律（冻结判法锚定）**：
  - **advisory §9 网关安全**：openclaw.json 只经 OpenClaw 写入口（`config patch`）写、写前必 `--dry-run` schema:true；**禁脚本直写 openclaw.json**（K3 崩因）；不写顶层 `hooks.*`；`agents.entries.*.plugins` schema 拒收（离线复现实证）。
  - **零发明**：上游插件 `@honcho-ai/openclaw-honcho@1.7.0` + 4 键即「验证过的接线」全部；carrier 弃用不重写；noisePatterns 是唯一新增键（configSchema 白名单内、上游已文档化）。
  - **不变更边界**：契约面只读；spec（arch §6.7/§1.3）口径修正归面 C `/arch-change`；honcho_* 挡置归部署轮 OQ-3 实测后另裁；探针库 kidpal-probe 不迁。
- **验收锚换代纪律**：`test_261022_memory_isolation.acceptance.py` 原件原地换代 = **显式治理动作**（advisory §10.4 human 已裁：批 B 锚换代承载，非静默改冻结件）；存活 4 组 + 换代 9 处按 advisory §7 清单逐条；drift_guard P1 现状锚逐字保。
- **live 停等**：host 面转绿后 status `awaiting-human:live-deploy`——live 面（插件 install --pin / 4 键 apply 按 §9 序 / 重启 / 三证采集 / 甲案 one_shot 判读 / 降级实测 / OQ-3 实测）归 human 部署轮或主线程经 human 授权；Pod 不自行上 ECS。
- **残余偏差申报位**：acceptance 头注 B.4/C 节重写（已验证接线四键 + 残余偏差）+ integrator docs-delta 逐条（front 读侧注入 / QQ 落消息）——spec 修正不归本包。
- **会话标识**：human 在场交互式会话（2026-10-23，M5 列车后续批）。
