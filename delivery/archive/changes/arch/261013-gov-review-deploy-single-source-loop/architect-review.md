# architect-review · 261013-gov-review-deploy-single-source-loop

> Track B 第 2 步评审循环记录（append-only，按轮次追加）。判官：architect（评审档，只读 canonical、只写本文件）。

---

## r1 · 2026-10-13 · verdict: RED（打回，1 blocker + 2 major + 4 minor）

### 0. 评审基准与独立复证

基准：`.claude/references/arch-review-protocol.md`「双轨路由」节 Track B 条款 + `docs/templates/PROPOSAL.template.md` 硬项定义（起草期实证义务 / 目标文件锚清单 / sweep 透镜 / 双时点验证集）。

判官侧独立复证（本 worktree 实跑，2026-10-13）：

- **仓内 md5 复证**：`tr -d '\r' < server/deploy/openclaw-patch/reminder_schedule_lib.py | md5sum` = `5bb74ca3f35438649c58073eece8093f`（= T4 证据③「仓内版」，与 ECS 孤儿副本同值——「新版在孤儿路径」断言自洽）；`server/gateway/provider.py` 行尾归一 md5 = `2029a7f697e1b6a70e8679de80cd6fd6`（= §5 第 6 条期望值）。
- **T6 单源断言独立复证**：`git ls-remote origin refs/heads/main` = `2ce0d4282e3bc920a5e8150133b648b8f1f9265e`（= 本地 main tip，`git rev-parse` 核实在册）。
- **结构门干跑**：`landing-fidelity-check.mjs --preflight` ok，anchorCount=17，anchorUnion 与 landing-manifest 逐 token 一致（§5 第 1 条为真）。
- **secret 扫描复证**：`git grep -c 'sk-3b3470a327544598'` exit 1（零命中）；`git ls-files server/deploy/secrets.env` 空（secrets.env 未跟踪）；`server/acceptance/archive/test_gateway_directionB.acceptance.py:762` 确为 `Bearer WRONG-TOKEN-0123456789abcdef` 测试常量。
- **file:line 引据逐条核对**：overlay `apply-xiaozhi-overlay.sh:137-141`（verbatim 吻合）、runbook `DEPLOY-RUNBOOK.md:176`（裸 `python3` 属实）、`README.md:33`（核验散文叮嘱属实）、`README.md:104`（`GATEWAY_DEPLOY_DIR` stale 典型值属实）、`openclaw-skills/README.md:4`（「live 部署面目前是手动的」属实）、`provider.config.yaml` artifact 已指 v10、kidpal-ecs-gateway skill 存在且记载 live config 真位 = `deploy/xiaozhi-docker-data`、`server/gateway/provider.py:32` 默认 `openclaw/kidpal`（live stale `openclaw/default` 断言与仓内态自洽）、仓内 `front_app.py` TEMP-DIAG 计数 0（live 多 3 行未回流断言自洽）。
- **在途撞面核查**：`git status -- server/deploy/ docs/crq/` 干净；`git diff main...HEAD` 对 T4/T5 六文件零差异（dev/pod-m2 == main）；全分支 2026-10-10 后无对 `apply-xiaozhi-overlay.sh` / `DEPLOY-RUNBOOK.md` 的未合并提交。**当前无实撞**；未来撞面见 F8。

硬项总评：五段固定式六 T 齐整；起草期实证义务基本达标（断言普遍带 2026-10-13 实测标注，仓内可复证部分判官全部复证通过）；§3 sweep UNCLASSIFIED=0；§5 双时点标注齐全、绝大多数机械可判（「首跑预期红」的漂移基线设计尤其好——可证伪）。问题集中在**消费路径派生面的 census 系统性**（F1/F2），恰是本批自我标榜要消灭的病灶类。

### 1. Findings

**F1 · BLOCKER（T1/T4，影响目标效果 → 升级 human）· reminder 三档配置的第二处路径分裂未处置——T4「单一写者闭合」宣称不成立。**

