# retro · 261017-arch-review-p3-story-body-sd-cache（Track B 全环：R1 BLOCK → R2 GREEN → landing → verifier PASS）

> 主线程第一人称热记忆 · 2026-10-17 · 本批为跨面变更 `261016-p3-story-body-sd-cache` 的**架构 + 契约面**（需求面已先行 frozen v1.64；实现面分档未起）· 判官 / 核验官 = 子 agent 形态（零持久上下文，各自独立派发）· 同日在同一变更内连踩三次「主线程误读路由语义」，本条为第一人称记录。

## 1. 时间账

| 相位 | 内容 | 备注 |
|---|---|---|
| 起草 | architect v1.0（217 行）→ 修订后 322 行 → FROZEN 版 364 行 | 三轮（首次落盘 + R1 修订 + FROZEN 前 token 收口） |
| 评审 R1 | **BLOCK(F1, F2)** | F1 = §12 changeId 行未列；F2 = 契约包三节未按 SSOT 字段形态在场 |
| 评审 R2 | **GREEN**（靶向复核） | 判档主路径 / 四分类 ACCOMPANIES_SPEC_CHANGE |
| FROZEN | human 立项 ack（原话「立项」）+ R2 GREEN | 主线程凭据自冻 |
| landing | T1–T6 + 契约包 + CDQ | commit `b3f50ebd`，4 canonical + 3 制品 |
| 核验 | landing-verifier **PASS** | 结构门完整模式 exit 0；T1–T6 逐字核对全中；超范围改动 0 |
| 合并 | **闸 BLOCK（假阴性）→ human 授权手工本地合并** | 见 §3 F3 |

**时间账诚实声明**：本会话未逐相位计时（无 harness 计时器可读），故不给时长列——**不装精确**。

## 2. Token 账

**计量方式**：取各 subagent task-notification 报告的 `subagent_tokens`（累计值），主线程自身无对应计量接口。**误差界：每个 subagent 的值为其累计口径、跨轮续跑会叠加，故下表按「agent 累计」而非「相位」读；主线程未计入**。

| 载体 | tokens（累计，报告值） |
|---|---|
| story-author（需求面两轮，前置相位） | 159,450 + 132,024 |
| architect（可行性三件，前置相位） | 117,228 |
| architect（本面起草 + 两轮修订 + FROZEN 收口） | 131,336 → 247,849（累计，四次续跑） |
| architect-reviewer（R1 + R2） | 158,063 → 192,880（累计） |
| landing-verifier | 83,168 |
| uiux-author（前置相位，两轮） | 159,895 → 228,928（累计） |

**花费最大处**：architect 本面（约 248k 累计，四轮续跑）与 uiux-author（约 229k 累计，含一次「增量 v6 → 完整母版重建」的返工）。**可压缩分析**：architect 的四轮里，R1 那轮（+105 行）与 FROZEN 前 token 收口（+42 行）本可在首稿一次做对——两处都是**本可自查的 SSOT 义务**（契约包三节在 `contract-change-evidence.md:7` 明写；packet-id 范围 token 是自洽性）。首次即足可省约一轮。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 主线程对本仓路由语义连续误读三次（最痛，且全部由 human 当场纠正）**：① 把 `/triage-change` 读成「只是实现面分流门」（实为跨面编排入口，容器）→ 答「不是 triage 先行」；② 更正后又称 triage 为「③ 实现面」，仍把容器当链上一环；③ 给 architect 的 dispatch 写「不写任何文件」，而 `architect.md:29/:66` 明写 governance PROPOSAL 入 `delivery/review/**`、`:28` 明写「主线程不亲自执笔」——**该指令的实际后果是逼我违反 cast 纪律亲自代写 PROPOSAL**。根因 = **先入框架未被后续阅读覆盖、只被追加**：AGENTS.md 摘要行「实现面变更 → /triage-change 五档分流」是常驻指令、每次会话重新加载，而权威定义在 `.claude/commands/triage-change.md` 与 `change-triage.md`（非常驻）——读一次压不住灌很多次。证据锚：`delivery/intake/261016-p3-story-body-sd-cache.md` 的「起草期更正留痕」节；`docs/debt/gov/_archive/2026/ARQ-261016-intake-entry-identity-unstated.md`（该缺陷已由 `261016-gov-review-triage-entry-identity` 批修掉 canonical，本条记录的是**修好之后我仍误读一次**）。

- **F2 · `spec-anchor-check.mjs` 的「最高版号即完整母版」静默打断全仓 UI 锚**：现象 = `uiux-author` 按「只加一个新态」写了一份增量 `kidpal-ui-design-v6.html`（3 个 `data-od-id`，v5 为 60），该文件一落盘即被 `currentDesignFile()` 选为现行稿，全仓 **12 条 UI 锚**判悬空、涉 10 条 story。根因 = `currentDesignFile()` 只按版本号最大选文件、**无完整性校验**（`spec-anchor-check.mjs:35-41`）；同时间接根因 = 该批 Write 撞单次体积上限（约 14KB 成功 / 77KB 失败 vs 完整母版约 107KB），「增量发布」是大文件受限时的自然产物。证据锚 = 本次 before/after 实测（增量件 → 12 条 `ok:false`；重建为完整母版 61 个 id → `ok:true`）+ README 归因对照（用 HEAD 版 spec 重跑得同一错误集，证明与本批编辑无关）。**已登记** `ARQ-261017-anchor-check-master-unvalidated`。

