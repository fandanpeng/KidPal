# PROPOSAL.template · Track B 提案模板（canonical · 260802-gov-review-track-b-hardening T1 落地，OQ2=(b) 选址 docs/templates/）

> 用法：复制本件为 `changes/arch/<change-id>/PROPOSAL.md`，替换 `<尖括号>` 占位；**带【硬项】的小节是义务不是建议**——缺失即判官 BLOCK 的合法理由。
> **先起名**：目录名必须命中 `.claude/scope-map.json` review-agent 写槽 token——governance-lane = `<YYMMDD>-gov-review-<slug>`、spec-lane = `<YYMMDD>-arch-review-<slug>`；缺 token 时判官/核验官制品写入被 enforce-scope 拦截，起草期 `changes-index.mjs --upsert <id>` 亦点名（见 protocol「双轨路由」节命名条款）。
> 文体：散文节按 AGENTS.md「输出标准」节写通顺完整的句子；Status 凭据行、landing-manifest 围栏块、packet-binding 行等机器校验结构保持精确格式。
> 形制来源：Track B 前四乘客 PROPOSAL 实际形状归纳（auth-axis / unit-gate / verifier-dispatch / claude-consistency）+ 四乘客 retro §4 建议第①③④类固化 + 本批 R1/R2 评审修订（硬项 2a）。

---

# PROPOSAL · <change-id>

> **状态：<DRAFT vX.Y | FROZEN vX.Y> · <date> · <阶段说明>**（FROZEN 行须含凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果；主线程凭据自冻，授权记 Status 行）。
> **承载 ARQ**：`<ARQ-id>`（根/批次关系写清；不关的根写留根指针）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。Track A 触发时改标 Track A + 命中判据。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（<T 编号范围>）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：<low | low-medium | medium>（一句话证成：闸向严/行为保持/触点面）。
> **review profile**：<G2 | G3 | G2+G3>（触面枚举；G1/契约面必须为零或显式升级）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
<一行一个目标路径；恰 = §2 各 T 目标文件锚清单并集（条件锚按裁断裁剪；迁移锚只记迁入路径）>
```

---

## 0. 想实现的效果与差距

目标效果一句话 + 现状差距 + 非目标（本批不做什么，防 scope creep）。

## 1. 问题与根因（地面真值）

每个问题类一节：证据（file:line verbatim / 实测输出 / 制品锚点）+ 根因 + 实证次数（若跨环共性）。

**【硬项 1a · 起草期实证义务】** 本节的断言纪律：

- 凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；无实测不写断言，写「未实测」。
- 凡声称「对偶 / 比照 <先例>」→ 先读先例文件，引 file:line verbatim 确认形态再落笔。
- 凡「全 / 均 / 无」量化声明 → 每次改写其覆盖集合时，对新入集成员重新取证并留痕。
- 凡引用 commit SHA → 先 `git rev-parse <sha>` 核实存在，输出留痕。
- 凡数字引用（计数 / 行号 / 版本号等）→ 起草期现跑实测（命令 + 输出留痕）或核对权威原处；不得转引陈旧制品数字，确需转引时显式注出处与复核结果。
- 凡引用脚本 / 闸 / CLI 命令入口（如 `--self-check`、闸调用命令）→ 起草期亲核入口存在（实跑一次，或至少验证该入口在脚本帮助/源码中），不得凭印象构造。
- 搬运 / 改写既有文本中的引据（file:line / 编号 / 数据点）视同新断言复证——版本迭代搬运期的旧引据漂移不得免检。

## 2. 改法（T1…Tn）

每 T 一节：改什么（文件/段落）+ 怎么改 + 为什么这样改（对 §1 哪个根因）。扩承重闸优先、克制新立脚本。

**【硬项 2a · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」（该 T 落地时要改/新增的仓内文件，新增/改分注，条件触发的标触发条件）。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单：**缺锚清单 = 门 RED 点名缺锚（fail-loud，不假绿）**。只认「路径出现」不含「改/读」语义的提取规则在真实 PROPOSAL 上歧义爆炸（260802-gov-review-track-b-hardening R1-F1 活例实证），故锚清单为义务。承重 landing 指令（箭头替换的目标文本 / 联动指称）必须原文引用钉死并给出替换后完整句，禁止转述（「那行」「上面那句」类两读指称不许）。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| ... | ... | 新增/G2/G3/未触 | 消费方写清 |

**【硬项 1b · sweep 透镜义务】** 若本批落地新门/新断言：以其**判定 pattern** 全仓预跑，原始输出（或摘要 + 命令 + 日期）进本节证据段；「哪些包/文件有 X」类 census 清单一律机械枚举生成（Glob/Grep/脚本，注明命令），判官只需 spot-check。枚举 census 遇工具截断/省略显示形态（如「Omitted long matching line」）必须逐条展开（`-o` 只看匹配片段 / 定点 Read 该行）再计数，枚举声明须注明展开取证处。

**【硬项 1c · 复扫模式完备性先验】** 若涉退役语面/残留清扫：模式集**显式枚举全形态**（词形变体 × 有/无 backtick × 目标词），附一句完备性论证；不靠判官评审期人工扩模。

## 4. human 取舍点（OQ）

每个 OQ：选项 (a)/(b)/(c) + 推荐位 + 推荐理由一句话。该 human 裁的（挂载面扩张、全会话新机制、不可逆性）必须给全。

## 5. 验证集（landing 前机械前置 + landing 后核验）

每条机械可判：命令 + 预期。自检/重放类遵守真空绿规约（重放先断言输入非空；自检验证输出含新增自检名 + 调用顺序审查；环境红先立基线）。

## 6. 留根清单

不关的根 + 交日落复盘项 + 登记不硬治项 + 自律件（不进模板的环内纪律）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
