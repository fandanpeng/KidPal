---
id: ARQ-261021-landing-summary-template
title: landing-summary 结构硬项（packet-id 行 + manifest 围栏）无模板承载，仅活在门脚本与先例文件里
domain: gov
status: open
severity: SHOULD
owner: architect
trigger: 下一批 Track B landing（判据可机判：landing-fidelity-check 首跑是否因 summary 格式 FAIL）
opened: 2026-10-21
closed: ""
closedBy: ""
paths: ["delivery/review/261019-arch-review-p3-record-dialogue-guide/architect-landing-summary.md", ".claude/references/arch-change.md"]
source: 261021-arch-review-p3-record-cancel-lifecycle retro F2 + landing-verdict FAIL 段（retro.md §3 F2 / §4 建议 2）
---

**事实**：261021 批 landing-summary 首跑 landing-fidelity-check.mjs FAIL，唯二阻塞 = ① 缺 ```landing-manifest``` 围栏块（门硬失败条目 7）② 缺字面「packet-id」token（packet-binding 判据，硬失败条目 3）。这两件是结构门判据，但无 summary 模板/义务条文承载——261019 先例同件 :3（packet-id 行）/:39（manifest 块）是事实标准，零上下文主线程凭先例对照才补齐，白耗一轮 FAIL→补件→翻正环。

**修法方向（归 gov-change 消费时裁）**：二选一——① 立 `docs/templates/LANDING-SUMMARY.template.md` 最小骨架（packet-id 行 / ```landing-manifest``` 围栏 / 落地↔改法对应表 / 偏差申报节四件硬项）；② arch-change.md Track B landing 义务节加两句字面要求（成本更低，推荐先做，模板视复发再立）。

**判据（机判）**：landing-fidelity-check.mjs 首跑 ok:true（格式层零 FAIL）的批次占比；跟踪两批，若再因 summary 格式 FAIL 则立模板。

**Why**：结构门 FAIL 属 fail-loud 设计意图，但 FAIL 原因若是「义务未成文」而非「落地失真」，则门在惩罚信息不对称而非保真度——成文即消。

**关联**：261021 retro §6 旁注——intake INDEX 对 spine faceMap 平文字串的投影空值 + ⚠值域外（changes-index.mjs 解析面），同批交 loop-retro 观察面。
