# KidPal Agent Instructions

## 自有代码与参考代码

- **自有代码** = 固件业务代码 + 云端 shim 代码。具体根目录（候选：`firmware/`、`server/`，或沿用小智工程内 overlay 布局）在动工前由 human 定夺并登记进 `.claude/scope-map.json`——定之前按「未登记实现面」处理，不做机械强制。
- **参考代码** = `ESP32-S3-RLCD-4.2-Demo/`（官方 Demo + 小智 V2.1.0 源码），是第三方源码，只作参考、不在版本控制内。所有 agent 对它只读；要实现它承载的行为时，在自有代码里重写，不直接改参考树。
- **系统 spec 面** = `docs/requirements.md`（需求：产品是什么）+ `docs/architecture.md`（架构：怎么造），两文件互相锚定（项目 = KidPal，docs 平铺、不再套 `systems/<sys>/` 层；2026-09-13 前为单文件 `docs/spec.md`，已归档 `docs/archive/spec.md`）。
- **契约面** = `docs/contracts/**`（单一事实源 [interface-contract.md](docs/contracts/interface-contract.md)：笔顺 JSON / display 统一响应块 / 课表 MD 结构 / shim API）。契约改动 = 架构变更的一种，走 `/arch-change` loop（261015 批：无人签门，合并过机械凭据闸）。

## 治理路由（四面 loop 元结构：起草/审稿对偶 + 纯编排主线程）

各面默认走路径清晰的常驻短链；主线程四面纯编排，不起草变更内容。治理面自 261029 批起采用 direct-implementation，架构+契约面继续 Track B 后端。

> **前端对齐 + 例外反选**：human 只在新方向、新取舍和例外升级时展开判断；既定 ARQ 波次只作一句启动确认，不重复展开旧修法。其余评审、实施、验证与本地合并由 agent + 机械闸执行。远程发布 = 主线程跑 `node .claude/scripts/release-pr.mjs --publish`（只推 `release/*` 分支并开 PR），PR 合并与其它 remote push 仍 human。

1. 主线程派 **architect** 起草：治理面写轻设计、验收标准与轻量 `landing-manifest`（只列预计路径、实施归属、保护标记），架构+契约面按既有 PROPOSAL 要求起草。输入 = human 原话 / spine sources / 讨论记录。
2. **architect-reviewer** 独立评审并自写评审段。治理面判「常规 + 条件加强」，可升可降；架构+契约面沿用四查与轻/主判档，只升不降。影响目标效果的方向问题升级 human。
3. 主线程凭设计 GREEN 标 FROZEN。治理面冻结意图、边界、关键方案和验收标准，不冻结未来字节。
4. **治理面作者实施**：architect 在当前 gov worktree 非 main 分支直接修改普通治理文件、自测并提交真实 commit；scope-map、settings、指令根、自身与 review-loop agent 定义等保护面由主线程按同一份设计处理。实际非证据 diff 必须 ⊆ 轻量 scope，出界回 reviewer。架构+契约面仍按既有 landing-lane 落地。
5. **治理面真实 diff 审**：reviewer 核对实现、scope 与测试判别力，以最后 verdict 和 reviewedCommit/reviewedTree 钉点。landing-verifier 仅在核心合并/权限自改、高影响难回滚、评审争议或真实语义风险时作第二独立核验；架构+契约面旧核验义务不变。
6. 主线程在设计、设计评审、实现提交、实现评审等节点实时追加本批 `events.jsonl`，摩擦、建议和实际 usage 当场记。普通治理批不再强制事后 retro；架构+契约面 retro 义务不变。
7. 主线程跑前置升级闸（含 off-main 校验），放行后自动合并本地 main；闸拦则升级人。**适用面**：本闸管 dev（`delivery/taskpackages/<changeId>`）与 gov（`delivery/review/<changeId>`）两条 lane；变更集全部落在账本类制品时报 `NOT_APPLICABLE`（exit 0，不是放行），此类合并按 merge-to-main skill 的 human 授权通道执行；其余无法归 lane 的变更集仍 BLOCK 交人。状态转移时跑 `node .claude/scripts/changes-index.mjs --upsert <id>` 更新对应 INDEX；`landed` 与 `mergedInMain` 由脚本现推。

**升级判据（命中 → 所属面 Track A 段，对抗收敛后流回该面实施）**：方向未收敛、存在根本方案分叉，或 human / architect 声明红队；触及承重机器不自动升级。架构+契约面评审 3 轮未 GREEN → human 三选一（转 Track A / 撤回缩圈 / 换新判官实例再战 1 轮）；治理面不设机械轮数升级，评审争议按条件加强处理，方向未收敛才进 Track A。治理面双向判档不等于方向升级；存疑时可先加强，事实澄清后允许 reviewer 降回常规，human 明确指定的加强只能由 human 解除。

