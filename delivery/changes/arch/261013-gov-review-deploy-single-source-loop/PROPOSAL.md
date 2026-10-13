# PROPOSAL · 261013-gov-review-deploy-single-source-loop

> **状态：FROZEN v1.2 · 2026-10-13 · 冻结凭据**：human 立项 ack + OQ 终裁（2026-10-13 会话，原话「ack，oq同意你的推荐」——OQ-1/2/3/4 全取推荐位 (a)）+ architect r3 GREEN（评审记录同目录 `architect-review.md` r1–r3 段，2026-10-13；OQ-1 取 (a)，不触发 r4 回炉条款）。沿革：v1.0 起草 → r1 RED（F1 blocker 等 7 findings）→ v1.1 → r2 RED 轻量（F9–F11 声明面收尾）→ v1.2 → r3 GREEN。
> **承载 ARQ**：无（本批治理变更自承载）。联动 tech 域条目 `TECH-261012-qq-schedule-write-format-gap`（deferred）：本批修其证据 3 揭示的部署一致性根因并 append 补记该条目（补记同时钉「修复项 ③ 已由本批 T1 承接、dev 批只对账 ①④」防双做）；修复项 ①（kidpal-schedule-write skill）④（front surfaced 告警）留根归 dev 批。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium——含 ECS live 部署路径切换与 live agent 部署副本替换，需真机复证；不触 `docs/contracts/**`、不触固件面、不触 `.claude` 治理面（G1/契约面为零）。
> **review profile**：G2（治理/部署流程变更，触面 = `server/deploy/**` + `docs/crq/tech/` 补记一行）。

```landing-manifest
server/deploy/manifest.yaml
server/deploy/deploy.sh
server/deploy/verify-deploy.sh
server/deploy/kidpal-deploy-drift.service
server/deploy/kidpal-deploy-drift.timer
server/deploy/ecs-bootstrap.md
server/deploy/openclaw-skills/workspace-main/AGENTS.md
server/deploy/openclaw-skills/workspace-main/kidpal-board-context/SKILL.md
server/deploy/openclaw-skills/workspace-main/kidpal-char-strokes/SKILL.md
server/deploy/openclaw-skills/workspace-main/kidpal-schedule-answer/SKILL.md
server/deploy/openclaw-skills/workspace-main/qqbot-reply-diagnosis/SKILL.md
server/deploy/apply-xiaozhi-overlay.sh
server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md
server/deploy/README.md
server/deploy/openclaw-skills/README.md
server/deploy/openclaw-agents/README.md
server/deploy/openclaw-reminder.config.yaml
server/deploy/openclaw-schedule.config.yaml
server/deploy/provider.config.yaml
docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md
```

---

## 0. 想实现的效果与差距

目标效果一句话：ECS 三件套（kidpal-front、xiaozhi 引擎 overlay、OpenClaw workspace 创作面）的部署从手工匠艺改为「GitHub 单一源 → ECS 拉取 → manifest 铺放 → 机械核验」闭环，live↔仓内漂移在部署时 fail-loud、在日常由每日 timer 报警落 alerts。

现状差距（2026-10-13 主线程 ECS 实测，命令均经 `python c:/tmp/ssh_helper.py "<remote cmd>"` 执行）：三件套三种部署法且都无核验——front 是手工拷贝（`/opt/kidpal/front` 非 git 仓，靠 14 个 `.bak-<日期>-<原因>` 留痕）；引擎 overlay 经 `apply-xiaozhi-overlay.sh` 渲染但铺放目标与 cron 消费路径分裂；OpenClaw workspace 文件直接手写 live、仓内镜像只覆盖 6 个 skill 中的 2 个。本次实测共发现 6 处漂移（见 §2 各 T 证据），其中 reminder lib 路径分裂直接导致 TECH-261012 记录的「触发③从未 live」；architect r1 评审另复证出**两处配置消费位缺文件**（与 lib 路径分裂同型的第二处分裂，见 T1/T4）——`openclaw-reminder.config.yaml` 的真实消费位 `/opt/kidpal/openclaw-reminder.config.yaml` 与 `openclaw-schedule.config.yaml` 的真实消费位 `/opt/kidpal/front/deploy/` 均无文件，当前分别被 `DEFAULT_LADDER_CONFIG` 同值回退与 `school_start=None` 静默容错掩盖。

