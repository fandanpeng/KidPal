# Launch Card — 261010-p3-storylist-cold-start-restore

跑 261010-p3-storylist-cold-start-restore（m4-star-notebook，mergeOrder 49）。② 单包 Full〔riskTier medium〕：P3 story_list 冷启动恢复推送——front 建连附带重推（data.restore 标记）+ 板端只填缓存不抢前台 + 空态两态渲染（v1.63 话术）+ FIX-A1 负锚齿翻正；契约 CCP D1–D4 先批后落同批原子落。常驻树一树一会话串行接力（上一包 261012-kidpal-realtime-facts 已 merged）；**本批 launch 形态 = 本树既有会话即 Pod 本体就地起 loop**（m2-char-window 先例，human 2026-10-12 指示「我们开始处理这个吧」）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-12 · 261010-p3-storylist-cold-start-restore launch prep」段带 SHA 行（@fde1eb9 双面实测零红）——server 面 152 passed / 0 failed / 4 deselected / 9 subtests passed；firmware host 面 155 Tests / 0 Failures / 0 Ignored（热缓存 no work to do，与 261012 锚 @9d07329 同数持平、firmware 面 git diff 9d07329..HEAD 零差异）。intake 差异判停：`git diff --quiet fde1eb9 HEAD -- <面域>` 零差异 → 引用锚行不重跑。
- **上一包留根**：`delivery/changes/dev/261012-kidpal-realtime-facts/pod-retro.md`（若在），intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **行为语义权威 = requirements v1.63**（S-F2.1 AC1 数据已送达限定 + AC5 无内容空态〔×0 计数 +「本子空空的，讲个故事点亮第一颗星吧」〕+ AC6 数据未达空态〔不显计数 +「本子还没翻开，等等再来看看吧」〕+ 补充规则冷启动恢复，frozen 2026-10-12）；TECH-261010-p3-storylist-cold-start-empty = 根因与 scoping 权威；spine = delivery/changes/intake/261010-p3-storylist-cold-start.md（三面 anchors 已回填：req = v1.63 / arch = :186+:214 镜像已落 / contract = CCP HUMAN_APPROVED 2026-10-12）。
  ② **契约落地通道（本批特殊：先批后落）**：CCP = delivery/taskpackages/261010-p3-storylist-cold-start-restore/contract-gate/CONTRACT-CHANGE-PACKET.md（D1–D4 逐字 diff），HUMAN_APPROVED 在案、diff 未落——builder/integrator 通道 = 逐字 patch 过目 → git apply（CRLF 保全，settings deny 仅 Edit 形态、Bash 无重定向通道，260921-m3-k1 + 261011 先例），人工 diff 逐字核对留痕进 gate evidence §5/§7；落地后 display_schema.py V27 floor 同步扩（D3 联动）。**契约文件任何会话不得用 Write/Edit 直改**。
  ③ **FIX-A1 负锚齿翻正**：test_m4k1_story_cloud_link.acceptance.py :1000-1044「建连零 story_list 帧」→「建连恰一帧 restore」；test_m4k3_record_session_live.acceptance.py :25 跨包负锚同批翻正——tighten-only 拦截预期命中，REVISION LOG 授权链 = CCP id + §9 v1.63 行 + human 方向锚留痕（260928 FIX-A2 先例形态）。
  ④ **板端分流形态复刻 schedule :926-991**（restore=true → cache_story 装缓存 + begin/end_page 不调 + P3 在屏仅重绘本页；缺省/false = 现行为零偏移，K2 消费面 + K3 [S4] gating 全保持）；布尔解析复用 mcp_json_get_bool（:444-446，absent/非 true 字面同判 false——与契约缺省语义逐字对齐）。
  ⑤ **ECS live ops 纪律**：恢复推送开关默认关（schedule grid_restore_enabled 先例，front_app.py :743 形态）→ SL-A5 真机验证后 human 人工开闸（gate evidence C1）；ecs_put.py 通道（exec_command+stdin、MSYS_NO_PATHCONV=1）、上传前核远端==基线。
  ⑥ **已知地雷前置申报**：SL-A1~A4/A6 为 wire 帧与 host 纯逻辑断言、无 DB/事件断言 → supervisor 机械闸可能判 all-mock 触 riskTier 重算 high → 需 human 放行（261012 v9 同款先例申报）。
  ⑦ **真机轮 human 在场**（SL-A5：重启冷启动翻 P3 见列表含计数、任意页无 P3 抢占、空态两态话术各验收一次）；烧录 = app 分区单烧（本批无 spiffs 资产变更）。
  ⑧ **ctx(story) 条件推送归并**（front_app.py :216 defer 时代「会话建立同拍」路径）与新建连推送的归并处置 = 本包行为策略（arch :186 三时点对齐），owner = 本包（ARQ-260911 登记）；归并方案若动摇 arch :214 已落表述 → 回 planner 升级，不带病下行。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  ※ 本机实证坑（261010 台账）：`--dangerously-skip-permissions` 被平台分类器硬拦 → headless 实跑用无旗标形态。**本批实跑 = 形态①就地（本树既有会话即 Pod 本体），不另起第二会话同树并行（树池协议）。**
