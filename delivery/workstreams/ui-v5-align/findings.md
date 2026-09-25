# ui-v5-align · findings — V5 设计稿 vs 固件实现 差距登记

> 调查方法：主线程逐张读 `docs/ui/V5/` 十二张 400×300 锚图（管线 shoot_design 阈值化产物，
> 母版 = `docs/ui/kidpal-ui-design-v5.html`），逐文件读 `firmware/main/display/` 渲染面
> （home_p1.c / schedule_page.c / story_page.c / reminder_page.c / char_page.cc /
> char_stroke.c / lcd_display.cc / keybar.c / kidpal_voice_states.c / voice_round.c /
> art_assets.*），并核对 `firmware/main/assets/spiffs_root/` 134 枚资产的消费面与
> `docs/requirements.md`（main，v1.57 已含 260922-spec-v5-ui-align 对齐）的裁定。
> 登记日期 2026-09-24。行号引用为调查时 main  HEAD（b35d3ed）实况。
> 复核 2026-09-25（10a099c 后：spec v1.58 四态线框重画、设计稿提醒右区收敛单一文本框）：
> 设计列与性质列按 v1.58 订正（订正处均带 260925 注）；实现列全部未动
> （b35d3ed..HEAD firmware/ 零改动，实现行号仍准）。锚图 PNG 为 1bpp，本次复核以母版
> HTML 为据（锚图本即母版阈值化管线产物）。

## 0. 读法

- 「设计」= V5 锚图实况；「实现」= 当前代码实况；「spec」= requirements.md（main）裁定。
- 标注 **[spec 已钉]** = 该差距同时是 spec 要求（实现落后 spec+设计双方）；
  **[纯视觉]** = spec defer 设计稿（「视觉形态归设计稿」）；**[冲突 Cx]** = 见 task_plan 裁决表
  （该表已随 task_plan 删除、见 §7 终裁；C2–C5 标签保留原登记，冲突内容以各行列述为准）。

## 1. 跨页全局

1. **字体族**：设计全文圆体手写感（ZCOOL KuaiLe 一路）；实现全屏钉 `font_puhui_14_1`
   （lcd_display.cc:1152/1176、schedule_page.c:668、char_page.cc:645/659、story_page 继承屏字体）。
   管线有 `font_zcool_30.bin` 单档未接线。**字号（260925 补登）**：spec v1.58 钉四态文本区
   13px 同字号，实现 14px（font_puhui_14_1）——同源差距，随字体轮一并裁。[冲突 C4]
2. **墨色**：设计纯 1bpp 黑白；实现灰阶常量遍地——schedule_page.c:650-652（0x222222/0x999999）、
   story_page.c:1105-1109（0x222222/0x333333/0xDDDDDD）、char_page.cc:209-220（0xd0/0xe6 引导线）、
   char_page.cc:269（0x99 点链）。RLCD 阈值化后灰阶层次丢失。[纯视觉]
3. **键帽条**：胶囊造型（高 26/圆角 8/3px 描边白底黑字，lcd_display.cc:1035-1094）与基层三页文案
   （说话/翻页 · 问课程/翻页 · 说话·双击看/下移，keybar.c:44-46）、五态键帽（说完啦/不等了/打断/
   知道了 + 结束/关闭，keybar.c:68）**与 V5 逐字一致** ✓。差距：
   - 页点：设计非当前页 = 空心环；实现 = 8px 实心缩点（lcd_display.cc:1348-1350）。[纯视觉]
   - 录音态键帽收尾/结束（spec WF-F2-02 已钉）、阅读态念给我听/翻屏（spec WF-F2-04/锚图）、
     查字窗重播/结束（spec F3 线框已钉）在 keybar 值域**不存在**（keybar.c:43-68），
     落错基层槽。[spec 已钉]
   - 录音态页点：spec WF-F2-02「（无页点）」；实现录音 = P3 基层页态 → 页点显示
     （keybar.c:26 overlay_top 判定不覆盖此态）。[spec 已钉]
