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

### P1 首页（canvas-p1）/ P2 课表（canvas-p2）

> **2026-09-26 收官删除**：human 真机看屏裁定「首页、课表也都 OK」，两页差距条目按此从本文件
> 删除（P1 九行 / P2 六行，被删原文见 commit `71ccc97` 本文件 §2 同位置）；残留未闭项不随之
> 消失、另在册，见 §10。

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

### 语音三态（state-listening / state-thinking / state-speaking）

> **2026-09-26 收官删除**：human 真机看屏裁定三态「也 OK 了」（含当轮修掉的播报态声波条
> 上下颠倒），本节八行差距条目按此从本文件删除（被删原文见 commit `71ccc97` 本文件 §2
> 同位置）；残留未闭项不随之消失、另在册，见 §10。

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

## 8. 批后簿记（2026-09-26，五阶段直改批收尾）

> human 指定不走 taskpackage 的主线程直改批（跳过 dev-loop 一次性例外，逐阶段
> architect 评审 + commit），2026-09-25/26 执行：阶段1 全局页点空心环+P2 表格、
> 阶段2 P1 首页、阶段3 语音三态、阶段4 提醒、阶段5 查字窗。host runner 每阶段
> 103 Tests 0 Failures，设备树全量构建每阶段通过。行号引用为批前 b35d3ed 实况
> （各阶段 commit：0ded990 / 70ade0b / 9f4c82a / b50cf5a）。

### 8.1 资产消费回写（§3 表更新）

- **转已消费**（本批接线）：bub_listening/thinking/speaking（语音三态全气泡）、
  arc_listen、qmark_trio、bell_trio（三态+提醒弧位 56×56 专属件）、doodle_cloud_52
  （P1 卡顶 + 语音态右上）、rec_wave（P1 左下 + 语音态左下）、
  p1_chip_jiexialai/mingzao（P1 黑帽标，p1_label_* 转零消费退役）、ic_wifi_14
  （P1 辅助条打头）、ic_star_22（查字窗右栏★）。
- **仍零消费**：bub_recording/bub_rec_guide、pose_record_96、voice_chip_p3、
  stamp_seal_r8、frm_flat_240_96/frm_sm_160_120、ic_star_18/20/38、
  imgfont_letter_*/colon（随 P3 挂起区 / 字体追加轮 / 删除确认功能债）。
- **转零消费（退役）**：p1_label_jiexialai/mingzao.bin（白底黑字标签，被 p1_chip_*
  替换）——收尾时确认无其他消费面后可入退役清单。

### 8.2 遗留登记（后续轮裁量面，本批不扩 scope）

1. **bub_remind「记得哦」烘焙资产**：程序化气泡无尾巴无 3px 描边，资产化归后续
   资产轮（「跟着写」黑帽标同列候选）。
2. **提醒多事项聚合**：母版右区两条提醒同框；设备侧 RM-A4 冻结队列逐条弹，聚合
   需改队列弹出语义（功能裁量，architect 评审注记 3）。
3. **hp1_state_t 增星期/联网字段**：日期「Tue 22 Sep」星期与 WiFi 条件显隐均需
   契约面增字段（findings §2 P1 行 2/行 1 半），本批契约面不碰。
4. **明天帽标字样**：p1_chip_* 闭集只有「明早」，「明天」字样资产缺（§2 P1 行 6
   半、[冲突 C5] 遗留）。
5. **字体追加轮**：全局 font_puhui_14_1 → 圆体手写感 + 13px 同字号（§1.1 [冲突
   C4]）+ 查字窗拼音带调大字（imgfont_letter_* 无带调元音字形缺口）。
6. **像素级观感清单（真机轮）**：页点环 8/12 vs 母版 9/11、波浪密度/灰两档、
   chip 胶囊化 (0,4) 骑框 -4° 旋转、mic 回退态相对位、网格线两端缩进、云朵 52
   vs 母版 40px 等——architect 各阶段评审注记汇总，真机看板时一并裁量。

## 9. 真机像素对齐轮（2026-09-26，P1 拍图 ↔ canvas-p1）

**触发**：human 拿真机拍图对照 `docs/ui/V5/canvas-p1.png` 逐项验收，指出多处不符；
主线程核对代码 + 像素级实测（设计图 400x300 与屏 1:1，PIL 逐行/逐列扫描）确认拍图与
当时 HEAD 渲染**完全一致**——差别全部是设计↔实现的真差距，非固件版本或数据 bug。
拍图里「明天」组整体缺席经 human 确认是**数据问题**（服务端未下发 preview），
preview 逻辑本轮零改动。

**本轮落地的差别处置**（逐项，授权 = human 看屏裁定 + 直改批模式）：