非目标（本批不做什么）：不做 push 触发的自动部署（部署动作仍由 human 在 ECS 手动执行 `deploy.sh`，remote push 仍归 human——OQ-3 已裁的 push 铁律不变）；不做 front 容器化；不实施 TECH-261012 修复项 ①（main workspace 补 `kidpal-schedule-write` skill）与 ④（front 对 `schedule_md_err` 的 surfaced 告警）——两项归 dev 批课表录入链路收口时对账处理；不碰 OpenClaw 运行态文件（`DREAMS.md`、`memory/`、`sessions/`、agent sqlite——manifest 只管创作面文件）。

## 2. 问题、根因与改法（T1–T6）

### T1 · 部署无清单 + 仓内镜像缺口
**问题**：live 部署面没有单一事实源清单，「什么该在 live 上」散落在 README 散文与各人记忆里；仓内 OpenClaw 镜像只覆盖 live main workspace 5 个 skill 中的 1 个（`kidpal-reminder-semantic`）+ kidpal 侧 1 个，main workspace 的 AGENTS.md 完全无镜像。缺口导致漂移无 diff 基准，只能事后人工 md5 对账（本次即如此）。
**证据**（2026-10-13 ECS 实测）：`ls /root/.openclaw/workspace/skills/` 列出 5 目录：`kidpal-board-context`、`kidpal-char-strokes`、`kidpal-reminder-semantic`、`kidpal-schedule-answer`、`qqbot-reply-diagnosis`；仓内 `server/deploy/openclaw-skills/` 仅有 `workspace-main/kidpal-reminder-semantic/` 与 `workspace-kidpal/kidpal-schedule-answer/`。行尾归一 md5 实测：main 侧 `kidpal-schedule-answer/SKILL.md` = `972967357a33fa450a1c0a4fb5df0173` ≠ kidpal 侧同名词 = `73341e2dd793f21060164c4760866550`（同名不同文件，须各占一镜像位）；main workspace `AGENTS.md` = `a9449870ea8998b4fe1a7e9f6aecf72d`（无仓内对应）。已镜像 2 件与 live 一致（`7de61925…` / `73341e2d…`），workspace-kidpal 的 `AGENTS.md` 亦一致（`12cda0c8…`）。另两处配置消费位缺文件（r1 F1，`ls /opt/kidpal/` 实测无 `openclaw-reminder.config.yaml`、`ls /opt/kidpal/front/` 实测无 `deploy/` 目录）：lib 侧 [reminder_schedule_lib.py:472-474](../../../server/deploy/openclaw-patch/reminder_schedule_lib.py) 的 `KIDPAL_REMINDER_CONFIG` 默认 = `dirname/../openclaw-reminder.config.yaml` → cron 实际消费位 `/opt/kidpal/openclaw-reminder.config.yaml`（不存在；cron 作业诊断行 `night_at=20:30 morning_at=07:45 before_min=15` 恰 = [reminder_schedule_lib.py:46](../../../server/deploy/openclaw-patch/reminder_schedule_lib.py) `DEFAULT_LADDER_CONFIG` 逐值——配置从未到达消费者的直接实证）；front 侧 [front_app.py:672-674](../../../server/gateway/front/front_app.py) 的 `SCHEDULE_CFG` = `parents[2]/deploy/openclaw-schedule.config.yaml` → `/opt/kidpal/front/deploy/openclaw-schedule.config.yaml`（不存在，`_schedule_school_start` 静默回退 None）。
**根因**：`server/deploy/openclaw-skills/README.md:4` 自记「live 部署面目前是手动的」；镜像按需增补、无清单义务约束，新 skill 落 live 时无机制提醒回仓。配置缺位的同型根因：部署面按「渲染到固定目录」思维铺放，从不按「消费者的路径解析规则」对账（详见 §3 census 派生方法）。
**改法**：新增 `server/deploy/manifest.yaml`——逐条登记「仓内源路径 → live 绝对路径 → 核验方式（md5/标记行）」，首批覆盖三件套全部创作面文件（front 源 12 件、overlay 渲染目标、engine-patch 2 件、两 workspace 的 AGENTS.md 与 6 个 skill、reminder lib 的 cron 消费路径、**两个配置文件消费位直管**：`server/deploy/openclaw-reminder.config.yaml` → `/opt/kidpal/openclaw-reminder.config.yaml` 与 `server/deploy/openclaw-schedule.config.yaml` → `/opt/kidpal/front/deploy/openclaw-schedule.config.yaml`、2 个 openclaw cron 作业定义的命令行字符串）；补齐 5 件镜像缺口（main AGENTS.md + main 侧 4 个未镜像 skill）。manifest 是唯一「什么该在 live」权威，后续加部署面 = 改 manifest 进 review。**声明面收尾（r2 F9）**：两份配置文件头的部署声明同步改写——`openclaw-reminder.config.yaml:3-4`「随 server/deploy overlay 上 ECS」与 `openclaw-schedule.config.yaml:3-4` 同句，均改指「由 `server/deploy/manifest.yaml` 登记、`deploy.sh` 直管消费位」（后者本就失实：overlay 从未渲染过 schedule config，正是其消费位缺位的根因之一）；**只改部署声明句、不动三档值与 `school_start`**（`server/tests/test_reminder_units.py:132` 的 `test_ladder_config_carrier_matches_spec_constants` 钉死 20:30/07:45/15，改值必红）。
目标文件：`server/deploy/manifest.yaml`（新增）· `server/deploy/openclaw-skills/workspace-main/AGENTS.md`（新增）· `server/deploy/openclaw-skills/workspace-main/kidpal-board-context/SKILL.md`（新增）· `server/deploy/openclaw-skills/workspace-main/kidpal-char-strokes/SKILL.md`（新增）· `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-answer/SKILL.md`（新增）· `server/deploy/openclaw-skills/workspace-main/qqbot-reply-diagnosis/SKILL.md`（新增）· `server/deploy/openclaw-reminder.config.yaml`（改）· `server/deploy/openclaw-schedule.config.yaml`（改）

