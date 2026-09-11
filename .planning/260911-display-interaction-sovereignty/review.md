---
ws: 260911-display-interaction-sovereignty
date: 2026-09-11
reviewer: architect（/triage-change 批次②，只读 + 审）
verdict: GREEN-for-triage（③档成立，PROPOSAL 可落盘待 human 冻结；不构成设计终裁）
---

# 分流 review · 设备显示与交互主权包

## Findings

F1 [BLOCKING·设计门] 设计先行约束成立：三屏主权方向是下游全部包的结构前提，
PROPOSAL 须以方案空间 + human 取舍点形态落盘，不得预支设计结论。已核实
render_node 三标志 + 1s tick 对账现状（kidpal_render_node.cc L58/L63/L75/L88）。
处置：本 PROPOSAL 已按此写成；human 裁 OQ1 前禁止拆实现包。

F2 [HIGH·逃生门] 键位漂移根因核实：板级仅 boot_button_（waveshare-s3-rlcd-4.2.cc
L45/L276），key_gesture_parser_feed 设备侧零调用（全仓 grep 仅宿主
session_logic.acceptance.c 命中）；spec §4.0/§5.3 为权威。OQ4 选 B 触 spec 修订
→ Track B，本门不批。

F3 [HIGH·契约面核查] #5 M1 初判不触契约（E1 钉结构/校验/信封，时序未钉板；
kidpal_display_blocks 为引擎→front 内部面）；必触 kidpal_display_strip.py 冻结
离线契约 → acceptance-author 修订。M3 变体与契约 §1.1「回复文本末尾」逐字冲突
→ 必须走契约门，dev-loop 内禁止自行采用。PKG-4 开工前 reviewer-first 复核。

F4 [MEDIUM] #6 与 spec §5.3「说完自动/手动结束回首页」语义模糊（现实现 auto 轮
回 Listening，application.cc L458-466）；定案冲突则 spec 修订走 Track B。

F5 [MEDIUM] PKG-4 部署硬依赖批次①（apply-xiaozhi-overlay.sh 漂移未修，先跑断
语音）；已写入 PROPOSAL §4 PKG-4 前置条件。

F6 [MEDIUM] 方向 B 波及 SetupUI 建立面 + 全部写屏路径，需 host 矩阵回归评估；
观感/字体边界已切给批次③（PROPOSAL §0 非目标）。

F7 [LOW] char 独立屏应并入 s_kidpal_scr 体系（render_node.cc L399-416 自证），
纳入 PKG-2 验收。

F8 [LOW] alert/reminder 前台优先级须进仲裁器设计（同族崩溃已修症状未修架构）。

F9 [INFO] 路线 A 可复用冻结纯逻辑；GPIO18 悬空电平真机确认列为 PKG-1 验收项。

## Sign-off

- 四档裁决：③ 大 feature（拆包），riskTier=high，理由见分流裁决（跨多切片 /
  多 Pod / 高风险 / 设计耦合四轴全中）。
- 逃生门：spec（OQ4=B / OQ3 语义钉板）与契约（M3 变体）触面已在 PROPOSAL §7
  申报并转出对应门；本门未批准任何事实源变更。
- PROPOSAL 文本（DRAFT v0.1）经本 review 通过为「可落盘待 human 冻结」状态；
  human 冻结（触点②）后方可在 portfolio-planner 拆包。
- architect 不自冻、不落地；本件只读 + 审，落盘归主线程。

## 触点①结果（主线程记录）

- human 2026-09-11 确认 ③大 feature 裁决（AskUserQuestion，选项「确认 ③大 feature」）。
- PROPOSAL 保持 freezeState: pending-human——human FROZEN（触点②）+ grilling OQ1-4
  后方可交 portfolio-planner 拆包。

## 触点②结果（主线程记录，2026-09-11）

- grilling 四 OQ 已裁（AskUserQuestion，两轮——首轮 OQ2/OQ3 human 要求解释后重裁）：
  **OQ1 = 纯 B 归一屏**（非本 review 倾向 A→B，human 直选终态；F6 波及面全额生效）、
  **OQ2 = M1 围栏闭合即发**、**OQ3 = (c) 随 OQ1=B 自然成立**、**OQ4 = A 补接 KEY，
  A1 全局层先行**。
- 裁决语义影响：F1 设计先行约束以「方案空间 + OQ 裁定」形态闭环；F4（OQ3 与
  spec §5.3 语义模糊）随 OQ3=c 不触发独立 spec 修订，冲突时走逃生门；F5（PKG-4
  部署硬依赖批次①）保留并写入 PROPOSAL §4 PKG-4 前置。
- PROPOSAL 已升 **FROZEN v1.0**（frontmatter freezeState: frozen；裁决记录见
  PROPOSAL §3，主线程逐字落盘）。portfolio-planner 可拆包，首用试金石 =
  ARQ-260911-cross-package-behavior-owner-gap（KEY 接线 owner 登记）与
  ARQ-260911-split-package-implementation-prose-gap（核心包 PKG-2 产包级 plan.md）。
