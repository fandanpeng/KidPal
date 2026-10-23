---
workstream: device-memory-overhaul
workType: [new-capability, remediation, refactor]
status: frozen
opened: 2026-10-22
upstreamRefs: ["docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md", "docs/debt/tech/TECH-261018-schedule-read-compliance-residual.md", "docs/debt/tech/TECH-261022-openclaw-prep-tax-memory-pressure.md", "delivery/intake/261022-device-memory-prompt-overhaul.md"]
---

# 设备语音链路「越用越聪明」整体重构（持久 session + Honcho 记忆 + prompt 迁 AGENTS.md + 工具收窄 + 课表机械注入）

> **状态：FROZEN v1.0 · 2026-10-22**（FROZEN 凭据：human 触点①确认 ③拆包 + architect 分流 review 回执存档 [review.md](review.md) + 契约四分类闭合 + human 触点②盖 FROZEN。作者≠判官。冻结后本件不改。）

## 1. 要达成什么（业务目的清单）

**【硬项 · 枚举式】**

| # | 业务目的 | 可观察判据 | workType | 来源 |
|---|---|---|---|---|
| 1 | 每轮 prompt 瘦身、回合更快更省 | 单轮 DeepSeek `prompt_tokens` 从 22,116 降到 ≤11,000（≥50%），cache hit 覆盖稳定前缀（v11 迁 AGENTS.md 后目标翻倍量级） | refactor | `TECH-261022-device-memory-prompt-overhaul` 证据 2 |
| 2 | 小伴跨会话记得孩子的事实 | 新 WS、新会话问「我最喜欢的颜色 / 我的小狗叫什么」答对（全新 user id 对照答不出） | new-capability | **待需求面立 story**（本批 /requirement-change 档 2 产出，预期 F5 或 S-F4.2；story 编号落定后回填本表） |
| 3 | 家长的隐私边界不被记忆穿透 | 注入家长姓名/电话/家庭安排后，画像与答复零命中；隐私负向探针 100% 不回忆 | new-capability | **待需求面立 story AC + NFR-PRIV-1**（同上） |
| 4 | 课表回答不再有编造长尾 | 受控 ≥20 轮提问零工具调用也不编造，答案与 `kidpal/schedule.md` 逐日一致 | remediation | `TECH-261018-schedule-read-compliance-residual`（本列车的 P5 落地即消费该条） |
| 5 | 工具面收窄、系统提示噪声消除 | kidpal agent 可见工具 = 4 项（read/write/exec/web_search），Tooling 章收缩、57KB schema 消失 | refactor | `TECH-261022-device-memory-prompt-overhaul` 证据 3 |
| 6 | 资源与固定税可解释 | prep 税/内存压力复测有 2C/4G 基线读数，无 event_loop_delay 38s 级退化 | remediation | `TECH-261022-openclaw-prep-tax-memory-pressure` |

**【硬项 · 起草侧自检两查】**

- [x] 清单引用的 story 编号无一命中已砍清单——目的 2/3 的 story **尚不存在**（本列车需求面产出，标记待回填，不存在误引）；目的 1/4/5/6 引 debt 条目非 story。
- [x] 引用 story 的可行性三态无 🔴——同上，无既有 story 引用。

## 2. 现在在哪（混合型：并列三节）

### 现状与差距（新建型）

- **已就绪**：Honcho 自托管栈已在 ECS 运行（/opt/honcho，127.0.0.1:18100，五容器全绿）；探针 profile（/root/.openclaw-probe）端到端验证通过——openai-http 路径钩子触发、deriver 以 deepseek-v4-flash 抽 observation、representation 产出结构化事实、before_prompt_build 跨 session 注入生效（零历史新会话答出已记录事实）。
- **可复用**：v11 prompt 版本化 artifact 与三面接线（carrier/provider.config/overlay）；261017 skills 定点 patch 先例（声明+apply+drift+backup+rollback）；261017 spine 的 faceMap/facePlan 体例；dashscope key（ASR 同源）。
- **差距**：记忆未接入设备主链路（探针 profile 隔离验证，生产 profile 未装插件）；引擎请求无 `user` 字段（session 每轮随机）；prompt 承载仍在引擎 `.config.yaml`（v11 每轮随 extraSystemPrompt 挂缓存边界之下，不进缓存）；工具面 34 项未收窄；课表机械注入未建；历史语料未 backfill。

