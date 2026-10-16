# landing-verdict · 260914-gov-review-delivery-restructure

> 判定：**FAIL（RED）——退回主线程修 landing，不放行 merge。**
> 判定人：landing-verifier（独立核验，只读 codebase、只写本件）。核验日期 2026-09-15。基线 `3dda7bd` → landing commit `bd81f57`。

## 0. 结论与根因

结构保真全绿（结构门 ok:true、137 锚全落、零越界件、历史面 508 件纯 R100 搬运），但人读 diff 抽样发现 **4 处批量替换引入的逻辑损伤**（3 件 G3 门脚本运行即崩 + 1 件 C 验收件编译必炸），landing-summary 均未申报、机械门集均探不到。ErrorEnvelope：

```json
{
  "ok": false,
  "failingFile": ".claude/scripts/closeout-check.mjs（另 3 件，见 §2）",
  "expected": "T2 冻结改法 = 路径常量按映射表平移、逻辑零变更（closeout-check.mjs:317 应为 taskPackage.deliveryMilestoneStateChanged；device_ws_switch.acceptance.c:864 应为 wsa_scan_tree_both(\"delivery/changes\", …) 保持 7 参）",
  "actual": "taskPackagedelivery/milestonesMilestoneStateChanged（点号成员访问被正则吃掉，成 undefined 标识符 + 除法表达式）；C 实参被塞成 8 参（char* 传入 int 形参 ci）",
  "category": "批定改法落错——落地引入逻辑损伤（越出 packet 的「行为保持」硬约束）"
}
```

**根因假设**：分段字面量替换把 `'.delivery'` / `'.planning'` 两条映射实现成了无右边界守卫的 `.delivery` / `.planning` 形式，命中了点号成员访问表达式（`taskPackage.deliveryImpact`、`finding.planningRecommendation`）并连同成员点号一起吃掉；C 双引号实参规则按「前插一段」实现而 `wsa_scan_tree_both` 是固定 7 参签名、正解是单串 `rel_dir` 改写。三类宿主都不在任何机械门可见域：V1 主/子检只认旧路径 token（这些行改后不含旧路径形）、`node --check` 对除法表达式放行、三脚本不在 V3 自检 10 件清单内——与 landing-summary §3-4 已申报的「孤儿反斜杠」同族（正则字面量损伤），但 §3-4 只修了 10 处转义形、漏掉了点号粘连形与 C 实参形。

## 1. 结构门与对账（全绿部分）

- **landing-fidelity-check.mjs**：`ok:true`，exit 0，baseRef = HEAD~1（= 3dda7bd，无需基线重跑）。`unauthorized` / `claimedNotLanded` / `plannedNotLanded` 全空。citation advisory 1 条（`.delivery/delivery-workspace.json:19` file-not-exist）= 迁移前路径的证据引述，迁移后必然失配，advisory 级、可接受。
- **manifest ∪ rename 面全量对账**：diff 共 649 件 = 519 R + 126 M + 2 A + 2 D。非 rename 触碰件 130 件中 127 件 ∈ manifest（137 锚），非 manifest 残余**恰为 3 件已知例外**：`changes/INDEX.md`（D）+ `delivery/changes/INDEX.md`（A，45+ 行全量重建致 rename 检测失效、实质同件）、`changes/dev/.gitkeep`（D，T1 明文删除）、`architect-landing-summary.md`（A，packet 制品）。519 件 rename 全部落于 `delivery/` 下（零杂散），其中 508 件 R100 纯搬运、11 件带内容改动（10 件 manifest 锚 + PROPOSAL.md 自身，见 §3-10 核验）。
- **逐类抽样**（超出任务最低要求）：机械层 mjs 7 件（changes-index / closeout-check / verify / delivery-status / governance-gate-shared / arch-review-handoff-check / taskpackage-required-check）、治理 markdown 7 件（repo-structure.md / AGENTS.md / DEV-GOVERNANCE.md / architect.md / requirements / ADR / ARQ）、代码面 5 件（device_ws_switch.c / char_stroke.c / voice_roundtrip_b.c / test_gateway_directionB.py / test_story_wiring.py + test_g_relay_fix.py）、契约面 1 件、迁入/数据件 5 件（delivery-workspace.json / milestones.jsonl / current/taskpackage.json / workstreams/README.md / changes/INDEX.md）、忽略/属性面 3 件（.gitattributes / .cbmignore / .claude/.cbmignore）。
- **干净面实证**：scope-map glob 全量平移 + `bashDenyWrite` 增补 `delivery/taskpackages/**` 到位、`_bashDenyWrite_doc` 显式枚举改写（§3-2 双应用损伤已修复属实）；AGENTS.md 恰两行声明改动（INDEX 路径 + KidPal 图表）；architect.md 仅路径指针、零权限/写域扩张；milestones.jsonl 键名 `planningRefs` 未损、值改新前缀；delivery-workspace.json `planRoot`/`milestoneBaseline`/`statusRollup` 三处到位；代码面抽样的 .py 全为 docstring/消息串/路径常量（`CHANGES_DEV`、`LIVE_EVIDENCE_DIR` 等 pathlib join 形延长合法）；`firmware/server` 非测试实现面 diff **零件**；`.gitattributes` merge=union 行顺修至 `_archive/2026/` 实位。
- **契约面**：`docs/contracts/interface-contract.md` 全 diff 恰 3 行（行 4/5/96），行 4 替换句与 T6 冻结文本逐字一致，行 5/96 纯前缀平移；`docs/contracts/**` 其余零触碰。T6 保真成立。

