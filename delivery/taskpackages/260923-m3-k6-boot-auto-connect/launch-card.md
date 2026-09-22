# Launch Card · 260923-m3-k6-boot-auto-connect

跑 `260923-m3-k6-boot-auto-connect`（m3-schedule workstream attach-existing，常驻树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch` 第 10 棒，常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`）。

**基线锚（差异判停 2026-09-23）**：

- server 面：`git diff --quiet dc6094a HEAD -- server` 零差异 → **引用** K4 launch 段锚「server 95 @ dc6094a」（量测命令 `python -m pytest -q --import-mode=importlib -m "not live" <repo>/server/acceptance`，面域 = server）——intake 判停在引用，不重跑。
- firmware 面：**本段现跑**（K5 批 21 文件致 9cae452 锚失效）——host runner **92 Tests / 0 Failures** @ `b869658`（量测命令 WSL host runner `firmware/main/test/host_app espidf_wsl.sh build` + `host_infra_test.elf` run，2026-09-23，面域 = firmware）。语义提醒：本包验收红基线（acceptance-author 新冻结齿）恒现跑，无缓存可言。

**上一包留根**：`delivery/changes/dev/260922-m3-k5-sntp-time-sync/pod-retro.md`——intake 逐条评估（构成 blockingDecision → fail-closed 上报）。重点：① mutation 执行面粒度差跨三棒（R3/R4 均以设计态红≙变异态在案，实跑变异未落仓内 evidence——**本棒必须处理**）；② 睡眠/延时类板级行为 host 齿只钉形态不钉时序的常识（本包重试退避时序同理适用）。

**固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。

**本包特记**：

- human 离席夜间自主跑（2026-09-23 预授权：「技术问题 call architect 分析判断」）——方案形状 = architect Shape A（标志互斥 + 状态恒 Idle，弃 Connecting 互斥），详见 plan.md「关键接缝」；human 回席后补立项 ack 留痕。
- 真机锚（K6-A7 四子锚）挂设备轮窗口，human 在场收口——**Pod 不等真机，host 半收口即走 supervisor 合并门**。
- input-brief 双件由 intake 相位补齐（K5 先例）。

**命令行（两形态并列）**：

1. 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
2. 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
