# 启动卡 · 261022-p3-record-front-polish-fix

跑 `261022-p3-record-front-polish-fix`（feature 单包，rigor=lite，riskTier=medium，sourceType=regression；无 workstream/mergeOrder——debt 条目直起）。

- **reviewed plan**：`delivery/taskpackages/261022-p3-record-front-polish-fix/plan.md`（architect `/triage-change` 比例化 review 2026-10-22 + human 触点①确认：②单包 Lite ×2 并联，本包 = 包 B front 记录会话族；并联包 A = `261022-p3-record-chrome-ghost-fix` 固件面，零文件交集零依赖）。
- **上游面**：无——四面探测触面 = 1（纯实现面），不建 spine；逃生门预查干净（缺陷④落点路由注记：引擎侧过滤候选排除——引擎 = 第三方部署树，front 侧自足）。**launch 前置已满足**。
- **基线锚**（带锚引用，producer=consumer 同会话）：server 面 = **238 passed, 4 deselected, 33 subtests passed, 0 failed** @ SHA `64d54eba`（2026-10-22 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`）。server 面零已知红，无红包袱。firmware 面本包不触（并联包 A 的面），无锚可判。本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **上一包留根**：同树前一包 = `261022-p3-record-chrome-ghost-fix`（代码面全链绿、supervisor APPROVE、board=blocked 待真机轮；closeout 制品齐，commit `64d54eba`）——its review-packet 的 pod-retro 教训（docRefs 齿锚、round 序数跨相位单调、变异预测矩阵固化）intake 逐条评估。树池交接卫生已核：tracked 净（HEAD `64d54eba`）、untracked 仅本包目录。
- **固件严格模式**：本包零固件触面（changeManifest 全在 server/**）——firmware-build 闸按 scoped 相位 defer 短路（lite 口径），无固件环境义务。
- **环境事实**：复用池树 C:/AI/KidPal-pod-m2（常驻分支 dev/pod-m2，包 A 之后顺序复用——同树顺序执行消 INDEX.md 并行相交面）；pytest 环境现跑即绿（12.7s 全套）。
- **部署注记**：本包落地后 front 需重部署 ECS（`/opt/kidpal/front/front/front_app.py`）才生效——部署归真机轮前动作（kidpal-ecs-gateway 流程），非本 Pod 自动步。
- **真机门前停等预期**：验收含 human 真机目检项（引导不独播、说「说完了」正常收尾、新篇列表最上且标题可识别）——planner 到真机门前停（status=`awaiting-human:real-device`），与包 A 收尾轮、diag-log-content 收尾轮合并执行省烧板。
- **合并门预期**：riskTier=medium 非 high，supervisor 机械闸正常判；前置升级闸已知 tier3 并行相交面（taskpackages/INDEX.md，与包 A 同源）——合并时点归 human 拍并行取舍。
- **会话标识**：human 在场交互式会话（VSCode 扩展，2026-10-22 点火）。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

本单由 human 在场会话直启（/build-feature 261022-p3-record-front-polish-fix 已在本会话触发，launch 面预检 → 本卡补齐后回 Step 1 intake）。备选形态：

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`

② 无人值守 headless：`CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261022-p3-record-front-polish-fix —— 从 delivery/taskpackages/261022-p3-record-front-polish-fix/plan.md 读 reviewed plan 做 intake；真机目检项到门即停等 human，见本启动卡真机门前停等预期"`

> 实证注记：dev-loop headless 不用 `--dangerously-skip-permissions`（平台分类器硬拦），依赖 `dev-loop.settings.json` allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
