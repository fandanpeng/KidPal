# pod-retro · 260923-m3-k7-schedule-grid-restore（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包 human-decision 事件 ×3（部署+开闸授权、升级合并批准、rebase 后 ff 方式选定）；零 agent-escalation；零 fixer 轮。子代理派发 ×6（architect 起草 ×2 轮、scenario-author、challenger、defender、adjudicator、landing-verifier、acceptance-author ×2、builder ×2、integrator）。

## 主轮体感（2026-09-23 · host 半闭环 + 真机部署，零 fixer 轮）

1. **上游链条长，但每一站的产物都直接成了下游的输入，没有一次返工**：triage 门把「P1 已修 / P2 是设计缺口」拆干净 → arch-change 一轮对抗 review 自收敛（challenger 的 v1/v2 错位还被我 grep 当场复核掉）→ v3 定稿 → 契约门 → 包 → dev loop。回看，真正省时间的不是某个 agent 快，而是**每站都有 frozen 制品**：challenger 的 findings、编者的 v3、landing-verdict，后面每一棒都在读文件而不是读我的转述。转述是损耗之源——这次链条长却没损耗，就是因为我把「读原文」当成了纪律。
2. **我自己在包 schema 上连撞两个机械形状坑**（verify r1/r2 两红）：`sourceType` 我凭语义写了 `arch-change-handoff`（闸只认枚举集 `arch-review`）、`crossEnd.counterparts` 我写成散文串（闸要 `{path,symbol}` 结构体）、`mutationEvidence` 我写成嵌套对象（闸要扁平 perAcceptance 记录）。三次都是「语义直觉造形状」，三次都是闸用报错教我。**教训：起包形状 = 复制既有包 + 对闸源码点检，绝不能凭字段名猜。** 这条我写进下次起包习惯。
3. **子代理 Write 被环境拦是本会话的系统形状**：challenger / defender / adjudicator / acceptance-author（后者是 taskpackage 半）全撞「Subagents should return findings as text, not write report files」。前两次我事后补救（代存），第三次起我改成派单时就写明「Write 被拦 → 全文回执，我代存」——**这个约定应该在派单模板里，而不是让每个 agent 撞一次再学**。
4. **builder 无 `delivery/taskpackages/**` 写权，第二次复现**（K6 retro 已记过）：K7 的 mutation evidence 与部分制品又得由我/integrator 代落。同一坑跨棒复现 = 记账没升级成机制。我这次把代落做成了「原始输出入文件 + 注明代落原因」，但**建议升级为派单前置 checklist 硬条目**（谁落盘、落哪、有没有写权）。
5. **TEST4 memcmp 那一下，builder 的处置比修法更值得记**：它发现红是验收方法学缺陷（结构体未初始化填充字节）后**没有自认合规、没有偷偷改冻结件、也没有绕**，而是升级并给出两个选项 + 建议。我的裁决（授权 `ksp_copy_str` 零填充）之所以敢下，正是因为它的证据链是自研复现的逐字段比对。**「本地红来自流程缺口而非实现缺陷」这条 red-flag 用对了一次，就省掉了一轮假 fixer 循环。** 连带经验：验收齿对含 padding 的结构体做整包 `memcmp` 是脆弱形态，值得写进 acceptance-author 撰写约定（跨棒建议）。
6. **契约面把机械合并路径整条封死**：`merge-driver` 内部 invoke 的升级闸里，「契约门证据」是硬编码 `false`（自注「后续接入；现 false → 契约面 diff 恒 BLOCK（向严）」）。结果是：**凡碰契约的包，merge-driver 形同虚设，只能人合**。这次靠 pre-merge 预检（零重叠）+ rebase 后 ff 人工走通，但这是每个契约包都要重演一遍的摩擦。建议立 ARQ：要么接入证据、要么在 merge-driver 里对契约包显式改道「人合 runbook」，别让每个 planner 现场发现。
7. **C1 的条件写法有内在张力**：它同时要求「固件先行」与「旧固件行为实测」，而固件一先行，旧固件那个窗口就不存在了——两个要件互相覆盖。我按「时序满足保护意图 + 降级如实记」处理，但这说明**closeout condition 的写法要区分「保护意图」与「证据清单」**，否则执行者要么造假证据、要么违条件、要么像我这样写一段解释。建议升级为 condition 撰写约定。
8. **真机这一半的价值被低估过**：host 齿把 restore 消费钉得很死（层栈/哨兵/幂等），但「屏幕到底有没有内容」只有真机能回答。部署后那段冷启动日志（`schedule_grid_restore courses=37 cached=37` + 零接管）第一次让我觉得 ADR-005 的 restore 语义是**看得见的**；human 目视 P2 正常 = K7-A8② 达成，比任何齿都直接。

