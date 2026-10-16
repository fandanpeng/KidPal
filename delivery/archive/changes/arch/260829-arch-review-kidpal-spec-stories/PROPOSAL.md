# PROPOSAL · 260829-arch-review-kidpal-spec-stories

> **状态：FROZEN v0.2 · 2026-08-29 · 主线程凭据自冻：human 立项 ack ✓ + human 契约门签字 ✓(story_list.data 加 total)+ architect R1打回→R2→R3 GREEN ✓(2026-08-29)· human 三项裁定 ✓(B1=KEY长按回首页 / M2=P3 KEY双击进阅读 / m5=改设计稿加废弃标注)**
> **契约门凭据**：architect-reviewer 四分类预审 = CONTRACT_CHANGE_REQUIRED(见 `CONTRACT-CHANGE-PACKET.md`),human 批准。落地随本包进行 + 补 `architecture-gate-evidence.md` + 登记 CDQ。
> **承载 ARQ**：无（本批为 spec 功能细化 + 一处 additive 契约字段，非治理改进项）。
> **review 轨**：Track B 主链 + **契约门**（因触 `docs/contracts/**`，命中升级判据①；以「Track B 主链串联契约门」形态走，整包一次评审闭环，契约门作为其中一步）。SSOT：`AGENTS.md`「治理路由」节 + `.claude/rules/contracts.md`。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1 spec + T2 契约）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary + 契约门证据（CONTRACT-CHANGE-PACKET + architecture-gate-evidence）。
> **riskTier**：low-medium（证成：主体是 spec 文档细化 = low；叠加一处 additive 契约字段 `story_list.data.total`——仅新增可选字段、零删改、零 breaking、板子端可忽略不读，行为保持。因触契约面，升 low-medium 闸向严）。
> **review profile**：G2（spec 面）+ 契约面（一处 additive 字段，走契约门四分类预审）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行已在场（见下）。判官随 GREEN 结论核对，缺项打回。

```landing-manifest
docs/spec.md
docs/contracts/interface-contract.md
docs/ui/kidpal-ui-design-v2.html
```

> landing-manifest 变更说明：初版不含设计稿（原非目标「不改设计稿」）；R1 评审 m5 经 human 裁定「改设计稿加废弃标注」，范围扩大纳入设计稿（仅加废弃标注 HTML 注释/figcaption，不动样式结构）。2026-08-29 记录。

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 spec §4 三条功能清单展开成一组**可验收、与 UI 设计稿 v2 锚点对齐、标端云职责**的用户 story；同时为故事果计数补齐一个 additive 契约字段，使 P3 计数器有数据源。

**现状差距**：①spec §4 原为 12 行粗粒度功能点，无验收/无锚点/无端云划分，与已完成的 UI 设计稿 v2 脱节；②契约 `story_list` 缺显式故事总数字段，P3 故事果计数器无稳定数据源。

**非目标（本批不做）**：
- 不做「当前课反白高亮」（human 裁定价值低，见 OQ-1）。
- 不做「故事正文生字自动反白高亮」（human 裁定价值不足抵复杂度，见 OQ-2）。
- 除 `story_list.data.total` 外，不改契约任何其他字段/端点/view。
- ~~不改 UI 设计稿~~（R1 评审 m5 经 human 裁定**改设计稿加废弃标注**，范围已扩，见 landing-manifest 变更说明）、不写固件/服务端实现代码。
- spec §4 以外章节：仅按 R1 评审修复做最小同步（§5.3 键位注、§5.4 P2/P3/P4 条目、§5.6 查字尺寸），不做额外改写。

## 1. 问题与根因（地面真值）

**问题 1**：spec §4 功能需求停留在粗粒度清单，与 UI 设计稿 v2 脱节，无可验收标准。
- 证据：修改前 spec.md 第 250–268 行（F1.1–F3.4 共 12 bullet），每条一句话、无验收、无锚点、无端云划分。
- 根因：spec 初稿先于 UI 设计完成；设计稿 v2 落地后功能层未同步细化。

