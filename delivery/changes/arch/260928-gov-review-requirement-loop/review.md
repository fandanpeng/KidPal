# review · 260928-gov-review-requirement-loop（Track B 评审循环）

> 判官：architect（Track B 常驻轻路径判官，按 `.claude/references/track-b-judge-brief.md` 纪律执行）。
> 评审对象：`PROPOSAL.md` DRAFT v0.2（human 立项 ack 已获，OQ-1~4 已裁全按推荐位）。
> 评审日期：2026-09-29。纪律：只读全仓 canonical；本文件与 PROPOSAL §7 append 为唯一可写面。

## R1

### 一、引据抽验留痕（file:line 亲读核对，全部 2026-09-28/29 实读）

| PROPOSAL 引据 | 亲核结果 |
|---|---|
| `requirement-change.md:26`「story-author 起草——修订稿 + 完稿自检 + spec-anchor-check + 相邻 story 撞车报告，同会话 return 交主线程」 | 吻合（verbatim 逐字核对） |
| `requirement-change.md:28`「过即落稿——主线程写 requirements.md + §9 追加版本行。**无 human 终确认**…」 | 吻合 |
| `requirement-change.md:34`「**architect 可行性判断**（出场由 human 指定）」 | 吻合 |
| `requirement-change.md:58` 角色分工表 story-author 行「不裁定、不自评、无 Write」 | 吻合 |
| `requirement-change.md:61` 主线程行「派发 + 落盘 + §9 记账」 | 吻合 |
| `requirement-change.md:19`（档 0 主线程直改） | 吻合（§2 前言引据） |
| `story-author.md:3` description「修订稿以同会话 return 交主线程，本 agent 无 Write、不直接改 spec 面」 | 吻合 |
| `story-author.md:29`「同会话 return（本 agent 无 Write，不落任何文件）：修订稿全文 + 自检结果 + 撞车报告 + （档 2）牵连面清单。主线程负责转交 requirement-reviewer 与落盘。」 | 吻合 |
| `requirement-reviewer.md:3`「Use when: story-author 交回修订稿后要四查」 | 吻合 |
| `requirement-reviewer.md:25`「同会话 return（本 agent 无 Write）：结论 PASS / FAIL。」 | 吻合 |
| `scope-map.json:125-128` story-author 条目 `allow: []` / `deny: ["**"]` | 吻合（实读 :125-:128 逐字） |
| `requirements.md:7`「> 当前版本 **v1.59**；版本历史与逐次变更见文末 §9「变更记录」。」 | 吻合 |
| `AGENTS.md:26` 需求面变更节整行 | 吻合（逐字） |
| §0 差距 1「commands Glob 枚举 6 命令」 | 吻合（实跑 Glob：arch-change / loop-retro / review-arch / review-governance / triage-change / build-feature，恰 6 个，无需求面入口） |
| T5 先例形态「你现在是 planner（主线程编排者）」开场 | 吻合（`build-feature.md:8` verbatim「你现在是 **planner（主线程编排者）**。按 `.claude/agents/planner.md` 的规则推进。」） |
| T6 ② 锚点「§9 纪律引言定稿载体句」 | 吻合（`requirements.md:680` 实读，requirement-face 批 T4 增补句在场） |
| §0/T4 引 260928-gov-review-requirement-face 批 §3 强制层行自述 | 吻合（该批 PROPOSAL.md:103，转述如实；「实际登记已由该批后续落地为 deny-all 占位条目」与 scope-map 实况一致） |
| T1 根因引该批「以 return 换零 scope-map 触碰」 | 吻合（该批 §3 强制层行 + 非目标⑤） |
| T5 根因引该批非目标④「human 已裁 B 方案不新增命令入口」 | 吻合（该批 PROPOSAL.md:30 verbatim「④ 不新增命令入口（human 已裁 B 方案：判据落 reference，将来 triage 统一路由来消费）」）——见 F7 |
| OQ-2 承接该批 OQ-1 既有裁定（档 1 无 human 终确认） | 吻合（该批 PROPOSAL.md:112），承接非冲突 |
| 「260928 spine openQ Q4」形态 | 吻合（`delivery/changes/intake/260928-char-window-key-triggered-play.md:55-57`：human 未指定出场、主线程判定在先评估已覆盖、终拍随拍通过） |
| 「architecture.md 镜像句由主线程代写（spine 实录）」 | 吻合（spine:37「主线程落稿 requirements.md + architecture.md」） |

