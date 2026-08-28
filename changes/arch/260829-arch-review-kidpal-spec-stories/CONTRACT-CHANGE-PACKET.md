# CONTRACT-CHANGE-PACKET · 260829-arch-review-kidpal-spec-stories · T2

> Gate: 260829-arch-review-kidpal-spec-stories（契约门子步骤，PROPOSAL T2）
> Date: 2026-08-29
> Status: PRECHECKED（architect-reviewer 四分类已给出，human 尚未签字）
> 唯一变更面：`docs/contracts/interface-contract.md` §1.3 ③ `story_list` —— `data` 内新增 additive 可选字段 `total`（整数，故事总数 = 故事果计数）。

---

## 1. 变更范围（Gate Scope）

**In-scope（唯一一处）**：
- `docs/contracts/interface-contract.md` §1.3「各 view 类型定义」③ `story_list`（现 [interface-contract.md:79-91](../../../docs/contracts/interface-contract.md)），在其 `data` 对象内新增**一个**字段 `total`。

**Explicitly out-of-scope（本批一律不动）**：
- 契约其余六个 view 类型：`char` / `schedule` / `story_text` / `text` / `home` / `control`（[interface-contract.md:45-130](../../../docs/contracts/interface-contract.md)）。
- `story_list` 自身除 `total` 外的任何字段：不删、不改名、不改 `items[{id,title,date}]` 结构、不加第二个字段。
- shim API 端点（`/v1/asr /v1/chat /v1/tts /v1/alerts`）语义、display 顶层结构 `{view, data, speech_hint?}`、笔顺 JSON（D3）、课表 MD（B1）、提醒文件结构（三）——全部不动。
- 已由 human 裁定砍掉、故无需契约变更的两处：`schedule` 当前课高亮（OQ-1）、`story_text` 生字标注（OQ-2）。
- spec.md 其余章节、固件/服务端实现代码、UI 设计稿。

## 2. 提议 diff（proposedContractDiff）

变更面只属于「displayBlock / view 类型 story_list」的 `data` 载荷；shimApi / strokeFormat / scheduleMD 均无变化。

`docs/contracts/interface-contract.md` §1.3 ③ `story_list` 的 JSON 样例由：

```json
{
  "view": "story_list",
  "data": {
    "items": [
      { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" },
      { "id": "2026-08-25-yonggan", "title": "勇敢的小明", "date": "08-25" }
    ]
  }
}
```

改为：

```json
{
  "view": "story_list",
  "data": {
    "total": 3,
    "items": [
      { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" },
      { "id": "2026-08-25-yonggan", "title": "勇敢的小明", "date": "08-25" }
    ]
  }
}
```

字段说明补一句（落在该 view 的板子行为/字段说明处）：

> `total`（可选整数）= 孩子已录故事总数，即故事果计数。板子据此渲染 P3 右上角故事果计数器（×N）与盖章帧「第 N 颗」。`total` 由云端（stories 元数据）计算下发；缺省时板子回退用 `items.length`。

注：样例中 `total:3` 而 `items` 列 2 条，是有意的——表达「total 是全集计数，items 可为窗口/截断」，两者不强相等。

## 3. 四分类判定

| 变更面 | 分类 | 理由 |
|---|---|---|
| `story_list.data` 增 `total` | **CONTRACT_CHANGE_REQUIRED** | 见下 |

**为什么不是其它三类：**

- **非 `NO_CONTRACT_CHANGE`**：现有契约 `story_list.data` 仅 `items[{id,title,date}]`（interface-contract.md:84-88），无任何承载「故事总数」的字段。spec S-F2.1 要求 P3 右上角「故事果计数器（×N）」（spec.md:329）、S-F2.4 要求盖章帧「第 N 颗」且「N 与 P3 故事果计数器严格同步」（spec.md:357）。**现有契约表达不出这个已批准行为所需的稳定数据源**——这是定义层面的缺口，不是实现读错契约。

- **非 `IMPLEMENTATION_ADAPTER`（关键反证）**：唯一候选的 adapter 是「板子端用 `items.length` 代替 `total`」。该方案**在列表为全集时成立，但不可靠**：`total` 语义是 stories 元数据全集计数（PROPOSAL §6 定「total 由云端算，板子只读」），一旦将来 items 因列表分页/截断/仅下发最近 N 条而不等于全集，`items.length` 与真实总数漂移，P3 计数器与盖章帧「第 N 颗」即失同步——而 S-F2.4 显式要求二者严格同步（spec.md:357）。故不能把它当纯板端 adapter 打发；`items.length` 只能作为 **`total` 缺省时的向后兼容回退**，不能作为正式数据源。这正是需要契约显式给字段的原因。

