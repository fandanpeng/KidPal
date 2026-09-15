# docs-delta — TaskPackage 260831-ui-art-assets（subsystem: tools，testHost: host+SDL）

> 读者是人（supervisor / human / 后续 wrap-up）。本文记录「shipped behavior 与 canonical docs 之间已同步 / 已确认无漂移」的部分，
> 以及「有漂移但 integrator 不直改、路由 planner / 主线程」的建议。
> 边界注：integrator scope-map 写允许面本包不含 `docs/notes/**`（TP-B 与 TP-C 同——`docs/notes/` 归 main-thread / dev-loop wrap-up）。
> 故 feature-inventory 与实现策略等 docs/notes 的状态翻转只能「建议 + 留给后续 wrap-up sync」，不能由 integrator 直改。
> `docs/ARCH-REVIEW-QUEUE.md` 在 integrator 写允许面内——本包对 ARQ-260831-i1-indexed-asset-on-device 的「TP-B B8 判定」触发条件
> 已消解（未超预算、不触发 I1），仅在该条「复议触发」列补记，不动其余行、不新增 ARQ。
> `docs/contracts/**` 零触（冻结，本包零契约面，契约门未触发）。

## 1. 变更摘要

把 V2 设计稿里此前未验的美术元素全部经资产管线出图并机判验收：吉祥物其余 6 姿态、手绘框族 9 宫格、元素倾斜烘资产、
三档网点平铺、全图标族、站酷快乐体授权+栅格、Baloo 2 动态时间 imgfont，并核算资产总量对 SPIFFS 分区预算。
47 枚资产全量 RGB565 出图，总量 599,698 字节对 8M 分区 margin 54.1%，不触发 I1。产出 V3 设计稿（实现导向）可行性输入。

本包落地的主要产物（实体落 `changes/dev/260831-ui-art-assets/` + `tools/`）：

- 《UI 资产可行性报告》+《资产总量预算报告》（E4 出口）+ 13 张阈值化出样截图。
- 47 枚 RGB565 资产（`assets.c/h` + `png/*`）+ 3 框 9 宫格切片（`9slice/`，30 文件）。
- 字体与授权落档（站酷快乐体 + Baloo 2 的 TTF + OFL 1.1 原文）。
- 30 项机判脚本 `verify_art_assets.mjs`（30/30 绿）。

## 2. 已改（本包落地，integrator 直接执行）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| `docs/ARCH-REVIEW-QUEUE.md` 行 42（ARQ-260831-i1-indexed-asset-on-device） | 「复议触发」列 | 补记「TP-B B8 已核：未超预算、不触发 I1」 | A9.3 判定未超预算，原「TP-B B8 判定必须上 I1 时」这一升级触发条件已消解；其余行/列不动 |
| `changes/dev/260831-ui-art-assets/*` | 新增 | `UI-art-feasibility.md` + `budget.md` + `snapshots/`（13）+ `assets-manifest.md` + `docs-delta.md` + `review-packet.md` | 本包 closeout/audit 制品（blocking 必交 + E4 出口） |

## 3. 建议 planner / 主线程后续 wrap-up sync（docs/notes/** scope 外，integrator 未直改）

### 3.1 `docs/notes/feature-inventory.md`

**§1.3 美术元素（本包全部 B1..B8 已验，🟡→✅）**，精确翻转为：

| 功能点 | 现 | 改为 |
|---|---|---|
| 吉祥物七姿态 | 🟡 只验 pose-daily 一枚 | ✅ 7 姿态全量出图（96×96 同族）；口径变更：`pose_daily` 98×98(-1°) → 96×96(0°)，倾斜回归运行时/页面层 |
| 手绘不规则圆角框族 | 🟡 未出第一张框资产 | ✅ 3 框型 9 宫格出图 + 拼回 diff=0；**一张 9 宫格忠实性红线交 human 裁定**（见 review-packet，`.frm` 255px 水平大半径 vs 24px 角片协议） |
| 元素倾斜 ±1–2° | 🟡 只烘大时间字块 | ✅ 贴纸微倾 + 印章 -8° 全烘入（OQ-1a） |
| 三档网点 d50/d25/d12 | 🟡 未验 | ✅ 4×4 精确贴图 + Bayer 抖动，占比 56.3%/25.0%/12.5%，平铺周期成立 |
| 成就印章 -8° | 🟡 未验 | ✅ 双圈外实内虚结构保全；-8° 台阶锯齿对「盖章」语义可接受，内圈 2.5px 虚线脆弱点单列交 human 目测 |
| 图标家族 14 枚 | 🟡 只出 2 枚 | ✅ 14 枚全出；**取舍落定 = 离线位图资产**（非 FontAwesome/imgfont 图标字） |

