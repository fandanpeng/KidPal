# PROPOSAL · 260908-gov-review-input-surface-frontload

> **状态：FROZEN v1.0 · 2026-09-09 · 主线程凭「human 立项 ack（2026-09-08）+ R4 GREEN」标定，内容自此冻结、落地后不回改本文件。** 授权链：① human 立项 ack；② human 五裁（2026-09-09：OQ1=(a) 扩 over-scan 新型 / OQ2=(a) 不预授权 digest / **OQ3=(a) 且连同 T1 第六条 + T4 输入面改写连批** / R1-F1 修法 = 全量重跑刷新 / 消费方②③档（只读参考树 · 黑盒）留 ARQ 不扩）；③ **human 熔断三选一裁 ③「换新判官实例再战 1 轮」**（2026-09-09，闭合 §8-E 所留「裁断另行登记」记录链——R4 残余观察 1 的处置）。
> **评审轨迹**：R1 NEEDS-WORK（BLOCKING 3 / SHOULD 7 / COULD 4）→ v0.2 十四条全闭合 → R2 NEEDS-WORK（SHOULD 2 / COULD 4）→ v0.3 六条全闭合 → R3 NEEDS-WORK（SHOULD 1，同判官三轮满 → 熔断）→ v0.4 落 R3-F1 verbatim 替换句（§8-E）→ **R4 零上下文新判官实例：GREEN，零 findings，census 第三次独立重算逐数吻合、新矛盾扫描五处全闭合**（`architect-review-r4.md` + §7 R4 条目）。§8-B 两件主线程加码经 R2 定性 = OQ3(a) 必要配套，R4 复核五处同口径未破，维持。三轮共同结论：批次方向、G1 归类（确认不改判）、Track B 降档（力度足额）稳定未动摇；残余四项非 finding 观察见 R4 记录（含 T4 Step 9 重跑触发词窄松动 → 交 retro，不改冻结文本）。
> **承载 ARQ**：无既有 ARQ。来源 = human 2026-09-08 指示的 pod 遥测专项分析（v0.2 刷新后口径 = 13 包 / 91 条 dispatch-completed 事件，见 §1 P1 修订注）；动机件之一 `260907-g-relay-fix` pod-retro「下次改进 1」由本批机械化承载，不另立 ARQ。姊妹项（dispatch-stamp 补记实际生效模型 + token 分项）为已执行测出的机械改进项，走 gov-arq 快路径另行登记，见 §6。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。升级判据核对：不触 `docs/contracts/**`（契约面为零）、非跨系统结构性变更（治理/流程面单侧）→ 不升 Track A。**R1 判官独立复核降档成立、力度足额，无方向级升级 human 项（评审记录 D-2 节）。**
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：闸向严不向松、**历史**包零追溯、有 manifest-missing 降级兜底故 downside bounded；但改四个 agent def 的派发/交付行为 + 扩承重闸判定，非纯成文。**追溯口径 v0.2 修正（R1/F4）**：零追溯对**已 merge 历史包**为真，对 **landing 时在飞（current）包**有一次性过渡摩擦——补救动作见 T3 末「过渡注记」，不改变档位）。
> **review profile**：G1+G2+G3（触面 = `docs/DEV-GOVERNANCE.md` G1 成文 + 四 agent def 与三份 references G2 + **三脚本** G3〔`brief-inputs.mjs` 新增 / `taskpackage-required-check.mjs` 扩判定 / `pod-events-aggregate.mjs` 加列〕；**G1 归类经 R1 判官裁断确认不改判**：DEV-GOVERNANCE §6 表 `:212` 字面列名该文件，batch-2 先例「标 G2 并声明 G1 为零」与 SSOT 表矛盾、不予沿用，v0.1 的「请判官复核改判」请求已闭合；G1 不改轨道——非 Track A 判据，Track B 通道明文 G1/G2/G3 全落；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/DEV-GOVERNANCE.md
.claude/scripts/brief-inputs.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/agents/acceptance-author.md
.claude/agents/builder.md
.claude/references/task-intake-and-slicing.md
.claude/references/dev-loop.md
.claude/references/dev-loop-operating-system.md
.claude/scripts/pod-events-aggregate.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 dev-loop 子 agent（acceptance-author / builder）的输入面从「开放自扫」收敛为「起包期派生的封闭清单 + 超读申报」，压缩重派发挂钟长尾，并把 g-relay-fix 实证教训「跨端验收必须读对端解析器源码」变成有闸承载的起包字段。

**现状差距**：TaskPackage 的引用字段已经锚点化（`taskpackage-required-check.mjs` 头注 `:11`：「sourceRefs 必填非空语义收窄为代码范围锚点」——R1/F10 行号校正，v0.1 误标 `:12`），但没有任何一步把它们在派发期展开成封闭清单；agent 定义里写的输入是开放措辞（`builder.md:24`「相关 contracts、events、shared types、system spec」、`acceptance-author.md:24`「相关 `docs/contracts/**` 与系统 spec」），子 agent 每次派发各自重新发现输入面。跨端性在既有机械字段上不可见（§1 P2 全量普查实证），无字段、无闸承载。

**非目标（本批不做）**：
- 不做契约/spec 节选 digest 落 brief（影子副本风险，phase-2 凭 §5 仪表盘数据再议；**OQ2 已由 human 2026-09-09 裁 (a) 不预授权**）；
- 不动任何模型档位/派发 model 覆盖规则（MODEL-TIER 条款原样；dispatch-stamp 遥测补全另走 ARQ，§6）；
- 不触契约面、不触 arch-review 侧角色（scenario-author/challenger 等的 charter 机制是对偶先例，不是改动对象）;
- 不追溯**已 merge 历史包**（闸只读 current taskpackage，历史包天然不触发）；**landing 时在飞包非零影响，一次性过渡见 T3「过渡注记」（R1/F4）**；
- **（v0.2 新增，human 2026-09-09 裁）不扩消费方②③档**：`crossEnd.counterparts` 本批只承载**自有实现面**消费方（`firmware/**`、`server/**`，`{path, symbol}` 可 grep 解析）；只读参考树（`ESP32-S3-RLCD-4.2-Demo/**`，版本控制外、跨 checkout 路径不稳）与黑盒消费方（OpenClaw 引擎、物理声/显表现——无源码可指、须另立 live oracle 锚语义）双双留 ARQ，见 §6。

## 1. 问题与根因（地面真值）

### P1 · 重派发长尾的形态 = 开放输入面自扫

遥测现跑（**v0.2 按 human 2026-09-09 对 R1/F1 的裁断 = 全量重跑刷新**；as-of commit `c1e26a2`〔2026-09-08T23:40+08:00〕；命令 = 对 `.claude/taskpackages/*/pod-events.jsonl` ∪ `changes/dev/*/pod-events.jsonl` 按 eventId 去重、筛 `eventType=dispatch-completed`，一次性 python 现跑，原文见本节末「复现命令」）：**91 条** dispatch-completed 事件（**13 包**，2026-08-31T18:48Z → 2026-09-08T07:53Z），其中 **47 条** token 精确（`tokenUsage.status=exact`）。

**口径披露（v0.1 缺、R1/F1 补）**：「事件 n」= 全事件数 / `durationMs` 有效子集数双列；挂钟与 toolUses 统计**只在有效子集上算**（缺值主要落在主线程自派与 verifier 轮——见 §6「planner 主线程自身 token 无遥测」留根）。长尾阈值 = 单次派发挂钟 ≥ 40 min。planner / verifier / architect 行一并列出并标缺口，不再静默取舍。

| actor | 事件 n（全 / dur 有效） | 挂钟合计 | 挂钟中位 | toolUses 中位 | 长尾实例（eventId 前 8 位） |
|---|---|---|---|---|---|
| builder | 23 / 17 | 508.9 min | 17.8 min | 43 | **`9de6ab28` schedule-reminder build 109.4min / 226 tools（全数据集最大）**；`6121e6de` schedule-reminder verify 70.7min / 50；`20241a2c` story-p3 implementation 69.3min / 109；`e4b70402` gateway-directionB token-plan-probe 42.2min / 97 |
| acceptance-author | 23 / 18 | 285.9 min | 8.4 min | 18.5 | `cc79b2fb` session-logic acceptance 61.2min / 84 tools / 108,585 tok；`1618c89d` story-p3 acceptance-freeze 52.0min / 113 |
| integrator | 10 / 8 | 140.1 min | 10.3 min | 42 | `f7ea4133` schedule-reminder integrate 71.9min / 248 tools（**v0.1 该行「长尾 —」在全量下为假**） |
| fixer | 6 / 3 | 16.3 min | 5.4 min | 24 | — |
| planner | 22 / 2 | 缺口为主 | — | — | 主线程自派基本无 durationMs（§6 留根项） |
| verifier | 4 / 0 | 全无值 | — | — | 同上 |
| architect | 1 / 1 | 9.8 min | 9.8 min | 15 | — |
| architect-reviewer | 2 / 1 | 6.0 min | 6.0 min | 21 | — |

**v0.1 → v0.2 修订注（留痕，供 R2 与 landing-verifier 对账）**：v0.1 表载 72 事件 / 12 包 / 40 exact，与全量差 **19 条事件 / 1 包 / 7 exact**。差额可**确定归因**（非猜测）：分析会话跑在 `260829-schedule-reminder` 遥测随该 pod merge 进本分支（`e8b73c0`，2026-09-08T16:53）之前，而 PROPOSAL 于 17:27 提交且 commit message 自述「前会话遗留入版本控制」——时点可解释，但 v0.1 既未 pin as-of 也未声明排除，故按文中命令不可复现（R1/F1 BLOCKING）。刷新后 P1 论据**增强而非削弱**：builder 最大长尾由 69.3min 升至 109.4min / 226 tools，integrator 亦现出 71.9min / 248 tools——「开放输入面自扫」在三条 actor 线上同型，不止子 agent 两条；§5 V6 基线随之全表更新（见该条）。

长尾派发的过程体感与「自扫输入面」直接对应：`260829-story-p3/pod-retro.md:11`（verbatim 摘句）「它烧了将近 19 分钟、几十次工具调用，**把 spec/契约/先例读了个遍**，然后回一句『Now I have all the inputs…』就停了——磁盘零落件」（对应 friction 事件 `0ebaf461`，stall 型，实测 payload：actor=planner / frictionType=stall / 「acceptance-author 18.7min / 49tools 读完输入后在动笔前终止，磁盘零落件」）。

**复现命令**（原样可跑；上表由本命令输出手工转录，min 取一位小数、中位取原值）：

```bash
PYTHONIOENCODING=utf-8 python - <<'PY'
import json,glob,statistics,collections
ev={}
for p in glob.glob('.claude/taskpackages/*/pod-events.jsonl')+glob.glob('changes/dev/*/pod-events.jsonl'):
    for line in open(p,encoding='utf-8'):
        line=line.strip()
        if not line: continue
        d=json.loads(line); ev[d['eventId']]=d
dc=[d for d in ev.values() if d.get('eventType')=='dispatch-completed']
print(len(dc), len({d['changeId'] for d in dc}),
      sum(1 for d in dc if d['payload'].get('tokenUsage',{}).get('status')=='exact'))
by=collections.defaultdict(list)
[by[d.get('actor','?')].append(d) for d in dc]
for a,ds in sorted(by.items(), key=lambda kv:-sum(x['payload'].get('durationMs',0) for x in kv[1])):
    dv=[d for d in ds if isinstance(d['payload'].get('durationMs'),(int,float))]
    mins=[d['payload']['durationMs']/60000 for d in dv]
    tu=[d['payload'].get('toolUses',0) for d in dv]
    print(a, len(ds), len(dv), round(sum(mins),1),
          round(statistics.median(mins),1) if mins else '-', statistics.median(tu) if tu else '-',
          [(d['eventId'][:8], d['changeId'], round(d['payload']['durationMs']/60000,1),
            d['payload'].get('toolUses')) for d in dv if d['payload']['durationMs']/60000>=40])
PY
```

