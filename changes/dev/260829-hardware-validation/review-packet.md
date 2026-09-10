# Review Packet · 260829-hardware-validation（包 9b 硬件真机验收：物理冒烟 + 全功能端到端 + F2 接线 + ECS 窗口）

> dev-loop integrator 收口 · 2026-09-10 · 分支 `dev/260829-hardware-validation`（worktree
> `C:/AI/KidPal-pod-hardware-validation`，HEAD `2c753ab7cc380a6026f9ef3bd5005d34edc3c228`，
> commit 冻结、**不 push、不 merge**）
> 承重磅据 = final verifier-round fact
> `.claude/taskpackages/260829-hardware-validation/verifier-evidence/rounds/r1.json`
> （`authority:"bearing"`，phase=final，round 1，invocationId `6a1ab284-bf72-4e69-b0eb-2195f3d60703`，
> `ok:true`，`observedGateExit:0`，9 闸全绿：taskpackage-required / firmware-build（真编译
> xiaozhi.bin 0x2c2060、app 分区 20% free）/ contract-lint / acceptance-coverage / mutation-check
> （skip：all-structural）/ docs-sync / security / unit 163P 6sk 45des / acceptance 126P 6sk 45des）。
> `.claude/taskpackages/current/verdict.json` 只是 non-bearing 兼容投影，本 packet 的判断不来自它。
> **本包特殊性（读人必知）**：testHost=device，16 条 acceptance 里约半数是「human 持板才能验」的
> 物理项——本 packet 按 HWV-E2 证据纪律逐条给「绿 / 部分绿 / OPEN」真值，**不把 OPEN 包装成绿**。
> 包内验收 scope 的真机面完成度（6 项绿/部分绿 + 8 项 OPEN）与代码面完成度（全部已 commit 且闸绿）
> 是两个独立事实，合并门裁断时请分开读。

## 0. Story 摘要（人读版）

把 B 段在电脑上验过的能力放到真硬件上确认。当日真机轮从「开机即挂」打到「语音轮 0 崩溃 0 复位」：
KidPal 四页 UI（home/char/schedule/story）首次上真机 RLCD 并 BOOT 双击翻页循环，首页 V3 换皮
（47 枚 KAP1 资产）可见；RTC 走时 + SHTC3 温湿度直读正确；证书自签 CA pinning 真机首连
`wss://8.137.91.16:8000` 握手通过；语音轮端到端（单击→ASR 全句→LLM 回答→故事页上屏→TTS 出声→
listening 平稳→「继续」再入 speaking）跑通，全程 0 崩溃 0 复位。代价是修了 9 项崩溃/缺陷链
（I2C 总线恢复 / BCD 解码 / SHTC3 唤醒+命令字 / 换皮消费接线 / DRAM 溢出迁 PSRAM / 单击路由 /
esp_timer 栈 / ssl_receive 栈 / SetStatus 悬空指针）——逐项根因见 PROGRESS-SUMMARY-260910.md §二。

## 1. 实现清单（两个 commit 的范围）

- **`3edbb24`「dev(9b): 硬件真机验收——渲染节点上屏 + 语音/故事/音频端到端跑通 + 栈溢出/悬空崩溃根治」**
  53 files，+7039/-316。四块：
  - **固件渲染节点 + 显示纪律**：`firmware/main/display/kidpal_render_node.cc/.h` 新建（设备专属、
    host 不编；BOOT 双击翻页 / 1s tick 前台对账 / **begin_page/end_page 渲染夹钳**（持 LVGL 锁 +
    预载 s_kidpal_scr + pin，MCP 7 处渲染闭包全部夹入——SetStatus 悬空崩溃的根治点））；
    `home_p1.c` 内容渲染（去实心块→描边 + imgfont 数字 + art blit）；char/reminder/schedule 页接
    夹钳；`view_router.c` 路由扩展；`s_p1_fb` 480KB 迁 PSRAM（`.ext_ram.bss` +
    `CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=y`）。
  - **板级 + 音频**：`boards/waveshare-s3-rlcd-4.2/{config.h,waveshare-s3-rlcd-4.2.cc}`（PCF85063
    BCD 解码 + SHTC3 wake 0x3517 + measure 0x7866 no-stretch + RecoverI2cBus）；`audio_service.cc`
    （三段埋点 + silk_resampler 上采样越界 +8 余量修复）；`sdkconfig` / `sdkconfig.defaults.esp32s3`
    （ESP_TIMER_TASK_STACK 3584→8192 等）；`partitions/v2/16m.csv`（新增 art 分区 0x780000/0x80000，
    ota_1 0x3f0000→0x370000——planner 裁定，pod-events `f9fbc6dc`）。
  - **F2 云侧接线 + 部署面**：`server/gateway/front/front_app.py`（uuid session_id + feed/stop +
    断连 cancel 钩子）；`openclaw-system-prompt.v4.md` 新增（F4 分工修正）+ CARRIER/overlay/
    provider 三面同切 v4；`mcp_server.cc`（HandleStoryKey + rec_tick + VRT 路由大缓冲挪堆 + 渲染闭包）。
  - **验收 + 证据**：`hardware_validation.acceptance.c` 新冻（结构齿 + 守卫）、
    `test_story_wiring.acceptance.py` 新冻（离线 5 齿 + live 4 齿）、`story_page_stack.test.c`
    防回归 guard（3 齿）、14 件真机日志 + PROGRESS-SUMMARY + 勾验清单。
