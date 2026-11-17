# pending-human-ruling · 261116-channel-resident-core

> 非交互收尾的 fail-closed 载体（planner 收尾相位，2026-11-18）。本包**修复轮离线面已全部完成并 commit**，停在合并门；以下事项需 human 裁决/执行，未闭合前不推进、不合并、不结项。

## 当前状态（已完成、可复核）

- 真机轮 A 三缺陷（D1/D2/D3）修复落地：`application.cc`（D3 空素材 producer）+ `front_app.py`（D1 无条件注入 + 审计行；D2 discard 前释放 + 迟到帧门）。
- 冻结验收 20 条（原 15 + 修复轮 5：SCRF-B7/B8/B9、SCR-A8、SCRF-B10；RDG-A4 显式重锚）；变异证据 19 组（`mutation-evidence/round-a-fix.md`），含 4 条如实观察项。
- verifier 机械证据：`verifier-evidence/rounds/r4.json`（scoped 8 闸 ok）、`r5.json`（final 10 闸 ok，含 `VERIFY_FW_BUILD_REQUIRED=1` 真编译）；`verifier-evidence-check` exit 0。r3 为 `ok:false` 的**簿记失配**事实（已保留占序，非实现缺陷）。
- 实跑：host `307 Tests / 3 Failures`（3 红 = 上游存量 `p1_auxbar_style`，零新增红）；server `486 passed / 0 failed`。
- closeout：`docs-delta.md`、`review-packet.md`、`pod-retro.md` 齐；`closeout-check.mjs` exit 0；提交栈（本地 `dev/pool-channel`，**未 push、未 merge**）：`b7b2c423` → `363a80d3` → `36508e47`。
- 机械 supervisor verdict = **ESCALATE**（declared riskTier high）；`supervisor-gate-check.mjs` exit 0（本轮因 r3 失败轮留有 verify-log，G4 未触发——**不代表 G4 已修**，见待裁三）。

## 待裁一：真机轮 B 复测（本包出口，human 持板）

- 轮 A = **FAILED 诊断轮**（三缺陷取证见 `device-round-a-evidence.md` + `device-evidence/round-a/*` + 三份 SHA-256）。
- 轮 B 判据（review-packet「设备轮证据」节，含 B-D1/B-D2/B-D3 + A5/A6/A7 回归）：
  - B-D1：故事页起会话每会话进抑制；普通问句只回显、零 TTS、设备不 `idle -> speaking`。
  - B-D2：录音中 BOOT 丢弃（尾音在途）→ 设备回列表并停留 Idle；引擎无该回合 `大模型收到用户消息` / `发送第一段语音`；front 出现 abort-free 的 active=false 先于 listen stop 次序证据。
  - B-D3：故事页起会话后完全不说话，60–70s 内设备**自行**回列表；front 无 story_list/story_text、STORIES_DIR 零新增；**≥10s 后**普通问答可应答（≤10s 首次话语允许被抑制、再按即恢复 = 已冻结 accepted residual）。
- 轮 B 通过后由 human 交付 `sourceCommit` + `sha256(xiaozhi.bin)`，planner/integrator 才能生成 `device-round-evidence.md`（含逐 acceptanceRef PASS observation）。**当前未生成、不伪造 PASS**。

## 待裁二：合并门 ESCALATE + riskTier high

- 前置升级闸 BLOCK 残项：`device-round-evidence-missing`、`riskTier=high（supervisor 升级）`、`supervisor verdict=ESCALATE`、`parallel-overlap-tier3`（见待裁三）。
- human 明确批准合并后：planner 落 `human-decision`（`payload.decision="approve-escalated-merge"`）→ `merge-driver --pod <changeId> --human-approval-ref <eventId>`。**人批不豁免真机证据、不豁免 tier3**。

## 待裁三：两处工具/治理缺口（阻断合并动作）

1. **G4 verify-logs 结构性缺口**（上轮已报，见 `gate-tooling-gap-g4.md`）：全绿 Pod 无 verify-logs → 监督闸恒红。本包本轮因 r3 失败轮留有 log 而暂未触发，**不等于已修**。主线程修 `verify.mjs` 的 `writeGateLog` 位置后需再跑一轮自然产 log。
2. **`parallel-overlap-tier3` 对生成件误判**（本轮新发现，见 `gate-tooling-gap-overlap-index.md`）：`delivery/workstreams/INDEX.md` 实撞仅「最后更新」日期行（生成件），按 §10d 应判 ②；architect 消解评审（`a7c1ad6c73ea2da9f`）判 ②，建议 `.gitattributes` 加 `merge=union`（+ 同批裁 `index.jsonl`）。**该条不在 `escalateExempt` 内，人批豁免不了**；合并前必须让冲突消失（union 条目 或 主线程 rebase/merge 消解 + 重跑 `planning-index.mjs`）。

## 待裁四：spec 漂移（主线程/需求面，Pod 零写）

- D3 收窄 requirements §2.1「记录会话不适用 60s 会话超时」的逐字冻结（空素材记录会话由设备侧 60s 收尾 = 释放不归档；AC5 只覆盖有素材场景）。`docsSyncRequired=false` 与实际影响不一致。
- 建议按需求面轻量注记处理（§2.1 记录会话段对账句 / §7 开放问题 7 追加），由 architect 在需求面落；**本 Pod 对 `docs/**` 零 diff**。

## 非裁项（已按纪律处置，仅备案）

- 两条 debt（`TECH-261114` / `TECH-261012`）维持 `deferred`（结项凭据 = 轮 B 真机）；轮 B 通过后由主线程改 `closed`/`closedBy` 并 `debt-render.mjs` 归位。
- `docs/requirements.md:48` 的 60s 对账 ⚠️ 注记：归 workstream 留根 1（主线程路由）。
- 变异观察项 4 条（超串改名/注释分号不敏感、B10 md 断言判别力、start_organizing 观测面）已在 `mutation-evidence/round-a-fix.md` 与 review-packet 如实登记，不阻塞。
- workstream `261115-session-channel-resident` 的 progress/INDEX 与 spine 回填归主线程（Pod 零写 `delivery/intake/**`）。