**根因**：refs 锚点已存在但无机械展开步骤；「相关 X」的开放措辞把输入面发现成本压给每次派发的子 agent 会话。

### P2 · 跨端对端源码不在输入面，机械字段推不出跨端性

动机实证：`changes/dev/260907-g-relay-fix/pod-retro.md:23`（verbatim）「跨端协议验收：acceptance-author 输入清单固定加『对端解析器源码路径』」——该包 builder 按冻结契约自创 `display_relay` 帧类型，离线全绿、真机被固件 `Unknown message type` 拒（同文件 `:5` 段），冒烟炸后靠翻 `application.cc`/`mcp_server.cc` 才补上课。

全量普查（2026-09-09 重跑刷新，python 对 **19 个包**的 subsystem/sourceRefs/contractRefs/docRefs/acceptanceTestsFile 判定；**口径 = 排除 `.claude/taskpackages/current/` 指针目录**——raw glob `*/taskpackage.json` 实命中 20 个文件 = 19 包 + current 副本，v0.1 未注明此口径，R1/F2）：
- subsystem 含 firmware+server 双面的包 **8/19**（v0.1 计 9/19 系将 current/ 副本〔内容 = `260829-schedule-reminder`，本身即双面〕重复计入；实测名单八个：char-stroke / hardware-validation / schedule-p2 / schedule-reminder / smoke-e2e / story-p3 / voice-roundtrip / device-ws-switch）；
- **动机包 g-relay-fix 自身 `subsystem=server`、`rigor="lite"`，全字段递归扫描含 "firmware" 的字符串结果为空（零命中）**——纯路径 / subsystem 谓词抓不住它；
- 19/19 包 refs 均引 `docs/contracts/interface-contract.md`——「引契约」无判别力，不能当跨端谓词；
- **（v0.2 补、v0.3 按 R2-F1 修正谓词：(a) 相对 (b) 的净增覆盖，定量辩护 OQ3 裁断）按 T2 advisory 的同一谓词**（锚点**绑定** `docs/contracts/interface-contract.md` ∧ 节 ∈ **T2 常量集合 {§1.5, §三, §四}**）实测：**引 wire 节的包 = 5/19**——schedule-reminder（§三·双面）、voice-roundtrip（§四·双面）、device-ws-switch（§四·双面）、**gateway-directionB（§四·`subsystem=server` 非双面）**、**g-relay-fix（§1.5+§三·`subsystem=server` 非双面）**；subsystem 已标双面的 3 个是推导式谓词 (b) 唯一能抓到的，**另 2 个只有申报式 (a) 抓得到**。即 (a) 的净增覆盖 = **2/19 包**，对价 = 每包起包多答一行（含一行豁免）。**成员修正后恰与 v0.1 OQ3(b) 注亲口点名的两包（g-relay-fix / gateway-directionB）吻合——(a) 裁断不动摇、论据比 v0.2 版更强。**
  **v0.2 → v0.3 修订归因（R2-F1，主线程自查为实测误计）**：v0.2 首版写「4/19，成员 = schedule-p2 / schedule-reminder / simulator-lvgl-align / g-relay-fix」不可复现，两处成因：① **普查谓词未绑文件名**——`simulator-lvgl-align` 的 `§1.5` 锚实指 `docs/notes/feature-inventory.md`（其对契约的 ref 是零锚点裸路径），被宽匹配误纳；② **节集合与 T2 常量分叉**——误纳 schedule-p2 唯一契约锚 `§二`（不在 T2 集合内）、又漏掉 §四 的三个真引者。**教训即本批自己主张的单源纪律**：防御性数字与上线 advisory 谓词必须同出一个常量，否则两套谓词必然分叉——已在 T2 advisory 处补单源声明。

**根因**：跨端性是设计判断不是可推导属性；既有 schema 无申报字段，闸无承载，教训只活在 retro 散文里（= 五条清单头注 `:291` 说的「期望只活在闸的判定代码里、起草侧零成文」的镜像形态：这次连闸都没有）。

### P3 · 超读/探索行为无记账面、无仪表盘

friction 枚举 canon（`dev-loop-operating-system.md:122`，verbatim）「`{frictionType: stall|scope-drift|scan-miss|degraded, summary}`」——无 over-scan 形态；`pod-events-aggregate.mjs` 现输出仅 tokens/coverage/escalation/friction 四块（实跑 `node .claude/scripts/pod-events-aggregate.mjs --change-id 260829-story-p3` 确认，2026-09-09，eventsTotal=17），**无 per-actor 挂钟/toolUses 分布列**——本次专项分析只能手写一次性 python（即 §1 P1 末「复现命令」那段），cadence 复盘（/loop-retro）无现成消费面。**（v0.2 补口径注，R1/F1 同主题）不可裸跑取默认**：`current/` 指针现已改指 `260829-schedule-reminder`（v0.1 起草时为 story-p3），裸跑输出 eventsTotal=33——as-of 漂移会让「默认包」断言失真，故本批一切聚合断言一律显式带 `--change-id`（同步钉进 §5 V4）。

**根因**：观测面缺位 → 「哪些派发在超读」不可见 → 输入面治理没有反馈回路。

### 先例论证（对偶迁移，已读先例原文确认形态）

- **arch 侧 charter 机制**：`scenario-author.md:31`（verbatim）「建**双向消费方清单**:把 spec 里每个 expose/喂给/只读投影/事件 producer 列出,标『匹配的消费方声明在哪』—— 留空处就是 challenger 要打的断链点」；`:64`「单列才暴露留空的断链点(毛利缺口教训)」；`:74` charter 产物含「双向消费方清单(含留空点)」。本批 input-brief = 该冻结阅读面机制在 dev-loop 侧的对偶（acceptance-author/builder 消费，planner/脚本产出）。
- **五条清单同型**：`docs/DEV-GOVERNANCE.md:289-297`「起包格式期望前置」五条（mutationEvidence 骨架 / 断言格式 / 验收 ID / 降级声明前置 / 闸判据自查），四方对象恰为本批同一组角色；本批第六条与之同质（起包期把派发期期望前置成文 + 字段 + 闸）。第 1 条「空骨架也行，字段必须在场」与第 4 条「降级声明（不适用 + 理由）在 Draft 阶段写字段」是 T3 闸形态的直接模板。

## 2. 改法（T1…T7）

### T1 · DEV-GOVERNANCE 五条清单扩六：「输入面期望前置」

改 `docs/DEV-GOVERNANCE.md`「## 起包格式期望前置」节：头注 `:291` 末句「五条前置清单：」→「六条前置清单：」（承重 landing 指令，原文引用钉死，替换后完整句 = 「…跨 4 包 5 子项实证）。六条前置清单：」）；第 5 条后追加第 6 条，要素：
1. planner 派 acceptance-author / builder 时附 **input-brief**（`brief-inputs.mjs` 从 TaskPackage refs 派生的封闭输入清单：锚点节定位 + 跨端对端面 + 先例封顶 3 个）；
2. 子 agent 清单外阅读允许，但交付报告必须列「超读文件 + 原因」段——激励措辞钉死：**超读是常态动作，不申报才是问题**（防止为过闸藏超读、申报数据失效）；
3. `crossEnd` 申报字段 Draft 必在（空骨架也行）+ finalize 时 counterparts 非空 **或** exempt+理由 二选一（与第 1/4 条同型）。

对应 §1 P1/P2 根因：把「输入面期望」从散文教训升为条文 + 字段 + 闸。**（v0.2 记 OQ3 裁断，human 2026-09-09）取 (a)，且明令连批**：T1 第六条 + T4 的「四 agent def 派发指令与 Inputs 封闭化」是本条成立的承重前提——闸的调用相位在 scoped verify（Step 10 起），若不同批把期望前置到 Step 5/7 成文并让 planner/portfolio 预填，则新包会靠闸红反推格式，正复刻被本清单头注 `:291` 点名要消灭的老病灶。三格（Step 5 骨架 → Step 8 二选一 → Step 10 兜底红）缺一格即退化为纯台账。

目标文件：`docs/DEV-GOVERNANCE.md`

### T2 · 新脚本 `brief-inputs.mjs`（派生清单，不新造事实源）

`node .claude/scripts/brief-inputs.mjs --change-id <id> --role <acceptance-author|builder> [--dry-run] [--self-check]`，读 `.claude/taskpackages/<id>/taskpackage.json`，输出 `.claude/taskpackages/<id>/input-brief-<role>.md`（`--dry-run` 输出 stdout 不落盘，供历史包 fixture 验证）。**生成时点 = planner Step 7 派发前一次产双份（见 T3.3/T4，R1/F3）；派生只消费 Draft 即齐的字段（contractRefs/docRefs/sourceRefs/crossEnd + 他包先例），不依赖任何 finalize-only 字段**。四路展开：

- **① 契约/spec 锚点**：`contractRefs`/`docRefs`/`sourceRefs` 的 `#锚点` → 定位所属标题节，列「路径 + 锚点 + 节标题」；**不抽正文**（避免契约影子副本；digest 属 OQ2 phase-2，human 已裁 (a) 不预授权）。**（v0.2 补，R1/F12）无锚点 refs**（实测 g-relay-fix `contractRefs` 即裸路径）在 brief 内标「无锚点（整文件）」并附 stderr advisory 提示 planner 补锚——封闭清单对裸路径会静默退化成整文件引用，须显式呈现该退化而非隐藏它；
- **② 跨端对端面**：`crossEnd.counterparts[]` 的 `{path, symbol}` → 生成时点 grep 解析 symbol 行号区间 + 写生成时间戳。**本批只承载自有实现面消费方（`firmware/**`、`server/**`——动机包该读的对端实测在 `firmware/main/application.cc`、`firmware/main/mcp_server.cc`，正属此档）；②③档（版本控制外只读参考树 / OpenClaw 与物理声显这类无源码黑盒）经 human 2026-09-09 裁为留根，见 §6。** 两条硬规约：**(a) 解析失败 fail-loud**——path 不存在或 symbol 零命中 → 脚本非零退出并点名条目，绝不静默产出空锚（静默空锚会让 `crossEnd` 的牙全泄；来源标注见 §8-B1）；**(b) 快照语义（v0.2 按 R1/F11 改措辞，原「行号不落库，防腐化」过强）**——行号只落 brief（带时间戳、按派发再生、权威窗口 = 本次派发消费期的短命快照），不落 taskpackage.json；brief 本身 Git-tracked 同样会漂，此处防的是「把旧行号当长期事实源」，消费时若 symbol 已移位则重新 grep（此语义在 T6 注册文里同文重申，免读者只看到一半）；
- **③ 先例封顶**：同 subsystem 已 merged 包的 `acceptanceTestsFile`，按时间倒序封顶 3 个（治 P1「把先例读了个遍」）。**（v0.2 按 R1/F8 钉死三判定源，脚本不得自由发挥）**merged 判定源 = `changes/dev/<id>/supervisor-verdict.json` 的 verdict=APPROVE ∨ 该包 pod-events 存在 merge 类事件——**明确排除 `$(git-common-dir)/devloop/pods/<id>.json` board，因 board 不入版本控制、新 worktree 不可依赖**；subsystem 匹配 = **token 化包含**（`firmware` 包与 `firmware+server` 包互为先例；字符串相等判据弃用，否则本仓 5 种 subsystem 取值实际只成 5 个互不相干的先例池）；时间轴 = **changeId 的 YYMMDD 日期前缀**（不用文件 mtime / merge 时刻——跨 worktree 与跨 checkout 不稳定）。三源均须写进脚本注释；
- **④ 超读申报段模板**：交付报告的「清单外阅读 + 原因」空表。

