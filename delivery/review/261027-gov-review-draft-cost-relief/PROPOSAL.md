# PROPOSAL · 261027-gov-review-draft-cost-relief

> **状态：FROZEN v0.2 · 2026-10-27（凭 human 立项 ack + R2 GREEN 标冻结；N1 junction 清理归主线程落地执行。沿革：DRAFT v0.1 → R1 BLOCK(F1,F2) + F3 NIT → DRAFT v0.2 → R2 GREEN。Track B 主路径；T1–T3 修法 + T4 条目结项一批）。**
> **承载 ARQ**：`ARQ-261027-draft-sandbox-assembly-scaffolding`（SHOULD；本批 = 条目「方向裁定追注」收敛后的修法集落地批，T1–T3 对应三修法、T4 结项，条目整条结项）。来源 = workstream `gov-debt-reduction` P6.5 节（`delivery/workstreams/gov-debt-reduction/task_plan.md:95-99`，立项 ack 原话「立项吧」）+ ARQ「方向裁定追注」。
> **review 轨**：**Track B 主路径**（human 2026-10-27 立项 ack 定；T1 触 `.claude/scripts/**`，按 [gov-change.md](../../.claude/references/gov-change.md) 轻路径判据③「机器面命中即恒 Track B」）。Track A 段判据均不命中：非跨系统结构性变更（不改固件 / shim / 契约 / spec 两文件）；方向由 human 专项讨论裁定收敛（否决写权通道 ① 与承诺粒度降级 ③、取两便宜项 ②⑤ + 新规则）；无声明红队。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**medium**（一句话证成：T1 新增一条具备文件写入能力的工具——虽以「输出根强制仓外」对 universal `bashDenyWrite` 的 `.claude/**` 红线做工具侧镜像，但它仍是一条新的写路径，按「新写路径」而非「纯文本句」审；T2/T3 为 G2 文面、T4 为账本结项。失败模式与缓释见 §2 T1 末）。
> **review profile**：**G2+G3**（G3 = 新增 `.claude/scripts/assemble-edits.mjs`；G2 = `docs/templates/PROPOSAL.template.md` 治理制品骨架 + `.claude/references/gov-change.md` 治理面流程 SSOT；另触 `docs/debt/gov/**` 条目结项记账）。**G1 零触碰、契约面零触碰、hooks / scope-map / settings / verify-gates / 既有门脚本零触碰、无新增门、无门判定链变化。**
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/assemble-edits.mjs
.claude/references/gov-change.md
docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md
docs/templates/PROPOSAL.template.md
```

---

## 0. 想实现的效果与差距

**目标效果**（P6.5 批，human 2026-10-27 立项 ack；方向原话见 ARQ「方向裁定追注」）：

1. **起草期装配从「每批现写一次性脚手架」变成「一条命令 + 编辑清单」**：`assemble-edits.mjs` 以旧串锚定装配——逐条断言 `oldStr` 在现行磁盘内容中恰命中 1 次（0 次 / 多次即中止）、多文件顺序施加、输出根强制仓外、逐件 sha256 回报。清单的 `oldStr` 就是 §2 的「原文」段，装配与引文同源，天然抗行号漂移。
2. **复跑分档**：改稿轮与落地确认轮按影响面窄化复跑（只复跑被改 T 的验证项），全量复跑义务钉在两个时点——FROZEN 标注前、落地后核验前。
3. **脚本批拆分纪律成文**：`.claude/scripts/**` 净增 >~150 行、或 feature 级改动（新增子命令 / 判定重写）单独成批；fix 级小改（<~50 行）可并批搭车。初值随复发校准。
4. **模板骨架自带指路**：§5 增装配句，后续起草者从模板骨架里就知道有该工具，不再只靠批间口口相传。
5. **ARQ-261027 如实结项**（T4；frontmatter 翻转 + 文末 append 注记，`debt-render` 按 status 归位）。

**现状差距**（起草期 2026-10-27 实跑，命令与输出见 §2.0 与 §5 基线）：

- 共享装配器不存在：`ls .claude/scripts/` 无 assemble 类脚本；`grep -rlF "assemble-edits"`（语料 = 全仓 `.md` / `.mjs` / `.json`，排除本批目录）**0 文件**；`lib/` 五件均为门共享解析面（`governance-gate-shared` / `acceptance-id-shared` / `changeid-shared` / `current-pointer-shared` / `transcript-telemetry`），无装配语义。
- 模板 §5 无复跑档位：现行只有全量口径（`docs/templates/PROPOSAL.template.md:87` 开头段 + `:89` 三规则）；起草期自检句四句（`:69`），无脚本批自查。
- `gov-change.md` 起草节三条 bullet（`.claude/references/gov-change.md:12-14`），无批拆分纪律。
- ARQ-261027 `status: deferred`，条目末句自述「本条目待三修法落批后结项」。

**非目标（本批不做，防 scope creep）**：

- **不动写权矩阵**：不给 architect 起草期写槽（human 否决修法 ①，原话「它如果真的把哪个闸改坏了，确实蛮危险的」）；本工具落点强制仓外，恰是该否决的工具侧落地。
- **不降 §2 承诺粒度**：逐字节承诺与 `--cite-check` 对账形态维持（human 否决修法 ③）。
- **不溯及本批**：拆分纪律自落地起对后续批生效（本批为立法批；本批自身形态如实申报，见 §4 取舍 H 与 §6 留根 3）。
- **不改任何门**：`.claude/hooks/**` / `.claude/scope-map.json` / `*.settings.json` / `.claude/verify-gates.json` / `pre-merge-escalation-gate.mjs` / `landing-fidelity-check.mjs` / `lib/governance-gate-shared.mjs` 零 diff；装配器不挂任何闸链。
- **不改契约面与 spec 两文件**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零 diff。
- **不动实现面**：`firmware/**` / `server/**` / `tools/**` 零 diff。
- **不引入 insert-after 语法**：插入行用「oldStr = 锚行、newStr = 锚行 + 新文本」表达（清单形态最小）。
- **不引入新机制**：新增工具脚本不属新子系统 / 新外部依赖集成 / 数据迁移（无第三方依赖，只用 Node 标准库），按 [change-triage.md](../../.claude/references/change-triage.md)「新机制批判定」不命中。

## 2. 问题、根因与改法（T1…T4）

> **新机制 spike 申报**：本批不引入新机制——新增工具脚本不属「新子系统 / 新外部依赖集成 / 新数据迁移」（无第三方依赖、只用 Node 标准库），按 [change-triage.md](../../.claude/references/change-triage.md)「新机制批判定」不命中。

### 2.0 复证结论（起草期 2026-10-27；ARQ 行是待验证假设）

> 纪律：每条结论对应实跑命令或亲读文件；ARQ 原文的机制描述按「搬运引据视同新断言复证」逐条核过，漂移处如实更正。

**复证 1 · human 方向裁定与立项 ack 原样在场。**
- 条目「方向裁定追注」（`docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md:26`）关键句：「**human 裁定：不走 dev-loop**——「它如果真的把哪个闸改坏了，确实蛮危险的」（执法层自改风险否决写权通道 ① 与 dev-loop 路由 ③）。**收敛后的修法集 = 两便宜项 + 一新规则**：② 装配工具化……⑤ 复跑分档……**新规则 = 脚本批拆分纪律**——`.claude/scripts/**` 净增 >~150 行（初值、随复发校准）或 feature 级改动（新增子命令 / 判定重写）单独成批，fix 级小改（<~50 行）可并批搭车……①写权通道与③承诺粒度降级**否决**。」
- task_plan P6.5 立项 ack（`delivery/workstreams/gov-debt-reduction/task_plan.md:99`）写道：「**立项 ack（2026-10-27 human）**：「立项吧」。changeId = `261027-gov-review-draft-cost-relief`，Track B（触 `.claude/scripts/**`）。T1 落地后本批起草即自消费试用。」
- 两处片段起草期 `grep -cF` 各命中 1 次（§5 B9）；条目 `:26` 全行逐字切片进 T4。

**复证 2 · 一次性脚手架两例实况与两条代价（P5 / P6）。**
- P6 脚手架形态（条目 `:16`）：「读仓内 775 行原脚本 → 行号降序替换/插入清单 11 处 → 每处先核首末行锚文 fail-loud → 落仓外 `C:/tmp/p6-assembly/`」——行号定位 + 首末行锚文核对：安全但脆（上游一动即中止，须重排清单）。
- P5 同族摩擦（P5 retro `:45` F1，逐字）：「现象 = R1 抓 T2 编辑 B/C/D 三处行号漂移、R2 抓 A0 括注一处同类、R3 抓 §2.0 考据行未随 N2 同步——三处全是 file:line 引据类断言 → 根因 = PROPOSAL 多轮改稿中，改动点自身行号随编辑漂移，而「引据行号」不在起草自检的重核清单里（装配锚有 fail-loud 兜底、散文引据行号没有）」。
- 同纪律二次发明（P6 PROPOSAL §5 断言纪律行，`delivery/review/261027-gov-review-p6-landing-gate-cluster/PROPOSAL.md:878`）：「装配脚本 `_scratch/assemble-script.mjs` 从本稿 §2 的片段文件装配、不手抄，每处旧串先断言恰 1 次」「文档面预期值 = 仓外副本实跑（`_scratch/assemble-docs.mjs`，旧串 `split(old).length-1 === 1` fail-loud）」——同一纪律 P5 / P6 各写一遍、控制流相同、代码不同、批后即删。
- 复跑税（P6 retro `:23` / `:28` / `:34` / `:39`，逐字片段）：「改稿轮 | 369K | 22% | 文本级四项修订却带全套复跑」；「落地执行轮的 architect resume 378K 大部分是「确认收口 + 复跑」的确认税——沙盘复跑应更窄化」；「花费最大处：改稿轮 + 落地确认轮的**复跑全套**（preflight + cite-check + 一致性 40/40 + V6 干跑）——单行改动也复跑全套是本批最大可压缩点」；「**F2 · 复跑全套税**：现象 = v0.2 改稿轮为四项文本级修订复跑全套沙盘验证（369K）；……根因 = PROPOSAL §5 未区分「全量复跑」与「影响面窄化复跑」档位」。
- feature / fix 混装（ARQ `:26` 实测账）：P6 单文件 +387/−33（净 354 行，自检夹具约 1/4–1/3、纯逻辑 ~250 行）vs PROPOSAL 1035 行 vs 全批 ~1.69M tokens；复盘口径「feature 级 cite-check ~306 行与 fix 级 T2/T3 ~114 行同批，fix 陪 feature 走全套 apparatus 是主要浪费」。

**复证 3 · 落点现状与符号碰撞（亲核）。**
- `ls .claude/scripts/` = 42 件 `.mjs` + `lib/` 五件；assemble 类零命中。`grep -rlF "assemble-edits"` = 0 文件（排除本批目录）。
- 新符号碰撞（同语料、排除本批目录）：`assemble-edits` / `isInsideRepo` / `countOccurrences` / `eolHint` / `writePlan` / `assemble(` = **0**；`runSelfCheck` 与 8 件既有脚本 + 2 件 hooks 的本地函数同名（共 10 件）——本批改名 `runAssembleSelfCheck`，改名后 0 命中（§3 census 留痕）。
- 待改行现状：模板 `:87`（验证集开头段）、`:89`（三规则表头）、`:69`（自检句四句段）；gov-change `:14`（判档 bullet，其后无同级 bullet）；ARQ `:26` 末行 + frontmatter 三字段——全部经注入器逐字切片进 §2（§5 B9 注入对账表）。

**复证 4 · 本批基线（供给侧，全部起草期实跑）。**
- 原型 `--self-check`：**16/16**、exit 0（B1）。
- 判别性变异：**9/9**——基线绿 + m1–m8 各自变红（B2；变异清单见 §5）。
- 真仓装配（本批清单 → 仓外 `C:/tmp/dcr-assembly`）：ok:true、4 件、逐件 sha256（B3）；装配副本 `node --check` 过、`--self-check` 16/16（B4）。
- 输出根仓内拦截实跑：exit 2 + 拦截消息逐字（B5）。
- 原型 `wc -l` = **201**（工具自报 `afterLines` 202 = `split('\n')` 片段数，文件以 LF 结尾）。
- 行尾：三件跟踪目标文件（模板 / gov-change / ARQ）现文 `git ls-files --eol` 实测均 `i/lf w/lf`（本 gov worktree 实况）；新增脚本为未跟踪件、不进该命令，落地后按同口径 + V1 的 sha256 对账复核（R1-F3 更正口径）。
- 仓内 `_scratch/` 纪律：本批脚手架（原型 / 注入器 / 变异器 / 清单）在 `delivery/review/261027-gov-review-draft-cost-relief/_scratch/`，落地 commit 前删除（V5 正向断言机械兜底）。

### T1 · 共享锚文装配器（旧串锚定替代行号基线）

**问题**：architect 起草 PROPOSAL 不落 canonical（`landing` 恒 = 主线程 packet-bound），而 §2 硬条款要求逐字节承诺「改哪几行、改成什么样」、§5 要求验证集实跑回填——两者合力使「装配出仓外副本」成为每批不可省的起草期动作，但仓里没有共享载体：P5 / P6 各写一遍一次性脚手架、批后即删（P6 版行号定位 + 首末行锚文核对），质量取决于当批起草者自觉。

**证据**：§2.0 复证 2（P6 行号 + 首末行锚文形态、P5 F1 行号漂移三连、P6 二次发明原文、两批的复跑 / 混装代价）；ARQ `:16`（影响面四条）；`assemble-edits` 全仓 0 命中（复证 3）。

**根因**：① **装配动作不可省**——逐字节承诺 × 实跑回填 × 无 canonical 写权的结构性张力，只能靠「仓外副本」消化；② **行号是位置而不是内容**——P6 式行号基线把「上游不变」当隐含前提，多文件同批编辑下上游移动是常态，漂移即中止（安全但每次重排）；而 §2 的「原文」引文本身就是内容锚，逐批却被重新发明成行号清单；③ **无共享载体 → 无质量下限**：脚手架写法、fail-loud 语义、落点纪律全靠当批自觉，没有模板、没有 checklist 句可引。

**改法**（新增脚本一件 + 模板一处）：

一、**新增 `.claude/scripts/assemble-edits.mjs`（全文如下，落地照抄）**——设计要点逐条对根因：

| # | 设计点 | 做什么 | 对哪个根因 / 为什么 |
|---|---|---|---|
| 1 | 旧串锚定 | `oldStr` 在当前磁盘内容上 `split(old).length - 1` 断言恰 1 次；0 次（漂移 / 清单陈旧）或多次（锚太短）即中止 | 对根因②：锚串是内容，上游增删行不改命中；且锚串同时是 §2 的原文段——装配与引文同源 |
| 2 | 全部通过才写盘 | `assemble()` 是纯函数，`failures` 非空则 `plan` 恒空；写盘是第二阶段 | 副本不出现半装配态：判官 / 核验官看到的副本要么是完整改法集、要么不存在 |
| 3 | 顺序施加 | 同一文件多条按清单顺序，后一条的 `oldStr` 在前一条结果上查找 | 链式编辑可用（先插入、再在新文本上继续改），无需行号 |
| 4 | 行尾原样 | 纯字符串替换，不做 LF / CRLF 归一、不剥 BOM；0 命中且目标文件为 CRLF、清单为 LF 时给诊断句（提示不代偿） | 字节保真：副本必须等于「按清单施加后的字节」；归一化会静默改写字节（B2 m3 实测） |
| 5 | 插入形态 | 插入行 = `oldStr` 取锚行、`newStr` = 锚行 + 新文本 | 清单形态最小：不新增 insert-after / 行区间语法，每条只回答「从什么到什么」 |
| 6 | 函数式替换 | `replace(oldStr, () => newStr)`——`$&` / `$1` 按字面落盘 | 字符串式 replace 会解释 `$` 模式、把文档里的 `$&` 类字面改坏（B2 m2 实测） |
| 7 | 输出根强制仓外（含链接解析） | `isInsideRepo` 先对最深存在祖先 `realpathSync`、再拼回不存在尾段后判仓界；为真 → exit 2，零写盘 | 根因③ 的机械下限：architect 不落 canonical；与 universal `bashDenyWrite` 的 `.claude/**` 红线同向（工具永远做不成仓内写通道；junction / 符号链接不构成绕行，R1-F2） |
| 8 | `copy{from,to}` | 原样复制伴随文件（脚本 import 的 lib / `_scratch` 里的新文件原型 / 其它目标文件） | 副本要可跑：单件脚本脱离 `lib/` 跑不起来；新文件批的「落地全文」不来自仓内旧文 |
| 9 | sha256 回报 | 逐件 `beforeSha256` / `afterSha256` / `afterLines` | 副本 = 起草者、判官、核验官三方共用的 byte 对账基准 |
| 10 | 自检 16 断言 | 0 命中 / 多命中 / byte 等值 / 链式 / 插入 / CRLF / `$&` / 零写盘 / copy / 仓界双向 / 真实路径（不存在尾段 + junction） / CRLF 诊断 | 判别性：反转实现即红——8 条变异 9/9（B2） |

新增文件全文（逐字节，落地照抄）：
```js
#!/usr/bin/env node
// 锚文装配器（assemble-edits）——治理批起草期共享沙盘装配器（261027-gov-review-draft-cost-relief T1 · ARQ-261027）
//
// 补的缺口：architect 起草 PROPOSAL 但不落 canonical、§2 又须逐字节承诺改法，于是 P5 / P6 每批现写一次性
// 装配脚手架、批后即删；P6 版按行号降序替换 / 插入、每处先核首末行锚文——上游一动即漂移中止、清单须重排。
// 本工具以「旧串锚定」替代行号基线——
// 清单逐条（file / oldStr / newStr / label）断言 oldStr 在现行磁盘内容中恰命中 1 次，多文件顺序施加，
// 落仓外副本并回报 sha256。旧串 = §2 的逐字引文，天然抗漂移：上游漂移 → 0 命中 → fail-loud 中止。
//
// 用法（cwd = 仓根；仓根 = CLAUDE_PROJECT_DIR || cwd，与仓内其它脚本同口径）：
//   node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>
//   node .claude/scripts/assemble-edits.mjs --self-check
// 编辑清单（JSON）：
//   { "edits": [ { "file": "<仓根相对路径>", "label": "<人读名>", "oldStr": "…", "newStr": "…" } ],
//     "copy":  [ "<仓根相对路径>" | { "from": "<仓根相对路径>", "to": "<仓根相对路径>，可省 = from" } ] }
//   copy 可省：原样复制的伴随文件（如脚本 import 的 lib/、_scratch 里的新文件原型），让副本可跑。
//
// 语义：
//   1. 逐条断言恰 1 次命中：0 次（上游漂移 / 清单陈旧）与多次（锚串太短）都中止；全部断言通过才写盘——
//      副本不出现半装配态。
//   2. 顺序施加：同一文件多条按清单顺序，后一条的 oldStr 在前一条的结果上查找（链式编辑可用）。
//   3. 字符串操作保行尾原样（不做 LF/CRLF 归一、不剥 BOM）；插入行 = oldStr 取锚行、newStr = 锚行 + 新文本。
//   4. 输出根必须在仓外（architect 不落 canonical 的机械兜底）——指向仓内直接 exit 2；仓界判定先对最深
//      存在祖先 realpath（符号链接 / junction 不构成绕行，见 isInsideRepo 注）。
// 退出码：0 全部命中且写盘成功 / 1 断言失败（未写任何文件）/ 2 用法或输入错误（含输出根在仓内）。

import { existsSync, mkdirSync, mkdtempSync, readFileSync, realpathSync, rmSync, symlinkSync, unlinkSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { basename, dirname, isAbsolute, join, relative, resolve } from 'node:path';
import { tmpdir } from 'node:os';

const projectDir = resolve(process.env.CLAUDE_PROJECT_DIR || process.cwd());

const sha256 = (s) => createHash('sha256').update(s, 'utf8').digest('hex');
const countOccurrences = (hay, needle) => hay.split(needle).length - 1;
// 0 命中且目标文件为 CRLF、清单为 LF 时给出诊断——本工具不归一（语义 3），提示而不是代偿。
const eolHint = (content, oldStr) => (oldStr.includes('\n') && !oldStr.includes('\r\n') && content.includes('\r\n')
  ? '（目标文件含 CRLF、清单为 LF——本工具不归一，请按现行行尾写清单）' : '');

// 仓内判定（261027 批 R1-F2 加固）：先对「最深存在祖先」做 realpath 再判，最后拼回不存在的尾部。
// resolve + relative 不解析符号链接 / junction——蓄意构造的 junction 可让仓内路径被判「仓外」（R1 实测两向：
// 仓内 junction 被放行、反向 junction 被当输出根）；realpath 之后链接面不再有绕法（自检含 junction 断言）。
function realpathDeepest(p) {
  let cur = resolve(p);
  const tail = [];
  while (!existsSync(cur)) {
    const parent = dirname(cur);
    if (parent === cur) return cur;
    tail.unshift(basename(cur));
    cur = parent;
  }
  return join(realpathSync(cur), ...tail);
}

function isInsideRepo(abs, repoRoot) {
  const rel = relative(realpathDeepest(repoRoot), realpathDeepest(abs));
  return rel === '' || (!rel.startsWith('..') && !isAbsolute(rel));
}

// 纯函数装配（不写盘）：返回 { ok, plan, failures }；任一断言失败 → plan 恒空（中止语义）。
function assemble(list, repoRoot, readFile = (p) => readFileSync(p, 'utf8')) {
  const failures = [];
  const entries = new Map();
  const edits = list && Array.isArray(list.edits) ? list.edits : [];
  const copyList = Array.isArray(list && list.copy) ? list.copy : [];
  if (!edits.length && !copyList.length) failures.push({ kind: 'input', message: '清单缺 edits 与 copy（至少一件）' });
  edits.forEach((e, i) => {
    const label = (e && e.label) || `#${i + 1}`;
    const rel = e && e.file;
    if (typeof rel !== 'string' || !rel || isAbsolute(rel) || !isInsideRepo(resolve(repoRoot, rel), repoRoot)) {
      failures.push({ kind: 'input', index: i, label, message: 'file 须为仓根内相对路径' });
      return;
    }
    if (typeof e.oldStr !== 'string' || !e.oldStr) { failures.push({ kind: 'input', index: i, label, file: rel, message: 'oldStr 须为非空字符串' }); return; }
    if (typeof e.newStr !== 'string') { failures.push({ kind: 'input', index: i, label, file: rel, message: 'newStr 须为字符串' }); return; }
    let entry = entries.get(rel);
    if (!entry) {
      const abs = resolve(repoRoot, rel);
      if (!existsSync(abs)) { failures.push({ kind: 'missing-file', index: i, label, file: rel, message: '目标文件不存在' }); return; }
      const text = readFile(abs);
      entry = { before: text, content: text, labels: [] };
      entries.set(rel, entry);
    }
    const n = countOccurrences(entry.content, e.oldStr);
    if (n !== 1) {
      const why = n === 0 ? 'oldStr 命中 0 次（上游漂移或清单陈旧）' : `oldStr 命中 ${n} 次（须恰 1 次：加长锚串）`;
      failures.push({ kind: 'anchor-count', index: i, label, file: rel, count: n, message: why + eolHint(entry.content, e.oldStr) });
      return;
    }
    // 函数式替换：newStr 中的 $& / $1 等按字面落盘（字符串式 replace 会把它们当替换模式解释）。
    entry.content = entry.content.replace(e.oldStr, () => e.newStr);
    entry.labels.push(label);
  });
  const copies = [];
  for (const c of copyList) {
    const from = typeof c === 'string' ? c : c && c.from;
    const to = typeof c === 'string' ? c : (c && c.to) || c.from;
    const bad = (p) => typeof p !== 'string' || !p || isAbsolute(p) || !isInsideRepo(resolve(repoRoot, p), repoRoot);
    if (bad(from) || bad(to)) { failures.push({ kind: 'input', copy: c, message: 'copy 条目须为仓根内相对路径' }); continue; }
    const abs = resolve(repoRoot, from);
    if (!existsSync(abs)) { failures.push({ kind: 'missing-file', file: from, message: 'copy 源文件不存在' }); continue; }
    copies.push({ rel: to, text: readFile(abs) });
  }
  if (failures.length) return { ok: false, plan: [], failures };
  const plan = [...entries].map(([rel, x]) => ({ rel, before: x.before, after: x.content, labels: x.labels }))
    .concat(copies.map((c) => ({ rel: c.rel, before: c.text, after: c.text, labels: ['copy'] })));
  return { ok: true, plan, failures: [] };
}

function writePlan(plan, outRoot) {
  return plan.map((p) => {
    const abs = resolve(outRoot, p.rel);
    mkdirSync(dirname(abs), { recursive: true });
    writeFileSync(abs, p.after, 'utf8');
    return { path: p.rel, edits: p.labels, beforeSha256: sha256(p.before), afterSha256: sha256(p.after), afterLines: p.after.split('\n').length };
  });
}

// 自检（判别性 = 反转实现必须变红）：0 命中 / 多命中中止；成功拷贝 byte 等值；链式顺序；插入形态；
// CRLF 原样（反转 = 归一 → 行尾断言红）；$& 字面（反转 = 字符串式 replace → 字面断言红）；失败即零写盘；
// copy{from,to}；输出根仓内判定；真实路径（不存在尾段拼回 + junction 指仓内须拦）；CRLF 诊断句。
function runAssembleSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'assemble-edits', label, got, want })); }
  };
  const root = mkdtempSync(join(tmpdir(), 'assemble-edits-'));
  const repo = join(root, 'repo');
  mkdirSync(join(repo, 'sub'), { recursive: true });
  writeFileSync(join(repo, 'a.txt'), '头\n锚行\n尾\n', 'utf8');
  writeFileSync(join(repo, 'b.txt'), '重复\n重复\n', 'utf8');
  writeFileSync(join(repo, 'crlf.txt'), '甲\r\n乙\r\n丙\r\n', 'utf8');
  writeFileSync(join(repo, 'sub', 'src.mjs'), '导出甲\n', 'utf8');
  const one = (file, oldStr, newStr) => assemble({ edits: [{ file, oldStr, newStr }] }, repo);
  const after1 = (r) => (r.plan[0] || {}).after;
  const fail1 = (r) => r.failures[0] || {};
  check('0命中·中止', fail1(one('a.txt', '不存在的串', 'x')).count, 0);
  check('多命中·中止', fail1(one('b.txt', '重复', 'x')).count, 2);
  const okRes = one('a.txt', '锚行', '锚行');
  check('成功·ok', okRes.ok, true);
  check('成功·byte 等值', after1(okRes), '头\n锚行\n尾\n');
  const chain = assemble({ edits: [
    { file: 'a.txt', oldStr: '锚行', newStr: '锚行·甲' },
    { file: 'a.txt', oldStr: '锚行·甲\n尾', newStr: '链\n尾' },
  ] }, repo);
  check('链式·第二条在前一条结果上查找', after1(chain), '头\n链\n尾\n');
  const ins = one('a.txt', '锚行', '锚行\n新增行');
  check('插入形态·锚行保留且恰 1 次', countOccurrences(after1(ins) || '', '锚行'), 1);
  check('插入形态·新行落位', after1(ins), '头\n锚行\n新增行\n尾\n');
  const crlf = one('crlf.txt', '乙\r\n', '乙\r\n新增\r\n');
  check('CRLF·行尾原样（CRLF 计数 = 纯 LF 计数 = 4）', JSON.stringify([countOccurrences(after1(crlf) || '', '\r\n'), countOccurrences(after1(crlf) || '', '\n')]), JSON.stringify([4, 4]));
  const dollar = one('a.txt', '锚行', '锚行$&$1');
  check('$& 字面落盘', after1(dollar), '头\n锚行$&$1\n尾\n');
  const mixed = assemble({ edits: [{ file: 'a.txt', oldStr: '锚行', newStr: 'X' }, { file: 'b.txt', oldStr: '无', newStr: 'y' }] }, repo);
  check('任一失败·中止且零写盘计划', JSON.stringify([mixed.ok, mixed.plan.length]), JSON.stringify([false, 0]));
  const cp = assemble({ edits: [{ file: 'a.txt', oldStr: '头', newStr: '首' }], copy: [{ from: 'sub/src.mjs', to: 'sub/dst.mjs' }] }, repo);
  check('copy{from,to}·同字节', (cp.plan.find((p) => p.rel === 'sub/dst.mjs') || {}).after, '导出甲\n');
  check('输出根仓内·拦截', isInsideRepo(join(repo, 'out'), repo), true);
  check('输出根仓外·放行', isInsideRepo(join(root, 'out'), repo), false);
  // 真实路径（261027 批 R1-F2）：① 不存在尾段须拼回最深存在祖先；② 指向仓内的 junction 不得被当「仓外」。
  check('真实路径·不存在尾段拼回最深存在祖先', realpathDeepest(join(root, 'no', 'such', 'x')), join(realpathSync(root), 'no', 'such', 'x'));
  let linked = true;
  try { symlinkSync(repo, join(root, 'link'), 'junction'); } catch { linked = false; }
  if (!linked) console.error(JSON.stringify({ selfCheck: 'assemble-edits', skip: 'junction 不可建（平台 / 权限）——链接面断言按跳过记绿，真空绿风险如实申报' }));
  check('junction 指仓内·须拦（不可建时跳过记绿）', linked ? isInsideRepo(join(root, 'link', 'out'), repo) : true, true);
  writeFileSync(join(repo, 'crlf2.txt'), '甲\r\n乙\r\n', 'utf8');
  check('CRLF 与 LF 清单不符·诊断句', String(fail1(one('crlf2.txt', '乙\n', '丙\n')).message).includes('CRLF'), true);
  if (linked) { try { unlinkSync(join(root, 'link')); } catch { /* 已不存在 */ } }
  rmSync(root, { recursive: true, force: true });
  console.log(JSON.stringify({ selfCheck: 'assemble-edits', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

function main(argv) {
  if (argv.includes('--self-check')) { runAssembleSelfCheck(); return; }
  const [listArg, outArg] = argv;
  if (!listArg || !outArg) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', message: '用法：assemble-edits.mjs <编辑清单.json> <仓外输出根>；或 --self-check' }));
    process.exit(2);
  }
  let list;
  try { list = JSON.parse(readFileSync(resolve(projectDir, listArg), 'utf8')); } catch (e) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', message: `清单不可读或非法 JSON：${e.message}` }));
    process.exit(2);
  }
  const outAbs = resolve(outArg);
  if (isInsideRepo(outAbs, projectDir)) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', outputRoot: outAbs, message: '输出根在仓内——沙盘副本必须落仓外（architect 不落 canonical 的机械兜底）' }));
    process.exit(2);
  }
  const res = assemble(list, projectDir);
  if (!res.ok) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', failures: res.failures, note: '断言失败：未写任何文件' }));
    process.exit(1);
  }
  const files = writePlan(res.plan, outAbs);
  console.log(JSON.stringify({ ok: true, gate: 'assemble-edits', outputRoot: outAbs, files }));
}

