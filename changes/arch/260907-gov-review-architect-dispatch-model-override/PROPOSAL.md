# PROPOSAL · 260907-gov-review-architect-dispatch-model-override

> **状态：FROZEN v1.1 · 2026-09-07 · 凭据：human 立项 ack + OQ1/2/3=(a)/(a)/(a) 裁断（2026-09-07，回灌 §4）+ architect R1 GREEN（0 BLOCK / 3 NIT；F1 census 下界化 / F2 JSON 衔接句 / F3 依赖成文已按修订要求回灌，均不触改法实体）**
> **承载 ARQ**：`ARQ-260907-architect-dispatch-model-override`（docs/ARCH-REVIEW-QUEUE.md §1.2；单一根，无并批）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。Track A 触发时改标 Track A + 命中判据。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T2）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（机械拦截面窄：仅命中「architect 派发 + model 参数」一个组合；builder/fixer 授权升档通道不动；行为保持性由自检用例表与探针保证）。
> **review profile**：G2+G3（`.claude/agents/planner.md` 正文消歧一句 + 新增一个 G3 hook + `.claude/settings.json` 注册）。G1 与契约面为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/hooks/agent-model-guard.mjs
.claude/settings.json
.claude/agents/planner.md
```

---

## 0. 想实现的效果与差距

目标效果一句话：**planner（及任何派发方）派 architect 时，`model` 参数被机械拦截，architect 恒以 frontmatter `model: fable` 默认档运行；MODEL-TIER（planner.md:57）授权的 builder/fixer 升档通道不受影响。**

现状差距：planner.md:71 ARCH-ADVISORY 档已明文「派发不带 model 覆盖」，但该约束只存在于 prompt 层——2026-09-07 directionB worktree Pod 实测违反（§1 证据一），fable 默认档被 `model: "opus"` 静默绕过。纯 prompt 约束已实测失效一次，靠再写一遍不可靠。

非目标（本批不做）：

- 不泛化拦截到其它 agent 的 model 覆盖（builder/fixer 的 MODEL-TIER 升档是 FROZEN 授权面，保持原样；泛化方案作为 OQ1 选项 (b) 留给 human，若裁 (a) 则本批只拦 architect）。
- 不动 AInativecompany 仓的 agent 定义漂移（非本仓治理面，见 §6 留根与 OQ3）。
- 不改任何 agent frontmatter 的 `tools`/`name`/`model`/write scope——planner.md 只做正文规则句消歧。
- 不动模型档位映射（cc-switch 侧 fable→上游模型映射是用户环境配置，不属仓内治理）。

## 1. 问题与根因（地面真值）

### 1.1 问题：architect 派发被显式 `model: opus` 覆盖，fable 默认档失效

**证据一（执行记录，harness 派发 meta）**——directionB worktree dev-loop Pod 会话 `a83cc1a6-cee2-4edd-afef-638e59893bca`（项目目录 `C:\Users\15802\.claude\projects\c--AI-KidPal-pod-gateway-directionB`），2026-09-07T04:43:46.980Z 派发 architect（「架构评审：display fence 修复落点」），Agent 调用 input 携带 `"model":"opus"`。子 agent meta.json 逐字：

```json
{"agentType":"architect","description":"架构评审：display fence 修复落点","toolUseId":"call_279a0e8aecaa49569179f210","spawnDepth":1,"model":"opus"}
```

路径：`...\subagents\agent-a9ca880c7b1c844d1.meta.json`。该子 agent transcript（`agent-a9ca880c7b1c844d1.jsonl`）30 条 assistant 消息 model 字段全部为 `glm-5-3-flash`（起草期 2026-09-07 机械计数：`Counter` 扫 transcript 实测）。

**证据二（模型侧对照，cc-switch `proxy_request_logs`）**——2026-09-07 当日（本地时区）代理侧 request_model 分布：`claude-opus-4-8`→`glm-5-3-flash` 834 条、`claude-sonnet-4-6`→`deepseek-v4-flash-ga-260731` 788 条、**`claude-fable-5` 0 条**。即本会话 architect 的请求走了 opus 档，fable 档根本未发出。

**证据三（对照组，同一 frontmatter 无覆盖时 fable 生效）**——前日（2026-09-06）KidPal 主会话 `a035a29d` 派 architect 评审 260906 批（2026-09-06T09:17:45Z，**input 无 model 参数**），代理侧 09:17:55–09:47:46 UTC 收到 **14 条 `request_model=claude-fable-5`**；子 agent transcript（`agent-a1e8d3274ed52d983.jsonl`）46 条消息 model 全为 `k3`（fable 档在代理侧的当日落地模型）。同一段 frontmatter，有无 model 参数是唯一差异——「Agent 调用 model 参数优先级 > agent frontmatter」由此组对照实测成立，非文档推断。

**证据四（同会话授权升档对照）**——同一 worktree 会话内 builder `a0327d6ca67d6f729`、integrator `adcd7c5adeede980c`/`aff2ce46e75f48f3d` 派发同样显式带 `model: opus`，这些是 planner.md:57 MODEL-TIER **明文授权**的升档（「派重头形态时派发显式带 `model: opus` 升档」），行为合法。违反仅在 architect。

### 1.2 根因：MODEL-TIER「派发期覆盖」手法无边界句，prompt 约束单层承载

planner.md:57（起草期逐字核对于本节末）教给 planner 的手法是「派发显式带 `model: opus` 升档」，且补了实测背书（「派发期覆盖已实测生效」）——但该条未写明授权**仅限 builder/fixer**。planner.md:71 虽对 architect 明文禁止，却是另一条独立条款；两处相隔 14 行，planner 在派发期把 57 的手法泛化到了 architect。约束写在 prompt 层而模型执行在派发瞬间，中间无机械面兜底——失效一次即证此层不足。

根因 = **规则边界句缺失（G2）+ 派发期无机械防呆（G3）** 的组合。修 G2 防再教错，修 G3 防再走错；单修其一都留下另一半缺口。

§1.2 引据逐字（起草期 2026-09-07 `sed -n '57p;71p' .claude/agents/planner.md` 实测）：

> **[MODEL-TIER，gov-review-agent-model-tiering FROZEN 2026-08-31]** builder/fixer frontmatter 默认 `model: sonnet`；派重头形态（新子系统首次实现 / 验收面从零钉 / 首次全量复验）时派发显式带 `model: opus` 升档，例行形态沿用默认。派发期覆盖已实测生效（260831 PROPOSAL §5-5）；dispatch-stamp 若可得须补记实际生效模型。

> 10b. **[ARCH-ADVISORY 疑难分析档]** dev loop 进行中疑难问题可派 architect（其 frontmatter `model: fable` 默认档生效，**派发不带 model 覆盖**）出只读 advisory 分析意见，执行与裁定仍归原链（fixer 拥有补丁、verifier 拥有判定）。

**【硬项 1a · 起草期实证义务】声明**：

- 「matcher=Agent 的 PreToolUse hook 在本仓 Claude Code 版本（2.1.263）可触发」——**未实测**（本仓既有 hooks matcher 先例仅 Bash / Edit|Write|MultiEdit / Grep|Glob，无 Agent 先例；settings.json 不可能起草期改动自测）。诚实标注，T1 设计已含缓解（matcher 写 `Agent|Task` 双形态）+ §5 探针 V5 在 landing 注册后实测，探针红即打回。
- 「tool_input 键名为 `subagent_type` / `model`」——实测依据：transcript 中 Agent tool_use 的 input JSON 逐字即用此二键（证据一同源），PreToolUse stdin 的 `tool_input` 即该 input 对象（deny-main-push.mjs:503 同构读取 `tool_input` 先例）。
- 「Bash 子进程改不了 Claude Code 主进程环境变量」——操作系统语义（父子进程环境单向继承），非实测项，作为 OQ2 (a) 的设计依据如实标注性质。
- 数字引用（834 / 788 / 0 / 14 / 30 / 46 条）——起草期 2026-09-07 现跑 SQLite 聚合与 transcript `Counter` 计数实测，非转引。
- 脚本入口存在性——`route-regression-check.mjs` / `lint-descriptions.mjs` / `landing-fidelity-check.mjs` 起草期 `ls .claude/scripts/` 亲核存在；`route-regression-check.mjs` 含 `--self-check` 入口（grep 实测）。

## 2. 改法（T1–T2）

### T1 · 新增 G3 hook：agent-model-guard（architect 派发 model 参数 deny）

**改什么**：新增 `.claude/hooks/agent-model-guard.mjs`（新文件，模式对标 `deny-main-push.mjs`：stdin 读 PreToolUse JSON → 判定 → deny exit 2 / 放行 exit 0，附 `--self-check` 结构化自检）；`.claude/settings.json` 注册 PreToolUse matcher。

**怎么改**：判定 pattern 窄而准——`tool_input.subagent_type === 'architect'` **且** `tool_input.model` 为非空字符串 → deny；其余一切派发（含 builder/fixer 带 model、architect 不带 model）放行。deny 文案引 planner.md:71 条款，给 agent 自纠路径（去掉 model 参数重派）。逃生门见 OQ2。

**落地文件全文逐字钉死**（`.claude/hooks/agent-model-guard.mjs` 新增，逐字节按此落地）：

```js
#!/usr/bin/env node
// ARQ-260907-architect-dispatch-model-override（gov-review-architect-dispatch-model-override，G3 机械防呆）：
// architect 派发携带 model 参数 → deny。frontmatter `model: fable`（architect.md:5）是 FROZEN 默认档，
// planner.md:71 ARCH-ADVISORY 档明文「派发不带 model 覆盖」；MODEL-TIER（planner.md:57）升档授权仅限 builder/fixer。
// 判据：PreToolUse(Agent|Task) tool_input.subagent_type === 'architect' 且 tool_input.model 为非空字符串 → exit 2。
// builder/fixer 等其它 agent 的 model 覆盖不在本闸范围（MODEL-TIER 授权面，不误伤）。
// 逃生门（human 裁定后生效，见 PROPOSAL OQ2）：env KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1 放行——
//   仅会话启动者/human 可设（Bash 子进程改不了 Claude Code 主进程环境，agent 无法自开）；设置时放行并打印 advisory。
//   完备性依赖（R1-F3）：agent 写 `.claude/**` 被封（scope-map universal.bashDenyWrite）兜住另一条路——
//   将来若放宽 `.claude` 写面，本逃生门随之敞开，须同步复评本门。
// self-check：--self-check 跑内置用例表，输出 JSON {selfCheck,pass,fail,total,ok}，fail>0 → exit 1。
//
// owner=architect, approver=human (G3 hook, see DEV-GOVERNANCE.md §6)

