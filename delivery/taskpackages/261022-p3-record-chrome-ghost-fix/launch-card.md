# 启动卡 · 261022-p3-record-chrome-ghost-fix

跑 `261022-p3-record-chrome-ghost-fix`（feature 单包，rigor=lite，riskTier=medium，sourceType=regression；无 workstream/mergeOrder——非拆包产物，debt 条目直起）。

- **reviewed plan**：`delivery/taskpackages/261022-p3-record-chrome-ghost-fix/plan.md`（architect `/triage-change` 比例化 review 2026-10-22 + human 触点①确认：②单包 Lite ×2 并联，本包 = 包 A 固件 chrome 族；并联包 B = `261022-p3-record-front-polish-fix`，front 面，零文件交集零依赖）。
- **上游面**：无——四面探测触面 = 1（纯实现面），不建 spine；逃生门预查干净（五修法零契约/spec/agent/gate 触点；缺陷②帧类型负锚 = 不得新发明下行帧类型）。**launch 前置已满足**。
- **基线锚**（带锚引用，producer=consumer 同会话）：firmware host 面 = **227 Tests 3 Failures**（3 条全为 p1_auxbar AB-A1/A2/A4 先存红，222→227 = diag-log 五齿新增计入）@ SHA `0df2c0c3`（2026-10-22 实测；命令 = WSL `espidf_wsl.sh build`（host_app sdkconfig 已删、从 tracked defaults 重生）+ `host_infra_test.elf`）。相对旧锚 5412af06 firmware 面有 diff（diag-log 两笔固件提交）故重跑，本锚即新地面真值。本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **上一包留根**：`delivery/taskpackages/261022-diag-log-content/`（closeout 残留已 commit `0df2c0c3`：supervisor APPROVE + CDQ-261022-diag-log-docs-sync-write-scope 核销）；pod-retro 未见独立成件（ Lite 包收尾制品在 review-packet），intake 如需逐条评估以 review-packet 为准。树池交接卫生已核：tracked 净（HEAD `0df2c0c3`）、untracked 仅本包目录、host_app sdkconfig 已删强制重生。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 lite 判注「host 验收断言可闭环、机制面断言 host 闭环、观感终判归 human 真机目检」——无 defer 短路声明，firmware-build 相位照跑）。
- **环境事实**：复用池树 C:/AI/KidPal-pod-m2（常驻分支 dev/pod-m2），固件环境已就位（diag-log 包同树刚跑过全链，build/ 热缓存含 diag-log 五缝实现）；专属图索引 `C-AI-KidPal-pod-m2` 在册随树刷新（list_projects 实测在册）。
- **真机门前停等预期**：本包验收含 human 真机目检项（翻阅读卡与记录会话全程键帽无鬼影双闪、归档后首按 BOOT 即生效）——planner 跑完代码面齿 + 设备编译闸后在真机门前停（status=`awaiting-human:real-device`），真机轮建议与 diag-log-content 收尾轮合并省烧板。
- **合并门预期**：riskTier=medium 非 high，supervisor 机械闸正常判；lite 无特殊性豁免。
- **会话标识**：human 在场交互式会话（VSCode 扩展，2026-10-22 点火）。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

本单由 human 在场会话直启（/build-feature 261022-p3-record-chrome-ghost-fix 已在本会话触发，launch 面预检 → 本卡补齐后回 Step 1 intake），无需另起 headless。备选形态：

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`

② 无人值守 headless：`CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261022-p3-record-chrome-ghost-fix —— 从 delivery/taskpackages/261022-p3-record-chrome-ghost-fix/plan.md 读 reviewed plan 做 intake；真机目检项到门即停等 human，见本启动卡真机门前停等预期"`

> 实证注记：dev-loop headless 不用 `--dangerously-skip-permissions`（平台分类器硬拦），依赖 `dev-loop.settings.json` allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
