---
id: ARQ-261015-lite-package-execution-overhead
title: Lite 包执行成本与代码改动量严重不成比例——3 行改动端到端 2h52m，环境/重复/机制开销远超代码，待完整治理方案
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批完整设计「lite 快路径」时（或 human 明确推进 lite 包执行优化）
opened: 2026-10-15
paths:
  - .claude/skills/kidpal-pod-launch-prep/SKILL.md
  - .claude/commands/build-feature.md
  - .claude/agents/planner.md
  - .claude/references/task-intake-and-slicing.md
  - .claude/rules/repo-structure.md
  - .claude/scripts/brief-inputs.mjs
  - .claude/verify-gates.json
  - .claude/hooks/verify.mjs
  - .claude/scripts/firmware-build-check.mjs
  - .claude/hooks/enforce-scope.mjs
source:
  - delivery/taskpackages/261015-voice-rightcol-label-overflow/exit-report.md
  - delivery/taskpackages/261015-voice-rightcol-label-overflow/pod-events.jsonl
  - delivery/taskpackages/261015-voice-rightcol-label-overflow/verify-runs.jsonl
  - delivery/taskpackages/261015-voice-rightcol-label-overflow/verifier-scope.json
  - delivery/changes/dev/261015-voice-rightcol-label-overflow/pod-retro.md
  - docs/debt/gov/ARQ-261015-launch-prep-host-only-env-defer.md
---

**证据**：261015-voice-rightcol-label-overflow（3 行 LVGL label 宽度 264→196）实测时间线——human 会话贴图报障 10:30:45 → 真机确认「正常了」13:22:52 = **2h52m07s**；「走分流开始修」10:49:28 → 「merge 到 local main」13:24:03 = 全程 2h54m44s。其中真正写代码 ~10 分钟（且 3 行在 builder 派发前已在位）。时间去向：分流立项 ~18min → 环境/worktree ~50min → 验收冻结（并行线写 16m25s + canonical 核 3m16s）→ 冷构建+返工 ~25min → 验证+机械闸 ~45min → 真机轮 ~14min；另有一条**并行线 2h01m 全程白跑**（11:08→13:09，产物全部丢弃）。human 体感「一个多小时」= 从代码改完（12:20）到真机确认（13:22）的 1h02m，与墙钟 2h52m 差约一倍——「改几行代码」本身约 1 小时，但把这件事办完近 3 小时。

五处浪费源（各有独立根因、独立落点）：

- **A 环境准备从头做（~50min）**：host-closable 包为用不上的 IDF 环境支付 launch 面全套成本（worktree + 固件环境等价化 + per-worktree 图索引 + 作用域引导）。**已在 [[ARQ-261015-launch-prep-host-only-env-defer]] 登记，本条只交叉引用、不重述。**
- **B 双 input-brief 冗余 + 内容无关**：builder 与 acceptance-author 两份 brief 逐字相同（diff 只差 4 行 §①c，内容还是 `none`）；且 §①b 把 S-F1.3 整节照录、因包声明 AC1 与 AC3 两个 id 被**贴了两遍**（36 行），全是「孩子问课表」需求散文，与「label 宽 264→196」无关。真正的改动指令（264→196 + 三处行号）在 plan.md，brief 对 plan.md 只给一行 `bare` 指针。根因 = `brief-inputs.mjs` `renderBrief()`（role 只影响标题+§①c）+ `renderStoryRoad()`（按完整 id 去重、每 id 整节照录）。
- **C firmware-build 门每轮真编译 IDF（5 轮累计 323s）**：该 gate `timing:scoped`（verify-gates.json:52），每轮 verifier 无条件跑；`gateProfile` 字段对选闸**零机械作用**（全仓无脚本读它选闸）；选闸唯一咽喉 = `verify.mjs:108-113` `gateActive()`（对包内容零感知）。语义红线：判据不能是「验收走 host」（261015 验收走 host 但改的是设备源 lcd_display.cc，host_app 是独立 linux target 不编译 esp32s3），须以「changeManifest 是否触设备编译面」为界，且是**后置/合并到终审或真机轮一次**而非跳过——它是设备 C++ 编译唯一的机械信号。
- **D worktree 派发被 enforce-scope 卡死**：原 Pod 从 pod-m2 cwd 启动却建兄弟 worktree，`enforce-scope.mjs:130` 以 `CLAUDE_PROJECT_DIR` 为基准解析路径（:164-165），所有子 agent 写 worktree 路径被判 `../KidPal-pod-...` 越界全部拦截——烧 5.6min 探针后退化成「planner 自己往 m2 树写」，直接造成与 canonical 线双树写互踩（taskpackage.json 两边同改）。这是影响所有用 worktree 的包的机制 bug，非 lite 专属。
- **E 「叫停」只删 worktree、没停进程 → 2h 并行线白跑**：叫停动作删了 worktree 与分支，但 headless planner 会话仍在跑，一路跑完整条 loop、13:09 才发现 canonical 线已交付、自行拆除归档（遥测见 /c/tmp/261015-parallel-incarnation/）。重派指令的「在途存量」实际正是这条没被杀干净的线的产出——省下的 16min 验收件是捡来的，不是设计出来的。

**影响面**：所有 host-closable 小包（显示层 / 纯逻辑 seam / host 齿可闭环的 Lite bug/小 feature）拉 Pod 时都要为用不上的 IDF 环境支付 launch + verify 面成本；包越小占比越荒谬（本包 3 行改动）。不修的代价：小包吞吐被拖慢，human 对「小活拉 Pod」望而却步、倾向绕过 dev-loop 直修——恰是治理路由想防的。D/E 影响面超出 lite（所有用 worktree 的包、所有叫停动作）。

**建议改动**（候选，交 review、不自批；本批分叉未裁，见风险段）：立「host-closable lite 快路径」——A（launch 面环境后置，方案见 [[ARQ-261015-launch-prep-host-only-env-defer]] 建议改动）+ B（brief 合并为单份+角色增量；bug 类包 brief 带改动摘要而非 story 整节照录）+ C（firmware-build 门对 host-closable 包后置到真机轮，真机轮加设备编译预检）。D/E 是影响所有包的机制 bug，不塞进 lite 快路径，另行登记或随治理批一并修。真设计分叉：① 资格判据谁裁、何时判（triage architect 申报 vs 机械判）；② 后置点锚定（真机轮入口「环境就绪」预检——当前全仓不存在该锚点）；③ firmware-build 改法（窄改仅 host-closable 跳过 vs 宽改统一后置）。

**风险**：riskTier: medium——改治理机制面（launch 面 + verify 面闸语义），影响所有后续 lite 包的环境就绪与设备编译验证语义；D 若纳入还触及 enforce-scope 承重面。**review profile：full-adversarial + human（Track A）**——Track B 三要件中 ①执行测出（yes，本条证据段）③爆炸半径已执行揭示（yes，实测时间线）齐，但 ②「修复明确无真设计分叉」不成立（资格判据/后置锚点/改法宽窄三处需 human 拍板），按「存疑向严」如实标 full，不走 Track B 快道。owner = architect（起草候选）→ human 裁定 → review-loop 落地。
