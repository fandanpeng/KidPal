# KidPal 功能点拆解与可行性盘点（V3 设计依据 + 验证包输入）

> 日期：2026-08-31（主线程盘点）
> 用途：① UI 元素逐点可行性分级，作为 **V3 设计稿**（实现导向）的输入；② OpenClaw 对接功能「已打通 / 待验证」清单，作为**功能支撑验证 TaskPackage** 的输入。
> 依据：设计稿 v2（`docs/ui/kidpal-ui-design-v2.html`）、实现策略（[ui-implementation-strategy.md](ui-implementation-strategy.md)）、踩坑记录（[ui-to-lvgl-pitfalls.md](ui-to-lvgl-pitfalls.md)）、spec §4/§7、契约（[interface-contract.md](../contracts/interface-contract.md)）、2026-08-31 对阿里云 ECS（8.137.91.16）的现场盘点。
> 分级口径：**✅ 已打通/已验证**（跑过、有实证）· **🟢 可行待实现**（路线已定、无技术悬念，只差做）· **🟡 需验证**（路线有方向但有未实证的风险点，必须先验）· **🔴 高风险/存疑**（可能走不通，需备选方案）。

> **更新 2026-09-01（TP-B）**：功能支撑验证批三包全部落地——TP-A（260831-cloud-link-bringup）打通 OpenClaw 云端链路，TP-C（260831-simulator-lvgl-align）把模拟器对齐固件 9.3.x，TP-B（260831-ui-art-assets）把 V2 美术元素全量出图 + 预算核算。本表 §1.2/§1.3/§1.5 相关行已翻 ✅；仍待后续 = OC-12..15 云端业务、I1 真机可用性、真机字体子集化、16px CJK 真机可读性。

---

## 一、UI 方面：设计元素逐点可行性

### 1.1 布局骨架与动态文本（LVGL 原生控件承担区）

| 功能点 | 设计稿锚点 | 分级 | 说明 |
|---|---|---|---|
| 4 页循环骨架（P1 首页/P2 课表/P3 故事/P4 查字）+ BOOT 翻页 | `canvas-p1..p4` | 🟢 | LVGL 正常发挥区；小智基座已有显示框架 |
| 绝对定位密排布局 | 各页 | 🟡 | 能做但**手抄坐标工时占一半**（实战 V5）——必须做「设计稿坐标→C 坐标」转写管线，否则 P2–P4 铺开成本失控 |
| 底部页点 `pgdots` + 键帽提示条 `comp-keybar`（永不消失） | `pgdots`/`comp-keybar` | 🟢 | 原生控件直接做 |
| 反白强调块/胶囊 `.inv`（"接下来"标签、REC 点） | `.inv` | 🟢 | 原生 fill 直接做 |
| 动态正文/标题文字（课表条目、故事正文滚动） | 各页 | 🟢 | 原生 label；真机走 `lv_font_conv` 子集 `.bin`（踩坑 §7.5 已定路线） |
| 列表选中态移动（P3 光标下移） | `canvas-p3` | 🟢 | 原生控件 |

### 1.2 大字体与排版（字体专项）

| 功能点 | 设计稿锚点 | 分级 | 说明 |
|---|---|---|---|
| 首页超大时间（约 110px，Baloo 2 ExtraBold 风） | `font-num` | ✅ | 已由 TP-B 出动态 imgfont：数字 0–9 + 冒号共 11 枚等宽箱体（48×64）、基线对齐，真体 FONTVERDICT REAL。真机仍须以 Baloo 2 ExtraBold 子集重烘（测试件替身出图已记录口径） |
| 冒号眨眼动画（4.4s 一眨） | `canvas-p1` | ✅ | 昨天已用原生定时器验证（`colon_blink_timer`） |
| 标题可爱字体：**站酷快乐体 ZCOOL KuaiLe** | `font-title`/`font-disp` | ✅ | 已由 TP-B 落档：SIL OFL 1.1 授权原文存 `tools/asset-pipeline/fonts/OFL-ZCOOLKuaiLe.txt`（免费商用/可内嵌/可子集化 + 改名禁则逐字引用）；标题样字 `font_zcool_30` 子集化栅格非空白（FONTVERDICT REAL，per-user 装字宿主依赖已记录） |
| 正文/拼音伴随可爱字体 | `font-body` | 🟢 | 走同一字体管线；拼音声调字符须确认子集覆盖 |
| **P4 米字格示范字 = 标准规范字体红线**（思源黑体/楷体，横平竖直、禁可爱体） | `canvas-p4` | 🟢 | spec §5.6 已立红线； makemeahanzi 字形本身即标准字形，D3 数据直用即可 |
| 小字号 CJK（9–16px）在 1bpp 可读性 | 各页 | 🟡 | 实战 V6：12–16px 细字阈值化后糊边。**16px 以下 CJK 真机可读性未实测**——直接决定设计稿能用多小的字 |

