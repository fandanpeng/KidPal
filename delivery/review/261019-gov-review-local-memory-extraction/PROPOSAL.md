# PROPOSAL · 261019-gov-review-local-memory-extraction

> **状态：FROZEN · 2026-10-19 · 凭「human 立项 ack（含第五阀显式批准）+ architect-reviewer R1 GREEN（§7:215）」冻结，packet-bound 落地以 §2 改法集（T1–T4）+ §5 验证集为准**
> **承载 ARQ**：`ARQ-261019-local-memory-extraction-gaps`（唯一根条目，本批整体消费：缺口①②③ = T1/T2/T3；条目 5「取舍未决」= T4，human 前端已裁①搭车入批；条目 1/6 处置见 §6 留根清单，无留根指针）。
> **review 轨**：Track B 主路径（机器面〔agents / skills / templates / G1〕恒 Track B，**非**瘦制品轻路径——制品四件 + retro 照常；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节、`AGENTS.md`「治理路由」节、`.claude/references/gov-change.md`）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（四处均为文档/指令面局部增补——agent def 两句、模板自检一句、skill 一节、G1 输出标准一句；不动门脚本、不动机械判据、不触契约面。唯一升档因素 = T4 触 G1，human 前端已裁①显式授权，判档终裁归 architect-reviewer）。
> **review profile**：G2（`.claude/agents/landing-verifier.md`、`docs/templates/PROPOSAL.template.md`、`.claude/skills/kidpal-ecs-gateway/SKILL.md`）+ G1 显式触及一处（`AGENTS.md` 输出标准节，human 前端已裁①）；契约面 = 零。另注：landing-verifier.md 属 review-loop agent 定义（自改递归第五阀面），其 human-approved 由本批立项 ack 承载（见 §4 立项覆盖记录）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/landing-verifier.md
docs/templates/PROPOSAL.template.md
.claude/skills/kidpal-ecs-gateway/SKILL.md
AGENTS.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把四条只活在本机 Claude Code memory 里的治理/运维知识提取进仓内生成侧指令面——verdict 状态行形态义务进 landing-verifier agent def、「生成即验证」进 PROPOSAL 模板起草期自检、OpenClaw 2026.8.1 运维坑进 ECS gateway skill、禁「齿」具名示例进 AGENTS.md 输出标准——消除单机单点与 memory↔repo 漂移，防三类已实测事故按原频率复发。

