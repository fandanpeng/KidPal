# retro · 261014-gov-review-crq-debt-rename（Track B 全环：账本目录 crq → debt 改名批）

> 主线程第一人称热记忆 · 2026-10-14 · 本环 = human 口头发起的治理卫生批（「crq 这个名字变扭，其实就是 debt」）· harness 形态：判官 = typed architect agent（同实例 resume 三轮）、核验官 = typed landing-verifier agent，均零降级。跨乘客对照：与 260914-gov-review-delivery-restructure 同型（全仓路径 token 批改），本环是该先例教训的正面复用。

**【硬项 R1】** verdict 绿后即写，本会话热记忆，无隔场补记。

**【硬项 R2 · 计量诚实】** 判官 / 核验官 token 有 harness 计量（见 §2 表）；主线程 token 无逐相位计量，估计方式 = 本会话 1M 上下文窗口占用的粗估（起草 + 落地 + 收口全程），误差界 ±30%。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 讨论 + 起草 | human 三裁（不拆域 / 不搬 delivery / 只改名）+ census 八形态扫描 + PROPOSAL v0.1 | 会话前段 | 2026-10-14 | ~25 min（估） | 主线程估 ~120k ±30% |
| 评审 R1 | 判官打回（2 BLOCK、2 QUESTION） | 2026-10-14 | 同日 | ~11 min | 99,055 |
| 修订 v0.2 | BLOCK 正形修订 + preflight | 2026-10-14 | 同日 | ~5 min | 主线程内含 |
| 评审 R2 | 判官打回（1 BLOCK、1 QUESTION，closedBy 恒红自抓） | 2026-10-14 | 同日 | ~6 min | 139,409 |
| 修订 v0.3 | bare-CRQ 缩圈 + 引注补正 + 杂散反引号 | 2026-10-14 | 同日 | ~5 min | 主线程内含 |
| 评审 R3 | GREEN（窄复核） | 2026-10-14 | 同日 | ~1.5 min | 147,314 |
| FROZEN | 主线程凭据自冻（v0.3） | 2026-10-14 | 同日 | <1 min | — |
| landing | scope-map 先改 → git mv 146 件 → 34 锚跟改 → render 重渲 → V1–V5 全绿 | 2026-10-14 | 同日 | ~35 min（估） | 主线程估 ~150k ±30% |
| 核验 | landing-verifier PASS（3 ADVISORY） | 2026-10-14 | 同日 | ~7.6 min | 98,365 |
| **合计** | R1→PASS 当日闭环，human 触点 2 次（立项 ack + merge 暂缓指示） | | | ~95 min（估） | 判官 + 核验官 484,143（计量值） |

对照：260914 同型批（delivery 重组）为四轮评审 + landing 四轮修复；本环三轮评审 + landing 零修复轮，路径批改先例的形态族谱 / 裁腿纪律直接省了落地返工。

## 2. Token 账

计量方式：判官 R1/R2/R3 与核验官为 harness 精确计量；主线程无逐相位计量，按会话窗口占用粗估并标 ±30%（硬项 R2 声明）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 落地 + 收口） | ~270k（估 ±30%） | ~36%（估） | 34 锚逐文件 Edit 占大头 |
| 判官（R1 99,055 + R2 139,409 + R3 147,314） | 385,778（计量） | ~51% | R2/R3 含 resume 全上下文重载——resume 计费形态下窄复核轮不省 token |
| 核验官 | 98,365（计量） | ~13% | 六项全独立重推导 |

