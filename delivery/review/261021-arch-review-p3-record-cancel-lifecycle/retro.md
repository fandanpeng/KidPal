# retro · 261021-arch-review-p3-record-cancel-lifecycle（Track B 全环：triage 跨面 intake 内联 → arch+契约面轻批主路径）

> 主线程第一人称热记忆 · 2026-10-21 · 乘客 #N（261019 批次日同缝续作）· harness 形态 = 判官 / 核验官均为零上下文子 agent 全形态（非降级）；triage 分流 architect 同为零上下文子 agent · 跨乘客对照：261019 批（同缝上游批）为 headless 重面全批，本批 = spine facePlan 顺序 1 内联轻批首例（triage+arch 两门同一主线程会话贯通）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 翻正段未落（landing-verifier 正在补 PASS 机器锚）时即写；判档 = 主路径，retro 不豁免。

**【硬项 R2 · 计量诚实】** 子 agent 侧有 usage 计量（各 handback usage 字段逐轮累加，误差 ±5%）；主线程侧无计量——§2 以会话轮次估，误差界 ±30%，不装精确。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| triage | debt 查重 + 分流 architect（四面探测+五档+reviewed plan，52 tools） | 会话早期 | 触点①四裁决 | ~22min | 132k |
| 落盘 | spine + plan.md + debt 闭账 + INDEX 刷 | 触点①后 | 落盘毕 | ~3min | 主线程 |
| 起草 | PROPOSAL v1.0（33 tools，preflight 绿） | 派发后 | handback | ~19min | 136k |
| 评审 R1 | 四查 → BLOCK(F1,F2) | 派发后 | handback | ~8min | 77k |
| 修稿 | v1.1（F1 增列 T5 / F2 impact 改写 / 裁定落稿） | R1 转 | handback | ~8min | Δ~24k |
| 评审 R2 | 聚焦复核 → GREEN | 派发后 | handback | ~2min | Δ~20k |
| FROZEN+landing | §7 转录 / 标 FROZEN / T1–T5 四处落地 / CDQ / anchors | R2 后 | 落地毕 | ~8min | 主线程 |
| 核验 | landing-verifier FAIL（summary 格式两件）→ 补件 → 门复绿 → 翻正中 | 派发后 | 本 retro 时点 | ~10min+ | 79k+ |
| **合计** | 起草到核验翻正 | — | — | **~80min** | **子 agent ≈ 468k** |

对照：261019 批全环（含需求档 2 + arch 重批 + 实现包 S1–S5）跨多会话多日；本批单会话 ~80 分钟走完 triage → arch 批核验，facePlan 内联轻批的吞吐优势成立（但批体量不可比：本批 canonical diff 仅 +7/−2 行）。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（triage 落盘 + 转录 + FROZEN + landing + retro） | 估 60–90k（±30%，无计量） | ~15% | 大头是 PROPOSAL 全文两读 + 各回执转录 |
| 分流/起草 architect（同一实例两次停走） | 160k（累计） | ~34% | 三角色（探测/起草/v1.1）共 85 tools |
| 判官 R1+R2（同一实例） | 96k（累计） | ~21% | R2 聚焦复核 Δ 仅 20k——聚焦复核省 token 显著 |
| 核验官 | 79k+（翻正段未计） | ~17% | 含隔离实验成本 |
| 其余（changes-index/debt-render/机检复跑） | 忽略 | — | — |

