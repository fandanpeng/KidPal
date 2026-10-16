# mutation 取证 · 261015-p1-auxbar-style-fix（builder 第一方实跑，planner 逐字代存）

来源：builder 交付回执（agentId ab3b212e5e393aa9f，2026-10-15）§③ 逐字转存；驱动脚本 C:\tmp\pas_mutation_check.sh（单次调用内 sed 变异 + 编译 + 实跑 + 还原）；每族日志 C:\tmp\p1_auxbar_style_mutation_ab_a{1,2,3,5}.log。

## m1 · AB-A1（ic_batt_15.bin → ic_batt_48.bin）

红名 verbatim：
`AB-A1 RED: home_p1.c p1_draw_aux_batt 未取到 ic_batt_15.bin 消费点（15x12 原生直出件未接入）——电池图标仍走 ic_batt_48 48x48 塞 22x40 非等比盒旧路径（横压 2.18x + y0=0 clip 硬切, 全条最重最黑）; 修复 = 消费切换 ic_batt_48.bin -> ic_batt_15.bin 原生 15x12 直出不缩放`

RUNNER_EXIT=1（该族单红、其余五 case 绿）；还原后 live 名计数回位（ic_batt_15.bin=1、ic_batt_48.bin=0）。日志：C:\tmp\p1_auxbar_style_mutation_ab_a1.log

## m2 · AB-A2（ic_thermo_12.bin → thermo_man.bin）

红名 verbatim：
`AB-A2 RED: home_p1.c p1_draw_aux_temp 未取到 ic_thermo_12.bin 消费点（12x12 原生直出件未接入）——温度图标仍走 thermo_man 41x67 最近邻硬压 20x20 旧路径（五官细节全毁）; 修复 = 消费切换 thermo_man.bin -> ic_thermo_12.bin 原生 12x12 直出不缩放`

RUNNER_EXIT=1；还原复绿。日志：C:\tmp\p1_auxbar_style_mutation_ab_a2.log

## m3 · AB-A3（ic_drop_12.bin → drop_baby.bin）

红名 verbatim：
`AB-A3 RED: home_p1.c p1_draw_aux_hum 未取到 ic_drop_12.bin 消费点（12x12 原生直出件未接入）——湿度图标仍走 drop_baby 32x51 最近邻硬压 20x20 旧路径（五官细节全毁）; 修复 = 消费切换 drop_baby.bin -> ic_drop_12.bin 原生 12x12 直出不缩放`

RUNNER_EXIT=1；还原复绿。日志：C:\tmp\p1_auxbar_style_mutation_ab_a3.log

## m4 · AB-A5（"JAN" → "Jan"）

红名 verbatim：
`AB-A5 RED: k_p1_month_abbr 月简写表串含小写字母（首犯 "Jan"）——日期月/星期简写仍为 title-case, 12x16 降采样下小写笔画侵蚀与大小写混排参差在世（Tier-2④a human 触点①拍板「直接做」; 修复 = 表串全大写 JAN..DEC, 零新资产 imgfont_letter_A..Z 全套在库）`

RUNNER_EXIT=1；还原复绿。日志：C:\tmp\p1_auxbar_style_mutation_ab_a5.log

## 收尾绿

四族跑毕还原后复跑：`6 Tests 0 Failures / FINAL_RUNNER_EXIT=0`；源码 git diff 无旧消费名残留（销账 grep 净）。

## 豁免面（不取证）

AB-A4（同名同尺寸重出，无独立源码变异点——守卫咬合 job 同行锚/母版 symbol id/消费几何三锚在世）、AB-A6（负面锚零触碰面，被触碰时报 RED(infra)）、AB-A7（真机轮 live-manual structural 豁免）——口径冻结于 p1_auxbar_style.acceptance.c 头注与 taskpackage mutationEvidence.reason。
