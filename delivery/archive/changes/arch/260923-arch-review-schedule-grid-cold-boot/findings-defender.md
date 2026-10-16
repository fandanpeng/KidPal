# Defender 答辩 · 260923-arch-review-schedule-grid-cold-boot（r1，对抗 findings-challenger.md）

> defender（扮被审草案架构师）逐条回应 challenger r1。被审对象 = candidate-spec-draft.md **v2**（当前工作树文本）。所有引文逐字核验自仓内文件；行号标注 = 2026-09-23 工作树现状。
> 存档注记：defender 子代理 Write 被环境工具层拦截，本文件由 review-moderator 代为落盘，内容为 defender 回执全文逐字转存。
> **moderator 复核注记（2026-09-23）**：defender 的 v1/v2 错位提示经 moderator grep 直接复核成立——F-C1（`载荷顶层`）、F-C6（旧目录名落点）、F-C9（L85 改写文本）、F-C10（降级主语）、F-C14（三触发枚举）五条所引字符串在 v2 现文逐字零命中、且恰为 v2 版本行自列九项修正覆盖。按「已修结案（REBUT）」归类。

---

## 逐条裁定

### F-C1（restore 键位置矛盾 + 载荷顶层不可实现）· 裁定 = REBUT（v2 已修）

- **证据**：v2 §4 契约改动草案 ②（candidate-spec-draft.md:144）：「**`data.restore`**（可选布尔，additive——缺 `data.restore` 时设备行为完全不变）：`true` = 恢复性载荷……」；§4 字段位置段（:154）：「字段位置与校验缝：`data.restore` 钉死在 **data 内**（与 §1.3⑥ `data.upcoming` additive 先例同位，不扩顶层信封概念）」。被引的「载荷顶层可选布尔 `restore`」字符串在 v2 全文 grep 零命中。
- **理由**：challenger 指控的两半——位置矛盾与「message 顶层经中继链不可实现」——技术上都成立（front_app.py:241 `_display_wire_arguments` 确只装配 `{"view": ..., "data": json.dumps(...)}` 两键，顶层键装配即剥除），而 v2 的修正正是按这个结论把字段钉死在 data 内。针对当前 v2，矛盾不存在。
- **修法方向**：无（已修）。

### F-C2（触发③枚举三处不一致）· 裁定 = CONCEDE（收窄后成立）

- **证据**：v2 现存的不一致在 §3.3 与 §3.2 之间：requirements 追加句（candidate-spec-draft.md:109）：「冷启动恢复 = WS 建连与每日 0:00 跨天时云端附带重推周网格……」——无 marker；arch 追加文本（:105）：「……（**录入 marker 触发③同型附带**）」。challenger 引的第三处（§4 生产纪律句三触发枚举）在 v2 已删（见 F-C14），该分叉不存。
- **理由**：requirements 句是 acceptance-author / builder 的直接取材面，与 arch 文本枚举不一致 = 落地即漂移，真缺口。§3.2 头句「同两触发的附带推送」+ 括注塞第三触发的措辞也别扭，认。
- **修法方向**：§3.3 句统一含 marker 或显式补「录入 marker 同型附带（触发时机权威承载见 arch §5.2 / ADR-005）」，并顺平 §3.2 头句。

### F-C3（旧固件行为可由仓内代码定案 = 确定性接管）· 裁定 = CONCEDE

- **证据**：独立复核成立。schedule_page.c:607-612（`kidpal_schedule_grid_parse` 未知键分支）：「未知顶层字段值畸形 -> -1（否则忽略扩展位）」+ `if (!kspj_skip_value(&c)) { return -1; }`——restore 作为 data 内未知键走 skip 容忍；mcp_server.cc:620-644：parse 成功（n≥0）→ `kidpal_render_node_begin_page()` + `kidpal_schedule_grid_render` + `kidpal_render_node_cache_schedule_grid`——**接管前台推 SERVER_PAGE**。restore 载荷 data = `{week:{...}, restore:true}`，旧固件解析 week、跳过 restore、parse 成功 → 确定性顶到 P2。
- **理由**：「未定案」确属伪命题，草案 §6.2/风险②的分支枚举表述应按代码定案改写为唯一分支（确定性接管）；这使部署纪律从「预防性谨慎」升格为「防确定性回归」，反而**加重**了草案自身 C1 开关纪律的正当性——C1 closeout condition 保留不动。concede 不伤方案本体，伤的是风险段表述精度。
- **修法方向**：§6.2/风险②按「确定性接管（引 schedule_page.c:607-613 + mcp_server.cc:637-644 为证）」改写；C1 closeout condition 原样保留作 belt-and-suspenders。

