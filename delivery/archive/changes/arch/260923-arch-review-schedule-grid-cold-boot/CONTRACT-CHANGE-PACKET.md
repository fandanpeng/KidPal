# CONTRACT-CHANGE-PACKET · 260923-arch-review-schedule-grid-cold-boot

> Tier-1 契约变更包 · §1.3② schedule view（`data.restore` 恢复性推送语义）。状态：**已批准**——human 契约门 2026-09-23 AskUserQuestion「批准（C1 挂账）」。与 spec 改动同 PR 原子落地（落地 diff 见同目录 landing-manifest.md L7 + landing-verdict.md）。

## 基本信息

- **四分类**：CONTRACT_CHANGE_REQUIRED——**随 spec 改动携带（同 PR 原子落地）**（architect-reviewer 预审 2026-09-23）。
- **Tier / 面**：Tier-1，E1 display 契约 §1.3②（跨系统直接消费者：front 生产 / 设备路由消费）。
- **变更类型**：additive 可选字段（`data.restore`）+ **modifying 既有板子行为条款**（L85 无条件接管句逐字改写带例外）+ 新增生产纪律条款。对不置 restore 的既有发送方向向后兼容，但既有条款文本被修改——**如实记 modifying 分量，不标纯 additive**。
- **动机**：spec 修订（S-F1.2 冷启动恢复）随携带；ADR-005；对抗 review 全制品见同目录。

## Proposed Contract Diff（已落地，逐字）

1. **modifying L85 板子行为句**：在「切到 P2 课表页渲染一周总览网格（…今天列整列高亮）」后插「；`data.restore: true` 时除外——只填网格缓存、不接管前台，P2 在屏时仅重绘本页」。
2. **新增 `data.restore` bullet**（「今天」列 bullet 之后）：additive 可选布尔，缺省（absent）或显式 `false` 时设备行为完全不变；`true` = 恢复性载荷，只填网格缓存、不接管前台。降级双面注记：restore 载荷板端入板后解析失败 = 板端静默降级；front 校验面 V27 在入板前独立生效——两层降级互不替代。
3. **新增生产纪律句**：仅 front 后台附带推送路径可置 `restore`（触发时机属行为策略，权威承载 = arch §5.2 / ADR-005，不进契约）；意图薄载荷展开（`expand_schedule_intent_payload`）恒不置。
4. **契约变更记录行**：2026-09-23 记录，随 `260923-m3-k7-schedule-grid-restore` 同 PR 落地。

## Gate Scope（out-of-scope 确认零触碰）

本 gate 只动 §1.3②（L85 板子行为句 + restore bullet + 生产纪律句 + 契约变更记录行）。确认不改：§1.3⑥ home（`data.upcoming` 语义零变化）、§1.5 网关职责（restore 载荷经既有 mcp display 通道与既有 data shape 校验缝）、§1.6 system prompt（restore 与 LLM 无关）、§2.3 B1 解析规则、§四端云协议剖面与提醒面（无新消息类型）。

## Classification（架构 gate evidence 行）

- 触发来源：260923-arch-review-schedule-grid-cold-boot（Track A spec profile，full-adversarial，r1 收口）；F-C14 处置确认「推送时机不进契约」边界延续（2026-09-21 裁定口径）。
- Human Decision：① F-C5 拍板 = accept-as-risk（ADR-005 残余风险记录，采 defender 方案，2026-09-23）；② 契约门 = 批准四件 diff（C1 挂账，2026-09-23）。
- PR 编组：契约 + requirements S-F1.2/NFR-OFF-1 + arch §5.2/§6.2/追踪表 + ADR-005 同 commit 原子落地；landing-verifier GREEN（landing-verdict.md，9/9 PASS）。

## Closeout Conditions

- **C1**：builder 实测复核确认旧固件对 `data.restore` 载荷的确定性接管行为（仓内代码已定案：`schedule_page.c:607-613` data 内未知键忽略 + `mcp_server.cc:620-644` 无条件 begin_page——复核确认而非定案）之前，**ECS 推送开关保持默认关**。owner = 实现包 builder / integrator；required evidence = 旧固件行为复核实测记录 + 开关状态留痕 + 烧板验证后开闸记录。
- C1 生命周期语义（永续配置 vs 一次性闸）随烧板推进定案（review-packet §8 残余风险②）。

## Validation

- 契约 diff 逐字人工核对（本仓无 schema 校验器，contracts rule 注记口径；pass-oracle 机械门脚本不在场 = ARQ-260827-contract-mech-gate-gap 降级人工，如实记录）。
- landing-verifier 独立核验：`landing-verdict.md` GREEN——L7 单 hunk 4+/1-、L85 旧句全仓零残留、CRLF 完好（`git diff --check` 干净）。

## Docs Sync

- spec：requirements S-F1.2 补充规则句（L1）+ NFR-OFF-1 注记（L2）；arch §5.2 场景①（L3）+ 追踪表 NFR-OFF-1 行（L4）+ §6.2 澄清（L5）；ADR-005 append（L6）。

## Verdict

**APPROVED（C1 挂账）**——human 契约门 2026-09-23；落地已由 landing-verifier 独立核验 GREEN。Gate evidence 骨架对齐 `.claude/references/contract-change-evidence.md`（本 packet 即 consolidation 载体；validation 命令 = git diff 逐字比对，已在 landing-verdict 留痕）。
