# Review Packet · 260923-arch-review-schedule-grid-cold-boot（adjudicator 收口）

> 存档注记：adjudicator 制品 Write 被环境工具层拦截，本文件由 review-moderator 代为落盘，内容为 adjudicator 回执全文逐字转存。

## 1. 对象与定性
- 被审对象：`candidate-spec-draft.md` v2（Shape A：三触发附带推 grid，`data.restore` restore 语义）。spec lane + contract-touching flag（契约 §1.3②）。
- charter：冻结 7 轴 + 6 场景（S1-S6）+ 8 canary。
- 变更类型：决策滞后补齐（轴 7 考据 CONFIRMED：疏漏非取舍，五条证据链含 K1 验收锚新证据；Shape C 不复活）。

## 2. 辩论轮次
r1 单轮收敛：challenger（BLOCKER 1 / MAJOR 4 / MINOR 10 / INFO 2 + DISMISSED 3 + 论证结论 2，canary 8 条独立重派）→ defender（REBUT 6 / CONCEDE 9 / 认同 7）→ moderator grep 复核（v1/v2 错位五条成立：F-C1/C6/C9/C10/C14）→ adjudicator 终裁（本 packet）。无续问轮必要：CONCEDE 各条修法方向已收敛、REBUT 各条 challenger 无再质空间、唯一分歧点 F-C5 按 charter §6.2 打包。

## 3. 终裁分布
CONFIRMED 9（F-C2/C3/C4/C7/C8/C12/C13 FIX-SPEC + F-C16/C17 记法要求）/ DISMISSED-RESOLVED 5（F-C1/C6/C9/C10/C14）/ DISMISSED 类 7（F-C11/C15/C18/C19/C20/C21）/ DEADLOCK 1（F-C5，拍板包交 human）。详表见 findings.md。

## 4. pass-oracle（spec profile，5 关 + handoff 出口）
1. **轴覆盖 GREEN**：charter 轴 1-7 全部被 challenger 走过（含排除轴与预埋 1a-1e 攻点）；falsifiable-first 守约。
2. **场景覆盖 GREEN**：S1-S6 逐一走查——S1（F-C3/F-C4）、S2（F-C11）、S3（F-C10/联动 F-C5）、S4（清白，F-C20）、S5（F-C22）、S6（F-C5）。
3. **finding 全结 GREEN（有挂账）**：22+2 全落 outcome；F-C5 = DEADLOCK 已打包交 human，冻结前须销。
4. **spec↔contract 机械门 GREEN（有保留，如实记录）**：`arch-spec-contract-check.mjs` 本仓不在场（ARQ-260827-contract-mech-gate-gap：原仓脚本绑 Prisma 多文件结构未迁，单文件 MD 契约面无脚本兜底）——降级人工核对：v2 契约改动草案（additive `data.restore` 钉 data 内 + modifying L85 板子行为句全文 + 生产纪律句去枚举）与 §2/§3 文本、契约 §1.3⑥ additive 先例、「时机不进契约」C7 边界一致性核过（F-C14 v2 对齐）；修复清单 R1-R10 落定后须复读一次。
5. **canaries 全中 GREEN**：challenger 独立重派 8 条 = 7 成立 + C7 命中（对 v1 文本）；C7 命中经 moderator grep 复核已由 v2 九项修正覆盖（F-C14 DISMISSED-RESOLVED）——**canary 终分 = 8/8、无未决命中**。
6. **handoff 出口 GREEN**：本轮 CONFIRMED 全为草案/spec 文本修复，无实现侧修复 finding → `dev-loop-handoff.json` **not required**；实现侧工作在 spec 冻结 + 契约门批后走 dev-loop 正常路由（§5 验收锚 + 修复项验收齿为 acceptance-author 输入）。

反空过自检 PASS：challenger 独立重派全部 canary（非复述）、开局打可证伪项（F-C1 BLOCKER 靠 front_app.py:242 中继链代码证据）、轴 7 独立考据给出三条草案未引的新证据——辩论不浅。

