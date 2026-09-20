# PROPOSAL · 260920-gov-review-citation-anchor-residue

> **状态：FROZEN v1.0 · 2026-09-20 · 可进 landing**（凭据：**human 2026-09-20 立项 ack + OQ 全裁**〔OQ1=(a) git 派生；OQ2=(a) T3 子项 b 纳入本批；OQ3=(c) 三条流程建议留 `/loop-retro` 补——见 §4 裁决表〕+ **architect R3 GREEN**〔§7 `### R3`，`verdict: GREEN`，1 NIT 不拦；R1 `BLOCK`〔1 BLOCKING + 4 MAJOR + 2 NIT〕→ v0.2 全折入 → R2 `BLOCK`〔1 MAJOR + 2 NIT，均 R1-F1 折入的残留同步〕→ v0.3 全折入 → R3 逐条核为「真修复」〕+ **标冻前折入 R3-N1**〔§7 `v1.0 标冻前折入`；算术差查清真因 = 分母口径不同，非笔误〕。**三轮均无升级 human 项**〔T3 子项 b 的 `HUMAN_REQUIRED` 由 R1 判否、R2 / R3 维持〕。**标冻前折入项属标冻前修订**，故不适用「FROZEN 后改 packet 内制品须记 Status 行」那一条——本行即其记录〔承批③ 同款先例〕。**FROZEN 后 packet 内修订 1 处（2026-09-20，landing 期）**：§5 V7 两条 grep 断言按落地实况校准——① `ls-files` 由「两处调用」改为「一行两式」；② 模板 `〔第五段·锚清单〕` 由「零命中」改为「命中仅出现在形态沿革说明句内、无活体使用」（该标记被逐字回引在沿革说明里，与其在 V7 里的地位同构）。**两处均为断言校准、不扩不改任何改法内容**，目标文件三路径一字未变。
> **承载 ARQ**：`ARQ-260907-lfc-citation-docs-prefix`（T1 承接其残余「候选域维护策略」子项）、`ARQ-260914-landing-preflight-anchor-parse`（T3 承接其第 3 类**缩圈后的重新设计**）、`ARQ-260920-anchor-parse-fence-lineno`（T3 承接其两子项）。**T2 无在册 ARQ 行**——它是剩余计划里标「跨批实测新增（累计继续增长）」的那一族，本批承接并随落地登记。不关的根写 §6 留根指针。
> **批构成复证（起草期现跑，2026-09-20）**：剩余计划 P4 原列三条 + 一条另注。复证后**两条的队列描述与实况不符**，改变了 T2、T3 的构成：①「短形态引据族」被描述成「省略仓内已知前缀的相对名」，实测批③ 那 4 条是**三种形态**，其中**中段省略**（省略的不是前缀而是路径中段）与**省略号缩写**根本不属于该描述，前者结构上不可由前缀试探解析、后者压根不是路径；②`ARQ-260920-anchor-parse-fence-lineno` 的子项 b 被自陈「含设计判断」，本批按 human 裁决纳入并如实升声明。复证明细见 §2 各 T 的「证据」段。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条 Track A 硬判据（触契约面 / 跨系统结构性变更）均未命中。**但本批有一个须摊开的判断点：T3 子项 b 改 `parseSection2AnchorLists` 的解析语义**（「引号块内的 `目标文件：` 算不算锚行」是真选择）——`ARQ-260920-anchor-parse-fence-lineno` 自陈「触解析语义、触及时转 `full-adversarial + human`」。human 2026-09-20 已裁**纳入本批**（见 §4 裁决表）；**判官若判其为 `HUMAN_REQUIRED`，本批即升 human，本裁决不预支该判定**。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：三处改动**零门判定语义改动**——引据面（T1/T2）自 260826 上线起即 advisory-only、从不进 `errors`；锚面（T3）只动 `--preflight` 这一**明示不参与任何门判定的只读干跑入口**，其唯一被门消费的改动是 `parseSection2AnchorLists` 跳围栏（子项 b，即 OQ2 的判断对象）。零 `docs/contracts/**`、零 spec 面、零 agent frontmatter、零 settings / hooks / scope-map。不报 low 的理由 = G3 面两件脚本 + 子项 b 触门内函数语义）。
> **review profile**：G3 + G2（G3 = `.claude/scripts/landing-fidelity-check.mjs` + `.claude/scripts/lib/governance-gate-shared.mjs`；G2 = `docs/templates/PROPOSAL.template.md` 的起草纪律面）。契约面 / G1 / agent def / frontmatter / scope-map / settings / hooks **均为零**。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核對、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
docs/templates/PROPOSAL.template.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 `landing-fidelity-check.mjs` 的引据 / 锚两个解析面从「**逐批现撞现补**」改成「**候选域与判定由仓内既有事实派生、不可解析的形态各归其类**」。

**现状差距**：引据候选域自 260826 上线以来已经**逐批扩了四轮**（260826 初版无前缀重解析 → 260907 T2 加三候选前缀 → 260907 同批热修补 `docs/` → 260918 T1 改并集形态并递归派生 `docs/`、`.claude/` 两棵树）。每一轮的触发都是「某批的 PROPOSAL 里出现了一种新的短形态引据，当场报 file-not-exist」，每一轮的修法都是往前缀清单里再加一项。本批复证确认：**派生根只有两根**，而其上「并集形态」并未真正减少手工面——实测**去掉四项硬编码常量后两树递归只得 42 项、`docs/` 与 `.claude/` 两个根都不在其中**（见 §2 T1 证据），即两个根本身仍全靠常量撑着；于是 `delivery/` 面的短形态引据必然第五轮复发。同时锚面 `--preflight` 有两处精度缺陷，其中一处（不跳围栏块）**正在逼迫 canonical 模板的自己示范形态变形**。

**非目标（本批不做）**：

- **不裁撤引据 advisory 面**。复证实测：全仓 `delivery/**` 410 份 md 中未解析引据去重 **71 条**，信噪比确实不高。但 T1/T2 落地后这 71 条会被切成「**26 条转解析、7 条归缩写、11 条归歧义、27 条仍不中**」，四类各有明确处置——裁撤的收益要等这个切分做完才能评估。列为 §6 留根⑥ 交 `/loop-retro` 观察。
- **不把引据面升级成门**。自 260826 起 `citationAnchorAdvisory` 就是观察列、从不进 `errors`，本批不变。
- **不重开「幻影路径」判据**。`ARQ-260914-landing-preflight-anchor-parse` 第 3 类「幻影路径 / 注记夹带路径 token」已经三轮评审证伪（判据建立在「不存在的即幻影」这一不成立假设上）、按 human 熔断裁定缩圈移出。本批**不重试该判据**，只做两件不做判断的事：给它一个**可见面**（T3 子项 c）、把上游纪律**成文**（T3 搭车件）。
- **不追改存量历史制品**。实测 71 条里有相当比例是**写时就正确、其后路径迁走**的历史引据（如 `docs/spec.md:*` 在 2026-09-13 拆分为 `docs/requirements.md` + `docs/architecture.md` 之前确有此件）。冻结的历史评审记录不回改，见 §6 留根②。

---

## 2. 问题、根因与改法（T1…T3）

**【硬项 · 起草期实证义务】** 本节所有断言均为 2026-09-20 起草期现跑实测，命令与关键输出 verbatim 随条附上。涉及的基线：`node .claude/scripts/landing-fidelity-check.mjs --self-check` → 十路自检全绿（`citation-anchors` 14/14、`landing-preflight` 9/9、`section2Anchors` 19/19、`verbatim-quotes` 6/6）；`node .claude/scripts/route-regression-check.mjs` → `28 ok, 0 fail`。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列，形态按模板：**裸 `目标文件：`、不加粗、不在围栏内**。

**消费点普查（起草期现跑，全仓 `grep`）**：`citationPrefixes` / `checkCitationAnchors` / `CITATION_BASE_PREFIXES` 三个符号**只存在于 `landing-fidelity-check.mjs` 一个文件**，无第二消费点；`parseSection2AnchorLists` 的消费点**只有 `landing-fidelity-check.mjs` 三处调用**（`:173` 干跑 problems、`:192` 干跑 anchorCount、`:458` landing 期对账）**加它自己所在 lib 的 `runSection2AnchorSelfCheck`**；`landing-fidelity-check.mjs` 不被任何 hook 或 settings 引用（`.claude/hooks/` 与 `.claude/*.settings.json` 零命中）。⇒ 本批两个脚本的改动半径**完全封闭在这三处调用与一处自检内**。

### T1 · 引据候选域：逐批补前缀 → 由版本控制面派生

**问题**：短形态引据（省略仓根前缀的相对名）在候选域覆盖不到时报 `file-not-exist`，是每批治理 PROPOSAL 都会遇到的假噪音；而修法一直是「往清单里再加一项」，于是同一形态**逐批复发**。本轮触发实例 = 批③（`260920-gov-review-bearing-strength-align`）结构门报的 4 条中的 2 条。

**证据**：

