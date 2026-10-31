# PROPOSAL · 261028-font-typography-scaleup

> **状态：FROZEN v1.0 · 2026-10-28 · 凭「human 立项 ack + architect-reviewer R1 GREEN（判档 = 主路径，契约 = NO_CONTRACT_CHANGE）」标定；packet-bound 落地启动。**（原 DRAFT v0.1 · 2026-10-28 · human 立项 ack 已获，待 architect-reviewer 四查与判档——立项凭据 = spine `delivery/intake/261028-font-typography-scaleup.md` facePlan 顺序 2、human 触点①与需求面终拍两度 ack。）
> **承载 ARQ**：`ARQ-260831-small-cjk-1bpp-readability` 已由需求面批按 change-intake §3a 以 consumed-by 结项并归档（`docs/debt/tech/_archive/2026/ARQ-260831-small-cjk-1bpp-readability.md`）；本批不重复登记、不改账本。根来源 = 上述 spine + frozen workstream `delivery/workstreams/261028-font-typography-scaleup/PROPOSAL.md` F-1..F-9。
> **review 轨**：Track B；architect 预判**主路径**——§5.5.3 改变字体生产、装载与降级机制，并新增 ADR-017，轻路径判据①②④均不满足；最终判档归 architect-reviewer，只升不降。Track A 不触发：方向已由 human 立项与 workstream FROZEN 收敛。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（纯架构文档与 ADR 两文件，零契约、零实现、零部署；但字体装载与降级机制会约束 P0–P3，误写会误导后续实现，故不取 low。）
> **review profile**：架构面（`docs/architecture.md`）+ 决策记录（`docs/decisions/ADR.md`）；契约面 = `NO_CONTRACT_CHANGE`，G1 / G2 / G3 = 零触。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：架构面明确「16px 正文 + 20px 阅读标题」的字体的生产、落位、运行时装载与缺资产降级路径——两枚全量 CJK `.bin` 打入 assets 8MB 分区，经 cbin 运行时装载，编译内 14px 字体保留兜底；同时消除三处「可读性未实测」的过时表述，并以 ADR-017 留下容量与方案取舍的决策本体。

**现状差距**：`docs/architecture.md` §5.5.3 仍写「三族 SPIFFS 加载」与「16px 以下未实测」，既未覆盖新增两族，也未说清 host / 设备同源、cbin 装载和 14px 降级；§3.1、§4、§8.1 三处仍把已完成的真机实测写成未实测。现网代码已经具备 assets mmap、`Assets::GetAssetData`、`LvglCBinFont` 与 `cbin_font_create` 机制，但 `fonts_root` 在生产代码中零消费者，不能把「机制在编」误写成「本板字体已接线」。

**镜像句边界**：story-author 已核 `docs/architecture.md` 无需求面批的逐字镜像句；本批不重审、不回改 requirements v1.69，只承接其字体机制与架构决策。NFR-READ-1 的 14px 下限语义不变。

**非目标（本批不做）**：

- 不改 `docs/requirements.md`、UI 母版或 brief；这些已由需求面 v1.69 frozen。
- 不改固件、host、资产脚本或分区表；P0 spike 与 P1/P2/P3 页面实现归 dev-loop。
- 不触 `docs/contracts/**`，不把格容量或字号写上端云网线。
- 不做 OTA 字体下发，不做 20px 静态子集，不退役九档实验页与测试字体。
- 不重述需求面逐字句，不把实现期尚未生成的 16px / 20px 字体精确尺寸伪填为实测值。

---

## 2. 问题、根因与改法（T1…T5）

### T1 · §5.5.3 字体管线换代为 assets .bin + cbin 运行时装载 + 14px 兜底

**问题**：现文仍把字体管线概括为「PuHui 子集三族、SPIFFS 加载」，没有覆盖本批新增的 16px 正文与 20px 标题两族，也没有钉定 assets 分区、cbin 运行时装载、host 同源与编译内 14px 降级。末尾「未实测」亦与 NFR-READ-1 当前实况冲突。

**证据**：

- `docs/architecture.md:301` 现文逐字见下方「现文」。
- `firmware/main/assets.cc:52` 已查找名为 `assets` 的分区；`:157-168` 已存在 `GetAssetData` → `LvglCBinFont` → theme 的通用装载机制。
- `firmware/main/display/lvgl_display/lvgl_font.cc:5-12` 已存在 `cbin_font_create` / `cbin_font_delete` 包装。
- 2026-10-28 起草期实跑 `git grep -n "fonts_root" -- firmware/main ':!firmware/main/test'` 退出码 1，生产代码零命中；故只能写「机制在编、P0 接线」，不能写「现役字体已经运行时装载」。
- frozen workstream F-5 / F-6 / F-7 已裁：assets `.bin` + cbin + 编译内 14px 兜底；20px 全量 CJK、缺字回退 16px；host 与设备共用同一 `.bin`。

**根因**：260911 批只落了字体资产，运行时接线被拆到后续包，架构句停留在更早的「SPIFFS 加载」设想；本批若只加两个字号而不同时写清生产、装载与降级，会把既有文档漂移扩大成实现分叉。

**改法**：保留 `#### 5.5.3 字体管线` 标题，标题下现文整段替换为下列三段。新文明确区分设计期 FreeType、host / 设备同源资产、目标运行时装载链与降级，不宣称旧三枚资产已有生产消费者；需求阈值只指回 requirements，机制本体留在架构面。

现文（verbatim）：

