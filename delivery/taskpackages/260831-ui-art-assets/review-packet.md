# Review Packet — TaskPackage 260831-ui-art-assets

> integrator 收口。合并门前的审查包。本包为**规划工作流（planning-workstream）验证包**（subsystem: tools，
> testHost: host+SDL），目的不是改产品功能，而是把 V2 设计稿里此前未验的美术元素全部经资产管线出图、机判验收，
> 并核算资产总量对 SPIFFS 分区预算，产出 V3 设计稿（实现导向）的可行性输入。承重判断以 verifier round 3 复验结论
> + builder 30 项机判为准；结构闸 PASS/FAIL 不复述为 finding。

## Change Size

- **Files modified（10）**：
  - `tools/asset-pipeline/gen_assets.mjs`（资产管线唯一母版抽取 + 框族/网点/印章 CSS 父本构造 + 阈值化/RGB565）
  - `tools/simulator/LvglWindowsSimulator/assets/assets.c`（描述符数 8 → 47）
  - `tools/simulator/LvglWindowsSimulator/assets/assets.h`（extern 声明 8 → 47）
  - `assets/png/*`（7 枚 P1 既有资产重烘，其中 `pose_daily_96` 98×98 → 96×96）
- **Files added（约 90，纯新增，零删除既有产品代码）**：
  - `assets/png/*`（39 枚新位图：其余 6 姿态 + 14 图标 48 盒 + 3 整框 + 3 网点 + 印章 + 站酷样字 + 11 imgfont）
  - `assets/9slice/`（30 文件：3 框型 × 9 片 + 3 元数据 JSON）
  - `tools/asset-pipeline/fonts/`（5 文件：站酷/Baloo 2 TTF ×3 + OFL 原文 ×2，合计约 2.6 MB 授权存档）
  - `tools/asset-pipeline/verify_art_assets.mjs`（30 项机判）+ `make_snapshots.mjs`（阈值化拼版）+ `install-fonts.ps1`（per-user ZCOOL 安装）
  - `tools/asset-pipeline/tests/test_art_assets.acceptance.md`（冻结验收，~498 行）
  - `tools/asset-pipeline/report/{UI-art-feasibility.md, budget.md, snapshots/(13)}`
  - `changes/dev/260831-ui-art-assets/*`（本 closeout 制品：报告迁副本 + 13 快照 + manifest + docs-delta + review-packet）
- **Net diff**：约 +2.7 MB（主体为授权 TTF 存档 + 47 枚 RGB565 资产数据），文本 +~1.2k 行（管线 + 机判 + 冻结验收）。
- **Scope**：单子系统（tools，资产管线 + 模拟器资产目录）。零跨系统、零契约面、零新依赖（sharp 为既有 node 依赖）。
- **Split recommendation**：不拆。资产管线 + 出图产物 + 机判 + 冻结验收内聚，1:1 对应。

## Verification Evidence

### 结构闸 + verifier round 3（承重，planner 已跑、采信为终验）

verifier-evidence fact `r3.json`（`260831-ui-art-assets/verifier-evidence/rounds/r3.json`，`observedGateExit: 0`，`ok: true`）逐门：

| gate | 结果 | 说明 |
|---|---|---|
| taskpackage-required | ✅ ok | TaskPackage 结构完整（`contractRefs` 已含 `docs/contracts/interface-contract.md`） |
| firmware-build | ✅ ok（degraded） | `idf.py` 不可用，固件编译本机未核验（降级放行，非假绿；本包无固件改动） |
| contract-lint | ✅ ok（warn） | 仅 1 条 `contract-link-broken`（相对链接目标不存在，非本包引入，advisory） |
| acceptance-coverage | ✅ ok | UAA-A1..A9 全映射冻结验收文件 |
| mutation-check | ✅ ok（skip） | 资产生成包无源码 mutation 语义（机判脚本移除任一资产即红，敏感证据静态声明承载） |
| docs-sync | ✅ ok | `docsSyncRequired: false`（文档产物 = changes/dev 报告 + 预算） |
| security | ✅ ok | `securityReviewRequired: false` |

### 30 项机判（builder 已跑，`verify_art_assets.mjs` → `ALL GREEN 30/30`）

