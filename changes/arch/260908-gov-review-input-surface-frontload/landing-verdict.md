# landing-verdict · 260908-gov-review-input-surface-frontload

> 独立核验者 = `landing-verifier`（Track B 第 5 步；只读 canonical，只写本件）。核验时点 = 2026-09-09，工作树未 commit 态。
> 判据 = 确定性脚本退出码 + 逐文件人读 diff + 本机独立复跑，**不采信 landing-summary 自述**。

## Verdict：**GREEN — 可交合并门**（无 RED 项；三条非阻塞更正与七条残余风险在册）

- 结构门 `landing-fidelity-check.mjs` → `ok:true`（frozen / packetBinding 真，unauthorized / claimedNotLanded / plannedNotLanded 皆空，gitDegraded=false）。
- 设计侧门 `proposal-consolidation-check.mjs` = **N/A**：本批 Track B，证据集仅 PROPOSAL + r1 + r4 + landing-summary，无 `decision-packet.md`（已 `ls` 亲核）；批定一致性由 §7 R4 GREEN 记录 + Status 行 FROZEN 承载。
- 11 个 manifest 路径逐一人读 diff 对表 T1…T7：**改法全部落地、无漏改、无私货、无与批定矛盾**。承重替换句逐字命中，敏感面（G1 成文 + 承重闸 + 四 def frontmatter）实证零越界。

## 1. 结构门判读

| 字段 | 值 | 判读 |
|---|---|---|
| `frozen` | true | Status 行 FROZEN v1.0 + 授权链三件在册 ✓ |
| `packetBinding` | true | PROPOSAL 与 summary 的 landing-manifest 11 行同源 ✓ |
| `unauthorized` / `claimedNotLanded` / `plannedNotLanded` | 全空 | 11/11 双向闭合 ✓ |
| `gitDegraded` | false | 未降级，证据全量可得 |
| `citationAnchorAdvisory` | 21 查 14 报 `file-not-exist` | **主线程归因成立**：逐条实证 14 件全部是 PROPOSAL 内短式引用（`builder.md:24`、`dev-loop-operating-system.md:122`、`scenario-author.md:31`、`pod-coordination.md:130`、`260829-story-p3/pod-retro.md:11` 等）相对仓根解析不到，实文件均在 `.claude/agents/`、`.claude/references/`、`changes/dev/**` 下存在且被引内容对得上（逐条读行原文，见 §5 附录）。**是工具解析局限，不是落地缺陷** |
| `advisory.changedCanonicalNotInProposal` | `.claude/skills/show-me/SKILL.md` | 非本批产物：untracked、mtime 2026-09-08T23:47、文件自述「humanlayer/skills 迁入，human 直接指示的 one-shot，未经 Track B」，且 `git ls-files` 空（从未入版本控制）→ 与 summary §3 的「会话前既存」申报一致 ✓ |
| `lintDescriptionsAdvisory` | `landing-verifier.md` over-length (363>320) | 该文件不在本批 manifest、工作树未改 → 既存状况，不属本批 |

## 2. 逐文件人读对照（11 行）