### T2 · 部署动作无机械入口
**问题**：部署 = 人工 scp/手编 live 文件，每次即兴发挥；`.bak` 文件堆积（front 目录 14 个）正是无入口无版本化的代偿。紧急热修直接改 live 后无回流机制，形成 live→repo 漂移。
**证据**：`ls -la /opt/kidpal/front/front/` 实测 14 个 `.bak-*/.broken-*` 文件（260907 至 261010）；live `front_app.py`（md5 `f5c67e39…`）比仓内版本多 3 行 `TEMP-DIAG` 调试日志（story_push_diag / story_connect_push_diag，diff 实测），未回流。
**根因**：无「从某个 commit 一键铺放」的脚本，部署粒度是文件级手工操作。
**改法**：新增 `server/deploy/deploy.sh`，在 ECS deploy clone（T6 建）内执行：`git fetch` + checkout 指定 ref → 按 manifest 把源文件铺到各 live 路径（secrets 豁免：`secrets.env` 仅 ECS、manifest 不含）→ **按 manifest 条目声明的变更集决定重启面**——仅 front 面文件变更才 `systemctl restart kidpal-front`、仅引擎面（overlay 渲染 / 配置 / engine-patch）变更才 `docker restart xiaozhi-esp32-server`（引擎重启 = 设备全断，按需生效；r1 F6）、OpenClaw 面文件不重启仅提示 → 落 `.deployed-commit` 戳 → 末尾自动跑 T3 verify。live 紧急热修仍允许（不设技术拦截），但下次部署前 verify 会 fail-loud 逼回流。首次部署即完成漂移对账：live `front_app.py` 的 TEMP-DIAG 行按 OQ-4 裁断处置，live `provider.py` 的 stale 默认值 `openclaw/default`（diff 实测，仓内已为 `openclaw/kidpal`）随首铺消除。
目标文件：`server/deploy/deploy.sh`（新增）

### T3 · 漂移无日常机检
**问题**：live↔仓内漂移无任何机械报警，全靠故障后人肉 md5 对账（本次 TECH-261012 即如此——「触发③从未 live」静默存在数周）。
**证据**：2026-10-13 全套人工核对（行尾归一 md5）实测漂移 6 处：front_app.py、provider.py、ECS `deploy/provider.config.yaml`（声明面指 v9，仓内 v10；live `.config.yaml` prompt 已含 v10「实时事实回合」行，功能无影响）、reminder lib 双副本路径分裂（T4）、镜像缺口（T1）、runbook 裸 `python3`（T4）。另 `openclaw-schedule.config.yaml` 仓内有、消费位 `/opt/kidpal/front/deploy/` 无（`ls /opt/kidpal/front/` 实测无 `deploy/` 目录——r1 F2 修正：此前证据写「ECS `deploy/` 无」定位错位，真实缺位在 front 的 `parents[2]/deploy` 派生消费位，见 T1 证据）。
**根因**：核验只存在于部署文档的散文叮嘱（`README.md:33`「每次部署/重启后应做」），无脚本、无定时、无告警落点；census 只按目录枚举、不按消费者路径解析规则派生，env 默认派生位成为盲区（r1 F1/F2 两例同型实证）。
**改法**：新增 `server/deploy/verify-deploy.sh`——读 manifest，逐条算 live 校验和（行尾归一）与仓内源比对、cron 命令字符串与 `openclaw cron show --json` 比对，漂移则非零退出并写 `/opt/kidpal/alerts/deploy-drift.json`；新增 systemd timer `kidpal-deploy-drift`（每日一次）跑同一脚本。三个时机生效：部署后自检、每日定时、部署前前置（有未对账漂移则拒绝铺放）。**核验锚法分层（r1 F5）**：普通文件行尾归一 md5；secret 承载渲染件（`.config.yaml` 等）用标记行锚（engine-patch 标记行、prompt 版本锚「实时事实回合」等），**禁承载 secret 期望值、禁 md5**（secrets 注入差异使 md5 恒假）；`.config.yaml` 的 prompt 注入为 SKIP 幂等形态（[apply-xiaozhi-overlay.sh:118-120](../../apply-xiaozhi-overlay.sh)），verify 须兼容该形态。
目标文件：`server/deploy/verify-deploy.sh`（新增）· `server/deploy/kidpal-deploy-drift.service`（新增）· `server/deploy/kidpal-deploy-drift.timer`（新增）

