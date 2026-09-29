---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 重拆重审 2026-09-29）
date: 2026-09-29
---

# M4-K2 板端 P3 展示面重构（键路由补全 + 列表 + 删除覆盖层 + 阅读 + 盖章对账 + CJK 字体接线）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M4-star-notebook`（state=planning）nextAction 块①板端展示半 + 块②（F2 改名对账四项）。
> story 语义权威 = `docs/requirements.md` 现文（S-F2.1 AC1–AC4 + S-F2.3 AC1–AC2 + S-F2.4 AC1–AC2 + WF-F2-01/03/04/05 + F2 intro 对账块 v1.33 + §2.3 矩阵 P3 三行与消歧 dev 注记 + NFR-READ-1）；契约形状权威 = `docs/contracts/interface-contract.md` 现文 §1.3③④/§四（K1 落地批已冻结）；设计稿权威 = docs/ui/kidpal-ui-design-v5.html P3 组 + frozen 逐页 PNG（`docs/ui/V5/` P3 五面，uiux block designRefs 挂图，builder layout 常量按 PNG 定坐标）。
> 重拆注记（2026-09-29）：本件按 260928/260929 拆包新规收窄为五件骨架——「改法取舍」「阶段划分」撤出（包内实现决策承载位 = 冻结 acceptance 断言化 + taskpackage notes + planner 派发 prompt；planner Step 1g architect 前置预评后补写「技术方案」节并留痕）；原拆包侧实现决策要点已压缩进 taskpackage `notes` 备查。

## 背景摘录（新建型锚「现状与差距」）

- **现状·四态渲染在世但口径陈旧**：`story_page.c`（1390+ 行，260829-story-p3 产物、头注 FROZEN OFFLINE CONTRACT）四态渲染函数齐（LIST/READER/RECORDING/STAMP）；但页内输入表 = v1.27 前旧口径（KEY CLICK=光标下移、KEY 双击=进阅读、录音态 KEY 长按=丢弃——与 v1.47 矩阵相反/错位），列表 = 页名「我的故事」+ `●` 行 + 反白选中 + 方块堆计数，盖章文案 `KSTORY_STAMP_FMT` =「故事徽章」，阅读 KEY 短按仍产 `KSA_ACT_TTS_READ`（朗读 v1.52 已砍）。
- **现状·设备侧 P3 按键只有一条缝**：`waveshare-s3-rlcd-4.2.cc:674-677` BOOT 长按 → `HandleStoryKey` 唯一调用点；BOOT 短按恒走全局翻页、BOOT 双击接线已被移除、列表态 300ms 单双击消歧无机制。M1-K1a 已交付 `key_route_layer(key, g, overlay_top)` 分层路由 + `kidpal_layer` 层栈（STACK_MAX=8 有余量、头注授权 append-only 插入）——P3 页内手势族接线是本包在既有分流机制上的增量。
- **现状·CJK 字体零引用缺陷**：`story_page.c` 全文件不设字体 → LVGL 默认 montserrat_14（纯拉丁）——P3 全部中文面真机空白（v1.56 查字窗同族缺陷已修先例在案：显式接线 `font_puhui_14_1`，`lcd_display.cc` 五处 + `char_window_overlay.acceptance.c` LV_FONT_DEFAULT 负向齿）。
- **现状·数据消费面**：`mcp_server.cc` story_list/story_text 路由 + `kidpal_story_list_parse` + `fresh_save` 判定 + `kidpal_render_node_cache_story` 缓存 seam 在世；K1 已 merged（契约三触点随落地批进契约现文，`story_list` 推送在世——建连即推时点被 260928-story-connect-push-defer 摘除，现役两时点，见跨包节）。
- **现状·覆盖层基建齐备**：layer 栈 + `begin/end_*` 持锁夹钳（reminder/char_window 两样板）+ `dismiss_front_overlay` + 查字窗 `idle_step` 5 分钟自动关窗先例——删除确认弹窗（WF-F2-05 整屏覆盖、10s 静默取消）复用基建、渲染/裁决/超时新写。
- **现状·旧冻结面已退役**：`story_p3.acceptance.c` 已 archive（`test/acceptance/archive/`），本包新建齿不走 supersede；`story_page.h` FROZEN 头注随本包重写声明退役口径（新冻结面 = 本包新齿）。
- **差距**（本包交付）：① P3 键路由补全（全手势族分流 + 300ms 消歧 + 永不离页）；② 列表态重构（v1.28/30/31 交互 + 星星图标 + 对账②③④）；③ 删除确认覆盖层（WF-F2-05 全套）；④ 阅读态对齐（翻屏 wrap / KEY 长按回列表 / 朗读退役 / 排版实测）；⑤ 盖章对账（文案① + seq 消费 + 0.8s 定格消费方）；⑥ CJK 字体接线。

## 可行性判据

- **依赖前置实存**：K1 已 merged 本地 main（merge-completed e12a8b94）——story_list/story_text/delete 消费形状可即读契约现文核对，无上游在途等待；契约门 LP1 verified。
- **基建 seam 全部在世**（图上核实）：`key_route_layer` 分层路由（M1-K1a merged）、`kidpal_layer` 层栈 append-only 授权、覆盖层夹钳两样板、`kidpal_voice_states` 场景消费、键帽条三槽、art 资产集（ic_star_48/38/22/20/18 + stamp_seal_r8 + rec_wave + pose_jump 在 47 资产集、art 分区已烧）。
- **UI-ready L2 前提成立**：P3 组 frozen 逐页 PNG 五面在盘（`docs/ui/V5/canvas-p3.png` 列表 + p3-recording / p3-stamp-frame / p3-reader / p3-delete-confirm）；版本行分形核对 = 设计供给 v5 对齐 requirements v1.57、其后的 v1.59/v1.60 均为 F3 查字窗面零 P3 牵连——对齐版本（v1.57）不晚于当前 frozen 最新版本行（v1.60），判据满足。
- **host 齿基建在世**：`espidf_wsl.sh` build+run 双通道既有、`key_layer_dispatch.acceptance.c` 回归靶在世（零触碰、append-only 扩层表）、`espidf_wsl.sh` 通道②新齿独立红先例（K1/defer 包同款流程）。

