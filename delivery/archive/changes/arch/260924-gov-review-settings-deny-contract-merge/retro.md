# retro · 260924-gov-review-settings-deny-contract-merge（Track B 全环：起草实测 → 立项 ack+OQ 三裁 → R1–R3 三轮收敛 GREEN → FROZEN v0.4 → 零脚手架 landing → 核验 FAIL(F1)→回贴→PASS → commit）

> 主线程第一人称热记忆 · 2026-09-24 · 乘客序号 = N/A（Track B 乘客序号在仓内无权威注册面，INDEX 不含此列，不虚构）· harness 形态：判官 = typed `architect` agent、核验官 = typed `landing-verifier` agent（本仓有 typed agent，简报头注的「Kimi 零上下文 coder 降级」不适用）· 跨乘客对照：锚文/插入文本逐字性问题第 3 次现身（批 A R1-F1/F2 锚文失逐字 → 本批 R1-F1 转义伪影 + 核验 F1 插入段单字符），「申报与实况不符」第 2 次（批 A R1-F4 → 本批 R1-F3 同型 + 核验 A1 新形态）。

**【硬项 R1 · 环后趁热、第一人称】** verdict PASS（21:24:58）后即写、commit（21:27:14）后合并前完成，第一人称热记忆，无隔场补记。

## 1. 时间账

硬数据锚点表（时长 = harness 报告的 duration_ms；时刻锚 = 制品 mtime 与 smoke 日志时间戳）：

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草期实测 + 起草 | 四项只读实测（冒烟三形态/病灶复证/复扫/SKILL 面）+ ARQ 追注 ×2 + PROPOSAL v0.1 + preflight 绿 + INDEX 钉行 | ~18:30（smoke 日志 18:59/19:00/19:07 为锚） | ~19:50 | ~80 min | 主线程（见 §2） |
| 立项 ack + OQ | human 同场四问（立项 + OQ-1/2/3 全裁 (a)）→ v0.2 | 会话内 | 会话内 | 分钟级 | — |
| 评审 R1 | 判官全面审 → **BLOCK(F1–F6 + N1–N5)** | — | — | 37 min（2,205,459ms） | 169,365 |
| v0.3 修订 + R2 | F1–F6+N1–N3 修订 → 判官复核 → **BLOCK(R2-F1)** | — | — | 10.6 min（633,403ms） | 219,523 |
| v0.4 修订 + R3 | R2-F1 单点修订 → 判官复核 → **GREEN**（三轮收敛，未触熔断） | — | PROPOSAL mtime 20:43 | 5.8 min（345,795ms） | 235,610 |
| FROZEN + landing | 标 FROZEN v0.4 → T1（settings×2）→ T2（settings-lint 新建 + SKILL 接线）→ T3（gate/merge-driver/pod-coordination §10e）→ V1/V2/V3/V6 全绿 → T4（账本结项+render）→ summary | 20:43 | summary mtime 21:20 | ~37 min | 主线程 |
| 核验 第1轮 | verifier 六项 → **FAIL(F1)**（T2-ii 插入段漏 1 字符「两」）+ 3 ADVISORY | ~21:08 | — | 12.8 min（770,527ms） | 121,469 |
| F1 修复 + §3 修订 | 机械回贴 + byte 复证（490==490）+ summary §3 重写（A1 纠正/A2 处置/A3 披露/§3.2 F1 入账） | 21:20 | ~21:22 | ~5 min | 主线程 |
| 核验 第2轮 | 同 verifier resume → **PASS**（F1 FIX-CONFIRMED、ADVISORY 3→0、回归全绿） | — | verdict mtime 21:24:58 | 4.7 min（282,758ms） | 142,318 |
| commit + commit 后项 | staged 核验（17 files 735+/44−）→ commit 11fb5c1 → V5 upsert（landed）→ V4 幂等子句 commit 后复跑 exit 0 | — | 21:27:14 | ~3 min | 主线程 |
| **合计** | 全环 | ~18:30 | 21:27（retro 写完约 21:4x，先于合并） | **~3h** | |