**问题 2**：P3 故事果计数器（设计稿 `ic-apple` 右上角 ×N）缺云端数据源。
- 证据：契约 `story_list.data` 仅 `items[{id,title,date}]`，无总数字段；设计稿 P3 主视图右上角有「🍎×3」计数器，S-F2.1/S-F2.4 要求颗数同步。
- 根因：契约设计时未把「果子计数」作为显式数据字段。

**本批起草期实证**：
- UI 锚点核对：story 引用的 23 个 `data-od-id` 逐个 `grep` 设计稿验证，全部命中（2026-08-29 实测，23/23 ✓）。
- 契约全面核对：通读 `interface-contract.md` 全文，将 E1 七 view（char/schedule/story_list/story_text/text/home/control）+ 提醒三 逐一对 13 条 story 字段需求核对。结论：除 `story_list` 缺 `total` 外，其余字段对 story 已够用；`schedule` status、`story_text` 生字标注两处潜在缺口经 human 裁定**不做**，无需契约变更。

## 2. 改法（T1…Tn）

### T1 · spec §4 重写为「功能需求 → 用户 Story（与 UI 设计稿 v2 对齐）」

**改什么**：`docs/spec.md` 第 4 节整节替换。

**怎么改**：
1. 新增 **§4.0 共享底座**：语音会话生命周期 7 状态（聆听/思考/播报/喊停/提醒/低电/页点）+ 全局按键手势表，每条钉 UI 锚点 + 端云职责，各 story 复用不重复。
2. **§4.1 课表 F1** 拆 5 story（S-F1.1 常显首页 / S-F1.2 课表页今天明天 / S-F1.3 语音问课表 / S-F1.4 到点提醒 / S-F1.5 家长录入）。
3. **§4.2 故事 F2** 拆 5 story（S-F2.1 列表收集 / S-F2.2 一键录音 / S-F2.3 回看回听 / S-F2.4 成就盖章 / S-F2.5 云端润色·可选）。
4. **§4.3 查字 F3** 拆 3 story（S-F3.1 语音查字看笔顺 / S-F3.2 拼音笔画部首组词 / S-F3.3 演示控制）。
5. 每条 story 统一格式：「作为…我希望…以便…」+ UI 锚点（`data-od-id`）+ 端云职责 `[板]/[云]/[板+云]` + Given/When/Then 验收。

**已经 human 裁定收敛进本版的三处**（非草稿遗留，是已定稿内容）：
- **S-F1.2 砍「当前课反白高亮」**：圆点三态简化为两态（空心=已上完/明天未发生、实心=待上），并注明「设备主场景在家，孩子上课时不在设备旁，故不做现在进行指示」。
- **S-F2.3 砍「生字自动反白高亮」**：阅读态 KEY 专职「念给我听」，问生字走语音；注明砍掉理由。
- **提醒端云归属**：S-F1.4 改「云为主、RTC 为降级」，挂契约待确认点 3，补 text→富弹窗映射说明。

**目标文件**：改 `docs/spec.md`（§4 整节替换；其余章节不动）。

### T2 · 契约 `story_list.data` 加 additive 字段 `total`

**改什么**：`docs/contracts/interface-contract.md` §1.3 ③ `story_list`。

**怎么改**：在 `story_list.data` 中新增可选字段 `total`（整数，故事总数 = 故事果计数）。
- 落地 diff（草稿，最终经 architect-reviewer 四分类 + human 契约门定稿）：
  ```json
  {
    "view": "story_list",
    "data": {
      "total": 3,
      "items": [
        { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" }
      ]
    }
  }
  ```
  并在字段说明补一句："`total` = 孩子已录故事总数，板子据此渲染 P3 右上角故事果计数器（×N）与盖章帧『第 N 颗』；缺省时板子可回退用 items.length。"

**为什么这样改**（对 §1 问题 2）：给故事果计数一个显式、稳定的云端数据源；additive 不破坏既有，板子可向后兼容（缺 `total` 时回退 items.length）。

**目标文件**：改 `docs/contracts/interface-contract.md`（§1.3 ③ story_list 一处）。

> ⚠️ T2 触契约面，**必须先过契约门（architect-reviewer 四分类 + human 签字）才可 landing**；本 PROPOSAL 落的是「提议 diff」，不是已落地改动。

