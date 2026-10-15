# pod-retro · 260928-story-connect-push-defer（planner 第一人称热记忆）

> 写于 integrator closeout 完成、final 终审绿之后、supervisor 机械闸之前。机器事实（轮次/闸/verdict/测试计数）以 bearing facts、pod-events、dispatch stamps 为权威，不复述。

## 一、续棒复用在「同缺陷面修复」场景的红利

三棒全部是 K1 原班 SendMessage 续跑：acceptance-author 改自己冻过的齿（根因不用重新装载）、builder 摘自己写的调用点、integrator 熟门熟路。派发书只需要增量指令 + 新包指针，三棒都一次过。**同面缺陷修复不要起新上下文**——这是本轮最大的效率来源。

## 二、两轮 verify 红全在我自己的 finalize 面（如实记）

1. `contractRefs` 填了空数组——闸要非空（K1 包有 3 条，我自建 Lite 包想当然留空）。本包②③推送确实仍承载 §1.3③ wire 形状，引用合法，是我没想这一层。
2. FIX-A2 的 coverage 标记形态：齿 docstring 用复列标记（`M4K1-A3 / FIX-A2（…）`），闸要求 `coverage:` 后紧跟短 id。正确解不是迁就闸改标记，而是认清 FIX-A2 本体 = 语义修订记录齿（满足面 = 修订落实本身，行为敏感由 FIX-A1 + 修订后 a3 齿承载）→ 归 structural（与 M4K1-A7 同机制）。**structural 用在「记录齿」形态上是第一次，语义学比闸形学诚实。**

共同根因：**自建包起包时只预跑了 mutation-check 诊断，没跑全套只读闸**（K1 包我跑过四闸诊断、一次过的回报直接兑现过）。同一个坑不因为包小就变浅——起包 checklist 的「finalize 字段完整性 + 全闸诊断预跑」没有包体量豁免。

## 三、K1 行为等价假设②的证伪闭环做完了

「ws_gateway 调 _push_story_list ≙ 建连即推」在真机拓扑被证伪（设备每会话重开 WS）→ 提前爆雷 → human 裁止血 → 本包摘除 + 证伪记录落 review-packet（K1 件不改写）。体感：**显式申报的未验假设不是免责牌，是给未来留的账**——这笔账四天就收了，而且收的时候 K1 的上下文还在（续棒红利的前提）。

## 四、escalation themes（cross-ref pod-events）

- human-decision `b036f554`（approve-bleeding-stop-fix）：human 真机发现 + 方向裁定的唯一 human 介入点。
- 无 agent-escalation 事件：三棒零 Red Flag。
- friction：无新增（over-scan 各棒申报面均为零或派发指定件）。

## 五、下次改进

1. 自建 TaskPackage 起包预跑全套只读闸诊断（taskpackage-required / acceptance-coverage / mutation-check / docs-sync / security）——写进个人起包动作，不等闸来抓。
2. 「记录齿」形态的 acceptance 条目直接按 structural 申报（本包先例），别让 coverage 闸用标记形态来替我做分类。
