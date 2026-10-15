# retro · 261015-gov-review-face-loop-unify（Track B 全环：四面 loop 元结构 + 契约去人签门）

> 主线程第一人称热记忆 · 2026-10-15 · harness 形态：typed architect / landing-verifier subagent 承判官 / 核验官（本仓非降级形态）· 本批 = 「架构面 loop 化」多轮讨论（2026-10-14/15）的直接立项，含一次 mid-discussion 方向修正（三档拆法被 human 否，改判档制）与一次范围反转（我先过度收缩「不用建 loop」，被 human 的 graph-engineering 目的纠回）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写，第一人称热记忆，无隔场补记。

**【硬项 R2 · 计量诚实】** 本批未接 token 计量；§2 以轮次与 commit 数为代理指标，时长为会话内估计（±，无精确钟），如实声明。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 带 现象 → 根因 → 证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议标载体。

**【硬项 R5 · 保险丝判定】** 见 §5。

## 1. 时间账（代理指标：轮次 / commits）

| 相位 | 内容 | 产物锚 |
|---|---|---|
| 讨论/起草 | PROPOSAL v0.1（多轮 human 对话收敛后起草，含一次架构重定） | plan 文件 → PROPOSAL |
| 评审 R1 | BLOCK(F1..F7)（BLOCKING ×4） | §7 R1 |
| 修订 | v0.2/v0.3 七项 | §2 各 T 内嵌 R1-Fx 标注 |
| 评审 R2 | BLOCK(F1,F2,F3)（瘦制品漏 landing-summary 等） | §7 R2 |
| 修订 | v0.4 三项 + 传导四处 | §2 R2-Fx 标注 |
| 评审 R3 | **GREEN** | §7 R3 |
| FROZEN | v1.0（preflight 干净） | Status 行 |
| landing | T1–T24，47 件 commit `68ab057`（+2 删 3 新增） | `git show --stat 68ab057` |
| 核验 | verifier R1 FAIL(F1,F2) → 修订 `3e587b7` → R2 **PASS + 6 ADVISORY** | `landing-verdict.md` |
| 收口 | verdict evidence commit + INDEX frozen upsert | 本 commit |

## 2. Token 账

未接计量。代理指标：判官三轮合计约 44 万 subagent tokens（R1≈127k / R2≈158k / R3≈166k——R3 反而最高，因全稿新矛盾扫描），verifier 两轮约 30 万。误差界 ±15%（harness usage 汇总）。教训：**三轮评审的 token 成本与「起草期判据清单的可机判度」负相关**——R1 的四个 BLOCKING 里三个（F1 写防线矛盾 / F3 过闸通道缺失 / F4 用例缺肢）都源于判据与制品形态在起草期没钉死到可机判。

## 3. 摩擦与卡点（现象 → 根因 → 证据锚）

- **F1 · 起草期把「档位拆分」当成了设计本体，被 human 推翻两次**：现象 = 我先按需求面模板造三档、被否后又在「取代 Track B」上收缩过度；根因 = **把既有面的流程形状当成了新面的设计约束**，而没有先问「这个面的风险结构是什么」（架构 = 无测试 oracle、风险不随字数缩放）；证据锚 = 会话实录（human 原话「就算只改一个字，你也得去 review」「Track B 还有 landing verifier 怎么会拿掉呢」）。**带走**：面 loop 设计时先答「该面的 oracle 与风险结构」，再套元结构。
- **F2 · 判官 R1-F1 抓到稿内正面矛盾（「deny 不动」vs T16 删 deny）**：现象 = 五处「写冻结 deny 不动」与 T16 冲突且失实（architect/review-moderator allow 本含契约路径）；根因 = 起草期凭印象写防线表述、未逐句实读 scope-map 的 allow 方向（deny-only 习惯视角漏了 allow 侧）；证据锚 = R1 §7 F1 + scope-map 实测。**带走**：写权表述必须 allow/deny 两侧都实读——「不动」断言尤其高危（承重断言与起草期同纪律，260928 批 B retro 教训的第三实例）。
- **F3 · 普查口径两轮不一致（21 → 34 文件）**：现象 = 初查 grep 漏 docs/showme 9 件 + glossary + dev-loop-OS；根因 = **grep 域手工圈定**（凭记忆圈 `.claude/ AGENTS.md CLAUDE.md docs/`，showme 在 docs/ 下却因 -v 过滤链漏掉）；证据锚 = R1 F2 + 判官复扫。**带走**：census 硬项的 grep 命令应与 V6 验证命令**同一份**（本批已在 PROPOSAL §5 对齐，但基线跑早了——先基线后普查域修正，顺序倒置付了一轮）。
- **F4 · 轻路径缺过闸通道（判官 R1-F3 抓出）**：现象 = 轻路径止于「主线程落 + 记账」，但升级闸 gov 机锚要 FROZEN+GREEN+verdict——轻批永不过闸；根因 = **设计新路径时只画了 happy path 的前半段**，没有走到「它怎么合并」；证据锚 = gate :302-310 实读。**带走**：任何新落地形态的规格必须以「过哪个闸、凭哪三件」收尾，不以「落」收尾。
- **F5 · manifest 外残留 5 句（verifier 升级项）**：现象 = FROZEN sweep 漏 requirement-change 命令/SSOT、architecture-decisions、brief-inputs 注释、AGENTS.template 五处人签门活句；根因 = **普查 pattern 按「命令名 + 四分类词」扫，没扫「人签门」语义词族**（人类契约门 / human contract gate / 签字制）；证据锚 = verdict ADVISORY-1。**带走**：语义级 sweep 要按「旧机制的措辞词族」建 pattern（契约门 / 人工门 / 人类签字 / human gate），不按命令名。

## 4. 建议与载体

- **「面 loop 设计时先答 oracle 与风险结构」方法论** → 固化候选：登记 ARQ 收件箱（gov 域，附本 retro 链接），下次建新面 loop 时消费。
- **「落地形态规格以过闸凭据收尾」** → 固化候选：并入 PROPOSAL.template 的硬项提示句（下个动模板的治理批并批；ARQ-260914-landing-preflight-anchor-parse 同族）。
- **语义级 sweep 词族 pattern** → 登记不硬治：写入本 retro（本批已按词族补扫 —— 五处残留转呈 human 裁，见 §5）。
- **manifest 外 5 处人签门残留句** → 待 human 裁（扩 FROZEN 修订 vs 首个 /gov-change 轻路径批收拾；主线程推荐后者）——已入 landing-verdict ADVISORY-1 与 summary §3.6，本批不触碰。
- **留根 6 条照 PROPOSAL §6 执行**（需求面形态差 / reviewer 大一统 / 变更记录节机械齿 / 凭据闸补强 / CDQ 存废 / commandEntry 复核）。

## 5. 保险丝判定

两方独立结构**接住了作者偏差**：判官三轮抓出 10 项（含一处正面矛盾、一处过闸通道缺失——都是起草者自信最满的两处）；核验官抓出 4 项（含 FROZEN sweep 缺口的 4+5 处残留句——作者普查自认完备后被证伪）。无一项由作者自证通过。**本批流程保险丝有效**；唯一漏网面（manifest 外 5 句）由核验官按 sweep 词族方法抓出而非起草者自查——「语义级 sweep 要按词族」已固化为 F5 带走项。
