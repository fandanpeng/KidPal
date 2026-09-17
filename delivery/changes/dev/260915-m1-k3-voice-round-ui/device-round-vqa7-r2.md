# device-round VQ-A7 · 第 2 轮（2026-09-17 human 复测，主线程转述；planner 转写）

板上固件 = 修复版（分支顶 54aa903 构建）。结果：**功能四项全通**——思考态持续显示 ✅（日志实锤 stt→首句 12s 窗口状态稳定）、回显 ✅、播报 ✅、打断 ✅。

## 剩余偏差（一类：各态内容区标识件缺失，渲染面标头，零状态机/逻辑改动）

1. **思考态**：右内容区缺「思考中」标头 + 问号图标（WF-B-02）——现只有右区文字（回显行）。kidpal_voice_state_look 的 look.bubble 应有对应冻结文案（「思考中」），核实单源消费落点；问号图标资产在世先核（art 资产闭集），缺资产则文案标头先行、图标 fail-closed 上报。
2. **聆听态**：内容区缺「我在听！」标识（WF-B-01；VQ-A2 冻结原文「回显在『我在听！』标识下方」）——补标头 + 回显行挂其下方；核上轮回显行在聆听态显隐口径（连续对话时看到上轮回显还是清空，按 §2.2 冻结稿取舍，存疑上报）。
3. **播报态**（WF-B-03）内容区标头按冻结稿核对补齐，同法。
4. **apply_look 的 look.bubble 现写 g_kidpal_chat_label**（聊天 chrome 旧挂点）——核实该对象在新整页版式下可见性，不可见则各态标头统一改挂 g_kidpal_voice_page 右内容区标头位，单源不 duplicates。

## planner 核实事实（intake 只读）

- kidpal_voice_states.c 冻结态：LISTEN bubble =「我在听！」在世；**THINK/TALK bubble = NULL**（注释「问号/气泡文案归 design review，不锁」——human 本次指令即消解该留白；WF-B-03 线框标头字面 =「播报」，思考标头 human 钉 =「思考中」= §2.2 态名口径）。
- g_kidpal_chat_label = chat_message_label_，挂 **bottom_bar_**（lcd_display.cc:958，键帽条内旧挂点）；新整页版式 g_kidpal_voice_page 为 lv_layer_top 全屏 OPA_COVER——bubble 写旧挂点观感上不是右区标头。human 钉方向：统一改挂 voice_page 右内容区标头位。
- **问号图标资产：art_assets.c 闭集无 question/问号资产**（只有 imgfont 数字/字母/冒号/百分号）→ 缺资产，文案标头先行，图标 fail-closed 上报（归后续资产批）。
- §2.2 冻结稿：回显区 = 标识下方逐句回显、会话内滚动超屏滚出；跨会话清零已有齿承载（builder test 2「重开清零」）；会话内播报→再聆听回显累积 = 「逐句回显」直读口径，真机观感兜底，非存疑项。

## 交接

修复轮纪律照旧：host 齿零新增红、新齿按 TEST 6 模式增补；verifier 终验后仍停合并门前；分支内收口 commit 照常（构建烧板需干净分支态），merge 等主线程点火。复测证据落本目录。
