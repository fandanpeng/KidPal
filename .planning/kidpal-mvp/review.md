# Review · kidpal-mvp 开发包拆分 PROPOSAL（DRAFT v0.1）

> **review 性质**：dev 侧分流单轮 review（`.claude/references/change-triage.md` §3，非对抗红队）。
> **reviewer**：architect（子 agent，只读+审，写域零扩张——本件由主线程依 architect 同会话 return 落盘）。
> **被审件**：`.planning/kidpal-mvp/PROPOSAL.md` DRAFT v0.1（2026-08-29）。
> **交叉核对源**：`docs/spec.md`、`docs/contracts/interface-contract.md`、`.claude/references/task-intake-and-slicing.md`。

---

## 总结论

**GREEN（可盖 FROZEN 交 portfolio-planner 拆包）——附 7 条建议级修正，无 BLOCKING。**

11 包拆分合法（全垂直切片、无横切）、依赖无环且 mergeOrder 拓扑一致、story 覆盖完备（含 §4.0 共享底座与 §6 接口，S-F2.5 合法留根）、契约面只消费不改。三段式测试策略与包 0 公共前置设计扎实；§7 对 ESP-IDF linux target 兼容性的诚实标注尤其好（把不确定性显式化为包 0 验收而非预判结论）。

全部 7 条 CONFIRMED 均为**建议级**（文本/标注/rationale 层），无一影响拆分骨架正确性，下游 verifier 验收门接得住。

## 维度结论速览

| 维度 | 结论 |
|---|---|
| 1 拆分合法性（垂直切片） | ✅ 全 11 包合法，包 0 符合 L0/internal-service 例外 |
| 2 依赖与 merge order | ✅ 无环、拓扑一致；2 条建议（关键路径收口/并行集表述） |
| 3 riskTier 标定 | ⚠️ 2 条建议（9a critical 误标 / 6/8 medium 张力） |
| 4 story 覆盖完备性 | ✅ 全覆盖；1 条建议（4 页循环跨页整合归属） |
| 5 契约面 | ✅ 只消费不改；1 条建议（contractBlocked 漏列 2/6） |
| 6 测试策略 | ✅ 三段式自洽；1 条建议（testHost 口径 + 硬件子能力殿后标注） |

## Findings 明细（7 条建议级）

| # | 维度 | 结论 | 修正项 | 建议处置 |
|---|---|---|---|---|
| F2-2 | 依赖 | CONFIRMED | 关键路径收口应是 8（mergeOrder 10）非 7 | 主线程回灌改表述 |
| F2-3 | 依赖 | CONFIRMED | 调度并行集把 4/5 提前（4/5 需 2 先就绪） | 主线程回灌 |
| F3-1 | riskTier | CONFIRMED | 9a 标 critical 不符 §riskTier 定义 | **回问 human**：改 high，或明示破格 + rationale |
| F3-2 | riskTier | CONFIRMED | 6/8 标 medium 与 cross-system=high 有张力 | **回问 human**：补 rationale 或升 high |
| F4-2 | 覆盖 | CONFIRMED | 4 页循环翻页跨页整合归属不明（9b 兜底） | 显式声明兜底或加整合验收点 |
| F5-2 | 契约 | CONFIRMED | §4 contractBlocked 漏列包 2/6（§7 有） | 主线程回灌补全 |
| F6-2 | 测试 | CONFIRMED | testHost §3/§2 口径不一 + 硬件子能力未标殿后 | 主线程回灌统一 + 标注 |

### F3-1 详述（riskTier 误标，回问 human）
`task-intake-and-slicing.md:150` 的 critical 触发条件 = 「`docs/contracts/**`、migrations、data deletion、irreversible data movement」——9a（AEC 声学喊停实测调阈值）一条都不触。9a 触的是 high 条件（`:151` state machines with user impact + 音频安全相关）。「最大技术风险」是实现不确定性高，不等于 riskTier 的 critical 档——riskTier 按爆炸半径/不可逆性分级，不按技术难度。标 critical 会给 9a 挂上不匹配的最重闸（契约门级审查），拖慢一个本就需反复实测返工的包。

### F3-2 详述（6/8 riskTier 张力，回问 human）
6（课表页）与 8（故事）都跨板+云两面，按 `:151`「cross-system behavior = high」字面应 high。可辩护点：二者只消费已冻结契约 + 渲染，无契约改动、无迁移、无 AI 成本、无重状态机，blast radius 有界——落 `:152` medium。与包 2/4（也跨板+云但标 high，因含 AI 音频链路 / D3 笔顺渲染复杂度）的区分可辩护，但 PROPOSAL 未写明 rationale。

