/*
 * Frozen acceptance tests — TaskPackage 260829-story-p3
 * (subsystem: firmware+server; 包 8「故事：列表 + 录制上传归档 + 回看/回听 + 成就
 *  盖章」设备可观察面: P3 故事页 story_list 渲染/光标导航/录音态演出/回看回听/盖章帧;
 *  mergeOrder 10; dependsOn 260829-host-test-infra + 260829-voice-roundtrip;
 *  riskTier high; testHost host+PC; 云侧孪生 = server/acceptance/test_story_p3.acceptance.py).
 *
 * Nature: host-runnable STRUCTURAL teeth (device_ws_switch / voice_roundtrip_b /
 * schedule_p2 tree-scan precedent) PLUS a frozen pure-logic offline contract for
 * the P3 故事页消费面 (voice_roundtrip_b weak-symbol precedent): the builder MUST
 * implement the pure-logic contract below; the __attribute__((weak)) defaults
 * return the sentinel -2 (KSTORY_UNIMPLEMENTED), so the red baseline is RED on
 * real missing behaviour ("frozen contract not implemented"), never on a
 * build/link error. The cloud half (整段转写归档 / story_list 下发 / story_text
 * 下发 / V31 四条回退 / 近 N 句缓冲有界) is asserted by the python twin.
 *
 * NOTE ON THIS COMMENT BLOCK: no nested comment delimiters are used inside it
 * (they would close the block early); contract field annotations use "--".
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure C, host-compilable, no
 * chip/FreeRTOS/ESP-component deps, self-contained minimal JSON scan — NO
 * external JSON library dependency (same discipline as display/schedule_page.c);
 * recommended home: firmware/main/display/story_page.c + story_page.h; builder
 * ALSO wires BOTH the implementation source and THIS acceptance file into
 * HOST_ACCEPTANCE_SRCS in firmware/main/test/host_app/main/CMakeLists.txt —
 * same one-line pattern as packages 2/3/5/7 used for view_router.c /
 * schedule_page.c / char_stroke.c. The host component already exposes
 * INCLUDE_DIRS "../../../display", so this file probes
 * __has_include("story_page.h"): once the header lands it compiles against the
 * REAL declarations (single source of truth); before that the frozen fallback
 * declarations in THIS file apply. story_page.h MUST carry them VERBATIM —
 * same struct field names, same signatures; the builder may ADD struct fields,
 * may not rename/remove/repurpose them. Return-value NUMERIC codes are the
 * contract; the acceptance-local names KSA_* below need not appear in the
 * header (builder-side names may differ, values must not).
 *
 * ---- ① story_list 载荷消费 (P3-A2, 契约 §1.3 ③) ----
 *
 *   typedef struct {
 *       char id[64];      -- 契约 item.id (回看/回听取正文的键), safe-truncated
 *       char title[96];   -- 契约 item.title (列表行标题), safe-truncated
 *       char date[16];    -- 契约 item.date "MM-DD"; 载荷缺省该项时为 ""
 *   } kidpal_story_item_t;
 *
 *   #define KSTORY_LIST_MAX     16  -- 单页 items 上限 (超出 = -1 降级, 不静默截断)
 *   #define KSTORY_TOTAL_ABSENT (-1) -- story_list 载荷未携带 total 字段
 *
 *   typedef struct {
 *       int total;                        -- 契约 total (>=0) 或 KSTORY_TOTAL_ABSENT
 *       int count;                        -- items 行数 (0..KSTORY_LIST_MAX)
 *       kidpal_story_item_t items[KSTORY_LIST_MAX];
 *   } kidpal_story_list_t;
 *
 *   int kidpal_story_list_parse(const char *data_json, kidpal_story_list_t *out);
 *       Parse the data OBJECT of a 契约 §1.3 ③ story_list payload
 *       ({"total":3,"items":[{"id","title","date"}...]}) — the string mcp_server.cc
 *       holds as the kidpal/display `data` argument, NOT the full {view,...}
 *       envelope. Returns item count (>=0); -1 on: NULL data_json/out / malformed
 *       JSON / "items" missing or not an array / "total" present but not a
 *       non-negative integer / any item not an object or missing a non-string
 *       "id" or "title" / item count > KSTORY_LIST_MAX (调用方须请求更小页,
 *       绝不静默丢行). Tolerant (additive): "total" absent -> out->total =
 *       KSTORY_TOTAL_ABSENT; "date" absent -> ""; unknown extra fields ignored
 *       (契约 data 形状不锁死扩展位). Field copies are safe-truncated, always
 *       NUL-terminated, never overflow. -1 is the device degrade signal: the
 *       caller MUST show a friendly hint and MUST NOT blind-render (V27 / spec
 *       §4.0 — same discipline as kidpal_schedule_parse).
 *
 *   int kidpal_story_counter(const kidpal_story_list_t *list);
 *       P3-A2 右上角「故事果计数器 ×N」的唯一数据源解析 (契约 §一 story_list 硬约束
 *       / 闸 A 冻结留记②): list->total >= 0 -> 该值 (云端权威全集计数); 否则 ->
 *       list->count (缺省回退 items.length, 向后兼容). NULL list -> 0.
 *       冻结不变式: items 分页/截断时 绝不 以 items.length 顶替 total —— 载荷
 *       {"total":40,"items":[3 条]} 的计数器必须是 40 不是 3。
 *
 * ---- ② story_text 载荷消费 (P3-A5, 契约 §1.3 ④) ----
 *
 *   typedef struct {
 *       char id[64];
 *       char title[96];   -- 缺省时 ""; 契约无「生字标注」字段 (P3-E4 已裁不做反白高亮)
 *       char text[2048];  -- 正文, safe-truncated (分页在渲染面, 不在解析处丢)
 *   } kidpal_story_text_t;
 *
 *   int kidpal_story_text_parse(const char *data_json, kidpal_story_text_t *out);
 *       Parse 契约 §1.3 ④ data {"id","title","text"}. Returns 0 on success; -1 on
 *       NULL args / malformed JSON / missing or non-string "id" / missing or
 *       non-string "text" (id 是回听键、text 空则无正文可读, 二者 render-critical);
 *       "title" 缺省 -> "" (容错, 非致命). Unknown extra fields (含任何
 *       生字/高亮标注位) 一律忽略、不消费 (P3-E4).
 *
 * ---- ③ P3 页视图态 + 页内输入特例 (P3-A3/A4/A5) ----
 *
 *   typedef enum {
 *       KSTORY_PAGE_LIST      = 0,   -- 列表态 (S-F2.1)
 *       KSTORY_PAGE_READER    = 1,   -- 阅读态 (S-F2.3)
 *       KSTORY_PAGE_RECORDING = 2,   -- 录音态演出 (S-F2.2, manual 聆听)
 *       KSTORY_PAGE_STAMP     = 3    -- 盖章帧 (S-F2.4)
 *   } kidpal_story_page_t;
 *
 *   typedef struct {
 *       int page;            -- kidpal_story_page_t
 *       int cursor;          -- LIST: 反白选中行 (0-based)
 *       int count;           -- LIST: 行数
 *       int counter;         -- LIST: 右上角故事果计数 ×N
 *       int screen;          -- READER: 当前屏 (0-based)
 *       int screens;         -- READER: 总屏数 (>=1)
 *       int rec_seconds;     -- RECORDING: 已录秒数 (大时长分子)
 *       int stamp_index;     -- STAMP: 「第 N 颗」
 *       int archive_pend;    -- 停录 -> 待归档(整段转写上传)请求计数
 *       int cancel_count;    -- V31 取消/丢弃次数
 *       int tts_requests;    -- 阅读态 KEY「念给我听」朗读请求计数
 *   } kidpal_story_view_t;
 *
 *   kidpal_story_page_input 返回值 (数值码冻结, 不得偏移; 本文件用 KSA_* 局部名):
 *     KSA_ACT_GLOBAL       (-2)  该手势属全局层: 页内不吞, 视图零改动
 *     KSA_ACT_DEGRADE      (-3)  非法入参 (NULL 视图/越界键位): 视图零污染
 *     KSA_ACT_NONE           0   页内已消费但无可观察变化 (空列表/末屏等)
 *     KSA_ACT_CURSOR_NEXT    1   LIST KEY 短按: 反白选中项下移
 *     KSA_ACT_OPEN_READER    2   LIST KEY 双击: 进阅读态
 *     KSA_ACT_PAGE_TURN      3   READER BOOT 长按: 翻下一屏
 *     KSA_ACT_TTS_READ       4   READER KEY 短按: 朗读正文 (TTS 触发)
 *     KSA_ACT_STOP_RECORD    5   RECORDING KEY 短按: 「讲完啦」停录 -> 归档请求
 *     KSA_ACT_CANCEL_RECORD  6   RECORDING KEY 长按: V31 取消、丢弃不归档
 *
 *   int kidpal_story_view_init(kidpal_story_view_t *v, const kidpal_story_list_t *list);
 *       装载列表视图: page=LIST, cursor=0, screen=0, screens=1,
 *       count = list ? list->count : 0, counter = kidpal_story_counter(list),
 *       rec_seconds/stamp_index/archive_pend/cancel_count/tts_requests 归零。
 *       返回 0; v == NULL -> -1 (list == NULL 合法: 空列表页)。
 *       刷新对齐语义 (闸 A 留记①): 以云端 story_list 为权威 —— 再次 init 覆盖
 *       counter 为 kidpal_story_counter(新载荷), 本地推值不跨刷新保留。
 *
 *   int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g);
 *       P3 页内输入特例 (spec §4.0 全局按键手势表「页内特例」列; 页内特例必须由
 *       页处理函数承载, 参照 kidpal_schedule_page_input 先例)。冻结表:
 *         LIST:  KEY CLICK -> CURSOR_NEXT, cursor = (cursor+1) % count (count>0;
 *                      循环浏览 — 到底回顶, 否则末项之后的故事再也选不到)
 *                count==0 -> ACT_NONE (空列表不动)
 *                KEY DOUBLE_CLICK -> OPEN_READER, page=READER, screen=0
 *                      (count==0 -> ACT_NONE, 无行可读)
 *                KEY LONG_PRESS / BOOT CLICK / BOOT LONG_PRESS / BOOT DOUBLE_CLICK
 *                      -> ACT_GLOBAL (全局回首页 / 翻页; 页内不吞)
 *         READER:
 *                BOOT LONG_PRESS -> PAGE_TURN, screen = min(screen+1, screens-1)
 *                      (末屏 钳制 不回绕: 「第 X 屏 · 共 N 屏」是孩子的阅读进度
 *                       指示, 误触回首屏会丢进度; 重读走 KEY 朗读)
 *                already 末屏 -> ACT_NONE (screen 不变)
 *                KEY CLICK -> TTS_READ, tts_requests++ (屏/页状态不变;
 *                      host 只测 触发标志, TTS 声学殿后 9b, P3-E2)
 *                其余手势 -> ACT_GLOBAL
 *         RECORDING:
 *                KEY CLICK -> STOP_RECORD, page=LIST, archive_pend++
 *                      (rec_seconds 定格; 停录 = 发 stop-accumulation 事件让网关
 *                       整段转写归档 — 真实采音不在本冻结面)
 *                KEY LONG_PRESS -> CANCEL_RECORD, page=LIST, cancel_count++,
 *                      archive_pend 不变 (V31: 已累积内容丢弃、不归档; 全局层照常
 *                      执行 HOME — 见本测试的全局路由守卫)
 *                其余手势 -> ACT_GLOBAL
 *         STAMP: 任何手势 -> ACT_GLOBAL (盖章帧页内零语义, 全局手势照常)
 *       NULL v / key >= KEY_ID_COUNT (PWR 面) -> KSA_ACT_DEGRADE, 视图零污染。
 *       gesture == KEY_GESTURE_NONE -> KSA_ACT_NONE (无副作用)。
 *
 *   int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action);
 *       消费 契约 §1.4 control 动作 (ctrl_action = kidpal_display_control_action
 *       的返回值, 包 2 冻结数值码 VRT_CTRL_*):
 *         VRT_CTRL_START_RECORDING (2) -> 1 (consumed-and-entered):
 *              page=RECORDING, rec_seconds=0, screen 复位, counter 不变
 *              (录完前不预推 +1 — 取消的故事不该占故事果位, 冻结决策 D2)。
 *              已处 RECORDING 再收到 -> 1 且 幂等 (page 保持、rec_seconds 不被
 *              清零; 网关重试下发不得重置孩子已录时长)。
 *         VRT_CTRL_END_SESSION / VRT_CTRL_STOP_SPEAKING -> 0 (已知但故事页不消费,
 *              视图零改动 — 交给全局会话/播报面)。
 *         其它值 (含 VRT_CTRL_UNKNOWN) 或 v == NULL -> -1 (降级, 视图零改动)。
 *
 *   int kidpal_story_rec_tick(kidpal_story_view_t *v, uint32_t elapsed_ms,
 *                             int *flash, int *wave);
 *       录音态演出一帧 (spec S-F2.2 p3-recording: REC 反白点闪 + 大时长 +
 *       手绘声波跳动 + pose-record)。置 v->rec_seconds = elapsed_ms / 1000
 *       (钳制上限 5999 秒 = 显示位 99:59, 绝不溢出显示位)，并按 elapsed_ms 计算:
 *         *flash: 0|1 (REC 点在/灭), 在 0..2000ms 窗口内 至少翻转一次 (闪点存在),
 *                 且相邻翻转间隔 >= 100ms (儿童视觉安全下界, 非频闪);
 *         *wave : >=0 声波相位档, 在 0..2000ms 窗口内 至少出现两个不同值
 *                 (声波在跳 = 「正在录」的可观察证据, 静止即红);
 *       确定性: 同一 elapsed_ms 恒得同值 (无隐式全局状态)。flash/wave 可为 NULL
 *       (调用方只要时长)。返回 0; v == NULL 或 page != RECORDING -> -1。
 *       (相位档数/占空比等美学参数不锁 — 留 design review; 锁的是「在闪/在跳」
 *        与显示位数钳制这些改变用户任务成败的事实。)
 *
 *   int kidpal_story_rec_time(const kidpal_story_view_t *v, char *out, size_t cap);
 *       大时长文本 (spec S-F2.2 示例「00:47 式」): out = "%02d:%02d" 的
 *       (rec_seconds/60, rec_seconds%60)。返回 0; 若 cap 不足以容纳完整串
 *       (含 NUL) -> -1 (调用方须加大缓冲); 恒 NUL 结尾、绝不溢出。
 *       v == NULL -> -1。
 *
 *   int kidpal_story_open_reader(kidpal_story_view_t *v, const kidpal_story_text_t *st,
 *                                int screen_lines);
 *       进阅读态并分页 (P3-A5 / S-F2.3): page=READER, screen=0,
 *       screens = 容纳 st->text 全文所需屏数 (>=1)。冻结性质 (与包 2
 *       kidpal_display_text_page_count 同纪律 — 全文上屏、显示端不丢内容):
 *         screens >= 1;
 *         screens * screen_lines >= 正文行数 (按 '\n' 计, 空文本记 1 行);
 *         正文更长 -> screens 不减 (单调);
 *         screen_lines <= 0 -> 防御为 1 屏 (返回 0) 或显式 -1, 绝不除零崩溃;
 *         v == NULL 或 st == NULL -> -1 (无正文不盲渲染)。
 *       返回 0 / -1。
 *
 *   int kidpal_story_screen_label(const kidpal_story_view_t *v, char *out, size_t cap);
 *       阅读态底部屏数指示 (spec S-F2.3 逐字): "第 %d 屏 · 共 %d 屏" 的
 *       (v->screen + 1, v->screens)。返回 0; cap 不足 -> -1; v == NULL -> -1。
 *       (反白/加粗等视觉不锁; 钉的是「第 X 屏 · 共 N 屏」文本与翻页联动。)
 *
 * ---- ④ 盖章帧 (P3-A6, spec S-F2.4) ----
 *
 *   KSA_STAMP_HOLD_MS     800    -- pose-jump 定格 0.8 秒 (S-F2.4「定格 0.8 秒」)
 *   KSA_STAMP_ANGLE_DECI (-80)   -- 斜盖 -8° 的十分度 (1bpp 整型, 无浮点)
 *
 *   typedef struct {
 *       int rings;         -- 双圈印章 -> 2
 *       int inner_dashed;  -- 外实内虚: 内圈虚线 -> 1
 *       int angle_deci;    -- 斜盖 -8° -> -80
 *       int hold_ms;       -- 定格 0.8s -> 800
 *   } kidpal_story_stamp_spec_t;
 *
 *   int kidpal_story_stamp_spec(kidpal_story_stamp_spec_t *out);
 *       冻结盖章几何: rings=2, inner_dashed=1, angle_deci=-80, hold_ms=800。
 *       返回 0; out == NULL -> -1。
 *
 *   int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id);
 *       云端保存成功 ack -> 触发盖章帧 (S-F2.2「上传成功那一瞬」/ S-F2.4):
 *         page=STAMP; stamp_index = counter + 1; counter = stamp_index
 *         (N 与 P3 故事果计数器 严格同步 — 闸 A 留记①: 板子持 total,
 *          保存后本地推 +1, 随后 story_list 刷新以云端权威对齐)。
 *         幂等: 同一 saved_id 重复 ack -> 返回 0 且 counter/stamp_index 不再 +1
 *               (mcp 回执重试不得重复发徽章)。
 *         新 saved_id -> 再推一颗。
 *       返回 0; v == NULL 或 saved_id == NULL 或空串 -> -1 (状态零污染)。
 *
 *   int kidpal_story_stamp_text(const kidpal_story_view_t *v, char *out, size_t cap);
 *       盖章文案 (spec S-F2.4 逐字): "故事徽章·第 %d 颗已入册！" 的 v->stamp_index。
 *       返回 0; cap 不足 -> -1; v == NULL -> -1。
 *       同步不变式 (测试钉死): 文案内嵌的 N == v->counter。
 *
 * ---- ⑤ 渲染面 (host 纯 LVGL, P3-A2/A4/A5/A6 渲染证据) ----
 *
 *   int kidpal_story_page_render(const kidpal_story_view_t *v,
 *                                const kidpal_story_list_t *list,
 *                                const kidpal_story_text_t *story);
 *       在活动屏上按 v->page 真画当前故事页 (纯 LVGL on the ACTIVE screen —
 *       lv_obj calls / lv_label / lv_anim only, no Board/Display class deps, so
 *       it compiles and runs in the host linux-target runner; 同
 *       kidpal_schedule_page_render 先例)。清屏后绘制 (幂等, 可反复调):
 *         LIST   -> 标题「我的故事」+ 右上角故事果计数器 ×N (kidpal_story_counter
 *                   值) + 每行 故事果图标 + 标题 + 日期 + 选中行反白 (cursor)
 *                   + 常驻语音入口提示文案「说"我要讲故事"开录新的」(S-F2.1)。
 *                   需要 list: list == NULL 且页面为 LIST -> -1 (显式降级)。
 *         READER -> 标题 + 当前屏正文 + 底部 kidpal_story_screen_label
 *                   (需要 story; 缺 -> -1)
 *         REC    -> REC 闪点 + 大时长 (kidpal_story_rec_time) + 声波
 *         STAMP  -> 双圈印章 + kidpal_story_stamp_text + pose-jump 定格
 *       返回 0 成功; -1 非法入参/该页所需数据缺失 (显式降级, 绝不盲渲染, V27);
 *       NULL v -> -1; NEVER crashes.
 *
 * 真实麦克风采音 / TTS 声学 / 真 ASR 引擎 不在本冻结面 (P3-E2: 采音为硬件强绑定
 * 子能力, 显式殿后包 9b; 本文件只测状态/演出/请求触发)。云端归档与下发的证据在
 * server/acceptance/test_story_p3.acceptance.py。
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria / sourceRef / decisionCoverage -> test):
 *   coverage: P3-A1 structural (+S-F2.1/S-F2.2/S-F2.3/S-F2.4 装配面) -> test P3-A1
 *             (故事页消费面装配: story 模块存在于设备树 + 编入设备构建 CMakeLists +
 *              mcp_server.cc 的 story_list/story_text 路由 ack(非 default 降级) +
 *              渲染面消费调用点 >= 2 文件 + 解析失败降级语义标记 + 盖章文案面 +
 *              常驻语音入口提示面; 包 2 底座在世 + 包 8 冻结数值码不回退守卫)
 *   coverage: P3-A2 behavioral (+S-F2.1) -> test P3-A2 (story_list 解析逐字段 +
 *             total 优先 / items.length 回退 双钉 + 分页时不得以 items.length
 *             顶替 total + 畸形/缺字段/越界 cap -1 降级 + 扩展字段容忍 + total=0
 *             与缺省可区分 + 空列表合法页 + 超长字段 safe-truncate)
 *   coverage: P3-A3 behavioral (+S-F2.1 / spec §4.0 页内特例) -> test P3-A3
 *             (KEY 短按光标下移循环 / KEY 双击进阅读态 / 空列表不动 / KEY 长按·
 *              BOOT 单击等全局手势不被页内吞 / 越界键位与 NULL 视图零污染)
 *   coverage: P3-A4 behavioral (+S-F2.2 / 契约 §1.4 start_recording / §四 manual
 *             聆听) -> test P3-A4 (control/start_recording -> 录音态 + 计数器不
 *             预推 + 重试幂等 + REC 演出帧(闪点在闪·非频闪/声波在跳/时长确定性) +
 *             大时长 00:47 式文本 + KEY 停录 -> 归档请求触发 + V31 KEY 长按取消
 *             丢弃不归档 + 非录音态/未知 action 不消费 + 包 3
 *             SES_STATE_MANUAL_RECORDING 接合面在世守卫 + 全局 HOME 表不回退守卫)
 *   coverage: P3-A5 behavioral (+S-F2.3 / 契约 §1.3 ④) -> test P3-A5 (story_text
 *             解析 {id,title,text} + 生字/高亮扩展位不消费 + 阅读态分页全文不丢 +
 *             BOOT 长按翻下屏·末屏钳制 + 「第 X 屏 · 共 N 屏」逐字 + KEY 朗读触发
 *             标志计数 + 无正文不盲渲染 + 包 2 story 路由续接守卫)
 *   coverage: P3-A6 behavioral (+decision contract-§一-story_list-total) -> test
 *             P3-A6 (on_saved -> 盖章帧 + 文案「故事徽章·第 N 颗已入册！」逐字 +
 *             几何 双圈/内虚/-8°/800ms + N 与 counter 严格同步不变式 + 同 id 幂等
 *             + 新 id 再推一颗 + 刷新以云端 total 权威对齐 + 畸形 ack 零污染)
 *   coverage: P3-A2/A3/A4/A5/A6 render face -> test render (host-runner-only, lvgl
 *             软渲染真帧: 列表内容暗像素 + 反白可见差异 + ×N 计数差异 + 阅读翻页
 *             差异 + 录音/盖章态画面互异 + 非法参数不崩) — requires lvgl.h;
 *             独立红基线 stub 编译无 lvgl include path 时被 __has_include 排除,
 *             其红态在 builder 接线进 host runner 时兑现 (如实记录)
 *   coverage: P3-E2 (两面证据 + 采音殿后 9b) -> 本文件只测状态/演出/触发, 渲染
 *             观感殿后 9b; 云侧证据见 python 孪生文件的 P3-E2 回归守卫
 *   coverage: P3-E3 / P3-E4 + boundary/security -> test boundary (S-F2.5 润色后置:
 *             固件零「润色/polish」引用; 生字反白高亮已裁: 零高亮态/零 story_text
 *             扩展字段消费; 归档落盘归云: 固件零 stories/ 引用; 扫描器自检防假绿;
 *             设备不直连 OpenClaw/18789; 契约 §1.4 start_recording 到达面在世)
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring: add BOTH lines to HOST_ACCEPTANCE_SRCS in
 *      firmware/main/test/host_app/main/CMakeLists.txt —
 *        "../../story_p3.acceptance.c"
 *        "../../../display/story_page.c"   (落地后; 强符号覆盖本文件 weak 哨兵)
 *      保持 "../../host_infra_smoke.acceptance.c" 在列表 最后 (包 0 boundary 齿
 *      注册序守卫: 后列出 = 先注册 = 先跑, 其 nothing-rendered 断言须对新鲜进程
 *      执行)。本包渲染齿排在既有 schedule/home render 齿之后, 不影响该守卫。
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *   (Freeze-phase red baseline used the standalone stub per
 *    firmware/main/test/acceptance/red-baseline/ — see
 *    firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md.)
 *
 * 冻结相位决策登记 (acceptance-author 裁定, spec 未逐字钉死之处; tighten-only,
 * builder 若需改语义走 planner 重新派发, 不得自改本文件):
 *   D1 列表光标 循环回绕 vs 阅读屏 末屏钳制 — 理由: 列表是有限项浏览集, 钳制会让
 *      末项之后的项再也够不着 (可操作性); 阅读屏是进度指示, 回绕会丢「读到哪了」
 *      (task-critical 状态), 重读走 KEY 朗读。
 *   D2 start_recording 时 counter 不预推 +1 — 理由: V31 取消路径下丢弃的故事不该
 *      占故事果位 (S-F2.4 严格同步的是「已入册」数)。
 *   D3 盖章 N 采「本地推 +1 → story_list 刷新以云端 total 权威覆盖」— 理由: 闸 A
 *      留记① 现解, 契约零改动即可立住; 若 builder 判 N 必须取云端保存响应权威值,
 *      那属 story_text 增 total 的 ① 契约触点, 须回 planner 重走契约门。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>

/* 包 2 / 包 3 已冻结的共享底座头（相对本文件位置，host runner 与独立 stub 都可达）。 */
#include "../key_gesture_parser.h"
#include "../session_state_machine.h"
#include "../display/view_router.h"

