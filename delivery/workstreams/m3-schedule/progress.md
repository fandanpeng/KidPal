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

## 2026-09-21 · K3 设备轮进行中 + 挂账④立项落包（triage ②单包 Lite）

- **v6 部署 ECS 完成**（human 批准，主线程执行）：6 文件推送（v6 prompt + apply 脚本 + provider.config + K1 遗留 reminder_schedule_lib + front_app.py + schedule.py，远端各留 `.bak-260921-k3-v6`）→ apply 渲染 → v6 prompt 注入 .config.yaml（容器内 grep 实证 4 命中）→ 引擎/front 重启双绿（5 组件初始化成功；venv python 导入 expand_schedule_intent_payload + front 截获接线实证）。
- **设备轮 offline 链已目击**：「帮我显示课表」意图 → 自动切 P2 → 网格有课（A2 链通）；「明天有什么课」问答框答复正常播报（S-F1.3 主验证点过）。已知缺口复现：设备时钟 tm_year 70（OTA 缺 server_time）→ 今天列高亮落周五（实为周日）——独立缺口如实挂账非 K3 义务。首页「接下来」固定标签 + time（ASCII）可画、CJK 课程名缺失 = 挂账④驱动实证（数据已到设备）。QQ 录入三触发项待 human 修 OpenClaw 后续跑。
- **挂账④立项落包**：human 立项 ack（动态内容用对话同款普通字体）→ triage 门 architect 四档裁决 **②单包 Lite / riskTier medium**（human 确认不改判；字体双分支实证收敛：设备侧 font_puhui_14_1 即对话页同款）→ 落包 `delivery/taskpackages/260921-m3-k4-p1-cjk-dynamic-text/`（plan.md + taskpackage.json，origin.type=human-confirmed-plan 指向本文件挂账段）。调度：排 K3 合并后串行。

## 2026-09-21 · K3 收口（integrator：设备轮制品 + closeout 四件 + final r2 九闸全绿）

- **M3K3-A3 设备轮落记录**：`delivery/changes/dev/260921-m3-k3-voice-schedule-entry-live/device-round-record.md`（human 目击设备轮 OK，2026-09-21 晚）。三源逐字对账（串口 c:/tmp/k3_round1.log 30min 完整捕获 + ECS journalctl + 引擎 docker logs，integrator 收口实拉）：AC2「帮我显示课表」意图链完整目击 ×2（22:11:37 courses=37 / 22:44:01 courses=40，均自动切 P2）；AC1 问答框答复不切页、日期准；AC4 备注读回（书法班时间地点进答复）；三触发①联网建连三度到端目击、②journalctl `trigger=midnight sent=1`（22:00:34，如实注记 = v6 部署重启补推非真零点 tick）、③ human 目击 + QQ 录入门数 37→40 全通路实证。A3 两齿随记录落盘转绿。
- **归因更正（诚实留痕）**：派单素材将 851.5s `view=home upcoming_apply ok` 归因为「三触发③录入后即时推送」；三源关联显示该时点（22:19:59）早于录入的设备侧实证时点、与 farewell 轮同拍（v6 口径「说再见 → home 空」）——记录已按证据更正，③ 的 journalctl `trigger=immediate` 全量回溯 0 命中（lib 已随部署批上 ECS 仍零 firing）转 K1 部署面复核项（review-packet Findings SHOULD + 未覆盖面④）。
- **K1/K2 deferred 真机半收口**：M3K1-A5 ①确认/②通路级/③human 目击+37→40（机器日志半缺位显式挂账）；M3K2-A7 切页/网格确认，※/★/「明早」/14px 四细节 human 未逐项口述——按 human 实际确认范围记录，不扩写。
- **verify 链**：scoped r1（verifier，7 闸全绿，offline 半程）→ final **r2 九闸全绿**（integrator，`d4839306-bfe9-4f4e-9716-d106dbd8ead2`，ok:true / observedGateExit:0；unit 95 passed、acceptance 41 passed 零新增红）+ verifier-evidence-check exit 0（factOk:true）。integrator 独立复跑 `server/acceptance/` 41 passed（本包 14/14）+ `server/tests/` 54 passed。
- **closeout 制品**：`delivery/changes/dev/260921-m3-k3-voice-schedule-entry-live/`（device-round-record.md + review-packet.md + docs-delta.md + verdict.json + verification.json）；review-packet 零 BLOCKING（1 SHOULD + 2 NIT；demo 对账一致带挂账 + 未覆盖面 8 条逐条显式化）；docsSyncRequired=false 义务核清（spec/契约/prompt 历史版本零触碰）。
- **状态**：commit 冻结 diff 后交 planner 跑 supervisor-verdict-gate 机械闸；不 push、不 merge。

## 2026-09-21 · K3 合并收尾（planner：机械闸 ESCALATE → human 批准 → merge-driver 进 main）