advisory（非门）：包 refs 引 wire 节 ∧ `crossEnd.counterparts` 空 ∧ 无 exempt → stderr WARN 提示 planner 复核申报。**（v0.2 按 R1/F5 封闭谓词）wire 节集合钉为脚本顶部显式常量 + 注释钉维护义务**（「判定源 = `docs/contracts/interface-contract.md` 节号；契约经契约门改节号时须同批同步本常量」——无维护锚的硬编码节号 = 契约改号后 WARN 静默失效的 vacuous advisory）；**集合按实测节结构扩**：§1.5「网关的职责」+ §三「提醒推送与出队语义」+ **§四「端云协议剖面（小智协议消息子集）」——v0.1 集合由动机包 refs 反推、漏了这个真正的 wire 形状核心节（mcp kidpal/display 载荷与 E1 JSON 承载均在此定义）**，另在实现期评估 **§1.2/§1.3（display 顶层结构 / view 类型）与 §二（schedule-p2 的唯一契约锚，实测落 T2 集合外）**是否入集合。**（v0.3 按 R2-F1 补单源声明）本常量集合即 §1 P2「净增覆盖 5/19、非双面 2/19」census 的唯一谓词源**：census 脚本与 advisory 判定必须共用同一常量（锚点还须绑定 `interface-contract.md` 文件名，不得只匹配节号文本）——否则防御性数字与上线谓词分叉，正是本批自警的双源漂移（v0.2 首版已实际踩中，成因见 §1 P2 末修订归因）。WARN 宿主 = planner（**Step 7 前置** Bash 现跑 brief-inputs 时 stderr 直进其会话输出，R1/F 节已核有主；**v0.3 一致性校正**：v0.2 此句作「Step 7/9」系 F3 改点后的残留，builder brief 现只在 Step 7 生成、Step 9 只引用，故 WARN 唯一宿主时点 = Step 7）。`--self-check` 用内置 fixture 断言四路输出形状（真空绿规约：fixture 非空断言先行）。对应 P1/P2。

目标文件：`.claude/scripts/brief-inputs.mjs`（新增）

### T3 · `taskpackage-required-check.mjs` 扩三条判定（扩承重闸，不新立）

1. `crossEnd` 字段在场性：对象 `{counterparts: [], exempt?: string}`，Draft 空骨架放行（同 mutationEvidence 条，模板 = `DEV-GOVERNANCE.md:293`）；
2. finalize 态（`acceptanceTestsFile` 非空——**该代理系既有约定**：本闸 `:167` 本就无条件要求它非空、planner Step 8 `:55` 同义；R1/E 节确认自洽，即 Draft 态闸恒红、绿态仅 finalize 可达，故本条判定恒有定义。本批新造的只是把它用作闸内**分支条件**）：`counterparts` 非空（每条须含 path+symbol）**或** `exempt` 非空理由，二选一，双缺 = RED 点名。**`exempt` 设长度地板 ≥ 12 字符，对齐本闸既有同型判据 `:292`（verbatim：`landsIn=dropped requires reason >= 12 chars`），防空破折号混关**（来源标注见 §8-B2）；
3. finalize 态：`input-brief-acceptance-author.md` 与 `input-brief-builder.md` 在 taskpackage 目录存在。**（v0.2 按 R1/F3 改写时序，原括注只论证了 acceptance-author 一侧）**双 brief 的唯一定义生成点 = planner **Step 7 派发前一并产出**（见 T4 第一条），故 Step 8 finalize 时两份必在场、Step 9 只引用；闸侧调用点枚举补证无鸡生蛋——scoped verify（Step 10 起）与 closeout（Step 11，`closeout-check.mjs:309-315` 程序化传 canonical 路径）均晚于 Step 7。

**追溯口径与过渡（v0.2 按 R1/F4 补齐，riskTier 行同步）**：对**已 merge 历史包**零追溯（闸只读 current taskpackage，与 CHANGE_ID cutoff 同约定）；对 **landing 时在飞包**有一次性过渡摩擦——已过 Step 7/9 而无 brief、无 `crossEnd` 的 current 包，下一轮 scoped verify 会撞 T3.2/T3.3 红。当前实测无在飞 pod（`current/` 指向已 merge 的 schedule-reminder），风险窗口 = 本批 FROZEN 至 landing 之间新起的 pod。过渡补救 = planner 对该包补跑 `brief-inputs.mjs`（脚本不依赖相位，随时可生）+ 补填一次 `crossEnd`，即转绿；**不设过渡豁免字段**（为窗口期造第二条口径的代价大于一次补跑）。

单源红利：closeout 相位程序化调用本脚本（`dev-loop.md:229`，verbatim 摘句「承重字段/decisionCoverage 校验由 closeout 相位程序化调用 `taskpackage-required-check` 单源承重（F-04）」），扩展自动传导，`verify-gates.json` 与 `closeout-check.mjs` 零改动（R1/E 节亲核 `verify-gates.json:32-34` 三行条目 + `closeout-check.mjs:309` 路径解析成立）。对应 P2。

目标文件：`.claude/scripts/taskpackage-required-check.mjs`

### T4 · 四 agent def 修订（派发指令 + Inputs 封闭化 + 引用行同步）

- **planner.md**：**Step 7（`:54` 派 acceptance-author）前置「一次产双 brief」**——先跑 `brief-inputs.mjs --role acceptance-author` **与** `--role builder` 两份（refs 在 Draft 即齐，见 T2 头/T3.3；**R1/F3：builder brief 不得留到 Step 9 才生，否则与本条 Step 8 硬前置自锁**），派发 prompt 引用 brief 文件路径 + 超读申报纪律一句，**不转抄清单正文**（防双源漂移）；Step 9（`:56`）builder 派发**只引用已在场的 `input-brief-builder.md` 路径**（不再生成；若 Step 8 之后 refs 有改动则重跑覆盖——**（v0.3 按 R2-F6 指定落点）重跑记录不落散文、落制品自身**：`brief-inputs.mjs` 每次生成刷新 brief 头部 `generated-at` 时间戳，覆盖再生时追加一行 `regenerated-because: <refs 改动一句>`，brief 本身 Git-tracked（T6 注册），`git diff` 即可审；planner 不另写 journal / prose 行，避免第二事实源）；Step 8 finalize（`:55`）硬前置加「两份 input-brief 在场 + `crossEnd` 二选一」；Step 1 intake 加跨端判断（设计判断字段：本包是否断言/改变跨端 wire 形状？判断不了按存疑向严填 counterparts 而非 exempt；**本批 counterpart 语义限自有实现面消费方，见 T2②**）；`:109` 引用行「五条前置清单」→「六条前置清单」。
- **portfolio-planner.md**：拆包草案预填 `crossEnd` 骨架（`:58` 引用行同步「五条」→「六条」+ crossEnd 预填点名——与 mutationEvidence 骨架教训同型：起草侧不带，闸红回派）。**（v0.3 按 R2-F2 补，闭 V5④）`:58` 第六条引用须同时点名 `input-brief`**，一句最小形：「`crossEnd` 骨架与 `input-brief` 同源——预填的 counterparts 即 planner Step 7 生成 brief 第②路的输入」（它是 crossEnd 预填的下游消费方，不点名则该 def 全文无 `input-brief` 字样，§5 V5④「四 def 各含 input-brief 引用」只能靠执行者自由发挥补齐 = landing-verifier 对账噪声源）。
- **acceptance-author.md**：`:24` Inputs「相关 `docs/contracts/**` 与系统 spec。」→「`input-brief-acceptance-author.md`（封闭输入面，脚本从 TaskPackage refs 派生）；brief 缺失时降级自扫并在交付标 `manifest-missing`。」；`:29` Step 1「读取 TaskPackage、contracts、系统 spec」同步改指 brief；Step 7 交付输出加超读申报段；**`:46` 引用行按 v0.2（R1/F7 BLOCKING）改写而非保持**：现文「（起包清单五条中的②③）」→「（起包清单六条中的②③）」，即 **②③ 选集语义保持、计数词五→六**，另追加第六条（输入面期望前置）引用。**理由：v0.1 写「句保持」＝保留「五条中的」字样，而 §5 V5 的负 grep 要求 `.claude/agents/` 对该 pattern 零命中——两硬项字面不可同时满足，落地者按 T4 字面落即 V5 红、按 V5 改即偏离冻结文本。冻结文本必须钉到无第二读法。**
- **builder.md**：`:24` Inputs「相关 contracts、events、shared types、system spec。」同型改法（`input-brief-builder.md`）；`:46` 引用行同步——实测该句现**不含**「五条」字样，故本条是「追加 input-brief / 第六条引用」而非计数词替换，由 V5「四 def 各含 `input-brief` 引用」兜此面。

对应 P1（封闭输入面）+ P2（intake 判断点）+ 五条→六条投影一致性。**landing 纪律（R1/D-2 注记）**：本条只动四件 dev-loop agent def 的**正文**（Inputs / Process / References）；实施中若发现需动 frontmatter 任一字段（`tools` / `name` / `model` / write scope），即触自改递归高敏面，**须停下另行升级 human，不得顺手改**。

目标文件：`.claude/agents/planner.md`、`.claude/agents/portfolio-planner.md`、`.claude/agents/acceptance-author.md`、`.claude/agents/builder.md`

### T5 · `task-intake-and-slicing.md` 字段级定义：crossEnd 入 Lite 不可省清单

`:211` 节（rigor 与 Lite TaskPackage 字段级定义）加 `crossEnd` 字段定义（形状 / 二选一语义 / **`exempt` 理由长度地板 ≥12 字符（对齐闸 `:292` 同型判据，见 T3.2 与 §8-B2）——起草侧必须在此看到地板，否则短 exempt 撞闸红再反推，正是本清单第六条要消灭的形态在第六条自身上的复现（R2-F5）** / advisory WARN 行为 / **本批覆盖档 = 自有实现面消费方，②③档见 §6 留根**），并把 `crossEnd` 骨架列入 Lite 不可省清单（`:217` 的「不可省」条目行）——实证依据：**动机包 g-relay-fix 本身是 Lite 包**，其 taskpackage 有两重机械佐证：`"rigor": "lite"` **字段值本身**（v0.2 按 R1/C 节建议改以字段为主证，比 prose 更强、可机判）+ notes verbatim「本包以新建 Lite 包承载并如实标注」。Lite 豁免即放过动机案例。对应 P2。

