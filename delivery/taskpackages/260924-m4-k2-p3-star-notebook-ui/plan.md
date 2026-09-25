---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-24
---

# M4-K2 板端 P3 展示面重构（键路由补全 + 列表 + 删除覆盖层 + 阅读 + 盖章对账 + CJK 字体接线）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M4-star-notebook`（state=planning）nextAction 块①板端展示半 + 块②（F2 改名对账四项）。
> story 语义权威 = `docs/requirements.md` 现文（S-F2.1 AC1–AC4 + S-F2.3 AC1–AC2 + S-F2.4 AC1–AC2 + WF-F2-01/03/04/05 + F2 intro 对账块 v1.33 + §2.3 矩阵 P3 三行与消歧 dev 注记 + NFR-READ-1）；契约形状权威 = K1 契约门定稿的 interface-contract.md §1.3③④/§四；设计稿权威 = docs/ui/kidpal-ui-design-v5.html P3 组。

## 背景摘录（新建型锚「现状与差距」）

- **现状·四态渲染在世但口径陈旧**：`story_page.c`（1390+ 行，260829-story-p3 产物、头注 FROZEN OFFLINE CONTRACT）四态渲染函数齐（LIST/READER/RECORDING/STAMP）；但页内输入表 = v1.27 前旧口径（KEY CLICK=光标下移、KEY 双击=进阅读、录音态 KEY 长按=丢弃——与 v1.47 矩阵相反/错位），列表 = 页名「我的故事」+ `●` 行 + 反白选中 + 方块堆计数，盖章文案 `KSTORY_STAMP_FMT` =「故事徽章」，阅读 KEY 短按仍产 `KSA_ACT_TTS_READ`（朗读 v1.52 已砍）。
- **现状·设备侧 P3 按键只有一条缝**：`waveshare-s3-rlcd-4.2.cc:674-677` BOOT 长按 → `HandleStoryKey` 唯一调用点；BOOT 短按恒走全局翻页（对账④「P3 的 BOOT 短按族不存在」）、BOOT 双击接线已被移除（对账③）、列表态 300ms 单双击消歧无机制（对账⑥）。M1-K1a 已交付 `key_route_layer(key, g, overlay_top)` 分层路由 + `kidpal_layer` 层栈（LOCAL_PAGE/VOICE/SERVER_PAGE/CHAR_WINDOW/REMINDER/PROVISION，STACK_MAX=8 有余量、头注授权 append-only 插入）——P3 页内手势族接线是本包在既有分流机制上的增量。
- **现状·CJK 字体零引用缺陷**：`story_page.c` 全文件不设字体 → LVGL 默认 montserrat_14（纯拉丁）——P3 全部中文面（列表标题/日期/提示/阅读正文/空态）真机很可能空白（v1.56 查字窗同族缺陷已修的先例在案：显式接线 `font_puhui_14_1`，`lcd_display.cc` 五处 + `char_window_overlay.acceptance.c` LV_FONT_DEFAULT 负向齿）。
- **现状·数据消费面**：`mcp_server.cc` story_list/story_text 路由 + `kidpal_story_list_parse` + `fresh_save` 判定 + `kidpal_render_node_cache_story` 缓存 seam 在世；story_list 云端推送 K1 接线（本包消费冻结形状）。
- **现状·覆盖层基建齐备**：layer 栈 + `begin/end_*` 持锁夹钳（reminder/char_window 两样板）+ `dismiss_front_overlay` + 查字窗 `idle_step` 5 分钟自动关窗先例——删除确认弹窗（WF-F2-05 整屏覆盖、10s 静默取消）复用基建、渲染/裁决/超时新写。
- **现状·旧冻结面已退役**：`story_p3.acceptance.c` 已 archive（`test/acceptance/archive/`），本包新建齿不走 supersede；`story_page.h` FROZEN 头注随本包重写声明退役口径（新冻结面 = 本包新齿）。
- **差距**（本包交付）：① P3 键路由补全（全手势族分流 + 300ms 消歧 + 永不离页）；② 列表态重构（v1.28/30/31 交互 + 星星图标 + 对账②③④）；③ 删除确认覆盖层（WF-F2-05 全套）；④ 阅读态对齐（翻屏 wrap / KEY 长按回列表 / 朗读退役 / 排版实测）；⑤ 盖章对账（文案① + seq 消费 + 0.8s 定格消费方）；⑥ CJK 字体接线。

