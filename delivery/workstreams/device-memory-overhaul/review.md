# review.md · device-memory-overhaul 分流 review 存档

> **provenance**：2026-10-22 /triage-change 会话内联派 architect（只读+同会话 return，零写入）执行四面探测（change-intake.md §3）与比例化 review（change-triage.md §2），返回回执全文如下（逐字存档，主线程未改写）。human 触点①裁决与 L1 答疑见 PROPOSAL.md §3.2 / spine openQ。

---

# /triage-change · architect 四面探测 + 比例化 review 回执（只读，未写任何文件）

## 0. 结论速览

- **需要建 spine**：触面 ≥2（需求 + 架构 + 实现；契约疑似、治理零命中）。
- **需求面：必须走 `/requirement-change` 档 2**。这是本回执最重要的判断——requirements.md 全文「记忆」出现 **0 次**（grep `记忆` count=0），没有任何 story/AC 承载「跨会话记住孩子」，且 Honcho 生产化会新增出网路径，现有 NFR-SEC-2 四条枚举无法覆盖。
- **架构面：实质架构变更（非 docs-sync）**，走 `/arch-change` 主路径 Track B；**不自动触 Track A**（核心方向 human 已裁、探针已验证；残余分叉是参数级，建议在需求面/架构 PROPOSAL 用 human 取舍点收口）。
- **契约面：零结构性变更，但有两处措辞陈旧疑似项**，按 §6 逃生门先派 architect-reviewer 四分类，**分类未闭合前实现包不得 launch**。
- **治理面：零命中**（部署资产在 `server/deploy/**`，属实现面；不触 `.claude/**` agent 定义/gate 登记）。
- **实现面：③ 拆包**（workstream PROPOSAL → human FROZEN → portfolio-planner）。排除 ①/⓪/②Lite/②Full 的论证见 §6。

## 1. 建档前查重（change-intake §3）

- `delivery/intake/` 在册 15 份逐份比对：无同意图件；最接近的 `c:/AI/KidPal/delivery/intake/261017-prompt-v11-schedule-recall.md`（已 closed）是 prompt v11 课表召回批，与本批同面但不同意图（本批要动 session 模型 + 新记忆子系统 + 需求语义），不消费旧 spine。
- debt 四域未结条目命中 4 条，走 §3a 晋升路径：
  - **主触发**：`docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md`（MAJOR，四层候选改动 + 七组实测证据）。
  - **本批消费**：`docs/debt/tech/TECH-261018-schedule-read-compliance-residual.md:26-28`（第 3 层机械注入落地即结项）。
  - **同批复测**：`docs/debt/tech/TECH-261022-openclaw-prep-tax-memory-pressure.md:19-25`（ECS 已升 2C/4G，prep 税/内存压力在 P3 后复测；注意该条目记录的是升级前 2GB 读数）。
  - **不改只引**：`docs/debt/tech/ARQ-260907-openclaw-tools-rejection-rootcause.md`（nointent 根因，本批明确不动）。
- 结论：首次消费，新立 spine（changeId `261022-device-memory-prompt-overhaul`，与 debt id 同日对齐，沿 261017 先例）。

## 2. faceMap（查出来的触面事实，带 file:line）

