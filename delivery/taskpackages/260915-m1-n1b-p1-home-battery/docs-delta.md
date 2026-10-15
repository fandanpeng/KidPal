# docs-delta — 260915-m1-n1b-p1-home-battery

> integrator 产出 · 2026-09-16 · 结论：**docsSyncRequired = false（canonical docs 零改动，无 docs 面增量）**。与 TaskPackage `docsSyncRequired:false` + `docsSyncReason` 及 final fact（r6）docs-sync 闸 `ok:true` 一致。

## 判定与逐条理由

1. **docs/requirements.md（spec 面）零改动、零需要**：本包落地的是 requirements S-F1.1（「常显首页看接下来干嘛」AC1–AC4）**已冻结在文的口径**——「接下来」主体版式（WF-F1-01 v1.4 重排序）、辅助条小图标、电量 ADC 接线（Phase 1 修复项）、低电闪烁最小信号、断网本地显示。实现向 spec 对齐，spec 不向实现漂移。PH-E1 兑现：首页日程数据口径零改动（`home_p1.h` schedule API 面未动，课表数据接首页归 M3 决策 26 链路），spec 面零修订。
2. **docs/contracts/（契约面）零触碰**：本包无 shim API / display 统一响应块 / 笔顺 / 课表结构触及，final fact contract-lint 闸 `ok:true`；契约面恒只读纪律未破（本包 git diff 无 docs/ 路径文件）。
3. **实现侧注记承载位置**：随代码注释（`adc_battery_monitor.h` 外部句柄构造重载注记、`waveshare-s3-rlcd-4.2.cc` 板级注入 corr_id 注记、`home_p1.c` v1.4 版式/缩放 blit 注记）与本 closeout 目录制品（`device-round-record.md` / `review-packet.md`）承载，无独立 docs 页面需要新增或更新。**日期格式实现先行侧注记（2026-09-16 刷新补）**：首页日期 human 裁定改英文 `DD Mon` 格式（如 16 Sep）——显示层格式非数据口径，spec-lane 不触；requirements S-F1.1 / WF-F1-01 现文仍为中文日期，实现与 spec 的该处不一致属**在案已知漂移**（ARQ-260916-home-date-format-english 在册，human 自行后续同步需求/设计文档，同步完成前不另开 review）——维持「实现先行 + 需求/设计殿后」口径，docs 面本相位仍零改动。
4. **delivery 面登记**：`delivery/milestones/milestones.jsonl` / `delivery/workstreams/m1-home-qa/progress.md` 的里程碑推进由主线程对账轮统一渲染（与 260915-m1-k1a-key-layer-dispatch 先例一致，Pod 内不改 workstream 汇总面时由 integrator 如实申报）。

## 漂移自检

- `git diff --name-only -- docs/` = 空（canonical docs 零触碰）。
- spec↔contract 一致性：无新行为面引入（电量百分比展示与低电闪烁均为 S-F1.1 已冻结文本；电池注入缝是固件内部 seam，不暴露契约面），无 contract 暴露需要变化。
- PH-E1 范围纪律自检：日程数据口径零改动（schedule API 面 git diff 无符号变更）；小伴睡姿演出（已砍项）零触碰。

## follow-up 候选登记建议（供主线程对账轮 / planner 参考，非本包义务）

1. **host harness 挂真资产 + 收紧齿（2026-09-16 刷新：部分销账）**：验收 fixture 默认不挂 art（weak 哨兵走兜底路径）的盲区已部分销账——接棒批落 host 截图 harness（`firmware/main/test/host_app/screenshot/home_p1_screenshot.c`，挂真资产走真字形路径出 PNG，human correction `416e4707` 工作法基建，并暴露 NTFS 大小写碰撞的 host 绿≠真机绿跨端分歧教训）；剩余部分 = 「图标半区相邻空隙零墨」收紧齿仍未入 acceptance（原 fixer 两轮手写 ink 实测表证据仍在册），归后续测试基建批。
2. **基座 chrome 状态栏联网小标尺寸**：human 真机观察偏大一圈——属 `LvglDisplay::UpdateStatusBar` 共享底座面、非本包 seam，与 k2-keybar 包潜在交叉，待 human 裁归属后由归属包承载。
