# arch-change · 架构+契约面变更流程（review loop）

> 权威面：本文件是架构+契约面变更的流程 SSOT（261015-gov-review-face-loop-unify 立）。入口命令 = `.claude/commands/arch-change.md`；对偶流程 = requirement-change.md（需求面）、gov-change.md（治理面）。统一元结构：**每面 = 起草/审稿对偶 + 纯编排主线程**。

## 立项与收敛度判断

1. **human 裁意图**——原话即起草依据（独立发起）；graph 发起（spine）则开场消费 faceMap / facePlan，不得要求 human 重述意图。
2. **收敛度判断**（human 立项时）：方向清晰（该变、收益与成本明确）→ 直接进起草审稿；方向未收敛（该不该变、收益与影响看不清楚）→ **Track A 段**（见下）。
3. **触契约探测**：变更是否改变端云数据格式 / shim API / 课表 MD 结构 / display 响应块——是则本批必须携带契约包三节（见「触契约分支」）。

## Track A 段判据（命中任一 → 走 Track A 段）

① human 立项时判断方向未收敛——该不该变、变更的收益与影响还看不清楚，需要先把方向讨论定下来；② architect-reviewer 评审中发现方向级缺口——方案存在未裁的根本分叉、收益与影响无法从方案判定（cover 不全），升级（只升不降）；③ 声明红队（human 或 architect 任一方）。**判据只看收敛度，不看触及面**：触及 §2.2 端云四层链路拓扑 / §2.3 端云职责划分 / 阶段划分 / §1.2 外部依赖增删等承重节，若方向清晰，走主路径即可——这些节由轻路径判据④保证必落主路径。

**Track A 段流程**（对抗机器，SSOT = arch-review-protocol.md）：主线程派 review-moderator 编排——architect 起草候选 → scenario-author 冻 charter → challenger↔defender 辩论 → adjudicator 裁定 → **human 拍方向**；收敛后流回起草审稿，内容不再重复四查（对抗机器已审得更深），落地走 Track B 后端。

## 起草与审稿

- **起草 = architect**（主线程派发；输入 = human 原话 / spine sources / 讨论记录，裸 prompt 停下反问）：Track B PROPOSAL（改法集）或 Track A 候选。
- **审稿 = architect-reviewer 四查**（**恒定，改一个字也审**）：① 意图保真（human 原话 → 文本表述）② 纪律合规（arc42 骨架 / 锚稳定规则 / 三份分工句——「指针不抄细节」「决策本体归 ADR.md」「数据格式归 contracts/」「需求与阈值归 requirements.md」）③ 撞车复核（与既有节 / ADR.md 既有条目 / 决策历史档冲突）④ 牵连面完整（反向锚牵连、联动文件、契约指针刷新、镜像句边界）。否回循环至过（architect 再改，状态留痕 PROPOSAL）。
- **判档 = architect-reviewer**（四查后）：轻路径 / 主路径，判据见下；**只升不降**。

## 轻路径判据（全部满足才可走轻）

① 零语义变化（不改任何机制/行为/拓扑/边界/阶段/阈值的含义）；② 改动性质限于：错字、排版、表述澄清、语言润色（可挂 `design-doc-polish` skill 执行）、锚行格式、指针刷新（版本号/日期/链接）、旧→新节号映射维护；③ 不触 `docs/contracts/**`；④ 不触 architecture.md §1.2 外部依赖清单、§2.2 拓扑图、§2.3 端云职责划分、§5.x 视图结构、§9 演进路线。**命中任一否定 → 主路径；评审中发现语义成分 → 只升不降。**

## 轻路径落地通道（R1-F3 / R2-F1）

轻路径批同样建 change 目录（token 同 `gov-review` / `arch-review` 命名槽），含**瘦制品五件**——瘦 PROPOSAL（Status 行 + 改法清单表 + 判档依据节）、§7 评审 GREEN、landing-manifest（数行）、瘦 architect-landing-summary（packet-id + manifest 复述 + claimed 清单，数行——landing-fidelity-check.mjs 结构门硬要求件）、landing-verdict PASS；升级闸 kind=gov 机锚（FROZEN + GREEN + verdict）全数在场、**闸零改动**；retro 豁免（瘦 PROPOSAL 尾节一行记由，复盘并入 cadence loop-retro）。**轻的是文书重量，不是核验重量**——独立核验与过闸纪律对轻批一分不减。

## 触契约分支

架构变更改变端云数据格式 / shim API / 课表 MD 结构 / display 响应块 → PROPOSAL 改法集必须含**契约包三节**（proposedContractDiff / impact / validationPlan，字段名沿 architect-reviewer 输出格式），契约 diff 与架构改动随批原子落地；纯契约变更（无架构配套）同样以本 loop 为入口。四分类枚举权威 = `.claude/rules/contracts.md`（五值）。**无人类签字门**——契约防线 = Bash 重定向写恒拦 + 实现面 agent 与 dev-loop 会话禁写不变 + architect 起草 / review-moderator 编排写权维持 scope-map 现状（落地恒归主线程 packet-bound）+ 合并机械凭据闸（`pre-merge-escalation-gate.mjs`：契约 diff 须有 FROZEN PROPOSAL + PASS verdict + manifest 覆盖三件联立凭据）。

## 定稿载体

- 架构内容变更的定稿标记 = **`docs/architecture.md` 文末「变更记录」节的 changeId 行**（不引入文档版本号；无状态行——PROPOSAL 模式下评审期架构文件不被触碰）。
- 消费保护：`launchPreconditions` 的「支撑语义 → `docs/architecture.md` 已冻结」判据 = 变更记录节对应 changeId 行在场（文句约定，机械齿留根）。
- 镜像句接缝：需求面批的**逐字镜像**句随需求面流程审、本 loop 不重复审（spine `anchors.arch` 记一笔）；超出逐字镜像（改了机制描述）→ 升格为架构面变更，走本 loop。

## 角色分工

| 角色 | 职责 | 禁止 |
|---|---|---|
| 主线程 | 派发 + packet-bound landing + 状态翻转 + 记账（四面纯编排） | 不起草变更内容；不改 reviewer 判决；不跳否回循环 |
| architect | 起草（PROPOSAL / 候选 / 契约包三节） | 不落 canonical（写入 ≠ 冻结）；不自评；自改递归须 human |
| architect-reviewer | 四查 + 轻/主判档 + Track A 段升级判定 + Track B 评审循环 | 不落 canonical；不批内容；与 architect 不得互兼；判档不自证 |
| review-moderator | Track A 段编排（对抗机器） | 不判档、不落地 |
| human | 立项裁意图与收敛度 + Track A 段拍方向 + 例外升级 | — |

语义权威恒 = human。尾句：触 `docs/contracts/**` 的变更是本 loop 的触契约分支，不再有独立契约门。
