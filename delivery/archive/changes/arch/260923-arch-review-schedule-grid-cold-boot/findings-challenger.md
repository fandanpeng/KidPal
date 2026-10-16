# Challenger Findings · 260923-arch-review-schedule-grid-cold-boot（r1）

> challenger 红队 findings，按冻结 charter 轴序（falsifiable-first）。被审对象 = candidate-spec-draft.md v2（2026-09-23）。所有引文逐字核对自仓内文件，行号为 2026-09-23 工作树现状。
> 结论速览：**BLOCKER 1 / MAJOR 4 / MINOR 10 / INFO 2**；canary 8 条独立重派 = 7 成立 + 1 命中（C7）；轴 4 / 轴 7 给独立论证结论；场景 S1-S6 逐一走查（S1/S2/S3/S5/S6 有 finding，S4 清白）。
> 存档注记：challenger 子代理 Write 被环境工具层拦截两次，本文件由 review-moderator 代为落盘，内容为 challenger 回执全文逐字转存。

## Findings

### F-C1 · 轴1 · BLOCKER · SPEC_INTERNAL —— restore 键位置矛盾在 v2 残留于 §4 契约改动草案，且「载荷顶层」位置经在役中继链不可实现

- **可证伪断言**：草案 §4 契约改动草案逐字文本把 restore 放在「载荷顶层」，与草案其余全部钉死的 `data.restore`（data 内）直接矛盾；且 message 顶层位置在现有 front 中继下**物理到不了设备**。
- **证据**：
  - §4（candidate-spec-draft.md:137）：「载荷顶层可选布尔 `restore`（additive，缺省/absent = 现行为…）」；§4:138「生产纪律（谁可置 `restore`）」同用裸键名。
  - 反面钉死位：§2:30「携带 data 内新可选布尔 `data.restore: true`…**字段位置钉死在 data 内**」；§3.2:105「契约 §1.3② `data.restore: true` 语义」；§3.4:116「增 data 内可选布尔 `data.restore`」；firmware 段:38「`mcp_server.cc` schedule 路由读 `data.restore`」。
  - 不可实现证据：`server/gateway/front/front_app.py:242` `_display_wire_arguments` 返回 `{"view": payload.get("view"), "data": json.dumps(payload.get("data"), ensure_ascii=False)}`——中继 wire 只转发 view/data 两键，message 顶层 restore 键在装配时即被剥除；firmware 侧 `mcp_server.cc` 路由也只解析 arguments 里的 view 与 data 字符串。
- **影响**：§4 是宣称「逐字、可直接搬进 canonical」的契约文本。若照抄落地：设备永远读不到顶层 restore（全部 restore 载荷按接管行为执行 = 回归级），且契约 wire 形状与 §1.3② JSON 形状图、firmware 解析路径三方漂移。charter 预埋 1a 判：v2 的修正只落了 §2/§3，§4 未同步——**未修掉**。
- **处置**：FIX-SPEC（§4 两句改写为 data 内 `data.restore`，与 §2/§3 对齐）。

### F-C2 · 轴1 · MAJOR · SPEC_INTERNAL —— 触发③枚举在三份将落 canonical 的追加文本间不一致

- **可证伪断言**：requirements 追加句（§3.3）只含两触发，arch 追加文本（§3.2）与契约生产纪律句（§4）含三触发，同一机制三处口径不一。
- **证据**：
  - §3.3:109：「冷启动恢复 = WS 建连与每日 0:00 跨天时云端附带重推周网格…」——无录入 marker。
  - §3.2:105：「WS 建连与每日 0:00 跨天时…附带重推周网格恢复性载荷…（**录入 marker 触发③同型附带**）」。
  - §4:138：「仅 front 自身的**三触发**推送路径（建连 / 每日跨天 / 录入 marker）可置 `true`」。
  - §5.7:159 验收锚亦含「每日/录入触发附带」。
- **影响**：requirements 单独读者（acceptance-author / builder）将实现两触发版；与契约「三触发可置」形成落地即漂移。§3.2 头句「同两触发的附带推送」自身措辞也别扭（括注又塞第三触发）。
- **处置**：FIX-SPEC（三处统一枚举；requirements 句显式含 marker 或显式说明「录入 marker 同型附带，见 arch §5.2」）。

