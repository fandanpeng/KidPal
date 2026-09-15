# Review · kidpal-mvp 开发包拆分 PROPOSAL（DRAFT v0.1）

> **review 性质**：dev 侧分流单轮 review（`.claude/references/change-triage.md` §3，非对抗红队）。
> **reviewer**：architect（子 agent，只读+审，写域零扩张——本件由主线程依 architect 同会话 return 落盘）。
> **被审件**：`.planning/kidpal-mvp/PROPOSAL.md` DRAFT v0.1（2026-08-29）。
> **交叉核对源**：`docs/spec.md`、`docs/contracts/interface-contract.md`、`.claude/references/task-intake-and-slicing.md`。

---

## 总结论

**GREEN（可盖 FROZEN 交 portfolio-planner 拆包）——附 7 条建议级修正，无 BLOCKING。**

11 包拆分合法（全垂直切片、无横切）、依赖无环且 mergeOrder 拓扑一致、story 覆盖完备（含 §4.0 共享底座与 §6 接口，S-F2.5 合法留根）、契约面只消费不改。三段式测试策略与包 0 公共前置设计扎实；§7 对 ESP-IDF linux target 兼容性的诚实标注尤其好（把不确定性显式化为包 0 验收而非预判结论）。

全部 7 条 CONFIRMED 均为**建议级**（文本/标注/rationale 层），无一影响拆分骨架正确性，下游 verifier 验收门接得住。

## 维度结论速览

| 维度 | 结论 |
|---|---|
| 1 拆分合法性（垂直切片） | ✅ 全 11 包合法，包 0 符合 L0/internal-service 例外 |
| 2 依赖与 merge order | ✅ 无环、拓扑一致；2 条建议（关键路径收口/并行集表述） |
| 3 riskTier 标定 | ⚠️ 2 条建议（9a critical 误标 / 6/8 medium 张力） |
| 4 story 覆盖完备性 | ✅ 全覆盖；1 条建议（4 页循环跨页整合归属） |
| 5 契约面 | ✅ 只消费不改；1 条建议（contractBlocked 漏列 2/6） |
| 6 测试策略 | ✅ 三段式自洽；1 条建议（testHost 口径 + 硬件子能力殿后标注） |

## Findings 明细（7 条建议级）

| # | 维度 | 结论 | 修正项 | 建议处置 |
|---|---|---|---|---|
| F2-2 | 依赖 | CONFIRMED | 关键路径收口应是 8（mergeOrder 10）非 7 | 主线程回灌改表述 |
| F2-3 | 依赖 | CONFIRMED | 调度并行集把 4/5 提前（4/5 需 2 先就绪） | 主线程回灌 |
| F3-1 | riskTier | CONFIRMED | 9a 标 critical 不符 §riskTier 定义 | **回问 human**：改 high，或明示破格 + rationale |
| F3-2 | riskTier | CONFIRMED | 6/8 标 medium 与 cross-system=high 有张力 | **回问 human**：补 rationale 或升 high |
| F4-2 | 覆盖 | CONFIRMED | 4 页循环翻页跨页整合归属不明（9b 兜底） | 显式声明兜底或加整合验收点 |
| F5-2 | 契约 | CONFIRMED | §4 contractBlocked 漏列包 2/6（§7 有） | 主线程回灌补全 |
| F6-2 | 测试 | CONFIRMED | testHost §3/§2 口径不一 + 硬件子能力未标殿后 | 主线程回灌统一 + 标注 |

### F3-1 详述（riskTier 误标，回问 human）
`task-intake-and-slicing.md:150` 的 critical 触发条件 = 「`docs/contracts/**`、migrations、data deletion、irreversible data movement」——9a（AEC 声学喊停实测调阈值）一条都不触。9a 触的是 high 条件（`:151` state machines with user impact + 音频安全相关）。「最大技术风险」是实现不确定性高，不等于 riskTier 的 critical 档——riskTier 按爆炸半径/不可逆性分级，不按技术难度。标 critical 会给 9a 挂上不匹配的最重闸（契约门级审查），拖慢一个本就需反复实测返工的包。