## 跨包事实与共享冻结面约束（含跨切片边角）

- **K1 冻结形状（消费权威，不得自定字段）**：story_list = items 最新 ≤16 条窗口 + total 恒全量（契约门 Q2 丙案裁定 2026-09-24；板端 `KSTORY_LIST_MAX=16` 维持、计数恒信 total——items.length 不顶替，>16 篇窗口条数 ≠ 计数；窗口外旧条目设备端不可达 = 知情裁定 CCP §5 注记）；story_text.data.seq = 盖章 N 唯一权威源（§1.3④，易失 stamp_index 退役）；kidpal_story_delete = §四端→网关单向私有消息（发送后不依赖 RPC 应答、靠 story_list 重推对账；隐式 ack 超时重发形态随契约 Q5 冻结）。
- **story_list 推送时点现状**：K1 原设计三时点（建连 + 归档后 + 删除后），其中①建连即推被 defer 包 260928-story-connect-push-defer 摘除（真机实证：每会话 WS 重连重复推送盖页），现役 = 归档后 + 删除后两时点；①恢复与否/以何形态恢复 = K3 重裁（crossPackageBehaviors 重裁条）——**K2 列表渲染消费面不受影响，但「进页即见列表」的时点保障语义随 K3 重裁结果对账**，K2 acceptance 冻齿时不得预钉建连时点。
- **K2/K3 缝切分（同文件族，串行双写）**：录音态**渲染**（REC/时长/声波 + WF-F2-02 版式 + 回显区占位缝）与**输入表语义位**（`kidpal_story_page_input` 消费缝按 v1.47 矩阵重写：KEY 长按=同短按保存、BOOT 三手势=丢弃）落在本包；录音**会话动作接线**（session_ctx 发送、引导态、stt 回显消费、收尾/丢弃会话侧执行、120s 演出收口）归 K3——K3 对本包冻结的输入缝写齿、不改语义。`mcp_server.cc` story 路由面双写 = mergeOrder 39→40 串行（本包先）。
- **发送原语单缝**：本包为 kidpal_story_delete 落 protocol 层 §四消息发送原语；K3 的 kidpal_session_ctx 发送同族扩展（同一缝）——原语形态一次做对、两包共用。
- **文件面不相交（尺寸维判据）**：`story_page.{c,h}` 本包独占重写；K3 只接输入事件与回显消费、不重写渲染面。
- **M1 基建 seam 零修改清单**（只消费）：`kidpal_voice_states` / 键帽条三槽 / begin-end 夹钳 / `dismiss_front_overlay` / `pop_transients` / `key_layer_dispatch.acceptance.c` 既有断言（新层值 append 走冻结纪律）/ 提醒/查字/课表/P1 四页渲染路径 / PAGE_COUNT 与页循环（v1.36 撤 P4 纪律）。
- **层栈新值约束**：删除确认覆盖层 = 新层值 append-only、低于 REMINDER（提醒最高层压过删除弹窗，矩阵口径）；入 transient 集（pop_transients 消费——BOOT 隔离/清层语义自动成立）。
- **deferred live 半**：真机 1bpp 目测（列表/弹窗/阅读/盖章四面）+ 300ms 消歧手感挂 K3 设备轮一次性收口（单真机不两轮撞车）；本包 demo 口径 = host 全量绿 + device build 闸 + deferred 挂账声明。
- **跨切片边角（builder 易漏）**：① 空态文案渲染入口在 `kidpal_render_node.cc` 非 story_page——两处都要改、口径一致；② 300ms 消歧仅列表态生效（状态门控，其余状态单击零延迟）；③ story_page.h FROZEN 头注 = 老包 offline 契约已随 archive 验收退役——按 v1.47 矩阵重写输入表，勿误守旧冻结；④ `fresh_save` 判定语义保持——盖章触发链不动，只换 N 源与文案；⑤ 若星星徽章美术需新形态资产 = art 分区变更、真机烧录 app+art 双分区（K4 空框教训）。

## 铁律约束

- 不改 `docs/contracts/**`（消费契约现文冻结形状）；不改 spec、不改 agent 定义/gate。
- `key_layer_dispatch.acceptance.c` / `char_window` / `reminder` / `schedule_grid` / `p1_home_battery` 等在世冻结齿零触碰（回归靶非修订面；层栈新值 append-only）。
- M1 基建 seam（见跨包节清单）只消费零修改。
- 提醒最高层语义不破（删除弹窗层值 < REMINDER；录音态提醒排队例外 RM-A3 零触碰）。
- 降级语义沿用：解析畸形/缺字段一律友好降级不盲渲染（V27）；seq 缺省盖章降级不带 N。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

host：新冻结齿 RED→GREEN 全程留痕（红名 verbatim + mutation 应用/还原记录）+ device build 绿；真机半不在本包——demo 口径 = host 全量绿 + 真机目测半 deferred 挂账声明（挂 M4-K3 设备轮，human 在场一次性目击：WF-F2-01/03/04/05 版式 1bpp 目测 + 300ms 消歧手感）。