消费链实测（判官从仓内代码独立派生）：`reminder_schedule_lib.py:472-474` 的 `KIDPAL_REMINDER_CONFIG` 默认值 = `dirname(__file__)/../openclaw-reminder.config.yaml`。lib 按 OQ-1(a) 落位 `/opt/kidpal/openclaw-patch/` 后，cron（argv verbatim 不含 `KIDPAL_REMINDER_CONFIG`）实际消费的配置路径 = **`/opt/kidpal/openclaw-reminder.config.yaml`**。而仓内唯一写者是 `apply-xiaozhi-overlay.sh:130-133`，渲染目标 = `$DATA_DIR/openclaw-reminder.config.yaml`（`xiaozhi-docker-data/` 下）。即：**lib 铺放目标与配置消费路径是同族的第二处分裂**——与 T4 自己修掉的 reminder lib 分裂完全同类，manifest 首版条目描述（T1：覆盖「overlay 渲染目标 + reminder lib 的 cron 消费路径」）对此无覆盖：`/opt/kidpal/openclaw-reminder.config.yaml` 既非 overlay 渲染目标、也不是 lib 本体。后果：三档时刻配置在真实消费位无写者、无 drift 核验（若 live 该位缺文件则静默回退 DEFAULT_LADDER_CONFIG——又一个「静默漂移」，本批目标效果的核心承诺正是不再有这类洞）。

连带两点：

- **OQ-1(a) 推荐位被削弱**：选 (b)（cron 改指 `xiaozhi-docker-data/openclaw-patch/`）时，lib 的 `dirname/../` 默认恰好落在 `$DATA_DIR/openclaw-reminder.config.yaml` = overlay 渲染目标——配置路径天然闭合；选 (a) 则配置孤儿化。推荐理由「(a) 改动面与回滚面最小」未计这笔账。(a) 仍可行，但必须补配「manifest/deploy.sh 直管 `/opt/kidpal/openclaw-reminder.config.yaml`」并同步处置 overlay 对 `$DATA_DIR` 副本的渲染（留 = 新孤儿，删 = 改 overlay 的 config 渲染块，二选一写进改法）。
- **修法方向**（供主线程修订参考，不代裁）：manifest 增配 `server/deploy/openclaw-reminder.config.yaml` → `/opt/kidpal/openclaw-reminder.config.yaml`（写者 = deploy.sh），并明确 overlay 渲染块的存废；T1 的 manifest 条目描述补「消费路径派生规则」一栏（见 F2 泛化）。

**F2 · MAJOR（T1/§3）· 消费路径派生面的 census 不系统性——front 的 `openclaw-schedule.config.yaml` 是第二个实例。**

§3 的机械枚举命令 = `ls …/workspace/skills/` × 2 + `ls /opt/kidpal/front/front/ /opt/kidpal/front/gateway/`——**不含 `/opt/kidpal/front/deploy/`**。而 `front_app.py:671-675` 的 `SCHEDULE_CFG` 默认 = `parents[2]/deploy/openclaw-schedule.config.yaml`，即 `/opt/kidpal/front/deploy/openclaw-schedule.config.yaml`。T3 证据「`openclaw-schedule.config.yaml` 仓内有、ECS `deploy/` 无」的 `deploy/` 若指此位，则 T5「随首铺消除」要求 manifest 覆盖它，但 census 命令没枚举到它——manifest 首版是否含该条存疑，且 §3 透镜义务的「目录成员逐条展开」声明与实际枚举范围不匹配。修法：census 不能只 `ls` 目录，须按「每个 manifest 条目 = 消费方 + 路径解析规则（env 显式 / env 默认 / dirname 相对）」系统派生一遍（reminder lib config〔F1〕与 front schedule config〔本条〕都是 env 默认派生位、都不是 ls 顶层可见位——两例同型不是巧合）。

**F3 · MAJOR（T4）· runbook §8.2 的 `KIDPAL_OPENCLAW_SCRIPT_DIR` export 行将变 dead instruction，T4 改法漏收。**

`DEPLOY-RUNBOOK.md:160-163` 现行文 instructs `export KIDPAL_OPENCLAW_SCRIPT_DIR=/opt/kidpal/openclaw-patch`（§8.2 铺脚本节）。staging 步骤删除后该 export 成无操作死指令，且「脚本与配置的目标目录」注释失实。T4 改法只提 §8.3 钉绝对路径——§8.2 须同批清理（本批目标恰是消灭「文档描述旧流程」的漂移，自己不留同类尾巴）。