**§1.5 资产管线与双端一致性**：

| 功能点 | 现 | 改为 |
|---|---|---|
| 资产总量 vs SPIFFS 分区预算 | 🟡 总预算未核算 | ✅ 已核：47 枚 599,698 字节 + 预估 P2–P4 3,252,096 = 3,851,794 字节，对 8M margin 54.1%，**不触发 I1** |
| 资产格式 RGB565 直出 | ✅（8 枚） | 维持 ✅，补「47 枚全量 RGB565 全覆盖」 |

**§1.6 三句话小结**：第 1 条「其余 6 姿态、框族、网点、全图标族、印章、动态时间字体全都还没出图」已过时，应改为
「已由 260831-ui-art-assets 全量出图验收」；第 2/3 条结论（无 🔴 项、最高风险 I1 空间回收 vs 16px CJK 真机可读性）不变。

### 3.2 `docs/notes/ui-implementation-strategy.md`

| 位置 | 漂移点 | 建议 |
|---|---|---|
| §4 line 73 | 「实测 P1 全套 8 资产约 96KB」 | 补全量口径：47 资产 599,698 字节（588 KiB）+ P2–P4 预估，总预算对分区的核算见 260831-ui-art-assets/budget.md |
| §2.2 / §2.3 | 美术元素资产化策略本身无需改（本包即执行该策略） | 可补一句「资产化已全量验收入库（260831-ui-art-assets）」，非必须 |

### 3.3 `docs/notes/ui-to-lvgl-pitfalls.md`

| 位置 | 漂移点 | 建议 |
|---|---|---|
| line 171 checkbox | `[ ] 网点纹理（d50/d25/d12）在 LVGL 中如何用 canvas/图片/自定义绘制模拟` | 勾选 ✅：已定案 = 4×4 平铺贴图资产（RGB565），超采样 + Bayer 抖动出图，不做 canvas/自定义绘制 |

### 3.4 后续 sync 清单（留根项，本包已识别、未落地，交 V3 / 后续包）

1. **B2 九宫格忠实性红线**（`.frm` 255px 水平大半径 vs 24px 角片 → 大半径角被裁成紧弯）：需 human 目测 `snapshots/frm_*_stretch_*.png`
   裁定「24px 角片协议」可接受，否则改大框 40px 角片档 / 固定尺寸整烘。→ 落 V3 设计稿决策。
2. **B8 笔顺字盘打包策略**（P4 唯一逼近预算项，保守整帧预估 ~3.1MB）：V3 前须锁定「笔形 3 态叠加复用 + 米字格单份背景」。→ 落 V3 设计稿决策。
3. **-8° 印章内圈 2.5px 虚线**（全场最细笔画）：短划断口/糊连脆弱点，human 目测；备选 3 条（增粗 3.5px / 降角 -5° / 运行时两圈分离叠加）。
4. **ZCOOL 真体宿主依赖**：真体机判 REAL 依赖 per-user 装字体，真机重烘以 ZCOOL/Baloo 子集为准；未装 ZCOOL 的机器会打 SUBSTITUTE。
5. **ARQ-260831-small-cjk-1bpp-readability**：30px CJK 1bpp 可读性落在该 ARQ 风险区，本包只出样不锁下限，仍待真机字号下限实测。

## 4. 无漂移确认

- `docs/contracts/interface-contract.md` 本包零写（契约门未触发，设计稿母版 `docs/ui/kidpal-ui-design-v2.html` 只读抽取，未改设计稿）。
- 无新增 `docs/` 根文件、无影子目录。
- 报告/证据落 `changes/dev/260831-ui-art-assets/` + `tools/`（资产已在 build/tools 面，非 docs live 面），不散写。
- 本包 `docsSyncRequired: false`——文档产物 = changes/dev 可行性报告 + 预算报告（E4 出口）；docs/notes 状态翻转归本 waiver 的后续 wrap-up sync。