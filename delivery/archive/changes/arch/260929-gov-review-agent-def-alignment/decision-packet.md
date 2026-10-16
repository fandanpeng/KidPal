# Decision Packet · R1 裁定 · 260929-gov-review-agent-def-alignment

> adjudicator · 2026-09-29 · 本次 confirmed 治理改法 delta，交 human 批。**不落 canonical**：human 批后由 architect 回灌 PROPOSAL 定稿（只机械回灌已批改动、不夹带新设计），human 盖 FROZEN，主线程 packet-bound landing。每项标注是否触自改递归 / 契约 / 强制层。R1 裁定基线：CONFIRMED 20 / DISMISSED 1（F4）/ DEADLOCK 0。

## 一、CONFIRMED 改法 delta（按 T 项组织）

### T1 · docs/systems 清扫（F4 dismissed + F16 + F1 地板两件）

- **改写方向（F4 裁定后形态）**：维持 defender 方向——全部替换为 `docs/requirements.md` + `docs/architecture.md` 平铺表述，**语义 = 同一既有语义换路径**（草案落 canonical 两文件、freeze 归 review-moderator spec profile）。
- **地板（F4 让步采纳，landing-verifier 核对项）**：architect.md 新路径表述必须**原样保留**「写入不等于 spec 已冻结」与「正式 spec soundness / 冻结判定归 review-moderator 的对抗 review（spec profile）」两句（HQ-1 门/约束句，T7 不得 prune）。
- **F16 修正**：T1 证据「共 11 行命中」改 **10 行**（自列清单 3+4+3=10）。
- **F1 地板两件（随 E4 裁断执行）**：`task-intake-and-slicing.md:338`（`docs/systems/<S>/UIUX-DESIGN-BRIEF.md`，已列 manifest 文件内、T1 现范围未覆盖）；`route-fixtures/surface-alignment.json:7/:12`（scope-map allow 镜像，T2 改后必漂）——两件不入账则 T1/T2 自身半截。
- 触面：G2 def 文本；不触契约 / 强制层。architect.md 自身在 T7 清单内 = 自改递归主体（human 门已在场）。

### T2 · scope-map 写域对齐（F3 / F5 / OQ4 材料）

- **riskTier 理由句改写（F5）**：删「写域换路径属对齐非扩张」，改「三腿执法层扩权（integrator / architect / review-moderator：现拦→后放，修复 260913 重构静默破坏的成文职责所必需）+ 一腿 deny 惰性（adjudicator：allow 非空、docs 根文件本就 allow-miss 被拦，deny 只改报错文案）+ OQ3 一腿收权（若裁收窄）」。
- **§5 增编号条目（F3，正反双向矩阵）**：构造钩子 stdin JSON 喂 `node .claude/hooks/enforce-scope.mjs`——① integrator → `docs/requirements.md` → exit 0；② integrator → `docs/architecture.md` → exit 0；③ integrator → `docs/contracts/**` → exit 2（deny 腿仍咬）；④ OQ3 裁收窄后 architect → `docs/contracts/**` → exit 2；⑤ adjudicator → 任一 docs 根文件 → block（allow-miss 惰性实证）。
- 触面：**G3 强制层**（OQ4 必答，human 放行后才可 FROZEN）；不触契约。

### T3 · builder 包边界（F8 / F16）

- description / :17 包边界改写 + :56 删 zod 句（PROPOSAL 原方向不变）；执行加两条硬后盾：diff 按 authoring-agents-skills 纪律 3 **逐字段隔离**人读；§5.6 如实标注 route-regression-check 对 builder **零覆盖**（fixture 补行见问题集 Q-F8）。
- 触面：G2 def 文本。

### T5 · S2/K8 死条文（F2 + 判官 delta）

- OQ2=(a) 时**全链同删**，联动清单在 PROPOSAL 原清单上补：`build-feature.md:21`（拆包侧 s2 声明指令整段）+ `build-feature.md:30`（S2 official scan 指令）+ `dev-loop.md:174`（字段定义全文）+ `task-intake:412`（起包 convention 例证句）+ **`planner.md:193-195`（s2ExploratoryRequired/Reason/Goals 示例字段三行——判官 grep 实证 delta，PROPOSAL 与 challenger 均未列）**。
- B3 结论随录：s2Exploratory / run_inline_check 无脚本 / 闸消费（scripts / hooks / verify-gates 零命中），删除无闸红风险。
- 触面：G2 def + G2 reference + G2 command；不触强制层。

