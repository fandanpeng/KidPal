# landing-verdict · 261014-gov-review-crq-debt-rename

verdict: PASS

- 判定来源：独立零上下文重推导——landing commit `05e73eb`（单笔原子）逐文件 diff 对照 FROZEN PROPOSAL v0.3（§2 T1–T5 + §5 验证集）+ 结构门 / preflight / V1–V5 全部亲跑，不采信 landing-summary 自述。
- 独立性声明：本 verdict 只写本文件；未改任何 canonical / 被验文件。

## 六项核验逐项结论

### 1. 逐文件 diff 对照（FROZEN §2 改法集）——ok

landing commit `05e73eb` 全量文件 = manifest 34 锚 + 136 条目纯 rename（裁腿面）+ PROPOSAL / landing-summary 两件制品 + `delivery/changes/arch/INDEX.md`（生成簿记，§3 预记「生成件自更」）。无 manifest 外 canonical 改动、无未授权文件（结构门 unauthorized: [] 独立佐证）。逐文件：

- **T1 四件机制脚本**：
  - `debt-ledger-shared.mjs`（R095 rename 自 `crq-ledger-shared.mjs`）：`LEDGER_ROOT = ['docs', 'debt']` 落位；头注 / 消费方提及全部改新名；`GENERATED_MARKER` 改指 `debt-render.mjs`。ok。
  - `debt-render.mjs`（R088）：硬编码 `docs/crq` 字面量全部改 `LEDGER_ROOT.join('/')` 派生（indexHeader 引用行 / 防真空 FATAL 串 / `--check` 待重渲染输出）；`indexHeader` 模板串、self-check 输出串（`debt-render --self-check: ok (8/8)`）、互 import 行全部改新名；import 增 `LEDGER_ROOT` 为派生必需、属改法覆盖内。ok。
  - `debt-doctor.mjs`（R090）：`:43` 硬编码前缀改 `LEDGER_ROOT.join('/')` 派生；互 import 两行改新名；self-check 输出串改 `debt-doctor --self-check`；防真空 FATAL 串规约保持（仅路径派生）。ok。
  - `crq-migrate.mjs`（保留原名，M）：import 行随迁改指 `debt-ledger-shared.mjs`；retired 注新增两行——旧头注行的 `docs/crq` 字样一并消除、retired 注写「路径与依赖已迁 `docs/debt` / `debt-ledger-shared.mjs`」，**不复述旧路径与旧脚本名字面，互锁条款（T1④ / QUESTION-R1-2 正形）遵守**。ok。
- **T2 结构双登记三件**：`scope-map.json` 恰三键（docsTopLevel.allowDirs + integrator.allow + adjudicator.allow）平移 `docs/crq` → `docs/debt`，无第四处；`route-fixtures/surface-alignment.json` adjudicator 槽 allow + rationale 跟改（lastReviewed 2026-10-14 更新，summary §1 已披露）；`repo-structure.md` 表行 / 白名单子目录行 / 消歧表 / 引用纪律 / 语义说明段全改，「CRQ = change request 账」行注随改名消亡、由「debt 账，原名 crq、261014 批改名」替代。ok。
- **T3 治理面 17 件**：agents 2 + commands 5 + references 7 + rules/contracts.md + skills 2——逐件人读 diff，全部仅路径 token / 术语 token（CRQ→debt）/ 旧脚本名 / 旧模板名替换，权限边界句零语义变更（HQ-1 边界遵守）；gov-arq SKILL 模板名引用（原 :27/:51 位置语义）与脚本名引用全部跟改；无写域扩张、无私货。ok。
- **T4 四件**：`glossary.md` 三词条路径跟改、「CRQ 账本」词条改名「debt 账本」并记沿革「原名 `crq`、261014-gov-review-crq-debt-rename 批改名」（**不写完整旧路径、不写旧脚本名——互锁条款遵守**）；CDQ 词条「CRQ tech 域」→「debt 账本 tech 域」；ARQ 词条「已重构为 CRQ 账本」句改「debt 账本」。`ARCH-REVIEW-QUEUE.md` 指路表 3 行相对链接 + 登记命令 + 两处脚本名全改。`debt-item.template.md`（R074 rename 自 `crq-item.template.md`）题行「CRQ 条目文件骨架」→「debt 条目文件骨架」+ 内文路径 / 脚本名跟改。`.gitattributes` 两条 union pattern 改指 `docs/debt/` + 注释块脚本名跟改。ok。
- **T5 六份生成件 INDEX**：六件头注生成行 / 指路行 / 引用带目录行全部重渲为新脚本名与新路径；条目行内容守恒（gov 已结 83 / tech 17 / req 2 与摘要行一致）；tech 归档年 INDEX 保留 7 处 `closedBy` 历史记名「CRQ」（BLOCK-R2-1 豁免面，V3 R093 纯函数重渲带入，符合 FROZEN 附注）。ok。
- **136 条目件**：`git show 05e73eb --name-status` 实测 R100 恰 136 条（零内容变更）；非 R100 rename 恰 10 件 = 3 脚本 + 6 INDEX + 1 模板，与 FROZEN T5 入锚裁决逐件对应。ok。