4. **资产消费率**：134 枚中 V5 件「已创建未消费」：bub_listening/thinking/speaking/recording/
   rec_guide（全气泡）、doodle_cloud_52、rec_wave、arc_listen、qmark_trio、bell_trio、
   stamp_seal_r8、p1_chip_jiexialai/mingzao（黑底白字帽标）、imgfont_letter_*+colon（拼音字模）、
   pose_record_96、ic_wifi_14、frm_flat_240_96、frm_sm_160_120、ic_star_* 族、ic_mic_22/32、
   voice_chip_p3、time_10_24。已消费：pose 族五枚、bubble_tail_24、ic_mic_16、question_16、
   frm_236_236、d50、thermo_man/drop_baby/ic_batt_48、imgfont 数字/%、p1_label_*（白底黑字版）。
   [纯视觉·接线]

## 2. 逐屏差距

> **挂起注（2026-09-24 human 复裁）**：P3 五屏（列表/阅读/录音/删除确认/盖章）差距照登不删，
> 但本批不拆包——M4-star-notebook 正在开发 P3 页面本体；M4 合并入 main 后以本章各表为
> 二次对齐清单。P3 行内 [spec 已钉] 条目 likewise 随 M4 包一并兑现或届时复核对账行。

### P1 首页（canvas-p1）— home_p1.c

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 状态栏首部 WiFi 小标 | 无（ic_wifi_14 未消费；辅助条自时间起 x=36，home_p1.c:891） | [spec 已钉] AC1「📶 联网小标打头」 |
| 2 | 日期 "Tue 22 Sep"（星期简写+日+月） | "22 Sep" 无星期（home_p1.c:925-959）；hp1_state_t 无星期字段 | [spec 已钉]+[冲突 C5] |
| 3 | 温/湿/电图标前置小字 | ✓ 一致（thermo_man/drop_baby/ic_batt_48 缩放接线） | 对齐 |
| 4 | 分组点状虚线在「接下来」组与「明天」组之间；状态栏下无线 | 虚线画在状态栏下 y=42 通栏（home_p1.c:1123-1150）；组间无分隔 | [纯视觉] **（260926 订正：本行设计列「状态栏下无线」为误读——V5 母版 canvas-p1 辅助条 border-bottom dotted 自首版在世（git log -S 证）、锚图 y25-26 实测点状线，实现侧 y42 即其适配位。阶段 2 批按母版恢复 y42 线 + 增补组间虚线，两件并存不争位）** |
| 5 | 「接下来」= 黑底白字圆角帽标 + 左侧云朵涂鸦 | 白底黑字烘焙字 p1_label_jiexialai.bin（home_p1.c:1074）；无云 | [纯视觉]；黑帽版 p1_chip_jiexialai.bin 在世未接线 |
| 6 | 「明天」帽标 + 多项逐行（春游+↕临时、编程 19:00+★） | 「明早」帽标（home_p1.c:1105，文案与资产停旧版）+ 单条 preview（hp1 单槽）；无★/↕临时（hp1_item_t 无类型字段） | [spec 已钉] AC2 明天区多项；[冲突 C3/C5] |
| 7 | 左下波浪涂鸦 | 无（rec_wave 未消费） | [纯视觉] |
| 8 | 猫咪左中常驻 | pose_daily_96 @ (24,80) 大致吻合（home_p1.c:1046） | 对齐 |
| 9 | 无倒计时贴纸 | 贴纸已退役（home_p1.c:1053 注） | 对齐 |

### P2 课表（canvas-p2）— schedule_page.c

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 7 列×3 槽网格 + 行标签列 + 表头 | ✓ 骨架一致（schedule_page.c:810-830） | 对齐 |
| 2 | 今天列粗黑框 | ✓ 2px 边框列高亮（schedule_page.c:835-845） | 对齐 |
| 3 | ★/※ 标记前置 | ✓ 字形+程序化兜底（schedule_page.c:741-779） | 对齐 |
| 4 | 表线细黑 | 灰 0x999999 2px（schedule_page.c:652/811-817） | [纯视觉] |
| 5 | 外框完整（含顶边/最左边） | 缺顶边与 x<39 左边（schedule_page.c:810-817） | [纯视觉] |
| 6 | 行标签垂直居中 | 顶对齐 y=slot+6（schedule_page.c:828） | [纯视觉] |