目标文件：`.claude/references/task-intake-and-slicing.md`

### T6 · `dev-loop.md` 文件地图注册 input-brief

§2 文件地图（`:72`）与 taskpackages 标准内容节（`:125` 段）注册 `input-brief-<role>.md`：脚本生成派生物、Git-tracked（taskpackages 目录本就 tracked）、证据属性（超读申报与 WARN 复核的锚点）。**（v0.2 按 R1/F11 补）注册文须同文写明快照语义**：brief 内行号 = 生成时点解析结果、带时间戳、按派发再生，**非长期事实源**（消费时 symbol 已移则重跑 grep），并写明生成时点 = Step 7 派发前一次产双份（与 T3.3/T4 一致，防读者只看到文件地图不知何时生）。对应 P3 的制品面。

目标文件：`.claude/references/dev-loop.md`

### T7 · 观测面：friction over-scan 型（OQ1 已裁 (a)）+ 聚合仪表盘列

- `dev-loop-operating-system.md:122` friction 枚举扩 **`over-scan`** 新型（human 2026-09-09 裁 OQ1=(a)）；维持 `:122` 既有定性「纯 advisory、绝不当门」（DP-D4）——over-scan 只记账不拦截。**（v0.2 按 R1/F14 补生产者，缺则成死 canon）触发定义钉进同一条目**：子 agent 交付报告的超读申报段**非空** → planner 落一条 over-scan friction（`frictionType: over-scan`，summary 引申报行数与代表文件，如「builder 超读 6 件：docs/notes/x.md 等」），与 T1 第 2 条激励措辞（「超读是常态动作，不申报才是问题」）连成一条闭环：申报 → 记账 → 仪表盘可见；对照 `:122` 现文 stall 型已有启发式触发注记，本条同型。
- `pod-events-aggregate.mjs` 加 per-actor `durationMs`/`toolUses` 中位 + 长尾（**≥40min**，与 §1 P1 阈值同源、不得两值）计数列，`/loop-retro` cadence 直接消费；pilot 2–3 包后凭该仪表盘回看长尾收敛（阈值从数据定，本批不预设「≤60 tools」类拍数目标）。

对应 P3，并给 OQ2（digest phase-2，已裁不预授权）提供决策数据面。

目标文件：`.claude/references/dev-loop-operating-system.md`、`.claude/scripts/pod-events-aggregate.mjs`

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 治理成文 | `docs/DEV-GOVERNANCE.md` | **G1 成文**（五条→六条 + 头注计数）——归类经 R1 判官裁断确认不改判（§6 表 `:212` 字面列名；batch-2「标 G2 且 G1 为零」先例与 SSOT 表矛盾、不予沿用） | 四 agent def 引用行（T4 同批同步）；grep「五条」全仓消费面：v0.1 现跑（2026-09-08）+ **判官 R1 独立复跑（2026-09-09，pattern `五条前置清单\|五条中的`，排除参考树）双证 canonical 恰四处**——`planner.md:109`、`portfolio-planner.md:58`、`acceptance-author.md:46`、`DEV-GOVERNANCE.md:291`，全部入 T1/T4 改动集、无漏网；宽 pattern「五条」的其余命中（`arch-review-protocol.md`「五条协议要点」、`pod-coordination.md:130`「消解纪律五条」、`DEV-GOVERNANCE.md:22`「五条设计思想」、`changes/dev/**` 历史证据）均系**异形同词或 append-only 历史件，不改亦不应改**；`builder.md:46` 引用同节但不含「五条」字样，由 V5「四 def 各含 input-brief 引用」兜 |
| 闸脚本 | `.claude/scripts/taskpackage-required-check.mjs` | G3 扩判定（向严） | `verify-gates.json:32-34` taskpackage-required 闸（配置零改动）；`closeout-check.mjs` F-04 单源调用（`dev-loop.md:229`）自动传导 |
| 新脚本 | `.claude/scripts/brief-inputs.mjs` | G3 新增（只读 taskpackage + 写 taskpackages 目录内 brief，无 canonical 写面） | planner Step 7 前置一次产双份（R1/F3 后语义）、Step 9 只引用；`--dry-run`/`--self-check` 供验证集 |
| 聚合脚本 | `.claude/scripts/pod-events-aggregate.mjs` | G3 加列（向后兼容，新列只增不改既有输出键） | `/loop-retro` cadence；本 PROPOSAL §5 V4 |
| agent def | planner / portfolio-planner / acceptance-author / builder 四件 | G2 行为修订 | 派发/交付合同变化；arch-review 侧角色零触碰；**限正文（Inputs/Process/References），frontmatter 零动——需动即触自改递归须停下升级（R1/D-2 landing 纪律注记）** |
| references | task-intake-and-slicing.md / dev-loop.md / dev-loop-operating-system.md | G2 成文 | 字段定义 / 文件地图 / friction 枚举的三个 SSOT 投影 |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| scope-map / settings / verify-gates.json | — | **未触** | 新脚本落点 `.claude/scripts/` 为既有平台面，无新写槽需求（R1/H 节实测 scope-map planner allow 已含 `.claude/taskpackages/**`，brief 写面在既有槽内） |
| taskpackage schema | `crossEnd` 新字段 | G2/G3 边界（字段定义在 references，判定在闸） | **已 merge 的 19 包零追溯**（闸只读 current）；**landing 时在飞包一次性过渡摩擦 + 补救见 T3「追溯口径与过渡」**（R1/F4） |

**【硬项 1b · sweep 透镜义务】** 新门判定 pattern 全仓预跑已完成：cross-end 谓词对 19 个存量 taskpackage 的普查（命令与输出摘要见 §1 P2，**2026-09-09 按 R1/F1、F2 重跑刷新，v0.3 再按 R2-F1 修正谓词**：口径排除 `current/` 指针副本、按 changeId 去重，逐包打印 subsystem + 双路径判定 + 契约锚点判定，无截断省略形态；**双面 8/19、wire 节引用 5/19（锚点绑定 `interface-contract.md` ∧ 节 ∈ T2 常量 {§1.5,§三,§四}）、其中非双面 2/19 = g-relay-fix + gateway-directionB**）；结果直接塑造了 T3 闸形态（纯机械谓词抓不住动机包 → 申报字段 + 二选一，而非硬推导；OQ3 已由 human 按此裁 (a)）。**单源义务（R2-F1 教训）**：本节 census 与 T2 advisory 共用同一常量与同一「锚点绑文件名」谓词，脚本实现期须引同一常量、不得各写一套。机械枚举 census：「五条」消费面 grep 命令与四处命中已列本表首行（判官 R1 独立复跑同 pattern 双证一致）。

**【硬项 1c · 复扫模式完备性先验】** 不适用：本批无退役语面/残留清扫（「五条」→「六条」为计数词更新，消费面已全量枚举于上表，非模式复扫）。

## 4. human 取舍点（OQ）

> **本节三 OQ 已由 human 2026-09-09 逐项裁断（R1「FROZEN 前置确认项」闭合）**：OQ1=**(a)**、OQ2=**(a)**、OQ3=**(a) 且连同 T1 第六条 + T4 输入面改写连批**。裁断去向：OQ1→T7 落 `over-scan` 型并补生产者触发（R1/F14）；OQ2→T2① 维持只出指针不抽正文、digest 递延凭数据再提案；OQ3→T1/T3/T5 现文即按 (a) 落地，无重写。另两问非 OQ 但同场裁：**F1 修法 = 全量重跑刷新**（→ §1 P1 已刷新 + V6 基线联动）；**消费方②③档 = 本批不扩、留 ARQ**（→ §6）。

