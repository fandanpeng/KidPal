# retro · 261018-arch-review-sd-log-persist（Track B 全环：R1 BLOCK(F1–F3) → v0.2 → R2 GREEN → landing → landing-verdict GREEN）

> 主线程第一人称热记忆 · 2026-10-18 · 本批为跨面变更 `261018-sd-log-persist` 的**架构面**（实现面承载包已由 `/triage-change` 判 ②单包 Full，未起）· 判官 / 核验官 = 子 agent 形态（零持久上下文，各自独立派发）· 同一变更在同日先经 `/triage-change` 出 reviewed plan，再进本 loop。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | architect v0.1（300 行）→ v0.2（417 行） | 2026-10-18 | 2026-10-18 | 265.2s + 115.7s | 143,988 + 190,369 |
| 评审 R1 | **BLOCK(F1, F2, F3)** | 2026-10-18 | 2026-10-18 | 441.2s | 182,806 |
| 评审 R2 | **GREEN**（靶向复核） | 2026-10-18 | 2026-10-18 | 93.2s | 222,428 |
| FROZEN | human 立项 ack（spine `:55`）+ R2 GREEN | 2026-10-18 | 2026-10-18 | 主线程即时 | 未计量 |
| landing | T1–T8 逐字落 `docs/architecture.md` + `docs/ADR.md` | 2026-10-18 | 2026-10-18 | 主线程即时 | 未计量 |
| 核验 | landing-verifier **GREEN** | 2026-10-18 | 2026-10-18 | 142.2s | 90,592 |

**时间账诚实声明**：上表时长取各 subagent task-notification 的 `duration_ms` 报告值，**不含主线程派发、读文件、编辑与等待间隔**；主线程相位无 harness 计时器可读，故只标「即时」——**不装精确**。

## 2. Token 账

**计量方式**：取各 subagent task-notification 报告的 `subagent_tokens`。**误差界 ±**：该值是每个 agent 的累计口径，跨续跑（architect 起草 → 修订；reviewer R1 → R2）可能叠加或重置，故下表按「agent 累计报告值」读，不与相位严格一一对应；主线程自身无计量接口，未计入。

| 载体 | tokens（报告值） | 备注 |
|---|---|---|
| architect（起草 + 修订） | 143,988 → 190,369 | 初稿 41,574 B / 300 行；修订后 67,009 B / 417 行 |
| architect-reviewer（R1 + R2） | 182,806 → 222,428 | R1 四查 + F1–F3；R2 靶向复核 |
| landing-verifier | 90,592 | 结构门 + 逐 T byte 对账 + 破坏性自检 |

**花费最大处**：reviewer 的两轮（累计报告值最高），其中 R1 的 441s 是全场最长的单次相位——四查要求逐条亲核 canonical 原文，成本主要花在「不采信 PROPOSAL 自述、重跑每一条 census」上。**可压缩分析**：architect 的 v0.1 → v0.2 修订（约 190k）本可避免——F1（ADR census 计数与行号）与 F3（`TECH-261016` trigger 轴）都是**起草期自查义务**，前者只需在落笔前重跑一次 `grep`/`wc -l`，后者只需把 spine 的「同族非同件」结论再拆出「身份轴 / 时机轴」两个问题；两者都不依赖评审输入。首稿即足可省约一轮修订。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · ADR 面 census 计数与行号失准（R1 F1）**：现象 = PROPOSAL 自述 `grep -n "日志" docs/ADR.md` 实跑 5 行并列入 `:94`，实测 4 行（`:45` / `:49` / `:63` / `:97`）；T7 引 ADR-011 范围写 `:94-101`，而文件仅 100 行、`:101` 不存在。根因 = **起草期把「ADR-011 条目体大致范围」与「日志命中行」两个 census 混在一起凭印象写**，没有对每个数字现跑命令留痕；`:94` 是标题行、不含「日志」，被误当命中。证据锚 = PROPOSAL §7 R1「否回项 F1」逐字；reviewer 实跑输出 `4 行` / `100 docs/ADR.md`。

- **F2 · Status 行称「待 human 立项 ack」，与在案 ack 相抵（R1 F2）**：现象 = v0.1 Status 写「待 human 立项 ack」，而 spine `delivery/intake/261018-sd-log-persist.md:55` 已在案 human 2026-10-18 架构面立项 ack。根因 = **主线程的 dispatch 文本写于 human ack 之前，ack 到达后没有同步更新派发指令**——architect 照指令逐字执行，错在编排侧而非起草侧。证据锚 = 本会话第一条派发 prompt 第四节「Status 行」硬要求；R1 F2。

- **F3 · `TECH-261016` 只答了「同件与否」、漏了 trigger 时机轴（R1 F3）**：现象 = §0 / §3 / §6 三处只写「同族非同件 → 不消费、只记账」，未处置该条目 frontmatter 的 trigger「下个动 P3 存储层 / SD 数据面的包……前必须有」。根因 = **spine 的查重结论（同族非同件）被当成完整处置**，而债务条目有两条独立轴：身份轴（是不是同一件）与时机轴（trigger 是否到期）；起草 checklist 只覆盖了前者。证据锚 = `docs/debt/tech/TECH-261016-sd-story-confidentiality.md` frontmatter `trigger:` 行；R1 F3；v0.2 在 §0 / §3 / §6 三处补窄读判定后闭合。

