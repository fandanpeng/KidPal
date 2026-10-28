# gov-change · 治理面变更流程（review loop）

> 权威面：本文件是治理面变更的流程 SSOT（261015-gov-review-face-loop-unify 立）。入口命令 = `.claude/commands/gov-change.md`；统一元结构同 arch-change.md（每面 = 起草/审稿对偶 + 纯编排主线程）。

## 立项（ARQ 消费）

1. **human 裁意图**（独立发起）或确认 ARQ 候选修法（治理项多自带 ARQ 预消化材料——证据 + 建议改动 + riskTier；命令开场先读条目，不让人重述）。
2. **收敛度判断**：治理项天生收敛（问题即方向边界；开口方向在成为 item 前已被讨论收敛）→ 默认直接起草审稿；Track A 段罕见（判据见下）。

## 起草与审稿

- **起草 = architect**（主线程派发；输入 = human 原话 / ARQ 条目 / spine sources）：Track B PROPOSAL（改法集）或轻路径瘦 PROPOSAL。
- **审稿 = architect-reviewer 四查**（**恒定**）：① 意图保真 ② 纪律合规（治理面版：路由表 / 写权边界 / 门语义不被文本漂移改动）③ 撞车复核（与既有 agent 定义 / 流程 SSOT / 门语义冲突）④ 牵连面完整（引用普查、fixtures、消费方）。否回循环至过。
- **判档 = architect-reviewer**：轻路径 / Track B，**只升不降**。
- **脚本批拆分纪律（261027 批 T3 立，ARQ-261027）**：`.claude/scripts/**` 净增合计 >~150 行（净增 = `git diff --numstat -- .claude/scripts/` 增行合计减删行合计；新增 / 未跟踪文件先 `git add -N` 再计，或按行数直计），或属 feature 级改动（新增子命令 / 判定重写）→ 该脚本改动**单独成批**；fix 级小改（<~50 行、无判定语义变化）可并批搭车。数值为初值、随复发校准。

## 轻路径判据（路径即判据）

diff **全部**同时满足才可走轻：① 仅落在 `.claude/references/**`、`.claude/skills/**`、`docs/notes/**`、`docs/templates/**`；② 性质为措辞/格式/链接/勘误（零行为语义变化）；③ 不触 `.claude/commands/**`（行为入口）、`.claude/agents/**`、`.claude/hooks/**`、`.claude/scripts/**`、`.claude/scope-map.json`、`*.settings.json`、`.claude/verify-gates.json`、`AGENTS.md`、`CLAUDE.md`、`docs/DEV-GOVERNANCE.md`——**命中任一 → 恒 Track B，哪怕只改一个字符**。

## Track A 段判据

① human 立项时判断方向开口（该不该改、怎么改还未收敛）；② architect-reviewer 评审中发现方向级缺口（方案存在未裁的根本分叉，cover 不全），升级（只升不降）；③ 声明红队。**与架构面同一原则：判据只看收敛度，不看触及面**——动承重机器（hooks / scripts / scope-map / 闸判定逻辑）但方向清晰的变更走 Track B（四查审遗漏 + landing-verifier + 升级闸兜底），不因此进 Track A 段；要额外攻击某一高风险项走「声明红队」。Track A 段机器 = arch-review-protocol.md（对抗机器 SSOT，两 loop 共享）。

## 轻路径落地通道

形态同 `arch-change.md`「轻路径落地通道」节（瘦制品五件过闸、升级闸零改动、retro 豁免），本文件不重复定义、只引用。

## Track B 后端

= Track B 七步的公共前端不变；FROZEN 后按 `landing-lane` 分流。P9 默认翻转前缺省 `packet-bound`；P9 达到 INDEX `landed + 已合main=yes` 且 retro 无未闭合 BLOCK 后，纯 scripts/hooks 批缺省 `author-implementation`，human 立项仍可改选旧通道。前两项机读，retro 项由主线程人读、属 convention-only；主线程须在新批 PROPOSAL 的 `landing-lane` 行旁落 `default-trigger-evidence`，记录 P9 changeId、INDEX 实值、retro 路径、人读结论与日期。作者通道由 architect 在当前 gov worktree 非 main 分支实现并提交，reviewer 审真实 diff，landing-verifier 验同一 commit/tree，再经 retro、升级闸与本地 main 合并。契约、spec、G1、settings、agent 定义、scope-map、verify-gates 与混合面恒走旧通道。落地制品义务见 `arch-change.md`「Track B 制品义务」节（单源，本文件不复制）；作者通道新增的 commit/tree 凭据以本文件为唯一流程 SSOT。

## 角色分工

| 角色 | 职责 | 禁止 |
|---|---|---|
| 主线程 | 派发 + packet-bound landing + 状态翻转 + 记账（四面纯编排） | 不起草变更内容；不改 reviewer 判决 |
| architect | 起草；受限 author-implementation 批实现 scripts/hooks 并提交 | 其余 canonical 不落；不自评、不扩 manifest、不写独立凭据、不合并 main；自改递归须 human |
| architect-reviewer | 四查 + 判档 + Track A 段升级判定 + Track B 评审循环 | 不落 canonical；与 architect 不得互兼 |
| human | 立项裁意图/确认 ARQ 修法 + 例外升级 | — |