```text
模拟器 FreeType 快速迭代 / 真机 `lv_font_conv` 生成 PuHui 子集 `.bin`（font_body/font_num_baloo/font_title_zcool 三族，SPIFFS 加载）；大号数字与图案化文字首选 imgfont 或预渲染位图（原生标签行距/基线对齐是高频痛点）；16px 以下 CJK 1bpp 真机可读性未实测（NFR-READ-1，单独烧版对照）。
```

新文（完整替换文本）：

```text
模拟器 FreeType 仅用于设计期快速迭代，不作为真机 1bpp 字形等价证据；host 验收与真机共用同一枚字体 `.bin`（host 编入 `cbin_font.c` 读同一文件，同源接线归 P0）。真机字体经 `lv_font_conv` 产出 `.bin` 并打入 **assets 8MB 分区**，运行时由 `Assets::GetAssetData` 取数据、经 `LvglCBinFont` / `cbin_font_create` 装载；缺资产或装载失败时回退编译内 `font_puhui_14_1`（14px），保证不黑屏。

资产字族在既有 `font_body.bin`（14px 正文）、`font_title_zcool.bin`（30px 标题静态子集）、`font_num_baloo.bin`（44px 数字）三枚之外，新增 **16px 正文全量 CJK** 与 **20px 阅读标题全量 CJK** 两枚；本批只接通新增两枚的生产消费，不把旧三枚追述为已完成运行时接线。正文与列表采用 16px，阅读标题采用 20px，标题缺字回退 16px；键帽条 / 辅助条 / 查字窗仍用 14px，九档实验页与测试字体维持编入、不退役。大号数字与图案化文字首选 imgfont 或预渲染位图（原生标签行距 / 基线对齐是高频痛点）。

NFR-READ-1 的 14px 下限已由 2026-09-15 九档真机对照实测定案，并于 2026-10-28 复测，不再挂「未实测」；字号、行数与页面容量的需求阈值归 requirements.md，本节只承载字体生产、落位、装载与降级机制。
```

**新机制 spike 申报**：命中「本板首次把 assets `.bin` → cbin 装载 → 1bpp 渲染 → 页面消费接通」。按 frozen workstream 的切片序，**P0 `font-assets-runtime-loading` 位于最前**，阻塞 P1/P2/P3。首 canary = 资产索引包含新增字体且 host 能读同一 `.bin`；尾 canary = 真机 16px / 20px 看屏取证与缺 assets 时 14px 降级不黑屏。该 spike 是实现面 launch 前置，不是本架构文档批的落地验收替代品。

目标文件：`docs/architecture.md`（改）

### T2 · §5.5.2 补字体资产预算注记

**问题**：现文只说 assets 8MB 分区承载字体 / 音效，没有明确本批新增两枚全量 CJK 字体计入 assets 而非 app 分区，也没有给出当前余量基线。若缺少这一注记，「编入固件」与「资产运行时装载」两条路线在预算面上仍可能混读。

**证据**：

- `docs/architecture.md:297` 现文逐字见下。
- `firmware/partitions/v2/16m.csv:7` 的 ota_1 为 `0x370000`；`:12` 的 assets 为 `8M`。
- 2026-10-28 实测 `xiaozhi.bin = 3,294,896 B`，ota_1 余量 `309,584 B = 302.328125 KiB`；`generated_assets.bin = 2,790,863 B`，assets 余量 `5,597,745 B = 5.338426 MiB`。命令与 verbatim 输出见 §5 基线 A。
- frozen workstream F-5 否决编入固件；§3.3 非目标明确「不做分区表重排」。

**根因**：预算段写于本批字号升级之前，只登记了美术与笔顺字盘预算，没有登记新增字体对两个存储域的不同影响。

**改法**：保留现段全文，段尾追加一条带日期的字体预算注记。容量数字只作 2026-10-28 起草期基线；两枚新字体的精确尺寸由 P0 实产后复测，不在 canonical 预填。

现文（verbatim）：

```text
设计稿 HTML 内嵌 SVG 为唯一母版 → 脚本抽取 symbol → 栅格化 → **RGB565 `.bin`**（I1 索引仅留真机 bin-decoder 优化项，可回收 16 倍空间）→ SPIFFS art 分区；模拟器同批资产双端渲染。管线三坑已修：透明 SVG 先 flatten(#ffffff) 再阈值化；libvips 不解析多层 `<use href>`；`<text>` 依赖运行环境字体（真机须以实际字体子集重烘）。⚠️ 预算口径 = 策略笔记实测于 art 切分前（47 资产 599,698B + P2–P4 估算 ≈3.85MB vs assets 8MB、余量 54.1%）；现分区为 art 512KB（KAP1 美术）+ assets 8MB 双分区（§5.5.6），47 项资产的实际落位与 P4 笔顺字盘（~3.1MB）装载目标须在 Phase 1 资产装载时复核。PSRAM 预算：LVGL draw buffer 240KB + RLCD 帧缓冲 15KB + 两张 LUT 360KB；ThorVG sdkconfig 默认关——预栅格化是复杂图形唯一低成本路径。详见 [notes/ui-implementation-strategy.md](notes/ui-implementation-strategy.md)。
```

新文（完整替换文本）：

