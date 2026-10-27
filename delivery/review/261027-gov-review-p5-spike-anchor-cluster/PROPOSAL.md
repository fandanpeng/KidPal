# PROPOSAL · 261027-gov-review-p5-spike-anchor-cluster

> **状态：FROZEN v0.5 · 2026-10-27 · 主线程凭「human 立项 ack（2026-10-27，两条并批 Track B 主路径 + 同日立项追加扩面与 OQ2 丙拍板）+ R4 GREEN」标 FROZEN。评审循环 R1 BLOCK(F1) → R2 BLOCK(F2,F3) → R3 BLOCK(F4) → R4 GREEN（熔断于 R3 触发、human 拍换新判官实例再战 1 轮，R4 解除）；判官注意事项已吸收：全文 860 行（R4 记录插入后）、`_scratch/` 落地 commit 前删除。Track B 主路径；OQ1 已裁 (a)，OQ2 已裁丙。**
> **承载 ARQ**：`ARQ-261024-new-mechanism-spike-first`（MAJOR，T1，整条结项候选）、`ARQ-261017-anchor-check-master-unvalidated`（SHOULD，T2，整条结项候选）、`ARQ-260908-versioned-guard-repoint-execution`（SHOULD，T3，搭载纯记账结项）。三条同批清偿；不关的根与跨条边界见 §6。
> **review 轨**：**Track B 主路径**（R1 已裁定确认，本稿不重开）。T2 扩面后判档不变：新增面 = 设计面声明文件（human 2026-10-27 立项追加明确授权）+ 既有手动机检脚本的判据改造 + 既有纪律句改写；非跨系统结构性变更（不改固件 / shim / 契约 / spec 两文件），方向由 human 追加拍定收敛。Track A 段两条判据均不命中。v0.3 的 OQ2 丙落法（只在既有文件内加规则句与声明面语义节，零脚本、零门、零路径变更）不改变爆炸半径，判档与 riskTier 均维持。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**medium**（v0.1 的 low-medium 不再成立，如实上调）。理由：T2 已不是「判据加法」而是**现行设计稿解析来源的更换**——机检、出图、story 锚引用三处消费者从此以声明面为准，「现行稿 = 最高版号」这条隐式判据废止；同批新增一个设计面 canonical 文件（`docs/ui/UI-DESIGN-VERSIONS.md`）并做 v2–v7 存量沿革回填。失败模式与缓释：① **误判红**（声明面被误删 / 指针笔误）→ 全部 fail-closed 且报文点名具体路径或行数，落点在单一手动机检脚本、不挂 hook、不进任何闸链；② **升版忘改指针**（旧稿继续当现行稿）→ 不判红但 stderr 显式提示「存在更高版号，以声明为准」；真正的内容塌缩仍由完整性自查兜底拦；③ **两套编号族再漂**（brief 版号 vs 设计稿版号）→ 声明面与 brief 行明写两族错位关系。声明面与脚本改动**同批原子落地**（单 commit），不存在「新脚本 + 无声明面」的中间态。
> **review profile**：**G2+G3**（G2 = `.claude/agents/**` / `.claude/references/**` / `.claude/skills/**`；G3 = `.claude/scripts/**`）；另触**设计面** `docs/ui/**` 两件（新增声明面 + brief 版本行；human 2026-10-27 立项追加授权；OQ2 丙裁只在这两件与既有纪律文面内加规则句 / 语义节，不增件）、`docs/templates/PROPOSAL.template.md`（治理制品骨架）与 `docs/debt/gov/**` 三条目结项记账。**G1 零触碰、契约面零触碰、`hooks` / `settings` / `scope-map` / `verify-gates` 零触碰、无新增门、无门判定语义变化。**
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/planner.md
.claude/agents/uiux-author.md
.claude/references/change-triage.md
.claude/references/dev-loop.md
.claude/references/requirement-change.md
.claude/scripts/spec-anchor-check.mjs
.claude/skills/write-story/SKILL.md
docs/debt/gov/ARQ-260908-versioned-guard-repoint-execution.md
docs/debt/gov/ARQ-261017-anchor-check-master-unvalidated.md
docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md
docs/templates/PROPOSAL.template.md
docs/ui/UI-DESIGN-VERSIONS.md
docs/ui/UIUX-DESIGN-BRIEF.md
```

---

## 0. 想实现的效果与差距

**目标效果**（P5 批，human 2026-10-27 立项 ack「两条并批 Track B 主路径」+ 同日立项追加「UI 设计版本结构整体调整并入本批 T2 一起改」，追加记录在 `delivery/workstreams/gov-debt-reduction/task_plan.md` 决策记录）：

1. **新机制批「先 spike、再开发、再数据回灌」从人脑落到可执行文面**：`/triage-change` 判档新增「新机制批判定」必答问（命中判定 → 切片序先立 spike 包）；`dev-loop.md` 给出 spike 的产品定义（验收看消费端真实观测、按全量规模设计 + 首尾 canary、可观测打点为标配产物；数据迁移类四问 + Gate E 先例）；`planner.md` 增加 intake 复查步（正式实现 / 数据迁移包必须引用已闭合 spike 包的实证记录）；PROPOSAL 模板新增「新机制 spike 申报」硬项。**不新增机械 gate**（形态分叉归 OQ1，已裁 (a)）。
2. **UI 母版塌缩不再伪装成 story 锚漂移**：`spec-anchor-check.mjs` 的「现行设计稿」改为**读显式声明面**的 `current:` 指针（不再按「最高版号 wins」推断），并**保留完整性自查兜底**（现行稿相对前序版本 `data-od-id` 塌缩过半即判「疑似增量稿占用母版命名」、exit 2 中止并显式指认两版锚数与占比）；声明缺失 / 指针失效的失败行为明确定义为输入缺失（exit 2）。
3. **UI 设计稿的版本结构从「隐式约定 + 多处各记一份」变成「单一声明面 + 逐版沿革」**：在 `docs/ui/` 立 `UI-DESIGN-VERSIONS.md`（形态镜像 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`：声明面记当前 + 全量沿革，其它文面只留指针），机检、出图、story 锚引用、TaskPackage `designRefs` 一律以声明为准；v2–v7 逐版备注按 git 考据回填；`current:` 初设 = v7（依据见 §2.0 复证 4）。命名族纪律升级为「完整母版 + 升版与声明面同一次落盘」。
4. **`ARQ-260908-versioned-guard-repoint-execution` 按 human 逐件授权结项**：四件待重指项经复证确认已被后续批整体消化，条目翻转 closed + append 结项注记，不执行任何重指动作。
5. **截图图夹布局按 human OQ2 丙裁定成文**（2026-10-27 拍板丙，否 architect 原推荐的甲、亦否乙）：页图「单一活图夹 + 升版冻结快照 + 引用一律走声明面 current」的语义落进 `uiux-author` 出图落点句（T2.3-C1b）与声明面 `docs/ui/UI-DESIGN-VERSIONS.md`（T2.1 语义节）；frozen 快照机制本体、目录整形与存量 `V5` / `V6` / `V7` 迁移**另立需求面动作**（§6 留根 4），本批零搬图、PNG 增删不进 manifest。

**现状差距**（全部为本批起草期 2026-10-27 实跑或亲核，命令与输出见 §5 基线表）：

- 「新机制」在三处落点与 planner.md 全文**无任何字面**（`grep -c` 全 0）；K4 五层实证在案，但流程面没有任何一步要求先做端到端最小验证。
- `spec-anchor-check.mjs` 的「现行设计稿」判据只有一条：目录里版本号最大的 `kidpal-ui-design-vN.html`（函数体 `:37-43`，注释 `:36`），**不校验完整性**。本批构造的病态夹具（v1 有 12 个锚、v2 只剩 2 个且恰好覆盖两条 story 引用）在**现行脚本**下输出 `"ok": true`、exit 0——即不止「12 条假红」这一形态，连「锚恰好没断」的塌缩母版都会假绿。装配版同一夹具 exit 2 并指认母版缺口。
- **UI 设计稿的版本声明散落三处、三处互不一致**：story 锚行规则钉「当前 = v3」（`.claude/skills/write-story/SKILL.md:48`）、brief 头部写「完整母版仍为 v6」（`docs/ui/UIUX-DESIGN-BRIEF.md:5`）、机器按「最高版号」取到 v7——而实测 v7 是 v6 的严格超集（64 ⊃ 61 个 `data-od-id`，缺失 0）。三处各记一份、口径互斥，是「版本声明漂移」的实相（human 追加以「说 v3、实际 v7」概括的即是此症）。
- `.claude/skills/write-story/SKILL.md:48` 的锚行规则第 4 条仍写「当前 = kidpal-ui-design-v3.html」，实测现行稿已是 v7。
- `ARQ-260908` 条目 status = deferred；附录 A.3 在册 8 行中待重指 4 件（#3 / #6 / #7 / #8）；后续批已整体归档消化（commit `77b6b0cf`），live 验收套件对这些常量/探针名 grep 命中 0。

**非目标（本批不做，防 scope creep）**：

- **不新增或改动任何门**：不改 `hooks` / `scope-map` / `settings` / `verify-gates` / `pre-merge-escalation-gate.mjs`；spike 不做机械检查（OQ1 已裁 (a)）。
- **不改契约面与 spec 两文件**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零 diff。
- **不动截图图夹与 `designRefs` 形态**：`docs/ui/V5/`、`V6/`、`V7/` 逐文件零 diff。图夹布局 **OQ2 已裁丙**（2026-10-27，单一活目录 + 冻结快照）——本批只落规则句（`uiux-author` 出图落点句）与声明面语义小节，**不搬图、不新增图夹、不落 frozen 快照机制本体**；目录整形、快照机制与存量 `V5` / `V6` / `V7` 迁移另立需求面动作（§6 留根 4）。
- **不批量重出图**：`tools/asset-pipeline/**` 零 diff（`verify_art_assets.mjs:14` 硬编码 v3 绝对路径与 `PHASE1-DESIGN-INPUT.md` 的陈旧「最新母版 = v4」声明只留根，见 §6）。
- **不合并 `ARQ-260918-spec-face-3page-reconcile`**（tech 域，另批）：本批只写互查咬合与边界（§6 留根 6）。
- **不吸收 `ARQ-261019-opendesign-shoot-script-precedent`**（母版写者分工 / 出图管道属它）：本批只钉命名族、声明面与落盘原子性（§6 留根 5）。
- **不执行 ARQ-260908 的任何重指动作**，`server/**` 零 diff；不追溯改历史 PROPOSAL / verdict / retro。

## 2. 问题、根因与改法（T1…T3）

### 2.0 复证结论（起草期 2026-10-27；ARQ 行是待验证假设）

> 纪律：每条结论对应实跑命令或亲读文件；ARQ 原文的路径 / 行号 / 机制描述按「搬运引据视同新断言复证」逐条核过，漂移处如实更正。

**复证 1 · `ARQ-261024` —— 三落点确实未被消化，证据链完整；勘误三条全部可核。**

- **落点在场性与干净度**：`grep -c "spike"` 在 `change-triage.md` / `dev-loop.md` / `PROPOSAL.template.md` / `planner.md` 全为 **0**；`grep -c "消费端"` 在 `change-triage.md` 亦为 0。条目 paths 三条全部在场、无插桩冲突。
- **证据链文件逐件在场**：`delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-attribution-fix-review-20261024.md`、同目录 `k4-recall-architecture-20261024.md`（Gate E 在 `:174-180`）、`delivery/taskpackages/261024-m5-k4b-corpus-parser-v2/`（含 `review-packet.md` 与 `verifier-evidence/` 外的收尾制品，目录实测存在）。
- **关键数字复核（文件台账）**：`1026/1683` 的口径说明在 `k4-attribution-fix-review-20261024.md:23`（1683 = corpus 1652 + transcript owner 31；1026 = 525 条直接 `Assistant:` 行 + 501 条嵌在 `User:` 行里的 `Assistant:` 文本）；`1187` 行主键冲突静默跳过在 `:30`（distinct seq 只有 120 个）；截断面在 `:51`（56 条 `output_tokens=2500`、27 条 `output_tokens=8000`）。**条目正文「27% 单元静默失败」的统计口径本批未复算，按原文引用、不追认。**
- **勘误①②③复核**：`k4-recall-architecture-20261024.md:13` 载观察者缺位（`agent-kidpal` 的 `observe_others` = null，`(observer=agent-kidpal, observed=owner)` 结论 0 条）与被动窗口 `WORKING_REPRESENTATION_MAX_OBSERVATIONS=100`；`:14` 载「重灌仍答不出」同构实测；`:55-62` 载成员配置实证。勘误③「单文件试灌假通过」与 `:14` 的窗口论证一致。
- **结论**：治理机制缺口成立，且本批推荐的落点（triage 判档 + dev-loop 定义 + template 申报）与条目建议 (a)(b) 及模板位一一对应；(c) Gate 链补位的具体断言形态按勘误②引用 Gate E 作先例，不在本批机械化为 gate（OQ1 已裁 (a)）。

**复证 2 · `ARQ-261017` —— 判据缺口成立；本批复现出比原条目更尖锐的形态（塌缩母版在「锚恰好都在」时也判绿）。**

- **机读落点（行号亲核）**：注释在 `:36`、函数 `currentDesignFile` 在 `:37`、函数体 `:37-43`（`grep -n "function currentDesignFile"` 实测 = 37；`return files[0];` / `}` 在 `:42-43`）。条目引 `:35-41` 是条目开具时的旧行号，内容与现行文件逐字一致、仅行号整体下移。
- **病态复现（比原形态更强）**：临时夹具 v1（12 个 `data-od-id`）→ v2（2 个 `data-od-id`，恰好覆盖 requirements 里两条 story 的引用）。**现行脚本**：`{"design":"kidpal-ui-design-v2.html","designIdCount":2,…,"errors":[],"ok":true}`、exit 0；**装配版**：`FATAL（母版完整性）` 报文、exit 2。原条目的 12 条假红形态只是「塌缩且锚也断了」，本批证明「塌缩但锚恰好没断」同样假绿——归因缺口比原条目记录的更宽。
- **阈值标定（真实演进史实测）**：v2→v3（0 变）、v3→v4（掉 6/61 = **9.8%**，含 `canvas-p4`，属合法页撤）、v4→v5（0）、v5→v6（+1）、v6→v7（+3 严格超集）。⇒ 任何「前序 id 缺一个即报」的硬规则都会误伤 v3→v4 这类合法页撤；0.5 阈值与真实分布的关系如实记：**合法页撤的最大实测收缩 9.8%，病态夹具 83%；阈值到两者的距离分别是 5.1 倍与 1.7 倍，中间没有观测样本**——阈值落在空档里靠的是保守取值，不是拟合（R1-N1 指出的「数量级」修辞已在 v0.2 收紧；判据本身成立）。
- **现行稿健康**：v7 有 64 个唯一 `data-od-id`，是 v6（61）的严格超集，缺失 0。装配版在真仓输出与改前逐字一致（§5 B1/B17）。
- **「读 brief 声明清单」判据被否决的实证**：`UIUX-DESIGN-BRIEF.md` 头部写「本批增量渲染见 kidpal-ui-design-v7.html，完整母版仍为 kidpal-ui-design-v6.html」，而实测 v7 ⊃ v6 —— 声明本身已漂，拿它当完整性真值会把健康仓库判红。**注意区分**：该句漂的是「设计稿版本声明」，不是「brief 自身版本」；brief 头部 `版本：v6` 是 brief 自己的版本号（另见复证 4 的两族错位）。本批不采用 brief 作完整性真值，但把 brief 的设计稿版本声明**改指声明面**（T2.3-C）。
- **消费方 census**：`spec-anchor-check` 的消费方共 6 处文档（`requirement-reviewer.md:21/:32`、`story-author.md:19/:34`、`commands/requirement-change.md:15/:21`、`references/requirement-change.md:28/:35`、`skills/write-story/SKILL.md:49-50`、`skills/arch-doc-restructure/SKILL.md:64-78`），**零代码 import**（脚本无导出、无 `import` 消费方）。消费方式均按 exit code / `ok` 字段判绿；新增失败模式带显式归因（exit 2 + 点名），不破坏消费面形状。
- **纪律侧两处漂移实证**：`write-story` 锚行规则第 4 条钉死 v3（`:48`）；brief 头部钉死 v6 为完整母版（`:5`）。

**复证 3 · `ARQ-260908` —— 四件重指义务已被后续批整体消化，纯记账结项成立。**

- **条目与权威清单**：条目 status = deferred；`delivery/archive/changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md` 附录 A.3（`:288` 起）在册 8 行，其中「待重指」4 件 = #3 schedule_p2 P2-A6 接线（`test_schedule_p2.acceptance.py:584-587`）/ #6 VRT live 探针 A4/A7（`test_voice_roundtrip_b.acceptance.py:509-512`）/ #7 CHR-A8/A9 live 探针（`test_char_stroke.acceptance.py:683-685`、`:722-724`）/ #8 `PROMPT_CARRIER_V2` 常量（`test_schedule_p2.acceptance.py:144`）。
- **消化事实**：`ARQ-260916-server-mvp-teeth-sweep` 闭账 commit `77b6b0cf`（2026-09-16）把 MVP / 验证期 server 面冻结验收 10 件整体归档——三份被引文件实测在 `server/acceptance/archive/`；`server/pytest.ini:1-6` 明写 `norecursedirs = … archive`（归档件不进裸收集；显式传参是 pytest 既定例外，闸侧由 `unit-run-check.mjs` 枚举跳过承载）。
- **零残留复核**：`server/acceptance/`（排除 archive）对 `PROMPT_CARRIER_V2` / `P2-A6` / `CHR-A8` / `CHR-A9` / `VRT-A4` / `VRT-A7` grep 命中 **0**；三个 archive 文件内的命中分别 77 / 24 / 33 行（同一 alternation 集合计数）。
- **继任覆盖在场**：`server/acceptance/test_prompt_v7_round_routing.acceptance.py` 按版本演进逐版钉（`PN-A1..A5` / `RF-A1..A5` / `PV-A1..A8`：落盘结构断言、制品首行无版本前言、前版 immutable 负锚、三面接线一致性），live 探针一律显式声明为「live 观测项——非自动断言」（`:36` / `:75` / `:115`）。
- **授权记录**：human 2026-10-27 逐件四问四拍「确认结项，不再重指」——该记录由主线程 AskUserQuestion 承载并随本批任务书携带；architect 无法独立复核对话本身，如实按携带记录引用（§5 不列为可机检断言）。
- **结论**：对已归档文件做版本无关重指无意义（归档树 append-only），结项 = 纯记账。

**复证 4 · UI 设计稿版本族谱与「实际在用」核查（human 2026-10-27 立项追加的现状基线）。**

- **逐版实测（`data-od-id` 计数与相邻集合差，脚本逐版比对）**：未版本化 `kidpal-ui-design.html` = 19（页级粗锚）；v2 = 61；v3 = 61（id 集 = v2）；v4 = 60（相对 v3 撤 6：`canvas-p4` / `p4-notes` / `pose-sleep` / `shot-p4` / `state-sleepy` / `swap-demo`；增 5：`char-notes` / `char-window` / `overlay-rules` / `p3-delete-confirm` / `shot-char`）；v5 = 60（id 集 = v4，纯版式重构）；v6 = 61（+1 = `p3-body-unavailable`）；v7 = 64（+3 = `p3-recording-guide` / `p3-recording-guide-spec` / `state-organizing`；**v6 的严格超集、缺失 0**）。
- **升版批考据（`git log --follow` 逐版）**：v2 ← `770c3009`（2026-08-29，吉祥物重绘 Hello Kitty 风 + P4 米字格改楷体）；v3 ← `54280819`（2026-09-09，V3 定稿：human 三观感裁定 + 吉祥物蝴蝶结，U 包 ART-E4 前置解锁）；v4 ← `4587bf62`（2026-09-14，对齐 requirements v1.51 + 锚指针批改）；v5 ← `ec0804bd`…`101629e4`（2026-09-22 → 2026-10-12，ui-v5 系列，要点四项自 `31d364d5`（2026-09-23）起，收拢于 `261011-p1-home-v5-rework`）；v6 ← `e6253317`（2026-10-17，批 `261016-p3-story-body-sd-cache`，requirements v1.64）；v7 ← `356a9e25`（2026-10-19，批 `261019-p3-record-dialogue-guide`，requirements v1.65）+ `05cf4910`（2026-10-21 后补账，两帧与 WF-F2-02 注记回同步）。
- **「实际在用」证据（不拍脑袋）**：① `261019-p3-record-dialogue-guide` 的 fixer brief（`delivery/taskpackages/261019-p3-record-dialogue-guide/current/fixer-brief-r1.md:46`）以「`docs/ui/kidpal-ui-design-v7.html` P3 节三帧 + `docs/ui/V7/` 三帧 PNG」为改法权威；② 该批 intake 决策记录载「仓内自有脚本 `tools/asset-pipeline/shoot_design.mjs` 直接吃 v7 设计 HTML，已执行 15 画布全帧出图，本批三帧捡进 `docs/ui/V7/`（对齐 261016 先例每批只落变更帧）」；③ `261020-p3-rec-frame-visual` 真机观感终判后回同步 v7；④ 机检改前基线（最高版号判据）输出 `design = kidpal-ui-design-v7.html`。四条同向 ⇒ `current` 初设 = **v7**。
- **版本声明漂移清单（本批 census）**：① `write-story/SKILL.md:48` 钉 v3（最老）；② `UIUX-DESIGN-BRIEF.md:5` 写「完整母版仍为 v6」（半新，且与 v7 严格超集关系矛盾）；③ 机器按最高版号取 v7（隐式，无声明）——三处互斥；④ 两族编号错位：brief 版本 `v5` ↔ 设计 `v6`（同在 `261016` 批）、brief `v6` ↔ 设计 `v7`（同在 `261019` 批），因设计 HTML 在自己的版式批也升、brief 只在需求批升；⑤ `docs/ui/PHASE1-DESIGN-INPUT.md:5` / `:128` 仍称「v4 已落地 = 最新母版」；⑥ `tools/asset-pipeline/verify_art_assets.mjs:14` 硬编码绝对路径 `C:/AI/KidPal/docs/ui/kidpal-ui-design-v3.html`。⑤⑥ 不在本批改（§6 留根）。
- **结论**：治本不是「再钉一个新版本号」（钉哪都漂），而是把「当前是哪个文件」从**隐式推断 + 多处复述**改成**单一声明面 + 显式指针**；完整性自查随之退为兜底（判据仍在，不再是唯一防线）。这也直接回答「截图图夹增量生成」的牵连——图夹布局归 OQ2（**现已裁丙**，见 §4）；本批只把语义成文，图夹本身与 `designRefs` 形态仍不动。

