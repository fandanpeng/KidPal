---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-21
---

# M3-K2 板端 P2 一周总览网格页重写 + 首页「接下来」注入接线

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M3-schedule`（state=planning）nextAction 块①板端半 + 块③消费半。
> story 语义权威 = `docs/requirements.md` 现文（S-F1.2 AC1–AC4 + WF-F1-02 线框 + NFR-READ-1；S-F1.1 AC1–AC2 消费半）；契约形状权威 = K1 契约门定稿的 interface-contract.md §1.3②/⑥。

## 背景摘录（新建型锚「现状与差距」）

- **现状·旧列表页**：`firmware/main/display/schedule_page.c`（872 行）= MVP 260829-schedule-p2 产物，消费契约旧形状 `{title, items:[{time,name,location}]}`（自足式最小 JSON 扫描、纯 LVGL、host 两侧同源可编、-1 降级语义——这些工程纪律沿用）；渲染节点 `kidpal_render_node.cc` 侧 `cache_schedule` 现为 16 项列表缓存（`s_sched_items[16]`），`cycle_next` 翻页读此缓存。
- **现状·差距**：requirements v1.19 已把 P2 改为一周总览网格（周一~周日 × 上午/下午/晚上、学校课逐节/课外课逐门、今天列高亮、※ 临时标记、星标呼应、小字 ≥14px、无格内滚动）——旧列表版式整体作废，需整页重写；首页 `home_p1.c` 的 `kidpal_home_p1_schedule_inject` 冻结 seam（day_key + items + 明日预告）已 host 验收但**从未有真实数据注入**（front 无 producer，K1 补 producer 后本包接线消费）。
- **前置已就绪**：三页循环 HOME→SCHEDULE→STORY（M2-K2 merged）、键帽条三槽（M1-K2 merged）、RTC 日期注入（M1-K1a merged，「今天」列推导依赖）；旧冻结验收 `schedule_p2.acceptance.c` 已 archive 退役，重写不走 supersede。
- **形状依赖**：网格 payload / home data 形状 = K1 契约门定稿——K1 契约 diff 落地前，本包 acceptance-author 只冻与形状无关齿（版式结构/今天列/回归靶），形状消费齿等 K1 CLOSED 后冻。

## 改法取舍

- **整页重写而非渐进改**：旧列表版式与周网格无共享版式逻辑（布局模型完全不同：线性列表 vs 7×3 网格），共享的只有 safe-truncate/HH:MM 解析等小工具与降级纪律——保留工具函数、重写版式主体。
- **「今天」列板端推导**：设备 RTC 本地日期（spec 明文「今天按设备本地日期零点切换」）；不依赖云端下发（断网跨天场景设备自主，NFR-OFF-1 语义一致）。
- **缓存结构网格化**：`cache_schedule` 从 16 项列表改网格缓存（容量上限按 7×3×每格上限定容，常量冻结进验收文件头）；BOOT 翻页 cycle_next 浏览读缓存——云端没推过数据 = 空态降级画面（K1 三触发保证常推）。
- **首页注入只消费不改 seam**：`home_p1` 冻结 seam 零修改（home_p1.acceptance.c 不动），本包做「mcp 消费 → seam 注入 → P1 在屏时重渲染」的接线层；缺/空 data 行为不变（向后兼容）。
- **版式高度预算**：400×300、页内无标题无提示行（v1.20 已删，操作提示归全局键帽条）——网格可用高度 = 全屏减键帽条；上午格 4 行课程名是字号压力最大面，14px 下限（NFR-READ-1 实测定案）内排布、行数自然变多时格内溢出走截断降级不做滚动（spec 口径）。
- 真机小字目测不预绑结论——NFR-READ-1 阈值已实测钉死 14px，版式按 ≥14px 排即合规；真机观感复核归 K3 设备轮。

## 阶段划分

1. acceptance-author 冻齿：版式结构齿（A1）/ 今天列齿（A2）/ 标记齿（A3）/ 解析降级齿（A4，形状齐后补冻）/ 注入接线齿（A5）/ 回归靶（A6）；实跑红基线落账。
2. builder：schedule_page.c 周网格重写 + cache_schedule 网格化 + mcp_server 新形状消费 + home 注入接线层。
3. verifier↔fixer：host runner 全量零新增红 + device build 闸（页面组件面必触）。
4. integrator/supervisor 闭环；真机目测 deferred 声明（M3K2-A7 挂 K3 设备轮）。

## 铁律约束

- 不改 `docs/contracts/**`（消费冻结形状）；不改 spec、不改 agent 定义/gate。
- `home_p1.acceptance.c` 与 home_p1 冻结 seam 零修改（仅消费）；`key_layer_dispatch.acceptance.c` 层表断言零触碰（回归靶非修订面）。
- 降级语义沿用：解析畸形/缺字段/越界 cap 一律 -1，友好降级绝不盲渲染（V27）。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

host：新增齿 RED→GREEN 全程留痕（红名 verbatim + mutation 应用/还原记录）+ device build 绿；真机半不在本包——demo 口径 = host 全量绿 + M3K2-A7 deferred 挂账声明。WF-F1-02 版式真机 1bpp 目测 + 翻页手感的最终目击证据归 M3-K3 设备轮（M3K3-A3）。