#define KSTORY_MAX_PATH 1024
#define KSTORY_MAX_READ (8u * 1024u * 1024u)

/* -------------------------------------------------------------------------- *
 * Frozen contract probe: use the real header once the builder lands it, frozen
 * fallback declarations before that (single source of truth switch).
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("story_page.h")
#include "story_page.h"
#define KSTORY_HAS_REAL_HEADER 1
#endif
#endif

#ifndef KSTORY_HAS_REAL_HEADER
/* FROZEN fallback declarations — story_page.h must carry these VERBATIM. */
typedef struct {
    char id[64];
    char title[96];
    char date[16];
} kidpal_story_item_t;

#define KSTORY_LIST_MAX     16
#define KSTORY_TOTAL_ABSENT (-1)

typedef struct {
    int total;
    int count;
    kidpal_story_item_t items[KSTORY_LIST_MAX];
} kidpal_story_list_t;

typedef struct {
    char id[64];
    char title[96];
    char text[2048];
} kidpal_story_text_t;

typedef enum {
    KSTORY_PAGE_LIST = 0,
    KSTORY_PAGE_READER = 1,
    KSTORY_PAGE_RECORDING = 2,
    KSTORY_PAGE_STAMP = 3
} kidpal_story_page_t;

typedef struct {
    int page;
    int cursor;
    int count;
    int counter;
    int screen;
    int screens;
    int rec_seconds;
    int stamp_index;
    int archive_pend;
    int cancel_count;
    int tts_requests;
} kidpal_story_view_t;

