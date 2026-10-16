# landing-verdict · 260918-gov-review-dispatch-chain-friction

**verdict：PASS + 1 ADVISORY**（落地-1）

- change-id：`260918-gov-review-dispatch-chain-friction`
- packet-id（核查对象）：FROZEN PROPOSAL v1.0 §2 改法集（T1 单条）+ §5 验证集
- manifest（2 路径）：`.claude/skills/kidpal-serial-monitor/SKILL.md`（T1 ①+②）、`.claude/references/pod-coordination.md`（T1 ③ 一行指针）
- review 轨：Track B（无 decision-packet，`proposal-consolidation-check.mjs` N/A）
- 核验者独立性：零上下文重启推导，不采信落地者 landing-summary 自述；判定来自 `git diff` 实证 + 脚本退出码。

---

## 一、逐文件语义核对（item 1）

manifest 两文件的实际 diff 对照 FROZEN §2 T1 的 ①②③。两文件 diff **均为纯新增**（`numstat`：SKILL.md `12/0`、pod-coordination.md `2/0`；`-` 行各 0）——既有内容**零删除零改写**。

| file | structuralTouched | semanticMatch | expected（来自 FROZEN §2） | actual（来自 diff） |
|---|---|---|---|---|
| `.claude/skills/kidpal-serial-monitor/SKILL.md` | ✓ | **ok** | ① 新增一节 `## Device round protocol / 真机轮协调协议`，三条（单一长捕获 + DTR/RTS 伪影模式识别判据 / human 就绪信号开窗 / 实时口令钉动作点）；② 既有 `## Rules that cost time when forgotten` 节补一行指向新节；不改既有三节内容、不改 frontmatter | `:32` 新增该双语标题节，`:36-41` 三条逐条在场（每条带 4 包 retro 实证）；`:23` 在既有 Rules 节末补一条指向新节；frontmatter（`:1-4`）零改动；既有三节标题仍在 |
| `.claude/references/pod-coordination.md` | ✓ | **ok** | ③ 真机轮相关处补**一行**引用指针（指向 skill 新节，不转抄正文，防双源漂移）；目的表述按 R1-F3 如实化（主通道 = skill description 触发，此处为次要发现路径） | `:198` 于 §11「demo 对账行」段后补一行（单 markdown 行）指针，指向 skill 新节；明写「主通道是它自身的 description 触发……此处指针是收口/对账读者的次要发现路径；不转抄正文」 |

### ① 新节三条逐条对上（重点核）

- **单一长捕获**：`一次开窗覆盖全窗口、中途绝不关端口` ✓ 逐字；DTR/RTS 伪影**判据** = `设备重启时刻与捕获启停时刻秒级吻合即为伪影、非真重启` ✓ 逐字；实证 `screen-unification 合并夜三次重启实测，21:15 / 22:50 两处可考` ✓（与 §2「三次重启、21:15 / 22:50 两处有实测记录」一致，R1-F4 计数更正已承）。
- **human 就绪信号开窗**：`窗口不由 agent 单方面开播，由 human 就绪信号开启` ✓；实证 `key-route-a1 第一次 300s 窗口开早了…重捕了一轮` ✓。
- **实时口令钉动作点**：`动作点不提前分发时间表，改实时口令` ✓；实证 `screen-unification 提前 15 分钟分发…改实时口令「现在按 BOOT！」一次成功` ✓。

三条均逐条对上，无缺项、无改写方向。

### ② 既有三节未被重写覆盖（重点核）

`grep -c '^## '` 实测：`## Tools (already on disk…)`=1 / `## Rules that cost time when forgotten`=1 / `## Reading a smoke round…`=1；diff `-` 行 = 0。既有三节**标题与内容均原样保留**。

### ③ 指针真未转抄正文 + 目的表述如实化（重点核）

指针**未复制三条规则的正文**（规则本体「全窗口覆盖 / 绝不关端口」「开窗前确认人已就位」「不提前分发时间表改实时口令」均未出现在 pod-coordination.md）。**目的表述如实化到位**：主通道 = skill 自身 description 触发、此处 = 收口/对账读者的次要发现路径，与 R1-F3 采纳口径逐点一致。**唯一边界**：指针含一短示例句「与捕获启停秒级吻合的『重启』是端口复位伪影、非真重启」，触及伪影规则本体一句 → 见 ADVISORY A1（非阻断，且为 R2 已背书的「判证据质量」立意所授权）。

