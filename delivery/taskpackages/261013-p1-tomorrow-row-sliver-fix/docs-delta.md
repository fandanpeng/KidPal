# docs-delta —— 261013-p1-tomorrow-row-sliver-fix

> blocking closeout 制品。integrator 落笔（2026-10-13 主轮制品轮；2026-10-13 补充轮收口更新）。本包 = Lite 单包（rigor=lite、sourceType=regression、riskTier=low）：首页 P1「明天」区第 2 日程项被剃成 1px 残线修复（human-confirmed triage bug）——home_p1.c p1_draw_card 明天区行流独立 clip 底界 pv_cy1 218→242 对齐卡区底界 + 两区行流整行容纳守卫（break）；**补充轮**（human 裁定不开新包并入本包）= 行距参数族压缩（line_h 28→22）+ 组级容纳门。

## 文档面改动清单

**本批零文档面改动**（taskpackage `docsSyncRequired=false`，docs-sync 闸 r5 PASS 承载）——plan.md「明确零触碰面」（主轮）与「修订 A §A3 明确零触碰面」（补充轮）逐条对账：`docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` 零 diff（git status 实证：docs/ 下无任何改动文件）。

- 需求面：S-F1.1（常显首页看「接下来干嘛」，AC2/AC5）语义零修订——本批 = 兑现既有验收语义（明天区第 2 项完整字形渲染 / 布局优先级「接下来 > 明天区」在世），行距/容量非需求钉面；布局优先级语义由浮动锚点 + 组级容纳门显式保留（plan 修订 A §A3）。story 语义权威 = docs/requirements.md 现文、零漂移检出，无需 architect 介入。
- 契约面：`docs/contracts/interface-contract.md` 零触碰——preview 数据形状（data.upcoming.preview）与 schedule 链路零变更（taskpackage `crossEnd.exempt` 在案：纯板端显示渲染修复、零 wire 面），contract-lint 闸 r5 PASS。
- 代码面 doc 钉行（随包在案、非本件义务）：home_p1.c p1_draw_card 函数头注释几何描述已随主轮 TR-A1 与补充轮 TR-A6 两度同步改写（底界依据改「卡区底界 containment」+ 新参数族 + 容量表）——注释与实现同 PR 原子落地，无 doc 漂移。

## TECH CRQ 登记建议（非目标留裁量，如实申报）

主轮 plan.md「非目标 / 已知残余」建议登记 TECH CRQ 的三项（明天区锚点随今天区行数下推越卡底 / 换行续行跨底界残线 / 明天区容量与设计线框差距）——taskpackage `docsSyncReason` 明确**不绑本包义务、留 human / 后续布局包裁量**，本批未登记、未动 docs/crq/**。补充轮 plan 修订 A 另登记两项已知取舍（band 守卫容量边角：今天第 8 项行带越卡底 1px 被整行让位 / 紧凑上浮版式随数据浮动 = human 选定语义），同属非目标、不绑义务。如后续立项，去向 = 主线程 triage 分流。

## 零改申报（其余面）

- `docs/crq/**`：零触碰（见上）。
- `.claude/**`、`server/**`、`tools/**`：零触碰。
- ADR：零新增（taskpackage `adrRequired=false`——pv_cy1 对齐卡底 + 整行容纳守卫 + 行距参数压缩 + 组级容纳门为单函数内显示行为/版式修复，非契约/跨系统边界裁决，不构成 ADR 事由；冻结件钉区重锚的授权链留痕由两验收文件尾 REVISION LOG 2026-10-13 条承载，属验收件内留痕非 ADR）。

## Post-cutoff 制品纪律确认

本批 post-EVENTS_SCHEMA_CUTOFF（>= 250725）：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner `pod-retro.md`（收尾相位，本包未到 planner 收尾相位、不在本批）。`delivery/taskpackages/261013-p1-tomorrow-row-sliver-fix/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write，含 r5 scoped round）；`pod-events.jsonl` 在场（含补充轮收尾段：acceptance-author reanchor / builder A1 dispatch + planner friction；advisory 检查归 planner/supervisor、non-blocking）。