import { readFileSync } from 'node:fs';

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

function block(reason) {
  process.stderr.write(reason);
  process.exit(2);
}

function judge(toolInput) {
  const subagentType = String(toolInput?.subagent_type || '');
  const model = toolInput?.model;
  if (subagentType !== 'architect') return { block: false };
  if (typeof model !== 'string' || model.trim() === '') return { block: false };
  return {
    block: true,
    reason: `agent-model-guard: architect 派发携带 model="${model}" — planner.md:71 ARCH-ADVISORY 档 FROZEN 条款「派发不带 model 覆盖」禁止覆盖，frontmatter model: fable 默认档才生效。去掉 model 参数重派；MODEL-TIER 升档授权仅限 builder/fixer（planner.md:57）。`,
  };
}

function runSelfCheck() {
  const cases = [
    // 假阴（architect 带 model → deny）
    { input: { subagent_type: 'architect', model: 'opus' }, expectBlock: true },
    { input: { subagent_type: 'architect', model: 'fable' }, expectBlock: true },
    { input: { subagent_type: 'architect', model: 'sonnet' }, expectBlock: true },
    { input: { subagent_type: 'architect', model: ' claude-opus-4-8 ' }, expectBlock: true }, // 带空白仍 deny
    // 假阳（architect 不带 model / 空串 → 放行）
    { input: { subagent_type: 'architect' }, expectBlock: false },
    { input: { subagent_type: 'architect', model: '' }, expectBlock: false },
    { input: { subagent_type: 'architect', model: '   ' }, expectBlock: false },
    { input: { subagent_type: 'architect', model: null }, expectBlock: false },
    // 授权升档面（builder/fixer/其它 agent 带 model → 放行，不误伤 MODEL-TIER）
    { input: { subagent_type: 'builder', model: 'opus' }, expectBlock: false },
    { input: { subagent_type: 'fixer', model: 'opus' }, expectBlock: false },
    { input: { subagent_type: 'integrator', model: 'opus' }, expectBlock: false },
    { input: { subagent_type: 'Explore', model: 'haiku' }, expectBlock: false },
    { input: { subagent_type: 'general-purpose', model: 'sonnet' }, expectBlock: false },
    // 畸形 input → 放行（fail-open：闸只对可判定的违例出手，不扩大误伤）
    { input: {}, expectBlock: false },
    { input: { subagent_type: 123, model: 'opus' }, expectBlock: false },
  ];
  let pass = 0;
  const failures = [];
  for (const c of cases) {
    const r = judge(c.input);
    if (r.block === c.expectBlock) pass++;
    else failures.push({ input: c.input, expect: c.expectBlock, got: r.block });
  }
  const ok = failures.length === 0;
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'agent-model-guard', pass, fail: failures.length, total: cases.length, ok, failures: ok ? undefined : failures })}\n`,
  );
  if (!ok) process.exitCode = 1;
}

// ---- 入口 ----
if (process.argv.includes('--self-check')) {
  runSelfCheck();
} else {
  let input;
  try {
    input = JSON.parse(readStdin() || '{}');
  } catch {
    process.exit(0);
  }
  const result = judge(input?.tool_input);
  if (result.block && process.env.KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE !== '1') {
    block(`blocked by ${result.reason}`);
  }
  if (result.block && process.env.KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE === '1') {
    process.stderr.write('agent-model-guard: KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1 — human 一次性豁免生效，本次放行。\n');
  }
  process.exit(0);
}
```

