# Review Packet — 261022-m5-k3-honcho-memory-production

- **rigor**: full · **riskTier**: high · **workstream**: device-memory-overhaul（P3）· **mergeOrder**: 53
- **test-scope 清单落盘路径**（final verifier fact r3 results 元数据必含行）：`delivery/taskpackages/261022-m5-k3-honcho-memory-production/test-scope-final.txt`
- **圈定路径**：per-Pod 专属图索引可用（project `C-AI-KidPal-pod-m2`）→ 正常圈定，无降级。

## Change Size

- Files changed: 7 tracked modified（taskpackage 簿记 4 + manifest.yaml + AGENTS.md + verify-deploy.sh）+ 16 新增文件（两验收件、`server/deploy/honcho/**` 5 件、`server/deploy/261022-openclaw-honcho-plugin/**` 4 件、taskpackage 簿记 5 件）+ 本包 closeout 制品（docs-delta / review-packet / test-scope-final）
- Net diff: tracked 488 insertions / 153 deletions；新增实现+验收正文约 2,300 行（验收两件 1,741 行 + 部署工件 567 行）
- Scope: server/deploy（honcho 栈、openclaw patch 工作台、manifest、verify-deploy、AGENTS.md v12）；`server/gateway/front/front_app.py` 零 diff（K3-E2 front 守卫 = P2 后现状即合规 must-stay-green）；`firmware/**` 零触碰；契约零 diff
- Split recommendation: 无需拆分——单子系统垂直切片，验收 E1–E7 与工件同批内聚

## Verification Evidence

- **final bearing（round 3，invocation `e59ae7a3-9317-4325-85e6-4b556617a30d`）**：`verifier-evidence/rounds/r3.json` `ok:true`，10 闸全 PASS（taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance），`verifier-evidence-check.mjs` 自证 PASS（exit 0，factOk=true）。
- **Acceptance（两口径）**：scope 圈定口径（final r3 acceptance 闸，按 test-scope-final.txt 11 件）= **140 passed / 0 failed / 6 deselected**（deselected = 本包两件 live 标记 + 既有 live 标记，属预期）；全量口径（builder 实跑 + integrator 复跑 `pytest -m "not live" acceptance`）= **313 passed / 0 failed / 6 deselected / 33 subtests passed**（= 既有 292 + 新件 21 条全绿，与红基线转绿目标逐字一致）。
- **Unit（两口径）**：final r3 unit 闸 = 181 passed / 0 failed / 6 deselected（13 文件收集面）；`server/tests` 单独复跑 = 115 passed / 19 subtests（与 builder 报告一致，零新增红）。
- **Security gate**：PASS（securityAxes 空 = 无未处置项）；securityEvidence 五面（loopback / 出网两条记账 / 凭据名字引用 / 删除备份语义 / 降级护栏）由 E1/E4/E5/E6 断言承载。
- **Mutation check**：skip（`mutationSkipReason` 在案：证据面而非分支逻辑面，ROI 不成立）。
- **Docs sync**：`docsSyncRequired=false`（架构/需求面已由 261022 两面 landing 落稿）；声明性注记见 `docs-delta.md`；spec 面漂移检出 = none。
- **契约**：`docs/contracts/**` 零 diff，contract-lint PASS（1 条既有 broken-link warning，非本包引入）。

## detect_changes 实际爆炸半径（advisory 记账，Step 9b）

- project `C-AI-KidPal-pod-m2`，inbound depth 3，base=main merge_base `9f58301a`：changed_files 16 项（含未跟踪新件），seed_symbols 100，**impacted_total = 56**（truncated=false）。
- 关键命中：`server.gateway.front.front_app._openclaw_chat_once`（hop 1——K3-E2 门载体 seam，结构零 diff、被 blast radius 正确捕捉）、front_app 整理/提醒族（_organize_and_archive / run_alert_poll_once / reminder_poll_loop，hop 2–3）、`server/tests/test_reminder_units.py`（5 符号）、`server/acceptance` 映射 5 符号（record_front_polish / schedule_grid_restore / archive schedule_reminder）。以上全部落入 test-scope-final.txt。
- `firmware/scripts` 39 符号为验收测试文件二进制读取符号的 DATA_FLOWS 映射噪声（Image_Converter / spiffs / p3_tools 家族，本包零固件 diff、零触达）；记为 advisory 噪声，不扩 scope（firmware 侧由 firmware-build 闸 + 零 diff 担保）。

