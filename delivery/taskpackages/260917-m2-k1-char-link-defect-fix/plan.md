---
rigor: full
riskTier: medium
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-17
---

# M2-K1 查字链路两缺陷正式修复（LVGL 栈定容 + SVG 解析器 C 命令）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M2-char-window`（state=planning）nextAction 块「live 查字链路修复（NFR-PERF-2 黄灯转绿前提）」。
> 缺陷登记权威：`docs/requirements.md` S-F3.1 可行性注记（2026-09-15 真机复验定案）+ NFR-PERF-2。

## 背景摘录（锚点摘录，story 语义权威 = requirements 现文）

- **缺陷①板端 LVGL 任务栈溢出**：esp_lvgl_port 默认 taskLVGL 栈 7168 < char_page 动画帧 ~6.5KB 需求（pts/sc/toks 256 点局部数组），panic。真机现场：`firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc:126-131`——16K 直改是一次性实验补丁（注记原文「正式修复（预解析去栈 + 栈定容）走 dev-loop，本行届时随包收编」），本包 = 该收编义务的承接。
- **缺陷②SVG 解析器缺 C 命令**：`firmware/main/display/char_stroke.c:62` 与 ：174-241 仅支持 M/L/Q/Z 四命令；makemeahanzi 路径词汇表含 C（三次贝塞尔），勇 9 笔中 6 笔含 C 被整笔丢弃、示范字残缺。D3 契约 `strokes` = makemeahanzi SVG path 自由文本（样例即含 Q），C 命令属词汇表内——解析补全，**非契约改动**（仍按契约纪律走 architect 快审确认归类）。
- 已核 260915-m1-k1a/k2/k3/k4 与 260917-m1-k4-rma8-fixbatch 均未触及两缺陷（grep 证据在 taskpackage notes），无重复拆包。

## 改法取舍

- **栈修复双管**：① char_page 动画帧路径去栈——256 点局部数组改为静态/预解析堆承载（渲染帧路径零大栈局部数组为验收断言）；② taskLVGL 栈定容——16384 收编为命名常量 + 注记从「一次性实验」改写为正式定容依据。取舍理由：只定容不去栈 = 治标（下一个大栈消费者再爆）；只去栈不定容 = 留 7168 裸奔余量。
- **C 命令离散**：与既有 Q（二次贝塞尔）同款离散成点策略，复用 char_stroke 既有点集管线，不引入新渲染路径。
- 架构级改法（换渲染管线/重写解析器为独立模块）不在本包——按需触发逃生门转 /arch-change。

## 阶段划分

1. acceptance-author 冻齿：M2K1-A1（C 命令齿，修复前 RED）+ A2（既有四命令回归齿）+ A3（栈修复结构齿），实跑红基线落账。
2. builder：char_stroke.c 补 C 命令离散 + char_page.cc 去栈改造 + custom_lcd_display.cc 栈定容收编。
3. verifier↔fixer：host runner 全量零新增红 + device build 闸（custom_lcd_display.cc 板级面必触）。
4. integrator/supervisor 闭环；live 复证 deferred 声明（挂 M2-K3 设备轮，M2K1-A4 structural）。

## 铁律约束

- 不改 `docs/contracts/**`、不改 spec、不改 agent 定义/gate。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。
- device build 闸不得缺席（本包触板级装配面）。

## demo 证据形态复述

host：新增齿 RED→GREEN 全程留痕（红名 verbatim + mutation 应用/还原记录）；真机半不在本包——demo 口径 = host 全量绿 + device build 绿 + M2K1-A4 deferred 挂账声明。live「勇 9 笔完整无缺笔」的最终目击证据归 M2-K3 设备轮 A7。
