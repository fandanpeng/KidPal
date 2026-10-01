# pod-retro · 261001-m4-k3-record-session-live-repair（planner 第一人称热记忆）

> 写于 integrator closeout 完成、final r2 十闸全绿之后、supervisor 机械闸出 ESCALATE（high riskTier）待人类批之前。机器事实（轮次/闸/verdict/测试计数）以 verifier-evidence/rounds、supervisor-verdict.json、closeout 制品为权威，本件不复述数字。

## 一、本棒形态：回归修复包的「launch 面从零补齐」

本包是 `/build-feature 261001-...` 直启，但 launch 面五件里 ①board 条目 ②启动卡 ③worktree 记录三件缺——于是先跑 `kidpal-pod-launch-prep` 补齐再进 intake。关键判断：**这不是 portfolio-planner 的拆包产物**（spine 明写「taskpackage.json/origin 归 planner intake 建」），却要过 commit-before-launch 的 base ref 门（`check-launch` 读 `base/m4-star-notebook:<id>/taskpackage.json`）。牙关在：**自建包要先把制品落到专用 base ref，check-launch 才绿**。我按 K2/K3 先例做：常驻分支 rebase main → 在 base ref 上 cherry-pick 一个「main + 制品」commit（`8ea3bce`）。human 授权「本会话全套补齐」，未走 portfolio-planner 回补。

## 二、元数据形态红是本棒的主要摩擦（连红 4 轮，全是形态非实现）

内循环 4 轮红，**没有一轮是实现/齿面红**，全是 TaskPackage 字段口径：

1. `taskpackage-required`：`contractRefs` 空数组不算在场（要非空）；`acceptance[].kind` 只认 `structural`（我写了 `regression`）；缺 `crossEnd` 骨架（finalize 须 counterparts 非空 ∨ exempt ≥12 字）。
2. `acceptance-coverage`：红跑 wrapper `.sh` 误入 `acceptanceTestsFile[]`（该字段只收「有断言的文件」）。
3. `acceptance-coverage` 二红：acceptance 条目 id 因**全角括号**未被切出（提取正则认 `ID: ` 半角冒号+空格），我写的是 `K3R-A1（D4a…）：…` → 提取成 `K3R-A1（D4a`。改 `K3R-A1: D4a…` 即愈。
4. `mutation-check`：缺 `mutationEvidence`。

**教训（可机械化的）**：`planner.md`/intake 的 TaskPackage 草案模板若内置「contractRefs∋非空 / acceptance 字符串 `ID: ` 形态 / crossEnd 骨架 / kind 白名单」四条预检，本棒能省 3 轮。建议进 loop-retro 素材池。

## 三、mutationEvidence 的一处真教训：设计态红 ≠ 变异证据

K3 先例把「设计态红」当 counterEvidence（并注「GREEN 后 builder 实做重放」）。本棒我一度想照抄，但止住了：本包的 RSR 齿是**行为齿调 seam**，seam 被 builder 实现后，「seam 缺失的设计态红」**不再证明当前实现对变异敏感**——若 builder 把 `[R1]` 实现错，齿可能照样绿。故我回退 builder 跑**5 次真实变异重放**（破坏实现点 → 确认对应齿转红 → 还原），取真 RED verbatim。代价 = 一轮 builder；收益 = evidenceComplete 站得住。**建议成文**：`mutationEvidence.counterEvidence` 必须是「实现后重放测得的红」，设计态红只能作为「齿敏感性的先验假设」，不能顶替。

## 四、两处 builder 侧诚实申报（都成立）

- **`story_page.h` 清单外**：builder 未自行扩清单而**举手**——正解。裁决依据来自**冻结验收件文件头**（seam 契约明写 home = `story_page.{c,h}`）+ 姊妹包 K3 manifest 含 `.h`；planner 裁定 append，非越权。
- **RSL-A4 tooth 3 张力**：acceptance-author 事先定位（D2 让位须落录音进入桥、保留 REC_SESSION case 的 `show_chat`），builder 选甲形态时遵此、零触碰 K3 冻结齿。**前置把「形态选择可能与既有冻结齿冲突」说清 = 省了一整轮 revision**。

## 五、escalation themes（cross-ref 交接回执）

- **agent-escalation / 内循环 fixer 轮：零**。verify 4 轮全是元数据形态，无需 fixer 打补丁。
- **supervisor：ESCALATE**（high riskTier，机械闸 priority-before-evidence）→ 停在人类批。
- **spec 面零写权移交**：integrator 检出两处 spec 事项（§5.2④ 引擎 manual 聆听机制对账归 Package B；§5.2④① 恢复时机文本收口归 K3 pending delta），**均未自改**，以 advisory 补正文本交 human spec 批——遵 `ARQ-260929-docs-sync-spec-handoff`。

## 六、本棒未覆盖（如实挂账，不伪装）

1. **真机复验四窗**（K3R-E3）：无真机证据，deferred 归 live 确认轮（human 持板）。
2. **AFE VAD 真沿** / **120s 静默自动归档**：设备轮盲区 / 时长窗未覆盖。
3. **D3「说完了」+ 逐句回显**：属 Package B（触需求/架构/契约三面），受 spec/contract anchors 门控。

## 七、下次改进（具体到动作）

1. TaskPackage 草案落盘前，自查四条形态（见 §二）——或推动把四条并进 `taskpackage-required` 的前置草图模板。
2. `mutationEvidence` 生产纪律成文：实现绿后重放，不抄设计态红（见 §三）。
3. 回归修复包 launch 面：`check-launch` 对「spine 自建包」应有一条明路（把「base ref 制品 cherry-pick」写进 launch-prep skill 的步骤），避免每棒临场想。
