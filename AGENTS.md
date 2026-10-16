# KidPal Agent Instructions

## 自有代码与参考代码

- **自有代码** = 固件业务代码 + 云端 shim 代码。具体根目录（候选：`firmware/`、`server/`，或沿用小智工程内 overlay 布局）在动工前由 human 定夺并登记进 `.claude/scope-map.json`——定之前按「未登记实现面」处理，不做机械强制。
- **参考代码** = `ESP32-S3-RLCD-4.2-Demo/`（官方 Demo + 小智 V2.1.0 源码），是第三方源码，只作参考、不在版本控制内。所有 agent 对它只读；要实现它承载的行为时，在自有代码里重写，不直接改参考树。
- **系统 spec 面** = `docs/requirements.md`（需求：产品是什么）+ `docs/architecture.md`（架构：怎么造），两文件互相锚定（项目 = KidPal，docs 平铺、不再套 `systems/<sys>/` 层；2026-09-13 前为单文件 `docs/spec.md`，已归档 `docs/archive/spec.md`）。
- **契约面** = `docs/contracts/**`（单一事实源 [interface-contract.md](docs/contracts/interface-contract.md)：笔顺 JSON / display 统一响应块 / 课表 MD 结构 / shim API）。契约改动 = 架构变更的一种，走 `/arch-change` loop（261015 批：无人签门，合并过机械凭据闸）。

## 治理路由（四面 loop 元结构：起草/审稿对偶 + 纯编排主线程）

治理变更默认走 **Track B 常驻轻路径**——七步闭环（cast 已统一：主线程四面纯编排，不起草变更内容）：

> **前端对齐 + 例外反选（260831-gov-review-auto-merge-local-main 起，261015 批续贯）**：human 只在两个真正判断点出场——① 前端一次性对齐「为什么改（需求）/ 往哪改（方案）/ 改动多大（量级）」= 立项 ack；② 命中例外清单升级拍板。其余（评审/实现/验证/落地核验/合并本地 main）全部 agent + 机械闸自动，例外升级，remote push 仍 human。

1. 主线程派 **architect** 起草 PROPOSAL（业务语言 + 初步技术方案 + human 取舍点标注，骨架 `docs/templates/PROPOSAL.template.md`；输入 = human 原话 / spine sources / 讨论记录，落盘后派发）→ human 立项 ack；
2. **architect-reviewer** 评审循环至 GREEN（只读 canonical、只写评审记录于 PROPOSAL §7；影响目标效果的打回项升级 human）；
3. 主线程凭「立项 ack + GREEN」标 FROZEN；
4. 主线程 packet-bound 落地（landing-manifest 绑定）+ 写 landing summary；
5. 主线程派 landing-verifier 独立核验——结构门 `landing-fidelity-check.mjs` + 逐文件人读 diff 核对实际落地 == frozen PROPOSAL 的 landing 清单 → 绿 → commit 分支；
6. 补写 retro report 闭环（骨架 `docs/templates/retro.template.md`）——趁热写、先于合并；
7. 主线程跑前置升级闸（含 off-main 校验），放行 → 自动合并本地 main（可回滚）；升级闸拦 → 升级人。制品 4 件：PROPOSAL + landing-summary + landing-verdict + retro。状态转移时跑 `node .claude/scripts/changes-index.mjs --upsert <id>` 单行钉进 `delivery/review/INDEX.md`（INDEX 已按 lane 分文件：review 段在 `delivery/review/INDEX.md`、intake 段在 `delivery/intake/INDEX.md`、dev 段在 `delivery/taskpackages/INDEX.md`，各 lane 只写自己的 INDEX）。**轻路径**（文档面措辞级批）= 瘦制品五件过闸（瘦 PROPOSAL + §7 GREEN + landing-manifest + 瘦 landing-summary + landing-verdict；retro 豁免），判据见各面 SSOT。

**升级判据（命中任一 → 所属面 loop 的 Track A 段，对抗收敛后流回 Track B 落地）**：① 跨系统结构性变更（如固件协议与服务端语义同时动）；② 方向未收敛（该不该变、收益与影响看不清——human 立项时判，或 reviewer 评审中升，只升不降）。「声明红队」= 自愿逃生门（human 或 architect 任一方随时可启）。**熔断**：评审 3 轮未 GREEN → human 三选一（转 Track A 段 / 撤回·缩圈 / 换新判官实例再战 1 轮——仍不绿自动转 Track A 段）。**存疑向严**：判据适用存疑默认升 human。