**F4 · MINOR（T4）· TECH-261012 append 补记须同时记录 ③ 的归属转移。**

PROPOSAL 头部声明「③ 并入本批 T1、①④ 留根归 dev 批」，但 T4 对该条目的改法只写「证据段 append 补记本根因修正」。该条目 frontmatter `handoff` 现指向「dev 批合并处理」且 trigger = dev 批 landing 前对账——若补记不含 ③ 已被本批消费，dev 批对账时有双做镜像补齐的风险。补记应一句钉死：③ 已由 261013 批 T1 承接，dev 批只对账 ①④。

**F5 · MINOR（T1/T3）· 含 secrets 渲染件的核验方式须显式钉「标记行」。**

`.config.yaml` 由 overlay 注入 `OPENCLAW_TOKEN` / `DASHSCOPE_API_KEY` 渲染生成——对它做「行尾归一 md5 与仓内源比对」恒不可能绿（仓内无对应源文件），且 manifest/verify 任何一处都不得承载 secret 期望值。T1 的「核验方式（md5/标记行）」已留位，但须落成硬规则：secret 承载渲染件一律标记行核验（如 `model_name` 行 / `close_connection_no_voice_time` 行 / prompt 版本行），并注意 overlay 的 prompt 注入是「已存在则 SKIP」的幂等形态（`apply-xiaozhi-overlay.sh:112-120`），标记行选取须与该形态兼容。

**F6 · MINOR（T2，设计注）· 重启粒度与 live 运行态。**

T2「按 manifest 声明重启对应服务（… docker xiaozhi-esp32-server …）」——引擎容器重启 = 设备连接全断（常驻在线假设已由 TECH-261010 钉过）。建议 manifest 重启声明按「条目变更集」生效：本轮 diff 未触 overlay/engine-patch 面则不重启引擎，仅提示。非阻断，写进 deploy.sh 设计即可。

**F7 · MINOR（OQ 审，意见不代裁）· OQ-1 / OQ-2 属技术裁量，可内化为设计默认；OQ-3 / OQ-4 应留 human。**

- OQ-1：纯接线选择、双向可逆（若按 F1 补配置直管后 (a) 才真成立为最小面）；OQ-2：front 运行形态，(a) 对 systemd 零改动、可逆。二者不触不可逆性 / 流程权力面，human 反馈「OQ 偏技术」在这两条上成立——判官意见：均可降为设计默认（(a)），PROPOSAL 修订时把 F1 修正吸收进 OQ-1 的 (a) 方案描述即可。
- OQ-3：GitHub 单源形态涉 deploy key 凭据面与 push 闸权力面——留 human 正确。OQ-4：丢弃 live-only 三行属不可逆（无回流即消失）——留 human 正确，虽量级小。
- 注意：OQ-1 若 human 终裁选 (a)，**必须**连带 F1 修正一起生效，否则 (a) 闭合性劣于 (b)。

**F8 · MINOR（在途撞面）· 当前无实撞，未来文本撞面可控。**

判官核查：dev/pod-m2 == main、`server/deploy/**` 工作区干净、全分支无未合并的 overlay/runbook 改动——**当前无在途批撞面**（任务简报所称「261012 批课表链路改动在途」在本仓 git 证据上不成立为未落盘改动，课表链路改动尚未在任何分支出现）。真实风险是时序性的：TECH-261012 留根的 ① ④ 归 dev 批，dev 批将改 runbook §8.3 邻域与 openclaw-skills README——与本批 T4/T5 同文件。本批先落（gov lane ff-only 入 main）、dev 批后到 rebase 即可，文本撞面小；F4 的补记把 ③ 归属钉死后，dev 批的改动面预期也干净了。

### 2. 结论

verdict = **RED**。阻断点唯一且集中（F1），方向清晰、修复面小（manifest 增一条 + overlay config 渲染块存废决定 + runbook §8.2 顺带清理 + census 方法补一段），无方向性翻盘、无需重起草。F2 为 census 义务的实质补全，与 F1 同法可收。**F1 标「升级 human」**：它打回的是本批核心宣称（T4 闭合 / 漂移零静默）而非边角，且其修复与 OQ-1 终裁耦合（human 正在与架构师讨论 OQ，F1/F7 意见应一并呈上）；OQ 终裁权在 human，判官只出意见。

