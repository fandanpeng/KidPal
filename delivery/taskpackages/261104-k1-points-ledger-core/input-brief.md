# input-brief · 261104-k1-points-ledger-core

> planner 备料（2026-11-05）；对端实读完成度 = ② 节逐条核对过 HEAD 85cc3747。消费方 = acceptance-author → builder。

## ①a 包面与写权

- 包件：`taskpackage.json`（exitCriteria K1L-E1..E6 + decisionCoverage + changeManifest）+ `plan.md`（背景/可行性/铁律/证据形态）+ `launch-card.md`（同目录）。rigor full / riskTier critical / gateProfile=[evidence-integrity, test-quality, real-db, state-machine, mcp-tool-contract]。docsSyncRequired=false（语义已冻结，本包只落实现）。
- **写权 = changeManifest 8 文件**：`server/gateway/points.py`、`server/gateway/front/front_app.py`、`server/gateway/display_schema.py`、`server/tests/test_points_ledger_units.py`（新增）、`server/acceptance/test_points_ledger_core.acceptance.py`（新增）、`server/deploy/manifest.yaml`、`server/deploy/deploy.sh`、`server/deploy/verify-deploy.sh`。验收件归 acceptance-author；实现归 builder；均不改 `docs/contracts/**`、requirements、architecture、story 数据域。
- base ref = `85cc3747`；包级 diff 恒用此 ref。

## ①b 行为权威（requirements v1.71 frozen @ docs/requirements.md）

- **S-F6.2**（:696–708）AC1（账本全量可及）+ AC4（删除重算允许负余额 = OQ-1；删除后兑换仍要求余额 ≥ 激活目标分值）；补充规则：删除 = 云端移除该分录并重算，其余分录保留不清零；「全量可及」为需求语义，分页/窗口形状权威归架构与契约（**不照搬故事 16 条窗口措辞**）。
- **S-F6.3**（:712–725）AC3（兑换只认激活目标分值、不符温和拒答）+ AC4（真实兑现 + 一次明确确认后才产生负向分录，绝不提前扣）+ AC5（同一次兑换/重复表达不重复扣——短窗起步 10 分钟）+ AC6（断网失败可见、不本地暂存补记）；补充规则：记账句式门槛 = K0/K2 面（模型侧），本包钉确定性去重；惩罚性扣分 / 语音删除 / 加分护栏第一版不做（human 批示）。
- **S-F6.5**（AC1/AC2/AC4 **存储半**）：四预置（巧克力 20 / 看电影 30 / 游乐场 40 默认 / 礼物 50）+ 自定义 5–200 正整数 + 预置不可删 + 激活中自定义禁删（OQ-2/OQ-3）；QQ 配置交互与设备刻度归 K4。
- **S-F6.1 AC5**（负余额显示）：本包钉**数据面**（余额可负、如实派生），显示归 K2。
- **NFR-OFF-1**：断网不本地补写、无离线写队列；**NFR-PERF-4**：软时延，只记录不写死阈值。

## ①c 跨包事实与留根（planner intake 已核）

