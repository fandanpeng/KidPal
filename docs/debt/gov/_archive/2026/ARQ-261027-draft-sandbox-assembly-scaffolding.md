---
id: ARQ-261027-draft-sandbox-assembly-scaffolding
title: architect 起草期无 canonical 写权但 PROPOSAL 须承诺「改哪几行、改成什么样」+ §5 验证实跑——每批现写一次性沙盘装配脚手架（行号基线写死 + 仓外副本），结构性别扭待专项讨论
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: human 立项专项讨论；或下一个触 `.claude/scripts/**` 落地面的治理批出现第三例脚手架时强制带出
opened: 2026-10-27
closed: 2026-10-27
closedBy: 261027-gov-review-draft-cost-relief
paths: [.claude/agents/architect.md, .claude/scope-map.json, .claude/scripts/, docs/templates/PROPOSAL.template.md]
source: [delivery/review/261027-gov-review-p6-landing-gate-cluster/_scratch/assemble-script.mjs（活例，起草期临时件）, delivery/review/261027-gov-review-p5-spike-anchor-cluster/PROPOSAL.md §5 断言纪律行（:531 附近）, delivery/review/261027-gov-review-p5-spike-anchor-cluster/retro.md（FROZEN+landing 行与 §3 主线程行）, delivery/review/261027-gov-review-p5-spike-anchor-cluster/architect-landing-summary.md（夹具留存注）]
---

**证据**（P5、P6 连续两批同型，human 2026-10-27 立项 P6 时明示「挺别扭的、值得仔细去讨论一下」）：治理分工规定 architect 起草 PROPOSAL 但不落 canonical（`.claude/scripts/**` 等正式改动 = 主线程 packet-bound 落地），而 PROPOSAL 硬条款同时要求 §2 写明「改哪几行、改成什么样」（逐字节级承诺）+ §5 验证集命令实跑回填实测数字——两者合力迫使 architect 每批现写一次性「沙盘装配脚手架」：P6 实例 = `delivery/review/261027-gov-review-p6-landing-gate-cluster/_scratch/assemble-script.mjs`（自述「起草期脚手架，P5 批先例；起草结束删除、不随批 commit」：读仓内 775 行原脚本 → 行号降序替换/插入清单 11 处 → 每处先核首末行锚文 fail-loud → 落仓外 `C:/tmp/p6-assembly/`）；P5 实例 = 同构先例——PROPOSAL §5 断言纪律行自述「脚本面预期值 = 『按本稿 §2 编辑逐字节装进 pristine 副本』的实跑输出（副本在仓外 `C:/tmp/p5-assembly`，装配脚本从 §2 文本装配、不手抄；装配脚本本体在 `_scratch/`，起草结束后删除）」，retro 记「落地走装配脚本（非逐处手工 Edit），比 P4 省手工」，landing-summary 记夹具本体落仓外、配方脚本随 `_scratch/` 删除。**摩擦实测**：P5 retro F1——装配锚 fail-loud 只兜机械锚，散文 file:line 引据不在任何机械兜底内，行号漂移三轮连发（R1-F1 / R2-F2 / R3-F4 同族）成该批最大纯摩擦。

**影响面**：凡触 `.claude/scripts/**`（或任何逐字节承诺落地面）的治理批每批一次：① 装配脚手架每批重写、不复用（行号基线写死，上游一动即漂移中止）；② 「仓外副本实跑 ≠ canonical 实跑」的残余差（副本装配正确性靠脚手架锚文人肉维护）；③ §5 实跑数字的成本每批重复支付；④ 不修则 P5 F1 型散文引据漂移摩擦持续，且脚手架质量完全取决于当批起草者自觉（无模板、无 checklist 句承载）。

**建议改动**（候选方向集，**交 review / Track A 段讨论，不预裁不自批**）：① **写权通道**——给 architect 授起草期临时写槽（如 scope-map worktree 内 allow、packet-bound 语义重裁为「commit 归主线程」）：治本但动写权矩阵与 packet-bound 防线语义，须重论边界；② **装配工具化**——把沙盘装配固化为共享脚本（通用按锚文装配器，锚 fail-loud 语义复用），architect 每批只写编辑清单：不动写权、治重复劳动，但「副本≠canonical」残余仍在；③ **承诺粒度降级**——PROPOSAL §2 降为编辑清单+意图、不逐字节：与 §5 验证实跑纪律冲突面须重论（可能削弱 landing-verifier 对账基准）；④ **维持现状、脚手架模板化**——template/checklist 句承载装配器写法与锚文纪律，最低成本、只治重复不治别扭。①③ 均触承重面（写权边界 / 验证纪律），②④ 不触。

