# architect-landing-summary · 260906-gov-review-architect-loop-advisory

packet-id：FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集 · 评审轮次 R1 GREEN（四条 NIT 判官授权主线程标 FROZEN 时机械落字）· change-id 260906-gov-review-architect-loop-advisory

> 落地人：主线程（Track B 步骤 4，packet-bound）· 日期：2026-09-06
> packet = FROZEN PROPOSAL + landing-manifest + 本 summary（architect-landing-summary）
> 凭据 = human 立项 ack（「三个问题都同意你的推荐，立项走review」2026-09-06）+ OQ 三题全裁（OQ1=(a) / OQ2=(a) / OQ3=(a)）+ architect R1 GREEN

```landing-manifest
.claude/agents/planner.md
.claude/agents/architect.md
.claude/agents/fixer.md
.claude/references/architect-operating-model.md
```

> manifest 注：4 路径 = T1–T4 一 T 一文件，无共享锚、无新增文件；四落点全 `.claude/` 面，CANONICAL_TEST 可机锚（不踩 ARQ-260906-lfc-canonical-anchor-gap 缺口）。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 planner 开 architect 疑难分析档 | `.claude/agents/planner.md` | ✅ 已落：tools 行白名单加 `architect`（逐字段隔离：name/model/description 零触碰，V5 机判）；Process 新增 Step 10b `[ARCH-ADVISORY 疑难分析档]`（位置 = Step 10 后、Step 11 前），触发闭集三 + human、派发契约、中介留痕、效力声明原文四件齐 |
| T2 architect 补路由入口 + 消歧 | `.claude/agents/architect.md` | ✅ 已落：description Use when 枚举句内「ADR/rationale」后追加 advisory 分析项；When to Use 新增一条（含消歧与「转 planner + TaskPackage、不就地动手」）；Boundaries 节零新增（既有红线已覆盖） |
| T3 fixer 升级路径补 architect 档 | `.claude/agents/fixer.md` | ✅ 已落：Red Flags「需要人类判断业务取舍或架构方向。」句内扩写（架构方向类先 advisory 档、拿不住或涉业务取舍仍升 human）；其余条目零触碰 |
| T4 operating-model §3 路由表对齐 | `.claude/references/architect-operating-model.md` | ✅ 已落：§3 表 Architect 行 Owns 格追加 in-dev-loop advisory analysis 一项；Does not own 列零触碰 |

manifest 外零落地：契约面（`docs/contracts/**`）、`docs/spec.md`、自有实现面（firmware/server）、G3（verify-gates.json / scope-map.json / hooks / scripts）均未触碰（PROPOSAL §0 非目标①③）。

## 2. 验证证据（§5 重放，2026-09-06）

landing 前（执行侧自检）：

- **V1** `node .claude/scripts/route-regression-check.mjs` → `28 ok, 0 fail` exit 0 ✅（与起草期基线一致，白名单扩张未回退既有信号）
- **V2** `node .claude/scripts/route-regression-check.mjs --self-check` → `56 ok, 0 fail` + `all required signals/whitelists falsifiable` ✅
- **V3** `node .claude/scripts/lint-descriptions.mjs` → `scanned 13, excluded 1 (architect.md), flagged 1`（唯一 flag = landing-verifier.md over-length 既有 warn，基线一致，无新增）✅
- **V4** `grep -c "Agent(architect," .claude/agents/planner.md` → `1`（恰加一处）✅
- **V5** `grep -c "^model: opus$"` / `grep -c "^name: planner$"` → 均 `1`（frontmatter 逐字段隔离）✅
- **V6** 消歧短语 census：`advisory` planner.md = `8`（≥7 ✅，基线 4 + 新段 ≥3，判官 F4 口径）；`ARCH-ADVISORY` planner.md = `1` ✅；`advisory` architect.md = `2`（≥1 ✅）；`ARCH-ADVISORY` fixer.md = `1` ✅

landing 后（核验侧）：

- **V7** 结构门：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260906-gov-review-architect-loop-advisory` → 见下（本 summary 落盘后执行，结果Verifier 包装载）。
- **V8** 人读 diff 四件：planner.md 只动 tools 行 + 新增 Step 10b 段；architect.md 只动 description 行 + When to Use 新增一条；fixer.md 只动 Red Flags 一行扩写；operating-model.md 只动 §3 表 Architect 行 Owns 格——四件 diff 均与 FROZEN §2 改法逐字对应（landing-verifier 逐文件人读核对）。
- **V9** Track A 判据复核：manifest 四路径全 `.claude/` 面，零 `docs/contracts/**`、零实现面、零 G3——机械枚举断言。

## 3. 诚实注记

1. **NIT 落字（判官授权，机械修订）**：R1 四条 NIT 已随标 FROZEN 落字——「五段→四段」（头部承载 ARQ 行）、「十条→八条」（§1 问题类 B）、「:54→:56」（T4 锚）、「≥4→≥7 + 基线口径括注」（§5 V6）；R1 评审记录原文引述处未回改（append-only 评审区保持原貌）。
2. **行号锚说明**：T1 新段编号取 Step 10b（插在 Step 10 与 Step 11 之间，不重排后续步骤号）；FROZEN PROPOSAL 表述为「新增一段（位置 = Step 10 之后、Step 11 之前）」，10b 编号与该位置约束一致，且对正文既有行号引用（:57/:58/:66 等在插入点之前）零扰动（判官行号漂移专项已核）。
3. **零脚本/G3 变更**：本批纯 prose/白名单修订，无任何脚本、钩子、scope-map、verify-gates 改动；`route-regression-check` / `lint-descriptions` 仅作验证集运行对象。
