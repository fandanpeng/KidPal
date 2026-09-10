# 设备验收清单（逐项勾验记录）· 260829-hardware-validation（包 9b）

> 【归位注记】本文件目标位置 = `.claude/taskpackages/260829-hardware-validation/
> device-acceptance-checklist.md`（launch 指令 C 钉的路径），但 enforce-scope 对
> acceptance-author 只放行 `firmware/**/*.acceptance.c(pp)` /
> `firmware/**/acceptance/**` / `server/**/acceptance/**` / `tools/**` —— 该路径被
> 机械拦。按 voice-roundtrip 红基线摘要同款先例，先落 scope 内
> （`firmware/main/test/acceptance/device-acceptance-checklist.md`），请 planner /
> integrator 原样归位到 taskpackage 路径。断言/清单内容零变更。
>
> 执行面：human 持板执行 + integrator 落盘证据。本清单是 HWV-A2~A8 + 续航基准 +
> 证书握手 + F2 真机端的**执行与回写**面，每条映射「烧录/操作步骤 + 待记录证据字段 +
> 判红判绿标准」。验收结论回写本文件表格（勾 + 证据字段 + 判语），并随
> `changes/dev/260829-hardware-validation/` 落盘原始证据（串口日志 / 烧录日志 /
> 照片 / 逐项记录）。
>
> **HWV-E2 证据纪律（序言，不得逾越）**：device 验收以**真机实测证据**（烧录日志 +
> 逐项勾验记录）落盘，**非模拟**。「没报错就算过」禁止；**probe ≠ exercise**——
> 每条必须真 exercise 一次真实路径（DEV-GOVERNANCE「验收测试纪律」§2）。host/模拟器
> 段只证明「布局/逻辑」，本清单只认真机事实。AEC 语音喊停（HWV-E4）**不在本包**，
> 归包 9a 单独隔离——本包只做物理冒烟 + 端到端，遇到喊停项仅记录「殿后 9a」不判色。

## 前置（烧录与接线，判红判绿共用的基线）

- 集成固件 = 全链路集成版（含 F2 接线 + RTC/SHTC3 driver + U 资产），烧录命令/环境
  见 skill `kidpal-idf-env` / `kidpal-serial-monitor`（COM5 单进程）。烧录日志落盘。
- 网关前置 = ECS 部署真落（DEPLOY-RUNBOOK §8：cron 声明 + 生产者脚本 + prompt v3 分工
  修正 + 引擎补丁认领 `kidpal_speak` + token/cert 就绪）；live 五齿 L1-L5 真跑证据在
  `changes/dev/260829-schedule-reminder/`。
- 证据字段速记：**log**=串口/烧录日志摘录；**photo**=屏幕实拍；**ticks**=逐项勾验
  记录；**into**=结论回写（尤其 HWV-A7/A6 的回写设计面）。

---

## 表 A：物理冒烟 + 端到端真机（原包 HWV-A2~A8）

