---
changeId: 261015-p1-auxbar-style-fix
rigor: lite
riskTier: low
sourceType: human-ad-hoc
reviewed-by: architect
date: 2026-10-15
---

# P1 辅助条像素级观感修复

> 落盘说明（主线程，触点①后）：本文 = architect reviewed plan 原样搬运；「human 触点①决议」节为主线程追加的裁决留痕，plan 正文未改。

## 建什么

human 原话：「首页 最上面那一条，整体看起来很不精致」。首页 P1 辅助条（y0-39 状态栏带）四个观感问题修复：

1. **wifi 图标粘连**：ic_wifi_14.bin 母版 SVG（docs/ui/kidpal-ui-design-v5.html:394，48x48 双弧 stroke 4.5）栅格化到 14x14 后双弧间隙闭合、粘连成实心扇面，读作「两朵 wifi 重叠」。
2. **电池图标失真**：home_p1.c:1104 把 ic_batt_48（48x48）塞进 22x40 非等比盒（横压 2.18x）+ y0=0 clip 硬切，实测墨迹 19x19，全条最重最黑（文字统一 11px 高）。
3. **温/湿度插画走样**：thermo_man / drop_baby 拟人插画最近邻硬压 20x20（home_p1.c:1040/:1070），五官细节全毁；管线已有小尺寸直出件 ic_thermo_12 / ic_drop_12 未被消费。
4. **日期观感 + 冒号眨眼**：48x64→12x16 四倍降采样下小写字母笔画侵蚀、大小写混排参差；时间冒号每 4.4s 眨眼（灭 1.1s），静态照片呈「18 14」。

实测基线（设备同款参数复现）：文字元素统一 y14-24（h=11，中线 y≈19）；wifi 10x8；temp_ic 13x18；hum_ic 19x18；batt_ic 19x19。

## 方案（分层：Tier-1 纯样式直接做 / Tier-2 human 拍板后才做）

### Tier-1（纯样式，无 spec/契约触点，builder 直接实施）

**① 电池图标换小尺寸直出件**——home_p1.c p1_draw_aux_batt：`ic_batt_48.bin` → `ic_batt_15.bin`（15x12 原生，已在 spiffs_root，仅切换消费、不重出）。默认以**原生尺寸直出不缩放**（p1_blit_art 或 dw/dh=原生），在图标半区 {342,6,362,33} 内居中、墨带以中线 y≈19 落座（与文字 11px 高同视觉级）；弃用 22x40 非等比盒。art 未挂载兜底描边保持在该钉区内（HPB 兜底义务，p1_home_battery.acceptance.c:101-104）。

**② 温/湿图标换小尺寸直出件**——p1_draw_aux_temp / p1_draw_aux_hum：`thermo_man.bin` → `ic_thermo_12.bin`、`drop_baby.bin` → `ic_drop_12.bin`（均 12x12 原生、已在库）。落位同 ①（原生直出、图标半区 {210,6,230,33} / {282,6,302,33} 内居中、中线 y≈19）。兜底同纪律。
（备选、默认不取：若 builder 实测原生 12-15px 显小气，可评 20x20 目标尺寸重出——但那要扩 gen_art_spiffs 68 枚闭集白名单，面变大，需先回评，不擅自扩。）

**③ wifi 资产按 14px 目标重设**——母版 ic-wifi symbol（docs/ui/kidpal-ui-design-v5.html:394）按 14px 原生可读性重设（加粗/拉开双弧间隙，或简化为单弧+点；builder 出 2-3 候选经 host screenshot 自查比对，终形归看屏轮 human 拍板）；gen_assets.mjs ic-wifi job（:286）重出 ic_wifi_14.bin——**同名同尺寸 14x14，白名单不动、验收零重锚**。设计母版内容级同步随包做（261011 facePlan 顺序4 先例）。

### Tier-2（观感裁决项，human 触点①或看屏轮拍板后才做；不拍 = 维持现状）

**④a 日期全大写**——k_p1_month_abbr / k_p1_wday_abbr 简写表改全大写（"Tue 22 Sep" → "TUE 22 SEP"），规避小写字母降采样侵蚀 + 大小写混排参差。字形零新资产（imgfont_letter_A..Z 全套实测已在 spiffs_root）。撞点：AC1 冻结示例「Tue 22 Sep」（requirements.md:226）与 2026-09-26 对齐轮 title-case 记录——**拍板后**需求面措辞同步走档 0 勘误 / human 直改如实记录（v1.17 先例），殿后项承载。

**④b 冒号眨眼改常亮（或降频）**——p1_draw_aux_time（home_p1.c:917-931）colon_visible 门控改常亮（HP1_BLINK_PERIOD_COLON_MS 常量退役）或降频改周期常量，二选一由 human 拍。**不动**：blink_visible 函数本体与 1s 电池闪（HPB-A4 活齿 :1047-1056 + p1_cjk_name:1342 符号钉 + time_sync tooth6 :1149-1155——三处都要求函数与装配调用在世）。撞点：requirements.md:564 趣味性「眨眼冒号」示例句 + 归档 P1-A2 4.4s 冻结语义——同 ④a 拍板纪律。

