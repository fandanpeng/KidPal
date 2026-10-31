# docs-delta — 261031-openclaw-cron-trigger-gating

> 本件 = integrator 制品面（delivery/taskpackages/** 授权内）。spec 面（`docs/requirements.md` / `docs/architecture.md`）integrator 无写权（ARQ-260929-docs-sync-spec-handoff）；本包判定 = **deploy 面文档同批更新、spec/契约面零变更**。理由与逐面判定见下。

## 1. docs-sync 状态：deploy 面文档同批已更新（docsSyncRequired=true，docsOwner=integrator）

`docRefs[]` 五件（plan.md + 两份 SKILL + DEPLOY-RUNBOOK + openclaw-skills README）在盘且承载本包 CRG-A1..A6 锚；docs-sync 闸绿（2026-10-31 本树 `node .claude/scripts/docs-sync-check.mjs` → `ok:true`，changeId 解析来源 `delivery/taskpackages/current/taskpackage.json`）。

## 2. 本批改了哪些 deploy 面文档（同批原子，非事后补）

- **`server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`**
  - 新增 **§8.3d「给两条 agent cron 加 trigger（变更驱动静默门）」**：背景（2026-10-31 实测 20.75M token/天）、两件套表（两个判定器 + 两个薄壳 + apply 工具）、所有权表（语义产物 / ack 件 / 告警件 / trigger.state 单写者与读方）、铺放 + 加 trigger 六步（含 `--dry-run` 与 `status` 核验）、L1–L7 观测口径、**一步回滚** `cron_triggers.py clear` 与等价手工 `openclaw cron edit <name> --clear-trigger`。
  - **§8.3b / §8.3c** 两处注册段各加一行指针：注册后按 §8.3d 加 trigger（不加则保持每 15 分钟无条件跑，功能正确但空转烧 token）。
  - **§8.6 回滚**：置顶加「先撤 trigger」（一步回到无条件 15m，payload / 频率零改动）+ 等价手工命令；补 `openclaw cron remove --name kidpal-schedule-health` 一条（此前遗漏）。
  - **§9 升级清单**：新增 trigger 核对段——升级后复查两条带 trigger 的 agent cron 仍含 `trigger.script`、`once` 非 true、`cron.triggers.enabled` 非 false、内联文本与盘上一致；升级若迁移 trigger 结构先 `clear` 止血。
- **`server/deploy/openclaw-skills/README.md`**：两条目（kidpal-reminder-semantic / kidpal-schedule-health）各加注记，指向 RUNBOOK §8.3d 的 trigger 变更驱动 / 判定层，声明「内容未变零 token」。
- **`server/deploy/manifest.yaml`**：新增「cron trigger 判定器 + 薄壳」段注释 + 5 条 entries（两判定器 + 两薄壳 + cron_triggers.py，均 `owner: deploy.sh, verify: md5, restart: none`）；`cron_jobs` 段注释补 trigger 核验说明；两条 job（kidpal-reminder-semantic / kidpal-schedule-health）各加 `expect_trigger_file`。
- **`server/deploy/openclaw-skills/workspace-main/kidpal-reminder-semantic/SKILL.md`**：新增「本回合由 cron trigger 变更驱动唤起（261031 起）」段——触发原因是提示非事实、semantic.json 新鲜度合同（四输入 + 60s 竞态余量 + 按全量重写）、无课表恒 fire:false。
- **`server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md`**：新增「trigger 判定层 vs skill 行为权威（261031 起）」段——trigger = 是否要跑的判定层，skill = 四分支行为与 ack 单写者权威，`state.lastFire` 仅反刷屏保险丝。

> 说明：上述 deploy 面文档全部落在本包 `changeManifest` 内（`server/deploy/**`），属实现面随批文档，非 spec 面追记。

## 3. 明确未改的 spec / 契约面

- **契约面零触碰**：`docs/contracts/**` 无 diff。本包只改调度成本模型（两个 cron 是否起 agent 回合），不改变任何对外数据形状（笔顺 JSON / display 统一响应块 / 课表 MD 结构 / shim API 均不变）；`docs/contracts/interface-contract.md`（contractRefs）只作引用锚，未消费改动。
- **需求面零变更**：`docs/requirements.md` 无 diff——本包不改变产品语义（三档时刻、提醒内容、家长告知行为均不变）。
- **架构面零变更**：`docs/architecture.md` 无 diff——相关锚 = §5.2 场景⑤ 与 §5.4 alerts 交接目录（sourceRefs），实现落在既有 OpenClaw 运行时部署面，不引入新架构决策、不触发 ADR（`adrRequired` 未置，taskpackage 无该字段）。
- **账本面**：本包来源 = human-ad-hoc（origin 锚在 taskpackage），未挂 debt 条目，无 consumed-by 追记位。旁证：父提交 `662cff08` 登记的两条 tech debt（docs/debt/tech/**）属上一批簿记，不在本包 changeManifest、非本包改动。

## 4. 零触碰面

- `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`（§3 理由）；
- `firmware/**`（本包零 firmware diff，change-manifest 手工核对 firmware 面 0 件）；
- `.claude/**`（无治理面变更）。

## 5. 留痕锚

- 范围圈定清单：`delivery/taskpackages/261031-openclaw-cron-trigger-gating/test-scope-final.txt`（final 验收命令与逐字末行、change-manifest 核对结论均记于该件；review-packet 记该路径一行）。