**结论：引据面无错位、无关键转述失真**（唯一计数失真见 F6，非承重锚）。

### 二、landing-manifest ≡ §2 各 T 目标锚并集核对

七 T 目标文件逐一并集 = manifest 七路径，逐一吻合（T7 条件锚 OQ-4 已裁 (a) 携带，AGENTS.md 在 manifest 正确）：

- T1 `.claude/references/requirement-change.md` ✓
- T2 `.claude/agents/story-author.md` ✓
- T3 `.claude/agents/requirement-reviewer.md` ✓
- T4 `.claude/scope-map.json` ✓
- T5 `.claude/commands/requirement-change.md` ✓
- T6 `docs/requirements.md` ✓
- T7 `AGENTS.md` ✓

结构字段在场：landing-manifest 围栏块 ✓（Status 头部块下方）、packet-id 行 ✓。

### 三、验证集实跑留痕

| # | 命令 | 实跑结果 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-requirement-loop/PROPOSAL.md` | `ok: true`，anchorCount 7，anchorUnion 七路径与 manifest 逐一吻合、无注记夹带（2026-09-29 实跑，exit 0） |
| V3 | `node .claude/scripts/route-regression-check.mjs` | `route-regression-check: 36 ok, 0 fail`（exit 0）——真空绿注记与 requirement-face 批 V3 同形态，已知且披露 |
| V4 | `node .claude/scripts/spec-anchor-check.mjs`（本阶段实跑取基线） | `ok: true`，designIdCount 60 / archSectionCount 46 / storyCount 13 / anchorLines 31——与 PROPOSAL V4「60/46/13/31 基线」逐项吻合 |
| V2 | `node .claude/scripts/lint-descriptions.mjs`（基线核） | exit 0（告警级；唯一 flag 为 `landing-verifier.md` 超长 363>320，系既有告警、与本批无关） |
| V5/V6/V7 | — | 落地期命令，形态可判（V5 grep 模式 `^> 状态：` 与 T6 增补行形态吻合；V7 行为探针设计成立，见下 enforce-scope 语义核对） |

**enforce-scope 语义核对（T4/V7 承重前提）**：`.claude/hooks/enforce-scope.mjs:160-196` per-agent 拦截仅对 Edit/Write/MultiEdit 生效（Bash 分支 :142-158 只拦 universal.bashDenyWrite）；白名单语义与 scope-map `_doc` 一致（deny 命中拦 / allow 非空未命中拦）。T4 改法后 story-author 写 requirements.md 放行、写 contracts 拦截，V7 探针预期成立。

### 四、findings

**F1（BLOCKING）· T2 遗漏 frontmatter `tools` 行补 Write/Edit——盘上写物理不可达。**

