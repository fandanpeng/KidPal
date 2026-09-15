# SunU 红基线判定摘要（acceptance-author 2026-09-11 实测）

TaskPackage `260911-screen-unification`（PKG-2 归一屏，workstream
260911-display-interaction-sovereignty 核心包）冻结验收文件
`firmware/main/test/screen_unification.acceptance.c` 的红基线执行记录。

## 红基线复核结论（intake 复核，非重跑全量 host build）

现状仍真（2026-09-11 acceptance-author 以工作树实测复确认，行号 = 生成时点）：

- `firmware/main/display/kidpal_render_node.cc` L58/L63/L75 三标志在位：
  `s_chat_overlay`（L58）/ `s_page_pinned`（L63）/ `s_current_page`（L75）；
- 1s tick 对账活跃路径在位：`RN_TICK_MS = 1000`（L88）+ timer 回调
  `GetDeviceState()`（L173）+ pin/voice 对账（L181/L193/L204-215）；
- `s_chat_scr` 独立屏在位（L54，`lv_screen_load(s_chat_scr)` L205/L315）；
- char 独立屏在位：`char_page.cc` L418 `g_page.scr = lv_obj_create(NULL)` +
  L420 `lv_screen_load(g_page.scr)`；
- `firmware/main` 零 `kidpal_layer_*` 模块（frozen contract 未实现）。

**结论：三标志退役机制齿 + char 并屏齿 + tick 对账轮询移除齿在基线上必红
（机制尚未建），结构性红基线成立。** 无一红因是链接/编译错误（BUILD_CLEAN 佐证，
见下）。

## C 侧红基线（命令与实跑）

冻结验收文件的闸上 runner 判据 = host_app linux-target ELF（[C4.4]），接线属
builder。本次按 WSA/CHR 先例用边界内脚手架（本目录 `unity.h` stub + `runner.c`，
非冻结件）把**同一份冻结验收文件**在 WSL Ubuntu-24.04 编成原生 Linux ELF 当场执行：

```
cd /mnt/c/AI/KidPal-pod-screen-unification/firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /tmp/sunu_red_runner \
    ../../screen_unification.acceptance.c runner.c
        # BUILD_CLEAN（error 0、warning 0）——T4 C 类冻结验收实编通道过
cd /mnt/c/AI/KidPal-pod-screen-unification
/tmp/sunu_red_runner
```

实测：**7 Tests 6 Failures**（runner 自报计数；`bash -c` 包装下 `echo $?` 恒 0
是 wsl.exe 参数重组伪影，VRT/CHR 摘要已记录同款，退出码按 runner 自报失败数 6 记）。

判读要点：行为齿经 weak-symbol frozen offline contract 真执行（6 个契约函数
`kidpal_layer_push / pop / top / has / chat_target / chat_clear_event`，weak 默认
哨兵 -2，断言先打 NOT-sentinel）——红 = 「frozen contract not implemented」缺行为
红 + 符号级残留/结构缺失红，非链接/编译红。

### 逐条判定

