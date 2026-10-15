# PROPOSAL · 261015-gov-review-lite-fast-path

> **状态：FROZEN v0.3 · 2026-10-15 · 主线程标（凭：human 立项 ack 七条 + R1 评审 GREEN〔0 blocking · 3 should · 1 advisory，F2/F3/F4 已核对折进本版、F5 顺手修〕）· 评审记录见 §7 · packet-id = §2 T1–T6 + §5 验证集**
> **承载 ARQ**：`ARQ-261015-lite-package-execution-overhead`（主条目，五浪费源 A~E 全收）+ `ARQ-261015-launch-prep-host-only-env-defer`（A 源姊妹条目，随批闭合）；另承接 tech 域条目 `TECH-261014-host-app-sdkconfig-env-drift` 的修法方向①（host_app sdkconfig 纳入镜像清单，落 T2，随批结项由主线程/渲染器归位）。
> **review 轨**：Track B 常驻轻路径（主路径五段式全件；判档依据见 §0 末小节）。SSOT：`.claude/references/gov-change.md`；`AGENTS.md`「治理路由」节。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium——改治理机制面（launch 面纪律 + verify 面闸语义 + enforce-scope 承重钩子），但闸向严不减（final 相位十闸恒全量、defer 缺省 fail-open 维持现状）、契约面零触、行为变化有界（仅 `rigor=lite` 包触发新语义）。
> **review profile**：G1+G2+G3——G1 = `docs/DEV-GOVERNANCE.md` 一句级改动（OQ-2 已裁 (a)，human 2026-10-15）；G2 = references/agents/skills 九件；G3 = hooks/scripts/verify-gates.json 七件；契约面零触。

