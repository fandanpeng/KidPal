# 包 9b 硬件真机验收 · 进展总结（2026-09-10）

> Pod `260829-hardware-validation`（mergeOrder 13，testHost=device）。本文是当日真机轮
> 的进展 / 问题 / 后续 consolidated 记录，供交班与收尾。实时逐条台账见同目录
> `pod-events.jsonl`；逐项验收勾验见 `device-acceptance-checklist.md`；原始串口/烧录
> 证据见 `changes/dev/260829-hardware-validation/*.log`。

## 一、今日达成的里程碑

- **设备渲染节点落地**：KidPal 4 页 UI（home/char/schedule/story）首次上真机 RLCD，
  BOOT 双击翻页循环可用（HWV-E3 兜底达成）。新增 `firmware/main/display/kidpal_render_node.cc/.h`。
- **首页 V3 换皮可见**：art SPIFFS 挂载 `/art`（47 枚 KAP1 资产）+ render node pushing
  home 400×300，用户实测「前 3 个可见 + 电池图标有」。
- **RTC 走时 + SHTC3 温湿度真机直读正确**（HWV-A4 部分绿）：`rtc=20:48:21`、
  `temp_c=28.52 rh_pct=46.5`，与室温吻合。
- **证书真机握手通过**（HWV-A15 绿）：自签 CA pinning 首连
  `wss://8.137.91.16:8000`，`Certificate validated` + session 建立。
- **语音轮 + 故事显示 + TTS 音频端到端真机跑通、0 崩溃 0 复位**（本包最硬里程碑，
  voice-round4.log）：单击→ASR 全句→speaking→story_text_dispatch screens=2→LLM 回答
  →speaking→listening 平稳→用户「继续」再入 speaking。音频输出管道真机证实可用
  （decode→resample 16k→24k→I2S→ES8311）。
- **ECS 云端语音链路已通**：ASR Qwen3ASRFlash + Intent nointent + LLM OpenClawOpenAI
  + TTS EdgeTTS，真机 ASR 全句 + LLM 回答 + TTS 出声验证。

## 二、修复的崩溃 / 缺陷链（按发现序，含根因）

1. **boot hang（task_wdt）**：SHTC3 在共享 I2C 总线（SDA13/SCL14，RTC+SHTC3+ES8311+ES7210）
   读后留 bus busy → ES8311 音频 codec init 永久挂起。修：RecoverI2cBus（i2c_master_bus_reset）
   + 有限超时 + GetAudioCodec-before-InitializeSensors 重排。
2. **RTC "18:72:36"**：PCF85063 寄存器是 BCD，代码只掩码未解码。修：BcdToDec（高半×10+低半）。
3. **SHTC3 系列**：① POR 后睡眠需 wake-up 0x3517；② clock-stretch 被驱动误判 stuck → no-stretch
   回退；③ **命令字 0x7852 错误**（planner 未查 Sensirion 数据手册给错，WebSearch 核实正确
   = **0x7866**，T 先、禁 stretch）。修后正常读数。
4. **换皮不可见**：art_assets 零调用 + U 包只留 `#include` 标记 + art SPIFFS 休眠
   （CMake spiffs_create_partition_image 未启）+ 渲染节点缺失（s_p1_fb→host weak no-op）。
   逐级修：分区表加 art 0x780000/0x80000 + esp_vfs_spiffs_register("/art") → 渲染节点 →
   内容渲染（描边框 + imgfont 数字 + art blit）。
5. **DRAM 溢出**：art_assets.c `g_fb[400×300]`=480KB 静态 BSS 超 ESP32-S3 内部 DRAM。
   修：改 loader 原语（mount/load/pixel/glyph）不链 g_fb + s_p1_fb 移 PSRAM
   （`.ext_ram.bss` + CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=y）。
6. **单击无反应**：render node 的 s_kidpal_scr 盖住 xiaozhi SetupUI（建在 lv_screen_active
   默认屏），单击开语音通道但聆听表情压在下面。修：init 捕获 s_chat_scr + 单击
   Schedule(show_chat) 切回 + 1s lv_timer tick 状态对账（voice→SetupUI 前台 / idle→回首页）。
