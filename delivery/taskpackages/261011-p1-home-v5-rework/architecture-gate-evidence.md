# Architecture Gate Evidence · 261011-p1-home-v5-rework

> Gate: 261011-p1-home-v5-rework（契约门，随包变更）
> Date: 2026-10-11
> Status: PRECHECKED（architect-reviewer 预审在案，human 未裁）

## 1. Gate Scope
- 面：`docs/contracts/interface-contract.md` §1.3⑥（:144-158）四触点（preview 形状+三态 / 示例值 / :155 seam 引用签名 / 变更记录行）。
- Owner：human 契约门（签字制）；预审 = architect-reviewer（只读，2026-10-11）。
- Out of scope：requirements.md / architecture.md（零 diff，faceMap 在案）；固件 seam 本体与 producer 实现（实现面，dev 包承载）；板端旧单对象过渡容忍（板端防御，不入契约文本）。

## 2. Inputs Reviewed
- spine：delivery/changes/intake/261011-p1-home-v5-rework.md（faceMap/facePlan/openQ q2 人数裁在案）
- plan：delivery/taskpackages/261011-p1-home-v5-rework/plan.md（契约 changeManifest + 设计要点 1/2/6 + R1-R7）
- 需求权威：docs/requirements.md S-F1.1 AC2/AC5/AC6/AC7（:227/:230/:231/:232）+ 补充规则 6c（:235）；§9 v1.61 行（commit 7f68b51）
- 契约：docs/contracts/interface-contract.md:144-158（全契约 grep preview|明早|next_day 仅 :150/:155 两处）
- K1 先例：firmware/main/display/home_upcoming.h:30-31（HH:MM 裁决原文）；delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/CONTRACT-CHANGE-PACKET.md
- producer 现状：server/gateway/schedule.py:433-448（:446 明日无项发 None = AC6 无载体实证）

## 3. Architect-Reviewer Classification
| 面 | 裁决 | rationale / refs |
|---|---|---|
| preview 形状+三态 | CONTRACT_CHANGE_REQUIRED | AC6/AC7 两态需不同 wire 值，null 已被占用、adapter 无解；interface-contract.md:144-158、requirements.md:230-232 |
| 示例值定名 | CONTRACT_CHANGE_REQUIRED（同 packet 顺修） | K1 裁决载荷钉 HH:MM（home_upcoming.h:30-31），终形 = HH:MM 数组；「明天」只落 chip 资产不入 wire |
| 变更记录行 | 归 packet 程序性组成 | 先例 :109-110/:158 格式 |
| :155 seam 引用签名 | CONTRACT_CHANGE_REQUIRED（plan 漏列触点，预审新发现） | 契约逐字引用旧签名，seam 同形改后不修即漂移 |
| 板端过渡容忍边界 | NO_CONTRACT_CHANGE | 过渡性部署缓冲非持久语义，不入形状定义；仅变更记录行带注记 |
| 其它面巡查 | NO_CONTRACT_CHANGE | display_schema home 无 shape floor、§1.5/§1.6 无连带、arch 零镜像句（grep 实证） |

## 4. Human / Architecture Decision
- Human question：见 CONTRACT-CHANGE-PACKET.md §5。
- Ruling：human 契约门 AskUserQuestion 批准（2026-10-11，主线程呈 packet、human 选「批准」）——四触点一次批（preview 多项数组+三态语义 / 示例值 HH:MM 顺修 / :155 seam 引用签名同形顺修 / 变更记录行 append），随 261011-p1-home-v5-rework 同 PR git apply 落地；无附加条件。pod-events human-decision 事件同步在案（pod-events.jsonl）。
- Status: HUMAN_APPROVED

## 5. Applied Diff Inventory
| 文件 | 面 | additive/breaking | 风险 | docs sync |
|---|---|---|---|---|
| docs/contracts/interface-contract.md §1.3⑥ | 契约 | breaking（形状重定义，三态兼容面已钉） | R1（[] 未发则话术死）/R2（端云错配窗口） | 变更记录行随 diff；ADR 追加 |
- 批级契约 gatekeeper 同步：本仓**无** `SANCTIONED_CONTRACT_CHANGES` 型白名单 gatekeeper（普查 2026-10-11：全仓 grep + 三先例 gate evidence 同口径）；在册机械闸 = pre-merge-escalation-gate contracts-block 与 contract-lint（处置见 §8），无白名单条目需加。

## 6. Closeout Conditions
- C1：契约 diff 经 human 契约门批准后，随本批同 PR git apply 落地（逐字 patch 过目 → apply → 人工 diff 逐字核对留痕；CRLF 保全）——owner 主线程，证据 = git diff 记录。
- C2：板端旧单对象过渡容忍注记随包落（home_upcoming.c 注释 + 变更记录行已带）——owner builder，证据 = 代码 diff。
- C3：CDQ 行 + docs/crq/tech/ 记账 + ADR 追加 + docs-sync 收口——owner integrator，证据 = 队列行/ADR diff。
- （门批后如有附加条件在此续登。）

## 7. Validation Evidence
- 〔门后填：contract-lint --self-check + 实盘 lint 输出；producer 三样本回归（非空数组 / [] / null）命令与 exit code；板端 host 四齿（数组 / [] / 旧单对象容忍 / 非 HH:MM 拒包）；raw log 路径 evidence/〕
- 契约 diff 人工逐字核对：pass-oracle 第 4 关无脚本兜底（ARQ-260827-contract-mech-gate-gap 在册）——如实降级人工核对并留痕。

## 8. Docs / Queue Sync
- docs/CONTRACT-DECISION-QUEUE.md：登本 gate 行（changeId 261011-p1-home-v5-rework；status 随门推进）。
- docs/crq/tech/：记 tech 侧条目 + `cdq:` 指针（gate authority 在 CDQ）。
- docs/ADR.md：追加 preview 形状重定义 + 三态语义裁定行。
- requirements.md / architecture.md：明确不改（v1.61 已 frozen；arch 零镜像句取证在案）。

## 9. Rollback / Follow-up
- 回滚 = 契约 diff 单文件 revert + producer 恢复单对象/None；板端过渡容忍使回滚单向安全（板端可解析旧形状，回滚不砸现网）。
- Follow-up（deferred）：spine openQ q4——requirements v1.61 补充规则内 v1.57 ⚠️ V5 对账注与实现漂移，后续勘误批候选（档 0），不阻塞本批。

## 10. Verdict
- Gate 状态 = HUMAN_APPROVED（2026-10-11）：dev 可按本 packet 实现（launchPreconditions anchors.contract 已满足）；gate 完整 CLOSED 待 closeout 条件 C1（契约 diff 同 PR git apply 落地）/ C2（板端过渡容忍注记）/ C3（CDQ + crq/tech 记账 + ADR + docs-sync）完成后回填，收口义务归本包 integrator。
