---
changeId: 261012-kidpal-realtime-facts
sources:
  humanVerbatim:
    - "原来在 openclaw 主环境时可以查 天气的，"孩子对话回合禁工具、一回合作答" 这个对于实时信息 应该查询，可以提示，我去查一下，然后开始查，这样体验也不会差"
    - "OpenClaw 侧 kidpal AGENTS.md 只有 11 行，我们是不是可以考虑怎么利用一些这个，还有 Openclaw kidpal 这个环境 应该没有主环境对应的 tool 这个应该给加上"
    - （2026-10-12 triage 触点①三答）"确认②单包 Lite" / "先行捎带" / "认可口径"
  refs:
    - docs/crq/tech/TECH-261012-weather-fabrication.md（根因 + scoping + human 方向裁定在案）
    - 真机现场：2026-10-12 串口 c:/tmp/hv5-eyeball-round.log 603065-732415ms 段（天气幻觉三源对账）
    - server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER（prompt 版本演进史）
    - 先例 delivery/taskpackages/261010-prompt-v9-schedule-anti-narration/plan.md
faceMap:
  requirements: 动（轻量记账）——NFR-SEC-2 出网路径 append 第 ④ 条（kidpal agent 实时事实查询走平台 Tavily）+ NFR-SAFE-1 实时转述注记；走 requirement-change 档 1 先行捎带（story-author 起草 + requirement-reviewer 审稿，内联执行）
  architecture: 动（指针刷）——arch §5.5.5 + §5.1（:196「版本化 v9」）prompt 指针刷 v10（docs-delta，v9 先例同款；由包内 integrator docs-sync 承载，非独立 arch 批）
  contract: 疑似触（interface-contract.md §1.6 :182-185 逐字引文钉 system prompt 核心约束）——architect 建议 NO_CONTRACT_CHANGE（引文未枚举回合路由/工具政策，v7/v9 同位未触）；判定归 architect-reviewer 四分类（change-triage §6 逃生门），留痕进本 spine anchors；CONTRACT_CHANGE_REQUIRED 则停 + 转契约门 + 重新分档
  implementation: 动——server/deploy/openclaw-system-prompt.v10.md（新建）+ OPENCLAW_SYSTEM_PROMPT_CARRIER（v9→v10 + 演进史 append）+ provider.config.yaml（三键）+ apply-xiaozhi-overlay.sh（PROMPT_SRC/DST + 注记）+ server/acceptance/test_prompt_v7_round_routing.acceptance.py（additive RF 齿）+ server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md + README.md（新建仓内镜像）；ECS live ops：openclaw.json 复核 →（条件）kidpal tools 覆写 → AGENTS.md 同步 → v10 渲染重跑（overlay 重跑后必核 model_name=openclaw/kidpal 与决策 21 两键未被冲——261001 冲刷陷阱）；固件零改动
  governance: 不动
facePlan:
  - 顺序1 契约面（轻，先行）：主线程派 architect-reviewer 四分类（预期 NO_CONTRACT_CHANGE；疑似触面成立 = §1.6 逐字引文邻接），判定留痕进 spine anchors
  - 顺序2 需求面（轻，先行捎带，human 已批）：requirement-change 档 1——story-author 起草 NFR-SEC-2 ④ / NFR-SAFE-1 注记 → requirement-reviewer 四查 → 主线程落稿翻 frozen + §9 版本行（v1.62）
  - 顺序3 实现面：②单包 Lite（rigor=lite / riskTier=medium / sourceType=regression）→ delivery/taskpackages/261012-kidpal-realtime-facts/plan.md → planner /build-feature 完整 dev-loop；launchPreconditions：Tavily kidpal 会话实证先于 ECS 部署 v10（包内 builder 承载）；槽位核验 server/deploy/ 无并行 .v10 占用
  - launchPrecondition: 顺序1/2 未闭（契约分类留痕 + 需求面 frozen）不阻塞 repo 制齿（architect 裁：轻面不阻塞），但 ECS 部署步骤须在契约分类落痕 + Tavily 实证后；真机轮 human 在场（问天气全链路 + 一次降级观测）
anchors:
  req: ""  # 待档 1 落稿后回填（v1.62）
  contract: ""  # 待 architect-reviewer 四分类留痕
  arch: "动=指针刷（§5.5.5/§5.1 v9→v10），归包内 integrator docs-sync，非独立批"
  imp: "taskpackage 261012-kidpal-realtime-facts 建包（plan.md 落盘 2026-10-12，②单包 Lite：rigor=lite / riskTier=medium / sourceType=regression；taskpackage.json 归 prep/planner intake）"
openQ:
  - id: q1-四档路由
    question: 四档路由确认
    ruling: human 2026-10-12 触点①裁——确认 ② 单包 Lite（rigor=lite / riskTier=medium / sourceType=regression；architect 事实修正：E4 冻结件已退役，咬 v10 的是四个版本无关守卫、零授权链）
    status: resolved
  - id: q2-需求面记账时序
    question: NFR-SEC-2/SAFE-1 档 1 记账先行捎带还是后置？
    ruling: human 2026-10-12 触点①裁——先行捎带（内联轻面执行）
    status: resolved
  - id: q3-延迟口径
    question: 实时事实回合延迟预算与降级口径？
    ruling: human 2026-10-12 触点①裁——认可（提示语 ~4-5s 内首句语音 / 真实答复目标 ≤15s 上限 20s / 先观测后钉阈值 / 真机轮加测降级路径）
    status: resolved
  - id: q4-Tavily可用性
    question: kidpal 会话实证若无 web search（平台覆写缺失或 key 失效）怎么办？
    ruling: architect 预案——无工具 → agents.entries.kidpal 增 tools 覆写（ECS ops 先例）后复测；平台级不可用 → 停、升级 human，可能转方向 B /requirement-change；prompt 侧写「工具不可用→诚实说不知道」降级口径使其对工具缺席鲁棒
    status: resolved
status: in-flight
---

# spine · 261012-kidpal-realtime-facts

TECH-261012 天气幻觉修法批：OpenClaw kidpal agent 实时事实查询能力开放（prompt v10「实时事实回合」——先播「我去查一下」→ 平台 Tavily 真查 → 只依据结果作答，查不到/失败诚实说不知道+引导，严禁编造/虚称已查）+ 通用反编造条款补死（对话行 + 底线节第三不变量）+ kidpal AGENTS.md 仓内镜像充实 + Tavily kidpal 会话实证。工具面平台层已启用（openclaw.json tools.web.search=tavily enabled、kidpal 无覆写），禁令在 v9 提示词——本批开闸不开新工具设施。

裁决来源：architect triage 快审（同会话 return，2026-10-12，含 E4 退役事实修正与四守卫核证），触点①三答在案。
