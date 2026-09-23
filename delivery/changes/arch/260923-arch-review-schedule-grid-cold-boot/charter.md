# Review Charter · P2 课表页周网格冷启动恢复时机（schedule grid cold-boot restore）

> **性质**：本文是 `/arch-change`（Track A，spec lane）对抗 review 的**冻结 charter**，由 scenario-author 于 challenger 派发前独立冻结。challenger / defender **只读**本文；修订须经 review-moderator 重派 scenario-author。被审草案 = 同目录 `candidate-spec-draft.md`（起草 architect 产候选，非 canonical）。
> **冻结日期**：2026-09-23。目录 = `delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/`。

---

## 1. Review 对象与定性

**被审对象**：`candidate-spec-draft.md` 提出的 Shape A 候选修订——三触发（WS 建连① / 每日 0:00 跨天② / 录入 marker③）推送点附带下发带 `restore: true` 的完整周网格载荷；设备只填 RAM 网格缓存（`s_grid_courses`）、不推 SERVER_PAGE 层、不接管前台；`restore` 缺省 = 现行为（薄载荷展开无条件接管前台，M3-K3 冻结行为）。备选 Shape B（设备按需拉取）/ Shape C（需求面砍 S-F1.2）/ Shape D（时间窗启发式）草案均自判否决。

**触面定性**（spec lane + contract-touching flag）：

| 文件 | 改动 | 性质 |
|---|---|---|
| docs/requirements.md | S-F1.2 补充规则（L271）追加冷启动恢复句 | spec 行为规则 |
| docs/architecture.md | §5.2 场景①（L207）追加 grid 恢复触发 + §6.2（L315）RAM 渲染缓冲澄清半句（§3.5，标可选） | spec 行为规则 + 边界澄清 |
| docs/ADR.md | append ADR-005 | 决策记录（append-only，头部约定） |
| docs/contracts/interface-contract.md | §1.3② additive 可选布尔 `restore` + 板子行为句 + front 生产纪律句 | **CONTRACT_CHANGE_REQUIRED（随 spec 同 PR 原子落地，走人类契约门）** |

**变更类型**：决策滞后补齐（决策 26 三触发只覆盖 upcoming，定性 = 「疏漏补齐，非有意取舍」）。**此定性本身是可攻轴**（charter 轴 7）：若 challenger 论证成立「当时的隐含设计 = 课表页语音入口唯一、属有意取舍」，则 Shape C（需求面砍）须重估并升 human——该分叉直接改变备选池，不可在辩前预判。

**本 spec 类型判定**：技术重（跨系统运行时推送语义 + 固件行为分支 + 契约 wire 字段），非领域重。按 rubric §怎么用，技术重必带 4/5/6/9/10/11 类轴——本 charter 轴集已覆盖其中可适用者；不适用轴的排除理由见 §2 末尾。

**必载仓内铁律**（challenger 判定依据，辩论全程有效）：

- **契约门**：契约改动 = architect-reviewer 四分类 + human 契约门；spec 改动与契约改动**同 PR 原子落地**，契约未批前 spec 不可冻结（arch-review-protocol Spec↔Contract 铁律 3）。
- **决策 24 介质禁令**（arch L414）：板端非易失介质白名单 = 凭证 + 预栅格化资产/字体 + 固件本体；一切业务状态只驻 RAM、掉电即失。任何恢复机制触碰 NVS/Flash/SD = BLOCKING。
- **决策历史档冻结**（ADR.md 头部 + arch §10 约定）：append-only，推翻旧决策 = 新增 ADR 并注明 supersedes，不改写决策 26 原文。
- **M3-K3 语音接管 + alert 起轮 + BOOT 翻页读缓存 = 在役冻结行为**（草案 §1 约束边界自列；challenger 须独立核其对「现行为零偏移」的承诺可证伪）。
- **推送时机不进契约**（2026-09-21 契约门裁定，契约 §1.3⑥ L147）：时机属行为策略，权威承载 = arch §5.2 + ADR-005；设备对载荷的行为语义与生产纪律属 §1.3② view 定义本体、可进契约。这条边界本身是可攻轴（additive 字段草案 §4 把「旧固件对未知字段的行为不构成兼容承诺」也写进契约句——「升级顺序纪律不进契约」的自判是否站得住）。