对照：批 A（260924-gov-review-planner-orchestration-align，同日更早）为 R1 BLOCK→R2 GREEN 两轮 + 脚手架形态；本批三轮评审 + 核验两轮，总时长相当，但本批为**零脚手架批**（G3 面在批准前不可写入），landing 全部发生在 FROZEN 之后——核验轮暴露的单字符偏差正是零脚手架形态下「FROZEN §2 引文 = 唯一比对基准」的代价面（见 §3 F1）。

## 2. Token 账

计量方式诚实声明：判官/核验官 tokens = harness 报告值（**resume 会话的报告值可能含既往轮上下文，逐轮净增不可精确拆分**——R2 报告值 219,523 大于其单点复核的合理净耗，按报告值如实记、不装作精确）；主线程 = 会话 total_tokens 余量差（15,000,000 → 14,678,601）≈ **321k**，口径含系统提示与全部工具回执，误差界 ±10%。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草实测 + 修订 + landing + 修复） | ≈321k | ~26% | 起草期四项实测 + 零脚手架 landing 全在此 |
| 判官（R1/R2/R3） | 169,365 / 219,523 / 235,610（合计 624,498，口径见上声明） | ~51% | R1 全面审为单轮最大（byte 比对 8 处锚文 + 全仓亲核） |
| 核验官（两轮） | 121,469 / 142,318（合计 263,787，同口径） | ~22% | 六项清单 + 承重插入 byte 比对 9 项 |