### F-C3 · 轴3/S1 · MAJOR · EVIDENCE_GAP —— 「旧固件对 restore 载荷行为未定案」可由仓内代码定案：确定性接管前台，非两分支皆可能

- **可证伪断言**：草案 §6.2/风险②称「旧固件忽略未知键 vs 畸形降级两分支未定案、需 builder 实测定案」——此不确定性是伪命题，在役固件解析器代码给出唯一答案。
- **证据**：
  - `firmware/main/display/schedule_page.c:609-613`（`kidpal_schedule_grid_parse` 未知顶层键分支）：「未知顶层字段值畸形 -> -1（否则忽略扩展位）」+ `if (!kspj_skip_value(&c)) return -1;`——未知键走 skip 容忍。
  - `schedule_page.c:273-278`（`kspj_skip_value`）：`strncmp(&c->s[c->pos], "true", 4) == 0) { c->pos += 4; return 1; }`——布尔值显式可跳。
  - `firmware/main/mcp_server.cc:620-656`：parse 成功（n≥0）→ `app.Schedule` 内 `kidpal_render_node_begin_page()` + `kidpal_schedule_grid_render` + `cache_schedule_grid`——**接管前台推 SERVER_PAGE**。
  - 在役固件 = K6 轮烧录的 xiaozhi.bin @ 91bcb08（m3-schedule progress.md:255），含此解析器。
- **影响**：S1 失败模式定案为唯一分支——server 先行 + 旧固件在场 = **每次建连被确定性顶到 P2**（非「可能弹错误提示」）。①草案风险段的分支枚举表述过时；②部署顺序纪律从「预防性谨慎」升格为「防确定性回归级事故」，直接加重 F-C4 的 canonical 无主问题；③gate evidence C1 closeout condition 仍有效，但其「实测定案」前提已在案头定案。
- **处置**：FIX-SPEC（草案 §6.2/风险②按定案分支改写；closeout condition 保留）。

### F-C4 · 轴3 · MAJOR · DESIGN_GAP —— 部署顺序纪律 canonical 承载无主（charter 轴 3 预埋判成立）

- **可证伪断言**：「固件先行、ECS 推送后行、开关默认关」这条 deployment-facing 不变量在草案里无 canonical 归属——不进 spec、不进契约、§7 自称「非本草案义务面」。
- **证据**：§7:174-178（「供后续 handoff / 实现包取材，非本草案义务面」）；§4:138 契约句只写「升级顺序纪律见 review 风险段，不进契约」；§3.4 ADR-005 决策段（:116）与理由/备选段（:117-118）均无部署时序句；arch §5.3 部署视图（architecture.md:217 起）无对应承载。review 风险段 = `delivery/changes/**` 瞬态证据面，随归档失去活性。
- **影响**：轴 9 失败模式（S1，F-C3 已定案为确定性回归）无设计响应的权威承载；canonical 读者（下一轮 spec 维护者、dev-loop integrator）无从得知该纪律。charter 预埋轴 3 末条判「FIX-SPEC 候选」——成立。
- **处置**：FIX-SPEC（ADR-005 决策句补部署时序一句，或 arch §5.3 部署视图补注；二选一，倾向 ADR-005——与决策本体同生）。

### F-C5 · 轴1b/S6（升轴10候选）· MAJOR · DESIGN_GAP —— restore 生产纪律运行时闸真空，且草案的「三道拦截」防御层数被高估：机械验收齿守错了面