**现状差距**：四条知识当前的唯一载体 = 本机 memory（`C:\Users\15802\.claude\projects\c--AI-KidPal\memory\`，单机单点——换机 / 重装 / headless Pod 即丢，且不回仓核验会随 repo 演化腐化；ui-visual 条已成冗余副本即实证，见 ARQ 条目 1）。仓内承载面缺口（2026-10-19 起草期实测基线，命令与输出见各 T 证据段）：landing-verifier.md 无 verdict 状态行义务（义务只钉在派遣侧简报 `track-b-verifier-brief.md:33`）；PROPOSAL 模板起草期自检块无「生成即验证」（260928 批 retro F3 已定载体未落）；kidpal-ecs-gateway SKILL.md 的 OpenClaw 运维关键词零命中；AGENTS.md 输出标准无禁「齿」具名示例（当前全文件「齿」零命中，本批为纯增量）。

**非目标（本批不做）**：不动任何门脚本 / 机械判据（`changes-index.mjs` / `landing-fidelity-check.mjs` / `scope-map.json` / `verify-gates.json` 零改动）；不触契约面；不改两个 touched 文件的 description（留根见 §6）；不专项清扫存量文档「齿」字（human 原裁「触碰时顺手改」）；不删本机 memory（批外收尾，human 已裁②，见 §6）；不为 OpenClaw 坑立新脚本（ECS `/tmp/clear_auth_failure.js` 类易丢制品不入仓，知识进 skill 即可）。

## 2. 问题、根因与改法（T1…T4）

**【硬项 · 起草期实证义务履行声明】** 本批全部行为断言均附起草期实测（命令 + 关键输出 + 日期 2026-10-19，主 worktree main 分支）：四个落地目标的现状 grep 基线现跑留痕（见各 T 证据段）；引用脚本入口均亲核在场——`debt-doctor.mjs` 实跑基线 = 「RED 零命中 —— ok」（条目 161 条：gov=111 tech=48 requirement=2），`landing-fidelity-check.mjs --preflight` 入口亲核源码 `:50-57`（`parseCliArgs` 干跑分支）并于落盘后实跑（输出见 §5 V1 留痕），`changes-index.mjs` deriveLanded 消费面读源码 `:194-228`；brief 义务句逐字引自 `.claude/references/track-b-verifier-brief.md:33` 原文；机器锚 regex 引自 lib 单源 `.claude/scripts/lib/governance-gate-shared.mjs:632`（行首判据 `^` + 可选 `>` 前缀 + 可选加粗，token 集 `PASS|GREEN|RED|FAIL|BLOCK`；landed 闭集 = `{PASS, GREEN}`，见 `changes-index.mjs:194`）。无「未实测」断言。

### T1 · landing-verifier 产出规格补 verdict 状态行形态义务

**问题**：「landing-verdict.md 必含 verdict 状态行」这一形态义务只钉在派遣侧简报模板（`track-b-verifier-brief.md:33`），而 `.claude/agents/landing-verifier.md` 的产出规格（Process-7 与 Verification 节）无此义务。agent def 是 typed agent 派发时实际注入的常驻指令面——本仓有 typed landing-verifier（brief :4 实况注记明言「Kimi 无 typed agent」降级头注在本仓不适用），派遣 prompt 若不逐字携带 brief 全文，义务即不进执行上下文、形同虚设。已实测复发一次：260929 批 verifier 写了完整 GREEN 结论但漏 token 行，INDEX 卡 frozen、主线程回派补行多跑一轮。

**证据**：
- 义务单源原文（`.claude/references/track-b-verifier-brief.md:33`，2026-10-19 起草期 Read 逐字）：「**verdict 行形态义务（260920-gov-review-verdict-anchor-align T1 钉死）**：`landing-verdict.md` 必须含一行**行首** `verdict: <TOKEN>`（TOKEN ∈ `PASS` / `FAIL(F1..Fn)`）——该行是 `changes-index.mjs` `deriveLanded` 的机器锚消费面，形态不符 = INDEX 推不出 landed。」
- agent def 现状基线（2026-10-19 起草期实跑）：`grep -c "行首\|状态行" .claude/agents/landing-verifier.md` → `0`；:34（Process-7「产出 verdict」）与 :68（Verification 节首条）均无 verdict 行形态义务（行号为本日 Read 实测锚）。
- 机器锚消费面（2026-10-19 读源码）：`.claude/scripts/lib/governance-gate-shared.mjs:632` `VERDICT_STATUS_RE`（行首判据 + 可选 `>` 引用前缀 + 可选加粗 + 可选反引号包 token），消费方 = `changes-index.mjs` deriveLanded（`:194-228`，landed = 最终 token ∈ `{PASS, GREEN}`）+ `pre-merge-escalation-gate.mjs` readGovAnchors 的 green 锚（lib 注释 `:628-629`）。
- 事故实证：260929 批 verdict 缺状态行事故（ARQ 缺口① + 本机 memory `landing-verdict-needs-status-line` 条；后果 = INDEX 卡 frozen 推不到 landed）。

**根因**：义务落错了面——只落在派遣侧简报（每环派遣时由主线程裁量携带），没落进被派遣者的 agent def（常驻注入面）。两处缺一同义锚，义务的实际生效依赖派遣 prompt 的偶然完整。

**改法**：agent def 两处追加，「引一处、不复制」——义务细则单源恒 = brief「verdict 格式」节，agent def 只写义务存在性 + 指锚，不整段照抄、不另立 token 枚举（防双源漂移）。

落点 A：Process-7（起草期 :34）行尾，锚文 = 「`**不放行 merge**。`」（该行句尾逐字），在其后追加：

```text
另，`landing-verdict.md` 必须含一行**行首** `verdict: <TOKEN>` 状态行（可带 `>` 引用前缀；TOKEN 闭集与 `changes-index.mjs` `deriveLanded` 机器锚消费面一致；形态细则单源 = `.claude/references/track-b-verifier-brief.md`「verdict 格式」节，此处不复制）——该行是 INDEX 推 landed 的机器锚，缺行 = 卡 frozen。
```

落点 B：Verification 节首条（起草期 :68）行尾追加（短指针，不复制义务本体）：

```text
+ verdict 状态行在场（行首 `verdict: <TOKEN>`，义务见 Process-7）
```

目标文件：`.claude/agents/landing-verifier.md`（改）

### T2 · PROPOSAL 模板起草期自检扩「生成即验证」第四句

**问题**：长中文单次 Write 有序列化损坏风险（乱码 / 重复行 / 噪音行混入），260928 批已两连实测、且一次险些带病入库 canonical（被写后自查接住的 near-miss）。该批 retro F3 已裁定载体 = 「登记 ARQ、作自检句③『生成即验证』扩写候选」，但 debt 账本查无登记（ARQ 本条即补登），模板起草期自检块至今仍停在三句——每批起草者照抄的生成侧指令面缺这一截，同类事故风险存续。

**证据**：
- `delivery/archive/changes/arch/260928-gov-review-cross-face-intake-spine/retro.md:38`（F3 条目：T1 长文 Write 序列化损坏 ×2，根因 = 长中文内容单次 Write 的输出序列化损坏风险 + 凭「Write 成功」自信未立即机检）；`:40` near-miss 教训句逐字：「**生成即验证，写入后立即机检自身产物，不凭工具「成功」自信**」；`:45` 建议②逐字：「**长文落地分块 + 写后自检纪律成文**：治 F3 · 载体 = ARQ 收件箱（登记为自检句③「生成即验证」扩写候选）· 预期收益 = 防序列化损坏带病入库。」
- `delivery/archive/changes/arch/260928-gov-review-cross-face-intake-spine/architect-landing-summary.md:45-47`（执行事故记录：两次输出序列化损坏均当场自查发现，处置 = 换小步分块策略、4 段干净落盘，终件 61 行 8 节噪音 grep 零命中）。
- 模板现状基线（2026-10-19 起草期实跑）：`grep -n "生成即验证" docs/templates/PROPOSAL.template.md` → 零命中（exit 1）；自检块在 :68「【起草期自检两句】」（三句形态，头部沿革注记「260924-gov-review-gate-parse-teeth T9 扩第三句——名称沿革保留、实为三句」）。

**根因**：纪律已被 retro 定稿载体但未落——账本登记缺位 + 模板自检块未扩，教训停在归档 retro 与本机 memory 两处非生成侧面。

**改法**：模板 :68 自检块两处改动（本 PROPOSAL 自身的起草已按新④句执行：分块落盘 + 写后机检，留痕见 §5 V1）。

落点 A：块头部沿革注记，锚文（逐字）= 「`260924-gov-review-gate-parse-teeth T9 扩第三句——名称沿革保留、实为三句`」，替换为：

```text
260924-gov-review-gate-parse-teeth T9 扩第三句、261019-gov-review-local-memory-extraction T2 扩第四句——名称沿革保留、实为四句
```

落点 B：③ 句末（锚文 = 「`批 A R1-F1/F2 + 批 B R1-F1/核验 F1 四实例在册）。`」句尾逐字）之后追加：

```text
④ **长文（中文、>40 行）落地分块追加、写后立即机检自身产物（grep 噪音特征 + 行数核对 + 抽读首尾），不凭 Write 返回成功自信**——260928-gov-review-cross-face-intake-spine retro F3 长文 Write 序列化损坏两连事故钉死（生成即验证）。
```

目标文件：`docs/templates/PROPOSAL.template.md`（改）

### T3 · kidpal-ecs-gateway SKILL.md 增 OpenClaw 运维节

**问题**：OpenClaw 2026.8.1 在 ECS 上的三个实测坑——凭据 sqlite 迁移死锁、计费锁（billing lock）、gateway 服务化安装器 bug——的手工解法只活在本机 memory 里；仓内运维注入面 `kidpal-ecs-gateway` SKILL.md 对这些关键词零命中。下次凭据 / 计费锁事故发生时（ECS 为共享部署面，任何会话都可能撞上），解法不可得；ECS 上 `/tmp/clear_auth_failure.js` 这类临时清理脚本也属易丢制品，不能当知识载体。

**证据**：
- skill 现状基线（2026-10-19 起草期实跑）：`grep -niE "sqlite|auth_profile|systemd|billing|doctor" .claude/skills/kidpal-ecs-gateway/SKILL.md` → 零命中（exit 1；与 ARQ 缺口③ 2026-10-19 实测一致）。
- 知识来源：本机 memory `openclaw-ecs-reinstall-2026-08-31` 条（2026-08-31 ECS 重装实测，主线程派发输入逐字持有全部技术要点）；ARQ 缺口③。
- skill 既有红线锚：SKILL.md :9「Never print/persist secrets in transcripts.」（本批新增节沿用此纪律，密钥一律不入）。

**根因**：2026-08-31 重装实测的运维知识沉淀时只写了本机 memory，未回填仓内 skill——skill 是 ECS 运维会话（server-side query / config change 路由）的实际注入面，缺节 = 知识对执行会话不可得。

**改法**：SKILL.md「Reading the engine log for a round」节之后、「Related:」行（锚文逐字 = 「`Related: [[kidpal-serial-monitor]] [[kidpal-nvs-ops]] [[kidpal-live-smoke-debug]]`」）之前插入新节。新增节保持英文（与该文件全文体例一致），密钥一律不入（沿用 :9 既有 Never-persist-secrets 红线）。插入文本 = 下列节标题行（逐字）+ 一个换行 + 围栏正文块（逐字）——两者合起来即完整新节：

节标题行：`## OpenClaw ops (2026.8.1 reinstall field notes)`

