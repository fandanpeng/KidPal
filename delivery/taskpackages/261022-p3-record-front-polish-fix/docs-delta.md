# docs-delta — 261022-p3-record-front-polish-fix

> 本件 = integrator 制品面（delivery/taskpackages/** 授权内）。**spec 面（docs/requirements.md / docs/architecture.md）integrator 无写权**（ARQ-260929-docs-sync-spec-handoff；本包判定 = **零 spec 变更**，无需交回 planner 落盘——理由见 §2）。

## 1. docs-sync 状态

- **verify 链 docs-sync 闸恒绿，零红轮**：r3 scoped 与 r4 final bearing（phase=final，invocationId `3b056af4-5b44-464a-b43d-b75f85fd7156`，`verifier-evidence/rounds/r4.json`，fact `ok:true` + verifier-evidence-check PASS）均绿；r1/r2 红分别落在 acceptance-coverage（FP-A6 标记形状）与 mutation-check（取证券），docs-sync 未参与任何红轮——docRefs 锚句在 launch 面（commit `80bc2027`）已随 TaskPackage Finalize 一次到位，未重演并联包 chrome-ghost-fix 的 r2 docs-sync 红教训（pod-retro 已记）。
- docRefs 两件：`docs/debt/tech/TECH-261022-p3-record-session-state-churn.md`（FP 锚句在 consumed-by 追记二段内）+ `delivery/taskpackages/261022-p3-record-front-polish-fix/plan.md`。

## 2. spec 面判定：零变更（理由申报）

- 本包 = front 应用面**内部修**：引导节奏三参数单源（GUIDE_RESPONSE_WINDOW_SECONDS / GUIDE_SPEECH_MAX_CHARS / GUIDE_STT_GUARD_SECONDS，front_app.py 模块常量，数值不进 spec 文本——FP-X2 负锚）+ 应答窗收尾通路 + 素材面播报窗过滤 + list_stories 同日 seq tiebreak + 标题敷衍兜底启发式。零新对外行为承诺、零新数据类、零 wire 形状改动。
- **tiebreak 不触契约**：契约面只钉「按日期倒序」，同日排序是实现自由度——list_stories 同日 seq 倒序（缺 seq 回退 id 不崩）不改变任何既有断言语义；`docs/contracts/**` 零 diff，triage 四面探测逃生门预查干净（NO_CONTRACT_CHANGE），实现未越该裁定。
- **需求面零变更**：`docs/requirements.md` 无 diff——缺陷修复不改变「产品是什么」；human 意图锚（KPLOG005 真机原话）经 debt 条目 TECH-261022 承载，不进需求 SSOT。is_finish_phrase 词表零触碰（v1.66 需求面已冻，FP-X2 负锚）。
- **架构面零变更**：`docs/architecture.md` 无 diff——三参数走 front 应用模块常量单源是既有「应用面内部节奏参数不上升架构」形态的就地沿用，不引入新架构决策、不触发 ADR（taskpackage `adrRequired: false`）。

## 3. 已落盘的文档面动作（本包实际 delta）

| 落点 | 动作 | 提交 |
|---|---|---|
| `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md` | consumed-by 追记二（append-only：缺陷③④⑤→本包承接，FP-A1..A7 齿枚举 + front 部署生效后 KPLOG 链可复验） | 80bc2027（launch 面，先于 verify 链落盘） |
| `delivery/taskpackages/261022-p3-record-front-polish-fix/plan.md` | 冻结齿映射节（FP-A1..A7 锚句，docs-sync 承重面） | 80bc2027 |
| `delivery/taskpackages/261022-p3-record-front-polish-fix/scope-final.md` | 影响面圈定清单（Step 4 制品，final scope-file） | 本轮 closeout |

## 4. 零触碰面

- `docs/contracts/**`（tiebreak 为实现自由度 + crossEnd 豁免：story_list / story_text / kidpal_speak / start_organizing 既有帧形状零 diff，零 wire 改动）；
- `docs/requirements.md` / `docs/architecture.md`（spec 面零变更，§2 理由）；
- `firmware/**`（并联包 261022-p3-record-chrome-ghost-fix 的面，本包零 diff——crossEnd 实测与 detect_changes impacted 106 符号全落 server 三模块一致）；
- 引擎侧候选排除（缺陷④的第三方部署树候选面）不在本包 manifest 内，story 已如实申报。
