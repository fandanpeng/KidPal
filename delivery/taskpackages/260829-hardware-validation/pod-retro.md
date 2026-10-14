# Pod Retro · 260829-hardware-validation（Track B 全环：硬件真机验收——从开机即挂到语音轮 0 崩溃）

> 收口相位制品（pod-retro 归 planner 收尾相位，本次按收口派单由 integrator 执笔落盘；本包当日
> 全过程有逐事件热记录 `.claude/taskpackages/260829-hardware-validation/pod-events.jsonl` 38+ 事件
> 与 PROGRESS-SUMMARY-260910.md 在案，本文以它们为热记忆底稿，**隔场代笔记已声明**——R1 义务由
> 台账时间戳链承担）。乘客 = planner Pod（acceptance-author / builder / fixer / verifier 多实例），
> harness 形态一句话：机械闸 + 多子 agent 派工，human 只在四个真正判断点出场（扩界 / ECS 窗口 /
> 换皮接线 / 渲染节点接线）。

**【硬项 R1 · 环后趁热、第一人称】** 见上——本包未隔夜（2026-09-09 14:29 intake → 09-10 17:20
最后一事件），逐事件台账即热记忆；代笔声明如上。

**【硬项 R2 · 计量诚实】** 本环无 token / 时长逐相位计量（真机轮烧录-复烧-目视的挂钟时间无法
从台账机械推得，约 09-09 23:20 首烧 → 09-10 14:10 轮4 全绿，跨约 15 小时含等待与人工持板段）。
§1/§2 以台账时间戳为准，误差界 ±（等待 human 目视与持板操作段不可分辨）。

## 1. 时间账

硬数据锚点表（pod-events eventId / 时间戳链）：

