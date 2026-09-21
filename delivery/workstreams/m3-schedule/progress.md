# progress · m3-schedule

> 执行日志（append-only）：改了什么、测了什么、失败/修复。行不覆盖。

## 2026-09-21 · 拆包（portfolio-planner）

- 输入 = `delivery/milestones/milestones.jsonl` 行 `M3-schedule` nextAction 三块（sourceType=delivery-story）；story 机查两道通过（S-F1.1/S-F1.2/S-F1.3/S-F1.5 无已砍命中、可行性全 ✅）。
- 拆 3 包：`260921-m3-k1-cloud-schedule-producer`（mergeOrder 33）/ `260921-m3-k2-p2-week-grid-page`（34，dependsOn K1）/ `260921-m3-k3-voice-schedule-entry-live`（35，dependsOn K1+K2）。
- 重叠面核查结论（详 decision-coverage `_doc`）：无在办/已合并包触课表面冲突；旧 schedule_p2 冻结验收板/云两侧已 archive 退役；prompt v6 槽位空闲。
- **契约触点裁定：触契约面**——E1 §1.3② schedule view 旧列表形状 ↔ requirements v1.19 周网格口径 = spec↔contract 漂移 + 形状重定义；决策 26 首页 upcoming 下发通道契约无承载定义。CONTRACT-CHANGE-PACKET 草案落 K1 包目录，先走 architect-reviewer 四分类预审 + 人类契约门，契约 diff 随 K1 同 PR（launchPreconditions 第 1 条）。
- 真机半安排：K1/K2 的 live 半全部 structural deferred 挂 K3 设备轮（单真机不两轮撞车，M2 先例）。
- 四件套 + task_plan.md（INDEX 状态单一源）+ 双 input-brief（brief-inputs.mjs 拆包侧同批产物）落盘；roadmap ② 节 render-roadmap.mjs 机械渲染。

## 2026-09-21 · 门 1 + 契约预审（主线程派发，同日）

- **门 1（architect 轻量 checklist）**：首轮 verdict: BLOCK(F1)——三包 taskpackage `story` 字段写成技术方案句、违反 story 锚点引用冻结纪律（其余四项全过、实现现状四主张抽查属实）。主线程同日修复（story 字段改锚点引用 + ADV-1 决策 26 字面偏差注记补 K1 plan 取舍点 C），architect 复核转 **verdict: GREEN**。裁决耐久记录 = review.md 门 1 节。
- **契约预审（architect-reviewer 四分类，并行）**：触点①（schedule view 周网格）= SPEC_CONTRACT_DRIFT、触点②（home 可选 data）= CONTRACT_CHANGE_REQUIRED，状态 PRECHECKED；漏问点 a–f 已并入 CONTRACT-CHANGE-PACKET v2（Human Question 扩为五问：形状与 day 基 / 甲案与连带改写 / home data additive / upcoming producer 归属 / 过渡口径）。gate evidence 骨架落 K1 包 `architecture-gate-evidence.md`（十节，§3 收编预审裁定表）。
- **停等 human 两判断点**：① 门 1 后半 = 过目切法（GREEN 已在案）；② 人类契约门裁五问（K1 launch 硬前置）。commit-before-launch（base/m3-schedule）在两判断点后执行。

## 2026-09-21 · human 两判断点落定（同日）

- **过目切法**：human 认可三包拆法（roadmap ③ 节过目）。
- **契约门 HUMAN_APPROVED**：human 裁五问（packet §5 全文）——Q1 扁平周网格形状 + day 0 基映射 + **备注不进网格载荷**（网格/首页只放课程名，备注露面处 = 语音答复 S-F1.3 AC4 + 提醒 speech S-F1.4，human 问例「舞蹈课带护膝」确认口径）；Q2 甲案（薄载荷 front 展开 + §1.5/§1.6/§2.3 连带改写一并批）；Q3 home data additive；Q4 upcoming 由 front 机械算（human 确认 front = 自有网关、话术归 OpenClaw）；Q5 校验收紧。launchPreconditions 第 1 条翻 verified（evidence = packet §5 + gate evidence §4）。
- commit-before-launch：拆包产物 commit 到 base/m3-schedule（自 main 88b2ff82 建，仿 base/m2-char-window 先例）。

