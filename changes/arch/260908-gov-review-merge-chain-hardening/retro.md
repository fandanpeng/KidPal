# retro · 260908-gov-review-merge-chain-hardening（Track A 全环：立项 → 对抗评审 → 拍板 → 冻结 → landing → verifier → 合并）

> 主线程第一人称热记忆 · 2026-09-08 · 本批 = 22 提交同步 main 后承接 schedule-reminder/story-p3 双包收口遗留的 5 条 260908 ARQ（merge 相关 2 条主承载 + 2 条并批 + 1 条顺带）+ 2 条 260907 merge ARQ 销账核销。harness 形态 = 判官/核验官零上下文子 agent 独立形态，moderator 主线程编排。

**【硬项 R1】** verdict 绿后趁热写，本会话内完成，非隔场补记。

**【硬项 R2】** token 无逐相位计量——子 agent 用量在 usage 元数据有实测（architect 111k / scenario-author 100k / challenger R1 136k / defender R1 81k / challenger R2 92k / defender R2 84k / adjudicator 130k / landing-verifier 111k+复核 0.3k），主线程用量无计量，§2 按此诚实声明。

**【硬项 R3】** 摩擦带根因 + 证据锚（见 §3）。

**【硬项 R4】** 建议标载体（见 §4）。

**【硬项 R5 · 保险丝判定】** 见 §5。

## 1. 时间账

无逐相位墙钟计量（会话内连续执行）；按 commit 链锚点：

| 相位 | 内容 | 锚 |
|---|---|---|
| 起草 | PROPOSAL v0.1 → 立项 ack 四裁 → v0.2（OQ1 裁并批升 Track A） | `014adfe` → `fae886a` |
| 评审 | charter 冻结 → challenger R1（13F）→ defender R1 → challenger R2（+5N）→ defender R2 → adjudicator 收口（22/4/0） | charter 至 `cc2bde7` |
| 定稿冻结 | 九项拍板 → 16 处回灌 → v1.0 候选 → human「冻结，landing吧」→ FROZEN | `1c75c30` → `af0ac8e` |
| landing | 8 canonical packet-bound 落地 + V1-V6 验证套件 + V5 队列簿记 | `b89bf33` |
| 核验 | landing-verifier 首判 RED 五前提 → 逐条修复 → 复核终态 GREEN | RED → 修复 commit → GREEN §7 |

## 2. Token 账

计量方式：子 agent 有实测 usage 元数据（括号内实数）；主线程无计量，估计与子 agent 总量同量级（±50%）。

| 载体 | tokens | 备注 |
|---|---|---|
| 主线程（起草+编排+landing+修复） | 未计量（估 ~200k ±50%） | sync/ARQ 盘点/PROPOSAL/OQ 编排/landing/verdict 修复 |
| architect（候选+回灌） | 111k + 89k | 候选稿 + 16 处回灌 |
| scenario-author（charter） | 100k | 10 轴/6 场景/12 canary/9 留空点 |
| challenger ×2 | 136k + 92k | R1 全域 + R2 定向 |
| defender ×2 | 81k + 84k | R1 全答 + R2 定向 |
| adjudicator | 130k | 27 条裁定 + pass-oracle + 制品 |
| landing-verifier ×2 | 111k + 0.3k | 首判 RED + 复核 GREEN |

