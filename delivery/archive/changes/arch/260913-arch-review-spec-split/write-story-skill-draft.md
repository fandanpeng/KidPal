# write-story SKILL.md · 草案（landing 预制品，非 canonical）

> 落地目标路径：`.claude/skills/write-story/SKILL.md`（随 260913-arch-review-spec-split 修订批 manifest 原子落地）。
> 用途：写新 story / 改既有 story / 审 story 质量时的统一纪律。本文件为草案，human 过目后才进 manifest。

```markdown
---
name: write-story
description: 写/改/审 docs/requirements.md 的 story（S-Fx.y）时用——五件结构模板、业务框架强制、AC 三段式、线框/锚行接缝、已砍纪律。拆包时「包内必须内嵌 story 原文」的转写规则不归本 skill（归拆包链治理），但拆包引用的 story 格式以本 skill 为准。
---

# write-story · KidPal story 撰写纪律

Story 是需求面的最小承重单元。它回答「产品是什么」，永不回答「怎么造」——后者归 architecture.md。

## Story 结构（五件，顺序固定）

```markdown
### S-Fx.y · <标题>

> 作为<角色>，我希望<能力>，以便<价值>。

**验收标准**（Given/When/Then，正常 + 边界 + 异常至少各一条）
- AC1：Given <前提>，When <触发>，Then <用户可感知结果>
- AC2：……

**线框**：WF-Fx-NN 标注②③（线框实体随 feature 节；待补写「线框待补（归设计稿迭代）」）

**补充规则**：字段校验 / 权限 / 数据口径 / 埋点 / 文案（无则写「无」）

**锚**：UI = `canvas-pX`（舞台粒度；0–3 个为本 skill 新增经验上限——超出先自问是否锚了「道具」或 story 该拆，非六条 SSOT 既有条款）｜ 支撑 = arch §x.y ｜ 可行性 = ✅/🟡/🔴
```

- **ID**：`S-Fx.y`——F = Feature 编号，y = feature 内序号；发布后稳定，不回收、不复用。
- **角色**：孩子 / 家长，二选一站稳；一条 story 只服务一个角色视角。
- **线框编号**：`WF-Fx-NN`（WF + Feature 号 + 两位序号），实体线框集中放 feature 节内，story 按编号 + 标注点交叉引用；线框管布局结构，视觉锚（data-od-id）管设计稿，两层不混。

## 撰写纪律（每条都对应一次真实翻车）

1. **业务框架强制**：story 原文必须「作为…我希望…以便…」三件齐。反例（真实，260829-schedule-reminder 包 story 字段）：「脑算网关推：OpenClaw cron（每 15 分钟定期重算 + QQ 录入即时触发）算三档时刻、幂等 id…」——这是技术方案，不是 story；对应真 story 是「作为孩子，我希望…设备提前主动提醒我，以便不丢三落四」。
2. **机制词禁令**：story 正文（原文 + AC + 补充规则）禁出现实现机制词——cron、散列、幂等、JSON、预合成、WebSocket、API、字段名。写到「怎么做」就停笔，搬去 architecture.md 或补充规则的「数据口径」（口径 = 需求语义，如「『今天』按本地日期零点切换」；格式权威在契约层，不复制）。
3. **AC 是业务可读的验收**：Given/When/Then 用人话写孩子/家长可感知的画面，覆盖正常 + 边界 + 异常；dev-loop 拆包时验收齿（Sx-Ay 形态）逐条回引 AC 号——AC 号就是 story 与验收测试之间的对账点。
4. **可行性诚实**：✅ = 有实测留档/包闭环证据；🟡 = 设计可行但关键环节未实测；🔴 = 当前架构撑不住。**🔴 必须同行挂开放问题或 ARQ 编号**，不许静默存在（Pod 无权自裁，处置 = 立项讨论）。
5. **改 story = 改需求**：编号不回收；砍掉的 story 进文件头部「已砍清单」并注明砍因与日期，不物理删除编号。
6. **NFR 回引**：story 内涉及的量化质量要求（延迟、断网、字号…）写成「具体场景的具体化 + 回引 NFR-ID」，同一约束全仓只在 NFR 章定义一次。

## 锚行规则（SSOT 指针，不复制）

UI 锚六条规则（舞台粒度 / 共享底座钉一次 / 组件例外 / 指现行设计稿 / id 稳定规约 / 机检）的 SSOT = requirements.md 文件头部「UI 锚规则块」；支撑锚目标 = architecture.md 真实小节。写完跑 `node .claude/scripts/spec-anchor-check.mjs` 验锚可解析。

## 完稿自检（写完逐条过）

- [ ] 「作为/我希望/以便」三件齐，角色单一
- [ ] 正文零机制词
- [ ] AC 覆盖正常 + 边界 + 异常，全部业务可读
- [ ] 线框引用或「待补」占位在场
- [ ] 补充规则无格式复制（契约面内容只写口径）
- [ ] 锚行三件齐，spec-anchor-check 绿
- [ ] 🔴 有挂账引用；🟡 写了待验什么

## 边界（不归本 skill）

- 拆包时「TaskPackage 必须内嵌 story 原文 + 验收齿回引 AC 号」的转写规则 → 拆包链治理（260912-gov-review-portfolio-decomposition-chain）。
- 架构怎么支撑 story → architecture.md；契约格式 → docs/contracts/**（契约门）。
```

## 模板应用例（S-F1.4 按新结构重写示例）

```markdown
### S-F1.4 · 日程提醒

> 作为孩子，我希望遇到非常规的事（要带的东西、临时安排）和容易忘的课
> （周末培训班这类）设备提前主动提醒我，以便不丢三落四；
> 例行去上的学校课我自己看课表就行。

**验收标准**
- AC1：Given 课表里有明天 8:00 的英语课，When 今天 20:00 到点，Then 设备亮屏并语音播报「明天要带……」
- AC2：Given 提醒弹窗驻留中，When 孩子短按 KEY，Then 弹窗关闭回到当前页
- AC3：Given 设备断网，When 提醒到点，Then 本地提醒照常弹出（回引 NFR-OFF-1）
- AC4：Given 同一提醒已推送过，When 网关重算或重推，Then 设备不重复弹同一提醒

**线框**：WF-F1-01 标注②（线框待补——现仅 P1 首页线框实存，归设计稿迭代）

**补充规则**：权限 = 课表录入仅家长侧（QQ 主通道），设备端只读；
数据口径 = 「今天/明天」按设备本地日期零点切换；文案 = 安抚句与播报语气见共享底座

**锚**：UI = `state-reminder`（v3 设计稿实存 id）｜ 支撑 = arch §4.2（提醒调度）、§3.3（弹窗渲染） ｜ 可行性 = ✅
```

（注：示例中的 arch §x.y 编号为占位，landing 时按 architecture.md 实际小节填；可行性终标取自 feasibility-initial-marking.md 过目结果。）
