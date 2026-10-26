# retro · 261025-spec-reconciliation-memory-writing-scope（Track B 全环：记忆写入范围口径对账——需求面 + 架构面）

> 主线程第一人称热记忆 · 2026-10-26 · harness 形态 = story-author / architect / requirement-reviewer / architect-reviewer / landing-verifier 均为独立子 agent；需求面与架构面串行（需求先 frozen 再起架构）。

**【硬项 R1 · 环后趁热、第一人称】** 落地与核验当轮即写，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens = harness usage 通知的累计值（含上下文重载，非净增量）；主线程 tokens 未单独计量（本会话多批次共享）。时长 = 通知 duration_ms。

**【硬项 R3 · 摩擦带根因 + 证据锚】** 见 §3。

**【硬项 R4 · 建议必标载体】** 见 §4。

**【硬项 R5 · 保险丝判定】** 见 §5。

## 1. 时间账

| 相位 | 内容 | 时长 | tokens |
|---|---|---|---|
| 需求起草 R1 | story-author 盘上起草（回合粒度版） | ~6.0 min | ≈70k |
| 需求核实 | architect 可行性三件（首轮判出 🔴「提醒语义回合归类」） | ~2.8 min | ≈102k |
| 需求起草 R2 | story-author 按「内容归属」重写 + 残余申报 | ~7.2 min | ≈82k |
| 需求实践核对 | architect 拿实现与 live 数据逐条核新措辞（6 条被点名，含包装指令缝隙） | ~4.1 min | ≈194k |
| 需求审稿 | requirement-reviewer 四查 → PASS 零否回 | ~4.3 min | ≈46k |
| 架构起草 | architect 起草 PROPOSAL（T1–T9） | ~5.9 min | ≈205k |
| 架构评审 R1 | architect-reviewer 四查 → GREEN 零否回 | ~7.6 min | ≈102k |
| landing | 主线程 packet-bound 落 T1–T9（三文件） | ~15 min | 未单独计量 |
| 核验 | landing-verifier 独立核验 → PASS（两条 advisory） | ~4.0 min | ≈49k |
| **合计** | | **~57 min**（主线程落地另计） | **≈85 万**（子 agent 合计，±10%） |

## 2. Token 账

本环 token 消耗的大头不在生成、在**核实**：需求面 architect 的两次核实（可行性 102k + 实践核对 194k）合计 296k，占全部约 35%。这笔钱花得值——第一版草稿的两个语义错误（回合粒度、提醒语义回合误归类）都是核出来的，不是猜出来的。架构面 PROPOSAL 205k 也偏高，成因是它要把 10 条实践残余逐条落到具体行号。

## 3. 摩擦与根因（逐条带证据锚）

**F1 · 需求面第一版按「回合粒度」写，被 human 当场纠正（最贵的一条）**
- 现象：`AC5 第一段「设备流程回合…统一归属、照常参与记忆」`，读起来像「小伴的回复也进孩子画像」，与实现相反。
- 根因：我把 architect 首轮裁决里的「统一归属」直接翻译成 AC 措辞，**没有先确认「一个回合里存在两种归属」这个实现事实**。而该事实（assistant 消息只产 agent 侧结论、不进 owner 画像）在 architect 首轮核实报告的 §2 里就写着，我是被 human 质疑后才回去读的。
- 证据锚：`docs/requirements.md` v1.68 §9 行流程留痕；`delivery/intake/261025-spec-reconciliation-memory-writing-scope.md` openQ OQ-3；architect 核实报告第 2 条。
- 教训：**需求措辞的单位必须与实现的归属单位对齐**。裁决词（「统一归属」）是结论，不是措辞；落到需求面要重新选单位。

