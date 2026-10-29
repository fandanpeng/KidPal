# 轻量设计 PROPOSAL · 261029-gov-review-gov-loop-simplification

> **状态：FROZEN v0.2**
> **gov-flow**: direct-implementation
> **范围：仅 `/gov-change` 治理面；不改 dev-loop，不改契约门。**
> **设计依据：human 已确认的减法方向、直接实施边界与既定 ARQ 波次启动方式。**

## 1. 目标与非目标

### 目标

把治理批的主要成本从「证明主线程逐字节搬运没有出错」转到三处真正仍有风险的判断：设计语义是否正确、真实 diff 是否兑现意图、验证是否能识别错误实现。

常规批采用一条短链：human 必要时确认意图，architect 轻设计，architect-reviewer 审设计并按实际风险决定常规或条件加强，architect 直接实施和自测，reviewer 再审真实 diff，机械检查后合并。普通批不再必经 landing-verifier，也不再要求整套 landing 制品和事后 retro 文档；保留轻量 scope 清单与实时事件流。

architect 的直接实施范围覆盖普通治理文件，不再限于 scripts/hooks。契约、权限矩阵、指令根和自改递归等保护面仍走主线程或既有专门边界，不因新流程而放宽。

### 非目标

- 不调整 dev-loop、实现面五档分流、supervisor 闸或设备结果门。
- 不调整 `/arch-change` 的常规流程，不削弱契约写防线和契约合并凭据。
- 不取消独立评审、真实分支隔离、机械合并检查、INDEX、ARQ 记账或 remote 发布边界。
- 不新建另一套多档级体系；只保留「常规流程 + 条件加强」。
- 不顺带删除旧脚本和历史制品；它们先服务存量批与条件加强场景。

## 2. 现状问题

当前流程默认由 architect 写逐字节设计，主线程代落地，再由 landing-verifier 证明实际落地等于冻结文本。为防止搬运偏差，又衍生 manifest、landing-summary、沙盘装配、引文与哈希对账、每批 verdict 和 retro。即使改动只是普通治理文档，也可能因路径而进入整套后端。

随着模型直接编辑能力提高，机械搬运错误已减少，继续把每批成本放在字节保真上收益偏低；但设计误解、漏掉消费方、语义越界和假绿测试并不会因此消失。因此应取消的是重复搬运证明，不是独立判断。

现行「只升不降」和路径一票升重还会让临时谨慎永久化：实际风险已经消失时仍不能减负，与按真实风险比例化治理相冲突。

## 3. 目标常规流程

1. **启动。** 新方向由 human 在必要时确认意图、边界和量级。既定 ARQ 波次由主线程读取已裁条目，只作一句启动确认，例如确认本波按哪条已裁方向执行；不重复展开旧修法。只有新增方向、新取舍或既有裁定失效时才重新问 human。
2. **轻设计。** architect 在同一份批记录中写清目标、非目标、关键方案、保护边界和验收标准，并用轻量 `landing-manifest` 列出预计触及的文件或目录、实施归属与保护面标记。清单不含逐行改法，不是逐字节授权。
3. **设计评审。** architect-reviewer 审意图保真、纪律边界、冲突和主要消费方，并决定常规流程是否足够、需要哪些条件加强。reviewer 直接把自己的评审追加到批记录，主线程不再常规转录；否回只要求解决具体发现，不要求每轮重写一篇完整长评。
4. **意图定稿。** 设计 GREEN 后，主线程把状态标为 FROZEN。这里冻结的是意图、边界、关键方案和验收标准，不是未来文件字节；实施中的合理措辞和局部接线可在边界内调整，实质方向变化必须回到 reviewer，新增 human 取舍仍回 human。
5. **作者实施。** architect 在当前 gov worktree 的非 main 分支直接修改被允许的治理文件，运行聚焦自测，并提交真实实现 commit。批记录中追加实际改动摘要、测试命令和关键结果；不再由主线程常规代落地。实际 diff 出预计 scope 时，先补理由并由 reviewer 回审，不带病合并。
6. **实现评审。** reviewer 对实现 commit 的真实 diff 独立评审，核对它是否兑现冻结意图、是否越出 scope、测试是否有判别力。GREEN 记录必须绑定 reviewedCommit 和对应 Git tree；后续任何实现变化都使该钉点失效，只重审新 diff 及受影响发现。reviewer 同样自写评审段。
7. **机械检查与合并。** 主线程跑适用的聚焦检查和前置升级闸，放行后按既有本地合并通道执行，再更新 `delivery/review/INDEX.md`。普通批到此结束，没有强制事后 retro；真实教训才另写 retro 或登记 ARQ。

上述设计、设计评审、实现提交、实现评审等节点，以及实际发生的摩擦和建议，由主线程实时追加到本批 `events.jsonl`，不留到收尾补写。

## 4. 批记录、轻量 scope 与事件证据

保留 `delivery/review/<changeId>/PROPOSAL.md` 路径以避免迁移成本，但把它用作**可追加的批记录**，不再只是落地前的字节合同。常规批依次追加轻设计、设计评审、实施与测试证据、实现评审和必要的条件加强记录。reviewer 对自己两段评审直接执笔并遵守 append-only；architect 不改 reviewer 段，主线程只编排，不常规代写或转录评审。

常规批只需要这份记录、轻量 scope 清单、实时事件流、真实实现 commit 及其测试证据、INDEX 一行。`architect-landing-summary.md`、`landing-verdict.md`、事后 retro 文档和实现凭据 JSON 不再是常规批义务；`implementation-review.json` 留作条件加强凭据。

### 机读锚形态

- 流程标记在头部独占一行，形态为 `> **gov-flow**: direct-implementation`，每份记录恰一条。它不碰 `landing-lane` 解析，也不占 Status 行，因此不与旧 lane 或 `isFrozen` 冲突。
- verdict 复用共享库 `VERDICT_STATUS_RE`，规范形态为行首 `verdict: GREEN` 或 `verdict: BLOCK(...)`，允许既有引用前缀。全记录多条 verdict 一律**后出者胜**；合并与 INDEX 必须共用这一终态，后写 BLOCK 不得被早先 GREEN 覆盖。
- 每轮实现评审以 `reviewedCommit: <40 位小写 SHA>`、`reviewedTree: <40 位小写 SHA>` 各一行钉点；以最后一组字段为最终实现评审，最后一组缺件或无法与 Git 对应即 fail-closed，不回退采用更早钉点。
- 共享库新增一个面向 direct-implementation 的解析谓词，统一读取流程标记、最后 verdict、最后 commit/tree 和 scope；前置升级闸与 INDEX 不各自写正则。SHA 校验复用现有 implementation evidence 的 40 位小写语义。

