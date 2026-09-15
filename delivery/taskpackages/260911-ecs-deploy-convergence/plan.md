---
changeId: 260911-ecs-deploy-convergence
rigor: full
riskTier: high
sourceType: drift-report
gateProfile: [live-deploy, evidence-recovery]
reviewed-by: architect (triage 2026-09-11, 来源锚: 包 9b review-packet F3 + PROGRESS-SUMMARY-260910 §四C/§七 #1/#2/#13)
date: 2026-09-11
---

# 批次① ECS 部署收敛包（#1 漂移修复 + #2 提醒 cron §8 部署 + live 五齿 L1-L5 + #13 证据回收）

> 分流裁决：② 单包 Full。理由见 frontmatter + 裁决记录。改自有实现面（云端 shim 部署制品）
> 行为 → 过门；单一 ECS 环境、单一部署窗口（human 已批部署窗口语义，pod-events 04174089）、
> 一轮可收 → 不拆包；触 live 生产配置 + 常驻 scheduler（cron 驱动设备推送）+ 证据回收需
> claim verification + 回滚必要 → riskTier=high → Full。
> 逃生门：执行中任何 delta 的合理化需要改 docs/contracts/** 或 docs/spec.md → 停，
> 转 /arch-change + architect-reviewer 四分类，本包不批事实源变更。

## 阶段 0 · 只读勘察（不改任何东西）

- S0.1 **live 快照**：ssh 只读备份 ECS live `.config.yaml` → 带时间戳文件（ECS 本地 +
  `changes/dev/260911-ecs-deploy-convergence/` 各一份）；逐字记录 selected_module 与
  provider 块（pod-event 留痕，格式对齐 053ae432 体检）。
- S0.2 **渲染 diff 枚举全部漂移**：`XIAOZHI_ROOT=<临时目录> bash server/deploy/apply-xiaozhi-overlay.sh`
  渲染到临时目录 → 与 live 备份逐行 diff → 落盘 delta 清单。预期 ≥4 处：ASR（60 行）、
  Intent（62 行）、base_url 缺 /v1（67 行）、TTS type=edge-tts（73 行）；以 diff 结果为准，
  不以本清单为准。
- S0.3 **per-delta 人工定谳**：每个 delta 分类「已裁定终态」vs「来历不明临时态」——
  已裁定：ASR=Qwen3ASRFlash（human 2026-09-06 终选，spec §A1 双候选之一）、Intent=nointent
  （spec §A3 相容）、base_url+/v1 与 TTS type=edge（provider.config.yaml 已记 live 教训）；
  来历不明 → 逐条问 human，不自行固化。**出现需改 spec/契约才能合理化的 delta → 触发逃生门。**

## 阶段 1 · 仓库修复（builder）

- S1.1 `server/deploy/apply-xiaozhi-overlay.sh`：按 S0.2/S0.3 结论对齐全部 delta
  （ASR 块改 Qwen3ASRFlash 声明——schema 以 live 备份为权威源；Intent→nointent；
  base_url 补 /v1；TTS type→edge）；同步更新文件头注释与凭据 env 读取
  （阿里云 appkey/token 若不再用则移除，换 Qwen3 所需 env——以 live 实际凭据来源为准）。
- S1.2 `server/deploy/provider.config.yaml`：providers.asr 声明对齐（provider 值改
  qwen3/dashscope 方向，保 GWA-A4 齿命中 aliyun|dashscope|qwen3 任一）；记决策指针
  （human 2026-09-06 ASR 终选）；`forbid_max_tokens` 与 18789 loopback 约束注释原样保留。
- S1.3 顺检 `server/deploy/README.md` 是否仍述旧 provider 值，是则同步。
- S1.4 **验证（verifier）**：聚焦跑 server 验收套（test_gateway_directionB 离线齿 +
  CHR-E3 + P3-A4 + test_story_wiring）——预期全绿、无齿绊倒；跑 `node -e` 或 shellcheck
  级脚本语法自检。

## 阶段 2 · live 部署（human 在场的部署窗口）

- S2.1 复跑 S0.2 渲染 diff（修复后）→ 与 live 备份 diff = **零意外 delta**（prompt v4
  切换是预期 delta）。不为零 → 停、回阶段 1。
- S2.2 正式 apply（secrets env 就位）→ 核验：引擎容器重载后 live config 四 provider 全对、
  `prompt:` 键指向 v4 内容（验收所测 = 生产所用）。
- S2.3 live 冒烟：一轮完整语音端到端（设备单击说话或既有 smoke 探针形态）——语音链路完好。
- S2.4 提醒 cron §8（DEPLOY-RUNBOOK §8.2-8.4）：确认脚本/配置已随 S2.2 铺到位 → 设
  `KIDPAL_ALERTS_DIR` 等目录 → `openclaw cron add --name kidpal-reminder-plan --every 15m …`
  → `openclaw cron list` 新增一条且 `state.lastRunStatus=ok` → 核 `kidpal-front.service`
  env 含 KIDPAL_ALERTS_DIR/KIDPAL_SERVER_DIR + poller 日志有 alert_poll 心跳。

## 阶段 3 · 证据回收与交班

- S3.1 **live 五齿 L1-L5**（device-acceptance-checklist 表 C）：L1 cron list 摘录 / L2
  pending.json 形状对照契约 §三 / L3 L4 设备推送+渲染回执（human 持板；等真实档位或在
  schedule.md 插入近期真实条目制造档位）/ L5 语义产物；证据落
  `changes/dev/260829-schedule-reminder/`；证据内不得出现 Invalid id / Unknown message
  type / 推送失败 / 排程失败 字样（probe ≠ exercise）。
- S3.2 **WSA-A4**：拷贝 `changes/dev/260829-hardware-validation/flash.log`（188 处真实
  烧录标记）→ `server/acceptance/_artifacts/device-ws-switch/flash.log`（gitignore 本地件，
  落在跑 host 门的 worktree；可用 WSA_ARTIFACT_DIR env）。
- S3.3 **WSA-A6**：当日真只读探针（连 :8080 期望 refused/timeout + 读安全组规则）→ 写
  `server/acceptance/_artifacts/device-ws-switch/ecs-teardown-evidence.md`，如实标注
  「260903 停服的回收性核验 + 当日探针逐字输出」。**8080 实际仍开 → 停、升级 human。**
- S3.4 host runner 复跑：预期 WSA-A4/A6 转绿；P2-A1 first-match 歧义红**不在本包范围**
  （归批次③渲染批），不喂哨兵让它消失。

## 验收 / exitCriteria

1. 修复后 overlay 渲染 diff vs live 备份 = 零意外 delta；S1.4 聚焦套全绿。
2. live：一轮完整语音轮 0 断链 + prompt v4 在场 + cron 在册且 lastRunStatus=ok。
3. L1-L5 证据齐且含真实标记；WSA-A4/A6 host 齿转绿（P2-A1 留红如案登记）。
4. 回滚就绪：live `.config.yaml` 时间戳备份可还原 + `openclaw cron remove --name
   kidpal-reminder-plan`（§8.6）演练路径写进包证据。
5. 冻结面（docs/contracts/**、*.acceptance.*、managed_components/**）git 零触碰。

## human 触点

- 触点①：确认/改判本裁决（②单包 Full / riskTier=high）——已确认（2026-09-11，②单包 Full）。
- 部署窗口 + L3/L4 设备在场：human 持板。
- S0.3 来历不明 delta：逐条问。
