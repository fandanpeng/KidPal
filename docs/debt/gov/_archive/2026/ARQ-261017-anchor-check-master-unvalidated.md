---
id: ARQ-261017-anchor-check-master-unvalidated
title: spec-anchor-check 假定「最高版号设计稿即完整母版」却不校验完整性——任何人产出一份增量 vN 设计稿即静默打断全仓 UI 锚机检（2026-10-17 实测 12 条悬空、涉 10 条 story）
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一治理批（动 `.claude/scripts/spec-anchor-check.mjs` / 设计母版发布流程 / `uiux-author` 或 write-story 的「母版产出」纪律任一时并批）；或再出现一次「写增量稿静默打断 UI 锚机检」时立即升格
opened: 2026-10-17
closed: 2026-10-27
closedBy: 261027-gov-review-p5-spike-anchor-cluster
paths: [.claude/scripts/spec-anchor-check.mjs]
source: [2026-10-17 主线程在 261016-p3-story-body-sd-cache 需求面流程内实测复现（before/after 对照，见正文证据 2/3）, delivery/intake/261016-p3-story-body-sd-cache.md（该批 spine；事件发生时序与处置留痕）, .claude/scripts/spec-anchor-check.mjs:36-41（currentDesignFile 逐字）]
---

**证据**：

1. **机读定义（逐字，可点开复核）**——`.claude/scripts/spec-anchor-check.mjs:35-41`：

   ```js
   /** 现行设计稿 = docs/ui/ 下版本号最大的 kidpal-ui-design-vN.html。 */
   function currentDesignFile() {
     if (!existsSync(UI_DIR)) return null;
     const files = readdirSync(UI_DIR).filter((f) => /^kidpal-ui-design-v\d+\.html$/.test(f));
     if (!files.length) return null;
     files.sort((a, b) => Number(b.match(/v(\d+)/)[1]) - Number(a.match(/v(\d+)/)[1]));
     return files[0];
   }
   ```

   即：**「现行设计稿」= 目录里版本号最大的那个文件，仅此一条判据**——不校验它是否包含完整画布集、不校验 `data-od-id` 数量、不校验与前一版的继承关系。同文件 `extractOdIds` / 各 story 锚校验均以这个文件为唯一比对源。

2. **实测复现（2026-10-17，before）**——本批 UI 生成阶段，`uiux-author` 为「只加一个新态」而写了一份**增量载体** `kidpal-ui-design-v6.html`（222 行、3 个 `data-od-id`；对照 v5 = 1250 行、60 个 `data-od-id`）。该文件一落 `docs/ui/`，随即被 `currentDesignFile()` 选中为现行稿，`node .claude/scripts/spec-anchor-check.mjs` 输出：

   ```json
   { "design": "kidpal-ui-design-v6.html", "designIdCount": 2, "ok": false,
     "errors": [ "S-F1.1（L238）：UI 锚 `canvas-p1` 在现行设计稿 … 无对应 data-od-id", "…共 12 条…" ] }
   ```

   **12 条悬空锚、涉 10 条 story**（S-F1.1 / S-F1.2 / S-F1.4 / S-F2.1 / S-F2.2 / S-F2.3 / S-F2.4 / S-F3.1 / S-F3.2 / S-F3.3）；逐条核**全部与本批改动无关**（用 HEAD 版 spec 在同一目录重跑得同一 12 条错误集），即**新引入的全仓级假红**。

3. **对照（after）**——把 v6 重建为完整母版（v5 内容 + 新态，61 个 `data-od-id`）后，**同一条命令**输出 `"ok": true`、`"errors": []`、exit 0。before/after 同命令、同目录、唯一变量 = 该文件是否完整 → 因果闭合。

4. **复现配方（任何人可重跑）**：在 `docs/ui/` 放一个 `kidpal-ui-design-v<N+1>.html`，内含任意少量 `data-od-id` → 跑 `node .claude/scripts/spec-anchor-check.mjs` → 可得同形红。**无需任何额外条件，也不需要动任何其它文件。**

**影响面**：

- **静默 + 全仓级 + 归因误导**三重叠加。写增量稿的一方（本例 = `uiux-author` 走 Write 工具撞单次体积上限后的合理变通）**看不到任何即时信号**——它自己跑机检会以为是自己的编辑弄红的，别人跑机检则会怀疑自己的分支。实测中 `story-author` 第二轮报告确曾把这条红**误归因**为「v6 是半成品」之外的自己编辑所致，需额外做归因对照才排除。
- **触发门槛极低**：任何一次「设计母版增量发布」都会触发，而「增量发布」恰恰是大文件写入受限时的自然产物（本例 Write 单次上限实测约 14KB 成功 / 77KB 失败，完整母版约 107KB，**无法单次写出**）。
- **代价形状**：机检恒红期间，**真正的 UI 锚漂移会被淹没在 12 条假红里**——机检失去鉴伪能力。本批因及时发现（当期重建为完整母版）未造成落地事故，但这是运气而非机制。
- **复现频率**：每次「设计稿出新版本」一次；随设计面版本数增长而线性增长。

**建议改动**（候选，交 review、不自批）：

