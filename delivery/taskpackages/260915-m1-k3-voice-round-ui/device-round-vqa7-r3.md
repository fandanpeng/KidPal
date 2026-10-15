# device-round VQ-A7 · 第 3 轮派单（2026-09-17 human 拍板「静态盘点先干、真机照片后置精修」；主线程转述，planner 转写）

## A. Chrome 分隔线（两条）

1. 底部键帽条上沿 3px 实线：lcd_display.cc bottom_bar_ 现 border_width=0，补 LV_BORDER_SIDE_TOP 3px 实线（brief §2.3「上沿 3px 实线分隔」原文；模拟器 mock kidpal_ui.c:226 同款画法可参照）。
2. 首页顶部信息栏下方虚线：kidpal_render_node.cc 首页 render 现无分隔线；画法参照 mock kidpal_ui.c:197-204（lv_line + dash_width/dash_gap）。几何锚按 design 稿（docs/ui/kidpal-ui-design-v4.html 首页节）；**设计稿锚行不明确则 fail-closed 上报不猜**。

## B. 气泡尾巴接线（有资产未消费）

bubble_tail_24.bin（KAP1 闭集，frm-sm 圆角框+左下尾巴）在 apply_look 被 (void) 闲置——三态标头（我在听！/思考中/播报）挂进气泡形态消费该资产。红线：v3_restore 齿消费该资产的既有断言零破坏（先读齿再动）；1bpp 阈值观感在设备侧验证，不在 host 假绿。

## C. 缺资产扩闭集（tools/asset-pipeline，冻结母版 docs/ui/kidpal-ui-design-v4.html）

1. 麦克风图标（WF-B-01 🎤）；
2. 问号（WF-B-02 飘问号 ×3）——两路评估：imgfont 闭集加 '?' 字形 vs 独立 KAP1 资产三位置静态摆（动画浮动可后置），取更贴近管线现状者并写明取舍；
3. 声波条（WF-B-03）——评估 KAP1 位图 vs LVGL 原生矩形对象+lv_anim 跳动（后者免管线、动画天然；与「资产闭集」治理不冲突则倾向原生，报告写明）。

art 分区（0x780000/512KB）余量先核，扩集后 gen 管线重跑 + generated_assets.bin/art.bin 随构建落位。

## D. 后置项（本轮不做）

内容区描边卡/框感精修 + 三态元素位置精调 = 等 human 真机照片比对后下一轮；**本轮不新增内容区边框**。

## 交接

host 齿零新增红；可断言处按 TEST 7 模式增补结构齿；verifier 终验后停合并门前汇报；分支内 commit 照常、merge 冻结等主线程点火。