| 相位 | 内容 | 台账锚点 | 时长（挂钟） |
|---|---|---|---|
| intake + 冻结 | launch 闸 → human 扩界×2 → acceptance Finalize（16 AC + crossEnd 16 counterparts） | `4eac83ab`~`109ca6d8`（09-09 14:29→15:13） | ≈45min |
| builder + verify | F2 接线 + RTC/SHTC3 driver 落地 → scoped 7 闸 → final 9 闸全绿 | `2470b34f`~`020cd787`（17:10→17:23） | ≈15min（闸） |
| 真机轮·传感器链 | 首烧 boot hang → I2C 恢复 → BCD → SHTC3 三步（wake/stretch/**命令字错值**） → boot5 全绿 | `91afc0c4`~`c3932295`（09-09 23:20→09-10 02:47） | ≈3.5h |
| 真机轮·显示链 | 换皮不可见 → human 裁接线 → art 分区 → 渲染节点上线 → 首页内容渲染 → 单击修复 | `fd6e84d1`~`b0b63dd2`（00:46→06:27） | ≈5.5h（含 human 目视等待） |
| 真机轮·语音链 | 轮1 esp_timer 溢出 → 轮2 ssl_receive 溢出 → 轮3 SetStatus 悬空 → **轮4 全绿 0 崩溃** | `7c1f4a9e`~`9e4f6a8b`（11:05→14:10） | ≈3h |
| 收尾 | consolidated commit `3edbb24` → 链路梳理 + 更正 + 自 review（`2c753ab`） → integrator closeout | `c1d2e3f4`~`a9c0d1e2`（15:05→17:20） | ≈2h+ |

对照：该 workstream 纯 host 包（char-stroke / story-p3）单日即收口；本包是首个 device 宿主包，
真机物理节奏（烧录 2min + 复烧验证 + human 目视）成为主循环周期，fixBudget 消耗集中在传感器与
崩溃链四轮复烧——预算结构完全不同。

## 2. Token 账

计量方式诚实声明：无逐实例 token 计量（台账未记）。估计法：本包四个子 agent 角色 + planner 主
线程 + 两次链路梳理大文件产出（VOICE-ROUND-FLOW 347 行 + 自 review 修 7 处），粗估落在大文件
撰写与逐文件链路核对；误差界 ±50%，不装精确。花费最大处：轮3 崩溃的跨任务栈腐蚀归因（假象
任务名轮换 + backtrace 腐蚀，靠 addr2line 完整 backtrace 一解）与 VOICE-ROUND-FLOW 链路梳理。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · planner 凭印象给错硬件命令字，烧掉一整轮**：现象 boot4「shtc3 cmd err」测量命令 NACK →
  根因 planner 上轮给 fixer 的 SHTC3 测量命令 0x7852 是**臆测值**（非 Sensirion 命令表内命令），
  WebSearch 核对数据手册正确值 = **0x7866**（Measure T first, clock stretching disabled）→ 证据锚
  pod-events `ec755e27`（「诚实留痕：错值源自 planner 未比对 SHTC3 命令表」）+ PROGRESS-SUMMARY
  §二.3。教训已成文：传感器命令字/寄存器以官方数据手册为准，不臆测（§三第一条）。
- **F2 · 跨任务栈腐蚀的「任务名诈骗」**：现象轮1 报 esp_timer 溢出、轮2 报 main 溢出——**报错误
  任务名与 backtrace（SP=垃圾值腐蚀）都是症状非病因** → 根因 = 整条 MCP 解析链跑在 esp-ml307
  `ssl_receive` 任务（esp_ssl.cc:61，栈仅 4096B），溢出踩进堆内邻接 main 栈区，canary 先在别处绊倒
  → 证据锚 pod-events `2f8e1c47`（诊断链）+ `5b9d2e7f`（轮3 LoadProhibited backtrace **完整未
  腐蚀** → addr2line 一解即中 SetStatus 悬空）。方法论沉淀：坏栈不盲猜，先造「完整未腐蚀
  backtrace」的条件再 addr2line。
- **F3 · fixer 子 agent 多次因 API 配额提前终止**：现象 fixer 中途断流 → 根因配额限制非任务问题
  → 处置 = SendMessage 恢复同一实例续做（保留已读上下文），不重派（PROGRESS-SUMMARY §三）。
  无独立 eventId（发生在子 agent 会话内），_anchor_ = PROGRESS-SUMMARY §三第三条。
- **F4 · 「殿后转手」落下真机无 owner 的窟窿**：现象 human 持板两次报「界面还是老的」→ 根因 =
  换皮消费接线与设备渲染节点在 U 包/home_p1 包之间殿后转手（art_assets.c:10 自注「殿后 9b」、
  home_p1.c 注「不在本包范围」），9b 派单时 planner **漏列**这两件 → 两次 human 目击 + 两轮返工
  （pod-events `fd6e84d1`、`ebbbcb23`）+ 两次扩界裁定（`35511573`、`a1e7a716`）。教训：**殿后项
  必须有 owner 字段**，「后包兜底」不等于「后包派单自动包含」。
- **near-miss（最险一刻）**：**ECS 部署漂移差点造成事故级覆写**。链路梳理阶段发现
  `apply-xiaozhi-overlay.sh:60/62` 仍写 ASR=AliyunStreamASR / Intent=function_call，与 live
  （Qwen3ASRFlash / nointent）不符——**任何人在云端调优前顺手跑一次 overlay，就会把已验证的
  live 语音链覆盖断掉**（VOICE-ROUND-FLOW §七.5 已钉为一切云端实验的前置项）。没出事纯粹因为
  当日没人跑脚本。教训：部署脚本与 live 的漂移要在「部署窗口开启」之前修，而不是之后。
- **near-miss（第二险）**：story_text 渲染经 Schedule 在 main 任务调 `kidpal_story_page_render`，
  其内部 `lv_screen_active()+lv_obj_clean()` 清掉了小智聊天屏控件——若不是渲染节点恰好有 1s tick
  把前台切走，这个悬空指针会在任何一次 story 下发时随机爆发。根治（begin_page/end_page 夹钳）是
  被轮3 崩溃逼出来的，不是设计出来的。

## 4. 优化建议（按 ROI 排序）

1. **「殿后项登记带 owner 与接收包派单核对」**：治 F4 · 载体 = 固化候选 → ARQ 收件箱（殿后项
   在 taskpackage / PROPOSAL 里必须带 `owner:<changeId>`，后包 launch 前 planner 核对上游
   殿后清单）· 预期收益：消灭「后包不知道自己要兜底」整类缺口，本包两次 human 目击返工直接消失。
2. **「硬件寄存器/命令字零臆测」**：治 F1 · 载体 = 登记不硬治（写入 dev-loop 角色指引 /
   PROGRESS-SUMMARY §三已留痕）· 预期收益：每省一轮复烧（≈20–30min 挂钟 + fixer 预算）。
3. **「部署脚本=live 一致性进 pre-deploy 闸」**：治 near-miss #1 · 载体 = 留批次（ECS 漂移修复
   立项时一并落 `apply-xiaozhi-overlay.sh` vs live 的 diff 检查）· 预期收益：杜绝事故级覆写。
4. **「诊断埋点先行」惯例化**：正收益复判（非治痛点）· 载体 = 登记不硬治 · 轮1 的音频管道三段
   埋点 + 栈高水位让轮2/轮3 复现时直接读出断点——「改崩溃先埋点」值得成为默认动作。

## 5. 一句总结

本包是该 workstream 首个 device 宿主包：真机物理节奏把「挂钟一天」打满（传感器链 3.5h + 显示链
5.5h + 语音链 3h），human 出场 5 次（扩界×2 + 接线×2 + 持板目视多次）但每次都是真判断点；最险
一刻是 ECS overlay 漂移差点断 live 语音（没人跑脚本纯属侥幸）；**保险丝判定：两方独立结构基本
接住作者偏差**——verifier 闸全绿的是代码面、真机面由 human 持板目视独立证伪（「界面还是老的」
两次戳破派单遗漏），唯一未接住的是 hardware_validation 结构齿对 first-match 歧义无免疫
（P2-A1 先行红，见 review-packet F2）；最值得带走的一条：**真机证据（human 目视 + 串口日志）
与 host 闸绿是两个独立事实源，dev-loop 第一次同时要两者**。

## 6. 本环遗留（批次型变更特有）

- **物理验收 OPEN 六项 + 续航双项**（HWV-A2 长按/消抖、A3 录音整轮、A4 断电对时、A5、A7、A8、
  A13、A14）→ human 持板，执行面 = device-acceptance-checklist.md。
- **F2 断链①②**（StopListening 无调用方 + manual 期间无 stt 帧）→ F2-1 真机戳实后立修包
  （pod-events `f7b8c9d0` next 字段已载）。
- **ECS 三件**：部署漂移修复（前置项）→ 提醒 cron §8 部署 → live 五齿 L1-L5 evidence 交班。
- **设备观感三件**：故事页排版（F5，ksy_label wrap + 分页）、字体（U 包边界，设计字体接正文）、
  课表页内容填充。
- **spec 两处待同步**：键位漂移处置（补接 KEY 或 Track B 过渡修订）、VOICE-ROUND-FLOW 结论
  review 后再同步 spec。
- **host 3 红回收**：WSA-A4/A6 证据门归他包/主线程；P2-A1 齿歧义交 acceptance-author。