```markdown
Field-tested during the 2026-08-31 ECS OpenClaw 2026.8.1 reinstall. **Do not expect `doctor --fix` to fix credential problems.**

- **Credential migration deadlock**: pre-2026.8.1 credentials live in `~/.openclaw/agents/main/agent/auth-profiles.json`; 2026.8.1 requires them migrated into sqlite, but `doctor --fix` (interactive / non-interactive / `--force`) performs no migration, while every auth CLI is gated behind "run doctor --fix first" — a deadlock.
- **Manual fix — write BOTH sqlite stores**: (1) agent-level `~/.openclaw/agents/main/agent/openclaw-agent.sqlite`, table `auth_profile_store` (`store_key='primary'`); (2) shared-level `~/.openclaw/state/openclaw.sqlite`, table `config_machine_state` (`state_key='authProfiles.store'`). Insert the `{version:1, profiles:{...}}` JSON into both. Use `node -e` with `node:sqlite` — the ECS python3 sqlite is too old to read STRICT tables.
- **Billing lock**: after a DeepSeek balance-exhaustion 402, 2026.8.1 writes the profile into `openclaw-agent.sqlite` table `auth_profile_state` (`usageStats.<id>.disabledUntil` + `disabledReason:"billing"`, ~5h lock; topping up does NOT auto-recover; error `Prepared direct auth fallback cannot bypass unavailable profiles`). Fix = stop gateway → via node:sqlite delete the `usageStats` field from `state_json` in that table → start gateway. The failure state lives in `auth_profile_state`, NOT `auth_profile_store`; there is no CLI reset command.
- **Gateway as a service**: `openclaw gateway install` fails with SERVICE_DEFINITION_UNKNOWN (installer bug) → hand-write a system unit `/etc/systemd/system/openclaw-gateway.service` (`User=root`, `ExecStart=node .../openclaw.mjs gateway run`, env `NODE_COMPILE_CACHE` + `OPENCLAW_NO_RESPAWN=1`), then `systemctl enable --now`.
- Red line unchanged: **no secrets in this skill** (extends the Never-persist-secrets discipline above); this section stays in English like the rest of the file.
```