```landing-manifest
.claude/agents/acceptance-author.md
.claude/agents/builder.md
.claude/agents/planner.md
.claude/hooks/enforce-scope.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/hooks/verify.mjs
.claude/references/change-triage.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/brief-inputs.mjs
.claude/scripts/firmware-build-check.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/verify-gates.json
docs/DEV-GOVERNANCE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：host 齿可闭环的 lite 包（显示层 / 纯逻辑 seam 的小 bug 与小 feature）走 dev-loop 时，launch 面与 verify 面不再为用不上的 IDF 设备环境付费——同类 3 行改动的 lite 包端到端从实测 2h52m07s 压向 1 小时内的量级。

**现状差距**（证据锚 = `docs/debt/gov/ARQ-261015-lite-package-execution-overhead.md` 实测时间线，261015-voice-rightcol-label-overflow 包，3 行 LVGL label 宽度改动）：launch 面环境准备 ~50min（A）；builder 与 acceptance-author 两份 input-brief 逐字相同且 story 节被整节照录两遍共 36 行无关散文（B）；firmware-build 闸 scoped 相位每轮真编译 IDF，5 轮累计 323s（C）；兄弟 worktree 派发被 enforce-scope 钩子全拦，烧 5.6min 探针后退化成跨树双写互踩（D）；「叫停」只删树没停会话进程，一条并行线 2h01m 全程白跑（E）。

**预期收益分解**：launch ~50min → 分钟级（池树复用近零成本、新树环境后置）；scoped 相位编译 323s → 0（final 相位一次保留）；双 brief 的生成与阅读成本减半；D 的探针与互踩消除；E 的白跑消除。

**非目标（防 scope creep）**：不为「full 但 host 可闭环」的包立新资格字段（复议触发器留 §6）；不实现 pod-stop.mjs 自动化脚本（§4 OQ-1）；不追溯清理存量包的双份 brief 历史制品；不改 merge-driver 冒烟闸集构成。

**不动清单（本批不变式，落地核验时逐条对照）**：

1. 验收先行 + 冻结 + 红基线一道不跳——「Lite 省的是方案/决策映射的文牍，不是 loop 的任何一道闸」（`task-intake-and-slicing.md`「rigor 字段与 Lite TaskPackage 字段级定义」节，2026-10-15 快照 :242）原样成立；
2. mutation 取证义务不变；
3. final 相位十闸全量不变，含 firmware-build——它是设备 C++ 编译唯一的机械信号，本批做的是 **scoped 相位后置 ≠ 跳过**，final 恒跑；
4. supervisor 机械闸（`supervisor-verdict-gate.mjs`）+ 前置升级闸（`pre-merge-escalation-gate.mjs`）判据零改动（T4 只动前者 self-check 夹具与注释，不动判定逻辑）；
5. 真机轮 human 持板不变——快路径把固件环境补齐义务钉到真机轮前，不是取消它；
6. 契约面零触——本批不改 `docs/contracts/**` 任何文件，也不改契约防线的任何条款；
7. 图索引「launch 面必办、不得降级读文件」（ARQ-260905）的**必办性与阻塞性均不变**——本批对 ARQ-260905 零修订（OQ-3 裁 (c)，human 2026-10-15）：专属图索引仍须在 intake 前在册，lite 快路径不开例外；快路径的 launch 面收益只来自池优先与新树固件环境等价化后置两件。

**判档依据（如实记录）**：两条 ARQ 原文自评 `review profile: full-adversarial + human（Track A）`，其唯一依据是「修复明确无真设计分叉」不成立（资格判据 / 后置锚点 / 改法宽窄三处分叉）。该三分叉已由 human 于 2026-10-15 立项 ack 逐条裁定闭合（资格判据复用 rigor 不新立字段；后置锚 = final 相位机械牙 + 启动卡真机轮预检行双锚；firmware-build 窄改仅 lite 包 scoped 后置），方向已收敛、不再开口——按 `gov-change.md`「Track A 段判据只看收敛度，不看触及面」，Track A 判据①不再命中（判据② reviewer 评审中升级与判据③声明红队保留为逃生门）。同时本批触 `.claude/agents/**`、`.claude/hooks/**`、`.claude/scripts/**`、`.claude/verify-gates.json`，按轻路径判据（路径即判据，命中任一恒 Track B）**轻路径不适用**，走 Track B 主路径五段式全件（PROPOSAL + landing-summary + landing-verdict + retro 四件，retro 不豁免）。

## 2. 问题、根因与改法（T1…T6）

> 以下 file:line 行号均为 2026-10-15 本分支（gov/261015-gov-review-lite-fast-path）起草时点快照，搬运者自负复证责任。

### T1 · triage 判注成文：rigor=lite ⇒ 快路径默认适用 + host 可闭环确认义务

**问题**：`rigor` 字段现有定义只覆盖「文牍从简」维度（哪些字段可省），launch 面环境成本与 verify 面闸语义不随 rigor 走——host 齿可闭环的 lite 包仍支付全套 IDF 环境准备与 scoped 相位每轮真编译。triage 判 lite 时也没有「host 可闭环性」确认义务，快路径资格无人裁、无落点、无例外声明通道。
**证据**：`change-triage.md` §2 四档表（2026-10-15 快照 :23-28）Lite 行判据 = 「单一来源/紧耦合小簇 · 单 owning 包 · 无悬挂契约决策 · 无迁移删数 · 一轮收」，无环境语义；§7（:100-102）只有一句指针指向字段级定义。ARQ-261015-lite-package-execution-overhead 证据段：3 行改动端到端 2h52m07s，环境/worktree ~50min。
**根因**：rigor 字段的消费面只有文牍维度（task-intake-and-slicing.md「rigor 字段与 Lite TaskPackage 字段级定义」节，:238-253），launch/verify 两面没有任何机制读它。
**改法**（human 2026-10-15 立项 ack 第 1 条已裁，如实记录不重开）：资格判据**复用 rigor、不新立字段**——`rigor=lite` ⇒ 快路径默认适用，机器只读 rigor。① `change-triage.md` §2 四档表 Lite 行判据列末尾补「⇒ 快路径默认适用（§7）」；② §7 扩「lite 快路径」段：`rigor=lite` ⇒ 快路径（launch 环境后置 + firmware-build scoped 相位后置，机制分别见 T2/T3）默认适用；**triage 判注义务**——architect 判 lite 时 rationale 须含「host 齿可闭环」确认句；若 lite 但设备编译面深（host_app target 盖不住的 seam），须注明「快路径不适用、按 full 环境备」；**复议触发器** = 出现首个真实「full 但 host 可闭环」的包时再评是否立新字段（YAGNI）；③ `task-intake-and-slicing.md` 的 Lite 字段级定义节补一句交叉引用（lite 快路径语义锚 = `change-triage.md` §7 + launch-prep SKILL 的 lite 分支），字段级定义本体一字不动。
目标文件：`.claude/references/change-triage.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）

### T2 · launch 面 lite 快路径：池优先 + 新树环境后置 + 图索引异步不阻塞

**问题**：host-only 可验的 lite 包 launch 面支付全套环境成本（实测 ~50min 量级：worktree + 固件环境等价化 + 作用域引导）；TECH-261014 记的 host_app sdkconfig 镜像缺位没有落点（该条目修法方向①明示「排期归启动面批」，本批即启动面批）。另：261015 D 事故暴露的纪律缺口——会话从哪棵树启动却在兄弟树干活——无成文条款。图索引一项**不是**本批要消解的痛点：ARQ-261015-launch-prep-host-only-env-defer 记的「图索引全量索引（本仓规模十几分钟级）」与 SKILL.md 步骤 5 现文「快照导入 + 增量，分钟级、近零成本」（main 已 commit 快照 artifact，b7aa3df 起）相互矛盾——按「疑似快照流漂移、留根观察」口径如实呈现（见 §6 留根），不夸大不掩盖；OQ-3 裁定（human 2026-10-15）= 快照导入 + 增量的设计成本为分钟级、阻塞可接受，规则保持「launch 面必办、intake 前在册」原样。
**证据**：ARQ-261015-launch-prep-host-only-env-defer 全条（时间线：intake 尽调 ~5min → 建包 ~1min → worktree 秒级 → 作用域引导与后续环境活分钟级起）；`kidpal-pod-launch-prep/SKILL.md` 步骤 4（2026-10-15 快照 :77-83）固件环境等价化无例外分支；产物清单第 5 件（:18）「图索引 launch 面必办，不得降级读文件」；`.claude/rules/repo-structure.md` `.codebase-memory` 行「图索引项目归属」段同口径。起草期实测（2026-10-15，git ls-files / check-ignore）：`firmware/sdkconfig` 是 tracked 文件，`firmware/dependencies.lock`、`firmware/main/test/host_app/sdkconfig`、`firmware/managed_components` 均 gitignored；`firmware/main/test/host_app/sdkconfig.defaults`（tracked，37 行）已含 `CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK=y`（grep 实证）——**新树 host 冷构建从 defaults 重生成即得当前配置**，TECH-261014 假红类的真实暴露面是「携带陈旧生成态的复用树/main」，不是全新树。
**根因**：launch 面无 rigor 感知，一例一态——所有包走同一条全量环境流水线。
**改法**（立项 ack 第 2、6 条 + OQ-3 裁定 + TECH-261014 承接）：

1. SKILL.md 增「lite 快路径」分支（触发 = 包 `rigor=lite` 且 triage 未声明「快路径不适用」）：**池优先**——先按 260915 换包协议复用空闲池树（复用树自带热环境：build/ 缓存 + managed_components 软链 + 既有图索引，环境后置对它无意义），无空闲树才经 human 决策建新树（换包协议第四条「建新树决策归 human」不变）；**新树免做的只有一件 = 固件环境等价化**（步骤 4 全件后置为真机轮前置条件，启动卡钉预检行「烧录前补齐固件环境等价化，或退主 checkout 构建烧录」——约定层，对偶 T3 的 final 相位机械牙 = 双锚）；**专属图索引维持「必办 + intake 前在册」原样**（OQ-3 裁 (c)，ARQ-260905 零修订），SKILL 增一条执行顺序注记：新树建成后**立即发起**专属图索引（`index_repository` 快照导入 + 增量，分钟级），与 board 预填 / 启动卡撰写等分钟级步骤自然交叠，intake 前必须在册。**产物清单（完成标准）第 4 件「每个包：固件环境等价化」（2026-10-15 快照 :17）同步补 lite 例外限定词**（R1-F3 折进）——形态如「（lite 包新树：后置真机轮前，见 lite 快路径分支）」；不补则 frozen 文本的完成标准与 lite 分支自相矛盾。
2. 步骤 4 镜像清单增 host_app sdkconfig 一行（`firmware/main/test/host_app/sdkconfig`，TECH-261014 修法方向①落地，随批结项）；该镜像义务在 lite 新树上随步骤 4 整体后移——镜像清单是步骤 4 的内部清单，后置条款只写在步骤 4 入口层，两条款同一落点、不打架（ack 第 2 条「防两条款打架」的落实方式）。池树交接卫生白名单注记补一句：host_app/sdkconfig 属 gitignored 再生面、不随树保留——换包交接时删除之，强制下次构建从 tracked sdkconfig.defaults 重生成（消 TECH-261014 陈旧假红类；新树无此险，见证据段实测）。
3. 坑节补纪律线成文：「**会话从哪棵树启动就在哪棵树干活**」——261015 D 事故根因（从 pod-m2 启动却建兄弟树派发，enforce-scope 全拦后退化成跨树写互踩）；跨树干活的合法形态 = 在该树起自己的会话，不是从本会话往兄弟树写。
4. `planner.md` Step 1 增 rigor 感知句：lite 包在快路径树上时，firmware-build scoped 轮报 notExecuted 是设计态（final 恒跑，机制见 T3），verifier 报告点名义务照 DEV-GOVERNANCE「验收测试纪律」§1 既有条文履行；loop 中段发现设备编译面需求（host_app 盖不住的 seam，如扩单触设备专有编译单元）→ 停，回 launch-prep 步骤 4 补环境或升级 human，不硬跑。

目标文件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）· `.claude/agents/planner.md`（改）

### T3 · verify 面：firmware-build 对 rigor=lite 包 scoped 相位后置（声明式 defer + notExecuted 上收，不假绿）

**问题**：firmware-build 闸 `timing:scoped`，每轮 verifier 无条件真编译 IDF（261015 包 5 轮累计 323s）；选闸唯一咽喉 `gateActive()` 对包内容零感知；TaskPackage 的 `gateProfile` 字段对选闸零机械作用（全仓无脚本读它选闸）。
**证据**：`verify-gates.json` firmware-build 条目（2026-10-15 快照 :49-54，`"timing": "scoped"`）；`verify.mjs` gateActive（:108-113）原文：

```js
function gateActive(gate, phase) {
  const timing = gate.timing || 'scoped'; // 未标默认 scoped
  if (timing === 'scoped') return true;
  if (timing === 'full' && phase === 'final') return true;
  return false;
}
```

ARQ-261015-lite-package-execution-overhead C 源（5 轮 323s）。语义红线（ARQ 原文钉死）：判据不能是「验收走 host」——261015 包验收走 host 但改的是设备源 lcd_display.cc；本批按 ack 裁定以 `rigor=lite` + triage 判注（T1）为界，**后置到 final 相位而非跳过**。
**根因**：选闸咽喉没有包上下文输入，timing 是唯一维度。
**改法**（立项 ack 第 3 条，窄改：仅 `rigor=lite` 包 scoped 相位后置，final 相位恒跑不变）：

1. `verify-gates.json` firmware-build 条目加声明式 defer 条件字段，形态：

```json
"defer": { "whenRigor": ["lite"], "phases": ["scoped"] }
```

条目 `_doc` 增补 defer 语义说明；顶层 `_doc` 的 timing 段补 defer 段（defer = 指定 rigor 档包在指定相位后置，缺省不 defer）；merge-driver 冒烟注记口径核对——「firmware-build 有意不入冒烟……由 verifier scoped 相位承担」一句（:32）与 `pod-coordination.md` §6 同文句（:88）同步改为「由 verifier scoped 相位承担（rigor=lite 包后置 final 相位）」。merge-driver 冒烟闸集构成不变（QUICK_CHECK_IDS 不动）。
2. `verify.mjs`：runGateChild 在 spawn 各闸前判 defer——读 `delivery/taskpackages/current/taskpackage.json` 的 `rigor`（**fail-open**：文件读不到 / rigor 字段缺席 / 闸无 defer 字段 = 不 defer = 维持现状；读法对偶 persistVerifyRuns 既有同款，:144-146）；命中（闸 defer 字段的 whenRigor 含包 rigor 且当前相位 ∈ defer.phases）即给该闸的 spawn env 注入 `VERIFY_GATE_DEFER=1`（附 `VERIFY_GATE_DEFER_REASON` 溯源串）；**final 相位永不注入**（defer.phases 不含 final 是配置事实，代码层再加一道相位硬判双保险）。判定逻辑抽纯函数 `shouldDeferGate(gate, phase, rigor)` 供 self-check 直调。
3. `firmware-build-check.mjs`：decide() 增 deferred 输入臂——`VERIFY_GATE_DEFER=1` 时**优先于一切**（含 VERIFY_FW_BUILD_REQUIRED=1 严格模式）返回 `{ ok: true, phase: "executor-skipped", reason: "rigor=lite 包 scoped 相位后置（final 相位恒跑；261015-gov-review-lite-fast-path）" }`，exit 0，短路先于工具链探测。`"phase": "executor-skipped"` 命中 verify.mjs `liftNotExecuted`（:184-194）既有解析 → 上收为 results[] 的机器可判 `notExecuted: true` + `notExecutedReason`，进 summarizeNotExecuted 的 `gatesNotExecuted`、verdict.json 与 round fact——**不假绿**：「没跑」全程机器可见，verifier 报告逐条点名义务由 DEV-GOVERNANCE「验收测试纪律」§1（:268）既有条文承载、不改。**优先级成文**：defer > required——strict mode 治的是「工具链缺失时静默降级」，defer 是「显式声明的不执行 + 机器标记」，语义不冲突但顺序须钉死（launch 卡固件严格模式声明行随卡走不变）。
4. self-check 补臂两脚本：verify.mjs 增纯判定臂（lite+scoped → defer；lite+final → 不 defer；full+scoped → 不 defer；包上下文缺席 → 不 defer；闸无 defer 字段 → 不 defer）+ **接缝臂**（spawn firmware-build-check 带 `VERIFY_GATE_DEFER=1`，stdout 喂 liftNotExecuted 断言 `notExecuted===true` 且 reason 非空——hermetic，defer 短路先于工具链探测，零环境依赖）；firmware-build-check.mjs 增 deferred 臂（executor-skipped 形状 + reason 点名）+ deferred∧required 优先级臂（defer 赢）。
5. `dev-loop.md` §5 验证阶梯表 firmware-build 行注记：rigor=lite 包 scoped 相位后置、final 恒跑、闸报 notExecuted 为设计态而非故障。

**实现路线取舍**（立项 ack 授权二选一，本 PROPOSAL 定稿）：选 **env 透传**、弃 verify.mjs 合成行——合成行路线要在 verify.mjs 内新立「没跑却产 results 行」的第二产线，与「results[] = 真跑闸记录」的既有语义有张力，且 liftNotExecuted 上收链用不上；env 透传让闸自报 executor-skipped，上收链**字面复用**、零新解析分支，两脚本各自的 self-check 臂也能各自闭合。
目标文件：`.claude/verify-gates.json`（改）· `.claude/hooks/verify.mjs`（改）· `.claude/scripts/firmware-build-check.mjs`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/pod-coordination.md`（改——仅 §6 冒烟注记一句同步）

### T4 · brief 面：story 基 id 去重 + 双份并单份 + bug 类包 story 照录降级

**问题**：builder 与 acceptance-author 两份 brief 逐字相同（261015 包实测 diff 只差 4 行 §①c 且内容是 none）；story 节按完整 id 去重导致同一 story 节因包声明 AC1/AC3 两个 id 被整节照录两遍（36 行与改动无关的需求散文）；bug 类包真正的改动指令在 plan.md，brief 对 plan.md 只给一行 bare 指针。
**证据**：`brief-inputs.mjs` renderBrief（2026-10-15 快照 :417）中 role 只影响标题行（:430 `# input-brief · ${role} · ...`）与 §①c builder 专列（:454）；renderStoryRoad（:99-132）去重键 = 完整 id 字符串（:100-105 `seen.add(id)`），AC 复合句法不合并；输出行 :761 `input-brief-${args.role}.md`。ARQ B 源。消费面普查（起草期 grep `input-brief` 全仓枚举，2026-10-15）：除归档债条目与 ARQ 本体外，命中面 = acceptance-author.md / builder.md / planner.md / portfolio-planner.md / supervisor-verdict-gate.mjs / dev-loop-operating-system.md / dev-loop.md / task-intake-and-slicing.md / brief-inputs.mjs / taskpackage-required-check.mjs / launch-prep SKILL.md / DEV-GOVERNANCE.md（逐一读过原行判定改/不改，见改法与 §3 表）。
**根因**：role 维度无实质内容差异却产双份文件；去重键设计粒度错（完整 id 而非基 id）；story 照录路不分包的来源类型——bug 类包的语义权威是 plan.md 的修复指令，story 整节对它只是背景。
**改法**（立项 ack 第 5 条）：

1. **story 节按基 id 去重**：renderStoryRoad 分组键改为剥掉 `-AC<n>` 的基 id——`S-Fx.y-AC1` + `S-Fx.y-AC3` → 同节只贴一次，AC 粒度回引标签并列列出；story 级（无 AC 后缀）与 AC 级引用同基 id 时同样合并。
2. **双 brief 并单份 `input-brief.md`**：`--role` 参数退役（ROLES 闭集与 main() 的 role 校验一并撤）；§①c UI 设计 refs 节保留、头部标注「本节主要消费方 = builder」（acceptance-author 直读 TaskPackage uiux block 的既有口径不变）；brief 头部标题与「消费方」行去 role。消费面同步改：
   - `taskpackage-required-check.mjs`：finalize 态「两份 input-brief-<role>.md 在场」判定（:545-553）改单份 `input-brief.md` 在场，头注（:12-15）同步；
   - `planner.md`：Step 7「一次产双份」改「一次产单份」（跑一遍不带 `--role`）；Step 8 硬前置「两份在场」改「一份在场」；Step 9 引用 `input-brief-builder.md` 改 `input-brief.md`；Step 5 的 current/ 投影纪律 ③「brief 双份投影」改单份；[JOURNAL-APPEND] 块（:74）的 `input-brief-<role>.md` 引用改单份文件名（与 dev-loop-operating-system.md :122 同文句，两处同步——R1-F4 折进）；Step 1e（:47）「与 input-brief 的 story 照录投影同源」补限定半句——bug 类包 brief ①b 降级为指针形态后，该句仅对 proposal/story 来源包全等为真（R1-F4 折进）；
   - `acceptance-author.md` / `builder.md` 输入面引用改单份文件名，并补半句：bug 类包的 brief ①b 为指针形态、改动指令源在 plan.md（见改法 3）；
   - `dev-loop.md`：§2 文件地图 brief-inputs 行（:89）「一次产双份」改单份；§3 变更制品 brief 行（:135）双文件名改单文件名；
   - `dev-loop-operating-system.md`：over-scan 触发定义（:122）的 `input-brief-<role>.md` 引用改 `input-brief.md`；
   - `docs/DEV-GOVERNANCE.md`「起包格式期望前置」第六条 ①（:311）「一次产双份 input-brief（`input-brief-acceptance-author.md` / `input-brief-builder.md`……）」句改单份口径——**G1 一句级改动，见 §4 OQ-2**；
   - `supervisor-verdict-gate.mjs`：self-check 假阳防护夹具（:736-746）中 `input-brief-acceptance-author.md` 路径名**保留**作合成回归锚（`*auth*` 路径轴保护必须仍是活的——acceptance-author 作为 agent 名存续，未来制品路径仍可能带 author 子串；夹具测的是排除闭集机器、不是文件存在性），:143/:203/:242 三处注释补一句更名注记，并增一臂断言新名 `input-brief.md` 不触发（平凡真但钉住意图）。判定逻辑零改动。
   - **过渡注记**：存量已 merged 包的双份 brief 是历史制品，不追溯清理；在飞包（落地时点 = 261015-voice-rightcol-label-overflow，已 supervisor APPROVE 停真机门）若落地后需补跑 verifier 轮，planner 先重跑一遍 `brief-inputs.mjs` 产单份 brief 再过闸——一句话成本，不构成阻断。
3. **bug 类包 story 照录降级**：`sourceType ∈ {regression, ci-failure, drift-report}` 且 `planRef` 在场时，①b 渲染「指针 + 声明 AC 行」（story id → 语义权威 = `docs/requirements.md` 现文，不照录；本包声明的 AC 回引号集并列一行），并在 brief 头部加「改动指令源 = plan.md（planRef 值）」行；proposal / story 来源包整节照录不变。
4. `brief-inputs.mjs` self-check 同步改 + 补臂：基 id 去重臂（同基两 AC 引用只贴一次、AC 标签并列）、bug 类降级臂（regression+planRef → 指针形态 + 头部指令源行）、proposal/story 来源照录不变的负向臂、单份头部形态臂；role 相关旧臂（①c 专列 builder 臂等）改写为单份语义等价形态。
目标文件：`.claude/scripts/brief-inputs.mjs`（改）· `.claude/scripts/taskpackage-required-check.mjs`（改）· `.claude/agents/planner.md`（改）· `.claude/agents/acceptance-author.md`（改）· `.claude/agents/builder.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）· `.claude/hooks/supervisor-verdict-gate.mjs`（改——仅注释与 self-check 夹具，判定逻辑零触）· `docs/DEV-GOVERNANCE.md`（改——一句级，OQ-2）

### T5 · enforce-scope worktree 感知：跨树重锚 + 反向洞封堵

**问题**：enforce-scope 钩子以 `CLAUDE_PROJECT_DIR` 为唯一基准算相对路径——兄弟 worktree 的目标路径被算成 `../` 前缀形态，per-agent allow  glob 全部失配（261015 D 事故：子 agent 写兄弟树全被拦，烧 5.6min 探针后退化成 planner 跨树双写互踩 canonical 线）；**反向同洞**：主线程从 A 树往 B 树的 `docs/contracts/**` 写时，universal 红线检查（bashDenyWrite 等）同样因 `../` 前缀失配——契约冻结防线在跨树方向是漏的。
**证据**：`enforce-scope.mjs` :130（`const projectDir = process.env.CLAUDE_PROJECT_DIR || input.cwd || process.cwd();`）、:164-165（`const abs = resolve(input.cwd || projectDir, filePath); const rel = relative(projectDir, abs)...`）；scope-map.json universal.bashDenyWrite 三条（`docs/contracts/**` / `.claude/**` / `delivery/taskpackages/**`，起草期读取实证 2026-10-15）——glob 锚定 rel 开头，`../KidPal-*/docs/contracts/x` 恒不命中。ARQ D 源。该钩子现无 self-check（起草期 grep `selfCheck` 0 命中实证）。
**根因**：rel 锚定的单树假设——钩子在 worktree 池机制（260915 批）之前写成，池化后「本仓路径」不再是单棵树。
**改法**（立项 ack W5 条）：

1. 目标路径解析落在本仓**任一在册 worktree** 时重锚：从目标路径向上找 `.git`（worktree 下它是文件，读 `gitdir:` 行定位 common dir），读 `<commondir>/worktrees/*/gitdir` 纯 fs 映射 + 主 checkout 根（common dir 上级）构造在册树集合——**不 spawn git**（钩子是热路径）；命中在册兄弟树 → rel 重锚到该树、读**该树**的 `.claude/scope-map.json` 判定（Bash 重定向分支与 Edit/Write 分支同走重锚）。
2. 该树 scope-map 缺失 / 不可读 / gitdir 映射歧义 = **fail-closed** 拦截并点名理由（不向宽）。
3. 非在册路径（不属任何在册树，如 c:\tmp）**行为不变**——rel 带 `../` 前缀的现行语义原样保留（universal 不命中放行主线程、per-agent allow 失配拦截子 agent）。
4. self-check 补建（argv 触发型，形态对偶 verify.mjs / firmware-build-check.mjs 既有 self-check；fixture 走 temp dir 假树，对偶 brief-inputs.mjs 自检的 mkdtemp 先例）：三臂——① 在册兄弟树写工作面放行（builder → 该树 firmware/ 下文件）；② 在册兄弟树写红线面拦截（Bash 重定向 → 该树 docs/contracts/ 下文件）；③ 非在册路径行为不变。
目标文件：`.claude/hooks/enforce-scope.mjs`（改）

### T6 · 叫停协议成文：先停会话进程、再删树删分支

**问题**：「叫停」动作只删 worktree 与分支、没停会话进程——261015 E 事故：headless planner 会话在被叫停后裸跑完整条 loop，2h01m 产出全部丢弃，且其「在途存量」被误当新包省下的验收件。全仓无叫停程序成文，启动卡无会话标识可核对「进程确实已死」。自动杀进程脚本（pod-stop.mjs）不存在（起草期 glob 实证 2026-10-15）。
**证据**：ARQ-261015-lite-package-execution-overhead E 源（11:08→13:09 并行线，遥测在仓外 /c/tmp/261015-parallel-incarnation/）；`pod-coordination.md` 全文无叫停条款（通读实证）。
**根因**：Pod 生命周期管理只有 launch 侧与 merge 侧程序，中止侧是空白。
**改法**（立项 ack W6 条，procedural 先行）：

1. `pod-coordination.md` 增叫停协议节——**顺序条款**：叫停一个 Pod = ① 先按启动卡登记的会话标识停会话进程（或确认其已退出），② 再处置树与分支 + board 条目置 abandoned。顺序倒置 = E 事故同型：树没了、会话还在跑、产出无人认领。**树处置两形态**（R1-F2 折进；与 §2c 换包协议对齐，不架空 human 决策权）：(i) **池树复位续用**——在册池树按既有 abandoned 复位路径处理（先 main→base 同步、再 `git reset --hard <同步后的专用 base 分支>`，§2c 第二条原文路径不变），树与常驻分支不换不删；(ii) **删树**——废弃非池化单包树、或 human 明示收缩池容时，才删除 worktree + 分支，**删树决策归 human**（对偶 §2c 第四条「建新树决策归 human」——池的增减都对人称责）。叫停完成 = 进程死 + 树处置落位（复位或删除）+ board abandoned 三件齐，缺一件在 progress.md / pod-events 留痕。
2. launch-prep SKILL.md：步骤 7 启动卡五要素改六要素——增「会话标识」占位行（点火后由 launch 执行者回填：pid / 终端标识 / 启动时刻）；坑节补「叫停顺序」坑条（与 pod-coordination 互指，不双写正文）。
3. `pod-stop.mjs` 自动化（读 board / 启动卡会话标识 + 杀进程 + 删树删分支一条龙）**不本批实现**，列 §4 OQ-1。
目标文件：`.claude/references/pod-coordination.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 references | `.claude/references/change-triage.md` | 改 | T1：§2 Lite 行 + §7 快路径段 |
| G2 references | `.claude/references/task-intake-and-slicing.md` | 改 | T1：Lite 字段级定义节交叉引用一句 |
| G2 skills | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 改 | T2 lite 分支 + 镜像清单 + 坑节；T6 启动卡六要素 |
| G2 commands | `.claude/commands/build-feature.md` | 未触 | OQ-3 裁 (c)：预检五件 ④ 图索引口径对所有包不变，v0.1 的 lite 口径改动撤销 |
| G2 agents | `.claude/agents/planner.md` | 改 | T2 Step 1 rigor 感知；T4 Step 5/7/8/9 单份化 + :47 限定句 + :74 引用单份化（R1-F4） |
| G2 rules | `.claude/rules/repo-structure.md` | 未触 | OQ-3 裁 (c)：图索引段保持 ARQ-260905 原样，本批零修订 |
| G2 agents | `.claude/agents/acceptance-author.md` | 改 | T4：输入面单份引用 + bug 类指针形态半句 |
| G2 agents | `.claude/agents/builder.md` | 改 | T4：同上 |
| G2 references | `.claude/references/dev-loop.md` | 改 | T3 §5 闸表注记；T4 §2 脚本表行 + §3 制品行 |
| G2 references | `.claude/references/dev-loop-operating-system.md` | 改 | T4：over-scan 定义的单份引用（:122） |
| G2 references | `.claude/references/pod-coordination.md` | 改 | T3 §6 冒烟注记一句；T6 叫停协议节 |
| G3 闸配置 | `.claude/verify-gates.json` | 改 | T3：firmware-build defer 字段 + _doc |
| G3 hooks | `.claude/hooks/verify.mjs` | 改 | T3：defer 判定 + env 注入 + self-check 臂 |
| G3 scripts | `.claude/scripts/firmware-build-check.mjs` | 改 | T3：deferred 臂 + 优先级 + self-check 臂 |
| G3 scripts | `.claude/scripts/brief-inputs.mjs` | 改 | T4：去重键 / 单份化 / bug 类降级 / self-check |
| G3 scripts | `.claude/scripts/taskpackage-required-check.mjs` | 改 | T4：finalize 态单份在场判定 + 头注 |
| G3 hooks | `.claude/hooks/supervisor-verdict-gate.mjs` | 改 | T4：仅注释注记 + self-check 夹具，判定逻辑零触 |
| G3 hooks | `.claude/hooks/enforce-scope.mjs` | 改 | T5：跨树重锚 + fail-closed + 新建 self-check |
| G1 指令根 | `docs/DEV-GOVERNANCE.md` | 改 | T4：第六条 ① 一句级（OQ-2，human 确认） |
| 契约面 | `docs/contracts/**` | 未触 | 本批零触（不变式 6） |
| G3 强制层其余 | `.claude/scope-map.json` / `*.settings.json` | 未触 | T5 纯钩子逻辑修复，映射表不动 |
| G2 agents | `.claude/agents/portfolio-planner.md` | 未触 | 其 :63 的 input-brief 提及是泛述（「Step 7 生成 brief 第②路的输入」），单份化后字面仍真 |
| G2 skills | launch-prep SKILL.md :123 的 input-brief 提及 | 未触（同文件他处改） | 该处是「既有指针由 planner/verifier 自身定义承载」泛述，与单双份无关 |
| 债账本 | `docs/debt/gov/_archive/**` | 未触 | 归档历史制品不改 |
| 实现面 | `firmware/**` / `server/**` | 未触 | 本批是治理机制批，不动实现 |

**sweep 透镜取证**（新判定 pattern 预跑）：

- `defer` 是 verify-gates.json 的新增键——起草期 grep 确认现版无此键（2026-10-15，`grep '"defer"' .claude/verify-gates.json` 零命中），零存量消费者，无前驱冲突。
- `rigor=lite` 存量 census（2026-10-15，`grep -rln '"rigor": *"lite"' delivery/taskpackages/*/taskpackage.json`）：20 包命中，其中 19 包已 merged/收尾（不再跑闸），在飞 1 包 = 261015-voice-rightcol-label-overflow（已 supervisor APPROVE 停真机门）。**defer 新语义的实际生效面 = 未来 lite 包**；在飞包若补跑闸走 T4 过渡注记。
- 双份 brief 消费面普查：见 T4 证据段（grep 全枚举 + 逐原行判读）。

## 4. human 取舍点（OQ）

- **OQ-1 · pod-stop.mjs 自动化时机**。(a) procedural 先行、本批不实现（**推荐**——与立项 ack 同向；叫停是低频动作，先把顺序条款与会话标识登记跑起来，出现首例「叫停时找不到/杀不掉会话」的实测摩擦再立项自动化）；(b) 本批实现 pod-stop.mjs（读会话标识 + taskkill + 删树删分支；代价 = 新 G3 脚本及其 self-check 进本批文件面，且杀进程语义未经 procedural 期校准直接固化）。**裁定（human 2026-10-15）：(a)**——pod-stop.mjs 本批不实现、procedural 先行；首例叫停摩擦实测后登记 gov 域新条目（§6 留根）。
- **OQ-2 · DEV-GOVERNANCE.md 的 G1 触面确认**（主线程派发清单外的起草期新增触面）。第六条 ① 现文明写「一次产双份 input-brief（`input-brief-acceptance-author.md` / `input-brief-builder.md`）」（:311）——T4 单份化后不改则 G1 与 G2/G3 双源漂移。(a) 同批改该句为单份口径（**推荐**——一句级改动，跟随已立项的 brief 合并裁定；本批 review profile 相应标 G1+G2+G3）；(b) 不触 G1、只改 G2/G3（留字面漂移，恰是本批反对的形态，不推荐）。**裁定（human 2026-10-15）：(a)**——human 原话「肯定要一致，改呀」；review profile 维持 G1+G2+G3，T4 改法不变。
- **OQ-3 · repo-structure.md 图索引段的 lite 例外**（同派发清单外新增）。该段现文「launch 面必办、不得降级读文件」与 v0.1 拟的「异步建、不阻塞 intake」存在字面张力。(a) 补一句 lite 异步例外；(b) 不改、靠 SKILL 单面承载（留规则文件与 SKILL 字面冲突，reviewer 撞车复核必点名）；(c) **不改 repo-structure.md、不翻 ARQ-260905**——图索引快照导入 + 增量的设计成本为分钟级、阻塞可接受，规则保持「launch 面必办、intake 前在册」原样，v0.1 的异步分支整体撤回。**裁定（human 2026-10-15）：(c)**——连锁修改已落 v0.2：T2 删异步分支、改为「新树建成后立即发起图索引、与分钟级步骤交叠、intake 前在册」执行顺序注记；build-feature.md 预检改动撤销、该文件出 manifest；§0 不动清单第 7 条改写为必办性与阻塞性均不变；§6 增快照流健康度留根。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 时点标注：pre-commit = 落地改动 commit 前在 gov 工作树内可跑；post-commit = 合并后消费期验证。起草期基线（2026-10-15 本分支实测）：brief-inputs `--self-check` 39/39 GREEN；verify.mjs `--self-check` 10/10 GREEN；firmware-build-check `--self-check` 全绿（工具链探测 PATH:idf.py=false / venv+wrapper=true 如实记录）；taskpackage-required-check `--self-check` 18/18 GREEN；supervisor-verdict-gate `--self-check` 73/73 GREEN；enforce-scope.mjs 无 self-check 入口（grep 0 命中）。

- **V1（pre-commit）六脚本自检全绿 + 新臂在场**：
  ```
  node .claude/scripts/brief-inputs.mjs --self-check
  node .claude/hooks/verify.mjs --self-check
  node .claude/scripts/firmware-build-check.mjs --self-check
  node .claude/scripts/taskpackage-required-check.mjs --self-check
  node .claude/hooks/supervisor-verdict-gate.mjs --self-check
  node .claude/hooks/enforce-scope.mjs --self-check
  ```
  预期：全 GREEN（exit 0）；且输出名单逐字含本批新臂——brief-inputs：基 id 去重 / bug 类降级 / proposal 照录不变负向臂 / 单份头部；verify.mjs：defer 判定五臂 + 接缝臂；firmware-build-check：deferred 臂 + defer∧required 优先级臂；enforce-scope：三臂（兄弟树放行 / 红线拦截 / 非在册不变）；supervisor-verdict-gate：新名不触发臂。**真空绿规约**：核对输出名单含新臂名（防新臂未接线空转）；接缝臂重放先断言 fixture 非空。
- **V2（pre-commit）defer 入口 hermetic 冒烟**：
  ```
  VERIFY_GATE_DEFER=1 node .claude/scripts/firmware-build-check.mjs
  ```
  预期：exit 0，stdout 含 `"phase": "executor-skipped"` 且 reason 点名 rigor=lite scoped 后置；不触发工具链探测（短路先于 probeToolchain，无 IDF 环境机器亦可跑——hermetic）。
- **V3（pre-commit）fail-open 回归**：包上下文缺席时行为不变——由 verify.mjs self-check 的「包上下文缺席 → 不 defer」纯判定臂承载（不碰真 `current/` 指针，防簿记污染）；另跑一遍不带 defer 环境的 scoped 闸冒烟（`node .claude/hooks/verify.mjs --phase=scoped` 于无 defer 命中的现状包）断言 firmware-build 仍真跑或按工具链现状诚实降级——**落地时点现状包 = 在飞 lite 包，命中 defer 属预期**，故本条改在自检臂 + 下一非 lite 包首跑验证（记 §6）。
- **V4（pre-commit）authoring 守卫零新增告警**：`node .claude/scripts/lint-descriptions.mjs` 与 `node .claude/scripts/route-regression-check.mjs --self-check`——本批不动任何 frontmatter `description`，预期零新增告警、自检 GREEN。
- **V5（pre-commit，FROZEN 前形态闸）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/261015-gov-review-lite-fast-path/PROPOSAL.md`——形态错零，`anchorUnion` 成员名单逐 token 核对 ≡ landing-manifest。
- **V6（post-commit，条件触发）lite 包真机链冒烟**：落地后首个 `rigor=lite` 包的 scoped 轮实测——firmware-build 结果行 `notExecuted: true` + verifier 报告点名 + final 轮真编译跑通；启动卡真机轮预检行在场。落地时点无合法在飞窗口（261015 包已停真机门不再跑闸）→ 记「留待下一 lite 包首跑验证」并在该包 pod-retro 回填本批凭据。
- **V7（post-commit，条件触发）跨树 enforce-scope 实测**：下一 worktree 包 launch 后实测一次——子 agent 写本包 worktree 工作面放行、主线程跨树写红线面被拦（exit 2 点名）；无窗口则留待，由 V1 的 self-check 三臂先行承载。