- **`2c753ab`「docs(9b): 语音轮端到端链路梳理 + 硬件速查」** 4 files，+440/-1：
  `VOICE-ROUND-FLOW-260910.md`（链路梳理，见 docs-delta §2）、
  `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md`（硬件速查）、PROGRESS-SUMMARY §五、pod-events 台账。

## 2. 逐条 acceptance 状态表（HWV-A1~A16，诚实真值）

> 色标：**绿** = 真机 exercise 且证据落盘；**部分绿** = 子项绿 + 子项未验；**OPEN** = 未验（无证据，
> 不判色）；**殿后** = 显式留根、非本收口缺口。证据指针 = 日志文件:行 / 勾验清单行 / pod-events eventId。

| AC | 内容 | 状态 | 证据指针 |
|---|---|---|---|
| HWV-A1 | 结构（structural，无 summary） | **绿**（host 结构面） | `hardware_validation.acceptance.c` 4 TEST_CASE 进 host runner 全 PASS（一手复跑 104T 中含本包齿）；acceptance-coverage 闸列 16 AC 全 structural，`r1.json` ok:true |
| HWV-A2 | 按键手感（消抖/职责分离符合 §4.0） | **OPEN**（部分事实存在） | 单击/双击真机可用（boot8 单击修复 + 4 页翻页多轮实测）；**长按 story PAGE_TURN 冻结语义待验、消抖连按 20 次计数未做**——清单表 A「HWV-A2」结论列留白；且 KEY(GPIO18) 未接，spec §4.0 两键分工只验了 BOOT 半边（见 finding F1） |
| HWV-A3 | 双麦采音 + ASR 转写 | **部分绿** | ASR 上行 ✅ `voice-round2.log:25` `Application: >> 讲个故事`（全句正确转写）；音频输出管道 ✅ `voice-round2.log:214-225`（16k→24k 重采样 decode→playback→codec enabled=1→ES8311 出声）。**双麦录音清晰度 + 故事录制→停录→盖章整轮未 exercise**——清单表 A「HWV-A3」结论列如实记录 |
| HWV-A4 | RTC 走时 + 断电续航 + SHTC3 直读 | **部分绿** | RTC 走时 ✅ `voice-round2.log:101` `rtc_pcf85063 corr_id=kdp-251 ok rtc=20:48:21`（BCD 解码修复后）；SHTC3 ✅ `voice-round2.log:102` `temp_c=28.52 rh_pct=46.5`（0x7866 命令修正后，boot5 `temp_c=25.04 rh_pct=53.0` 与室温吻合）。**拔 18650 ≥10min 断电对时子项未 exercise**——清单表 A「HWV-A4」如实记录 |
| HWV-A5 | 低电打盹（15%）+ RLCD 真屏观感 | **OPEN** | 无真机证据（需放电至 15% + 三观感项拍照）；清单表 A「HWV-A5」留白。注意：页面观感类问题（对比度/可视角度）与 A7/A6 观感项同批待 human 持板 |
| HWV-A6 | 全功能端到端：4 页循环 + story 主流程 + 换皮 UI | **部分绿（大头已绿）** | BOOT 双击 4 页循环 ✅（HWV-E3 兜底达成）；换皮首页 ✅（`boot6.log:21-22` `kidpal_rn: pushing home 400x300 render_rc=0 state_rc=0`；human 目视「前 3 个可见 + 电池图标有」，pod-events `d203022d`）；语音→故事显示主流程 ✅（`voice-round4.log` 全绿轮：`:26` `story_text_dispatch corr_id=kdp-180112 screens=2 degrade=false` → `:38` speaking→listening 平稳通过即轮3崩溃点 → 全程 grep 崩溃标记 0 次）。**剩余**：课表页具体内容/字体占位（human 报「没用课表具体内容」）、故事正文分页观感（ksy_label 不 wrap，finding F6）——清单表 A「HWV-A6」逐项如实记录 |
| HWV-A7 | 16px 以下 CJK 1bpp 真机可读性 | **OPEN** | 未验。与字体问题耦合：正文用 `font_puhui_basic_14_1`（阿里普惠 basic 子集），次常用字「猬」渲染空白；V3 设计字体（站酷体 zcool_30 + imgfont）只用于首页。无 photo、无结论回写（docs-delta §4 遗留） |
| HWV-A8 | 前光确认（spec §5.6） | **OPEN** | 未验（暗处/亮处拍照判定）；清单表 A「HWV-A8」留白 |
| HWV-A9 | F2 设备侧页内输入接线（input/on_saved/rec_tick/stamp_spec 落盘且被生产路径调用≥1） | **绿（结构面）** | `mcp_server.cc` HandleStoryKey + rec_tick 接线、`story_page.c` stamp_spec render 消费（builder-completed `2470b34f` 交付清单）；`hardware_validation.acceptance.c` 结构齿 PASS。**真机页内交互 exercise 殿后**（依赖 HWV-A2 手势面 + F2-1 整轮） |
| HWV-A10 | F2 云侧停录归档接线（feed/stop + archive_story 到达面） | **部分绿（接线在仓，可达性存疑）** | `front_app.py` feed/stop 旁路 + `_story_route_display_payload` 已接（crossEnd counterparts 核对在案）；离线齿 5 PASS。**但 VOICE-ROUND-FLOW-260910.md §二.5 两个疑似断链**：①全固件无 `StopListening()` 调用方→`listen state=stop` 发不出，front 归档只认此帧；②manual 录音期间引擎不出 stt 帧→front 累积缓冲恒空。若坐实则「真录→归档→盖章」真机不可达——**F2-1 真机戳实前不判绿**（finding F5） |
| HWV-A11 | F6 会话键不可复用 + WS 断开钩子 pop | **绿（结构面）** | `front_app.py` uuid session_id + cancel 断连钩子（builder-completed `2470b34f`）；`test_story_wiring.acceptance.py` 离线齿覆盖（final acceptance 闸 126P 含本包齿） |
| HWV-A12 | F4 分工修正（story_text(record) 组块下发 + prompt 无「回填」） | **绿（制品面）** | `openclaw-system-prompt.v4.md` 新增 + 三面同切（CARRIER/overlay/provider）；部署**未执行**到 ECS（live 仍旧版 prompt，见 §12 残余 7） |
| HWV-A13 | 实机续航基准（决策 21 三要素） | **OPEN** | 未测（需满电常连 WS 跑至打盹记录时长）；清单表 B「续-1」留白 |
| HWV-A14 | 常驻 WS 连接功耗观察 | **OPEN** | 未测（需外接电流计两态各 10 分钟）；清单表 B「续-2」留白 |
| HWV-A15 | 证书真实设备握手冒烟 | **绿** | `voice-round2.log:14-15`：`WsTlsCa: wss cacert enabled … handshake verified against self-signed CA` → `esp-x509-crt-bundle: Certificate validated` → `WS: Session ID: 78eba971-…`；轮4 复连再证（`voice-round4.log:11-12`）。CA pinning 真机首连，非 Loopback |
| HWV-A16 | ECS 部署真落 + live 五齿 L1-L5 evidence | **OPEN（链路已通，部署面未落）** | ECS 语音链路已通 ✅（pod-events `053ae432`：front active / docker Up / live config 四 provider 全对 / 引擎日志完整语音回合）。**未落**：apply overlay 漂移未修（跑脚本会覆盖 live 断语音，finding F4）、提醒 cron §8 未部署、L1-L5 五齿无真跑 evidence。清单表 C 全留白 |

