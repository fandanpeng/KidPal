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

---

## 11. Closeout 回填（integrator，2026-10-12）

### C1 · 契约 diff 人工逐字核对留痕（pass-oracle 第 4 关如实降级人工，ARQ-260827 在册）

- **核对日期**：2026-10-12（integrator 制品轮）。
- **核对方法**：机械对照——`git diff docs/contracts/interface-contract.md` 的全部 9 行增删行（5 增 / 4 删，git diff --numstat 实测）与 CCP §3 diff 块（```diff 围栏内，:57-:85）逐行 Python 脚本比对（PYTHONIOENCODING=utf-8，逐字符串相等）；另以字节级核验行尾（python bytes 层拆行统计 CR/LF）。
- **逐触点对照结论**（四触点全在场、位置一致，均落 §1.3⑥ :144-158 区域）：
  - 触点①（preview 形状 + 三态语义）：-单对象示例行 / +数组示例行 / +三态语义 bullet 行——逐字一致 ✓（工作文件 :150/:156）。
  - 触点②（示例值 HH:MM 顺修）：随触点①示例行承载（`"time": "08:00"` / `"19:00"`），无独立行——逐字一致 ✓。
  - 触点③（:155 seam 引用签名）：-旧 3 参签名行 / +新 5 参签名行 `kidpal_home_p1_schedule_inject(day_key, items, count, next_day_items, next_day_count)`——逐字一致 ✓（工作文件 :155）。
  - 触点④（变更记录行 append）：-2026-09-21 旧记录行 / +2026-10-11 新记录行——逐字一致 ✓（工作文件 :159）。
- **CRLF 保全实证**：工作副本 279 行 = 279 CR = 279 LF（每行恰一 CR，全文件统一 CRLF、零混行）；HEAD blob 为 LF 归一存储形态（0 CR / 278 LF，core.autocrlf 正常），diff 仅 5+/4- 行、**无整文件行尾翻转**。
- **差异清单（如实登记，2 处，均呈现性 / 非语义）**：
  1. **hunk 内排序差**：被删除的旧「分组口径」行在 git diff 输出中排在 +seam/+三态 之前（git diff 呈现件把删除行前置），CCP §3 中排在其后——同一 net 内容、两种行序生成的文件内容完全相同，非语义差。
  2. **变更记录行行首 bullet**：CCP §3 的 -/+ 变更记录行文本漏了行首 markdown bullet「- 」（写作 `- （契约变更记录…` = diff 标记 + 裸段）；实际契约行是列表项 `- （契约变更记录…`。落地版**保留 bullet**（与既有 2026-09-21 先例行同构、维持 markdown 列表结构），语义正文逐字一致。判读：packet 文本笔误级省略，落地取结构正确形态，不构成对批准内容的偏离（human 批准的是四触点语义，bullet 是 markdown 结构承载）。
- **结论**：**逐字一致**（净内容 9/9 行字符串相等；2 处呈现性差异如上如实登记、零语义漂移）。hunk 头行号差（CCP 手写 `@@ -141,21 +141,23 @@` vs git diff 再生 `@@ -147,15 +147,16 @@`）为上下文窗口宽度差（git diff 默认 3 行上下文），非差异项。

### C2 · 板端旧单对象过渡容忍注记

- **在场实证（builder 已落，2026-10-12 integrator grep 取证）**：`firmware/main/display/home_upcoming.c:410`（「旧单对象 {time,name} -> *count = 1（部署缓冲过渡容忍一个版本, CCP 触点②…）」）+ `home_upcoming.c:426`（行内注释「旧单对象过渡容忍 -> 1 项列表（板端防御一个过渡版）」）+ `home_upcoming.h:14` / `home_upcoming.h:34`（头注两处）。**C2 = done**。

### C3 · CDQ + crq/tech 记账 + ADR + docs-sync

- **ADR**：`docs/ADR.md` 追加 **ADR-009**（preview 形状重定义 + 三态语义裁定，日期/决策/理由/备选/关联五节齐，含 CCP §2 备选五项与放弃原因、CDQ 关联、tech 记账指针）——2026-10-12 integrator 落笔。**done**。
- **crq/tech 记账（cdq: 指针）**：新建 `docs/crq/tech/TECH-261011-spec-v5-reconcile-note-drift.md`（承载 spine openQ q4 / gate evidence §9 Follow-up——requirements v1.61 内 v1.57 ⚠️ V5 对账注漂移，deferred，frontmatter 带 `cdq: CDQ-261011-p1-home-v5-rework` 指针）+ `ARQ-260907-contract-1-5-strip-wording.md` append trigger 触发留痕（本批契约门触发其「下次契约门」trigger，但 §1.5 校准不在本批 CCP 批准范围，维持 deferred）+ `crq-render.mjs` 再生 tech/INDEX.md。**done**。
- **CDQ 行**：`docs/CONTRACT-DECISION-QUEUE.md` append `CDQ-261011-p1-home-v5-rework` LANDED 行——**integrator Edit 被 enforce-scope 钩子拦截**（scope-map integrator allow 面不含 docs/CONTRACT-DECISION-QUEUE.md，与派单清单冲突，机械闸优先未绕行）；行全文已交 planner 代落（见 integrator 回执）。**pending-planner**。
- **requirements.md / architecture.md**：零触碰（v1.61 frozen；arch 零镜像句取证在案，§8 口径不变）——2026-10-12 `git diff` 实证零命中。**done（零改申报）**。
- **V5 设计稿母版三处内容级同步**：human 直落（pod-events b7e2d94a 裁 B 案主线程直落，2026-10-12 落盘）；Pod 侧核对 = spec-anchor-check 复跑 ok=true（designIdCount=60 不变、errors 零，2026-10-12 integrator 实跑）。**done**。

### Closeout 后 Verdict（改判 §10 状态行）

- **Gate = CLOSED（LANDED）**：C1 = done（本节 §11 逐字核对留痕）；C2 = done（注记在场实证）；C3 = done（ADR + crq/tech + 母版同步零改申报 + CDQ 行文本备妥交 planner 代落——唯一 pending 项为落盘通道，非内容项）。契约 diff 已随包落地并经两面全绿终验（server 236 passed / host 155 Tests 0 Failures，2026-10-12 integrator 实跑，留痕 /c/tmp/hv5_integ_*.txt）。