花费最大处：判官三轮合计。可压缩分析：R1 六项 finding 中 F1（转义伪影）/F3（申报缺一）/F5（缺齿）/F6（超实证断言）四项属「提交前可自证」类——若主线程在提交 v0.2 前自跑一遍 byte 级锚文比对（判官 R1 用的 node 脚本形态，判官明示「本轮脚本可复用」）+ git status 全量对账 + 断言实证面反照，R1 有机会一轮 GREEN 或只剩 F2/F4 两项设计类 finding，估计可省 R2/R3 两轮（~455k 报告值口径）。核验 FAIL(F1) 同理：插入段落盘后自跑一次 byte 比对即免一轮（~142k）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · T2-ii 承重插入段落盘漏 1 字符（核验官 FAIL 抓出）**：现象——SKILL.md:48 插入段作「非 TTY 形态」，FROZEN §2 基准为「非 TTY **两**形态」（490 vs 489 codepoints，共同前缀 456/后缀 33）→ 根因——插入文本由主线程手打进 Edit 参数，落盘后未对 FROZEN 原文跑 byte 比对就申报「文本 == FROZEN 插入段」（批 A retro 教训 4「锚文机械提取 + 落笔前 byte 对账」只执行了锚文提取半边，**插入文本侧的对账缺位**）→ 证据锚：landing-verdict.md 首轮 FAIL(F1) 段（字符级 diff 三重机证）+ summary §3.2。
- **F2 · summary §3.1 换行形态披露不实（核验官 ADVISORY 抓出）**：现象——我在 summary 声称「Edit 工具把六个 CRLF 文件规范化为统一 LF」，实况六件纯 CRLF 全程未变（verifier 复跑 census：dev-loop 75/75、SKILL 138/138、gate 601/601、merge-driver 1559/1559，bareLF 全 0）→ 根因——我用 msys `awk '/\r$/'` 测得 bareLF=75 假读数（msys awk 对 CRLF 记录剥 `\r` 的工具行为），**单信号源正读数未交叉第二源**（`git ls-files --eol` / `tr -dc '\r' | wc -c`）就写进正式披露段——ARQ-260914「工具负结果先交叉第二信号源再归因」教训的镜像形态（这次是正结果）→ 证据锚：landing-verdict A1 段 + summary §3.1 修订版注记（三信号源裁定实录）。
- **F3 · R1 六项 finding 的共性根因 = 提交前自证缺位**：现象——R1 BLOCK(F1–F6)：转义伪影（BLOCKING，blockquote 承载含反引号 JS 行）、触发条件过宽、B6 申报缺一件（批 A R1-F4 同型第 2 实例）、self-check 契约缺位、V1 缺齿、断言超实证面 → 根因——v0.2 提交时把「byte 保真 / 申报全量对账 / 断言实证面反照」留给判官做，起草期自检两句（preflight + §5 实跑）只覆盖了形态与基线，不覆盖引文逐字性 → 证据锚：§7 R1 findings 六条（每条带 file:line 机证）。
- **F4 · settings-lint 夹具首跑自红（正面摩擦，痛感低）**：现象——self-check 6 例 1 红（`good-no-permissions` 夹具不含 `permissions` 字样，撞真空绿前置断言）→ 根因——夹具设计时前置断言口径（须含 permissions 段）与该好夹具的测试意图（无 permissions 段文件）未对齐 → 处置——改名 `good-permissions-no-arrays` + 夹具补 permissions 空对象，6/6 绿；**这正是判别性义务的自证**（self-check 真在咬）→ 证据锚：summary §3.3 + 本轮实跑输出。
- **near-miss（最险一刻）**：R1-F1 的转义伪影——PROPOSAL 初稿把 merge-driver 插入行写在 blockquote+code-span 里并以 `\`+反引号转义，**该文本是将 byte 照抄进 G3 门脚本的 JS 源码行**：若未在 FROZEN 前被抓出，landing 照抄即 `SyntaxError` 直接损坏 merge-driver（dev Pod 合并链全断），且 preflight/结构门全都不查引文逐字性——零机械面能拦。没出事是因为判官 R1 把「零脚手架批的 §2 引文 = landing-verifier 唯一比对基准」当作重点做了 8 处锚文 byte 级机械比对（判官 R1 记录 :309-311）。教训：**含反引号/转义字符的承重代码行必须用围栏代码块承载**（PROPOSAL 模板读法注未覆盖此形态）。

## 4. 优化建议（按 ROI 排序）

1. **承重引文 byte 级自证脚本化（起草侧 + landing 侧双钩）**：治 F1/F3/near-miss——把判官 R1 与核验官各自现写的 node byte 比对脚本固化为可复用件（如 `landing-fidelity-check.mjs --cite-check` 或独立脚本）：起草期对 PROPOSAL 全部「锚定原文（逐字）」引文 vs 磁盘自跑，landing 后对全部插入/替换文本 vs FROZEN §2 自跑；同根已三现（批 A R1-F1/F2、本批 R1-F1、本批核验 F1），够格机械化。载体 = **ARQ 收件箱（gov-arq 登记，gov 域）**——批 A 教训 4 的 memory 载体已承载纪律面，本条是其**机械化**候选，与 `ARQ-260924-landing-summary-template-pointer-gap`（模板指针缺位，同根）合并查重后落条目。
2. **PROPOSAL 模板读法注补「围栏承载条款」**：治 near-miss——含反引号/`${}`/转义序列的承重代码行禁用 blockquote+code-span 承载、必须围栏代码块（byte 保真 + 渲染稳）。载体 = **ARQ 收件箱**（模板属治理文面，改动走后续批；本条为一行级）。
3. **字节/换行类断言双信号源规约**：治 F2——工作树形态类断言（CRLF/LF、BOM、尾空白）必须两个独立信号源一致才可写入正式制品（如 `git ls-files --eol` + `tr -dc` 字节计数）；msys awk/sed 的 `\r` 剥离行为列入已知工具伪影清单。载体 = **ARQ 收件箱**（与 ARQ-260914「负结果交叉」条目同根追注候选，登记时查重）。
4. **申报生成方式改「全量机械枚举」**：治 F3-③（B6 缺一件，批 A R1-F4 同型第 2 实例）——工作树实况申报不手写清单，直接粘 `git status --porcelain` 全量输出再逐行注性质。载体 = **登记不硬治**（自律件，memory `partial-view-as-whole` 已有载体，本条是其在申报面的具体化；两次同型实例已在册，第 3 次出现时升 ARQ）。
5. **§5 幂等类断言强制标评估时点**：治 A2——「render 复跑后 git diff exit 0」类断言在 pre-commit 恒假（工作树对 index 有 diff），模板 §5 已有时点标注纪律（memory：§5 断言标评估时点），本批 FROZEN 文本标了双时点但子句未逐条拆分——细化为「双时点标注的验证集，逐子句注明归属时点」。载体 = **ARQ 收件箱**（模板读法注候选，与建议 2 可并批）。

## 5. 一句总结

成本 ≈1.2M tokens 报告口径（主线程 ~321k + 判官 624k + 核验官 264k，resume 口径声明见 §2）/ 全环 ~3h / human 触点 **1 次**（立项 ack + OQ 三裁同场，此后全自动至 commit）/ 制品 4 件齐 + 账本 2 条结项 / 对照批 A：评审多一轮、核验多一轮，多出的两轮全部由「提交前自证缺位」贡献（§4-1 可回收）· 最险一刻 = 转义伪影差一点 byte 照抄进 merge-driver（G3 门脚本 SyntaxError 级损坏，preflight 与结构门均不拦此类，判官 byte 比对是唯一网）· **保险丝判定：两方独立结构接住作者偏差**——判官 R1 独立抓出 6 项（含 BLOCKING 伪影，主线程自跑 preflight 全绿照样带病），核验官独立抓出落地单字符偏差 + 披露不实（主线程自跑 V1–V6 全绿照样带病）：两道网各自抓到了对方抓不到的层（判官抓冻结前文本、核验官抓落地后 byte），零依赖主线程自评 · 最值得带走的一条：**「机械门全绿」与「引文逐字」是两个正交面——preflight 只保证形态可解析，逐字性在零脚手架批里唯一基准就是 §2 引文本身，落笔前后各做一次 byte 对账比任何事后解释都便宜。**

## 6. 本环遗留（条件节）

1. **PROPOSAL §6 留根 1–7 全部维持**（真交互 TTY 观察项 / headless-260911 张力点记录不考古 / 建议① 证据接入带量化触发器 / lint `Read(` WARN 级复证 / VERSIONED_ARTIFACT_RE_LOCAL 双源 / headless 恢复前置归 human / 260908 V4 历史断言失效注）——去向均在 FROZEN §6，不重复展开。
2. **A2 处置兑现记录**：FROZEN §5 V4 幂等子句于 commit 后复跑——`node .claude/scripts/crq-render.mjs` 后 `git diff --exit-code -- docs/crq/` → **exit 0**（2026-09-24 commit 11fb5c1 后实跑），底层幂等性坐实；评估时点歧义教训入 §4-5。
3. **V5 后半**：合并 main 后须再跑 `changes-index.mjs --upsert` 翻 mergedInMain（commit 后 upsert 已翻 landed）；gov 领先 main 的既存两笔（23aed35/77000d9）随本批 ff 一并进 main（任务书先例口径，不单独发起合并）。
4. **detect_changes 影响面核对 = N/A (worktree)**：本批在 gov worktree（工作树与主索引 checkout 不一致），按 commit-safety 钩子口径以 `git diff --cached --stat`（17 files 735+/44−）+ 触面枚举替代抽查；本批触面全部为治理文面/脚本（`.claude/**` 在 `C-AI-KidPal` 主索引排除面、`KidPal-claude` 图索引覆盖 hooks+scripts——settings-lint.mjs 为新文件，待该项目下次重建/自刷新入图）。
5. **remote push 归 human**（本批不 push）；§4 五条建议的 ARQ 收件箱登记随下一治理批的「retro §4 载体兑现」环节处理（批 A 先例：77000d9 形态）。
6. **§4 载体兑现记录（2026-09-24，批 C 前置动作，独立 commit）**：§4-1/2/3/5 经 gov-arq 查重（grep 根因关键词面）判定四条均与 `ARQ-260914-landing-preflight-anchor-parse` 既有子面同根——逐字性子面（§4-1 机械化候选 + §4-2 围栏承载条款）、③ 连带项评估时点子面（§4-5 逐子句细则）、「工具负结果先交叉第二信号源」条款的正读数镜像（§4-3 双信号源规约）——按「同根追注、不新立」纪律追注该条目尾（含批 B 两实例证据与代价量化），未新立条目；§4-4 按原裁定不入账（载体 = memory `partial-view-as-whole`，登记不硬治）。追注尾段已标「载体合批提示」：(2)(3)(5) 改法候选与候补 ARQ-260924-landing-gate-dir-anchor-gap ③ 同落 PROPOSAL.template 面，批 C 立项 ack 时呈收圈取舍。
