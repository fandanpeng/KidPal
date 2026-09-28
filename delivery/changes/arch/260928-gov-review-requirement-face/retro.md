# retro · 260928-gov-review-requirement-face（Track B 全环：立项对齐 → R1 BLOCK→R2 GREEN → FROZEN → landing → verdict 绿）

> 主线程第一人称热记忆 · 2026-09-28 · 乘客 #（当日治理批序未清点，按当日序）· harness 形态：判官 / 核验官 = 同会话主线程派子 agent（architect 两轮延续上下文、landing-verifier 零上下文）· 跨乘客对照：R1 的 F1/F2 均为「起草期实证义务」此前多批反复钉过的同族坑（清扫 pattern 截断串 = 短引据残留清扫同族复现）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写，同日热记忆，未隔场。

**【硬项 R2 · 计量诚实】** 主线程无 token 计量，按会话体感估计 ±30%；判官 / 核验官有子 agent usage 实测（harness 报数）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|±30%| |
| 对齐 + 起草 | human 拍五点（B 方案 / 对偶 / 无终确认 / 跳过例外 / REQ- 改名）+ showme 图示两版 + PROPOSAL v0.1 | 会话前段 | | ~60 min（体感） | 未计量 |
| 评审 R1 | architect BLOCK(F1,F2)+SHOULD×5+NIT×4 | | | 676 s | 98,629 |
| 修订 v0.2 | F1–F10 全处置 + preflight 复跑 | | | ~20 min（体感） | 未计量 |
| 评审 R2 | architect GREEN + MINOR×4 | | | 184 s | 94,755 |
| FROZEN | human 裁 OQ-5=(a) + 冻结确认 | | | <1 min | — |
| landing | T1–T6 packet-bound + V1–V6 | | | ~25 min（体感） | 未计量 |
| 核验 | landing-verifier 绿（A1–A3 非阻断） | | | 497 s | 62,649 |
| **合计** | | | | 约 2.5 h（体感 ±30%） | 子 agent 合计 ~256 k（实测） |

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|1 旧格式误设不影响判读| |
| 主线程（对齐/起草/落地/簿记） | 未计量（估计占总量一半以上） | — | harness 无主线程 metering |
| 判官（R1+R2） | 193,384 | 子 agent 侧 ~75% | 同 agent 两轮延续上下文——R2 只跑 4 工具调用即收口，延续上下文省钱明显 |
| 核验官 | 62,649 | 子 agent 侧 ~25% | 零上下文，20 工具调用 |

花费最大处：判官两轮。可压缩分析：R2 延续上下文实证有效（184 s / 4 调用），后续批维持「R 复核续同 agent」。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 清扫 pattern 截断串陷阱**：现象 = V6 初稿用截断串 `ARQ-260916` 作残留清扫 pattern，判官实测撞 4 个无关同日条目 → 根因 = 残留清扫的本能写法是「短而有信号」的串，但 CRQ ID 族共享日期段，短串信号弱噪音强 → 证据锚 = review.md R1-F1 + §3 复扫硬项修订。教训固化已在案（§3 硬项 + path-form-family 教训同族）。
- **F2 · changeId 撞名**：现象 = 初稿「§9 版本行 + changeId」让 changeId 无铸造来源且撞 TaskPackage 强语义 → 根因 = 引入新标识符前未全仓查该 token 既有语义（changeId 在 task-intake-and-slicing.md:91-97 已被 TaskPackage 占用）→ 证据锚 = review.md R1-F2 + task-intake-and-slicing.md:91-97。教训 = **新标识符入流程前先 grep 全仓既有占用**。
- **F3 · 真「scope-map 零触」需要如实披露而非沉默**：现象 = 初稿 §3 证成引了失准先例且未披露两 agent 系全仓首批未登记 agent → 根因 = 证成写作时倾向挑好看先例 → 证据锚 = review.md R1-F4。教训 = 「未触」证成的先例必须亲核，披露义务优先于漂亮结论。
- **near-miss（最险一刻）**：V6 截断串若未被抓，landing 后按字面「零命中」销项会**永不可达**（4 个无关同日条目永远命中），核验官照 summary 执行 V6 将判红或被迫放宽判据——判官 R1 亲实测 catches it（grep 实跑），两方独立结构接住。

## 4. 优化建议（按 ROI 排序）

1. **新标识符入流程前 grep 全仓既有占用**：治 F2 · 载体 = ARQ 收件箱（登记候选）· 预期收益 = 防撞名类 finding 复发。
2. **残留清扫 pattern 一律全 ID/全形态 + 三归类销项**：治 F1 · 载体 = 已固化于本批 §3 复扫硬项 + path-form-family 同族；候选登记 ARQ 收件箱作为通用判官 checklist 项。
3. **R 复核延续同判官上下文**：治 token 成本 · 载体 = 登记不硬治（主线程操作习惯）· 预期收益 = R2 成本降一个量级（184 s / 4 调用 vs R1 676 s / 26 调用）。
4. **未触证成的先例亲核 + 如实披露**：治 F3 · 载体 = 登记不硬治（判官 R1 已实抓，机制已在）· 预期收益 = 减少 SHOULD 级返工。

## 4.5. 环外产出（非批内 scope，随批讨论产生）

- **`/arch-change` 退役方向获 human 明示**（human 2026-09-28：「/arch-change 也应该退役了，现在区分的更细了」）——治理路由演化方向留根 ARQ-260928 批处置，本批仅完成需求面摘出。

## 4.6 human 取舍触点账

立项 ack 1 次 + OQ-1~4 五点批量裁 1 次 + OQ-5 判后呈裁 1 次 + FROZEN 确认 1 次 = **human 触点 4 次**（Track B 前端对齐 + 例外裁定的标准形态）。

## 5. 一句总结

成本 ≈ 主线程 2.5 h + 子 agent ~256 k tokens；human 触点 4 次；制品 4 件 + showme 1 件；最险一刻 = V6 截断串（判官实跑接住）；**保险丝判定：两方独立结构接住作者偏差**（R1 判官实跑 grep 抓住主线程起草的截断串清扫 pattern，landing-verifier 独立实跑 V6 复核销项——作者偏差在被执行前被独立方实证抓住，非靠解释过关）；最值得带走的一条 = **新标识符入流程前先 grep 全仓既有占用**。

## 6. 本环遗留（留根去向）

- triage intake 接线 / `/arch-change` 退役安置 / AGENTS.md 路由补线 / scope-map 登记 + routing fixtures / review 制品化 → ARQ-260928 批（条目 T6 注记已回写）。
- task-intake-and-slicing.md 定稿版本号引用形态 → 拆包链治理下批。
- 语义撞车机检脚本 → 留根 ARQ 候选（retro §4.2）。
- REQ- 域 status 联动细节 → 首个新 REQ- 条目实证再定。