7. **esp_timer 栈溢出（语音轮1）**：speaking 起音频输出管道时 esp_timer 任务（栈仅 3584B）
   溢出复位。修：抬 ESP_TIMER_TASK_STACK_SIZE 3584→8192 + FREERTOS_TIMER_TASK_STACK_DEPTH
   2048→4096；**重采样输出缓冲越界修复**（silk_resampler 上采样按 batch 向上取整，非整除
   输入最多多写 1 样本踩堆 → +8 余量后截回）；音频管道三段埋点（收流/解码/I2S 写）+ 栈高水位诊断。
8. **ssl_receive 栈溢出（语音轮2）**：story_text 解析链跑在 esp-ml307 `ssl_receive` 任务
   （esp_ssl.cc:61，栈仅 **4096B**）上，叠 `kidpal_story_text_t parsed`（2208B）+
   `kidpal_story_text_parse` 内 `id/title/text_buf`（2208B）+ cJSON/解帧调用深度 ≥4.4KB 净缓冲
   → 溢出踩进堆内邻接 main 任务栈区 → main canary 先绊倒（报「task main」+ 坏栈是**症状非病因**）。
   修：两块大缓冲挪堆（parse 直写 out、DoToolCall 用 shared_ptr 进 Schedule 闭包）+ 同族
   story_list（2824B）/ schedule items[16]（1792B）/ ScheduleStoryRender（5140B）一并挪堆
   + 防回归 guard 测试 `story_page_stack.test.c`（3 齿）。
9. **SetStatus 悬空指针（语音轮3，addr2line 精确定位）**：story_text 渲染经 Schedule 在 main
   任务调 `kidpal_story_page_render`，该函数 `scr=lv_screen_active(); lv_obj_clean(scr)`；
   **语音轮期间活动屏 = xiaozhi 聊天屏 s_chat_scr**（渲染节点 tick yield 过去），clean 它销毁了
   `status_label_` 等 xiaozhi 控件，但 LvglDisplay 成员指针不置空 → 悬空 → speaking→listening
   转换时 `SetStatus` 写悬空 label → `lv_label_set_text→lv_free` 垃圾指针 → LoadProhibited
   （EXCVADDR 0x00210019，A15=0xcdcd 释放毒值）。修：渲染节点加 `begin_page/end_page`
   （持 LVGL 锁 + load s_kidpal_scr + pin），MCP **7 处**渲染闭包全部夹入，tick 尊重 pin
   （防 1s 后抢回聊天屏），pin 在 cycle_next + 新轮 connecting 清除；**额外捕获同族提醒弹窗
   路径**（RaiseReminderQueue 同样 clean 活动屏，提醒恰在语音轮到点会触发同款崩溃）。
   页面渲染函数签名零改（靠 begin_page 预载改变 active 屏）。

## 三、流程 / 工具教训

- **硬件寄存器命令必查数据手册**：planner 凭印象给 SHTC3 命令字 0x7852（错），未查 Sensirion
  表 → 浪费一轮。教训：传感器命令字/寄存器以官方数据手册为准，不臆测。
- **跨任务栈腐蚀的识别**：backtrace 腐蚀（SP=垃圾值）+ 报错误任务名（main/esp_timer 轮换）
  = 跨任务腐蚀典型症状，非真凶任务自身溢出。完整未腐蚀 backtrace + addr2line 才能定位真凶
  （轮3 LoadProhibited backtrace 完整 → 一解即中 SetStatus 悬空）。
- **fixer subagent 多次因 API 配额提前终止**：需 SendMessage 恢复同一 agent（保留上下文）续做。
- **诊断埋点先行**：轮1 加音频管道三段埋点 + 栈高水位，轮2/3 复现时直接读出断点，避免盲猜。

## 四、后续需要处理的问题

### A. 设备观感（用户轮4/5 实测报，待修）

