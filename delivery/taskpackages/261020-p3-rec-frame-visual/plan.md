---
changeId: 261020-p3-rec-frame-visual
rigor: nano
riskTier: low
sourceType: human-ad-hoc
reviewed-by: architect
date: 2026-10-20
---

# P3 录音帧两处观感修（波浪线挪猫下方 + REC/时长同行右移）

> 落盘说明（主线程，触点①后）：本文 = architect reviewed plan（意图定义五件）原样搬运；「human 触点①决议」节为主线程追加的裁决留痕，plan 正文未改。

## 意图定义五件（nano reviewed plan）

1. **改动意图一句**：讲故事录音页（KSTORY_PAGE_RECORDING，帧 A/B 共用块）两处观感修——① 声波波浪线从右侧中部 @(180,94) 挪到猫咪图标 @(45,87,96x96) 正下方（左演出区内，避开右侧回显列 x≥180）；② REC 闪点+「REC」+录音时长合并为一行并整体右移，消除与左侧对话气泡（@(24,28)，「需要我帮你记点什么？」约 168px 宽、右缘 ~192）的视觉重叠。

2. **目标符号/文件锚（changeManifest 种子）**：`firmware/main/display/story_page.c` —— `kidpal_story_page_render` 内 case KSTORY_PAGE_RECORDING 分支（:1434–1501）的位式常量与绘制坐标 + 分支头注释（:1444–1454 位式注释同步改写）。ksy_bubble / ksy_art_image / kstory_art_dsc / kidpal_story_rec_time 签名与本体不动；程序化兜底跳动条（:1469–1472）随资产分支同挪。

3. **预期行为差 before/after**：before = 波浪线与猫咪并排（右区中部），REC/时长分两行且与气泡区重叠；after = 波浪线在猫咪正下方左演出区内，REC 闪点+「REC」+时长同行整体右移、与气泡零重叠；元素集与「在记」语义不变。具体坐标 fixer 在左区自由度内定（206px 资产宽 vs 左区 ~170px 宽存在张力，允许调整兜底条间距/起点，真机目检为准）。

4. **既有齿覆盖态申报**：目标区域（RECORDING 帧位式坐标）**无齿**——实测 story_page_v2.test.c / story_page_v2.acceptance.c / record_dialogue_guide.acceptance.c / record_dialogue_v7_fix.test.c 只断言行为语义（页态/键位/子态/资产名在库/文案单源），零坐标断言。**无齿 → human 目检兜底**（human 截图驱动、观感直修+烧板+看屏偏好，兜底意愿已含在请求内）；白盒复核（Step 10c）为 nano 行为验收判分位。

5. **爆炸半径 trace_path 摘要**：本会话无 MCP 图工具，以精确调用点 grep 替代 `trace_path`（如实声明）——将触符号 `kidpal_story_page_render` 实现面 inbound 全在一个 owning 文件 `mcp_server.cc` 内 = 3 个命名函数（ScheduleStoryRender :108 / StoryRecTimerCb :142 / ScheduleStoryDeleteOverlay :518）+ AddCommonTools 内 3 个 story 工具 handler lambda（:1557/:1642/:1697），计 **6 个调用点 / 4 个调用作用域**；host 测试侧 story_page_v2.acceptance.c 3 处 + archive 退役面（不计）。按调用作用域 4 ≤ 5 达标、按调用点 6 略超阈值——**边界量数已申报，human 触点①一并确认**。该函数非中枢调度符号（对照先例：中枢 17），diff 预估 1 个实现面文件 ≤ 2。

## 定位结论（architect 审阅附注）

- 现状位式与 v7 设计稿逐像素一致：猫咪 pose-record 96x96 @(45,87)、气泡 @(24,28)、REC 闪点 12px @(180,18) +「REC」@(198,17)、时长 @(180,38)、rec_wave 206x34 @(180,94)、回显 @(180,184)。
- 已核实**无第二画法源**：lcd_display.cc 的 rec_wave 消费是语音 chrome 的 40x12 降采样涂鸦（另一功能），home_p1.c 仅为历史注释；REC/时长/声波的录音帧绘制全在该 case 分支。
- REC/时长/声波块画在 `rec_substate==1` 判定**之前**，帧 A（录音中）与帧 B（引导播报中）共用——一次编辑两帧同生效（与 v7 规格表「在记信号三帧常驻」语义一致）；帧 C（整理中 ic_wave_40 @(48,204)）不在本次范围。

