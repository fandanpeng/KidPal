# Review Packet — 261022-p3-record-front-polish-fix

> integrator 合并门前审查包（2026-10-22，dev/pod-m2 worktree，包基线 `80bc2027`）。verdict：**代码面收口完成、停在真机门前**——r4 final bearing 十闸全绿（fact `ok:true`，verifier-evidence-check PASS），closeout 制品齐；FP-A7 live-manual 真机目检未执行（前置 = front 重部署 ECS，human 持板，status=awaiting-human:real-device），本 Pod 不自行部署，之后交 planner 跑 supervisor 机械闸并停等 human。

## 包事实

- changeId `261022-p3-record-front-polish-fix` · sourceType `regression`（debt 条目 TECH-261022 缺陷③④⑤，KPLOG005 真机轮 human 原话：「所以那个okay不是我说的」「13点那笔故事记录下来了吗，标题叫啥，我咋找不到呢」）· rigor `lite` · riskTier `medium` · ②单包 Lite（triage architect 比例化 review 裁决，human 触点①确认）。
- 验收：FP-A1..A5 行为级 8 齿（`server/acceptance/test_record_front_polish.acceptance.py`）+ FP-A6 suiteGate 套件级回归门 + FP-A7 structural live-manual 真机目检（不入 runner 通过面）。
- 零 wire 形状改动（story_list / story_text / kidpal_speak / start_organizing 既有帧形状零 diff，tiebreak 只变同日到达序），`firmware/**` 零 diff（detect_changes impacted 106 符号全落 server 三模块实测一致）。

## Change Size

- Files changed：**4 实现面文件**（front_app.py +131/−13、story.py +43/−4、prompts/guide-round.v1.md +9/−1、验收件新建 +961）+ docs 面两件（plan.md 冻结齿映射节 + debt consumed-by 追记二，均在 launch 面 `80bc2027` 落盘）+ taskpackage 投影与 verify 证据；实现面净 diff 约 +183/−18（不含验收件）。
- Scope：server/gateway/front（引导节奏 + 素材过滤 + 标题兜底）+ server/gateway/story.py（list_stories seq tiebreak + guide_broadcast_since 锚）+ server/acceptance（pytest 验收件）。
- Split recommendation：单包不分拆（lite 档单主题——记录会话族三缺陷共享同一验收件与同一回归网，且与并联固件包 261022-p3-record-chrome-ghost-fix 零文件交集）。

## Verification Evidence

- **final bearing verification（scope 圈定）**：r4，phase=final，2026-10-22T13:31:17Z→13:31:56Z，invocationId `3b056af4-5b44-464a-b43d-b75f85fd7156`，observedGateExit **0**，fact `ok:true`，`verifier-evidence-check.mjs --phase final --round 4` **PASS**。十闸全绿：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ **firmware-build（工具链可用真编译 14s，xiaozhi.bin 链接通过——本包零 firmware diff，固件编译 = 分支健康锚，一编双证两并联固件包改动）** / contract-lint（1 条先存断链 warning，260923 arch 批路径，非本包）/ acceptance-coverage / mutation-check / docs-sync / security / **unit（scope 交集 9 件真跑：149 passed, 3 subtests passed）** / **acceptance（scope 交集 7 件真跑：98 passed, 3 subtests passed）**。**test-scope 清单落盘路径一行：`delivery/taskpackages/261022-p3-record-front-polish-fix/scope-final.md`**（三分法①正常圈定：per-Pod 图索引可用，detect_changes 包内 base=`80bc2027` seed 77 / impacted_total 106 精确无截断，全落 server/acceptance 55 + server/gateway 18 + server/tests 33，零 firmware 跨面——crossEnd 豁免与实测互证；scope floor 含 FP-A6 点名三族冻结件）。
- **server 验收红→绿轨迹**：acceptance-author 冻结实跑双通道红基线（通道 A 全量 238 passed 与锚 64d54eba 逐字一致零扰动；通道 B 本件 7F1P 全设计态红 + 行为层红点验证，冻结件 commit `c4b499ee`）→ builder 落地后（`6ed67a9f`）**全量 246 passed = 238 基线 + 8 新齿，零新增红**。FP-X1 满足。
- **Mutation check**：真实树五变异 **5/5 sensitive**（fixer 取证 commit `670db89e`；每变异只打红预测齿，`git checkout` 还原后 sha256 逐字节一致 + 通道 B 复跑回 8 passed）：①摘应答窗→FP-A1 两齿；②摘篇幅截断→FP-A2；③摘素材过滤窗→FP-A3；④摘 seq tiebreak→FP-A4；⑤摘标题兜底分支→FP-A5。冻结矩阵逐条命中，零意外返工。
- **Docs sync**：零红轮——r3 scoped 与 r4 final 恒绿（docRefs 锚句在 launch 面 `80bc2027` 随 TaskPackage Finalize 一次到位）。spec 面零变更申报见 `docs-delta.md` §2。
- **FP-A7（structural）**：未执行——真机目检，前置 = front 重部署 ECS，human 持板，本 Pod 到门停等；豁免理由见下节枚举。