### 问题与根因（整改型）

- **跨会话事实答不出**：openai-http 不带 `user` 字段 → 每请求 `openai:<randomUUID>` 新 session（部署版 bundle `resolveSessionKey` 逐字实证）；v11 孩子对话回合禁工具 + active-memory 不覆盖 one-shot → 记忆读路径三重封死。孩子 9-30 讲过的「王一诺」2026-10-22 问起答不出。
- **课表编造长尾**：探针复现模型称「先看课表」但零工具调用、直接编造（v11 后真机仍漏读，TECH-261018 在案）——纯 prompt 纪律不可靠。
- **v11 隐私守则过度泛化**：实测拒记同学真名/宠物名（「家庭小私事不特别记下来」），与记忆目标直接冲突。

### 结构性痛点（重构型）

- **prompt 承载拓扑**：v11 作为 extraSystemPrompt 挂缓存边界之下（参考 `openclaw-main/src/agents/system-prompt.ts:1297,1333-1336`），不可缓存且与 workspace AGENTS.md（1.7k）内容重复——承载反转到 AGENTS.md 后进入稳定前缀，去重 + 缓存命中翻倍量级。
- **工具面**：全局 `tools.profile="coding"` 对设备 agent 意味着 34 工具 57KB schema/轮——设备只需 4 项。
- **记忆飞轮**：OpenClaw 原生记忆 deep 阶段「Ranked N, Promoted 0」（检索信号饿死）、memory_search 搜「王一诺」命中「王水」——原生链路在设备调用形态下不闭环，改接 Honcho 专用画像服务（探针已验证）。

## 3. 怎么走（方案空间 + 裁定）

### 3.1 方案空间与裁定

**【硬项 · 每条裁定带 spec 关系标注】**

| 裁定 | 内容 | spec 关系 | 说明 |
|---|---|---|---|
| R1 | 引擎请求加 `user=<device_id>` + 只发新回合 → OpenClaw 服务端持久会话 | `需修订` | 推翻 architecture.md:377「stateless 不续、每轮透传历史」已定案——本列车架构面 /arch-change 承载修订（Track B），执行语义 = P2 包 launchPreconditions |
| R2 | v11 静态语义迁入 workspace-kidpal/AGENTS.md（主承载），引擎只发动态 context 小块 | `需修订` | 改 architecture.md:299-304 §5.5.5 承载声明与 AGENTS.md:15「次要承载」自声明——架构面承载；P1 launchPreconditions |
| R3 | 引入 Honcho 自托管记忆子系统（外部依赖） | `需修订` | architecture.md:32-41 §1.2 外部依赖清单增项 + 记忆子系统拓扑——架构面承载；P3 launchPreconditions |
| R4 | NFR-SEC-2 增第 ⑤ 条出网认账（deriver→DeepSeek 维持、embedding→DashScope 接受） | `需修订` | requirements.md:627-631——需求面档 2 承载（human 反问 DashScope 归属已澄清=阿里云，见 §3.2 OQ-2）；P3/P4 launchPreconditions |
| R5 | 新记忆 story（同会话/跨会话回忆 + 隐私负向 AC） | `需修订` | requirements.md 全文无记忆语义——需求面档 2 立新 F5 或 S-F4.2；P3 launchPreconditions |
| R6 | backfill 第三方姓名照记（不带 PII 过滤器） | `spec-silent` | human 2026-10-22 触点①裁；保留幂等/回滚/计数报告；语义是否入 NFR-PRIV-1 由需求面收口定 |
| R7 | front 一次性回合（整理/引导）不入记忆画像 | `spec-silent` | human 2026-10-22 触点①裁；P3 须实测隔离（front_app.py:170-186 无 user 字段）并落 AC |
| R8 | 课表机械注入走 schedule.py 单源，注入位置 = P1 动态 context 块 | `within-spec` | contract §1.5:184-186 与 architecture.md:197 已钉唯一解析器；本裁定只选实现路径 |
| R9 | 引擎 Intent 维持 nointent、thinking 维持关闭、分回合 thinking 不纳入 | `within-spec` | human 2026-10-22 裁；ARQ-260907 根因在案 |