## human 触点①决议（2026-10-15，主线程 AskUserQuestion 留痕）

1. **路由**：② 单包 Lite（rigor=lite / riskTier=low / sourceType=human-ad-hoc）——human 原话「最好一个lite包搞定」确认，architect 独立判断同判。
2. **Tier-2④a 日期全大写**：**拍板「直接做」**——纳入本包 builder 实施范围（与 Tier-1 同批）；需求面 AC1 示例措辞同步留殿后项。
3. **Tier-2④b 冒号眨眼**：**拍板「不做、维持眨眼」**（human：「那是时间，咋能不变呢」——眨眼即时钟跳动语义，保留）——④b 整项出包，home_p1.c:917-931 冒号门控不动。

## 影响面

- **代码**：firmware/main/display/home_p1.c（p1_draw_aux_batt / p1_draw_aux_temp / p1_draw_aux_hum / p1_draw_wifi_icon；拍板项 + p1_draw_aux_date / p1_draw_aux_time）。
- **资产**：firmware/main/assets/spiffs_root/ic_wifi_14.bin 重出（同名同尺寸）；ic_batt_15 / ic_thermo_12 / ic_drop_12 仅切换消费。
- **管线**：tools/asset-pipeline/gen_assets.mjs（ic-wifi job）；gen_art_spiffs.mjs 68 枚白名单不动。
- **设计母版**：docs/ui/kidpal-ui-design-v5.html（ic-wifi symbol + 辅助条注解内容级同步）。
- **工具**：firmware/main/test/host_app/screenshot/home_p1_screenshot.c:261 ic_batt_48 探针随消费切换更新。
- **验收测试**：零重锚（PH-A2 齿全为 ink 下界钉，区域不变；归档件不动）。
- **契约 / server / 需求面**：零改动（Tier-2 拍板后的措辞同步属殿后项，不入本包实现面）。
- **烧录**：spiffs 资产变更 → app+art 双分区烧录（261011 先例）。

## 验收口径

- host 全量不回退（155T0F 基线；PH-A2 / HPB-A4 / p1_cjk_name / time_sync tooth6 全绿）。
- 资产面：verify_art_assets.mjs 绿 + host screenshot 自查具重出辅助条逐元素墨带 bbox（修复前后对比留证）。
- 真机看屏轮 human 持板（TR5 同款 live-manual，观感件归真机探真）：① wifi 不再读作「两朵重叠」；② 电池/温/湿图标与文字同视觉级（墨高约 11-15px、中线 y≈19、无硬切感）；③ Tier-2 若拍板，新日期/冒号观感确认。

## 殿后项

- Tier-2④a 拍板后置项：requirements.md AC1 示例措辞（"Tue 22 Sep" → 全大写示例）同步——档 0 勘误或 human 直改如实记录（v1.17 先例）；决策留痕。
- home_p1.c 内过时注释顺手修（如 :1037「thermo_man 41x67」等尺寸描述随资产切换失实）。
- 联网小标条件显隐（hp1_state_t 增网络字段）维持登记遗留、本包不碰（home_p1.c:1123 注记）。

## 关键取证文件（绝对路径）

- c:\AI\KidPal-pod-m2\firmware\main\display\home_p1.c（:1040/:1070/:1104/:1127 四绘制点；:917-931 冒号门控——④b 不做故不触碰；:1123 联网字段遗留注记）
- c:\AI\KidPal-pod-m2\firmware\main\test\p1_home_battery.acceptance.c（:60-73 钉区表；:101-104 兜底义务；:1090-1134 PH-A2 下界齿；:1047-1056 HPB-A4 1s 电池闪活齿；:119-127 TR5 看屏豁免先例）
- c:\AI\KidPal-pod-m2\firmware\main\test\acceptance\archive\home_p1.acceptance.c（:239/:1003-1039 归档 4.4s 冒号相位齿，不在跑）
- c:\AI\KidPal-pod-m2\firmware\main\test\p1_cjk_name.acceptance.c:1342（blink_visible 符号钉）、c:\AI\KidPal-pod-m2\firmware\main\test\time_sync.acceptance.c:1149-1155（p1_draw_aux_time 结构齿）
- c:\AI\KidPal-pod-m2\tools\asset-pipeline\gen_art_spiffs.mjs:35-67（68 枚闭集白名单，ic_thermo_12/ic_drop_12/ic_batt_15 在册 :47）
- c:\AI\KidPal-pod-m2\tools\asset-pipeline\gen_assets.mjs:286（ic_wifi_14 job）
- c:\AI\KidPal-pod-m2\docs\requirements.md:226（AC1 冻结示例）、:564（趣味性眨眼冒号示例句）
- c:\AI\KidPal-pod-m2\delivery\changes\intake\261011-p1-home-v5-rework.md（status: closed；app/art 双分区烧录先例）
- c:\AI\KidPal-pod-m2\docs\contracts\interface-contract.md（全文资产面零命中，NO_CONTRACT_CHANGE 实证）