### 轻量 landing-manifest

沿用 `landing-manifest` 围栏名以减少解析分支，但新流程语义改为**预计 scope**：每行 `路径 | 实施归属 | 保护标记`，第二列取 `architect` 或 `main-thread`，第三列取 `ordinary` 或 `protected`，可再追加一列短理由；允许目录。共享提取先复用 `extractManifestBlock` 取行，再由新谓词解析字段。

新流程闸只要求实际非证据 diff 是预计路径集的子集，允许计划内文件最终未改，不要求旧流程的恰集相等，也不与 §2 锚清单或未来文本对账。证据尾沿用 `isEvidenceTailPath` 豁免；目录、归属和保护标记供 reviewer 判断，理由文本不参与机械判定。旧批的 manifest 恰集语义不变。

### 实时 events.jsonl

每批使用 `delivery/review/<changeId>/events.jsonl`，一行一个 JSON，最小字段为 `{schemaVersion:1, changeId, ts, eventType, actor, note, usage}`。起步事件类型为 design-complete、design-review-complete、implementation-commit、implementation-review-complete、friction、suggestion。

`usage` 固定为 `{agent, wallMs, tokens, toolUses}`，各值可为 null；`tokens` 是 harness 返回的 subagent 总量，不虚构输入/输出分项。主线程单写并按实际返回值补记，不可得写 null 不写 0。事件只供实时记录和后续聚合，fail-open：不进升级闸、不推 INDEX、不设齐全门，坏行跳过并在后续事件注明，不回改历史行。本批已实记 design-complete、design-review-complete、suggestion、friction 四类事件，直接沿用，不补造。

## 5. 删除、保留与条件加强

### 常规默认删除

- 主线程 packet-bound 代落地，以及 landing-lane 分流本身。
- 逐字节 PROPOSAL、manifest 恰集与文本保真链、packet-binding、沙盘装配、哈希和引文对账；轻量 scope 清单保留。
- 每批 landing-verifier、landing-summary、landing-verdict、事后 retro 文档；实时事件流替代形式复盘，独立 retro 仅条件保留。
- 路径一票升重、只升不降、每轮全量重审长记录、每批新造一套验证脚手架。

### 始终保留

- 作者不等于最终审者；设计先审、真实实现后审。
- 非 main 分支、真实 commit/diff、评审钉点和有判别力的验证。
- human 对新方向与例外的裁定；既定 human 明确要求的加强措施不能由 reviewer 自行撤掉。
- 契约、权限、自改递归、合并和 remote 发布保护边界。
- INDEX 与 ARQ 记账；发现的真实问题不得因精简制品而消失。

### 条件加强的触发判据

- **第二独立核验（landing-verifier）**：实际改动核心合并保护逻辑或自身/review-loop 权限；操作高影响且难回滚；实现评审出现独立复核才能消解的争议；或本批暴露了评审未覆盖的真实语义风险。核心合并逻辑的可枚举落点包括前置升级闸、共享闸库、merge-driver、deny-main-push、enforce-scope 和 INDEX landed 推导；只改注释不等于改判定逻辑。
- **新鲜 reviewer 实例**：原 reviewer 已卷入方向争议、上下文锚定影响独立判断，或 review-loop 第五阀要求换独立会话。
- **详细设计与完整普查**：消费方和爆炸半径无法由聚焦检索圈定，设计含穷举/零残留承诺，或多个 canonical 投影的语义关系本身不清楚。
- **Track A**：方向未收敛、存在根本方案分叉，或 human/architect 声明红队；不因触及某个目录自动触发。
- **独立 retro**：发生事故、出现可复用教训或实际结果明显偏离预期。普通顺利完成不补形式复盘。
- **主线程代落地**：触及保护面、architect 机械写权不足，或 human 明确选择；不再是常规默认。

## 6. 双向判档的最低规则

reviewer 在设计评审时只回答两件事：常规链是否足够；若不足，具体启用哪项加强、对应什么事实。每次加强都要留一句可验证的触发理由，不写「重要」「敏感」这类空泛标签。

新证据出现时，reviewer 可以升，也可以降。允许降档的典型事实包括：实际 diff 已缩到非核心语义、争议已由判别测试解决、不可逆操作已移除、完整普查后确认消费方远小于预估。降档同样记录事实和由哪项加强退回常规。

保护边界不属于可降档内容：作者与最终审者不得合并，契约/权限矩阵仍走专门通道，合并机械检查不能因批次看似简单而删除。human 明确指定的加强也只能由 human 解除。

路径只能帮助发现风险，不能单独决定档级。修改核心闸的一句注释不自动需要第二核验；修改一个普通文档但若引入新的全会话权限语义，也不能按文档轻批处理。

## 7. 写权与保护面建议

| 类别 | 建议范围 | 实施方式 |
|---|---|---|
| 普通治理文件 | commands、references、rules、skills、非 review-loop agent 定义、普通 scripts/hooks，以及配套模板 | architect 可直接实施、测试并提交；reviewer 审真实 diff |
| 核心但可由作者实施的机器文件 | 合并闸、INDEX 推导及其共享库 | architect 可实施，但改判定语义时建议第二独立核验 |
| 评审证据 | 两类批的 PROPOSAL.md；条件加强时的 implementation-review.json | architect-reviewer 自写评审，主线程不常规转录 |
| 权限与自改保护面 | scope-map、settings、verify-gates、architect 自身定义、review-loop agent 定义 | 主线程按已审设计处理；既有 human 审核和第五阀不变 |
| 指令根保护面 | `AGENTS.md`、`CLAUDE.md`、`docs/DEV-GOVERNANCE.md` | 建议仍由主线程处理，避免把新直接实施权一次性扩到最高指令层 |
| 契约与 spec | `docs/contracts/**`、canonical spec | 不进入本批新通道，原 `/arch-change` 边界不变 |
| 合并与发布 | 本地 main、remote 分支和 PR | architect 永不合并或发布，仍归主线程/human |

