# progress · 261103-star-points-story-cover（workstream 活文档，append-only）

> 拆包顺序链与 launch 面事实的家。设计权威 = [PROPOSAL.md](PROPOSAL.md)（FROZEN v1.0）；评审 = [review.md](review.md)；顺序链 = [task_plan.md](task_plan.md)。

## 2026-11-05 · K1 launch 面准备（pod-m2 树复用，planner 本会话内联）

**调度判定**：K1（261104-k1-points-ledger-core，mergeOrder 72）dependsOn = [K0a, K0] 均 merged（board 实查 + main 提交 3306abac / ace646e3），WIP 已回 1，串行链下一包 = K1；复用池树 `C:/AI/KidPal-pod-m2`（上一包 K0a 已 merged，常驻分支 dev/pod-m2 已 ff 到 main=85cc3747）。planner 以本会话内联形式跑（human 指示，非 headless；VS Code 扩展会话形态，降级申报见 K1 pod-events settings-presence）。check-launch blocking 空（--base-ref main）；LP9 判定 satisfied：K0a/K0 merged 双证 + K0 spike 真实消费端证据在位（spike-evidence.md §二/三/四），第二笔 QQ canary 已按 2026-11-05 human 修正判据降级顺延 K1/K2，其 K1 侧义务（restore=True 推送接线 + 真实消费端复核）归入本包验收面。**上一包留根**：K0 §六.1 GAP-1（QQ 落账后设备缓存同步推送缺位）交 K1 producer 正式化时一并接线——planner intake 判定为吸收（进 acceptance-author 冻结范围），非 blocking；树交接卫生核过（tracked 净；`sdkconfig.pre-launch-bak-20261104` 为 K0a 前备份残留，现役 `host_app/sdkconfig` 已在、K1 全 server 不涉固件构建，留待 K2 换包交接时清理）。

**基线锚（带锚引用三件齐）**：

- server 面 = **401 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `85cc3747`（2026-11-05 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；量测时 HEAD = 85cc3747；面域 = `server`）。旧锚 6ce77123（395/0）因 K0 改 server/ 判停失效，本行取代。
- firmware host 面：K1 changeManifest 全 server、零固件文件，本包验证不涉 host build；host 面已知红基线沿用 K0 收尾留痕（host 基线 278/3 @ K0a merged 后，见 261104-k0-points-e2e-spike taskpackage K0S-E4 行），K1 verifier 若见 host 红按该留痕判新增红。

**human 遗留项**：无新增（K0 的 gov 登记尾差与远程发布仍归主线程/human，与本包 launch 无涉）。

## 2026-11-04 · 拆包与首批 launch 面（K0a ∥ K0 并行对）

**调度判定**：K0a（261104-k0a-four-page-skeleton，mergeOrder 70）与 K0（261104-k0-points-e2e-spike，71）无互相依赖（dependsOn 均 []），按 human 2026-11-04 对齐的并行对同时 launch；merge 序钉死 K0a 先合，K0 的共享 CMake 追加 / host build / device build / 真机终验 / ready-to-merge 全部后置到 K0a merged 后（LP8）。K0a/K0 合并后回 WIP=1：K1→K2→K3→K4→K5。树分配：K0a → 池树 `C:/AI/KidPal-pod-m2`（host 构建热缓存，固件骨架最合适）；K0 → 池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`（server 为主）。两树 2026-11-04 交接卫生完成：tracked 净、常驻分支复位到 main=22b30654（旧分叉均为已 merged 包的树本地历史，board 在案）、`firmware/main/test/host_app/sdkconfig` 改名 `sdkconfig.pre-launch-bak-20261104`（可逆，防 TECH-261014 陈旧假红；确认重生成无误后可删）。专属图索引随树复用：`C-AI-KidPal-pod-m2` / `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 均在册（list_projects 实查 2026-11-04）。

**门结果**：拆包门 1 两轮 = BLOCK（F-01..F-05）→ planner 六项修正 → 聚焦复检 **GREEN**（verdict 落 [review.md](review.md)「门 1」节）；settings-lint 4 files 0 RED 0 WARN；check-launch 两包 blocking 均空（--base-ref main）；human 过目切法通过并授权 commit（2026-11-04）；commit 22b30654。