| acceptanceRef | 机判断言（check id） | 结果 |
|---|---|---|
| A2.1 (B1) | 7 姿态 PNG 全在场 + 描述符 ≥7 | ✅ |
| A2.2 (B1) | 7 姿态同族同尺寸（防跳位） | ✅ |
| A2.3 (B1) | 逐枚非全黑/非全白 | ✅ |
| A3.2 (B2) | 3 框型 × 9 片全在场（4 角 + 4 边 + 1 中心） | ✅ |
| A3.3 (B2) | 9 片拼回原始尺寸逐像素 diff = 0 | ✅ |
| A3.4 (B2) | 3 框型 4 角片均 `stretch=false` | ✅ |
| A4.1 (B3) | `rotate` 先于 `flatten` + 印章 128×128 → 145×145（真实旋转）+ 旋转 vs 0° diff | ✅ |
| A4.2 (B3) | 微倾贴纸在场非空白 | ✅ |
| A4.3 (B3) | 印章非空白且黑占比落内容区间 | ✅ |
| A4.4 (B3) | 报告 -8° 锯齿单列结论段在场 | ✅ |
| A5.1 (B4) | 3 枚网点精确 4×4 | ✅ |
| A5.2 (B4) | 密度递增 d50>d25>d12 且非空白 | ✅ |
| A5.3 (B4) | 2×2 平铺 8×8 逐 4×4 单元格一致 | ✅ |
| A6.1 (B5) | 14 图标 PNG 全在场 + 描述符 ≥20 | ✅ |
| A6.2 (B5) | 13 枚 48×48 统一 + `ic_wave` 48×14 特例 | ✅ |
| A7.1 (B6) | OFL 1.1 原文 + TTF 落档 | ✅ |
| A7.2 (B6) | 免费商用条款 + Reserved Font Name 改名禁则逐字引用 | ✅ |
| A7.3 (B6) | 站酷样字非空白（黑占比 >1%） | ✅ |
| A8.1 (B7) | 11 枚 imgfont（0–9 + 冒号）在场 | ✅ |
| A8.2 (B7) | 11 枚等宽箱体逐枚一致 | ✅ |
| A8.3 (B7) | 数字 bottom-y 一致 + 冒号垂直中心对齐 | ✅ |
| A9.1 (B8) | 分区表 `assets` 行在场（8M） | ✅ |
| A9.2 (B8) | 报告 total == `data_size` 求和（599,698） | ✅ |
| A9.3 (B8) | 预算报告含 I1 判定结论行 | ✅ |
| E2 | flatten 先于 greyscale + svgUse 内联 | ✅ |
| E3 | 所有 png 唯一值 ⊆ {0,255} | ✅ |
| E4 | 可行性报告 + 预算报告落位 | ✅ |

integrator 独立复证（非全量复跑，抽查）：47 枚 `png/`、30 文件 `9slice/` 计数一致；`report/snapshots/` 13 张全部
逐像素唯一值 ⊆ {0,255}（阈值化成立，E3 落档口径）。`assets.c` `LV_COLOR_FORMAT_RGB565` 描述符 = 47。

### 人工目测出口（OQ-3a，本包出样、不锁结论，非机判 green）

| 出口 | 承载 | 状态 |
|---|---|---|
| A2.4 造型达标（菱形耳朵/五官/卷尾） | `snapshots/poses_row.png` | 出样，归 human 目测 |
| A3.4 角部不变形 + 接缝观感（拉伸后） | `snapshots/frm_*_stretch_*.png` ×6 | 出样，归 human（**B2 红线单列，见 finding #3**） |
| A4.4 锯齿观感（-8° 印章） | `snapshots/stamp_3x.png` | 出样，归 human（**脆弱点单列，见 finding #5**） |
| A5.3 平铺接缝/密度观感 | `snapshots/frames_stamp_dots.png` | 出样，归 human |
| A7.3 站酷样字可读（30px CJK） | `snapshots/zcool_3x.png` | 出样，归 human（ARQ-260831-small-cjk-1bpp-readability 风险区） |
| A8.3 等宽表格线/冒号对齐 | `snapshots/imgfont_row.png` | 出样，归 human |

## Simplification Debt