这张表按 R1 收敛：三件指令根全部留在主线程保护面，不下放子集；`docs/glossary.md` 作为纯投影可由 architect 维护，但须先补写槽。目录分类只帮助分工，不能代替对实际语义风险的判断。

human 已明确评审报告应由 reviewer 自写，并已授权主线程预改 scope-map：architect-reviewer 的 allow 现为两类批的 `PROPOSAL.md` 与 `implementation-review.json`，deny 仅保留 `implementation-verification.json`。这项已发生的预开放纳入本批范围，不撤销、不重复修改；R1 的 deny 优先导致零写权问题随之消除。architect 若负责 templates、glossary 或 route-fixtures，仍需主线程同批补齐对应 allow，不能假设现有写槽已覆盖。

## 8. 兼容与切换方案

### 新流程识别

在批记录头部使用 §4 钉死的 `> **gov-flow**: direct-implementation` 单行标记，表示该批采用设计评审加真实 diff 评审的作者实施流程。没有该标记的存量批继续按旧凭据判定，不要求补新字段。

### 前置升级闸

`pre-merge-escalation-gate.mjs` 对新流程增加一条窄分支：要求设计已 FROZEN、最后 verdict 为 GREEN、最后一组 reviewedCommit/tree 与 Git 一致且为待合并 source 的祖先，并重算评审后的非证据改动；评审后若还有实现变化必须重审。它同时要求 `main...reviewedCommit` 的实际非证据 diff ⊆ 轻量 manifest 预计 scope，出界即 BLOCK。普通新批不再要求 landing-verdict、landing-summary 或旧恰集 manifest。

无标记的旧批、旧 author-implementation 批继续走现有分支。契约 diff 的通用检查保持原样，新流程标记不能豁免契约凭据、off-main、废止路径或其它通用检查。

### INDEX

`changes-index.mjs` 保留现有 draft → frozen → landed 状态外形。新批设计 GREEN 后为 frozen；同一份批记录中的最后 verdict 为 GREEN、且最后实现评审 commit 钉点有效后为 landed，后出 BLOCK 必须翻回非 landed。新流程 landed 每次从文件现推，不写入 immutable 缓存。旧批仍从 landing-verdict 推导，`mergedInMain` 的现有 Git 推导不变；`--upsert` 在落地 commit 前运行。

### 旧结构门与 Track A

`landing-fidelity-check.mjs` 不进入常规新链，暂不为兼容而改写；旧批和条件加强场景仍可调用。`proposal-consolidation-check.mjs` 继续服务 Track A 定稿，不变成常规批前置。

切换必须与闸、INDEX、写权和主要 canonical 投影同批原子完成。若只完成其中一部分，则停止合并，不通过临时补造旧制品来制造假兼容。

## 9. 候选触及面与依赖

以下就是本批的轻量 scope 清单；路径可最终未改，但不得无回审超出。

```landing-manifest
.claude/references/gov-change.md | architect | ordinary | 新常规链与双向判档
.claude/commands/gov-change.md | architect | ordinary | 入口同步
.claude/references/architect-operating-model.md | architect | ordinary | 作者实施与评审职责
.claude/agents/architect.md | main-thread | protected | 自身写权与职责
.claude/agents/architect-reviewer.md | main-thread | protected | 双向判档与自写评审
.claude/agents/landing-verifier.md | main-thread | protected | 条件核验定位
.claude/agents/review-moderator.md | main-thread | protected | Track A 接缝
AGENTS.md | main-thread | protected | 治理路由投影
docs/DEV-GOVERNANCE.md | main-thread | protected | 角色、流程与写权投影
docs/glossary.md | architect | ordinary | 制品词汇投影，需补写槽
.claude/references/arch-review-protocol.md | architect | ordinary | 只改治理面后端表述
.claude/rules/repo-structure.md | architect | ordinary | 批记录与事件形态
.claude/skills/merge-to-main/SKILL.md | architect | ordinary | 合并前置同步
.claude/skills/gov-arq/SKILL.md | architect | ordinary | ARQ 启动与下游同步
.claude/references/track-b-judge-brief.md | architect | ordinary | 避免恢复旧全量制品
.claude/route-fixtures/routing.json | architect | ordinary | 路由信号按 description 实改校准，需补写槽
docs/templates/PROPOSAL.template.md | architect | ordinary | 轻量批记录模板，需补写槽
.claude/scripts/pre-merge-escalation-gate.mjs | architect | protected | 新流程兼容分支
.claude/scripts/changes-index.mjs | architect | protected | landed 兼容推导
.claude/scripts/lib/governance-gate-shared.mjs | architect | protected | 共享机读谓词
.claude/scope-map.json | main-thread | protected | reviewer 预开放已在场；补 architect 必要写槽
delivery/review/INDEX.md | main-thread | ordinary | 脚本生成
delivery/review/.changes-index-state.json | main-thread | ordinary | 脚本生成
```

依赖顺序：先由主线程确认 scope-map 已预开放的 reviewer 写槽不再改动，并补齐 architect 对 glossary、template、route-fixtures 的必要 allow；再由 architect 实施普通文件和共享谓词，闸与 INDEX 共同消费谓词；最后由主线程处理保护面并运行生成件。

`routing.json` 是 route-regression 的真实输入：architect-reviewer 的「四查、判档」与 landing-verifier 的「verifier 对偶、landing 清单、frozen PROPOSAL」等必留信号若因 description 修订失效，须同批调整夹具或保留等价路由信号，不能只跑检查不改输入。

`arch-change.md` 预计零改动：gov-change 切断对其制品义务的引用即可，架构面正文保持原样。`landing-fidelity-check.mjs`、`proposal-consolidation-check.mjs`、契约规则和 dev-loop 文件也预计不改；若实施发现必须改动，先回 reviewer 说明依赖并扩 scope。

## 10. 验证方案

优先扩展现有 self-check 和临时 Git 仓夹具，不新造每批一次性的验证框架。