**基线锚（带锚引用三件齐；判停 = 消费前 git diff --quiet <SHA> HEAD -- <面域>）**：

- firmware host 面 = **265 Tests / 3 Failures / 0 Ignored** @ SHA `2f2c5871`（2026-11-02 实测；命令 = WSL `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`；3 红全 p1_auxbar AB-A1/A2/A4 存量族；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-11-02 P3 段）。判停 2026-11-04：`git diff --quiet 2f2c5871 HEAD -- firmware/` exit 0 → 引用不重跑。
- server 面 = **395 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `6ce77123`（2026-11-02 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" <pod 树>/server/acceptance`；落盘锚 = 同上 progress.md 2026-11-02 P2 段 server 锚行）。判停 2026-11-04：`git diff --quiet 6ce77123 HEAD -- server/` exit 0 → 引用不重跑。
- 本两包验收红基线（acceptance-author 新冻结齿）恒现跑，无缓存可言。

**human 遗留项**：① launch 命令 human 自跑（两张启动卡已落各包目录）；② K0 的两笔真实 QQ canary 需 human 从真实家长 QQ 发送（LP7 裁定路径：自包含明确指令 + canary token；禁直编 state 文件/禁终端 CLI 冒充/禁本地注入/禁提前建 K3 skill）；③ K0a merged 后 K0 才做共享 CMake 追加与真机终验（LP8）；④ K0 真机轮与 QQ 轮的设备操作窗口需 human 在场。

## 2026-11-05 · K2 launch 面准备（pod-m2 树复用）

**调度判定**：K2（261104-k2-points-device-ui，mergeOrder 73）dependsOn = [K1] 已 merged（board 实查 + main 提交 ae7725cf，human 批 ref=4a610ed9），WIP=1 串行链下一包 = K2；复用池树 `C:/AI/KidPal-pod-m2`（上一包 K1 已 merged，常驻分支 dev/pod-m2=b96b978d，merge-driver rebase 后即 main）。互斥租用检查通过：devloop/pods 扫描无活跃 Pod 落在该树或 dev/pod-m2 分支。LP10 判定 satisfied：K1 merged 双证，points.py 单 writer、删除/兑换不变量、keyset producer 已成可消费基线（K1 verify final 10/10 在案）。**上一包留根**：K1 pod-retro 四条逐条评估归 planner intake；其中 K1L-L1/L2 真机轮按 human 2026-11-05 意向挂 K2 真机轮一起跑，部署轮仍归主线程。树交接卫生核过（tracked 净；`sdkconfig.pre-launch-bak-20261104` 残留已按 K1 卡预告删除，现役 host_app/sdkconfig 在位）。

**门结果**：check-launch blocking 空（--base-ref main）；settings-lint 3 files 0 RED 0 WARN；board 预填 queued + worktreePath=C:/AI/KidPal-pod-m2（portfolio-reconcile 95 pods reconciled）；启动卡落 `delivery/taskpackages/261104-k2-points-device-ui/launch-card.md`。

**基线锚（带锚引用三件齐；判停 = 消费前 git diff --quiet <SHA> HEAD -- <面域>）**：

- server 面 = **411 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `b96b978d`（2026-11-05 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；量测时 HEAD = b96b978d；面域 = `server`）。旧锚 85cc3747（401/0）因 K1 改 server/ 判停失效，本行取代。
- firmware host 面 = **278 Tests / 3 Failures / 0 Ignored**（3 红全 p1_auxbar AB 存量族；K0 收尾留痕，落盘锚 = 261104-k0-points-e2e-spike taskpackage K0S-E4 / LP8 段，量测态 = K0 合并态 ace646e3）。判停 2026-11-05：`git diff --quiet ace646e3 HEAD -- firmware/` exit 0 → 引用不重跑；K2 builder 动 firmware/ 后该引用失效，后续消费方重跑（命令同 K0 锚行）。
- 本包验收红基线（acceptance-author 新冻结齿）恒现跑，无缓存可言。