**私货扫描**：两文件全部新增文本均可回溯至 §0/§1/§2 授权面（新节 intro 句「含真实硬件、真实人、不可重放…代价最高却最没有说明书」= §1 根因措辞；4 包 retro 实证清单 = §1 证据面）。**未发现超越 packet 的私货、未夹带权限/写域扩张**。

---

## 二、packet-binding 一致（item 2）：PASS

- summary `:3` packet-id 行 = 「FROZEN PROPOSAL v1.0 §2 改法集（T1）+ §5 验证集」 ↔ FROZEN §2 packet-id 字面行**逐字一致**。
- manifest（PROPOSAL 侧 `:13-16` / summary 侧 `:12-15`）两路径集合 = §2 目标文件锚清单并集 {`.claude/skills/kidpal-serial-monitor/SKILL.md`, `.claude/references/pod-coordination.md`} ✓。
- 结构门 V7 机判：`packetBinding: true`，`proposalTargets == summaryTargets`（2 路径并集）。

---

## 三、验证集复跑（item 3）：PASS（V1–V7 逐条与 summary §2 声称比对）

| 项 | 亲跑命令 | 实测结果 | 与 summary 声称 |
|---|---|---|---|
| V1 | `grep -n "Device round protocol"` + 三小条 `grep -c` | 新节标题命中（`:32`）；`单一长捕获`=1 / `就绪信号`=1 / `实时口令`=2 | 一致（summary 用锚定式 `^## Device round protocol` 得 1，等价，非差异） |
| V2 | `wc -l` + 三既有节标题各 `grep -c` | **43 行**（由 31 增至）；三既有节标题各 =1 | 一致 |
| V3 | `grep -c "kidpal-serial-monitor"` pod-coordination | **1**（指针在场）；目视核未转抄正文 | 一致 |
| V4 | `grep -c "DTR/RTS"` SKILL.md | **2**（`:10` 原有 1 + 新节 1） | 一致 |
| V5 | `node .claude/scripts/route-regression-check.mjs` + `lint-descriptions.mjs` | route `28 ok, 0 fail` exit 0；lint exit 0（仅既有 `landing-verifier.md` 超长告警） | 一致（frontmatter 未动 → 路由零差异，正面证据） |
| V6 | 见下节「V6 锚点校准核对」 | 字面锚得 1 commit（= 同族 P4，非本批夹带）；按本意核 = 本意满足 | 一致（summary §3 偏差 1 申报属实） |
| V7 | `node .claude/scripts/landing-fidelity-check.mjs <本批>` | see §四 | 一致 |

**V6 锚点校准核对（重点核 man·item 6）**：FROZEN §5 V6 字面锚 = `git log --oneline <FROZEN 时点 SHA>..HEAD -- .claude/scripts .claude/agents`，预期零提交。亲跑（FROZEN 时点 SHA = `80bd1b2`）得 **1 个 commit `95bc44b`**——经核该 commit = **同族 P4（`260918-gov-review-batch-drafting-friction`）的合法 landing**（`git show --stat 95bc44b` = `path-form-family.md` / `landing-fidelity-check.mjs` / PROPOSAL.template.md 等，manifest 本就含脚本面），**非本批夹带**。`git diff 80bd1b2..HEAD -- .claude/scripts .claude/agents` 仅 `landing-fidelity-check.mjs` 1 文件（P4 的）。本批**本意核**：工作树对 `.claude/scripts` / `.claude/agents` 两路径命中 **0**，本批待提交面恰为 manifest 两路径 ⇒ **本意满足**。summary §3 偏差 1 的申报与实际**完全一致**，其「V6 冻结措辞隐含『区间内无其它批次落地』前提、本族四批按序连续落地故前提不成立」的诊断**成立**。

---

## 四、结构门复跑（item 4）：PASS

`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260918-gov-review-dispatch-chain-friction` → **exit 0**，输出 `ok: true` / `frozen: true` / `packetBinding: true` / `unauthorized: []` / `claimedNotLanded: []` / `plannedNotLanded: []` / `gitDegraded: false`。

- `advisory.changedCanonicalNotInProposal` = `path-form-family.md` / `landing-fidelity-check.mjs` / `PROPOSAL.template.md` —— 经核**全部属 HEAD commit `95bc44b`（同族 P4 landing）**，`git diff HEAD~1..HEAD --stat` 与之逐条吻合；**本批工作树改动恰为 manifest 两路径**，故**非本批 scope creep**（门自身 note 亦提示可能为同工作树其它 landing）。
- `cross-surface-check.mjs`：**N/A**——本仓不存在该脚本（简报 §4 的模板遗留项）。
- `proposal-consolidation-check.mjs`：**N/A**——Track B 证据集无 `decision-packet.md`（已核目录实况），「frozen ↔ 批定一致性」由 §7 评审记录段 GREEN + Status 行 FROZEN 承载。