## 6. 留根清单

- **gateProfile 字段对选闸零机械作用**（ARQ C 源顺带事实）：本批 defer 走新增的 `defer` 声明字段、不消费 gateProfile；gateProfile 是否升为机械选闸输入另立项评估。
- **riskTier-overrides-rigor 单向守卫的机器牙齿**：change-triage §2 已诚实标注 convention-only 并记 ARQ 后续 G3——本批不动，维持留根。
- **「full 但 host 可闭环」资格字段复议触发器**（立项 ack 第 1 条 YAGNI）：首个真实命中包出现时再评，落点 = 新 ARQ 条目。
- **V3/V6/V7 的条件触发项**：下一非 lite 包首跑（defer 不命中回归）、下一 lite 包首跑（全链冒烟）、下一 worktree 包（跨树实测）——均由各包 pod-retro 回填闭环。
- **三条债条目的结项动作**：ARQ-261015-lite-package-execution-overhead、ARQ-261015-launch-prep-host-only-env-defer、TECH-261014-host-app-sdkconfig-env-drift 的状态翻转与各域 INDEX 重渲染归落地后主线程/渲染器，architect 不翻状态。
- **存量 20 个 lite 包的双份 brief 历史制品**：不追溯清理；`supervisor-verdict-gate.mjs` 夹具保留旧文件名作合成回归锚同理（测机器不测文件存在性）。
- **enforce-scope 热路径性能**：在册树映射 = 每次钩子调用读 `<commondir>/worktrees/*/gitdir` 小文件若干，纯 fs 不 spawn git；若实测可感延迟再评估进程内缓存（YAGNI）。
- **pod-stop.mjs**：OQ-1 已裁 (a)（human 2026-10-15）——本批不实现、procedural 先行；首例「叫停时找不到/杀不掉会话」摩擦实测后登记 gov 域新条目。
- **图索引建库时健康度**（OQ-3 裁 (c) 配套）：任一包实测专属图索引建库超 ~10 分钟 → 属快照流（`.codebase-memory` artifact）健康度问题，登记 tech 域债条目，不开治理例外；ARQ-261015-launch-prep-host-only-env-defer 记的「十几分钟级」按疑似快照流漂移留根观察。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### Round 1 · 2026-10-15 · architect-reviewer（独立实例，非作者）

