# retro · 261019-gov-review-docs-root-drift-cleanup（Track B 全环：立项 → 起草 v0.1 → R1 BLOCK(F1) → 否回 v0.2 → R2 GREEN → FROZEN v1.0 → landing T1–T9 → 核验 PASS）

> 主线程第一人称热记忆 · 2026-10-19 · gov lane INDEX 在册第 7 批 · harness 形态：typed agents 全配（architect / architect-reviewer / landing-verifier），判官与核验官简报 canonical 原样携带 · 跨乘客对照：同日第二批 gov 变更（前批 = 261019-gov-review-docs-root-ledger-rehome，本批 T4 正是其引用改指漏网的连带清理）。

**【硬项 R1 · 环后趁热、第一人称】** landing-verdict PASS 后即写，第一人称热记忆，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens / 时长有 harness usage 计量（精确）；主线程无计量，§2 以子 agent 实测 + 主线程步骤数估计，误差界声明在下。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 带 现象 → 根因 → 证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议标载体。

**【硬项 R5 · 保险丝判定】** §5 含两方独立结构判定。

## 1. 时间账

| 相位 | 内容 | 时长 | tokens |
|---|---|---|---|
| 立项 | human 前端对齐（三句话拍 (a) 方案 + 指 gov-change）+ 主线程勘查（漂移清单 8 处、事实核验） | 人时两段会话，未精确计时 | 主线程无计量 |
| 起草 | architect PROPOSAL DRAFT v0.1（252 行，含独立复核纠正主线程简报两处） | 714 s（harness 计量） | 85,094 |
| 评审 R1 | architect-reviewer 四查 + 判档 + 面归属专核 → BLOCK(F1)（1 SHOULD + 3 NIT） | 549 s | 79,300 |
| 否回修订 | architect 修 F1–F4 全采纳 → DRAFT v0.2 | 306 s | 累计 97,142 |
| 评审 R2 | 同判官聚焦复核 → GREEN（含音量调节日期戳「裁保留」裁决） | 153 s | 累计 99,512 |
| FROZEN | 主线程凭立项 ack + GREEN 标 FROZEN v1.0（闸锚形态 `FROZEN v\d+\.\d+` 先行核）+ upsert | — | 主线程 |
| landing | 主线程 packet-bound 落 T1–T9（architecture.md 6 改点 + DEV-GOVERNANCE.md 3 改点）+ §5 探针 1–7 全绿 + landing summary | — | 主线程 |
| 核验 | landing-verifier 六项核验 → PASS + 2 ADVISORY | 197 s | 54,364 |
| **合计** | 子 agent 五段 | 1,919 s（≈32 min，子 agent 侧） | ≈318k（子 agent 侧，含 resume 累计去重后 ≈236k） |

对照：同日前批 ledger-rehome 为 48 锚机器面大批；本批 2 文件 9 改点纯文档批，评审一轮否回即绿，属轻量 Track B 常乘客。

## 2. Token 账

计量方式诚实声明：子 agent tokens = harness usage 字段实测（resume 累计数已按末次减前次折算去重）；主线程（勘查 + 编排 + 落地 + retro）无计量，按本会话上下文增长粗估 ±50%，不装精确。

| 载体 | tokens | 备注 |
|---|---|---|
| 主线程（勘查 + 编排 + 落地） | 无计量（粗估 ≈100k ±50%，含两轮全文件 Read 与 PROPOSAL 全文读） | 大头 = 证据面通读 |
| architect（起草 + 否回） | 85,094 + 12,048 = 97,142 | 起草含独立复核 25 次工具调用 |
| architect-reviewer（R1 + R2） | 79,300 + 20,212 = 99,512 | R1 全面审 25 次调用；R2 聚焦 5 次 |
| landing-verifier | 54,364 | 12 次调用 |