```text
设计稿 HTML 内嵌 SVG 为唯一母版 → 脚本抽取 symbol → 栅格化 → **RGB565 `.bin`**（I1 索引仅留真机 bin-decoder 优化项，可回收 16 倍空间）→ SPIFFS art 分区；模拟器同批资产双端渲染。管线三坑已修：透明 SVG 先 flatten(#ffffff) 再阈值化；libvips 不解析多层 `<use href>`；`<text>` 依赖运行环境字体（真机须以实际字体子集重烘）。⚠️ 预算口径 = 策略笔记实测于 art 切分前（47 资产 599,698B + P2–P4 估算 ≈3.85MB vs assets 8MB、余量 54.1%）；现分区为 art 512KB（KAP1 美术）+ assets 8MB 双分区（§5.5.6），47 项资产的实际落位与 P4 笔顺字盘（~3.1MB）装载目标须在 Phase 1 资产装载时复核。PSRAM 预算：LVGL draw buffer 240KB + RLCD 帧缓冲 15KB + 两张 LUT 360KB；ThorVG sdkconfig 默认关——预栅格化是复杂图形唯一低成本路径。详见 [notes/ui-implementation-strategy.md](notes/ui-implementation-strategy.md)。**字体预算注记（`261028-font-typography-scaleup`，2026-10-28）**：本批新增 16px 正文与 20px 阅读标题两枚全量 CJK `.bin` 计入 assets 8MB 分区预算，不占 ota_0 / ota_1；assets 起草期实测余量 5,597,745B（约 5.34MiB，见 ADR-017），两枚新字体的实际尺寸由 P0 资产生成后复测，分区表不因本批重排。
```

目标文件：`docs/architecture.md`（改）

### T3 · 三处「可读性未实测」过时表述勘误

**问题**：§3.1、§4 与 §8.1 风险 10 仍称「未实测 / 待实测 / 字号下限悬置」，与 requirements v1.69 NFR-READ-1 的已完成验证及已归档 debt 结项记录直接冲突。

**证据**：

- 2026-09-15 九档真机对照已钉 14px 下限，2026-10-28 复测轮在案；权威 = `docs/requirements.md:676-680`。
- `ARQ-260831-small-cjk-1bpp-readability` frontmatter 已为 `status: closed`、`closed: 2026-10-28`，结项目录 = `docs/debt/tech/_archive/2026/`。
- 2026-10-28 对 `docs/architecture.md` 复扫，目标旧句命中恰为 `:121`、`:158`、`:301`、`:436` 四处；`:301` 已由 T1 覆盖，其余三行由本 T 覆盖。

**根因**：可读性实测与复测先落在需求面和 debt，架构三处投影未同步；风险表也未按既有「已闭账」先例收口。

**改法**：三行分别整行替换。新文只修正事实状态，不改变 NFR-READ-1 的 14px 下限，不改需求面历史句。

替换 1 · §3.1 `:121`

现文（verbatim）：

```text
- **1bpp CJK 可读性**：16px 以下真机可读性下限未实测（NFR-READ-1，ARQ-260831-small-cjk-1bpp-readability）。
```

新文（完整替换文本）：

```text
- **1bpp CJK 可读性**：14px 真机可读性下限已于 2026-09-15 九档对照实测定案，并于 2026-10-28 复测；本批正文 / 列表上调 16px、阅读标题 20px，键帽条 / 辅助条 / 查字窗维持 14px（NFR-READ-1，ARQ-260831-small-cjk-1bpp-readability 已结项）。
```

替换 2 · §4 `:158`

现文（verbatim）：

```text
| NFR-READ-1 1bpp CJK 下限 | 字体子集管线（`lv_font_conv` → SPIFFS `.bin`）+ 待实测（单独烧版对照） | §5.5.3 字体管线 |
```

新文（完整替换文本）：

```text
| NFR-READ-1 1bpp CJK 下限 | `lv_font_conv` 生成 assets 分区 `.bin`，经 cbin 运行时装载；14px 下限已由九档真机对照实测，编译内 14px 字体兜底 | §5.5.3 字体管线 |
```

替换 3 · §8.1 风险表第 10 行 `:436`

现文（verbatim）：

```text
| 10 | 16px 以下 CJK 1bpp 可读性未实测 | 字号下限悬置 | 单独烧版对照（ARQ-260831-small-cjk-1bpp-readability） | 真机字体任务 |
```

新文（完整替换文本）：

```text
| 10 | 1bpp CJK 字号下限实测（原「16px 以下未实测」） | **已闭账**：14px 下限已实测定案，不再悬置 | 2026-09-15 九档真机对照 + 2026-10-28 复测在案；NFR-READ-1 的 14px 下限语义不变，ARQ-260831-small-cjk-1bpp-readability 已按 `261028-font-typography-scaleup` consumed-by 结项 | 已闭（原 owner 真机字体任务） |
```

目标文件：`docs/architecture.md`（改）

### T4 · 新增 ADR-017 记录字体落位与否决编入固件的理由

**问题**：F-5 是跨资产预算、运行时装载与降级策略的架构决策，单改 §5.5.3 只能回答「怎么造」，不能长期保存「为什么不编入固件、为什么不放 SD、为什么 20px 不做静态子集」。现行 ADR 末号为 ADR-016，无字体落位决策条目。

**证据**：

- `docs/decisions/ADR.md` 头部规定每条采用固定五小节，且 append-only；2026-10-28 复扫 `^## ADR-` 的末条为 ADR-016，`^## ADR-017` 零命中。
- ota_1 余量实测 `309,584 B = 302.328125 KiB`；上游需求批探测给出的 16px 全量估算约 324KiB，见 `delivery/workstreams/261028-font-typography-scaleup/review.md:14`。该 324KiB 是上游估算，不是本批新产字体的精确实测；精确尺寸归 P0 实产回填。
- assets 余量实测 `5,597,745 B = 5.338426 MiB`；cbin 机制在编与 `fonts_root` 生产零消费者证据见 T1 / §5。
- human 已在 workstream OQ1 裁 20px 全量，spine q2 裁九档实验页与测试字体不退役。