| ID | 判红判绿标准 | 操作步骤 | 待记录证据字段 | 结论（执行后填写） |
|---|---|---|---|---|
| HWV-A2 | 绿 = KEY/BOOT 单击/双击/长按在真机各页触发与 §4.0 表格一致；消抖有效（无一次按出两次响应）；职责分离（KEY=对话、BOOT=翻页，对话中 BOOT=结束）。红 = 任一手势窜义/连击/手感不可用 | 逐页（P1/P2/P3/P4）+ 对话态逐手势按 KEY/BOOT 一次，对照 §4.0 全局表；连按 20 次 KEY 看是否多触发 | 勾验表（页 × 手势 × 期望 × 实测）；消抖连按计数 |  |
| HWV-A3 | 绿 = 双麦录音清晰无噪声啸叫，录一段话可上云 ASR 正确转写（字对得上）。红 = 采音失败/啸叫/ASR 转写乱码 | 录「小伴你好，今天是星期三」上传，拿 ASR 转写逐字对照 | 录音回放清晰度判定；ASR 转写原文 vs 所说 | **部分绿(2026-09-10 真机)**：ASR 上行 ✅ 全句正确转写(voice-round2.log:25 `Application: >> 讲个故事`，与用户所说一致；Qwen3ASRFlash 配置修复后整句不再截断)。**音频输出管道 ✅ 全链路证实**(复位后欢迎音：`decode queue <- packet #1: sr=16000 fd=60` → `decode frame #1: decoded=960 -> out 1440 samples @24000Hz`(16k→24k 重采样，越界修复后) → `playback frame #1: 1440 samples -> codec (enabled=1)` → I2S 写出+ES8311 使能)。**双麦录音清晰度 + 故事录制整轮未 exercise**(story_text 处理崩溃阻断，ssl_receive 栈溢出修复中)——殿后补测。 |
| HWV-A4 | 绿 = RTC 上电走时正确、**断电后重新上电仍保持正确时间**（备份电池续航）；SHTC3 温湿度直读正确（对常温体温湿计误差 ≤±2℃/±5%）。红 = 断电丢时/走时漂移/SHTC3 读全错或读不出 | 对时→断主电（拔 18650）≥ 10 分钟→重新上电看时间；SHTC3 读值与参照温湿计对比 | 断电前/后时间戳；SHTC3 读数 vs 参照 | **部分绿(2026-09-10 真机)**：RTC 走时 ✅ 多次开机直读正确(voice-round2.log:100 `rtc_pcf85063 corr_id=kdp-251 ok rtc=20:48:21`，BCD 解码修复后)；SHTC3 直读 ✅(`shtc3 corr_id=kdp-251 ok temp_c=28.52 rh_pct=46.5`，与室温吻合，wake 0x3517+measure 0x7866+no-stretch 修复后)。**断电续航子项未 exercise**(需拔 18650 ≥10min 再上电对时)——殿后补测，当前不判全绿。 |
| HWV-A5 | 绿 = 电量至打盹阈值（15%）真机触发低电打盹（首页电池闪 + 降功耗演出）；RLCD 真屏固定点位/时间对比度、可视角度、刷新三项观感可接受。红 = 打盹不触发/触发即冻屏；RLCD 观感不可用 | 放电至 15% 观察打盹；RLCD 三观感项各照一张（正视角/斜视角/滚动刷新） | 打盹触发时电量与演出截图；RLCD 观感三张 photo + 判语 |  |
| HWV-A6 | 绿 = BOOT 循环翻页 P1→P2→P3→P4→回绕 P1 无损（HWV-E3 兜底）；各 story 主流程（课表查询/查字笔顺/提醒弹窗/故事录制回看/通用问答）真机走通；换皮后 UI（含 U 资产 47 枚）可辨认、吉祥物/框/网点/印章正常显示。红 = 翻页断裂/任一主流程真机不可用/换皮资产错位缺失 | 循环 BOOT 翻 4 页一周期 ≥2 轮；每个 story 主流程走一遍；逐页核对 U 资产显示 | 翻页周期记录；各 story 主流程勾验表；换皮 UI photo + 判语 | **部分绿(2026-09-10 真机)**：BOOT 双击 4 页循环 ✅(home→char→schedule→story→回绕，用户实测翻页正常，HWV-E3 兜底达成)；换皮 V3 UI ✅ 首页可见(用户实测「前3个都可见+电池图标有」，render node pushing home 400x300 render_rc=0，art SPIFFS mounted /art 47 枚 KAP1)；单击→语音通道 ✅(进 listening，ASR 全句)。**story 主流程(故事讲述/回看)真机走通 ✅(2026-09-10 轮4 voice-round4.log)**：单击→ASR『能讲个故事吗？』→speaking→`story_text_dispatch screens=2`→故事阅读页画到 KidPal 屏(begin_page 修复后不再 clean 聊天屏)→LLM 回答《月亮上的补丁》→speaking→listening 平稳(轮3 LoadProhibited 崩溃点已根除)→用户『继续』再入 speaking，全程 0 崩溃 0 复位。课表页具体内容/字体仍为占位(用户报「没用课表具体内容」「字体都是正体非设计字体」)——殿后补测+回写设计面；故事正文分页观感(全文挤一屏超出)为已知 9b 殿后项。 |
| HWV-A7 | 绿 = **16px 及以下 CJK 在 1bpp 真机可读**（教学字/列表行/正文肉眼可辨认、不糊）；结论文案回写设计面（V3 字号下限）。红 = 16px 以下不可读 | 分别以 12/14/16px（及 16px 下界）渲染同一 CJK 字，真机肉眼判可读性，结论回写 | 各字号实拍 photo；可读性判语；**回写 V3 字号下限结论** |  |
| HWV-A8 | 绿 = 反射屏在暗处**确认无前光**（判定），明亮环境肉眼可读（spec §5.6 待确认项销账）。红 = （本项为确认项，非功能缺陷；判定有/无前光即可） | 暗处 + 亮处各照一张，判有无前光、明亮环境可读性 | 暗处/亮处 photo + 前光有/无误判定 |  |