### F-C4（部署顺序纪律 canonical 承载无主）· 裁定 = CONCEDE

- **证据**：ADR-005 全文草案（candidate-spec-draft.md:113-119）决策段与备选段确实无部署时序句；C1 纪律现仅存于 §4 CCP 骨架（:163，delivery 面）与 §7（:194，自称「非本草案义务面」）。
- **理由**：辩护过一次后放弃——我原可辩「closeout condition 挂 CCP 是 gate evidence 的既定机制、spec 描述稳态不描述迁移」，但 F-C3 定案后 S1 = **确定性**回归级事故，且 ADR-005 是 canonical、会活过本轮 review 归档；未来读者（integrator / 运维）从 ADR 单独进入时无从得知「固件先行、ECS 开关默认关」。一句话成本，认。
- **修法方向**：ADR-005 决策段末补一句部署时序（固件先行 + 旧固件行为定案前 ECS 推送开关保持默认关，引 C1），倾向 challenger 给的 ADR-005 落点（与决策本体同生）。

### F-C5（LLM 幻觉 restore 直达设备，运行时闸真空）· 裁定 = CONCEDE（降档：拒 ESCALATE、拒两个机械修法候选，采第三条路 = ADR-005 显式记残余风险）

- **证据（先核事实，challenger 的事实面全部成立）**：
  - 截获层只拦薄载荷：front_app.py:514 `if not isinstance(payload, dict) or payload.get("view") != "schedule": return None` + :511 注释「恰薄载荷才展开、其余 None」——幻觉直产的完整网格非薄载荷、不截获。
  - schema 层是 shape floor：display_schema.py:89-90 注释逐字「网格由 front 机械计算（schedule.py 单源）——**此校验只把关 shape floor，不作逐字段深度类型校验**」；:91-105 只查 `{}` 或 `{week:{courses:[{day,slot,name,flags}]}}` 下限，不拒 data 内多余键 `restore`。
  - 机械验收齿守错面：§5.2 齿断言对象 = `expand_schedule_intent_payload` 输出，与 LLM 直产路径无关。**草案 §6.1 的答辩「验收齿 2 即机械齿」对幻觉路径确实不成立，这半句我收回。**
- **理由（为什么拒 ESCALATE 与两个机械修法）**：
  1. **这是既存姿态，不是 restore 新开的洞**。Q5 收紧 = M3-K1 契约门裁定（display_schema.py:87-90 注释自陈），shape floor 是**有意**设计：LLM 幻觉直产**不带 restore 的完整周网格**今天就能过 floor 中继到设备并**接管前台**——危害严格大于 restore 变体（只填缓存、不接管）。restore 没有增加危害等级，反而提供了一个更低危害的变体。单独给 restore 键上运行时闸而对更重的既存洞不设防 = 不对称的安全表演。
  2. **与既有生产纪律的执行地位对称**。契约 §1.3② 既有 producer 纪律「front 机械计算……不由 LLM 生成」同样没有运行时闸——`validate_display` 按裁定不区分 provenance，机械上分不出 front 算的网格和 LLM 幻觉的网格。要求新纪律句「谁可置 restore」自带运行时闸，是超出既有契约纪律执行标准的更高要求。
  3. **两个最小修法候选对架构的伤害都大于收益**：① `validate_display` 机械拒绝 restore + provenance 旁路——实测 front 自产推送路径也过同一道门（front_app.py:768 建连推送走 `_relay_schedule_payload` → :298 `build_display_tool_calls([block])[0]` → 其 docstring :248 自陈经 `validate_display`），拒绝 restore 键会把 front 自己的 restore 推送一并拒掉，必须在**冻结的中继链入口**（front_app.py:533 自称「中继链的冻结入口」）上穿 provenance 标记——为一个低于既存危害的变体动冻结面，不值；② front 保留前缀键——等价于把 schedule data 校验从 floor 升为 ceiling，恰好侵蚀 §1.3⑥ `data.upcoming` 先例确立的 additive 演进路径（未来 additive 字段会被 front 在设备之前拒掉），与 C6 兼容纪律反向。
  4. **主缓解在位**：system prompt 薄载荷约束（契约 §1.6）+ ADR-004 单源纪律；残余风险敞口 = 幻觉网格被静默填入 P2 缓存、孩子翻页看到幻觉课表、不接管前台、下次三触发推送即被真数据覆盖。
