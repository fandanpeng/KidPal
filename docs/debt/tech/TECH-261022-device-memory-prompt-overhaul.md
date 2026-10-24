---
id: TECH-261022-device-memory-prompt-overhaul
title: 设备语音链路「越用越聪明」整体重构待 triage——持久 session（openai-http user 字段）+ Honcho 用户画像记忆 + 静态 prompt 迁 AGENTS.md + 工具白名单收窄 + 课表机械注入（现状单轮 prompt 22.1k tokens、跨会话事实答不出、课表幻觉可复现）
domain: tech
status: deferred
severity: MAJOR
owner: human（/triage-change 分流；跨实现面 + 生产运维 + prompt 资产）
trigger: 下一次 /triage-change 分流；或下次动 OpenClaw 部署 / 引擎调用面的批
opened: 2026-10-22
closed: ""
closedBy: ""
paths: ["server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md", "server/deploy/openclaw-system-prompt.v11.md", "server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER", "server/deploy/apply-xiaozhi-overlay.sh", "server/deploy/provider.config.yaml", "server/gateway/provider.py", "server/gateway/front/front_app.py"]
source: ["2026-10-22 讨论稿（工作区外，未入 VCS）：c:/tmp/kidpal-memory-plan-2026-10-22.md", "2026-10-22 探针实测：ECS 隔离 profile /root/.openclaw-probe（端口 18790）+ 自托管 Honcho（/opt/honcho，127.0.0.1:18100）"]
---

**证据**（2026-10-22 实测，全部可复跑）：

1. **每请求新 session**：部署版 bundle `/root/.npm-global/lib/node_modules/openclaw/dist/http-utils-Buw61ytF.js` 的 `resolveSessionKey` 逐字为 `const mainKey = user ? \`${prefix}-user:${user}\` : \`${prefix}:${randomUUID()}\``——不带 `user` 字段即每轮随机 session。探针验证：同 `user` 两轮可回忆（第 1 轮告知「喜欢紫色」→ 第 2 轮答出），不带 `user` 的对照轮不知。
2. **单轮真实 prompt = 22,116 tokens**：探针抓包（转存代理）DeepSeek 请求实测 `prompt_tokens=22116, prompt_cache_hit_tokens=7168, prompt_cache_miss_tokens=14948`；system 单条 22,167 字符；**34 个工具、schema 共 57,120 bytes**。此前 TECH-261018 语境下引用的「14,946 input」实为 cache miss 部分，非全量。
3. **工具面过宽**：live `openclaw.json` 仅全局 `tools.profile="coding"`，kidpal agent 无覆写；coding 档实测含 30+ 工具（memory_search/memory_get、sessions_*、subagents、cron、image、skill_workshop…）。
4. **课表幻觉复现**：探针课表回合（tool_choice=auto、工具可用）模型语音称「我先看一下课表哈」但零工具调用，直接编造「周四=语文/数学/英语/科学 + 16:00-17:00 美术课带画笔」；真实 `workspace/kidpal/schedule.md` 周四 = 语文/数学/道法/体育/英语/劳动/足球 + 18:30-20:00 编程。与 TECH-261018-schedule-read-compliance-residual 同族。
5. **v11 隐私守则过度泛化**：探针实测模型拒记「同学真名」「小狗名字」（原话如「家庭小私事，我这边就不特别记下来了」），与「越用越聪明」目标冲突。
6. **OpenClaw 原生记忆空转**：dreaming 语料库含 2026-09-30 孩子讲述的「王一诺」完整对话（`workspace-kidpal/memory/.dreams/session-corpus/2026-09-30.txt`），但 `memory/dreaming/deep/` 报告恒为「Ranked N, Promoted 0」、workspace 无 MEMORY.md；`openclaw memory search --agent kidpal '王一诺'` 实测命中「王水」噪声（无 embedding、corpus 不在索引、CJK trigram 弱匹配）。
7. **Honcho 端到端已验证**（自托管于 ECS，探针 profile）：openai-http 一次性路径钩子正常触发（`hooks.allowConversationAccess=true` 后）→ 消息入库并映射 `openclawSessionKey=agent:kidpal:openai-user:device-probe-honcho` → deriver 以 deepseek-v4-flash 抽 observation → representation 产出「favorite color is purple / dog named Doudou」→ **全新 user 字段零历史会话问「我最喜欢的颜色」答出紫色**（before_prompt_build 注入生效）。配置要点：`EMBEDDING_VECTOR_DIMENSIONS=1536`（dashscope text-embedding-v4 须显式维度，pgvector 列固定 1536）、`DERIVER_FLUSH_ENABLED=true`。

