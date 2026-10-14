---
id: ARQ-260916-unit-gate-archive-recursion
title: verify.mjs final 相位 unit 闸对 `server/acceptance/archive/` 归档件仍做 pytest 收集——sweep「移位即退役」口径只对治理闸 glob 成立，对 unit 闸不成立，final 闸系统性常红 + 裸 pytest 基线 121F 假红
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-16
closed: 2026-09-16
closedBy: ""
paths: []
source: ""
---

human 指示快速处理（一次性直修，如实记录跳过治理批流程；**闸修改事实如实声明**：修法含 `.claude/scripts/unit-run-check.mjs` 枚举面一处）。修法 = ARQ 候选①+②组合，起因是落地前实测证伪候选①单独成立的假设——**pytest 对显式传参文件不受 `norecursedirs` 与 conftest `collect_ignore` 约束**（260916 探针实证：两排除全挂仍 8/11 收集），而 unit 闸枚举后逐文件显式传参；①测试树 `server/pytest.ini` 加 `norecursedirs`（整项覆写 pytest 默认故保留默认形态 + archive）治裸 pytest 递归路（基线 121F 假红）；②闸 `unit-run-check.mjs` 的 `SKIP_DIRS` 补 `archive`（枚举跳退役面，固件侧 `main/test/acceptance/archive/` 同口径；对偶 sweep「移位即退役」语义——归档件按定义非现役 unit 面）治显式传参路；自检电池补实盘断言 `real-repo-no-archive-collected`（枚举命中 archive 路径即红，防 SKIP_DIRS 回退静默复发）。验证三路全绿：闸 `--self-check` 10/10（pyFiles=1，归档 8 件零枚举）+ 闸真实执行路 37 passed exit 0 + 裸 pytest 基线 37 passed（原 121F 陷阱消除）+ `acceptance-run-check.mjs --self-check` ok（治理闸口径不受影响）。证据 = 本次落地 diff（dev/260915-m1-k1a-key-layer-dispatch 分支，commit `4fc0969`；architect 复核裁定有保留 GREEN，保留项 R1 补 `*.egg` / R2 本行记录修准 / 断言 detail 补 fw 面已随本 commit 收口，R3-R4 登记 ARQ-260916-acc-gate-archive-skew follow-up）+ 探针输出。
