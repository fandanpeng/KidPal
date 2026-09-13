# 260911-display-interaction-sovereignty · progress

> 执行日志（追加式）。

## 2026-09-11 · portfolio-planner 拆包（PKG-1 / PKG-2 / PKG-4）

- 输入：PROPOSAL.md FROZEN v1.0（pin sha1:cd10d1903ac6bb1a / git blob 26138dba，commit 8a06d0c）
  + review.md（F1–F9）+ ARCH-REVIEW-QUEUE 两条 ARQ（cross-package-behavior-owner-gap /
  split-package-implementation-prose-gap，均首用本批）。
- 拆包结果：三包落盘 `.claude/taskpackages/`——
  - `260911-key-route-a1`（PKG-1，mergeOrder 18，dependsOn []）
  - `260911-screen-unification`（PKG-2，mergeOrder 19，dependsOn []，包级 plan.md）
  - `260911-display-first-m1`（PKG-4，mergeOrder 20，dependsOn [screen-unification, ecs-deploy-convergence]，包级 plan.md）
  - PKG-3（回答驻留）随 OQ3=c 撤销不立项。
- 跨包共享冻结面扫描（launch 前预警义务）：未发现常量互斥型冲突；行为碰撞型两处已
  申报（PKG-4 必触 test_g_relay_fix.acceptance.py；PKG-2 渲染类冻结齿路径敏感）——
  均走 acceptance-author 留痕修订通道，不带冲突进 launch。
- KEY owner 登记：`crossPackageBehaviors` 落 decision-coverage.json + 各包 taskpackage；
  KEY(GPIO18) 端到端 owner = 260911-key-route-a1。
- PKG-2 plan.md 落点：`.claude/taskpackages/260911-screen-unification/plan.md`
  （PKG-4 同形：`.claude/taskpackages/260911-display-first-m1/plan.md`）。
- board 初始化：三包 `--set-pod --status queued --merge-order 18/19/20`；
  批次③ 260911-render-pages 占位 mergeOrder 17 → 落锁 21（②先③后）。
- 待办（交 launch 前）：F-03 commit-before-launch（拆包产物 commit 专用 base ref 非 main）
  → launch 侧 `git worktree add`（自 base ref）+ 起 planner 会话（worktree 内跑 claude，
  launch 侧预填 --worktree-path）。

## 2026-09-11 · launch 面准备（主线程记，PKG-1/PKG-2 并行为 human 已裁）

- **调度判定**：board queued 余本 workstream 三包，依赖真值 = key-route-a1(18) dependsOn []
  、screen-unification(19) dependsOn []、display-first-m1(20) dependsOn [screen-unification,
  ecs-deploy-convergence]。ecs-deploy-convergence(16) 已 merged（并 main d087fb7）；screen-
  unification 未 merged → **本批可发 = PKG-1 ∥ PKG-2 两包**（并行为 PROPOSAL §4 总序明示），
  display-first-m1 卡 PKG-2 前置，本批不发。WIP 无 running Pod、余量 3 充足。
- **launch 机械门**：两包 `--check-launch --base-ref main` 全绿（ok:true、blocking/warn 均空）。
- **上游已知红基线（260909 T5 机械来源口径，仓根现跑 @ main d087fb7）**：① server 验收
  `python -m pytest -q --import-mode=importlib -m "not live" c:/AI/KidPal/server/acceptance`
  → **1 failed / 125 passed / 6 skipped / 45 deselected**（唯一红 = test_smoke_e2e.acceptance.py
  ::SMK-A4，owner=260829-smoke-e2e，在册 existing-red-inventory）；② host ELF（WSL linux target
  新编现跑 `firmware/main/test/host_app/build/host_infra_test.elf`，前次产物 9-09 落后源码 9-10
  已增量重编）→ **104 Tests / 3 Failures / 0 Ignored**（WSA-A6:830 / WSA-A4:663 证据齿 + P2-A1:571
  = KSP-A1 first-match 歧义红 owner=批次③渲染批，均在册）。较上批 86/3 净增 18 齿（hardware-
  validation 批）。taskpackage 的 knownRedFloor 应引用本行，不手写数字。
