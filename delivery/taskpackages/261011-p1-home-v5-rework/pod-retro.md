# pod-retro · 261011-p1-home-v5-rework（第一人称热记忆）

> 标签：第一人称热记忆——planner 全程在场的主观过程体感，2026-10-12 收尾相位写（integrator closeout 后、supervisor 闸前）。不复述、不冠权威 tokens/gates/rounds/verdict/test results——那些以 machine artifacts / pod-events / verifier-evidence 为权威。

## 本次跑法与体感

这是我的第一次「429 中断续棒」：开场勘盘（launch-card + spine + pod-events 全量 + git diff 现状）对上 851eb481 中断点清单后，续做派发几乎无痛——builder 零 redo 直接接上，中断清单把「已落盘/未完成」分得很清，这是本次最值钱的经验：**中断升级事件里带逐项中断点清单，是续棒安全的关键投资**。

## 卡点与绕轮原因的主观判断

1. **acceptance id 错位（scoped round 1 唯一红）是我 finalize 面的账没收干净**：intake 期我起的是 ACC-1..ACC-5 本地编号，acceptance-author 冻结时用了 HV5-* 在世名，两套名字的映射只在 acceptance 串括号里——coverage 闸按短 id 字面匹配就全缺了。教训：Step 8 finalize 时该把「冻结件实际在用 id」回填对齐，而不是只确认文件在场。修法零触碰冻结件（id 改判 + 原编留痕），五分钟收口，但本可避免。
2. **CDQ 行落盘撞了编制真空**：integrator 拦一次、我代落又拦一次，才发现 scope-map 根本没有任何登记角色能写 docs/CONTRACT-DECISION-QUEUE.md（integrator 白名单未含、我 deny docs/**）。这是 scope-map 与契约收口义务清单的历史错配——以前 CDQ 行多半是档 1 主线程一次性落地写的。升级 930257e5 已留痕，行全文在 review-packet 里逐字保存，不丢。下次起包遇到「收口义务清单」时应先对 scope-map 逐文件核写权（本次 1c 只核了 builder/acceptance 面，漏了 integrator 的收口面）。
3. **母版三处同步提前升级是对的**：fe6c3f46 在起 Pod 期就把「无执行角色」暴露掉，human 裁 B 主线程直落后完全不占 Pod 时间。若拖到集成夜才撞，就是另一种卡法。

## 别扭的验收 / 钩子体感

- mutation 取证被 `-Werror=unused-const-variable` 逼出 `if(0){}` 死代码守卫形态——行为变异本该是纯行为的事，编译器告警让它变形。builder 建议话术常量收数组或加 `(void)` 惯例，我同意，验收设计时值得采纳。
- 树级字面扫描齿（「今天接下来没有安排啦」在世齿）扫整棵树，验收件自己的消息文本就能满足它——弱锚。builder 观察第 5 条我完全共鸣：源字面在世齿应限定扫描面。

## 人工纠偏体感

human 两裁（fe6c3f46 母版直落 + b7e2d94a 续跑姿势）都给得干脆且落了 pod-events，续棒裁定「不 redo 已落盘半成品」一句救了整轮——没有它，新 builder 最自然的动作就是 reset 重来。

## escalation themes（post-cutoff，交叉引用 pod-events）

- fe6c3f46（母版同步无执行角色）→ b7e2d94a human 裁 B 主线程直落，已闭合。
- 851eb481（429 中断点清单）→ b7e2d94a 续跑裁定 + 本次会话重派续做，已闭合（d650d56e）。
- 930257e5（CDQ 行无落盘角色）→ **pending human**——待裁：主线程直落 / 扩 scope-map / 留 pending。真机看屏轮、ECS front 部署、riskTier=high 合并门同属 human 前置，均已在 review-packet 未覆盖面申报。

## 下次改进

1. 起包 checklist 加一条：收口义务清单（CDQ/ADR/crq 记账类）逐文件对 scope-map 核写权，无主即提前升级，不留到集成夜。
2. finalize 对齐「冻结件在用 id ↔ TaskPackage acceptance id」，coverage 闸前置单跑一遍再烧 verifier 轮。
3. 中断升级事件模板固化为「已落盘逐项 + 未完成逐项 + 盘面实证指引」三段（851eb481 形态）。