**环境事实**：专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-05 index_status 实查，21740 节点 / 64106 边，K1 points 面入图；复用树随树沿用，intake 自动增量）。

**human 遗留项**：无新增（K0 gov 登记尾差、远程发布、K1 部署轮仍归主线程/human，与本包 launch 无涉）。

## 2026-11-05 · K2 integrator 收口（离线面）

**final 终验**：scope 圈定清单 `delivery/taskpackages/261104-k2-points-device-ui/test-scope-final.txt`（detect_changes C-AI-KidPal-pod-m2 专属索引 since=b96b978d 正常圈定）；final round 2 承重（invocationId 3fb1729b-b9fa-47bd-9629-d71f696776f2）10 闸全跑零 skip——firmware-build 真编译 fresh 绿、unit 154 passed / acceptance 73 passed / 3 subtests，verifier-evidence-check factOk:true；round 1 留痕（scope 清单注记格式缺陷致 acceptance 闸 executor-skipped，格式修正后重跑）。host 287/3 判停续用（c1de02fe..HEAD firmware 零差异）。圈定合跑零误伤（8 件 server 面逐件实测 154/0）。

**closeout**：docs-delta.md（零文档变更）+ review-packet.md（demo 对账 E1/E2/E5 绿、E3/E4 pending 真机轮）+ overlap-probe.json（no-overlap；deviceOutcome required=true：device-round-evidence 待真机轮 + protocol.cc 申报登记集收窄项交 planner）落包目录；closeout-check PASS indexProjection inserted；pod-closed 事件 8f95f0f2 在 pod-events.jsonl。制品与三件测试面改动随后由 integrator commit 进 dev/pod-m2（不 push）。merge 门交 planner 跑 supervisor 闸；K2PTS-L1/L2/L3 真机轮（含 K1L-L1/L2 挂接）human 在场安排。

## 2026-11-14 · K3 launch 面准备（pod-m2 树复用）

**调度判定**：K3（261104-k3-qq-points-channel，mergeOrder 74）dependsOn = [K1, K2] 均已 merged（main 76ed332e，K1 @ ae7725cf），WIP=1 串行链下一包 = K3；复用池树 `C:/AI/KidPal-pod-m2`（上一包 K2 已 merged，常驻分支 dev/pod-m2=45518ac2=main）。互斥租用检查通过：devloop/pods 扫描无其他活跃 Pod 落在该树或 dev/pod-m2 分支。**LP11 判定**：K1/K2 merged 双证满足；「真实 QQ 验证窗口 + 设备联调条件」为 human 在场条件，设备轮阶段前确认（真实歧义输入不卡死是必做证据）。树交接卫生核过（tracked 净、untracked 无残留；本包 changeManifest 全 server，不涉固件构建）。**上一包留根**：K2 pod-retro 六教训与流程债交 planner intake 逐条评估；K2 真机轮（K2PTS-L1/L2/L3 + K1L-L1/L2 挂接）与 K3 的 K3Q-E4 设备观察共用同一 human 设备窗。

**门结果**：check-launch blocking 空（--base-ref main），launchPreconditions 余 LP11 pending（human 条件，见上）；settings-lint 3 files 0 RED 0 WARN；board 预填 queued + worktreePath=C:/AI/KidPal-pod-m2（portfolio-reconcile 95 pods reconciled）；启动卡落 `delivery/taskpackages/261104-k3-qq-points-channel/launch-card.md`。

**基线锚（带锚引用三件齐；判停 = 消费前 git diff --quiet <SHA> HEAD -- <面域>）**：

- server 面 = **415 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `45518ac2`（2026-11-14 实测，本段同日现跑；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；量测时 HEAD = 45518ac2；面域 = `server`）。旧锚 b96b978d（411/0）因 K1/K2 改 server/ 判停失效，本行取代。
- firmware host 面 = **287 Tests / 3 Failures / 0 Ignored**（3 红全 p1_auxbar AB 存量族；量测态 = K2 final `c1de02fe`）。判停 2026-11-14：`git diff --quiet c1de02fe HEAD -- firmware/` 之外另核 `76ed332e..HEAD -- firmware/` 为空（合并后无固件改动）→ 引用不重跑；本包 changeManifest 零固件文件，验证不涉 host build。
- 本包验收红基线（acceptance-author 新冻结件）恒现跑，无缓存可言。

