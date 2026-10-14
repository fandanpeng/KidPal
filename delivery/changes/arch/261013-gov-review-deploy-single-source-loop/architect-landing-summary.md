# architect-landing-summary · 261013-gov-review-deploy-single-source-loop

packet-id：FROZEN PROPOSAL §2 改法集（T1–T6）+ §5 验证集 · 评审轮次 R1 RED → R2 RED（轻量）→ R3 GREEN · change-id 261013-gov-review-deploy-single-source-loop

> 凭据：human 立项 ack + OQ 终裁（全推荐位 (a)，2026-10-13 会话）+ architect r3 GREEN（`architect-review.md` r1–r3）。落地时间 2026-10-13，落地者 = 主线程（packet-bound 单通道）。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 清单 + 镜像补齐 | `server/deploy/manifest.yaml`（新增，32 条目 + 2 cron）；`server/deploy/openclaw-skills/workspace-main/`（AGENTS.md + 4 skill 新增，SFTP 直拉、字节校验 5/5）；两 config 声明句改写 | 20 路径的部署单一事实源成文；main workspace 镜像缺口 5 件补齐 |
| T2 部署入口 | `server/deploy/deploy.sh`（新增） | fetch+checkout → manifest 铺放 → 变更集分判重启面 → 引擎面变更跑 overlay 渲染 → `.deployed-commit` 戳 + 快照 → 自检；`--first-run`/`--dry-run` 豁免语义如文 |
| T3 漂移机检 | `server/deploy/verify-deploy.sh`、`kidpal-deploy-drift.service`、`kidpal-deploy-drift.timer`（新增） | md5 行尾归一 + marker 锚 + cron 三维比对 + 漂移落 `deploy-drift.json`；每日 04:17 timer |
| T4 路径分裂修复 + 声明收尾 | `apply-xiaozhi-overlay.sh`（撤 125–146 双写块）；`DEPLOY-RUNBOOK.md`（§8.2 全节 + §8.3 钉 venv python3）；`provider.config.yaml`（声明句）；`docs/crq/tech/TECH-261012-…md`（append 补记 + 防双做） | 单一写者 = deploy.sh；两处配置消费位直管；TECH 证据表述修正落账 |
| T5 文档对齐 | `server/deploy/README.md`（部署入口节 + GATEWAY_DEPLOY_DIR 修正）；`openclaw-skills/README.md`（6 skill 全表化）；`openclaw-agents/README.md`（main AGENTS.md 行） | 文档与 manifest 实况一致；stale decoy 注记修正 |
| T6 GitHub 接线 | `server/deploy/ecs-bootstrap.md`（新增） | deploy key / clone / 首铺 / timer 安装 / 全绿确认五步成文，human 一次性引导就绪 |

## 2. 验证证据

pre-commit 段（2026-10-13 本 worktree 实跑，逐条机械可判）：

- `node .claude/scripts/landing-fidelity-check.mjs --preflight <PROPOSAL>` → `ok: true`、`anchorCount: 20`、`findings: []`（anchorUnion 与围栏块 20 行逐 token 一致）→ 绿。
- `node .claude/scripts/changes-index.mjs --upsert <id>` → INDEX 行在（draft → frozen 状态随 FROZEN 更新）→ 绿。
- `git grep -c 'sk-3b3470a327544598' -- .` → exit 1（ECS 实钥零命中）→ 绿；`bash -n` × deploy.sh / verify-deploy.sh / apply-xiaozhi-overlay.sh → 三绿；`grep -c 'verify-deploy.sh' server/deploy/kidpal-deploy-drift.service` → 1 → 绿。
- manifest 完整性（yaml.safe_load + 源存在性扫描）→ `entries=32`（owner: deploy.sh 30 / overlay 2）、`cron=2`、无缺源、唯一无 src 条目 = `.config.yaml`（marker 核验，预期）→ 绿。起草期两处 YAML 流映射解析红（note 内半角冒号 `prompt: 键`、`parents[2]` 方括号 = 流指示符）当场修复复验，两红均已闭合。
- `cd server && python -m pytest tests/test_reminder_units.py -q --import-mode=importlib` → **37 passed**（含 `test_ladder_config_carrier_matches_spec_constants` 钉值——三档值未动的实证）→ 绿。
- 镜像 5 件字节校验：SFTP 拉取后本地 md5 与 ECS `md5sum` 逐件一致（a9449870 / 97296735 / 3e53b51d / ad18b421 / 1cefff54）→ 5/5 绿。