花费最大处：起草 architect（四面探测 + 全锚现核 + preflight 实跑三合一）。可压缩分析：triage 探测与起草对同一批文件各读一遍（两角色零上下文隔离的代价）；若 triage reviewed plan 的锚核结论随 plan 传给起草人，可省起草期约一半现核量——但作者≠审者的独立现核是 R1 全 PASS 的成因，压缩需谨慎（见 §4 建议 3）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · §5 断言预期三处建模误差（landing 期才暴露）**：现象 = 条 3/4/5 机械断言实际值与 PROPOSAL 预期不符（2 vs ≥3 / 3 vs 2 / 2 vs 1）→ 根因 = 起草人推算预期值时未复算「目标词出现在同批其它插入件自身文本中」（T1 变更记录条自引 start_organizing、T4 §12 行自引「取消链显式化」）且把 `grep -c` 当次数口径（实为行计数，新行一行含 2 次命中）→ 证据锚 = architect-landing-summary.md 偏差申报表 + landing-verdict 独立裁断三处属实。R1/R2 均未覆此层（评审实跑记录的是基线值，预期值是纯推算）。
- **F2 · landing-summary 格式硬项无模板承载**：现象 = verifier 结构门 FAIL，唯二阻塞 = summary 缺 ```landing-manifest``` 围栏 + 缺字面 packet-id token → 根因 = 这两件是门判据（landing-fidelity-check.mjs 结构字段硬条款）但只活在先例文件与门脚本里，无 summary 模板成文 → 证据锚 = landing-verdict.md FAIL 段 + 261019 先例同件 :3/:39 对照。
- **F3 · changes-index `--upsert` 误用一次**：现象 = 对 spine 跑 `--upsert <id>` 报 naming 错（该旗标是 review lane 门，要求 delivery/review/<id> 在场）→ 根因 = 我按 AGENTS.md「状态转移时 --upsert」常驻句操作，未先辨 lane（spine 归 intake lane、走全量重建即收编）→ 证据锚 = changes-index.mjs checkArchDirNaming :528-531。无损，全量重建兜住。
- **F4 · triage architect 无图工具**：现象 = 派遣的 architect 工具面未挂 codebase-memory-mcp，爆炸半径降级手工 grep 计数 → 根因 = 子 agent 定义的工具清单不含 MCP graph 工具 → 证据锚 = 分流回执 §0 探测方法声明。已在 plan.md 如实标注「integrator 收口时以 detect_changes 实测校正」。
- **near-miss（最险一刻）**：F1 的条 4 断言若按「预期 2」机械判——实际 3——最坏走向是我误判「落地夹带」而回滚正确 diff，或反向：若实际值恰等预期（碰巧）则带病过门。没出事的原因 = 断言失配触发逐项核因而非二值取舍。教训 = 验证集预期值必须可复算（来自命令实际输出），不可来自起草推算。

## 4. 优化建议（按 ROI 排序）

1. **PROPOSAL §5 断言预期值改为「起草期实跑输出回填」硬项**：治 F1 · 载体 = ARQ 收件箱（已登记 ARQ-261021-proposal-assert-expectations，docs/debt/gov/）· 预期收益 = 消灭「推算预期 vs 实跑现实」整类失配，R1/R2 评审可机核预期值出处。
2. **landing-summary 最小模板成文**（packet-id 行 + manifest 围栏 + 落地对应表 + 偏差申报节四件硬项）：治 F2 · 载体 = ARQ 收件箱（已登记 ARQ-261021-landing-summary-template，docs/debt/gov/）· 预期收益 = verifier 结构门首跑即绿，省一轮 FAIL→补件→翻正环。
3. **triage reviewed plan 附「锚核结论附录」供起草人消费**（不减独立现核，只免重复 grep）：治 F4 姊妹问题 · 载体 = 登记不硬治（观察下一批内联轻批是否复发同成本）· 预期收益 = 起草期省 ~10min。
4. **AGENTS.md「--upsert」常驻句补 lane 辨识半句**：治 F3 · 载体 = 登记不硬治（一句常驻指令微调，下批 loop-retro 或下一次触碰 AGENTS.md 的治理批顺手带）· 预期收益 = 免一次报错往返。

## 5. 一句总结

成本 ~80 分钟 / 子 agent ≈ 468k tokens / human 触点 1 次（triage 触点①四裁决一次拍完，此后全自动）/ 制品 4+3 件（PROPOSAL + landing-summary + landing-verdict + retro + spine + plan.md + CDQ）/ 对照 261019 重批：内联轻批单人会话贯通可行。最险一刻 = §5 断言失配差点引发正确 diff 回滚误判，靠逐项核因而非二值取舍化解。**保险丝判定：两方独立结构接住作者偏差**——判官独立接住内容层（F1 impact 事实断言、F1 闭集枚举漏项，均 BLOCK 打回后修）；verifier 独立接住格式层（summary 两件缺口 FAIL）+ 独立复断判官未覆的 §5 预期层（三偏差裁属实不阻 GREEN）；起草人两处偏差（§5 预期推算、summary 格式）全部被接住，无一漏网。最值得带走的一条 = **验证集预期值必须来自实跑输出，不可推算**（F1 根因，已固化 ARQ）。

## 6. 本环遗留

- 实现面 F1–F4 全量 → dev 包 `261021-p3-record-cancel-lifecycle`（launch 前置 = 契约 anchor 回填——本批已回填，包可起；烧板轮不可省）。
- wire 语义端到端证据（验收种子 1–3 + 真机复验①）→ dev 包，非本批门。
- 「讲错啦」语音取消通道 → red-baseline story-p3 V31(b) 既有承载位，不另登（spine q4）。
- 本批 spine status 维持 in-flight（实现面 facePlan 顺序 2 未走完）；intake INDEX 对本 spine 的 faceMap 投影空值 + ⚠值域外（R1 旁注：faceMap 平文字串疑似未被 changes-index.mjs 解析）→ 随 ARQ-261021-landing-summary-template 一并交 loop-retro 观察面，检测不拦截。