**四面入口（每面一个 loop，互为对偶；`/triage-change` 兼跨面编排入口）**：
**需求面变更**：走 `/requirement-change` loop（260928-gov-review-requirement-loop 立；流程 SSOT `.claude/references/requirement-change.md`，story-author 盘上 updating 起草 + requirement-reviewer 审稿 + 档 2 architect 必派〔免派例外见 SSOT〕；260928-gov-review-requirement-face 立三档判据与定稿载体）。
**架构+契约面变更**：走 `/arch-change` loop（261015-gov-review-face-loop-unify 立；流程 SSOT `.claude/references/arch-change.md`——architect 起草 + architect-reviewer 四查〔恒定，改一个字也审〕+ 轻/主判档〔判据见 SSOT〕+ Track A 段〔只看收敛度，不看触及面〕；契约变更 = 架构变更的一种，契约包三节随批原子落、无人签门，合并过机械凭据闸）。
**治理面变更**：走 `/gov-change` loop（流程 SSOT `.claude/references/gov-change.md`——architect 轻设计与普通文件直接实施，architect-reviewer 设计审、真实 diff 审和双向判档；轻量 manifest + 实时事件流，条件加强按实际风险；既定 ARQ 一句启动）。
**实现面变更**：单面实现变更走 `/triage-change` 五档分流 → `/build-feature`（dev loop 不变；五档分流是 `/triage-change` 对实现面的收尾动作，不等于它只属实现面）。
**跨面 intake**：触面 ≥2 的跨面 idea 的编排入口 = `/triage-change`——它把需求 / 架构+契约 / 治理 / 实现四面合在一次陈述里做探测、落 spine，再按 facePlan 派发（需求 / 架构+契约 / 治理三面由它派发到各自 loop，实现面五档分流由它收尾；轻面本会话内联、重面 headless 拉起下游编排会话）；程序 SSOT `.claude/references/change-intake.md`（260928-gov-review-cross-face-intake-spine 立）——统一 intake ≠ 统一门，各面门照旧各归各裁；spine 免冻，anchors 记录各面自己的冻结态。CLAUDE.md 经 `@AGENTS.md` 导入传导，不另改。

**landing 通道（按面分开）**：`/gov-change` 常规 = architect 直接实施普通治理文件，主线程处理保护面，reviewer 审真实 diff；第二独立核验按条件启用。`/arch-change` 继续既有 packet-bound / author-implementation 与 landing-verifier 规则，不随治理面减法改变。两条面都不改 author≠judge、契约凭据、合并闸和 remote 发布边界。
**不动**：契约面防线（Bash 重定向写恒拦 + 实现面 agent 与 dev-loop 会话契约禁写 + 合并机械凭据闸 `pre-merge-escalation-gate.mjs`）、作者与最终审者分离、机械强制层（scope-map / hooks）。

## Hard Rules

- 主 AI 不直改自有实现面做正式工作——实现动作走 dev-loop（planner Pod 派 architect（前置技术预评）→ acceptance-author → builder → verifier↔fixer → integrator → supervisor）；唯一例外 = human 明确说跳过流程的一次性操作，且须诚实记录「跳过流程」。
- **契约冻结**：任何会话不得用 Write/Edit/Bash 重定向写 `docs/contracts/**`；改动走 `/arch-change` loop（architect-reviewer 四分类过滤保留，无人签门——261015 批）。
- Completion 不凭解释宣布——治理面常规批完成 = 真实实现提交 + reviewer 最终 diff GREEN 与有效钉点 + 前置升级闸放行 + INDEX 更新；条件加强另需对应独立核验绿。架构+契约面仍按既有 landing / verdict / retro 义务闭环。
- After `git add`, verify with `git diff --cached --stat` before committing. 历史记录类文件一律 append，不覆盖既有行。
- 本文件遵循 Instruction Budget 三问（模型自发现？所有任务用？数月后仍立？），定期复评修剪。

## CodebaseMemory 使用纪律

本仓自有代码及各参考工程均已建 CodebaseMemory 图索引（建图统一用 full 模式；2026-09-23 实测 moderate 比 full 少约 44% 节点、削减几乎全在 Variable 面，graph 会被静默削薄而 index_status 仍报 ready；mode 只在从零解析时生效，改档须删条目重建）。「trace before edit」用 `search_graph` / `trace_path` / `get_architecture` / `search_code` 等图工具，project 参数填下表项目名全称；查不到再降级 Grep。

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

agent 的散文产出用简体中文写成通顺、完整的句子；领域常用英文术语（skill 名、门名、git/CLI 命令、track/gate/review 类高频词）可保留。内部标识符首次出现必须带一句实质说明；向 human 提问或汇报时不裸甩编号。机器校验的结构字段保持精确格式；证据引用保持逐字精确。散文通顺、引用精确，两者不冲突。禁止读者需自猜的拼接缩写。散文禁用「齿」——验收测试 case 就说「验收测试 case」（或「验收断言」），不借黑话省字（human 2026-09-29 明示原话：「以后不要用齿，验收测试 case 就说验收测试 case」）。本条只管散文：机械面标识符（如 verify-gates 内部字段名）不动；存量文档不专项清扫，触碰时顺手改写。