## Demo 对账行（workstream 硬要求 ①）

对照 `delivery/workstreams/device-memory-overhaul/roadmap.md` §「261022-m5-k3-honcho-memory-production」效果段与 demo 证据形态，逐项：

| roadmap 效果/demo 形态 | host runner 承载（本包 r3 全绿断言） | 结论 |
|---|---|---|
| 「设备流程的整理/引导回合不混进画像」隔离 marker 探针结构面 | `test_k3_e2_gate_carrier_anchors` + `test_k3_e2_front_oneshot_payload_no_user`（payload 零 user 结构证）+ 插件 carrier `honcho-plugin.js` marker 门控锚（探针门控 + ARQ-260907 同族报警锚） | **结构面一致**；marker DB+日志双证 = live 面（`test_live_k3_e2_marker_probes_db_log_double_evidence`，deselected，K3-E7 live 轮） |
| 声明式 patch 幂等/回滚 | `test_k3_e6_apply_idempotent_and_preserves_p1_state` / `test_k3_e6_apply_drift_guard_exit3` / `test_k3_e6_apply_timestamped_backup` / `test_k3_e6_rollback_dry_run_and_restore` / `test_k3_e6_no_openclaw_json_in_repo_and_no_secret` | **一致**（幂等 SKIP、drift_guard exit 3、时间戳备份、dry-run 回滚全绿） |
| 降级路径（故障只是不记得，不影响聊天） | `test_k3_e5_degradation_guards` + `test_k3_e4_e5_e7_declarations_in_header`（bounded 失败护栏 + 不进用户可见兜底） | **结构面一致**；真栈降级实证 = live/部署轮 |
| 「隔天新会话答得上来」跨会话回忆 | host 面无自动断言（结构性不证 live 生效，验收件头注显式申报） | **未跑，如实申报**：K3-E7 live 项 ①（新 WS 新会话回忆对照 + 全新 user id 负对照）挂 `awaiting-human:live-deploy` |
| 「家长隐私不被记住、不被套问」seeded 负向探针 | `test_k3_e3_agents_md_v12_memory_section`（守则文本在场且极性正确，措辞级 guard-class） | **未跑 live 半**：seeded 负向探针集 100% 零命中（`test_live_k3_e3_seeded_negative_probe_set_zero_hit`，deselected）挂 live 轮；守则措辞只证文本、不证模型服从 |

**一致性结论**：roadmap 效果段的每个机制半均有承载断言且 final r3 全绿；live 半（真机跨会话回忆 / seeded 负向套问 / marker DB+日志双证 / 2C-4G 资源读数）按 K3-E7 structural 申报挂 human 部署轮，**本包收口不含 live demo，如实申报待跑**。

## 未覆盖面申报（workstream 硬要求 ②：对照 acceptance + 契约条目 + roadmap §3 效果段）

- E1「数据本机不出网」运行时无出网：host 面只有负锚词表扫描（K3_FORBIDDEN_HOSTS）= 代理，词表外新出网形态不可见；运行时出网观测 = K3-E7 live + 部署轮 verify-deploy 端口/防火墙核验。
- E3 模型服从面：措辞级守卫只证守则文本在场且极性正确，不证模型真的记孩子事实/拒家长隐私——由 live seeded 负向探针集承载。
- E4 删除语义机制半：删除 = 运维面手工属无机制面设计事实，host 只负锚「未引入自动化删除/备份」；真实删除演练与删除后画像为空验证 = live/运维轮（NFR-PRIV-1 第二半）。
- K3-E7 live 全项（跨会话回忆对照 / seeded 负向 / marker 双证 / prep 税与内存读数回填 TECH-261022-openclaw-prep-tax-memory-pressure / §6.7 抽取策略实测成形）：零 host 自动断言，挂 awaiting-human:live-deploy。
- 契约条目（interface-contract.md §四）：零 diff、零新增契约断言轴，覆盖差集 = none。
- 设备端（crossEnd）：固件零 diff、端→网关消息零触碰，覆盖差集 = none。
- 除上述显式申报外，acceptance E1–E6 断言轴对契约与 roadmap 效果段的覆盖差集 = **none**。