### P3 列表（canvas-p3）— story_page.c:1210-1255

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 选中行 = 粗描边圆角外框 + 黑字 | 实心 0x333333 反白 + 白字（story_page.c:1242-1245） | [spec 已钉]（v1.31 真机效果差裁定带边框，requirements §P3 对账行登记在案） |
| 2 | 行首★图标 | "●" 字符（story_page.c:1247） | [纯视觉] |
| 3 | 行间点状分隔线 | 无 | [纯视觉] |
| 4 | 右上 ★ x5 | 实心方块行 + "×N" 文本（story_page.c:1234-1238） | [纯视觉] |
| 5 | 虚线圆角开录框 + mic 图标 + 「我要讲故事 / 帮我记下来」双句 | 单行纯文本「说"我要讲故事"开录新的」（story_page.c:1253） | [spec 已钉]（对账行登记补「帮我记下来」） |
| 6 | 无页内标题 | 左上「我的故事」标题（story_page.c:1222） | [spec 已钉]（对账行登记应无标题） |
| 7 | 右下「第1屏·共3屏」分屏指示 | 无（仅阅读态画屏数） | 功能债关联（分屏翻页，见 §4） |
| 8 | 条目>5 时窗口化分屏 | 全量行 y=60+i*34 直排，超屏溢出不可见（story_page.c:1240） | 功能债关联 |

### P3 阅读（p3-reader）— story_page.c:1257-1303

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 标题 + 波浪线 + 通栏实线下划线 | 仅标题（story_page.c:1263） | [纯视觉] |
| 2 | 屏指示在键帽条上方居中 | 方格 tick y=250 + 标签 y=276 均压进键帽条带 y252-300（story_page.c:1295/1300） | [纯视觉·缺陷] |
| 3 | 键帽念给我听/翻屏 | 落基层槽说话·双击看/下移 | [spec 已钉]（WF-F2-04） |
| 4 | 页点显示（第 3 点实心） | ✓ 基层页态页点在 | 对齐 |

### P3 录音（p3-recording）— story_page.c:1305-1328

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 右上「● REC」 | 左上闪方块 (20,20)（story_page.c:1314-1318） | [纯视觉] |
| 2 | ~40px 粗体大字计时 | 14px 文本 (60,18)（story_page.c:1321） | [纯视觉] |
| 3 | 粗锯齿横向波浪 | 6 根竖条 (60+i*20, 90)（story_page.c:1324-1327） | [纯视觉]；rec_wave 在世未消费 |
| 4 | 录音姿猫咪左中 | 无（pose_record_96 未消费） | [纯视觉] |
| 5 | 有框气泡「跟着感觉说，我在记！」 | 无（bub_recording/bub_rec_guide 未消费） | [纯视觉] |
| 6 | 「我：…」回显字幕行 | 无 | [冲突 C6]（echo 机制在世，显示=接线） |
| 7 | 键帽收尾/结束 + 无页点 | 基层槽 + 页点显示 | [spec 已钉]（WF-F2-02） |

### P3 删除确认（p3-delete-confirm）

实现**无此状态**（story_page 仅 LIST/READER/RECORDING/STAMP 四态；无键帽值域）。
spec v1.48 AC4 已定义（BOOT 长按弹/KEY 确认/BOOT 取消/10s 静默取消）= 功能面未实现。
**登功能债 §4，本批不拆包**（human 裁不改功能）。

### P3 盖章（p3-stamp-frame）— story_page.c:1330-1384

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 双圈：外实 + 内**点线** | 内圈细实线代虚线（story_page.c:1356 注自认 LVGL9 无便捷 setter） | [纯视觉]；stamp_seal_r8.bin 在世未消费 |
| 2 | 圈内★ + 「星星徽章」文 | 圈内空 | [纯视觉] |
| 3 | 圈外散星 | 无（doodle_star_26 未消费） | [纯视觉] |
| 4 | 文案「『星星徽章』+1 颗已入册！」锚图 / spec 逐字「星星徽章·第 N 颗已入册！」 | 「故事徽章·第 N 颗已入册！」（story_page.c:27 KSTORY_STAMP_FMT） | [spec 已钉]（对账行登记改名）；锚图与 spec 逐字式差异 = 设计稿内部口径，随 C1 类文案批核 |
| 5 | 猫咪居中压文案 | pose_jump 贴右下角（story_page.c:1380） | [纯视觉] |
| 6 | 锚图无键帽条 | 键帽常驻 | [冲突 C2]（spec v1.57 钉常驻） |