1. **合并闸正向判别**：新流程没有 landing-summary、landing-verdict、事后 retro，但具有轻量 manifest、有效设计与实现评审、真实 commit/tree，能够放行。
2. **机读锚反向判别**：缺设计 GREEN、最后 verdict 非 GREEN、最后一组 SHA 缺件或形态错误、reviewedCommit/tree 与 Git 不符、评审后有实现尾改动、钉点不在 source 祖先链，分别必须 BLOCK；早 GREEN 后 BLOCK 也必须 BLOCK。
3. **scope 两向判别**：实际非证据 diff 是预计集的真子集仍可过，证明没有恢复恰集语义；新增一个清单外实现文件必须 BLOCK。扩清单后则由 reviewer 回审，不是作者自扩自过。
4. **真实消费层夹具**：在临时 Git 仓走新流程 CLI，证明实现 commit 加合法证据尾可过；随后追加实现改动，同一凭据必须变红。
5. **旧批回归**：现有 packet-bound、author-implementation、契约三件凭据和 NOT_APPLICABLE 夹具保持原判定，防止新分支吞掉旧语义。
6. **INDEX 判别**：新记录能从 frozen 到 landed；最后 verdict 翻回 BLOCK 时不得 landed，且现推结果不被旧缓存钉死；无新标记的旧记录仍只读 landing-verdict。
7. **事件流非门**：events 缺失、坏行或 usage 字段为 null 均不改变闸与 INDEX 判定；合法记录使用 `{agent, wallMs, tokens, toolUses}`，不得出现 tokensIn/tokensOut。
8. **文档与路由检查**：按最终触及面跑引用/链接检查、`route-regression-check.mjs` 和 `lint-descriptions.mjs`；核对架构面、契约门和 dev-loop 的现行义务没有被治理面减法误改。
9. **本批真实合并预演**：在待合并 source 上运行更新后的前置升级闸，不以 self-check 代替真实 CLI 结果。

每个新增断言都要有能使其变红的输入；只证明「文件被改了」或「命令执行过」不算通过。

## 11. 风险与回滚

- **语义误判仍在。** 由设计审、真实 diff 审和消费层判别测试承担；命中条件时再加第二独立核验，而不是恢复每批字节对账。
- **单份记录可能被作者事后改写。** reviewer 凭预开放写槽自写评审段并保持 append-only，architect 不改该段；主线程检查证据尾 diff，机械闸钉住 reviewedCommit/tree。Git 能证钉点真实，仍不能独立证明「reviewer 看过」，该信任强度与现行 PROPOSAL §7 相同；若发生真实篡改或争议，条件加强为 implementation-review.json 或第二独立核验，不常规复制全部制品。
- **新流程标记被误用。** 设计 reviewer 必须确认该标记与批次适用范围；契约和保护面检查不消费该标记作豁免。
- **兼容分支造成双源漂移。** 新旧凭据解析共享最小谓词，旧路径保持回归夹具；不再另建第三套状态名。
- **回滚。** 以整批可逆 revert 恢复旧闸和 canonical。若回滚时仍有未合新批，先暂停这些批的合并并恢复兼容读取，不用旧闸硬套，也不事后伪造旧凭据。

## 12. 本批自身的风险建议

本批方向已由 human 收敛，不建议仅因触及面多而转 Track A；但它会修改核心合并凭据语义、architect/reviewer 职责及可能的权限矩阵，属于高敏感、可在本地分支回滚的条件加强候选。

本批按 R1 建议保留 landing-verifier 第二独立核验，原因是同时改动核心合并保护逻辑与自身/review-loop 权限，且本批是新流程首个消费者。核验重点是独立复跑新旧合并闸消费层、真实 diff 和旧批回归；这是一次的条件加强，不恢复为后续常规批的强制 verifier。是否需要新鲜 reviewer 实例或完整普查，仍由 architect-reviewer 按真实 diff 判定。

## 13. 请 R2 重点核验的点

1. §4 的流程标记、后出者胜 verdict、最后一组 reviewedCommit/tree 与共享谓词是否已无歧义，且不与旧解析器碰撞。
2. 轻量 manifest 的字段形态和「实际非证据 diff ⊆ 预计 scope」是否完整保留，未暗回旧恰集或逐字节保真。
3. reviewer 自写权限是否已按 human 授权如实纳入；scope-map 预开放不被撤销或重复改，architect 对 glossary、template、route-fixtures 的写槽依赖是否闭合。
4. events 的 usage 是否严格为 `{agent, wallMs, tokens, toolUses}`、各值可 null，且事件流始终 fail-open、不参与合并和 INDEX 判定。
5. 本批第二独立核验保留为条件加强，arch-change.md 与架构/契约/dev-loop 语义不被顺手改写。

## 评审与实施记录（后续追加）

（设计评审、实施与测试证据、实现评审均追加在本节；本稿不预写结论。）

### R1 - 2026-10-29 - architect

> 评审范围：本设计稿 §1–§13 全节；human 两条已确认边界（① architect 直接实施扩到普通治理文件、保护面例外；② ARQ 波次一句启动）与本次「轻设计 → 设计评审 → 作者实施 → 真实 diff 评审」试验流程适配均已纳入。评审中段协调方转入 human 两条新设计输入（轻量 scope 清单恢复、事件流替代 retro），以 F3 纳入。亲核基线（全部实读，非采信转述）：AGENTS.md、.claude/references/arch-review-protocol.md、.claude/references/gov-change.md、.claude/commands/gov-change.md、.claude/references/arch-change.md（共用节）、.claude/scope-map.json、.claude/scripts/pre-merge-escalation-gate.mjs（全文）、.claude/scripts/lib/governance-gate-shared.mjs（全文）、.claude/scripts/changes-index.mjs（1–260 行）、.claude/scripts/route-regression-check.mjs、.claude/route-fixtures/routing.json、agents 四件关键行、architect-operating-model.md 节表、全仓 landing-verdict / landing-verifier / landing-fidelity / landing-manifest 消费面 grep（.claude 与 docs 各一轮）。

verdict: BLOCK(F1,F2,F3)

**F1（SHOULD · §9 候选触及面遗漏 route-fixtures/routing.json 与三处 architect 写槽事实）**

