# input-brief · acceptance-author · 260929-char-window-v5-trim

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-28T16:37:33.695Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/260929-char-window-v5-trim/taskpackage.json` · subsystem: firmware/main/display · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/kidpal-ui-design-v5.html` | — | 无锚点（整文件） | — | bare |
| `delivery/changes/intake/260929-char-window-v5-trim.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/260929-char-window-v5-trim/plan.md` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F3.2 · 源 = `docs/requirements.md` 节首行 :500

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F3.2 · 看拼音/笔画/部首/释义

> 作为孩子，我希望查字时同时看到拼音、共几画、部首和释义，以便不只学写法还学用法。

**验收标准**
- AC1：Given 查字结果，When 看右栏，Then 拼音带声调 + 「共 N 画」+ 部首标签 + 释义 四行。

**线框**：WF-F3-01（速查栏 = 窗内右栏）。
**补充规则**：内容规范 = 右栏**纯文本逐行列出**（无卡片容器——v1.57 弃速查卡框族，视觉归设计稿 v5），保持速查信息密度、不做关键词解释墙；释义 = 该字的核心释义、一行内。数据口径 = 释义随查字结果一并下发（权威格式在契约层，不复制）；**逐笔笔名只走听觉（TTS）、不上屏**（v1.56 human 裁——显示面只保留右栏速查栏）。已砍联动 = 「进度珠」（原 AC2：一行 N 颗与演示同步——米字格逐笔演示本身即进度，旁置进度珠冗余；实现侧从未实现，见 §8）。埋点 = 无。
**锚**：UI = `char-window`（与 S-F3.1 同窗，右栏为窗内区域；原锚 canvas-p4 同 S-F3.1 对账改名） ｜ 支撑 = arch §5.1（查字组件·笔顺渲染，板）、§5.2 场景③（查字链路，云）
**可行性**：✅（随 S-F3.1 于 2026-09-15 真机复验恢复；同链两缺陷——板端 LVGL 栈、SVG 解析器 C 命令——登记 dev-loop 修复）
```

### S-F3.2-AC1 · 源 = `docs/requirements.md` 节首行 :500 · 本包声明 AC 粒度回引：`S-F3.2-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F3.2 · 看拼音/笔画/部首/释义

> 作为孩子，我希望查字时同时看到拼音、共几画、部首和释义，以便不只学写法还学用法。

**验收标准**
- AC1：Given 查字结果，When 看右栏，Then 拼音带声调 + 「共 N 画」+ 部首标签 + 释义 四行。

**线框**：WF-F3-01（速查栏 = 窗内右栏）。
**补充规则**：内容规范 = 右栏**纯文本逐行列出**（无卡片容器——v1.57 弃速查卡框族，视觉归设计稿 v5），保持速查信息密度、不做关键词解释墙；释义 = 该字的核心释义、一行内。数据口径 = 释义随查字结果一并下发（权威格式在契约层，不复制）；**逐笔笔名只走听觉（TTS）、不上屏**（v1.56 human 裁——显示面只保留右栏速查栏）。已砍联动 = 「进度珠」（原 AC2：一行 N 颗与演示同步——米字格逐笔演示本身即进度，旁置进度珠冗余；实现侧从未实现，见 §8）。埋点 = 无。
**锚**：UI = `char-window`（与 S-F3.1 同窗，右栏为窗内区域；原锚 canvas-p4 同 S-F3.1 对账改名） ｜ 支撑 = arch §5.1（查字组件·笔顺渲染，板）、§5.2 场景③（查字链路，云）
**可行性**：✅（随 S-F3.1 于 2026-09-15 真机复验恢复；同链两缺陷——板端 LVGL 栈、SVG 解析器 C 命令——登记 dev-loop 修复）
```


## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯板端呈现面与本地资产改动（char_page 删星/删组词行/cardlabel 让位/引导线改黑点线、frm_236_236.bin 重烘）；server payload words 继续下发零改、wire 形状（D3 笔顺 JSON / E1 display 响应块）零触碰——architect 分流 review 四分类=无需改，spine anchors.contract 已钉

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260928-char-window-key-triggered-play | `firmware/main/test/char_window_overlay.acceptance.c` · `firmware/main/test/keybar_three_slot.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

