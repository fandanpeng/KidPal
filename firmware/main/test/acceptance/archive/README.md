# 冻结验收归档区（archive）

> 2026-09-15 human 裁定（M1 阶段一已进入）：MVP / 验证期积累的冻结验收不再编入 host
> runner 每轮全量跑，统一归档于此。归档 = `git mv` 移位 + 从
> `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS` 摘除 +
> 本清单登记；文件本体冻结态不变（builder/verifier 仍不得改写），历史 TaskPackage 的
> 验收证据链不受影响（文件仍在本仓、可按路径检索）。

## 归档件清单（13 件，来源包 → 文件）

| 来源包（era） | 归档文件 | 覆盖面 |
|---|---|---|
| 260829 批（MVP） | schedule_p2.acceptance.c | 260829-schedule-p2：P2 课表页渲染与数据契约 |
| 260829 批（MVP） | session_logic.acceptance.c | 260829-session-logic：会话状态机 + 键手势解析 |
| 260829 批（MVP） | voice_roundtrip_b.acceptance.c | 260829-voice-roundtrip：语音轮路由 |
| 260829 批（MVP） | home_p1.acceptance.c | 260829-home-p1：首页常显信息牌 |
| 260829 批（MVP） | char_stroke.acceptance.c | 260829-char-stroke：查生字 P4 |
| 260829 批（MVP） | schedule_reminder_b.acceptance.c | 260829-schedule-reminder：提醒弹窗消费面（KD-A4 钉点批改后终版） |
| 260829 批（MVP） | story_p3.acceptance.c | 260829-story-p3：P3 故事页 |
| 260901 批（验证期） | ui_art_integration.acceptance.c | 260901-ui-art-integration：美术资产层 |
| 260903 批（验证期） | device_ws_switch.acceptance.c | 260903-device-ws-switch：设备 WS 切换（在册红 WSA-A4/WSA-A6 随归档消红） |
| 260907 批（验证期） | hardware_validation.acceptance.c | 260907-hardware-validation：包 9b 树级结构守卫 |
| 260911 批（M1 前置） | key_route_a1.acceptance.c | 260911-key-route-a1：键位路由 PKG-1（在册红 KR-A6 随归档消红） |
| 260911 批（M1 前置） | screen_unification.acceptance.c | 260911-screen-unification：归一屏 layer 栈（在册红 SU-E3 随归档消红） |
| 260911 批（M1 前置） | v3_restore.acceptance.c | 260911-render-pages：V3 整屏还原 + 五态演出 |

## 归档时点事实

- 归档前 host runner 基线（K1a 包记录）：135 Tests / 4 Failures——4 红 = 上述 SU-E3 /
  KR-A6 / WSA-A4 / WSA-A6 四条在册既存红（见
  `delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/existing-red-inventory.md`
  R3–R6），全部落在归档件内，随归档自然消红；非修复、非豁免，勿计入修复战果。
- 归档同时从 host 构建摘除仅被归档验收消费的三个被测实现：
  `session_state_machine.cc` / `view_router.c` / `kidpal_voice_states.c`。其余被测实现
  （home_p1 / schedule_page / story_page / reminder_page / char_stroke / art_assets /
  key_gesture_parser.cc / kidpal_layer.c）保留——`happy_flow_capture.test.c` 与
  key_layer_dispatch 行为齿仍在消费。

## 归档件清单（第二批 · 普通 .test.c 单测，2026-09-15 同日 human 裁定）

| 来源包 | 归档文件 | 覆盖面 | 备注 |
|---|---|---|---|
| 260915-m1-k1a-key-layer-dispatch | key_layer_session_cascade.test.c | KD-A5 真机修复回归守卫（OVERLAY_CLOSE 级联结束会话 / 提醒关层不级联） | ⚠️ 归档后该修复仅剩 K1a 冻结验收结构齿间接覆盖，回潮时无独立守卫 |
| 260829-schedule-reminder | reminder_page.test.c | reminder_page.c 纯逻辑单测（fire_at / 截断 / 淘汰 / 顶替） | K4 落地时由其 acceptance-author 随新验收重挂 |
| 260913-happy-flow-host-capture | happy_flow_capture.test.c | Happy Flow 各页 host 真渲染 BMP 落盘（截图轮工具） | 截图配方文档在 HOST_TEST_CONVENTIONS.md「Host 整帧截图办法」节，需要时按配方复活 |
| 260907-hardware-validation | story_page_stack.test.c | ssl_receive 4096B 栈预算 + 大栈缓冲不回潮守卫 | |
| 260829-cloud-link 期 | ws_provision.test.cpp | 配网 WS 逻辑单测 | |
| 260829-host-test-infra（包 0） | seam_bounds.test.c | 渲染缝 NOT_RENDERED 哨兵 / 越界探针边界 | 原 host_app/main/ 组件内文件 |

第二批同时从 host 构建摘除仅被这批单测消费的被测实现：home_p1.c / schedule_page.c /
story_page.c / char_stroke.c / art_assets.c / reminder_page.c（设备实现面不动，仅出
host test 工程）。归档后 host runner 现役 = key_layer_dispatch.acceptance.c +
host_infra_smoke.acceptance.c 两件 + 其被测实现 key_gesture_parser.cc / kidpal_layer.c。

## 后续纪律

- 归档件**不得**重新接线进 `HOST_ACCEPTANCE_SRCS`；M1 后续包（K2/K3/K4/N1b）各自新写
  冻结验收文件，置于 `../../host_infra_smoke.acceptance.c` 之前（smoke 末位纪律不变）。
- 若某归档行为在 M1+ 被改动需要回归守卫，由改动包的 acceptance-author 新写钉点，不复活
  整份归档件。
- `red-baseline/` 目录不受本次归档影响（历史红基线 runner 与摘要照旧）。