- **exitCriteria**：HWV-E1 结构 ✅（host 齿）；HWV-E2 真机证据非模拟 ✅ 进行中（14 件日志 + 勾验
  回写，但物理项未完）；HWV-E3 4 页循环兜底 ✅（真机多轮）；HWV-E4 AEC 喊停不在本包 ✅（声明性，
  未越界）。
- **structural 豁免枚举**（supervisor 门核此项）：acceptance-coverage 输出 16 AC 全 structural、
  `suiteGates: []`（`r1.json` 原文在案）。豁免理由 = device 物理项以真机日志为证据面（HWV-E2
  「非模拟」）、F2/live 结构项无逐 AC 行为语义，全部按 `kind:"structural"` 显式对象标记（260731
  成文通道），非 prose 洗白；mutation-check 相应 skip（`reason:"only structural acceptance ids"`）。

## 3. 机械闸实测（integrator 亲手跑，2026-09-10 收口）

| 闸 | 命令 | 实测 | 判定 |
|---|---|---|---|
| final fact 有效性 | `node .claude/scripts/verifier-evidence-check.mjs --change-id 260829-hardware-validation --phase final --round 1 --invocation-id 6a1ab284-bf72-4e69-b0eb-2195f3d60703` | 见收口回执（跑于 packet 写就前） | 须 PASS 才进 closeout |
| 设备 build | verify.mjs firmware-build 闸（r1 内，真编译 via venv python + idf_run.py） | `Project build complete`，`xiaozhi.bin binary size 0x2c2060`，`Smallest app partition is 0x370000. 0xadfa0 bytes (20%) free`，art.bin spiffsgen 在 `[2/5]` 步骤可见 | RC=0 真绿（非降级） |
| host ELF | `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`（仓库根姿势） | **104 Tests / 3 Failures / 0 Ignored**（101 PASS） | 3 红逐条归因见 §4——**均非本包代码引入** |
| server 全量套 | `cd server && python -m pytest acceptance/ -q --import-mode=importlib` | **126 passed / 51 skipped / 0 failed**（51 skip = 6 ssss 项 + 45 live deselected 齿） | 0 failed；与本包 final 闸 126P/6sk/45des 计数一致（deselected 在无 `-m` 过滤时呈现为 skip） |

- 未重跑 verify.mjs final（r1 已定版 immutable，重跑会在 preflight 红于 round 序号；承重事实 =
  r1，已在档）。integra­tor 一手补跑的是 host ELF 与 server 全量套两条实机闸。