## 别扭的 acceptance / 帮倒忙的钩子

- 子代理报告 Write 拦截（见主轮 3）：三次事后补救 = 三次白跑的转录；约定要前置。
- `verify.mjs` 的相位参数是 `--phase=final`（不是 `full`），我第一次跑 `--phase=full` 静默退回 scoped——参数不认时**静默降级为默认相位**是危险形态（我差点以为 final 跑过了）。建议：未知 phase 值直接报错而非回落。
- `mutation-check` 的 `mutationEvidence` 形状（扁平 perAcceptance + 整段 acceptanceId 文本命中）仍只活在源码里（K5/K6 两次入册，本棒第三次撞）——**非阻塞建议第三次入册 = 该升硬条目了**。
- `serial_catch_tight.py` 的内层 `while True` 不回收截止时间（skill 已警告），我按下 kill 才释放 COM5；这类「用完必须手动收尾」的脚本值得加自退。

## 人工纠偏体感

- 三次 human 触点都在**真判断点**上：部署授权（生产写入）、升级合并（契约面）、合并方式（历史形态）。没有一次是「机械形状」类问题上去打扰——三次都拿到了明确答复，节奏是我经手几棒里最舒服的一次。
- 唯一一次「human 主动纠偏」是早期：我要建 task package 时，human 追问「Spec 差需求还是差架构设计」——这一问直接把定性从「可能是实现 bug」翻成「requirements↔architecture 漂移」，救掉了整条错路。**这个教训值得单独记：症状报告进来时，先问「哪一面在承诺、哪一面没兑现」，别急着进实现面。**

## 下次改进

- 起包形状：照抄既有包 + 对闸源码点检（sourceType 枚举 / crossEnd 结构 / mutationEvidence 扁平记录三处已栽过）。
- 派单模板加两条硬约定：① Write 被拦 → 全文回执；② scope 写权表明示（谁落哪个目录）。
- 建议升 DEV-GOVERNANCE 硬条目：mutationEvidence 键从 acceptance[] 复制原文（第三次入册）；structural memcmp 齿撰写约定；closeout condition 的「保护意图 vs 证据清单」分离写法。
- ARQ 候选（governance）：① 契约门证据接入 pre-merge 闸（或显式改道人合 runbook）；② `verify.mjs` 未知 phase 报错而非静默回落。

## 真机留账（设备轮已部分兑现）

- K7-A8：① 冷启动 P1 恢复 + restore 接收日志 ✅（13:57 实证）；② 冷启动直翻 P2 网格有内容 ✅（human 目视确认「页面显示正常」）；③ 语音「我的课表」接管照旧 ⏳（human 可随时试）；④ 旧固件行为复核 ⚠️ 被固件先行时序取代，降级如实记（deploy-evidence/DEPLOY-RECORD.md §3）。
- **线上状态备忘（下个会话必读）**：ECS `kidpal-front` 现挂 `KIDPAL_GRID_RESTORE_PUSH=1`（drop-in `/etc/systemd/system/kidpal-front.service.d/kidpal-grid-restore.conf`），**开关处于开启态**；灭活 = 删该 drop-in + `daemon-reload` + `restart kidpal-front`。设备侧固件 = K7 版（含 restore 消费分支）。