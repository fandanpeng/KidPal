# Ops Record — OpenClaw 家庭用户映射 + 设备端 kidpal agent 隔离（260924）

> **性质**：human 直授的一次性生产运维操作（**跳过 dev-loop，如实记录**）。执行人 = 主线程 Claude（经 `c:/tmp/ssh_helper.py` 直连 ECS），批准人 = David。执行窗口 2026-09-24 15:02–15:30 CST。本记录不是 TaskPackage（无 taskpackage.json / 验收链），留档供 k2 立项材料引用与后续追溯。
>
> **本记录不含任何 secret 值**；涉及凭据处一律 `<masked>`。

## 一、背景与问题

1. ECS 上的 OpenClaw（2026.8.1，`openclaw-gateway.service`，loopback `127.0.0.1:18789`）此前为**单 agent `main`**（星星人设），两条腿共用：家长 QQ 私聊 + 星星的设备语音链路（引擎 `POST /v1/chat/completions`，model `openclaw/default`）。
2. **「User 问题」**：main workspace 的 `USER.md` 全局宣称「你的人类是 David」，设备（孩子）会话同样加载该 bootstrap——perf-k1（260924-perf-k1-roundtrip-latency）实锤的「David 人设渗漏」根因（孩子会话回「你好呀，David」+ 丢 display 块，双峰随机），k2 已升最高优先。
3. **妈妈渠道缺失**：QQ 渠道为官方 QQ bot 单账号，`dmPolicy: allowlist` 且 `allowFrom` 只有 David 的 openid——妈妈私聊会被静默丢弃；且 `groupAllowFrom: ["*"]` 群口全开（bot 被拉进任何群都可对话，带 coding 工具档）。
4. **需求**：两个 QQ 渠道（爸爸 David / 星星妈妈）分别对应两个 QQ bot 账号；家长转发学校通知 → OpenClaw 按 B1 录入 `schedule.md` → 设备端显示与播报（既有提醒链路，不动契约面）。

## 二、目标映射（已实现）

| 谁 | 入口 | Agent | 身份识别 |
|---|---|---|---|
| 孩子星玥（小名星星，8 岁） | 设备 → 引擎 → `/v1`，model **`openclaw/kidpal`** | **`kidpal`**（新建，干净 workspace） | v6「小伴」人设由引擎逐请求注入；workspace 无任何家长隐私文件 |
| 爸爸 David | dad bot（现有 appId）私聊 | `main`（星星，家庭管家） | **account 级映射**：`accounts.dad` = 爸爸；allowlist 只含其 openid |
| 星星妈妈 | mom bot（第二个 appId，**凭据待补**）私聊 | `main` | `accounts.mom` = 妈妈；`enabled:false` 占位中 |

路由依据（读 dist 源码核实，同时回答了 perf-k1 遗留的「openclaw/default 名称路由语义调研」）：HTTP model 名 `openclaw/<agentId>` 直达对应 agent（`resolveAgentIdFromModel`，`dist/http-utils-Buw61ytF.js`）；QQ 渠道经 `agents add` 自动落的 binding `qqbot * → main` 恒落 main；QQ 两位家长因 `session.dmScope: per-channel-peer` 天然分会话。

## 三、变更清单与证据

### 1. 新建 kidpal agent（设备端隔离）

- `openclaw agents add kidpal --workspace /root/.openclaw/workspace-kidpal --model deepseek/deepseek-v4-flash --non-interactive`。
- **数据单源**：`workspace-kidpal/kidpal` → `/root/.openclaw/workspace/kidpal`、`workspace-kidpal/stories` → `/root/.openclaw/workspace/stories`（均 symlink，真身位置不动 → cron `kidpal-reminder-plan` 与 front 的路径 env 零改动）。写穿透验证：真身追加标记行 → symlink 侧读到 → 删除还原，✓。
- **干净 bootstrap**：`agents add` 自动种下的整套模板（AGENTS.md 7.9KB / BOOTSTRAP.md / SOUL.md / USER.md / IDENTITY.md）全部移入 `.trash-seed/`，替换为两份精简文件——IDENTITY.md（小伴 🌟，陪伴星玥）+ AGENTS.md（5 条硬纪律：①绝不称孩子 David，孩子叫星玥/星星；②与注入 system prompt 冲突时以注入为准；③kidpal/、stories/ 只读；④不引用家长隐私；⑤回复不出现秘密/技术词/路径）。设备请求的静态上下文由 ~25k tokens（v6 22.5KB + 主 workspace bootstrap 含 DREAMS.md 42KB）大幅瘦身（k2「静态上下文瘦身」的设备侧大头就此消除）。