证据：`story-author.md:4` verbatim `tools: Read, Grep, Glob, Bash`——无 Write/Edit。`enforce-scope.mjs:6-8` 注释 verbatim「子 agent 的 `tools` 字段是"工具级"允许列表，不是"路径级"；真正的路径冻结靠 settings 的 permissions.deny + 本 PreToolUse 钩子来做」：T4 的 scope-map allow 只解决「路径级」放行，工具级 Write/Edit 不在 tools 白名单内则 agent 根本调不出写工具。Bash 重定向路也不通：`enforce-scope.mjs` Bash 分支只拦 universal.bashDenyWrite（不含 docs/requirements.md），即机械上不拦，但 `story-author.md:33`「Bash 仅限跑 `node .claude/scripts/spec-anchor-check.mjs`，不执行其它命令」自禁。仓内全部有写权 agent 的 tools 行均含 Edit/Write（`acceptance-author.md:4`、`integrator.md:4`、`scenario-author.md:4` 等，Grep 实测 16 个 def），story-author 现行形态是「无 Write」先例（与 architect-reviewer 同形）——T2 授写却只改正文三处、不动 frontmatter，落地即得到一个「正文说能写、物理不能写」的 agent，核心机制（updating 起草）整体失效。
修法：T2 改法增第 ④ 点——frontmatter `tools` 行改为 `Read, Edit, Write, Grep, Glob, Bash`（逐字段隔离纪律照 authoring rule，只动 tools 行）；§3 sweep 表 story-author 行同步注记「含 frontmatter tools 行」。该修法是已 ack 方向（§0 目标效果「story-author 直接在 requirements.md 上以 updating 态起草」）的必要推论，不改变方向与量级；属 agent 工具面变更，FROZEN 呈 human 复核时主线程应显式点名此行。

**F2（BLOCKING）· 审稿「改动前后对照」机制缺失——四查 1 意图保真的可执行性较现状回退。**

证据链：T2 改法 ② return「不再携带修订稿全文」+ T3 改法 ①②「审稿对象 = 盘上 updating 态的 requirements.md（以盘上文件为准，不审转交文本）」+ `requirement-reviewer.md:29`「Bash 仅限跑 `node .claude/scripts/spec-anchor-check.mjs`，不执行其它命令」→ 盘上只剩 updating 一份，改前文本仅在 git HEAD，reviewer 无工具可取（无 Bash git diff、return 不再含修订稿）。而 `requirement-reviewer.md:18` 四查 1 verbatim「意图保真——对着 human 意图原话逐条核：修订稿是否如实承接、有无夹带原话之外的语义变更」：旧流程 reviewer 同时持「修订稿（拟改后文本）+ 盘上 frozen 原稿（改前文本）」两份可对照，夹带可检；新流程对照面消失，author 在已改锚行清单之外的句级夹带语义改动不可检。T1 改法 2 亦未规定 dispatch prompt 输入构成。这与本批目标效果之一「消灭『审的稿 ≠ 落的稿』漂移口子」形成新缺口：漂移口子关了，对照口子开了。
修法：T1 改法 2（或 T2 ②）明示 dispatch 输入构成 = human 意图原话 + story-author return 全件 + **改动前后对照**——推荐 author 在 return 内附受影响 story 的句级 before/after 对照（不扩 reviewer 权限面、不加工具）；备选 = 主线程 dispatch 时附 `git diff HEAD -- docs/requirements.md` 输出文本（同样不需要放宽 reviewer Bash）。requirement-change.md 改版稿把该输入构成写成机械可判条款。

**F3（MINOR）·「V5 grep 齿」名实不符。** T1 改法 4 与 OQ-3 (a) 把 V5 称为消费保护的「grep 齿」，但 V5 实际核验的是「落稿时状态行恰一行且 frozen」——是状态行完整性自检，不拦截任何下游消费。消费保护本批实态 = 纯成文规则（约定级），机检齿整体留根。建议措辞改「状态行完整性核验（V5）」并如实标消费保护为约定级，防后续读者误判已有机械保护。不阻断（OQ-3 (a) 为 human 显式裁定，方向本身成立）。

**F4（MINOR）· T7 压缩文案丢免派例外。** T7 新行写「档 2 architect 必派」，但 T1 改法 3 保留免派例外（human 明示在先评估承载时免派、事由记 §9 依据列）。指令根路由行可压缩（SSOT 在 reference），但本批 T7 的立项目的恰是防指令根与 SSOT 漂移——建议行内补「（免派例外见 SSOT）」半句。

**F5（MINOR）· 状态机边界场景未成文。** 两个场景缺规则：① 文件处于 updating 态时插入档 0 勘误——档 0「主线程直改即 frozen」与在途 updating 互踩（直改落在在途稿上、状态语义冲突），应裁「updating 期间档 0 一律并入在途批次或等 frozen 后做」；② updating 中断（会话崩/批次弃）后的恢复路径——谁裁弃/续未定（建议：主线程读 updating 态后与 human 确认，弃稿 = git 还原 + 状态行回 frozen）。建议 T1 新节补两句成文。低频、fail-closed 方向（updating 态本身已挡下游），不阻断。

