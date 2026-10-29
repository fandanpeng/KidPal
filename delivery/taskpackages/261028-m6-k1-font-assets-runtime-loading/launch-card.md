# 启动卡 · 261028-m6-k1-font-assets-runtime-loading

跑 261028-m6-k1-font-assets-runtime-loading（261028-font-typography-scaleup 列车 P0 spike，mergeOrder 60，dependsOn 空，full / riskTier high）。

- **基线锚**（带锚引用三件齐；producer 五要素落盘）：
  - firmware host 面 = **242 Tests / 3 Failures** @ SHA `589e01275aced4d2f04bc22d4ea796eb25257d8f`（2026-10-28 本树实测；命令 = WSL `bash firmware/main/test/host_app/espidf_wsl.sh build` + `./firmware/main/test/host_app/build/host_infra_test.elf`；3 条红全为 p1_auxbar AB-A1/A2/A4 存量族，与锚 `aecded8b`（237/3）同族，零新增红；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-10-28 段）。本包动工后 firmware 面有 diff，intake/verifier 首轮见红先对此基线，新增红才是本包责任。
  - server 面 = **370 passed / 7 deselected / 33 subtests passed / 0 failed** @ SHA `6ea7769b`（2026-10-28 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`；落盘锚 = `delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/launch-card.md` 基线锚行）。**差异判停 2026-10-28 本树实测**：`git diff --quiet 6ea7769b HEAD -- server/` exit 0 → 引用不重跑。本包 changeManifest 零 server 面。
  - 本包验收红基线（acceptance-author 新冻结断言）恒现跑，无缓存。
- **本包 base ref**：prep 提交（含 plan / taskpackage / 本启动卡 / workstream progress.md）之后的 HEAD，即包首提交父；常驻分支 `dev/pod-m2`。包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。SHA 以 `pod-events.jsonl` 的 `pod-started` 事件所记为凭。
- **上一包留根**：`delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/pod-retro.md`（同树上一批，已 merged）。intake 逐条评估，构成 blockingDecision → fail-closed 上报。相关一条：「设备 TU 不进 host 构建的包，验收设计一开始就把三分申报写死」——本包 assets/cbin 装载面与 host seam 的 host 可验 / 设备专有切分在 acceptance 设计时即按此申报。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)）。本包 full、无 defer 短路；且触 assets 8MB 分区打包与 cbin 运行时装载（设备专有面，host 构建不覆盖烧写/mount），firmware-build 相位必须真跑。
- **环境事实**：复用池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，点火时 HEAD 589e0127 = main 357bc5fc 之上另载拆包制品；上一包 mirror-desync board 残记 awaiting-human:merge-gate 已修正为 merged，无其它活跃 Pod 占树）；树内固件环境在位（sdkconfig / dependencies.lock / host_app sdkconfig / managed_components 软链 / host build 热缓存）；专属图索引 `C-AI-KidPal-pod-m2` 在册（ARQ-260905 ④）。
- **会话标识**：本会话 = VS Code 扩展会话，changeId 直启（无 `--settings` / `--agent`，**settings-presence = extension-session**）——dev-loop.settings.json 三件按缺席处理：Stop 的 `supervisor-gate-check.mjs`、PostToolUse `portfolio-sync` 自动 reconcile、permissions.ask 的 git merge/rebase；收尾相位手工跑 `supervisor-gate-check.mjs` 与 `portfolio-reconcile.mjs`。
- **命令行**（两形态并列，供日后同形包复用）：
  - ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  - ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  - 非 CLI 形态（VS Code 扩展 / IDE 内会话）降级面：不能传 `--settings` / `--agent` ⇒ 不加载 `dev-loop.settings.json`，三件按缺席处理；per-agent allow/deny 对主线程恒不生效（`enforce-scope.mjs:9`）。起步在 pod-events 记 `settings-presence`（`declaredForm=extension-session`）。
- **真机轮预检**：full 包、环境已就位。本包为 spike-first 底座——真机探针轮（FAR-E3）是收口要件：看屏照片 + 串口日志证明 16px/20px 字形来自运行时资产，另做缺 assets 降级开机（不黑屏）。真机轮烧录前核 `VERIFY_FW_BUILD_REQUIRED=1` 真编译产物。
