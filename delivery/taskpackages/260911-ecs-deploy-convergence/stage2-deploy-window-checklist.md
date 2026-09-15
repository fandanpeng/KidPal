# 阶段 2 部署窗口执行单 · 260911-ecs-deploy-convergence

> 前置条件：verifier scoped 轮绿 + 阶段 1 修复已 commit。窗口内 human 在场（S2.2 apply / S2.3 冒烟 / S2.4 cron）。
> 回滚锚：ECS 侧 `.config.yaml.bak-260911-planner-s01`（5150 字节，260907 态）；本地脱敏快照见 `s0.1-s0.2-live-snapshot-and-delta-ledger.md`。
> ECS 访问：`python /c/tmp/ssh_helper.py "<cmd>"`。凭据经 `/opt/kidpal/xiaozhi-esp32-server/deploy/secrets.env`（含 DASHSCOPE_API_KEY / XIAOZHI_DEVICE_TOKEN / OPENCLAW_TOKEN），任何输出先脱敏再落盘。

## S2.1 复跑渲染 diff（apply 前最后确认）
1. 仓库最新代码上重渲染：`XIAOZHI_ROOT=/c/tmp/ecs-conv-render-s21` + stub token + `XIAOZHI_SECRETS=/c/tmp/nonexistent-secrets.env`（本地渲染，产物为声明形状、凭据占位）。
2. 与 live 快照（`c:\tmp\kidpal-live-config-260911.yaml`）diff：**唯一允许 delta = ①凭据占位行 ②server.auth.enabled false→true（D5 human 裁决）③prompt v1→v4（D7 预期）④注释/生成头形式差异**。任何其它结构 delta → 停、回阶段 1。
3. diff 证据（脱敏）落 `changes/dev/260911-ecs-deploy-convergence/s2.1-render-delta.txt`。

## S2.2 正式 apply（secrets env 就位）
1. 同步仓库 `server/deploy/**`（apply-xiaozhi-overlay.sh / provider.config.yaml / openclaw-system-prompt.v4.md / openclaw-patch/reminder_schedule_lib.py / openclaw-reminder.config.yaml）到 ECS 对应位置（rsync/scp，先 dry-run 列清单）。
2. `source deploy/secrets.env`（或 systemd 同源 env）后执行 `bash server/deploy/apply-xiaozhi-overlay.sh`（XIAOZHI_ROOT 默认即 live data 目录）。
3. 核验：`.config.yaml` 四 provider 全对（Qwen3ASRFlash / OpenClawOpenAI / EdgeTTS / nointent / nomem）+ `auth.enabled: true` + `server.ip/port 127.0.0.1:8010` + `prompt:` 块头 v4。
4. 引擎重载：`docker restart xiaozhi-esp32-server`（或 compose 等价）→ 等 healthy/日志就绪。
5. **注意**：auth enabled 变 true 后设备须带 token 连接（固件已实现 决策22 token）——S2.3 冒烟即时验证；若设备连不上，第一怀疑 auth，勿回滚前先查 front 日志。

## S2.3 live 冒烟（一轮完整语音端到端，probe≠exercise）
1. 设备（human 持板）单击说话 → 语音回答完整、display 块上屏 → 0 断链。
2. 顺带问一句课表/提醒类触发 display view，确认 provider 链（ASR qwen3 / LLM openclaw / TTS edge）各环节无报错。
3. front 日志抽查：`journalctl -u kidpal-front.service --since "-10 min"` 无 Invalid id / Unknown message type。
4. live 齿（显式 RUN_LIVE=1，部署窗口内跑）：`cd server && RUN_LIVE=1 python -m pytest -q --import-mode=importlib acceptance/test_gateway_directionB.acceptance.py -k live`。

## S2.4 提醒 cron §8（DEPLOY-RUNBOOK §8.2-8.4）
1. 确认 reminder_schedule_lib.py 已铺到 OpenClaw 可读目录（S2.2 步骤 1 同步）。
2. `KIDPAL_ALERTS_DIR` 等目录就位（RUNBOOK §8.2 路径）。
3. `openclaw cron add --name kidpal-reminder-plan --every 15m --command <载荷>`（§8.3 逐字）。
4. 核 `openclaw cron list` 新增条目且 `state.lastRunStatus=ok`（等首跑或手动触发一次）。
5. 核 `systemctl show -p Environment kidpal-front.service` 含 KIDPAL_ALERTS_DIR/KIDPAL_SERVER_DIR；`journalctl -u kidpal-front.service | grep alert_poll` 心跳在场。
6. 回滚演练路径记录：`openclaw cron remove --name kidpal-reminder-plan`（§8.6，只记录命令形态不执行）。

## 证据落点
- S2.1 diff、S2.2 apply 输出/核验、S2.3 冒烟记录、S2.4 cron 证据 → `changes/dev/260911-ecs-deploy-convergence/`（integrator 收口阶段归档；证据件一律脱敏）。
- L1-L5 五齿证据 → `changes/dev/260829-schedule-reminder/`（ECS-A5 口径）。
