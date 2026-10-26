# PROPOSAL · 261024-gov-review-retro-efficiency-instrumentation

> **状态：FROZEN v1.0 · 2026-10-26 · human 立项 ack（OQ1=(a) 零新增记录；OQ2=二分域规则；OQ3=多模型去重后 `+` 连接；OQ4=(b) 测试行进表）+ architect R9 GREEN（Track B 主路径，riskTier medium）**（凭据行：human 2026-10-26 四项拍板 + R9 verdict GREEN。）
> **承载 ARQ**：无在册 ARQ 直承。输入 = human 2026-10-25 原话（只留四个数：代码行 / token 与时间 / 当时模型 / 文档代码比）+ human 2026-10-26 追加（按 Agent 角色分别花了多少时间与 token；数据规范化）与第一步基线报告 `delivery/retro/2026-10-24-pod-efficiency-baseline.md`。`ARQ-261014-pod-events-booking-telemetry-fidelity` **部分消费**（读侧 actor 值域分类 + 写入侧字段在场约束与告警，见 T3），其根因修复保持 deferred，本批不做 landing 期追加动作（见 §6 第 6 条）。
> **review 轨**：Track B 主路径（触 `.claude/scripts/**` 与 `.claude/commands/**` 命中轻路径排除清单，**非**瘦制品轻路径；SSOT：`.claude/references/gov-change.md`「轻路径判据」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（T3 触 `.claude/scripts/dispatch-stamp.mjs`——每次派发的写入路径；全线 fail-open、零 schema 破坏、零门语义、零 agent 定义、G1 与契约面零触。写入面风险较仅只读版本升一档，向严取 medium，不取 low-medium）。
> **review profile**：G2+G3（`.claude/commands/**`、`.claude/references/**`、`.claude/scripts/**`）。G1 触面为零、契约面为零。
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/commands/loop-retro.md
.claude/references/dev-loop-operating-system.md
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/retro-efficiency.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：周期复盘时能一眼看到每个 Pod 的四个数——**① 写了多少行代码、② 花了多少 token 与多少时间、③ 当时用的是哪个模型、④ 文档与代码的比例**——再附一张**⑤ 按 Agent 角色汇总的时间与 token 表**（builder / acceptance-author / integrator / fixer / architect / planner 等分别花了多少）；数字全部从现有 `pod-events.jsonl` 与 git 历史直接复算，不引入归因框架、不新增每 Pod 记录。

**现状差距**（2026-10-25 起草期本 worktree 现跑，命令与数值见 §2 开头）：

- token、agent 时长、生效模型与事件时间本来就记在 `pod-events.jsonl` 的 dispatch 事件里；行数也能从每个 Pod 的 merge commit 相对第一父的 `git diff --numstat` 算出来——**四类数据都已存在**，缺的只是一个仓内的只读聚合器与一张固定口径的表。
- 第一步报告用的统计脚本住在仓外一次性目录（硬编码仓库路径、结果写回 `c:/tmp`），换机器或换 worktree 就得到另一套数；而且它按「模型族」归并、按五个路径桶统计，比现在需要的口径复杂得多。
- 现有数据的真实缺口（分母 = `delivery/taskpackages/` 下含 `taskpackage.json` 或 `pod-events.jsonl` 的目录，`current/` 除外，共 **92** 个）：**17 个归因不到 merge commit**（行数算不出）、**24 个没有 exact token**、**44 个没有任何 dispatch 记过模型**。这些拿不出来的格子按空值处理，不补 0、不推测。
- **planner 是主线程、不走派发**：38 条自记 stamp 全部没有 exact token；主线程成本只能从 `payload.mainThreadUsage` 的累计检查点做同 Pod 首末差——48 个 Pod 有检查点、46 个有 ≥2 个，其中只有 **34 个首末差为正、可算**（12 个为 0 或负，属会话重置/截断型失效）。

**非目标（本批不做，human 2026-10-25 明确不要）**：不建能力纪元台账、不做按纪元的跨期比较与分层归因；不记门开火（verifier 红轮 / supervisor 裁决 / 契约闸 / mutation 发现）；不做合并后逃逸归因；不补主线程与 nano 的用量检查点；不统计验收断言数；不做事件类型与 phase 收敛；不做四桶路径分类或任何多维仪表盘/散点/归因结论。数据规范化只作用于**未来写入侧 + 分析侧映射**，不追溯重写历史事件。原 v0.2 草案里为这些服务的 T 条目、字段、脚本与验证集已整体撤下（处置见文末「起草注记」）。

## 2. 问题、根因与改法（T1…T3）

**【硬项 · 起草期实证义务履行声明】** 本节行为断言均附 2026-10-25 起草期实测（本 worktree `gov/kidpal`）：

- **可复算性原型**：用与 T1 规格同口径的一次性脚本（`node -e`，遍历 `delivery/taskpackages/*/pod-events.jsonl` + `git log --merges` + `git diff --numstat`）全量跑通 92 个包目录——**75 个可归因到 merge commit、17 个不可**；68 个有 exact token、24 个没有；48 个有模型记录、44 个没有。其中 5 个包只有 taskpackage 无 pod-events（`260829-audio-bargein-hw` / `260829-shim-skeleton` / `260831-cloud-link-bringup` / `260831-simulator-lvgl-align` / `261024-qa-session-timeout-producer`），1 个目录只有事件无 taskpackage（`260920-gov-review-dispatch-telemetry-source`）——两类都按同一目录口径进表，缺的列留空。
- **逐 Pod 抽样**（2026-10-26 human 拍定的二分域规则，规则全文见 T1）：`261023-honcho-memory-wiring-restoration` = 代码 2,109 行 / 文档 1,998 行 / 其中测试 1,336 行（比值 0.95）；`261022-m5-k4-history-corpus-backfill` = 2,489 / 1,784 / 1,158（0.72）；`260829-story-p3` = 5,194 / 1,848 / 3,246（0.36，代码行含 14 行域外路径）。
- **新分类规则实测**（75 个可归因包全量复算，与 T1 成文规则同源）：中位文档代码比 **1.29**；治理域的数据/配置文件（taskpackage.json、pod-events.jsonl、verify-gates.json、`delivery/**` 下的 `.txt` 收口清单等）计入文档行，业务域的数据/配置文件（含 `server/deploy/**`、`firmware/**/test/**`）计入代码行；**域外路径先按类型规则判定**（`.planning/**` 的 markdown 仍算文档），类型规则未覆盖的默认按代码侧计——`outsideKnownRoots`（不属于三个已知域根的路径）共 9 个文件、198 行，仅作 `--json` 诊断计数。
- **规则为何要换**（旧口径的两个失真，human 2026-10-26 据此拍定新规则）：旧口径把 JSON/YAML 一律算代码，机器工件占「代码行」中位 **41.3%**（极值 13%–96.6%）；同时 `delivery/**` 治理文书占「文档行」中位 **92.8%**（honcho 1,260 文档行中 1,213 行来自 `delivery/`）。
- 现有事件结构：dispatch 事件携带 `payload.tokenUsage` / `durationMs` / `model` / `actor`（`.claude/scripts/dispatch-stamp.mjs:303-322`），事件行带 `timestamp`；主线程累计值在 `payload.mainThreadUsage`（`source: 'transcript'`，`.claude/scripts/dispatch-stamp.mjs:286-292`）——四类 Pod 指标与角色汇总的数据面都已在场。
- **角色汇总原型**（同一口径全量复算，92 个包目录）：派发角色 exact token 合计 59,402,388、时长合计 366,932,390 ms（6,116 分钟）。token 占比（分母含 planner）= builder 29.7% / acceptance-author 29.2% / **planner 16.7%** / integrator 15.4% / fixer 6.9% / architect 2.0% / landing-verifier 0.1% / architect-reviewer 0.05%；时长占比（分母只含派发行）= builder 39.9% / acceptance-author 35.5% / integrator 15.2% / fixer 8.1% / architect 1.2%。planner 38 条自记 stamp 无 exact token；可算 Pod 34 个，合计 11,867,898 token；同批主线程时段（墙钟 − Σ派发时长）= 91.2 小时（含人等）。verifier 4 次派遣、supervisor 无派遣，均无 token/时长数据。
- **数据质量普查**（同一遍历，2026-10-26 起草期快照）：1,133 条事件 / 457 条 dispatch 中，199 条缺 `model`、57 条缺 `durationMs`（另 15 条为 0）、57 条缺 `tokenUsage`（11 条 unavailable、4 条 `exact` 但 `total` 为 0 或缺失）、11 条事件缺时间戳；模型 **11 个具名取值 + 199 条无记录（合计 12 种）**；48 个 Pod 有主线程检查点（2 个单点、7 个首末相同、6 个出现相邻倒退，可做差 34 个）——T3 据此立项。事件总数为起草期快照，R6 判官复算为 1,135（多出 2 条非 dispatch 事件，全部派生计数不变）；landing 前以现跑为准。
- **preflight 实跑结果**见文末「起草注记」。
- 本批不新增任何记录或 schema；脚本行为属设计规格，不是对现状的断言。

**【硬项 · 目标文件锚清单】** 每个 T 末尾显式列「目标文件：」行，本批并集 4 个路径，与头部 landing-manifest 逐行一致；缺锚清单 = 门 RED 点名。

### T1 · 只读效率表脚本与固定口径

**问题**：四个数散在三处——token / 时长 / 模型在 pod-events、行数在 git 历史、时间在事件时间戳——没有一处能在仓内一键复算；第一步的统计脚本是仓外一次性件，口径还比需要的复杂。

