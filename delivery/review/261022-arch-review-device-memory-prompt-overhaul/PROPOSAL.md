# PROPOSAL · 261022-arch-review-device-memory-prompt-overhaul

> **状态：FROZEN v1.1 · 2026-10-22**（凭据 = human 触点① 立项 ack + R2 靶向复核 GREEN + 主线程标冻。R1 BLOCK(F1,F2,F3) → v1.1 修毕 → R2 GREEN。修订摘要：R1-F1 = §5 第 6 条「部署期首验」计数按落地实况校正为 **2**（两处均为历史引述：§四 :283 替换句 + 变更记录条；承载行 operative 语义不再含该词），取 (a) 案；R1-F2 = 风险 9 闭账句尾补「决策 21 所列结构级备选（方案二）随 ADR-015 承载定案失效」；R1-F3 = §6.1 补既有 `server/deploy/kidpal-agent-prompt.txt` 的去留 / 改写归 P1 定形、P1 不得使两个动态模板件并存（取代或原地改写，二选一）；另清理 §7 重复标题与占位行（R1 记录逐字保留、内容零改）。v1.0 输入说明：输入 = spine `delivery/intake/261022-device-memory-prompt-overhaul.md`（facePlan 顺序 2；需求面 anchors 已回填：requirements v1.67 frozen）+ workstream `delivery/workstreams/device-memory-overhaul/PROPOSAL.md`（FROZEN v1.0）+ 同目录 `review.md`（契约四分类回执，spine OQ-5 已闭合）+ 主触发 debt `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md`（七组实测证据）。**本件写入 ≠ 冻结；canonical 文件（architecture / contracts / ADR / glossary / 部署资产）本会话零触碰。**
> **承载 ARQ**：主触发 `TECH-261022-device-memory-prompt-overhaul`（首次消费；条目收口 = 实现批落地后主线程登记动作，非本批 diff）；本批消费 `TECH-261018-schedule-read-compliance-residual`（P5 机械注入落地即结项）；同批复测 `TECH-261022-openclaw-prep-tax-memory-pressure`；只引 `ARQ-260907-openclaw-tools-rejection-rootcause`（Intent=nointent 不改）。`ARQ-260902-multiturn-continuity` **已于 2026-10-12 关闭**（条目现居 `docs/debt/tech/_archive/2026/`，status=closed）——本批只清契约 §四 :283 的字面残留引用，不重复收口。
> **review 轨**：Track B 常驻轻路径的**主路径**（SSOT：`.claude/references/arch-change.md`）。起草侧判档预判 = 主路径：轻路径判据④「外部依赖增删」直接否（§1.2 新增 Honcho 自托管栈与 DashScope embedding 两项）；最终判档归 architect-reviewer。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（新增自托管子系统 + 会话模型与 prompt 承载两处反转 + 契约一处定案改写；全部为文档与声明面改动、无运行时代码，回滚 = 声明面回滚 + 契约行还原）。
> **review profile**：架构面 + 契约面（触契约分支，契约包三节随批原子落）；G1/G2/G3 治理面 = 零（不触 `.claude/**`）。

```landing-manifest
docs/architecture.md
docs/contracts/interface-contract.md
docs/decisions/ADR.md
docs/glossary.md
server/deploy/manifest.yaml
server/deploy/openclaw-agents/README.md
server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把「设备语音链路越用越聪明」在架构上落成四根承重——**持久会话**（`user` 标识 + 服务端会话）、**自托管记忆画像**（Honcho）、**静态语义缓存化**（v11 语义迁 workspace `AGENTS.md`）、**课表机械注入**（front 单源派生文本进动态 context）——使 requirements v1.67 F5「小伴的记忆」五条 AC 与 NFR-PRIV-1 / NFR-SEC-2 ⑤ 有明确的架构承载与验收挂点。

**业务目的清单不复制**：workstream PROPOSAL §1（FROZEN v1.0）的六条业务目的与可观察判据即本批业务权威；本件只回答「怎么造」与「改哪些 spec 面」。

**现状差距**（四根承重逐一对照现状，证据见 §2 对应 T）：

1. **会话**：引擎每请求不带 `user`，OpenClaw 侧 `resolveSessionKey` 的判据为「有 `user` 走 user 键、无 `user` 走随机 UUID」（原文见 T2 证据围栏）——每轮新 session；引擎每轮透传全部历史（架构 §8.2 :377 已定案），跨会话事实无法累积（debt 证据 1）。
2. **记忆**：OpenClaw 原生记忆在设备调用形态下空转（deep 阶段恒「Ranked N, Promoted 0」、`memory search` 对「王一诺」命中「王水」）；架构 §1.2 外部依赖清单与 §5.1.2 组件表均无记忆子系统位置（debt 证据 6）。
3. **prompt 承载**：v11 章节作为引擎注入的 system prompt 落在 OpenClaw 缓存边界之下（参考实现 `extraSystemPrompt` 段）；单轮 prompt 实测 22,116 tokens、cache miss 14,948（debt 证据 2）；workspace `AGENTS.md` 自声明为「次要承载」（:15），与「主承载」目标相反。
4. **课表正确性**：课表回合「该读未读、凭印象编造」在生产与探针均可复现（debt 证据 4；TECH-261018 在案）；纯 prompt 纪律不可靠，缺机械注入。

**非目标（本批不做，防 scope creep）**：业务侧非目标引 workstream PROPOSAL §3.3（Intent/thinking 不动、不做 OpenClaw 替换、不触 display E1 / B1 / §三提醒 / §四私有消息帧线格式、不改历史 prompt 版本文件、分回合 thinking 不入批）。本批专属非目标：

- **不动需求面**（requirements v1.67 是上游权威，本批零需求 diff）；
- **不动实现面代码**（P1–P5 的 changeManifest 归 workstream / portfolio-planner；本批只改 spec、ADR、glossary、契约，以及三处静态承载声明口径）；
- **不出 v12 同义副本、不新建 `server/deploy/prompts/` 平行目录**（OQ-8 裁定，见 T3）；
- 不做多设备 / 多孩子身份体系（设备级 peer 先行，T1 / T4）；
- 不在本批收口 debt 条目（主线程登记动作）。

---

## 2. 问题、根因与改法（T1–T7）

> **起草期实证留痕（2026-10-22 本会话现读 / 现跑）**：`docs/architecture.md` :32-43 / :62 / :77-80 / :114 / :150 / :187 / :196-198 / :211 / :239 / :272 / :299-301 / :357-371 / :377 / :402 / :417 / :419 / :436-446 逐段现读；`docs/contracts/interface-contract.md` :178-194 / :268-284 现读；`docs/decisions/ADR.md` 全文现读（尾部 = ADR-013，下一号 = ADR-014）；`docs/glossary.md` :12 / :31 现读；`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` :13-15、`server/deploy/openclaw-agents/README.md` :20-22、`server/deploy/manifest.yaml` :106-111 现读；`server/gateway/front/front_app.py` :169-186、`server/gateway/provider.py` :97-108、参考引擎 `…/05_XiaoZhiServer/…/providers/llm/openai/openai.py` :92-108 现读；OpenClaw 参考实现 `src/agents/system-prompt.ts` :71-81 / :1267-1283 / :1287-1296 / :1333-1336 与 `docs/concepts/{memory-honcho,session,compaction,active-memory}.md` 现读。基线机检与 census 输出见 §5「起草期实跑记录」。
>
> **体例先例（已读先例文件再落笔）**：「取代注记」体例照 `docs/architecture.md` §10 决策 25 的既有形态（「⚠️ **已被 ADR-013 取代（…）**：…」）——本批的会话模型反转对 :377 / 决策 21 连带 / 风险 9 三处采用同型注记，保留原文、追加取代句，不改写历史结论；契约定案改写照 :283-:284 既有行文粒度（只钉形态事实、不钉实现细节）。
>
> **【硬项 · 起草期实证义务】**本节断言（file:line / 数字 / 命令 / 先例形态）均于 2026-10-22 本会话现读现跑；转引 debt 数字注出处；未实测项已显式标注（T4 抽取策略的 deriver 提示可配置性「未验」、T7 ADR 为「要点级」）。
>
> **【硬项 · 目标文件锚清单】**每个 T 末尾均有「目标文件：」裸路径行；§2 锚并集 ≡ landing-manifest 七文件（preflight 实测 `anchorCount:7` 已核）。

**每条改法与 spec 的关系标注**：

| T | spec 关系 | 落点 | 上游依据 |
|---|---|---|---|
| T1 | **需修订**（外部依赖增项 + 新子系统落位） | arch §1.1 / §1.2 / §1.3 / §3.1 / §4 / §5.1.2 / §5.3.2 / §5.3.3 / §5.4 / §8.1 / §10 决策 19 | requirements NFR-SEC-2 ⑤ / NFR-PRIV-1 |
| T2 | **需修订**（推翻 §8.2 已定案） | arch §2.2 / §5.2② / §5.4 / §8.1 / §8.2 / §10 决策 21 | requirements S-F5.1 AC1·AC2；ADR-015 |
| T3 | **需修订**（承载拓扑反转 + ADR-001 重定义） | arch §5.5.5；三处部署声明口径 | debt 证据 2 / 5；ADR-001 / ADR-014 |
| T4 | **需新增**（新横切机制） | arch §6.7；§5.1.2 front 行 | requirements S-F5.1 AC3·AC4·AC5 / NFR-PRIV-1 |
| T5 | **within-spec 机制选择**（单源红线内选路径） | arch §5.2② / §5.1.2 引擎行 / §5.1.3 | 契约 §1.5；debt 证据 4；TECH-261018 |
| T6 | **ACCOMPANIES_SPEC_CHANGE**（契约随批原子落） | 契约 §1.6 / §四 | OQ-5 四分类回执 |
| T7 | **新增记载**（ADR + 术语） | `docs/decisions/ADR.md` / `docs/glossary.md` | 架构文档头部纪律 / glossary 使用纪律 |

### T1 · 外部依赖、部署与数据视图：Honcho 记忆子系统落位

**问题**：设备链路要「越用越聪明」，但现行架构没有记忆子系统的位置——§1.2 外部依赖清单（:32-43）只有 ASR / DeepSeek / edge-tts / Tavily / 字库 / QQ / ECS 七项，§5.1.2 服务端组件表无记忆组件，§5.3.2 云端拓扑无记忆承载，§5.4 数据视图无画像 / 消息存储行；同时 NFR-SEC-2 已随需求批升为五条出网路径（requirements v1.67 :648），架构 §3.1 :114 与 §4 :150 仍写「四条」。不补则实现包没有落位承载、出网记账在架构面缺投影。

**证据**：
- `docs/architecture.md:34-43` 外部依赖表现核：DeepSeek 行「出网路径②」、Tavily 行「出网路径④」；无记忆相关行。`grep -c "Honcho" docs/architecture.md` = **0**（2026-10-22 实跑）。
- `docs/architecture.md:114` 现文「**儿童数据出网**：四条路径（音频→阿里云 / 转写→DeepSeek / 回复→edge-tts / 实时事实查询→Tavily 联网搜索）human 已记账接受」；`:150` 现文「NFR-SEC-2 出网记账 | 四条出网路径…」。`grep -c "四条路径\|四条出网路径" docs/architecture.md` = **2**（实跑）。
- `docs/architecture.md:239-245` §5.3.2 云端拓扑四行（front / 引擎 / 编排大脑 / 交接目录 / 配置密钥），无记忆行。
- `docs/architecture.md:260-272` §5.4 数据视图十二行，无记忆画像 / 消息存储行。
- 需求上游：`docs/requirements.md:648`（NFR-SEC-2 ⑤：deriver→DeepSeek + embedding→DashScope，human 2026-10-22 裁接受）、`:654`（NFR-PRIV-1：边界 / 保留 / 删除三阈值）；`docs/requirements.md:602` S-F5.1 锚「支撑 = arch §5.1（服务端组件，云）、§1.2（外部依赖清单）——记忆子系统落位随架构文档修订后对账」。
- 探针证据（debt 证据 7，逐字）：Honcho 自托管于 ECS `/opt/honcho`、`127.0.0.1:18100`、五容器；`before_prompt_build` 注入生效、全新 user 字段零历史会话答出已记录事实。

**根因**：记忆是新增子系统，其外部依赖、部署承载与数据落点必须先在架构面显式记账（出网、资源、数据边界三件），否则实现包会各自发明落位并绕过 NFR-SEC-2 / NFR-PRIV-1 的记账义务。出网条目为 NFR-SEC-2 ⑤ 的架构投影；隐私边界语义的权威在需求面，本批只钉落位与生命周期。

**改法**（逐节，落地文本按围栏逐字）：

1. **§1.1 系统上下文图 · 外部服务行**：补 Honcho 与 DashScope embedding。

```text
外部服务（设备/云端经其完成能力）：
   阿里云 ASR（Qwen3-ASR-Flash）· 阿里云 DashScope embedding（text-embedding-v4，记忆画像）· edge-tts（微软非官方接口）· DeepSeek（OpenClaw 内；含 Honcho deriver 抽取）
   makemeahanzi 字库（静态数据）· 小智官方服务端镜像（xinnan-tech，容器运行时）· Tavily（OpenClaw 平台联网搜索，实时事实查询）· Honcho 自托管记忆子系统（ECS 本机五容器）
