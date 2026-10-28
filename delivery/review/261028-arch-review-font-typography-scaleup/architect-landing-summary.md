# architect-landing-summary · 261028-font-typography-scaleup

packet-id：FROZEN PROPOSAL §2 改法集（T1–T5）+ §5 验证集 · 评审轮次 R1（GREEN，无否回；两条非阻断注记已 FROZEN 前折入） · change-id arch-review-261028-font-typography-scaleup

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 §5.5.3 字体管线整段替换 | docs/architecture.md | 三段新文落位（assets .bin + cbin 运行时装载 + 14px 兜底；机制在编 ≠ 通路已接对冲句在位） |
| T2 §5.5.2 字体预算注记追加 | docs/architecture.md | 段尾字体预算注记追加（assets 余量 5,597,745B 基线；精确尺寸归 P0 复测） |
| T3-1 §3.1 可读性行勘误 | docs/architecture.md | 14px 下限定案 + 复测表述替换「未实测」 |
| T3-2 §4 NFR-READ-1 映射行勘误 | docs/architecture.md | assets .bin + cbin 装载口径替换「SPIFFS + 待实测」 |
| T3-3 §8.1 风险 10 闭账 | docs/architecture.md | 「已闭账」行保留历史索引，ARQ consumed-by 结项指针在位 |
| T4 ADR-017 尾部 append | docs/decisions/ADR.md | ADR-017 五小节完整追加（决策/理由/备选放弃/关联）；既有 ADR-001..016 零改 |
| T5 §12 changeId 行插入 | docs/architecture.md | `261028-font-typography-scaleup` 行置顶，格式与既有行同构 |

落地方式 = 共享锚文装配器（assemble-edits.mjs，7 编辑点恰 1 命中 7/7）产仓外副本 → 主线程 packet-bound 复制进 canonical；装配前后 sha256 见 assemble 输出（architecture ce44fe0e…→6986dc73…、ADR c3303f2f…→713e23b1…）。

## 2. 验证证据

- **§5.1 容量基线复核（landing 前）**：重跑基线 A——xiaozhi.bin = 3,294,896 B、ota_1 余量 309,584 B；generated_assets.bin = 2,790,863 B、assets 余量 5,597,745 B——与 FROZEN §5 基线逐字一致（构建产物未变）；基线 C fonts_root 生产 grep exit=1（零消费者）复现。
- **§5.2 spec 锚**：spec-anchor-check ok:true、archSectionCount = 48（编号零变动，landing 前后各跑一次均绿）。
- **§5.3 结构门**：landing-fidelity-check --preflight ok:true、findings []、anchorUnion = {docs/architecture.md, docs/decisions/ADR.md} = manifest。
- **§5.4 装配器**：7 编辑点全部恰 1 命中后产副本（fail-loud 未见触发）；输出仅含两件目标文件。
- **§5.5 零触断言**：git diff --exit-code -- docs/requirements.md docs/contracts/interface-contract.md 退出码 0。
- **§5.6 承重引文对账**：--cite-check 13 处引文——6 处「新文」全部命中 canonical；5 处「现文」missing = 恰为被替换/插入的旧文（落地成功后必然形态；工具 v1 对「替换前引文」不区分 replaced 状态，advisory 性质不进门链）；T4 落地锚现文（append 场景）仍命中。落地前的现文匹配由装配器 7/7 恰 1 命中独立证明。
- **§5.7 ADR append-only**：git diff --numstat = `8 0 docs/decisions/ADR.md`（删 0 增 8）；`^## ADR-017 ·` 计数 = 1、`^## ADR-018` = 0。
- **§5.8 残留/正向配对**：四个旧可读性断言字面在 architecture.md 计数 0；§5.5.3 新机制关键词（Assets::GetAssetData / cbin_font_create / font_puhui_14_1 / 16px 正文全量 CJK / 20px 阅读标题全量 CJK）在位；§12 恰 1 行以 `| 261028-font-typography-scaleup |` 开头（全文件命中 3 处含 §12 行内引用与历史说明，行首形态恰 1）。
- **§5.9 全量复跑**：第 1–5 项 landing 后复跑无窄化（本 summary 记录值即复跑值）。

## 3. 诚实注记

- **cite-check ok:false 定性**：5 处「现文」missing 均为已被本批替换的旧文（非引文错误）；工具 v1 不带 replaced 状态语义，advisory 不进门链。落地前匹配由装配器断言承载。无未解释的 missing。
- **324KiB 为上游估算**：ADR-017 与 T2 注记均显式标注「上游需求批探测估算，精确尺寸归 P0 实产回填」——本批未生成新字体资产，未伪称实测。
- **「机制在编 ≠ 通路已接」**：canonical 新文明确「同源接线归 P0」，fonts_root 生产零消费者事实经三处对冲句承载（T1 新文 / ADR-017 ④ / §6 留根）。
- **ADVISORY 披露（枚举）**：R1 非阻断注记 2 条（T1 句法压缩改写、T2 冗余空格），均已 FROZEN 前折入改法文本并在 §7 留痕；无其它 WARNING/ADVISORY。

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
```