**F6（MINOR）· T6 证据 blockquote 计数失真。** T6 证据称「头部引用区当前四行 blockquote，:3–:7」——实读 `requirements.md:3-:7` 为**五行** blockquote（:3 溯源 / :4 对偶与术语 / :5 目标用户 / :6 范围定名 / :7 当前版本），且「四行」与自引区间「:3–:7」自相矛盾。锚点 :7 本身正确、改法（:7 后追加）不受影响，按先例（requirement-face R2「①类示例失实」同形）记 MINOR，landing 前顺手改「五行」。

**F7（MINOR）· T5 推翻前批 human 裁定建议加显式标签。** T5 推翻 requirement-face 批非目标④的 human 裁定（B 方案：不新增命令入口、将来 triage 统一路由消费）。根因段已实质披露（「human 已裁『不新增命令入口』……属新裁定」）且本批 human 立项 ack 覆盖 T5，方向无冲突；但本仓先例形态是显式标签留痕（requirement-face T5 对 260923 批 v0.4 verbatim「**本项显式推翻 260923 批 v0.4 的不改名裁定**（human 权限内的新裁定，PROPOSAL 留痕）」）。建议 T5 根因段首句加「本项显式推翻 requirement-face 批非目标④（B 方案）」标签，对齐先例。

### 五、审查维度结论

- **目标效果 ↔ 改法集咬合**：方向咬合（盘上状态机 + 写槽 + 入口 + 必派四件互为支撑；「审的稿 ≠ 落的稿」旧漂移口子确实关闭）；两处缺口见 F1（写通道物理不可达）、F2（对照口子新开）。
- **riskTier / review profile 自评**：medium + G2+G3+G1 单行显式呈报成立——G1 触碰经立项 ack + OQ-4 (a) 显式呈报，契约面零触（sweep 表如实），不动 hooks/scripts/verify-gates。Track B 轨道正确（governance 类不强制升级，无跨系统结构性变更形状）。
- **状态机设计**：翻转权（frozen 只归主线程；档 1 PASS 即翻沿用既有裁定、档 2 human 终拍后翻）自洽；档 0 豁免有 F5 ① 的边界洞；消费保护判据（§9 版本行在位）可用但本批仅约定级（F3）。
- **T2/T4 自洽性**：OQ-1 (a) 下 allow 两路径与 T2 写面声明一致；deny 重申契约面无害；reviewer 恒无 Write 与 T3 不动写权一致。唯一不自洽 = F1（tools 行漏改）。
- **先例承接/冲突**：OQ-1 既有裁定、定稿载体、REQ- 纪律均承接不冲突；scope-map 登记留根已由前批落地为占位条目、本批首授 allow，披露如实；两处显性冲突（AGENTS.md 留根改由本批承接、非目标④命令入口被推翻）均已披露，F7 建议补标签。
- **验证集可判性**：V1/V3/V4/V2 实跑全过、基线逐项吻合；V5 形态可判（名实不符见 F3）；V6 复扫模式集完备（四词形覆盖旧表述面）；V7 探针设计与 enforce-scope 实际语义核对成立。

### 六、verdict

**BLOCK(F1,F2)**。F1、F2 均为已 ack 方向内的机械补全（补 tools 行、补 dispatch 输入构成条款），不改变方向、量级、riskTier、profile 与轨道，**无升级 human 项**——修订后报 R2 复核。F3–F7 为 MINOR，landing 前顺手修，不阻断。

## R2

### 一、F1–F7 修订到位性复核（聚焦，不重做全量抽验）