| TEST_CASE | 判定 | 红因（各自真实缺失行为/结构） |
|---|---|---|
| layer 前台优先级表进出序 + 单一权威 (SU-A2/A4) | **RED** | `kidpal_layer_top` 未实现（weak 哨兵 -2）——前台归属优先级表单一权威未落成（三散标志的仲裁器内核缺失） |
| 聊天 layer 落点 + 驻留清除时机 (SU-A1/A5) | **RED** | `kidpal_layer_chat_target` 未实现——聊天内容落点 + 驻留清除时机未定义（OQ3=c 清除时机内核缺失） |
| 语音轮→回答驻留→回首页 前台归属 (SU-A5/A7) | **RED** | `kidpal_layer_push` 未实现（weak 哨兵 -2，首断言 `push(VOICE)` 即红）——KEY 长按=回首页的显示归属面未能经 layer 权威到达 target |
| 三标志 + tick 对账 + s_chat_scr 符号级零残留 (SU-A2) | **RED** | `s_chat_overlay` 符号级残留（1 文件 kidpal_render_node.cc）；后续 `s_page_pinned`/`s_current_page`/`s_chat_scr`/tick 对账四齿同群必红——三散标志+独立聊天屏+轮询对账在位 |
| layer 模块在树 + 设备构建 + host 接线 + 消费者 (SU-A1) | **RED** | `kidpal_layer_top` 0 文件引用（模块零树）、未编入设备 CMakeLists、无生产消费者、本文件未接线进 HOST_ACCEPTANCE_SRCS |
| char 页并入 s_kidpal_scr——display/ 单屏不变式 (SU-A3) | **RED** | `display/` 有 2 个顶层屏创建点 `lv_obj_create(NULL)`（kidpal_render_node.cc 的 s_kidpal_scr + char_page.cc 的 g_page.scr），须 == 1；`g_page.scr` 独立屏句柄残留 |
| 冻结面零越界 + 扫描器自检 + 直连守卫 (SU-E3/boundary) | **GREEN**（守卫, 必须保持绿） | 契约/spec canary 在世；firmware 无 openclaw/18789 直连；扫描器四态自检 + 计数器通过 |

## 上游在册红口径（如实记录，非本包责任，不得标成本包红、不得修）

host ELF 基线 @ main d087fb7 = **104 Tests / 3 Failures**。这三条是**其他批在册
红**，与本包 host-teeth 面无关：

| 在册红 | owner | 说明 |
|---|---|---|
| WSA-A6:830 | 260903-device-ws-switch 证据齿 | 本地-only 证据齿，从未入仓（非本包） |
| WSA-A4:663 | 260903-device-ws-switch 证据齿 | 同上，本地-only 证据齿 |
| P2-A1:571 | 批次③ 260911-render-pages | KSP-A1 first-match 歧义（渲染批 owner，本包后并） |

builder/verifier 处置纪律：这三条红在 host 矩阵里持续可见属**正常基线**，本包 merge
前后均不负责转绿；不得在本包证据里把它们标成「本包引入的红」、不得尝试修补（越界）。

## 红的构成一句话

**红 = 6 齿对当前 story 缺失行为/结构成立（layer 管理仲裁器内核未实现 = weak 哨兵；
三散标志 `s_chat_overlay/s_page_pinned/s_current_page` + 独立聊天屏 `s_chat_scr` +
1s tick 对账轮询符号级残留；char 独立屏在位致 display/ 双屏创建点；模块零树未接线），
SU-E3/boundary 守卫为 green。** 无一红因是 import/collection/编译错误（BUILD_CLEAN、
runner 自报 7 Tests 6 Failures，断言全落在缺失行为/结构上）。本包把它切到绿。

## 冻结契约给 builder 的输入（唯一被迫接受面）

落点 `firmware/main/display/kidpal_layer.(c|h)`（C linkage、零芯片依赖；若落 .cc 需
`extern "C"`）。类型/数值码/语义见验收文件头「FROZEN OFFLINE CONTRACT」，头部
`__has_include("kidpal_layer.h")` 在实现头落地后自动切到真实声明（单一事实源）。要点：

- 类型 `kidpal_layer_stack_t { int layers[8]; int n; }`。
- 数值码：`NONE=0 / CHAT=1 / LOCAL_PAGE=2 / SERVER_PAGE=3 / VOICE=4 / REMINDER=5 /
  PROVISION=6`（前台优先级 = 数值越大越靠前）；事件码 `ANSWER_END=0 / PAGE_SWITCH=1 /
  NEW_VOICE=2 / KEY_PRESS=3`。
- 语义：push 幂等去重、top = 在场最高优先级、pop 指定层、has 在场判定；chat_target
  恒 = CHAT；chat_clear_event 按事件表 0/1、未知事件保守 0。