### T1 · 新机制批 spike-first：判档必答 + 相位定义 + intake 复查 + 模板申报（ARQ-261024）

**问题**：凡「首次引入架构中的新子系统 / 新外部依赖集成 / 新数据迁移」的批，现有流程没有任何一步要求先做端到端最小验证：判档只看切片与风险，启动只看 TaskPackage 完备，Gate 链只验生产者侧。结果是开发与数据回灌在前、消费端（召回注入 / 用户可见行为）验证完全缺位，缺口只能在灌完、真机两轮答不出之后才被发现。影响面 = 每一个新机制批；代价形状 = K4 五层问题各吃一轮返工、两批回滚重灌、两次画像毒化、human 全天注意力（§2.0 复证 1）。

**证据**：见 §2.0 复证 1（三落点零字面；K4 证据链逐件在场与关键数字；勘误①观察者缺位 + 被动窗口、勘误②Gate E、勘误③单文件试灌假通过）。

**根因**：三处流程面各缺一块——① 判档清单没有「新机制」维度：`/triage-change` 判五档，只看切片规模、riskTier 与排除清单，不问「消费端链路验证过没有」；② dev-loop 没有把 spike 定义成产品：谁执行、验收看什么、产物落哪里都没成文，「先做一轮小验证」只活在人脑与该批的临时决定里（K4 里它是在三轮返工之后才被 human 提出来的）；③ 模板没有申报位：治理 / 架构批若引入新机制，没有任何机制要求它交代 spike 状态。三者叠加 = 纪律无入口、无定义、无申报位。

**改法**（五件，四文件，零门；插入文本逐字节如下，落地照抄）：

1. **`change-triage.md` §2 新增「新机制批判定」小节**（插入位置：五档表之后、「**小 feature 与小 bug 合流为同一档（②单包）**：」段（`:32`）之前）：

```markdown
### 新机制批判定（spike-first 前置问 · ARQ-261024，261027 批立）

判档前先答一问：**本变更是否首次引入架构中的新子系统、新外部依赖集成或新数据迁移？** 信号举例——接入从未接过的外部服务、引入新的存储 / 消息 / 检索组件、把存量数据灌进从未消费过它的链路、新建一条端到端数据通路。既有依赖的常规新用法、纯内部重构不命中；**存疑向严按命中走**。

- **命中**：判档 rationale 必含「spike 前置」一句——本变更必须先有一个**端到端最小验证 spike**（定义、验收锚与产物落点见 `dev-loop.md`「新机制批 spike 相位」节；原则原话 = human 2026-10-24「其实应该是得先做一轮小验证，然后再做开发，再做数据的回灌」）。spike 独立成包（② 单包，Lite / Full 按触面定；**不判 ⓪ nano**——spike 的意图天然需要新行为验收语义承载，命中 nano 排除清单）。reviewed plan 的切片序把 spike 包写在正式实现与数据迁移之前，并写明其**消费端验收观测**（不是生产者自证）。
- **未命中**：rationale 写一句「非新机制批」及理由即可，不写长论证。
- **性质**：本问是判档纪律、非机械 gate（与 §9 同性质 convention-only）。漏答的兜底 = 人类触点① 与 planner intake 复查（`planner.md` Step 1i）。
```

2. **`dev-loop.md` 新增「新机制批 spike 相位」节**（插入位置：`### 变更制品（Change Artifact）`（`:119`）之前）：

```markdown
### 新机制批 spike 相位（spike-first · ARQ-261024，261027 批立）

> 原则原话（human 2026-10-24）：「其实应该是得先做一轮小验证，然后再做开发，再做数据的回灌」。来源 = K4 记忆回灌三轮返工：引入「记忆画像 + 历史回灌」这一新机制时，消费端（召回注入）验证完全缺位，五层问题各吃一轮返工、一层比一层晚发现（证据链与逐层代价见 `docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md`）。

**触发**：首次引入架构中的新子系统 / 新外部依赖集成 / 新数据迁移。判档落点见 `change-triage.md`「新机制批判定」节；planner intake 复查见 `planner.md` Step 1i。

**spike 定义（钉死三条，防形式化空转）**：spike = 在**真实链路**上、用最小数据量、从生产端跑到**消费端**各一次的可执行验证。判定 spine 三条：

1. **消费端真实观测，非生产者自证**——验收只看最终消费方看到了什么（用户可见行为 / 消费端读回内容 / diag 打点）。写入成功、读回成功、统计口径正确，都不能替代消费端观测。K4 实证：数据（归属 / 日期 / 内容）全对，仍因放错召回视野而全量返工。
2. **按全量规模设计、按最小数据执行**——最小数据 ≠ 最小规模：单文件 / 单条试灌会**假通过**（K4 实证：新结论必落在被动窗口内，单文件试灌掩盖窗口挤出症状）。spike 必须按全量规模设计观测位（窗口 / 分页 / 位序 / 容量），并用**首尾 canary**（最早与最新各一条可检索标记）确认两端都可达。
3. **消费端可观测打点是标配产物**——产物不是「跑过了」的结论，而是让下一次观测能发生的打点与记录（K4 实证：human 坚持「先加日志再测」后，一条 diag 日志即时定位五层里最贵的一层）。

**数据迁移类 spike 的四问**（三层 Gate 只验生产者侧的对偶补位；具体断言形态可复刻 `delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-recall-architecture-20261024.md` §C 的 Gate E：E1 成员与观察视角配置断言 / E2 派生 + canary / E3 消费端同款请求 + 窗口位移模型 / E4 用户可见行为 / E5 通道健康）：① 消费端**谁看得到**这份数据（成员与观察视角配置）；② 数据在消费端的**检索位序 / 窗口**里是否还活着（旧数据会不会被更晚的内容挤出）；③ 消费端**读到的内容**是不是这一份（canary）；④ **用户可见行为**是否随之改变。

**产物与落点**：spike 实证记录落 `delivery/taskpackages/<spike-change-id>/spike-evidence.md`（形态见「变更制品」表）。spike 包照常走自己的验收与合并门——**不因是小验证而豁免任何闸**；正式实现 / 数据迁移包的 intake 引用它（`planner.md` Step 1i）。

**不做什么**：spike 不替代正式验收、不替代全量回归、不构成对消费端行为的最终验收。它只回答一个问题：这条端到端链路今天是否真的通。
```

3. **`dev-loop.md` 变更制品表新增一条**（插入位置：`dispatch-progress-<agent>.md` 条目（`:136`）之后，与既有条目同为 `- ` 列表项）：

```markdown
- `spike-evidence.md`（261027 批 T1，ARQ-261024）：新机制批 spike 相位的消费端实证记录——打点 / 日志片段 + 观测结论 + 首尾 canary 结果（spike 定义与验收锚见「新机制批 spike 相位」节）。**惯例非门**：不入 closeout 必交清单、不进任何闸；消费者 = 正式实现包的 intake（`planner.md` Step 1i）与 review-packet 的未覆盖面申报（写清「本批依赖的端到端链路已验证到什么程度」）。
```

4. **`planner.md` 新增 Step 1i**（插入位置：Step 1h（`:59-65`）之后、`2. 对任何疑似契约变更先派 …`（`:66`）之前）：

```markdown
1i. **新机制批 spike 前置核（intake 纪律步、零新门；ARQ-261024，261027 批 T1）**：intake 时按 `change-triage.md`「新机制批判定」节答一问——**本包是否首次引入新子系统 / 新外部依赖集成 / 新数据迁移？**（判断不了按命中处理，存疑向严。）
  - **本包就是 spike 包**（reviewed plan / plan.md 声明）：跳过本核；验收以**消费端真实观测**承载（定义见 `dev-loop.md`「新机制批 spike 相位」节），产物 `spike-evidence.md` 随包落盘。
  - **本包是命中判定的正式实现 / 数据迁移包**：`sourceRefs[]` / `docRefs[]` 必须引用**已闭合的 spike 包**（其 `spike-evidence.md` 路径或 changeId）。spike 未闭合，或证据只到生产者侧（写入成功 / 读回成功）→ **停**，不派 builder：回 triage 先立 spike 包，或升 human 裁「为什么本包可以不做 spike」（人裁须留痕）。
  - **未命中**：在 taskpackage `notes` 记一句「非新机制批」及理由，不写长论证。
```

5. **`PROPOSAL.template.md` §2 新增硬项**（插入位置：「**【硬项 · 目标文件锚清单】**」段结束后、`**条目形态（五段固定式，照此书写）**：`（`:48`）之前）：

```markdown
**【硬项 · 新机制 spike 申报】**（261027-gov-review-p5-spike-anchor-cluster T1 · ARQ-261024）本批若**首次引入架构中的新子系统 / 新外部依赖集成 / 新数据迁移**（判定见 [change-triage.md](../../.claude/references/change-triage.md)「新机制批判定」节），§2 必须写明 spike 相位结论之一：① 已在先导批完成（给 `spike-evidence.md` 路径或先导 changeId）；② 在本批切片序最前（写清消费端验收观测与首尾 canary 设计）；③ 本批不做（给理由与升 human 的裁断出处——不做不默认）。不命中者写一句「本批不引入新机制」即可。**缺席 = 判官 BLOCK 的合法理由。**
```

6. **`ARQ-261024` 条目结项记账**（frontmatter 三字段翻转 + 正文末尾 append）：

   frontmatter：`status: deferred` → `status: closed`；`closed: ""` → `closed: 2026-10-27`；`closedBy: ""` → `closedBy: 261027-gov-review-p5-spike-anchor-cluster`。

   正文 append（插在文末「…spike 验收必须按**全量规模 + 首尾 canary + 用户可见行为**设计。」之后，空一行）：

```markdown
**2026-10-27 · 结项（`261027-gov-review-p5-spike-anchor-cluster`，T1）**：原则落三处文面 + 一处模板硬项——① `change-triage.md` §2 增「新机制批判定（spike-first 前置问）」：命中判定的变更必须在判档 rationale 写 spike 前置一句、切片序把 spike 包放到正式实现与数据迁移之前；② `dev-loop.md` 增「新机制批 spike 相位」节：spike 定义钉死三条（**消费端真实观测、非生产者自证** / **按全量规模设计、按最小数据执行** + 首尾 canary / **消费端可观测打点为标配产物**），数据迁移类给出四问并引用 Gate E（E1–E5）作断言形态先例，勘误①的「观察者缺位 + 被动窗口」与勘误③的「单文件试灌假通过」均写入正文；③ `planner.md` 增 Step 1i：正式实现 / 数据迁移包 intake 必须引用已闭合 spike 包的 `spike-evidence.md`，spike 未闭合或只到生产者侧一律停；④ `PROPOSAL.template.md` §2 增「新机制 spike 申报」硬项（三选一 + 缺席即判官 BLOCK）。**未落机械 gate**（本批路线 = 文面纪律，判定见本批 PROPOSAL §4 OQ1）；本条原条目三条 paths 之外另落 `planner.md`——无 intake 消费方的纪律等于没有执行面（理由见 PROPOSAL §2 T1）。留根：spike 规则暂无机械强制，观察期后再裁。
```

**为什么这样改**：对三个根因一一对应——① 判档必答把「新机制」变成判档时无法回避的问题（命中的 rationale 不写 spike 前置就是判档不完整）；② dev-loop 把 spike 定义成有产物、有验收锚的产品（消费端真实观测 / 全量规模 + 首尾 canary / 可观测打点三条钉死，正是防「跑个假验证就算数」的形式化空转）；③ 模板硬项让治理 / 架构批也无处回避。**为何加 `planner.md` Step 1i（超出 ARQ 原三条 paths）**：三处文面没有运行时消费方——planner 才是每个 dev 包 intake 的必经点，没有 Step 1i，「spike 未闭合不得进正式实现」只是一句没人执行的建议；这一步只读 plan / refs 并落一句 notes，零新门、零脚本（§4 取舍 C 给出退路）。**为何不落机械 gate**：见 OQ1——K4 缺的是一个相位而不是一道检查，且机械 gate 需新 schema 字段与脚本改动（G3），按 ARQ 预告须升 full-adversarial + human。

目标文件：`.claude/references/change-triage.md`（改） · `.claude/references/dev-loop.md`（改） · `.claude/agents/planner.md`（改） · `docs/templates/PROPOSAL.template.md`（改） · `docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md`（改）

### T2 · UI 设计稿版本声明面：显式 current 指针 + 指针读法改造 + 完整性自查兜底 + 纪律文面升级 + 存量迁移（ARQ-261017 + human 2026-10-27 立项追加 + OQ2 丙裁成文）

**问题**：`spec-anchor-check.mjs` 的「现行设计稿」= `docs/ui/` 下版本号最大的 `kidpal-ui-design-vN.html`，**只有这一条判据**。任何人写一份增量 vN（Write 单次体积上限使增量写入成为自然产物）就静默成为全仓 UI 锚的比对源：整份母版塌缩时，机检要么产出成片假红（原条目实测 12 条 / 10 条 story），要么在引用恰好都还在时**直接判绿**（复证 2 复现，更隐蔽）。两种形态的共同病灶是**归因误导**：失败信息只说「你的锚没了」，不说「母版不完整」。而「当前是哪一版」这件事在仓里**没有任何单一来源**——story 锚行规则钉 v3、brief 头部钉 v6、机器按最高版号取 v7，三处互斥（复证 4 漂移清单 ①②③）。

**证据**：见 §2.0 复证 2（判据定义 `:36-43`；病态夹具 before/after；真实演进史 9.8% vs 83%；brief 声明漂移；write-story v3 指针；消费方 census）与复证 4（逐版 id 计数与集合差；升版批考据；「实际在用」四条证据；六处漂移清单）。

**根因**（四条，逐条对应改法）：

1. **判据是隐式的**：「现行设计稿」由文件系统里的最高版号推断，而版本号是写者自报的——一个文件叫什么名字，决定了它是否成为全仓比对源。
2. **没有声明面**：版本演进史只活在 git log 与散文里（brief 头部、intake 记录、carrier 式声明各自为政），机检无法读、成文处各记一份、必漂。
3. **归因面缺口**：错误文本只描述锚缺失这一后果，不指认母版塌缩这一原因。
4. **命名族无纪律**：增量 / 局部稿可以合法占用母版命名，且顺序上没有「升版号必须伴随内容完整 + 声明面同步」的约定。

**改法**（五组：T2.1 声明面新文件 · T2.2 脚本六处编辑 · T2.3 纪律文面四处 · T2.4 存量迁移 · T2.5 结项记账。**原 v0.1 的编辑 A–E 里 A 原样保留、B/C 扩写、D+E 合并重写；原实施件 6/7（纪律面）被新方案取代并改写**）：

#### T2.1 新增声明面 `docs/ui/UI-DESIGN-VERSIONS.md`（新文件，31 行正文）

形态镜像 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 的先例（**该文件第 3–8 行**声明了同一套结构：声明面记当前 + 全量沿革，渲染面 / 散文只留「当前 = vN」指针、不复述沿革）。本仓命名取语义名 `UI-DESIGN-VERSIONS.md`（备选 `DESIGN-VERSION.md` / 镜像式 `KIDPAL_UI_DESIGN_CARRIER`；取前者因仓内 docs 文档族一律语义命名、全大写 CARRIER 是 deploy 面特例），全文逐字节如下：

```markdown
# KidPal UI 设计稿版本声明（design version declaration）

> 本文件是 `docs/ui/kidpal-ui-design-vN.html` 版本族的**唯一声明面**（261027-gov-review-p5-spike-anchor-cluster T2 · ARQ-261017）。
> 形态镜像 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 的先例：**声明面记当前版本 + 全量沿革，其它文面只留指针、不复述版本号**。
> 消费者：机检 `spec-anchor-check.mjs`（防御式解析本文件的 `current:` 行）、story 锚行（`write-story` 锚行规则第 4 条）、出图（`requirement-change` 档 2 流程 4b）、TaskPackage `designRefs[]`。
> 维护纪律：**母版升版 = 内容完整落盘 + 本文件 `current:` 行 + 逐版备注追加，同一次落盘**（禁中间态）；本文件 append-only，历史行不回改。
> 版本编号两族（判读歧义已实测）：本文件的 `vN` 是**设计 HTML 版号**；`UIUX-DESIGN-BRIEF.md` 头部 `版本：vN` 是 **brief 自身的版本**——brief 只在需求批升版，设计 HTML 在自己的版式批也升，两族自 2026-09 起错位（brief v5 ↔ 设计 v6，brief v6 ↔ 设计 v7）。引用设计稿一律用本文件，不引 brief 版本号。

## 机读声明（全文恰一行 `current:`，行首匹配；勿改格式）

current: kidpal-ui-design-v7.html

## 逐版备注（append-only；新版本追加在末尾）

| 版本 | 落盘日期 | 升版批 / 触发 | 要点（`data-od-id` 计数为实测） |
|---|---|---|---|
| （版本化前基座）`kidpal-ui-design.html` | 2026-08-27 | 治理迁移入仓（commit `9d626251`）后的初版 | 19 个 `data-od-id`，页级粗锚（`page-*` / `state-*` / `components`），无版本化命名 |
| v2 | 2026-08-29 | commit `770c3009`（吉祥物重绘 + P4 米字格改楷体） | 61 个 id；锚粒度细化为画布 / 状态 / 子态（`canvas-pX` / `state-*` / `shot-*`），首个完整母版 |
| v3 | 2026-09-09 | commit `54280819`（V3 定稿：human 三观感裁定 + 吉祥物蝴蝶结） | 61 个 id，id 集与 v2 完全一致——纯观感修订，不动锚 |
| v4 | 2026-09-14 | commit `4587bf62`（对齐 requirements v1.51） | 60 个 id；撤 6 个（`canvas-p4` / `p4-notes` / `pose-sleep` / `shot-p4` / `state-sleepy` / `swap-demo`，P4 页撤除与睡眠姿态砍除）、增 5 个（`char-notes` / `char-window` / `overlay-rules` / `p3-delete-confirm` / `shot-char`，查字窗上线） |
| v5 | 2026-09-22 → 2026-10-12 | commits `ec0804bd`…`101629e4`（ui-v5 系列 + `261011-p1-home-v5-rework`；要点四项自 `31d364d5`（2026-09-23）起） | 60 个 id，id 集与 v4 完全一致——纯版式重构（四态骨架归一 / 聆听态重排 / 提醒态右区收敛 / 首页明天组时间打头） |
| v6 | 2026-10-17 | commit `e6253317`，批 `261016-p3-story-body-sd-cache`（requirements v1.64） | 61 个 id（+1 = `p3-body-unavailable`，WF-F2-06 正文取不回提示覆盖层）。**本版曾出事故**：增量件（3 个 id）先占用 v6 命名 → 全仓 12 条 story 锚假红（涉 10 条 story），重建为完整母版后复绿——即 ARQ-261017 的立案现场 |
| v7 | 2026-10-19（2026-10-21 后补账） | commits `356a9e25` + `05cf4910`，批 `261019-p3-record-dialogue-guide`（requirements v1.65） | 64 个 id（+3 = `p3-recording-guide` / `p3-recording-guide-spec` / `state-organizing`），且为 v6 的**严格超集**（缺失 0）。按本批 intake R1 裁定由主线程「合并完整 v7 母版（v6 基底 + 三帧）」；15 画布全帧出图、三帧入库 `docs/ui/V7/` |

## 现行指针的依据记录（一次性，append-only）

**2026-10-27 · current 初设 = v7（本声明面设立批的判定依据）**：① v7 含 v6 全部 61 个 id 且 +3，严格超集、缺失 0，是最新且形质兼备的完整母版；② 最近两批消费实证均指向 v7——`261019-p3-record-dialogue-guide` 的 fixer brief 以 v7 三帧为改法权威、出图脚本（`tools/asset-pipeline/shoot_design.mjs`）按该批 intake 记录自 v7 HTML 全帧出图 15 画布、`261020-p3-rec-frame-visual` 真机观感终判后回同步 v7；③ 机检基线（改前按最高版号选稿）输出的 `design` 亦为 v7，声明化后输出不变；④ `UIUX-DESIGN-BRIEF.md` 头部「完整母版仍为 v6」的说法被严格超集关系证伪，属陈旧声明，不再作判据。

## 活图夹与冻结快照（页图版本语义；2026-10-27 OQ2 丙裁成文）

**页图只有一个活图夹**：`docs/ui/` 的页图（400×300 画布 / 状态 PNG，`designRefs[]` 的布局权威）只在**单一活图夹**内更新——活图夹始终反映本文件 `current:` 所指母版的现行形态，母版升版即覆盖更新。**升版冻结义务**：母版升版时，上一版页图**冻结为快照**，快照一旦落盘不再改动；已发出的 `designRefs[]` 指向冻结快照（快照 = 布局权威，不随时间漂移），新引用（`designRefs` / 出图 / 截图）一律以本文件 `current:` 为准、不钉版本夹（机制落地后生效；落地前维持现行版本夹惯例：`docs/ui/V<N>/` 逐批落变更帧）；旧包 `designRefs` 不追溯改。**本批（261027）只成文语义、不落机制**：frozen 快照的目录形态与生成流程、活图夹的具体目录名、`docs/ui/V5/` / `V6/` / `V7/` 存量页图迁移均另立需求面动作（登记见 261027-gov-review-p5-spike-anchor-cluster 的 PROPOSAL §6 留根 4）；机制落地前图夹维持现状。
```