### 1.3 美术元素（资产管线承担区——昨天验证的主战场）

| 功能点 | 设计稿锚点 | 分级 | 说明 |
|---|---|---|---|
| 吉祥物「小伴」七姿态（Hello Kitty 风贝塞尔 SVG） | `pose-daily/listen/think/talk/record/sleep/jump` | ✅ | 已由 TP-B 全量栅格化 7 姿态 96×96（同族同尺寸防跳位，pose_daily 中立化 0°）；造型保真归 human 目测 OQ-3(a) |
| **手绘不规则圆角框族**（八值不对称 `border-radius`，`.frm/.frm-sm`） | `redlines` 框族 | ✅ | 已由 TP-B 出 3 框型（`.frm/.frm-sm/.frm-flat`）9 宫格切分（4 角+4 边+1 中心、角片 stretch=false），拼回原始尺寸逐像素 diff=0；255px 大半径角的 24px 角片忠实性留 human 目测裁定（review-packet finding #3） |
| **元素倾斜 ±1–2°**（`.tilt1/tilt2`，每屏 ≤3 处） | `.tilt*` | ✅ | 已由 TP-B 烘入：贴纸微倾（thermo_man -1°、drop_baby +2°）+ 印章 -8°（128→145 扩展、rotate 先于 flatten 真烘入）；-8° 锯齿单列结论归 human 目测 |
| 三档网点纹理 d50/d25/d12（4px 格点阵） | `.d50/.d25/.d12` | ✅ | 已由 TP-B 出 3 枚精确 4×4 平铺贴图，密度 d50(56%)>d25(25%)>d12(13%)、2×2 平铺周期性成立；密度观感归 human 目测 |
| 涂鸦点缀（星/云/波浪线） | `sec-icons` 涂鸦 | ✅ | 昨天已出 `doodle_star_26` / `doodle_cloud_52` 两枚 |
| 成就印章（双圈外实内虚、斜盖 -8°） | `comp-stamp`/`p3-stamp-frame` | ✅ | 已由 TP-B 出 `stamp_seal_r8` 145×145（外实内虚双圈 + 中心文字 + -8° 烘入），黑占比 16.4%；-8° 内圈 2.5px 虚线断口/糊连是目测重点（review-packet finding #5） |
| 图标家族（14 枚，FontAwesome 式或 imgfont） | `sec-icons` | ✅ | 已由 TP-B 全量出 14 枚（13 枚 48×48 + ic_wave 48×14 特例）；取舍落定 = 离线位图资产，FontAwesome 图标字留真机 Flash 优化备选（report 已记录） |

### 1.4 动画与状态演出

| 功能点 | 设计稿锚点 | 分级 | 说明 |
|---|---|---|---|
| 眨眼冒号 | `canvas-p1` | ✅ | 已验证（原生定时器） |
| 思考态飘问号 / 播报态声波条跳动 | `state-thinking`/`state-speaking` | 🟢 | 局部重绘原生动画，无技术悬念（未做但路线直通） |
| 笔顺逐笔动画（lv_canvas 折线逼近贝塞尔，每笔 950ms） | `canvas-p4` | 🟢 | **契约 D3 已定**、板端渲染机制已定（spec §3.0.2"确定性，无技术风险"）；未实现但契约兜底 |
| 米字格网格 + 笔顺三态（已写实心/当前笔白衬边托出/未写虚线点链） | `canvas-p4` | 🟡 | 虚线点链 dasharray 观感 vs D3 canvas 折线渲染**需视觉对齐核验**（策略 §6 留根） |
| 提醒弹窗整屏压 d50 网点 + pose-jump 跳入 | `state-reminder` | 🟡 | 依赖网点资产 + pose-jump 资产（两者都未出） |
| 全局状态演出（聆听/思考/播报/录音/提醒/低电 6 态换姿态） | `sec-states` | 🟡 | 依赖 7 姿态全量资产 |

### 1.5 资产管线与双端一致性（UI 能不能"真机也长这样"的地基）

