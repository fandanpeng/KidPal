# PROPOSAL.template · Track B 提案模板（canonical · 260802-gov-review-track-b-hardening T1 落地，OQ2=(b) 选址 docs/templates/）

> 用法：复制本件为 `delivery/changes/arch/<change-id>/PROPOSAL.md`，替换 `<尖括号>` 占位；**带【硬项】的小节是义务不是建议**——缺失即判官 BLOCK 的合法理由。
> **先起名**：目录名必须命中 `.claude/scope-map.json` review-agent 写槽 token——governance-lane = `<YYMMDD>-gov-review-<slug>`、spec-lane = `<YYMMDD>-arch-review-<slug>`；缺 token 时判官/核验官制品写入被 enforce-scope 拦截，起草期 `changes-index.mjs --upsert <id>` 亦点名（见 protocol「双轨路由」节命名条款）。
> 文体：散文节按 AGENTS.md「输出标准」节写通顺完整的句子；Status 凭据行、landing-manifest 围栏块、packet-binding 行等机器校验结构保持精确格式。
> 形制来源：Track B 前四乘客 PROPOSAL 实际形状归纳（auth-axis / unit-gate / verifier-dispatch / claude-consistency）+ 四乘客 retro §4 建议第①③④类固化 + 本批 R1/R2 评审修订（硬项 · 目标文件锚清单）。
> 结构沿革：260920-gov-review-proposal-structure-merge——原「问题与根因」节并入本节、条目改五段固定式、四【硬项】标签去数字（数字原为旧节号镜像，合并后 `1a`/`2a` 同落一节即自相矛盾）；节号保持 `2` 不变（机械门定位锚）。

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
<一行一个目标路径；恰 = §2 各 T 目标文件锚清单并集（条件锚按裁断裁剪；迁移锚只记迁入路径）；一行一个**文件**路径——目录须展开为成员枚举（landing 门不消费目录锚，目录简写收口期必红）>
```

---

## 0. 想实现的效果与差距

目标效果一句话 + 现状差距 + 非目标（本批不做什么，防 scope creep）。

## 2. 问题、根因与改法（T1…Tn）

每 T 一节，五段固定式：先写「问题」与「证据」，再写「根因」，再写「改什么 + 怎么改 + 为什么这样改（对哪个根因）」。扩承重闸优先、克制新立脚本。

**【硬项 · 起草期实证义务】**（原「硬项 1a」，随原「问题与根因」节并入本节；标签去数字）本节的断言纪律：

- 凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；无实测不写断言，写「未实测」。
- 凡声称「对偶 / 比照 <先例>」→ 先读先例文件，引 file:line verbatim 确认形态再落笔。
- 凡「全 / 均 / 无」量化声明 → 每次改写其覆盖集合时，对新入集成员重新取证并留痕。
- 凡引用 commit SHA → 先 `git rev-parse <sha>` 核实存在，输出留痕。
- 凡数字引用（计数 / 行号 / 版本号等）→ 起草期现跑实测（命令 + 输出留痕）或核对权威原处；不得转引陈旧制品数字，确需转引时显式注出处与复核结果。
- 凡引用脚本 / 闸 / CLI 命令入口（如 `--self-check`、闸调用命令）→ 起草期亲核入口存在（实跑一次，或至少验证该入口在脚本帮助/源码中），不得凭印象构造。
- 搬运 / 改写既有文本中的引据（file:line / 编号 / 数据点）视同新断言复证——版本迭代搬运期的旧引据漂移不得免检。

**【硬项 · 目标文件锚清单】**（原「硬项 2a」）每 T 末尾显式列「目标文件：<仓根相对路径清单>」（该 T 落地时要改/新增的仓内文件，新增/改分注，条件触发的标触发条件）。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单：**缺锚清单 = 门 RED 点名缺锚（fail-loud，不假绿）**。只认「路径出现」不含「改/读」语义的提取规则在真实 PROPOSAL 上歧义爆炸（260802-gov-review-track-b-hardening R1-F1 活例实证），故锚清单为义务。承重 landing 指令（箭头替换的目标文本 / 联动指称）必须原文引用钉死并给出替换后完整句，禁止转述（「那行」「上面那句」类两读指称不许）。改法标题一律 `### T<n> · <标题>` 标题形态——机械门 `parseSection2AnchorLists` 只认 `### T<n>`（260906 批 FROZEN 后格式返修实证，粗体段形态不被机辨）；landing-manifest 围栏块一行一个裸路径，不带括注（`extractManifestBlock` 逐字取行）。（两句为 260907-gov-review-arq-governance-batch-2 T8 钉死。）

**条目形态（五段固定式，照此书写）**：

```
### T<n> · <问题的短名>
**问题**：<完整陈述——是什么 + 影响面 + 发生频率>
**证据**：<file:line verbatim / 实测输出 / 制品锚点>
**根因**：<为什么是这样>
**改法**：<改什么 + 怎么改>
目标文件：<裸路径，反引号包裹>（改）· <裸路径>（新增）
```

**读法注（形态约束 · 260920-gov-review-citation-anchor-residue 收口后定稿）**：

