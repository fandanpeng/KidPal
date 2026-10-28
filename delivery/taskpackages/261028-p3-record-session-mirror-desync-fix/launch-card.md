# 启动卡 · 261028-p3-record-session-mirror-desync-fix

跑 261028-p3-record-session-mirror-desync-fix（单包，非拆包产物，mergeOrder null）。

- **基线锚**（带锚引用三件齐；producer 五要素）：
  - firmware host 面 = **237 Tests / 3 Failures** @ SHA `aecded8b`（3 条全为 p1_auxbar AB-A1/A2/A4 先存红，与 261022 锚 `0df2c0c3` 同一族；命令 = WSL `bash firmware/main/test/host_app/espidf_wsl.sh build` + `./firmware/main/test/host_app/build/host_infra_test.elf`；落盘锚 = `delivery/taskpackages/261024-qa-session-timeout-producer/launch-card.md` 基线锚行）。**差异判停 2026-10-28 本树实测**：`git diff --quiet aecded8b HEAD -- firmware/` exit 0 → 引用不重跑。
  - server 面 = **370 passed / 7 deselected / 33 subtests passed / 0 failed** @ SHA `6ea7769b`（2026-10-28 本树实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`）。因 `aecded8b..HEAD` server 面有 diff 故重跑，未引用旧锚。
  - 本包验收红基线（acceptance-author 新冻结断言）恒现跑，无缓存。
- **本包 base ref**：prep 提交（含 plan / taskpackage / 本启动卡）之后的 HEAD，即包首提交父；常驻分支 `dev/pod-m2`。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261025-openclaw-dreaming-disable/pod-retro.md`（同树上一批，已 merge）。intake 逐条评估，构成 blockingDecision 即 fail-closed 上报。本包直接相关一条：M2 插行不得插在 `kidpal_keybar_scene_reset` 与其判据之间（CG 窗口齿）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)）。本包 full、无 defer 短路；且触 `application.cc` / `kidpal_render_node.cc` / `mcp_server.cc` 三个设备专有 TU（host 构建不覆盖），firmware-build 相位必须真跑。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，点火时与 main 同点 `6ea7769b`，历史 Pod 全 merged、无其它活跃 Pod 占用）；树内固件环境在位（sdkconfig / dependencies.lock / host_app sdkconfig / managed_components 软链 / host + firmware `build/` 热缓存）；专属图索引 `C-AI-KidPal-pod-m2` 在册且 ready（nodes 20782 / edges 60008）。
- **会话标识**：本会话 = VS Code 扩展会话，changeId 直启（无 `--settings` / `--agent`，**settings-presence = extension-session**）——dev-loop.settings.json 三件按缺席处理：Stop 的 `supervisor-gate-check.mjs`、PostToolUse `portfolio-sync` 自动 reconcile、permissions.ask 的 git merge/rebase；收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`。
- **命令行**（两形态并列，供日后同形包复用）：
  - ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  - ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  - 非 CLI 形态（VS Code 扩展 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ 不加载 `dev-loop.settings.json`，三件按缺席处理；per-agent allow/deny 对主线程恒不生效（`enforce-scope.mjs:9`）。起步已在 pod-events 记 `settings-presence`（`declaredForm=extension-session`）。
- **真机轮预检**：非 lite/nano 包，环境已就位；真机轮（RSL2-A5 T1-T5）烧录前核 `VERIFY_FW_BUILD_REQUIRED=1` 的真编译产物，按键口径 = **KEY 长按**（BOOT 长按在基层是空分支、录音态才是丢弃）。