## 2. 红项：落地逻辑损伤（4 处，全为 T2 批改面越出「行为保持」）

以下均为 `git diff 3dda7bd..bd81f57` 实证（旧行 → 新行），非 summary 自述：

1. **`.claude/scripts/closeout-check.mjs:317`**（顶层语句）：`taskPackage.deliveryMilestoneStateChanged` → **`taskPackagedelivery/milestonesMilestoneStateChanged`**——`taskPackagedelivery` 未定义标识符，脚本每次运行到此必抛 ReferenceError，closeout 机械闸瘫痪。语法合法（`node --check` 过），故 summary §3-4 的全量 `node --check` 体检探不到。
2. **`.claude/scripts/closeout-check.mjs:332-333`**（顶层语句）：`taskPackage.planningWorkstreamRequired` → **`taskPackagedelivery/workstreamsWorkstreamRequired`**；`taskPackage.planningRefs || taskPackage.planningRef` → **`taskPackagedelivery/workstreamsRefs || taskPackagedelivery/workstreamsRef`**。同上运行即崩。
3. **`.claude/scripts/taskpackage-required-check.mjs:198-199,236-243`**：`taskPackage.deliveryImpact` → **`taskPackagedelivery/milestonesImpact`**（×4 处）、`taskPackage.planningImpact` → **`taskPackagedelivery/workstreamsImpact`**（×6 处）、`taskPackage.planningRefs` → **`taskPackagedelivery/workstreamsRefs`**（×4 处）——required-check 门顶层判定行全数失真，运行即崩。
4. **`.claude/scripts/arch-review-handoff-check.mjs:117-118`**：`finding.deliveryImpact` → **`findingdelivery/milestonesImpact`**、`finding.planningRecommendation` → **`findingdelivery/workstreamsRecommendation`**——校验函数体内，触发即崩。

5. **`firmware/main/test/device_ws_switch.acceptance.c:864`**：`wsa_scan_tree_both("changes", "shim.py", "archiv", 1, NULL, hit, sizeof(hit))`（7 参，签名 :372 `rel_dir, needle, needle2, ci, exclude_base, hit, cap`）→ **`wsa_scan_tree_both("delivery", "changes", "shim.py", "archiv", 1, NULL, hit, sizeof(hit))`**（8 参）——实参超签名一位，`"archiv"`（const char*）落到 `int ci` 形参，host runner 编译必炸。冻结 T2 对 C 形只说「映射同喂：前插 delivery 段」，但正解是单串 `rel_dir` 改 `"delivery/changes"`（对偶实证：voice_roundtrip_b.acceptance.c 的 `vrt_find_evidence` 单串形改法正确，说明此处是执行错误非 packet 歧义）。