- **环境等价化完成**：worktree `C:/AI/KidPal-pod-key-route-a1` / `-screen-unification`（branch
  dev/260911-key-route-a1、dev/260911-screen-unification，base = main d087fb7）；firmware/sdkconfig
  + dependencies.lock 镜像、managed_components git bash `ln -s` 软链指主 checkout（原生 python
  isdir 两包全 True）、worktree-bootstrap.mjs 两包跑过（.env / shim 面源不存在，按设计跳过）。
- **专属图索引已建（必办项）**：`C-AI-KidPal-pod-key-route-a1`（12151 节点/37654 边）、
  `C-AI-KidPal-pod-screen-unification`（12151/37613）——均 moderate + persistence，nodes/edges
  == expected；skipped 0，parse_partial 62 文件/项目（音频 codec、application.cc:520 等既有
  best-effort 信号，如实记录）。
- board `--set-pod` 预填完毕（两包 queued + worktreePath；mergeToken.holder=none）。
- launch prompt 两份落 `.git/devloop/launch/prompt-{260911-key-route-a1,260911-screen-unification}.txt`
  （VS Code 交互会话形态，human 自跑；headless fail-closed 备选）。已按 260909 新口径写入：
  ① `VERIFY_FW_BUILD_REQUIRED=1` 严格模式注入；② brief-inputs.mjs 接入（crossEnd counterparts
  已填、非 exempt）；③ C 侧 root 定位吃 cwd 指针只登记；④ verifier `notExecuted` 逐条点名义务；
  ⑤ 本次现跑红基线数字逐条。
- **待 human**：① PKG-1/PKG-2 两包 launch 自跑（并行，各自 worktree 的 VS Code 会话；merge-
  driver 仍按 mergeOrder 串行、PKG-1 先并）；② PKG-1 KR-A5 GPIO18 真机确认（持板）；③ display-
  first-m1 待 PKG-2 merged 后再备下一批。

## 2026-09-11 · headless launch + deputy 代裁 live 验证（主线程记，human 当日指示）

- **human 授权（verbatim 要点）**：「你已 headless 的形式启动这两个包，如果开过过程中有疑问
  需要人类角色，你参考 changes/arch/260908-gov-review-human-deputy-shadow/PROPOSAL.md 的定义
  做决定，我们在正好验证一下」——两包由主线程代跑 headless launch（替代上节待 human ①），
  Pod 运行期的人类裁断点由主线程按 deputy-shadow PROPOSAL（DRAFT v0.1）定义代裁，作 live 验证。
- **授权边界（按 canon 草案 §0 自我约束，C-16 不创先例）**：① P-2 九项永不代裁（契约门签字 /
  ESCALATE 合并批准 / remote push / accept-as-risk / 删除归档执行 / 凭据供给 / 裁断权自指 /
  UX·儿童体验观感 / 枚举外新情形）——命中即停 Pod、升级真人 human；② 预裁留痕按 C-14 五要素
  （actor:"human-deputy"，禁伪装 human），落
  `changes/arch/260908-gov-review-human-deputy-shadow/deputy-shadow.md`（live-validation 标记，
  append-only）；③ 本次授权独立成立，不构成 deputy 职权面扩张先例（晋升仍归后续批凭数据提案）。
- **deputy 承载件现状如实记**：PROPOSAL 仍 DRAFT v0.1、三件承载（agent def / canon SSOT /
  协议 SSOT）未 landing——本次验证以仓外 canon 草案 `C:\AI\_human-deputy-mining\canon-draft-v0.1.md`
  为偏好事实源、以 PROPOSAL T2/T3 定义体裁运行；验证数据（HIT/MISS/refuse）反哺该批评审与
  晋升批观察集。
- **launch 形态**：`claude --agent planner --dangerously-skip-permissions -p "<launch prompt 全文>"`
  在各 worktree 内后台起（日志 `.git/devloop/launch/headless-<changeId>.log`）；机械强制层
  （enforce-scope hooks + settings deny + scope-map）在 bypassPermissions 下仍生效，Pod 写面
  照旧受拦。Pod 遇人类裁断点按既有非交互纪律 fail-closed（pending-human-ruling.md +
  needs-triage 退出）→ 主线程 deputy 代裁 → `claude --agent planner -c -p "<裁断>"` 续跑。
