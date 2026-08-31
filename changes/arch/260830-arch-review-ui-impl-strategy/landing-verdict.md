# Landing Verdict · 260830-arch-review-ui-impl-strategy

> 核验人：landing-verifier（Track B 步骤 5，独立核验；只读 codebase，只写本 verdict）
> 日期：2026-08-31
> 判定依据：FROZEN `PROPOSAL.md` §2 landing 清单（T1/T2/T3）+ §5 验证集 + 实际 git diff 实证。**不采信 landing-summary 自述**。

## 0. 结论

**绿（放行建议）——带两个治理留根（均不阻塞本批 landing 内容合格，属机制层缺口）。**

landing-manifest 三路径全部落地、改法与 FROZEN PROPOSAL §2 逐字/逐段一致、无 manifest 外 landing 内容、append-only 面未被破坏。结构门因 cross-surface 子门（GAR-001）所需子脚本在本仓缺失而以 MODULE_NOT_FOUND 失败——**属治理迁移缺口，非 landing 内容不合格**；该子门本应机判的部分已由人读核验覆盖（见 §4、§5）。

---

## 1. 结构门（机器）输出

命令：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260830-arch-review-ui-impl-strategy`

```
{ "ok": false, "gate": "landing-fidelity",
  "message": "cross-surface alignment sub-gate failed",
  "subGate": "cross-surface-check", "subExitCode": 1,
  "subOutput": "Error: Cannot find module 'C:\\AI\\KidPal\\.claude\\scripts\\cross-surface-check.mjs' ... code: 'MODULE_NOT_FOUND'" }
EXIT=1
```

**判读**：脚本在到达 cross-surface 子门**之前**的全部硬断言均已通过——若任一前置硬断言（未冻结 / 无 packet-binding / summary⊄proposal / claimed-but-not-landed / planned-but-not-landed / manifest 块缺失 / §2 锚清单≢manifest）失败，脚本会在 cross-surface 子门（源码 `:313` 起）**之前** `fail(...)` 退出。实际失败点正是 cross-surface 子门本身，证明前置结构断言全绿。失败根因 = 子脚本 `.claude/scripts/cross-surface-check.mjs` 在本仓不存在（触发条件 = diff 面含 `.claude/scope-map.json`，见 `:317`），与 landing 内容无关。**故结构门判「机制缺口红、内容侧前置断言绿」，不作 landing 不合格处理**（与任务口径一致）。

---

## 2. 逐文件人读 diff 核对（landing 清单三路径）

| manifest 路径 | structuralTouched | semanticMatch | expected（FROZEN PROPOSAL §2） | actual（diff 实证） |
|---|---|---|---|---|
| `docs/notes/ui-implementation-strategy.md` | ok（untracked 新增，git status `??`） | **ok** | T1：五段结构（可行性矩阵 / 方案定论 / 模拟器对齐 / 性能预算 / 分阶段路线）+ v0.4 实战回填口径（RGB565 基准、imgfont 增量、1bpp 滤镜、坐标转写、执行期注意项） | 全文 90 行齐备：§1 可行性矩阵（含 R1-3 网点口径「需资产化（同框族走资产管线，与 OQ-2(a) 口径一致）」）；§2 方案定论（§2.3 资产管线 RGB565 基准+I1 留真机优化项、§2.4 字体含 imgfont/预渲染增量、§2.5 坐标转写）；§3 模拟器对齐（9.3 锁定 + lv_conf 对齐 + 1bpp 滤镜/阈值化快照 + 400×300）；§4 性能预算（LUT 240KB+120KB 修正版、RGB565 96KB/大图 55KB 盯总量）；§5 分阶段路线；§6 执行期注意项（含 simulator 包接管实验代码一项）。全部命中，无漏段。 |
| `docs/spec.md` | ok（` M`） | **ok** | T2：新增 `### 5.7 UI 实现策略` 小节（== §2 T2 引文）；决策 17 仅追加规定补注半句 | diff 新增 `:496-498` 三节标题 + 正文 + 空行；`:498` 正文与 PROPOSAL §2 T2 引文**逐字一致**（byte-identical，含 `[ui-implementation-strategy.md](notes/ui-implementation-strategy.md)` 链接）。决策 17（`:540`→`:544`）唯一改动 = 行尾追加「——美术元素（吉祥物/手绘框/涂鸦）不走运行时绘制，走预栅格化 1bpp 资产（§5.7）」，前半句原样未动。spec.md 全 diff 仅此一处删除（旧决策 17 行）+ 对应替换行 + 新增节——无越权改动。 |
| `docs/ARCH-REVIEW-QUEUE.md` | ok（` M`） | **ok** | T3：append-only 新增 3 条登记（§2 T3 三条），既有行未动 | diff **零删除行**（append-only 成立）。新增 3 条：`ARQ-260831-p4-stroke-dasharray-visual-check`（1.2 deferred，P4 虚线点链视觉对齐，对应 §2 T3 第三条）、`ARQ-260831-simulator-lvgl-version-drift`（1.3 watchlist，版本错位 + I1 调色板行为差异，对应 §2 T3 第一条 + §8.2-V1）、`ARQ-260831-simulator-truecolor-vs-1bpp`（1.3 watchlist，显色差异 + 阈值化验收口径，对应 §2 T3 第二条 + §8.2-V6）。三条内容与 §2 T3 口径一致。 |