**环境事实**：专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-14 index_status 实查，21960 节点 / 64713 边，K2 固件面入图）。

**human 遗留项**：① 真实 QQ 验证窗口与设备联调窗口需 human 在场（LP11）；② K2 真机轮与 K3 设备观察建议合并一次设备窗；③ 远程发布与部署轮仍归主线程/human，与本包 launch 无涉。

**勘误（2026-11-14，K3 launch 段上文两处修正；append-only 不改旧行）**：上文「K2 真机轮…与 K3 设备观察共用同一 human 设备窗」「human 遗留项②K2 真机轮与 K3 设备观察建议合并一次设备窗」有误——K2 真机轮 + 复验轮**已跑完并全 PASS**（2026-11-06 首轮 / 2026-11-14 复验轮，`delivery/taskpackages/261104-k2-points-device-ui/device-round-evidence.md` §一/§五，human 收口原话「可以了，合并吧」，合并 @ 76ed332e）。其中 `K1L-L1 外部写入不抢前台` 已实证 front `points_observe` 观察 CLI 写入并自推恰一帧 restore，K3 的 QQ 写入可复用该观察缝。K3 仍需**自己的**真实 QQ 设备观察轮（QQ 消息 → front push → 设备不抢前台/积分页原地刷新，K3Q-E4），该轮需 human 在场（LP11）。

## 2026-11-15 · K4 launch 面准备（pod-m2 树复用；叠跑 K3，human override）

**调度判定**：K4（261104-k4-reward-goal-config，mergeOrder 75）dependsOn = [K1, K2, K3]。实况 = K1/K2 merged，**K3 未 merged**（supervisor ESCALATE，等 human 批 + 真实 QQ 设备轮）。LP12 要求「K4 等 K1、K2、K3 merged 后方可 launch」——human 2026-11-15 原话「继续开发K4，中国时间明天上午到时候一块验证」构成**明确 override**：K4 叠在未合并的 K3 之上开工，base ref = `cad07f2c`（K3 HEAD），合并序仍钉死 K3 → K4。K4 合并门前必须先完成 K3 合并与 K3 的 live 证据。K3 的 live 轮（K3Q-E4）与 K4 的真机轮（K4G-E4）合并到 2026-11-16 上午一次设备窗。

**K3 当前状态（交接事实）**：离线闭环完成（scoped 8/8、final 10/10、supervisor ESCALATE）；已部署到 ECS（`cad07f2c`，drift 0，front 重启、openclaw-gateway 重启后 skill 可见、QQ dad/mom 双通道 READY、CLI `show/recent` 实测可用）。未合并。

**门结果**：check-launch blocking 空（--base-ref main）；LP12 pending 按上述 override 处理；settings-lint 3 files 0 RED 0 WARN；board 预填 queued + worktreePath=C:/AI/KidPal-pod-m2；启动卡落 `delivery/taskpackages/261104-k4-reward-goal-config/launch-card.md`。

**基线锚（带锚引用三件齐；判停 = 消费前 git diff --quiet <SHA> HEAD -- <面域>）**：

- server 面 = **435 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `cad07f2c`（2026-11-15 实测，本段同日现跑；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；面域 = `server`）。该基线含 K3 的 server 改动。
- firmware host 面 = **287 Tests / 3 Failures / 0 Ignored**（3 红全 p1_auxbar AB 存量族；量测态 = K2 final `c1de02fe`）。判停 2026-11-15：`git diff --quiet 76ed332e HEAD -- firmware/` exit 0 → 引用不重跑；K4 动 firmware 后该引用失效，builder 必须重跑并判新增红。
- 本包验收红基线（acceptance-author 新冻结件）恒现跑。

**环境事实**：专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-15 实查，随树增量）。