（呈现形态注：节标题行以行内 code 钉死、不入围栏，是因 `parseSection2AnchorLists` 的 §2 边界探测不跳围栏——围栏内 `^## ` 行会把 §2 提前截断、吞掉其后全部锚行与 T 条目（本批起草期 preflight 实测抓获，T3 缺锚误报 + T4 整条目消失）。标题 + 正文分两件逐字钉死，落地时拼接，byte 保真义务不减。该门行为是否算缺陷见 §6 留根。）

目标文件：`.claude/skills/kidpal-ecs-gateway/SKILL.md`（改）

### T4 · AGENTS.md 输出标准追加禁「齿」具名示例

**问题**：「散文禁用『齿』黑话」这一输出纪律当前只有 human 口头裁定 + 本机 memory 两个载体，未入 G1 指令根——而 AGENTS.md 输出标准节是全 agent 散文产出的统一约束面。不入根则新会话 / headless Pod / 他机协作者无从知晓，黑话会随生成量回流。

**证据**：
- human 2026-09-29 明示原话（逐字）：「以后不要用齿，验收测试 case 就说验收测试 case」（本机 memory `no-tooth-jargon-for-acceptance-cases` 条持有）。
- 取舍沿革：原裁「触碰时顺手改、不单独立项」（ARQ 条目 5），本批经 human 前端裁①升级为搭车入批具名化（触 G1，判档相应升，终裁归 architect-reviewer）。
- AGENTS.md 现状基线（2026-10-19 起草期实跑）：`grep -n "齿" AGENTS.md` → 零命中（exit 1）——本批为纯增量，无既有文本冲突；输出标准节在 :64-66，段尾句 = 「禁止读者需自猜的拼接缩写。」