### 其余 5 条（文本级，主线程回灌）
- **F2-2**：§3 关键路径 `…→7→9b` 与 mergeOrder 8(10)>7(9) 矛盾；9b 真正等的最晚 merge 包是 8。
- **F2-3**：§3 调度说「0/1 就绪后包 3/4/5/6/8 可并行」，但 4/5 需 2 先就绪；0/1 就绪后立即可并行的只有 3/6/8（及 2）。真正最长链是 `S→{0|1}→2→{4|5}→9b→9a`。
- **F4-2**：§7 决策 14「4 页循环 BOOT 翻页」的跨页端到端整合，host 阶段无包认领，靠 9b 兜底——建议显式声明。
- **F5-2**：§4 contractBlocked 只列包 7，但 §7 明确契约待确认点 1/2/3 影响包 2/6/7——§4 应补列 2/6 或统一指 §7。
- **F6-2**：§3 简化「3/4/5/6/7/8=host」与 §2 表（5 含 SDL、6/8 含 PC 云端）口径不一；S-F2.2 录音采集、S-F1.1 RTC/SHTC3 直读是硬件强绑定子能力，应显式标殿后 9b。

## sign-off

architect（单轮 review）2026-08-29：GREEN，附 7 条建议级修正。F3-1/F3-2 触 riskTier 判断，按从严解释回问 human；其余 5 条文本级可由主线程回灌后冻结。human 盖 FROZEN 时建议钉标准 pin（`sha1:<16hex>`，raw-content 行尾归一化算法，见 task-intake-and-slicing.md:53）。

---

# Review · kidpal-mvp 开发包拆分 PROPOSAL（DRAFT v0.3 修订稿）

> **review 性质**：dev 侧分流单轮 review（`.claude/references/change-triage.md` §3，非对抗红队）。
> **reviewer**：architect（子 agent，只读＋审，写域零扩张——本件由主线程依本 return 落盘）。
> **被审件**：`.planning/kidpal-mvp/PROPOSAL-v0.3-draft.md`（DRAFT v0.3，全文重写，独立修订稿，不改 FROZEN v0.2 正文）。
> **交叉核对源**：`docs/spec.md`、`docs/contracts/interface-contract.md`、`.claude/references/task-intake-and-slicing.md`、FROZEN `.planning/kidpal-mvp/PROPOSAL.md`、`.planning/kidpal-mvp/review.md`（v0.2 记录）、`.planning/kidpal-mvp/progress.md`、`docs/notes/validation-batch-conclusion.md`、`docs/ARCH-REVIEW-QUEUE.md`、`server/shim.py`、`.claude/scripts/portfolio-reconcile.mjs`。

## 总结论

**GREEN（可交 human 盖 FROZEN）——附 10 条建议级修正，无 BLOCKING，无一项需回问 human。**

13 包拆分合法（全垂直切片、无横切；新包 U 成立为合法切片）、依赖 DAG 无环且 mergeOrder 拓扑一致（关键路径 7 级成立）、riskTier 全部稳定（U 标 high 站得住）、story 覆盖完备（含 §4.0 共享底座 / §6 接口 / §7 已定决策 / §5.6–5.7 UI 美术层）、契约面只消费不改（两个前置闸标注正确、contractBlocked 解除/维持符合验证批事实）、三段式测试策略自洽。全部 10 条 CONFIRMED 均为文本级/标注级修正，无一影响拆分骨架正确性。

**v0.2 连续性核对**：v0.2 的 7 条 findings 已全部落实于本稿正文；v0.2 冻结裁决（WIP=3、9a/6/8 tier、冒烟批、切分轴、硬件殿后粒度）忠实继承，无回退。

## 维度结论速览

| 维度 | 结论 |
|---|---|
| 1 拆分合法性（垂直切片） | ✅ 全 13 包合法；新包 U 成立（跨「板＋tools」是共享资源层切片、非技术层横切） |
| 2 依赖与 merge order | ✅ 无环、拓扑一致、关键路径 7 级成立；3 条表述精度修正 |
| 3 riskTier 标定 | ✅ 全部稳定（U 标 high 站得住）；1 条建议补 U 的 high rationale |
| 4 story 覆盖完备性 | ✅ 全覆盖；1 条建议补 decisionCoverage 映射两点 |
| 5 契约面 | ✅ 只消费不改；闸 A/B 标注正确；2 条引注精度 + 1 条 spec §528 错引 |
| 6 测试策略 | ✅ 三段式自洽；2 条建议（包 4 云侧 testHost、U 换皮后 host 回归归属） |