### 语音三态（state-listening/thinking/speaking）— lcd_display.cc:1109-1628

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 气泡左上包标头文字（圆角框+尾） | 标头裸文字 (124,16) + 单枚 24px 气泡尾 (102,12)（lcd_display.cc:1148/1573） | [纯视觉]；bub_* 全气泡未消费 |
| 2 | 猫左中 (≈25,100)-(110,190) | pose 挂点左上 (16,36) 96×96（lcd_display.cc:1128-1130） | [纯视觉] |
| 3 | 标识件贴猫旁：聆听弧/三问号飘猫头/均势粗条 5 根≈35px | 贴标头文字后：mic 16px (188,15)/单问号 16px (172,15)/4 根 3×6 小条 (158,15)（lcd_display.cc:1588/1601/1613-1624） | [spec 已钉]（260925 升格：v1.58 版式通则钉弧位元素各态专属、56×56 同位贴猫；造型细节仍归设计稿）；arc_listen/qmark_trio 未消费 |
| 4 | 云右上、波浪左下 | 无 | [spec 已钉]（260925 升格：v1.58 版式通则左演出层脚下波浪线＋右区顶部云朵入 spec） |
| 5 | 标头文案：我在听！✓/让我想想…/听我说哦（260925 订正：原记「听我说啦」为误读，母版自 54c2523 起恒为「哦」，git log -S 证） | 我在听！/思考中/播报（kidpal_voice_states.c:27-29 冻结闭集） | [spec 已钉]（260925 重标：v1.58 锚点列采设计口径，实现闭集落后 spec+设计双方；原 [冲突 C1] 裁决表已随 task_plan 删除，按 v1.58 现行口径重标） |
| 6 | 聆听回显行右栏 | ✓ echo_box 右列可滚（lcd_display.cc:1159-1177） | 对齐（v1.58 线框去「我：」前缀 = spec 旧口径自纠；设计母版与实现〔firmware 无「我：」串〕本就无前缀，三方归一） |
| 7 | 思考态右栏「你刚说」引导行 + 末句 | 无引导行（echo 整列直排） | [spec 已钉]（260925 升格：v1.58 锚点列＋WF-B-02 钉「你刚说」标签＋刚问的问题） |
| 8 | 无页点 | ✓ 覆盖层页点隐藏 | 对齐 |

### 提醒（state-reminder）— reminder_page.c:1187-1308

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 开阔式：无压暗无卡片 | d50 网点整屏压暗 + 白底 3px 描边圆角卡（reminder_page.c:1226-1251） | [spec 已钉·废形态]（§2.2 提醒行：v1.13 卡形态作废、网点压暗消解；WF-B-04 同语音骨架） |
| 2 | 气泡「记得哦！」左上 | REMIND look.bubble = NULL（kidpal_voice_states.c:60） | [spec 已钉]（260925 重标：v1.58 锚点列明确含气泡「记得哦！」；原 [冲突 C1] 裁决表已随 task_plan 删除，按 v1.58 现行口径重标） |
| 3 | 猫左中 + 三铃铛组贴猫右上（260925 订正：原记「太阳/铃铛 trio」为锚图阈值化误读，母版为三枚 ic-bell） | 卡内左上 pose-jump 矢量占位+位图（reminder_page.c:1254-1279）；bell_trio 未消费 | [spec 已钉]（260925 升格：v1.58 版式通则钉三铃铛组 56×56 与三问号组同位〔弧位〕；卡内位置即落后） |
| 4 | 右区单一文本框（260925 按母版收敛后实况改写）：时间＋事项同行、一行一条、多事项换行——「明天 07:45 记得带画具！／明天 08:10 水壶也别忘了哦。」，时间内联加粗（数字字体 800）、13px/700/1.7 四态同字号；独立时间行与 25px 大字均为母版明弃形态 | 卡内 200% 大字 title + HH:MM 行（无「明天」日词）+「KEY OK」工程占位字（reminder_page.c:1284-1301）；大字 = 母版明弃 25px 大字旧形态的复活 | [spec 已钉]（WF-B-04 v1.58 单一文本框口径）；speech 上屏=接线 |
| 5 | 键帽知道了/关闭 | ✓ 一致（keybar.c:68） | 对齐 |