---

## 五、真空绿规约（item 5）：PASS

- **在场性 + 防覆盖双向断言**：V1/V2/V4 均含「新节在场」+「既有三节标题仍在 =1」双断言（非单向）。实测双向均成立，**非真空绿**。
- **重放类先断言输入非空**：V6 亲跑得非空输出（1 commit）后才解释结果，**未把空输入的绿当通过**——恰因非空（P4 落地在区间内）才触发锚点校准核对。
- **环境红基线**：本批无涉闸执行层的红，无需基线归因。

---

## 六、诚实披露核对（item 6）：PASS

summary §3 三条偏差逐条核：

1. **V6 锚点校准**——与实际一致（见 §三 V6 核对）；该记已记，诊断成立。✓
2. **SKILL.md 中英混排**——实测：既有内容为英文、新增节为中文、标题为 FROZEN 给定双语形态；符合仓规「输出标准」（散文用简体中文）。✓
3. **`实时口令` 计数 = 2**——实测 =2，V1 预期「各 ≥1」满足。✓

**有无该记未记**：未发现该记未记项。唯二非材料观察（`Related:` 页脚行因新节插入而顺移至文件末、V1 标题 `grep` 锚定与否计数差）均属格式级、不影响保真，不构成「该记未记」。

---

## 亲核留痕（命令 + 关键输出）

```
$ git status --short
 M .claude/references/pod-coordination.md
 M .claude/skills/kidpal-serial-monitor/SKILL.md          ← 本批工作树改动恰 = manifest 两路径
 M .codebase-memory/artifact.json / graph.db.zst          ← 机器再生面（AGENTS.md：不追交、不计数）
?? delivery/.../architect-landing-summary.md

$ git diff HEAD --numstat -- <两文件>
2	0	.claude/references/pod-coordination.md
12	0	.claude/skills/kidpal-serial-monitor/SKILL.md     ← 纯新增，- 行各 0

$ grep -c '^## (Tools…|Rules that cost time…|Reading a smoke round…)' SKILL.md → 各 =1
$ wc -l SKILL.md → 43（原 31）
$ grep -c "DTR/RTS" SKILL.md → 2 ; grep -c "kidpal-serial-monitor" pod-coordination.md → 1

$ node .claude/scripts/route-regression-check.mjs → 28 ok, 0 fail (exit 0)
$ node .claude/scripts/lint-descriptions.mjs → warn only landing-verifier.md (exit 0)
$ node .claude/scripts/landing-fidelity-check.mjs <本批> → ok:true (exit 0)
$ git log --oneline 80bd1b2..HEAD -- .claude/scripts .claude/agents → 95bc44b (P4 同族 landing，非本批夹带)
```

---

## ADVISORY（非阻断观察）

- **A1（轻）· pod-coordination 指针含一句触及规则本体的示例**：`:`198 指针含「与捕获启停秒级吻合的『重启』是端口复位伪影、非真重启」——该短句属伪影规则本体的结论片段，与 §2 T1③「不转抄正文（防双源漂移）」字面存在轻微张力。**判为合规、不阻断**：理由是（a）三条规则的**操作本体**（窗全开 / 不关端口 / 就位开窗 / 实时口令）**均未被复制**，双源漂移风险极低；（b）该示例正是 R2 为保留指针所背书的「让收口/对账读者判证据质量」立意的实现，属 §2 T1③ 已如实化的目的面。建议后续若撤并 skill 该节时，同步复核此示例以免实质漂移。
- **A2（提示性，非 finding）**：`## Device round protocol` 新节插于 `## Reading a smoke round` 节与文件末 `Related: [[…]]` 页脚行之间，页脚行顺移至文件末——格式级，语义无碍。

---

## 结论

**PASS + 1 ADVISORY(A1)**。两 manifest 文件实际 diff 与 FROZEN §2 T1 ①②③ 语义保真、纯新增无覆盖、无私货、无 scope creep；packet-binding 逐字一致；验证集 V1–V7 复跑与 summary 声称一致；结构门 V7 exit 0；诚实披露完整属实。**建议放行**——merge（本地 main）仍归 human / 前置升级闸，本 verdict 不含 commit/merge 动作。