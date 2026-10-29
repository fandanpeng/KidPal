# test-scope 清单 · 261028-m6-k2a-p1-home-16px（integrator 范围圈定终验）

- 日期：2026-10-29；圈定者 = integrator；base ref = `08cc46c4`（包首提交父，包级 diff 恒用此 ref + 工作树未提交态）。
- 圈定输入：`git diff 08cc46c4` + detect_changes（项目 `C-AI-KidPal-pod-m2`，base `08cc46c4`，inbound depth 2：changed_files 21、seed_symbols 130、impacted_total 7，全部收敛在 `firmware/main` 模块，无 server 面、无清单外符号外溢）+ 本包 impactedTestsSeed（taskpackage 三条：p1_home_battery / p1_cjk_name frozen-rebaseline + schedule_grid regression）。
- 三分法申报：per-Pod 专属图索引可用（`C-AI-KidPal-pod-m2` 在册，launch 面必办项）→ **正常圈定**，非降级。

## 圈定结果（影响面清单）

| 层 | 范围 | 依据 |
|---|---|---|
| 本包冻结验收件（floor） | `firmware/main/test/p1_home_16px.acceptance.c`（P16-A1..A3） | taskpackage acceptanceTestsFile |
| 相邻冻结锚 canary（floor） | `firmware/main/test/p1_cjk_name.acceptance.c`（supersede 重基线后）+ `firmware/main/test/p1_home_battery.acceptance.c`（同） | taskpackage impactedTestsSeed frozen-rebaseline 两条；detect_changes impacted 含 p1_cjk_name acceptance 模块 |
| 回归面（floor） | `firmware/main/test/schedule_grid.acceptance.c`（home_upcoming wire/preview 三态回归） | taskpackage impactedTestsSeed regression 条；wire 不变（F-9 裁定）故只回归不重锚 |
| 宿主守卫（floor） | `firmware/main/test/host_infra_smoke.acceptance.c`（包 0 守卫，HOST_ACCEPTANCE_SRCS 末位最先执行） | host CMakeLists 接线 |
| 验收接线面 | `firmware/main/test/host_app/main/CMakeLists.txt`（HOST_ACCEPTANCE_SRCS + FAS 结构清单七键）+ `host_infra_seam.c`（[S7] host 强定义） | 接线增量（acceptance-wiring-patch.md 授权） |
| blast radius 映射 | detect_changes impacted 7 符号全部在 `firmware/main`：kidpal_home_p1_render_frame（直接改）、font_assets_runtime / p1_cjk_name / p1_home_16px 三 acceptance 模块、CMakeLists、home_p1_screenshot（截图消费面）、happy_flow_capture（archive 回放）——均属渲染帧/验收消费关联，无新增行为依赖 | detect_changes advisory |
| 安全余量 | host 全量套件 253 Tests 全跑（非只跑本包档），设备 build 真编译（firmware-build 闸） | verifier 轮实测（verify-runs.jsonl，2026-10-29T13:11:08Z） |

## 终验口径与执行记录

- 范围执行 = **host 全量 253 Tests / 3 Failures（仅存量 p1_auxbar AB-A1/A2/A4，零新增红；P16 全绿 + P0-chain PCN-A11 转绿）** + 设备 build 绿（xiaozhi.bin 0x324c20 bytes，app 分区 0x370000，余 0x4b3e0 = 9%）——verifier 8 闸全绿（error:null，`current/verdict.json` + `verify-runs.jsonl`）。
- 变异终验口径：taskpackage mutationEvidence 5 条全 turned red（P16-A1 一条、P16-A2 三条、P16-A3 一条），FINAL 干净跑 253/3。
- floor 说明：本包无对端包冲突消解（Pod 分支无 merge/rebase 消解轮），「双方冻结件 floor」即本包冻结件 + 相邻锚 p1_cjk_name / p1_home_battery——均含于 host 全量安全余量内。
- 本清单路径一行记入 review-packet.md（留根锚：圈定 vs 事后全 blast radius 差集对照）。