- **可证伪断言**：S6 场景（LLM 幻觉产带 restore 的完整网格）在现役校验面下**可直达设备**，草案自列关注点 1 的答辩「验收齿 2 即机械齿」守的是展开路径、不守幻觉路径。
- **证据**：
  - 截获层不拦：`front_app.py:511-514`（`_expand_schedule_intent`）只对 `view=="schedule"` 载荷查 `expand_schedule_intent_payload`——恰薄载荷才截获；LLM 幻觉直产的 `{"view":"schedule","data":{"week":{...},"restore":true}}` 非薄载荷、不截获。
  - schema 层不拦：`server/gateway/display_schema.py:87-107`（`validate_display` schedule 分支）只查 `week` dict + `courses` list + 条目四键——形状下限、不查多余键；data 内 `restore: true` 畅通。MAX_DATA_BYTES=65536（:39），3-4KB 载荷远低于上限。
  - 验收齿 2（草案 §5.2:154）断言对象 = `expand_schedule_intent_payload` 输出——即 front 自己的展开函数，与 LLM 直产 display 块路径无关。
- **影响**：restore=true 的「谁可置」纪律实际运行时面 = 无闸（仅 system prompt 软约束 + 守错路径的验收齿）。幻觉后果 = 设备静默填入未经 schedule.md 校验的幻觉网格（不接管前台，危害低于接管但违背 ADR-004 单源纪律的板端呈现面）。charter 排除轴注记「challenger 若发现校验面真缺口（轴 1b）可升级为轴 10 finding」——此即真缺口。
- **处置**：ESCALATE 候选 / FIX-CONTRACT——最小修法候选：`validate_display` 对 schedule 载荷 data 内 `restore` 键机械拒绝，front 自产 restore 推送以 provenance 旁路（front 自产路径 `build_display_tool_calls` 前打内部标记）或改用 front 保留前缀键；至少 spec/ADR 须显式回应「LLM 直产 restore 块」这一路径，不能以「薄载荷由 front 截获」概括。

### F-C6 · 轴1c · MINOR · SPEC_INTERNAL —— CCP 落点目录名残留旧名（v2 修了 §3.4、漏了 §4）

- **可证伪断言**：§4 CCP 骨架写落 `delivery/changes/arch/arch-review-260923-schedule-grid-cold-boot/`，与实际目录名（`260923-arch-review-schedule-grid-cold-boot`，草案 :6 自注已改名）不符。
- **证据**：candidate-spec-draft.md:140 vs :6（「本目录现名 `260923-arch-review-schedule-grid-cold-boot`（原 `arch-review-260923-schedule-grid-cold-boot`）」）。§3.4:119 关联行 v2 已改正、:140 未改。
- **影响**：CCP 落错目录名 = scope-map 写槽 glob 不命中，机械层拒收。
- **处置**：FIX-SPEC（:140 路径改正）。

### F-C7 · 轴2 · MINOR · SPEC_INTERNAL —— §3.5 澄清半句的判据措辞与决策 24 实际判据不齐，「跨连接会话」读法会误伤在役缓存

- **可证伪断言**：§3.5 拟追加文本「『不缓存』禁的是跨连接会话的持久层缓存与非易失介质」的判据短语含混：在役 `s_grid_courses` 恰恰**跨 WS 连接会话存活**（重连风暴不清缓存，S4 场景自身依赖此点），按「跨连接会话（即禁）」的歧义读法即自相违规。
- **证据**：§3.5:125 原文；决策 24（architecture.md:414）实际判据 = 「只能驻留 RAM、**掉电即失**、禁落任何非易失介质」——判据是掉电边界不是连接边界；`kidpal_render_node.cc:121-122`（`s_grid_courses`/`s_grid_n` 静态存储、掉电前不清）。
- **影响**：拟落 canonical 的澄清句自身埋下第二层字面张力——正是 §3.5 想消解的那种。
- **处置**：FIX-SPEC（判据短语对齐决策 24：改为「禁的是落非易失介质的持久层；板端 RAM 渲染缓冲——掉电即失——不禁」）。

### F-C8 · 轴2 · MINOR · SPEC_INTERNAL —— NFR-OFF-1 半拉子确认：requirements 侧「不缓存」措辞两处零改动，且是验收回引的常客

