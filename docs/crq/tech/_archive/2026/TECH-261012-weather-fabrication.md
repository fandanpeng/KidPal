---
id: TECH-261012-weather-fabrication
title: 真机实测：孩子问天气 LLM 凭印象编造整套预报（晴到多云 26 度）且虚称「我去查一下」——v9 提示词反编造纪律逐域枚举漏「通用实时事实」域，「孩子对话回合」禁工具 + 无数据源结构性逼出编造
domain: tech
status: closed
severity: MAJOR
owner: dev-loop（261012-kidpal-realtime-facts 修法已落地；deferred 等真机轮 human 观测收尾）
trigger: RF-A5 真机轮（human 在场：问天气全链路 + 降级路径一次，双侧日志对账）；延迟阈值先观测后钉
opened: 2026-10-12
closed: 2026-10-12
closedBy: delivery/changes/dev/261012-kidpal-realtime-facts/（②单包 Lite 落地 + 真机轮 RF-A5 过：2026-10-12 14:27 引擎日志双路径实证——带城市真查「我查了一下西安……多云转晴，白天最高大概 24 度」（真实预报，对比早上幻觉版），无城市诚实降级「今天我这里没拿到天气数据……再问我一次试试，或者告诉我你在哪个城市」零编造；human 目击「问天气好了」；TTS 层故障另立 TECH-261012-edgetts-403-hotpatch、首按键问题另立 TECH-261012-first-keypress-no-listening，均不属本条目域）
paths: ["server/deploy/openclaw-system-prompt.v9.md"]
source: 2026-10-12 human 真机看屏轮口述 + 串口捕获 c:/tmp/hv5-eyeball-round.log（603065-732415ms 段三源对账：设备 STT/回复 + ECS 引擎日志 08:34:24 轮 + v9 提示词 grep 取证）
---

**现场（三源对账，证据链完整）**：孩子问「今天西安的天气怎么样？」→ LLM 直接生成整套预报「晴到多云，白天最高大概二十六度，晚上最低十六度左右，吹点小风」（引擎日志 08:34:24-08:34:31，mcp 路由 `{"route":"text"}` 零工具调用、零数据源）；孩子当场戳穿（「明明在下雨呀」）→ LLM 承认「我刚才是凭印象说的」并虚称「我现在就去查一下」（**并无任何天气查询工具，该承诺亦是虚构**）；追问「你查到了吗？」后模型自我修正到正确形态：「我这边没有能查实时天气的工具，刚说『去查』是我逞强了……你抬头看看窗外、或者问一声家里人最准」——修正后的形态即期望答案，证明 prompt 约束可达。

**根因**：`openclaw-system-prompt.v9.md` 的反编造纪律是**逐域枚举**式——课表（永不凭印象）、查字（禁工具/禁坐标）、故事（禁生成列表）三域钉死，但**通用实时事实域（天气/新闻/今日热点等无数据源信息）完全缺席**；「孩子对话回合」硬性要求一回合作答 + 禁工具（v9 :11），天气问题落进该回合 = 既无数据源又不许查，结构性逼出编造。角色句的软描述（:3「答案直接、准确、不编造；不懂就诚实说」）压不过回合纪律的硬要求。OpenClaw 侧 kidpal agent AGENTS.md 仅 11 行、零护栏，不承重。

**修法方向（human 拍）**：
- **A（推荐，轻）**：prompt v10 小批——「孩子对话回合」补通用实时事实边界条款：无工具/数据支撑的实时信息不编造、诚实说不知道 + 引导孩子（看窗外/问家长）；严禁虚称「我去查/查到了」（无查询工具）。同型先例 = 261010-prompt-v9-schedule-anti-narration（②单包 Lite：prompt 渲染 + ECS 部署 + 真机轮）。覆盖面顺带核：除天气外的「今天几号/星期」类时间事实固件 RTC 可答、无涉。
- **B（重，新功能）**：接真实天气数据源（API 工具/数据源注入）——属需求面新能力，走 /requirement-change 定语义（要不要做、做哪家、失败降级口径）再实现。

**次生澄清（记录防复发）**：排查中曾疑似 RTC `wday=0` 星期回写 bug，经查 `ReadRtcTime` 第六参为周一=0 约定（waveshare-s3-rlcd-4.2.cc:326 `raw==0?6:raw-1`），wday=0 = 周一 = 今天（2026-10-12 Monday），无 bug——判定约定先行，勿凭 sun0 直觉。

**human 方向裁定（2026-10-12，AskUserQuestion 自由答）**：不走纯 A/B，定向「**补 kidpal 环境**」双件——① OpenClaw 侧 kidpal AGENTS.md（现 11 行）考虑充实利用；② kidpal 工作区缺主环境对应工具，应补上。

