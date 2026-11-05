/*
 * Frozen acceptance tests — TaskPackage 261104-k2-points-device-ui（host 面）。
 * (subsystem: firmware/main——display/points_page.h/.c 积分主页四态判定/进度线/
 *  键位决策表/删除确认子态隐式 ack/keyset 分页客户端状态/selected_id 注入决策 +
 *  display/keybar.c/h 积分子态键帽三槽 + mcp_server.cc 消费接线扩展 +
 *  protocols/protocol.h/.cc kidpal_points_delete 删除上行驱动方 +
 *  application.cc/protocols 积分语境（points_ledger + selected_id）上行构造;
 *  rigor=full; riskTier=high)。
 *
 * Story 语义权威 = docs/requirements.md v1.71 S-F6.1（看积分进度与最近记录
 * AC1–AC6）/ S-F6.2（浏览详细账本与删除误记 AC1–AC5）/ §2.3（按键手势权威矩阵：
 * P3 积分主页行 / P3 详细账本行 / P3·账本删除确认子态行 / 消歧注记）/
 * NFR-OFF-1（积分断网边界）+ FROZEN PROPOSAL 261103-star-points-story-cover
 * （D-02 单页主页与完整账本子页 / D-03 删除重算负余额如实显示 / D-04 完整语音句
 * 短窗去重设备语音不开放删除 / D-07 restore 不抢前台主动语音例外跳页 / D-08
 * session_ctx·删除·分页私有帧消费）+ 契约 docs/contracts/interface-contract.md
 * §1.3⑦⑧（points_home / points_ledger 载荷，只读）+ §四 kidpal_session_ctx
 * （points 两值 + selected_id 只读）/ kidpal_points_delete（单向幂等删除、权威
 * 重推隐式 ack）/ kidpal_points_page（keyset 分页 wire 规则）。
 *
 * Nature: 行为级（新 seam 纯逻辑真函数 + weak 哨兵：实现未编入前哨兵恒返回
 * K2P_UNIMPLEMENTED = 「冻结契约未实现」设计态红，不是编译错）+ 结构级（真实
 * 工作树源锚扫描，points_spike [G4] 先例形态；正向断言只认**注释剥离后可执行源**
 * 的字面形态）。
 *
 * 本文件**不 include** display/points_page.h / keybar.h（镜像公共形状，K0/SPV2
 * 先例；落地后 host runner 中 display/points_page.c 与 display/keybar.c 的强符号
 * 覆盖本文件 weak 哨兵）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）。
 * 冻结纪律：本文件全部 seam 为 261104-k2-points-device-ui 新增 additive 面，
 * K0 冻结面（points_spike [G1]..[G4]、points_page.h 既有形状）零触碰——
 * points_page.h 只增不改/移，keybar.c 只 additive 追加函数。
 *
 *   常量交叉对表（内部一致性锚，跨 case 引用同一镜像宏；与实现面同值约束）:
 *     K2P_DEL_IDLE_MS    = 10000UL 〔删除确认 10 秒静默取消，§2.3 矩阵注记；
 *                                    与 story_page.h KSPV2_DELETE_IDLE_MS 同值同族〕
 *     K2P_DISAMBIG_MS    = 300u   〔积分主页单击消歧窗，§2.3 消歧注记（~300ms）；
 *                                    与 story_page.h KSPV2_DISAMBIG_MS 同值同族〕
 *     K2P_DEL_RESEND_MS  = 5000UL / K2P_DEL_RESEND_MAX = 3 〔删除超时重发上限，
 *                                    契约 §四「超时未收到对账帧可重发」；与
 *                                    KSPV2_DELETE_RESEND_* 同族同值〕
 *     K2P_PAGE_REQ_LIMIT = 6     〔设备缺省分页请求 limit，契约 §四 缺省 6；
 *                                    与 K0 冻结 KPTS_LEDGER_ITEMS_MAX 同值——
 *                                    请求窗口与单帧容量一致，不继承 story 16 条〕
 *     K2P_RECENT_MAX     = 3     〔最近三条，契约 §1.3⑦；与 KPTS_RECENT_MAX 同值〕
 *     K2P_PAGE_POINTS_HOME = 2   〔KP_PAGE_POINTS_HOME K0a 冻结镜像
 *                                   （four_page_skeleton [1]；不新增
 *                                    KP_PAGE_POINTS_LEDGER——详细账本是 P3 子态〕
 *
 *   [P1] 积分主页四态判定（home = display/points_page.h + points_page.c;
 *        S-F6.1 AC1/AC4/AC5；「数据未达」与「确无分录」两态显式分叉）:
 *
 *          #define KPTS_HOME_STATE_NODATA   0  〔数据未达：不显示余额数字与
 *                                                进度线，纯等待口径〕
 *          #define KPTS_HOME_STATE_EMPTY    1  〔确认空账：显示 0 与目标、
 *                                                空账话术、进度线照常画〕
 *          #define KPTS_HOME_STATE_NORMAL   2  〔正常（含超目标，同页结构）〕
 *          #define KPTS_HOME_STATE_NEGATIVE 3  〔负余额：如实显示负值不藏数〕
 *
 *          int kidpal_points_home_state(const kidpal_points_home_t *frame);
 *            -- frame == NULL（设备从未收到权威帧 = RAM 初态）-> NODATA；
 *               frame->balance < 0 -> NEGATIVE；balance == 0 且 recent_count == 0
 *               -> EMPTY；其余（含 balance==0 但 recent 非空）-> NORMAL。
 *               不得用 balance:null / 伪造 0 / 断网标志混同两态（铁律；NODATA
 *               判据只落「有无权威帧」，不落「断网与否」）。
 *
 *   [P2] 进度线画法数据面（home = display/points_page.h + points_page.c;
 *        S-F6.1 AC2/AC3/AC5；V9 points-home*.png 冻结页级 ref 的画法判据半——
 *        视觉排版归 design review，本 seam 只钉数据判据）:
 *
 *          typedef struct {
 *              int valid;          〔1 = 本帧可画进度线；负余额也画（钳位）〕
 *              int pos;            〔小伴位置：0..target_points；超目标 > target〕
 *              int target_points;  〔场景标分值位（激活目标分值）〕
 *              int solid_len;      〔实线段长：已走过部分；负余额 = 0（全虚线）〕
 *              int overshoot;      〔1 = 余额超激活目标：实线越场景标延伸、
 *                                    同页结构（不新增「待兑现」页）〕
 *          } kidpal_points_progress_t;
 *
 *          int kidpal_points_progress(const kidpal_points_home_t *frame,
 *                                     kidpal_points_progress_t *out);
 *            -- frame NULL / out NULL -> -1（不画）；balance < 0 -> valid=1、
 *               pos=0、solid_len=0、overshoot=0（V9 points-home-negative.png：
 *               小伴钳位画在起点、全线虚线——负数不画负位，余额**数字**如实负值
 *               由 frame.balance 承载、本 seam 不得改写 frame）；0 <= balance
 *               <= target -> pos=balance、solid_len=balance、overshoot=0；
 *               balance > target -> pos=balance、solid_len=balance、overshoot=1
 *               （AC2「实线与小伴越过场景标继续延伸、页面结构不变」）。
 *
 *   [P3] 键位决策表（home = display/points_page.h + points_page.c；§2.3 权威
 *        矩阵 P3 积分主页行 / P3 详细账本行；数值本文件冻结）:
 *
 *          #define KPTS_GESTURE_KEY_SHORT   0
 *          #define KPTS_GESTURE_KEY_DOUBLE  1
 *          #define KPTS_GESTURE_KEY_LONG    2
 *          #define KPTS_GESTURE_BOOT_SHORT  3
 *          #define KPTS_GESTURE_BOOT_DOUBLE 4
 *          #define KPTS_GESTURE_BOOT_LONG   5
 *
 *          #define KPTS_ACT_VOICE_SESSION   0  〔开语音会话（积分语境）〕
 *          #define KPTS_ACT_OPEN_LEDGER     1  〔KEY 双击进详细账本〕
 *          #define KPTS_ACT_BACK            2  〔KEY 长按逐层返回（主页->回首页 /
 *                                              账本->回积分主页，§2.3）〕
 *          #define KPTS_ACT_CURSOR_DOWN     3  〔BOOT 短按光标下移（列表同型：
 *                                              屏底翻屏落首项、到底 wrap 永不离页）〕
 *          #define KPTS_ACT_NEXT_SCREEN     4  〔BOOT 双击翻下一屏〕
 *          #define KPTS_ACT_DELETE_ENTER    5  〔BOOT 长按进删除确认子态〕
 *          #define KPTS_ACT_NONE           (-1)
 *
 *          int kidpal_points_home_key_action(int gesture);
 *            -- KEY_SHORT -> VOICE_SESSION；KEY_DOUBLE -> OPEN_LEDGER（S-F6.2 AC1
 *               「积分主页 KEY 双击进详细账本」）；KEY_LONG -> BACK；BOOT_SHORT /
 *               BOOT_DOUBLE / BOOT_LONG -> NONE（积分主页 BOOT 页循环 = P1→P2→P3→
 *               P4 四页骨架，归 K0a 冻结域 four_page_skeleton，不落本表零偏移）；
 *               其余 -> NONE。
 *          int kidpal_points_ledger_key_action(int gesture);
 *            -- KEY_SHORT -> VOICE_SESSION（积分语境带账本上下文）；KEY_DOUBLE ->
 *               NONE（账本无双击语义，§2.3）；KEY_LONG -> BACK（回积分主页）；
 *               BOOT_SHORT -> CURSOR_DOWN；BOOT_DOUBLE -> NEXT_SCREEN；
 *               BOOT_LONG -> DELETE_ENTER；其余 -> NONE（BOOT 永不离页，§2.3）。
 *
 *          [P3b] 积分主页单击消歧门（home = display/points_page.h + points_page.c;
 *                §2.3 消歧注记「P4 故事列表 + P3 积分主页两处」第二处；到期/冲销
 *                判沿 story KSPV2 同族机制复用、同值 300ms——本缝只裁「哪页挂起」）:
 *
 *          #define KPTS_DISAMBIG_MS 300u
 *          int kidpal_points_debounce_gate(int page);
 *            -- page == KPTS_PAGE_POINTS_HOME（KP_PAGE 空间镜像 = 2）-> 1（挂起
 *               单击消歧窗）；其余页（含 P4 故事——故事域归 KSPV2 自己的门）-> 0。
 *
 *   [P4] 删除确认子态状态机 + 隐式 ack（home = display/points_page.h +
 *        points_page.c；S-F6.2 AC3/AC4/AC5 + 契约 §四 kidpal_points_delete；
 *        WF-F2-05 同族、与 story KSPV2 [K3]/[K4]/[K5] seam 同型）:
 *
 *          #define KPTS_DEL_GESTURE_KEY_SHORT   0  〔确认删除〕
 *          #define KPTS_DEL_GESTURE_BOOT_SHORT  1  〔取消〕
 *          #define KPTS_DEL_GESTURE_BOOT_DOUBLE 2  〔取消〕
 *          #define KPTS_DEL_GESTURE_BOOT_LONG   3  〔取消〕
 *          #define KPTS_DEL_GESTURE_OTHER       4  〔KEY 双击/长按等：不响应〕
 *
 *          #define KPTS_DEL_ACT_NONE     0
 *          #define KPTS_DEL_ACT_CONFIRM  1  〔发送 kidpal_points_delete——**绝不
 *                                          本地删缓存**，只信权威重推〕
 *          #define KPTS_DEL_ACT_CANCEL   2  〔取消回详细账本〕
 *
 *          int kidpal_points_delete_confirm_decide(int gesture);
 *            -- KEY_SHORT -> CONFIRM；BOOT_SHORT / BOOT_DOUBLE / BOOT_LONG ->
 *               CANCEL；OTHER -> NONE（其余手势不响应，§2.3 子态行）。
 *
 *          #define KPTS_DELETE_IDLE_MS 10000UL   〔10 秒静默取消，§2.3〕
 *          int kidpal_points_delete_idle_step(unsigned long *idle_ms,
 *                                             unsigned long dt_ms);
 *            -- 累计 idle 并判到点：*idle_ms += dt_ms 后 >= KPTS_DELETE_IDLE_MS
 *               -> 1（调用方**静默**取消回账本并复位——不显示倒计时）；未到点
 *               -> 0；NULL -> -1。任意手势重计 = 输入消费侧义务（KSPV2 同款）。
 *
 *          #define KPTS_DELETE_RESEND_MS  5000UL
 *          #define KPTS_DELETE_RESEND_MAX 3
 *          int kidpal_points_delete_resend_due(int tries, uint32_t age_ms);
 *            -- tries < KPTS_DELETE_RESEND_MAX 且 age_ms >= KPTS_DELETE_RESEND_MS
 *               -> 1（调用方重发同一 id 并 tries++——契约 §四「超时未收到不含该
 *               id 的对账帧可重发」）；否则 0（放弃后静默停发，不假删）。
 *
 *          int kidpal_points_delete_reconcile_hit(const char *pending_id,
 *                                                 const kidpal_points_ledger_t *frame);
 *            -- 隐式 ack 对账：pending_id 非空且命中 frame->items 窗口 -> 1
 *               （分录仍在，不冲销 pending）；不在窗口（含 items 空——权威删除
 *               重推首页必不含已删 id）或 pending 空串 -> 0（调用方冲销 pending；
 *               缓存更新**恒**经 kidpal_points_ledger_apply 权威帧纪律——确认
 *               动作本身绝不移除缓存分录，「未收到权威对账不假删」）；pending
 *               NULL / frame NULL -> -1。
 *
 *   [P5] keyset 全量分页客户端状态（home = display/points_page.h + points_page.c;
 *        S-F6.2 AC1 全量可及 + 契约 §四 wire 规则；不复用 story 16 条窗口）:
 *
 *          #define KPTS_PAGE_REQ_LIMIT 6   〔设备缺省请求 limit（契约 §四 缺省 6；
 *                                            = KPTS_LEDGER_ITEMS_MAX，请求窗口
 *                                            与单帧容量一致）〕
 *
 *          typedef struct {            〔K2 设备侧账本分页状态（RAM，掉电即失；
 *                                          不写 NVS/SD——D-06 铁律）〕
 *              int revision;           〔缓存所属账本版本〕
 *              int cached_count;       〔已缓存分录条数〕
 *              int has_more;
 *              int page_total;         〔最近权威帧 page.total = 全量分录数——
 *                                        **不用 items.length 顶替**〕
 *              char next_cursor[KPTS_CURSOR_CAP];  〔续页游标（原样回传）〕
 *          } kidpal_points_ledger_state_t;
 *
 *          int kidpal_points_ledger_state_apply(kidpal_points_ledger_state_t *st,
 *                                               const kidpal_points_ledger_t *f);
 *            -- NULL 入参 -> -1；f->reset != 0 -> **先清旧分页再装首页**（revision/
 *               cached_count/has_more/page_total/next_cursor 全字段随帧替换）-> 0；
 *               f->reset == 0 且 f->revision != st->revision -> -1（拒绝跨版本
 *               追加、st 零扰动，K0 [G3] 同纪律）；同版本 -> cached_count +=
 *               item_count、page_total/next_cursor/has_more 随帧 -> 0。
 *
 *          int kidpal_points_ledger_next_request(const kidpal_points_ledger_state_t *st,
 *                                                char *cursor_out, size_t cap,
 *                                                int *known_revision_out,
 *                                                int *limit_out);
 *            -- 任一输出指针 NULL -> -1；st->has_more 且 next_cursor 非空 ->
 *               拷贝游标（cap 不足 -> -1）、*known_revision_out = st->revision、
 *               *limit_out = KPTS_PAGE_REQ_LIMIT -> 1（有续页可取）；否则 ->
 *               cursor_out 置空串（首页请求语义）、*known_revision_out =
 *               (st->revision > 0 ? st->revision : -1)（无缓存 = 不带版本键，
 *               协议面 known_revision<0 = 缺省）、*limit_out = KPTS_PAGE_REQ_LIMIT
 *               -> 0。
 *
 *          int kidpal_points_ledger_screen_total(const kidpal_points_ledger_state_t *st,
 *                                                int rows_per_screen);
 *            -- 「第 x 屏 · 共 y 屏」的 y：以 st->page_total（全量）为准、
 *               **不以 cached_count 顶替**（全量可及——未取完的页也可见屏数）；
 *               ceil(page_total / rows_per_screen)；st NULL / rows <= 0 -> -1。
 *
 *   [P6] selected_id 只读注入决策（home = display/points_page.h + points_page.c;
 *        D-08 + 契约 §四「selected_id 只能来自最近收到的 points_ledger items、
 *        它不是删除或写入指令」）:
 *
 *          int kidpal_points_selected_id_injectable(const kidpal_points_ledger_t *frame,
 *                                                   const char *selected_id);
 *            -- frame NULL -> -1；selected_id NULL / 空串 -> 0（不附带）；
 *               命中 frame->items（最近收到的权威分页窗口）-> 1；未命中 -> 0
 *               （设备不造 id，非法值绝不上行）。
 *
 *   [P7] 积分子态键帽三槽（home = display/keybar.h + keybar.c additive 追加；
 *        子态常量住 points_page.h；键帽字面 = V9 冻结页级 ref 逐字）:
 *
 *          #define KPTS_SUBSTATE_NONE           0  〔非详细账本子态（基层 P3 主页，
 *                                                  键帽归页槽表——消费方回落）〕
 *          #define KPTS_SUBSTATE_LEDGER         1  〔详细账本列表态〕
 *          #define KPTS_SUBSTATE_DELETE_CONFIRM 2  〔删除确认整屏覆盖层子态
 *                                                  （覆盖层口径页点消失——
 *                                                  kidpal_keybar_dots
 *                                                  overlay_top != 0 既有冻结
 *                                                  语义承载，不另造机制）〕
 *
 *          int kidpal_keybar_points_slots(int substate, const char **out_left,
 *                                         const char **out_right);
 *            -- KPTS_SUBSTATE_LEDGER -> 左「问一问」右「下移」（V9
 *               points-ledger.png）；KPTS_SUBSTATE_DELETE_CONFIRM -> 左「删掉」
 *               右「算了」（V9 points-delete-confirm.png；WF-F2-05 同族、与
 *               story KSTORY_PAGE_DELETE 键帽同字）；NONE / 未知子态 -> -1
 *               （消费方回落基层页槽表，绝不画错键帽）；任一输出 NULL -> -1。
 *
 *   [P8] 消费闭环结构锚 + 登记守（结构级；home = mcp_server.cc +
 *        protocols/protocol.h/.cc + application.cc + host_app/main/CMakeLists.txt）:
 *            -- mcp_server.cc：K2 消费接线在世——kidpal_points_home_state 引用
 *               >= 1（四态判定接线 = 「未达不显示数字」承接面）+
 *               kidpal_points_ledger_state_apply 引用 >= 1（keyset 分页状态
 *               纪律接线）+（kidpal_keybar_points_slots 或
 *               KPTS_SUBSTATE_DELETE_CONFIRM）引用 >= 1（积分子态键帽/覆盖层
 *               接线）；
 *            -- 删除上行驱动方在世（契约 §四 kidpal_points_delete 设备半）：
 *               protocols/protocol.cc 或 protocol.h 引用 "kidpal_points_delete"
 *               >= 1（SendKidpalPointsPage K0 先例同型原语；无驱动方 = 确认
 *               后设备发不出删除，S-F6.2 AC4 全链死）；
 *            -- 积分语境上行构造在世："selected_id" 引用 >= 1（protocols/
 *               protocol.h 或 protocol.cc 或 application.cc——详细账本语境
 *               selected_id 上行）且 "points_ledger" 字面引用 >= 1（application.cc
 *               或 display/story_page.c 或 protocols/protocol.cc——source_page
 *               语境映射扩点，家不锁、实现自由；
 *               ⚠ 单点现居 story_page.c kidpal_story_ctx_source_page，不在
 *               changeManifest——builder 落点偏移须申报，见交付报告）；
 *            -- 登记守（anti-vacuity；非 acceptance 条目）：host_app/main/
 *               CMakeLists.txt 同含 "points_device_ui.acceptance.c"（本文件进
 *               host runner）与 "points_page.c"（行为齿强符号源）。
 *
 *   [P9] live-manual 真机轮观测项（**不落自动断言、不计入 runner 通过面**）:
 *   coverage: K2PTS-L1 —— 真实语音记账/兑换全链（S-F6.3 AC1..AC6 真机半）：完整句
 *             成功跳页、只提事件不记账、金额不匹配与余额不足拒答、短窗重复、断网
 *             可见失败不补记；不落自动断言。
 *   coverage: K2PTS-L2 —— 删除与恢复真机半（S-F6.2 AC3/AC4/AC5 + 冷启动恢复）：
 *             删除确认覆盖层目击（整屏覆盖、页点消失、10 秒静默取消）、删除后重算
 *             与负余额屏显、断网删除可见失败不假删、冷启动 restore 不抢前台/积分页
 *             原地刷新；**挂接 K1L-L1/L2**（human 2026-11-05 意向：QQ 落账不抢前台
 *             GAP-1 活性 + 真实消费端分页对账）一并入本轮 coverage 清单。
 *   coverage: K2PTS-L3 —— NFR-PERF-4 实测时延记录（软时延，不写死阈值）。
 *             三项载体 = device-round-evidence.md，真值判定 = 消费端看屏目击 +
 *             三源日志互证；离线断言不替代真机终验。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收
 * 断言」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | S-F6.1 AC1/AC4 两态分叉（未达 vs 空账） | TEST 1（home_state 决策表真跑：NULL/空账/零余额有分录/正常/负） | 行为级 |
 * | S-F6.1 AC2/AC3/AC5 进度线（钳位/超目标/负余额不藏数） | TEST 1（progress 决策表真跑；数字如实负值 = frame 只读断言） | 行为级（视觉排版归 design review） |
 * | S-F6.1 AC6 / §2.3 P3 积分主页键位行 | TEST 2（home_key 决策表 + BOOT 页循环归 K0a 不落本表负锚） | 行为级 |
 * | S-F6.2 AC1 / §2.3 P3 详细账本键位行 | TEST 2（ledger_key 决策表：光标/翻屏/删除入口/永不离页） | 行为级 |
 * | §2.3 消歧注记（积分主页 300ms 第二处） | TEST 2（debounce_gate 决策表）+ K2P_DISAMBIG_MS 同值锚 | 行为级 + 源锚代理（接线归 TEST 7） |
 * | S-F6.2 AC3 / §2.3 删除确认子态行 | TEST 3（decide 表 + idle 10s 静默取消真跑） | 行为级 |
 * | 契约 §四 kidpal_points_delete 隐式 ack / S-F6.2 AC4 不假删 | TEST 3（reconcile 决策表 + resend 上限）+ K0 [G3] apply 纪律（缓存唯一写通道） | 行为级（链路半；真机归 L2） |
 * | S-F6.2 AC1 全量可及 / 契约 §四 keyset wire | TEST 4（state_apply / next_request / screen_total 决策表 + 版本重置清缓存 + page.total 非 items.length） | 行为级 |
 * | D-08 / 契约 §四 selected_id 只读 | TEST 5（injectable 决策表）+ TEST 7（上行构造源锚） | 行为级 + 源锚代理 |
 * | S-F6.2 AC3 键帽 / V9 冻结页级 ref | TEST 6（points_slots 逐字键帽 + dots 覆盖层回归半） | 行为级（字面单源 = V9 页级 ref） |
 * | K2D-E1 消费闭环 | TEST 7（mcp/protocol/app 源锚 + 登记守） | 源锚代理（运行时归 L1/L2 真机轮） |
 * | S-F6.3 全链 / NFR-PERF-4 | coverage K2PTS-L1/L3（live-manual 单列） | live-manual |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1 四态的运行时渲染（未达态不画余额/进度线、空账话术排版、负余额数字字体
 *         形态）= mcp 接线层消费 + design review（V9 六帧冻结页级 ref）+ 真机轮；
 *         TEST 1 只钉判定与画法**数据判据**，不转脆性视觉断言。
 *   GAP-2 删除确认的 10 秒静默取消在真实输入流中的「任意手势重计」接线 = 输入消费
 *         侧义务（KSPV2 同款）；TEST 3 只钉累计判到点纯函数 + decide 表。
 *   GAP-3 「确认后只信权威重推」的全链活性（发删 -> front 消费 -> 权威重推 ->
 *         reconcile 冲销）离线不可闭环；设备半由 TEST 3/7 承载、front 半由 server
 *         件 K2PTS-A1 承载、端到端归 K2PTS-L2 真机轮。
 *   GAP-4 覆盖层页点消失的运行时效果 = kidpal_keybar_dots overlay_top 既有冻结
 *         语义（keybar_three_slot 冻结域）；K2 只钉子态->覆盖层接线源锚（TEST 7），
 *         不重开 keybar_three_slot 冻结域。
 *   GAP-5 restore 帧「P3 在屏原地刷新 / 他页不抢前台」的运行时效果 = 渲染面 +
 *         真机轮 L2（D-07；K0 takeover 分流 [G2] 已冻结、本包不重钉）。
 *   GAP-6 source_page 语境映射单点现居 display/story_page.c
 *         （kidpal_story_ctx_source_page，不在 changeManifest）——K2 扩
 *         points_ledger 语境的落点（原函数扩展 vs 映射点迁 application.cc）为
 *         builder 申报面；TEST 7 对语境字面按「三文件任一在世」钉，不锁家。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1 「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：standalone 红跑不链接
 *        points_page.c / keybar.c，哨兵恒红 = 设计态红；host runner 中
 *        points_page.c（K0 已在册）+ keybar.c（已在册）+ 本文件必须经登记守
 *        （TEST 7）进 HOST_ACCEPTANCE_SRCS，否则哨兵红掩盖「未接线」。
 *   EQ-2 「镜像 == 真头」：本文件 kidpal_points_entry_t / kidpal_points_home_t /
 *        kidpal_points_ledger_t 镜像 = K0 冻结形状（points_spike [G2]/[G3] 同源
 *        逐字复制）；新类型 kidpal_points_progress_t / kidpal_points_ledger_state_t
 *        的形状权威 = 本文件（builder 头文件须逐字同序同型，漂移由 TEST 1/4 锚先红
 *        兜底）。
 *   EQ-3 「源锚 = 运行时行为」：TEST 7 只证明源码具备消费/驱动/语境形状，不证明
 *        设备上真完成删除确认->上行->权威重推->原地刷新——K2PTS-L1/L2 真机轮承载。
 *   EQ-4 「V9 冻结页级 PNG 键帽字面 = 设备键帽字面单源」：TEST 6 键帽逐字断言以
 *        docs/ui/V9/points-ledger.png / points-delete-confirm.png 为准；文案单源
 *        漂移须先改设计 ref 再改本件（tighten-only 修订窗口不放宽字面）。
 *   EQ-5 「page.total = 全量分录数」由 producer（front/points.py）保证（K1L-A7
 *        冻结）；设备半 TEST 4 只钉「消费侧以 page_total 为准、不以 cached_count
 *        顶替」，两端以同一 wire 字段对账。
 *
 * Fixture 自含性声明：全部样例载荷/分录为文件内字面构造（契约 §1.3⑦⑧ 形状 +
 * V9 帧 facts），不依赖外部资产/时间真值/环境；结构锚依赖仓根（KIDPAL_REPO_ROOT
 * 显式注入或 walk-up 探测——acceptance-evidence「root 注入」纪律）。
 *
 * Run protocol（host runner, C4.4 惯例；**登记属 builder FIRST ACTION**——K0 先例）:
 *   0) builder 接线（重取基线后 FIRST ACTION）: 向 firmware/main/test/host_app/
 *      main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS 追加
 *      "../../points_device_ui.acceptance.c"（points_page.c / keybar.c 已在册，
 *      无需重复登记强符号源；TEST 7 登记守红 = 未接线）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，k0pts 通道同款）:
 *     cd firmware/main/test/acceptance/red-baseline && \
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash -lc \
 *       'gcc -std=c11 -Wall -Wextra -I. -o /tmp/k2p_red_runner ../../points_device_ui.acceptance.c runner.c && /tmp/k2p_red_runner'
 *     （exit code = 失败用例数；COMPILE_EXIT=0 = C 类冻结验收实编通过，
 *     DEV-GOVERNANCE §4；不链接任何实现源，weak 哨兵恒红 = 设计态红）
 *
 * RED baseline（冻结相位；host/device build 红基线按 K0S-E4 先例由 builder 接线后
 *   实跑，本记录 = 独立冒烟通道设计态红）: 7 个 TEST_CASE 全红，红因逐条对应当前
 *   story 缺失的真实行为（K2 正式化未落）：
 *     TEST 1 -> [P1]/[P2] 四态判定与进度线 seam 缺席（哨兵）
 *     TEST 2 -> [P3] 键位决策表/消歧门缺席（哨兵）
 *     TEST 3 -> [P4] 删除确认子态状态机/隐式 ack 缺席（哨兵）
 *     TEST 4 -> [P5] keyset 分页客户端状态缺席（哨兵）
 *     TEST 5 -> [P6] selected_id 注入决策缺席（哨兵）
 *     TEST 6 -> [P7] 积分子态键帽行缺席（哨兵）
 *     TEST 7 -> [P8] mcp 消费扩展 / 删除驱动方 / 语境上行 / 登记守缺席（builder
 *               接线后转绿）
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K2PTS-P1 -- 积分主页四态判定（NODATA/EMPTY/NORMAL/NEGATIVE）+
 *             进度线画法数据面（钳位/超目标/负余额）（TEST 1）
 *   coverage: K2PTS-P2 -- P3 积分主页 + P3 详细账本键位决策表 + 积分主页单击消歧
 *             门（TEST 2）
 *   coverage: K2PTS-P3 -- 删除确认子态手势决策 + 10 秒静默取消 + 超时重发上限 +
 *             权威重推隐式 ack 对账（TEST 3）
 *   coverage: K2PTS-P4 -- keyset 全量分页客户端状态（apply 重置/跨版本拒绝/
 *             next_request 全量遍历/page.total 权威/屏数换算）（TEST 4）
 *   coverage: K2PTS-P5 -- selected_id 只读注入决策（TEST 5）
 *   coverage: K2PTS-P6 -- 积分子态键帽三槽（LEDGER/DELETE_CONFIRM 逐字键帽）+
 *             覆盖层页点消失语义回归半（TEST 6）
 *   coverage: K2PTS-P7 -- 消费闭环结构锚（mcp 消费扩展/删除上行驱动方/积分语境
 *             上行构造）+ host runner 登记守（TEST 7；anti-vacuity 半为 builder
 *             FIRST ACTION 锚，非 acceptance 条目）
 *   coverage: K2PTS-L1 -- live-manual ①真实语音记账/兑换全链（S-F6.3 真机半，
 *             载体 device-round-evidence.md），不落自动断言；
 *   coverage: K2PTS-L2 -- live-manual ②删除/恢复真机半 + K1L-L1/L2 挂接
 *             （QQ 落账不抢前台 GAP-1 活性 + 真实消费端分页对账；载体
 *             device-round-evidence.md），不落自动断言；
 *   coverage: K2PTS-L3 -- live-manual ③NFR-PERF-4 实测时延记录（不写死阈值），
 *             不落自动断言。
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* -------------------------------------------------------------------------- *
 * 冻结数值镜像（K0 冻结形状 + 本包新 seam 数值；跨 case 同一宏引用，禁散字面）。
 * ------------------------------------------------------------------------ */

