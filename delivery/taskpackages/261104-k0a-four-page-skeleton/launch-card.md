# 启动卡 · 261104-k0a-four-page-skeleton

跑 261104-k0a-four-page-skeleton（261103-star-points-story-cover 列车，mergeOrder 70，dependsOn=[]，full / riskTier high，gateProfile=[evidence-integrity, test-quality]）。

- **基线锚**（带锚引用三件齐；落盘锚与判停结论 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-04 段）：
  - firmware host 面 = **265 Tests / 3 Failures** @ SHA `2f2c5871`（2026-11-02 实测，3 红全 p1_auxbar AB 存量族；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-11-02 P3 段）。差异判停 2026-11-04：`git diff --quiet 2f2c5871 HEAD -- firmware/` exit 0 → **引用不重跑**。
  - server 面 = **395 passed / 0 failed** @ SHA `6ce77123`（落盘锚 = 同上 P2 段 server 锚行）。差异判停 2026-11-04：`server/` 零 diff → 引用不重跑。本包不触 server。
  - 本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **本包 base ref**：`22b30654`（prep 后树 HEAD = 包首提交父；常驻分支 `dev/pod-m2`，2026-11-04 已复位）。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261028-m6-k3-p3-story-typography/pod-retro.md`（同树上一包，已 merged）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开）。本包触 `kidpal_render_node.cc`（设备专属 TU）——firmware-build 相位必须真跑。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":true,"acceptanceRefs":["S-F2.1","S-F2.2","S-F2.3"],"hostUncompilablePaths":["firmware/main/display/kidpal_render_node.cc","firmware/main/mcp_server.cc","firmware/main/application.cc"],"reason":"launch 预估：四页循环/页点/逐页 source_page 为设备可见行为（K0A-E3 真机轮核验），故事三 story 零回归由 host 冻结断言承载；kidpal_render_node.cc 为设备专属 TU，mcp_server.cc/application.cc 为设备侧消费面——Pod intake 按实际 diff 收窄"}}

- **跨包约束（LP8 对偶）**：`firmware/main/test/host_app/main/CMakeLists.txt` 是两包唯一共享构建清单，由本包先落；K0 等 K0a merged 后才追加登记。本包不得触碰 points 业务文件（K0 的新 points_page.c 经本包接管缝接入，见 K0A-E2）。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，2026-11-04 复位到 22b30654）；固件环境在位（managed_components 软链 + host build 热缓存）；`firmware/main/test/host_app/sdkconfig` 已改名 `sdkconfig.pre-launch-bak-20261104`（构建从 defaults 重生成，确认无误后可删）；专属图索引 `C-AI-KidPal-pod-m2` 在册随树复用。
- **会话标识**：点火后由 launch 执行者回填。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件（Stop supervisor-gate-check 门 / PostToolUse portfolio-sync 自动 reconcile / permissions.ask 的 git merge/rebeke 三件）按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`。