| 面 | 动不动 | 动哪 | 依据 |
|---|---|---|---|
| **需求** | **动（真实变更）** | ① 新增「跨会话记得我」story（建议新 F5 或 S-F4.2）；② **NFR-SEC-2 增第 ⑤ 条出网路径**（Honcho deriver → DeepSeek 云、embedding → DashScope）；③ 「孩子事实必须记 / 家长隐私不记」边界与记忆数据保留/删除语义（建议新增 NFR-PRIV-1 或落 story 补充规则）；④ §9 追加版本行（v1.67） | `docs/requirements.md:199`（Epic E1/F1–F4 结构）、`:565-584`（F4 通用问答，现有最近邻）、`:627-631`（NFR-SEC-2 四条出网记账）、`:641-645`（NFR-SAFE-1）、`:733`（当前 v1.66）；全文「记忆/personalization」零命中。架构原则「所有数据、逻辑、记忆在云端」只是架构句，`docs/architecture.md:62`，不构成产品 AC |
| **架构** | **动（实质）** | ① **新增 Honcho 记忆子系统**（外部依赖、服务组件、数据视图、跨切面、部署/资源）；② **持久 session 模型反转**——现「每轮透传历史」定案被改写为「OpenClaw 服务端按 `user` 标识持久会话 + 引擎只发新回合」；③ **prompt 承载反转**——v11 静态部分从「注入 system prompt 主承载」迁入 workspace-kidpal/AGENTS.md（现声明为「次要承载」）；④ 课表机械注入的上下文装配机制；⑤ 决策 21 连带「上下文跨 UI 会话累积」定性从「待 human 拍」转为 feature；⑥ 新 ADR（记忆架构 / session 模型）+ glossary 新词 | `docs/architecture.md:32-41`（§1.2 外部依赖清单，DeepSeek :38 / Tavily :40）、`:62`（记忆在云端）、`:78-79`（OpenClaw「记忆+工具+cron」/「每轮透传历史」）、`:197-198`（§5.1.2 OpenClaw 本体）、`:211`（场景②「每轮透传历史」）、`:299-304`（§5.5.5 prompt 版本化，现状 v11 + 三面接线不变式 ADR-001）、`:368`（风险债 9「上下文跨 UI 会话累积，未定」）、`:377`（「多轮会话连续性已定案：stateless 不续、每轮透传历史续——承载方案关闭」）、`:402`（原则 4 薄桥 + OpenClaw 自带 OpenAI 兼容 API）、`:417`（出网路径④记账投影）、`:419`（决策 21 常驻连接） |
| **契约** | **零结构性变更，两处措辞陈旧疑似项（须四分类）** | ① `interface-contract.md:189-194` §1.6 写「**通过 system prompt** 让 DeepSeek 始终按此格式回复」——迁移后格式约束的承载变成 workspace `AGENTS.md`（OpenClaw 组装时并入 system prompt），字面仍成立但指向陈旧；② `interface-contract.md:283` §四「会话连续性承载」写「`type:openai` 接入默认无状态；方案（网关侧状态 vs 每轮透传历史）以部署期首验定案」——本次选的「引擎传 `user` 标识 + OpenClaw 服务端会话、只发新回合」不在所写两选项直面上，注释陈旧。**B1 课表 MD 结构（`:196-247`）、display E1（`:9-194`）、§三提醒、§四全部私有消息线格式均不变**；课表机械注入只改「谁把 B1 内容喂给模型」，不改 B1 结构，也不新增端云帧 | `docs/contracts/interface-contract.md:189-194`、`:283`；旁证 `.claude/rules/contracts.md` 四分类表 |
| **治理** | **零命中** | 不动任何 `.claude/agents/**`、`.claude/commands/**`、`.claude/hooks/**`、`.claude/scripts/**`、`verify-gates.json`；不新增 gate 登记。部署资产（manifest / provider.config / overlay / openclaw 配置声明件）全在 `server/deploy/**`，归 dev-loop 实现面 | `server/deploy/manifest.yaml:84-157`（部署单一事实源在实现面）、`:106-109`（workspace AGENTS.md 由 deploy.sh 铺放）；`.claude/**` 零改动 |
| **实现** | **动（大，跨多个 owning 面）** | 引擎部署面（prompt 承载 / 动态 context / 新 provider 补丁 / 课表注入）、OpenClaw 部署面（Honcho 插件 + hooks + agent tools 覆写 + workspace 文件）、运维面（Honcho 栈常驻 + ECS 资源 + 历史 backfill）、front 一次性回合隔离（可选） | `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md:13-15`（自声明「次要承载」）、`server/deploy/openclaw-agents/README.md:20-21`（护栏主阵地 = 注入 system prompt）、`provider.config.yaml:26-55`、`apply-xiaozhi-overlay.sh:88-90,110-141`、`manifest.yaml:84-109,144-157`、`server/deploy/engine-patch/connection.py:154,294,883-894,1274-1277,1399-1412`、`server/deploy/docker-compose.yml:48-49`、`server/gateway/front/front_app.py:170-186`、参考引擎 `ESP32-S3-RLCD-4.2-Demo/05_XiaoZhiServer/main/xiaozhi-server/core/providers/llm/openai/openai.py:88-110`（payload 无 `user` 字段）与 `:113-127`（response 签名只收 session_id）、OpenClaw 参考 `c:/AI/OpenSourceReference/openclaw-main/src/agents/system-prompt.ts:1297,1333-1336`（extraSystemPrompt 在缓存边界之下） |