**根因**：纪律停在口头裁定与单机 memory，未落成文进 G1——约束面与受众（全 agent 散文产出）不匹配。

**改法**：AGENTS.md 输出标准节（起草期 :66）段尾，锚文 = 「`禁止读者需自猜的拼接缩写。`」（段尾逐字），在其后追加：

```text
散文禁用「齿」——验收测试 case 就说「验收测试 case」（或「验收断言」），不借黑话省字（human 2026-09-29 明示原话：「以后不要用齿，验收测试 case 就说验收测试 case」）。本条只管散文：机械面标识符（如 verify-gates 内部字段名）不动；存量文档不专项清扫，触碰时顺手改写。
```

目标文件：`AGENTS.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| review-loop agent 定义 | `.claude/agents/landing-verifier.md` | G2（第五阀面） | 消费方 = 主线程每环派发的 landing-verifier（typed agent 常驻注入面）；review-loop agent 定义改动须显式 human-approved，由本批立项 ack 承载（§4 立项覆盖记录） |
| 治理模板 | `docs/templates/PROPOSAL.template.md` | G2 治理文档面 | 消费方 = 后续所有治理批起草者（照抄面：模板示范形态 = 未来每份 PROPOSAL 的形态） |
| skill | `.claude/skills/kidpal-ecs-gateway/SKILL.md` | G2 | 消费方 = ECS 运维会话（server-side query / config change / engine log 路由面） |
| 指令根 | `AGENTS.md` | G1（显式升级触及，human 前端已裁①） | 消费方 = 全 agent 散文产出；经 `@AGENTS.md` 导入传导至 CLAUDE.md，不另改 |

**sweep 透镜义务 / 复扫模式完备性先验**：均 N/A——本批不立新门、不立新断言（无新判定 pattern 可预跑）；无退役语面清扫（「齿」存量不专项清扫是 human 原裁，已在 T4 改法中显式豁免，防后续批误立清扫项——见 §6）。

## 4. human 取舍点（OQ）

本批**无待裁 OQ**——human 前端已裁毕，如实记录：

- **已裁①**：禁「齿」具名化搭车入批（T4 触 G1，判档相应升——终裁归 architect-reviewer）。
- **已裁②**：本批 landing-verdict 绿后，主线程在本机删除 4 条已提取 memory 并同步 MEMORY.md 索引——**本机操作，不属仓内 landing 清单**（批外收尾触发，详见 §6）。
- **立项覆盖记录**：landing 清单四文件由主线程按 human 前端对齐结果明示派发；其中 `.claude/agents/landing-verifier.md` 属 review-loop agent 定义（自改递归第五阀面，改动须显式 human-approved）——本批以其入列立项 ack 为显式批准的承载。判官若判立项 ack 未满足该阀，请打回升级 human（存疑向严）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit（起草期实跑，输出留痕见表后） | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261019-gov-review-local-memory-extraction/PROPOSAL.md` | exit 0 / `ok:true`；`anchorUnion` 恰 4 成员 = §2 四个目标文件锚；无 bold-anchor-line / missing-anchor findings |
| V2a | post-commit | `grep -n "行首" .claude/agents/landing-verifier.md`；`grep -n "track-b-verifier-brief" .claude/agents/landing-verifier.md` | 两条均 ≥1 命中（义务句 + 指锚同在；前者基线 2026-10-19 = 0） |
| V2b | post-commit | `grep -n "生成即验证" docs/templates/PROPOSAL.template.md`；`grep -n "实为四句" docs/templates/PROPOSAL.template.md` | 两条均 ≥1 命中（自检④ + 沿革注记同步改；前者基线 2026-10-19 = 0） |
| V2c | post-commit | `grep -ciE "sqlite\|auth_profile\|systemd\|billing" .claude/skills/kidpal-ecs-gateway/SKILL.md`；`grep -niE "sk-[A-Za-z0-9]{8,}\|Bearer [A-Za-z0-9._-]{8,}\|api[_-]?key.{0,20}[A-Za-z0-9]{16,}" .claude/skills/kidpal-ecs-gateway/SKILL.md` | 前者 ≥5（新节四要点关键词在场；基线 2026-10-19 = 0）；后者零命中（零密钥字面值——注意 "secrets" 字样本身在既有红线句与新增节红线句中合法出现，不算密钥字面） |
| V2d | post-commit | `grep -n "齿" AGENTS.md` | 命中行全部落在输出标准节新增句内（纯增量；基线 2026-10-19 = 0） |
| V3 | post-commit | `node .claude/scripts/debt-doctor.mjs` | 尾行「RED 零命中 —— ok」（pre-commit 基线 2026-10-19 起草期已跑 = ok：条目 161 条 gov=111 tech=48 requirement=2，advisory 193 条不阻断） |
| V4 | post-commit（结构门） | `node .claude/scripts/landing-fidelity-check.mjs delivery/review/261019-gov-review-local-memory-extraction` | exit 0 / `ok:true` |
| V5 | post-commit（回归防护） | `node .claude/scripts/lint-descriptions.mjs` | exit 0 且无针对两个 touched 注入面（`landing-verifier.md` / `SKILL.md`）的新增 flag——本批不动 description，此条防误伤 |