---

## 2. Rubric 轴集（逐轴可裁 falsifiable）

challenger 按序攻击，**falsifiable-first**：先打可证伪项，再打软判断。每条 finding 必须落到 spec/contract 文件名 + 行号。

### 轴 1 · 内部矛盾与漂移（falsifiable-first 开局轴；rubric 轴 4/8）

- **1a（预埋）`restore` 键位置内部矛盾**：草案 §2 Shape A 称「携带新 wire 字段 `restore: true`」、§3.4 ADR-005 称「契约 §1.3② 增 additive 可选布尔」；但 §3.2/§3.4 firmware 段称「`mcp_server.cc` schedule 路由读 **`data.restore`**」，§6.2 又称「旧固件对 **data 顶层未知兄弟键**行为未证实」，而 §4 契约改动草案却写「**载荷顶层**可选布尔 `restore`」。「载荷顶层」（message 顶层）与「data 顶层兄弟键」（data 对象内）是两个不同的 wire 位置，影响旧固件解析路径、front `display_schema` 校验面与契约 §1.3② JSON 形状图——challenger 必须首先逼出唯一位置。
- **1b（预埋）front 机械校验面断链**：契约 §1.5.3（interface-contract.md L167）声明 front 对 display 块做机械 schema 校验（data shape / 大小上限）。若 schedule data shape 校验按键白名单或形状严判，新增 `restore` 键可能被校验层拒掉——草案 §3.1 修订总览只列 §1.3②，**未列 §1.5 校验面的同步改动或「已兼容」的证明**。challenger 判：这是 spec↔contract 漂移还是已覆盖？
- **1c（预埋）ADR-005 关联行引用失效路径**：§3.4 关联段写证据落 `delivery/changes/arch/arch-review-260923-schedule-grid-cold-boot/`，而草案 §0 自注目录已改名 `260923-arch-review-schedule-grid-cold-boot`——若 canonical 落 ADR-005 时照抄，即写入死链。判严重度（NIT 级候选，但属可机判矛盾）。
- **1d**：requirements 补充句（§3.3）「联网恢复后**下次建连**自动补齐」vs arch 追加文本（§3.2）三触发含 0:00 跨天与 marker——「断线重连是否构成触发①」（front_app.py 建连推送在每次 websocket 握手路径，重连 = 新连接即重推）两边口径是否一致、§6.4 断线重连语义是否需要显式对齐。
- **1e**：跨 spec 矛盾扫描——S-F1.2 正文/AC1-AC4/锚/可行性行声称零改动（§3.1），challenger 逐行核对 Shape A 落地后 AC1「进课表页即见」在**断网场景**是否仍与 R-S0.2 空态条款自洽（S-F1.2 补充句自己写了「断网显示友好空态」——AC1 是否需要 Given 限定，还是补充句足以承载）。

### 轴 2 · 决策 24 介质禁令与「RAM 缓存填充」边界精确性

- 决策 24（arch L414）白名单措辞「一切业务/身份/事项状态只能驻留 RAM、掉电即失、禁落任何非易失介质」。M3-K2 起在役的 `s_grid_courses`（kidpal_render_node.cc:728-748，84×60B≈5KB）是合法 RAM 渲染缓冲——但 §3.5 澄清句只改 **arch §6.2**，requirements 侧 NFR-OFF-1 的「不缓存」措辞**零改动**（草案 §6.6 自认「可能被攻半拉子」）。challenger 判：① §3.5 半句是否足以消解字面张力，还是 NFR-OFF-1 行必须同步注记（FIX-SPEC 候选）；② `s_grid_courses` 填充算不算「事项状态驻 RAM」的合法面——「渲染缓冲」与「业务状态缓存」的分界在 spec 文本里是否有可判定判据（掉电即失 + 不跨连接会话是否足够）。
- 决策 24 冻结不动是硬约束：任何 shape 变体或 challenger 替代方案若引入持久化，直接 BLOCKING。