**证据**：
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:126`：「脚本和四桶分类目前不在仓内；第二步落地时应把它们正式收进 `.claude/scripts/`，否则下次复算会得到另一套数字」。
- 第一步脚本 `C:/tmp/retro-metrics/collect.mjs:10`（硬编码仓库路径）、`:175`（写回 `c:/tmp`）、`:21-32`（五桶分类）、`:123-126`（墙钟 = 首末事件差）。
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:14`：69/87 能归因到合并提交、18 个不能——行数确实存在归因缺口；本次复算为 75/92 可归因、17 不可（样本集与时点不同，缺口性质一致）。
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:22`：验收断言数、起止时间等列当年留空——本批只取能从现有数据算出的四项，其余不碰。

**根因**：缺一个仓内的只读聚合器与一份固定口径。四类数据本身都已存在（token / duration / model 在 dispatch 事件，行数在 git，时间在事件时间戳），没有任何一项必须靠新增记录才能拿到——第一步之所以补不出行数，是因为它只依赖「按 commit message 反推」这一条路；而把归因算法搬进仓内、对归因不到的 Pod 如实留空，就能用现有数据把表跑出来。

**改法**：新增只读脚本 `.claude/scripts/retro-efficiency.mjs`（无写盘副作用，stdout 输出）：

- **参数与行集**：`--since <YYMMDD|YYYY-MM-DD>` / `--until <…>`（**过滤键 = changeId 前 6 位日期前缀**，与 `/loop-retro` 游标的 change-id 周期语义一致；事件 `timestamp` 只用于墙钟计算，不作过滤键）/ `--change-id <id>`（单 Pod 调试用）/ `--json` / `--self-check`。缺省输出 markdown 表。**行集** = `delivery/taskpackages/` 下含 `taskpackage.json` 或 `pod-events.jsonl` 的目录（`current/` 除外；当前 92 个）——只有 taskpackage 的包照常进表、token / 时间 / 模型列留空；只有事件的目录也进表。
- **口径（写进脚本头部注释与表注，单一权威在脚本）**：
  - **token** = 该 Pod 全部 `dispatch-completed` 事件中 `payload.tokenUsage.status === 'exact'` 的 `total` 之和（`total` = input + output，cache 两路不并入）；单元格显示 `总量 (有数据 dispatch 数/总 dispatch 数)`，`unavailable` 绝不计 0。
  - **agent 分钟** = 同批事件 `payload.durationMs` 之和 ÷ 60000（缺 `durationMs` 的 dispatch 不计，覆盖数进 `--json`）。
  - **墙钟分钟** = 末条事件 `timestamp` − 首条 `timestamp`（表注写明：事件稀疏时低估，属观测近似，不补记录）。
  - **模型** = 该 Pod `dispatch-completed` 的 `payload.model` 去重集合（多个用 `+` 连接；一条都没有则留空）。只记录、不归因、不映射、不按模型分层。
  - **第一张表：Pod 表**（列固定为 `changeId / 模型 / 代码行 / 其中测试行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟`）：文档代码比的分母 = **代码行（实现 + 测试）**，表注写明；`--json` 另给 `testLines` 与 `ratioDocImpl`（文档行 ÷ 实现行）。
  - **代码 / 文档分类（human 2026-10-26 拍定的二分域规则）**：先按文件类型、再按消费域判定——
    1. **源码**（`.c` `.cc` `.cpp` `.h` `.hpp` `.py` `.mjs` `.js` `.ts` `.tsx` `.sh` `.ps1` `.s` `.ld`）恒算**代码**，不论落在哪个域（包括 `.claude/scripts/**` 的 `.mjs`）；
    2. **Markdown / README**（`*.md` `*.mdx` `README*`）恒算**文档**，不论落在哪个域；
    3. **数据 / 配置**（`*.json` `*.jsonl` `*.yaml` `*.yml` `*.toml` `*.ini` `*.conf` `*.cfg` `*.env`）按**消费域**二分：**治理域 → 文档；业务域 → 代码**。不存在「deploy 所以特殊」「test 所以特殊」——`server/deploy/**` 的数据/配置在业务域起作用 → 代码，`firmware/**/test/**` 的数据/配置也在业务域 → 代码；
    4. **二进制 / 资产**（`*.bin` `*.png` `*.jpg` `*.ico` `*.bmp` `*.ttf` `*.otf` `*.zip` `*.gz` `*.zst` `*.pdf` `*.hex`）同样二分：业务域 → 代码、治理域 → 文档；二进制在 `git diff --numstat` 里本就是 `-`（计 0 行），分类只影响文件计数；
    5. **其余类型**按域二分：业务域 → 代码；治理域 / 产品文档域 → 文档；**已知域之外的路径**（如 `.codebase-memory/**`、`.planning/**`、根 dotfiles）先按类型规则判定，类型规则未覆盖的**默认按业务/工具侧算代码**——默认归类是确定的，不留空、不等人拍板；`--json` 另给 `outsideKnownRoots` 诊断计数（不属于三个已知域根的行数）。
    - **域定义**：治理域 = `delivery/**`、`.claude/**`、`docs/debt/**`、`docs/intake/**`、`docs/templates/**`、`docs/archive/**`（+ 历史遗留治理证据树 `changes/**`）；业务域 = `firmware/**`、`server/**`、`tools/**`；产品文档域 = `docs/**` 其余部分。
  - **规则应用示例（机械判定结果，不是复核事项）**：`.claude/scope-map.json` / `verify-gates.json` / `settings.json` → 文档；`server/deploy/manifest.yaml` → 代码；`firmware/**/test/*.json` → 代码；`delivery/**/*.json` → 文档；`docs/debt/**` 的数据文件 → 文档；`.claude/scripts/*.mjs` → 代码；`AGENTS.md` → 文档；`.codebase-memory/artifact.json`、根 `.gitignore` → 代码（并计入 `outsideKnownRoots` 诊断计数）；`.planning/**/*.md` → 文档（Markdown 规则优先，同样计入 `outsideKnownRoots`）。
  - **测试行**（`tests?/`（含单数 `test/`）、`__tests__/`、`*.test.*`、`*.spec.*`、`*_test.*`、`test_*.py`、`*.acceptance.*`）从代码行中单列到「其中测试行」列，同时保留在 `--json.testLines`；比值分母仍用代码行（实现 + 测试）。
  - **文档代码比** = 文档行 ÷ 代码行（代码行 = 0 时留空，不写 `Infinity`）。
- **第二张表：按角色汇总**（列固定为 `角色 / 派发次数 / exact token / agent 分钟 / token 占比 / 时长占比 / 覆盖（有数据 Pod 数）`）：
  - 角色取 `dispatch-completed.actor`；至少列 builder / acceptance-author / integrator / fixer / architect / architect-reviewer，观测到的其它 actor（如 landing-verifier）照列；每 Pod 的角色明细只进 `--json` 的 `rolesByPod`，不铺成宽表。
  - token 只求和 `exact`（与主表同口径）；`agent 分钟` = Σ`durationMs` ÷ 60000；覆盖列写作 `token x / 时长 y`（该角色有 exact token 的 Pod 数 / 有 `durationMs` 的 Pod 数）。
  - **占比分母**：token 占比的分母 = 本表所有**有 exact 值**的行合计（含 planner）；时长占比的分母 = 所有**有 `durationMs` 的派发行**合计（planner 时段与脚本行不并入）。表注写明分母，不拿缺数据的角色凑分母。
  - **planner（主线程）单列一行**：planner 不走 `Agent()` 派发，其 token 只能从 `payload.mainThreadUsage`（`source: 'transcript'`）的累计检查点取**同 Pod 首末差**；**只有 ≥2 个检查点且差值 > 0 的 Pod 可算**，差值 ≤0 视为失效（会话重置/截断），留空并计入排除数，不写 0。时间列报「**主线程时段** = 墙钟 − Σ 派发 `durationMs`」（在可算 Pod 上求和），**显式标注它包含人等与串行等待、不是 planner 的纯工作时长**；该值不并入时长占比，占比格留空并注原因。
  - **脚本驱动角色**（verifier / supervisor）：supervisor 是机械闸、无 dispatch；verifier 历史 4 次派遣也无 token/时长数据。二者合并单列一行标「脚本驱动，不计入 agent 成本」，成本格留空，不把 0 混进合计与分母。
- **归因算法**：`git log --all --merges --grep=<changeId>` 起手，命中「subject 以 `Merge <changeId>` 开头 / 含 `merge: dev/<changeId>`」或有触碰 `delivery/taskpackages/<changeId>/` 的 merge 才采用；差量取 first-parent `git diff --numstat`，一个 Pod 有多个 merge 时逐文件取 max 后求和（沿用第一步脚本算法，搬进仓内单源）。**归因不到 → 代码行 / 其中测试行 / 文档行 / 比值全部留空**，`--json` 给 `unattributedReason`，不写 0。
- **fail-open**：畸形事件行跳过并在 stderr 计数；git 命令失败只让对应 Pod 行数留空，脚本仍以 exit 0 输出其余行。
- **`--self-check`**（临时目录夹具，真空绿规约：先断言夹具非空）：① 分类器方向性——把 doc/code 判定反转必须变红；② token 只求和 `exact`——夹具里的 `unavailable` 条目**带一个可识别的非零 `total`（如 999999）**，断言和仍只等于 exact 之和（否则错实现把 unavailable 一并求和时断言恒真、真空绿）；③ 无 merge 的 Pod 行数留空、不是 0；④ 模型去重与 `+` 连接；⑤ 文档代码比公式（含代码 0 → 空）；⑥ **零写盘**——运行前后对夹具目录做**内容级**快照（递归枚举、稳定排序、每文件记录 `path:size` 与内容 sha256），断言快照完全一致——既抓新建文件，也抓对既有夹具文件的原地覆写（如误 append 到夹具 `pod-events.jsonl` 这类清单不变但内容变的情况）；⑦ **角色汇总**——夹具里造 builder / integrator 两条 exact 与一条 `unavailable`，断言 builder 行 token 只等于其 exact 之和、占比分母为全表 exact 行合计；⑧ **planner 检查点**——造一个 `mainThreadUsage` 首末差为正的夹具（计入）与一个差为负的夹具（不计入、计为排除），断言负差 Pod 不写 0、不计入合计；⑨ **`model-family-map`**（同一族的多种写法归一到同一族、未知值落 `unmappedModels`）；⑩ **`actor-classification`**（`main-thread` → planner、`merge-driver` → script、`human` 不进角色表、未知落 `unmappedActors`）；⑪ **`consumer-domain-classification`**（治理域 JSON → 文档、业务域 JSON → 代码，含 `server/deploy/**` 与 `firmware/**/test/**` 两个业务域样例；Markdown 恒文档、源码恒代码）；⑫ **`outside-roots-default-code`**（域外且类型规则未覆盖的路径默认归代码并计入 `outsideKnownRoots`；所有路径都有确定归类，无「未决」桶）；⑬ **`test-column`**（测试行同时出现在「其中测试行」列与 `--json.testLines`，比值分母含测试）；⑭ **`governance-data-as-doc`**（把一条 `.claude/*.json` 的判定反转成「代码」时断言必红）。

目标文件：`.claude/scripts/retro-efficiency.mjs`（新增）

### T2 · `/loop-retro` 输出接一张效率表

**问题**：周期复盘的输出契约里没有效率轴。`/loop-retro` 现在只产出 top findings、升级主题、分流、advisory 与 token coverage，四个数一个都不在视野里；即使 T1 的脚本存在，没人知道该在复盘时跑它。

**证据**：`.claude/commands/loop-retro.md:26-44`（步骤 1–6 的输出清单无效率表）；`.claude/references/dev-loop-operating-system.md:145`：「New-format 用 on-demand aggregation 提取 token coverage、escalation 与 friction themes」——同样没有产出轴。

**根因**：retro 命令自建立起只面向「摩擦与流程债」一条轴；新增效率视角需要显式写进命令的执行步骤与输出清单，否则脚本存在也不会被消费。

**改法**：

- `.claude/commands/loop-retro.md`：步骤 2 增一句「另跑 `node .claude/scripts/retro-efficiency.mjs --since <周期起点>` 取本周期效率表」；步骤 5 的输出清单增「**效率表两张**：① 九列 Pod 表（含「其中测试行」列）；② 按 Agent 角色汇总表（角色 / 派发次数 / exact token / agent 分钟 / token 占比 / 时长占比 / 覆盖）——原样贴出脚本输出 + 一句数据缺口说明（归因不到 / 无 exact token / 无模型记录 / planner 检查点不足 / `outsideKnownRoots` 行数）」。
- `.claude/references/dev-loop-operating-system.md` §6 增一句：new-format 复盘可跑 `retro-efficiency.mjs` 取四指标 Pod 表与角色汇总表，**口径以脚本为唯一权威**、本节不复述列定义；游标、触发条款、分流纪律不动。
- 第一张表九列——`changeId / 模型 / 代码行 / 其中测试行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟`；第二张表七列——`角色 / 派发次数 / exact token / agent 分钟 / token 占比 / 时长占比 / 覆盖`；planner 行的时间列带「主线程时段（含人等）」标签。不做多维仪表盘、不排序结论、不做归因。

目标文件：`.claude/commands/loop-retro.md`（改） · `.claude/references/dev-loop-operating-system.md`（改）

### T3 · 支撑效率表的数据规范化（写入侧 + 分析侧）

**问题**：两张表要的三列在写入面有系统性缺口与漂移，不规范化就既缺又碎、还会静默低估。全量普查（2026-10-26 起草期快照，87 个有事件目录、1,133 条事件、457 条 dispatch；R6 判官复算事件数为 1,135、派生计数不变）：**199 条 dispatch 没有 `model`**；**57 条没有 `durationMs`（另 15 条为 0）**；**57 条没有 `tokenUsage`（11 条 unavailable、4 条 `exact` 但 `total` 为 0 或缺失）**；**11 条事件没有时间戳**。模型 **11 个具名取值 + 199 条无记录（合计 12 种）**，同一模型族并存 3 种写法（如 `glm-5-3-flash` / `glm-5-3-flash-260828` / `glm-5.3`）；主线程检查点方面，48 个 Pod 有检查点、2 个只有一个点，46 个可做差的 Pod 中 **7 个首末完全相同、6 个出现相邻倒退**。角色值域在 dispatch 层基本干净，但全事件面还有 `main-thread`（1）、`human-deputy`（2）、`merge-driver`（70）等非 agent 值——直接汇总会把脚本与人类混进角色成本。

**证据**：
- 上述普查为「`node -e` 遍历 `delivery/taskpackages/*/pod-events.jsonl`」现跑（2026-10-26，本 worktree）；与主线程普查的 457 dispatch / 199 缺 model / 57 缺 duration / 57 缺 tokenUsage / 11 缺时间戳一致；**负增量一处差异**：按「首末差 <0」计为 5 个、按「出现任一相邻倒退」计为 6 个（主线程口径），本批两种都记、判据分开写。
- `.claude/scripts/dispatch-stamp.mjs:303-322`：payload 组装里 model 只在有值时写入（`...(model ? { model } : {})`），缺值即键缺失、无法与「没这条字段」区分。
- 同件 `:200`：`durationMs` 是必填旗标，但调用方可以传 0，没有「真实零」与「取不到」的区分；`:264-274` transcript 路径的 token 与 `:290-292` 主线程检查点均无零值/单调性注记。
- 时间戳：`pod-events-append.mjs:70-73` 与 `merge-driver.mjs:1056` 都自动写 `timestamp`；11 条缺失来自早期手工直写，属历史异常。
- 同根：`docs/debt/gov/ARQ-261014-pod-events-booking-telemetry-fidelity.md` 的修法候选 ②（actor 归属）与 ⑥（模型/用量数据失真、mainThreadUsage 快照不可信）——本批**消费这两个子项**，不另造账。

**根因**：写入面把「有就写、没有就省略」当默认，缺值不可区分于「字段不存在」；模型名是代理层透传原值，仓内没有族映射；主线程检查点的「同会话单调累计」语义从未成文，相同/倒退快照没有告警；角色值域没有 agent / 脚本 / 人类三分。历史直写绕过了 append 的默认时间戳。

**改法**（未来写入侧 + 分析侧映射；不追溯重写历史事件）：

- **写入侧 `.claude/scripts/dispatch-stamp.mjs`**（全部 fail-open，绝不阻断派发）：
  - **model 必在场**：payload 恒含 `model` 键（字符串或 `null`）；transcript 未命中且无 `--model` → `model: null` + `modelStatus: 'unavailable'` + `modelReason`；命中 → 字符串 + `modelStatus: 'exact'`。transcript 生效模型与 `--model` 旗标不一致时 stderr 告警并记 `modelFlagMismatch: true`（消费 ARQ-261014 ⑥ 的「声明 ≠ 实际」对账面）。
  - **durationMs 必在场**：`--duration-ms` 接受非负整数或字面量 `unavailable`；不可得 → `durationMs: null` + `durationStatus: 'unavailable'` + `durationReason`；传 0 → 写 0 并带 `durationNote: 'reported-zero'`（真实零与缺失可区分，沿用 token 的「绝不写 0 充数」政策）。
  - **tokenUsage 保持必在场**：现有路径已恒写；补一条 transcript 路径下 `exact` 且 `total === 0` 时同样打 `note: 'reported-zero'`，并保证 `exact` 必有 `total` 且 `total = input + output`。
  - **主线程检查点语义 + 告警**：写入前读取本包既有 pod-events 的最后一条 `mainThreadUsage` 累计值；与本条相同或更小 → stderr 告警 + `cumulativeNote: 'non-increasing'`（fail-open，不改事件类型、不阻断）。OS §5d 同步钉死「同一会话内单调累计、每次 dispatch 刷新」。
- **分析侧 `.claude/scripts/retro-efficiency.mjs`**：
  - **模型族映射表**（脚本内单源、可扩展）：原始值 → 族（`glm-5.3` / `deepseek-v4.1` / `deepseek-v4` / `deepseek-flash` / `kimi-k3` 等）；表中显示族名，`--json` 保留原始值并列出 `unmappedModels`（未登记的新名字落此，不静默吞）。
  - **角色三分类**：agent（builder / acceptance-author / integrator / fixer / architect / architect-reviewer / landing-verifier / planner）/ script（verifier / supervisor / merge-driver）/ human（human / human-deputy）；legacy 别名 `main-thread` → planner，`merge-driver` → script（不计 agent 成本）；未知值落 `unmappedActors` 并列出。
  - **覆盖标记**：`--json` 输出 model / duration / token 的缺失计数、exact 零值计数、缺失时间戳计数；表内 token 与时长单元格带覆盖。
  - **检查点有效性**：首末差 ≤0 的 Pod 不做差并计排除；`--json` 区分 `identical`（首末相同）与 `decreasing`（出现相邻倒退）。
- **文档 `.claude/references/dev-loop-operating-system.md` §5d**：dispatch payload 必带 `model` / `durationMs` / `tokenUsage` 三键与 `unavailable + reason` 语义；`mainThreadUsage` 同会话单调累计；一切缺值告警 fail-open、不阻断派发。
- **不纳入本批**：`eventType` 51 种自造值与 `phase` 155 种写法（两张表只消费 `dispatch-completed`，枚举收敛留 §6）；历史 11 条缺时间戳的事件不追溯补造，只在覆盖标记里如实计数。
- **与 ARQ-261014 的承接**：本批消费其六类实据的 **②（actor 归属误落——读侧值域三分类 + legacy 别名）** 与 **⑥（模型 / 用量数据失真——字段在场、族映射、检查点失真告警）**，其中模型「声明 ≠ 实际」对账对应其修法候选 ③；**留给该 ARQ 的**：实据 ② 的写侧归属纠错（source/actor 混用）、实据 ③（同决定双记）、实据 ④（`refEventId` 单写）、实据 ⑤（时间戳倒挂），以及 ⑥ 的快照刷新机制根因修复（7 相同 / 6 相邻倒退的实际成因）——本批只做告警与剔除。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改） · `.claude/scripts/retro-efficiency.mjs`（改） · `.claude/references/dev-loop-operating-system.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 commands | `.claude/commands/loop-retro.md` | 改（T2） | 步骤 2 跑表、步骤 5 输出增两张效率表 |
| G2 references | `.claude/references/dev-loop-operating-system.md` | 改（T2/T3） | §6 一句指向脚本；§5d 补 dispatch 三键必在场、unavailable 语义与检查点单调累计语义 |
| G3 scripts | `.claude/scripts/retro-efficiency.mjs` | 新增（T1/T3） | 只读聚合器（九列 Pod 表 + 角色表 + 消费者分域分类 + 模型族映射 + 覆盖标记），stdout 输出，零写盘 |
| G3 scripts | `.claude/scripts/dispatch-stamp.mjs` | 改（T3） | 派发写入面：model / durationMs / tokenUsage 必在场、缺值显式 unavailable、检查点非单调告警；全 fail-open、不阻断派发 |
| 承重未触 | `.claude/scripts/pod-events-append.mjs`、`pod-events-aggregate.mjs`、`merge-driver.mjs` | 未触 | **零新增记录、零 schema 变更**：四类 Pod 指标与角色汇总全部从现有事件与 git 直接算；append 的默认时间戳已足够，历史缺时间戳行不追溯 |
| 承重未触 | `.claude/agents/planner.md`、`integrator.md` 及全部 agent 定义 | 未触 | 无新派发点、无新步骤、无新写权 |
| 承重未触 | `closeout-check.mjs`、`supervisor-verdict-gate.mjs`、`pre-merge-escalation-gate.mjs`、`supervisor-gate-check.mjs`、`.claude/hooks/verify.mjs` | 未触 | 门判据与合并链零改动；不新增工程门、不加任何牙 |
| 承重未触 | `.claude/scope-map.json`、`.claude/*.settings.json`、`.claude/verify-gates.json`、`.claude/hooks/**` | 未触 | 写域零扩张 |
| 未触 | `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` | 未触 | 契约面与 spec 零触 |
| 历史制品 | `delivery/taskpackages/**`、`delivery/retro/**` | 未触 | append-only；不回填历史、不补造数据 |