### 3. 下一轮重点复核清单（主线程按 findings 修订后）

1. **F1 落法**：manifest 是否新增 `openclaw-reminder.config.yaml` → `/opt/kidpal/openclaw-reminder.config.yaml` 条目（写者 = deploy.sh）；overlay 的 `$DATA_DIR` config 渲染块存废是否明确裁定；OQ-1 的 (a) 方案描述是否已吸收配置直管（否则 (a)/(b) 闭合性对比是假对比）。
2. **F2 落法**：§3 census 是否补「消费方 + 路径解析规则」派生表，`/opt/kidpal/front/deploy/openclaw-schedule.config.yaml` 是否入 manifest；「首铺消除」断言是否落到具体条目。
3. **F3/F4**：runbook §8.2 清理是否入 T4 目标文件改法描述；TECH-261012 补记是否含 ③ 归属转移句。
4. **F5/F6**：manifest 核验方式对 secret 承载件是否钉标记行规则；deploy.sh 重启粒度是否按变更集。
5. 结构面复跑 `--preflight`（manifest 若因 F1/F2 增删条目，landing-manifest 围栏块须同步——目前 17 锚恰等于围栏块，任何增删都要两边一起动）。

---

## r2 · 2026-10-13 · verdict: RED（轻量打回，1 major + 2 minor——r1 全部 findings 已闭合，本轮仅声明面收尾；收敛中，无结构性缺口）

复审对象：DRAFT v1.1。判官侧独立复核（本 worktree 实跑）：

- **F1 闭合核验**：`reminder_schedule_lib.py:472-474`（env 默认派生）、`front_app.py:672-674`（`parents[2]/deploy` 派生）、`reminder_schedule_lib.py:46`（DEFAULT_LADDER_CONFIG 三值）、`openclaw-reminder.config.yaml:13-14`（「与本文件三档常量一致」自注）、`front_app.py:700-708`（`_schedule_school_start` 静默回退 None）逐处 verbatim 属实；「cron 诊断值 = DEFAULT 逐值」为可证伪实证设计，好。**overlay 撤除行范围核验精确**：125-146 = 注释（125-129）+ config 渲染（130-133）+ staging（134-143）+ WARN else（144-146）整块，prompt 渲染（95-123）与 decision-21 heredoc（85-90）不含在内，「不撤」边界正确——active 验收 `test_261010_engine_decision21_overlay_persist.acceptance.py`（D21-A2 钉 provider.config.yaml engine_timeout 块 + overlay heredoc 两键）不受影响；归档验收 `archive/test_schedule_reminder.acceptance.py` 的 RMD-A2 wiring 断言（要求 deploy 目录某文件提及配置名）撤块后仍由 manifest.yaml 满足且在 archive/ 不入门链——均不构成撤除阻力。
- **preflight 复跑**：ok，17 锚与 landing-manifest 逐 token 一致（判官复跑确认修订未夹带新目标文件）。
- r1 findings 处置逐条：F1 闭合（T1 两直管条目 + T4 整块撤 + OQ-1 吸收连带义务）；F2 闭合（§3 补「消费位派生枚举」方法 + 落地期 census 表承诺）；F3 部分闭合（见 F10）；F4 闭合（补记钉 ③ 承接 + 头部行同步）；F5 闭合（锚法分层，示例小疵见 F11）；F6 闭合（按变更集决定重启面）；F7 按意见档处理（注入不代裁）；F8 闭合（时序性撞面注记）。

### r2 Findings

**F9 · MAJOR（T4/§3）· 渲染面声明收尾缺失——三个仓内文件的「随 overlay 上 ECS」声明在撤块后失实，且未入 sweep 分类。**

T4 撤 overlay 125-146 后，下列仓内声明的「渲染面」指向不复存在：

