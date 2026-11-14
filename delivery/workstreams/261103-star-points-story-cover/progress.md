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
