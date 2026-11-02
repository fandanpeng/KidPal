# 启动卡 · 261028-m6-k2b-p2-schedule-16px

跑 261028-m6-k2b-p2-schedule-16px（261028-font-typography-scaleup 列车 P2，mergeOrder 62，dependsOn=[P0 已 merged]，full / riskTier high，gateProfile=[evidence-integrity, test-quality, device-round]）。

- **基线锚**（带锚引用三件齐）：
  - firmware host 面 = **253 Tests / 3 Failures / 0 Ignored** @ SHA `6ce77123`（2026-11-02 本树实测；命令 = WSL `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`；3 红全为 p1_auxbar AB-A1/A2/A4 存量族，与 P0 锚 `589e0127`（242/3）、P1 锚（253/3）同族红名单零变化；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-11-02 段）。**注意**：本次实测前 host_app sdkconfig 已按交接卫生删除、从 tracked `sdkconfig.defaults` 重生——基线即为重生配置下的真值。P1 锚 `789c0244` 差异判停有 diff（P0+P1 已合 main）→ 本锚为强制重测产物，不引用旧锚。
  - server 面 = **395 passed / 9 deselected / 33 subtests passed / 0 failed** @ SHA `6ce77123`（2026-11-02 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`；落盘锚 = 同上 progress.md 段）。旧锚 `6ea7769b` 差异判停有 diff（261031 批过 server 面）→ 重测。本包 crossEnd counterpart（`server/gateway/schedule.py` `week_grid_payload`）**零 producer diff 预期**——wire 输入不变，云端仍全量下发（F-8）。
  - 本包验收红基线（acceptance-author 新冻结断言 + 收紧型 supersede 重基线）恒现跑，无缓存可言。
- **本包 base ref**：prep 提交（含本启动卡 + progress 追加）之后的 HEAD，即包首提交父；常驻分支 `dev/pod-m2`。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261028-m6-k2a-p1-home-16px/pod-retro.md`（同树同列车上一包 P1，已 merged）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。中间隔的 `261031-openclaw-cron-trigger-gating`（已 merged）无 pod-retro.md、与本列车无文件交集，不作留根来源。
- **依赖与并行边**：P0（16px 统一句柄 + 统一降级）与 P1（`kidpal_render_node.cc` 首页消费）均已 merged 本地 main——本包只消费 P0 句柄，不在 `schedule_page` 内另写资产加载（crossPackageBehaviors 第 1 条）。P3 无文件交集，但本包 merged 前 P3 不得收口（crossPackageBehaviors 第 3 条）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开）。本包触 `schedule_page.c`（设备专有 TU），firmware-build 相位必须真跑。
- **验证形态申报（JSON）**：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":false,"acceptanceRefs":[],"hostUncompilablePaths":[],"reason":"none"}}
- **ARQ-261029 注记（真机轮预期管理）**：上游 ※ 写入侧任务仍待派单——P2 下划线消费 `flags bit0`（wire 语义不变），若真机轮当周课表无临时课样本，下划线观测按「观测受限」记录、不记失败（P1 统一轮下划线休眠同先例）；※ 字形探测/程序化兜底退役照常执行（E3）。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，已 ff 到 main `6ce77123`）；树内固件环境在位（managed_components 软链原生 python 验真、host build 热缓存、sdkconfig 已按交接卫生重生）；专属图索引 `C-AI-KidPal-pod-m2` 在册（随树复用，自动增量）。交接卫生 2026-11-02 实测：tracked 净、零 untracked。
- **会话标识**：本会话 = VS Code 扩展会话，changeId 直启（无 `--settings` / `--agent`，**settings-presence = extension-session**）——dev-loop.settings.json 三件按缺席处理（Stop supervisor-gate-check / PostToolUse portfolio-sync / permissions.ask git merge/rebase）；收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`；起步在 pod-events 记 settings-presence。
- **命令行**（两形态并列，供日后同形包复用）：
  - ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  - ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
- **真机轮预检**：本包自有真机轮一轮（P2S-E5）——看 pitch 19 紧凑观感、4/4/3 容量、晚上第 4 门静默截断、下划线/★ 与长课表现；环境已就位无需 lite 注记。结果供 P3 终端收口轮复证。
