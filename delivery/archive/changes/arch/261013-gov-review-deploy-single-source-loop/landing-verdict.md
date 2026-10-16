# Landing Verdict · 261013-gov-review-deploy-single-source-loop

**verdict = PASS**（可进 retro → 升级闸 → 合并本地 main；merge 归 human）

核验者 = landing-verifier（独立实例，只读核验）；核验时间 2026-10-13；核验方法 = 结构门机检 + 逐文件人读 diff，不采信 landing-summary 自述（其结论全部亲自复跑）。无 SSH 通道，post-landing（ECS 段）未连接、未尝试。

## 1. 结构门（机械）

`node .claude/scripts/landing-fidelity-check.mjs --preflight …/PROPOSAL.md` → **ok:true**，section2BodyFound=true，anchorCount=20，findings=[]，advisories=[]；anchorUnion 20 成员与 landing-manifest 围栏块 20 行逐条一致。EXIT=0。

## 2. 越权扫描（机械）

`git status --porcelain`：修改件 9（全在 manifest 内）+ 新增件 11（全在 manifest 内）+ `delivery/changes/arch/261013-gov-review-deploy-single-source-loop/`（review 证据集）+ `delivery/changes/.changes-index-state.json` 与 `delivery/changes/arch/INDEX.md`（changes-index 脚本产物，允许）+ `.codebase-memory/`（机器再生面，按仓纪常脏不算）。**manifest 外实质改动 = 0。**

## 3. 逐文件语义核对（20/20）

| # | 文件 | T | 状态 | 依据（actual diff/内容 vs PROPOSAL expected） |
|---|---|---|---|---|
| 1 | `server/deploy/manifest.yaml`（新增） | T1 | ok | 32 条目清点核实：front 源 12 + 引擎 overlay 输入 5 + 渲染产物 2（owner=overlay，只核验不铺）+ OpenClaw 创作面 8 + reminder 消费位 3 + 漂移 timer 2；另 cron_jobs 2（argv 命令串 + agent/schedule）。两个配置消费位（`/opt/kidpal/openclaw-reminder.config.yaml`、`/opt/kidpal/front/deploy/openclaw-schedule.config.yaml`）与 cron lib 消费位 `/opt/kidpal/openclaw-patch/` 均直管——OQ-1(a) 连带义务（r1 F1 补正）落实 |
| 2 | `server/deploy/deploy.sh`（新增） | T2 | ok | 实现与冻结面对位：部署前漂移拦截（fail-loud，`--first-run` 首铺豁免、`--dry-run` 预览豁免）→ fetch+checkout → 按 manifest 铺 owner=deploy.sh 条目 → **变更集决定重启面**（仅 front 面变更 systemctl restart kidpal-front；仅引擎面变更 docker restart xiaozhi-esp32-server；OpenClaw 面 restart:none）→ 引擎面变更才跑 overlay 渲染 → `.deployed-commit` 戳 + deploy-backups 快照 → 末尾自动 verify。secrets.env 零引用 |
| 3 | `server/deploy/verify-deploy.sh`（新增） | T3 | ok | md5 行尾归一比对 + marker 锚（`.config.yaml` 用锚集 `["实时事实回合","engine_timeout"]` = prompt v10 版本锚 + decision-21 两键，r3 非阻断注记的双锚集在 manifest 定稿补全——闭合）+ cron 命令串/归属/调度比对；漂移写 `/opt/kidpal/alerts/deploy-drift.json` + exit 1，`--no-alerts` 供部署前拦截调用。禁 md5 于 secret 承载件、禁期望值——r1 F5 落实 |
| 4 | `server/deploy/kidpal-deploy-drift.service`（新增） | T3 | ok | ExecStart = deploy-clone 内 verify-deploy.sh；`grep -c 'verify-deploy.sh'` = 1（§5 第 13 条过） |
| 5 | `server/deploy/kidpal-deploy-drift.timer`（新增） | T3 | ok | OnCalendar 每日 04:17 + Persistent |
| 6 | `server/deploy/ecs-bootstrap.md`（新增） | T6 | ok | 只读 deploy key（不勾 write）、`/opt/kidpal/deploy-clone` 克隆、首铺 `--first-run` 预期红基线（6 处漂移 + 2 处配置缺位）、timer 安装、全绿确认清单、secrets 免疫验证步骤 |
| 7–11 | workspace-main 镜像 5 件（新增） | T1 | ok | 5/5 非空；md5 与 PROPOSAL T1 证据的 live 实测值逐件一致：AGENTS.md=a9449870ea8998b4fe1a7e9f6aecf72d、board-context=3e53b51d…、char-strokes=ad18b421…、schedule-answer(main)=972967357a33fa450a1c0a4fb5df0173、qqbot-reply-diagnosis=1cefff54… |
| 12 | `apply-xiaozhi-overlay.sh`（改） | T4 | ok | 撤除块 = 原 config 渲染 + reminder lib staging 两段（PROPOSAL 指称 125–146 行范围，diff 实删 24 行 + 尾换行），换 4 行指针注释指向 manifest；**prompt 渲染与 `.config.yaml` 注入段原样保留**（grep 复核在位）；`bash -n` 过 |
| 13 | `DEPLOY-RUNBOOK.md`（改） | T4 | ok | §8.2 全节改写：手工铺放步骤（overlay 渲染句 + export KIDPAL_OPENCLAW_SCRIPT_DIR + 目标目录注）撤除，`KIDPAL_ALERTS_DIR` 交接目录语义保留（r2 F10）；§8.3 改钉 `/opt/kidpal/venv/bin/python3` 绝对路径并注记裸 python3=3.6.8 SyntaxError 依据 |
| 14 | `README.md`（改） | T5 | ok | 新增「部署入口」节（manifest 单一权威 + deploy.sh 流程 + verify 三时机 + bootstrap 链接 + 热修 fail-loud 纪律）；`GATEWAY_DEPLOY_DIR` 注记修正（`main/xiaozhi-server` = stale decoy，真身 `deploy/xiaozhi-docker-data/`） |
| 15 | `openclaw-skills/README.md`（改） | T1/T5 | ok | 「live 部署面目前是手动的」句废除；6 skill + 双 AGENTS.md 全清单表化；「新增 live skill 同步加 manifest 条目」纪律补入 |
| 16 | `openclaw-agents/README.md`（改） | T5 | ok | workspace-main AGENTS.md 镜像行补齐 + manifest/deploy.sh/verify 管辖声明 |
| 17 | `openclaw-reminder.config.yaml`（改） | T1/F9 | ok | 仅头注释部署声明句改指 manifest/deploy.sh 直管；`pytest tests/test_reminder_units.py` **37 passed**（`test_ladder_config_carrier_matches_spec_constants` 绿）= 三档值未动实证 |
| 18 | `openclaw-schedule.config.yaml`（改） | T1/F9 | ok | 同上，声明句注明原「随 overlay 上 ECS」为从未渲染的失实声明；`school_start` 未动（37 passed 同证） |
| 19 | `provider.config.yaml`（改） | T4/F9 | ok | reminder_config 块「渲染面 = apply-xiaozhi-overlay.sh」改指 manifest 登记、deploy.sh 直管消费位——T5「渲染面==声明面」不变式修复；值面未动 |
| 20 | `docs/crq/tech/TECH-261012-…md`（改） | T4 | ok | **纯 append**（diff 仅新增段、既有行零删改）：证据 3 第 2 条表述精确化（新版已在 ECS、真根因 = 铺放/消费路径分裂）+ 同型第二处配置缺位 + 修复项归属钉死（②③由本批承接、dev 批只对账 ①④——防双做，r1 F4 闭合） |