- **修法方向（第三条路）**：ADR-005 增一段显式风险记录：「LLM 绕过薄载荷约束直产带 `data.restore` 的完整网格理论上可过 shape floor 达设备、静默填缓存——与既存的 LLM 直产完整网格直达路径〔危害更高：接管前台〕同类且更低；根因缓解 = system prompt 薄载荷约束 + ADR-004 单源；不单独为 restore 键设运行时闸（与 §1.3② 既有 producer 纪律的执行地位对称；单独设闸对更重的既存洞不设防）。如未来整体收紧 LLM 直产 display 块面（provenance 旁路 + floor 收紧），restore 拒键随该收口一并做。」——同时把「三道拦截」的高估表述从草案 §6.1 答辩面撤掉，换成这句诚实记录。是否接受此第三条路 vs challenger 的机械闸，**正反都有据，标 deadlock 候选交 adjudicator**。

### F-C6（CCP 落点目录名残留旧名）· 裁定 = REBUT（v2 已修）

- **证据**：v2 §4（candidate-spec-draft.md:152）：「见 [CONTRACT-CHANGE-PACKET.md](../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md)」；:156「落 `delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/`」——均新名。旧名 `arch-review-260923` 在 v2 全文仅存于 :6 改名注记（记录原名本身，非落点）。被引的 :140 旧路径在 v2 grep 零命中。
- **理由**：v2 版本行（:5）九项修正自列「CCP 落点改新目录」。对当前 v2 不成立。
- **修法方向**：无（已修）。

### F-C7（「跨连接会话」判据措辞与决策 24 实际判据不齐）· 裁定 = CONCEDE

- **证据**：§3.5 拟追加文本（candidate-spec-draft.md:125）：「（「不缓存」禁的是跨连接会话的持久层缓存与非易失介质；板端 RAM 渲染缓冲——P2 网格缓存、P1 seam——不禁，掉电即失、离线不补，见 §5.4 板端状态行。）」。决策 24 实际判据（arch L414）= 掉电边界：「一切业务状态只驻 RAM、掉电即失」。
- **理由（独立判断，非复述 challenger）**：这条我攻不动，而且认为它比 challenger 自评的更硬：问题不止措辞歧义——「跨连接会话」引入了一个决策 24 **根本没有的判据维度（连接边界）**，而在役 `s_grid_courses` 恰恰跨 WS 连接存活（3 分钟闲置断连重连不清缓存，S4 场景自己依赖这一点）。拟落 canonical 的澄清句里放一个与在役现实相反的判据，是主动制造第二层字面张力——正是 §3.5 想消解的那种。澄清句的唯一使命是钉准判据，判据钉错了等于白澄清。
- **修法方向**：采 challenger 措辞方向——判据对齐决策 24 的掉电边界：「禁的是落非易失介质的持久层；板端 RAM 渲染缓冲——掉电即失——不禁」，删「跨连接会话」判据短语。

### F-C8（NFR-OFF-1 半拉子）· 裁定 = CONCEDE

- **证据**：草案 §6.6（candidate-spec-draft.md:188）自认：「本草案只改 arch 侧，requirements 侧 NFR 行零改动，可能被攻半拉子」。NFR-OFF-1 是验收 AC 回引常客（S-F1.1 AC4 / S-F1.4 AC5，challenger 引 requirements.md:232/:299）。
- **理由**：草案既然主动开 §3.5 澄清（且我方在 §3.5 上已按 F-C7 认了「要澄清就钉准」），半边澄清留下 requirements 读者拿 NFR-OFF-1 字面对照在役 grid 缓存得「违规」结论的口子，是自造的不一致。认。
- **修法方向**：NFR-OFF-1 行 + arch L149 追踪表行各补半句「RAM 渲染缓冲不禁、掉电即失」注记，与 §3.5（按 F-C7 修正后的措辞）同 PR。