自本次改动中 grep `minimal:` 标记 → 无（零命中，`no-trigger`）。管线无过早抽象、无重复实现；
`make_snapshots.mjs` 拼版出样与 `gen_assets.mjs` 栅格化分离，职责单一。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | 资产口径 / 跨包一致性 | `pose_daily_96` 口径变更：旧 98×98(-1°) → 本包中立化 96×96(0°)。理由：-1° 倾斜属 P1 摆放层关注点（CSS `tilt1`），烘进姿态族会导致「换姿态 + 换倾斜」双重跳位；7 姿态统一 0° 才真同族同尺寸（A2.2 判绿前提）。副作用：轻微改变 sibling 包 `260831-simulator-lvgl-align` P1 快照里吉祥物 1° 倾角。 | `gen_assets.mjs`（姿态族统一 0°）+ `UI-art-feasibility.md` §B1「口径变更（记录）」；旧产物 `pose_daily_96` 98×98 被同一命名覆盖。 | 记录口径变更（已在本 review-packet + 报告 §B1 + docs-delta §3.1 三处落痕）；sibling P1 快照的吉祥物 1° 倾角变化为口径调整的预期结果，非本包回归，无需回改 sibling。 |
| SHOULD | judgement-call | 字体可复现性 / 宿主依赖 | **ZCOOL KuaiLe 真体宿主依赖**：librsvg 对 ZCOOL 的 `@font-face` data-uri 静默忽略（1.5MB full 超出处理）、对 Baloo 2 data-uri 生效。故 ZCOOL 走 per-user 装字体（`%LOCALAPPDATA%\...\Fonts` + HKCU 注册），`FONTVERDICT` 真体机判 REAL；但**未装 ZCOOL 的机器重跑管线会打 SUBSTITUTE**（回落 Microsoft YaHei）。 | `install-fonts.ps1` + `gen_assets.mjs` `doFontVerdict()`（REAL/SUBSTITUTE 逐字记录）+ 报告「字体替身坑记录」。 | 真机重烘以 ZCOOL/Baloo 2 子集为准（FreeType 子集，不依赖宿主安装）。管线可复现性已在报告/脚本注释明示；文档已记录 FONTVERDICT 行为，未装 ZCOOL 机器可预期打 SUBSTITUTE。 |
| SHOULD | judgement-call | B2 框族 9 宫格忠实性（V3 决策输入） | `.frm` 的 255px/225px 水平大半径是「近乎平直的手绘角」，24px 角片协议只会截取大半径曲线前 24px，把平缓手绘角裁成紧弯角、扭曲手绘不对齐感。机判只能证角部结构不变形（`stretch=false`），证不了手绘「忠实度」；需 human 目测拉伸出样裁定。 | `UI-art-feasibility.md` §B2「9 宫格忠实性红线」+ `snapshots/frm_*_stretch_*.png`（6 张）；CSS 源 `--hand-r: 255 15 225 15/...`。 | V3 前 human 目测 `frm_*_stretch_*.png` 裁定「24px 角片协议」可接受；不可接受则改「大框 40px 角片档 / 小框 20px 角片档」或「框型按固定尺寸整烘」。 |
| SHOULD | judgement-call | B8 预算隐患（V3 决策输入） | 唯一逼近预算的是 P4 笔顺字盘——保守按整帧直烘预估 ~3.1MB（占当前总量 3,851,794 字节的绝大部分）。若按「每笔每态各烘一份整帧」盲目直烘即踩预算；须在 V3 前锁定打包策略。 | `budget.md` §3「P4 笔顺字盘 米字格 236×236 整帧 + 例字 9 笔 × 3 态」预估 3,118,976 字节；§4「唯一隐患」结论。 | V3 前锁定「笔形 3 态叠加复用 + 米字格单份背景」打包策略，否则 V3 重算本预算。 |
| SHOULD | judgement-call | -8° 印章内圈虚线（V3 决策输入） | 「外实内虚」双圈中内圈是 2.5px 虚线，是全场最细笔画。-8° 斜盖后台阶锯齿主要落外圈 4px 实线（机判外圈 1956px 实线在场），但内圈虚线（196px）短划有断口/糊连脆弱点；机判只证「双圈 + 文字均在场非空白」，证不了细虚线观感。 | `snapshots/stamp_3x.png` + 报告「-8° 成就印章锯齿结论（单列）」段。 | human 目测 `stamp_seal_r8` 内圈虚线断口可接受性；不可接受备选 3 条（内圈增粗 3.5px / 降角至 -5° / 运行时两圈分离叠加）。 |

> 无 BLOCKING finding。五项均为「已声明、已落痕、指向 V3/后续」的 judgement-call：2 条 FYI/SHOULD 是诚实记录的
> 非绿项（口径变更 + 字体宿主依赖），3 条 SHOULD 是 V3/human 决策输入。均不进入合并门前退回，但须随本 review-packet
> 与 docs-delta §3.4 一并带到 V3 设计稿阶段。

## Five-Axis Review

1. **Correctness**：所有声明的 UAA-Ax / E 项均有机械或运行时断言承接（30 项机判全绿）。7 姿态同族同尺寸（A2.2）、
   9 宫格拼回 diff=0（A3.3）、倾斜真实烘入（A4.1 旋转产物 ≠ 0° 版本）、网点精确 4×4 + 密度递增（A5）、imgfont 等宽
   基线对齐（A8）、预算求和对账 == `assets.c` `data_size` 求和（A9.2）。无空过断言（「存在符号即过」在框族/网点/印章
   三处被明令禁止，只钉真实产物）。