### F3-2 详述（6/8 riskTier 张力，回问 human）
6（课表页）与 8（故事）都跨板+云两面，按 `:151`「cross-system behavior = high」字面应 high。可辩护点：二者只消费已冻结契约 + 渲染，无契约改动、无迁移、无 AI 成本、无重状态机，blast radius 有界——落 `:152` medium。与包 2/4（也跨板+云但标 high，因含 AI 音频链路 / D3 笔顺渲染复杂度）的区分可辩护，但 PROPOSAL 未写明 rationale。

### 其余 5 条（文本级，主线程回灌）
- **F2-2**：§3 关键路径 `…→7→9b` 与 mergeOrder 8(10)>7(9) 矛盾；9b 真正等的最晚 merge 包是 8。
- **F2-3**：§3 调度说「0/1 就绪后包 3/4/5/6/8 可并行」，但 4/5 需 2 先就绪；0/1 就绪后立即可并行的只有 3/6/8（及 2）。真正最长链是 `S→{0|1}→2→{4|5}→9b→9a`。
- **F4-2**：§7 决策 14「4 页循环 BOOT 翻页」的跨页端到端整合，host 阶段无包认领，靠 9b 兜底——建议显式声明。
- **F5-2**：§4 contractBlocked 只列包 7，但 §7 明确契约待确认点 1/2/3 影响包 2/6/7——§4 应补列 2/6 或统一指 §7。
- **F6-2**：§3 简化「3/4/5/6/7/8=host」与 §2 表（5 含 SDL、6/8 含 PC 云端）口径不一；S-F2.2 录音采集、S-F1.1 RTC/SHTC3 直读是硬件强绑定子能力，应显式标殿后 9b。

## sign-off

architect（单轮 review）2026-08-29：GREEN，附 7 条建议级修正。F3-1/F3-2 触 riskTier 判断，按从严解释回问 human；其余 5 条文本级可由主线程回灌后冻结。human 盖 FROZEN 时建议钉标准 pin（`sha1:<16hex>`，raw-content 行尾归一化算法，见 task-intake-and-slicing.md:53）。

---

# Review · kidpal-mvp 开发包拆分 PROPOSAL（DRAFT v0.3 修订稿）

> **review 性质**：dev 侧分流单轮 review（`.claude/references/change-triage.md` §3，非对抗红队）。
> **reviewer**：architect（子 agent，只读＋审，写域零扩张——本件由主线程依本 return 落盘）。
> **被审件**：`.planning/kidpal-mvp/PROPOSAL-v0.3-draft.md`（DRAFT v0.3，全文重写，独立修订稿，不改 FROZEN v0.2 正文）。
> **交叉核对源**：`docs/spec.md`、`docs/contracts/interface-contract.md`、`.claude/references/task-intake-and-slicing.md`、FROZEN `.planning/kidpal-mvp/PROPOSAL.md`、`.planning/kidpal-mvp/review.md`（v0.2 记录）、`.planning/kidpal-mvp/progress.md`、`docs/notes/validation-batch-conclusion.md`、`docs/ARCH-REVIEW-QUEUE.md`、`server/shim.py`、`.claude/scripts/portfolio-reconcile.mjs`。

## 总结论

**GREEN（可交 human 盖 FROZEN）——附 10 条建议级修正，无 BLOCKING，无一项需回问 human。**

13 包拆分合法（全垂直切片、无横切；新包 U 成立为合法切片）、依赖 DAG 无环且 mergeOrder 拓扑一致（关键路径 7 级成立）、riskTier 全部稳定（U 标 high 站得住）、story 覆盖完备（含 §4.0 共享底座 / §6 接口 / §7 已定决策 / §5.6–5.7 UI 美术层）、契约面只消费不改（两个前置闸标注正确、contractBlocked 解除/维持符合验证批事实）、三段式测试策略自洽。全部 10 条 CONFIRMED 均为文本级/标注级修正，无一影响拆分骨架正确性。