- **可证伪断言**：§3.5 只改 arch §6.2；requirements NFR-OFF-1 行（L594「设备不缓存任何事项」）与 arch 追踪表行（L149「设备不缓存事项/课表 + 介质禁令」）零改动——而 NFR-OFF-1 是验收 AC 的回引常客。
- **证据**：requirements.md:594；architecture.md:149；回引点 requirements.md:232（S-F1.1 AC4）、:299（S-F1.4 AC5「设备不缓存事项（回引 NFR-OFF-1）」）；草案 §6.6:170 自认「可能被攻半拉子」。
- **影响**：acceptance-author 按 S-F1.4 AC5 的引用惯例拿 NFR-OFF-1 对照在役 grid 缓存即得字面违规。张力先于本草案存在（M3-K2 起在役），但草案既然主动开 §3.5 澄清、就应一次澄清到位。
- **处置**：FIX-SPEC（NFR-OFF-1 行 + arch L149 追踪表行各补半句「RAM 渲染缓冲不禁、掉电即失」注记，与 §3.5 同 PR）。

### F-C9 · 轴1 · MINOR · EVIDENCE_GAP —— L85 板子行为句「逐字改写」有承诺无文本，契约门无文可审

- **可证伪断言**：§3.1/§2 承诺「modifying 既有板子行为句（L85 逐字改写）、不得只追加 bullet」，但 §4 契约改动草案只给两条新增 bullet（restore 字段句 + 生产纪律句），改写后的 L85 句子全文缺失。
- **证据**：§2:36「须逐字改写带 restore 例外，**不得只追加 bullet**」；§3.1:99 同；§4:135-138 标注「（§1.3② 内，逐字）」却无 L85 改写句。
- **影响**：v2 修订记录（:5）自称「L85 板子行为句逐字改写」为九项修正之一，实际只是加了改写义务、未给改写文本——contract gate 与 adjudicator 无法审「带 restore 例外」的确切措辞；且新句须与 F-C1（data 内位置）、F-C10（降级作用域）联动改写。
- **处置**：FIX-SPEC（§4 补 L85 改写后全文）。

### F-C10 · 轴3/S3 · MINOR · SPEC_INTERNAL —— 契约句降级条款作用域歧义：可误读为覆盖缺省路径的在役 SetChatMessage 降级

- **可证伪断言**：§4 契约句「解析失败降级 = 静默记日志、不弹提示不动前台」未显式限定 restore=true 分支，字面可读成 schedule view 全部解析失败改静默——误伤缺省路径在役降级行为。
- **证据**：§4:137 原文（「…`true` = 恢复性载荷：…；解析失败降级 = 静默记日志、不弹提示不动前台」——分号并列，作用域未钉）；缺省路径在役降级 = `mcp_server.cc:630-636`（`SetChatMessage("system", "课表数据有点问题，稍后再试试")`，M3-K2 冻结行为）。charter 轴 3 第三问（「缺省路径降级是否被新句误伤」）判：有被误伤的字面空间。
- **影响**：S3 双降级分支的契约句不完整；落地后 M3-K3 语音路径的降级提示可能被静默化（冻结行为回归）。
- **处置**：FIX-SPEC（限定语改「`data.restore: true` 载荷解析失败」）。

### F-C11 · 轴3/S2 · MINOR · DESIGN_GAP —— 「P2 在屏」判定谓词未定义，home 先例同型移植会漏 SERVER_PAGE 接管态

- **可证伪断言**：草案 firmware 段（:38）只说「仿 `kidpal_render_node_home_on_screen` 先例补一个 on-screen 查询/重绘小缝」，spec/ADR 层未钉谓词语义；而 home 先例谓词以 LOCAL_PAGE 为门，同型移植到 P2 将判不出「语音推课表接管（SERVER_PAGE 层）下在屏的 P2」。
- **证据**：`kidpal_render_node.cc:472-477`：`return (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE && s_local_page == KP_PAGE_HOME) ? 1 : 0;`——接管态（mcp_server.cc:656 `begin_page` 推 SERVER_PAGE）下 top ≠ LOCAL_PAGE。
- **影响**：S2 竞争场景（P2 在屏过零点 + 语音接管）层栈交互无 spec 条款；接管态下 restore 到达的行为（重绘 or 仅填缓存）由谓词实现隐式决定——两态皆无害但语义未钉，正是 charter S2 攻点「前台仲裁语义是否有 spec 条款」。
- **处置**：FIX-SPEC 轻量（ADR-005 决策句钉一句：「P2 在屏谓词 = 本地页前台态；SERVER_PAGE 接管态下的 restore 仅填缓存——接管渲染本已携带新数据」或等价裁定）。