### T4 · reminder lib 铺放目标与消费路径分裂（触发③未 live 的根因）
**问题**：overlay 把新版 `reminder_schedule_lib.py`（含 `drop_refresh_marker`）铺到 `xiaozhi-docker-data/openclaw-patch/`（容器 data 目录内，无人消费的孤儿位置），而 cron 实际执行 `/opt/kidpal/openclaw-patch/reminder_schedule_lib.py`（2026-09-11 旧版，无 marker 链）。TECH-261012 证据 3 记「仓内新版未同步到 ECS」表述不准——新版已在 ECS，在孤儿路径上。
**证据**（2026-10-13 实测 verbatim）：① cron 作业 `kidpal-reminder-plan` 的 payload（`openclaw cron show 862ce6ab-079d-4510-aaab-9d59df66e3e8 --json`）：`"argv": ["sh", "-lc", "KIDPAL_SERVER_DIR=/opt/kidpal/front KIDPAL_SCHEDULE_MD=/root/.openclaw/workspace/kidpal/schedule.md KIDPAL_ALERTS_DIR=/opt/kidpal/alerts KIDPAL_SEMANTIC_JSON=/opt/kidpal/alerts/semantic.json /opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/reminder_schedule_lib.py"]`——注意 cron 实际命令已钉 `/opt/kidpal/venv/bin/python3` 绝对路径（runbook 裸 `python3` 陷阱仅影响人工复跑）；② [apply-xiaozhi-overlay.sh:137-141](../../apply-xiaozhi-overlay.sh)：`mkdir -p "${KIDPAL_OPENCLAW_SCRIPT_DIR:-$DATA_DIR/openclaw-patch}"` + `cp "$HERE/openclaw-patch/$script" "${KIDPAL_OPENCLAW_SCRIPT_DIR:-$DATA_DIR/openclaw-patch}/$script"`；③ 行尾归一 md5：`/opt/kidpal/openclaw-patch/reminder_schedule_lib.py` = `246ae98954cbc0751b7ba50ce06bc21d`（旧），`xiaozhi-docker-data/openclaw-patch/reminder_schedule_lib.py` = `5bb74ca3f35438649c58073eece8093f` = 仓内版（新）；④ runbook [DEPLOY-RUNBOOK.md:176](../../../server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md) 写裸 `python3`。
**根因**：overlay 脚本的铺放目标默认值与 cron 消费路径是两个独立演化面，无清单对账（T1 缺失的直接后果），漂移静默。
**改法**：按 OQ-1 裁断（推荐 (a)，r1 F1 补正后 (a) 含连带义务）：manifest/deploy.sh 直接管 `/opt/kidpal/openclaw-patch/`（cron 消费路径不动）**并直管两个配置消费位**（`/opt/kidpal/openclaw-reminder.config.yaml` 与 `/opt/kidpal/front/deploy/openclaw-schedule.config.yaml`，见 T1——不补此两条则「单一写者」宣称不成立：overlay 渲染的 config 落 `$DATA_DIR` 无人消费位，配置缺位被 `DEFAULT_LADDER_CONFIG` 同值回退掩盖，仓内 [openclaw-reminder.config.yaml:13-14](../openclaw-reminder.config.yaml) 自注三档常量与 DEFAULT 逐值相等，故「改配置不生效」缺陷至今未显症状）；`apply-xiaozhi-overlay.sh` 第 125–146 行整块撤除（config 渲染 + lib staging 两个无人消费位写者一次清掉，单一写者 = deploy.sh；**不撤** prompt 渲染与 `.config.yaml` 注入——那是引擎消费闭环，保留）；runbook §8.3 人工复跑命令改钉 `/opt/kidpal/venv/bin/python3` 绝对路径并与 cron 实际命令对齐、**§8.2 全节（第 155–163 行）对账清理**——不只 export 行（r1 F3）：第 158 行「+ openclaw-reminder.config.yaml」渲染句、第 159 行目标目录注释随撤块一并失实，全节改写为「脚本与配置由 `deploy.sh` 按 manifest 铺放，`KIDPAL_ALERTS_DIR` 仅交接目录语义保留」（r2 F10）；**渲染面声明收尾（r2 F9）**：[provider.config.yaml:47-48](../provider.config.yaml) 的 `reminder_config` 块注释「渲染面 = apply-xiaozhi-overlay.sh（与 prompt 同批上 ECS）」改指「manifest 登记、`deploy.sh` 直管消费位」——该文件是 T5 自引「渲染面 == 声明面」不变式的承载者，声明失实即破不变式，同批必改；TECH-261012 条目证据段 append 补记：本根因修正（「仓内新版未同步到 ECS」句的精确化）+ **钉「修复项 ③ 已由本批 T1 承接、dev 批只对账 ①④」防双做**（r1 F4）。与在途 dev 批的撞面为时序性：其改动未落盘，后到 rebase 即可，文本撞面小（r1 F8）。
目标文件：`server/deploy/apply-xiaozhi-overlay.sh`（改）· `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（改）· `server/deploy/provider.config.yaml`（改）· `docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md`（改，append-only 补记）

### T5 · 部署文档与实况脱节
**问题**：部署面文档（README / 两镜像 README / runbook）描述的是「手工 + 各自为政」的旧流程，`README.md:104` 的 `GATEWAY_DEPLOY_DIR` 典型值仍指向 skill 已记为 stale decoy 的 `main/xiaozhi-server` 路径；ECS `deploy/provider.config.yaml` 渲染副本声明面指 v9 而仓内已 v10（渲染面 == 声明面不变式在 live 侧破口）。
**证据**：ECS `deploy/provider.config.yaml` diff 实测：live 含 `artifact: "openclaw-system-prompt.v9.md"`，仓内 [provider.config.yaml](../provider.config.yaml) 已指 v10；live `.config.yaml` 的 `prompt:` 块实测含 v10「实时事实回合」行（功能已生效，属声明副本滞后）。
**根因**：ECS `deploy/` 是 overlay 渲染目标 + 手工同步混放（git status 实测 `deploy/` untracked），无重铺机制时渲染副本滞后于仓内演进。
**改法**：`deploy.sh` 把 `provider.config.yaml` 等 overlay 输入一并纳入 manifest 重铺（首铺即对齐 v10）；README 部署节改写为 deploy.sh 单入口 + manifest 说明，`GATEWAY_DEPLOY_DIR` 注记修正为 live 真实配置位（`deploy/xiaozhi-docker-data`，与 kidpal-ecs-gateway skill 记载一致）；两镜像 README 的「手动部署」句改为「manifest 管、deploy.sh 铺」；`openclaw-schedule.config.yaml` 的 ECS 缺失随首铺消除。
目标文件：`server/deploy/README.md`（改）· `server/deploy/openclaw-skills/README.md`（改）· `server/deploy/openclaw-agents/README.md`（改）

### T6 · GitHub 单源接线与 ECS deploy clone
**问题**：部署闭环缺第一环——ECS 无拉取源。仓内 `origin` 已配 `https://github.com/fandanpeng/KidPal.git`。
**证据**：2026-10-13 起草期两次实测——起草前 `git ls-remote origin` 为空仓（零引用），起草中 human 完成首推，复测 `refs/heads/main = 2ce0d4282e3bc920a5e8150133b648b8f1f9265e`（= 本地 main tip，已含至 261012 批全部历史）。push 前置 secret 扫描实测：ECS 实钥 `sk-3b3470a327544598` 仓树零命中（`git grep -c` exit 1），长 token 形态唯一命中为验收测试常量 `Bearer WRONG-TOKEN-0123456789abcdef`（`server/acceptance/archive/test_gateway_directionB.acceptance.py:762`，非真凭据）。
**根因**：repo 自建立即本地演化，push 铁律（remote push 归 human，OQ-3 已裁 (a)）下首推长期未发生；ECS 侧从未建拉取面。
**改法**：新增 `server/deploy/ecs-bootstrap.md`——ECS 侧一次性引导指引：deploy key（只读）生成与 GitHub 配置、`/opt/kidpal/deploy-clone` 克隆（shallow、可 sparse 至 `server/deploy/`）、secrets 豁免声明、首跑 `deploy.sh verify` 的红→绿预期（红 = §2 实测 6 处漂移基线）。首推已发生；本批 landing 合并 main 后 human 再推一次含本批的 main，ECS 即可 bootstrap（验证集 §5 第 8 条时点相应为「本批 main push 后」）。
目标文件：`server/deploy/ecs-bootstrap.md`（新增）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 部署清单 | `server/deploy/manifest.yaml` | 新增 | 消费方：deploy.sh / verify-deploy.sh / 人工对账 |
| 部署入口 | `server/deploy/deploy.sh`、`server/deploy/verify-deploy.sh` | 新增 | 消费方：human 部署动作 + systemd timer |
| 定时核验 | `server/deploy/kidpal-deploy-drift.service`、`server/deploy/kidpal-deploy-drift.timer` | 新增 | 消费方：ECS systemd |
| 镜像补齐 | `server/deploy/openclaw-skills/workspace-main/**`（5 件） | 新增 | 消费方：manifest diff 基准；live 副本来源 |
| overlay 脚本 | `server/deploy/apply-xiaozhi-overlay.sh` | G2 | 删 staging 步骤（T4）；消费方：ECS 引擎 overlay 渲染 |
| runbook | `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` | G2 | cron 命令对齐 + 入口改写 + §8.2 全节对账；消费方：运维人工 |
| 配置声明面 | `server/deploy/provider.config.yaml`、`server/deploy/openclaw-reminder.config.yaml`、`server/deploy/openclaw-schedule.config.yaml` | G2 | 部署声明句改指 manifest/deploy.sh 直管（r2 F9，只改声明不动值）；消费方：声明面读者 + manifest |
| 部署文档 | `server/deploy/README.md`、两镜像 README | G2 | 流程描述改写；消费方：部署执行者 |
| CRQ 条目 | `docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md` | G2 | append-only 证据补记；消费方：dev 批对账 |
| 引导指引 | `server/deploy/ecs-bootstrap.md` | 新增 | 消费方：human 一次性 bootstrap |
| 固件 / 契约 / `.claude` | — | 未触 | G1/契约面为零 |

