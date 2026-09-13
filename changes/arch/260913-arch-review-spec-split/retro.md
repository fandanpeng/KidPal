# retro · 260913-arch-review-spec-split（Track B 全环：brainstorm → R1–R6 → 双 GREEN → re-FREEZE v1.1 → landing → 核验）

> 主线程第一人称热记忆 · 2026-09-13 · 乘客 #5+（Track B 常驻化后治理批）· harness 形态：判官 = 独立 architect 子 agent 六轮（R5/R6 为 compaction 后新实例）；核验官 = landing-verifier 子 agent。· 跨乘客对照：首个「spec 面拓扑重构」型变更（前几乘客多为闸/流程件）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | brainstorm（2026-09-12 晚起）+ PROPOSAL v0.1 | 09-12 | 09-13 午 | ~半天（跨会话） | 未计量 |
| 评审 R1–R4 | R1 打回 7 项（F1–F7）→ R2 打回 4 项（F8–F11）→ R3 打回 3 项（F12–F14）→ R4 GREEN（附 F15 建议级） | 09-13 午 | 09-13 午 | ~2h | 未计量 |
| OQ + 可行性 | OQ-1~4 裁定；OQ-2 逐条过目（升降砍九项定案） | 09-13 午 | 09-13 午 | ~0.5h | — |
| 重开 delta | OQ-5=(b)（story 五件重写 + skill 折入）→ v0.5 修订 → R5 打回 2 项（F16/F17）→ R6 GREEN → re-FREEZE v1.1 | 09-13 午后 | 09-13 午后 | ~1h | 未计量 |
| landing | 18 行 manifest 落地 + 锚检脚本开发 + 验证集 10 条 | 09-13 晚 | 09-13 晚 | ~1h | 未计量 |
| 核验 | landing-verifier verdict | 09-13 晚 | | | |
| **合计** | | | | ~1 天（净作业 ~5h） | |

对照：比 260908-merge-chain-hardening 类纯闸件变更重——内容迁移型变更多了「重写 + human 逐条过目」两个相位。

## 2. Token 账

计量方式诚实声明：无 token 计量（主线程与子 agent 均无计数埋点），以下为结构估计（±50%）：主线程上下文约 15M tokens 窗口内跑了全程（含一次 compaction）；判官 R1–R6 各为独立子 agent（R1–R4 同实例、R5–R6 新实例）。花费最大处 = 主线程的 spec 全文读取（644 行）+ PROPOSAL 多轮修订 + landing 期两大文件撰写。可压缩分析：判官亲核清单逐轮增长（R3 已达 10 项亲核），对内容迁移型变更这笔开销是必要承重墙，不建议压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · story/NFR 计数失真（R5-F16）**：现象 = 我在验证集写下「15 条 story」「6 域 6 条」两个错误常量，判官实测纠正为「14 全集 − 1 砍 = 13 活跃」「5 域 7 条」→ 根因 = 凭记忆写数而非当场 `grep -oE 'S-F[0-9]+\.[0-9]+' | sort -u` 实跑 → 证据锚 = PROPOSAL §7 R5 亲核清单①。**同类先例 = R1-F4（源清点不实），同根 = 起草方不先实跑就落数**。
- **F2 · census 两趟才扫全（R2-F9 → R3-F12）**：现象 = 首趟 census 漏扫 `.delivery/` 面（SSOT 字段 ×7 + 权威指针），判官 R3 补扫抓出 → 根因 = 执行口径问题（首趟命令没覆盖该目录），且我当时误归因为「rg 尊重 gitignore」——判官 F15-a 实测证伪（rg 正常命中、无 ignore）→ 证据锚 = PROPOSAL §7 R3-F12、R4-F15-a。教训：**漏扫先查执行口径，别赖工具**。
- **F3 · 锚检脚本落地期两处解析缺陷**：现象 = 首跑「全绿」实为真空绿——`支撑 = ` 带空格形态不匹配正则，story 支撑锚全部漏检；`## 3.` 顶层小节标题不解析 → 根因 = 写完脚本只用真实数据跑了一遍，没用「故意悬空」反例证伪 → 证据锚 = 本目录 architect-landing-summary.md §1 第 14 行注记；修复后反向视图才显出 story 行。教训：机检脚本自身的「假绿」风险与被检对象同级，**先跑一个必败用例再信绿**。
- **F4 · git mv 目标目录不存在**：现象 = `git mv docs/spec.md docs/archive/spec.md` 报 No such file or directory——repo-structure 早已登记 `archive/` 子目录，但目录实体从未建过 → 根因 = 结构登记表与文件系统实况漂移（登记 ≠ 存在）→ 证据锚 = landing 会话实录。影响零（mkdir 即解），但提示 repo-structure 的「已登记」不等于「已存在」。
- **near-miss（最险一刻）**：landing 前盘点时发现 S-F3.3 的 spec 原锚 `ic-penhand` 在 v3 设计稿根本不存在——若 verbatim 搬迁，新文件第一天就带一个死锚，且锚检脚本（若按原样写）会立刻翻红报警、把「设计稿缺 id」与「迁移错误」两种失败混在一起。没出事是因为 human 的 S-F3.3 收窄裁定顺带移除了该锚的语义依赖，且 v3 全量 61 id 在评审期已被判官双文件比对过。教训：**锚迁移前必须先对现行设计稿实测锚存在性**（已固化为 spec-anchor-check ② 检与 T1 规约第 5 条）。