`.claude/settings.json` 的注册指令逐字钉死——在 `"hooks"` → `"PreToolUse"` 数组内、`"matcher": "Edit|Write|MultiEdit"` 整个对象（当前 55–63 行）之后追加以下对象（逗号衔接合法，落位后 PreToolUse 共三个 matcher 块：Bash / Edit|Write|MultiEdit / Agent|Task）：

```json
      {
        "matcher": "Agent|Task",
        "hooks": [
          {
            "type": "command",
            "command": "node \"${CLAUDE_PROJECT_DIR}/.claude/hooks/agent-model-guard.mjs\""
          }
        ]
      }
```

`.claude/settings.json` 的 `_doc` 数组末尾追加一行（逐字）。**JSON 衔接（R1-F2）：现行 `_doc` 末行（settings.json:8，`"permissions.allow 只收可复用通配条目；一次性命令由会话临时授权承载。"` 行）无尾逗号，须先给该行补尾逗号再追加本行**：

```json
    "agent-model-guard（ARQ-260907-architect-dispatch-model-override）：PreToolUse(Agent|Task) 拦 architect 派发带 model 参数——planner.md:71「派发不带 model 覆盖」的机械兜底；MODEL-TIER 授权升档（builder/fixer）不拦。逃生门 env KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1 仅会话启动者可设。"
```

