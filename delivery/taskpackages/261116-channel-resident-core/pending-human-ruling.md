# pending-human-ruling · 261116-channel-resident-core

> 非交互收尾的 fail-closed 载体（planner 收尾相位，2026-11-18）。本包**修复轮离线面已全部完成并 commit**，停在合并门；以下事项需 human 裁决/执行，未闭合前不推进、不合并、不结项。

## 当前状态（已完成、可复核）

- **轮 B 修复轮离线面已全部完成并 commit**（2026-11-18）：新缝 `firmware/main/session_voice_watch.{c,h}`（判据本体，host 可编）+ `application.cc` 七处接线 + 两侧 CMakeLists 登记 + builder 单测；front 与引擎本轮**零 diff**。
- 判据改造：有效语音证据 = 连续电平落在可信窗 `[3s, 100s]` **或** 引擎 stt 新鲜（`asr_age_s < 25s`）；纯静默恰 60 秒收尾，电平卡死最迟 160 秒有界收尾（`suspect=1`）；A7「连续讲 >90 秒不误收」由可信窗 100 > 90 无条件保证。
- D3 素材判据换源：`s_story_rec_entered` → seam 的 `asr_seen`（与 front 累积器同源）。
- 白盒复核补丁：引导播报 TTS 造成 Listening→Speaking→Listening 时，全清复位会清掉素材证据 → 改为 `reset_soft`（清六保一）+ 按迁移来源分派。
- 可观测性：`listening_diag` 十字段低频行（`src/state/vad/run/quiet/asr/ticks/rec_ctx/rec_mat/suspect`），周期 10 秒 + 跳变、最小间隔 3 秒、非聆听态零输出；两条 producer 审计行追加 `quiet= / asr= / suspect= / src=`。
- 冻结验收 27 条（原 20 + 新增 SCR-A9..A15；SCR-A5/A8 显式重锚并退役一条）；变异证据 13 组（`mutation-evidence/round-b-fix.md`，含 kill-shot「删可信窗上界」、A7 承重「可信窗 100→30」、电平门回潮负锚、软复位退化、条件分派删除）。
- verifier 机械证据：`verifier-evidence/rounds/r6.json`（scoped 八闸 ok）、`r7.json`（final 十闸 ok，含影响面圈定 + 真编译）；`verifier-evidence-check` 两轮均 exit 0。
- 实跑：host `321 Tests / 3 Failures`（3 红 = 上游存量 `p1_auxbar_style`，零新增红）；server `486 passed / 0 failed`。
- closeout：`docs-delta.md`、`review-packet.md`、`pod-retro.md` 齐；`closeout-check.mjs` exit 0；提交栈（本地 `dev/pool-channel`，**未 push、未 merge**）：`f368926c` → `818ae8bb` → `f0b9ace5`。
- 机械 supervisor verdict = **ESCALATE**（declared riskTier high，证据完整）；`supervisor-gate-check.mjs` exit 0（本包因既有失败轮留有 verify-log，G4 未触发——**不代表 G4 已修**，见待裁三）。
- **未合并、未 push**：本包出口未闭合（`device-round-evidence.md` 未生成，轮 C 未跑）。**不得合并**。

## 待裁一：真机轮 B（2026-11-18 已跑，D3 与 A6 失败 → 进入 B 修复轮）

- 轮 A = **FAILED 诊断轮**（三缺陷取证见 `device-round-a-evidence.md` + `device-evidence/round-a/*` + 三份 SHA-256）。
- 轮 B = **部分通过 + 两项失败**（2026-11-18，human 持板；设备日志摘录与三源判定见 `device-evidence/round-b/kplog-excerpt.txt`）：
  - D1 通过、D2 通过、A5 归档通过。
  - **D3 失败**：故事页起会话、全程不说话，07:15:03 listen start → 07:17:43 手动丢弃（2m41s）未自动结束，设备日志无 `story_empty_session_timeout`。
  - **A6 回归失败**：普通会话 07:18:23 → 07:21:36（3m13s）未自动关麦，无 `session_idle_timeout`；A6 producer 本轮代码一字未动。
  - **planner 只读定位（新证据）**：D3 的根因不是电平门——每次故事会话起步 0.25–0.7 秒内必出一条 `story_vad_recording_enter`（VAD 在无人说话时让出一记 speech 沿），它把 D3 的「从未收到素材」判据 `s_story_rec_entered` 置真，使该会话对 D3 producer 永久不可达；同一沿还把页面无端跳到录音页。A6 窗内零状态迁移（计时未被状态迁移清零），但设备不打印 VAD 电平，M1（电平恒真）/ M2（起音沿周期性清零计时）无法在日志里区分——这正是 human 要求补低频 listening 诊断行的理由。
- **human 2026-11-18 已裁「按 B 修」**（pod-events `human-correction` 事件）：判据改造 + D3 复用同一新判据 + 可观测性必办 + 新冻结断言与变异证据；契约面与引擎代码零改动。修复轮在本包边界内推进，离线转绿后需**轮 C 真机复测**（至少 A6 / A7 / D3 / A5 回归）才能闭合本包出口。

### 轮 C 真机复测清单（human 持板；写死在 review-packet「设备轮证据」节）