## 裁决 rationale（architect）

**⓪ 档 0 nano ｜ riskTier: low ｜ sourceType: human-ad-hoc**（human 截图直提，非 proposal、非 regression）。

nano 判据逐条核：

1. **改动独立**：单一 UI 页面元素调整簇（同一帧内两处位式），意图一句话说清。满足。
2. **爆炸半径**：见五件第 5 条（调用作用域 4 ≤ 5 达标、调用点 6 略超，边界申报在案；单 owning 文件紧耦合簇）。满足（含边界申报）。
3. **无验收语义净增**：纯位式微调，元素集/语义零变化（REC 闪点+大时长+声波+回显四件全保留，WF-F2-02 / v7「在记信号三帧常驻」语义不动），不引入新行为决策，不触契约语义；sourceType 非 proposal。满足。
4. **排除清单零命中**：不触 wire/契约面（display 统一响应块是 server→device wire schema，本改是设备端本地像素布局——rules/contracts.md 四分类表明列「设备端本地渲染细节」归 NO_CONTRACT_CHANGE）；不触状态机（页态/子态/键位逻辑零触）；不跨端（server 零触）；不触安全轴；无迁移删数；意图不需新行为验收语义承载。满足。

**host 齿可闭环判注**：nano 回归网 = 既有 host 套件（story_page_v2 / record_dialogue_guide / record_dialogue_v7_fix 等 27 件同 runner 单 ELF），可在 host_app target 闭环，保证不出界破坏行为语义；视觉裁决本身归 human 真机目检（见五件④）。

**逃生门自查（reviewer-first）**：不触契约（四分类 NO_CONTRACT_CHANGE）；需求面语义（S-F2.2 AC1 元素集、AC2 回显常驻/键位）不变，位式归属设计面细节；不派 architect-reviewer。透明申报：ask①（波浪线挪到猫下方）偏离 WF-F2-02 线框画法与 v7 帧现位式（ask② REC+时长同行反而更贴线框「● REC 02:35」一行画法）；按 human 观感直修偏好（闸链后补账先例）判为实现面观感修，human 触点①已确认此判。

## human 触点①决议（2026-10-20，主线程 AskUserQuestion 留痕）

1. **路由**：档 0 nano（rigor=nano / riskTier=low / sourceType=human-ad-hoc）——human 确认（含爆炸半径边界量数申报与无齿目检兜底声明）。
2. 两处观感修均纳入本包 builder 实施范围，坐标由 fixer 在左演出区自由度内定、真机目检为准。

## 追加修订（2026-10-21，NRF-X4 真机目检中的 human 追加指令，主线程留痕）

真机看屏：原两处到位（波浪线在猫下 ✓ / REC 行与气泡零重叠 ✓）；human 追加第三处——**回显文字列仍偏下**（v7 位式 @(180,184)），上半右栏被 REC 行与声波腾出的空间浪费，且底部回显行被键位条裁切。裁定：并入本包（同文件同分支同意图族、changeManifest 未越界），回显列上移消费腾空空间；builder 定具体坐标（右栏 x≥180，顶起于 REC 行下方、底缘留键位条安全距），真机目检为准。

## 后补账提示（不阻塞本包）

落地后 v7 设计稿 `docs/ui/kidpal-ui-design-v7.html`（:945–979 p3-recording / p3-recording-guide 两帧的 REC/时长/声波坐标）与 WF-F2-02 线框将与实现漂移——按观感直修先例在真机观感终判后回同步设计稿/线框注记；本包顺带构成上一包遗留「帧B观感终判」的部分兑现。

## 关键取证文件

- c:\AI\KidPal-pod-m2\firmware\main\display\story_page.c（:1191 kidpal_story_page_render；:1434–1501 RECORDING 分支；:1444–1454 位式注释；:1469–1472 程序化兜底跳动条）
- c:\AI\KidPal-pod-m2\firmware\main\mcp_server.cc（:108 ScheduleStoryRender / :142 StoryRecTimerCb / :518 ScheduleStoryDeleteOverlay / :1557/:1642/:1697 story 工具 handler lambda——爆炸半径申报的调用点全集）
- docs/ui/kidpal-ui-design-v7.html（:945–979 两帧位式，后补账回同步对象）