**根因**：容量否决与降级底线属于长期决策，若只留在 workstream 或聊天里，后续实现包会重新争论路线，甚至为腾 app 空间误删 14px 兜底或实验字体。

**改法**：在 `docs/decisions/ADR.md` 文件尾保留 ADR-016 末行原文，追加完整 ADR-017。落地文本见 §6「ADR-017 落地附件」，含固定五小节；数字区分「实测余量」与「上游估算」，不把尚未生成的字体写成已实测。

落地锚现文（verbatim）：

```text
- **2026-10-26 注记（决策②的机制枚举部分失效，决策本体不变）**：`noisePatterns` 已于 2026-10-24 退役（R7/OQ-4：一次性回合统一携 `user=<device_id>`、孩子原话照常参与记忆；live 实读为 `[]`），决策② 所列「front 一次性回合的 user 消息以插件 `noisePatterns` 挡」不再成立；`captureSystemRuns=false` 的覆盖也由 2026-10-25 实测收窄——`memory-core` dreaming 子回合落 chat 分支、不被该键排除，现以「生成器全局关闭」处置。决策② 的**本体**（写入口径 = 已验证接线的实际行为、残余逐行申报于 arch §6.7）不变；现行边界与全部残余的权威 = arch §6.7（`261025-spec-reconciliation-memory-writing-scope`）。
```

新文 = 上述锚行原文保留，紧接追加 §6 的 ADR-017 完整文本；不覆盖、不改写任何既有 ADR 条目。

目标文件：`docs/decisions/ADR.md`（改，文件尾 append）

### T5 · §12 变更记录表新增本批 changeId 行

**问题**：架构面「已冻结」的消费判据 = §12 对应 changeId 行在场；本批不落行，spine `anchors.arch` 与 P1/P2/P3 launchPreconditions 无可消费定稿锚。

**证据**：`docs/architecture.md:505-511` 规定新行在上、四列格式与 changeId 定稿语义；当前首行为 `261025-spec-reconciliation-memory-writing-scope`。`.claude/references/arch-change.md:50-54` 同口径。

**根因**：流程定稿义务，非产品缺陷；packet-bound landing 必须把该行纳入改法集，不能依赖落地时临时补写。

**改法**：在 §12 表头分隔行与现行首行之间插入新行；changeId 沿 spine 身份，依据列指实际 arch-review 目录，不含 FROZEN 占位 token。

现文锚（verbatim，表头分隔行 + 当前首行）：

```text
|---|---|---|---|
| 261025-spec-reconciliation-memory-writing-scope | 2026-10-26 | §4（NFR-PRIV-1 映射行：两把配置键口径 → 类级规则 + 内容归属）/ §5.1.2（front 行一次性回合携 `user` 落设备 session 键族；记忆子系统行指针核后不动）/ §6.7 改写（逐行枚举 → 类级规则 + 现行机制与界限 + 七行实例表 + 残余与未闭合申报八条 + 记忆身份补一次性回合归属 + 画像边界来源门换代） | delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/PROPOSAL.md（需求面 v1.68 为语义输入、不回改；契约面零动作；依据 = requirements v1.68 + spine + TECH-261026 + 261025 部署包与 docs-delta + architect 2026-10-26 核实） |
```

新文（完整替换文本，新行置顶）：

```text
|---|---|---|---|
| 261028-font-typography-scaleup | 2026-10-28 | §3.1 / §4 NFR-READ-1 映射 / §5.5.2 字体资产预算注记 / §5.5.3 字体管线换代（16px 正文 + 20px 标题、assets `.bin` + cbin 运行时装载 + 编译内 14px 兜底）/ §8.1 风险 10 闭账 / §12 本行 | delivery/review/261028-arch-review-font-typography-scaleup/PROPOSAL.md（需求面 v1.69 frozen 为语义输入、逐字不回改；契约面零动作；ADR-017 同批落） |
| 261025-spec-reconciliation-memory-writing-scope | 2026-10-26 | §4（NFR-PRIV-1 映射行：两把配置键口径 → 类级规则 + 内容归属）/ §5.1.2（front 行一次性回合携 `user` 落设备 session 键族；记忆子系统行指针核后不动）/ §6.7 改写（逐行枚举 → 类级规则 + 现行机制与界限 + 七行实例表 + 残余与未闭合申报八条 + 记忆身份补一次性回合归属 + 画像边界来源门换代） | delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/PROPOSAL.md（需求面 v1.68 为语义输入、不回改；契约面零动作；依据 = requirements v1.68 + spine + TECH-261026 + 261025 部署包与 docs-delta + architect 2026-10-26 核实） |
```

目标文件：`docs/architecture.md`（改）

---

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 架构 canonical | `docs/architecture.md` | 改 | T1 §5.5.3、T2 §5.5.2、T3 三处勘误、T5 §12 changeId 行 |
| 决策记录 | `docs/decisions/ADR.md` | 改（append） | T4 新增 ADR-017；既有 ADR-001..016 原文零改 |
| 需求 canonical | `docs/requirements.md` | 未触 | v1.69 已 frozen；无逐字镜像句需本批重审 |
| 契约 canonical | `docs/contracts/interface-contract.md` | 未触 | `NO_CONTRACT_CHANGE`；纯板端字体与容量，无 wire / API / MD 形状变化 |
| 实现面 | `firmware/**`、`server/**` | 未触（只读取证） | P0–P3 归 dev-loop；本批不落实现、不跑烧板 |
| 治理面 | `.claude/**`、G1 roots | 未触 | 无角色、流程、门、脚本或设置改动 |
| debt 账本 | `docs/debt/**` | 未触 | ARQ-260831 已由需求批结项归档，不重复登记 |
| UI 设计面 | `docs/ui/**` | 未触 | 母版 v8 / brief v7 / V8 四帧已随需求批 frozen |