**风险**：riskTier: medium——修法候选 ①③ 任一都动写权边界或验证纪律等承重面，②④ 虽轻但治标。review profile：**full-adversarial + human**（Track B 三要件 ② no——修法存在真设计分叉且方向未收敛，human 2026-10-27 明示「值得仔细讨论」；①③ 命中 G3/mixed 机检硬排除）。查重注记：与 ARQ-261014-closeout-obligation-write-authority-vacuum（收口期写权真空）、ARQ-260929-docs-sync-spec-handoff（integrator spec 面，已结）、CDQ-261022（docs-sync 写权事件）相邻但不同根——本条独占「起草期实跑落地面」根。owner=architect。

**立项材料追注（2026-10-27，P6 批起草期 human 两句观察 + token 净账缺口，append）**：human 看 P6 PROPOSAL（966 行）原话定性——「就感觉像是在看一个详细设计，连代码怎么实现都写好了」+「token 消耗应该巨大」。结构性成因：gov 批无 builder 角色、主线程落地纯编排不许创作，实现整体前移进 PROPOSAL（设计文档与代码载体合一），逐字节承诺是 `checkVerbatimQuotes` 字节对账 + landing-verifier 保真判定的消费需要。**token 两侧账（讨论第一张表）**：烧钱侧 = P6 起草单 architect ~343K / P5 批整环子 agent ~1.24M + 主线程估 350–500K / 沙盘脚手架纯增项；省钱侧 = ARQ-260914 追注估计自证到位省 R2/R3 ~455K + 核验 ~142K（byte 纪律的原始动机即评审轮次砍削）。**两侧从未对抵**——P5 retro 只记成本未记纪律省下的轮次净值，「逐字节承诺净赚还是净亏」 presently 无计量答案，应作专项讨论的起点问题。

**方向裁定追注（2026-10-27，human 与主线程专项讨论后收敛，append）**：P6 实测账 = 单文件 +387/−33（净 354 行，其中自检夹具约 1/4–1/3、纯逻辑 ~250 行）vs PROPOSAL 1035 行（描述 ≈ 实现 3 倍）vs 全批 ~1.69M tokens；讨论曾评估「脚本批路由 dev-loop 实现」（TaskPackage + 验收测试模型，估 3–5 倍价差）。**human 裁定：不走 dev-loop**——「它如果真的把哪个闸改坏了，确实蛮危险的」（执法层自改风险否决写权通道 ① 与 dev-loop 路由 ③）。**收敛后的修法集 = 两便宜项 + 一新规则**：② 装配工具化（共享锚文装配器入仓，architect 每批只写编辑清单）；⑤ 复跑分档（PROPOSAL §5 验证集区分全量 / 影响面窄化两档，治 retro F2 复跑税）；**新规则 = 脚本批拆分纪律**——`.claude/scripts/**` 净增 >~150 行（初值、随复发校准）或 feature 级改动（新增子命令 / 判定重写）单独成批，fix 级小改（<~50 行）可并批搭车（P6 复盘：feature 级 cite-check ~306 行与 fix 级 T2/T3 ~114 行同批，fix 陪 feature 走全套 apparatus 是主要浪费）。P6 批逐字节评审形态本身**维持**（判官审的就是将落地的代码，R1 抓 V6 断言字面证明其价值）。①写权通道与③承诺粒度降级**否决**。本条目待三修法落批后结项。

**2026-10-27 · 结项（`261027-gov-review-draft-cost-relief`，T1–T3 落地）**：三修法全部落批——① **装配工具化**：`.claude/scripts/assemble-edits.mjs` 入仓（旧串锚定装配器：逐条断言恰 1 次命中、全部通过才写盘、输出根强制仓外、sha256 回报；落地验证 V1 复跑 `--self-check` 16/16）；② **复跑分档**：`docs/templates/PROPOSAL.template.md` §5 增复跑分档句——改稿轮 / 落地确认轮按影响面窄化复跑（只复跑被改 T 的验证项），全量复跑义务保留在 FROZEN 标注前与落地后核验前两个时点；③ **脚本批拆分纪律**：`.claude/references/gov-change.md` 起草节 + 模板起草期自检句 ⑤ 成文——净增 >~150 行或 feature 级改动单独成批、fix 级 <~50 行可搭车（初值随复发校准）。**写权通道 ① 与承诺粒度降级 ③ 维持否决**（human 2026-10-27 裁，理由见上方方向裁定追注）。本条目由 `debt-render` 按 status 归位 `docs/debt/gov/_archive/2026/`。


