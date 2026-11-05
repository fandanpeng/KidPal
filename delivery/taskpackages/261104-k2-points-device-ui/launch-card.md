# 启动卡 · 261104-k2-points-device-ui

跑 261104-k2-points-device-ui（261103-star-points-story-cover 列车，mergeOrder 73，dependsOn=[261104-k1-points-ledger-core] 已 merged @ ae7725cf，full / riskTier **high**，gateProfile=[evidence-integrity, test-quality, state-machine, mcp-tool-contract]）。设备积分本正式化：points_page.c 从 K0 spike 最小渲染换为积分主页（四态/最近三条/负余额如实显示）+ 详细账本页（keyset 取页/删除确认覆盖层）+ 积分语境与语音闭环。

- **基线锚**（带锚引用三件齐；落盘锚 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-05 K2 launch 段）：
  - server 面 = **411 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `b96b978d`（2026-11-05 实测，本段同日现跑；命令与五要素见锚行）。旧锚 85cc3747（401/0）因 K1 改 server/ 判停失效，**以 b96b978d 行为准**。
  - firmware host 面 = **278 Tests / 3 Failures / 0 Ignored**（3 红全 p1_auxbar AB 存量族；K0 收尾留痕，落盘锚 = 261104-k0-points-e2e-spike taskpackage K0S-E4 / LP8 段）。判停 2026-11-05：`git diff --quiet ace646e3 HEAD -- firmware/` exit 0（K0 合并态→现 HEAD 固件面零差异）→ **引用不重跑**；本包 builder 动 firmware/ 后该引用失效，verifier 见红先对 3 条存量族判新增红。
  - 本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **本包 base ref**：`b96b978d`（prep 后树 HEAD = 包首提交父口径锚；本卡与 progress 锚随 prep commit `c4fcd619` 落库）。包级 diff（终验 scope ⊆ changeManifest 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261104-k1-points-ledger-core/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。**特别挂接**：K1L-L1/L2 真机轮（QQ 落账不抢前台/原地刷新 GAP-1 活性 + 真实消费端分页对账）按 human 2026-11-05 意向挂本包真机轮一起跑，planner intake 判定吸收进真机轮计划；部署轮（points.py 铺放 + state 迁移）仍归主线程，不在本包 scope。
- **铁律约束**（taskpackage plan.md 全文为准）：设备不重算余额、不本地持久化账本（RAM 缓存可恢复重建，不写 NVS/SD）；删除只走按键确认 + kidpal_points_delete 私有上行，语音不开放删除；完整句落账跳页、不完整句普通对话零写入；restore 与显式导航分流、其他页推送不抢前台；keyset 分页消费 K1 producer 形状（不复用 story 16 条窗口）；不改 requirements / architecture / `docs/contracts/**`，发现线格式或语义缺口立即停并升级。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 diff 触 firmware 设备面，device build 闸必须真编译）。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":true,"acceptanceRefs":["S-F6.1","S-F6.2","S-F6.3"],"hostUncompilablePaths":["firmware/main/mcp_server.cc","firmware/main/application.cc","firmware/main/protocols/protocol.cc"],"reason":"launch 预估：语音记账/删除确认/冷启动恢复为设备可见行为（K2D-E3 真机轮核验）；mcp_server.cc 与 application.cc 为设备专属 TU（K0a 先例在册），protocol.cc 为上行帧构造面——Pod intake 按实际 diff 收窄，points_page.c 沿 K0 先例 host 可编不预列"}}

- **跨包交接面**：K2 建立 restore/原地刷新与进度线消费缝——K3（QQ 到账不抢前台复证）与 K4（目标切换刻度更新复证）复用，两包不另写设备渲染分支；积分数字与故事星星独立，不合并两套 total（K5 收口对表）。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（上一包 K1 已 merged，常驻分支 dev/pod-m2=b96b978d）；专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-05 实查，21740 节点/64106 边，K1 points 面在图）；host_app 现役 `sdkconfig` 在位，`sdkconfig.pre-launch-bak-20261104` 残留已按 K1 卡预告清理。
- **会话标识**：〈点火后由 launch 执行者回填——叫停时核「进程确实已死」用〉
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件（Stop supervisor-gate-check 门 / PostToolUse portfolio-sync 自动 reconcile / permissions.ask 的 git merge/rebase 三件）按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`（K1 先例照办）。