| 功能点 | 分级 | 说明 |
|---|---|---|
| 设计稿 SVG → 位图资产脚本（`tools/asset-pipeline/gen_assets.mjs`） | ✅ | 昨天跑通，8 枚资产一次出齐；三大坑（flatten/use 链/字体替身）已修 |
| 资产格式：RGB565 直出 | ✅ | 昨天实证 9.5 下 I1 调色板被绕过会糊版，RGB565 双端一致 |
| **I1 格式在真机 9.3 + SPIFFS 文件源路径可用性** | 🟡 | RGB565 占 Flash 是 I1 的 16 倍（P1 一套已 96KB，大时间字块独占 55KB）。**I1 若能用可回收 16 倍空间**——9.3 bin-decoder `decode_indexed_line` 路径未在真机验证 |
| **模拟器 LVGL 9.5 vs 固件 9.3 版本对齐** | ✅ | 已由 260831-simulator-lvgl-align 落地：模拟器降到 9.3.x 与固件同版（`lv_version.h` 三号一致 + MSBuild 编译通过），「9.5 过 ≠ 9.3 无坑」的错位已消除 |
| **1bpp 阈值化快照自查**（模拟器真彩 → 真机黑白观感校准） | ✅ | 已正式化（260831-simulator-lvgl-align SLA-A4）：`snapshot_timer` 收编为 `kidpal_snapshot` 工具，验收图一律以阈值化 PNG 为准 |
| 资产总量 vs SPIFFS 分区预算 | ✅ | 已由 TP-B 核算：47 资产 RGB565 总量 599,698 字节 vs SPIFFS assets 分区 8M，margin 54.1%，**不触发 I1**；P4 笔顺字盘（保守整帧 ~3.1MB）是唯一逼近预算项，V3 前须锁笔形 3 态复用打包策略 |
| 实验代码正式化（`kidpal_ui.c`/`gen_assets.mjs` 由 dev-loop 接管重写） | ✅ | 已由 260831-simulator-lvgl-align 接管正式化（SLA-A6：kidpal_ui.c / gen_assets.mjs / vcxproj 资产挂接），实验代码不再沉淀为事实标准 |

### 1.6 UI 可行性小结（给 V3 设计的三句话）

1. **TP-B 已把 V2 设计稿的全部美术元素经资产管线出图并机判转绿**——吉祥物 7 姿态、手绘框族 9 宫格、三档网点、全图标族、印章 -8°、动态时间 imgfont、站酷快乐体授权，**不再有"方案已定但未出图"的项**；造型/锯齿/接缝等观感保真归 human 对阈值化 PNG 目测 OQ-3(a)。
2. **已验元素里没有 🔴 级（走不通）项，也没有"做不到"项**——资产化管线全数覆盖；剩余 🟡 是「I1 空间回收（真机）」和「16px 以下 CJK 真机可读性」两条，仍待真机联调期验证。
3. **V3 设计的核心约束不变但更实**：① 倾斜/手绘框/网点全部烘进资产（已实证）；② 框族 255px 大半径角的 9 宫格角片忠实性、-8° 印章内圈虚线观感，需 human 目测裁定后锁 V3 框/印章规格；③ 预算已实（599,698 字节 vs 8M 不超），但 P4 笔顺字盘 ~3.1MB 须先锁打包策略再定 V3 字盘形态。

---

## 二、与 OpenClaw 对接：已打通 vs 待验证

> 现场盘点时间 2026-08-31（经 SSH 查 8.137.91.16）。**重要修正**：spec 里写"已实测跑通"的只有 D3 数据格式一项；**shim、whisper、edge-tts、OpenAI 端点目前全都还没就位**（详见下表）。

### 2.1 今天已能打通（✅ 有实证）

| # | 功能 | 实证 |
|---|---|---|
| ✅ OC-1 | **OpenClaw 网关在跑** | `openclaw-gateway.service` active，公网 `ws://8.137.91.16:18789` 已通，HTTP 根路径返回 200 |
| ✅ OC-2 | **D3 笔顺数据格式** | makemeahanzi `dictionary.txt` + `graphics.txt` 已下到 `/root/.openclaw/workspace/kidpal/chardata/`；spec §3.0.2 标"已实测跑通"（"勇"字约 2KB JSON） |
| ✅ OC-3 | **定时能力（cron）** | `openclaw cron list` 在场且能跑（heartbeat、memory-dreaming、skill-review 等 4 个 job 在册，还有一个 95 天前的 `class-reminder` 历史 job）——**B3 提醒写文件的调度底座可用** |
| ✅ OC-4 | **DeepSeek 意图理解/多轮对话** | OpenClaw agent 自带能力（spec 决策 11）；agent `main` 在册 |

### 2.2 验证结果（TP-A 260831-cloud-link-bringup 已验 OC-5..11 标 ✅；OC-12..15 仍待验证 🟡）