1. **第五段以 `目标文件：` 起头、不带任何前缀、不加粗**——本节示范块里的形态**就是可照抄的正形**。**形态沿革**：v0.1~v0.2 期间该行曾带 `〔第五段·锚清单〕` 变形标记，那是**为绕开解析器不跳 fenced code block 的缺陷而加**（彼时若示范行以 `目标文件：` 起头，它会被当作**真锚行**吃进锚集）。该缺陷已由 `260920-gov-review-citation-anchor-residue` T3 子项 b 修复——`parseSection2AnchorLists` 与干跑探针现**都跳围栏内**，故变形标记同步撤销。**该批起草期实测的教训值得记**：变形标记只存在了两批，就被起草者当正形照抄进真实 PROPOSAL（该批自曝，`--preflight` 三条锚行全落空）——**模板里的示范必须与门同形，任何「为绕开机器而加」的变形都会变成下一批的错误正形**。
2. **该行不得加粗**——锚行判据是 `/^目标文件\s*[:：]/`（仅 strip 前导 `>` 后按**行首**匹配），写成 `**目标文件**：` 即不被识别 ⇒ 门 fail-loud 报「缺锚清单」。**这正是 `ARQ-260914-landing-preflight-anchor-parse` 记录的原始错误形态**（该批起草期把该行写成加粗，`--preflight` 当场报 `bold-anchor-line`）。
3. **锚行注记不引路径 token**（260912 拆包链批 near-miss + 该批 landing-verdict D4 处置意见成文，落点由 `260920-gov-review-citation-anchor-residue` T3 搭车件承接）：`parseSection2AnchorLists` 会把锚行里**所有** canonical 路径形态的 backtick token 并入锚集——**注记（`（改）`/`（新增）` 之类的括注）里回引任何路径，都会被机器当成一个真锚**，造成 §2↔manifest 假失配（fail-loud 卡死）或在 manifest 偶然含它时**静默放行**。⇒ **路径 token 只出现在真锚列，注记只写人读语义**。起草期用 `--preflight` 的 `anchorUnion` 列（并集**成员名单**，非仅个数）逐 token 核一遍即可当场看出。
4. **含反引号 / `${}` / 转义序列的承重代码行必须用围栏代码块承载**（260924-gov-review-settings-deny-contract-merge R1-F1 实例注）：禁 blockquote + code-span 转义形态——转义伪影一旦 byte 照抄进门脚本 = SyntaxError 级损坏；围栏承载同时保 byte 保真与渲染稳定。

**其余四段加粗只作视觉分隔、不承载机器语义，故可自由。** 第 1、2 条合起来是「**形态必须与机械门同形**」的具体落地：模板给出的示范形态，就是未来每份 PROPOSAL 会照抄的形态——所以模板里的示范必须与门同形。

**【起草期自检两句】**（260918-gov-review-batch-drafting-friction T2 钉死；260920-gov-review-citation-anchor-residue T3 扩充；260924-gov-review-gate-parse-teeth T9 扩第三句——名称沿革保留、实为三句）① **FROZEN 标注前跑一次 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>`**——只读干跑，报两类**形态**错（加粗锚行 `**目标文件**：`、无锚清单含整行缺失），**并输出 `anchorUnion` 即锚并集成员名单**（对照读法注第 3 条逐 token 核注记有无夹带路径记号；报出的行号是**文件绝对行号**，直接可定位）**与 `advisories` 目录 token 提示**（对照 manifest 说明行），**不参与任何门判定、不进任何闸链**；**并通过 §5 验证集命令实跑**（不实跑的验证集 = 「冻结可判性」停留在纸面，260914 批实测瑕疵），且 §5 每条断言**标注评估时点（pre-commit / post-commit）**——双时点标注的验证集须**逐子句**注明归属时点（批 B A2 实例：幂等类「复跑后 diff 为 0」在 pre-commit 恒假）。② **路径类变更先查 `.claude/references/path-form-family.md` 的形态族谱表**——一仓路径至少有八种文本写法，逐类残留清扫子检模式与三类替换风险注记见该表（260914 批因路径批改付了四轮评审 + 落地四轮修复，其中最隐蔽的一类四轮机检全漏）。③ **承重引文 / 插入文本落盘后 byte 对账**——锚文与承重插入段一律机械提取（`grep -o` / 单行 Read 后复制）+ 落笔前后各一次 byte 比对，禁长输出目视转录；工具负结果（Edit not found / 探针 -1）先用第二信号源交叉再归因（ARQ-260914 追注 (B) 建议原文；批 A R1-F1/F2 + 批 B R1-F1/核验 F1 四实例在册）。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| ... | ... | 新增/G2/G3/未触 | 消费方写清 |

**【硬项 · sweep 透镜义务】**（原「硬项 1b」）若本批落地新门/新断言：以其**判定 pattern** 全仓预跑，原始输出（或摘要 + 命令 + 日期）进本节证据段；「哪些包/文件有 X」类 census 清单一律机械枚举生成（Glob/Grep/脚本，注明命令），判官只需 spot-check。枚举 census 遇工具截断/省略显示形态（如「Omitted long matching line」）必须逐条展开（`-o` 只看匹配片段 / 定点 Read 该行）再计数，枚举声明须注明展开取证处。

**【硬项 · 复扫模式完备性先验】**（原「硬项 1c」）若涉退役语面/残留清扫：模式集**显式枚举全形态**（词形变体 × 有/无 backtick × 目标词），附一句完备性论证；不靠判官评审期人工扩模。

## 4. human 取舍点（OQ）

每个 OQ：选项 (a)/(b)/(c) + 推荐位 + 推荐理由一句话。该 human 裁的（挂载面扩张、全会话新机制、不可逆性）必须给全。

## 5. 验证集（landing 前机械前置 + landing 后核验）

每条机械可判：命令 + 预期。自检/重放类遵守真空绿规约（重放先断言输入非空；自检验证输出含新增自检名 + 调用顺序审查；环境红先立基线）。

## 6. 留根清单

不关的根 + 交日落复盘项 + 登记不硬治项 + 自律件（不进模板的环内纪律）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）