## 3. facePlan 建议

| 序 | 面 | 入口 | 谁裁 | 前置 |
|---|---|---|---|---|
| 1 | **需求** | `/requirement-change` **档 2**（new story + NFR 新增 = 结构性） | story-author 盘上起草 → requirement-reviewer 四查 → **architect 可行性必派** → **human 终拍**（frozen + §9 v1.67） | 无。human 需先裁开放问题（记忆范围、保留/删除、出网路径认账）——出网/backfill/整理回合三项已于触点①及 L1 裁定，story 落位与 NFR-PRIV-1 形态留需求面 |
| 2 | **架构** | `/arch-change` **主路径 Track B** | architect 起草 PROPOSAL → architect-reviewer 四查 + 判档 → human 立项裁收敛度；Track A 仅在 reviewer 发现方向级分叉时升（只升不降） | 需求面 frozen；起草可并行酝酿 |
| 3 | **契约** | **逃生门先行**：architect-reviewer 对 §1.6 / §四:283 四分类 | architect-reviewer；若判 `ACCOMPANIES_SPEC_CHANGE`，契约包三节并入架构面 PROPOSAL 随批原子落 | 架构面起草前完成分类 |
| 4 | **实现** | ③ 拆包 → workstream PROPOSAL → **human 触点② FROZEN** | 主线程落 reviewed PROPOSAL → human FROZEN → portfolio-planner 拆 P1–P5 | req frozen + arch 变更记录 changeId 行在场 + 契约四分类闭合（launchPreconditions） |
| 5 | **治理** | 无 | — | — |

**死亡顺序禁令**：需求/架构 anchors 未回填前，实现包不得 launch（change-intake §8；本批尤其不能再犯「指针先于实物」）。

## 4. 判断题逐条回答

**1. 需求面是否需要 `/requirement-change`？—— 需要，档 2。**
- ①「孩子事实必须记住 vs 家长隐私不记」**不在**现有需求语义内：requirements.md 零「记忆」提及，v11 隐私句只是 prompt 纪律（`openclaw-system-prompt.v11.md:14` 无记忆条款；真正的隐私底线句在 `workspace-kidpal/AGENTS.md:7` 与 `v11:8-9`），没有任何 AC 可言。
- ②「越用越聪明 / 长期记忆」**需要立 story**：它是可观察的产品能力（跨会话回忆孩子事实），必须 AC 化，否则实现包没有验收语义、测试 case 无处挂。建议新 F5「记忆」或 S-F4.2，AC 至少四条：同会话回忆、跨会话回忆（新 WS 新会话仍记得）、**隐私负向 AC**（家长姓名/联系方式/家庭安排不记录、不复述）、记忆数据保留/删除口径。
- 附带但硬性：**NFR-SEC-2 必须增第 ⑤ 条**——Honcho deriver 调 DeepSeek 云抽 observation、embedding 调 DashScope `text-embedding-v4`，均携带孩子对话内容出网。现有四条（`requirements.md:627-631`）不覆盖，按该条自身纪律「human 已知晓并接受」必须显式认账。
- 另需 human 裁**记忆数据存储与删除**：Honcho postgres 在 ECS 本机（不出网），但契约 §四 `kidpal_story_delete` 现行边界明确「仅 story MD，不触 OpenClaw 侧对话记忆/日志」（`interface-contract.md:272` 段），记忆库无删除语义——不能由实现包自行发明。建议新增 NFR-PRIV-1。