### F-C9（L85 改写有承诺无文本）· 裁定 = REBUT（v2 已修）

- **证据**：v2 §4 ①（candidate-spec-draft.md:139-140）给出旧句与新句全文：旧句「板子行为：切到 P2 课表页渲染一周总览网格（学校课逐节、课外课逐门小字竖排、※/星标按 flags 渲染、今天列整列高亮）；薄载荷不直达设备（front 截获展开）；畸形/旧形状 data 由 front 机械校验拒绝并降级提示，不入板盲渲染（V27）。」→ 新句同文插「；`data.restore: true` 时除外——只填网格缓存、不接管前台，P2 在屏时仅重绘本页；」。
- **理由**：v2 版本行（:5）九项修正自列「L85 板子行为句逐字改写」。契约门有文可审。
- **修法方向**：无（已修）。

### F-C10（降级条款作用域歧义）· 裁定 = REBUT（v2 已修）

- **证据**：v2 §4 ②（candidate-spec-draft.md:144）：「降级双面注记：**restore 载荷板端入板后**解析失败 = 板端静默降级（记日志、缓存零变化、不弹提示、不动前台）；**front 校验面 V27**（脏块拒绝/降级提示、不入板盲渲染）在入板前独立生效——**两层降级互不替代**。」ADR-005 决策段（:116）同口径：「恢复性载荷板端入板后解析失败降级 = 静默记日志……（区别于主动展示路径的 SetChatMessage 降级……）」。
- **理由**：降级条款的主语在 v2 已显式钉为「restore 载荷 / 恢复性载荷」且「板端入板后」限定作用层，「两层降级互不替代」显式保住了缺省路径的 V27 在役降级（mcp_server.cc:628-631 SetChatMessage 冻结行为）——challenger 担心的「缺省路径降级被静默化」在 v2 文本上没有立足空间。被引的 §4:137 无主语句是 v1 文本。剩余分歧只在「restore 载荷」vs「`data.restore: true` 载荷」的措辞偏好，属风格不属缺口。
- **修法方向**：无（已修；adjudicator 若偏好更显式限定语，属编辑性采纳，非 finding 处置）。

### F-C11（P2 在屏谓词未定义）· 裁定 = REBUT

- **证据**：spec 层不变量已钉：草案 §2（candidate-spec-draft.md:30）：「设备收到 restore 载荷 → 解析、**只填 RAM 网格缓存、不推 SERVER_PAGE 层、不接管前台**，若 P2 恰在屏则仅重绘 P2 本页」；契约句（:140/:144）同口径。ADR-005 决策段（:116）显式指名同构先例：「（同构先例 = home view `data.upcoming` 注入 seam 的「不抢前台」语义，mcp_server.cc home 路由）」——而 home 先例的谓词定义就是 kidpal_render_node.cc:472-478 的 LOCAL_PAGE 门（`kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE && s_local_page == KP_PAGE_HOME`），ADR 引先例即按引用导入了谓词语义。
- **理由（独立判断，正面攻 challenger 自评最弱条）**：charter S2 真正问的是「前台仲裁语义是否有 spec 条款」——有：**restore 恒不接管、不推 SERVER_PAGE 层**，这条不变量与谓词取值无关地成立。剩下的分歧只是「接管态下要不要多刷一次本页」，两个取值都无契约可观察偏差：接管态下在屏的 P2 本就渲染着接管载荷展开的新鲜网格（薄载荷展开 = 当前数据），restore 不重绘不留任何旧数据窗口；即便某实现选择在接管态也重绘，也只是无害冗余。challenger 自己承认「两态皆无害」。把一个两态皆无害、且已被 ADR 按引用导入先例语义的查询谓词升为 spec 条款，是把实现细节抬进 spec——恰是 ADR-001 教训（断言耦合实现细节）要防的方向。谓词实现（含 C3 canary 注记的 LVGL 锁）归 builder，验收锚 ③ 已覆盖行为面。
- **修法方向**：无。若 adjudicator 仍求零风险，可在 ADR-005 关联段加半句括注（「在屏判定沿 home on_screen 先例口径；SERVER_PAGE 接管态下 restore 仅填缓存、不重绘」）——属编辑性加固，不认缺口。

