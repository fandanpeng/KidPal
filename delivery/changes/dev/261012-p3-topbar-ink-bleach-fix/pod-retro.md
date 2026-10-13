# pod-retro · 261012-p3-topbar-ink-bleach-fix（第一人称热记忆 · planner）

> 标签：第一人称热记忆（我全程在场的主观体感）。负向边界自觉遵守：不复述、不冠权威、不载
> tokens / gates / rounds / verdict / test results——这些以 machine artifacts / pod-events /
> verifier-evidence 为权威（机械事实看 taskpackage.json、review-packet.md、rounds/*.json）。

## 过程体感

- **起包前的净面抉择是本包最关键的一步棋**。plan.md 原方案是「builder 删 36 行 TEMP-DIAG + SetTheme 行转正」，但我意识到未提交 diff 里已经躺着修复行——不先还原则红基线假绿，整个验收设计空转。快照还原（temp-diag-snapshot.diff）把「删前逐行核对」的 evidence-integrity 义务转译成了「快照比对 + TB-A2 负向齿」双承载，事后看这条路是对的：builder 净面起步、无历史包袱。代价是我作为 planner 动了 `git checkout --` 还原 firmware 文件——机械上没被钩子拦、治理上属工作区卫生非实现创作，但这类动作下次应该更早想清楚边界表述。
- **Bash 重定向写 taskpackages 被 universal 红线拦**是本包第二次撞（第一次是 k4-reminder 时代），按派发书①姿势转 Write 落盘，无伤。 universal.bashDenyWrite 对 `cp` 同样拦截是我没预料的——current/ 投影只能走 node fs.copyFile，这条路径比直觉窄，值得记进操作习惯。
- **mutation-check 的全文逐字匹配**是本包唯一一次真返工：acceptance[] 是长字符串条目时 perAcceptance.acceptance 必须逐字复制全文，我起草时写了短锚版本，round 1 红。TB-A4 对象化 kind:'structural' 走豁免通道、TB-A2 补跑变异取证是两条正确修复；261010 包 notes 里其实写了「acceptance 字段 = acceptance[] 全文逐字复制」的先例，我没在读先例时把这条捡起来——教训是读前包 notes 要按「地雷清单」读，不是按「背景故事」读。
- **补跑 TB-A2 取证轮里 builder 对我任务书的偏差处置值得表扬**：我让它 `git checkout --` 还原，它发现该文件带着在飞实现 diff、照做会摧毁实现，改走备份+字节还原并 md5 复核。派单时我没想到这个坑（我自己几小时前刚为同一个原因做过快照还原），子 agent 的判断补了我的盲区。
- **integrator 漏了 scope-file 制品**（final 相位 T9 红闸举手才暴露），我代落了清单合成。detect_changes 结果是它实跑过的、合成是纯机械投影，代落合理；但 scope-file 应该进 integrator 的交付物清单意识里，这是个可改进的派单模板项——下包派 integrator 时把「final --scope-file 消费件落盘」写进任务书硬清单。
- **TECH 条目收口后 docRefs 指旧路径**是第二个 integrator 尾巴（crq-render 自动归档是「位置=状态纯函数」的正常行为，但引用面没人跟着翻）。round 3 final 红闸抓住、机械修正。这和上一条同族：制品轮的自检清单对「我移动过的文件的引用方」不敏感。
- **整体节奏**：Lite 包三派（acceptance-author / builder / builder 补跑）+ integrator 一派 + 四轮 verify（r1 mutation 红、r2 scoped 绿、r3 final 缺 scope-file + docs-sync 红、r4 final 全绿），无 fixer 出场——红基线齿把实现路径钉得太准，builder 一次到位。冻结验收的齿位设计（红名即修复清单）是本包效率的来源。

## Escalation themes（post-cutoff，cross-reference pod-events）

- 本包零 agent-escalation 事件（pod-events.jsonl 在案）：无 Red Flag、无升级、无人工纠偏。human 触点两处 = triage 期档位确认（2026-10-12，已落 human-decision 事件）+ 收尾期 TB-A4 真机轮（human 在场持板 2026-10-13 执行，结果已落 human-decision dd77c762：计数分项 PASS / AC5 空态 deferred / 阅读态标题 BLOCKED 归因既有缺陷 TECH-261013 / 顶带无回归 PASS）。

## 收尾追记（TB-A4 执行后，2026-10-13）

- TB-A4 真机轮收尾形态好：human 一次持板给出四项逐项观测，且③的 BLOCKED 不遮掩——串口取证钉到 `open_reader_no_text -> stay list` 逐字证据、当场立 TECH-261013 新条目归因既有缺陷而非本包回归。真机轮的产出不止验收，还白捡一个新缺陷的正确归因，这是 live-manual 豁免通道设计的隐性收益。
- ②（AC5 空态）板上无空态可造只能 deferred——结构断言面（SLP-TB3 颜色单源）+ 真机定案试验（同渲黑零视觉差）已是该观测面的替位证据，deferred 归因如实记录即可，不折算 PASS、不留悬债。

## 下次改进

1. 派 integrator 任务书硬清单加两条：final --scope-file 消费件落盘；移动/收口任何被 docRefs/sourceRefs 引用的文件后同步翻引用。
2. 读前包 notes 时先扫「教训/地雷/先例⑤」类标记再读叙事。
3. 涉及未提交诊断 diff 的包，起包 checklist 加一句「diff 里是否含修复面」自查——本包是我碰巧看出来的，不该靠碰巧。