- 接线（builder duty, scope 不属 acceptance-author）：HOST_ACCEPTANCE_SRCS 加
  `../../screen_unification.acceptance.c`（排 smoke 之前）+ 落地后
  `../../../display/kidpal_layer.c` + include dir `../../../display`；设备构建
  `firmware/main/CMakeLists.txt` 编入 `kidpal_layer.c`。

## 降级与留痕记录（诚实边界）

1. **闸 runner 未接线**（scope 拦，接线属 builder）：C 红以 stub runner 执行同一文件
   得出，断言行为与闸上一致；builder 接线 + 实现落地后 verifier 须在 host_app ELF
   复跑（转绿期望 = 全 TEST_CASE PASS + EXIT=0 + 强符号覆盖 weak 哨兵）。
2. **观感边界（SU-E2）**：字体子集/换肤/排版归批次③ 260911-render-pages（mergeOrder
   21 本包后），本文件零视觉排版断言，只钉结构/归属。
3. **char_stroke 冻结齿守绿**：SU-A3「char_stroke 冻结齿复跑绿」属 SU-A6 矩阵回归
   动作（既有冻结件，非本文件新齿）；本文件只钉「char 页并入 s_kidpal_scr 层机体
   系、无独立屏」，不重写 char_stroke.acceptance.c。
4. **真机面（SU-A8）**：语音轮/推送页/提醒并发 0 复位 0 崩溃 = human 持板，证据落
   `delivery/changes/dev/260911-screen-unification/`，非 host 齿、本文件不冒充真机已验。

---

## 优先级语义修订（SU-RULING-01 方案A）后红基线复核（acceptance-author 2026-09-11 二次实编实测）

冻结验收件 `firmware/main/test/screen_unification.acceptance.c` 同日执行前台优先级
语义修订（SERVER_PAGE/VOICE 数值码 3↔4，回答期间服务端推送页前台可见、语音不拦内容
页；actor="human-deputy" 代裁，裁断出处
`delivery/changes/arch/260908-gov-review-human-deputy-shadow/deputy-shadow.md#SU-RULING-01`，
对齐 PROPOSAL §0 L26-27 / §5 L152-153），修订后复跑 C 类冻结实编冒烟 + 红基线：

```
cd /mnt/c/AI/KidPal-pod-screen-unification/firmware/main/test/acceptance/red-baseline
gcc -std=c11 -Wall -Wextra -I. -o /tmp/sunu_red_runner \
    ../../screen_unification.acceptance.c runner.c
        # BUILD_CLEAN（error 0、warning 0）——T4 C 类冻结验收实编通
cd /mnt/c/AI/KidPal-pod-screen-unification
/tmp/sunu_red_runner
```

实测：**7 Tests 6 Failures**（runner 自报）。优先级语义修订不改变任一齿的红/绿判据
——**结构齿仍红、行为齿仍红在哨兵**，s4 边界守卫仍 GREEN。

### 复核逐条判定

| TEST_CASE | 判定 | 红因（真实缺失行为/结构） |
|---|---|---|
| layer 前台优先级表进出序 + 单一权威 (SU-A2/A4) | **RED** | `kidpal_layer_top` 未实现（weak 哨兵 -2）——新码序「配网(6) > 提醒(5) > 服务端页(4) > 语音(3) > 本地页(2)」前台归属优先级表单一权威未落成，红在哨兵 |
| 聊天 layer 落点 + 驻留清除时机 (SU-A1/A5) | **RED** | `kidpal_layer_chat_target` 未实现（weak 哨兵 -2），不变 |
| 语音轮→回答驻留→回首页 前台归属 (SU-A5/A7) | **RED** | `kidpal_layer_push` 未实现（weak 哨兵 -2，首断言 `push(VOICE)` 即红）——修订后的「回答期间 push SERVER_PAGE 后 top==SERVER_PAGE」断言仍未执行到，红在哨兵不退为编译/链接错 |
| 三标志 + tick 对账 + s_chat_scr 符号级零残留 (SU-A2) | **RED** | `s_chat_overlay` 符号级残留（1 文件 kidpal_render_node.cc）；三散标志 + 独立聊天屏 + 1s tick 对账在位 |
| layer 模块在树 + 设备构建 + host 接线 + 消费者 (SU-A1) | **RED** | `kidpal_layer_top` 零文件引用（模块零树），未编入设备 CMakeLists / host HOST_ACCEPTANCE_SRCS、无生产消费者 |
| char 页并入 s_kidpal_scr——display/ 单屏不变式 (SU-A3) | **RED** | `display/` 2 个顶层屏创建点（s_kidpal_scr + char 独立屏 g_page.scr），须 == 1 |
| 冻结面零越界 + 扫描器自检 + 直连守卫 (SU-E3/boundary) | **GREEN**（守卫, 必须保持绿） | 契约/spec canary 在世；firmware 无 openclaw/18789 直连；扫描器四态自检 + 计数器通过 |