- `server/deploy/provider.config.yaml:47-53`：reminder_config 块注释明写「渲染面 = apply-xiaozhi-overlay.sh（与 prompt 同批上 ECS）」——撤块后此声明为假；该文件是 T5 自己引证的「渲染面 == 声明面」不变式承载者，本批不留破口。
- `server/deploy/openclaw-reminder.config.yaml:3-4`：「随 server/deploy overlay 上 ECS（与 openclaw-schedule.config.yaml 同目录渲染面）」——撤块后为假。
- `server/deploy/openclaw-schedule.config.yaml:3-4`：「随 server/deploy overlay 上 ECS（与 openclaw-system-prompt.v2.md 同目录渲染面）」——**本就失实**（overlay 从未渲染过它，正是其消费位缺文件的根因之一），本批改 manifest 直管后应一并改真。

修法：三文件进 T4（或 T1）目标文件锚清单 + landing-manifest 围栏块（17→20，两处同动、preflight 复跑），声明改指「manifest/deploy.sh 直管消费位」；`provider.config.yaml` 的 `ladder:` 三值副本与配置文件、脚本 DEFAULT 构成三重复制，本批不强制合并，但声明面须真。**约束**：只改声明/注释，不动三档值——`server/tests/test_reminder_units.py:132` 钉了配置文件值（20:30/07:45/15），值动即破 active 单测。sweep 表须补分类这三处（当前 UNCLASSIFIED=0 是在漏分类前提下的假零）。

**F10 · MINOR（T4）· runbook §8.2 清理范围收窄。**

F3 修订只钉 export 行；§8.2 第 158 行注释「渲染当前版 system prompt + openclaw-reminder.config.yaml」的渲染句在撤块后同样失实，159 行「脚本与配置的目标目录」注释亦然。收尾范围应钉「§8.2 全节（155-163）对账」。

**F11 · MINOR（§5 第 12 条）· 锚法示例的死臂。**

`grep 'kidpal display-strip patch active\|实时事实回合' .config.yaml` 的首臂恒假——display-strip 标记是 `engine-patch/connection.py` 的**运行时日志行**，不出现在 `.config.yaml`（部署时容器都未必起过）。条目仍可判（次臂独撑 ≥1），但作为 r1 F5 的落法**示例**应改纯：只留 `.config.yaml` 内真实存在的锚（prompt 版本锚「实时事实回合」、decision-21 两键 `close_connection_no_voice_time: 86400` / `end_prompt.enable: false`），display-strip 标记行的核验位在第 10 条（docker logs）——避免示例教坏后续 manifest 锚法书写。

### r2 结论

verdict = **RED（轻量）**。r1 的 blocker/结构性缺口（F1/F2）已全部闭合且闭合质量高（撤除边界精确、实证可证伪、census 方法成文）；本轮打回项全部是**声明面收尾**（F9 三文件一句改指 + 入锚，F10/F11 措辞），无新设计、无方向问题，一轮可收。不需升级 human（不影响目标效果，属机械收尾）。

**OQ 项（human 终裁 pending，判官意见不变、显式列出）**：OQ-1（(a) 含 F1 连带义务 vs (b) 天然闭合）、OQ-2（front 运行形态）、OQ-3（GitHub 单源形态——deploy key/流程权力面，必留 human）、OQ-4（TEMP-DIAG 不可逆丢弃——必留 human）。意见：OQ-1/OQ-2 属技术裁量可内化默认 (a)；若 OQ-1 终裁 (b)，T4 改法须按 (b) 重写（cron 改指 + 配置落点搬 + overlay 保留渲染），届时需 r3 复核 (b) 分支闭合性——F9 的三个声明改写在 (b) 下方向不同（provider.config.yaml 渲染面声明可保真），修订时按终裁分支走。

### r3 重点复核清单

1. F9 三文件入锚后 preflight 复跑（17→20，围栏块与 §2 锚两处同动）；声明改写是否「只改声明不动值」（test_reminder_units.py:132 钉值）。
2. F10（§8.2 全节对账）、F11（锚法示例改纯）措辞收口。
3. 若 OQ 终裁回来且非 (a)：按裁断分支复核 T4/OQ-1 描述的 (b) 侧改写与 F1 闭合等价性。

---

## r3 · 2026-10-13 · verdict: GREEN（可冻结——以 human 立项 ack + OQ 终裁为前提；OQ-1 若终裁非 (a) 须回炉，见尾注）

复审对象：DRAFT v1.2。判官侧独立复核：

