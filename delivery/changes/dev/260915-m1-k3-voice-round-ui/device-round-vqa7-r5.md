# device-round VQ-A7 · 第 5 轮派单（2026-09-17 human r4 复测 + 主页面照片定谳；主线程转述，planner 转写）

## 流程红线（先行）

① human 反馈有歧义或与冻结设计锚冲突时 **fail-closed 澄清，禁靠猜改锚**（上轮虚线猜错教训，pod-retro 在案）；
② **1bpp 资产禁运行时 lv_image_set_scale 缩放**（抗锯齿稀释笔画、设备 1bpp 阈值下隐身）——尺寸变化一律走 tools/asset-pipeline 重出目标尺寸资产。

## 任务

1. **顶部虚线复位**：home_p1.c p1_draw_aux_dash_sep 恢复 r3 锚 = 顶部信息栏下方 y42、通栏（左右 6px 内缩），撤销 r4 的 y246 / x138-389 半宽落点及注释（r4 该次重定位是对 human r3 反馈的误读，注释写明销账）；dash/dot needle 保留（TEST 8 tooth 2 不变绿）。
2. **mic/question 资产重出**：asset-pipeline 重出 16px 锐利 1bpp 资产各一（gen 管线出图，非运行时缩放）；聆听 mic 挂「我在听！」后、思考问号单枚挂「思考中」后，尺寸 ≈ font_puhui_14_1 行高（~14-16px）；r4 的 lv_image_set_scale / LV_IMAGE_ALIGN_TOP_LEFT / 16px widget 收紧写点全撤；标头下布局不变。
3. **键帽 + 键槽样式**（human 拍板：本轮不动 bottom_bar_ 高度 48px 设计值，先看键帽效果）：按 brief §2.3 补键帽胶囊——高 26px / 圆角 8px / 3px 描边 / 白底黑字 / KEY·BOOT 字样用圆胖数字体（mock kidpal_ui.c keycap_at() 同款，设备侧字体按在世全量字体族取最近口径，取舍报告写明）；左右槽文案与页点语义零变化（kidpal_keybar_slots 单源不动）；r3 上沿 3px 实线写点保留。
4. **中部实线定谳修复**（照片证据到位，先取证再改）：现象 = 四主页面逻辑 y≈163 全宽实线；键槽上沿 y252 反而无实线；语音轮弹框页无此线。头号嫌疑：bottom_bar_（LV_SIZE_CONTENT + min_height 48 + 子件 chat_message_label_ 宽 368 WRAP 居中）被内容撑高 → 上沿 3px 实线随顶边跑到中部；槽内左右槽/页点 BOTTOM 对齐仍在底部；弹框页 g_kidpal_voice_page（后建全屏不透明）盖住它解释「弹框页正常」。修法 = 取证实锤后限高：bottom_bar_ height 钉 48 + 内容溢出裁剪（或 chat_message_label_ 限行/限高），**禁别的猜测改法**。取证：串口打印 lv_obj_get_height(bottom_bar_) 与 chat_message_label_ 行数实锤落报告；嫌疑证伪则次候选 = 软帧 p1_stroke_rect(138,50,390,166) 底边（y166≈163 但解释不了全宽与左段），排除或证实都留痕。
5. **首页主体空渲染核查**（照片新发现）：照片显示首页大时间块/卡片框全缺（只有辅助条+猫+两条线+槽文字）。先核数据态：无日程/doze 态下软帧本就该空 → 报告说明即闭；数据在而没画才是 bug，修复走最小改动并单独列针。

## 验收

host 齿零新增红（TEST 8 结构针随几何改同步校准，禁为绿改针）；新资产过管线校验；verifier scoped 复验 + final 全闸；完成后停合并门前汇报，主线程构建烧板，human 看过再点火集成。
