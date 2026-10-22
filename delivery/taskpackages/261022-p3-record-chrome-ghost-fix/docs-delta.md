# docs-delta — 261022-p3-record-chrome-ghost-fix

> 本件 = integrator 制品面（delivery/taskpackages/** 授权内）。**spec 面（docs/requirements.md / docs/architecture.md）integrator 无写权**（ARQ-260929-docs-sync-spec-handoff；本包判定 = **零 spec 变更**，无需交回 planner 落盘——理由见 §2）。

## 1. docs-sync 状态

- **verify 链 docs-sync 闸全绿**：r4 final bearing（phase=final，invocationId `b9404d48-a4ba-454d-a2ed-1fd5d40891bc`，`verifier-evidence/rounds/r4.json`，fact `ok:true` + verifier-evidence-check PASS）与 r3 scoped 均绿。
- **r2 曾红的闭环路径**：r2 docs-sync 红（`docRefs do not mention any declared acceptance ids`——两份 docRefs 均未出现 CG 锚）→ fixer/收口轮补齐（commit `d71ec015`）：① `delivery/taskpackages/261022-p3-record-chrome-ghost-fix/plan.md` 增「冻结齿映射」节（CG-A1..A5 ↔ 验收方向 1①/1②/1③/2/3 逐条映射）；② `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md` 正文尾部 append-only 追记 consumed-by 段（缺陷①②→本包承接 + seam 契约 [CG-C1]/[CG-C2]/[CG-C3] 落点 + 帧类型负锚守住声明）。两文件即 taskpackage `docRefs`，锚句入文后 docs-sync 转绿，r3/r4 恒绿。

## 2. spec 面判定：零变更（理由申报）

- 本包 = 固件 chrome **内部修**：键帽裁决读侧（keybar.c scene 生命周期单源 + story 过渡窗三态裁决）+ 成功归档路径层栈收口（复用既有 `kidpal_render_node_story_local_front` 通道）——零新对外行为承诺、零新数据类、零 wire 形状改动。
- **契约面零触碰**：`docs/contracts/**` 无 diff；帧类型负锚（不得新发明下行帧类型）在冻结齿 CG-A3 断言内承载并在七变异取证中验证（mutation 5 摘收口即红）。triage 四面探测逃生门预查干净（NO_CONTRACT_CHANGE），实现未越该裁定。
- **需求面零变更**：`docs/requirements.md` 无 diff——缺陷修复不改变「产品是什么」；human 意图锚（KPLOG005 观感原话）经 debt 条目 TECH-261022 承载，不进需求 SSOT。
- **架构面零变更**：`docs/architecture.md` 无 diff——场景单源上收 keybar.c 是既有「纯逻辑单源 + 设备侧接线」分层形态的就地沿用（keybar_three_slot 包先例），不引入新架构决策、不触发 ADR（taskpackage `adrRequired: false`）。

## 3. 已落盘的文档面动作（本轮实际 delta）

| 落点 | 动作 | 提交 |
|---|---|---|
| `delivery/taskpackages/261022-p3-record-chrome-ghost-fix/plan.md` | 冻结齿映射节补齐（CG-A1..A5 锚句，docs-sync 修复位） | d71ec015 |
| `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md` | consumed-by 追记（append-only：缺陷①②→本包承接、缺陷③④⑤→并联包 261022-p3-record-front-polish-fix 待起） | d71ec015 |

## 4. 零触碰面

- `docs/contracts/**`（帧类型负锚 + crossEnd 豁免，零 wire 改动）；
- `docs/requirements.md` / `docs/architecture.md`（spec 面零变更，§2 理由）；
- `server/**`（并联包 261022-p3-record-front-polish-fix 的面，本包零 diff——crossEnd 实测与 detect_changes impacted 全落 firmware/main 一致）。
