# VRT 红基线判定摘要（acceptance-author 2026-09-07 实测）

TaskPackage `260829-voice-roundtrip`（方向 B 语音回环 + display 消费链路）冻结验收
两文件的红基线执行记录：

- `firmware/main/test/voice_roundtrip_b.acceptance.c`（C 侧, host 单测）
- `server/acceptance/test_voice_roundtrip_b.acceptance.py`（python 侧, offline + live）

## 路径归位注记（acceptance-author, 机械 scope 边界所限）

enforce-scope 对 acceptance-author 只放行 `firmware/**/*.acceptance.c(pp)`、
`firmware/**/acceptance/**`、`server/**/acceptance/**`。三处偏差按 WSA 包先例
（red-baseline-summary.md 的归位注记形态）先落 scope 内、由有写权的收口方归位：

1. **python 验收文件**：TaskPackage `acceptanceTestsFile` 钉 `server/tests/test_voice_roundtrip_b.acceptance.py`，scope 拦截，实落 `server/acceptance/test_voice_roundtrip_b.acceptance.py`（与既有冻结验收同目录同通路，断言零变更）。归位二选一：(a) 更新 taskpackage 路径为本路径； (b) 移至 server/tests/ 并在 server/pytest.ini 加 `addopts = --import-mode=importlib`。文件头注已写明。
2. **本摘要**：目标位置 `.claude/taskpackages/260829-voice-roundtrip/red-baseline-summary.md` 被 scope 拦，实落本目录，请原样归位。
3. **host_app CMakeLists 接线**：`../../voice_roundtrip_b.acceptance.c` 加入 `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS`（builder wiring 职责, [C4.2]；WSA 同款）。builder 还须把 view_router 实现源一并编入（见下 frozen contract）。

## C 侧红基线（命令与实跑）

冻结验收文件的闸上 runner 判据 = host_app linux-target ELF（[C4.4]），但接线属
builder。本次用边界内脚手架（本目录 `unity.h` stub + `runner.c`，非冻结件）把
**同一份冻结验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /mnt/c/tmp/vrt_red_runner \
    ../../voice_roundtrip_b.acceptance.c runner.c        # BUILD_CLEAN（零告警）