**【硬项 · sweep 透镜义务】** 本批**不新增门、断言或记录 schema**——T3 的写入侧规范化是字段在场约束与 fail-open 告警，不拦截任何派发。新增的判定 pattern 有两类：① 路径按二分域归入代码 / 文档（域外默认代码侧，另有 `outsideKnownRoots` 诊断计数）；② dispatch 字段的在场与可用性（`model` / `durationMs` / `tokenUsage`）及检查点单调性。起草期已用与规格同口径的原型脚本对 92 个包目录全量预跑两类 census（命令与计数见 §2 开头与 T3 证据段），判官可 spot-check。

**【硬项 · 复扫模式完备性先验】** 本批不退役、不改写任何既有术语或枚举（不改事件类型、不改 phase、不改桶名），无复扫对象，本节不适用；原 v0.2 草案里的「事件类型闭集」「phase 收敛」等条目已整体撤下，未进入任何 canonical 文本，故无需清扫。

## 4. human 取舍点（OQ）

> 四项均已于 2026-10-26 由 human 拍定，以下为裁定记录；规则本体与规则应用示例见 T1。

**OQ1 · 是否为零新增记录**：**human 2026-10-26 已裁 (a)**——完全不加每 Pod 记录，行数从 merge commit 归因，归因不到留空。保留理由：四类指标用现有数据已能全量跑通；(b) 会重新引入生产点、事件类型与 schema 约束，正是本批要砍掉的复杂度。若真实复盘里留空率成为障碍，再单独立 (b)。

**OQ2 · 代码 / 文档分类口径**：**human 2026-10-26 已裁：二分域规则**，不再是开放选项。原话（纠正版）：「你就发现它是在 Governance 域起作用的，它就是文档；它要是在业务系统起作用的，它就是代码。你不管它是 deploy 的还是 test 的，只要在业务域起作用，那它就是代码。」规则全文（源码恒代码 / Markdown 恒文档 / 数据与配置、二进制按域二分 / 域外默认代码侧）与规则应用示例见 T1；**不存在需要 human 逐条复核的边界项**——分类由脚本机械执行，人不需要逐条看。

**OQ3 · 一个 Pod 出现多个模型时怎么记**：**human 2026-10-26 已定**——去重后 `+` 连接（保持现状）。只记录事实、不做选择，表宽可控且能看出「中途换过模型」。

**OQ4 · 测试行要不要占表列**：**human 2026-10-26 已裁 (b)**——Pod 表加「其中测试行」列（现为九列），测试行同时保留在 `--json.testLines`；文档代码比的分母仍用「实现 + 测试」，表注写明；`--json` 另给 `ratioDocImpl`（文档行 ÷ 实现行）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **（pre-commit，起草期已跑）** `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261024-gov-review-retro-efficiency-instrumentation/PROPOSAL.md` → 预期 `ok:true` ∧ `findings:[]` ∧ `section2BodyFound:true` ∧ `anchorCount:4`，`anchorUnion` ≡ landing-manifest 四行（实跑结果见文末「起草注记」）。
2. **（post-commit）** `node .claude/scripts/retro-efficiency.mjs --self-check` → exit 0，输出含**十四条**具名断言：`doc-code-directional`（反转实现必红）、`token-exact-only`（unavailable 夹具带 999999 的 total，求和仍只等于 exact 之和）、`no-merge-blank-not-zero`、`model-dedupe-join`、`ratio-zero-code-blank`、`no-write-side-effects`（运行前后对夹具目录做内容级快照 `path:size` + sha256、稳定排序比对完全一致，覆盖新建与原地覆写两种写入）、`role-token-sum`、`planner-delta-validity`、`model-family-map`、`actor-classification`、`consumer-domain-classification`（治理域 JSON → 文档、业务域 JSON → 代码，含 `server/deploy/**` 与 `firmware/**/test/**` 两个业务域样例；Markdown 恒文档、源码恒代码）、`outside-roots-default-code`（域外且类型未覆盖的路径默认归代码并计入 `outsideKnownRoots`，所有路径都有确定归类）、`test-column`、`governance-data-as-doc`（反向断言）；夹具先断言非空。
3. **（post-commit）** `node .claude/scripts/retro-efficiency.mjs --since 260829` → exit 0；第一张表表头恰为 `changeId / 模型 / 代码行 / 其中测试行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟` **九列**（「其中测试行」列在位）；行集 92 行（含 5 个无事件包与 1 个仅事件目录），其中 75 行有行数、其余行对应格子为空（抽查一个不可归因 Pod，确认是空而不是 0）；`--since` 按 changeId 日期前缀过滤，边界包抽查正确。
4. **（post-commit）** 抽一个 Pod 双向核对：`261023-honcho-memory-wiring-restoration` 的代码行 2,109 / 其中测试行 1,336 / 文档行 1,998 与 `git diff --numstat <merge>^1 <merge>` 按 T1 的二分域规则手算一致；token 与 `node .claude/scripts/pod-events-aggregate.mjs --change-id 261023-honcho-memory-wiring-restoration` 的 exact 合计一致；agent 分钟与同件 `durationMs` 合计一致。
5. **（post-commit）** 角色汇总机械核对：`node .claude/scripts/retro-efficiency.mjs --json --since 260829` → 断言 `roles.builder.token` ≡ 全量事件中 `actor==='builder'` 且 `tokenUsage.status==='exact'` 的 `total` 之和；`roles.planner` 覆盖 = **34/92**（46 个 Pod 有 ≥2 检查点，其中 12 个首末差 ≤0 被剔除：7 个首末相同、5 个首末为负；按「出现相邻倒退」计为 6 个 Pod，两种计数都进 `--json`），无检查点或差 ≤0 的 Pod 不进合计且不写 0；`roles.verifier` 与 `roles.supervisor` 成本格为空且不参与任何分母；token 占比之和 = 100%（±0.1%），时长占比分母 = 派发行合计（planner 不并入），分母口径与表注一致。
6. **（post-commit）** 模型、角色与路径分类核对：同上 `--json` 输出里，11 个具名模型值（另 199 条无记录，合计 12 种）全部映射到族、`unmappedModels` 为空、原始值仍保留；`main-thread` 归入 planner 行、`merge-driver` 归 script 类不出现在 agent 行、`human` / `human-deputy` 不进角色表、`unmappedActors` 为空；分类抽查——`.claude/verify-gates.json` 计入文档、`server/deploy/**` 与 `firmware/**/test/**` 的数据/配置计入代码、`AGENTS.md` 计入文档、`.codebase-memory/artifact.json` 计入**代码**并计入 `outsideKnownRoots`；**所有路径都有确定归类（无未决桶）**，`outsideKnownRoots` 全量 198 行 / 9 个文件（`.codebase-memory/**`、`.planning/**`、根 dotfiles）。
7. **（post-commit）** dispatch-stamp 缺值写入夹具（`CLAUDE_PROJECT_DIR` 指向夹具根、`KIDPAL_CLAUDE_PROJECTS_DIR` 指向空目录以制造 transcript 未命中）：`node .claude/scripts/dispatch-stamp.mjs --dispatch-id <id> --actor builder --phase build --duration-ms unavailable --duration-reason fixture --tool-uses 1 --terminal-status success --token-status unavailable --task-package-id <夹具 id>` → exit 0；写入事件含 `model: null` + `modelStatus:'unavailable'`、`durationMs: null` + `durationStatus:'unavailable'`、`tokenUsage.status:'unavailable'`；随后 `retro-efficiency --json` 的覆盖计数各 +1，角色合计不写 0。
8. **（post-commit）** 零值政策回归：`--duration-ms 0` → `durationMs:0` + `durationNote:'reported-zero'`；`--token-status exact --token-total 0` → `note:'reported-zero'`（既有行为保持）；两者均 exit 0、均不阻断。
9. **（post-commit）** 检查点告警夹具：夹具 pod-events 预置一条 `mainThreadUsage.total = 100` 的 dispatch，再用累计值为 50 的 fixture transcript 跑一次 stamp → exit 0 ∧ stderr 含非单调告警 ∧ 新事件带 `cumulativeNote:'non-increasing'`，写入不被阻断。
10. **（post-commit）** `grep -n "retro-efficiency.mjs" .claude/commands/loop-retro.md .claude/references/dev-loop-operating-system.md` 两文件各至少一处命中；命令步骤 5 的输出清单含「两张」效率表（Pod 表 + 角色汇总表）。
11. **（post-commit）** `git diff --cached --stat` 对照 landing-manifest 四行：落地文件集 ≡ manifest（无夹带、无 scope creep）。