### 轴 3 · restore 语义完备性（rubric 轴 9 不变量与失败模式）

逐分支逼问语义是否有 spec 文本承载，缺一处即 finding：

- **缺省分支**：`restore` absent/false = 现行为无条件接管前台——契约草案句「缺省/absent = 现行为」是否覆盖 `restore: false` 显式置位（而非仅 absent）？两值三态（absent / false / true）还是布尔二态？
- **P2 在屏重绘语义**：restore 到达且 P2 在屏 = 仅重绘本页——重绘时序 vs 正在进行的语音渲染/alert 弹窗覆盖层（§2.9）的层栈交互有无语义条款？「P2 在屏」的判定谓词（`kidpal_render_node_home_on_screen` 同型小缝，草案 firmware 段承认需新补）在 spec 层是否需要命名约定。
- **降级静默语义**：parse 失败 = 静默记日志、不动前台——与主动展示路径的 `SetChatMessage("课表数据有点问题")` 降级形成**同 view 双降级行为**。契约句是否把两个降级行为都写清（§4 草案句已含「解析失败降级 = 静默」，但只写了 restore 路径的降级；缺省路径降级 = 契约既有文本，challenger 核对是否被新句误伤）。
- **多腿建连时序**：建连双推（upcoming 先、grid 后）非原子——一条成功一条失败的不一致窗口；重连风暴下的重复双推幂等性（草案答辩 = 缓存覆盖写天然无害，§5.7）。challenger 判该答辩是否需要 spec 层不变量句还是留验收锚即可。
- **旧固件部署窗口**：server 先行 + 旧固件在场 = 回归级事故（草案 §6.2 自列，且「旧固件忽略未知键」未证实）。**焦点问题：这条部署顺序纪律的 canonical 承载在哪？** 草案把它放「review 风险段不进契约」+ §7「非本草案义务面」——即目前**无 canonical 归属**（不进 spec、不进契约、§7 自称 handoff 取材面）。challenger 判：deployment-facing 不变量无主 = 轴 9 失败模式无设计响应，FIX-SPEC（arch §5.3 部署视图或 ADR-005 决策句内承载）候选。

### 轴 4 · 触发②③附带 grid 的必要性 vs 复杂度（rubric 轴 8 完整性 + F3 记账）

- §6.4 已知「设备闲置约 3 分钟休眠断 WS」——0:00 时设备大概率离线，跨天重推实由次日建连兜底；触发②附带 grid 可能是离线空转。草案自辩（§6.4）：在线挂 P2 过零点需刷新 ※ 过期项与星标位（bit0/bit1 云端算）；且「三触发同源同附带」不变式比「两附带一不附带」好守。**草案自己标注「此点真实存在取舍空间，供 adjudicator 裁」——本 charter 冻结该点为必辩项，challenger 必须给出独立论证（同意/反对及理由），不得复述草案双方话术。**
- 触发③（录入 marker）附带：录入后用户多半在 P1/P0 看「接下来」，grid 附带的即时价值 vs 每条录入路径的额外帧；以及与 piggyback 通道现有失败语义（不阻断）的相容性。
- F3 记账纪律：§5.7 承诺不复制 `_daily_push_local_date` 形态、不新增第二套进程内日账——challenger 核对三触发共用同一 due 判定点的表述与 schedule_push.py `connect_push_payload` 单源注释（L73-81）是否一致、有无第二套记账的潜伏入口。

### 轴 5 · 双向消费方完整性（rubric 轴 1；详见 §4 清单）

- 每个 expose/推送出口有匹配消费方声明；每个被改条款有具名读者。特别核对：**验收测试两面**（server pytest + firmware host acceptance）是否在 spec 文本里有锚（草案 §5 只是「方向」非 canonical 条款——冻结后 acceptance-author 消费它，但 canonical spec 里验收锚承载在哪一句）；**后续 shape 变体扩展性**（P3 story 列表同病）——restore 模式是通用「数据面更新 vs 前台接管分离」模式还是 schedule 专属一次性补丁，ADR-005 理由段应可判。

