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

## 7. P0 主线程代执行证据链(2026-09-24 08:03~08:16 CST,ECS 时钟;append)

> 勘误注记:launch-prep-record.md「P0 主线程代执行注记」标注 01:40 CST 为笔误(该节实际写于 ~08:00);P0 实际执行窗口 = 08:03~08:16 CST(journalctl/ECS date 可锚)。探针脚本 = 主线程诊断形态 `/tmp/kidpal_probe.py`(2472B,ECS /tmp,非验收工件;正式探针脚本仍归 builder 落仓)。v6 prompt 逐字断言:ECS 渲染件 sha256=425d293d…cc62(22464B)== 仓内 `server/deploy/openclaw-system-prompt.v6.md` LF 规范化后 sha256(仓内 raw CRLF=bf5c6b4c…,22745B;内容同一,EOL 规范化差异如实记)。

### 7.1 重启前快照与回滚锚(08:03)
- free:Mem available 845MB;**Swap 已用 734MB**;load 1.04(2 核);gateway 连跑 2d09h;近 30min memory pressure 告警 6 次;sessions 511。
- 回滚锚:`openclaw backup create` → `/root/2026-09-24T08-03-14.525+08-00-openclaw-backup.tar.gz`;`/root/.openclaw/openclaw.json.bak-260924-preP0`(7817B)。

### 7.2 重启前探针批(pre-restart,3题×3,带 v6,08:06:55~08:08:45)
| 题 | 中位 total_s | 样本 | display 块 |
|---|---|---|---|
| 你好呀 | **13.42** | 16.91/13.42/9.11 | 3/3 ✓(小伴人设) |
| 明天有什么课程? | **11.43** | 11.43/9.14/13.54 | 3/3 ✓ |
| 今天西安天气怎么样? | **9.54** | 8.86/9.54/17.94 | **0/3 ✗** |

### 7.3 P0 重启(08:12:57)+ 五项核验(08:13~08:14)全绿
①cron kidpal-reminder-plan 在册(every 15m/Next 8m/last ok);②qqbot 08:13:09 重连 READY(重启后 12s);③18789 锁 127.0.0.1+::1(V28 ✓,pid 2839131);④char-stroke dist marker 'KidPal C1' 在位(3 处命中);⑤引擎容器→18789 HTTP 200。附:swap 734→**251MB**,重启后 pressure 告警 **0**;近 3min error|warn 计 11 行(未逐条归因,留 Pod/晨间)。

### 7.4 重启后探针批(post-restart,3题×3,08:13:53~08:15:18)+ 稳定性补测(08:16)
| 题 | 中位 total_s | 样本 | display 块 |
|---|---|---|---|
| 你好呀 | **6.64** | 9.29/6.64/4.38 | **0/3 ✗(全部「David」人设)** |
| 明天有什么课程? | **11.21** | 9.73/11.77/11.21 | 3/3 ✓ |
| 今天西安天气怎么样? | **11.68** | 11.68/7.64/12.03 | **0/3 ✗** |
| 你好呀(稳定性补测×1) | 13.06 | — | 1/1 ✓(小伴人设) |

sessions 批后计数 **529**(511+18,与两批探针数吻合;store 膨胀速率实证,archive 留待 Pod 步⑦协议)。

### 7.5 P0 归因结论与新发现(供 P1/k2 与晨间决策消费)
1. **闲聊延迟双峰、质量-延迟耦合**:快样本(4.4~9.3s)全部 = 滑向 workspace「David」轻人设 + 丢 display 块;慢样本(9.1~17s)= 遵循 v6「小伴」纪律 + display 块。中位数改善(13.42→6.64)主要是快样本占比变化,**非干净的重启收益**;v6 遵循度(25k tokens 混合上下文:HTTP system 注入 v6 + workspace bootstrap 含 USER.md「David」)本身不稳定——architect「David 人设注入质检」k2 候选已实证为**live 生产面问题**(带 v6 也复现),升级 k2 优先项。
2. **thinking=high 实证在档**(重启后 startup 日志逐字:`agent model: deepseek/deepseek-v4-flash (thinking=high, fast=off)`)——thinking 档位是活配置,**B 案(v4-flash 关/降 thinking)有具体抓手**,Pod 步⑤ timebox 内查 agents.defaults/models 配置的 thinking 键与 catalog `reasoning` 语义。
3. **ttfc == total(全部 28 个探针样本)**:OpenClaw agent run 完成后才一次性吐 content deltas——引擎中途拿不到可提前 TTS 的文本;「首轮 agent run 总时长」是唯一优化标的,流式收益当前架构下不可得(k2 结构性调研项:dist 补丁已触流式出口,确认缓冲点在 agent loop 还是 openai-http 层)。
4. **天气题 display 块 0/6(两批全缺)**,而昨晚 live 轮(07:38)有块——display 纪律在天气路径现配置下已不稳;A2 门(100% 合法率)的基线事实:现状本身就过不了 A2,该门对新旧配置同等严苛,判定语义无需放宽、但晨间定案需带此认知。
5. P0 止血有效面:swap 释放 483MB、pressure 告警清零、轻闲聊轮 preflight 分量下降(快样本 4.4s 含全程);工具轮(课表 11.2s)不受重启影响——与「工具轮大头在上游多轮生成」判断一致。
6. 门对照(带 v6 中位数,post-restart):闲聊 6.64(保底 6 **未达**,目标 4 未达)/课表 11.21(保底 9 **未达**)/天气 11.68(保底 14 ✓)——**P1 模型切换仍是必需项**,P0 只解决方差放大器。