### structural / suiteGates 豁免逐条枚举（r4 acceptance-coverage：structural=[FP-A7]，suiteGates=[FP-A6]）

| 条目 | 为何豁免（一句） |
|---|---|
| FP-A7 live-manual | 真机目检观测项（引导不独播 / 「说完了」正常收尾 / 新篇列表最上标题可识别——三者均需设备实听实看，host 机不可判），前置 = front 重部署 ECS，human 持板执行并留痕，非自动断言故不入 runner 通过面——Pod 到真机门前停等，不豁免后续：supervisor 门以 awaiting-human:real-device 状态停。 |
| FP-A6 suiteGate | 套件级回归门（server 全量 246 = 238 基线 + 8 新齿，零新增红）非单齿断言：coverage 闸按单齿形状计则形状不符（r1 红的本体），改标 suiteGate 后由 r2/r3 的全量实跑证据 + r4 scope 交集真跑共同承载，非跳过。 |
| unit 层承载裁定 | requiredLayers 含 unit 但 changeManifest 不含 server/tests/**——planner 裁定行为级 8 齿即承载 unit 层（窗口分支 / 缺 seq 回退 / 截断边界 / 兜底判定全为行为断言，Lite 口径不为边际价值扩 manifest）；实际执行比裁定更严：scope-final §3.4 把 detect_changes 映射到的 server/tests 两件直接单测消费方圈入 scope 真跑（r4 unit 闸 149 passed 含之），unit 闸由 blast radius 而非 manifest 扩面承载，无豁免残余。 |
| firmware-build（r3 scoped 位 executor-skipped） | rigor=lite 包 scoped 相位程序化后置（261015-gov-review-lite-fast-path 判定，VERIFY_GATE_DEFER=1），**final 相位恒跑**——r4 已真编译执行且绿，无实际豁免残余。 |

## 简化债 / 取舍申报台账

- `minimal:` 标记扫描（实现面 4 文件）：**零命中，本包零新增简化债**。
- builder 取舍申报（plan 冻结面内，非债）：
  - 三参数**数值不冻结**（名字冻结、数值手感）：初值取现网实测合理中位（应答窗 8s 与 STUCK_GUIDE_SECONDS 叠加共 16s 覆盖实测 14s 迟到上界；篇幅 30 字留 prompt 20 字指令的容差余量；过滤窗 8s ≈ 30 字语音时长上界），终判归 human 真机轮（FP-A7）——如真机轮手感偏差走数值修订不返工结构。
  - 引导制品措辞硬化（guide-round.v1.md）与代码截断护栏构成双道防线：prompt 面要求 ≤20 字 + 代码面 30 字硬夹紧，截断产物恒为原文前缀不另编内容。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | front_app.py `_story_feed_from_stt` 窗口过滤 | 引导播报窗过滤先于 routing 判定，`pending_stop` 路由同样被滤——孩子停止录音后 8s 内到达的末句转写若恰逢引导播报窗会被静默丢弃（return False 不触发收尾沿），最坏情形 pending 超时释放不归档（素材损失）。stop 已按 = 孩子主动结束信号，此窗内转写几乎必为孩子真话（引导回声在播报期间即到达、不会迟到到 stop 之后），豁免 pending_stop 是严格更优收窄 | changed-code hunk front_app.py:458-460（窗口判定先于 routing 分支）+ heuristic `risk-shrink`（护栏误伤面：FP-A3 反向 case 只测了窗口过期/未开播两形态，未覆盖 stop-后-转写形态） | 不 BLOCKING 本门（冻结验收契约未含该形态，行为不违反任何齿）；建议后续小包或真机轮后修订：`pending_stop` 分支豁免窗口过滤，保留 accumulate 路由设防 |
| NIT | judgement-call | front_app.py `_story_timeout_sweep` 应答窗叠加 | 应答窗 +8s 叠加对**全部** auto-finish due 会话生效（`finish_timeout_s` 全局替换），非仅「末段引导已发」的会话——从未播报引导的会话自动收尾同样延后 8s。安全侧取舍（多等 8s 才收尾、不丢任何收尾机会），非最小影响面 | changed-code hunk front_app.py `_story_timeout_sweep` finish_timeout_s 行 + heuristic `simplicity-over-scope`（按会话区分需引入末段引导态查询，lite 档边际价值低） | 无强制动作；如真机轮反馈收尾偏慢再按会话态收窄 |
| FYI | judgement-call | front_app.py 引导篇幅双防线 | 引导制品 prompt 写 ≤20 字、代码硬上限 30 字——两道防线数值不同为有意容差设计（prompt 面尽量短 + 代码面兜底夹紧），非漂移 | changed-code hunk front_app.py GUIDE_SPEECH_MAX_CHARS 注释段（容差余量语义明文） | 无动作；注释已自证，后人勿「顺手对齐」两数值 |

（结构闸 PASS/FAIL 不复述为 finding；十闸全绿已记 Verification Evidence。无 BLOCKING。SHOULD 一条不阻合并门——不违反冻结齿与契约，属边界收窄建议，交 planner 裁是否随真机轮反馈立修订包。）

## Five-Axis Review

1. **正确性/契约**：应答窗只延后计时起点、轮数语义（AC5「2 轮无回应」）与 is_finish_phrase 词表零触碰（FP-X2 负锚，mutation 1 证敏感）；tiebreak 是契约 §1.3③「日期倒序」之下的实现自由度，items 字段形状 {id,title,date} 零偏移、total 语义零动，账本**纯读**不重建（front 单写者纪律，fetch_story 先例）；素材窗只滤素材面，收尾词判定面不经窗（FP-A1 本体）；标题兜底复用 archive_story 的 _fallback_title 单源，替换形态不另编内容，faithful-organize 制品零触碰。契约面字面零 diff。
2. **Test quality**：设计态红双通道冻结（238 基线零扰动 + 7F1P 全设计态红 verbatim + 行为层红点验证）+ 五变异全 sensitive 双向可满足性取证；缺 seq 回退齿为 must-stay-green 平凡绿锚（钉「不崩 + 纯读」）；FP-A3 反向 case 必载（窗口过期/未开播不误伤）防过滤窗过宽；monkeypatch 注入承载全部参数值（GAP-2 双向可满足性），fixture 自含离线快失败。
3. **Contract/boundary safety**：`docs/contracts/**` 零 diff；crossEnd 豁免与 detect_changes 实测互证（impacted 106 全落 server 三模块、零 firmware）；tiebreak 走 triage 逃生门预查（NO_CONTRACT_CHANGE）且实现未越裁定；三常量模块级单源、所有消费点现读全局（无内联字面量，验收可 monkeypatch）；时钟注入纪律（锚与 now 均不落墙钟，仅生产泵路径缺省墙钟）。
4. **Operability/security**：窗内丢弃 / 兜底替换 / 截断三处均带 corr_id 结构化日志（story_feed_guard dropped=guide_broadcast_window / story_organize_round generic_title / story_guide_round chars=N truncated=true——真机轮可对账）；零新增依赖、零权限面变化；`securityReviewRequired: false`（riskTier=medium 单向守卫声明成立）；启发式纯函数只读模块常量，无 IO。
5. **Maintainability/simplicity**：三参数 + 词表全部模块级单源集中一段（注释承重，数值手感语义与终判位明文）；`_story_guide_window_active` 防御性处理时钟回拨/类型失真（宁可放行素材，不误判过滤）；deletion test：删 `guide_broadcast_since` 锚则过滤窗失去真播报锚、被抑制轮也开窗误吞孩子接话 = FP-A3 反向 case 红，深模组信号（留）；三常量未上升为配置文件——手感参数就地可调 + 验收可注入，lite 档合理。

## Residual Risk

- Known limitation：
  - Findings SHOULD 一条：pending_stop 路由窗内末句转写可被滤（素材损失边界，概率低、有 pending 超时兜底不崩）；
  - 幻听缺陷④的引擎侧候选排除（第三方部署树）不在本包 manifest 内，front 素材面设防 = 应用层单道防线——若引擎层仍有回声候选，KPLOG 链可复验后另立包；
  - 三参数数值为现网中位初值，终判归 human 真机轮。
- Follow-up recommended：并联包 `261022-p3-record-chrome-ghost-fix`（缺陷①②）同批收口；FP-A7 真机轮可与 chrome-ghost CG-A5、diag-log 收尾真机轮合并执行省烧板/省部署。
- Human decision needed：**FP-A7 真机目检操作单**（human 持板，前置 = front 重部署 ECS；Pod 停等 status=awaiting-human:real-device）：
  1. 记录会话全程引导**不再独播垄断**（单段篇幅受限、末段引导后有应答窗）；
  2. 说「说完了」**正常收尾归档**（不再孤儿帧——全天四笔笔笔迟到 5~14s 的实证形态消失）；
  3. 新篇在列表**最上且标题可识别**（不再「我的故事」+ 随机序找不到刚录篇目）。
- decisionCoverage：taskpackage 无 `decisionCoverage` 字段（lite 档未启用），`landsIn: dropped` + `reason: risk-accepted` 条目为空集，无 human 风险接受项需列。

## 白盒复核（actual diff vs changeManifest，`git diff --numstat 80bc2027 HEAD -- server/`）

| changeManifest glob | 实际 diff | 对账 |
|---|---|---|
| `server/gateway/front/front_app.py` | +131/−13（三常量 + 启发式单源 + 窗口过滤 + 应答窗叠加 + 第 3 轮守卫 + 篇幅截断 + 标题兜底） | ✓ |
| `server/gateway/front/prompts/*` | guide-round.v1.md +9/−1（措辞硬化：硬上限 + 播报期不作数 + 不邀请接话） | ✓ |
| `server/gateway/story.py` | +43/−4（list_stories seq tiebreak + guide_broadcast_since 锚 + guide_broadcast_anchor 纯查询 + 新会话清锚） | ✓ |
| `server/acceptance/**` | test_record_front_polish.acceptance.py 新建 +961 | ✓ |

零 manifest 外实现面文件。detect_changes 复核（per-Pod 图索引可用，未走降级）：seed 77 / impacted 106 全落 server 预期集群（front_app 引导收尾族 / story 账本列表族 / 验收与单测消费面），无预期外符号——详 `scope-final.md` §2。范围判定归 scope-final（Step 4），本节只记账（Step 9b advisory 位）。

## 状态与未竟项

- closeout 制品齐：`docs-delta.md` / `review-packet.md`（本件）/ `pod-retro.md` / `scope-final.md`；`closeout-check.mjs --update-index` 已跑（输出见 integrator 交付摘要）。
- 本包到真机门前停等 human（FP-A7 三项观测，前置 = front 重部署 ECS），Pod 不自行部署；之后 planner 跑 supervisor 机械闸（supervisor-verdict-gate）并停等，不执行 merge。