### 修订对红基线的净影响

- b1：数值码 ordering 守卫、`push(SERVER_PAGE)` 后 `top()==SERVER_PAGE`、pop 回落链
  （pop REMINDER→SERVER_PAGE / pop VOICE→SERVER_PAGE）按新码序重钉；红因 = weak 哨兵
  -2（frozen contract 未实现），与修订点无关，红基线性状不变。
- b3：`push(SERVER_PAGE)` 从「top==VOICE」改为「top==SERVER_PAGE」，「语音结束
  pop VOICE 后 top==SERVER_PAGE」驻留语义维持自洽；尾部「手动翻页/回首页」的
  LOCAL_PAGE 前台断言修正为与 frozen 优先级表（LOCAL_PAGE 最低）自洽的「push 基层 +
  pop 服务端页」达前形式（原「push(LOCAL_PAGE) 即前台」在 SERVER_PAGE 在场时与优先级
  表矛盾 = 测试错误红，已在 REVISION LOG 记 tighten，非语义放松）。b3 红因 = weak 哨兵
  -2，未执行到这些新断言，红基线性状不变。
- s1/s2/s3 结构齿、s4 边界守卫、b2 聊天落点/驻留清除时机：零改动，判据不变。

一句话：**优先级语义修订后红基线复核 = 结构齿（s1/s2/s3）仍红、行为齿（b1/b2/b3）
仍红在 weak 哨兵、s4 守卫仍绿；C 类实编 0 error 0 warning；无一红是编译/链接错误。**

---

## 单物理屏收紧（SU-RULING-02 方案1, human-deputy 代裁）后 C 类实编冒烟 + 红基线复核（acceptance-author 2026-09-11 三次实编实测）

**修订性质**：本修订 = tighten 向设计权威对齐（SU-RULING-02 方案1；actor="human-deputy",
代裁, deviation=false, 禁止记成 human 亲裁, canon C-14；裁断出处
`delivery/changes/arch/260908-gov-review-human-deputy-shadow/deputy-shadow.md#SU-RULING-02`）。
真归一为单物理屏、物理 reparent 留 PKG-2 包内随包交付。收紧「除 s_kidpal_scr 外，
display/ 不得存在第二个前台屏载体」不变式，封死 builder 首轮两处符号级零残留齿被绕过
的路径：①捕获改名（旧 `s_chat_scr` 改名 `s_voice_ui_scr`，s1 只扫旧名漏判）；②非创建点
（`s_voice_ui_scr` 靠 `lv_screen_active()` 捕获而来、非 `lv_obj_create(NULL)` 创建点，
s3 只数创建点漏判）。

**新增断言（纯加法，不放松任何既有齿）**：
- s1 补钉：`sunu_scan_count("firmware/main", "s_voice_ui_scr") == 0`（第二屏载体变量零残留，
  封死捕获改名）。现状 = 1 文件（kidpal_render_node.cc）→ **必红**。
