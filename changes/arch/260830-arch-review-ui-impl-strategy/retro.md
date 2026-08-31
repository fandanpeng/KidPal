# retro · 260830-arch-review-ui-impl-strategy（Track B 全环：实战验证型乘客）

> 主线程第一人称热记忆 · 2026-08-31 · 本仓 Track B 首名乘客 · harness 形态：判官 = architect 子 agent 三轮（R1 打回 / R2 GREEN / R3 GREEN），核验官 = landing-verifier 子 agent 一轮绿 · 特殊形态：本环在 R2 GREEN 后按 human 指示插入了一次「实战验证」相位（模拟器上真做 P1），是本仓第一次「策略先实战、后 landing」的 Track B 变体。

【硬项 R1】verdict 绿后趁热写，第一人称；【硬项 R2】无 token/时长精确计量，§2 为估计法+误差界声明；【硬项 R3–R5】见对应节。

## 1. 时间账

无逐相位时间戳计量（会话跨多次 compaction），按制品 mtime 与会话顺序粗排：

| 相位 | 内容 | 备注 |
|---|---|---|
| 起草 | PROPOSAL v0.1 → v0.3（含 OQ 三项 human 裁定回填） | 2026-08-30 起草，human 当日裁定 OQ |
| 评审 R1 | architect 打回：1 项事实修正（LUT 字节数）+ 2 项可选澄清 | R1-1 必修项当日修订 |
| 评审 R2 | architect 复审 GREEN | 2026-08-31 |
| 实战验证 | human 指示插入：模拟器上用本方案真做 P1，产出 gen_assets.mjs 管线 + kidpal_ui.c 重写 + 三张对比图；发现 V1–V6 回填为 v0.4 | 本环最大相位（约占全程一半以上工作量） |
| 评审 R3 | architect 对 §8 实战节复审 GREEN | 2026-08-31 |
| FROZEN | human 立项 ack（「先landing吧」）→ 状态行 FROZEN 化 + changes-index 钉 frozen | 2026-08-31 |
| landing | T1 新增策略文档 / T2 spec §5.7 + 决策 17 补注 / T3 ARQ 三条登记 | 同日 |
| 核验 | landing-verifier verdict：绿（带两个治理留根） | 同日 |
| **合计** | 全程跨 2 个自然日（2026-08-30 起草 → 2026-08-31 闭环） | — |

对照：本仓 Track B 首名乘客，无前乘客基线；流程骨架自 AInativecompany 迁入，本环是它在新仓的首次全程实跑。

## 2. Token 账

计量方式诚实声明：无 token 精确计量，仅 landing-verifier 子 agent 有 usage 回执（44,019 tokens / 17 次工具调用）。其余相位按工作性质粗估，误差界 ±50%：

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 实战验证 + 落地） | ~250k（估） | ~75% | 实战验证相位占大头：资产管线排障（I1 反色、flatten 黑版、libvips use 链）+ 两轮布局碰撞调优 |
| 判官（R1+R2+R3 三轮 architect） | ~60k（估） | ~18% | R3 含 LVGL 源码逐字核验 |
| 核验官（landing-verifier） | 44k（实测） | ~13% | 占比小但接住结构门缺口判定 |

花费最大处：实战验证的排障循环（I1 反色 → 读 LVGL 9.5 源码定位调色板绕过 → 改 RGB565 直出）。可压缩分析：若管线起步前先做一次「I1 vs RGB565 最小渲染实验」，可省一轮全资产返工；但源码级结论（V1）本身正是本轮最有价值的产出之一，不全算浪费。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · I1 索引图像在 LVGL 9.5 被绕过调色板，资产全部反色**：现象——管线按直觉出 I1（1bpp 索引+内嵌调色板），模拟器上全部反色/糊版，运行时转换尝试还产出交替条纹坏块 → 根因——9.5 SW blend 路径 `i1_image_blend` 把位图当 alpha/灰度直接 `get_bit*255` 混色，调色板只有 bin-decoder 对文件源才查表（`lv_draw_sw_blend_to_argb8888.c:402-491` 函数体零 palette 引用）→ 证据锚：PROPOSAL §8.2-V1 + R3 复核表逐字证实。**教训：资产格式选型不能凭文档直觉，最小渲染实验先行。**
- **F2 · libvips/sharp 栅格化三连坑**：现象——透明 SVG 不 flatten 整版变黑（透明像素 RGB=0 被阈值判黑）、吉祥物三层 `<use href>` 间接链渲染为空白、`<text>` 依赖环境字体（Baloo 2 被 Segoe UI Bold 替身）→ 根因——libvips 的 SVG 支持面比浏览器窄 → 证据锚：PROPOSAL §8.2-V2/V3，gen_assets.mjs `:47` flatten 修复在场。
- **F3 · 400×300 绝对定位密排的碰撞调优吃掉约一半实战工时**：现象——涂鸦与电池、预告文字与吉祥物、贴纸与湿度读数三轮碰撞，靠「快照 PNG → 人眼 → 改坐标」循环收敛；期间还因手抄坐标偏离设计稿返工 → 根因——设计稿坐标没有管线化转写，全靠人肉 → 证据锚：PROPOSAL §8.2-V5，已固化为 T1 §2.5「坐标转写」条款。
- **F4 · landing 被 enforce-scope 白名单拦截**：现象——T1/T2 首次写入被 universal.docsTopLevel 拦下，`docs/spec.md`/`docs/notes/`/`docs/ui` 均未登记（docs 平铺重构在 git 里建了这些路径，scope-map 白名单没跟上）→ 根因——治理面登记与仓库实际结构漂移，且钩子提示的 `.claude/rules/repo-structure.md` 骨架文档根本不存在 → 证据锚：landing-summary §2 插曲、landing-verdict §7。处置为最小登记三路径，系统性对账留治理包。
- **F5 · 结构门末段子门因迁移缺口不可用**：现象——`landing-fidelity-check.mjs` 前置断言全绿，末段 cross-surface 子门（GAR-001）因子脚本 `cross-surface-check.mjs` 未随迁出而 MODULE_NOT_FOUND → 根因——治理迁移未迁全脚本集 → 证据锚：landing-verdict §结构门判读（「前置结构断言全绿，红纯属机制缺口」）。
- **near-miss（最险一刻）**：F4 的 scope-map 登记若处理不当有两种坏法——要么为了过门把 landing 内容塞进已登记的错误路径（影子落地），要么顺手把 scope-map 改成大口放行（机械强制层被 convenience 腐蚀）。没出是因为钩子报错文本本身指明了最小登记路径，且我把这次前置修复与 landing 内容在 summary/retro 里分开如实披露，让 verifier 能独立核验其最小性（verdict 确认「diff 严格 = 三处，无夹带扩张」）。**教训：机械层拦你时，先修登记的错，不要绕门。**

