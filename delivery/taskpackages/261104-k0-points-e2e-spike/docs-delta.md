# docs-delta — 261104-k0-points-e2e-spike

> 本件 = integrator 制品面（delivery/taskpackages/** 授权内）。spec 面（`docs/requirements.md` /
> `docs/architecture.md`）integrator 无写权（ARQ-260929-docs-sync-spec-handoff）。本包判定 =
> **docsSyncRequired=false 豁免成立：spec/契约面零变更、零新增文档债**。逐面判定见下。

## 1. docs-sync 状态：豁免（docsSyncRequired=false，docsOwner=integrator）

taskpackage `docsSyncReason`：「spike 消费的 display、薄意图和分页契约已 landed；本包产实现与
spike 证据，不重写上游语义。」docs-sync 闸绿（verify.mjs 2026-11-04T23:52Z 第 7 闸
`ok:true`，reason 原文同上）。

## 2. 组件级登记判断：architecture.md 已齐，无需追记

本包新增组件 `server/gateway/points.py` 的组件级登记**已在上游 261103-star-points-story-cover
架构批（261103-arch-review-star-points-story-cover，ADR-018）随批落进 `docs/architecture.md`**：
§5.1.2 points.py 组件行（「积分账本与目标配置 | server/gateway/points.py | 唯一确定性
writer」）、§5.2 场景⑥、display_schema 9-view 白名单 + 积分 shape floor + 来源门禁行、
front_app 积分面行、view_router 9 view 行（text/home/control/char/schedule/story_list/
story_text/points_home/points_ledger）、NFR-PERF-4 链路行。本包实现即按该已冻结架构落位，
架构面无本包引入的新决策、无漂移——一行组件级登记**不缺位，不追记**（integrator 也无
spec 面写权）。

注记一处非漂移的措辞差：architecture.md §5.1.2 points.py 行写「以进程间锁串行执行」——那是
K1（261104-k1-points-ledger-core）的正式化形态；K0 spike 落的是进程内锁 + tmp+replace 最小
可靠写（points.py 模块注记与 taskpackage crossPackageBehaviors 均已显式声明该升级归属）。
上游 spec 描述的是目标态、非本包实现引入的漂移，不改 spec、不改实现，K1 在同一文件上补齐。

## 3. 明确未改的 spec / 契约面

- **契约面零触碰**：`docs/contracts/**` 无 diff。本包消费 §1.3⑦⑧ / §1.4 / §1.5 / §四 的
  2026-11-04 已 landed 形状（contractRefs 五锚只作引用锚）；契约 lint 闸绿（唯一 warning 为
  存量断链，与本包无关）。
- **需求面零变更**：`docs/requirements.md` 无 diff——本包交付 S-F6.2/S-F6.3/S-F6.4 最小通路，
  不改产品语义。
- **架构面零变更**：`docs/architecture.md` 无 diff（§2 判断）；`adrRequired=false`，无新 ADR。

## 4. 本批实改的文档（实现面随批文档，非 spec 面）

- `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`：新增「积分（星星）」节
  （add_points/redeem_points 薄意图纪律、导航薄载荷 `{}`、不产余额/revision 禁令锚、
  不完整句不落账）+ view 白名单表加 points 两行 + 硬约束句同步（九 view 措辞）。
  该件在本包 changeManifest 内，属实现面随批文档（K0PTS-A3 断言锚），非 spec 面追记。

## 5. 零触碰面

- `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/debt/**`（本包
  未挂 debt 条目）、`.claude/**`（无治理面变更）、K0a 的 render_node/keybar/story_page 文件
  与 `firmware/main/test/host_app/main/CMakeLists.txt`（LP8 共享清单，K0a merged 前 不碰）。

## 6. 留痕锚

- 范围圈定清单：`delivery/taskpackages/261104-k0-points-e2e-spike/test-scope-final.txt`
  （final 验收命令与逐字末行、圈定判定均记于该件；review-packet 记该路径一行）。