**四面入口（每面一个 loop，互为对偶）**：
**需求面变更**：走 `/requirement-change` loop（260928-gov-review-requirement-loop 立；流程 SSOT `.claude/references/requirement-change.md`，story-author 盘上 updating 起草 + requirement-reviewer 审稿 + 档 2 architect 必派〔免派例外见 SSOT〕；260928-gov-review-requirement-face 立三档判据与定稿载体）。
**架构+契约面变更**：走 `/arch-change` loop（261015-gov-review-face-loop-unify 立；流程 SSOT `.claude/references/arch-change.md`——architect 起草 + architect-reviewer 四查〔恒定，改一个字也审〕+ 轻/主判档〔判据见 SSOT〕+ Track A 段〔只看收敛度，不看触及面〕；契约变更 = 架构变更的一种，契约包三节随批原子落、无人签门，合并过机械凭据闸）。
**治理面变更**：走 `/gov-change` loop（同批立；流程 SSOT `.claude/references/gov-change.md`——同 cast；轻路径 = 文档面〔路径即判据〕，机器面〔commands / agents / hooks / scripts / scope-map / settings / G1〕恒 Track B；ARQ 条目直接作立项材料消费）。
**实现面变更**：走 `/triage-change` 五档分流 → `/build-feature`（dev loop 不变）。
**跨面 intake**：触面 ≥2 的跨面 idea 先过统一 intake 建 spine（`delivery/intake/<changeId>.md`；程序 SSOT `.claude/references/change-intake.md`，260928-gov-review-cross-face-intake-spine 立）——统一 intake ≠ 统一门，各面门照旧各归各裁；spine 免冻，anchors 记录各面自己的冻结态。CLAUDE.md 经 `@AGENTS.md` 导入传导，不另改。

**landing 通道（常态规则）**：恒 = 主线程 packet-bound 一条；architect 恒起草、architect-reviewer 恒审稿、两者永不 landing。
**不动**：契约面防线（Bash 重定向写恒拦 + 实现面 agent 与 dev-loop 会话契约禁写 + 合并机械凭据闸 `pre-merge-escalation-gate.mjs`）、landing-verifier 独立核验、机械强制层（scope-map / hooks）。

## Hard Rules

- 主 AI 不直改自有实现面做正式工作——实现动作走 dev-loop（planner Pod 派 architect（前置技术预评）→ acceptance-author → builder → verifier↔fixer → integrator → supervisor）；唯一例外 = human 明确说跳过流程的一次性操作，且须诚实记录「跳过流程」。
- **契约冻结**：任何会话不得用 Write/Edit/Bash 重定向写 `docs/contracts/**`；改动走 `/arch-change` loop（architect-reviewer 四分类过滤保留，无人签门——261015 批）。
- Completion 不凭解释宣布——治理变更完成 = landing 完成 + landing-verifier 绿 + retro 齐。
- After `git add`, verify with `git diff --cached --stat` before committing. 历史记录类文件一律 append，不覆盖既有行。
- 本文件遵循 Instruction Budget 三问（模型自发现？所有任务用？数月后仍立？），定期复评修剪。

## CodebaseMemory 使用纪律

本仓自有代码及各参考工程均已建 CodebaseMemory 图索引（moderate 模式）。「trace before edit」用 `search_graph` / `trace_path` / `get_architecture` / `search_code` 等图工具，project 参数填下表项目名全称；查不到再降级 Grep。

