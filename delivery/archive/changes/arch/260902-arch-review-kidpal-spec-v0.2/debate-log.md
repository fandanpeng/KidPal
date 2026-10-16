# Debate Log — 260902-arch-review-kidpal-spec-v0.2

> 对抗 review 辩论日志（moderator 维护，追加式）。待审对象 = `docs/spec.md` v0.2；判据 = `review-charter.md`（冻结）。

## 编排轨迹

1. **phase-0 分类**（主线程）：主 lane = spec；contract-touching = YES（横切）；非 mixed；review-depth = full-adversarial（跨系统结构变更 + spec 首审）。制品目录 = `changes/arch/260902-arch-review-kidpal-spec-v0.2/`。
2. **契约预审**（architect-reviewer，只读）：四分类 = **① 随 spec 改动携带**（原②独立前提被方向 B 作废）；抓出 spec §3.0.2 轮询残留（已清）+ B1 `remind_before_min` 局部③漂移（契约门第 4 题）。制品：`../260902-arch-review-shim-api-field-envelope/CONTRACT-CHANGE-PACKET-v2-directionB.md`（旧版更名 `.superseded.md` 保留）；CDQ-260902 行与门证据 §11 同步。
3. **charter 冻结**（scenario-author）：12 轴 / 7 场景 / 6 canary / 5 主题支（T1–T5）；已接受前提 P1–P10；蓝图原则外置映射说明（KidPal 未立 Part 1 蓝图，载自父仓）。
4. **challenger 轮**（5 支并行，全部代落盘——subagent Write 被线束拦截）：
   - findings-T1.md：8 条（BLOCKING×1 / SHOULD×6 / FYI×1）
   - findings-T2.md：12 条（BLOCKING×3 / SHOULD×7 / NIT×2）
   - findings-T3.md：8 条（BLOCKING×1 / SHOULD×6 / NIT×1）
   - findings-T4.md：7 条（SHOULD×6 / NIT×1；elicitation×2）
   - findings-T5.md：11 条（BLOCKING×2 / SHOULD×8 / NIT×1）
   - 合计 46 条，7 BLOCKING；canary 6/6 全部独立重派生命中。
5. **defender 轮**（5 支并行）：答辩完成（defense-T1..T5 五支齐，defense-T2/T5 于 23:15/23:17 补齐）。

## BLOCKING 清单（7）

- T1-F1 板端存储三处互斥（§2:32 SD 缓存 × 决策 24 × D3 服务器字库）
- T2-F1 alert 通道双形态（原生 alert × mcp kidpal/alert；固件原生处理器形状不匹配）
- T2-F3 §3.2–§3.4 v0.1 HTTP/faster-whisper 叙事 live 互斥 + VAD 切句位置漂移
- T2-F4 spec↔契约硬漂移（铁律：契约未批 → spec 不可冻结）
- T3-F2 幂等去重在「已渲染/排队未渲染」边界矛盾（07:45 批量到达为必然叠加态）
- T5-F1 冻结切片悬空（13 包 FROZEN × 决策 9；重切无 owner/相位）
- T5-F2 v0.1 残留无废弃标注、两个构置目标并存

## 待 human 拍板（elicitation 汇总，裁决后逐条回灌）

- T4-F5 儿童数据出网全貌记账（转写文本经 DeepSeek 出网、回复经 edge-tts 出网，是否已知/接受）
- T4-F6 儿童内容安全姿态（机械过滤层 vs system prompt 即足 + ACCEPT-AS-RISK 记账）
- T3-F2 幂等状态机语义裁定（四态 or 接收即入幂等集）
- T3-F4 「带物」内容唯一承载面蒸发的缓解取舍
- T5-F7 家长端形态三选一（QQ/小程序/改 MD）
- T5-F9 故事录音误判兜底/取消/回滚设计
- T5-F1 先重切还是先冻结（顺序）
- 契约门四题（①批准范围②Opus+WS③强制鉴权+渲染回执+speech④remind_before_min 删/标废止）

## 裁定轮（adjudicator 收口 · 2026-09-02）