### T6 · uiux-author 编制（F9）

- OQ1=(a) 时执行清单按 **7 处**重列：planner.md:4（frontmatter Agent 列表）/ :61（Step 6b）、acceptance-author.md:23、task-intake:320 / :326、dev-loop.md:55 / :68 + `grep -rn "uiux-author" .claude/` 兜底；OQ1=(a) 重写 6b 时对同句 `uiux-designer` 引用（def 不存在、DEV-GOVERNANCE §8「dispatch 尚未接线」在册）带「未接线」限定词或去引用；G1 编制表行本批不动留根。
- 触面：G2 def + reference；不触强制层。

### T7 · 过程注记清理（F6 / F12 / F13 / F16 / F19）

- **F6**：§0 非目标改「story-author / requirement-reviewer / scenario-author / challenger / landing-verifier 五个干净 def 除 T1/T4 点名行**与 T7 的 description 收短（条件：仍超 lint 阈）**外不动」。
- **F12（HQ-1 硬条款，进执行纪要）**：defender.md:47 **trim-not-delete**——留 must-never 禁令句（「必须用 Write 工具落盘，禁止用 Bash 重定向写」）、去括注历史教训；review-moderator:12/:104、adjudicator 卡点 D 注记同理逐条按 HQ-1 判据判；§5.8 补「按 ARQ-260929-agent-def-provenance-noise 散文清单逐条销账」为核对输入。
- **F13**：path-form-family 八类形态复扫从 §6 留根**升 §5 编号硬前置**（引号分段 / 转义 / 相对链接形至少覆盖）。
- **F16**：T7/§3 def 计数 12 改 **13**（charter §0 同误，随 charter 一并记）；census 复跑表述不变。
- 触面：G2 def 文本 ×13；architect.md = 自改递归主体（human 门在场）。

### T8 · 能力透镜锚（F15）

- OQ5 定稿时透镜行与 acceptance-author 既有承载（:36 红基线步、:45-46 引 DEV-GOVERNANCE 验收纪律 §2/§3）**显式去重**（引用或收编、不并列两套）；§5.9 验证锚改「OQ5 定稿措辞固定子串」或「Step 6 旁新增行在场」行计数。
- 触面：G2 def 文本。

### OQ3 / F10 / F18 · 契约写域收窄

- **OQ3 范围待 E3 裁断扩展**：architect.allow 的 `docs/contracts/**`（PROPOSAL 既列）+ review-moderator.allow 的 `docs/contracts/**`（F10，同性质 approved-landing 遗留、review-moderator.md:90 自述不改契约）。
- **F18 论证补录进 OQ3 条目**（human 裁断在案）：OQ3(a) 收窄后 architect 契约预审合法写面仍有落点——`delivery/changes/arch/*arch-review-*/**` 与 `*gov-review-*/**` 保留（scope-map:88）、authoring 面 `.claude/references/**` 在 G2 五面内、契约本体恒 human 门 + 主线程 atomic PR。OQ3(a) 方向安全。
- 触面：**G3 强制层（收权方向）**；不触契约本体。

### ARQ 证据段修正（F16，定稿阶段执行）

ARQ-260929-agent-def-reality-drift：「16 行」改 14、:88 错标 architect.deny 改 architect.allow；ARQ-260923：「:54 Step 6b」改 :61。由定稿 / landing 阶段按批内修复执行，本裁定不代改。

### manifest 增补件候选清单（按问题集裁断取舍）

| 候选件 | 触发裁断 | 说明 |
|---|---|---|
| `.claude/commands/build-feature.md` | E4=a 且 OQ2=(a) | T5 联动（s2 声明 + scan 指令） |
| `.claude/references/dev-loop.md` | E4=a 且 OQ2=(a) | T5 联动（字段定义 :174） |
| `.claude/commands/arch-change.md` / `review-arch.md` | E4=a（先答 E1） | spec-lane 草案流改写，与 E1 语义答案耦合 |
| `.claude/references/arch-review-protocol.md` / `architect-operating-model.md` / `security-hardening.md` | E4=a | docs/systems / zod 同构残留 |
| `.claude/skills/brainstorming/SKILL.md` | E4=a | docs/systems 残留 |
| `.claude/route-fixtures/surface-alignment.json` | E4=a（T2 联动，地板件） | scope-map 镜像同步 |
| `.claude/route-fixtures/routing.json` | Q-F8=补牙 | builder / planner / landing-verifier 三行 |
| `.claude/agents/uiux-author.md` | OQ1=(b) | PROPOSAL 原条件锚（新增） |

