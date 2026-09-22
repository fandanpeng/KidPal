# 启动卡 · 260921-m3-k1-cloud-schedule-producer

跑 `260921-m3-k1-cloud-schedule-producer`（workstream `m3-schedule`，mergeOrder 33）。

基线锚：`delivery/workstreams/m3-schedule/progress.md` 2026-09-21「launch 面就绪」段带 SHA 行（@ 40aefa4）——server 面 = 47 passed / 0 failed；firmware host 面 = 63 Tests / 0 Failures；消费按差异判停协议：`git diff --quiet 40aefa4 HEAD -- server/`（或 `-- firmware/`）零差异 → 引用不重跑，有差异 → 同命令重跑该面。

上一包留根：`delivery/changes/dev/260920-m2-charwindow-display-fix/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。

固件严格模式默认开：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。

命令行（两形态并列，标注适用条件）：
① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`（human 在独立终端盯进度，权限层在场生效；不加 `-p` / `--dangerously-skip-permissions` / 时延环境变量）
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（自治 loop、无人点权限提示；另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`——print 模式默认只等后台子任务 600s 即 terminate，会掐断长时 Pod）

包事实速览（intake 自探测为准）：K1 = 云端课表数据链路——契约 diff（E1 §1.3② 周网格 + §1.3⑥ home data，HUMAN_APPROVED 裁五问 = 包内 CONTRACT-CHANGE-PACKET.md §5，C2–C4 收口义务随包）+ B1→周网格/upcoming 计算 + 决策 26 三触发 producer + 录入即时通道（取舍点 C 甲案倾向）。subsystem = server；pytest offline 齿；契约面只经契约门通道（签字已在案，diff 落地即按 packet §1 执行）。
