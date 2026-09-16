# input-brief · acceptance-author · 260915-m1-k2-keybar-three-slot

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-16T07:52:36.847Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/260915-m1-k2-keybar-three-slot/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `delivery/milestones/milestones.jsonl` | — | 无锚点（整文件） | — | bare |
| `docs/ui/brand-spec.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/UIUX-DESIGN-BRIEF.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `§2.2` | 2.2 状态表（会话生命周期 × UI 演出） | 58 | ok |
| `docs/requirements.md` | `§2.4` | 2.4 页面结构（3 页循环） | 172 | ok |
| `delivery/milestones/milestones.jsonl` | `M1-home-qa` | （未定位到标题节） | — | not-found |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/display/lcd_display.cc` | `kidpal_voice_state_look` | :1040-1042 | — |
| `firmware/main/display/kidpal_voice_states.c` | `kidpal_voice_state_look` | :23-66 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260915-m1-k1a-key-layer-dispatch | `firmware/main/test/key_layer_dispatch.acceptance.c` |
| 260915-m1-n1b-p1-home-battery | `firmware/main/test/p1_home_battery.acceptance.c` |
| 260911-display-first-m1 | `server/acceptance/test_g_relay_fix.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