**契约四分类建议 = `NO_CONTRACT_CHANGE`**。2026-10-28 实跑：

```text
$ git -C /c/AI/KidPal-pod-m2 grep -n -E "字体|字号|px|每格|容量|font" -- docs/contracts/interface-contract.md
contract keyword grep exit=1 (1 means zero hits)
```

本批不改变端云数据格式、display 响应块、课表 MD 或 shim API，故不携带契约包三节。若实现期发现需要把容量或字体信息上网线，按逃生门转 architect-reviewer 四分类，不在 Pod 内自裁。

**复扫模式与完备性**：本批退役语面仅为「CJK 可读性未实测」的当前态断言，模式覆盖 `16px 以下真机可读性下限未实测`、`16px 以下 CJK 1bpp 真机可读性未实测`、`16px 以下 CJK 1bpp 可读性未实测`、`待实测（单独烧版对照）` 及同风险行的「字号下限悬置」。2026-10-28 对 architecture 的命中为 `:121`、`:158`、`:301`、`:436`，T1 与 T3 全覆盖。requirements 的 NFR-READ-1 历史句受 spine q6 的 append-only 处置保护， debt 归档正文不改，均不纳入残留清扫。其它领域的「未实测」不按本批模式扩大处理。

本批不新增门或机械断言，故无新门 pattern 的全仓预跑义务；§5 的正向存在断言与负向旧文残留断言成对给出。

---

## 4. human 取舍点（OQ）

**无新增 human 取舍点。** 本批直接消费已裁结论：

- workstream F-5：字体落位 = assets `.bin` + cbin 运行时装载 + 编译内 14px 兜底；否决编入固件。
- workstream F-6 / OQ1：20px 取全量 CJK，缺字回退 16px，不做静态子集。
- workstream F-7：host 与设备共用同一 `.bin`。
- spine q2 / workstream OQ3：九档实验页与测试字体不退役。
- spine q6：requirements 的 NFR-READ-1 历史句不改，当前态注记已足够。
- 契约面零触点已在 spine 记录，本批起草期复跑关键字取证一致。

下列仅为 architect 建议，归 architect-reviewer 独立裁定，不升 human：主路径判档、ADR-017 新增而不改既有条目、§8.1 风险 10 以闭账行保留历史而非删行。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

### 起草期实跑基线（2026-10-28，基线 commit `429643b3`）

**基线 A · ota_1 与 assets 容量**（命令读取分区表和二进制实际大小，非估算）：

```text
$ node -e "const fs=require('fs');const root='C:/AI/KidPal-pod-m2/';const csv=fs.readFileSync(root+'firmware/partitions/v2/16m.csv','utf8');const lines=csv.split(/\r?\n/);console.log(lines.find(l=>l.startsWith('ota_1,')));console.log(lines.find(l=>l.startsWith('assets,')));const fw=fs.statSync(root+'firmware/build/xiaozhi.bin').size;const used=fs.statSync(root+'firmware/build/generated_assets.bin').size;const ota=0x370000,cap=8*1024*1024;console.log('xiaozhi.bin = '+fw+' B');console.log('ota_1 remaining = '+(ota-fw)+' B = '+((ota-fw)/1024).toFixed(6)+' KiB');console.log('generated_assets.bin = '+used+' B');console.log('assets remaining = '+(cap-used)+' B = '+((cap-used)/1048576).toFixed(6)+' MiB');"
ota_1,    app,  ota_1,   ,          0x370000,
assets,   data, spiffs,  0x800000,  8M
xiaozhi.bin = 3294896 B
ota_1 remaining = 309584 B = 302.328125 KiB
generated_assets.bin = 2790863 B
assets remaining = 5597745 B = 5.338426 MiB
```

**基线 B · fonts_root 三枚现资产尺寸**：

```text
$ ls -la /c/AI/KidPal-pod-m2/firmware/main/assets/fonts_root/font_body.bin /c/AI/KidPal-pod-m2/firmware/main/assets/fonts_root/font_num_baloo.bin /c/AI/KidPal-pod-m2/firmware/main/assets/fonts_root/font_title_zcool.bin
-rw-r--r-- 1 15802 197609 248252 Oct 13 09:35 /c/AI/KidPal-pod-m2/firmware/main/assets/fonts_root/font_body.bin
-rw-r--r-- 1 15802 197609    964 Oct 13 09:35 /c/AI/KidPal-pod-m2/firmware/main/assets/fonts_root/font_num_baloo.bin
-rw-r--r-- 1 15802 197609   9060 Oct 13 09:35 /c/AI/KidPal-pod-m2/firmware/main/assets/fonts_root/font_title_zcool.bin
```

**基线 C · cbin 机制在编 + fonts_root 生产零消费者**：