**为何漏网**：V1 主命令与子检 2 的正向模式匹配旧路径 token，上述行改后不含任何旧路径形；`node --check` 对 `x / y === true` 形态放行；closeout-check / taskpackage-required-check / arch-review-handoff-check 均不在 V3 十件自检清单。全仓粘连形扫描（`[A-Za-z_]delivery/(milestones|workstreams|taskpackages|changes)` 等）确认粘连损伤**仅此 3 件 mjs**、无更多宿主。

## 3. landing-summary §3 十条诚实注记逐条裁断

| # | 注记 | 裁断 |
|---|---|---|
| 1 | 两轮批量替换（r4 前导卫兵漏相对链接形，第二遍补） | 属实，机制合理；但第二遍正是本 verdict §2 粘连损伤的引入轮——申报了「双应用损伤 1 处」却漏了同族的点号成员访问损伤 |
| 2 | scope-map `_bashDenyWrite_doc` 双应用损伤修复 | **属实、已修复**（diff 实证：现文为显式枚举 `delivery/taskpackages/、delivery/changes/dev/`） |
| 3 | 越界吃历史行后定点恢复 | **属实、恢复干净**：CDQ 零 diff（配合 §3-10 剔出）；ADR 仅 :3；requirements 仅 610/676 两活行；ARQ 12 处 hunk 全落 §0/§1（§2 自 :88 起、零触碰），活行改动均为路径指针 |
| 4 | 正则字面量损伤 10 处手工修复 + node --check 过 | 申报部分属实；**但不完整**——同族点号粘连损伤 ≥14 处（§2 的 1–4）与 C 实参形漏修，node --check 对后者天然失明。此条申报反而掩盖了残留 |
| 5 | 新增行级豁免 2 类 | **均正当**：① DEV-GOVERNANCE:20 实证为原仓名引述句按原意重写（`原仓（AInativecompany）的 .delivery/.planning … 与本仓 delivery/** 无关`），语义保真；② changes/INDEX origin 数据列 46 行确为历史包 taskpackage.json origin.ref 投影（`.planning/…`、`changes/dev/…` 原值留档），链接列 46/46 全新前缀，改写反会伪造历史——接受豁免 |
| 6 | 子检 1 冻结命令缺第二段过滤（补滤版为准） | 口径瑕疵属实、补滤合理；子检 2 复跑 0 命中实证（本 verdict §4） |
| 7 | 主索引刷新为 landing 后操作项 | 合理（与 T5「操作项」条款一致） |
| 8 | 手工语义修正 2 处 | 均实证吻合（DEV-GOVERNANCE:20、.gitattributes 顺修） |
| 9 | repo-structure.md 三行新增被权限分类器误拦、human 放行 | 记录属实（diff 见三行 delivery 表 + 「工作产物去哪」改写到位）；流程注记，不影响保真 |
| 10 | post-FROZEN 结构修正 3 项 | **可接受**：PROPOSAL.md diff 恰为声明内容（四目录根锚剔除 + CDQ 剔除 + 各 T 补「目标文件：」行 + manifest 注记）；「目标文件」行合计 137 与 manifest 逐一对应；三项均为机检 fail-loud 所迫的透明小修、零 landing 面变更，且已回写 FROZEN 件留痕（严格说 post-FROZEN 改 packet 本身应零——但内容为锚清单形式补齐、语义零变、与 gate 结果互证，判可接受，retro 收录） |

## 4. 残留清扫复跑（任务项 4）

- **V1 主命令**（PROPOSAL §5 原文）：命中全部落在申报豁免集——`ledger.jsonl:2`（1 行）、`ADR.md` 4 行（:16/:24/:31/:32 历史决策行）、`ARQ.md` §2 已结索引 :90–:128（39 行）、`CDQ.md` 3 行（:8/:9/:11 已结行）、`requirements.md` 3 行（:682/:693/:734 版本历史）、`DEV-GOVERNANCE.md:20`（§3-5 豁免①）。**豁免外零命中** ✓。
- **V1 子检 2**（字面量全家族，作用域含代码面）：**0 命中** ✓。
- **V4 route-regression-check**：28 ok / 0 fail，exit 0 ✓。
- 注：V2 派生索引再生与 V3 自检未复跑（涉及改写投影件）；但 V3 清单不含三受损脚本，复跑也无法为 §2 红项翻案。

