# Pod Retro — 260903-device-ws-switch（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 卡点

- **最后一里路的 LLM 卡点不是网络，是「工具配置」**：设备都上屏出 STT 文字了，却回「小智现在有点忙」。引擎日志钉死真因——`LLM stream processing error: invalid tool configuration`，引擎以 `Intent: function_call` 把 11 个工具（7 个服务端 + 4 个设备 MCP）塞进发给 OpenClaw 的请求，OpenClaw 的 OpenAI 兼容端点拒收。切 `nointent` 即解。教训：**冒烟片要先想清楚「这一片到底要不要工具调用」**，tracer-bullet 不需要，就不该让 function_call 进门。
- **协议版本是个会漂移的 NVS 键，害了一轮崩溃 + 一轮无声**：官方服务端 0.9.6 在 ws 腿上发的是**裸 opus**（v2 二进制头只走 MQTT 网关），原生设备该用 version=1。早先我 CSV 里误写 version=2，设备按 v2 头解裸帧 → 垃圾 payload_size → OOM 崩溃；加固（校验 + try/catch）后变成「全帧丢弃、无声」。真正的教训是：**既然我们的网关恒讲 version=1，这个值就不该是一个能被刷错/刷漂移的 NVS 数据键**——它是设计该钉死的常量，不是运维数据。
- **一次假阳性差点埋雷**：早些时候的「NVS 手术补丁 version→1」脚本，页面魔数门写错（`\xfe\x83`，真实活动页是 `\xfe\xff`），永远不匹配、悄悄什么都没改，而日志看起来「patch 成功」。version=1 实际是后来一次刷写落进去的。教训：**改 NVM/flash 这类离线二进制，必须回读复核落地值，别信脚本自己打印的「已改」**。这条已沉淀进 ops skill（kidpal-nvs-ops）。
- **验收要的字面串被 ws 压缩藏住**：A4 原本要 `smoke-roundtrip.log` 里出现 `stt/tts/sentence_start` 字面串，但引擎↔前置这段 ws 协商了 permessage-deflate，文本帧全被压成二进制，只读 tcpdump 解不出、设备和引擎日志也都不打原始 JSON。为几个字面串要去动生产前置，不值。

## verifier↔fixer 绕轮的主观判断

结构面（A1/A2/A3/A5）的机械红都是「真问题被闸兜住」，闸工作正常。真正耗挂钟的是 live 面——而且暴露的是**上游 G 包（方向 B 网关）的一撮潜在缺陷**：前置 websockets v17 不兼容、缺 device-id 头透传、TTS `type` 要写 `edge` 不是 `edge-tts`、OpenClaw `base_url` 必须带 `/v1`、容器拿不到 loopback 上的 OpenClaw（最后靠 `network_mode: host` 解）。这些 G 的 live 验收当年只「探测可达」、没「真跑一遍全链」，所以全留到本包真机冒烟才连环爆。主观建议：**live 验收别只 probe，要真 exercise 一次真实路径**。

## 别扭的 acceptance

A4 是本包最别扭的一条：它要的证据形态（线上原始报文字面串）恰恰是这套栈在自然配置下**永远不会产**的形态（被压缩吃掉）。四个协议事实本身都是该验的（握手/ASR回传/TTS生成/流式句级），但「用什么字符串证明」冻结时没想清压缩这层。最后由 human 拍板「验收改认已有证据」：四事实仍强制，标记换成本栈真实会打的（session_id 保留字面 + `识别文本`/`语音生成成功`/`SentenceType` 等）。这是**改革证据形态、不是放松严格度**。教训给 acceptance-author：**冻结证据型验收时，标记要选「这套栈真实会落盘」的形态，别选要为此动生产才能拿到的形态**。

## 帮倒忙的钩子/规则

