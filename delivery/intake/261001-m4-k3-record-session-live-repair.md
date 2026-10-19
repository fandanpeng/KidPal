---
changeId: 261001-m4-k3-record-session-live-repair
sources:
  humanVerbatim:
    - "今晚晚上 server 和板子都重新部署了，为了测试M4 story 功能，在部署前 我测试对话都是好的，部署后 Openclaw 返回400了，你查一下是什么问题"
    - "你开串口监控，我走一遍 P3 的功能，把问题报给你，你在分析，准备修改包"
    - "1. 切换到P3 还是默认页面，没有列表，后台应该有一条记录了 2.点说话直接跳到 声波页面，不是record页面，没有任何回显，3.语音说 说完了 没有反应，4.点击Key（说完了）直接崩溃重启"
    - "1.同意甲， 2. 走triage吧"
  refs:
    - 真机冒烟串口捕获 c:/tmp/p3-smoke-261001.log（2026-10-01 00:21-00:23，COM5 被动，含 LVGL panic + backtrace；ELF SHA 0422fa290 与本机 firmware/build/xiaozhi.elf 一致）
    - 引擎 docker logs（261001 00:21:38 listen start manual；整窗零识别文本）+ front journalctl（仅连接事件）
    - 变更草稿 delivery/taskpackages/261001-m4-k3-record-session-live-repair/plan.md
    - 前序包 delivery/taskpackages/260924-m4-k3-record-session-live/（本轮缺陷的落地包）