## Findings 明细（10 条，v0.3 系列新编号 F1–F10）

> 编号说明：v0.3 重新编号 F1–F10（v0.2 的 `F<dim>-<n>` 已全部落实、编号语义作废）。本批无回问 human 项。

| # | 维度 | 结论 | 修正项 | 处置 |
|---|---|---|---|---|
| F1 | 1 拆分合法性 | CONFIRMED | 新包 U 成立为合法切片；补切片性质 rationale | 主线程回灌 |
| F2 | 2 依赖 | CONFIRMED | U 等待集表述漏包 7（经 6 亦达 level 4） | 主线程回灌 |
| F3 | 2 依赖 | CONFIRMED | 「9b dependsOn 2~8 + U（全部 B 段功能包）」不含包 0，文字与语义不符 | 主线程回灌 |
| F4 | 3 riskTier | CONFIRMED | U 标 high 站得住，但 rationale 未写明、与 F3-2 口径不同源 | 主线程回灌 |
| F5 | 4 覆盖 | CONFIRMED | decisionCoverage 补两点：§7 决策 17 显式进 U；§5.6 教学字体红线显式认领 | 主线程回灌 |
| F6-1 | 5 契约面 | CONFIRMED | 闸 B 引注「契约 §九」——现契约无 §九 节 | 主线程回灌 |
| F6-2 | 5 契约面 | CONFIRMED | 闸 A 引注「生成 char 块」非字面 | 主线程回灌 |
| F7 | 6 测试策略 | CONFIRMED | 包 4 云侧（C1 查表拼装＋预热＋延迟验收）缺 testHost 标注 | 主线程回灌 |
| F8 | 6 测试策略 | CONFIRMED | U 换皮后的页面 host 回归归属未写明 | 主线程回灌 |
| F9 | 5 引注 | CONFIRMED | 留根「spec §528」——spec 无 §528，应为 §7 决策 1 | 主线程回灌 |
| F10 | 2 依赖 | CONFIRMED | 包 1 dependsOn S 从产物依赖变验证序依赖，性质变化需文本说明 | 主线程回灌 |

### 关键详述（择要）

- **F1（U 切片合法性）**：U 承载「美术资产系统」这一完整可独立验证能力，非技术层横切；页面包不 dependsOn U、先占位资产跑绿，U 作迁移批殿后、9b 作整合复验——满足「中间包可独立验证 + 声明 blocking edges」。
- **F2（U 等待集）**：按依赖深度 4（S→1→2→4）、5、7（S→1→6→7）都达 level 4，6/8 是 level 3；U 最深前驱集 = {4,5,7}，非「4/5 之一 + 8」。
- **F4（U=high）**：不触 critical、够不上 medium；落 high 理由 = blast radius 覆盖全 UI 面 + SPIFFS 分区级改动 + 真机强绑定 + 9b last gate。与 6/8（板＋云跨系统）不同源但不矛盾。
- **F5（覆盖）**：§7 决策 17 应显式进 U 承载项；§5.6 教学字体红线由包 4（渲染侧，主）+ U（字体资产侧）共担。
- **F7（包 4 云侧 testHost）**：查表拼装走 PC 级单测；char 延迟验收对真 ECS/OpenClaw、按 ARQ-260901-g3-server-live-acceptance 标 live（不 auto-run）。
- **F9（引注）**：spec §528 为错引，应为 spec §7 决策 1（「顶层定位：所有数据/逻辑/记忆在 OpenClaw」）。

## 特别盯新东西的三项结论（复核）

1. **C1 落进包 4 云侧守薄桥红线**：✅ 守住——静态字库挂 OpenClaw 侧、绝不入 shim；makemeahanzi 数据源本就在服务器侧。
2. **包 S 缩域后「前置通行证」定位**：✅ 成立——复用 `server/shim.py`（核验存在、已并入 main）后 S 只做设备侧闭环，仍完整证明三层链路；无 dependsOn 正确。
3. **包 1 起点前移与 S 缩域自洽**：✅ 自洽——dependsOn S 转验证序依赖仍成立（F10）。

## sign-off

architect（单轮 dev-side review）2026-09-01：**GREEN，附 10 条建议级修正（全文本级，主线程回灌即可，无回问 human 项）**。U 切片合法性、U=high、包 1 dependsOn S、C1 薄桥红线四项均确认判断（非打回）。主线程回灌后可盖 FROZEN；盖时钉标准 pin（`sha1:<16hex>`），并同步 §6 的 12→13 decision-coverage 映射（含 F5 两点）与 §8 评审记录。
