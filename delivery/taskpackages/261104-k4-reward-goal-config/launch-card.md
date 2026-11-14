# 启动卡 · 261104-k4-reward-goal-config

跑 261104-k4-reward-goal-config（261103-star-points-story-cover 列车，mergeOrder 75，dependsOn=[K1, K2, K3]，full / riskTier **high**，gateProfile=[evidence-integrity, test-quality, state-machine, mcp-tool-contract]）。奖励场景与激活目标：四预置 + 通用旗标资产、QQ 自定义/切换/删除、唯一激活、切换不扣分、刻度与兑换联动。

- **调度特例（human override，必须记录）**：LP12 要求「K4 等 K1、K2、K3 merged 后方可 launch」。实况 = K1/K2 已 merged，**K3 尚未 merged**（supervisor ESCALATE，等 human 批 + 明天上午真实 QQ 设备轮）。human 2026-11-15 明确要求「继续开发K4，中国时间明天上午到时候一块验证」→ **K4 叠在未合并的 K3 之上开工**。合并序仍钉死 K3 → K4；K4 的合并门前必须先完成 K3 合并。同步到 K4 的真机轮安排：与 K3 的 live 轮合并，明天上午一次设备窗跑完（K3Q-E4 + K4G-E4）。
- **基线锚**（带锚引用三件齐；落盘锚 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-15 K4 launch 段）：
  - server 面 = **435 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `cad07f2c`（2026-11-15 实测，本段同日现跑；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`；面域 = `server`）。该基线已含 K3 的全部 server 改动（K3 未合并但不影响本树基线口径）。
  - firmware host 面 = **287 Tests / 3 Failures / 0 Ignored**（3 红全 p1_auxbar AB 存量族；量测态 = K2 final `c1de02fe`）。判停 2026-11-15：`git diff --quiet 76ed332e HEAD -- firmware/` exit 0（K3 零固件改动）→ 引用不重跑。**本包动 firmware/ 后该引用失效**，builder 必须按 K0 锚命令重跑 host 基线并判新增红。
  - 本包验收红基线（acceptance-author 新冻结件）恒现跑。
- **本包 base ref**：`cad07f2c`（K3 未合并 HEAD = 本包包首提交父；常驻分支 dev/pod-m2）。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261104-k3-qq-points-channel/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。另注：K3 的真实 QQ + 设备轮**未跑**，已顺延到 2026-11-16 上午与 K4G-E4 合并一次设备窗；K3 的 skill/CLI 已部署到 ECS（`cad07f2c`，drift 0），live 侧 CLI `show/recent` 实测可用。
- **铁律约束**（taskpackage plan.md 全文为准）：预置恒为巧克力 20 / 看电影 30 / 游乐场 40 默认 / 礼物 50；预置不可删；自定义只接受 5–200 正整数；激活自定义必须先切换再删；同刻唯一激活、不出现「暂无目标」页；设备端不设目标，配置只走 QQ。切目标不自动扣分，之后兑换只认新目标分值，旧分值拒答且不写账。资产标识只取 `sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag`，自定义恒用 `sc-flag`，不猜图、不逐个定制、不回用 `sc-castle`。不新增字体/分区/设备设置页，不手改生成后的二进制；不改 requirements / architecture / `docs/contracts/**`；需要新资产标识或线格式时立即停并升级。
- **跨包串行**：K4 与 K5 共用资产管线；K4 先产五枚场景资产，K5 后序补 `ic-coin` 与书封——两包不得并行改生成器、资产清单或生成件。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":false,"acceptanceRefs":[],"hostUncompilablePaths":[],"reason":"设备可见的场景标与刻度由 host 可编 points_page.c 与既有渲染缝产出，本包 changeManifest 不含 host 不可编路径；真实设备轮 K4G-E4 仍必做，不走机械 deviceOutcomeGate"}}

  - **复核点**：Step 1g 若把 `firmware/main/display/kidpal_render_node.cc`（host 不可编登记路径）扩进 changeManifest，本行必须在合并前改为 `userVisible:true` 并列入该路径 + acceptanceRefs（否则前置升级闸 fail-closed）。
  - 验收文件不得添加 `device-outcome:` / `host-uncompilable-path:` 标记，除非上述复核点已改。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（K3 未合并，常驻分支 dev/pod-m2=cad07f2c）；专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-15 实查）。
- **会话标识**：VS Code 扩展会话内联 planner（human 指示非 headless，2026-11-15 起）。
- **命令行**（两形态并列）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`（K1/K2/K3 先例）。