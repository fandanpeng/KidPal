# docs-delta — TaskPackage 260831-simulator-lvgl-align（subsystem: tools，testHost: host+SDL）

> 读者是人（supervisor / human）。本文记录「shipped behavior 与 canonical docs 之间已同步 / 已确认无漂移」的部分，
> 以及「有漂移但 integrator 不直改、路由 main-thread」的建议。承重判断以 verifier 复验结论为准，本文不复述 gate 输出。

## 1. 变更摘要

把 LVGL Windows PC 模拟器从 9.5.0 降级/锁定到与固件同版 9.3.0，`lv_conf.h` 以固件 `sdkconfig` 为基准对齐
（ThorVG 关、I1 亮度阈值 127、FreeType 栈 ≥32KB），把 1bpp 阈值化快照自查正式化（`kidpal_snapshot.c/.h`），
并让 P1 的 8 资产在 9.3 下回归（阈值化 PNG 机判）。目标是让「模拟器验证结论 ≈ 真机结论」重新成立。

本包落地的主要产物：

- 阈值单一源 `LUM_THRESHOLD = 127`（`tools/asset-pipeline/gen_assets.mjs`），与固件 `sdkconfig`、
  模拟器 `lv_conf.h`、快照工具 `KIDPAL_SNAPSHOT_LUM_THRESHOLD` 三处同源，`< 128` 残留清零（B8 判绿）。
- 正式快照工具 `tools/simulator/LvglWindowsSimulator/kidpal_snapshot.c/.h`：整屏快照 → RGB565 亮度阈值化 →
  逐像素 2 级 PNG 落盘 → `exit(0)` 截完即退。
- 回归 harness `tools/simulator/tests/verify_p1_regression.mjs`：A4.1（严格两级 {0,255}）+ A5.2（黑白占比容差 5%）+ A5.3（8 资产在场）。
- 工程文件同步工具 `tools/simulator/regenerate-vcxproj-lvgl.mjs`：重生成 `.vcxproj(.filters)` 里的 LVGL 文件清单。

## 2. 版本对齐 + gitignored-LVGL 可复现步骤（关键，务必保留）

**LVGL 组件实树不入库**：`tools/.gitignore` 第 2 行 `simulator/LvglPlatform/` 把整个 LVGL 组件树排除出 git。
9.3 降级是本地整树替换（`cp -r firmware/managed_components/lvgl__lvgl → tools/simulator/LvglPlatform/lvgl`），
**这个替换本身不进 git**。`.vcxproj(.filters)` 里引用的 9.3 文件清单（已用 `regenerate-vcxproj-lvgl.mjs`
重新生成并入库）指向的实树文件并不存在于仓库。任何人干净 checkout 后、构建模拟器前，**必须执行以下两步**，
否则 vcxproj 引用 9.3 文件、实树却是 9.5 / 空：

1. 把固件 9.3 组件整树拷到模拟器平台目录：
   ```bash
   cp -r firmware/managed_components/lvgl__lvgl tools/simulator/LvglPlatform/lvgl
   # Windows 等价：xcopy /E /I firmware\managed_components\lvgl__lvgl tools\simulator\LvglPlatform\lvgl
   ```

2. 同步 vcxproj 的 LVGL 文件清单（幂等，可重复执行）：
   ```bash
   node tools/simulator/regenerate-vcxproj-lvgl.mjs
   ```

版本对齐的机械验证（验收 A2.1）：
```bash
diff \
  <(grep -E 'LVGL_VERSION_(MAJOR|MINOR|PATCH)' tools/simulator/LvglPlatform/lvgl/lv_version.h) \
  <(grep -E 'LVGL_VERSION_(MAJOR|MINOR|PATCH)' firmware/managed_components/lvgl__lvgl/lv_version.h)
```
→ 输出为空（二者均 MAJOR 9 / MINOR 3 / PATCH 0）。

## 3. 关键改动清单

| 文件（tracked） | 改动 | 承载验收 |
|---|---|---|
| `tools/asset-pipeline/gen_assets.mjs` | 阈值统一 `LUM_THRESHOLD = 127`（原 `< 128`）+ 三处同源注释，头注释标 owner/「生成勿手改」 | A3.2 / A6.2 / B8 |
| `tools/simulator/LvglWindowsSimulator/kidpal_snapshot.c/.h` | 新增，正式快照工具（RGB565 阈值化 → 2 级 PNG → exit(0)，路径经 `KIDPAL_SNAPSHOT_PATH` 覆盖） | A4.1 / A4.2 / A4.3 |
| `tools/simulator/LvglWindowsSimulator/kidpal_ui.c` | 删实验快照（`C:/tmp/*`、`lodepng_encode*`、`debug_mascot`、`snapshot_timer`），改调 `kidpal_snapshot_schedule()` | A6.1 / B7 |
| `tools/simulator/LvglWindowsSimulator/assets/assets.h` | 每个 `extern lv_image_dsc_t` 带 `/* LV_COLOR_FORMAT_RGB565 */` 标注（真实 `.cf` 在 assets.c） | A5.1 |
| `LvglWindowsSimulator.vcxproj(.filters)` | 用 `regenerate-vcxproj-lvgl.mjs` 重生成 9.3 文件清单；显式挂接 `kidpal_ui.c` / `kidpal_snapshot.c` / `assets/assets.c` | A2.2 / A6.3 |
| `tools/simulator/regenerate-vcxproj-lvgl.mjs` | 新增，同步 vcxproj 清单（mirror 上游 C# LvglProjectFileUpdater 语义） | A2.2 |
| `tools/simulator/tests/verify_p1_regression.mjs` | 新增，A4.1/A5.2/A5.3 机判 harness | A4.1 / A5.2 / A5.3 |
| `changes/dev/260831-simulator-lvgl-align/p1_thresholded.png` | 9.3 下 P1 全屏阈值化快照（验收图，A4.4） | A4.4 / A5.3 |

## 4. docs-sync 判定

行为变化 = 模拟器 LVGL 版本 9.5.0 → 9.3.0（+ 阈值统一 127 + 快照工具正式化）。

### 4.1 有漂移、路由 main-thread（integrator 无 docs/notes/** 写权，不直改）

| 文档 | 漂移点 | 建议 |
|---|---|---|
| `docs/notes/ui-to-lvgl-pitfalls.md` | header（line 5）与 §1.1（line 11）写「LVGL v9.5.0」 | 补 9.3 对齐注记：模拟器现已锁定 9.3.0，与固件同版。 |
| `docs/notes/ui-implementation-strategy.md` | §4 line 62 live-state 声明「当前模拟器是 9.5.0」 | 改为 9.3.0（决策本身 §3 无需改——本包就是执行该决策）。 |
| `docs/notes/feature-inventory.md` | §1.5 line 64 状态行「模拟器 LVGL 9.5 vs 固件 9.3 版本对齐 🟡」 | 本包闭合这条，合并后把 🟡 改 ✅。 |
| `tools/simulator/LvglWindowsSimulator/kidpal_ui.c` 头注释 line 6 | 写「预栅格化 1bpp 资产」，与同块 line 8「RGB565 位图」自相矛盾 | 更正为 RGB565（tools/ 不在 integrator 写权，路由 main-thread / builder）。 |

### 4.2 无需更新

- `docs/notes/ui-implementation-strategy.md` §3：是决策来源（降级 9.3 的前瞻决策），本包执行之，不改决策文字。
- `docs/contracts/interface-contract.md`：本包零契约面（LVGL 版本/阈值/快照对齐，不触 E1 / B1 / D3 / shim API）。
  契约门未触发，无契约变更。