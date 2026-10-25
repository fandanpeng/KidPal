# retro · 261025-gov-review-release-pr-publish（Track B 全环：起草 → 评审 4 轮 → 落地 → 核验）

> 主线程第一人称热记忆 · 2026-10-25 · 治理面第 N 位乘客（本会话内无跨乘客对照基线）· harness 形态 = 本仓 typed `architect` / `architect-reviewer` / `landing-verifier` 三 agent 全在场，非零上下文降级形态。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 立项 | human 原话 + 两问裁边界（OQ 先裁自动化边界与对账） | ~19:25 | ~19:27 | ~2 min | — |
| 起草 | vet r0.1（5 T，含条件项 T5） | ~19:27 | ~19:41 | ~13.3 min | 246,350（agent 累计首轮） |
| 立项 ack | human 四裁（OQ1–OQ4） | ~19:42 | ~19:45 | ~3 min | — |
| 修订 v0.2 | T5 裁剪 + 四裁入册 | ~19:45 | ~19:48 | ~3 min | 33,158（增量，累计 279,508） |
| 评审 R1 | BLOCK(F1–F9) | ~19:48 | ~19:54 | ~6.4 min | 131,809 |
| 修订 v0.3 | F1–F9 逐条 | ~19:55 | ~19:58 | ~3 min | 37,593（累计 317,101） |
| 评审 R2 | BLOCK(F10)+N1–N3 | ~19:58 | ~20:00 | ~2.2 min | 39,012（累计 170,821） |
| 修订 v0.4 | F10 + N1–N3 | ~20:00 | ~20:01 | ~1.2 min | 15,110（累计 332,211） |
| 评审 R3 | GREEN + N4/N5 | ~20:01 | ~20:02 | ~1.5 min | 22,885（累计 193,906） |
| 修订 v0.5 | N4/N5 收口 | ~20:02 | ~20:02 | ~0.5 min | 6,083（累计 338,294） |
| 评审 R4 | GREEN 维持 | ~20:02 | ~20:03 | ~0.6 min | 6,672（累计 200,578） |
| FROZEN + 起草 commit | Status 翻 FROZEN v1.0 | 20:03 | 20:03:38 | <1 min | — |
| landing | T1–T4 落地 + 自检 | 20:03 | 20:07:32 | ~4 min | — |
| 核验 | landing-verifier PASS + 7 ADVISORY | 20:07 | 20:11:21 | ~3.2 min | 96,536 |
| 落地 commit | 9 文件 | 20:12:02 | | <1 min | — |
| **合计** | | ~19:25 | ~20:12 | **~47 min（±5）** | agent 侧 635,408 |

时长按制品 mtime 链 + 子 agent 自报 duration 推算，误差界 ±5 分钟。

## 2. Token 账

计量方式：子 agent 侧取 harness 回报的累计 `subagent_tokens`（可信）；主线程无独立计量，按会话 `total_tokens` 差值估计（约 2.5×10^5，误差界 ±30%）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（立项 / 编排 / landing / 记账） | ~250,000（估） | ~28% | 含通读 PROPOSAL 与 6 个 canonical 文件 |
| architect（5 次运行，累计） | 338,294 | ~35% | 起草 + 四次修订；v0.1 首轮独占 246k |
| architect-reviewer（4 轮，累计） | 200,578 | ~21% | R1 最重（132k），R2–R4 递减 |
| landing-verifier | 96,536 | ~10% | 六项清单 + 全量验证集复跑 |
| **合计** | **~885,000（±50k）** | | |

花费最大处 = 起草首轮（246k，占全环 28%）：PROPOSAL 模板的【硬项】要求每条断言起草期实测，agent 需要亲跑大量命令取证。可压缩分析：条件项 T5（gh pr merge 拦截）最终被 human 裁掉，起草期约 1/5 的取证量花在一个未落地的 T 上——「条件项先裁后起草」可省这部分。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 验证集把行为约束写成源码字符串断言**：现象 = R1 判 BLOCKING，三处「源码零字符串」grep 与自检断言互斥，只能靠刻意拼串同时满足，而拼串后的断言证不了任何行为。根因 = 起草时把「push 不能带 --force」这类行为约束，直觉地落到 `grep -c "--force" = 0` 的形态上——负向字符串检查看起来最机械，实际是真空绿的反面。证据锚 = PROPOSAL §7 R1-F1（`release-pr.mjs` V1b 三条 = 0 断言 vs T1 第 7/10 条自检断言）。
- **F2 · 白名单 ⊆ 不等于正向行为断言**：现象 = R2 判 BLOCK(F10)：gh 子命令集合 ⊆ 白名单，脚本一次 PR 都不开也满足。根因 = 只写了「不允许什么」，没写「happy path 上必须恰好发生什么」。证据锚 = §7 R2-F10；修法（`publish-happy-path` 恰一次 `pr create` + 相邻 flag 对）由 architect 在 v0.4 补齐。
- **F3 · 条件项的取证成本被沉没**：现象 = T5（gh pr merge 拦截）在起草期做了完整证据与 11 条自检设计，human 随后裁 (b) 删除。根因 = 立项时先起草后裁 OQ，方向性选项与事实取证耦合在同一步。证据锚 = PROPOSAL §4 OQ1 裁定行 + v0.2 删除 T5 的修订记录。
- **F4 · 落地期一次编辑误删既有代码行**：现象 = 在 `merge-driver.mjs` 插入 T9 断言时，Edit 的 old_string 多带了一行 `results.forEach((r) => {`，替换后该行消失，自检函数的循环壳被截断。根因 = 用「读到的相邻三行」做锚点却没有把整段包含进 new_string。**没出事的原因**：插入后立即定点 Read 该区域（1328–1341）复核，看到 1335 行悬空，当场补回。证据锚 = `merge-driver.mjs:1333-1339` 现形（恢复后的 T9 块 + forEach）。
- **F5 · 面结构门 verbatim 对账对长行子句产生假阴性**：现象 = `landing-fidelity-check` 的 `verbatimQuoteAdvisory.missing` 把 DEV-GOV:214 与 protocol:58 的新句报 not-found，实际两串逐字在场。根因 = PROPOSAL 围栏块带尾换行，而目标文件里该句是长段中的子句、后面还有文字，`includes` 因尾换行失配。证据锚 = FROZEN PROPOSAL §2 T4 第 2/4 块 vs `docs/DEV-GOVERNANCE.md:214` / `arch-review-protocol.md:58`；verifier ADVISORY A5 同判。
- **near-miss（最险一刻）**：F4 的误删发生在我落地 G3 承重脚本的中间态——若没有紧接的定点复核，`merge-driver.mjs` 会以语法错误形态进入后续自检，而 `--self-check-merge-friction` 正是本批的回归门；更糟的是它可能被「commit 成功」的假象掩盖，直到下次真实合并才炸。没出事靠的是「改完立即读回被改区域」这一步，而不是任何自动门。

