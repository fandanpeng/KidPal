# architect-review-260924 — 语音问答往返延迟优化方案 advisory 评审

> 状态:ADVISORY(只出意见,不执行、不 landing、不改任何配置;非 Track B 评审循环的 GREEN/FROZEN 判定物,供主线程与 dev-loop 消费)
> 评审人:architect(advisory 实例)
> 日期:2026-09-24
> 评审对象:`delivery/taskpackages/260924-perf-k1-roundtrip-latency/plan-draft.md`(状态 DRAFT)+ 同目录 `evidence/latency-baseline-260924.md`
> 评审手段:仓库只读(契约/arch/server/deploy/先例包 K3)+ ECS 只读核查(jq/journalctl/openclaw CLI 只读子命令)+ 1 次授权 loopback 探针(见 §0.2 披露)
> **落位注记**:任务书要求本文件落 `delivery/taskpackages/260924-perf-k1-roundtrip-latency/architect-review-260924.md`,但 scope-map 对 architect 的写边界不含 `delivery/taskpackages/**`(enforce-scope 钩子实拦,机械强制层不绕过)——故落本目录,**请主线程搬运/复制到任务书指定路径**(搬运动作归主线程,属其写面)。

---

## 0. 评审输入与新增证据

### 0.1 仓库侧已核

- `docs/contracts/interface-contract.md` §一(E1)/§1.6(prompt 要求)/§四(provider 硬约束:流式禁 max_tokens、会话连续性承载、18789 loopback)。
- `docs/architecture.md` §2 外部依赖表(`DeepSeek(deepseek-v4-flash)| OpenClaw 内部 provider`)、§5.2 场景②(「OpenClaw(每轮透传历史)流式回复」)、§5.3.3 运维面、§5.5.5 prompt 版本化(接线不变式:渲染面==声明面==被测面,ADR-001)、决策 18/19(出网记账含「转写文本 → OpenClaw → DeepSeek 云(deepseek-v4-flash)」)/决策 26。
- `server/deploy/`:`provider.config.yaml`(system_prompt.artifact = v6)、`OPENCLAW_SYSTEM_PROMPT_CARRIER`(v6 声明 + ARQ-260908 槽位预约规则)、`openclaw-patch/`(drift-guard 形态:锚点 grep 不中即 exit 3 拒绝,「STOP on mismatch rather than produce a property-binding diff」)、`DEPLOY-RUNBOOK.md`。
- 先例包 `delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/taskpackage.json`(riskTier medium / gateProfile ["test-quality"] / mutation 三选一形态 / live 真机轮 human 持板)。

### 0.2 ECS 只读核查新增证据(2026-09-24 00:35~00:45 CST,本评审采集)

以下均为只读命令产出;**评审期间未改任何配置、未重启任何服务**。凭证类值(gateway token、qqbot clientSecret、api_key)在核查输出中出现过,一律不收录进本文件。