2. **Test quality**：机判走 sharp/libvips 真实栅格化 + 逐像素统计（黑占比/尺寸/拼回 diff/阈值化唯一值集合），非 mock；
   harness 对逐资产缺失/尺寸/密度/求和敏感（移除任一即红）。30 项断言逐条对齐冻结验收 `test_art_assets.acceptance.md` §12。
3. **Contract/boundary safety**：`docs/contracts/**` 零写（零契约面，契约门未触发）；设计稿 `docs/ui/kidpal-ui-design-v2.html`
   只读抽取，未改设计稿；vendor `ESP32-S3-RLCD-4.2-Demo/**` 零触；无新依赖、无跨子系统调用、无循环依赖。安全。
4. **Operability/security**：字体授权（OFL 1.1 原文 + 免费商用 + 改名字段条款逐字引用）落档；改名禁则保守结论已记录
   （设备端子集化重分发须核对 RFN 或改名）。无 secret、无网络、无权限面。管线可复现性依赖 ZCOOL host 安装（finding #2，
   已明示）。安全。
5. **Maintainability/simplicity**：唯一母版（设计稿 SVG/CSS）→ `gen_assets.mjs` 抽取 → 栅格化的一路管线，无散件 SVG；
   `flatten 先于阈值化` + `svgUse 内联` + `字体替身` 三处既有修复保留（E2）。无过早抽象、无重复实现、无 `minimal:` 债务。干净。

## Residual Risk

- **Known limitation**（两条非绿项 + 三条 V3 决策输入，均已在 Findings 逐条落痕）：
  - `pose_daily_96` 口径变更（98×98/-1° → 96×96/0°）——sibling P1 快照吉祥物 1° 倾角随之变化，预期非回归。
  - ZCOOL 真体依赖 per-user 宿主安装——未装 ZCOOL 机器打 SUBSTITUTE，真机重烘以子集为准。
  - 模拟器（真彩灰度 FreeType 抗锯齿）与真机（1bpp 阈值化）仍有系统性显色差异（ARQ-260831-simulator-truecolor-vs-1bpp）：
    本包验收图一律阈值化 PNG（E3）以校准此差异，但 30px 以下 CJK 真机可读性仍未锁下限。
- **Follow-up recommended**：见 docs-delta §3.4 的 5 项留根清单（B2 九宫格忠实性裁定 / B8 字盘打包策略 / -8° 印章虚线 /
  ZCOOL 字体宿主 / ARQ-260831-small-cjk 字号下限），随 V3 设计稿阶段逐一消解。
- **Human decision needed**（V3 / human 三选口径）：
  - B2「24px 角片协议」是否可接受（不可接受 → 40px 角片档 / 固定尺寸整烘）。
  - -8° 印章内圈虚线断口是否可接受（不可接受 → 增粗 3.5px / 降角 -5° / 运行时两圈分离叠加）。
  - P4 笔顺字盘打包策略锁定（笔形 3 态叠加复用 + 米字格单份背景）。

## 预算 I1 判定结论

已出图 47 枚 = **599,698 字节**（`== assets.c data_size` 求和，A9.2 对账一致）；+ 预估 P2–P4 = 3,252,096 字节 →
预计总占用 **3,851,794 字节**，对 8M（8,388,608）**margin = 4,536,814 字节（54.1%）**。**未超预算 → 不触发 I1**，
`ARQ-260831-i1-indexed-asset-on-device` 不升 P0（docs-delta §2 已在 ARCH-REVIEW-QUEUE 该条「复议触发」列补记「TP-B B8 已核：
未超预算、不触发 I1」）。唯一逼近预算的隐患 = P4 笔顺字盘（finding #4），V3 前锁定打包策略即可。

## decisionCoverage 落位反查（4 决策，全部 landsIn -> acceptance，无 dropped / risk-accepted）

| decision | 承载 acceptance | 状态 |
|---|---|---|
| strategy-§2.2-美术元素资产化 | UAA-A2 (B1)（七姿态）+ 同族框/网点/图标/印章分别由 UAA-A3/A5/A6/A4 承载 | ✅ 已验（对应机判 A2/A3/A5/A6/A4 全绿） |
| OQ-1a-倾斜烘进资产 | UAA-A4 (B3) | ✅ 已验（A4.1 rotate 先于 flatten + 印章真实旋转 + 微倾出样） |
| OQ-2a-框族 9 宫格资产化 | UAA-A3 (B2) | ✅ 已验（A3.2 切片在场 + A3.3 拼回 diff=0 + A3.4 角片 stretch=false） |
| strategy-§4-资产总量盯防 | UAA-A9 (B8) | ✅ 已验（A9.1 分区锚 + A9.2 求和对账 + A9.3 I1 判定） |

四决策均 `landsIn: acceptance:...`，无 `dropped`、无 `risk-accepted` 条目。