typedef struct {
    int rings;
    int inner_dashed;
    int angle_deci;
    int hold_ms;
} kidpal_story_stamp_spec_t;

int kidpal_story_list_parse(const char *data_json, kidpal_story_list_t *out);
int kidpal_story_counter(const kidpal_story_list_t *list);
int kidpal_story_text_parse(const char *data_json, kidpal_story_text_t *out);
int kidpal_story_view_init(kidpal_story_view_t *v, const kidpal_story_list_t *list);
int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g);
int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action);
int kidpal_story_rec_tick(kidpal_story_view_t *v, uint32_t elapsed_ms, int *flash,
                          int *wave);
int kidpal_story_rec_time(const kidpal_story_view_t *v, char *out, size_t cap);
int kidpal_story_open_reader(kidpal_story_view_t *v, const kidpal_story_text_t *st,
                             int screen_lines);
int kidpal_story_screen_label(const kidpal_story_view_t *v, char *out, size_t cap);
int kidpal_story_stamp_spec(kidpal_story_stamp_spec_t *out);
int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id);
int kidpal_story_stamp_text(const kidpal_story_view_t *v, char *out, size_t cap);
int kidpal_story_page_render(const kidpal_story_view_t *v,
                             const kidpal_story_list_t *list,
                             const kidpal_story_text_t *story);
#endif /* !KSTORY_HAS_REAL_HEADER */

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — acceptance-local numeric codes + weak defaults.
 * Values are the contract; builder-side macro names may differ, numbers may not.
 * ------------------------------------------------------------------------ */
#define KSTORY_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

#define KSA_ACT_GLOBAL        (-2)
#define KSA_ACT_DEGRADE       (-3)
#define KSA_ACT_NONE            0
#define KSA_ACT_CURSOR_NEXT     1
#define KSA_ACT_OPEN_READER     2
#define KSA_ACT_PAGE_TURN       3
#define KSA_ACT_TTS_READ        4
#define KSA_ACT_STOP_RECORD     5
#define KSA_ACT_CANCEL_RECORD   6

#define KSA_STAMP_HOLD_MS     800
#define KSA_STAMP_ANGLE_DECI (-80)