### F-C12 · 轴3 · MINOR · SPEC_INTERNAL —— `restore: false` 显式置位态未定义

- **可证伪断言**：契约句只写「缺省/absent = 现行为」，布尔显式 `false` 态未列（charter 轴 3 第一问）。
- **证据**：§4:137 原文「缺省/absent = 现行为：无条件接管前台切 P2」——无 false 分支；§3.4:116 同。
- **影响**：三值两态（absent/false/true）还是二态不钉，设备实现自由裁量；未来 front 侧「显式置 false」的调用方行为无契约承载。
- **处置**：FIX-SPEC（句补「或显式 `false`」）。

### F-C13 · 轴1 · MINOR · SPEC_INTERNAL —— canonical 追加句引用无定义标识符 R-S0.2

- **可证伪断言**：§3.3 拟落 requirements 的追加句含「（R-S0.2）」，但 R-S0.2 在 docs/ 全域（含 archive/spec.md、git -S 全史）零定义——仅存于 delivery review packet（260911-render-pages/review-packet.md:101）与固件注释（kidpal_render_node.cc:740）。
- **证据**：§3.3:109 原文「显示友好空态（R-S0.2）」；`grep -rn "R-S0" docs/` = 空输出（含 docs/archive/）；`git log --all -S "R-S0.2" -- docs/` = 空。
- **影响**：落地后 requirements 读者遇悬空引用；canonical 不得引用 delivery 瞬态面标识符（与 F-C4 同类纪律）。
- **处置**：FIX-SPEC（追加句去标签，或以一句自足描述承载空态行为）。

### F-C14 · 轴5/C7 · MINOR · CONTRACT_DRIFT —— 契约生产纪律句枚举三触发 = 时机信息入契约，撞 2026-09-21「推送时机不进契约」边界（C7 canary 命中）

- **可证伪断言**：§4 契约句「仅 front 自身的三触发推送路径（建连 / 每日跨天 / 录入 marker）可置 true」把触发枚举（= 时机语义）写进契约句，与 charter 必载铁律「推送时机不进契约（2026-09-21 契约门裁定，契约 §1.3⑥ L147）」冲突。
- **证据**：§4:138 原文；契约 §1.3⑥:147「推送时机（WS 建连 / 每日零点 / 新建日程即时）属行为策略，权威承载 = arch §5.2 场景① + §10 决策 26，不进契约」——既有生产纪律先例（契约 §1.3②:83）只写 producer 出处（「front 机械计算…不由 LLM 生成」），不写触发时机。另同句尾括注「升级顺序纪律见 review 风险段，不进契约」把契约读者指向 delivery 瞬态文档 = canonical 内死指针（与 F-C13 同类）。
- **影响**：C7 canary 判「§4 契约草案句没有把时机/顺序纪律偷运进契约」——命中。触发枚举入契约后，未来触发集调整（如轴 4 裁定砍②）须重开契约门，契约面被不必要地钉死在时机上。
- **处置**：FIX-CONTRACT（改出处纪律措辞：「仅 front 机械计算产出的恢复性载荷可置 `data.restore`；意图薄载荷展开恒不置」；删「见 review 风险段」括注）。

### F-C15 · 轴6 · INFO —— 七条验收锚可机判性逐条核 + charter 预埋三攻点全部有基建答案；唯一缺口 = 部署顺序无可机判锚（归 F-C4）