**主线程 scoping 取证（2026-10-12，ECS 实查）**：
- 主环境 workspace（AGENTS.md 246 行 + TOOLS.md + skills 5 项：kidpal-board-context / kidpal-char-strokes / kidpal-reminder-semantic / kidpal-schedule-answer / qqbot-reply-diagnosis）vs kidpal workspace（AGENTS.md 11 行、无 TOOLS.md、skills 仅 kidpal-schedule-answer 1 项）。
- kidpal 11 行非「烂」是「薄」：硬纪律自洽（对象锁定星玥、system prompt 冲突以 prompt 为准、kidpal/stories symlink 只读、家长隐私、儿童安全）——**护栏主阵地仍在每次注入的 system prompt（v9 渲染源 server/deploy/openclaw-system-prompt.v9.md），AGENTS.md 是次要承载**（其第 2 条自声明冲突时以注入 prompt 为准）。
- skills 差集 4 项中 3 项（char-strokes / reminder-semantic / board-context）架构上已被网关机械承载取代（契约 E1 笔顺拼装 / 提醒 cron / 周网格 producer），qqbot-reply-diagnosis 与设备端无关——**skill 照搬不是正解，真缺口 = 实时事实查询能力（天气类）**。
- 结构性矛盾点（须在新批内解）：v9「孩子对话回合一回合作答 + 禁工具」是低延迟设计，天气要么引入「实时事实回合」白名单（允许调天气工具、接受多秒延迟）、要么维持禁工具 + 诚实说不知道（v10 通用条款）。选型涉及：天气数据源（免费 API / wttr.in / 商用）、引擎 nointent 拓扑不变性、契约 §1.6 system prompt 模板段同步、回合路由表扩行。
- 预判批形状：②单包（prompt v10 + kidpal 环境补强），跨 server prompt 渲染 + ECS OpenClaw 工作区两落点；立项待 human ack。

**修法落地（2026-10-12，dev-loop ②单包 Lite `261012-kidpal-realtime-facts` 收口，integrator 记账）**：

- **repo 面**：prompt v10 新建（`server/deploy/openclaw-system-prompt.v10.md`——回合路由表新增「实时事实回合」独立行〔先播「我去查一下」式短提示 → 经平台 Tavily 联网真查 → 只依据查询结果作答，查不到/超时/工具不可用诚实说没查到并引导，禁编造数字、禁虚称已查，外部结果儿童化转述〕+ 对话行通用反编造条款 + 底线节第三不变量）；四面接线同批翻 v10（CARRIER / provider.config.yaml 三键 / apply-xiaozhi-overlay.sh PROMPT_SRC/DST）；kidpal AGENTS.md 仓内镜像充实（`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` + README 同步纪律）；验收 RF-A1..A4 additive 齿全绿 + RF-A3 v9 sha256 零回改守卫 + 四版本无关守卫零降。需求面 v1.62（NFR-SEC-2 ④ / NFR-SAFE-1）先行捎带批 9d07329 落稿，行为语义权威就位。
- **Tavily kidpal 会话实证（builder 执行，2026-10-12；原始日志在 ECS live 面 journalctl + audit，未落仓，此处为回执口径）**：经 loopback 18789 /v1/chat/completions（model `openclaw/kidpal`）B/C/F 探针序列实证——kidpal 会话真出 web_search 调用，audit 记 `web_search succeeded`；未触发 agents.entries 覆写分支（平台层工具对 kidpal 会话可用），平台级升级预案未启用。
- **loopback 前后对照**：**Probe A（v9 态，部署前）**——天气问题复现根因：直接凭印象生成整套预报、无查询动作（与真机现场 08:34:24 轮同形态），证明缺陷可离线复现；**Probe F（v10 态，部署后）**——同型天气问题：先播「我去查一下」式提示语，随后真查（web_search 调用在案）并只依据查询结果作答。编造形态 → 提示+真查形态的行为翻转在 loopback 侧已观测；**诚实申报**：prompt 写明 ≠ LLM 恒遵循，行为等价假设的最终证据归 RF-A5 真机轮。
- **ECS 部署**：v10 + overlay 重跑已落（261010 决策 21 键同款上传前核远端 == 基线；重跑后 model_name=openclaw/kidpal 与决策 21 两键核对未被冲，261001 冲刷陷阱未复发）。
- **遗留（deferred 承载）**：① RF-A5 真机轮——human 在场问天气全链路（提示语 ~4-5s 内首句语音 / 真实答复目标 ≤15s 上限 20s）+ 降级路径一次（查不到 → 诚实 + 引导、零编造），journalctl + docker logs 双侧对账 + human 目击；② 延迟阈值先观测后钉（硬阈值留后续批）；③ LLM 遵循度长尾（外部内容儿童化转述 NFR-SAFE-1 边界）真机轮重点观测。