__attribute__((weak)) int kidpal_story_list_parse(const char *data_json,
                                                  kidpal_story_list_t *out)
{
    (void)data_json; (void)out;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_counter(const kidpal_story_list_t *list)
{
    (void)list;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_text_parse(const char *data_json,
                                                  kidpal_story_text_t *out)
{
    (void)data_json; (void)out;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_view_init(kidpal_story_view_t *v,
                                                 const kidpal_story_list_t *list)
{
    (void)v; (void)list;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key,
                                                  key_gesture_t g)
{
    (void)v; (void)key; (void)g;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action)
{
    (void)v; (void)ctrl_action;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_rec_tick(kidpal_story_view_t *v, uint32_t elapsed_ms,
                                                int *flash, int *wave)
{
    (void)v; (void)elapsed_ms; (void)flash; (void)wave;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_rec_time(const kidpal_story_view_t *v, char *out,
                                                size_t cap)
{
    (void)v; (void)out; (void)cap;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_open_reader(kidpal_story_view_t *v,
                                                   const kidpal_story_text_t *st,
                                                   int screen_lines)
{
    (void)v; (void)st; (void)screen_lines;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_screen_label(const kidpal_story_view_t *v, char *out,
                                                    size_t cap)
{
    (void)v; (void)out; (void)cap;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_stamp_spec(kidpal_story_stamp_spec_t *out)
{
    (void)out;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id)
{
    (void)v; (void)saved_id;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_stamp_text(const kidpal_story_view_t *v, char *out,
                                                  size_t cap)
{
    (void)v; (void)out; (void)cap;
    return KSTORY_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_page_render(const kidpal_story_view_t *v,
                                                   const kidpal_story_list_t *list,
                                                   const kidpal_story_text_t *story)
{
    (void)v; (void)list; (void)story;
    return KSTORY_UNIMPLEMENTED;
}

/* 包 2 / 包 3 的真实强符号（host runner 里由 view_router.c / session_state_machine.cc /
 * key_gesture_parser.cc 提供；独立 stub 编译时未链接 —— 以 weak 指针判空跳过）。 */
extern __attribute__((weak)) int kidpal_display_route(const char *view);
extern __attribute__((weak)) int kidpal_display_control_action(const char *action);
extern __attribute__((weak)) void ses_machine_init(ses_machine_t *m);
extern __attribute__((weak)) const ses_transition_t *ses_machine_dispatch(ses_machine_t *m,
                                                                         ses_event_t ev);
extern __attribute__((weak)) global_action_t key_route_global(key_id_t key, key_gesture_t g);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), WSA/VRT/KSP precedent.
 * All scans run against the REAL working tree located from the runner cwd.
 * Directories named test / host_app / build / .git / assets are EXCLUDED from
 * every scan (test/ hosts THIS frozen file whose comments legitimately mention
 * the frozen needles; host_app/build are build artifacts; assets are binary
 * art, not implementation surface).
 * ------------------------------------------------------------------------ */

static char kstory_root[KSTORY_MAX_PATH];
static int kstory_root_ready;

static int kstory_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kstory_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kstory_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int kstory_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KSTORY_MAX_PATH];

    if (kstory_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kstory_is_regular(probe)) {
            snprintf(kstory_root, sizeof(kstory_root), "%s", cands[i]);
            kstory_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kstory_rel(char *out, size_t cap, const char *rel)
{
    if (!kstory_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", kstory_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", kstory_root, rel);
    return 1;
}

static int kstory_exists(const char *rel)
{
    char p[KSTORY_MAX_PATH];
    return kstory_rel(p, sizeof(p), rel) && kstory_is_regular(p);
}

static char *kstory_read(const char *rel, size_t *out_len)
{
    char p[KSTORY_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kstory_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSTORY_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int kstory_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int kstory_contains_ci(const char *hay, const char *needle)
{
    size_t n;

    assert(hay != NULL && needle != NULL);
    n = strlen(needle);
    if (n == 0) {
        return 1;
    }
    for (const char *h = hay; *h != '\0'; h++) {
        size_t i = 0;
        while (i < n && h[i] != '\0' &&
               tolower((unsigned char)h[i]) == tolower((unsigned char)needle[i])) {
            i++;
        }
        if (i == n) {
            return 1;
        }
    }
    return 0;
}

static int kstory_file_contains(const char *rel, const char *needle)
{
    char *buf = kstory_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kstory_contains(buf, needle);
    free(buf);
    return r;
}

static int kstory_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = kstory_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

static int kstory_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                          const char *needle, int ci,
                                          char *first_hit, size_t cap, int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[KSTORY_MAX_PATH];
        char rel_next[KSTORY_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            kstory_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += kstory_count_files_with_needle(abs_next, rel_next, needle, ci,
                                                    first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KSTORY_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL);
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                int m = ci ? kstory_contains_ci(buf, needle) : kstory_contains(buf, needle);
                if (m) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int kstory_scan_count(const char *rel_dir, const char *needle, int ci,
                             char *first_hit, size_t cap)
{
    char abs[KSTORY_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!kstory_rel(abs, sizeof(abs), rel_dir) || !kstory_is_dir(abs)) {
        return 0;
    }
    return kstory_count_files_with_needle(abs, rel_dir, needle, ci, first_hit, cap, 0);
}

#define KSTORY_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kstory_locate_root(), \
    "KSTORY RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen offline contract not being implemented yet. */
static void kstory_require_implemented(int value, const char *what)
{
    if (value == KSTORY_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KSTORY RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic P3 故事页 per the frozen contract "
                 "in firmware/main/test/story_p3.acceptance.c header and wire it into "
                 "HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* -------------------------------------------------------------------------- *
 * Canonical payloads — 契约 §1.3 ③/④ 示例逐字 (story side of interface-contract).
 * NOTE the contract's own example carries total:3 with only 2 items: 分页/截断时
 * 计数器仍取 total（不得以 items.length 顶替）—— P3-A2 的硬约束就地可测。
 * ------------------------------------------------------------------------ */
static const char *KSTORY_LIST_CANONICAL =
    "{\"total\":3,\"items\":"
    "[{\"id\":\"2026-08-20-xiaotuzi\",\"title\":\"小兔子找朋友\",\"date\":\"08-20\"},"
    "{\"id\":\"2026-08-25-yonggan\",\"title\":\"勇敢的小明\",\"date\":\"08-25\"}]}";

static const char *KSTORY_TEXT_CANONICAL =
    "{\"id\":\"2026-08-27-maoxian\",\"title\":\"我的大冒险\","
    "\"text\":\"从前有一只小猫……（正文）\"}";

/* 10 行正文 (每屏 4 行 -> >=3 屏, 全文不丢) — 阅读态分页性质钉。 */
static const char *KSTORY_TEXT_TEN_LINES =
    "第一行\n第二行\n第三行\n第四行\n第五行"
    "\n第六行\n第七行\n第八行\n第九行\n第十行";

/* -------------------------------------------------------------------------- *
 * P3-A1 — P3 故事页消费面装配结构（structural, +S-F2.1/S-F2.2/S-F2.3/S-F2.4）
 * Teeth, each a REAL repo fact:
 *   1) a story page module exists in the device tree referencing the frozen
 *      contract symbol kidpal_story_list_parse( — today NOTHING under
 *      firmware/main implements the P3 故事页 contract (needle count 0);
 *   2) the module is compiled into the DEVICE build: firmware/main/CMakeLists.txt
 *      lists the file's basename (assembly fact, not a floating file);
 *   3) the mcp kidpal/display handler really WIRES both story routes:
 *      mcp_server.cc carries VRT_ROUTE_STORY_LIST and VRT_ROUTE_STORY_TEXT acks
 *      (today routes 5/6 fall into the default degrade — no story face exists);
 *   4) the render face is REACHED: >= 2 distinct files reference
 *      kidpal_story_page_render( (definition + consumer call site);
 *   5) the story module carries the explicit parse-degrade semantic (-1 ->
 *      friendly hint, never blind render, V27 discipline on the data shape);
 *   6) the 盖章 copy face and the 常驻语音入口提示面 exist in the device tree
 *      (S-F2.4「故事徽章」/ S-F2.1「我要讲故事」开录提示) — 两者今日零命中;
 *   guards (must stay green): 包 2 路由底座在世 + 包 8 预留数值码未偏移。
 * RED today: teeth 1)-6) have no implementation; guards green (native).
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: P3 故事页消费面装配结构 (P3-A1)", "[story_p3][P3-A1][structural][linux]")
{
    char hit[KSTORY_MAX_PATH];
    int n_parse, n_render;

    KSTORY_REQUIRE_ROOT();

    /* tooth 1: story 页模块存在于设备树 */
    n_parse = kstory_scan_count("firmware/main", "kidpal_story_list_parse(", 0, hit,
                                sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_parse >= 1,
        "KSTORY-A1 RED: no module under firmware/main implements the frozen story page "
        "contract (kidpal_story_list_parse absent) — P3 故事页消费面缺失 "
        "(契约 §1.3 ③④, S-F2.1/S-F2.3)");

    /* tooth 2: 模块被设备构建引用（CMakeLists 装配，非浮文件） */
    {
        char base[KSTORY_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(kstory_file_contains("firmware/main/CMakeLists.txt", base),
            "KSTORY-A1 RED: the story page source is not referenced by "
            "firmware/main/CMakeLists.txt — 模块未编入设备构建（装配缺失）");
    }

    /* tooth 3: mcp kidpal/display 的 story_list / story_text 两条路由真正接渲染面 */
    TEST_ASSERT_TRUE_MESSAGE(
        kstory_file_contains("firmware/main/mcp_server.cc", "VRT_ROUTE_STORY_LIST"),
        "KSTORY-A1 RED: mcp_server.cc carries no story_list route ack — kidpal/display "
        "的 story_list 视图仍落 default 降级分支, P3 列表渲染面未接入消费链路 "
        "(契约 §1.3 ③ / §四 mcp 工具调用)");
    TEST_ASSERT_TRUE_MESSAGE(
        kstory_file_contains("firmware/main/mcp_server.cc", "VRT_ROUTE_STORY_TEXT"),
        "KSTORY-A1 RED: mcp_server.cc carries no story_text route ack — 回看/回听的 "
        "story_text 视图仍落 default 降级分支 (契约 §1.3 ④, S-F2.3)");

    /* tooth 4: 定义 + 消费调用点 >= 2 个文件 */
    n_render = kstory_scan_count("firmware/main", "kidpal_story_page_render(", 0, hit,
                                 sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_render >= 2,
        "KSTORY-A1 RED: kidpal_story_page_render is defined but never called — P3 渲染"
        "面未接入 mcp 消费链路（仅定义不调用不算接入）");

    /* tooth 5: 解析失败 = 显式降级语义标记（绝不盲渲染, V27 纪律在 data 形状上） */
    {
        char render_hit[KSTORY_MAX_PATH];
        char *buf = kstory_read(hit, NULL);
        if (buf == NULL) {
            (void)kstory_scan_count("firmware/main", "kidpal_story_page_render(", 0,
                                    render_hit, sizeof(render_hit));
            buf = kstory_read(render_hit, NULL);
        }
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "KSTORY-A1 RED: story page file unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            kstory_contains(buf, "降级") || kstory_contains_ci(buf, "degrade") ||
            strstr(buf, "-1") != NULL,
            "KSTORY-A1 RED: the story page module carries no parse-degrade semantic "
            "marker — 解析失败必须显式降级而非盲渲染 (V27 / spec §4.0)");
        free(buf);
    }

    /* tooth 6a: 盖章文案面存在于设备树 (S-F2.4) */
    {
        char stamp_hit[KSTORY_MAX_PATH];
        int n_stamp = kstory_scan_count("firmware/main", "故事徽章", 0, stamp_hit,
                                        sizeof(stamp_hit));
        TEST_ASSERT_TRUE_MESSAGE(n_stamp >= 1,
            "KSTORY-A1 RED: no device-tree file carries the 故事徽章 stamp copy — 盖章帧"
            "文案面缺失 (S-F2.4「故事徽章·第 N 颗已入册！」)");
    }
    /* tooth 6b: 列表页常驻语音入口提示面存在 (S-F2.1; 意图判定在云端, 提示文案在设备) */
    {
        char hint_hit[KSTORY_MAX_PATH];
        int n_hint = kstory_scan_count("firmware/main", "我要讲故事", 0, hint_hit,
                                       sizeof(hint_hit));
        TEST_ASSERT_TRUE_MESSAGE(n_hint >= 1,
            "KSTORY-A1 RED: no device-tree file carries the 常驻语音入口提示「我要讲故事」"
            "— S-F2.1 列表页常驻开录提示缺失");
    }

    /* guards (must stay green): 包 2 路由底座 + 包 8 冻结数值码不回退 */
    TEST_ASSERT_TRUE_MESSAGE(kstory_file_contains("firmware/main/mcp_server.cc",
                                                  "display/view_router.h"),
        "KSTORY-A1 RED(infra): mcp_server.cc no longer includes display/view_router.h — "
        "包 2 路由底座被移除");
    TEST_ASSERT_TRUE_MESSAGE(kstory_file_contains("firmware/main/CMakeLists.txt",
                                                  "view_router.c"),
        "KSTORY-A1 RED(infra): view_router.c no longer compiled into the device build — "
        "包 2 底座装配被破坏");
    TEST_ASSERT_TRUE_MESSAGE(kstory_file_contains("firmware/main/mcp_server.cc", "kidpal/display"),
        "KSTORY-A1 RED(infra): kidpal/display tool no longer registered in mcp_server.cc — "
        "display 下发到达面被破坏");
    TEST_ASSERT_TRUE_MESSAGE(VRT_ROUTE_STORY_LIST == 5 && VRT_ROUTE_STORY_TEXT == 6 &&
                                 VRT_CTRL_START_RECORDING == 2,
        "KSTORY-A1 RED(infra): the numeric codes reserved for 包 8 接合 drifted "
        "(VRT_ROUTE_STORY_LIST must be 5, STORY_TEXT 6, CTRL_START_RECORDING 2) — "
        "包 2 冻结离线契约数值码不得偏移");
}

/* -------------------------------------------------------------------------- *
 * P3-A2 — story_list 渲染数据面（behavioral, frozen offline contract）
 * 双钉: 优先契约 total 字段; 缺省时回退 items.length。再加契约硬约束（分页/截断
 * 时不得以 items.length 顶替 total）与降级/边界路径。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: story_list 载荷解析 + 故事果计数器 total/回退双钉 (P3-A2)",
          "[story_p3][P3-A2][linux]")
{
    kidpal_story_list_t list;
    int n, c;

    /* canonical 契约 §1.3 ③ 示例: total=3, items=2 条 */
    memset(&list, 0, sizeof(list));
    n = kidpal_story_list_parse(KSTORY_LIST_CANONICAL, &list);
    kstory_require_implemented(n, "kidpal_story_list_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == 2,
        "KSTORY-A2 RED: canonical 契约 §1.3 ③ payload must parse to exactly 2 items");
    TEST_ASSERT_TRUE_MESSAGE(list.count == 2,
        "KSTORY-A2 RED: out->count must equal the parsed item count");
    TEST_ASSERT_TRUE_MESSAGE(list.total == 3,
        "KSTORY-A2 RED: 契约 total 字段必须被解析为 3 (故事果全集计数)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(list.items[0].id, "2026-08-20-xiaotuzi") == 0 &&
                                 strcmp(list.items[0].title, "小兔子找朋友") == 0 &&
                                 strcmp(list.items[0].date, "08-20") == 0,
        "KSTORY-A2 RED: item 0 fields wrong — id/title/date 必须逐字段保真 (契约 §1.3 ③)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(list.items[1].id, "2026-08-25-yonggan") == 0 &&
                                 strcmp(list.items[1].title, "勇敢的小明") == 0 &&
                                 strcmp(list.items[1].date, "08-25") == 0,
        "KSTORY-A2 RED: item 1 fields wrong — id/title/date 必须逐字段保真 (契约 §1.3 ③)");

    /* 钉① 优先 total: 分页/截断载荷 (total=40, 3 条) -> 计数器 40 不是 3 */
    {
        const char *paged =
            "{\"total\":40,\"items\":"
            "[{\"id\":\"a1\",\"title\":\"故事一\",\"date\":\"09-01\"},"
            "{\"id\":\"a2\",\"title\":\"故事二\",\"date\":\"09-02\"},"
            "{\"id\":\"a3\",\"title\":\"故事三\",\"date\":\"09-03\"}]}";
        memset(&list, 0, sizeof(list));
        n = kidpal_story_list_parse(paged, &list);
        TEST_ASSERT_TRUE_MESSAGE(n == 3,
            "KSTORY-A2 RED: paged payload must parse its 3 items");
        c = kidpal_story_counter(&list);
        kstory_require_implemented(c, "kidpal_story_counter unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(c == 40,
            "KSTORY-A2 RED: 计数器必须取契约 total(40) 而非 items.length(3) — 契约 §一 "
            "story_list 硬约束「items 分页/截断时不得以 items.length 顶替 total」");
    }

    /* 钉② 缺省回退 items.length: 载荷无 total -> 计数器 = count */
    {
        const char *no_total =
            "{\"items\":[{\"id\":\"b1\",\"title\":\"旧故事一\",\"date\":\"08-01\"},"
            "{\"id\":\"b2\",\"title\":\"旧故事二\",\"date\":\"08-02\"},"
            "{\"id\":\"b3\",\"title\":\"旧故事三\",\"date\":\"08-03\"}]}";
        memset(&list, 0, sizeof(list));
        n = kidpal_story_list_parse(no_total, &list);
        TEST_ASSERT_TRUE_MESSAGE(n == 3,
            "KSTORY-A2 RED: payload without total must still parse (向后兼容)");
        TEST_ASSERT_TRUE_MESSAGE(list.total == KSTORY_TOTAL_ABSENT,
            "KSTORY-A2 RED: absent total must surface as KSTORY_TOTAL_ABSENT (-1), "
            "never as 0 (0 意味着「真的一个故事都没有」)");
        c = kidpal_story_counter(&list);
        TEST_ASSERT_TRUE_MESSAGE(c == 3,
            "KSTORY-A2 RED: 无 total 时计数器必须回退 items.length (契约 §一 缺省回退)");
    }

    /* total == 0 是合法值（一个故事都没录），与「缺省」必须可区分 */
    {
        memset(&list, 0, sizeof(list));
        n = kidpal_story_list_parse("{\"total\":0,\"items\":[]}", &list);
        TEST_ASSERT_TRUE_MESSAGE(n == 0, "KSTORY-A2 RED: total=0 empty page must parse");
        c = kidpal_story_counter(&list);
        TEST_ASSERT_TRUE_MESSAGE(c == 0,
            "KSTORY-A2 RED: total=0 must read as 0 颗 (零故事是真实状态, 不是缺省)");
    }

    /* tolerant (additive): 缺 date -> "" ; 未知扩展字段忽略 (含生字标注位) */
    {
        const char *p = "{\"items\":[{\"id\":\"c1\",\"title\":\"没日期\","
                        "\"newWordMarks\":[\"猫\"],\"extra\":1}]}";
        memset(&list, 0, sizeof(list));
        n = kidpal_story_list_parse(p, &list);
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            "KSTORY-A2 RED: payload with a missing date / unknown extra fields must parse "
            "(additive tolerance) — 不得因扩展位整包拒绝");
        TEST_ASSERT_TRUE_MESSAGE(strcmp(list.items[0].date, "") == 0,
            "KSTORY-A2 RED: missing date must parse as empty string");
    }

    /* degrade path: 畸形/缺字段/越界全部 -1（友好降级, 绝不盲渲染） */
    n = kidpal_story_list_parse("{\"total\":", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSTORY-A2 RED: malformed JSON must return -1 (degrade), not 0/partial");
    n = kidpal_story_list_parse(NULL, &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSTORY-A2 RED: NULL payload must be defended (-1)");
    n = kidpal_story_list_parse(KSTORY_LIST_CANONICAL, NULL);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSTORY-A2 RED: NULL out must be defended (-1)");
    n = kidpal_story_list_parse("{\"items\":\"x\"}", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSTORY-A2 RED: non-array items must be rejected (-1)");
    n = kidpal_story_list_parse("{}", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSTORY-A2 RED: data without items must be rejected (-1) — items 是契约 §1.3 ③ "
        "data 形状的必带字段");
    n = kidpal_story_list_parse("{\"items\":[{\"title\":\"无 id 的行\"}]}", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSTORY-A2 RED: item without id must be rejected (-1) — id 是回看/回听取正文的键");
    n = kidpal_story_list_parse("{\"items\":[{\"id\":\"d1\"}]}", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSTORY-A2 RED: item without title must be rejected (-1) — 行缺标题是畸形载荷");
    n = kidpal_story_list_parse("{\"total\":\"three\",\"items\":[]}", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSTORY-A2 RED: non-integer total must be rejected (-1) — 契约 total 为整数");
    n = kidpal_story_list_parse("{\"total\":-2,\"items\":[]}", &list);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSTORY-A2 RED: negative total must be rejected (-1) — 故事果计数不可能为负");

    /* cap: 超 KSTORY_LIST_MAX 条 -> -1（绝不静默丢行, 与 schedule 同纪律） */
    {
        char big[4096];
        size_t used = 0;
        int i, w;

        w = snprintf(big, sizeof(big), "{\"items\":[");
        if (w < 0) {
            w = 0;
        }
        used = (size_t)w;
        for (i = 0; i <= KSTORY_LIST_MAX; i++) {
            w = snprintf(big + used, sizeof(big) - used,
                         "%s{\"id\":\"x%d\",\"title\":\"故事%d\",\"date\":\"09-01\"}",
                         i == 0 ? "" : ",", i, i);
            if (w < 0 || (size_t)w >= sizeof(big) - used) {
                break;
            }
            used += (size_t)w;
        }
        snprintf(big + used, sizeof(big) - used, "]}");
        memset(&list, 0, sizeof(list));
        n = kidpal_story_list_parse(big, &list);
        TEST_ASSERT_TRUE_MESSAGE(n == -1,
            "KSTORY-A2 RED: item count > KSTORY_LIST_MAX must return -1, never "
            "silently truncate (调用方须请求更小页)");
    }

    /* safe-truncation: 超长 title 不溢出、恒 NUL 结尾 */
    {
        const char *long_title =
            "{\"items\":[{\"id\":\"e1\",\"title\":\"很长很长很长很长很长很长很长很长"
            "很长很长很长很长很长很长很长很长很长很长很长很长很长的故事标题\","
            "\"date\":\"09-04\"}]}";
        memset(&list, 0, sizeof(list));
        n = kidpal_story_list_parse(long_title, &list);
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            "KSTORY-A2 RED: an overlong title must still parse (safe-truncate)");
        /* 不溢出 + 返回值与 out->count 一致 (真 assert() 逻辑守卫在扫描 helper 侧,
         * 被测面用 TEST_ASSERT: abort 会带走整个 runner, 掩盖其它齿的判定) */
        TEST_ASSERT_TRUE_MESSAGE(strlen(list.items[0].title) < sizeof(list.items[0].title),
            "KSTORY-A2 RED: overlong title overflowed the destination buffer");
        TEST_ASSERT_TRUE_MESSAGE(n == list.count,
            "KSTORY-A2 RED: return value must equal out->count (一致性不变式)");
    }
}

/* -------------------------------------------------------------------------- *
 * P3-A3 — 光标导航（behavioral, S-F2.1 + spec §4.0 页内特例）
 * KEY 短按 = 反白选中项下移（页内特例）; KEY 双击 = 进入阅读态（页内特例）;
 * 全局手势（KEY 长按回首页 / BOOT 单击翻页）不得被页内吞。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: 列表光标下移 + KEY 双击进阅读态 + 全局手势不吞 (P3-A3)",
          "[story_p3][P3-A3][linux]")
{
    kidpal_story_list_t list;
    kidpal_story_view_t v;
    int r, n;

    memset(&list, 0, sizeof(list));
    memset(&v, 0, sizeof(v));
    n = kidpal_story_list_parse(KSTORY_LIST_CANONICAL, &list);
    r = kidpal_story_view_init(&v, &list);
    kstory_require_implemented(r, "kidpal_story_view_init unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && n == 2,
        "KSTORY-A3 RED: setup must parse the canonical list and init the view");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_LIST && v.cursor == 0 &&
                                 v.count == 2 && v.counter == 3,
        "KSTORY-A3 RED: view_init must land on the LIST page with cursor 0, count 2 "
        "and counter from 契约 total (3) — 列表初始态缺失");

    /* KEY 短按: 反白选中项下移 (S-F2.1) */
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    kstory_require_implemented(r, "kidpal_story_page_input unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_CURSOR_NEXT && v.cursor == 1,
        "KSTORY-A3 RED: KEY 短按 in the list page must move the 反白 selection down "
        "one row (spec §4.0 页内特例: P3 列表=光标下移)");

    /* 到底回绕 (冻结决策 D1: 循环浏览保证前面的故事仍可达) */
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_CURSOR_NEXT && v.cursor == 0,
        "KSTORY-A3 RED: cursor must wrap round to the first row after the last one "
        "(否则末项之后的故事再也选不到)");

    /* 空列表: 不动, ACT_NONE */
    {
        kidpal_story_view_t empty;
        memset(&empty, 0, sizeof(empty));
        r = kidpal_story_view_init(&empty, NULL);
        TEST_ASSERT_TRUE_MESSAGE(r == 0 && empty.count == 0 && empty.counter == 0,
            "KSTORY-A3 RED: NULL list must init an empty list page (count=0, counter=0)");
        r = kidpal_story_page_input(&empty, KEY_ID_KEY, KEY_GESTURE_CLICK);
        TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_NONE && empty.cursor == 0,
            "KSTORY-A3 RED: KEY 短按 on an empty list must not move a phantom cursor");
        r = kidpal_story_page_input(&empty, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK);
        TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_NONE && empty.page == KSTORY_PAGE_LIST,
            "KSTORY-A3 RED: KEY 双击 on an empty list must not open a phantom reader");
    }

    /* KEY 双击: 进入阅读态 (S-F2.1 -> S-F2.3) */
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_OPEN_READER && v.page == KSTORY_PAGE_READER,
        "KSTORY-A3 RED: KEY 双击 on a selected story must enter the reading state "
        "(spec §4.0 页内特例: P3 列表=进入阅读/播放选中)");
    TEST_ASSERT_TRUE_MESSAGE(v.screen == 0,
        "KSTORY-A3 RED: opening the reader must start at the first screen");

    /* 全局手势不得被页内吞 (spec §4.0「KEY 长按 回首页 所有页面一致」) */
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A3 RED: KEY long press (global HOME) must NOT be consumed by the P3 page "
        "— 全局层被页内吞会破坏「所有页面一致」");
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A3 RED: BOOT click (global PAGE_NEXT) must NOT be handled inside P3");
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A3 RED: BOOT 长按 belongs to the reader special case only while a screen "
        "can still turn — on a non-reader page it must go to the global layer untouched");
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_NONE);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_NONE || r == KSA_ACT_GLOBAL,
        "KSTORY-A3 RED: a NONE gesture must be a no-op, never a state change");

    /* 越界键位 (模拟 PWR) / NULL 视图 -> 降级, 状态零污染 */
    {
        kidpal_story_view_t snapshot;
        memcpy(&snapshot, &v, sizeof(snapshot));
        r = kidpal_story_page_input(&v, KEY_ID_COUNT, KEY_GESTURE_CLICK);
        TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_DEGRADE,
            "KSTORY-A3 RED: out-of-range key id must return KSA_ACT_DEGRADE "
            "(PWR 不入交互面, SES-E3 同纪律)");
        TEST_ASSERT_TRUE_MESSAGE(memcmp(&snapshot, &v, sizeof(v)) == 0,
            "KSTORY-A3 RED: a refused gesture must leave the view UNTOUCHED (零污染)");
        r = kidpal_story_page_input(NULL, KEY_ID_KEY, KEY_GESTURE_CLICK);
        TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_DEGRADE,
            "KSTORY-A3 RED: NULL view must be defended (DEGRADE), not crash");
    }
}

/* -------------------------------------------------------------------------- *
 * P3-A4 — 录制上传归档的设备面（behavioral, S-F2.2 / 契约 §1.4 start_recording）
 * 说「我要讲故事」→ 云端判意图 → control/start_recording → 设备切录音态演出
 * （REC 闪点 + 大时长 + 声波）→ 按 KEY 停录 → 发 stop-accumulation 归档请求。
 * 真实采音/ASR/TTS 声学不在本冻结面（P3-E2: 采音殿后 9b）；包 3 的
 * SES_STATE_MANUAL_RECORDING 接合面作为 must-stay-green 守卫。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: control/start_recording 进录音态 + REC 演出 + 停录触发归档 (P3-A4)",
          "[story_p3][P3-A4][linux]")
{
    kidpal_story_list_t list;
    kidpal_story_view_t v;
    char tbuf[16];
    int r;

    memset(&list, 0, sizeof(list));
    memset(&v, 0, sizeof(v));
    (void)kidpal_story_list_parse(KSTORY_LIST_CANONICAL, &list);
    r = kidpal_story_view_init(&v, &list);
    kstory_require_implemented(r, "kidpal_story_view_init unimplemented");

    /* ① control/start_recording（包 2 冻结数值码 VRT_CTRL_START_RECORDING=2）*/
    r = kidpal_story_on_control(&v, VRT_CTRL_START_RECORDING);
    kstory_require_implemented(r, "kidpal_story_on_control unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        "KSTORY-A4 RED: control/start_recording was not consumed — 网关切累积态后设备"
        "必须进录音态 (契约 §1.4 start_recording / S-F2.2)");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_RECORDING,
        "KSTORY-A4 RED: start_recording did not switch the P3 view to the recording "
        "state (S-F2.2 p3-recording)");
    TEST_ASSERT_TRUE_MESSAGE(v.rec_seconds == 0,
        "KSTORY-A4 RED: entering recording must start the 大时长 at 0");
    TEST_ASSERT_TRUE_MESSAGE(v.counter == 3,
        "KSTORY-A4 RED: entering recording must NOT pre-increment the 故事果计数器 "
        "(冻结决策 D2: V31 取消路径下丢弃的故事不该占位)");

    /* ② 幂等：网关重试下发不得重置已录时长 */
    v.rec_seconds = 12;
    r = kidpal_story_on_control(&v, VRT_CTRL_START_RECORDING);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && v.page == KSTORY_PAGE_RECORDING &&
                                 v.rec_seconds == 12,
        "KSTORY-A4 RED: a repeated start_recording must be idempotent (page stays, "
        "已录时长不被清零)");

    /* ③ 录音态演出帧: REC 闪点在闪 (非频闪) + 声波在跳 + 确定性 */
    {
        int transitions = 0, wave_distinct = 0;
        int flash = 0, wave = 0;
        int prev_flash = -1, prev_wave = -1;
        unsigned ms;

        for (ms = 0u; ms <= 2000u; ms += 10u) {
            r = kidpal_story_rec_tick(&v, ms, &flash, &wave);
            kstory_require_implemented(r, "kidpal_story_rec_tick unimplemented");
            TEST_ASSERT_TRUE_MESSAGE(r == 0,
                "KSTORY-A4 RED: kidpal_story_rec_tick must succeed in the recording state");
            TEST_ASSERT_TRUE_MESSAGE(flash == 0 || flash == 1,
                "KSTORY-A4 RED: REC 闪点 must be a 0/1 phase (反白点亮/灭)");
            TEST_ASSERT_TRUE_MESSAGE(wave >= 0,
                "KSTORY-A4 RED: 声波相位 must be a non-negative marker (手绘声波在跳)");
            if (prev_flash >= 0 && flash != prev_flash) {
                transitions++;
            }
            if (prev_wave >= 0 && wave != prev_wave) {
                wave_distinct++;
            }
            prev_flash = flash;
            prev_wave = wave;
        }
        TEST_ASSERT_TRUE_MESSAGE(transitions >= 1,
            "KSTORY-A4 RED: the REC dot never blinks within 2s — REC 闪点缺失 (S-F2.2"
            "「REC 反白点」是「正在录」的可观察证据)");
        TEST_ASSERT_TRUE_MESSAGE(transitions * 100 <= 2000,
            "KSTORY-A4 RED: the REC dot flips faster than every 100ms — 频闪 (儿童视觉"
            "安全下界), 不是可辨识闪烁");
        TEST_ASSERT_TRUE_MESSAGE(wave_distinct >= 1,
            "KSTORY-A4 RED: the hand-drawn 声波 is static within 2s — 声波跳动缺失 "
            "(S-F2.2 声波 = 「还在录」的三层冗余反馈之一)");

        /* 确定性: 同一 elapsed_ms 恒得同值 */
        {
            int f1 = -1, w1 = -1, f2 = -1, w2 = -1;
            (void)kidpal_story_rec_tick(&v, 1234u, &f1, &w1);
            (void)kidpal_story_rec_tick(&v, 1234u, &f2, &w2);
            TEST_ASSERT_TRUE_MESSAGE(f1 == f2 && w1 == w2,
                "KSTORY-A4 RED: the recording performance frame is not deterministic in "
                "elapsed_ms (隐式全局状态会让演出不可测)");
        }
    }

    /* ④ 大时长「00:47 式」文本 (S-F2.2 示例) */
    v.rec_seconds = 47;
    memset(tbuf, 0, sizeof(tbuf));
    r = kidpal_story_rec_time(&v, tbuf, sizeof(tbuf));
    kstory_require_implemented(r, "kidpal_story_rec_time unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(tbuf, "00:47") == 0,
        "KSTORY-A4 RED: 大时长 must render 47s as \"00:47\" (spec S-F2.2 大时长 00:47 式)");
    v.rec_seconds = 75;
    memset(tbuf, 0, sizeof(tbuf));
    r = kidpal_story_rec_time(&v, tbuf, sizeof(tbuf));
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(tbuf, "01:15") == 0,
        "KSTORY-A4 RED: 大时长 must carry minutes (75s -> \"01:15\")");
    r = kidpal_story_rec_time(&v, tbuf, 4);
    TEST_ASSERT_TRUE_MESSAGE(r == -1,
        "KSTORY-A4 RED: a too-small buffer must be refused (-1), never truncated away "
        "or overflowed");
    r = kidpal_story_rec_time(NULL, tbuf, sizeof(tbuf));
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A4 RED: NULL view must be defended (-1)");

    /* ⑤ tick 防御: 非录音态 / NULL / out 参数可空 */
    {
        kidpal_story_view_t idle;
        memset(&idle, 0, sizeof(idle));
        (void)kidpal_story_view_init(&idle, &list);
        r = kidpal_story_rec_tick(&idle, 1000u, NULL, NULL);
        TEST_ASSERT_TRUE_MESSAGE(r == -1,
            "KSTORY-A4 RED: tick outside the recording state must be refused (-1)");
        r = kidpal_story_rec_tick(NULL, 1000u, NULL, NULL);
        TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A4 RED: NULL view must be defended (-1)");
        r = kidpal_story_rec_tick(&v, 500u, NULL, NULL);
        TEST_ASSERT_TRUE_MESSAGE(r == 0,
            "KSTORY-A4 RED: NULL flash/wave out params must be tolerated (只要时长)");
    }

    /* ⑥ 按 KEY「讲完啦」停录 -> 发 stop-accumulation 归档请求 (host 只测触发) */
    v.rec_seconds = 47;
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_STOP_RECORD,
        "KSTORY-A4 RED: KEY 短按 while recording must stop the recording "
        "(S-F2.2「按 KEY 讲完啦」)");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_LIST,
        "KSTORY-A4 RED: after 停录 the view must leave the recording state");
    TEST_ASSERT_TRUE_MESSAGE(v.archive_pend == 1,
        "KSTORY-A4 RED: 停录 must raise exactly one archive request (网关整段转写归档"
        "触发缺失; 真实采音/上传殿后 9b, P3-E2)");

    /* ⑦ V31 回退·录音中 KEY 长按取消 -> 丢弃、不归档 */
    r = kidpal_story_on_control(&v, VRT_CTRL_START_RECORDING);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && v.page == KSTORY_PAGE_RECORDING,
        "KSTORY-A4 RED: setup for the cancel path failed");
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_CANCEL_RECORD,
        "KSTORY-A4 RED: KEY 长按 while recording must cancel "
        "(V31 显式回退路径「录音中 KEY 长按取消」)");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_LIST && v.cancel_count == 1 &&
                                 v.archive_pend == 1,
        "KSTORY-A4 RED: a cancelled recording must DISCARD (不新增归档请求) — V31"
        "「已累积内容丢弃」; archive_pend 须停在取消前的值 1");
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A4 RED: BOOT 长按 in the recording state must stay global (页内零语义)");

    /* ⑧ 已知但不归故事页消费的 control / 未知 action */
    r = kidpal_story_on_control(&v, VRT_CTRL_END_SESSION);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "KSTORY-A4 RED: end_session must be reported as not-consumed-by-P3 (交给"
        "全局会话面), not mistaken for a recording switch");
    r = kidpal_story_on_control(&v, VRT_CTRL_STOP_SPEAKING);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && v.page == KSTORY_PAGE_LIST,
        "KSTORY-A4 RED: stop_speaking must not change the story view");
    r = kidpal_story_on_control(&v, VRT_CTRL_UNKNOWN);
    TEST_ASSERT_TRUE_MESSAGE(r == -1,
        "KSTORY-A4 RED: an unknown control action must degrade (-1), never switch pages");
    r = kidpal_story_on_control(NULL, VRT_CTRL_START_RECORDING);
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A4 RED: NULL view must be defended (-1)");

    /* ⑨ 包 3 接合面在世守卫 (host-runner-only: 独立 stub 未链接 .cc 时跳过)
     *    LISTENING --START_RECORDING--> MANUAL_RECORDING --STOP_RECORDING--> LISTENING */
    if (ses_machine_init != NULL && ses_machine_dispatch != NULL) {
        ses_machine_t m;
        const ses_transition_t *t;

        ses_machine_init(&m);
        t = ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
        TEST_ASSERT_TRUE_MESSAGE(t != NULL && t->accepted == 1 &&
                                     t->to == SES_STATE_LISTENING,
            "KSTORY-A4 RED(infra): IDLE --KEY 短按--> LISTENING broken — 包 3 会话底座"
            "被破坏");
        t = ses_machine_dispatch(&m, SES_EVT_START_RECORDING);
        TEST_ASSERT_TRUE_MESSAGE(t != NULL && t->accepted == 1 &&
                                     t->to == SES_STATE_MANUAL_RECORDING,
            "KSTORY-A4 RED(infra): LISTENING --START_RECORDING--> MANUAL_RECORDING broken "
            "— 包 3 为本包预留的 manual 录音接合面不得回退");
        t = ses_machine_dispatch(&m, SES_EVT_START_RECORDING);
        TEST_ASSERT_TRUE_MESSAGE(t != NULL && t->accepted == 0,
            "KSTORY-A4 RED(infra): 累积态重复切入必须被拒绝 (包 3 冻结表)");
        t = ses_machine_dispatch(&m, SES_EVT_STOP_RECORDING);
        TEST_ASSERT_TRUE_MESSAGE(t != NULL && t->accepted == 1 &&
                                     t->to == SES_STATE_LISTENING,
            "KSTORY-A4 RED(infra): MANUAL_RECORDING --STOP_RECORDING--> LISTENING broken "
            "— 停录退回聆听态的接合面缺失");
    }
    /* 全局层仍把 KEY 长按路由到 HOME: 页内取消是附加副作用, 不是吞全局 */
    if (key_route_global != NULL) {
        TEST_ASSERT_TRUE_MESSAGE(
            key_route_global(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS) == GLOBAL_ACT_HOME,
            "KSTORY-A4 RED(infra): KEY 长按 的全局 HOME 路由被改动 — 包 3 冻结手势表"
            "(spec §4.0 所有页面一致) 不得偏移");
    }
}

