---
changeId: 261021-p3-record-cancel-lifecycle
sources:
  humanVerbatim:
    - "开始处理这个debt"
  refs:
    - TECH-261021-story-session-cancel-lifecycle（debt 条目 = 本变更技术输入；trigger 字段载 human 2026-10-21 原裁定「本包收口、三症状记 debt、修位同一处、单开对话处理」）
    - 261019-p3-record-dialogue-guide（来源批，spine delivery/intake/261019-p3-record-dialogue-guide.md；三症状在该包真机目检轮附带发现，同根 = 其 S2 sweep/引导回合/收尾链与设备 ctx 链两侧未闭环）
    - 261001-m4-k3-record-session-live-repair（守卫修复先例：show_home 守卫设计意图「收尾后自然放行」出处）
faceMap:
  requirements: 不动——「丢弃 = 销毁素材、不入册」语义锚已存在（S-F2.2 AC4，requirements.md:462），症状⑤ = 违反既有 AC 的实现缺口，零新增需求语义；S-F1.4 AC6（:303）只作回归项；v1.65 §2.3 键位口径（:153 引导播报中 BOOT 无操作）不动——复验③重定义为实现侧验收定义（openQ q3），不触需求面
  architecture: 动（轻）——场景④链路描述（architecture.md:215）补取消链显式化一句对账；ADR-013 / 决策 25 本体不动；两侧生命周期闭环 = 已批准架构内实现补齐，不命中升级判据（wire 一条私有消息非「协议与语义同时动」）
  contract: 动——新增 `kidpal_story_discard`（端→网关私有扩展消息，与 kidpal_story_delete 先例同构）→ 预判 CONTRACT_CHANGE_REQUIRED（现有契约无法表达已批准设计 S-F2.2 AC4 的端→云销毁通知语义）；start_organizing 行「设备离开录音聆听演出」包络内措辞对账（至多一行，预判 NO_CONTRACT_CHANGE 或措辞级 ACCOMPANIES）；复用通道（abort / listen stop / 断连隐式）已由 architect 实证排除（abort 同会话态正面冲突 mcp_server.cc:701-711 → protocol.cc:56-57；listen stop = 归档触发语义相反 application.cc:894-895；断连通道 teardown 悬挂实锤失效 front_app.py:1496-1506）；正式四分类归 architect-reviewer（本批 /arch-change 内）
  implementation: 动——server = front_app.py（teardown gather 悬挂修复 / discard 消费分支 / 在途引导双点存续守卫 / 腿集会话归口）+ story.py 预判零改~极小；固件 = mcp_server.cc（start_organizing 消费补 StopListening settle + DISCARD_RECORD case 补发送）+ protocols/protocol.h/.cc（SendKidpalStoryDiscard）+ host 齿两文件（record_session_live / record_dialogue_guide）；五档 = ② 单包 Full / riskTier high / sourceType regression / rigor full（architect 裁，human 触点① 2026-10-21 确认）；launch 前置 = 契约面 anchor 回填（change-intake §8）
  governance: 不动——verify-gates.json / .claude/hooks / scope-map / agents 定义零命中（architect grep 核实）
facePlan:
  - 顺序1 架构+契约面（轻面，本会话内联）：/arch-change Track B——architect 起草瘦 PROPOSAL（含契约包三节：§四新增 kidpal_story_discard 行 + 变更记录条 + start_organizing 措辞对账 + 场景④取消链一句对账）→ architect-reviewer 四查 + 四分类正式裁定 + 轻/主判档 → 主线程凭立项 ack + GREEN 标 FROZEN → packet-bound 落地 → landing-verifier → 瘦制品五件过闸（retro 豁免）→ 合并本地 main〔**判档结论注记（主线程回填）**：R1 正式裁定 = 主路径（轻路径判据 ①③④ 三否），全件制品 + retro 照常，本行「瘦制品五件过闸（retro 豁免）」预判以裁定覆盖〕
  - 顺序2 实现面：顺序1 anchors 回填后，human 触点③ 跑 planner 会话 `/build-feature 261021-p3-record-cancel-lifecycle`（从 delivery/taskpackages/261021-p3-record-cancel-lifecycle/plan.md 读 reviewed plan 做 intake）；烧板轮不可省（mcp_server.cc / protocol.cc 属设备编译单元，按 full 环境备）