### 轴 6 · 验收锚可机判性（rubric 轴 8；供 acceptance-author）

- 草案 §5 七条逐一判：可机判？可独立重派（不依赖草案行文语境）？两面（server pytest / firmware host）覆盖齐？
- 预埋攻点：⑤「不触发 SetChatMessage」的机判方式（日志断言？输出捕获？）；③「层栈快照不新增 KIDPAL_LAYER_SERVER_PAGE」需要 host 测试基建具备层栈快照能力——现存 acceptance 面是否有此前例；⑦「载荷幂等性」断言的判据（缓存覆盖写后位 diff 为零？）。
- 七条是否够：部署顺序（旧固件窗口）有没有对应可机判锚，还是纯流程纪律。

### 轴 7 · 「疏漏 vs 有意取舍」定性考据（软轴，但裁决影响备选池）

- 草案证据 = 时序（决策 26 定稿 2026-09-02 早于周网格设计 12 天）+ M3-K2 实现 BOOT 翻页读缓存（若有意语音入口唯一则该路径无意义）+ K6 真机实证 P1 恢复 P2 恒空。challenger 判考据强度：① 复核决策 26 定稿时的 grilling 记录/arch 修订史有无「课表页语音入口」的显式表述；② 若判「有意取舍」，Shape C 升 human 重估——此时本 review 的推荐位表（§2 推荐位汇总）作废，adjudicator 打包 human 拍板。**challenger 必须给出独立考据结论，不能以「草案已自证」放行。**

### 排除轴及理由

- 轴 0 范围/层级越权、轴 2 相位依赖、轴 3 命名冲突、轴 5 所有权&Tier、轴 6 门/编排、轴 7 DEFER：本变更不新增系统能力归属、不跨相位依赖、不改门结构（producer 仍 = front schedule.py 单源，ADR-004 不动），无适用面。
- 轴 10 安全/信任边界：token/鉴权/18789 面零触碰；`restore` 生产纪律的 LLM 幻觉置位风险已被「薄载荷由 front 截获展开」+ display_schema 校验拦截——但 challenger 若在轴 4/5 辩论中发现校验面真缺口（轴 1b），可升级为轴 10 finding，不预冻。
- 轴 11 AI-native：本变更不新增人工门，自动化程度只增不减，无适用面。

---

## 3. 冻结场景（ATAM 质量属性 + pre-mortem，challenger 逐个走）

| # | 类型 | 场景（刺激 → 环境 → 预期响应） | 敏感点/攻点 |
|---|---|---|---|
| S1 | 失败/部署（**含失败场景**） | server 先升级 ECS、旧固件在场 → 每次建连收到 restore 载荷 → 旧固件按现行为接管前台（开机被顶到课表页）或弹「课表数据有点问题」 | 回归级事故；canonical 承载无主问题（轴 3 末条）；「旧固件忽略未知键」未证实须 builder 实测定案 |
| S2 | 一致性/并发 | P2 在屏挂过 0:00（触发②附带 restore 重绘本页）同时孩子喊「帮我显示课表」（薄载荷展开接管前台）→ 两腿竞争 | 缓存写序 / 前台仲裁语义是否有 spec 条款；层栈交互与 alert 弹窗覆盖 |
| S3 | 健壮性 | restore=true 且 data 畸形/超限 → 静默降级；对照缺省路径畸形 → SetChatMessage | 双降级行为分支的契约句完整性；front display_schema 校验先行拦截还是设备兜底（轴 1b 联动） |
| S4 | 性能/成本 | 设备闲置 3 分钟休眠断 WS（§6.4 残余）→ 反复重连 → 每次重连双推 upcoming + grid(~3-4KB) | 重连 churn 下的重复双推幂等性；两推独立降级不阻断建连的失败面（一条成功一条失败窗口） |
| S5 | 可改性 | P3 故事列表冷缓存同病出现 → 是否复用 restore 模式 | 模式泛化 vs schedule 一次性补丁；若每类数据都要新 additive 字段，契约面膨胀轨迹 |
| S6 | 信任边界（升级候选） | OpenClaw/LLM 产出的 display 块幻觉携带 restore 键或完整网格 data | system prompt 约束薄载荷 + front 截获展开 + display_schema 校验三道拦截是否足以钉死「谁可置 restore」的运行时面（联动轴 1b / 生产纪律只有验收齿无运行时闸的草案自列关注点 1） |