## 2026-09-21 · launch 面就绪（本批 = 单包 K1）

**调度判定**：本批 = 单包 K1（mergeOrder 33）。依赖图说了算——K2 dependsOn [K1]、K3 dependsOn [K1, K2] 均未就绪；当前无 running Pod、无 merge token holder。树池串行复用第 8 棒（K1a → N1b → K2 → K3 → K4 → rma8 → m2-charwindow-display-fix → 本包），同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（display-fix merged 后 merge-driver rebase 步已同步至 main 88b2ff82，不换不删）。

**commit-before-launch（F-03/F-05）**：拆包四件套 + 双 input-brief + workstream 制品 + milestones.jsonl planningRefs 挂接，已 commit 常驻分支 `40aefa4`；专用 base ref = `base/m3-schedule`（指向 40aefa4）。`--check-launch 260921-m3-k1-cloud-schedule-producer --base-ref base/m3-schedule` → ok:true，blocking 空、warn 空。

**上游已知红基线（260909 T5 机械来源口径，本树现跑 @ 40aefa4）**：
- **server 面**：`python -m pytest -q --import-mode=importlib -m "not live" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/tests" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **47 passed / 0 failed**（2026-09-21；面域 = server；archive 归档件由根级 pytest.ini norecursedirs 挡递归。m2-char-window 2026-09-17 锚行「server/tests 37 passed」后被 260920 display-fix 新增 test_char_meaning_merge 10 齿扩至本基线，引用以本行为准）。零红。
- **firmware host 面**：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && ./build/host_infra_test.elf`（树内热缓存增量编译）→ **63 Tests / 0 Failures / 0 Ignored**，exit 0（2026-09-21；面域 = firmware。M2 期 35 Tests 经 rma8/display-fix 等批扩至 63）。零红。
- Pod intake 按 260915 T3 差异判停协议消费本段：`git diff --quiet 40aefa4 HEAD -- server/`（或 `-- firmware/`）零差异 → 引用本行不重跑；有差异 → 同命令重跑该面。

**环境事实**：图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（status ready，17345 nodes / 46026 edges，随树自动增量已跟上——server/gateway 面符号可达已实证）；固件环境等价化沿用树内既有 sdkconfig / dependencies.lock / managed_components 软链（host 构建刚实证 63T/0F，跳过重建）；board worktreePath 已预填本树。

**launch 启动卡**：已落 `delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/launch-card.md`（260921 T2 新落点，Git-tracked 随本段同 commit；五要素齐）。

**launchPreconditions 回写**（launch 执行者核验）：① 契约门 verified（packet §5 + gate evidence §4）；② commit-before-launch verified（40aefa4，本段）；③ 门 1 GREEN + human 过目 verified（review.md 门 1 节 + human 2026-09-21 过目认可）；④ 真机硬依赖 = package:K3（K3 launch 前再核，本批不涉及）；⑤ 冻结面与槽位 verified（archive 双侧在案、prompt v6 槽位空闲 2026-09-21 已核）；⑥ 图索引 verified（本段环境事实）。

**遗留 human 项**：launch 由 human 执行（交互会话形态默认）——本 skill 不代跑 launch。命令行两形态见启动卡第五要素。




## 2026-09-21 · 契约 diff 落地（主线程，human 乙通道授权，K1 Pod 启动前）

- **CCP 落地**：`260921-m3-k1-cloud-schedule-producer/CONTRACT-CHANGE-PACKET.md`（HUMAN_APPROVED §5 五问裁定）→ 主线程起草逐字 patch（4 hunk，+35/−10）→ human 过目确认 → `git apply` 落盘 `docs/contracts/interface-contract.md`（§1.3② 周网格形状重定义 breaking + §1.3⑥ home 可选 data.upcoming additive + §1.5/§1.6/§2.3 连带改写 + 两处变更记录行）。
- **随包收口义务同批完成**：schedule.py 模块头钉行更新（doc 零行为）；CDQ-260921-m3-k1-schedule-grid 登记（LANDED）；ADR-004 追加；gate evidence §5–§10 回填（Status → LANDED，C1–C4 全 done）；evidence/validate_display 三样本 ALL-SAMPLES-GREEN。
- **验证**：patch 4 hunk 与 packet §1 触点一一对应（人工逐字核对降级，ARQ-260827 在册）；server 面 `pytest -m "not live"` → 47 passed / 0 failed（与基线锚一致，零新增红）。
- **K1 影响注记**：契约面已在本分支 working tree——K1 dev-loop 起跑时 intake 应将本批 diff 计入基线（redBaseline 面无行为变化）；K1 builder 不再触碰契约耦合钉行；Q5 front 收紧与 A2–A4 实现齿照 plan 归 Pod。