## 改法取舍

- **键路由增量而非重造**：M1-K1a 的 `key_route_layer` 分流 + 板级 `keya_front_route_top` 桥在世——本包把 P3 基层（LOCAL_PAGE 顶 = story 页在屏）的 BOOT/KEY 全手势接进 `kidpal_story_page_input` 消费（对账④收口），不新造分发层；`key_layer_dispatch.acceptance.c` 层表断言零触碰（回归靶）。
- **300ms 消歧带状态门控**：iot_button 现成 OnClick/OnDoubleClick（双击窗 350ms 设备实参既有）——列表态消歧 = 双击窗内暂缓单击动作的状态门控逻辑（纯逻辑 host 可测），**仅列表态生效**（§2.3 dev 注记：其余状态单击零延迟）；删除确认/阅读/录音态直接消费不延迟。
- **删除确认 = 新覆盖层非新页**：新层值（append-only，命名归实现如 `KIDPAL_LAYER_DELETE_CONFIRM`，低于 REMINDER——提醒最高层压过删除弹窗、矩阵口径）+ 整屏渲染函数（reminder 夹钳样板复用）+ 专属裁决函数（KEY 确认/BOOT 取消/其余 no-op）+ 10s idle-step（查字窗先例同族）；入 transient 集（`pop_transients` 消费——BOOT 隔离/清层语义自动成立）。**PAGE_COUNT 与页循环零触碰**（v1.36 撤 P4 纪律）。
- **删除发送 = protocol 层私有消息原语**：`kidpal_story_delete`（契约冻结形状）发送函数新增——与 K3 的 `kidpal_session_ctx` 发送同族（两个 §四端→网关消息共用一个发送原语缝；K2 先落 delete、K3 复用扩展 ctx，crossPackageBehaviors 在案）。本地即时（条目移除 + 计数 −1）+ story_list 重推校准（幂等消费，最终一致）。**隐式 ack 超时重发**（契约门 Q5 拟形态）：发出 delete 后未收到「不含该 id 的 story_list 重推」即超时重发（front 幂等承接）——重发计时与次数上限归 acceptance 冻齿。
- **items 上限 = 契约门 Q2 丙案定稿（2026-09-24）**：`KSTORY_LIST_MAX=16` **维持现值不抬**——front 恒推最新 16 条窗口 + total 恒全量；板端列表 = 窗口内浏览（分屏/跳屏/wrap 语义在 16 条内全成立），星星计数 ×N **恒信 total**（警示句：items.length 不顶替 total——>16 篇时窗口条数 ≠ 计数）；超限整单 -1 降级语义保持为防御面（常态不可达）。旧条目设备端可达性收窄为知情裁定（CCP §5 Q2 注记），不做板端分页取次页面。
- **列表星星图标 = 既有 art 资产消费**：`ic_star_48/38/22/20/18` 在 47 资产集内（art 分区已烧）——列表行/计数消费；**星星徽章盖章主体**（v1.33「双圈印章→星星徽章美术形态归设计定」）：先按设计稿 v5 P3 组核对——若 v5 形态 = 星星主体可拼既有资产（ic_star_48 + pose_jump 既有）则零新增；若需新形态 = 资产管线子任务在本包（gen_art_spiffs 清单扩 + **双分区烧录注记**：art 分区变更真机必须 app+art 双烧，K4 空框教训）。取舍留 acceptance 冻齿前设计核对步。
- **阅读排版实测回收估算**：`KSTORY_READER_LINES_PER_SCREEN=4` 是老包最小估算（头注自陈）——按 13.5px 起/行高 1.72（NFR-READ-1）+ 400×300 版面实测重冻常量（host 排版齿钉行数×屏数计算）；`total_screens>12` 截断回收（真实分栏，长文全篇可达）。
- **字体接线 = 显式不赌继承**：全部 CJK label 显式 `font_puhui_14_1`（v1.56 勘误教训 + lcd_display.cc 五处先例）；负向齿（LV_FONT_DEFAULT 作 text_font == 0）同批冻。
- **录音态渲染/输入表归属切分**（与 K3 的缝）：录音态**渲染**（REC/时长/声波既有 + WF-F2-02 版式对齐、回显区占位缝）与**输入表映射**（v1.47 矩阵：KEY 长按=同短按语义位、BOOT 三手势=丢弃语义位）在本包 story_page 重写内落表；录音**会话动作接线**（session_ctx 发送、引导态、stt 回显消费、收尾/丢弃的会话侧执行、120s 演出收口）归 K3——K3 对 K2 冻结的输入缝写齿、不改语义。
- **引导态/语音轮消费零修改**：`kidpal_voice_states` 场景码 M1-K3 冻结面只消费（引导态气泡文案换「需要我帮你记点什么？」的接线在 K3）。

