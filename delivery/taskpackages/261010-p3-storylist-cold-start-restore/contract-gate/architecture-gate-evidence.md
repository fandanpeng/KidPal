# Architecture Gate Evidence · 261010-p3-storylist-cold-start 契约面（story_list data.restore additive）

> Gate: 261010-p3-storylist-cold-start（spine）/ TECH-261010-p3-storylist-cold-start-empty
> Date: 2026-10-12
> Status: HUMAN_APPROVED（2026-10-12 human 契约门批准；diff 与实现同批原子落，落地时翻 LANDED/CLOSED）

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

- 待批准后回填：D1（§1.3③ 插入 2 bullet，additive）/ D2（:105 板子行为句句尾追加，modifying）/ D3（:177 floor 枚举追加，modifying）/ D4（变更记录 bullet，append-only）。全部 additive 或注记级，零破坏（缺省/false 行为不变论证见 packet §4）。
- 迁移/数据风险：无（无持久化 schema；wire 字段 additive）。docs sync：requirements v1.63 与 arch :186/:214 已先行落（挂法「随契约门落、尚未落地」，契约落本批后两句自动转既成——无需回改）。
- Batch gatekeeper sync：census 待主线程落批前复核（预审未发现扫描 `docs/contracts/**` 的现役批级结构门；如实记录，见 §8）。

## 6. Closeout Conditions

- C1（开闸时序，schedule 先例同型）：ECS front 恢复推送开关默认关 → 固件烧板验证（SL-A5 真机冷启动轮，human 在场）→ human 人工开闸。owner = 261010-p3-storylist-cold-start-restore 包 + human（开闸）。
- C2（门生效前提）：契约 diff 与实现同批原子落（面 B 分类成立前提；分批即回炉）。

## 7. Validation Evidence

- 待批准后回填：契约 diff 逐字人工核对记录；JSON 示例回归（restore / 非 restore / 畸形 restore 三例过 display_schema story_list floor）；plan.md SL-A1~A6 验收齿执行记录。
- 本仓无 spec↔contract 机械门（ARQ-260827 口径：降级为人工核对并如实记录）。

## 8. Docs / Queue Sync

- spine anchors.contract：批准后回填（gate = HUMAN_APPROVED，diff 落地时点 = 实现批）。
- ADR-010：批准后随批新增（packet §7 建议）。
- CDQ/CRQ：TECH-261010-p3-storylist-cold-start-empty 保持 open 至实现落地；`docs/CONTRACT-DECISION-QUEUE.md` 门行登记 = 批准后随批。
- Batch gatekeeper census：预审未发现现役批级契约 diff 结构门（evidence reference §5 要求如实记录；主线程落批前复核确认，无则记「census 无、无需 whitelist sync」）。

## 9. Rollback / Follow-up

- 回滚 = 契约 diff revert（append-only 行删除 + D2/D3 句尾还原）+ 实现包 revert，同批回滚。
- Follow-up：defer 时代 ctx(story) 条件推送（front_app.py :216）与新建连推送的归并处置 = 行为策略，归实现包（plan.md :29 owner 登记），不属本门。

## 10. Verdict

- 未结（PRECHECKED）。human 批准 + 与实现同批落 + 验证证据齐 → CLOSED；实现包在此之前不得以「契约已定」自居 launch（launch 前置 = 本门 HUMAN_APPROVED 在案）。
