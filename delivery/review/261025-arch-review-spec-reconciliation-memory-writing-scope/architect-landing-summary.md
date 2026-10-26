# architect-landing-summary · 261025-spec-reconciliation-memory-writing-scope

packet-id：FROZEN PROPOSAL §2 改法集（T1–T9）+ §5 验证集 · 评审轮次 R1 GREEN（零否回，三条非阻塞备注 N1–N3）· change-id 261025-spec-reconciliation-memory-writing-scope

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 §6.7 写入范围总段换代（类级规则 + 内容归属三类 + 「记忆巩固」两义 + 两把杠杆实况与界限） | `docs/architecture.md` §6.7 | 原「实际边界由三件构成」①②③ 整段替换为四段；「类别优先于内容」优先序随第三类写入 |
| T2 实例枚举表 1/2/3/5 行换代 + 新增第 7 行 + 表前引导句与表后注记 | `docs/architecture.md` §6.7 | 表前引导句改为「类级规则的实例枚举与实测归属」；1/2 行按统一携 `user` 照常入库改写；3 行标为平台内部系统回合实例；5 行标为边界外第三类（家长侧）；第 7 行新增 dreaming 实例（生成器关闭、重开即复发）；表后注记补 Honcho 自身 dream 不是回合 |
| T3 新增「残余与未闭合申报」块（一）–（八） | `docs/architecture.md` §6.7 | 置于表后注记与「记忆身份」段之间，八条逐字落地，每条带事实与去向、不承诺已修 |
| T4 记忆身份段补一次性回合归属；画像边界来源门换代 | `docs/architecture.md` §6.7 | 记忆身份段尾追加一次性回合携 device 身份落同键族 + 降级指向残余（五）；来源门 ① 整句按类级规则与内容归属重写 |
| T5 `:192` §5.1.2 front 行一句替换（spine 后补漏列项）；`:203` 记忆子系统行核后不动 | `docs/architecture.md` §5.1.2 | front 行「不携 `user`（user 消息不入库）」整句替换为「携 `user=<device_id>` 落同一设备 session 键族」；记忆子系统行指针式表述经核仍有效，按判零改动 |
| T6 §4 NFR-PRIV-1 映射行换代 | `docs/architecture.md` §4 | 两把配置键口径替换为类级规则 + 内容归属 + 家长侧边界外 + 残余指向 |
| T7 ADR-016 决策② 带日期注记（正文零改） | `docs/decisions/ADR.md` | ADR-016 条目末尾追加一条 2026-10-26 注记行；决策文本一字未改 |
| T8 术语表新增「平台内部系统回合」词条 | `docs/glossary.md` | 置于「记忆子系统」词条之后，钉住三类回合互斥与「记忆巩固」两义指向 |
| T9 §12 变更记录表落本批 changeId 行 | `docs/architecture.md` §12 | 新行置顶（表头分隔行之后、261023 行之前） |

## 2. 验证证据

1. **§5 机械前置 `spec-anchor-check.mjs`**：落地后实跑 → `ok: true`、`errors: []`、`storyCount: 14`、`archSectionCount: 48`、`anchorLines: 36`（落地前基线同为 forward / errors 空，未因本批新增悬空锚）。
2. **`landing-fidelity-check.mjs --preflight`（只读干跑）**：`ok: true`、`section2BodyFound: true`、`anchorCount: 3`、`anchorUnion` = `docs/architecture.md` / `docs/decisions/ADR.md` / `docs/glossary.md`、`advisories: []`、`findings: []`——§2 锚清单形态可解析且与 manifest 并集一致（该脚本自陈只验形态、不做 §2↔manifest 对账，对账归 landing-verifier）。
3. **陈旧断言 census 复核（落地后实读）**：`docs/architecture.md` 中 `noisePatterns` 命中数 **1**（仅存 T1 的退役事实句，PROPOSAL §3 预测值 = 1）；`captureSystemRuns` 命中 **2**（T1 两把杠杆段 + 表第 3 行）；「不携 `user`」在全仓 live 面仅剩残余（五）的降级描述一处（该处语义正确：指 `device_id` 缺席时的诚实降级，非旧口径）。
4. **改动面 = manifest 三文件**：`git diff --stat` 实测 `docs/architecture.md` +33/−10、`docs/decisions/ADR.md` +1/−0、`docs/glossary.md` +1/−0，无第四个文件被动。（初版 summary 误记为「+43/−10」——把 `--stat` 的图宽数字读成了插入数；经 landing-verifier ADV-1 指出后按 `--numstat` 更正如上。）

## 3. 诚实注记

- **评审非阻塞备注 N1 / N2 未回改 PROPOSAL**：N1 指 §3 census 中「一次性回合」的行位写作 `:192`（实际精确子串在 `:155`，`:192` 是「一次性 LLM 回合」）；N2 指 T9 引「表头行 `:488`」实为 `:487`。两处均为引用行号笔误，落地动作按语义正确执行（T5 改的是 `:192` 的 front 行、T9 新行插在表头分隔行之后），不影响落地保真。
- **T8 属派发清单外增量**：起草时 architect 主动加入（理由 = 类名不落词条会留三义漂移面），architect-reviewer 特裁「收」，故保留在 manifest 内。若判官认为应收窄，裁掉该条并同步删 manifest 的 `docs/glossary.md` 行，其余 T 项不依赖它。
- **目录名与其 changeId 双轨（机械偏离，已在 PROPOSAL 头部留痕）**：派发路径原为 `delivery/review/261025-spec-reconciliation-memory-writing-scope/`，该目录名不含 `arch-review` 写槽 token、被 `enforce-scope` 拦截；实际落盘 `delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/`。changeId 仍为 spine 的 `261025-spec-reconciliation-memory-writing-scope`，§12 变更记录行与 spine 引用均指向合规目录。此形态在仓内有先例（261023 批对 261022 spine）。
- **未落项：无**。T1–T9 全部落地，无留根项。
- **ADVISORY：无**（两道机检 `advisories` 均为空数组）。

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
docs/glossary.md
```
