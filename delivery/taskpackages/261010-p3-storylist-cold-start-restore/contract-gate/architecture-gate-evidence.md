# Architecture Gate Evidence · 261010-p3-storylist-cold-start 契约面（story_list data.restore additive）

> Gate: 261010-p3-storylist-cold-start（spine）/ TECH-261010-p3-storylist-cold-start-empty
> Date: 2026-10-12
> Status: LANDED·CLOSED（2026-10-12 human 契约门批准；契约 diff 已随 261010-p3-storylist-cold-start-restore 实现同批原子落——落地核证见 §5/§7，verdict 见 §10）

## 1. Gate Scope

- 面 A：`docs/contracts/interface-contract.md` §1.3③ story_list data 块增设可选布尔 `data.restore`（+ 板子行为句例外子句 + §1.5 第 3 条 floor 枚举联动 + §1.3③ 变更记录行——packet D1–D4 四处）。
- 面 B：§1.5 第 6 条 :180 时点枚举句（story-author/reviewer 双报的既有漂移核对项）。
- Out of scope：§1.3② schedule 面、§四 协议剖面、requirements/architecture（只读引）。
- 契约落地时点 = 与 261010-p3-storylist-cold-start-restore 实现**同批原子落**（spine facePlan ③；面 B 分类此前提成立）。

## 2. Inputs Reviewed

- spine = delivery/changes/intake/261010-p3-storylist-cold-start.md（faceMap/facePlan/OQ-1）。
- requirements.md §9 v1.63（S-F2.1 AC1 :414 / AC5 :418 / AC6 :419 / 补充规则 :422，frozen）。
- architecture.md :186（§5.1.2 front 网关行三时点）/ :214（§5.2 场景④ story_list 权威行重载）。
- docs/contracts/interface-contract.md :85-87（§1.3② restore 先例）/ :92-110（§1.3③ 现文）/ :177（§1.5 第 3 条 floor）/ :180（§1.5 第 6 条）。
- docs/ADR.md :42-49（ADR-005）/ :76（ADR-009，新 ADR 编号依据）。
- 实现侧：delivery/taskpackages/261010-p3-storylist-cold-start-restore/plan.md（reviewed plan，Full）；firmware/main/mcp_server.cc :1033-1119（story_list 分支）/ :926-991（schedule restore 先例）/ :444-446（mcp_json_get_bool）；server/gateway/front/front_app.py :179/:216/:223/:573/:1005-1012/:1013-1019。

## 3. Architect-Reviewer Classification

| 面 | 分类 | rationale 摘要 |
|---|---|---|
| A · §1.3③ `data.restore` additive | **CONTRACT_CHANGE_REQUIRED** | 现 data 块只有 total/items 两字段，wire 无法表达「恢复性推送（只填缓存）vs 主动推送（接管前台）」区分；需求 v1.63 + arch 镜像已明示该区分须进 wire；§1.3② + ADR-005 逐字同构先例（2026-09-23 契约门获批），本例 = ADR-005 :46 泛化立场预告的第三实例。非字段映射（无既有键可承载）、非本地渲染细节（端云 wire 字段）、非 Phase 2 提前、不破 ADR-005 边界。 |
| B · §1.5 第 6 条 :180 枚举句 | **NO_CONTRACT_CHANGE** | 该句自声明「触发时机属行为策略、权威 = arch §5.2 场景④、不进契约」；恢复时点①后枚举与 arch :186/:214 三时点表述逐点吻合，无需改写。restore 的 wire 语义归 §1.3③ 字段定义，不应注入职责枚举行。既有漂移随本批自愈——**前提 = 契约 diff 与实现同批原子落**；若分批则 :180 须改「WS 建连恢复推送」（随批携带精化句）。 |

（分类全文与权威引文行号见 architect-reviewer 2026-10-12 预审报告；packet 本目录 CONTRACT-CHANGE-PACKET.md。）

## 4. Human / Architecture Decision

