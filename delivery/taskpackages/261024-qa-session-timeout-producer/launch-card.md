# 启动卡 · 261024-qa-session-timeout-producer

跑 `261024-qa-session-timeout-producer`（feature 单包，rigor=full，riskTier=high，sourceType=regression；无 workstream / mergeOrder——debt 条目直起）。

- **reviewed plan**：`delivery/taskpackages/261024-qa-session-timeout-producer/plan.md`（architect `/triage-change` 比例化 review 2026-10-24 判 ②单包 Full / riskTier high + human 触点①确认；nano 不成立：命中排除清单「状态机」+「验收语义净增且既有语义必改」+ 实现面 5 文件超 ≤2 参照）。
- **需求锚**：`docs/requirements.md:42`（结束通道① 60 秒无语音）、`:47`（⚠️ 对账项）、`:57`（记录会话不适用 60 秒）。方向②结束意图保持 deferred，`TECH-261014-session-end-channels-unwired` 不关账。逃生门预查 = NO_CONTRACT_CHANGE。
- **基线锚**（带锚引用，producer=consumer 同树）：firmware host 面 = **237 Tests 3 Failures**（3 条全为 p1_auxbar AB-A1/A2/A4 先存红）@ SHA `aecded8b`（2026-10-24 本 worktree 实测；命令 = WSL `bash firmware/main/test/host_app/espidf_wsl.sh build` + `./firmware/main/test/host_app/build/host_infra_test.elf`；host_app sdkconfig 已删后从 tracked defaults 重生成）。server 面零 diff，不跑不引用。本包验收红基线（acceptance-author 新冻结断言）恒现跑。
- **上一包留根**：`delivery/taskpackages/261024-vq-overlay-close-end-session/pod-retro.md`（同树上一包，已 merge `d5515ce0`）。intake 逐条评估，构成 blockingDecision 即 fail-closed 上报。与本包直接相关的三条：① finalize TaskPackage 时把 `mutationEvidence.perAcceptance` 的计划形态写齐，不留空骨架进验证期（上包 r2 因此被打回）；② 派 integrator 时给 scope-file 机器消费格式样板，要求 narrative md + `scope-list.txt` 纯路径清单双产出（上包 final r4 因此 fail-closed）；③ regression 包红轮先问「红因落在谁的写权面」，acceptance 件侧缺口直接回 acceptance-author，不进退 fixer。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包新增 `qa_session_timeout.c` 进 `firmware/main/CMakeLists.txt`，漏登记 = 链接错误，设备编译闸必须真跑或如实申报 degraded）。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`（常驻分支 `dev/261016-sd-card-mount`，已 reset 到 main `aecded8b`），树内 tracked 净、host 与 firmware `build/` 热缓存在场、managed_components 在场；专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 已按 main 刷新（nodes 20632 / edges 59105 = expected）。交接卫生已核：host_app sdkconfig 已删并重生、无其它 untracked 残留。
- **真机门前停等预期**：行为半（麦克风真的关、记录会话真的不关）host 不可闭环，验收含 human 真机目检六项（plan ⑩：原报告路径复现 / 日常 60s / 记录会话静默 >90s 不关 / 连续说话跨 60s 不被切 / 唤醒词入口 / 5s 常驻重建）。planner 跑完代码面断言 + 设备编译闸后在真机门前停（status=`awaiting-human:real-device`，按现有闭集可退 `blocked` + 事件），不自行烧录。
- **合并门预期**：riskTier=high 会触发 supervisor 机械闸 ESCALATE，需 human 批准；契约面零触碰。姊妹包刚合 main，`EndVoiceSession` 可直接扩 cause 形参，无 dependsOn。
- **会话标识**：主线程 headless 拉起（2026-10-24 点火；启动后回填 PID）。

## 命令行（worktree = C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch，分支 dev/261016-sd-card-mount）

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`

② 无人值守 headless：
`CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261024-qa-session-timeout-producer —— 从 delivery/taskpackages/261024-qa-session-timeout-producer/plan.md 读 reviewed plan 做 intake；真机目检项到门即停等 human，见本启动卡真机门前停等预期"`