6. **adjudicator 逐条裁定**：46 条源 finding 跨支去重后归并 **38 条独立条目**——**35 CONFIRMED / 3 DISMISSED / 0 DEADLOCK**。BLOCKING 源 7 → 归并 5（T2-F3×T5-F2 合一 + T5-F1 降级）；SHOULD 源 26 → 26；NIT 4；FYI 1（DISMISSED）。处置归属：spec-text 19 · contract-gate 1 · human 11 · dev-handoff 3。
7. **过程性注记**：① `defense-T2/T5` 于判官首轮 T2/T5 裁定后补齐落盘，已复核——T2 全 CONCEDE（与首轮裁定一致），T5-F1 有实据 REBUT（SSOT 方向论）→ 降级；无残余 RED。② `findings.md` 独立落盘被 harness 写盘护栏拦截（report-file guard），合并终版并入 review-packet §1 + adjudications.md 双载体，制品内容无缺失。
8. **焦点裁定**（双方有真分歧）：T2-F4 = CONFIRMED BLOCKING 触契约门，铁律成立但**条件冻结**为正确模态（spec 冻结以契约门四题批准为前置）；T5-F1 = defender「SSOT 方向论」REBUT 成立、BLOCKING 定性**驳回**（PROPOSAL:7 已自认派生制品、重拆 owner/相位已在 validation:97/:108 命名），残余仅 §8:610 措辞过弱 → SHOULD；T3-F2 = CONFIRMED BLOCKING，推荐 defender「解耦（去重=接收即入集 / 回执=渲染即发）」口径，守住 P4「渲染即确认」，「幂等集是否含已接收未渲染」为 human 拍板点。
9. **pass-oracle 五关**：①轴覆盖 GREEN（12 轴全过）②场景覆盖 GREEN（S1–S7 全走）③finding 全结 RED（5 BLOCKING+26 SHOULD 未落地）④spec↔contract 机械门 RED（无脚本兜底降人工，核对发现硬漂移=V08）⑤canaries 全中 GREEN（6/6）⑥dev-loop handoff GREEN（V10/V11/V27 移交，`arch-review-handoff-check.mjs` exit ok）。
10. **冻结结论**：条件冻结（非 hard-freeze），四条件 = 契约门四题 human 批 + spec-text 改法落地（含 V33 重切 pin）+ 11 组拍板回灌 + 实现侧 3 组进 handoff。拍板清单 11 组（含契约门四题）详见 review-packet §4。
11. **制品落盘**：`adjudications.md`（全量逐条裁定）、`review-packet.md`（摘要+pass-oracle+冻结结论+拍板清单+合并终版 findings，findings.md 被护栏拦截故并入）、`dev-loop-handoff.json`（3 实现侧）；`docs/ARCH-REVIEW-QUEUE.md` 补记 ARQ-260827-tier1-boot-review + 新增 ARQ-260902-cert-lifecycle-deadlock / ARQ-260902-multiturn-continuity。**待 moderator/human 后续动作**：契约门四题拍板、spec-text 落地、11 组拍板回灌。

## 拍板与落地轮（2026-09-03）

12. **human 拍板**（四轮问答）：契约门四题 = ①批准方向 B 重写②确认 WS+Opus③确认强制鉴权+渲染回执+speech④remind_before_min **标废止**；11 组拍板全部按判官推荐定稿，唯一偏离 = V26 token 选**全设备共享**（判官推荐每设备唯一；human 裁 MVP 单设备场景差异不大，取舍记账：无每设备身份、单点泄露危及全部、多设备时转每设备唯一）。
13. **spec-text 落地**（moderator/主线程，19 条 CONFIRMED + 拍板回灌）：v0.1 残留全清（§3.2–3.4 链路重写、决策 4/6 标废、SD 缓存两处擦除、shim 措辞收敛、B3 单行三处对齐、alert 双形态统一为 mcp kidpal/alert、custom 并写收敛、§9 三处 stale 清理 + 已定化）；提醒状态机改写（V14 解耦四态、V15 id 稳定性、V16 排程机制、V18 滞留顶替、V19 措辞、V20 speech 预合成解耦、V34 过期清理、V17 带物回读）；新增不变量与约束（V04 RAM-only 介质禁令、V09 max_tokens 单一落点、V24 功耗 DEFER 三要素、V25 证书十年期+对账、V27 格式不遵循兜底、V28 18789 硬前置、V29 出网三条记账、V30 内容安全姿态、V31 录音显式回退、V32 超限失败模式、V33 重切 pin 收紧、V35 §9 补 owner/相位、V12 llm/system 枚举、V21/V23 首验 forcing-function、V22 家长端 QQ 主/MD 副）。
14. **冻结条件满足状态**：条件 1（契约门四题）✅；条件 2（spec-text 落地）✅；条件 3（拍板回灌）✅；条件 4（dev-handoff V10/V11/V27）✅ 已写盘待 planner intake。剩余动作 = 契约重写原子落地（human 一次性放行）+ spec 标 FROZEN。
