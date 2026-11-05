# Review Packet — 261104-k1-points-ledger-core

> integrator 收口（2026-11-05，本树 dev/pod-m2，包级 base ref 85cc3747，HEAD cac13aee）。
> 收口形态 = 离线面收口 + 诚实挂起：K1L-L1/L2 真机轮待跑、points.py 部署铺放与
> KIDPAL_POINTS_STATE 迁移归部署轮；本包不宣告 ready-to-merge，终裁归 supervisor 门。

## Change Size

- Files changed（server 面，85cc3747..HEAD）：8 tracked modified/new——points.py
  +621/−103（单 writer 正式化）、front_app.py +138/−（观察缝 GAP-1）、冻结验收新件
  1551 行、test_points_ledger_units.py 新件 497 行、test_points_spike_units.py
  +44/−（回归重锚）、deploy 三件 +33（manifest 补 points.py 条目 + 布局注记）。
  display_schema.py 0 行 diff（0 改动申报，见 Findings F4）。
- Net diff：全 diff（含 delivery 投影与包级簿记）3307 insertions / 557 deletions；
  实际 diff ⊆ 扩单后 9 项 changeManifest（change-manifest 闸核过）。
- Scope：server points 账本核心正式化（跨进程锁 / 原子写 / 两类幂等 / 删除重算 /
  目标配置 / keyset 分页）+ front 观察缝 + K0 units 回归重锚 + 部署面补全。零固件
  diff、零契约/requirements/architecture 改动、零 story 数据域触碰。
- Split recommendation: 不拆——K1 单包承载账本核心，mergeOrder 72（依赖 K0a/K0 已合）。

## Verification Evidence

- verifier 相位（planner 内联跑 verify.mjs）：**8/8 GREEN**（taskpackage-required /
  change-manifest / firmware-build / contract-lint / acceptance-coverage /
  mutation-check / docs-sync / security），闸修三处簿记（sourceRefs 补 AC2/AC5、
  live-manual coverage 标记承载、perAcceptance 对齐），verify-runs.jsonl 落盘。
- **final 相位（integrator，scope 圈定后实跑）**：`verify.mjs --phase=final
  --scope-file=delivery/taskpackages/261104-k1-points-ledger-core/test-scope-final.txt`
  → **ok:true，10 闸全跑零 skip**：taskpackage-required ✓ / change-manifest ✓ /
  firmware-build ✓（ESP-IDF device build 真编译绿，xiaozhi.bin 8% 分区余量）/
  contract-lint ✓（1 条存量 warning：arch 历史批相对链接断链，非本包引入）/
  acceptance-coverage ✓（K1L-A1..A10 十条全覆盖） / mutation-check ✓（perAcceptance
  10/10 对齐）/ docs-sync ✓（docsSyncRequired=false 豁免）/ security ✓ /
  unit ✓（17 passed）/ acceptance ✓（17 passed）。verifier-round fact 落
  verify-runs.jsonl 末轮（ts 2026-11-05T10:50:00Z，10/10 ok=true）。
- **四组测试数字**：① 冻结验收 K1L-A1..A10 **10/10 绿**（sha256 复核一致
  bd9dec61d017d65c69398b84ddc3c0450cb6bf7631f78dc30883e23d61b6f414）；② units——
  本包 ledger 48 + K0 spike 51（含 3 处回归重锚）+ story 40 = 139 passed；③ 种子
  回归（m4k1_story_cloud_link + record_dialogue_guide）53 passed / 3 subtests；
  ④ scoped final 合跑 **202 passed / 0 failed / 3 subtests**（范围清单 =
  test-scope-final.txt，落盘路径同上）；builder-complete 时全 acceptance 面
  411 passed / 0 failed（基线 401 + 本件 10）为旁证，本终验不重跑全量（人工触发位）。
- Mutation: perAcceptance **10/10 turnedRed**（builder 实放 2026-11-05，字节级
  sha256 还原校验全 True；A4/A6/A9 在重冻修正后补取——原三红归因冻结件缺陷非变异
  不敏感）。敏感变异 5 条（A1/A2/A6/A8/A10），收尾零残留。