- 逐条：①帧序断言（pytest offline，`_relay_schedule_payload` 可注入 fake websocket——在役 K1 三触发模拟齿先例）✅；②展开产物无 restore（纯函数断言）✅；③层栈断言——**基建在位**：`kidpal_layer.h:59-61` push/pop/top 编入 host runner（key_layer_dispatch.acceptance.c:19 自述「kidpal_layer.c 均已编入 runner」）+ `kidpal_render_node_front_layer()` 在世；「不新增 SERVER_PAGE」可用 top 查询判（push 必改 top）✅；④薄载荷回归（host 行为级）✅；⑤「不触发 SetChatMessage」——host runner weak 哨兵/stub 捕获 Display 调用（key_layer_dispatch.acceptance.c:18-19 weak 哨兵模式先例）可行 ✅；⑥alert 起轮（pytest，`_register_alert_legs` 先于建连推送、front_app.py:764-773 顺序不动）✅；⑦幂等——两次注入后缓存 memcmp 判据可行 ✅。
- charter 预埋三攻点（⑤机判方式 / ③层栈基建 / ⑦判据）——全部有现成基建答案，预埋不成立为 finding。
- 唯一缺口：部署顺序（S1）无可机判锚，纯流程纪律——与 F-C4 合并处置。
- **处置**：无独立 finding；F-C4 连带。

### F-C16 · 轴4（必辩项，独立论证，非 finding）—— 触发②必要性成立；触发③诚实定性 = 靠不变式与直觉一致性支撑、独立价值薄

- **②独立论证（同意保留）**：触发①只在 WS 握手时发射；§6.4 的「闲置约 3 分钟休眠」只覆盖闲置态——活跃使用/充电座场景设备可长在线不重连，①无法触达。而 grid 的 flags 是日期敏感计算：bit0 一次性安排「过期自动忽略」（契约 §2.3:218）、bit1 星标跟随当日 group_upcoming——跨零点后旧网格的 ※ 过期项与星标位必然 stale。对持续在线设备，②是唯一刷新时机，非空转。成本 = 每 online 设备每日一帧 ~3-4KB，可忽略。**草案答辩方向（※/星标过期）经独立推导成立**。
- **③独立论证（保留，但理由要如实记）**：诚实地说③的独立价值薄——设备闲置 3 分钟断连 → 重连频率高，grid staleness 窗口本就 ≈ 分钟级，③只惠及「录入瞬间恰在线且此后持续在线不重连」的窄窗口。砍③省的帧可忽略，代价是制造「两附带一不附带」异例：读者必问「录入新课后 upcoming 即时刷新、grid 反而不刷新？」——为回答它要另立解释条款，维护成本反高于增量实现（piggyback 点单一，front_app.py:557-578 同一循环）。**结论：保留③，理由 = 不变式维护 + 产品直觉一致性，非独立必要性；ADR-005 理由段宜按此如实记，勿将③与②并列为同等必要**。供 adjudicator 裁（charter 预定路径）。

### F-C17 · 轴7（考据结论，非 finding）—— 疏漏定性 CONFIRMED，且证据强于草案自引；Shape C 保持否决、推荐位表不作废

独立考据五条证据（前三条为草案未引用的新证据）：
1. **决策 26 原文自限范围**（architecture.md:416）：「P1『接下来』刷新三触发 = …」——决策本体从未主张覆盖 grid，不存在「有意排除 grid」的文本痕迹；决策 26 定稿（2026-09-02）时周网格尚不存在，无排除对象。
2. **K1 设计期默认 producer 推 grid**：260921-m3-k1 plan.md 取舍点 A 乙案表述「设备用缓存网格（**producer 已推过**）仅切页」——K1 设计空间里「producer 推 grid」是默认前提，但实现的 M3K1-A4 三触发齿只接了 upcoming（taskpackage.json:30「①WS 建连即推…」全段无 grid）。意图与实现的落差有文档实证。
3. **M3K1-A5 验收文本预期课表页刷新**（taskpackage.json structural：「真机联网/零点/录入三时点**首页与课表页**实际刷新目击」）——K1 自己的 live 验收锚预期三时点课表页刷新，而实现根本不推 grid，该锚自实现起即落空。这是「疏漏」最强的直接证据：意图面（验收锚）写了、实现面漏了。
4. **human 认知定性**：K6 立项（m3-schedule progress.md:237）症状 B 被定性「根因更深…另走设计入口（可能触契约）」且 human 拍板——human 认知里这是待修缺陷、不是产品语义；:252「维持 human 拍板『另走设计入口』」持续挂账。
5. **M3-K2 实现了 BOOT 翻页读缓存**（mcp_server.cc:644-646「供 BOOT 双按翻页 cycle_next 读取」）——若「课表页语音入口唯一」是有意设计，此路径无存在意义。
- 反方假说（当时隐含设计 = 语音入口唯一）五条证据下无一立足。**轴 7 清白：定性 = 决策滞后疏漏，Shape C 不复活**。

