# pod-retro · 260921-m3-k2-p2-week-grid-page（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：human-decision `5b54bb47` launch 授权、dispatch ×3、friction over-scan ×2）。

## 卡点与体感

1. **这一棒最顺的部分是「交接注记全部提前拆雷」**。K1 留根的坑我在 intake 逐条过了一遍：origin 枚举预检（f6a3df5 已修）、current/ 指针（launch-prep 已刷）、mutation-check 先读提取器（我真的去读了 `mutation-check.mjs` + `acceptance-id-shared.mjs`）——结果这次坑果然变形：260731 批把 `mentionsId` 收紧成全串子串匹配，perAcceptance 短 id 全部假阴，必须把验收全文整串写进记录才命中。**「读 checker 再回填」的纪律这次真救了一轮闸红**，这比 K1 retro 记「坑的形状」有用得多——形状会变，机制不会。
2. **我自己制造的唯一一轮红**：回填 taskpackage 后忘了刷新 `current/` 指针副本，round 1 taskpackage-required 红。这不是代码缺陷，是 planner 代落动作的半途而废——「改 canonical 必刷 current/」应该跟我自己的回填动作绑成一个原子步骤，而不是靠闸来兜。好在 scoped 轮便宜，round 2 即绿。
3. **preview.time 裁决点是我最担心的交接，结果平安落地**。启动卡说「如发现与契约门本意冲突，升级 human」——acceptance-author 读契约原文后论证了口径自洽（冻结 seam 的 `p1_valid_item` 物理容不下 CJK 字面，`"明早"` 是示意非 wire 值），并把「非 HH:MM 拒收」钉成了行为齿。**这是「升级条件 = 真冲突」而非「有疑点就升」的好例子**：疑点用原文消解、消解过程留痕在齿里，human 不用出场。
4. **brief 机制的「行号快照非权威」提醒这次被反向验证**：两个 brief 的符号锚 `not-found`（kidpal_schedule_parse 等在旧列表版代码里），两个子 agent 都没有照旧行号硬读，而是按 SSOT 重定位并如实申报超读。超读申报表两轮都非空且理由扎实——over-scan 记账落了两条，纯记账，无摩擦。
5. **enforce-scope 代落惯例第三次平滑运转**：acceptance-author 回填 taskpackage 被拦 → 我代落 + diff 验证，零返工。这条边界（验收作者不碰 taskpackage）每次都觉得「挡正确的活」，但代价已经稳定趋零，不值得再动。

## 别扭的 acceptance / 帮倒忙的钩子

- 无别扭验收。冻结齿的「BUILDER DUTY 6 条 + 离线契约文件头」形态让 builder 有实现合同可对照，整页重写这种大体量活一次交绿。
- 帮倒忙候选：`structuralWords` 正则不含中文「编译/设备构建」，E3 那类中文表述的准则永远不会触发 structural 豁免——本包 A1–A6 全有 mutation 证据所以无伤，但对纯 build 闸包这是个潜伏的假红源。不值当单独立项，记下备查。

## 人工纠偏体感

- 本棒 human 只出场一次（launch 授权），后半程零打断——K1 验证过的「前半程过目、后半程自主」分层继续成立。
- loop-retro 阈值 advisory 已到 16，发起责任在主线程/human——留给收尾报告提请。

## 下次改进

- **planner 代落 taskpackage 字段后，原子刷新 `delivery/taskpackages/current/taskpackage.json`**——写进自己的动作序列，不等闸红。（对偶 launch-prep 已办结的「起包刷指针」：那条管起包时点，这条管中途回填时点。）
- mutation-check 的坑记录应该指向「checker 源码行号」而非坑形态——本次有效正是因为真读了源码；下次还应读（机制不变、坑会变形）。
- `structuralWords` 的中文盲区若未来咬人，方向是补词表而非改流程。