花费最大处：判官 R1 全面审（四查 + 面归属 + (a) 保真 + 验证集反照）。可压缩分析：R1 的亲核留痕质量直接换掉了后续一切返工，这笔是 Track B 的设计性花费、不压；真正可省的是主线程勘查简报的失真返工（见 F1/F2）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 被治的病灶在改法文本里复发（R1 BLOCK）**：现象 = architect 起草的 T2 新 bullet 在自宣「本档不再内嵌逐条状态，防再漂移」的同句里内嵌了「2026-10-19 快照：7 条全 deferred」日期戳快照——本批要根治的写法在新文里重演。根因 = 起草时把「举证时点」（一次性勘查证据）与「状态断言」（会随账本翻假的存量声明）混写进同一句。证据锚 = PROPOSAL §7 R1 F1（:271）；判官修法一行删改，R2 核到位。
- **F2 · 主线程勘查简报两处失真，靠起草方亲核接住**：现象 = 我派发的简报写「CDQ 6 条」（实测 7 条，漏 CDQ-260924-m4-k1-storytext-seq）、引 §10 自宣作「不内嵌注记」（原文 = 「不内嵌 ADR 注记」）。根因 = 勘查时凭 `ls` 输出转述计数、凭记忆缩写引文，未把最后一跑实测钉进简报——与在册记忆「别把看到的一处当全部」「现读再粘」同族复发。证据锚 = architect 起草报告「起草期独立复核结果」两条；PROPOSAL §2 T2 证据段按实测 7 条落笔。
- **F3 · landing 期 T8b 首次 Edit 凭记忆拼相邻行未命中**：现象 = 删 §8 缺口条目时我按记忆拼「该行下邻 = `.claude/**` 一致性机械门行」，实际相邻关系相反，Edit 未命中（零写入）。根因 = 同 F2：拼接 old_string 凭记忆而非现读原文。证据锚 = Edit tool 两次 error 返回；重读 :242-253 后一次命中。
- **near-miss（最险一刻）**：R1 若不拦 F1，「7 条全 deferred」快照会随 FROZEN 落进 architecture.md 正文——下一本 CDQ 翻状态（如音量调节补登记，留根 4 在案）即让 SSOT 再度说谎，而那时已无评审眼睛盯着这行。没出事的唯一原因 = 判官独立实例对「改法文本是否自带被治病灶」做了元层检查。教训：清理类批次要专门反照「新文本身是否通过本批自己的清理判据」。

## 4. 优化建议（按 ROI 排序）

1. **清理/修正类批次加一条起草反照规约：「新文须通过本批自己的清理判据」**——治 F1 · 载体 = 固化候选 → ARQ 收件箱（建议落 gov 域，作为此类批 PROPOSAL §2 自查行 + 判官透镜一句）· 预期收益 = 把 R1 这次的元层拦截从判官个人警觉变成制度。
2. **勘查简报的数字与引文由派发前最后一跑实测填充，不凭转述**——治 F2 · 载体 = 登记不硬治（主线程纪律，与在册记忆 partial-view-as-whole / carry-canonical-briefs-verbatim 同族，不另立条目）· 预期收益 = 省掉起草方逐条纠错的隐形成本。
3. **landing 期 Edit 的 old_string 一律现读拼接（禁凭记忆拼相邻行）**——治 F3 · 载体 = 登记不硬治（同上族）· 预期收益 = 消掉一类零写入返工。
4. **「音量调节」补登记进 debt contract 域**——留根 4 · 载体 = 留批次（主线程后续 human-approved 批次处理，本批不擅自补登）。
5. **日落复盘项：迁移批引用普查何以漏掉同文件两处行内链接**——留根 6 · 载体 = 留批次（loop-retro 对照本批与前批 ledger-rehome 的普查方法）。

## 5. 一句总结

成本 ≈236k 子 agent tokens / 子 agent 侧 ≈32 分钟 / human 触点 2 次（前端对齐拍 (a) + 看草稿后放行）/ 制品 4 件齐 · 最险一刻 = R1 F1（被治病灶险些随新文落进 SSOT）· **保险丝判定：两方独立结构接住作者偏差**——判官独立实例拦下改法文本自带的病灶（F1），landing-verifier 独立逐字核十改点无私货（PASS），起草方还独立纠正了主线程简报两处失真（F2），三层独立各接住一次 · 最值得带走的一条：清理类批次的第一判据要反照自身——新文若通不过本批自己的清理标准，判再多旧账也是白干。

## 6. 本环遗留

留根 7 项去向 = PROPOSAL §6 原样（:47 --no-ff 已核排除 / 收编注本体保留 / §10 其余决策注记留 human 续裁 / 音量调节补登记归主线程 / contract 域条目处置不属本批 / 日落复盘项交 loop-retro / architecture.md :4 裸括注后续批顺手对齐）；landing-verifier ADVISORY 2 条（summary 真空绿声明句式形制提示 / verbatimQuoteAdvisory missing 为替换成功预期副产物）——登记不硬治；§5 第 8 条 post-merge 簿记待合并后执行（锚机检复跑 + `--upsert` mergedInMain 归位）。