**human 遗留项**：① K3 合并批（supervisor ESCALATE 需 approve-escalated-merge）；② 2026-11-16 上午设备窗：K3 真实 QQ 加兑查删/歧义/重复 + K4 目标配置与刻度联动，一次跑完；③ K4 合并排 K3 之后；④ 远程发布仍归主线程/human。

## 2026-11-15 · K4 离线收口（叠跑 K3；真机轮待设备窗）

**final 终验**：scope 圈定清单 `delivery/taskpackages/261104-k4-reward-goal-config/test-scope-final.txt`（12 件：本包 2 冻结件 + 11 条 impacted 种子去 1 非测试件 + crossEnd seam + smoke 余量）；final round 1 承重（invocationId `7d3f9c1e-5b2a-4e8f-9a61-c2d4b8f0a7e3`）10 闸全跑零 skip——firmware-build 真编译生成 xiaozhi.bin、unit 110 passed、acceptance 41 passed（firmwareAcceptanceFiles=5 / serverAcceptanceFiles=3）、verifier-evidence-check factOk:true。host 全量 **292 Tests / 3 Failures**（3 红全 p1_auxbar 存量族 AB-A1/A2/A4；292−287=5 = K4G-H1..H5 全 PASS，零新增红）。

**实现**：四枚场景资产（`sc_choco/sc_movie/sc_gift/sc_flag_42_42.bin`，各 268B，纯追加 4A/0M；`sc_wheel` 复用 K2 件）+ `kidpal_points_scene_asset` 五键映射 + `kidpal_points_scale_x` 刻度几何 + render_node 3 槽 strcmp 按名重载 + pointstarget 三扁平动词（target-add/switch/delete）+ SKILL.md 目标管理改写。commit `e87310de`。

**冻结件修正与扩单（如实留痕）**：K4G-A1 不可满足断言、K4G-A7 与 A2 冲突——acceptance-author 最小修正并重冻（revision log 在案）；`read_state` additive 两键致 K0 精确三键断言失效——architect 扩单 APPROVE 后 append `server/tests/test_points_spike_units.py` 并按最小 diff 改（投影 equality + 出厂两键 + receipts 负断言）。commit `63d24c5f` / 扩单在 `e87310de`。

**closeout**：docs-delta.md（零 canonical 变更，理由成立）+ review-packet.md（无 BLOCKING/SHOULD，3 NIT；design-review 文本证据在场；8 条未覆盖面全部如实申报"未跑"）+ verification.json + closeout-check --update-index exit 0。integrator commit `e68713a1`。

**合并门**：`supervisor-verdict-gate` = **ESCALATE**（riskTier high 结构性；evidenceComplete=true，completedRounds=1）。前置升级闸会在合并期因 `deviceOutcome{userVisible:true, kidpal_render_node.cc, K4G-L1}` 缺 device-round-evidence 而拦停——预期。pod-retro 落盘，planner 收尾 commit `98619ffa`。

**未跑 / 下一步**：K4G-L1 真机轮（真实 QQ 目标配置 + 看屏 + 不抢前台 + 时延 + RAM）与 K3 live 轮合并到下一个设备窗；K4 的 ECS 部署轮（points.py/SKILL/四枚资产）归主线程；合并序 K3 → K4，两包合并均需 human 批（supervisor 均 ESCALATE）。

## 2026-11-15 · K3/K4 合并入 main（叠跑栈，单次 merge）

**合并**：`merge-driver --pod 261104-k4-reward-goal-config --human-approval-ref 010e2fe8…`（human 2026-11-15 原话「合并吧，删了这种情况再观察一下」）→ merge commit `b8c53b89c11f`，main 簿记头 `22684638`。K3 的提交为 K4 合并提交的祖先（叠跑栈），无独立 K3 merge commit；K3 board 已归 merged，K4 board merged。

**合并门**：K4 supervisor ESCALATE（high 结构性，completedRounds=2，final r2 承重轮 factOk）；前置升级闸 PASS（device-round-evidence K4G-L1 human-visual PASS，binary sha 35c71e…，sourceCommit 2127a19f 为 HEAD 祖先；human-approval-ref 五要素核验 OK）。