/* points_page.h 镜像容量（[G2]/[G3] K0 冻结形状，逐字同值）。 */
#define K2P_ENTRY_ID_CAP      64
#define K2P_ENTRY_REASON_CAP  96
#define K2P_ENTRY_AT_CAP      40
#define K2P_TARGET_NAME_CAP   64
#define K2P_TARGET_ASSET_CAP  16
#define K2P_RECENT_MAX        3   /* 契约 §1.3⑦：recent 最多三条（= KPTS_RECENT_MAX） */
#define K2P_LEDGER_ITEMS_MAX  6   /* 单帧 items 容量（= KPTS_LEDGER_ITEMS_MAX） */
#define K2P_CURSOR_CAP        128

/* KP_PAGE_* 镜像（K0a four_page_skeleton [1] 冻结四值；不新增 LEDGER 页枚举）。 */
#define K2P_PAGE_HOME        0
#define K2P_PAGE_SCHEDULE    1
#define K2P_PAGE_POINTS_HOME 2
#define K2P_PAGE_STORY       3
#define K2P_PAGE_COUNT       4

/* ---- 本包新 seam 常量（[P1]..[P7]；数值单源 = 本节，实现面同值约束） ------- */

#define K2P_HOME_STATE_NODATA   0 /* KPTS_HOME_STATE_NODATA */
#define K2P_HOME_STATE_EMPTY    1 /* KPTS_HOME_STATE_EMPTY */
#define K2P_HOME_STATE_NORMAL   2 /* KPTS_HOME_STATE_NORMAL */
#define K2P_HOME_STATE_NEGATIVE 3 /* KPTS_HOME_STATE_NEGATIVE */

