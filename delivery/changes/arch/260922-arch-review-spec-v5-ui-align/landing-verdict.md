# landing-verdict · 260922-arch-review-spec-v5-ui-align

> 判定者：landing-verifier（独立核验，只读 codebase）· 2026-09-22
> 核验对象：gov/260922-spec-v5-ui-align @ 9bd042a，diff 基线 = main
> **结论：RED**（1 项 blocking：结构门 B2——landing-summary 缺 manifest 围栏块；语义面 T1–T6 全部核对通过，修一处格式后可重判）

---

## 1. 结构门（机器）

```
node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260922-arch-review-spec-v5-ui-align
→ ok:false · gate=landing-fidelity
→ 缺 landing-manifest 围栏块（architect-landing-summary.md）：格式契约要求 frozen PROPOSAL §5
  与 architect-landing-summary 各含恰 canonical 目标路径集的 manifest 围栏块（B2 fail-loud）。
```

- PROPOSAL.md 侧围栏块在（landing-manifest = `docs/requirements.md` 单成员）；**architect-landing-summary.md 只有散文提及「landing-manifest（`docs/requirements.md`）」，无 ``` 围栏块**。31 份既有 change 的 landing-summary 全部带围栏块（含 260921-gov-review-canonical-text-align 等），非新规误伤。
- 脚本在 B2 处中止，后续 claimed-but-not-landed / planned-but-not-landed / scope-creep 机检**未跑到**——修块后须重跑结构门补齐。
- preflight 干跑（验证集第 6 条）独立实跑：ok，anchorCount=1，anchorUnion=[docs/requirements.md]，findings 空。

**blocking**：是。

## 2. 逐文件人读 diff 核对（docs/requirements.md，23 个 hunk 全读）

| T | PROPOSAL 改法 | diff 实际 | 判定 |
|---|---|---|---|
| T1① | WF-F1-01 重画 V5 形态 | 头注补 v1.57 说明；线框改 Tue 22 Sep / 「接下来」文本列表两行（14:00 钢琴课、16:30 英语培训，时间+课名同行）/「明天」标签区多项逐行（春游 ※临时、编程 19:00，注「仅课外班与临时事件」）/ 键帽 [说话][翻页] | ok |
| T1② | AC1 删倒计时贴纸 | 「页面带倾斜点缀等趣味细节」替代；「接下来」改文本列表表述 | ok |
| T1③ | AC2 逐项列出 + 明天区 OQ2 口径 | 「时间+课名同行、正常换行」+「明天」区多项逐行、**仅课外班与临时事件**（v1.57 OQ2 human 裁定） | ok |
| T1④ | 补充规则：布局优先级删倒计时 + 日期口径句 + 明天区口径句 | 布局优先级 =「接下来（文本列表）> 明天区 > 小伴演出 > 时间 > 温湿度」+「明天」区数据口径句 ✓；**日期英文简写口径句落在 AC1**（「Tue 22 Sep = 星期简写+日+月简写、无『今天』字样，v1.57」）而非补充规则——PROPOSAL 字面写补充规则新增，实质内容全落、summary 如实披露落点在 AC1 | ok（落点微偏，实质等价、已披露） |
| T1④附 | P1 实现侧对账登记 | 补充规则尾新增「⚠️ V5 版式对账（v1.57 登记）」：home_p1.c 按 V4 实现、四点差异、归 Phase 1 dev-loop | ok |
| T1⑥ | §4:550 可读性分级 | 「（提醒大字、星星计数等）用大字（v1.57：『接下来』改常规字号文本列表——退出大字例举）」——「与倒计时」删、「接下来」移出 | ok |
| T1⑦ | §6:614 路线图 | 「接下来」文本列表 +「明天」区 + 辅助条小图标（日期英文简写）+ 电量 ADC 接线——「+ 倒计时」删 | ok |
| T1⑧ | §8 已砍清单新增行 | 「P1 倒计时贴纸（『还有 36 分』）｜常显页两级信息足够；1bpp 小字可读性存疑（OQ1 裁定）｜2026-09-22 human 裁」 | ok |
| T2 | 「粗框/反色」双口径收窄单口径 | WF-F1-02 线框注释「今天所在列外框加粗高亮（反色弃——真机效果不好，v1.57）」+ AC2「外轮廓加粗高亮（列外框加粗、列内格线不加粗；反色弃用——真机效果不好，v1.57）」 | ok |
| T3①② | §2.2 键帽条行 + 提醒弹窗行 + 母版指针 | 「左动作提示 · 中 3 页点 · 右动作提示——键名（KEY/BOOT）不上屏，盒内直接写动作文字（长按前缀『长按·』，v1.57）」；提醒弹窗「KEY 知道了/BOOT 关闭」→「知道了/关闭」；「设计稿 v4…（v4 为现行设计母版）」→ v5/v5 | ok |
| T3③ | 全部线框键帽 [KEY X]/[BOOT X] → [X] | 11 条键帽行 × 2 = **22 处**实例改写（WF-B-01~04、WF-F1-01/02、WF-F2-01/02/04/05、WF-F3-01）；WF-F2-03 线框本无键帽画（全屏定格、注「键帽条照常常驻」），无漏。⚠️ landing-summary 披露「共 20 处」与实际 22 处不符——计数误差，改动本身完整（0 残留） | ok（披露计数偏差，见问题 2） |
| T3§2.3 | 手势表物理键位语义不改 | §2.3 无 diff hunk | ok |
| T4 | F2 组框族口径引注 | 线框清单行后新增引注「框族口径（v1.57）：正常圆角矩形（单值圆角、无倾斜）…星星计数（★×N）去框裸排。视觉细节归设计稿（现行母版 v5）」 | ok |
| T5 | S-F3.2 补充规则 + 交互包注记 + :485 对账块不改写 | 补充规则「右栏**纯文本逐行列出**（无卡片容器——v1.57 弃速查卡框族…）保持速查信息密度」；交互包尾「右栏形态 = 纯文本逐行（v1.57，弃速查卡框族）」；v1.38 对账块（:485）与 main 的 :480 行**逐字一致**，未改写 | ok |
| T6 | 头部版本行 + §9 | v1.55 → **v1.57**；§9 表首新增 v1.57 行（内容六点摘要 + 依据含 Track B 凭据），既有 v1.56 勘误行以下全部未动 | ok |
| 顺带归一 | （PROPOSAL 未列，summary 披露） | 页点 4→3 共 **4 处**：WF-F1-01、WF-F1-02、WF-F2-01、WF-F2-04——与 landing-summary 披露数一致，仅此范围，无额外；§9 v1.57 行注明「v1.36 既有口径」 | ok（授权外改动但如实披露、口径有据 v1.36、范围最小） |

**私带检查**：23 个 hunk 全部归属上表，无 PROPOSAL 未授权且未披露的改动。分支总触面 = docs/requirements.md + change 目录四制品（PROPOSAL/landing-summary/review-r1/review-r2）+ INDEX.md 登记行（changes-index 合法随批件），manifest 外无 canonical 文件被本批改动。

## 3. §5 验证集独立复跑（六条全绿）

1. `grep -n '粗框/反色'` → 0 命中 ✓
2. `grep -n '\[KEY \|\[BOOT '` → 0 命中（含 §9）✓
3. `grep -n '当前版本'` → 恰 1 行（:7）= v1.57 ✓
4. `grep -c 'v1\.57'` → 13（≥2）✓
5. `grep -n '倒计时'` → 5 命中全部落枚举闭集：:235（V5 版式对账句，对账类文字）、:361（WF-F2-05「不显示倒计时」，正交概念）、:674（§8 砍项行）、:686（§9 v1.57 行）、:696（§9 v1.48 历史行）——正文 AC/线框/路线图零残留 ✓
6. preflight 干跑 → ok，anchorUnion = `docs/requirements.md` 单成员，findings 空 ✓

## 4. 破坏性自检

- §9 既有行：仅表首插入 v1.57 行，v1.56 勘误行及以下逐字未动 ✓
- :485 对账块（v1.38 登记 + v1.56 更新）：与 main 逐字一致 ✓
- §8 已砍清单既有行：append 新行，未改写 ✓
- `docs/contracts/**`、`docs/architecture.md`、`firmware/**`、`docs/ui/**`：分支 diff 零触碰 ✓
- packet-binding：landing-summary 引 packet = 本 change FROZEN PROPOSAL v1.2 + manifest + 本件；R2 评审记录含 GREEN 结论与 FROZEN 凭据行；landing 实际范围（requirements.md 单文件）⊆ manifest ✓

## 5. 问题清单

1. **[blocking] architect-landing-summary.md 缺 ```landing-manifest 围栏块**——结构门 B2 fail-loud 红，机检后续项未跑到。修法 = summary 补围栏块（内容 = `docs/requirements.md` 单行），重跑 `landing-fidelity-check.mjs` 至 ok。
2. [minor] landing-summary 披露键帽改写「共 20 处」，diff 实际移除 22 处 `[KEY/BOOT X]` 实例——计数误差，改动本身完整（残留 0），修 summary 时顺带更正。
3. [minor·不阻塞] T1④ 日期格式口径句落在 AC1 而非补充规则——实质等价、summary 如实披露，不构成漏改。

## 6. 结论

**RED（仅问题 1）**。语义保真全绿：T1–T6 改法逐 hunk 落到位、验证集 6/6 复跑绿、历史块 append 纪律维持、零契约/架构/实现触面、无未披露私货。修 landing-summary 围栏块（+顺带更正计数）→ 重跑结构门 ok 后，本 verdict 语义核对部分可直接沿用重判 GREEN；merge 决定仍归 human manual-merge 门。

---

## 7. 复判（2026-09-22，blocking 修复后 · append 不覆盖原判）

**复判结论：GREEN**（取代 §6 的 RED）。

**修复核验**（commit 60bbfd6）：
- `architect-landing-summary.md` diff 仅三处：① 头部新增 ```landing-manifest 围栏块（`docs/requirements.md` 单行，与 PROPOSAL 侧一致）；② 新增 packet-binding 行（packet-id = `260922-arch-review-spec-v5-ui-align` §2（T1–T6）+ §5 验证集；被批 review = `architect-review-r2.md` GREEN @ DRAFT v1.2）；③ T3 键帽计数 20→22 勘正——与本 verdict §5 问题 2 的实证数（11 键帽行 × 2 = 22）一致。
- `docs/requirements.md` 在 9bd042a → 60bbfd6 间零变化（`git diff 9bd042a..60bbfd6 -- docs/requirements.md` 为空）——**§2 逐文件语义核对、§3 验证集复跑、§4 破坏性自检的原判结论对 60bbfd6 全部沿用有效**。
- 修复 commit 其余触面 = 本 verdict 文件入库（verifier 制品随批），无 manifest 外 canonical 改动。

**结构门独立复跑**（本 verifier 实跑，非沿用主线程自述）：
- `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260922-arch-review-spec-v5-ui-align main` → **ok:true，exit 0**。frozen ✓、packetBinding ✓、proposalTargets == summaryTargets == [docs/requirements.md]、unauthorized/claimedNotLanded/plannedNotLanded 全空、citation/lint/verbatim advisory 无问题。
- 注：不带 baseRef 跑会因「landing 已提交、HEAD~1..HEAD 只含修复 commit」而误报 claimed-not-landed——按脚本用法传 baseRef=<landing 前 ref>（= main）后判定正确（脚本 hint 明示此用法）。非门缺陷，记录备查。

**复判依据汇总**：blocking（问题 1）已修 + 结构门独立复跑 ok + minor 项 2 已勘正 + 原判语义面 T1–T6 / 验证集六条 / 破坏性自检 / packet-binding 部分沿用。放行建议：可进入升级闸与合并流程；merge 决定归 human manual-merge 门。

**遗留 minor（不阻塞）**：问题 3（T1④ 日期格式口径句落 AC1 而非补充规则）维持原判——实质等价、已披露，不构成漏改。
