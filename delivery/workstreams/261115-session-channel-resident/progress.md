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

## 2026-11-16 · P1 `261116-channel-resident-core` 收口（integrator）

**收口状态**：P1 离线面全部转绿、待 supervisor 复核 + 前置升级闸 + merge-driver 本地合并；真机轮 A 未发生，故 workstream 效果段与两条 debt 结项均**未闭合**。

- **final 终验**：round 2（`verifier-evidence/rounds/r2.json`，`ok:true / observedGateExit:0`）十闸全绿（含 `VERIFY_FW_BUILD_REQUIRED=1` 真编译）；`verifier-evidence-check` exit 0 / `factOk:true`。scoped r1 先绿，final 按 scope 圈定（`test-scope-final.txt`，25 条）跑。
- **验收实跑**：host `306 Tests / 3 Failures / 0 Ignored`（3 红全为上游 `p1_auxbar_style` AB-A1/A2/A4，本包零新增红）；server `473 passed / 0 failed / 9 deselected`。scr 15 条 acceptance（SCR-A1..A7 + SCRF-B1..B6 含 B2b/B4b）全绿，15 条 per-acceptance 变异转红留痕。
- **证据指针**：`delivery/taskpackages/261116-channel-resident-core/{review-packet.md, docs-delta.md, verification.json, verdict.json, test-scope-final.txt, verifier-evidence/rounds/r2.json}`。
- **debt 处置**：`TECH-261114`（候选 A = T1）与 `TECH-261012`（心跳保活 = T5/T6）实现已 land、离线绿，但按 PROPOSAL §6「结项凭据必须真机」status 维持 `deferred`、仅刷新 trigger + 追加收口注记；真机后由主线程结项并 `debt-render.mjs` 归位。
- **跨包交接（给 P2 `261117-session-end-intent-longretry`）**：SCR-A2 in-flight 守卫重锚、`mcp_server.cc` 注释双源、退避变事实长跑形态基线、end_session 三段补链 / 自动重连封顶——五项见 `verification.json.crossPackageHandoff` 与 `taskpackage.crossPackageBehaviors`。
- **遗留 human 项**：**一轮真机轮 A（A1–A13，human 持板，front 由 agent 部署）**——本包 exitCriteria.KRC-E4 出口；三源日志互证、证据落本包 `current/`；未发生前不宣布 workstream 目的达成、不结项 debt。