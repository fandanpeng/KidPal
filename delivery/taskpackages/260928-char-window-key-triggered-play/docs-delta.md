# docs-delta — TaskPackage 260928-char-window-key-triggered-play（subsystem: firmware/main/display，查字窗 KEY 起播）

> 读者是人（supervisor / human / 后续 wrap-up）。本包 docsOwner = integrator，本文记录
> 「docs-sync 判定：零 docs 改动」的对账结论，docsSyncRequired=false。

## 1. 变更摘要

查字窗演示与播报解耦（requirements v1.59，S-F3.1 AC1/AC3/AC5 + S-F3.3 AC1/AC2 +
WF-F3-01 键帽随态变）：建窗首帧改米字格静态整字（`charpage_render_frame(-1, n_strokes)`）、
timer 建表即 pause（未按 KEY 前零逐笔演示、零动画拍）；KEY 短按 = 首次起播 / 从头重播
同路径（`replay` 归零 elapsed + 置 started + `lv_timer_resume`，锁内改态、锁外刷键帽）；
演完 REST 相位定格不循环（timer_cb 渲染定格帧 + 刷定格文案后 pause + return，不推进
elapsed）；键帽随态变（未起播「播报」/ 起播后「重播」，新并行纯逻辑单源
`kidpal_keybar_char_slots`，lcd_display sync 查字窗前台分支消费）。

## 2. docs-sync 判定（docsSyncRequired=false 的对账结论）

- **spec 面 v1.59 已由需求面档 2 落稿，本包零 docs 改动**：requirements.md v1.59
  （含 §9 追加行、S-F3.1 AC5 新增、WF-F3-01 键帽随态变注、NFR-PERF-2 阈值终点重锚）
  与 architecture.md 两处镜像句（:111 查字延迟句、:211 场景③）已随需求面档 2 于
  commit `546832c` 落稿（story-author → requirement-reviewer 四查 → human 终拍批准，
  全程见 `delivery/changes/intake/260928-char-window-key-triggered-play.md` anchors.req
  / anchors.arch 回填）。镜像句是 requirements 口径的逐字镜像（openQ Q3 裁定随档 2
  同批，human 终拍已确认），本包**不重改**。
- 实现与 spec 口径一致性核对：v1.59 三关键语义（静态整字初始态 / KEY 起播重播 /
  演完定格不循环）与 TTS 播报不动，逐条与 char_page.cc 落地 diff 对读一致；
  spec-anchor-check 在 verifier scoped r3 全绿（无新漂移）。
- **零触 `docs/contracts/**`**：本包纯板端呈现面改动，不新增或改变端→网关消息、帧
  类型、字段与序列化形态（taskpackage crossEnd.exempt 申报；D3 笔顺 JSON 与 E1 display
  响应块消费形状不变）——闸 A 四分类 ④ 无需改（NO_CONTRACT_CHANGE）。
- 本包文档产物仅落 closeout 制品目录
  `delivery/changes/dev/260928-char-window-key-triggered-play/`（本件 + review-packet.md）
  与包目录内 final-scope.txt，不散写。
