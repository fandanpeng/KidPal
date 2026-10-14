# Review Packet — TaskPackage 260831-simulator-lvgl-align

> supervisor / human 合并门前审阅件。承重判断以 verifier 复验结论为准；结构闸 PASS/FAIL 不复述为 finding。

## Change Size

- Files changed (tracked, 已绿):
  - `tools/asset-pipeline/gen_assets.mjs`（阈值单一源 127）
  - `tools/simulator/LvglWindowsSimulator/LvglWindowsSimulator.vcxproj` + `.vcxproj.filters`（9.3 LVGL 文件清单重生成）
  - `tools/simulator/LvglWindowsSimulator/assets/assets.h`（RGB565 标注）
  - `tools/simulator/LvglWindowsSimulator/kidpal_ui.c`（实验快照清零 → 调正式工具）
- Files added (untracked → 本包新增):
  - `tools/simulator/LvglWindowsSimulator/kidpal_snapshot.c`（122 行）+ `kidpal_snapshot.h`（31 行）
  - `tools/simulator/regenerate-vcxproj-lvgl.mjs`（154 行）
  - `tools/simulator/tests/verify_p1_regression.mjs`（111 行）+ `test_lvgl_align.acceptance.md`（冻结验收，298 行）
  - `changes/dev/260831-simulator-lvgl-align/p1_thresholded.png`（9.3 阈值化快照，验收图）
- Net diff (tracked): 5 files, 10908 insertions / 14751 deletions（vcxproj churn 为主，因 9.5→9.3 LVGL 文件清单整体重写）
- Scope: 单包、单子系统（tools/simulator + tools/asset-pipeline），零跨系统、零契约面、零新依赖。
- Split recommendation: 无需拆分。vcxproj 大 diff 是机械重生成的清单替换，非手写逻辑。

## Verification Evidence

- Structural gates（closeout 承重闸）:
  - `node .claude/scripts/taskpackage-required-check.mjs .claude/taskpackages/260831-simulator-lvgl-align/taskpackage.json`
    → **FAIL（BLOCKING，见下）**：`missing: ["contractRefs"]`。taskpackage.json 的 `contractRefs` 为空数组，
    而该闸要求非空。仓库 14/14 已提交包均用 `["docs/contracts/interface-contract.md"]`。此为 planner 侧字段缺口，
    **本项目前未达 closeout 完整性，须 planner 补齐后才能过 supervisor gate**。integrator 未自行改写
    planner-finalized 字段（见 §Residual Risk / 最终交付报告）。
  - `closeout-check.mjs`：跳过并注明——该脚本引用的 `delivery-status.mjs` 尚缺（已登记 ARQ，见本包源仓治理档）。
- Acceptance/unit/api/e2e（verifier 复验结论，planner 已跑、采信为终验）:
  - A2.1 版本对齐：`diff(sim lv_version.h, fw lv_version.h)` 空 → 9.3.0 == 9.3.0。
  - B8 阈值单一源：`gen_assets.mjs` `const LUM_THRESHOLD = 127` + 注释四处同源；`< 128` 残留 0。
  - A6.1 残渣：`kidpal_ui.c` 内 `C:/tmp/*` 直写、`lodepng_encode32` 命中 0。
  - A5.1 格式：`assets.c` 内 `.cf = LV_COLOR_FORMAT_RGB565` ×8，I1/L8 ×0（integrator 复证）。
  - A4.1/A5.2/A5.3：`node tools/simulator/tests/verify_p1_regression.mjs changes/dev/260831-simulator-lvgl-align/p1_thresholded.png`
    → `ALL CHECKS PASSED`（快照严格两级 {0,255}、8 资产全在场、black-ratio 差最大 2.79% ≤ 5%）。
  - A2.2 构建：MSBuild x64 Debug `/t:Rebuild` 退出码 0，exe 产物就位（builder 报告，本机工具链完整）。
  - A6.3 挂接：`.vcxproj` 显式 `<ClCompile>` 含 `kidpal_ui.c` / `kidpal_snapshot.c` / `assets/assets.c`（integrator 复证）。
- Security gate: 未声明（`securityReviewRequired` 未设），N/A。
- Mutation check: 未声明。本包为配置/版本对齐类工作，SLA-A1/E1 已用 `kind: structural` 对象标记豁免；
  可断言条目（A2/A3/A4/A5/A6）均有机械/运行时断言承接。
- Docs sync: 契约面零触、无需改；`docs/notes/**` 四处 drift 已识别并路由 main-thread（见 docs-delta §4.1）。

## Simplification Debt

