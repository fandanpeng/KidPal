# OQ-3 裁决记录 · 261023-honcho-memory-wiring-restoration（2026-10-23 晚，部署轮实测后）

> append-only 记账。OQ-3 原题 = 家长 QQ 内容可被 `honcho_*` 工具检索的处置（arch §6.7 残余第 5 行 / 面C F4 挂账）。

**部署轮实测**：插件注册 3 个工具（`honcho_session` / `honcho_context` / `honcho_search_conclusions`）；主线程先按保守默认对 kidpal agent 上 `tools.deny`，**human 当场推翻**。

**human 裁决（2026-10-23，原话大意）**：没必要挡——QQ 渠道里发的信息本来就都是跟小孩相关的，不会有跟小孩不相关的内容发到这个 OpenClaw agent 里来。

**处置**：`tools.deny` 撤销（= 空数组，config valid，热加载生效）；kidpal agent 工具面维持 P1 口径 `allow=[read,write,exec,web_search]`。

**对账影响**：
- 面C（261023-arch-review-memory-wiring-verified-behavior）§6.7 第 5 行「`honcho_*` 可检索风险待部署轮实测，未闭合前按未闭合风险记账」→ **已闭合**：风险按 human 渠道判断接受，不构成残余。
- 面C F4（NFR-PRIV-1 ①「不被复述」半句张力）：随本裁决降级——QQ 渠道内容按使用方式界定为孩子相关事务，「不被复述」的保障重心回到**抽取边界**（deriver 只抽孩子世界的事实，P3 实测回填）；需求面批（/requirement-change，与 REQ-261023 合并起草）按此口径起草措辞。
- 若日后 QQ 渠道使用方式变化（出现与孩子无关的内容），本裁决的适用前提失效，须重裁。