**私货 / 夹带核验**：三个 manifest 文件 diff 内未发现 PROPOSAL §2 未授权的改动；无权限/写域扩张语句；无与批定矛盾处。

---

## 3. PROPOSAL §5 验证集重放（原始输出）

§5.2 事实断言复核（全部命中，真空绿规约「重放先断言输入非空」满足）：

```
grep -n "lvgl/lvgl" firmware/main/idf_component.yml        → 43:  lvgl/lvgl: ~9.3.0
grep -n "version: 9.3.0" firmware/dependencies.lock         → 702:    version: 9.3.0   （行号 :702，PROPOSAL 引 :701，§5.2 已声明行号漂移可接受、行须在——成立）
grep -n "CONFIG_LV_USE_THORVG is not set" firmware/sdkconfig → 3087:# CONFIG_LV_USE_THORVG is not set
grep -n "CONFIG_LV_USE_VECTOR_GRAPHIC is not set" ...        → 2854:# CONFIG_LV_USE_VECTOR_GRAPHIC is not set
ls firmware/managed_components/lvgl__lvgl/src/others/imgfont/ → lv_imgfont.c  lv_imgfont.h（目录在场）
ls firmware/main/display/lvgl_display/gif/gifdec.c           → 在场
```

§5.3 spec 链接核验：

```
grep -n "ui-implementation-strategy.md" docs/spec.md → 498:（§5.7 正文链接行，≥1 处成立）
grep -n "§5.7\|5.7" docs/spec.md                    → 496:### 5.7 UI 实现策略（新增节在场）
```

§5.1 锚点完整性（T1 引用的设计稿 `data-od-id` 锚点逐一 grep）：`canvas-p1 / sec-icons / canvas-p4 / pose-daily / pgbuts / pgdots / .frm / .frm-sm / tilt1 / tilt2 / .d50 / .d25 / .d12 / .inv` 共 14 个全部 `OK` 命中 `docs/ui/kidpal-ui-design-v2.html`。

---

## 4. 破坏性自检（manifest 外路径 / 契约面 / 工具面）

