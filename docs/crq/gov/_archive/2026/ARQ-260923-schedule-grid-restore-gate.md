---
id: ARQ-260923-schedule-grid-restore-gate
title: `data.restore` 生产纪律运行时闸真空——LLM 幻觉直产带 restore 的完整网格可过 shape floor 静默填缓存，处置分歧（机械闸 vs ADR-005 残余风险记录）adjudicator 判 DEADLOCK 待 human 拍板
domain: gov
status: closed
severity: MAJOR
owner: ""
trigger: F-C5 拍板包 human 裁定后 + 契约门批后，随 spec 冻结销行（若 human 裁 accept-as-risk 记法则转 §1.3 watchlist 持续监控面）→ **2026-09-23 已结**：human 裁 accept-as-risk（ADR-005 残余风险记录已落，见 docs/ADR.md ADR-005 残余风险段）；契约门同日批准 §1.3② 四件（C1 挂账）；spec/契约同 PR 落地，本条转 LLM 直产 display 面整体收口 watchlist（restore 拒键随该收口一并做）。
opened: 2026-09-23
closed: 2026-09-23
closedBy: human 裁 accept-as-risk 2026-09-23（docs/ADR.md ADR-005 残余风险段）+ 契约门同日批准 §1.3②；delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/
paths: []
source: 260923-arch-review-schedule-grid-cold-boot（findings F-C5 + deadlocks.md，2026-09-23）
---

**证据**（260923-arch-review-schedule-grid-cold-boot 对抗 review，F-C5，2026-09-23）：事实面双方独立核验一致且经判官复核——① 截获层只拦薄载荷（`server/gateway/front/front_app.py:511-514` 恰 `view=="schedule"` 薄载荷才展开）；② schema 层是 shape floor（`server/gateway/display_schema.py:87-105` 只查 `{}` 或 `{week:{courses:[{day,slot,name,flags}]}}` 下限，不拒 data 内多余键 `restore`）；③ 机械验收齿 2 守的是 `expand_schedule_intent_payload` 展开路径、不守 LLM 直产路径——三道拦截主张被证伪两道半。**危害面**：幻觉网格被静默填入 P2 缓存（不接管前台、下次三触发推送即被真数据覆盖），低于既存「LLM 直产不带 restore 完整网格可接管前台」的洞，但违背 ADR-004 单源纪律的板端呈现面。**处置分歧（拍板包在 `delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/deadlocks.md`）**：正方（challenger）= `validate_display` 机械拒 restore 键 + front 自产路径 provenance 旁路（把生产纪律从 prose 升为可执行）；反方（defender）= 不单独设闸——既存更重的洞（LLM 直产完整网格接管前台）不设防而单独给更低危害变体上闸是不对称安全表演、且动冻结中继链（front_app.py:533）代价高，第三条路 = ADR-005 显式记残余风险、留待 LLM 直产 display 面整体收口时一并做（adjudicator 推荐此路线，建议即地板、可砍更大）。**连带状态**：同轮 review 终裁 CONFIRMED 9（含 2 条 ADR-005 记法要求）/ DISMISSED-RESOLVED 5 / DISMISSED 7 / DEADLOCK 1，修复清单 10 项（reviewDir `findings.md`）；spec 可冻结双前置 = ① 本条 human 拍板 + ② 契约门批准 §1.3②（additive `data.restore` + modifying L85 板子行为句）——契约未批 spec 不可冻结（charter 铁律）。

**结项**（2026-09-23，260923-gov-review-arq-ssot-restructure 迁移期按 PROPOSAL T3 例外条款归位）：源队列中本条为「就地结项未搬 §2」的在案漂移实例（事故④）——`处理时机` 格已写明 human 裁 accept-as-risk、契约门批准、spec/契约同 PR 落地，但状态仍滞留 §1.1。新模型下结项 = 翻字段：`status: closed` + `closed/closedBy` 如上；持续监控面（LLM 直产 display 整体收口时 restore 拒键一并做）已并入 `trigger` 字段原文留痕，不再另立 watchlist 行。