- **OQ1 · over-scan friction 型归属 → 裁 (a)**：(a) `:122` 枚举扩 `over-scan` 新型（**推荐**——与 `scan-miss`（该扫没扫）语义相反（不该扫扫了），独立型才可分向统计；枚举是关键词形态、扩型零迁移成本）；(b) 复用 `scan-miss` 靠 summary 措辞区分（省一个枚举值，但聚合统计混向）；(c) 不入 friction，只留交付报告段（零 canon 改动，但 cadence 复盘无机器可聚合信号）。
- **OQ2 · phase-2 digest 是否预授权 → 裁 (a)**：(a) **不预授权**（推荐）——phase-1 封闭清单落地后，凭 T7 仪表盘量 2–3 包，若整文件通读仍占长尾大头再提案；digest = 契约节选副本，与影子目录禁令精神相抵，须凭数据立项；(b) 本批预授权（全节抽取 + sha256 钉源 + 非权威标注），省一轮提案但先造副本面后验需求。
- **OQ3 · crossEnd 闸形态 → 裁 (a) 并连批**：(a) **骨架必在 + 非空或豁免二选一**（推荐——§1 P2 普查实证硬谓词抓不住动机包；豁免理由留痕可被 integrator/loop-retro 复核，形态照抄五条清单第 1/4 条既成模式）；(b) 仅 subsystem 含双面时硬红（会漏 g-relay-fix/gateway-directionB 类单面标签跨端包，等于为可见的一半设闸；**v0.3 定量（谓词经 R2-F1 修正为 T2 同源）：wire 节引用 5/19，(b) 只能抓到其中双面 3 个，(a) 净增覆盖 2/19 = g-relay-fix（真炸过）+ gateway-directionB（引擎方向 B 定向包，refs 引 §四端云协议剖面——`subsystem=server` 单面标签、业务实跨端）——v0.1 此处凭直觉点名的两包恰即修正后的实测成员，(a) 论据较 v0.2 更强**）；(c) 纯 convention 不设闸（回到「期望零成文零承载」老路，与本批立项理由自相矛盾）。**连批理由（判官 I 节同向）**：(a) 单独落地不成立——闸在 scoped verify 相位，不前置到 Step 5/7 成文 + 起草侧预填，就靠闸红反推，正是 DEV-GOVERNANCE `:291` 要消灭的形态。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · 脚本自检**：`node .claude/scripts/brief-inputs.mjs --self-check` → exit 0，stdout 含四路形状断言名（锚点节/对端面/先例封顶/申报段）；重放真空绿规约：self-check 先断言内置 fixture 非空。**（v0.2 按 R1/F11、F12、§8-B1 扩三条子断言）**① 对端条目 path 不存在 / symbol 零命中 → 非零退出且 stderr 点名条目（fail-loud 路径必测，防「空锚静默」）；② 裸路径 refs（无 `#锚点`）→ brief 出现「无锚点（整文件）」标记 + advisory；③ 先例封顶三判定源各造一样例（verdict=APPROVE 命中、subsystem token 包含命中、changeId 日期前缀排序命中）。
- **V2 · 历史包 dry-run fixture**：`node .claude/scripts/brief-inputs.mjs --change-id 260907-g-relay-fix --role acceptance-author --dry-run` → stdout 含 `interface-contract.md` 锚点节行 + 超读申报段模板；随后 `git status --short .claude/taskpackages/260907-g-relay-fix/` → 空输出（dry-run 不落盘断言）。前提亲核 ✓（R1/G 节：该包 refs 实含 `#§1.5`/`#§三` 两锚，目录 git 现清洁，断言有料且不被污染）。
- **V3 · 闸负正两态**：fixture taskpackage（refs 引 wire 节、缺 `crossEnd`）→ `node .claude/scripts/taskpackage-required-check.mjs <fixture路径>` exit 1 且 message 点名 `crossEnd`；补 `{counterparts:[{path:"firmware/main/mcp_server.cc",symbol:"<实存符号>"}],exempt:""}`（或补 `exempt: "<≥12 字理由>"`）+ 两份 brief fixture 文件 + `acceptanceTestsFile` 非空 → exit 0。先断言 fixture 文件非空（真空绿规约）。**（v0.2 按 R1/F9 补齐规约）**① **落点 = 仓外绝对路径 temp 目录**（闸对显式 argv 走 `resolve(projectDir, explicit)`，绝对路径可用）——**严禁落 `.claude/taskpackages/` 内**，否则污染未来包数 census（本批 R1/F2 的 9↔8 正是 current 副本污染实例）；用后清理义务 = V3 末尾 `rm -rf` 该 temp 目录并 `ls` 断言不存在；② **正态 fixture 须满足闸全部既有必填**，最小字段清单照抄 `planner.md` TaskPackage 格式示例：`changeId`（带合法 YYMMDD- 前缀，避 `:208-230` 前缀红）/ `subsystem` / `story` / `redBaselineCommand` / `redBaselineResult` / `deliveryImpact` / `planningImpact` / `riskTier` / `origin.type∨sourceType` / 七数组（acceptance、acceptanceTestsFile、exitCriteria、contractRefs、docRefs、sourceRefs、gateProfile）/ `iterationBudget` / `fixBudget` / `docsOwner=integrator`——不注明则执行者被 crossEnd 之外的红项绊住误判；③ 两份 brief fixture 落点 = 与 fixture taskpackage **同目录**（T3.3 语义），并在 V3 写明；④ 加测 exempt 长度地板：`exempt: "—"`（< 12 字）→ 仍 RED。
- **V4 · 聚合新列**：`node .claude/scripts/pod-events-aggregate.mjs --change-id 260829-story-p3` → 输出 JSON 含 per-actor 挂钟/toolUses 中位键，且 acceptance-author 行 toolUses 中位与 pod-events 原值一致（**R1/G 节实测锚点：story-p3 acceptance-author 四条派发 = 113/8/18/19 tools，中位 18.5**；113-tools 派发在场使 spot-check 可辨）。**一律显式带 `--change-id`，不得裸跑取默认**（`current/` 指针已漂移，见 §1 P3 口径注）。
- **V5 · 投影 grep（v0.2 按 R1/F6、F7 补齿）**：① `grep -c "六条前置清单" docs/DEV-GOVERNANCE.md` ≥1（头注计数已换）**且** `grep -c "五条前置清单" docs/DEV-GOVERNANCE.md` == 0（不扫 DEV-GOVERNANCE 自身是 v0.1 漏齿）；② **第 6 条内容锚**：`grep -c "input-brief" docs/DEV-GOVERNANCE.md` ≥1 且 `grep -c "crossEnd" docs/DEV-GOVERNANCE.md` ≥1（正 ①只证换了个计数词，证不了第 6 条正文被追加——换头注不追加条目照样绿）；③ `grep -rn "五条前置清单\|五条中的" .claude/agents/` → 零命中（**此项与 T4 acceptance-author `:46` 改写条联动：该句必须按 v0.2 改成「六条中的②③」，R1/F7 已消歧，两硬项不再互斥**）；④ 四 def 各含 `input-brief` 引用（`grep -l input-brief .claude/agents/{planner,portfolio-planner,acceptance-author,builder}.md | wc -l` == 4）；⑤ `dev-loop-operating-system.md` `:122` 枚举含 `over-scan`（OQ1 裁断值）；⑥ `dev-loop.md` 文件地图含 `input-brief`。
- **V6 · pilot 回看（交日落复盘，非 landing 门）**：本批 landing 后前 2–3 个新包，凭 T7 仪表盘对比基线；**基线随 v0.2 F1 全量刷新联动更新**——acceptance-author 长尾 52.0–61.2min（两例不变）、**builder 长尾上限由 69.3min 改 109.4min/226 tools（新最大者 = `9de6ab28` schedule-reminder build）、integrator 长尾由「无」改 71.9min/248 tools（`f7ea4133`）**、story-p3 stall 形态保持。守护指标两条，**计数口径按 R1/F13 钉死**：① **冻结后** acceptance 缺陷再派次数（phase 集合 = 冻结派发**之外**的 harness-repair / repoint / amendment 类 dispatch-completed）——基线 story-p3 = **3 次**（`b65a58bd`/`b45eb47f`/`e1a4aaef`；v0.1 的「4 次」系把冻结派发本身计入，口径已并表）；② fixer 轮数——基线 **13 包共 3 次**（实测两口径均 3，包数随 F1 刷新由 12 改 13）。两者**不得恶化**；恶化即清单切漏必要输入，回补 brief 展开规则而非废机制。

## 6. 留根清单

- **消费方②③档（human 2026-09-09 裁：本批不扩，另登 ARQ）**：`crossEnd.counterparts` 现形状 `{path, symbol}` 只能表达**自有实现面**消费方，两档落不进去——**(b) 只读参考树**（`ESP32-S3-RLCD-4.2-Demo/**` 小智 V2.1.0 源码：可读、可 grep，但在版本控制外、跨 checkout 路径不稳定，行号/符号锚更易腐，需一套「参考树锚 = 相对参考根 + symbol，不落行号」的子语义）；**(c) 无源码黑盒消费方**（OpenClaw 引擎侧——`260907-g-relay-fix` taskpackage notes 即记「live 引擎 Intent:nointent，OpenClaw 拒 tools 数组，WS 包 SHOULD #2」，正是我们自创形状被外部消费者拒的第二实例；以及物理声/显表现）。第 (c) 档要的不是源码路径而是**「哪条 live 断言钉住它」的 live oracle 锚**，其语义（什么算钉住、如何判缺席）本批无数据支撑，硬造即拍数。→ 登记 ARQ：`crossEnd 消费方分档（参考树 / 黑盒 oracle 锚）`，凭 pilot 2–3 包实际遇到再提案。
- **`pre-merge-escalation-gate.mjs` gov GREEN 机锚向宽（R1 范围外观察，主线程认同、不入本批 findings）**：`:83-84` 的 green 机锚 regex 含**裸 `GREEN` 分支**，注释意图是「verdict 同段」但实装未收窄——**任何正文里出现 "GREEN" 字样的 PROPOSAL 都恒满足 gov 档第 6 关机锚**（本批 §7 判官 verdict 文本天然含该词，即自证其害）。属升级闸向宽失真，非本批 manifest 面（本批零触 `pre-merge-escalation-gate.mjs`）→ 另登 ARQ，修法方向：机锚限「verdict 行 / 段」形态匹配（与注释意图对齐），并配 `--self-check` 反例 fixture（正文含 GREEN 但无 verdict → 须判不满足）。
- **dispatch-stamp 遥测补全**（另走 gov-arq，不入本批）：`planner.md:57` MODEL-TIER 条款「dispatch-stamp 若可得须补记实际生效模型」欠账未实现 + token input/output/cache 分项缺失 + provider 槽位映射（260903 期间 opus/sonnet 槽位曾映射 deepseek-v4-flash，两包数据模型标签失真）无字段钉——三项一并登记 ARQ。**v0.2 补实测规模（R1/F1 刷新后）**：91 条派发事件中 44 条无 token 精确值（47/91 = 52%）、builder 23 条中 6 条无 durationMs，planner 自派 22 条几乎全无值——缺口不是边角案例，是本数据集的主形态之一。
- **frictionType `environment` 漂移**：实测数据已出现 `environment` 值（260829-session-logic）但不在 `:122` 枚举——本批不夹带（防 scope creep），留 ARQ 或下批治理批收编。**（与本批 T7 的关系点明）这是「生产者改了形状、消费者（聚合脚本/枚举 canon）没对齐」的治理侧实例，恰是 crossEnd 要治的那类洞在 `.claude/` 面自身的复现**——收编时可直接用 T3 的二选一形态（枚举扩值 ∨ 显式豁免注记）。
- **planner 主线程自身 token 无遥测**：本批仪表盘只覆盖子 agent 派发；主线程消耗（可能是 Pod 最大单项）观测另议。
- **交日落复盘项**：V6 pilot 回看（2–3 包后）；OQ2 digest 立项与否凭该数据。
- **自律件（不进模板）**：planner 派发 prompt 只贴 brief 文件路径 + 纪律一句，不转抄清单正文；brief 生成失败时降级派发须标 `manifest-missing`，不静默。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect verdict：NEEDS-WORK（2026-09-09，评审发起 09-08 跨日收尾）

**结论：打回（NEEDS-WORK）**——BLOCKING ×3 / SHOULD ×7 / COULD ×4；全文（含逐节论证、亲核留痕、coverage）见同目录 `architect-review-r1.md`。方向与根因对应成立、归类与降档均站得住，打回项皆为机械修订，不动批次方向，**无升级 human 项**。

**BLOCKING 逐条**：
- **F1（事实准确性/可复现性，§1 P1 + §5 V6）**：按文中命令独立复现 = **91 事件 / 13 包 / 47 token 精确**，非 72/12/40。对账确定归因：起草数据集静默排除了 `260829-schedule-reminder` 整包（减法后 72/12/40 严丝合缝；该包遥测 09-08T16:53 才 merge 进本分支、PROPOSAL 17:27 提交且自述「前会话遗留」——分析先跑可解释，但文中未 pin as-of、未声明排除），且 actor 表 n 列 = durationMs 有效子集口径未披露（builder 全事件 21 vs 表中 15）。实质后果：被排除包含全数据集**最大**长尾（builder `9de6ab28` 109.4min/226tools、integrator `f7ea4133` 71.9min/248tools → integrator 行「长尾—」在全量下为假），V6 基线被传染。修法：全量重跑刷新 §1 P1 表与 V6 基线（推荐），或 pin as-of commit + 显式声明排除与 n 口径。四条长尾 eventId（`20241a2c`/`cc79b2fb`/`1618c89d`/`0ebaf461`）逐条实测**精确命中** ✓。
- **F3（T4 内部时序矛盾，T4 vs T3）**：Step 8 finalize 硬前置「两份 input-brief 在场」与 builder brief 唯一定义生成点「Step 9 派发前置」冲突——照字面执行的 planner 在 Step 8 死锁；T3 `:108` 括注只论证了 acceptance-author 侧（闸侧经调用点枚举补证无鸡生蛋：scoped verify ≥ Step 10、closeout = Step 11，均晚于 Step 9）。修法：钉死「Step 7 派发前双 brief 一并生成（brief-inputs 只消费 Draft 即齐的 refs + crossEnd，不依赖 finalize-only 字段），Step 8 核在场，Step 9 只引用」。
- **F7（两硬项不可同时满足，T4 acceptance-author 条 vs V5）**：「`:46` 引用行『五条中的②③』句保持」按字面 = 保留「五条中的」字样，而 `:46` 实文含该 pattern、V5 负 grep 要求 `.claude/agents/` 零命中——按 T4 字面落即 V5 红，按 V5 改即偏离冻结 T4 字面。修法：T4 改写为「『②③』选集语义保持、计数词五→六（『起包清单六条中的②③』），另追加第六条引用」。