**格式契约（机读，供脚本防御式解析）**：全文**恰一行**以 `current:` 起头（其余行不得同形）；值为母版文件名，容忍两侧反引号；违反任一 = 输入缺失（exit 2，见 T2.2-D）。逐版备注中禁出现行首 `current:`（写入本表时已核：全文 `^current:` 计数 = 1）。

#### T2.2 `spec-anchor-check.mjs` 六处编辑（脚本文件为 **CRLF**，落地须保持行尾；原文行号已逐条实跑亲核，R1-F1 的漂移已按实测更正）

**编辑 A0（新增）· 头注 ② 条之后补声明面一句**（原文 `:6` 之后插入一行）：

原文（`:6`，逐字节）：
```text
//   ② story 锚行的 UI 锚 → docs/ui/ 现行设计稿真实存在的 data-od-id；
```
改后在原文行后插入一行（插入行以 `//` 起首、六个空格续行缩进；原 `:7`（`//   ③ 可行性 🔴 …`，即 ③ 行）及其后各行顺延一行；插入行逐字节照抄下方围栏）：
```text
//      现行设计稿 = docs/ui/UI-DESIGN-VERSIONS.md 声明面的 current 指针（261027 批 T2；不再按「最高版号」推断）。
```

**编辑 A（原样保留）· 用法块之后补逃逸旗说明**（原文第 10–12 行；插入两行）：

原文（`:10-12`，逐字节）：
```text
//   配合 write-story skill「锚行规则」第 5 条；被 story 引用而消失的 id 显式点名）。
//
// 拦阻强度：手动 / review 时跑（OQ-4 裁 (a)），不挂 hook。
```
改后（逐字节）：
```text
//   配合 write-story skill「锚行规则」第 5 条；被 story 引用而消失的 id 显式点名）。
// --allow-master-shrink：跳过母版完整性自查（261027 批 T2 · ARQ-261017）——只应用于有意的整版收缩，
//   跑者承担裁断依据留痕；常态不传。
//
// 拦阻强度：手动 / review 时跑（OQ-4 裁 (a)），不挂 hook。
```

**编辑 B（扩写）· 真空绿规约句之后追加声明面 + 完整性自查四行**（原文 `:13` 之后；v0.1 的两行扩为四行）：

原文（`:13`，逐字节）：`// 真空绿规约：先断言两文件非空 + story 计数 > 0 + 锚行计数 > 0，再谈检查通过。`

改后（逐字节）：
```text
// 真空绿规约：先断言两文件非空 + story 计数 > 0 + 锚行计数 > 0，再谈检查通过。
// 设计稿版本声明（261027 批 T2 · ARQ-261017）：现行设计稿 = 声明面 docs/ui/UI-DESIGN-VERSIONS.md 的
//   current 指针所指文件；声明缺失 / current 行缺失或重复 / 指针失效 = 输入缺失（exit 2），不按最高版号兜底。
// 母版完整性自查（261027 批 T2 · ARQ-261017）：现行设计稿相对前序版本 data-od-id 收缩过半 ⇒ 判
//   「疑似增量稿占用母版命名」，exit 2 并中止，让失败指认「母版不完整」而不是「你的锚没了」。
```

**编辑 C（改写）· 退出码行**（原文 `:20` 整行替换）：

原文（`:20`，逐字节）：`// 退出码：0 全绿 / 1 有悬空或违规 / 2 用法或输入缺失。`

改后（逐字节）：`// 退出码：0 全绿 / 1 有悬空或违规 / 2 用法或输入缺失（含设计稿版本声明缺失或指针失效）或母版疑似不完整。`

**编辑 D（合并重写，取代 v0.1 的 D + E）· `currentDesignFile` 整块替换为声明面读法 + `previousDesignFile`**（原文 `:36-43`，八行整块；v0.1 的编辑 D「在 `:46-47` 后插入函数」与编辑 E「新函数」由本编辑整体取代）：

原文（`:36-43`，逐字节）：
```text
/** 现行设计稿 = docs/ui/ 下版本号最大的 kidpal-ui-design-vN.html。 */
function currentDesignFile() {
  if (!existsSync(UI_DIR)) return null;
  const files = readdirSync(UI_DIR).filter((f) => /^kidpal-ui-design-v\d+\.html$/.test(f));
  if (!files.length) return null;
  files.sort((a, b) => Number(b.match(/v(\d+)/)[1]) - Number(a.match(/v(\d+)/)[1]));
  return files[0];
}
```
改后（逐字节）：
```js
/** 现行设计稿 = 声明面 docs/ui/UI-DESIGN-VERSIONS.md 的 current 指针所指文件（261027 批 T2：显式指针取代「最高版号 wins」）。 */
const DESIGN_DECL_PATH = join(UI_DIR, 'UI-DESIGN-VERSIONS.md');
const DESIGN_MASTER_RE = /^kidpal-ui-design-v\d+\.html$/;

/** 声明面输入缺失 / 非法 = 输入缺失（与真空绿规约同档，exit 2）。 */
function designDeclarationFatal(reason) {
  process.stderr.write(
    `[spec-anchor-check] FATAL（设计稿版本声明）: ${reason}——` +
      `现行设计稿以 docs/ui/UI-DESIGN-VERSIONS.md 的 current 指针为准（261027 批 T2），不再按最高版号推断；` +
      `声明缺失 / current 行缺失或重复 / 指针指向的文件不存在或不合母版命名族（kidpal-ui-design-vN.html）均为输入缺失。\n`,
  );
  process.exit(2);
}

function currentDesignFile() {
  if (!existsSync(DESIGN_DECL_PATH)) designDeclarationFatal(`声明文件不存在（${DESIGN_DECL_PATH}）`);
  const curLines = readFileSync(DESIGN_DECL_PATH, 'utf8')
    .split(/\r?\n/)
    .filter((line) => /^current:/.test(line));
  if (curLines.length !== 1) designDeclarationFatal(`current 行应恰一行，实得 ${curLines.length} 行`);
  const name = curLines[0].slice('current:'.length).trim().replace(/^`|`$/g, '');
  if (!DESIGN_MASTER_RE.test(name)) designDeclarationFatal(`current 指针「${name}」不属母版命名族（kidpal-ui-design-vN.html）`);
  if (!existsSync(join(UI_DIR, name))) designDeclarationFatal(`current 指针「${name}」指向的文件不存在`);
  const curV = Number(name.match(/v(\d+)/)[1]);
  const higher = readdirSync(UI_DIR)
    .filter((f) => DESIGN_MASTER_RE.test(f))
    .map((f) => ({ f, v: Number(f.match(/v(\d+)/)[1]) }))
    .filter((x) => x.v > curV)
    .sort((a, b) => b.v - a.v);
  if (higher.length) {
    process.stderr.write(
      `[spec-anchor-check] 提示: docs/ui/ 存在更高版号 ${higher[0].f}，但声明面 current = ${name}——以声明为准；` +
        `若属升版遗漏，请同一次更新声明面（形质兼备再升版，见 .claude/agents/uiux-author.md）。\n`,
    );
  }
  return name;
}

/** 前序设计稿 = 版本号严格小于现行稿的最大者；无则 null（母版完整性自查用，261027 批 T2）。 */
function previousDesignFile(current) {
  const cur = Number(current.match(/v(\d+)/)[1]);
  const files = readdirSync(UI_DIR)
    .filter((f) => DESIGN_MASTER_RE.test(f))
    .map((f) => ({ f, v: Number(f.match(/v(\d+)/)[1]) }))
    .filter((x) => x.v < cur)
    .sort((a, b) => b.v - a.v);
  return files.length ? files[0].f : null;
}
```

**编辑 E（原样保留，行号已亲核）· 正向模式完整性自查块**（插在 `// 正向三检`（`:169`）与 `const errors = [];`（`:170`）之间；因位置在 `--reverse` 早退之后，该自查**只对正向模式生效**）：

```js
// 母版完整性自查（261027 批 T2 · ARQ-261017）：现行设计稿相对前序版本塌缩过半 ⇒ 疑似增量稿占用母版命名。
// 中止并指认母版缺口，避免把「母版不完整」误报成 12 条 story 锚漂移（261017 实测形态）。仅正向模式生效。
const MASTER_SHRINK_MIN_PREV_IDS = 10;
const MASTER_SHRINK_MIN_RATIO = 0.5;
const prevDesign = designFile ? previousDesignFile(designFile) : null;
if (designFile && prevDesign && !args.includes('--allow-master-shrink')) {
  const prevIds = extractOdIds(readFileSync(join(UI_DIR, prevDesign), 'utf8'));
  const missing = [...prevIds].filter((id) => !designIds.has(id));
  const ratio = prevIds.size ? missing.length / prevIds.size : 0;
  if (prevIds.size >= MASTER_SHRINK_MIN_PREV_IDS && ratio >= MASTER_SHRINK_MIN_RATIO) {
    process.stderr.write(
      `[spec-anchor-check] FATAL（母版完整性）: 现行设计稿 ${designFile} 仅 ${designIds.size} 个 data-od-id，` +
        `前序 ${prevDesign} 有 ${prevIds.size} 个（缺失 ${missing.length} 个、占 ${Math.round(ratio * 100)}%）——` +
        `疑似增量稿占用母版命名。kidpal-ui-design-vN.html 命名族 = 完整母版（纪律见 .claude/agents/uiux-author.md）：` +
        `增量/局部稿用临时命名（如 kidpal-ui-design-v<N>-partial.html），母版升版号与内容完整同一次落盘。` +
        `本检查已中止，以免把母版缺口误报成 story 锚漂移；若确为有意的整版收缩，带 --allow-master-shrink 复跑。\n`,
    );
    process.exit(2);
  }
}
```

#### T2.3 纪律文面四处 / 五条目（v0.1 的实施件 6/7 由本组取代并扩写；C-1b = 261027 批 OQ2 丙裁新增）

**C-1 · `uiux-author.md` Process 第 2 步**（`:26` 整行替换；v0.1 只写命名族，本稿并写声明面）：

原文（逐字节）：`2. 修订/生成设计 HTML（母版迭代载体）。`

改后（逐字节）：
```text
2. 修订/生成设计 HTML（母版迭代载体）。**母版命名族与版本声明纪律（261027 批 · ARQ-261017）**：`docs/ui/kidpal-ui-design-vN.html` 命名族 = **完整母版**（含全部画布与状态锚的单一文件）——升版号与内容完整必须同一次落盘，禁中间态占用母版命名；受写入体积限制须分块时，先写临时命名（如 `kidpal-ui-design-v<N>-partial.html`，不匹配机检母版正则 `^kidpal-ui-design-v\d+\.html$`），构建完整后一次重命名落位。**同一次落盘还须更新声明面 `docs/ui/UI-DESIGN-VERSIONS.md`**：`current:` 指针指向新母版 + 逐版备注追加一行（日期 / 升版批 / 要点）——机检 `spec-anchor-check.mjs`、出图与 story 锚引用一律以声明面为准，不按「最高版号」推断；声明缺失或指针失效时机检 exit 2 中止。机检在现行稿相对前序版本塌缩过半时另会显式报「母版完整性」并中止，而不是把母版缺口报成 story 锚漂移。
```

**C-1b · `uiux-author.md` Process 第 3 步出图落点句**（`:27` 整行替换；261027 批 OQ2 丙裁成文——页图落点语义：单一活图夹 + 升版冻结义务 + 引用一律走声明面 current）：

原文（`:27`，逐字节）：
```text
3. 产出**出图规格单**：逐页 pageId/stateId、画布 400×300、黑白 1bpp 风格约束、内容要点、对应 brief 锚——交主线程代执行出图（od_*），PNG 文件名对齐 pageId/stateId 落 `docs/ui/`。**PNG 是 builder 布局坐标的定位权威。**
```
改后（逐字节）：
```text
3. 产出**出图规格单**：逐页 pageId/stateId、画布 400×300、黑白 1bpp 风格约束、内容要点、对应 brief 锚——交主线程代执行出图（od_*），PNG 文件名对齐 pageId/stateId 落 `docs/ui/` 的**单一活图夹**（261027 批 OQ2 丙裁）。**PNG 是 builder 布局坐标的定位权威。** **页图版本语义**：活图夹始终反映声明面 `docs/ui/UI-DESIGN-VERSIONS.md` 的 `current:` 所指母版的现行形态；母版升版时上一版页图**冻结为快照**，快照落盘后不再改动——已发出的 `designRefs[]` 指向冻结快照，布局权威的不变性由此保障；新引用（`designRefs` / 出图 / 截图）一律以 `current:` 所指活图夹为准，不钉版本夹（机制落地后生效；落地前维持现行版本夹惯例：`docs/ui/V<N>/` 逐批落变更帧），旧包引用不追溯改。活图夹的具体目录名、frozen 快照机制本体与 `docs/ui/V5|V6|V7` 存量迁移另立需求面动作（本批只成文；机制落地前图夹维持现状）。
```

**C-2 · `write-story/SKILL.md` 锚行规则第 4 条**（`:48` 整行替换；指针由「钉死版本号」改指声明面——v0.1 的「版本无关形态（最高版号）」由本稿取代，理由：最高版号本身就是复证 4 的病灶源之一）：

原文（逐字节）：`4. **指向**：锚引用指「docs/ui/ 现行设计稿」语义，版本更迭只改本行一处；当前 = [kidpal-ui-design-v3.html](../../docs/ui/kidpal-ui-design-v3.html)。`

改后（逐字节）：
```text
4. **指向**：锚引用指「docs/ui/ 现行设计稿」语义，版本更迭不需要改本行；当前稿 = 声明面 [UI-DESIGN-VERSIONS.md](../../docs/ui/UI-DESIGN-VERSIONS.md) 的 `current:` 指针所指的 `kidpal-ui-design-vN.html`（机检 `spec-anchor-check.mjs` 自行解析；261027 批：原写法「版本更迭只改本行一处」实际每次升版必漂——实测曾长期停在 v3 而现行稿已是 v7。声明面是唯一来源，不按最高版号推断；完整母版命名族纪律见 `uiux-author` 定义）。
```

**C-3 · `docs/ui/UIUX-DESIGN-BRIEF.md` 版本行尾部**（`:5` 行内替换；**决定 = 保留 brief 自身版本行、删设计稿版本声明**——依据：`requirement-change.md:58` 把「UIUX-DESIGN-BRIEF 版本行注明对齐的本批 requirements 版本号」定为 UI frozen 与 launchPreconditions 的机械判据，删版本行会打断该判据；故版本行留、其中夹带的设计稿版号声明外移）：

原文（`:5` 行内子句，逐字节）：`本批增量渲染见 [kidpal-ui-design-v7.html](kidpal-ui-design-v7.html)，完整母版仍为 [kidpal-ui-design-v6.html](kidpal-ui-design-v6.html)。`

改后（逐字节）：
```text
设计稿版本（现行母版与逐版沿革）以声明面 [UI-DESIGN-VERSIONS.md](UI-DESIGN-VERSIONS.md) 为准，本行不复述——本行「版本：vN」是**本 brief 自身的版本**，与设计稿版号是两个编号族（brief 只在需求批升版、设计 HTML 在自己的版式批也升，两族自 2026-09 起错位：brief v5 ↔ 设计 v6、brief v6 ↔ 设计 v7）。（261027 批：原「增量渲染见 v7、完整母版仍为 v6」的说法在 v7 已并合为 v6 严格超集母版后失真。
```

（末行落盘时与原有的外括号 `）` 相接，全文读作「…后失真。）」；`:106` 的批次叙事句「增量渲染见 v7（帧 A/B/C）」属本批历史记录，**不改**。）

**C-4 · `.claude/references/requirement-change.md` 4b 出图段**（`:45` 行内插入；human 追加要求「出图、截图引用一律以声明为准」的落点）：

原文（`:45` 行内子句，逐字节）：`回填 PNG 落 \`docs/ui/\`，文件名对齐 pageId/stateId。`

改后（逐字节）：
```text
回填 PNG 落 `docs/ui/`，文件名对齐 pageId/stateId。**设计稿版本以声明面 `docs/ui/UI-DESIGN-VERSIONS.md` 的 `current:` 指针为准**（261027 批：机检 / 出图 / 截图引用同源；母版升版即同一次更新声明面，不按最高版号推断）。
```

#### T2.4 存量迁移（逐版备注回填依据）

逐版备注的每个单元格均有对应证据（复证 4）：日期 / commit / 批取自 `git log --follow` 逐版实跑；`data-od-id` 计数与集合差由脚本逐版比对（v2 61 / v3 61 / v4 60 / v5 60 / v6 61 / v7 64；v3→v4 撤 6 增 5；v4→v5 零变更；v5→v6 +1；v6→v7 +3 且严格超集）。**`current:` 初设 = v7 的四条依据**（v7 严格超集、fixer brief 以 v7 为权威、15 帧全帧出图、机检基线同指 v7）写进声明面的「现行指针的依据记录」节——这不是拍脑袋选最新号，而是「最新号 + 实际消费 + 形质兼备」三证合一。**本节迁移仅指设计 HTML 版本沿革的声明材料回填；截图页图（PNG）的存量迁移不在本批**——OQ2 丙裁已给出图夹目标语义，但机制本体与目录整形另立需求面动作（§6 留根 4）。

#### T2.5 `ARQ-261017` 条目结项记账

frontmatter 三字段翻转：`status: deferred` → `status: closed`；`closed: ""` → `closed: 2026-10-27`；`closedBy: ""` → `closedBy: 261027-gov-review-p5-spike-anchor-cluster`。

正文 append（插在文末「…由跨面变更 `261016-p3-story-body-sd-cache` 实施期实测派生（**该变更自身的暴露项，非外部发现**）。」之后，空一行）：

```markdown
**2026-10-27 · 结项（`261027-gov-review-p5-spike-anchor-cluster`，T2）**：① 声明面（治本）——新增 `docs/ui/UI-DESIGN-VERSIONS.md`（形态镜像 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 先例：声明面记当前 + 全量沿革，其它文面只留指针）：机读 `current:` 指针对当前母版 + v2–v7 逐版备注（git 考据回填，含 v6 增量件事故现场）+ 现行指针依据记录；初设 `current = kidpal-ui-design-v7.html`（v7 = v6 的严格超集、缺失 0；最近两批消费实证与机检基线同指 v7）。② 机器侧——`spec-anchor-check.mjs` 的「现行设计稿」改由声明面 `current:` 指针决定，原文「目录里版本号最大者」的隐式判据废止；声明缺失 / `current` 行缺失或重复 / 指针指向文不存在或不合母版命名族一律 exit 2（输入缺失档），不静默回退最高版号；存在更高版号而声明未跟随 → stderr 提示、以声明为准。③ 完整性自查保留为兜底——现行稿相对「版本号严格小于它的最大者」比 `data-od-id` 集，前序 ≥10 且缺失比例 ≥50% 即判「疑似增量稿占用母版命名」、exit 2 并指认两版锚数与占比；逃逸旗 `--allow-master-shrink` 只用于有意的整版收缩（阈值按真实演进史标定：v3→v4 实测收缩 9.8% 属合法页撤，病态夹具 83%）。两层叠加：增量件占母版命名的旧病由声明面挡（比对源不跟最高版号走），声明面跟进升版时由完整性自查挡（形质不兼备的升版当场拦）。④ 消费面行为：正向 / `--reverse` 均要求声明面在场（同属输入缺失档；逆视图输出结构不变）；`--diff-ids` 不读声明面、行为不变。⑤ 纪律侧——`uiux-author` Process 第 2 步成文「命名族 = 完整母版 + 升版与声明面同一次落盘」、第 3 步出图落点句成文「单一活图夹 + 升版冻结快照 + 引用一律走声明面 current」（OQ2 丙裁）；`write-story` 锚行规则第 4 条指针改指声明面；`UIUX-DESIGN-BRIEF.md` 头部的设计稿版本声明改指声明面（brief 自身版本号保留——它是「对齐哪版 requirements」的判据载体，两族错位如实记）；`requirement-change` 4b 出图版本指声明面。**无已污染制品需清理**（原条目补充说明仍成立）。**边界**：与 tech 域 `ARQ-260918-spec-face-3page-reconcile`（同工具不同根）不合并——该条治 spec 叙述与实现漂移、仅引用本脚本作验证手段，本条治脚本判据自身缺口；本批 `docs/requirements.md` / `docs/architecture.md` 零 diff。留根：① 截图图夹布局 OQ2 已裁丙（单一活目录 + 冻结快照）——本批只落规则句与声明面语义节，frozen 快照机制本体、活图夹目录整形与 `docs/ui/V5|V6|V7` 存量迁移另立需求面动作，本批不动图夹与 `designRefs` 形态；② `tools/asset-pipeline/verify_art_assets.mjs:14` 仍硬编码 `kidpal-ui-design-v3.html` 绝对路径、`docs/ui/PHASE1-DESIGN-INPUT.md:5` / `:128` 仍称 v4 为最新母版——两处待另行登记。
```

