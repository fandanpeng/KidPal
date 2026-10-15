# docs-delta —— 261010-p3-storylist-cold-start-restore

> blocking closeout 制品。integrator 落笔（2026-10-12 制品轮）。契约权威原文 = 包内 `contract-gate/CONTRACT-CHANGE-PACKET.md` §2（D1–D4）；gate 收口回填 = 同目录 `contract-gate/architecture-gate-evidence.md` §5/§7/§8/§10。

## 文档面改动清单（本批全部落点）

| 面 | 文件 | 变更 | 性质 / 授权 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` §1.3③ + §1.5 第 3 条 | 四触点（D1 `data.restore` 字段定义 + 生产纪律两 bullet 插入 / D2 板子行为句 restore 例外子句 modifying / D3 §1.5 floor 枚举补 restore 可选布尔 / D4 变更记录行 append） | 契约门通道（CCP §2 逐字 git apply，HUMAN_APPROVED 2026-10-12，先批后落随包同 PR）；逐字核对留痕 = gate evidence §5（五条净内容行程序 needle 逐字 HIT + 实测 5 added / 2 removed；CRLF 保全 = 落地后 282 行全 CRLF、0 裸 LF 字节级实证） |
| ADR | `docs/ADR.md` | 追加 **ADR-010**（P3 story_list 冷启动恢复 = 「数据面更新 vs 前台接管分离」第三实例——决策/两点差异/理由/被否备选三项/泛化立场履行/关联六节；备选自 CCP §3） | taskpackage `adrRequired=true`（CCP §7 建议，durable semantics）；append-only，不覆盖 ADR-001..009 |
| crq/tech | `docs/crq/tech/TECH-261010-p3-storylist-cold-start-empty.md` | 收口 closed（frontmatter `status: closed` + `closed: 2026-10-12` + `closedBy` = integrator closeout；正文 append「收口」节——修法三件全落 + 契约随批 + 残余 = SL-A5 真机轮与 C1 开闸归 human） | 条目正文 append-only；渲染脚本归档归位 |
| crq 索引 | `docs/crq/tech/INDEX.md` + `docs/crq/tech/_archive/2026/INDEX.md` | `crq-render.mjs` 再生（TECH-261010 归档 `_archive/2026/`，两 INDEX 更新） | 生成件，脚本再生、未手改 |
| gate evidence | `delivery/taskpackages/261010-p3-storylist-cold-start-restore/contract-gate/architecture-gate-evidence.md` | closeout 回填（Status 翻 LANDED·CLOSED + §5 Applied Diff Inventory 实测 + §7 Validation Evidence 四项 + §8 census/queue 状态 + §10 Verdict CLOSED（C1 保持 pending 归 human）） | 十节骨架 closeout 回填义务 |
| spec 面·架构 | `docs/architecture.md` :186 / :214 | 「随契约门落、尚未落地」挂法句翻既成——**落盘 pending-planner**：integrator Edit 被 enforce-scope 拦截（scope-map integrator allow 面不含 docs/architecture.md，机械闸优先未绕行）；两处最小 patch 全文已拟（见 review-packet「交 planner 代办」） | docsSyncReason 挂法句翻既成义务；最小改动只翻挂法、零新增语义 |
| 契约队列 | `docs/CONTRACT-DECISION-QUEUE.md` | `CDQ-261010-p3-storylist-cold-start-restore` LANDED 行 append——**落盘 pending-planner**（同上 enforce-scope 拦截，261011 同款先例；行全文已拟） | gate evidence §8 义务 |
| spine | `delivery/changes/intake/261010-p3-storylist-cold-start.md` | anchors.contract 落地态回填 + `status: in-flight → landed`——**落盘 pending-planner**（intake/** 不在 scope-map integrator allow 面，同款拦截；回填文本已拟） | spine 收口态义务 |
| spec 面·需求 | `docs/requirements.md` | **零触碰**（v1.63 已 frozen 定稿 2026-10-12，§9 v1.63 行 = 定稿载体；本批 = 兑现 S-F2.1 AC1/AC5/AC6 + 补充规则冷启动恢复，无新增语义） | 零改申报 |

## 代码面 doc 钉行（非本件义务、随包在案备查）

- `server/gateway/display_schema.py` :116-122：story_list restore 可选布尔 floor 注记（D3 联动，builder 落）。
- `firmware/main/mcp_server.cc` story_list 分流 restore 分支 + `kidpal_render_node.cc` refresh_story / story_on_screen seam（announced seam 四件，见 taskpackage notes ⑤）。

## 零改申报汇总

- `docs/requirements.md`：零触碰（见上表）。
- `docs/contracts/**` 其余文件（interface-contract.md 之外）：零触碰；interface-contract.md 本批改动 = CCP D1–D4 批准范围逐字，零 packet 外字符（gate evidence §5）。
- `.claude/**`：零触碰。
- 核证面：本包 changeManifest 十文件即全部实现面落点（change-manifest 闸 pre-cutoff 存量放行 + 人工核对一致）；detect_changes 77 impacted 全落预期面（front_app 既有消费方函数族 / reminders hop1 / schema 消费面为 blast radius 命中、非被改动）。

## Post-cutoff 制品纪律确认

本批 post-EVENTS_SCHEMA_CUTOFF（>= 250725）：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner `pod-retro.md`（收尾相位）。`delivery/taskpackages/261010-p3-storylist-cold-start-restore/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write）；pod-events advisory 检查归 planner/supervisor、non-blocking。