#define K2P_GESTURE_KEY_SHORT   0 /* KPTS_GESTURE_KEY_SHORT */
#define K2P_GESTURE_KEY_DOUBLE  1
#define K2P_GESTURE_KEY_LONG    2
#define K2P_GESTURE_BOOT_SHORT  3
#define K2P_GESTURE_BOOT_DOUBLE 4
#define K2P_GESTURE_BOOT_LONG   5

#define K2P_ACT_VOICE_SESSION   0 /* KPTS_ACT_VOICE_SESSION */
#define K2P_ACT_OPEN_LEDGER     1
#define K2P_ACT_BACK            2
#define K2P_ACT_CURSOR_DOWN     3
#define K2P_ACT_NEXT_SCREEN     4
#define K2P_ACT_DELETE_ENTER    5
#define K2P_ACT_NONE            (-1)

#define K2P_DISAMBIG_MS 300u        /* KPTS_DISAMBIG_MS（= KSPV2_DISAMBIG_MS） */

#define K2P_DEL_GESTURE_KEY_SHORT   0 /* KPTS_DEL_GESTURE_KEY_SHORT */
#define K2P_DEL_GESTURE_BOOT_SHORT  1
#define K2P_DEL_GESTURE_BOOT_DOUBLE 2
#define K2P_DEL_GESTURE_BOOT_LONG   3
#define K2P_DEL_GESTURE_OTHER       4