**影响面**：设备主链路（每个语音回合）。不修的代价：孩子的事实永远记不住（产品核心承诺缺失）；每轮多付 ~1.5 万 token；课表类回答存在编造风险（MAJOR 级正确性）；v11 纪律在 22k token 里被稀释。

**建议改动**（候选，交 triage 与各面门，不自批）：

- **第 1 层（调用形态）**：① v11 静态部分迁入 `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` 并同步部署（引擎每轮只发动态 context 小块 ~600 字符 + 用户回合）；反转「AGENTS.md 次要承载」表述；v11 由此进入稳定前缀，缓存命中提升。② 引擎请求加 `user: <device_id>`，同批改为每轮只发新回合（避免历史双倍累积）。③ kidpal agent tools 覆写 allow=[read,write,exec,web_search]。④ 清空/瘦身 SOUL.md、USER.md。
- **第 2 层（记忆）**：⑤ Honcho 生产化（生产 profile 装插件 + 钩子授权 + baseUrl 指本机）；⑥ v11→v12 守则修订，区分「家长隐私不记」与「孩子事实必须记」；⑦ 历史 session-corpus backfill 进 Honcho。
- **第 3 层（正确性）**：⑧ 课表回合由引擎机械注入当日课表内容进 `<context>`（与 TECH-261018 候选根治同向，落地即消费该条）。
- **第 4 层（战略/资源）**：⑨ 保留 OpenClaw 但按需瘦身（Honcho 跑通后其钩子/记忆价值成立）；⑩ ECS 已升 2C/4G，TECH-261022-openclaw-prep-tax-memory-pressure 的 prep 税与内存压力应在同批复测。
- **不改**：引擎 `Intent` 维持 nointent（OpenClaw 拒收引擎 tools 数组，见 ARQ-260907）；thinking 维持关闭（模型侧延迟、语音 UX）；分回合 thinking 不纳入本批（human 2026-10-22 裁）。

**风险**：riskTier: medium——改动横跨实现面（引擎/部署资产）、生产配置与 prompt 资产，但全部可回滚（配置备份 + prompt 版本化 artifact）；Honcho 自托管数据留 ECS 本机，隐私面可控；主要风险是 prompt 迁移后行为回归，需探针 + 真机双验。

---

**结项注记（2026-10-23，append）——P1 遗留「装配壳 jinja2 单遍渲染占位字面泄漏」观察项结项**：P5 live 实测 transcript 全量 `{{` 零命中、日期类回答全部正确（progress.md 2026-10-23 P5 部署节），定性**外观级、无行为级泄漏**；advisory review S7（delivery/review/261022-arch-review-device-memory-prompt-overhaul/agents-md-advisory-20261023.md）复核本文件无一句依赖这些占位、判「无需为 {{today_date}} 改措辞」；唯一新依赖（课表摘要行）由批 A+B `261023-agents-md-static-semantics-alignment` 的 B1 降级句承载（摘要缺失/截断 → 读文件或诚实说查不到，同时覆盖引擎缺省文本「（本次会话未提供）」）。**结项、不另起批**（human 2026-10-23 指示「其他的先记录到 debt」语境下的裁量为结项记账而非挂账）。

---

**勘误 append（2026-10-23，主线程，批 B 261023-honcho-memory-wiring-restoration 收口记账）——证据 7 键形措辞勘误**：① 证据 7 所记 `hooks.allowConversationAccess=true` 为**缩略写法**，真实全路径 = `plugins.entries.openclaw-honcho.hooks.allowConversationAccess`（插件条目内；顶层 `hooks` 键在 OpenClaw 2026.8.1 schema 只有 `internal`，写顶层 = config 校验 exit 78 网关崩循环——2026-10-23 部署轮实测）。② 证据 7 所记 `openclawSessionKey=agent:kidpal:openai-user:device-probe-honcho` 是 **session key（session metadata / OpenClaw 会话键），不是 peer 键**——探针库 peers 只有 `owner` 与 `agent-kidpal`，不存在设备级 peer。③ 接线本体 = 官方 npm 插件 `@honcho-ai/openclaw-honcho@1.7.0`（`openclaw plugins install`），非自研 carrier——K3 自研 carrier 三层失效已弃用（261023-honcho-memory-wiring-restoration）。权威口径 = arch §6.7（261023-arch-review-memory-wiring-verified-behavior 改后版）+ ADR-016。

