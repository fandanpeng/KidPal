# 启动卡 · 261016-sd-card-mount

跑 261016-sd-card-mount（human-ad-hoc 单包，rigor=full，riskTier=medium；无 workstream/mergeOrder——非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261016-sd-card-mount/plan.md`（architect triage 审过 + human 触点①三项确认 2026-10-16）。intake 读它建 taskpackage.json 并**首提交**（check-launch 唯一 blocking = taskpackage.json 未 commit 到基准 ref，属本流程预期，intake 提交后即消）。
- **基线锚**：无 workstream progress.md 锚（非拆包产物）。判停事实：本包预定改动面 = 板级 config.h/.cc + 两处文档行，**不触 `server/**`**（server 面无锚可判）；host_app 盖不住 SDMMC 设备专有编译单元（triage 实核，快路径不适用）→ 本包无 host 化齿，验收 = 设备编译闸 + 真机轮；acceptance-author 新冻结齿恒现跑，无缓存可言。
- **上一包留根**：无——本树分支零自有提交（bd4beac = main），交接卫生已核（tracked 净，untracked 仅本包 delivery 制品）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 rigor=full，无 defer 短路）。
- **真机门前停等（本包特有，写进派发即生效）**：A1（插卡 roundtrip）/ A2（无卡降级 + KEY 冒烟）需 human 持板。planner 依次跑完 intake → acceptance-author → builder → verifier 代码面齿 → A3 设备编译闸后，**在真机门前停**：status 写 `awaiting-human:real-device`、退出报告交回，把「插卡/拔卡/观察项」整理成一份持板操作单（含原理图三脚目视核对销 GPIO21 疑问）；human 完成操作后 resume 续跑。
- **会话标识**：＿（点火后由 launch 执行者回填 pid / 启动时刻，叫停核「进程确实已死」用）。

## 命令行（worktree = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch，分支 dev/261016-sd-card-mount）

① 人在场交互式：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless（推荐，真机门前自动停等）：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261016-sd-card-mount —— 从 delivery/taskpackages/261016-sd-card-mount/plan.md 读 reviewed plan 做 intake（建 taskpackage.json 并首提交，消 check-launch blocking）；changeId=261016-sd-card-mount；真机轮到门即停等 human，见启动卡真机门前停等条款"
```

> 实证注记（260924 批 + 本仓 memory）：`--dangerously-skip-permissions` 形态被平台分类器硬拦——headless 用上方无旗标形态，依赖 dev-loop.settings.json allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑（429 掐断同理：带续跑说明重拉，不重建包）。
