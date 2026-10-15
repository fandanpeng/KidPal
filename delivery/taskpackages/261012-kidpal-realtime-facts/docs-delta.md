# docs-delta · 261012-kidpal-realtime-facts

> author: integrator · date: 2026-10-12 · TaskPackage: `delivery/taskpackages/261012-kidpal-realtime-facts/`（exitCriteria docs-sync 条 + taskpackage docsSyncRequired）
> docs-sync 状态：**完成申报**——下表逐行落毕，零 open 尾巴。

## 结论一览

| 面 | 动作 | 状态 |
|---|---|---|
| `docs/architecture.md` §5.5.5（:298）现状指针 | 刷 v9→v10 + 沿革链尾补 v10 单行语义（实时事实回合新行 + 对话行通用反编造 + 底线节第三不变量） | **已落（architect handoff 执行，2026-10-12，2 insertions/2 deletions 实测）** |
| `docs/architecture.md` §5.1（:197）现状指针 | 刷 v9→v10（组件表 OpenClaw 本体行） | **已落（architect handoff 执行，随 §5.5.5 一并）** |
| `server/deploy/README.md` prompt 指针节（:44-52） | 核对（builder 已刷「现状 = v10」+ v10 单行语义） | 核对通过（节题、被接线版本名、v9 继承语义与 carrier/provider/overlay 三面一致；:55-57 渲染面/声明面描述与实际键名一致） |
| `docs/contracts/**` | 零改（NO_CONTRACT_CHANGE，architect-reviewer 四分类 2026-10-12，spine anchors.contract 留痕） | 零改（本包 diff 无契约面文件，git status 实证） |
| `docs/requirements.md` | v1.62（NFR-SEC-2 出网路径 ④ Tavily + NFR-SAFE-1 转述边界） | **非本包 diff**——需求面档 1 先行捎带批（requirement-change 档 1，主线程落稿 commit 9d07329，先行于本包），本包仅消费不重述；arch 五处镜像随该批同落 |
| `docs/crq/tech/TECH-261012-weather-fabrication.md` | 收口节（修法落地 + Probe A→F 前后对照 + 遗留） | **已落（integrator 本批，docs/crq/** 在 integrator allow 面）** |

## arch 指针刷新（spec 面，architect handoff 路径）

- 按 v9 先例（ARQ-260929-docs-sync-spec-handoff：integrator 对两份 spec 文件无写权），arch 两处指针刷新由 planner 派 architect handoff 执行（pod-events dispatch 2026-10-12T05:03:16 actor=architect phase=docs-sync-handoff 在案）；本 integrator 只核验落盘内容与本 delta 拟落内容一致。
- §5.1（:197）`system prompt 版本化 v9` → `v10`；§5.5.5（:298）`**现状 = v10**` + 沿革链尾 `→ v10 +回合类型路由表新增「实时事实回合」行〔先播提示→联网真查→只依据查询结果作答+降级诚实，TECH-261012 真机天气幻觉根治〕+ 对话行通用反编造 + 底线节第三不变量`。沿革正文仍在 CARRIER（唯一全量副本），arch 只留指针 + 单行语义，符合 §5.5.5 自身收敛纪律。

## README.md 核对（builder 件，integrator 核）

- `server/deploy/README.md:44` 节题「OpenClaw system prompt 版本化接线（现状 = v10）」与 :47-52「当前被接线的那一版 = `openclaw-system-prompt.v10.md`（回合类型路由表新增「实时事实回合」行……其余段落零偏移继承 v9；261012-kidpal-realtime-facts）」与 carrier / provider.config.yaml / apply-xiaozhi-overlay.sh 三面一致。零改需求，核对通过。

## 零改面核验

- `docs/contracts/**`：git status 无契约面文件；NO_CONTRACT_CHANGE 四分类在案（契约 §1.6 :182-185 逐字引文只钉四语义面，v10 additive 逐面核对零冲突、引文未枚举回合路由与工具政策）。
- `docs/requirements.md`：本包 diff 零改（v1.62 已由先行捎带批承载，见上表）；NFR-SEC-2 ④ / NFR-SAFE-1 为 v10 行为语义权威，v10 文本逐条对齐（转述儿童化改写 / 降级诚实引导锚在 v10 实时事实回合行在世）。
- 历史 v1..v9 prompt 文件：零回改（RF-A3 sha256 负锚机检承载，v9 sha256 0b051e14… 前缀在红基线留痕）。