cd <repo root>
wsl -d Ubuntu-24.04 -- /mnt/c/tmp/vrt_red_runner         # exit code = 失败 TEST_CASE 数
```

实测：**8 Tests 5 Failures，ELF 退出码 = 5**（退出码经直取 argv 形态单独核实；
`bash -c` 包装下 echo $? 恒 0 是 wsl.exe 参数重组伪影，WSA 摘要已记录同款）。

判读要点（冻结纪律：红必须红在真实行为缺失）：
- **行为齿经 weak-symbol frozen offline contract 真执行**：验收文件头钉死纯逻辑
  契约 `kidpal_display_route` / `kidpal_display_control_action` /
  `kidpal_display_text_page_count`（数值码 + 语义冻结），acceptance 文件内
  `__attribute__((weak))` 默认返回哨兵 -2——契约未实现时断言以「frozen contract
  NOT implemented」红，builder 落地强定义后 weak 被覆盖、真逻辑被测。红 = 行为
  缺失，不是链接/编译错误（BUILD_CLEAN 佐证）。

### 逐条判定

| TEST_CASE | 判定 | 红因（各自真实缺失行为, 2026-09-07 实测） |
|---|---|---|
| VRT-A1（display 消费面装配结构, +E1） | **RED** | firmware/main 无任何文件引用 `kidpal_display_route(`——mcp kidpal/display 消费面不存在（齿 1/2/3/4 全缺：无路由模块、未编入 CMakeLists、无会话面调用点、无降级语义）；application.cc 原生 mcp 分支守卫绿（到达面在世） |
| VRT-A3（view 路由行为） | **RED** | frozen contract 未实现（weak 哨兵）——text/home/control 三路由、7 view 注册表（VRT-E3 共享底座）、白名单外降级、control 动作解析全部缺失 |
| VRT-A3（text 全文上屏分页） | **RED** | `kidpal_display_text_page_count` 未实现——全文不丢分页（S-F4.1 否摘要）、翻屏承载缺失 |
| VRT-A2（stt/tts 状态机原生缝） | GREEN（守卫, 必须保持绿） | 契约 §四 设备半是 WS 包复活的原生管线：application.cc 的 tts start/sentence_start(带文字→SetChatMessage assistant)/stop + stt(→SetChatMessage user) 全在世；端到端 live 由证据齿承载 |
| VRT-A5（网关兜底设备侧消费） | **RED** | 兜底 text 视图消费面缺失（weak 哨兵）+ 格式不遵循独立 UNKNOWN 降级路由缺失；tts stop→Idle 当轮结束守卫绿。生成逻辑归 G 已 landed（python 侧守卫），本包缺口 = 设备侧消费 |
| VRT-A6（abort 发送路径） | GREEN（守卫, 必须保持绿） | 原生在世：protocol.cc SendAbortSpeaking 发 `{"session_id","type":"abort"}`；application.cc `AbortSpeaking(` 出现 5 处（定义 + 4 打断调用点 ≥ 3 阈值）。**对 taskpackage redBaselineResult「无 abort 发送路径」的勘误：发送路径 xiaozhi 原生在世，本包 A6 的真缺口在云端取消生成证据（python 侧）与 9a 真机停播实测** |
| VRT-A2/E2（live 冒烟证据） | **RED** | changes/dev/260829-voice-roundtrip/ 不存在——流式回环 live 冒烟（五事实：session_id / stt / tts / sentence_start / state-speaking 文字同步）未执行（evidence-based 齿按性质红，WSA-A4 先例） |
| boundary/security（扫描器自检 + 范围守卫） | GREEN（必须保持绿） | 扫描器四态自检 + 计数器校验通过；「网络出小差了」兜底文案未硬入固件（生成逻辑归 G, F2）；设备无 openclaw/18789 直连引用 |

## python 侧红基线（命令与实跑）

```
cd server && python -m pytest acceptance/test_voice_roundtrip_b.acceptance.py -q --import-mode=importlib
```

收集口径（钉死）：文件名两点的 ModuleNotFoundError 须 importlib 模式（G 包先例）；
launch 授权的 server/pytest.ini addopts 改动因 scope 拦截未落，暂以显式 flag 为冻结
命令口径；若收口方选归位方案 (b)（文件移 server/tests/），同时把
`addopts = --import-mode=importlib` 落进 server/pytest.ini 即可裸命令收集。
**live 不 auto-run 已机械化**：新增 `server/acceptance/conftest.py`（launch 授权的
conftest 最小调整），默认收集给 @pytest.mark.live 项打 skip（ARQ-260901-g3 纪律从
运行命令自觉变机械闸），显式 `RUN_LIVE=1` 或 `-m live` 放行；该 conftest 同样罩住
G 包既有 live 验收的默认运行（与其 docstring「NOT auto-run by any merge gate」一致，
断言零变更）。

实测（2026-09-07, Windows python 3.14.0 + pytest 9.1.1）：
**默认跑 = 5 failed, 3 passed, 3 skipped**（offline 8 齿收集零错误；live 3 齿被
conftest skip）。显式 live 试跑（`RUN_LIVE=1 -m live`）= **1 passed, 2 failed**——
SSH 前置真连通过（ECS 在线），a4/a7 live 齿红在冻结 system prompt 缺失（探针首断言
即红, 未发任何业务请求）。

### 逐条判定（offline 类）

| test | 判定 | 红因 |
|---|---|---|
| test_a2_protocol_profile_twin_aligned | GREEN（守卫） | C 孪生文件在世且承载 §四 剖面标记（同会话冻结, 对齐成立） |
| test_a3_cloud_display_envelope | GREEN（守卫） | G 已 landed 的 mcp_relay/display_schema offline contract 对齐（tools/call 信封 + 白名单 == 契约 §1.3/§1.4） |
| test_a5_cloud_fallback_alignment | GREEN（守卫） | fallback.py 生成面零漂移：ERROR_FALLBACK 措辞、4000ms、text view 载荷形状 == 设备消费面、V32 截断 |
| test_a4_adherence_recheck_evidence_record | **RED** | changes/dev/260829-voice-roundtrip/ 无遵循度复验记录——方向 B fence 路径统计性复验未执行（契约待确认点 1 不复用 12/12） |
| test_a6_abort_cloud_cancel_evidence | **RED** | 无 abort→云端取消生成观察记录（设备发送路径由 C 侧守卫；云端半未实测） |
| test_a7_system_prompt_f4_discipline_offline | **RED** | server/deploy/openclaw-system-prompt.v1.md 不存在——三句纪律/儿童化/E1 遵循无处钉死 |
| test_e4_system_prompt_versioned_landed | **RED** | 版本化 system prompt 未落盘（fence 指令/白名单/两部分/v1/增量扩展区全缺） |
| test_e4_system_prompt_wired_into_deploy_overlay | **RED** | deploy overlay / 网关声明面无 openclaw-system-prompt 引用——即便将来落盘也不得是孤儿文件 |
| test_live_ssh_reachable / test_a4_live_* / test_a7_live_* | 不在红基线（@live） | A4 = 对真 OpenClaw 统计遵循率（fence 请求不带 tools, 失败样本逐条处置落盘）；A7 = 百科问答链路（口语答案 + display text view + ≤3 句） |

## 红的构成一句话

**红 = VRT-A1/A3/A5 六条 C 齿对当前 story 缺失行为成立（固件无 mcp kidpal/display
消费面、无 view 路由、无 text 全文分页）+ python 五齿成立（system prompt 未落盘/
未接线、遵循度复验与 abort 云端取消证据未产生）；A2/A6 设备半与 G 生成面为原生
green 守卫。本包把它切到绿。** 无一红因是 import/collection 错误（C BUILD_CLEAN、
python 收集零错误、断言全落在缺失行为上）。

## 降级与留痕记录（诚实边界）

1. **闸 runner 未接线**：CMakeLists 接线属 builder（[C4.2]）；本次红以 stub runner
   执行同一文件得出，断言行为与闸上一致；builder 接线后 verifier 应在 host_app ELF
   上复跑（转绿期望 = 全 TEST_CASE PASS + EXIT=0，并把 view_router 实现源一并编入
   HOST_ACCEPTANCE_SRCS 使强符号覆盖 weak 默认）。
2. **python 验收文件落位偏差**：见头注记；断言零变更，由 planner 定归位方案。
3. **live 面**：A4 统计复验与 A7 问答链路真连 ECS/OpenClaw 才有意义，标 @live 不
   auto-run；若 live 探针被环境卡死（OpenClaw non-stream 不支持等）按 launch 纪律
   fail-closed 升级, 不降级「记录即过」。
4. **frozen contract 的 builder 输入**：纯逻辑 view router 建议落
   `firmware/main/display/view_router.(c|cc)`（C linkage, 无芯片依赖），数值码与
   语义以 C 验收文件头注为准——builder 唯一被迫接受的是这三个函数签名, 换取
   「行为真执行」的验收强度（G 包 python frozen-contract 先例的 C 对偶）。

---
---

# HP1 红基线判定摘要（TaskPackage 260829-home-p1 · acceptance-author 2026-09-07 实测）

（本节按 append 纪律追加，上文 VRT 段零改动。）

冻结验收文件：`firmware/main/test/home_p1.acceptance.c`（weak-symbol frozen
offline contract 模式，照 VRT 先例；taskpackage `acceptanceTestsFile` 钉的路径，
无落位偏差）。冻结契约 = `firmware/main/display/home_p1.c/h` 的 11 个纯逻辑
函数（数值码与语义冻结在验收文件头注），builder 落地强定义后 weak 哨兵 -2 被
覆盖、真逻辑被测。

## C 侧红基线（命令与实跑）

冻结验收文件的闸上 runner 判据 = host_app linux-target ELF（[C4.4]），但接线属
builder。本次用边界内脚手架（本目录 `unity.h` stub + `runner.c`，同 VRT；另加
`seam_stub.c` = host_infra 渲染缝最小替身，因裸 gcc 脚手架不编 LVGL）把**同一份
冻结验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /mnt/c/tmp/hp1_red_runner \
    ../../home_p1.acceptance.c ../../../display/view_router.c seam_stub.c runner.c
    # BUILD_CLEAN（零告警；view_router.c 显式链入 = guard 齿真执行包 2 强符号）
cd <repo root>
wsl -d Ubuntu-24.04 -- /mnt/c/tmp/hp1_red_runner   # 退出码 = 失败 TEST_CASE 数
```

实测：**11 Tests 10 Failures，ELF 退出码 = 10**。退出码按「直取 argv 形态」
单独核实（`wsl -- <elf>` 后 Windows 侧 `$?` = 10，与报告行 10 Failures 一致；
`bash -c` 包装下 `echo $?` 恒 0 已复现为 wsl.exe 参数重组伪影，勿用作判据）。

判读要点（冻结纪律：红必须红在真实行为缺失）：
- 全部行为齿红在 weak 哨兵 -2「frozen contract NOT implemented」或装配齿红在
  「模块不存在/未编入 CMakeLists」——不是编译/链接错误（BUILD_CLEAN 佐证）。
- guard 齿绿：包 2 view_router 冻结契约（home→1 / schedule→4 / text→0 /
  NULL→-1）经强符号真执行不回归；契约 §1.3 canary 在世；模块纯净性扫描在模块
  未落地时平凡绿（落地后为强守卫）。

### 逐条判定

| TEST_CASE | 判定 | 红因（各自真实缺失行为, 2026-09-07 实测） |
|---|---|---|
| P1-A1/P1-E1（验收文件在世 + 模块装配结构, structural） | **RED** | 齿 1/2 绿（验收文件在世于钉定路径 + 头注 builder duty 标记在位）；齿 3/4 红——firmware/main 无任何文件实现 frozen contract（`kidpal_home_p1_schedule_inject(` 零命中）、home_p1.c 未编入 firmware/main/CMakeLists.txt（装配缺失） |
| P1-E3/E2（路由底座不回归 + 契约 canary + 模块纯净性, guard） | GREEN（守卫, 必须保持绿） | 包 2 view_router 冻结契约强符号真执行全对（home/schedule/text/NULL）；契约 §1.3 home anchor 在世；纯净性扫描平凡绿（模块未落地） |
| P1-A3/E3（日程缓存 inject/读取 verbatim + 防御） | **RED** | frozen contract 未实现（weak 哨兵）——分组结果注入/verbatim 消费（设备不重算分组）/次日预告/整体替换/「今日无待办」清空/容量与形状防御全部缺失 |
| P1-E3（跨 0:00 日滚动） | **RED** | `kidpal_home_p1_day_rollover` 未实现——刷新三触发的设备半（跨天清缓存+更新日键）缺失（另两触发产生端归 producer/网关, 头注留根） |
| P1-A3（倒计时贴纸） | **RED** | `kidpal_home_p1_countdown` 未实现——最近未开始项选择/「3 个半小时」数值半/全过期无贴纸边界缺失 |
| P1-A2（眨眼冒号相位） | **RED** | `kidpal_home_p1_blink_visible` 未实现——4.4s 眨眼周期/1s 电池闪同款相位/周期灭窗唯一性缺失 |
| P1-A4（低电打盹判定） | **RED** | `kidpal_home_p1_doze_state` 未实现——阈值相对行为/充电叫醒覆盖缺失（阈值默认 15 已钉, 9b owns 调定） |
| P1-A5（布局优先级解析） | **RED** | `kidpal_home_p1_layout_resolve` 未实现——时间>接下来>温湿度贪心序/冲突时温湿度让位（隐藏）语义缺失 |
| P1-A2（P1 版面在位 + 冒号眨眼墨水差） | **RED** | `kidpal_home_p1_set_state`/`kidpal_home_p1_render_frame` 未实现（weak 哨兵）——P1 首页渲染面不存在，大时间/日期/温湿度/电量区域在位与眨眼墨水差无从执行 |
| P1-A3（接下来卡/贴纸/次日预告在位 + 空边界） | **RED** | 同上（渲染面缺失）——分组卡/倒计时贴纸/次日预告区域在位与「无待办全空」「全开始无贴纸」边界缺失 |
| P1-A4（打盹电池每秒闪 + 充电叫醒） | **RED** | 同上（渲染面缺失）——state-sleepy 电池闪墨水相位与充电覆盖渲染缺失 |

## 红的构成一句话

**红 = 10 条齿对当前 story 缺失行为成立（home_p1 纯逻辑模块 + P1 首页渲染面全
部未实现：日程缓存 verbatim 消费/跨天滚动/倒计时贴纸/眨眼相位/低电打盹/布局
优先级/版面在位）；1 条 guard 齿绿（包 2 view_router 底座 + 契约 canary 必须
保持绿）。本包把它切到绿。** 无一红因是编译/链接错误（BUILD_CLEAN），全部红因
落在 weak 哨兵或装配缺失的真实行为上。

## 降级与留痕记录（诚实边界）

1. **闸 runner 未接线**：`HOST_ACCEPTANCE_SRCS` 与 firmware/main/CMakeLists.txt
   接线属 builder（验收文件头 BUILDER DUTY 清单 1–5 条）；本次红以 stub runner
   执行同一文件得出，断言行为与闸上一致；builder 接线后 verifier 应在 host_app
   ELF 上复跑（转绿期望 = 全 TEST_CASE PASS + EXIT=0，并把
   `../../../display/home_p1.c` 一并编入 HOST_ACCEPTANCE_SRCS 使强符号覆盖
   weak 默认）。
2. **渲染缝替身**：`seam_stub.c`（本目录, 非冻结件）只为裸 gcc 脚手架供链；闸上
   用真 `host_app/main/host_infra_seam.c`（LVGL 软离屏渲染）。渲染齿在 stub 下
   红于渲染入口哨兵（远早于像素探针），替身内容不会掩盖任何红。
3. **红因精度修正一次（冻结前）**：P1-A4 render 齿初版在 `set_state` 哨兵检查前
   直接断言 `==0`，红因措辞不精确；冻结前补哨兵前置检查（断言语义不变），复跑
   确认红因统一为「frozen contract NOT implemented」。此后断言语义冻结不再改。
4. **taskpackage 字段不动**：`redBaselineResult` 为 planner 责任面，本摘要只落
   实测记录。
5. **统一红说明（acceptance-evidence 纪律）**：7 条行为齿共享「模块未实现」这一
   缺失原因，统一红对逐条断言敏感性零信息——上表已按条目给出各齿对应的缺失
   行为映射，转绿后 verifier 须做 mutation check（破坏一个真实行为期待变红），
   不得豁免。