## 5. 破坏性自检（任务项 5）

- `docs/contracts/**`：全 diff = interface-contract.md 恰 3 行（行 4/5/96），与 T6 冻结文本逐字一致；契约其余零触碰 ✓。
- `firmware/server` 实现代码本体：非 test/acceptance/evidence/deploy 面 diff **零件**；测试面抽样改动全为注释/消息串/路径常量（唯一代码行例外 = §2 红项 5 的 C 实参损伤）✓（除该红项）。

## 6. 放行条件（退回主线程的修复清单）

1. 修复 3 件 mjs 的点号粘连损伤（恢复 `taskPackage.deliveryImpact` / `taskPackage.planningRefs` / `finding.planningRecommendation` 等原标识符，仅改其内的路径常量字符串），逐处 diff 复核。
2. 修复 `device_ws_switch.acceptance.c:864` 为 7 参形 `wsa_scan_tree_both("delivery/changes", "shim.py", "archiv", 1, NULL, hit, sizeof(hit))`（或等效单串改写）。
3. 修复后建议补两道防回归：`grep -rEn '[A-Za-z_]delivery/'` 粘连形并入残留清扫；三受损脚本纳入 V3 自检或至少实跑冒烟。修复以 amend/新 commit 落地后重派 landing-verifier 复核（仅核修复面）。
4. landing-summary §3-4 补记本批漏网损伤（诚实注记增补），retro 收录「正则替换须带成员访问边界守卫 + 固定签名 C 函数禁按插段实现」教训。

**merge 门：不放行。** 结构与文档面保真优秀（137 锚全落、历史面零损、契约/敏感面逐字合规），唯 T2 批改面存在 4 处会使 G3 门与 host 验收编译即刻失败的逻辑损伤，属「批定改法落错」而非「漏落」或「夹带」——修复面小且机械，修完复验即可转绿。

---

## 7. 复核轮记录（2026-09-15，append-only，不改写上轮 FAIL 结论）

> 判定：**PASS（GREEN）——放行 merge（merge 仍归 human manual-merge 门）。**
> 判定人：landing-verifier（同一角色、独立复核，只读 codebase、只写本件复核节）。修复基线：landing commit `bd81f57` → 修复 commit `1857dd3`（原基线 `3dda7bd`）。核验方式：`git diff bd81f57..1857dd3` 逐文件人读 + 三脚本亲自复跑 + 防回归扫描亲自复跑 + 结构门亲自复跑。

### 7.1 修复面逐文件裁断（diff 实证）

| 文件 | 声明修复 | 实证裁断 |
|---|---|---|
| `.claude/scripts/closeout-check.mjs` | 恢复原属性名 :317/:332-333 | ✓ 恰 3 行、全部复原（`taskPackage.deliveryMilestoneStateChanged` / `planningWorkstreamRequired` / `planningRefs ‖ planningRef`），无其它改动 |
| `.claude/scripts/taskpackage-required-check.mjs` | 恢复原属性名 :198-199/:236-243 | ✓ 恰 6 行、9 处粘连全复原（`deliveryImpact`/`planningImpact`/`planningRefs`），无其它改动 |
| `.claude/scripts/arch-review-handoff-check.mjs` | 恢复原属性名 :117-118 | ✓ 恰 2 行、4 处粘连全复原（`finding.deliveryImpact`/`finding.planningRecommendation`） |
| `firmware/main/test/device_ws_switch.acceptance.c` | 8 参改回 7 参单串 | ✓ 恰 1 行：`wsa_scan_tree_both("delivery/changes", "shim.py", "archiv", 1, NULL, hit, sizeof(hit))`——与上轮 §6-2 指定形态逐字一致 |
| `delivery/taskpackages/260911-render-pages/taskpackage.json` | canonical 回写对齐 current | ✓ 恰为路径指针改写（upstreamRef/planRef/planningRefs/docRefs/sourceRefs/crossEnd.sourceRefs/notes/mutationSkipReason 的旧前缀 → `delivery/` 新前缀），语义字段零变；与 landing 对 current 的批改同构 |
| `architect-landing-summary.md` | 补记 §3-11/12 | ✓ 恰 2 行，内容与实际修复相符（含「pre-landing 字节一致、失配 100% landing 造成」的诚实归因） |
| `retro.md` | 新建收口 | ✓ 新件 62 行；§3-F2-④ 把成员访问粘连形列为第 8 种路径形态、§3-F2b 记 canonical 活面误判、§4-2 固化建议收录形态族谱表——上轮 §6-4 两条教训均落档 |
| `landing-verdict.md` | 本件（上轮 verdict） | ✓ 上轮 FAIL 结论原文保留，本节 append |