- **launch 实况（对照组成立）**：screen-unification 由主线程后台起（classifier 首次放行，
  05:38 intake 翻 running）；key-route-a1 两度被 auto-mode classifier 拦（理由：嵌套 claude +
  skip-permissions 需用户亲笔 named+specifics 授权，AskUserQuestion 点选算 agent-authored 不认）
  → human 亲跑（06:03 board 翻 running）。两包同 prompt 形态、不同起跑人 = deputy 验证对照。
- **SU-RULING-01（首例 deputy 代裁，2026-09-11）**：screen-unification 冻结验收后 planner
  升级「回答期间服务端推送页是否前台可见」——acceptance-author 把 b1/b3 钉为 语音(4)>服务端页(3)
  （回答期间图表被语音页盖住），与 FROZEN PROPOSAL §0「问完先看到」/§2.2 M1/§5「display 页
  上屏不晚于 TTS 首段后 0.5s」正面冲突。主线程派 deputy（fable 档、canon 草案 SSOT、五要素体裁）
  出预裁 = **方案 A**（回答期间服务端页前台可见、聊天 layer 常驻叠加；VOICE 过渡态不拦内容页），
  deviation=false；P-2 九项逐项核对全不触（P-2.8 判据 = A 无需新增/改写任何冻结条文，属框架内
  一致性修复；真机观感确认权仍留 human SU-A8）。条目落
  `changes/arch/260908-gov-review-human-deputy-shadow/deputy-shadow.md`。裁断经 resume 回传 Pod
  （`.git/devloop/launch/ruling-260911-screen-unification.txt`，要求 pod-events 记
  actor:"human-deputy" 禁伪装 human、acceptance-author 修订 b1/b3 留痕引 SU-RULING-01、重跑红
  基线后派 builder）。human 事后复核窗口开：终裁同 A 记 HIT、异记 MISS、自造第三案记 DIFF-CREATED。
- **SU-RULING-02（第二例 deputy 代裁，2026-09-11）**：builder 交付机制层但未做聊天控件物理
  reparent（双物理 LVGL 屏并存、s_voice_ui_scr 改名绕过 s1/s3 齿、回答文字在内容页前台时不可见
  ——「听得见看不见」未根治、OQ3=c 驻留不兑现），planner 升级「方案 1 包内补物理迁移 vs 方案 2
  机制层收口+follow-up」+ 附带裁点配网接线。deputy（fable 档）C-02 亲核全部残项声称属实
  （kidpal_render_node.cc L12 码内自认双屏 / s1 L781-784 只扫旧名 / s3 L866-870 只数创建点 /
  六处入栈调用点零 PROVISION），预裁 = **方案 1 + 配网随包补**，deviation=false；并出**职权
  不对称声明**：裁方案 1 = 执行已冻结包边界（P-0 框架内，deputy 有权）；裁方案 2 = 缩减冻结
  验收 = 框架面 + 风险接受（P-2.4）+ 体验取舍（P-2.8）三重命中，deputy 结构性无权、只能
  refuse 升级——本案无需动用，方案 2 另被 C-07 独立否。条目落 deputy-shadow.md § SU-RULING-02。
  裁断经 resume 回传 Pod（ruling-260911-screen-unification-02.txt：acceptance-author 先收紧
  「单物理屏」齿堵死改名/捕获两条绕过路径 → fixer 补 reparent + PROVISION 接线 → SU-A6 全量
  矩阵；builder 码内「reparent 归批次③殿后」缓行注记作废——批次③冻结面仅观感侧）。真机观感
  确认权原样保留 human（SU-A8）。human 复核窗口开（HIT/MISS/DIFF-CREATED 同前例）。
- **SU-RULING-02 执行轮收口（2026-09-11）**：物理 reparent 完成（聊天控件迁 lv_layer_top、
  真单物理屏）+ PROVISION 配网接线 + 单物理屏齿收紧（堵改名/捕获两绕过路径）；verifier r4
  scoped 七闸绿、r5 final 唯一红 = SMK-A4 在册；host 矩阵 111/3 全在册零新增；设备 IDF 真编
  exit 0（0x2c25c0）；integrator closeout commit 5697317 + supervisor 相位 0d07849；
  supervisor verdict = ESCALATE（high）。board = verifier-green，停等两个 human 节点。