## 2026-09-21 · K1 Pod 开跑（acceptance 相位完成）

- **基线重锚**：契约落地批 24bb1b8 动了 server/（schedule.py 钉行 doc 零行为）→ 按 260915 T3 差异判停协议 server 面视为有差异；落地批自证 47 passed 零新增红 + Pod 内实跑复核一致（既有 47 全绿保持），基线数值不变、锚更新为本批。
- **intake**：上一包 pod-retro 五条留根逐条评估，无 blockingDecision；`VERIFY_FW_BUILD_REQUIRED=1` 声明随卡。
- **契约落地核对（planner 逐字）**：4 hunk == packet §1 + §5 五问裁定，PASS；C1–C4 收口（gate evidence LANDED）。裁决点留观 1 条：§1.3⑥ 示例 `preview.time="明早"` 与 home_p1.h seam `hp1_item_t.time ≤5 字节` 冲突——K1 齿按 seam 兼容口径钉 preview.time=明日首项 HH:MM，「明早」判设备渲染标签（K2 落），如 human 契约门本意是载荷直发标签需回裁。
- **acceptance-author 冻齿**：17 齿落 `server/acceptance/test_m3k1_schedule_producer.acceptance.py`（13 RED + 4 must-stay-green，红基线 13 failed/51 passed 实跑落账 taskpackage.json）；7 条遗留裁决点中 5 条为钉深申报、Q5 收紧落点=validate_display（planner 采认，契约 §1.5 落地文同口径）、preview.time 见上条。A5 live 半 structural deferred 挂 K3。

## 2026-09-21 · K1 合并落账（Pod 全程闭环）

- **质量面**：17 齿全绿（13 RED→GREEN + 4 must-stay-green），全量 75 passed / 0 failed；final r5 九闸 PASS（evidence `delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/verifier-evidence/rounds/r5.json`）；review-packet 零 BLOCKING（1 SHOULD + 2 NIT 留档）。
- **合并**：ESCALATE（diff 重算 critical：触契约 critical-path）→ human 批准 → 升级闸硬编码拦契约批（`hasContractGateEvidence` 恒 false，证据接入未实现——gate-tooling 缺口，escalation `1a13b9a2`）→ human 裁乙案拆批：契约批 `24bb1b8` 先行治理 lane ff 合 main，实现批经 merge-driver 标准链合并（merge commit `ce56940`，human-approval-ref `36840095`）。
- **收尾**：INDEX 双 lane 重建（K1 行 closed）+ escalation 销账 + K2/K3 origin 枚举预检修正（`1548267`）。K2 dependsOn K1 解锁，launch 由 human 走 launch-prep；K2 起包时 preview.time 裁决点（契约示例「明早」 vs seam HH:MM）与分界时刻留痕随包交接。
- **治理跟进项（执行测出，归 /loop-retro 提炼）**：① 升级闸契约证据探测接入（hasContractGateEvidence 硬编码缺口）；② 起包 checklist 补 current/ 指针刷新；③ dispatch-stamp 恒传 --task-package-id；④ loop-retro 阈值已达（15 ≥ 10）待 human 发起。

## 2026-09-21 · K2 launch 面就绪（本批 = 单包 K2，human 指定本树续跑）

**调度判定**：本批 = 单包 K2（mergeOrder 34）。依赖图说了算——K2 dependsOn [K1] 已 merged 解锁；K3 dependsOn [K1, K2] 仍被 K2 挡住；当前无 running Pod、mergeToken 无持有者。树池串行复用第 9 棒，同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（K1 收尾两 commit rebase 到 main cfdcdba 之上，新头 e6a62a9，main ⊆ HEAD 已同步；不换不删）。

