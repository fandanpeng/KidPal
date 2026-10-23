---
id: REQ-261023-family-arrangement-scope-clarification
title: NFR-PRIV-1 ①「家庭安排」范围未澄清——与 S-F1.3 AC4「备注列参与语音答复」在接送/出行类备注上字面冲突（spec 内部张力，非措辞问题）；一句话澄清即可消解
domain: requirement
status: deferred
severity: SHOULD
owner: /requirement-change loop（story-author 起草；一句话澄清 = 档 1 语义修改）
trigger: 下一次需求面变更批；或 human 指示即刻起草（批 A+B 已落 prompt 侧表态，spec 侧澄清不阻塞）
opened: 2026-10-23
closed: ""
closedBy: ""
paths:
  - docs/requirements.md
source: "261023-agents-md-static-semantics-alignment triage——architect 升级判定（plan.md §6）+ architect-reviewer 四分类裁决（NO_CONTRACT_CHANGE，2026-10-23）；human 甲案指示，主线程代登记"
---

## 张力本体（两句已冻结 spec 原文的字面冲突）

- `docs/requirements.md:654`（NFR-PRIV-1 ①）：「家长隐私信息（姓名、联系方式、**家庭安排**）不进记忆画像、**不被复述**」——「不被复述」未限定记忆回忆语境，按字面可覆盖接送类备注（如「周三奶奶接」）。
- `docs/requirements.md:286-289`（S-F1.3 AC4 + 补充规则）：「Given 日程条目带备注（如携带画具），When 语音问课表，Then 答复包含备注内容」——P5 起备注**逐字**注入课表摘要。
- 契约面无歧义（`docs/contracts/interface-contract.md:82/:240/:242`：备注露面处 = 语音答复读回侧）——冲突纯在 requirements 内部两句之间。

## 建议澄清口径（architect-reviewer 裁决附带的将来 spec 一句话）

「课表 / 提醒备注中与孩子当天安排相关的接送、携带、老师称呼，按 S-F1.3 AC4 原词读回，不属 NFR-PRIV-1 ① 的『家长隐私』；家长隐私 = 成人自身的身份与事务信息（备注以外的家庭安排内容仍受禁止）。」

## 现状与风险余量

- **prompt 侧已表态**（批 A+B `261023-agents-md-static-semantics-alignment` S3 条，四分类 `NO_CONTRACT_CHANGE` 后随包落）：AGENTS.md 对象锁定句在 K3 冻结锚串之后追加孩子日程澄清——与冻结 spec 的读回侧一致，无冲突落地风险。
- **未澄清前**：spec 两句的字面张力在案；将来任何动 NFR-PRIV-1 或 S-F1.3 的批都可能重新撞上，届时以本条口径为对账基准。
- **live 判读注记**（同批四分类附带）：K3 seeded 负向探针「100% 零命中」按**答案来源**判（源自记忆画像/回忆 = 命中；课表回合内源自摘要或读文件的备注读回不计入），探针问句避免「今天谁接我」类课表回合本该答的问法，并核对探针语料与真 schedule.md 备注无同串。

## 关联

- 批 A+B 包 `delivery/taskpackages/261023-agents-md-static-semantics-alignment/plan.md` §6（升级判定全文 + 甲案裁决记录）。
- K3 包 live 轮 checklist（seeded 负向探针判读注记）。