main(process.argv.slice(2));
```

二、**模板 §5 装配句（`docs/templates/PROPOSAL.template.md:87` 整行替换）**——把「有该工具」写进骨架，后续起草者不再靠口口相传。这是 T1 的可发现性载体；不写进 `architect.md` 的理由见 §4 取舍 G 与 §6 留根 4（自改递归面，需单独 human 批）。

原文（`docs/templates/PROPOSAL.template.md:87`，逐字节）：
```text
每条机械可判：命令 + 预期。自检/重放类遵守真空绿规约（重放先断言输入非空；自检验证输出含新增自检名 + 调用顺序审查；环境红先立基线）。
```

改后（逐字节）：
```text
每条机械可判：命令 + 预期。自检/重放类遵守真空绿规约（重放先断言输入非空；自检验证输出含新增自检名 + 调用顺序审查；环境红先立基线）。**装配**：§5 实跑副本一律用共享锚文装配器 `node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>` 装配（261027 批 T1，ARQ-261027）——旧串锚定、逐条断言恰命中 1 次（0 次 / 多次即中止，全部命中后才写盘）、行尾原样不归一、输出根强制在仓外；清单 = §2 编辑的 JSON 形态（file / oldStr / newStr / label），不再每批现写一次性脚手架。
```

**为什么这样改**：对三个根因一一对应——① 装配动作固化成一个入仓工具，起草者每批只写 JSON 清单（oldStr / newStr / label 逐条），脚手架不再现写现删；② 旧串锚定把「§2 原文段 = 装配锚」这条本来就成立的同源关系显式化：漂移 → 0 命中 → fail-loud，漂移被指向内容而不是行号，P5 F1 式「改动点自身行号随编辑漂移」不再有载体可漂；③ 落点纪律（仓外、零写盘中止、函数式替换、不归一）从「自觉」变成「再实现一遍也没用」——工具行为由自检 16 断言 + 变异 8 条背书，下批起草者引用它而不是重写它。
**为什么落 `.claude/scripts/` 而不是 `lib/`**：`lib/` 五件的定义是「被多个脚本 import 的共享解析面」；本工具是独立 CLI、无 import 关系、单文件可拷可跑（B3 的 `copy` 用法即演示）。放进 `lib/` 反而要求所有消费方按模块解析，且它与门判定链无关，不能沾「门共享面」的评审重量。
**为什么不把「输出根仓外」做成可配**：可配 = 写权通道的复辟（human 否决 ① 的实质）；仓内输出这一条能力一旦存在，任何批都可以拿本工具当 canonical 写路径。强制常量 + 自检 m6 是这条红线的最小实现。
**失败模式与缓释**（riskTier medium 的对应）：① 输出根判定漏洞（含链接面）→ 自检「输出根仓内·拦截 / 仓外·放行 / 真实路径·不存在尾段拼回 / junction 指仓内·须拦」四断言 + m6 / m8 变异实测（R1-F2 后 junction 绕过面闭合：`mklink /J` 到仓内后作输出根实跑 exit 2、零写盘）；② 误写/改写源文件 → 工具对源文件只 `readFileSync`，写盘只发生在 `outRoot` 下（`writePlan` 是唯一写点，路径 = `outRoot + 仓根相对路径`）；③ 本工具被当成写权通道 → 仓外强制 + 本 PROPOSAL §0 非目标 + 判官可核输出里回报的 `outputRoot`；④ CRLF 清单失配 → 诊断句把 0 命中归因到行尾而不是让人盲改锚串。

目标文件：`.claude/scripts/assemble-edits.mjs`（新增） · `docs/templates/PROPOSAL.template.md`（改）

### T2 · 复跑分档句（治改稿轮 / 落地确认轮的复跑全套税）

**问题**：P6 批改稿轮为四项文本级修订复跑全套沙盘验证（preflight + cite-check + 一致性 40/40 + V6 干跑），烧 369K tokens（占全批 22%）；落地确认轮同样全套（378K，大部分是「确认收口 + 复跑」的确认税）。复跑本身承重，但只在两个时点承重：FROZEN 标注前（验证集可判性）与落地后核验前（落地 == FROZEN）。中间的改稿轮全量复跑重复付出、不增加证据。

**证据**：§2.0 复证 2（P6 retro `:23` / `:28` / `:34` / `:39` 逐字）；模板 §5 现文只有全量口径（`:87` 开头段 + `:89` 三规则，逐字切片见下方改法）。

**根因**：模板 §5 把「验证集」定义为一份全量清单（命令 + 预期 + 时点），没有定义**轮次档位**——改稿轮的复跑范围因此默认等于全量，起草者与判官都没有第二档可引用，窄化复跑看起来像偷工。

**改法**（模板 §5 一处：在「每条机械可判」段与三规则硬项之间插入一段，三规则本身一字不动）：

原文（`docs/templates/PROPOSAL.template.md:89`，逐字节）：
```text
**【硬项 · 验证集撰写三规则】**（261026 批合写；ARQ-261021 / ARQ-261019 / ARQ-261025 各一）
```

改后（逐字节，`:89` 起两段）：
```text
**复跑分档**：改稿轮（评审 findings 修订后）与落地确认轮按影响面窄化复跑——只复跑被改 T 的验证项，并在修订记录里列明复跑范围；**全量复跑义务保留在两个时点**：FROZEN 标注前、落地后核验前，两处不得窄化（261027 批 T2 立，ARQ-261027 复跑税）。

