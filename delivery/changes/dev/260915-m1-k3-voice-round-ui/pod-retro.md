# pod-retro · 260915-m1-k3-voice-round-ui（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/rounds、pod-events、taskpackage）为权威。接棒前序包（K1a → N1b → K2）教训吸收情况附末节。escalation themes cross-ref pod-events。

## 卡点与体感

1. **这包我烧掉两轮 verifier 的都是「落账/环境形态」而非代码**。r1 是 firmware/build 里残留的 Visual Studio 生成器 CMakeCache（上一位住客留下的），r2 清了缓存后 cmake 走到组件解析、真错（voice_round.c 没接设备 SOURCES）才浮出来——第一次只看 logTail 差点把环境残留当代码病。教训：**fingerprint 相同不等于根因相同，logTail 截断处后面的世界才是现场**；环境残留（build 缓存）归编排者直接消解，不烧 fixer 预算（k2 pod-retro「烧录面与代码面分账」的镜像）。

2. **第三轮红是我自己的落账形态错**：mutationEvidence 回填用了短 id（VQ-A1），而 acceptance[] 是整串文本、匹配器取 raw 全 id——k2 期已经踩过「acceptanceId 字节一致」这颗雷，我回填时没对齐先例形态。落账字段不是自由文本区，**每个结构字段都有先例形态可抄，回填前先 cat 上一包同字段**。

3. **fixer 那轮是教科书式最小补丁**：ErrorEnvelope 把 undefined reference 的三处调用点、根因假设、k2 同病史都钉进 prompt，fixer 单文件 4 行收工。前面 diagnosis 花力气是值得的——「logTail 不够就翻 idf_py_stdout_output 原件」这一步把 fixer 的搜索空间压到零。

4. **VQ-A7 真机轮 deferred 出 Pod 的口径交接顺畅**：human 追加指令一到，我落了 human-decision 事件，integrator 在 review-packet 的 demo 对账行里如实标「host 面一致，真机面待 human 持板确认」没有半句虚构。这种「structural 条目 + 事件留痕 + 制品如实 pending」的三件套是干净的延期姿势，下次长依赖跨包照抄。

5. **uiux-author 编制缺位是本轮唯一的流程空转**：判定 L2 后发现 .claude/agents/ 根本没有 uiux-author.md（verifier 退役的同构问题，planner.md 工具列表与实际编制不同步）。直翻降级 + friction 留痕走通了，但这个编制缺口值得 gov 批收一下——要么补 .md，要么从 planner.md 的可派列表里摘掉。

## verifier↔fixer 绕轮原因的主观判断

- r1/r2：firmware-build 红两连——r1 环境残留（我消解）、r2 真缺源（fixer 一轮）。若 intake 时顺手验一次设备 build 缓存的生成器一致性，r1 可以免掉。
- r3：mutation-check 红 = 我落账形态错（我修正，非 fixer 轮）。
- r4 绿、final r5 一次过。fixBudget 3 只用了 1，预算余量健康。

## escalation themes（cross-ref pod-events）

- 无 agent-escalation 事件。human 决策点 1 条：`1dc757f9` device-round-deferred-out-of-pod（VQ-A7 真机轮 deferred 出 Pod，主线程烧板、human 持板）。friction 5 条（degraded×1 uiux 编制缺位、over-scan×2、scan-miss×1 红基线判读缺口、加 brief-inputs 无关项不计）。

## 别扭的 acceptance / 帮倒忙的钩子

- enforce-scope 拦 acceptance-author 写 changes/dev/（红基线记录回传我转写）——与 k2 期同结论：拦截是对的，转写惯例已顺。
- mutation-check 的整串文本匹配形态对短 id 落账不友好（报错把整串 acceptance 甩脸上才看得出形态差），但这不是 bug，是形态纪律的代价；先例对照即解。
- verify.mjs 的 logTail 截断曾误导一轮判读——logFile 原件比 logTail 可信。

## 下次改进

- **复用树 intake 加一步：设备 build 缓存健康检查**（CMakeCache 生成器是否 Ninja 一致），避免 r1 式环境残留首红。
- 回填 taskpackage 结构字段（mutationEvidence 等）前先读上一包同字段形态，不凭记忆造 schema。
- 真机/长依赖跨包条目的延期三件套：structural 对象化 + human-decision 事件 + review-packet 如实 pending——已成惯例，继续用。

## 前序教训吸收核销

- K2① 烧录偏移读分区表（skill kidpal-flash-device）：本 Pod 不烧板（VQ-A7 deferred 主线程烧），未触发 ✓。
- K2② 设备侧视觉 bug 首轮仪器化：本包真机轮 deferred，host 齿 + 结构断言先行，无设备调试轮 ✓（VQ-A7 持板时若翻车，此教训由主线程承接）。
- K2④ host 齿盖不到运行时链路、视觉类包验收权重早压真机轮：本包 demo 对账行已把「真机才能确认的观感面」显式划出 ✓。
- K2⑤ / N1b⑥ mutation 命名零原字面子串：8 条变异命名全过 mutation-check ✓（r3 红是形态错不是命名错）。
- K2⑥ unit 闸归档口径缺口：已在本分支 ARQ-260916 闭账（4fc0969/0aed071），final r5 unit 闸绿未再踩 ✓。

## 修复轮追加(device-round VQ-A7 r1 后,2026-09-17)

- **k2 教训④在本包原样重演**:host 齿 23/0 全绿 + mutation 全过,真机三项不过——字体缺形、触发时机、父级挂载全在齿断言面之外。本轮对策 = 真机取证四条直接转译成 TEST 6 五颗结构齿(增补齿设计态红 → fixer 转绿),把「真机才能发现的东西」尽机器化;观感像素仍归真机 oracle,齿只锁结构反形态。
- **取证先行又赢了一轮**:主线程串口日志 + 代码复读把三根因钉到行号,fixer 一轮全清——对比 k2 期「盲修两轮才上仪器化」,这次是反过来先仪器后动手,fixBudget 只烧 1。
- **feed 锚假红坑**:fixer 修复注释里逐字写了禁针 token,结构齿扫注释命中假红——「树扫含注释」这个齿特性值得写进所有后续修复轮派单(已落 friction c721520b)。
- **write-once 闸与修复轮的兼容性待观察**:supervisor-verdict.json 首轮已 write-once,修复轮重跑闸的落账行为见本轮记录。