花费最大处：判官 resume 轮（R2/R3 合计 286k > R1 全量评审）。可压缩分析：Track B 窄复核若改派零上下文新实例 + 只喂「修订段 + R2 入口」，token 或可省一半——但同判官 resume 保上下文连续性的价值是熔断条款明文（R1–R3 = 同判官），留根不动。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · bare-CRQ 判据被 closedBy 历史记名恒红（R2 唯一 BLOCK）**：现象 = V1 第八条 `grep -rn "CRQ"` 活面口径下判据不可通过 → 根因 = 已结条目 frontmatter `closedBy` 的 append-only 历史记名（「2026-10-12 tech 域 CRQ 盘点」）经重渲原样带入归档年 INDEX，R1 起草时未预见「生成件会携带 frontmatter 历史记名」这一传播路径 → 证据锚 = `docs/debt/tech/_archive/2026/INDEX.md:19-26`（7 处，主线程落地前亲核复核）。教训：**判据设计要追「文本从哪来」到源，生成件的 provenance 链比手写件多一层**。
- **F2 · 旧名引用面比「路径字面量」宽一层（R1 最大 BLOCK）**：现象 = T1③ 只覆盖 `docs/crq` 字面量，而旧脚本名 `crq-render` 等以自指串（GENERATED_MARKER / indexHeader / self-check 输出）/ 指令面命令行 / bare 文件名三形态散布 → 根因 = 起草时把「改名」窄理解为「路径批改」，漏了「文件名本身是 token」→ 证据锚 = 判官 R1 记录 25 件命中枚举（PROPOSAL §7）。教训：**改名批的两个 token 族（目录路径 + 文件名 stem）要分别 census**——这正是 path-form-family 八形态族谱未覆盖的盲区（族谱管路径文本形态，不管「文件名 stem 被引用」），本批以「第二 token 族」论证收编、族谱未 append（判官 R2 采信）。
- **F3 · 渲染器防冲掉闸拦重渲**：现象 = debt-render 拒写 6 份旧 INDEX（缺新生成标记头）→ 根因 = GENERATED_MARKER 文本变了，盘上旧生成件「不再是生成件」→ 处置 = rm 后重渲（生成件纯函数再生，doctor 136 守恒佐证）→ 证据锚 = landing FATAL 输出（architect-landing-summary §3 第一条）。教训：**标记头里含脚本名的设计让「脚本改名」与「生成件再生」耦合**——未来若再改脚本名，先删生成件再渲是正道，不必绕闸。
- **near-miss（最险一刻）**：落地期 V1 裸段形当场抓出 repo-structure 语义说明里「时名 `crq/`」——主线程自己写的沿革注带斜杠，撞上自家判据。为什么没出事 = V1 在 commit 前实跑而非纸面通过；教训 = **凡自己写的新文本也要过自己的清扫判据**（写「时名 `crq`」无斜杠即免）。另 R3 判官 nit：V1 附注引文写 `grep -cn`（-c 下 -n 无效）——引文形态与实跑形态不一致的小漂移，数字经复算为真，记档未阻塞。

## 4. 优化建议（按 ROI 排序）

1. **改名批起草清单补「第二 token 族」检查项**：凡重命名文件 / 脚本 / 模板的批次，census 必须同时枚举「旧路径」与「旧文件名 stem」两个 token 族的引用面（本批 F2 的通用化）· 载体 = **ARQ 收件箱（gov-arq 快路径登记，本批顺手登记）** · 预期收益：下次改名批省一轮评审打回。
2. **生成件 provenance 传播检查**：设计「grep 判据」时对生成件先跑一次「其数据源里有没有该 token」再定活面口径（本批 F1 的通用化）· 载体 = **登记不硬治**（判据设计经验，写进本 retro 即可复用；改 path-form-family 或 PROPOSAL 模板属扩面，无执行证据不动）· 预期收益：避免 R2 型恒红判据。
3. **resume 计费形态下的窄复核策略留根**：R2/R3 resume 全上下文重载使「窄复核」实际 token 高于全量评审 · 载体 = **留批次观察**（若后续 Track B 批普遍复现，再议判官实例策略；本次不改——熔断条款明文同判官 resume）· 预期收益：Token 账真实性数据积累。
4. **landing summary 模板补「human 指示快照」占位**：本环 merge 暂缓是 human 中途指示，summary 诚实注记已承载；若「landing 完成但合并悬置」成为常态，INDEX 或需显式 `heldFromMerge` 状态 · 载体 = **留批次** · 预期收益：合并悬置期的状态可读性。

## 5. 一句总结

当日闭环 ~95 min（估）/ 判官 + 核验官 484k tokens（计量）/ human 触点 2 次 / 制品 4 件齐 + 2 commit（landing 05e73eb + 收口）；最险一刻 = 自写沿革注撞自家裸段形判据、V1 commit 前实跑接住；**保险丝判定：两方独立结构接住作者偏差**——判官 R1/R2 两轮共抓 3 BLOCK（其中 closedBy 恒红为主线程与判官同责的 R2 干跑自抓、判官如实记档），verifier 六项独立重推导零偏差，作者（主线程）起草期的两处覆盖缺口（旧名引用面、closedBy 传播）全部被独立方接住；最值得带走的一条 = **改名批的 census 是两个 token 族，判据要追文本来源到生成链源头**。

## 6. 本环遗留

- **merge 悬置**（human 2026-10-14 指示 main 侧在途改动，本批停合并门前）——放行条件 = human 通知；届时跑 `pre-merge-escalation-gate.mjs` PASS 后 ff-only 合并本地 main（INDEX `mergedInMain` 由合并回同步时 `--upsert` 现推）。
- **ARQ 收件箱登记**：§4 建议 1（改名批两 token 族 census）按 gov-arq 快路径登记新条目——主线程后续动作，不阻塞本批。
- **verifier ADVISORY 3 条**：INDEX 行 landed 现推（已按 ADVISORY-1 重跑 upsert 完成）；V1 窄口径勿误读为全仓零字面（豁免面在 FROZEN §3）；debt-ledger-shared.mjs 末行无换行符沿袭旧件（nit，不动）。
- **用户级 auto-memory**：主 AI 记忆文件里的 `docs/crq` 路径引用，本环收口时自更（仓外面，不入 manifest）。