**F2 · 第一版草稿把「提醒到点语义回合」放进「统一归属」侧，属冻结陈述的反转**
- 现象：该回合跑在 main agent 的 cron 上、孩子没开口，却被列入「孩子原话照常记入」的例举。
- 根因：改写时沿用了 v1.67 的枚举顺序，没有逐条回核每项的实况；而 v1.67 §9 历史行里该项本就列在「不入画像」一侧。
- 证据锚：architect 可行性报告第一轮 🔴 项；`docs/architecture.md` §6.7 表现第 3 行（落地后）。
- 教训：改枚举型条文时，**每项都要单独回核实况**，不能靠「原顺序 + 新口径」批量平移。

**F3 · 派发架构面时目录名缺 `arch-review` 写槽 token，被 enforce-scope 拦**
- 现象：派发指定 `delivery/review/261025-spec-reconciliation-memory-writing-scope/`，architect 落盘时被拒，改落 `261025-arch-review-spec-reconciliation-memory-writing-scope/`。
- 根因：主线程按 changeId 直接起目录名，未按 `arch-review-protocol.md`「双轨路由」的命名条款带 token。
- 证据锚：PROPOSAL 头部落盘路径注记；该批 §12 变更记录行指向合规目录。
- 教训：**changeId ≠ review 目录名**。派发 prompt 里给定路径前先核写槽 token。

**F4 · landing-summary 的改动行数记错（+43 实为 +33）**
- 现象：summary §2.4 写 `docs/architecture.md +43/−10`。
- 根因：读 `git diff --stat` 的图宽数字当插入数，未用 `--numstat`。
- 证据锚：landing-verdict.md ADV-1；summary 已按 `--numstat` 更正并留痕。
- 教训：**落账数字一律取 `--numstat`**，`--stat` 的数字含图宽不可直接引用。

**F5 · spine faceMap 漏列 `architecture.md:192`**
- 现象：该行「引导 / 整理一次性 LLM 回合不携 `user`（user 消息不入库）」与 live 相反，但初版面图是从 261024 docs-delta 的六条过时面平移来的，那份清单本身漏了它。
- 根因：面图直接消费上游批的申报清单，未独立 census。
- 证据锚：spine faceMap 后补注；architect 核实报告第 5 条「landing 完整性」。
- 教训：**面图必须独立 census 一次**，上游清单只能当起点。

## 4. 建议（每条标载体）

1. **`/arch-change` 派发模板加一句目录命名规则**（changeId ≠ review 目录名，必须含 `arch-review` 写槽 token）。载体建议 = `/gov-change` 治理面批（`.claude/references/arch-change.md` 属 G2，需走治理 loop）；本批不夹带。
2. **packet-bound 落地的计量纪律**：summary 数字取 `--numstat`。载体建议 = 并入上条治理批（`arch-change.md` 或 `track-b-verifier-brief.md` 加一行）。
3. **enum 型条文的改写纪律**（每项单独回核实况，不按原顺序批量平移）——与 F2 同源。载体建议 = 已有 `write-story` 或 requirement-change SSOT 的「起草纪律」节可加一行；同样归治理批，不夹带本批。
4. **本轮不需要新增 debt**：F3/F4/F5 均为执行纪律类、已有 SSOT 位置可承载，无机制缺口；F1/F2 的教训已在需求面流程内被 human 保险丝接住，无需另立。

## 5. 保险丝判定（R5）

本环有**两次人肉保险丝**发挥作用：

1. **需求面口径错误**被 human 当场纠正（F1）——这是本环最贵的一次摩擦，但被接住了，且接住之后触发了两次独立核实（architect 实践核对 194k），把「包装指令被抽成过程性结论」这条此前无人知晓的实践缝隙挖了出来并登记成 `TECH-261026`。
2. **架构面 stale 行漏列**（F5）由 architect 在核实阶段自捕，未流入 landing。

机械保险丝方面：`spec-anchor-check` 全程绿（未提供额外保护，因为本批未断锚）；`landing-fidelity-check` 与 landing-verifier 独立核验抓出了 F4 的记账错误并判 PASS。

总判：**保险丝有效，但 F1 说明「裁决词 → 需求措辞」这一步缺少机械或流程保护，目前完全靠 human 在场**。若将来需求面批不设 human 复核点，同类错误会静默流出——这是 §4 第 3 条建议的现实动因。
