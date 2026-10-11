---
changeId: 261011-p1-home-v5-rework
sources:
  humanVerbatim:
    - "我们现在来处理 requirement 下的那两个REQ"
    - "你先等一下，我觉得这个需求咱们先讨论好,除了现在登记的要有一些默认的话术之外，你还得看一下你接下来显示的这些内容是按什么区分的，以及你明天显示的内容是按什么区分的？"
    - （2026-10-11 AskUserQuestion 需求三答）"空态联动上学日" / "「今天接下来没有安排啦」" / "我们不剔除上学了，他也放在明天区里，同包一块做吧"
    - （2026-10-11 复核两裁）"维持档 1" / "「XX:XX 上学」合并项"
    - "需求改好后 你正常走 triage 拆包，拆好包后准备好环境，然后 headless 启动开发"
    - （2026-10-11 triage 触点①三答）"确认②单包 Full" / "认可数组+空数组语义" / "知悉，按收窄范围做"
  refs:
    - docs/requirements.md §9 v1.61 版本行（2026-10-11 定稿，commit 7f68b51）
    - docs/crq/requirement/_archive/2026/REQ-261010-p1-schedule-empty-state.md（已结）
    - docs/crq/requirement/_archive/2026/REQ-260916-home-date-format-english.md（已结）
    - 2026-10-10 human 真机反馈第 4 条（空态病根原话）
