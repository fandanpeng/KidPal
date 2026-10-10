# Launch Card — 261010-prompt-v9-schedule-anti-narration

跑 261010-prompt-v9-schedule-anti-narration（独立变更，无 workstream，mergeOrder 45；② 单包 Lite：prompt v9 课表回合补对称反叙述纪律）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-10 · 261010 真机 bug 批 launch prep」段带 SHA 行——server 面（139 passed / 0 failed / 4 deselected / 9 subtests @2d94c36）+ firmware host 面（147 Tests / 0 Failures @2d94c36，命令见锚行）。intake 差异判停：以该段锚行 SHA 为基准对各自面域跑 `git diff --quiet <anchor> HEAD -- <面域>`，零差异即引用锚行不重跑，有差异重跑（命令同锚行）。本包只触 server/deploy prompt 族 + 验收 + arch 指针 docs-sync，firmware 面恒零差异。
- **上一包留根**：`delivery/changes/dev/261001-openclaw-model-name-overlay-drift/pod-retro.md`（intake 逐条评估；构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **本包是仓内 prompt 版本化 bump**：新建 `openclaw-system-prompt.v9.md`（v8 全量 + 课表行补对称反叙述纪律），历史 v1..v8 **零回改**（PN-A3 v8 sha256 零回改齿钉死）；槽位纪律 ARQ-260908——已核 server/deploy 无 .v9 在场。
  ② **四面接线同步**（版本化铁律：渲染面 == 声明面 == 被测面）：CARRIER 现值 v8→v9 + 演进史 append；`provider.config.yaml` system_prompt 三键；`apply-xiaozhi-overlay.sh` PROMPT_SRC/PROMPT_DST；`test_m3k3_voice_schedule_entry` 版本无关三面守卫随接线自然转绿（零改动消费）。
  ③ **docs-sync 双面**：arch §5.5.5（:297）/§5.1（:196）现状指针刷 v9，**顺带收敛既有漂移**（arch 现写 v7、实际接线 v8——v8 批漏刷，漂移事实记入 docs-delta）；`server/deploy/README.md` prompt 指针节同核。
  ④ **部署 = v9 + overlay 重跑上 ECS**（261010 决策 21 键同款先例：`ecs_put.py` exec_command+stdin + MSYS_NO_PATHCONV=1，**上传前核远端 == 基线**——ECS 侧脚本副本曾滞后仓内〔261001 旧值地雷〕，勿盲信远端已是最新）。部署后真机问「明天有什么课」做 PN-A5 live 观测（human 在场）。
  ⑤ **已知地雷**：本包验收全为文件内容锚、无 DB/事件断言——supervisor 机械闸判 all-mock 触发 riskTier 重算 high → 需 human 放行（先例在案，plan 前置申报）。
  ⑥ **禁止顺手改契约/需求**：`docs/contracts/**`、`docs/requirements.md` 零改（architect review 已核零命中；arch 指针刷新 = 唯一 docs-sync 面）。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
