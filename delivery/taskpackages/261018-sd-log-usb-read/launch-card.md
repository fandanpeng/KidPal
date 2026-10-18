# 启动卡 · 261018-sd-log-usb-read

跑 `261018-sd-log-usb-read`（human-ad-hoc 单包，rigor=lite，riskTier=medium；无 workstream/mergeOrder——非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261018-sd-log-usb-read/plan.md`（architect `/triage-change` 比例化 review + human 触点①确认 2026-10-18：②单包 Lite；lite 快路径不适用，按 full 环境备）。intake 读它建 `taskpackage.json` 并首提交（check-launch 当前唯一 blocking = taskpackage.json 未建，属预期）。
- **上游依赖（未合并）**：`261018-sd-log-persist` 的代码已提交在**本分支** `dev/pod-m2`（HEAD 含 `0dc0447e` 集成收口 + `49e1073b`/`99dda34a` 合并门 ESCALATE 停 human）；本包 base ref = `dev/pod-m2`，不得假设它已在 main。
- **合并顺序（主线程裁，OQ-1）**：两笔干净合并——先用本包的取回命令验完 persist 的真机四项 → persist 合入 main → 本包 rebase 到新 main 后立即合。**persist 未验完前，两个包都不合**（human 2026-10-18「等取回做好一起合」的意图）。
- **基线锚**：无 workstream progress.md 锚（非拆包产物）。判停事实：改动面 = `firmware/main/diagnostics/log_cmd.*` + `sd_log.h`/`sd_log_fs.cc` 只读访问器 + `main.cc` + 两处 CMakeLists + `tools/diag/kidpal_logs.py`，**不触 `server/**`**；host_app 可盖解析/行装配/帧格式化纯逻辑齿，但 `/dev/secondary` VFS 读、FATFS、板级任务是设备 seam → **快路径不适用，按 full 环境备**；验收 = host 齿 + 设备编译闸 + 真机轮；acceptance-author 新冻结齿恒现跑。
- **上一包留根**：`delivery/taskpackages/261018-sd-log-persist/pod-retro.md`（intake 逐条评估；本包对 persist 的 `sd_log.h`/`sd_log_fs.cc` 只加只读访问器，不改其行为与冻结验收）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 lite 但设备 seam 深，按 full 环境备）。
- **真机门前停等（本包特有）**：真机轮需 human 持板 + Claude Code 调 `tools/diag/kidpal_logs.py`。**第一步先做单命令 smoke** 验证 `/dev/secondary` 可读（`kplog list`）；失败按 plan 的降级 A（HAL 直读）→ 降级 B（console primary，须回 triage 升档）。planner 跑完 intake → acceptance-author → builder → verifier 代码面齿 → 设备编译闸后，**在真机门前停**：status 写 `awaiting-human:real-device`、交回操作单；完成后续跑。
- **合并门预期**：riskTier=medium，机械门可能直接放行或升级 human；按实际结果处理。
- **会话标识**：headless 启动任务 ID `（点火后回填）`。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

① 人在场交互式：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless（不阻塞在场；真机门前自动停等）：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261018-sd-log-usb-read —— 从 delivery/taskpackages/261018-sd-log-usb-read/plan.md 读 reviewed plan 做 intake（建 taskpackage.json 并首提交，消 check-launch blocking）；changeId=261018-sd-log-usb-read；上游依赖 261018-sd-log-persist 的代码已在本分支未合并；真机轮到门即停等 human（status=awaiting-human:real-device），见本启动卡真机门前停等条款"
```

> 实证注记：dev-loop headless 不用 `--dangerously-skip-permissions`（平台分类器硬拦），依赖 `dev-loop.settings.json` allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