## 8. 联网调研 + CLI 地面真值:OpenClaw 慢的已知解法(2026-09-24 08:30 CST,append;human 指示联网查)

### 8.1 CLI 地面真值(ECS 只读探查,08:28)
- `openclaw config get agents.defaults.thinkingDefault` → **unset**,回执逐字:「The runtime default applies until you set an authored value with `openclaw config set agents.defaults.thinkingDefault <value>`」——startup 日志的 `thinking=high` 是**运行时默认**(openclaw.json 无 authored 值;疑随 23 日 22:43 remote catalog 更新而来)。**B 案抓手坐实:`openclaw config set agents.defaults.thinkingDefault off`(或 low)一条命令,无需换模型**。
- `openclaw models set` = 切默认模型的官方 CLI(A 案机制);`agents.defaults.models` 现仅 v4-flash 的 alias 条目、无 params override。
- **catalog 已扩到 6 模型**(remote catalog 更新生效):v4-flash / v4-pro / deepseek-chat(131k)/ deepseek-reasoner / **deepseek-flash(1000k,text+image,新)** / v4-flash-vision-exp——**A 案候选从 1 个变 3 个**(chat / flash / v4-flash+thinking-off),A/B 裁决面更宽;各模型 reasoning 标志以 `openclaw models list --json` 为准(builder 步⑤先查)。