**2. 架构面量级？—— 实质架构变更，非 docs-sync。**
- 依据：新增子系统（Honcho）+ 外部依赖增删（§1.2，轻路径判据④直接否）+ session 模型反转（推翻 `architecture.md:377` 已定案）+ prompt 承载拓扑反转（改 `:299-304` 与 `workspace-kidpal/AGENTS.md:15` 自声明）+ 决策 21 上下文累积定性变化（`:368`/`:419`）。
- 由谁落：**独立 architect 会话走 `/arch-change`**，不塞进 dev 包 docs-sync。
- **Track A？— 不自动触**。核心方向 human 已裁且 Honcho 探针端到端验证；残余分叉是参数级（多设备 peer 映射、保留/备份、backfill 过滤、嵌入/deriver 出网）。若 architect-reviewer 认为「多设备身份模型 / 记忆删除语义」是方案无法自判的方向级分叉，按「只升不降」转 Track A。
- 建议同批产出 ADR（决策本体归 `docs/decisions/ADR.md`）：ADR-0xx「记忆架构：Honcho 自托管 + 设备级 peer + 引擎动态上下文」、ADR-0xx「引擎↔OpenClaw 会话模型：user 标识 + 服务端持久会话、只发新回合」。并把 §5.5.5 的三面接线不变式（ADR-001）重定义为新拓扑下的「AGENTS.md 单一事实源 + carrier 声明 + overlay 核验」不变式。

**3. 契约面？—— 结构性零触碰确认，但有两处措辞待四分类。**
- display E1、B1 结构、§三提醒、§四全部私有消息帧：**均不变**。
- 课表机械注入**不触契约语义**：B1 文件格式不动，注入只是把同一 B1 内容（经唯一解析器 `server/gateway/schedule.py` 派生）喂给模型；但需守住 §1.5「课表解析/网格计算由 schedule.py 唯一解析器单源承载」——引擎侧若自己解析 MD 会造第二解析器，属实现红线，须在架构 PROPOSAL 里钉。
- 疑似项 ①§1.6:189-194、②§四:283 的陈旧措辞，先派 architect-reviewer 四分类；倾向 `NO_CONTRACT_CHANGE`（两处均为说明性文字、不承载线格式），但不自裁。

**4. 治理面？—— 零命中。**
`server/deploy/**` 与 OpenClaw 运行时 `openclaw.json` 都在实现面/运维面；`.claude/**` 一个不动。「Ops Record」形态在本仓治理枚举里没有对应面，生产手工步骤建议落 workstream roadmap 的运维节 + landing 后 human 部署记录，不新立治理面。

**5. 五档判定：③ 拆包。**
- **非 ①**：改实现面行为。
- **非 ⓪**：命中排除清单——跨端、安全/隐私轴、迁移、多文件多子系统、验收语义净增。
- **非 ②Lite**：多 owning 面、数据迁移与 backfill、生产重启窗口、共享文件面。
- **非 ②Full**：至少四个可独立演示、独立回滚的行为结果，塞一包造巨包并压制「快速拿回 token/正确性收益」。
- **③ 的依据**：跨多切片 + 需多 Pod + 迁移/backfill + 高风险大 feature，四项全中。

**6. 逃生门？—— 有，两处。**
1. `interface-contract.md:189-194` §1.6「通过 system prompt」承载措辞。
2. `interface-contract.md:283` §四「会话连续性承载」两选项枚举。
另有一处**非契约但需在架构 PROPOSAL 钉死**：`workspace-kidpal/AGENTS.md:15` 与 `openclaw-agents/README.md:20-21` 的「次要承载」自声明必须随批反转，否则形成双源矛盾。

## 5. reviewed plan 要点（③ 档输入 workstream PROPOSAL）

（业务目的清单 / 切片与 merge order / changeManifest / impactedTests 种子 / 风险面 / 验收口径 / 非目标——已由主线程按模板搬入 [PROPOSAL.md](PROPOSAL.md) §1–§4；changeManifest 清单逐文件如下供 portfolio-planner intake 消费：）