post-landing 段（§5 第 4–11 条）**未执行、非偷工**：其时点 = 本批合并 main + human push 后经 `ecs-bootstrap.md` 引导（首铺 `--first-run`、timer 安装、全绿确认），PROPOSAL T6 已声明该时点；bootstrap 执行时逐条留痕。

landing-verifier 独立核验：verdict = **PASS**（2026-10-13，`landing-verdict.md`——结构门、越权扫描、20/20 逐文件、机械集复跑、破坏性自检全过；两条 info 级提示不阻断，见 §3）。

## 3. 诚实注记

- ADVISORY / WARNING 类披露共 2 条（均为 landing-verifier info 级）：① timer 面上 verify-deploy.sh exit 1 会记 unit failed——注释已自洽（告警载体 = alerts/deploy-drift.json），属有意设计；② deploy.sh 快照目录拼接依赖 `BACKUP_DIR` 无尾斜杠——当前正确，后续改脚本勿加尾斜杠。
- 实现细节层两处冻结面内决定：`--dry-run` 跳过部署前漂移拦截（预览不该被拦死）；引擎面变更时以 ECS deploy 目录的 overlay 副本执行渲染。
- 未落项（PROPOSAL §6 留根，去向不变）：TECH-261012 修复项 ①④ 归 dev 批；char_stroke_lib.py / warm_char_cache.py 消费点未证实、不进 manifest v1；front 侧 `.bak` 历史与 front-test/、shim/ 清理另行小动作。
- OQ-4(a) 效果预告：live front_app.py 的 3 行 TEMP-DIAG 将随 ECS 首铺消失、不回流仓（不可逆丢弃，human 已裁）。

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

---

## 附：post-landing 段执行留痕（2026-10-13，ECS bootstrap 实况）

§5 第 4–11 条于当日 bootstrap 执行完毕，关键结果：

- **bootstrap**：只读 deploy key（ed25519，human 在 GitHub Deploy keys 贴入）+ `/opt/kidpal/deploy-clone`（shallow，`6fc7eef`）建成；`deploy.sh --first-run` 首铺 11 件（dry-run 预演清单与实跑一致）。
- **第 4–9 条**：`verify-deploy.sh` 首跑报 3 漂移 → 全部定性处置：① `.config.yaml` 锚 `engine_timeout` = 本批 manifest 自编锚错（真实键 `close_connection_no_voice_time`）；② `kidpal-reminder-semantic` agent 漂移 = 本批 verify 脚本读键错（实际 JSON 顶层 `agentId`）——①②修复 commit `c98a511`（经 PR #2 合入，clone 复拉复验）；③ `kidpal-schedule-health` 未注册 = §8.3c post-deploy 步骤，当日注册（every 15m / agent main）后自然消解。
- **终验**：`verify-deploy.sh` 复跑 **`{"drift_count": 0}` exit 0**（全 34 条目 + 3 cron 核验绿）。
- **第 10 条**：engine-patch 标记行在位（引擎重启后组件全初始化成功，`unhealthy` 为已知装饰性）；live 实况另证：reminder lib 新版在位（`drop_refresh_marker` ×2，**触发③ marker 链首次真 live**）、两配置消费位就位、main workspace 7 skill、front active 且 TEMP-DIAG 归零（OQ-4 执行）、provider 默认值归一。
- **timer**：`kidpal-deploy-drift.timer` 已 enable（首跳 2026-10-14 04:17）。
- **部署戳**：`.deployed-commit` = `6fc7eef`；备份 `/opt/kidpal/deploy-backups/20261013-135900/`（回滚可用）。

**遗留（human 面）**：QQ 路由目标（家长 openid，TECH-261012 证据 5 既有缺口，非本批新增）配置后 health 主动转告方可投出；QQ 录入真机轮（TECH-261012 的 QSW-A5）待人工执行留痕。
