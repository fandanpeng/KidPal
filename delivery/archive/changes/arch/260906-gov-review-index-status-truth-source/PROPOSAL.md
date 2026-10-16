# PROPOSAL · 260906-gov-review-index-status-truth-source

> **状态：FROZEN v1.0 · 2026-09-06 · 凭据 = human 立项 ack（「记录ARQ，出proposal，快速修了」2026-09-06）+ OQ 三题全裁（OQ1=(a) / OQ2=(a) / OQ3=(a)，human 2026-09-06）+ R1 NEEDS-REVISION → 主线程修订（N-1 判官授权机械落锚行 / N-2 证据与计数修正 / N-3 主线程裁 (a) + NIT-1~4）→ R2 GREEN（§7）· R2 四条授权机械修订（R2-NIT-1~4）由主线程标 FROZEN 时顺手落字（判官授权，机械修订）**（前端对齐：为什么改 = changes/INDEX.md 的 dev/arch 状态投影被一次全量重建大面积翻错（ARQ-260906-index-rebuild-dev-status-drift），根因已深挖到「两个写者、两套真值源、缝无机械看护」；往哪改 = G3 三件（shared lib + changes-index.mjs + closeout-check.mjs），零契约零 spec 零 G2 零实现面；改动多大 = 单谓词单源化 + 两个推导函数的判据修正 + 自检夹具扩充，无新闸、无新事件类型、无 scope-map 变更）。
> **承载 ARQ**：ARQ-260906-index-rebuild-dev-status-drift（SHOULD，docs/ARCH-REVIEW-QUEUE.md:37，2026-09-06 登记）——本批即其指定归口「下次动 changes-index.mjs 的治理批」。
> **review 轨**：Track B 常驻轻路径。Track A 判据核查：① 不触 `docs/contracts/**`（manifest 三路径全 `.claude/scripts/` 面）；② 非跨系统结构性变更（单一 G3 簿记投影修复，无固件/shim 行为变更）——两判据均否，维持 Track B。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low——changes-index.mjs 自声明「非第二事实源、不做拦阻依据」，纯投影面；closeout-check.mjs 属 dev-loop closeout 承重闸，本批对其仅做**具名导入单源化（逐字可证的零行为变更）**，不动门逻辑、不动判定语义、不动写面。
> **review profile**：G3——`.claude/scripts/lib/governance-gate-shared.mjs`、`.claude/scripts/changes-index.mjs`、`.claude/scripts/closeout-check.mjs`。G1 / G2 / 契约面 / spec 实现面 = 零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行已按硬项 2a 在场。

```landing-manifest
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/closeout-check.mjs
```

> manifest 注：3 路径 = T1–T4 §2 目标文件锚并集——T1（lib 新增谓词）、T2/T4（changes-index 两处改法）、T3（closeout-check 单源化）；三文件全 G3 脚本/lib 面，CANONICAL_TEST 可机锚（不踩 ARQ-260906-lfc-canonical-anchor-gap 缺口）。

---

## 0. 想实现的效果与差距

**目标效果**：`changes-index.mjs` 全量重建（含 `--upsert` 后的簿记性重建）产出的 `changes/INDEX.md` dev 段状态列 == dev loop 收口事实（收口制品集在场的包 = closed），arch 段状态列 == review loop 终态事实（landing-verdict.md 结论 GREEN/PASS 在场的批 = landed）——两个写者（closeout-check 增量写行 / changes-index 全量重建）共用同一机械谓词，漂移缝闭合。

**现状差距**（2026-09-06 深挖实测，证据在 §1）：

- **dev 段**：重建判据（changes-index.mjs:138-139，R5 遗留）读 `verification.json`/`verdict.json` 在场——而这两件按 closeout-hardening 制品 SSOT 是 advisory、**永不晋升** `changes/dev/<id>/`，判据对全部 8 包恒 false → 一重建 dev 段全 `open`，把 closeout-check 亲写的 `closed` 行翻回（f0aaa87 实录）。
- **arch 段**：`deriveLanded` 的正则要行首即 verdict 词，新格式 `## 7. Verdict`（编号标题）+ 次段 `**GREEN**`（加粗散文）双不命中（实测 260905 一批 + 两批 260906 共三批均 false，260830/260904 两批另有散文式病灶）→ fail-closed 投 `frozen` 投不出 `landed`。

**非目标**：① 不改 supervisor-verdict.json 的产出与判定链（supervisor Stop 门 G2 零触碰）；② 不改 INDEX 的「投影非事实源」定位与列结构；③ 不收敛 CHANGE_ID_CUTOFF 的其余 3 处约定副本（taskpackage-required-check.mjs / devloop-artifact-check.mjs / hooks/supervisor-gate-check.mjs——钩子副本不动，余归机械替换小批，见 §6）；④ 不改 landing-verifier 的 verdict 行文模板（G2，另批评估）；⑤ 不给 INDEX 加任何「拦阻依据」地位。

## 1. 问题与根因（地面真值）

**问题类 A：dev 双写者真值源不一致（T1/T2/T3）**。

