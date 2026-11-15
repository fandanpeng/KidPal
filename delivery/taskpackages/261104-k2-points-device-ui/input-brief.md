# input-brief · 261104-k2-points-device-ui（builder 备料，planner 落）

> K1 复盘点名「input-brief 行号级备料值回票价」——本件为 builder 起步备料。acceptance-author
> 已完成清单外实读（报告 §五.1 有清单+原因表），本 brief 转录其行号级事实与先例通道，builder
> 零考古起步。来源 = acceptance-author 交付报告 + K1 pod-events + K0 先例；行号以交付时 HEAD
> （dev/pod-m2 @ aea4285d）为准，实现动工后自然漂移。

## builder FIRST ACTION（不做的后续全白跑）

- **CMake 登记守**：向 `firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS`
  追加 `../../points_device_ui.acceptance.c`（K0 先例；points_page.c / keybar.c 已在册）。
  不登记则冻结件不进 host 构建，verifier 见盲区即红。

## 对端 seam 实读指针（acceptance-author 逐字实读过的对象）

- **冻结件**：`firmware/main/test/points_device_ui.acceptance.c`（7 TEST_CASE，weak 哨兵 −99
  即 seam 位）、`server/acceptance/test_points_device_ui.acceptance.py`（4 test）——**只读，绝不改**。
- **host 待落 seam**（K0 spike 现状 → K2 正式化）：`firmware/main/display/points_page.c`（spike 最小
  渲染）、`keybar.c` / `view_router.c|h`（K0a 四页骨架 + KSPV2 家族消歧/删除确认先例 seam 同构）、
  `story_page.h`（KSPV2_DELETE_IDLE_MS=10000UL / KSPV2_DISAMBIG_MS=300u 常量锚）。
- **mcp/protocol 面**：`mcp_server.cc` points 消费块（display 路由族）、`protocol.cc|h`
  （`SendKidpalPointsPage` 约定：`known_revision >= 0` 才拼键——TEST 4 逐字核对过）、
  `application.cc` 会话发起段（语境发送单点）。
- **server 面**：`server/gateway/front/front_app.py` 的 `_consume_kidpal_control` /
  `_push_display_payload` / `_consume_points_intent`（A4 单写入口锚：全模块 `gw_points.add_points(`
  恰 1 处）、`server/points.py`（PAGE_LIMIT_DEFAULT=6 / HOME_RECENT_MAX=3 / delete_entry
  revision=rev_before+1 / deleted=False 不写账）、`server/gateway/display_schema.py`
  `validate_display`。
- **语境映射家不锁**（acceptance-author 申报）：source_page 语境映射单点现居
  `display/story_page.c` 的 `kidpal_story_ctx_source_page`，但 **story_page.c 不在 changeManifest**。
  builder 二选一：扩点放 application.cc/protocol 层（manifest 内），或触 story_page.c 并**在交付
  报告申报 manifest 外偏差**。TEST 7 钉「三文件任一在世」，不锁家。

## K1 先例通道惯例（照办不发明）

- host 红基线通道：`firmware/main/test/acceptance/red-baseline/`（C 类实编，WSL gcc）——红基线
  独立冒烟用；正式 host 构建走 host_app runner。
- server 验收：`python -m pytest -q --import-mode=importlib server/acceptance/...`（仓根执行）。
- 相邻冻结件合跑纪律：K1 先例 = 三相邻件 + 冻结件合跑，确认零新增红后才交 verifier。

## K1 rulings 转录（承接判定，防跨包事实丢失）

- GAP-1 承接：QQ/restore 推送 `restore=True` 接线义务归 K2 设备面（本包 K2D-E2/TEST 系 +
  K2PTS-L2 真机段已冻结承载）。
- 第二笔 QQ canary（积分页在屏原地刷新）已按 human 2026-11-05 修正判据降级顺延至本包真机轮。
- K1L-L1/L2 真机轮（QQ 落账不抢前台 GAP-1 活性 + 真实消费端分页对账）挂本包 K2PTS-L2
  一起跑（human 2026-11-05 意向）——离线全绿不冒充真机终验。