证据：`route-regression-check.mjs:16` = `const CFG = JSON.parse(readFileSync(join(ROOT, '.claude', 'route-fixtures', 'routing.json'), 'utf8'));`；`routing.json:31-36` 钉 architect-reviewer 必留信号（`四查`、`判档`），`:62-66` 钉 landing-verifier 必留信号（`verifier 对偶`、`landing 清单`、`frozen PROPOSAL`）。本批要改 architect-reviewer.md（双向判档）与 landing-verifier.md（改条件消费）的正文与 description——上述信号子串可能失效；authoring-agents-skills 规则明文「改 description 后须复跑仍绿」，复跑红则 routing.json 必须同批改，而 §9 未列它（§10 第 6 条只说「跑」该检查，没把夹具本身列入触及面）。另三处写槽事实：architect 的 scope-map allow（scope-map.json:161-175）不含 `docs/templates/**`、`docs/glossary.md`、`.claude/route-fixtures/**`——§9 把 PROPOSAL.template.md 与 glossary 分给「architect 或主线程 / 按最终写权边界处理」，凡最终定 architect 的都必须同批扩 allow，否则作者实施第一步即被 enforce-scope 拦。评审执行中另实测一处本稿未涉的机械事实：enforce-scope.mjs:306-307 为 deny 先行判定，而 architect-reviewer 的 deny = `["**"]`（scope-map.json:263）——typed architect-reviewer 当前对全仓零写权、含其名义 §7 窄写槽；本 R1 即因此无法自行 append，由主线程转录。新流程要求两轮评审都 append 进同一份批记录，「谁物理执笔」（scope-map 窄豁免 vs 主线程转录边界）必须在 §4 钉死。

建议修法方向：§9 表补 route-fixtures/routing.json 一行（实施归属随最终写权边界定）；scope-map 行补注上述三处写槽缺口与 reviewer 执笔边界，实施期按最终归属一次调齐，不靠实施中途回环。

**F2（SHOULD · 机器锚形态未钉死——新流程的全部凭据收敛到批记录内机读字段，但只给「建议语义」，有重复 260920 verdict-anchor 病灶类的具体风险）**

§4/§8 把闸与 INDEX 的凭据收敛为：gov-flow 标记、最终 verdict、reviewedCommit、reviewedTree。现行单源判据全部行首形态敏感：lib/governance-gate-shared.mjs:727 `VERDICT_STATUS_RE`（行首 `verdict:`，可带 `>` 前缀与反引号）；pre-merge-escalation-gate.mjs:111 FROZEN 判据 `/^[>\s]*\*\*状态：FROZEN v\d+\.\d+/`（有在册教训：写成 `FROZEN（v0.4.1）` 闸即判不出）；changes-index.mjs:202-232 deriveLanded 与 isGreenVerdict 同取「后出者胜」。三个具体缺口：

1. gov-flow 标记无精确行形态，且须验证不与 parseLandingLane（lib:106-119）及 isFrozen 状态行解析碰撞；
2. 最终实现评审 verdict 行未钉「行首 `verdict: GREEN`（复用 VERDICT_STATUS_RE 单源）」+ 未钉「最终 GREEN 必须是全记录最后一条 verdict 形态行」——append-only 多轮评审下，其后任何一轮补写的 verdict 行都会按后出者胜静默覆写闸与 INDEX 的判定；
3. reviewedCommit/reviewedTree 未钉字段形态——建议沿用 implementation-review.json 的键名与 40 位小写 SHA 语义（lib readImplementationEvidence:128-143 的校验谓词可平移），闸与 INDEX 共用同一 lib 谓词（§11「共享最小谓词」句的落点应明确到具体 RE）。

若采纳 F3 的轻量 scope 清单与事件流，二者的行形态与 eventType 闭集同样进本条钉死范围（scope 清单复用 extractManifestBlock 单源；events 沿 pod-events.jsonl 先例的 JSON 行 + schemaVersion=1）。

建议修法方向：§4 或 §8 补一段「机读锚形态」，各件给一字面示例行 + 单源谓词指向。

**F3（SHOULD · 意图保真：human 评审中段补充的两条设计输入未落稿，现稿多处与其冲突）**

human 评审中段原话两条：①「landing manifest，我觉得这个可以加。就算做一个轻的方案，也可以去看一下自己可能会改哪些 scope。这在设计阶段应该就能判断出来，比如改哪个文件，但不涉及里面具体改什么。」；②「Retro 的话你可以参考 Dev Loop 里边 Event 的记录方式：我们在每次 architect 设计完方案、arch Review 审完方案之后，都可以实时去写这个 Event。……那将来我们在优化这套 governance 的时候，就有数据可查了。」两条与现稿冲突处（必须修订的段落）：

- §1 目标段「普通批……不再要求整套 landing 制品和 retro」——需改为「保留轻量 scope 清单与实时事件流，废逐字节保真链与事后 retro 文档」；
- §3 步骤 2「设计可以列候选文件，但不形成逐字节授权清单」——候选文件列表须升格为 scope 清单义务；步骤 7 须加各节点实时事件与收尾；
- §4 机读最小字段——须纳入 scope 清单作为机械消费面；
- §5 删除清单第 2 行整词删「landing-manifest」、第 3 行删「独立 retro」——分别改为「逐字节 manifest 保真链（轻量 scope 清单保留）」「事后 retro 文档（实时事件流替代；独立 retro 条件保留）」；
- §10 验证 1「新流程没有 …manifest、retro」——改述并新增 scope ⊆ 判别与「事件流不拦 merge」验证项。

两条的最小形态裁决（机械可行、不重新膨胀）：

1. **轻量 scope 清单**：最小字段 = 一行一条：路径（文件或目录）、实施归属（architect / 主线程 / human）、保护面标记（保护面 / 普通），可选一句理由；禁旧串/新串、哈希、行级内容。机械消费 = 新流程闸分支做 ⊆ 判定：`main...reviewedCommit` 非证据改动 ⊆ 清单（证据尾豁免复用 isEvidenceTailPath——批记录目录 + INDEX + state），实施 diff 出清单即 BLOCK 点名，回审规则 = 补记理由扩清单或重审新 diff，二者都回 reviewer、不回 human（触保护面除外）。解析复用 extractManifestBlock 单源；围栏名沿用 `landing-manifest` 或另起 `gov-scope` 二选一——沿用旧名须在文档钉死「新流程语义 = 预计集（⊆ 判定）、旧批语义 = 恰集（= 判定）」，另起名更干净、代价是 lib 加一个提取器。防膨胀闸：闸只读路径与归属两列做集合判定，不读理由文本；清单不与 §2 锚清单对账（parseSection2AnchorLists 义务随旧链退役），允许目录粒度。此清单同时收窄 OQ2 的 spoof 面（作者把实现藏进批记录目录的行为仍不可检——清单不覆盖证据尾文件，OQ2 结论不变、不追加义务）。
2. **事件流替代 retro**：存储 = `delivery/review/<changeId>/events.jsonl`（对偶 dev lane pod-events.jsonl 先例；在 isEvidenceTailPath 豁免集内，天然不触发闸尾改判定）。最小 schema = `{schemaVersion:1, changeId, ts, eventType, actor, note, usage}`；eventType 闭集起步 = design-complete / design-review-complete / implementation-commit / implementation-review-complete / friction / suggestion；usage = `{agent, wallMs, tokensIn, tokensOut}` 各值可 null——agent 耗时/token 由主线程在 Agent 返回后按 harness 实际 usage 补记，不可得记 null 不记 0（不要求 agent 猜测）。写者边界 = 起步主线程单写（编排元数据、非变更内容，不破「主线程不起草变更内容」）；architect/判官的摩擦与建议先落在评审记录 prose，由主线程转事件。fail-open：events 不进升级闸、不进 INDEX 推导、不设「事件齐全」门；坏行跳过并在后续事件中注明，不静默改写已写行（append-only）。聚合：本批不建聚合器；schema 稳定 + ts 在场即保证后续 loop-metrics / pod-events-aggregate 式消费可聚合，不制造新门。