### F-C18 · 轴5 · INFO（DISMISSED 候选）—— 验收面 canonical 承载「断链」不成立

- charter 消费方表标「server pytest 两面锚非 canonical——验收面消费方在 spec 文本中无承载句」为断链候选。核：canonical spec 惯例不列测试文件名（S-F1.2 既有 AC1-AC4 亦无测试锚，L264-268）；acceptance-author 从行为条款派生验收 = dev-loop 既有模式；S-F1.2 锚行（L272）指 arch §5.5.4/§5.1 已覆盖实现面锚定。草案 §5 定位「供 acceptance-author 起包取材」与既有模式一致。**DISMISSED-WITH-GROUNDING**。

### F-C19 · 轴1d · DISMISSED —— 断线重连构成触发①，两处口径一致

- front_app.py:762-773 建连推送在每次 websocket 握手路径（`ws_gateway` 连接处理内），重连 = 新握手 = 重推；§3.3「联网恢复后下次建连自动补齐」与之自洽。无 finding。

### F-C20 · 轴3/S4 · DISMISSED —— 重连风暴重复双推幂等，无需 spec 不变量句

- 缓存覆盖写幂等（同数据重写）；两推各自独立 try/except 不阻断建连（现状语义，front_app.py:765-774）；P2 在屏重绘为视觉 no-op。留验收锚（§5.7）足够。草案答辩成立。

### F-C21 · 轴1e · DISMISSED —— AC1 断网自洽由补充句承载足够

- S-F1.2 补充规则本就是 AC 的标准限定机制（数据口径等先例在位，L271）；AC1 字面断网张力先于本草案存在（在役空态自 M3-K2 起），补充句显式承载断网口径后自洽。AC 无需加 Given 限定。

### F-C22 · 轴5/S5 · MINOR · DESIGN_GAP —— restore 模式泛化性无表态，P3 story_list 同病复发时无既定路径

- **可证伪断言**：ADR-005 草案（§3.4）未显式说明 restore 是通用「数据面更新 vs 前台接管分离」模式还是 schedule 一次性补丁；S5 场景（story 列表冷缓存同病）在修订文本中零响应。
- **证据**：同病实存——`kidpal_render_node.cc:764`「故事缓存仅在语音…（R-S0.2, **schedule 空态同款**）」；ADR-005 理由段（§3.4:117）只述 schedule 本案，未立泛化立场。
- **影响**：S5 复发时每类冷缓存数据重新走全流程对抗（或各自造新 additive 字段，契约面膨胀轨迹——charter S5 敏感点原话）；反之若显式立泛化立场，后续 case 是模式套用、轻量得多。
- **处置**：FIX-SPEC 轻量（ADR-005 理由或关联段加一句泛化立场表态；具体取「通用模式、逐案套用仍走各自 review」还是「schedule 专属」由 adjudicator 裁，但须有表态）。

## Canary 独立重派结果（charter §5，8 条）