### 2. 引擎切换 model_name

- live 配置真身（`docker inspect` 确认挂载，防 decoy）：`/opt/kidpal/xiaozhi-esp32-server/deploy/xiaozhi-docker-data/.config.yaml` 第 23 行 `model_name: openclaw/default → openclaw/kidpal`（只改此一值），`docker restart xiaozhi-esp32-server`。
- 验证：启动日志 `初始化组件: llm成功 OpenClawOpenAI`、`intent成功 nointent`、ASR/TTS 正常。

### 3. 设备路径探针（5 次，kidpal agent 会话 +5，如实计数）

| 探针 | 结果 |
|---|---|
| 裸「你好呀」（此前 David 渗漏的实锤场景） | 「你好呀！🌟 我是小伴～」——**无 David** ✓ |
| 带引擎内 v6 prompt「今天有什么课」 | 有合法 ```display 块、无 David、无 `<details>` ✓（未读课表而是引导说「帮我显示课表」——v6「一回合作答不读文件」与课表问答要读文件的既有内部张力，归 prompt v7，非本次引入） |
| 带 v6「帮我看看课表文件，明天有什么课？要带什么？」 | **symlink 读课表全对**：周五课程逐项 ✓、备注空不发明带物 ✓、主动读出当天 18:30 编程课「提前15分钟准备」→18:15 坐好 ✓ |
| 流式「你好」（生产同形） | 1.0s，「你好呀，星星！🌟 我是小伴」 ✓ |
| 非流式「你好」 | 3.9s ✓（期间一次瞬时 "No response from OpenClaw."，网关重启后冷启动抖动，双模复测未复现） |

### 4. QQ 渠道迁移多账号 + 收紧群口

- `channels.qqbot` 由平铺单账号迁为 `accounts` 结构：`accounts.dad`（name「爸爸的QQ」，appId/clientSecret/allowFrom 经 jq 服务器内原样迁移、全程未出显，dmPolicy allowlist + groupPolicy disabled）+ `accounts.mom`（name「妈妈的QQ」，`enabled:false`，allowFrom=`["PENDING-MOM-OPENID"]` 占位）。
- **插件运行时校验比内嵌 schema 严**（首迁被拒、网关短暂宕机 ~1 分钟，自动修复后恢复）：①顶层必须保留 `allowFrom`；②allowFrom 不允许空数组。修复 = 顶层 allowFrom 复制 dad 白名单 + mom 占位符。网关启动期的 "Tencent QQBot 2.0" 自动迁移另移除了 `defaultAccount`（dad 居首即默认，接受其语义）。
- 顶层最终态：`dmPolicy: "allowlist"`、`groupPolicy: "disabled"`，原 `groupAllowFrom: ["*"]` 已删。
- 验证：`openclaw channels status` = `QQ Bot dad (爸爸的QQ): enabled, configured, running, connected`；网关日志零告警，WebSocket connected / gateway RESUMED。
- David 的 openid（掩码留档）：`7FC1C647…D99C`（dad 账号 allowFrom 唯一条目）。

### 5. main workspace 身份档案与录入纪律

- `USER.md` 重写为**家族档案**：dad 账号=爸爸 David / mom 账号=星星妈妈（**绝不称 David**）/ 孩子星玥只经设备由 kidpal agent 服务（main 不直接面对孩子）；通道即身份、无法确认就问不猜；两家长私聊互不转述（孩子共同事务除外）。
- `AGENTS.md` 增补「KidPal 家庭面工作纪律（2026-09-24）」——学校通知七条录入纪律：①判性质落区块（每周课外班→每周固定课表；单次事件→一次性安排，缺失时新建，五列同款）；②带物进备注列、用家长原词、不发明；③B1 结构零变更、`remind_before_min` 废止不写；④录入成功后立即执行提醒重算（与 cron 同载荷：`KIDPAL_SERVER_DIR=/opt/kidpal/front KIDPAL_SCHEDULE_MD=… KIDPAL_ALERTS_DIR=/opt/kidpal/alerts KIDPAL_SEMANTIC_JSON=… python3 /opt/kidpal/openclaw-patch/reminder_schedule_lib.py`）；⑤成人口吻向家长复述确认；⑥缺关键信息先追问；⑦不直推设备（P1「接下来」与三档提醒归 front/cron）。
- 同文件原有「KidPal 小伴」段的 v3 时代 stale 内容对齐 v6（断言式逐行替换 4/4 命中）：废弃 `{title,items}` 课表形状→薄载荷 `{}`（周网格 front 机械展开）；char 最小意图块 `{"char"}`→`{"char","meaning"}` 两键；prompt 唯一生成源指针 v3→v6；补拓扑行（设备=kidpal agent / 家长 QQ=main）。
- `kidpal/brain-spec.md` 全文重写为 **E1 v6 对齐版**（取代 2026-09-10 v3 版），含新拓扑说明；冲突时以 v6 为准的指针不变。

## 四、回滚点（全部留档于 ECS）

| 对象 | 回滚件 |
|---|---|
| openclaw.json（全量原始态） | `/root/.openclaw/openclaw.json.bak-20260924-150244-userfix` |
| openclaw.json（QQ 迁移前） | `/root/.openclaw/openclaw.json.bak-20260924-151506-phase4` |
| 引擎配置（model_name 原值） | `…/deploy/xiaozhi-docker-data/.config.yaml.bak-20260924-150353-userfix`（恢复 = cp 回 + `docker restart xiaozhi-esp32-server`） |
| workspace 文档 | `.USER.md.bak-userfix` / `.AGENTS.md.bak-userfix` / `kidpal/brain-spec.md.bak-userfix`（均在 `/root/.openclaw/workspace/`） |
| kidpal agent 整体撤销 | `openclaw agents delete kidpal` + 引擎 model_name 改回 `openclaw/default` + restart |
| 种子模板 | `/root/.openclaw/workspace-kidpal/.trash-seed/` |

## 五、待办（交接）

1. **QQ live 验证**（David）：给 dad bot 发一条消息确认星星正常回复（账号迁移后的真人验收）。
2. **真机终验**（David，持板）：设备说「你好」+「帮我显示课表」，确认称呼、display 块、P2 网格。
3. **mom bot 启用**（等 David 补 appId/clientSecret）：填入 `accounts.mom` + `enabled:true` → 重启 → 临时 `dmPolicy:"open"`（仅 mom 账号）→ 妈妈发一条消息 → 从 `known-users.json`/日志抓 openid → 写入 mom `allowFrom`（只此一条）→ 改回 `allowlist` → 重启验证（白名单外无响应）。注意 QQ openid 按 bot 维度隔离，爸爸在 mom bot 下是另一个 openid。
4. **仓库漂移同步**：`server/deploy/provider.config.yaml:67` 仍为 `model: "openclaw/default"`——下次 overlay 渲染会把 live 打回旧值。需走流程（k2 立项携带最合适）同步为 `openclaw/kidpal`。
5. **k2 立项落账**：本次已实际覆盖 k2 议题中的「David 人设渗漏治理」与「静态上下文瘦身（设备侧大头）」，立项材料应引用本记录避免重复立项。
6. **安全卫生**：本次会话转录曾两次意外打印 secret（tavily apiKey、dashscope ASR key，均在探查配置时随 grep/jq 出显）——**建议轮换两枚 key**；ECS 磁盘 94%（剩 2.3G）需清理；qqbot 插件 2.0.3 → 2.0.4 可升（不急）。

## 六、遗留已知问题（非本次引入，登记不处理）

- v6 prompt「一回合作答：不调用任何工具、不读取任何文件」与课表问答需读 `schedule.md` 的内部张力（perf-k1 已记录，归 prompt v7 / 课表轮次收敛议题）。
- cron `kidpal-reminder-plan` 的 Delivery 列显示 `announce -> last (no route, fail-closed)`——该 cron 以 `--command` 载荷写 `pending.json`，不依赖投递，`lastRunStatus=ok` 正常；显示语义留观。
- `agents add` 触发了一次 legacy state 迁移（`/root/.openclaw/agents/main/agent.legacy-1790233512277` 留存），未影响服务，留档。