1. **故事页排版**（本包 9b，story_page 渲染殿后项到期）：`ksy_label`（story_page.c:1062）
   建标签不设宽度、不开 wrap → 正文 `story->text`（可达 2047 字）被当一整行渲染、横向裁切
   （用户见「小刺猬」只剩「小刺」）+ 下方大片空白；`kidpal_story_open_reader` 算了 screens
   但渲染端画全文不消费分页。修法：ksy_label 开 wrap + reader 按 screens 分页只显示当前屏正文。
   **风险**：story_page.c 在 host 编译矩阵、有冻结 story_p3 验收齿（8 齿）采样渲染帧，
   改布局须跑 host runner 保齿绿；碰齿则升级 acceptance-author。
2. **字体**（U 包 ui-art 字体资产边界）：本板正文用 `font_puhui_basic_14_1`（阿里普惠 basic
   子集，14px 1bpp），次常用字「猬」等不在子集 → 渲染空白；且正文是普惠体非 V3 设计字体
   （资产里有 `font_zcool_30.bin` 站酷体 + imgfont 数字/冒号，但只用于首页）。影响 HWV-A7
   16px CJK 可读性。需确认设计字体在设备可用 + 接到正文。
3. **课表页**：用户报「没用课表具体内容」「字体都是正体非设计字体」——课表数据填充 + 字体占位待处理。

### B. 设备物理验收剩余项（HWV-A2~A8 + 续，需 human 持板）

- HWV-A2 按键手感：单击/双击可用，长按 story PAGE_TURN 冻结语义待验；消抖连按计数。
- HWV-A3 双麦录音整轮：ASR 上行已验，录音清晰度 + 故事录制→停录→盖章整轮待验。
- HWV-A4 断电续航子项：RTC 走时 + SHTC3 直读已验，拔 18650 ≥10min 再上电对时待验。
- HWV-A5 低电打盹（15% 阈值触发）+ RLCD 真屏观感（对比度/可视角度/刷新）。
- HWV-A7 16px CJK 可读性（与字体问题 A.2 耦合）。
- HWV-A8 前光确认（反射屏暗处可见性，spec §5.6）。
- HWV-A13/A14 实机续航基准 + 常驻 WS 功耗观察（决策 21 三要素）。
- F2-1（HWV-A10/A11/A12 真机端）：真录故事→网关 archive_story 落盘→story_text 回看→盖章；
  断开重连不串味。

### C. ECS 部署（HWV-A16）

- **部署漂移**（危险）：`apply-xiaozhi-overlay.sh:60/62` 仍写 ASR=AliyunStreamASR /
  Intent=function_call，`provider.config.yaml:77` asr.provider=aliyun，与 live config
  （Qwen3ASRFlash / nointent）不符——**直接跑 overlay 会覆盖 live、断语音**。需先修漂移
  使脚本=live，再选择性部署。
- **提醒 cron §8 部署**（DEPLOY-RUNBOOK）：reminder_schedule_lib.py + kidpal-reminder-plan cron 未落。
- live 五齿 L1-L5 真跑一轮 evidence 交班。

### D. dev-loop 收尾

- integrator closeout + pod-retro + supervisor-verdict-gate + pre-merge-escalation-gate
  + merge-driver（local --no-ff，禁直推 main / 禁 remote push）。
- verify.mjs scoped（逐轮）/ final（合并就绪）复验。
- **host runner 4 红归因**（均非本包代码引入）：VRT-A2/E2、WSA-A6/A4 = 证据门（缺他包
  flash.log / ecs-teardown / roundtrip smoke 落盘制品，超 fixer 权限，属 supervisor/human 回收）；
  **P2-A1 结构齿**（schedule_page.c 标题弹跳 lv_anim）= first-match 歧义先行红——该齿取
  `kidpal_schedule_page_render(` first-match 文件再要求含 `lv_anim|弹跳`，现恒命中先行轮新增的
  `kidpal_render_node.cc:362`（无该标记）。修法二选一：render_node 侧补真实标记，或
  acceptance-author 修齿 first-match 歧义（不自行喂哨兵让红消失）。