#define K2P_DEL_ACT_NONE     0      /* KPTS_DEL_ACT_NONE */
#define K2P_DEL_ACT_CONFIRM  1
#define K2P_DEL_ACT_CANCEL   2

#define K2P_DELETE_IDLE_MS    10000UL /* KPTS_DELETE_IDLE_MS（= KSPV2_DELETE_IDLE_MS） */
#define K2P_DELETE_RESEND_MS  5000UL  /* KPTS_DELETE_RESEND_MS（= KSPV2 同族） */
#define K2P_DELETE_RESEND_MAX 3       /* KPTS_DELETE_RESEND_MAX */

#define K2P_PAGE_REQ_LIMIT 6          /* KPTS_PAGE_REQ_LIMIT（= PAGE_LIMIT_DEFAULT） */

#define K2P_SUBSTATE_NONE           0 /* KPTS_SUBSTATE_NONE */
#define K2P_SUBSTATE_LEDGER         1
#define K2P_SUBSTATE_DELETE_CONFIRM 2

/* 未实现哨兵：永不是任何整型 seam 的合法返回值。 */
#define K2P_UNIMPLEMENTED (-99)

/* kidpal_points_entry_t / kidpal_points_home_t / kidpal_points_ledger_t 镜像
 * （K0 冻结形状逐字复制，EQ-2）。 */
typedef struct {
    char id[K2P_ENTRY_ID_CAP];
    int delta;
    char reason[K2P_ENTRY_REASON_CAP];
    char occurred_at[K2P_ENTRY_AT_CAP];
    int balance_before;
    int balance_after;
} kidpal_points_entry_t;

typedef struct {
    int revision;
    int balance;
    char target_name[K2P_TARGET_NAME_CAP];
    int target_points;
    char target_asset[K2P_TARGET_ASSET_CAP];
    kidpal_points_entry_t recent[K2P_RECENT_MAX];
    int recent_count;
    int restore;
} kidpal_points_home_t;

typedef struct {
    int revision;
    int balance;
    kidpal_points_entry_t items[K2P_LEDGER_ITEMS_MAX];
    int item_count;
    int page_total;
    char next_cursor[K2P_CURSOR_CAP];
    int has_more;
    int reset;
    int restore;
} kidpal_points_ledger_t;

/* kidpal_keybar_dots_t 镜像（keybar.h 冻结形状）。 */
typedef struct {
    int visible;
    int total;
    int active;
} kidpal_keybar_dots_t;

/* ---- K2 新类型镜像（[P2]/[P5]；形状权威 = 本文件，EQ-2） ------------------ */

typedef struct {
    int valid;
    int pos;
    int target_points;
    int solid_len;
    int overshoot;
} kidpal_points_progress_t;

typedef struct {
    int revision;
    int cached_count;
    int has_more;
    int page_total;
    char next_cursor[K2P_CURSOR_CAP];
} kidpal_points_ledger_state_t;

/* -------------------------------------------------------------------------- *
 * 纯逻辑 seam 声明（与真头逐字同型；home = display/points_page.c +
 * display/keybar.c——host runner 强符号覆盖 weak 哨兵）。
 * ------------------------------------------------------------------------ */

int kidpal_points_home_state(const kidpal_points_home_t *frame);
int kidpal_points_progress(const kidpal_points_home_t *frame,
                           kidpal_points_progress_t *out);
int kidpal_points_home_key_action(int gesture);
int kidpal_points_ledger_key_action(int gesture);
int kidpal_points_debounce_gate(int page);
int kidpal_points_delete_confirm_decide(int gesture);
int kidpal_points_delete_idle_step(unsigned long *idle_ms, unsigned long dt_ms);
int kidpal_points_delete_resend_due(int tries, uint32_t age_ms);
int kidpal_points_delete_reconcile_hit(const char *pending_id,
                                       const kidpal_points_ledger_t *frame);
int kidpal_points_ledger_state_apply(kidpal_points_ledger_state_t *st,
                                     const kidpal_points_ledger_t *f);
int kidpal_points_ledger_next_request(const kidpal_points_ledger_state_t *st,
                                      char *cursor_out, size_t cap,
                                      int *known_revision_out, int *limit_out);
int kidpal_points_ledger_screen_total(const kidpal_points_ledger_state_t *st,
                                      int rows_per_screen);
int kidpal_points_selected_id_injectable(const kidpal_points_ledger_t *frame,
                                         const char *selected_id);
int kidpal_keybar_points_slots(int substate, const char **out_left,
                               const char **out_right);
