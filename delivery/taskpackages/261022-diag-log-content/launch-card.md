# 启动卡 · 261022-diag-log-content

跑 `261022-diag-log-content`（feature 单包，rigor=lite，riskTier=medium，sourceType=human-ad-hoc；无 workstream/mergeOrder——非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261022-diag-log-content/plan.md`（architect `/triage-change` 比例化 review 2026-10-22 + human 触点①确认：②单包 Lite / 钩子层每行带墙上时间）。intake 读它建 `taskpackage.json` 并首提交（check-launch 的 taskpackage.json 未 commit blocking 属本流程预期，intake 提交后即消）。
- **上游面**：无——四面探测触面 = 1（纯实现面），不建 spine；契约四分类 NO_CONTRACT_CHANGE（triage architect 自明，依据 architecture.md:270 板端日志行无契约权威）。**launch 前置已满足**，不等任何上游面。
- **基线锚**（带锚引用，差异判停已跑）：firmware host 面 = **222 Tests 3 Failures**（3 条全为 p1_auxbar AB-A1/A2/A4 先存红）@ SHA `5412af06`（261021 合并点），实测落盘锚 = `delivery/taskpackages/261021-p3-record-cancel-lifecycle/review-packet.md:32`；本包 intake 时复判 `git diff --quiet 5412af06 HEAD -- firmware`——2026-10-22 实测零 diff，引用成立不重跑。server 面本包不触，无锚可判（261018 同款）。本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **上一包留根**：`delivery/taskpackages/261021-p3-record-cancel-lifecycle/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 lite 判注「设备编译面深、快路径不适用、按 full 环境备」→ 无 defer 短路，firmware-build 相位照跑）。
- **环境事实**：复用池树 C:/AI/KidPal-pod-m2（常驻分支 dev/pod-m2），固件环境已就位（跳过等价化）；专属图索引 `C-AI-KidPal-pod-m2` 在册随树刷新。交接卫生已核：tracked 净（HEAD `fe5d1e09`），untracked 仅本包目录。
- **真机门前停等（本包特有，写进派发即生效）**：真机轮观测项 = ① 导卡逐行核五类记录面（每行墙上时间前缀〔含未同步 BOOT+ms 降级〕/ PAGE_SWITCH from→to+trigger / 状态机跳转含被拒转移 / 按键语义名 / SystemInfo 降频 60s）；② 串口 COM5 输出不中断；③ 无钩子递归迹象（写盘任务内零 ESP_LOG）；④ 轮转与 dropped 计数正常（内容增强后日志量上升，128KB 环形 + 16MB 封顶下观察）。planner 跑完代码面齿 + 设备编译闸后**在真机门前停**：status 写 `awaiting-human:real-device`、退出报告交回，整理持板操作单；human 完成后 resume 续跑。
- **合并门预期**：riskTier=medium 非 high，supervisor 机械闸正常判；lite 无特殊性豁免。
- **会话标识**：human 在场交互式会话（VSCode 扩展，2026-10-22 点火）。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

本单由 human 在场会话直启（/build-feature 261022-diag-log-content 已在本会话触发，launch 面预检 → 本卡补齐后回 Step 1 intake），无需另起 headless。备选形态：

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`

② 无人值守 headless：`CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261022-diag-log-content —— 从 delivery/taskpackages/261022-diag-log-content/plan.md 读 reviewed plan 做 intake；真机轮到门即停等 human，见本启动卡真机门前停等条款"`

> 实证注记：dev-loop headless 不用 `--dangerously-skip-permissions`（平台分类器硬拦），依赖 `dev-loop.settings.json` allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
