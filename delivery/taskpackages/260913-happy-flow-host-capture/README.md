# Happy Flow 全页面 host 截图记录（2026-09-13）

> **性质**：human 一次性授权的主线程直做操作（**跳过 dev-loop 流程**，Hard Rules 例外条款如实记录）。
> 产物 = host 渲染缝落盘的整帧截图，作为当前渲染实现面的一次验证结果记录。非 RP-A5 真机对照轮
> 替代品——真机观感终审仍归 human 持板轮。
> **方法已固化**：[HOST_TEST_CONVENTIONS.md](../../firmware/main/test/HOST_TEST_CONVENTIONS.md) 的
> 「Host 整帧截图办法（HOST CAPTURE）」一节——其他 pod 截图照该配方办，本文只是首用记录。

## 这是什么

把 Happy Flow 涉及的、host runner 已有渲染面的页面，用与冻结验收测试**同源同路径**的 LVGL 软渲染
逐页真渲染，经渲染缝新增的 additive 落盘函数 `host_infra_dump_bmp()`（
`firmware/main/test/host_app/main/host_infra_seam.c`）整帧写成 BMP，转 PNG 存档。驱动代码 =
`firmware/main/test/happy_flow_capture.test.c`（普通 `.test.c`，非冻结件，env 门控
`KIDPAL_CAPTURE_DIR`）。

## 截图清单

| 文件 | 页面/状态 | 渲染来源 |
|------|-----------|----------|
| `capture/01_home_p1.png` | P1 首页（10:24 / 22°C 45% / 电量 85% / 今日日程 + 14:00 书本 + 铃铛倒计时） | `kidpal_home_p1_render_frame`（P1-A2 同源） |
| `capture/02_schedule_today.png` | P2 课表·今天（12:00 时点：10:00 空心已上 / 16:30 实心待上） | `kidpal_schedule_page_render`（P2-A3 同源） |
| `capture/03_schedule_tomorrow.png` | P2 课表·明天（圆点全空心态） | 同上 |
| `capture/04_story_list.png` | P3 故事列表（3 条 + 故事果 ×4，首行选中） | `kidpal_story_page_render` LIST（P3-A2 同源） |
| `capture/05_story_reader_screen1.png` | P3 阅读·第 1 屏（10 行正文分 3 屏的第 1 屏） | READER（P3-A5 同源） |
| `capture/06_story_reader_screen2.png` | P3 阅读·第 2 屏（BOOT 长按翻屏后） | 同上 |
| `capture/07_story_recording.png` | P3 录音态（REC 演出 + 00:47 大时长） | RECORDING（P3-A4 同源） |
| `capture/08_story_stamp.png` | P3 盖章帧（故事徽章·第 4 颗） | STAMP（P3-A6 同源） |
| `capture/09_char_stroke.png` | P4 查生字（真实光栅管线 + 速查卡，画布合成「十」+ 米字格） | `kidpal_char_stroke_parse/transform/raster_stroke`（CHR-A4 同源） |
| `capture/10_reminder_popup.png` | 提醒弹窗（明天钢琴课 16:15，pose-jump + 压暗） | `kidpal_reminder_page_render`（RMD-A5 同源） |

## 与真机的已知差异（读图须知）

1. **字体（已知未收口项）**：host 用 LVGL 内置 **Source Han Sans SC 14 CJK——仍是部分子集**
   （约 1000–3000 字）；截图里 琴/勇/钢/刺猬 等次常用字是豆腐块，04 号图「钢琴/英语/钓鱼」、
   09 号图「十个」的「个」等可见。真机走 V3 字体资产（font_body.bin，GB2312 全量，SPIFFS
   fonts_root 加载），字形观感**不代表真机**。根因与候选修法（Kconfig 无 `LV_USE_BINFONT`、
   `LV_FONT_DEFAULT_*` 是死配置等）固化在 HOST_TEST_CONVENTIONS.md「字体现实」节。
2. **P4 笔画数据**：光栅走真实 D3 管线（parse → transform → raster_stroke），但笔画数据是合法
   合成闭合轮廓（「十」两笔），非 makemeahanzi 真字库条目——验证的是管线，不是字形美学。
   像素帧用 `lv_canvas` + `lv_draw_buf_create` 合成（裸 lv_image_dsc_t + static 缓冲实测不绘制）。
3. **语音轮五态不在内**：五态演出内容面（RP-A9）在 host 无渲染面（落点在设备侧聊天 layer），
   无法 host 截图；提醒态的弹窗本体（压暗 + pose-jump）已含在 10 号图。
4. P1 资产层（吉祥物/框族网点）依赖 art 挂载，已挂真实 `firmware/main/assets/spiffs_root`。

## 复现方式

照 HOST_TEST_CONVENTIONS.md「跑起来」节的 WSL 单条命令配方（编译 → env 跑 → PIL 转 PNG → cp
回 changes 目录，必须一条 wsl 调用做完）。要点：捕获目录用 `/tmp`（ELF 的 fopen 不认
`/mnt/c` 绝对路径）；不设 `KIDPAL_CAPTURE_DIR` 则纯验证渲染契约返回值、不落盘。

## 实跑记录

见 `runner-run.md`：129 Tests / 3 Failures——3 个失败均为**既有证据文件齿**（VRT-A2 live 冒烟
证据 / WSA-A4 device+live 证据 / WSA-A6 eg 归档，工作树本地证据文件缺失所致），非本次新增；
5 个截图 TEST_CASE 全 PASS，10 枚截图全落盘。

## 纪律留痕

- **跳过流程**：本记录全部改动 = human 明确说「你直接做吧」的一次性授权直做（Hard Rules 例外
  条款），未走 dev-loop（planner→acceptance-author→builder→verifier 链），如实记录于此。
- 渲染缝落盘函数与 `happy_flow_capture.test.c` 均 **additive**：不改任何冻结验收断言；未调用
  hardware_validation 观察的三个 story 符号（以直接设 view 字段驱动状态）；大结构体 static
  守栈纪律。
- host_app `sdkconfig.defaults` 新增 `CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK=y`（仅 host 测试
  工程，设备构建不受影响；改后需删生成的 `sdkconfig` 重配）。
- host_app `main/CMakeLists.txt` 新增 `-Wno-error=comment`（冻结验收件 REVISION LOG 注释携带
  `docs/contracts/**` 路径字面量触发 GCC -Wcomment，冻结件不可动，仅降为非错误）。**附带发现**：
  分支 HEAD（dd61af3）不含此豁免时 host runner **无法 clean build**——主检出 `C:/AI/KidPal`
  自 2026-09-11 的 stale `.o` 把该断层挡住了；若要根因清理冻结件注释文字，需 acceptance-author +
  REVISION LOG，走正常 dev-loop。