| 文件 | 结构被动 | 语义对表 | expected（FROZEN packet） | actual（diff 实证） |
|---|---|---|---|---|
| `docs/DEV-GOVERNANCE.md` | ✓ | **ok** | T1：头注末句替换 + 第 6 条三要素 | diff 仅 1 改 1 增。**替换句 byte 级 = 「（跨 4 包 5 子项实证）。六条前置清单：」**（python `in` 断言 True）；① 一次产双份 + 不转抄正文 ✓；② 钉死措辞「**超读是常态动作，不申报才是问题**」逐字 ✓；③ Draft 骨架必在 + finalize 二选一 + `≥12 字符` + 「跨端性是设计判断不是可推导属性」+ 两案实证（story-p3:11 / g-relay-fix `display_relay`）✓。全文件 `五条前置清单` 计数 0 ✓ |
| `.claude/scripts/brief-inputs.mjs`（新） | ✓ 474 行 | **ok** | T2 四路 + 单源常量 | ①只出路径/锚点/节标题/节首行、**无正文抽取**（裸路径标「无锚点（整文件）」+ ADV，定位不到标 not-found 不猜）✓；② `resolveSymbolRange` brace 平衡 + 病态封顶 400 行 + `generated-at`，失败 fail-loud exit 1 **不写盘**（两分支本机实证见 §4）✓；③三判定源全部写进注释、`current` 显式排除、**全文件零处读 board/pods**（grep `pods|git-common-dir|devloop` = none）✓；④空表模板 ✓。`WIRE_SECTIONS = ['§1.5','§三','§四']` 顶部常量 + 契约改号同步维护注释 + §1.2/§1.3/§二 待评清单 + `export` ✓；**`isWireRef` 确绑定文件名**（R2-F1 改正点独立探针：`interface-contract.md#§四`=true、`feature-inventory.md#§1.5`=false、裸路径=false、`§二`=false）✓ |
| `.claude/scripts/taskpackage-required-check.mjs` | ✓ | **ok** | T3 三条向严、零新门 | 纯追加（唯一既存行改动 = import 加 `dirname,join`）；**既有必填字段语义零动**（实证：真包 `260907-g-relay-fix` 拷为 fixture，红项只落新三条、`missing(other)=[]`；补齐后 exit 0）。① 骨架必在（空数组算在场）✓ ② finalize 二选一 + `CROSS_END_EXEMPT_MIN_CHARS=12`（同闸 `:296-297` `landsIn=dropped reason.trim().length<12` 实测同地板，非拍数）✓ ③ 两 brief 同目录必在、红消息内嵌 Step 7 生成命令 ✓。**`verify-gates.json` / `closeout-check.mjs` 零改动实证**（`git diff HEAD --name-only` 空），且 `closeout-check.mjs:309` 确以 `resolve(...taskpackage-required-check.mjs)` + `pathToTaskPackage` 程序化调用 → 「单源红利自动传导」为真、非声称 |
| `.claude/agents/planner.md` | ✓ | **ok** | T4 planner 五改点 | 新步 `1d` 跨端判断（含「依据不是 subsystem 写没写两面」+ g-relay-fix 反例 + 向严填 counterparts）✓；Step 7 前置一次产双份（明写「builder 那份不得留到 Step 9」）+ 派发 prompt 只引路径不转抄 ✓；Step 8 硬前置两条 ✓；Step 9 **只引用**、重跑事由落 `regenerated-because` ✓；`:110`（原 `:109`，因插入行位移）引用行五→六并点名第 ⑥ 条 ✓ |
| `.claude/agents/portfolio-planner.md` | ✓ | **ok** | T4 + R2-F2(i) | `:58` 六条 + 点名 `crossEnd` 骨架与 `input-brief` 同源最小句形 ✓ |
| `.claude/agents/acceptance-author.md` | ✓ | **ok** | T4（含 R1/F7 消歧） | `:24` Inputs 封闭清单 + `manifest-missing` ✓；Step 1 改指 brief + **对端解析器必读**（含「按本侧契约想象」反面教训）✓；Step 7 回传申报表 + 指出 planner 据此落 over-scan ✓；`:47` 引用行 = 「起包清单**六**条中的②③」+ 追加第 ⑥ 条 → **F7 互斥已消歧、V5③ 可绿** ✓ |
| `.claude/agents/builder.md` | ✓ | **ok** | T4 | `:24` 封闭清单 + `manifest-missing` + 申报条 ✓；`:47` 追加第六条（原句不含「五条」，追加而非替换，与 packet 实况注记一致）✓ |
| `.claude/references/task-intake-and-slicing.md` | ✓ | **ok** | T5 | `crossEnd` 字段级定义段（形状 / Draft 骨架 / finalize 二选一 / **`≥12` 地板显式投影起草侧**（R2-F5）/ 向严 / 本批覆盖档 + ②③档留 ARQ / advisory WARN 行为 / 消费方）✓ 且列入 Lite 不可省清单并注明动机包本身即 Lite ✓ |
| `.claude/references/dev-loop.md` | ✓ | **ok** | T6 | §2 表新增 `brief-inputs.mjs` 行 ✓；taskpackages 内容节新增 `input-brief-*` 条：生成时点 = Step 7 前置双份 / Step 8 核在场 / Step 9 只引用、证据属性、**快照语义同文重申**、非权威只出指针 ✓ |
| `.claude/references/dev-loop-operating-system.md` | ✓ | **ok** | T7 第一条 | `:122` 枚举扩 `over-scan`（OQ1=(a)）+ **生产者触发定义钉同一条目**（交付报告超读段非空 → planner 落账、summary 引行数与代表文件）+ 与 `scan-miss` 反向语义 + **「纯 advisory、绝不当门」（DP-D4）原句未破** ✓ |
| `.claude/scripts/pod-events-aggregate.mjs` | ✓ | **ok** | T7 第二条 | 只增 `actors` 键（本机实测顶层键 = ok/changeId/eventsTotal/malformedLines/tokens/**actors**/escalations/friction，既有四块完好）；per-actor `dispatches`/`withDurationMs`/`durationMsMissing`（缺值如实计数不补 0）/合计/中位/toolUses 中位/长尾 `LONGTAIL_MIN_MS=40min` 单常量 + eventId·phase 明细、按合计倒序 ✓ |

