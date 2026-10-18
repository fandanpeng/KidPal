# 启动卡 · 261018-sd-log-persist

跑 `261018-sd-log-persist`（feature 单包，rigor=full，riskTier=high；无 workstream/mergeOrder——非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261018-sd-log-persist/plan.md`（architect `/triage-change` 比例化 review + human 触点①确认 2026-10-18：②单包 Full / riskTier=high）。intake 读它建 `taskpackage.json` 并**首提交**（check-launch 当前唯一 blocking = taskpackage.json 未 commit 到基准 ref，属本流程预期，intake 提交后即消）。
- **上游架构面（已冻结、已 landing）**：`delivery/review/261018-arch-review-sd-log-persist/`（PROPOSAL FROZEN v1.0 + landing-verdict GREEN + retro；landing commit `5a37b001`：决策 24 白名单增列「设备诊断日志」+ 六处镜像 + ADR-012）。**launch 前置已满足**，不再等任何上游面。
- **基线锚**：无 workstream progress.md 锚（非拆包产物）。判停事实：本包预定改动面 = `firmware/main/diagnostics/sd_log.{h,c}` + `sd_log_fs.cc` + `main.cc` + waveshare 板 `.cc` + 两处 `CMakeLists.txt`，**不触 `server/**`**（server 面无锚可判）；host_app 可覆盖纯逻辑齿（环形缓冲/轮转/文件名），但 `esp_log_set_vprintf` 钩子 / FATFS 写 / 板级 attach 是设备专有编译 seam → **快路径不适用，按 full 环境备**；验收 = host 齿 + 设备编译闸 + 真机轮；acceptance-author 新冻结齿恒现跑，无缓存可言。
- **上一包留根**：本树上一批 = 同一 changeId 的架构面 landing（`delivery/review/261018-arch-review-sd-log-persist/retro.md`，非 dev 包 pod-retro）；交接卫生已核——tracked 净（HEAD `27b4ecd5`），untracked 净。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 rigor=full，无 defer 短路）。
- **真机门前停等（本包特有，写进派发即生效）**：真机轮需 human 持板——观察项 = ① 日志文件真实落 `/sdcard/logs/` 且含 STT/LLM 原文片段；② 串口输出不因装钩子中断；③ 循环覆盖（32 × 512KB，目录 ≤16MB）；④ 拔卡/坏卡不崩、30s 重试。planner 跑完 intake → acceptance-author → builder → verifier 代码面齿 → 设备编译闸后，**在真机门前停**：status 写 `awaiting-human:real-device`、退出报告交回，把观察项整理成一份持板操作单；human 完成后 resume 续跑。
- **合并门预期**：riskTier=high，supervisor 合并门会升级 human；这是预期行为。
- **会话标识**：headless 启动任务 ID `bnbnjhpqf`（2026-10-18 22:10 点火；叫停须先停会话进程再处置树/分支——`TaskStop` 杀不死 `claude -p` 时按 CommandLine 定点 taskkill）。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

① 人在场交互式：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless（不阻塞在场；真机门前自动停等）：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261018-sd-log-persist —— 从 delivery/taskpackages/261018-sd-log-persist/plan.md 读 reviewed plan 做 intake（建 taskpackage.json 并首提交，消 check-launch blocking）；changeId=261018-sd-log-persist；上游架构面 FROZEN + landed（commit 5a37b001）；真机轮到门即停等 human（status=awaiting-human:real-device），见本启动卡真机门前停等条款"
```

> 实证注记（260924 批 + 本仓 memory）：dev-loop headless 不用 `--dangerously-skip-permissions`（平台分类器硬拦），依赖 `dev-loop.settings.json` allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
