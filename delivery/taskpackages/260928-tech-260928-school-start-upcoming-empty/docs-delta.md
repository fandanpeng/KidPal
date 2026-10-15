# docs-delta — TaskPackage 260928-tech-260928-school-start-upcoming-empty（subsystem: server，周中 upcoming 恒空修复）

> 读者是人（supervisor / human / 后续 wrap-up）。本包 docsOwner = integrator，本文记录
> 「docs-sync 已完成/已确认无漂移」部分，以及「有漂移但 integrator 不直改、路由 planner /
> 主线程」的建议。

## 1. 变更摘要

修复周一~周五 P1 首页「接下来」组与「明早」预告恒空（TECH-260928，代码半）：意图展开路径
（`server/gateway/front/front_app.py` 原 :642）取 `home_upcoming_payload` 时已补传
`_schedule_school_start()`，但建连推（原 :685）与 refresh 推（原 :708）两处调用漏传同参，
导致非假期日 school_start 侧 upcoming 恒为空。本次两处调用与意图展开路径同参对齐，补传
`_schedule_school_start()`，行为归一。

## 2. 已改（本包 docs-sync，integrator 直接执行）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| `docs/crq/tech/TECH-260928-school-start-upcoming-empty.md` | 已由 crq-render.mjs 归位到 `docs/crq/tech/_archive/2026/`，INDEX 重渲染 | status `blocker` → `closed`（closed: 2026-09-28，closedBy: dev-loop integrator）+ append 收口记录 | 代码半修绿、运维半移交留痕，条目生命周期完结。 |
| `changes/dev/260928-tech-260928-school-start-upcoming-empty/*` | 新增 | `docs-delta.md` + `review-packet.md` | 本包 closeout/audit blocking 制品（closeout 制品 SSOT 表）。 |

## 3. 建议 planner / 主线程（integrator 出范围不直改）

- **运维半移交项（不属本包代码面，已在 TECH 条目收口记录留痕）**：ECS 侧需将
  `server/deploy/openclaw-schedule.config.yaml` 部署到 `KIDPAL_SCHEDULE_CONFIG` 指向路径，
  并给 `kidpal-front.service` 加 `Environment=KIDPAL_SCHEDULE_CONFIG`。落位即生效、无需重启。
  **运维半落地前现网行为不变（依旧空 upcoming）**——本包代码修复只有在该配置在位时才在
  现网生效。此为部署/运维面动作，建议路由 planner / 主线程按既有 ECS 运维通道跟进。

## 4. 无漂移确认

- **零触 `docs/contracts/**`**：本包为既有 payload 组装链的接线回归（补传既有参数），不改
  跨边界 wire format，闸 A architect-reviewer 四分类 ④ 无需改（NO_CONTRACT_CHANGE）。
- 无 spec 面（`docs/requirements.md` / `docs/architecture.md`）措辞变更需求：upcoming 行为
  本身即 spec 预期，本次只是让实现回到预期，无需更新 canonical docs。
- 无新增 `docs/` 根文件、无影子目录；本包文档产物仅落 CRQ 条目归档 +
  `changes/dev/260928-tech-260928-school-start-upcoming-empty/`，不散写。