- **F3 · 合并闸的契约凭据取件 ref 用错，判据结构性不可满足**：现象 = 三件凭据（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖）齐备，闸仍 BLOCK 并报「无候选满足三件联立，已检查 = `{frozen:false, verdict:false, covered:false}`」。根因 = `pre-merge-escalation-gate.mjs:143` 用 `git show ${target}:${proposalPath}`，而 `target` 在 :431 硬编码为 `'main'`——**候选 PROPOSAL 之所以成为候选正因它出现在 `source..main` diff 里，故按定义不在 main 上**，取件恒失败 → 三件连锁塌为 false。该判据由 261015 批 T15 新增（替换原硬编码 `false`），**本批是其首次真实执行，首跑即命中**。证据锚 = 闸逐字 BLOCK 输出 + `git show main:…` 失败 / `git show HEAD:…` 成功两条复现命令。**已登记** `ARQ-261017-merge-gate-contract-evidence-ref`。

- **F4 · `landing-fidelity-check` 的 `verbatimQuoteAdvisory` 假阳性**：现象 = 报告 5 条「逐字未命中」，而 landing-verifier 剥掉 PROPOSAL 的 `【…】` 头行与 `原：/新：` 标记后逐段比对，T1–T6 **全部逐字命中**。根因 = 该 advisor 取整块 fenced block 原文匹配（`landing-fidelity-check.mjs:463-477`），而本 PROPOSAL 的围栏块按 SSOT 体例本就含头行与替换标记——**这些本不该落进 canonical**。证据锚 = landing-verdict A1。**未登记**（见 §4 建议 3）。

- **near-miss（最险一刻）**：**我差点亲自代写 PROPOSAL**。我在 dispatch 里写「不写任何文件」，architect 老实照做并只在 hand-back 里返回全文；如果我没去核 `architect.md` 就顺手把那份 24.9KB 搬进 `delivery/review/**`，就会直接构成「主线程亲自执笔变更内容」——违反 261015 批统一 cast（`architect.md:28` 逐字「主线程不再亲自执笔」），而且是**没有任何机闸会拦的那种违规**（写面本身对我开放）。**没出事的原因 = 我在落盘前多问了一句「PROPOSAL 该谁写」**；不是结构接住的。

## 4. 优化建议（按 ROI 排序）

1. **修合并闸取件 ref（source 优先）+ 加归因自检**：治 F3 · 载体 = **ARQ 收件箱（已登记 `ARQ-261017-merge-gate-contract-evidence-ref`）**，与 `ARQ-261017-merge-gate-scope-unstated` 同批评审可合并处置路径 · 预期收益 = 消掉 arch-lane 契约批的恒 BLOCK，且把「缺凭据」这类误导性归因改成「取件 ref 疑似用错」。

2. **给 `spec-anchor-check` 加母版完整性校验 + 命名纪律**：治 F2 · 载体 = **ARQ 收件箱（已登记 `ARQ-261017-anchor-check-master-unvalidated`）** · 预期收益 = 把「写增量稿的人不知道自己打断了全仓」变成显式报错。

3. **给 `landing-fidelity-check` 的 verbatim advisor 加剥头 / 分侧解析**：治 F4 · 载体 = **建议登记 ARQ（本次未登记——假阳性不阻断，危害等级低于 F2/F3，可并入 F3 那条的同批治理）** · 预期收益 = 消掉每次落地都要人肉剥头复核的固定成本。

4. **主线程「路由语义」类判断先读权威文件、不采信常驻摘要行**：治 F1 · 载体 = **登记不硬治（行为纪律）**——本仓摘要行与权威文件已两次出现「摘要落后于权威」的形态（`ARQ-261016-intake-entry-identity-unstated` 治的正是这个），但**给我自己一条硬规则：凡回答「某入口/某角色干什么」，先打开它的定义文件与流程 SSOT，再答**。预期收益 = 今天三次误读里有三次都由 human 兜底，成本全落在 human 身上。

## 5. 一句总结

成本 = 本面 4 个 agent 相位（architect ×4 次续跑、reviewer ×2、verifier ×1）+ 制品 4 件；触点 = human **4 次**（需求面终拍看图、架构面立项 ack、合并假阴性授权、以及**三次路由误读纠正**）· 最险一刻 = 差点亲自代写 PROPOSAL（cast 违规无闸可拦）· **保险丝判定：两方独立结构（判官独立 + verifier 独立）**——**接住了作者（architect）的偏差**（判官 R1 抓出 F1/F2 两条 SSOT 义务缺口、verifier 独立剥头复核证伪了工具的假阳性），**但没有接住编排者（主线程）的判断偏差**：今天三次路由误读**全部由 human 当场纠正**，判官与 verifier 一次也没看见（它们拿到的是我已经写歪的 packet）。· 最值得带走的一条 = **「容器 vs 链上环节」这类结构性判断，读摘要不算读过——必须打开定义文件**；这条今天付了三次学费。

## 6. 本环遗留

- **实现面五档分档未起**：`/triage-change` 的收尾动作，前置 = spine `anchors.arch` / `anchors.contract` 回填（本批合并后回填）。
- **在册 ARQ 两条**（同脚本同批可评）：`ARQ-261017-merge-gate-scope-unstated`（lane 白名单）、`ARQ-261017-merge-gate-contract-evidence-ref`（本批登记，取件 ref）。
- **`landing-fidelity` verbatim advisor 假阳性**：未登记，建议并入上条同批。
- **跨面变更整体未结**：需求面 frozen / 架构+契约面 landing 已核，**实现面未起**；spine `status` 维持 `in-flight`。