1. **机器侧（治本，改动小）**：`currentDesignFile()` 选出候选后**加一道完整性自查**——例如比对该文件 `data-od-id` 数与前序版本（低于则应报「现行设计稿疑似不完整：v6 仅 N 个画布，前一版 v5 有 M 个」），或对「本文件声明的画布清单」与「实际 `data-od-id` 集」做一致性核。**关键是让失败显式指认「是母版不完整」而不是「你的锚没了」**——把归因误导消掉即达成主要收益。
2. **纪律侧（治标但必要）**：在设计面纪律文面（`uiux-author` agent 定义 / write-story 相关节）明写「**`kidpal-ui-design-vN.html` 命名族 = 完整母版**；增量/局部稿不得占用该命名，或以其它命名存放」。命名即契约——把「能被机检选中」与「是完整母版」绑死。
3. **顺序侧（低成本兜底）**：约定「母版升版号」与「其内容完整」同一次落盘，禁中间态存在（本批实际就是这样收口的：完整母版与出图同一次提交）。

**补充（本条目自身的一条诚实注记，供 reviewer 一并裁）**：本批实际处理方式 = 由主线程读 `uiux-author` 备好的构建脚本后代跑、把 v6 重建为完整母版并同 commit 落盘，**期间 v6 的不完整态从未进入版本库**。故本条目**无「已污染主线的制品」需清理**——它记的是**机制缺口**，不是**待修数据**。

**风险**：riskTier: low——纯机检脚本判据 + 纪律文面，零行为面、零契约面、单批可收。但**触 G3**（`.claude/scripts/**` 属治理机器面），故按 gov-arq 判据 **review profile = full-adversarial + human**，不得美化快路径；硬排除六项逐项：不触 `docs/contracts/**`、无 agent frontmatter、非 architect.md self-def、非 review-loop agent 定义、**非新增/改动门**（本项改的是既有脚本的判据，不新增 gate）、无 spec-contract drift。

**路由**：归 `/gov-change` loop；paths 含 `.claude/scripts/**` = **机器面**，按 AGENTS.md「治理面变更」节**恒 Track B**（gov-change 侧轻路径不适用）。**关联**：与 `docs/debt/tech/ARQ-260918-spec-face-3page-reconcile.md`（同**工具不同根**——该条治 spec 叙述与实现漂移、仅引用本脚本作验证手段；本条治脚本判据自身的覆盖面缺口）建议同批评审时互查咬合。由跨面变更 `261016-p3-story-body-sd-cache` 实施期实测派生（**该变更自身的暴露项，非外部发现**）。

**2026-10-27 · 结项（`261027-gov-review-p5-spike-anchor-cluster`，T2）**：① 声明面（治本）——新增 `docs/ui/UI-DESIGN-VERSIONS.md`（形态镜像 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 先例：声明面记当前 + 全量沿革，其它文面只留指针）：机读 `current:` 指针对当前母版 + v2–v7 逐版备注（git 考据回填，含 v6 增量件事故现场）+ 现行指针依据记录；初设 `current = kidpal-ui-design-v7.html`（v7 = v6 的严格超集、缺失 0；最近两批消费实证与机检基线同指 v7）。② 机器侧——`spec-anchor-check.mjs` 的「现行设计稿」改由声明面 `current:` 指针决定，原文「目录里版本号最大者」的隐式判据废止；声明缺失 / `current` 行缺失或重复 / 指针指向文不存在或不合母版命名族一律 exit 2（输入缺失档），不静默回退最高版号；存在更高版号而声明未跟随 → stderr 提示、以声明为准。③ 完整性自查保留为兜底——现行稿相对「版本号严格小于它的最大者」比 `data-od-id` 集，前序 ≥10 且缺失比例 ≥50% 即判「疑似增量稿占用母版命名」、exit 2 并指认两版锚数与占比；逃逸旗 `--allow-master-shrink` 只用于有意的整版收缩（阈值按真实演进史标定：v3→v4 实测收缩 9.8% 属合法页撤，病态夹具 83%）。两层叠加：增量件占母版命名的旧病由声明面挡（比对源不跟最高版号走），声明面跟进升版时由完整性自查挡（形质不兼备的升版当场拦）。④ 消费面行为：正向 / `--reverse` 均要求声明面在场（同属输入缺失档；逆视图输出结构不变）；`--diff-ids` 不读声明面、行为不变。⑤ 纪律侧——`uiux-author` Process 第 2 步成文「命名族 = 完整母版 + 升版与声明面同一次落盘」、第 3 步出图落点句成文「单一活图夹 + 升版冻结快照 + 引用一律走声明面 current」（OQ2 丙裁）；`write-story` 锚行规则第 4 条指针改指声明面；`UIUX-DESIGN-BRIEF.md` 头部的设计稿版本声明改指声明面（brief 自身版本号保留——它是「对齐哪版 requirements」的判据载体，两族错位如实记）；`requirement-change` 4b 出图版本指声明面。**无已污染制品需清理**（原条目补充说明仍成立）。**边界**：与 tech 域 `ARQ-260918-spec-face-3page-reconcile`（同工具不同根）不合并——该条治 spec 叙述与实现漂移、仅引用本脚本作验证手段，本条治脚本判据自身缺口；本批 `docs/requirements.md` / `docs/architecture.md` 零 diff。留根：① 截图图夹布局 OQ2 已裁丙（单一活目录 + 冻结快照）——本批只落规则句与声明面语义节，frozen 快照机制本体、活图夹目录整形与 `docs/ui/V5|V6|V7` 存量迁移另立需求面动作，本批不动图夹与 `designRefs` 形态；② `tools/asset-pipeline/verify_art_assets.mjs:14` 仍硬编码 `kidpal-ui-design-v3.html` 绝对路径、`docs/ui/PHASE1-DESIGN-INPUT.md:5` / `:128` 仍称 v4 为最新母版——两处待另行登记。