- 证据①（写者一）：[closeout-check.mjs:157](.claude/scripts/closeout-check.mjs#L157)——closeout 相位**硬写** `closed` 行（状态列字面量，无推导）；其真值源 = 收口事件本身（能跑到这行 = 承重闸已过）。
- 证据②（写者二）：[changes-index.mjs:135-139](.claude/scripts/changes-index.mjs#L135-L139)——R5 注释（2026-07-24 冻结）自述「C4 退役 final-summary.md → 状态判定改用 verification.json」，判据 = 两件 advisory 投影在场。
- 证据③（SSOT 分类）：[closeout-check.mjs:13-15](.claude/scripts/closeout-check.mjs#L13-L15)——closeout-hardening（2026-07-27 冻结）头注：`supervisor-verdict.json` = 唯一 blocking 机器件；`verdict.json`/`verification.json` = 投影·advisory（non-bearing，本脚本 machineEvidenceFiles 维持 advisory-only）。**R5 与 closeout-hardening 两批各自局部合理、在「包目录里到底有什么」这条缝上互相矛盾，无机械门看护。**
- 证据④（实况 census，2026-09-06 亲跑）：8 个 dev 包**无一**有 verification.json/verdict.json；6 个有 `supervisor-verdict.json`（值 = ESCALATE×5 + APPROVE×1），2 个（cloud-link-bringup / simulator-lvgl-align）无 sv 但有完整收口制品集。8/8 有 `docs-delta.md` + `review-packet.md`（post-cutoff 收口必填两件，[closeout-check.mjs:263-265](.claude/scripts/closeout-check.mjs#L263-L265)）+ `pod-retro.md`。
- 证据⑤（sv 值不可作判据）：device-ws-switch 的 sv `verdict: ESCALATE`（[supervisor-verdict.json 实况](../changes/dev/260903-device-ws-switch/supervisor-verdict.json)，cross-system + security-axis 触发 human adjudication）——包**确已收口**（closeout-check 亲写 closed 行 + 完整制品集），故按 sv **值**解析会把已收口包判 open，按 sv **在场**会把 cloud-link-bringup / simulator-lvgl-align 判 open。sv 排除为判据。
- 结论：唯一与收口事实同构、且全 8 包判对的真值源 = **closeout-check 承重闸自身的 requiredFiles 谓词**。

**问题类 B：arch landed 推导与现格式脱节（T2）**。

- 证据①（实测）：`node --input-type=module -e "import {deriveLanded} …"` 对 `260905-gov-review-verify-gates-align`、`260906-gov-review-architect-loop-advisory` 与 `260906-gov-review-arq-governance-batch` **三批均返 false**（2026-09-06 主线程亲跑 + R1 判官全仓 census 双证），而三批 landing-verdict.md 结论 GREEN 在场、`mergedInMain=yes` 已钉；另 `260830-arch-review-ui-impl-strategy`（散文式结论）与 `260904-gov-review-escalate-merge-approval`（行内 token 非行首）两批同返 false（真 landed 漏标，另因，见 §6 留根）。
- 证据②（格式病灶）：verdict 文件用 `## 7. Verdict` 编号标题（`VERDICT_HEADING_RE` 要求行首即 verdict 词，`7.` 阻断）+ 次段 `**GREEN** — …` 加粗散文行（`VERDICT_STATUS_RE` 要求行首即 verdict 词，`**GREEN**` 阻断）；文件末机器 JSON 行 `"verdict": "GREEN"` 亦无锚。三层现格式全部脱靶；fail-closed 设计（「宁漏标不错标」，changes-index.mjs:149）把 miss 静默成 `frozen`。

**时序账（为何两月未爆）**：2026-07-24 R5 定判据 → 07-27 closeout-hardening 降 advisory 不晋升 → 判据对全包恒 false；日常只有 closeout-check 增量写行使 INDEX 看似正常，全量重建极少跑（header 自述其定位 =「手改/漂移后的纠偏手段」）——潜伏至 2026-09-06 簿记性重建首次大规模执行 R5 判据即爆。

## 2. 改法（T1…T4）

### T1 · shared lib 新增收口谓词单源（governance-gate-shared.mjs）

**改法**：新增两个具名导出（本文件其余导出零触碰）：

```js
// closeout 收口制品谓词单源（260906-gov-review-index-status-truth-source T1，FROZEN 2026-09-06）：
//   消费方 = closeout-check.mjs（承重闸，本批具名导入替换本地拷贝）+ changes-index.mjs collectDev（INDEX dev 状态推导）。
//   防漂移钉死：清单值与判定逻辑只此一份；改清单 = 同时改变闸行为与投影语义，须经治理批。
// CHANGE_ID_CUTOFF 自 closeout-check.mjs 本地常量收敛（按符号名引用，不引行号——同批 T3 删常量后行号即漂）；
//   该处头注自列「禁各自另造」约定副本共 4 处（本批收敛其一；余 3 副本本批不动，见 §6 留根）。
import { CHANGE_ID_PREFIX_RE } from './changeid-shared.mjs';
export const CHANGE_ID_CUTOFF = '2026-07-23';

export function isPostCutoffChangeId(changeId) {
  return CHANGE_ID_PREFIX_RE.test(String(changeId))
    && String(changeId).slice(0, 6) >= CHANGE_ID_CUTOFF.replace(/-/g, '').slice(2);
}

export function closeoutRequiredFiles(changeId) {
  return isPostCutoffChangeId(changeId)
    ? ['docs-delta.md', 'review-packet.md']
    : ['taskpackage.json', 'docs-delta.md', 'review-packet.md'];
}
```

（`changeid-shared.mjs` 为叶子模块、无反向依赖，导入无循环风险——260906 已亲读其全文确认。）

**为什么这样改**：把「收口 = 什么制品在场」的谓词提升为双写者共用的机械单源——本批病灶的本质就是两写者各持一份语义相同的判定而无人对齐；单源化后，未来改清单只存在一处，另一消费方编译期即绑定。**判据选制品集而非 sv 在场**，理由 = §1 证据④⑤（sv 在场漏判 2 真收口包、sv 值不可解析）。
目标文件：`.claude/scripts/lib/governance-gate-shared.mjs`（改：新增 2 导出 + CHANGE_ID_CUTOFF 改具名导出 + 1 具名 import）。

### T2 · changes-index dev 状态推导换谓词 + arch 推导补双锚（changes-index.mjs）

**改法①（collectDev，:135-139 区域）**：删 R5 注释块与 `hasVerdict` 判据，collectDev 改调 T4 提取的导出纯函数（与 selfCheck 共用同一实现，杜绝自检验假实现——NIT-2 统一口径）：

```js
// dev 状态推导（260906-gov-review-index-status-truth-source T2，替代 R5 判据）：
//   closed = closeoutRequiredFiles(changeId) 全在场（与 closeout-check 承重闸同 predicate，lib 单源）。
//   R5 判据（verification.json/verdict.json 在场）作废——该两件按 closeout-hardening SSOT 为 advisory、永不晋升包目录，
//   对全包恒 false（ARQ-260906-index-rebuild-dev-status-drift 病灶）；sv 值/在场均不可作判据（§1 证据④⑤）。
export function deriveDevClosed(dir, changeId) {
  return closeoutRequiredFiles(changeId).every((f) => existsSync(join(dir, f)));
}
// collectDev 内：const status = deriveDevClosed(dir, name.name) ? 'closed' : 'open';
```

（顶部 import 区加 `closeoutRequiredFiles` 至既有 `./lib/governance-gate-shared.mjs` 导入行；`deriveDevClosed` 与 collectDev 同文件、一并加入末尾具名导出清单供 selfCheck 夹具直调。）

**改法②（deriveLanded 双锚，:151-152 区域）**：

- `VERDICT_HEADING_RE` 加编号宽容：`^#{1,4}\s*(?:\d{1,3}[.、]\s*)?(?:verdict|总判|总判定|结论|复验结论)…`——`## 7. Verdict` 命中进 pendingHeading，次段 `**GREEN**` 走既有下扫逻辑取 token（空行不打断 pendingHeading，亲核既有实现成立）。
- 新增 JSON 行锚（在行循环内、两种既有正则之后追加一支）：`/"verdict"\s*:\s*"(PASS|GREEN|RED|FAIL|BLOCK)"/` 命中取 token——机器 JSON 块是 verifier 程序化产出、行文漂移度最低的第二锚；与既有锚冲突时**后出者胜**（JSON 恒在文件末，天然最末取）。

**为什么这样改**：dev 侧与承重闸同真值源（T1 理由）；arch 侧双锚互补——标题锚吃人类行文变体、JSON 锚吃机器格式漂移，且两者都落在「最终 token 后出胜」的既有语义内，不新立判定框架。
目标文件：`.claude/scripts/changes-index.mjs`（改：collectDev 换谓词 + deriveLanded 双锚 + import；selfCheck 夹具扩充见 T4）。

### T3 · closeout-check 谓词改具名导入（closeout-check.mjs，零行为变更）

**改法**（N-3 取舍点主线程裁 **(a)**：常量一并单源具名导出——判官倾向同向，(a) 是 OQ3=(a)「单源化」已批准方向的直接延伸，不留字面量残余）：① 本地 `const CHANGE_ID_CUTOFF = '2026-07-23';` 删除，改从 lib 具名导入 `{ CHANGE_ID_CUTOFF, isPostCutoffChangeId, closeoutRequiredFiles }`；② 原 `:260` 的 `isPostCutoff` 布尔与 `:263-265` 的 `requiredFiles` 三元表达式替换为 `const isPostCutoff = isPostCutoffChangeId(changeId);` + `const requiredFiles = closeoutRequiredFiles(changeId);`；③ **`:250` 语义比较** `firstCommit >= CHANGE_ID_CUTOFF` 与 **`:251` advisory 文案插值 `${CHANGE_ID_CUTOFF}`** 双处引用均改用具名导入常量（R1 实测纠正：PROPOSAL v0.2 误记为「:250 文案」，实为 :250 比较 + :251 文案双引用，删常量不改则双断引用 ReferenceError）；④ `:86-87`「convention 级同步清单 = 四文件」头注同批更新为指向 lib 单源（注释行、零行为——该注落地后即陈迹）。**清单值、谓词逻辑、门行为、写面逐项零变更**（R1 判官已机证：9 样例〔post/pre/无前缀/混合大小写/空/非串〕新旧 requiredFiles 取值逐字相等；§5 V5 landing 后人读 diff 亲证）。
目标文件：`.claude/scripts/closeout-check.mjs`（改：谓词具名导入单源化 + 头注指向更新，零行为变更）。

**为什么这样改**：closeout-check 是 dev 段 closed 行的另一写者——只修 changes-index 侧等于把缝留在原地，下轮清单漂移可无声复发；双写者绑同一 lib 谓词才是病灶的根治。对其承重闸身份的风险控制 = 变更仅限 import 与具名替换，diff 可逐字审计。

### T4 · selfCheck 夹具扩充（changes-index.mjs selfCheck）

**改法**：`selfCheck()` 新增 fixture 用例（沿用 :384-393 `mkFix/rmFix` os.tmpdir 模式，写夹具→跑真函数→清理）：

- dev 谓词三例：post-cutoff id 夹具 {docs-delta.md, review-packet.md} → `closed`；缺 review-packet.md → `open`；pre-cutoff id（如 `260101-x`）夹具 {docs-delta.md, review-packet.md}（无 taskpackage.json）→ `open`。走 **collectDev 将调用的同一推导函数**（T2 把判定提取为可导出纯函数 `deriveDevClosed(dir, changeId)`，collectDev 与 selfCheck 共用，杜绝自检验假实现）。
- landed 两例：`['## 7. Verdict', '', '**GREEN** — 实际落地…']` → true（编号标题 + 加粗行，本批实况格式回归夹具）；`['{"ok": true, "verdict": "GREEN"}']` → true（JSON 锚）；另补 `['## 7. Verdict', '', '**RED** — …']` → false 反例。

**为什么这样改**：F4 活教训（真空绿验证集）——新判据必须非真空：dev 谓词夹具显式覆盖「无 sv 包 closed」与「pre-cutoff 缺 taskpackage.json 判 open」两边界；landed 夹具把本批实况格式（编号标题 + 加粗散文 + JSON 块）固化为回归网。
目标文件：`.claude/scripts/changes-index.mjs`（改：selfCheck 夹具扩充 + deriveDevClosed 导出）。

目标文件汇总：T1 = `.claude/scripts/lib/governance-gate-shared.mjs`（新增）；T2/T4 = `.claude/scripts/changes-index.mjs`（collectDev + deriveLanded 双锚 + selfCheck 夹具 + import）；T3 = `.claude/scripts/closeout-check.mjs`（import 单源化）。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 lib | `.claude/scripts/lib/governance-gate-shared.mjs`（T1） | 改 | 新增 3 导出（1 常量 + 2 函数）+ 1 具名 import；既有导出零触碰；消费方 = 2 脚本（一新一旧） |
| G3 脚本 | `.claude/scripts/changes-index.mjs`（T2/T4） | 改 | INDEX 投影推导（non-bearing）；selfCheck 夹具扩充 |
| G3 脚本（承重闸） | `.claude/scripts/closeout-check.mjs`（T3） | 改 | closeout 相位承重闸——仅谓词具名导入单源化，零行为变更，门逻辑/判定/写面不动 |
| G1 / G2 / 契约面 / spec 实现面 | — | 未触 | 零 |

**sweep 透镜义务核对**：本批**新立判定 pattern 数 = 0**——无新门、无新正则对外、无新事件类型；T2 的两处正则是既有推导函数的内部判据修正，T1 谓词是既有闸语义的具名化。受影响集合机械枚举 = manifest 三件。pod-events-append.mjs 的 `EVENTS_SCHEMA_CUTOFF` 消费链零扰动（本批不涉）。

## 4. human 取舍点（OQ）

**OQ1 · dev 段 closed 判据形态**：human 2026-09-06 裁 **(a)** 收口制品谓词（requiredFiles 全在场，lib 单源，与承重闸同 predicate）。**裁夺过程留痕**：human 首提「pod-retro.md 在场」候选（理由 = retro 是收尾相位产物、语义贴近「收口」）；主线程实证反例——retro 在 closeout-check 为 Q5=warn 非必填（closeout-check.mjs:288-292），存在「闸门已放行、retro 未写」的合法收口包，retro 判据会把这类真收口包投 open（= 本批病灶同类错误），且 cutoff 前老包无 retro（现 0 个，规则失通用性）；human 复裁「那你的推荐吧」定 (a)。落选项：(b) supervisor-verdict.json 在场——cloud-link-bringup / simulator-lvgl-align 两真收口包误判 open（§1 证据④），不推荐；(c) 制品集 ∪ sv 在场——实况等价 (a) 但语义模糊，不推荐。

**OQ2 · arch 段 deriveLanded 修法**：human 2026-09-06 裁 **(a)** 双锚（编号标题宽容 + JSON 行锚）——本批实况格式双保险，JSON 锚抗下轮行文漂移；落选项：(b) 仅编号标题宽容——本次够用，机器 JSON 块弃锚，格式再漂即复发；(c) 只规范未来模板（改 landing-verifier agent def，G2）不补解析——存量两批 260906 永久 frozen，不推荐（R2 校：实况存量三批，含 260905；决策结论不受影响）。

**OQ3 · 单源化范围**：human 2026-09-06 裁 **(a)** closeout-check 同批改消费 lib 谓词（零行为变更、diff 逐字可证）——双写者机械缝本批闭合；落选项：(b) 只改 changes-index 侧——本批 diff 更小，但 closeout-check 本地谓词副本与 lib 之间留缝，与「根治漂移」目标相悖，不推荐。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**landing 前（起草期基线，2026-09-06 已实测或已可实跑）**：

1. `node .claude/scripts/changes-index.mjs --self-check` → 全 pass（含 T4 新增 dev 三例 + landed 三例；基线 2026-09-06 亲跑实测 = 23 例全过，落新例后 total = 29 全过——断言 total≥29 防真空绿）。
2. 实况推演断言（node -e 直调 `deriveDevClosed` 于 8 个真实包目录）：8/8 = closed（含无 sv 的 cloud-link-bringup / simulator-lvgl-align）；反例：夹具缺件目录 = open。
3. `deriveLanded` 三批实况断言：`260905-gov-review-verify-gates-align` / `260906-gov-review-architect-loop-advisory` / `260906-gov-review-arq-governance-batch` → **true**（基线现状 false，本批改后翻转）。
4. `node --check` 三文件全过 + `node --input-type=module -e "import './.claude/scripts/closeout-check.mjs'"` 解析不炸（import 单源化无循环/无路径错）。
5. closeout-check 语义零变更：人读 diff 亲证 `requiredFiles` 替换前后对 pre/post-cutoff 各一样例 changeId 取值逐字相等（post：['docs-delta.md','review-packet.md']；pre：['taskpackage.json','docs-delta.md','review-packet.md']）。

**landing 后（核验侧）**：

6. 全量重建 `node .claude/scripts/changes-index.mjs` → INDEX dev 段 8 行全 `closed`；arch 段 **3 行翻 landed**——`260905-gov-review-verify-gates-align` + 两批 260906（R1 判官 census 实证翻转集为 3 批；grep 三断言）。**预期 INDEX 大 diff（8 行翻 closed + 3 行翻 landed + 段计数修正）= 本批预期纠偏成果，非病灶**；`260830-arch-review-ui-impl-strategy` / `260904-gov-review-escalate-merge-approval` 两批维持 frozen（散文式 / 行内 token 病灶，§6 留根，不在本批翻转集内）。
7. 结构门 `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260906-gov-review-index-status-truth-source` → 绿（manifest 3 行 ≡ T1–T4 锚并集）。
8. 人读 diff 三件：lib 仅新增；changes-index 仅 collectDev/deriveLanded/selfCheck/import 四处；closeout-check 仅 import 与两处具名替换——与 FROZEN §2 逐字对应。
9. Track A 判据复核：manifest 三路径全 `.claude/scripts/` 面，零契约、零 G2、零实现面——机械枚举断言。

## 6. 留根清单

**不关的根（登记不硬治 / 归口他处）**：

- CHANGE_ID_CUTOFF 余 **3 处**约定副本——`taskpackage-required-check.mjs:95` / `devloop-artifact-check.mjs:70` / `hooks/supervisor-gate-check.mjs:58`（R1 判官 grep 实证，纠正 v0.2 漏列 taskpackage-required-check 的计数错）。本批收敛 closeout-check 一份后余 3；lib 单源已立，余副本改具名导入 = 机械替换小批（钩子副本归「不动钩子」纪律下批）。
- landing-verifier verdict 行文模板规范化（`> verdict: PASS/GREEN` 规范行，changes-index.mjs:150 注释所述约定未被执行）——G2 agent def 面，另批评估；本批 JSON 锚 + 编号标题锚已从解析侧吸收该漂移类。
- supervisor-verdict 值语义（ESCALATE = 触发 human adjudication，收口后仍是收口）在 INDEX 不区分——INDEX 是粗投影；细粒度状态消费方去读 pod-retro.md / review-packet.md。
- INDEX「已合main」列对 dev 段不存在（arch 列专属）——列结构改动超本批 scope。
- `260830-arch-review-ui-impl-strategy`（结论为「放行建议」散文）与 `260904-gov-review-escalate-merge-approval`（结论为「`**最终 verdict：GREEN。**`」行内 token 非行首）两批真 landed（mergedInMain=yes、四件齐，R1 判官亲核）在本批新逻辑下仍 fail-closed 漏标——按设计归 `LANDED_ADJUDICATION` 人工钉死；因 adjudication 条目头注要求人工核 verdict 确认，**不并入本批**（两批均 human 已批合并、证据在册，human 可在本批 ack 时一句话追认，或归下一小批补钉）。
- `changeid-shared.mjs:29-31` 裸 `process.argv.includes('--self-check')` 自检块——被 changes-index 具名 import 即触发，致 `--self-check` stdout 吐双 JSON 行（R1 判官实测复现；全仓无机器消费方、exit code 不受影响，输出面变脏而已）。改 `invokedDirectly` 判式 = 小微批，归口下批（changeid-shared 不在本批 manifest）。

**交日落复盘项**：本批落地后首次 dev loop 收口（closeout-check 实跑）时，核对其 INDEX 行写入与本批重建投影是否一致（同 predicate 应天然一致，实跑一次确认）。

**自律件（不进 landing）**：下次全量重建后 diff 若再出现 dev 段状态大面积翻动，视为谓词漂移信号，直接查 lib 单源是否被旁路。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-06 · architect 判官 · 结论：NEEDS-REVISION

**评审范围声明**：本 R1 对 §0–§6 全文做逐项实证评审，全部证据由判官亲跑亲读（命令 + 输出摘要随项附），未采信 PROPOSAL 自述。判官全程只读 canonical，scratch 实验均在 `c:\tmp\r1-scratch\`（仓外），本记录是唯一写入。

**透明披露**：判官 = architect 实例，本批 manifest 三件（governance-gate-shared.mjs / changes-index.mjs / closeout-check.mjs）的 OWNER 均为 architect（G3 强制层），属自涉面；但本批为纯判官评审（不落地、不实现），且所有证据均机器可复跑，自涉风险由实证密度对冲。

**A. 硬项 2a（锚清单 ≡ manifest）——不通过（Finding N-1）**。manifest 围栏块在场（3 路径），packet-id 行在场（头部第 6 行）✓；manifest 三路径与 T1–T4 的语义目标逐一对应 ✓。但 **§2 各 T 条目均无「目标文件：」锚行**（锚信息只在 §2 末尾「目标文件汇总」散文段，该行不匹配解析器的 `/^目标文件\s*[:：]/`），机械对账实测：`parseSection2AnchorLists(PROPOSAL)` 返 4 条缺锚 fail-loud（T1/T2/T3/T4 各一），union=[] ≢ manifest=[3 路径]。landing-fidelity-check.mjs:266-278 把 problems 直接进 `errors`（硬失败道）——**按现状落地，结构门必红**，§5 V7「结构门 → 绿」断言不成立。对照先例：260906 两批已落地 PROPOSAL 均为每 T 一条「目标文件：」行（grep 实证，含整裁腿形态）。

**B. 硬项 1a（§1 证据断言逐项亲跑亲读）——12 项全部成立**：
1. closeout-check.mjs:157 亲读：`const newRow = \`| [${changeId}](...) | ${date || '—'} | ${originRef || '—'} | closed | ${workstream || '—'} |\``——状态列硬编码 `closed` ✓。
2. closeout-check.mjs:14-15 亲读：SSOT 分类句在场（`supervisor-verdict.json = 唯一 blocking 机器件（supervisor Stop 门 G2）`；`verdict.json / verification.json = 投影·advisory（non-bearing…）`）；关键句实跨 :13-15（见 NIT-4）✓。
3. closeout-check.mjs:263-265 亲读：requiredFiles 两段清单逐字一致（post=`['docs-delta.md','review-packet.md']`；pre=`['taskpackage.json','docs-delta.md','review-packet.md']`）✓。
4. closeout-check.mjs:89 亲读：`const CHANGE_ID_CUTOFF = '2026-07-23';`，:86-87 头注「convention 级同步清单 = **四文件**」✓——注意头注是四文件，PROPOSAL T1 注记与 §6 的副本计数与此不符（Finding N-3）。
5. closeout-check.mjs:288-292 亲读：pod-retro 牙 = `if (isPostCutoff)` + `advisories.push('pod-retro.md missing…(Q5=warn advisory; zero-retroactivity…)')`——post-cutoff 且 warn 级 ✓，支撑 OQ1 否 retro 判据。
6. changes-index.mjs:135-139 亲读：R5 注释块 + `hasVerdict` 判据逐字一致 ✓。
7. changes-index.mjs:149 亲读：fail-closed 设计句逐字一致（「宁漏标不错标」）✓。
8. census 亲跑：8 个 `changes/dev/*/` 无一含 verification.json/verdict.json；6 个有 supervisor-verdict.json（逐包 python 读值：ESCALATE×5 + APPROVE×1〔ui-art-assets〕）；2 个无 sv 包 = cloud-link-bringup / simulator-lvgl-align，均有 docs-delta.md + review-packet.md + pod-retro.md；8/8 有 docs-delta + review-packet + pod-retro ✓。另证：8 包 changeId 均 ≥ 260829，全 post-cutoff，「pre-cutoff 老包现 0 个」成立 ✓。
9. device-ws-switch supervisor-verdict.json 亲读：`"verdict": "ESCALATE"`（cross-system + security-axis 触发）✓——sv 值不可解析为判据成立。
10. `node --input-type=module -e "import {deriveLanded}…"` 亲跑：两批 260906 均返 `false` ✓。
11. 两批 landing-verdict.md 亲 grep：advisory = L71 `## 7. Verdict` + L73 `**GREEN** — …` + L76 `"verdict": "GREEN"`；batch = L87 / L89 / L92 同构 ✓。
12. INDEX 头注亲读：「手改会被全量重建覆盖。INDEX 不是第二事实源，不做拦阻依据。」逐字在场 ✓；消费方 grep（`.claude/scripts`、`hooks`、verify-gates、settings）——**无任何脚本/钩子读 INDEX.md 的 dev 状态列**（closeout-check 只写不读；planning-index 是 .planning 另一 INDEX；merge-to-main SKILL 仅作人读冲突热点提及）——「无门消费」断言成立 ✓。另：`git show f0aaa87` 亲证实录——该次 upsert 触发全量重建，diff 含 `-| … | closed | …` → `+| … | open | …`（device-ws-switch 行），病灶实录逐字吻合 ✓。

**C. §5 验证集评估——设计 sound，两处修正**：
- V1 基线亲跑：`--self-check` = 23 pass / 0 fail / total 23 ✓；T4 新例算术 23+6=29 ✓。`total≥29` 用 ≥ 而非 = 合理（==29 会对未来合法加例脆断）；「少加例」被 ≥ 拦（加 4 例则 27<29 fail），「加水例」残余风险由 T4 钉死的具名例清单 + landing-verifier 人读 diff 对冲——可接受。
- V2 预演亲跑（新谓词模拟于 8 真实包目录）：8/8 = closed（含两无 sv 包）✓，landing 后可判定性强。
- V3 = B.10 的 false×2 基线已实证，改后翻转 true 的断言机械可判定 ✓。
- V4/V5 设计成立；V5 我已用机器比对提前加固（见 D.4）✓。
- **V6 预期 diff 计数错误**：实况翻转集 = **3 批**（见 Finding N-2）。

**D. 改法技术合理性——主体成立，一处替换计划不完备**：
1. T1：slice 算术亲跑 `'2026-07-23'.replace(/-/g,'').slice(2)` = `'260723'`，与 closeout-check.mjs:260 表达式逐字同形 ✓；lib→`./changeid-shared.mjs` 同目录相对路径正确 ✓；changeid-shared.mjs 全文亲读零 import（叶子），governance-gate-shared 新增该 import 无环 ✓；pod-events-append.mjs 亲读仅 3 行 node builtins import、无 lib 依赖、本批不引 ✓。
2. T2①：collectDev 全文亲读——`dir`（:122）、`name.name`（循环变量）在替换点均在作用域；`join`/`existsSync` 已 import（:19-20）；:23 已有同 lib 的 `resolveOriginRef/isFrozen` import 先例，合并或并列均合法 ✓。
3. T2② scratch 实证（构造修改后正则 + JSON 锚，模拟行循环，read-only）：**两真实文件均取最终 token GREEN**（false→true）✓；selfCheck 既有 4 夹具 + T4 新增 3 夹具全过 ✓；误命中探针：`## 3. 结论与展望`/`## 3. 结论`/`### 2. 复验结论` 均不命中（CJK 字符后 `\b` 不成立，**向 fail-closed 偏**，与 `gov-review-governance-doc-restructure` 需人工 adjudication 的史例一致）✓；`## 12. 总结` 不在词表 ✓；`## 7. Verdict` 后无 token → false（fail-closed 保持）✓。**知情记录一处 widened surface**：构造例 `## 4. Verdict analysis` + 后文行首 `**GREEN**` 散文 → 误 true（英文 verdict 词编号标题的 pendingHeading 化）；残余风险可接受——INDEX 投影 non-bearing（B.12 证无门消费）、需复合形态才触发、adjudication 缓存可人工覆盖。
4. T3：grep 全量引用点 = :87 头注、:89 定义、**:250 比较表达式** `firstCommit >= CHANGE_ID_CUTOFF`、:251 文案插值、:260 谓词。**PROPOSAL 只处理了「:250 文案」，漏了 :250 的比较引用（语义引用，非文案）与 :251**——按现文删 :89 后 :250/:251 双断引用、脚本必炸 ReferenceError（Finding N-3 的另一半）。零行为变更声明本身经机器加固：9 样例（post/pre/无前缀/混合大小写/空/非串）新旧 requiredFiles 取值逐字相等 ✓。
5. T4：mkFix 扩展多文件夹具与 deriveDevClosed 导出纯函数共用设计 sound（杜绝自检验假实现）✓；pre-cutoff 缺 taskpackage.json → open 的反例覆盖真实边界 ✓。

**E. Track A 判据独立复核——两判据均否，维持 Track B 成立**：manifest 三路径全 `.claude/scripts/` 面（CANONICAL_TEST 可机锚，不踩 ARQ-260906-lfc-canonical-anchor-gap）✓；零契约、零 G2、零 spec/实现面 ✓；非跨系统结构性（单 G3 投影修复 + 承重闸等价重构，行为等价已机证）✓。

**F. Track B 三要件 + 机检硬排除——齐**：human 立项 ack 凭据在头部（「记录ARQ，出proposal，快速修了」2026-09-06 引文）✓；OQ 三裁（(a)/(a)/(a)）在 §4 且 OQ1 含 retro 备选被否的裁夺留痕 ✓；骨架 §0–§7 齐 ✓；机检硬排除逐项过（不触 contracts、不动 verify-gates 门语义、非 mixed lane、不触 L0、无门增删、无新事件类型；§3 sweep 声明「新立判定 pattern 数 = 0」经 D 项亲核成立）✓。

**G. §6 留根对照——一处事实错误**：ARQ-260906-index-rebuild-dev-status-drift 在册（docs/ARCH-REVIEW-QUEUE.md:37，归口「下次动 changes-index.mjs 的治理批」= 本批 ✓）；ARQ-260906-lfc-canonical-anchor-gap 与本批 manifest 可机锚声明无冲突 ✓。**但 CHANGE_ID_CUTOFF 副本计数错**：closeout-check:86-87 头注声明「同步清单 = **四文件**」，grep 实证 taskpackage-required-check.mjs:95 / devloop-artifact-check.mjs:70 / hooks/supervisor-gate-check.mjs:58 三处均仍持 `'2026-07-23'` 本地副本——本批收敛 closeout-check 后**余 3 副本**，PROPOSAL T1 注记（「3 处约定副本之一」）与 §6（「余 2 处」，漏列 taskpackage-required-check.mjs）均与实况不符（Finding N-3 的第三半）。

**Findings 分类**：

- **N-1（NEEDS-REVISION，落地阻断）**：§2 T1–T4 补逐 T 锚行。**授权主线程机械落字**（四句，落字后 union ≡ manifest 机检自过）：
  - T1 条目末补：`目标文件：\`.claude/scripts/lib/governance-gate-shared.mjs\`（改：新增 2 导出 + 1 具名 import）。`
  - T2 条目末补：`目标文件：\`.claude/scripts/changes-index.mjs\`（改：collectDev 换谓词 + deriveLanded 双锚 + import）。`
  - T3 条目末补：`目标文件：\`.claude/scripts/closeout-check.mjs\`（改：谓词具名导入单源化，零行为变更）。`
  - T4 条目末补：`目标文件：\`.claude/scripts/changes-index.mjs\`（改：selfCheck 夹具扩充）。`
  - §2 末「目标文件汇总」段可保留作散文总览，不冲突。
- **N-2（NEEDS-REVISION，证据完整性）**：arch 翻转集实况 = **3 批**。判官 census（旧 deriveLanded vs 模拟新逻辑 × 全 11 arch 目录，含 adjudication/cache 优先层）实测翻转：`260905-gov-review-verify-gates-align`（其 landing-verdict.md 同为 `## 7. Verdict`+`**GREEN**`+JSON GREEN 新格式，mergedInMain=yes、retro 齐——**同病灶第三批，翻 true 是正确纠偏**）+ 两批 260906。**修订指令**：§1 问题类 B 证据①「实测两批 260906 均 false」扩为含 260905 的三批实况；§5 V6「2 行翻 landed」改「3 行翻 landed」并点名 260905。同时**建议 §6 留根增记一条**：`260830-arch-review-ui-impl-strategy`（结论为「放行建议」散文）与 `260904-gov-review-escalate-merge-approval`（结论为「`**最终 verdict：GREEN。**`」行内 token 非行首）两批真 landed（mergedInMain=yes、四件齐）在新逻辑下仍 fail-closed 漏标——按设计归 LANDED_ADJUDICATION 人工钉死；因 adjudication 条目头注要求人工判定，不并入本批，登记由 human 核 verdict 后的后续小批补钉。
- **N-3（NEEDS-REVISION，T3 替换计划完备性 + §6 事实）**：T3 冻结文本须钉死 :250/:251 处置——二选一：(a) lib 一并 `export const CHANGE_ID_CUTOFF`（T1 代码块同步加 export），closeout-check 具名导入用于 :250 比较与 :251 文案（保单源最干净）；(b) :250 比较与 :251 文案双改字面量 `'2026-07-23'`（行为等价已由判官机证，但本文件留一份字面量，须在 §6 登记为残余副本）。另：T3 落地后 closeout-check:86-87「同步清单 = 四文件…本文件」头注即成陈迹，**同批更新该注释**（注释行、零行为）；§6 副本计数改「余 3 处（taskpackage-required-check / devloop-artifact-check / supervisor-gate-check）」，T1 注记「3 处副本之一」改「4 处约定副本之一」。**裁 (a) 还是 (b) 属设计取舍，留主线程/human**；判官倾向 (a)。
- **NIT-1**：T1 落地后 `node changes-index.mjs --self-check` 会先吐一行 `{"selfCheck":"changeid-prefix",…}`——changeid-shared.mjs:29-31 的自检块用裸 `process.argv.includes('--self-check')`，import 即触发（判官实测：`node import-test.mjs --self-check` 双 JSON 行）。grep 全仓无脚本/钩子机器解析 changes-index stdout，无污染闸、exit code 不受影响，但 V1 输出面变脏。**不在本批扩 scope 修**（changeid-shared 不在 manifest）——建议 §6 留根登记「changeid-shared 自检块改 invokedDirectly 判式」小微批。
- **NIT-2**：T2① 代码草块在 collectDev 内联 `.every` 判定，T4 又说提取 `deriveDevClosed` 共用——冻结前统一表述为「collectDev 调 deriveDevClosed(dir, name.name)」，免 landing 人读 diff 口径摇摆。
- **NIT-3**：T1 注释引「closeout-check.mjs:89」行号——同批 T3 删常量后行号即漂；建议注释引符号名不引行号。
- **NIT-4**：§1 证据③引 closeout-check.mjs:14-15，SSOT 分类句实跨 :13-15；顺手扩为 :13-15。

**结论与后续序列**：病灶诊断、谓词选型（制品集谓词 8/8 判对 vs sv 6/8）、双锚设计、零行为变更证明、Track B 分流、三要件与硬排除——主体工程判断全部经实证成立，判官对设计本身无保留。三条 NEEDS-REVISION 均为**机械可落的文本/计划修正**（N-1 已授权主线程直接落字；N-2 为证据与计数修正 + 一条留根增记；N-3 含一个二选一取舍点须主线程/human 裁后定稿），无设计返工。修订后转 R2 快核（判官重点复验：锚行四句机检对齐、:250/:251 处置与头注更新落字、§1/§5/§6 三处计数与留根改文），预期一轮可 GREEN。

### R2 · 2026-09-06 · architect 判官 · 结论：GREEN（附四条判官授权机械落字，FROZEN 前落、landing-verifier 复核）

**复验范围**：对照 R1 Findings（N-1/N-2/N-3 + NIT-1~4）逐条核 v0.3 落字，四项派遣重点全验，另做修订新矛盾全文排查。判官本轮同样只读 canonical + 亲跑机检，本记录是唯一写入。

**① 锚行机检对齐（N-1 复核）——通过**。判官独立复跑 `parseSection2AnchorLists(v0.3)` + `compareAnchorSets`：`union = [governance-gate-shared.mjs, changes-index.mjs, closeout-check.mjs]`，`problems = []`，`aligned = true`——union ≡ manifest 双向对齐，缺锚 fail-loud 清空。逐 T 锚行均在（T1 :81 / T2 :106 / T3 :111 / T4 :123），「目标文件汇总」散文段保留不冲突。结构门锚断言路径已通。

**② T3 冻结文本完备性（N-3 复核）——通过**。T1 代码块已含 `export const CHANGE_ID_CUTOFF = '2026-07-23'`（裁 (a) 落实）✓；T3 改法四条钉死：① 删本地常量 + 具名导入 `{ CHANGE_ID_CUTOFF, isPostCutoffChangeId, closeoutRequiredFiles }`；② :260/:263-265 谓词与清单替换；③ **:250 语义比较与 :251 文案插值双处均走导入常量**（R1 指出的 ReferenceError 断引用面闭合——判官 R1 grep 全量引用点 :87/:89/:250/:251/:260 五处，现 ①+③+④ 全覆盖）；④ :86-87 头注同批更新（注释行、零行为）✓。补验：cross-surface-check.mjs 不扫 CUTOFF 字面量/拷贝数（grep 空），lib 新导出不触既有防回退机械面 ✓。

**③ §1/§5/§6 计数与留根改文（N-2 复核）——通过**。§0 arch bullet 改「260905 一批 + 两批 260906 共三批均 false，260830/260904 另有散文式病灶」✓；§1 证据①扩为三批 false（主线程亲跑 + R1 判官 census 双证）+ 增记 260830/260904 另因漏标 ✓——与判官 R1 census 输出逐批吻合；§5 V6 改「3 行翻 landed」、点名 260905、显式声明 260830/260904 不翻转 ✓；§6 增记两批归 LANDED_ADJUDICATION（human 一句话追认或归下一小批的双轨表述，主线程默认不扩 scope——分寸正确）✓、changeid-shared 双 JSON 留根登记 ✓（NIT-1 落实）、副本计数「余 3 处」点名三文件 ✓、T1 注记改「4 处约定副本之一」+ 符号名引用 ✓（NIT-3 落实）、§1 证据③引文扩 :13-15 ✓（NIT-4 落实）、T2① 统一为 collectDev 调 `deriveDevClosed(dir, name.name)` 且 T2/T4 口径互洽 ✓（NIT-2 落实）。

**④ 修订新矛盾排查——发现一处事实残留 + 三处表述瑕疵，均判官授权机械落字**：

- **R2-NIT-1（必落，事实矛盾）**：§0 非目标③（:31）仍写「不收敛 CHANGE_ID_CUTOFF 的其余 **2 处**约定副本（devloop-artifact-check.mjs / hooks/supervisor-gate-check.mjs——…）」——v0.2 旧计数残留，与同文 §6:167「余 **3 处**」（taskpackage-required-check.mjs:95 / devloop-artifact-check.mjs:70 / supervisor-gate-check.mjs:58，判官 R1 grep 实证）直接打架。**授权主线程机械落字**：「其余 2 处约定副本（devloop-artifact-check.mjs / hooks/supervisor-gate-check.mjs——不动钩子，本批纪律）」→「其余 3 处约定副本（taskpackage-required-check.mjs / devloop-artifact-check.mjs / hooks/supervisor-gate-check.mjs——钩子副本不动，余归机械替换小批，见 §6）」。
- **R2-NIT-2（必落，表述一致）**：§3 表 T1 行（:131）「新增 2 导出 + 1 具名 import」与 T1 代码块实际 3 个 export 语句（1 常量 + 2 函数）不符。**授权落字**：「新增 2 导出」→「新增 3 导出（1 常量 + 2 函数）」。
- **R2-NIT-3（建议落，前后口径）**：§5 V3（:152）仍写「双批实况断言」只点名两批 260906；翻转集既已更正为三批，landing 前断言名单宜含 260905（V6 在 landing 后覆盖三批，验证不缺，此处纯口径对齐）。**授权落字**：「双批实况断言」→「三批实况断言」，名单加 `260905-gov-review-verify-gates-align`。
- **R2-NIT-4（建议落，校注 + typo）**：OQ2 落选项 (c)（:142）「存量两批 260906 永久 frozen」系决策当时认知（实况三批含 260905）——历史决策记录不改写，**授权加校注**「（R2 校：实况存量三批，含 260905；决策结论不受影响）」；T3 ③（:110）「`${CHANGE_ID_CUTOFF}`` 双处引用」多一个反引号，顺手修单。

**结论**：R1 三条 NEEDS-REVISION 全部闭环（N-1 机检对齐、N-2 计数与留根与实况逐字吻合、N-3 裁 (a) 且替换计划完备），NIT-1~4 全部落实，修订未引入新设计面、未碰 manifest 外路径、Track B 三要件与机检硬排除维持。判 GREEN。后续序列：主线程落上述四条授权机械修订（R2-NIT-1/2 必落、3/4 建议落）→ 凭「立项 ack + 本 GREEN」标 FROZEN → packet-bound landing → landing-verifier 独立核验（复跑锚对账 + 逐文件 diff + §5 验证集）→ retro → 前置升级闸。判官提醒落地侧两点：V1 落新例后断言 `total≥29`；V6 grep 断言按三批名单（260905 + 两批 260906）执行。
