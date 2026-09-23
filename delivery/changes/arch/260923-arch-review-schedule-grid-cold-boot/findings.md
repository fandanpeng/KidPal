# Adjudicator 终裁 · 260923-arch-review-schedule-grid-cold-boot（r1 辩论收口）

> adjudicator 终裁表。被审对象 = candidate-spec-draft.md v2；输入 = findings-challenger.md + findings-defender.md（r1）+ moderator grep 复核注记。2026-09-23。
> 裁定口径：defender CONCEDE → CONFIRMED；defender 引 v2 原文完整反驳且 challenger 无法再质 → DISMISSED；moderator 已核 v1/v2 错位 → DISMISSED-RESOLVED（被审文本 v2 已含修正）；满预算仍争且正反都有据 → DEADLOCK（拍板包交 human）。
> 存档注记：adjudicator 制品 Write 被环境工具层拦截，本文件由 review-moderator 代为落盘，内容为 adjudicator 回执全文逐字转存。

## 1. 终裁表（22 条 + 2 论证结论）

| # | 轴/场景 | 原判 | 终裁 | outcome | 裁定理由（一句，引文件:行号） |
|---|---|---|---|---|---|
| F-C1 | 轴1 | BLOCKER | **DISMISSED-RESOLVED** | NO-CHANGE | moderator grep 复核成立：被引「载荷顶层可选布尔 `restore`」（v1 §4:137）在 v2 零命中，v2:144/:154 已钉死 data 内——被审文本已含修正；challenger 对「message 顶层经中继链不可实现」（front_app.py:242 只装配 view/data 两键）的技术论证成立且恰为 v2 修正依据，留档备查。 |
| F-C2 | 轴1 | MAJOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE（收窄后成立）：v2 现存不一致在 §3.3:109（两触发）与 §3.2:105（括注含 marker 触发③）之间——requirements 句是 acceptance-author/builder 直接取材面，枚举不一致 = 落地即漂移；修法与 charter 轴 1 一致。 |
| F-C3 | 轴3/S1 | MAJOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE 且双方独立核同链：schedule_page.c:607-613 未知键 skip 容忍 + mcp_server.cc:620-644 parse 成功即 begin_page 接管——旧固件行为定案 = 确定性顶到 P2，草案 §6.2/风险②「两分支未定案」过时须改写；C1 closeout condition 保留（belt-and-suspenders）。 |
| F-C4 | 轴3 | MAJOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE：部署纪律现仅存 delivery 瞬态面（§4:163、§7:194），canonical 无承载；F-C3 定案确定性回归后更须承载——落点采 ADR-005 决策段补一句（与决策本体同生）。 |
| F-C5 | 轴1b/S6（升轴10候选） | MAJOR | **DEADLOCK** | 拍板包交 human（不裁死） | 事实面双方独立核验一致（front_app.py:511-514 只拦薄载荷；display_schema.py:87-105 shape floor 不拒 restore 键——本判官独立复核成立；验收齿 2 守展开路径不守幻觉路径）；处置分歧正反都有据且均不违 charter 硬约束（见 deadlocks.md），按 charter §6.2 打包交 human；判官推荐 = defender 第三条路（ADR-005 残余风险记录），建议即地板、可砍更大。 |
| F-C6 | 轴1c | MINOR | **DISMISSED-RESOLVED** | NO-CHANGE | moderator grep 复核成立：被引旧目录名落点（v1 §4:140）v2 零命中，v2:152/:156 均新名。 |
| F-C7 | 轴2 | MINOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE 且独立加硬：「跨连接会话」是决策 24（arch:414）没有的判据维度（实际判据 = 掉电边界），在役 s_grid_courses 恰跨 WS 连接存活（kidpal_render_node.cc:121-122）——澄清句自带第二层字面张力；修法对齐掉电边界。 |
| F-C8 | 轴2 | MINOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE，草案 §6.6:170 自认半拉子：NFR-OFF-1（requirements.md:594）是 AC 回引常客（:232/:299），arch L149 追踪表行同漏——既开澄清就一次到位。 |
| F-C9 | 轴1 | MINOR | **DISMISSED-RESOLVED** | NO-CHANGE | v2 §4 ①（:139-140）已给 L85 旧句→新句全文，「有承诺无文本」已消。 |
| F-C10 | 轴3/S3 | MINOR | **DISMISSED-RESOLVED** | NO-CHANGE | v2 §4 ②（:144）降级主语已钉「restore 载荷板端入板后」+「两层降级互不替代」，缺省路径在役 SetChatMessage 降级（mcp_server.cc:630-636）无被静默化的文本空间。 |
| F-C11 | 轴3/S2 | MINOR | **DISMISSED**（附编辑性括注） | NO-CHANGE + 编辑性括注 | defender 反驳成立：restore 恒不接管、不推 SERVER_PAGE 层的 spec 不变量已在 §2:30/:116 在案且与谓词取值无关地成立；谓词按引用导入 home on_screen 先例语义（kidpal_render_node.cc:472-477），两态皆无害（challenger 自认）——把两态皆无害的实现谓词升为 spec 条款即 ADR-001 教训要防的方向。采纳 defender 折中：ADR-005 关联段半句括注（R10），编辑加固、非 finding 义务。 |
| F-C12 | 轴3 | MINOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE（NIT 级）：v2:144 只给 absent/true 两态名，显式 `false` 严格读介于两句之间；契约不留严格读未定义缝是本仓惯例。 |
| F-C13 | 轴1 | MINOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE 无辩面：R-S0.2 在 docs/ 全域零定义（双方独立 grep 一致），canonical 引用 delivery 瞬态面标识符 = 机械可判纪律违例；与 F-C2 同句合并改写。 |
| F-C14 | 轴5/C7 | MINOR | **DISMISSED-RESOLVED** | NO-CHANGE | v2 §4 ③（:148）生产纪律句已去三触发枚举 + 删「见 review 风险段」死指针，与契约 §1.3⑥:147 边界同向；C7 canary 对 v2 不命中。 |
| F-C15 | 轴6 | INFO | **DISMISSED-WITH-GROUNDING** | NO-CHANGE | 七条验收锚逐条可机判、charter 预埋三攻点全有在役基建答案（key_layer_dispatch.acceptance.c:18-19 weak 哨兵先例等）；部署锚缺口归 F-C4。 |
| F-C16 | 轴4（论证结论） | — | **采纳为 ADR-005 记法要求** | FIX-SPEC（随文本落地） | 双方一致且论证互补：触发②对长在线设备具独立必要性（flags 日期敏感、①触达不了活跃/充电座长在线态）；触发③保留但理由如实记「不变式维护 + 产品直觉一致性、非独立必要性」（砍③须另立显式负向规则、维护成本反高于近零增量附带）。要求见 §3。 |
| F-C17 | 轴7（考据结论） | — | **采纳为 ADR-005 记法要求** | FIX-SPEC（随文本落地） | challenger 五条证据链（决策 26 原文自限范围 arch:416、K1 plan「producer 已推过」默认前提、M3K1-A5 structural 验收锚「三时点首页与课表页实际刷新」意图面写了实现面漏了、human K6 立项定性待修缺陷、M3-K2 BOOT 翻页读缓存）defender 无从反驳——疏漏定性 CONFIRMED，Shape C 不复活、推荐位表不作废；理由段补引 K1 验收锚。 |
| F-C18 | 轴5 | INFO | **DISMISSED-WITH-GROUNDING** | NO-CHANGE | canonical spec 惯例不列测试文件名（S-F1.2 既有 AC1-AC4 亦无），acceptance-author 从行为条款派生验收 = dev-loop 既有模式；断链不成立。 |
| F-C19 | 轴1d | DISMISSED | **DISMISSED** | NO-CHANGE | front_app.py:762-773 建连推送在每次 websocket 握手路径，重连 = 新握手 = 重推；与 §3.3 自洽。 |
| F-C20 | 轴3/S4 | DISMISSED | **DISMISSED** | NO-CHANGE | 缓存覆盖写幂等 + 两推独立 try/except 不阻断建连 + P2 在屏重绘视觉 no-op；留验收锚足够。 |
| F-C21 | 轴1e | DISMISSED | **DISMISSED** | NO-CHANGE | 补充规则是 AC 标准限定机制（先例 L271）；AC1 断网张力先于本草案存在，补充句显式承载后自洽。 |
| F-C22 | 轴5/S5 | MINOR | **CONFIRMED** | FIX-SPEC | defender CONCEDE（轻）：ADR-005 理由段（:117）只述本案、S5 同病实存（kidpal_render_node.cc:764「schedule 空态同款」）——表态句取「通用模式第二实例、逐案走各自 review、不自动套用不预扩字段」（双向封漂移）。 |

