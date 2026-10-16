# retro · 260928-gov-review-cross-face-intake-spine（Track B 全环：跨面 intake + spine 机制批）

> 主线程第一人称热记忆 · 2026-09-28 · harness 形态 = 判官（architect）与核验官（landing-verifier）均为零上下文子 agent，经 SendMessage 续轮复用上下文。
> 承载 ARQ-260928-triage-cross-face-intake-gap 建议 1/3/4；human 全程在场：四轮形态讨论 + 立项 ack + OQ 四裁 + Track A→B 改判。

## 1. 时间账

| 相位 | 内容 | 时长（实测） |
|---|---|---|
| 讨论 | 四轮 showme 形态讨论（输入两层论 → 落点候选 C → 发起机制/headless → 门表 + 三级处置） | 同日跨数小时（human 节奏） |
| 起草 | PROPOSAL v0.1 起草 + 起草期实证 + preflight | ~40 min |
| OQ 呈裁 | 四 OQ 澄清与呈裁（两轮问答） | ~10 min |
| 评审 R1 | architect 判 BLOCK（2 BLOCKING + 6 MINOR） | 397 s（子 agent 实测） |
| 修订 | F1–F8 逐条修订（15 处 Edit） | ~25 min |
| 评审 R2 | architect 判 GREEN（R2-M1/M2 残留顺手修级） | 170 s（实测） |
| FROZEN | 顺手修 + Status 标冻 + commit d62f1cd | ~10 min |
| landing | T1–T7 十文件落地 + landing-summary | ~45 min |
| 核验 | verifier 第一轮 FAIL（summary 双格式缺陷）→ 修复 → 第二轮 PASS | 387 s + 109 s（实测） |
| **合计** | — | — |

## 2. Token 账

计量方式诚实声明：判官 / 核验官有 harness 实测（判官 R1+R2 = 184,924 tokens / 31 次工具调用；核验官两轮 = 103,628 tokens / 21 次）；主线程无逐段计量，估计占大头（起草 + 修订 + landing 全程长上下文），误差界 ±40%。

| 载体 | tokens | 备注 |
|---|---|---|
| 主线程（讨论 + 起草 + 修订 + landing + 收口） | ~250k（估，±40%） | 含四轮讨论上下文 |
| 判官（R1+R2） | 184,924 | 实测 |
| 核验官（两轮） | 103,628 | 实测 |
| **合计** | ~540k（估） | 子 agent 两载体重计量可复用 |

花费最大处 = 主线程讨论 + landing。可压缩分析：四轮讨论是 human 拍板必需、不可压；landing 摩擦见 F1/F2，可压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · census 断言失实（R1 唯一 BLOCKING 之一）**：现象 = 起草期断言「第一刀」全仓 3 处，判官复跑实测 9 处（docs/crq 历史行 3 + showme 3 处集外）→ 根因 = 断言覆盖集合未先定义 scope 再 grep（把「门内术语」直觉当成了全仓事实）→ 证据锚 = review.md R1 F1（判官实测输出在案）。
- **F2 · V1 止于 --preflight，完整门 landing 后才首跑（R1 后暴露双缺陷）**：现象 = landing-summary 缺 manifest 围栏块 + packet-binding 字面 token，到 verifier 才抓 → 根因 = 验证集把 `--preflight` 当 V1，但 preflight 不检 summary 围栏与 binding；完整门 landing 后从未实跑 → 证据锚 = landing-verdict.md 第一轮 FAIL 段 + 门首错即停导致第一轮未报 binding（verifier 已留痕澄清）。
- **F3 · T1 长文 Write 序列化损坏 ×2**：现象 = change-intake.md 全文单次 Write 两次输出劣化（乱码 / 重复行 / 噪音行混入），一次混入元评论噪音 → 根因 = 长中文内容单次 Write 的输出序列化损坏风险 + 我凭「Write 成功」自信未立即机检 → 证据锚 = architect-landing-summary.md 执行事故记录（含终件 grep 零噪音 + 全文人读核对）。
- **F4 · 分块编辑抄写摩擦**：现象 = landing 十文件 5 处 MINOR 逐字偏差（标点形变『』→「」、注记 token 丢失、「本批立」→「…立」）→ 根因 = 小步编辑的人读转录误差 → 证据锚 = landing-verdict.md 差异 #2–#6。
- **near-miss（最险一刻）**：T1 损坏若未自查会带病入库 canonical（change-intake.md 是全批唯一新增 SSOT）——被「Read 复核 + 噪音 grep 自查」接住，未进 commit。教训：**生成即验证，写入后立即机检自身产物，不凭工具「成功」自信**。

## 4. 优化建议（按 ROI 排序）

1. **完整结构门进验证集**：治 F2 · 载体 = ARQ 收件箱（登记为 PROPOSAL 自检句①扩写候选——V1 应含「summary 落盘后立即跑完整门」，或 PROPOSAL.template 验证集模板改双形态）· 预期收益 = 消灭「verifier 才抓 summary 格式缺陷」整类返工。
2. **长文落地分块 + 写后自检纪律成文**：治 F3 · 载体 = ARQ 收件箱（登记为自检句③「生成即验证」扩写候选）· 预期收益 = 防序列化损坏带病入库。
3. **census 断言先定义 scope 再 grep**：治 F1 · 载体 = 登记不硬治（硬项 1a 已有此纪律，本批是执行失守非纪律缺位；retro 记录即可）· 预期收益 = 下批起草时想起来。
4. **5 处 MINOR 逐字偏差**：治 F4 · 载体 = 登记不硬治（verdict 差异清单已留痕）· 预期收益 = 留痕即可。

##  §5. 一句总结

总成本 ~540k tokens（估）/ human 出场 4 次（讨论×4 轮 + 立项 + OQ 两轮，同日集中）/ 制品 4 件齐 + showme 讨论稿 · 最险一刻 = T1 损坏险些带病入库（被写后自查接住）· **保险丝判定：两方独立结构接住作者偏差**——判官 R1 抓住 census 失实 + sweep 漂（作者盲区一）；verifier 抓住 summary 双格式缺陷（作者盲区二）；两方均独立于作者、均真接住，无一次靠作者自查过门。最值得带走的一条：**preflight ≠ 完整门，summary 落盘后立即跑完整门**。
最值得带走的一条（重复强调）：F2 的教训是验证集设计问题不是执行疏忽——`--preflight` 只保 PROPOSAL 形态，不保 summary 格式；完整门对 summary 侧的检查只在 landing 后可跑，验证集应把「summary 落盘后跑完整门」设为显式一步。