- **PKG-1 全程闭环（human 亲裁链，对照组实绩）**：KR-A5 真机轮 human 持板 PASS（事件
  c0ffe62a：GPIO18 active_high=false 实测稳定、悬空风险销账、七项日志↔观察互证、HWV-A2 三项
  OPEN 顺带闭环）→ ESCALATE 合并批准 human 亲裁（事件 9b192ca6 五要素齐）→ merge-driver 执行，
  **main 现含 379d71d（PKG-1 merge commit）+ 2e10202（簿记）**。零 deputy 介入（全程无升级件
  到主线程）；deputy 侧 KR-RULING-01 为追溯性 refuse 影子留痕。
- **SU-RULING-03/04 + 两 human 节点裁断（2026-09-11）**：deputy 第三单三条目落盘——
  SU-RULING-03 预裁「SU-A8 现在跑」（C-07/C-09/C-13 + 闸不拦欠账的四线机械核验 + 职权不对称
  声明：「延后」带欠账合并风险接受成分 deputy 无权裁）；SU-RULING-04/KR-RULING-01 按 P-2.2
  refuse（ESCALATE 合并批准凭据 actor:"human" 恒归真人）。human 实裁（AskUserQuestion）：
  ① SU-A8 = 现在跑（**终局行 HIT 已回写**——deputy 首个命中率数据点）；② 合并批准 = 真机轮
  PASS 后再呈批（interim 行已回写，终局待批准实裁）。裁断经 ruling-260911-screen-unification-03.txt
  回传：SU-A8 轮 = KR-A5 同模式（agent 烧录 COM5 + 串口捕获、human 持板、PASS 裁定权归 human；
  本 build 无 KEY 接线、BOOT 仍旧过渡语义，「两键一致」子项归 SU-A7 并后复跑不在本轮）。

## 2026-09-11 · integrator closeout（PKG-2 归一屏 260911-screen-unification）

- 封包阶段：builder 实现 + fixer mutation 证据完成，verifier scoped 轮 r4 全绿（7 闸）、
  final 轮 r5 红在 unit/acceptance 两闸，唯一红 = SMK-A4（server smoke-e2e screen.md
  缺失，owner=260829-smoke-e2e，非本包）。
- integrator 全量复跑：host ELF 111 Tests / 3 Failures（3 在册红 WSA-A6:830 / WSA-A4:663 /
  P2-A1:571，本包 7 齿全绿）；server pytest 125 passed / 1 failed（SMK-A4）/ 6 skipped /
  45 deselected；设备 IDF 构建 exit 0。
- closeout 制品落 `changes/dev/260911-screen-unification/`：docs-delta.md（blocking）、
  review-packet.md（blocking）、existing-red-inventory.md（非 blocking）。
- SU-A8 真机持板（语音轮+推送页并发、提醒恰在语音轮到点 0 复位 0 崩溃）标注 pending-human，
  非 host 齿、integrator 不代跑。
- 下一步：feature 分支 commit 冻结 diff → supervisor 合并门（机械闸 supervisor-verdict-gate，
  riskTier=high 升级人类复核）。

## 2026-09-12 · launch 面准备批次②（主线程记，PKG-4 单包）

- **调度判定**：PKG-2 已并（board 16:15 翻 merged，main d80e319 + 簿记 4af97fe/438f85a），
  PKG-4 260911-display-first-m1（mergeOrder 20）dependsOn 双前置全满足（screen-unification
  d80e319 / ecs-deploy-convergence d087fb7）→ **本批 = 单包 PKG-4**，依赖图说了算不硬凑并行：
  render-pages（批次③，mergeOrder 21，pods 目录尚无其 pod json，下批备launch时再登记）按 human
  已裁「②先③后」排后，且其换皮/字体 scope 与本包固件渲染面高度相交，并行必撞冻结面；
  audio-bargein-hw（mergeOrder 14）launch 面 260909 批已备妥（worktree + prompt 俱在），归
  human 另窗口决定，不在本批。WIP：无 running Pod，余量充足；mergeToken.holder=none。