**launch 机械门**：`--check-launch 260921-m3-k2-p2-week-grid-page --base-ref base/m3-schedule` → ok:true，blocking 空、warn 空；launchPreconditions 本包相关已核 4 条、无待核（另 2 条属其他包已折叠）。

**上游已知红基线（260909 T5 + 260915 T3 差异判停协议）**：
- **server 面**：`git diff --quiet 40aefa4 HEAD -- server/` 有差异（K1 实现批 +1349 行）→ 重跑：`python -m pytest -q --import-mode=importlib -m "not live" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/tests" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **75 passed / 0 failed**（2026-09-21；量测 @ e6a62a9，面域 = server；launch 批仅动 delivery/ 与 current/ 指针，server 面续引无需再跑）。零红。
- **firmware host 面**：`git diff --quiet 40aefa4 HEAD -- firmware/` 实测零差异 → **引用不重跑**：63 Tests / 0 Failures / 0 Ignored，exit 0（2026-09-21 @ 40aefa4，面域 = firmware，上一段锚行 verbatim）。
- Pod intake 按差异判停协议消费本段：对相应面域先 `git diff --quiet e6a62a9 HEAD -- <面域>`，零差异引用本行、有差异同命令重跑。

**环境事实**：交接卫生核过（tracked 净 + untracked 净，build/ 与 managed_components 软链白名单在场——host 构建热缓存随树）；固件环境等价化沿用树内既有 sdkconfig / dependencies.lock / managed_components 软链（复用树跳过重建）；图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（status ready，17461 nodes / 46504 edges，自动增量已跟上 K1 后代码面）；board 已预填 K2 → 本树（--set-pod queued，mergeToken 无持有者）。

**launch 侧三件**：① board worktreePath 重指（本段）；② 基线差异判停（上段）；③ 启动卡已落 `delivery/taskpackages/260921-m3-k2-p2-week-grid-page/launch-card.md`（五要素齐 + 交接注记：dispatch-stamp 恒传 --task-package-id、mutation-check 先读提取器、preview.time 裁决点 K2 落渲染标签口径、M3K2-A7 live 半 deferred K3）。`delivery/taskpackages/current/` 指针已刷新指向 K2（K1 retro 改进项②由 launch-prep 侧办结）。

**遗留 human 项**：launch 由 human 执行（交互会话形态默认）——launch-prep 不代跑。命令行两形态见启动卡第五要素。

## 2026-09-21 · K2 Pod 完成落账（acceptance→builder→verifier→integrator 全程闭环）

- **acceptance 相位**：11 齿落 `firmware/main/test/schedule_grid.acceptance.c`（A1 版式区钉 / A2 今天列 / A3 标记 / A4 解析降级 / A5 注入接线 / A6 回归靶）；红基线两段落账 taskpackage.json——既有齿基线 63 Tests 0 Failures exit 0（@ 36d1519 回归靶零偏移）+ stub 红基线 7 Tests 5 Failures exit 5（红全在真实缺失行为：grid parse 缺失 ×2 / inject_weekday 缝缺失 / home_upcoming_apply 缺失 / mcp 路由未消费）。形状依赖就绪：K1 契约门 CLOSED + `24bb1b8` 已在分支基线，A4/A5 形状齿随批冻齐（launch-card 交接注记兑现）。
- **builder 相位**：schedule_page.c 整页重写（旧列表版式退役，五函数→两 FROZEN OFFLINE CONTRACT 函数）+ cache_schedule 网格化（84 定容）+ mcp_server 新形状消费与 home 注入接线 + 板级 PCF85063 0x08 星期寄存器带出映射（周一=0 基）+ `home_upcoming.{c,h}` 接线层新文件（home_p1 冻结 seam 零修改）。mutation 14/14 RED→GREEN（counterEvidence 回填 taskpackage.json；原始日志 `C:/tmp/ksg_mut2/` 仓外临时区落账后可弃）。
- **verifier 相位**：scoped r1 红（taskpackage-required current/ 指针未随回填刷新，治理投影时序非实现缺陷）→ 回写后 r2 七闸全绿（evidence `verifier-evidence/rounds/r2.json`）。
- **integrator 收口**：final r3 九闸全绿（`6375d877-217c-4b40-ac52-38875ba87de5`，ok:true / observedGateExit:0）+ evidence-check exit 0；host runner 全量独立复跑 **74 Tests / 0 Failures / 0 Ignored**（wrapper 脚本文件取码 build exit 0 + run exit 0，`VERIFY_FW_BUILD_REQUIRED=1` 内联；既有 63 齿零偏移 + 新 11 齿全绿）；device build 闸 exit 0（xiaozhi.bin 0x30c2a0 bytes, 0x63d60 bytes (11%) free，Windows 原生 IDF 5.5）。closeout 制品落 `delivery/changes/dev/260921-m3-k2-p2-week-grid-page/`（docs-delta + review-packet + verdict.json + verification.json）；review-packet 零 BLOCKING（1 SHOULD + 2 NIT 留档；demo 对账七项全一致 + 未覆盖面 6 条挂账）。
- **挂账**：M3K2-A7 真机半（版式 1bpp 目测/翻页手感/今天列观感）deferred 归 K3 设备轮（M3K3-A3）；板级零点跨天重读 RTC 未落本包（重启恢复正确，K3 前修注释或落重读线）；host 子集缺 ※/★ 字形走程序化兜底（设备真字形观感归 K3）。K3 dependsOn [K1, K2] 中 K2 待合并解锁。

## 2026-09-21 · K2 合并落账（merge complete）

- **合并**：merge-driver 本地 `--no-ff` 合入 main（merge commit `618bd4dfadd4`，human-approval-ref=`8bcdd601-7b45-4ba6-ad1c-d7db00b3475f`）；机械 supervisor-verdict 闸产 ESCALATE（riskTier high 声明先验升级，provenance v2.0 过 supervisor-gate-check）→ human 批准 → pre-merge escalation gate PASS → merge OK，merge token 已释放。board 置 merged。
- **合并前分支链**：e11aaa5 feat（实现 + 验收 + closeout 四件）→ 2e3471a chore（pod-retro + supervisor-verdict + pod-events）→ eeaab30 chore（Step 0 治理面收口）。
- **human 在场实测随批交接（K3 设备轮 / 部署）**：① 真机空网格降级行为实证正确（courses=0 → 「暂时没有数据」不盲渲染，A4 降级语义真机侧目击）；② 设备日期时间错误根因 = ECS 网关未下发 server_time（ota.cc settimeofday 无输入），随 K1 部署面一并消解、非 K2 缺陷。
- **解锁**：K3 dependsOn [K1, K2] 全 merged，K3 设备轮可 launch（M3K3-A3 真机确认轮含 K2 挂账三项：版式目测/翻页手感/今天列观感 + 格内 >4 门裁剪与「明早」渲染标签显示形态目击）。

## 2026-09-21 · K1 live 半热修（human 批准跳过流程，诚实留痕）

- **线上实测发现 K1 缺陷**：三触发推送（建连/零点/即时）全部被降级成「这张小卡片没显示好」text 兜底，首页「接下来」恒空。根因 = `_relay_schedule_payload` 把 dict 载荷直接塞进冻结中继 `build_display_tool_calls` 的 blocks 列表，而该冻结入口只对 str 块 json.loads，dict 判「not JSON-object parseable」一律降级。本地 17 齿测 payload 产出 + validate_display 单元面，未覆盖 dict-穿-中继集成形状（live 半 structural deferred 的漏网形状）。
- **热修**：`_relay_schedule_payload` 先 `json.dumps` 成 str 再入中继（只动 K1 调用点，不碰 260907-g-relay-fix 冻结四 callable）。全量 server 75 passed 零回归；部署 ECS 后探针复验 = 建连推送真发 view=home + upcoming 数据。
- **流程豁免留痕**：human 2026-09-21 批准「跳过流程热修」（在场等真机效果场景），主线程直接改自有实现面，本段即诚实记录。K1 契约耦合钉行零触碰。
- **随轮登记的独立缺口（未修）**：① 语音轮结束后 ~36s WS 被对端关闭、设备静默掉线至按键才重连（front 无关线日志，疑 NAT/keepalive 面，待查）；② 设备时间错误 = 服务端 OTA 响应缺 server_time 下发（ota.cc settimeofday 无输入，独立小变更）；③ 课表页网格空 = 预期内——薄意图展开接线归 K3（voice-schedule-entry-live）。④ 首页 P1 CJK 殿后清单立项形状（2026-09-21 真机轮与 human 对齐）：固定标签（接下来/明天/今天 等封闭集合）烘焙小资产 + 动态内容（课程名/事项名，开放集合）走字体——复用网格页已接的 lv_font_source_han_sans_sc_14_cjk，P1 软帧管线补「LVGL 字体栅格化进帧」画字 helper + 版式落位，待立项。

## 2026-09-21 · K3 launch 面就绪（本批 = 单包 K3，树池串行复用第 10 棒）

**调度判定**：本批 = 单包 K3（mergeOrder 35）。依赖图说了算——K3 dependsOn [K1, K2] 均已 merged（K1 @ 08:16Z、K2 @ 11:58Z）解锁；无其他可跑包、WIP 无压力、mergeToken 无持有者。同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch` 续跑（不换不删）。分支 vs main 实测 ahead 3 / behind 2——merge-driver rebase 式合并正常形态：树内内容为超集（含 K1 live 半热修 85e7d48，随 K3 合并回 main；main 侧 2 commit 为合并侧事件记录，分支已自带等价收尾账），树内开发无缺口。

