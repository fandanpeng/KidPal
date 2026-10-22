# Review Packet — 261022-p3-record-chrome-ghost-fix

> integrator 合并门前审查包（2026-10-22，dev/pod-m2 worktree，包基线 `e5361caf`）。verdict：**收口完成、停在真机门前**——r4 final bearing 十闸全绿（fact `ok:true`，verifier-evidence-check PASS），closeout 三件齐；CG-A5 live-manual 真机目检未执行（human 持板，status=awaiting-human:real-device），Pod 不自行烧录，之后交 planner 跑 supervisor 机械闸并停等 human。

## 包事实

- changeId `261022-p3-record-chrome-ghost-fix` · sourceType `regression`（debt 条目 TECH-261022 缺陷①②，KPLOG005 真机轮 human 观感原话「日志里边记录的状态乱跳，跟我在屏幕上看到的状态乱跳是一样的」）· rigor `lite` · riskTier `medium` · ②单包 Lite（triage architect 比例化 review 裁决，human 触点①确认）。
- 验收：CG-A1..A4 host 自动齿（`firmware/main/test/chrome_ghost.acceptance.c`）+ CG-A5 structural live-manual 真机目检（human 持板，不入 runner 通过面）。
- 零 wire 形状改动（帧类型负锚 + crossEnd 豁免声明），`server/**` 零 diff（detect_changes impacted 18 符号全落 firmware/main 实测一致）。

## Change Size

- Files changed：**8 实现面文件**（application.cc +19、keybar.c +81、keybar.h +45、lcd_display.cc +53/−14、mcp_server.cc +12、chrome_ghost.acceptance.c 新建 +1246、chrome_ghost.test.c 新建 +188、host_app/main/CMakeLists.txt +16）+ docs 面两件（plan.md 齿映射节 + debt consumed-by 追记）+ taskpackage 投影与 verify 证据；实现面净 diff 约 +574/−14（不含验收件）。
- Scope：firmware/main/display（keybar 纯逻辑单源 + lcd_display 接线）、application.cc（Idle 收尾沿）、mcp_server.cc（两闭包收口）、test 登记一件 CMakeLists。
- Split recommendation：单包不分拆（lite 档单主题——chrome 鬼影族两缺陷共享同一验收件与同一回归网）。

## Verification Evidence

- **final bearing verification（scope 圈定）**：r4，phase=final，2026-10-22T10:12:14Z→10:13:20Z，invocationId `b9404d48-a4ba-454d-a2ed-1fd5d40891bc`，observedGateExit **0**，fact `ok:true`，`verifier-evidence-check.mjs --phase final` PASS。十闸全绿：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ **firmware-build（真编译 64s，xiaozhi.elf 链接通过，keybar.c/lcd_display.cc/mcp_server.cc/application.cc 全重建）** / contract-lint（1 条先存断链 warning，260923 arch 批路径，非本包）/ acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance。**test-scope 清单落盘路径一行：`delivery/taskpackages/261022-p3-record-chrome-ghost-fix/scope-final.md`**（三分法①正常圈定：per-Pod 图索引可用，detect_changes seed 58 / impacted 18 精确无截断；scope floor 含 CG-A4 点名三族冻结件）。
- **host 验收红→绿轨迹**：acceptance-author 冻结实跑双通道红基线（通道 A 227T3F 与锚 0df2c0c3 全同零扰动；通道 B 5T5F 全设计态红，冻结件 commit `a92706c0`）→ builder 落地后通道 A **235 Tests 3 Failures**（227+5 冻结件+3 builder 单测；3 Failures 恰为先存 p1_auxbar AB-A1/A2/A4，零新增红零扰动）→ 通道 B 5 Tests 0 Failures（COMPILE_EXIT=0 零 warning）。CG-X1 满足。
- **设备编译闸（CG-X2）**：VERIFY_FW_BUILD_REQUIRED=1 真编译通过（r4 firmware-build）；keybar_three_slot / story_page_v2 / key_layer_dispatch 三族冻结锚全绿（CG-A4）。
- **Mutation check**：真实树七变异 **7/7 sensitive**（fixer 取证 commit `e25ad6f6`；每变异只打红预测 TEST_CASE，`git checkout` 还原后 sha256 逐字节一致 + 通道 B 复跑回 0F）：①复位值入语音闭集→T1；②过渡窗产语音残留→T2；③摘过渡窗接线→T3；④static 第二真相源回归→T3；⑤盖章闭包摘收口→T4；⑥摘 host 登记→T5；⑦摘会话收尾沿复位→T3。诚实注记：变异 7 打红文本前缀为 CG-A1（收尾沿复位属 [CG-C3]③，T3 同件承载 CG-A1/CG-A2 结构齿），已在 mutationEvidence 内留痕。
- **Docs sync**：r2 红（docRefs 无 CG 锚）→ `d71ec015` 补 plan.md 冻结齿映射节 + debt consumed-by 追记 → r3/r4 恒绿。spec 面零变更申报见 `docs-delta.md` §2。
- **CG-A5（structural）**：未执行——真机目检，human 持板，本 Pod 到门停等；豁免理由见下节枚举。

