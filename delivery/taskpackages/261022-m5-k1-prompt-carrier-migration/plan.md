---
changeId: 261022-m5-k1-prompt-carrier-migration
title: prompt 承载迁移与上下文装配——v11 静态语义迁 workspace AGENTS.md、引擎只发动态 context 小块、kidpal agent 工具面收窄为四项，三面接线与 ADR-001 重定义不变式同批
rigor: full
riskTier: medium
sourceType: proposal
reviewed-by: architect（workstream PROPOSAL FROZEN v1.0 + arch PROPOSAL FROZEN v1.1，R2 靶向复核 GREEN；门 1 待 human 派 architect）
date: 2026-10-22
gateProfile: ["evidence-integrity", "test-quality", "ai-cost-audit"]
dependsOn: []
mergeOrder: 50
---

# 背景摘录（锚「结构性痛点（重构型）」，PROPOSAL §2）

- **prompt 承载拓扑**：v11 作为引擎注入的 system prompt（`provider.config.yaml` → overlay 渲染进 `.config.yaml` 的 `prompt:` 键 → 引擎作为 `{{base_prompt}}` 注入 `kidpal-agent-prompt.txt`）落在 OpenClaw 缓存边界之下（参考 `openclaw-main/src/agents/system-prompt.ts:1297,1333-1336` 的 `extraSystemPrompt` 段），不可缓存，且与 workspace `AGENTS.md`（1.7k）内容重复。实测单轮 `prompt_tokens=22,116`、`prompt_cache_hit_tokens=7,168`、`prompt_cache_miss_tokens=14,948`（TECH-261022 证据 2）。
- **工具面**：全局 `tools.profile="coding"` 对设备 agent 意味着 34 工具 57KB schema/轮；设备只需 4 项（read / write / exec / web_search）（TECH-261022 证据 3）。
- **承载反转已由架构批钉死**（arch PROPOSAL T3 / `docs/architecture.md` §5.5.5）：静态语义单一事实源 = `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（缓存边界之上）；动态 context 模板 = `server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`（新增，缓存边界之下）；**否决 v12 双件、不新建 `server/deploy/prompts/` 平行目录**（OQ-8）。
- **既有 `server/deploy/kidpal-agent-prompt.txt` 的去留/改写由本包定形**：「由新动态模板件取代」或「原地改写」二选一，**不得两个动态模板件并存**（arch PROPOSAL §6.1 R1-F3）。

# 可行性判据

- 承载反转的 spec 前提已满足：`docs/architecture.md` §5.5.5 整节重写、AGENTS.md 自声明反转、README / manifest note 反转均随 arch 批 landing（commit `0411e957`）。本包只落实现面与部署资产。
- host 面可闭环：三面接线一致性、manifest / marker 锚静态可解析、tools 声明 apply/rollback 幂等、既有验收套件可离线跑；live 面（token 体积 / cache 命中 / 工具列表）归 human 部署轮观测。
- **已知窗口**：AGENTS.md 仓内镜像改动后、P1 deploy 前，`verify-deploy.sh` 会报该文件 md5 漂移——属预期已知窗口，不据此判红；**P1 落地前不得单独铺放该文件**（v11 语义尚未迁入时提前反转承载声明 = 护栏真空，arch PROPOSAL §5 第 11 条 / T3 第 5 条）。
- **manifest marker 碰撞（本包必须同批解）**：`.config.yaml` 现 marker 锚集含 `"实时事实回合"`——它是 v11 静态内容锚。本包把静态语义移出引擎 prompt 后，该锚必失效；manifest.yaml 条目与 verify-deploy.sh 必须在同批撤换为动态块锚（如本回合时间/日期、课表摘要占位），否则部署核验必红。

# 跨包事实与共享冻结面约束（只有 RTE 看全局的部分）

- **单 Pod 串行**：本包是 P1，与 P2 / P3 / P5 共享 `manifest.yaml` / `provider.config.yaml` / `apply-xiaozhi-overlay.sh` / `OPENCLAW_SYSTEM_PROMPT_CARRIER` / `engine-patch/connection.py` / `workspace-kidpal/AGENTS.md`——禁止并行认领；merge order = P1 → P5/P2（P5 可先于 P2）→ P3 → P4。
- **P3 会再改同一份 AGENTS.md**：本包负责迁移 v11 静态语义并保持 arch 批已落的自声明；P3 只追加 v12 记忆守则（孩子事实必须记 / 家长隐私不记）。本包不得越写 P3 的守则内容；P3 不得回改本包迁移段的语义。
- **P5 消费本包建立的动态 context 块**：本包只建机制与有界模板，不实现课表摘要内容；P5 的注入内容必须落在本包模板的占位/装配面内，不得另建第二动态块。
- **openclaw.json 声明式 patch 组合**：本包的 `261022-openclaw-kidpal-tools/**` 与 P3 的 Honcho 插件/hooks 声明工件作用于同一运行态文件（`openclaw.json` 不入仓）。本包先落，P3 的 drift_guard 现状锚须以本包已应用态为准；两包均不得整文件改写。
- **冻结验收语义锚升级**：`test_prompt_v7_round_routing.acceptance.py` / `test_261010_engine_decision21_overlay_persist.acceptance.py` / `test_261017_openclaw_kidpal_skills.acceptance.py` 承载现行三面接线与 v11 语义断言。承载拓扑反转后这些断言需语义升级（acceptance-author 留痕 supersede），**不静默改写冻结断言**；`test_261001_openclaw_model_name_overlay_drift.acceptance.py` 作为声明式 patch 同型守卫参照。
- **契约面只读**：`docs/contracts/**` 本包零改；契约 §1.6 承载对账注记已随 arch 批落稿，本包不改契约字面；display E1 线格式零改动。
- **非本包**：v12 守则内容（P3）、Honcho 记忆（P3/P4）、课表注入内容与单源反证（P5）、持久会话 payload（P2）。
- **SOUL/USER 清理（human 2026-10-22 门 1 终裁 A1，live 处置项）**：live 核实 `workspace-kidpal/SOUL.md` 与 `USER.md`（ECS 生产 workspace live 在场，仓内无镜像、不在本包 changeManifest）——存在则清空/瘦身，处置登记为 P1 launch card 人工步骤，随 deploy.sh 铺放批次一并处理；不存在则显式记为 moot。不得据此扩包 changeManifest；host 面不作自动断言，按 live 观测项诚实申报。

# 铁律约束

- 契约防御：本 Pod 任何角色不得写 `docs/contracts/**`；疑似契约变更走 architect-reviewer 四分类 + `/arch-change`，不在 Pod 内自裁。
- 不新建 v12 同义副本、不新建 `server/deploy/prompts/` 目录；v1–v11 历史 prompt 文件原地冻结、零回改。
- 动态块有界且不含静态纪律；静态纪律只进 AGENTS.md。
- `openclaw.json` 整文件（含 gateway 凭据）不入仓；只入无 secret 声明 + apply/rollback 脚本。
- 部署单一事实源 = `manifest.yaml`；新增/变更资产必须登记并接入 `verify-deploy.sh`；live 铺放只经 deploy.sh，不在 Pod 内手工上 ECS。
- 本 Pod 只做本包 changeManifest 内文件；发现需要扩单 → 停、报 planner 走 architect 评估，不自行扩面。

# demo 证据形态（与 roadmap 一致）

- host runner 输出（pytest 验收套件 + 静态接线/锚集断言）。
- 部署后 live：human 执行 deploy.sh 后 `verify-deploy.sh` 绿；单轮 DeepSeek `prompt_tokens` ≤11,000 与 cache 命中观测；`openclaw` 工具列表 = 4 项。
- 真机轮默认不在本包（核心包配额留 P5 收口轮）；如 human 要求可加轮，仅做抽问回归。