- **HUMAN_APPROVED · 2026-10-12**：human 经 AskUserQuestion 签署（答复「批准（推荐）」，问答原文留痕于本批主线程会话）。所答精确问题 = packet §5：「是否批准 interface-contract.md §1.3③ story_list 的 data 块新增可选布尔字段 `data.restore`（additive——缺省/false 设备行为完全不变；true = 恢复性载荷只填 RAM 列表缓存不接管前台、P3 在屏仅重绘本页；复刻 §1.3② / ADR-005 先例），连同 §1.3③ 板子行为句 restoring 例外子句、§1.5 第 3 条 floor 枚举随批补『restore 可选布尔』、§1.3③ 变更记录行，与 261010-p3-storylist-cold-start-restore 实现同批原子落？」
- 附带裁定随选项说明一并确认：ECS 恢复推送开关默认关 → 烧板验证后人工开闸（C1）；随批新增 ADR-010；面 B（§1.5 :180 枚举句）不动。
- 落地通道 = CCP 先例（契约门批准后逐字 patch 过目 → git apply、收口义务同批）；diff 尚未落（与实现批同拍，C2）。

## 5. Applied Diff Inventory

- **已落地核证（2026-10-12 integrator 实测）**：`git diff main...HEAD -- docs/contracts/interface-contract.md` = **5 insertions / 2 deletions**（两处 modifying = D2/D3 各 1−1+，两处插入 = D1 两 bullet + D4 变更记录行），全部落 §1.3③ 与 §1.5 第 3 条两个预期区域、零 packet 外契约字符。
- **逐字核对留痕（CCP §2 → 落地文本，程序 needle 比对 2026-10-12 integrator 实跑）**：D1-L1（241 字）HIT / D1-L2（263 字）HIT / D2 新句整行 HIT / D3 新句整行 HIT / D4 变更记录行（398 字）HIT——五条净内容行逐字一致；D2/D3 各删原句一行 + 插新句一行（modifying 形态与 packet 声明一致）。
- **CRLF 保全字节级实证**：落地后工作副本 282 行全 CRLF、0 裸 LF（`file` 报 CRLF line terminators；字节级计数实证）——260921-m3-k1 先例的 CRLF 陷阱未复发。
- 迁移/数据风险：无（无持久化 schema；wire 字段 additive）。docs sync：requirements v1.63 已先行落；arch :186/:214「随契约门落、尚未落地」挂法句翻既成——**翻法 = 最小改动只翻挂法**，patch 文本已拟（见本包 review-packet「交 planner 代办」），integrator Edit 被 enforce-scope 拦截（scope-map integrator allow 面不含 docs/architecture.md，机械闸优先未绕行），**落盘 pending-planner**。
- Batch gatekeeper sync：census 已录（见 §8）。

## 6. Closeout Conditions

- C1（开闸时序，schedule 先例同型）：ECS front 恢复推送开关默认关 → 固件烧板验证（SL-A5 真机冷启动轮，human 在场）→ human 人工开闸。owner = 261010-p3-storylist-cold-start-restore 包 + human（开闸）。
- C2（门生效前提）：契约 diff 与实现同批原子落（面 B 分类成立前提；分批即回炉）。

## 7. Validation Evidence