## 4. 已知红（host runner 3 红，逐条归因）

> PROGRESS-SUMMARY-260910.md §四.D 记录当时 4 红（VRT-A2/E2 + WSA-A4/A6 证据门 + P2-A1）；
> 收口一手复跑 = **3 红**——VRT-A2/E2 已绿（VRT-A2 是 application.cc tts/stt 结构守卫，当前树满足；
> VRT-E2 证据面随本包制品落盘回收）。现存 3 红逐条：

| 齿 | 归因 | 回收归属 |
|---|---|---|
| WSA-A4（`device_ws_switch.acceptance.c:663`）`flash.log 落盘缺失` | 证据门：读的是**他包**（260903-device-ws-switch）的 flash.log 本地-only 证据件，本仓 gitignore、超 fixer 权限补投 | supervisor/human 侧回收（证据门缺制品，非代码缺陷） |
| WSA-A6（`device_ws_switch.acceptance.c:830`）`ecs-teardown-evidence.md 落盘缺失` | 同上，ECS shim 停服安全收尾证据未记录 | 同上 |
| P2-A1（`schedule_p2.acceptance.c:571`）`KSP-A1 RED: the render module carries no title bounce animation face (lv_anim)` | **first-match 歧义（先行红）**：该结构齿取 `kidpal_schedule_page_render(` first-match 文件再要求含 `lv_anim\|弹跳`，现恒命中**本包新增**的 `kidpal_render_node.cc:362`（无该标记）——本包改动**暴露**了齿的脆弱性，但「标题弹跳缺失」在 schedule_page.c 侧是否为真需重验 | 二选一交 planner：render_node 侧补真实标记，或 acceptance-author 修齿 first-match 歧义（本包不自行喂哨兵让红消失，见 finding F2） |

## 5. 跨包回归结论

- server 全量 0 failed：本包新冻 `test_story_wiring.acceptance.py` 离线齿全过；既有失败地板
  （smoke_e2e 结构断言组等）在本仓 verify 闸口径下呈 45 deselected + 6 skip，未新增红。
- host ELF 3 红 ≠ 本包代码引入：WSA-A4/A6 证据门读他包制品（`git diff` 未触
  `device_ws_switch.acceptance.c` 及其证据面）；P2-A1 见 §4（齿的 first-match 脆弱性，且断言目标
  在 schedule_page.c 侧）。
- detect_changes（项目 `C-AI-KidPal-pod-hardware-validation`，base=main / merge_base=`fc14e89`）：
  `seed_symbols: 202`、changed_files 55、**impacted_total: 13、全部收敛在 firmware/main 模块**
  （audio_service Initialize/Start/PlaySound、mcp_server AlertDrainTimerCb/HandleAlertCall、
  application Initialize/InitializeProtocol/ActivationTask、main.cc app_main、CMakeLists + 三个
  test 模块）——外溢半径受控，与「改动集中在固件显示/音频/板级 + front_app/deploy 面」的预期一致。

## 6. 架构 / 依赖卫生

- **契约边界**：`docs/contracts/**` 零改动（§7 机械佐证 + contract-lint 闸绿）。display 下发走既有
  view 白名单；`mcp_relay.py` / `display_schema.py` 本包未动 → 未绕过 contracts、无本地类型平行面。
- **重复实现**：`kidpal_render_node.cc` 为设备专属显示仲裁（host 不编），与 host 侧渲染面是「同源
  不同 host」的既有分工（s_p1_fb weak 符号交接），非新重复。
- **跨层调用**：设备只消费下发块（mcp tools/call），front 只校验转发（display_schema 白名单）；
  无 firmware→server 反向依赖、无循环依赖。
- **依赖卫生**：零新增第三方依赖；art 资产走既有 SPIFFS 分区机制；`managed_components/**` 零触碰
  （esp-ml307 的 ssl_receive 4KB 栈纪律用「挪堆」而非改组件，story_page_stack.test.c 防回归）。
- **越界核对**：`git diff 3edbb24~1..2c753ab --name-only` 55 文件全部落在
  `firmware/main/**` / `server/**` / `changes/dev/260829-hardware-validation/**` /
  `.claude/taskpackages/260829-hardware-validation/**`（+current 投影）/ `docs/notes/**`（planner/
  主线程链路梳理产物，notes 区属 dev-loop 可写面）；参考树 `ESP32-S3-RLCD-4.2-Demo/**` 零接触。
- **分区表变更**（16m.csv art 0x780000 + ota_1 缩 512KB）是 flash 布局裁决，经 planner 裁定
  （pod-events `f9fbc6dc`）、U 包 canary 齿与 HWV 齿在 verify 全绿中未被打破。

## 7. 机械闸预检（收口命令与结果）