**v0.2 连续性核对**：v0.2 的 7 条 findings 已全部落实于本稿正文；v0.2 冻结裁决（WIP=3、9a/6/8 tier、冒烟批、切分轴、硬件殿后粒度）忠实继承，无回退。

## 维度结论速览

| 维度 | 结论 |
|---|---|
| 1 拆分合法性（垂直切片） | ✅ 全 13 包合法；新包 U 成立（跨「板＋tools」是共享资源层切片、非技术层横切） |
| 2 依赖与 merge order | ✅ 无环、拓扑一致、关键路径 7 级成立；3 条表述精度修正 |
| 3 riskTier 标定 | ✅ 全部稳定（U 标 high 站得住）；1 条建议补 U 的 high rationale |
| 4 story 覆盖完备性 | ✅ 全覆盖；1 条建议补 decisionCoverage 映射两点 |
| 5 契约面 | ✅ 只消费不改；闸 A/B 标注正确；2 条引注精度 + 1 条 spec §528 错引 |
| 6 测试策略 | ✅ 三段式自洽；2 条建议（包 4 云侧 testHost、U 换皮后 host 回归归属） |

## Findings 明细（10 条，v0.3 系列新编号 F1–F10）

> 编号说明：v0.3 重新编号 F1–F10（v0.2 的 `F<dim>-<n>` 已全部落实、编号语义作废）。本批无回问 human 项。

| # | 维度 | 结论 | 修正项 | 处置 |
|---|---|---|---|---|
| F1 | 1 拆分合法性 | CONFIRMED | 新包 U 成立为合法切片；补切片性质 rationale | 主线程回灌 |
| F2 | 2 依赖 | CONFIRMED | U 等待集表述漏包 7（经 6 亦达 level 4） | 主线程回灌 |
| F3 | 2 依赖 | CONFIRMED | 「9b dependsOn 2~8 + U（全部 B 段功能包）」不含包 0，文字与语义不符 | 主线程回灌 |
| F4 | 3 riskTier | CONFIRMED | U 标 high 站得住，但 rationale 未写明、与 F3-2 口径不同源 | 主线程回灌 |
| F5 | 4 覆盖 | CONFIRMED | decisionCoverage 补两点：§7 决策 17 显式进 U；§5.6 教学字体红线显式认领 | 主线程回灌 |
| F6-1 | 5 契约面 | CONFIRMED | 闸 B 引注「契约 §九」——现契约无 §九 节 | 主线程回灌 |
| F6-2 | 5 契约面 | CONFIRMED | 闸 A 引注「生成 char 块」非字面 | 主线程回灌 |
| F7 | 6 测试策略 | CONFIRMED | 包 4 云侧（C1 查表拼装＋预热＋延迟验收）缺 testHost 标注 | 主线程回灌 |
| F8 | 6 测试策略 | CONFIRMED | U 换皮后的页面 host 回归归属未写明 | 主线程回灌 |
| F9 | 5 引注 | CONFIRMED | 留根「spec §528」——spec 无 §528，应为 §7 决策 1 | 主线程回灌 |
| F10 | 2 依赖 | CONFIRMED | 包 1 dependsOn S 从产物依赖变验证序依赖，性质变化需文本说明 | 主线程回灌 |

### 关键详述（择要）