**敏感面逐字核结论**：G1 指令根（`CLAUDE.md`/`AGENTS.md`）零改动；`architect.md` 与全部 review-loop agent def 零改动（`.claude/agents/` 只有 manifest 的四件被改）；**四 def 的 frontmatter 逐字节相同**（`git show HEAD:` 与盘上前 12 行 CR 归一后 md5 全等，且 diff 的 `@@` 起点全部 ≥ 行 24，diff 内无 `tools:`/`name:`/`model:`/`description:` 行增删）→ D-2 landing 纪律（动 frontmatter 即触自改递归须升级）**实证零触**。`docs/contracts/**`、`scope-map.json`、`*.settings.json`、`verify-gates.json`、`hooks/**` 全部 `git diff HEAD --name-only` 为空。新脚本写面唯一一处 `writeFileSync`，落 `.claude/taskpackages/<id>/`，且 scope-map planner/portfolio allow 早含 `.claude/taskpackages/**`（`:52/:56`）→ **无新写槽、无权限扩张**。

## 3. §1b 六条补位与偏差逐条裁定

| # | 申报内容 | 裁定 | 依据 |
|---|---|---|---|
| 1a | `planner.md` TaskPackage 格式示例块加 `"crossEnd": {…}` 一行 | **属冻结 packet 的必然投影，接受** | 该示例块是仓内起草侧唯一模板源（既存 `mutationEvidence` 骨架就在块内），T3.1 + T1⑥ 要求 Draft 骨架必在、V3② 更把「照抄 planner.md 格式示例」写为验证规约；不补则 planner 起草看不到形状 → 闸红反推，正是第 6 条要消灭的形态。不改语义、不新增文件/门/权限 |
| 1b | `planner.md` `[JOURNAL-APPEND]` 枚举加 `over-scan` + SSOT 指针 + 生产者义务 | **必然投影，接受** | T7 明写生产者 = planner；def 该行仍列四型会让 over-scan 永无产出者（= R1/F14 判的「死 canon」在 def 面复现）。机械面实测不破：`journal-append.mjs:99/162` 对 `--friction-type` 取自由串无闭集、`aggregateFriction` 按 `byType[ft]` 动态归桶 → 新值可落账可聚合。且补位选择「本行只投影不另立」并指回 SSOT，**未造第二闭集** |
| 1c | `dev-loop.md` §2 表 `pod-events-aggregate` 行补 T7 新列说明 | **接受（文件地图如实性修订）** | T6 的宿主文件 + T7 的实际改动交集；不改则文件地图对新键失真。同 manifest 文件内、零新设计 |
| 2 | T4「Step 1 intake 加跨端判断」落为新步 `1d` | **形式差异，接受** | Step 1 已有 ①②子块与 1b/1c 平行步；`1d` 内容逐要素齐（判断问题式、判据不是 subsystem、反例包、向严指引、Step 7 前须定）。语义与 packet 一致 |
| 3 | 新增 `--regenerated-because` 可选 flag | **接受，未越界** | R2-F6 要求事由「落制品自身」，实现只可能在生成器侧；未扩 manifest、未加第二事实源。**注**：无机械力强制 planner 重跑时传该 flag（纪律级），与 R4 残余观察 (2) 同面，留 retro |
| 4 | merged 判定源实测有三种 verdict 落点（三取、全缺判未 merged） | **接受（fail-closed 容错）** | 与 packet「verdict=APPROVE」不冲突，缺即判未 merged = 少收先例、不误扩阅读面。**但见 §5 残余风险 (a)：该判定源在本仓实况下严重少报** |
| 5 | 过渡态：landing 后裸跑承重闸 RED 3 项 | **申报属实** | 本机裸跑实证 exit 1，红项恰 = `crossEnd` + 两份 brief，`current/` → `260829-schedule-reminder`；补救命令内嵌在红消息里。无在飞 pod → 无实际阻塞。不设过渡豁免与 packet 一致 |
| 6 | 未产出任何真实 brief 文件 | **实证属实** | `git status --short .claude/taskpackages/` 空、目录数仍 20（19 包 + current）→ 包数 census 未被污染（V3① 规约执行到位） |