| 闸 | 命令 | 结果 |
|---|---|---|
| final 证据有效性 | `node .claude/scripts/verifier-evidence-check.mjs --change-id 260829-hardware-validation --phase final --round 1 --invocation-id 6a1ab284-bf72-4e69-b0eb-2195f3d60703` | 见收口回执（PASS 方进 commit） |
| closeout | `node .claude/scripts/closeout-check.mjs`（默认只读） | 见收口回执；advisory 逐条归属在回执 |
| 冻结面核对 | `git diff 3edbb24~1..2c753ab --name-only | grep -E "docs/contracts|\.acceptance\.|managed_components"` | **`docs/contracts/**` 零命中、`managed_components/**` 零命中；`.acceptance.*` 仅 2 条 = 本包自冻新件**（`firmware/main/test/hardware_validation.acceptance.c` + `server/acceptance/test_story_wiring.acceptance.py`，均为 acceptance-author 冻结新增 A，无任何**既有**验收文件 M） |

## 8. Change Size

- Files changed：**55**（`3edbb24` 53 files +7039/-316；`2c753ab` 4 files +440/-1；合计 +7478/-316）。
- 其中实现面 `firmware/** + server/**` ≈ 26 文件；真机证据 14 件日志（HWV-E2 承载）；治理/证据面
  （taskpackages + changes/dev + docs/notes）其余。
- Scope：单包 `subsystem:"firmware+server"`，与 TaskPackage 声明一致；testHost=device 的真机证据
  面是本包特有的大头。
- **Split recommendation：不建议拆**。渲染节点 + 夹钳 + 栈纪律是同一真机验收链的相互依存修复
  （拆开任何一件，语音轮 4 的 0 崩溃结论不成立）。已知殿后项（故事页排版、字体、课表页、ECS
  漂移、F2-1 戳实、物理项）**建议另开**，见 §12。
- 回滚：包改动集中在 feature 分支两点（`3edbb24`+`2c753ab`），合并后回滚 = `git revert -m 1
  <merge-sha>`（本地 main，merge-driver 产物）；设备已烧录件不受 revert 影响（无 OTA）。