**sweep 透镜义务**：本批新断言 = verify-deploy.sh 的漂移判定（live 校验和 ≠ manifest 期望）。落地前以该判定 pattern（行尾归一 md5 全量比对）对当前 live 预跑一遍，原始输出进 landing-summary——即本 PROPOSAL §2 各 T 证据段的 2026-10-13 实测表（6 处漂移清单），预跑结果与该表一致（同批同日实测）。manifest 首版条目清单由机械枚举生成：`ls /root/.openclaw/workspace/skills/` + `ls /root/.openclaw/workspace-kidpal/skills/` + `ls /opt/kidpal/front/front/ /opt/kidpal/front/gateway/`（2026-10-13 实测，目录成员逐条展开，无「Omitted」截断形态）**+ 消费位派生枚举**：对每个消费者按其路径解析规则（env 显式覆盖 / `dirname(__file__)` 相对 / `parents[N]` 派生）推导默认消费位并入 census——先导两例 = `reminder_schedule_lib.py:472-474` 与 `front_app.py:672-674`（r1 F1/F2 教训：`ls` 目录枚举抓不到 env 默认派生位，两处配置缺位正是由此漏出；落地期 census 表随 manifest 定稿逐消费者补全解析规则列）。

## 4. human 取舍点（OQ）

- **OQ-1 · reminder lib 及配置的铺放落点**：(a) manifest/deploy.sh 直管 cron 现路径 `/opt/kidpal/openclaw-patch/` + 两个配置消费位，overlay 撤第 125–146 行整块（**推荐**——cron 命令、runbook 复跑入口与消费路径全不动，代价是 overlay 动两处：删 staging 删 config 渲染）；(b) cron 改指 `xiaozhi-docker-data/openclaw-patch/`（lib 与 config 同目录、路径天然闭合、overlay 现行为可整体保留，但要改 openclaw cron 作业 + runbook，且 live 配置落点跟着搬）。架构师评审意见（r1 F7，供参考不代裁）：本 OQ 与 OQ-2 属技术裁量、可内化为设计默认 (a)；OQ-3/OQ-4 应留 human。若 human 采内化意见，默认位即 (a) 连带 r1 F1 补正。
- **OQ-2 · front 运行形态**：(a) deploy clone → manifest 拷贝到 `/opt/kidpal/front`（**推荐**——systemd 单元与运行布局零改动，verify 闭环比照即可）；(b) systemd 直指 checkout 内运行（一致性更强，但 live 耦合整棵仓树卫生 + import 路径面要动）。（r1 F7 意见：技术裁量、可内化默认 (a)，同 OQ-1 注。）
- **OQ-3 · GitHub 单源形态**：(a) 整仓单 origin、human push 即部署闸（**推荐**——现有 push 铁律直接复用，无第二源）；(b) 部署专用镜像仓（避免 ECS deploy key 触达治理面，但引入双仓同步问题）。若选 (b)，ecs-bootstrap.md 相应改写。
- **OQ-4 · live TEMP-DIAG 三行的处置**：(a) 随首次部署以仓内版覆盖、不回流仓（**推荐**——TEMP-DIAG 是排障临时日志，与固件侧 261012 批「TEMP-DIAG 清零」先例同性质）；(b) 先回流仓再清零（多一轮 review，价值低）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