| # | 差别 | 处置 |
|---|------|------|
| a | 左下波浪呈折线 | **用错资产**：实现消费 `rec_wave`（录音态声波折线，SVG 全直线段），设计要的是 `ic_wave_58`（`ic-wave` symbol 平滑正弦波，画布目标尺寸直出件）。改消费 `ic_wave_58` @ (18,231)，墨迹 (18,233)-(75,247) 与设计逐像素吻合；`rec_wave` 保留给语音态声波（本意） |
| b | 上虚线过低、下实线过高 | 顶虚线 y42→y33（设计 y25-26 在状态栏墨底 6px 下，实现辅助条墨底 ~28 等比贴置；旧 VQ-A7 r5「y42 定谳」随本批人读推翻）；底实线 = `bottom_bar_` 上沿边框 252→262（bar 高 48→38——canvas-p1/char-window/canvas-p2 三页设计实测底线均 y262-264） |
| 2 | 四角星 sparkle 缺失 | `ic_sparkle_22` 转已消费（c738879 只落了资产）：放大 28x28 @ (118,208)，墨 (124,214)-(138,228) ≈ 设计星墨 |
| 3 | 今日条目起始偏下 | ROW1 y96→62（字顶 68 = 设计实测；行距 28 与设计一致，未动） |
| 4 | 云朵+帽标偏右偏大 | 云朵 `doodle_cloud_52`→`ic_cloud_40_t2` 放大 46x46 @ (103,29)（墨 108,36-147,64 ≈ 设计 107,36-147,62）；帽标 (192,60)→(151,41)（墨 152,42-208,61 ≈ 设计 x152-209/y42-60）；内容列 x0 136→152 |
| 5 | 猫偏上 | `pose_daily_96` (24,80)→(28,116)——设计猫墨 x36-109/y127-209，反解盒原点后**墨迹逐像素吻合**（大小 96 未动，human 更正） |
| 6 | 日期无星期 | `hp1_state_t` 增 `wday` 字段（周一=0..周日=6，-1=未注入；set_state 门 -1..6）；数据源复用既有 `kidpal_render_node_inject_weekday` 缝（板级已接线，非契约面新增——原 §8.2③ 的「契约面」判断经本轮核实为误判，星期由本地 RTC 注入即可）；`p1_draw_aux_date` 渲染 "Www DD Mon"，无星期退回 "DD Mon" 不画假星期 |
| 7 | 温度无 °C | 增 `imgfont_degree.bin`（`gen_imgfont_letters.py` 与 `%` 同缝生成，48x64 cell 同基线；`art_assets.c` `kidpal_art_glyph` 按 `(unsigned char)0xB0` 分流）；`p1_draw_aux_temp` 尾随 ° + 复用既有 `imgfont_letter_C` |

**版式连带重排**：辅助条三元素右移（温度 210 / 湿度 282 / 电量 342，数值半含 °C）；
预告区重构为设计结构 = **帽标独立行 + 项行在下**（原实现帽标与项同行；现 MINGZAO
@151/fy+33、项行 fy+60，组间虚线 fy+22，均按 canvas-p1 明天区实测）；卡区 clip
{136,48,392,242}→{103,36,392,242}。

**验收面**：HPB（p1_home_battery.acceptance.c）与 PCN（p1_cjk_name.acceptance.c）
两文件钉区常量整批重钉 + 尾部 REVISION LOG 追记；`pcn_rect_card` y0 首钉 29 实跑
即红两齿（辅助条电量墨 x393-396/y≤31 混入右界带），纠正为 36（云墨顶）。辅助条钉区
底缘 33→31，避免缺席/灭相零墨齿被重钉后的虚线墨误触发。

**验证**：host runner 103 Tests / 0 Failures / OK（复跑两次一致）；设备树
`idf.py build` 通过（xiaozhi.bin 0x30f2e0，余 11%）；host 截图 harness 挂真实
spiffs_root 出图，逐元素墨盒实测：猫 (36,127)-(109,209) 与设计**逐像素一致**，
云/帽标/星/波浪/顶虚线均在 1px 内吻合（行流文字因该 harness 本就不含 14px 字体缝
而零墨，行位由 PCN 像素 diff 齿绿证）。

**本轮未做（登记）**：① 明天组多项化（设计两条 + 「临时」标记）——单槽 preview
是契约/数据面（§8.2④ 同源）；② 「明天」字样资产缺（现闭集仍「明早」）；
③ §8.2⑥ 像素级观感清单余项（页点环、chip 旋转、wave 灰两档等）真机看板时续裁。