**SHOULD 逐条**：F2 census 双面实测去重 = **8/19** 非 9/19（9 = current/ 指针副本重复计入；「19 个」raw glob 实为 20 含 current/，口径须注明；g-relay-fix subsystem=server + 全字段零 firmware ✓、19/19 refIC ✓ 均复现）；F4 「存量零追溯」漏 in-flight 包（landing 时在飞 pod 下轮 scoped verify 撞 T3.2/T3.3 红，补救可行但未成文——当下无在飞 pod，窗口 = 至 landing 间新起 pod）；F5 T2 wire 节谓词未封闭（§四「端云协议剖面」才是 wire 核心节未入集合；节号硬编码 G3 脚本随契约改节号静默失效）；F6 V5 判别力不足（正 grep 证不了第 6 条被追加；负 grep 不扫 DEV-GOVERNANCE 自身残留）；F8 T2③ 先例封顶三判定源未钉（merged 判定源 / subsystem 匹配规则 / 时间轴）；F9 V3 fixture 落点与清理义务未指定（落进 .claude/taskpackages/ 即污染 census——F2 正是实例）+ 正态 fixture 须满足闸全部既有必填未注明；F14 OQ1(a) over-scan 枚举值无生产者触发定义（无生产者的枚举值 = 死 canon，仪表盘列可能恒空）。

**COULD 摘要**：F10 §0 引头注行号 `:12` 实为 `:11`（文本 verbatim 准确）；F11 「行号不落库防腐化」措辞过强（行号落进 Git-tracked brief，实际防腐 = 时间戳 + 按派发再生快照语义，应成文）；F12 无锚点 refs（g-relay-fix contractRefs 即裸路径实测）呈现行为未定；F13 V6「story-p3 单包 4 次」口径未钉（实测冻结后再派 = 3：harness-repair/repoint/tighten，含冻结派发才 = 4）。

**归类裁断**：`docs/DEV-GOVERNANCE.md` = **G1 确认，不改判**——§6 表 `:212` 字面列名「入口与指令根（G1）| AGENTS.md、CLAUDE.md、docs/DEV-GOVERNANCE.md」；batch-2 先例「标 G2 并声明 G1 为零」与 SSOT 表矛盾，不予沿用。本批 profile = G1+G2+G3 成立；G1 不改轨道（非 Track A 判据，Track B 通道明文 G1/G2/G3 全落）。

**降档裁断**：**Track B 成立、力度足额**——直接 Track A 两判据零命中（契约面零触碰：manifest 11 文件亲核无 contracts；非跨系统结构性：治理面单侧）；机检硬排除各条均标「Track A 触发时适用」且双轨路由节明文 governance 类不强制升级；四件 dev-loop agent def 非 review-loop/self-def、T4 不动 frontmatter（landing 纪律注记：实施若发现需动 frontmatter 即触自改递归高敏面，须停下另行升级）。riskTier low-medium 复核成立（向严 + 零追溯〔F4 补齐后〕+ manifest-missing 兜底；前置升级闸不解析 PROPOSAL riskTier，字段为描述性）。

**FROZEN 前置确认项（S4 先例）**：OQ1/OQ2/OQ3 正文已按推荐 (a) 编入（T7 依赖 OQ1 裁断值、T2① 按 OQ2(a)、T3/T5 按 OQ3(a)）——FROZEN 前须 human 显式裁断落记录；任一 OQ 改选非 (a) 则对应 T 项重写后再送审。

**结构字段核对** ✓：landing-manifest 围栏块在场（11 行 = T1–T7 目标文件精确闭合，无孤儿无缺项）+ packet-id 行在场。**五条→六条投影完备性** ✓：消费面 grep 自跑，canonical 恰四处（planner.md:109 / portfolio-planner.md:58 / acceptance-author.md:46 / DEV-GOVERNANCE.md:291），无漏网；头注 `:291` 承重替换句 verbatim 成立；第 1/4 条作 T3 闸形态模板成立。**单源红利** ✓：dev-loop.md:229 + closeout-check.mjs :309-315 程序化调用 + verify-gates.json:32-34 亲核，「零改动即传导」断言成立。**finalize 代理定性**：「acceptanceTestsFile 非空」= 既有约定（planner Step 8 + 闸 :167 无条件必填），本批新造的只是闸内分支条件——自洽（闸绿仅 finalize 态可达）。

**范围外观察（不入本批 findings，建议主线程另登记 ARQ）**：`pre-merge-escalation-gate.mjs:83-84` green 机锚 regex 裸 `GREEN` 分支与注释意图（「verdict 同段」）不符——任何含 "GREEN" 字样的 PROPOSAL 文本即满足 gov 档第 6 关机锚，向宽失真；本批不触该脚本。

### R2 · architect verdict：NEEDS-WORK（2026-09-09，对 v0.2）

**结论：打回（NEEDS-WORK）**——R1 十四条**全部闭合** ✓（F1 刷新表经判官以 §1 P1 复现命令原文逐字重跑**逐行一致**：91/13/47、八 actor 双 n 列、toolUses 中位、长尾四例含 `e4b70402` phase=token-plan-probe、as-of `c1e26a2` 时间戳、V6 基线三 eventId、§6 派生算术全中）；human 五裁去向逐一核对入文 ✓；结构字段 landing-manifest 11 行零改动 + packet-id 在场 ✓。残余 **SHOULD ×2 / COULD ×4**，全部机械修订、修法已给到可直接誊写，修后 R3 仅聚焦六条不全量重核。全文见 `architect-review-r1.md` R2 节。

**SHOULD 逐条**：
- **R2-F1（v0.2 新增，§1 P2 末条 + §3 硬项 1b + §4 OQ3(b) 注）**：「wire 节引用 4/19（schedule-p2/schedule-reminder/simulator-lvgl-align/g-relay-fix），非双面 2/19」按 **T2 自身集合 {§1.5,§三,§四} + 锚点绑定 interface-contract.md** 不可复现——判官独立普查实测 = **5/19**（schedule-reminder §三·双面 / voice-roundtrip §一+§四·双面 / device-ws-switch §四·双面 / **gateway-directionB §四·server 非双面** / g-relay-fix §1.5+§三·server 非双面）。确定归因：① 谓词未绑文件名——simulator-lvgl-align 的 `§1.5` 锚实指 `docs/notes/feature-inventory.md#§1.5`（其 ic ref 仅裸路径零锚点），误计；② 节集合与 T2 常量分叉——计入 §二（schedule-p2 唯一 ic 锚，不在 T2 集合）、漏 §四（三个真引者全丢）。**净增覆盖 2/19 数值不变、成员应为 gateway-directionB + g-relay-fix**，恰与 v0.1 OQ3(b) 注点名两包吻合，(a) 裁断不受动摇、论据更强。修法：三处按 T2 谓词改写（5/19、双面 3、非双面 2）+ 声明 census 谓词与 T2 常量单源 + §二可并入实现期评估清单。
- **R2-F2（R1 漏审诚实自记，v0.1 即在，V5④ vs T4 portfolio-planner 条）**：V5④ 要求四 def 各含 `input-brief`，但 T4 portfolio-planner 条只指定 crossEnd 预填 + 计数词同步，该 def 现文实测 `grep -c input-brief` = 0——冻结 T4 文本不产出 V5④ 强制元素（非 F7 型互斥：packet 含 §5，执行者可补 mention 满足，但依赖自由发挥 = landing-verifier 对账噪声）。修法二择一：(i) 推荐 T4 portfolio-planner 条补「`:58` 第六条引用点名 input-brief（crossEnd 预填的下游消费方 = planner Step 7 brief 生成）」；(ii) 或 V5④ 收窄三 def。

**COULD 摘要**：R2-F3 §8-A 映射表 V1 交叉引用错位（F11 行引 V1② 实为 F12 断言、F12 行引 V1① 实为 B1 断言）；R2-F4 T7 末重复句（v0.1 旧句残留未删）；R2-F5 B2 地板未显式投影进 T5 括注（起草侧不知地板 → 短 exempt 撞闸红反推，复刻本批自治病灶微形态）；R2-F6 T4 Step 9「记一行」落点未指定（非矛盾——重跑覆盖不破坏 Step 8 在场断言的单调性）。

**§8-B 定性（协调者问 3）**：**B1（counterpart 解析失败 fail-loud）与 B2（exempt ≥12 字符地板）均 = OQ3(a) 必要配套，维持不回退**——B1 是 (a) 二选一语义在解析相位的直接机械投影（静默空锚 = (a) 被空洞满足、牙全泄；不加新字段/不立新门/不扩写面，未扩 human 裁断范围；只打非空 counterparts 条目，空骨架+exempt 路径不受触碰）；B2 是 (a) 文本「理由」二字的可判化（无地板则 `exempt:"是"` 过闸、二选一退化为一选；数源 = 同闸 `:292-293` 既有同型判据 `landsIn=dropped requires reason >= 12 chars`，非拍数）。「human 未单独裁断」标注诚实合规，human 在 FROZEN 授权时天然保留否决位。

**协调者四题答案**：① 生成时点五处（T2 头/T3.3/T4/T6/§3）表述一致 ✓、无旧读法残留，「refs 后改重跑覆盖」与 Step 8「核在场」不构成新两义（在场单调；唯一松点 = R2-F6）；② V3 四补后与 T3 完全可满足 ✓（正态例引用文件实存、字段清单与闸 `:161-251` 核对无缺项），V5 ①②③⑤⑥ 闭合 ✓、④ 臂不闭合 = R2-F2，F7 型互斥未复现；③ B1/B2 维持（见上）；④ F1 新数字全部自洽 ✓，唯一不复现 = P2 wire 节定量（R2-F1），修正后覆盖差论据数值不变成员更强。

**R3 预告**：落 R2-F1–F6 后仅核四条——wire census 三处改写与判官实测五元组一致 + census↔T2 常量单源声明在场；V5④-T4 闭合方式自洽；四条 COULD 落点；无新引入矛盾。

### R3 · architect verdict：NEEDS-WORK（2026-09-09，对 v0.3）→ 触发熔断条款，升 human 三选一

**结论：打回（NEEDS-WORK），唯一项 SHOULD ×1**——R1/R2/R3 三轮未 GREEN，按 `arch-review-protocol.md`「Track B 内熔断升级」升 **human 三选一**（① 转 Track A / ② 撤回·缩圈 / ③ 换新判官实例再战 1 轮，仍不绿自动转 Track A）。照实判、未为免熔断放宽。全文见 `architect-review-r1.md` R3 节。

