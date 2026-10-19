# pod-retro · 261018-sd-log-usb-read（第一人称热记忆）

> 标签：第一人称热记忆。只记我在场的过程体感与判断，不复述机器数据；token / gates / rounds / verdict / 测试数字以 machine artifacts、pod-events、verifier-evidence 为权威。

## 这个包整体给我的感觉

这是我跑过的「计划先行度」最高的一条包：plan 是 architect 比例化 review 过的、launch-card 把停等点写死、acceptance-author 一轮就把 seam 契约钉到 builder 一次落地。整条 loop 我没有升过一次 human 裁决，真机门前的停等是设计好的动作而非事故——这就是启动卡纪律的价值。

## 卡点与绕轮的主观判断

- **零 fixer 轮**：scoped r1 与 final r2 都一次绿。我把这归功于两件前置：一是 persist retro 教训在 intake 期就消化了（相邻冻结齿字面锚扫查做在派单前，TEST 6 结构锚的加位兼容性提前核掉，builder 没有为 must-stay-green 绕轮）；二是 mutation-check 闸我提前直跑了一次，发现 TEST 2 无专属变异的证据缺口，回传 acceptance-author 补 M9——如果等到 verifier 才发现，就是一次 fixer 往返。**「闸提前预检」连续第二个包证明是省轮次的动作，值得固化为我的固定相位。**
- **final 相位的 scope-file 依赖我低估了**：我原以为 planner 直跑 final verify 就行，实际 final 相位硬性要求 --scope-file（integrator Step 4 产物），verify.mjs fail-closed 举手才让我确认了流序——scoped 绿 → integrator 圈定 → final。persist 的 git log 里 final r2 与「集成收口」同 commit 也印证了这个顺序。这个流序值得记进我个人的跑包清单，下次不再试错。
- **acceptance-author 的两轮取证质量都高**：第一轮冻结件 1591 行、7 TEST + 契约↔断言强度申报表 + GAP 申报；第二轮补 M9 缺口时还自查出「变异跨次累积」缺陷并重跑受影响变异——取证诚实度体现在它主动报告修正过程，而不是只给结论。
- **builder 的「残留验证缺口」申报是加分项**：它明确说 port 不进 host 构建、设备编译归闸，没有把「host 绿」包装成「全绿」。这种诚实边界申报让我的 verifier 判读很轻。

## 别扭的 acceptance / 规则

- change-manifest 闸又走了 pre-cutoff skip（persist retro 已经记过「绿得没有信息量」）：仓根 gitFirstCommitDate 早于 cutoff 导致存量放行。我人工做了 closed-set 对账兜底，但这个闸对老仓的形态性失效依然值得记一笔。
- landsIn 单 id 约束第一次踩：decisionCoverage 的 landsIn 我写了「LUR-A1/LUR-A3」复合形态，required-check 立刻红。判据本身合理（可机判），是我起包时没按单 id 投影。

## 帮倒忙 / 帮上忙的钩子与规则

- 帮上忙：verifier-evidence-check 的 invocationId 精确对账、brief-inputs 的单份投影纪律、taskpackage-required-check 对 landsIn 的机判——都是零噪音放行。
- 略别扭：final 相位缺 scope-file 的报错信息写得清楚（fail-closed 举手 + 指明产物来源），这个体验比静默默认全量好得多。

## 人工纠偏体感

本轮零 human 纠偏。唯一 human 触点是启动卡里已裁好的两件事：OQ-1 合并顺序、真机门停等。headless 启动的 allowlist 没有卡过一步。

## 下次改进

1. 「闸提前预检」固化为 planner 固定动作：派 builder 前直跑 mutation-check / change-manifest / docs-sync 预检（本包省了一轮，persist 包也验证过）。
2. final 相位流序（scoped → integrator 圈定 scope-file → final）记进个人跑包清单，不再靠 verify.mjs 举手教学。
3. decisionCoverage landsIn 起包时就按单 id 写，避免 finalize 期返工。

## escalation themes（post-cutoff，cross-reference pod-events）

本包 pod-events 零 agent-escalation 事件；唯一非常规事件是 intake 期的 launch-precheck 记录（human-decision 形态，承 launch-card 既有裁决，非升级）。后续新增三个事件锚：`df4e0382`（smoke 失败升级）→ `38862edb`（降级 A 闭合）→ `056d1338`（human 批合并）。

## resume 补记（2026-10-19 真机轮 + 降级 A 收口后，第一人称热记忆）

- **降级 A 一发命中**：fixer 的最小补丁（VFS open/read → `usb_serial_jtag_ll_read_rxfifo` 轮询）让 TEST 6 重锚后转绿、真机 smoke 重测通过。plan 风险节预写降级链的价值在真机门兑现了——smoke 失败当晚就有合法路径，没有升级裁决停摆。**Kconfig 帮助文本「secondary 仅输出」的警告是真的**：`/dev/secondary` VFS read ops 注册了但直读未达，HAL 直读是实际可用路径——对后续要碰 USB-Serial/JTAG 的包是高价值事实，review-packet 补记已收。
- **真机九项 8 PASS + 1 NOT-EXECUTABLE**：第 7 项（拔卡）卡物理难拔，human 如实标注、host 验收测试覆盖降级路径——「诚实申报不可执行」比凑一个假 PASS 好得多，这个判定形态值得沿用。
- **跨包共享分支上的收口会话交叠**：persist 收口会话直接在共享的 dev/pod-m2 上落了 15 个提交（含 current/ 指针翻走、fixer fsync 补丁、合并后簿记），我 resume 时 current/ 指针已指 persist——**共享分支多 Pod 交叠时 current/ 指针在每次闸前必须实核**（机械闸按 current/ 解析 TaskPackage，指错包 = 静默错评）。INDEX 行用显式位置参数兜住了。
- **rebase 权限 ask 挡在最后一步**：settings 把 `git rebase` 放 ask 列（防绕 merge token 串行化的设计意图），headless 会话 ask 弹窗未达 human 即自动拒绝。merge-driver 内部 rebase 与 merge 原子绑定、无 rebase-only 入口——「planner 手搓 rebase → 闸 → merge-driver」这条 human 序列在当前权限面下需要 human 批一次 rebase 或亲自跑一条命令。流程与权限面的真实摩擦点，值得 ARQ 记一笔（`git rebase` ask 判据的 headless 形态）。
- **多包共享一个机械闸的状态机摩擦**：verdict write-once + stale 移除 + 「新验收测试轮才重跑」语义，遇到共享分支上另一包的收口会话也在动 diff 时，stale 判定需要人肉对齐——本包两次 stale 移除都是我核对 diff 基准后手动做的。不算坏设计，但共享分支形态下值得在 ARQ 里提「verdict 基准 = main...HEAD 显式化」。

## 最终收口事实（供 resume 链消费，机器数据以 artifacts 为权威）

真机九项 8 PASS + 1 NOT-EXECUTABLE（LUR-X4 成立）；降级 A 补丁 `ef09aa97` 后 verifier r3/r4 十闸全绿；human 批合并凭据 = pod-events `056d1338`；current/ 指针已修回本包（`e6224c20`）；待办 = rebase（待 human 批权）→ verdict 复判 → 升级闸 → merge-driver。