**终裁分布**：CONFIRMED 9（含 2 记法要求）；DISMISSED-RESOLVED 5；DISMISSED/DISMISSED-WITH-GROUNDING 7；DEADLOCK 1。全结，无未落 outcome 的 finding。

## 2. 修复清单（单一清单，R1–R10）

| # | 文件/位置 | 修法一句话 |
|---|---|---|
| R1（F-C2+F-C13 同句合并） | 草案 §3.3（requirements 追加句，:109） | 统一为三触发枚举（建连、每日 0:00、录入 marker，或两触发主枚举 + 显式句「录入 marker 同型附带，触发时机权威承载见 arch §5.2 / ADR-005」），同时去「（R-S0.2）」标签、以一句自足描述承载空态行为。 |
| R2（F-C2 连带） | 草案 §3.2（:105） | 头句「同两触发的附带推送」顺平为三触发口径一致表述。 |
| R3（F-C3） | 草案 §2 风险②（:42）+ §6.2（:184）+ §7（:194） | 按仓内代码定案改写为唯一分支「旧固件 = 确定性接管前台顶到 P2（引 schedule_page.c:607-613 + mcp_server.cc:620-644 为证）」，删「两分支未定案」；C1 closeout condition 原样保留。 |
| R4（F-C4） | 草案 §3.4 ADR-005 决策段末（:116） | 补部署时序句：固件先行 + 旧固件行为已由仓内代码定案为确定性接管（引证），builder 实测复核前 ECS 推送开关保持默认关、烧板验证后人工开闸。 |
| R5（F-C7） | 草案 §3.5（:125） | 判据对齐决策 24 掉电边界：「禁的是落非易失介质的持久层；板端 RAM 渲染缓冲——掉电即失——不禁」，删「跨连接会话」维度。 |
| R6（F-C8） | requirements.md NFR-OFF-1 行（:594）+ arch 追踪表行（:149）〔经 §3.1 修订总览增两行承载〕 | 各补半句「RAM 渲染缓冲不禁、掉电即失」注记，与 §3.5（R5 措辞）同 PR。 |
| R7（F-C12） | 草案 §4 ② 契约句（:144） | 「缺 `data.restore` 时设备行为完全不变」改「缺省（absent）或显式 `false` 时设备行为完全不变」。 |
| R8（F-C16/C17） | 草案 §3.4 ADR-005 理由段（:117） | 按 §3 记法要求改写：补引 K1 验收锚 + ②/③分开如实记。 |
| R9（F-C22） | 草案 §3.4 ADR-005 理由段末 | 加泛化立场句（「数据面更新 vs 前台接管分离」模式第二实例；后续逐案 review、不自动套用、不预扩字段）。 |
| R10（F-C11 编辑性括注，非必做） | 草案 §3.4 ADR-005 关联段（:119） | 半句括注：「在屏判定沿 home on_screen 先例口径；SERVER_PAGE 接管态下 restore 仅填缓存、不重绘。」（建议做） |

**级联自检**：① 枚举边界——R1/R2 只落 requirements/arch 侧，契约侧维持 v2 去枚举（F-C14 边界），两侧一致；② R5/R6 同用「掉电即失」单一判据；③ 部署纪律 canonical 归属唯一（ADR-005），契约侧无第二承载；④ R1 同句双修不留中间态；⑤ R8/R9 同段无互斥。

## 3. 采纳的 ADR-005 理由段记法要求（F-C16/C17）

1. 补引 K1 验收锚（`delivery/taskpackages/260921-m3-k1-*/taskpackage.json` structural「真机联网/零点/录入三时点**首页与课表页**实际刷新目击」）——意图面写了 grid 刷新、实现面漏了，为「疏漏非取舍」最强直接证据。
2. 触发②独立必要性（长在线设备 flags 日期敏感、①触达不了）与触发③「不变式维护 + 产品直觉一致性、非独立必要性」**分开如实记，不并列同等必要**。
3. 泛化立场（R9）。

## 4. F-C11 说明

defender REBUT 成立（不变量已在案、谓词按引用导入先例、两态皆无害）；维持 spec 层不加谓词条款；采纳编辑性括注（R10），非 finding 修复义务。