---

## 4. 双向消费方清单（expose/推送 ↔ 消费方声明；留空 = challenger 必打断链点）

| 条款/载荷 | 生产方 | 声明的消费方 | 消费声明在哪 | 断链/留空 |
|---|---|---|---|---|
| 契约 §1.3② `restore` 字段 | front 三触发推送路径 | 设备 mcp_server.cc schedule 路由 | 草案 §3.4 契约句「板子行为条款补句」 | **缺省分支消费方**：现役旧固件（不识 restore）——其行为被「不构成兼容承诺」句排除出契约保护，但部署窗口事实存在（S1） |
| 三触发推送序列（upcoming + grid 两条） | front_app.py 建连推送点（L762-773）+ schedule_push.py 单源委托（L73-81）+ 0:00/marker piggyback | 设备建连初始化路径 | 草案 §3.2 arch 追加文本 | server pytest（§5.1）两面锚非 canonical——**验收面消费方在 spec 文本中无承载句** |
| requirements S-F1.2 补充句 | spec（本修订） | acceptance-author、arch §5.2、契约行为句、dev-loop builder | S-F1.2 锚行（L272）引 arch §5.5.4/§5.1 | 补充句新增的「断网友好空态」口径在 R-S0.2 侧无回链声明 |
| arch §5.2 场景① grid 恢复触发 | spec（本修订） | 决策 26 读者、ADR-005、front 实现 | §3.2 文内交叉引用决策 26 | 决策 26 原文（arch L416）不改——**读者从决策 26 单独进入时无指回 ADR-005 的声明**（append-only 代价，判是否需在 §10 决策历史档加注） |
| arch §6.2 RAM 渲染缓冲澄清 | spec（§3.5） | NFR-OFF-1 读者、决策 24 读者 | §3.5 文内引 §5.4 板端状态行 | **requirements NFR-OFF-1 行零改动、无回链**（草案 §6.6 自认）——半拉子候选 |
| `s_grid_courses` RAM 缓存 | kidpal_render_node.cc:728-748（M3-K2 在役） | BOOT 翻页读路径（586-601 SERVER_PAGE 层） | arch §5.5.4 | 无 |
| ADR-005 | spec（本修订） | 未来 spec/ADR 读者、契约 §1.3② 变更记录 | §3.4 关联段 | 关联行引旧目录名（轴 1c） |
| K6 锚①（建连 upcoming 8.6s 恢复） | front（在役） | 本 review 证据 + K6 progress | commit bd21d24 | 无 |

---

## 5. Regression canaries（challenger 必须独立重新派生，漏则辩论判浅）

