# 启动卡 · 261022-m5-k3-honcho-memory-production

跑 `261022-m5-k3-honcho-memory-production`（workstream device-memory-overhaul P3，rigor=full，riskTier=high，sourceType=proposal，mergeOrder=53，dependsOn=K1+K2b 均已合并且 live 达标 ✓，无在途冲突）。本树即 Pod 本体就地起 loop（K1/P5/P2 同款）。

- **reviewed plan**：`delivery/taskpackages/261022-m5-k3-honcho-memory-production/plan.md`（跨包事实节七条全读：AGENTS.md 与 P1 共享 / openclaw.json 双声明式 patch 组合 / peer 键消费 P2 常量 / P4 消费本包 peer 与画像边界 / front 一次性回合 / manifest marker 锚 / 与 P4 的 changeManifest 子树分层）。
- **上游面**：需求 v1.67 frozen；workstream PROPOSAL FROZEN v1.0（sha1:b03fdcbb2150cbfa）+ arch PROPOSAL FROZEN v1.1 两面已 landing；契约 §四只读。launch 前置满足。
- **基线锚**：server acceptance = **292 passed / 0 failed / 4 deselected / 33 subtests**（P2 落地后，2026-10-23 本树实测）。本包红基线由 acceptance-author 冻结相位实跑回填（新断言按设计态红，既有套件零新增红）。
- **架构权威**：arch review PROPOSAL **T4**（资格门：仅 kidpal 语音链路且请求携 `user=<device_id>` 的回合可读写记忆；**白名单入册、不做黑名单剔除**；读写同门）+ arch §6.7 六行表（AC5 完整枚举承载）+ workstream PROPOSAL §3.1/§3.2（R3 五容器 / R4 出网认账）。
- **纪律红线（冻结判法锚定）**：
  - **A4 禁区**：`server/deploy/honcho/backfill/**` 子树归 P4——K3 只建栈本体（compose / 配置 / apply-rollback / 健康检查），不预建、不改动 backfill 目录。
  - **A2 归属**：`server/deploy/261022-openclaw-honcho-plugin/**` 属种子外扩、确认归本包（human 2026-10-22）。
  - **openclaw.json 双声明式 patch 顺序**：P1 tools allowlist 已应用在先（live 态 allow=[read,write,exec,web_search]）；本包 Honcho 插件 + hooks.allowConversationAccess 的 patch 须**顺序 apply、幂等**，drift_guard 以 P1 已应用态为现状锚；**openclaw.json 整文件不入仓**（含凭据）。
  - **AGENTS.md 单向追加**：只追加 v12 记忆守则（孩子世界事实必须记 / 家长隐私信息不记），**不得回改 P1 迁移段语义**，纪律不回引擎动态块。
  - **peer 键不另造**：消费 P2 的 `user=<device_id>` 与 `agent:kidpal:openai-user:<device_id>` 键；device 级 peer 先行、多设备身份不做（OQ-7）。
  - **出网红线**：五容器端口全 127.0.0.1；出网仅 NFR-SEC-2 ⑤ 两条已记账管线（deriver→DeepSeek、embedding→DashScope），不得新增未记账出网；备份 = 不备份靠重建（OQ-10），不引 pg_dump 本机副本或出网备份。
  - **删除语义**：运维面手工、范围 = 画像与 observation/消息存储；不删 OpenClaw 完整日志 / 回填源语料 / 诊断日志；无自助删除界面。
  - **抽取策略退路**：deriver 提示可配置性未验——实测不可配则按 arch T4 退回消息侧过滤并如实申报；§6.7 实测成形句回填若需改 spec 走 `/arch-change` 逃生门。
- **front 隔离（若需）**：`server/gateway/front/front_app.py` 在 changeManifest 内；隔离判据 = 请求是否携 user（R7/OQ-4），读写同门；最小改动，不改 provider 单源构造与契约 §四禁 `max_tokens` 约束。
- **源 ref 核对**：brief-inputs 对 `NFR-PRIV-1` / `NFR-SEC-2` / `TECH-261022-*` 裸 token 形态报「文件不在磁盘」为探针误报（P2 同款）——权威在 `docs/requirements.md` 与 `docs/debt/tech/` 同件全路径 ref，可解析。**A5：OQ 编号两套不同源，按包内 sourceRefs 精确指针读，勿裸引 OQ 号**。
- **P5 遗留观察**：装配壳 jinja2 单遍渲染 `{{today_date}}` 等占位字面泄漏 = 外观级候选，**不在本包扩单**。
- **live 停等**：K3-E7 = live 观测项（新 WS 新会话跨会话回忆对照 / 2C/4G prep 税与内存读数复测回填 debt 条目 / §6.7 抽取策略实测成形），**零自动断言、docstring 申报**——host 面转绿后 status `awaiting-human:live-deploy`。
- **合并门预期**：riskTier=high → supervisor 闸 ESCALATE 停人类合并门为**预期路径**（K2b 同款）；gateProfile = security / real-db / evidence-integrity / ai-cost-audit。
- **会话标识**：human 在场交互式会话（2026-10-23 K1/P5/P2 同会话续跑）。