### F-C12（`restore: false` 显式置位态未定义）· 裁定 = CONCEDE（NIT 级）

- **证据**：v2 契约句（candidate-spec-draft.md:144）：「（可选布尔，additive——**缺 `data.restore` 时**设备行为完全不变）：`true` = 恢复性载荷……」——absent 与 true 两态有名，显式 false 无名。
- **理由**：布尔三值二态确实留了一个字面缝：false 既非「缺」也非「true」，严格读介于两句之间。实际无调用方会显式置 false、语义必然归并缺省，但契约文本不留「严格读未定义」的缝是本仓惯例。认，改动两个词。
- **修法方向**：句改「缺省（absent）或显式 `false` 时设备行为完全不变」。

### F-C13（R-S0.2 悬空引用）· 裁定 = CONCEDE

- **证据**：本方独立 `grep -rn "R-S0" docs/`（含 docs/archive/）= 零命中，与 challenger 一致；§3.3 追加句（candidate-spec-draft.md:109）确含「（R-S0.2）」。
- **理由**：canonical 不得引用 delivery 瞬态面标识符，机械可判的纪律违例，无辩面。
- **修法方向**：追加句去标签、以一句自足描述承载空态行为（如「显示友好空态（『还没有课表数据，问我「我的课表」吧』）」）。

### F-C14（三触发枚举入契约撞 C7 边界）· 裁定 = REBUT（v2 已修）

- **证据**：v2 §4 ③生产纪律句（candidate-spec-draft.md:148）：「**生产纪律（谁可置 `data.restore`）**：仅 front 后台附带推送路径可置 `restore`（**触发时机属行为策略，权威承载 = arch §5.2 / ADR-005，不进契约**）；意图薄载荷 `{view:schedule,data:{}}` 的展开（`expand_schedule_intent_payload`）恒不置——展开产物 = 主动展示、接管前台（M3-K3 冻结行为）。」——无触发枚举、无「见 review 风险段」死指针。
- **理由**：v2 版本行（:5）九项修正自列「生产纪律句去三触发枚举」。现句与契约 §1.3⑥ L147 边界同向（producer 路径纪律可进契约、时机不进），与既有 §1.3②:83 producer 出处纪律先例同构。C7 canary 对 v2 不命中。
- **修法方向**：无（已修）。

### F-C15（验收锚 INFO）· 认同 challenger 裁定

- 七条可机判 + 预埋三攻点全有基建答案，无独立 finding；部署锚缺口归 F-C4（已认）。无异议。

### F-C16（轴 4：触发②成立；触发③诚实定性）· 认同 + 明确表态

- **②**：认同 challenger 独立论证，且它补强了草案自辩——「闲置 3 分钟休眠」只覆盖闲置态，活跃/充电座长在线设备是触发①触达不了的盲区，而 grid flags（bit0 过期忽略、bit1 星标随当日）是日期敏感计算，跨零点必 stale。②是长在线设备的唯一刷新时机，独立必要性成立。
- **③**：**同意 challenger 结论：保留③，但理由须如实记为「不变式维护 + 产品直觉一致性」，不与②并列为同等必要**。我补一条辩护方自己的论证：砍③不是省事——它要求显式立一条负向规则（「录入 marker 推送点附带 upcoming 而显式不附带 grid」），这条负向规则本身就是 spec 文本 + 一条验收齿 + 一个未来读者必问必答的解释条款；而 piggyback 附带在现役单点（front_app.py:557-578 同一循环）上是近零增量。**砍③的维护成本反而高于留③**，这与 challenger 的「两附带一不附带异例要另立解释条款」是同一论证的两面。
- **ADR-005 理由段如实记法建议**：「触发③（录入 marker）附带的直接价值窗口较窄（仅覆盖录入瞬间在线且此后持续在线不重连的设备——闲置休眠设备由重连触发①兜底），保留它主要基于三触发同源同附带的不变式维护成本（砍除需另立显式负向规则）与产品直觉一致性（录入新课后 upcoming 与 grid 同帧更新），非独立必要性；触发②对长在线设备具独立必要性（flags 日期敏感，①无法触达长在线态）。」