**预告四条复核结果**：① wire census 判官当场以 T2 同源谓词复算，五元组与文内三处（§1 P2 末条 / §3 硬项 1b / §4 OQ3(b)）**逐字一致**（5/19；schedule-reminder §三·双 / voice-roundtrip §四·双 / device-ws-switch §四·双 / gateway-directionB §四·server / g-relay-fix §1.5+§三·server；双面 3、非双面 2），单源声明 + §二入评估清单在场，归因段与判官归因逐点一致 ✓；② V5④↔T4 取 (i) 闭合成立——最小句形含 `input-brief` 字面（四臂必然可绿）且纯描述下游消费关系（portfolio-planner Boundaries 零触碰）✓；③ 四条 COULD 全到位（R2-F3 两行对位精确、「F11 无专属断言由 V5⑥+T6 人读承载」的处理判官接受不要求补齿；R2-F4 重复句已删；R2-F5 地板入 T5 括注 + 理由；R2-F6 落 brief 头部 `generated-at`/`regenerated-because:`、与 T2②/T6 一致）✓；④ WARN 宿主「Step 7」校正**正确且逻辑自洽有证**（Step 8 硬前置强制 crossEnd 二选一后，WARN 前提〔counterparts 空 ∧ 无 exempt〕在 Step 9 条件重跑中恒不可满足，「唯一宿主时点 = Step 7」为真命题）；「Step 9 生成」读法规范性文本零残留（全 8 处逐核：4 处规范面均为「只引用」，4 处系 §7/§8 历史记录引述，append-only 不改属正当）✓。

**SHOULD 唯一项 R3-F1（v0.3 新增，R2-F1 修法执行时带入，§4 OQ3(b) 括注）**：「gateway-directionB（**OpenClaw 拒 tools 数组那次**）」系史实错置——该事件记录源 = `changes/dev/260903-device-ws-switch/review-packet.md:76` SHOULD 行（verbatim「引擎 `Intent: nointent`（工具调用关闭）——OpenClaw 拒绝 tools 数组（`invalid tool configuration`）…」，即 g-relay-fix notes 所引「WS 包 SHOULD #2」）；gateway-directionB 的 changes/dev 全目录 grep「tools 数组」零命中、其 review-packet `:73` verbatim「无 BLOCKING / SHOULD finding」；事件链 = voice-roundtrip live 遗留（`:109`）→ device-ws-switch SHOULD #2 → g-relay-fix FYI + `ARQ-260907-openclaw-tools-rejection-rootcause`，三环均不系 gateway-directionB；且与同文 §6 留根 (c) 档自引的「WS 包 SHOULD #2」形成**内部矛盾**。census 成员资格不受影响（gateway-directionB 入净增覆盖 2 凭「引 §四 ∧ subsystem=server」机判成立，判官复算在场），错的只是着色短语。**修法 = verbatim 替换句**：括注改「gateway-directionB（引擎方向 B 定向包，refs 引 §四端云协议剖面——`subsystem=server` 单面标签、业务实跨端）」；若保留 OpenClaw 事件色彩，归属应写 g-relay-fix 自身（FYI+ARQ）或指 device-ws-switch SHOULD #2。**严重度论证**：非 COULD——虚假史实将随 FROZEN 入 human 裁断记录（该括注系 v0.3 新文本、human 五裁时未见），文内不可自恢复、会作先例传播；与 R1/F2 同级，重于 R1/F10。

**结构字段** ✓：landing-manifest 11 行 v0.1→v0.3 零改动、packet-id 行在场；§8-D 回灌表与实文逐条对位、主线程自查项如实单列。

**供 human 三选一裁量（判官义务性说明，不代裁）**：三轮打回项均非方向级（R1 = 数据可复现性 + 两处冻结文本自洽性；R2 = census 谓词单源 + V5④ 闭合；R3 = 一个短语的史实错置），批次方向、G1 归类、Track B 降档三轮稳定未动摇；R3 残余项修法零判断量。若选 ③，新判官复核面 = R3-F1 落点 + 无新矛盾两项。

### R4 · architect verdict：GREEN（2026-09-09，对 v0.4，熔断后新判官实例）

**结论：GREEN（零 findings，无升级 human 项）**——human 三选一 ③「换新判官实例再战 1 轮」执行；零上下文新判官按 R3 预告两项复核面独立复核全部通过，未发现前轮漏审的同类残留。全文（含实测命令与输出摘录）见同目录 `architect-review-r4.md`。

**焦点一 · R3-F1 落点到位性 ✓（四项）**：① §4 OQ3(b) 括注现文与 R3 判官替换句 **byte 级一致**（python `in` 断言 True，含 `` `subsystem=server` `` 反引号形态）；旧短语「OpenClaw 拒 tools 数组那次」§4 零残留（全文件 grep 仅 `:281`/`:348` 两处 §7/§8-E append-only 历史记录，正当）。② **新短语三要素独立取证为真**：gateway-directionB taskpackage 实测 `subsystem="server"`（`:12`）、docRefs/sourceRefs 均含 `docs/contracts/interface-contract.md#§四`（`:62`/`:67`）、契约 `:223` 标题实测「四、端云协议剖面（小智协议消息子集…）」；「业务实跨端」独立证据（不采信前轮转述）= 该包 GWA-A2「wss 端口**设备侧可达**」+ GWA-A6「**设备回执回传闭环**」+ GWA-A7「**设备侧消费验收归包 2**」+ story「方向 B 流式网关 bring-up」+ spec.md「方向 B」定义（WebSocket + Opus 流式端云通信）——单面标签而验收面横跨端云，描述语成立。③ §6 (c) 档归属自洽：g-relay-fix notes `:127` 实测 verbatim 含「live 引擎 Intent:nointent（OpenClaw 拒 tools 数组，WS 包 SHOULD #2）」，device-ws-switch review-packet `:76` 实测为其第二条 SHOULD 行（OpenClaw 拒 tools 数组），gateway-directionB changes/dev 全目录 grep「tools 数组|nointent」零命中——§4 现文与 §6 同事件归属矛盾消解。④ **census 三处独立重算**（python 以 T2 同源谓词重跑：锚点绑文件名 ∧ 节 ∈ {§1.5,§三,§四}、排除 current/）：**wire 引用 5/19 = schedule-reminder(§三·双) / voice-roundtrip(§四·双) / device-ws-switch(§四·双) / gateway-directionB(§四·server) / g-relay-fix(§1.5+§三·server)，双面 3、非双面 2 = g-relay-fix + gateway-directionB**——与 §1 P2 末条 / §3 硬项 1b / §4 OQ3(b) 三处逐数吻合；附带复核双面 8/19（八包名单逐一相同）、refIC 19/19、g-relay-fix rigor=lite + 全文件 "firmware" 零命中、P1 表唯一 gateway-directionB 遥测归属 `e4b70402`（builder / token-plan-probe / 42.2min / 97tools 逐字命中）——R3-F1 同类事件归属错误无残留。

**焦点二 · v0.2/v0.3/v0.4 无新矛盾 ✓（五处闭合）**：① 生成时点六处（T2 头 `:132` / WARN 宿主 `:139` / T3.3 `:147` / T4 planner `:157` / T6 `:174` / §3 表 `:193`）全部「Step 7 派发前一次产双份、Step 9 只引用」；全文件「Step 9」9 处 = 规范面 4（均只引用语义）+ 历史面 5（append-only），「Step 9 生成」读法规范面零残留；WARN 唯一宿主 = Step 7 为真命题（Step 8 强制二选一 → WARN 前提在 Step 9 条件重跑恒不可满足，即便 refs 改动新增 wire 引用）。② V5 六臂与 T1/T4 互为可满足：DEV-GOVERNANCE「五条前置清单」实测恰 1 处（`:291`，T1 替换锚 verbatim 吻合）→ V5① 两齿单点替换即满足；agents/ pattern 实测恰 3 处（planner`:109`/portfolio`:58`/aa`:46`）全在 T4 改动集、builder`:46` 无「五条」→ V5③ 零命中可达；T4 冻结文本为四 def 各注入 `input-brief` 字面 → V5④ ==4 必然产出；V5②⑤⑥ 各有 T1 要素/T7/T6 承载——F7 型互斥未复现。③ V3② 正态 fixture 清单与闸实测无条件必填**逐项吻合、多列漏列皆无**（origin∨sourceType `:161` + 八标量 `:163-164` + 七数组 `:167-168` + 双预算 `:247-248` + docsOwner `:250-251`）；条件红项由「照抄 planner.md 示例」覆盖或非 proposal fixture 不触发；显式 argv → 跳过 current 校验且 `resolve(projectDir, explicit)`（`:23-24`）支持仓外绝对路径，V3① 落点规约与闸行为一致；T3.2 引锚 `:167`/`:292`（`reason.trim().length < 12`）实测命中。④ B1 三处（T2②/V1①/§8-B1）同条件同行为、B2 四处（T3.2/T5/V3④/§8-B2）同地板（≥12 字符）同数源（`:292`），无一处写死另一处写活。⑤ `:122` 现文实测关键词联合枚举 + stall 启发式触发注记 + 「纯 advisory、绝不当门」（DP-D4）verbatim——T7 扩型零迁移、触发定义钉同条目与 stall 同型、DP-D4 零改动；T1 第 2 条与 T7 激励措辞「超读是常态动作，不申报才是问题」逐字同一，闭环成立。

**结构字段核对 ✓**：landing-manifest 围栏块在场（`:12-24`，**11 行** = T1(1)+T2(1)+T3(1)+T4(4)+T5(1)+T6(1)+T7(2) 精确闭合，无孤儿无缺项，v0.1→v0.4 零改动）；packet-id 行在场（`:6`）。

**profile / 归类 / 降档 / riskTier**：本轮亲核未发现反证，按派遣纪律不重开（G1+G2+G3、Track B 降档、low-medium 维持 R1 起三轮稳定结论）。

**范围外观察（非 findings，详见 `architect-review-r4.md`）**：(1) `pre-merge-escalation-gate.mjs:83-84` 裸 GREEN 机锚亲核属实（本批正文含 "GREEN" 即恒真，自证其害），主线程**另登 ARQ、不入本批 manifest 的处置恰当**（并入即 scope creep）；本轮 verdict 行「verdict：GREEN」同时命中窄分支 `verdict[：:]\s*GREEN`，ARQ 修法（收窄 verdict 行/段 + self-check 反例）落地后机锚可读性不减，无需本批额外动作。(2) T4 Step 9 重跑触发词钉「refs 有改动」、未含「crossEnd 于 Step 8 后补填」——非矛盾（R2「新鲜度 = Step 9 纪律」定性自洽），暴露面为 planner 无视 WARN 的窄纪律偏差路径；不建议改冻结文本，建议 retro 记录、pilot 期凭 T7 仪表盘观察再议。(3) 标 FROZEN 时建议 Status 行补记 human 三选一 = ③ 裁断（2026-09-09），闭合熔断记录链。(4) §6 (c) 档引 notes 的括号→逗号标点归一微注：内容词逐字在场、零事实影响，不构成任何级 finding，不要求修改。

**主线程可凭「立项 ack + GREEN」标 FROZEN**，进 packet-bound landing（landing-manifest 11 行绑定）→ architect-landing-summary → landing-verifier 独立核验 → retro → 前置升级闸 → 自动合并本地 main。

## 8. Review 定稿记录（主线程机械回灌 · 2026-09-09 · A/B/C 节 = v0.1→v0.2 承 R1 十四条 + human 五裁，D 节 = v0.2→v0.3 承 R2 六条）

> **回灌者 = 主线程（PROPOSAL 作者），非判官**——遵 `.claude/references/track-b-judge-brief.md` 纪律 1「只写评审记录，修订是主线程的事」；判官不越手改正文，R2 判「修订到位性 + 是否引入新矛盾」。作者 ≠ 判官在本批的具体形态。
> **回灌范围 = 已裁来源，无自添设计**：R1 findings 14 条 + human 2026-09-09 五裁（OQ1/OQ2/OQ3 + F1 修法 + ②③档范围）+ 判官亲核留痕。**landing-manifest 11 行零改动**（v0.2 未增删 canonical 目标：②③档与 pre-merge 机锚双双留 §6/ARQ）。

