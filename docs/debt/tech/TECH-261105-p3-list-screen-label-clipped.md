---
id: TECH-261105-p3-list-screen-label-clipped
title: P3 故事列表分屏指示「第 x 屏 · 共 y 屏」真机不可见——绘制 y=262 撞键帽条上沿 262 被裁
domain: tech
status: open
severity: MINOR
owner: 主线程（固件 P3 列表渲染面，按包分流）
trigger: 下个动 story_page 列表渲染 / 键帽条布局的包顺手修（一行常量位移动）；或 human 指定并入观感快环
opened: 2026-11-05
closed: ""
closedBy: ""
paths: ["firmware/main/display/story_page.c"]
source: 2026-11-05 真机轮（261104-k0a-four-page-skeleton K0A-E3 验收轮附带发现，human 看屏照片 + 代码坐标核对）；同轮发现者 = planner，human 裁「记 debt 后收口 K0a」
handoff: 修复 = 把 KSTORY_LIST_SCREEN_Y 从 262 移到键帽条上沿之上（如 240-248 带与 footer 226 的间距核对）；改动触 story_page.c 冻结齿面（SPV2 像素齿不探该标签位，预计零齿冲突但须复跑 story_page_v2/story_typography 确认）；host 齿探不到此缺陷（host 不画键帽条）——修复后须真机看屏验证
---

**现象**（human 看屏照片，2026-11-05 10:47）：故事列表 4 屏 16 条时，屏上看不到「第 x 屏 · 共 y 屏」分屏指示；requirements S-F2.1 AC1（v1.28）要求「列表超一屏时底部显示『第 x 屏 · 共 y 屏』」。

**根因（已钉到代码）**：分屏指示实现存在（`story_page.c:1352-1358`，`v->screens > 1` 即画），但绘制纵坐标 `KSTORY_LIST_SCREEN_Y = 262`（`story_page.c:67`）恰好落在**键帽条上沿 262**，被键帽条渲染覆盖/裁切——真机不可见。同案前科在案：`story_page.c:1562` 注释「268 被键位条上沿 262 裁切（真机照片实证）」（同类撞键帽条问题在该元素上修过，列表分屏指示漏改）。

**为什么 host 齿探不到**：host runner 不画键帽条（设备专属渲染叠层），像素探针在 host 帧上能看到 262 处的标签——host/真机不等价接缝的又一实例（K0A-E3 真机轮价值实证）。

**修复建议**：`KSTORY_LIST_SCREEN_Y` 上移至键帽条上沿之上（与 footer 226 行距核对，建议 240-248）；改动为单常量位移，须复跑 story_page_v2 / story_typography 冻结齿确认零冲突 + 真机看屏验证。