**V1 起草期实跑留痕**（2026-10-19 落盘后执行，输出逐字摘录）：

```text
ok: true, gate: "landing-preflight", section2BodyFound: true, anchorCount: 4,
anchorUnion: [".claude/agents/landing-verifier.md", ".claude/skills/kidpal-ecs-gateway/SKILL.md",
              "AGENTS.md", "docs/templates/PROPOSAL.template.md"],
advisories: [], findings: [], exit=0
```

（首跑曾 RED 一次：T3 围栏内含 `^## ` 标题行致 §2 被提前截断、T3 误报缺锚 + T4 条目消失——修复 = T3 改「标题行行内钉死 + 围栏承载正文」形态后复跑转绿，门行为观察已记 §6。）

**起草期自检④（本批新增候选句）自我适用留痕**：本 PROPOSAL 分两块落盘（Write + Edit 追加），写后机检 = `wc -l`（192 行）+ 噪音特征 grep（`\x{FFFD}` / `\x{FEFF}` / 占位标记 / 260928 批事故噪音串 `D-PWR` / `g-5d8` 零命中）+ 重复行 sanity（无异常重复）+ 抽读首尾 + `--preflight` 干跑（上方留痕）。

## 6. 留根清单

- **批外收尾触发（human 已裁②）**：本批 landing-verdict 绿后，主线程在本机删除 4 条已提取 memory——`ui-visual-change-package-granularity` / `write-serialization-corruption-guard` / `landing-verdict-needs-status-line` / `openclaw-ecs-reinstall-2026-08-31`——并同步 MEMORY.md 索引。本机操作：不进仓、不属 landing 清单、不进本 packet，删后本机与仓内单源一致（仓内为唯一承载面）。
- **ARQ 结项**：本批 landed 后 `ARQ-261019-local-memory-extraction-gaps` 四条全覆盖（缺口①②③ + 条目 5 取舍），结项动作（status → closed + 重跑 `debt-render.mjs`）归主线程按账本纪律处置，本批不代做。
- **留本机为正当（ARQ 条目 6，维持不动）**：`aliyun-ecs-ssh-access`（含明文密钥，永不入仓）、`cc-switch-model-tier-mapping`（单机代理环境，不可迁移）、`governance-naming-for-resume`（个人对外叙事，非工程事实）。
- **登记不硬治**：`kidpal-ecs-gateway` 的 description 是否补 OpenClaw 运维路由信号（authoring-agents-skills 纪律面）——本批不动 description，留后续批评估。
- **存量「齿」字不专项清扫**（human 原裁「触碰时顺手改」，T4 改法已显式豁免）：本条防后续批把「清扫存量齿」误立为新变更项。
- **登记不硬治（门行为观察，ARQ 候选）**：`parseSection2AnchorLists`（`governance-gate-shared.mjs:363`）的 §2 边界探测（`:371-374`，`/^##\s/` 找下一个节标题）**不跳围栏**——§2 内围栏块若含 `^## ` 行（如落地内容是带 `##` 标题的 markdown 新节），§2 被提前截断，其后锚行与 T 条目全部消失，报错形态具误导性（T3 被误报「缺锚清单」、T4 整条目静默消失不点名）。本批 T3 以「标题行行内钉死 + 围栏承载正文」形态规避（起草期 preflight 实测抓获、本 PROPOSAL 落笔前修复）；门是否修补（边界探测复用 `fenceLineFlags`）留后续批评估，本批不动门脚本（非目标）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-19 - architect-reviewer