## 二、human 问题集（E1–E5 + OQ1–OQ5 合并，逐项裁决）

| 问 | 选项 | 推荐 | 理由一句 | 触面标注 |
|---|---|---|---|---|
| OQ1 uiux-author 编制 | (a) 摘引用降级（L2 面「直翻 + human design review」）/ (b) 补 def | **(a)** | k3 实走形态已验证、零新编制；UI 母版在 docs/ui/，真需要时再立 | 否（G2 文本） |
| OQ2 S2/K8 死条文 | (a) 删除（含 F2 全链联动）/ (b) 保留改写 | **(a)** | 本仓无对应工具，死条文诱导找不存在的工具；全链同删防半截 | 否（G2 文本） |
| OQ3 architect 契约写面 | (a) 本批收窄（含 F18 论证）/ (b) 留根批次 2 | **(a)** | 收权方向、无现行合法用途、一次清干净；landing 通道恒主线程不受影响 | **G3 强制层（收权）** |
| OQ4 G3 写域变更确认（必答） | 按 F5 修正口径放行与否 | 按修正口径放行 | 三腿扩权系修复实害所必需、有 DEV-GOVERNANCE §2 成文职责背书；deny 惰性与收权腿如实呈现 | **G3 强制层** |
| OQ5 能力透镜锚 | (a) 两处都加 / (b) 只 acceptance-author / (c) 本批都不加 | **(a)** | 返工证据最厚且复发 ≥2 次；措辞去重由 F15 承载 | 否（G2 文本） |
| E1 spec 草案落点（F4 裁定后为**确认项**） | 默认 = 维持现行成文语义（草案写 canonical 两文件、freeze 归 review-moderator spec profile、T1 保留 freeze 两句）/ 备选 = 重新设计草案通道（arch-review 证据目录或 scratch，= spec-lane 设计变更另立项） | **默认** | 现行成文已载答案（architect.md:53 / review-arch.md:20 / operating-model:79 ARQ-110 human 已接受）；重新设计不属本对齐批 | 默认项间接关联 G3（architect.allow 语义）；备选项另立项 |
| E2 agent 存在性机检归属 | (a) 本批评审提级立 ARQ（owner = architect，触发 = 下一批触 planner / Agent() 白名单的治理批）/ (b) 交 /loop-retro | **(a)** | policy-as-code 方向（route-regression-check 补牙）与 routing.json `_doc` 在册 DEFER 对齐；需明确触发防幽灵 DEFER | (a) 若同批做则 G3；登记动作否 |
| E3 review-moderator 契约写面 | (a) 同批并入 OQ3 收窄 / (b) 留根 owner+触发 | **(a)** | 同性质遗留、零成本顺车、同一次 human 过目，与 OQ3「一次清干净」自洽 | **G3 强制层（收权）** |
| E4 域外残留入账 | (a) 同批追加 T 项（地板两件必入 + commands/references 面先答 E1 再改写）/ (b) 立 ARQ 同车（owner+触发）/ (c) 扩本批 sweep 至全 `.claude` 面 | **(a)** | task-intake:338 + surface-alignment.json 两件不入账则 T1/T2 自身半截；半截代价 > 批边界膨胀代价 | (a) 扩 manifest；(b) 否；(c) 批范围扩 |
| E5 第五阀执行形态 | (a) R2/landing 核验由不加载被改 def 的独立会话 / harness 跑 / (b) 全部安排在新 canonical 生效前 | **(a)** | 被改 def 含全部对抗角色 + architect.md，harness 独立性最干净；R1 已属 (b) 形态合规，缺的是 R2/landing 显式落字 | **第五阀（高敏）** |

## 三、回灌执行顺序建议（供 architect 定稿参考）

1. 先裁 E1（确认项）与 E4/E3/E5（决定批边界与第五阀安排）→ 2. 按裁断整理 T1–T8 修正与 manifest 增补 → 3. OQ2/OQ4/OQ5 落字 → 4. §5 验证集并入 F3 矩阵、F13 硬前置、F15 验证锚 → 5. architect 回灌 + human 冻结 → review-moderator 核验 + `proposal-consolidation-check.mjs` → 主线程 packet-bound landing。