## 表 B：v0.4 新增（续航基准 + 常驻 WS 功耗 + 证书握手 + F2 真机端）

| ID | 判红判绿标准 | 操作步骤 | 待记录证据字段 | 结论 |
|---|---|---|---|---|
| 续-1 (HWV-A13) | 绿 = 实机续航基准落盘（决策 21 三要素承载：触发点=本包、预警阈值、owner=本包）——满电到打盹（15%）的真实连续时长测出。红 = 未测/伪造数字 | 满电常连 WS 跑至打盹，记录真实时长 | 起始/结束电量 + 时长 + 日志 |  |
| 续-2 (HWV-A14) | 绿 = 常驻 WS 连接功耗观察落盘（常连 vs 断连的平均电流/瓦时对比，供决策 21 预警阈值定标）。红 = 无观测/只空跑 | 外接电流计测常连 WS 与断连两态功耗，各 10 分钟取均值 | 两态 mA/W 均值 + 日志 |  |
| 证-1 (HWV-A15) | 绿 = 证书真实设备握手冒烟通过（CA pinning 真机首连：自签证书 + 真实设备密钥首连成功，非 Loopback）。红 = 握手被拒/证书不匹配 | 真机上电首连 wss 网关，看握手日志 + 连接建立 | 握手日志（证书校验通过 + session 建立） | **绿(2026-09-10 真机)**：真机首连 wss://8.137.91.16:8000 自签 CA 握手通过——voice-round2.log:14-15 `WsTlsCa: wss cacert enabled: custom cert bundle path = main/certs/kidpal_ca.pem (esp_crt_bundle_attach trust face) — handshake verified against self-signed CA` → `esp-x509-crt-bundle: Certificate validated` → `WS: Session ID: 78eba971-...` 会话建立。CA pinning 真机首连成功，非 Loopback。 |
| F2-1 (HWV-A10/A11/A12 真机端) | 绿 = 真录一段故事→网关 archive_story 落盘 stories/*.md→story_text 组块下发回设备阅读→保存成功触发盖章帧；断开重连后会话不串味（故事不串进新会话）。红 = 归档缺失/正文不下发/盖章不触发/串味 | 真机录 1 段（含标点/停顿）→停录→回看列表/正文→看盖章；断开重连再录第二段核对不串 | 归档文件 stories/*.md 摘录 + story_text 下发日志 + 盖章 photo + 两段隔离勾验 |  |

## 表 C：live 五齿 L1-L5（包 7 承接，ECS + 真机轮执行后勾）

| ID | 判红判绿标准 | 操作步骤 | 待记录证据字段 | 结论 |
|---|---|---|---|---|
| L1 | 绿 = openclaw cron 任务真在册且真跑过（name/enabled/schedule.expr/payload.kind/state.lastRunStatus） | DEPLOY-RUNBOOK §8 部署后 `openclaw cron list` 摘录 | cron list 摘录 |  |
| L2 | 绿 = alerts/pending.json 真身落地且条目形状合契约 §三（id/fire_at/title/speech/view 逐字段） | 部署后看 ECS pending.json | pending.json 摘录 |  |
| L3 | 绿 = 到点真经 mcp kidpal/alert 推到设备（串口/front 日志摘录） | 等一个档位到点，看设备串口/front 日志 | 推送日志摘录 |  |
| L4 | 绿 = 渲染回执真回来且档位出队（回执摘录 + 无重复推送计数） | 设备渲染弹窗后看回执/出队日志 | 回执 + 出队计数 |  |
| L5 | 绿 = 真大模型语义产物（备注→带物 + 文案）落语义中间产物且与机械兜底可区分 | 看 semantic 产物 + 对照机械兜底 | semantic 产物摘录 |  |

## 回写与落盘纪律

- 每项结论「绿」必须同时有**证据字段**（log/photo/ticks/into）落盘；纯勾无证据 = 按 HWV-E2 判红不通过。
- HWV-A7 字号下限与 HWV-A6 换皮 UI 观感的**结论回写设计面**（`docs/ui/UIUX-DESIGN-BRIEF.md` / V3 口径），由 integrator 落地，本清单只产结论字段。
- 本清单结论 + 原始证据随 `changes/dev/260829-hardware-validation/` 交 verifier 独立核验（HWV-E2 证据完整性）。