1. **A6**：普通会话（home 页发起）静置 60 秒 → 设备自动关麦；审计行 `session_idle_timeout` 在场。
2. **A7**：连续讲 >90 秒 → 不被误收（会话保持聆听）。
3. **D3**：故事页起会话、全程不说话，静置 60 秒 → 设备自动回列表、**不归档**；front 无 `story_list` / 无 `story_text` / STORIES_DIR 零新增；随后普通问答可应答（≥10 秒后，或 10 秒内首次话语被抑制、再按一次即恢复 = 已冻结 accepted residual）。
4. **A5 回归**：有素材的故事会话全链（讲完 → 整理 → 归档 → 盖章）正常。
5. **诊断采集（承重）**：每窗读 `listening_diag` 的 `suspect=` / `quiet=` / `asr=` / `run=` / `rec_ctx=` / `rec_mat=` / `ticks=`；C-1 收尾时刻 `suspect` 值（判本轮是否命中电平卡死）、C-2 A7 窗 `asr=` 列最大值（标定 `ASR_FRESH_S=25`）、C-3 home 会话 `rec_ctx=` 恒 0（M4 排查）、C-4 静置窗 `run=` 是否反复归零（M2 排查）、C-5 日志落盘增量对账、C-6 D3 窗 `rec_ctx=1 && asr_seen=0 && quiet` 到阈值。
6. **引导播报回归（白盒补丁的真机承载）**：记录会话中触发一次卡壳引导播报（Listening→Speaking→Listening），随后静置 60 秒 → **不得**被 D3 判成空素材释放；已讲内容仍能归档。
7. 轮 C 通过后由 human 交付 `sourceCommit` + `sha256(xiaozhi.bin)`，planner/integrator 才能生成 `device-round-evidence.md`（含逐 acceptanceRef PASS 目击）。**未生成、未跑即不合并、不伪造 PASS。**

## 待裁二：合并门 ESCALATE + riskTier high

- 前置升级闸 BLOCK 残项：`device-round-evidence-missing`、`riskTier=high（supervisor 升级）`、`supervisor verdict=ESCALATE`、`parallel-overlap-tier3`（见待裁三）。
- human 明确批准合并后：planner 落 `human-decision`（`payload.decision="approve-escalated-merge"`）→ `merge-driver --pod <changeId> --human-approval-ref <eventId>`。**人批不豁免真机证据、不豁免 tier3**。

## 待裁三：两处工具/治理缺口（阻断合并动作）

1. **G4 verify-logs 结构性缺口**（上轮已报，见 `gate-tooling-gap-g4.md`）：全绿 Pod 无 verify-logs → 监督闸恒红。本包本轮因 r3 失败轮留有 log 而暂未触发，**不等于已修**。主线程修 `verify.mjs` 的 `writeGateLog` 位置后需再跑一轮自然产 log。
2. **`parallel-overlap-tier3` 对生成件误判**（本轮新发现，见 `gate-tooling-gap-overlap-index.md`）：`delivery/workstreams/INDEX.md` 实撞仅「最后更新」日期行（生成件），按 §10d 应判 ②；architect 消解评审（`a7c1ad6c73ea2da9f`）判 ②，建议 `.gitattributes` 加 `merge=union`（+ 同批裁 `index.jsonl`）。**该条不在 `escalateExempt` 内，人批豁免不了**；合并前必须让冲突消失（union 条目 或 主线程 rebase/merge 消解 + 重跑 `planning-index.mjs`）。
3. **`change-manifest` 闸在 current/ 指针流程下恒放行**（轮 B 修复轮新发现，见 `gate-tooling-gap-manifest-cutoff.md`）：脚本用「解析后的 TaskPackage 路径」的首次提交日期判存量包，而正常 Pod 流程解析到 `delivery/taskpackages/current/taskpackage.json`（该文件 2026-09-11 起就在版本控制里）→ 任何包都被判 pre-cutoff 放行，`changeManifest` 对账与「清单外实现面即红」两条判据**结构性不执行**。本轮 planner 已按同样规则手工对账（七件实现面文件，清单外 0 件）并补登 builder 单测文件，但**闸本体需主线程修**（修法建议：按 changeId 目录或 changeId 前缀日期判包龄，current/ 投影文件不得作包龄判据）。

## 待裁四：spec 漂移（主线程/需求面，Pod 零写）

- D3 收窄 requirements §2.1「记录会话不适用 60s 会话超时」的逐字冻结（空素材记录会话由设备侧 60s 收尾 = 释放不归档；AC5 只覆盖有素材场景）。`docsSyncRequired=false` 与实际影响不一致。
- 建议按需求面轻量注记处理（§2.1 记录会话段对账句 / §7 开放问题 7 追加），由 architect 在需求面落；**本 Pod 对 `docs/**` 零 diff**。
- **新增（轮 B 修复轮）**：`docs/architecture.md` §5.1.1 对 60 秒 producer 的描述需要补「有效语音证据」的定义（连续电平可信窗 `[3s,100s]` 或引擎 stt 新鲜 `<25s`；纯静默仍恰 60 秒；电平卡死最迟 160 秒有界收尾 + `suspect` 审计位）。同样只作架构面轻量注记，**本 Pod 对 `docs/**` 零 diff**。

## 非裁项（已按纪律处置，仅备案）

- 两条 debt（`TECH-261114` / `TECH-261012`）维持 `deferred`（结项凭据 = 轮 C 真机）；轮 C 通过后由主线程改 `closed`/`closedBy` 并 `debt-render.mjs` 归位。
- `docs/requirements.md:48` 的 60s 对账 ⚠️ 注记：归 workstream 留根 1（主线程路由）。
- 变异观察项 4 条（超串改名/注释分号不敏感、B10 md 断言判别力、start_organizing 观测面）已在 `mutation-evidence/round-a-fix.md` 与 review-packet 如实登记，不阻塞。
- workstream `261115-session-channel-resident` 的 progress/INDEX 与 spine 回填归主线程（Pod 零写 `delivery/intake/**`）。