### 2. packet-binding 一致——ok

- summary `packet-id` 行（:3）= 「FROZEN PROPOSAL v0.3 §2 改法集（T1–T5）+ §5 验证集 · R3 GREEN」，与 PROPOSAL 头部 packet-binding 行、§7 R3 GREEN 结论一致。
- manifest 双侧（PROPOSAL :12-47 / summary :34-69）各 34 行，结构门 report `proposalTargets` ≡ `summaryTargets` 逐行全等；preflight 亲跑 `ok=true, anchorCount=34`，anchorUnion 与 manifest 集合全等。
- summary 骨架 = `docs/templates/architect-landing-summary.template.md` 结构复制（标题行 / packet-id 行 / §1 对应表 / §2 验证证据 / §3 诚实注记 / manifest 围栏块五件组件齐备），`landing-manifest` 围栏块在场。
- T2 组件 / T3 清单绑定面：落的 == FROZEN §2 目标文件锚清单（见第 1 项）。

### 3. 验证集复跑（§5）——ok（全部亲跑）

- **V1 活面残留清扫（七形态，两轮）**：活面 = 30 锚（34 减 6 INDEX）+ `AGENTS.md` + `CLAUDE.md`（README.md 仓根实测不存在，summary 披露属实）。第一轮全域扫（`.claude` + `docs` + `.gitattributes`）、第二轮窄活面扫，两轮活面命中全 0：斜杠形 / 分段引号形 / 裸段形 / 转义形 / 点号粘连形 / 旧名文件引用形（`crq-(render|doctor|ledger-shared|item)`）/ bare-CRQ 形（减 6 INDEX）。幂等复跑要求满足（两轮一致 0）。宽扫命中全部落在 FROZEN §3 显式豁免/未触面（见 ADVISORY-2）。
- **V2 机检面**：`node --check` 四脚本各 exit 0；`debt-doctor.mjs --self-check` → `debt-doctor --self-check: ok (13/13)`（自检名已随改名更新——「自检真跑了」核过：输出含新自检名）；`debt-doctor.mjs` 真实仓 → `条目 136 条：gov=98 tech=36 requirement=2` + `advisory 172 条（不阻断）` + `RED 零命中 —— ok`，与 pre 基线逐字守恒；`debt-render.mjs --check` → exit 0 无输出（零漂移）。防真空双闸在位（doctor FATAL 0 条目拒判绿 + V3 计数守恒），无真空绿。
- **V3 守恒与纯 rename**：`git ls-files docs/crq | wc -l` = 0；`git ls-files docs/debt | wc -l` = 142；`git diff -M 8dcc330 05e73eb --summary` = 146 rename，非 100% similarity 恰 10 件（3 脚本 + 6 INDEX + 1 模板，全部 FROZEN 入锚改法面）——136 条目全 100% similarity。
- **V4 双登记一致**：scope-map `allowDirs` = [contracts, templates, archive, notes, ui, debt, showme] 与 repo-structure 白名单子目录七项逐 token 集合相等、只含 `docs/debt` 不含 `docs/crq`；`grep -c "docs/crq"` 两件各 0。
- **V5 结构门基线**：`landing-fidelity-check.mjs --self-check` 11 组全 ok（isFrozen 19/19；`conflicting-status-lines` advisory 为其夹具自身 first-wins 提示，非本批红）。
- **配套机检（summary 声称，亲跑复证）**：`route-regression-check.mjs --self-check` 106 ok 0 fail / 真实扫描 53 ok 0 fail；`lint-descriptions.mjs` scanned 16、flagged 0、clean。
- **preflight 复跑（landing 后）**：`ok=true, anchorCount=34`。

### 4. 结构门复跑——ok

`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/261014-gov-review-crq-debt-rename` → `ok: true`，EXIT=0；`frozen: true`、`packetBinding: true`、`unauthorized: []`、`claimedNotLanded: []`、`plannedNotLanded: []`、`gitDegraded: false`。advisory `changedCanonicalNotInProposal` 列出的 136 件全部为 git mv rename 条目（FROZEN §2 显式裁腿面「迁移件零内容变更不产锚」），逐一核对与 rename 清单重合，非私货。

### 5. 真空绿规约——ok

- 重放类：V1 各命令输入非空（活面 30 文件在盘，grep 退出码 1 = 零命中而非空输入）；V2 doctor 真实仓 136 条目非空输入 + 内建 FATAL 防真空闸在位；V3 计数 0/142 双侧断言（左侧 0 是「旧路径应空」的守恒断言、非空输入问题，右侧 142 断非空）。
- 接线类：本批无 `--self-check` 新接线；改名后自检名已核（`debt-doctor --self-check: ok (13/13)` 含新名，输出非旧名残留）。
- 环境红基线：无闸执行层红需归因（全绿）；LF→CRLF add 警告为仓内 autocrlf 常态（261013 齿钉批结论），非本批引入。