**运维调参 append(2026-10-24,主线程,K4 live 轮)——deriver 输出上限 2500→8000**:K4 backfill 灌入 1716 条后,deriver representation 工作单元约 27% 因输出截断失败(finish_reason=length,DEFAULT_MAX_TOKENS=2500,静默失败仅日志可见)。human 裁:放大上限多花 token 好过静默失败。/opt/honcho/.env 增 `LLM_DEFAULT_MAX_TOKENS=8000` + `docker compose up -d deriver` 重建生效(注意:docker restart 不重读 .env,compose env 是容器创建时定死)。**ECS live 改动未入仓**(.env 不入仓惯例);候选监控:drift timer 加 queue with_err 计数告警(同 edge-tts 探针思路,未做)。

---

**挂账 append（2026-10-24，integrator，K4b 261024-m5-k4b-corpus-parser-v2 收口记账）——transcript 影子导出器重建（human D2 裁：transcript 批次一并回滚、不重灌）**：K4 归因修复轮（architect 对抗 review `delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-attribution-fix-review-20261024.md` §1.2/§2.2）实锤 transcript 影子导出三缺陷：① 导出脚本（ECS `/tmp/export_shadow.js`，**不在仓内**）用裸 `seq` 当 shadow 主键，真实表主键是 `(session_id, seq)`——distinct seq 仅 120、1187 行主键冲突被静默 `skipped`，灌入的 64 条是「按 seq 去重后的偶然抽样」；② 跨会话混采（P2 会话外混入 2 条以上其它会话事件）且不可复现（重跑同脚本结果会变 120 行）；③ 3 条 `toolResult` 误归 owner（K4B-E4 已在解析器 v2 修 `role in {assistant,toolResult,tool} → agent-kidpal` 路由，但 transcript 批次不重灌、修复只在重灌时生效）。human D2 裁：本次一并回滚、不重灌，重建等真正需要补 P2 以前历史时再做。**重建前置件**：per-session 过滤 + 主键改 `(session_id, seq)` + role 含 toolResult + created_at 保留 + 脚本入仓（否则不可复现）。**transcript 源形状漂移**（生产表 `(session_id, seq, event_json, created_at)` vs 工具/验收契约 `id/role/content` 直读）随本条一并承载，K4b 修复轮不修（taskpackage notes ⑥）。触发位 = 下次要补 transcript 历史时；届时若牵动 spec 语义走 `/arch-change`。

**运维清理 append(2026-10-24,主线程,K4 重灌后)——「自我无知」毒结论清除**:测试轮的「agent-kidpal 没有王一诺/方可心/王艺诺 的信息」自述结论(5 条,observer=observed=agent-kidpal)被注入检索命中,诱发模型持续答「不知道」(自我实现遗忘)。按 NFR-PRIV-1 运维面手工删除(precise id ×5)。**根因类**:聊天回复全量入画像的残余(批 C 申报在册)+ deriver 对 agent 自述无知照实抽取 = 自我毒化环。候选机制修复(未做):deriver 跳过「无信息」类自述,或注入层过滤 agent-ignorance 形态结论——登记待议。

**遗留队列 append（2026-10-24，human 裁「暂时不做」，入册防丢）**:① P1 稳定事实卡（peerCard 人工 10-15 条必注入——无窗口限制、每轮注入，唯一不受 recent-100 影响的被动通道;现空）;② P2 scope 只读 A/B（`scope_backfill.py` 零 LLM 复制 344 条,但 recent-100 仍裁、丢 agent 视角）;③ 可靠性债:摘要 0 落库（DeepSeek 超时 687 次 → `_basic_fallback_summary` 降级不写库,140 任务 processed 0 error 0 落库）/ omni 梦 0 派生（需受控复跑定性）/ backfill 覆盖 80.4%（31 条 EmptyRepresentation + 2 条 embedding 失败）/ deriver 输出截断残量;④ 回灌工具未来修:create_session 成员配置补 `observe_others=true`（观察者扇出前提）+ Gate E;⑤ 线上 diag 热补丁（插件 context.js 打点,`.bak-diag` 备份,非仓内资产）——建议下批定夺「入仓成正式日志开关或还原」，防插件升级静默丢失。权威分析 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-recall-architecture-20261024.md。