### 查字窗（char-window）— char_page.cc

| # | 设计 | 实现 | 性质 |
|---|---|---|---|
| 1 | 手绘大框 + 米字格八向**点线**（含对角） | frm_236_236 ✓ 接线；引导线 = 灰实线十字+四宫格、无对角（char_page.cc:204-222） | [纯视觉] |
| 2 | 框内左上「跟着写」黑帽标 | 无 | [纯视觉] |
| 3 | 右栏拼音大字手写感（带调，≈40px） | 拼音与正文同 14px（char_page.cc:631-645）；imgfont_letter_* 未消费 | [纯视觉] |
| 4 | 右栏行序：拼音/共N画/部首/组词 + 底部★ | 五行含「释义：」行（spec F3 钉五行含释义）、无★ | [spec 已钉] 释义行保留（spec 权威）；★ = 锚图附加，随视觉批核 |
| 5 | 逐笔三态 + 6px 点链 | ✓ 一致（char_stroke.c 契约） | 对齐 |
| 6 | 键帽重播/结束 + 无页点 | 键帽值域缺（落基层/语音槽）；页点 ✓ 隐藏（覆盖层） | [spec 已钉]（F3 线框） |
| 7 | 底部无进度文字（锚图） | 提示条「第 X 笔·共 N 笔」y244（char_page.cc:654） | spec F3 AC2 钉进度文案上屏 = spec 权威，锚图缺漏视之（同 C2 类登记） |

## 3. 资产消费核对表（V5 件在世未消费）

| 资产 | 设计用途 | 消费点现状 |
|---|---|---|
| bub_listening/thinking/speaking.bin | 三态全气泡框 | 仅 bubble_tail_24 单件（lcd_display.cc:381） |
| bub_recording/bub_rec_guide.bin | 录音态气泡/开录虚线框 | 零消费 |
| doodle_cloud_52.bin | P1 接下来云 + 语音态右上云 | 零消费 |
| rec_wave.bin | P1/语音左下波浪 + 录音粗波浪 | 零消费 |
| arc_listen.bin | 聆听弧 | 零消费 |
| qmark_trio.bin | 思考三问号 | 仅 question_16 单枚 |
| bell_trio.bin | 提醒三铃铛组（260925 订正：母版为三枚 ic-bell、无太阳） | 零消费 |
| stamp_seal_r8.bin | 盖章双圈点线印章 | 零消费（矢量双圈代） |
| p1_chip_jiexialai/mingzao.bin | 黑底白字帽标 | 零消费（p1_label_* 白底版在世） |
| imgfont_letter_*/colon | 拼音带调大字 | 零消费 |
| pose_record_96.bin | 录音姿猫 | 零消费 |
| ic_wifi_14.bin | 状态栏 WiFi 小标 | 零消费 |
| frm_flat_240_96/frm_sm_160_120 | 开录虚线框/删除确认行框/选中行框族 | 零消费（仅验收齿引用） |
| ic_star_18/20/22/38.bin | 列表行首★/★ x5/盖章散星/查字★ | 零消费（P2 用字体字形★） |
| voice_chip_p3.bin | P3 录音标识件 | 零消费 |

## 4. 功能债登记（本批不拆包，human 裁「不改功能」）

1. **删除确认屏**（p3-delete-confirm）：spec v1.48 AC4 全交互流未实现（状态/按键/云端同步/10s 取消）。
2. **列表分屏翻页键语义**：spec §2.2 P3 行「BOOT 短按光标下移（分屏翻屏、到底 wrap）、双击跳屏」
   未实现；关联视觉件（窗口化 + 第 X 屏·共 Y 屏指示）若 triage 判纯呈现可入 P3 包，判功能则回落本表。