时点标注：pre-commit = landing 提交前（本 worktree）；post-landing = 本批合并 main 且 human push 后（ECS 侧命令经 `python c:/tmp/ssh_helper.py` 执行）。

1. pre-commit：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/261013-gov-review-deploy-single-source-loop/PROPOSAL.md` → 形态零错、anchorUnion 成员名单与 §2 各 T 锚清单逐 token 一致。
2. pre-commit：`node .claude/scripts/changes-index.mjs --upsert 261013-gov-review-deploy-single-source-loop` → `delivery/changes/arch/INDEX.md` 出现本 id 行（裸跑全量重建亦幂等）。
3. pre-commit：`git grep -c 'sk-3b3470a327544598' -- .` → exit 1（零命中，push 前置）；`bash -n server/deploy/deploy.sh && bash -n server/deploy/verify-deploy.sh` → 语法零错。
4. post-landing（push 后 ECS bootstrap）：按 `ecs-bootstrap.md` 建 clone 后 `bash server/deploy/deploy.sh verify` → 首跑**预期红**：输出漂移清单含 §2 实测 6 处（漂移基线快照留 landing-summary）。
5. post-landing（首次部署）：`bash server/deploy/deploy.sh <ref>` → 铺放清单与 manifest 一致、服务重启成功；随后内嵌 verify → exit 0。
6. post-landing：`python c:/tmp/ssh_helper.py "md5sum /opt/kidpal/front/gateway/provider.py"` → 行尾归一后 = 仓内 `server/gateway/provider.py` 值（`2029a7f697e1b6a70e8679de80cd6fd6`，含 `openclaw/kidpal` 修正）。
7. post-landing：`python c:/tmp/ssh_helper.py "tr -d '\r' < /opt/kidpal/openclaw-patch/reminder_schedule_lib.py | md5sum"` → = 仓内值 `5bb74ca3f35438649c58073eece8093f`；`openclaw cron show 862ce6ab --json` 的 argv 仍指该路径（未动）。
8. post-landing：`python c:/tmp/ssh_helper.py "grep -c TEMP-DIAG /opt/kidpal/front/front/front_app.py"` → 0（按 OQ-4 (a) 裁断时）。
9. post-landing：`systemctl list-timers | grep kidpal-deploy-drift` → timer 在列；等一跳后 `/opt/kidpal/alerts/deploy-drift.json` 生成且 `drift_count == 0`。
10. post-landing：`python c:/tmp/ssh_helper.py "docker logs --since 30m xiaozhi-esp32-server 2>&1 | grep 'kidpal display-strip patch active'"` → 补丁标记行仍在（部署未破坏 engine-patch bind-mount 面）。
11. post-landing：`python c:/tmp/ssh_helper.py "tr -d '\r' < /opt/kidpal/openclaw-reminder.config.yaml | md5sum"` 与 `tr -d '\r' < /opt/kidpal/front/deploy/openclaw-schedule.config.yaml | md5sum` → 均等于仓内源行尾归一值（两个配置消费位缺文件现状消除，r1 F1 闭合实证）；再等一跳 cron 后 `openclaw cron show 862ce6ab --json` 的 diagnostic 三档值与配置文件一致（不再依赖 `DEFAULT_LADDER_CONFIG` 回退）。
12. post-landing：`python c:/tmp/ssh_helper.py "grep -c '实时事实回合' /opt/kidpal/xiaozhi-esp32-server/deploy/xiaozhi-docker-data/.config.yaml"` → ≥1（secret 承载渲染件以标记锚核验、非 md5——r1 F5 落法示例；display-strip 标记行是 `connection.py` 运行时日志、核验位在第 10 条 docker logs，r2 F11 修正后的纯锚集 = prompt 版本锚 + decision-21 两键，具体锚集随 manifest 定稿）。
13. pre-commit：`bash -n` 覆盖全部新增 shell 脚本与 systemd unit 的 ExecStart 引用一致性——`grep -c 'verify-deploy.sh' server/deploy/kidpal-deploy-drift.service` → 1。

## 6. 留根清单

- TECH-261012 修复项 ①（main workspace 补 `kidpal-schedule-write` skill：B1 写入格式纪律 + 写后机检 + 写后重算落 marker）与 ④（front 对 `schedule_md_err` 的 surfaced 告警，含设计分叉）——归 dev 批课表录入链路收口对账，本批不实施。
- `server/deploy/openclaw-patch/char_stroke_lib.py` 与 `warm_char_cache.py`：ECS 全盘无部署痕迹、消费点未证实（2026-10-13 find 实测零命中）——不进 manifest v1；处置（部署/归档/退役）交 dev 批或下次 cadence retro。
- front 目录 14 个 `.bak-*` 历史备份、`/opt/kidpal/front-test/`、`/opt/kidpal/shim/`（已停用）：首次部署后集中清理，另行小动作。
- main workspace AGENTS.md 与 4 个新镜像 skill 的**内容**未经治理裁定（镜像只取 diff 基准职责，不视为内容批准）——内容演进仍走「先改仓内镜像进 review 再同步 live」纪律（openclaw-agents README 既有约定）。
- deploy.sh 的 rollback：`.deployed-commit` 戳 + 部署前 `.bak` 快照（deploy.sh 自建、替代手工 .bak 习惯），完整回滚流程殿后到首次实弹部署后按实况补。
- 每日 drift 告警的消费方（谁看 `/opt/kidpal/alerts/deploy-drift.json`）：本批只落盘，surfaced 化与 TECH-261012 ④ 同型分叉，另行裁定。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

> 判官：architect（Track B 第 2 步评审循环）。完整 findings、逐项复证与判官侧独立实跑记录在同级 `architect-review.md`（r1/r2/r3 段）；本节为 verdict 轨迹摘要（pre-merge-escalation-gate 判读位）。

**r1 · 2026-10-13 · verdict: RED**（1 blocker + 2 major + 4 minor + 1 意见档）。F1 blocker：reminder 三档配置第二处路径分裂未处置（lib:472-474 env 默认消费位 `/opt/kidpal/openclaw-reminder.config.yaml` 无写者、被 `DEFAULT_LADDER_CONFIG` 同值回退掩盖），T4「单一写者闭合」宣称不成立，OQ-1(a) 推荐位连带削弱——升级 human；F2 major：census 漏消费位派生枚举（front `SCHEDULE_CFG` 同型）；F3/F4/F5/F6/F8 minor；F7：OQ-1/OQ-2 可内化默认 (a)、OQ-3/OQ-4 留 human（意见不代裁）。

**r2 · 2026-10-13 · verdict: RED（轻量打回，1 major + 2 minor）**。r1 全 findings 闭合核验通过（撤块行边界 125-146 精确、钉值实证可证伪、preflight 17 锚）；F9 major：三个仓内文件的「随 overlay 上 ECS」渲染面声明在撤块后失实且未入 sweep（`provider.config.yaml:47-53` / 两 config 头注）；F10 minor：runbook §8.2 清理范围收窄；F11 minor：§5 第 12 条锚法示例死臂。无结构性缺口、不需升级 human。

**r3 · 2026-10-13 · verdict: GREEN**。DRAFT v1.2 复审：preflight 20 锚与围栏块逐 token 一致；F9/F10/F11 全部闭合（含钉值测试 `test_reminder_units.py:132` 守值约束核对属实）；无未结 BLOCKING/SHOULD；一条非阻断注记（§5 第 12 条示例单锚 vs 散文双锚集，已收口「具体锚集随 manifest 定稿」，landing-summary 核验时顺带看一眼）。三轮累计实证义务复核（md5 ×3、ls-remote、secret 扫描、全部 file:line 引据、撤除行边界、钉值测试名）全过，sweep UNCLASSIFIED=0（真零），验证集 13 条双时点机械可判。**结论：GREEN，可冻结（以 human 立项 ack + OQ 终裁为前提，主线程报两者均已达成）**。回炉条款未触发：OQ-1 终裁 = (a)（r3 尾注的「终裁非 (a) 须回炉 r4」条款不生效，无需 r4）；OQ-2 亦 (a)。GREEN 后流程照协议：主线程标 FROZEN → packet-bound landing → landing-verifier 独立核验 → retro → 前置升级闸。

**verdict: GREEN**（r3 · 2026-10-13 · Track B 评审循环收口——判官终判；r1 RED / r2 RED 轻量轨迹与完整 findings 见上及 `architect-review.md`）