- Security: securityReviewRequired=false，闸绿。结论一句：**无扩大写权**——state
  路径仍单源 `KIDPAL_POINTS_STATE` env；跨进程锁用 fcntl/msvcrt 纯 stdlib（零新增
  依赖、零第三方位）；执行器 API 签名禁 balance/revision/delta/entry_id/cursor/
  target_id 写入入口（K1L-A5 断言钉死）；front 侧不传 command_id（planner ruling5
  认可，消费方 = K3 CLI）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | server/acceptance | 重冻四处：冻结验收件四处断言缺陷（A4 兑换后 revision 计数 2→3、_assert_chain_contiguous 第二循环前驱序、A9:1413 污染后期望字节、A9:1428 余额 7→2），builder 阶段实证为**冻结件缺陷、非实现缺陷**，acceptance-author 重冻（作者≠builder 边界保持，断言实质与 binding refs 不变） | changed hunk test_points_ledger_core.acceptance.py + planner ruling1（pod-events 3366e797，planner 逐处对源码独立核验属实） | 已闭合——重冻后 10/10 绿、planner 复跑一致；无遗留 |
| NIT | judgement-call | server/tests | manifest 扩单（第 9 项）：K0 units 两断言与 K1 冻结契约直接冲突（:387 同目录闭集 ['state.json'] vs K1L-A1 常驻 .lock；:394 CLI 二次兑非零 vs K1L-A6 去重先于余额门返 0），最小回归重锚 + :396 巧合绿用例显式化 | changed hunk test_points_spike_units.py + planner ruling2（无扩单无法收口的冲突实录） | supervisor 复核扩单裁定（merge packet 已列；K0a pod-retro 教训 2 的 server 侧同型消费） |
| NIT | judgement-call | server/deploy | points.py 从未进部署清单（K0 遗留缺口，live 无积分执行器）；本包 manifest 补 points.py 条目（restart: front）+ /opt/kidpal/points/ 目录布局与迁移注记，deploy.sh §2b 幂等建目录 | changed hunk server/deploy/manifest.yaml + planner ruling4（部署面补全非语义改线） | 上线部署轮铺放 + KIDPAL_POINTS_STATE 迁移（见未覆盖面申报） |
| NIT | judgement-call | server/gateway | display_schema.py 0 改动申报：K1L-A3 冻结红线（整数可负不钳位）由既有 _points_floor 满足，无冻结依据的行为变更不做 | git diff 85cc3747..HEAD display_schema.py 空输出 + planner ruling3 | no-trigger |

结构闸 PASS/FAIL 不复述为 finding（约定级约束）；gate 全绿仍执行了五轴语义 review
（下节）。无 BLOCKING、无 SHOULD。

## Five-Axis Review

- **类型/边界**：分录与目标双 shape 校验（_entry_shape_error / _target_shape_error）
  坏形状 ValueError 拒绝不当空账；_is_positive_int 显式排 bool（True/2.5/字符串拒，
  K1L-A8 断言钉死）；read_state 对损坏 state raise 绝不静默当空账覆盖（数据丢失面
  守住，K1L-A9/A10 损坏态不抛不推不改文件）；负和不钳位（−35/−40 如实派生，钳位
  禁令过 display floor）。
- **错误处理**：拒绝路径三不原则一致（不落账/不推帧/不递增 revision——空账/金额
  不匹配/余额不足/非法 id 全零副作用）；原子写 tmp+os.replace 失败时旧状态完整无
  半写（K1L-A1 断言）；执行器硬失败注入 → 磁盘零写入、恢复后仅真实一笔无离线写
  队列幽灵（K1L-A10，D-06 离线边界钉死）。
- **契约/单源**：余额与逐条前后余额全部由剩余分录派生、不信任磁盘副本（A2 变异
  实证敏感）；分录六键 / page 四键 / target 四键逐字对契约 §四；keyset
  (occurred_at,id) 时间倒序 + revision 变化续页重置、API 签名禁 offset（K1L-A7）；
  两类幂等分离——command_id 传输重放返首次结果（含模拟重启）与 600s 注入时钟短窗
  语义去重互不替代、传输幂等先于语义去重（K1L-A5/A6）；唯一 writer——front 与 QQ
  同一执行器、锁文件常驻不删（unlink-while-held 竞态规避）；历史兑换名目快照切目标
  不改写、预置四 id 拒删 + 激活中自定义禁删（OQ-2/OQ-3）。