## 6. 留根清单

1. **归因不到的 Pod 行数留空**：本次实测 17/92 个包目录归因不到 merge commit（发布批合并、无 `Merge <changeId>` 的早期包——含 5 个无事件包中早期直落 main 的情形——等）。这些包的代码行**在「只认 merge commit」的口径下算不出**；若需要，可在后续批次加一条「subject 含 changeId 的非 merge 提交区间」回退归因，本批不预埋。若真实复盘里留空率成为障碍，再评估 OQ1 的 (b) 最小收口记录。
2. **墙钟是观测近似**：首末事件差在事件稀疏时低估；不为此补记录，表注如实写明。
3. **测试行进表**：九列 Pod 表含「其中测试行」列，同时保留 `--json.testLines`（OQ4=(b)）；文档代码比分母 = 实现 + 测试；`--json` 另给 `ratioDocImpl`（文档行 ÷ 实现行）。
4. **模型列可能为空**：44/92 个包目录没有任何 dispatch 记过模型（历史 transcript 遥测未覆盖），只能留空；这是数据事实，不是脚本缺陷。
5. **表只按 Pod 读、不加总**：个别超大治理制品提交（单包近 10 万行）会让任何全局合计失去意义；脚本不输出全局合计，避免把量级噪声当结论。
6. **ARQ-261014 保持 deferred（部分承接）**：T3 消费其六类实据的 ②（actor 归属误落——读侧分类）与 ⑥（模型 / 用量失真——字段在场、族映射、检查点告警；「声明 ≠ 实际」对账对应修法候选 ③）；实据 ② 的写侧归属纠错、实据 ③（同决定双记）、实据 ④（`refEventId` 单写）、实据 ⑤（时间戳倒挂）与 ⑥ 的快照刷新根因修复仍留在该 ARQ，本批不做 landing 期指针注记（无 `docs/debt/**` 触面）。
7. **第一步基线报告归属**：`delivery/retro/2026-10-24-pod-efficiency-baseline.md` 未被 git 跟踪，本批 manifest 不含它；由主线程另择批补提交为 retro 证据。
8. **不做的事（防回摆）**：能力纪元、按纪元的跨期比较、门开火、逃逸归因、主线程/nano 用量检查点、验收断言数、事件类型/phase 收敛、四桶分类——human 2026-10-25 已明确不要；若日后要看这些，单独立批，不在本脚本上叠加。
9. **引据时点（自律件）**：本 PROPOSAL 的行号引据以 2026-10-26 本 worktree `gov/kidpal` 为准；landing 期若目标文件已被并行批改动，以 FROZEN 时点重核为准。
10. **planner 覆盖率与失效差**：48/92 个包目录有主线程检查点、2 个只有单点、46 个可做差；其中 7 个首末完全相同、5 个首末为负（按任一相邻倒退计为 6 个 Pod）被剔除，可算 34 个；planner token 11,867,898、主线程时段 91.2h（含人等）。失效差属 ARQ-261014 ⑥ 的快照保真面，本批只告警与剔除，不修刷新机制。
11. **脚本驱动面**：verifier / supervisor 不计 agent 成本（supervisor 为机械闸、无 dispatch；verifier 历史 4 次派遣无 token / 时长数据）；若将来要给机械闸计时，另立批，不在本脚本加。
12. **数据规范化的边界**：写入侧约束只对**新写入**生效——历史 199 条缺 `model`、57 条缺 `durationMs`、57 条缺 `tokenUsage`、11 条缺时间戳的事件不追溯重写，只在覆盖标记与 `unmappedModels` / `outsideKnownRoots` 里如实反映；模型族映射是脚本内单源表，新模型名到来时先落 `unmappedModels`，需人工补表（补表只是分析侧一行数据，不改结构）；`eventType` 51 种与 `phase` 155 种不收敛（两张表只消费 `dispatch-completed`），若将来要收敛另立批。
13. **历史零值不可区分**：15 条历史 `durationMs === 0`、4 条 `exact` 但 `total` 为零/缺失的 dispatch，无法区分「真实零」与「充数零」；新写入带 `durationNote` / `note: 'reported-zero'`，历史值按原值计入并在 `--json` 计数，不推测、不改写。
14. **分类规则与域定义的维护**：域定义写死在脚本内单源（治理域 / 业务域 / 产品文档域 + 历史 `changes/**`）；新增顶层目录时无需人工登记——其下文件先按类型规则判定，类型未覆盖的默认按业务/工具侧算代码，并出现在 `outsideKnownRoots` 诊断计数里（fail-open、不阻塞），供 loop-retro 观察是否需要把该根补进域定义。**所有路径恒有确定归类，不存在未决桶，也没有需要 human 逐条复核的边界项。**

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-24 - architect

**亲核留痕（全部本 worktree `gov/kidpal` 实测，非转述采信）**：
- 普查复算（`node -e` 全量遍历 `delivery/taskpackages/*/pod-events.jsonl` 与 `*/taskpackage.json`）：dirs 88（含 current）→ 带 taskpackage.json 85（含 current）/ 84（不含，与 §0 一致）；pod-events 81；tp∧events 交集 80（与 §0「84 中 80」吻合；差 1 = `260920-gov-review-dispatch-telemetry-source` 有 events 无 tp，81 = 80+1 自洽）；rounds 79、supervisor-verdict 80、pod-closeout 0、escapeRef 0；事件 1070 行（undefined eventType 11 行）、eventType 51 种、merge-completed 69 条中带 payload.mergeSha 1 条、dispatch-completed 429 条中 phase 155 种、model 12 种取值（含 (none) 199）；nano 样本 5 事件（human-decision×3、merge-completed×2、0 dispatch）。以上与 §2 普查段逐项吻合，唯两处措辞偏差见 F3/F6。
- 引据逐条亲核：`pod-events-append.mjs:26`（EVENTS_SCHEMA_CUTOFF）/`:29-36`（六值）/`:48-50`（throw）✓；`dispatch-stamp.mjs:290`（`const mainThreadUsage = telemetry.found`，PROPOSAL 记 :286-292 区间 ✓）；`merge-driver.mjs:1051`（mergeSha）/`:1052-1063`（payload 仅 summary）/`:1066`（commit message `merged <sha12>`）✓；`supervisor-verdict-gate.mjs:434/:447`（writeVerdictOnce + linkSync EEXIST write-once）✓；`pre-merge-escalation-gate.mjs:775`（仅 probeOutPath 落盘、不写 BLOCK 判定）✓；`verifier-evidence-check.mjs:126-131`（roundsDir/evidenceRef）✓；OS `:130`（六值闭集）/`:139`（见 F1）/`:145`（on-demand aggregation 无效率轴）✓；`dev-loop.md:91`/`:334` ✓；`journal-append.mjs:60`（round-outcome|friction 自有枚举）✓；`planner.md:42`（F-08）/`:53`（3n 保留清单）/`:55`（Step 5）/`:89`（[FS-EVIDENCE-3]）✓；`loop-retro.md:26-44`（输出契约无效率轴）✓；`collect.mjs`（c:/tmp/retro-metrics）：REPO 硬编码、classify 四+1 桶、墙钟首末事件差、govArtifact+govConfig 求和口径、写回 c:/tmp ✓。
- 结构门复跑：`landing-fidelity-check.mjs --preflight` → `ok:true`、`section2BodyFound:true`、`findings:[]`、`advisories:[]`、`anchorCount:13`，anchorUnion 13 成员与 landing-manifest 围栏块逐 token 一致 ✓。各 T 目标文件并集独立手算 = 13 路径，与 manifest 一致 ✓。
- 差分测试独立复现（非采信起草者结论）：`C:/tmp/tpfixture/delivery/taskpackages/261023-fixture-x` 注入 `escapeRef` 前后各跑 `node .claude/scripts/taskpackage-required-check.mjs <tp>/taskpackage.json`，两轮输出同形，唯一 `missing` 均为 `input-brief.md`，无未知键报错（复测后夹具已还原为无 escapeRef 态）✓。源码侧旁证：required-check 无顶层未知键白名单校验，`acceptance-coverage`/manifest 校验均只读具名字段。
- 反向影响面亲核：`verify.mjs`、`supervisor-gate-check.mjs`、`supervisor-verdict-gate.mjs` 对 `eventType`/pod-events 零读取（grep 实测零命中）；`closeout-check.mjs:269-274` 对 pod-events 仅 advisory 存在性检查；`loop-metrics.mjs:237` 只过滤 `dispatch-completed`（不受新类型影响）。EVENTS_SCHEMA_CUTOFF 消费方（closeout-check/dispatch-stamp/journal-append/loop-health-report/loop-metrics）全部只把它当日期谓词用，扩事件类型不破坏其语义。「只观测、不 blocking、不改门」两条在改法层守住（T7 告警不拦截、OQ3=(a) 不加牙、§6-8 自律件、scope-map/verify-gates/settings 零触）。
- scope-map 亲核：planner allow 含 `delivery/taskpackages/**`，pod-stamp 程序 fs 追加与 dispatch-stamp 同通道、无新写权需求 ✓；`escapeRef` 全仓 grep 零撞名 ✓；`.claude/loop-retro/` 现存 ledger.jsonl，T1 同目录纪律成立 ✓；`merge-driver.mjs:1629` `--self-check-merge-friction` 在场 ✓。

**Findings**：

- **F1 · SHOULD** —— OS §5d:139 现行句「Verifier rounds、gate outcomes、test results、supervisor verdict 不进入 pod-events，仍以 immutable verifier evidence / supervisor verdict 为权威」与 T4 的 `pod-closeout.gateFires` 段（verifier 红轮数、supervisor 最终裁决、mutation 计数写入 pod-events payload）直接冲突，而 T2 的「OS §5d 同步」清单（闭集枚举、两新事件 payload schema、fail-open 一句）未列对本句的修订。落地后 canonical 自相矛盾，后手实现者可据 :139 拒绝 gateFires 载荷。修法方向：T2 的 OS §5d 同步范围补一条——:139 句加 carve-out（「`pod-closeout` 的 `gateFires` 为收口时点自 immutable 制品的派生快照，权威仍在原制品；本句指原始 gate outcomes 不落 pod-events」即可，语义与 T4「不造第二账本」一致）。
- **F2 · SHOULD** —— 验证集第 8 条基线计数错误：实测 `node .claude/scripts/taskpackage-required-check.mjs --self-check` → `{"selfCheck":"taskpackage-required-check","pass":23,...,"total":23,"ok":true}`（changeid-prefix 4 项无误），PROPOSAL 写「基线 18 项」。核验官按 18 对表会误判。修法：改 23，或统一写「以 FROZEN 时点实测计数为准」并如实记 23。
- **F3 · SHOULD** —— T1 证据句「模型字符串普查（见本节开头）：13 种写法并存」与 §2 普查「12 种 model 取值」自相矛盾；实测 12 种（11 具名 + `(none)`）。修法：T1 证据句改「12 种」。
- **F4 · SHOULD** —— 验证集第 6 条断言「`nonCanonical` 列表非空（该包含历史异形类型）」对 `261023-honcho-memory-wiring-restoration` 在 eventType 轴不成立：实测该包事件类型 = human-decision×2 / friction×3 / dispatch-completed×4 / agent-escalation×1 / merge-completed×1，T7 九值闭集后 `eventTypes.nonCanonical` 预期为**空**；非 canonical 仅在 phase 轴成立（实测 `acceptance-freeze`×1、`acceptance-marker-fix`×1）。断言不点名轴，核验时要么误红要么空过。修法：第 6 条改为「`phases.nonCanonical` 非空（acceptance-freeze / acceptance-marker-fix 两值）；`eventTypes.nonCanonical` 预期为空」。
- **F5 · SHOULD** —— 验证集第 9 条自相矛盾：写「七字段非缺失」但只枚举六个字段名（epochId / openedAt / models / harness / dispatchEnvelope / reconstructed）；T1 字段清单为七项（另有 `note`）。`note` 必填与否两处不一致，机械断言无法照写。修法：钉死 `note` 可选，断言改「六必填字段非缺失 + note 可选」，或七字段全列。
- **F6 · NIT（不阻冻结）** —— phase 普查「前五 = acceptance 47 / build 40 / integrate 33 / closeout 17 / fix 14」排名错位：实测并列第五为 `acceptance-freeze` 17，`fix` 14 实为第七（前六 = (none) 52 / acceptance 47 / build 40 / integrate 33 / closeout 17 / acceptance-freeze 17）。所引各计数值本身准确、§2 与 T7 同源，仅「前五」措辞失真。修法：改「前六」并补 acceptance-freeze，或删排名只留计数值。
- **F7 · NIT（不阻冻结）** —— 两处落地执行面模糊：① §6 第 5 条「主线程在该条目追加本批指针注记」指向 `docs/debt/gov/ARQ-261014-…md`，该文件不在 manifest 13 行内、不在 §3 sweep 表内——若随 landing packet 同 commit，landing-fidelity-check 会以 scope-creep 误红；② T7 的 13 值 canonical phase 集合只存在于代码导出（CANONICAL_PHASES），OS §5d 同步只加 fail-open 句未写明集合的权威落点（按「机械面枚举的锚点纪律」应指向脚本导出而非在 OS 复制名册）。修法方向：① §6-5 钉死「独立 chore commit、不进本批 manifest」（与 INDEX backfill 先例同通道）；② OS §5d 补一句指向 `dispatch-stamp.mjs` 导出的 CANONICAL_PHASES。