- **契约 diff 逐字人工核对记录**：见 §5（五条净内容行程序 needle 逐字 HIT + 5 added/2 removed 实测 + CRLF 保全字节级实证）。
- **JSON 示例回归（2026-10-12 integrator 实跑，CCP §6.2 三例过 display_schema story_list floor）**：① restore 帧（`{"restore":true,...}`）→ PASS ok；② 非 restore 帧（data 无 restore 键）→ PASS ok；③ 畸形 restore 帧（`"restore":"yes"` 非 bool 字面）→ REJECT（`story_list restore must be a boolean when present`，V27 脏块拒绝不入板盲渲染）。三例结果与 packet §6 预期逐条吻合。
- **两面测试执行**：verifier final 轮（scoped r4 全绿后，final phase 以 test-scope-final.txt 圈定跑）承载——server 面 pytest 求交与 firmware host 单 ELF 全量，逐轮 fact = `verifier-evidence/rounds/`；两面测试末行数字与 final fact 引用见本包 review-packet「Verification Evidence」节（单一权威，此处不重述数字）。
- **变异取证**：taskpackage `mutationEvidence.perAcceptance` 六条（SL-A1~A6）全 `turnedRed:true` 且还原复绿（mutation-check 闸 scoped r4 PASS；SL-A6 变异在 builder 落地后重放取证、SL-A5 host 可测前置半含前序 MUTANT 残留还原取证）——诚实留痕见 review-packet provenance 申报。
- 本仓无 spec↔contract 机械门（ARQ-260827 口径：降级为人工核对并如实记录；本节即人工核对记录）。

## 8. Docs / Queue Sync

- spine anchors.contract：**pending-planner 回填**（落地态文本已拟，见 review-packet「交 planner 代办」；integrator Edit 被 enforce-scope 拦截——delivery/changes/intake/** 不在 scope-map integrator allow 面，机械闸优先未绕行）。
- ADR-010：**已随批新增**（docs/ADR.md 末，2026-10-12 integrator 落——第三实例泛化 + 与 §1.3② 两点差异 + 被否备选三项 + ADR-005 :46 泛化立场履行节；append-only 未覆盖 ADR-001..009）。
- CDQ：**pending-planner 代落**——`CDQ-261010-p3-storylist-cold-start-restore` LANDED 行全文已拟（见 review-packet「交 planner 代办」）；integrator Edit 被 enforce-scope 拦截（scope-map 不含 docs/CONTRACT-DECISION-QUEUE.md，261011 同款先例）。
- CRQ：TECH-261010-p3-storylist-cold-start-empty **已收口 closed**（2026-10-12 closedBy=integrator closeout、正文 append 收口节；`crq-render.mjs` 重跑归档 `_archive/2026/` + INDEX 再生）。
- Batch gatekeeper census（2026-10-12 integrator grep 实录）：`.claude/scripts/` + `.claude/hooks/` 全量扫描 `docs/contracts` 引用——命中 **contract-lint.mjs**（verify 闸集内，markdown 契约结构完整性 lint：节标题/围栏闭合/断链，非 diff 扫描门）+ brief-inputs.mjs / crq-ledger-shared.mjs / merge-driver.mjs / pre-merge-escalation-gate.mjs / settings-lint.mjs / enforce-scope.mjs（路径边界与 needle 核验引用，非契约 diff 结构门）。**census 结论：存在一个扫描 `docs/contracts/**` 的闸级门 = contract-lint（本批 scoped r4 PASS），无批级契约 diff 结构门、无 whitelist sync 义务**。

## 9. Rollback / Follow-up

- 回滚 = 契约 diff revert（append-only 行删除 + D2/D3 句尾还原）+ 实现包 revert，同批回滚。
- Follow-up：defer 时代 ctx(story) 条件推送（front_app.py :216）与新建连推送的归并处置 = 行为策略，归实现包（plan.md :29 owner 登记），不属本门。

## 10. Verdict

- **CLOSED（LANDED · 2026-10-12）**：human 批准在案（§4）+ 契约 diff 与实现同批原子落（C2 兑现，§5 核证）+ 验证证据齐（§7：逐字核对 + JSON 三例回归 + 两面测试 final 轮 + 变异取证全 turnedRed）→ 门闭环。
- C1（开闸时序）**仍未结、按设计保持**：ECS front 恢复推送开关默认关 → SL-A5 真机冷启动轮（human 在场持板）→ human 人工开闸。C1 是运行面开闸条件、非契约门条件——门 CLOSED 不受其 pending 影响（schedule `grid_restore_enabled` 先例同型）。
