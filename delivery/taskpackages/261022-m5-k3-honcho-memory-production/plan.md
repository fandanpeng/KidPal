---
changeId: 261022-m5-k3-honcho-memory-production
title: Honcho 记忆生产化 + 守则 v12——五容器 loopback 常驻、生产 profile 插件与 hooks 授权、资格门（user 缺席即非资格）、孩子事实必须记 / 家长隐私不记、一次性回合隔离实测
rigor: full
riskTier: high
sourceType: proposal
reviewed-by: architect（workstream PROPOSAL FROZEN v1.0 + arch PROPOSAL FROZEN v1.1；门 1 待 human 派 architect）
date: 2026-10-22
gateProfile: ["security", "real-db", "evidence-integrity", "ai-cost-audit"]
dependsOn: ["261022-m5-k2b-persistent-session", "261022-m5-k1-prompt-carrier-migration"]
mergeOrder: 53
---

# 背景摘录（锚「现状与差距（新建型）」，PROPOSAL §2）

- **已就绪**：Honcho 自托管栈已在 ECS 运行（`/opt/honcho`，`127.0.0.1:18100`，五容器全绿）；探针 profile（`/root/.openclaw-probe`）端到端验证通过——openai-http 钩子触发、deriver 以 deepseek-v4-flash 抽 observation、representation 产出结构化事实、`before_prompt_build` 跨 session 注入生效（零历史新会话答出已记录事实）。
- **差距**：记忆未接入设备主链路（探针 profile 隔离验证，生产 profile 未装插件）；v11 隐私守则过度泛化（实测拒记同学真名 / 宠物名：「家庭小私事不特别记下来」），与记忆目标直接冲突；课表之外的记忆写入资格未建。
- **资格门已由架构批定形**（arch PROPOSAL T4 / `docs/architecture.md` §6.7）：仅 kidpal 语音链路且请求携 `user=<device_id>` 的回合可读写记忆；main agent / cron / front 一次性回合一律不入库；**白名单入册，不做黑名单剔除**；AC5 完整枚举 = §6.7 六行表。
- **隐私边界三层纵深**：来源门（资格门）/ 抽取策略（deriver 只抽孩子世界事实；首选 deriver 提示配置，若自托管版本不支持则退回消息侧过滤）/ 注入过滤（低置信 / 敏感类不回注）；验收 = seeded 负向探针集 100% 零命中。

# 可行性判据

- 机制已探针实证（TECH-261022 证据 7）；生产 profile 安装插件 + `hooks.allowConversationAccess=true` 后一次性路径钩子会正常触发（debt 证据 7：不隔离则入库），因此本包的隔离判据必须先于插件启用量产验证。
- P2 已提供 `user` 字段：资格门判据与 P2 语义同源、零新标识符；若 P2 未合并则本包不得启动（dependsOn 硬边）。
- **marker 探针可闭环**：DB（Honcho postgres）与日志双证；主链路正证 / 一次性回合负证 / seeded 负向集三组。
- 抽取策略实现形态「deriver 提示可配置性」未验（arch §6.4 留根）——本包实测后若不可配，按 arch T4 退回消息侧过滤，并在交付中如实申报；§6.7 的实测成形句回填若需改 spec，走 `/arch-change`，不在 Pod 内改。
- 资源面：ECS 已升 2C/4G；prep 税 / 内存压力读数在本包后复测回填 `TECH-261022-openclaw-prep-tax-memory-pressure`（该条目记录的是升级前 2GB 读数）。

# 跨包事实与共享冻结面约束（只有 RTE 看全局的部分）

- **`AGENTS.md` 与 P1 共享**：P1 已迁移 v11 静态语义并保持「静态语义单一事实源」自声明；本包只追加 v12 记忆守则（孩子世界事实必须记 / 家长隐私信息不记），**不得回改 P1 迁移段的语义**，也不得把纪律放回引擎动态块。
- **`openclaw.json` 声明式 patch 组合**：本包的 Honcho 插件 + `hooks.allowConversationAccess` 声明与 P1 的 tools allowlist 声明作用于同一运行态文件。apply 顺序 = P1 先、P3 后；本包 drift_guard 现状锚 = P1 已应用态；两包都只做声明 + 幂等 apply，不整文件改写；`openclaw.json` 含凭据、不入仓。
- **`user` 常量与 peer 键**：本包消费 P2 的 `user=<device_id>` 与 `agent:kidpal:openai-user:<device_id>` 键，不得另造 peer 键；device 级 peer 先行、多设备身份不做（OQ-7）。
- **P4 消费本包的 peer 与画像边界**：backfill 按本包设备的 peer 落库；历史照记豁免不分回合性质，但新增回合仍走本包资格门——两条口径不互串。
- **front 一次性回合**：`server/gateway/front/front_app.py` 的 `_openclaw_chat_once` 现状不携 `user`；若实测发现仍有携 `user` 的旁路，最小改动隔离，不改 provider 单源构造与契约 §四禁 `max_tokens` 约束。
- **manifest marker 锚**：若 Honcho 配置经 overlay 渲染，P1 更新后的 `.config.yaml` 锚集由本包追加 Honcho 相关锚；不得回退 P1 的动态块锚。
- **与 P4 的 changeManifest 子树分层（门 1 终裁 A4）**：本包 `server/deploy/honcho/**` 的 glob 与 P4 的 `server/deploy/honcho/backfill/**` 重叠——**`server/deploy/honcho/backfill/**` 子树归 P4**。本包只建栈本体（compose / 配置 / apply-rollback / 健康检查），不预建、不改动 backfill 目录；P4 只在其子树内新增、不动本包栈文件。两包的 `manifest.yaml` / `verify-deploy.sh` 条目按 merge 顺序各自串行追加。

# 铁律约束

- 契约面只读；记忆库形状非契约；`kidpal_story_delete` 现行边界不变（删除记忆是独立运维动作）。
- 五容器端口全部 127.0.0.1；出网仅 NFR-SEC-2 ⑤ 两条已记账管线；不得新增未记账出网路径。
- 资格门读写同门：非资格回合既不写 observation 也不注入画像；不做黑名单剔除。
- 删除 = 运维面手工、范围限定（画像 / observation / 消息存储），不删 OpenClaw 完整日志、回填源语料、诊断日志；Phase 1 无自助删除界面。
- 备份 = 不备份靠重建（OQ-10 human 裁）；不得引入 pg_dump 本机副本或出网备份。
- 本 Pod 只做本包 changeManifest 内文件；spec 需改 → 走 `/arch-change` 逃生门。

# demo 证据形态（与 roadmap 一致）

- host runner 输出：隔离 marker 探针（DB / 日志双证）+ seeded 负向探针集 + 降级路径 + 声明式 patch 幂等/回滚。
- 部署后 live：human 持板——新 WS 新会话问「我最喜欢的颜色 / 我的小狗叫什么」答对；front 整理 / 引导回合 marker 零命中；隐私负向套问不复述。
- 资源读数：prep 税 / 内存 / `event_loop_delay` 前后对照，回填 debt 条目。
