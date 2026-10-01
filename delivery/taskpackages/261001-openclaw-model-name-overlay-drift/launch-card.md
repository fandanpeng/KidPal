# Launch Card — 261001-openclaw-model-name-overlay-drift

跑 261001-openclaw-model-name-overlay-drift（独立变更，无 workstream，mergeOrder 44；② 单包 Lite：引擎 overlay model_name 冲刷的仓内回流修复）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-01 · B1/C launch prep」段带 SHA 行——server 面（127 passed / 0 failed / 4 deselected @60744cb）+ firmware host 面（147 Tests / 0 Failures @60744cb，命令见锚行）。intake 差异判停：以该段锚行 SHA 为基准对各自面域跑 `git diff --quiet <anchor> HEAD -- <面域>`，零差异即引用锚行不重跑，有差异重跑（命令同锚行）。本包只触 server/deploy 模板族 + 验收 + ADR，firmware 面恒零差异。
- **上一包留根**：`delivery/changes/dev/261001-m4-k3-record-session-live-repair/pod-retro.md`（intake 逐条评估；构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **本包是仓内回流**，不是修 live：ECS 活配置已于 261001 热修（`.config.yaml` → `openclaw/kidpal` + `docker restart`，探针 200，备份 `.config.yaml.bak-260930-modelname-clobber`）；本包修的是仓内模板/验收，防下次重部署再冲。
  ② **改四处默认值 + 重钉一处冻结验收**：`apply-xiaozhi-overlay.sh:72`（渲染面，live 真源）/ `provider.config.yaml:62`（声明面）/ `provider.py:32`（**无 live 消费方**，仅声明一致）/ `perf-k1/probe_batch.py:42`；冻结齿 `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py:916/:938` 期望值重钉，**并订正 `:917/:939` 的失准文案**（写「契约 §四钉 openclaw/default」，但契约对 `model_name` 零命中——architect-reviewer 已复核）。
  ③ **ADR append-only**：`docs/ADR.md` 新增 **ADR-007** amend ADR-006 的「model_name 零改 / 模型选择对引擎完全不可见」支撑句（双 agent 下对引擎可见）；ADR-006 核心决策（降 thinking 档、模型 primary 不变）不受影响。**不得改 ADR-006 正文**。
  ④ **禁止顺手改契约/spec**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零改动（architect 快审已复核零命中）。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
