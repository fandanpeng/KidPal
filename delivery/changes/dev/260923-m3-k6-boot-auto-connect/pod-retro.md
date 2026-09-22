# pod-retro · 260923-m3-k6-boot-auto-connect（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：dispatch ×3——acceptance-author `a6452dc7137eb4a2a`、builder `a429325f1dc73b9a9`、integrator `aef08351af89a791a`；friction ×3——over-scan ×2 `22dd511c`/`122ebc88`、degraded ×1 `408604ef`；零 human-decision、零 human-correction——human 全程离席夜间自主跑）。

## 主轮体感（2026-09-23 · host 半收口，零 fixer 轮）

1. **plan.md 的接缝密度直接兑换了零返工**：architect Shape A 把行号级接缝（四 call site 行号、OnNetworkError lambda 区位、task 模板参数、边角四条）全部钉进 plan.md，acceptance-author 与 builder 都没有再来问「这里怎么算」—— Lite 包 + reviewed plan 的组合是我经手几棒里派发损耗最低的形态。plan 里连「builder 勿画蛇添足复位 error_occurred_」这种防过度工程的话都写了，builder 也真没画蛇。
2. **verifier 两轮空转都是机械形状坑，不是实现问题**：r1 栽在 brief 双份维护形态（闸按 current/taskpackage.json 所在目录找 brief，K5 retro 已记为「非阻塞建议」却跨棒复现）；r2 栽在 mutation-check 隐式匹配（perAcceptance 的 acceptance 键须与 acceptance[] 整段文本值精确命中，起包时用摘要改写静默失配，同样是 K5 已记未消化的坑）。两条都是「retro 记了、闸没改、下棒照栽」的形态——**retro 建议消化不掉就升级成闸报错提示或起包 checklist 硬条目**，否则它只会跨棒复发。我落 friction degraded 在案，但记账不等于修复。
3. **mutation 欠账销账的通道这次走通了**：K5 三棒都只做到「设计态红≙变异态在案」的转述，本棒把「六条变异第一方实跑 + 原始输出落 mutation-evidence/」写进 integrator 派单，integrator 用字节级备份回拷 + sha256 复核的还原手法把执行证明做实了。关键是我提前想到 builder 没有 delivery/taskpackages 写权、把落盘任务挪给了 integrator——**scope 边界信息要在派单时就进 prompt，不能等 agent 撞了钩子再补救**（builder 主轮确实撞了一次，白跑一轮转录落差）。
4. **K5 留根常识②（host 齿只钉形态不钉时序）当场兑现**：acceptance-author 把退避 5s/10s/20s 钉成决策表形态齿（位序 + 封顶），builder 实现为 `kAutoConnectBackoffS[3]` 常量表——齿与实现同构，零时序断言。这是 K4/K5 两棒「双稳态 motif」教训第一次在设计端前置生效而不是在真机端事后救火。
5. **夜间自主跑的分层纪律第七棒成立**：human 只在预授权里给了「技术问题 call architect」一句，全程零打断、零拍板请求；真机锚挂设备轮不阻塞 host 半收口的调度裁定让 Pod 能一次跑完合并门前全部机械链。升级闸/合并门的 human 拍板项照常 fail-closed 上报，没有借「预授权」扩权。

## 别扭的 acceptance / 帮倒忙的钩子

- brief 双份维护（taskpackage dir + current dir）第三棒被同一闸咬——建议闸报错后自动提示 cp 命令（r1 报错文本其实已带 brief-inputs 命令，但没说「现有文件复制即可」，我多花了一次生成而非复制，方向对、路径绕）。
- mutation-check 的匹配语义仍只活在脚本源码里——起包方看 expectedRed 写摘要天经地义，闸却要求整段命中；DEV-GOVERNANCE 起包前置清单值得加一行硬条目（非阻塞建议第三次入册）。

## 人工纠偏体感

- 无（human 全程离席，零纠偏事件）。

## 下次改进

- 起包时 perAcceptance 的 acceptance 键直接从 acceptance[] 数组复制原文，禁止摘要改写——写进我的起包习惯，并推动升 DEV-GOVERNANCE 硬条目。
- 派单前置 scope 写权核对：谁落盘、落哪个目录、有没有权，进派发 prompt 而不是等钩子教育。
- brief 双份维护：intake 相位直接 cp 两份，别只生成单份。

## 真机半留账（挂设备轮窗口，human 在场收口）

- K6-A7 四子锚（冷启动建连 / wifi 闪断恢复避 IsTimeout / 宕机静默零错误卡 / task 栈高水位）真机实证未跑——Pod 按调度裁定不等真机，host 半先走合并门；真机轮后 progress.md K6 段落账在本文件之外由追加轮承担。
- 真机轮采样注意：本包 task 内重试退避是 vTaskDelay 时序行为，host 齿测不出实时节奏——按 K5「跨多边界采样」常识，wifi 闪断子锚至少采两次断连相位。
