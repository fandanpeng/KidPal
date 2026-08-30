# Review · kidpal-mvp 开发包拆分 PROPOSAL（DRAFT v0.1）

> **review 性质**：dev 侧分流单轮 review（`.claude/references/change-triage.md` §3，非对抗红队）。
> **reviewer**：architect（子 agent，只读+审，写域零扩张——本件由主线程依 architect 同会话 return 落盘）。
> **被审件**：`.planning/kidpal-mvp/PROPOSAL.md` DRAFT v0.1（2026-08-29）。
> **交叉核对源**：`docs/kidpal/spec.md`、`docs/contracts/interface-contract.md`、`.claude/references/task-intake-and-slicing.md`。

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