**判档与升级**：治理面机器面（scripts / agents / commands / references）恒 Track B，本批轨道自评（Track B 主路径、非轻路径）成立；riskTier medium 成立（触 G3 承重脚本四件 + 新增三件，全线只观测不改门；G1 零触、契约面零触亲核无误）；Track A 段判据两条均不命中（非跨系统结构性、方向已由 human 两步走决定收敛）。无升级 human 项——上列 findings 均为起草层修订，不动改法方向。

verdict: BLOCK(F1..F5)


### R2 - 2026-10-24 - architect

**轮次纪律**：只核 R1 findings（F1–F7）修订到位性 + 是否引入新矛盾，不做全量重核。修订后版本 = DRAFT v0.2（Status 行实测）。

**逐条核验（行号为 v0.2 现行行号，均亲读原文）**：

- **F1（修订到位）**：T2 `:101` 补 :139 carve-out——替换句明确「原始 gate outcomes / test results 不落 pod-events，权威仍在 immutable 制品；`pod-closeout` 的 `gateFires` 是收口时点自这些 immutable 制品派生的**快照**（advisory derived），不构成第二事实源、不进任何判定链」。核：① 原句的过度枚举（「supervisor verdict 不进入 pod-events」字面）被撤换，字面冲突消解；② gateFires 上游 = write-once 制品（supervisor-verdict.json EEXIST fail-loud / verifier rounds immutable，R1 已亲核），快照无漂移源，「不构成第二事实源」成立；③「不进任何判定链」与 OQ3=(a)（`:261-265`）、§6-8 自律件一致，无新增牙。T4 `:143` 首条已加交叉引用；§3 `:223` OS 行说明已补 carve-out。三处落点齐。
- **F2（修订到位）**：验证集第 8 条 `:297` 改「基线 23 项 + changeid-prefix 4 项」并附双实测注记；与本判官 R1 实测 `pass:23,total:23` 一致。
- **F3（修订到位）**：T1 证据 `:65` 改「12 种取值并存」，与 §2 `:48`、T1 问题段 `:59` 单一口径；全文 grep 无「13 种」残留。
- **F4（修订到位，机械可判）**：`:295` 改两轴点名——eventTypes.nonCanonical 预期为空（五类事件逐类列举，与本判官 R1 普查实测逐类吻合）；phases.nonCanonical 非空且点名 `acceptance-freeze` / `acceptance-marker-fix`（各 1 条，与实测一致）。断言从「列表非空」含糊态变为两轴各可判真/假。
- **F5（修订到位）**：T1 `:71` 钉「六项必填 + `note` 可选（在场时须为字符串）」；验证集第 9 条 `:299` 同步为「六必填字段非缺失、note 可选」。两处口径闭合，断言可照写。
- **F6（修订到位）**：§2 `:47` 与 T7 `:199` 两处同源改「计数最高者 = (none) 52 / acceptance 47 / build 40 / integrate 33 / closeout 17 / acceptance-freeze 17（fix 14 紧随其后）」，删「前五」排名；与 R1 实测排序逐值一致。
- **F7①（修订到位）**：§6-5 `:307` 钉死「独立 chore commit」并写明 scope-creep 误红机理与 INDEX backfill 先例通道。
- **F7②（修订到位，源码归属核成立）**：T7 `:209` 改为 pod-events-append.mjs 只导出 `VALID_EVENT_TYPES`；`:210` `CANONICAL_PHASES` 归 dispatch-stamp.mjs 导出、权威落点 = 代码导出；`:212` OS 指向导出、明写「不在 OS 复制名册」；§3 `:226` / `:228` 同步。亲核现行源码：`VALID_EVENT_TYPES` 现居 `pod-events-append.mjs:29`（事件类型写权归属正确）；`CANONICAL_PHASES` 为新增导出（设计规格），dispatch-stamp 是 phase 唯一写方、归属成立；§3 复扫先验 `:242` 的模式集仍指向 pod-events-append 旧常量面，与修订不冲突。

**新矛盾扫描（聚焦修订面）**：① carve-out「不进任何判定链」↔ T6 仪表盘消费——仪表盘是只读输出非判定链，无矛盾；② carve-out 撤换后 :139 剩余语面（human decision/correction append 句）不受影响；③ F7② 的导出分工与 `:211` 聚合消费面（eventTypes/phases 两段各取所辖导出）对齐；④ preflight 复跑（本判官，v0.2）：`ok:true`、`section2BodyFound:true`、`findings:[]`、`anchorCount:13`，anchorUnion 与 manifest 13 行一致——行号位移未破坏结构字段；⑤ Status 行已升 DRAFT v0.2、landing-manifest 与 packet-id 行在场（GREEN 前结构字段核对通过）。

**判档与升级**：维持 Track B 主路径、riskTier medium；无新升级项。

verdict: GREEN


### R3 - 2026-10-25 - architect

**轮次纪律**：v0.3 是 human 指示下的范围收缩重写（非 v0.2 修订轮），R2 的 GREEN 不沿用——本记录按 R1 纪律对 v0.3 全面审。human 原话基准（意图保真）：只留四个数——产生多少代码 / 消耗多少 token 与时间 / 当时用什么模型 / 文档与代码的比例，「不用把那些变化因素都考虑进去」。

**意图保真（核过，通过）**：目标段（`:21`）与表列（`:84` 八列 = changeId / 模型 / 代码行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟）逐项对应 human 四数；非目标（`:29`）显式撤下能力纪元 / 门开火 / 逃逸归因 / 主线程与 nano 检查点 / 验收断言数 / 事件类型与 phase 收敛 / 四桶分类；T1 明写模型「只记录、不归因、不映射、不按模型分层」（`:63`），OQ3 选去重并集；§6-8 防回摆。残留学面 grep（`纪元\|epoch\|gateFires\|escapeRef\|逃逸\|验收断言\|四桶\|门开火`）命中仅出现在 :29 非目标、:153 留根、:211 起草注记——设计正文无归因框架残留。**无偷偷留下归因框架。**

**覆盖计数亲核（全部独立复算，与 PROPOSAL 逐数吻合）**：
- 同时有 taskpackage 与 pod-events 的 Pod = **86**（今日重算；date-prefixed 目录 94，含 current 剔除）；**可归因 merge commit 75 / 不可 11**（按 `git log --all --merges --grep` + `Merge <id>` / `merge: dev/<id>` / 触碰 `delivery/taskpackages/<id>/` 三判据 + first-parent numstat 复跑；不可归因名单 11 个已逐一列出）——与 §0 `:27`、§2 `:35` 完全一致；**有 exact token 67 / 无 19**、**有模型记录 47 / 无 39**，同 86 分母，逐数吻合。
- 抽样行数（同一 glob 口径、numstat 增删合计、multiple merge 逐文件 max）：`261023-honcho-memory-wiring-restoration` = 代码 **2,847** / 文档 **1,260** / 测试 **1,336**（逐数精确吻合）；`261022-m5-k4-history-corpus-backfill` = **4,011** / **262** / **1,158**（精确吻合）；`260829-story-p3` = **6,087** / **955**（吻合）、测试 3,246（严格按所述 pattern 复算 3,242，见 F5）。
- 字段可行性：`dispatch-stamp.mjs:303-321` 的 pod-events payload 实含 `tokenUsage` / `durationMs` / 条件写入的 `model`，事件行带 `timestamp`——四类指标数据面在场，T1 无需新记录即可算，成立。
- 验证集第 4 条双向核对实跑可行：honcho 的 numstat 两侧 = 2,847 / 1,260；`pod-events-aggregate.mjs --change-id 261023-honcho-memory-wiring-restoration` 的 exact 合计 = **555,102**，与本判官逐事件直算一致（withTokenData 4/4，agent 30.2 分钟）——该验证条机械可判且两侧同源。
- 结构字段：preflight 复跑 `ok:true` / `section2BodyFound:true` / `findings:[]` / `anchorCount:3`，anchorUnion 三成员 ≡ manifest 三行；§7 R1/R2 记录与判官原写入逐字节一致（`sha256` 实比：R1 `78ee01ef1243…`、R2 `3091c3bf28dc…`，append-only 未被改动）。
- 归因算法与第一步脚本同源：`collect.mjs:53-78` 的 `diffStatsFor` 确为「逐文件 a/d 分别取 max 后求和」，T1 `:64/:66` 所述「搬进仓内单源」属实。

**Findings**：

