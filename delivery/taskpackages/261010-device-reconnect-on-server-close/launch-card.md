# Launch Card — 261010-device-reconnect-on-server-close

跑 261010-device-reconnect-on-server-close（独立变更，无 workstream，mergeOrder 46；② 单包 Full〔riskTier high〕：固件断线自动重连 + 永不休眠常驻在线）。**串行接力：等 261010-prompt-v9-schedule-anti-narration 合并后再起本包**（同常驻树一树一会话）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-10 · 261010 真机 bug 批 launch prep」段带 SHA 行——firmware host 面（147 Tests / 0 Failures @2d94c36，命令见锚行）为主锚；server 面（139 passed / 4 deselected / 9 subtests @2d94c36）恒零差异（本包 server 面零改动）。intake 差异判停：`git diff --quiet <anchor> HEAD -- firmware` 非空则重跑（前包 prompt-v9 只触 server 面，firmware 面预期零差异；若前包真机轮侧写了固件面则必须重锚）。
- **上一包留根**：`delivery/changes/dev/261010-prompt-v9-schedule-anti-narration/pod-retro.md`（intake 逐条评估；构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **human 裁定在案（plan「human 裁定记录」节，原话「永远不休眠」）**：子项 A = 服务端断自动重连；子项 B = 会话收尾本地关后短延迟常驻重建（不动「会话结束关通道」既有拓扑）。裁定落档 = arch 四投影点 docs-sync（§10 新增**决策 29**——编号已核，27/28 已占用；plan 原稿笔误已由建包方修正留痕）。电池代价（Wi-Fi 常供电、续航转随用随充）已随裁定接受，**不重开讨论**；留 modem 档位调优自由度（RC-B6 观测项）。
  ② **本地关 8 调用点逐点归类留痕 = 验收前提**（plan 归类表：:805/:849/:1189 B 重建；:422/:1081/:1100/:1254/:1267 零重建；builder 复核 :1189/:1254 语义后归类留痕）。误重连会打架会话收尾与休眠门——分类互斥齿（RC-A2）承重。
  ③ **复用基建纪律**：重连/重建恒走 TryStartAutoConnect（application.cc:477-517）守卫与退避——零第二条 task、零新退避表（RC-A3 钉死）；3 次穷尽静默停不自旋；重连区零 SetDeviceState（屏显不受影响，requirements.md:67）。
  ④ **boot_auto_connect 冻结件**：新触发点如撞「触发点 = 2」类结构 needle → 正常改验收通道 + human 方向锚留痕（260928 FIX-A2 先例形态）；既有 27 齿零破坏红线。
  ⑤ **真机轮 human 在场持板为硬前置**：四场景（服务端断恢复 / 常驻重建跨夜提醒可达——front/引擎日志无 alert_skip no_online_device / 功能性关零重建 / 复用通道会话回归）；烧录 = 仅 app 分区（无资产变更，不触双分区烧录规则）。本包 log 侧复刻 10-10 三日志源对账法（[[kidpal-live-smoke-debug]] / [[kidpal-serial-monitor]]）。
  ⑥ **riskTier high**：supervisor 必升 human，合并须人批；verifier 首轮见红先对基线锚（新增红才是本包责任）。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
