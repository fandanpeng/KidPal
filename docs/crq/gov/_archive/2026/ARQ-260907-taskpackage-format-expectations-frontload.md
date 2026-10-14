---
id: ARQ-260907-taskpackage-format-expectations-frontload
title: taskpackage/验收的机械闸格式期望没有前置对齐，多个 Pod 靠闸红反推再回派/补字段，同型红在 ≥3 包复现（跨 4 包 5 子项）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

DEV-GOVERNANCE 新节「## 起包格式期望前置」五条清单（mutationEvidence 骨架 / 断言格式 / 验收 ID 禁括号后缀 / 降级声明前置 / 新约定包闸判据自查）+ 四件 agent def 引用行（planner/portfolio-planner/builder/acceptance-author，合计 +6，OQ2=(a) human 已批）。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。**复现（loop-retro-2026-09-13，已结项未被行为闭合）**：260911 批三包仍同型红——display-first-m1 verifier 三趟「补一行 JSON 再重验」（DF-A3~A6 未标 structural / mutationEvidence acceptanceId 缩写失配 / docsSyncRequired=false 缺 reason，retro 原话「四轮能压成一轮」）、render-pages 三轮 scoped 红全为 marker/字段类（coverage map 缺 5 id、mutationSkipReason 缺字段、docs/notes 缺锚点）、screen-unification 元数据面反复卡包（验收条目无 kind 标记 / mutation 证据空 / docs 同步理由缺）。并暴露五条清单未覆盖的新子项：① hasCoverageMarker 只认 `coverage: <id>` 前缀形态而 HOST_TEST_CONVENTIONS 未钉机器可识别形态（key-route-a1 r1 红一轮实证）；② structural 豁免须对象条目 kind:'structural' 编码、验收作者交付结论未接闸契约（display-first-m1 实证）；③ docs/notes 锚点 append 通道「每次都靠人肉记住」（render-pages 第三次应验）。按 GC1 记复现不新立，新子项随本条后审一并处置。

**main 侧追注同步（2026-09-24 main→gov 回同步 merge 迁入；来源 = loop-retro-2026-09-23 第 3 轮对旧队列行的追加证据，逐字搬运）**：**复现二（loop-retro-2026-09-23，18 包复盘）**：第三波同型红跨 5 包，形态继续变形——rma8（K4 改进项未真正执行，mutationEvidence 短 id 第二次踩）、m3-k4（scoped r1/r2 两轮全为 planner 元数据形态错：contractRefs 空数组被判缺 + 短 id）、k6 r2（perAcceptance 的 acceptance 键用摘要改写、与 acceptance[] 整段文本静默失配）、k7 r1/r2（sourceType 凭语义写 `arch-change-handoff` 非枚举闭集 `arch-review` / crossEnd.counterparts 写散文串非 `{path,symbol}` 结构体 / mutationEvidence 写嵌套对象非扁平记录）——k7 retro 明言「非阻塞建议第三次入册 = 该升硬条目了」。**正向对照**：凡「回填前先读 checker 提取器源码」的棒（m2-k1 / m3-k2 / k5）全部免税；k5 与 m3-k4 追加轮把「finalize 预飞三闸判据 checklist」当动作清单用后零元数据红——m3-k1 retro 判词「记坑的形状不如记『读 checker 的提取器再回填』——坑会变形，机制不会」。**新增子项**：mutation-check 匹配语义只活在脚本源码，报错（missingSensitive）不提示「perAcceptance 须 acceptance[] 全文 + sensitive 信号」（m3-k4 / k5 / k6 / k7 四棒同报）。**升级建议（候选，交 review，不自批）**：① DEV-GOVERNANCE 起包前置清单补硬条目「perAcceptance 的 acceptance 键从 acceptance[] 复制原文，禁摘要改写 / 短 id / 嵌套对象」；② mutation-check 报错信息带一句格式提示（治本半——报告口径是唯一能拦住形态变形的手段）。证据：[loop-retro-2026-09-23 报告](../changes/retro/2026-09-23-loop-retro.md)。

**追加证据（loop-retro-2026-10-14，18 包复盘）——复现三（第四波，闭环建议仍未落地）**：同型红跨 6 包且形态继续变形——260928-tech-school-start（pod-events pe-005 taskpackage-required 三连红：origin.type/sourceType 闭集外自造值、exitCriteria 缺字段、mutationRequired=false 缺 mutationSkipReason，planner 连续三次 Write 坏 taskpackage.json，retro 原话「闸是唯一救了我的东西」）、260928-char-window-key-triggered-play（origin.upstreamType 闭集外自造 + contractRefs 空数组 + decisionCoverage 自创字段形态，上批 m3-k4/k7 同款）、261010-p3-storylist-cold-start-restore（exitCriteria 漏字段闸红 + coverage 标记须紧邻 `coverage:`，retro 自称「第三次烧同一类坑」）、261012-kidpal-realtime-facts（RF-A5 漏标 kind:'structural' + mutationSkipReason 写错层级，retro 自评「mutation-check 只认顶层、同义信息两字段位是给起草人挖的坑」）、261012-p3-topbar-ink-bleach-fix（mutation-check 全文逐字匹配 r1 红——写短锚版未逐字复制，前包 notes 先例在案未读）、261013-p1-tomorrow-row-sliver-fix（r1 冻结件头注 coverage 行合写 `TR-A1/TR-A2`、机械正则只认 `coverage:` 后第一个 id；r2 perAcceptance 未逐 id 全覆盖）。**新增子项**：① coverage 标记正则只认首 id、合写形态（`A/B`）被判缺——「一标一 id」未成文；② perAcceptance 骨架须按 acceptance[] 全集逐 id 起、占位 `turnedRed=false`（tomorrow-row 收敛方案原话「起包直接给 N 条全量骨架」——本波唯一治本级实践）；③ 全角冒号/全角括号形态继续变形（261001-archive-order event 559ec927：acceptance[] 用全角「：」+ id 后括号致 shortAcceptanceRef 抽出整段散文，并预警兄弟包同形）。**正向对照**：perf-k1「六闸预跑基线让 finalize 后的红极便宜」、topbar「读前包 notes 按地雷清单读」。**升级建议不变且加急**：报错格式提示（治本半）与「一标一 id / 全集骨架」两条硬条目至今未落，第四波已到——建议并入下一治理批优先处置。证据：`delivery/changes/retro/2026-10-14-loop-retro.md`。