- 现役实现的候选域 = 硬编码四项（`.claude/scripts/`、`.claude/scripts/lib/`、`.claude/hooks/`、`docs/`）∪ `docs/`、`.claude/` 两棵树的递归派生集。**实测（2026-09-20）**：派生前缀共 **43 项**（14.99ms）。
- **四项硬编码在 git 派生域下均被覆盖，但在「两树递归」域下不是**——这一条 R1-F1 打回后**重测更正**（v0.1 的原始断言是自证式的：现役 `citationPrefixes()` 以 `new Set(CITATION_BASE_PREFIXES)` 起手，拿含这四项的集合去验「四项是否被覆盖」必然全 true）。**重测（命令：把四项常量换成空集、只跑两树递归）**：walk-only = **42 项**，`docs/` 在集 = **false**、`.claude/` 在集 = **false**——两个**根**都不由 walk 派生（与 `landing-fidelity-check.mjs:82-84` 代码自注 R1-F1「docs/ 根不是子目录」一致；`.claude/` 根同理）。⇒ 正确口径是：**「git 派生域下四项均被覆盖」成立，「两树递归回退域不含 `docs/` / `.claude/` 两个根」也成立**——两者不冲突，但决定了常量**不能整条删除**（见改法 1、改法 2）。
- 本批实测的两个新候选方案（同一份代码骨架，只换 roots / 数据源）：

  | 候选 | 前缀数 | 耗时 | 备注 |
  |---|---|---|---|
  | 现役（硬编码四项 ∪ 两树递归） | 43 | 15ms | 基线 |
  | 根清单派生 · 三根（`docs`/`.claude`/`delivery`） | 230 | 17ms | 纯 fs，无 git 依赖 |
  | 根清单派生 · 六根（+`firmware`/`server`/`tools`） | 3071 | 153ms | 拖入 `firmware/build/`（1124 目录）与 `firmware/managed_components/`（1504 目录），二者合计 2628/2814 且**均为 gitignore 产物** ⇒ 引据可能「解析成功」到构建残留 |
  | **git 派生（本 T 采纳）** | **439** | **147ms** | 零手工清单、天然排除 build/vendor |

- **全量普查（实测，2026-09-20）**：扫 `delivery/**` 共 **410 份 md**，未解析引据去重 **71 条**（同一口径下先扫 `delivery/changes/arch` + `delivery/changes/dev` + `delivery/workstreams` 共 337 份，去重后同为 71 条——两次扫描口径独立、结果一致）。按 T1 的 git 派生域重算：**26 条转为前缀解析**（`oldOnly: 0`，即现役实现一条都没解析出来）、**7 条归缩写形态**（T2）、**11 条归多命中歧义**（T2）、**27 条仍不中**（多为已迁走路径，见 §6 留根②）。
- **批③ 那 4 条的落地预言（实测）**：`m1-home-qa/decision-coverage.json:116` → `delivery/workstreams/`、`_archive/2026/260911-display-interaction-sovereignty/decision-coverage.json:113` → `delivery/workstreams/`、`…/PROPOSAL.md:126` → 缩写形态、`progress.md:18` → **5 命中歧义**（`delivery/workstreams/` 下 m1-home-qa / m2-char-window / 三个 `_archive` 各有一份 `progress.md`）。⇒ 改后该批的 `file-not-exist` 由 4 归零。

**根因**：候选域的数据源选错了。它一直是**手工维护的前缀清单**（无论硬编码还是「根清单 + 递归」），而前缀清单的变化频率 ≈ 每批新目录出现的频率；仓内真正稳定的、且已被 `.gitignore` / 版本控制天然维护的，是**版本控制面本身**。把候选域挂到版本控制面，就不存在「何时补、何时摘」这个维护面了——这正是 `ARQ-260907-lfc-citation-docs-prefix` 残余那句「候选域维护策略」的答案。

**改法**：

1. **候选域改由 git 索引面派生**（`citationPrefixes` 函数体替换）：取 `git ls-files -z` 与 `git ls-files -z --others --exclude-standard` 两个输出（前者 = 跟踪面、后者 = 未跟踪但未被忽略面），对每一条路径逐段累积目录前缀入集。语义 = 「仓内可被引用的目录」。零手工清单；build / vendor 产物因 `.gitignore` 自动排除；新增目录自动入集。
   **`CITATION_BASE_PREFIXES` 的去留（R1-F1 更正）**：**正常 git 路径不再消费该常量**，但**四项常量保留于回退分支**（改法 2）——**不得整条删除**。理由见证据段重测：两树递归域不含 `docs/` 与 `.claude/` 两个根，两个根在回退域下仍全靠常量撑着；删掉会让回退路径下裸 docs 引据（现役自检里的 `docs/GOV.md` 那一型）重新报 `file-not-exist`，而 V5 明确依赖回退路径正确。
2. **失败即留痕、不静默降级**：git 不可用或非 git 仓时，回退到现役「硬编码四项 ∪ 两树递归」派生，并在观察列新增字段 `prefixSource` 记为 `'fallback-static'`（正常路径记 `'git-index'`）。**不得静默回退**——回退会让「新增目录自动入集」这一保证失效，读者须看得见。
   **如实声明回退域的已知缺口**：两树递归不含 `docs/`、`.claude/` 两个根（靠常量补），且**不含 `delivery/` / `firmware/` / `server/` / `tools/` 四棵树**——即回退域比 git 域**窄得多**。这与 T2 第 5 步的判定交互见 T2 改法 5。
3. **按 dir 缓存的既有形态保留**（现役 `citationPrefixCache`）：git 调用每目录至多两次。自检夹具用临时目录、缓存键不同即重新派生，此性质不变（防假绿）。
4. **连带义务 · 多命中不再取首个**：现役实现 `for (const prefix of citationPrefixes(...)) { if (exists) { resolvedBy = prefix; break; } }` 取**排序后首个**命中。候选域从 43 项扩到 439 项后，多命中从「不可能」变成「常见」（实测 11 条），取首个 = **静默解析到错误的文件**。改法 = 收集**全部**命中：唯一命中才进 `resolvedByPrefix`；≥2 命中归 T2 的歧义类。这是本 T 的承重连带项，漏掉它会用一个假绿换掉一个假红。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）

### T2 · 引据结果：单一 `file-not-exist` 一分为四

**问题**：引据检查把「解析不中」一概记成 `file-not-exist`（文件不存在）。实测这一格里混着**四种语义完全不同**的东西，其中两种根本不该按「文件不存在」读：一种压根不是路径，一种语义上无法唯一解析。读者只能逐条人工回读原文才能分辨，而这正是 advisory 信噪比差的直接来源。

**证据**（2026-09-20 实测，分两部分）：

- **批③ 那 4 条的**逐条定性：`m1-home-qa/decision-coverage.json:116` 与 `_archive/2026/260911-display-interaction-sovereignty/decision-coverage.json:113` 是**前缀省略**（真身分别在 `delivery/workstreams/m1-home-qa/` 与 `delivery/workstreams/_archive/2026/260911-display-interaction-sovereignty/`，实存）——归 T1；`progress.md:18` 是**中段省略**（真身 `delivery/workstreams/m1-home-qa/progress.md`，实测仓内 `progress.md` 共 **5 份**：`m1-home-qa` / `m2-char-window` / `_archive/2026/{260911-display-interaction-sovereignty, kidpal-mvp, kidpal-validation}` 各一）——**结构上不可唯一解析**；`…/PROPOSAL.md:126` 的路径段含 `…`，**不是字面路径**（作者用它表示「上一段同路径」）。
- **全量普查的分类结果**（410 份 md / 71 条去重）：**缩写形态 7 条**（`260914-…/retro.md:38`、`260914-.../PROPOSAL.md:504`、`260915-.../PROPOSAL.md:3`、`260830-.../PROPOSAL.md:215`、`260906-.../PROPOSAL.md:168`、`_archive/…decision-coverage.json:113`、`…/PROPOSAL.md:126`——含省略号 `…` 或连续三点 `...`）；**多命中歧义 11 条**（`PROPOSAL.md:7` 33 命中、`review-packet.md:69` 35 命中、`progress.md:18` 5 命中等，均为同批兄弟文件的相对引据或裸名）。

**根因**：结果模型只有一维（中 / 不中），而「不中」是**至少四种成因的并集**。既然成因可机辨（路径段是否含省略号、试探是否 ≥2 命中、是否仓根相对形态），把它们压成同一个标签就是信息丢失——读者被迫回来做本可机械做完的那一步。

**改法**：把 `checkCitationAnchors` 的返回结构由 `{ checked, problems, resolved }` 扩为六列，判定顺序钉死如下：

1. **字面直中** → 不报（既有行为）。
2. **路径段含 `…` 或 `...`** → 新类 `abbreviated`（缩写形态，非字面路径）。**必须在试探之前判**——否则这些 token 会先走前缀试探、命中一个碰巧同名的文件而假绿。
3. **前缀试探唯一命中** → `resolvedByPrefix`（既有行为，消警留痕不放静默）。
4. **前缀试探 ≥2 命中** → 新类 `ambiguousShortForm`（带命中列表）。向严：不取首个（T1 改法 4）。
5. **试探 0 命中、且路径首段是仓内实存的一级目录**（如 `docs/`、`delivery/`）→ 保留 `problems` 的 `file-not-exist` 原义（真·断链：仓根相对路径确实不存在）。实测样本 `docs/spec.md:252`（`docs` 是一级目录、`docs/spec.md` 已归档）落此类。
6. **其余**（裸名 / 首段非一级目录）→ 新类 `shortFormUnresolved`，附补救句「建议写全仓相对路径，或核对是否已迁走」。

