# 启动卡 · 261104-k0-points-e2e-spike

跑 261104-k0-points-e2e-spike（261103-star-points-story-cover 列车，mergeOrder 71，dependsOn=[]，full / riskTier high，gateProfile=[evidence-integrity, test-quality]）。**spike-first 命中包（ARQ-261024）**：以设备真看到权威余额与原因为通过证据，不以文件写入 / CLI 返回 / pytest 读回自证（K0S-E1）。

- **基线锚**（带锚引用三件齐；落盘锚与判停结论 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-04 段）：
  - server 面 = **395 passed / 0 failed** @ SHA `6ce77123`（2026-11-02 实测；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-11-02 P2 段 server 锚行）。差异判停 2026-11-04：`git diff --quiet 6ce77123 HEAD -- server/` exit 0 → **引用不重跑**。本包 server 侧新增 points 文件属新验收面、无缓存可言。
  - firmware host 面 = **265 Tests / 3 Failures** @ SHA `2f2c5871`（落盘锚 = 同上 P3 段）。差异判停 2026-11-04：`firmware/` 零 diff → 引用不重跑；本包 host build / device build / 真机终验**后置到 K0a merged 后**（LP8），届时按判停协议对 `firmware/` 重取差异。
  - 本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **本包 base ref**：`22b30654`（prep 后树 HEAD = 包首提交父；常驻分支 `dev/261016-sd-card-mount`，2026-11-04 已复位）。包级 diff 恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261024-qa-session-timeout-producer/pod-retro.md`（同树上一包，已 merged）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开）。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":true,"acceptanceRefs":["S-F6.1","S-F6.3","NFR-PERF-4"],"hostUncompilablePaths":["firmware/main/display/points_page.c","firmware/main/display/kidpal_render_node.cc","firmware/main/mcp_server.cc"],"reason":"launch 预估：spike 的通过证据即设备看屏（K0S-E1/E3），真机终验按 LP8 后置到 K0a merged 后；points_page.c 为新增文件、host 编入待共享 CMake 追加时定，kidpal_render_node.cc 与 mcp_server.cc 为设备专属/设备侧 TU——Pod intake 按实际 diff 收窄"}}

- **QQ 取证路径（LP7 裁定，human 在场项）**：真实 QQ 一次性明确指令（带唯一 canary token）→ 既有 main agent → K0 最小 add CLI；**首选两笔同形 canary**（第一笔设备在其他页、第二笔积分页在屏）覆盖 K0S-E3 双状态。四条禁令：禁直编 points/state 文件、禁终端调 CLI 冒充 QQ、禁本地 OpenClaw 注入冒充、禁提前建 K3 正式 skill。预评通过 ≠ 执行预绿，证据由 spike-evidence.md 判定。
- **跨包约束（LP8）**：`firmware/main/test/host_app/main/CMakeLists.txt` 为两包唯一共享构建清单——K0a 先落；本包在其 merged、重取基线后才追加登记并做 host/device build 与真机终验；不修改 K0a 的 render_node/keybar/story_page 文件，P3 接管使用 K0a 留下的缝。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`（常驻分支 `dev/261016-sd-card-mount`，2026-11-04 复位到 22b30654）；固件环境在位（managed_components 软链；host_app sdkconfig 已改名 `sdkconfig.pre-launch-bak-20261104`）；专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 在册随树复用。
- **会话标识**：点火后由 launch 执行者回填。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件（Stop supervisor-gate-check 门 / PostToolUse portfolio-sync 自动 reconcile / permissions.ask 的 git merge/rebase 三件）按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`。
