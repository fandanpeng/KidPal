# 启动卡 · 261104-k3-qq-points-channel

跑 261104-k3-qq-points-channel（261103-star-points-story-cover 列车，mergeOrder 74，dependsOn=[261104-k1-points-ledger-core, 261104-k2-points-device-ui] 均已 merged，full / riskTier **high**，gateProfile=[evidence-integrity, test-quality, state-machine, mcp-tool-contract]）。QQ 家长积分通道正式化：新建 `kidpal-points-write` skill，复用 K1 CLI 与 K2 restore 消费缝，提供加/兑/查/删与一次性普通文本错误回复，全程无 `ask_user`。

- **基线锚**（带锚引用三件齐；落盘锚 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-14 K3 launch 段）：
  - server 面 = **415 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `45518ac2`（2026-11-14 实测，本段同日现跑；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；量测时 HEAD = 45518ac2；面域 = `server`）。旧锚 b96b978d（411/0）因 K1/K2 改 server/ 判停失效，**以 45518ac2 行为准**。
  - firmware host 面 = **287 Tests / 3 Failures / 0 Ignored**（3 红全 p1_auxbar AB 存量族；K2 final 轮实测态 `c1de02fe`，且 `76ed332e..HEAD -- firmware/` 为空，合并后无固件改动）。本包 changeManifest 全 server、零固件文件，验证不涉 host build；真机轮烧录用现役 main 固件即可。
  - 本包验收红基线（acceptance-author 新冻结件）恒现跑，无缓存可言。
- **本包 base ref**：`45518ac2`（prep 前树 HEAD = 包首提交父口径锚；本卡与 progress 锚随 prep commit 叠于其上落库）。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261104-k2-points-device-ui/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。另注：K2 真机轮（K2PTS-L1/L2/L3，含 K1L-L1/L2 挂接）与 K3 的 K3Q-E4 设备观察共用同一 human 在场设备窗，部署轮仍归主线程。
- **LP11 前置**：K1/K2 已 merged（merge 双证在案）；真实 QQ 验证窗口 + 设备联调条件由 launch 执行者/human 确认——真实歧义输入不卡死是必做证据，设备轮阶段前须 human 在场。
- **铁律约束**（taskpackage plan.md 全文为准）：所有加/兑/查/删只调 points.py CLI 并以真实执行回执回复；不得直接编辑状态文件、不得自算第二套余额、不得写 schedule.md、不得把模型生成的分录 id 当事实。不调用 `ask_user`、不依赖多轮澄清；无法唯一理解时普通 QQ 文本一次说明正确说法；删除只用稳定记录 id 或最近列表序号定位。兑换只认激活目标分值且真实兑现后扣，同一次兑换不重复扣，不提供惩罚性扣分入口；查账回答来自真实状态。QQ 推送永不抢前台；本包不新增设备设置页、不提前实现 K4 目标管理，不宣称修复 TECH-261030 / TECH-261012 / TECH-261023。不改 requirements、architecture、`docs/contracts/**` 或 workspace-kidpal 数据只读边界；需要扩大写权或直接文件写时立即停并升级。
- **传输幂等**：QQ command_id 来自平台消息 id 加命令序号；新的家长消息即使文案相同也必须新建 command_id，再交 K1 短窗语义去重，skill 不得自行吞重。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":false,"acceptanceRefs":[],"hostUncompilablePaths":[],"reason":"none"}}

  - 口径：本包 changeManifest 全 `server/**`（skill 文本 + points.py + front 推送 + 验收测试），合并 diff 不含 host 不可编固件路径，机械 deviceOutcomeGate 不触发（K1 先例同型）。**因此验收文件不得添加 `device-outcome:` / `host-uncompilable-path:` 标记**——无固件路径入 diff 时该标记会把闸推到「必须列 hostUncompilablePaths 且在 diff 内」而必然红。真机轮义务仍由 exitCriteria K3Q-E4 以 live-manual 证据面承担（设备看屏/串口 + front/points/QQ 日志），由 integrator/human 判读，不由机械闸代替。
- **跨包交接面**：K1 拥有全部命令语义（points.py 单 writer），K2 拥有 restore/原地刷新消费缝；K3 只建 skill 通道并用真实 QQ 串起两端，不绕开任一缝、不另写设备渲染分支、不合并两套 total。K4 在同一 skill 上新增目标配置，不另建通道。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（上一包 K2 已 merged，常驻分支 dev/pod-m2=45518ac2）；专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-14 实查，21960 节点 / 64713 边）。
- **会话标识**：VS Code 扩展会话内联 planner（human 指示在本 worktree 非 headless 启动，2026-11-14 起）——叫停时核「进程确实已死」用。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件（Stop supervisor-gate-check 门 / PostToolUse portfolio-sync 自动 reconcile / permissions.ask 的 git merge/rebase 三件）按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`（K1/K2 先例照办）。