## 阶段划分

1. 设计核对步（acceptance 冻齿前）：v5 设计稿 P3 组五面 ↔ 线框 WF-F2-01/03/04/05 逐面对账（星星徽章美术形态判定 → 资产子任务与否）；K1 契约 diff 落地状态核对（未落地 → 先冻与形状无关齿：键路由/版式结构/字体/对账文案/裁决表）。
2. acceptance-author 冻齿：键路由齿（A1）/ 列表齿（A2）/ 删除覆盖层齿（A3）/ 阅读齿（A4）/ 盖章齿（A5）/ 字体齿（A6）/ 回归靶（A7 host 半），实跑红基线落账（双通道）。
3. builder：story_page.{c,h} 重写（输入表 v1.47 矩阵化 + 四态渲染对齐 + FROZEN 头注退役声明）+ 板装配层 P3 手势接线 + 消歧门控 + 删除覆盖层（新层值 + 渲染 + 裁决 + idle-step）+ delete 发送原语 + mcp_server 消费改形 + 字体接线 +（如需）资产子任务。
4. verifier↔fixer：host runner 全量零新增红 + device build 闸（页面组件面必触）。
5. integrator/supervisor 闭环；真机目测 deferred 声明（M4K2-A7 挂 K3 设备轮）；docs-sync（对账块销账）随收口。

## 铁律约束

- 不改 `docs/contracts/**`（消费 K1 冻结形状）；不改 spec、不改 agent 定义/gate。
- `key_layer_dispatch.acceptance.c` / `char_window` / `reminder` / `schedule_grid` / `p1_home_battery` 等在世冻结齿零触碰（回归靶非修订面；层栈新值 append-only）。
- M1 基建 seam（kidpal_voice_states / 键帽条三槽 / begin-end 夹钳 / dismiss_front_overlay / pop_transients）只消费零修改。
- 提醒最高层语义不破（删除弹窗层值 < REMINDER；录音态提醒排队例外 RM-A3 零触碰）。
- 降级语义沿用：解析畸形/缺字段一律友好降级不盲渲染（V27）；seq 缺省盖章降级不带 N。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## 边角（review 前自查登记，builder 易漏）

- 旧输入表语义反转三处：KEY CLICK 列表（旧=光标下移 → 新=开记录语境会话【K3 接线，本包留缝】）、KEY LONG 录音（旧=丢弃 → 新=同短按保存【K3 接线，本包落表】）、BOOT 长按列表（旧=翻屏特例 → 新=删除确认）。
- 空态文案「还没有故事，说『我要讲故事』开录吧」→ 星星记录本口径 + 双指令（渲染入口在 `kidpal_render_node.cc:773-784`，非 story_page——两处都要改）。
- `fresh_save` 判定（archive_pend + last_saved_id）语义保持——盖章触发链不动，只换 N 源与文案。
- 分屏指示与阅读屏指示同心智（「第 x 屏 · 共 y 屏」措辞逐字一致）。
- 键帽条三槽消费：列表 [说话·双击看]/[下移]、删除弹窗 [删掉]/[算了]（页点消失）、阅读 [长按退出]/[翻屏]——v5 键名不上屏口径（v1.57 §2.2）。

## demo 证据形态复述

host：新增齿 RED→GREEN 全程留痕（红名 verbatim + mutation 应用/还原记录）+ device build 绿；真机半不在本包——demo 口径 = host 全量绿 + M4K2-A7 deferred 挂账声明。WF-F2-01/03/04/05 版式真机 1bpp 目测 + 300ms 消歧手感的最终目击证据归 M4-K3 设备轮（M4K3-A6）。