**亲核取证**：本节全部 file:line / 行为断言由判官在本分支（gov/261015-gov-review-lite-fast-path）实测复核，非转采起草者证据。实测基线：brief-inputs --self-check 39/39 GREEN、verify.mjs --self-check 10/10 GREEN、firmware-build-check --self-check 全绿、taskpackage-required-check --self-check 18/18 GREEN、supervisor-verdict-gate --self-check 73/73 GREEN——与 §5 起草期基线逐一吻合。

#### 查 1 · 意图保真 —— GREEN

- 立项 ack 七条逐条对账：第 1 条 → T1（rigor 复用 + triage 判注两件 + YAGNI 触发器，全数在场）；第 2 条 → T2-1/T2-2（双锚：final 相位机械牙 + 启动卡预检行「约定层」；TECH-261014 镜像义务随同后移，且以「镜像清单是步骤 4 内部清单、后置条款写步骤 4 入口层」消解 ack 明示的「两条款打架」险）；第 3 条 → T3（窄改 + 声明式 defer + notExecuted 上收 + env 透传定稿如实标注）；第 4 条 → T5/T6 独立工作项；第 5 条 → T4-3（sourceType 闭集 + planRef 在场双条件、proposal/story 照录不变）；第 6 条 → T2-1/T2-3（池优先 + 纪律线成文）；第 7 条 → §4 三裁定行与 ack 逐一吻合。
- OQ-3 裁 (c) 的连锁修改四件全部落实：T2 异步分支已撤（改为执行顺序注记、「intake 前在册」原样）、build-feature.md 已出 manifest（§3 未触行）、§0 不变式 7 改写为「必办性与阻塞性均不变」、§6 增快照流健康度留根。
- 两条 ARQ 的 A~E 五源全部有落点；C 源语义红线（ARQ 原文主张 changeManifest 判据）与 ack 裁定（rigor=lite + triage 判注）的偏差在 T3 证据段如实记录、未重开、未静默置换 ✓；A 源「图索引十几分钟级」与 SKILL.md:98「分钟级、近零成本」的矛盾按「疑似快照流漂移」如实呈现 + §6 留根，不夸大不掩盖 ✓。
- 无 ack 之外的新语义夹带；无 ack 项丢失。