int kidpal_keybar_dots(int page, int overlay_top, kidpal_keybar_dots_t *out);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（host runner 中 display/points_page.c / display/keybar.c 强定义
 * 覆盖；kidpal_keybar_dots 为 K0a 既有冻结 seam，本文件只作回归半载体）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) int kidpal_points_home_state(const kidpal_points_home_t *frame)
{ (void)frame; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_progress(const kidpal_points_home_t *frame,
                                                 kidpal_points_progress_t *out)
{ (void)frame; (void)out; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_home_key_action(int gesture)
{ (void)gesture; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_ledger_key_action(int gesture)
{ (void)gesture; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_debounce_gate(int page)
{ (void)page; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_delete_confirm_decide(int gesture)
{ (void)gesture; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_delete_idle_step(unsigned long *idle_ms,
                                                         unsigned long dt_ms)
{ (void)idle_ms; (void)dt_ms; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_delete_resend_due(int tries, uint32_t age_ms)
{ (void)tries; (void)age_ms; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_delete_reconcile_hit(const char *pending_id,
                                                             const kidpal_points_ledger_t *frame)
{ (void)pending_id; (void)frame; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_ledger_state_apply(kidpal_points_ledger_state_t *st,
                                                           const kidpal_points_ledger_t *f)
{ (void)st; (void)f; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_ledger_next_request(const kidpal_points_ledger_state_t *st,
                                                            char *cursor_out, size_t cap,
                                                            int *known_revision_out,
                                                            int *limit_out)
{ (void)st; (void)cursor_out; (void)cap; (void)known_revision_out; (void)limit_out;
  return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_ledger_screen_total(const kidpal_points_ledger_state_t *st,
                                                            int rows_per_screen)
{ (void)st; (void)rows_per_screen; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_selected_id_injectable(const kidpal_points_ledger_t *frame,
                                                               const char *selected_id)
{ (void)frame; (void)selected_id; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_keybar_points_slots(int substate, const char **out_left,
                                                     const char **out_right)
{ (void)substate; (void)out_left; (void)out_right; return K2P_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_keybar_dots(int page, int overlay_top,
                                             kidpal_keybar_dots_t *out)
{ (void)page; (void)overlay_top; (void)out; return K2P_UNIMPLEMENTED; }

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 子串与同行计数（自包含复制自
 * points_spike.acceptance.c 的 k0_* 助手族，命名前缀 k2p_；全 static）。
 * ------------------------------------------------------------------------ */

#define K2P_MAX_PATH 1024
#define K2P_MAX_READ (8u * 1024u * 1024u)

static char k2p_fm_buf[1024];
static const char *k2p_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *k2p_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(k2p_fm_buf, sizeof(k2p_fm_buf), fmt, ap);
    va_end(ap);
    return k2p_fm_buf;
}

static int k2p_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char k2p_root[K2P_MAX_PATH];
static int k2p_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int k2p_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[K2P_MAX_PATH];
    const char *env_root;
    size_t i;

    if (k2p_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (k2p_is_regular(probe)) {
            snprintf(k2p_root, sizeof(k2p_root), "%s", env_root);
            k2p_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (k2p_is_regular(probe)) {
            snprintf(k2p_root, sizeof(k2p_root), "%s", cands[i]);
            k2p_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *k2p_read(const char *rel, size_t *out_len)
{
    char p[K2P_MAX_PATH * 2 + 2]; /* root(<=1024) + '/' + rel — no truncation */
    FILE *f;
    long sz;
    char *buf;

    if (!k2p_locate_root()) {
        return NULL;
    }
    snprintf(p, sizeof(p), "%s/%s", k2p_root, rel);
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > K2P_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 注释剥离（K0/SPV2 同款）：正向断言只被**可执行源**满足——注释字节抹成空格；
 * 字符串 / 字符字面量体保留。保持缓冲长度，调用方 free。 */
static char *k2p_strip_c_comments(const char *src)
{
    size_t n, i = 0, o = 0;
    char *out;

    if (src == NULL) {
        return NULL;
    }
    n = strlen(src);
    out = (char *)malloc(n + 1);
    if (out == NULL) {
        return NULL;
    }
    while (i < n) {
        if (src[i] == '/' && i + 1 < n && src[i + 1] == '*') {
            i += 2;
            while (i + 1 < n && !(src[i] == '*' && src[i + 1] == '/')) {
                i++;
            }
            i = (i + 2 > n) ? n : i + 2;
            out[o++] = ' ';
            continue;
        }
        if (src[i] == '/' && i + 1 < n && src[i + 1] == '/') {
            while (i < n && src[i] != '\n') {
                i++;
            }
            out[o++] = ' ';
            continue;
        }
        if (src[i] == '"' || src[i] == '\'') {
            char q = src[i];
            out[o++] = src[i++];
            while (i < n) {
                out[o++] = src[i];
                if (src[i] == '\\' && i + 1 < n) {
                    out[o++] = src[i + 1];
                    i += 2;
                    continue;
                }
                if (src[i] == q) {
                    i++;
                    break;
                }
                i++;
            }
            continue;
        }
        out[o++] = src[i++];
    }
    out[o] = '\0';
    return out;
}

static char *k2p_strip_file(const char *rel)
{
    char *raw = k2p_read(rel, NULL);
    char *out;

    if (raw == NULL) {
        return NULL;
    }
    out = k2p_strip_c_comments(raw);
    free(raw);
    return out;
}

/* 子串出现次数（非重叠）。 */
static int k2p_count_substr(const char *hay, const char *needle)
{
    int count = 0;
    size_t nlen;

    if (hay == NULL || needle == NULL || needle[0] == '\0') {
        return 0;
    }
    nlen = strlen(needle);
    while ((hay = strstr(hay, needle)) != NULL) {
        count++;
        hay += nlen;
    }
    return count;
}

/* 同行双 needle 计数。 */
#define K2P_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(k2p_locate_root(), \
    "K2PTS RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

/* 哨兵守卫：-99 = seam 未实现（设计态红）；host runner 登记后转绿。 */
#define K2P_REQUIRE_IMPL_INT(v, what) \
    TEST_ASSERT_TRUE_MESSAGE((v) != K2P_UNIMPLEMENTED, \
        k2p_fm("%s RED: 冻结 seam 未实现（weak 哨兵返回 -99）——%s；builder 落地 " \
               "display/points_page.c（additive seam）并登记 " \
               "points_device_ui.acceptance.c 进 HOST_ACCEPTANCE_SRCS 后转绿", \
               "K2PTS", what))

/* -------------------------------------------------------------------------- *
 * 样例构造（fixture 自含：全部字面，契约 §1.3⑦⑧ 形状 + V9 帧 facts）。
 * ------------------------------------------------------------------------ */

/* 构造 points_home 帧（四态判定 / 进度线样例）。 */
static kidpal_points_home_t k2p_make_home(int balance, int target_points,
                                          int recent_count)
{
    kidpal_points_home_t f;

    memset(&f, 0, sizeof(f));
    f.revision = 12;
    f.balance = balance;
    snprintf(f.target_name, sizeof(f.target_name), "%s", "游乐场");
    f.target_points = target_points;
    snprintf(f.target_asset, sizeof(f.target_asset), "%s", "sc-wheel");
    f.recent_count = recent_count;
    if (recent_count > 0) {
        snprintf(f.recent[0].id, sizeof(f.recent[0].id), "%s", "pt_01J...");
        f.recent[0].delta = 5;
        snprintf(f.recent[0].reason, sizeof(f.recent[0].reason), "%s", "今天扫地");
        snprintf(f.recent[0].occurred_at, sizeof(f.recent[0].occurred_at), "%s",
                 "2026-11-04T09:00:00+08:00");
        f.recent[0].balance_before = balance - 5;
        f.recent[0].balance_after = balance;
    }
    return f;
}

/* 往 ledger 帧的 items 追加一条分录（测试 fixture；item_count 由调用方维护）。 */
static void k2p_ledger_add_item(kidpal_points_ledger_t *lg, const char *id,
                                int delta, const char *reason,
                                int before, int after)
{
    kidpal_points_entry_t *e;

    if (lg->item_count >= K2P_LEDGER_ITEMS_MAX) {
        return; /* fixture 容量守卫：测试样例不超过单帧容量 */
    }
    e = &lg->items[lg->item_count++];
    snprintf(e->id, sizeof(e->id), "%s", id);
    e->delta = delta;
    snprintf(e->reason, sizeof(e->reason), "%s", reason);
    snprintf(e->occurred_at, sizeof(e->occurred_at), "%s",
             "2026-11-03T20:00:00+08:00");
    e->balance_before = before;
    e->balance_after = after;
}

/* 构造 ledger 帧骨架。 */
static kidpal_points_ledger_t k2p_make_ledger(int revision, int balance,
                                              int page_total, const char *cursor,
                                              int has_more, int reset)
{
    kidpal_points_ledger_t lg;

    memset(&lg, 0, sizeof(lg));
    lg.revision = revision;
    lg.balance = balance;
    lg.page_total = page_total;
    if (cursor != NULL) {
        snprintf(lg.next_cursor, sizeof(lg.next_cursor), "%s", cursor);
    }
    lg.has_more = has_more;
    lg.reset = reset;
    return lg;
}

/* ========================================================================== *
 * TEST 1 — K2PTS-P1 积分主页四态判定 + 进度线画法数据面。
 * coverage: K2PTS-P1 -- kidpal_points_home_state 决策表（NULL=未达 / 空账 /
 *           零余额有分录=正常 / 正常 / 负余额）+ kidpal_points_progress 决策表
 *           （负余额钳位不画负位、超目标越标延伸、空账照常画、frame 只读不改写）
 * RED today（设计态）: points_page.c 无 [P1]/[P2] seam（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points device ui: home four-state decision and progress line clamp "
          "(K2PTS-P1)",
          "[points_device_ui][K2PTS-P1][linux][behavior]")
{
    kidpal_points_home_t f;
    kidpal_points_progress_t pg;

    /* ---- [P1] 四态判定决策表 ------------------------------------------------ */
    K2P_REQUIRE_IMPL_INT(kidpal_points_home_state(NULL), "kidpal_points_home_state");

    /* 数据未达：设备从未收到权威帧（frame NULL = RAM 初态）——纯等待口径，
     * 不显示余额数字与进度线（S-F6.1 AC4 未达半；判据落「有无权威帧」，
     * 不落「断网与否」）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_HOME_STATE_NODATA, kidpal_points_home_state(NULL),
        "K2PTS-P1 RED: 无权威帧（NULL）未判数据未达态 —— 「数据未达不显示数字」"
        "（S-F6.1 AC4）两态分叉的承载 seam 缺席或语义偏移。");

    /* 确认空账：balance==0 且 recent 空 -> 显示 0 与目标（AC4 空账半；与未达
     * 显式分叉——不得用伪造 0 混同未达）。 */
    f = k2p_make_home(0, 40, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_HOME_STATE_EMPTY, kidpal_points_home_state(&f),
        "K2PTS-P1 RED: balance=0 且 recent 空未判确认空账态 —— 空账须显示 0 与"
        "目标（S-F6.1 AC4），与「数据未达」是两态。");

    /* 零余额但有分录（加减相抵）-> 正常态（不是空账——「确无分录」判据落
     * recent，不只落 balance）。 */
    f = k2p_make_home(0, 40, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_HOME_STATE_NORMAL, kidpal_points_home_state(&f),
        "K2PTS-P1 RED: balance=0 但有分录被误判空账 —— 空账判据 = 确无分录"
        "（recent 空），有分录的零余额是正常态。");

    /* 正常（含超目标，同页结构——AC2 不设独立「已达成」页）。 */
    f = k2p_make_home(23, 40, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_HOME_STATE_NORMAL, kidpal_points_home_state(&f),
        "K2PTS-P1 RED: 正常余额未判 NORMAL。");
    f = k2p_make_home(55, 40, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_HOME_STATE_NORMAL, kidpal_points_home_state(&f),
        "K2PTS-P1 RED: 超激活目标被判成独立态 —— 单页进行中，超目标只是线上"
        "位置关系、同页结构不变（S-F6.1 AC1/AC2；不新增「待兑现」页）。");

    /* 负余额：如实显示负值、不藏数（AC5 / OQ-1；负余额只来自删除重算路径）。 */
    f = k2p_make_home(-40, 40, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_HOME_STATE_NEGATIVE, kidpal_points_home_state(&f),
        "K2PTS-P1 RED: 负余额未判 NEGATIVE 态 —— 负数不可藏成 0 或空态"
        "（S-F6.1 AC5 / D-03）。");

    /* ---- [P2] 进度线画法数据面 ---------------------------------------------- */
    K2P_REQUIRE_IMPL_INT(kidpal_points_progress(NULL, &pg), "kidpal_points_progress");
    f = k2p_make_home(23, 40, 1);
    K2P_REQUIRE_IMPL_INT(kidpal_points_progress(&f, NULL), "kidpal_points_progress(out NULL)");

    /* 正常：小伴在余额位、实线 = 已走过部分（AC2）。 */
    f = k2p_make_home(23, 40, 1);
    memset(&pg, 0xAA, sizeof(pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_progress(&f, &pg),
        "K2PTS-P2 RED: 正常帧进度线数据不可得 —— 进度线 seam 缺席（AC2 画法"
        "判据半；V9 points-home.png 起点刻度场景标布局归 design review）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pg.valid,
        "K2PTS-P2 RED: 正常帧 valid != 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, pg.pos,
        "K2PTS-P2 RED: 小伴位置非余额位（AC2「小伴标志放在线上代表当前余额"
        "位置」）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(40, pg.target_points,
        "K2PTS-P2 RED: 场景标分值位非激活目标分值。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, pg.solid_len,
        "K2PTS-P2 RED: 实线段非已走过部分（AC2「已走过的部分为实线」）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.overshoot,
        "K2PTS-P2 RED: 未超目标却报 overshoot。");
    /* frame 只读：进度线计算不得改写权威帧（负余额数字不藏数的载体）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, f.balance,
        "K2PTS-P2 RED: 进度线计算改写了 frame.balance —— 权威帧只读纪律被破"
        "（余额数字如实负值显示的载体被动过 = 藏数面）。");

    /* 超目标：小伴与实线越过场景标继续延伸、同页结构（AC2；overshoot=1 但
     * 不换页不新增态）。 */
    f = k2p_make_home(55, 40, 1);
    memset(&pg, 0, sizeof(pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_progress(&f, &pg),
        "K2PTS-P2 RED: 超目标帧进度线不可得。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(55, pg.pos,
        "K2PTS-P2 RED: 超目标时小伴位置被钳在场景标 —— AC2「实线与小伴越过"
        "场景标继续延伸、页面结构不变」。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(55, pg.solid_len,
        "K2PTS-P2 RED: 超目标时实线未越过场景标延伸。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pg.overshoot,
        "K2PTS-P2 RED: 超目标帧 overshoot != 1。");

    /* 负余额：小伴钳位画在起点、实线段为零（全线虚线）——V9
     * points-home-negative.png 冻结画法；余额数字的负值仍由 frame 如实承载。 */
    f = k2p_make_home(-40, 40, 1);
    memset(&pg, 0, sizeof(pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_progress(&f, &pg),
        "K2PTS-P2 RED: 负余额帧进度线不可得（V9 negative 帧进度线照画、钳位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pg.valid,
        "K2PTS-P2 RED: 负余额帧 valid != 1 —— 进度线应照画（钳位画法），不是"
        "消失（V9 points-home-negative.png：小伴在起点、全线虚线）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.pos,
        "K2PTS-P2 RED: 负余额未把小伴钳位到起点 —— 画法不得画出负位（V9 钳位"
        "画法；「如实显示负值」钉在余额数字，不等于画负位置）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.solid_len,
        "K2PTS-P2 RED: 负余额实线段非零 —— 负余额无「已走过」部分（V9：全线"
        "虚线）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-40, f.balance,
        "K2PTS-P2 RED: 进度线钳位把负余额改写成了非负值 —— 「不把负数藏成 0」"
        "（plan 铁律）：钳位只管线、数字必须仍如实负值。");

    /* 空账：进度线照常画（小伴在 0、实线零长）——AC4 空账「照常画目标」。 */
    f = k2p_make_home(0, 40, 0);
    memset(&pg, 0, sizeof(pg));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_progress(&f, &pg),
        "K2PTS-P2 RED: 空账帧进度线不可得 —— AC4 空账「照常画目标」。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pg.valid,
        "K2PTS-P2 RED: 空账帧 valid != 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.pos, "K2PTS-P2 RED: 空账小伴位置非 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.solid_len, "K2PTS-P2 RED: 空账实线非零。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pg.overshoot, "K2PTS-P2 RED: 空账 overshoot 非 0。");
}

/* ========================================================================== *
 * TEST 2 — K2PTS-P2 键位决策表（P3 积分主页行 + P3 详细账本行）+ 单击消歧门。
 * coverage: K2PTS-P2 -- home_key / ledger_key 决策表（§2.3 权威矩阵两行）+
 *           debounce_gate（§2.3 消歧注记第二处；KSPV2 同值 300ms 机制复用）
 * RED today（设计态）: points_page.c 无 [P3] seam（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points device ui: P3 home and ledger key action tables and debounce "
          "gate (K2PTS-P2)",
          "[points_device_ui][K2PTS-P2][linux][behavior]")
{
    static const struct {
        int gesture;
        int home_expect;
        int ledger_expect;
    } table[] = {
        /* gesture          home（§2.3 P3 积分主页行）   ledger（§2.3 P3 详细账本行） */
        { K2P_GESTURE_KEY_SHORT,   K2P_ACT_VOICE_SESSION, K2P_ACT_VOICE_SESSION },
        { K2P_GESTURE_KEY_DOUBLE,  K2P_ACT_OPEN_LEDGER,   K2P_ACT_NONE },
        { K2P_GESTURE_KEY_LONG,    K2P_ACT_BACK,          K2P_ACT_BACK },
        { K2P_GESTURE_BOOT_SHORT,  K2P_ACT_NONE,          K2P_ACT_CURSOR_DOWN },
        { K2P_GESTURE_BOOT_DOUBLE, K2P_ACT_NONE,          K2P_ACT_NEXT_SCREEN },
        { K2P_GESTURE_BOOT_LONG,   K2P_ACT_NONE,          K2P_ACT_DELETE_ENTER },
        { 6,                       K2P_ACT_NONE,          K2P_ACT_NONE }, /* 闭集外 */
        { -1,                      K2P_ACT_NONE,          K2P_ACT_NONE }, /* 闭集外 */
    };
    size_t i;

    K2P_REQUIRE_IMPL_INT(kidpal_points_home_key_action(K2P_GESTURE_KEY_SHORT),
                         "kidpal_points_home_key_action");
    K2P_REQUIRE_IMPL_INT(kidpal_points_ledger_key_action(K2P_GESTURE_KEY_SHORT),
                         "kidpal_points_ledger_key_action");

    for (i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            table[i].home_expect, kidpal_points_home_key_action(table[i].gesture),
            k2p_fm("K2PTS-P2 RED: 积分主页 gesture=%d 未返回 %d —— §2.3 P3 积分"
                   "主页行键位偏移（KEY 短=语音 / 双=详细账本 / 长=回首页；BOOT "
                   "翻页归 K0a 四页骨架、不落本表）",
                   table[i].gesture, table[i].home_expect));
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            table[i].ledger_expect, kidpal_points_ledger_key_action(table[i].gesture),
            k2p_fm("K2PTS-P2 RED: 详细账本 gesture=%d 未返回 %d —— §2.3 P3 详细"
                   "账本行键位偏移（KEY 短=语音 / 长=回积分主页 / BOOT 短=光标"
                   "下移 / 双=翻屏 / 长=删除确认；BOOT 永不离页）",
                   table[i].gesture, table[i].ledger_expect));
    }

    /* 消歧门：仅积分主页挂起 300ms 单击消歧窗（§2.3 消歧注记第二处；故事域归
     * KSPV2 自己的门——P4 返回 0 不抢故事冻结域）。 */
    K2P_REQUIRE_IMPL_INT(kidpal_points_debounce_gate(K2P_PAGE_POINTS_HOME),
                         "kidpal_points_debounce_gate");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_debounce_gate(K2P_PAGE_POINTS_HOME),
        "K2PTS-P2 RED: 积分主页未挂起单击消歧 —— §2.3 消歧注记「P4 故事列表 + "
        "P3 积分主页两处」第二处缺席（KEY 双击=进详细账本会误触成两次语音入口）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_debounce_gate(K2P_PAGE_STORY),
        "K2PTS-P2 RED: P4 故事页被积分消歧门接管 —— 故事列表消歧归 KSPV2 冻结域"
        "（story_page_v2 [K2]），积分门不得越域（两门并存、各管各页）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_debounce_gate(K2P_PAGE_HOME),
        "K2PTS-P2 RED: 非积分页误挂起消歧 —— 其余状态单击零延迟（§2.3）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_debounce_gate(-1),
        "K2PTS-P2 RED: 负值页未防御。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_debounce_gate(K2P_PAGE_COUNT),
        "K2PTS-P2 RED: KP_PAGE_COUNT 未防御。");
}

/* ========================================================================== *
 * TEST 3 — K2PTS-P3 删除确认子态：手势决策 + 10 秒静默取消 + 超时重发 +
 *          权威重推隐式 ack 对账。
 * coverage: K2PTS-P3 -- kidpal_points_delete_confirm_decide / delete_idle_step /
 *           delete_resend_due / delete_reconcile_hit 决策表
 * RED today（设计态）: points_page.c 无 [P4] seam（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points device ui: delete confirm substate decide, idle cancel, resend "
          "and authoritative reconcile (K2PTS-P3)",
          "[points_device_ui][K2PTS-P3][linux][behavior]")
{
    kidpal_points_ledger_t lg;
    unsigned long idle;

    /* ---- 手势决策表（§2.3 P3·账本删除确认子态行） --------------------------- */
    lg = k2p_make_ledger(12, 23, 3, "", 0, 0); /* 哨兵探测前置初始化（实现面读帧安全） */
    K2P_REQUIRE_IMPL_INT(kidpal_points_delete_confirm_decide(K2P_DEL_GESTURE_KEY_SHORT),
                         "kidpal_points_delete_confirm_decide");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_DEL_ACT_CONFIRM,
        kidpal_points_delete_confirm_decide(K2P_DEL_GESTURE_KEY_SHORT),
        "K2PTS-P3 RED: KEY 短按未确认删除 —— §2.3 子态行「KEY = 确认」。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_DEL_ACT_CANCEL,
        kidpal_points_delete_confirm_decide(K2P_DEL_GESTURE_BOOT_SHORT),
        "K2PTS-P3 RED: BOOT 短按未取消回账本。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_DEL_ACT_CANCEL,
        kidpal_points_delete_confirm_decide(K2P_DEL_GESTURE_BOOT_DOUBLE),
        "K2PTS-P3 RED: BOOT 双击未取消（「BOOT 任一手势 = 取消」）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_DEL_ACT_CANCEL,
        kidpal_points_delete_confirm_decide(K2P_DEL_GESTURE_BOOT_LONG),
        "K2PTS-P3 RED: BOOT 长按未取消。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_DEL_ACT_NONE,
        kidpal_points_delete_confirm_decide(K2P_DEL_GESTURE_OTHER),
        "K2PTS-P3 RED: 其余手势未保持不响应（§2.3「其余手势不响应」）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_DEL_ACT_NONE,
        kidpal_points_delete_confirm_decide(-1),
        "K2PTS-P3 RED: 闭集外手势未防御。");

    /* ---- 10 秒静默取消（累计判到点；不显示倒计时 = 显示面义务） -------------- */
    K2P_REQUIRE_IMPL_INT(kidpal_points_delete_idle_step(&idle, 1000),
                         "kidpal_points_delete_idle_step");
    idle = 0;
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_delete_idle_step(NULL, 1000),
        "K2PTS-P3 RED: idle_step(NULL) 未防御。");
    idle = 0;
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_delete_idle_step(&idle, 9999),
        "K2PTS-P3 RED: 9999ms 未到点即报取消 —— 提前取消会掐掉慢操作孩子的"
        "确认窗（10 秒口径被缩短）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(9999UL, idle,
        "K2PTS-P3 RED: idle 累计值偏移（应累计到 9999）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_delete_idle_step(&idle, 1),
        "K2PTS-P3 RED: 累计到 10000ms 未报静默取消 —— §2.3「10 秒无操作静默"
        "取消」未兑现。");
    idle = 9999UL;
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_delete_idle_step(&idle, 1),
        "K2PTS-P3 RED: 恰 10000ms 边界未判到点（>= 语义）。");

    /* ---- 超时重发上限（契约 §四「可重发」；上限后静默停发、不假删） ---------- */
    K2P_REQUIRE_IMPL_INT(kidpal_points_delete_resend_due(0, 0),
                         "kidpal_points_delete_resend_due");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_delete_resend_due(0, 4999),
        "K2PTS-P3 RED: 4999ms 即报重发 —— 重发间隔偏短会打爆 front。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_delete_resend_due(0, 5000),
        "K2PTS-P3 RED: 5000ms 到点未报重发 —— 未收到权威对账帧时的重发通道"
        "缺席（契约 §四）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_delete_resend_due(2, 9000),
        "K2PTS-P3 RED: tries=2 未达上限却拒绝重发。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_delete_resend_due(3, 9000),
        "K2PTS-P3 RED: 超过重发上限仍报重发 —— 无限重发面。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_delete_resend_due(4, 0),
        "K2PTS-P3 RED: tries 超上限未防御。");

    /* ---- 隐式 ack 对账（确认后 pending 只信权威重推） -----------------------
     * lg 已在上方哨兵探测前初始化（实现面读帧安全）。 */
    K2P_REQUIRE_IMPL_INT(kidpal_points_delete_reconcile_hit("pt_a", &lg),
                         "kidpal_points_delete_reconcile_hit");

    /* 分录仍在权威窗口 -> 不冲销（对账帧未证明删除生效）。 */
    lg = k2p_make_ledger(13, 10, 3, "", 0, 1);
    k2p_ledger_add_item(&lg, "pt_a", 50, "误记", 0, 50);
    k2p_ledger_add_item(&lg, "pt_b", 5, "今天扫地", 50, 55);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_points_delete_reconcile_hit("pt_a", &lg),
        "K2PTS-P3 RED: pending id 仍在权威 items 却被冲销 —— 权威重推仍含该"
        "分录 = 删除未生效，冲销会假删（S-F6.2 AC4「删除生效才刷新」）。");

    /* 权威删除重推首页不含已删 id -> 冲销（删除生效的隐式 ack）。 */
    lg = k2p_make_ledger(14, -40, 2, "", 0, 1);
    k2p_ledger_add_item(&lg, "pt_b", 5, "今天扫地", -45, -40);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_points_delete_reconcile_hit("pt_a", &lg),
        "K2PTS-P3 RED: 权威重推已不含 pending id 却不冲销 —— 删除生效后确认"
        "子态 pending 永挂，重发通道会误重发。");

    /* pending 空串 -> 0（无 pending，无需冲销）；NULL -> -1。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_delete_reconcile_hit("", &lg),
        "K2PTS-P3 RED: 空 pending 未归 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_delete_reconcile_hit(NULL, &lg),
        "K2PTS-P3 RED: pending NULL 未防御。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_delete_reconcile_hit("pt_a", NULL),
        "K2PTS-P3 RED: frame NULL 未防御。");

    /* items 空（权威空账重推）-> 冲销（0）。 */
    lg = k2p_make_ledger(15, 0, 0, "", 0, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_delete_reconcile_hit("pt_a", &lg),
        "K2PTS-P3 RED: 权威空账重推未冲销 pending。");
}

/* ========================================================================== *
 * TEST 4 — K2PTS-P4 keyset 全量分页客户端状态：apply 重置/跨版本拒绝 +
 *          next_request 全量遍历 + page.total 权威 + 屏数换算。
 * coverage: K2PTS-P4 -- kidpal_points_ledger_state_apply / next_request /
 *           screen_total 决策表（S-F6.2 AC1 全量可及 + 契约 §四 wire 规则）
 * RED today（设计态）: points_page.c 无 [P5] seam（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points device ui: keyset pagination client state, full traversal and "
          "version reset (K2PTS-P4)",
          "[points_device_ui][K2PTS-P4][linux][behavior]")
{
    kidpal_points_ledger_state_t st;
    kidpal_points_ledger_t f;
    char cursor[K2P_CURSOR_CAP];
    int known_rev, limit;

    /* ---- apply：reset = 先清旧分页再装首页 ---------------------------------- */
    memset(&st, 0, sizeof(st));
    f = k2p_make_ledger(0, 0, 0, "", 0, 0); /* 哨兵探测前置初始化（实现面读帧安全） */
    K2P_REQUIRE_IMPL_INT(kidpal_points_ledger_state_apply(&st, &f),
                         "kidpal_points_ledger_state_apply");
    memset(&st, 0, sizeof(st));
    st.revision = 3;
    st.cached_count = 9;   /* 旧分页残留 */
    st.page_total = 9;
    snprintf(st.next_cursor, sizeof(st.next_cursor), "%s", "stale-cursor");
    f = k2p_make_ledger(7, 10, 5, "c1", 1, 1); /* reset=true 首页 */
    k2p_ledger_add_item(&f, "pt_1", 5, "今天扫地", 5, 10);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_state_apply(&st, &f),
        "K2PTS-P4 RED: 首页 reset 帧被拒 —— 版本变化或显式打开后必须先清旧分页"
        "再装首页（契约 §四 reset=true 语义）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, st.revision,
        "K2PTS-P4 RED: reset 后 revision 未随帧。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, st.cached_count,
        "K2PTS-P4 RED: reset 后旧分页未清（cached_count 须替换为本帧条数，不"
        "叠加）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, st.page_total,
        "K2PTS-P4 RED: page_total 未取权威 page.total —— 全量分录数不得用"
        "items.length 顶替（契约 §1.3⑧；本帧 items 1 条、total 5）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("c1", st.next_cursor,
        "K2PTS-P4 RED: 续页游标未随帧。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, st.has_more, "K2PTS-P4 RED: has_more 未随帧。");

    /* ---- apply：同版本续页累加 ---------------------------------------------- */
    f = k2p_make_ledger(7, 10, 5, "c2", 1, 0); /* reset=false 正常续页 */
    k2p_ledger_add_item(&f, "pt_2", 2, "老师表扬", 8, 10);
    k2p_ledger_add_item(&f, "pt_3", 3, "整理房间", 7, 10);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_state_apply(&st, &f),
        "K2PTS-P4 RED: 同版本续页被拒。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, st.cached_count,
        "K2PTS-P4 RED: 续页未累加（1 + 2 = 3）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("c2", st.next_cursor,
        "K2PTS-P4 RED: 续页后游标未更新。");

    /* ---- apply：跨版本追加拒绝（缓存零扰动） -------------------------------- */
    f = k2p_make_ledger(8, 10, 5, "cx", 1, 0); /* reset=false 但版本已变 */
    k2p_ledger_add_item(&f, "pt_x", 1, "跨版本", 9, 10);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_state_apply(&st, &f),
        "K2PTS-P4 RED: 跨版本续页未被拒 —— 契约 §四「不得跨版本追加」被破"
        "（K0 [G3] 同纪律在本状态面上的延续）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, st.cached_count,
        "K2PTS-P4 RED: 跨版本拒绝后缓存被扰动 —— 拒绝必须零扰动。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("c2", st.next_cursor,
        "K2PTS-P4 RED: 跨版本拒绝后游标被扰动。");

    /* ---- next_request：全量遍历驱动（全量可及，不复用 story 16 条窗口） ------ */
    K2P_REQUIRE_IMPL_INT(kidpal_points_ledger_next_request(&st, cursor,
                                                           sizeof(cursor),
                                                           &known_rev, &limit),
                         "kidpal_points_ledger_next_request");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_ledger_next_request(
        &st, cursor, sizeof(cursor), &known_rev, &limit),
        "K2PTS-P4 RED: 有续页（has_more=1、cursor 非空）却报无续页 —— 全量"
        "可及中断（S-F6.2 AC1「任意历史分录都能翻到」）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("c2", cursor,
        "K2PTS-P4 RED: 续页游标未原样回传（keyset 游标设备不解释内容）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, known_rev,
        "K2PTS-P4 RED: known_revision 未带缓存版本 —— front 版本重置判定失据。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_PAGE_REQ_LIMIT, limit,
        "K2PTS-P4 RED: 请求 limit != 6 —— 契约 §四缺省 6；请求窗口与单帧容量"
        "一致（KPTS_LEDGER_ITEMS_MAX=6），不继承 story 16 条窗口。");

    /* 取完（has_more=0）-> 首页请求语义 + 版本键照带。 */
    f = k2p_make_ledger(7, 10, 5, "", 0, 0);
    k2p_ledger_add_item(&f, "pt_4", 1, "帮妈妈拿快递", 9, 10);
    k2p_ledger_add_item(&f, "pt_5", 1, "读英语", 9, 10);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_state_apply(&st, &f),
        "K2PTS-P4 RED: 末页续页被拒。");
    memset(cursor, 'X', sizeof(cursor));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_next_request(
        &st, cursor, sizeof(cursor), &known_rev, &limit),
        "K2PTS-P4 RED: has_more=0 仍报有续页 —— 已到最早一页（契约 §四 "
        "next_cursor 空且 has_more=false）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cursor[0],
        "K2PTS-P4 RED: 无续页时游标未置空串（首页请求语义）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, known_rev,
        "K2PTS-P4 RED: 首页请求未带当前版本（版本变化后 front 才能判重置）。");

    /* 无缓存（revision 0）-> 不带版本键（known_revision_out = -1，协议面缺省）。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_next_request(
        &st, cursor, sizeof(cursor), &known_rev, &limit),
        "K2PTS-P4 RED: 无缓存首页请求被拒。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, known_rev,
        "K2PTS-P4 RED: 无缓存仍带版本键 —— 冷启动首取无版本可对（协议面 "
        "known_revision<0 = 缺省不带）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_next_request(
        NULL, cursor, sizeof(cursor), &known_rev, &limit),
        "K2PTS-P4 RED: next_request(NULL) 未防御。");

    /* ---- screen_total：以 page_total 为准（全量屏数，不以 cached_count 顶替） */
    memset(&st, 0, sizeof(st));
    st.cached_count = 4;
    st.page_total = 5;
    K2P_REQUIRE_IMPL_INT(kidpal_points_ledger_screen_total(&st, 2),
                         "kidpal_points_ledger_screen_total");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, kidpal_points_ledger_screen_total(&st, 2),
        "K2PTS-P4 RED: 屏数以 cached_count 计（ceil(4/2)=2）而非 page_total"
        "（ceil(5/2)=3）—— 全量可及的「共 y 屏」必须按全量分录数，未取完的页"
        "也要可见。");
    memset(&st, 0, sizeof(st));
    st.page_total = 6;
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_ledger_screen_total(&st, 6),
        "K2PTS-P4 RED: 整除边界屏数错误（6/6 = 1 屏）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_screen_total(&st, 0),
        "K2PTS-P4 RED: rows<=0 未防御（分除零面）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_screen_total(NULL, 6),
        "K2PTS-P4 RED: screen_total(NULL) 未防御。");
}