**为什么这样改**：对 §1.2 根因的 G3 半边——prompt 约束失效一次即证不足，派发瞬间的机械面是唯一可靠的兜底层；判据收窄到「architect + model 非空」精确命中违例组合，MODEL-TIER 授权面零误伤（自检用例表把两个面都钉死）。matcher 写 `Agent|Task` 双形态是对「Agent 工具名跨版本可能为 Task」的版本鲁棒性缓解（§1 硬项 1a 未实测项），多余 matcher 不匹配时无行为。

目标文件：`.claude/hooks/agent-model-guard.mjs`（新增）、`.claude/settings.json`（改）。
（格式修订 2026-09-07，landing-verifier 结构门实测：锚行原为粗体 `**目标文件**：`，机械门 `parseSection2AnchorLists` 只认行首裸 `目标文件：`——去粗体过门，语义零改；260906 批同款病灶第二例。）

### T2 · planner.md MODEL-TIER 段补边界句（G2 消歧）

**改什么**：`.claude/agents/planner.md:57` MODEL-TIER 段末，在手法被「教会」的位置钉死授权边界。

**怎么改**——承重 landing 指令原文引用钉死：现行第 57 行整句为

> **[MODEL-TIER，gov-review-agent-model-tiering FROZEN 2026-08-31]** builder/fixer frontmatter 默认 `model: sonnet`；派重头形态（新子系统首次实现 / 验收面从零钉 / 首次全量复验）时派发显式带 `model: opus` 升档，例行形态沿用默认。派发期覆盖已实测生效（260831 PROPOSAL §5-5）；dispatch-stamp 若可得须补记实际生效模型。

替换后完整句（在该行末尾、`dispatch-stamp 若可得须补记实际生效模型。` 之后追加一句，行内其余不变）：

> **[MODEL-TIER，gov-review-agent-model-tiering FROZEN 2026-08-31]** builder/fixer frontmatter 默认 `model: sonnet`；派重头形态（新子系统首次实现 / 验收面从零钉 / 首次全量复验）时派发显式带 `model: opus` 升档，例行形态沿用默认。派发期覆盖已实测生效（260831 PROPOSAL §5-5）；dispatch-stamp 若可得须补记实际生效模型。**升档覆盖仅限 builder/fixer；architect 派发恒不带 model 覆盖（见 10b ARCH-ADVISORY 档，frontmatter `model: fable` 默认档生效；机械兜底 = agent-model-guard hook）。**