- **机械合并门**：supervisor-verdict-gate 产 **ESCALATE**（write-once 落盘 supervisor-verdict.json；G6 diff 重算 medium→high：`all-mock:test-files-no-real-db/event-assertion` 判据命中 + adjudicate 规则 critical/high→ESCALATE）；supervisor-gate-check exit 0（provenance v2 全过：mechanicalSupervisorGate.ran=true、bearing fact=r2 ok、closeout refs 安全）。board 置 blocked 停等。
- **human 批准升级合并**（主线程 AskUserQuestion 拍板，制品过目后确认：ESCALATE 按 high 档接受）→ pod-events human-decision `7a929952`（decision=approve-escalated-merge，eventId 即人批凭据）→ `merge-driver --pod --human-approval-ref`：approval-ref 五要素核验 OK → 升级闸 PASS → rebase OK → quick-check 2 闸绿 → **merge OK**，main tip `72c654a`（merge commit 携 human-approval-ref）；Step 5b merge-completed `56b2557b` 落 pod-events；board 置 merged；token released（next holder: none）。
- **批内随进**：K4 立项 commit `40ce8c7`（纯 delivery 制品，human 已知悉属预期）；分支实现 commit 以 rebase 形态进 main（`91b9d0c` 收口 + `24d58e5` Step 0 治理面 3 文件 + `b032d39` pod-events 簿记）。
- **合并后收尾**：dev INDEX K3 行 closed 在场（integrator closeout-check --update-index 刷入，K1/K2/K3 三行齐、arch lane 无涉免重建）；分支 pod-events 同步 merge-completed 事件（受门 Edit 补行，非重定向）；progress 本段；收尾 commit 落分支（树池续跑，分支 vs main 收敛）。
- **欠账移交（不随本包闭合）**：① `trigger=immediate` ECS 取证——K1 部署面复核项（review-packet SHOULD：journalctl 全量回溯 0 命中、疑 OpenClaw 容器 drop 入口未触达/交接目录错位）；② loop-retro 阈值 advisory 17——主线程发起（run /loop-retro）；③ milestones.jsonl M3-schedule 状态（K3 merged 后是否关里程碑）——主线程判定；④ 设备时钟（OTA 缺 server_time）/ WS ~36s 掉线族——K1 热修轮两独立缺口维持挂账。

## 2026-09-22 · K4 launch 面就绪（本批 = 单包 K4，树池串行复用第 11 棒）

**调度判定**：本批 = 单包 K4（`260921-m3-k4-p1-cjk-dynamic-text`，mergeOrder 36）。依赖图说了算——K4 dependsOn []，K1/K2/K3 全 merged、mergeToken 无持有者；挂账④立项批（triage ②单包 Lite，human 确认）随 K3 合并已进 main。同树同常驻分支续跑（分支收尾 commit dc6094a 领先 main 属树池惯例、树内超集，合并时随批回 main）。

**launch 机械门**：`--check-launch 260921-m3-k4-p1-cjk-dynamic-text --base-ref base/m3-k4` → ok:true，blocking 空、warn 空、本包前置无待核（另 2 条属其他包折叠）。base/m3-k4 新建 @ `40ce8c7`（K4 立项 commit，专用 base ref 非 main 惯例）。

**上游已知红基线（260909 T5 + 260915 T3 差异判停协议）**：
- **server 面**：`git diff --quiet 91b3b5c HEAD -- server/` 有差异（K3 实现批）→ 重跑 → **95 passed / 0 failed**（2026-09-22；量测 @ dc6094a，面域 = server；K3 新增 20 齿并入基线 75→95）。零红。
- **firmware host 面**：`git diff --quiet 91b3b5c HEAD -- firmware/` 实测零差异 → **引用不重跑**：74 Tests / 0 Failures / 0 Ignored，exit 0（2026-09-21 @ 91b3b5c，面域 = firmware，上一段锚行 verbatim）。零红。
- Pod intake 按差异判停协议消费本段：对相应面域先 `git diff --quiet dc6094a HEAD -- <面域>`，零差异引用本行、有差异同命令重跑。

**环境事实**：交接卫生核过（工作树净）；图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（status ready，随树自动增量）；board 已预填 K4 → 本树（--set-pod queued，mergeOrder 36，mergeToken 无持有者）；`delivery/taskpackages/current/` 指针已刷新指向 K4；milestones.jsonl M3-schedule state planning→**in_delivery**（对齐 M1/M2 词汇；完全关闭无注册表先例，留 human 拍板）。

**launch 侧三件**：① board worktreePath 重指（本段）；② 基线差异判停（上段）；③ 启动卡已落 `delivery/taskpackages/260921-m3-k4-p1-cjk-dynamic-text/launch-card.md`（五要素齐 + 交接注记：plan.md 边角 UTF-8 截半容错、E2 铁律冻结面清单、字体双分支实证、真机目测挂窗、在案欠账勿吸清单）。

**遗留 human 项**：K4 launch 执行形态待 human 指定（主线程代跑 / 交互自跑 / headless，命令行见启动卡第五要素）；loop-retro 阈值 advisory 17 待 human 发起（第三次提请）。

## 2026-09-22 · K4 Pod 跑完 + 合并落账（planner Pod，主线程代跑 launch）