## 9. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| **SHOULD** | hard-violation | Correctness / spec 对齐 | **F1 · spec 键位分工 vs 实现漂移（已识别、未处置）**：spec 设计两键分工（KEY 主司对话 / BOOT 主司翻页），实现仅接 BOOT 一键（单击=对话/双击=翻页），KEY(GPIO18) 完全未接；依附 KEY 的页内特例（P2 问课程 / P3 列表光标+双击进阅读 / P3 阅读念给我听 / P4 播笔顺 / KEY 长按回首页 / 提醒弹窗 KEY 关闭）全部悬空，HWV-A2 的「职责分离符合 §4.0」判据因此只能验 BOOT 半边。 | `docs/spec.md` §4.0 全局手势表 + §5.3「KEY 主司对话……我们改为 KEY 主司对话」为设计权威（该 spec 断言与实现行为的对照逐字记录在 `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md` §一「spec↔实现键位对照」）；实现面 hunk：`firmware/main/boards/waveshare-s3-rlcd-4.2/config.h`（`BOOT_BUTTON_GPIO GPIO_NUM_0` 为唯一按键定义）+ `waveshare-s3-rlcd-4.2.cc:202`（单击 show_chat+ToggleChat / 双击翻页）。过渡方案在真机轮被 human 目击运行、未被告知违反 spec，故不判 BLOCKING。 | 交 planner/human 裁决二选一：① 新包补接 KEY（GPIO18）落地 spec 两键分工；② spec 过渡性修订承认一键方案（Track B 七步）。integrator 不改 spec。 |
| **SHOULD** | judgement-call | Test quality（齿的 first-match 脆弱性） | **F2 · P2-A1 结构齿 first-match 歧义被本包暴露**：齿取 `kidpal_schedule_page_render(` first-match 文件判定「标题弹跳 face」，本包新增 `kidpal_render_node.cc:362`（render_node 内的 schedule 缓存渲染调用点）恒被命中，齿红但 schedule_page.c 侧真实状态被遮蔽。 | changed-code hunk：`firmware/main/display/kidpal_render_node.cc` 新建含 `kidpal_schedule_page_render(` 调用（`git diff 3edbb24~1..2c753ab`）；齿原文 `firmware/main/test/schedule_p2.acceptance.c:571`（KSP-A1 RED 文案）；一手复跑 host ELF 104T/3F 含此红。heuristic：first-match 文件选择在「同名调用点跨文件扩散」时失效——属 judgement-call（齿行为），非本包违反任何规则。 | 交 planner 二选一：render_node 侧补真实标记（若弹跳 face 本就该跨渲染路径存在）或 acceptance-author 修齿为双文件/白名单解析。**禁止**喂哨兵字符串让红消失。 |
| **SHOULD** | judgement-call | Operability / 部署面 | **F3 · ECS overlay 部署漂移（危险项）**：`apply-xiaozhi-overlay.sh:60/62` 仍写 ASR=AliyunStreamASR / Intent=function_call，与 live config（Qwen3ASRFlash / nointent）不符——**直接跑 overlay 会覆盖 live、断语音**。 | changed-code hunk：`server/deploy/apply-xiaozhi-overlay.sh:60/62`（本包只切了 PROMPT_SRC/DST v4，未动 ASR/Intent 行）；live 对照 = pod-events `053ae432`（ECS 体检只读：`selected_module ASR Qwen3ASRFlash / Intent nointent`）；VOICE-ROUND-FLOW-260910.md §七.5 已列为云端实验前置项。 | 交 planner 立项：先修漂移使脚本=live，再选择性部署；提醒 cron §8 部署与 live 五齿 L1-L5 evidence 随同批（HWV-A16 剩余面）。 |
| **SHOULD** | judgement-call | Correctness / 端到端可达性 | **F4 · 故事录音云侧归档双断链（疑似，待真机戳实）**：①全固件无 `Application::StopListening()` 调用方→`listen state=stop` 发不出，front 停录归档（`_story_stop_on_listen_stop`）只认该帧；②manual 录音期间引擎只攒音频不出 stt 帧→front 累积缓冲恒空，即使修①归档也拿空转写（`archived=false reason=empty_accum`）。若坐实，「真录故事→archive_story 落盘→story_text 回看→盖章」整轮真机不可达。 | changed-code/grep：全固件无 `StopListening` 调用点（VOICE-ROUND-FLOW-260910.md §二.5 逐文件核对，`firmware/main/application.cc:600` 与 `waveshare-s3-rlcd-4.2.cc:194-222` 手势路由）；front 面 `server/gateway/front/front_app.py`（`_story_feed_from_stt` 喂缓冲 + listen-stop 归档触发）。标注为**疑似**：链路梳理是事实核对，未真机 exercise（HWV-A3/F2-1 录音整轮 OPEN）。 | F2-1 真机戳实（human 持板）：真录一段→停录→看归档/盖章。若坐实，立修包（设备补 StopListening 接线 + 引擎整段 ASR 结果交网关归档，或弃 front 累积改直通归档——修法二选一已在 VOICE-ROUND-FLOW §二.5 列）。 |
| **SHOULD** | judgement-call | 设备观感（9b 殿后项到期） | **F5 · 故事页排版**：`story_page.c` 的 `ksy_label`（story_page.c:1062 一带）建标签不设宽度、不开 wrap→正文 `story->text`（可达 2047 字）当一整行渲染、横向裁切（human 实测「小刺猬」只见「小刺」）；`kidpal_story_open_reader` 算了 screens 但渲染端画全文不消费分页。 | changed-code hunk：`firmware/main/display/story_page.c`（本包 +152 行改造范围内，渲染夹钳接入了排版面但排版本身未修）；human 目视记录 pod-events `9e4f6a8b`（轮4 剩余殿后清单明列）。PROGRESS-SUMMARY §四.A.1 载修法与风险（改布局须保 host runner 冻结齿 8 齿绿）。 | 交 planner 立项修包：ksy_label 开 wrap + reader 按 screens 分页；碰 story_p3 冻结齿则按纪律升级 acceptance-author。 |
| **NIT** | judgement-call | 设备观感 / 架构 | **F6 · 设备 UI 三屏主权混乱（架构根因，症状已修架构未变）**：s_chat_scr / s_kidpal_scr / char 独立屏三屏 + 两套 UI 体系，前台靠 3 标志 + 1s tick 对账；回答文本写在被 pin 覆盖的聊天屏、显示滞后语音 1–3s、轮间驻留结局随机。当日三个崩溃（悬空指针/clean 聊天屏/栈溢出同族）的结构性土壤。 | `firmware/main/display/kidpal_render_node.cc`（tick/begin_page/pin 仲裁）+ VOICE-ROUND-FLOW-260910.md §五 6 项逐条代码证据（三屏建立处、写方、竞态窗口）。 | 不阻塞。VOICE-ROUND-FLOW §七.1/2/4 改法候选（UI 主权收口 / 围栏闭合即发块 / 回答驻留）交 human 挑优先级立项。 |
| **NIT** | judgement-call | Operability / 体验 | **F7 · 首音时延 3–6s（四段串行云 RTT）**：VAD 尾点 1s（silero `min_silence_duration_ms` 默认值）+ 整句非流式 ASR + LLM 首 token + EdgeTTS 整段合成；固件/front 非瓶颈。 | VOICE-ROUND-FLOW-260910.md §六逐跳估算表（八段构成 + 每段可测证据方法）。 | 不阻塞。提速候选（流式 ASR / 调短 min_silence / 精简 prompt）见 §七.3——**任何引擎配置实验前先修 F3 部署漂移**。 |

**无 BLOCKING finding**（合并门可进，但见 §14 的 ESCALATE 预期）。F1 的 spec 漂移是「已识别待
裁决」而非「隐瞒违 spec」；F3/F4 是本包界外的部署面/真机面；F5/F6/F7 是殿后项如实登记。结构闸
（§3/§7）不复述为 finding；上表全部来自闸后五轴语义 review。

## 10. Five-Axis Review（gate 全绿仍完整执行）

1. **Correctness（对每个声明的 HWV-Ay）**：代码面接线（A9/A10/A11/A12）有冻结结构齿 + 离线齿
   背书；真机面 A15/A4 走时侧/A3 上行侧/A6 主流程有逐行日志证据（§2）。边界条件实核：渲染夹钳
   begin_page/end_page 覆盖 MCP 7 处渲染闭包 + 同族提醒弹窗路径（RaiseReminderQueue）一并夹入；
   栈纪律三处（esp_timer 8192 / ssl_receive 大缓冲挪堆 / resampler +8 余量）各有根因与
   story_page_stack.test.c 防回归。缺口如实记：A10 归档可达性存疑（F4）、A2 长按/消抖未验、
   物理四项全 OPEN（§2）。跨面一致性：prompt v4 与 front 组块下发分工一致（A12 制品面绿）。