```text
$ git -C /c/AI/KidPal-pod-m2 grep -n "fonts_root" -- firmware/main ':!firmware/main/test'
production fonts_root grep exit=1 (1 means zero hits)

$ git -C /c/AI/KidPal-pod-m2 grep -n -E "cbin_font_create|LvglCBinFont|GetAssetData\(\"index\.json\"|esp_partition_find_first.*\"assets\"" -- firmware/main/assets.cc firmware/main/display/lvgl_display/lvgl_font.cc
firmware/main/assets.cc:52:    partition_ = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "assets");
firmware/main/assets.cc:110:    if (!GetAssetData("index.json", ptr, size)) {
firmware/main/assets.cc:158:            auto text_font = std::make_shared<LvglCBinFont>(ptr);
firmware/main/assets.cc:275:            auto text_font = std::make_shared<LvglCBinFont>(ptr);
firmware/main/display/lvgl_display/lvgl_font.cc:5:LvglCBinFont::LvglCBinFont(void* data) {
firmware/main/display/lvgl_display/lvgl_font.cc:6:    font_ = cbin_font_create(static_cast<uint8_t*>(data));
firmware/main/display/lvgl_display/lvgl_font.cc:9:LvglCBinFont::~LvglCBinFont() {
```

**基线 D · spec 锚现状**：

```text
$ node /c/AI/KidPal-pod-m2/.claude/scripts/spec-anchor-check.mjs
{
  "specAnchorCheck": "forward",
  "design": "kidpal-ui-design-v8.html",
  "designIdCount": 64,
  "archSectionCount": 48,
  "storyCount": 14,
  "anchorLines": 36,
  "errors": [],
  "ok": true
}
```

### landing 前（pre-commit）

1. **容量与现资产基线复核**：重跑基线 A / B / C。若构建产物未变，预期逐字为上述数值；若落地前构建产物已变化，以落地实跑覆写本节与 T2 / ADR-017 对应数字，并在 §7 留痕，禁止沿用旧数。基线 C 的负向零消费者断言与正向 cbin 机制命中必须同时成立，防止空扫描假绿。
2. **spec 锚完整性**：`node .claude/scripts/spec-anchor-check.mjs` → 预期 `ok: true`、`errors: []`；本批不改小节编号，`archSectionCount` 预期维持 48。
3. **PROPOSAL 结构门**：主线程落盘草稿后跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261028-arch-review-font-typography-scaleup/PROPOSAL.md` → 预期 `ok: true`、`findings: []`，`anchorUnion` 恰为 {`docs/architecture.md`, `docs/decisions/ADR.md`}，与 landing-manifest 集合相等，无加粗锚行或注记夹带路径。起草会话按派发要求零写文件，故本项是 FROZEN 前必跑，未伪报已跑。
4. **共享锚文装配器**：以 §2 的 7 个编辑点生成 JSON 清单（T1×1、T2×1、T3×3、T4×1、T5×1），跑 `node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>` → 预期每个 oldStr 恰命中 1 次，7/7 全部命中后才产副本；输出仅含 `docs/architecture.md` 与 `docs/decisions/ADR.md` 两件，输出根在仓外。主线程在 FROZEN 前实跑并保留关键输出。
5. **契约与需求零触**：`git diff --exit-code -- docs/requirements.md docs/contracts/interface-contract.md` → 预期退出码 0、无输出；正向落点由第 4 项两件目标文件证明，避免只查禁改面形成真空绿。

### landing 后、commit 前与 post-commit

6. **承重引文 byte 对账**：`node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261028-arch-review-font-typography-scaleup/PROPOSAL.md` + landing-verifier 人读 diff → T1 一段 replace、T2 一段 replace、T3 三段 replace、T4 一段 ADR append、T5 一行 insert，实际落地与 FROZEN 文本一致，无夹带。
7. **ADR append-only**：landing 后、commit 前跑 `git diff --numstat -- docs/decisions/ADR.md` → 删除列必须为 0、新增列大于 0；post-commit 可用 `git diff HEAD^ HEAD --numstat -- docs/decisions/ADR.md` 复核同一判据。正向断言 `git grep -c "^## ADR-017 ·" -- docs/decisions/ADR.md` = 1；负向断言 `git grep -c "^## ADR-018 ·" -- docs/decisions/ADR.md` = 0。
8. **旧文残留与新机制正向配对**：四个旧可读性断言字面值在 architecture 当前节中均为 0（历史变更记录与 debt 归档不在清扫范围）；同时 §5.5.3 必须各至少一次含 `Assets::GetAssetData`、`cbin_font_create`、`font_puhui_14_1`、`16px 正文全量 CJK`、`20px 阅读标题全量 CJK`，§12 恰有 1 行以 `| 261028-font-typography-scaleup |` 开头。
9. **落地后全量复跑**：第 1–5 项不得窄化；再由 landing-verifier 跑结构门与逐文件人读 diff。P0 的真机字形、缺资产降级和新增字体尺寸不是本架构批的 post-commit 判据，归其实现包验收。

---

## 6. 留根清单

- **16px / 20px 新字体精确尺寸**：本批只有上游 16px 全量约 324KiB 的探测估算，未生成新资产，未伪称精确实测；P0 必须用同一 `lv_font_conv` 产线实产后回填实际尺寸并复核 ota_1 / assets 余量。若实测推翻容量结论，回 `/arch-change`，不在实现包内静默改路线。
- **P0 spike**：资产打包、cbin 装载、host 同源、真机 1bpp 看屏与缺 assets 降级是后续实现验收主战场；本批文档 GREEN 不等于设备通路已验证。
- **实验字体**：spine q2 已裁不退役，不能为腾 ota_1 空间顺手删除九档实验页或测试字体。
- **历史句保护**：requirements NFR-READ-1 的「实验页已撤」历史句按 spine q6 不改；本批只修 architecture 当前态三处。
- **debt**：ARQ-260831 已 closed 并归档，本批零账本动作。
- **实现分派**：P0 → P1/P2/P3 的拆包、launch 前置与真机轮由 portfolio-planner / planner 按 frozen workstream 执行，本批不产实现代码。

**ADR-017 落地附件**（packet-bound landing 逐字 append 到 `docs/decisions/ADR.md` 尾）：

```text
## ADR-017 · 字体落位 = assets 分区 `.bin` + cbin 运行时装载 + 编译内 14px 兜底

