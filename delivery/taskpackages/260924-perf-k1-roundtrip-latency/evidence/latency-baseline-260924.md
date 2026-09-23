# 语音问答往返延迟基线证据(2026-09-24 00:10~00:20 CST 采集)

> 采集人:主线程(延迟排查轮)。三源:引擎 docker 日志(--timestamps)、openclaw-gateway
> journalctl、ECS loopback 直连探针(curl → 127.0.0.1:18789/v1/chat/completions)。
> 本文件是「优化前」基线,后续对照数据 append,不覆盖。

## 1. 引擎日志逐段拆解(近 24h 实轮,时间为 docker UTC 时间戳)

| UTC 时刻 | 用户问题 | 大模型收到→发送第一段语音 | 备注 |
|---|---|---|---|
| 09-22T23:38:27→23:38:55 | 今天西安天气怎么样 | **28.5s** | OpenClaw 内 3 次上游调用(web 搜索轮) |
| 09-22T23:49:00→23:49:30 | 今天西安天气怎么样 | **30.6s** | 同问题复测 |
| 09-23T02:36:17→02:36:25 | 显示我的课表 | 7.6s | |
| 09-23T02:42:19→02:42:36 | 显示我的课表 | **17.3s** | 同问题方差 9.7s |
| 09-23T04:41:25→04:41:39 | 结束语(长文案) | 14.3s | |
| 09-23T04:49:24→04:49:40 | 嗯嗯嗯(闲聊) | **16.3s** | 回「我在呢」一句话也要 16s |
| 09-23T13:25:03→13:25:22 | 为什么会一冷一热 | 19.2s | |
| 09-23T13:26:09→13:26:23 | 明天有什么课程 | 14.0s | OpenClaw 内 3 次上游调用 |
| 09-23T13:26:48→13:26:56 | Thank you | ~7.3s | 1 次上游调用 |

- ASR 段:`识别文本` 与 `大模型收到用户消息` 时间差 ≤3ms(Qwen3ASRFlash 流式,收尾即出)。
- TTS 段:`发送第一段语音`→`语音生成成功` 1.2~2.0s(EdgeTTS 首句合成)。

## 2. OpenClaw gateway 日志(journalctl,同轮对齐)

「明天有什么课程」轮(21:26 CST):
- 21:26:09.425 引擎发出 → **21:26:12.096 第一次上游 POST(preflight 2.67s)**
- 21:26:12.363 响应头 200(elapsedMs=267,SSE)
- 21:26:14.204 第二次上游 POST;21:26:16.922 第三次上游 POST(agent 多轮工具循环)
- 21:26:22.18 最终回答落地(含 ```display 薄载荷块,形状正确)

「嗯嗯嗯」轮(12:49 CST,纯闲聊、零工具):
- 12:49:24.44 引擎发出 → **12:49:29.763 第一次上游 POST(preflight 5.3s)**
- 12:49:30.164 响应头(elapsedMs=403)
- 12:49:39.05 **`[sqlite/transaction] slow SQLite transaction hold`**
- 12:49:39.13 回答落地 → 单次生成耗时 ≈9s(一句话回复)

天气轮(07:38 CST):preflight **6.1s** + 3 次上游调用(tavily web 搜索)+ slow SQLite,总 27s。

## 3. loopback 直连探针(2026-09-24 00:16~00:19 CST,优化前基线)

`curl -w` 对 `http://127.0.0.1:18789/v1/chat/completions`(model=openclaw/default, stream=true),
各 1 次:

| 提示词 | TTFB(SSE 头) | 总耗时 |
|---|---|---|
| 你好呀 | 0.08s | **13.1s** |
| 明天有什么课程? | 0.08s | **12.5s** |
| 今天西安天气怎么样? | 0.03s | **18.7s** |

注:SSE 首 chunk 只是 role 声明,正文 token 远晚于 TTFB——「总耗时」才反映真实首句可用时间。
探针绕开了设备/front/引擎/TTS 全链,证明 **10~15s 的大头在 OpenClaw 网关内部**。

## 4. 主机与网关状态

- ECS:2 vCPU,**1870MB RAM**,available 仅 ~807MB;**swap 3GB 已用 790MB**;load 0.71(CPU 不是瓶颈);up 23 天。
- openclaw-gateway(node):自 09-21 运行,RSS 500~650MB,**每 5 分钟报 memory pressure warning**(threshold 479.8MiB,ratio 100~130%),日志自带处置建议「restart gateway if pressure persists」。
- 同网关还驻 **qqbot 通道**(QQ 机器人,每 5 分钟 session 重连一次)。
- 引擎 docker 容器 Up 2 days。
- OpenClaw 版本 2.0.3(有 2.0.4 可升,未升)。

## 5. 当前 live 模型配置(openclaw.json,已核)

- `agents.defaults.model.primary = deepseek/deepseek-v4-flash`,**`reasoning: true`**(推理模型,回答前先产 CoT)。
- `modelPolicy.allow = [deepseek/deepseek-v4-flash]`(白名单仅此一个,切模型需同时改 allow)。
- catalog 内可选:`deepseek-chat`(非推理,131k ctx)、`deepseek-v4-pro`(推理,贵 12 倍)、`deepseek-reasoner`。
- 引擎侧 `selected_module.LLM = OpenClawOpenAI`(type openai,base_url 127.0.0.1:18789/v1,model_name openclaw/default)——引擎不感知底层模型,换模型不动引擎配置、不触契约 §四。
- `tools: [profile, web]`(web=tavily 搜索);OpenClaw 会话历史随引擎长连接累积(引擎 session id 全天恒定 `SiQwOpEdnonoOp`)。

## 6. 勘误与限定(2026-09-24 00:50 CST,architect 评审后 append;不覆盖上文)

- **版本口径勘误**:§4「OpenClaw 版本 2.0.3(有 2.0.4 可升)」为 npm 包版本号;产品版本 `openclaw --version` = **OpenClaw 2026.8.1 (ea80657)**。两套号并存,升级面判断以 runbook §0 版本漂移闸(dist 锚点)为准。
- **§3 探针限定(评审 D1)**:基线探针是**裸探针**(未带生产 v6 system prompt——v6 由引擎 `.config.yaml` `prompt:|` 段随每请求注入)。裸探针证明「大头在 OpenClaw 内部」仍成立,但**不是生产形态**(响应无 display 块、workspace bootstrap 人设渗出「David」);凡 A1/A2 判定探针必须带仓内 v6 prompt 逐字 + hash 断言。
- **方差限定(评审推论乙)**:同配置同题「你好呀」00:16 = 13.1s vs 00:41(architect 授权探针)= 6.6s,半小时 2 倍方差(swap 790→690MB 期间)。**单次探针数字不得当门证据**;门 = N≥3 中位数 + 主机状态快照 + 当晚同窗口基线重测。
- **§5 会话累积假设修正(评审 D2)**:「OpenClaw 会话历史随引擎长连接累积」不确——HTTP chatCompletions **每请求新建独立 session**(实证 510→511),儿童多轮连续性靠**引擎侧透传历史**;增长轴实为引擎侧历史(轮间增长、重启清零)+ OpenClaw session store 行数(持久增长,511 行/93MB sqlite,slow SQLite hold 活跃)。
- **qqbot 共 agent 实证(评审 Q2)**:`openclaw agents list` 唯一 main agent、零 routing 规则,qqbot 与 voice HTTP 同 agent 同 model——改 `agents.defaults.model.primary` 必然同时切 qqbot;当晚不做结构隔离,以 QQ 录入回归 + 提醒语义冒烟代偿,隔离记 k2 候选。