## 4. 独立复跑结果（自报 vs 我实测，逐条）

| 验证 | 主线程自报 | 我实测 | 一致性 |
|---|---|---|---|
| **V1** | 19/19 exit 0 | `--self-check` → 19 行全 PASS、`19/19 passed`、`SELF-CHECK GREEN`、exit 0 | ✓ |
| **V2** | dry-run 出两锚点节行 + ④ 模板 + wire WARN + 3 裸路径 ADV；包目录 git 清洁 | exit 0；stdout 含 `§1.5`→节首行 **138**、`§三`→**205**、④ 空表；stderr 恰 1 条 WARN + **3** 条 ADV；`git status --short .claude/taskpackages/260907-g-relay-fix/` 空 | ✓ 全项吻合 |
| **V3** | 负①缺 crossEnd / 负②exempt 1 字 红；正态 exit 0；fixture 仓外、用后清理 | 负① exit 1 点名 `crossEnd(骨架必在…)` + 两 brief；exempt 1 字 exit 1（`>= 12 chars (got 1)`）；**我加测 11 字亦红、12 字即绿**（地板精确 = 12，非 ≥13 亦非 >12）；正态（counterparts 含 `firmware/main/mcp_server.cc#McpServer` + 两份 brief）exit 0 且 `ok:true` | ✓ 并加宽边界 |
| **V4** | `actors` 键在；acceptance-author toolUses 中位 18.5；builder 69.3min/109tools 长尾 1；三既存键不破 | 顶层键 `tokens/actors/escalations/friction` 齐；acceptance-author `toolUsesMedian=18.5`、原始 toolUses `[113,8,18,19]` 与 packet V4 锚一致；builder `69.3min/109` LT=`20241a2c`；threshold=40；**另用独立 python 重算三 actor 合计/中位/长尾全部吻合** | ✓ |
| **V5** | 六臂全绿 | ①1/0 ②1/1 ③0 ④4/4 ⑤1 ⑥2 —— 六臂逐数命中 | ✓ |
| **V6** | 交日落复盘，不当本门 | 不跑（与 packet「非 landing 门」一致） | ✓ 处置正确 |

