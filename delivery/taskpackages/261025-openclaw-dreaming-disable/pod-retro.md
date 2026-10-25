# pod-retro · 261025-openclaw-dreaming-disable

> 标签：**第一人称热记忆**（planner 全程在场的体感）。本件不复述 token / gate / round / verdict / test 结果——那些以 machine artifacts / pod-events / verifier-evidence 为权威。收尾相位写于 integrator closeout 之后、supervisor 机械闸之前。

## 这次顺在哪

这个包是我近期少见的一路直行。输入给得极硬：诊断件把机制、量化、architect 裁决、human 四裁决全钉死了，debt 条目又把落地形态与清理细节写透，我几乎不需要自己发明方向。同族先例（honcho / kidpal-tools）把「声明 + apply 编排 + rollback + 冻结验收 + manifest 登记」的骨架摆得整整齐齐，builder 基本是照骨架换内容。acceptance-author 也很稳：它冻结完自己在仓外临时树搭了一份参考实现验证断言双向可满足（能做绿、也能被反向变异打红），这一步让我对「这 17 条不是空过也不是不可满足」心里有底。

## 卡点与我的主观判断

**1）drift_guard 到底几条叶。** 派单写「按同族三叶 AND 写法锚 tools.deny / tools.profile / tools.allow」，但验收要求里又有「dreaming 显式 true → drift」和「fresh 机 unset 接受」。这两句凑在一起，逼出一个我一开始没想清的取舍：dreaming 叶要不要也进 drift_guard？如果不进，true 就永远走不到 exit 3；如果进，就得接受「显式 true 会 fail-closed 挡住部署」。我把这个歧义直接丢给 architect 1g 预评，它给了明确裁决（dreaming 叶进判定面、profile 必须精确 full 不接受 unset）。事后看，**把有歧义的点前置给 architect，比我自己硬猜省了一整个 fixer 轮**。

**2）SKIP 先于 drift 还是联合幂等。** 姊妹脚本一个「SKIP 先于 drift」（honcho）、一个「联合幂等」（kidpal-tools），派单要求「照被复制的那一份如实保持」。我选了 honcho 序，理由是这包的写目标只有 dreaming 一叶、工具面锚是前置基线不是写目标。但心里一直有个小别扭：dreaming 已经关掉、工具面却回退时，SKIP 会静默放过。architect 给的补丁是「SKIP 分支打 drift WARN，exit 0 不变」，我把它钉进了冻结验收的一个子 case。这是这次最舒服的一次「不静默、但不改门」的折中。

**3）rollback 直写与「禁脚本直写 openclaw.json」的表面矛盾。** 派单一边要「禁脚本直写」，一边要 rollback「形态照 rollback_honcho_plugin.py」——而那个脚本就是直接 write_text 还原备份的。我起初觉得这是派单自相矛盾，后来想明白：禁令针对的是「脚本拼装新目标态绕过 zod」（K3 崩因），而还原 apply 前那份被网关正常加载过的快照是另一回事。architect 也确认了这个区分，验收里还专门钉了头注语义锚。**教训：别把一句铁律当成对所有写入动作的一刀切，要看它防的到底是什么坏结果。**

**4）final 相位缺 scope-file 把我拦了一下。** 我按常规在 verifier 阶段直接跑 final，结果被 fail-closed 顶回来——scope 圈定是 integrator Step 4 的产物，得先有清单。这不是 bug，是我对「final 归谁跑」的记忆有点糊。把 integrator 拉进来之后一次就过了。**下次起包时我会在 plan 里就写清 final 轮由 integrator 圈定后跑，不再自己抢跑。**

## 帮倒忙 / 帮上忙的钩子与规则

- **帮倒忙**：我按角色文档让 integrator 跑 `changes-index.mjs --upsert`，结果那个旗标是 review lane 专用，dev 包必失败。integrator 没硬顶，改用裸跑全量重建再回退别的 lane，只留 dev 那一行——处理得比规则文档更清醒。这个坑值得写回记忆：**dev 包的 INDEX 不走 `--upsert`**。
- **帮上忙**：brief-inputs 的「清单退化整文件引用」advisory 提醒我 refs 该带锚点；acceptance-coverage 的短 id 机制让我们用 DD-A1..A5 这种干净编号就够；closeout-check 只把 pod-retro 缺失当 warn、不挡门，让我能按正确顺序（integrator 之后、supervisor 之前）补写这份热记忆，而不是被顺序绑死。

## 人工纠偏体感

本 loop 内没有一次人工打断——human 的四个裁决在起包前就全文给了我，我全程按它们走。唯一一次「人的声音」出现在合并门的定位上：派单明确说无论 APPROVE 还是 ESCALATE 都停在门前，不得 push/合并。这让收尾的心理负担很轻：我把证据链备齐，剩下的交给主线程呈报。这种「把 human 的判断点前置、把 agent 的执行面拉长」的分工，体感是顺的。

## 下次改进

1. 起包时就把「final 轮 = integrator 圈定后跑」写进 plan 的执行次序，别等 fail-closed 提醒。
2. 凡是角色文档里的 CLI 旗标（如 changes-index `--upsert`），先核它对哪条 lane 有效再派单，别把常识当事实传下去。
3. drift_guard 这类「判定面该几条」的设计题，继续前置给 architect；这次证明它比我自己在 acceptance 与实现之间来回猜要省。

## escalation themes

本 Pod 全程无 agent-escalation 事件。唯一的 friction 记账是 acceptance-author 的超读申报（over-scan），已在 pod-events 留痕，属正常取证阅读、非越界扫描。