**架构复核（2026-09-26 architect 实例，直改批模式逐批评审约定）**：结论 = P1 本体 GREEN
（结构忠实度/不变式/wday 装配点/契约面四项全部通过），**单一跨页打回项** = 本批
`bottom_bar_` 高 48→38 是全局 chrome 改动，P2 课表网格底缘 `KSG_GRID_BOTTOM=251` 未跟随
（网格悬浮于新底条顶 262 之上 10px，本批把它从「贴旧错位 252」改成「与新位置之间有缝」）。
**处置**：精测 canvas-p2 后按设计位重钉 251→254（设计实测网格底线 y254、底条上沿
y262-264，两者之间有 7px 固有间隙——不是硬贴底条）；连带 `ksg_slot_ys` slot2 下缘
251→254、验收 A6 零墨带口径 y>=252→y>=255、头注同步。

**另两条 note 的处置**：① wday 零值 = 周一 的静默 footgun——两处夹具（PCN/HPB）改为
显式赋值（PCN 钉周二 1 与设计 "Tue 22 Sep" 同款；HPB 钉 -1 未知），并新增一条注入门齿
（`wday=-1` 合法、`wday=7` 必拒）落 HPB TEST 2；② `char_page.cc` 提示条让位注释与
`char_window_overlay.acceptance.c` 报错文案里的旧底条几何（高 48 / y252-300）同步更新
（断言口径保持保守下界 252 不收紧，实测取值 244 两口径皆过）。

**终态实跑**：host runner 103 Tests / 0 Failures / OK（含新增星期门齿，共跑三轮一致）；
设备树 `idf.py build` 通过（xiaozhi.bin 0x30f2e0）。**烧录面提醒**：本轮新增
`imgfont_degree.bin` 属 SPIFFS 资产 → 真机核验须 app + art 双分区烧（只烧 ota_0 新资产
不生效，flash-asset-partition-rule 教训）。

**合并簿记（2026-09-26）**：本批 commit `6ed6b67` 已 ff 合入本地 main（main tip
`c738879 → 6ed6b67`，单 commit、无双侧撞件、`git diff --check` 无残留）。前置升级闸
拦两因（taskpackage 级 riskTier=high 与 supervisor verdict=ESCALATE，均为 2026-09-15
整包记录、非本批新因）→ human 会话内拍板覆盖（原话「merge to main」），人批事件
`5b17ca6a-ccc5-46d4-8e40-7eebd6b73cb6`（approve-escalated-merge）落
`delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/pod-events.jsonl`。
merge-driver 未接管（板面该包已是 merged 态，本轮属 workstream 后续批次，先例
阶段 2-5 同为直合）。**remote push 仍留 human 走 PR**。

**第二/三批次（2026-09-26，human 看屏两轮追加）**：真机看屏后 human 指出三事——
①云朵与「接下来」位置偏高、②「接下来」偏小、③四角星要「一闪一闪」。调查与处置：

- **帽标偏小（成立）**：设计 pill 58×24 / 白字 37×13；旧资产 58×21 / 白字 32×11（字号
  等效小 ~15%）→ `gen_assets.mjs` chip() 重烘（h 21→24、font-size 12.5→14.5），
  jiexialai 与 mingzao 两枚同批；现白字 38×12。
- **云朵偏高（成立）**：母版 HTML 明写 `width:40px;left:104px;top:36px`，设计云墨
  (107,42)-(143,67)；旧实现 46px @ (103,29)、墨 (108,36)-(147,64) —— 大 6px 且高 6px。
  改按母版 40px 直出。
- **组内相对关系曾被反转**：设计帽标顶（36）比云朵顶（42）高 6px；旧实现里云朵反在
  帽标上方。已纠正。
- **相对上虚线仍偏高（第三轮裁定，成立）**：用「空行数」量呼吸——设计上虚线底 → 帽标顶
  **9 行空**、线底 → 云朵顶 **15 行空**，旧实现只有 3 / 9 行。处置 = **整卡下移 6px**
  （云朵 (104,42)、帽标 (151,41)、行流 fy 62→68、组间虚线 140→146、明天组随移），
  组内关系不动；现实现 9 / 15 行，与设计同值。
- **四角星闪烁（新增功能）**：1s tick 粒度两相位尺寸脉动（大 28px ↔ 小 20px，周期 2s，
  绕同心 (132,222) 胀缩不位移），驱动源 = 首页每秒重渲的动画路径（与冒号眨眼同源）；
  host 双 tick 实测墨 15×15 / 11×11 同圆心。
- **根因登记（结构性，本轮未动）**：实现上虚线只能钉在 y30-32——辅助条图标是 **20px
  盒**，母版约 **13px**，状态栏整体比设计高 ~9px，直接吃掉线上下两侧的气口。故整卡虽然
  与线的**呼吸同值**，绝对 y 仍比设计低 6px。要让虚线与卡同时回到设计的绝对高度，须
  缩辅助条图标（M1-N1b 已标定面），属独立裁量项，真机看板时一并议。