faceMap:
  requirements: 动（仅注记）——:431 S-F2.2 可行性注记「manual 聆听 + 网关累积态已备」为机制表述，取甲后漂移；行为语义（:57/:181/:367/:424 S-F2.2-AC2「整段累积、AI 不逐句回复、切句只用于回显、说『我说完了』/『讲完啦』+ KEY 兜底」）不变
  architecture: 动——:213 §5.2④「引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）」须改机制表述并修其内部矛盾（manual 模式产不出 stt）；:416 决策 25 属 :390 起冻结历史档，按 ADR 新增修订、不改正文
  contract: 动（疑似漂移）——interface-contract.md:266「设备 manual 聆听仍经既有 listen 消息承载」+ :170 §1.4 start_recording「设备切 REC 录音演出（manual 聆听模式）」两处机制表述随取甲漂移；**human 261001 触点①裁「引擎侧再动抑制 LLM」后，范围扩及 :266 同句的「引擎不感知本信号」（累积态需 front→引擎 或等价机制让引擎侧可抑制 LLM 调用）——属独立契约变更候选，须 Track A 定形后重备 packet**。D1 的 story_list 推送时点属行为策略（:179 明载不进契约），不触契约
  implementation: 动——纯固件面 = firmware/main/{mcp_server.cc, application.cc, display/kidpal_render_node.cc, display/story_page.c, boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc}；跨面面 = firmware 聆听模式切换（application.cc:880/883）+ server/gateway/front/front_app.py（累积态压制 llm/tts 下行）+ server/gateway/story.py:59（收尾词表口径待确认）
  governance: 不动——.claude/agents/** 定义 / gate / settings / scope-map 零命中；change-triage / change-intake 程序本身不动
facePlan:
  - 顺序1 主线程建 spine（本件，faceMap + facePlan 落盘，anchors 留空）
  - 顺序2 **Package A 先行（human 261001 触点①确认）**：纯固件修 D1/D2 层栈半/D4a/D4b → ② 单包 Full（riskTier=high / rigor=full / sourceType=regression）→ delivery/taskpackages/261001-m4-k3-record-session-live-repair/plan.md → planner /build-feature 开工，不依赖契约门（止血优先）
  - 顺序3 Package B（D3-甲，跨面）：architect-reviewer 四分类已完成（触点1/2 = 随 spec 改动携带（同 PR）；触点3 = arch 面；触点4 = ADR 新增；触点5 = 需求注记）→ **human 触点①已裁「引擎侧再动抑制 LLM」**，契约面随之扩及 :266「引擎不感知本信号」→ 须先由 `/arch-change` Track A 定形机制（front→引擎 信号形态 / 引擎侧抑制实现）再重备 ContractChangePacket → 契约门签字 → **Package B 实现面冻结至 spec/contract anchors 回填**
  - 顺序4 Package C（独立小包，human 触点①裁「另立」+ 确认档位）：引擎 overlay model_name 冲刷修复 → `delivery/taskpackages/261001-openclaw-model-name-overlay-drift/plan.md`（② 单包 Lite / riskTier=low / sourceType=drift-report；经 architect 快审：单面实现、不触契约/spec、逃生门不触发、不建独立 spine）
  - launchPrecondition（Package A）：无上游锚依赖，可即时 launch；Package B 受顺序3 anchors 门控；Package C 独立
anchors:
  req: "**已闭环（2026-10-19 回填）**——D3-甲需求侧语义与 :431 注记随 261019-p3-record-dialogue-guide 需求面 v1.65 frozen 承载（S-F2.2 补充规则翻案留痕段含引擎侧抑制连带）：requirements.md §9 v1.65"
  contract: "**已闭环（2026-10-19 回填）**——Package B 契约面（:266「引擎不感知本信号」措辞对账 + 引擎侧抑制机制）由 261019-arch-review-p3-record-dialogue-guide 吸收定形：interface-contract.md 三触点 LANDED（含 §四 session_ctx「引擎侧抑制由 front 内部注入等价信号承载」），条目 = docs/debt/contract/CDQ-261019-p3-record-dialogue-guide.md；261019 主线程 rebase 合并期按 ledger-rehome 迁移格式迁入 contract 域"
  arch: "**已闭环（2026-10-19 回填）**——Package B 架构面（§5.2④ 链路 + 引擎侧抑制定形）由 261019 批 ADR-013 承载（supersedes 决策 25 机制面），architecture.md 五改法点 LANDED（landing-verdict PASS 27 项对账）"
  imp: "Package A：taskpackage 261001-m4-k3-record-session-live-repair **已收口**（dev INDEX closed，2026-10-16 T5 整改实证——②单包 Full 全程走毕）。Package C（独立变更另 id）：taskpackage 261001-openclaw-model-name-overlay-drift **已收口**（dev INDEX closed）。Package B：定形 + 实现义务经 261019 spine q1（human 裁吸收）转移至 261019-p3-record-dialogue-guide——实现归其实现面承载包（待 261016 合并后五档分档），本 spine 无剩余在途义务"
openQ:
  - id: q1-四档路由
    question: architect 判 ③ 拆包（Package A 纯固件 ②单包 Full / Package B 取甲跨面转契约门+Track A），human 是否确认或改判？
    ruling: human 261001 触点①裁——**确认③拆包**（Package A 先行、Package B 待契约/架构面冻结）
    status: resolved
  - id: q2-取甲机制漂移分类
    question: 取甲改聆听机制触契约（:266/:170）+ 架构（:213）+ 需求注记（:431），四分类与门路由？
    ruling: architect-reviewer 预审（261001）：触点1/2 = 随 spec 改动携带（同 PR，机制表述对账、wire 语义零变）、触点3 = arch 面走 Track A、触点4 = 决策 25 按 ADR-008 新增不改正文、触点5 = 需求注记。**human 触点①裁「引擎侧再动抑制 LLM」后范围扩及 :266「引擎不感知本信号」→ 分类须随 Track A 定形后复裁、packet 重备**
    status: resolved（待 Track A 定形后复裁）
  - id: q3-收尾词表口径
    question: 「说完了」（三字）是否并入 _FINISH_PHRASES（story.py:59 现仅「我说完了/讲完啦」）？
    ruling: 随 D3-甲 进需求面一并裁，未确认前按 S-F2.2-AC2 原文实现（不阻塞拆包）
    status: deferred
  - id: q4-D2层栈闭集取向
    question: 进录音态让位的实现形态——弹出 VOICE 层（则 D4a 判据天然成立）vs 录音页提到 VOICE 之上（则 D4a 判据须显式补 RECORDING 分支）？
    ruling: 归 Package A 设计决策点，plan.md 须写明二选一 + host 可机判齿
    status: pending
  - id: q5-模型名冲刷包
    question: 引擎 overlay model_name 被部署冲刷是否另立包？
    ruling: human 261001 触点①裁——**另立独立小包**；architect 快审判 ② 单包 Lite（低风险、单面实现、不触契约/ADR-006 按 append-only 新增 ADR-007 修订、PERF-K1 冻结验收重钉 + 订正失准的「契约 §四」文案），human 复确认档位；plan 落 `delivery/taskpackages/261001-openclaw-model-name-overlay-drift/plan.md`，不另建 spine
    status: resolved
  - id: q6-甲案外部性处置
    question: 取甲后引擎内部仍逐句把录音内容送 OpenClaw（front 截不住），契约 §1.4「不按句送对话」读法二选一如何处理？
    ruling: human 261001 触点①裁——**引擎侧再动抑制 LLM**（取「全程无 LLM 回合」读法 (乙)）；跨系统面加重：须 Track A 定形 front→引擎 抑制机制，契约 §四「引擎不感知本信号」句随之入面
    status: resolved
  - id: q7-需求注记路由
    question: requirements.md:431 注记（「manual 聆听 + 网关累积态已备」）走需求面 /requirement-change，还是并入本 arch 批顺带？
    ruling: 待订（architect-reviewer 指出路由归需求面/human；注记-only 不阻塞 Package A）
    status: deferred
  - id: q8-D3根因复核准（261001 包A收口后主线程复核 r2 真机日志）
    question: Package B 立项前的 D3 根因口径——retro/docs-delta 记「stt 帧产出正常、AI 也逐句回复，真实缺口在 front 累积/回传接线」，与主线程原判「manual + NON_STREAM 产不出 stt 帧」不一致，哪条对？
    ruling: 主线程复核 `c:/tmp/serial-live-repair-r2.log` 原始行序，两条各对一半、真因是**第三件**：① 录音**期间**确无 stt（manual 模式 ASR 在 listen stop 才触发），故收尾词前置检测物理上不可能——原判方向对；② stt 在 `story_key stop_rec archive_pend=1`(t=131012) **之后** 1.7s 才回（t=132772），而 front 的 `_story_stop_on_listen_stop` 在同一条 `listen/state=stop` 上行帧上**立即** `acc.stop()` → 累积缓冲此时必然为空 → `empty_accum` 是**顺序缺陷（归档早于转写结果），不是竞态、也不是「没 stt」**；③ 收尾后引擎把整段当**一轮**回答并 TTS（`<< 放假还有作业` 等，是同一轮的多句），故「逐句回复」表述不精确——实录是「收尾后整段回一轮」，同样违反「收尾即盖章、AI 闭嘴」口径。r2 另证 ctx 到达正常（t=69242 `story_list_dispatch` 于 ctx 沿推送）。**Package B 立项据此订正**：除聆听形态/抑制面的 Track A 外，须含 front 归档时序修法（等转写结果再归档）。retro/docs-delta 的表述订正随 B 的 docs-sync 落地
    status: resolved（供 Package B 立项消费）
  - id: q9-B1归档时序包（q8 的直接产物）
    question: q8 复核出的「归档早于转写」缺陷如何立项？
    ruling: human 261001 触点②同意先开；architect 分流判 ② 单包 Full（riskTier=high / rigor=full / sourceType=regression；存疑向严理由 = 数据丢失 + 触累积态状态机 + 改写已 merged 包的冻结验收时序假设），human 复确认档位。plan 落 `delivery/taskpackages/261001-m4-k3-story-archive-order/plan.md`（修法 = 停录挂起 pending-stop + 转写到达后归档 + 正文剔收尾词 + 10s 超时兜底；120s sweep 同源缺陷划 B2/B3）。**不建独立 spine**（单面实现）
    status: resolved
status: closed（2026-10-19 结项：Package A/C 已收口；Package B 定形 + 实现义务经 261019-p3-record-dialogue-guide spine q1 human 裁定吸收——其 arch/contract 面 FROZEN v1.2 + landing-verdict PASS 并已合本地 main（ad28fd7c），实现归同批实现面承载包，见 anchors 三行回填）
---

# spine · 261001-m4-k3-record-session-live-repair

2026-10-01 真机 P3 冒烟暴露 `260924-m4-k3-record-session-live` 落地后的四缺陷：D1 星星记录本列表不出现（延迟推送无消费方）/ D2 按 KEY 进声波层而非法录音页（会话层压住录音页）/ D3 语音收尾「我说完了」不生效（manual 聆听 + NON_STREAM ASR 产不出 stt 帧）/ D4a 录音态 KEY 落全局会话 toggle 致毁稿 + D4b 录音演出定时器致首页图像悬垂 LVGL panic 重启。human 已拍板 D3 取甲（改可产逐句 stt 的聆听形态 + front 累积态压制逐句回复）。

裁决来源：architect 分流 review（同会话 return，2026-10-01），faceMap/facePlan 取其报告 §1/§2 落盘；逃生门判定（D3-甲 触契约/架构面）见其 §5。

## openQ 账 · T5 整改留痕（2026-10-16，261015-gov-review-changes-taxonomy T5 第 3 条）

- **全面实审回填**：anchors 四键如实回填实况（imp = Package A / Package C 均已收口，dev INDEX closed 双实证；req/contract/arch = Track A 待跑实况）；`status: in-flight → awaiting-upstream` 实判——Package B 实现面冻结至 Track A 定形（facePlan 顺序 3 门控），spine 整体处于等上游面状态（值域 = change-intake.md §2.7）。预期漂移标：⚠未闭环（req/contract/arch 实况回填后 anchors 4/4 但未 closed）+ ⚠滞留Nd（awaiting-* 滞留超 7 天）——如实呈现待 loop-retro 复议。