- **preflight 复跑**：ok，20 锚，anchorUnion 与 landing-manifest 围栏块 20 行逐 token 一致（17→20 增量为 F9 三文件，两处同动无遗漏）。
- **F9 闭合核验**：`provider.config.yaml` 入 T4 锚 + `reminder_config` 块声明句改指 manifest/deploy.sh（判官复核 `provider.config.yaml:47-48`「渲染面 = apply-xiaozhi-overlay.sh」行号与引文属实）；两 config 文件入 T1 锚 + 头注释部署声明句改写、schedule config 的「本就失实（overlay 从未渲染过它）」按 r2 措辞入文（`openclaw-reminder.config.yaml:3-4` / `openclaw-schedule.config.yaml:3-4` 行号属实）；§3 sweep 新增「配置声明面」行（G2）；**值钉约束核验属实**——`server/tests/test_reminder_units.py:132` 确为 `test_ladder_config_carrier_matches_spec_constants`，`assertEqual` 钉 `(20:30, 07:45, 15)`，PROPOSAL 的「只改部署声明句、不动三档值与 school_start」与该钉值测试对得上。
- **F10 闭合核验**：T4 改法已扩为「§8.2 全节（第 155–163 行）对账清理」，158/159 行失实句点名，改写方向（`KIDPAL_ALERTS_DIR` 仅交接目录语义保留）明确。行号复核属实。
- **F11 闭合核验**：§5 第 12 条 alternation 死臂已除——命令改纯 grep「实时事实回合」，display-strip 核验位指回第 10 条 docker logs，锚集描述（prompt 版本锚 + decision-21 两键）与「具体锚集随 manifest 定稿」的收口方式正确。
- r2 findings 全结：F9/F10/F11 均已按判官改法落地，无未结 BLOCKING/SHOULD。
- 一条非阻断注记（不阻 GREEN）：§5 第 12 条示例命令只 grep 单锚而散文描述双锚集（prompt 锚 + decision-21 两键）——属「示例 vs 全集」的表达差，且已显式收口「具体锚集随 manifest 定稿」，landing 期 manifest 定稿时补全即可，判官在 landing-summary 核验时顺带看一眼。

### r3 结论

三轮累计：r1 RED（1 blocker + 2 major + 4 minor——结构性缺口 F1/F2）→ r2 RED 轻量（1 major + 2 minor——声明面收尾）→ 本轮全结。六 T 五段固定式自洽、根因↔改法对位（含 r1 补正后的 T4 单一写者闭合 + 两个配置消费位直管）、起草期实证义务达标（判官三轮累计独立复证：md5 ×3、ls-remote、secret 扫描、全部 file:line 引据、撤除行边界、钉值测试名）、sweep UNCLASSIFIED=0（真零——F9 三文件入分类后）、验证集 13 条双时点标注齐全且机械可判、首跑预期红的漂移基线设计可证伪。

**verdict = GREEN（可冻结）**。生效前提（Track B 第 3 步序列）：

1. human **立项 ack**；
2. **OQ 终裁**：OQ-1 / OQ-2 / OQ-3 / OQ-4。判官意见（不代裁）：OQ-1/OQ-2 可内化默认 (a)（PROPOSAL 改法与推荐位均按 (a) 书写）；OQ-3（deploy key/流程权力面）与 OQ-4（live-only 内容不可逆丢弃）必留 human。
3. **若 OQ-1 终裁非 (a)**（即选 (b)）：T4 改法（overlay 撤 125–146 / manifest 直管消费位 / provider.config.yaml 与两 config 声明句改指）须按 (b) 分支重写，F1 闭合形态不同（(b) 下 overlay 渲染保留、cron 改指 data 目录），届时回炉 **r4 复核**，GREEN 失效直至 r4 通过。OQ-2 若终裁 (b) 同理影响 T2 改法的 systemd 面描述。
4. 冻结前主线程再人工复核 landing-manifest 围栏块（20 行）与 §2 锚清单一致（preflight 已绿，模板义务内的最后一眼）。

GREEN 后流程照协议走：主线程标 FROZEN → packet-bound landing → landing-verifier 独立核验 → retro → 前置升级闸。评审判官职责至此收口，本文件不再有新轮次（除非 OQ 终裁触发 r4 回炉条款）。