### structural / suiteGates 豁免逐条枚举（r4 acceptance-coverage：structural=[CG-A5]，suiteGates=[]）

| 条目 | 为何豁免（一句） |
|---|---|
| CG-A5 live-manual | 真机目检观测项（鬼影双闪不可 host 机判，观感验收形态先例 DLG-A6 同款），human 持板执行并留痕，非自动断言故不入 runner 通过面——Pod 到真机门前停等，不豁免后续：supervisor 门以 awaiting-human:real-device 状态停。 |
| firmware-build（r3 scoped 位 executor-skipped） | rigor=lite 包 scoped 相位程序化后置（261015-gov-review-lite-fast-path 判定，VERIFY_GATE_DEFER=1），**final 相位恒跑**——r4 已真编译执行且绿，无实际豁免残余。 |
| acceptance 闸 oracle-pass note（on-target 验收需实机） | 本闸担保「不静默跳过」而非「实机已过」——on-target 观测面由 CG-A5 真机轮承载，闸面如实标注非豁免。 |
| suiteGates | 空集，无 suiteGates 级豁免需要枚举。 |

## 简化债 / 取舍申报台账

- `minimal:` 标记扫描（实现面 8 文件 + 两新建件）：**零命中，本包零新增简化债**。
- builder 取舍申报（plan 冻结面内，非债）：
  - 单事件双同步**未修**（plan 修法③）：1g 预评口径 = 过渡窗门控 + 收尾沿复位已把两次同步的可见伪影（残留键帽）消掉，双同步机制本身良性保留——如真机轮仍见可感知闪烁，走 tighten-only 修订。
  - changeManifest 缩面 4 文件未触碰（kidpal_voice_states.c / kidpal_render_node.cc/.h——种子含、builder 未动）：场景复位落点经 1g 预评裁准 application.cc 收尾沿；层栈收口复用 `kidpal_render_node_story_local_front` 既有通道零新缝。缩面非扩面，白盒对账见下节。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | mcp_server.cc 盖章闭包 | 层栈收口调用 `kidpal_render_node_story_local_front()` 在 `app.Schedule` 闭包区内以裸调用出现，与前推闭包两处共三处同形调用，无单一收口 helper | changed-code hunk mcp_server.cc 盖章/前推两闭包区 + heuristic `shrink`（三处同形可收敛）；但收敛会引入新 helper 符号（帧类型负锚精神 = 零新缝符号优先），且调用点均在同一文件同族闭包 | 无强制动作；若后续包再增同类收口点（第三处以上扩散）再收敛单 helper |
| FYI | judgement-call | taskpackage.json | changeManifest 种子 4 文件（kidpal_voice_states.c / kidpal_render_node.cc/.h 及连带）未触碰，manifest 与实际 diff 存在合法缩面差 | git diff --numstat 实测（`git diff e5361caf HEAD -- firmware/`）+ taskpackage changeManifestNotes 预评落点条款 | 无动作；缩面已在本 packet 白盒对账节与 scope-final §2 留痕 |

（结构闸 PASS/FAIL 不复述为 finding；十闸全绿已记 Verification Evidence。无 BLOCKING、无 SHOULD。）

## Five-Axis Review

1. **正确性/契约**：复位值 `KIDPAL_KEYBAR_SCENE_NONE(-1)` 落语音场景闭集 [0..4] 之外（EQ-3 防御链：出闭集→通用分支仲裁落越界 -1 防御→残留键帽不再被画出，mutation 1 已证）；过渡窗裁决只在云端渲染夹钳窗（`front_layer == SERVER_PAGE` 且本地页仍故事页）接管，VOICE/REMINDER/CHAR_WINDOW/STORY_DELETE 前台是合法覆盖层语境不被抢键帽；Idle 收尾沿复位落位在 voice_end 之前（令该次键帽同步即读默认态），提醒驻留时跳过复位（免熄提醒材料壳）。零 wire 改动、零新帧类型，契约面字面零触碰。
2. **Test quality**：设计态红双通道冻结（227T3F 零扰动 + 5T5F 全设计态红 verbatim）+ 七变异全 sensitive 双向可满足性取证；语音残留键帽负扫描（「说完啦/结束」等字面全域不得出现）承载真实行为断言而非存在性锚；回归网点名三族冻结件防回归缩网（mutation 6 守登记）。
3. **Contract/boundary safety**：`docs/contracts/**` 零 diff；crossEnd 豁免与 detect_changes 实测互证（impacted 18 全落 firmware/main）；复用既有帧与层栈 dismiss 通道对齐 4068dc6c 失败路径形态；帧类型负锚由 CG-A3 断言 + mutation 5 承载。
4. **Operability/security**：收尾沿复位带 corr_id 结构化日志（真机轮可对账）；零新增依赖、零权限面变化；`securityReviewRequired: false`（riskTier=medium 单向守卫声明成立）；keybar 新缝纯 C 无芯片/FreeRTOS/LVGL 依赖，host 与设备树同源。
5. **Maintainability/simplicity**：keybar.c/h 全部 additive 尾部追加，scene 生命周期单源上收（lcd_display.cc 文件级 static 第二真相源退场，负锚 :1022 守零残留——mutation 4 已证）；deletion test：删 `kidpal_keybar_story_transition_slots` 则过渡窗三态裁决散落回 lcd_display sync 区并复归 -1 读缺陷 = 复杂度扩散，深模组信号（留）；三处同形收口调用未收敛（见 Findings NIT，负锚精神下 intentional）。