- **并发/资源**：两层串行化——进程内锁 + state 同目录 `.lock` 跨进程文件锁
  （POSIX flock / Windows msvcrt.locking 双平台 stdlib），锁持面最小（read+validate+
  recompute+replace）；8 线程 + 10 子进程并发零丢账、前后余额链逐条核（K1L-A1）；
  front 观察缝 `_observe_points_revision` 挂 reminder_poll_loop 每分钟节拍（最小
  实现形态，活性接线真机目击归 K1L-L2）。
- **安全**：见 Verification Evidence security 行——无扩大写权、零新增依赖、模型侧
  无任何权威值写入入口（A5 签名断言）；K0 轮 AGENTS.md workspace 纪律锚
  （不产余额/revision/不宣称已记账）沿用。

## Simplification Debt

`minimal:` 标记收成：本包 diff **零新增 minimal: 标记**（仓内唯一在世标记
front_app.py:2201 story sweep 固定间隔，属 261022-p3-record-front-polish-fix 遗产，
不在本包 diff 内；升级触发已在原包记账）。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| front_app.py `_observe_points_revision`（reminder_poll_loop 每分钟节拍） | 观察缝活性接线 = 轮询节拍缝级实现，离线验收只验缝行为（A9） | 真机活性目击（QQ 外部写入 → 设备屏权威帧） | K1L-L2 真机轮（human 在场） |
| 无 upgrade trigger 的条目：无——上条有明确载体。 | | | |

## Residual Risk

- Known limitation: ① **K1L-L1/L2 真机轮待跑**（live-manual 两条，human 在场轮）——
  L1 语音链真机记账、L2 观察缝活性接线真机目击；载体 live-evidence 未创建，收口
  不伪造。② **观察缝活性接线为离线缝级验证**：K1L-A9 验证 `_observe_points_revision`
  缝行为（基线零帧/恰一帧/删除双推/损坏静默），轮询节拍与真机屏面联动未目击。
  ③ **部署面待铺放**：points.py 部署条目已补登记但 live 未铺，/opt/kidpal/points/
  布局与 KIDPAL_POINTS_STATE 迁移归部署轮（现 live 无积分执行器）。④ **全量回归 =
  人工触发位**：本终验跑圈定清单 202 条，全量 acceptance 411 条只在 builder 轮
  旁证过，final 未重跑（按 dev-loop 收口惯例）。
- Follow-up recommended: K2 设备完整积分本消费（权威帧渲染 + 分页触发接线 + 满窗
  翻页）、K3 QQ 正式通道（command_id 消费方 CLI）、K4 目标配置 QQ 管理与刻度联动。
- Human decision needed: 无新增——本包无 `decisionCoverage` 中 `landsIn: dropped`
  且 `reason` 以 `risk-accepted` 开头的条目（八条 decision 全部 landsIn 到
  exitCriteria 或 K2/K3 承接包），无 risk-accepted 台账项可列。

## 未覆盖面申报（诚实挂起清单）

1. K1L-L1 真机语音记账目击——live-manual，human 在场轮。
2. K1L-L2 观察缝活性接线真机目击——live-manual；本包仅离线缝级验证（K1L-A9）。
3. points.py 部署铺放 + /opt/kidpal/points/ 布局落地 + KIDPAL_POINTS_STATE 迁移
   ——归部署轮；manifest/deploy.sh/verify-deploy.sh 已备，live 未铺。
4. 全量 acceptance 回归（411 条）——人工触发位，final 终验未重跑。
5. QQ 正式交互（K3）、设备完整版式与翻页（K2）、目标配置管理交互（K4）——按
   crossPackageBehaviors 归承接包，非本包范围。

## 设备轮证据

deviceOutcome 未触发——none（本包零固件 diff，真机轮归 K1L-L1/L2 待跑项）。