| 范围 | 图索引项目名 | 覆盖内容 | 排除 |
|------|--------------|----------|------|
| 本仓主索引 | `C-AI-KidPal` | `firmware` 自有固件 + `server/` 云端 shim | `docs/`、`tools/`（含 LVGL 模拟器）、`delivery/`（交付工件面，260914 批 OQ3(a) 裁定排除）、`build/`、`managed_components/`（排除规则落 `.cbmignore` + `.gitignore`） |
| 参考·Arduino | `ESP32-S3-RLCD-4.2-Demo-01_Arduino` | Arduino 示例、LVGL8/9 库 | — |
| 参考·ESP-IDF | `ESP32-S3-RLCD-4.2-Demo-02_ESP-IDF` | ESP-IDF 官方示例（含 10_FactoryProgram） | — |
| 参考·XiaoZhi | `ESP32-S3-RLCD-4.2-Demo-03_XiaoZhi` | 小智 V2.1.0 源码（显示/字体/资源方案参考） | — |
| 参考·Firmware | `ESP32-S3-RLCD-4.2-Demo-04_Firmware` | 固件相关（内容极少） | — |
| 参考·XiaoZhiServer | `ESP32-S3-RLCD-4.2-Demo-05_XiaoZhiServer` | 小智官方 Python 服务端 `xiaozhi-esp32-server`（流式网关参考实现，MIT；2026-09-02 定向 B 后克隆，部署源在 ECS `/opt/kidpal/xiaozhi-esp32-server`） | — |
| 治理机检 | `KidPal-claude` | `.claude/hooks` + `.claude/scripts`（治理钩子与机检脚本，2026-09-13 human 裁定入图；2026-09-14 human 裁定维持不扩） | `.claude` 其余内容（agents/skills/rules/settings 等，控噪规则落 `.claude/.cbmignore`；taskpackages 已迁出至 `delivery/taskpackages/`，2026-09-15） |

- 主索引项目名 = 工具按路径派生（`C:/AI/KidPal` → `C-AI-KidPal`），与全部 Pod worktree / 参考工程条目同格式；`index_repository` 刷主仓**不传 `name`**——手工另传名会在同 root 产生双条目（2026-09-23 实测发生过一次，已删旧人工名条目）。图工具报 project not found 时先 `list_projects` 核对实际条目名，不凭记忆猜。`KidPal-claude` 为唯一人工命名特例（`.claude` 独立建项），不受本条约束。
- 参考代码的索引不并入主索引，避免第三方代码淹没调用图；参考代码只读，不直接改。
- `.claude` 整目录被工具内置 skip-list 硬排除（`.cbmignore` 取反压不过根级硬排除）；`KidPal-claude` 项目以 `.claude` 为索引根独立建项绕开，内部 `scripts/` 再靠 `.claude/.cbmignore` 的 `!scripts` 取反放行（「scripts」目录名在同层级 skip-list、可被取反覆盖）。该项目 `persistence=false`——不入 `.codebase-memory` 快照流，本地重建即可。
- 被排除的目录（如 `tools/simulator/` 的模拟器 UI 源码）不在图内，查它们降级用 Grep。
- `.codebase-memory/artifact.json` 与 `graph.db.zst` 是 indexer 持续自刷新的机器再生面：常驻脏 = 常态，不追交、不 stage、不 stash、不 checkout 还原；合并冲突已由 `.codebase-memory/.gitattributes` 的 `merge=ours` 免除。**快照流向（2026-09-09 human 裁）**：源 = main——偶发、主线程在主 worktree（`C:/AI/KidPal`，普通跟踪态、不设 skip-worktree）直接 add 这两件 + 独立 commit 上 main，不与其它改动混提；分发 = gov 经 main→gov 回同步 merge、Pod 经 rebase + merge-driver 内容还原（Pod worktree 的 skip-worktree 由 merge-driver 幂等维持，勿手动撤）。gov worktree 不设 skip-worktree，脏可见。gov/pod 分支**禁止私带快照 commit**——`merge=ours` 保执行合并的本方，私带会在回同步时挤掉 main 的新快照、ff 回传时静默顶掉。唯一「checkout 还原」例外 = main→gov 回同步 merge 前的定点还原（见 merge-to-main skill，丢弃的只是可再生漂移）。

## 输出标准（人与工件统一）

agent 的散文产出用简体中文写成通顺、完整的句子；领域常用英文术语（skill 名、门名、git/CLI 命令、track/gate/review 类高频词）可保留。内部标识符首次出现必须带一句实质说明；向 human 提问或汇报时不裸甩编号。机器校验的结构字段保持精确格式；证据引用保持逐字精确。散文通顺、引用精确，两者不冲突。禁止读者需自猜的拼接缩写。