**四查结论**：① 意图保真——T1/T2/T3 与 ARQ 缺口①②③ 一一对应，T4 = ARQ 条目 5 经 human 前端裁①升级搭车，human 2026-09-29 原话逐字在案；ARQ「两处不得漂移」建议以「引一处、不复制」落实，是加强而非偏离。② 纪律合规——四处均为纯增量指令面增补，路由表 / 写权边界 / 门语义零改动；T3 沿用 SKILL.md :9 Never-persist-secrets 红线，新增节无密钥字面。③ 撞车复核——无冲突：模板读法注第 1 条所称「已修复的跳围栏」指锚行扫描（`governance-gate-shared.mjs:375-384` 围栏内跳过属实），与本批 §6 所指「§2 边界探测不跳围栏」（:372-373 确以 `/^##\s/` 截断、不查 `fenceLineFlags`）是两处不同行为，PROPOSAL 描述准确无自相矛盾；AGENTS.md / 模板 / agent def / SKILL.md 四处均纯增量。④ 牵连面完整——manifest == anchorUnion == 四 T 目标文件并集（亲跑 preflight 核）；V5 防误伤 lint-descriptions 已列；批外收尾（删 memory）与仓内 landing 清单边界划清。

**findings**（无 BLOCKING；两条 NIT 不阻断）：

