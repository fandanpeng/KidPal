---
changeId: 261014-single-tier-bringlist-home
sources:
  humanVerbatim:
    - "我们现在讨论一下：像这种怎么加到我们的提醒中去，比如我把它通过 QQ 渠道转发到 Open claw，Open claw 怎么把这个信息转成我们需要的格式，然后再在终端上显示出来？（2026-10-13，老师作业/通知三类真实消息为样本）"
    - "我现在发现，我们早上 7 点 45 的通知，包括晚上睡前的通知，并没有起到什么作用，所以我想把通知只保留事前 15 分钟提醒。另一方面，我想利用首页展示接下来的行程。比如「去上学」是在早上 8 点到下午 6 点这个时间段，里面可能需要带的东西、需要准备的事情都可以合在一起，直接显示在「8 点上学」后面，跟着一堆像带跳绳、带水瓶之类的清单。（2026-10-14）"
    - "聚合到上学项，因为他上学之后，在学校内怎么安排我们是不知道的，也控制不了，所以它分为几点几点，对我们来说没有意义（2026-10-14）"
    - "校内时段的边界怎么划？早上 8 点到下午 6 点（2026-10-14）"
    - "卷进去的就不用提醒了呀……我在想我们是不是搞一个别的呀？有个就是上学需要带的东西，这可以再仔细讨论一下（2026-10-14——带物清单升一等公民的转向点）"
    - "字数的话，我觉得就可以把它精简，让 AI 直接把它精简。我们每一个区域至少可以显示三行，看这个大小，一行显示十几个字应该是够的（2026-10-14）"
  refs:
    - TECH-261012（QQ 课表录入链路既有能力面，server/deploy/openclaw-skills/**）
    - REQ 历史行 v1.61（明天区多项化 + preview 契约门先例，delivery/changes/arch/261011 同批）
spineCreatedAt: 2026-10-14
faceMap:
  requirement:
    hit: true
    what: S-F1.4 补充规则（docs/requirements.md:303 三档时刻 = 20:30/07:45/前 15min → 单档 + 校内窗口静默）；S-F1.1 AC2/补充规则（:227/:235 上学行挂带物清单 + 校内一次性事项折叠进「上学」合并项）；S-F1.4 AC1（:295 Given 例锚定 20:30 档须随档位改写）；S-F1.5（家长录入扩带物清单录入面）。触 UI 批次（首页版式加清单行）→ 档 2 流程含 uiux-author 出图。
  architecture:
    hit: true
    what: docs/architecture.md §5.1 组件行 :196（「三档算术 20:30/07:45/前 15min」）与 §5.2 场景⑤ :216（「重算三档」）需镜像修订；§5.2 场景① 日程刷新三触发口径复查（折叠后「接下来」语义变化）。
  contract:
    hit: true
    what: interface-contract.md §1.3⑥ home view data.upcoming 逐项 {time,name} → additive 字段承载上学行带物清单文字；§三 :246 「按课表算三档时刻」→ 单档口径。走 `/arch-change` 触契约分支（architect-reviewer 四分类 + 契约包三节随批原子落、无人签门、合并过机械凭据闸——261015-gov-review-face-loop-unify 批现行口径，2026-10-16 本批 T4 第 3 条刷新，原文见 delivery/archive/changes/ 归档前版本 git 历史）。
  implementation:
    hit: true
    what: 服务端（openclaw-reminder.config.yaml / reminder_schedule_lib.py 单档化 + 静默窗口；schedule.config.yaml school_end；front_app.py + schedule.py bringlist 读取与折叠拼装）；OpenClaw 创作面（AGENTS.md 家庭面纪律 + schedule-write skill 扩带物清单录入）；固件（home_p1.h/.c + home_upcoming.c 清单行渲染，板端对旧载荷过渡容忍——261011 preview 先例）；部署面（manifest.yaml 新消费位 + kidpal-front.service env）。
governanceProbe: 未命中治理面（.claude/** 零改动）；升级判据①触契约面命中 → 路由归 OQ-1 human 裁（Track B + 契约门随批 vs Track A）。
facePlan:
  - 需求面：requirement-change 档 2（story-author 起草 → requirement-reviewer 四查 → architect 可行性三件 → 触 UI 批次 uiux-author 出图 → human 终拍看图）
  - 契约面：`/arch-change` 触契约分支——architect-reviewer 四分类 → 契约包三节（proposedContractDiff / impact / validationPlan）随批原子落、无人签门（现行口径，2026-10-16 T4 第 3 条刷新；additive 字段 + §三措辞）
  - 实现面：triage-change 比例化 → dev-loop Pod（服务端包 + 固件包可拆两包并行；launch 前置 = 需求/契约 anchors 回填）
  - 架构面：镜像修订随需求面同批（arch §5.1/§5.2 两处 + 场景①复查），不单独立批
  - 顺序：立项 ack → 需求面起草与评审（含契约四分类预审）→ 契约包随批原子落（无人签门）→ dev-loop launch
anchors:
  req: ""
  contract: ""
  arch: ""
  imp: ""
openQ:
  - q: 路由——触契约面命中升级判据①，Track B 独立评审批 vs spine 分面推进？
    a: human 2026-10-14 裁「降为立项稿、不建独立评审批」——设计稿落在 intake companion（261014-single-tier-bringlist-home.design.md，无 FROZEN-landing），设计对抗审由 requirement-change 档 2 architect 必派承担；原 delivery/changes/arch/261014-gov-review-single-tier-bringlist-home/ 目录撤除、INDEX 行随全量重建消行。
  - q: 带物清单是否要一条早晨语音兜底（07:45 单条合并播报）？
    a: human 2026-10-14 讨论倾向纯首页展示；立项稿 OQ-2 正式呈裁。
  - q: 校内窗口边界值？
    a: human 2026-10-14 口头定 08:00–18:00；立项稿 OQ-3 随批确认 school_end: "18:00" 进配置。
status: awaiting-human:立项-ack
---

# intake spine · 261014-single-tier-bringlist-home

## 意图一句话

把提醒模型从「三档到点念给孩子」改成「事前 15 分钟单档 + 校内时段静默」，把「上学要带的东西」升为带物清单一等公民（OpenClaw 录入、AI 落笔精简、按日期一行），首页「上学」行后面直接跟当天清单文字，校内一次性事项按 08:00–18:00 窗口折叠进「上学」项——从「到点推」转向「一瞥可见 + 最小打扰」。

## 触面事实（本会话查证，file:line）

- 需求：S-F1.4 补充规则钉三档时刻（docs/requirements.md:303）；S-F1.4 AC1 Given 例直接锚 20:30 档（:295）；S-F1.1 AC2 与补充规则钉「接下来/明天」分组口径与 {time,name} 形态（:227/:235）。
- 架构：§5.1 组件行（docs/architecture.md:196「三档算术 20:30/07:45/前 15min」）与 §5.2 场景⑤（:216「重算三档」）——镜像修订面。
- 契约：§三 首句「按课表算三档时刻」（docs/contracts/interface-contract.md:246）+ §1.3⑥ home 载荷 {time,name}（板端 seam home_p1.h 同形，K1 裁决 HH:MM 字面）。
- 实现：三档常量在 openclaw-reminder.config.yaml:18-20；首页「接下来/明天」载荷由 schedule.py group_upcoming 单源产出（:289-344，逐项 {time,name,location}，home 侧剥 location）；板端 seam home_p1.h preview 三态 + HP1_PREVIEW_MAX（:22、:75-93）。

## 立项对齐（为什么改 / 往哪改 / 改动多大）

- **为什么改**：三档中前晚与早上两档真机使用无效（human 2026-10-14 反馈原话「并没有起到什么作用」）；校内事项的分钟级时刻是家庭不可知不可控的假精度；携带物信息靠到点语音送达不可靠。
- **往哪改**：单档临期提醒 + 首页常驻带物清单（设计载体 = intake companion `261014-single-tier-bringlist-home.design.md`，T1 单档化 / T2 静默窗口 / T3 带物清单 / T4 首页挂清单+契约 additive）。
- **改动多大**：需求面 2 个 story 结构性修订（档 2）；契约面 1 次 additive 契约变更；实现面服务端 + 固件两包可并行；架构面 2 处镜像。整体量级 = 中。
