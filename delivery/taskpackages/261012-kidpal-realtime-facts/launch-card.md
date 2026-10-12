# Launch Card — 261012-kidpal-realtime-facts

跑 261012-kidpal-realtime-facts（独立变更，无 workstream，mergeOrder 48；② 单包 Lite〔riskTier medium〕：TECH-261012 天气幻觉修法——prompt v10「实时事实回合」+ 通用反编造 + kidpal AGENTS.md 仓内镜像 + Tavily 实证）。常驻树一树一会话串行接力（上一包 261011-p1-home-v5-rework 已 merged）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-12 · 261012 launch prep」段带 SHA 行（@9d07329 双面实测零红）——server 面 145 passed / 0 failed / 4 deselected / 9 subtests；firmware host 面 155 Tests / 0 Failures（**本批固件零改动**，firmware 面对锚零 diff 可引用）。intake 差异判停：`git diff --quiet 9d07329 HEAD -- <面域>` 零差异 → 引用锚行不重跑。
- **上一包留根**：`delivery/changes/dev/261011-p1-home-v5-rework/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **行为语义权威 = requirements v1.62**（NFR-SEC-2 ④ + NFR-SAFE-1 转述边界，frozen；human UX 定调「我去查一下，然后开始查」）；TECH-261012 = 根因与 scoping 权威；spine = delivery/changes/intake/261012-kidpal-realtime-facts.md（anchors 三面已回填：contract = NO_CONTRACT_CHANGE 四分类留痕 / req = v1.62 / arch = 指针刷归 docs-sync）。
  ② **Tavily 实证先于 ECS 部署 v10**（launchPrecondition）：经 loopback 18789 /v1/chat/completions（model openclaw/kidpal）发实时事实问题，journalctl + agent 会话日志双侧对账真出 web search 调用；无工具 → agents.entries.kidpal 增 tools 覆写后复测；平台级不可用 → 停、升级 human。
  ③ **prompt v10 版本演进纪律**：v9 全量继承零偏移（v9 sha256 零回改齿 RF-A3 钉）；CARRIER 演进史 append 零删；四个版本无关守卫（m3k3 A1 / MCD / M4K3-A8 / PN-A4）**四面同批翻即绿、漏翻必红**——builder 改接线时逐守卫核对。
  ④ **回合路由 = 独立新行**（不做对话行内白名单——行内白名单 = 句内自相矛盾，v6 病根）；判据「天气/新闻/今日热点等当下实时信息」；兜底「判不清按对话回合处理」（误判双向安全收敛）。
  ⑤ **ECS live ops 纪律**：ecs_put.py 通道（exec_command+stdin，MSYS_NO_PATHCONV=1），上传前核远端==基线；**overlay 重跑后必核 model_name=openclaw/kidpal 与决策 21 两键未被冲**（261001 冲刷陷阱 + D21 齿）；AGENTS.md「先改仓内镜像进 review，再同步 live」。
  ⑥ **AGENTS.md 镜像内容边界**：15–25 行（角色一句 / 反编造底线一句含实时事实指针 / 数据只读 / 「冲突以注入 prompt 为准」自声明保留）；不复述回合路由表全文（两源漂移免疫）。
  ⑦ **已知地雷前置申报**：全文件内容锚 + live 观测项、无 DB/事件断言 → supervisor 可能判 all-mock 触 riskTier 重算 high → 需 human 放行（v9 同款先例）。
  ⑧ **真机轮 human 在场**（问天气全链路：提示语 ~4-5s 内首句语音 → 真实答复 ≤15s 目标 20s 上限；加测一次降级路径：查不到 → 诚实+引导零编造）；观测 = journalctl + docker logs 双侧对账 + human 目击；串口捕获器关闭会硬复位设备的坑注意归因。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  ※ 本机实证坑（261010 台账）：`--dangerously-skip-permissions` 被平台分类器硬拦 → 实跑用**无旗标形态** `claude -p --settings .claude/dev-loop.settings.json --agent planner`。