- **Pod 闭环**：intake 四点探测全命中（board running）→ acceptance-author 冻结 `firmware/test/p1_cjk_name.acceptance.c`（6 TEST_CASE，红基线两通道实跑：既有 74 齿零偏移全绿 + 新齿 stub 独立红 5/6，红因 = 字形 seam 未实现真实缺失）→ builder 实现 [S1] 字形 seam 三落点（home_p1.c 弱默认 + kidpal_render_node.cc 设备强符号 font_puhui_14_1 + host_infra_seam.c Source Han）+ UTF-8 逐字形 blit + host_app 注册面接线 → host runner 80 齿全绿（74 零偏移 + 新齿 6）→ scoped r3 五闸绿 → final r4 九闸绿（xiaozhi.bin 0x30c5b0、app 分区余量 11% free——A6 build 闸兑现）。
- **冻齿 TEST 2 算术缺陷修复**：builder 升级 + planner 亲核（pcn_diff_stats out_total 整帧语义 → TEST 2 求和 3× 重复计数、D==3D 与 total>0 互斥）→ acceptance-author 最小补丁（total 改单次整帧计数 D）+ REVISION LOG + 判别力两层保全（tooth 1 右界带直扫 + tooth 2 并集外 diff）。A2 去 clip 变异复验仍转红，齿强度未弱化。
- **verifier 轮红 ×2 均 planner 元数据**：r1 contractRefs 空数组（补冻结契约面引用）、r2 mutationEvidence 短 id 不匹配全 id 判据（全 id + sensitive:true 回填）。代码面零红轮。
- **合并门**：机械 supervisor-verdict 闸 APPROVE（riskTier medium，provenance v2 全过）→ 前置升级闸 PASS → merge-driver --pod 本地合并：merge commit `f8703d5`（main @ `6eeb43d` 簿记），token 释放。Review Packet findings 零 BLOCKING（1 NIT：mutation 执行面留痕粒度，planner 裁不阻塞——红基线 stub ≙ A1 变异态在案 + A2 复验在 REVISION LOG）。
- **E2 铁律全绿**：home_upcoming.{c,h} 零改、docs/contracts/** 与 spec 零改、schedule_page/K2 面零触碰、hp1_item_t 与 11 冻结契约函数零偏移、固定标签烘焙资产零改（diff 闭集 + 冻齿 canary 双证）。
- **遗留（非阻塞）**：① P1CJK-A6 真机目测三点（课程名可见/与 time 基线协调/14px 1bpp 可读性）挂设备轮窗口——按 kidpal-serial-monitor 协调协议向 human 约开窗（可独立开窗或随下次设备轮）；② loop-retro 阈值 18 advisory 持续挂账（主线程发起责任）。

## 2026-09-22 · K4 live 轮 A6 FAIL 修复 + 版式对齐追加轮（planner Pod，human 两判断点）

- **A6 首轮真机目测 FAIL**：设备侧 CJK 课程名乱码（碎点断笔），ASCII time 正常、host 80 齿全绿。human 指示先只读根因调查再修——planner 调查定案：font_puhui_14_1（bpp=1、PLAIN、fdsc->stride=0）raw A1 位图为跨行连续位流（LVGL lv_font_fmt_txt.c 官方解码循环 stride_in==0 时位计数器不逐行复位），强符号按字节对齐行索引致 box_w 非 8 倍数字形逐行漂移；host Source Han bpp4 偶宽字形两寻址碰巧等价故 host 全绿（假绿盲区）。
- **human 拍板**（pod-events c4785230）：B 方案（去 req_raw_bitmap=1，走 LVGL 官方解码到 A8 静态 draw_buf、阈值 128 化 1bpp、box 上限收 32×32、双强符号同改）+ 版式对齐 WF-F1-01：大 imgfont 时间退役、「接下来」烘焙大字标签（新资产 p1_label_jiexialai.bin，83×30）、文字行全 14px 逐字形 blit 行流语义（ROW1/2/3 + 正常换行）、末行「明早」烘焙标签（p1_label_mingzao.bin，38×20，文案非「明天」）+ HH:MM + 首项名并入主体第三行、倒计时贴纸随时钟缺口挂账不做；固定标签殿后由 human 显式解除（立项 ack）。
- **冻齿修订**：PCN 原地修订（A1–A6 重钉行带 + 新增 A7–A10：烘焙标签在场/行内时间值承载/末行预告构成与三重退役负断言/满载换行压力）+ HPB 跨包修订窗口（p1_home_battery tooth2/4/5，授权链 = human 拍板事件原文）。追加轮两处冻齿缺陷均 planner 亲核 + acceptance-author 修（TEST 9 tooth4 live/快照帧测量笔误）。
- **实现与验证**：builder 一次实现全部设计面 + 资产管线（tools/asset-pipeline/gen_kidpal_labels.py，KAP1 格式照抄先例）+ art_assets.c host 接线；host runner **83 Tests / 0 Failures**（既有 74 零偏移）；verifier scoped r5 + final r6 九闸全绿（xiaozhi.bin 0x30c340、11% free——体积较修复前还小，B 方案静态增量被版式退役对冲）；E2 铁律全绿（既有资产零删改、契约/spec/home_upcoming/schedule_page 零触碰）。
- **收口**：commit `2d60006`（分支 dev/260915-m1-k1a-key-layer-dispatch，树池惯例领先 main 随下批回 main）；review-packet 追加轮 addendum 五节（因果链/demo 对账/冻齿缺陷入册/E2 表/未覆盖面申报）。
- **遗留（非阻塞）**：① **真机复验半挂设备轮窗口**（五项目测：B 方案后 1bpp 可读性、「接下来」大字标签、「明早」文案、行流版式与线框一致、右上区三重退役无残墨）——主线程按 kidpal-serial-monitor 协调协议约窗烧板（xiaozhi.bin @ commit 2d60006 构建，0x20000 保 NVS）；② loop-retro 阈值 18 advisory 持续挂账。

## 2026-09-22 · K4 live 第 2 轮 FAIL → 设备字体上 host → 真根因修复（第 3 追加轮）

- **live 第 2 轮部分 FAIL**：art 分区补烧后「接下来/明早」烘焙标签完美渲染（资产管线/版式骨架 OK），seam 文字行仍乱码且形态改变（稀疏破碎疑似叠印，非第 1 轮斜切）——B 修复改变行为未踩中根因。human 硬要求：设备字体上 host 实证复现后再修（pod-events 6ad46b62；烧录面教训 app+art 双分区已留痕）。
- **A11 冻结**：font_puhui_14_1（纯 C）编入 host runner + [S6] 追加缝 `kidpal_p1_font_glyph_for_font`（与 [S1] 共享解码核心、纯 additive）+ 独立参考解码器 oracle（零调用官方解码入口，bpp1 连续位流 / bpp4 阈值判别双面 + advance 尺度化 + 行尾 padding 齿）。
- **host 复现成功 = 真根因**：`font->get_glyph_bitmap` 返回 `lv_draw_buf_t*` 结构指针，旧实现（含 B 方案）强转像素指针读的是结构头垃圾（随内存布局漂移 = 两轮乱码形态不同的解释）；`lv_font_get_glyph_dsc` 不清 req_raw_bitmap 栈局部、旁路官方包装归一化是次生嫌疑一并显式清零。两 TU 同构修复（[S1]=固定字体调核心、[S6]=参数化核心）；r7 设备 TU C++ const cast 编译红由 fixer 单点补丁闭环。
- **验证**：host runner **84 Tests / 0 Failures**（83 零回归 + TEST 10 五 tooth）；verifier scoped r8 + final r9 九闸全绿（xiaozhi.bin 0x30c340、11% free，A11 零体积增量）；E2 铁律全绿（[S6] 纯 additive、既有资产/契约/spec/home_upcoming/schedule_page 零触碰）。
- **收口**：commit `14a27f6`（分支领先 main 随批回 main）；review-packet 第 3 追加轮 addendum（因果链/demo 对账/治理收益/未覆盖面）。
- **治理收益**：设备字体 × 设备解码路径的 host oracle 覆盖永久入册——live 路径字形回归从此免烧板。
- **遗留**：① 真机第 3 轮复验挂设备轮窗口（seam 乱码消除目测 + 烘焙标签回归 + app+art 双分区烧录）；② loop-retro 阈值 advisory 持续挂账。

## 2026-09-22 · K4 真机第 3 轮 PASS + 合并收尾（planner Pod）

- **human 真机第 3 轮复验 PASS**（原话「这次对了，提交合并吧」pod-events 560daa86）：seam 文字行乱码消除（第 3 追加轮真根因修复生效）、烘焙标签/版式目测通过——A6 真机半闭环，全验收齿闭环。
- **合并链**：supervisor-verdict-gate 依 fail-loud 指引以新 bearing 轮（final r9 九闸绿）refresh stale verdict → **APPROVE**（riskTier medium，completedRounds 9，provenance v2 全过，supervisor-gate-check exit 0）→ 前置升级闸 PASS → merge-driver --pod：rebase OK / quick-check OK / **merge OK**——merge commit `74b6929`（main @ `29749c0` 簿记），追加轮三批（2d60006+75d01b5、14a27f6+7dec9de）随批回 main，token 释放。
- **K4 全包闭环**：主轮（合并 f8703d5）→ 追加轮 1（A6 live FAIL 根因 + B 方案 + 版式对齐）→ 追加轮 2/3（设备字体上 host 复现真根因 draw_buf 结构头误读 → 修复 + A11 host oracle 永久入册）→ 真机第 3 轮 PASS。治理遗产：设备字体 × 设备解码路径 host 免烧板覆盖、行流版式 [S3]/[S5] 资产契约、独立 oracle 齿设计先例。
- **持续挂账（非本包义务）**：loop-retro 阈值 18 advisory（主线程 /loop-retro）；设备时钟缺口（贴纸随时钟恢复另立项）。

## 2026-09-22 · 时间缺口 triage 建包（主线程 /triage-change）

- K3 设备轮挂账「设备时间错误」立项推进：主线程实测修正根因——引擎 OTA 端点实发正确 `server_time`（timestamp UTC ms + offset 480），真因 = 方向 B 短路激活后 `ActivationTask()` 不再调 `Ota::CheckVersion()`（全固件唯一 settimeofday 点）且固件无 SNTP，真机串口 `tm_year: 70` 实证。
- `/triage-change` 派 architect 比例化 review → **②单包 Lite**（不触契约面/spec 冻结面，逃生门不命中）；F1 范围裁定 human 拍板 **(a) SNTP + 写回 PCF85063 + 周期 re-inject**、F2（P1 boot 快照冻结）随包一并修。
- 建包 `260922-m3-k5-sntp-time-sync`（plan.md reviewed plan + taskpackage.json 骨架，intake 时 planner 补齐验收面），挂本 workstream（attach-existing），排常驻树第 9 棒。下一步 = 人类起 planner 会话跑 `/build-feature 260922-m3-k5-sntp-time-sync`。

## 2026-09-22 · K5 launch 面就绪（主线程 launch prep，树池续棒）

- **树池**：常驻树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch` 续棒。launch prep 开树时树面状态：常驻分支落后 main 四笔（ui-v5 gov 批，仅 docs/delivery 面）且领先两笔（K4 收尾落账 `7a5d909` 未回 main——K4 合并后落账 commit 留在了常驻分支）——已 rebase 到 main（`20df3cb`），closeout 现为 `6cb4824`、立项落包 `b054660`。**K4 收尾落账此前未回 main，随本批 pod 合并回 main**（merge-driver rebase 步自然携带）。
- **launch 侧异常处置**：`delivery/taskpackages/260922-m3-k5-sntp-time-sync/taskpackage.json` 原始落盘截断损坏（` .time": ""` 残片 + iterationBudget 后尾部全缺），launch prep 对照 K4 立项骨架修复补齐（补 contractRefs/docRefs/sourceRefs/fixBudget/notes/crossEnd 等尾部字段），JSON 校验过（35 字段）。plan.md 完好未动。
- **上游已知红基线锚（差异判停 2026-09-22，两面引用不重跑）**：
  - server 面锚：`git diff --quiet dc6094a HEAD -- server` 零差异 → 引用 K4 launch 段锚「server 95 @ dc6094a」（量测命令 `python -m pytest -q --import-mode=importlib -m "not live" <repo>/server/acceptance`，2026-09-22 量测于 dc6094a，面域 = server）；
  - firmware 面锚：`git diff --quiet 9cae452 HEAD -- firmware` 零差异 → 引用 K4 第 3 追加轮段锚「host runner 84 Tests / 0 Failures」（量测命令 WSL host runner `firmware/main/test/host_app espidf_wsl.sh build` + `host_infra_test.elf` run，2026-09-22 量测于 9cae452（main 侧同内容双生 SHA `14a27f6`），面域 = firmware；rebase 后至本段落笔 HEAD `b054660` 复核仍零差异）。
  - 语义提醒：本包验收红基线（acceptance-author 新冻结齿）恒现跑，无缓存可言。
- **制品**：launch-card.md（本包特记：taskpackage.json 修复留痕、input-brief 由 intake 补齐）+ current/ 指针刷新为 K5（K4 残留 input-brief/verdict 清除，canonical 存 K4 包目录）+ milestones.jsonl M3 nextAction 增补 K5 进行中。board 预填 queued + worktreePath 本树。
- **launch 指示**：human 已指示主线程准备好后直接启动 dev loop（planner），不另起交互会话。

## 2026-09-22 · K5 Pod 全程闭环 + 合并落账（planner Pod，合并门 human 批准升级合并）

- **Pod 闭环（主轮 + 追加轮 R3）**：intake 四点探测全命中（基线两面零差异引用 K4 锚 server 95 @ dc6094a / host 84 @ 9cae452，K4 留根逐条评估零 blockingDecision）→ acceptance-author 冻结 `firmware/main/test/time_sync.acceptance.c`（主轮 6 TEST_CASE / 34 tooth：[S1] 纯逻辑时间缝六函数 + [S2] SNTP 挂点 + [S3] PCF85063 写回恢复 + [S4] 周期 re-inject + [S5] 铁律守卫；红基线两通道实跑既有 84 齿零偏移 + 新齿独立红）→ builder 一次跑绿（time_sync.{c,h} 新缝 + SNTP 挂点 esp_sntp 原生 API + 板级 WriteRtcTime/boot settimeofday 恢复/60s 周期 reinject + lvgl 节流 + sdkconfig 双面 SNTP_MAX_SERVERS=3；host 90 齿零偏移全绿 + device build 11% free + mutation 自证 5 组实跑）→ verifier scoped r1 + final r2 九闸全绿（零 fixer 轮）→ integrator 零 finding 收口。
- **追加轮 R3（真机第 1 轮 A1 正锚全过 + human 两拍板 pod-events fc43063d）**：①隐藏 XiaoZhi chrome 状态栏时钟（lvgl_display UpdateStatusBar 区 SetStatus(time_str) 退役，守门行/节流分支保留裁定在案，P1 辅助条保留为唯一常显时间面）②re-inject 读点对齐分钟边界（新冻结缝 [S6] kidpal_ts_ms_to_next_minute + TimeSyncPeriodicLoop 分钟边界相位，boot 相位固定节拍退役）。冻齿修订 TEST 7/8 + A1 锚重指 P1 辅助条 → builder 一次跑绿（host 92 齿）→ verifier r3/r4 九闸全绿 → integrator 零 finding。
- **真机窗口收口（human 目测，pod-events 0e07412a）**：R3a 状态栏时钟消失 PASS、R3b P1 滞后 <1s PASS、A3b 长开机 34 分钟日志无断 PASS、1s tick 冒号闪烁佐证；**A3 断电续航 human 明示延期**（「断网现在做不了，跳过，后面观察」——账面如实记 deferred 由 human 承担，后续口头反馈补账）；advisory 挂账：reinject 双触发缺陷（pdMS_TO_TICKS 向下取整 + vTaskDelay(0) 自旋致边界前 ~10ms 双发，显示收敛 <1s 无损，修法留 pod-retro 供后续批）。
- **合并门（human 批准升级合并，pod-events f0d8ca46）**：机械 supervisor-verdict 闸 **ESCALATE**（git diff 重算 riskTier critical：firmware/sdkconfig + sdkconfig.defaults 命中 critical-path 清单；实际 diff = CONFIG_LWIP_SNTP_MAX_SERVERS 1→3，human 知悉上下文后裁决批准）→ merge-driver --pod --human-approval-ref 标准链（off-main/契约/provenance 闸照拦不豁免，全过）：rebase OK / quick-check OK / **merge OK**——merge commit `dedf986`（main @ `3cc37e5` 簿记），token 释放。**K4 收尾落账 commit（`6cb4824`，此前留在常驻分支）经 rebase 随本批一并回 main**（launch 段预告的「随下批 pod 合并回 main」兑现）。
- **收尾落账**：dev INDEX closed 行（integrator closeout 相位落）、board merged、pod-events merge-completed @dedf98697e82（main 簿记）、pod-retro 主轮/追加轮/真机窗三段（第一人称热记忆）。
- **持续挂账（非本包义务）**：① loop-retro 阈值 19 advisory（主线程 /loop-retro，连续第四棒提请）；② 图索引 re-index（time_sync [S1]/[S6] 新符号未入图）；③ A3 断电续航真机实证（human 观察，口头反馈补账）；④ reinject 双触发修法（advisory，后续批参考）。

## 2026-09-22 · K5 R4 追加轮闭环 + 合并落账（planner Pod，human 批准合并）

- **R4 起因**：R3b 分钟对齐真机复观察发现**双稳态缺陷**（有时 <1s / 有时恒慢 60s）——根因 = pdMS_TO_TICKS 向下取整 + vTaskDelay(0) 自旋 + 板级 I2C 读 RTC 耗时 → 唤醒落分钟边界前注入旧值；主线程串口捕获（23:06-23:07，边界后 55s 零注入、仅两条注入都带旧分钟 23:06）实锤。human 拍板放宽（pod-events `d2148f66`）：注入点改边界后 +`KIDPAL_TS_INJECT_AFTER_BOUNDARY_MS`(=2000)，最坏滞后 ≈3s 明示接受。
- **R4 执行（零 fixer 轮）**：acceptance-author 冻齿修订（TEST 8 teeth 7-11：常量定值 strtol==2000 钉死偏离须重冻结 + R3 裸形态退役负锚 + 首拍序守卫；teeth 0-6 零偏移；设计态红通道①b 8/1 + 通道② 92/1 红恰 = tooth 7）→ builder 单点补丁（板文件一处：常量 #define + 延迟表达式 base_ms + 2000，host 92 Tests / 0 Failures，device build 11% free）→ verifier scoped r5 + final r6 九闸全绿 → integrator 零 finding 收口（`8efe520`）。
- **R4 真机复验 PASS（human 批准合并，pod-events `e8762159`）**：串口捕获 23:48:15→23:51:41 跨 3 边界——writeback 23:48:59.4 ok；reinject 三连边界（23:49:01±1 / 23:50:01±1 / 23:51:01±1）**恒带当分钟新值、单次注入无旧值对、间隔精确 60.02s、无长静默**——R3 双稳态特征（边界前旧值 + 55s 零注入）消失，冻齿真机项达标；human 原话「合并吧」。
- **合并链**：merge-driver --pod --human-approval-ref 标准链 → rebase OK / quick-check OK / **merge OK**——R4 批回 main，token 释放。
- **R4 遗留挂账**：① mutation 执行面粒度差跨三棒（R3/R4 均以设计态红≙变异态在案，实跑变异未落仓内 evidence——pod-retro 已记「下棒必须处理」）；② A3 断电续航真机实证（human 观察反馈补账，`0e07412a` deferred 知情）；③ loop-retro 19 advisory（第四棒提请）；④ 图索引 re-index（time_sync [S1]/[S6] 符号）。
- **验收常识沉淀（pod-retro R4 段）**：睡眠/延时类板级行为 host 齿只能钉形态不能钉时序——时序语义必须设备窗实证且跨多次边界采样（单边界观察会采到幸运相位；双稳态 motif = K4 双分支盲区后第二例）。

## 2026-09-23 · K6 开机自动建连立项 + launch 面就绪（主线程，human 离席夜间自主跑）

- **立项**：human 现场报告「开机/重启后设备与 Front 连接不自动建立，需按键才建连；首页『接下来』无内容、课表页直切无数据」。主线程实证根因——`WebsocketProtocol::Start()` 空桩（小智上游惰性连接语义），`OpenAudioChannel()` 仅四个人工交互点调用（application.cc:712/:745/:784/:986），boot 链路无建连；server 侧 ws_gateway 建连即推首页「接下来」+ 提醒轮询，设备不连全悬空。课表页（症状 B）根因更深：周网格缓存唯一填充源是语音轮 display tool call，三推送触发均不推周网格——**human 拍板 B 另走设计入口（可能触契约），A-only 入包**。
- **triage**：architect 裁决 **②单包 Lite**（不触契约面——契约 §1.3⑥ 明文建连/推送时机属行为策略不进契约，architect 已核 interface-contract.md）+ **Shape A 形状修订**：弃 Connecting 态互斥（REBUT 两条：Connecting 是承重 UI 语义——voice_begin 弹语音轮层清提醒弹窗；断网 handler 在 Connecting 态 CloseAudioChannel 与后台 task 构成 use-after-free），改「`auto_connect_in_flight_` 标志互斥 + 状态恒 Idle + 主循环 handler 内判起 + OnNetworkError lambda 静默拦截 + 3 次退避重试 + 四人工 call site 加守卫」。human 离席前预授权「技术问题 call architect」，Shape A 按授权落、回席后补 ack（plan.md 留痕）。
- **建包** `260923-m3-k6-boot-auto-connect`（plan.md reviewed plan + taskpackage.json + launch-card.md），挂本 workstream（attach-existing），排常驻树第 10 棒（K5 已闭环 @b869658）。
- **上游已知红基线锚（差异判停 2026-09-23，两面分记）**：
  - server 面锚：`git diff --quiet dc6094a HEAD -- server` 零差异 → 引用 K4 launch 段锚「server 95 @ dc6094a」（量测命令 `python -m pytest -q --import-mode=importlib -m "not live" <repo>/server/acceptance`，2026-09-22 量测于 dc6094a，面域 = server）；
  - firmware 面锚：`git diff --quiet 9cae452 HEAD -- firmware` **有差异**（K5 批 21 文件）→ **现跑** WSL host runner：**92 Tests / 0 Failures** @ `b869658`（量测命令 `firmware/main/test/host_app espidf_wsl.sh build` + `host_infra_test.elf` run，2026-09-23 量测于 b869658，面域 = firmware）。
  - 语义提醒：本包验收红基线（acceptance-author 新冻结齿）恒现跑，无缓存可言。
- **launch 指示**：human 离席（2026-09-23 夜）预授权主线程准备好后直接启动 dev loop（planner Pod in-process，K5 先例），真机锚挂设备轮窗口不阻塞 host 半收口。

## 2026-09-23 · K6 host 半闭环（dev loop 主轮零返工 + 本地合并；真机半挂设备轮）

- **dev loop 全链零 fixer 轮**：acceptance-author 冻结 6 TEST_CASE 结构齿（`firmware/main/test/boot_auto_connect.acceptance.c`，1047 行，MUTATION REPLAY MAP M1..M6 在册）+ 双通道红基线实跑（通道①既有齿 92/0 与锚零偏移、通道②新齿 6/6 设计态红）；builder 按 Shape A 实现（application.h 标志成员 + application.cc 缝实现/双触发点/四 call site 守卫/OnNetworkError 拦截/Schedule 包裹 + host_app CMakeLists 注册），三通道全绿：通道② 6/0、通道① **98 Tests / 0 Failures**（92+6 零偏移）、device build 闸过（xiaozhi.bin 余量 0x61900 = 11%）。verifier r1（brief 双份维护坑）/r2（mutation-check 隐式匹配坑）两轮机械形状空转后 r3 scoped + r4 final 全闸绿。
- **K5 跨三棒 mutation 欠账本包销账**：六条变异第一方实跑（字节级备份回拷 + sha256 复核还原），原始输出落 `delivery/taskpackages/260923-m3-k6-boot-auto-connect/mutation-evidence/` 六件，逐条红 tooth 与 expectedRed 对上（M4 附带 TEST2 tooth9 同源共红为预期内注记）。
- **合并链**：机械 supervisor 闸 APPROVE（green-channel，provenance v2.0）→ 前置升级闸 PASS 无例外 → merge-driver 本地 `--no-ff` 合并 main（`6d1bec9`，Step 0 治理面收口 `ccae6b2`，pod-events merge-completed `6d1bec9344d4`），human 离席零打断、零拍板请求。
- **剩余（真机半，挂设备轮窗口 human 在场收口）**：K6-A7 四子锚真机实证（①冷启动建连推送 + P1「接下来」②wifi 闪断自动恢复避 IsTimeout ③宕机静默零错误卡 ④AutoConnect task 栈高水位）→ 真机轮后 progress.md K6 段终账。真机采样注意：退避为 vTaskDelay 时序行为，wifi 闪断子锚至少采两次断连相位（K5 跨多边界采样常识）。
- **症状 B（课表页冷缓存）**：维持 human 拍板「另走设计入口」，不入本包。

## 2026-09-23 · K6 真机第 1 轮收口（human 在场，主线程烧板 + 串口捕获）

- **烧录**：xiaozhi.bin @ 常驻树 91bcb08（与 main 6d1bec9 固件面同内容）esptool 0x20000 app-only 烧录（K6 无资产变更，单分区合规），hash 校验过；无复位循环（0 RE-ENUM）。
- **锚 ① 冷启动自动建连 PASS（日志 + 目测双证）**：无按键冷启动时序——激活完成 7.2s → `WS: Connecting wss://8.137.91.16:8000` 7.3s（无按键）→ CA 校验过 8.1s → SNTP ok 8.2s → **front 建连推送 `display_dispatch view=home upcoming_apply ok` 8.6s** → Session ID 8.6s → 首页 `render home render_rc=0` 落屏；human 目测确认 P1「接下来」有内容（原话「有内容了」）。K5 SNTP 功能共存正常（rtc_writeback/reinject 在窗内）。静默面：全程零 Alert 零错误卡零重试。
- **锚 ④ task 栈水位 PASS**：`AutoConnect task exit, stack high water mark=4912`（8192 栈余 4912，无溢出迹象）。
- **锚 ②/③ human 知情延期**（原话「那两个我没法测，后面观察吧」——K5 A3 断电续航同款 deferred 口径，账面如实记、后续观察反馈补账）：② wifi 真实闪断重连（含双断连相位采样提醒已留 pod-retro）；③ server 宕机静默零错误卡。证据链原始日志：c:/tmp/k6-coldboot.log（会话临存，关键行已录本段）。
- **K6 全包状态**：host 半闭环 + 合并（6d1bec9）+ 真机锚 ①④ PASS + ②③ 知情延期——闭环。遗留：① Shape A 立项 ack 补留痕（human 回席未提异议、已按预授权链执行，账面继续挂）；② loop-retro 阈值 20 advisory（主线程 /loop-retro 待发）；③ 症状 B 课表页冷缓存独立设计入口；④ verifier 双坑（brief 双份维护形态 + mutation-check 隐式匹配）升级为闸报错提示的 retro 建议。

## 2026-09-23 · K7 课表网格冷启动恢复（dev loop 主轮 → integrator 收口；真机锚挂设备轮）

- **上游**：Track A full-adversarial 收口（260923-arch-review-schedule-grid-cold-boot，adjudicator GREEN + F-C5 human 裁 accept-as-risk + 契约门批准 C1 挂账 + landing-verifier GREEN，commit `565cb2d`）——契约 §1.3② 增 `data.restore` additive 布尔（+ modifying L85 板子行为句 + 生产纪律句），ADR-005 定 restore 语义与部署时序（固件先行、开关默认关）。本包为实现包（sourceType arch-review），实现面 = server + firmware 双端。
- **dev loop 全链零 fixer 轮**：acceptance-author 冻齿（server 16 齿 `test_schedule_grid_restore.acceptance.py` + firmware 5 TEST_CASE `schedule_grid_restore.acceptance.c` 含 MUTATION REPLAY MAP M1..M5）+ 三通道红基线实跑（通道①既有 98/0 零偏移、通道②新齿设计态红 5/4、server pytest 16 收集 10 failed/6 passed）；builder 按 Shape A 实现（server：`schedule_push.py` 开关 + `grid_restore_payload` 变体 producer、`front_app.py` 建连缝 `_push_schedule_connect`/piggyback 接线；firmware：`mcp_server.cc` restore 消费分支、`kidpal_render_node_schedule_on_screen` 谓词、`schedule_page.c` `ksp_copy_str` 零填充[planner 扩展授权]）；verifier r1 final 九闸全绿。
- **integrator 收口（2026-09-23）**：全量 bearing final verify 复跑 9/9 绿（`verifier-evidence/rounds/r1.json`，invocationId `b3aeb5a1-49aa-491f-8a7f-944aba7b6d0d`，fact ok:true / checker exit 0）；host runner 通道① 第一方实跑 **103 Tests / 0 Failures / 0 Ignored，exit 0**（K6 记账 98 + K7 新齿 5，既有齿零偏移）；server pytest K7 面 **16 passed**；device build 闸过（xiaozhi.bin 0x30e840，11% free）。closeout 制品落 `delivery/changes/dev/260923-m3-k7-schedule-grid-restore/`（review-packet + docs-delta + verdict.json）；commit 见本段末。
- **m3-schedule 遗留③（症状 B 课表页冷缓存）销账（host 半闭环）**：K6 拍板「另走设计入口」的独立设计入口（本包 + 上游 arch-change）已落地——云端三触发附带推 `data.restore` 周网格恢复性载荷、板端只填缓存不接管前台，S-F1.2 AC1 冷启动「翻到 P2 即见网格」的 host 半机判闭环（restore 消费分支 + 在屏谓词 + 缓存幂等 + 冻结链回归守卫全绿）。**真机半（K7-A8 四子锚）挂设备轮窗口（human 在场）**——销账口径 = host 半闭环完成、真机锚挂设备轮，不宣称真机实证闭环。
- **C1 挂账保持（开闸 = human 动作）**：ECS 推送开关默认关交付（env `KIDPAL_GRID_RESTORE_PUSH` 未设 = 灭活），本包在真机复核前保持开关关；C1 evidence ③④（旧固件行为复核 + 烧板后开闸记录）挂设备轮窗口。**开关关 ⇒ 本包对线上行为零影响**（灭活性交付）。
- **残项（交 planner）**：① Findings SHOULD——K7-A2/A4 两组变异缺 mutation-evidence 原始输出（plan §4.1.2「逐 acceptance 一文件」未全覆盖），planner 补跑或明示降级；② Findings NIT——`schedule_page.c` 零填充未登记 taskpackage 变更面（轻量 scope 修正通道程序要件缺，planner 补登记）；③ Findings NIT——frozen server 验收头注 sourceType 注释与 taskpackage 不一致（纯注释）；④ 真机锚 K7-A8 四子锚 + C1 开闸挂设备轮；⑤ pod-retro.md（planner 收尾相位）；⑥ 图索引 re-index（`kidpal_render_node_schedule_on_screen` / `grid_restore_payload` / `grid_restore_enabled` 三新符号）。
- **里程碑**：`deliveryImpact=milestone-affecting` 但为中段 change、**无阶段跳变**——milestones.jsonl 状态未动、STATUS.md 未重生成；`STATUS.md` 相对 `milestones.jsonl` 的存量漂移（M3 planning/in_delivery、在途 20%/30%）为 K7 前既有、非本包引入（如需消漂移单独跑 `node .claude/scripts/delivery-status.mjs`）。