| # | Canary | 重派结论 |
|---|---|---|
| C1 | 语音接管不回归 | **已独立重新找到**：风险点 = F-C10（降级条款作用域歧义可能静默化缺省降级）+ F-C5（验收齿 2 守错面）；守卫 = §5.2/§5.4 有效但需 F-C10 修复 |
| C2 | alert 起轮不回归 | **已独立重新找到**：`_register_alert_legs`（front_app.py:764）先于建连推送 try 块（:766-774），grid 追加推在 try 块内 upcoming 之后，注册时序不受扰；§5.6 断言面充分 |
| C3 | BOOT 翻页读缓存不回归 | **已独立重新找到**：restore 只经 `kidpal_render_node_cache_schedule_grid`（:945-957）写缓存，BOOT 读路径（:739-754 空态/渲染分支）零改动；但注意 F-C11（新缓存写须持 LVGL 锁——现 takeover 路径经 begin_page 隐式持锁，restore 路径无 begin_page，须显式持锁，实现面注意项） |
| C4 | home upcoming 不回归 | **已独立重新找到**：§5.1 帧序断言（home 先 grid 后）+ K6 锚① 实证链（progress.md:257） |
| C5 | 决策 24 介质禁令 | **已独立重新找到并扫四 shape**：A/B/C/D 均零 NVS/Flash/SD 依赖；Shape A 全程 RAM（`s_grid_courses` 静态存储、掉电即失） |
| C6 | additive 向后兼容 | **已独立重新找到**：缺省分支零偏移（旧 server 载荷无 restore 键 → 设备走现路由；`kspj_skip_value` 对未知键容忍先例 = schedule 空态同源解析器）——但注意 F-C3：向后兼容的反向（新 server + 旧固件）不成立，恰是 S1 |
| C7 | 推送时机不进契约 | **命中**：F-C14——三触发枚举入契约句 + 「见 review 风险段」死指针 |
| C8 | F3 记账纪律 | **已独立重新找到**：grid 附带沿用 `run_schedule_poll_once` 同一 due 判定点（front_app.py:571 `_daily_push_local_date` 单点），无第二套记账入口；§5.7 承诺与 schedule_push.py:73-81 单源注释一致 |

**Canary 记分：7 独立重派成立 + 1 命中（C7）+ 0 漏。**

## 场景走查结论（charter §3，S1-S6）

| # | 判定 |
|---|---|
| S1 | **有 finding**：F-C3（失败模式已可定案 = 确定性接管，草案「未定案」表述过时）+ F-C4（部署纪律 canonical 无主） |
| S2 | **有 finding**：F-C11（P2 在屏谓词未定义、层栈仲裁无条款）；竞争双腿本身无害（缓存写 + 接管渲染各携数据，LVGL 锁串行） |
| S3 | **有 finding**：F-C10（双降级契约句作用域歧义）；F-C5 联动（front 校验先拦 vs 设备兜底的分工未钉） |
| S4 | **清白**：确定性响应在案（§6.3 + §5.7），幂等成立（F-C20） |
| S5 | **有 finding**：F-C22（泛化性无表态） |
| S6 | **有 finding**：F-C5（三道拦截主张被证伪两道半——system prompt 之外，截获层只拦薄载荷、schema 层不查 restore；机械齿守错面） |

## 清白轴清单（供 defender 对表）

- **轴 1**：不清白（F-C1 BLOCKER + F-C2/C6/C9/C13），但预埋 1d（F-C19）清白。
- **轴 2**：半清白——决策 24 禁令本体零触碰（C5 清白）；不清白在措辞面（F-C7 §3.5 判据短语、F-C8 NFR-OFF-1 半拉子）。「渲染缓冲 vs 业务状态」分界问题：判据不需新立，决策 24 的「RAM + 掉电即失」已是可判定判据（见 F-C7 处置）。
- **轴 3**：不清白（F-C3/C4/C10/C11/C12）；多腿建连/幂等半边清白（F-C20）。
- **轴 4**：无论证性 finding（F-C16 为论证非 finding）；结论 = ②必要性独立成立、③靠不变式支撑宜如实记。
- **轴 5**：基本清白——验收面承载断链不成立（F-C18 DISMISSED）；不清白仅 C7 契约边界（F-C14）。
- **轴 6**：清白——七条锚可机判 + charter 预埋三攻点全有基建答案（F-C15，INFO）；部署锚缺口归 F-C4。
- **轴 7**：清白（F-C17 CONFIRMED 疏漏定性，新证据三条）。
- **场景**：S4 清白；S1/S2/S3/S5/S6 见上表。

## 自评最弱 finding（最可能被 defender 翻掉）

- **F-C11**：defender 可辩「谓词是实现细节、归 builder，两态皆无害故无需 spec 条款」——反驳余地在「无害不等于无需钉语义」，但严重度本就 MINOR。
- **F-C7**：defender 可辩「跨连接会话的持久层缓存」显指持久层、我的读法是敌意曲解——但拟落 canonical 的句子值得钉准判据，FIX 成本一句话。
