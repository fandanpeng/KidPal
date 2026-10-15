# Review Packet —— 261013-qq-schedule-write-closure

> integrator 制品轮产出（2026-10-13）。状态：本包 Lite 制品轮（raw intake 形状：无 portfolio taskpackage、无 PROPOSAL、无 launch-card）——offline 面全绿（冻结验收 11 齿 + server/tests 91 + test_mnf_a1 转绿 + 八闸 PASS），integrator final 圈定复跑在案；closeout 两 blocking 制品齐；**QSW-A5 真机轮殿后**（post-deploy、human 触发，本包不停门）；feature 分支 commit 待合并门；supervisor 裁断与 merge-driver 归 planner 主线程。rigor=lite 单包、sourceType=regression、riskTier=medium、gateProfile=["test-quality","docs-sync"]。

## 来源链

TECH CRQ 驱动（`docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md`，MAJOR/deferred）——2026-10-12 主线程 live 排障实录（ECS 8.137.91.16：家长经 QQ 让 main agent 写入带星期后缀的日期格 `| 2026-10-12 周一 |` → front 单源 parser `ValueError` 整包拒 →「接下来」三触发全静默停摆、家长零感知）→ 登记 CRQ。本包收口其**剩余修复项 ①④**（②③已由 261013-gov-review-deploy-single-source-loop 承接收口、条目补记钉死归属防双做）；④ 形态 human 2026-10-13 会话裁定 = alerts 件 + agent QQ 转告（弃「仅 alerts 件」与「推设备屏幕」两案）。architect 1g lite 前置技术预评全文 = `plan.md` §4（自建 intake 形状）。

## 根因与修法（一段）

根因 = main workspace 无写课表 skill——家长经 QQ 让 main agent 改写 `kidpal/schedule.md`（B1 §2.4 主录入通道）时无 B1 格式纪律、无写后机检、无写后即时重算，一个脏日期格（日期带星期后缀）即令整份 `schedule.md` 在 front 单源解析器（P2-A2 脏行大声拒绝、整包拒）失败、三触发短路；且 front 报错只在服务端日志、家长零感知。修法 = ① 新增 `kidpal-schedule-write` skill（B1 写入纪律 + 写后机检走 front 单源 parser + 通过后 cron 同款 env 跑 `reminder_schedule_lib.py` 落 `schedule.refresh` marker 触发即时刷新 + 失败还原优先）；④ front `_load_schedule_model()` 解析失败挂接 `_write_schedule_parse_alert` 落 `alerts/schedule-parse-error.json`（同族告警件）+ 新增 `kidpal-schedule-health` skill（cron 每 15m 由 main agent 读告警件、四分支判定、QQ 转告家长、自有 ack 状态件防重复打扰）。

## Change Size

