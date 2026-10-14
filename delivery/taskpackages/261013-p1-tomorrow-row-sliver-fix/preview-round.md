# 预览轮留痕（2026-10-13，主线程记录）

## 性质与授权

human 真机看效果驱动的一次性预览操作——**Hard Rules「主 AI 不直改自有实现面」的 human 明示跳过流程例外**（human 原话「直接调行间距，我来看效果」）。预览轮不落正式实现、不跑验收套件（冻结齿按 28px 旧几何标定，预览轮必红属预期，不作证据消费）；源码改动烧录后即还原，工作树零残留。

## 动作

1. 预览参数集（architect triage 参数集 P 全量）直接改 `firmware/main/display/home_p1.c` `p1_draw_card()`：line_h 28→22、text_yoff 6→4、rows `max(2,·)`→`max(3,·)`、组间虚线 ty+22→ty+8、明天帽标 ty+33→ty+13、明天项行/AC6/AC7 起点 ty+60→ty+43、新增组级容纳门（ty+43+line_h−1 > pv_cy1 → 整区让位）。
2. 构建 + app 分区单烧（0x20000，esptool hash verified），真机给 human 目击。
3. 烧后源码从备份（c:/tmp/home_p1.c.pre-261013b-preview）逐字节还原，`git diff` 核对零残留。

## human 真机裁定（定稿输入）

- 行距 22px：**合意，定稿**。
- 恒定 3 行网格：**不要**——「还是紧凑上浮好」，锚点保留现状 `max(2, s_count)`。

## 正式包输入

定稿参数集 = 参数集 P 的 pitch/组头偏移族（22/4/8/13/43 + 组级容纳门）**减去** `max(3,·)` 改动（保留 `max(2,·)` 紧凑上浮）。锚点形态变化使 architect 原 reviewed plan 的容量表与两个冻结验收文件的齿推导表需按 `max(2,·)` 重推——architect 修订后正式落盘。
