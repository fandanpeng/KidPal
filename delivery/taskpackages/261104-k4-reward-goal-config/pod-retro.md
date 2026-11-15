# pod-retro · 261104-k4-reward-goal-config

> Pod 收尾复盘（planner 第一人称热记忆，closeout advisory 指名）。形态 = VS Code 扩展会话内联
> planner；base ref `cad07f2c`（叠在未合并 K3 之上，human override LP12）；pod-events 全程。
> 只记根因、摩擦、纠正与未解项，不复述 tokens/gates/rounds。

## 顺利面（保持）

- **Step 1g 预评纠正了两处会变贵的错误**：① 场景标换图必须动 host 不可编的
  `kidpal_render_node.cc`（场景槽按指针比较），据此把 deviceOutcome 申报改 `userVisible:true`、
  manifest 加该路径，避免了合并门才炸；② 资产落盘名不是契约逻辑标识（`sc-choco` →
  `sc_choco_42_42.bin`），原 manifest 五个文件名全错，且 `sc_wheel` 是 K2 已交付件。
- **扩单通道按规矩走通**：`read_state` 增量两键打破 K0 精确三键断言 → 不先改后补，
  architect 评估 APPROVE 后 append manifest，并按给定最小 diff（投影 equality + 出厂两键 +
  `receipts` 负断言）改，形状纪律不退让。
- **纯追加守资产零回归**：资产管线整文件重生成会漂既有件，builder 以「管线产出后把 4 个新块
  纯追加、其余还原」守住 `spiffs_root` 4A/0M，并如实申报偏离。
- **planner 独立复跑**：冻结件红基线、K0 单测红、资产 4A/0M、scoped 8 闸都自己跑过，未凭报告采信。

## 踩坑与教训（供 loop-retro）

1. **冻结验收自身会自相矛盾**：K4G-A1 对空目录先快照再断言「非空」（不可满足）；K4G-A7 与
   K4G-A2 对「删激活中自定义」给出相反期望。是 builder 发现、planner 核实后回派修正。
   **共性**：acceptance-author 冻结前必须做一次**跨用例一致性自检**（同一构造不得出现互斥期望；
   快照/前置必须与新建夹具状态匹配），K2 的同一教训没有沉淀成必做步骤。
2. **`acceptance-coverage` 不认 Unity 断言**：gate 的 `hasRealAssertion` 只认 JS/Python 形态，
   `TEST_ASSERT_*_MESSAGE` 一律不认；纯 Unity 的 C 验收件会假红，K2 能过只是因为恰有一处
   libc `assert()`。本包按 K2 先例补真实 OOM 守卫绕过。**共性**：闸的断言识别集缺 ESP-IDF
   unity host 分支，属治理机检缺口（loop-retro/ARQ 候选）。
3. **manifest 起草没对照落盘命名律**：拆包期把契约逻辑标识直接写成文件名，五个里五个错。
   **共性**：凡 manifest 含生成件路径，起草时必须对照生成器的命名律（`<name>.bin` 根名 =
   assets.h 符号名），否则 builder 一动手就清单外。
4. **预算普遍低估**：builder 124/45、acceptance-author 26/20、architect 18/14、integrator 34/30。
   资产管线、host 全量重编、逐 acceptance 变异重放都是重活。**共性**：含资产/二进制/宿主编译的
   包，预算要按「次数量级」给，不要按纯 server 包估。

## 未解项 / 交接（交主线程与 human）

- **K4G-L1 真机轮未跑**：真实 QQ 目标配置全过程、四预置 + `sc-flag` + 200 大分值/越目标看屏、
  切目标后旧场景标消失、其他页不抢前台/原地刷新、NFR-PERF-4 同款时延、RAM 预算、V9 真机
  design review——全部待 human 设备窗，与 K3 的 live 轮合并。
- **ECS 部署轮未跑**（K4 的 points.py/SKILL/四枚资产尚未上线）；归主线程。
- **合并序 K3 → K4**：两包都未合并，K4 叠在未合并 K3 上；K3 的 supervisor 结构性 ESCALATE
  也待 human 批。
- **verdict.json advisory**：本轮 bearing 事实是 `rounds/r1.json`，`current/verdict.json` 未产；
  若后续要与 K3 同形，明确由程序 fs-write 产出，不由 agent 写机器件。
- **资产管线漂移**：`ic_wifi_14` 母版 map / `p1_chip_mingzao.bin` 与提交件不一致（K2 pod-retro
  已记同族 tech-debt），任何人全量重跑管线都会踩；本包不重复立账。
- **loop-retro 阈值已触**（26 ≥ 3）：发起责任 = 主线程。
- 遗留：integrator 建议把「scope-file 格式 + final 证据写入路径」沉进角色卡，可省 3–4 次读取。