- **验证**：host runner 103 Tests / 0 Failures（两批各跑一次）；设备树构建通过并已烧录
  真机核验两次（boot 0 RE-ENUM、render_rc=0）。验收钉区常量与文档坐标随版式同步
  （辅助条底缘 29、卡区整体 +6）；harness 的 `wday` 零值陷阱一并修掉。

**合并簿记（第二/三批次，2026-09-26）**：commit `efa8513` 经 `46af830`（簿记）ff 合入
本地 main（`fcb77d0 → 46af830`，单链 ff、`git diff --check` 无残留）。前置升级闸仍拦
同两因（taskpackage 级 riskTier=high / supervisor ESCALATE，9-15 旧记录）→ human 会话内
再次拍板覆盖（原话「ok了，commit merge吧」），人批事件
`99851e91-4404-44f0-8ac3-03dd67acfff7`（approve-escalated-merge）落 pod-events.jsonl。
**remote push 仍留 human 走 PR**。

## 10. findings 收官删除（2026-09-26）

### 10.1 首页 / 课表

- **触发**：human 真机看屏裁定「首页、课表也都 OK」——§2 的 P1 首页（九行）与 P2 课表（六行）
  差距条目按此裁定从本文件删除，两页不再作为本 workstream 的待办面。
- **可回溯**：被删原文位于 commit `71ccc97` 的 `findings.md` §2 同位置；需要查证历史差距时
  从此处取回。
- **残留未闭项（不随本次删除消失，各自在册）**：明天组多项化 + 「明天」字样资产缺 =
  §8.2④ + §9 未做①；字体追加轮（圆体 + 13px 同字号 + 查字窗拼音带调大字）= §8.2⑤；
  P1/P2 像素级观感余项（页点环、chip 旋转、wave 灰两档、网格线缩进等）= §8.2⑥ + §9 未做③。
  连同 P3 挂起区（§6）与功能债（§4），均为本 workstream 仍在册的开口。

### 10.2 语音三态（聆听 / 思考 / 播报）

- **触发**：human 真机看屏裁定「listen / thinking / speak 也 OK 了」——§2 语音三态表（八行）
  差距条目按此删除。此前当轮另修掉播报态声波条上下颠倒一项（见 10.3）。
- **可回溯**：被删原文同在 commit `71ccc97` 的 §2 同位置。
- **残留未闭项**：这三页无专属开口；受全局项复用——字体追加轮（三态文本区圆体 + 13px/700/1.7，
  §8.2⑤）与 bub_remind「记得哦」资产化（§8.2①，提醒态共用同一语音骨架）仍在册。

### 10.3 播报态声波条上下颠倒修正（2026-09-26，直改批）

- **报告 → 根因**：human 真机指出播报态「说话的动态图」上下颠倒。根因 = 声波条旧实现把每条
  **顶边**钉死在弧位盒内 y12、动画只改 `height`，条子自顶向下垂；母版 `.eq` 是
  `align-items:flex-end` 的**底对齐**条组，五条共底缘、只向上长，方向恰好相反。
- **设计实测**（`state-speaking.png` 逐行扫描）：五条宽 6、节距 11（x136/147/158/169/180）、
  **底缘齐 y127**、高 12/20/14/24/10——与母版 HTML inline 值逐一吻合。
- **处置**（`lcd_display.cc` 声波条动画回调 + 建对象段）：① 翻转 = 底缘恒定、高变时同步 set_y；
  ② 几何随实测对齐 = 条宽 4→6、节距 9→11、起点 x 8→4（abs x136 起）、圆角 1→3（母版 6px 宽配
  `radius:3px` 胶囊端）；③ 幅度口径 = 母版 `@keyframes eqBar` 的 `scaleY(.35..1)` 逐条按自身
  基准高缩放（旧为统一 8..30，最高会顶出设计包围盒）。相位错开与 900ms 周期未动（r4 定值）。
  逐条验算：底缘 abs y127、x 落 136+i·11、h=24 那条占 y104-127，与锚图逐项吻合。
- **验证**：host runner 103 Tests / 0 Failures（与基线同值，`lv_anim` needle 未破）；设备树
  `idf.py build` 通过（xiaozhi.bin 0x30f350）；**已烧 app 分区**（0x20000，本轮不动资产故
  不需 art 双烧）；boot 日志 `render_rc=0` 无 panic，报出的 ELF SHA256 与本地
  `build/xiaozhi.elf` 逐字一致 = 板面确为新镜像。

### 10.4 口径说明

- 本文件惯例为「追加新节 + 就地订正」，不删既有行；10.1/10.2 两次删除是 human 明确指示的
  唯一例外，特此留痕，其余章节维持原惯例。