- **auto-mode 权限分类器在收尾半路拦了前置部署**：即便早前给过「全套 ECS 操作」授权，它仍要求「本次部署由 human 亲口点名」。安全上合理，但把我逼去先走只读 tcpdump 的弯路（撞了压缩墙）才回到正解。教训：**涉及生产的取证动作，派发时就把「需要 human 原话授权」前置说清楚**，别让 agent 先试一条注定被拦/被压缩堵死的路。
- **`espidf_wsl.sh` 是 CRLF 行尾**，WSL 的 bash 直接解析崩（`$'\r': command not found`）。我没动仓库文件、生成 LF 副本跑的。**shell 脚本入库就该是 LF**，这类 Windows checkout 行尾问题值得在仓级 `.gitattributes` 钉死。
- **tcpdump 两个坑**：stdout 重定向到文件是块缓冲（不加 `-l` 看不到东西）；`pkill -f tcpdump` 会把正在执行的 bash 外壳自己也匹配杀掉（报 127）。都已写进 skill。

## 人工纠偏体感

human 这包介入次次关键：dashscope key 直接解锁 ASR（G 一直没拿到阿里云 ASR 凭据）；「V28 为什么不能松」逼我把「18789 锁 loopback」从字面（bind）和不变量（不可公网可达）讲清，最后落 `network_mode: host` 这个更干净的拓扑；「不能为了不合理的设计绕一大圈」这句直接促成了 A4 证据形态的改革而不是硬闯生产；还有冒烟时「还没说就先说好了」的时间差误会——**真机冒烟的「开始/好了」信号要和抓包窗口严格对齐**，这个协调成本下次要在派发时讲明。

## 下次改进

1. **协议版本钉常量**：方向 B 网关恒讲 version=1，固件侧别把 version 当可漂移的 NVS 数据键；要么代码钉死、要么配网写入面强制校验。
2. **live 验收要真跑全链**（见上），尤其跨进程的 G 类包。
3. **证据型验收的标记预检**：冻结时先确认「这个标记在本栈自然日志里真实存在」。
4. **shell 脚本 LF 入库 + `.gitattributes`**，消灭 CRLF 在 WSL 下的崩。
5. **NVS CSV 直注 WiFi 凭据连不上**这个根因本包没查（绕过了，走设备配网口），留作后续包跟进。

## 升级主题（pod-events 交叉引用）

- A4 证据形态改革（human 拍板「验收改认已有证据」）：human-decision 事件 `2a53a2f8`。
- 五项 live 偏差（Intent nointent / qwen3_asr_flash 用 dashscope 而非 aliyun_stream / TTS edge / 引擎 network_mode host / 前置 websockets v17 + 头透传）：已记入 review-packet，供 supervisor 闸与下游包参考。
- NVS 页面魔数假阳性：ops 教训，落 skill（kidpal-nvs-ops），本包不升级。

## 收口后补记：合并相位自己又踩三个坑（2026-09-06，人批合并后补）

- **合并工具本身在 main 上演进时，Pod 分支自带的副本可能是旧的**。我分支岔出于 merge-driver worktree 拓扑热修（ARQ-260905）之前，自带旧版会在本 worktree `checkout main` 被 git 拒（main 由主 checkout 持有）。解法 = 改用 main 上已修好的 merge-driver.mjs 跑（它定位持有 main 的 worktree 在彼处 `--no-ff`）。教训：跑合并门前先比对「我分支的闸脚本 vs main 的闸脚本」，别默认分支里的就是新的。
- **后台图索引器会把合并通道撞断（环境级、每包复发）**。CodebaseMemory 后台索引器持续重写 worktree 内被 git 跟踪的 `.codebase-memory/graph.db.zst`，git 树永远脏，`git rebase`/`git merge` 因 unstaged changes fail-closed——`merge=ours` 只决冲突内容、管不到「rebase 要净树」这层。本次靠给两个 worktree 设 `skip-worktree` 权宜放行，已立 `ARQ-260906-live-graph-dirty-blocks-merge` 追根治。这不是本包内容问题，是「活进程写被跟踪缓存文件」的结构性摩擦。
- **跨 Pod rebase 冲突是机械并集，不吓人**。与已并的 session-logic 交叉 5 处：两个 `current/` 投影取最新活跃包（本包），`progress.md`/`.planning/INDEX.md`/`host_app/CMakeLists.txt` 三处做并集（两包内容都保留，历史记录只增不改）。多 Pod 在同一份 CMakeLists/INDEX 上叠加是常态，rebase 时耐心解并集即可。