- **F1（U 切片合法性）**：U 承载「美术资产系统」这一完整可独立验证能力，非技术层横切；页面包不 dependsOn U、先占位资产跑绿，U 作迁移批殿后、9b 作整合复验——满足「中间包可独立验证 + 声明 blocking edges」。
- **F2（U 等待集）**：按依赖深度 4（S→1→2→4）、5、7（S→1→6→7）都达 level 4，6/8 是 level 3；U 最深前驱集 = {4,5,7}，非「4/5 之一 + 8」。
- **F4（U=high）**：不触 critical、够不上 medium；落 high 理由 = blast radius 覆盖全 UI 面 + SPIFFS 分区级改动 + 真机强绑定 + 9b last gate。与 6/8（板＋云跨系统）不同源但不矛盾。
- **F5（覆盖）**：§7 决策 17 应显式进 U 承载项；§5.6 教学字体红线由包 4（渲染侧，主）+ U（字体资产侧）共担。
- **F7（包 4 云侧 testHost）**：查表拼装走 PC 级单测；char 延迟验收对真 ECS/OpenClaw、按 ARQ-260901-g3-server-live-acceptance 标 live（不 auto-run）。
- **F9（引注）**：spec §528 为错引，应为 spec §7 决策 1（「顶层定位：所有数据/逻辑/记忆在 OpenClaw」）。

## 特别盯新东西的三项结论（复核）

1. **C1 落进包 4 云侧守薄桥红线**：✅ 守住——静态字库挂 OpenClaw 侧、绝不入 shim；makemeahanzi 数据源本就在服务器侧。
2. **包 S 缩域后「前置通行证」定位**：✅ 成立——复用 `server/shim.py`（核验存在、已并入 main）后 S 只做设备侧闭环，仍完整证明三层链路；无 dependsOn 正确。
3. **包 1 起点前移与 S 缩域自洽**：✅ 自洽——dependsOn S 转验证序依赖仍成立（F10）。

## sign-off

architect（单轮 dev-side review）2026-09-01：**GREEN，附 10 条建议级修正（全文本级，主线程回灌即可，无回问 human 项）**。U 切片合法性、U=high、包 1 dependsOn S、C1 薄桥红线四项均确认判断（非打回）。主线程回灌后可盖 FROZEN；盖时钉标准 pin（`sha1:<16hex>`），并同步 §6 的 12→13 decision-coverage 映射（含 F5 两点）与 §8 评审记录。

---

## v0.4 单轮 dev-side review（architect，2026-09-03）

> 性质：dev 侧分流单轮 review（非对抗红队）。对象 = `.planning/kidpal-mvp/PROPOSAL-v0.4-draft.md`。交叉核对源 = `docs/spec.md`（v0.2 FROZEN）、`docs/contracts/interface-contract.md`（方向 B 版）、`changes/arch/260902-arch-review-kidpal-spec-v0.2/dev-loop-handoff.json`、FROZEN `PROPOSAL.md`（v0.3）、`decision-coverage.json`、`progress.md`、`docs/ARCH-REVIEW-QUEUE.md`、Pod S / TP-A / TP-B / TP-C 四份 taskpackage.json 与 `server/acceptance/` 冻结验收面。
> reviewer：architect（子 agent，只读+审，写域零扩张——本件由主线程依 architect 同会话 return 落盘）。

### 总结论

**GREEN-with-fixes（无阻断级、无回问 human 项；8 条建议级修正，主线程回灌后即可 human 盖冻结）。**

重切骨架正确：依赖 DAG 无环、mergeOrder 拓扑有序（G=3 早于全部服务端扩展包 7/8；2=5 早于消费者 4/5/6/8）、关键路径「G→WS→2→6→7→U→9b→9a」计 **8 级成立**（8 节点，较 v0.3 的 7 级 +1 归因正确 = tracer-bullet 拆成 G/WS 串行对）。故事覆盖完备：S-F1.1~F1.5 / F2.1~F2.4 / F3.1~F3.3 / **S-F4.1**（→包 2）/ §4.0 共享底座（→包 3 + 包 2）全部有主；§6.1 协议剖面闭合（hello→G/WS、stt/tts→2、abort→2+9a、mcp display→G+2、mcp alert→G+7、listen/ping→WS 原生）；`dev-loop-handoff` V10→WS / V11→G / V27→G（+2 消费端）映射正确；**contractBlocked=空 断言成立**（待确认点 3 已实证销账、点 1 复验放包 2 验收而不 gate 妥当、点 2 传输无关结论沿用）。G/WS 均为合法切片（G 为服务端基建切片、类比包 0 的 L0/internal-service 例外，其"独立验证"=部署+鉴权+首验探针而非用户故事）。

