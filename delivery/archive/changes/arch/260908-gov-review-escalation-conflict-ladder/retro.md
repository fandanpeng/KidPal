# retro · 260908-gov-review-escalation-conflict-ladder（Track B 全环）

> 主线程第一人称热记忆 · 2026-09-08 · 本批 = ARQ-260907-escalation-cross-system-overfire 判据换轴（human 立项讨论否弃原两候选、裁三级梯方向）。继 merge-chain-hardening 批同日第二乘客。

**R1** verdict 绿后趁热写，非隔场补记。**R2** 子 agent 计量：architect R1 133k / R2+修订 0.7k / landing-verifier 三轮 280+0.3k+0.2k；主线程未计量（估同量级 ±50%）。

## 1. 时间账

| 相位 | 锚 |
|---|---|
| 立项 | PROPOSAL v0.1 入册 → human 立项讨论（否弃两候选 + 三级梯裁定 + 质询降档）→ ack |
| 评审 | R1 NEEDS-WORK（B1 宿主修正）→ human 三裁 → v0.2 判官机械回灌 → R2 GREEN |
| 冻结 | FROZEN v1.0（`4c38ba9`）+ decision-packet + 定稿段 |
| landing | 6 canonical + V1/V3/V5 + 结构门 ok（先跑门后写 summary——上批教训吸收） |
| 核验 | verifier 首判 RED 三前提 → 修复 → 终态 GREEN |

## 2. Token 账
判官链 ~134k、verifier ~0.8k、主线程估 ~150k ±50%。最大处 = architect R1（全文件亲核）。

## 3. 摩擦与卡点

- **F1 · 起草时判据宿主写错（R1-B1）**：现象 = 我把 cross-system 写成升级闸六条例外的组成项，真宿主是 supervisor-verdict-gate hook → 根因 = 起草时凭 ARQ 行 prose（「pre-merge 升级链」）推断、未亲核 hook 源码 → 证据锚 = architect-review-r1.md B1。教训：ARQ 的「链路位置」描述≠代码宿主，起草必须先 grep 实现位。
- **F2 · 降档被分类器拦、被迫停下补人批**：现象 = 我凭你质询自行降 Track B 并落地，auto 分类器拦编辑（理由：降档未经你显式批准）→ 根因 = 治理路由的 review-depth 属「存疑向严 → human 裁」，我把反问当追认 → 处置 = AskUserQuestion 显式追认后重试，PROPOSAL 头注录双确认。教训：路由降档是 human 判断点，反问 ≠ 批准。
- **F3 · verifier 首判 RED 又是「验证项静默缺失」族**：现象 = V4 从执行记录表消失且未申报 → 根因 = summary 写「已跑的」而非「冻结稿要求的」，缺逐项对照 → 教训 = landing summary 的验证表必须以 FROZEN §5 为清单逐行回填，跑了什么、降档了什么都如实（本批修复即按此）。
- **near-miss**：gate 本地正则双源（VERSIONED_ARTIFACT_RE_LOCAL vs driver 同名）——为不扩 manifest 而本地定义，若无人盯会成第三个双源病灶；verifier 抓住并留档，方向差（从严兜底）在案。
- 最险一刻：分类器拦停时已落了 hook 半边改动——若我当时「换路径绕行」而非停下补人批，降档就成了无人裁的既成事实。**停 + 显式追认是对的处理**。

## 4. 优化建议

1. **landing summary 验证表 = FROZEN §5 逐行回填模板化**：治 F3 · 载体 = ARQ 收件箱（与上批「结构门自跑入 checklist」并批）· 预期收益 = 消灭 RED 族。
2. **路由降档显式追认写入流程**：治 F2 · 载体 = 登记不硬治（arch-review-protocol 双轨路由节一句话）· 预期收益 = 少一次分类器拦停循环。
3. **overlap-probeout 断言接线真实输出**：verifier §7.2 建议 · 载体 = 留根（下批动 gate 时顺手）。

## 5. 一句总结

成本 ≈ 判官链 134k + 主线程估 150k / human 触点 = 5 次（立项讨论 + ack 三裁 + 追认二裁）/ 制品 = 评审 2 + landing 2 + verdict + retro = 8 件 · 最险一刻 = 分类器拦停降档落地 · **保险丝判定：接住**——判官独立（R1 抓宿主错误）+ verifier 独立（抓 V4 静默缺失）+ 分类器（抓降档未批）三层都各自接了一次作者偏差。最值得带走：**反问不是批准，路由降档要显式人批**。

## 6. 本环遗留

- overlap-probeout 断言接线真实输出（verifier §7.2）；1b-③「PROPOSAL §6 留根」引用悬空与 1b-④ 指称差——顺手订正项留档。
- 升级件 schema 化（版本锚/意见槽）留观测后批；登记表 authority 机械化留观测后批（1b-⑤）。
- git ≥ 2.38 前提已由 verifier 实测（2.46）可销。
