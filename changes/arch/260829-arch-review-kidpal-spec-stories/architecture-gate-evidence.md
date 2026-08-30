# 契约门证据台账 · 260829-arch-review-kidpal-spec-stories(T2 story_list.total)

> 骨架依据 `.claude/references/contract-change-evidence.md`。本台账证明：审了什么、批了什么、改了什么、跑了什么验证。

## 1. Gate Scope（门范围）
- **变更面（唯一）**：`docs/contracts/interface-contract.md` §1.3 ③ `story_list` 的 `data` 新增可选整数字段 `total`。
- **显式 out-of-scope**：契约其余所有 view（char/schedule/story_text/text/home/control）、B1 课表 MD、提醒三、所有既有字段——零改动。
- **owner**：human(DavidFan)签字;主线程落地。

## 2. Inputs Reviewed（评审输入）
- `changes/arch/260829-arch-review-kidpal-spec-stories/PROPOSAL.md`(§2-T2 提议 diff、§4 OQ-2′ 裁定)
- `changes/arch/260829-arch-review-kidpal-spec-stories/CONTRACT-CHANGE-PACKET.md`(architect-reviewer 四分类预审)
- `docs/contracts/interface-contract.md`(改前 §1.3 ③ 现状:仅 items)
- `docs/kidpal/spec.md` §4(S-F2.1 计数器 ×N / S-F2.4 第 N 颗——total 的需求源)

## 3. Architect-Reviewer Classification（四分类预审）
| 变更面 | 分类 | 理由(详 CONTRACT-CHANGE-PACKET.md) |
|---|---|---|
| `story_list.data` + `total` | **CONTRACT_CHANGE_REQUIRED** | 契约现状无承载故事总数的字段;`items.length` 在 items 分页/截断时与全集计数漂移,不能作正式数据源,只能回退。属定义层缺口,需契约变更。 |

## 4. Human / Architecture Decision（人类契约门）
- **裁者**:human(DavidFan);**日期**:2026-08-29;**状态**:`HUMAN_APPROVED`。
- **批准的精确问题**:「是否批准 `interface-contract.md` §1.3 ③ `story_list.data` 新增 additive 可选整数 `total`(=孩子已录故事总数/故事果计数,云端算下发,板子渲染 P3 计数器与盖章帧,缺省回退 items.length)?除该字段外契约零改动。」
- **答复**:批准加 total 字段。

## 5. Applied Diff Inventory(实际落地清单)
| 文件 | 符号/字段 | additive/breaking | 迁移/数据风险 | 文档同步 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `story_list.data.total`(整数,可选) | **additive**,零删改、零 breaking | 无(新可选字段,旧固件不读则回退 items.length) | spec S-F2.1/S-F2.4 已引用;契约内加变更记录行 |
- 批量结构闸同步:本仓暂无扫描 `contracts/**` diff 的批量闸(原仓 SANCTIONED_CONTRACT_CHANGES 未迁),无需白名单同步。

## 6. Closeout Conditions(闭环条件)
- 无未决条件。`total` 为 additive 可选,无强制迁移。

## 7. Validation Evidence(验证证据)
| 验证 | 命令/方法 | 预期 | 实际 |
|---|---|---|---|
| 契约 diff 仅 story_list 一处 | `git diff docs/contracts/interface-contract.md` | 只增 `total` 相关行 | ✓ 实际 diff 仅 +total 字段 +字段说明 +变更记录(2026-08-29 实测) |
| story_list 含 total 样例 JSON 合法 | `node -e JSON.parse(...)` | 解析通过 | ✓ total=3 合法(2026-08-29 实测) |
| story 锚点真实性(关联 spec 侧) | 逐个 grep 设计稿 | 全命中 | ✓ 19/19(V1 复验) |

## 8. Docs / Queue Sync(文档/队列同步)
- spec §4 S-F2.1/S-F2.4:已含果子计数 story(以 total 为数据源)。
- CDQ:登记 `docs/CONTRACT-DECISION-QUEUE.md`(本次新建,见 CDQ-260829-storylist-total)。
- ADR:本变更为单字段 additive,未改持久策略/语义,不出 ADR。

## 9. Rollback / Follow-up(回滚/后续)
- **回滚**:`git` 还原 `interface-contract.md` 该处 diff 即可(additive,无数据/实现耦合)。
- **后续**:OpenClaw 生成 story_list 时实算 total(实现期);板子固件读 total 渲染果子计数(实现期,留 dev-loop)。

## 10. Verdict(判定)
- **门状态:CLOSED / 已结**(human 契约门签字 ✓ + diff 已落地 ✓ + 验证证据在案 ✓ + CDQ 已登记 ✓)。
- dev 可依据带 `total` 的 story_list 契约进行实现。CDQ 行可移「已结」。