| R1 finding | v0.3 修订位 | 复核结论 |
|---|---|---|
| F1（BLOCKING）tools 行漏 Write/Edit | T2 改法第 ④ 点（`Read, Grep, Glob, Bash` 改 `Read, Edit, Write, Grep, Glob, Bash`，并引 enforce-scope per-agent 拦截只对 Edit/Write/MultiEdit 生效的语义）；T2 头「三处」改「四处」；§3 sweep 表 story-author 行同步注记「tools 行增 Edit/Write——R1 F1；写面由 T4 scope-map 路径级圈定」 | **到位**——改法形态与 R1 建议一致，双层合围（工具级 × 路径级）语义成立；但 ④ 末句新引入错述，见 F8 |
| F2（BLOCKING）对照机制缺失 | T2 ② return 内容增「改前/改后句级对照（逐修订段『原句 → 新句』两行式……return 不再携带修订稿全文但对照不可省）」；T1 第 2 步增 dispatch 输入构成句「主线程 dispatch prompt 的输入构成**显式含 author 产出的改前/改后句级对照**……reviewer 靠对照与盘上文件两相核对做四查 1 意图保真的夹带检测」 | **到位**——对照的产出方（author return）、形态（句级两行式）、消费方（dispatch → reviewer 四查 1）三点闭合，未扩 reviewer 权限面 |
| F3（MINOR）V5 名实不符 | §5 V5 预期列增名实注记：「本项是状态行在场与形态自检；消费保护……按 OQ-3 裁 (a) 本批为约定级成文……不由本 grep 承载」 | **到位** |
| F4（MINOR）T7 丢免派例外 | T7 改法行内补「〔免派例外见 SSOT〕」 | **到位** |
| F5（MINOR）状态机边界场景 | T1 第 1 步增边界场景两句：① updating 在途期档 0 不并行（等 frozen 后补做或并入在途变更，主线程裁其一、§9 依据列留痕）；② 中断弃/续由主线程下次入口裁（弃 = git 还原 + 状态行回 frozen + 不 append §9；续 = 重派 author 接改） | **到位**——两个场景均有可判处置与留痕位 |
| F6（MINOR）blockquote 计数失真 | T6 证据改「头部引用区当前 blockquote 五行，:3–:7」 | **到位**（与 R1 实读一致） |
| F7（MINOR）显式推翻标签 | T5 根因段首加「**本项显式推翻 260928-gov-review-requirement-face 批非目标④**（推翻标签按先例形态……human 权限内的新裁定，PROPOSAL 留痕）」 | **到位**（先例形态对齐） |

### 二、修订引入新矛盾检查

Status 头升 DRAFT v0.3、R1 评审记录原样保留（append-only 纪律遵守）；manifest 七路径、packet-id、riskTier / profile / 轨道自评均未动，无漂移。T1–T7 其余段落与 §0/§3/§4/§6 逐段比对无连带失配。**唯一新问题 = F8**：

**F8（BLOCKING）· T2 ④ 末句对强制层语义事实性错述——「Bash 重定向写仍由 universal.bashDenyWrite 恒拦、无绕行口」为假。**

证据（2026-09-29 实读）：`scope-map.json:25` verbatim `"bashDenyWrite": ["docs/contracts/**", ".claude/**", "delivery/taskpackages/**"]`——**不含** `docs/requirements.md` 与 `docs/architecture.md`；`enforce-scope.mjs:142-158` Bash 分支只按该清单拦显式重定向写。即 story-author 经 Bash 重定向写两 allow 路径**不被任何钩子机拦**，真正的旁路约束是 agent def 的 Bash 白名单（`story-author.md:33`「Bash 仅限跑 `node .claude/scripts/spec-anchor-check.mjs`，不执行其它命令」——约定级，T2 改法未触碰该 bullet、落地后仍在）。v0.3 该句把约定级约束错述为 universal 层机拦并断言「无绕行口」：若照此冻结，未来读者（含后续治理批）可能据此误判「Bash 旁路已机械封死」而放松 agent def 白名单，恰是 260909-gov-review-gate-truthfulness 批收口的那类门真话失真。
修法（一句改写，机械）：T2 ④ 末句改为如实表述——「Bash 重定向写两 allow 路径不在 universal.bashDenyWrite 覆盖内（scope-map.json:25），旁路约束 = agent def『Bash 仅限跑 spec-anchor-check』白名单（约定级，与 requirement-face 批 §3 披露同性质）；Bash 旁路机拦属新齿，归 OQ-3 留根同流」——或最简删去「恒拦、无绕行口」断言、保留「旁路由 agent def Bash 白名单承接（约定级）」。不改变 F1 改法本身（tools 行照改）。