花费最大处：adjudicator（制品最重）。可压缩分析：challenger R1 136k 含大量全文件亲核——charter 若预锚「未攻面」优先级可省两成。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 首判 RED 全是包装/格式类**：现象 = landing-verifier 首判 RED，五前提里三条是制品名/manifest 块/packet-id 包装件、两条是文本缺陷（掉字/表头），语义保真全过 → 根因 = 主线程把「summary 写对内容」当终点、没把「结构门实跑」当 landing 步骤的一部分（落地线程当时未实跑 landing-fidelity-check）→ 证据锚 = landing-verdict.md §7.3 留档行 + 修复 commit。教训：**结构门实跑是 landing 的第六个验证步，不是 verifier 的专属**。
- **F2 · 冻结稿自带断言笔误会被自己的机械门咬**：现象 = V4 大小写笔误 + T3 锚行或锚 fail-loud + 我修复或锚时注记含「条件」触发 B7 条件锚裁剪把 integrator.md 从锚并集踢掉 → 根因 = 冻结稿的验证集与锚行都是「代码」，同样会漂移，且回灌 sed 修复会二次引入字面陷坑 → 证据锚 = architect-landing-summary §1b 偏差 1 + PROPOSAL:83 修正注记。教训：冻结 ≠ 不会再变红，landing 前把冻结稿的断言全量预跑一遍（本批 V4 是 landing 后才首跑）。
- **F3 · hooks 治理层三连拦 + 误拦一**：现象 = enforce-scope 拦 heredoc 写 lib（按设计工作）、deny-main-push 误拦含 push 字样的制品归档、auto 分类器误拦一次 git add → 根因 = fail-safe 机器层对上下文不可见 → 处置 = 全部改受门 Write/Edit 路径，零绕行。教训：机器层误拦按「改用合规路径」处置而非对抗；真绕行需求升 human。
- **F4 · V6 演练夹具两轮调试**：现象 = pkgB 基点取错（base commit 而非更新后 main）+ base 提交空目录不被 git 跟踪导致 checkout 删目录 → 根因 = 演练场景的 git 语义细节（add/add 需真 add/add、空目录需 .keep 锚）→ 证据锚 = 会话内两轮失败输出。教训：端到端夹具的「分支基点」语义要在第一轮就画时序图。
- **near-miss（最险一刻）**：T6 spawn→import 若按 PROPOSAL 原文（「五要素核验共享函数提取」）静默落地，会把 approvalRefUnlock 解锁核验集从「全闸六条」收窄为五要素且无申报——challenger R2 N5 抓住、defender 用 Step 1b 背书证据反证「fail-open」定性过重、adjudicator 裁选项 2（收窄+背书夹具）收口。**为什么没出事**：对抗评审的双向结构在落地前接住了核验集收窄分叉；教训 = 改法面里「提取范围」类措辞是核验集半径的承重词，评审必须逐字。

## 4. 优化建议（按 ROI 排序）

1. **landing 前置结构门自跑入 checklist**：治 F1/F2 · 载体 = **ARQ 收件箱**（登记为「主线程 landing 步骤应含 landing-fidelity-check 实跑 + 冻结稿断言预跑」，下一治理批成文进 DEV-GOVERNANCE 或 landing 通道规约）· 预期收益 = 消灭首判 RED 的包装类返回（本批 5/5 前提全属此类）。
2. **agent 制品 TEXT-MODE 归档惯例成文**：challenger 两轮写盘被 harness 拦（workspace 边界），moderator 逐字归档 worked fine · 载体 = **登记不硬治**（review-protocol 一句话注记）· 预期收益 = 少一次每轮的拦停-返工循环。
3. **escalation Track A 批启动**：治 P1 病根族 · 载体 = **留批次**（OQ4 已裁「本批 ack 后即启」，待 human 开 `/arch-change`）· 预期收益 = 升级门恢复区分度。
4. **B-8 重指排队行执行**：载体 = **留批次**（队列 :56 行，逐件 human 授权）。
5. **mer 课文级：V4 类断言大小写/字面在 charter 期与附录正文交叉核对** · 载体 = **登记不硬治**（scenario-author 撰写纪律一句话）。

## 4b. human 触点记账

立项 ack 四裁 / 拍板 4+4 问（整包 5 项 + A-3 + A-4+OQ3 + N5）/ 计数合成拍板 + 冻结指令 / OQ4 已裁待启动——**本批 human 触点 4 次**，全部 AskUserQuestion 在册；deputy shadow：无影子（该批仍在等 ack，不在场）。

## 5. 一句总结

成本 ≈ 845k 子 agent tokens（实测）+ 主线程同量级（估）/ 时长 = 单会话内闭环 / 触点 = human 4 次 / 制品数 = 评审链 10 件 + landing 2 件 + verdict + retro = 14 件 · 最险一刻 = T6 核验集收窄分叉被 N5 在落地前接住 · **保险丝判定：两方独立结构接住作者偏差**——判官独立（adjudicator 独立亲核推翻/确认双方自述：A-3 同形裁、N5 Step 1b 背书亲核）+ verifier 独立（首判 RED 五前提全为作者侧盲区：结构门未自跑、T8 夹具缺、掉字、表头），两句证成。最值得带走的一条：**结构门实跑是 landing 的第六验证步，不是 verifier 的专属**。

## 6. 本环遗留

- escalation-cross-system-overfire Track A 批：OQ4 已裁即启，待 human 开 `/arch-change`（留根 §6 首行）。
- B-8 重指排队行（队列 :56）：待重指 4 件，逐件 human 授权。
- 留根 §6 其余行（hook airtight / B-1 挂点 / pod-events 事件型收编 / pass-oracle 第 4 关兜底 / HOST_ACCEPTANCE_SRCS 根治）逐行在册，去向已标。
- 三条流程性留档（verifier §7.3）：T8b 注入先例注记 / V1-④ 探针与 driver 无 import 守卫张力 / 落地线程未实跑结构门——已进 F1 教训与建议 1。