## 4. 优化建议（按 ROI 排序）

1. **补迁或裁剪 cross-surface-check 子门**：治 F5 · 载体 = 固化候选 → ARQ 收件箱（已登记 `ARQ-260831-cross-surface-check-missing`）· 预期收益：结构门恢复全程可用，不再靠 verifier 人肉覆盖末段。
2. **docs 平铺后 scope-map 白名单系统性对账 + 建 repo-structure.md 骨架**：治 F4 · 载体 = 固化候选 → ARQ 收件箱（已登记 `ARQ-260831-docs-top-level-registry-gap`）· 预期收益：消灭「git 里存在但白名单未登记」的隐性漂移类，下一个 landing 不再撞门。
3. **坐标转写管线化（设计稿 HTML style → 布局表）**：治 F3 · 载体 = 已固化进策略文档 T1 §2.5，执行归 P2–P4 dev-loop 包 · 预期收益：直接砍掉 P2–P4 各页一半左右的碰撞调优工时。
4. **模拟器降级 9.3 的优先级提高**：治 F1 同类（9.5 验证通过 ≠ 9.3 无坑）· 载体 = 登记不硬治（ARQ-260831-simulator-lvgl-version-drift 已含），执行归 simulator dev-loop 包 · 预期收益：把版本错位假信号从「不可分辨」降为「构建期可机判」。
5. **资产格式选型先做最小渲染实验**：治 F1 · 载体 = 留批次（作为经验写进策略文档 §2.3 注记），不单独固化 · 预期收益：后续新增资产格式（如 I1 真机复验）先跑一枚小样再全量。

## 5. 一句总结

成本 ~350k tokens（估，±50%）/ 2 个自然日 · 触点 human 三次（OQ 裁定、实战验证指示、立项 ack）· 制品 4 件齐（PROPOSAL v0.4 / architect-landing-summary / landing-verdict / 本 retro）· 本仓 Track B 首名乘客、无前基线 · 最险一刻 = scope-map 前置登记差点变成绕门或大口放行 · **保险丝判定：两方独立结构接住了作者偏差**——判官 R1 独立揪出 LUT 字节数事实错误（我起草时算错的 240K+120K）、R3 独立逐字核验了 V1 源码断言；核验官独立判读结构门红色为「机制缺口非内容不合格」并对 scope-map 登记做了最小性核验，两处我单方说了算的地方都有第二双眼睛 · 最值得带走的一条：**「策略提案 + 实战验证后再 landing」这个 human 指示的变体非常值得保留**——它把「需资产化」从推断变成实证，还顺手贡献了 V1 源码级结论和 RGB565 基线修订，没有这一步，T1 的资产格式口径会带着 I1 的错误默认落地。

## 6. 本环遗留

- `ARQ-260831-cross-surface-check-missing`（本 retro F5 固化）：cross-surface-check.mjs 迁移缺口，建议补迁或裁剪触发。
- `ARQ-260831-docs-top-level-registry-gap`（本 retro F4 固化）：docs 平铺后 scope-map 白名单系统性对账 + `.claude/rules/repo-structure.md` 骨架建册。
- T3 三条既有登记（simulator-lvgl-version-drift / simulator-truecolor-vs-1bpp / p4-stroke-dasharray-visual-check）：去向见各条目「处理时机/复议触发」列。
- 卫生项（landing-verdict §7 提示，非留根）：`tools/asset-pipeline/node_modules/`（sharp/libvips 二进制 50+ MB）未被 .gitignore 覆盖，合并前须防误提交；仓根零字节碎片 `0`、`CANONICAL_TEST.test(f)` 建议清理。
- 模拟器实验代码（`kidpal_ui.c` / `gen_assets.mjs` / vcxproj 资产挂接）不进本批 landing，由后续 simulator dev-loop 包接管正式化（策略文档 §6 已点名）。