/* -------------------------------------------------------------------------- *
 * P3-A5 — 回看/回听（behavioral, S-F2.3 / 契约 §1.3 ④）
 * story_text 解析 {id,title,text}（无生字标注字段, P3-E4）+ 阅读态正文滚动分屏
 * （全文不丢）+ BOOT 长按翻下一屏 + 「第 X 屏 · 共 N 屏」+ KEY 朗读正文（TTS 触发）。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: 阅读态正文分屏 + BOOT 长按翻屏 + KEY 朗读触发 (P3-A5)",
          "[story_p3][P3-A5][linux]")
{
    kidpal_story_text_t st;
    kidpal_story_view_t v;
    char label[64];
    char expect_label[64];
    int r, pages10;

    memset(&st, 0, sizeof(st));
    memset(&v, 0, sizeof(v));

    /* ① story_text 载荷解析（契约 §1.3 ④ 逐字示例） */
    r = kidpal_story_text_parse(KSTORY_TEXT_CANONICAL, &st);
    kstory_require_implemented(r, "kidpal_story_text_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "KSTORY-A5 RED: canonical 契约 §1.3 ④ payload must parse");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(st.id, "2026-08-27-maoxian") == 0 &&
                                 strcmp(st.title, "我的大冒险") == 0 &&
                                 strstr(st.text, "从前有一只小猫") != NULL,
        "KSTORY-A5 RED: {id,title,text} must be preserved verbatim (契约 §1.3 ④)");

    /* ② 降级路径 + P3-E4 扩展位不消费 */
    r = kidpal_story_text_parse(NULL, &st);
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A5 RED: NULL payload must be defended (-1)");
    r = kidpal_story_text_parse("{\"id\":\"x\"}", &st);
    TEST_ASSERT_TRUE_MESSAGE(r == -1,
        "KSTORY-A5 RED: missing text must be rejected (-1) — 无正文不可阅读");
    r = kidpal_story_text_parse("{\"text\":\"从前…\"}", &st);
    TEST_ASSERT_TRUE_MESSAGE(r == -1,
        "KSTORY-A5 RED: missing id must be rejected (-1) — id 是回看/回听的键");
    r = kidpal_story_text_parse(KSTORY_TEXT_CANONICAL, NULL);
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A5 RED: NULL out must be defended (-1)");
    {
        kidpal_story_text_t no_title;
        memset(&no_title, 0, sizeof(no_title));
        r = kidpal_story_text_parse(
            "{\"id\":\"z1\",\"text\":\"从前有只猫\",\"newWordMarks\":[\"猫\"],"
            "\"story_highlight\":[0,1]}", &no_title);
        TEST_ASSERT_TRUE_MESSAGE(r == 0,
            "KSTORY-A5 RED: unknown extension fields (含生字标注位) must be tolerated — "
            "契约 data 形状不锁扩展位, 但设备侧一律不消费 (P3-E4 已裁不做反白高亮)");
        TEST_ASSERT_TRUE_MESSAGE(strcmp(no_title.title, "") == 0,
            "KSTORY-A5 RED: a missing title must parse as empty string (容错, 非致命)");
    }

    /* ③ 进阅读态 + 分页（正文全文不丢: screens * screen_lines >= 行数） */
    memset(&v, 0, sizeof(v));
    (void)kidpal_story_view_init(&v, NULL);
    memset(&st, 0, sizeof(st));
    snprintf(st.id, sizeof(st.id), "s1");
    snprintf(st.title, sizeof(st.title), "我的大冒险");
    snprintf(st.text, sizeof(st.text), "%s", KSTORY_TEXT_TEN_LINES);
    r = kidpal_story_open_reader(&v, &st, 4);
    kstory_require_implemented(r, "kidpal_story_open_reader unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && v.page == KSTORY_PAGE_READER && v.screen == 0,
        "KSTORY-A5 RED: opening the reader must land on page READER at screen 0");
    pages10 = v.screens;
    TEST_ASSERT_TRUE_MESSAGE(pages10 >= 3,
        "KSTORY-A5 RED: 10 行正文 at 4 行/屏 must span >= 3 screens — 正文滚动/分屏缺失 "
        "(S-F2.3 底部「第 1 屏 · 共 3 屏」)");
    TEST_ASSERT_TRUE_MESSAGE(pages10 * 4 >= 10,
        "KSTORY-A5 RED: screens*screen_lines < 行数 — 分屏丢了正文 (全文上屏不丢内容, "
        "与包 2 kidpal_display_text_page_count 同纪律)");

    /* ④ 「第 X 屏 · 共 N 屏」逐字（S-F2.3 底部文案） */
    memset(label, 0, sizeof(label));
    r = kidpal_story_screen_label(&v, label, sizeof(label));
    kstory_require_implemented(r, "kidpal_story_screen_label unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "KSTORY-A5 RED: screen label must render");
    snprintf(expect_label, sizeof(expect_label), "第 1 屏 · 共 %d 屏", pages10);
    /* 判定走 TEST_ASSERT（体内 assert() 一旦不满足会 abort 整个 runner、
     * 掩盖其余齿的判定；真 assert() 逻辑守卫在扫描 helper 侧） */
    TEST_ASSERT_TRUE_MESSAGE(strcmp(label, expect_label) == 0,
        "KSTORY-A5 RED: 底部屏数指示必须逐字为「第 1 屏 · 共 N 屏」(spec S-F2.3)");

    /* ⑤ BOOT 长按翻下一屏（页内特例）+ 末屏钳制（冻结决策 D1） */
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_PAGE_TURN && v.screen == 1,
        "KSTORY-A5 RED: BOOT 长按 in the reader must turn to the next screen "
        "(spec §4.0 页内特例: P3 阅读=翻下一屏)");
    memset(label, 0, sizeof(label));
    r = kidpal_story_screen_label(&v, label, sizeof(label));
    snprintf(expect_label, sizeof(expect_label), "第 2 屏 · 共 %d 屏", pages10);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(label, expect_label) == 0,
        "KSTORY-A5 RED: after one turn the label must read 第 2 屏 · 共 N 屏 (翻页与"
        "屏数指示联动, S-F2.3)");
    for (r = 0; r < 6; r++) {
        (void)kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    }
    TEST_ASSERT_TRUE_MESSAGE(v.screen == v.screens - 1,
        "KSTORY-A5 RED: the reader must CLAMP at the last screen (进度指示不丢, 冻结决策 "
        "D1) — 末屏再翻不得回到第 1 屏");
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_NONE,
        "KSTORY-A5 RED: turning past the last screen must report a no-op, not a wrap");

    /* ⑥ KEY 朗读正文（TTS 触发标志; 声学殿后 9b） */
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_TTS_READ && v.tts_requests == 1,
        "KSTORY-A5 RED: KEY 短按 in the reader must raise the「念给我听」TTS request "
        "(spec §4.0 页内特例: P3 阅读=念给我听; host 只测触发不测声学, P3-E2)");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_READER,
        "KSTORY-A5 RED: 朗读 must not change the page/screen state");
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(v.tts_requests == 2,
        "KSTORY-A5 RED: repeated 朗读 must count (每次按都发一次朗读请求)");
    /* 阅读态的全局手势照常不吞 */
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A5 RED: KEY 长按 in the reader must stay global (回首页, 所有页面一致)");
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A5 RED: BOOT 单击 in the reader must stay global (翻页 P1→P2→P3→P4)");
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A5 RED: KEY 双击 in the reader has no page-local meaning — must not "
        "re-enter the reader or swallow the gesture");

    /* ⑦ 分页单调 + 非法参数防御（不除零、不盲渲染） */
    {
        kidpal_story_view_t v2;
        kidpal_story_text_t longer;

        memset(&v2, 0, sizeof(v2));
        memset(&longer, 0, sizeof(longer));
        snprintf(longer.id, sizeof(longer.id), "s2");
        snprintf(longer.text, sizeof(longer.text), "%s\n第十一\n第十二\n第十三",
                 KSTORY_TEXT_TEN_LINES);
        r = kidpal_story_open_reader(&v2, &longer, 4);
        TEST_ASSERT_TRUE_MESSAGE(r == 0 && v2.screens >= pages10,
            "KSTORY-A5 RED: page count is not monotonic in text length — 分页不稳定");
        memset(&v2, 0, sizeof(v2));
        r = kidpal_story_open_reader(&v2, &st, 0);
        TEST_ASSERT_TRUE_MESSAGE(r == 0 || r == -1,
            "KSTORY-A5 RED: screen_lines=0 must be defended (1 屏 或显式 -1), "
            "绝不除零崩溃");
        r = kidpal_story_open_reader(&v, NULL, 4);
        TEST_ASSERT_TRUE_MESSAGE(r == -1,
            "KSTORY-A5 RED: opening the reader without a story payload must degrade (-1), "
            "never blind-render (V27 / spec §4.0)");
        r = kidpal_story_open_reader(NULL, &st, 4);
        TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A5 RED: NULL view must be defended (-1)");
    }
    /* ⑧ 屏数指示由视图态派生 (0-based screen -> 1-based display) */
    {
        kidpal_story_view_t one;
        memset(&one, 0, sizeof(one));
        one.page = KSTORY_PAGE_READER;
        one.screen = 1;
        one.screens = 5;
        memset(label, 0, sizeof(label));
        r = kidpal_story_screen_label(&one, label, sizeof(label));
        TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(label, "第 2 屏 · 共 5 屏") == 0,
            "KSTORY-A5 RED: the label must derive from the view state");
        r = kidpal_story_screen_label(&one, label, 4);
        TEST_ASSERT_TRUE_MESSAGE(r == -1,
            "KSTORY-A5 RED: a too-small label buffer must be refused (-1), never overflow");
        r = kidpal_story_screen_label(NULL, label, sizeof(label));
        TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A5 RED: NULL view must be defended (-1)");
    }

    /* ⑨ 路由续接不回退 (host-runner-only): story_text/story_list 仍指包 2 数值码 */
    if (kidpal_display_route != NULL) {
        r = kidpal_display_route("story_text");
        TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_STORY_TEXT,
            "KSTORY-A5 RED: view story_text no longer routes to VRT_ROUTE_STORY_TEXT — "
            "包 2 共享路由底座续接被破坏 (契约 §1.3 ④)");
        r = kidpal_display_route("story_list");
        TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_STORY_LIST,
            "KSTORY-A5 RED: view story_list no longer routes to VRT_ROUTE_STORY_LIST — "
            "包 2 共享路由底座续接被破坏 (契约 §1.3 ③)");
    }
}