## 3. 影响面 sweep 分类（UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 系统 spec 面 | `docs/spec.md` | G2（改） | §4 整节替换，其余章节不动 |
| 契约面 | `docs/contracts/interface-contract.md` | **契约门（additive）** | 仅 §1.3 ③ story_list.data 加 `total` 可选字段；零删改、零 breaking |
| UI 设计稿 | `docs/ui/kidpal-ui-design-v2.html` | 未触 | 仅引用其 `data-od-id` 锚点 |
| 实现面 | `firmware/`、`server/` | 未触 | 本批纯文档 + 一契约字段 |
| 治理机制 | `.claude/**`、`AGENTS.md` 等 | 未触 | 非本批 |

消费方：固件 `firmware`（读 story_list.total 渲染果子计数；旧固件不读该字段则回退 items.length，向后兼容）、服务端 `server/shim` 与 OpenClaw（生成 story_list 时补 total）、后续开发以 story 为验收依据。

## 4. human 取舍点（OQ）— 全部已裁

| OQ | 问题 | 裁定 | 落点 |
|---|---|---|---|
| OQ-1 | 课表「当前课反白高亮」 | **砍**（在家场景价值低；提醒功能才是重点） | S-F1.2 已删该承诺；schedule 契约不动 |
| OQ-2 | 故事正文生字自动反白高亮 | **砍**（价值不足抵契约复杂度；问生字走语音） | S-F2.3 已删该承诺；story_text 契约不动 |
| OQ-2′ | 故事果计数数据源 | **保留 `total` 字段**（不砍，走契约门） | T2：story_list.data 加 total |
| OQ-3 | 「先落地后补评审」追认 | **待 human 立项 ack 时一并确认**（推荐追认，评审作用于已落地内容，打回再改/退） | 流程形态 |
| OQ-4 | C2 契约字段与 spec story 评审组织 | **并入本包一次走完**（先看 story，再核契约一致性） | 本包形态 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 验证 | 命令 / 方法 | 预期 |
|---|---|---|---|
| V1 | story 引用的 UI 锚点全部存在于设计稿 | 提取 spec §4 锚点 ID 逐个 `grep` `kidpal-ui-design-v2.html` | 全部命中（起草期已实测 23/23 ✓；landing 后复跑） |
| V2 | 契约改动仅限 story_list total 一处 | `git diff docs/contracts/interface-contract.md` 人工核对 | 仅 §1.3 ③ story_list 出现 `total` 新增，无其他 diff |
| V3 | 本批改动文件 = landing-manifest 两文件 | `git status --short` 于 landing 提交 | 仅 spec.md + interface-contract.md（+ 本包制品） |
| V4 | spec §4 结构完整 | 人工核对含 §4.0 + §4.1/4.2/4.3 共 13 条 story，每条带 UI 锚点 + 端云 + Given/When/Then | 13 条齐备 |
| V5 | story 与契约一致性 | 人工核对：story 提到的契约字段（char/schedule/story_list/story_text/提醒）在契约中均存在且无矛盾；已砍功能（当前课高亮/生字高亮）在 story 与契约两侧均无残留承诺 | 一致 |
| V6 | 契约 JSON 样例合法 | story_list 含 total 的样例 JSON 解析通过 | 合法 JSON |

> spec↔contract 机械门本仓暂无脚本兜底（ARQ-260827-contract-mech-gate-gap），V2/V5 降级为人工核对并如实记录。

## 6. 留根清单

- **提醒主链路契约待确认点 3**（OpenClaw 定时能力）：S-F1.4 已标注依赖，留 OpenClaw 对齐时确认。
- **P3 阅读态「问生字」走语音的实测可行性**：留 M4/M5 联调期复核。
- **故事果计数 N 的持久化**：定为云端 stories 元数据（total 由云端算），板子只读；实现细节留 dev-loop。
- **F3.2 组词数据源**：makemeahanzi 是否含组词字段需与 OpenClaw 核对；本批按契约 D3 `words` 引用，留待对齐。
- **契约门证据补齐**：本包 landing 前需产 `CONTRACT-CHANGE-PACKET.md` + `architecture-gate-evidence.md`（骨架 `.claude/references/contract-change-evidence.md`），并登记 CDQ。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