### 6. 诚实披露核对——ok（四条注记全部与实况相符）

- 「渲染器防冲掉闸触发一次、rm 6 份生成件再重渲」：与六份 INDEX 重渲 diff 形态相符（生成头注整段替换、条目行守恒）；无手工内容损失可证（INDEX = 条目纯函数 + doctor 136 守恒亲证）。
- 「落地期新修 repo-structure『时名 `crq/`』→『时名 `crq`』」：diff 实况终态确为不带斜杠的「时名 `crq`」，与互锁条款一致，披露属实。
- 「ADVISORY 留根」（route-fixtures / .gitattributes 不在 GATE_LAYER_RULES G1–G4 正则内；census 710/169 与 688/168 之差 = PROPOSAL 自身）：与 FROZEN §3 注记 3 / 注记 1 及 debt-ledger-shared.mjs `GATE_LAYER_RULES` 四正则实测相符。
- 「merge 未随本批执行（human 指示暂缓）」：与 `delivery/changes/arch/INDEX.md` 行状态（`frozen | —`）及 human 指示一致；本 verdict 不涉及 merge 判定。
- 无该记未记项。

## 亲核留痕（命令 + 关键输出）

- `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/261014-gov-review-crq-debt-rename` → `ok:true` EXIT=0（frozen/packetBinding true、unauthorized []）。
- `node .claude/scripts/landing-fidelity-check.mjs --preflight …/PROPOSAL.md` → `ok:true, anchorCount=34`。
- `git show 05e73eb --name-status` → 25 M + 2 A + 146 R（R100×136）；非 100% 恰 10 件（debt-doctor R090 / debt-ledger-shared R095 / debt-render R088 / 6 INDEX / 模板 R074）。
- 逐文件 diff：T1 四脚本（blob 级新旧对比）+ T2 三件 + T3 十七件 + T4 四件 + T5 六 INDEX 全部人读过，无遗漏（136 条目按 V3 R100 汇总核对）。
- V1 七形态 × 两轮：活面（30 锚 + AGENTS.md + CLAUDE.md）全 0；README.md 仓根不存在（`ls` 实证）。
- V2：`node --check` ×4 OK；`debt-doctor --self-check` → `ok (13/13)`；真实仓 → `条目 136 条：gov=98 tech=36 requirement=2`、`advisory 172 条`、`RED 零命中 —— ok`；`debt-render --check` exit 0。
- V3：`git ls-files docs/crq | wc -l` = 0；`docs/debt` = 142。
- V4：allowDirs 七项与 repo-structure 白名单集合相等；两件 `docs/crq` 计数 0/0。
- V5：`landing-fidelity-check.mjs --self-check` 11 组 ok、exit 0。
- 配套：`route-regression-check` 106 ok / 53 ok；`lint-descriptions` flagged 0。
- 互锁专项：`crq-migrate.mjs` retired 注与 glossary 沿革注均不含旧路径完整字面（`docs/crq`）与旧脚本名（`crq-render` 等）——V1 对二文件零命中即机证。
- 工作树脏面：仅 `.codebase-memory/` 两件（工具自刷新常态面），landing 改动全部封存于 `05e73eb` 单笔。

## ADVISORY 清单（非阻断观察）

1. **INDEX 行状态待翻**：`delivery/changes/arch/INDEX.md` 本批行现值 `frozen | —`；verdict 落盘后主线程重跑 `node .claude/scripts/changes-index.mjs --upsert 261014-gov-review-crq-debt-rename` 即由 verdict 行现推 `landed`（既有惯例，非本批缺陷）。
2. **V1 活面是窄口径**：全域宽扫下 `docs/ADR.md`、`docs/CONTRACT-DECISION-QUEUE.md`（已结历史行）、`docs/showme/` 3 件、`docs/debt/**` 条目旧引文 5 件、`landing-fidelity-check.mjs`（:244 历史实录注释 + :340-341 self-check 夹具）仍有旧 token 命中——与 FROZEN §3 sweep 分类逐件相符（历史豁免 / 非活引用），非残留；后续批引用本批「V1 全 0」结论时须知口径 = 34 锚活面，勿误读为全仓零字面。
3. **nit**：`debt-ledger-shared.mjs` 末行无换行符——与旧 `crq-ledger-shared.mjs` 逐字节同形（末字节均 `}` 无 LF），沿袭非本批引入，不处置。

## 结论

实际落地 == FROZEN PROPOSAL v0.3 landing 清单：五改法逐文件语义保真、无私货、无漏改、无与批定矛盾；敏感面（scope-map 写槽 / adjudicator def / review-loop 邻接件）改动逐字核对均为纯 token 平移、零权限或写域变更；验证集与结构门全部亲跑绿。**放行建议成立**；merge 判定归 human（human 已指示 main 在途改动期间暂缓，本 verdict 不涉及）。
