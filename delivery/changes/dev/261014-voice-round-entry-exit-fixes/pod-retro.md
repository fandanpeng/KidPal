# pod-retro · 261014-voice-round-entry-exit-fixes（planner 第一人称热记忆，2026-10-14）

> 标签：第一人称热记忆。不复述、不冠权威、不载 tokens/gates/rounds/verdict/test results——那些以 machine artifacts / pod-events / verifier-evidence 为权威。escalation themes 引 pod-events eventId。

## 过程体感

- **这是一次「续跑」而不是「新起包」**：上一轮 headless 会话在 intake 落盘后被账号配额（429）掐断，盘上 TaskPackage 全字段齐、current 指针已刷。我按 1f 四问对磁盘核了一遍（方案锚点、冻结件钉区、基线锚差异判停）零漂移，就直接从 acceptance-author 续了——没有重复建包、没有重归一。省下来的成本是真的，代价是**我必须自己补跑本该在 Step 7 产的 brief**（中断那轮没来得及产），这一步顺手抓出了不少东西（见下）。续跑纪律里值得固化的一条：**续跑第一件事是核 current 指针指向本包**——指针是隐性的全局面，指错了会一路静默假绿。
- **「闸绿」和「闸真在跑」是两件事，这是本包最值得记的一条**。intake 遗留的 sourceRefs 用的是 `docs/requirements.md#S-F1.4` 这种带路径锚点的写法，而机械 AC 回引齿的 storyRefs 正则只认裸 `S-F1.x`——于是那道齿**整个静默不激活**，taskpackage-required 回报 green。我是做 Step 8b 人工核对（AC 回引号集对账）时才发现本包压根没有任何 AC 回引，补了 `S-F1.4-AC1` / `S-F1.4-AC6` 复合句法后齿才真的跑起来。要是没有那一步人工核对，这个包的 AC 联动会一路假绿到底。**同一个判据：闸报绿时，还该问一句「它到底跑了没」。**
- **第二个 intake 遗留形状缺陷**：`mutationEvidence.perAcceptance[].acceptance` 写成了缩写而非 acceptance 原文，违反 `taskpackage-authoring.md` §5「必须原样使用 acceptance 完整文本」。机械闸查不出来，我按约定改成原文照录。两条都是**同一个教训的两种形态**：起包期省下的手工核对，最后都要在 finalize 期用更贵的方式还回来。
- **发现一处闸的「松」**：mutation-check 判记录敏感与否，正则 `/sensitive/` 是打在记录 JSON 文本上的，而记录里天然带 `"sensitive": false` 这个键——**键名一在就命中**。换句话说，起包骨架的 `turnedRed:false` 占位本身就足以过闸。我没拿这个松口偷懒：仍然让 builder 真跑了四项变异、逐字红输出落 evidence 文件、turnedRed 置真。但这条得攒给 loop-retro——**想让这张齿真承重，判据得看值、不能看键名**。
- **builder 的两处「须 planner 裁」都裁在边界内，动作是对的**：采样载体它落成 application.cc 文件级 static 而非类成员（因为 changeManifest 没含 application.h，它选择守边界、不越权改头文件）；unit 层它也选择只用在册测试件承载而不新增清单外文件。**「不越权扩单、把张力交回 planner」比「顺手把该改的都改了」强得多**——这是 scope 纪律真正在起作用的样子。但根因在起包：plan 措辞写「冻结成员」而 manifest 没列 application.h，是 intake 时方案与 manifest 没对齐。**下次起包该加一问：方案提到的每个落点文件，是不是都在 changeManifest 里。**
- **integrator 自曝的一处红值得记**：它首版 scope 清单每行带了**行内** `#` 注释，而闸只剥整行注释 —— 结果整份清单被判「盘上全部不存在」、fail-closed 举手。**闸拦得对**（畸形清单没被静默按空跑绿），但真正的信息量在于：我派单书里写「如 `delivery/changes/dev/<id>/test-scope-final.txt`」，它没照抄，而是按 SSOT 落到了 TaskPackage 目录 sibling，并在回执里给出了依据。**派单书里塞「如 <路径>」的示例有风险**——子 agent 有时当指令、有时当建议；这次它当建议是对的，但下次我该把示例和硬要求写清楚，别让子 agent 自己猜。
- **分层出场口径的直接受益**：本包全程**零 human 请示**。launch 时 human 已把六条执行边界钉死（尤其是：真机轮不在本 Pod 范围、engine 改动只 commit 不部署 ECS、不合并 main），所以 acceptance/builder/integrator 三个常规派发我一次都没停下来问。**把「只有人才能做的事」在 launch 时就切出去，比在 loop 里反复判断「这要不要请示」省太多**——这是本包最顺的地方，没有之一。
- **没有 verifier↔fixer 绕轮**：scoped 一轮即绿，final 轮的红是制品格式红（integrator 自纠），不是代码红。**改动面小、验收面先钉死、派单书写到行号**——这三件叠加起来，内循环几乎不烧。

## escalation themes（引 pod-events）

- 本包**零 agent-escalation 事件**——全程没有 Red Flag 出 Pod 边界。builder 报告里那两条「须 planner 裁」是边界内裁定（文件级 static 载体形态 / unit 层由在册件承载），不是 out-of-scope、不是 human ruling request，所以**没有**落 escalation 事件（避免 advisory 语义滥用）；两裁定的成文留痕在 taskpackage `notes`。
- 唯一的人工面事件 = launch 六条执行边界，`human-decision` eventId `6fac8138-41b4-41d9-af9e-463b3df07595`（human 授权、planner-intake 记录）——本 Pod 全程按此跑，未再触人。
- 两条超读记账（`over-scan`，纯 advisory、不拦截）：`73e5a4f5-05cb-4e94-ba7e-8a496d7096c7`（acceptance-author 10 行）、`8e2263d8-7d1d-4937-ab69-ecc1b5d881b8`（builder 8 行）——均为核可编性/闸口径所必需，判合理。

## 下次改进（攒给 loop-retro）

1. **起包 finalize 前加一问**：`changeManifest ⊇ 方案提到的所有落点文件`（本包 application.h 险些要靠扩单救）。
2. **sourceRefs 引 requirements story 一律用裸 `S-Fx.y` + AC 复合句法**，别用 `docs/requirements.md#S-Fx.y` 路径锚——后者会让 AC 回引齿**静默失效**（隐性假绿通道，建议 loop-retro 评估是否给闸加一条「锚点路径形态 story 引用」的 advisory）。
3. **mutationEvidence 骨架的 `turnedRed:false` 能被闸的键名匹配放行**：若要让这张齿真承重，判据应看值。
4. **scope 清单格式写进 integrator 派单模板**：一行一个仓根相对路径、注释必须整行 `#`（行内 `#` 会被整条判为不存在）。
5. **续跑纪律成文**：续跑第一步核 `current/` 指向本包；中断轮未产的 Step 7 brief 必须补跑，且补跑本身是抓 intake 遗留缺陷的高产位置（本包两条遗留形状缺陷都是补跑 brief + Step 8b 核出来的）。
