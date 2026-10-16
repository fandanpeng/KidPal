# retro · 261015-gov-review-changes-taxonomy（覆盖两波）

> Track B 第 6 步闭环件（波段二后趁热写、先于其后续 upsert）。骨架 = docs/templates/retro.template.md 的精简同形；上文 = 两波 landing-summary（-w1 中间件 + 终态件）。

## 1. 批画像

- **变更性质**：`delivery/changes/` 伞解散——六目录新树（review / intake / retro / archive + 既有三家）+ 生命周期三层语义成文 + debt→intake 晋升桥 + intake INDEX 投影齿。human 2026-10-15 四裁定收敛方向，Track B 主路径，riskTier medium。
- **量级**：manifest 82 件 + 目录级搬移（archive 整树约 300 件）+ 约 50 件活引用改写；评审 R1–R4 四轮（R1 一 SHOULD / R2 三 SHOULD / R3 一 BLOCKING → 熔断 → human 裁「换新判官实例再战 1 轮」→ R4 GREEN）；landing 两波（波段一 a8ee823 抢齿、波段二原子切换）。

## 2. 顺的（保持）

- **牙齿先行**：波段一单拆 T1（残件归位 + 双齿）抢在旧布局分支合并前进 main——执行期内真实旧布局分支（dev/pod-m2）实跑闸取到 `retired-path-merge-block` 单因 BLOCK 证据，防复发价值当场兑现；波段二期间该分支经 Pod 自己的流程落 main，齿未误伤（残留路径已清）。
- **判官纪律扛住了压力**：R1-F2 行号实测偏一 → v0.2 顺修反向 → R2 三法交叉实测纠正 → R3 终裁确认 v0.1 本对——「地面真值亲核」义务两次拦住错误传播；R3 的 BLOCKING（7 件未入 §2、landing 必红）由真机对账（node 直调解析函数）钉死，不是凭感觉打回。
- **熔断机制按设计工作**：三轮未 GREEN 自动触发 human 三选一，human 裁「换新判官实例」后 R4 一轮收敛——余量判断（仅剩机械补列）被证实。
- **V6 全等预期兑现**：起草期 preflight 发现的 16 件新树锚静默丢弃缺口，经 T6 第 5 条扩根 + R3-F1 七件补列两路归零，波段二终态 preflight anchorCount 82 ≡ manifest 82。

## 3. 踩坑（改法已落或留根）

1. **两波程序变体 × 闸机锚互斥（FROZEN 盲区，human 临时裁占位件）**：闸 gov 档 `readGovAnchors` 以 `existsSync(landing-verdict.md)` 判档 6，而 FROZEN 把 landing-verdict.md 设计为波段二终态件——波段一合并必 BLOCK。评审四轮（含两名判官与三次修订轮）都没抓到，落地期才现形。**根因**：程序变体的制品清单新增时，没对既有机械锚做联立核对。**改法**：占位件方案（无 token、deriveLanded 不消费）；**留根**：后续再有「拆波/分期落地」类 PROPOSAL，起草自检应加一条「分期制品命名 × 既有 existsSync 型机锚联立核对」。
2. **faceMap 双形态 + frontmatter 无闭合围栏**：intake lane 解析器落地时实测 6/8 spine 解析失败——CRLF（`---\r`）两件 + 历史手写无闭合围栏两件 + 结构化 faceMap 与 inline 散文并存。**教训**：给「手写自由格式」建机读投影时，解析器必须先对全量存量做兼容性实测，不能只按规范样本写。修复 = CRLF 容忍 + EOF 补围栏（如实登记）。
3. **R1-F2 行号集体偏一**：判官亲核义务执行时用了会漂的取样方法，四处 file:line 全偏、修订轮照单全收反向。**教训**：file:line 断言的亲核要用带行号 Read / grep -n 直测，不接受推算。
4. **cherry-pick 撞上并行 Pod 会话**：收口 pod-m2 残件时 cherry-pick 的 plan.md commit，与并行 Pod 会话自己走完整流程落 main 的同一文件变冗余——主线程做跨 lane 收口前先看 main 是否已被并行会话推进（本例 git log main..branch 立刻可见）。
5. **sed 分隔符撞 Markdown**：`s###`（撞 `###` 标题）、`s|…(arch|dev)…|`（撞交替符）各翻车一次——批量替换选分隔符先扫模式内冲突字符；正则元字符密集处直接用 Edit。

## 4. 量化

- 评审 4 轮 / 修订 3 轮 / findings 合计 1 BLOCKING + 7 SHOULD + 6 NIT（全机械补列与行号类，零方向分歧）。
- 波段一：7 件 / verdict PASS+3A；波段二：82 件 manifest 全量对账 + V1–V9 全绿 + anchorUnion 全等。
- 机械检查基线演进：gate 43→48→49 臂；changes-index 31→41 臂。

## 5. 待办交接

- OQ-A 裁 (b)：本批合并后另起 `/arch-change` 轻批专改 interface-contract.md 三处历史链接指向归档内路径。
- verifier ADVISORY A1/A2（scope-map `_doc` 波段二预告行兑现情况、summary 措辞收敛）已随波段二处理/登记。
- intake INDEX 漂移标 3 件（261014/261012/261001）待下轮 loop-retro 复议（T5 复议触发条款）。
