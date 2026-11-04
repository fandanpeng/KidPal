---
changeId: 261103-star-points-story-cover
status: in-flight
sources:
  humanIntentVerbatim:
    - "我还有一个想法：我们能不能把故事页前面再加一页？比如加个类似封皮的页面，点了可以进入之后才到 list 页。这样的话就可以保证我们在首页的时候，按 BOOT 键不断切换是个循环的"
    - "我们是不是把目标设置成活的，比如 40分 游乐场，50分 礼物，我们提前设计好几个常见的礼物场景，后面可以通过qq渠道去设置目标"
    - "语音触发 必须是明确的这个场景，比如说我们今天去游乐场了，扣40分，然后才进入积分也扣分，加分一样，我今天扫地的了，加5分，这样才能触发，不能小孩随便说了一句我们去游乐场了，就扣分或者加分"
    - "允许负余额"（OQ-1，触点① 2026-11-03）
    - "禁止删，先切换"（OQ-2）；"5–200 正整数"（OQ-3）；"KEY短按=语音"；"确认 ③拆包+spike"
  refs:
    - delivery/ideas/261103-story-cover-navigation.md
    - delivery/ideas/261103-star-points-book.md
    - docs/notes/showme/261103-star-points-home-wireframe.html
    - docs/debt/tech/TECH-261028-p3-reader-key-falls-to-session-toggle.md
    - docs/debt/tech/ARQ-260918-spec-face-3page-reconcile.md