**§13 五开放点逐条裁决**

1. **指令根边界**：AGENTS.md / CLAUDE.md / docs/DEV-GOVERNANCE.md 全部保留主线程保护面，不下放子集。理由：本批自己就要改 AGENTS.md 治理路由节（自指批、最高爆炸半径），三件是一切边界的定义层；scope-map 现状 deny（:189-191）与 human「契约/权限矩阵等保护面例外」一致。唯一可安全下放的子集 = docs/glossary.md（纯投影词汇表、无权限语义）——下放须同批给 architect allow（见 F1）。
2. **单份记录替代 implementation-review.json**：常规批足够，前提 = F2 三件形态钉死 + F3 scope 清单落地。残余缺口如实陈述：implementation-review.json 是 scope-map 对 architect 机械 deny 的判官专写面（scope-map.json:192-193），是 author-implementation lane 唯一作者改不了的凭据；并入 PROPOSAL 后判官段可被作者改写，闸从 Git 重算只能证 commit/tree 真实、证不了「判官看过该 commit」。该信任强度与 packet-bound lane 的 §7 GREEN 锚相同（readGovAnchors 的 green 锚本就读 architect 可写的 PROPOSAL 文本），human 已明示接受该级信任。结论：不新增常规义务；implementation-review.json 保留为条件加强凭据（核心合并逻辑批 / 争议批），lib readImplementationEvidence 与闸旧 author-implementation 分支原样保留、服务存量批。
3. **INDEX 状态外形**：复用 frozen/landed、只换 landed 推导来源——足够，不需要新完成标记（新标记 = 第三套状态名，与 §11「不再另建第三套状态名」自相矛盾）。两条实施注记：新流程 landed 须从文件现推、不入 immutable 缓存（changes-index.mjs:252 注释「frozen/landed 从文件现推」的现行语义恰好满足，保持即可）；`--upsert` 在落地 commit 之前跑（连续两批中招的在册教训，INDEX 静默漏带生成件）。
4. **第二独立核验触发宽窄**：方向正确，建议把「核心合并保护逻辑」钉成可枚举文件集——pre-merge-escalation-gate.mjs、lib/governance-gate-shared.mjs、merge-driver.mjs、hooks/deny-main-push.mjs、changes-index 的 landed 推导、enforce-scope hook——命中枚举集即触发，机械可判；「真实语义风险」保留为判官裁断触发，理由句按 §6 可验证要求留痕。不算过宽：代价只是一次独立复核，且只落在枚举集 / 高影响难回滚 / 争议三类上。
5. **本批第二独立核验**：建议启用（§12 作者自荐成立）——本批同时命中「核心合并保护逻辑自改」（闸凭据语义 + INDEX 推导 + 共享库）与「自身权限」（architect/reviewer 职责 + 可能 scope-map）两类触发，且是新流程第一个消费者。第二核验重点 = 独立复跑新旧合并闸消费层夹具 + 真实 diff + 旧批回归夹具全绿。**架构面共用文档最小同步范围**：arch-change.md 很可能零改动——治理面对其「轻路径落地通道 / Track B 制品义务」的消费点在 gov-change.md:27 与 :31 的引用句（「本文件不重复定义、只引用」），切断引用即可，arch-change.md 正文照旧服务架构面；arch-review-protocol.md 只动治理面默认后端表述（双轨路由第 4–7 步的 gov 语义、landing 通道节 gov 半边、轻路径判档 gov 行、不动清单行的 landing-verifier 措辞），实施评审时逐行核不误伤架构面段。

**升级 human 项**：无——三条 finding 均属 human 已拍边界与已明示输入范围内的设计完备性 / 意图保真问题，不触方向收敛；OQ1 裁决与 human 边界①一致，F3 两条本就是 human 的新输入，此处只做最小形态落地裁决。

**第二独立核验（本批）**：建议启用，见 OQ5 裁决。

**亲核留痕**：Read pre-merge-escalation-gate.mjs 全文（:111 frozen 形态、:127-168 author-implementation 凭据 Git 重算、:669 `else if (!a.landingVerdict) push('Track B：landing-verdict 不在产')`）、lib/governance-gate-shared.mjs 全文（:727 VERDICT_STATUS_RE、:101-143 lane 与凭据谓词、:121-126 isEvidenceTailPath）、changes-index.mjs:1-260（:194-232 deriveLanded 三层兜底、:252 现推注释）、scope-map.json 全文、route-regression-check.mjs:16、routing.json:1-80、enforce-scope.mjs deny 先行段（:306-311）、gov-change.md 全文（:14 只升不降、:19 路径一票 Track B、:27/:31 对 arch-change.md 的制品义务引用）、arch-review-protocol.md 全文、commands/gov-change.md 全文；Grep 全仓 landing-verdict|landing-verifier|architect-landing-summary|landing-fidelity 消费面——脚本侧命中 changes-index（landed 推导）、pre-merge-escalation-gate（:108/:212 契约凭据 + :669 gov 锚）、landing-fidelity-check（结构门本体，本批不改）、retro-efficiency.mjs:40 与 debt-ledger-shared.mjs:76（仅角色名单/正则，不受影响）、brief-inputs.mjs:637（注释）——均不构成 §9 之外的新触及面遗漏；docs 侧 DEV-GOVERNANCE.md（:28/:52/:137/:177）、glossary.md:40 已在 §9 清单内。

