# pod-retro · 260911-key-route-a1（PKG-1 键位路线落地）

> 第一人称热记忆（planner 主线程，integrator closeout 之后、supervisor gate 之前写）。
> 负向边界：本件不复述 tokens / gates / rounds / verdict / test results——权威在
> machine artifacts（verifier-evidence r1~r3、pod-events.jsonl、review-packet.md）。
> escalation themes 交叉引用 pod-events eventId 见文末。

## 过程体感

整体顺：launch 三件套（taskpackage / PROPOSAL / board）齐整，portfolio handoff 探测
一次通过，fail-closed 没触发。装配型包豁免 plan.md 的分级判据（ARQ-260911-split-
package-implementation-prose-gap）体感成立——改法确实全在 PROPOSAL §2.4 + 冻结齿
接缝契约里承载，planner 会话没有「隐性消化掉、无处可查」的实施方案 prose。

## 卡点与绕轮原因（主观判断）

1. **基线口径 104/3 vs 实测 104/4**：intake 给的红基线引自首跑注记，但该注记活在
   主 worktree 未提交的 progress.md 里，Pod worktree checkout 不可见；acceptance-
   author 实测多出一条 VRT-A2/E2。裁决靠的是 existing-red-inventory R4 在册身份
   （owner=260829-voice-roundtrip），不是靠注记本身。体感教训：**launch 侧红基线
   口径应以已提交制品（inventory）为引，未提交注记是易碎载体**——这次没造成返工
   （inventory 兜住了），但判读多花了一轮探查。
2. **frozen 齿 coverage marker 形态返工一轮（r1 红）**：acceptance-author 按
   HOST_TEST_CONVENTIONS 的「coverage 标记随测试名」写了 `(KR-Ax)` 后缀 + tag，
   但机械闸 hasCoverageMarker 只认 `coverage: <id>` 前缀形态（session_logic 先例
   恰好是对的形状，conventions 文档没钉机器可识别形态）。r1 红→定位→acceptance-
   author 重派修复（断言零变更）→r2 绿，整轮开销不大但完全可以避免。**治理改进
   候选（交 /loop-retro，不自批）：HOST_TEST_CONVENTIONS 或 acceptance-evidence
   撰写约定把「机器可识别 coverage 行形态」钉成文，或让闸兼容 C 齿测试名形态。**
3. **真机轮捕获窗口与 human 操作不同步**：第一次 300s 窗口开早了（checklist 还在
   读），human 操作落在窗口外，口述「符合预期」与日志互证不上——按「completion 不
   凭解释宣布」纪律重捕。第二次窗口 human 主动喊开始，一次过。**体感教训：human
   实时持板轮的捕获窗口应由 human 就绪信号开启，不是 agent 单方面开播**；这次靠
   重捕兜住，没有造成假证据入档。
4. **docsSyncReason 缺字段**：portfolio 拆包产物 docsSyncRequired=false 但没带
   reason，docs-sync 闸 dry-run 预跑抓到（round 2 前修掉）。**预跑 scoped 闸这个
   习惯省钱**——mutation-check/docs-sync 两个纯静态闸都可以在派 builder 前干跑。

## 别扭的点 / 帮倒忙的钩子

- INDEX.md 被 closeout-check --update-index 写成混合换行（Windows 常态噪声），
  integrator 手工归一 LF 重 stage——老问题，ARQ-260906-shell-scripts-lf-
  gitattributes 同族，line-ending 治理债再次现形。
- closeout advisory 报「origin PROPOSAL not FROZEN (body-text check)」——实际
  frontmatter freezeState: frozen + 触点②记录在册，body-text 判据没认引用行
  「状态：FROZEN v1.0」。warn-only 没拦路，但判据与实况有漂移嫌疑，记录备查。
- loop-retro 阈值 advisory（12 ≥ 3）已挂起待主线程触发——本 Pod 不越权，移交
  备注留在本件。

## 人工纠偏体感

human 两次出场都干脆：真机轮执行方式选了「agent 驱动捕获」（推荐位），中断一次
 AskUserQuestion 后改用「human 喊开始」的自由文本协议反而更顺（见卡点 3）；手势
轮口述与日志全量互证，零分歧。AskUserQuestion 在「human 正在物理操作设备」的场
景下不如自由文本回合制——问题面板会把 human 锁在选项里，而持板轮需要 human 主导
节奏。

## mutation 纪律体感

builder 五条变异实测顺利，其中 KR-A3 首次变异名 `_TOGGLE_X` 因 strstr 子串仍命中
而不红、改 `_STUB` 才红——「变异命名不得保留原字面为子串」已落进 mutationEvidence
note，值得在 acceptance-evidence 约定里推广成通则（交 /loop-retro 汇集）。

## escalation themes（交叉引用 pod-events）

- `human-decision 8561409b`：真机轮执行方式裁定（agent 驱动捕获）。
- `human-decision c0ffe62a`：KR-A5 device-round PASS 确认（互证表在该 payload）。
- `friction 0d2b7577 / dc6c918c`：acceptance-author 与 builder 的 over-scan 申报
  记账（各 8 行，全部合理：母版先例/对端符号/回归面排查）——封闭清单+申报机制
  工作正常，没有拦路也没有漏记。
- 无 agent-escalation 事件（全程无 Red Flag 升级）；r1 红走的是缺陷修复重派
  （acceptance-author），不是 fixer 通道——齿缺陷与实现缺陷的分流体感清晰。

## 下次改进（本 Pod 视角）

1. launch 红基线口径引用已提交 inventory，不引未提交注记。
2. 冻结齿落盘前 acceptance-author 自验 acceptance-coverage-check（本次修复轮已让
   其养成了这个习惯，REVISION LOG 在册）。
3. human 持板轮：捕获窗口由 human 就绪信号开启；证据互证表当场产出（这次的两段
   捕获—分析—互证节奏是对的，保留）。
4. 预跑静态 scoped 闸（mutation-check / docs-sync / taskpackage-required）在派
   builder 前完成。