## 5. spec 可冻结判定
**现不可冻结。** 冻结前置条件（全满足方可标 FROZEN）：
1. F-C5 拍板包 human 裁定（deadlocks.md；裁设闸 → 修复清单增项；裁 ADR 记法 → 按 defender 方案落 + ARQ-260923-schedule-grid-restore-gate 销行或转 watchlist）；
2. 修复清单 R1-R10 回灌草案定稿（moderator/architect）；
3. **契约门批准 CCP**——契约未批 spec 不可冻结（DEV-GOVERNANCE L94 / protocol Spec↔Contract 铁律 3 / charter 冻结条款 6）；
4. 契约门批后 spec 与契约同 PR 原子落地（packet-bound landing + landing-verifier 独立核验）。

## 6. dev-loop-handoff 要点（供冻结后 acceptance-author 取材）
- **验收锚 7 条**（草案 §5 原样有效）：①server 推送帧序断言（home 先 grid 后、schedule.md 缺失/坏不阻断建连）②薄载荷展开恒不置 restore（机械齿）③firmware 缓存-only 不接管前台（三前台态层栈断言，基建在位 key_layer_dispatch.acceptance.c weak 哨兵先例）④语音接管回归守卫 ⑤restore 降级静默（不触发 SetChatMessage）⑥alert 轮询起轮不回归 ⑦每日/录入触发附带 + 不新增第二套进程内日账（F3）。
- **修复项对应验收齿增量**：R3（旧固件行为定案句）→ C1 closeout evidence 保留（旧固件行为实测记录 + 开关状态留痕 + 烧板验证开闸记录）；R4（ADR 部署句）→ 同上开关留痕消费；R5/R6（判据措辞）→ 无新齿（文档面）；R7（false 态）→ 可并入锚②断言面（absent/false 同判）；F-C5 若 human 裁设闸 → 新增 validate_display 拒 restore 键齿 + provenance 旁路齿；裁 ADR 记法 → 齿面维持现状、幻觉路径入残余风险 watchlist。
- **实现注意项（challenger C3 canary 注记，转 builder）**：restore 缓存写路径无 begin_page 隐式持锁，须显式持 LVGL 锁。

## 7. 契约门议程（CCP 待 human 批点清单，沿用 architect-reviewer 预审 §4 五条）
1. §1.3② 既有板子行为句 modifying（v2 §4 ① L85 旧句→新句全文，逐字审）；
2. 新增 `data.restore` bullet（additive 可选布尔，data 内，含降级双面注记——restore 板端静默 / front V27 入板前，两层互不替代）；
3. 生产纪律句（去三触发枚举口径：仅 front 后台附带推送路径可置、薄载荷展开恒不置；触发时机权威承载 = arch §5.2 / ADR-005 不进契约）；
4. 契约变更记录行（对齐 L87/L149 先例格式）；
5. Gate Scope / out-of-scope 清单（只动 §1.3②；确认不改 §1.3⑥/§1.5/§1.6/§2.3/§四）+ C1 类 closeout condition（ECS 推送开关默认关）。

## 8. 残余风险与冻结建议
- 残余风险：① F-C5 幻觉 restore 路径（若 human 裁 ADR 记法则以显式风险记录缓释，敞口 = 幻觉网格静默填缓存、下次三触发推送覆盖，低于既存 LLM 直产完整网格接管前台的洞）；② C1 若长期不闭合（设备不烧新固件），ECS 开关生命周期语义（永续配置 vs 一次性闸）未定——随 F-C5 拍板或烧板推进定案；③ 建连双推一过性不一致窗口（F-C20 已判留验收锚足够）。
- 冻结建议：条件冻结路径成立——本 review 无 BLOCKER 级未结项（F-C1 BLOCKER 经 v2 修正 DISMISSED-RESOLVED）；满足 §5 四前置后可标 FROZEN。