## 五、晚间链路梳理新增问题（2026-09-10，详见同目录 VOICE-ROUND-FLOW-260910.md）

主线程对「语音轮端到端」逐文件梳理 + 自 review 后新增，均有代码证据：

1. **故事录音云侧归档双断链（功能不可达，高危）**：①全固件无 `StopListening()`
   调用方 → `listen state=stop` 发不出（front 归档只认此帧）；②manual 录音期间
   引擎不出 stt 帧 → front 累积缓冲恒空，即使修①归档也拿空转写。待 F2-1 真机戳实。
2. **设备 UI 三屏主权混乱（架构根因）**：s_chat_scr / s_kidpal_scr / char 独立屏 +
   两套 UI 体系，前台靠 3 标志 + 1s tick 对账；回答文本写在被 pin 覆盖的聊天屏；
   当日三个崩溃（悬空指针/clean 聊天屏/栈溢出同族）的结构性土壤。
3. **显示滞后语音 1–3s**：display 块等 LLM 全流 finish() 才发侧通道。
4. **首音时延 ≈3–6s**：VAD 尾点 1s（min_silence_duration_ms 默认值）+ 整句非流式
   ASR + LLM 首 token + EdgeTTS 整段合成四段串行；固件/front 非瓶颈。
5. **spec↔实现键位漂移**：spec §4.0/§5.3 设计 KEY 主司对话 + BOOT 主司翻页两键
   分工；实现仅接 BOOT（单击对话/双击翻页），KEY(GPIO18) 未接，KEY 依附的页内
   特例（回首页/光标/播笔顺/弹窗关闭）全部悬空。
6. **轮间驻留反直觉**：auto 轮答完回 Listening 不回 Idle，页 pin 与首页交还的
   结局随是否有 display 块随机。

问题合并排序与分批处理方案见 §七（2026-09-11 human 确认收尾后登记）。

## 六、当前固件状态

- 分支 `dev/260829-hardware-validation`，最后烧录 bin = `xiaozhi.bin 0x2c2060`（2,891,872 B），
  app 分区余 20%，设备语音轮 + 故事显示 0 崩溃。
- 冻结面未碰：`docs/contracts/**`、`*.acceptance.*`、`managed_components/**`。
- `.codebase-memory/artifact.json` + `graph.db.zst` 为 indexer 自刷新机器再生面，常驻脏、不提交。

## 七、遗留问题总清单与分批方案（2026-09-11 登记，收尾合并后）

> 本包已合入本地 main（merge 876b550，human 批准 ESCALATE）。以下 14 项遗留合并自
> 本文档 §四/§五、review-packet §9（F1~F7）与 docs/notes/board-hardware §一键位对照。

### 7.1 总清单（14 项）

| # | 问题 | 来源锚 | 状态 |
|---|---|---|---|
| 1 | ECS overlay 部署漂移（脚本≠live，跑即断语音） | F3 / §四 C | 待修（P1 前置） |
| 2 | 提醒 cron §8 未部署 + live 五齿 L1-L5 evidence 未交班 | §四 C / A16 | 待修（随 #1 同批） |
| 3 | 故事归档双断链①②（listen stop 发不出 + 累积缓冲恒空） | F4 / §五.1 | 疑似，待 F2-1 真机戳实 |
| 4 | 设备 UI 三屏主权混乱（架构根因） | F6 / §五.2 | 待立项（先设计讨论） |
| 5 | 显示滞后语音 1-3s（围栏 finish 才发块） | §五.3 | 随 #4 同批 |
| 6 | 轮间驻留反直觉（回答无可见落点） | §五.6 | 随 #4 同批 |
| 7 | spec 键位漂移（KEY 未接，两键分工未落地） | F1 / 硬件速查 §一 | **accept-risk 已锚**（human 2026-09-10，pod-events d9e0f1a2），留交互批次定路线 |
| 8 | 首音时延 3-6s（四段串行云 RTT） | F7 / §五.4 | 待实验（依赖 #1） |
| 9 | 故事页排版（ksy_label 不换行横向裁切） | F5 / §四 A.1 | 待修包 |
| 10 | 字体（普惠子集缺字 + 设计字体未接正文） | §四 A.2 | 待修包（U 包边界待协商） |
| 11 | 课表页（数据填充 + 设计字体） | §四 A.3 | 待修包 |
| 12 | P2-A1 结构齿 first-match 歧义 | review-packet F2 / §四 D | 归 acceptance-author（随渲染批处理） |
| 13 | host 证据门 2 红（WSA-A4 flash.log / WSA-A6 ecs-teardown） | review-packet §4 | 归他包制品回收（supervisor/human） |
| 14 | 物理验收 OPEN 8 项（A2/A3剩/A4剩/A5/A7/A8/A13/A14） | device-acceptance-checklist | human 持板（F2-1 顺带戳实 #3） |