- 实现面 = changeManifest 7 件逐一对账：**新增** `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md`（123 行）+ `.../kidpal-schedule-health/SKILL.md`（53 行）；**改** `server/deploy/manifest.yaml`（+12/−1：两条 skill 条目 + health cron 条目 + provider note 残留修）、`server/deploy/openclaw-skills/README.md`（+2）、`server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（+34，§8.3c）、`server/gateway/front/front_app.py`（+28/−1，告警写 helper + 挂接）、`docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md`（+16，收口补记 append）。
- 另含：冻结验收件 `server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py`（新增 +661，acceptance-author 产、非 changeManifest——按仓内先例链落 `server/acceptance/`）+ closeout 制品（本目录）+ `docs/crq/tech/INDEX.md`（生成件投影刷新 +2/−4）。
- Net（实现面 tracked 非 `.codebase-memory`）：7 文件 +75/−2（另 2 新 SKILL.md +176、冻结验收件 +661 为 additive）。`.codebase-memory/*`（artifact.json / graph.db.zst）为机器再生面，Pod 分支**不 commit**（AGENTS.md 快照流纪律）。
- Scope: 纯 server 面内闭环——skill 文本（OpenClaw 消费）+ front python 告警 additive；零 wire 面（display/设备载荷零触碰，`crossEnd.exempt` 在案）、零契约面、零 spec 面、零 `.claude/**`。
- Split recommendation: 不拆——单根因（QQ 录入链路缺操作层）单包原子变（Lite 分包即回炉）。

## Verification Evidence

- **八闸（scoped 面，integrator 手工复跑 2026-10-13）**：taskpackage-required PASS（warnings=[]）/ change-manifest PASS（**skipped=pre-cutoff**：gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28 → 存量放行）/ contract-lint PASS / acceptance-coverage PASS（QSW-A1~A5 全落，structural 豁免 = QSW-A5）/ mutation-check PASS（**skipped**：mutationRequired=false，reason 在场）/ docs-sync PASS（docsSyncRequired=true，docRefs 三件在场、docsNoChangeReason 在场）/ security PASS（securityReviewRequired=false）/ firmware-build 不在本包改动面（零固件改动）。
- **final 圈定复跑（acceptance / unit，--phase=final 按 scope-file 圈定）**：
  - `acceptance`：`node .claude/scripts/acceptance-run-check.mjs --phase=final --scope-file delivery/taskpackages/261013-qq-schedule-write-closure/test-scope-final.txt` → `ok:true, phase:executor-run, serverAcceptanceFiles:4, structuralCollected:34`，实跑 **34 passed, 6 subtests passed**（EXIT=0）。
  - `unit`：`node .claude/scripts/unit-run-check.mjs --phase=final --scope-file <同上>` → `ok:true, phase:executor-run, structuralCollected:125`，实跑 **125 passed, 6 subtests passed**（EXIT=0）。
- **冻结验收件直跑（红→绿终态）**：`cd server && PYTHONIOENCODING=utf-8 python -m pytest -q --import-mode=importlib -m "not live" acceptance/test_261013_qq_schedule_write_closure.acceptance.py` → **11 passed**（红基线 = 9 failed/2 passed，缺件设计态红全数转绿；2 守卫齿 must-stay-green 保持）。
- **回归观察面**：`server/tests/` 全档 → **91 passed**（exitCriteria「91 passed 基线保持」满足）；`acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py`（test_mnf_a1）→ **3 passed, 4 subtests passed**（跨包回归转绿：manifest provider note 字面残留 `openclaw/default` 清零）。
- **planner 独立复跑（交叉印证）**：planner 2026-10-13 独立复跑确认（14 passed + 4 subtests 面 + 91 passed）——与 integrator 复跑终态一致。
- **影响面核对记账（Step 9b advisory，detect_changes 实跑 2026-10-13）**：Pod 专属图索引 `C-AI-KidPal-pod-m2`，base=main，merge_base=af1bc3b，inbound depth=2，含工作树未 commit diff：**57 seed / 10 impacted**——impacted 全落 `server/gateway`（front_app.py 六函数：`run_schedule_poll_once` / `reminder_poll_loop` / `_expand_schedule_intent` / `_push_schedule_connect` / `build_display_tool_calls` / `ws_gateway`）+ `server/acceptance`（poll 入口/school_start 消费面），**零跨子系统越界**。无降级（per-Pod 图索引用常，三分法正常圈定）。test-scope 清单落盘 = `delivery/taskpackages/261013-qq-schedule-write-closure/test-scope-final.txt`。
- **变异取证**：`mutationRequired=false`（additive 闭环，红基线即缺件设计态红承载判别力；261010 prompt-v9 同类先例）——见下方 disclosure「mutationSkipReason 补正」。
- **机器产线**：本包 raw intake 形状未跑 `verify.mjs` round 产线（无 `verifier-evidence/rounds/r<N>.json`）——`verdict.json` / `verification.json`（投影·advisory）因此不在场（closeout-check advisory-only、non-blocking；不手工伪造，SSOT 规定该两件由脚本 fs-write、非 agent 落盘）。bearing fact 以本节 integrator final 圈定复跑（acceptance 34 / unit 125）+ 八闸实跑结果承载，供 planner/supervisor 判读。

## ambient 清单（3 条历史红，非本包领地）

HEAD 基线盘点（本包零权限面、不越界修）：

| 齿 | 文件 | 红因 | 归属 |
|---|---|---|---|
| `test_a8_v7_prompt_immutable` | `server/acceptance/test_m4k3_record_session_live.acceptance.py` | prompt 文件哈希期望未随 260925/261010 批 bump 更新（`1acb2b5…` ≠ 钉 `0b051e1…`） | 260925/261010 提示词批 |
| `test_pn_a3_v8_prompt_file_immutable` | `server/acceptance/test_prompt_v7_round_routing.acceptance.py` | 同上（v8 哈希钉陈旧） | 同上 |
| `test_rf_a3_v9_prompt_file_immutable` | `server/acceptance/test_prompt_v7_round_routing.acceptance.py` | 同上（v9 哈希钉陈旧） | 同上 |

- 三条均为**提示词哈希不可变齿**（`*.acceptance.py` 冻结面，本包唯一写权不含验收面），红因 = 历史 bump 未随更的期望陈旧，与本包改动（server/deploy skill + front 告警写）零关联。**scope-file 圈定时明确排除两者的两文件**（`test_m4k3_record_session_live.acceptance.py` 与 `test_prompt_v7_round_routing.acceptance.py` 不在 `test-scope-final.txt` 中），故 final 圈定复跑不受其染红——此即「范围 = 影响面圈定清单」的意义（非本包领地不牵入）。
- 建议（follow-up，非本包义务）：三条 ambient 红登记 CRQ 或归 260925/261010 批后手修（见 Residual Risk）。

## 未覆盖面申报（对照 acceptance[] 逐条）

- **QSW-A1 / A2 / A3 / A4：已覆盖**（offline 面）——A1/A4 = skill 文本护栏齿（机制级，钉 B1 表头逐字范本 / 日期无星期后缀纪律 / 写后机检机器面 / ack-QQ 语义）；A2 = manifest 两 skill 条目 + health cron 条目字段完整 + src 在仓内存在（stdlib 文本/路径断言，yaml 结构合法性归 ECS 侧 verify-deploy.sh）；A3 = front 告警写（行为级：脏输入真跑 → 告警件在场且字段可判 + 合法文件不写 + poll 入口触发 + 写失败静默吞）。共 11 齿全绿。
- **QSW-A5（真机轮 live-manual structural 豁免，不计入 offline runner 通过面）——状态 = 待 post-deploy、human 触发，本包停门项**，two-part 清单：
  1. 家长 QQ 录入一条脏格式课表 → main agent 按 skill 纠格式/引导重录，写后机检通过 + 重算落 marker，front 日志 `schedule_push trigger=immediate`、设备「接下来」≈1 分钟内更新；
  2. ECS 手改注入脏行后 front 落 `schedule-parse-error.json`，下一跳 health cron 后家长 QQ 收到转告。
  **依赖**：ECS bootstrap（`server/deploy/ecs-bootstrap.md`，261013-gov-review-deploy-single-source-loop 产物）+ openclaw cron 注册（runbook §8.3c）+ **QQ 路由目标配置（家长 openid，TECH-261012 证据 5 既有缺口）**——本包 runbook/skill 两层如实写、不虚诺，live 配置归 human post-deploy。
- exitCriteria 其余各条：①QSW-A1~A4 冻结齿全绿已满足；②server/tests 91 passed 保持已满足；③test_mnf_a1 转绿已满足；④manifest 8→10 entries + 3→ 条 cron_jobs（既有条目零破坏，verify-deploy.sh 语义兼容归 ECS 侧）；⑤docs-sync 三处（TECH append + README 两行 + runbook §8.3c）已落；⑥acceptanceTestsFile 归位 `server/acceptance/`已满足；⑦QSW-A5 真机轮留痕 = 殿后申报（上文）。

## 已落 vs 未落项

- **已落（6/7 changeManifest 件 + 冻结件）**：两 SKILL.md、manifest.yaml、skills README、DEPLOY-RUNBOOK §8.3c、front_app.py 告警写 —— builder 落地，integrator 核实到位；第 7 件 TECH append —— integrator docs-sync 相位落地（builder scope-map 写槽不含 `docs/crq/**`）。
- **未落（殿后）**：QSW-A5 真机轮（post-deploy，非 offline 通过面）+ ④ 投递依赖的 QQ 路由目标配置（human，既有缺口）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md / kidpal-schedule-health/SKILL.md / server/gateway/front/front_app.py | 本包改动面 `git diff` grep `minimal:` 零命中——零新增债务；改动文件内既有标记（如有）原样在世、不属本批 | n/a | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | closeout 制品面（taskpackage 字段对齐） | taskpackage `mutationRequired=false` 原仅带 `mutationEvidence.reason`（内容在场），缺顶层 `mutationSkipReason`/`mutationReason` 键 → `mutation-check` 闸红（"mutationRequired=false must include mutationSkipReason/mutationReason"）。integrator closeout 相位机械补齐顶层键（内容与 `mutationEvidence.reason` 同源、零语义变，两份 taskpackage 副本同改保一致） | changed-code hunk（`taskpackage.json` mutationRequired 邻键）+ gate 红输出 heuristic（`.claude/scripts/mutation-check.mjs:129-135` 键名约定）；非契约/spec 源，故非 hard-violation | 无（已补齐、闸转绿；taskpackage 作者侧未来写 mutationRequired=false 时预置顶层 reason 键可免此摩擦） |
| NIT | judgement-call | ambient 三红治理 | 本包 scope-file 排除的三条提示词哈希不可变红（v7/v8/v9，见 ambient 清单）未由本包登记 CRQ——本包零权限面（验收面只读）、属历史 bump 陈旧 | ambient 清单表 + `test-scope-final.txt` 排除注 + 闸实跑输出 heuristic | 无（申报在案；建议归 260925/261010 批后手或 CRQ 登记，见 Residual Risk） |
| NIT | judgement-call | docs/crq 生成件 | `docs/crq/tech/INDEX.md` 摘要行残留三行 union-merge 冲突（既有，非本包引入）——跑 `crq-render.mjs` 归一为单行，表体零变动 | changed-code hunk（INDEX 摘要 diff）+ crq-render merge=union 纪律 heuristic | 无（已归一；merge=union 后各分支重渲染归后续） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。本轮 findings 面清洁：无任何 hard-violation 级 finding；契约/spec 类 finding 的起引与台账核销约定同 pre-merge-escalation-gate 的字面集定义（B-4 写作侧对齐），本轮 none（本包零契约/spec 改动）。

## Five-Axis Review

- **安全**：告警写 helper 自身 try/except 全吞（至多 debug 日志，不炸轮询循环）——`_load_schedule_model` 契约「返回 None」保持（守卫齿 `test_a3_alert_write_failure_is_swallowed` 实证）；`os.makedirs(..., exist_ok=True)` + `os.replace` 原子落；`detail` 截断 120 字（与现日志同源）；零新增出网、零 secret、零权限面变更。skill 文本为 LLM 消费护栏、非执行代码，无注入面。零新增攻击面。
- **契约**：零触（contract-lint PASS）——skill 只读消费 B1 §2.2/§2.3 既有格式、front 告警件为内部运维面（非契约四大面）；无影子实现绕 contracts；`schedule.py` 冻结纯解析器零改动。
- **边界与依赖卫生**：零新增依赖（纯 stdlib + 既有 front/OpenClaw 面）；无跨层调用、无循环依赖、无重复实现（front 单源 parser 复用 `/opt/kidpal/front/gateway/schedule.py`，不另建第二套解析）；detect_changes 10 impacted 零越界佐证；manifest 条目字段与既有 5 条 skill 完全同款（同批原子落，verify: md5 src 存在性齿 `test_a2_referenced_skill_srcs_exist_on_disk` 实证）。
- **可运维性**：告警件与 ack 件均有明确单写者（front 覆写告警件 / health skill 单写 ack 件），可观测面清晰（告警件 + `journalctl schedule_md_err`）；health cron 频率恒 15m（verify-deploy.sh 硬核验 `every_min == 15`）；runbook 两层如实写路由降级、不虚诺；回滚 = 本 commit revert（见 Residual Risk）。
- **资源与功耗**：告警写为每分钟至多一次小文件写（仅解析失败期间）+ 原子 rename，零可测运行时开销；changeId/correlationId 未涉及新日志路径（沿用既有 `schedule_md_err` 行）。

## Residual Risk

- **Known limitation**: ① QSW-A5 真机轮待 post-deploy（offline 面闭环、live 效果未闭环——skill 护栏只证「护栏已写对」不证「main agent 读到后按此改写」，等价假设显式化于冻结件头注）；② ④ 的 QQ 转告投递依赖家长 openid 路由目标配置（TECH-261012 证据 5 既有缺口，路由未配时 health 回合仍跑、告警件与 journal 可观测，主动告知 fail-closed 投不出）。
- **Follow-up recommended**: 三条 ambient 提示词红（v7/v8/v9）建议登记 CRQ 或归 260925/261010 批后手修（本包零权限面，如实申报不越界）；taskpackage 作者侧未来 `mutationRequired=false` 时预置顶层 `mutationSkipReason` 键可免 closeout 补正摩擦。
- **Human decision needed**: QSW-A5 真机轮 = post-deploy human 触发（依赖 ECS bootstrap + cron 注册 + QQ 路由目标配置）；合并门裁断归 supervisor（低/中风险机械闸放行 / 高/critical 升 human）。taskpackage **未启用 `decisionCoverage` 字段**——`landsIn: dropped && reason: risk-accepted` 条目 = **零**，Residual Risk 节按模板申报零条目。

## disclosure（如实申报）

- **input-brief item 7 归属写错**：`input-brief-builder.md` 第 7 件把 TECH append 列为 builder 交付物，但 builder scope-map（`agents.builder.deny` 含 `**/acceptance/**` 且 allow 不含 `docs/crq/**`）机械拦截——`docs/crq/**` = integrator 写槽。builder 未落该件、如实回退，integrator 于 docs-sync 相位按证据实况校对后 append（内容 = ①④ 落地说明 + 部署面联动 + QSW-A5 殿后依赖清单 + 范围证据）。**根因 = brief 生成侧未对账 scope-map 写槽**，建议 brief 生成侧（planner 派单）对越界写槽项预标 owner。
- **TECH append 内容归属**：builder 起草要点、integrator 按实况校对落笔（append-only、零改既有行；README 两行与 runbook §8.3c 为 builder 已落、integrator 核实到位）。
- **`mutationSkipReason` 补正**：见 Findings 首行——integrator closeout 相位机械补齐（内容零变）。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——单包无对端冻结件，floor = 本包冻结验收件 `server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py`（final 圈定复跑承载）。detect_changes 记账见 Verification Evidence 末行（57 seed / 10 impacted 零越界，无降级）。全量回归 = 人工触发位，本轮为 Lite 包按派单口径收口。

## 可回滚路径

- **仓库侧**：本变更经 feature 分支 `dev/261013-qq-schedule-write-closure` 单 commit 收口，回滚 = `git revert <commit>`（单包原子：additive skill 文本 + front additive helper + manifest/README/runbook 追加 + docs append——无删数据、无迁移、无既有行为改写，revert 干净）。
- **live 侧**：本变更**尚未 deploy**（post-deploy 归 human）——若已 deploy 需回退：移除 `/root/.openclaw/workspace/skills/kidpal-schedule-{write,health}/` + 删 cron `kidpal-schedule-health`（`openclaw cron rm`）+ 下次 `deploy.sh` 按 reverted manifest 覆盖；front 告警件 `alerts/schedule-parse-error.json` 为运行态输出（front 回退版本即不再写，件本身无害可留）、ack 件 `alerts/schedule-parse-health.json` 同。
- **回滚无残留副作用**：告警件与 ack 件均为运行态输出（不进 manifest），revert 后无孤儿状态。