```

2. **§1.2 外部依赖清单**：DeepSeek 行风险归属列补 deriver 同域记账；其后新增两行（表内行，逐字）：

```text
| 阿里云 DashScope embedding（text-embedding-v4） | 记忆子系统容器内 provider，凭据沿用 `DASHSCOPE_API_KEY` | 按量计费；额度尽 / 不达标 → 画像管线降级（不阻塞对话） | 出网路径⑤（NFR-SEC-2 ⑤ 记账；与 ASR 同供应商同信任域） |
| Honcho 自托管记忆服务栈 | Docker 容器（五容器；api `127.0.0.1:18100` loopback；postgres + pgvector 持久卷） | 与 ECS 同机共生；故障 → 画像注入缺失、对话照常（§6.7 降级） | 部署面运维（§5.3.2）；数据本机不出网（NFR-PRIV-1） |
```

  DeepSeek 行风险归属列原「出网路径②」逐字替换为「出网路径②（Honcho deriver 抽取复用同域，NFR-SEC-2 ⑤）」。

3. **§1.3 范围声明**：追加一条（逐字）：

```text
- 多设备 / 多孩子**记忆身份体系**——本批**设备级 peer** 先行（换设备 = 新 peer、旧画像不迁移）；身份体系另立（§6.7 / OQ-6 留根）。
```

4. **§3.1 质量属性目标**：出网条目由四条改五条（逐字替换整条）：

```text
- **儿童数据出网**：五条路径（音频→阿里云 / 转写→DeepSeek / 回复→edge-tts / 实时事实查询→Tavily 联网搜索 / 记忆画像管线→deriver 复用 DeepSeek + embedding→DashScope）human 已记账接受（NFR-SEC-2；第④条 v1.62 增、第⑤条 v1.67 增）；
```

5. **§4 质量属性 → 架构对策映射**：NFR-SEC-2 行逐字替换为五条，并在其后新增 NFR-PRIV-1 行：

```text
| NFR-SEC-2 出网记账 | 五条出网路径（§1.2 表；第④条 = 实时事实查询回合经 Tavily 联网搜索，v1.62 增；第⑤条 = 记忆画像管线 deriver→DeepSeek / embedding→DashScope，v1.67 增）+ 决策 19 记账 | §1.2 |
| NFR-PRIV-1 记忆边界与保留 | 记忆写入资格门（白名单入册）+ 抽取三层纵深（来源门 / 抽取策略 / 注入过滤）+ 自托管不出网 + 删除 = 运维手工 | §6.7 / §5.1.2 记忆子系统 / §5.4 记忆行 |
```

6. **§5.1.2 服务端组件表**：OpenClaw 本体行逐字替换（记忆面 + 承载指针），并在其前插入记忆子系统行：

```text
| **记忆子系统（Honcho 自托管侧车）** | `server/deploy/honcho/**`（compose + 配置 + apply/rollback + 健康检查） | peer / session / message 存储、deriver 抽 observation、representation 生成、`before_prompt_build` 画像注入；五容器全部绑定 127.0.0.1（api 18100）；写入受 §6.7 资格门约束；与引擎 / OpenClaw 同机共生、数据本机不出网 |
| OpenClaw 本体 | ECS 部署（2026.8.1） | 完整 agent（意图 / 多轮 / 工具 / QQ 录入）；**对话历史 = 服务端持久会话**（按 `user` 标识，ADR-015）；**记忆画像经 Honcho 插件**（§6.7）；**静态语义承载 = workspace `AGENTS.md`**（§5.5.5）；原生 cron（提醒 15min 重算 + 录入即时触发） |
```

7. **§5.3.2 云端拓扑**：在「编排大脑」行后新增一行（逐字）：

```text
| 记忆子系统 | Honcho 自托管栈（`server/deploy/honcho/**` 声明 → ECS `/opt/honcho` compose；五容器） | api `127.0.0.1:18100` 全 loopback（端口探针纳入 §5.3.3 部署后核验）；postgres + pgvector 落本机持久卷；资源基线 = ECS 2C/4G（2026-10-22 升级后基线；prep 税 / 内存压力 P3 后复测，TECH-261022-openclaw-prep-tax-memory-pressure） |
```

8. **§5.4 数据视图**：表末追加两行（逐字）：

```text
| 记忆·会话消息（peer / session / message） | Honcho postgres（ECS 本机；pgvector 向量列） | OpenClaw Honcho 插件（每轮 persist，受 §6.7 资格门约束） | 持久；删除 = 运维手工（NFR-PRIV-1） | 云端持久 | Honcho 内部 schema（非契约） |
| 记忆·画像（observation / representation） | 同上 | Honcho deriver（LLM 抽取）→ representation | 持久、随抽取滚动更新；删除同上 | 云端持久 | 语义权威 = requirements NFR-PRIV-1；形状非契约 |
```

9. **§8.1 风险表**：追加风险行（逐字；编号顺延，现表已有重复编号 8 的历史问题不属本批）：

```text
| 13 | 记忆子系统单点（Honcho 五容器与 ECS 同机；画像 / 消息库在本机 postgres） | 故障 = 无记忆（对话不阻塞）；数据损坏 = 画像需自消息库 / backfill 重建；删除 = 运维手工 | 降级语义 §6.7；备份 / 恢复形态 = OQ-10（待 human）；数据不出网（NFR-PRIV-1） | 运维面 |
```

10. **§10 决策 19**：原文①–④不动，追加第⑤条投影记账（逐字）：

```text
**出网路径⑤ 记忆画像管线（human 2026-10-22 裁，v1.67 增补注记，原文①–④不动）**：Honcho deriver 抽 observation → DeepSeek 云（与 ② 同域，非新增暴露面）+ embedding 向量化 → DashScope（阿里云 text-embedding-v4，与 ① ASR 同供应商同信任域），human 已知晓并接受；权威 = requirements NFR-SEC-2 ⑤，本条为投影记账。
```

11. **§5.3.3 运维面 · 部署后核验三件**：第③条端口探针句追加 Honcho 端口（逐字追加于该条末）：

```text
；另核记忆子系统 api 18100 仅本机可达、五容器其余端口公网不可达
```

目标文件：`docs/architecture.md`（改）

### T2 · 会话模型反转：`user` 标识 + 服务端持久会话 + 双历史单一权威

**问题**：架构 §8.2 开放问题 1（:377）已定案「stateless 不续、**每轮透传历史**续」；但该定案在设备语音链路上同时造成三件事——跨会话无连续性（每请求随机 session，孩子 9-30 说过的「王一诺」今天答不出）、每轮 prompt 随历史线性膨胀、上下文的家只有引擎 RAM（跨 UI 会话因常驻连接被动累积，决策 21 连带定性「feature 或 bug 待 human 拍」）。本批反转承载形态，必须同时回答**双历史关系**（引擎侧 RAM Dialogue 与 OpenClaw 持久会话并存时谁是权威、谁可重放），否则 P2 实现会造两源漂移。

**证据**：
- `docs/architecture.md:79` 现文「│  意图/多轮对话（每轮透传历史）/ 业务 MD 读写 / QQ 录入」；`:211` 现文「→ OpenClaw（每轮透传历史）流式回复 →」；`:272` 现文「| 对话历史（剥离前原文） | 引擎容器内存 | 引擎 | 连接期（跨 UI 会话累积——决策 21 连带，feature/bug 待 human 拍） | 掉电即失 | — |」；`:377` 现文「~~多轮会话连续性~~ **已定案（2026-09-11 V21 探针实证）**：stateless 不续、**每轮透传历史**续——承载方案关闭；剩余相位矛盾见 ARQ-260902-multiturn-continuity（session_id 语义与协议剖面承载定义随契约门 §四）。」；`:368` 风险 9 现文「对话历史跨 UI 会话累积（决策 21 连带） | 上下文污染 or feature，未定 | human 拍板；结构级备选（方案二…）留 Phase 1 | human」；`:419` 决策 21 尾句现文「已知连带：引擎侧对话上下文将跨 UI 会话累积（当 feature 或当 bug 待 human 拍）；结构级备选…」。
- 引擎 payload 逐字（参考引擎 `openai.py:92-108`）：`def response(self, session_id, dialogue, **kwargs)` → `request_params = {"model": …, "messages": dialogue, "stream": True}`——`dialogue` 由 `connection.py:1401/1409` 的 `self.dialogue.get_llm_dialogue_with_memory(...)` 全量给出；payload **无 `user` 字段**。
- OpenClaw 侧 session 键逐字（debt 证据 1，部署版 bundle `resolveSessionKey`，原文照录）：

```js
const mainKey = user ? `${prefix}-user:${user}` : `${prefix}:${randomUUID()}`
```

- 依据：OpenClaw 参考实现 `docs/concepts/session.md`（DM 默认可续、cron fresh session）与 `docs/concepts/compaction.md:25-27`（auto-compaction 默认开）。

**根因**：`type:openai` 接入的连续性承载被拆成「无状态服务端 + 调用方自带历史」；V21 探针只验证了「透传历史可续」，未验证「服务端按身份持久会话」。当 OpenClaw 已提供按 `user` 键的持久会话能力时，继续让引擎重放历史既是 token 浪费，也把「上下文的家」留在不可持久、跨会话语义模糊的引擎 RAM 里；而两边同时累积则必然产生「同一段话被重放两次」的双历史漂移。

**改法**：

1. **裁定（双历史关系，本 PROPOSAL 结论 = 落 ADR-015 + §5.4 两行）**：**对话历史的唯一权威 = OpenClaw 服务端持久会话**（按 `agent:kidpal:openai-user:<device_id>` 键）；引擎 RAM `Dialogue` 降为**本地非权威副本**——只用于本地簿记（标题生成 / 调试日志），**一律不向 OpenClaw 重放**；前端不持对话历史。记忆库（Honcho 消息存储）不是会话上下文的第二真相源——它只作画像原料仓库，「刚才这段对话说了什么」的权威判定仍归持久会话。

2. **§2.2 容器级总图**：整块替换（含记忆侧车行，避免同一代码块二次编辑；逐字）：

```text
   │  LLM type:openai ▼ loopback 127.0.0.1:18789（每轮：user=<device_id> + 本回合 + 动态 context 块）
