# 启动卡 · 261028-m6-k3-p3-story-typography

跑 261028-m6-k3-p3-story-typography（261028-font-typography-scaleup 列车 P3 终端包，mergeOrder 63，dependsOn=[P0/P1/P2 全 merged]，full / riskTier high，gateProfile=[evidence-integrity, test-quality, device-round]）。

- **基线锚**（带锚引用三件齐）：
  - firmware host 面 = **258 Tests / 3 Failures / 0 Ignored** @ SHA `d8d37558`（2026-11-02 本树实测；命令 = WSL `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`；3 红全为 p1_auxbar AB-A1/A2/A4 存量族，与 P0 锚 `589e0127`（242/3）、P2 锚 `6ce77123`（253/3）红名单零变化；总数 253→258 = P2 开发期新增 schedule_grid_16px 断言随 P2 合入，预期内；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-11-02 P3 段）。差异判停：P2 锚对 `firmware/` 有 diff → 本锚为强制重测产物，不引用旧锚。
  - server 面 = **395 passed / 9 deselected / 33 subtests passed / 0 failed** @ SHA `6ce77123`（2026-11-02 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`；落盘锚 = 同上 progress.md P2 段 server 锚行）。差异判停 2026-11-02 复核：`git diff --quiet 6ce77123 HEAD -- server/` exit 0 → 引用不重跑。本包 crossEnd counterparts（`server/gateway/story.py` `list_stories` / `server/gateway/front/front_app.py` `_relay_display_blocks_to_device`）**零 producer diff 预期**——P3 只消费 display 块、wire 输入不变。
  - 本包验收红基线（acceptance-author 新冻结 story_typography 断言 + story_page_v2 收紧型 supersede 重基线）恒现跑，无缓存可言。
- **本包 base ref**：prep 提交（含本启动卡 + progress 追加）之后的 HEAD，即包首提交父；常驻分支 `dev/pod-m2`（已并入 main 4ee8f796 @ merge `d8d37558`）。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261028-m6-k2b-p2-schedule-16px/pod-retro.md`（同树同列车上一包 P2，已 merged）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。
- **依赖与并行边**：P0（16px/20px 运行时句柄 + 缺字回退 + 缺资产降级）、P1（首页消费）、P2（课表版式）均已 merged 本地 main——本包在列表/阅读真实消费点验证 P0 句柄语义（kidpal_font 统一句柄、20px→16px 回退），不另造字体加载分支（crossPackageBehaviors 第 1 条）；P1/P2 页面只在终端真机轮复证，不抢占其文件面。**本包 = 列车终端包**：P3T-E4 四页并集终端真机轮 + human 终判 = M6 workstream 收口标准（PROPOSAL §5）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开）。本包触 `story_page.c`（host 构建在编）+ `mcp_server.cc`（设备专有 TU，不进 host 构建）——设备面改动经 mcp_server.cc 触达，firmware-build 相位必须真跑。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":false,"acceptanceRefs":[],"hostUncompilablePaths":[],"reason":"launch 时预估 mcp_server.cc 会携改动故申报 true；实现轮实证其零 diff（两消费点随 story_page.h 常量 4→3 自动同步，FAS story_mcp_lines_sync=1 守卫），本包 diff 未触任何已登记 host 不可编路径——设备可见排版结果由 host 真渲染路径 STY 冻结断言承载（STY-A1..A6 以真实 lvgl 渲染 + 真实运行时字体资产出墨），终端轮 P3T-E4 目击独立落 device-round-evidence.md（2026-11-02 planner 修正申报，P2 同形先例同口径）"}}

- **ARQ-261029 注记（终端轮预期管理）**：上游 ※ 写入侧任务仍待派单——终端轮复证 P1/P2 页下划线时若当周课表无临时课样本，按「观测受限」记录、不记失败（P1 统一轮、P2 真机轮同先例）；P3 故事两态自身不消费 ※。
- **设计评审与豁免在案项**：design review 以 `docs/ui/V8/canvas-p3.png` + `p3-reader.png` 完成（uiux.designReviewRequired，证据随批落盘）；缺 assets 14px 降级真机观测已于 2026-10-29 human 裁豁免（P0 批裁定：14px 常显真机正常 + host 半 FAS-A4 已验回退逻辑 + 全量烧录恒带 assets），P3T-E4 该子项按豁免记录、不再重测。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，已并入 main）；树内固件环境在位（managed_components 软链原生 python 验真、host build 热缓存）；专属图索引 `C-AI-KidPal-pod-m2` 在册（随树复用，自动增量）。交接卫生 2026-11-02 实测：tracked 净、零 untracked。
- **会话标识**：本会话 = VS Code 扩展会话，changeId 直启（无 `--settings` / `--agent`，**settings-presence = extension-session**）——dev-loop.settings.json 三件按缺席处理（Stop supervisor-gate-check / PostToolUse portfolio-sync / permissions.ask git merge/rebase）；收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`；起步在 pod-events 记 settings-presence。
- **命令行**（两形态并列，供日后同形包复用）：
  - ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  - ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
- **真机轮预检**：本包终端真机轮一轮（P3T-E4）——一次目击四页新字号（故事列表 16px + 标题省略不压日期、阅读 16px 正文自然行 3 + 20px 居中省略标题、首页/课表复证）、长标题与超限截断、阅读翻页手感（F-4 真机终判）；human 终判后 M6 收口。环境已就位无需 lite 注记。