- **日期**：2026-10-28（`261028-font-typography-scaleup` 架构面 Track B；决策输入 = frozen workstream F-5 / F-6 / F-7）
- **决策**：① 新增 16px 正文与 20px 阅读标题两枚全量 CJK 字体，经 `lv_font_conv` 产出 `.bin` 并打入 assets 8MB 分区；设备运行时由 `Assets::GetAssetData` 取数据，经 `LvglCBinFont` / `cbin_font_create` 装载，host 与设备读同一文件。② 编译内 `font_puhui_14_1` 保留为缺资产或装载失败时的兜底，保证不黑屏；键帽条 / 辅助条 / 查字窗仍用 14px。③ 20px 标题不做静态子集，缺字回退 16px；九档实验页与测试字体维持编入、不退役。④ 本决策不重排分区表、不引入 OTA 字体下发，旧三枚 `fonts_root` 资产不因此被追述为已完成生产接线。
- **理由**：2026-10-28 实测 ota_1 容量 3,604,480B、当前 `xiaozhi.bin` 3,294,896B，余量仅 309,584B（302.328125KiB）；上游需求批探测给出的 16px 全量估算约 324KiB，已超过该余量，且本方案还必须保留编译内 14px 兜底并另增 20px 标题，编入固件没有可持续余量。相对地，assets 8MB 分区当前镜像 2,790,863B、余量 5,597,745B（5.338426MiB），足以承载资产化路线。现有 assets mmap 与 cbin 机制已在编，选择它只需由 P0 打通本板字体消费，不必新造装载格式；host / 设备读同一 `.bin` 也消除字体数据双源。16px 精确尺寸与 20px 尺寸均待 P0 实产复测，本 ADR 不把上游估算冒充精确实测。
- **备选与放弃原因**：① 新字体编入固件——放弃：实测 ota_1 余量 309,584B 小于上游 16px 全量约 324KiB 的估算，更无法同时容纳 20px 与 14px 兜底。② 字体放 SD / TF 卡——放弃：assets mmap + cbin 已是现成的内部 Flash 通路，转 SD 会增加挂载与拔卡状态依赖，且缺资产降级仍须另做；本批不为字体再引入一个存储域。③ 20px 使用静态子集——放弃：阅读标题是动态文本，子集必遇缺字，human 已裁全量 CJK + 16px 回退。④ 删除 14px 兜底或九档实验字体以腾 app 空间——放弃：前者破坏缺资产不黑屏底线，后者违反 spine q2 的明确裁定。
- **关联**：requirements v1.69 NFR-READ-1 / S-F2.1 / S-F2.3；arch §5.5.2 / §5.5.3；spine = `delivery/intake/261028-font-typography-scaleup.md`；frozen workstream = `delivery/workstreams/261028-font-typography-scaleup/PROPOSAL.md` F-5 / F-6 / F-7；实现前置 = P0 `font-assets-runtime-loading` spike；代码机制锚 = `firmware/main/assets.cc`、`firmware/main/display/lvgl_display/lvgl_font.cc`、`firmware/managed_components/78__xiaozhi-fonts/src/cbin_font.c`。
```

---

## 7. 评审记录

（判官 append-only 区；待 architect-reviewer 四查、判档与契约分类。主线程不预判结论。）

---

### R1 · 2026-10-28 · architect-reviewer（独立审稿；只读 canonical，唯一写入 = 本节）

**verdict: GREEN**

**查一 · 意图保真：PASS。** T1↔faceMap 架构行逐条对应（§5.5.3 管线换代 + 两族扩容 + 运行时装载路径 / §5.5.2 预算注记 / 三处勘误 :121/:158/:436 / ADR-017）；:301 旧句由 T1 整段替换覆盖，与 faceMap「三处过时表述」列举自洽。F-5 / F-6 / F-7 语义逐条承接（assets `.bin` + cbin + 编译内 14px 兜底 / 20px 全量 CJK + 16px 缺字回退、不做静态子集 / host 与设备同一 `.bin`）。夹带扫描：imgfont 句为现文原样保留；「九档实验页不退役」= spine q2；「键帽条/辅助条/查字窗 14px」= F-1——均非新增语义。§4「无新增 OQ」成立：六项引用（workstream F-5 / F-6+OQ1 / F-7、spine q2 / q6、契约零触点）逐一到源文件核实，均有既裁依据。

**查二 · 纪律合规：PASS。** T1–T5 零改节编号与标题；spec-anchor-check 本审独立实跑 `ok: true`、`errors: []`、`archSectionCount` = 48 维持。分工句在位：T1 新文明写「字号、行数与页面容量的需求阈值归 requirements.md，本节只承载机制」；决策本体归 ADR-017（§5.5.3 不承载理由长文）；契约面零触（`数据格式归 contracts/` 无涉）。ADR-017 五小节齐（日期/决策/理由/备选与放弃原因/关联），与 ADR.md 头部格式规定一致。「机制在编 ≠ 通路已接」如实承载：fonts_root 生产零消费者本审独立复扫（`git grep "fonts_root" -- firmware/main ':!firmware/main/test'`）exit=1 复现；新文「同源接线归 P0」、ADR-017 ④「旧三枚不因此被追述为已完成生产接线」、§6 留根「文档 GREEN 不等于设备通路已验证」、§5.9 P0 真机判据不入本批 post-commit——四处对冲在位。

**查三 · 撞车复核：PASS。** `^## ADR-` 独立复扫末条 = ADR-016、ADR-017 零命中；ADR-001..016 无字体落位决策条目。ADR-012（TF 卡白名单）仅承载诊断日志，与 ADR-017 备选②（字体不上 SD/TF）不冲突、不互斥。T2 追加注记自带日期与 changeId 标识，与既有 ⚠️「预算口径实测于 art 切分前」估算口径可分辨、不自相矛盾（旧句为切分前估算、新注记为当期实测余量）。T3 风险 10 闭账行「**已闭账**：…｜已闭（原 owner …）」与 §8.1 既有行 3 / 行 9 先例同构；T5 新行四列与 §12 既有行同构、依据列无 FROZEN 占位 token。分区事实自洽：art 512KB（16m.csv:11）与 assets 8MB（:12）双分区与 §5.5.2 现文一致。