【编排大脑】OpenClaw（持久会话 + 记忆画像 + 工具 + cron）
   │  意图/多轮对话（服务端持久会话，按 user 标识；引擎只发新回合）/ 业务 MD 读写 / QQ 录入
   │  记忆画像：Honcho 自托管侧车（loopback 127.0.0.1:18100）deriver / representation
   │  提醒：cron 重算（openclaw-patch 确定性脚本）写 alerts/pending.json
```

3. **§5.2 场景②**：局部替换（逐字，见下方围栏）——原句片段与替换后片段：

```text
原文片段：→ OpenClaw（每轮透传历史）流式回复 →
替换后：  → OpenClaw（服务端持久会话：请求携 user=<device_id>、只发本回合，见 ADR-015）流式回复 →
```

4. **§5.4 数据视图**：原「对话历史（剥离前原文）」行拆为两行（逐字替换原行）：

```text
| 对话历史（服务端持久会话） | OpenClaw 会话存储（按 `agent:kidpal:openai-user:<device_id>` 键） | OpenClaw（每轮追加本回合；压缩 = auto-compaction，默认开） | 持久；跨 UI 会话保留（ADR-015；增长 / 压缩成本 = §8.1 风险 14 观察项） | 云端持久 | 非契约（wire 无此形状） |
| 对话历史（剥离前原文，引擎本地副本） | 引擎容器内存 | 引擎 | 连接期；**本地非权威副本、不向 OpenClaw 重放**（ADR-015） | 掉电即失 | — |
```

5. **§8.1 风险表**：风险 9 追加闭账句、并新增风险 14（逐字追加于风险 9 行缓解列末与表尾）：

```text
风险 9 缓解列追加：**已闭账（2026-10-22，`261022-device-memory-prompt-overhaul`）**：定性 = feature（跨会话连续性 + 记忆画像的底座），决策本体 = ADR-015；上下文污染的重放双源风险由「引擎只发新回合 + 持久会话单一权威」结构性消除；残余 = 历史增长 / 压缩成本（见风险 14）。决策 21 所列结构级备选（方案二：网关设备腿常驻、引擎腿按会话起停）随 ADR-015 承载定案失效。
风险 14 新行：| 14 | 持久会话历史增长与 auto-compaction 成本 / 质量（ADR-015 连带） | 长会话 token 成本；压缩后早期细节可能丢失（长期事实另存于记忆画像，兜底） | P2 上线后观测；压缩默认 safeguard（参考实现 `docs/concepts/compaction.md`）；必要时调 `keepRecentTokens` | dev-loop 运维面（观察项） |
```

6. **§8.2 开放问题 1**：整条替换（逐字；保留原文结论为历史、追加取代注记）：

```text
1. ~~多轮会话连续性~~ **已定案（2026-09-11 V21 探针实证）→ 2026-10-22 再次定案（取代）**：2026-09-11 定「stateless 不续、**每轮透传历史**续」；2026-10-22 `261022-device-memory-prompt-overhaul` 起取代为「引擎请求携 `user=<device_id>`、只发本回合，OpenClaw 按标识维护**服务端持久会话**」（决策本体 = ADR-015）——旧路线承载方案关闭；契约 §四「会话连续性承载」同批改写为定案记载（ARQ-260902-multiturn-continuity 已于 2026-10-12 关闭，本批只清契约字面残留）。
```

7. **§10 决策 21**：尾句后追加取代注记（逐字；体例照决策 25）：

```text
⚠️ **2026-10-22 取代注记（部分）**：「引擎侧对话上下文将跨 UI 会话累积（当 feature 或当 bug 待 human 拍）」已定案为 **feature**（跨会话连续性 + 记忆画像的底座），并由 `261022-device-memory-prompt-overhaul` 结构性落实——引擎请求携 `user=<device_id>`、只发本回合，上下文累积的家移到 OpenClaw 服务端持久会话（决策本体 = ADR-015）；引擎 RAM 副本降为非权威、不重放。决策 21 其余（常驻连接 / 24h 兜底 / 告别语关闭）不变。
```

目标文件：`docs/architecture.md`（改）

### T3 · prompt 承载反转：AGENTS.md 静态语义单一事实源 + 动态 context + ADR-001 不变式重定义

**问题**：v11 的静态语义（回合路由表 / 纪律 / 各域边界）现在作为引擎 `prompt:` 键，每轮随请求进入 OpenClaw 的 `extraSystemPrompt` 段——落在**缓存边界之下**，不进稳定前缀；同时 workspace `AGENTS.md` 自声明为「次要承载」（:15），与它承载的护栏内容形成「谁说了算」的双源张力。静态语义的主承载必须反转到 `AGENTS.md`，动态块瘦身，并把 ADR-001 的三面接线不变式重定义到新拓扑；这会连带改写三处部署资产的承载声明口径（AGENTS.md / README / manifest note）。

**证据**：
- 架构 :299-301 现文（逐字节选）：「声明件 `OPENCLAW_SYSTEM_PROMPT_CARRIER`（无扩展名）显式声明当前接线版 + **承载版本演进史唯一全量副本**……**接线不变式：渲染面（overlay PROMPT_SRC）== 声明面（provider.config 三键）== 被测面**（ADR-001：守卫从结构化指向解析当前版，禁版本字面断言）。」
- OpenClaw 参考实现（2026-10-22 现读）：`src/agents/system-prompt.ts:71-81` context file 序 `["agents.md", 10]` 居首、`DYNAMIC_CONTEXT_FILE_BASENAMES = new Set(["heartbeat.md"])`（仅 heartbeat 归 dynamic）；`:1267-1283` stable 段落（含 `AGENTS.md`）在 `SYSTEM_PROMPT_CACHE_BOUNDARY` 之前；`:1287-1296` dynamic 段落在其后；`:1333-1336` `extraSystemPrompt` 亦在其后（注释逐字「Channel/session-specific guidance lives below the cache boundary so large stable workspace context can remain a byte-identical prefix across turns」）。
- 实测（debt 证据 2）：单轮真实 prompt = `prompt_tokens=22116`、`prompt_cache_hit_tokens=7168`、`prompt_cache_miss_tokens=14948`；system 单条 22,167 字符。
- 自声明三处现文（逐字）：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md:15`「本文件是**次要承载**——护栏主阵地是每次会话注入的 system prompt。本文件与注入的 system prompt 冲突时，**一律以注入的 system prompt 为准**。」；`server/deploy/openclaw-agents/README.md:20-22`「护栏主阵地 = 每次会话注入的 system prompt（渲染源 `server/deploy/openclaw-system-prompt.vN.md`，经 `apply-xiaozhi-overlay.sh` 上 ECS）；两侧 `AGENTS.md` 是**次要承载**，其自声明『冲突时以注入 prompt 为准』。」；`server/deploy/manifest.yaml:109`「note: 板侧 kidpal agent 工作区护栏（次要承载，冲突以注入 prompt 为准）}」。
- v11 守则与记忆目标冲突的实测（debt 证据 5）：模型以「家庭小私事不特别记下来」拒记同学真名 / 宠物名——v12 守则修订（孩子事实必须记 / 家长隐私不记）归 P3 内容，但其**承载位置**由本 T 决定。

**根因**：历史顺序造成的承载倒置——护栏最初经由「每次注入的 system prompt」送达，`AGENTS.md` 因此被降格为次要承载；而当静态语义需要**缓存化**时，承载必须搬到 OpenClaw 的 stable project context（`AGENTS.md`），于是原自声明与目标互相矛盾。ADR-001 的不变式本身（结构化指向 + 三面集合相等）不变，变的是「哪三面」与「哪个文件是当前承载」。