| # | Canary | 来源（历史决策/既往证据） | 独立重派判据 |
|---|---|---|---|
| C1 | 语音接管不回归——薄载荷展开无条件接管前台（M3-K3 冻结行为） | 契约 §1.3② 生产纪律（2026-09-21 契约门）+ `server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py` + 草案 §1 约束边界 | challenger 须自行指出 restore 引入后 M3-K3 面的回归风险点与守卫（§5.2/§5.4 验收齿），不得复述草案 |
| C2 | alert 起轮不回归——`_register_alert_legs` / `_ensure_alert_loop` 时序与注册行为零偏移 | 决策 20/27（arch L410/L417）+ 包 260829-schedule-reminder（front_app.py L762-764 注释） | 建连推送扩为两条后 alert 登记时序是否被扰动，challenger 独立提出断言面 |
| C3 | BOOT 翻页读缓存不回归——kidpal_render_node.cc 读路径零改动 | M3-K2（requirements §2.3 键位矩阵 BOOT 翻页） | restore 只写缓存不推 SERVER_PAGE 层的承诺对 BOOT 路径的零影响，独立核 |
| C4 | home upcoming 不回归——K6 锚①冷启动自动建连 8.6s 恢复 | K6 260923-m3-k6-boot-auto-connect（progress.md 2026-09-23 段 + commit bd21d24） | grid 追加推送不得扰动 upcoming 帧序/时序/成功语义，独立核 |
| C5 | 决策 24 介质禁令不可触碰——恢复机制不得引入 NVS/Flash/SD 持久化 | 决策 24（arch L414，2026-09-02 grilling 定） | 任何 shape 变体若暗中依赖持久化即 BLOCKING；challenger 独立扫四 shape |
| C6 | additive 向后兼容纪律——缺新字段时设备行为完全不变 | `data.upcoming` 先例（契约 §1.3⑥ L135/L149，2026-09-21 契约门批准） | `restore` 缺省分支的「现行为不变」承诺须与该先例同一严格度检验；challenger 独立核缺省分支零偏移 |
| C7 | 推送时机不进契约——时机属行为策略、权威承载 = arch §5.2 + ADR | 2026-09-21 契约门裁定（契约 §1.3⑥ L147） | challenger 独立核对 §4 契约草案句没有把时机/顺序纪律偷运进契约（「升级顺序纪律不进契约」自判是否成立） |
| C8 | F3 记账纪律——不新增第二套进程内日账、三触发共用 due 判定点 | m3-schedule 期 F3 + schedule_push.py 单源注释（L73-81） | grid 附带是否引入平行记账形态，独立扫 |

---

## 6. Charter 冻结条款

1. **不可改性**：challenger / defender / adjudicator 均不得改本文；覆盖或修正须由 review-moderator 重派 scenario-author。**绝不改真 spec / 草案来造洞**（反空过守卫）。
2. **裁定口径**：辩论逐条收敛于 CONFIRMED / DISMISSED / DEADLOCK 三值；deadlock 由 adjudicator 打包正反最强论点 + 利害 + 代价 + 推荐 → human 拍板（轴 4 触发②③与轴 7 定性两处草案已自标「供 adjudicator 裁」，按此走）。
3. **finding 精度**：每条 finding 必须引 spec/contract/草案文件名 + 行号（canonical 锚：requirements S-F1.2 L260-273、arch §5.2 L207 / §6.2 L314-315 / §6.4 L321 / §10 决策 24 L414 / 决策 26 L416、契约 §1.3② L65-87 / §1.3⑥ L135-149 / §1.5.3 L167、ADR.md 头部 L5-6），逐字精确，禁止凭感觉。
4. **falsifiable-first 排序**：开局先打轴 1（内部矛盾/漂移），再按轴序推进；软轴（轴 7）不得先于可证伪轴。
5. **结案方式**：按 arch-review-protocol finding outcomes（FIX-SPEC / FIX-IMPLEMENTATION / DISMISS-WITH-GROUNDING / ACCEPT-AS-RISK / HUMAN-RULED / SPAWN-NEW-CONCERN）；实现侧修复 CONFIRMED 必产 `dev-loop-handoff.json`。
6. **冻结门**：任何未结 BLOCKING/SHOULD ⇒ spec 不可冻结；契约部分（§1.3②）未过人类契约门 ⇒ 全案不可冻结（原子性）；pass-oracle 六关（轴覆盖 / 场景覆盖 / finding 全结 / spec↔contract 机械门 / canaries 全中 / handoff 完整）全过方可出可冻结结论。
7. **契约面只读**：review 全程任何角色不得写 `docs/contracts/**`；契约改动以 ContractChangePacket 承载走 architect-reviewer 四分类 + 人类契约门。