- **F1（NIT）· V2c 命令含 markdown 表格转义符**：`grep -ciE "sqlite\|auth_profile\|..."` 中 `\|` 是表格单元格内的管道转义（GFM code span 内 `\|` 渲染为 `|`），landing-verifier 复跑时须以未转义 `|` 交替形态执行，否则模式匹配字面竖线恒零命中、假 FAIL。意图无歧义（起草者基线即以未转义形态实跑证成），不阻断；建议 landing-summary 诚实注记段提一句，防零上下文 verifier 照抄转义形态。
- **F2（NIT）· T4 豁免句例证与实测分布不一致**：T4 追加句举「如 verify-gates 内部字段名」为机械面标识符例，实测 `.claude/verify-gates.json` 「齿」零命中；机械面实际含「齿」处在 `.claude/scope-map.json`（「T1 齿一」）与 `.claude/scripts/**` 注释（如 changes-index.mjs「T5 投影齿」、closeout-check.mjs「T6 回显齿」）。例证作前瞻性类别示例可成立（「如」不断言现存），且「存量不专项清扫」已兜底，不阻断；landing 落笔时可顺手把例证换成实测在案处，或保留不动（措辞级裁量归落地者，不构成打回）。

**起草者自报两点复核**：① T3 规避形态成立——§2 边界探测缺陷亲核源码证实（:372-373），「标题行行内 code 钉死 + 围栏承载正文」两件拼接落地，byte 保真义务不减（围栏正文逐字、标题行逐字，落地时机械拼接即可对账）；门不修、记 §6 ARQ 候选 = 登记不硬治，与本批「不动门脚本」非目标自洽，处置得当。② preflight 复跑亲核 exit 0 / ok:true / anchorCount 4 / anchorUnion 与 manifest 恰一致，与 §5 V1 留痕一致；debt-doctor 基线复跑尾行「RED 零命中 —— ok」（161 条：gov=111 tech=48 requirement=2）与 §2 硬项声明一致。

**第五阀承载核**：T1 触 `.claude/agents/landing-verifier.md`（review-loop agent 定义），§4 立项覆盖记录在场且与主线程派发输入的 human 显式批准一致——承载成立。

**判档结论**：**Track B 主路径成立**（判据引用：`.claude/references/gov-change.md`「轻路径判据」③——T1 触 `.claude/agents/**`、T4 触 `AGENTS.md`（G1），任一即恒 Track B，非轻路径；riskTier low 维持，两处敏感触点〔G1、第五阀〕均有人类显式凭据承载）。Track A 段不触发：方向已收敛（ARQ 预消化 + 修法无设计分叉 + human 立项 ack 在案），判据只看收敛度、不看触及面。

**升级 human 项**：无。

**结构字段核对**：landing-manifest 围栏块（:12-17）与 packet-id 行（:6）在场。

**亲核留痕**（2026-10-19，主 worktree main）：preflight 复跑 exit 0（anchorUnion 4 成员 = manifest）；`grep -c "齿" AGENTS.md` = 0（exit 1）；`grep -c "生成即验证" docs/templates/PROPOSAL.template.md` = 0（exit 1）；`grep -c "行首\|状态行" .claude/agents/landing-verifier.md` = 0（exit 1）；SKILL.md 关键词 grep = 0（exit 1）；`VERDICT_STATUS_RE` :632 / `deriveLanded` 闭集 {PASS,GREEN} :228 / `parseSection2AnchorLists` :363+:372-373 亲读源码；`track-b-verifier-brief.md:33` 义务句逐字核；T1 锚 `**不放行 merge**。`（agent def :34 句尾，加粗形态全文件唯一）、T2 锚两条（模板 :68 逐字）、T3 锚（SKILL.md :46 Related 行逐字）、T4 锚（AGENTS.md :66 段尾句逐字）均亲读核；T3 技术要点与本机 memory 源条 `openclaw-ecs-reinstall-2026-08-31` 逐项吻合（两 sqlite 表 / 死锁机理 / 计费锁表与清理法 / SERVICE_DEFINITION_UNKNOWN / systemd unit 要点）；260928 批 retro :38/:40/:45 与 landing-summary :47 逐字核；debt 账本 F3 前期登记查无（仅本批 ARQ 命中）。

verdict: GREEN
