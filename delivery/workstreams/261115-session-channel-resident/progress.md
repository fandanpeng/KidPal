# 261115-session-channel-resident · progress

（append-only；本文件记录调度判定、门结果、环境事实与遗留 human 项。）

## 2026-11-16 · 拆包与 launch 面准备

**调度判定**：P1 = `261116-channel-resident-core`（mergeOrder 77，riskTier high）先行，serial WIP=1；P2 `261117-session-end-intent-longretry`（mo78）等 P1 合 main 后再 launch。依据 = workstream PROPOSAL §4 + package-split §4.1（P2 依赖 P1 建立的统一会话结束处理器与常驻模型守卫语义）。

**架构+契约面锚**：FROZEN PROPOSAL v1.1（R2 GREEN）packet-bound landed，reviewedCommit `87dbf775`，合并本地 main `7e4c44d8`；`docs/architecture.md §12` 已出现 `261115-arch-review-session-channel-resident` 行。

**拆包 commit**：`214e7a2e`（dev/pool-channel；base ref = `base/261115-session-channel-resident`）。

**上游已知红基线（producer 记锚，五要素）**：

- **server 面**：`457 passed, 9 deselected, 1 warning, 33 subtests passed`（0 failed）。
  - 命令：`python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-channel/server/acceptance"`（仓根起）
  - 日期：2026-11-16 ｜ 量测 HEAD：`214e7a2e` ｜ 面域：`server`
- **host 面（firmware）**：`301 Tests 3 Failures 0 Ignored`——3 红全为既有 `p1_auxbar_style.acceptance.c` 存量红（`ic_wifi_14` / `ic_thermo_12` / `ic_batt_15`），与本 workstream 两包无关。
  - 命令：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` → `cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`（仓根起）
  - 日期：2026-11-16 ｜ 量测 HEAD：`214e7a2e` ｜ 面域：`firmware`
- **consumer 差异判停**：Pod intake 拿到本锚后先跑 `git diff --quiet 214e7a2e HEAD -- server` / `-- firmware`；零差异 → 引用本行（SHA + 命令 + 落盘锚 verbatim），有差异 → 重跑该面。

**环境事实**：新工作树 `C:/AI/KidPal-pod-channel` 已完成固件环境等价化——`firmware/main/test/host_app/sdkconfig` 与两份 `dependencies.lock` 自 pod-m2 树复制，`firmware/managed_components` 与 `firmware/main/test/host_app/managed_components` 建为指向 pod-m2 树的 junction（共享组件缓存）；新树 `build/` 首建完成后热缓存随树保留。专属图索引 `C-AI-KidPal-pod-channel` 已建（full，22,339 nodes / 66,622 edges）。

**门结果**：`portfolio-reconcile --check-launch 261116-channel-resident-core --base-ref base/261115-session-channel-resident` = `ok:true / blocking:[] / warn:[]`；`settings-lint` = 0 RED / 0 WARN；`taskpackage-required` 仅缺 planner Step 7 自产的 `input-brief.md`（预期）。

**遗留 human 项**：无（headless 拉起授权 = human 2026-11-16 明确选择「授权无审批 headless」）。