### F-C17（轴 7：疏漏定性 CONFIRMED）· 认同 challenger 裁定

- 五条证据链（尤其 K1 taskpackage structural 验收文本「真机联网/零点/录入三时点**首页与课表页**实际刷新目击」——意图面写了、实现面漏了，这是疏漏定性的最强直接证据）我方无从反驳，反而建议 ADR-005 理由段补引此锚，使「疏漏非取舍」的考据强度高于草案自引的时序证据。Shape C 维持否决，推荐位表不作废。

### F-C18 / F-C19 / F-C20 / F-C21 · 认同 challenger 裁定（DISMISSED）

- 验收面承载惯例、断线重连 = 触发①、重连风暴幂等、AC1 断网自洽——四条 challenger 的 grounding 与我方对草案的独立核验一致，无补充辩护。
- 连带对表：轴 6 / 轴 7 清白、S4 清白、canary 7+1 记分——均认同。

### F-C22（restore 模式泛化性无表态）· 裁定 = CONCEDE（轻）

- **证据**：ADR-005 理由段（candidate-spec-draft.md:117）只述 schedule 本案 + home 先例同构，无泛化立场句；S5 同病实存（kidpal_render_node.cc:764 故事缓存空态「schedule 空态同款」）。
- **理由**：一句话成本换掉 S5 复发时的重新全流程对抗（或无纪律的字段繁殖），划算；且表态句可以同时封住两个方向的漂移——既防「每类数据各造一个新 additive 字段」的契约面膨胀，也防「援引 ADR-005 先例跳过评审」的纪律侵蚀。
- **修法方向**：ADR-005 理由段末加一句：「本机制 = 「数据面更新 vs 前台接管分离」模式的第二个实例（home `data.upcoming` 为首个）；后续同类冷缓存需求（如 P3 story 列表）逐案走各自 review，不自动套用、不预扩字段。」

---

## 答辩汇总

| 裁定 | 条目 | 计数 |
|---|---|---|
| REBUT（v2 已修 / 指控对 v2 不成立） | F-C1、F-C6、F-C9、F-C10、F-C11、F-C14 | 6 |
| CONCEDE | F-C2（收窄）、F-C3、F-C4、F-C5（降档）、F-C7、F-C8、F-C12（NIT）、F-C13、F-C22（轻） | 9 |
| 认同 challenger 裁定（无独立辩护） | F-C15、F-C16、F-C17、F-C18、F-C19、F-C20、F-C21 | 7 |

**v1/v2 错位提示（给 adjudicator）**：F-C1/C6/C9/C10/C14 五条引用的字符串在当前 v2 中 grep 零命中、且恰为 v2 版本行自列九项修正所覆盖——建议按「已修结案（REBUT）」归类，勿再为它们开 FIX 项。（moderator 已 grep 复核确认，见头部注记。）

**deadlock 候选（仅一处）**：F-C5 的处置选择——challenger 的机械闸路线（validate_display 拒 restore 键 + provenance 旁路）vs 我方第三条路（ADR-005 显式记残余风险 + 不单独设闸，留待 LLM 直产 display 面整体收口时一并做）。正方有据（运行时闸把生产纪律从 prose 升为可执行）；反方有据（既存更重的洞不设防、与既有 producer 纪律执行地位对称、动冻结中继链的代价）。建议 adjudicator 按 charter §6.2 打包正反论点 + 利害 + 代价交拍板；其余 CONCEDE 各条修法方向均已收敛、无僵点。

**我反对的 challenger 处置建议清单**：
1. F-C5 处置「ESCALATE 候选 / FIX-CONTRACT（validate_display 机械拒绝 + provenance 旁路 / front 保留前缀键）」——整体反对：降档为 ADR-005 残余风险记录（理由见 F-C5 段四点）；其中「保留前缀键」候选另犯 additive 演进路径倒退（与 C6 反向）。
2. F-C11 处置「ADR-005 钉谓词一句」——反对作为 finding 处置；至多编辑性括注（语义已按引用导入 home 先例）。
3. F-C16 ③的 ADR 记法——反对将③与②并列记为同等必要（challenger 自己也已此意，此条实为共同立场）。