**第 5 步「一级目录集合」的数据来源（R1-F2 补）**：取 **T1 派生前缀集的一级段**（对每个候选前缀 `p` 取 `p.split('/')[0]`，去重）——不另造事实源，与候选域同源同缓存。**回退路径下的交互须钉死**：回退域（两树递归 ∪ 四项常量）的一级段只有 `docs`、`.claude` 两个，**不含 `delivery` / `firmware` / `server` / `tools`** ⇒ 回退状态下 `delivery/...` 型引据会从第 5 步（`file-not-exist`）落到第 6 步（`shortFormUnresolved`）。这是**分类漂移、不是假绿**（两类都不进 `errors`、都带各自的补救句），但读者会看到同一引据在两种 `prefixSource` 下归类不同——**如实声明，不掩盖**；`prefixSource` 字段正是给读者判读用的。

观察列输出形态（`citationAnchorAdvisory`）相应扩为 `{ checked, problems, resolvedByPrefix, abbreviated, ambiguousShortForm, shortFormUnresolved, prefixSource }`。

**`problems` 字段的准确语义（R1-F3 更正）**：`problems` **不是**「收窄为第 5 类」——它还**继续承载两条与本次改动无关的既有 reason**：`line-out-of-range`（字面直中但行号 > 文件行数）与 `file-unreadable`（文件存在但读失败），二者见 `landing-fidelity-check.mjs:136-141`，走的是「字面直中」分支、**不进**六步判定。⇒ 准确表述是：**`problems` 的 `file-not-exist` 一类被收窄为第 5 步的判据**，另两条 reason 原样保留、位置不变。字段名 `problems` 与 `resolvedByPrefix` 保持不变（既有 landing-summary / verdict 引用它们）。**这两条不被 T1/T2 触碰**，其行为与改前逐字一致。

**如实声明（不治）**：第 4 类里的**中段省略**（`progress.md:18` 型）本批**只分类、不解析**。实测已确认：前缀试探治不了（省略的是路径中段，任何前缀拼上去都不存在），后缀匹配也治不了（5 个同名文件、歧义不可消解）；要强行「治」只能造枚举补丁，而枚举补丁不可收敛的判据已被 `ARQ-260914-landing-preflight-anchor-parse` 三轮证伪过一次，本批不重复。它的正解是**起草期写全路径**，已写进 T3 搭车件的模板 checklist。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）

### T3 · `--preflight` 干跑：两处精度缺陷 + 一个可见面 + 一处上游成文

**问题**：`--preflight` 是 260918 批新立的**冻结前只读干跑入口**（明示「不参与任何门判定、不进任何闸链」），它的价值全部来自「起草期就能看见 landing 期才会炸的形态错」。实测两处缺陷正在削这个价值：一处让作者每次都要心算行号偏移；另一处更重——它**正在逼迫 canonical 模板的示范形态变形**，而模板给出的示范形态就是未来每份 PROPOSAL 会照抄的形态。

**证据**（2026-09-20 实测，两组夹具 + 一次源码定位）：

- **子项 a · 行号是 §2 节域内相对号**。夹具：`## 2.` 在第 3 行、加粗锚行在**第 6 行**。实跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <夹具>` 得 `{"class":"bold-anchor-line","lines":[3]}`——**报 3、真身 6**。源码定位：`landing-fidelity-check.mjs` 的 `preflightFindings()` 里 `body.forEach((l, i) => { ... boldLines.push(i + 1); })`，`body` 是 `section2Body()` 返回的**节域切片**、`i` 是切片内下标，未加节域起始偏移。偏移量 = `start + 1`（`start` = `## 2.` 行的 0-based 下标）。
- **子项 b · 锚解析不跳 fenced code block**（**两具夹具，R1-F5 打回后重测更正**——v0.1 把「裸 `目标文件：`」的描述与「带 backtick」夹具的输出混写，二者不自洽）。两具夹具同形：§2 内 T1 段之后放一个围栏块，块内含 `### T1 · 围栏内示范条目` 与一行 `目标文件：`；**唯一差别 = 该行的路径是否 backtick 包裹**。
  - **夹具甲（围栏内路径带 backtick）**，实跑 verbatim：`{"section2BodyFound": true, "anchorCount": 1, "findings": [{"class":"missing-anchor-list","detail":["缺锚清单：T1 · 甲 无「目标文件：」行（fail-loud，不假绿）"]}]}`。⇒ 三项同时发生：围栏内的示范条目被当成**真条目**（幻影条目）、围栏内那条 backtick 路径**被吃进锚并集**（`anchorCount:1`）、**真条目 T1 反被判成缺锚清单**。最后一项最险——若此并集与 manifest 不符，就是 landing 期的 fail-loud 假失配。
  - **夹具乙（围栏内路径裸写、无 backtick）**，实跑 verbatim：`{"section2BodyFound": true, "anchorCount": 0, "findings": [{"class":"missing-anchor-list","detail":["缺锚清单：T1 · 甲 无「目标文件：」行（fail-loud，不假绿）","歧义锚：T1 · 围栏内示范条目 「目标文件：」行无 backtick 包裹路径（锚行格式 = 路径须 backtick 包裹，向严按缺锚处理）"]}]}`。⇒ 同样造幻影条目与真条目误判，但围栏内那行因无 backtick 触发的是「**歧义锚**」而非入并集（`anchorCount:0`）。
  - 源码定位：`governance-gate-shared.mjs` 的 `parseSection2AnchorLists` 逐行扫 `### T<n>` 条目头与 `目标文件：` 锚行，只做 `replace(/^\s*>\s*/, '') + trim()`，**无 fence 状态机**。
- **后果已经在 canonical 模板里兑现**（verbatim，`docs/templates/PROPOSAL.template.md:61`）：「**真实 PROPOSAL 里第五段以 `目标文件：` 起头、不带任何前缀**；上引号块里的 `〔第五段·锚清单〕` 标记是**为避开解析器行为而加**——`parseSection2AnchorLists` **不跳 fenced code block**，若示范行以 `目标文件：` 起头，它会被当作**真锚行**吃进并集、报「歧义锚」（该批起草期实测）。」⇒ canonical 模板**无法示范自己承诺的形态**，只能加一个前缀标记绕开。**示范被逼变形，而模板的示范形态正是未来每份 PROPOSAL 会照抄的形态**——这是形态漂移的温床，也正是 `ARQ-260920-anchor-parse-fence-lineno` 自陈的病根。
- **子项 c 的由来**：`ARQ-260914-landing-preflight-anchor-parse` 第 3 类（幻影路径 / 注记夹带路径 token）三轮证伪后缩圈移出，交接里给的两条替代方向之一即「改治上游因」。实测该类的**真实发生形态**是有记录可查的：260912 批的 near-miss——T1 锚行注记逐字回引了已撤的幻影路径 `.claude/skills/triage-change/SKILL.md`，撤掉加粗形态后该 token 会进锚并集、造成 §2↔manifest 假失配（该批 `retro.md:51`，verbatim：「锚行注记引用任何路径都会被机器当成锚，注记只写人读语义、路径 token 只出现在真锚列」）。当次没出事，靠的是核验官 D4 处置意见里点名了这条纪律、主线程逐 token 排了一遍——**即纯靠人**。

**根因**：三个子项同根——`--preflight` 的输出**只有判断结论、没有判断依据**。(a) 给出结论却不给可定位的坐标；(b) 的解析器不区分「声明的锚」与「被引用的示范文本」，因为 §2 里 fenced block 的角色从未被明确定义过；(c) 同理，锚并集进了什么，只有个数（`anchorCount`）没有名单。

**改法**：

1. **子项 a**：`section2Body()` 改为同时返回节域在文件中的起始偏移，`preflightFindings()` 报**文件绝对行号**（`boldLines.push(i + 1 + offset)`）。修复明确、无分叉。
2. **子项 b**：给 §2 的逐行扫描加**围栏状态机**（三反引号开合跟踪，含开合行自身），`section2Body()`（供加粗探针）与 `parseSection2AnchorLists()`（供条目头扫描 + 锚行扫描）两处都跳围栏内的行。**这是 OQ2 的判断点**（「引号块内的 `目标文件：` 算不算锚行」）——本批采取的立场是：**§2 的 fenced block 是示范 / 原文钉死内容，不是声明**。该立场有仓内既有用法支撑：`checkVerbatimQuotes()` 早已把 §2 围栏块正文当作「作者原文钉死的目标文件内容」做字节级对账，即 §2 围栏在本仓已是**正式构件**、且其语义就是「被引用的文本」。
3. **子项 c**：`runPreflight()` 的输出新增 `anchorUnion`（锚并集成员名单，排序）。**纯可见面，不是判据**——`ok` 判定一个字节不动，`--preflight` 仍然「只保证 §2 锚清单形态可解析」。作者在起草期即可看到「哪些 token 进了锚集」，260912 那种注记夹带当场可见（该次锚并集 10 项 vs manifest 9 行）。
4. **搭车件 · 上游成文（模板两处）**：
   - `docs/templates/PROPOSAL.template.md` 的起草 checklist 补一句纪律——**锚行注记里不要回引路径 token**（会被并入锚集造成 §2↔manifest 假失配）。这一句是 260912 批 landing-verdict D4 处置意见里「建议登 ARQ 一条」的落点，本批直接落成文、不再另立 ARQ。
   - 同文件「读法注 1」改写：子项 b 落地后 `〔第五段·锚清单〕` 这个**为绕开解析器而加的变形标记可以撤销**，模板示范行恢复裸 `目标文件：`（与门同形）。**这是本 T 的可验证收益**——模板的示范形态从此等于门认识的形态。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）· `.claude/scripts/lib/governance-gate-shared.mjs`（改：`parseSection2AnchorLists` 加围栏状态机 + 自检补围栏夹具）· `docs/templates/PROPOSAL.template.md`（改：起草 checklist 补一句 + 读法注 1 改写与示范行还原）