- **非 `SPEC_CONTRACT_DRIFT`**：未发现 spec 与 contract 相互**矛盾**。spec 只承诺「计数器 + 第 N 颗」的**行为**，未钉字段名/字段位置；contract 只是**缺**该字段，并非写了一个与 spec 冲突的定义。spec §4 引契约处（spec.md:252、:133）与本字段无张力。属「契约滞后于已批准 spec 行为」，归 CONTRACT_CHANGE_REQUIRED 而非 drift。

**最小变更原则核对**：本补丁只加一个可选字段，不重命名任何既有概念、不动 `items` shape、不把二期能力（WebSocket 流式/唤醒词）夹带进 MVP 契约、不借机扩成「故事元数据厚模型」。`total` 复用既有 `story_list` view，仅扩 `data` 载荷，符合最小变更。

**结论：`CONTRACT_CHANGE_REQUIRED`**——触契约文件，且现有契约无法表达已批准的 S-F2.1/S-F2.4 计数行为。

## 4. 影响（Impact）

- **additive / breaking**：纯 **additive、非 breaking**。新增可选字段，零删除、零改名、零类型变更。
- **向后兼容**：**兼容**。旧固件/OpenClaw 不读 `total` 时按既有逻辑跑；新固件在 `total` 缺省时回退 `items.length`（由契约字段说明显式授权此回退）。无强制双侧同步升级窗口。
- **消费方**：
  - 固件 `firmware/kidpal`：读 `story_list.data.total` 渲染 P3 右上角计数器与盖章帧「第 N 颗」；字段缺省时回退 `items.length`。可选读取，不解耦即可上线。
  - 服务端 shim / OpenClaw：生成 `story_list` 时由 stories 元数据补 `total`（云端算）。shim 只是透传 display JSON，无需为 `total` 改 shim 逻辑本身。
- **迁移 / 数据风险**：**无**。无持久化格式变更、无数据库、无既有样本失效。对已跑通的 D3 笔顺链路（makemeahanzi）零影响——本变更不触 `char` view。
- **需同步的文档**：
  - `docs/contracts/interface-contract.md` §1.3 ③（本体改动，走人类契约门落地）。
  - spec.md 侧无需为 `total` 改字——S-F2.1/S-F2.4 行为描述已成立，本字段是其数据源的落地。
  - 契约门证据链：本 PACKET + 待补 `architecture-gate-evidence.md`（骨架 `.claude/references/contract-change-evidence.md`），并登记 CDQ。

## 5. 给 human 的契约门提问

> **请 human 裁决是否批准：`docs/contracts/interface-contract.md` §1.3 ③ `story_list` 的 `data` 内，新增 additive 可选整数字段 `total`（= 孩子已录故事总数/故事果计数，由云端计算下发，板子据此渲染 P3 右上角计数器与盖章帧「第 N 颗」，缺省时回退 `items.length`）。除该字段外契约零改动。批准 / 拒绝？**

## 6. 验证计划（Validation Plan，落地时执行）

- 契约 diff 逐字核对：`git diff docs/contracts/interface-contract.md` 应仅 §1.3 ③ story_list 出现 `total` 新增，无其它 diff（对应 PROPOSAL V2）。
- JSON 样例合法性：含 `total` 的 story_list 样例 JSON 解析通过（对应 PROPOSAL V6）。
- story↔契约一致性：人工核对 S-F2.1/S-F2.4 提到的计数字段与本契约新增一致，已砍功能（当前课高亮/生字高亮）在两侧均无残留（对应 PROPOSAL V5）。
- spec↔contract 机械门本仓暂无脚本兜底（ARQ-260827-contract-mech-gate-gap），上述核对降级人工并如实记录。

## 7. 消费方一览（Consumers）

| 消费方 | 角色 | 对 `total` 的关系 |
|---|---|---|
| 固件 `firmware/kidpal` | 渲染端 | 可选读取；渲染 P3 计数器 + 盖章帧「第 N 颗」；缺省回退 `items.length` |
| 服务端 shim | 透传薄桥 | 无感知；display JSON 原样转发，无需改 shim |
| OpenClaw | 生成端 | 由 stories 元数据计算 `total`，在 `story_list` display 块内下发 |