/* ========================================================================== *
 * TEST 5 — K2PTS-P5 selected_id 只读注入决策。
 * coverage: K2PTS-P5 -- kidpal_points_selected_id_injectable 决策表（D-08 +
 *           契约 §四「只能来自最近收到的 items、不是删除或写入指令」）
 * RED today（设计态）: points_page.c 无 [P6] seam（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points device ui: selected_id read-only injection decision (K2PTS-P5)",
          "[points_device_ui][K2PTS-P5][linux][behavior]")
{
    kidpal_points_ledger_t lg;

    lg = k2p_make_ledger(12, 23, 25, "c1", 1, 0);
    k2p_ledger_add_item(&lg, "pt_a", -40, "兑换游乐场", 63, 23);
    k2p_ledger_add_item(&lg, "pt_b", 3, "主动洗碗", 18, 21);

    K2P_REQUIRE_IMPL_INT(kidpal_points_selected_id_injectable(&lg, "pt_a"),
                         "kidpal_points_selected_id_injectable");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_selected_id_injectable(&lg, "pt_a"),
        "K2PTS-P5 RED: 权威 items 内的 id 未判定可注入 —— 「我为什么得了3分」"
        "类分录摘要语境不可达（S-F6.3 积分语境带账本上下文）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_selected_id_injectable(&lg, "pt_b"),
        "K2PTS-P5 RED: 第二条分录 id 未判定可注入。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_selected_id_injectable(&lg, "pt_zz"),
        "K2PTS-P5 RED: 非权威 id 未拒绝 —— 设备不造 id（契约 §四「其值只能"
        "来自最近收到的 points_ledger items」；非法值上行会被 front 拒绝并"
        "浪费一轮语境）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_selected_id_injectable(&lg, ""),
        "K2PTS-P5 RED: 空 id 未归 0（不附带）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_selected_id_injectable(&lg, NULL),
        "K2PTS-P5 RED: NULL id 未归 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_selected_id_injectable(NULL, "pt_a"),
        "K2PTS-P5 RED: frame NULL 未防御。");
}

/* ========================================================================== *
 * TEST 6 — K2PTS-P6 积分子态键帽三槽 + 覆盖层页点消失语义回归半。
 * coverage: K2PTS-P6 -- kidpal_keybar_points_slots（V9 冻结页级 ref 逐字键帽）+
 *           kidpal_keybar_dots overlay_top 语义回归（keybar_three_slot 冻结域
 *           must-stay-green；删除确认覆盖层页点消失的机制载体）
 * RED today（设计态）: keybar.c 无 [P7] seam（哨兵 -99；dots 为既有冻结 seam、
 *           host runner 中转绿，standalone 通道随哨兵红）。
 * ========================================================================== */
