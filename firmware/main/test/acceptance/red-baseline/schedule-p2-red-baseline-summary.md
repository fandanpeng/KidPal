# KSP2 红基线判定摘要（acceptance-author 2026-09-07 实测）

TaskPackage `260829-schedule-p2`（课表页 今天/明天渲染 + 语音问课表滚动定位 + 家长录入→可消费链路）冻结验收两文件的红基线执行记录：

- `firmware/main/test/schedule_p2.acceptance.c`（C 侧, host 单测, taskpackage `acceptanceTestsFile` 钉定）
- `server/acceptance/test_schedule_p2.acceptance.py`（python 侧, offline + live）

## 路径归位注记（acceptance-author, 机械 scope 边界所限）

1. **python 验收文件**：TaskPackage `acceptanceTestsFile` 只钉 C 侧文件，python 侧落
   `server/acceptance/test_schedule_p2.acceptance.py`（与 260829-voice-roundtrip 先例同目录同通路，launch 输入授权此落点）。planner 可选把该路径 append 进 `acceptanceTestsFile`（不改动其它冻结字段）。
2. **本摘要**：落在 scope 内 `firmware/main/test/acceptance/red-baseline/`（与 unity stub 同目录）；如 planner 想要 `delivery/taskpackages/260829-schedule-p2/red-baseline-summary.md` 形态，请原样归位。
3. **host_app CMakeLists 接线**：builder 须把 `"../../schedule_p2.acceptance.c"` 与
   schedule_page 实现源（frozen offline contract, 建议家 `firmware/main/display/schedule_page.{c,h}`）加进 `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS`（[C4.2]，包 2/3 同款一行模式）。

## C 侧红基线（命令与实跑）

冻结验收文件的闸上 runner 判据 = host_app linux-target ELF（[C4.4]），但接线属 builder。本次用边界内脚手架（本目录 `unity.h` stub + `runner.c`，非冻结件）把**同一份冻结验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /mnt/c/tmp/ksp_red_runner \
    ../../schedule_p2.acceptance.c runner.c        # BUILD_CLEAN（零告警）
