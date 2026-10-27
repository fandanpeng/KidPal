---
id: ARQ-261026-verbatim-extract-backtick-cut
title: checkVerbatimQuotes 的围栏提取正则把块内容中的 ``` 字面（js 夹具串形态）当块边界——观察列大量伪 missing，噪声稀释 advisory 可信度
domain: gov
status: closed
severity: NIT
owner: architect
trigger: 下一次动 `.claude/scripts/landing-fidelity-check.mjs` verbatim-quote 面的治理批
opened: 2026-10-26
closed: 2026-10-27
closedBy: 261027-gov-review-p6-landing-gate-cluster
paths: [.claude/scripts/landing-fidelity-check.mjs]
source: [delivery/review/261026-gov-review-p2-anchor-template-cluster/landing-verdict.md（ADVISORY-1）, delivery/review/261026-gov-review-p2-anchor-template-cluster/retro.md（F3）]
---

**证据**：261026 批 landing 结构门实测：`landing-fidelity-check.mjs` verbatim-quote 观察列 24 条 missing，其中大量为伪 missing——PROPOSAL §2 的 js 夹具串内含 ``` 字面（如 `'/^\s*```/.test(l)'`、`'```md'` 数组元素），提取正则 `/```[a-zA-Z]*\n([\s\S]*?)```/g` 把代码内 ``` 当块边界提前闭合，块被腰斩、其后散文被误当「钉死块」。检查本身 advisory-only（exit 0、不阻断），但观察列噪声每次都要人核一遍——与 ARQ-261019-verbatim（已结）的「狼来了」同族。

**影响面**：凡 §2 夹具串内含 ``` 字面的 PROPOSAL（门脚本批几乎必现——修门脚本的批要在夹具里写围栏字面）；核验者每批多付一轮「伪 missing 逐条排除」裁定量。

**建议改动**（候选，交 review、不自批）：闭合围栏判定加「独立成行」条件（如 `/^```\s*$/m` 或逐行扫描时要求闭合行首无其它内容），块内容中的行内 ``` 字面不再作边界；补一条「js 夹具串含 ``` 字面、块仍完整提取」的自检夹具。

**风险**：riskTier: low——提取精度提高属观察列噪声减少方向，advisory-only 不改任何 RED/exit 语义。

**关联**：261026 批已修的「块尾换行假阴性」（ARQ-261019-verbatim-quote-line-level-false-negative，已结）与「§2 切片跳围栏」（T1）同检查面；本条是其相邻新形态，登记时检查面刚被 261026 批动过，落点行号以动工时复测为准。

**2026-10-27 · 结项（`261027-gov-review-p6-landing-gate-cluster`，T2）**：`checkVerbatimQuotes` 的围栏提取改为逐行状态机（`extractFencedBlocks`）——开行 = 行首可选空白 + ``` + 可选 info 串，闭行须**独立成行**（``` 后只允许空白）；块内容中的行内 ``` 字面（js 夹具串形态，如 `'/^\s*```/.test(l)'`、`'```md'` 数组元素）不再作边界。自检新增 `verbatim-backtick-in-content-*` 三条（块完整提取 + 命中 + 无 missing），判别键 = 块完整断言（反转实现把块切在首个行内 ``` 处、checked[0] 变截断串即红——截断串仍是目标文件子串，missing 不翻转，故以块完整为准）。同批的 `--cite-check` 主门 advisory 共用新提取器。留根：`fenceFlags` / `parseSection2AnchorLists` 的 `fenceLineFlags` 仍用「行首 ``` 即翻转」的宽松态（只用于节域近似，未随本批改——若块内容出现**行首** ``` 仍可能错切节域，见该批 PROPOSAL §6 留根）。