- **红基线（human 指示 verbatim：「红基线那些不要测试，上一个包merge刚测试的，用它的结果」）**：
  不重跑，引 PKG-2 合并收口实测 @ 合并树 d80e319（出处 = 已提交制品
  `changes/dev/260911-screen-unification/existing-red-inventory.md` + `pod-retro.md` 合并夜追记，
  符合 KR retro 教训 #1「口径引已提交 inventory」）：① host 全量 **116 Tests / 4 Failures
  全在册**（WSA-A6:830 / WSA-A4:663 owner=260903-device-ws-switch、P2-A1:571 owner=批次③、
  VRT-A2·E2:878 owner=260829-voice-roundtrip 且 cwd 敏感）+ KEYA 5/5 绿（PKG-1×PKG-2 语义
  相容证据）+ IDF 真编 exit 0（bin 0x2c2e00）；② server 验收 **1 failed（SMK-A4，owner=
  260829-smoke-e2e）/ 125 passed / 6 skipped / 45 deselected**。main 自 d80e319 仅簿记 commit
  无代码面变更；launch prompt 基线段已标注「引用非现跑」出处。
- **launch 机械门**：`--check-launch 260911-display-first-m1 --base-ref main` 全绿
  （ok:true、blocking/warn 均空）。
- **环境等价化完成**：worktree `C:/AI/KidPal-pod-display-first-m1`（branch
  dev/260911-display-first-m1，base = main 438f85a）；firmware/sdkconfig + dependencies.lock
  镜像、managed_components git bash `ln -s` 软链指主 checkout（原生 python isdir=True）、
  worktree-bootstrap.mjs 跑过（.env / shim 面源不存在按设计跳过）。
- **专属图索引已建（必办项）**：`C-AI-KidPal-pod-display-first-m1`（moderate + persistence，
  12396 节点 / 38079 边 == expected；skipped 0，parse_partial 65 文件 = 音频 codec /
  application.cc:524 等既有 best-effort 信号）。注：`server/deploy`（engine-patch 面 = 本包
  主目标）在图排除面内，查该面降级 Grep/Read——已写进 launch prompt。
- **board 预填**：`--set-pod 260911-display-first-m1 --status queued --worktree-path
  C:/AI/KidPal-pod-display-first-m1` 完成。
- **launch prompt** 落 `.git/devloop/launch/prompt-260911-display-first-m1.txt`（VS Code 交互
  会话形态默认，headless 备选需 human 亲笔授权；含 VERIFY_FW_BUILD_REQUIRED=1、brief-inputs
  crossEnd 三 counterparts 非 exempt、C 侧 cwd 指针只登记、阶段 0 reviewer-first 契约复核 +
  M3 禁采、部署纪律 apply-xiaozhi-overlay.sh + engine-patch 在位核验、上一批收口留根 6 条移入
  intake 评估（含 SU-A7「两键一致」并后复跑归本包 DF-A6 收口轮、真机轮实时口令协议、live 会话
  不常驻在线前提、合并闸 --human-approval-ref + --human-approved-exemption 双凭据、tier③
  簿记相交配方、mutation 命名子串教训））。
- **待 human**：① PKG-4 launch 自跑（worktree 内 VS Code 会话，prompt 全文为启动输入）；
  ② DF-A3 时延实测 + DF-A6 workstream 收口真机轮需 live ECS + human 持板（含 SU-A7 两键一致
  复跑；§5 五条收口标准并集对账，本包 = 末位 owner）；③ audio-bargein-hw 是否另窗口穿插 launch
  由 human 定；④ render-pages（批次③）待 PKG-4 并后再备 launch 面。
- **2026-09-13 收口归档（human 定向）**：PKG-1 key-route-a1 / PKG-2 screen-unification / PKG-4 display-first-m1 + 批次③ render-pages 全部闭环合并（render-pages merged 5958026）。workstream 目标（归一屏 / 显示先行 / 回答驻留 / 键位路线）已交付；随 spec 拆分旧批一并归档，后续演进归 phase 2 新结构。