**launch 机械门**：`--check-launch 260921-m3-k3-voice-schedule-entry-live --base-ref base/m3-schedule` → ok:true，blocking 空、warn 空；launchPreconditions 待核 1 条 = 真机持板硬依赖（M3K3-A3 设备轮固有，K1/K2 host 半已 merged 满足其前半条件），其余 4 条已核、1 条属其他包折叠。

**上游已知红基线（260909 T5 + 260915 T3 差异判停协议）**：
- **server 面**：`git diff --quiet e6a62a9 HEAD -- server/` 有差异（K1 live 半热修动 front_app.py）→ 重跑：`python -m pytest -q --import-mode=importlib -m "not live" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/tests" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **75 passed / 0 failed**（2026-09-21；量测 @ 91b3b5c，面域 = server）。零红。
- **firmware host 面**：`git diff --quiet 36d1519 HEAD -- firmware/` 有差异（K2 实现批 +2861/−640）→ 重跑（WSL host runner build + run）→ **74 Tests / 0 Failures / 0 Ignored**，exit 0（2026-09-21；量测 @ 91b3b5c，面域 = firmware；既有 63 齿零偏移 + K2 新 11 齿全绿）。零红。
- Pod intake 按差异判停协议消费本段：对相应面域先 `git diff --quiet 91b3b5c HEAD -- <面域>`，零差异引用本行、有差异同命令重跑。

**环境事实**：交接卫生核过（tracked 净 + untracked 净，firmware/build 与 managed_components 白名单在场——host 构建热缓存随树）；固件环境等价化沿用树内既有面（复用树跳过重建）；图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（status ready，17534 nodes / 46802 edges，自动增量已跟上 K2 后代码面）；board 已预填 K3 → 本树（--set-pod queued）；`delivery/taskpackages/current/` 指针已刷新指向 K3。

**launch 侧三件**：① board worktreePath 重指（本段）；② 基线差异判停（上段）；③ 启动卡已落 `delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/launch-card.md`（五要素齐 + 交接注记：M3K1-A5/M3K2-A7 双 live 半归本包设备轮、板级零点重读 RTC 遗留评估归属、prompt v6 部署 ECS 需 human 确认窗口、K1 热修轮两独立缺口如实记录）。

**遗留 human 项**：本次 launch 由 human 显式指示主线程代跑（交互会话内派 planner agent），偏离启动卡「human 自跑」默认形态并在此留痕；M3K3-A3 真机确认轮仍需 human 持板开窗，prompt v6 部署 ECS 窗口需 human 确认。