faceMap:
  requirements: 不动——v1.61 已 frozen 定稿（§9 v1.61 行 = 定稿载体，commit 7f68b51）；S-F1.1 AC2/AC5/AC6/AC7 + 补充规则 6a/6b/6c 即本包实现标的
  architecture: 不动——arch §5.2 场景①（:207）与 §10 决策 26（:417）无 preview/明日预告/明天区形状镜像句（architect 取证 2026-10-11）；决策 26 历史句「分组由 OpenClaw 算」为过时历史档表述、不追改（被 V22 producer 命名 + 2026-09-21 契约门裁定取代）
  contract: 动——interface-contract.md §1.3⑥（:144-158）：data.upcoming.preview 单 {time,name} → 多项数组形状 + 空数组语义（[]=确认明日无项）/null 容忍（=未提供）+ 分组口径句扩「明日全量进区、按开始时间升序」+ 示例值「明早」→「明天」定名旧漂移顺修 + 契约变更记录行 append——升级判据①命中（触 docs/contracts/**）→ architect-reviewer 四分类 → ContractChangePacket → human 契约门（先例 260921-m3-k1 preview additive 随包同 PR）
  implementation: 动，跨固件/云端两子面——firmware：home_p1.h/.c（seam 形改 + 多项缓存 + 空态渲染 + 明天区多项行流）、home_upcoming.c/.h（preview 数组解析 + 旧单对象过渡容忍）、schedule_grid.acceptance.c（A5 语义权威段与齿）、p1_home_battery.acceptance.c / p1_cjk_name.acceptance.c（inject 调用点适配）、spiffs 资产「明早」chip 重烘「明天」；server：schedule.py home_upcoming_payload（:433-448）preview 多项化；front_app.py / schedule_push.py / display_schema.py 及四件 server acceptance = 核证面预期零 diff（front_app.py 实路径 server/gateway/front/front_app.py，三推送调用点 :685/:708/:1005 + schedule_push.py:117 单源 helper）；TECH-260928 school_start 同参齿已收口、勿重复修
  governance: 不动——无 .claude/** 改动、无 scope-map/gate/agent 定义触点
facePlan:
  - 顺序1 需求面：已定稿（v1.61 frozen），不进本批；anchors.req 建档即回填
  - 顺序2 契约面（先行，先于 Pod launch）：主线程派 architect-reviewer 四分类（预判 CONTRACT_CHANGE_REQUIRED，示例值「明早」修正属定名漂移顺修、同 packet 合并裁）→ CCP 起草（骨架 contract-change-evidence.md，落 delivery/taskpackages/261011-p1-home-v5-rework/CONTRACT-CHANGE-PACKET.md，先例 260921-m3-k1 放包目录）→ human 契约门签 → 主线程回填 anchors.contract；packet 先批后落：批准后契约 diff 逐字 patch 过目 → git apply 同 PR 落地、收口义务同批（CCP 先例）
  - 顺序3 实现面（单 Pod dev-loop，Full）：planner /build-feature 261011-p1-home-v5-rework——acceptance-author（红基线，含冻结件修订授权链 = CCP id + §9 v1.61 行）→ builder（firmware + server + 资产重烘 + 契约逐字 patch + 设计稿母版内容级同步）→ verifier↔fixer → integrator（docs-sync + host 全量 + ECS 部署）→ 真机看屏轮（human 持板：空态两态 + 明天区多项 + chip「明天」）→ supervisor 机械闸（high 升级 human）→ 前置升级闸 → 本地 main 合并
  - 顺序4 设计稿母版更新：归包内 builder 做（内容级：上学行示例置首 + 空态话术占位 + 旧口径句改判；~10 行不值独立设计活，拆出反致 builder 渲染参照与母版失同步）；视觉像素级呈现归真机看屏轮 human 裁（260926 对齐轮先例）
  - launchPrecondition: anchors.contract 未回填（契约门未签）前承载包不得 launch；spine anchors.req = v1.61 已在位
anchors:
  req: "requirements.md 定稿 v1.61（§9 v1.61 行 2026-10-11 = 定稿载体，commit 7f68b51）：S-F1.1 AC2 明天区改判（学校课「XX:XX 上学」并入、多项升序）+ AC5/AC6/AC7 空态两态分叉 + WF-F1-01 线框同步 + 补充规则 6a/6b/6c（含实现侧对账与契约门登记）；requirement-change 档 1 全程走毕（story-author 两轮 → requirement-reviewer 两轮四查复审 PASS）；spec-anchor-check ok=true（落稿后复跑）"
  contract: "CONTRACT-CHANGE-PACKET.md（delivery/taskpackages/261011-p1-home-v5-rework/，packet id = 同 changeId）——human 契约门 AskUserQuestion 批准 2026-10-11（HUMAN_APPROVED，四触点：preview 多项数组+三态语义 / 示例值 HH:MM 顺修 / :155 seam 引用签名同形顺修 / 变更记录行 append；先批后落，随包同 PR git apply，CRLF 保全）；架构门证据 = architecture-gate-evidence.md（十节 ledger，C1/C2/C3 closeout 归包内 integrator）；pod-events human-decision approve-contract-change-packet 在案（喂 pre-merge-escalation-gate contracts-block）"
  arch: "不动——architecture.md 零镜像句（triage 分流 review 独立取证 2026-10-11）"
  imp: "taskpackage 261011-p1-home-v5-rework 建包（plan.md 落盘 2026-10-11，②单包 Full：rigor=full / riskTier=high / sourceType=human-ad-hoc；taskpackage.json/origin 归 planner /build-feature intake 时建）"
openQ:
  - id: q1-四档路由
    question: 四档路由确认
    ruling: human 2026-10-11 触点①裁——确认 ② 单包 Full（rigor=full / riskTier=high / sourceType=human-ad-hoc）；契约/固件/云端同批原子变，拆包反而制造中间态红
    status: resolved
  - id: q2-preview-wire形状
    question: preview 单对象 → 数组 + [] 空数组语义（承载 AC6 空态话术）+ null/缺省 = 未推送（承载 AC7 未达态）；固件对旧单对象形状过渡容忍一个版本——方向认可？
    ruling: human 2026-10-11 触点①裁——认可（CCP 起草前置输入）
    status: resolved
  - id: q3-V5范围收窄
    question: V5 版式重排取证收窄——home_p1.c 已非 V4 布局（ui-v5-align 阶段 2 commit 70ade0b + 260926 真机对齐轮 6ed6b67/efa8513 已落行流 14px/英文日期/黑帽标，倒计时贴纸已退役）；本批剩 chip「明早」→「明天」定名 + 明天区多项行流化 + 空态话术渲染
    ruling: human 2026-10-11 触点①裁——知悉，按收窄范围做
    status: resolved
  - id: q4-spec对账注漂移
    question: requirements v1.61 补充规则内 v1.57 时代 ⚠️ V5 版式对账注（「大字+倒计时贴纸+中文日期与 V5 不符」）与 home_p1.c 实现已漂移（architect triage 取证）——本批不动需求面（human 已拍），是否后续勘误批修
    ruling: 登记为后续勘误批候选（档 0 勘误通道），不阻塞本批
    status: deferred
  - id: q5-契约门签
    question: CONTRACT-CHANGE-PACKET.md 四触点（preview 多项数组+三态 / 示例值 HH:MM / :155 seam 引用顺修 / 变更记录行）签不签？
    ruling: human 2026-10-11 契约门 AskUserQuestion 批准（无附加条件；预审另新发现 :155 seam 引用签名触点并入 packet、示例终形按 K1 口径 HH:MM 数组「明天」不入 wire）
    status: resolved
status: in-flight
---

# spine · 261011-p1-home-v5-rework

P1 首页落地 v1.61 三件 + 契约同步：① 空态话术渲染（S-F1.1 AC5/AC6/AC7 两态分叉——「今天接下来没有安排啦」/「明天没有什么课喽」/数据未达友好提示）；② 明天区多项化（preview 单项→数组：学校课「XX:XX 上学」+ 课外班 + 临时事件同列、按开始时间升序，翻转 v1.57 OQ2 的 human 改判）；③ chip「明早」→「明天」定名收尾（V5 重排收窄后剩余项）；④ 契约 §1.3⑥ preview 形状重定义 + 「明早」示例值旧漂移顺修——契约门先行（先批后落），固件/云端/设计稿母版同包原子交付。

裁决来源：architect 分流 review（同会话 return，2026-10-11，含三处取证出入：front_app.py 实路径 / V5 重排范围收窄 / sourceType=proposal 机械不合法改 human-ad-hoc），faceMap/facePlan/plan 内容取其报告落盘；触点①三答 2026-10-11 在案。
