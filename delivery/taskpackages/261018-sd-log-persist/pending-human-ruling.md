# pending-human-ruling · 261018-sd-log-persist

> 停等点：2026-10-18 · planner · escalation eventId `8c1db24e-a926-47ca-868a-35854500a40d`（category = scope-or-permission）
> 位置：host 实现与设备编译均已绿，**verifier 内循环尚未开始**；真机门前停等仍在更后面。

## 一句话问题（只此一问）

是否授权把**已合并包 261016 的冻结验收齿** `sd_card_mount.acceptance.c` 中 SD-A2 tooth 6 的期望值 **5→8**（含四处文字镜像 + REVISION LOG append），以消解与本包 `max_files=8` 的互斥？

## 事实与证据

- 本包 reviewed plan（human 触点①已确认）设计要点 4 逐字：`max_files` 5→8（给日志文件 FD + story_sd_io + 探针留头）。上游 FROZEN PROPOSAL 决策 24 注记亦以该写盘任务为前提。
- builder 已按 plan 实现：`waveshare-s3-rlcd-4.2.cc:853` `max_files = 8`；本包冻结齿 `sd_log.acceptance.c` TEST 7 断言 8。
- 旧冻结齿 `sd_card_mount.acceptance.c`（261016，已合并）在 `:655-659` 断言同一处字面 5，另有 `:49 / :94 / :556 / :559` 四处文字镜像；文件当前 sha256 = `6ff4fca19d05cd0ecf82fd073dd1750ab90d24ddec3e46f7535ef38af3d3ed4a`。
- 两齿不可同时为真：host 全量现为 203 Tests 4 Failures = 3 条 p1_auxbar 先存红 + **1 条新增红**（`sd_card_mount...SD-A2 RED: max_files 取值非字面 5（取到 8）`）。
- governance：`docs/DEV-GOVERNANCE.md` §6 tighten-only 窗口仅允许纯 additive（删除行数 = 0、不改既有断言语义）；改 5→8 超窗，条文称「须走正常返工 / 改验收通道（可能触 human）」。本包 plan 的 changeManifest 种子与验收计划**均未枚举**该旧文件，human 触点①未被告知此跨包后果。
- architect 10b advisory（2026-10-18）：方向正确（旧齿意图 =「字面整数 + 判别机制」，5→8 只重锚、不破坏意图；「不得改判为绿」禁的是不重锚就翻绿），但**建议补一条窄 human-decision**，不建议 planner / acceptance-author 自授权。

## 选项

| 选项 | 动作 | 代价 / 后果 |
|---|---|---|
| **A（architect 推荐）** | 批准重锚：旧齿 5→8 + 四处镜像 + REVISION LOG（append，不覆盖 2026-10-16 两条）+ sha256 前→后；随后 1g 扩单评估、manifest append、acceptance-author 执行、host 全量复跑 | 最小改动；旧齿判别机制与其他断言零改动；本包可继续到真机门 |
| B | 不批准 | 回退 plan 改 5 + 重冻本包 TEST 7（TEST 7 刚冻结，改它同样超窗、改动面更大）；或旧齿保持红 → SLP-X1 不成立、verifier 必红、合并被挡 |
| C | 暂缓 / 交更晚批次 | 本包停在 host 红状态，不能进 verifier / 真机门 |

## 批准后的机械序列（不需要新 loop / 新包）

1. planner 落 `human-decision` pod-event（`payload.decision` + `reason` ≥12 字符 + `scope` + `originalInputRef`）。
2. 派 architect 做正式 1g 扩单回评（本 advisory 仅作输入）。
3. planner append `changeManifest[] = firmware/main/test/sd_card_mount.acceptance.c` + `impactedTestsSeed`（kind = frozen-file-reanchor）+ `mutationEvidence` 一条（非门判据、耐久证据）。
4. acceptance-author 执行重锚：只改期望字面与四处文字镜像 + 文件尾 REVISION LOG append；C 齿逻辑、其他断言、261016 其他收尾制品零触碰。
5. host 全量复跑：`sd_log` TEST 7 与 `sd_card_mount` TEST 2 双绿、其余零新增红；然后进 verifier 内循环。
6. 本项根因（intake/1g 未扫既有冻结齿相邻字面锚）已落 pod-events friction `scan-miss`，收口交 pod-retro → ARQ。

## 回复方式

直接回复「批准 A」/「不批准，走 B」/「暂缓 C」即可；也可给出其他口径。planner 收到后按上表续跑。