2. **Test quality**：断言真行为——本包齿含防回归 guard（story_page_stack.test.c 3 齿钉「大缓冲
   挪堆」不变式，防栈溢出修复被回退）；设备 build 闸是真编译非降级（r1 firmware-build
   `phase:build-pass`、非 degraded）。弱点如实记：①本包 16 AC 全 structural→mutation-check skip，
   逐 id 变异不适用（M-4 口径，device 物理项本就无法 mutation）；②结构齿的 first-match 脆弱性
   （F2）与 keyword-existence 型证据齿的散文可满足性是既有家族弱点，本包未新增同型齿。
3. **Contract / boundary safety**：零契约改动（§7 三重佐证：git 范围 grep 空 + contract-lint 绿 +
   display_schema/mcp_relay 未动）。display 下发载荷形状走既有 `validate_display` 白名单；
   story_text 组块下发是 story-p3 契约面的消费接线，无新增字段。未发现影子类型/平行契约。
4. **Operability / security**：日志面结构化（`corr_id=kdp-*` correlationId 链贯穿 voice-round
   各轮、`degrade=false` 机器可 grep）；崩溃诊断有 addr2line 精确锚（PROGRESS-SUMMARY §二.9）。
   零新端口/零鉴权改动（front Bearer 鉴权既有面）；零 secret 落仓（真机日志含内网 IP
   8.137.91.16——与已入库部署文档口径一致，无凭据）。`securityReviewRequired=false` 按闸通过。
   **残余隐私面（FYI）**：语音整句上行 ECS 云（Qwen3ASR/dashscope），MVP 单孩子设备可接受，
   建议隐私面复议时一并看。
5. **Maintainability / simplicity**：改动围绕「让真机跑通」的最短路径，无过早抽象（渲染节点单文件
   成形、夹钳是函数对而非框架）；`minimal:` 标记 8 处（本包 diff 内 6 处 + 既有 2 处漂移）全部带
   殿后去向（§11 台账）。诊断埋点（音频管道三段 + 栈高水位）是「为下一轮排障留设施」的正当复杂度，
   不打 shrink。

## 11. Simplification Debt（`minimal:` 标记台账，收口一手 grep）

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `firmware/main/audio/audio_service.cc:344` | silk_resampler 上采样 +8 余量后截回（非整除输入最多多写 1 样本） | 精确按 resampler 输入块对齐 | **有**：采样率映射再变更时 |
| `firmware/main/mcp_server.cc:728` | 阅读态分页行高按 400x300 最小估算 | 真实分栏排版（与 F5 同源） | **有**：故事页排版修包 |
| `firmware/main/display/reminder_page.c:977` | 网点层按 16/32bit 色深用半透明压屏等效 | LVGL 原生 dash/纹理 | **有**：提醒弹窗观感复议 |
| `firmware/main/display/story_page.c:1040/1165/1197/1276` | 布局常量 host seam 坐标 / fruit 显示位钳 16 / total_screens 钳 12 / 内圈虚线等效 | 真机排版（**殿后 9b 已到期**，即 F5） | **有（已触发）**：F5 修包 |
| `firmware/main/display/schedule_page.c:623` | 布局常量按 host seam 定坐标（story-p3/schedule-p2 既有，行号漂移） | 真机 RLCD 观感校准 | **有**：课表页观感修包（human 报「没用课表具体内容/字体正体」） |
| （非本包，同 grep 命中）`server/shim.py:295` / `firmware/scripts/Image_Converter/LVGLImage.py:540` | 既有面 | — | 不入本包台账 |

## 12. Residual Risk

**已知限制（本包 shipped 边界）**

1. **物理验收 6 项 OPEN**：HWV-A2（长按/消抖）、A3（录音清晰度/整轮）、A4（断电对时）、A5（低电
   打盹/RLCD 观感）、A7（CJK 可读性）、A8（前光）——全部需 human 持板，执行面 =
   `.claude/taskpackages/260829-hardware-validation/device-acceptance-checklist.md`。
2. **续航双项 OPEN**：HWV-A13/A14（决策 21 三要素承载 + WS 功耗观察）——触发点在本包的预警
   阈值定标数据缺位，决策 21 的 DEFER 状态不能关闭。
3. **HWV-A16 未完**：部署漂移（F3，危险）+ 提醒 cron §8 + live 五齿 L1-L5 evidence。
4. **F2 真机可达性存疑**：断链①②（F4）待 F2-1 戳实；A10 真机面不判绿。
5. **host runner 3 红**（§4）：2 证据门红归属他包回收、1 齿歧义交 planner——合并后它们仍在树。
6. **spec 键位漂移**（F1）与 **VOICE-ROUND-FLOW→spec 同步**均待 human/review，spec 暂与实现
   不一致地共存。