**为什么这样改**：对 §1.2 根因的 G2 半边——把边界句放在手法模板句的同一行内（而非隔着 14 行的另一条），派发期读到手法即读到边界；同时把 T1 hook 写进句内，让 G2 条款与 G3 机械面互相指称、防未来单边退役。

目标文件：`.claude/agents/planner.md`（改）。
（格式修订 2026-09-07，同上：去粗体过机械门，语义零改。）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 强制执行层 | `.claude/hooks/agent-model-guard.mjs` | 新增/G3 | 新 hook，PreToolUse(Agent|Task)；自检入口 `--self-check` |
| 强制执行层 | `.claude/settings.json` | 改/G3 | PreToolUse 注册 + `_doc` 登记行；全会话常驻 hook 三处并查纪律的 settings.json 侧 |
| 角色定义 | `.claude/agents/planner.md` | 改/G2 | 仅正文规则句消歧；frontmatter（含 model: opus）不动；不改 tools/name/write scope |
| 派发消费方 | 全部 planner 派发面（dev-loop 各 Pod + 主线程 + review-moderator 对 architect 的派发） | 未触/行为面 | 派 architect 不带 model 的既有合法形态全部保持原行为（自检用例 + V5 探针覆盖）；仅「带 model 派 architect」这一违例形态被拦 |
| 验证脚本 | `.claude/scripts/route-regression-check.mjs`、`lint-descriptions.mjs` | 未触 | T2 只追加正文句，不动 description；V3 复跑作回归确认 |
| 契约面 | `docs/contracts/**` | 未触 | 零接触 |
| 上游工程 | AInativecompany 仓 `.claude/agents/architect.md` | 未触（本批） | 分叉漂移 `model: opus`，非本仓治理面，§6 留根 + OQ3 |

**【硬项 1b · sweep 透镜义务】** 本批落地新门，判定 pattern = 「Agent 派发 + `subagent_type=architect` + `model` 非空」。全仓预跑形态：仓内无 Agent 派发日志的机械面（`.claude/taskpackages/*/dispatch-log.jsonl` 仅记 actor/phase/tokens，**不记 model 参数**——起草期 2026-09-07 读 `260903-gateway-directionB/dispatch-log.jsonl` 实测），故 census 只能在 harness 侧 transcript 做：起草期 2026-09-07 对 3 个项目（KidPal 主 checkout、KidPal-pod-gateway-directionB、AInativecompany）harness 侧截至当日的既有会话全量机械枚举 Agent 派发（`json.loads` 逐行扫 `type=assistant` 的 `Agent` tool_use；窗口 = 全量无时间下界，故计数记下界），结果 = architect 派发 ≥19 次（R1 判官独立枚举复核一致：KidPal 主项目 14 / AInativecompany 4 / directionB worktree 1），其中带 model 覆盖恰 1 次（即 §1 证据一）；builder/fixer/integrator 带 model 覆盖 4 次（MODEL-TIER 授权形态，同枚举）。本闸若已在线，命中集 = 恰那 1 次，无误伤（R1 判官独立枚举复核证实）。

**【硬项 1c · 复扫模式完备性先验】** 本批无退役语面/残留清扫（纯新增 hook + 追加一句），不适用。

## 4. human 取舍点（OQ）