- **F4 · §5 验证集的隐私计数预期公式失准（landing 后由 verifier 发现）**：现象 = FROZEN §5 第 6 条预期 ADR `对话原文:1` / `TECH-261016…:2`，实测 `2 / 3`。根因 = **计数公式按「每条目 1 次」估算**，漏计 ADR-012 标题行的「含对话原文」与关联小节的 `TECH-261016-sd-story-confidentiality`；且 §5 的预期只在起草期跑过基线（0/1），FROZEN 前没有模拟落地后的计数。证据锚 = `landing-verdict.md` D-1；`grep -c` 实测输出。

- **near-miss（最险一刻）**：**§5 计数差异差点被当成落地偏差判 BLOCK**。verifier 首轮比对 ADR-012 时报告 `not-found`，若直接采信，会把一次 CRLF/LF 归一化差异 + 一个预期公式错误读成「落地不保真」。没出事的原因 = verifier 没有停在第一层报错：它逐字符定位后确认 `\r` 差异、剥离围栏头行后重新对账，才把问题降级为 D-1。**这是结构接住的**（核验官被明确要求「独立判断预期失准 vs 落地偏差」），不是运气。

## 4. 优化建议（按 ROI 排序）

1. **验证集的「落地后预期计数」应在 FROZEN 前模拟落地文本再定稿**：治 F4 · 载体 = **登记不硬治（起草纪律，写入本仓 retro 供下批引用）**；若同类复发两次，转 ARQ 收件箱固化到 PROPOSAL 模板 §5 注记 · 预期收益 = 消掉「落地后才发现验证集公式错」这一类非阻断噪声。
2. **`landing-fidelity-check.mjs` 的 `verbatimQuoteAdvisory` 应识别围栏头行与「原：/新：」形态**：治本轮与 `261017` 批同款的固定假阳性（每批落地都要人肉剥头复核）· 载体 = **ARQ 收件箱候选（本会话不写 debt——按编排约束，债务登记归主线程/后续 gov 批）**，可与 `261017` retro 建议 3 合并处置 · 预期收益 = 消掉每次 landing 的固定人工成本。
3. **审计 `landing-fidelity-check.mjs` 的 `advisory.changedCanonicalNotInProposal` baseRef 口径**：治本轮 verifier 观察（`docs/debt/**` 的 8 条改动实为 `HEAD~1..HEAD` 历史提交、非本批工作树）· 载体 = **ARQ 收件箱候选（同上，本会话不写 debt）** · 预期收益 = 避免把上一提交的改动误算进本批、减少每次核验的排除解释。
4. **派发指令中的「状态类字段」在 human 门通过后必须重发或显式更正**：治 F2 · 载体 = **登记不硬治（编排纪律）**——本批 F2 的根因完全在编排侧，architect 无过错 · 预期收益 = 消掉「制品状态与在案事实相抵」这类零语义但必被否回的返工。

## 5. 一句总结

成本 = 本面 3 个 agent 相位（architect ×2、reviewer ×2、verifier ×1；subagent 报告 token 合计约 830k、时长合计约 1,057s）+ 制品 4 件 · 触点 = human **2 次**（立项 ack + 本收口指令）· 最险一刻 = §5 计数差异差点被误判为落地偏差 · **保险丝判定：两方独立结构（判官独立 + verifier 独立）接住了作者偏差**——判官 R1 抓出 F1/F2/F3 三条 SSOT 义务缺口（其中 F2 实际是编排侧错误），verifier 独立逐 T byte 对账 + 破坏性自检，既证实了落地保真，也把 §5 公式错误准确定性为 D-1 而非阻断 · 最值得带走的一条 = **债务条目的「身份轴」与「时机轴」必须分开答**（F3 的教训）：同族非同件不等于 trigger 未到期，trigger 判定要单独落笔。

## 6. 本环遗留

- **隐私记账（必办）**：向 `docs/debt/tech/TECH-261016-sd-story-confidentiality.md` 正文 append 暴露面加宽 + trigger 已过目一句——归主线程收口时落，本会话未写。
- **spine 回填**：`delivery/intake/261018-sd-log-persist.md` 的 `anchors.arch` 回填为「PROPOSAL FROZEN + landing 完成（commit <sha>）」、`status` 相应推进——归主线程，本会话未写。
- **实现面承载包**：TaskPackage `261018-sd-log-persist`（②单包 Full）launch 硬前置已满足（arch FROZEN + landed），后续由 dev-loop 推进。
- **合并**：本批 commit 落 `dev/pod-m2`；合并本地 main 归主线程 `merge-to-main`，本会话未做。
- **ARQ 候选两条**（§4 建议 2、3）：`landing-fidelity` verbatim advisor 假阳性 / baseRef 口径——本会话按编排约束未写 debt 条目，留主线程或后续 gov 批登记。