**读法注（本件起草期实测所得，与 T3 子项 b 同址）**：本 PROPOSAL 自身的 §2 **刻意不含**「围栏块内写 `### T<n>` 或行首裸 `目标文件：`」的示范——因为 T3 子项 b 落地**之前**，解析器仍然是旧行为，本件要过的 `--preflight` 也仍是旧的。本件是「修桥的人自己先得过旧桥」的实例：T3 子项 b 落地后，这条约束对后续 PROPOSAL 不再成立。

**而本件起草期确实当场踩了这个坑（第七个实例，verbatim）**：v0.1 三处锚行照抄了 canonical 模板 fenced 示范块里的形态，写成 `〔第五段·锚清单〕目标文件：…`。首次干跑 `--preflight` 报 `{"ok":false,"anchorCount":0,"findings":[{"class":"missing-anchor-list","detail":["缺锚清单：T1 · … 无「目标文件：」行（fail-loud，不假绿）","缺锚清单：T2 · …","缺锚清单：T3 · …"]}]}`——三条全部落空。撤掉 `〔第五段·锚清单〕` 前缀后复跑转绿。**这就是 T3 立案理由的第七个实例，且是第一个「作者照抄模板示范即出错」的实例**：模板里那个为绕开解析器而加的变形标记，被起草者当成了正形照抄。T3 子项 b + 搭车件（撤标记、示范还原裸形态）合起来正是对它的根治。

---

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 · 机械强制层 | `.claude/scripts/landing-fidelity-check.mjs` | 改 | 引据候选域 / 结果分类 / 干跑三子项。**引据面 advisory-only 不变**；`--preflight` 不进任何闸链不变 |
| G3 · 机械强制层 | `.claude/scripts/lib/governance-gate-shared.mjs` | 改 | 仅 `parseSection2AnchorLists` 加围栏状态机 + 其自检补夹具。**唯一被门消费的改动**（子项 b） |
| G2 · 治理成文面 | `docs/templates/PROPOSAL.template.md` | 改 | 起草 checklist 补一句 + 读法注 1 改写。**起草纪律面，非门** |
| 消费点普查 | `parseSection2AnchorLists` 三处调用 + 一处自检 | 已普查 | 三处全在 `landing-fidelity-check.mjs`；无 agent / hook / settings 消费。**R1-F7 补**：该 lib **另有 8 个 import 面**（`devloop-artifact-check` / `closeout-check` / `changes-index` / `merge-driver` / `portfolio-reconcile` / `pre-merge-escalation-gate` / `proposal-consolidation-check` / `taskpackage-required-check`，加本批的 `landing-fidelity-check` 共 9 个），**逐一核对均为具名导入其它函数、无一 import `parseSection2AnchorLists` 或 `compareAnchorSets`** ⇒ 本批改动不触它们，UNCLASSIFIED=0 |
| 契约面 | — | **零** | 不触 `docs/contracts/**` |
| spec 面 | — | **零** | 不触 `requirements.md` / `architecture.md` |
| agent def | — | **零** | 零 frontmatter、零 write scope、零 authority 语义、零 `tools` / `name` / `model` |
| settings / hooks / scope-map | — | **零** | 零改动 |
| 存量制品 | 71 条历史引据 / 存量 landing-verdict | **不改** | 见 §6 留根② |

---

## 4. human 取舍点（OQ）

### OQ1 · 引据候选域用哪种派生方式

**背景**：四个候选各自的覆盖 / 成本实测见 T1 证据表（43 / 230 / 3071 / 439 项；15 / 17 / 153 / 147 ms）。**取舍点**是「用一份会随仓布局缓慢变化的手工根清单，还是用版本控制面这个仓内既有的事实源」。

> **(a) git 派生（采纳）**：候选域 = 版本控制面全部目录前缀。零手工清单、天然排除 build/vendor 产物、新增目录自动入集。**代价**：自检夹具现用 `os.tmpdir()` 下的裸目录（非 git 仓），须改建为真 git 仓（`git init -q` + `git add -A`；起草期已实测可行，且 `git add` 在 Windows 上会往 stderr 吐 CRLF 警告，需一并静音）；另需一条非 git 仓回退路径（T1 改法 2）。**采纳理由一句话**：本批的立案理由就是「逐批补前缀」这个维护面；选任何一份手工根清单，等于把同一个维护面缩小一号保留下来——五根变六根时照旧要改一次。
> **(b) 根清单派生 · 三根**：纯 fs、17ms、无 git 依赖、夹具不动。**代价**：仍是手工根清单；`firmware` / `server` / `tools` 面留缺口（实测 2 例，且都出自 dev review 记录、而 advisory 只扫 PROPOSAL，属纸面缺口）。
> **(c) 根清单派生 · 六根全量**：覆盖最广。**代价**：拖入 `firmware/build/`（1124 目录）与 `firmware/managed_components/`（1504 目录）共 2628 个 gitignore 产物目录 ⇒ 引据可能「解析成功」到一个构建残留，**假绿换假红**；要治得再引入 skip 规则，即再添一个手工清单。
> **(d) 最小改动（仅往并集补 `delivery/`）**：一行、零风险。**代价**：根因不动、第五轮同型复发在路上；且本批的立案理由（「停止逐批补前缀」）随之不成立。

**这项为什么该 human 裁**：它是**成本 / 覆盖面 / 维护面三者之间的取舍**，不是技术对错——(d) 在纯工程风险上其实最低。裁的是「要不要为治根因而吃下夹具改造与回退路径的复杂度」。

### OQ2 · T3 子项 b（§2 锚解析跳围栏）纳不纳入本批

**背景**：`ARQ-260920-anchor-parse-fence-lineno` 自陈该子项「**触解析语义**（『引号块内的 `目标文件：` 算不算锚行』是真选择），如实标注分叉、触及时转 `full-adversarial + human`」，并列了备选「不改解析、把『§2 内禁写锚行示范』成文进模板」。实测（T3 证据）表明当前行为是**纯 bug 面**：围栏内的示范条目会造幻影条目、吞掉锚、把真条目判成缺锚清单。

> **(a) 纳入本批（采纳）**：与子项 a / c 同函数同文件，拆出去反而要多开一批、多一轮评审。收益具体且可验证：模板的 `〔第五段·锚清单〕` 变形标记可撤销、示范形态回到与门同形。**代价**：若判官判 `HUMAN_REQUIRED`，本批升 Track A、多一轮 human 拍板。
> **(b) 本批只做 a / c，b 单走**：本批保持零判断点、判官能逐行核完。**代价**：模板变形示范继续存在；且要为一行状态机改动单开一批。
> **(c) 不纳入，改用成文**：零代码。**代价**：与 `checkVerbatimQuotes` 对 §2 围栏的既有用法方向相抵（§2 围栏是本仓的正式构件，一边用它做原文钉死对账、一边禁止它承载声明形态示范，规则会变得难记）；且实测的幻影条目是纯 bug 面，成文治不了。

**这项为什么该 human 裁**：它决定**本批是否吃下解析语义判断点**，而该判断点直接决定本批会不会被升级到 full-adversarial + human——是本仓反复付过学费的取舍点。

### OQ3 · 剩余计划 §1「立即待办」的处置

**背景**：批③ retro §4 的三条流程建议（① 改法文本里的位置 / 顺序副词是承重指令、落地逐词核；② 拟 canonical 形状 / 枚举前先做全写面 fan-out；③ 引据的归因句与 file:line 同标准核）至今未登记成 ARQ 行。

> **(a) 现在走 `gov-arq` 落成 ARQ 行**：队列状态与实际一致。
> **(b) 并入本批 §6 簿记顺带落**：省一次动作。**代价**：登记时点被推到 landing 期，而这三条正是**起草期**纪律——本批起草就要用，推后等于本批白交一次学费。
> **(c) 留 `/loop-retro` 补（采纳）**：批③ retro §6 留根⑥③ 已记「若未登记，`/loop-retro` 须补」，顺其既定路线走。

**这项为什么该 human 裁**：它是**流程债的清偿时点**选择，且三条纪律的载体本就是 ARQ 收件箱——不在本批的改法面内，agent 不得自行扩张批的半径。

### OQ 裁决（human 2026-09-20）

| OQ | 裁决 | 备注 |
|---|---|---|
| OQ1 | **(a) git 派生** | 候选域 = 版本控制面全部目录前缀；接受夹具改造为真 git 仓 + 非 git 仓回退留痕两条代价。 |
| OQ2 | **(a) 纳入本批** | 子项 b 按本批立场（§2 围栏 = 示范 / 原文钉死内容，非声明）落地。**判官若判 `HUMAN_REQUIRED`，即升 human——本裁决不预支该判定。** |
| OQ3 | **(c) 留 `/loop-retro` 补** | 三条流程建议本批不登记、不并入 §6 簿记；由 `/loop-retro` 按批③ §6 留根⑥③ 的既定路线补。 |

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