- **GAP-1 承接（K0 §六.1）**：QQ 落账后设备缓存同步推送缺位。机制架构已定：**front 观察 points.py 状态 `revision` 变化并推送同一权威视图**（arch §5.1.2 front 行 + §5.2 场景⑥ B）——QQ 写入后推送 points_home（删除时另推 points_ledger 首页）**restore 载荷**（设备其他页不抢前台、积分页在屏原地刷新——设备行为归 K2，本包钉 producer 侧帧形状与触发）。front `_consume_points_intent` 的 restore kwarg 已在但**无活调用方传 True**（K0 QQ 链走 main agent → CLI 直写 state，不经 front）。
- **K0 rulings**（pod-events.jsonl planner-ruling）：①契约 limit 为**上限非定长**——front `_POINTS_PAGE_STEP=1` 步进合法（满窗翻页归 K2）；②**points_ledger 载荷无 target 字段**，仅 points_home 强制 target 四键。
- **K3/K4 交接**：QQ CLI = `python -m gateway.points`（`KIDPAL_POINTS_STATE` env 单源、`main(argv)` 入口）——K3 只整理一次性命令调 CLI，回执形状须可读交接（K1L-E6）；目标配置存储与校验在本包建立（K1L-E4），交互归 K4。
- **相邻冻结锚（零修改、只回归）**：`story.py` seq 账本独立域（含 `_save_ledger`）；`server/tests/test_story_units.py`；`server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（story_list/story_text 语义不变、积分私有帧不落故事分支）；`server/acceptance/test_record_dialogue_guide.acceptance.py`（gateway 决定类与 forwardable 分叉保持，积分动作只由 front 截获）；`server/acceptance/test_points_e2e_spike.acceptance.py`（K0 冻结件 14 断言，须保持全绿）。
- **基线锚**：server 面 401 passed / 0 failed / 33 subtests / 9 deselected @ 85cc3747（workstream progress.md 2026-11-05 段，带锚三件齐）。本包验收红基线恒现跑。

## ② 对端解析器实读（HEAD 85cc3747 现状）

- **`server/gateway/points.py`**（K0 最小路径，482 行）：常量 `DEFAULT_TARGET`(:47)/`HOME_RECENT_MAX=3`/`PAGE_LIMIT_DEFAULT=6`/`MIN=1`/`MAX=50`；`read_state`(:154)/`_write_state`(:192)/`_append_entry`(:222)/`add_points`(:249，`now` 注入参数在)/`redeem_points`(:262)/`build_points_home`(:289)/keyset cursor 编解码(:303/:311)/`build_points_ledger_page`(:338)/CLI `main`(:438)。**缺**：删除路径、命令回执（command_id 幂等）、短窗语义去重、目标配置存储面（自定义/切激活/禁删校验）、revision 观察点、进程间锁显式化（现为进程内 `threading.Lock`）。
- **`server/gateway/front/front_app.py`**：`POINTS_STATE_PATH`(:80，env 单源)；`_POINTS_SINGLE_SOURCE_VIEWS`(:84)；`_POINTS_CONTROL_ACTIONS`(:86)；`_expand_points_nav_intent`(:645)；`_consume_points_intent`(:671，restore kwarg 在、缺省 False，无活调用方传 True——GAP-1 缝)；`_POINTS_PAGE_STEP=1`（:87 一带）。
- **`server/gateway/display_schema.py`**：points_home / points_ledger 帧校验（K0 版；K1 scope = shape floor 收紧）。
- **契约**（docs/contracts/interface-contract.md，禁写只读）：§1.3⑦ points_home / ⑧ points_ledger；§1.4 add_points/redeem_points 薄意图；§1.5 来源门禁；§四 kidpal_points_page / kidpal_points_delete 上行。
- **架构**（docs/architecture.md）：§5.1.2 front 行（QQ 侧 revision 观察推送）/ points.py 行（进程间锁、同临界区读→校验→重算→原子替换→存回执）/ QQ skill 行（workspace-main/kidpal-points-write）；§5.2 场景⑥ A–E；§5.3.2 积分状态位置（`/opt/kidpal/points/` state.json + .lock，预置四场景为实现内不可删常量、出厂激活游乐场 40）；§5.4 逻辑事实清单（分录 id/delta/事由/occurred_at/前后余额/通道/command_id + 激活目标/自定义目标/revision/回执；删除后按时间序重算全部前后余额）；**§6.5 幂等与一致性 1/2**（传输幂等 = 同 command_id 同临界区查回执重放返首次结果、新回合必须新 command_id；短窗去重 = 可注入时钟、同正负分值 + 保守归一化事由〔去空白/标点、全半角统一、不改时间前缀〕、窗口外同文案合法新命令）；ADR-018。
- **PROPOSAL**（workstream 目录，FROZEN v1.0）：D-03（分录为事实/删除重算）/ D-04（短窗去重与完整记账规则）/ D-06（云端唯一真相源）/ D-08（删除与 keyset producer）/ D-10（唯一确定性 writer）；OQ-1（删除可负）/ OQ-2（禁删激活目标）/ OQ-3（5–200）。

## ③ 验证环境

- server 基线命令：`python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m2/server/acceptance"`（仓根执行）；units 在 `server/tests`。
- 时间注入先例：`add_points(..., now=None)` 参数形态可续用；验收断言禁 wall-clock 依赖（短窗去重必须注入时钟）。
- mutationRequired=true：账本不变量逐条可转红（perAcceptance 变异草案随交付报告出）。
- K0 红基线通道先例：验收文件设计态红（实现面缺席、收集零错误、无环境型红）。