- **OQ1 · 拦截范围**：(a) 仅拦 architect（推荐——精确命中已实测违例面，自检/探针简单，零误伤风险）；(b) 泛化拦「frontmatter 钉 model 且无授权条款」的全部 agent（一劳永逸，但需 hook 读 agent def frontmatter 做授权白名单，机械复杂度与误伤面显著上升，且其它 agent 的违例尚无执行证据——对 ARQ 快路径「修复明确无设计分叉」是个扩张）；(c) 只 warn 不 deny（保持 agent 可继续，监督靠事后 transcript 审计——约束力回到 prompt 层，等于没修根因）。**【human 裁断 2026-09-07：(a)】**
- **OQ2 · 逃生门**：(a) env `KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1` 一次性豁免（推荐——human 显式跳过流程的铁律本就存在（Hard Rules），给个干净通道；env 只有会话启动者能设，agent 无法自开）；(b) 无逃生门，向严（真要豁免时临时改 hook 源码——改动本身又要走治理，应急成本更高）。**【human 裁断 2026-09-07：(a)——附裁断理由：deny-main-push 可无逃生门因 push-to-main 有正道（分支+合并），而「architect 临时换档」无别的正道，堵死反逼出改源码的更脏应急路径；变量不设时 (a) 与 (b) 行为完全一致。】**
- **OQ3 · AInativecompany 旁支**：(a) 本批不动，§6 留根（推荐——非本仓治理面，跨仓治理动作不应夹带）；(b) 本批同期去那仓走一次独立小改（那仓 agent def 同样须 human-approved，另起炉灶）。**【human 裁断 2026-09-07：(a) 留根不动】**

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · hook 自检（真空绿规约：自检输出含 `selfCheck: 'agent-model-guard'` 名 + 全用例计数，先断言输出非空再判 `ok`）**：`node .claude/hooks/agent-model-guard.mjs --self-check` → stdout JSON `ok:true`，`fail:0`，`total:15`。
- **V2 · hook 探针（stdin 实喂三形态）**：`echo '{"tool_input":{"subagent_type":"architect","model":"opus"}}' | node .claude/hooks/agent-model-guard.mjs` → exit 2 + stderr 含 `agent-model-guard:`；`echo '{"tool_input":{"subagent_type":"architect"}}' | …` → exit 0；`echo '{"tool_input":{"subagent_type":"builder","model":"opus"}}' | …` → exit 0。
- **V3 · G2 回归**：`node .claude/scripts/route-regression-check.mjs` → exit 0（绿）；`node .claude/scripts/lint-descriptions.mjs` → exit 0（T2 只动 planner.md 正文句，description 未触，预期零新告警）。
- **V4 · settings.json 结构**：`node -e "JSON.parse(require('fs').readFileSync('.claude/settings.json','utf8')); console.log('settings.json OK')"` → `settings.json OK`；且 PreToolUse matcher 块计数 = 3（Bash / Edit|Write|MultiEdit / Agent|Task）。
- **V5 · 在线触发探针（landing 后，验证 §1 硬项 1a 未实测项）**：landing 完成后的新会话中，主线程派一次 `subagent_type=architect` + `model: "opus"` 的 Agent 调用 → 预期被 PreToolUse deny（stderr 含 `agent-model-guard:`）；随后不带 model 重派同任务 → 预期正常起跑且 transcript meta 无 model 字段。探针红（未被拦）→ 判官/verifier 打回，不得以「V1–V4 绿」抵充。
- **V6 · landing 保真度**：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-architect-dispatch-model-override` → GREEN（anchor 集 ≡ manifest 三路径并集）。

## 6. 留根清单

- **AInativecompany 仓 architect 定义漂移**（`model: opus`，分叉未同步）：留根待 OQ3 裁断；裁 (a) 则该仓治理批另起。
- **`dispatch-log.jsonl` 不记 model 参数**（`.claude/taskpackages/*/dispatch-log.jsonl` 现无 model/dispatch 参数字段，本批 census 只能靠 harness transcript）：登记为观测 gap，不属本批修复面；若未来需要机械审计派发形态，由 pod-events/pod-retro 面的治理批承接。
- **模型档位命名混淆**（代理侧 fable 档落地名随 provider 漂移：`k3` / `glm-5.3`，与 opus 档 `glm-5-3-flash` 字面相近，人查「有没有用 fable」易误读）：用户环境配置面，不入仓治理；本批 §1 证据二已把「以 `request_model` 档位名为准」的核对口径成文，供后续追查复用。
- **逃生门完备性依赖成文（R1-F3）**：env 逃生门防 agent 自开依赖两层——「子进程改不了父进程 env」（OS 语义，已写入 hook 注释）+「agent 写不了 `.claude/**`」（scope-map universal.bashDenyWrite 兜底）。将来任何放宽 `.claude` 写面的治理批，须同步复评本逃生门（hook 源码注释已钉同一提醒）。
- **自律件**：本批 landing 后，主线程/各 Pod 派 architect 的 dispatch-stamp 复核（meta.json model 字段应为空）纳入 loop-retro 例行抽查面。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-07 - architect

**结论：GREEN**（0 BLOCK / 3 NIT；无升级 human 项）。R2 轮次若开，聚焦 F1–F3 修订到位性即可。

**Track B 承载裁定**：本批含「新增机械门（PreToolUse deny hook）」事实，ARQ 行（docs/ARCH-REVIEW-QUEUE.md §1.2，`ARQ-260907-architect-dispatch-model-override`）与 PROPOSAL 头部 review profile 均已如实声明交判。判 AGENTS.md「治理路由」升级判据两项均不命中（不触 `docs/contracts/**`；G2 一句消歧 + G3 单点 hook 非跨系统结构性变更），且先例成立——本仓机械门新增此前已由 Track B 承载（260802-gov-review-track-b-hardening 落 landing-fidelity-check）。**Track B 可吃下，不须转 full-adversarial。**

**三要件机判**：① 执行测出——§1 四条证据全部亲核属实（见下留痕）；② 修复明确无设计分叉——T1/T2 各治 §1.2 根因的 G3/G2 半边，OQ1/2/3 均已 human 2026-09-07 裁断 (a)/(a)/(a) 并回灌 §4；③ 半径已照出——§3 sweep 表 + census 命中断言经判官独立枚举证实（恰 1 次命中、授权面零误伤）。

**G2/G3 机检硬排除逐项**：不触 `docs/contracts/**` ✓；不改任何 agent frontmatter tools/name/model（T2 仅 planner.md:57 行末追加正文句）✓；非 self-def（architect.md）与 review-loop agent def 改动（planner.md 不属 review-loop 八 def）✓；新增 gate 事实已如实声明交判 ✓；非 mixed lane ✓；不触 L0 ✓。

**硬项核对**：

- 硬项 1a（起草期实证义务）：通过。引据逐字核对——planner.md:57 / planner.md:71 引文与本仓实文逐字一致；architect.md:5 `model: fable` 属实。证据一 meta.json 逐字一致（判官亲读 `C:\Users\15802\.claude\projects\c--AI-KidPal-pod-gateway-directionB\a83cc1a6-cee2-4edd-afef-638e59893bca\subagents\agent-a9ca880c7b1c844d1.meta.json`，含 `"model":"opus"`）；该 transcript 判官机械计数 = 30 条 assistant 消息全 `glm-5-3-flash`，与 PROPOSAL 一致。证据三对照组成立——主会话 a035a29d 于 2026-09-06T09:17:45.002Z 派 architect 无 model 参数（Agent tool_use input 实读），子 agent meta.json 无 model 字段，transcript 46 条 assistant 消息全 `k3`，cc-switch `proxy_request_logs` 该日 09:17:55–09:47:46 UTC 窗口 `request_model=claude-fable-5` 恰 14 条（判官 SQLite 独立聚合）；同 frontmatter、有无 model 参数为唯一差异的对照成立。证据四 builder/integrator opus 授权形态属实（a0327d6ca67d6f729 / adcd7c5adeede980c / aff2ce46e75f48f3d meta 均带 `"model":"opus"`）。「matcher=Agent 可触发未实测」如实标注 + V5 在线探针闭合 + 探针红即打回条款在，处置得当。deny-main-push.mjs:503 `input?.tool_input?.command` 同构读取先例属实（判官亲读该行）。
- 硬项 2a（目标文件锚清单）：通过。T1 末锚清单两件 + T2 末锚清单一件，landing-manifest 围栏块三路径 ≡ 并集 ✓；packet-id 行在场 ✓。hook 源码与 settings.json 注册指令逐字钉死，且判官已把 PROPOSAL 内钉死源码逐字节抽出实跑（见下），可 landing 性经实测背书。
- 硬项 1b（sweep 透镜）：承重断言通过，计数声明有缺口（见 F1）。「dispatch-log 不记 model 参数」诚实声明属实——判官 grep `.claude/taskpackages/260903-gateway-directionB/dispatch-log.jsonl` 得 0 条 model 字段命中，与样例（actor/phase/tokens 形态）吻合。
- 硬项 1c：纯新增 + 追加句，不适用，声明正确。

**T1 技术合理性抽查**：hook 判定逻辑与 15 条自检用例表逐一过、自洽（含 `model: 123` 数字形态走 `typeof !== 'string'` 放行、`' claude-opus-4-8 '` 空白形态 trim 后 deny 的两侧边界）。判官另做了预 landing 实跑：从 PROPOSAL 围栏块逐字节抽取钉死源码至临时副本，`--self-check` 输出 `{"selfCheck":"agent-model-guard","pass":15,"fail":0,"total":15,"ok":true}` exit 0；三形态 stdin 探针（architect+opus → exit 2 + stderr `agent-model-guard:` / architect 无 model → exit 0 / builder+opus → exit 0）全部与 §5 V1/V2 预期逐项一致；逃生门 env `KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1` 实跑放行 + advisory 输出正确。matcher `Agent|Task` 双形态设计与「多余 matcher 不匹配时无行为」论证成立；settings.json 插入位描述与文件实况吻合（判官亲核现行 55–63 行恰为 `Edit|Write|MultiEdit` matcher 块，落位后 PreToolUse 三块计数成立）；`_doc` 追加行形态合法（见 F2 的衔接缺口）。

**T2 技术合理性抽查**：追加句与 planner.md:57 原文及 10b 段语义一致、不夹带新设计 ✓；G2 句内指称 agent-model-guard 与 hook 注释/文案指称 planner.md:71/57 构成互指，二者同一 packet 落地、无单边退役窗口，无循环依赖问题 ✓。route-regression-check `--self-check` 入口与 lint-descriptions 入口均亲核存在（`.claude/scripts/` ls + grep），T2 不触 description/frontmatter，V3 绿预期合理。

**§5 验证集**：V1–V6 命令入口全部真实（判官亲核脚本存在与 landing-fidelity-check.mjs argv[2]=reviewDir 签名）；V1/V2 判官已实测背书；V5 在线探针设计足以闭合 1a 未实测项（deny 生效 + 不带 model 重派正常起跑两腿都有可机判预期，且「探针红不得以 V1–V4 绿抵充」条款在）；真空绿规约遵守（V1 先断言输出非空含自检名再判 ok）。

**§6 留根清单**：对照 ARQ 行（旁支 ③ AInativecompany 漂移、dispatch-log 无 model 字段）与 §1 全部证据链（模型档位命名混淆、1a 未实测项由 V5 承接），无漏关的根。

**Findings**：

- **F1 · NIT**：§3 硬项 1b census 计数不可机械复跑。「最近 3 个项目全部近期会话」窗口未钉死（无会话文件清单 / 时间界），判官独立枚举（json.loads 逐行扫 assistant 的 Agent tool_use）得 architect 派发远不止 5 次（KidPal 主项目近 7 个含命中会话共 14 次、AInativecompany 4 次、directionB 1 次，全部不带 model 除证据一那次）——「不带 4 次」按任何自然窗口解释都对不上。**承重断言无恙**：判官独立枚举证实全量命中集 = 恰 1 次（即证据一），「本闸若已在线，命中集 = 恰那 1 次、无误伤」成立。修订要求：把 census 窗口钉死（列枚举的会话文件清单或明确时间界），或把计数改为「≥N 次、带覆盖 1 次」的下界声明；不改不影响 GREEN（依据：模板硬项 1b「注明命令、机械枚举生成」+ 本 R1 亲核枚举输出）。
- **F2 · NIT**：T1 `_doc` 追加指令缺 JSON 衔接句。现行 `_doc` 末行（settings.json:8 `"permissions.allow 只收…"`）无尾逗号，机械追加钉死行会产出非法 JSON；指令说「数组末尾追加一行（逐字）」但未写「现行末行须补逗号」。V4（JSON.parse + matcher 计数）可机械兜住，故不 BLOCK。修订要求：注册指令补一句「现行 `_doc` 末行补逗号后再追加本行」（依据：`.claude/settings.json:3-9` 实况 + 硬项 2a「逐字钉死到可 landing」）。
- **F3 · NIT（观察级）**：逃生门「agent 无法自开」论证成立（子进程改不了父进程 env，OS 语义），但其完备性隐式依赖另一前提——agent 写不了 `.claude/settings*.json`（scope-map 各角色 allow 面均不含 `.claude/**`，universal bashDenyWrite 拦 `.claude/**` 重定向写）。依赖关系成立但未成文；将来若放宽 `.claude` 写面，逃生门随之敞开而无人知情。修订建议（不强制）：在 `_doc` 登记行或落地后 retro 里补一句此依赖关系（依据：`.claude/scope-map.json` universal.bashDenyWrite 含 `.claude/**`、architect deny 含 `.claude/settings.json`、各 dev-loop 角色 allow 面判官已亲读）。

**升级 human 判定**：无 BLOCK 项；F1–F3 均不触「影响目标效果」线（目标效果的承重断言——命中集恰 1 次、V1/V2 探针实测过、三要件齐——全部亲核为真）。不升级。