## Residual Risk

- Known limitation：
  - 单事件双同步机制保留（伪影已消、机制未并——见取舍申报）；
  - 过渡窗门控只在云端渲染夹钳窗生效，纯本地层切换窗依赖收尾沿复位兜底（plan 修法②逐字范围，非在途相位残留由 [CG-C1] scene 复位兜底，mutation 2 红证已锚此分工）；
  - host 套件先存红 3 条（p1_auxbar AB-A1/A2/A4）@ 锚 0df2c0c3，非本包。
- Follow-up recommended：并联包 `261022-p3-record-front-polish-fix`（缺陷③④⑤，front 面）待起；如真机轮仍见双闪再走 tighten-only。
- Human decision needed：**CG-A5 真机目检操作单**（human 持板，与 261022-diag-log-content 收尾真机轮合并执行省烧板；Pod 停等 status=awaiting-human:real-device）：
  1. 翻阅读卡与记录会话全程键帽**无鬼影双闪**（含无语音会话的开卡/翻屏场景——KPLOG005 最刺眼实证形态：阅读卡里翻看仍闪出 75 秒前天气问答残留键帽）；
  2. 记录会话归档成功后**首按 BOOT 即生效**（不再被 overlay_close 白吃一次按键）；
  3. 顺手观测收尾沿 corr_id 日志与 KPLOG005 同位段比对（状态乱跳观感是否消失）。
- decisionCoverage：taskpackage 无 `decisionCoverage` 字段（lite 档未启用），`landsIn: dropped` + `reason: risk-accepted` 条目为空集，无 human 风险接受项需列。

## 白盒复核（actual diff vs changeManifest，`git diff --numstat e5361caf HEAD -- firmware/`）

| changeManifest glob | 实际 diff | 对账 |
|---|---|---|
| `firmware/main/display/lcd_display.cc` | +53/−14（场景单源改造 + 过渡窗接线 + last_known 追踪器） | ✓ |
| `firmware/main/display/keybar.c` / `keybar.h` | +81 / +45（additive：scene set/get/reset + story_transition_slots） | ✓ |
| `firmware/main/display/kidpal_voice_states.c` | 零 diff | 缩面（预评裁准复位落点在 application.cc，voice_states 生命周期零改动） |
| `firmware/main/display/kidpal_render_node.cc` / `.h` | 零 diff | 缩面（复用既有 story_local_front 收口通道，零新缝） |
| `firmware/main/mcp_server.cc` | +12（前推/盖章两闭包收口） | ✓ |
| `firmware/main/application.cc` | +19（Idle 收尾沿复位 + 提醒驻留守卫 + corr_id 日志） | ✓ |
| `firmware/main/test/**` | chrome_ghost.acceptance.c 新建 +1246、chrome_ghost.test.c 新建 +188、host_app/main/CMakeLists.txt 登记 +16 | ✓ |

零 manifest 外实现面文件。detect_changes 复核（per-Pod 图索引可用，未走降级）：seed 58 / impacted 18 全落 firmware/main 预期集群（lcd_display chrome 族 / Application / 渲染节点 / mcp_server / main.cc / 测试面），无预期外符号——详 `scope-final.md` §2。范围判定归 scope-final（Step 4），本节只记账。

## 状态与未竟项

- closeout 三件齐：`docs-delta.md` / `review-packet.md`（本件）/ `pod-retro.md`；`closeout-check.mjs --update-index` 已跑（输出见 integrator 交付摘要）。
- 本包到真机门前停等 human（CG-A5 三项观测），Pod 不自行烧录；之后 planner 跑 supervisor 机械闸（supervisor-verdict-gate）并停等，不执行 merge。