**改法**：

1. **§5.5.5 整节重写**（逐字替换原节；原 v1–v11 沿革不再在本节复述，沿革全量副本归 carrier）：

```text
#### 5.5.5 OpenClaw prompt 承载与版本化（静态 / 动态两件）

**静态语义（主承载）**：单一事实源 = `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（仓内镜像 → live `/root/.openclaw/workspace-kidpal/AGENTS.md`；经 `server/deploy/manifest.yaml` 登记、`deploy.sh` 铺放、`verify-deploy.sh` 每日 md5 核验）。OpenClaw 把 workspace `AGENTS.md` 作为 stable project context 组装进 system prompt 的**缓存边界之上**（参考实现：context file 序 `agents.md` 居首、仅 `heartbeat.md` 归 dynamic，stable 段落置于 `SYSTEM_PROMPT_CACHE_BOUNDARY` 之前）——跨回合字节稳定、命中缓存前缀。v1–v11 历史 prompt 版本文件原地冻结、可 diff；演进史全量副本 = `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`。

**动态 context（每轮，缓存边界之下）**：模板 = `server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`（新增），经 `apply-xiaozhi-overlay.sh` 渲染进引擎 `.config.yaml` 的 `prompt:` 键；内容 = 有界的回合内动态信息（本回合时间 / 日期、front 注入的课表摘要等），**不含静态纪律**。它在 OpenClaw 侧落在缓存边界之下的 Conversation Context（参考实现 `extraSystemPrompt` 段），不污染稳定前缀。引擎既有 `prompt_template` 键与动态块的分工归 P1 实现，本节只钉「静态不进动态块、动态块有界且不含纪律」。

**承载不变式（ADR-001 重定义，`261022-device-memory-prompt-overhaul`）**：① **静态面**：渲染面（live workspace `AGENTS.md`）== 声明面（`OPENCLAW_SYSTEM_PROMPT_CARRIER` + `provider.config.yaml` 静态承载三键，键形归 P1 实现）== 被测面（探针直接读同一 `AGENTS.md`）；② **动态面**：渲染面（`.config.yaml` `prompt:` 值）== 声明面（`provider.config.yaml` 动态模板三键）== 被测面（探针请求同源）。守卫从结构化指向解析当前承载（版本无关），集合相等照 ADR-001 本体两原则执行；ADR-001 的「版本化部署制品」实例枚举由本批重定义收窄为上述两面（ADR-014 记载 amend，不覆盖其决策本体）。

**OQ-8 裁定（2026-10-22，本批）**：**否决「v12 双件」**——v12 与 `AGENTS.md` 同载静态语义 = 两处事实源，ADR-001 的三面相等会退化为「三面同指一版 + 一个影子副本」。**「单一事实源」原则采纳，落点 = 现有 prompt 资产家**：静态语义唯一件 = `openclaw-agents/workspace-kidpal/AGENTS.md`（原地）；动态模板与 carrier 留在 `server/deploy/` 现址（`kidpal-dynamic-context.v1.txt` / `OPENCLAW_SYSTEM_PROMPT_CARRIER`）。**不新建 `server/deploy/prompts/` 平行目录**——现成 `openclaw-agents/` 家仍在（`AGENTS.md`、README、历史版本文件），新造平行目录会产生「两个 prompt 家」的新歧义；搬家本身不产生单源语义收益，只增加 manifest / deploy / verify / README 四处路径改写与冻结验收测试 corpus 迁移的漂移面。目录归纳如确需，另立纯搬迁批，不阻塞本列车。
```

2. **workspace `AGENTS.md` 自声明反转**（逐字替换 :13-15 小节）：小节标题替换为 `## 静态语义单一事实源`（二级标题；本行因 §2 结构解析限制以行内形态列出，landing 时按此标题落）；小节正文逐字如下：

```text
本文件是**静态语义的主承载与单一事实源**：每次会话注入的只是**动态 context 块**（时间 / 日期、课表摘要等回合内信息，不含纪律）。两者冲突时，**以本文件为准**；动态 context 块只补上下文、不改纪律。
```

3. **`openclaw-agents/README.md` 承载口径反转**（逐字替换 :20-22 三行）：

```text
护栏与静态语义承载（`261022-device-memory-prompt-overhaul` 起反转原口径）：

- **kidpal 侧（设备语音链路）**：静态语义单一事实源 = `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`
  （manifest 登记、`deploy.sh` 铺放、`verify-deploy.sh` 每日核验）；每次会话注入的只是动态 context 块
  （时间 / 日期、课表摘要等，不含纪律），冲突以 `AGENTS.md` 为准。
- **main 侧（家长 QQ / cron）**：护栏承载 = `server/deploy/openclaw-skills/workspace-main/AGENTS.md` + workspace skills
  （不经引擎注入）。
```

4. **`manifest.yaml` 条目 note 反转**（逐字替换 :109 的 note 值；条目结构、新增资产登记归 P1）：

```text
note: 板侧 kidpal agent 静态语义单一事实源（261022 起主承载；每次会话注入的动态 context 块不含纪律，冲突以本文件为准）}
```

5. **落地时序约束（必须写进 landing summary 与 P1 brief）**：本批只改仓内镜像与声明口径；live 铺放随 P1 的 `deploy.sh` 执行。中间窗口内 `verify-deploy.sh` 会报 `AGENTS.md` md5 漂移——**属预期已知窗口**；**P1 落地前不得单独 `deploy.sh` 铺放该文件**（v11 语义尚未迁入时提前反转承载声明 = 护栏真空）。P1 launchPreconditions 已含「arch 变更记录在场」，与本约束一致。

目标文件：`docs/architecture.md`（改）· `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（改）· `server/deploy/openclaw-agents/README.md`（改）· `server/deploy/manifest.yaml`（改）

### T4 · 记忆写入资格门与 AC5 完整枚举（含一次性回合隔离机制裁定）

**问题**：记忆子系统上线后，若不做资格区分，deriver 会把**所有**经过 kidpal agent 的回合当聊天抽取——包括 front 的两类一次性回合（卡壳引导 / 忠实整理）与提醒到点语义回合。requirements S-F5.1 AC5 明确要求「设备自身流程触发的回合其内容不进记忆画像」，且把「完整枚举」交给架构文档定义；同时 AC3/AC4 的隐私边界不能只靠 agent 的 prompt 纪律——deriver 看的是**原始消息**而非 agent 回复，prompt 里的「家长隐私不记」约束不到抽取器。

**证据**：
- requirements `:598` AC5 逐字：「Given 设备自身流程触发的回合（不是孩子跟小伴真实聊天；完整枚举由架构文档定义——现知至少含：front 故事整理/卡壳引导回合、提醒到点语义回合），When 回合发生，Then 其内容不进记忆画像」；`:654` NFR-PRIV-1 边界三条。
- front 一次性回合实现（2026-10-22 现读）：`server/gateway/front/front_app.py:170-186` `_openclaw_chat_once` 只构造 `messages = [{"role":"system",…},{"role":"user",…}]`；`server/gateway/provider.py:97-108` 的 `openclaw_stream_request` 只产出 `{model, messages, stream}`——**无 `user` 字段**。
- Honcho 插件行为（参考实现 `docs/concepts/memory-honcho.md`）：「After every AI turn, the conversation is persisted to Honcho. Both user and agent messages are observed」；注入发生在 `before_prompt_build` 钩子。debt 证据 7：`hooks.allowConversationAccess=true` 后，**一次性路径钩子即正常触发 → 消息入库**。
- 提醒语义回合承载（参考实现 `docs/concepts/session.md` 路由表）：「Cron jobs | Fresh session per run」；本仓该回合落在 **main** agent 的 cron 任务（`server/deploy/openclaw-skills/workspace-main/kidpal-reminder-semantic/SKILL.md`），不在设备语音链路。

**根因**：记忆子系统的默认写入面 = 「经过 agent 的一切」，而设备流程回合与真实聊天共享同一 agent / 同一 OpenClaw 端点；不引入**资格判据**（谁有资格被记），AC5 只能靠黑名单式排除，必然漏。而 AC3/AC4 的边界若只写在孩子对话的守则里，抽取器仍会照抽，属于防线放错层。

**改法**：

1. **新增 §6.7 横切机制小节**（追加于 §6.6 之后；逐字）：

```text
### 6.7 记忆写入资格与画像

**在哪侧：记忆子系统（§5.1.2）+ 资格门（跨 front / 引擎 / OpenClaw 的部署面断言）。**

**资格门（唯一判据，出生即定）**：仅当回合来自 kidpal agent 的语音链路、且请求携带 `user=<device_id>` 时，该回合内容才可进入记忆管线（写 observation / 落消息）；**读侧同门**——非资格回合不注入画像。资格门**全局生效**（不只 kidpal）：main agent 的 QQ / cron 回合、任何不携 `user` 的调用一律不入库。设备流程触发的回合因此结构性不入画像，完整枚举（S-F5.1 AC5 的「完整枚举由架构文档定义」即本表）：

| # | 回合 | 触发 / 承载 | 不入画像的原因 |
|---|---|---|---|
| 1 | 卡壳引导回合 | front 一次性 LLM（`_openclaw_chat_once`，request 级 system 指令） | 不携带 `user`（front 内部回合，无设备身份） |
| 2 | 忠实整理回合 | 同上 | 同上 |
| 3 | 提醒到点语义回合 | OpenClaw cron → **main** agent 产 `semantic.json` | 非 kidpal 语音链路；cron 会话无设备 `user` |
| 4 | 提醒三档机械脚本 | `reminder_schedule_lib.py` | 无 LLM 回合 |
| 5 | 家长 QQ 录入 / 查日志回合 | QQ 通道 → main agent | 非设备语音链路；peer = 家长通道身份，非设备 peer |
| 6 | 故障 / 兜底固定文案 | front 固定安抚句 / 引擎 `system_error_response` | 无 LLM 回合 |

（记录语境期间引擎侧本就无 LLM 回合〔ADR-013 读法乙〕，不在枚举内。新回合形态入库前须先判资格门，不满足者默认不入画像——**白名单入册，不做黑名单剔除**。）