**查四 · 牵连面完整：PASS。** ① 反向锚：requirements.md:679「支撑：arch §5.5.3（字体管线）」——T1 保留标题原样、零重编号，锚稳定规则不触；§3.1 / §4 / §8.1 均为行内容替换，锚行引用零动。② 「未实测」退役清扫完备性：本审独立复扫（窄模式 + 扩大「未实测」模式两层），architecture.md 命中恰为 :121 / :158 / :301 / :436 四处、扩大模式零新增命中，T1(:301) + T3(:121/:158/:436) 全覆盖申报属实。③ landing-manifest 围栏块 {docs/architecture.md, docs/decisions/ADR.md} 与 §2 目标文件并集相等（⊆/⊇ 双向核对）。④ ARQ-261021 实跑回填纪律：基线 A–D 本审独立复跑逐字吻合——ota_1 = 0x370000（:7）、assets = 8M（:12）、xiaozhi.bin = 3,294,896 B、ota_1 余量算术复核 3,604,480 − 3,294,896 = 309,584 B = 302.328125 KiB、generated_assets.bin = 2,790,863 B、assets 余量 5,597,745 B = 5.338426 MiB、三枚现资产尺寸（248,252 / 964 / 9,060 B）逐字节一致、基线 commit 429643b3 与 HEAD 一致。「324KiB」诚实处理：T4 证据 / ADR-017 理由 / §6 留根三处一致标注为上游需求批估算（workstream review.md:14 核实），精确尺寸归 P0 实产回填、不冒充实测。

**非阻断注记（两条，不构成否回）**：(1) T1 新文「不追述旧三枚已完成运行时接线」句法压缩、可读性弱于 ADR-017 ④ 的「不把旧三枚追述为已完成生产接线」——建议落地编辑时顺手改写为后者语式，语义不变。(2) T2 新文「详见 …。」与「**字体预算注记」之间存在一个空格，排版级，可不改。

**判档：主路径（Track B）。** 轻路径判据逐条：① 零语义变化——不满足（§5.5.3 字体管线机制描述换代 = 机制/行为语义变化）；② 改动性质——不满足（非错字/排版/表述澄清/锚行格式/指针刷新级）；③ 不触 `docs/contracts/**`——满足；④ 不触 §5.x——不满足（§5.5.2 / §5.5.3 内容改写）。判据须全部满足，①②④任一即否定 → **主路径**。Track A 段不触发：方向已收敛——human 立项 ack + workstream FROZEN v1.0（human 触点②原话在案）+ 需求面 v1.69 frozen；方向级取舍（F-5 落位、F-6 全量子集）均已裁，无未裁根本分叉。

**契约四分类：`NO_CONTRACT_CHANGE`（独立裁定，与起草方建议一致）。** 本审独立复跑契约关键字 grep（`字体|字号|px|每格|容量|font` 于 `docs/contracts/interface-contract.md`）exit=1 零命中复现；本批零 wire / shim API / 课表 MD / display 响应块形状变化，格容量属板端渲染容量。反证检查过：无需 adapter/mapper、无现有扩展字段可放、无本地命名撞 canonical、无二期能力前塞、无既有形状可复用。实现期若发现容量/字体信息需上网线，按逃生门转四分类，不在 Pod 内自裁（PROPOSAL §3 已如实申报同一口径）。

**结构字段核对：** landing-manifest 围栏块与 packet-id 行在场（GREEN 前义务满足，最终期限 FROZEN）；结构门 preflight 已绿（anchorUnion = 两文件，与本审核对一致）。§5 验证集 pre / landing-after / post 分层完整，编辑点计数 7（T1×1 + T2×1 + T3×3 + T4×1 + T5×1）与 §2 改法集一致。

**结论：GREEN + 判档 = 主路径。** 无否回项。主线程可凭「立项 ack + 本 GREEN」标 FROZEN 并进入 packet-bound 落地；落地期实跑若与 §5 基线有字节差异，按 ARQ-261021 以落地实跑覆写并在本节留痕。

**主线程 FROZEN 前折入记录（2026-10-28）**：R1 两条非阻断注记已在 FROZEN 前折入改法文本——T1「不追述旧三枚已完成运行时接线」→「不把旧三枚追述为已完成运行时接线」（ADR-017 ④ 同语式）；T2 注记句号后冗余空格删除。折入属措辞级、零语义变化，未触四查结论。
