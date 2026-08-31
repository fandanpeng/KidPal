# 资产清单 — TaskPackage 260831-ui-art-assets

> 本包资产产物按 E4 exit criteria 落位；为避免在 `changes/dev/` 重复拷贝数百 KB 二进制，
> 本文档以清单形式指向真实产物位置（`tools/simulator/LvglWindowsSimulator/assets/` 与 `tools/asset-pipeline/`），
> 报告与阈值化验收截图已实体迁至本目录（`UI-art-feasibility.md`、`budget.md`、`snapshots/`）。

## 1. 全量生成资产（真实位置，不入本目录）

| 类别 | 路径 | 数量/说明 |
|---|---|---|
| RGB565 描述符与数据 | `tools/simulator/LvglWindowsSimulator/assets/assets.c` | 47 枚资产 `lv_image_dsc_t` 描述符 + 像素数据 |
| 描述符头 | `tools/simulator/LvglWindowsSimulator/assets/assets.h` | 47 条 `extern lv_image_dsc_t` 声明 |
| 单枚位图 | `tools/simulator/LvglWindowsSimulator/assets/png/*.png` | 47 枚（7 姿态 + 14 图标 + 3 整框 + 3 网点 + 印章 + 站酷样字 + 11 imgfont + P1 既有 7） |
| 框族 9 宫格切片 | `tools/simulator/LvglWindowsSimulator/assets/9slice/` | 3 框 × 9 片 PNG + 3 份元数据 JSON = 30 文件 |

## 2. 字体与授权（真实位置）

| 文件 | 说明 |
|---|---|
| `tools/asset-pipeline/fonts/ZCOOLKuaiLe-Regular.ttf` | 站酷快乐体真体（per-user 安装后按名解析） |
| `tools/asset-pipeline/fonts/Baloo2-800.ttf` | Baloo 2 ExtraBold 静态实例（wght 800） |
| `tools/asset-pipeline/fonts/Baloo2[wght].ttf` | Baloo 2 variable |
| `tools/asset-pipeline/fonts/OFL-ZCOOLKuaiLe.txt` | 站酷快乐体 SIL OFL 1.1 授权原文 |
| `tools/asset-pipeline/fonts/OFL-Baloo2.txt` | Baloo 2 SIL OFL 1.1 授权原文 |

## 3. 管线与机判脚本（真实位置）

| 文件 | 说明 |
|---|---|
| `tools/asset-pipeline/gen_assets.mjs` | 资产管线（SVG/CSS 唯一母版 → 抽取 → 栅格化 → 阈值化 → RGB565） |
| `tools/asset-pipeline/verify_art_assets.mjs` | 30 项机判脚本（已跑 30/30 绿） |
| `tools/asset-pipeline/make_snapshots.mjs` | 阈值化拼版出样生成器 |
| `tools/asset-pipeline/install-fonts.ps1` | per-user 字体安装（ZCOOL） |
| `tools/asset-pipeline/tests/test_art_assets.acceptance.md` | 冻结验收（builder/fixer 只读） |

## 4. 本目录实体产物

- `UI-art-feasibility.md` — UI 资产可行性报告（B1–B8 逐元素结论 + -8° 印章单列 + 图像字取舍 + 字体使用记录）
- `budget.md` — 资产总量预算报告（599,698 字节 vs 8M，margin 54.1%，不触发 I1）
- `snapshots/` — 13 张阈值化出样截图（均满足唯一值集合 ⊆ {0,255}）