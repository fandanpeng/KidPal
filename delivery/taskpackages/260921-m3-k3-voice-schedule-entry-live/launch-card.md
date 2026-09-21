# Launch Card · 260921-m3-k3-voice-schedule-entry-live

跑 260921-m3-k3-voice-schedule-entry-live（workstream m3-schedule，mergeOrder 35，riskTier medium / rigor full）。

## 基线锚（m3-schedule progress.md 带锚行 · 260915 T3 差异判停协议）

两面在 launch-prep 时均实测有差异、均已重跑（2026-09-21 @ 91b3b5c，面域锚行见 progress.md「K3 launch 面就绪」段）：

- **server 面**：K1 hotfix 动了 front_app.py → 已重跑：`python -m pytest -q --import-mode=importlib -m "not live" <repo>/server/tests <repo>/server/acceptance` → **75 passed / 0 failed**（2026-09-21 @ 91b3b5c）。零红。
- **firmware host 面**：K2 实现批动 firmware/ → 已重跑：WSL host runner build + run → **74 Tests / 0 Failures / 0 Ignored**，exit 0（2026-09-21 @ 91b3b5c；既有 63 齿 + K2 新 11 齿全绿）。零红。
- 本包验收红基线（acceptance-author 新冻结齿）**恒现跑**，无缓存可言。
- intake 消费：对相应面域先 `git diff --quiet 91b3b5c HEAD -- <面域>`，零差异引用 progress.md 锚行、有差异同命令重跑。

## 上一包留根

`delivery/changes/dev/260921-m3-k2-p2-week-grid-page/pod-retro.md`——intake 逐条评估（构成 blockingDecision → fail-closed 上报）。`delivery/taskpackages/current/taskpackage.json` 指针已刷新指向本包（launch-prep 侧办结）。

## 固件构建闸

固件严格模式默认开：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。

## 命令行（两形态并列）

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）

## 交接注记（K2 closeout / K1 热修随包交接项）

- **M3K1-A5 + M3K2-A7 两个 live 半 structural deferred 均归本包设备轮（M3K3-A3）**：真机清单含 K2 挂账三项（版式 1bpp 目测 / 翻页手感 / 今天列观感）+ 格内 >4 门裁剪显示 + 契约示例「明早」渲染标签显示形态 + ※/★ 真字形观感（host 子集走程序化兜底，真字形归设备轮目击）。
- **K2 挂账一处代码遗留**：板级零点跨天重读 RTC 未落 K2 包（重启恢复正确）——本包「K3 前修注释或落重读线」的落点在本包边界内，intake 时评估归属。
- **薄意图展开接线是本包核心**（M3K3-A2）：K1 热修轮实测「课表页网格空」为预期——意图信号 → front 展开 → view=schedule 下发的链路归本包实现。
- **prompt v6 部署 ECS**（live 轮前置）：走 apply-xiaozhi-overlay.sh + kidpal-ecs-gateway skill 纪律；生产面变更，部署窗口需 human 确认。
- **K1 热修轮两独立缺口未修**（非本包义务，设备轮若复现如实记录）：① 语音轮结束 ~36s WS 被对端关闭、设备静默掉线（疑 NAT/keepalive 面）；② 服务端 OTA 响应缺 server_time 下发（设备时间错误根因）。
- dispatch-stamp 恒传 `--task-package-id 260921-m3-k3-voice-schedule-entry-live`；mutation-check 回填前先读 checker 的 id 提取器/匹配语义（ARQ-097 口径，坑会变形）。
