# Launch Card — 261001-m4-k3-record-session-live-repair

跑 261001-m4-k3-record-session-live-repair（m4-star-notebook，mergeOrder 42；Package A：纯固件修复）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-01 · K3-repair launch prep」段带 SHA 行——server 面（127 passed / 0 failed / 4 deselected @ 72ce58f，命令见锚行）+ firmware host 面（136 Tests / 0 Failures / 0 Ignored @ 72ce58f，命令见锚行）。intake 差异判停：以该段锚行 SHA 为基准对各自面域跑 `git diff --quiet <anchor> HEAD -- <面域>`，零差异即引用锚行不重跑，有差异重跑（命令同锚行）。
- **上一包留根**：`delivery/changes/dev/260924-m4-k3-record-session-live/pod-retro.md`（本轮四缺陷的落地包，intake 逐条评估；构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **本包只含纯固件面四缺陷**（D1 列表无消费方 / D2 层栈让位 / D4a 键路由毁稿 / D4b 悬垂 panic）；D3 语音收尾「说完了」与逐句回显内容触需求/架构/契约三面，属 Package B，**明确不在本包验收面**。
  ② **D2 层栈让位形态须与 D4a 判据闭集同一次设计决策内一致选定**（spine openQ q4：弹 VOICE 层 → D4a 判据天然成立；或录音页提到 VOICE 之上 → 须显式补 keya_story_page_wants 的 RECORDING 分支）。plan.md §3 须在包内钉死 + 补 host 可机判齿。
  ③ **D4b 三修并行、缺一不可**（会话收尾复位录音页态并停定时器 / 录音演出渲染走渲染节点认可路径〔夹 begin/end 或经 rn_render_*〕/ `host_infra_render_p1_frame` 加 `lv_obj_is_valid` 守卫）。
  ④ **契约面零改**（本包纯固件实现面）；Package C（引擎 overlay `model_name` 冲刷）为独立小包（`261001-openclaw-model-name-overlay-drift`），不在本包。
  ⑤ **live 真机复验矩阵四窗须 human 在场持板**（单真机不撞车；串口 + 引擎日志双源对齐，取证纪律见 kidpal-live-smoke-debug / kidpal-serial-monitor skill）。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