#### 查 2 · 纪律合规 —— GREEN（两项 should 见查 3）

- **后置≠跳过守得住**：verify-gates.json firmware-build timing=scoped（:49-54 实锚）、gateActive（verify.mjs:108-113 实锚，全仓唯一选闸咽喉——grep 实证）；defer 语义 = phases 不含 final（配置事实）+ 代码层相位硬判双保险；final 相位十闸全量不变式成立（闸表实数 10 闸）。executor-skipped 上收链复用 liftNotExecuted（verify.mjs:184-194 实锚）+ summarizeNotExecuted 既有链，零新解析分支，与「不假绿」红线一致；「没跑全程机器可见 + verifier 点名义务由 DEV-GOVERNANCE 验收测试纪律 §1（:268 实锚）承载」表述准确。
- **T4 supervisor 判定逻辑零触边界成立**：改法枚举仅 :143/:203/:242 三处注释（实锚均为 input-brief-acceptance-author.md 相关注记）+ :736-746 夹具（实锚：夹具测的是排除闭集机器、路径轴保护活性，保留旧文件名作合成回归锚的论证成立）+ 新增不触发臂；判定函数零触。
- **T5 方向向严**：fail-closed（scope-map 缺失/不可读/映射歧义 = 拦截）+ 非在册路径行为不变（现行 `../` 语义原样保留）——无向宽开口。
- **「Lite 省文牍不省闸」未被违反**：task-intake-and-slicing.md:242 原样（实锚）；T1-③ 明示字段级定义本体一字不动；验收先行/红基线/mutation/final 全量/真机持板五道均在不变式清单逐条在场。
- **门语义零漂移**：merge-driver QUICK_CHECK_IDS 构成不动（merge-driver.mjs:429 实锚 = contract-lint + acceptance 两门）；T3 对 verify-gates.json _doc :32 与 pod-coordination :88 仅做文句同步、不动闸集。