- P1：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`、`server/deploy/openclaw-agents/README.md`、`server/deploy/openclaw-system-prompt.v12.md`*（新建）、`server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`、`server/deploy/provider.config.yaml`、`server/deploy/apply-xiaozhi-overlay.sh`、`server/deploy/kidpal-agent-prompt.txt`（动态 context 模板）、`server/deploy/manifest.yaml`、`server/deploy/verify-deploy.sh`、`server/deploy/261022-openclaw-kidpal-tools/**`*（声明 + apply/rollback，仿 261017 先例）
- P2：`server/deploy/engine-patch/connection.py`、`server/deploy/engine-patch/openai.py`*（新 bind-mount）、`server/deploy/docker-compose.yml`、`server/deploy/manifest.yaml`、`server/deploy/verify-deploy.sh`
- P3：`server/deploy/honcho/**`*（compose + 配置 + apply/rollback + 健康检查）、`server/deploy/manifest.yaml`、`server/deploy/apply-xiaozhi-overlay.sh`（若配置经 overlay）、`server/deploy/verify-deploy.sh`、`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（v12 守则）、`server/gateway/front/front_app.py`（仅当一次性回合需隔离过滤）
- P4：`server/deploy/honcho/backfill/**`*、`server/deploy/manifest.yaml`、`server/deploy/verify-deploy.sh`
- P5：`server/deploy/engine-patch/connection.py`（或 context provider 端点）、`server/gateway/schedule.py`（只读复用，改动从严）、`server/deploy/manifest.yaml`、`server/deploy/verify-deploy.sh`

impactedTests 种子：`test_prompt_v7_round_routing.acceptance.py`（语义锚升级）、`test_261010_engine_decision21_overlay_persist.acceptance.py`、`test_261001_openclaw_model_name_overlay_drift.acceptance.py`、`test_261017_openclaw_kidpal_skills.acceptance.py`（tools 覆写先例仿制）、`test_m3k1_schedule_producer.acceptance.py`、`test_m3k3_voice_schedule_entry.acceptance.py`、`test_perf_k1_roundtrip_latency.acceptance.py`（prep 税复测）。新增断言：① AGENTS.md 承载 v11/v12 硬约束逐条在场；② carrier/provider.config/overlay 三面指向一致新不变式；③ 引擎 payload 形状（恰一条新 user message、user=device_id、无 history 重放）；④ tools allowlist apply/drift/rollback；⑤ 课表注入内容 == `parse_schedule_md` 同日派生（单源反证）；⑥ 隐私负向集；⑦ 部署单一事实源 md5+drift。

## 6. 开放问题（已分流）

- 触点①已裁：路由 ③拆包；backfill 照记；一次性回合不入画像。
- L1 已裁（主线程）：出网认账（embedding→DashScope=阿里云同 ASR 信任域；deriver→DeepSeek 非新增暴露）。
- 留需求面：story 落位（F5 vs S-F4.2）、NFR-PRIV-1 形态、保留/删除语义。
- 留架构面：Track A 不自动声明（human/reviewer 可启动）；prompt 版本化新形态（v12 双件 vs prompts/ 单源）；多设备 peer 映射；deriver 供应商收敛备选。

---

# 附件 · 契约四分类回执（architect-reviewer，2026-10-22，独立裁定逐字存档）

## 处 A · §1.6 :189-194（display 格式约束的物理承载迁 AGENTS.md）

**fourClass = `NO_CONTRACT_CHANGE`**

契约原文（逐字）：:189「### 1.6 对 OpenClaw 的要求（system prompt 约束）」；:191「在调 `/v1/chat/completions` 时，通过 system prompt 让 DeepSeek **始终**按此格式回复」。

理由：① 契约未钉承载归属——:191 只钉「格式约束经 system prompt 到达 DeepSeek」这一语义事实，未写明 system prompt 内容由谁组写；迁移后 OpenClaw 组装请求时仍把 AGENTS.md 并入发往 DeepSeek 的 system prompt，字面成立。② 线格式零改动（display 块结构、六类 view 白名单、:192 引用文本的课表意图薄载荷与星星记录本禁产条款全部不变）。③ 引擎侧承载拓扑是 arch 自己的不变式（arch :301），不是契约的。

**措辞对账挂账建议（非必需）**：随本列车 /arch-change 批顺手在 §1.6 加承载对账注记（形如「承载对账 2026-10-22：静态语义物理承载 = workspace AGENTS.md，OpenClaw 组装时并入 system prompt；『经 system prompt 约束』语义不变」）——同 2026-10-19 §1.4 措辞对账随批先例。若不加，契约仍自洽。

## 处 B · §四 :283（会话连续性承载）

**fourClass = `ACCOMPANIES_SPEC_CHANGE`**（与分流 architect 预判 NO_CONTRACT_CHANGE 不一致，独立裁定）

契约原文（逐字 :283）：「**会话连续性承载**：`type:openai` 接入默认无状态；多轮连续性承载方案（网关侧状态 vs 每轮透传历史）以**部署期首验**定案（两轮追问连续性探针；失败 → 网关适配传会话标识/历史，或 A4 连续追问降级单轮问答并记账，ARQ-260902-multiturn-continuity）。」

理由：① 这是契约内的「待定案」记录条款，本批正是闭合这个开放决策的批准动作——所选形态（user=<device_id> + OpenClaw 服务端持久会话 + 只发新回合）是括注两选项之外的第三形态；不定案则 :283 永久停留开放态且枚举失真。② 「默认无状态」作为 operative 形态被本批推翻，落地后契约字面与运行现实相反，构成事实性漂移。③ 撞车复核发现历史欠账：arch :377 已于 2026-09-11 V21 定案「stateless 不续、每轮透传历史续」，契约 :283 从未记录——契约与已批 spec 此刻已有文本级不同步，本批 R1 再翻转 :377，同批原子落是唯一不产生新漂移的窗口。④ 线格式核对：display E1、§四全部私有消息帧、B1、§三零改动；`user` 是引擎→OpenClaw additive 字段、设备端零感知；:284「禁带 max_tokens」不受影响。

反证检查（五问全过）：非实现走偏、非载荷字段问题、无命名撞 canonical、无二期前塞、零新形状。

## 契约包三节草案要点（随架构面 /arch-change PROPOSAL 原子落）

- **proposedContractDiff**：§四 :283 整句改写为定案记载（user 标识承载连续性 + 服务端持久会话 + 只发新回合；ARQ-260902-multiturn-continuity 闭合，两条降级路径随定案撤销）；粒度只钉形态事实（同 :284 粒度先例），device_id→peer 映射细节归 arch/ADR；追加契约变更记录行（指向架构批 PROPOSAL）；处 A 对账注记同批顺手落。
- **impact**：线格式零改动、设备端零感知、向后兼容（省略 user = 旧行为）；牵连带（同批必须齐）= arch :377 定案翻转（R1）、:301 承载反转与 ADR-001 接线不变式重定义（R2）、新 ADR×2、glossary；requirements 侧无反向锚（逐行核过 §四上下文），需求面先行 frozen 不阻塞此句改写。
- **validationPlan**：两轮追问连续性探针（ARQ-260902 原探针）新形态复验；跨 WS/跨会话回忆命中；降级路径验证（省略 user = 随机 session 旧行为不崩线）；七类回合行为 parity 对 v11 基线（兼作处 A §1.6 格式遵循度回归义务）；spec↔contract 人工逐字对账（pass-oracle 第 4 关无脚本兜底，ARQ-260827 在案，如实记录人工核对）。

## 结论

A = `NO_CONTRACT_CHANGE`（附非必需措辞对账建议）；B = `ACCOMPANIES_SPEC_CHANGE`（契约包三节随架构面 PROPOSAL 原子落）。主线程动作：① spine anchors.contract 与 OQ-5 回填（已完成）；② PROPOSAL §7 回填（已完成）；③ 契约包三节草案要点带入架构面 /arch-change PROPOSAL 起草输入。facePlan「分类未闭合前实现包不得 launch」——B 判 ACCOMPANIES_SPEC_CHANGE 后，闭合条件 = 架构批契约包落地，实现包 launch 闸维持。