## Simplification Debt

本包新增/改动文件内零新增 `minimal:` 标记（全仓 grep 命中均为存量文件，不在 changeManifest 内）。台账 = 空。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Operability | `verify-deploy.sh` honcho 段健康门用 `docker compose`（v2 plugin 子命令）；若 ECS 仅有 docker-compose v1 独立二进制则该段恒判 DRIFT | changed hunk `server/deploy/verify-deploy.sh` 新增段 + heuristic：部署环境工具链假设应显式（manifest 同批 compose 用法一致，风险低） | 部署轮首次跑 verify-deploy 时确认 ECS docker compose v2 在位；如仅 v1，随部署轮顺手改命令形态，不阻塞合并门 |

无 BLOCKING、无 SHOULD。

## Five-Axis Review

1. **Correctness**：E1 五容器 compose（api 127.0.0.1:18100、全 published 端口显式 loopback、恰一 pgvector service 带本机持久卷、容器间 expose）与 arch §5.3.2/§5.3.3 对齐；E2 资格门读写同门（user 缺席即非资格，front 一次性回合 payload 零 user 结构证 + P2 正证不复钉）；E3 v12 两边界（孩子事实必须记 / 家长隐私不记）+ P1 段逐字锚零回改 + 动态块负锚；E4 删除/重建语义负锚（零 pg_dump、零远端备份端点、零自助删除面）；E6 声明工件 schema、双声明式 patch 组合（P1 tools allowlist 保全、drift_guard 现状锚 = P1 已应用态）。边界条件（缺 .env 拒跑、缺 config/非法 JSON exit 2、drift exit 3 不写不备份）齐。
2. **Test quality**：验收断言真实工件（compose 解析、manifest 条目逐条核对、apply 脚本行为级实跑含幂等/备份/回滚文件内容、负锚扫描），DB/事件/审计态归 live 齿并显式申报；错误路径（exit 2/3）有断言；红基线 17 红全落新件设计态红、既有套件零新增红已由两口径复跑证实。
3. **Contract/boundary safety**：契约零 diff；display E1/B1/提醒/私有消息帧零触碰（crossEnd 免除在案）；不修历史 prompt 版本、不建 v12 双件、不建 prompts 平行目录（OQ-8）；peer 键不另造（`agent:kidpal:openai-user:` 前缀唯一形态）；A4 backfill 子树零创建。
4. **Operability/security**：凭据全部名字引用（`.env` 模板空值、真实值仅 ECS）；openclaw.json 不入仓 + sk- 负锚；apply 有时间戳备份 + 一步回滚 + drift_guard；verify-deploy loopback/健康双核验入每日 drift timer；降级 bounded 失败不进用户可见兜底、不静默重试风暴；secret 面无新增暴露。
5. **Maintainability/simplicity**：改动局部（deploy 面声明工件 + 两验收件），无过早抽象（apply/rollback 与 261017/261022 P1 同形制复用，非新造轮子）；AGENTS.md 单向追加零回改守则文本可 diff；无重复实现（manifest/verify 恒 manifest 驱动零特例）。deletion test：移除任一工件（如 rollback 脚本）复杂度会泄漏到运维手操，属深模组，留。

## Residual Risk

- decisionCoverage `landsIn: dropped && reason: risk-accepted` 条目 = **none**（8 条决策全部 landsIn exitCriteria 或后续包 K4）。
- Known limitation：live 面（跨会话回忆 / 隐私负向量化 / marker 双证 / 资源读数）未跑——structural 申报挂 `awaiting-human:live-deploy`；E1 运行时出网为负锚代理（见未覆盖面申报）。
- Follow-up recommended：① human 部署轮执行 K3-E7 live 项并回填 TECH-261022-openclaw-prep-tax-memory-pressure；② §6.7 抽取策略实测成形后如需改 spec 走 /arch-change 逃生门；③ 首次 verify-deploy 确认 docker compose v2 在位（见 Findings NIT）。
- Human decision needed: 无（本包收口不引入需人类即时裁决事项；live 轮本身即 human 部署动作）。