| # | 功能 | 现状（2026-08-31 实测） | 要验什么 |
|---|---|---|---|
| ✅ OC-5 | **OpenAI 兼容端点 `POST /v1/chat/completions`** | 已开通（TP-A）：`gateway.http.endpoints.chatCompletions.enabled=true`，端点从 404 变为正常 completion、可挂 system prompt | ✅ 已验（正常返回） |
| ✅ OC-6 | **DeepSeek 按格式输出 ` ```display ` 块**（契约 E1 命脉） | 已实测（TP-A）：挂契约 §1.6 system prompt 连发 12 条，` ```display ` 块出现率 + JSON 可解析率 12/12（occ 1.0 / parse 1.0），char/schedule/story_list/story_text/text 全覆盖 | ✅ 12/12（E1 双通道方案成立、无需换切分） |
| ✅ OC-7 | **shim 薄桥（FastAPI `/v1/asr /v1/chat /v1/tts /v1/alerts`）** | 已建（TP-A）：`server/shim.py` 起 `/v1/chat` 一路（文字→OpenClaw→切分正文/display→正文送 TTS→回 `{audio, display}`）跑通；asr/tts 独立口 + alerts 留 stub | ✅ 全路径跑通 |
| ✅ OC-8 | **ASR faster-whisper** | 已装（TP-A）：另建 venv（Py3.11）装 faster-whisper，tiny 档中文短句转写正确，延迟约 0.85–1.92s | ✅ 已验（tiny 档可用） |
| ✅ OC-9 | **TTS edge-tts** | 已装（TP-A）：edge-tts 出童声 mp3（晓晓音色）可播放 | ✅ 已验（童声 mp3 可播） |
| ✅ OC-10 | **ECS 算力**（whisper+DeepSeek+edge-tts+网关同机） | 已核（TP-A）：ECS 2 核 / 1870MB，whisper+DeepSeek+edge-tts+网关同机只扛得住 whisper **tiny** 档（base/small 内存不够） | ✅ 已核（只能 tiny 档） |
| ✅ OC-11 | **OpenClaw 读/写 schedule.md**（B1 课表结构消费） | 已实测（TP-A）：DeepSeek 按知识库 `schedule.md` 正确筛选「今天下午」课程，答复正确并吐 `view:schedule` | ✅ 已验（B1 课表消费正确） |
| 🟡 OC-12 | **cron 到点写 `alerts/pending.json` → shim 读出**（B3 提醒链路） | cron 底座在（OC-3），但"扫课表→写提醒文件"的 job 未配 | 端到端：cron 触发→写文件→shim `/v1/alerts` 出队 |
| 🟡 OC-13 | **故事归档 stories/*.md + 起标题/润色**（C1/C2） | 未实测 | DeepSeek 写文件 + 自动起标题 |
| 🟡 OC-14 | **故事列表/回放经 display 下发**（C3/C4） | 未实测 | `story_list`/`story_text` view 下发（依赖 OC-6） |
| 🟡 OC-15 | **家长经 QQ 录课表**（B4） | 未配置 | QQ 通道接入 + 解析改写 schedule.md |

### 2.3 对接小结（三句话）

1. **"已打通"今天只有 4 条**：网关在跑、D3 字库在位、cron 底座可用、DeepSeek 对话自带。**shim / ASR / TTS / OpenAI 端点这四块 MVP 必经之路上一块都还没有。**
2. **最大的单点命门是 OC-6**（DeepSeek 能不能稳定吐 ` ```display ` 块）——它不通，E1 整个"语音+屏幕双路"设计就要换切分方案。应排进验证包第一批。
3. **次风险是 OC-8 的 Python 3.6.8**——faster-whisper 要求 ≥3.8，装不上就要先解决 Python 环境，这可能比装 whisper 本身更费时。

---

## 三、给验证 TaskPackage 的建议优先级

> 验证包目标 = 把上表所有 🟡 项各落一条"可机判/可目测"的验收，跑完出"可行/不可行+备选"结论。**先云端后 UI**（云端是 MVP 链路主线，UI 是 M4）。

**P0（不通则链路断，最先验）**：OC-5 → OC-6 → OC-8/OC-9（ASR/TTS 装上并出声出字）→ OC-7（最小 shim 串起 `/v1/chat` 一路）。
**P1（业务功能命脉）**：OC-11（课表 MD 消费）→ OC-12（cron 提醒）→ OC-13（故事归档）；UI 侧 1.5 的「模拟器 9.3 对齐」+「I1 真机验证」+「资产总量预算核算」。
**P2（V3 设计输入）**：1.3 的「7 姿态全量出图」「手绘框族 9 宫格资产」「网点平铺」→ 1.2 的「站酷快乐体授权+栅格」「Baloo 2 imgfont 动态时间」→「16px CJK 1bpp 可读性」。