8 条建议级修正集中在「计数一致性」与「归属/接缝精度」两类，无一伤切片骨架。

### 维度速览

| 维度 | 结论 |
|---|---|
| A 覆盖性 | 完备；F2（网关侧兜底逻辑归属）、F3（决策 26 分组 producer）两条归属澄清 |
| B DAG/mergeOrder | 无环、拓扑有序、8 级关键路径成立；无修正 |
| C 切片合法性 | G/WS 合法；F4（包 1 承接映射漏名 loopback） |
| D 风险/门 | 门簇完备；F6（前置门③ ASR 误标）、F7（display 复验双写）两条精度 |
| E 冲突面 | 常量互斥型断言成立；F5（TP-A 冻结面漏列） |
| F 交接 | F8（ESCALATE merge 缺口留根提示）；其余留根已覆盖 cert-deadlock/multiturn-continuity/401 复核注 |
| G 可执行性 | G 偏大但内聚（tracer-bullet），不主张拆；包 2 为 B 段最大包，建议明确子验收里程碑 |

### Findings 明细（8 条，全建议级）

1. **F1（计数 off-by-one，需冻前钉死）**：§2 标题写「14 包：12 live + S merged + 1 abandoned」，但 §2 表实有 **15 行**——13 live（0/G/WS/2/3/4/5/6/8/7/U/9a/9b）+ S merged + 1 abandoned；§6「12 live 包内容更新」「落 14 pod」同错。这是 v0.2「文头 11 包 vs 盘上 12」同款（progress.md 已记先例）。改法：标题改「15 包：13 live + S merged + 1 abandoned」，§6 改「13 live / 落 15 pod」，§0「11 包保留」对账为「12 保留 + 1 作废 + 2 新建 = 15」（注意 2/7 重写是保留的子集、"保留/作废/新建"与"重写/小调"是两个正交口径，建议只留一套计数），decision-coverage 映射与 board 计数同步。

2. **F2（网关侧兜底逻辑归属模糊）**：spec §4.0 明确 4s 安抚句是「流式链路实现归属：网关定时器」、错误兜底与 V32 超句截断分屏均为「网关处理」（服务端）。但本稿 G 只列「安抚句/『网络出小差了』兜底**配置**」（仅字符串），包 2 列「网关兜底**消费端**」（消费端=设备侧）——三者中谁写网关侧定时器/错误捕获/截断代码未落包。改法：二选一并明示——(a) 把 4s 定时器、错误兜底生成、V32 截断分屏都归 G（与 V27 机械校验同处网关 push 面、同批 gate）；或 (b) 显式扩包 2 云侧范围、把「消费端」改名为「网关兜底生成 + 设备消费」并在 dependsOn 注明依赖 G。不落在「G 只配字符串、2 只消费、生成逻辑无主」的缝里。

3. **F3（决策 26 分组呈现 producer 落包未明）**：包 5（home-p1）标「板」、只做「分组呈现**消费**」，但「分组由 OpenClaw 算好」的编排侧（周中学校课合并「上学」取首节、培训班/一次性逐项）没显式落包。改法：把该 OpenClaw 侧 computation 显式落到包 6（schedule OpenClaw 侧，与「QQ 自改写即时推」同源）或包 2（基础 system prompt 扩展）；否则 producer 无主。

4. **F4（包 1 承接映射漏名 OpenClaw loopback）**：原包 1 范围 = 四接口补全 + 设备鉴权 + **OpenClaw loopback 转发**。承接映射写了「设备鉴权→G」「四接口→方向 B 协议剖面」「alerts 出队→7」，但 loopback 转发未点名。改法：补「OpenClaw loopback 转发 → G（provider `type:openai` 接 OpenClaw、真 token 不出内网）」，防歧义。