### 8.2 网上解法清单(与本包解剖逐条映射)
1. **thinking/reasoning 档位**(官方 docs「Thinking levels」):级别 `off|minimal|low|medium|high|xhigh`;配置键 `agents.defaults.thinkingDefault` 与 per-model `agents.defaults.models[<key>].params`(GitHub #72407 实证键名);**消息内联指令 `/think:<level>`、`/t <level>` 对任意 inbound body 生效**——探针可在不改配置的情况下先 A/B 测 thinking=off 的效果(builder 步⑤ timebox 首选姿势)。注意(docs concepts/models):thinking 档位是 prompt cache identity 的一部分,**改档会一次性打穿缓存**,首个窗口 input 成本/延迟可能上浮,属预期。
2. **上下文膨胀 = 14s+ 慢回复的第一大成因**(betterclaw「~80% of the time」)——与我们实测 ~25k tokens/请求(v6 22.5KB + workspace bootstrap,DREAMS.md 40KB/USER.md 等)完全对上;解法 = 精简 bootstrap 文件、减注入工具/skills、会话 /new(HTTP 路径天然每请求新会话,已等效)。**k2「静态上下文瘦身」获社区经验佐证为第一杠杆;USER.md 即 David 人设渗漏源,瘦身与人设治理一石二鸟**。
3. **小 VPS 的 RAM/swap 先查**(blink 诊断序:RAM/swap → 版本 → 上游 API 延迟 → 网络 RTT)——与我们 P0(重启释放 swap 734→251MB)+ P3(4GB 升级 packet)路径一致。
4. **新版本含性能修复**(answeroverflow:2026.5.22 事件循环修复、provider-auth prewarm 延迟化、启动期 fs/plugin 操作精简)——本机 2026.8.1 已晚于该批;npm 侧 2.0.4 可升(晨间决策点 6,过 runbook §0 漂移闸)。
5. **fast mode 不是我们的杠杆**:官方口径 fast=服务档位(OpenAI `service_tier=priority`/Anthropic Fast),对 deepseek openai-completions 自定义 provider 大概率无映射(startup 日志 fast=off);不与 thinking 混淆。
6. **社区杂项**:并行 skills/plugins 过多、memory thrashing 亦为慢因(reddit gateway-slow 帖);`reasoning` 无 config 级默认、回落 off(GitHub #45880)——治理面在 thinking 键,不在 reasoning 键。

### 8.3 对本包计划的落点(不改冻结范围,只是把步⑤的弹药上膛)
- **B 案(优先试,timebox 内)**:探针带 `/think:off` 内联指令先测质量与延迟 → 满意则声明工件里落 `thinkingDefault: off`(经幂等 apply 脚本,不手改)。**B 案若成,模型不换、catalog/allowlist 零动,漂移面最小**。
- **A 案(并行准备)**:`openclaw models set deepseek/deepseek-chat`(或 deepseek-flash,先查 reasoning 标志与 1000k ctx 真伪)+ modelPolicy.allow 增补——三候选同批探针裁决(A2 质量门一视同仁)。
- 改档/换模后**首批探针缓存打穿属预期**,N≥3 中位数从第二批起算(避免误判)。
- 来源:docs.openclaw.ai/tools/thinking · docs.openclaw.ai/concepts/models · github.com/openclaw/openclaw/issues/72407 · github.com/openclaw/openclaw/issues/45880 · betterclaw.io/blog/openclaw-slow · blink.new/blog/openclaw-slow-latency-performance-fix-2026 · answeroverflow.com/m/1508564311746805870 · reddit r/openclaw(gateway slow 帖)。

## 9. 勘误:exit-4 成因不是「CLI 升级」,是 launch 形态差异(2026-09-24 08:40 CST,human 质询触发排查,append)

- **CLI 版本时间线(本机实测)**:npm 全局 `@anthropic-ai/claude-code` = **2.1.268,装于 2026-09-11 12:40**,此后未变(`where claude` 唯一命中 npm shim;AppData 各 Claude 目录为 desktop 配置 6/15、Claude-3p 7/4、cli-nodejs 遥测缓存 4/25——均非 CLI 本体;cli-nodejs/Cache mtime 09-24 01:28 恰为失败 launch 时刻的进程缓存,佐证走的同一安装)。
- **同二进制、两种形态、两种结果**:K7(260923)= 交互式 launch(pod-events 首事件含 AskUserQuestion「human 当场授权」→ human 在场交互形态),同一份 dev-loop.settings.json 跑通全程;260924 01:28 本包首次用 **headless `-p` 形态** → 同一文件被 settings 校验硬拒 exit 4。
- **修正结论**:`Write(./docs/contracts/**)` 无效规则**长期潜伏**(至少 9/11 前即在文件里),交互式形态容忍(或 K7 launch 未带 --settings,两可、无法从现存证据判定),headless 形态硬失败——「CLI 校验新口径/疑自动更新」的前说**不成立,撤回**。ARQ-260924 行内「防 CLI 升级再静默炸 launch 面」措辞随本勘误修正为「**headless launch 面的 settings 合法性 dry-run 机检**」(ARQ 行本体不回改,owner=architect 治理批消费本勘误)。
- **对决策点 0 的影响**:修复必要性不变且更清晰——无人值守/headless 是本包与后续夜间包的既定 launch 形态,settings 必须过 headless 校验;替代路径(不改 settings、由 human 交互式陪跑 launch)与「明早看效果/无人值守」诉求冲突,不取。

## 10. pre-P1 真机基线对照轮(2026-09-24 09:41:56~09:43:20 CST,human 持板,append)

> 协议合规:human 就绪信号开窗(09:40)、单一长捕获(被动 480s 窗,无复位、无 DTR/RTS 伪影)、实时口令钉三题。串口原件归档 `evidence/serial-baseline-pre-p1-260924.log`;三源对齐(串口 ms 时戳 / 引擎 docker --timestamps / 文本逐字互证),`with version: 1` ✓、Session 1b88b4c8、状态机 listening↔speaking 干净收尾 idle、无 payload_size invalid、无崩溃重连。

| 轮 | 问句(串口 STT 逐字) | 设备侧 >>→<< 首句 | 引擎侧 大模型收到→发送第一段语音 | 差 |
|---|---|---|---|---|
| 1 闲聊 | 早上好 | **17.22s** | 17.23s(09:41:56.87→09:42:14.09) | <0.1s |
| 2 课表 | 帮我查一下明天有什么课 | **13.64s** | 13.64s(09:42:28.64→09:42:42.28) | <0.1s |
| 3 天气 | 今天西安的天气怎么样? | **15.05s** | 15.04s(09:43:02.27→09:43:17.31) | <0.1s |

- 答复质量(三轮均好):小伴人设 ✓、课表含六节课+兴趣课空档+课外无安排 ✓、天气含真实检索数据(17~25℃/霾/傍晚小雨)✓;TTS 首句 0.3~1.2s。
- **读数①(双峰生产实锤)**:闲聊轮真机落**慢 mode**(守 v6 纪律、答质量好)= 17.2s;而 §7.4 探针中位 6.64s 是快 mode(David+丢块)采样——**真机儿童体验以慢 mode 为准,P0 重启对守纪律路径无实质提速**。
- **读数②(对照昨晚 §1)**:闲聊 16.3→17.2s、课表 14.0→13.6s(持平);天气 28.5→15.0s(P0 重启后大降——主机劣化分量主要在多轮工具路径)。
- **读数③(对门)**:A3 保底门(闲聊≤8s/课表≤10s)当前超 1.7~2.2 倍——**P1(thinking-off,B1 裁定主案)是决定性一刀**;本节即 A3 复跑时的同日对照锚。

## 11. 验收齿冻结(acceptance-author,2026-09-24 本 Pod 树,append)

> 阶段划分 1 产物。齿文件 = `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`(27 齿 = 23 offline + 4 live);设计说明 / 逐齿映射 / mutation 原始输出留档 = `server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md`(本节不复制其正文,只记事件与关键读数)。
> 本棒在池树跑(树迁址见 launch-prep-record.md);任务书 = `input-brief-acceptance-author-round2.md`(B1 修正版)。

- **红基线实测**:`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/`(仓根)= **18 failed / 62 passed / 4 deselected**。18 红全为**设计态红**(声明工件 / apply / 回滚 / probe_gate / probe_batch 五件未落盘);`--collect-only` **收集零错误**(冻结即冒烟过)。
- **既有齿零新增红(planner 独立复核)**:既有 4 个 acceptance 文件单跑 = **57 passed / 0 failed**,与上游基线锚「@1746c14」吻合(`git diff --quiet 1746c14 HEAD -- server` 零差异);全量 62 passed = 既有 57 + 本包 5 条 must-stay-green 设计态即绿齿,算术自洽。
- **机械闸**:`acceptance-coverage-check` = **ok**(PERF-A1~A4 全 functional,无 structural/suite 豁免);`mutation-check` 暂卡「缺 mutationEvidence 字段」——由 acceptance-author 补记录、planner 落 taskpackage 后复跑(见 launch-prep 待办)。
- **齿形态落位**(评审 §3 + B1 修正):CI 齿只断机械面(声明工件 schema + **B 主案/A fallback 两形态皆可判**、apply 幂等 + drift-guard「锚点不中→exit 3」+ 时间戳备份、回滚 dry-run 与一步还原、探针脚本 v6 注入 + hash 断言、must-stay-green);live 齿取 **evidence-read 形态**(不 SSH,判定器逻辑另有 offline 齿);mutation 打在**配置接线**上而非超时判定。
- **mutation 实测**(notes §6 原始输出):GREEN 对照组(合法 B/A 声明 = 0 violation)+ 6 条接线 mutation 全转红(M1 删声明文件 / M2 thinking 值改 `high` / M3 删 allow 条目 / M4 `reasoning` 翻真 / M5 两轴混声明 / M6 改错 model id);另以 tempdir 下的**临时参考实现**反证 apply/rollback 四条行为齿非空过、契约可实现(含「摘掉 drift-guard 循环 → mismatch 场景 rc 由 3 变 0 → 该齿转红」的 counterEvidence)。
- **冻结契约五件**(builder 须实现,路径以齿文件头注 FROZEN OFFLINE CONTRACT 为准):`server/deploy/perf-k1/openclaw-model-declaration.json`(JSON,`case ∈ {B,A}`)+ `apply_model_declaration.py` + `rollback_model_declaration.py` + probe_gate 纯判定器 + probe_batch live 执行器。
- **planner 裁决留痕**(acceptance-author 提的 5 条歧义):① 写边界拦截未绕行,`redBaselineResult` 与本节由 planner 代落;② apply/rollback 取 **Python 载体**——任务书「照抄 STOP-on-mismatch 形态」按**行为形制**解读(拒绝 + exit 3),形制逐字保留,语言差异非偏离;③ live 面取 **evidence-read、不做 SSH**,真机发 HTTP 的探针批执行器归 builder/verifier live 相位;④ A 案 allow 不钉全集精确相等(避免过度脆性);⑤ 声明工件取 JSON(零依赖 stdlib,K3 先例读 YAML 亦走 regex)。
