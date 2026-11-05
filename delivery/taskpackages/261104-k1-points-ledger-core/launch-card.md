# 启动卡 · 261104-k1-points-ledger-core

跑 261104-k1-points-ledger-core（261103-star-points-story-cover 列车，mergeOrder 72，dependsOn=[261104-k0a-four-page-skeleton, 261104-k0-points-e2e-spike] 均 merged，full / riskTier **critical**，gateProfile=[evidence-integrity, test-quality, real-db, state-machine, mcp-tool-contract]）。K0 最小通路（points.py 482 行）正式化为唯一确定性 writer；**GAP-1 承接包**：K0 §六.1 交办的「QQ 落账后设备缓存同步推送（restore=True 活调用方）缺位」在本包 producer 正式化时一并接线（planner intake 判定吸收，见 workstream progress.md 2026-11-05 段）。

- **基线锚**（带锚引用三件齐；落盘锚 = `delivery/workstreams/261103-star-points-story-cover/progress.md` 2026-11-05 段）：
  - server 面 = **401 passed / 0 failed / 33 subtests / 9 deselected** @ SHA `85cc3747`（2026-11-05 实测，本段同日现跑；命令与五要素见锚行）。旧锚 6ce77123 因 K0 改 server/ 已失效，**以 85cc3747 行为准**。
  - firmware host 面：本包 changeManifest 全 server、零固件文件，验证不涉 host build；host 已知红沿用 K0 收尾留痕（278/3 @ K0a merged 后，见 261104-k0-points-e2e-spike taskpackage K0S-E4 行），verifier 见 host 红按该留痕判新增红。
  - 本包验收红基线（acceptance-author 新冻结齿）恒现跑。
- **本包 base ref**：`85cc3747`（prep 后树 HEAD = 包首提交父；常驻分支 `dev/pod-m2`，2026-11-05 已 ff 到 main）。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。
- **上一包留根**：`delivery/taskpackages/261104-k0a-four-page-skeleton/pod-retro.md`（同树上一包）+ `delivery/taskpackages/261104-k0-points-e2e-spike/spike-evidence.md` §六边界发现（merge order 上一包）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。
- **铁律约束**（taskpackage plan.md 全文为准）：余额与前后余额全由剩余分录派生、入口不得接收模型提交的权威余额/目标 id/分录 id/revision/游标作写入事实；原子写覆盖回执一致性，同 command_id 重放返首次结果；删除重算允许负余额（OQ-1）与兑换资格约束（余额 ≥ 激活目标分值）不得互相钳位；短窗语义去重（起步 10 分钟、注入时钟）与 command_id 传输幂等分离；keyset 分页 `(occurred_at,id)` 时间倒序、默认 limit=6、允许 1–50，禁止复用 story_list 16 条窗口或 offset；四预置不可删、激活自定义禁删、自定义 5–200 正整数；不改 requirements / architecture / `docs/contracts/**` / 故事数据域，发现线格式或语义缺口立即停并升级。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包零固件 diff，firmware-build 闸按空集处理）。
- **验证形态申报**（机器消费行，闸按顶格前缀解析）：

验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":false,"acceptanceRefs":[],"hostUncompilablePaths":[],"reason":"none"}}

- **跨包交接面**：K1 是数据语义 owner——K2 消费权威余额/分页/删除重推（设备端不重算），K3 复用同一 CLI 与回执形状（不建第二套规则），K4 在本包存储校验上做配置交互。K1L-E6 要求 producer/CLI 与回执形状有可读交接证据；本包不占真机 UI 配额，消费端复核结果交 K2/K3 复证。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（上一包 K0a 已 merged，常驻分支 `dev/pod-m2` 已 ff 到 main=85cc3747）；专属图索引 `C-AI-KidPal-pod-m2` 在册随树复用（2026-11-05 index_status ready，K0 新增 points 面已入图）；`host_app/sdkconfig` 现役在位、`sdkconfig.pre-launch-bak-20261104` 为 K0a 前备份残留（本包不涉固件构建，留待 K2 换包交接时清理）。
- **会话标识**：2026-11-05 于本树以 VS Code 扩展会话直启（human 指示「以 Planner 形式在本会话跑，不 headless」）；扩展会话降级形态，settings-presence 事件已落 pod-events.jsonl。
- **命令行**（两形态并列，标注适用条件；本包实际以扩展会话内联形态跑）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ dev-loop.settings.json 三件（Stop supervisor-gate-check 门 / PostToolUse portfolio-sync 自动 reconcile / permissions.ask 的 git merge/rebase 三件）按缺席处理；起步在 pod-events 记 settings-presence（declaredForm=extension-session），收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`。
