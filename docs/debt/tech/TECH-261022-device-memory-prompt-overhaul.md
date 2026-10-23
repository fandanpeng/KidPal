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