anchors:
  req: "N/A——需求面零命中，无 §9 版本行（S-F2.2 AC4 既有语义实现对齐）"
  arch: "architecture.md §12 changeId 行 `261021-p3-record-cancel-lifecycle`（2026-10-21 landed）——§5.2 场景④ 取消链显式化一句（设备丢弃 → 契约 §四 `kidpal_story_discard` → 网关按连接权威 cancel；断连 cancel 兜底）+ §5.1.2 front 行闭集枚举补一词（R1-F1 裁定随批补）；制品 = delivery/review/261021-arch-review-p3-record-cancel-lifecycle/（FROZEN v1.1 + R1 BLOCK(F1,F2)→v1.1 修毕 + R2 GREEN + landing-summary 偏差申报三处）"
  contract: "interface-contract.md 两触点 LANDED（CDQ-261021-p3-record-cancel-lifecycle）：① §四新增 `kidpal_story_discard`（端→网关私有扩展消息、网关按本连接 session_id 权威取消不信载荷值、幂等 + 审计、单向无应答、省略 = cancel-on-disconnect 兜底维持）+ 变更记录条；② §1.4 `start_organizing` 行包络核对零改（NO_CONTRACT_CHANGE，R1 正式裁定）。四分类 = CONTRACT_CHANGE_REQUIRED（主触点，R1 正式裁定）；落地核验待 landing-verdict"
  imp: ""
openQ:
  - id: q1-wire分叉
    question: 设备→server 丢弃通知复用既有通道（abort / listen stop / 断连隐式）还是新增显式 cancel 信号？
    ruling: **human 2026-10-21 触点① 裁：Design B 新增 `kidpal_story_discard`**——三条复用通道 architect 实证排除（abort 同会话态承载「打断继续聆听」正面冲突；listen stop = 归档触发语义相反、固件注释钉死严禁发；断连隐式通道 teardown 悬挂实锤失效且无法区分主动丢弃 vs 意外掉线）；破坏性用户意图走显式可审计通道，与 kidpal_story_delete 先例同构；契约走 /arch-change 契约包三节随批原子落（261019 先例：arch 批先落、实现后跟）
    status: resolved
  - id: q2-掉线会话语义
    question: 纯掉线会话（Wi-Fi 抖动 mid-recording、无显式丢弃）掉线后处置——cancel-on-disconnect 还是保留待自动归档？
    ruling: **human 2026-10-21 触点① 裁：维持 cancel-on-disconnect**（HWV-A11 既有设计语义；内存卫生、实现简单；代价 = 掉线丢素材可重录；跨连接续存机制复杂度不做）
    status: resolved
  - id: q3-复验③可达性
    question: v1.65 §2.3 键位口径下「引导播报中按 BOOT」= 无操作（story_page.c:1718 已落 NONE，story_page.h:111 注释绝不落毁稿路径），debt 复验③「引导播报中丢弃」不可达；语音「讲错啦」取消通道双侧未接线——如何处置？
    ruling: **human 2026-10-21 触点① 裁：(a) 重定义复验③** =「引导播报刚停、子态回聆听瞬间丢弃 → 在途/在播引导残余立即断」，不动需求面、本包内闭环
    status: resolved
  - id: q4-讲错啦缺口归属
    question: 语音「讲错啦」取消通道双侧未接线（server/gateway 与 firmware grep 零命中；red-baseline story-p3 已记 V31(b) 缺失 RED）是否另登 debt？
    ruling: 主线程 L0 裁——不另登：red-baseline story-p3 既有 RED 条目已是承载位，本包不动该通道（S-F2.2 AC4 语音取消语义属需求面后续批），复验走按键路径
    status: resolved
status: in-flight