**不符点 1 处（自报误差，非落地缺陷）**：summary §1 称 `brief-inputs.mjs` 「约 340 行」，实测 **474 行**。功能面逐项对表无差异，但该数字为不实自述 → 请更正（见 §6 更正清单 C1）。

## 5. 破坏性自检（mutation 对偶，全部仓外 temp，用毕已清理并断言不存在）

| 探针 | 目的 | 结果 |
|---|---|---|
| `crossEnd.counterparts = [{path:"server/nope.py", symbol:"Ghost"}]` 真跑 brief-inputs | 证伪「闸看似有效实则静默产空锚」 | **真 fail-loud**：exit 1、stderr 点名 `counterpart-unresolved: server/nope.py#Ghost (path-not-found)`、**stdout 0 字节、盘上不产出 brief** ✓ |
| 既存文件 + 幽灵 symbol（`resolveSymbolRange` 直探针） | 自检查仅覆盖 path-not-found 分支，补测另一分支 | `{error:"symbol-not-found", totalLines:28}` ✓ 两分支皆活；正例（真实 def / `McpServer`）解析出区间，`McpServer` 命中 :61-62 构造器 + 9 条 extraHits 备查 |
| `exempt` 长度 1 / 11 / 12 三态跑承重闸 | 证伪「豁免地板是摆设」 | 1 字红、**11 字红（`got 11`）**、12 字绿 → 地板真实且边界精确 |
| 临时 projectDir 造 `current/`（含 merge 事件 + 非空 acceptanceTestsFile）与 `260101-decoy`（同构） | 证伪「`current/` 排除臂是空断言」 | 产出先例表**只含 decoy、不含 current** → 扫描是活的（decoy 命中即正控制）、排除是真判据 ✓ |
| 越界档 counterpart（`ESP32-S3-RLCD-4.2-Demo/**`） | 证伪「②③档留根被实现成硬红」 | exit 0 + 一条 ADV 点名未覆盖档并指引改填 exempt ✓ 与 packet「advisory 不硬红」一致 |
| `journal-append` / `aggregateFriction` 闭集检查 | 证伪「over-scan 落账即被机械面拒」 | 无闭集校验（自由串 / 动态归桶）→ 新枚举可落账可聚合，机械面未破 ✓ |

**未发现任何「落地声称有牙、实为空转」的项。** 唯一被证出的空转在**测试臂**而非产品码：self-check 的「②③档越界提示（不静默不硬红）」断言含一个恒真合取 `off.advisories.length + advisoriesGlobal.length >= 0`，故该臂只验了「不硬红」半边，「提示确实发出」未受断言（我手工验为真）。→ 记为残余风险 (c)。

## 6. 残余风险与留根建议（不阻塞合并）