**为什么这样改**：对四条根因一一对应——① 声明面把「当前是哪一版」从文件系统隐式推断变成**一个显式、可读、可机检的指针**；② 逐版沿革集中在声明面，其它文面只留指针（carrier 先例的核心收益：沿革只有一份全量副本，不会各记一份）；③ 完整性自查保留为兜底层并维持「先于锚校验」的位置——增量件占了母版命名时失败第一句就是「母版疑似不完整」而不是「你的锚没了」；④ 命名族纪律 + 临时命名兜底 + 「升版与声明面同一次落盘」堵住顺序侧：**形质兼备才升版，升版即改指针**。**两层防线为什么必须叠加（而不是只留声明面）**：声明面挡的是「比对源跟着版本号跑」（旧病），完整性自查挡的是「声明面被跟着改到一份塌缩稿上」（新病的唯一入口）——只留任何一层都有明确缺口；两层叠加时，病态夹具仍 exit 2（§5 B5），而真仓在正确声明下输出与改前逐字一致（§5 B17）。**为何失败一律 exit 2 而非 warn**：声明面是输入，不是判据——输入缺失时任何「按旧规则兜底」都会把「当前是哪版」这个唯一事实交还给隐式推断，等于把本批要治的病请回来。**为何「存在更高版号」只提示不拦**：声明是权威，更高版号存在本身不代表异常（可能是并行分支的临时件、也可能是忘改指针）；拦它等于重新引入「最高版号 wins」，提示则把人引到正确动作（同一次更新声明面）。**`--reverse` 也要求声明面的如实标注**：声明解析在 `--reverse` 早退之前（与 `designIds` 同一处），故逆视图模式在声明缺失时同样 exit 2；这是**有意的行为变化**（输入校验前移），逆视图的 JSON 结构零变化（§5 B15）。

目标文件：`.claude/scripts/spec-anchor-check.mjs`（改） · `.claude/agents/uiux-author.md`（改） · `.claude/skills/write-story/SKILL.md`（改） · `.claude/references/requirement-change.md`（改） · `docs/ui/UI-DESIGN-VERSIONS.md`（新建） · `docs/ui/UIUX-DESIGN-BRIEF.md`（改） · `docs/debt/gov/ARQ-261017-anchor-check-master-unvalidated.md`（改）

### T3 · `ARQ-260908` 结项记账（四件重指授权「确认结项，不再重指」）

**问题**：条目在册的 4 件待重指项（版本无关重指）悬置已一年有余；若继续按「逐件授权重指」处理，实际义务已经消失——四件的落点全部进了归档面，重指没有执行对象。

**证据**：见 §2.0 复证 3（A.3 清单与行号；teeth-sweep commit `77b6b0cf`；`server/pytest.ini` 的 archive 排除；live grep 0；继任测试逐版钉 + live 观测项声明；human 2026-10-27 四问四拍）。

**根因**：条目开具时的前提（4 件仍在 live 验收套件里、版本无关重指有实际收益）被后续的 `ARQ-260916-server-mvp-teeth-sweep` 批整体归档改变；条目 trigger 只说「发起 = 下一批触碰对应冻结件时」，没有「落点已被归档消化即结项」的收口判据，于是挂账至今。

**改法**（纯记账两件，不执行任何重指）：

1. **frontmatter 三字段**：`status: deferred` → `status: closed`；`closed: ""` → `closed: 2026-10-27`；`closedBy: ""` → `closedBy: 261027-gov-review-p5-spike-anchor-cluster`。`owner: ""` **不动**（记账不追认归属）。
2. **正文末尾 append 结项注记**（插在文末「…触他人冻结件逐件 human 授权」之后，空一行）：

```markdown
**2026-10-27 · 结项（`261027-gov-review-p5-spike-anchor-cluster`，纯记账、未执行任何重指）**：原义务 = 对附录 A.3 在册的 4 件待重指项逐件取 human 授权；权威清单 = `delivery/archive/changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md` 附录 A.3（归档只读）。复证（2026-10-27 本批起草期实测）：① A.3 的 4 件落点已被后续批 `ARQ-260916-server-mvp-teeth-sweep`（commit `77b6b0cf`，2026-09-16，MVP / 验证期 server 面冻结验收 10 件整体归档）消化——三份被引冻结件 `test_char_stroke.acceptance.py` / `test_schedule_p2.acceptance.py` / `test_voice_roundtrip_b.acceptance.py` 已移入 `server/acceptance/archive/`，归档件由 `server/pytest.ini` 的 `norecursedirs = … archive` 排除出裸收集；② live 验收套件（`server/acceptance/`，排除 archive）对 `PROMPT_CARRIER_V2` / `P2-A6` / `CHR-A8` / `CHR-A9` / `VRT-A4` / `VRT-A7` 全部 grep 命中 0，即无第二个执行面消费这些版本钉死常量；③ 继任覆盖在场：`server/acceptance/test_prompt_v7_round_routing.acceptance.py` 按版本演进逐版钉（v7 → v9 → v10 → v11：落盘结构断言 + 制品首行无版本前言 + 前版 immutable 负锚 + 三面接线一致性），live 探针一律显式声明为「live 观测项——非自动断言」（PN-A5 / RF-A5 / PV-A8），A.3 当年担心的「版本钉死假绿」在继任面已结构性消解；④ 归档树 append-only 禁改写，对已归档 case 做重指无意义。human 2026-10-27 逐件授权（AskUserQuestion 四问四拍，主线程记录随本批任务书携带）：① schedule_p2 P2-A6 接线（A.3 #3）② VRT live 探针 A4/A7（A.3 #6）③ CHR-A8/A9 live 探针（A.3 #7）④ schedule_p2 `PROMPT_CARRIER_V2` 常量（A.3 #8）——四件均裁「确认结项，不再重指」。无留根。
```

**为什么这样改**：对根因——条目义务的落点已不存在，唯一正确的收口是把「为什么可以结项」写成可复查的注记（A.3 出处 + 消化批 commit + 零残留复核 + 继任覆盖 + 逐件授权），而不是把条目默默移走。**不执行重指**是因为归档树 append-only、对已归档 case 改断言无意义且会破坏历史证据（§0 非目标）。

