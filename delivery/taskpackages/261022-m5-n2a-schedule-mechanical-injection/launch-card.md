# 启动卡 · 261022-m5-n2a-schedule-mechanical-injection

跑 `261022-m5-n2a-schedule-mechanical-injection`（workstream device-memory-overhaul P5，rigor=lite，riskTier=medium，sourceType=regression，mergeOrder=51，dependsOn=K1 已合并 ✓）。本树即 Pod 本体就地起 loop。

- **reviewed plan**：`delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/plan.md`（含 2026-10-23 planner 注记：注入落点 = `kidpal-agent-prompt.txt` 的 `{{schedule_summary}}` 占位，装配壳无内容占位勿改；v11 声明面刻意保留）。
- **上游面**：K1 已合并并部署实测达标（progress.md 2026-10-23 三节）；需求 v1.67 frozen；arch 批 T5 landed。launch 前置满足。
- **基线锚**：server acceptance = **268 passed / 4 deselected / 33 subtests**（K1 落地后，2026-10-23 本树实测）；unit 115 passed。本包红基线由 acceptance-author 冻结相位实跑（新断言设计态红）。
- **架构权威**：arch review PROPOSAL **T5**（注入语义：逐轮在场 / 有界 / 会话建立时刷新 / 窗口外诚实）+ 契约 §1.5 单源红线（引擎不解析课表 MD、不建第二解析器）。注入通道形态（front 派生 → 内部通道 → 引擎拼入动态块）以 T5 + connection.py / front_app.py 现状为准；若冻结/实现发现必须改 front 侧文件而 changeManifest 未含 → 停，报 planner 扩单。
- **live 停等**：K5-E4 live 复证（真机 ≥20 轮课表提问零编造的 live 段）挂 human 部署轮——host 面转绿后 status `awaiting-human:live-deploy`。部署面：connection.py 为 engine-face（deploy.sh 重启引擎 = 设备断连重连，K1 同款）；注意 ECS 默认 python3=3.6 坑。
- **合并门预期**：riskTier=medium 非 high；lite 无特殊豁免；gateProfile 两项（evidence-integrity / test-quality）。
- **会话标识**：human 在场交互式会话（VSCode 扩展，2026-10-23 K1 同会话续跑）。