## 4. 优化建议（按 ROI 排序）

1. **验证集模板补一句硬规则**：负向白名单断言（「不含 / 只允许」）必须配一条正向 happy-path 断言（「恰一次」「相邻 flag 对」）。治 F1 + F2（同根）· 载体 = **ARQ 收件箱**（`docs/debt/gov/`，建议 id 形如 `ARQ-261025-verify-set-falsifiability-pairing`）· 预期收益 = 这类 BLOCKING 在评审期平均多耗一轮（本批 R1→R2 两轮都栽在同根）。
2. **条件项先裁后起草**：PROPOSAL 起草前把「可能被 human 裁掉的 T」列成 OQ 先裁，再决定是否取证。治 F3 · 载体 = **留批次**（写入下一次 Track B 主线程的操作习惯，不固化脚本）· 预期收益 = 本批可省约 1/5 起草 tokens。
3. **面结构门的 verbatim 对账改为 trim 后比较**：`checkVerbatimQuotes` 现用含尾换行的 `includes`，对「子句替换」形态恒假阴性。治 F5 · 载体 = **ARQ 收件箱**（脚本面改进，`landing-fidelity-check.mjs`）· 预期收益 = 每批少一条需人工解释的 advisory。
4. **落地编辑的 old_string 纪律**：凡插入型 Edit，old_string 只取「真正要被替换的最小锚」，new_string 必须把 old_string 全文原样包含后再追加。治 F4 · 载体 = **登记不硬治**（属操作纪律，无法机械门）· 预期收益 = 消除本类误删的复发面。

## 5. 一句总结

成本 ~47 分钟（±5）/ agent 侧 ~885k tokens（±50k）/ human 触点 2 次（立项边界两问 + 四裁）/ 制品 4 件齐；最险一刻 = 落地 merge-driver 时误删自检函数的循环壳、靠改后立即读回复核兜住；**保险丝判定：两方独立结构（判官独立 + verifier 独立）接住了作者偏差**——判官在 R1/R2 各抓一次同根缺陷（负向断言缺正向配对），verifier 又独立复跑全验证集并另出 7 条 ADVISORY，二者都不是主线程自评；最值得带走的一条：发布分支 tip 恒等于推送时的本地 main tip，PR merge commit 才会把本地 main 放进祖先链，`merge --ff-only` 才永远成立——这是整条流程的结构性承重点，比脚本本身更重要。

## 6. 本环遗留（条件节）

- **首次真实 `--publish` 是上线事件**：本轮只到夹具与 dry-run（主 worktree 因 `delivery/review/INDEX.md` 未提交改动正确 fail-closed）。首跑须 human 在场，结果补记。
- **`gh pr merge` 无机械牙**（human 裁 OQ1=(b)）：只留文档约定；`gh api` / `curl` / 网页同不在覆盖。
- **`release-pr.mjs` 的 push 在 `deny-main-push.mjs` 视野外**：防线 = push argv 白名单 / G3 写权 / landing 人读 diff。
- **7 条 ADVISORY 处置**：A1（happy path 未对 `calls.gh` 全体做字面集合相等）与 A4（gh auth 未显式断言 `repo` scope）→ 维护顺手收紧，不进本批；A3（`${merged}` vs `${mergedCount}`）与 A2（幂等闸先于命名）→ 无行动；A5（verbatim 假阴性）→ 见 §4 建议 3；A6（派遣清单仍提本仓不存在的 `cross-surface-check.mjs`）→ 与既有悬空引用同根，留 ARQ；A7（verifier 未在 main worktree 复跑 dry-run，以夹具佐证）→ 主线程已实跑，证据在 landing-summary §2。
- **存量 14 条 `release/*` 分支不清理**（human 裁 OQ4=(a)）。