- **`docs/contracts/**` 零改动**：`git diff HEAD --stat -- docs/contracts/` 输出为空。契约面未被本批触碰——符合 PROPOSAL §3「契约面 未触」声明与 Hard Rules 契约冻结。
- **`tools/` 不在 landing 内**：manifest 三路径无 `tools/`。工作树确有 `tools/` 改动（`M tools/simulator/...kidpal_ui.c/.vcxproj/lv_conf.h`、`?? tools/asset-pipeline/**`），但据 PROPOSAL §8 与 summary §2，这些是**实战验证的一次性实验代码、明确不属于本批 landing**（§8.4 声明由后续 simulator 包接管）。landing-verifier 不把它们计入本批 landing 范围，亦不视为 scope creep——但提示：`tools/asset-pipeline/node_modules/`（sharp/libvips 依赖，含 50+ MB 二进制）未被 `.gitignore` 覆盖（`.gitignore` 无 `node_modules` 规则），合并入 main 前须避免误提交该目录。
- **canonical 面越界改动**：tracked 改动（`git diff HEAD --name-only`）= `.claude/scope-map.json` + `changes/INDEX.md` + 两个 manifest 文件 + 三个 `tools/` 实验文件。其中 `changes/INDEX.md` 的改动 = changes-index upsert 单行钉 `| 260830-arch-review-ui-impl-strategy | 2026-08-30 | frozen | — |`（append-only，既有行未动）——这是 Track B 流程规定的 INDEX 钉入动作，非 manifest 内 landing，但属合法流程产物。无其它 canonical 面（G1 指令根 / `.claude/agents/**` / review-loop 定义）被改。
- **stray 文件**：仓根有两个零字节 untracked 文件 `0` 与 `CANONICAL_TEST.test(f)`（2026-08-31 10:05 时间戳），系 shell 重定向碎片，非 landing 内容，建议清理但不影响本批判定。

---

## 5. scope-map 前置登记最小性核验（summary §2 插曲）

`.claude/scope-map.json` diff（全量）：

```
allowFiles:  + "docs/spec.md"                       （其余 5 行既有）
allowDirs:   + "docs/notes", + "docs/ui"            （其余 4 行既有）
```

**判定：最小。** 改动严格 = 「allowFiles 补 `docs/spec.md` + allowDirs 补 `docs/notes`/`docs/ui`」两项三项，与 summary §2 自述完全一致，无任何额外 allow/deny 扩张、无 universal 段改动、无契约面放行。此登记填补了 docs 平铺重构后的白名单缺口（这些路径已由先前提交在 git 建立、scope-map 未同步），属 landing 的合法前置修复，已如实披露。**未发现借登记夹带的写域扩张。**

---

## 6. 破坏性自检（verifier 自身）

本核验全程只读 codebase（git diff / git status / grep / ls / Read），唯一写入 = 本文件 `changes/arch/260830-arch-review-ui-impl-strategy/landing-verdict.md`（本目录下 verdict，权限允许）。**未写任何 canonical 面、未触 `docs/contracts/**`、未 push。** 自检通过。

---

## 7. 两个治理留根建议（不阻塞本批，进留根/队列）

1. **cross-surface 子门脚本缺失（GAR-001 机制缺口）**：`.claude/scripts/cross-surface-check.mjs` 在本仓不存在，`landing-fidelity-check.mjs` 末段子门一旦被触发（diff 面含 scope-map / agents / scripts / hooks / route-fixtures / settings）即以 MODULE_NOT_FOUND 红。建议作为治理留根：补迁 `cross-surface-check.mjs`，或在本仓裁剪/旁路该触发（本仓治理面结构与原仓不同，子门判据未必适用）。登记建议进 `docs/ARCH-REVIEW-QUEUE.md`。
2. **scope-map 登记缺口残留**：docs 平铺重构后 scope-map 白名单未同步的缺口，本批只补了 `docs/spec.md`/`docs/notes`/`docs/ui` 三处（landing 即时需要的最小集）。**其余 docs 顶层路径（如 `docs/ui/` 下其它既有文件、`docs/archive` 等）是否仍有未登记项未系统性排查**；且 `.claude/rules/repo-structure.md` 不存在（骨架建册留待后续）。建议登记一个「docs 平铺后 scope-map 白名单全面对账 + 骨架文档建册」的治理留根项，避免每次 landing 撞钩子再零星补登记。

---

**放行建议**：本批 landing 内容合格，建议进入步骤 6（retro）与步骤 7（human 合并）。**merge 仍归 human manual-merge 门**，本 verdict 不替 human 拍板。