### 3.2 human 取舍点（OQ）与裁定

- **OQ-1 路由**：③拆包 vs ②单包 Full → **human 2026-10-22 确认 ③拆包**（architect 裁决未改判）。
- **OQ-2 出网认账**：human 反问「Dashscope 是厂家的 embedding 模型，在哪里部署的，是不是可以考虑 aliyun 的 embedding 模型，我们的 ASR 也用的 aliyun」→ 主线程 L1 澄清：DashScope 即阿里云百炼，text-embedding-v4 就是阿里云 embedding 模型（与 ASR 同平台）；暴露面 = embedding→阿里云（与 ASR 同信任域，ASR 已送原始语音）+ deriver→DeepSeek（非新增暴露，主对话本就全量发 DeepSeek）→ **human 无再异议，按接受处理**；全供应商收敛备选（deriver 改 DashScope qwen 系）留架构面裁。
- **OQ-3 backfill 隐私**：照记（human 裁，见 R6）。
- **OQ-4 一次性回合**：不入画像（human 裁，见 R7）。
- **OQ-5 记忆 story 落位 / NFR-PRIV-1 形态**：需求面终拍项（开放）。
- **OQ-6 记忆保留/备份/删除语义**：需求面裁（契约 §四:272 段 story 删除不触对话记忆，本列车须另钉）（开放）。
- **OQ-7 多设备/多孩子 peer 映射**：先 device 级 peer，多设备身份体系不做（架构面留根）（开放）。
- **OQ-8 prompt 版本化新形态**：v12 双件 vs `server/deploy/prompts/` 单一事实源（架构面裁，影响 carrier 与 ADR-001 不变式重定义）（开放）。

### 3.3 非目标（防 scope creep）

- 引擎 `Intent` 维持 nointent（ARQ-260907 不改）；thinking 维持关闭；分回合 thinking 不纳入本批（human 2026-10-22 裁）。
- 不做 OpenClaw 替换/自研记忆层；不做多设备身份体系（先设备级 peer）。
- 不触碰 display E1 / B1 课表 MD / §三提醒 / §四私有消息帧的线格式。
- 不修改历史 prompt 版本文件（v1–v11 不改、可 diff）。
- 分回合 thinking、front 60s 重试等体验兜底归各自既有线，不入本列车。

### 3.4 逃生门申报

实现期发现与 spec / 契约冲突时走 `/arch-change` 判级 + human 冻结；契约面走 architect-reviewer 四分类后随批原子落，不在 Pod 内自裁。已申报疑似项两处（§1.6:189-194、§四:283）已按 facePlan 先派四分类，结论见 spine anchors.contract。

## 4. 拆包预判（意图提示，非权威）

| 切片 | 内容 | riskTier / rigor / sourceType | 依据 |
|---|---|---|---|
| P1 上下文装配与 prompt 承载迁移 | v11 静态语义迁 AGENTS.md（吸收旧文件底线句）、引擎只发动态 context 小块+本回合、tools 覆写 allow=[read,write,exec,web_search]、SOUL/USER 清理；carrier/provider.config/overlay/manifest 四处同批改「AGENTS.md 主承载」新不变式 | medium / full / proposal | 跨 6 部署文件+引擎补丁+生产行为回归面；host 齿可闭环 |
| P2 持久会话 | 引擎请求加 `user=<device_id>`（engine-patch 新 provider 补丁 + bind-mount）+ payload 只留本回合；补丁漂移护栏同批 | high / full / proposal | ARQ-260907-engine-deploy-patch-drift 同族 |
| P3 Honcho 生产化 + 守则 v12 | Honcho 栈 loopback 常驻、生产 profile 插件+hooks 授权、v12 守则（孩子事实必须记/家长隐私不记）、front 一次性回合隔离实测 | high / full / proposal | 隐私/安全轴+新 DB+新出网；gateProfile=["security","real-db","evidence-integrity","ai-cost-audit"] |
| P4 历史 corpus backfill | session-corpus 灌 Honcho（照记不滤姓名）、幂等可重跑、前后计数报告、清库回滚 | high（过滤/回滚设计未收敛升 critical）/ full | migration gate |
| P5 课表机械注入 | 注入源走 schedule.py 单源，落 P1 动态 context 块内；消费 TECH-261018 | medium / lite / regression | host 齿可闭环（server pytest + loopback 探针） |

