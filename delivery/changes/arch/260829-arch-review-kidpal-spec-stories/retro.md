# retro · 260829-arch-review-kidpal-spec-stories（Track B 全环：spec story 细化 + 一处 additive 契约字段）

> 主线程第一人称热记忆 · 2026-08-29 · 本仓 Track B 第 1 乘客 · harness 形态：architect / architect-reviewer / landing-verifier 均为子 agent 实例（architect 跨 R1/R2/R3 续同一实例保上下文）。本次同时是本仓 Track B 流程的**首次实跑**。

## 1. 时间账

| 相位 | 内容 | 锚点 |
|---|---|---|
| 起草 | PROPOSAL v0.1 →（human 三轮裁定扩范围）→ v0.2 | 立项 ack 前 |
| 契约预审 | architect-reviewer 四分类 → CONTRACT-CHANGE-PACKET | 33k tokens |
| 评审 R1 | architect 打回 7 项（1B+3M+3m） | 79k tokens |
| human 裁定 | B1 / M2 / m5 三项 | — |
| 评审 R2 | 6 项修复确认、B1 设计稿侧残留 | 69k tokens |
| 评审 R3 | B1 设计稿收口确认 → GREEN | 75k tokens(含 R2 上下文) |
| FROZEN | 立项 ack + 契约门签字 + GREEN + 三裁定 凭据齐 | INDEX→frozen |
| landing | spec(评审循环中已落)+ 契约 total + evidence + CDQ + summary | — |
| 核验 | landing-verifier verdict 绿（5 PASS/0 FAIL） | 35k tokens |

对照：本仓首例，无历史基线。

## 2. Token 账

计量方式诚实声明：本子 agent 实例在通知里带精确 subagent_tokens；主线程（起草/修复/落地）无独立计量，按交互密度估计。**误差界：主线程部分 ±50%**。

| 载体 | tokens | 备注 |
|---|---|---|
| architect-reviewer（契约预审） | 33k | 精确 |
| architect（R1+R2+R3） | 79k+69k+75k ≈ 223k | 精确；R3 续实例含 R2 上下文故偏大 |
| landing-verifier | 35k | 精确 |
| 主线程（起草+三轮修复+landing+retro） | 未计量，估 ~80–120k | 估计 |

花费最大处：architect 三轮评审（约 223k，占子 agent 大头）。可压缩分析：R1 全面审最值（抓住真问题）；R2/R3 本是增量复审，但续实例带全上下文导致 token 偏高——若改用「只传打回项清单 + 增量 diff」的窄上下文复审，可省。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 重写 §4 时声明「其他章节不动」，结果 §5.3/§5.4 旧键位定义与新手势表打架（B1/M2 根源）**：现象 = architect R1 抓出 KEY 长按 §4.0(预留)vs §5.3(回首页)直接冲突、P3 进阅读手势三处三口径。→ 根因 = 我把「§4 细化」当局部改动，没意识到按键手势是跨 §4/§5 的全局语义，重写 §4 必须同步 §5。→ 证据锚 = review-r1-architect.md B1(spec.md:268×439)、M2(spec.md:347×459×设计稿:880)。
- **F2 · 「先落地后补评审」导致设计稿侧收口漏一轮（B1 残留到 R2）**：现象 = R1 我只改了 spec 侧，设计稿真相源的 gesture-table/P3/P4 底部键帽没同步，R2 才暴露。→ 根因 = 修复时聚焦 spec，忘了设计稿也是真相源且已被 human 裁定扩入范围。→ 证据锚 = review-r2「设计稿侧未收口」(kidpal-ui-design-v2.html:620/1002/880)。
- **F3 · spec 承诺一度宽于契约，靠 human 裁定收敛**：现象 = 生字高亮/当前课高亮/果子计数三处，spec 想要的功能契约没有字段。→ 根因 = story 从设计稿反推，设计稿画了但契约没承载。→ 证据锚 = PROPOSAL §1 问题2、§4 OQ-1/2/2′。human 砍掉两个、保留一个(total)，是好的收敛。
- **near-miss（最险一刻）**：契约 `total` 字段差点被我最初用「板子 items.length 推」糊弄过去（不加契约字段）。architect-reviewer 预审时独立指出——**items 一旦分页，items.length 就与全集计数漂移，破坏 S-F2.4 盖章帧「第 N 颗」与计数器的严格同步**。没出事是因为四分类预审强制独立判断「IMPLEMENTATION_ADAPTER 是否成立」。教训：「能本地推就不加字段」的省事直觉，在数据可能分页时会埋不一致的雷。

## 4. 优化建议（按 ROI 排序）

1. **spec 跨章节的「全局语义清单」登记**：按键手势这类跨 §4/§5 的全局语义，应有一处权威定义 + 他处引用，而非多处平行定义。治 F1 · 载体 = 登记不硬治（写进 spec 维护约定，或 ARQ 收件箱作流程提醒）· 预期收益 = 下次改交互语义不再漏同步。
2. **「先落地后补评审」形态下，修复清单须显式含「所有真相源」列**：本批真相源 = spec + 设计稿（+契约），修复时逐列打钩。治 F2 · 载体 = 留批次/PROPOSAL 模板可考虑加「真相源清单」硬项 · 预期收益 = 避免 R2 这种「修了一半」。
3. **「全面审契约」值得固化为 spec 细化的标配动作**：本次逐 view 对 story 核对，主动暴露了 3 个字段缺口（虽 2 个被砍）。治 F3 · 载体 = 登记不硬治 · 预期收益 = spec 承诺与契约承载提前对齐，减少评审期返工。

## 5. 一句总结

成本约 300k+ tokens（评审占大头）/ human 触点 6 次（范围×2 + OQ×3 批 + 立项 + 契约门）/ 制品 6 件（PROPOSAL + PACKET + review×3 + landing-summary + verdict + retro + evidence）· 最险一刻 = total 字段差点被 items.length 糊弄，幸被四分类预审独立拦下 · **保险丝判定：两方独立结构（architect 评审独立 + landing-verifier 核验独立 + architect-reviewer 契约预审独立）接住了作者偏差**——R1 抓跨章冲突、预审拦下数据一致性雷、verifier 抓出 S-F1.2 叙述句残留瑕疵，三处都是主线程自己没看出的 · 最值得带走的一条：**重写一个章节时，先找出它承载的「全局语义」会泄漏到哪些其他章节，一并纳入改动范围**。

## 6. 本环遗留

- OpenClaw 定时能力（契约待确认点 3，S-F1.4 依赖）→ 留 OpenClaw 对齐。
- P3 阅读态语音问生字的实测可行性 → 留 M4/M5 联调。
- story_list.total 的云端实算 + 固件读取 → 留 dev-loop 实现。
- F3.2 组词数据源（makemeahanzi 是否含 words）→ 留 OpenClaw 核对。
- m5 观察项（S-F1.2 叙述句残留）→ 已在 retro 前顺手清除。
- 本仓 Track B 首例跑通，验证流程可用；结构门脚本/机械门未迁的降级路径（人读 diff）已如实记录。