### R2 - 2026-10-29 - architect

> 评审范围（R2 聚焦）：R1 三条 findings（F1 route-fixtures/写槽、F2 机读锚形态、F3 human 两条新输入落稿）的修订到位性、修订是否引入新矛盾、§13 五个待核点；不重做 R1 全量审查。写权实测：本 R2 由 typed architect-reviewer 经手工窄开放后的 scope-map 自行 Write append（见文末实测行）。

verdict: GREEN

**F1 闭合（到位）**：`routing.json` 已入 §9 轻量 scope 清单（`.claude/route-fixtures/routing.json | architect | ordinary`，注明「需补写槽」），§9 依赖顺序段钉「主线程补齐 architect 对 glossary、template、route-fixtures 的必要 allow」，且 §9 新增段亲引 route-regression 消费关系（「architect-reviewer 的『四查、判档』与 landing-verifier 的『verifier 对偶、landing 清单、frozen PROPOSAL』等必留信号若因 description 修订失效，须同批调整夹具或保留等价路由信号」）——R1 点名的夹具本体 + 三处写槽缺口全部入圈。reviewer 执笔边界随 human 授权落定：scope-map 亲核（grep verbatim）architect-reviewer allow = `delivery/review/*arch-review-*/PROPOSAL.md`、`delivery/review/*gov-review-*/PROPOSAL.md`、两类批 `implementation-review.json`，deny 仅 `delivery/review/**/implementation-verification.json`——deny 优先导致的零写权病灶消除，且 verification 件仍判官不可写、独立核验者独立性保留。§3 步骤 3 / §4 首段 / §7 新增「评审证据」行把「reviewer 自写、主线程不转录」钉成流程义务。

**F2 闭合（到位）**：§4 新增「机读锚形态」节四条：① 流程标记钉死为 `> **gov-flow**: direct-implementation` 恰一条（本稿头部 line 4 即活例）；② verdict 复用 `VERDICT_STATUS_RE` 行首形态 + 显式「后出者胜」+「后写 BLOCK 不得被早先 GREEN 覆盖」；③ reviewedCommit/reviewedTree 各一行 40 位小写 SHA、以最后一组为准、缺件 fail-closed 不回退早钉点；④ 共享库新增 direct-implementation 解析谓词单源，「前置升级闸与 INDEX 不各自写正则」。碰撞面亲核（R1 已读单源正则）：`parseLandingLane` 只认含 `landing-lane` 标签的行（lib:109）、`isFrozen` 只认 Status/状态行（lib:65-66）、`VERDICT_STATUS_RE` 标签闭集 = verdict/总判/总判定/结论/复验结论（lib:727）——`gov-flow` 行对三者均不命中，无碰撞。§10 第 2 条新增「早 GREEN 后 BLOCK 也必须 BLOCK」判别，直接覆盖后出者胜的失败形态。

**F3 闭合（到位）**：R1 点名的五处修订逐处在场——§1 目标段（「保留轻量 scope 清单与实时事件流」）、§3 步骤 2（manifest 升格为义务 + 「不含逐行改法，不是逐字节授权」）、步骤 5（出 scope 回审「不带病合并」）、§3 末段与 §4 events 节、§5 删除清单改写（「manifest 恰集与文本保真链……轻量 scope 清单保留」「事后 retro 文档……独立 retro 仅条件保留」）、§10 第 1/2/3/7 条。events.jsonl 实记亲核：本批目录 `events.jsonl` 五行在场，schema 与 §4 钉死逐字段一致（`schemaVersion:1`、六类 eventType 中四类已实记、usage 为 `{agent, wallMs, tokens, toolUses}` 且 null 在场、无 tokensIn/tokensOut），human suggestion 与 R1 friction 事件均在档——实时记录机制已在本批自证运转。

**§13 五个待核点结论**：

1. **机读锚无歧义、不碰撞**：成立，见 F2 闭合段碰撞面亲核；共享谓词单源 + SHA 语义复用 implementation evidence 现有谓词，无双源分叉面。
2. **⊆ scope 语义闭合、未暗回恰集**：成立。§4「新流程闸只要求实际非证据 diff 是预计路径集的子集，允许计划内文件最终未改，不要求旧流程的恰集相等，也不与 §2 锚清单或未来文本对账」+ §8 闸判据「实际非证据 diff ⊆ 轻量 manifest 预计 scope，出界即 BLOCK」+ §10 第 3 条双向判别（真子集可过 = 反恰集判别；清单外文件 BLOCK；扩清单由 reviewer 回审「不是作者自扩自过」）。逐字节保真零回潮：清单只有路径/归属/保护标记/理由四列，「理由文本不参与机械判定」。本批 §9 自身即以该形态给出 23 行清单，可作首例样张。
3. **reviewer 自写按 human 授权如实纳入、预开放不重复改**：成立。scope-map 亲核 verbatim（见 F1）；§7 明文「这项已发生的预开放纳入本批范围，不撤销、不重复修改」；architect 对 glossary、template、route-fixtures 的写槽依赖以清单三行「需补写槽」+ 依赖顺序段闭合。NIT 级观察（不阻 GREEN）：清单条目允许目录，新谓词实现目录对实际文件路径的前缀/段匹配规则时需一句钉死（建议 = 按路径段前缀匹配，与 isEvidenceTailPath 同风格），实现期随谓词 self-check 落一行断言即可。
4. **events usage 严格、fail-open**：成立。§4 钉死 `{agent, wallMs, tokens, toolUses}`、tokens 为 harness 总量不虚构分项、不可得写 null 不写 0；fail-open 四不（不进升级闸、不推 INDEX、不设齐全门、坏行跳过不回改）+ §10 第 7 条「events 缺失、坏行或 usage 字段为 null 均不改变闸与 INDEX 判定」判别在场；且「不得出现 tokensIn/tokensOut」反向断言防 R1 旧字段名回潮。实记文件亲核一致（见 F3）。
5. **第二核验保留为条件加强、跨面零顺笔**：成立。§12 明确「这是一次的条件加强，不恢复为后续常规批的强制 verifier」；核心合并逻辑已枚举化（前置升级闸、共享闸库、merge-driver、deny-main-push、enforce-scope、INDEX landed 推导），「只改注释不等于改判定逻辑」排除注释级误触发；§9 清单 23 行全落治理面，无任何 dev-loop/契约/架构面语义文件，`arch-change.md` 预计零改动（切断 gov-change.md 引用句即可，与 R1 OQ5 一致）。

