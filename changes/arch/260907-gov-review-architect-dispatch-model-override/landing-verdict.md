# landing-verdict · 260907-gov-review-architect-dispatch-model-override

**verdict：APPROVE** —— 结构门 GREEN（F1 修复后本 verifier 亲跑复证，非采信转述）；manifest 三路径落地内容与 FROZEN PROPOSAL §2 逐字节一致，V1–V4 亲跑全绿，无 scope creep、无私货、无漏改。可 commit 分支、进 retro + 合并闸。V5（在线探针）维持「新会话补跑」裁定不变（§5），补跑红即打回。

## 0. F1（已修复闭合）——历史记录

首轮核验时结构门 RED，ErrorEnvelope：

```json
{
  "ok": false,
  "failingFile": "changes/arch/260907-gov-review-architect-dispatch-model-override/PROPOSAL.md",
  "expected": "§2 每个 T 条目含行首裸「目标文件：」锚行（解析规约 .claude/scripts/lib/governance-gate-shared.mjs:368 /^目标文件\\s*[:：]/，B7 FROZEN 2026-08-25）",
  "actual": "T1/T2 锚行写作粗体「**目标文件**：」（首轮 PROPOSAL.md:201、:217），行首为 `**` 不匹配解析正则 → 结构门报「缺锚清单」两条、exit 1",
  "category": "structural-gate-red（PROPOSAL 锚行格式与机械门解析规约不兼容；非漏落、非改法偏离）"
}
```

根因：起草时用粗体 markdown 强调「目标文件」标签，R1 判官人工核对锚语义在场但机械门 landing 期才首次可跑，粗体形态与 B7 行首正则不兼容在此暴露。

**修复核验（本 verifier 亲证，2026-09-07）**：主线程去粗体为行首裸 `目标文件：`（现行 :201/:218）+ 各补一行格式修订注记（:202/:219，自述「语义零改」属实）——本 verifier 逐行核对：锚路径与首轮完全一致（零语义漂移）、hook 围栏与落地文件仍逐字节相等、FROZEN 状态行与 manifest 围栏未动、修订注记内出现的 `**目标文件**：` 系引述文字非锚行。结构门复跑 GREEN（§9）。

## 1. 结构门（首轮 RED 原始输出，历史记录）

首轮 `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-architect-dispatch-model-override` → exit 1：

```json
{
  "ok": false,
  "gate": "landing-fidelity",
  "message": "governance landing 保真度核验未过",
  "errors": [
    "PROPOSAL §2 锚清单：缺锚清单：T1 · 新增 G3 hook：agent-model-guard（architect 派发 model 参数 deny） 无「目标文件：」行（fail-loud，不假绿）",
    "PROPOSAL §2 锚清单：缺锚清单：T2 · planner.md MODEL-TIER 段补边界句（G2 消歧） 无「目标文件：」行（fail-loud，不假绿）"
  ],
  "gitDegraded": false
}
```

两条 error 同根因（F1）；无 scope creep / claimed-but-not-landed / planned-but-not-landed / 未冻结 / 缺 packet-binding 类 error。git 未降级。修复后复跑 GREEN 见 §9。

## 2. 逐文件人读 diff（manifest 三路径，全部 semanticMatch: ok）

