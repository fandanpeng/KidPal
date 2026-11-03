# test-scope · 261028-m6-k2b-p2-schedule-16px

> integrator 范围圈定清单（2026-11-02）。包级 diff ref = `bfb8551e`（pod-started 在案），非 main。本清单 = 影响面圈定（detect_changes 机械辅证 + impactedTestsSeed 合成），非全量、非缩水到只剩本包档。

## 一、diff 圈定（bfb8551e..HEAD）

| 文件 | 归属 |
|---|---|
| `firmware/main/display/schedule_page.c` | changeManifest（实现主体：pitch 19 / 带 78-78-69 / 容量 4-4-3 / 下划线 / ※ 退役） |
| `firmware/main/display/schedule_page.h` | changeManifest（带锚注释 y[30..107]/y[186..254] 等） |
| `firmware/main/test/schedule_grid_16px.acceptance.c` | changeManifest（新冻结 SG16-A1..A6） |
| `firmware/main/test/schedule_grid.acceptance.c` | changeManifest（收紧型 supersede 重锚） |
| `firmware/main/test/schedule_grid_restore.acceptance.c` | changeManifest（镜像同步） |
| `firmware/main/test/host_app/main/CMakeLists.txt` | acceptance-wiring-patch.md 授权（六处增量逐字落线） |
| `delivery/taskpackages/261028-m6-k2b-p2-schedule-16px/**` | 治理制品（taskpackage.json / pod-events.jsonl / verify-runs.jsonl / acceptance-wiring-patch.md） |
| `delivery/taskpackages/current/taskpackage.json` / `current/verdict.json` | 镜像同步 + verify.mjs 脚本 fs-write 投影 |

**出界文件（1 件，已解释归属）**：`delivery/taskpackages/261031-openclaw-cron-trigger-gating/verify-runs.jsonl` +1 行 = 2026-11-02T04:01:09Z verifier-round（8 闸全绿，changeId 解析仍指 261031-openclaw）——本包 verifier 首轮启动时 current/taskpackage.json 尚未切到本包的窗口期产物，治理台账 append、零代码改动，归属无害。

**crossEnd**：`server/gateway/schedule.py`（counterpart `week_grid_payload`）零 diff ✓——云端全量下发不变（F-8），server 395 passed 基线零偏移。

## 二、测试影响面

| 范围 | 内容 | 状态 |
|---|---|---|
| 新增冻结验收 | `schedule_grid_16px.acceptance.c` SG16-A1..A6（11 tooth 族 + 结构半 FAS 键） | 全绿（修订轮后） |
| supersede 重锚 | `schedule_grid.acceptance.c` TEST 8 / 9 / 11（带 78/78/69、pitch 19、bit0 帧差） | 收紧后转绿 |
| 镜像回归 | `schedule_grid_restore.acceptance.c` 5 条 TEST | 零偏移全绿 |
| 存量回归面 | host 其余 247 条验收（解析矩阵 / A2 注入缝 / A5 upcoming 族 / 今天列 TEST 10 等） | 零偏移；仅存量 p1_auxbar AB-A1/A2/A4 三红（launch 基线锚在册、与本包无关） |
| 变异面 | M1/M2/M3/M4/M5/M6/M7/M8/M9 共 9 条全 turnedRed（M3/M6 首测 MASKED、修订轮修掉冻结件缺陷后复测转正） | 证据回填 taskpackage.json mutationEvidence |
| server 面 | 零 diff；395 passed / 9 deselected / 33 subtests passed 基线维持 | 不重跑义务（启动卡申报） |
| device build | xiaozhi.bin 0x324d10 bytes（app 分区余 9%） | 绿 |
| 真机轮 | P2S-E5 design review + 烧板看屏 | 未做——收口后主线程/human 执行，不在本清单范围内判绿 |