**新矛盾扫描**：未发现修订引入的方向级或机械级矛盾。两点 NIT 级留痕（均不阻 GREEN、实现期一句话可钉）：① 目录条目匹配规则（见待核点 3）；② `hasArchLoopContractEvidence` 会扫 diff 中任何 `delivery/review/*/PROPOSAL.md` 的 manifest 围栏做契约覆盖判定——新流程批按边界不触契约且无 landing-verdict，联立必不成立、无误放行路径，但实施期可在闸新分支注释一句「新流程批不得消费本判定」，防未来有人把契约批改挂 gov-flow 标记。

**升级 human 项**：无。

**第二独立核验（本批）**：维持建议启用（§12 已按 R1 收敛为一次条件加强）——理由不变：本批同时改核心合并凭据语义与自身权限，且是新流程首个消费者；核验重点 = 独立复跑新旧合并闸消费层夹具、真实 diff、旧批回归夹具全绿。

**写权实测**：本 R2 记录由 typed architect-reviewer 以 Write 工具直接 append 至本 PROPOSAL——scope-map 窄开放（allow 两类批 PROPOSAL.md + implementation-review.json，deny 仅 implementation-verification.json）后的首次真实写权验证，R1 friction 事件所记病灶闭环；若本次写入被拦，将按纪律报告机械输出不绕行（实际结果以本行在场为准）。

**亲核留痕**：Read 修订稿全文（line 4 标记活例、§4:52-69 机读锚/manifest/events 三节、§5:76-77、§9:149-179 清单与依赖、§10:185-193、§11:200、§12:209）；Grep scope-map architect-reviewer 段 verbatim（allow 四模式 / deny 仅 implementation-verification.json）；Read `events.jsonl` 全文五行（schema 逐字段核对、四类事件 + human suggestion + friction 在档、usage null 形态在场）；Grep enforce-scope.mjs `matchesAny`（:52-54 glob 判定入口）确认 allow 形态可匹配 PROPOSAL.md——本写入即终验。

### 作者实施与测试证据 - 2026-10-29 - architect

实施摘要：共享库新增 direct-implementation 单源解析，覆盖 gov-flow、后出 verdict、最后 commit/tree 钉点与轻量 scope；前置升级闸新增真实 Git 钉点、scope 子集和证据尾检查，旧 lane 与契约检查保留；INDEX 新批从批记录现推 landed，不读 events、不写旧 landed 缓存。`/gov-change` 入口、architect 操作模型、治理面协议投影、repo-structure、merge/gov-arq skills、judge brief、glossary 和模板已同步。route fixtures 未改，因为现有必留信号仍符合实际职责；arch-change.md、dev-loop、契约门与 main-thread 保护面未改。

实际实现文件集（PROPOSAL 自身另列）：

- `.claude/scripts/lib/governance-gate-shared.mjs`
- `.claude/scripts/pre-merge-escalation-gate.mjs`
- `.claude/scripts/changes-index.mjs`
- `.claude/references/gov-change.md`
- `.claude/commands/gov-change.md`
- `.claude/references/architect-operating-model.md`
- `.claude/references/arch-review-protocol.md`
- `.claude/rules/repo-structure.md`
- `.claude/skills/merge-to-main/SKILL.md`
- `.claude/skills/gov-arq/SKILL.md`
- `.claude/references/track-b-judge-brief.md`
- `docs/glossary.md`
- `docs/templates/PROPOSAL.template.md`
- `delivery/review/261029-gov-review-gov-loop-simplification/PROPOSAL.md`（本实施证据追加）

测试命令与结果：

- `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check`：117/117 通过。含 direct CLI happy path（无旧 landing 制品、manifest 真子集、events 坏行不影响）、后出 BLOCK、错 tree、非祖先钉点、真实出 scope、评审后实现尾改，以及旧 lane / 契约 / NOT_APPLICABLE 回归；目录 `foo/bar/` 与 `foo/barley` 判别通过。
- `node .claude/scripts/changes-index.mjs --self-check`：57/57 通过。含 direct landed、后出 BLOCK、坏 events 不参与、旧缓存不钉死及旧 verdict 推导。
- `node .claude/scripts/landing-fidelity-check.mjs --self-check`、`node .claude/scripts/proposal-consolidation-check.mjs --self-check`：全部通过，旧结构门行为未回归。
- `node .claude/scripts/route-regression-check.mjs`：52 ok / 0 fail；加 `--self-check`：104 ok / 0 fail。
- `node .claude/scripts/lint-descriptions.mjs`：扫描 16，flagged 0。
- 三个改动脚本 `node --check` 全部通过；`git diff --check` 无格式错误（仅既有 Windows CRLF 提示）。

范围偏差：无。保护面、scope-map、INDEX/state、events.jsonl 与 `.codebase-memory/**` 均未由作者修改或纳入实现提交。真实本批 CLI 在实现 commit 后先作评审前预演；最终 GREEN 预演仍须 reviewer 钉点及 main-thread 保护面齐备后复跑。

### 保护面实施与复核 - 2026-10-29 - main-thread

按 FROZEN §9 的实施归属，主线程完成 `.claude/agents/architect.md`、`architect-reviewer.md`、`landing-verifier.md`、`review-moderator.md`、`AGENTS.md` 与 `docs/DEV-GOVERNANCE.md` 的治理面投影：治理面采用轻设计、作者实施、真实 diff 审和条件加强，架构+契约面及旧批规则保留。scope-map 的 reviewer 窄写槽和 architect 普通文件写槽按 human 授权与设计预开放，未开放指令根、自身定义或独立验证凭据。

主线程复跑结果：`route-regression-check --self-check` 104 ok / 0 fail；`lint-descriptions` 扫描 16、flagged 0；`enforce-scope --self-check` 全绿；前置升级闸 117/117、changes-index 57/57；`git diff --check` 无格式错误。以上为保护面实施证据，不替代 reviewer 的最终真实 diff 评审与第二独立核验。