- s3 补钉：`display/` 下 `lv_screen_load(` 出现次数 == `lv_screen_load(s_kidpal_scr)` 出现次数
  （且后者 >= 1）——每次屏加载实参恒为 s_kidpal_scr，封死非创建点捕获屏被切前。现状 =
  9 次 `lv_screen_load(` vs 8 次 `lv_screen_load(s_kidpal_scr)`（kidpal_render_node.cc L170
  `lv_screen_load(s_voice_ui_scr)`）→ **必红**。
- **方法论注记（据代码实际加强，非偏离裁断）**：裁断原建议「display/ 下 `= lv_screen_active()`
  赋值捕获计数 == 0」按字面落地会把 char_page.cc L421 / schedule_page.c L710 / story_page.c
  L1132 / reminder_page.c L1128 / oled·lvgl·lcd_display.cc 的多处「取活动屏绘制句柄」合法用法
  判红（测试错误红，违反红因真实纪律）。故把「赋值捕获归零」精确化为「第二屏载体加载归零」——
  `lv_screen_load(` 是「前台屏载体被切前」的唯一语义锚，按出现次数相等判定，既封死两条绕过
  路径的通型（不依赖变量名），又不误伤合法活动屏绘制。s3 既有「display/ 单处
  lv_obj_create(NULL)」与「g_page.scr 零残留」两齿原样不动（对创建型第二屏 + char 独立屏句柄
  仍真）。

**C 类冻结实编冒烟（T4/§4 实编通道）**：两个 build 均 BUILD_OK（`gcc -std=c11 -Wall -Wextra`，
0 error / 0 warning）。

**红基线复跑结果**（runner 自报计数；`echo $?` 恒 0 = wsl.exe 参数重组伪影，VRT/CHR 摘要已记）：

| 协议 | 命令 | 结果 |
|---|---|---|
| stub-only（历史 canonical，弱哨兵） | `gcc -std=c11 -Wall -Wextra -I. -o /tmp/sunu_red_runner ../../screen_unification.acceptance.c runner.c` | **7 Tests 5 Failures**：b1/b2/b3 红在 weak 哨兵（未链 kidpal_layer.c）+ s1/s3 红在新收紧齿 |
| **链真实现（本修订权威基线）** | 加 `-I../../../display ../../../display/kidpal_layer.c` | **7 Tests 2 Failures**：b1/b2/b3 **GREEN**（kidpal_layer.c 强符号覆盖 weak 哨兵, frozen contract 已实现）、s2 GREEN、s4 GREEN；**仅 s1/s3 红在新收紧的单物理屏齿** |

**判读（链真实现视图 = 本修订红基线权威口径）**：
- b1/b2/b3 已转绿（真实现 kidpal_layer.c 落盘后 frozen offline contract 行为侧兑现；stub-only
  下仍红在 weak 哨兵是「未链实现」的协议性红, 非缺行为）。
- s2 已绿（模块在树 kidpal_layer.h/.c + device CMakeLists 编入 + host_app HOST_ACCEPTANCE_SRCS
  接线序守卫 + 生产消费者 wiring 全部就位）。
- s3 既有「display/ 单处 lv_obj_create(NULL) == 1」「g_page.scr == 0」已转绿（builder 首轮已把
  char 页并入 s_kidpal_scr），但新增「第二屏载体加载清零」齿红——display/ 仍有一个非
  s_kidpal_scr 的第二前台屏载体 s_voice_ui_scr 被切前。
- **红基线主角 = 新收紧的单物理屏齿**：s1（s_voice_ui_scr 符号残留）与 s3（9 vs 8 屏加载）两齿
  在 builder 现状交付下必红，且红因真实（第二屏载体在位、捕获改名/非创建点事实存在），
  非编译/链接错误，非测试错误红。

**一句话**：本轮收紧把 builder 首轮「改名 s_voice_ui_scr + 非创建点捕获」两条绕过路径全部封死
——链真实现后 b1/b2/b3/s2/s4 全绿，红收敛到恰好两处真实第二屏载体残留（s1/s3 新收紧齿），
红基线成立。