**合计 mjs 粘连修复 17 处 + C 件 1 处 = 18 处，与声明计数吻合。** 上轮 §6 三条件全部核销：

1. **§6-1（mjs 粘连损伤修复）**：✓。增强实证——三脚本 `git diff 3dda7bd..1857dd3` 的属性访问行净零差（对 pre-landing 原版逐属性复原，非仅修掉粘连串）。
2. **§6-2（C 件 7 参形）**：✓，形态逐字符合。
3. **§6-3（防回归扫描 + 冒烟 + 复核重派）**：✓。核验官亲自复跑：①粘连形扩展扫描 `[A-Za-z0-9_})\]]delivery/(milestones|workstreams|taskpackages|changes)` 全树（含代码面、排除证据制品）**零命中**；②`closeout-check.mjs` 实跑 `ok:true` exit 0；③`taskpackage-required-check.mjs` 实跑 `ok:true` exit 0（读 `delivery/taskpackages/current/taskpackage.json`，changeId 260911-render-pages）；④`arch-review-handoff-check.mjs` 实跑到达 usage 业务判定（exit 1 = 缺目录参数的正常 usage 出口，非 ReferenceError）。
4. **§6-4（summary 补记 + retro 收录）**：✓（§3-11/12 + retro F2-④/F2b/§4-2）。

### 7.2 canonical↔current 复核

`git diff --no-index delivery/taskpackages/260911-render-pages/taskpackage.json delivery/taskpackages/current/taskpackage.json` → **exit 0，字节一致**。required-check 门恢复正常判定基础。定性：canonical 在途包属活面（census 起草期误归历史面的覆盖缺口），修复 = 补完 T2 批定改法本身（同构指针平移），非 scope creep；已在 landing-summary §3-12 + retro F2b 如实留痕。

### 7.3 结构门复跑（含基线口径注记）

- 首两跑 `ok:false` 均为**调用口径问题、非 landing 缺陷**：①不给 baseRef 时脚本默认对 `HEAD~1..HEAD`（= `bd81f57..1857dd3`，只见修复轮改动，全 manifest 误报 claimed-but-not-landed）；②`baseRef=3dda7bd` 键值形不被解析（baseRef 取位置参数 argv[3]，键值串被当 ref 名）。
- 正确调用 `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260914-gov-review-delivery-restructure 3dda7bd`（位置 baseRef 覆盖 bd81f57+1857dd3 两 commit 全部已提交改动）：**`ok:true`，frozen:true，packetBinding:true**，unauthorized/claimedNotLanded/plannedNotLanded 全空，exit 0。两 commit 跨越不影响结论（合并视角 = 相对 3dda7bd 的全量 diff）。

### 7.4 终态

上轮 4 红项 + canonical 对齐缺口 + 制品补记全部修复且经独立复证实证；修复面恰为声明面、无新私货；结构门 ok:true。**终态 PASS，放行 merge（Track B 自动合并前置闸由主线程跑，merge 决定权归流程既定归属）。**

- 结构门：`ok:true`（baseRef=3dda7bd 位置参数，exit 0）
- 冒烟：closeout-check `ok:true` / taskpackage-required-check `ok:true` / arch-review-handoff-check 到 usage 判定（均核验官亲跑）
- 防回归扫描：粘连形全树零命中（核验官亲跑）
- canonical↔current：字节一致（`git diff --no-index` exit 0）
- 破坏性自检：修复轮 diff 仅触上轮红项文件 + 两个制品件 + 本件，零越界
