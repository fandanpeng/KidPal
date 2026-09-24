# docs/showme/ —— 一次性视觉解释稿归档

放「show-me」类 HTML：讲一个概念、方案、演化或拓扑时画的示意图 / 信息图 / 短幻灯。它们是**解释用的视觉稿**，不是需求、不是设计母版、不是评审证据。

| 文件 | 讲什么 | 成稿日期 |
|---|---|---|
| [show-me-decomposition-chain.html](show-me-decomposition-chain.html) | 拆包 → 执行链：从 story 到 TaskPackage 到 dev-loop 的治理方案草案全链 | 2026-09-12 |
| [show-me-devloop-launch-to-merge.html](show-me-devloop-launch-to-merge.html) | Dev Loop 全程：launch 面准备 → launch → Pod 内十步 → 合并门 → 落本地 main（含 WIP=3 并行与自改进回路） | 2026-09-24 |
| [show-me-gov-evolution.html](show-me-gov-evolution.html) | KidPal ⇄ AInativecompany 治理演化全景（两仓同源、各自长成什么样） | 2026-09-22 |
| [show-me-gov-mechanism-growth.html](show-me-gov-mechanism-growth.html) | 治理机制成长史：诞生 → 实战摔跟头 → 补机制长骨 | 2026-09-22 |
| [show-me-gov-track-a.html](show-me-gov-track-a.html) | Track A 对抗重路径（`/arch-change`）逐步拆解：phase-0 分类 → review-depth 四裁断 → 对抗 loop → 双 profile 分叉 → governance 定稿五步 → 两套 pass-oracle | 2026-09-24 |
| [show-me-gov-track-b.html](show-me-gov-track-b.html) | Track B 常驻轻路径（默认 · 七步闭环）逐步拆解：起草 PROPOSAL → 立项 ack → architect 评审至 GREEN → FROZEN → packet-bound 落地 → landing-verifier → retro → 升级闸自动合并 main（含 3 轮熔断） | 2026-09-24 |
| [show-me-gov-triage.html](show-me-gov-triage.html) | Triage 前置门（`/triage-change`）逐步拆解：第一刀事实源分区 → architect 比例化 review 判四档 → human 确认路由 → 主线程落盘 → 冻结 → 报下一步命令（含 reviewer-first 逃生门） | 2026-09-24 |
| [show-me-index-topology.html](show-me-index-topology.html) | CodebaseMemory 索引拓扑：主索引 `C-AI-KidPal` 与各 Pod worktree 专属索引的分工 | 2026-09-23 |

用浏览器直接打开即可（Git Bash 下 `start docs/showme/<文件>`）。

## 同类的稿子还在别处

下面几件也是同一类视觉稿，但它们各自是评审 / 复盘证据包的一部分、被对应 PROPOSAL 或 retro 引用，因此**留在原处不迁**，此处仅作索引：

| 文件 | 讲什么 |
|---|---|
| [delivery/changes/arch/260912-gov-review-portfolio-decomposition-chain/decomposition-chain.html](../../delivery/changes/arch/260912-gov-review-portfolio-decomposition-chain/decomposition-chain.html) | 拆包→执行链（评审落地版） |
| [delivery/changes/arch/260913-arch-review-spec-split/show-me-spec-split.html](../../delivery/changes/arch/260913-arch-review-spec-split/show-me-spec-split.html) | spec 拆分为 requirements + architecture 两文件 |
| [delivery/changes/arch/260918-gov-review-merge-chain-friction/split-overview-friction-map.html](../../delivery/changes/arch/260918-gov-review-merge-chain-friction/split-overview-friction-map.html) | 合并链摩擦点地图 |
| [delivery/changes/retro/2026-09-13-loop-retro-efficiency.html](../../delivery/changes/retro/2026-09-13-loop-retro-efficiency.html) | loop retro 效率复盘 |

**UI 设计稿不在本目录**——设计稿母版在 `docs/ui/`（`kidpal-ui-design*.html`），它属资产管线的母版，不是一次性解释稿。

## 收录口径

- 收：用 show-me 手法画的解释性 HTML（概念图、方案图、演化史、拓扑图、短幻灯）。
- 不收：UI 设计稿（→ `docs/ui/`）、评审/复盘的正式证据（→ 各自 `delivery/changes/**` 证据包）、过程产物（→ `delivery/taskpackages/`）、第三方文档存档。
- 本目录的登记与边界定义在 [.claude/rules/repo-structure.md](../../.claude/rules/repo-structure.md)（结构单一事实源）+ [.claude/scope-map.json](../../.claude/scope-map.json) `universal.docsTopLevel.allowDirs`，两处须一致。