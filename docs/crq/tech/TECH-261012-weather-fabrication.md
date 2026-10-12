---
id: TECH-261012-weather-fabrication
title: 真机实测：孩子问天气 LLM 凭印象编造整套预报（晴到多云 26 度）且虚称「我去查一下」——v9 提示词反编造纪律逐域枚举漏「通用实时事实」域，「孩子对话回合」禁工具 + 无数据源结构性逼出编造
domain: tech
status: blocker
severity: MAJOR
owner: 主线程 scoping → /triage-change 立项（kidpal 环境强化批）
trigger: human 立项 ack 后走 /triage-change 分流（预判 ②单包，跨 prompt v10 + OpenClaw kidpal 环境两面）
opened: 2026-10-12
closed: ""
closedBy: ""
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