7. **部署未执行**：prompt v4 制品在仓，ECS live 仍旧版；ECS overlay 漂移未修前**禁止**跑
   apply-xiaozhi-overlay.sh。
8. **设备 UI 三屏主权**（F6）：崩溃症状已根治，架构土壤未动；后续 UI 修包需过 host 矩阵回归评估。

**decisionCoverage 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目**

`taskpackage.json.decisionCoverage` 共 4 条（PROPOSAL-§2-包9b → `acceptance:HWV-A6`；
PROPOSAL-§7-跨页翻页兜底 → `exitCriteria:HWV-E3`；PROPOSAL-§3-硬件殿后汇总 →
`acceptance:HWV-A4`；PROPOSAL-§7-9b新增两条真机验收 → `acceptance:HWV-A7`），`landsIn` 全部落在
acceptance/exitCriteria，**`landsIn:"dropped"` 条目 0 条、`risk-accepted` 开头条目 0 条** → 无
human risk-accepted 残留需列。（注：intake 阶段两条 human 裁定——F2 扩界 `026cdaa9`、ECS 窗口
`04174089`——经 planner 处理收进 acceptance summaries + pod-events，未走 dropped 通道。）

## 13. 未执行 / 不适用项（如实记录）

- **S2 official exploratory scan：未执行**。TaskPackage 未声明 `s2ExploratoryRequired` /
  `s2ExploratoryGoals`，按角色边界不自创题目。本包的探索式证据以真机四轮日志 + 链路梳理件承载。
- **UI design-review 证据**：TaskPackage 未声明 designReviewRequired。本包确有大量 UI 工作面
  （渲染节点/四页/换皮），其证据以真机日志 + human 目视记录（pod-events `d203022d` 等）承载；
  viewport/a11y/design-drift 摘要未产。该 intake 现状交 planner 判断。
- **`.delivery/milestones.jsonl` / `.planning/kidpal-mvp/progress.md` 收口行**：归 planner 相位，
  integrator 不代写（见 docs-delta §4）。
- **changes/INDEX.md 行**：收口时 INDEX dev 段无 `260829-hardware-validation` 行；本次 closeout
  commit 按派单约束只含三制品 + pod-events，INDEX 行投影交 planner/主线程跑
  `closeout-check --update-index` 或全量重建（story-p3 先例同）。
- **`verdict.json` / `verification.json` 投影**：脚本产线件（advisory-only），不由 agent 手造；
  承重 = `verifier-evidence/rounds/r1.json`。

## 14. 制品、commit 与合并门输入

- 晋升 `changes/dev/260829-hardware-validation/`：`docs-delta.md` + `review-packet.md` +
  `pod-retro.md`（三件；pod-retro 本次按收口派单由 integrator 落）。post-cutoff 口径确认：不产
  `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` /
  `test-evidence.md` / `final-summary.md` / changes/dev 下 taskpackage 快照（docs-delta §3）；
  `pod-events.jsonl` 在档（advisory，非 blocking），本次追加 integrator closeout 事件。
- commit：见本节末（feature 分支本地 commit，**不 push、不在 main 上 commit**）。
- 回滚路径：见 §8。
- supervisor 输入清单：final fact `r1.json`（ok:true，9 闸，真编译）+ §3 一手闸计数（host ELF
  104T/3F、server 126P/0F）+ §2 逐 AC 真值表（6 绿/部分绿 + 8 OPEN/殿后）+ §4 3 红归因 +
  §9 findings（**0 BLOCKING / 5 SHOULD / 2 NIT**）+ §11 简化债（1 项已触发到期）+ §12 残余风险
  8 条 + decisionCoverage dropped 为 0。
- **本 packet 不自行判定通过**：合并门由 planner 跑 `supervisor-verdict-gate.mjs` 机械闸裁断。
  **ESCALATE 预期明示**：riskTier=high + 16 AC 中 8 项 OPEN + host runner 3 红 + spec 漂移待裁，
  机械闸判 ESCALATE 属**正常预期**而非异常——升级人类时应带的判断材料是：①真机端到端主链已绿
  （§2 A6/A15），OPEN 项全部是「human 持板才能验」或「已立项待修」的显式殿后，不是隐瞒缺口；
  ②两个最危险项（F3 部署漂移、F4 断链）都有明确前置关系与修法；③host 3 红的回收归属（2 证据门
  归他包/主线程、1 齿歧义归 planner）需要 human 知情后排优先级。

### commit（本次收口）

- 三制品 + pod-events 追加 = 单独一个 commit（本文件写就后紧接 `git add` → `git diff --cached
  --stat` 核对 → commit），hash 见交付回执。
- 分支既有两点：`3edbb24`（真机轮实现 + 证据）→ `2c753ab`（链路梳理 + 硬件速查）→ **本次收口
  commit**。
- 工作树 `.codebase-memory/{artifact.json,graph.db.zst}` 为工具派生快照（常驻脏面），本次 commit
  **有意不纳入 staged**；`.claude/taskpackages/**`/`.delivery/STATUS.md` 的 verify-runs 与
  verifier-evidence 等机器产线件亦不混入本 commit（归 planner/主线程节奏处理）。