**merge order 建议**：P1 → P2 → P3 → P4，P5 插 P1 之后（可与 P2 换序；共享 `connection.py`/manifest，不建议并行）。**单 Pod 串行**——拆包价值在独立验收/回滚边界，不在并行。共享文件面：`manifest.yaml`、`provider.config.yaml`、`apply-xiaozhi-overlay.sh`、`OPENCLAW_SYSTEM_PROMPT_CARRIER`、`engine-patch/connection.py`、`workspace-kidpal/AGENTS.md`。

**launch 前置（死亡顺序禁令）**：需求面 frozen + 架构面变更记录 changeId 行在场 + 契约四分类闭合，先于实现包 launch。

## 5. workstream 收口标准（跨包并集验收）

- 体积：单轮 prompt_tokens ≤11,000（基线 22,116）；cache hit 覆盖稳定前缀（目标翻倍量级）。
- 记忆：同一 device id 跨 WS/跨会话回忆命中；负向：家长 PII 零命中（seeded probe）。
- 正确性：课表回合 ≥20 轮受控零编造；七类回合行为 parity 对 v11 基线。
- 隔离：front 一次性整理/引导回合不产生用户画像 observation（日志/DB 双证）。
- 出网：NFR-SEC-2 ⑤ 记账在场；Honcho/Postgres/Redis 端口全部 127.0.0.1。
- 资源：prep 税与内存读数有前后对照，event_loop_delay 无 38s 级退化。
- 部署：manifest 覆盖全部新资产，verify-deploy.sh 绿，drift timer 绿。
- 收口轮 owner 包 = P5（最后一个落地的行为切片）+ P4 计数报告对账。

## 6. 留根清单

- 多设备/多孩子 peer 映射（OQ-7）——设备级 peer 先行，身份体系另立。
- 持久 session 历史增长与 compaction 成本——P2 上线后观测再钉 NFR-PERF 观察项。
- deriver/dialectic 供应商收敛（DashScope qwen 系备选）——架构面裁。
- 记忆保留/备份/加密/删除——需求面 NFR-PRIV-1（OQ-6）。
- 分回合 thinking（提醒 cron/整理回合开 thinking 换准确率）——human 裁不入本批，留根。
- TECH-261022-openclaw-prep-tax-memory-pressure 复测读数——P3 后回填该 debt。

## 7. 评审记录

- 2026-10-22 architect 四面探测 + 比例化 review：findings + sign-off 全量存档 [review.md](review.md)。
- 2026-10-22 human 触点①：确认 ③拆包；OQ-3/OQ-4 同场裁定；OQ-2 经主线程 L1 答疑收敛（详见 §3.2）。
- 2026-10-22 主线程落盘本件（DRAFT v1.0，freezeState pending-human）——待 human 触点②盖 FROZEN。
- 2026-10-22 契约四分类（architect-reviewer，独立裁定，回执存 [review.md](review.md) 同日追加）：处 A §1.6:189-194 = `NO_CONTRACT_CHANGE`（附非必需随批措辞对账建议，沿 2026-10-19 §1.4 先例）；处 B §四:283 = `ACCOMPANIES_SPEC_CHANGE`——与分流预判不一致处已独立证成：该句是契约内「以部署期首验定案」的待定案记录条款，本批批准动作直接闭合之（user 标识持久会话系两选项外第三形态），且 arch:377 定案后契约从未记录、既有文本级欠账，同批原子落是唯一不产生新漂移的窗口。契约包三节草案要点（proposedContractDiff / impact / validationPlan）已在回执，**带入架构面 /arch-change PROPOSAL 起草输入随批原子落**；实现包 launch 闸维持（架构批契约包落地为准）。