**【硬项 · 验证集撰写三规则】**（261026 批合写；ARQ-261021 / ARQ-261019 / ARQ-261025 各一）
```

**为什么这样改**：把「窄化」升格为模板承认的档位而不是默许——同一句里钉死两个不得窄化的时点（FROZEN 标注前、落地后核验前），窄化只在中间轮次合法；并要求「在修订记录里列明复跑范围」，让判官能对账「窄化是否成立」（判据 = 复跑记录列明的项 ⊆ 被改 T 的验证项 + 受其波及的条目），而不是凭感觉判。本 T 与 T3 同文件但互不覆盖：T2 只加段、不改规则；T1 的装配句在 `:87` 段内，三处锚文互不重叠。

目标文件：`docs/templates/PROPOSAL.template.md`（改）

### T3 · 脚本批拆分纪律（`.claude/scripts/**` 的 feature / fix 分道）

**问题**：P6 批把 feature 级 `--cite-check`（~306 行，新增子命令）与 fix 级 T2/T3（~114 行）装进同一批，fix 级陪着 feature 级走完整套 apparatus——起草、两轮评审、落地核验、retro 各一遍。批边界靠临时判断，没有成文阈值可引。

**证据**：§2.0 复证 2（ARQ `:26` 实测账与复盘口径逐字）；P6 批形态本身（PROPOSAL 1035 行 / 全批 ~1.69M tokens）。

**根因**：review / landing / retro 的 apparatus 是**常量成本**，不随 diff 行数线性下降；无成文阈值 → 批边界随「顺手」漂移 → fix 级搭车分摊 feature 级的固定成本，而 feature 级又因混装拿不到独立的全强度评审焦点。

**改法**（两处各一句；判据单源在 gov-change，模板句只作起草时触发）：

一、**`gov-change.md` 起草节新 bullet**（`:14` 判档 bullet 之后）：

原文（`.claude/references/gov-change.md:14`，逐字节）：
```text
- **判档 = architect-reviewer**：轻路径 / Track B，**只升不降**。
```

改后（逐字节，`:14` 起两行）：
```text
- **判档 = architect-reviewer**：轻路径 / Track B，**只升不降**。
- **脚本批拆分纪律（261027 批 T3 立，ARQ-261027）**：`.claude/scripts/**` 净增合计 >~150 行（净增 = `git diff --numstat -- .claude/scripts/` 增行合计减删行合计；新增 / 未跟踪文件先 `git add -N` 再计，或按行数直计），或属 feature 级改动（新增子命令 / 判定重写）→ 该脚本改动**单独成批**；fix 级小改（<~50 行、无判定语义变化）可并批搭车。数值为初值、随复发校准。
```

二、**模板起草期自检句 ⑤ + 表头计数**（`:69` 两处子句）：

原文（`docs/templates/PROPOSAL.template.md:69` 行内片段，逐字节，`grep -cF` = 1）：
```text
261019-gov-review-local-memory-extraction T2 扩第四句——名称沿革保留、实为四句）
```

改后（逐字节）：
```text
261019-gov-review-local-memory-extraction T2 扩第四句、261027-gov-review-draft-cost-relief T3 扩第五句——名称沿革保留、实为五句）
```

原文（`docs/templates/PROPOSAL.template.md:69` 行末片段，逐字节，`grep -cF` = 1）：
```text
两连事故钉死（生成即验证）。
```

改后（逐字节，⑤ 接排）：
```text
两连事故钉死（生成即验证）。⑤ **脚本批拆分自查**：本批 diff 触 `.claude/scripts/**` 时先算净增合计（`git diff --numstat -- .claude/scripts/`，增行合计减删行合计；新增 / 未跟踪文件先 `git add -N` 再计，或按行数直计）——>~150 行、或属 feature 级改动（新增子命令 / 判定重写）→ 依 [gov-change.md](../../.claude/references/gov-change.md)「起草与审稿」节拆分纪律单独成批，并在 §4 列拆分建议交裁；fix 级小改（<~50 行、无判定语义变化）可并批搭车（261027 批 T3 立，初值随复发校准）。
```

**机械可判性（判官可客观判违反）**：净增 = `git diff --numstat -- .claude/scripts/` 的增行合计减删行合计（**未跟踪 / 新增文件不进 numstat**——先 `git add -N` 再计，或按行数直计，R1-F1）；feature 级 = 新增子命令（新 CLI 入口）或判定重写（既有判定函数语义变更，diff 里看得见）；fix 级 = <~50 行且判定函数未变。阈值带「~」是初值（human 原话「初值、随复发校准」），判官按「是否显著超阈值」+「是否 feature 形态」两问判，不抠个位行数。
**本批自指（如实申报）**：本批 `.claude/scripts/**` 净增 = **201 行**（B1 `wc -l`），>~150 初值，且 T1 是新增独立工具（与「新增子命令」同族的 feature 形态）——按本批将立的规则本应单独成批。本批以两条理由并批并留痕：① 本批是**规则立法批**，规则自落地起对后续批生效、不溯及（§0 非目标 + §6 留根 3）；② 四 T 同属 ARQ-261027 单一消费簇、human 一次性 ack（拆开会付两套 review + landing + retro apparatus，恰是规则要治的成本）。此形态在本批后再出现时即按规则拆批。判官可 challenge 此豁免（§4 取舍 H）。

**为什么这样改**：阈值挂在**批的边界**而不是脚本内部——feature 级单独成批保评审深度（P6 的 feature 级 cite-check 值得全强度评审，fix 级不值得陪跑）；fix 级可搭车保小改不被官僚化。两处承载分工：流程 SSOT（gov-change）给判据，模板自检句给起草者动笔时的触发点——模板句显式指回 gov-change，不复述为第二 SSOT。

目标文件：`.claude/references/gov-change.md`（改） · `docs/templates/PROPOSAL.template.md`（改）

### T4 · ARQ-261027 结项记账

**问题**：条目末句自述「本条目待三修法落批后结项」（`:26`）；T1–T3 落地即三修法全落，条目应结项，否则债务账本虚挂。

**证据**：frontmatter `status: deferred` / `closed: ""` / `closedBy: ""`（`grep -cF` 各 = 1）；`:26` 末句逐字在场（切片见下）。

**根因**：结项是 status 的纯函数——`debt-render.mjs` 按 frontmatter 归位 `docs/debt/gov/_archive/<年>/`；不翻 status 就永远挂在未结清单里。

**改法**（frontmatter 三字段翻转 + 文末 append）：

原文（逐字节）：
```yaml
status: deferred
```

原文（逐字节）：
```yaml
closed: ""
```

原文（逐字节）：
```yaml
closedBy: ""
```

改后：`status: closed` / `closed: 2026-10-27` / `closedBy: 261027-gov-review-draft-cost-relief`。

文末尾行（原文 `docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md:26`，逐字节）：
```text
**方向裁定追注（2026-10-27，human 与主线程专项讨论后收敛，append）**：P6 实测账 = 单文件 +387/−33（净 354 行，其中自检夹具约 1/4–1/3、纯逻辑 ~250 行）vs PROPOSAL 1035 行（描述 ≈ 实现 3 倍）vs 全批 ~1.69M tokens；讨论曾评估「脚本批路由 dev-loop 实现」（TaskPackage + 验收测试模型，估 3–5 倍价差）。**human 裁定：不走 dev-loop**——「它如果真的把哪个闸改坏了，确实蛮危险的」（执法层自改风险否决写权通道 ① 与 dev-loop 路由 ③）。**收敛后的修法集 = 两便宜项 + 一新规则**：② 装配工具化（共享锚文装配器入仓，architect 每批只写编辑清单）；⑤ 复跑分档（PROPOSAL §5 验证集区分全量 / 影响面窄化两档，治 retro F2 复跑税）；**新规则 = 脚本批拆分纪律**——`.claude/scripts/**` 净增 >~150 行（初值、随复发校准）或 feature 级改动（新增子命令 / 判定重写）单独成批，fix 级小改（<~50 行）可并批搭车（P6 复盘：feature 级 cite-check ~306 行与 fix 级 T2/T3 ~114 行同批，fix 陪 feature 走全套 apparatus 是主要浪费）。P6 批逐字节评审形态本身**维持**（判官审的就是将落地的代码，R1 抓 V6 断言字面证明其价值）。①写权通道与③承诺粒度降级**否决**。本条目待三修法落批后结项。
```

append 段（逐字节，插在尾行之后、空一行）：
```markdown
**2026-10-27 · 结项（`261027-gov-review-draft-cost-relief`，T1–T3 落地）**：三修法全部落批——① **装配工具化**：`.claude/scripts/assemble-edits.mjs` 入仓（旧串锚定装配器：逐条断言恰 1 次命中、全部通过才写盘、输出根强制仓外、sha256 回报；落地验证 V1 复跑 `--self-check` 16/16）；② **复跑分档**：`docs/templates/PROPOSAL.template.md` §5 增复跑分档句——改稿轮 / 落地确认轮按影响面窄化复跑（只复跑被改 T 的验证项），全量复跑义务保留在 FROZEN 标注前与落地后核验前两个时点；③ **脚本批拆分纪律**：`.claude/references/gov-change.md` 起草节 + 模板起草期自检句 ⑤ 成文——净增 >~150 行或 feature 级改动单独成批、fix 级 <~50 行可搭车（初值随复发校准）。**写权通道 ① 与承诺粒度降级 ③ 维持否决**（human 2026-10-27 裁，理由见上方方向裁定追注）。本条目由 `debt-render` 按 status 归位 `docs/debt/gov/_archive/2026/`。
```

**为什么这样改**：结项事实由文末 append 承载、状态由 frontmatter 承载——沿用本仓 debt 记账形态（P6 批三条目同法），`debt-render` 归位是纯函数、人不搬文件。

目标文件：`docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 强制执行层 G3 | `.claude/scripts/assemble-edits.mjs` | 新增（T1） | 起草期装配器：旧串锚定、恰 1 次 fail-loud、顺序施加、仓外输出、sha256 回报。消费方 = 各治理批起草者（§5 装配）、判官 / 核验官（byte 对账基准）、核验官（落地副本重建） |
| 治理制品骨架 G2 | `docs/templates/PROPOSAL.template.md` | 改（T1② / T2 / T3②） | §5 装配句 + 复跑分档段；起草期自检句 ⑤ 与表头四句→五句。消费方 = 后续全部 PROPOSAL 起草者与判官 |
| 流程定义 G2 | `.claude/references/gov-change.md` | 改（T3①） | 起草节新增脚本批拆分纪律 bullet。消费方 = 治理面起草者、判官 / architect-reviewer（判档时可引） |
| 账本 G4 | `docs/debt/gov/ARQ-261027-draft-sandbox-assembly-scaffolding.md` | 改（T4） | status / closed / closedBy 翻转 + 文末 append；落地后 `debt-render.mjs` 自动归位 `_archive/2026/` |
| 生成件（不在 manifest） | `docs/debt/gov/INDEX.md` · `docs/debt/gov/_archive/2026/INDEX.md` · `delivery/review/INDEX.md` | 再生 | 前者 `debt-render.mjs` 重渲染；后者 `changes-index.mjs --upsert` 单行钉入（禁手改） |
| 指令根 G1 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | 未触 | 零 diff |
| 契约面 | `docs/contracts/**` | 未触 | 零 diff |
| 规范 spec | `docs/requirements.md` / `docs/architecture.md` | 未触 | 零 diff |
| 实现面 | `firmware/**` / `server/**` / `tools/**` | 未触 | 零 diff |
| 其余强制层 | `.claude/hooks/**` / `.claude/scope-map.json` / `*.settings.json` / `.claude/verify-gates.json` / `.claude/scripts/pre-merge-escalation-gate.mjs` / `.claude/scripts/landing-fidelity-check.mjs` / `.claude/scripts/lib/governance-gate-shared.mjs` | 未触 | 零 diff；无新增门、无门判定链变化（装配器不接闸链、不被任何脚本 import） |

**【硬项 · sweep 透镜义务】新字面 / 新判定 pattern 全仓预跑（起草期 2026-10-27，命令 + 结果）**：

- **新字面碰撞基线**（`grep -rlF`，语料 = 全仓 `.md` / `.mjs` / `.json`，排除本批目录）：机器标识符 **7 项全 0**——`assemble-edits` / `isInsideRepo` / `realpathDeepest` / `countOccurrences` / `eolHint` / `writePlan` / `assemble(`；`runSelfCheck` 撞 10 件既有件的本地函数名（8 脚本 + 2 hooks）→ 本批改名 `runAssembleSelfCheck`（改名后 0 命中）。话术四项按豁免面申报、不作「= 0」断言：`复跑分档`（4 文件：P6 retro / task_plan / ARQ-261027 / 本批 PROPOSAL）、`脚本批拆分`（3 文件：task_plan / ARQ-261027 / 本批 PROPOSAL）、`锚文装配器`（3 文件：同前）、`净增`（通用中文词，多件）。四项均为立项材料 / 处方源文 / 通用词，本无碰撞语义。
- **新判定 pattern 全仓预跑**：装配器 `--self-check`（16 断言）+ 变异（9/9）实跑（B2）；真仓装配实跑（B3，4 件 sha256）；输出根仓内拦截实跑（B5，exit 2）。**不接闸链**：全仓无脚本 import 本工具、无 hook 引用（除本批目录 `assemble-edits` 0 命中）。
- **`debt-render` 消费面**：ARQ 结项后 `--check` 应报待归位（起草期基线 = `--check` exit 0，B8）；渲染后自动归位，主 INDEX 与归档 INDEX 均为生成件、本批零手改。

**【硬项 · 复扫模式完备性先验】**：本批无目录搬家、无路径改落（`path-form-family.md` 形态族谱无消费面）。**退役语面一处**——自检句表头计数「实为四句」：模式集显式枚举全形态（有无反引号 × 整句 / 片段）：`实为四句` / `` `实为四句` ``。实跑定位（`grep -rlF`）：现行承载 = `docs/templates/PROPOSAL.template.md:69` **一处**（本批改为「实为五句」）；其余命中 = `261019-gov-review-local-memory-extraction` 批三件（PROPOSAL / landing-summary / landing-verdict，历史证据 append-only、零改）。**「替换 / 插入型」文本共九处**：模板四处（`:87` 整段追加式插入、`:89` 前插段、`:69` 表头片段、`:69` 句末 ⑤）、gov-change 一处（`:14` 后插 bullet）、ARQ 四处（frontmatter 三行 + 文末 append）；另新增文件一件（脚本全文）——全部以 §5 V3 的字面 grep 正反配对 + V2 的 `--cite-check` 逐字对账兜底。

## 4. human 取舍点（OQ）

**本批无待 human 拍板项**：方向由 human 2026-10-27 专项讨论裁定收敛（否决 ①③、取 ②⑤ + 新规则，原话见 ARQ `:26`），立项 ack「立项吧」已给；以下为实现层技术取舍，判官可 challenge，任一被推翻都有明确退路。

- **取舍 A · 落点 `.claude/scripts/assemble-edits.mjs`（非 `lib/`）**：`lib/` 的现行准入是「被多脚本 import 的共享解析面」（5 件全是门共享）；本工具是独立 CLI、零 import 关系、单文件可拷可跑。退路：判官若要求收进 `lib/`，须先定义该目录准入并接受所有消费方按模块解析。
- **取舍 B · 清单形态 = JSON `{edits, copy}`，插入行用「锚行 + 新文本」表达**：不设 insert-after / 行区间语法——每多一种语法就多一条要评审、要自检的判定路径，而「锚行 + 新文本」已能表达插入。退路：若出现无法用「oldStr → newStr」表达的编辑（整段重排等），那类编辑本就宜人工做而非装配。
- **取舍 C · 输出根强制仓外、不做开关**：见 T1「为什么不把『输出根仓外』做成可配」——可配即写权通道复辟。退路：若将来出现合法仓内副本场景（如需要 git 参与的装配），另批重论，不在本工具开洞。
- **取舍 D · 任一断言失败即零写盘（而非顺序写、报错误行）**：半装配副本会被误当完整改法集（判官 / 核验官无法自证副本完整）。退路：若要求「尽量写出已完成部分」，每条 sha256 都要额外带「本副本不完整」标记，收益不抵。
- **取舍 E · 不归一换行、CRLF 只提示不代偿**：归一化会静默改写字节，与逐字节承诺根本冲突。退路：若要求自动适配 CRLF，等价于接受「工具改写清单行尾」——须同时在 §2 声明这条改写语义。
- **取舍 F · `copy{from,to}` 是清单的一部分而非 CLI 开关**：伴随文件是「这一批要装配什么」的一部分，与 edits 同源留痕、同源 sha256。退路：切成 CLI 参数会丢失「清单 = 批的完整定义」这条性质。
- **取舍 G · 可发现性载体 = 模板 §5 句，不写 `architect.md`**：`.claude/agents/architect.md` 是自改递归面（改自身定义须显式 human 批），本批立项 ack 未覆盖；模板骨架是每个起草者必读的载体，覆盖同一目标且不进自改面。退路：判官若认为模板句不够（起草者可能先写正文后读 §5），由 human 批准下一批在 `architect.md` 加一句。
- **取舍 H · 本批净增 201 行 > ~150 初值而并批**：本批是规则立法批、不溯及；四 T 同属 ARQ-261027 单一消费簇、human 一次性 ack；拆批 = 两套 apparatus，恰是规则要治的成本。退路：判官若判「应拆」，可拆为「T1 工具批 + T2/T3/T4 文面批」，代价 = 双份 review / landing / retro 并由 human 重排。
- **取舍 I · 自检句表头「四句 → 五句」而非另起句块**：⑤ 与 ①–④ 同属起草期自检义务，同块承载即同一条 checklist 的第五项；表头计数按既有先例（T9 扩第三句 / 261019 扩第四句）随句改数。退路：判官若认为该从句块拆出，改回独立段落即可，判据不变。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律（T 自守）**：脚本面与文档面预期值 = 「按本稿 §2 逐字节装进 pristine 副本」的实跑输出——装配由本批 T1 的装配器完成（仓外副本 `C:/tmp/dcr-assembly`，清单 `_scratch/edits.json`）：清单的 `oldStr` 即 §2 的原文段、`newStr` 即 §2 的改后段，逐条恰 1 次命中；新增文件全文以 `copy{from,to}` 从原型装入。落地期实跑若与本表有字节差异，以落地实跑为准覆写 §5 并在 §7 留痕。
> **时点定义**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。
> **复跑分档（本批 T2 新句的首次自消费）**：本稿若进改稿轮，只复跑被改 T 的验证项并在「修订记录」列明复跑范围；全量复跑落在两个时点——FROZEN 标注前、落地后核验前。
> **夹具配方**（任何人可重建）：① 自检夹具 = 内嵌在脚本里的 16 断言（永久载体，B1）；② 变异夹具 = `_scratch/mutants.mjs`（起草期删除，结论固化进 B2）；③ 装配清单 = `_scratch/edits.json` + 原型 `_scratch/assemble-edits.mjs`（B3 可原样重放）。

**基线（起草期 2026-10-27 实跑）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node delivery/review/261027-gov-review-draft-cost-relief/_scratch/assemble-edits.mjs --self-check` | `{"selfCheck":"assemble-edits","pass":16,"fail":0,"total":16,"ok":true}`、exit 0；原型 `wc -l` = **201** |
| B2 | `node delivery/review/261027-gov-review-draft-cost-relief/_scratch/mutants.mjs` | 基线 16/16 绿；**9/9 判别成立**——m1 命中计数退化为存在性 → 「多命中·中止」红（连带 CRLF 测量断言红，因自检复用同一计数函数）；m2 字符串式 replace → 「$& 字面落盘」红（got `头\n锚行锚行$1\n尾\n`）；m3 读入即归一 CRLF → `[0,0] ≠ [4,4]` + 诊断句红；m4 每条在原始内容上查找 → 「链式」红；m5 失败照写已装配部分 → `[true,2] ≠ [false,0]` 红；m6 仓界判定失效 → 「输出根仓内·拦截」红（连带 file 路径守卫组红，同一判据两用）；m7 `copy` 忽略 `to` → 「copy{from,to}·同字节」红；m8 仓界判定去掉 realpath（junction 可绕）→ 「junction 指仓内·须拦」红 |
| B3 | `node _scratch/assemble-edits.mjs _scratch/edits.json C:/tmp/dcr-assembly`（真仓） | `ok:true`、4 件：模板 `84bfff15…→658bb4d4…`（103 行）· gov-change `044f0957…→5ef0c2e3…`（41 行）· ARQ `5db925d7…→51698ef8…`（31 行）· 脚本 copy `a5c924b0…→a5c924b0…`（`afterLines` 202） |
| B4 | `node --check C:/tmp/dcr-assembly/.claude/scripts/assemble-edits.mjs`；`（cwd=C:/tmp/dcr-assembly）node .claude/scripts/assemble-edits.mjs --self-check` | 语法过；`pass 16 / fail 0`、exit 0——装配副本可跑（`copy` 让单件脚本在副本内自足） |
| B5 | 同 B3 但输出根 = `C:/AI/KidPal-gov/_scratch-out`（仓内） | exit 2：`{"ok":false,…,"message":"输出根在仓内——沙盘副本必须落仓外（architect 不落 canonical 的机械兜底）"}`；零写盘 |
| B6 | 负向样例（`_scratch/neg.json` 0 命中 / `_scratch/neg2.json` 多命中） | 0 命中：`count:0`、exit 1；多命中（`architect-reviewer` 在 gov-change 出现 5 次）：`count:5`、`oldStr 命中 5 次（须恰 1 次：加长锚串）`、exit 1；两次均**未创建**输出目录 |
| B7 | 装配副本字面正推（`grep` 计数，REPO → OUT） | V3 基线行逐项（见 V3）；三规则表头 `**【硬项 · 验证集撰写三规则】**（261026 批合写` = 1 → 1（未误改）；F1 口径两项（模板与 gov-change 各含字面「未跟踪文件先 `git add -N` 再计」）0 → 1 |
| B8 | `node .claude/scripts/debt-render.mjs --check` | exit 0（起草期基线干净；结项后归位由 V7 消费） |
| B10 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-draft-cost-relief/PROPOSAL.md` | `ok:true`、`section2BodyFound:true`、`anchorCount:4`、anchorUnion = manifest 四行逐 token 一致、`findings:[]`、`advisories:[]`、exit 0 |
| B11 | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-draft-cost-relief/PROPOSAL.md`（起草期自动 draft） | `ok:true`、`phase:draft`、**checked 9 / missing 0 / notChecked 7 / unverified 0**（9 = 原文段：模板 `:87` / `:89` / `:69` 两片段 + gov-change `:14` + ARQ 尾行 + frontmatter 三行） |
| B12 | **post-landing 沙盘**：装配副本树（B3 四件）+ 本稿副本，`--phase landing` | `ok:true`、`phase:landing`、`targetCount:4`、**checked 8 / missing 0 / notChecked 9（全部为原文块）/ unverified 0**（8 = 新增文件全文 1 + 改后围栏 5 + ARQ append 1 + frontmatter 行内 1）——V2 的终态预演 |
| B13 | 行尾 / 结构自检 | 三件跟踪目标文件 `git ls-files --eol` 实测均 `i/lf w/lf`；新脚本为未跟踪件、不进该命令（落地后按同口径 + V1 sha256 复核，R1-F3）；PROPOSAL 626 行、U+FFFD 0、`### T<n>` 标题 4、目标文件锚行 4、注入标记残留 0、围栏定界行 34（偶数）、manifest 块 4 行 |

**B9 · §2 注入对账（`_scratch/inject.mjs` 逐段 sha256，2026-10-27）**——§2 的原文 / 改后 / 新增文件全文全部机械注入（原文 = 磁盘切片；改后 = 装配副本切片或清单 `newStr`；新增文件全文 = 原型文件去尾 LF），本稿无手抄长输出：

| 段（marker） | bytes | sha256 |
|---|---|---|
| SCRATCH:assemble-edits.mjs | 13266 | `b00895bed53b7c51f5cc4bbf4e2e7f373c986932654e96d1e5840f7dfdd704b3` |
| SLICE:REPO:docs/templates/PROPOSAL.template.md:87:87 | 196 | `1553aa795326ea482897c4cebeadc8ea5e0fdc8c436e6d1a05b427be6b96cff5` |
| SLICE:OUT:docs/templates/PROPOSAL.template.md:87:87 | 644 | `774a5ea0080a9f80fc0c5bc2404094759a72aa297cfd7436de2577ac51c4b77c` |
| SLICE:REPO:docs/templates/PROPOSAL.template.md:89:89 | 112 | `fcbe872f736c9b59e788f32d3adde803f62392acbcb65ee0de14f0bbda75ec04` |
| REGION:OUT|docs/templates/PROPOSAL.template.md|**复跑分档** | 449 | `5729228f7fb9f6596fdf77bb761ab115fd8372f8825c35e12f8e9a71f0bb4be3` |
| SLICE:REPO:.claude/references/gov-change.md:14:14 | 77 | `0376fddd1ab9dc7442e4b714160b3341d90ac31fb9e6c4c78a8b13d0dab3bd15` |
| SLICE:OUT:.claude/references/gov-change.md:14:15 | 547 | `5403389e782027ac43ed822d32880341a963be851c9ad53f885f7b7839401aac` |
| NEWSTR:T3·自检句表头四句→五句 | 153 | `58def9c5fb0551c157fd672ea02062e9f22657371d3c0492cf94bda440827c73` |
| NEWSTR:T3·自检句⑤脚本批拆分自查 | 622 | `bffdbe4e1b6383420b6807ed0faf39a708180b10d2a4146190883e9a16cdb64b` |
| SLICE:REPO:docs/debt/gov/ARQ-261027-draft-sandbox-asse | 1407 | `e04153693fc2c28e5381a6cbf8ed134c1993690aaddc5b7fec1d9b5d89352397` |
| NEWTAIL:T4·文末结项 append | 1041 | `58e82aedb7a17ddb8a0b2bedaca4ade7863713da29d8914f000f67e7c0321707` |

**片段在场核（复证 1 的引文，`grep -cF` 各 = 1）**：ARQ 条目「human 裁定：不走 dev-loop」= 1、「它如果真的把哪个闸改坏了，确实蛮危险的」= 1；task_plan「**立项 ack（2026-10-27 human）**：「立项吧」」= 1。

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填 / 正推） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | `node --check .claude/scripts/assemble-edits.mjs`；`node .claude/scripts/assemble-edits.mjs --self-check` | 语法过；与 B1 **逐值相同**（`pass 16 / fail 0 / total 16`、exit 0）；落地文件 sha256 = `a5c924b0c8b7905b818d2defca6488dbf1c3c17f862e570f0c9f5d086bfa4be5`（= §2 全文围栏内容 sha256 `b00895be…` + 末尾 LF 一枚，B9 表） | 真空绿：输出含自检组名 `assemble-edits` + 三计数；判别力由 B2 八条变异背书 |
| V2 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-draft-cost-relief/PROPOSAL.md`（届时本稿 FROZEN，自动 phase=landing） | `ok:true`、`phase:landing`、`missing:[]`；`checked` 非空（改后段 + 插入段 + 新增文件全文）；原文块全进 `notChecked`。起草期沙盘预演 = B12（checked 8 / missing 0 / notChecked 9） | 负向：`missing` 为空 × 正向：`checked` 非空且逐条 `matched != null`；任一字符漂移即 missing 非空 |
| V3 | pre-commit | 字面正推（`grep -cF` 逐项，REPO → OUT，文件 = manifest 四件） | 模板：`实为四句` **1 → 0**、`实为五句` **0 → 1**、`**复跑分档**：改稿轮` **0 → 1**、`**装配**：§5 实跑副本一律用共享锚文装配器` **0 → 1**、`⑤ **脚本批拆分自查**` **0 → 1**、`**【硬项 · 验证集撰写三规则】**（261026 批合写` **1 → 1**（防过度替换）；gov-change：`脚本批拆分纪律（261027 批 T3 立` **0 → 1**；ARQ：`status: deferred` **1 → 0**、`status: closed` **0 → 1**、`closed: 2026-10-27` **0 → 1**、`closedBy: 261027-gov-review-draft-cost-relief` **0 → 1**；F1 口径（R1 修订）新增两项：模板与 gov-change 各含字面「未跟踪文件先 `git add -N` 再计」 **0 → 1** | 每条负向（旧句 = 0）配正向（新句 = 1）；frontmatter 正反配对防「只 append 不翻转」；三规则表头不变项作「未误改」哨 |
| V4 | pre-commit | `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md server/ firmware/ tools/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json .claude/scripts/pre-merge-escalation-gate.mjs .claude/scripts/landing-fidelity-check.mjs .claude/scripts/lib/ AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md` | 空输出 | 负向（禁触面零 diff）；正向配对 = `git diff --name-only`（`debt-render` 之前）恰等于 manifest 四件。**并发改动口径**：若落地时同工作树另有主线程账本登记或生成件在途（起草期已见 `task_plan.md` / `docs/debt/gov/INDEX.md` / `delivery/review/INDEX.md` 在途），逐件核性质后如实记入 landing-summary 的 `changedCanonicalNotInProposal` 说明——不是静默豁免、不并入本批 manifest |
| V5 | pre-commit | `ls delivery/review/261027-gov-review-draft-cost-relief/_scratch/` | 目录不存在（原型 / 注入器 / 变异器 / 清单已删） | 负向（无残留）+ 正向配对 V4 的「diff 恰等于 manifest」会机械拦住漏删 |
| V6 | pre-commit | ① `node .claude/scripts/assemble-edits.mjs <任意小清单.json> C:/AI/KidPal-gov/out`（仓内输出根）；② 链接面：`mklink /J C:/tmp/dcr-jrepo <仓根>/docs` 后以 `C:/tmp/dcr-jrepo/out` 为输出根 | ① ② 均 exit 2 + 拦截消息逐字（B5 / R1-F2 复现场景） | 落地后复跑仓界红线仍成立（含链接面）——工具不会因落地而变成仓内写通道 |
| V7 | post-commit | `node .claude/scripts/debt-render.mjs` → `--check` → `--self-check`；再跑 `--cite-check <本稿> --phase landing` | ARQ 落 `docs/debt/gov/_archive/2026/`、主 INDEX 转已结指针、归档 INDEX 出薄行；`--check` exit 0；`--self-check ok`；cite-check `missing:[]`（归档位兜底在终态仍命中） | 归位是 status 的纯函数；V2 的命中在归位后复跑——防「落地前对、归位后断」 |
| V8 | pre-commit（主线程 chore） | `node .claude/scripts/changes-index.mjs --upsert 261027-gov-review-draft-cost-relief` | `delivery/review/INDEX.md` 出现该 id 行、state 缓存同步；须在落地 commit 之前跑 | 生成件，禁手改 |
| V9 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261027-gov-review-draft-cost-relief --source-ref HEAD` | `PASS (…, kind=gov)`、exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff |

## 6. 留根清单

**本批结项（landing 后由主线程按 T4 落账；architect 不做）**：`ARQ-261027-draft-sandbox-assembly-scaffolding`（整条结项）。

**留根与复议触发**：

1. **装配器覆盖面 = 「oldStr → newStr」替换**：结构重排（整段移动、路径改名）不在覆盖面——那类编辑宜人工做或拆成可表达的多条。复议触发 = 出现第三条「必须手工、清单表达不了」的装配动作。
2. **输出根强制仓外无开关**：复议触发 = 出现合法需要仓内副本的装配场景（如需 git 状态参与）；届时另批重论，不在本工具开洞（§4 取舍 C）。链接面已由 realpath 判定闭合（R1-F2：`mklink /J` 到仓内后作输出根实测 exit 2）；残余绕过面只剩「蓄意同时构造路径与 FS 链接」——蓄意行为体本可直接违纪，防线主目标是防意外 canonical 写。
3. **拆分纪律不溯及本批**：规则自本批落地起对后续批生效；本批净增 201 行 + 新增工具形态如实申报（§4 取舍 H）。阈值初值 150 / 50 行随复发校准——复议触发 = 连续两批出现「按阈值该拆而未拆」或「拆了反而更贵」的实例。
4. **装配器无架构层指路**：`architect.md` 未加句（自改递归面，需单独 human 批）；当前指路 = 模板 §5 句 + 本批 PROPOSAL。复议触发 = 出现第三例「不知道有该工具、又现写脚手架」。
5. **复跑分档的判官可判性依赖「修订记录列明复跑范围」**：若出现「窄化但未列明」的模糊实例，复议是否把「列明」升为硬项。
6. **生成件不进 manifest**：`docs/debt/gov/INDEX.md` / `docs/debt/gov/_archive/2026/INDEX.md` / `delivery/review/INDEX.md` 均再生（V7 / V8），landing-verifier 按最终路径核内容。
7. **`copy{from,to}` 只做字节复制**：新文件落地的权威仍是 §2 全文围栏（V2 逐字节对账兜底）；copy 只负责让副本可跑、可复算。

**环内自律**：起草期脚手架全在 `delivery/review/261027-gov-review-draft-cost-relief/_scratch/`（原型 / 注入器 / 变异器 / 清单 / 负向样例），落地 commit 前删除、不随批 commit（V5 正向断言机械兜底）；仓外装配副本在 `C:/tmp/dcr-assembly`，负向样例输出根 `C:/tmp/dcr-neg`（未创建），不进仓。

## 起草期自检留痕（模板五句自检的落地记录，2026-10-27）

1. **preflight 实跑**：`ok:true`、`anchorCount:4`、anchorUnion = manifest 四行、`findings:[]`、`advisories:[]`、exit 0（B10）。
2. **路径形态族谱**（`path-form-family.md`）：本批无搬家 / 改名 / 存量路径改落；新增文件一件（`.claude/scripts/assemble-edits.mjs`），新符号已按 §3 census 预跑（6 项 0；`runSelfCheck` 撞名已改名后 0）。
3. **承重引文 byte 对账**：§2 原文段与改后段全部由 `_scratch/inject.mjs` 机械注入（原文 = 磁盘切片；改后 = 装配副本切片或清单 `newStr`；新增文件全文 = 原型文件去尾 LF），零手抄长输出；11 段内容注入逐段 sha256 见 B9（另 REPORT 汇总块 1）；`--cite-check --phase draft` = checked 9 / missing 0（B11），post-landing 沙盘 = checked 8 / missing 0（B12）。
4. **长文落地自检**：PROPOSAL 626 行、U+FFFD 0、`### T<n>` = 4、目标文件锚行 = 4、注入标记残留 0、围栏定界行 34（偶数）、manifest 块 4 行（B13）；写入按三段追加（头部 / §0 / §2.0–T4、§3–§4、§5–§7），每段写后即机检（生成即验证）。
5. **本批自消费**：T1 原型先写后测（B1 / B2），随后用它装配本批自己的仓外副本（B3 / B4）——工具以「装配自己」完成首次实战；§5 顶部断言纪律行即其消费方式（清单 `oldStr` = §2 原文段）。

## 修订记录（作者侧 · v0.1 → v0.2，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 记录逐字未动（本节物理位置在 §7 之前）。

**依据**：R1 verdict BLOCK(F1,F2) + F3 NIT（§7）。方向零打回、无升级 human、取舍 H 成立。

| # | 来源 | 修订 |
|---|---|---|
| 1 | R1-F1 | T3 两处净增口径补「新增 / 未跟踪文件先 `git add -N` 再计，或按行数直计」（gov-change bullet 与模板自检句 ⑤ 同步）；§2 T3「机械可判性」段同步点明未跟踪件不进 numstat；V3 / B7 增该字面的 0 → 1 断言。 |
| 2 | R1-F2 | 取判官修法 ①（治本）：`isInsideRepo` 改为先对「最深存在祖先」`realpathSync`、再拼回不存在尾段后判仓界；自检新增「真实路径·不存在尾段拼回」「junction 指仓内·须拦」两断言（14 → 16），变异新增 m8（去掉 realpath → junction 断言红，8 → 9 项，9/9 判别成立）；实跑复现 R1 场景：`mklink /J C:/tmp/dcr-jrepo <仓根>/docs` 后以其下路径为输出根 → exit 2、仓内零写盘。 |
| 3 | R1-F3 | 复证 4 / B13 行尾口径改准：三件跟踪目标文件 `git ls-files --eol` 实测 `i/lf w/lf`；新脚本为未跟踪件、不进该命令，落地后按同口径 + V1 sha256 复核。 |
| 4 | 连带 | 因 T1 脚本与 T3 文案变化，同步重装配仓外副本（`C:/tmp/dcr-assembly`）、重出 B1 / B2 / B3 / B4 / B7 / B13 与 B9 注入表，原型 179 → **201** 行（净增申报随更）；ARQ 结项 append 内的自检数随之更新（14/14 → 16/16），B3 的 ARQ afterSha 重出。 |

**复跑范围（按本批 T2 新规窄化）**：本改稿轮只复跑受影响项——T1 全部（自检 16 断言 / 变异 9/9 / 真仓装配 / 装配副本自检 / 仓界与 junction 实跑）、T3 两处字面（V3 新增两项 + 装配复跑）、F3 触及的行尾口径；T2 / T4 内容未变（台账号随装配重出）。全量复跑在 FROZEN 标注前执行（按 T2 句的两时点义务）。

**未变**：manifest 4 行整、packet-binding、riskTier medium、review profile G2+G3、判档 Track B 主路径、§2 四 T 结构；§7 R1 记录逐字未动。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-27 - architect-reviewer

**轮次**：R1 全面审（Track B 主路径）。判官 = 独立 architect-reviewer 实例；以下事实断言全部亲核（Read / Grep / Bash 实测），未采信 PROPOSAL 转述。

**四查**：

1. **意图保真 — PASS**。T1–T4 与 ARQ-261027「方向裁定追注」逐项对应（条目全文亲读）：②装配工具化→T1、⑤复跑分档→T2、新规则拆分纪律→T3、「本条目待三修法落批后结项」→T4；①写权通道与③承诺粒度降级否决被如实承接——§0 非目标明写、T1 无起草期写槽且输出根强制仓外、§2 逐字节承诺形态未降级。立项 ack 原话「立项吧」（task_plan:99 亲读）与 changeId / Track B 定位一致；「T1 落地后本批起草即自消费试用」在本稿 §5 / 起草留痕 5 中兑现（装配器装配本批自己的仓外副本，实测复现见下）。
2. **纪律合规 — 基本合规，两处技术缺口见 F1 / F2**。写权边界：工具对源文件只 `readFileSync`、写盘唯一点 `writePlan` 限 outRoot 下仓根相对路径（亲读 §2 全文代码）；但仓界判定存在 junction 绕过面（F2 实测）。路由表 / 门语义零触碰：装配器不接闸链、不被任何脚本 import（全仓 grep 亲测 0 命中）；§3 未触清单与 V4 禁触面一致。判档 Track B 恒定成立（触 `.claude/scripts/**`，gov-change.md:18 判据③亲读）。
3. **撞车复核 — PASS**。与 P6 刚落的 `--cite-check` 互补不重叠（装配器产仓外副本，cite-check 对账 PROPOSAL 引文与落地文件，消费面不相交）；`lib/` 准入「被多脚本 import 的共享解析面」亲核成立（`lib/` 五件实测均为解析面、本工具零 import 关系，取舍 A 不触发退路）；符号 census 亲测吻合——`assemble-edits` / `isInsideRepo` / `countOccurrences` / `eolHint` / `writePlan` / `runAssembleSelfCheck` 全仓 0 命中，`runSelfCheck` 撞 10 件（8 scripts + 2 hooks 逐件亲点）与自报一致、改名处置正确。
4. **牵连面完整 — PASS**。census 抽查：`复跑分档` 实测 4 文件（P6 retro / task_plan / ARQ-261027 / 本批）与申报吻合；消费方普查在 §3 列明（模板消费 = 后续起草者与判官、gov-change bullet 消费 = 治理面 loop 参与者、装配器消费 = 起草 / 评审 / 核验三方）；判别夹具 8/8 亲测复现（`mutants.mjs` 实跑：基线 14/14 绿 + m1–m7 各自红，逐条红点与 B2 自报吻合）；取舍 G 成立——`.claude/agents/architect.md` 是自改递归面（prompt-authoring.md 硬排除 + DEV-GOVERNANCE §6 约束），本批立项 ack 未覆盖，模板句承载可发现性 + §6 留根 4 复议触发，理由链完整。

**自消费证据亲核（全数复现，逐项实测）**：

- §2 围栏脚本（:110-288）与 `_scratch/assemble-edits.mjs` 逐字节 diff 为空；原型 sha256 = `a8491161…3613a`（= V1 落地断言值）；去尾 LF sha256 = `9c8f36b4…a6c83`（= B9 表值）。
- `--self-check` 14/14 exit 0；`mutants.mjs` 8/8 判别逐条红（含 m2 got `头\n锚行锚行$1\n尾\n`、m3 `[0,0] ≠ [4,4]`）。
- 真仓装配复跑到全新输出根 `C:/tmp/dcr-r1-verify`：ok:true、4 件、逐件 before/after sha256 与 B3 表逐值相同（模板 `84bfff15→2a69a992` / gov-change `044f0957→73e70387` / ARQ `5db925d7→b7f9b460` / 脚本 copy `a8491161` 恒等）；现盘三件 sha256 与 before 值逐一吻合（锚未漂移）。
- V3 字面正推逐项亲测全中（`实为四句` 1→0、`实为五句` 0→1、`**复跑分档**：改稿轮` 0→1、装配句 0→1、`⑤ **脚本批拆分自查**` 0→1、三规则表头 1→1、gov-change bullet 0→1、ARQ frontmatter 四项正反配对全中）。
- 负向：0 命中 exit 1 / 多命中（count:5）exit 1、两次输出目录均未创建；仓内输出根 exit 2 + 拦截消息逐字。
- 装配副本四处改动点逐处人读（gov-change 新 bullet 上下文、模板 §5 三段、`:69` ⑤ 接排、ARQ 文末 append）：无格式损伤、三处锚文互不覆盖、T1/T2/T3 与既有文本衔接成立。
- `--preflight` ok / anchorCount 4 / anchorUnion = manifest 四行 / advisories 空；`--cite-check` draft checked 9 / missing 0 / notChecked 7 / unverified 0；`debt-render --check` exit 0。
- B9 抽验三段 slice sha256（模板 `:87` = `1553aa79…cff5`、`:89` = `fcbe872f…ec04`、gov-change `:14` = `0376fddd…bd15`）与磁盘切片逐值相同。
- 行数口径亲核：模板 100 行 / gov-change 39 行 / ARQ 28 行（wc -l 实测），OUT `afterLines` 103 / 41 / 31 = 实行数 + T2 两行 / T3 一行 / T4 两行 + 尾 LF split 计数差——PROPOSAL 自注口径成立；原型 179 行、PROPOSAL 548 行实测吻合。

**Findings**：

- **F1 · SHOULD · T3 净增公式漏计新文件**：T3① bullet 与 T3② ⑤ 句把「净增 = `git diff --numstat -- .claude/scripts/` 增行合计减删行合计」写成唯一机械口径。实测：`git diff --numstat` 不含未跟踪新文件（本批 `delivery/review/261027-gov-review-draft-cost-relief/` 整目录未跟踪、内含 8 件，`git diff --numstat -- <该目录>` 实测 0 行输出）；且 clean-staged 新文件同样不进 `git diff`（worktree↔index 口径的 git 文档语义）——新增 `.claude/scripts/**` 文件的批在起草期（⑤ 自查时点）与落地期（staged 后）按该公式都算出净增 0，恰在「新增大文件该触发拆分」的最主要场景失灵；括注「新文件 = 增行合计」只在 intent-to-add（`git add -N`）后成立而条文未写。修法方向：两处口径各补一句「新文件先 `git add -N` 再 numstat，或未跟踪新文件按行数直计」，方向不动。
- **F2 · SHOULD · 仓界判定不解析链接，junction 可绕过「输出根强制仓外」**：`isInsideRepo` 基于 `resolve`+`relative`，不解析符号链接 / junction。无害实测：① 建 junction `C:\tmp\dcr-jrepo → C:\AI\KidPal-gov\docs`，以工具自身谓词判定 `isInsideRepo = false`（被判「仓外」）；② 反向 junction（指向仓外目标）实测被工具接受为输出根（进入锚断言阶段 exit 1，即仓界判定放行）。合成结论：`node assemble-edits.mjs <清单> C:\tmp\<指向仓内的 junction>` 会通过红线、写入实际落仓内——§2 设计点 7「工具永远做不成仓内写通道」的性质断言在链接面不成立，而这条恰是 human 否决写权通道 ① 后的本批实质防线（riskTier medium 证成的承重点）。评 SHOULD 不评 BLOCKING 的理由：绕过须蓄意构造 junction，蓄意行为体本可直接违反纪律，防线主目标（防意外 canonical 写）仍达成。修法方向（R2 须处置其一）：outRoot 判定前对最深存在祖先 `realpathSync` 后再判仓界 + 自检补仓内 junction 拦截断言；或最低限度在 §6 留根显式登记该绕过面与复议触发。测试 junction 已清理（rmdir，未写穿）。
- **F3 · NIT（不拦绿）· 行尾断言「四件…均」超出实测口径**：复证 4 / B13 写「四件目标文件 `git ls-files --eol` 均 `i/lf w/lf`」，实测该命令只回报三件跟踪件（未跟踪新脚本不列，括注已自认）——「四件…均」字面把未实测件计入断言。建议改「三件跟踪件实测 + 新脚本落地后按同口径复核（V1 sha256 对账已兜）」，可与 F1 同轮顺手改。

**取舍 H 判定（判官 challenge，结论：成立，不升 human）**：本批净增 179 行 > ~150 初值且为新增工具形态，按将立规则本应拆。不接受拆批的三条理由亲核成立：① 立项 ack 原话明示「T1 落地后本批起草即自消费试用」（task_plan:99）——human 已按「T1 在本批」的形态拍板，拆批反而违背 ack；② 不溯及已写进 §0 非目标，立法批自洽；③ 四 T 同属 ARQ-261027 单一消费簇，拆开 = 双套 apparatus，恰是 T3 要治的成本。阈值带 `~` 且明写初值随复发校准。此形态在本批后再现即按规则拆批、无豁免惯例化空间（§6 留根 3 已载）。

**判档**：Track B 主路径，维持。触 `.claude/scripts/**` → gov-change.md:18 判据③恒 Track B；Track A 段判据均不命中——方向由 human 专项讨论裁定收敛（ARQ「方向裁定追注」亲读，否决 ①③、取 ②⑤ + 新规则）；riskTier medium 如实（新写路径工具，§2 失败模式四条与缓释一一对应）；review profile G2+G3 如实（G3 = 新增脚本、G2 = 模板 + gov-change 流程 SSOT）；G1 / 契约面 / spec 两文件 / hooks / scope-map / settings / verify-gates 零触碰由 §3 未触清单 + V4 禁触面双重覆盖。

**结构字段**：manifest 围栏块在场且恰 4 行（PROPOSAL.md:12-17 亲读）、packet-id 行在场（:6）、manifest 不含 `_scratch/`（与 V5 / §6 环内自律自洽）。

**升级 human 项**：无。F1 / F2 均为实现层缺口、修法方向不动摇三修法 + 结项的批方向，R2 聚焦修订到位性 + 新矛盾扫描。

verdict: BLOCK(F1,F2)

### R2 - 2026-10-27 - architect-reviewer

**轮次**：R2 聚焦审（依 R1 findings 修订到位性 + 新矛盾扫描，不做全量重核）。判官 = 独立 architect-reviewer 实例；以下断言逐项亲测。

**F1 修订到位 — PASS**。T3① gov-change bullet（v0.2 :378）与 T3② 模板自检句 ⑤（:400）均补「新增 / 未跟踪文件先 `git add -N` 再计，或按行数直计」；§2 T3 机械可判性段（:403）同步点明「未跟踪 / 新增文件不进 numstat——先 `git add -N` 再计，或按行数直计，R1-F1」；V3 增两项 0→1 字面断言、B7 同步。装配副本实测：gov-change 与模板两份「未跟踪文件先 `git add -N` 再计」各 REPO:0 → OUT:1，正推成立。
**F2 修订到位 — PASS（治本案 ①）**。`isInsideRepo` 改为先对最深存在祖先 `realpathSync`、拼回不存在尾段后判仓界（`realpathDeepest`，§2 :152-162 亲读——存在段 realpath 解析链接、不存在尾段拼接，dangling 链接路径走上级分支不误判）；自检 14→16（「真实路径·不存在尾段拼回最深存在祖先」+「junction 指仓内·须拦（不可建时跳过记绿）」，跳过分支真空绿风险如实申报）；变异 8→9（m8 去 realpath → junction 断言红，实跑 9/9 亲测复现）。**端到端亲测**：`mklink /J C:\tmp\dcr-jrepo-r2 → C:\AI\KidPal-gov\docs` 后以 `C:\tmp\dcr-jrepo-r2\out` 为输出根实跑 → **exit 2 + 拦截消息逐字、仓内零写盘**（docs/out 不存在亲测）——R1 的绕过面闭合。设计点 7「工具永不成仓内写通道」性质断言恢复成立；§6 留根 2 残余面登记如实（蓄意构造路径+链接 = 直接违纪面，防线主目标不变）。
**F3 修订到位 — PASS**。复证 4（:80）与 B13（:510）改「三件跟踪目标文件实测 `i/lf w/lf` + 新脚本未跟踪件不进该命令、落地后按同口径 + V1 sha256 复核」，与实测口径一致。

**连带一致性（抽查亲测）**：原型 179→201 行（wc -l 实测 201；fence :110-310 与原型逐字节 diff 为空）；取舍 H（:485）/ 留根 3（:552）/ T3 自指（:404）/ B1（:499）四处 201 同步；自检 16/16 实跑复现（B1 逐值相同）；mutants 9/9（m8 红点 = 「junction 指仓内·须拦」got false，与 B2 自报吻合）；B3 重出值逐项复现（模板 `84bfff15→658bb4d4` 103 / gov-change `044f0957→5ef0c2e3` 41 / ARQ `5db925d7→51698ef8` 31 / 脚本 copy `a5c924b0` 202——现盘三件 before 值与锚未漂移同前）；V1 落地 sha256 = `a5c924b0…4be5` 与原型全文 sha256 实测相同；B9 SCRATCH 去尾 LF = `b00895be…04b3` 实测相同；ARQ append 自检数 16/16（:444）、NEWTAIL sha 同步重出；census 7 项含 `realpathDeepest` 全 0（亲测）；PROPOSAL 626 行实测吻合（B13）。

**新矛盾扫描**：
- **结构不变量全保持**：manifest 4 行整（:12-17）、packet-id（:6）、riskTier medium、profile G2+G3、判档 Track B 主路径零变化；§7 R1 记录逐字未动（verdict 行全文件恰 1 处、F2 finding 标题与取舍 H 判定段逐字在原位亲点）。
- **修订记录段位置**：置于 §7 之前（:568-583），作者侧留痕、自报「不替代 §7 判官记录」——与 §7 append-only 纪律不冲突；其「复跑范围」段按 T2 新规窄化申报（T1 全部 + T3 两处字面 + 行尾口径；T2/T4 台账号随装配重出），与 T2 句判据（复跑项 ⊆ 被改 T 验证项）自洽。
- **preflight / cite-check v0.2 复跑**：ok / anchorCount 4 / advisories 空；cite-check draft checked 9 / missing 0（逐值同 B11）。
- **N1 · NIT（不拦绿）· V6② 链接面用例缺清理注记**：V6②（:539）教落地期建 `mklink /J C:/tmp/dcr-jrepo <仓根>/docs` 后复跑拦截，但未写「核毕 `rmdir` 清理」与「junction 路径已存在时先清」——残留 junction 虽无实害（指向 docs、不写穿），但会在 V6 复跑时因路径占用而误报失败。建议主线程落地执行 V6 时顺手补清理动作并在 landing-summary 留痕即可，不必改稿。

**升级 human 项**：无。

verdict: GREEN