5. **F5（行为碰撞型漏列 TP-A 冻结验收面）**：行为碰撞型只列 Pod S。但 TP-A（260831-cloud-link-bringup）的冻结验收 `server/acceptance/test_cloud_link.acceptance.py` 同样承载 HTTP shim 形状断言（CLB-A7「server 起最小 shim，POST /v1/chat 一路跑通」，CLB-E2「shim 保持薄 ~200 行、承载 ASR/TTS HTTP 口子」），且与 Pod S 共享 `server/shim.py`（正是 WS 退役对象）——WS 归档 shim.py 后该文件若再跑会红。改法：把 TP-A 冻结验收纳入行为碰撞型扫描，与 shim.py 归档同批做退役/supersede 处置（或明示方向 B 退役吞并之）。已核实 TP-B（tools/asset-pipeline，资产）与 TP-C（tools/simulator，LVGL）不载 shim/HTTP 形状、不撞。

6. **F6（G 前置门③ ASR 开通误标为门）**：§4「G launch 前置门簇」③写「阿里云 ASR 开通（部署期动作，G 内部步骤，非外部门）」自相矛盾——既是"前置门"又自述"G 内部步骤、非外部门"。改法：把③移出"前置门簇"、保留为 G 范围项（G 已列「ASR 阿里云云端开通与首验终选」）；前置门簇只留①②两项真外部门/人批门（① 18789 硬前置、② cert-deadlock 人批）。

7. **F7（display 遵循度复验双写，边界未明）**：「display 遵循度复验**基础**」（G）与「display 格式遵循度复验（契约待确认点 1）」（包 2）并存，未说明分工。改法：明示 G = 切分 + 机械校验基建（V27 确定性单测）、包 2 = 真 OpenClaw 遵循度复验（统计性 9/10 口径）+ 消费端；避免两包各假设对方做复验。这同时把 D 轴「待确认点 1 复验放包 2 验收而不 gate」坐实为妥当——契约已冻结、复验是实证非确认，包 2 owner system prompt + 复验，失败在包内 resolve，不该 gate-block。

8. **F8（ESCALATE merge 缺口留根提示）**：G/WS 均 high + 跨系统 + 安全轴，merge 时大概率再次命中 supervisor-verdict-gate 判 ESCALATE；ARQ-260902-escalate-merge-no-human-path 未结（human 批准的 ESCALATE 无机械合并路径）。改法：§7 留根补一条「G/WS merge 前确认 human 批准路径就绪」，其余既有留根（cert-deadlock、multiturn、401 复核注）已覆盖到位，无漏。

### 补充确认（非发现、仅销账）

- V18（滞留弹窗顶替）在 S-F1.4 弹窗形态为行为细则，包 7 scope 已含「驻留弹窗 + 排队逐个弹」，建议落地时在 acceptance 补 V18 顶替断言（建议级、可随 F 级一并回灌，不单列）。
- V26 共享 token（G 服务端 constant-time 比对 + WS 配网生成写入）、V28 18789、V31 录音回退、决策 21 功耗 DEFER（9b 三要素）、决策 25 故事流式（包 8 manual 聆听 + 网关累积）、决策 27 提醒播报（包 7 三件套 + speech 预合成）均已正确落包；V29 出网记账系 human 接受性记账、非实现项，无包属正确。

### sign-off

architect（单轮 dev-side review）2026-09-03：**GREEN-with-fixes，8 条建议级修正（无阻断级、无回问 human 项）**。F1（计数）建议冻前钉死，其余 7 条为归属/接缝精度，主线程回灌后可 human 盖冻结；盖时同步 §6 的 board/decision-coverage 计数（随 F1 校正）与 §8 评审记录。