目标文件：`docs/debt/gov/ARQ-260908-versioned-guard-repoint-execution.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 流程定义 G2 | `.claude/references/change-triage.md` | 改（T1①） | §2 增「新机制批判定」小节；消费方 = `/triage-change` 下 architect 判档（命令侧 `triage-change.md` 只引用本 SSOT，无第二源） |
| 流程定义 G2 | `.claude/references/dev-loop.md` | 改（T1②③） | 新增「新机制批 spike 相位」节 + 变更制品表 `spike-evidence.md` 条目；消费方 = planner / dev-loop 会话 |
| 流程定义 G2 | `.claude/agents/planner.md` | 改（T1④） | 新增 Step 1i（intake 复查）；消费方 = 每个 dev 包 planner 会话 |
| 治理制品骨架 | `docs/templates/PROPOSAL.template.md` | 改（T1⑤） | §2 增「新机制 spike 申报」硬项；消费方 = 后续治理 / 架构批起草者与判官 |
| 强制执行层 G3 | `.claude/scripts/spec-anchor-check.mjs` | 改（T2.2） | 头注四处（A0 / A / B / C）+ `currentDesignFile` 整块重写（声明面读法、D）+ 正向完整性自查（E）；消费方 = 6 处文档消费（requirement-reviewer / story-author / requirement-change 命令与 reference / write-story / arch-doc-restructure），零代码 import；`--diff-ids` 行为不变、`--reverse` 新增声明面在场要求（输出结构不变） |
| 流程定义 G2 | `.claude/agents/uiux-author.md` | 改（T2.3-C1 / C1b） | Process 第 2 步增「命名族 = 完整母版 + 声明面同次落盘」纪律、第 3 步出图落点句增「单一活图夹 + 升版冻结快照 + 引用走声明面 current」语义（OQ2 丙裁）；消费方 = 需求 loop 档 2 触 UI 批（同文件另有 `ARQ-261019` 的写者分工待落，见 §6 留根 5） |
| 流程定义 G2 | `.claude/skills/write-story/SKILL.md` | 改（T2.3-C2） | 锚行规则第 4 条指针改指声明面；消费方 = story-author / requirement-reviewer |
| 流程定义 G2（需求面 SSOT，单句指针） | `.claude/references/requirement-change.md` | 改（T2.3-C4） | 4b 出图段增「设计稿版本以声明面为准」一句（human 2026-10-27 立项追加授权；**仅指针、零流程语义变化**，本节该行其余内容不动）；消费方 = 需求 loop 档 2 触 UI 批的出图执行方 |
| 设计面（声明面，新增） | `docs/ui/UI-DESIGN-VERSIONS.md` | 新建（T2.1） | 版本族唯一声明面（`current:` 指针 + 逐版备注 + 依据记录 + 活图夹与冻结快照语义节〔OQ2 丙裁成文〕）；消费方 = 机检 / 出图 / story 锚 / `designRefs`；写者 = uiux-author（`docs/ui/**` 在其 scope-map allow 内）或主线程代落（human 立项追加授权本批落地） |
| 设计面 | `docs/ui/UIUX-DESIGN-BRIEF.md` | 改（T2.3-C3） | 版本行尾部的设计稿版本声明改指声明面；**brief 自身版本号与 requirements 对齐号保留**（`requirement-change.md:58` 的 launchPreconditions 判据载体） |
| 账本 / 证据 G4 邻接 | `docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md` · `docs/debt/gov/ARQ-261017-anchor-check-master-unvalidated.md` · `docs/debt/gov/ARQ-260908-versioned-guard-repoint-execution.md` | 改（T1⑥ / T2.5 / T3） | 三条目 status 翻转 + append 结项注记；落地后由 `debt-render.mjs` 自动归位 `docs/debt/gov/_archive/2026/`（生成动作，不进 manifest，见 §5 V8） |
| 生成件（不在 manifest） | `docs/debt/gov/INDEX.md` · `docs/debt/gov/_archive/2026/INDEX.md` · `delivery/review/INDEX.md` | 再生 | 前者由 `debt-render.mjs` 重渲染；后者由 `changes-index.mjs --upsert` 单行钉入（禁手改） |
| 指令根 G1 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | 未触 | 零 diff |
| 契约面 | `docs/contracts/**` | 未触 | 零 diff |
| 规范 spec | `docs/requirements.md` / `docs/architecture.md` | 未触 | 零 diff（与 tech 域 `ARQ-260918` 的边界，见 §6 留根 6） |
| 设计面其余 | `docs/ui/kidpal-ui-design-v*.html` · `docs/ui/V5|V6|V7/**` · `docs/ui/PHASE1-DESIGN-INPUT.md` · `docs/ui/brand-spec.md` | 未触 | 零 diff；图夹布局 OQ2 已裁丙（本批只成文语义、零搬图，机制与存量迁移另立需求面动作）、PHASE1 陈旧声明留根（§6） |
| 实现面 | `firmware/**` / `server/**` / `tools/**` | 未触 | 零 diff；不执行任何重指、不批量重出图 |
| 其余强制层 | `.claude/hooks/**` / `.claude/scope-map.json` / `*.settings.json` / `.claude/verify-gates.json` / `.claude/scripts/pre-merge-escalation-gate.mjs` | 未触 | 零 diff；无新增门、无门判定语义变化 |

**【硬项 · sweep 透镜义务】新字面 / 新判定 pattern 全仓预跑（起草期 2026-10-27，命令 + 结果）**：

- **新字面碰撞基线**（`grep -rlF`，语料 = 全仓 `.md` / `.mjs` / `.json` / `.txt` / `.yaml` / `.yml`，单根 `.`，排除本批 `delivery/review/261027-gov-review-p5-spike-anchor-cluster/`）：`UI-DESIGN-VERSIONS` = **0** 文件；`designDeclarationFatal` = **0**；`DESIGN_DECL_PATH` = **0**；`DESIGN_MASTER_RE` = **0**；`设计稿版本声明` = **0**；`母版命名族与版本声明纪律` = **0**；`设计稿版本以声明面` = **0**；`形质兼备` = **0**；行首 `^current:` = **0**；`spike-evidence` = **0**；`allow-master-shrink` = **0**；`previousDesignFile` = **0**；`MASTER_SHRINK` = **0**；`母版命名族纪律` = **0**；`母版完整性` = **1**（`delivery/review/261017-arch-review-p3-story-body-sd-cache/retro.md:50`，同一根的历史收件记录，非撞车）；`新机制批` = **1**（`docs/debt/gov/ARQ-261024…md` 自身，非撞车）；`spike-first` = **4**（`ARQ-261024` / `docs/debt/gov/INDEX.md` / `k4-recall-architecture-20261024.md` / `gov-debt-reduction/task_plan.md`，全部同根治理记录，语义同向）；`以声明为准` = **1**（`gov-debt-reduction/task_plan.md:78` 的立项追加记录本身——同根，非撞车）；**v0.3 新增字面（OQ2 丙裁）复跑 2026-10-27**：`活图夹` = **0**、`单一活图夹` = **0**、`布局权威` = **0**、`冻结为快照` = **0**、`升版冻结义务` = **0**、`引用一律走声明面` = **0**、`页图版本语义` = **0**、`活图夹与冻结快照` = **0**；`冻结快照` = **8** 文件（`261001-openclaw-model-name-overlay-drift/plan.md`、`261013-lang-config-thinking-fix/taskpackage.json` 与 verifier-evidence `r2`–`r5`、`firmware/main/test/acceptance/lang-config-gen.acceptance.md`——均为历史批「配置 / 历史快照」语义，与页图图夹无关；`gov-debt-reduction/task_plan.md` 含该词处 = OQ2 决策记录本身，同根）；**v0.4 新增字面（F3 过渡限定 / N2 v5 起点）复跑 2026-10-27**：`机制落地后生效` = **0**、`落地前维持现行版本夹惯例` = **0**、`docs/ui/V<N>/` = **0**、`ec0804bd` = **0**（批目录外；R2 判官记录内引用不计）、`要点四项自` = **0**；`spike` 另在 `docs/templates/GOVERNANCE-MIGRATION-GUIDE.md` 出现，但指 K9/K1 业务记账，与本规则无关。
- **新判定 pattern 全仓预跑**：指针语义 = 「声明面 `current:` 行 → 文件」；完整性兜底 = 「现行稿 ⊂ 前序稿且缺失 ≥50%」。装配版对真仓（两种声明值）与 11 个夹具跑全量（输出见 §5 B4–B18）：真仓在正确声明下 `ok:true` 无新红；唯一触发形态 = 病态夹具与显式构造的声明面错例。消费方 census 见复证 2（6 处文档消费、0 代码 import）。
- **`debt-render` 消费面**：三条目 status 翻转后 `debt-render.mjs --check` 应报待归位（本批落地前为干净基线，§5 B21），渲染后自动归位；主 INDEX 与归档 INDEX 均为生成件，本批零手改。

**【硬项 · 复扫模式完备性先验】**：本批无退役语面、无重命名、无目录搬家、无路径改落（`path-form-family.md` 的形态族谱无消费面）。「替换型」文本共十处——文档侧五处：`write-story` 第 4 条整句替换、`uiux-author` 第 2 步整行替换（原句前缀保留、后接新纪律）、`uiux-author` 第 3 步整行替换（C-1b，v0.3 新增：出图落点句接丙语义）、`brief` 版本行尾部子句替换、`requirement-change` 4b 子句插入；脚本侧五处（`:6` 后插入、`:10-12` 块、`:13` 追加、`:20` 整行替换、`:36-43` 整块替换）。全部以 §5 V3 的字面 grep 正反配对断言兜底（旧句 = 0 / 新句 = 1；`write-story` 的 `kidpal-ui-design-v3.html` 旧指针 1 → 0；`brief` 的 `](kidpal-ui-design-v6.html)` 旧链 1 → 0）。

## 4. human 取舍点（OQ）

**OQ1 · `ARQ-261024` 的落地形态：文面纪律 vs 机械 gate —— R1 已裁 (a)，本稿按裁落地，不重开**

（a）**文面纪律 + 纪律步（R1 裁定，本稿采用）**：三处文面 + 模板硬项 + planner Step 1i，convention-only、零脚本零新字段。理由（R1 §7 原文）：K4 证据形状显示缺的是**一个相位**（先验证再全量）而非一道检查；机械 gate 会触发 full-adversarial + human，与当前证据强度不成比例；§6 留根 1 已写死复议触发。诚实标注：无机械强制，漏做靠 trio（triage rationale / planner intake / 模板申报）与人类兜底。

（b）**机械 gate 落地**（未采用）：在 `taskpackage-required-check.mjs` 增必填字段（如 `newMechanism: {hit, spikeRef}`）或 `verify-gates.json` 增项；代价 = schema + 脚本 + 夹具 + 存量包兼容策略，且按约定升 full-adversarial + human。收益 = 漏做即红。

（c）**先文面 + 观察期后升机械**：与本稿路线实质相同；复议触发已写死在 §6 留根 1。

**OQ2 · 截图图夹布局：逐版增量 vs 逐版全量 vs 单一活目录 —— human 2026-10-27 已裁丙，本稿按裁落地，不重开**

**裁决（human 2026-10-27 拍板）**：取 **丙（单一活目录 + 冻结快照）**；否 architect 原推荐的甲，亦否乙。human 给出的理由（如实记录，不代编造）：丙的目录最干净（同一 pageId 只有一个现行文件）；旧引用的布局权威不变性有机制保障（升版即冻结上一版为快照，已发出的 `designRefs` 不会被覆盖静默改据）。决策原话记录 = `delivery/workstreams/gov-debt-reduction/task_plan.md` 决策记录节（「P5 批 OQ2 截图图夹布局取丙（单一活目录 + 冻结快照）（否 architect 推荐甲）；本批只落规则句与声明面备注，frozen 快照机制与存量 V5/V6/V7 目录整形另立需求面动作」）。

**本批落法边界（按裁，两步分离）**：本批只落**规则句 + 声明面语义节**——① `uiux-author` Process 第 3 步出图落点句成文：页图落**单一活图夹**、升版**冻结上一版为快照**、引用一律以声明面 `current:` 为准（T2.3-C1b；规则句含过渡限定——机制落地后生效，落地前维持现行版本夹惯例，见 v0.4 F3 收口）；② 声明面 `docs/ui/UI-DESIGN-VERSIONS.md` 补「活图夹与冻结快照」语义节（T2.1）。**另立需求面动作**（本批明确不做）：frozen 快照机制本体、活图夹目录整形、存量 `docs/ui/V5|V6|V7/` 页图迁移与按丙语义的新出图落点 → 本批零搬图、零新增图夹、PNG 增删不进 manifest；旧包 `designRefs` 不追溯改（三案通例，维持）。留根登记见 §6 留根 4。

**三候选原文（保留供评审追溯；甲 / 乙 = 落选记录，丙 = 裁定采用）**：

现状：`docs/ui/V5/`（12 张）、`V6/`（1 张）、`V7/`（3 张）三夹并存，每批只落变更帧；`designRefs[]` 现存引用形如 `docs/ui/V5/canvas-p1.png`（示例见 `.claude/references/task-intake-and-slicing.md:348`）；`uiux-author` 定义载「PNG 文件名对齐 pageId/stateId 落 `docs/ui/`、PNG 是 builder 布局坐标的定位权威」，`requirement-change.md:45` 亦写「回填 PNG 落 `docs/ui/`」——**两处文字都不带版本夹**，与实况已有轻微不一致。三个候选：

- **候选甲（落选 · 现状维持 + 声明面补语义；architect 原推荐）**：图夹继续逐版增量，声明面在每个版本备注里列明该版变更帧；`designRefs[]` 继续指 `docs/ui/V<N>/<pageId>.png`；旧包引用不追溯。代价 = 单看目录不完整（须读声明面才知道「V7 只有 3 帧」是设计意图），新引用无「整版全量」保证。兼容 = 零改动，frozen 语义天然成立（版本夹一旦落就不再改）。
- **候选乙（落选 · 逐版全量快照）**：每版图夹 = 该版全部 400×300 画布 / 状态 PNG（v7 应含全量帧），声明面记「全量 N 帧」。代价 = 仓体积每版约 +60KB（可接受）、与现行「每批只落变更帧」出图定例冲突（需改惯例为每批全量重出）、旧包引用不追溯。收益 = 目录自明、任意版本夹可独立作布局权威。新引用统一 `docs/ui/V<current>/…`。
- **候选丙（裁定采用 · 单一活目录 + 冻结快照）**：`docs/ui/pages/<pageId>.png` 为现行面（覆盖式更新），冻结包经 `designRefs` 指 `docs/ui/pages/_frozen/<changeId>/…`。代价 = 两族引用并存、需为「frozen 页图」新增快照机制（否则覆盖会静默改历史包依据——而 designRefs 作布局权威正依赖这条不变性）；迁移成本最高。收益 = 同 pageId 单一现行文件。**注**：丙的目录形态（`docs/ui/pages/` + `_frozen/`）是候选描述；具体目录名与机制本体归另立需求面动作定，本批规则句只钉语义、不钉路径。

**技术取舍（已给理由，判官可 challenge；不是 human 拍照项）**：

- **取舍 A · spike 不判 ⓪ nano**：nano 的排除清单含「意图需要新行为验收语义承载 → 升 Lite」（`change-triage.md`「档 0 nano 判据」节），spike 天然命中；写进 rationale 可防把 spike 拆成无验收的微包。
- **取舍 B · spike 独立成包 + 新增 `spike-evidence.md`（惯例非门）**：独立成包让「先验证后开发」在切片序上可见、可阻塞；实证记录单独落文件便于正式实现包引用。**退路**：判官若认为应并入同包首切片，把 T1② 的产物句改为「落本包 `plan.md` 技术方案节 + review-packet」，其余不动。
- **取舍 C · 落 `planner.md` Step 1i（超出 ARQ 原三条 paths）**：理由见 T1 段末（无 intake 消费方的纪律没有执行面）。**退路**：判官若判超面，撤第 4 件与 ARQ append 里的对应句，T1 退化为「判档必答 + 定义 + 模板硬项」——代价是纪律只在 triage 与文面层存在。
- **取舍 D · 完整性判据用「相邻前序版本 id 集」而非 brief 的声明画布清单**：brief 的**设计稿版本声明**已实测漂移（复证 2 / 4），制品自身继承关系无第二源。本稿进一步把它降为**兜底层**（主判据 = 声明面指针）。**退路**：若判官要求以 brief 为完整性真值，须同时解决 brief 声明维护（另一件事，会扩大 manifest）。
- **取舍 E · 阈值 `≥50%` / 前序锚数 `≥10` / exit 2 / `--allow-master-shrink`**：阈值按真实演进史标定（合法页撤 9.8% vs 病态 83%，阈值到两者的距离 5.1 倍 / 1.7 倍，中间无观测样本——如实记「保守取值」而非「拟合」）；exit 2 复用「输入不完整」语义。**退路**：判官可调阈值或把完整性失败降为 warn——但不推荐 warn-only（本批复现的假绿形态里 warn 同样会被忽略）。
- **取舍 F · `write-story` 指针改指声明面（而非「最高版号」）**：v0.1 曾写「版本无关形态 = 最高版号」，经复证 4 判定它就是病灶源之一（版本号是写者自报的），v0.2 改为指声明面。**退路**：判官若认为 SKILL 不应引入第二个路径依赖，可恢复「版本无关」措辞并改为「由机检解析，本行不钉版本」——代价是读 SKILL 的人看不到单一来源在哪。
- **取舍 G · `requirement-change.md` 4b 加一句指针（触及需求面 SSOT）**：human 立项追加明写「出图、截图引用一律以声明为准」，而 4b 是出图流程的 SSOT 落点；只改一句、零流程语义。**退路**：判官若判跨面越界，撤该件（manifest 13 → 12），纪律只在 `uiux-author`（执行者）与声明面 cabeza 承载——代价是出图流程的正典文面看不到版本来源。
- **取舍 H · 声明面失败一律 fail-closed（exit 2）、不提供「无声明面则回退最高版号」的兼容开关**：回退开关等于把本批治的病（隐式推断）保留为默认路径。**退路**：若判官认为并行分支 / Pod 过渡期需要兼容，可加一次性 `--allow-legacy-max-version` 旗——但本批不预置（YAGNI，且会稀释「声明是唯一来源」）。
- **取舍 I · 「存在更高版号」只 stderr 提示、不改退出码**：见 T2「为什么这样改」。**退路**：判官可要求升级为 exit 1 或去掉提示；本稿选提示是因为它能教会正确动作而不夺回判据权。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律（T 自守）**：脚本面预期值 = 「按本稿 §2 T2.2 的编辑 A0/A/B/C/D/E 逐字节装进 pristine 副本」的实跑输出（副本在仓外 `C:/tmp/p5-assembly`，装配脚本从 §2 文本装配、不手抄；装配脚本本体在 `_scratch/`，起草结束后删除）；文档面预期值 = **处方正推 + 仓外副本实跑**（把 §2 的插入 / 替换文本施加到目标文件副本，逐字面 `grep -cF` 计数；基线数字均为起草期实跑）。落地期实跑若与本表有字节差异，以落地实跑为准覆写 §5 并在 §7 留痕。
> **时点定义**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。
> **夹具配方**（任何人可重建，仓外 `_scratch/make-fixtures.mjs` 生成）：临时目录按「夹具名」生成 `docs/requirements.md`（两条 story 锚行 `UI = \`canvas-p1\`` 与 `UI = \`state-a\``，支撑锚 `arch §1.1`）+ `docs/architecture.md`（`## 1.1 fixture section`）+ `docs/ui/` 下设计稿与声明文件。设计稿 id 集：`base12` = `canvas-p1` / `state-a` / `d-01`…`d-10`。变体：**A** v1(12) → v2(12，换 1)，声明 = v2；**B** v1(12) → v2(2)，声明 = v2；**C** v1(12) → v2(11，掉 `canvas-p1`)，声明 = v2；**D** 只有 v1，声明 = v1；**E** v1(12) + v2(11，掉 `canvas-p1`)，声明 = **v1**（指针语义判别夹具）；**F** 与 A 同文件、**无声明文件**；**G** v1 + v2，声明 = v3（指向不存在）；**H1** 声明无 `current:` 行；**H2** 声明两条 `current:` 行；**H3** 声明指 `kidpal-ui-design-v2-partial.html`（非母版命名）；**H4** 声明值带反引号。全部用 `CLAUDE_PROJECT_DIR=<夹具目录>` 跑。

**基线（现行 HEAD，起草期 2026-10-27 实跑，本 gov worktree + 仓外夹具）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node .claude/scripts/spec-anchor-check.mjs`（**现行脚本**，真仓） | `{"specAnchorCheck":"forward","design":"kidpal-ui-design-v7.html","designIdCount":64,"archSectionCount":48,"storyCount":14,"anchorLines":36,"errors":[],"ok":true}`，exit 0 |
| B2 | 现行脚本 + 夹具 B（病态塌缩 12 → 2，锚恰好未断） | `{"design":"kidpal-ui-design-v2.html","designIdCount":2,…,"errors":[],"ok":true}`，**exit 0（假绿实证）** |
| B3 | 现行脚本（最高版号 wins）+ 夹具 E（应选 v2 并报错） | `design: kidpal-ui-design-v2.html`、`designIdCount: 11`、`errors: ["S-F1.1（L4）：UI 锚 \`canvas-p1\` 在现行设计稿 kidpal-ui-design-v2.html 无对应 data-od-id"]`，**exit 1** |
| B4 | 装配版 + 夹具 A（正常演进 12 → 12，缺 1，声明 = v2） | `ok:true`、`designIdCount:12`，exit 0 |
| B5 | 装配版 + 夹具 B（病态塌缩，声明 = v2） | stderr 逐字：`[spec-anchor-check] FATAL（母版完整性）: 现行设计稿 kidpal-ui-design-v2.html 仅 2 个 data-od-id，前序 kidpal-ui-design-v1.html 有 12 个（缺失 10 个、占 83%）——疑似增量稿占用母版命名。kidpal-ui-design-vN.html 命名族 = 完整母版（纪律见 .claude/agents/uiux-author.md）：增量/局部稿用临时命名（如 kidpal-ui-design-v<N>-partial.html），母版升版号与内容完整同一次落盘。本检查已中止，以免把母版缺口误报成 story 锚漂移；若确为有意的整版收缩，带 --allow-master-shrink 复跑。`，**exit 2**，stdout 空 |
| B6 | 装配版 + 夹具 C（单点收缩 12 → 11） | `ok:false`、`errors` 与 B3 同形（1 条 `canvas-p1` 悬空），exit 1（不回落到母版完整性判定） |
| B7 | 装配版 + 夹具 D（无前序版本，声明 = v1） | `ok:true`、`design: kidpal-ui-design-v1.html`、`designIdCount:12`，exit 0（完整性自查跳过） |
| B8 | 装配版 + 夹具 E（声明 = v1、v2 存在） | `ok:true`、`design: kidpal-ui-design-v1.html`、`designIdCount:12`，exit 0；stderr = `[spec-anchor-check] 提示: docs/ui/ 存在更高版号 kidpal-ui-design-v2.html，但声明面 current = kidpal-ui-design-v1.html——以声明为准；若属升版遗漏，请同一次更新声明面（形质兼备再升版，见 .claude/agents/uiux-author.md）。` |
| B9 | 装配版 + 夹具 F（声明文件缺席） | stderr `[spec-anchor-check] FATAL（设计稿版本声明）: 声明文件不存在（…\docs\ui\UI-DESIGN-VERSIONS.md）——…`，**exit 2**、stdout 空 |
| B10 | 装配版 + 夹具 G（指针 → 不存在的 v3） | stderr `FATAL（设计稿版本声明）: current 指针「kidpal-ui-design-v3.html」指向的文件不存在——…`，**exit 2** |
| B11 | 装配版 + 夹具 H1 / H2（`current:` 0 行 / 2 行） | stderr `FATAL（设计稿版本声明）: current 行应恰一行，实得 0 行` / `…实得 2 行`，各 **exit 2** |
| B12 | 装配版 + 夹具 H3（指针不合母版命名族） | stderr `FATAL（设计稿版本声明）: current 指针「kidpal-ui-design-v2-partial.html」不属母版命名族（kidpal-ui-design-vN.html）——…`，**exit 2** |
| B13 | 装配版 + 夹具 H4（声明值带反引号） | `ok:true`、`design: kidpal-ui-design-v2.html`，exit 0（容忍形态） |
| B14 | 装配版 + 夹具 B + `--allow-master-shrink` | `ok:true`、`designIdCount:2`，exit 0（逃逸旗生效，走普通锚校验） |
| B15 | 装配版 + 夹具 E `--reverse` / 夹具 F `--reverse` | E：stderr 提示同上 + `{"reverse":[{"archSection":"§1.1",…,"referencedBy":["S-F1.1","S-F1.2"]}],"storyCount":2,"anchorLines":2}`，exit 0；F：`FATAL（设计稿版本声明）`、exit 2（**如实标注的行为变化**：逆视图也要求声明面在场，输出结构不变） |
| B16 | 装配版 + 夹具 F `--diff-ids v1 v2` | 正常 diff（`oldCount:12 newCount:12`、`removed:["d-10"]`、`added:["d-11"]`），exit 0（**不读声明面**） |
| B17 | 装配版 + 真仓副本（声明 `current: kidpal-ui-design-v7.html`） | 与 B1 **逐字相同**（`design v7` / 64 / 48 / 14 / 36 / `ok:true`），exit 0 |
| B18 | 装配版 + 真仓副本（声明临时改 `current: kidpal-ui-design-v6.html`） | stderr 提示「存在更高版号 kidpal-ui-design-v7.html」；stdout `design: kidpal-ui-design-v6.html`、`designIdCount:61`、`ok:true`，exit 0（声明为准 + 提示可见） |
| B19 | 锚唯一性（§2 全部编辑的原文锚，`split(anchor).length - 1`） | 文档侧 22 处（triage 1 / dev-loop 2 / planner 1 / 模板 1 / uiux 2（C-1 + C-1b）/ write-story 1 / brief 1 / requirement-change 1 / 三条目各 4 = 12）+ 脚本侧 6 处（A0 / A / B / C / D / E）= **28 处全部 = 1**（仓外装配脚本 fail-loud 保证：锚数 ≠ 1 即中止） |
| B20 | 新字面碰撞基线（单根 `grep -rlF`，排除本批目录） | 见 §3 硬项逐项（`UI-DESIGN-VERSIONS`/`designDeclarationFatal`/`DESIGN_DECL_PATH`/`DESIGN_MASTER_RE`/`设计稿版本声明`/`设计稿版本以声明面`/`形质兼备`/`^current:` 全 0；`母版完整性` 1 文件；`新机制批` 1 文件；`spike-first` 4 文件；`以声明为准` 1 文件 = 立项追加记录本身；v0.3 新增字面：`活图夹`/`单一活图夹`/`布局权威`/`冻结为快照`/`升版冻结义务`/`引用一律走声明面`/`页图版本语义`/`活图夹与冻结快照` 全 0，`冻结快照` 8 文件均为历史批异义散文；v0.4 新增字面：`机制落地后生效`/`落地前维持现行版本夹惯例`/`docs/ui/V<N>/`/`ec0804bd`/`要点四项自` 全 0） |
| B21 | `node .claude/scripts/debt-render.mjs --self-check`；`… --check` | `debt-render --self-check: ok (10/10)`；`--check` exit 0（基线干净，无待渲染差异） |
| B22 | 行尾与语法（13 件） | `spec-anchor-check.mjs` CRLF 原 197 行 → 装配版 CRLF **265 行**、无裸 LF、`node --check` 通过；`write-story` / `ARQ-261017` / `ARQ-261024` / `UIUX-DESIGN-BRIEF` 为 CRLF；`planner` / `uiux-author` / `change-triage` / `dev-loop` / `requirement-change` / `PROPOSAL.template` / `ARQ-260908` 为 LF；**新文件 `UI-DESIGN-VERSIONS.md` 用 LF**。落地须逐文件保持行尾 |
| B23 | 文档副本施加后字面计数（仓外） | `1i. **新机制批 spike 前置核`：planner = 1；`新机制 spike 申报`：模板 = 1；`母版命名族与版本声明纪律`：uiux = 1；`](../../docs/ui/UI-DESIGN-VERSIONS.md)`：write-story = 1；`版本更迭不需要改本行`：write-story = 1；`kidpal-ui-design-v3.html`：write-story = **0**；`-partial.html`：uiux = 1；`非新机制批`：triage = 1、planner = 1；`以声明面 [UI-DESIGN-VERSIONS.md]`：brief = 1；`](kidpal-ui-design-v6.html)`：brief = **0**；`设计稿版本以声明面`：requirement-change = 1；**v0.3 新增（OQ2 丙裁）**：`单一活图夹`：uiux = 1、声明面 = 1；`冻结为快照`：uiux = 1、声明面 = 1；`活图夹与冻结快照`：声明面标题 = 1；`^current:`：声明面 = **1**（新节无行首 `current:`）；**v0.4 新增（F3 过渡限定 / N2 v5 起点）**：`机制落地后生效；落地前维持现行版本夹惯例`：uiux = 1、声明面 = 1；`docs/ui/V<N>/`：uiux = 1、声明面 = 1；`ec0804bd`：声明面 = 1；`2026-09-22 → 2026-10-12`：声明面 = 1；声明面仍 **31 行正文** |
| B24 | 装配脚本字面计数（脚本面） | `DESIGN_DECL_PATH` = 4；`DESIGN_MASTER_RE` = 4；`designDeclarationFatal` = 5；`previousDesignFile` = 2；`MASTER_SHRINK_MIN_RATIO` = 2；`MASTER_SHRINK_MIN_PREV_IDS` = 2；`--allow-master-shrink` = 3；`母版完整性` = 5 行；`FATAL（母版完整性）` = 1；`UI-DESIGN-VERSIONS.md` = 5 |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填 / 正推） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/spec-anchor-check.mjs`（落地后脚本 + 落地后声明面，真仓） | 与 B17 / B1 逐字相同，exit 0 | 反转实现（不读声明面、按最高版号）在夹具 E 下 exit 1（B3）、穷塌缩夹具 B 下假绿 exit 0（B2）——两条断言在反转实现下必红 |
| V2 | pre-commit | 夹具 A–H4 + `--allow-master-shrink` + `--reverse` + `--diff-ids`（落地后脚本，配方见上） | A / D / E / H4 exit 0；B exit 2 且 stderr 与 B5 逐字同；C exit 1 且 errors 与 B6 同形；F / G / H1 / H2 / H3 exit 2 且归因点名各自形态；`--allow-master-shrink` exit 0；`--reverse`（E）结构视图 / （F）exit 2；`--diff-ids`（F）exit 0 | 与 B2 / B3 对拍（同一夹具两版脚本结论相反）；C 证明单点收缩不误判母版；F/G/H 三族证明声明面失败行为与报文归因可区分 |
| V3 | pre-commit | 文档字面正推（`grep -cF`，命令见 §3；文件 = manifest 十三行） | 逐条 = B23 列值（新字面由基线 0 变指定值；`write-story` 的 `kidpal-ui-design-v3.html` 由 1 变 **0**；`brief` 的 `](kidpal-ui-design-v6.html)` 由 1 变 **0**；声明面 `^current:` 恰 1、且指针文件在盘） | 正向：每个新句至少一条；负向：被替换旧句 0 残留；配对见 V4 |
| V4 | pre-commit | 条目 frontmatter 正反配对：`grep -cF "status: deferred"` 三条目 = **0**、`grep -cF "status: closed"` = **1**（各）；`grep -cF 'closed: ""'` = 0、`grep -cF "closed: 2026-10-27"` = 1（各）；`closedBy` 同法 | 逐条成立 | 负向（旧值 0）× 正向（新值 1）配对，防「只 append 不翻转」 |
| V5 | pre-commit | `node .claude/scripts/debt-render.mjs --check` | **非 0**：报盘上 ≠ 应渲染（三条目待归位 + 两处 INDEX 待更新）——这是翻转后的预期中间态 | **未实测**（依赖落地翻转）；渲染后转 V8 |
| V6 | pre-commit | `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md server/ firmware/ tools/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json .claude/scripts/pre-merge-escalation-gate.mjs AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md delivery/archive/ docs/debt/gov/INDEX.md docs/debt/gov/_archive/` | 空输出 | 负向（禁触面零 diff，含 `tools/` 与生成 INDEX）；正向配对 = `git diff --name-only` 恰等于 manifest 十三行（debt-render 之前），且 `git diff --name-only -- docs/ui/` ⊆ {`docs/ui/UI-DESIGN-VERSIONS.md`, `docs/ui/UIUX-DESIGN-BRIEF.md`} |
| V7 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-p5-spike-anchor-cluster/PROPOSAL.md` | `ok:true`、`anchorCount` = 13、`anchorUnion` 与 manifest 十三行逐条一致、`findings:[]`、`advisories:[]` | 起草期已实跑（见文末），落地后复跑同值 |
| V8 | post-commit | `node .claude/scripts/debt-render.mjs`（渲染归位）→ `--check` → `--self-check` | 三条目落 `docs/debt/gov/_archive/2026/`；主 INDEX 该三行转为「已结」指针、归档 INDEX 出现薄行；`--check` exit 0；`--self-check: ok (10/10)` | 归位是 `status` / `closed` 的纯函数；V5 的中间态在此收口 |
| V9 | post-commit | 复跑 V1 / V2 / V3 / V4 | 与上表逐值相同 | **未实测**（依赖落地提交）；由主线程 / landing-verifier 执行 |
| V10 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261027-gov-review-p5-spike-anchor-cluster --source-ref HEAD` | `PASS (…, kind=gov)`，exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff，契约凭据分支不触发 |
| V11 | pre-commit（主线程 chore） | `node .claude/scripts/changes-index.mjs --upsert 261027-gov-review-p5-spike-anchor-cluster` | `delivery/review/INDEX.md` 出现该 id 行、state 缓存同步；须在落地 commit 之前跑（跑晚了只静默漏带生成件） | 生成件，禁手改 |

## 6. 留根清单

**本批结项（landing 后由主线程执行登记 / 状态变更；architect 不做）**：`ARQ-261024-new-mechanism-spike-first`（整条结项；未机械化的部分见留根 1）、`ARQ-261017-anchor-check-master-unvalidated`（整条结项；图夹机制与存量迁移见留根 4）、`ARQ-260908-versioned-guard-repoint-execution`（整条结项、纯记账）。

**留根与复议触发**：

1. **spike 规则暂无机械强制**（OQ1 路线 = 文面纪律）：漏做靠 triage rationale / planner Step 1i / 模板申报与人类兜底。复议触发 = 出现第一例「判为命中但没做 spike 就进全量开发」的包（任何角色发现），或连续两轮 retro 出现新机制类返工；届时按 §4 OQ1(b) 评估机械 gate 并升 full-adversarial + human。
2. **`spike-evidence.md` 不进聚合器、不进 closeout 必交**：惯例非门；复议触发 = 连续两个引用它的正式实现包缺失该文件。
3. **完整性自查是启发式**：塌缩 < 50% 的母版缺口仍会以 story 锚漂移形态出现（本批不改锚错误文案）；且它现在只是**兜底层**——主判据是声明面。复议触发 = 出现「塌缩 < 50% 但实为增量稿」的实例，届时评估把归因提示加进锚错误行。
4. **截图目录整形 + frozen 快照机制 + 存量迁移（OQ2 丙已拍，另立需求面动作）**：页图语义（单一活图夹 + 升版冻结快照 + 引用一律走声明面 current）已在本批成文（T2.1 语义节 + T2.3-C1b）；**未落**的是机制本体与整形——frozen 快照的目录形态与生成流程、活图夹目录名、`docs/ui/V5|V6|V7/**` 存量页图迁移、以及按丙语义的新出图落点。**发起时机判据**：下一次触 UI 的出图批到来之前必须先把活图夹定下来（否则新 PNG 无落点、出图规格单无法执行），届时按**需求面**动作承载（出图流程归 `/requirement-change` 面 loop 档 2 4b）或独立 intake 立批。本批零动 `docs/ui/V5|V6|V7/**` 与 `designRefs` 形态。复议触发 = 出现「新出图无活图夹可落」的实例，或下一次触 UI 批时图夹实况与声明面语义节不一致。
5. **两处陈旧版本引用留根**：`tools/asset-pipeline/verify_art_assets.mjs:14` 硬编码绝对路径 `…/kidpal-ui-design-v3.html`（工具面，`tools/**` 属 dev-loop 面）；`docs/ui/PHASE1-DESIGN-INPUT.md:5` / `:128` 仍称「v4 = 最新母版」。两处建议主线程登记债项；复议触发 = 下次触资产管线（工具面）或下次设计批。
6. **不吸收 `ARQ-261019-opendesign-shoot-script-precedent`**：母版 / 增量页**写者分工**与出图管道归它（条目 trigger 写「uiux-author 重派前落地」）；本批只钉**命名族 = 完整母版**、**声明面 = 唯一版本来源**与**落盘原子性**，三者叠加不冲突。复议触发 = 261019 落地时逐句核措辞有无双源。
7. **不合并 `ARQ-260918-spec-face-3page-reconcile`**（tech 域，同工具不同根）：该条治 `docs/architecture.md` / `docs/requirements.md` 的叙述与实现漂移，仅**引用**本脚本作验证手段；本批治脚本判据自身缺口。互查咬合 = 该条落地时，"改后无悬空锚"的验证会经本批加固后的脚本（且该验证现在也需要声明面在场——本批落地即已提供）；本批零改两文件，不预支其改动。复议触发 = 两批改动意外落在同一文件时。
8. **260908 结项后无后续重指义务**：若未来再出现「版本钉死接线断言」，按 `acceptance-evidence.md` 的 versioned-guard 规约新立条目，不复活本条目。
9. **归档移动不在 manifest**：三条目落地后由 `debt-render.mjs` 归位 `docs/debt/gov/_archive/2026/`；landing-verifier 按最终路径核内容（生成动作不是手写，manifest 记落地编辑路径）。

**环内自律**：起草期有仓外装配副本（`C:/tmp/p5-assembly`：脚本装配产物 + 11 个夹具 + 真仓副本 + 12 份文档副本），全部不进仓、不 stage；§2 的脚本与文档改法以装配实跑为准回填 §5。仓内 `delivery/review/261027-gov-review-p5-spike-anchor-cluster/_scratch/` 的装配脚本起草结束后删除（不随批 commit）。

## 起草期自检留痕（模板四句自检的落地记录，2026-10-27，v0.5 复跑）

1. **preflight 实跑**（只读干跑，V7）：见本节末「preflight 输出」块。

2. **路径形态族谱**（`path-form-family.md`）：本批**新增一个路径**（`docs/ui/UI-DESIGN-VERSIONS.md`，新文件、不搬家、不改名其它路径）；新增的规则小节名（「新机制批判定」「新机制批 spike 相位」「机读声明」「活图夹与冻结快照」）、命令旗（`--allow-master-shrink`）、常量与函数名（`MASTER_SHRINK_*` / `DESIGN_DECL_PATH` / `DESIGN_MASTER_RE` / `designDeclarationFatal` / `previousDesignFile`）、制品名（`spike-evidence.md`）与话术（「母版完整性」「设计稿版本声明」「活图夹」「单一活图夹」「冻结快照」、v0.4 新增的过渡限定语〔`机制落地后生效` / `落地前维持现行版本夹惯例` / `docs/ui/V<N>/`〕与 v5 起点 commit `ec0804bd`）已按 §3 硬项做碰撞 census（B20）。

3. **承重引文 byte 对账**：§2 的每处被替换原文 / 插入锚，起草期以装配脚本 `split(anchor).length - 1` 逐条核过恰 1 次（B19：28 处全部 = 1，锚不匹配当场失败）；文档侧另把改后文本施加到副本再计数（B23），脚本侧实跑 11 个夹具与真仓两态（B4–B18）。**未手抄长输出**：脚本装配文本由装配脚本从本稿 §2 的围栏块逐字节构造，文档装配文本同法。

4. **长文落地自检**：本文件写入后机检 = `wc -l` = 831、U+FFFD 替换字符扫描零命中、`### T<n>` 标题计数 = 3、`目标文件：` 锚行计数 = 3、§2 围栏内「`目标文件：` 形锚行」计数 = 0（防幻影条目）；§2 围栏内另有 **4 条以 `## ` 起头**的行——那是声明面文件内容自带的二级标题（`## 机读声明` / `## 逐版备注` / `## 现行指针的依据记录` / `## 活图夹与冻结快照`），解析侧跳围栏已承载（261026 批修复），实测佐证 = preflight `anchorUnion` 13 项与 manifest 逐条一致、`findings:[]`。声明面围栏块与装配产物逐字节一致（`sha256 = 7bdf3851…`，v0.5 复核）；`uiux-author` C-1b 替换行与装配产物逐字节一致（`P.includes` 断言为 true，两侧过渡限定各 1 次）。§7 R1 / R2 / R3 判官记录逐字未动（sha256：R1 = `edcadacf…`、R2 = `2d1e2c82…`、R3 = `82019ba8…`，见「修订记录」）。

**preflight 输出**：见下方（落地期复跑同值）。

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-p5-spike-anchor-cluster/PROPOSAL.md
{
  "ok": true,
  "gate": "landing-preflight",
  "proposal": "delivery/review/261027-gov-review-p5-spike-anchor-cluster/PROPOSAL.md",
  "section2BodyFound": true,
  "anchorCount": 13,
  "anchorUnion": [
    ".claude/agents/planner.md",
    ".claude/agents/uiux-author.md",
    ".claude/references/change-triage.md",
    ".claude/references/dev-loop.md",
    ".claude/references/requirement-change.md",
    ".claude/scripts/spec-anchor-check.mjs",
    ".claude/skills/write-story/SKILL.md",
    "docs/debt/gov/ARQ-260908-versioned-guard-repoint-execution.md",
    "docs/debt/gov/ARQ-261017-anchor-check-master-unvalidated.md",
    "docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md",
    "docs/templates/PROPOSAL.template.md",
    "docs/ui/UI-DESIGN-VERSIONS.md",
    "docs/ui/UIUX-DESIGN-BRIEF.md"
  ],
  "advisories": [],
  "findings": [],
  "note": "只读干跑：只保证 §2 锚清单「形态可解析」，不做 §2↔manifest 对账；不参与任何门判定。"
}
exit=0
```

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-27 - architect

**轮次**：R1 全面审（Track B 评审循环，零上下文判官实例）。只读全仓 canonical，唯一写入 = 本节 append。

**判档裁定**：预判 Track B 主路径成立（确认，无升档事由）。引证：`.claude/references/gov-change.md:18` 轻路径判据③——「不触 `.claude/commands/**`（行为入口）、`.claude/agents/**`、`.claude/hooks/**`、`.claude/scripts/**`、…——**命中任一 → 恒 Track B，哪怕只改一个字符**」。本批 T2 触 `.claude/scripts/spec-anchor-check.mjs`、T1④ 触 `.claude/agents/planner.md`，机器面命中恒 Track B。Track A 段两条判据均不命中：非跨系统结构性变更；方向已收敛（三条 ARQ 预消化 + human 2026-10-27 立项 ack「两条并批 Track B 主路径」）。

**OQ1 裁定（评审裁定位）**：取 **(a) 文面纪律 + planner Step 1i + 模板硬项**。理由：① K4 证据形状显示缺的是**一个相位**（先验证再全量）而非一道检查——五层问题没有一层是「有检查就能拦住」的形态；② ARQ-261024 形态注意如实预告机械 gate → full-adversarial + human，与当前证据强度不成比例；③ §6 留根 1 已写死复议触发（首例「判命中未做 spike 就进全量开发」或连续两轮 retro 出现新机制类返工 → 按 OQ1(b) 重评并升 full-adversarial + human）。本裁定不剥夺人裁权：human 例外反选 (b)/(c) 时按约定升档。分叉如实给到评审位、无预裁——本项通过。

**manifest 扩面审查**（相对 ARQ 原始 paths 的三件扩面，逐件核）：① `.claude/agents/uiux-author.md` 与 ② `.claude/skills/write-story/SKILL.md` = `ARQ-261017` 建议改动 ② 原文点名的两个落点（「在设计面纪律文面（`uiux-author` agent 定义 / write-story 相关节）明写…命名族 = 完整母版」）——非越界，是条目自身处方的落地；③ `.claude/agents/planner.md` = `ARQ-261024` 落点候选 (b)「`/build-feature` plan/launch 阶段」的 intake 承载具体化（planner 是每包 intake 必经点），已在取舍 C 显式标注理由与退路（撤第 4 件则 T1 退化为「判档必答 + 定义 + 模板硬项」）。三件均在 ARQ 处方语义内、已显式标注、可撤——不需升 human。

**findings**：

- **F1 · SHOULD · §2 T2 编辑 B/C/D 行号引用漂移（三处）**。判官实测：编辑 B 称真空绿规约句在「`:15`」——`grep -n "真空绿规约" .claude/scripts/spec-anchor-check.mjs` = **:13**；编辑 C 称退出码行「`:24` 整行替换」——实测 = **:20**；编辑 D 称 `return files[0];` / `}`（「`:46-47`」）——实测 = **:42-43**。同稿编辑 A「原文第 10–12 行」与编辑 E「`:169`/`:170`」实测正确，`grep -n "function currentDesignFile"` = 37 亦与复证 2 一致——漂移仅 B/C/D 三处孤立，非系统性位移。同一错误行号复现于 §3 复扫段括注（「`:10-12` 块、`:15` 追加、`:24` 整行」）。判言：§2.0 复证 2 刚以「仅行号整体下移」更正过 ARQ 原条目的同类漂移，本稿自身新引用不得带漂移进 FROZEN。影响评估：无机械消费面——落地按逐字节文本锚执行、B10 fail-loud（判官独立装配复现：五处编辑锚全部恰 1 次命中、`node --check` 过），不属 BLOCKING；但按 arch-review-protocol「严重度 → 可冻结性」未结 SHOULD ⇒ 不可冻结。**修法**：编辑 B `:15`→`:13`、编辑 C `:24`→`:20`、编辑 D `:46-47`→`:42-43`，§3 括注同步更正；围栏块文本零改动。R2 聚焦核该三处即可。
- **N1 · NIT（不阻断）**：T2「为什么这样改」称 50% 阈值与真实分布「有一个数量级断层空档」——实测 9.8%→50% 为 5.1 倍、50%→83% 为 1.7 倍，「数量级」修辞偏松；actionable 判据（50% 分离合法页撤与病态塌缩）成立，不必改；修订 F1 时顺手可收紧措辞。

**本批特有焦点逐项裁定**：

- **261024 措辞红线**：spike 定义 spine 第 1 条 = 「消费端真实观测，非生产者自证」在场（T1①「写明其**消费端验收观测**（不是生产者自证）」+ T1② spike 定义 1）；勘误①（被动窗口 → spike 定义 2「新结论必落在被动窗口内」/ 观察视角配置 → 四问①「成员与观察视角配置」）、勘误②（Gate E E1–E5 引用为断言形态先例）、勘误③（「单文件试灌会假通过」→ spike 定义 2）均吸收。通过。
- **T3 记账保真**：判官逐件实测——A.3 在册 8 行、待重指 #3/`test_schedule_p2:584-587`、#6/`test_voice_roundtrip_b:509-512`、#7/`test_char_stroke:683-685`/`:722-724`、#8/`PROMPT_CARRIER_V2:144`（`delivery/archive/changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md` :294/:297/:298/:299 起）；commit `77b6b0cf`（2026-09-16，三份被引文件实测移入 `server/acceptance/archive/`）；`server/pytest.ini:6` `norecursedirs = … archive`；live 套件 grep 0、archive 内 77/24/33 命中；继任 `test_prompt_v7_round_routing.acceptance.py` PN-A5/RF-A5/PV-A8「live 观测项（非自动断言…）」声明在 `:36`/`:75`/:115、v7/v9/v10/v11 逐版钉在场——与注记逐项吻合。注记明标「纯记账、未执行任何重指」，无「结项记账偷写成重指执行」；human 逐件授权如实标注为携带记录、不列为可机检断言——诚实承载。通过。
- **261017 × `ARQ-260918-spec-face-3page-reconcile` 边界**：互查咬合写清（§6 留根 6 + T2⑧ append：该条治 spec 叙述漂移、仅引用本脚本作验证手段，其落地验证会经本批加固后的脚本），无越界合并；本批 spec 两文件零 diff（V6 负向清单含 `docs/requirements.md` / `docs/architecture.md`）。通过。
- **T2 判据加法性质与夹具判别力**：不挂 hook、不新增门、`verify-gates.json`/`hooks`/`settings`/`scope-map` 零触碰（V6 负向清单覆盖）；exit 2 复用既有「输入缺失」语义档；阈值机械可判（前序锚数 ≥10 且缺失比例 ≥50%）。夹具判别力判官实测：**反转实现（现行脚本）在夹具 B 下 `ok:true` exit 0（假绿实证）vs 装配版 exit 2**——反转变红成立；夹具 A（正常演进 12→12 缺 1）ok:true、夹具 C（单点收缩）exit 1 且只报锚错不误判母版、夹具 D（无前序）跳过、`--allow-master-shrink` 生效 exit 0——逐条与 §5 B3–B7 一致。通过。

**亲核留痕**（判官 2026-10-27 独立实测，不凭转述采信）：

- 基线复现：B1 真仓 `{"design":"kidpal-ui-design-v7.html","designIdCount":64,"archSectionCount":48,"storyCount":14,"anchorLines":36,"errors":[],"ok":true}` exit 0；B2 现行脚本 + 病态夹具 `ok:true` exit 0（假绿实证复现）；判官按 §2 编辑 A–E 从头装配副本（5 处锚各恰 1 次、`node --check` 通过、CRLF 233 行）后 B3/B4/B5/B6/B7/B8/B9 逐条复现——B5 stderr 与 §5 表逐字一致（含「缺失 10 个、占 83%」）exit 2、stdout 空，B6 errors 逐字一致 exit 1，B9 与 B1 逐字相同。
- 文档装配复现：以仓内 `_scratch/assemble-docs.mjs` + `check-consistency.mjs` 独立重跑——19 处文档编辑锚全部恰 1 次（ALL DOC EDITS OK）、双向一致性 ALL-CONSISTENT（新增行全在 PROPOSAL、围栏块整块全在副本）；B13/B14 字面计数逐项相符（含 `kidpal-ui-design-v3.html` write-story = 0）。
- census/结构：B11 新字面碰撞 5 字面 0 文件、`母版完整性` 1 文件（261017 retro）、`新机制批` 1、`spike-first` 4——文件集与 §3 逐项一致；B12 `debt-render --self-check: ok (10/10)` + `--check` exit 0；V7 preflight `ok:true`、`anchorCount` 10、`anchorUnion` 与 manifest 十行逐条一致、`findings:[]`/`advisories:[]`；结构字段（landing-manifest 围栏块 + packet-id 行）在场；B15 行尾清单 10 文件逐一实测一致（CRLF 4 件 + LF 6 件、原脚本 197 行）。
- 复证 1/2/3 关键数字抽核：`1026/1683` 口径（k4-attribution-fix-review-20261024.md:23）、`1187` 冲突/distinct 120（:30）、截断 56/27（:51）、Gate E E1–E5（k4-recall-architecture-20261024.md:174-180）、勘误①②（:13/:14）、成员配置实证（:55-62）；四落点文件在场（含 `delivery/taskpackages/261024-m5-k4b-corpus-parser-v2/` 目录）。阈值标定集合差实测：v2→v3 缺失 0、v3→v4 缺失 6（含 `canvas-p4`）= 6/61 = 9.8%、v4→v5 0、v5→v6 0（+1 新增）、v6→v7 缺失 0（严格超集）；v6 = 61、v7 = 64 唯一 id。`UIUX-DESIGN-BRIEF.md` 头部「完整母版仍为 kidpal-ui-design-v6.html」漂移在场；`write-story/SKILL.md:48` 钉 v3 在场。
- 消费方 census 复核：6 处文档消费逐一在场（requirement-reviewer :21/:32、story-author :19/:34、commands/requirement-change :15/:21、references/requirement-change :28/:35、write-story :49-50、arch-doc-restructure :64-78），`.claude/scripts/` 内零 import 消费方。
- 取舍 A 事实前提：`change-triage.md:43` nano 排除清单含「意图需要新行为验收语义承载（→ 升 Lite）」——spike 命中，裁定成立。

verdict: BLOCK(F1)

### R2 - 2026-10-27 - architect

**轮次**：R2 聚焦轮（零上下文判官实例）：R1 findings 修订到位性 + v0.2→v0.3 扩面新增内容全强度四查（视同首审）+ 修订新矛盾扫描。只读全仓 canonical，唯一写入 = 本节；R1 记录未触碰（插入点 = R1 verdict 行之后，其余物理位置零变动）。

**判档复核**：Track B 主路径维持（确认，无升档事由）。v0.3 扩面新增的设计面两件不引入新的机器面命中——机器面锚仍是 T2.2（`.claude/scripts/spec-anchor-check.mjs`）与 T1④（`.claude/agents/planner.md`），R1 已判恒 Track B；非跨系统结构性变更（固件 / shim / 契约 / spec 两文件零触），方向已由 human 立项追加（`delivery/workstreams/gov-debt-reduction/task_plan.md:78`）与 OQ2 丙裁（同文件 :113）收敛。riskTier medium 维持成立（解析来源更换的失败模式三条与缓释已如实点名）。preflight 判官独立复跑：`ok:true`、`anchorCount` 13、`findings:[]` / `advisories:[]`、exit 0。结构字段（landing-manifest 围栏块 13 行 + packet-id 行）在场。

**R1 findings 修订到位性**：

- **F1 · 已收口**。判官实测：编辑 B 锚 `grep -n "真空绿规约"` = **:13**、编辑 C 锚 = **:20**、编辑 D 整块 = **:36-43**（八行逐字在场，docstring 行 36 至 `}` 行 43）；§3 复扫段括注已同步为 `:10-12` / `:13` / `:20` / `:36-43`。六处编辑锚（A0 `:6` / A `:10-12` / B `:13` / C `:20` / D `:36-43` / E `:169-170`）判官逐条亲核，全部与现行 HEAD 脚本吻合（脚本 197 行、CRLF，与 B22 一致）。
- **N1 · 已收紧**。T2「为什么这样改」与 §4 取舍 E 现文均为实测倍数口径（9.8% / 83%、5.1 倍 / 1.7 倍、「保守取值而非拟合」）；残留「数量级」字样三处皆为对 R1-N1 本身的合法引用（本稿 :76 修订自指、:657 R1 原文、:686 修订表），正文修辞无残留。

**扩面新增内容四查——事实断言亲核结果（判官独立实测，不凭转述）**：

- **T2.1 声明面**：围栏块 31 行正文、行首 `current:` 恰 1（值 = `kidpal-ui-design-v7.html`）、二级标题 4 个——与 B23 自报一致。镜像先例 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 第 3–8 行实测载「唯一全量副本 / 只留『当前 = vN』指针、不复述沿革」。逐版备注 git 考据逐版复证：v2 ← `770c3009`（2026-08-29 吉祥物重绘 + 楷体，`--follow` 在案）、v3 ← `54280819`（2026-09-09 V3 定稿，commit 主语逐字合）、v4 ← `4587bf62`（2026-09-14，requirements v1.51）、v6 ← `e6253317`（2026-10-17，母版 v5→v6）、v7 ← `356a9e25`（2026-10-19）+ `05cf4910`（2026-10-21 后补账）——日期与批全对（v5 系列起点微漂见 N2）。逐版 id 计数与集合差脚本实测：base 19 / v2 61 / v3 61（与 v2 差 0）/ v4 60（撤 6 = `canvas-p4`/`p4-notes`/`pose-sleep`/`shot-p4`/`state-sleepy`/`swap-demo`，9.8%；增 5 与声明逐字同）/ v5 60（与 v4 差 0）/ v6 61 / v7 64（v7−v6 = `p3-recording-guide`/`state-organizing`/`p3-recording-guide-spec`，v6 于 v7 缺失 0 = 严格超集）——与声明面表格逐格吻合。v6 事故「12 条假红涉 10 条 story」在 `ARQ-261017` 条目 :3 / :40 在案。V5/V6/V7 图夹 12 / 1 / 3 张实测与 OQ2 现状一致；`designRefs` 引例 `docs/ui/V5/canvas-p1.png` 在 `task-intake-and-slicing.md:348` 在场。
- **T2.2 脚本六处编辑**：判官从本稿 §2 围栏块独立装配（锚计数 fail-loud，6 处全部恰 1 次；D 原文围栏与现行脚本 :36-43 逐字相等）——装配版 265 行、CRLF、`node --check` 过，与 B22 自报一致；B24 十项字面计数逐项复现。变量前提实测：`args` :115、`designFile`/`designIds` :147-148 均先于编辑 E 落点 :169；`:147` 的 `currentDesignFile()` 调用在 `--reverse` 早退 :150 之前、`--diff-ids` 分支 :116-123 在设计解析之前——「逆视图要求声明面」与「`--diff-ids` 不读声明面」两条行为声明与脚本结构吻合。
- **装配夹具判别力独立抽复现**（判官按 §5 配方自建 11 夹具 + 修正版 2 夹具，双脚本对跑）：指针语义反转对成立——夹具 E（声明 = v1、v2 在场）现行脚本 exit 1 报 `canvas-p1` 悬空（B3 同形）vs 装配版 exit 0 + stderr「存在更高版号…以声明为准」（B8 报文逐字同）；塌缩对成立——夹具 B 现行脚本 `ok:true` exit 0（假绿实证）vs 装配版 exit 2、stderr 与 B5 逐字一致（含「缺失 10 个、占 83%」）；夹具 A ok:true、C exit 1 只报锚错不误判母版、D 跳过、H4 容忍反引号、F/G/H1/H2/H3 各 exit 2 且归因逐字对形；`--allow-master-shrink` exit 0（B14）；`--reverse`（E）exit 0 结构不变、（F）exit 2（B15）；`--diff-ids`（F）oldCount 12 / newCount 12 / removed `d-10` / added `d-11` exit 0（B16 逐字）。
- **真仓三态复现**：B1 现行脚本 `{"design":"kidpal-ui-design-v7.html","designIdCount":64,"archSectionCount":48,"storyCount":14,"anchorLines":36,"errors":[],"ok":true}` exit 0；装配版 + 声明 v7 与 B1 逐字相同（B17）；装配版 + 声明 v6 → 提示 + `design v6` / 61 / ok:true exit 0（B18）。
- **T2.3 四处五条目**：锚与原文逐字亲核——`uiux-author.md:26` / :27（C-1b 原文整行）、`write-story/SKILL.md:48`（钉 v3 原句）、`UIUX-DESIGN-BRIEF.md:5`（「完整母版仍为 v6」子句；:5 行尾外括号「）」在场，C-3 接续说明成立；:106 批次叙事句在场且本批不改）、`requirement-change.md:45`（4b 子句）全部吻合；`requirement-change.md:58` 的「UIUX-DESIGN-BRIEF 版本行注明对齐的本批 requirements 版本号」launchPreconditions 判据在场——C-3 保留版本行的决定依据实测成立。
- **T1 / T3 落点锚抽核**（扩面未触、验未被新增文本污染）：`change-triage.md:32`、nano 排除清单 :43、`dev-loop.md:119` / :136、`planner.md:59` / :66、`PROPOSAL.template.md:46` / :48、三条目 frontmatter 与文末 append 锚三处逐字在场。
- **OQ2 丙裁决段引述保真**：决策原话与 `task_plan.md:113` 逐字吻合（粗体标记除外）；立项追加与 :78 吻合；`ARQ-260908`「现在授权清掉」与 :101 吻合。human 理由句（「目录最干净」「不变性有机制保障」）不出自 task_plan 决策行，出处 = 派发携带的 AskUserQuestion 记录——PROPOSAL 已分层（原话给出来源并逐字引用、理由标「如实记录不代编造」），与 T3 授权记录同款诚实承载；「亦否乙」属取丙的逻辑必然，非代编。通过。
- **新字面 census 抽核**：`活图夹` / 行首 `^current:` 排除本批目录后全仓 0 文件实测成立。

**R2 特有焦点逐项裁定**：

- **C-1b 只钉语义不钉路径**：裁定成立——现在钉 `docs/ui/pages/` + `_frozen/` 会与本批「零新增图夹 / 机制另立」自相矛盾，且丙候选路径在 §4 已明标「属候选描述」；可执行性由留根 4 发起时机判据（下一次触 UI 出图批前必须先定活图夹）+ 声明面语义节保障。过渡期措辞两读见 F3（收窄即可，不需钉路径）。
- **声明面引用批 PROPOSAL §6 留根 4（带 changeId）**：耐久性可接受——review 批目录为常驻证据（GC human 门控、changes-index 在册、含 changeId 可检索），同一决策另有 task_plan.md:113 与声明面语义节两处承载。可选增强（非义务）：主线程落地时可按留根 5 同款把「机制另立动作」登记 REQ 域债项。
- **留根 4 改写而非新增**：成立——PROPOSAL 尚处 DRAFT，v0.3 原地改写反映丙裁后状态，无 append-only 义务；改写后发起时机判据与承载面反而更完整。
- **`requirement-change.md` 4b 不再补句**：裁定成立——C-4 已落「版本以声明面为准」单句；丙的目录语义在机制落地前无可钉内容，机制批落地时 4b 本就要随之更新，现在补句制造双源。防双源优先，通过。
- **B22 行尾机械可判性**：成立——`git ls-files --eol` / CR 字节计数可机判；13 件双通道核验（blob 全 i/lf；工作树 7 LF / 5 CRLF 与清单一致；CR 字节实测 planner = 0、脚本 = 197）；`core.autocrlf=true` 下 commit 恒归一 LF。
- **`--reverse` 行为变化**：裁定可接受——逆视图输出不消费设计稿，但声明解析属输入校验前移；同批原子落地保证「有新脚本必有声明面」，fail-closed 与「声明是唯一来源」自洽，变化已在 T2.5④ / B15 如实标注、结构零变化实测复现。
- **fail-closed 无兼容开关 / 更高版号只提示 / brief 版本行保留**：扩面后仍自洽——三者共承「声明面是唯一事实源」；brief 版本行保留依据（:58 机械判据载体）实测在场，两族错位在声明面头部明写。通过。
- **「起草期自检留痕」小节**：内容合法（模板四句自检义务的落地记录，author 侧随草稿版本更新正当）；物理位置问题见 N3。

**findings**：

- **F2 · SHOULD · T2.2 编辑 A0 括注对 `:7` 的描述与实况错位**。括注称「原 `:7` 的空注释行保留」——实测 `:7` = `//   ③ 可行性 🔴 必须同行挂开放问题/ARQ 引用。`（③ 行），非空注释行；最近空注释行在 :11（A 区）与 :14。同括注「`//` 与 `②` 之间的缩进不变」中「②」在插入行内并不出现（插入行内容起于「现行设计稿」），两读。影响评估：机械锚（`:6` 逐字节、恰 1 次）无恙、插入位置指令（原文行后插入一行）无歧义、判官独立装配实跑插入结果正确——不属 BLOCKING；但按 R1-F1 同标准（T2.2 行引用零容忍、本稿自身新引用不得带漂移进 FROZEN），SHOULD 未结不可冻结。**修法**：括注更正为「原 `:7`（③ 行）及其后各行顺延」（或径删括注）；围栏块文本零改动，B19/B24 不受影响。
- **F3 · SHOULD · C-1b 与声明面「活图夹与冻结快照」节的过渡期两读**。「新引用（`designRefs` / 出图 / 截图）一律以 `current:` 所指活图夹为准，不钉版本夹」为无条件规范句；收尾句「机制落地前图夹维持现状」中的现状 = 新引用钉版本夹（现行 practice：`docs/ui/V<N>/` 逐批变更帧，designRefs 实例 `docs/ui/V5/canvas-p1.png`）——过渡期两句指令方向相反。按前句执行 → 下一个触 UI 批在活图夹目录未定义时须自创目录（违反本批「零新增图夹」）；按后句执行则忽视「一律」。留根 4 发起时机判据把窗口压到零至一批，但机制批一旦顺延两读窗口即敞开，且 C-1b 是 FROZEN 后进 uiux-author 会话的常驻正文。**修法**：给「新引用…不钉版本夹」句加过渡限定（如「（机制落地后生效；落地前维持现行版本夹惯例）」），C-1b 改后围栏与声明面语义节两处同改；改动不影响 B19 锚计数，B23 的 `单一活图夹` / `冻结为快照` 计数不因加词变化（仍各 1）。

**NIT（不阻断）**：

- **N2**：声明面 v5 行写「2026-09-23 → 2026-10-12，commits `31d364d5`…`101629e4`」——实测 v5 文件创建于 `ec0804bd`（2026-09-22，`git log --diff-filter=A`），09-22 另有两笔（`20df3cbe` / `c75f894b`）；表格起点差一天、漏三笔。要点四项确实自 09-23 起，属代表性别名而非全量区间；append-only 历史备注、语义（v5 = 纯版式重构、id 集 = v4）不受影响。建议落地时把起点更正为 `ec0804bd`（2026-09-22）或注明「要点自 `31d364d5` 起」。
- **N3**：「## 起草期自检留痕」小节物理上位于「## 7. 评审记录」标题之后、「### R1」之前，使 R1/R2 判官记录在标题层级上嵌于 author 侧小节之下而非 §7 之下；author 在 v0.2/v0.3 的复跑更新亦落在 §7 区内。完整性现由 sha256 对照与 preflight 机械护栏兜底、verdict 行首形态消费不依赖层级，故 NIT。建议 FROZEN 前把该小节整块移到「## 7. 评审记录」之前（R1/R2 记录逐字不动、仅物理归位）。
- **另记（非 finding，主线程落地前动作提醒）**：批目录 `_scratch/`（assemble-docs.mjs / assemble-script.mjs / check-consistency.mjs / make-fixtures.mjs / preflight.json）仍在——§6 环内自律要求起草结束后删除、不随批 commit；V6 正向断言（diff 恰等于 manifest 13 行）会机械拦住漏删，此处仅留痕。

**升级 human 项**：无。F2/F3 均为文面级修订，不动目标效果与方向；判档与 riskTier 不因本两处变化。

**亲核留痕（判官 2026-10-27 独立实测）**：脚本六锚 grep -n 逐条；装配复现（265 行 CRLF、node --check 过、B24 十项）；夹具 13 次双脚本对跑（退出码与报文逐字比对）；真仓三态（B1/B17/B18）；逐版 id 集合差脚本（含 9.8% 与 +3 名单）；git 考据 --follow / --diff-filter=A 逐版；`OPENCLAW_SYSTEM_PROMPT_CARRIER` :3-8、`ARQ-261017` :3/:40、`requirement-change.md` :45/:58、`task-intake-and-slicing.md` :348、`verify_art_assets.mjs` :14、`PHASE1-DESIGN-INPUT.md` :5/:128、`fixer-brief-r1.md` :46 逐处读核；行尾 `git ls-files --eol` + CR 字节双通道；preflight 独立复跑 ok:true / 13 锚；`task_plan.md` :78/:101/:113 决策记录逐字比对。

verdict: BLOCK(F2,F3)

### R3 - 2026-10-27 - architect

**轮次**：R3 聚焦轮（零上下文判官实例）：R2 findings（F2/F3）修订到位性 + N2/N3 收口 + 修订新矛盾扫描。只读全仓 canonical，唯一写入 = 本节；R1/R2 记录未触碰（本节插入点 = R2 verdict 行之后、作者侧修订记录之前）。

**R2 findings 修订到位性**：

- **F2 · 已收口**。判官逐项实测：现行脚本 `:6` = `//   ② story 锚行的 UI 锚 → docs/ui/ 现行设计稿真实存在的 data-od-id；`（② 行）、`:7` = `//   ③ 可行性 🔴 必须同行挂开放问题/ARQ 引用。`（③ 行）、`:8` 起为 `--reverse` 说明——与新括注三件事实逐字一致；插入行 `//      现行设计稿 = docs/ui/UI-DESIGN-VERSIONS.md …` 为 `//` + 六个空格续行缩进、行内无 `②`。判官独立复跑装配（`_scratch/assemble-script.mjs`，A0 锚已收紧为「② 行 + ③ 行」两行整块，脚本 `:19-28` 亲读）：六处编辑锚各恰 1 次（OK ops=usage-2-decl,usage-diffids,vacuum-line,exit-line,current-design-block,forward-block），装配版插入行落 **`:7`**、③ 行顺延 **`:8`**（sed 5–9 行亲核），265 行 CRLF（node 实测 CR=265）、`node --check` 过。围栏块文本零改动成立（B24 十项字面计数逐项复现）。
- **F3 · 已收口**。过渡限定语「机制落地后生效；落地前维持现行版本夹惯例：`docs/ui/V<N>/` 逐批落变更帧」在两处规范句逐字在场且两处全等：C-1b（本稿 `:391`）与声明面语义节（`:228`）；两处收尾句「机制落地前图夹维持现状」（`:228` 末 / `:391` 末）与限定语不再相抵（机制落地前按现行版本夹惯例执行）。全仓「不钉版本夹」实测恰 4 处：`:228` / `:391`（规范句，已限定）、`:713`（R2 判官原文 F3 finding 引述，判官写面零触碰）、`:778`（作者修订记录引述）——本稿之外其余文件 0 命中（canonical 未落地，正确）。B23 两侧计数 uiux = 1 / 声明面 = 1 独立复现；`uiux C-1b line == proposal: true`、C-1b 行内过渡限定 count = 1（verify-v04 实跑）。装配文档全链重跑（判官先以仓内现行文件刷新 `docs-copy` 再跑）：23 处编辑全部 ok、`check-consistency.mjs` **ALL-CONSISTENT**。

**N2 · 已收口（声明面）**：判官独立 git 实测——`git log --diff-filter=A -- docs/ui/kidpal-ui-design-v5.html` = `ec0804bd`（2026-09-22，创建提交）；该路径 2026-09-22 当日恰四笔（`ec0804bd` / `d23fb0db` / `20df3cbe` / `c75f894b`），与修订记录第 3 行「共四笔」一致；要点四项对应 commit 逐笔实测 = 聆听态重排 `31d364d5`（09-23）/ 四态骨架归一 `54c25234`（09-24）/ 提醒态右区收敛 `10a099c8`（09-24）/ 首页明天组时间打头 `c7388799`（09-26）——最早确为 `31d364d5`，「要点四项自 `31d364d5`（2026-09-23）起」注记成立。声明面 v5 行 id 主张（60 个、id 集 = v4）与要点四项本体逐字未变、围栏仍 31 行 / 行首 `current:` 恰 1。**残留见 F4**。

**N3 · 已收口**：物理序实测——自检留痕小节（`:598`）在「## 7. 评审记录」（`:640`）之前，R1（`:644`）/ R2（`:677`）恢复挂在 §7 之下。判官记录完整性独立证实：R1 段（`:644-675`，LF、无尾换行口径）sha256 = `edcadacf90478c0b71447aa3f778677088d5f61d315ab3058757b6bf4269f17a`、R2 段（`:677-725`）= `2d1e2c820be51b1d311f1ec96166f3f8d5c60f36dbcebca146801e46ad9aed9a`——判官对边界变体穷举复算，两值与作者声明逐一命中，即 R1 自 v0.3 声明时点、R2 自 v0.4 声明时点起逐字未动；`_scratch/r1-block.md` 快照与现行 R1 段逐字节相等，交叉印证。自检段刷新范围六项（版本标签 v0.4 / `wc -l` 784〔实测相符〕/ 声明面 sha256 `7bdf3851…`〔判官独立复算 = `7bdf3851efa8f3d9cf506ff60e59879dc74fb84678d0ad53eae8a7322e271d08`〕/ R1·R2 双 hash / C-1b 逐字节断言 / census 增列 v0.4 新字面）与修订记录第 4 行声明逐项相符，未发现声明外改写（U+FFFD 扫描零命中亦复核）。

**N3 处置越权疑点自裁**：派发语「整块逐字不动移到 §7 之前」的承重对象是判官记录（R2-N3 建议原文即「R1/R2 记录逐字不动、仅物理归位」）；自检段是 author 侧活记录（模板四句自检的落地记录），其内容为自指元数据（行数 / sha256 / 版本标签），原地平移而不刷新会使其断言失真（767 ≠ 784）。作者「R1/R2 不动 + 归位本身不改写 + 自检段自身 v0.4 刷新且逐项申报」的理解成立，不构成越权，不作 finding。

**findings**：

- **F4 · SHOULD · §2.0 复证 4 升版批考据的 v5 行未随 N2 更正，与已收口的声明面直接矛盾**。复证 4（`:94`）现文仍为「v5 ← `31d364d5`…`101629e4`（2026-09-23 → 2026-10-12，ui-v5 系列，收拢于 `261011-p1-home-v5-rework`）」——同一文档内声明面 v5 行已更正为「2026-09-22 → 2026-10-12，commits `ec0804bd`…`101629e4`（…；要点四项自 `31d364d5`（2026-09-23）起）」（`:218`），而 git 实测 v5 创建提交 `ec0804bd`（2026-09-22）与 09-22 另三笔（`d23fb0db` / `20df3cbe` / `c75f894b`）均在 `31d364d5` 之前、被该行区间遗漏。影响评估：复证 4 是起草期证据记录、无机械消费面、不落任何 landing 制品，不属 BLOCKING；但它使本稿对同一事实持有两个互斥陈述（N2 修订的未传播残留，修订记录「未纳入」清单亦未申报），且本稿复证纪律自设「每条结论对应实跑命令或亲读文件」——按 R1-F1 / R2-F2 同款标准（本稿自身事实断言零漂移进 FROZEN；R1-F1 判言明认「无机械消费面」仍 SHOULD），SHOULD 未结不可冻结。**修法**：`:94` 的 v5 子句与声明面同口径（起点改 `ec0804bd`（2026-09-22）+ 注「要点四项自 `31d364d5` 起」，或等义措辞）；一处散文更正、零围栏改动，R4 聚焦核该行即可。

**本轮其余焦点逐项裁定**：

- **§5 B23 v0.4 新列 / §3 census「全 0」抽查**：五组 v0.4 新字面（`机制落地后生效` / `落地前维持现行版本夹惯例` / `docs/ui/V<N>/` / `ec0804bd` / `要点四项自`）判官独立 `grep -rlF`（排除本批目录）实测全 0，与 §3 硬项 / B20 声明一致；B23 v0.4 四组计数逐项复现（过渡限定语 uiux / 声明面各 1、`docs/ui/V<N>/` 各 1、`ec0804bd` = 1、`2026-09-22 → 2026-10-12` = 1、声明面 31 行正文）。
- **夹具全谱与真仓三态**：verify-v04 全量重跑（判官自刷副本后）——B1 真仓 v7 / 64 / 48 / 14 / 36 / ok:true exit 0、B2 现行脚本 + 夹具 B 假绿 exit 0、B3 exit 1；装配版 A / D / E / H4 exit 0、B exit 2（stderr 含「缺失 10 个、占 83%」）、C exit 1、F / G / H1 / H2 / H3 各 exit 2 且归因逐字对形、`--allow-master-shrink` exit 0、`--reverse` E exit 0 / F exit 2、`--diff-ids` exit 0、B17 与 B1 同值、B18 exit 0。
- **结构字段与判档**：landing-manifest 围栏块 13 行整（`:13-25`）、preflight 判官独立复跑 `ok:true` / `anchorCount` 13 / `findings:[]`；判档 Track B 主路径维持（机器面锚 = T2.2 脚本 + T1④ planner，R1 已判恒 Track B；v0.4 四处均为文面级，不改爆炸半径）；riskTier medium 维持成立。F2 / F3 / N2 / N3 收口均未引入机械面或门语义变化。
- **新观察（非 finding，留痕）**：① `_scratch/` 现含 `r1-block.md` / `verify-v04.mjs` 两件 R2 时点未列的文件（作者侧 v0.4 自检驱动与 R1 快照）——仍受「起草结束后删除、不随批 commit」约束，V6 正向断言机械兜底；② 判官复跑时先撞 ANCHOR-FAIL——根因是 `C:/tmp/p5-assembly/docs-copy/` 为作者上轮装配的残留脏态（已含改后文本），以仓内现行文件刷新副本后 23 处编辑全过——装配链 fail-loud 按设计生效，非提案缺陷。

**升级 human 项**：无。F4 为单行文面更正，不动目标效果与方向；判档与 riskTier 不因本轮变化。

**亲核留痕（判官 2026-10-27 独立实测）**：现行脚本 `:6/:7/:8` 三行逐字读核；装配独立复跑（六锚 ok、插入行落 `:7`、③ 行顺延 `:8`、265 行 CRLF〔node 计 CR=265〕、`node --check` 过）；文档装配先以仓内源刷新 `docs-copy`（11 件 + 按 PROPOSAL 围栏重建声明面）后全量重跑（23 ok + ALL-CONSISTENT）；verify-v04 全谱（B1–B18 / B23 / B24 / 声明面 sha256 `7bdf3851…` / C-1b includes 断言）；五字面 census `grep -rlF` 排除批目录全 0；「不钉版本夹」全仓恰 4 处逐处定位；git `--diff-filter=A` + 全量 log（v5 路径 10 笔、09-22 恰四笔、要点四 commit 逐笔读）；R1/R2 段 sha256 边界变体穷举命中声明值（R1 = `:644-675`、R2 = `:677-725`）；`r1-block.md` 与现行 R1 段逐字节相等；preflight 复跑 ok:true / 13 锚；`wc -l` = 784、U+FFFD 零命中。

verdict: BLOCK(F4)

### R4 - 2026-10-27 - architect

**轮次**：R4 聚焦轮（熔断条款第三选项：评审 3 轮未 GREEN 后 human 拍换新判官实例再战 1 轮；零上下文判官实例。本轮若仍非 GREEN 按条款自动转 Track A 段，故只裁事实、不放宽标准）：R3-F4 修订到位性 + v0.5 改动新矛盾扫描 + 作者侧自申报范围核验。只读全仓 canonical，唯一写入 = 本节（插入点 = R3 verdict 行之后、作者侧修订记录之前；R1 / R2 / R3 三段逐字未动——判官亲测三段 sha256 与声明值逐一命中，见留痕）。

**R3-F4 修订到位性 · 已收口**。判官独立 git 实测 + 逐字对读：

- git 实测：`git log --diff-filter=A -- docs/ui/kidpal-ui-design-v5.html` = `ec0804bd`（2026-09-22，创建提交）；该路径全量 log 恰 10 笔，其中 2026-09-22 当日四笔（`ec0804bd` / `d23fb0db` / `20df3cbe` / `c75f894b`）；要点四项最早一笔 `31d364d5` = 2026-09-23（聆听态重排，另三项 09-23 / 09-24 / 09-26）；区间末端 `101629e4` = 2026-10-12——与 R3-N2 实测口径一致。
- 改后 `:94`（§2.0 复证 4）逐字：「v5 ← `ec0804bd`…`101629e4`（2026-09-22 → 2026-10-12，ui-v5 系列，要点四项自 `31d364d5`（2026-09-23）起，收拢于 `261011-p1-home-v5-rework`）」；声明面 v5 行（`:218`）逐字：「| v5 | 2026-09-22 → 2026-10-12 | commits `ec0804bd`…`101629e4`（ui-v5 系列 + `261011-p1-home-v5-rework`；要点四项自 `31d364d5`（2026-09-23）起） |」。三项同值成立：起点 commit（`ec0804bd`）/ 日期区间（2026-09-22 → 2026-10-12）/ 要点起注（`31d364d5`（2026-09-23）起）。R3-F4 指认的「同文档两个互斥陈述」消除。
- 残留定位：`31d364d5` 与旧区间串「2026-09-23 → 2026-10-12」全文其余出现逐处定位 = `:717`（R2-N2 判官原文）/ `:736` / `:744`（R3 判官原文）/ `:811`（v0.4 修订记录）/ `:826` / `:831`（v0.5 修订记录引旧文与未纳入说明）——全部为 append-only 历史留痕与判官记录引述，§2.0 与 §3 / §5 无残留旧口径。
- 围栏零改动证实：声明面围栏（`:198-228`，31 行）sha256 判官独立复算 = `7bdf3851efa8f3d9cf506ff60e59879dc74fb84678d0ad53eae8a7322e271d08`（LF + 尾换行口径），与 v0.4 / v0.5 两轮声明值逐字相同；T2.2 脚本围栏五处关键行（`DESIGN_DECL_PATH` 定义 `:298` / `designDeclarationFatal` `:302` / `previousDesignFile` `:336` / `MASTER_SHRINK_MIN_PREV_IDS` `:352` / `MASTER_SHRINK_MIN_RATIO` `:353`）在场、位置未移。

**自申报范围核验（v0.5 修订记录第 2 行申报四项，判官逐项实测）**：① 版本标签 `:598` = 「…2026-10-27，v0.5 复跑」✓；② `wc -l` = 831：插入前实测 = 831 ✓；③ 声明面 sha256 标注 `:606` = 「`sha256 = 7bdf3851…`，v0.5 复核」✓（复算值同上）；④ 判官记录校验扩为三 hash：`:606` 载 R1 = `edcadacf…` / R2 = `2d1e2c82…` / R3 = `82019ba8…` ✓——判官对三段各做四种边界变体（LF / LF+尾换行 / CRLF / CRLF+尾换行）穷举复算，LF 口径逐一命中：R1（`:644-675`）= `edcadacf90478c0b71447aa3f778677088d5f61d315ab3058757b6bf4269f17a`、R2（`:677-725`）= `2d1e2c820be51b1d311f1ec96166f3f8d5c60f36dbcebca146801e46ad9aed9a`、R3（`:727-757`）= `82019ba85f7b9e53a9901ab5240d4a4d98803cd2b6c666e637e94041a912f5e1`。自检段其余文字与 R3 轮所见逐项相符、未发现申报外改写（`:602` census 清单含「v0.4 新增的过渡限定语…与 v5 起点 commit `ec0804bd`」，与 v0.4 修订记录第 4 行「census 增列 v0.4 新字面」申报一致，非本轮新增）；U+FFFD 判官复测零命中。自检段四句自检断言抽核：`### T` 标题恰 3（`:99` / `:178` / `:441`）、`目标文件：` 锚行恰 3、自检段载 preflight 输出块与判官复跑输出逐字一致。

**新矛盾扫描（v0.5 四处改动 × §3 census / §5 验证集交叉，判官实测）**：

- §3 硬项 / §5 B20 对 `ec0804bd` / `要点四项自` / `机制落地后生效` / `落地前维持现行版本夹惯例` / `2026-09-22 → 2026-10-12` 的「批目录外 = 0」主张：判官独立 `grep -rlF`（单根、md / mjs / json / txt / yaml / yml 六类、排除 `.git` 与本批目录）复跑——五字面全部 0 文件命中。`:94` 改动落在本批目录内，不影响批目录外基线，B20 各值仍成立。
- §5 B23「`ec0804bd`：声明面 = 1」「`2026-09-22 → 2026-10-12`：声明面 = 1」：声明面围栏 31 行内两字面各恰 1（均在 `:218`）——成立；B19 / B23 / B24 的计数对象（落地文件副本与脚本装配产物）均不含 §2.0 散文，`:94` 单行更正不触任何计数断言。
- preflight 判官独立复跑：`ok:true`、`anchorCount` 13、`anchorUnion` 与 manifest 十三行逐条一致、`findings:[]` / `advisories:[]`、exit 0——与自检段载块逐字一致。

**结构不变量**：manifest 围栏 13 行整（`:13-25`）；全文围栏行恰 50 条、偶数配平；全文行首 `^current:` 恰 1（`:208`，声明面围栏内）；自检小节物理位置在「## 7. 评审记录」（`:640`）之前（`:598`），R1（`:644`）/ R2（`:677`）/ R3（`:727`）挂在 §7 之下；判档 Track B 主路径（`:5`）与 riskTier medium（`:7`）v0.5 未动。判官插入本记录后全文行数实测 = 860 行——作者侧自检段「`wc -l` = 831」系判官插入前状态，该字段属作者侧活记录、判官不代刷新（R3 先例：作者下一轮写入时刷新；若本轮 GREEN 即 FROZEN，主线程标 FROZEN 时以本行实测值为准）。

**FROZEN 前终态一致性确认（派发焦点第 5 项）**：OQ1 = (a) 文面纪律 + planner Step 1i + 模板硬项（`:495-497`，R1 裁定落法在场、本稿不重开）；OQ2 = 丙（`:503-507`，human 拍板记录 + 落法边界「只规则句 + 声明面语义节」+ 丙目录形态明标「属候选描述」）；T3 记账边界 = 「纯记账、未执行任何重指」（T3 改法明写、§0 非目标与复证 3 授权承载方式一致）；判档 = Track B 主路径、riskTier = medium，v0.5 单行散文更正不改爆炸半径。四项互相一致、无遗留互斥；R1-F1 / R2-F2 / R2-F3 既往已核销收口，本轮不重开。

**升级 human 项**：无。

**亲核留痕（判官 2026-10-27 独立实测）**：git 全量 log + `--diff-filter=A`（v5 路径 10 笔、09-22 恰四笔、`ec0804bd` / `31d364d5` / `101629e4` 日期逐笔）；`:94` / `:218` 逐字节对读；三段判官记录 sha256 四边界变体穷举（LF 口径命中声明值）；声明面围栏 31 行 sha256 复算命中；preflight 独立复跑（ok:true / 13 锚 / findings:[]）；插入前 `wc -l` = 831；围栏行 50 计数、`^current:` 恰 1；五字面 census `grep -rlF` 排除批目录全 0；`31d364d5` / 旧区间串全文逐处定位；U+FFFD 零命中；T2.2 脚本围栏五关键行在场抽核；自检段 `### T` / `目标文件：` 计数抽核。

verdict: GREEN

## 修订记录（作者侧 · v0.1 → v0.2，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 记录逐字未动（改动前后 sha256 相同）。

**依据**：R1 verdict BLOCK(F1)（§7）+ human 2026-10-27 立项追加「UI 设计版本结构整体调整并入本批 T2 一起改」（`delivery/workstreams/gov-debt-reduction/task_plan.md` 决策记录）。

| # | 来源 | 修订 |
|---|---|---|
| 1 | R1-F1 | §2 T2 三处行号漂移更正：编辑 B `:15` → **`:13`**、编辑 C `:24` → **`:20`**、编辑 D `:46-47` → **`:42-43`**；§3 复扫段括注同步（`:10-12` 块、`:13` 追加、`:20` 整行）。v0.2 另对全部脚本编辑锚做了逐条实跑亲核（含新增的 `:6` 与 `:36-43` 整块），围栏块文本按亲核结果重排为 A0/A/B/C/D/E 六处 |
| 2 | R1-N1 | T2「为什么这样改」与 §4 取舍 E 的「一个数量级断层空档」修辞收紧为实测倍数（9.8% → 50% 为 5.1 倍、50% → 83% 为 1.7 倍，中间无观测样本；阈值属保守取值而非拟合），判据不变 |
| 3 | 立项追加（human 2026-10-27） | T2 由「完整性自查 + 命名族纪律」整体重写为「**声明面 + 显式 current 指针 + 指针读法改造 + 完整性自查兜底 + 纪律文面升级 + 存量迁移**」：新增 `docs/ui/UI-DESIGN-VERSIONS.md`（T2.1，含 v2–v7 逐版备注与 current=v7 依据记录）；脚本 `currentDesignFile` 整块重写为声明面读法（T2.2-D，原 D+E 合并）；编辑 B 扩为四行、编辑 C 改写退出码口径；纪律面扩为四处（uiux-author / write-story / brief / requirement-change）；原 v0.1 实施件 6（uiux 命名族）与 7（write-story 最高版号形态）**由新方案取代并改写**（写者纪律升级为「升版与声明面同次落盘」、指针改为指声明面） |
| 4 | 立项追加 | manifest 10 → **13**：增 `docs/ui/UI-DESIGN-VERSIONS.md`（新）、`docs/ui/UIUX-DESIGN-BRIEF.md`（版本行尾部改指声明面）、`.claude/references/requirement-change.md`（4b 出图版本指声明面） |
| 5 | 立项追加 | riskTier `low-medium` → **medium**（T2 现为「解析来源更换 + 新增设计面 canonical + 存量迁移」，失败模式与缓释三条已点名，见头部声明） |
| 6 | 立项追加 | §0 目标效果增第 3 条（版本结构显式化）、现状差距增「版本声明散落三处互斥」一条；非目标改写（`docs/ui/**` 由「零 diff」改为「仅两件、图夹与其余零 diff」；增「不动图夹 / 不批量重出图 / tools 零 diff」） |
| 7 | 立项追加 | §2.0 新增**复证 4**（版本族谱逐版 id 计数与集合差、升版批 git 考据、「实际在用」四条证据、六处漂移清单）；复证 2 同步（行号亲核、N1 措辞、brief 声明与 brief 自身版本的区分） |
| 8 | 立项追加 | §4 取舍 D/E/F 重写并新增 G（requirement-change 指针句，含退路）、H（fail-closed 无兼容开关）、I（更高版号只提示）；新增 **OQ2**（截图图夹布局三候选） |
| 9 | 立项追加 | §5 验证集重写：夹具扩为 A–H4 十一形态（含指针语义判别夹具 E）、基线 B1–B24（含真仓两态 B17/B18）、判据锚 27 处；V2/V3/V6 同步（V6 负向清单加 `tools/`、生成 INDEX，正向加 `docs/ui/` 白名单断言） |
| 10 | 立项追加 | §6 留根重写：原留根 4（brief 声明漂移未修）**已由本批修复**，替换为图夹布局（OQ2）与两处陈旧版本引用（`verify_art_assets.mjs:14`、`PHASE1-DESIGN-INPUT.md:5/:128`）留根；其余条目按新方案调整编号与措辞 |
| 11 | 立项追加 | §3 sweep 表增三行（声明面 / brief / requirement-change）、设计面其余行改写；硬项 census 补新字面十项与 `^current:` 基线 |

**未纳入（作者判断，judge 可 challenge）**：`docs/ui/V5|V6|V7/**` 图夹不动（OQ2 已裁丙，机制与整形另立需求面动作，见 v0.3 记录）；`tools/asset-pipeline/**` 零改（留根 5）；`PHASE1-DESIGN-INPUT.md` 不修（留根 5）；`brief:106` 的批次叙事句不改（历史记录）；`arch-doc-restructure/SKILL.md` 的「现行设计稿」表述不改（无版本推断，不漂）。

## 修订记录（作者侧 · v0.2 → v0.3，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 记录逐字未动（改动前后 sha256 相同：`edcadacf90478c0b71447aa3f778677088d5f61d315ab3058757b6bf4269f17a`）。

**依据**：human 2026-10-27 拍板 OQ2 取丙（单一活目录 + 冻结快照）（否 architect 推荐的甲、亦否乙）；决策记录 = `delivery/workstreams/gov-debt-reduction/task_plan.md` 决策记录节。

| # | 来源 | 修订 |
|---|---|---|
| 1 | OQ2 丙裁 | §4 OQ2 由「待拍三候选」改写为「已裁记录」：裁决（丙）+ 拍板时间 + human 给出的丙胜出理由（目录最干净、旧引用的布局权威不变性有机制保障；只记 human 语义、不代编造）+ 本批落法边界（只规则句）+ 留根指针；甲 / 乙原文保留、降为落选记录，丙标注「裁定采用」并注明其目录形态属候选描述、具体路径归另立动作 |
| 2 | OQ2 丙裁 | T2.1 声明面新增「活图夹与冻结快照」语义节：页图只有一个活图夹（始终反映 `current:`）、升版冻结义务、快照 = `designRefs` 布局权威、新引用走 `current:`、旧包不追溯；明记「本批只成文、不落机制」。声明面正文 27 → **31 行**（`^current:` 仍恰 1） |
| 3 | OQ2 丙裁 | T2.3 新增 **C-1b**：`uiux-author` Process 第 3 步出图落点句整行成文（单一活图夹 + 升版冻结义务 + 引用一律走声明面 current + 机制 / 迁移另立）；T2.3 标题改「四处 / 五条目」 |
| 4 | OQ2 丙裁 | T2.5 `ARQ-261017` append 同步：⑤ 纪律侧补第 3 步句；留根 ① 改已裁形态（丙已拍、机制与存量迁移另立） |
| 5 | OQ2 丙裁 | §3 sweep 表三行同步（uiux 两处编辑、声明面语义节、设计面其余行）；§3 硬项 census 补 v0.3 新字面（`活图夹` / `单一活图夹` / `布局权威` / `冻结为快照` / `升版冻结义务` / `引用一律走声明面` / `页图版本语义` / `活图夹与冻结快照` 全 0；`冻结快照` 8 文件均为历史批异义）；复扫段「替换型」计数九 → **十**并补列 C-1b（v0.2 该段自报「文档侧五处」却仅列四处，顺带校正为实际五处） |
| 6 | OQ2 丙裁 | §5 验证集：B19 锚 27 → **28**（文档侧 21 → 22）、B23 增三组新字面计数与声明面 31 行、B20 同步；夹具 / 脚本面 / 真仓两态基线与脚本编辑零变（T2.2 一字未动），V3 判据自然覆盖新字面（预期 = B23 列值） |
| 7 | OQ2 丙裁 | §6：留根 4 改写为「截图目录整形 + frozen 快照机制 + 存量迁移」并写清发起时机判据（下一次触 UI 的出图批到来前必须定活图夹，否则出图无落点）与承载（需求面 loop 档 2 4b 或独立 intake）；§6 顶部 `ARQ-261017` 一条的留根指针同步 |
| 8 | OQ2 丙裁 | §0：目标效果增第 5 条、非目标 bullet 改写、复证 4 结论句同步；T2 标题与 T2.4 增「图夹存量迁移不在本批」一句 |
| 9 | — | **manifest 不变（13 行整）**：C-1b 落在既有 `uiux-author.md`、语义节落在既有新文件 `UI-DESIGN-VERSIONS.md` 的围栏内——无增删；**判档与 riskTier 不变**（维持 Track B 主路径 / medium）：丙落法只加规则句与声明面语义节，零脚本、零门、零路径变更，不改变爆炸半径、不引入新失败模式 |

**未纳入（作者判断，judge 可 challenge）**：图夹目录整形 / frozen 快照机制本体 / `docs/ui/V5|V6|V7/**` 存量迁移 / 按丙语义的新出图落点（递延为另立需求面动作，§6 留根 4）；`requirement-change.md` 4b 的出图落点句不再改——丙语义只在 `uiux-author`（执行者）与声明面承载，避免双源；4b 现有「PNG 落 `docs/ui/`」与「单一活图夹在 `docs/ui/` 下」不冲突。

## 修订记录（作者侧 · v0.3 → v0.4，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 / R2 判官记录逐字未动（改动前后 sha256 相同：R1 = `edcadacf90478c0b71447aa3f778677088d5f61d315ab3058757b6bf4269f17a`、R2 = `2d1e2c820be51b1d311f1ec96166f3f8d5c60f36dbcebca146801e46ad9aed9a`）。

**依据**：R2 verdict BLOCK(F2,F3)（§7）+ N2 / N3 两条 NIT。

| # | 来源 | 修订 |
|---|---|---|
| 1 | R2-F2 | §2 T2.2 编辑 A0 括注事实更正。原括注「`//` 与 `②` 之间的缩进不变，原 `:7` 的空注释行保留」与实况错位——逐行实跑：现行脚本 `:6` = `//   ② story 锚行的 UI 锚 → …`（② 行）、`:7` = `//   ③ 可行性 🔴 必须同行挂开放问题/ARQ 引用。`（③ 行，**非**空注释行）、`:8` 起为 `--reverse` 说明；插入行字节 = `//` + 六个空格 + 正文（行内不含 `②`）。括注改为「插入行以 `//` 起首、六个空格续行缩进；原 `:7`（`//   ③ 可行性 🔴 …`，即 ③ 行）及其后各行顺延一行」。**围栏块文本零改动**（插入行逐字节照旧）；装配脚本 `assemble-script.mjs` 的 A0 锚同步收紧为「② 行 + ③ 行」两行整块（明示插入位在 ③ 行之前，消除原锚以 `//` 子串命中的隐式性），装配实跑六处锚各恰 1 次。 |
| 2 | R2-F3 | 过渡期两读收口（**两处同改**）：C-1b（`uiux-author.md` 第 3 步替换句）与 T2.1 声明面「活图夹与冻结快照」节，均在「新引用（`designRefs` / 出图 / 截图）…不钉版本夹」之后加「（机制落地后生效；落地前维持现行版本夹惯例：`docs/ui/V<N>/` 逐批落变更帧）」——机制落地前按现行版本夹惯例执行，不再与「机制落地前图夹维持现状」相抵。改动后逐字节复核：`check-consistency.mjs` ALL-CONSISTENT；`P.includes(C-1b 替换行)` = true；两侧过渡限定语各恰 1 次；声明面围栏块与装配产物逐字节一致（`sha256 = 7bdf3851…`）。§4 OQ2 落法边界 ① 加一句指回过渡限定（不改语义）。B23 增两侧计数各 1。 |
| 3 | R2-N2 | 声明面逐版备注 v5 行起点回填：原「2026-09-23 → 2026-10-12，commits `31d364d5`…`101629e4`」更正为「2026-09-22 → 2026-10-12，commits `ec0804bd`…`101629e4`（…；要点四项自 `31d364d5`（2026-09-23）起）」。依据实跑：`git log --diff-filter=A -- docs/ui/kidpal-ui-design-v5.html` = `ec0804bd`（2026-09-22，v5 文件创建提交）；该路径 2026-09-22 另有 `d23fb0db` / `20df3cbe` / `c75f894b` 三笔（原行起点漏掉创建 + 三笔共四笔）；要点四项最早一笔 = `31d364d5`（2026-09-23 聆听态重排，其余三项在 09-24 / 09-26），故按「要点自 `31d364d5` 起」如实标注。id 集与要点本体不变、声明面仍 31 行正文。 |
| 4 | R2-N3 | 「起草期自检留痕」小节整块从 §7 标题之后（R1 之前）物理归位到「## 7. 评审记录」之前，R1 / R2 判官记录由此恢复挂在 §7 之下（判官建议的 FROZEN 前归位，本稿提前执行）；归位后内容按 v0.4 复跑刷新：版本标签 v0.3 → v0.4、`wc -l` 767 → 784、声明面围栏 sha256 更新、判官记录校验由「R1 单条」改为「R1 / R2 双 hash」并补 C-1b 逐字节断言一句、第 2 条 census 清单增列 v0.4 新字面；其余文字逐字保留。 |
| 5 | — | §5 同步：B23 增 v0.4 四组计数（过渡限定语 uiux / 声明面各 1、`docs/ui/V<N>/` 各 1、`ec0804bd` = 1、`2026-09-22 → 2026-10-12` = 1；声明面仍 **31 行正文**）；B20 与 §3 硬项 census 增五个新字面（批目录外实测全 0）；B19（28 处锚）/ B24（脚本十项）/ B22（装配版 265 行 CRLF）与夹具基线零变——脚本围栏一字未动。 |
| 6 | — | **manifest 不变（13 行整）**：F2 只动括注散文、F3 / N2 只动既有围栏内容与声明面既有行、N3 只做物理归位——无增删件；**判档与 riskTier 不变**（Track B 主路径 / medium）：四处均文面级（F3 是同一句内加过渡限定、N2 是既有单元格事实更正、N3 是标题层级归位），不改爆炸半径、不引入新失败模式。 |

**未纳入（作者判断，judge 可 challenge）**：`ARQ-261017` 结项 append 里「第 3 步出图落点句成文『单一活图夹 + 升版冻结快照 + 引用一律走声明面 current』」的历史记账句不加过渡限定——它记录本批写了什么、不是对后续会话的规范指令；§6 留根 4 与 §3 sweep 表的语义列举同理（均已明写机制本体另立、本批只成文）。

## 修订记录（作者侧 · v0.4 → v0.5，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 / R2 / R3 判官记录逐字未动（改动前后 sha256 相同：R1 = `edcadacf90478c0b71447aa3f778677088d5f61d315ab3058757b6bf4269f17a`、R2 = `2d1e2c820be51b1d311f1ec96166f3f8d5c60f36dbcebca146801e46ad9aed9a`、R3 = `82019ba85f7b9e53a9901ab5240d4a4d98803cd2b6c666e637e94041a912f5e1`）。

**依据**：R3 verdict BLOCK(F4)（§7）；熔断拍板 = 换新判官实例再战 1 轮（human 2026-10-27）。

| # | 来源 | 修订 |
|---|---|---|
| 1 | R3-F4 | §2.0 复证 4「升版批考据」的 v5 子句按 N2 同口径更正（单行散文、零围栏改动）：原「v5 ← `31d364d5`…`101629e4`（2026-09-23 → 2026-10-12，ui-v5 系列，收拢于 `261011-p1-home-v5-rework`）」改为「v5 ← `ec0804bd`…`101629e4`（2026-09-22 → 2026-10-12，ui-v5 系列，要点四项自 `31d364d5`（2026-09-23）起，收拢于 `261011-p1-home-v5-rework`）」。依据 = R3-N2 已独立 git 实测的同一事实（`git log --diff-filter=A -- docs/ui/kidpal-ui-design-v5.html` = `ec0804bd`，2026-09-22，v5 创建提交；要点四项最早一笔 = `31d364d5`，2026-09-23），本稿只对齐口径、不重跑全谱夹具。改后与声明面 v5 行（`:218`）并排对读：起点 commit、日期区间、要点起注三项同值。 |
| 2 | — | 「起草期自检留痕」小节按 v0.5 复跑刷新自指元数据（版本标签 v0.4 → v0.5、`wc -l` 784 → **831**、声明面围栏 sha256 复核标 v0.5、判官记录校验由「R1 / R2 双 hash」扩为「R1 / R2 / R3 三 hash」）；该小节其余文字逐字保留。 |
| 3 | — | 自检最小集复跑（v0.5，改后实跑）：preflight `ok:true` / `anchorCount` 13 / `findings:[]` / `advisories:[]`；U+FFFD 零命中；围栏行 **50** 条、配平（偶数）；全文行首 `^current:` 恰 1（在声明面围栏内）；R1 / R2 / R3 三段 sha256 与改前逐字相同；声明面围栏块 sha256 = `7bdf3851efa8f3d9cf506ff60e59879dc74fb84678d0ad53eae8a7322e271d08` 未变。 |
| 4 | — | **manifest 不变（13 行整）**；**判档与 riskTier 不变**（Track B 主路径 / medium）：单行散文更正，零围栏、零脚本、零门、零路径变更，不改爆炸半径、不引入新失败模式。 |

**未纳入（作者判断，judge 可 challenge）**：v0.4 修订记录第 3 行（N2 条目，`:811`）与 R2 / R3 判官记录中对旧区间「2026-09-23 → 2026-10-12」的引述属历史留痕（append-only），不回改；§5 验证集不因本轮变动——B19 / B23 / B24 的计数对象是落地文件副本与脚本装配产物，本轮 §2.0 散文不在其中。