faceMap:
  requirements: |
    动（档 2 结构性 + 触 UI）。§2.2 键帽条页点 3→4、§2.3 按键矩阵重排（P3 积分主页/
    P3 详细账本/P4 封皮/P4 故事列表/P4 阅读各态）、§2.4 页面结构 3→4、F2 封皮化下沉
    （拟 S-F2.6 故事封皮与分层导航）、新增 F6 星星积分本（拟 S-F6.1–S-F6.5）、
    NFR-OFF-1 补积分断网边界、§6 路线图登记、§9 记账。逐条行号依据 =
    delivery/workstreams/261103-star-points-story-cover/review.md faceMap 节。
  architecture: |
    动。设备组件表（积分主页/详细账本/故事封皮组件、页枚举、键归属）、服务端组件表
    （账本与目标配置确定性 owner、front 记账校验执行推送、QQ skill、prompt 接线）、
    运行场景（新增记账端到端场景 + 场景④ P3 假设修订）、数据视图（账本分录/激活目标/
    场景库/设备渲染缓存）、幂等横切（短窗去重与传输幂等分离）、资产登记（书封+四场景
    +自定义通用标记）、§9 演进、§12 记账；§5.1.1 撤查字页前旧四页叙述随批消化。
  contract: |
    动（预期 ACCOMPANIES_SPEC_CHANGE，最终分类归 architect-reviewer）。§1.3 view 白名单
    加积分主页/账本列表两类下行形状（余额、激活目标、最近记录、兑换前后余额、空态/未达、
    restore、分页语义）、§1.4 记账/兑换薄意图控制动作、§1.5 front 记账与删除机械职责、
    §1.6 system prompt view 列表与生成纪律、§四 kidpal_session_ctx 扩语境 + 设备删除上行
    私有消息 + 账本分页请求/应答；story_list 板子消费语义改 P4 模块（封皮计数复用 total）。
  implementation: |
    动。firmware：kidpal_render_node 页枚举 3→4 与循环、story_page source_page 数值映射
    （case 2 承重接线点）改枚举驱动、key_gesture_parser、keybar、view_router、
    mcp_server display 工具说明；server：display_schema view/动作/shape floor、
    front_app 推送与私有帧消费接线、OpenClaw skill（新积分 skill）与两工作区 AGENTS.md
    薄意图规则（保持 kidpal 数据目录只读、模型只产薄意图）。
  governance: 不动——不触 governance 面、不需要 /gov-change（architect 核实；新测试落既有
    test 体系，无新 agent/gate/scope-map 变更；若实施中发现须改 .claude/** 则回 intake 加面呈 human）。
facePlan: |
  1. 需求面先行：/requirement-change 档 2 触 UI 批次（story-author 盘上 updating 起草 →
     architect 可行性三件 → uiux-author 出图规格 + 主线程代执行出图 → requirement-reviewer
     触 UI 五查 → human 看图终拍）；完成后主线程回填 anchors.req。
  2. 架构+契约面第二：/arch-change 主路径 Track B（architect 起草组件/数据 owner/端到端
     时序/账本幂等与分页 + 契约包三节随批原子落；architect-reviewer 四查 + 契约四分类）；
     完成后回填 anchors.arch 与 anchors.contract。
  3. 实现面第三：③拆包，workstream = 261103-star-points-story-cover，K0 端到端 spike →
     K1 账本核心 → K2 设备积分 → K3 QQ 通道 → K4 目标配置 → K5 故事封皮导航，WIP=1
     串行；PROPOSAL 冻结（human 触点②）待 req/arch/contract anchors 回填、拟议 story 锚
     真实入 frozen requirements 并过 §1 两查后执行；随后 portfolio-planner 拆包（门 1）。
  4. governance 不派发；debt 结项（consumed-by）由主线程按实际落地处理。
  launch 硬前置：先于实现面的各面 anchors 未回填不得 launch（change-intake.md §8）；
  触 UI 包另须 frozen PNG/brief 版本对齐。
anchors:
  req: "requirements.md §9 v1.71（2026-11-04 档 2 结构性·触 UI 定稿、frozen）——星星积分本（新增 F6 五 story S-F6.1~S-F6.5）+ 故事封皮与四页导航（新增 S-F2.6、F2 下沉 P4 模块、§2.2/§2.3/§2.4 共享底座重排）+ §4 金币/星星视觉边界（封皮徽记同族形态）+ NFR-OFF-1 积分边界 + 新增 NFR-PERF-4（软时延待真机定数）+ §6 阶段五；glossary 五词条 + 三条注记/连带随批；human 四轮看图终拍（三轮打回、四轮通过）；reviewer 五查 + 三轮靶向复审 PASS；architect 可行性三件无 🔴；UI 产物 = 设计母版 v9 + brief v8 + V9 变更帧 11 张（UI-ready 消费判据 = §9 v1.71 版本行 + brief v8 对齐注记，均在位）"
  arch: 待回填
  contract: 待回填
  imp: 待回填
openQ:
  - id: route
    q: 五档路由与 spike-first
    ruling: human 2026-11-03 触点① 确认 ③拆包 + spike-first 命中（K0 端到端最小 spike，②Full，不判 nano）
  - id: OQ-1
    q: 删除早期加分致重算负余额，与「余额不为负」底线冲突
    ruling: human 2026-11-03 裁「允许负余额」——仅删除重算路径可为负；兑换动作仍要求余额 ≥ 激活目标分值；负余额的进度线画法（小伴钳位等）留需求起草/线框迭代；ideas 文件与 PROPOSAL D-03 已同步修订
  - id: OQ-2
    q: QQ 删除当前激活的自定义目标
    ruling: human 2026-11-03 裁「禁止直接删除，先切换到其他目标再删」（预置场景不可删不变）
  - id: OQ-3
    q: 自定义奖励目标分值范围
    ruling: human 2026-11-03 裁「5–200 正整数」
---

# 261103-star-points-story-cover · spine

> 路由账本，非设计文档（spine 免冻）；设计权威 = 两份 ideas 讨论记录 + workstream PROPOSAL/review.md。

## 变更一句话

新增 P3 星星积分本（金币账本：单页进行中、设备语音 + QQ 双通道记账、可配置奖励目标）并把故事列表下沉为 P4 故事封皮的子页，主页面恢复 4 页 BOOT 循环。

## 探测与裁决（摘要；全文 = delivery/workstreams/261103-star-points-story-cover/review.md）

- 五档 = **③拆包**（新云端持久数据域 + 双通道 + 端云新契约 + 设备两页族 + 故事层级重排，跨多切片）；spike-first **命中**（新端到端账本数据通路；K0 = ②Full spike，真实语音/QQ 各写一笔、设备真看到，不以文件写入自证）。
- workType = [new-capability, remediation]；workstream riskTier 总体 high，包级由 portfolio-planner 逐包派生。
- 触面 = 需求 + 架构 + 契约 + 实现 四面；governance 不触。

## debt 消费关系

| 条目 | 关系 | 处置 |
|---|---|---|
| ARQ-260918-spec-face-3page-reconcile | 被本变更消化 | spec 重写页循环时消解陈旧叙述；spec 定稿后主线程按 consumed-by 结项 |
| TECH-261028-p3-reader-key-falls-to-session-toggle | 被 K5 消化 | 故事封皮/导航包同步修复阅读键路由缺陷 |
| TECH-261012-qq-schedule-write-format-gap | 并行，只复用先例 | 新 QQ 积分 skill 参考机检/marker 形态，不宣称修复该条目 |
| TECH-261030-qqbot-ask-user-block | 并行，构成硬设计约束 | QQ 命令不依赖 ask_user，歧义用普通回复 + 稳定记录定位 |
| TECH-261022-device-memory-prompt-overhaul | 并行底座 | 复用持久 session 与静态 prompt 承载，做记忆归属回归，不并入遗留项 |
| TECH-261023-openclaw-tool-face-prompt-only-readonly | 并行防线 | kidpal 数据目录保持只读、模型只产薄意图；若改为直接写账本须回审 |
| ARQ-260907-openclaw-tools-rejection-rootcause | 不阻塞不消费 | 不重开引擎 Intent |
| TECH-261022-p3-record-session-state-churn | 有独立承接包 | 共享 keybar/故事页文件须串行对账，不重复承接 |
| CDQ-260914-kidpal-session-ctx | 本变更扩展之 | 仅当同批验收覆盖原三值发送与 front 拦截义务时方可核销 |
