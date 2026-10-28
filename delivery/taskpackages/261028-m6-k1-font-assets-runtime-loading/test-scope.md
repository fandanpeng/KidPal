# test-scope 清单 · 261028-m6-k1-font-assets-runtime-loading（integrator 范围圈定终验）

- 日期：2026-10-28；圈定者 = integrator；base ref = `9f8b2170`（包首提交父，包级 diff 恒用此 ref）。
- 圈定输入：`git diff 9f8b2170`（含工作树未提交态）+ detect_changes（项目 `C-AI-KidPal-pod-m2`，base `9f8b2170`，inbound depth 2：changed_files 17、seed_symbols 63、impacted_total 14，全部收敛在 `firmware/main` 模块，无 server 面、无清单外符号外溢）+ 本包 acceptanceTestsFile 种子。
- 三分法申报：per-Pod 专属图索引可用（`C-AI-KidPal-pod-m2` 在册）→ **正常圈定**，非降级。

## 圈定结果（影响面清单）

| 层 | 范围 | 依据 |
|---|---|---|
| 本包冻结验收件（floor） | `firmware/main/test/font_assets_runtime.acceptance.c`（FAS-A1..A8） | taskpackage acceptanceTestsFile |
| 相邻冻结锚 canary（floor） | `firmware/main/test/p1_cjk_name.acceptance.c` | FAS-A8 锚；detect_changes impacted 含 kidpal_render_node 符号 |
| 宿主守卫（floor） | `firmware/main/test/host_infra_smoke.acceptance.c`（包 0 守卫，HOST_ACCEPTANCE_SRCS 末位最先执行） | host CMakeLists 接线 |
| 验收接线面 | `firmware/main/test/host_app/main/CMakeLists.txt`（HOST_ACCEPTANCE_SRCS + FAS 结构清单块） | 清单内文件，验收面 = 冻结件 + host CMakeLists |
| blast radius 映射 | detect_changes impacted 14 符号全部在 `firmware/main`（Application 生命周期 / CustomBoard / render_node 三函数 / mcp_server 两处）——均为 Assets::Apply 与 render_node 模块级关联，非新增行为依赖 | detect_changes advisory |
| 安全余量 | host 全量套件 250 Tests 全跑（非只跑本包档），设备 build 真编译（firmware-build 闸） | verifier 第 4 轮实测 |

## 终验口径与执行记录

- 范围执行 = **host 全量 250 Tests / 3 Failures（仅存量 p1_auxbar AB-A1/A2/A4，零新增红）** + 设备 build 绿（xiaozhi.bin 0x324950，app 分区余 0x4b6b0 = 9%）——verifier 第 4 轮（2026-10-28T14:45:50Z，8 闸全绿，`current/verdict.json` + `verify-runs.jsonl` 第 4 行）。
- floor 说明：本包无对端包冲突消解（crossEnd 恒空、dependsOn 空），「双方冻结件 floor」即本包冻结件 + 相邻锚 p1_cjk_name——两者均已含于 host 全量安全余量内。
- 本清单路径一行记入 review-packet.md（留根锚：圈定 vs 事后全 blast radius 差集对照）。