- **(a) 先例路③在本仓实况下严重少报 merged 包（建议另登 ARQ，本批不算失真）**。`isMerged()` 实测 19 包只判 4 包 merged，而 git log 显示至少 8 个包是经 merge-driver/`merge(devloop)` 真实合并的（`260829-schedule-p2 / home-p1 / session-logic / host-test-infra / voice-roundtrip`、`260903-device-ws-switch / gateway-directionB`、`260907-g-relay-fix`）。根因确定：这些包 `supervisor-verdict.json` 的 verdict = **ESCALATE**（人批 ESCALATE 合并是本仓主形态），而合并批准在 pod-events 里的 eventType 是 **`human-decision`**（`payload.decision = "approve-escalated-merge"`），既非 APPROVE、eventType 也不含 "merge"，两源皆失。后果 = brief 第③路给出的是陈旧先例（对 `g-relay-fix` 实测只回 260829 三包，260903/260907 的更新先例被漏）。**方向是 fail-closed（少给先例 = 少读，不放松任何门），且判定源文本本身是冻结 packet 钉的（「verdict=APPROVE ∨ merge 类事件」），故不判 landing 失真**；但「按时间倒序取最新先例」这一设计目的实质未达。ARQ 方向：merged 判据加 `human-decision ∧ decision~approve-escalated-merge` 分支，或改读 `changes/dev/<id>/`（含晋升后事件），并配 self-check 反例 fixture。
- **(b) `exempt` 地板的适用面比 packet 措辞略严**：packet/T5 措辞为「**用作不适用分支时** ≥12」，实现则在 finalize 态只要 `exempt` 非空即验长度（`counterparts` 已非空时给个 `"—"` 也红）。属向严、不 loosening，**接受**；建议 T5 文下次修订时把措辞对齐实现（或实现放宽为仅在 exempt 分支生效），避免读者以为「填了 counterparts 就能乱填 exempt」。
- **(c) self-check 的越界档臂含恒真合取**（§5 末），「advisory 真发出」未受断言 → 下批补 `off.advisories.some(含未覆盖档)` 即可。同类小松动：`isWireRef` 用 `anchor.startsWith(w)`，`#§1.5abc` 亦判 true（advisory-only，向宽仅多提示不误红）。
- **(d) 冻结 PROPOSAL 的行锚在 landing 后位移**（`planner.md:109→110`、`acceptance-author.md:46→47`、`builder.md:46→47`、`dev-loop.md:229→231`、闸 `:292→:296-297`）：PROPOSAL 系 append-only 冻结件、**不应回改**（packet 自明「落地后不回改本文件」），但引这些锚的后续件须知道锚已漂。留根：建议 retro 记一句「承重件引用优先用可 grep 的短语锚而非行号」。
- **(e) 过渡态在册**（§3 #5）：`current/` 包裸跑闸 RED 3 项，补救已内嵌红消息；**合并前若新起 pod 即撞此项**，非缺陷但是执行注意。
- **(f) R4 残余观察 (2)（Step 9 重跑触发词未含「crossEnd 迟填」分支）+ 本批 V6 pilot 回看** → 均已声明交日落复盘，本门不裁。
- **(g) `pre-merge-escalation-gate.mjs` 裸 `GREEN` 机锚向宽**、**消费方②③档**、**dispatch-stamp 补全** 三项 packet §6 已裁另登 ARQ，本批 manifest 零触该脚本（实证 `git diff HEAD --name-only` 空）→ 处置恰当，无 scope creep。

### 更正清单（建议合并前顺手改，不改亦不阻塞）

- **C1** `architect-landing-summary.md` §1 `brief-inputs.mjs` 行「约 340 行」→ 474 行。
- **C2** 同 §1 `taskpackage-required-check.mjs` 行引 `verify-gates.json:32-34` / `closeout-check.mjs:309`：实测闸条目在 `:31-36`、`closeout-check.mjs:309` 命中 `bearingGatePath` 行 ✓（后者精确）→ 前者可注「含 timing 行」。
- **C3** 建议把 §5 残余风险 (a)（merged 判定源对本仓主合并形态失效）从「残余」升为**必须另登的 ARQ**，并在下个包生成 brief 时目视核先例新鲜度。

## 7. 结论

结构门绿、设计侧门 Track B N/A、11/11 语义保真、敏感面（G1 成文 + 承重闸 + 四 def frontmatter + 契约/强制层）实证零越界、packet-binding 成立（packet-id = §2 T1–T7 + §5，landing 范围 ⊆ packet，唯一超出者为 §1b 已申报并裁定为必然投影的三处补位）、五条 landing 门（V1–V5）独立复跑逐数吻合、六项破坏性自检无一空转。

**建议：放行至合并门（Track B 自动合并本地 main 之前置升级闸照常执行）；merge 归 human / 机械闸，非本 verdict 权限。** 上述 (a) 项建议在本批 retro 或紧随的 ARQ 登记中消化，不留白。