## 4. §5 pre-commit 验证集（亲自复跑）

| 项 | 结果 |
|---|---|
| 结构门 --preflight | ok:true，20 锚，零 findings |
| `git grep -c 'sk-3b3470a327544598' -- .` | **exit 1**（零命中，push 安全） |
| `bash -n` deploy.sh / verify-deploy.sh / apply-xiaozhi-overlay.sh | 三绿 |
| `grep -c 'verify-deploy.sh' kidpal-deploy-drift.service` | 1（§5 第 13 条过） |
| `pytest tests/test_reminder_units.py` | **37 passed**（声明面只改句不动值的机械实证） |
| INDEX 钉行 | `INDEX.md:13` 本 id 行在（frozen 态） |

## 5. packet-binding 核对

landing-summary 引 packet = FROZEN PROPOSAL v1.2 本 id；architect-review.md r3 verdict GREEN、生效前提（human ack + OQ 全推荐位 (a)）按主线程凭据满足，OQ-1=(a) 未触发 r4 回炉条款。landing 实际范围 = manifest 20 行 = §2 anchorUnion，无 scope creep（第 2 节机检 + diff 抽查双证）。

## 6. 破坏性自检

- manifest 条目逐条过目：**零 OpenClaw 运行态文件**（无 DREAMS.md / memory/ / sessions/ / agent sqlite；文件头纪律明写「永不入清单」）。
- deploy.sh / verify-deploy.sh 全文扫描：**零 secrets.env 读写**（唯一命中 = manifest 注记本身声明 `.config.yaml` 为 secret 承载件、禁 md5 禁期望值——是防护声明不是触碰）；secrets 注入仍由 overlay 渲染承担（owner=overlay 条目 deploy.sh 只核验不铺）。
- 部署写入面 = manifest dst 全集（创作面 + 引擎部署面 + systemd 单元）+ 自有备份/戳/告警路径，无误伤面。

## 7. post-landing 未执行段核对

§5 第 4–11 条属 ECS bootstrap 后执行，前置 = 本批合并 main + human push + ecs-bootstrap.md 引导。landing-summary 如实标注未执行，与 PROPOSAL T6 时点声明（「本批 main push 后」）一致——**非偷工，是时点正确的待执行项**。本判定不含 ECS 侧实证，human merge 后按 ecs-bootstrap.md 逐条留痕。

## Findings 清单

无 FAIL 级 finding。两条 info 级观察（不阻断、不要求返工）：

- **[info] verify-deploy.sh timer 面 exit 1 语义**：drift.service `SuccessExitStatus=0` 下漂移日 unit 记 failed——注释已自洽（告警载体 = alerts/deploy-drift.json 落盘，非 unit 状态链），属有意设计；首次实弹部署后若 human 觉得 failed unit 噪音大，可后续小动作调 `SuccessExitStatus=0 1` 并把 exit 2 单列告警。
- **[info] deploy.sh 快照目录拼接**：`"$BACKUP_DIR$(dirname "$DST")"` 依赖 BACKUP_DIR 无尾斜杠的字符串拼接，当前行为正确（快照保留 live 绝对层级）；仅提示后续改动时勿在 BACKUP_DIR 加尾斜杠，否则路径变形。

## 结论

实际落地 diff == FROZEN PROPOSAL v1.2 的 landing 清单与 §2 各 T 改法，逐文件对位、无私货、无漏改、无与批定矛盾。**PASS——放行进 retro 与升级闸；merge 归 human manual 门。**
