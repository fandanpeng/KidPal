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
  req: "待回填（D3-甲 随 spec 修订顺带改 :431 注记；路由 = 需求面 /requirement-change，见 openQ q6）"
  contract: "待回填（architect-reviewer 四分类 261001 已预审；Track A 定形后重备 packet + 契约门）"
  arch: "待回填（§5.2④ 改法 + ADR-008；/arch-change Track A）"
  imp: "Package A：taskpackage 261001-m4-k3-record-session-live-repair（②单包 Full，riskTier=high/rigor=full/sourceType=regression；plan.md 落盘 261001，taskpackage.json/origin 归 planner intake 建）。Package B：未落包（待 spec/contract anchors 回填后另立）。Package C（独立变更，另 id）：taskpackage 261001-openclaw-model-name-overlay-drift（②单包 Lite，plan.md 已落盘 261001）"
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
status: in-flight
---

# spine · 261001-m4-k3-record-session-live-repair

2026-10-01 真机 P3 冒烟暴露 `260924-m4-k3-record-session-live` 落地后的四缺陷：D1 星星记录本列表不出现（延迟推送无消费方）/ D2 按 KEY 进声波层而非法录音页（会话层压住录音页）/ D3 语音收尾「我说完了」不生效（manual 聆听 + NON_STREAM ASR 产不出 stt 帧）/ D4a 录音态 KEY 落全局会话 toggle 致毁稿 + D4b 录音演出定时器致首页图像悬垂 LVGL panic 重启。human 已拍板 D3 取甲（改可产逐句 stt 的聆听形态 + front 累积态压制逐句回复）。

裁决来源：architect 分流 review（同会话 return，2026-10-01），faceMap/facePlan 取其报告 §1/§2 落盘；逃生门判定（D3-甲 触契约/架构面）见其 §5。