### 三、verdict

**BLOCK(F8)**。F1–F7 全数到位、无回退；F8 为 F1 修订句引入的单点错述，一句改写即解，不改变方向、量级、riskTier、profile 与轨道，**无升级 human 项**——修订后报 R3 收口。

## R3

### 一、F8 修订到位性复核（聚焦）

v0.4 T2 ④ 勘正句 verbatim：「**Bash 旁路如实表述（R2 F8 勘正）**：两 allow 路径的 Bash 重定向写不在 universal.bashDenyWrite 覆盖内（scope-map.json:25 该清单仅 contracts / .claude / taskpackages 三项），旁路约束 = agent def「Bash 仅限跑 spec-anchor-check」白名单（约定级，本改法不触碰该 bullet、落地后仍在）；Bash 旁路机拦属新齿，归 OQ-3 留根同流」。

逐点核对（2026-09-29 实读）：
1. 「不在 universal.bashDenyWrite 覆盖内（scope-map.json:25 仅三项）」——与 `scope-map.json:25` verbatim `"bashDenyWrite": ["docs/contracts/**", ".claude/**", "delivery/taskpackages/**"]` 一致，**准确**；
2. 「旁路约束 = agent def Bash 白名单（约定级，本改法不触碰该 bullet）」——`story-author.md:33` bullet 在 T2 改法 ③ 的改写范围外（③ 只改「不改 docs/requirements.md」句），**准确**；
3. 「Bash 旁路机拦属新齿，归 OQ-3 留根同流」——与 §4 OQ-3 (a)、§6 首条留根一致，**闭合**。

**F8 到位，无残留错述。**

### 二、新矛盾检查（v0.3 → v0.4 全文比对）

全文复读比对：除 T2 ④ 勘正句与 Status 头升 DRAFT v0.4 外，T1/T3–T7、§0/§3/§4/§5/§6 均无改动；R1/R2 评审记录 append-only 保留。无新引入矛盾。

### 三、GREEN 前结构字段核对（judge brief 透镜 7）

- **landing-manifest 围栏块在场**（Status 头部块下方固定位，一行一路径，共 7 条）✓；
- **packet-id 行在场**（Status 头「packet-binding（FROZEN 生效）」行）✓；
- **manifest ≡ §2 各 T 目标文件锚并集**：T1 `.claude/references/requirement-change.md`、T2 `.claude/agents/story-author.md`、T3 `.claude/agents/requirement-reviewer.md`、T4 `.claude/scope-map.json`、T5 `.claude/commands/requirement-change.md`、T6 `docs/requirements.md`、T7 `AGENTS.md`（OQ-4 裁 (a) 携带）——七路径逐一吻合，无多无漏 ✓；
- **V1 preflight 复跑**（2026-09-29 实跑）：`ok: true`、anchorCount 7、anchorUnion 与 manifest 七路径逐一吻合、advisories/findings 空 ✓。

### 四、verdict

**GREEN**。R1 两项 BLOCKING（F1 tools 行、F2 对照机制）+ R2 一项 BLOCKING（F8 Bash 旁路错述）+ R1 五项 MINOR（F3–F7）共八项 findings 全数修订到位、无回退、无新矛盾；riskTier medium / G2+G3+G1 单行显式呈报 / Track B 轨道自评维持成立；契约面零触；OQ-1~4 已裁全按推荐位。主线程可凭「human 立项 ack（2026-09-28）+ 本 GREEN」标 FROZEN。三轮内收口（R1 BLOCK → R2 BLOCK → R3 GREEN），未触发熔断线。

