---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-17
---

# M2-K3 页→窗改造（查字会话内覆盖窗 + F3 真机确认轮 + NFR-PERF-2 live 复测）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M2-char-window` nextAction 块「页→窗改造（F3 全节）」+ NFR-PERF-2 live 端到端实证。
> 需求权威：`docs/requirements.md` F3 全节（行 458–522）+ §2.2 行 68（查字窗行）/ 行 67（提醒层级口径）+ §2.3 键位矩阵 + NFR-PERF-2（行 568–572）。

## 背景摘录（锚点摘录，story 语义权威 = requirements 现文）

- **查字窗语义（行 68）**：会话内专门窗口、**不是常驻页面**——孩子说「勇怎么写」，当前页上弹出米字格 + 逐笔演示 + 速查栏；契约 E1 的 view=char 本就是 display 响应块、与问答框 text 同级，覆盖窗显示时机是实现侧，**契约零改动**。
- **窗口交互包**：播完会话即结束不回聆听态；驻留到按键或 5 分钟无操作自动关窗回原页（计时随任意按键重计，v1.43）；KEY 短按 = 重播笔顺（含打断播报）、长按 = 关窗回原页；BOOT = 关窗回原页不翻页（v1.39 隔离口径）；窗内不显示页点；想接着问 = 关窗重新开会话（无连续会话）。
- **AC4 逐笔念笔名（v1.55）**：笔名由 OpenClaw 回复文本承载、真机已实证查「勇」9 笔零错误（含横折/横折钩区分）；试点期人工核错误率；D3 契约扩展 stroke_names 试点期**不做**（需求已注记）——云端 `server/` 本包零改动。
- **右栏对账三项（F3 行 480 实现侧对账）**：① 进度珠砍除清理（`char_page.cc:44` progress_label 死成员零赋值 + 头注虚标）；② 握笔小手 12px 圆环占位删除（`charpage_draw_penhand`）；③ 部首行补上屏（card.radical 有值、label 拼装漏——S-F3.2 AC1 义务）。
- **锚改名**：canvas-p4 → char-window（v1.38 登记的 Phase 1 dev 修复项，本包收尾）。
- **基建已就位**：layer-aware dispatch（M1-K1a 已并）、keybar overlay 页点隐藏路径（M1-K2 已并）、提醒最高层覆盖 + 录音排队例外（M1-K4 + rma8-fixbatch 已并并闭环）——本包复用前置，不重实现；只做「提醒弹窗压查字窗、不穿透」的查字窗侧对账确认。

## 改法取舍

- **层模型扩展**：新增查字窗层进 kidpal_layer 栈模型（stackable 语义一致），弹窗 = 当前页之上覆盖、pop 回原页——不走「伪页面切屏」路线（违背 v1.36 撤页语义）。
- **键位分流**：查字窗前台语义挂 layer-aware dispatch 的新层列，key_route_global 基层列本体零改动（K1a 冻结等价 guard 齿零回归；新层修订走 acceptance-author 留痕）。
- **5 分钟计时**：随任意按键重计的无操作计时器，host 侧以可注入时钟/秒数模拟冻齿。
- **逃生门**：改造若钉死需动四态骨架共享对象层或提醒/语音/查字三层仲裁架构 → 停，转 /arch-change。

## 阶段划分

1. acceptance-author 冻齿：M2K3-A1（层注册）+ A2（键位矩阵）+ A3（驻留计时）+ A4（页点隐藏）+ A5（右栏对账三项）+ A6（锚改名结构钉），实跑红基线落账。
2. builder：层注册 + dispatch 新层列 + char_page 改窗（覆盖形态 + 右栏三项 + 去页点）+ mcp_server view=char 显示时机改造。
3. verifier↔fixer：host 全量零新增红 + device build 闸。
4. integrator/supervisor 闭环 → F3 真机确认轮（M2K3-A7 human 持板：S-F3.1 AC1–AC4 / S-F3.2 AC1 / S-F3.3 AC1–AC2 / 交互包全项 / 提醒层级对账 / K1 两缺陷 live 复证）+ NFR-PERF-2 live 复测（M2K3-A8：5 秒档，未达升 human，不预绑优化包）。

## 铁律约束

- 不改 `docs/contracts/**`（E1 view=char 既有；stroke_names 契约扩展不做）、不改 spec、不改 agent 定义/gate。
- AC4 笔名承载 = OpenClaw 回复文本；错误率不可接受时的回退路线（纯引导语 / 权威笔名数据源）= 届时 human 裁，不预实现。
- 提醒/语音层既有闭环不重实现、不顺手改；云面 `server/` 零改动。

## demo 证据形态复述

host：A1–A6 齿全绿留痕；真机：A7 确认轮清单逐项 + 串口/网关日志/human 目击三方互证（沿用 RM-A8 取证形态），A8 延迟计时与 2026-09-15 实测 ~21.6s 口径对齐；证据落 `delivery/changes/dev/<changeId>/`。里程碑 demo 口径 = 孩子说「勇怎么写」当前页弹窗、9 笔完整演示 + 速查栏全四行、按 KEY 重播、5 分钟自动关窗回原页、NFR-PERF-2 计时在档。