**口径注**（承 260914 批 F5 实测瑕疵、260920 批 §5 体例）：本集分两类——**起草期已跑**（V1 干跑、V2/V3 基线，结果见各条 verbatim）；**landing 后核验**（V4–V7，其对象是改后的脚本，起草期无法真跑，故 FROZEN 前只承担「断言口径写明 + 基线可判」义务，**不宣称已跑**）。V8 由 `landing-verifier` 独立执行。

**V1 · 起草期干跑（FROZEN 前）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-citation-anchor-residue/PROPOSAL.md` → 预期 `ok:true`。**本件起草期已跑，结果 verbatim（2026-09-20）**：`{"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 3, "findings": []}`（`anchorCount` = 3，与 landing-manifest 三路径一致）。**首次干跑为红、已修**：v0.1 三处锚行照抄 canonical 模板的变形示范形态（前缀 `〔第五段·锚清单〕`），报 `{"ok":false,"anchorCount":0,"findings":[{"class":"missing-anchor-list","detail":["缺锚清单：T1 / T2 / T3 …"]}]}`；撤前缀后转绿。该实例已记入 §2 T3 的读法注，是 T3 立案理由的第七个实例。

**V2 · 自检全绿 + 断言数提升（真空绿规约）**：`node .claude/scripts/landing-fidelity-check.mjs --self-check` → 预期十路全绿，**断言数为基线 + 新增**。**起草期基线已跑 verbatim（2026-09-20）**：`citation-anchors` 14/14、`landing-preflight` 9/9、`section2Anchors` 19/19、`verbatim-quotes` 6/6、`extractTargets` 6/6、`resolveOrigin` 29/29、`computeFrozenPin` 4/4、`changeid-prefix` 4/4、`lint-descriptions-trigger` 2/2、`isFrozen` 19/19。**新增夹具**（每条须先断言夹具输入非空再断言输出，防假绿）：
- 引据面（T1/T2）**四类各一**，均为**真形态**（形态取自本批实测的批③ 4 条）：① 前缀省略型（`m1-home-qa/decision-coverage.json:116` 形态）→ `resolvedByPrefix`；② 中段省略多命中型（`progress.md:18` 形态）→ `ambiguousShortForm` 且**命中数 ≥2**；③ 缩写形态（`…/PROPOSAL.md:126` 形态）→ `abbreviated`；④ 真断链（首段是实存一级目录）→ `problems` 的 `file-not-exist`。
- 候选域面（T1）：**夹具须是真 git 仓**（`git init -q` + `git add -A`），并含一条**正控断言**——夹具内 `.gitignore` 覆盖的目录**不得**成为候选前缀（证「派生自版本控制面」真生效，而非把目录全收进来）。
- 干跑面（T3）：① 行号断言 —— 加粗锚行的真身行 ≠ 节域相对号（夹具刻意让二者相差 ≥3 行）；② 围栏断言 —— §2 围栏内含 `### T<n>` 与裸 `目标文件：` 的示范块，断言**零误报、零幻影条目**、且围栏内 token **不进锚并集**；③ `anchorUnion` 断言 —— 输出含并集成员名单且与 `anchorCount` 一致。

**V3 · 路由回归（回归）**：`node .claude/scripts/route-regression-check.mjs` → 预期 `28 ok, 0 fail`。**起草期基线已跑 verbatim（2026-09-20）**：`route-regression-check: 28 ok, 0 fail`（exit 0）。本批不动任何 agent def / command，属纯回归；landing 后复跑、**不得低于基线**。

**V4 · 批③ 端到端复放（最强齿）**：对已成稿的 `delivery/changes/arch/260920-gov-review-bearing-strength-align/PROPOSAL.md` 跑引据检查 → 预期 `file-not-exist` 由 **4 归零**：`m1-home-qa/decision-coverage.json:116` 与 `_archive/2026/260911-display-interaction-sovereignty/decision-coverage.json:113` 转 `resolvedByPrefix`（`delivery/workstreams/`）、`…/PROPOSAL.md:126` 转 `abbreviated`、`progress.md:18` 转 `ambiguousShortForm`（5 命中）。**命令与输出摘要进 landing summary**。（该件是唯一一件「语料在盘、形态已知、期望值可先验写死」的真件。）

**V4b · 本件自身复放（改前 / 改后对照）**：对本 `PROPOSAL.md` 自身跑引据检查。**承载断言 = 分类切分（去重后）**，不是 raw 计数——**R1-F4 打回后更正**：v0.1 把 raw 计数「29」当了 headline，而该量**随文档增删漂移**（证据段逐字回引畸形引据的条数会随定稿追加而增：v0.1 起草时 29 → R1 复跑 34 → v0.2 定稿 39）。**改前（现役实现）**：`resolvedByPrefix` = **0 条**、全部进 `problems`——**这一条是承载断言，且稳定**。**raw 计数不作断言，只因它们随本文档每次增删而变**：本件实测序列 `problems` 29（v0.1）→ 34（R1 复跑）→ 39（v0.2 定稿）→ 41（v0.3 定稿）；`checked` 40 → 42；去重 14 → 16（R2 复跑）→ 17。**R2-G2 报的「去重应为 16」与本文的 17 差 1，来源即复跑时点不同、非笔误**；主线程曾试图把它归因于「省略号 `…` / `...` 变体归一」，**实测证伪**（本件两条 `_archive/…decision-coverage.json` 与 `_archive/.../decision-coverage.json` 一有斜杠一无、归并不相等），故如实撤回该解释。**⇒ 判官 / 核验官复跑本项所得数字与本文不符，属预期，不构成失配**——本文档每被编辑一次，它自己逐字回引的引据数就变一次。**这正是「引据 advisory 对以引据为证据的批次天然高噪」的量化长相**：连它自己的计数都随自己增删而变。**改后切分（新设计，稳定量）**：进入分类的 **16 条** = `resolvedByPrefix` **2 条** + `abbreviated` **8 条** + `ambiguousShortForm` **4 条** + `file-not-exist` **2 条**。**去重 17 与 16 的分母差（R3-N1 折入）**：去重总数 **17** = **1 条字面直中**（`docs/templates/PROPOSAL.template.md:61`，本件自引 canonical 模板、不进任何分类）+ **16 条进入分类**；R2 / R3 判官复跑数得「去重 16」即**只数了进入分类的那一档**——**两边都对，是分母口径不同，非笔误**。V4b 的完整恒等式：**17 去重 = 1 字面直中 + 16 分类**。**按 T1 + T2 的新设计重算（起草期同口径实测，同一份 v0.2 定稿）**：去重 17 条切成——`resolvedByPrefix` **2 条**（`delivery/workstreams/`）、`abbreviated` **8 条**、`ambiguousShortForm` **4 条**（`progress.md:18` 5 命中 / `PROPOSAL.md:7` 命中数 = 同批 PROPOSAL 总数 / `review-packet.md:69` 同 / `retro.md:51` 同）、`file-not-exist` **2 条**。⇒ **本件自身的 `file-not-exist` 由 39 归 2**。那 2 条须逐条交代（**不掩盖**）：其一 = `docs/spec.md:252`，**真信号**（`docs/spec.md` 已于 2026-09-13 拆分为 `docs/requirements.md` + `docs/architecture.md`）；其二 = `docs/GOV.md` 那一型，**是自检夹具名而非仓内文件引据**，它出现在本件里的唯一原因是 §7 逐字回引了 R1 判官记录——**而判官记录是不可变制品、不得为迁就 advisory 而改写**。这第二条本身就是「引据 advisory 对以引据为证据的批次天然高噪」的活证。**这条齿的意义**：本件是**引据噪音的上界样本**——它的证据段必须逐字回引畸形引据（不逐字回引就不成其为证据），因而它把上述性质顶到了极限；即便如此 `file-not-exist` 仍能收敛到 2 条（真信号 1 条 + 不可改制品回引 1 条），说明 T1 + T2 的分类真的在做功，而不是把噪音换了个标签。**该性质本身记入 §6 留根⑥**。

**V5 · 反空过（对偶 mutation 思路）**：把 git 派生**故意打断**（在非 git 仓目录下调用，或以环境变量强制走回退路径）→ 预期观察列 `prefixSource` 记为 `'fallback-static'`、且**不静默假绿**（回退派生域下批③ 的两条前缀省略型引据——`m1-home-qa/decision-coverage.json:116` 与 `_archive/2026/260911-display-interaction-sovereignty/decision-coverage.json:113`——**不再进 `resolvedByPrefix`**，改落 `shortFormUnresolved`〔**R1-F6 更正分类名**：二者首段均非一级目录，在 T2 六步下走第 6 步而非第 5 步；v0.1 误写 `file-not-exist`〕，证回退确实生效、不是「无论如何都绿」）。**本条为证明力检查，非门。**

**V6 · 闭集核对**：`git status --porcelain` + `git diff --numstat` 逐路径核对 = landing-manifest **三路径**，零多零少。（`docs/ARCH-REVIEW-QUEUE.md` 的簿记结移为**独立 commit**、不进 manifest——承批③ 先例 `589c35e`。）

**V7 · 语义面复核（人读一遍式）**：改后 `grep -n "ls-files" .claude/scripts/landing-fidelity-check.mjs` 命中派生调用行（**同一行内含 `ls-files -z` 与 `ls-files -z --others --exclude-standard` 两式**，非两行——起草期断言原写「两处调用」，落地实况校准为「一行两式」）；**`CITATION_BASE_PREFIXES` 的去留（R2-G1 更正）**——`grep -n "CITATION_BASE_PREFIXES" .claude/scripts/landing-fidelity-check.mjs` **命中应仅出现在「常量定义行」与「回退分支」**（正常 git 派生路径**不引用**它），**不是「零命中」**（v0.2 曾误写「零命中（四项已删）」，与改法 1「保留于回退分支、不得整条删除」相抵）；`grep -n "ambiguousShortForm\|abbreviated\|shortFormUnresolved\|prefixSource" .claude/scripts/landing-fidelity-check.mjs` 四类字段齐备；`grep -n "fenceLineFlags" .claude/scripts/lib/governance-gate-shared.mjs` 命中围栏状态机；**搭车件复核**：`grep -n "〔第五段·锚清单〕" docs/templates/PROPOSAL.template.md` **命中应仅出现在「形态沿革」说明句内**（记录该标记的历史与撤销，**无活体使用**——起草期断言原写「零命中」，落地实况校准：该标记被逐字回引在沿革说明里，与其在 V7 里的地位同构）、`grep -n "锚行注记不引路径 token" docs/templates/PROPOSAL.template.md` 命中新增纪律句（读法注第 3 条）。

**V8 · landing-verifier 独立核验**：独立重跑 V1–V7（含 V4b 的改前 / 改后对照），并逐文件人读 diff 核对改法 == §2（尤其 OQ2 裁决后子项 b 的立场是否被完整落地、以及 T1 改法 4「多命中不取首个」是否真在代码里）。

---

## 6. 留根清单

- **留根① · 中段省略形态结构性不解析**：`progress.md:18` 型的引据（省略路径**中段**、且省略后不唯一）本批**只分类（`ambiguousShortForm`）、不解析**。实测已确认前缀试探与后缀匹配**都**不能唯一解析（仓内 5 个同名 `progress.md`）。**正解 = 起草期写全路径**（T3 搭车件的模板 checklist）。**触发条件**：若该类在后续批次中高频出现且模板纪律未收效，再评估是否值得为它设计「同批上下文约束下的消歧」——那需要新事实源，本批不做。
- **留根② · 存量历史引据不追改**：71 条未解析引据中「写时正确、其后路径迁走」的一类（典型 = `docs/spec.md:*` 在 2026-09-13 拆分为 `docs/requirements.md` + `docs/architecture.md` 之前的引据、`.claude/taskpackages/*` 迁出至 `delivery/taskpackages/` 之前的引据），**不追改**——冻结的历史评审记录是当时事实的载体。义务自本批起对新 PROPOSAL 生效。
- **留根③ · 簿记结移（落地时随 batch 执行，非改法）**：`ARQ-260907-lfc-citation-docs-prefix` **结移 §2**（其残余「候选域维护策略」由 T1 回答：候选域改由版本控制面派生后无需维护前缀清单，四项硬编码**由正常路径移除、保留于回退分支**〔R2-G1 更正：v0.2 曾误写「已删」，与改法 1 相抵〕）；`ARQ-260920-anchor-parse-fence-lineno` **结移 §2**（子项 a / b 均由 T3 落地）；`ARQ-260914-landing-preflight-anchor-parse` **第 3 类残余更新**——以「可见面（T3 子项 c）+ 上游成文（T3 搭车件）」重新设计后如实标为**部分结**（判据仍不机械化，ARQ 行注明「本批不重开解析判据，交接见 260918 §6 留根⑥」）。**T2 无在册 ARQ 行**，随本批登记一行（短形态引据族：前缀省略已由 T1 治、中段省略结构性不治只分类、缩写形态归 `abbreviated`）。簿记走**独立 commit**，不进 landing-manifest（承批③ `589c35e` 先例）。
- **留根④ · 非 git 仓回退面**：候选域在 git 不可用时回退到现役静态派生（四项常量 ∪ 两树递归）并留痕 `prefixSource: 'fallback-static'`。**这不是等价降级**——回退域下「新增目录自动入集」失效，且域本身**窄得多**（只有 `docs` / `.claude` 两棵树 + 四项常量，**无 `delivery/` / `firmware/` / `server/` / `tools`**），并因此连带影响 T2 第 5/6 步的分类归属（见 §2 T2 改法 5 的交互声明）。该回退路径**无自动化触发面**（本仓恒为 git 仓），只作防御。**触发条件**：若出现一次真实回退（观察列出现 `fallback-static`），须登记 ARQ 复查回退成因。
- **留根⑤ · 第 3 类判据仍不机械化**：`ARQ-260914-landing-preflight-anchor-parse` 第 3 类的判据（对锚并集做仓内存在性检查）**本批不重试**——三段证伪结论（「不存在的即幻影」不成立、四形态合法不在场、枚举补丁不可收敛）维持。本批落的是**不做判断的两件**：可见面 + 成文。**若未来要机械化，须单开一批并重新论证可判性。**
- **留根⑥ · 交日落复盘项**：`/loop-retro` 观察三件事——① 剩余计划 §1 的三条流程建议（OQ3 裁 (c)）是否按批③ §6 留根⑥③ 如期登记；② **引据 advisory 的信噪比是否真随 T1/T2 改善**（本批已给出改前基线：410 份 md / 71 条去重 / 四类切分为 26 : 7 : 11 : 27）；③ **「以引据为证据的批次天然高噪」这一性质是否被承认**——本件自身即上界样本（V4b：改前 39 条、改后 2 条、其中真信号 1 条〔R2-G3 更正：v0.2 曾留 v0.1 残影「改前 29 条、改后 1 条」〕），凡治理批次在证据段逐字回引畸形引据，advisory 条目数就会偏高，**这是证据纪律的必然代价、不是缺陷**；若后续把该条目数当质量指标读，就是读错了。**若改后信噪比未改善，即说明问题不在分类粒度而在该面本身是否值得保留**，届时评估裁撤。
- **自律件（不进模板的环内纪律）**：本批起草期对 P4 四条逐条实测复证，**两条的队列描述与实况不符**（短形态引据族被描述为「前缀省略」、实为三形态；`ARQ-260920-anchor-parse-fence-lineno` 的子项 b 被标判断点、实测其当前行为是纯 bug 面）——「ARQ 的建议改动是待验证假设」这条纪律再得一次实证，与 §1 的三条流程建议同族，随 OQ3 裁 (c) 交 `/loop-retro`。

---

## 7. 评审记录

（待 architect 评审；R1 记录落本节。）

### 标冻前记录 · 2026-09-20 · 主线程

**起草期自检（【起草期自检两句】第 ① 句）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-citation-anchor-residue/PROPOSAL.md` → `{"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 3, "findings": []}`（exit 0）。首次干跑为红（三处锚行照抄模板变形示范形态），撤前缀后转绿——该实例已记入 §2 T3 读法注。

**第 ② 句（路径类变更先查形态族谱）**：**不适用**——本批不含路径迁移类批量改动，目标文件三路径均为原地修改、无新增 / 无迁址。如实记录「不适用」而非省略。

**基线现跑 verbatim（2026-09-20）**：`landing-fidelity-check.mjs --self-check` 十路全绿（见 §2 硬项段）；`route-regression-check.mjs` → `28 ok, 0 fail`。

**OQ 裁决**：见 §4 裁决表（human 2026-09-20 立项 ack，三项全裁）。

**留待评审确认的判断点**：T3 子项 b 的解析语义立场（§2 T3 根因段「§2 的 fenced block 是示范 / 原文钉死内容，不是声明」）——本批自陈该立场**有仓内既有用法支撑（`checkVerbatimQuotes` 对 §2 围栏的既有消费）**，但它是本批自己裁的，判官若判 `HUMAN_REQUIRED` 即升 human（OQ2 裁决的备注已写明不预支该判定）。**R1 已判 `HUMAN_REQUIRED` = 否。**

### v0.2 修订记录 · 2026-09-20 · 主线程（折入 R1 全部 7 条）

| R1 项 | 级别 | 折入动作 |
|---|---|---|
| F1 · T1 改法 1/2 自相矛盾 + 证据断言自证式 | **BLOCKING** | **重测**（四项常量换空集、只跑两树递归 → walk-only **42 项**、`docs/` 与 `.claude/` 根**均不在集**），证据段改为「git 派生域下四项均覆盖 ∧ 两树递归回退域不含两个根」；改法 1 改「正常路径不消费该常量、四项**保留于回退分支**」（不再「整条删除」）；改法 2 补回退域缺口声明；§0 现状差距同步改写 |
| F2 · T2 第 5 步「一级目录集合」数据源未写明 | MAJOR | 补数据源 = T1 派生前缀集的一级段（同源同缓存）；钉死回退路径下 `delivery/...` 型引据由第 5 步落第 6 步的分类漂移，并声明其**不是假绿**；§6 留根④ 同步 |
| F3 · `problems`「收窄为第 5 类」遗漏既有两类 reason | MAJOR | 改准确表述：**只有 `file-not-exist` 一类被收窄**；`line-out-of-range` / `file-unreadable`（`landing-fidelity-check.mjs:136-141`）原样保留、不被 T1/T2 触碰 |
| F4 · V4b raw「29」与实况不符 | MAJOR | 承载断言改为**去重后的分类切分**（稳定量）；raw 计数改为显式漂移说明（29 → R1 复跑 34 → 定稿见标冻前记录），不再当 headline |
| F5 · T3 子项 b verbatim 与夹具描述不自洽 | MAJOR | **重测两具夹具**（围栏内路径带 backtick / 裸写），两段 verbatim 各自完整列出：夹具甲 `anchorCount:1` + 1 条 findings、夹具乙 `anchorCount:0` + 2 条 findings；措辞改准（裸路径触发的是「歧义锚」而非入并集） |
| F6 · V5 分类名「file-not-exist」不准 | NIT | 改为 `shortFormUnresolved`（首段非一级目录 → 第 6 步） |
| F7 · §3 sweep 未点名 lib 其余 import 面 | NIT | §3 补：lib 共 9 个 import 面，逐一核对**均不 import `parseSection2AnchorLists` / `compareAnchorSets`** |

**自查**：F1 是主线程的**测量方法论错误**（拿含这四项的集合去验「四项是否被覆盖」，同义反复）——判官打回成立，已重测更正。F4/F5 同族（**用了早于定稿时点的实测数字**，且 F5 的描述与夹具错配）。

### v0.3 修订记录 · 2026-09-20 · 主线程（折入 R2 全部 3 条）

| R2 项 | 级别 | 折入动作 |
|---|---|---|
| G1 · F1 折入未同步 V7 与 §6 留根③ 的「四项已删」 | MAJOR | V7 断言由「**零命中**（四项已删）」改为「命中**仅出现在常量定义行与回退分支**、正常 git 派生路径不引用」；§6 留根③ 由「四项硬编码已删」改为「**由正常路径移除、保留于回退分支**」——两处与改法 1 对齐 |
| G2 · V4b「去重 17」 | NIT | **改为「该量随正文增删漂移、不作断言」**（主线程复核中先写成「省略号变体归一得 16」，随即**自测证伪**——两条 `_archive/…decision-coverage.json` 与 `_archive/.../decision-coverage.json` 一有斜杠一无、归并不相等——故撤回该解释），如实列出 v0.1→v0.3 的漂移序列并声明 R2 的 16 与本文的 17 **差异来自复跑时点不同**；§7 补复现命令 |
| G3 · §6 留根⑥ 留 v0.1 残影数字 | NIT | 「改前 29 条、改后 1 条真信号」→「改前 39 条、改后 2 条、其中真信号 1 条」 |

**自查**：R2 三条同源——**F1 的更正只改了三处、漏同步另两处**（V7 / 留根③），是「改单源判据时须 grep 其全部消费点逐一过」这条既有纪律的又一次实证（该纪律见批③ retro §4 与本次剩余计划 §8 第 4 条）。主线程在 v0.3 折入时已对 `CITATION_BASE_PREFIXES` / 「已删」/ 旧数字三类词做了全文 grep 复核。

### v1.0 标冻前折入 · 2026-09-20 · 主线程（折入 R3 的 1 条 NIT）

| R3 项 | 级别 | 折入动作 |
|---|---|---|
| N1 · 「去重 17 条切成 [2+8+4+2=16]」算术不自洽 | NIT | 查清真因 = **分母口径不同**：去重 17 = **1 条字面直中**（`docs/templates/PROPOSAL.template.md:61`，本件自引 canonical 模板、不进分类）+ **16 条进入分类**；§5 V4b 补完整恒等式，§7 本行同步。**两边都对，非笔误** |

**说明**：本条为**标冻前折入项**（R3 已判 GREEN 之后、标 FROZEN 之前），故不适用「FROZEN 后改 packet 内制品须记 Status 行」那一条——Status 行即其记录（承批③ 同款先例）。

**标冻前记录（v1.0 定稿现跑，2026-09-20）**：

- **V1 干跑 verbatim**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-citation-anchor-residue/PROPOSAL.md` → `{"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 3, "findings": []}`（exit 0）。标冻前末次复跑同结果。
- **锚并集 ↔ manifest 逐字一致**：`parseSection2AnchorLists` 的 union（3 项，排序）= landing-manifest 围栏块 3 行，**逐字相同**。
- **V4b raw 计数（**不作断言**；承载断言见 §5 V4b）**：该量随本文档每次增删而变（序列见 §5 V4b）。**以 v1.0 定稿的某一时点现跑**：`checked` = 42 条、去重 17 条（= 1 字面直中 + 16 分类）、`problems` = 41 条、`resolvedByPrefix` = **0 条**（现役实现）。按 T1 + T2 新设计重算的切分（**稳定量**）= `resolvedByPrefix` 2 / `abbreviated` 8 / `ambiguousShortForm` 4 / `file-not-exist` 2（其中 1 条为真信号、1 条为 R1 判官记录里逐字回引的自检夹具名）。**复现命令**：以本件为输入跑 `checkCitationAnchors` 等价的 `CITATION_RE` 提取 + 前缀试探；全量基线的命令与数字见 §2 T1 证据段。

### R1 · 2026-09-20 · architect

**verdict: BLOCK**

独立复跑：`--self-check` 十路全绿（citation-anchors 14/14、landing-preflight 9/9、section2Anchors 19/19、verbatim-quotes 6/6、extractTargets 6/6、resolveOrigin 29/29、computeFrozenPin 4/4、changeid-prefix 4/4、lint-descriptions-trigger 2/2、isFrozen 19/19）、`route-regression-check` 28 ok 0 fail——均复现。消费点普查独立复跑：`citationPrefixes` / `checkCitationAnchors` / `CITATION_BASE_PREFIXES` 代码面仅 `landing-fidelity-check.mjs` 一处；`parseSection2AnchorLists` 仅 `landing-fidelity-check.mjs:173/:192/:458` 三调用 + lib 自检 `runSection2AnchorSelfCheck`；`.claude/hooks/` 与 `.claude/*.settings.json` 对 `landing-fidelity-check` 零命中。核心数字复现：现役前缀 43；git 派生 439（tracked-only，我含在途 untracked PROPOSAL 得 440）；批③ 4 条逐条复现；`progress.md` 5 命中 / `review-packet.md` 35 / `retro.md` 28 / `PROPOSAL.md` 33（含在途件 34）；410 份 md / 71 去重（含在途件 411 / 72）；git 派生四类切分 26 : 7 : 11 : 27（含在途件 ambiguousShortForm 12）。T3 子项 b 判 HUMAN_REQUIRED：**否**——「§2 fenced block 是示范 / 原文钉死，非声明」立场成立，有 `checkVerbatimQuotes`（`landing-fidelity-check.mjs:306-334`）对 §2 围栏块的既有字节级对账消费作支撑；跳围栏后真实声明若误入围栏仍会 fail-loud「缺锚清单」，无静默绕过面。

**F1（BLOCKING）· T1 改法 1 与改法 2 自相矛盾 + 证据「四项已被派生集覆盖」对现役两树递归为假**：§2 T1 证据段称「四项硬编码已全部被派生集覆盖…`docs/=true`…可整条删除」，改法 1 令「`CITATION_BASE_PREFIXES` 四项删除」，改法 2 却令「git 不可用回退到现役『硬编码四项 ∪ 两树递归』」。独立复跑：现役 `citationPrefixes` = 43 项；去掉四项常量后两树递归 walk-only = **42 项且 `docs/` 不在其中**——`docs/` 现役在集全凭硬编码常量、非派生所得（与 `landing-fidelity-check.mjs:82-84` 代码自注 R1-F1「docs/ 根不是子目录」一致）。git 派生域下四项均被覆盖（`docs/` 等 = true），但**回退路径用的是两树递归、非 git**。照改法 1 字面「整条删除」后，回退域失去 `docs/` 前缀，裸 docs 引据（现役自检 `docs/GOV.md:2` 型）在非 git 回退下报 file-not-exist，且 V5 明确依赖回退路径正确。**修法**：改法 1 改「正常 git 路径不再消费 `CITATION_BASE_PREFIXES`，但四项常量（或其等价内联）保留于回退分支」；证据段「四项已被派生集覆盖」改为「四项在 git 派生域下均被覆盖；两树递归回退域不含 `docs/` 根，回退分支须保留四项」。

**F2（MAJOR · SHOULD）· T2 第 5 步「首段是仓内实存一级目录」的判据数据来源未写明**：§2 T2 改法 5 只给判据、未写「一级目录集合」从何取得。最自然是复用 T1 派生前缀集的一级段（我复跑即以 git 前缀一级段为源、分类正确）。但改法未声明**回退路径**下该集合随之收窄（回退域只含 `.claude/*` + `docs/*` 子目录 + 四项，无 `delivery/` / `firmware/` 等），导致 `delivery/...` 型引据在回退域下从 `file-not-exist` 变 `shortFormUnresolved`——这一交互须钉死。

**F3（MAJOR · SHOULD）· T2「`problems` 收窄为第 5 类」遗漏既有 `line-out-of-range` / `file-unreadable` 两类 reason**：六步只覆盖「字面不直中」分支；「字面直中但行号越界 / 不可读」仍 push 进 `problems`（`landing-fidelity-check.mjs:136-141`）。§2 T2 改法末段称「problems 的语义收窄为第 5 类（file-not-exist）」与实现不符，且未声明这两类去向。**修法**：声明 `problems` 仍含 `line-out-of-range` / `file-unreadable`（或另立字段），把「收窄为第 5 类」改为准确表述。

**F4（MAJOR · SHOULD）· V4b「改前 problems 29 条」与实况不符**：§5 V4b 称「problems 29 条（去重 14 条）」「由 29 归 1」。独立复跑现役 `checkCitationAnchors` 于本件：checked=35、problems=**34**、resolvedByPrefix=0；去重 14 条、四类切分 2/7/4/1 **均复现**。仅 raw 计数 29 不复现（本件证据段逐字回引畸形引据、随定稿追加而漂移）。承重结论（去重 14 → file-not-exist 1 真信号）成立，headline「29」须改「34」（或「去重 14」）。

**F5（MAJOR · SHOULD）· T3 子项 b verbatim 与夹具描述不自洽**：§2 T3 证据段称夹具「裸 `目标文件：`」且输出 `{"anchorCount":1,"findings":[...缺锚清单：T1 · 甲...]}`。独立复跑同形夹具实得 `anchorCount:1` + **两条** findings（`缺锚清单：T1 · 甲` **及** `歧义锚：T1 · 围栏内示范条目 「目标文件：」行无 backtick 包裹路径`）。且「裸 `目标文件：` 被吃进锚并集（anchorCount:1）」措辞不准：裸 `目标文件：` 无 backtick、触发的是「歧义锚」而非并集；`anchorCount:1` 实来自真锚行（在围栏后）被误归幻影条目。核心结论（不跳围栏 → 幻影条目 + 缺锚 + 真条目误判）**成立并已复现**，唯 verbatim 与措辞须修准。

**F6（NIT）· V5「批③ 两条前缀省略型引据重新报 file-not-exist」分类名不准**：在 T2 六步下 `m1-home-qa/decision-coverage.json:116` 与 `_archive/.../decision-coverage.json:113` 首段均非一级目录 → 应归 `shortFormUnresolved`（第 6 步）非 `file-not-exist`（第 5 步）。V5 意图（回退后确不静默假绿）成立，分类名须改准。

**F7（NIT）· §3 sweep 未显式点名 `governance-gate-shared.mjs` 的其余 import 面**：独立 grep 得该 lib 被 9 脚本 import（devloop-artifact-check / closeout-check / changes-index / landing-fidelity-check / merge-driver / portfolio-reconcile / pre-merge-escalation-gate / proposal-consolidation-check / taskpackage-required-check），但均只 import 非 `parseSection2AnchorLists` 的函数，本批改动不触它们。「消费点普查」行已覆盖实质，建议补一句「其余 8 import 面均不触 `parseSection2AnchorLists`」以闭合 UNCLASSIFIED。

**升级 human 项**：无。T3 子项 b 判 HUMAN_REQUIRED：否（理由见上）。其余打回均为机械 / 措辞修正，主线程可直接折入。

### R2 · 2026-09-20 · architect

**verdict: BLOCK**

独立复跑（v0.2 定稿）：V1 `--preflight` → `ok:true / anchorCount:3 / findings:[]`（复现）；V4b 现役实现 → `checked=40 / problems=39 / resolvedByPrefix=0`（复现），去重 **16** 条；git 派生重算 → `resolvedByPrefix 2 / abbreviated 8 / ambiguousShortForm 4 / file-not-exist 2`（逐项复现，含 `docs/spec.md:252` 真信号 + `docs/GOV.md:2` 自检夹具名）；F5 两具夹具 verbatim 逐字复现（夹具甲 `anchorCount:1` + 1 条 findings、夹具乙 `anchorCount:0` + 2 条 findings）；`.claude/` 根 walk-only 独立复跑 = **false**（与新增断言「`.claude/` 根同样不由 walk 派生」相符，walk-only 共 42 项）；`compareAnchorSets` 消费点独立复跑 = 仅 landing-fidelity-check + lib 自检（§3 补句属实）。

**F1–F7 折入判定**：F2/F3/F5/F6/F7 **真修复**（各自独立复跑相符）；F1 **部分**——改法 1/2、证据段、§0 均已修（`.claude/` 根断言实测成立），但**残留自相矛盾**（见下 G1）；F4 **真修复**（承载断言已改为去重切分、复现），但残留一处计数笔误（见下 G2）。

**G1（MAJOR）· F1 折入未同步 V7 与 §6 留根③ 的「四项已删」表述，二者仍与改法 1「保留于回退分支（不得整条删除）」自相矛盾**：§5 V7 仍写「`grep "CITATION_BASE_PREFIXES" … 零命中（四项已删）」，§6 留根③ 仍写「四项硬编码已删」——均源自 v0.1「整条删除」，未随改法 1 更正。改法 1（§2 T1）已明令「四项常量保留于回退分支、不得整条删除」，则 V7 的「零命中」断言与 landing-verifier 实际会看到的「常量仍在（或内联）」相抵。**修法**：V7 改为「`grep "CITATION_BASE_PREFIXES"` 仅回退分支命中（正常 git 路径不引用）」；§6 留根③ 改为「四项硬编码由正常路径移除、保留于回退分支」——两处与改法 1 对齐即可。

**G2（NIT）· V4b「去重 17 条」与四类切分自相矛盾，应为「去重 16 条」**：§5 V4b 与标冻前记录均写「去重 17 条」，但其「切成 resolvedByPrefix 2 / abbreviated 8 / ambiguousShortForm 4 / file-not-exist 2」= **16**；独立复跑去重 = **16**。改「17」为「16」。

**G3（NIT）· §6 留根⑥ 仍写「V4b：改前 29 条、改后 1 条真信号」（v0.1 旧数字）**：应同步为「改前 39 条、改后 2 条（其中 1 条真信号）」——「1 条真信号」本身仍对，唯「改前 29」与「改后 1 条」是 v0.1 残影。

**升级 human 项**：无。T3 子项 b 判 HUMAN_REQUIRED：否（R1 已判，本批立场与 `checkVerbatimQuotes` 消费支撑未变）。G1/G2/G3 均为机械 / 措辞修正，主线程可直接折入。

### R3 · 2026-09-20 · architect

**verdict: GREEN**

独立复跑（v0.3 定稿）：V1 `--preflight` → `ok:true / anchorCount:3 / findings:[]`（复现）；V4b 现役实现 → `checked=42 / problems=41 / resolvedByPrefix=0`（复现），**去重=16**、git 派生切分 `resolvedByPrefix 2 / abbreviated 8 / ambiguousShortForm 4 / file-not-exist 2`（=16，逐项复现）。全文 grep `已删` / `整条删除` / `四项硬编码`：除「不能/不得整条删除」「由正常路径移除、保留于回退分支」等更正态，以及 §7 评审/修订记录对旧措辞的合法逐字回引外，**无第三处残留**——G1 真修复。

**G1/G2/G3 判定**：G1 真修复（V7 改「命中仅出现在常量定义行与回退分支」、§6 留根③ 改「由正常路径移除、保留于回退分支」，与改法 1 对齐；grep 无第三处残留）；G2 真修复（「raw 计数不作断言」转向合理，见下免责声明判断；「省略号归一」假设自测证伪并撤回，如实）；G3 真修复（§6 留根⑥ 数字已同步为「改前 39、改后 2、真信号 1」）。

**免责声明是否过宽（本轮核心独立判断）**：**不过宽**。该声明只覆盖 V4b 的 raw 计数（`checked` / `problems` / 去重），其漂移机制真实且可复现——本文档每被编辑一次，§2/§5/§7 逐字回引畸形引据的条数就变一次（我三轮回跑实测：`problems` 34→39→41、去重恒 16）。而**承载断言被显式排除在声明之外、仍可判**：改前 `resolvedByPrefix=0`（三版本恒 0）与改后切分 `2/8/4/2`（R2/R3 均复现）均标「稳定量」，V4（批③ 4 条归零）、V5（回退行为）、V1–V3、V6、V7 亦不在此声明覆盖内。故不存在「用声明掩盖真实失配」的通道——若落地时切分或 `resolvedByPrefix` 偏离 `2/8/4/2/0`，该声明**不豁免**。

**N1（NIT）· 「去重 17 条切成 [2+8+4+2=16]」残留算术不自洽，漂移序列末项应为 16**：§5 V4b 仍写「去重 17 条切成——…」，其「17」与紧随的四类切分之和 16 在同一句内指同一集合；独立复跑 v0.3 定稿去重 = **16**（非 17），故漂移序列「14→16（R2 复跑）→17」末项应作「→16」。承「raw 计数不作断言」框架此属 NIT，建议标 FROZEN 前把该「17」改「16」（或删去该数、只留切分）。承载断言（切分 2/8/4/2）本身复现、不受影响。

**升级 human 项**：无。T3 子项 b 判 HUMAN_REQUIRED：否（R1/R2 已判，未变）。N1 为纯措辞，不构成打回。
