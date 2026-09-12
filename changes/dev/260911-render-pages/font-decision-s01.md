---
changeId: 260911-render-pages
date: 2026-09-12
decision-id: human-20260912-S0.1-font-scheme
touched-by: human（VS Code 交互会话 AskUserQuestion 两轮，planner 实测输入后拍板）
status: DECIDED
---

# S0.1 V3 字体方案决策记录（human 触点②）

## 实测输入（planner intake，2026-09-12）

| 候选 | 覆盖实测 | 结论 |
|------|----------|------|
| 板级内置（CMakeLists BUILTIN_TEXT_FONT=font_puhui_basic_30_4，实际正文行为与 font_puhui_30_4 吻合：刺/小可见、猬缺失） | 最大 6807 字形，缺「猬」 | 不达标 |
| cbin 家族 font_puhui_common_{14_1,16_4,20_4,30_4}.bin（272KB 级，lv_font_load） | 四款共享同一字表 ≈6813 字形，**「猬」「猾」「狸」全缺**（犭旁动物字整族漏收） | 不达标 |

实测方法：lv_font_fmt_txt 二进制/cmap 结构离线解析（cbin = 序列化 C 结构，cmap 条目 20 字节自然对齐步长；内置 C 源按 cmap 块 + run-length unicode_list 解码），抽样集含课表课程名/地点高频字与绘本生字。「刺」在内置/cbin 均有、「猬」全缺，与真机实证（「小刺」可见、「猬」空白）交叉吻合，方法可信。

注：plan.md 记述的正文 14px 与板级实际（30px 基础子集/正文行为吻合 font_puhui_30_4）有偏差——自产子集后该偏差消解，不另行追溯。

## 决策（human 拍板，2026-09-12）

1. **正文 = 自产子集字体**（lv_font_conv 从选定字体源抽字表新产）——cbin 与全部内置字体实测不达标，自产为唯一路径（human 确认）。
2. **正文字表 = GB2312 全集 6763 字**（含犭旁生僻动物字「猬猾狸」）——缺字策略主体 = 全集一次到位，辅以抽样生字实测锁定（「猬」等抽样字渲染可见证据为 RP-A4 验收件）。
3. **标题（站酷快乐体）/ 数字（Baloo 2）= 都走 SPIFFS .bin 非 KAP1 独立落地** + acceptance-author 补「字体分型」验收齿（一次性工序，守 B8 资产闭集）——改字体不重烧固件。
4. **预渲染位图路线废除**（human 裁定原文口径：时间/温度大数字非主要功能，没必要为它花大力气做图片渲染）——RP-A4 三分道路径中「预渲染位图回 KAP1」分支**不再使用**；Baloo 数字走正常字体路径（SPIFFS）。

## 三分道路径逐字钉（RP-A4/F1 兑现口径）

| 字体 | 路径 | 落地 | 守门 |
|------|------|------|------|
| 正文（字体源待 builder 选型：开源可商用、覆盖 GB2312，如阿里普惠体系） | 编译内置 或 SPIFFS（builder 按体积实测裁，二选一，理由落实现注记） | GB2312 6763 字子集，字号按 V3 阶梯正文档 | 自产非 KAP1，若 SPIFFS 落地并入「字体分型齿」闭集 |
| 标题（站酷快乐体） | SPIFFS .bin 非 KAP1 独立落地 | 标题用字子集（30px 1bpp，字表小） | 字体分型齿 |
| 数字（Baloo 2） | SPIFFS .bin 非 KAP1 独立落地 | 数字/冒号/常用标点子集 | 字体分型齿 |
| 气泡尾巴 | KAP1 闭集修订（唯一新增 KAP1，压暗用既有 d50 网点资产） | U 闭集 REVISION LOG + 跨包留痕 | ui_art_integration B8 齿修订 |

## 承接

- RP-A4 / 阶段 3 S3.2 / 阶段 4 据此执行；字体分型新齿与 U 闭集修订清单由 acceptance-author 冻结。
- 本记录即 RP-E1「字体决策记录在案」的 evidence 主体。
