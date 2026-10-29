# review.md · 261028-font-typography-scaleup 分流 review

> reviewer = architect（/triage-change 四面探测 + 五档裁决，2026-10-28，只读 + 同会话 return）。
> 本件为 sign-off 记录，全文报告见 spine `delivery/intake/261028-font-typography-scaleup.md` faceMap/facePlan 与会话留痕。

## Findings 与处置

| # | finding | 级别 | 处置 |
|---|---|---|---|
| 1 | S-F2.3 补充规则钉「13.5px 起、行高 1.72」（requirements.md:479），不改则 spec 与实现立即失真 | MUST（上游） | F-1 走需求面档 2 修订；执行语义 = 各页面包硬 launch 依赖 |
| 2 | S-F1.2「逐门列出」无容量上限，晚上 4→3 后超限语义缺失 | MUST（上游） | F-3 容量口径落字（human 触点① 确认） |
| 3 | 「font_body 运行时接线」残余只在 taskpackage 文档（260920-m2-charwindow-card-fix plan.md:18）未登记 debt | SHOULD | spine openQ q4 裁不另建条目，P0 spike 为正式清偿载体 |
| 4 | architecture.md:299-301「三族 SPIFFS 加载」与实现（编入固件）漂移；三处「未实测」表述过时 | SHOULD | F-5 / 架构面批内勘误 |
| 5 | 编入固件路线不可行：ota_1 余量 302KB < 16px 全量约 324KB | 事实约束 | F-5 落位 = assets .bin + cbin 运行时装载 + 14px 兜底 |
| 6 | assets .bin → cbin 装载 → 1bpp 渲染通路设备上从未接过（fonts_root 零消费者） | 事实约束 | spike-first 命中：P0 独立成包前置，消费端观测（真机看屏 + 缺资产不黑屏） |
| 7 | host 字形永不等于真机（HOST_TEST_CONVENTIONS.md:237-249），字体数据双端不同源 | SHOULD | F-7 host 同源加载，spike 内验证 |
| 8 | 工作树 3 个未提交改动（font_size_test_page 防崩修正等） | NOTE | PROPOSAL §6 留根：P0 开工前单独收编，不混提 |

## 裁决（sign-off）

- **五档 = ③ 拆包**（跨 4 切片 + spike 前置 + 多 Pod；nano 排除清单逐条命中；② 单包违反 spike 独立成包条款）。
- **workstream riskTier = high；全包 rigor = full**（快路径不适用：决定性工作在 host_app 覆盖不到的设备 seam——assets 烧写、开机 mount、cbin 装载、1bpp 真机字形；真机轮必须有配额）。
- **facePlan = 需求面档 2 → 架构面 /arch-change → 实现面拆包**，串行；契约面零触点、governance 零触点。
- 单向守卫：触契约不成立（已取证），high 信号齐备（跨包/冻结验收面/设备专有编译烧写面/UI 布局）——不给 lite。

sign-off：architect 2026-10-28（同会话 return，零写入）

## 门 1 · 拆包 checklist review（2026-10-28）

- reviewer = architect（门 1 轻量 checklist，六项全 PASS、零打回，GREEN）。
- 切法 = 四包：P0 `261028-m6-k1-font-assets-runtime-loading`（spike，full/high）→ P1 `261028-m6-k2a-p1-home-16px`（full/high）+ P2 `261028-m6-k2b-p2-schedule-16px`（full/high，可并行 WIP=2）→ P3 `261028-m6-k3-p3-story-typography`（full/high，终端真机轮，dependsOn P0/P1/P2）；P4 并入 P3。
- 对 task_plan 底稿的三处细化均判「合理细化」：P4 并入（PROPOSAL §5 本就允许）+ P1/P2 dependsOn 防提前终判；riskTier medium-high→high（枚举合法化，向严归一）；F-9 跨包承接（首页半归 P1 / 课表半归 P2，spine q5 口径）。
- 机检：devloop-artifact-check 四包全绿；render-roadmap --self-check 绿；taskpackage-required-check 仅 Draft 预期四项待 Pod 内 acceptance-author 补齐。
- 状态：**human 过目通过（2026-10-28，原话「切法通过，P0 我单独 session 启动」）** → commit 专用 base ref → P0 由 human 另起 session 走 `/build-feature 261028-m6-k1-font-assets-runtime-loading`（launch 面预备 = kidpal-pod-launch-prep skill）。
