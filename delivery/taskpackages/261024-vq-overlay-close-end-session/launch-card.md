# 启动卡 · 261024-vq-overlay-close-end-session

跑 `261024-vq-overlay-close-end-session`（feature 单包，rigor=full，riskTier=high，sourceType=regression；无 workstream / mergeOrder——非拆包产物，debt 条目直起）。

- **reviewed plan**：`delivery/taskpackages/261024-vq-overlay-close-end-session/plan.md`（architect `/triage-change` 比例化 review 2026-10-24 判 ②单包 Full / riskTier high + human 触点①确认；nano 被三条独立判据否掉：跨 3 个实现面文件、命中排除清单「触状态机」、两处既有断言必红须重钉）。
- **需求锚**：`docs/requirements.md:147` §2.3「问答 · 播报中」行——KEY 长按 / BOOT 短按 = 结束会话回原页。本包是实施偏离修正，需求 / 架构 / 契约 / 治理四面零变更，不建 spine；逃生门预查 = NO_CONTRACT_CHANGE。
- **基线锚**（带锚引用，producer=consumer 同树）：firmware host 面 = **236 Tests 3 Failures**（3 条全为 p1_auxbar AB-A1/A2/A4 先存红，与 261022 锚 `0df2c0c3` 同一族）@ SHA `6e8425ff`（2026-10-24 本 worktree 实测；命令 = WSL `bash firmware/main/test/host_app/espidf_wsl.sh build` + `./firmware/main/test/host_app/build/host_infra_test.elf`；host_app sdkconfig 已删后从 tracked defaults 重生成）。server 面零 diff，不跑不引用。本包验收红基线（acceptance-author 新冻结断言）恒现跑。
- **上一包留根**：`delivery/taskpackages/261016-sd-card-mount/pod-retro.md`（本树上一包，已 merge）。intake 逐条评估，构成 blockingDecision 即 fail-closed 上报。其中两条与本包相关：① 起包期按 agent 粒度核 doc-sync 落点写权——本包 `docsSyncRequired=false`，无 docs 写面；② board 状态闭集缺 `awaiting-human:real-device` 的老 friction 仍在，真机门按 `blocked` + 事件承载。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 full，无 defer 短路，firmware-build 相位照跑）。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`（常驻分支 `dev/261016-sd-card-mount`，已 reset 到 main `6e8425ff`），树内 tracked 净、host 与 firmware `build/` 热缓存在场、managed_components 在场；专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 已按 main 刷新（nodes 20629 / edges 59118，等于 expected）。树池交接卫生已核：host_app sdkconfig 已删并从 defaults 重生、无其它 untracked 残留。
- **真机门前停等预期**：本包行为半（真断连 / 真静音 / 首页不再收音）host 不可闭环，验收含 human 真机目检六项（plan ⑧-5：播报中 BOOT 短按 / KEY 长按收尾；播报中 KEY 短按仍打断；聆听态零回归；Connecting 可回 Idle；T 秒常驻重建）。planner 跑完代码面断言 + 设备编译闸后在真机门前停（status=`awaiting-human:real-device`，按现有闭集可退 `blocked` + 事件），不自行烧录。
- **合并门预期**：riskTier=high 触发升档判据与 supervisor 重点核；契约面零触碰，但 `pre-merge-escalation-gate.mjs` 照跑。
- **会话标识**：主线程 headless 拉起（2026-10-24 19:18 点火；npm wrapper PID `24324` / node PID `8644`）。叫停时先核这两个进程确实已死，再处置树与分支。

## 命令行（worktree = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch，分支 dev/261016-sd-card-mount）

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`

② 无人值守 headless：
`CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261024-vq-overlay-close-end-session —— 从 delivery/taskpackages/261024-vq-overlay-close-end-session/plan.md 读 reviewed plan 做 intake；真机目检项到门即停等 human，见本启动卡真机门前停等预期"`

> 注记：dev-loop headless 不用 `--dangerously-skip-permissions`（平台分类器硬拦），依赖 `dev-loop.settings.json` allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑（261022 启动卡实证同款）。
