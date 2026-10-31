# 启动卡 · 261028-m6-k2a-p1-home-16px

跑 261028-m6-k2a-p1-home-16px（261028-font-typography-scaleup 列车 P1，mergeOrder 61，dependsOn=[P0 已完成]，full / riskTier high）。

- **基线锚**（带锚引用三件齐）：
  - firmware host 面 = **250 Tests / 3 Failures** @ SHA `789c0244`（P0 绿跑 2026-10-28 实测；命令 = WSL `bash firmware/main/test/host_app/espidf_wsl.sh build` + `./firmware/main/test/host_app/build/host_infra_test.elf`；3 红全为 p1_auxbar AB-A1/A2/A4 存量族；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-10-29 段 + P0 review-packet）。**差异判停 2026-10-29 本树实测**：`git diff --quiet 789c0244 HEAD -- firmware/` exit 0 → 引用不重跑。本包动工后 firmware 面有 diff，intake/verifier 首轮见红先对此基线。
  - server 面 = **370 passed / 7 deselected / 33 subtests passed / 0 failed** @ SHA `6ea7769b`（2026-10-28 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`；落盘锚 = `delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/launch-card.md`）。**差异判停**：`git diff --quiet 6ea7769b HEAD -- server/` exit 0 → 引用不重跑。本包 crossEnd counterparts（server/gateway/schedule.py home_upcoming_payload）**零 producer diff**——wire 输入不变。
  - 本包验收红基线（acceptance-author 新冻结断言 + supersede 重基线）恒现跑。
- **本包 base ref**：prep 提交（含本启动卡）之后的 HEAD，即包首提交父；常驻分支 `dev/pod-m2`。包级 diff 恒用此 ref（= P0 冻结面 789c0244 + P0 收尾制品），不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261028-m6-k1-font-assets-runtime-loading/pod-retro.md`（同树上一包 P0，verifier-green 后置合并）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。相关三条：F1 树扫弃用（结构断言走 FAS-MANIFEST configure 期采集机制）；F2「未验证前提最便宜一步实测先行」；F4 变异证据回填先读闸匹配逻辑（acceptance 全文长串 + turnedRed）。
- **串行边**：P0/P1 共享 `kidpal_render_node.cc` 与 host 字体 seam——P0 已 commit（789c0244），P1 在该冻结面上切首页消费，不得并行改共享 seam；`kidpal_p1_font.h` 在 P0 changeManifest 外、本包 manifest 内。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开）。本包触 `home_p1.c` / `kidpal_render_node.cc`（设备专有 TU），firmware-build 相位必须真跑。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，P0 全绿在树）；树内固件环境在位（sdkconfig / managed_components 软链 / host build 热缓存）；专属图索引 `C-AI-KidPal-pod-m2` 在册。spike-first launch 前置由 human 2026-10-29 裁定重排：P0 已 commit + 全绿即满足「P0 完成」，FAR-E3 真机轮后置到 P1 后统一轮（pod-events human-merge-gate-deferred 事件在案）。
- **会话标识**：本会话 = VS Code 扩展会话，changeId 直启（无 `--settings` / `--agent`，**settings-presence = extension-session**）——dev-loop.settings.json 三件按缺席处理（Stop supervisor-gate-check / PostToolUse portfolio-sync / permissions.ask git merge/rebase）；收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`；起步已在 pod-events 记 settings-presence。
- **命令行**（两形态并列，供日后同形包复用）：
  - ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  - ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
- **真机轮预检**：本包不另占真机轮（P1H-E4）——首页真机观感 + 缺 assets 降级复证并入 P3 终端收口轮（即 human 裁定的 P0+P1 统一真机轮载体，P3 launch/收口前 P1 必须 merged）。