自本次改动中 grep `minimal:` 标记 → 无（零命中，`no-trigger`）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 资产数据生成一致性 | 已提交的 `assets.c` / `png/*.png` 是 9.5 时代产物（阈值 128 生成）；`gen_assets.mjs` 脚本已把阈值单一源统一到 127，但资产数据未重跑，存在 127/128 一档边界像素错位风险。当前 A5.2 机判在 5% 容差内通过（black-ratio 差最大 2.79%）。重跑会 ~15k 行 churn，且 `time_10_24` 文本栅格对字体环境（Segoe UI / Arial Rounded MT Bold）敏感。 | `gen_assets.mjs:21` 阈值=127（脚本已改）vs `assets.c`/`png` 二进制仍为旧阈值；A5.2 通过。 | defer 到 TP-B 统一重生成；单列 residual risk 交 human 裁（本包内重跑 vs TP-B 统一）。 |
| NIT | judgement-call | 验收 harness 精度 | 验收 §A5.1 grep 目标指 `assets.h`，真实 `.cf` 在 `assets.c`。builder 在 `assets.h` 每个 extern 加 `/* LV_COLOR_FORMAT_RGB565 */` 注释使 grep 命中；真值已复核 = `assets.c` 内 `.cf RGB565` ×8、I1/L8 ×0。 | 验收文件 §A5.1 grep 目标 `assets.h`（写 extern+注释，非 `.cf` 定义）；assets.c 是真实 `lv_image_dsc_t` 定义。 | 不阻塞；后续修订验收把 A5.1 grep 目标改为 `assets.c`（定义面），防「改 c 但标注意外残留」漏检。 |
| NIT | judgement-call | 快照退出语义 + 注释残留 | 原实验快照 `PostQuitMessage(0)` 在 `while(1){lv_timer_handler;lv_delay_ms}` 主循环下不泵 Win32 消息、不会退出；已改 `exit(0)` 保证「截完即退」脚本化不挂起，语义不变。`kidpal_snapshot.h:20` 注释仍写「截完即退(PostQuitMessage)」，与实现 `exit(0)` 漂移。 | `kidpal_snapshot.c:105-108` 注释 + `exit(0)` 实现；`kidpal_snapshot.h:20` 注释残留 `PostQuitMessage`。 | FYI 记录语义变更；NIT 建议 main-thread 更正 `kidpal_snapshot.h:20` 注释（tools/ 不在 integrator 写权）。 |
| SHOULD | judgement-call | docs-sync（live-state drift） | 模拟器 9.5.0→9.3.0 后，`docs/notes/**` 三处 + `kidpal_ui.c` 一处头注释仍是 9.5 时代表述（详见 docs-delta §4.1）。docs/notes/** 归 main-thread，integrator 不直改。 | 行为变更 9.5→9.3 vs `ui-to-lvgl-pitfalls.md:5,:11`、`ui-implementation-strategy.md:62`、`feature-inventory.md:64`、`kidpal_ui.c:6` 的 stale 表述。 | 路由 main-thread 更正（清单见 docs-delta §4.1）；本包不阻塞。 |

## Five-Axis Review

1. Correctness: 版本对齐逐字节 9.3.0（A2.1）、lv_conf 以固件 sdkconfig 为基准（ThorVG 关 / 阈值 127 / FreeType 栈 ≥32KB，A3）、
   8 资产 RGB565 渲染无回归（A5）。快照工具阈值与真机 flush 后处理同源（`LV_DRAW_SW_I1_LUM_THRESHOLD`）。满足所有声明的 `SLA-Ax`。
2. Test quality: 断言全部机械/可目测（版本 grep、conf 键值、2 级 PNG 唯一值集合、逐资产黑白占比、错误边界 B1–B8），
   harness 走 host+SDL 真实软件渲染非 mock。质量良好。一处精度缺口：A5.1 grep 目标错位（finding ②）。
3. Contract/boundary safety: 零契约面（`docs/contracts/**` 零触、契约门未触发）；vendor `ESP32-S3-RLCD-4.2-Demo/**` 零触；
   9.3 对齐本身消除「模拟器↔固件版本漂移」这一类第三类失真风险。安全。
4. Operability/security: 构建可复现性依赖 gitignored-LVGL 的两步手工恢复（docs-delta §2 已写死步骤）；
   快照路径经 `KIDPAL_SNAPSHOT_PATH` 环境变量覆盖（无硬编码 `C:/tmp`）。无 secret / 网络 / 权限面。安全。
5. Maintainability/simplicity: 阈值单一源消除魔数漂移；`regenerate-vcxproj-lvgl.mjs` 幂等、mirror 上游 C# 语义；
   快照逻辑隔离在 `kidpal_snapshot.c`（`kidpal_ui.c` 只调入口）。无过早抽象、无重复实现、无 `minimal:` 债务。干净。

## Residual Risk

- decisionCoverage `dropped` / `risk-accepted` 条目：无（3 条决策均 `landsIn: acceptance:...`，无 dropped / risk-accepted）。
- S2 exploratory scan：TaskPackage 未声明 `s2ExploratoryRequired`，未执行。
- Closeout 阻断项（非代码缺陷、须 planner 处理）：`taskpackage-required-check` 红于 `contractRefs` 空数组
  （§Verification Evidence → Structural gates）。补 `["docs/contracts/interface-contract.md"]`（仓库惯例）后重跑即绿。
  sibling 包 `260831-ui-art-assets` 有同型空 `contractRefs`，提示本批 planner 侧统一缺口。
- Known limitation（finding ①）：资产数据仍是旧阈值产物，与 127 脚本阈值有一档理论错位，A5.2 在 5% 容差内通过。
- Follow-up recommended：TP-B 统一重生成 8 资产数据（脚本 127 与数据一致）；main-thread 更正 docs/notes/** 4 处 drift。
- Human decision needed：finding ① 是「本包内重跑资产数据」还是「defer TP-B 统一重生成」——重跑代价 ~15k 行 churn +
  字体环境敏感，建议 defer，交 human 裁。