### A · finding → 改法映射（逐条闭合，供 R2 复核与 landing-verifier 对账）

| finding | 落点（v0.2） |
|---|---|
| **F1** BLOCKING 遥测不可复现 | §1 P1 全表按全量重跑刷新（91/13/47，as-of commit `c1e26a2`）＋口径披露段（双 n 列、dur 有效子集、≥40min 阈值、planner/verifier 行不再静默省略）＋ v0.1→v0.2 修订注（差额确定归因）＋**复现命令原文入节**；§5 V6 基线联动刷新；§4 头 OQ 段记 F1 裁断；承载 ARQ 行改口径 13 包/91 条 |
| **F2** SHOULD 双面 9/19 | §1 P2 改 **8/19** + 注明排除 `current/` 指针副本口径 + 八包实测名单；§3 硬项 1b 同步（2026-09-09 重跑） |
| **F3** BLOCKING Step 8/9 自锁 | T4 planner 条改写为「**Step 7 前置一次产双 brief**、Step 9 只引用（refs 后改则重跑覆盖）」；T3.3 括注同步改写并补闸侧调用点枚举（scoped ≥ Step 10 / closeout Step 11）；T2 头部钉生成时点与「只消费 Draft 即齐字段」 |
| **F4** SHOULD in-flight 过渡 | T3 新增「追溯口径与过渡」段（历史包零追溯 / 在飞包一次性摩擦 + 补救 = 补跑 brief + 补填 crossEnd；不设过渡豁免）；非目标第 4 条与 riskTier 行、§3 schema 行同步措辞 |
| **F5** SHOULD wire 节未封闭 | T2 advisory 改为「集合 = 脚本顶部显式常量 + 注释钉维护义务（契约门改节号须同批同步）」，**纳入 §四「端云协议剖面」**，实现期评估 §1.2/§1.3 |
| **F6** SHOULD V5 判别力 | §5 V5 重编为 ①–⑥，补「`五条前置清单` 在 DEV-GOVERNANCE 计数 == 0」与第 6 条内容锚（`input-brief` / `crossEnd` 各 ≥1） |
| **F7** BLOCKING T4↔V5 互斥 | T4 acceptance-author 条改为**计数词替换**「起包清单五条中的②③ → 六条中的②③」（②③ 选集语义保持），并写明「按字面保持即 V5 恒红」的理由，冻结文本无第二读法 |
| **F8** SHOULD 先例封顶三判定源 | T2③ 钉死：merged = `supervisor-verdict.json` APPROVE ∨ pod-events merge 事件（**排除 board**）、subsystem = token 化包含、时间轴 = changeId 日期前缀；V1 加三源子断言 |
| **F9** SHOULD V3 fixture 规约 | §5 V3 补①落点 = 仓外 temp + 清理义务（严禁入 `.claude/taskpackages/`，F2 即污染实例）②既有必填最小清单（逐项列名）③brief fixture 同目录④exempt 地板负例 |
| **F10** COULD 行号 | §0 `:12` → `:11`（带修订标注） |
| **F11** COULD 行号防腐措辞 | T2②(b) 改述为快照语义（时间戳 + 按派发再生 + 权威窗口 = 本次派发），T6 注册文同文重申（**v0.3 按 R2-F3 校正交叉引用：V1 无 F11 专属断言**——快照语义由 V5⑥「`dev-loop.md` 文件地图含 `input-brief`」+ T6 注册文人读承载，不硬造 grep 齿） |
| **F12** COULD 无锚点 refs | T2① 补「标『无锚点（整文件）』+ advisory 提示补锚」，**V1②** 加子断言（v0.2 原写 V1① 系错位，V1① 实属 §8-B1 fail-loud） |
| **F13** COULD V6 计数口径 | §5 V6 钉「冻结后再派」口径，基线 4 → **3**（列三 eventId），fixer 轮数 12 包 → **13 包** 共 3 次 |
| **F14** SHOULD over-scan 无生产者 | T7 第一条补触发定义：交付报告超读段非空 → planner 落 `over-scan` friction（summary 引申报行数），与 T1 第 2 条激励措辞连成闭环 |

### B · 主线程加码修订（**非** R1 findings、**非**判官建议，来源如实标注待 R2 裁）

| # | 改法 | 来源与状态 |
|---|---|---|
| **B1** | T2② counterpart 解析失败 fail-loud（path 不存在 / symbol 零命中 → 脚本非零退出点名条目，不静默产空锚）+ V1① 子断言 | 主线程 2026-09-09 在向 human 解释 OQ3 时提出（「(a) 的牙」一节），**human 未就该细节单独裁断**，仅裁 (a) 本体；随 (a) 一并送 R2 判：若判官认为属新设计/超 (a) 语义，回退为 advisory 亦可，不影响批次方向 |
| **B2** | T3.2 `exempt` 长度地板 ≥12 字符，对齐本闸 `:292` 既有同型判据 + V3④ 负例 | 同 B1（主线程提出，引本仓既成先例，非拍数）；送 R2 判 |

### C · human 裁断登记（2026-09-09，逐项对应 §4 与两处非 OQ 问）

1. **OQ1 = (a)** 扩 `over-scan` 新型（连钉生产者触发，R1/F14）；
2. **OQ2 = (a)** 不预授权 phase-2 digest；
3. **OQ3 = (a)** 骨架必在 + 二选一，**且明令连批 T1 第六条 + T4 派发指令与 Inputs 封闭化**（判官 I 节同向：(a) 单独落地即靠闸红反推）；
4. **F1 修法 = 全量重跑刷新**（非 pin as-of）；
5. **消费方②③档 = 本批只做自有面，参考树与黑盒两档留 ARQ**（§6 首条）。

R1 唯一「FROZEN 前置确认项」（三 OQ 未显式裁）随第 1/2/3 项闭合。**R2 复核范围**：A 表 14 条逐条到位性 + B 两件的定性（属 (a) 必要配套？还是新设计须回退？）+ 修订是否引入新矛盾（重点自核：T2/T3/T4 三处生成时点表述是否已一致、V3/V5 与 T3/T4 改后文本是否仍互为可满足）。

### D · v0.2 → v0.3 回灌（R2 六条，2026-09-09）

> R2 verdict = NEEDS-WORK（**SHOULD 2 / COULD 4**，判官明示 R1 十四条全闭合、无升级 human 项、修后 R3 仅聚焦六条不全量重核）。回灌者仍 = 主线程。

| finding | severity | 落点（v0.3） |
|---|---|---|
| **R2-F1** | SHOULD | §1 P2 末条按 T2 同源谓词重算：**wire 节引用 5/19、双面 3、非双面 2 = g-relay-fix + gateway-directionB**（净增覆盖 2/19 不变、成员改正）+ 主线程自查归因两条（谓词未绑文件名 / 节集合与 T2 常量分叉）；T2 advisory 补**单源声明**（census 与上线 advisory 共用同一常量）并把 §二 并入实现期评估清单；§3 硬项 1b 与 §4 OQ3(b) 注三处同步 |
| **R2-F2** | SHOULD（R1 漏审、v0.1 即在） | 取判官建议 (i)：T4 portfolio-planner 条补「`:58` 第六条引用须点名 `input-brief`」并给最小句形 → V5④「四 def 各含 input-brief」四臂全闭合，不留执行者自由发挥位 |
| **R2-F3** | COULD | §8-A 的 F11/F12 两行交叉引用改正：V1① 归 §8-B1（fail-loud）、V1② 归 F12（无锚点 refs）、F11 明示「V1 无专属断言，由 V5⑥ + T6 注册文承载」（不硬造 grep 齿） |
| **R2-F4** | COULD | T7 末 v0.1 旧句「对应 P3，并给 OQ2（digest phase-2）提供决策数据面。」重复残留删除（留 v0.2 带裁断标注的那条） |
| **R2-F5** | COULD | T5 字段定义括注显式投影 `exempt ≥12 字符` 地板并注明理由（起草侧看不到地板 → 短 exempt 撞闸红反推 = 本批病灶在第六条自身上的微形态复现） |
| **R2-F6** | COULD | T4 Step 9「重跑覆盖并记一行」落点指定 = **落制品自身**（brief 头部 `generated-at` 刷新 + `regenerated-because:` 一行，Git-tracked、`git diff` 可审），planner 不另写 journal/prose，免第二事实源 |
| 主线程自查附加 | — | T2 advisory 的 WARN 宿主时点「Step 7/9」→「Step 7」（F3 改点后的残留，非判官点名项，如实标注请 R3 一并核） |

**R3 复核范围（判官预告四条）**：① wire census 三处改写与实测五元组一致 + 单源声明在场；② V5④↔T4 闭合方式自洽；③ 四条 COULD 落点；④ 无新矛盾（含上表末行主线程自查项）。

### E · v0.3 → v0.4 回灌（R3 一条 + 熔断登记，2026-09-09）

| finding | severity | 落点（v0.4） |
|---|---|---|
| **R3-F1** | SHOULD | §4 OQ3(b) 括注按判官 verbatim 替换句改正：`gateway-directionB（OpenClaw 拒 tools 数组那次）` → `gateway-directionB（引擎方向 B 定向包，refs 引 §四端云协议剖面——subsystem=server 单面标签、业务实跨端）`。主线程自查确认：`changes/dev/260903-gateway-directionB/` 与 `.claude/taskpackages/260903-gateway-directionB/` 全目录 grep「tools 数组 / nointent」**零命中**；该事件原始记录 = `changes/dev/260903-device-ws-switch/review-packet.md:76` 的 SHOULD 行（g-relay-fix notes 所称「WS 包 SHOULD #2」即此），与 §6 留根 (c) 档自引「WS 包 SHOULD #2」的异归属矛盾同时消解。**census 成员资格不受影响**（gateway-directionB 入 2/19 凭「引 §四 ∧ `subsystem=server`」机判成立，判官已复算在场），错的只是着色短语。 |

**两条同源归因（供本批 retro 与 authoring 复评）**：R2-F1（census 谓词未绑文件名）与 R3-F1（为给数字加画面感，把另一事件的色彩缝到本事件上）同一个毛病——**主线程写「防御性 / 着色性」prose 时没有复用机判谓词的单一事实源**。本批已把单源声明写成条文（v0.3 / T2 advisory），条文在落地前就先打了自己作者两次；此即「成文 + 字段 + 闸」三件套里为何**着色句也要引谓词、不得重写一套**的实证。

**熔断登记**：Track B「同判官 3 轮未 GREEN」条款于 R3 触发 → 升 human 三选一（① 转 Track A full-adversarial / ② 撤回·缩圈 / ③ 换新判官实例再战 1 轮，仍不绿自动转 Track A）。判官附义务性说明（不代裁）：三轮打回项均非方向级（R1 = 数据可复现 + 冻结文本自洽；R2 = census 单源 + V5④ 闭合；R3 = 一个短语的史实错置），方向 / G1 归类 / Track B 降档三轮稳定未动摇，R3 残余项零判断量且替换句已备；若选 ③，新判官复核面 = R3-F1 落点 + 无新矛盾两项。human 裁断在 §7 之后另行登记，本表不预判。