## 4. 优化建议（按 ROI 排序）

1. **「凡写数量，先实跑枚举」写进 PROPOSAL 起草自检**：治 F1 · 载体 = 固化候选 → ARQ 收件箱（PROPOSAL.template 或 review-moderator 硬项加一条「计数类断言须附枚举命令」）· 预期收益：R5-F16/R1-F4 这类同源打回直接消失。
2. **机检脚本交付前必跑一个构造的失败用例**：治 F3 · 载体 = 固化候选 → ARQ 收件箱（可进 write-story 同族——「脚本红比脚本绿先可信」）；spec-anchor-check 本身后续可补 `--self-check`（留批次，本次 YAGNI）· 预期收益：真空绿在交付前自爆。
3. **census 执行口径留痕**：治 F2 · 载体 = 登记不硬治（本 PROPOSAL §3 已立「原始输出进证据段」先例，后续批次沿用即可）· 预期收益：漏网面在评审期可见，不再靠判官补扫。
4. **repo-structure「登记≠存在」注记**：治 F4 · 载体 = 登记不硬治（一句话注记即可，不值一个 ARQ）· 预期收益：下次 landing 少一次报错往返。

## 5. 一句总结

成本 ~1 天 / human 触点 4 次（立项 ack、OQ-1~4、可行性逐条过目、OQ-5 + commit/landing 授权）/ 制品 6 件（PROPOSAL + 可行性终标 + story 重写稿 + skill 草案 + landing-summary + verdict+retro）· 最险一刻 = `ic-penhand` 死锚差点随迁移进新文件 · **保险丝判定：两方独立结构接住作者偏差**——判官六轮抓出计数失真、census 漏扫、状态自述陈旧三类作者偏差共 17 项（R1:7 / R2:4 / R3:3 / R5:2 + R4 建议级 1）；verifier 独立性见 landing-verdict · 最值得带走的一条 = **内容迁移型变更的错误不在设计而在清点，清点必须实跑留痕**。

## 6. 本环遗留

- CDQ-260913-spec-split-fallout（canary 翻红 + 契约 :4 断链）→ phase 2 验收基线重建处置。
- S-F3.1/3.2 🟡、NFR-PERF-2 🟡、NFR-READ-1 🟡 → phase 2 立项输入。
- requirements.md §7 开放问题 4 条（语音查下一个字 / 温湿度云端记录 / 家长管控形态 / 线框待补）。
- 设计稿 v2/v3 内 spec 引用各 1 处 → v4 迭代顺带改。
- spec-anchor-check 拦阻强度升级（挂 hook）→ 跑出误报率后再议（OQ-4）。
- show-me 图（show-me-spec-split.html）仍是 v1.0 形态标注（manifest 17 行/明日 landing）——与 v1.1 实况（18 行、当日 landing）有漂移，如需分发应更新（未更新，如实登记）。
- 核验期补发（2026-09-13，retro 落盘后）：① landing-summary 缺 ```landing-manifest 围栏块 + packet-id 字面（B2/A9 格式项，verifier 首轮 RED 唯一拦阻）——教训同 F3 族：制品格式契约要靠机检跑一遍再交付，不靠人读自查；② 结构门第二层暴露两盲点：PROPOSAL §2 锚行加粗形态不解析（已格式归一，PROPOSAL「落地后不可回改」例外待 human 追认）+ ANCHORABLE_TEST 不识 `.delivery/**`（ARQ-260913-lfc-anchorable-delivery-gap，本批闭环待 human 例外拍板）。
