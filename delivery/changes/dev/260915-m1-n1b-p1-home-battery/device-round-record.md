# Device Round Record — 260915-m1-n1b-p1-home-battery（PH-A6 真机确认轮）

> integrator 产出 · 2026-09-16（接棒修复批后刷新）· **状态：PASS（附带条件）——human 已裁**。视觉终验四项全过（human 表态锚 `f6132282`）+ 收尾裁定锚 `45910f60`（充电捕获/断网切换跳过、按 PASS 处理）；附带条件 = 三项挂起观察项入档、标「human 后续观察、非验证通过」，不虚报绿。

## 环境与烧录

- 板：waveshare-s3-rlcd-4.2 真机，human 持板；串口 COM5；烧录口令「现在可以烧录」（human decision `1c42781c`，板就绪 COM5）。
- **烧录四轮**：
  1. **首版**（scoped r2 全绿后整编产物）；
  2. **修复版**（fixer r1 字形缩放补丁 + fixer r2 图标缩放补丁后整编产物）；
  3. **接棒版**（imgfont 大小写编码 bin 52 枚重生成 + 联网小标 font_awesome_14_1 降档后整编产物，scoped r9 后重烧）；
  4. **对齐版**（% 真字形 `imgfont_pct.bin` + 辅助条单行垂直居中 + chrome 顶带同线后整编产物，scoped r10 后重烧）。
- agent 侧烧录 + 被动串口捕获，human 持板执行动作点。捕获日志四份（本目录）：
  - `device-round-boot-capture-20260916.log`（首版，228 行）；
  - `device-round-fix-boot-capture-20260916.log`（修复版，193 行）；
  - `device-round-takeover-boot-capture-20260916.log`（接棒版，227 行；含 `rtc_pcf85063 corr_id=kdp-252 ok rtc=11:24:56 date=01-02`、`battery_inject corr_id=kdp-1222 level=95 charging=0 vbat_ok=1`、`render home 400x300 render_rc=0` + `show_home: LOCAL_PAGE front (layer top=2)`）；
  - `device-round-align-boot-capture-20260916.log`（对齐版，192 行；同径自证——同三条承重行齐备）。

## 已捕获真机证据（PH-A3 端到端实证）

四份捕获日志中同径命中两条承重行：

1. `I (272) adc_battery_estimation: Use external ADC handle` —— 板级外部句柄构造重载生效（`adc_battery_monitor` 死成员 `adc1_handle/cali_handle/vbat_status` 接线成功，基座自建 ADC 路径已被替换）。
2. `I (1222) waveshare_rlcd_4_2: battery_inject corr_id=kdp-1222 level=95 charging=0 vbat_ok=1` —— 板级注入缝端到端：**真电量 95（非恒 100）**、未充电、vbat 有效，correlationId 链路（kdp-1222）完整。`s_battery_pct 恒 100` 现状销账的真机实证。

渲染节点正常起页：`kidpal_rn: render home 400x300 render_rc=0 state_rc=0`、`show_home: LOCAL_PAGE front (layer top=2)`（修复版起四份日志均在册）。

## 已验项（真机证据 + human 看屏确认）

- **PH-A3 端到端**：external ADC handle + `battery_inject level=95 charging=0 vbat_ok=1 corr_id=kdp-1222` + `render home` / `show_home` 正常（上节，四份日志同径）。
- **human 看屏四轮迭代收敛**，最终轮逐字「可以了，整体都可以」（`f6132282`）——单行对齐 ✅ / % 百分号 ✅ / WiFi 同线 ✅ / 整体观感 ✅；湿度 = 100 % 截断边界一并接受不扩区。
- **断网 WiFi 斜杠（开机默认态）可见 ✅**（human 看屏回报 `83180e57` ① 项，开机默认态；联网后主动断网切换未验，见挂起观察项 ②）。
- **"16 Sep" 日期渲染正确 ✅**——含 NTFS 大小写碰撞缺陷的接棒轮发现消账：host 截图绿 ≠ 真机绿的跨端分歧（NTFS 文件名大小写不敏感使大小写 bin 互相覆盖、真机 SPIFFS 大小写敏感缺 e/p 字形），接棒轮裁大小写编码文件名（`imgfont_letter_X` / `imgfont_letter_lc_x`，52 枚重生成）后真机渲染正确。
- **旧电池图标残留消失 ✅**——chrome `battery_label_` 全链摘除（fix-visual 批，human 裁定 `294dafb6` 同批）。

## human 已裁事项

- **PH-E2 降级（低电闪烁真机项）**：PH-A4 真机项按 PH-E2 降级预案执行 = host 机制齿 + 真机读数偏差记录承载，不虚报绿（human decision `4c10c620`）。
- **日期英文 DD Mon**：首页日期改英文格式（如 16 Sep），docs 面不同步（human 自行后续改需求/设计文档，ARQ-260916-home-date-format-english 在册）（human decision `474491c9`）。
- **视觉 PASS 表态**：「可以了，整体都可以」——单行对齐 / % 百分号 / WiFi 同线 / 整体观感四项全过（human decision `f6132282`）。
- **充电捕获/断网切换跳过**：逐字「这两个不好测，跳过吧，我后面观察」——两项主动跳过、留待后续日常观察，该表态按 human 对 PH-A6 的 PASS 裁定处理（human decision `45910f60`）。

## 人裁挂起观察项（标「human 后续观察、非验证通过」，不虚报绿）

1. **charging=1 捕获窗**（充电路径真机读数）：human 逐字「这两个不好测，跳过吧，我后面观察」（`45910f60`）。
2. **联网→断网切换沿**：开机默认态已验 ✅，主动断网切换未验——MAC 隔离方案 human 裁跳过，留待日常观察。
3. **低电闪烁真机可见性**：PH-E2 降级在案（`4c10c620`），由 host 机制齿（PH-A4 两条 mutation 敏感齿）承载。

## PASS 裁定（已填）

```
PH-A6 PASS 裁定：PASS（附带条件）
裁定人：human ｜ 时间：2026-09-16
表态锚：f6132282（视觉四项 PASS 表态）+ 45910f60（收尾裁定——充电捕获/断网切换跳过、按 PASS 处理）
附带条件：上述三项挂起观察项入档、标「human 后续观察、非验证通过」
```

- 附带条件三项已入上节台账；后续 final verify full → VRT-A2 归属 → supervisor 机械闸 → 合并链由 planner 主线程推进（本记录只承载真机证据与 PASS 裁定事实，不替代合并门判据）。