**真机轮结果**：K4 目标配置全过程 + 场景标修复后看屏（摩天轮↔通用旗↔摩天轮）+ 不抢前台全 PASS；K4G-L1 闭环。K3 真实 QQ 加/查/删 PASS；**歧义删除未闭环**（三次「昨天/今天那条」被模型猜删，SKILL.md §5.1 + AGENTS.md 常驻纪律已部署但运行层未复测通过；main agent 模型 deepseek-v4-flash），按 human 裁定观察后续，已在 K3 device-round-evidence 与 review-packet 如实申报。

**未做**：远程发布（`release-pr.mjs --publish` 只推 release/* 并开 PR，PR 合并仍 human）；K5 未开。

**教训**：叠跑两个包在同一分支，使 merge-driver 的「单 pod diff ⊆ manifest」假设失效——本次以 K4 为栈代表合并、K3 作为祖先随批进入并单独归 board；后续叠跑应避免跨包 live 修复落在对方 manifest 之外。

## 2026-11-15 · K5 launch 面准备（池树复用；列车最后一包）

**调度判定**：K5（261104-k5-story-cover-nav，mergeOrder 76）dependsOn = [K2, K4]，两者均已 merged 本地 main（K2 @`76ed332e`、K4 @`b8c53b89`）；**LP13 核销**。WIP=1 串行链尾，本批单包。复用池树 `C:/AI/KidPal-pod-m2`（上一包 K4 已 merged；常驻分支 `dev/pod-m2` 先 rebase 到 main 清理两处分叉簿记——`8b0b0543`/`25deb165` 重放为 `78e4cbf2`/`03630b65`）。互斥租用检查通过：board 扫描无其他活跃 Pod 落在该树或 `dev/pod-m2` 分支。

**门结果**：check-launch blocking 空（`--base-ref main`），launchPreconditions LP13 满足（见上）；settings-lint 3 files / 0 RED / 0 WARN；board 预填 queued + worktreePath=`C:/AI/KidPal-pod-m2`；启动卡落 `delivery/taskpackages/261104-k5-story-cover-nav/launch-card.md`。

**基线锚（带锚引用三件齐；判停 = 消费前 git diff --quiet <SHA> HEAD -- <面域>）**：

- server 面 = **447 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `03630b65`（2026-11-15 实测，本段同日现跑；命令 = `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；面域 = `server`）。旧锚 435 @`cad07f2c`（K4 launch 段）因 K3/K4 合并改 server/ 判停失效，本行取代。
- firmware host 面 = **292 Tests / 3 Failures / 0 Ignored** @ SHA `03630b65`（2026-11-15 实测，本段同日现跑；执行目录 = `firmware/main/test/host_app`（HOST_TEST_CONVENTIONS [C4.4] 文档命令位）；命令 = 仓根 `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` 后 `cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`；面域 = `firmware`）。3 红全 p1_auxbar 存量族（AB-A1 电池图标 / AB-A2 温度图标 / AB-A4 wifi 资产）。旧锚 292/3 @K4 final `c1de02fe` 判停：`git diff --quiet c1de02fe HEAD -- firmware/` 非零 → 重跑取代。交接卫生：已按换包协议删除 gitignored 的 `host_app/sdkconfig`，本次构建从 tracked `sdkconfig.defaults` 重生成（消 TECH-261014 陈旧假红类）。
- 本包验收红基线（acceptance-author 新冻结件）恒现跑。

**环境事实**：专属图索引 `C-AI-KidPal-pod-m2` ready（2026-11-15 实查，22215 节点 / 66109 边）。

**human 遗留项**：① 列车级真机 + 真实 QQ 联合收口轮仍归主线程另组织（K5 只收导航并集）；② 远程发布仍归主线程/human；③ K3 歧义删除观察项未闭环（按 human 裁继续观察）。

**教训留根（K4 pod-retro 转入 K5 intake）**：预算按次数量级给；冻结件跨用例一致性自检；manifest 含生成件须对照命名律；acceptance-coverage 闸不认 Unity 断言（沿 K2/K4 先例补真实守卫）。
