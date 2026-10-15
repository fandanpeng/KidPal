# pod-retro · 260915-m1-k2-keybar-three-slot（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/verify-runs.jsonl、pod-events、taskpackage）为权威。接棒前序包（K1a → N1b）教训吸收情况附末节。

## 卡点与体感

1. **这包最大的坑是我自己挖的：烧录偏移按惯例猜，不读分区表**。0x10000（ESP32 常见 factory 偏移）写到本板 = ota_0@0x20000 错位 → bootloader 校验失败 → 7 秒重启循环、黑屏、USB 叮咚连响。最贵的是它伪装成「新固件崩溃」——host 18/0 全绿 + 设备必挂，指向性极差，fixer 烧了三轮 bisect 才用「同代码双偏移 A/B」把代码摘出去。**烧录偏移永远读分区表（16m.csv），这条已经沉淀成 skill kidpal-flash-device，下次 launch 卡直接引用。**

2. **「逻辑层全通、像素层无字」这种断裂，只有仪器化能撕开**。左右槽无字卡了两轮：第一轮修 sync 触发面（合理假设，没生效），第二轮 K2DIAG 仪器化直接把链路照透——四个入口全触发、slots rc=0、文本值全对、指针全非空，断点瞬间收窄到「set_text 之后、像素之前」。字体缺字形（板级 basic30 子集 787 字形缺键帽用字）不是靠猜的，是靠「页点可见 + 文字不可见」这个观察和逐字缺字实测钉死的。**教训：设备侧视觉 bug，与其盲修两轮，不如第一轮就上临时日志 + 活体操作捕获——一次烧录换全链路可见性，性价比碾压。**

3. **活体协调的捕获窗浪费了两次**。前两轮被动捕获窗开着我这边等、human 那边没动板，整窗只有心跳行。第三次学乖了：明确「捕获已开、现在操作、做完回话」三段口令，一轮拿到 96 条 K2DIAG。真机轮协调协议（n1b 教训④）要加一条：**捕获窗与操作必须实时握手，不能异步各干各的。**

4. **host 齿的结构断言盖不到运行时链路，这是本轮最深的结构性体感**。5 颗新齿全绿、mutation 6/6 敏感——而真机上左右槽压根没字。齿断言的是「代码里有 needle + 逻辑决策表对」，设备侧的字体管线、SetupUI 实际执行路径、渲染触发时序都不在覆盖面。KB-A5 真机轮是唯一能兜住这层的闸，它 FAIL 得其所。**视觉类包的验收权重应该更早压到真机轮，host 齿只是必要条件。**

5. **挂账三件都带 owner 出手，不越权**：BOOT 翻页序 P1→P4→P2→P3（k1a 枚举序，KB-E3 边界内只判不修）、键帽 14px 字号 tradeoff（30px 全量超分区，专属子集字库归后续批）、unit 闸归档收集口径缺口（治理 ARQ 建议件）。sweep 闭账时「archive/ 天然不入收集」的口径只对治理闸 glob 成立、对 pytest 递归不成立——launch 面预警对了，但 final 闸还是踩了，这个口径缺口值得独立治理批收掉。

6. **fixer 的高光轮**：三轮 bisect（中和 keybar 块 A/B 偏移）把「必然复现 = 偏移伪影」钉成铁证，避免了往代码里塞防御补丁的错误方向；诊断残留清零纪律（DIAG-R1/K2DIAG grep 零命中）执行到位。烧板诊断的观测判据（0 RE-ENUM + RX 字节 + uptime 推进）是可复用资产。

## verifier↔fixer 绕轮原因的主观判断

- scoped 第 1 轮：firmware-build 红（keybar.c 未接设备 SOURCES）——builder 只接了 host 面，一轮 fixer 消账。
- final 恒红：unit 闸归档收集 artifact——非本包红，走 existing-red-inventory 豁免件，不进 fix 循环。
- 真机轮 FAIL 不算 verifier 绕轮，是 KB-A5 structural 闸的本职工作：偏移伪影 1 轮 + 字体 1 轮 + 收尾 1 轮，fixBudget 3 恰好用满——**预算设计上该给视觉类包留真机轮专项余量，或把「烧录程序面失败」与「代码缺陷修复」分账**（本轮第 1 轮根本不是代码问题）。

## escalation themes（cross-ref pod-events）

- 无 agent-escalation 事件。human 决策点：launch 前 deviation 拍板（launch 面 AskUserQuestion）+ KB-A5 四项裁定（复轮 + 终裁两轮）。

## 别扭的 acceptance / 帮倒忙的钩子

- enforce-scope 拦 acceptance-author 写 changes/dev/ 是对的（红基线冻结记录回传我转写，n1b 教训②的前置版本照常生效）。
- verify.mjs final 对 server/acceptance/archive 的递归收集——见挂账 d，闸本身需要修口径。

## 下次改进

- **烧录动作统一走 skill kidpal-flash-device**：偏移读分区表、烧前杀捕获进程、烧后 30s 追端口判启动——三步都机械执行，不凭记忆。
- 视觉类包 intake 时预答两问：「设备活动字体的字形覆盖是否含全部新增文案值」「渲染触发点与页循环/覆盖层进出的同源性」——两问都是本轮真机轮炸出来的。
- 真机轮口令协议固化为三段式：开窗确认 → 操作 → 回话读数。
- 捕获脚本 serial_catch_tight.py 的锁死 COM5 坑已写进 skill；用完必杀进程。

## 前序教训吸收核销

- K1a⑥ / N1b⑥ mutation 纪律：变异命名零原字面子串，6 条全过 ✓。
- N1b② taskpackage 形态预检（structural 条目对象化 + acceptanceId 字节一致）：KB-A5 structural + Finalize 字节校验 True ✓。
- N1b③ 枚举外环境事故不裁而报：本轮无枚举外事故，未触发。
- N1b⑤ 合并闸双凭据：本轮待合并门按此执行（--human-approved-exemption + --human-approval-ref 一次讲明）。