1. **agent 拓扑**:`openclaw agents list` → 唯一 agent `main (default)`,「Model: deepseek/deepseek-v4-flash」「Routing rules: 0」「Routing: default (no explicit rules)」;`openclaw agents bindings` → 「No routing bindings.」。`agents.entries = {"main": {}}`(空,继承 defaults)。qqbot 通道配置无 agent 绑定字段 → **qqbot 与 voice HTTP 路径共用 main agent,确认**。
2. **模型 catalog**(`models.providers.deepseek.models`):`deepseek-v4-flash` `"reasoning": true`,cost input 0.14/output 0.28,contextWindow 1000000;`deepseek-chat` `"reasoning": false`,cost input 0.28/output 0.42,contextWindow 131072,maxTokens 8192,`compat.maxTokensField: "max_tokens"`;v4-flash 条目**无** `compat.supportsReasoningEffort` 键(deepseek-reasoner 条目显式 `supportsReasoningEffort: false`)。
3. **会话库**:`openclaw sessions list` → 「Sessions listed: 100 of 510 (limit 100)」;昨晚 00:16~00:19 的 3 次基线探针各建了一个独立 session(key 形如 `agent:main:opena...`,单会话 tokens「25k/1000k (2%)」,model deepseek-v4-flash)。agent sqlite 落盘 **93MB**(+4MB WAL)。执行 CLI 期间现场打出 `[sqlite/transaction] slow SQLite transaction hold`——该问题此刻仍在活跃发生。
4. **会话维护能力**:`openclaw sessions` 子命令含 `archive / cleanup / compact`;另有 `openclaw backup`(「Create, verify, and restore backup archives and SQLite snapshots」)可作清理前快照锚。
5. **HTTP 端点**:`gateway.http.endpoints.chatCompletions.enabled: true`,`bind: loopback`,`port: 18789`(V28 断言仍成立)。
6. **cron**:`openclaw cron list` → `kidpal-reminder-plan every 15m`,Last「11m ago」,Status「ok (not delivered)」(announce 投递路由「no route, will fail-closed」为既有形态,提醒实际投递走 pending.json + front 轮询,不经 announce)。
7. **主机**:free → Mem 1870 total / available 761MB;Swap 已用 690MB(基线时 790MB);近 30 分钟 memory pressure warning **10 次**(仍在持续);gateway node 进程已运行 2 天 2 小时,RSS 与基线一致量级。
8. **prompt 注入路径(关键)**:引擎 `.config.yaml` 第 33 行 `prompt: |` 段内嵌 **v6 全文**(data 目录渲染件 `openclaw-system-prompt.v6.md`,22464B,Sep 21 21:55)——即 **v6 system prompt 由引擎随每次请求注入**,不是 OpenClaw workspace 承载。OpenClaw workspace 侧是通用 bootstrap(AGENTS.md 9795B + DREAMS.md 40328B + SOUL/IDENTITY/USER/TOOLS,合计 ~53KB,注入面清单待仪表化确认)。
9. **本评审探针披露(1 次,授权范围内)**:00:41 CST,`POST 127.0.0.1:18789/v1/chat/completions`,body 仅 `{"model":"openclaw/default","stream":true,"messages":[{"role":"user","content":"你好呀"}]}`(**未带 system prompt**),结果 `TTFB=0.037512s TOTAL=6.639012s SIZE=747B`;响应正文逐字:「你好呀,David 🌟 这么晚还没睡?我在的,有什么想聊的或需要帮忙的随时说。」——**无 ```display 块**。session 计数 510 → **511**(本次探针新建 1 个持久会话,已如实计入;这是评审对该系统留下的唯一状态变化)。
10. **版本口径注记**:`openclaw --version` → 「OpenClaw 2026.8.1 (ea80657)」;证据文件 §4 写「OpenClaw 版本 2.0.3(有 2.0.4 可升)」——两套版本号并存(npm 包版本 vs 产品版本),建议证据文件 append 一行勘误注记,避免后续包误判升级面。

### 0.3 由 9 得出的两个即时推论(贯穿全篇裁决)

- **推论甲(探针方法学缺口)**:裸探针不带 v6 system prompt 时,响应无 display 块、行为由 OpenClaw workspace 通用 bootstrap 主导(还带出「David」人设)。**证据文件 §3 的基线探针(13.1/12.5/18.7s)是裸探针**——它证明「大头在 OpenClaw 内部」仍然成立(这点不受影响),但它测的不是生产 prompt 形态;凡用于 A1/A2 判定的探针必须带生产 v6 prompt,否则违反「验收所测 = 生产所用」(carrier 文件原文纪律)。
- **推论乙(方差未受控)**:同配置、同问题「你好呀」,00:16 裸探针 13.1s,00:41 裸探针 **6.6s**——半小时内差 2 倍(期间 swap 已用 790→690MB)。**任何单次探针数字都不得直接当门证据**;A1 必须 N≥3 取中位数、探针前后各记录主机状态快照,且「优化前基线」须当晚同窗口重测(跨夜数字不可比)。

---

## 1. 六个开放问题逐项裁决

### Q1 非推理模型质量回退风险 vs v4-flash 关 thinking 备案——裁决路径是否成立

**判定:路径成立(同意 A/B 探针裁决),附 4 条修改意见,其中第 1 条为硬性。**

1. 【硬性】探针必须带生产 system prompt:messages = [system(仓内 `openclaw-system-prompt.v6.md` 逐字), user(题目)]。理由见推论甲——不带 prompt 的探针对 A2(display 块纪律、备注读回)完全不可判,对 A1 测的也是非生产形态。prompt 源用仓内 v6 文件(单一来源),不用 ECS 渲染件。
2. 备案 B(v4-flash 关 thinking)**可行性未证实**:catalog 里 v4-flash 条目 `reasoning: true` 且无 `supportsReasoningEffort` 兼容键;请求级参数能否穿透 OpenClaw 到 DeepSeek 未知。给 B 案设 **30 分钟 timebox**(查 OpenClaw models 配置能力 / 一次带备份的 catalog 试验),到时未证实即放弃 B、直接走 A。不要让 B 案烧掉当晚窗口。
3. A2 守卫从「抽检」升级为**全量判定 + 明确回滚触发**:每批探针全部运行计 display 块出现率与 schema 合法率,**要求 100%**(front V27 会机械兜底脏块,但兜底 = 体验已坏,不能算过);天气题答复必须含真实检索数据(tavily 轮成功的证据,防非推理模型工具调用行为变化导致幻觉);课表题必须含备注读回(S-F1.3 AC1/AC4);语音 ≤3 句纪律抽检。**回滚触发:切 A 后 display 块合法率 <100%(N 次中任一次缺块/脏块)→ 当晚回滚**,把数据留给晨间 human。
4. 质量守卫补两条共 agent 面(见 Q2):家长 QQ 录入回归 1 次 + 提醒语义回合(semantic.json 产物形状)冒烟 1 次——这两条跑在同一个 main agent、同一个新模型上,是 A 案的隐性回退面。

补充记账(不阻塞):deepseek-chat 单价 input 0.28/output 0.42 vs v4-flash 0.14/0.28(catalog cost 字段),约 2x/1.5x;单设备量级下绝对成本大概率微小,但应进晨间 decision packet 一行。

### Q2 qqbot 通道与 voice 共用 agents.defaults 的隔离问题

**判定:确认共用;当晚不做结构隔离,以回归检查代偿(修改草案的「要不要隔离」为「不隔离 + 代偿 + 记 k2 候选」)。**

- 事实:`agents list` 唯一 main agent、零 routing 规则、qqbot 无绑定字段;voice HTTP 会话键 `agent:main:openai...`、qqbot 会话键 `agent:main:qqbot_direct_...`(import-archive 文件名可见)——**两通道同 agent、同 model 配置,改 `agents.defaults.model.primary` 必然同时切 qqbot**。
- 隔离能力存在但成本不对称:`openclaw agents add/bind` 支持多 agent + 路由绑定,但第二 agent = 独立 workspace/auth,而 schedule.md、stories、memory、提醒 cron、semantic.json 管线全部长在 main workspace(`/root/.openclaw/workspace/kidpal/`)上;拆 agent 要么复制数据面、要么配共享——结构成本远超一晚运维窗口,且家长录入路径**质量敏感、时延不敏感**(QQ 异步,慢几秒无感),与 voice 的诉求正交。
- 代偿(当晚/晨间):A2 批次加 QQ 录入回归(human 晨间用 QQ 录一条测试安排,验 MD 改写 + 三档重算触发 + 提醒文案质量);提醒 cron 语义回合冒烟(查 semantic.json 新产物形状合法)。
- 记账:若 QQ 侧实测质量回退 → k2 候选「per-channel 模型策略或 agents bind 隔离」,届时先查 OpenClaw config schema 有无通道级 model override(本评审未证实有无);另一未探明杠杆:`model=openclaw/default` 的名称路由语义(能否 `openclaw/<agent>` 指到不同 agent/模型)留 k2 调研。

### Q3 P2(prompt v7 + 会话治理)入本包还是拆后续包

**判定:拆(反对入本包)。本包 = P0 + P1 + 仅测量面的 preflight 仪表化;P2 治理动作全部拆 k2,携当晚数据立项。**

理由:①归因卫生——一晚只动一个变量(模型),prompt v7 与会话治理同夜上会把 A1/A3 数字搅浑,晨间 human 无从判断哪刀见效;②v7 有版本化工件的完整纪律成本(ARQ-260908 槽位预约、冻结齿重锚、mutation 证据、v6 基座逐字保留可 diff),不适合塞进运维型当晚包;③会话治理的正确靶点本评审已发现与草案假设不同(见 §5 D2),需要先仪表化再动手——草案自己写的「不盲改」纪律,适用于此。
例外(条件步,非必做):`openclaw sessions archive/cleanup` 属运维卫生而非结构改动——**仅当** P0 重启后 slow SQLite hold 仍频繁出现时,作为独立归因步执行(先 `openclaw backup` 快照,再清理,再复测探针,三段数字分开记)。

### Q4 preflight 仪表化是否升级必做

**判定:升级为条件必做 + 收窄靶点(同意草案方向,补假设清单)。**

- 触发条件(满足任一即必做,当晚做,只测量不改):P1 落地后 A1 保底门(见 §4)红;或 P0 重启后 gateway 日志 preflight 仍 >2.5s;或 slow SQLite hold 在重启后 30 分钟内再现。
- 仪表化应验证的假设清单(本评审给出的靶点,替代盲拆):preflight 2.7~6.1s ≈ **每请求新建 session + 静态上下文装配(~25k tokens:引擎注入 v6 prompt 22.5KB + workspace bootstrap ~53KB 中实际注入部分 + 工具 schema)+ SQLite 写(swap 抖动放大)**。重点拆:session create / context assemble / first upstream POST 三段时戳;并记录 workspace 文件注入清单(DREAMS.md 40KB 是否进上下文,直接决定 k2 的瘦身面)。
- 结构性注记:HTTP 路径**每请求一个全新 session**(探针实证 510→511),意味着 preflight 的 session 装载成本是**每轮都付**的,不是「会话越长越慢」——这与草案 P2 的假设相反(见 D2),仪表化数据将决定 k2 的治理面是「静态上下文瘦身 + store 维护」还是别的。

### Q5 验收门数字校准(A1 探针门 / A3 引擎日志门)

**判定:数字需修(草案自认「拍的」,现给出校准依据与两层门方案)。**

- 内部不一致(必须修):草案 A1 闲聊 ≤4s、A3 闲聊 ≤5s,只给 TTS 首句留 1s;而证据文件实测「`发送第一段语音`→`语音生成成功` 1.2~2.0s(EdgeTTS 首句合成)」。对齐公式:**A3 阈值 = A1 阈值 + [1.5, 2.5]s**(TTS 首句 + 引擎转发开销 ~0.1s + 余量)。
- 方差(必须修):推论乙——同配置半小时内 13.1s vs 6.6s。所有门 = **N≥3 中位数**,探针前后记录 `free -m` + memory pressure warning 计数 + session store 计数,且当晚重测基线配置数字作对照锚(跨夜数字禁止直接对比)。
- 校准后的两层门建议(基于:非推理模型单轮生成 1~2s、preflight 重启后未知但当前 2.7~6.1s、课表/天气各 3 轮上游、tavily 每轮 1~3s):
  - **A1 保底门(当晚必须达标,否则回滚或留数据晨判)**:闲聊 ≤6s、课表 ≤9s、天气 ≤14s(中位数,带 v6 prompt)。
  - **A1 目标门(达标 = 当晚收工;未达 = P2/k2 立项依据,不算当晚失败)**:闲聊 ≤4s、课表 ≤6s、天气 ≤10s(即草案原数字,降级为目标层)。
  - **A3(晨间真机,human 持板)**:保底 闲聊 ≤8s、课表 ≤10s;目标 闲聊 ≤5s、课表 ≤7s(草案原数字)。真机轮尽量安排在引擎重启后**会话早段**(引擎侧历史随轮次增长,是 A3 的系统性混淆因子,见 D2)。
  - A1 天气门附加条件:答复含真实检索数据(tavily 轮成功),否则计时再好也判红。
- 依据链:生产闲聊基线 7.3~16.3s(证据 §1「Thank you ~7.3s(1 次上游调用)」至「嗯嗯嗯 16.3s」),其中生成 ~9s(证据 §2「单次生成耗时 ≈9s(一句话回复)」)→ 换非推理模型省 7~8s → 7.3−7≈0.3 至 16.3−8≈8.3s 区间,保底 6s/目标 4s 落在此区间内且给 preflight 留了改善要求。

### Q6 探针会话痕迹污染 default agent 历史的隔离必要性

**判定:草案担心的污染形态不存在,真实的污染形态是另一回事(修改问题本身)。**

- 实证:HTTP chatCompletions **每请求新建独立 session**(3 次基线探针 = 3 个不同 session id;本评审探针 session 计数 510→511)。设备语音连续性靠**引擎侧每轮透传历史**(arch §5.2 场景②原文:「OpenClaw(每轮透传历史)流式回复」),不靠 OpenClaw 侧长驻会话——**探针不会写进儿童会话历史,「独立探针会话隔离」无必要**(天然隔离)。
- 真实污染 = **store 膨胀**:每次探针留一个 ~25k token 的持久 session 行,511 个会话 / 93MB sqlite,而 slow SQLite hold 正在活跃发生——探针批次本身在加剧它要测量的问题。对策:①探针脚本每批结束记录 session 计数进证据;②批次后对探针新建 session 跑 `openclaw sessions archive`(先 `openclaw backup` 快照);③k2 评估 HTTP 端点 session 保留策略(retention/cleanup 定时)。
- 注:儿童语音每轮同样新建 session(每轮 ~25k+ token 行)——store 膨胀是生产固有速率,不只是探针问题;这是 D2 的一部分。

---

## 2. 契约影响判定

**判定:不触 `docs/contracts/**`(同意草案预判)。依据逐条:**

1. **§四 shim API/端点约束零变化**:引擎 `.config.yaml` LLM 段不动(实测现状 `type: openai / base_url: http://127.0.0.1:18789/v1 / model_name: openclaw/default`,与契约「base_url 带 /v1、model_name openclaw/default」一致);「流式请求禁带 `max_tokens`」约束的对象是引擎→OpenClaw 请求,本包不碰;OpenClaw→DeepSeek 的 `compat.maxTokensField: "max_tokens"` 是另一段链路,不在契约约束面内。
2. **E1 display 统一响应块零变化**:块格式、view 白名单、薄载荷纪律全部由 system prompt(v6,仓内版本化工件)+ front 机械校验(V27)承载,契约文本不钉底层 DeepSeek 型号;契约 §1.6 是对 prompt 内容的要求,换模型不改契约文字。模型切换只改变「谁来满足 §1.6」,满足与否由 A2 守卫实测——这正好落在契约「待 OpenClaw 确认点 1」(「能否稳定按 system prompt 输出 ```display 块……须复验」)的复验义务里,A2 全量判定即履行该义务。
3. 无新消息类型、无协议语义改动、firmware 零触碰、front 零触碰。
4. **两项非契约记账义务(触发,须入包 exitCriteria)**:
   - architecture.md 漂移:§2 依赖表「DeepSeek(deepseek-v4-flash)」与决策 19 出网记账「转写文本 → OpenClaw → DeepSeek 云(deepseek-v4-flash)」钉了现型号。切换若成为生产常态 → `docsSyncRequired: true`(改 arch 两处型号名,出网路径实质不变——转写文本仍出至 DeepSeek 云,记账实质无变化,属名称同步)或立 ARQ 记账,二选一,不许静默漂移。
   - 若 k2 落 prompt v7:版本化工件纪律(槽位预约、v6 逐字保留、冻结齿重锚),仍不触契约,但 v7 必须保住 §1.6/薄载荷/备注读回锚语义(K3 包冻结齿所钉)。

---

## 3. 路由与量级

**判定:Track B dev taskpackage 成立;riskTier = medium(确认草案候选);不升级 Track A。**

- 升级判据逐条:①契约面——不触(§2 已裁);②跨系统结构性变更——不满足:改动面 = ECS 运维动作 + `server/deploy/` 声明工件,协议语义零改动、固件零改动、引擎配置零改动,「跨系统但非结构性」的草案自评成立。
- riskTier medium 依据:改的是**在产儿童服务**的模型配置(质量回退面真实存在)+ 共 agent 波及家长通道;但一步备份回滚、无代码改动进引擎、live 面有人持板晨检——不到 high。对齐先例 K3(medium / gateProfile ["test-quality"] / mutation 必做 / live 轮 human 持板)。
- gateProfile 建议:`["test-quality"]` + live 证据轮(晨间 A3),live 面沿用 `@pytest.mark.live` + `RUN_LIVE=1` 纪律(G3)。
- 包内工件形态建议:模型/白名单声明 artifact + 幂等 apply 脚本沿 openclaw-patch 既有纪律——**drift-guard 必配**(apply 前 grep/jq 锚点核对现状,不匹配即拒绝执行,照抄 `make_openclaw_patch.py` 的「REFUSES (exit 3)」形态),备份用时间戳文件 + 回滚一步脚本。**仓内齿只断言机械面**(声明文件在位、apply 脚本幂等、锚点核对逻辑、备份/回滚脚本存在且可 dry-run);**时延数字不做 CI 齿**(timing 齿天然 flaky),时延门 = live 证据记录 + integrator/human 按冻结阈值判定——这也修正草案「mutation 自证:验收脚本先 RED 再 GREEN」的形态:mutation 应打在配置接线上(删声明文件/改错 model id/删 allow 条目 → 齿红),不是打在超时判定上。
- 夜间 ECS 运维动作(重启、改配置)已获 human 授权(任务书口径),dev-loop 执行时逐步留痕即可,无需再升级。

---

## 4. P0~P3 分步 / A1~A4 / 回滚设计评审

- **P0(同意,补核验清单)**:重启 gateway 前后各跑一次探针批(带 v6 prompt、N=3、记录主机状态)——三点归因(重启前/重启后/换模型后)才拆得出「主机劣化」分量。重启后**五项核验**:①`openclaw cron list` 中 `kidpal-reminder-plan` 在册且 Next 正常(提醒是安全相关面,不许被重启静默弄丢);②qqbot 通道重连(5 分钟周期内);③18789 仍锁 loopback(V28 部署期断言);④char-stroke dist 补丁 marker 在位(runbook 校验点,补丁在磁盘上、重启不应丢,但要核);⑤引擎容器对 18789 可达。注意:**重启不清 93MB session store**(持久化在盘),对 store 膨胀无效——别把重启当 P2 的替代。
- **P0 引擎 docker 重启(同意为可选,建议做)**:放晨间真机轮开窗之前,清引擎侧累积历史,给 A3 一个干净起点;真机轮排会话早段(历史增长是 A3 混淆因子)。
- **P1(同意为主杠杆,方法学按 Q1 修正)**:apply 走仓内声明 + 幂等脚本 + drift-guard(见 §3);A/B 裁决带 v6 prompt;B 案 30 分钟 timebox;A2 全量判定 + 回滚触发线。「落仓防漂移」一段判断正确且重要——`openclaw.json` 的模型选择现状确实不在仓内跟踪(patch 链只管 char-stroke dist),声明化是正解。
- **P2(同意方向,拆 k2,靶点按 D2 修正)**:草案「OpenClaw 侧历史只增不减 → compaction」的假设与实证不符(每请求独立 session,不存在单会话无限增长);k2 的正确靶点候选 = ①静态上下文瘦身(~25k tokens/请求:workspace bootstrap 注入清单 + v6 prompt 22.5KB)、②session store 维护策略(archive/cleanup/retention,511 会话 93MB 且在涨)、③课表 3 轮工具循环收敛(prompt v7 或工具配置;注意 v6 自身「一回合作答:不调用任何工具、不读取任何文件」与「读 schedule.md(契约 §二 B1)筛选后」存在内部张力,3 轮可能就是该张力的产物——v7 应显式化「课表题允许且仅允许一次读文件」类纪律)、④引擎侧历史增长治理。全部待 Q4 仪表化数据定型。
- **P3(同意,decision packet 化)**:4GB 升级 = 停机 resize,排白天窗口;packet 里附本评审数据(available 761MB、swap 690MB、30 分钟 10 次 pressure warning、同题 2 倍方差)与替代/过渡项(每夜 gateway 重启 cron、sessions 维护定时)。OpenClaw 2.0.4 升级维持「当晚不动」(同意),但入晨间决策点:升级须过 runbook §0 版本漂移闸(dist 锚点核对,char-stroke 补丁面)。
- **A4 回滚验证(同意,补一层)**:openclaw.json 备份还原 + 探针复测之外,补 `openclaw backup` 的 SQLite 快照作为「清理类动作」的专属回滚锚(若条件步 sessions archive/cleanup 执行了的话);回滚后核验五项同 P0。
- **A2/A3 口径**:A2 按 Q1-3 升级;A3 按 Q5 对齐公式改数字,真机轮排早段。

---

## 5. 结构性缺陷与替代方案(输出要求③)

草案总体成立(杠杆排序 P0>P1>P2>P3 与证据吻合),但有 4 处需修正,其中 D1 是硬缺陷:

- **D1 探针方法学缺陷(硬)**:基线与未来 A1/A2 探针若不带生产 v6 system prompt,则测的不是生产形态——实证:裸探针响应无 display 块、唤「David」人设(§0.2-9)。修正:验收探针脚本固定 messages = [system(v6 仓内工件逐字), user(题)],并在脚本里对 system prompt 做 hash 断言(渲染面==被测面,ADR-001 精神)。
- **D2 会话增长轴误诊**:草案假设「OpenClaw 侧历史只增不减(单会话)」。实证:HTTP 每请求新建独立 session(510→511),儿童多轮连续性靠引擎透传历史;增长轴实为 **引擎侧历史(轮间增长,重启清零)+ OpenClaw session store 行数(持久增长,511 行/93MB)**。compaction 杠杆据此重定向(见 P2 修正)。替代方案不需要——这是诊断修正,不是路线推翻。
- **D3 门数字内部不一致 + 方差未受控**:A1/A3 留白 1s < TTS 首句实测 1.2~2.0s;单次探针方差 2 倍。修正 = 两层门 + 对齐公式 + N=3 中位数 + 主机状态快照 + 当晚基线重测(Q5)。
- **D4 应急预案(若 A 案质量失败且 B 案不可行)**:保底 = 维持 v4-flash 现状,当晚只留 P0 收益(重启分量),把全部火力转 k2(上下文瘦身 + 轮次收敛 + store 维护 + RAM 升级)——这些杠杆攻击 prefill/preflight/轮次而非单轮生成,量级上同样能把闲聊从 9s 生成压下来一部分,只是天花板低于换模型。此路径写进包内 contingency,避免当晚现场临时决策。
- **顺带观察(不入本包,记 k2 产品面)**:裸探针被唤「David」说明 workspace USER.md/memory 的人设注入面向所有会话——儿童语音生产会话是否也被注入「David」语境,值得 k2 产品质检查一眼(本评审未读 USER.md 内容,不下结论)。

---

## 6. 今晚最小可执行集(排序,输出要求①)

1. **证据与回滚锚冻结**:主机状态快照(free/vmstat/pressure 计数)+ `openclaw sessions list --limit all` 计数(当前 511)+ `openclaw backup create`(SQLite 快照)+ `openclaw.json` 时间戳备份。证据文件 append(不覆盖)。
2. **验收探针脚本落仓**(server/acceptance 或 server/tests,带 v6 prompt 注入 + hash 断言 + N=3 中位数 + 计时输出),先跑**基线配置重测**(同夜对照锚,D1/推论乙修正)。
3. **P0 重启 gateway** → 五项核验(cron/qqbot/loopback/dist marker/引擎可达)→ 探针复测(归因第二段)。
4. **P1 声明工件 + 幂等 apply 脚本落仓**(drift-guard:现状 jq 锚点核对不中即拒绝;备份+一步回滚)。
5. **B 案 timebox(≤30min)**:证实 v4-flash 可关 thinking 则 A/B 都测;否则直接 A 案(allow 增补 deepseek-chat + primary 改指)。
6. **A 案探针批**:标准 3 题 ×3(带 v6 prompt)+ A2 全量质量判定(display 块 100%/备注读回/天气真实数据/≤3 句)。触发回滚线 → 回滚留数据。
7. **(条件步)slow SQLite 仍频繁** → `sessions archive/cleanup`(backup 快照在先)+ 探针复测(归因第三段,分开记)。
8. **引擎 docker 重启**(晨间真机轮开窗前)+ 全部证据整理 append + 晨间决策包成文。

## 7. 晨间 human 决策点(输出要求②)

1. **A3 真机轮(human 持板,会话早段)**:闲聊/课表/天气计时对两层门;三源日志取证(串口/front systemd/引擎 docker logs)。
2. **共 agent 回归**:QQ 录入 1 条测试安排(MD 改写 + 三档重算触发 + 提醒文案质量)+ 提醒 semantic.json 新产物形状抽查。
3. **模型定案拍板**:A/B 案批准为生产常态(附成本注记:deepseek-chat input 2x/output 1.5x)+ architecture.md 决策 19/依赖表型号名 docsSync 或 ARQ 记账二选一。
4. **P3 拍板**:4GB 升级 decision packet(停机窗口排期)+ 过渡项(每夜重启 cron / sessions 维护定时)取舍。
5. **P2/k2 立项**:携当晚仪表化数据(preflight 三段拆解 + workspace 注入清单 + store 增长速率),范围候选 = 静态上下文瘦身 / store retention / 课表轮次收敛(prompt v7,槽位预约先行)/ 引擎侧历史治理 / 「David」人设注入质检。
6. **OpenClaw 2.0.4 升级窗口**:须过 runbook §0 版本漂移闸(char-stroke dist 补丁锚点),与 P3 停机窗口可合并。

---

*评审留痕:本评审全程只读 + 1 次授权探针(§0.2-9 披露,副作用 = session store +1 行);未改任何配置、未重启任何服务、未写 ECS 任何持久文件(探针 SSE 落 /tmp 后即删)。凭证值不入档。*