/* -------------------------------------------------------------------------- *
 * P3-A6 — 成就盖章帧（behavioral, S-F2.4 + decision contract-§一-story_list-total）
 * 保存成功那一瞬: 双圈印章（外实内虚、斜盖 -8°）+「故事徽章·第 N 颗已入册！」+
 * pose-jump 定格 0.8s；N 与 P3 故事果计数器严格同步（闸 A 留记①: 板子持 total →
 * 保存后本地推 +1 → 随后 story_list 刷新以云端权威对齐 — 钉同步不变式, 不钉云端值）。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: 盖章帧双圈/-8°/0.8s + 文案 N 与计数器严格同步 (P3-A6)",
          "[story_p3][P3-A6][linux]")
{
    kidpal_story_list_t list;
    kidpal_story_view_t v;
    kidpal_story_stamp_spec_t spec;
    char txt[64];
    char expect[64];
    int r;

    memset(&list, 0, sizeof(list));
    memset(&v, 0, sizeof(v));
    memset(&spec, 0, sizeof(spec));
    (void)kidpal_story_list_parse(KSTORY_LIST_CANONICAL, &list); /* total=3 */
    r = kidpal_story_view_init(&v, &list);
    kstory_require_implemented(r, "kidpal_story_view_init unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(v.counter == 3,
        "KSTORY-A6 RED(setup): the P3 counter must start from the 契约 total (3)");

    /* ① 盖章几何冻结 (S-F2.4: 双圈/外实内虚/斜盖 -8°/定格 0.8s) */
    r = kidpal_story_stamp_spec(&spec);
    kstory_require_implemented(r, "kidpal_story_stamp_spec unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "KSTORY-A6 RED: stamp spec must be readable");
    TEST_ASSERT_TRUE_MESSAGE(spec.rings == 2,
        "KSTORY-A6 RED: 印章 must be 双圈 (rings == 2) — S-F2.4 双圈印章");
    TEST_ASSERT_TRUE_MESSAGE(spec.inner_dashed == 1,
        "KSTORY-A6 RED: 内圈 must be 虚线 (外实内虚) — S-F2.4 冻结形态");
    TEST_ASSERT_TRUE_MESSAGE(spec.angle_deci == KSA_STAMP_ANGLE_DECI,
        "KSTORY-A6 RED: 印章倾角必须为 -8° (十分度 -80) — S-F2.4「斜盖 −8°」");
    TEST_ASSERT_TRUE_MESSAGE(spec.hold_ms == KSA_STAMP_HOLD_MS,
        "KSTORY-A6 RED: pose-jump 定格时长必须为 800ms — S-F2.4「定格 0.8 秒」");
    r = kidpal_story_stamp_spec(NULL);
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A6 RED: NULL out must be defended (-1)");

    /* ② 保存成功 ack -> 盖章帧 + N 与计数器严格同步 */
    r = kidpal_story_on_saved(&v, "2026-09-08-xiaomiao");
    kstory_require_implemented(r, "kidpal_story_on_saved unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "KSTORY-A6 RED: a save-success ack must be accepted — 盖章触发缺失 (S-F2.2"
        "「上传成功那一瞬」-> S-F2.4)");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_STAMP,
        "KSTORY-A6 RED: save success must switch to the stamp frame (p3-stamp-frame)");
    TEST_ASSERT_TRUE_MESSAGE(v.stamp_index == 4 && v.counter == 4,
        "KSTORY-A6 RED: 盖章 N 必须 = 已知 total + 1 且与 P3 故事果计数器严格同步 "
        "(S-F2.4「N 与 P3 故事果计数器严格同步」/ 闸 A 留记① 本地推)");

    /* ③ 盖章文案逐字 (S-F2.4) + N == counter 不变式 */
    memset(txt, 0, sizeof(txt));
    r = kidpal_story_stamp_text(&v, txt, sizeof(txt));
    kstory_require_implemented(r, "kidpal_story_stamp_text unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "KSTORY-A6 RED: stamp copy must render");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(txt, "故事徽章·第 4 颗已入册！") == 0,
        "KSTORY-A6 RED: 盖章文案必须逐字为「故事徽章·第 4 颗已入册！」(spec S-F2.4)");
    snprintf(expect, sizeof(expect), "故事徽章·第 %d 颗已入册！", v.counter);
    TEST_ASSERT_TRUE_MESSAGE(strcmp(txt, expect) == 0,
        "KSTORY-A6 RED: 文案里的 N 与 counter 不一致 — 严格同步不变式被破坏");
    r = kidpal_story_stamp_text(&v, txt, 6);
    TEST_ASSERT_TRUE_MESSAGE(r == -1,
        "KSTORY-A6 RED: a too-small copy buffer must be refused (-1), never overflow");
    r = kidpal_story_stamp_text(NULL, txt, sizeof(txt));
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A6 RED: NULL view must be defended (-1)");

    /* ④ 幂等: 同一 saved_id 的重复 ack 不得重复发徽章 */
    r = kidpal_story_on_saved(&v, "2026-09-08-xiaomiao");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && v.counter == 4 && v.stamp_index == 4,
        "KSTORY-A6 RED: a repeated save ack for the SAME story must be idempotent — "
        "mcp 回执重试不得让孩子的徽章凭空多一颗");
    /* 新故事再存 -> 再推一颗, 仍与计数器同步 */
    r = kidpal_story_on_saved(&v, "2026-09-09-xiaogou");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && v.counter == 5 && v.stamp_index == 5,
        "KSTORY-A6 RED: a NEW saved story must push the counter exactly one (同步不变式)");

    /* ⑤ 刷新对齐: 随后 story_list 下发以云端权威 total 覆盖本地推值 */
    {
        kidpal_story_list_t fresh;
        memset(&fresh, 0, sizeof(fresh));
        r = kidpal_story_list_parse("{\"total\":6,\"items\":["
                                    "{\"id\":\"f1\",\"title\":\"一\",\"date\":\"09-06\"},"
                                    "{\"id\":\"f2\",\"title\":\"二\",\"date\":\"09-07\"}]}",
                                    &fresh);
        TEST_ASSERT_TRUE_MESSAGE(r == 2 && fresh.total == 6,
            "KSTORY-A6 RED: a paginated refresh payload (total=6, 2 items) must parse");
        memset(&v, 0, sizeof(v));
        r = kidpal_story_view_init(&v, &fresh);
        TEST_ASSERT_TRUE_MESSAGE(r == 0 && v.counter == 6,
            "KSTORY-A6 RED: after 刷新 the counter must take the 云端权威 total (6) — "
            "本地推值不跨刷新保留 (闸 A 留记① 现解: 板子持 total→本地推→刷新对齐)");
    }

    /* ⑥ 防御路径: 畸形 ack 零状态污染 */
    r = kidpal_story_on_saved(&v, NULL);
    TEST_ASSERT_TRUE_MESSAGE(r == -1 && v.counter == 6,
        "KSTORY-A6 RED: NULL saved_id must be refused (-1) with zero state pollution");
    r = kidpal_story_on_saved(&v, "");
    TEST_ASSERT_TRUE_MESSAGE(r == -1 && v.counter == 6 && v.page == KSTORY_PAGE_LIST,
        "KSTORY-A6 RED: an empty saved_id must be refused (-1) — 无键的保存 ack 是畸形, "
        "不得盖出第 7 颗");
    r = kidpal_story_on_saved(NULL, "x");
    TEST_ASSERT_TRUE_MESSAGE(r == -1, "KSTORY-A6 RED: NULL view must be defended (-1)");

    /* ⑦ 盖章帧不吃页内输入（全局手势仍交给全局层） */
    (void)kidpal_story_on_saved(&v, "2026-09-10-render");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSTORY_PAGE_STAMP,
        "KSTORY-A6 RED(setup): must be on the stamp page");
    r = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL,
        "KSTORY-A6 RED: on the stamp frame KEY 短按 must go to the global layer "
        "(盖章 0.8s 演出内 P3 无页内语义)");
    r = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(r == KSA_ACT_GLOBAL && v.page == KSTORY_PAGE_STAMP,
        "KSTORY-A6 RED: BOOT 长按 on the stamp frame must not turn pages");
}