TEST_CASE("points device ui: points substate keybar slots and overlay dots "
          "regression (K2PTS-P6)",
          "[points_device_ui][K2PTS-P6][linux][behavior]")
{
    const char *left = NULL;
    const char *right = NULL;
    kidpal_keybar_dots_t dots;

    K2P_REQUIRE_IMPL_INT(kidpal_keybar_points_slots(K2P_SUBSTATE_LEDGER,
                                                    &left, &right),
                         "kidpal_keybar_points_slots");

    /* 详细账本列表态键帽（V9 points-ledger.png 冻结页级 ref 逐字）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_keybar_points_slots(K2P_SUBSTATE_LEDGER, &left, &right),
        "K2PTS-P6 RED: 详细账本子态键帽不可得。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("问一问", left,
        "K2PTS-P6 RED: 账本左槽键帽偏移 —— V9 points-ledger.png 左槽「问一问」"
        "（KEY 短按 = 开语音会话，积分语境）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("下移", right,
        "K2PTS-P6 RED: 账本右槽键帽偏移 —— V9 points-ledger.png 右槽「下移」"
        "（BOOT 短按 = 光标下移，与故事列表同型）。");

    /* 删除确认覆盖层键帽（V9 points-delete-confirm.png；WF-F2-05 同族、与 story
     * KSTORY_PAGE_DELETE 键帽同字「删掉/算了」）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_keybar_points_slots(K2P_SUBSTATE_DELETE_CONFIRM, &left, &right),
        "K2PTS-P6 RED: 删除确认子态键帽不可得。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("删掉", left,
        "K2PTS-P6 RED: 删除确认左槽键帽偏移 —— V9 points-delete-confirm.png "
        "「删掉」（KEY 短按 = 确认删除）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("算了", right,
        "K2PTS-P6 RED: 删除确认右槽键帽偏移 —— V9 points-delete-confirm.png "
        "「算了」（BOOT = 取消）。");

    /* 非积分子态 -> -1（消费方回落基层页槽表，绝不画错键帽）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_keybar_points_slots(K2P_SUBSTATE_NONE, &left, &right),
        "K2PTS-P6 RED: NONE 子态未回落 —— 基层 P3 主页键帽归页槽表（K0a 已冻结"
        "「问一问/下一页」），本缝接管会画出错键帽。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_keybar_points_slots(99, &left, &right),
        "K2PTS-P6 RED: 未知子态未防御。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_keybar_points_slots(K2P_SUBSTATE_LEDGER, NULL, &right),
        "K2PTS-P6 RED: out_left NULL 未防御。");

    /* 覆盖层页点消失（回归半；keybar_three_slot 冻结域语义——删除确认整屏覆盖
     * 层以 overlay_top != 0 驱动页点隐藏，机制零新增）。 */
    K2P_REQUIRE_IMPL_INT(kidpal_keybar_dots(K2P_PAGE_POINTS_HOME, 1, &dots),
                         "kidpal_keybar_dots");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_keybar_dots(K2P_PAGE_POINTS_HOME, 1, &dots),
        "K2PTS-P6 RED: 覆盖层 dots 调用失败（既有冻结 seam 偏移）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, dots.visible,
        "K2PTS-P6 RED: 覆盖层顶层页点未隐藏 —— 删除确认整屏覆盖弹窗页点消失"
        "（§2.3 覆盖层口径）的机制载体偏移。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K2P_PAGE_COUNT, dots.total,
        "K2PTS-P6 RED: 覆盖层下 total 被抹 —— 隐藏不抹机制状态（keybar 冻结"
        "语义）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_keybar_dots(K2P_PAGE_POINTS_HOME, 0, &dots),
        "K2PTS-P6 RED: 基层 dots 调用失败。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, dots.visible,
        "K2PTS-P6 RED: 基层页点不可见 —— 覆盖层关闭后页点恢复偏移。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, dots.active,
        "K2PTS-P6 RED: P3 页点 active 偏移（第 3 实心 = active 2）。");
}