cd <repo root>
wsl -d Ubuntu-24.04 -- /mnt/c/tmp/ksp_red_runner   # exit code = 失败 TEST_CASE 数
```

实测：**6 Tests 5 Failures，ELF 退出码 = 5**（经 WSL 侧 wrapper 脚本 `$? > 文件` 直取核实；`bash -c` 包装下 `echo $?` 恒 0 是 wsl.exe 参数重组伪影，WSA/VRT 摘要已记录同款）。

判读要点（冻结纪律：红必须红在真实行为缺失）：
- **行为齿经 weak-symbol frozen offline contract 真执行**：验收文件头钉死纯逻辑契约
  `kidpal_schedule_parse` / `kidpal_schedule_dot_state` / `kidpal_schedule_page_input` /
  `kidpal_schedule_locate` / `kidpal_schedule_page_render`（形状 + 数值语义冻结），
  acceptance 文件内 `__attribute__((weak))` 默认返回哨兵 -2——契约未实现时断言以
  「frozen contract NOT implemented」红，builder 落地强定义后 weak 被覆盖、真逻辑被测。
  红 = 行为缺失，不是链接/编译错误（BUILD_CLEAN 佐证）。

### 逐条判定

| TEST_CASE | 判定 | 红因（各自真实缺失行为, 2026-09-07 实测） |
|---|---|---|
| P2-A1（P2 课表页消费面装配结构, structural） | **RED** | firmware/main 无任何文件实现 frozen contract（`kidpal_schedule_parse` 零引用）——齿 1/2/4/5/6 全缺：无 schedule 页模块、未编入 CMakeLists、mcp_server.cc 无 `"schedule"` 路由 ack（schedule view 仍落 default 降级）、渲染面无调用点、无降级语义标记、无 lv_anim 弹跳面；包 2 底座守卫绿（view_router.h include / CMakeLists 装配 / kidpal/display 注册在世） |
| P2-A2 device face（schedule view 载荷解析与降级） | **RED** | `kidpal_schedule_parse` 未实现（weak 哨兵）——正规载荷逐字段解析、缺 location 容忍、畸形/缺字段/cap 越界 -1 降级全部缺失 |
| P2-A3（圆点两态） | **RED** | `kidpal_schedule_dot_state` 未实现——空心=已上完/明天、实心=待上两态语义缺失 (S-F1.2) |
| P2-A4（BOOT 长按切 今天⇄明天） | **RED** | `kidpal_schedule_page_input` 未实现——页内特例手势缺失；KEY 长按/BOOT 单击/双击不吞守卫同缺 |
| P2-A5（语音问课表滚动定位 + 路由续接） | **RED** | `kidpal_schedule_locate` 未实现（weak 哨兵）；`kidpal_display_route("schedule")==VRT_ROUTE_SCHEDULE` 续接守卫因独立 stub 未链接 view_router.c 被弱指针判空跳过（host-runner-only，接线后生效——如实记录） |
| boundary/security（扫描器自检 + remind_before_min/提醒范围守卫） | GREEN（守卫, 必须保持绿） | 扫描器四态自检 + 计数器校验通过；firmware/main 零 `remind_before_min` 引用（P2-E4 废止不消费）、零 `pending.json` 引用（提醒投递归包 7） |

### 独立 stub 编译排除项（如实记录）

P2-A3/A4 render TEST_CASE（LVGL 软渲染真帧冒烟：渲染返回行数 + 帧内容非纯背景 + 今天实心 vs 明天全空心采样网格可见差异）需要 `lvgl.h` include path——独立 stub 编译无 lvgl 路径，经 `__has_include` 守卫被编译排除，**其红态在 builder 接线进 host runner 时兑现**（frozen contract 未实现 → weak 哨兵红）。机制：先经包 0 冻结渲染缝 `host_infra_render_frame()` 置 rendered 态，被测渲染契约函数在活动屏真画，`lv_refr_now` 后经缝 flush 进可探针缓冲、`host_infra_read_pixel` 采样网格断言真实像素——不经任何 seam 修改。

## python 侧红基线（命令与实跑）

```
cd server && python -m pytest acceptance/test_schedule_p2.acceptance.py -q --import-mode=importlib
```

实测：**10 failed, 2 passed, 1 skipped（0.89s），收集零错误**（§3 冻结即冒烟：pytest 收集 error = 0）。

| 测试 | 判定 | 红因 |
|---|---|---|
| test_a2_parse_b1_three_blocks | **RED** | server/gateway/schedule.py 缺失（frozen offline contract 未实现）——B1 三类区块解析缺失 |
| test_a2_parse_rejects_malformed_rows | **RED** | 同上——脏行 ValueError 拒绝缺失 |
| test_a2_filter_today_tomorrow_weekday_and_expired_ignored | **RED** | 同上——按星期筛选 + 过期一次性安排忽略缺失 |
| test_a2_time_filter_items_after | **RED** | 同上——当前时间筛选缺失 |
| test_payload_shape_conforms_contract | **RED** | 同上——契约 §1.3 ② producer 载荷形状 + 真实 validate_display 门缺失 |
| test_decision26_grouping_school_merged_and_individual | **RED** | 同上——决策 26 producer「上学」合并/逐项分组缺失 |
| test_decision26_grouping_upcoming_rules | **RED** | 同上——upcoming 规则（进行中保留/已结束剔除）缺失 |
| test_a6_entry_write_consume_chain | **RED** | 同上——录入→写 MD→消费链路缺失 |
| test_a6_prompt_carrier_v2_landed_and_wired | **RED** | server/deploy/openclaw-system-prompt.v2.md 未落盘 + overlay 接线未引用（版本化惯例，内容不锁） |
| test_e4_remind_before_min_not_consumed | **RED** | 同上——废止字段不变式断言缺承载模块 |
| test_boundary_no_view_invention | GREEN（守卫, 必须保持绿） | view 白名单 7 view + MAX_DATA_BYTES=65536 冻结面在世（display_schema 未被动） |
| test_e1_existing_directionb_acceptance_green | GREEN（守卫, 必须保持绿） | gateway-directionB 套件独立复跑 exit 0（20 Tests OK） |
| test_a6_live_qq_entry_evidence | SKIP（机械闸） | `@pytest.mark.live` + conftest 默认 skip（ARQ-260901-g3），`RUN_LIVE=1` 或 `-m live` 显式执行 |

### 守卫目标选择的如实记录

E1 回归守卫首版选 voice_roundtrip_b 套件，实测发现其 **VRT-A4 证据齿在本包冻结时点已存在先行红**（adherence record 缺失败处置条目——与本包新文件无关的既有状态，本包未触碰其任何输入）。为避免守卫恒红，E1 守卫目标改为 gateway-directionB 套件（GRF-E1 同款、实测 20 Tests OK）；voice_roundtrip_b 的先行红如实记录于此，不属本包红基线、不阻塞冻结。

## 冻结即冒烟（§3）与 C 类实编（§4）

- 编译通道：C 侧冻结验收文件 `-Wall -Wextra` BUILD_CLEAN（零告警零错误）；python 侧 pytest 收集零 error。
- 执行通道：两侧各自当场执行一次（stub ELF + pytest），error = 0（全部 failure 为断言 failure = 设计态红基线，非 error）。
- live 面：`@pytest.mark.live` 不 auto-run；真录一轮（QQ 录入→改写→问课消费, probe ≠ exercise）由 operator 在本包相位内执行并把证据落 `delivery/changes/dev/260829-schedule-p2/`。
