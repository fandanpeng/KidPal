---
changeId: 261028-m6-k3-p3-story-typography
title: 故事列表/阅读排版与 M6 真机收口——16px 正文、20px 居中省略标题、自然行 3
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261028-font-typography-scaleup/PROPOSAL.md
  workstream: 261028-font-typography-scaleup
reviewed-by: architect（/triage-change 分流 review 2026-10-28；门 1 待 human 派 architect）
date: 2026-10-28
gateProfile: ["evidence-integrity", "test-quality", "device-round"]
dependsOn: ["261028-m6-k1-font-assets-runtime-loading", "261028-m6-k2a-p1-home-16px", "261028-m6-k2b-p2-schedule-16px"]
mergeOrder: 63
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- 故事列表与阅读正文目前仍以 14px 为主，列表标题缺少可靠的超宽省略约束，长标题会挤压日期列；阅读标题需要独立的 20px 档，正文每屏自然行数也需随字号上调收敛。
- F-4 将阅读自然行由 4 调为 3，并把短段落疏密与翻页成本留作真机终判；F-6 要求 20px 标题缺字回退 16px，不能用静态子集碰运气。
- requirements v1.69 已冻结列表标题省略、阅读 16px/20px 居中单行省略及自然行分屏口径；V8 `canvas-p3` 与 `p3-reader` 提供两帧布局权威。

# 可行性判据

- P0 合并后提供 16px/20px 同源资产与统一回退句柄；`story_page` 的列表/阅读渲染、`story_page_v2` 真帧探针和 mcp 两个分页消费点均在位，可分别做 host 与设备回归。
- P0/P1/P2 全部合并后，本包可在同一真机轮完成故事两态与 workstream 四页并集终判，避免独立 P4 再多一次烧板和合并边界。
- UI-ready 判据满足：requirements v1.69 frozen、V8 两帧在盘、brief v7 对齐注记在位。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **P4 并入本包**：本包除故事排版外还是 M6 终端真机 owner，因此 `dependsOn` 在 P0 之外增加 P1/P2。P0 探针证据和 P2 pitch 轮是输入，P1 真机半并入本包；未齐三包不得宣布 workstream 收口。
- **统一字体语义**：P0 定义资产装载、20px 缺字回退 16px 与缺资产回退 14px；本包只在真实列表/阅读消费点验证，不另造加载或回退分支。
- **相邻冻结锚扫查（拆包期非零命中）**：`story_page_v2.acceptance.c` [K9] 钉 `font_puhui_14_1` 与 14px，文件头还留 13.5px/1.72 旧排版锚；`story_page.h` 与 mcp 两个消费点钉 `KSTORY_READER_LINES_PER_SCREEN=4`。处置 = acceptance-author 收紧型留痕重基线；输入表、计数、列表分屏、录音/整理态和云端载荷形状不在撤改面。
- **mcp 影响面补录**：图 trace 对 `kidpal_story_reader_slice` 只解析到 story_page 与 archive caller，未覆盖 `mcp_server.cc` 两个常量消费点；Grep 已交叉补录，两个点必须同步，archive `happy_flow_capture` 只读不改。
- **其他包边界**：P1/P2 的文件不进入本包实现面；本包终端轮只复证其效果。P2 的 schedule restore 与 P1 的 home wire 均不得因本包 mcp 同文件改动而回归。

# 铁律约束

- 阅读正文每屏自然行=3；标题 20px 水平居中、单行、超宽尾部省略，不压正文；20px 缺字回退 16px。列表日期列必须完整可见。
- 录音、整理、盖章、删除确认、正文取不回提示等未变更页态沿用既有设计，不跟随放大，不改交互或会话生命周期。
- 契约/spec/design 零改；不改变 story_list/story_text 形状、seq、restore 或本地正文缓存语义。
- 冻结验收修订只走 acceptance-author 收紧型留痕 supersede，记录旧/新断言与授权；注释不算证据，撤改法必红。
- 终端真机轮是收口硬证据，不能用 host 字形或单包生产者自证替代；human 未终判不得宣布 M6 完成。

# demo 证据形态（与 roadmap 一致）

- host runner：列表 16px/长标题省略、阅读 16px/自然行 3、20px 居中省略与 16px 回退、mcp 两个消费点同步的 RED→GREEN 和变异证据；既有故事/记录会话回归零新增红。
- device build + V8 两帧 design review。
- 终端真机轮：故事翻页手感与长标题，加上 P1 首页、P2 课表、P0 缺 assets 降级的四页并集目击；照片、串口日志和 human 终判留痕。