- **F1 · SHOULD** —— OQ2 推荐理由的事实前提不成立。`:119` 称「`delivery/**` 里的 markdown 计入文档行，`*.json` 机器制品计入代码行——这两类占比小且逐 Pod 可见，够用」，实测（75 个可归因 Pod 全量复算）：**JSON 机器制品占「代码行」的份额中位 40.7%**（top：260911-ecs-deploy-convergence 94%、261020-p3-rec-frame-visual 91%、260917-m1-k4-rma8-fixbatch 87%、261024-vq-overlay-close-end-session 74%；honcho 26%、k4 36%）；**`delivery/**` 的 markdown 占「文档行」的份额中位 92.8%**（honcho 1,260 文档行中 1,213 行来自 delivery/**，仅 47 行来自 `docs/**`）。即「代码行」列对治理重的 Pod 主要在数 JSON 工件、「文档代码比」列主要在比「验收/复盘文书 ÷（代码+机器制品）」，与 human 问的「产生多少条代码 / 文档跟代码的比例」有明显失真。这不改方向（OQ2 本就是 human 裁点），但推荐语必须换实测数字，否则 human 在错误前提上拍 (a)。修法方向：撤回「占比小」表述，改列上述中位数与两三个极值例子，并写明各列实际度量的是什么；选项 (a)/(b)/(c) 保留交 human。
- **F2 · SHOULD** —— self-check ②（`:68`「token 只求和 `exact`（夹具里放一条 `unavailable`，求和不变）」）判别力不足：若夹具的 `unavailable` 条目不带 `total` 数字（与真实 unavailable 事件同形），「错把 unavailable 也求和」的实现同样得到不变的和 → 反转不变红，属真空绿（「夹具必须能判别」）。修法：钉死夹具 —— `unavailable` 条目带一个可识别的非零 `total`（如 999999），断言和仍等于仅 exact 之和；这样错实现必红。校验 `:138` 的自检名清单同步点明这一条。
- **F3 · NIT（不阻冻结）** —— 表分母未写死：`:35` 的 86 口径 = 「同时有 taskpackage 与 pod-events」；实测另有 **5 个只有 taskpackage 无 pod-events** 的 Pod（`260829-audio-bargein-hw`、`260829-shim-skeleton`、`260831-cloud-link-bringup`、`260831-simulator-lvgl-align`、`261024-qa-session-timeout-producer`）不会进表，其代码行本可从 merge 归因算出。建议 T1 一句写明分母与这类 Pod 的处置（进表留空或明确排除）。
- **F4 · NIT（不阻冻结）** —— `--since` / `--until` 的过滤键未指定（changeId 日期前缀 vs 事件 `timestamp`）。T2 让 `/loop-retro` 传「周期起点」，若不定死键，下次复算可能换口径。建议在 T1 口径段一句钉死「按 changeId 日期前缀过滤」。
- **F5 · NIT（不阻冻结）** —— 样数与合计的小差：`260829-story-p3` 测试行 3,246 与所述 pattern 严格复算 3,242 差 4 行（`firmware/main/test/host_app/main/CMakeLists.txt` 被计入，而 pattern 列的 `tests/` 不匹配单数 `test/`）；全局合计 343,790 / 141,038 与本判官同规则复算 343,619 / 140,971 有 0.05% 级差（全局本就标注「只作量级参照」）。建议样例数改 3,242（或把 `test/` 加进 pattern），全局数注记「近似值」。
- **F6 · NIT（不阻冻结）** —— 「无写盘副作用」是 T1 的显式契约（`:56`），但验证集无对应断言。建议 self-check 顺手加一条：夹具目录跑前后文件清单不变。

**验证集与自检总评**：6 条验证断言均可机械执行；第 4 条已实跑两头对数。判别力除 F2 一处外成立（① 反转必红、③ 空非零、④ 去重连接、⑤ 代码 0 留空均有明确反向行为）。第 5 条为存在性检查（两文件命中 + 步骤 5 含「效率表」），对文面接线够用。

**判档与风险**：治理面机器面（`.claude/scripts/**` + `.claude/commands/**` + references）→ 恒 Track B 主路径，architect 自判「非瘦制品轻路径」正确（命中轻路径排除清单）。riskTier **low-medium 认可**：一件只读脚本 + 两处文面接线，零 schema / 零每 Pod 新记录 / 零门语义 / 零 agent 定义 / G1 与契约面零触；`low-medium` 在本仓 PROPOSAL 有 5 个先例（261019 / 261021 / 261023 等）。Track A 段判据不命中（非跨系统结构性、方向已由 human 收敛）。**无升级 human 项**；F1 的实测数字建议随 OQ2 一并呈给 human 在立项 ack 时拍。

verdict: BLOCK(F1..F2)


### R4 - 2026-10-25 - architect

**轮次纪律**：只核 R3 findings（F1–F6）修订到位性与新矛盾，不做全量重核。版本 = DRAFT v0.4（Status 行实测）。

**逐条核验（全部独立复算 / 亲读，未采信转述）**：

- **F1（到位，实测逐数复现）**：OQ2 重写为四选项附实测。独立复算（75 个可归因包、同规则）——机器工件（`json`/`jsonl`/`yaml`/`yml`）占朴素「代码行」份额 **中位 41.3%**、极值 **12.6%–96.6%**（PROPOSAL 记 41.3% / 13%–96.6%，一致）；`delivery/**` 占「文档行」**中位 92.8%**（honcho 1,260 中 1,213 来自 delivery、47 来自 `docs/`，逐数一致）。四选项中位文档代码比复现：**(a) 0.197→0.20 / (b) 0.038→0.04 / (c) 0.001→0.00 / (d) 0.324→0.32**；样例外：honcho a 0.44 / b 0.03 / d 0.67，k4 a 0.07 / b 0.00 / d 0.10，story-p3 a 0.16 / d 0.18——与 OQ2 所列**逐值吻合**；(c) 的「story-p3 仅 10 行」亦复现（docs 改动 10 行）。推荐 (d) 的理由（修两块失真、规则仍两条）与实测相符，交 human 拍板的前提已修正。
- **F1 配套（(d) 口径抽样，逐数复现）**：honcho **代码 1,871 / 文档 1,260 / 测试 1,336**、k4 **2,515 / 262 / 1,158**、story-p3 **5,170 / 955 / 3,246**——与 §2 `:36` 完全一致（code = 非文档且非 json/jsonl/yaml/yml；doc 规则不变；测试形态含 `tests?/`）。`--self-check` 与验证集第 4 条的 1,871/1,260 对表可执行。
- **F2（到位，判别力成立）**：T1 `:68` self-check ② 的 `unavailable` 夹具钉死带可识别非零 `total`（999999），断言和仍只等于 exact 之和——错实现（把 unavailable 一并求和 / 按 total 存在与否过滤）必得 exact+999999，断言变红。非真空绿。
- **F3（到位）**：行集钉死 = 含 `taskpackage.json` 或 `pod-events.jsonl` 的目录（`current/` 除外，当前 **92**）；5 个仅 taskpackage 包与 1 个仅事件目录（`260920-gov-review-dispatch-telemetry-source`）明写进表、缺列留空；§6-1 补「只认 merge commit 口径下同样归因不到、回退算法列为后续可选」。独立复算行集 = **92**，与 PROPOSAL 一致。
- **F4（到位）**：过滤键钉死为 changeId 前 6 位日期前缀，事件 `timestamp` 只用于墙钟；验证集第 3 条补「边界包抽查」。
- **F5（到位）**：测试形态改 `tests?/`（含单数 `test/`），story-p3 测试行 **3,246** 与本判官按该 pattern 复算一致（3,242 + `firmware/main/test/host_app/main/CMakeLists.txt` 4 行）；§2 全局合计已删（§6-5 改为「表只按 Pod 读、不加总」，与「量级噪声」理由自洽）。
- **F6（到位，机械可判）**：self-check ⑥ `no-write-side-effects`——运行前后对夹具目录做文件清单快照、断言清单不变；判据机械（可目录递归列路径比对）。

**新矛盾扫描**：

- **F1 · SHOULD（本轮唯一打回项）** —— T1 证据 `:51` 残留 v0.3 旧数：「本次复算为 **75/86 可归因、11 不可**」，与同件 §0 `:27`、§2 `:35`、§6-1 `:150`、验证集 `:143` 的 **92 / 17** 直接冲突（起草注记第 8 条已声明「86/11/19/39 已被取代」，但此句漏改）。分母 86 与 92 并存会误导核验官对「行集 92 行、75 行有行数」的判读。修法：改为「75/92 可归因、17 不可」（括号内与基线 69/87 的对照说明可留）。
- 其余口径自洽核过：92 行集 = tp-only 5 + ev-only 1 + both 86；归因 75/17、exact 68/24、模型 48/44 四项均在 92 分母下成立（本判官逐项复算：92 / 75+17 / 68+24 / 48+44 全对）；OQ2 四比值与 (d) 抽样、T1 口径、§5 第 3/4 条、§6-1/4 五处互洽，无第二套数。
- **F2 · NIT（不阻冻结）** —— F6 的「文件清单快照」按路径清单比对，能抓新建文件，但原地覆写既有夹具文件（如误 append 到夹具 `pod-events.jsonl`）不改清单、不会变红。建议快照含文件大小或内容 hash（一行 `stat` 即可），让「零写盘」断言完整。
- 结构面：preflight 复跑 `ok:true` / `section2BodyFound:true` / `findings:[]` / `anchorCount:3`，anchorUnion 三成员 ≡ manifest；§7 R1/R2/R3 记录与判官原写入逐字节一致（sha256：R1 `78ee01ef1243…`、R2 `3091c3bf28dc…`、R3 `f98dc45c6cf3…`），append-only 未破。

**判档与风险**：维持 Track B 主路径、riskTier low-medium（认可，无变化：仍是一件只读脚本 + 两处文面接线，零 schema / 门语义 / agent 定义 / G1 / 契约面）。Track A 判据不命中。**无升级 human 项**；F1 是一行旧数订正，不动口径与方向。

verdict: BLOCK(F1)


### R5 - 2026-10-25 - architect

**轮次纪律**：只核 R4 两条（F1/F2）修订到位性与新矛盾。版本 = DRAFT v0.5（Status 行实测）。本记录为 GREEN 结论；全局口径与实测数字已在 R3/R4 逐数复算，本轮不重复全量重核。

- **F1（到位，单一口径核过）**：T1 证据 `:51` 已改「本次复算为 **75/92** 可归因、**17** 不可」，与 §0 `:27`（92 / 17 / 24 / 44）、§2 `:35`（92 / 75+17 / 68+24 / 48+44）、T1 行集 `:58`（92）、§6-1 `:150`（17/92）、§6-4 `:153`（44/92）、验证集 `:143`（92 行、75 行有行数）六处一致。全文 grep：`75/86` / `11 不可` 仅剩两处，均在合法历史位——§7 R4 评审记录 `:257`（引述被打回的旧文本）与起草注记 `:272`（显式标注「已被第 8 条取代，保留作 v0.3 历史记录」）；正文与验证集零残留。
- **F2（到位，内容级且机械可判）**：T1 `:68` self-check ⑥ 升级为内容级快照——递归枚举、稳定排序、每文件记录 `path:size` 与内容 sha256，断言快照前后完全一致；明写覆盖「新建文件」与「对既有夹具文件的原地覆写（误 append 到夹具 `pod-events.jsonl` 这类清单不变但内容变的情况）」。判定机械（两次枚举逐项字符串比对，任一差异即断言失败）；sha256 使原地覆写必然变红，R4 指出的漏检面已闭合。§5 第 2 条 `:142` 同步为该口径。
- **新矛盾扫描**：Status 行 v0.5；OQ2 四选项与实测数字（41.3% / 92.8% / 0.20 / 0.04 / 0.00 / 0.32 及样例外）与 R4 已核版本一致、未被本次修订波及；landing-manifest 围栏块三行原样；preflight 复跑 `ok:true` / `section2BodyFound:true` / `findings:[]` / `advisories:[]` / `anchorCount:3`，anchorUnion ≡ manifest。§7 R1–R4 记录与判官原写入逐字节一致（sha256：R1 `78ee01ef1243…`、R2 `3091c3bf28dc…`、R3 `f98dc45c6cf3…`、R4 `d5b734dd0dc9…`），append-only 未破。未发现新矛盾。
- **判档与风险**：维持 Track B 主路径、riskTier low-medium（无变化）；Track A 判据不命中；无升级 human 项。R4 的 NIT 已被本修订吸收（内容级快照）。

verdict: GREEN


### R6 - 2026-10-26 - architect

**轮次纪律**：v0.6 在 v0.5 上追加 human 2026-10-26 两项要求（⑤ 按 Agent 角色汇总、T3 数据规范化），R5 的 GREEN 不沿用——本记录按 R1 纪律对 v0.6 全面审。human 原话基准：①「不同 Agent 分别花的时间……当然也包括 token」；②「有没有一些数据现在记的不规范、需要我们去规范起来的？这个也应该加在这个批次里面。」

**意图保真（核过，通过）**：角色汇总表（`:70-75` 七列，含 planner 主线程行与 verifier/supervisor 脚本驱动行）对应原话一——「后边定义的角色（不是跑脚本的）」落为 architect / architect-reviewer / landing-verifier 照列 + planner 单列，跑脚本的（verifier / supervisor）单列并标「不计入 agent 成本」；T3（`:98-127`）对应原话二，且明确「只作用于未来写入侧 + 分析侧映射，不追溯重写历史」（`:31`）。角色占比是按已记录 actor 的直接聚合，不构成归因框架；非目标清单与防回摆条目原样保留。

**角色汇总数字亲核（全部独立复算，逐数吻合）**：
- 派发角色 exact token 合计 **59,402,388**、时长合计 **366,932,390 ms（6,116 分钟）**——与 `:41` 完全一致。
- token 占比（分母含 planner = 59,402,388 + 11,867,898 = 71,270,286）：builder **29.65%→29.7** / acceptance-author **29.18%→29.2** / planner **16.65%→16.7** / integrator **15.39%→15.4** / fixer **6.91%→6.9** / architect **2.04%→2.0** / landing-verifier **0.12%→0.1** / architect-reviewer **0.05%**——逐值吻合。
- 时长占比（派发行分母）：builder **39.9** / acceptance-author **35.5** / integrator **15.2** / fixer **8.1** / architect **1.2**——逐值吻合（余三行 ~0.1% 未列，非全称断言）。
- planner：38 条自记 stamp、0 条 exact ✓；有检查点 Pod 48 / 单点 2 / ≥2 可做差 46 / 首末差 >0 可算 **34** ✓；planner token **11,867,898** ✓；主线程时段（墙钟 − Σ派发时长，可算 Pod 求和）**91.2h** ✓。verifier 4 次派遣、无 token/时长 ✓；supervisor 无派遣 ✓。
- 分母与标签足以防误读：token 分母含 planner、时长分母不含 planner 时段——两分母均被 `:73` 钉死且要求表注写明；两列占比各自合计 = 100%（token：所有有 exact 值的行全列含 planner；时长：派发行全体，planner 时段不是行）。planner 时间列带「主线程时段（含人等）」标签 + 「不是纯工作时长」明示 + 不并入时长占比、占比格留空注原因（`:74`）——足够。verifier/supervisor 空行不进任何分母（`:75`）✓。
- **6 vs 5 口径裁决**：表口径应采用「首末差 ≤0 即剔除」（可算性的操作判据：12 剔除 = 7 相同 + 5 首末为负，34 可算）；「出现相邻倒退 = 6」是快照保真的诊断计数（ARQ-261014 ⑥ 面），不是剔除数。现稿两处已分开记（`:103`「两种都记、判据分开写」、`:122` `--json` 分 `identical`/`decreasing`、`:187` 验证集双计数、`:206` 留根）——**认可现状，不改**。

**T3 普查与可行性亲核（独立复算）**：
- 87 个有事件目录 ✓、457 条 dispatch ✓、199 缺 model ✓、57 缺 durationMs + 15 条 0 ✓、57 缺 tokenUsage（11 unavailable + 4 exact 零/缺）✓、11 缺时间戳 ✓、模型 11 具名值 ✓、merge-driver 70 / human-deputy 2 / main-thread 1 ✓、检查点 48/2/7/5/6/34 ✓——T3 普查段逐项复现。
- 写入侧现状断言：`dispatch-stamp.mjs:200`（`--duration-ms` 必填、可传 0）✓；`:264-274`（tokenUsage transcript 路径恒写、无零值注记）✓；`:290-292`（mainThreadUsage 条件写入）✓；`:303-322`（model 条件展开 `...(model ? { model } : {})`）✓。
- 兼容性：`model: null` / `durationMs: null` 对既有消费方安全——`pod-events-aggregate.mjs:168`（`typeof m === 'string'`）、`:126`（`typeof durationMs === 'number'` 过滤）、`loop-metrics.mjs:132/:139`（`|| 0`）均空值安全；`modelStatus` / `durationStatus` / `durationNote` / `cumulativeNote` / `modelFlagMismatch` 全为新增键、只增不改；门与钩子对 pod-events payload 零读取（R1 已核，本轮复核未变）。验证集第 7 条的 `KIDPAL_CLAUDE_PROJECTS_DIR` env 机制**现行已存在**（`transcript-telemetry.mjs:33`），夹具路径可行。
- OS §5d 增补（三键必在场 + unavailable 语义 + 检查点单调累计）为 additive，与既有「不传则不带该键」的 mainThreadUsage 条件语义不冲突。
- **ARQ-261014 承接核过**：其六类实据 ②（actor 归属误落）/ ⑥（模型/用量失真、checkpoint 不可信）确实存在且与本批消费面相符；留给 ARQ 的（写侧归属纠错、快照刷新根因）边界写清（`:125`/`:202`），无重复立项。附注：ARQ 条目 `:22` 的追加证据仍写「可与本批 T7 同批」——T7 已随 v0.3 撤下，现承接体是本批 T3；本批不动 ARQ 文件（§6-6），该指针留待 ARQ 后续更新时修正。

**验证集与自检（11 条 + 十条具名断言，判别力逐条核）**：`role-token-sum`（builder/integrator exact + 一条 unavailable 夹具，角色行只等于自身 exact 和、分母为全表 exact 行合计——错实现必红）✓；`planner-delta-validity`（正差计入、负差不计入不写 0）✓；`model-family-map`（同族多写法归一、未知落 `unmapped`）✓；`actor-classification`（`main-thread`→planner、`merge-driver`→script、`human` 不进角色表）✓；第 7 条缺值夹具（exit 0 + 三键 unavailable 形态 + 覆盖计数 +1）✓；第 8 条零值政策（0 + `reported-zero`，真实零与缺失可区分）✓；第 9 条检查点告警夹具（预置 100、写 50 → 告警 + `cumulativeNote`、不阻断）✓ 需要读前值逻辑落地，判别力成立；第 5 条「token 占比和 = 100%±0.1%」机械可判 ✓。

**Findings（全部 NIT，不阻冻结）**：
- **F1 · NIT** —— 模型取值计数口径不一：§2 `:42` 与 T3 `:100` 写「模型取值 12 种」，验证集第 6 条写「11 个具名原始模型值」；实测 11 具名 + 缺省哨兵 = 12。建议统一为「11 具名 + 缺省」或「12 种含缺省」，避免核验官对表。
- **F2 · NIT** —— 事件总数快照漂移：`:42`/`:100` 记「1,133 条事件」，本判官 2026-10-26 复算为 **1,135**（+2，非 dispatch 事件；全部派生计数 199/57/57/11/457 复算不变）。属 §6-9 引据时点自律范围，建议 landing 前重核时顺手刷新并注明快照时点。
- **F3 · NIT** —— T1 self-check 清单（①–⑧）与验证集第 2 条「十条具名断言」差两条：`model-family-map` / `actor-classification` 属 T3 分析侧、未列入 T1 的 ①–⑧ 枚举。建议 T1 清单补齐为十条或注明「另含 T3 两条」。
- **F4 · NIT** —— ARQ 承接标注两处小瑕疵：① `:125`/`:202` 把「同决定双记（ARQ 实据 ③）」「`refEventId` 单写（实据 ④）」归在「② 的写侧归属纠错」名下，编号与 ARQ 六类实据不完全对位；② header `:4`「本批不治」与 §6-6「部分承接消费 ②/⑥」措辞有张力，建议 header 改「部分消费（读侧分类 + 告警），根因修复保持 deferred」。

**判档与风险**：**riskTier medium 认可**——T3 触 `dispatch-stamp.mjs` 每次派发的写入路径（较 v0.5 只读版升一档有据），但全线 fail-open、零 schema 破坏、零门语义、零 agent 定义、G1 与契约面零触，不取 high；Track B 主路径成立（机器面恒 Track B；方向已由 human 两轮原话收敛，Track A 判据不命中）。**无升级 human 项**——四条 NIT 均为措辞/快照层，不动口径与方向。

verdict: GREEN


### R8 - 2026-10-26 - architect

**轮次说明**：本记录合并覆盖 R7 请求（v0.7）与 R8 请求（v0.8）。R7 派发时评审对象为 v0.7（unmappedPaths 设计），审阅期间文件已被 human 纠正推进到 **DRAFT v0.8**（二分域机械判定、无人工复核项）——按判官纪律以当前盘上版本为准，对 v0.8 全面审；R7 范围内与分类无关的核对（数字、表形、验证集、R6 NIT 订正）并入本记录。v0.7 的中间态不再单独出 verdict。

**意图保真（核过，通过）**：二分域规则（T1 `:70-77`）忠实 human 纠正版原话——「不管它是 deploy 的还是 test 的，只要在业务域起作用，那它就是代码」落为规则 3 的无特例二分（`server/deploy/**`、`firmware/**/test/**` 的数据/配置 → 代码，明文写死）；源码恒代码（含 `.claude/scripts/**.mjs`，规则 1 优先于域）、Markdown 恒文档（含域外 `.planning/**/*.md`）；OQ1=(a) / OQ3 `+` 连接 / OQ4=(b) 九列均已按拍板落。**人工复核残留：零**——v0.7 的「边界判定清单（需 human 复核）」整节已删，改为「规则应用示例（机械判定结果，不是复核事项）」；`:164`/`:199` 明示「不存在需要 human 逐条复核的边界项」；§6-14 把域外默认代码 + `outsideKnownRoots` 诊断定为新根的观察机制（fail-open）。原 v0.7 的十条边界判定（changes/**→治理、tools/**→业务、`.codebase-memory`→域外等）在 v0.8 下全部变为机械可判且方向一致。

**数字亲核（全部独立复算）**：

- **outsideKnownRoots 198 行 / 9 文件：复现 ✓**。全量枚举 75 个可归因包全部 merge 的域外文件：`.planning/**`×4（85+15+6+1=107，均 `.md` → 文档 + 计入 outside）、`.codebase-memory/artifact.json` 70 → 代码、`.gitattributes` 15 / `.gitignore` 1 → 代码、`AGENTS.md` 5 → 文档、`.codebase-memory/graph.db.zst` 0 —— 合计 198 行 / 9 文件，与 `:39`/验证集第 6 条一致。
- **抽样三包：两个复现、一个口径分裂（见 F1）**：story-p3 **5,194 / 1,848 / 3,246（0.36）**按成文规则精确复现 ✓；honcho **2,138 / 1,969** 与 k4 **2,537 / 1,736** 按成文规则复算得 **2,109 / 1,998（0.95）**、**2,489 / 1,784（0.72）**——差值恰为 `test-scope-final*.txt`（honcho 29 行、k4 22+26=48 行）：即 claimed 数只在「`.txt` → 代码」的**隐含规则**下复现，而成文规则第 5 条写「其余类型：治理域 → 文档」（`.txt` 不在任何类型清单）。
- **中位数 1.07：不复现（见 F1）**。成文规则下 75 个可归因包的中位文档代码比 = **1.293**；`.txt`→代码口径 = **1.281**；把 17 个未归因包按 0 计入的 92 值中位 = 0.979；doc/impl = 4.56——任何合理变体都得不到 1.07。稳健性本身：p25 0.72 / p75 2.01 / 均值 2.42（被个别超大治理包拉高，max 32.4），中位数作为口径参考是稳健的，问题只在与 claimed 值不同源。
- 不变项复核（与 R6 一致）：92 行集 / 归因 75+17 / exact 68+24 / 模型 48+44 ✓；角色汇总全数（59,402,388 / 366,932,390 ms / 各占比 / planner 34 Pod / 11,867,898 / 91.2h）✓；T3 普查（199/57+15/57+4/11、actor 三类、检查点 48-2-7-5-6-34）✓；写入侧设计空值安全（aggregate typeof 守卫、loop-metrics `|| 0`、五新键只增）✓；`KIDPAL_CLAUDE_PROJECTS_DIR` env 在场（transcript-telemetry:33）✓。

**Findings**：

- **F1 · SHOULD（本轮唯一打回项）** —— 分类规则文本与原型数字不同源。claimed 抽样 honcho 2,138/1,969、k4 2,537/1,736 只在「`.txt` → 代码」的隐含规则下复现（`test-scope-final*.txt`，integrator 的 test-scope 清单件），而 T1 成文规则第 5 条为「其余类型：治理域 → 文档」；story-p3 则按成文规则复现——同一节内两套口径并存。且 claimed 中位数 **1.07** 在成文规则（1.293）、`.txt`→代码（1.281）、含未归因零（0.979）、doc/impl（4.56）任何变体下都不复现。验证集第 4 条要求按「T1 的二分域规则」手算 2,138/1,969——按现文本手算得 2,109/1,998，**该验证条按现文本必挂**。修法（二选一，规则与数字必须同源）：① 把 `.txt`（工具/清单类文本件）的归类写进规则文本（如「`.txt` → 代码」或按域二分），并把中位数如实改为该口径复算值（1.28）；② 保持成文规则不动，把三个抽样与中位数改回 2,109/1,998、2,489/1,784、1.29。另建议 §2 `:39` 的「域外路径…按**代码**侧计」改为「先按类型规则判定、类型未覆盖的默认代码侧，全部计入 `outsideKnownRoots`」——现句与 T1 `:77` 自己的示例（`.planning/**/*.md` → 文档且计入 outside）矛盾（198 行中 112 行实为文档类）。
- **F2 · SHOULD** —— §6-3 `:188` 残留 v0.6 旧文「**测试行不进表**：只在 `--json` 的 `testLines` 可取（OQ4=(a)）」，与 OQ4 已裁 (b)、九列表「其中测试行」列（T1 `:69`/`:78`、验证集第 3 条）直接冲突。修法：改为「测试行单列『其中测试行』列，同时保留 `--json.testLines`（OQ4=(b)）」。
- **F3 · NIT（不阻冻结）** —— header `:4` ARQ 承接仍写「读侧的 actor 值域分类与告警面」——T3 实际含写入侧字段在场约束（model / durationMs / tokenUsage 必在场 + 检查点告警），建议改为「读侧 actor 分类 + 写入侧字段在场约束与告警」。§6-6 的实据 ②③④⑤⑥ 对位已在 v0.8 订正到位 ✓。
- 其余 R6 NIT 复核：模型计数已统一（「11 个具名取值 + 199 条无记录（合计 12 种）」，§2/T3/验证集三处同口径）✓；事件数快照已注记（1,133 起草期 / 1,135 判官复算 / landing 前现跑）✓；self-check ①–⑭ 与验证集第 2 条十四条具名断言逐一对齐 ✓。

**验证集与自检（11 条 / ①–⑭，判别力核过）**：新增四条断言——`consumer-domain-classification`（治理 JSON→文档、业务 JSON→代码，含 server/deploy 与 firmware test 两样例；Markdown 恒文档、源码恒代码）✓ 反转必红；`outside-roots-default-code`（域外类型未覆盖默认代码 + `outsideKnownRoots` 计数 + 无未决桶断言）✓；`test-column`（测试行同现于表列与 `--json.testLines`、比值分母含测试）✓；`governance-data-as-doc`（`.claude/*.json` 反转成代码必红）✓。其余十条 R6 已核未变。第 4 条在 F1 修前不可执行（手算对不上），修后可执行。

**判档与风险**：riskTier **medium 维持认可**（T3 触每次派发的写入路径，fail-open、零 schema 破坏、零门语义、G1 与契约面零触）；Track B 主路径成立（机器面恒 Track B；OQ 四项均经 human 拍板、方向收敛，Track A 判据不命中）。**无升级 human 项**——F1 是规则文本与原型的同源化订正（规则本体已由 human 拍板，不需再裁），F2 是残留清理。

**结构**：§7 R1–R6 记录与判官原写入逐字节一致（sha256：R1 `78ee01ef…`、R2 `3091c3bf…`、R3 `f98dc45c…`、R4 `d5b734dd…`、R5 `d1b72d59…`、R6 `04421579…`），append-only 未破；preflight 复跑 `ok:true` / `findings:[]` / `advisories:[]` / `anchorCount:4`，anchorUnion ≡ manifest 四行；Status 行 DRAFT v0.8。

verdict: BLOCK(F1..F2)


### R9 - 2026-10-26 - architect

**轮次纪律**：只核 R8 三条（F1–F3）修订到位性与新矛盾。版本 = DRAFT v0.9（Status 行实测）。

- **F1（到位，规则与数字已同源）**：architect 选修法 ②——成文规则不动（治理域起作用 → 文档，`delivery/**` 下的 `test-scope-final*.txt` 收口清单明写计入文档行），原型脚本改为与 T1 规则逐条一致后重算。独立复算（本判官，成文规则口径）：中位文档代码比 **1.293 → 1.29** ✓；honcho **2,109 / 1,998 / 1,336（0.95）**、k4 **2,489 / 1,784 / 1,158（0.72）**、story-p3 **5,194 / 1,848 / 3,246（0.36）**——与 §2 `:38` 逐数吻合；story-p3 的 5,194 确为 v0.7 的 5,180 + 14 行域外类型未覆盖路径按「默认代码」并入（doc 不变）✓。§2 `:39` 域外措辞改为「先按类型规则判定（`.planning/**` 的 markdown 仍算文档），类型规则未覆盖的默认按代码侧计」——与 T1 `:77` 示例不再矛盾；`outsideKnownRoots` 9 文件 / 198 行明写「仅作 --json 诊断计数」✓。验证集第 4 条已改回 2,109 / 1,336 / 1,998，按成文规则手算可执行 ✓。旧数（2,138 / 1,969 / 2,537 / 1,736 / 1.07 / 0.92 / 0.68）正文零残留——仅存于 §7 R8 评审记录（历史引述，合法）；起草注记第 12 条（v0.8）的旧数已随之订正、第 13 条记录 v0.9 重算 ✓。
- **F2（到位）**：§6 第 3 条现为「**测试行进表**：九列 Pod 表含『其中测试行』列，同时保留 `--json.testLines`（OQ4=(b)）；文档代码比分母 = 实现 + 测试；`--json` 另给 `ratioDocImpl`」——与 T1 `:69`/`:78`、OQ4、验证集第 3 条四处同口径；全文 `OQ4=(a)` /「测试行不进表」零残留（仅 §7 历史记录）✓。
- **F3（到位）**：header 承载 ARQ 行改为「**部分消费**（读侧 actor 值域分类 + 写入侧字段在场约束与告警，见 T3）」——与 T3 实际半径（写入侧 model / durationMs / tokenUsage 必在场 + 检查点告警）对齐 ✓。
- **新矛盾扫描**：Status 行 v0.9；§2 :38-39 与 T1、§6-3、OQ4、验证集 3/4/6 六处同口径；不变项（92 行集 / 75+17 / 68+24 / 48+44、角色汇总全数、T3 普查与写入侧设计）未被本次修订波及；preflight 复跑 `ok:true` / `findings:[]` / `advisories:[]` / `anchorCount:4`，anchorUnion ≡ manifest 四行 ✓；§7 R1–R8 记录与本判官原写入逐字节一致（sha256：R1 `78ee01ef…`、R2 `3091c3bf…`、R3 `f98dc45c…`、R4 `d5b734dd…`、R5 `d1b72d59…`、R6 `04421579…`、R8 `ed9096ca…`），append-only 未破。未发现新矛盾。
- **判档与风险**：维持 Track B 主路径、riskTier medium（认可）；Track A 判据不命中；无升级 human 项。

verdict: GREEN

---

**起草注记（非判官区）**：

1. **v0.3 收窄（2026-10-25）**：按 human 2026-10-25 指示，把原 v0.2 的七条改法集整体收窄为两条（T1 只读效率表脚本、T2 `/loop-retro` 输出接线），只保留四个数——代码行 / token 与时间 / 当时模型 / 文档代码比。human 原话要旨：「这个我们一次性可能想要的太多了……能不能简化一下，只看这几项指标：产生多少条代码、消耗了多少 token、花了多少时间、当时用的是什么模型、文档跟代码的比例。」四类数据全部从现有 `pod-events.jsonl` 与 git 直接算，零新增每 Pod 记录。§7 评审记录（判官 append-only 区）原样保留、未改动一字。
2. **R1 findings 处置（内容整体撤下，逐条不再适用）**：F1（OS §5d:139 与 `gateFires` 冲突）——`gateFires` 整段删除，carve-out 无对象；F2（required-check 基线 18 改 23）——该验证条已随回归面条目删除；F3（13 种改 12）——原 T1 证据句已删，新 §2 只写本次实测的「47 有模型 / 39 无模型」；F4（aggregate 断言两轴）——aggregate 不再改动，断言删除；F5（epochs 字段必填性）——纪元台账整段删除；F6（phase 排名措辞）——phase 普查整段删除；F7①（ARQ 指针须独立 chore commit）——该 landing 期动作删除，ARQ 保持 deferred、本批不动作；F7②（`CANONICAL_PHASES` 权威落点）——phase 收敛整段删除。R1 的 BLOCK 结论对 v0.2 仍成立、记录保留，v0.3 请判官按新范围重新评审。
3. **收窄后的实证（2026-10-25，本 worktree `gov/kidpal`；**本条数字已被第 8 条取代**，保留作 v0.3 历史记录）**：原型脚本全量跑 86 个 Pod（同时有 taskpackage 与 pod-events）——可归因 merge commit 75、不可 11；有 exact token 67、无 19；有模型记录 47、无 39；抽样 `261023-honcho-memory-wiring-restoration` 代码 2,847 / 文档 1,260 / 测试 1,336，`261022-m5-k4-history-corpus-backfill` 4,011 / 262 / 1,158，`260829-story-p3` 6,087 / 955 / 3,246；全局代码 343,790 / 文档 141,038（含超大治理制品提交，只作量级参照）。
4. **preflight 实跑（修订后最终态）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261024-gov-review-retro-efficiency-instrumentation/PROPOSAL.md` → `ok:true`、`section2BodyFound:true`、`findings:[]`、`advisories:[]`、`anchorCount:3`；`anchorUnion` = `.claude/commands/loop-retro.md`、`.claude/references/dev-loop-operating-system.md`、`.claude/scripts/retro-efficiency.mjs`，与 landing-manifest 三行逐 token 一致。
5. **路径形态族谱（`path-form-family.md`）**：本批无搬迁 / 改名 / 路径串改写，形态族谱表查表义务不适用，在此声明。
6. **承重引文 byte 对账**：v0.3 保留的引据（基线报告 :14 / :22 / :126、`collect.mjs:10` / `:175` / `:21-32` / `:123-126`、`dispatch-stamp.mjs:303-322`、`loop-retro.md:26-44`、OS `:145`）均经 `node -e` 逐条 `includes` 比对通过（起草期实跑）。
7. **长文与自身产物机检**：v0.3 全文一次重写并落盘，落盘后核行数、T 条目数（2）、`目标文件：` 行数（2）、manifest 行数（3）、分块哨兵零残留；正文无禁用字符。
8. **v0.4（R3 修订，2026-10-25）**：F1 → OQ2 重写为四选项并附实测数字（JSON/YAML 占代码行中位 **41.3%**、`delivery/**` 占文档行中位 **92.8%**、四口径中位文档代码比 0.20 / 0.04 / 0.00 / 0.32），推荐位改为 (d)（代码排除机器工件、文档保留治理文书），T1 口径与抽样数同步改按 (d)（honcho 1,871 / 1,260、k4 2,515 / 262、story-p3 5,170 / 955）；F2 → self-check ② 的 `unavailable` 夹具带可识别非零 `total`（999999），断言和仍只等于 exact 之和；F3 → T1 钉死行集与分母（含 `taskpackage.json` 或 `pod-events.jsonl` 的目录、`current/` 除外、当前 92 个；5 个无事件包与 1 个仅事件目录的处置写明）；F4 → `--since` / `--until` 过滤键钉死为 changeId 日期前缀，事件时间戳只用于墙钟；F5 → 测试形态改 `tests?/`（含单数 `test/`），story-p3 测试行 3,246 与最终口径一致，全局合计删除（判官复算与原型互差 0.05% 级，且表按 Pod 读不需要合计）；F6 → self-check 增 `no-write-side-effects`（夹具目录文件清单前后不变）。上一条（第 3 项）的 86/11/19/39 与全局合计数字已被本条取代。
9. **v0.5（R4 修订，2026-10-25）**：F1 → T1 证据段残留的「75/86 可归因、11 不可」改为「75/92 可归因、17 不可」，与 §0 / §2 / §6-1 / 验证集的 92/17 单一口径对齐；F2 → self-check ⑥ `no-write-side-effects` 从「路径清单快照」升级为「内容级快照（每文件 `path:size` + sha256、稳定排序）」，同时覆盖新建文件与对既有夹具文件的原地覆写；§5 第 2 条的断言描述同步。其余内容未动。
10. **v0.6（human 2026-10-26 追加两项）**：
    ① **按 Agent 角色汇总**（新增第二张表）：实测 dispatch 角色 exact token 合计 59,402,388、时长合计 366,932,390 ms（6,116 分钟）；token 占比（分母含 planner）= builder 29.7% / acceptance-author 29.2% / planner 16.7% / integrator 15.4% / fixer 6.9% / architect 2.0%。planner 38 条自记 stamp 全无 exact token；48 个 Pod 有检查点（2 个单点）、46 个有 ≥2；其中 7 个首末相同、5 个首末为负（按任一相邻倒退计为 6 个 Pod）→ 可算 **34** 个，合计 11,867,898 token、主线程时段 91.2 小时。planner 时间按协调方给的 (b) 口径报「主线程时段 = 墙钟 − Σ派发时长」，显式标注含人等、不并入时长占比。
    ② **数据规范化**（新增 T3）：普查 1,133 条事件 / 457 条 dispatch——199 缺 `model`、57 缺 `durationMs`（另 15 个为 0）、57 缺 `tokenUsage`（11 unavailable、4 个 exact 但 total 为 0/缺）、11 缺时间戳；模型 11 个具名取值 + 199 条无记录（合计 12 种）；全事件面 actor 含 `main-thread` 1 / `human-deputy` 2 / `merge-driver` 70。写法：写入侧 `dispatch-stamp` 三键必在场 + 检查点非单调告警（fail-open），分析侧模型族映射 + 角色三分类 + 覆盖标记；不追溯历史。与 ARQ-261014 实据 ②/⑥ 的承接与剩余见 T3 与 §6 第 6 条。riskTier 因触派发写入面由 low-medium 升 **medium**。
    ③ **口径差异如实记**：主线程普查记「6 个负增量」，我按首末差计为 5 个、按「出现任一相邻倒退」计为 6 个——两种口径都写进验证集第 5 条与 §6 第 10 条，判官可就地复核。
11. **v0.7（human 2026-10-26 四项拍板 + R6 NIT 订正，2026-10-26）**：
    ① **OQ1 裁 (a)**（零新增记录）；**OQ3 定**（多模型去重后 `+` 连接）；**OQ4 裁 (b)**（Pod 表九列，含「其中测试行」；文档代码比分母 = 实现+测试；`--json` 另给 `ratioDocImpl`）。
    ② **OQ2 规则来源** = human 2026-10-26 原话「如果是业务系统去消费的，我们就认为它是代码；如果是 Governance 去消费的，我们就认为它是文档。」落成可机判规则：源码恒代码（含治理域源码）、Markdown 恒文档、数据/配置与二进制按消费域（治理 → 文档 / 业务 → 代码 / `docs/**` → 文档）、其余按域归类、域外落 `unmappedPaths`；十条边界判定见 T1（`changes/**` 归治理域、`tools/**` 归业务域、`.codebase-memory/**` 与根 dotfiles 落 `unmappedPaths`）。
    ③ **新口径实测**（75 个可归因包全量复算）：中位文档代码比 **1.29**；三个抽样——`261023-honcho-memory-wiring-restoration` 代码 2,109 / 文档 1,998 / 测试 1,336（比值 0.95）、`261022-m5-k4-history-corpus-backfill` 2,489 / 1,784 / 1,158（0.72）、`260829-story-p3` 5,180 / 1,848 / 3,246（0.36，另有 14 行 `unmappedPaths`）；`unmappedPaths` 全量合计 86 行（`.codebase-memory/artifact.json` 70 / `.gitattributes` 15 / `.gitignore` 1）。旧口径的两个失真数字（JSON 占代码中位 41.3%、`delivery/**` 占文档中位 92.8%）保留在 §2 作换规则的依据。
    ④ **R6 四条 NIT 订正**：模型计数统一为「11 个具名取值 + 199 条无记录（合计 12 种）」；事件总数注明为起草期快照并记 R6 复算 1,135（多 2 条非 dispatch、派生计数不变）；T1 self-check 扩到 ①–⑭，与验证集「十四条」对齐；ARQ 承接按六类实据编号对位（② actor / ③ 同决定双记 / ④ refEventId / ⑤ 时间戳；⑥ 的「声明 ≠ 实际」对账对应修法候选 ③），header 改「部分消费、根因修复保持 deferred」。
    ⑤ **riskTier 维持 medium**（触 dispatch-stamp 写入面，fail-open、零 schema 破坏、零门语义）。
12. **v0.8（human 2026-10-26 纠正边界处理，2026-10-26）**：原话「你就发现它是在 Governance 域起作用的，它就是文档；它要是在业务系统起作用的，它就是代码。你不管它是 deploy 的还是 test 的，只要在业务域起作用，那它就是代码。」据此：删除「边界判定清单（需 human 复核）」整节，改为「规则应用示例」（纯机械判定结果）；分类改为**二分域**——源码恒代码、Markdown 恒文档、数据/配置与二进制按域二分（治理 → 文档、业务 → 代码），`server/deploy/**` 与 `firmware/**/test/**` 按业务域算代码，无 deploy/test 特例；域外路径先按类型规则判定，类型未覆盖的默认按业务/工具侧算代码，`outsideKnownRoots` 只作 `--json` 诊断计数。验证集 ⑫ 改为 `outside-roots-default-code`（所有路径有确定归类、无未决桶）；其余（角色表、T3、九列表、R6 订正的模型计数与 ARQ 编号）未动。
13. **v0.9（R8 订正，2026-10-26）**：F1 选**修法 ②**——保留 human 的成文规则（治理域起作用 = 文档，`delivery/**` 下的 `test-scope-final*.txt` / `scope-list.txt` 正是治理消费产物，归文档），把原型脚本改为与成文规则严格同源后重算：中位文档代码比 **1.29**；honcho 2,109 / 1,998 / 1,336（0.95）、k4 2,489 / 1,784 / 1,158（0.72）、story-p3 5,194 / 1,848 / 3,246（0.36，代码含 14 行域外路径）。§2 域外措辞改为「先按类型规则判定、类型未覆盖的默认代码侧」，与 T1 示例（`.planning/**` markdown → 文档）不再矛盾。F2 → §6 第 3 条由「测试行不进表（OQ4=(a)）」改为「测试行进表、九列表、分母 = 实现+测试、`--json` 另给 `ratioDocImpl`（OQ4=(b)）」。F3 → header ARQ 承接改为「读侧 actor 值域分类 + 写入侧字段在场约束与告警」。F4 无动作（R6 三条已到位）。