| 文件 | structuralTouched | semanticMatch | expected（FROZEN §2） | actual（diff/全文实读） |
|---|---|---|---|---|
| `.claude/hooks/agent-model-guard.mjs`（新增） | 是（untracked 新文件） | **ok（逐字节相等）** | §2 T1 围栏源码（含 R1-F3 完备性依赖注释两行） | 机械抽取 PROPOSAL ```js 围栏（4154 字节）与落地文件逐字节比对 `byteEqual=true`，零差异；F1 修复后复验仍 `byteEqual=true` |
| `.claude/settings.json`（改） | 是 | **ok（归一化逐字节相等）** | ① PreToolUse 第三 matcher 块 `Agent\|Task` 钉死 JSON、位置在 `Edit\|Write\|MultiEdit` 块之后；② `_doc` 末行（原 :8）补尾逗号（R1-F2）+ 追加登记行逐字 | diff 恰两处：① `_doc` 原末行补尾逗号 + 追加登记行（与围栏归一化后逐字节相等）；② PreToolUse 追加 `Agent\|Task` matcher 块（归一化逐字节相等），落位在 `Edit\|Write\|MultiEdit` 块后；diff 无其它任何改动 |
| `.claude/agents/planner.md`（改） | 是 | **ok（行内容逐字相等）** | §2 T2「替换后完整句」在 :57 MODEL-TIER 段末追加、行内其余不变、10b/:71 与 frontmatter 不动 | diff 恰一行（:57）：追加句与 PROPOSAL「替换后完整句」逐字相等（唯一差异 = 行首 3 空格列表缩进，diff 证实旧行同缩进，为 landing 前既有格式）；10b 行与 PROPOSAL 引据逐字相等（未漂移）；diff hunk 上下文含 frontmatter `model: opus` 行未动 |

**破坏性自检**：`git status --porcelain -uall` 全集 = manifest 三路径 + `changes/INDEX.md` + `docs/ARCH-REVIEW-QUEUE.md` + 本 review 证据目录制品。canonical 面无第四处改动、无意外删改。INDEX/ARQ 两件见 ADVISORY-2。

## 3. 验证集亲跑（V1–V4 重跑，不采信 summary 转述）

- **V1 hook 自检**：`node .claude/hooks/agent-model-guard.mjs --self-check` → `{"selfCheck":"agent-model-guard","pass":15,"fail":0,"total":15,"ok":true}`，exit 0。真空绿规约满足（自检名在场 + 全量计数先行断言非空）。
- **V2 探针三形态**：architect+opus → exit 2，stderr 逐字含 `blocked by agent-model-guard: architect 派发携带 model="opus" — planner.md:71 ARCH-ADVISORY 档 FROZEN 条款…`；architect 无 model → exit 0；builder+opus → exit 0。全部与 §5 V2 预期一致。
- **V2+ 逃生门（附加亲跑）**：env `KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1` 下 architect+opus → 放行 exit 0，stderr `agent-model-guard: KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1 — human 一次性豁免生效，本次放行。`，与 OQ2 (a) 裁断形态一致。
- **V3 G2 回归**：`route-regression-check.mjs` → `route-regression-check: 28 ok, 0 fail`，exit 0；`lint-descriptions.mjs` → exit 0，flagged 1 = `.claude/agents/landing-verifier.md` 超长（363>320）——**系既有告警**（该文件最后改动于 commit ed63d0c，本批 diff 零触碰），非 T2 新增，「零新告警」预期成立（ADVISORY-3）。
- **V4 settings 结构**：`JSON.parse` → `settings.json OK; PreToolUse matchers: ["Bash","Edit|Write|MultiEdit","Agent|Task"]`，matcher 块计数 = 3 且顺序与 PROPOSAL 钉死位次一致。
- **V6 landing 保真度**：修复后 GREEN，见 §9。
- **V5 在线探针：裁定延后**（见 §5）。

## 4. packet-binding 核对

- summary packet-id 行 = 「FROZEN PROPOSAL v1.1 §2 改法集（T1–T2）+ §5 验证集（V1–V6）· R1 GREEN」，与本 review id 一致。
- 结构门复跑 GREEN 证 `proposalTargets ≡ summaryTargets ≡ manifest 三路径`、`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]`。
- landing 实际范围 = manifest 三路径并集 = PROPOSAL §2 目标文件并集。
- 无私货：三文件 diff 内容均限于 PROPOSAL §2 钉死文本；无权限/写域扩张夹带（hook 判据收窄于「architect + model 非空」，settings 仅加 hook 注册与 `_doc` 登记，planner.md 仅一句追加）。

## 5. V5 在线触发探针裁定：延后至新会话补跑（不假绿、不因延后打回）

裁定依据：① PreToolUse hook 注册表在会话启动时快照，本会话启动时该 hook 未注册，本会话内派发是否被拦不可靠（未拦 ≠ 门失效）；② 本 verifier 为子 agent，自身无 Agent/Task 派发工具，无法代跑。V1–V4 + 结构门 + 人读 diff 为本批承重，V5 依 PROPOSAL §5 自身条款在新会话闭合。

**补跑指令（新会话，主线程执行）**：
1. 派一次 Agent 调用 `subagent_type=architect` + `model: "opus"` → **预期** PreToolUse deny，stderr 含 `agent-model-guard:`（证明 matcher=Agent 在 2.1.263 实际触发，闭合硬项 1a 未实测项）。
2. 随后同任务**不带 model** 重派 → **预期** 正常起跑，子 agent meta.json 无 `model` 字段（证明不误伤合法形态）。
3. 探针红（带 model 未被拦）→ 打回本批 landing，不得以 V1–V4 绿抵充。

## 6. ADVISORY 清单（非阻断）

- **ADVISORY-1（行尾差异，非漂移）**：`.claude/settings.json` 工作副本为 CRLF，PROPOSAL 围栏为 LF；归一化后逐字节相等。hook 新文件为 LF 且与围栏严格逐字节相等。结构门复跑的 `verbatimQuoteAdvisory` 两条 `not-found-verbatim`（settings 注册块与 `_doc` 行）即此 CRLF 假失配——本 verifier 已做人读归一化比对逐字节相等背书，归 ARQ-260906-lfc-verbatim-crlf-citation-gap 已登记病灶，不人手复跑归因。
- **ADVISORY-2（manifest 外改动两件，均系本 change 的流程登记）**：`changes/INDEX.md`（upsert 本 change 行，状态 draft）与 `docs/ARCH-REVIEW-QUEUE.md`（新增 ARQ-260907-architect-dispatch-model-override 行，内容为评审前登记的队列文案；复跑结构门 `changedCanonicalNotInProposal` 点名的即此件）。二者是 AGENTS.md 治理路由的随批 bookkeeping、PROPOSAL 头部已声明「承载 ARQ」，非夹带私货，建议随分支一并 commit。其中 ARQ 行证据栏写「全程 46 条消息」，PROPOSAL §1.1 写「30 条 assistant 消息」——口径差异（transcript 总条数 vs assistant 类型计数），建议后续顺手对齐，不影响本批改法。
- **ADVISORY-3（既有 lint 告警）**：`lint-descriptions` flagged `.claude/agents/landing-verifier.md` 超长（363>320），最后改动于 ed63d0c，本批未触；留待后续 agent-def 治理批。
- **ADVISORY-4（流程观察，F1 后更新）**：F1 暴露「锚行格式机械门 landing 期才首跑」的时序缺口，且 260906 批已有一例同款（修订注记自述「同款病灶第二例」）——建议后续治理批把结构门预检（或锚行书写形态 lint）前移到 FROZEN 前机械可跑的位置，避免同类 RED 反复出现在 landing 期。

## 7. 亲核留痕（命令 + 关键输出）

- `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-architect-dispatch-model-override`：首轮 exit 1（§1）；修复后 exit 0（§9）。
- 逐字节比对（node stdin 脚本）：hook `byteEqual=true`（4154/4154，修复后复验仍相等）；settings 两围栏归一化后在场相等；planner MODEL-TIER 行与「替换后完整句」逐字相等（缩进为既有）；10b 行与引据逐字相等。
- V1/V2/V2+/V3/V4 输出见 §3。
- `git status --porcelain -uall` / `git diff` 三文件逐一实读（见 §2）。
- `git log --oneline -1 -- .claude/agents/landing-verifier.md` → ed63d0c（ADVISORY-3 既有性证据）。
- F1 修复核验：锚行 :201/:218 行首裸形态、修订注记 :202/:219 在场、FROZEN 行与 manifest 围栏未动（见 §0）。

## 8. 终局结论

**APPROVE**：F1 已修复并经本 verifier 亲跑复证（结构门 exit 0 GREEN + 修复仅限格式、改法实体零变更），manifest 三路径语义保真全绿，V1–V4 亲跑全绿，packet-binding 一致，无 scope creep、无私货。**放行 commit 分支、进 retro + 合并闸**（merge 仍归 human manual-merge 门）。V5 按 §5 指令在新会话补跑闭合，探针红即打回。

## 9. 结构门复跑记录（F1 修复后，本 verifier 亲跑，2026-09-07）

`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-architect-dispatch-model-override` → **exit 0，GREEN**。关键输出：

```json
{
  "ok": true,
  "gate": "landing-fidelity",
  "reviewDir": "changes/arch/260907-gov-review-architect-dispatch-model-override",
  "frozen": true,
  "packetBinding": true,
  "proposalTargets": [".claude/hooks/agent-model-guard.mjs", ".claude/settings.json", ".claude/agents/planner.md"],
  "summaryTargets": [".claude/hooks/agent-model-guard.mjs", ".claude/settings.json", ".claude/agents/planner.md"],
  "unauthorized": [],
  "claimedNotLanded": [],
  "plannedNotLanded": [],
  "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 1, "problems": [] },
  "verbatimQuoteAdvisory": { "checked": 3, "missing": ["PreToolUse 注册块（CRLF 假失配）", "_doc 登记行（CRLF 假失配）"] }
}
```

- `verbatimQuoteAdvisory.missing` 两条为 CRLF 工作树已知假失配（ADVISORY-1，ARQ-260906-lfc-verbatim-crlf-citation-gap）；本 verifier 归一化逐字节比对已独立背书内容相等。
- 附跑 `lintDescriptionsAdvisory` exit 0（仅 ADVISORY-3 既有告警），cross-surface 子门未触发失败。
- `changedCanonicalNotInProposal` = `docs/ARCH-REVIEW-QUEUE.md`（ADVISORY-2，流程登记）。
