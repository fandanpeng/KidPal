# retro · 260922-arch-review-spec-v5-ui-align（Track B 全环：spec-lane 单文件对齐，R1 一轮打回即绿、verifier 一轮 RED 即绿）

> 主线程第一人称热记忆 · 2026-09-22 · 同日全环（设计迭代与治理环同一会话，治理环为其中一段）· harness 形态：判官 / 核验官 = 独立子 agent（architect / landing-verifier 定义形态，非降级）· 跨乘客对照：spec↔design 对齐类首例（此前乘客均为 governance 类）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 复判 GREEN 后即写，同会话热记忆。

**【硬项 R2 · 计量诚实】** 见 §2 声明。

**【硬项 R3 · 摩擦带根因 + 证据锚】** 见 §3。

**【硬项 R4 · 建议必标载体】** 见 §4。

**【硬项 R5 · 保险丝判定】** 见 §5。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL v1.0（preflight 绿） | ~16:00 | ~16:20 | ~20min | 无计量（见 §2） |
| 评审 R1 | architect **BLOCK**（4 打回 + 4 小修 + 3 advisory） | 16:2x | 16:3x | 629s | 60,742 |
| 修订+R2 | 主线程修订 v1.2 → architect **GREEN** | 16:3x | 16:4x | 134s（resume） | ~9,400（delta） |
| FROZEN | 立项 ack + GREEN + 契约零触点 + preflight | ~16:45 | — | — | — |
| landing | T1–T6 落地 + 验证集六条实跑 | ~16:45 | ~17:2x | ~35min | 无计量 |
| 核验 R1 | landing-verifier **RED**（1 blocking 结构件） | ~17:3x | ~17:4x | 457s | 49,458 |
| 修复+复判 | summary 补围栏块/packet-binding → 结构门 ok → **GREEN** | ~17:4x | ~17:5x | ~15min（含 verifier 复判） | 复判未单独计 |

对照：治理环全程约 2 小时内闭环、判官一轮打回、verifier 一轮 RED——比 260921-gov-review-launch-pipeline（多轮打回）显著轻，与单文件对齐的量级相称。

## 2. Token 账

计量方式诚实声明：判官 / 核验官 tokens 来自子 agent 任务计量（准确）；主线程无计量，估计法 = 本环对话段（起草 + 修订 + 落地 + 修复）约占会话上下文的 1/3，量级 80k–150k，误差 ±30%。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 落地 + 修复） | ~80k–150k（估） | ~55% | 含 requirements.md 逐处编辑与验证集复跑 |
| 判官（R1 + R2） | ~70,100 | ~30% | R1 60,742 + R2 delta ~9,400 |
| 核验官（R1 + 复判） | ~50,000+ | ~17% | 49,458 + 复判未计 |

花费最大处：主线程 landing 逐处编辑（约 25 处、两次正则批改 + 三轮手工勘正）。可压缩分析：键帽正则若首轮形态枚举对（见 F3）可省一轮批改与复验；summary 结构件若照先例抄（见 F1）可省整轮 verifier RED。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · landing-summary 结构件双缺 → verifier RED 一轮**：现象 = 首版 summary 缺 ```landing-manifest 围栏块与 packet-binding 字面量，结构门两次 fail-loud（先 B2 后 A9）→ 根因 = **summary 的结构义务不在 PROPOSAL 模板正文，只活在 landing-fidelity-check.mjs:560-570 的断言里**，主线程第一人称起草没读机检脚本的断言清单 → 证据锚：两次 `ok:false` errors 输出（"缺 packet-binding（须引 packet-id + 被批 review id，A9）"）、verdict RED 段。
- **F2 · 验证集第 5 条初稿不可机械判定 → R1 打回**：现象 = 「倒计时」grep 预期写成散文（「历史文字合法」），判官指出不可判且漏 :356 正交概念 → 根因 = 写「零残留」预期时没枚举合法命中闭集 → 证据锚：architect-review-r1.md R3。
- **F3 · 键帽正则形态写反，首轮只改一半**：现象 = BOOT 式正则写成 `\[X BOOT\]`，实际形态是 `[BOOT X]`，首轮输出「BOOT 式 0」但 diff 里 `[BOOT 结束]` 残留 → 根因 = 批改正则前没先 grep -o 枚举实际形态 → 证据锚：首轮脚本输出「KEY式 10 BOOT式 0」vs 复验 grep 残留 10 处；复跑后清零。
- **near-miss（最险一刻）**：FROZEN 后落地时新写的三处 v1.57 注记措辞自带「倒计时」字样（wireframe 注释 / 布局优先级句 / 可读性分级句），超出 FROZEN 验证集的枚举闭集——验证集⑤复验时自查抓到、赶在 verifier 之前改掉。若漏过去 = 又一轮 RED 且打的是「落地超 FROZEN 口径」。教训：**FROZEN 之后新写的每一句落地文本也要过验证集口径**，不只改既有文本。

## 4. 优化建议（按 ROI 排序）

1. **summary 结构件 checklist 固化**：治 F1 · 载体 = **固化候选 → ARQ 收件箱**（在 PROPOSAL 模板或独立 summary 模板补「summary 结构义务」节：围栏块 + packet-binding 字面量 + baseRef 用法注记）· 预期收益：消灭本环唯一 RED 的复发面，每乘客省一轮 verifier 往返（~50k tokens + 15min）。
2. **「词清除类验证集 = 枚举合法命中闭集」写进模板 §5 指引**：治 F2 · 载体 = **固化候选 → ARQ 收件箱**（与 260906 批真空绿规约同族）· 预期收益：判官少一类可预期打回。
3. **批改正则前先枚举形态（grep -o 列形再写 pattern）**：治 F3 · 载体 = **自律件**（本 retro 记录即生效，不立 ARQ）· 预期收益：杜绝「改了一半」类静默半改。
4. **landing-fidelity-check 不带 baseRef 会误报 claimed-not-landed**：治 verifier 备查注记 · 载体 = **登记不硬治**（verifier 已记入 verdict 备查段；脚本默认窗口 HEAD~1..HEAD 对「landing 后有修复 commit」场景不友好）· 预期收益：后续批次少一次误报排查。

## 5. 一句总结

成本 ~2 小时 / 200k± tokens 量级 / human 触点 1 次（立项 ack 含 OQ 三问）/ 制品 6 件（PROPOSAL + summary + verdict + retro + r1 + r2），对照单文件对齐量级相称 · 最险一刻 = FROZEN 后新写文本自带被清除词、自查兜住 · **保险丝判定：接住**——判官独立接住 4 项起草盲区（sweep 漏扫 ×2 / 验证集不可判 / packet 枚举缺口），verifier 独立接住 summary 结构件缺失，两方捕获面互补、零重叠遗漏 · 最值得带走的一条：**结构义务活在机检脚本里时，起草者必须先读断言清单再落笔**。

## 6. 本环遗留

- minor（verifier 不阻塞）：T1④ 日期格式口径句落 AC1 而非补充规则——实质等价、summary 已披露，不返工。
- 留根（PROPOSAL §6）：设计母版 v4 残留段（html :1128/:1129/:700/:704/:1135 沿引 v3/v4 表述）另批清；「设计先行→spec 同步」漂移窗口管控交日落复盘。
- 「设计交付三件套」流程约定（布局锚图入库 / launch-prep 输入清单 / 截图管线归属）仍待另批 PROPOSAL——本环只做 spec 对齐，未收流程面。