#### 查 3 · 撞车复核 —— 2 项 should、1 项并入 F3

- **F2（should）· T6 叫停协议与换包协议 §2c 的树处置分叉未裁**：T6 顺序条款写「② 再删 worktree + 删分支」为无条件形态，而 pod-coordination §2c 换包协议（:56-58 实锚）第二条钉「一树一常驻分支——包与包之间不换不删」、第四条钉「建新树决策归 human」。叫停对象若为池树，删树既违背池模型、又以既成事实架空 human 的建新树决策权（池树被删 = 下包被迫建新树）；「既有换包协议的 abandoned 复位路径不变」一句只消解 board 状态路径、未消解树处置归属。修法方向：T6 叫停协议节补一句处置分叉——叫停后树处置两形态（池树按换包协议 reset 复位续用 ∨ 删树删分支归 human 决策），与会话标识停进程的顺序条款正交。不改不动摇「先停进程」核心；故 should 非 blocking。
- **F3（should）· T2 漏列 SKILL 产物清单第 4 件的同步限定**：SKILL.md「产物（完成标准）」第 4 件现文「每个包：固件环境等价化（:17 实锚）」——T2 lite 新树后置后，该完成标准行不加速定词则 frozen 文本自带「完成标准说每包已做、lite 分支说后置」的自相矛盾，落地核验按 frozen 文本逐条对照时必撞。修法方向：T2-1 改法枚举补一句「产物清单第 4 件加『lite 新树 = 后置真机轮前，见步骤 4 lite 分支』限定」。
- **F4（should）· T4 残留清扫漏 planner.md :74 与 :47 两处**：T4-2 对 planner.md 的改法按 Step 枚举（Step 5/7/8/9），但 :74〔JOURNAL-APPEND〕内「超读申报段（`input-brief-<role>.md` 第④节表）」（实锚）在单份化后文面失真——它与 dev-loop-operating-system.md:122 是同文句、后者已列入改法而前者漏，同批改一处漏一处即双源漂移；:47（1e 需求权威「与 input-brief 的 story 照录投影同源」）在 bug 类降级（T4-3）后「照录」措辞对该包类部分失真。修法方向：T4-2 planner 枚举补 :74 改单份引用、:47 补「bug 类包为指针形态」半句限定。
- 其余撞车面核查无冲突：change-triage §2 四档表（:23-28）/§7（:100-102）实锚与 T1 改法兼容；dev-loop.md §5 验证阶梯表 firmware-build 行（:227）注记落点成立；launch-prep SKILL 步骤 3/4/5/7 与 T2/T6 改法兼容（步骤 4 现文本就「仅新建树；复用树跳过」，lite 分支是收窄不是翻转）；repo-structure.md 图索引段零修订与 OQ-3 裁 (c) 一致；TECH-261014 条目（实锚）修法方向① + 「排期归启动面批」与本批承接关系自洽。

