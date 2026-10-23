---
workstream: device-memory-overhaul
slug: device-memory-overhaul
status: active
opened: 2026-10-22
closed:
summary: 设备语音链路「越用越聪明」整体重构——持久会话 + Honcho 记忆画像 + prompt 迁 AGENTS.md + 工具收窄 + 课表机械注入（P1–P5 单 Pod 串行）
owner: main-thread
area: systems
changes:
  - 261022-m5-k1-prompt-carrier-migration
  - 261022-m5-n2a-schedule-mechanical-injection
  - 261022-m5-k2b-persistent-session
  - 261022-m5-k3-honcho-memory-production
  - 261022-m5-k4-history-corpus-backfill
---

# task_plan · device-memory-overhaul（workstream 级分波交付计划）

> 本件 = workstream 拆包顺序链必备节（delivery/workstreams/README.md「拆包顺序链」纪律；portfolio-planner 261022 停等打回的两件补齐之一）。设计权威 = [PROPOSAL.md](PROPOSAL.md)（FROZEN v1.0）；里程碑登记 = `delivery/milestones/milestones.jsonl` M5-device-memory（2026-10-22 补登，state=planning）。

```yaml
workstream: device-memory-overhaul
phases:
  - id: P1
    name: 上下文装配与 prompt 承载迁移
    scope: v11 静态语义迁 workspace-kidpal/AGENTS.md（吸收旧文件底线句）、引擎只发动态 context 小块 + 本回合、tools 覆写 allow=[read,write,exec,web_search]、SOUL/USER 清理；carrier/provider.config/overlay/manifest 同批改「AGENTS.md 主承载」新不变式（ADR-014 amend 声明落地）
    changeManifest: server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md、openclaw-agents/README.md、openclaw-agents/kidpal-dynamic-context.v1.txt（新增）、OPENCLAW_SYSTEM_PROMPT_CARRIER、provider.config.yaml、apply-xiaozhi-overlay.sh、kidpal-agent-prompt.txt（去留/改写 P1 定形，不得两动态模板件并存）、manifest.yaml、verify-deploy.sh、261022-openclaw-kidpal-tools/**（新增，仿 261017 先例）
    riskTier: medium
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality, ai-cost-audit]
  - id: P2
    name: 持久会话（user 标识 + 只发新回合）
    scope: 引擎请求加 user=<device_id>（engine-patch 新 provider 补丁 + bind-mount）+ payload 只留本回合；补丁漂移护栏同批（ARQ-260907 同族）
    changeManifest: server/deploy/engine-patch/connection.py、engine-patch/openai.py（新增）、docker-compose.yml、manifest.yaml、verify-deploy.sh
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: P3
    name: Honcho 生产化 + 守则 v12
    scope: Honcho 栈 loopback 常驻声明 + 生产 profile 插件/hooks 授权、v12 守则（孩子事实必须记/家长隐私不记——落 AGENTS.md 静态语义）、front 一次性回合隔离实测（§6.7 资格门 marker 探针 DB/日志双证）
    changeManifest: server/deploy/honcho/**（新增）、manifest.yaml、apply-xiaozhi-overlay.sh（若配置经 overlay）、verify-deploy.sh、openclaw-agents/workspace-kidpal/AGENTS.md（v12 守则）、front_app.py（隔离过滤，若需）
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [security, real-db, evidence-integrity, ai-cost-audit]
  - id: P4
    name: 历史 corpus backfill
    scope: session-corpus 灌 Honcho（照记全量、不滤姓名——human 2026-10-22 终拍裁）、幂等可重跑、前后计数报告、清库回滚
    changeManifest: server/deploy/honcho/backfill/**（新增）、manifest.yaml、verify-deploy.sh
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [migration, security, real-db]
  - id: P5
    name: 课表机械注入
    scope: 注入源走 schedule.py 单源（front 派生 → 内部通道 → 引擎动态 context 块），落 P1 建立的动态装配机制；消费 TECH-261018
    changeManifest: server/deploy/engine-patch/connection.py（或 context provider 端点）、gateway/schedule.py（只读复用，改动从严）、manifest.yaml、verify-deploy.sh
    riskTier: medium
    rigor: lite
    sourceType: regression
    gateProfile: [evidence-integrity, test-quality]
```

## merge order 与串行约束

**P1 → P2 → P3 → P4，P5 插 P1 之后（可与 P2 换序）；单 Pod 串行**——共享文件面（manifest.yaml / provider.config.yaml / apply-xiaozhi-overlay.sh / OPENCLAW_SYSTEM_PROMPT_CARRIER / engine-patch/connection.py / workspace-kidpal/AGENTS.md）禁止并行认领；拆包价值在独立验收/回滚边界，不在并行。

## launch 前置（launchPreconditions，已全部满足）

- 需求面 frozen：requirements **v1.67**（§9 v1.67 行在案，b4b9c90a）；
- 架构面变更记录 changeId 行在场：architecture.md §12 首行 `261022-device-memory-prompt-overhaul`（0411e957，landing-verdict PASS）；
- 契约包随批原子落：§四 :283 定案改写 + 变更记录条 + §1.6 对账注记（同上 commit）。

## 已知窗口与边界（转 P1/P3 brief）

- **AGENTS.md md5 漂移已知窗口**（架构 PROPOSAL §5 第 11 条）：仓内镜像已反转、live 未铺——P1 落地前不得单独 `deploy.sh` 铺放该文件（护栏真空）；verify-deploy 报漂移不判红。
- P3 验收含 marker 探针 DB/日志双证（§6.7 资格门）+ seeded 负向探针集（AC3）。
- OQ-10 已裁：备份 = 不备份靠重建（重建源 = session-corpus 源语料 + OpenClaw 会话存储；重评触发见 arch §8.1 风险 13）。
