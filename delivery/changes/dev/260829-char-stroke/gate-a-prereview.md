# 闸 A 契约预审结论 · TaskPackage 260829-char-stroke（C1 静态字库方案）

> architect-reviewer 只读预审 · 2026-09-07 · 由 planner 代为落盘存档（预审 agent 只读、不写文件）
> 裁定：**④ 无需改契约（NO_CONTRACT_CHANGE）**，机制归实现侧适配（IMPLEMENTATION_ADAPTER 性质），doNotEscalateToHuman: true——不需要人类契约门出場。

## 一、四分类裁定

**裁定：④ 无需改（NO_CONTRACT_CHANGE）**，依据（落到具体契约条文）：

1. **C1 不改变任何跨边界 wire format**。契约对 char 场景的唯一格式约束是 §1.3①（46-63 行）`char` view 的 data 全量 D3 形状（char/pinyin/radical/strokeCount/strokes/medians/structure/meaning/words）和 §1.5 第 3 条网关机械校验。C1 只改变**数据由谁生产**（LLM 逐 token 复述 → OpenClaw 侧静态字库查表拼装），不改变**跨网关下发的块长什么样**——拼装后到达设备侧的仍是 §1.3① 定义的全量 D3 JSON，逐字一致。
2. **§1.6 现文与 C1 无冲突**。§1.6（148-149 行）现文为：「在调 `/v1/chat/completions` 时，通过 system prompt 让 DeepSeek **始终**按此格式回复：> "你是儿童学习伙伴。每次回复分两部分：先用口语化、适合8-12岁孩子的话回答（这部分会被念出来）；然后在末尾附一个 ```display 代码块，内含屏幕显示指令 JSON，view 取 char/schedule/story_list/story_text/text/home/control 之一。"」——它约束的是**回复的两部分格式与 view 白名单**，从未约束「display 块内的数据必须由 LLM 自己生成」。C1 下 OpenClaw 回复末尾照样附含全量 D3 的 ```display 块，§1.6 描述的行为不变。需要改的只是 system prompt v1 里「增量扩展区」的包 4 占位措辞（v1 第 60-61 行：「查字场景按契约 §1.3 ① 产出完整笔顺数据」）——该文件在 `server/deploy/openclaw-system-prompt.v1.md`，是**实现面部署物、非契约面**，按版本化落盘规则出 `.v2` 即可，不触 `docs/contracts/**`。
3. **§1.5 薄桥红线反而被 C1 强化**：拼装点在 OpenClaw 侧（网关上游），网关保持零自研业务逻辑，符合 §1.5「薄，零自研业务逻辑红线不变」与 CHR-E3。
4. **反证检查通过**：无字段映射需求（D3 形状原样复用）；无本地命名 vs canonical 名冲突（「最小块」是 OpenClaw 内部约定，非跨边界形态）；未夹带二期能力；char view 类型原样复用。

## 二、逐点分析

### 2.1 §1.6 措辞：措辞级关切、契约级无需动

关键判断：§1.6 是**接口行为约束节**（OpenClaw 对网关发出什么），不是**数据生产方式节**（OpenClaw 内部怎么造出这个数据）。C1 改的是后者。若把「LLM 只出意图＋字、静态字库拼装」写进契约，反而犯「把 OpenClaw 内部实现机制塞进端云边界契约」的错——契约只需保证：到达网关的 ```display 块是 §1.3① 全量形状。现文已保证这一点。**结论：契约 §1.6 零改动**；措辞调整全部落在 system prompt v2（见第四节建议文）。

### 2.2 「最小块」是否需要契约级定义：不需要，且必须不越过 OpenClaw→网关边界

这是本闸的核心问题，两种形态的裁定：

**事实核查（读了 `server/gateway/display_schema.py` 全文）**：该文件目前对 char **没有 per-view data shape 校验**——只校验 view 白名单、data 必须是 JSON object（char 缺 data 会拒）、64KB 大小上限、control 的 action 表。也就是说 `{"view":"char","data":{"char":"勇"}}` 这种最小块**当前会通过网关机械校验**并被转发到设备（板子 P4 查字页拿不到 strokes/medians 会渲染失败）。

**裁定**：

- **契约要求、也是唯一合法的跨网关形态 = 拼装后全量块**。按 C1 架构（静态字库挂 OpenClaw 侧 ECS），拼装点在 OpenClaw agent 回合内（LLM 出意图＋字 → OpenClaw 侧查表工具拼出全量块 → 全量块进回复正文），网关看到并校验的是**全量块**。§1.3① 全量 D3 形状同时覆盖设备←网关和网关←OpenClaw 两段边界，单一 canonical 形状、无歧义，契约无需动。
- **「最小块」不进契约**。它只是 DeepSeek 与 OpenClaw 侧查字工具之间的内部约定（prompt/工具协议），存在于 system prompt v2 与工具实现里。把它定义进契约会 (a) 把 OpenClaw 内部机制泄漏进边界契约、(b) 制造「网关到底收到哪种形状」的双形态歧义、(c) 迫使网关为最小块新增透传或改写逻辑——直接违反 §1.5 薄桥红线（网关改写 display 载荷 = 自研业务逻辑）。**存疑向严的裁定是：最小块必须终止于 OpenClaw 回合内，绝不以 wire 形态出现在网关入口。**
- **失败路径的鲁棒性缺口是实现问题、非契约问题**：若 OpenClaw 侧工具故障导致 LLM 直出最小块，现网关校验会放行。补法是在网关为 char 增加必需字段校验（要求 char/pinyin/radical/strokeCount/strokes/medians 在场）——§1.5 第 3 条本就明文要求「data shape/大小上限」机械校验，补严只是**履行既有契约条款**，不是改契约。**一个必须向 planner 显式提示的坑**：`display_schema.py` 文头自述为「Frozen acceptance GWA-A5. Frozen offline contract」，动了 `validate_display` 的判定语义可能撞 G 包冻结验收——要么在本包内走 acceptance-author 的 supersede 处置（PROPOSAL §4 行为碰撞型同款流程），要么 MVP 先不改网关、靠 prompt 纪律 + 板子侧对缺字段做防御性降级。这是 planner 的包内取舍，不构成契约门事项。

### 2.3 D3 笔顺 JSON 格式本身：确认零改动

strokes SVG path + medians 骨架点、makemeahanzi 数据源（9574 字，已实测跑通，spec 决策 12「板子只渲染」）均不在 C1 触碰范围内。CHR-A2~A7 的固件消费面、坐标变换、教学字体红线全部原样。spec §3.0.2 / §4.3 / §5.6 与契约无漂移——spec 241 行现文「DeepSeek 判意图=查字，提取"勇"，查 makemeahanzi 组笔顺 JSON」本就是查表拼装口径，与 C1 一致。

## 三、ContractChangePacket

**不需要**——四分类判 ④，无契约面改动，不产 packet、不进契约门、不动 `docs/contracts/**`。

## 四、给 system prompt v2 的措辞建议（planner 直接引用）

v2 中「增量扩展区」包 4 占位（v1 第 60-61 行）建议替换为：

> **包 4 — char（查字）扩展**：查字场景你只负责判断「孩子要查字」的意图并从话中提取**目标单字**；完整笔顺显示数据（pinyin/radical/strokeCount/strokes/medians/structure/meaning/words，契约 §1.3 ①）由本地查字工具自动查库拼装。**禁止自己生成或复述 strokes/medians 等坐标数据**；display 块中 `view:"char"` 的 data 必须原样使用查字工具返回的完整结果，不得手写、截短或省略字段。语音部分照常口语讲解字义与笔数，三句以内。

注意：具体「LLM 出意图＋字 → 工具拼装 → 全量块入回复」的机制（OpenClaw 工具调用 / skill / 预处理管道）由 planner 按 OpenClaw 实际能力定，措辞须与所选机制对齐；prompt 文件本身在 `server/deploy/`，版本化落 `.v2`，属包内实现变更。

## 五、对 planner 的下一步建议

1. **无 blockingDecision，契约门不出場**：本预审结论为 ④ 无需改，闸 A（PROPOSAL §4 / CHR-E3）就此销账，可正常 launch。建议把本结论文本钉进 TaskPackage 执行记录（append，不覆盖）。
2. **包内待办三件**（均非契约事项）：
   - system prompt v2 按第四节措辞落盘（机制对齐后定稿），并保持「验收所测 = 生产所用」纪律（live 探针以 v2 为唯一 prompt 源）；
   - 裁定是否给网关 char 加必需字段校验：加则按 GWA-A5 冻结验收走 supersede 处置，不加则板子侧对缺字段 char 块做防御性降级（二选一，包内记录即可）；
   - CHR-A8 验收断言「逐字对照数据源正确」应打在**拼装产物**上（工具输出 = makemeahanzi 原文），而非 LLM 输出。
3. **提醒一句边界**：若后续实测发现 OpenClaw 工具机制无法在回复正文内可靠注入全量块（即拼装点被迫下沉到网关），那才是真正的契约门事项（§1.5 薄桥红线被破）——届时按四分类重新进预审，不适用本结论。

关键证据文件：`docs/contracts/interface-contract.md`（§1.3①/§1.5/§1.6）、`server/gateway/display_schema.py`、`server/deploy/openclaw-system-prompt.v1.md`（第 58-63 行增量扩展区）、`.planning/kidpal-mvp/PROPOSAL.md`（§4 闸 A 条款）、`.claude/taskpackages/260829-char-stroke/taskpackage.json`（CHR-A8/CHR-E3）。