#### 查 4 · 牵连面完整 —— GREEN（1 项 advisory）

- landing-manifest 17 件 = G2 九件 + G3 七件 + G1 一件，与 §2 各 T 目标文件并集逐件吻合；§3 sweep 25 行 UNCLASSIFIED=0。
- input-brief 全仓普查（判官独立 grep，排除 _archive）：命中 12 个治理面文件 + ARQ 本体，与 T4 证据段枚举完全闭合；file:line 逐点实锚（brief-inputs :417/:430/:454/:100-105/:761、taskpackage-required-check :545-553/:12-15、dev-loop :89/:135、dev-loop-operating-system :122、DEV-GOVERNANCE :311、supervisor-verdict-gate :143/:203/:242/:736-746）。acceptance-author :30 属「输入面引用改单份」文件级改法覆盖、无需单列。
- §3「未触」抽核全过：portfolio-planner.md:63 泛述单份化后字面仍真（实锚）；SKILL.md :123 泛述与单双份无关（实锚）；build-feature.md 预检五件 ④ 图索引口径不变（:16 实锚）。
- git 层证据实测吻合：firmware/sdkconfig tracked；dependencies.lock / main/test/host_app/sdkconfig / managed_components 均 gitignored（check-ignore 逐条实锚）；sdkconfig.defaults tracked 且含 CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK=y（:34）。**F5（advisory）**：该文件实测 `wc -l` = 37 行、PROPOSAL 写 38 行——数字微差，FROZEN 前顺手修正或标注口径，不阻塞。
- enforce-scope 无 self-check（grep 0 命中实锚）与 T5-4「补建」表述一致；enforce-scope.mjs :130/:164-165 双行实锚。
- V3/V6/V7 条件触发留根均有可执行闭环（pod-retro 回填）；V1 真空绿规约 + 接缝臂 fixture 非空断言在场。
- rigor 机器消费面复核：全仓脚本对 rigor 无语义分支（仅 brief-inputs :438 头部打印、:549 自检 fixture），T1 根因「launch/verify 两面无机制读它」成立。