**画像边界（AC3 / AC4 抽取边界，三层纵深）**：① **来源门** = 上表资格门；② **抽取策略** = deriver 只抽「孩子世界的事实」（喜好 / 宠物 / 同学 / 经历），家长隐私信息（姓名 / 联系方式 / 家庭安排）不产 observation——策略承载首选 Honcho deriver 侧的提示配置；若自托管版本不支持自定义 deriver 提示，退回「持久化前消息侧过滤」；③ **注入边界** = representation 注入前的类别过滤（低置信 / 敏感类不回注）。验收判据 = seeded 负向探针集 100% 零命中（NFR-PRIV-1）；三层任一失守即不达标。P3 实测成形后回填本小节。

**历史 backfill**：2026-09-30 前 `session-corpus` 全量照记（human 2026-10-22 裁：不分回合性质、含第三方姓名）；幂等可重跑、前后计数报告、清库回滚（实现归 P4）。

**删除语义**：NFR-PRIV-1 —— 运维面手工删除记忆画像与 observation / 消息存储；不删 OpenClaw 完整日志、历史补录源语料、设备诊断日志（各按既有接受口径保留）。

**降级**：记忆服务不可用 → 本轮无画像注入、对话与回复照常（不阻塞、不静默重试风暴）；记忆失败不进 §6.3 的用户可见兜底（用户无感知）。
```

2. **§5.1.2 front 网关行**：行尾（`提醒轮询循环` 之后、行末 ` |` 之前）追加一句（逐字）：

```text
；**引导 / 整理一次性 LLM 回合不携 `user`**（§6.7 资格门：非资格回合不写记忆、不注入画像）；**课表摘要注入**（会话建立时把 `schedule.py` 派生的课表文本经内部通道注入引擎，供动态 context 块消费——§5.2 场景②）
```

3. **隔离机制三候选与裁定（本批定形）**：

| 候选 | 形态 | 结论 |
|---|---|---|
| 甲（推荐） | **按 `user` 缺席过滤**：资格门以「请求是否携带 `user`」为唯一判据；主链路（P2 起引擎必带）与一次性回合（不携）结构性区分；读侧同门 | **采纳**——零新标识符、零新 agent、与 P2 的 `user` 语义天然同源 |
| 乙 | sentinel user（`user=__internal__` 之类）标识内部回合 | 否决——该标识**不能自带过滤能力**，仍要落一条过滤规则；且给内部回合留下「可被当作用户」的键，未来若需会话连续性反而扩大污染面。留作甲失效时的备选 |
| 丙 | 独立路径 / 独立 agent（一次性回合改走 main 或新 no-memory agent） | 否决——插件钩子是全局的，换 agent 不自动隔离记忆；main 有自己的 workspace / 工具 / 人格，不合适；新 agent = 部署面扩张 |

  甲的实现落点（插件配置过滤位或薄包装补丁，沿 `openclaw-patch` 先例）归 P3；架构面只钉判据与验收。

4. **验收方式（写入 P3 gateProfile）**：① **marker 探针**——在引导回合输入一段唯一 marker 串，查 Honcho（DB / 日志双证）零命中；同法对整理回合、提醒语义回合各一次；② 主链路正证——同一 marker 出现在真实聊天回合，画像 / 消息库命中；③ front 侧结构断言——一次性回合 payload 无 `user`、主链路 payload 恰一条 user message 且 `user=device_id`；④ seeded 负向探针集（AC3）100% 零命中。

目标文件：`docs/architecture.md`（改）

### T5 · 课表机械注入：front 单源派生 → 内部通道 → 动态 context 块

**问题**：课表回合「该读未读、凭印象编造」在真机与探针均可复现；v11 已把识别特征扩列到极致仍不能保证执行——纯 prompt 纪律把「是否读文件」留给模型自由裁量，是不可靠层。需要机械注入：把**同一份**课表内容随每轮 prompt 送达模型，让「零工具调用」也能答对，同时守住 `schedule.py` 唯一解析器的单源红线（契约 §1.5）。

**证据**：
- debt 证据 4（逐字节选）：探针课表回合模型语音称「我先看一下课表哈」但**零工具调用**，直接编造「周四=语文/数学/英语/科学 + 16:00-17:00 美术课带画笔」；真实 `workspace/kidpal/schedule.md` 周四 = 语文/数学/道法/体育/英语/劳动/足球 + 18:30-20:00 编程。
- 契约 `:184`（§1.5 第 3 条）现文：「课表解析 / 网格计算仍由 `schedule.py` 唯一解析器单源承载（arch §5.1.2 明列课表解析组件归 front）——此为机械计算与校验，非自研业务逻辑，**零自研业务逻辑红线不变**。」
- 架构 `:197` 现文：「课表解析 | `gateway/schedule.py` | B1 三区块 MD 纯解析（`parse_schedule_md` / `items_for_date` / `group_upcoming` 分组）——**唯一解析器**，openclaw-patch 复用不自建第二套」。
- workstream PROPOSAL R8（FROZEN）：「课表机械注入走 schedule.py 单源，注入位置 = P1 动态 context 块」。

**根因**：模型有工具不等于会用；「先读再答」是行为承诺而非结构保证。把课表内容放进**每轮必达**的动态 context，读取动作从「模型决定」变成「装配决定」，才与「机械注入」名实相符。

**改法**：

1. **候选与裁定**：

| 候选 | 形态 | 结论 |
|---|---|---|
| 甲（推荐） | **front 拿解析结果经内部通道注入**：front `schedule.py` 派生课表文本 → 会话建立时经既有内部注入通道（`kidpal_speak` / 记录语境信号同型，ADR-003 / ADR-013 先例）送入引擎 → 引擎拼进每轮动态 context 块 | **采纳**——单源零触碰；引擎零解析、零新挂载；复用已验证注入形态；课表新鲜度 = 会话建立时点 |
| 乙 | **engine-patch 绑定单源**：引擎容器 import `schedule.py`（bind-mount 模块）或读 front 预渲染工件 | 否决为主案——前者把 gateway 冻结模块引入引擎运行时（跨容器双部署副本 + `sys.path` / 挂载漂移面）；后者新增文件交接面与第二读取位，等同把单源拆成「谁写工件、谁读工件」两段时序耦合 |
| 丙 | 引擎自解析 `schedule.md` | **直接否**——造第二解析器，违反契约 §1.5 单源红线 |

2. **注入语义（写进本节，P5 实现细节归 P5）**：① **逐轮在场**——不按意图门控（是否课表回合的判断本身就是模型裁量，要机械就不要把它留给模型）；② **有界**——注入窗口 = 今日 + 近日（建议 ≤3 日、≤2KB，P5 定稿），超窗日期仍走读文件或诚实说查不到，禁凭印象；③ **刷新**——会话建立时注入；课表变更（QQ 录入 / 跨天）最迟下一会话生效，会话内变更不追（记录为边界）；④ **单源可核**——注入内容必须与 `parse_schedule_md` / `items_for_date` 同日派生逐项一致（P5 以「注入内容 == 解析器输出」做单源反证）。

3. **§5.2 场景②**：在 ② 段末追加（逐字）：

```text
**动态 context 装配（`261022-device-memory-prompt-overhaul`）**：引擎每轮发往 OpenClaw 的系统侧内容分两层——静态语义在 workspace `AGENTS.md`（缓存边界之上，§5.5.5）、动态 context 块在缓存边界之下；动态块含本回合时间 / 日期与 front 经内部注入通道提供的**课表摘要**（front `schedule.py` 唯一解析器派生，会话建立时注入；引擎不解析 MD、不建第二解析器——契约 §1.5 单源红线）。课表回合因此无需工具调用即可作答；摘要窗口外日期仍走读文件或诚实说查不到。
```

4. **§5.1.2 引擎部署副本补丁行**：行尾（`启动标记行 … = 补丁在位的唯一机械报警锚` 之后、行末 ` |` 之前）追加（逐字）：

```text
；**动态 context 装配**（本回合时间 / 日期 + front 注入的课表摘要拼入每轮动态块，缓存边界之下、不含静态纪律）
```

5. **§5.1.3 组件依赖方向**：整句替换（逐字）：

```text
设备协议层 ⇄ front（唯一对外面）；front → 引擎（明文 ws；服务端内部注入：`kidpal_speak` / 记录语境等价信号 / 课表摘要）→ OpenClaw（OpenAI 兼容；每轮 `user` 标识 + 本回合 + 动态 context 块）；front ↔ 提醒交接目录（pending.json/semantic.json）；openclaw-patch 脚本 ← 复用 gateway/schedule.py 解析器（单源）；设备渲染层 ← MCP 工具面注入（view 路由单向）。
```

目标文件：`docs/architecture.md`（改）

### T6 · 契约包三节（§四「会话连续性承载」定案改写 + §1.6 承载对账注记）

**问题**：契约 §四 :283 是**契约内「待定案」记录条款**——它写「`type:openai` 接入默认无状态；多轮连续性承载方案（网关侧状态 vs 每轮透传历史）以**部署期首验**定案」，而实际运行形态早已越过这两选项：本批选的是「引擎传 `user` 标识 + OpenClaw 服务端持久会话 + 只发新回合」（第三形态）。不定案，契约字面与运行现实相反，且与已批架构 §8.2 的旧结论、本批的新结论三方不同步。另 §1.6 :191 的「通过 system prompt」在静态语义迁入 `AGENTS.md` 后承载指向陈旧（语义仍成立），按四分类回执附**非必需随批措辞对账注记**。

**证据**：
- 契约 `:283` 现文逐字：「**会话连续性承载**：`type:openai` 接入默认无状态；多轮连续性承载方案（网关侧状态 vs 每轮透传历史）以**部署期首验**定案（两轮追问连续性探针；失败 → 网关适配传会话标识/历史，或 A4 连续追问降级单轮问答并记账，ARQ-260902-multiturn-continuity）。」`grep -c "部署期首验" docs/contracts/interface-contract.md` = **1**、`grep -c "会话连续性承载" = 1`（2026-10-22 实跑）。
- 契约 `:191` 现文逐字节选：「在调 `/v1/chat/completions` 时，通过 system prompt 让 DeepSeek **始终**按此格式回复」。
- 四分类回执（workstream `review.md` 附件，architect-reviewer 2026-10-22 独立裁定）：处 A §1.6 = `NO_CONTRACT_CHANGE`（附非必需对账注记建议）；处 B §四 :283 = `ACCOMPANIES_SPEC_CHANGE`（本批批准动作闭合该待定案条款；五问反证全过；线格式零改动）。
- ARQ 状态校正（2026-10-22 现核）：`ARQ-260902-multiturn-continuity` **status=closed、closed=2026-10-12**，条目在 `docs/debt/tech/_archive/2026/`——本批只清契约字面引用，不重复收口（回执原文「ARQ 闭合」按此校正读法：契约文本级残留闭合）。

**根因**：:283 定稿于 V21 之前，V21 的定案（每轮透传历史）从未回写契约；本次 R1 再翻转承载形态，若不同批写回，契约将永久停在「待定案」且枚举失真。

**改法（契约包三节）**：

**① proposedContractDiff**

- **处 B · §四 :283 整句替换**（逐字）：

```text
- **会话连续性承载**：`type:openai` 接入默认无状态；多轮连续性**已定案（2026-10-22，`261022-device-memory-prompt-overhaul`，决策本体 = ADR-015）**：引擎请求携带 `user=<device_id>`、只发本回合新内容，OpenClaw 按该标识维护**服务端持久会话**（跨 UI 会话保留对话历史）；省略 `user` 时回落旧行为（每请求随机 session、连续性由调用方自携带历史负责），向后兼容。`device_id` 与记忆子系统 peer 的映射细节归架构与 ADR，本契约不钉（原「以部署期首验定案」及两条降级路径随定案撤销；ARQ-260902-multiturn-continuity 已于 2026-10-12 关闭，本行只清字面残留）。
```

- **处 B · 契约变更记录条**（追加在 §四既有变更记录段 :279 之后，独立一段；逐字）：

```text
（契约变更记录：2026-10-22 经 `/arch-change` 批准改写 §四「会话连续性承载」为定案记载（`user=<device_id>` 标识 + OpenClaw 服务端持久会话 + 引擎只发新回合；省略 `user` 回落旧行为；原「部署期首验」与两条降级路径随定案撤销）+ §1.6 承载对账注记（静态语义物理承载 = workspace `AGENTS.md`，格式约束语义不变、display 线格式零改动）——requirements v1.67 F5 / NFR-PRIV-1 / NFR-SEC-2 ⑤ + ADR-014 / ADR-015；随 `261022-device-memory-prompt-overhaul` 同批原子落地，见 delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md。）
```

- **处 A · §1.6 承载对账注记**（追加在 :192 引用块之后；逐字）：

```text
> 承载对账（2026-10-22）：本行格式约束语义的物理承载 = workspace `AGENTS.md`（OpenClaw 组装时并入 system prompt，arch §5.5.5）；「通过 system prompt 约束」语义不变——本节未钉承载归属，display 线格式零改动。
```

**② impact**

- **线格式零改动**：display E1（:9-194）、B1 课表 MD（:196-247）、§三提醒、§四全部私有消息帧均不变；`user` 是引擎→OpenClaw 的 **additive 请求字段**、设备端零感知；§四 :284「禁带 `max_tokens`」不受影响。
- **向后兼容**：省略 `user` = 旧行为（每请求随机 session + 调用方自携带历史）；旧引擎 / 第三方调用方不崩线。
- **牵连带（同批必须齐）**：arch §8.2 定案翻转（T2）、§5.5.5 承载反转与 ADR-001 不变式重定义（T3）、ADR-014 / ADR-015（T7）、glossary 三词条（T7）、§6.7 资格门（T4）——五件任一缺位即产生新漂移。
- **反向锚核对**：§四上下文已逐行核过，requirements 侧无反向锚；需求面 v1.67 先行 frozen，不阻塞本句改写。
- **ARQ 记账**：ARQ-260902 已 closed（2026-10-12），本批不新增 / 不重复关闭；收口口径见 §6 留根。

**③ validationPlan**（实现期验证，非本批落地门）

1. **两轮追问连续性探针**（ARQ-260902 原探针形态）新形态复验：同 `user` 两轮、payload 无历史重放，第二轮答对第一轮信息；
2. **跨 WS / 跨会话回忆命中**：新会话问此前讲过的事实（S-F5.1 AC2）；
3. **降级路径**：省略 `user` 的调用 = 随机 session + 调用方自携带历史，旧行为不崩线；
4. **七类回合行为 parity** 对 v11 基线（兼作 §1.6 格式遵循度回归）；
5. **一次性回合隔离 marker 探针**（AC5；T4 第 4 条）；
6. **seeded 负向探针集**（AC3；NFR-PRIV-1）；
7. **spec↔contract 人工逐字对账**：pass-oracle 第 4 关本仓无脚本兜底（debt `ARQ-260827-contract-mech-gate-gap` 在案），按人工核对执行并如实记录结论。

目标文件：`docs/contracts/interface-contract.md`（改）

### T7 · ADR×2 与 glossary 拓扑词条

**问题**：本批反转了两个长期决策面（记忆架构 / 会话模型）并重定义了 ADR-001 的实例枚举；按架构文档头部纪律，「新架构决策一律登记 ADR」，且术语表要求「新词先落这里再使用」——`记忆子系统` / `持久会话` / `动态 context 块` 三个拓扑词在正文使用前必须在 glossary 定义。

**证据**：`docs/decisions/ADR.md` 尾部现为 ADR-013（下一号 = ADR-014 / 015；格式 = 日期 / 决策 / 理由 / 备选与放弃原因 / 关联）；`docs/glossary.md` 现有「记忆画像」（:12）与「observation」（:31），无「记忆子系统」词条、无「持久会话」、无「动态 context 块」；requirements v1.67 已随需求批落「记忆画像」「observation」两词条（glossary 承接标注在案）。

**根因**：决策本体与术语不在 spec 正文里重复定义，否则两处必腐烂；ADR 收决策与备选，glossary 收一句话定义，架构正文只放机制。

**改法**：

1. **ADR 追加两条**（要点级；正文 landing 时按五件格式落）：

- **ADR-014 · 记忆架构：Honcho 自托管 + 设备级 peer + 白名单资格门 + 抽取三层纵深**（日期 2026-10-22，`261022-device-memory-prompt-overhaul`）
  - 决策：① 引入 Honcho 自托管记忆子系统（五容器、loopback 18100、postgres+pgvector；生产 profile 装插件 + `hooks.allowConversationAccess=true`）；② 记忆身份 = **设备级 peer**（`user=<device_id>` → `agent:kidpal:openai-user:<device_id>`），多设备 / 多孩子身份体系不做；③ **资格门** = 仅 kidpal 语音链路且携 `user` 的回合可读写记忆（白名单入册；§6.7 完整枚举）；④ **抽取三层纵深**（来源门 / deriver 抽取策略 / 注入过滤），语义权威 = NFR-PRIV-1；⑤ 历史 backfill 照记全量（human 2026-10-22 裁），幂等 / 回滚 / 计数报告；⑥ 出网 = NFR-SEC-2 ⑤（deriver→DeepSeek 同域、embedding→DashScope 同信任域）；⑦ **承载面连带**：静态语义单一事实源 = workspace `AGENTS.md`（否决 v12 双件）、动态 context 模板单件、ADR-001 不变式重定义（**本条 amend ADR-001**，不覆盖其两原则本体）。
  - 理由：探针端到端已实证（debt 证据 7）；原生记忆空转（证据 6）；资格门以 `user` 缺席为判据，与 P2 的 `user` 语义同源、零新标识符。
  - 备选与放弃：hosted Honcho（数据出网，撞 NFR-PRIV-1）；OpenClaw 原生记忆（空转，证据 6）；自研记忆层（超出本批、成本更高）；v12 双件（双源）；sentinel user / 独立 agent 隔离（T4 候选表否决理由）；多设备身份（留根）。
  - 关联：spine / workstream / debt TECH-261022 / requirements F5·NFR-PRIV-1·NFR-SEC-2⑤ / arch §1.2·§5.1.2·§5.3.2·§5.4·§5.5.5·§6.7 / ADR-001·003·013 / P3·P4。
- **ADR-015 · 会话模型：`user` 标识 + 服务端持久会话 + 只发新回合；对话历史单一权威**（同日期）
  - 决策：① 引擎请求携 `user=<device_id>`、只发本回合；OpenClaw 按标识维护服务端持久会话，承载跨 UI 会话连续性；② **双历史单一权威** = OpenClaw 持久会话（唯一可重放历史）；引擎 RAM `Dialogue` = 本地非权威副本、不重放；前端不持对话历史；③ 省略 `user` = 旧行为（调用方自携带历史），向后兼容；④ 记忆库不是会话上下文的第二真相源（画像只作长期事实补充）；⑤ 修复历史增长 / 压缩 = auto-compaction 默认开，成本观察项（arch §8.1 风险 14）；⑥ 部署顺序：OpenClaw 侧 `user` 能力已在场（部署版 bundle 实读），引擎补丁随 P2。
  - 理由：每请求随机 session 的三重封死（debt 证据 1）；双历史重放必造漂移；探针同 user 两轮可回忆。
  - 备选与放弃：网关侧状态（front 存历史 → 网关变厚、违背脑算网推 / 决策 20·23）；继续每轮透传（22k token、无跨会话）；每轮透传 + `user`（历史被再写进持久会话 = 重复累积）；device→peer 多设备（留根）。
  - 关联：arch §2.2·§5.2②·§5.4·§8.1·§8.2·§10 决策 21 / 契约 §四 :283 / requirements F5 AC1·AC2 / ADR-014。

2. **glossary 追加三条**（追加在「架构与系统」节 `observation` 之后；逐字）：

```text
- **记忆子系统**：云端自托管的 Honcho 服务栈（ECS 本机五容器、端口全 loopback），承担消息 / 会话存储、observation 抽取（deriver）与画像表示（representation），在 prompt 构建前注入画像；写入资格与边界见 arch §6.7，语义权威 = requirements NFR-PRIV-1，出网记账 = NFR-SEC-2 ⑤（261022-device-memory-prompt-overhaul）。
- **持久会话**：引擎以 `user=<device_id>` 标识、只发本回合，OpenClaw 服务端按该标识维护的跨 UI 会话对话历史——对话历史的唯一权威（ADR-015）；引擎 RAM 内的副本非权威、不重放。
- **动态 context 块**：引擎每轮发往 OpenClaw 的系统侧小块（时间 / 日期、课表摘要等），位于缓存边界之下、不含静态纪律；静态语义在 workspace `AGENTS.md`（缓存边界之上，§5.5.5）。
```

3. **§12 架构变更记录行**（表头下首行追加；逐字）：

```text
| 261022-device-memory-prompt-overhaul | 2026-10-22 | §1.1 / §1.2（+Honcho 自托管栈、+DashScope embedding、DeepSeek deriver 同域注记）/ §1.3（记忆身份体系范围声明）/ §2.2 总图（持久会话 + 记忆侧车）/ §3.1·§4（出网四条→五条、+NFR-PRIV-1 对策行）/ §5.1.2（+记忆子系统行；OpenClaw 本体行 / front 行 / 引擎补丁行）/ §5.1.3 / §5.2 场景②（动态 context 装配 + 课表摘要）/ §5.3.2（+Honcho 行）/ §5.3.3（Honcho 端口核验）/ §5.4（+记忆两行、对话历史两行）/ §5.5.5 整节重写（静态 / 动态承载 + ADR-001 重定义）/ §6.7 新增（记忆写入资格与画像）/ §8.1（风险 9 闭账、+13/14）/ §8.2（会话定案取代注记）/ §10 决策 19 ⑤、决策 21 取代注记 | delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md（契约面同批原子落：§四「会话连续性承载」定案改写 + §1.6 承载对账注记；ADR-014/015 + glossary 词条同批落） |
```

目标文件：`docs/decisions/ADR.md`（改）· `docs/glossary.md`（改）· `docs/architecture.md`（改）

---

## 3. 影响面 sweep 分类

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 架构 canonical | `docs/architecture.md` | 改 | T1–T5 + T7 §12 行；本批唯一 spec 载体 |
| 契约 canonical | `docs/contracts/interface-contract.md` | 改（`ACCOMPANIES_SPEC_CHANGE`） | T6：§四 :283 定案改写 + 变更记录条 + §1.6 对账注记；线格式零改动 |
| ADR | `docs/decisions/ADR.md` | 新增条目 | T7：ADR-014 / ADR-015（append-only） |
| 术语 | `docs/glossary.md` | 新增条目 | T7：记忆子系统 / 持久会话 / 动态 context 块 |
| 部署声明 / 仓内镜像 | `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`、`server/deploy/openclaw-agents/README.md`、`server/deploy/manifest.yaml` | 改（承载口径反转） | T3；只改自声明与 note，live 生效随 P1 deploy（时序约束见 T3 第 5 条） |
| 需求面 | `docs/requirements.md` | 未触 | v1.67 frozen = 本批上游权威（F5 / NFR-PRIV-1 / NFR-SEC-2 ⑤），零需求 diff |
| 实现面 | `server/deploy/`（`honcho/**`、`engine-patch/connection.py`、`provider.config.yaml`、`apply-xiaozhi-overlay.sh`、`front_app.py`、`openclaw-agents/kidpal-dynamic-context.v1.txt` 等） | 未触（本批） | P1–P5 changeManifest 在册（workstream §4）；P1 的 v12 新建项按 OQ-8 裁定撤销、改新增动态模板件 |
| 治理面 | `.claude/**` | 未触 | G1/G2/G3 零命中（spine faceMap 治理行；本会话未写任何 `.claude/**`） |
| debt 账本 | `TECH-261022` / `TECH-261018` / `TECH-261022-openclaw-prep-tax-memory-pressure` | 主线程登记动作 | 非 manifest 目标（条目文件不进本批 diff） |
| spine anchors | `delivery/intake/261022-device-memory-prompt-overhaul.md` | 主线程 landing 后回填 | anchors.arch / anchors.contract |

> **【硬项 · sweep 透镜义务】**：本批**不新增门 / 脚本 / 断言执行器**——sweep 用既有 `grep` / `spec-anchor-check.mjs` / `landing-fidelity-check.mjs`，原始输出见 §5。census 一律按命令机械枚举，判官可 spot-check。
>
> **【硬项 · 复扫模式完备性先验】**：涉退役 / 反转语面的模式集显式枚举（2026-10-22 实跑）：

```text
grep -n "次要承载" server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md server/deploy/openclaw-agents/README.md server/deploy/manifest.yaml → 3 命中（:15 / :21 / :109；另 docs/debt/tech/TECH-261022 条目有 1 处在手账本引述，不在退役面）
grep -n "每轮透传历史" docs/architecture.md → 3 命中（:79 / :211 / :377；另 docs/contracts :283 一处 = 本批改写对象、server/evidence 与验收 archive 为历史证据，不扫改）
grep -c "四条路径\|四条出网路径" docs/architecture.md → 2（:114 / :150）
grep -c "部署期首验" docs/contracts/interface-contract.md → 1（:283）
```

完备性论证：四组模式均以**无引号 / 无加粗的字面子串**检索，覆盖「**次要承载**」「『以注入 prompt 为准』」等词形变体；`docs/debt/**` 的手在账本引述、`docs/contracts/**` 的本批改写对象、`server/evidence/**` 与验收 archive 的历史证据均按各自纪律处理（账本 append-only / 契约走本批原子改 / 历史证据不改写）；`delivery/**` 过程证据不做语面改写。反转后预期见 §5 第 5–8 条。

---

## 4. human 取舍点（OQ）

**已裁项记录（不得重新开放，human 2026-10-22）**：① 路由 = **③ 拆包**（architect 裁决未改判）；② backfill **照记全量**（含第三方姓名、不分回合性质）；③ 整理 / 引导一次性回合**不入画像**；④ 出网认账（NFR-SEC-2 ⑤：deriver→DeepSeek 维持、embedding→DashScope 接受——DashScope 即阿里云百炼，与 ASR 同信任域）；⑤ **Track A 不声明**（方向已裁、探针已验；human / reviewer 可随时启动）；⑥ Intent=nointent、thinking off、分回合 thinking 不入本批。requirements v1.67 frozen 语义为准。

**本批 architect 裁定（架构职权内；architect-reviewer 可打回）**：

| 编号 | 议题 | 裁定 | 依据 |
|---|---|---|---|
| OQ-6 | 多设备 / 多孩子 peer 映射 | **设备级 peer 先行**（`user=<device_id>`）；身份体系本批不做 | 单设备现役；跨设备身份 = 新需求面（留根） |
| OQ-8 | prompt 版本化形态 | **否决 v12 双件**；静态语义唯一件 = `AGENTS.md`；不新建 `prompts/` 目录（纯搬家另批） | T3 OQ-8 段 |
| — | 课表注入路径 | **甲：front 解析 → 内部通道 → 引擎动态块** | T5 候选表 |
| — | 一次性回合隔离 | **甲：`user` 缺席即非资格（读写同门）** | T4 候选表 |
| — | 双历史关系 | **OpenClaw 持久会话唯一权威；引擎副本不重放** | T2 改法第 1 条 / ADR-015 |

**新上报 human 取舍点**：

- **OQ-10 · 记忆库备份策略**（NFR-PRIV-1 只钉保留与删除，未钉备份）：
  - (a) **不备份、靠重建**（推荐）——画像可由消息库 / backfill 语料重抽；Phase 1 数据价值可重建；零新运维面。
  - (b) 每日 `pg_dump` 落本机、有界轮转——保住历史恢复能力，代价 = 多一份本机明文副本与磁盘占用。
  - (c) 备份出网（对象存储等）——**否**：撞 NFR-PRIV-1「数据不出网」。
  - 推荐理由：现状五容器与 ECS 同机，备份与主数据同盘，抗毁收益有限；Phase 1 以「可重建」换零新增隐私面，待数据累积到不可重建规模再立批。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **PROPOSAL 结构门（pre-commit 干跑；起草期已实跑，结果 = 绿）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` → 起草期实跑 `ok:true`、`findings:[]`、`advisories:[]`、`anchorCount:7`；`anchorUnion` 成员 ≡ landing-manifest 七文件、无注记夹带路径 token。FROZEN 标注前须重跑一次（本件冻结时会因评审修订而变）。
2. **锚完整性（pre-commit 基线已跑 + post-commit 复跑）**：`node .claude/scripts/spec-anchor-check.mjs` → 基线 `ok:true` / `errors:[]` / `designIdCount 64` / `storyCount 14` / `anchorLines 36`（2026-10-22 实跑）；post-commit 预期 `errors` 仍零、`designIdCount` 不跌（本批不改小节编号、不触 UI 锚；新增 §6.7 为追加节）。
3. **manifest 完整性（post-commit）**：`git diff --name-only` 与 landing-manifest 七文件**集合相等**；无清单外文件（尤其 `docs/requirements.md` / `.claude/**` / 任何实现面代码零 diff）。
4. **架构面存在性硬断言（pre-commit 基线已跑 + post-commit）**（2026-10-22 基线 → 预期）：

```text
grep -c "Honcho" docs/architecture.md                         基线 0 → 预期 ≥6（§1.2 / §2.2 / §5.1.2 / §5.3.2 / §5.4 / §6.7 各至少一处）
grep -c "记忆画像" docs/architecture.md                       基线 0 → 预期 ≥3（§1.1 / §2.2 / §5.1.2）
grep -c "每轮透传历史" docs/architecture.md                   基线 3 → 预期 1（仅 §8.2 取代注记内的历史引述）
grep -c "四条路径\|四条出网路径" docs/architecture.md          基线 2 → 预期 0
grep -c "五条路径\|五条出网路径" docs/architecture.md          基线 0 → 预期 2（§3.1 + §4）
grep -c "user=<device_id>" docs/architecture.md               基线 0 → 预期 ≥2（§2.2 / §10 决策 21 注记）
grep -c "ADR-015" docs/architecture.md                        基线 0 → 预期 ≥3
```

5. **承载口径反转硬断言（post-commit）**：`grep -c "次要承载"` 在 `AGENTS.md` / `README.md` / `manifest.yaml` 三文件**均 = 0**（基线 1/1/1）；`grep -c "静态语义单一事实源"` 三文件 ≥ 1 各至少一处。
6. **契约面硬断言（pre-commit 基线已跑 + post-commit）**：`grep -c "部署期首验" docs/contracts/interface-contract.md` 基线 1 → 预期 **2**（**两处均为历史引述**：§四 :283 替换句「原『以部署期首验定案』及两条降级路径随定案撤销」+ 变更记录条；承载行的 operative 语义不再含该词，人工核位置）；`grep -c "会话连续性承载"` 基线 1 → 预期 **2**（承载行 + 变更记录条各一）；`grep -c "承载对账"` 基线 0 → 预期 **2**（§1.6 注记 + 变更记录条引述）；`grep -c "user=<device_id>" docs/contracts/interface-contract.md` 基线 0 → 预期 ≥1（§四）。
7. **ADR / glossary 硬断言（post-commit）**：`grep -c "^## ADR-014"` / `"^## ADR-015"` 各 1；`grep -c "^- \*\*记忆子系统\*\*"` / `"^- \*\*持久会话\*\*"` / `"^- \*\*动态 context 块\*\*"` 各 1（基线全 0）。
8. **边界守持核（人读 diff，post-commit）**：契约 diff 仅 §283 行 + 变更记录段 + §1.6 注记；display E1 / B1 / §三 / §四其余消息帧零改。架构 diff 仅本 PROPOSAL 各 T 列明节位；`§10` 历史决策正文不改写（只追加注记）。AGENTS.md diff 仅 :13-15 小节；README 仅 :20-22 段；manifest 仅 :109 note 值。
9. **契约机械凭据闸（post-commit，合并前）**：`node .claude/scripts/pre-merge-escalation-gate.mjs` 对契约 diff 三件联立（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖契约路径）→ 缺一 BLOCK。
10. **对齐核验（人读，post-commit）**：landing-verifier 逐文件核对实际落地 == 本 PROPOSAL §2 各 T 的逐字段；特别核对 §6.7 六行枚举与 T6 三节文本。
11. **已知窗口（落地时序）**：`AGENTS.md` 仓内镜像改动后、P1 deploy 前，`verify-deploy.sh` 会报该文件 md5 漂移——**预期已知窗口**，不得据此判红；P1 brief 必须写明「P1 落地前不得单独铺放该文件」（T3 第 5 条）。

**起草期实跑记录（2026-10-22）**：第 2 条基线 = `ok:true` / `errors:[]` / `designIdCount 64` / `storyCount 14` / `anchorLines 36` / `archSectionCount 47`；第 4 条基线 `Honcho`=0、`记忆画像`=0、`每轮透传历史`=3、`四条路径|四条出网路径`=2、`user=<device_id>`=0、`ADR-015`=0；第 5 条基线三文件 `次要承载`=1/1/1；第 6 条基线 `部署期首验`=1、`会话连续性承载`=1、`承载对账`=0；第 7 条基线 ADR-014/015=0、三词条 headword=0；锚文 byte 复核（`grep -c` 逐条 = 1）：契约 :283 原句 / :191 原句 / arch :79 / :272 / :368 / :419 四处逐字。第 1 条 preflight 实跑 = `ok:true` / `findings:[]` / `advisories:[]` / `anchorCount:7` / `anchorUnion` = 七文件全集；实跑中修正两处**本件自身**的形态问题（四反引号围栏与围栏内 H2 标题干扰 §2 解析器，已改为三反引号围栏 + 标题行内列示）——属起草期自检产物，不涉 canonical。

---

## 6. 留根清单

### 6.1 拆包预判（引 workstream §4，不重拆）

切片 P1–P5、merge order、单 Pod 串行与共享文件面 = workstream PROPOSAL §4（FROZEN v1.0）原样；本件只补一条 OQ-8 连带的清单修正：**P1 的 `openclaw-system-prompt.v12.md` 新建项撤销**，改为新增 `server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`；「v11 语义迁 AGENTS.md」内容项不变。另：**既有 `server/deploy/kidpal-agent-prompt.txt`（在役 v11 极简模板）的去留 / 改写归 P1 定形，P1 不得使两个动态模板件并存（由新动态模板件取代或原地改写，二选一）**。portfolio-planner 拆包时以本段为准。

### 6.2 收口标准（引 workstream §5）

跨包并集验收七条（体积 / 记忆 / 正确性 / 隔离 / 出网 / 资源 / 部署）与「收口轮 owner 包 = P5 + P4 计数报告对账」= workstream PROPOSAL §5 原样，不重复、不加减。

### 6.3 逃生门

实现期发现与 spec / 契约冲突时走 `/arch-change` 判级 + human 冻结；契约面走 architect-reviewer 四分类后随批原子落，不在 Pod 内自裁（workstream §3.4 原样）。本批已闭合两处契约疑似项（OQ-5）；新增疑似项须重开 loop，不得在 P1–P5 内顺手改契约。

### 6.4 留根

- **多设备 / 多孩子记忆身份体系**（OQ-6）——设备级 peer 先行；换设备 = 新 peer、旧画像不迁移（记录为已知后果）。
- **记忆库备份 / 恢复策略**（OQ-10，待 human）。
- **deriver / dialectic 供应商收敛**（DashScope qwen 系备选）——OQ-2 留架构面；本批按 NFR-SEC-2 ⑤ 维持 DeepSeek，不另立批。
- **持久会话增长与 compaction 成本**——arch §8.1 风险 14 观察项（P2 上线后）。
- **抽取策略实现形态**——deriver 提示可配置性未验；若不可配，退回消息侧过滤（P3 定形并回填 §6.7）。
- **一次性回合 prep 税复测**——静态语义迁入 `AGENTS.md` 后，引导 / 整理回合的 prompt 构建成本需复测（现测 prep ~7s、引导回合超时 10s；P1 / P3 复核，必要时调超时参数，不改语义）。
- **v12 守则细则与 v11 语义迁入内容**——归 P1（迁入）与 P3（守则修订：孩子事实必须记 / 家长隐私不记）。
- **记忆删除自助界面**——Phase 1 不做（NFR-PRIV-1 已钉运维手工）。
- **目录归纳（`server/deploy/prompts/`）**——如确需，另立纯搬迁批，不阻塞本列车。
- **debt 收口动作**——TECH-261022 主触发、TECH-261018 consumed-by、prep 税复测回填，均归主线程 / 实现批登记；ARQ-260902 已于 2026-10-12 关闭（本批仅清契约字面）。
- **历史 prompt v1–v11**——原地冻结、可 diff（非目标）。

---

## 7. 评审记录

### R1 · architect-reviewer（2026-10-22，独立实例，verdict: BLOCK(F1,F2,F3)）

四查主体全 PASS（意图保真 PASS——T4 资格门判属已裁语义的机制承载、采纳；纪律合规 PASS——契约包对回执保真、未越权改判四分类，ARQ-260902 状态校正如实；撞车复核 PASS——:377/决策 21/决策 19/ADR-001 取代面全部显式，P1 changeManifest 连带修正判合规；牵连归位 PASS——manifest 七文件判定正确、docker-compose/verify-deploy/engine-patch 归实现包）。判档：**主路径维持不降、Track A 不升**（轻路径四判据全否；收敛度清晰）。

否回项：
- **F1（必改）**：§5 验证集第 6 条「部署期首验」计数断言与 T6 ① 替换句/变更记录条自相矛盾（落地后实际 = 2，断言预期 = 1）——修订二选一：(a) 预期改 2 + 括注改「两处均为历史引述」；(b) :283 替换句改不含词面表述、预期维持 1。
- **F2（低危）**：风险 9 闭账未处置决策 21「结构级备选（方案二）留 Phase 1」悬置——闭账句尾补「决策 21 所列结构级备选（方案二）随 ADR-015 承载定案失效」。
- **F3（低危）**：T3 新增 `kidpal-dynamic-context.v1.txt` 与既有 `server/deploy/kidpal-agent-prompt.txt`（在役 v11 极简模板）双件并存隐患——§6.1 补「既有件去留/改写归 P1 定形，P1 不得使两个动态模板件并存（取代或原地改写，二选一）」。

三处提请采纳：AGENTS.md live 铺放窗口（采纳，spine/P1 brief 双落）；抽取边界可实施性（采纳带条件——P3 早验 deriver 可配置性，退回路径同过 seeded 负向探针）；§6.7 行内标题落点（采纳——landing-summary claimed 清单点名该行）。

OQ-10 意见（不终裁，呈 human）：支持 (a) 不备份靠重建；拍 (a) 时补两点记账 = 重建源显式枚举（session-corpus 源语料 + OpenClaw 会话存储）+ 重评触发条件。

OREQ 修订轮：主线程已回派 architect 按本清单修订（F1 必改 + F2/F3 随轮），修订版就位后靶向复核（R2 聚焦三 finding 到位性 + 新矛盾扫描）。

### R2 · architect-reviewer 靶向复核（2026-10-22）

verdict: GREEN

按 R1 裁决口径执行：聚焦 R1-F1/F2/F3 到位性 + 新矛盾扫描，不重开全量四查。全部核验基于读盘（DRAFT v1.1，612 行），grep 亲测。（verdict 行 = 主线程按 reviewer 回执原判以闸可解析的行首形态补记——判决本体未改。）
F1 到位确认（(a) 案：预期 2，括注「两处均为历史引述」，事故面逐行对账、旧断言零残留）；F2 到位确认（方案二闭账句与决策 21 原文语义一致）；F3 到位确认（双件隐患关闭，「portfolio-planner 拆包时以本段为准」覆盖两条修正）。新矛盾扫描：§5 其余断言与 v1.1 文本复算一致、§7 结构单一标题 + R1 逐字在档、无新 finding。判档维持 R1（主路径、Track A 不升）；OQ-10 与三处提请采纳意见不变，随批呈 human 与 landing 执行。**可进 FROZEN**；FROZEN 标注前主线程按 §5 第 1 条重跑 preflight 义务不变。

### FROZEN · 主线程标注（2026-10-22）

立项 ack（human 触点① facePlan 确认）+ R2 GREEN 兑现，主线程凭「立项 ack + GREEN」标 **FROZEN v1.1**（AGENTS.md Track B 第 3 步）。随即 packet-bound landing（manifest 七文件）。

### human 终裁记录（2026-10-22，landing 收口时）

- **OQ-10 记忆库备份策略**：human 裁 **(a) 不备份、靠重建**——画像可由消息库 / backfill 语料重抽（重建源 = session-corpus 源语料 + OpenClaw 会话存储，P3/P4 链路建成后幂等重跑）；零新增隐私面与运维面；重评触发 = 数据累积到重建不可接受 / 家长提出恢复诉求时另立批。落地投影 = arch §8.1 风险 13 缓解列同步更新。
- **off-main 升级批准**：升级闸 off-main BLOCK（main 直落工作流 vs 分支模型）经 AskUserQuestion 升级 human，human 裁**直接提交 main**并视为人工豁免记录在案——本批为走完整 loop 后的正 landing（FROZEN v1.1 + R2 GREEN + landing-verdict PASS + 契约三件联立内容在场）。