3. **录音回显推送链路在世性**（C6）：echo 生产者若缺，录音字幕半降级出包。
4. 既有 spec 登记功能修复项（60s 超时生产者、120s 录音自动收尾等）各自通道，不并入本 workstream。

## 5. 已对齐面（防过度报告）

键帽胶囊造型与基层三页/五态键帽文案；P2 网格语义/今天列/★※；P1 辅助条图标前置序与低电闪；
查字窗手绘大框与逐笔三态；盖章几何单源消费；覆盖层页点隐藏；聆听回显区可滚；
提醒键帽知道了/关闭；P1 猫常驻位大致吻合；倒计时贴纸双方皆无。

## 6. 挂起区（M4 落地后二次对齐，2026-09-24 human 复裁）

- P3 列表 / 阅读 / 录音 / 盖章 / 删除确认五屏全部差距条目（§2 对应各表）——M4-star-notebook
  （milestones.jsonl state=planning）将重构 P3 实现面，现对齐即对作废表面做功。
- 随 P3 挂起的键帽条条目：收尾/结束 · 念给我听/翻屏两组值域、录音态无页点（spec WF-F2-02）。
- 回进触发：M4 合并入 main；动作 = 主线程核 M4 新实现面 vs §2 P3 各表 → 开二次对齐批次挂本
  workstream changes；删除确认屏仍属功能债 §4，不随二次对齐自动回进。

## 7. 治理事故执行记录（260924；登记 = docs/crq/gov/ARQ-260924-split-order-granularity-gate-gap.md）

- 2026-09-24 主线程：ui-v5-align workstream 建立（task_plan/findings/progress 三件），当日即派
  portfolio-planner 拆包——**此时 milestones.jsonl 无本倡议登记、workstream 无 roadmap/phases、
  无 PROPOSAL.md**。
- 一轮派遣：planner 产 15 个 taskpackage（260924-v5-k1a..k5-*，含 plan.md/taskpackage.json/双
  input-brief）并回写 task_plan changes 列表。
- human 复裁①：P3 五屏挂起（M4-star-notebook 在开发 P3 本体）；复裁②：粗粒度（≤6 包）——
  15 包作废。
- 二轮派遣（6 包口径）：运行中被 human 中断，6 个包目录残留盘面（260924-v5-k1a-asset-ink-chrome /
  k2a-p1-home / k2b-char-window / k2c-voice-trio / k2d-reminder-open / n1b-font-family）。
- human 质问原话：「roadmap 都没产生 开始在那不断的创建 task package，是哪里逻辑定义的不对？」
  「别拆了，还是这么碎，以后必要吗，根据什么逻辑判断一定要这么做的」——顺序链缺陷显形：
  M1–M4 先例链 = milestones.jsonl 登记（nextAction 即 roadmap 摘要）→ workstream phases →
  taskpackages；该顺序无机械闸；planner intake fail-closed 仅在「输入为 PROPOSAL.md」时触发
  （portfolio-planner.md Step 1），非标准输入绕开；门 1（拆包 review 门，同文件 Step 5b）两轮
  均未跑到。
- 主线程补序动作（均在两轮建包之后）：milestones.jsonl M5 行（事故暴露后还原）、task_plan
  Roadmap 节（随清理删除）——证 roadmap 层可补但时序已错。
- human 终裁：「findings 留下，其他的都删掉」——21 个包目录清零、task_plan/progress 删除、
  M5 行还原；本文件为唯一会话制品。粒度定稿 = 三包（P1 / P2 / 演出覆盖页族）+ 三包合并后字体
  追加轮；**粒度判据三条**（human 三轮复裁钉死）= ① 包间文件所有权不相交 ② 门禁依赖包内同质
  （gated 项走包内轮次）③ 单包验证预算一 Pod 可消化；视觉对齐批不套 dev-loop 功能切片细粒度
  逻辑。载体 = 本节 + CRQ 条目 + 主线程长期记忆（task_plan 已删，此处为仓内唯一承载）。