#### 判档与升级判定

- **判档**：manifest 17 件含 G1（docs/DEV-GOVERNANCE.md）+ G2 九件 + G3 七件（hooks/scripts/verify-gates.json）——轻路径判据第 ③ 条命中任一恒 Track B，轻路径不适用；§0 判档依据如实（引用 gov-change.md 轻路径判据 + Track A 判据「只看收敛度」，且两条 ARQ 原文 full-adversarial 自评的三分叉已被 human 立项 ack 闭合）。走 Track B 主路径五段式全件、retro 不豁免——本判维持。
- **Track A 段升级判定**：不升。资格判据 / 后置锚 / 改法宽窄三分叉均已闭合且 PROPOSAL 未重开；本评审四项发现（F2-F4 should、F5 advisory）均为条文完备性级，无方向级缺口、无未裁根本分叉。

#### Round 1 结论

- **F2（should）**：T6 叫停协议补树处置分叉句（池树复位续用 vs 删树归 human），与 §2c 对齐。
- **F3（should）**：T2-1 补 SKILL 产物清单第 4 件限定词。
- **F4（should）**：T4-2 planner 枚举补 :74 单份引用 + :47 bug 类限定半句。
- **F5（advisory）**：sdkconfig.defaults 行数 38 → 37（实测）。
- 无 blocking。**ROUND1 VERDICT: GREEN**（三项 should 修订须在 FROZEN 前随下一版 PROPOSAL 落实，主线程标 FROZEN 时核对；F5 顺手改）。
verdict: GREEN（R1 · 2026-10-15 · 0 blocking · 3 should 已随 v0.3 折进）