/* -------------------------------------------------------------------------- *
 * P3-A2/A3/A4/A5/A6 render face — host-runner-only LVGL 软渲染真帧（渲染证据）。
 * 机制: 先经包 0 冻结渲染缝 host_infra_render_frame() 初始化 LVGL 离屏显示并置
 * rendered 态, 再调被测渲染契约函数在活动屏上真画, lv_refr_now 刷新后用
 * host_infra_read_pixel 采样网格探针断言真实像素。requires lvgl.h — 独立红基线
 * stub 编译无 lvgl include path, 本测试被编译排除（如实记录, 其红态在 builder
 * 接线进 host runner 时兑现）。RLCD 真实屏上观感殿后包 9b (P3-E2 口径)。
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("lvgl.h")
#define KSTORY_HAVE_LVGL 1
#include <lvgl.h>
#endif
#endif

#ifdef KSTORY_HAVE_LVGL

extern int host_infra_render_frame(void);
extern int32_t host_infra_read_pixel(int x, int y);

#define KSTORY_GRID_STEP 20
#define KSTORY_GRID_W (400 / KSTORY_GRID_STEP)
#define KSTORY_GRID_H (300 / KSTORY_GRID_STEP)

static void kstory_sample_grid(int32_t *out)
{
    int i = 0;
    for (int y = KSTORY_GRID_STEP / 2; y < 300; y += KSTORY_GRID_STEP) {
        for (int x = KSTORY_GRID_STEP / 2; x < 400; x += KSTORY_GRID_STEP) {
            out[i++] = host_infra_read_pixel(x, y);
        }
    }
}

static void kstory_render_and_sample(const kidpal_story_view_t *v,
                                     const kidpal_story_list_t *list,
                                     const kidpal_story_text_t *story, int32_t *grid)
{
    int n = kidpal_story_page_render(v, list, story);
    kstory_require_implemented(n, "kidpal_story_page_render unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KSTORY render RED: the story render face did not report a drawn frame");
    lv_refr_now(NULL);
    kstory_sample_grid(grid);
}

TEST_CASE("story p3: P3 页渲染面真帧——反白/计数/翻页/盖章可见差异 (P3-A2/A3/A4/A5/A6 render)",
          "[story_p3][P3-A2][render][linux]")
{
    static int32_t grid_list0[KSTORY_GRID_W * KSTORY_GRID_H];
    static int32_t grid_list1[KSTORY_GRID_W * KSTORY_GRID_H];
    static int32_t grid_list10[KSTORY_GRID_W * KSTORY_GRID_H];
    static int32_t grid_reader0[KSTORY_GRID_W * KSTORY_GRID_H];
    static int32_t grid_reader1[KSTORY_GRID_W * KSTORY_GRID_H];
    static int32_t grid_rec[KSTORY_GRID_W * KSTORY_GRID_H];
    static int32_t grid_stamp[KSTORY_GRID_W * KSTORY_GRID_H];
    kidpal_story_list_t list;
    kidpal_story_list_t big_list;
    kidpal_story_text_t story;
    kidpal_story_view_t v;
    int dark = 0, diff = 0, uniform_fill = 1, r, i;

    /* 包 0 冻结渲染缝: 初始化 + rendered 态 (fill 0x336699)。 */
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KSTORY render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    memset(&list, 0, sizeof(list));
    memset(&story, 0, sizeof(story));
    (void)kidpal_story_list_parse(KSTORY_LIST_CANONICAL, &list);
    (void)kidpal_story_view_init(&v, &list);

    /* ① 列表态 (cursor 0): 内容真的画到屏上 */
    kstory_render_and_sample(&v, &list, NULL, grid_list0);
    for (i = 0; i < KSTORY_GRID_W * KSTORY_GRID_H; i++) {
        if (grid_list0[i] != 0x336699) {
            uniform_fill = 0;
        }
        if (grid_list0[i] < 0xC0C0C0) {
            dark++;
        }
    }
    TEST_ASSERT_FALSE_MESSAGE(uniform_fill,
        "KSTORY-A2 render RED: frame still uniform seam-fill after render — 渲染面没有"
        "真的画到屏上 (P3-A2 列表渲染缺失)");
    TEST_ASSERT_TRUE_MESSAGE(dark >= 5,
        "KSTORY-A2 render RED: too few dark pixels — 标题/×N 计数/每行图标·标题·日期"
        "内容缺失 (P3-A2 列表内容面)");

    /* ② 选中项反白可见: cursor 0 vs 1 的真帧必须不同 */
    (void)kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    kstory_render_and_sample(&v, &list, NULL, grid_list1);
    diff = 0;
    for (i = 0; i < KSTORY_GRID_W * KSTORY_GRID_H; i++) {
        if (grid_list0[i] != grid_list1[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KSTORY-A3 render RED: cursor 0 and cursor 1 frames are pixel-identical — 选中项"
        "反白在渲染面上不可见 (P3-A2/S-F2.1「选中项反白+微倾」)");

    /* ③ 故事果计数器 ×N 可见: 同列表 total 3 vs 10 的真帧必须不同 */
    memset(&big_list, 0, sizeof(big_list));
    memset(&v, 0, sizeof(v));
    r = kidpal_story_list_parse("{\"total\":10,\"items\":["
                                "{\"id\":\"a\",\"title\":\"小兔子找朋友\",\"date\":\"08-20\"},"
                                "{\"id\":\"b\",\"title\":\"勇敢的小明\",\"date\":\"08-25\"}]}",
                                &big_list);
    TEST_ASSERT_TRUE_MESSAGE(r == 2 && big_list.total == 10,
        "KSTORY render RED(setup): total=10 payload must parse");
    (void)kidpal_story_view_init(&v, &big_list);
    kstory_render_and_sample(&v, &big_list, NULL, grid_list10);
    diff = 0;
    for (i = 0; i < KSTORY_GRID_W * KSTORY_GRID_H; i++) {
        if (grid_list0[i] != grid_list10[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KSTORY-A2 render RED: the ×3 and ×10 frames are pixel-identical — 右上角故事果"
        "计数器未渲染 (P3-A2「×N」)");

    /* ④ 阅读态翻页可见差异 (第 1 屏 vs 第 2 屏) */
    memset(&v, 0, sizeof(v));
    (void)kidpal_story_view_init(&v, &list);
    snprintf(story.id, sizeof(story.id), "r1");
    snprintf(story.title, sizeof(story.title), "我的大冒险");
    snprintf(story.text, sizeof(story.text), "%s", KSTORY_TEXT_TEN_LINES);
    r = kidpal_story_open_reader(&v, &story, 4);
    kstory_require_implemented(r, "kidpal_story_open_reader unimplemented");
    kstory_render_and_sample(&v, &list, &story, grid_reader0);
    (void)kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    kstory_render_and_sample(&v, &list, &story, grid_reader1);
    diff = 0;
    for (i = 0; i < KSTORY_GRID_W * KSTORY_GRID_H; i++) {
        if (grid_reader0[i] != grid_reader1[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KSTORY-A5 render RED: screen 1 and screen 2 frames are pixel-identical — 正文"
        "滚动/分屏在渲染面上不可见 (P3-A5/S-F2.3)");

    /* ⑤ 录音态 / 盖章态各自与前一态互异（四态各有画面） */
    memset(&v, 0, sizeof(v));
    (void)kidpal_story_view_init(&v, &list);
    r = kidpal_story_on_control(&v, VRT_CTRL_START_RECORDING);
    kstory_require_implemented(r, "kidpal_story_on_control unimplemented");
    v.rec_seconds = 47;
    kstory_render_and_sample(&v, &list, NULL, grid_rec);
    diff = 0;
    for (i = 0; i < KSTORY_GRID_W * KSTORY_GRID_H; i++) {
        if (grid_rec[i] != grid_list0[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KSTORY-A4 render RED: the recording frame equals the list frame — REC 演出"
        "(闪点/大时长/声波) 未渲染 (P3-A4/S-F2.2)");

    (void)kidpal_story_on_saved(&v, "2026-09-08-render");
    kstory_render_and_sample(&v, &list, NULL, grid_stamp);
    diff = 0;
    for (i = 0; i < KSTORY_GRID_W * KSTORY_GRID_H; i++) {
        if (grid_stamp[i] != grid_rec[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KSTORY-A6 render RED: the stamp frame equals the recording frame — 盖章帧"
        "(双圈印章 + 文案 + pose-jump) 未渲染 (P3-A6/S-F2.4)");

    /* ⑥ 非法参数: 不崩、显式降级 */
    {
        int n = kidpal_story_page_render(NULL, &list, NULL);
        TEST_ASSERT_TRUE_MESSAGE(n == -1,
            "KSTORY render RED: NULL view must be rejected (-1), never crash");
        lv_refr_now(NULL);
        memset(&v, 0, sizeof(v));
        (void)kidpal_story_view_init(&v, NULL);
        n = kidpal_story_page_render(&v, NULL, NULL); /* LIST page without its list */
        TEST_ASSERT_TRUE_MESSAGE(n == -1,
            "KSTORY render RED: the list page without payload data must degrade (-1), "
            "never blind-render (V27 / spec §4.0)");
        lv_refr_now(NULL);
        (void)kidpal_story_view_init(&v, &list);
        n = kidpal_story_page_render(&v, &list, NULL);
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            "KSTORY-A2 render RED: a valid list page must render (return 0)");
        lv_refr_now(NULL);
    }
}

#endif /* KSTORY_HAVE_LVGL */

/* -------------------------------------------------------------------------- *
 * boundary / security — 扫描器自检（防假绿）+ 本包范围红线守卫
 *   - scanner integrity: real file exists / bogus file does not / unreadable
 *     path reads NULL / no-match needle matches nothing / counter counts a
 *     known literal — the negative teeth elsewhere must be CAPABLE of failing;
 *   - P3-E3: S-F2.5 云端润色 = 可选、MVP 后置, 不进本包 (固件零润色引用面);
 *   - P3-E4: 阅读态「生字反白高亮」已裁 (零高亮态引用、零 story_text 扩展字段消费);
 *   - 归档面归属: stories 目录 MD 由云端网关落盘, 设备树零引用 (哑终端纯洁性);
 *   - 设备不直连 OpenClaw (WSA/VRT/KSP 守卫对偶, must stay green).
 * ------------------------------------------------------------------------ */
TEST_CASE("story p3: 扫描器自检 + 润色/生字高亮/归档归属守卫 (boundary/security)",
          "[story_p3][boundary][security][linux]")
{
    char hit[KSTORY_MAX_PATH];
    int n;

    KSTORY_REQUIRE_ROOT();

    /* scanner integrity */
    TEST_ASSERT_TRUE_MESSAGE(kstory_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(kstory_exists("firmware/main/display/__kstory_no_such__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(kstory_read("firmware/main/display/__kstory_no_such__.c", NULL),
        "boundary RED(infra): kstory_read returned data for a nonexistent file");
    n = kstory_scan_count("firmware/main/display", "__kstory_no_such_needle__", 0, hit,
                          sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = kstory_count_in_file("firmware/main/display/view_router.c", "VRT_ROUTE_STORY_LIST");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");

    /* P3-E3: S-F2.5 云端润色为可选、MVP 后置——设备侧零润色面。
     * (只用中文关键词: 大小写不敏感的 "polish" 会撞上 Kconfig.projbuild 里的
     *  TTS 语言名 Polish, 与本包无关——先例教训: 负向齿必须先是「能绿」的。) */
    n = kstory_scan_count("firmware/main", "润色", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "P3-E3 RED: firmware references 润色 — S-F2.5 云端润色为可选项、MVP 后置, 不进"
        "本包必做面 (PROPOSAL §7 留根); 出现即范围越界");

    /* P3-E4: 阅读态不做生字反白高亮 (2026-08-29 human 裁定, 需 story_text 增生字字段) */
    n = kstory_scan_count("firmware/main", "story_highlight", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "P3-E4 RED: firmware references story_highlight — 生字反白高亮已裁, 不得引入"
        "自造高亮态 (双源漂移); 阅读态问生字走语音跳 P4 (S-F2.3)");
    n = kstory_scan_count("firmware/main", "newWordMarks", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "P3-E4 RED: firmware consumes a 生字标注 payload field — 契约 story_text 无此"
        "字段、本包不消费 (闸 A 判 ④ 零契约改动)");

    /* 归档归属: stories 目录 MD 落盘在云侧网关, 设备只消费下发的 story_list/story_text */
    n = kstory_scan_count("firmware/main", "stories/", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED: firmware references the stories archive path — 归档落盘归云端网关"
        "(server/acceptance/test_story_p3.acceptance.py 承载), 设备侧出现即双源漂移");

    /* 设备不直连 OpenClaw（WSA/VRT/KSP 守卫对偶, must stay green） */
    n = kstory_scan_count("firmware/main", "openclaw", 1, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references openclaw directly — 设备只连自建网关");
    n = kstory_scan_count("firmware/main", "18789", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references port 18789 — OpenClaw 端口不得出现在设备侧");

    /* 契约 §1.4 start_recording 到达面在世 (包 2 冻结, 本包只消费不改数值码) */
    if (kidpal_display_control_action != NULL) {
        n = kidpal_display_control_action("start_recording");
        TEST_ASSERT_TRUE_MESSAGE(n == VRT_CTRL_START_RECORDING,
            "boundary RED(infra): control/start_recording no longer parses to "
            "VRT_CTRL_START_RECORDING — 契约 §1.4 冻结动作面被破坏 (包 8 录音接合入口)");
    }
}

/* --------------------------------------------------------------------------- *
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-08  initial freeze (acceptance-author). 覆盖 P3-A1..A6 设备可观察面
 *               + P3-E2/E3/E4 边界; 冻结契约 14 函数 / 4 页态 / 9 动作码 /
 *               盖章几何 4 常量; 真实采音·TTS 声学·真 ASR 显式殿后 9b (P3-E2)。
 * --------------------------------------------------------------------------- */