/* ========================================================================== *
 * TEST 7 — K2PTS-P7 消费闭环结构锚 + 登记守（源锚代理；运行时归 K2PTS-L1/L2）。
 * coverage: K2PTS-P7 -- mcp_server.cc K2 消费接线 + protocols 删除上行驱动方 +
 *           积分语境（points_ledger / selected_id）上行构造 + host_app CMake
 *           登记守（anti-vacuity，非 acceptance 条目）
 * RED today（设计态）: 实现面接线缺席（builder 落地后转绿）。
 * ========================================================================== */
TEST_CASE("points device ui: consumption closure structural anchors and host "
          "registration guard (K2PTS-P7)",
          "[points_device_ui][K2PTS-P7][linux][structural]")
{
    char *mcp;
    char *proto_cc;
    char *proto_h;
    char *app;
    char *story_page;
    char *cmake;
    int ledger_literal;

    K2P_REQUIRE_ROOT();

    /* ---- mcp_server.cc：K2 消费接线在世 -------------------------------------- */
    mcp = k2p_strip_file("firmware/main/mcp_server.cc");
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "K2PTS-P7 RED(infra): mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        k2p_count_substr(mcp, "kidpal_points_home_state") >= 1,
        "K2PTS-P7 RED: mcp_server.cc 无 kidpal_points_home_state 接线 —— 四态判定"
        "未接入 kidpal/display 消费链，「数据未达不显示数字」（S-F6.1 AC4）无承接"
        "面，未达态会照画余额数字误导孩子。");
    TEST_ASSERT_TRUE_MESSAGE(
        k2p_count_substr(mcp, "kidpal_points_ledger_state_apply") >= 1,
        "K2PTS-P7 RED: mcp_server.cc 无 kidpal_points_ledger_state_apply 接线 —— "
        "keyset 分页客户端状态纪律未接（K0 时代 g_points_ledger_view 最小缓存之外"
        "的全量可及状态缺席）。");
    TEST_ASSERT_TRUE_MESSAGE(
        k2p_count_substr(mcp, "kidpal_keybar_points_slots") >= 1 ||
        k2p_count_substr(mcp, "KPTS_SUBSTATE_DELETE_CONFIRM") >= 1,
        "K2PTS-P7 RED: mcp_server.cc 无积分子态键帽/覆盖层接线 —— 详细账本与删除"
        "确认子态键帽（kidpal_keybar_points_slots）或覆盖层子态（"
        "KPTS_SUBSTATE_DELETE_CONFIRM）至少一处须在消费链在世。");
    free(mcp);

    /* ---- 删除上行驱动方在世（契约 §四 kidpal_points_delete 设备半） ---------- */
    proto_cc = k2p_strip_file("firmware/main/protocols/protocol.cc");
    TEST_ASSERT_NOT_NULL_MESSAGE(proto_cc,
                                 "K2PTS-P7 RED(infra): protocol.cc unreadable");
    proto_h = k2p_strip_file("firmware/main/protocols/protocol.h");
    TEST_ASSERT_NOT_NULL_MESSAGE(proto_h,
                                 "K2PTS-P7 RED(infra): protocol.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        k2p_count_substr(proto_cc, "kidpal_points_delete") >= 1 ||
        k2p_count_substr(proto_h, "kidpal_points_delete") >= 1,
        "K2PTS-P7 RED: protocols 无 kidpal_points_delete 上行驱动方 —— 删除确认"
        "后设备发不出删除帧（SendKidpalPointsPage K0 先例同型原语缺席），S-F6.2 "
        "AC4 删除重算全链死（无驱动方 = 确认按钮空转）。");

    /* ---- 积分语境上行构造在世（D-08；契约 §四 selected_id + points_ledger）--- */
    app = k2p_strip_file("firmware/main/application.cc");
    TEST_ASSERT_NOT_NULL_MESSAGE(app, "K2PTS-P7 RED(infra): application.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        k2p_count_substr(proto_h, "selected_id") >= 1 ||
        k2p_count_substr(proto_cc, "selected_id") >= 1 ||
        k2p_count_substr(app, "selected_id") >= 1,
        "K2PTS-P7 RED: 上行构造面无 selected_id —— 详细账本语境 selected_id 只读"
        "注入上行缺席（契约 §四；「我为什么得了3分」的账本上下文载体）。");
    /* source_page 语境字面：家不锁（映射单点现居 story_page.c、不在 changeManifest
     * ——见文件头 GAP-6），三处任一在世即锚定。 */
    story_page = k2p_strip_file("firmware/main/display/story_page.c");
    ledger_literal = (k2p_count_substr(app, "\"points_ledger\"") >= 1 ||
                      k2p_count_substr(proto_cc, "\"points_ledger\"") >= 1 ||
                      (story_page != NULL &&
                       k2p_count_substr(story_page, "\"points_ledger\"") >= 1));
    TEST_ASSERT_TRUE_MESSAGE(
        ledger_literal,
        "K2PTS-P7 RED: 语境映射面无 \"points_ledger\" 字面 —— 详细账本语境 "
        "source_page 取值缺席（契约 §四闭集；kidpal_session_ctx 只报 points_home "
        "会让账本内提问丢失分录上下文）。");
    free(proto_cc);
    free(proto_h);
    free(app);
    if (story_page != NULL) {
        free(story_page);
    }

    /* ---- 登记守（anti-vacuity；builder FIRST ACTION，非 acceptance 条目）----- */
    cmake = k2p_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake,
                                 "K2PTS-P7 RED(infra): host CMakeLists unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(cmake, "points_device_ui.acceptance.c") != NULL,
        "K2PTS-P7 RED: 本文件未登记进 HOST_ACCEPTANCE_SRCS —— host runner 不编"
        "本件则全部行为齿空转（builder FIRST ACTION；K0 登记守先例）。");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(cmake, "points_page.c") != NULL,
        "K2PTS-P7 RED: points_page.c 未登记为强符号源 —— weak 哨兵不被覆盖，"
        "行为红会掩盖「未接线」。（K0 已在册；本守为幂等防撤。）");
    free(cmake);
}