### 7.2 分包方案（合并逻辑 = 按验证环境分面 + 按依赖链排序）

| 批次 | 包 | 内容 | 合并进一包的逻辑 | 验证面 / 前置 |
|---|---|---|---|---|
| ① | **ECS 部署收敛**（Lite~Full） | #1 漂移修复 + #2 cron 部署 + live 五齿 evidence + #13 证据回收 | 同一 ECS 环境、同一次部署窗口、同一风险面（碰 live 配置）；human 已批部署窗口语义（pod-events 04174089） | ECS live；**解锁 #8 时延实验与提醒链真机** |
| ② | **设备显示与交互主权**（Full，先 PROPOSAL/grilling 定方案再拆实现包） | #4 主权收口 + #5 显示先行 + #6 回答驻留 + #7 键位路线落地 | 同一个设计问题「孩子看到什么/何时看到/怎么操作」——三屏归一决定驻留怎么做、键位决定状态机输入，拆开必返工 | 真机观感 + host 结构齿；无硬前置 |
| ③ | **渲染观感殿后**（Lite，可与②串行） | #9 排版 + #10 字体 + #11 课表页 + #12 齿修复 | 同一渲染层（story_page/schedule 页 + 字体资产）、同一 host 冻结齿验证面；P2-A1 齿歧义顺手在此批修 | host runner 冻结齿（story_p3 八齿）；#10 先与 U 包协商边界 |
| ④ | **故事断链修复**（Full，等判据） | #3 双断链收敛 | 跨固件+front+引擎，修法二选一需 F2-1 真机判据 | **前置：#14 持板时跑 F2-1** |
| 流程 | 时延实验（#8）：配置实验非代码包，随批次①后进行，若上流式 ASR 再立 provider 包；物理验收（#14）：human 持板操作，随时穿插；loop-retro：closeout 触发阈值 10≥3，主线程 cadence 责任 | | | |

**总排序**：① → ② → ③ → ④；每个包走 `/triage-change` 四档判定 + human 确认后立项。
（2026-09-11 修正：原「②③无共享文件冲突可前后接续」经 architect triage 核查**不完全
成立**——③ 的课表数据链与 #9 screen_lines 接线与 ② 重构面（kidpal_render_node.cc /
mcp_server.cc）同文件，锁定 **② 先 ③ 后**。）

**立项落盘登记（2026-09-11，/triage-change 全批次过完）**：

- **批次①** = `taskpackages/260911-ecs-deploy-convergence/`（②单包 Full / riskTier=high，
  human 触点①已确认，planner `/build-feature` 可开工）。
- **批次②** = `.planning/260911-display-interaction-sovereignty/`（③大 feature，
  PROPOSAL DRAFT v0.1 + review.md；**freezeState: pending-human**——待 human FROZEN
  + grilling OQ1 三屏方向 / OQ2 显示机制 / OQ3 驻留形态 / OQ4 键位路线 后才拆实现包）。
- **批次③** = `taskpackages/260911-render-pages/`（②单包 Full / riskTier=medium，
  human 触点①已确认含 P2-A1 方案 B 修法与 ② 先 ③ 后；开工顺位在批次②后）。
- **批次④**：不立项，等 F2-1 真机判据（#14 持板时顺带戳实）。
