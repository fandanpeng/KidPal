/*
 * Frozen acceptance tests — TaskPackage 260924-m4-k2-p3-star-notebook-ui (subsystem:
 * firmware; P3 星星记录本展示面重构: v1.47 输入表矩阵 + 列表态重构 + 删除确认覆盖层 +
 * 阅读态对齐 + 盖章 seq 对账 + CJK 字体接线; rigor=full / riskTier=high / mergeOrder 39;
 * dependsOn 260924-m4-k1-story-cloud-link 已 merged)。
 *
 * Story 语义权威 = docs/requirements.md F2（S-F2.1 AC1–4 / S-F2.3 AC1–2 / S-F2.4 AC1–2
 * + §2.3 状态×手势全矩阵 v1.47 + 补充规则）。契约形状权威 = docs/contracts/
 * interface-contract.md §1.3③（story_list: items 最新 <=16 窗口 + total 恒全量、板端
 * 计数恒信 total）/ §1.3④（story_text.data.seq = 盖章 N 唯一权威源、缺省降级板端自定
 * Q4）/ §四（kidpal_story_delete: 端→网关单向私有消息、隐式 ack 超时重发 Q5、story_list
 * 重推对账）。旧冻结面 story_p3.acceptance.c 已 archive（test/acceptance/archive/），
 * 本文件为其继任：页内动作码闭集整体重钉、旧 offline 契约退役（story_page.h FROZEN
 * 头注由 builder 随落地重写声明退役）。
 *
 * Nature: 行为级（纯逻辑决策表, weak 哨兵模式——story_page.c 未编入 host runner 前
 * 哨兵恒红 = 「frozen contract not implemented」设计态红）+ 结构级（真实工作树扫描,
 * kld 先例形态）+ 真帧探针（LVGL 软渲染 seam, p1_cjk_name 先例形态）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [K1] v1.47 输入表改形（home = firmware/main/display/story_page.c
 *        kidpal_story_page_input(kidpal_story_view_t*, key_id_t, key_gesture_t)——
 *        签名零改、语义整体重写为 §2.3 矩阵 P3 三行 + 删除确认子态行）。动作码闭集
 *        重钉（旧闭集 DEGRADE-3/GLOBAL-2/NONE0/CURSOR_NEXT1/OPEN_READER2/PAGE_TURN3/
 *        TTS_READ4/STOP_RECORD5/CANCEL_RECORD6 随 archive 退役；mcp_server.cc 本地
 *        镜像注释块同步 = builder 义务）。新闭集（数值 = 本文件 KSPV2_ACT_* 冻结值）:
 *          KSPV2_ACT_DEGRADE        (-3)  畸形入参/越界键位（PWR 面）——零污染
 *          KSPV2_ACT_GLOBAL         (-2)  页不拥有该手势 -> 交回全局路由层
 *          KSPV2_ACT_NONE             0   页拥有但无副作用（不响应 = 消费不动）
 *          KSPV2_ACT_REC_SESSION      7   LIST  KEY 短按 -> 记录语境会话（K3 会话
 *                                         动作接线消费位; 本包只钉语义位）
 *          KSPV2_ACT_OPEN_READER      2   LIST  KEY 双击 -> 进阅读态（语义与旧码
 *                                         同位, 零回归守卫）
 *          KSPV2_ACT_CURSOR_NEXT      3   LIST  BOOT 短按 -> 光标下移（屏感知+wrap）
 *          KSPV2_ACT_SCREEN_JUMP      4   LIST  BOOT 双击 -> 翻下一屏落该屏首项
 *          KSPV2_ACT_DELETE_ENTER     5   LIST  BOOT 长按 -> 删除确认子态
 *          KSPV2_ACT_DELETE_CONFIRM   6   DELETE KEY 短按 -> 确认删除（本地即时移除
 *                                         + 计数 -1 + 发送 kidpal_story_delete）
 *          KSPV2_ACT_DELETE_CANCEL    7   DELETE BOOT 任一手势 / 10s idle -> 取消回列表
 *          KSPV2_ACT_PAGE_TURN        8   READER BOOT 短按（双击同短按）-> 翻屏 wrap
 *          KSPV2_ACT_READER_EXIT      9   READER KEY 长按 -> 退出阅读回列表
 *          KSPV2_ACT_STOP_RECORD     10   RECORDING KEY 短按 & KEY 长按同义（保存,
 *                                         防长按误触毁稿 v1.47）
 *          KSPV2_ACT_DISCARD_RECORD  11   RECORDING BOOT 任一手势（丢弃退回列表）
 *        KSA_ACT_TTS_READ 退役（朗读 v1.52 已砍）。页枚举 additive:
 *          KSTORY_PAGE_DELETE = 4（LIST0/READER1/RECORDING2/STAMP3 之后追加）。
 *        决策表（矩阵逐格，v1.47 requirements §2.3）:
 *          LIST:    KEY 短=REC_SESSION / KEY 双=OPEN_READER / KEY 长=GLOBAL(回首页) /
 *                   BOOT 短=CURSOR_NEXT / BOOT 双=SCREEN_JUMP / BOOT 长=DELETE_ENTER
 *          DELETE:  KEY 短=DELETE_CONFIRM / KEY 双=NONE / KEY 长=NONE（不响应=消费
 *                   不动, 绝不 GLOBAL——漏网手势会被通用覆盖层列误映射 SESSION_TOGGLE,
 *                   删除确认里绝不能开会话）/ BOOT 短·双·长=DELETE_CANCEL
 *          READER:  KEY 短=NONE（朗读已砍, 且 tts_requests 零递增）/ KEY 双=NONE /
 *                   KEY 长=READER_EXIT / BOOT 短=PAGE_TURN / BOOT 双=PAGE_TURN /
 *                   BOOT 长=NONE（页循环 P3 断开, 不离页）
 *          RECORDING: KEY 短=STOP_RECORD / KEY 长=STOP_RECORD（同短按）/ BOOT
 *                   短·双·长=DISCARD_RECORD
 *          STAMP:   全手势=GLOBAL（演出帧页内零语义, 零回归守卫）
 *          空列表(count<=0): KEY 短=REC_SESSION（开录不受空列表影响）/ BOOT
 *                   短·双=NONE / BOOT 长=NONE（无行可删）
 *        DELETE 确认本地半（边角⑨）: 条目本地即时移除（count-- / counter-- / cursor
 *                   收敛合法界）+ pending_delete_id 记账（经 [K5] 探针观察）；云端
 *                   story_list 重推到达后经 [K5] reconcile 对账冲销——板端不造状态机
 *                   第二真相。
 *
 *   [K2] 300ms 单双击消歧纯逻辑三缝（home = story_page.c; 时间戳显式注入同 rec_tick
 *        纪律, 无真实 timer; 原语形态零共享态——状态归调用方/mcp 消费侧）:
 *          int kidpal_story_debounce_gate(int page);
 *            -- 状态门控（notes 边角②）: 仅 page == KSTORY_PAGE_LIST 返回 1（挂起
 *               消歧窗）; 其余页态返回 0（单击零延迟直发）。
 *          int kidpal_story_debounce_expired(uint32_t arm_ts_ms, uint32_t now_ms);
 *            -- 挂起单击到期判: now - arm_ts >= KSPV2_DISAMBIG_MS -> 1（直发单击动作）。
 *          int kidpal_story_debounce_cancel(uint32_t arm_ts_ms, uint32_t now_ms);
 *            -- 双击到达冲销判: 0 <= now - arm_ts < KSPV2_DISAMBIG_MS -> 1（冲销挂起
 *               单击、执行双击动作）; 到点后到达的迟到双击 -> 0（单击已直发）。
 *          冻结数值: KSPV2_DISAMBIG_MS = 300（决策点 3 裁定——死线**严格短于**板端
 *          iot_button 双击判定窗 350ms（waveshare-s3-rlcd-4.2.cc InitializeButtons
 *          double_gap_ms=350）, 消歧窗是双击窗的子集、防重复触发; test 内断言
 *          300 < 350 关系恒成立）。
 *
 *   [K3] 删除确认子态 10s idle-step（home = story_page.c; char_window
 *        kidpal_char_window_idle_step 同款签名/语义, 时长缩参 10s）:
 *          int kidpal_story_delete_idle_step(unsigned long *idle_ms, unsigned long dt_ms);
 *            -- 累计 idle 并判到点: >= KSPV2_DELETE_IDLE_MS(=10000) 返回 1（调用方
 *               静默取消回列表并复位）; 未到点 0; NULL -> -1。**静默**取消——不显示
 *               倒计时（负向结构齿承载）。任意手势重计 = 输入函数 DELETE 态消费侧义务
 *               （每次手势驱动即复位累计; host 行为齿不可达半, 强度申报见下）。
 *
 *   [K4] 隐式 ack 重发决策（home = story_page.c 纯逻辑 + mcp_server.cc tick 驱动;
 *        契约 §四 Q5: 发送后未收到不含该 id 的 story_list 重推即超时重发, 无应答
 *        消息对, front 幂等承接）:
 *          int kidpal_story_delete_resend_due(int tries, uint32_t age_ms);
 *            -- tries < KSPV2_DELETE_RESEND_MAX(=3) 且 age_ms >=
 *               KSPV2_DELETE_RESEND_MS(=5000) -> 1（调用方重发同一 id 并 tries++）;
 *               否则 0（未到期 / 已达上限放弃——放弃后静默停发, 不新增请求-应答消息对）。
 *          冻结数值（决策点 3）: 重发间隔 5000ms、上限 3 次（与契约 §三 网关提醒重推
 *          「上限 3 次」同档; story_list 重推秒级可达, 5s 间隔足以对账且不轰炸网关）。
 *
 *   [K5] story_list 重推对账冲销（home = story_page.c 纯逻辑 + mcp_server.cc 消费）:
 *          const char *kidpal_story_pending_delete_id(const kidpal_story_view_t *v);
 *            -- 待删 id 只读探针: 无 pending -> NULL; 有 -> NUL 结尾 id 串指针。
 *          int kidpal_story_delete_reconcile_hit(const char *pending_id,
 *                                                const kidpal_story_list_t *list);
 *            -- 重推到达判: pending_id 非空且命中 items 窗口 -> 1（仍存在, 不冲销）;
 *               不在窗口（含空窗口）/ pending 为空串 -> 0（调用方冲销 pending 并以
 *               list->total 刷新计数——counter 恒信 total）; NULL 入参 -> -1。
 *          发送原语（§四 消息形状 {"type":"kidpal_story_delete","id":"<story-id>"}）:
 *          protocol.h/.cc 增 SendKidpalStoryDelete(const char *id)（逐字仿
 *          SendAbortSpeaking 形态; application 层公开委托同缝, K3 的
 *          kidpal_session_ctx 发送同族复用）。id 闭集纪律: 只发 story_list items
 *          下发过的 id, 板端不造 id。
 *
 *   [K6] seq 只读探针（home = story_page.c; kidpal_story_text_t additive 尾部追加
 *        int seq 字段带缺省哨兵——冻结纪律: 只增不改/移, 新字段一律尾部追加）:
 *          #define KSTORY_SEQ_ABSENT (-1)
 *          int kidpal_story_text_seq(const char *data_json);
 *            -- story_text data 的 seq 值探针: 缺省 -> KSTORY_SEQ_ABSENT; 非负整数
 *               -> 值; 畸形 seq（非整数/字符串）-> KSTORY_SEQ_ABSENT 同径降级（seq 是
 *               additive 装饰字段, 不为它整包拒——正文照常入阅读态）。
 *
 *   [K7] 盖章 N = 入册流水号（S-F2.4 AC2 v1.47 拆分口径; 旧「N = counter+1 本地推」
 *        冻结决策 D3 退役; 易失 stamp_index 退役负锚——story_page.{c,h} + mcp_server.cc
 *        零 stamp_index 残留）:
 *          int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id,
 *                                    int seq);   -- 三参改形（旧两参退役）
 *            -- 语义: stamp N = seq（[K6] 探针取值）; seq 缺省 -> 降级演出不带 N;
 *               **counter 零触碰**（现存篇数权威恒 = story_list.total, 由重推刷新——
 *               板端不本地推计数）; 同一 saved_id 幂等（不重复演出）。
 *          int kidpal_story_stamp_serial(const kidpal_story_view_t *v);
 *            -- 入册流水号只读探针（未保存/缺省 -> KSTORY_SEQ_ABSENT）。
 *          int kidpal_story_stamp_text(int serial, char *out, size_t cap);
 *            -- 文案逐字（二值决策表）:
 *               serial >= 0 -> 「星星徽章·第 %d 颗已入册！」（N = serial）
 *               serial <  0 -> 「星星徽章已入册！」（seq 缺省降级不带 N——契约 Q4
 *                              板端自定形态, 决策点 3 裁定文案）
 *               cap 不足 -> -1（拒绝, 绝不截断/溢出）; NULL -> -1。
 *
 *   [K8] LIST 分屏纯逻辑三缝（home = story_page.c; 原语形态, 行/屏数显式注入）:
 *          int kidpal_story_list_cursor_step(int cursor, int count, int rows,
 *                                            int screen, int *out_cursor,
 *                                            int *out_screen);
 *            -- BOOT 短按光标下移（S-F2.1 AC2）: cursor+1; 越过当前屏底 -> 翻下一屏
 *               并落该屏首项; 到底 -> wrap 回首项（**永不离页**）。count<=0 -> 0（零
 *               副作用）; rows<=0 -> 按 1 行/屏防御; NULL out -> -1; 其余 -> 1。
 *          int kidpal_story_list_screen_next(int screen, int screens, int rows,
 *                                            int count, int *out_screen,
 *                                            *out_cursor);
 *            -- BOOT 双击跳屏（S-F2.1 AC2）: screen=(screen+1)%screens, cursor 落该屏
 *               首项。screens<=0 / NULL out -> -1。
 *          int kidpal_story_list_screen_label(int screen, int screens, char *out,
 *                                             size_t cap);
 *            -- 分屏指示逐字（v1.28, 与阅读态同心智）: 「第 %d 屏 · 共 %d 屏」
 *               （1-based）; screen<0 / screens<1 / cap 不足 / NULL -> -1。
 *
 *   [K9] CJK 字体接线 + 真帧探针（home = story_page.c 渲染面; p1_cjk_name 先例形态;
 *        缺陷 = 现文全文件不设字体 -> LVGL 默认 montserrat_14 纯拉丁 -> P3 全部中文面
 *        真机空白）:
 *          -- 全 label 显式接线 font_puhui_14_1（lcd_display.cc 五处先例同款）,
 *             不赌继承: story_page.c 内 "font_puhui_14_1" >= 5 处（结构齿）;
 *          -- 真帧非空白探针: kidpal_story_page_render 后经 host_infra_render_frame /
 *             host_infra_read_pixel（host_infra_seam.c 既有 seam, 本文件 weak 桩,
 *             runner 内被强定义覆盖）在 LIST 标题带 / STAMP 文案带探 CJK 墨——默认
 *             字体下 CJK 不落墨, 探针必红; font_puhui_14_1 接线后转绿。版式坐标不钉
 *             （像素级呈现归真机轮 + design review, 探针带按线框语义粗带）。
 *
 *   [K10] 层值接线（1g 裁读: M2-K3 先例符号级插入 + 上移）:
 *          #define KIDPAL_LAYER_STORY_DELETE 6   （新层值, home = kidpal_layer.h）
 *          KIDPAL_LAYER_REMINDER 6 -> 7;  KIDPAL_LAYER_PROVISION 7 -> 8。
 *          约束: STORY_DELETE < REMINDER（提醒最高层压删除弹窗, 序不变——矩阵口径）;
 *          全仓消费点符号常量引用零数值字面耦合 = 符号级安全。kidpal_layer.c
 *          pop_transients transient 集纳入 STORY_DELETE; kidpal_render_node.cc
 *          front_is_overlay 闭集纳入 STORY_DELETE（删除弹窗无页点同源成立）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: S-F2.1-AC1 列表呈现/计数/分屏 -> SPV2-A7（counter 恒信 total 守卫）+
 *             SPV2-A2（分屏纯逻辑 + 分屏指示逐字）+ SPV2-B1（CJK 真帧）+ SPV2-B2
 *             （页内无标题负锚 / 选中反白退役 / 空态单源）
 *   coverage: S-F2.1-AC2 列表手势 -> SPV2-A1（LIST 决策表）+ SPV2-A2（cursor_step/
 *             screen_next）+ SPV2-A3（300ms 消歧仅列表态）
 *   coverage: S-F2.1-AC3 常驻语音入口双指令 -> SPV2-B2（「我要讲故事」+「帮我记下来」
 *             逐字）+ SPV2-B1（真帧）
 *   coverage: S-F2.1-AC4 删除确认 -> SPV2-A1（DELETE_ENTER）+ SPV2-A4（子态决策表/
 *             本地移除/idle_step）+ SPV2-A5（隐式 ack 重发/对账冲销）+ SPV2-A9（层值
 *             序不变）+ SPV2-B3（发送原语/板级全手势族接线）
 *   coverage: S-F2.2-AC1 KEY 开记录语境会话语义位 -> SPV2-A1（REC_SESSION 动作码;
 *             会话动作接线归 K3——本包只钉页内缝, 未覆盖面申报在案）
 *   coverage: S-F2.3-AC1 阅读态分屏 -> SPV2-A6（open_reader/screen_label/reader_slice
 *             >12 屏）+ SPV2-B1（真帧）
 *   coverage: S-F2.3-AC2 阅读态手势 -> SPV2-A6（KEY 短按不响应/翻屏 wrap 永不离页/
 *             BOOT 长按 NONE）+ SPV2-B2（TTS_READ 退役负锚）
 *   coverage: S-F2.4-AC1 盖章演出 -> SPV2-A8（文案逐字二值决策表）+ SPV2-B1（真帧）
 *   coverage: S-F2.4-AC2 计数/N 双口径 -> SPV2-A7（counter 恒信 total）+ SPV2-A8
 *             （N=seq 只增不减、counter 零本地推、stamp_index 退役负锚）
 *   coverage: 契约 §1.3③ story_list 形状消费 -> SPV2-A7（16 窗口降级/total 缺省回退
 *             守卫）
 *   coverage: 契约 §1.3④ story_text.seq -> SPV2-A8（seq 探针三值决策表）
 *   coverage: 契约 §四 kidpal_story_delete Q5 -> SPV2-A4/A5/B3
 *   coverage: 跨包承接（K1 seq 账本/删除链路, carriedBy = 本包）-> SPV2-A8 + SPV2-A5
 *             + SPV2-B3
 *   coverage: SPV2-C1 注册序守卫 + 需求/契约 canary + 扫描器自检
 *
 * ---------------------------------------------------------------------------
 * Run protocol:
 *   0) builder FIRST ACTION: 本文件注册进
 *      firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *      （相对 host_app/main 为 "../../story_page_v2.acceptance.c"），连同被测实现
 *      "../../../display/story_page.c"（其 art 引用由在世 art_assets.c 强符号 /
 *      p1_home_battery 在世弱桩覆盖, 无需另接），**置于 "../../host_infra_smoke.
 *      acceptance.c" 之前**（smoke 末位 = 最先执行, 注册序守卫; SPV2-C1 tooth 1 校验）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/
 *      host_infra_test.elf
 *   RED baseline 判读口径见文件尾。
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载断言 -> 性质档; 弱于契约处逐条）:
 *   §1.3③ total 恒全量/16 窗口 -> SPV2-A7 行为齿 -> 行为级（解析面全承载）
 *   §1.3④ seq 唯一权威源 -> SPV2-A8 行为齿 -> 行为级（板端消费半; 账本 owner=K1）
 *   §四 kidpal_story_delete 单向+隐式 ack -> SPV2-A4/A5 行为齿 + SPV2-B3 结构齿 ->
 *         行为级纯逻辑 + 机制级接线（wire 真发面 host 不可达——live 复证挂 K3,
 *         crossPackageBehaviors 承接条在案）
 *   S-F2.1 AC1 选中行带边框 -> SPV2-B2 机制级（反白退役负锚 + LIST 切区 border 正锚）
 *         ——**弱于契约**: 边框像素观感未钉（版式归 PNG/design review + 真机 1bpp 目测
 *         挂 K3）; 行为等价假设 = 反白填充退役且选中行走 border 样式路径即承载 AC1
 *         「带边框不反白」语义。
 *   S-F2.1 AC3 双指令逐字 -> SPV2-B2 结构齿 + SPV2-B1 真帧 -> 源锚代理 + 真帧级
 *   S-F2.3 排版 13.5px/行高 1.72 -> SPV2-B2 仅钉 KSTORY_READER_LINES_PER_SCREEN
 *         单源在世（值 = builder 按 NFR-READ-1 实测重冻, 本文件不锁数值——锁数值会
 *         与实测排版常量冲突）; >12 截断回收 = SPV2-A6 行为齿（15 屏切片可达）。
 *   300ms 消歧手感（可感知延迟已接受） -> SPV2-A3 纯逻辑齿 -> 机制级; 真实手感挂 K3
 *         设备轮（deferred 在案）。
 *   DELETE 任意手势重计（10s 无操作） -> SPV2-A4 纯逻辑齿只承载到点判与累计;
 *         重计接线 = mcp 消费侧义务（SPV2-B3 needle kidpal_story_delete_idle_step
 *         在 mcp_server.cc 在世）——行为等价假设: 输入函数 DELETE 态每次手势消费
 *         即复位累计; 该半 host 不可达, 记入盲区申报。
 *   空态文案两处一致（边角③） -> SPV2-B2 单源机制齿（KSTORY_EMPTY_HINT 三处在世:
 *         story_page.h 定义 + story_page.c / kidpal_render_node.cc 消费）——机制级;
 *         具体文案归 PNG/design review, 不钉逐字。
 *   fixture 自含性: 全部行为齿 fixture = 手构 kidpal_story_list_t / 手写 JSON 串,
 *         无资产/时间真值/环境前提; 真帧探针依赖 host LVGL 软渲染 seam（host 前提 =
 *         host_infra_seam.c 强符号在世）——host/真机近似假设: host ARGB8888 软渲染
 *         的墨判（暗像素）是 1bpp 真机墨的过集（真机半挂 K3 申报）。
 *   盲区申报: ① 板级 P3 全手势族分流的真实触发（board OnClick/OnDoubleClick/长按
 *         分流 wiring）= 结构齿承载, 行为半真机挂 K3; ② story_list 重推真实网络到达
 *         = K1/live 面; ③ 「进页即见列表」时点保障 = K3 重裁（本文件禁预钉建连即推
 *         ——决策点 6 硬约束, 全文件无建连时点断言）。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/* 稳定在世底座（key 枚举/手势枚举/层值头, 已编入 host runner）。 */
#include "../key_gesture_parser.h"
#include "../display/kidpal_layer.h"

/* 修订 2026-09-29 R5: LVGL 门控 include（schedule_grid / p1_cjk_name 先例形态——
 * host runner 通道 lvgl 在 include path, 独立红跑 stub 通道无 -> 编译出）。 */
#if defined(__has_include)
#  if __has_include("lvgl.h")
#    include "lvgl.h"
#    define KSPV2_HAVE_LVGL 1
#  endif
#endif

#define KSPV2_MAX_PATH 1024
#define KSPV2_MAX_READ (8u * 1024u * 1024u)

/* 板级装配文件（P3 手势族分流接线 home, seam [K5] 结构齿）。 */
#define KSPV2_BOARD_CC "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"

/* -------------------------------------------------------------------------- *
 * 类型镜像与哨兵（story_page.c 尚未编入 host runner -> 全部 story 符号 weak 哨兵）。
 * 类型镜像 = story_page.h 冻结形状的**公共前缀镜像**（页态/列表/正文三 struct 的
 * 冻结字段逐字一致; 落地后 builder 对 story_page.h 只做 additive 尾部追加, 公共
 * 前缀布局不变 -> 镜像与真实类型 ABI 兼容。story_page.h 本头被 builder 独占重写,
 * 本文件不 include 它以避免与 [K7] 三参 on_saved 改形的声明冲突——镜像即冻结形状
 * 的第二份逐字拷贝, 若头改了冻结字段 = 双处同时红）。
 * ------------------------------------------------------------------------ */
#define KSPV2_UNIMPLEMENTED (-9999)

#ifndef KSTORY_LIST_MAX
#define KSTORY_LIST_MAX 16
#endif
#ifndef KSTORY_TOTAL_ABSENT
#define KSTORY_TOTAL_ABSENT (-1)
#endif
#ifndef KSTORY_SEQ_ABSENT
#define KSTORY_SEQ_ABSENT (-1)
#endif
/* 测试内别名（与在世常量同值单源, 不另立第二真相）。 */
#define KSPV2_TOTAL_ABSENT KSTORY_TOTAL_ABSENT
#ifndef KSPV2_SEQ_ABSENT
#define KSPV2_SEQ_ABSENT KSTORY_SEQ_ABSENT
#endif
#ifndef KSPV2_LIST_MAX
#define KSPV2_LIST_MAX KSTORY_LIST_MAX
#endif

typedef struct {
    char id[64];
    char title[96];
    char date[16];
} kspv2_story_item_t;

typedef struct {
    int total;
    int count;
    kspv2_story_item_t items[KSTORY_LIST_MAX];
} kspv2_story_list_t;

typedef struct {
    char id[64];
    char title[96];
    char text[2048];
} kspv2_story_text_t;

/* kidpal_story_view_t 冻结字段公共前缀镜像（与 story_page.h 逐字同序同型）。 */
typedef struct {
    int page;
    int cursor;
    int count;
    int counter;
    int screen;
    int screens;
    int rec_seconds;
    int stamp_index; /* 槽位对位真实 stamp_serial（同 offset 同型 int; 语义改名随 seam [K7], 镜像名不影响 ABI） */
    int archive_pend;
    int cancel_count;
    int tts_requests;
    char last_saved_id[64];
    /* additive 尾部（修订 2026-09-29 R4）: 真实 kidpal_story_view_t 尾部追加字段逐字
     * 镜像（seam [K5] 冻结的 pending_delete_id 记账字段）。旧镜像止于 last_saved_id
     * （108B < 真实 172B）——真实实现写 pending 记账即越界写破栈上相邻 fixture
     * （SPV2-A4 tooth 2「取消零副作用」红根因; host 实证: confirm 后调用方 list.
     * count 被打爆为垃圾值 = 冻结 seam [K5] 契约在旧镜像下对任何实现不可满足）。 */
    char pending_delete_id[64];
} kspv2_story_view_t;

/* 页态闭集（LIST/READER/RECORDING/STAMP = 在世值; DELETE = [K1] additive 追加位）。 */
#define KSPV2_PAGE_LIST       0
#define KSPV2_PAGE_READER     1
#define KSPV2_PAGE_RECORDING  2
#define KSPV2_PAGE_STAMP      3
#define KSPV2_PAGE_DELETE     4

/* 动作码闭集重钉（seam [K1]; 冻结数值, 见头注决策表）。 */
#define KSPV2_ACT_DEGRADE        (-3)
#define KSPV2_ACT_GLOBAL         (-2)
#define KSPV2_ACT_NONE             0
#define KSPV2_ACT_OPEN_READER      2
#define KSPV2_ACT_CURSOR_NEXT      3
#define KSPV2_ACT_SCREEN_JUMP      4
#define KSPV2_ACT_DELETE_ENTER     5
#define KSPV2_ACT_DELETE_CONFIRM   6
#define KSPV2_ACT_DELETE_CANCEL    7
#define KSPV2_ACT_REC_SESSION      7
#define KSPV2_ACT_PAGE_TURN        8
#define KSPV2_ACT_READER_EXIT      9
#define KSPV2_ACT_STOP_RECORD     10
#define KSPV2_ACT_DISCARD_RECORD  11

/* 决策点 3 冻结数值（seam [K2]/[K3]/[K4]）。 */
#define KSPV2_DISAMBIG_MS        300u
#define KSPV2_BOARD_DOUBLE_MS    350u   /* 板端 iot_button double_gap_ms 既有值（零触碰参照） */
#define KSPV2_DELETE_IDLE_MS     10000UL
#define KSPV2_DELETE_RESEND_MS   5000UL
#define KSPV2_DELETE_RESEND_MAX  3

/* 层值接线 fallback（seam [K10]; 落地前 STORY_DELETE 与在世 REMINDER 同值 6 -> 值
 * 断言必红 = 真实缺失行为红, 非编译期作弊）。 */
#ifndef KIDPAL_LAYER_STORY_DELETE
#define KIDPAL_LAYER_STORY_DELETE 6
#endif

/* ---- 在世 story 符号 weak 哨兵（强定义 home = story_page.c; 落地进 runner 后覆盖）---- */
__attribute__((weak)) int kidpal_story_view_init(kspv2_story_view_t *v, const kspv2_story_list_t *list)
{ (void)v; (void)list; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_page_input(kspv2_story_view_t *v, key_id_t key, key_gesture_t g)
{ (void)v; (void)key; (void)g; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_list_parse(const char *data_json, kspv2_story_list_t *out)
{ (void)data_json; (void)out; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_counter(const kspv2_story_list_t *list)
{ (void)list; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_text_parse(const char *data_json, kspv2_story_text_t *out)
{ (void)data_json; (void)out; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_open_reader(kspv2_story_view_t *v, const kspv2_story_text_t *st, int screen_lines)
{ (void)v; (void)st; (void)screen_lines; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_screen_label(const kspv2_story_view_t *v, char *out, size_t cap)
{ (void)v; (void)out; (void)cap; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) const char *kidpal_story_reader_slice(const kspv2_story_text_t *st, int screen, int lines_per_screen, int *out_len)
{ (void)st; (void)screen; (void)lines_per_screen; (void)out_len; return NULL; }
__attribute__((weak)) int kidpal_story_page_render(const kspv2_story_view_t *v, const kspv2_story_list_t *list, const kspv2_story_text_t *story)
{ (void)v; (void)list; (void)story; return KSPV2_UNIMPLEMENTED; }

/* ---- 新缝 weak 哨兵（seam [K2]..[K8]; 签名 = 头注冻结形状）---- */
__attribute__((weak)) int kidpal_story_debounce_gate(int page)
{ (void)page; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_debounce_expired(uint32_t arm_ts_ms, uint32_t now_ms)
{ (void)arm_ts_ms; (void)now_ms; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_debounce_cancel(uint32_t arm_ts_ms, uint32_t now_ms)
{ (void)arm_ts_ms; (void)now_ms; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_delete_idle_step(unsigned long *idle_ms, unsigned long dt_ms)
{ (void)idle_ms; (void)dt_ms; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_delete_resend_due(int tries, uint32_t age_ms)
{ (void)tries; (void)age_ms; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_delete_reconcile_hit(const char *pending_id, const kspv2_story_list_t *list)
{ (void)pending_id; (void)list; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) const char *kidpal_story_pending_delete_id(const kspv2_story_view_t *v)
{ (void)v; return NULL; }
__attribute__((weak)) int kidpal_story_text_seq(const char *data_json)
{ (void)data_json; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_stamp_serial(const kspv2_story_view_t *v)
{ (void)v; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_on_saved(kspv2_story_view_t *v, const char *saved_id, int seq)
{ (void)v; (void)saved_id; (void)seq; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_stamp_text(int serial, char *out, size_t cap)
{ (void)serial; (void)out; (void)cap; return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_list_cursor_step(int cursor, int count, int rows, int screen,
                                                        int *out_cursor, int *out_screen)
{ (void)cursor; (void)count; (void)rows; (void)screen; (void)out_cursor; (void)out_screen;
  return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_list_screen_next(int screen, int screens, int rows, int count,
                                                        int *out_screen, int *out_cursor)
{ (void)screen; (void)screens; (void)rows; (void)count; (void)out_screen; (void)out_cursor;
  return KSPV2_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_list_screen_label(int screen, int screens, char *out, size_t cap)
{ (void)screen; (void)screens; (void)out; (void)cap; return KSPV2_UNIMPLEMENTED; }

/* host_infra seam 弱桩（强定义 home = test/host_app/main/host_infra_seam.c, runner
 * 内覆盖; 独立红跑/未接线态下返回 not-rendered -> 真帧探针红因 = 契约未实现, 不崩）。 */
#define KSPV2_NOT_RENDERED (-1)
__attribute__((weak)) int host_infra_render_frame(void)
{ return KSPV2_NOT_RENDERED; }
__attribute__((weak)) int32_t host_infra_read_pixel(int x, int y)
{ (void)x; (void)y; return KSPV2_NOT_RENDERED; }

/* -------------------------------------------------------------------------- *
 * 工具: 哨兵必查 + 格式化消息 + 仓根定位/树扫描（kld 先例形态; 扫描跑真实工作树,
 * root 自手持 cwd 定位——仓根或 firmware/main/test/host_app 两处可跑, root 注入
 * 纪律 = 不依赖调用 cwd 的绝对路径假设）。
 * ------------------------------------------------------------------------ */

static void kspv2_require_impl(int value, const char *what)
{
    if (value == KSPV2_UNIMPLEMENTED || value == KSPV2_NOT_RENDERED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "SPV2 RED: frozen seam NOT implemented — %s（builder 落地 story_page.c "
                 "重写面并接线进 HOST_ACCEPTANCE_SRCS 后转绿; 红因 = 冻结契约未实现, "
                 "见 story_page_v2.acceptance.c 头注 seam contract）", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* 修订 2026-09-29 R5: render 后直刷原语（schedule_grid 先例 ksg_render_snap 同款
 * 时序）。host runner 通道 = lvgl 强符号 lv_refr_now（真刷被测内容进 seam 帧缓冲）;
 * 独立红跑 stub 通道 = no-op（该通道 render 哨兵先行 abort, 不会到达本调用——
 * 设计态红语义不变）。 */
static void kspv2_refr_now(void)
{
#ifdef KSPV2_HAVE_LVGL
    lv_refr_now(NULL);
#endif
}

static char kspv2_fm_buf[1024];
static const char *kspv2_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kspv2_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kspv2_fm_buf, sizeof(kspv2_fm_buf), fmt, ap);
    va_end(ap);
    return kspv2_fm_buf;
}

static char kspv2_root[KSPV2_MAX_PATH];
static int kspv2_root_ready;

static int kspv2_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kspv2_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kspv2_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int kspv2_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KSPV2_MAX_PATH];

    if (kspv2_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kspv2_is_regular(probe)) {
            snprintf(kspv2_root, sizeof(kspv2_root), "%s", cands[i]);
            kspv2_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kspv2_rel(char *out, size_t cap, const char *rel)
{
    if (!kspv2_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", kspv2_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", kspv2_root, rel);
    return 1;
}

static int kspv2_exists(const char *rel)
{
    char p[KSPV2_MAX_PATH];
    return kspv2_rel(p, sizeof(p), rel) && kspv2_is_regular(p);
}

static char *kspv2_read(const char *rel, size_t *out_len)
{
    char p[KSPV2_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kspv2_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSPV2_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int kspv2_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL);
    return strstr(hay, needle) != NULL;
}

static int kspv2_file_contains(const char *rel, const char *needle)
{
    char *buf = kspv2_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kspv2_contains(buf, needle);
    free(buf);
    return r;
}

static int kspv2_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = kspv2_read(rel, NULL);
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

static char *kspv2_region_between(const char *rel, const char *start_needle,
                                  const char *end_needle)
{
    char *buf = kspv2_read(rel, NULL);
    char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int kspv2_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

/* 递归树扫描: 统计 rel_dir 下命中 needle 的文件数（test/build/.git/assets 目录跳过
 * ——test 目录住着本冻结文件自身, 其注释合法提及 needle）。首命中文件经 first_hit
 * 回传（诊断用）。 */
static int kspv2_scan_walk(const char *abs_dir, const char *rel_prefix, const char *needle,
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
        char abs_next[KSPV2_MAX_PATH];
        char rel_next[KSPV2_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            kspv2_is_skipped_dir(e->d_name)) {
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
            count += kspv2_scan_walk(abs_next, rel_next, needle, first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode) && (unsigned long long)st.st_size <= KSPV2_MAX_READ) {
            char *buf = kspv2_read(rel_next, NULL);
            if (buf != NULL) {
                if (kspv2_contains(buf, needle)) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
                free(buf);
            }
        }
    }
    closedir(d);
    return count;
}

static int kspv2_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                            size_t cap)
{
    char abs[KSPV2_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!kspv2_rel(abs, sizeof(abs), rel_dir) || !kspv2_is_dir(abs)) {
        return 0;
    }
    return kspv2_scan_walk(abs, rel_dir, needle, first_hit, cap, 0);
}

#define KSPV2_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kspv2_locate_root(), \
    "SPV2 RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app")

/* 手构 fixture 助手: 三行列表（ids/title 可覆盖）。 */
static void kspv2_make_list(kspv2_story_list_t *l, int total)
{
    memset(l, 0, sizeof(*l));
    l->total = total;
    l->count = 3;
    snprintf(l->items[0].id, sizeof(l->items[0].id), "2026-08-20-a");
    snprintf(l->items[0].title, sizeof(l->items[0].title), "小兔子找朋友");
    snprintf(l->items[0].date, sizeof(l->items[0].date), "08-20");
    snprintf(l->items[1].id, sizeof(l->items[1].id), "2026-08-25-b");
    snprintf(l->items[1].title, sizeof(l->items[1].title), "勇敢的小明");
    snprintf(l->items[1].date, sizeof(l->items[1].date), "08-25");
    snprintf(l->items[2].id, sizeof(l->items[2].id), "2026-08-27-c");
    snprintf(l->items[2].title, sizeof(l->items[2].title), "我的大冒险");
    snprintf(l->items[2].date, sizeof(l->items[2].date), "08-27");
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — SPV2-A1: v1.47 输入表 LIST 态决策表（behavioral, seam [K1]）
 * 列表态六手势动作码逐格断言 + 空列表特例。RED today: 哨兵 -9999（story_page.c 未
 * 接线）/ 旧码（KEY 短按 = 旧 CURSOR_NEXT、BOOT 族 = 旧 GLOBAL——与 v1.47 矩阵逐格
 * 对不上）。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC2 -- 列表态六手势决策表（KEY 短按=记录语境会话语义位 / BOOT 短按=光标下移 / BOOT 双击=跳屏 / BOOT 长按=删除确认子态） */
/* coverage: S-F2.2-AC1 -- KEY 开记录语境会话页内语义位（K3 会话动作接线承接, 未覆盖面申报在案） */
/* coverage: SPV2-A1 -- LIST 态 v1.47 决策表（六手势动作码逐格 + 空列表特例 + DEGRADE guard） */
TEST_CASE("story_page_v2: LIST 态 v1.47 决策表——六手势+空列表特例 (SPV2-A1)",
          "[story_page_v2][SPV2-A1][behavioral]")
{
    kspv2_story_view_t v;
    kspv2_story_list_t l;
    int act;

    kspv2_make_list(&l, 3);
    kspv2_require_impl(kidpal_story_view_init(&v, &l), "kidpal_story_view_init");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &l) == 0,
        "SPV2-A1 RED: kidpal_story_view_init 非零——列表视图装载底座回归（fixture 无法建立）");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSPV2_PAGE_LIST && v.cursor == 0 && v.counter == 3,
        kspv2_fm("SPV2-A1 RED(infra): view_init 语义漂移 page=%d cursor=%d counter=%d, "
                 "期望 LIST/0/3——fixture 前提被破坏", v.page, v.cursor, v.counter));

    /* tooth 1: KEY 短按 = KSPV2_ACT_REC_SESSION(7)——记录语境会话语义位（旧码
     * CURSOR_NEXT=1 / 旧空表 NONE=0 都与本值错位, 不会假绿）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    kspv2_require_impl(act, "kidpal_story_page_input");
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_REC_SESSION,
        kspv2_fm("SPV2-A1 RED: LIST 态 KEY 短按动作码 = %d, 期望 REC_SESSION(=%d)——"
                 "v1.47 矩阵「KEY = 开记录语境会话」未进输入表（旧码残留）",
                 act, KSPV2_ACT_REC_SESSION));

    /* tooth 2: KEY 双击 = OPEN_READER(2)（零回归守卫: 语义与旧码同位）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK)
            == KSPV2_ACT_OPEN_READER,
        "SPV2-A1 RED: LIST 态 KEY 双击动作码漂移——「KEY 双击进阅读」（S-F2.1 AC2）"
        "被输入表重写误伤");

    /* tooth 3: KEY 长按 = GLOBAL（回首页, 列表 = P3 顶层一层到家; 补充规则出路口径）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS) == KSPV2_ACT_GLOBAL,
        "SPV2-A1 RED: LIST 态 KEY 长按不再交全局——「KEY 长按回首页」路径被误伤");

    /* tooth 4: BOOT 短按 = CURSOR_NEXT(3)（旧码 GLOBAL=-2 错位）。 */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_CURSOR_NEXT,
        kspv2_fm("SPV2-A1 RED: LIST 态 BOOT 短按动作码 = %d, 期望 CURSOR_NEXT(=%d)——"
                 "「选中项下移」未进输入表（旧码 BOOT=GLOBAL 残留, §2.3 对账④）",
                 act, KSPV2_ACT_CURSOR_NEXT));

    /* tooth 5: BOOT 双击 = SCREEN_JUMP(4)。 */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_SCREEN_JUMP,
        kspv2_fm("SPV2-A1 RED: LIST 态 BOOT 双击动作码 = %d, 期望 SCREEN_JUMP(=%d)——"
                 "「翻下一屏落该屏首项」未进输入表（§2.3 对账③: 双击语义实现侧不存在）",
                 act, KSPV2_ACT_SCREEN_JUMP));

    /* tooth 6: BOOT 长按 = DELETE_ENTER(5) + 页态切删除确认子态（页值 4）。 */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_DELETE_ENTER && v.page == KSPV2_PAGE_DELETE,
        kspv2_fm("SPV2-A1 RED: LIST 态 BOOT 长按动作码 = %d page=%d, 期望 "
                 "DELETE_ENTER(=%d) 且页态 = 删除确认子态(%d)——删除确认子态入口未接 "
                 "（S-F2.1 AC4）", act, v.page, KSPV2_ACT_DELETE_ENTER, KSPV2_PAGE_DELETE));

    /* tooth 7: 空列表特例——KEY 短按仍 REC_SESSION（开录不受空列表影响）; BOOT 族
     * 全 NONE（无行可移/无屏可跳/无篇可删, 消费不动、绝不 GLOBAL 误开翻页）。 */
    {
        kspv2_story_list_t e;
        kspv2_story_view_t ev;

        memset(&e, 0, sizeof(e));
        e.total = 0;
        e.count = 0;
        TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&ev, &e) == 0,
            "SPV2-A1 RED(infra): 空列表 view_init 失败");
        act = kidpal_story_page_input(&ev, KEY_ID_KEY, KEY_GESTURE_CLICK);
        TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_REC_SESSION,
            kspv2_fm("SPV2-A1 RED: 空列表 KEY 短按动作码 = %d, 期望 REC_SESSION(=%d)"
                     "——开录不应受空列表影响（S-F2.2 双入口）",
                     act, KSPV2_ACT_REC_SESSION));
        TEST_ASSERT_TRUE_MESSAGE(
            kidpal_story_page_input(&ev, KEY_ID_BOOT, KEY_GESTURE_CLICK) == KSPV2_ACT_NONE &&
            kidpal_story_page_input(&ev, KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK) == KSPV2_ACT_NONE &&
            kidpal_story_page_input(&ev, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS) == KSPV2_ACT_NONE,
            "SPV2-A1 RED: 空列表 BOOT 族动作码非 NONE——空态页内应消费不动（NONE=0, "
            "绝不 GLOBAL 交回全局翻页）");
    }

    /* tooth 8 (guard): 越界键位（PWR 面）= DEGRADE 零污染。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, (key_id_t)99, KEY_GESTURE_CLICK) == KSPV2_ACT_DEGRADE,
        "SPV2-A1 RED(guard): 越界键位未按 DEGRADE 拒收——PWR 面零污染纪律被破坏");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — SPV2-A2: LIST 分屏纯逻辑决策表（behavioral, seam [K8]）
 * cursor_step（屏底翻屏落首项/到底 wrap 回首项）/ screen_next（跳屏落首项/末屏 wrap）/
 * 分屏指示逐字。RED today: 哨兵 -9999。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC1 -- 列表超一屏分屏指示逐字「第 x 屏 · 共 y 屏」（v1.28） */
/* coverage: S-F2.1-AC2 -- 光标下移屏感知步进（屏底翻屏落首项/到底 wrap 永不离页）+ BOOT 双击跳屏 */
/* coverage: SPV2-A2 -- LIST 分屏纯逻辑（cursor_step/screen_next/分屏指示逐字 + defensive 边界） */
TEST_CASE("story_page_v2: LIST 分屏纯逻辑——cursor_step/screen_next/分屏指示 (SPV2-A2)",
          "[story_page_v2][SPV2-A2][behavioral]")
{
    int c, s, r;

    /* ---- cursor_step: count=10, rows=4 -> screens=3 ---- */
    /* tooth 1: 屏内普通步进（cursor 0 -> 1, 屏不动）。 */
    c = -1; s = -1;
    r = kidpal_story_list_cursor_step(0, 10, 4, 0, &c, &s);
    kspv2_require_impl(r, "kidpal_story_list_cursor_step");
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && c == 1 && s == 0,
        kspv2_fm("SPV2-A2 RED: 屏内步进 r=%d cursor=%d screen=%d, 期望 1/1/0", r, c, s));

    /* tooth 2: 屏底再按 -> 翻下一屏并落该屏首项（cursor 3 -> 4, screen 0 -> 1, 且
     * cursor 落 4 = 第 2 屏首项, 非 5）。 */
    c = -1; s = -1;
    r = kidpal_story_list_cursor_step(3, 10, 4, 0, &c, &s);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && c == 4 && s == 1,
        kspv2_fm("SPV2-A2 RED: 屏底步进 r=%d cursor=%d screen=%d, 期望 1/4/1（落该屏"
                 "首项）——「光标到当前屏底再按翻下一屏并落该屏首项」语义缺失", r, c, s));

    /* tooth 3: 到底 wrap 回首项（cursor 9 -> 0, screen -> 0, 永不离页）。 */
    c = -1; s = -1;
    r = kidpal_story_list_cursor_step(9, 10, 4, 2, &c, &s);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && c == 0 && s == 0,
        kspv2_fm("SPV2-A2 RED: 到底步进 r=%d cursor=%d screen=%d, 期望 1/0/0——"
                 "「列表到底 wrap 回首项（永不离页）」缺失", r, c, s));

    /* tooth 4 (defensive): count<=0 -> 0 零副作用; rows<=0 -> 按 1 行/屏防御不崩。 */
    c = 5; s = 2;
    r = kidpal_story_list_cursor_step(0, 0, 4, 0, &c, &s);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && c == 5 && s == 2,
        kspv2_fm("SPV2-A2 RED: 空列表步进 r=%d cursor=%d screen=%d, 期望 0/5/2（零副作用）",
                 r, c, s));
    r = kidpal_story_list_cursor_step(0, 3, 0, 0, &c, &s);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && c == 1 && s == 1,
        kspv2_fm("SPV2-A2 RED: rows<=0 防御 r=%d cursor=%d screen=%d——须按 1 行/屏防御, "
                 "绝不除零/越界", r, c, s));

    /* ---- screen_next: BOOT 双击跳屏 ---- */
    /* tooth 5: 翻下一屏落该屏首项（screen 0 -> 1, cursor 落 4）。 */
    c = -1; s = -1;
    r = kidpal_story_list_screen_next(0, 3, 4, 10, &s, &c);
    kspv2_require_impl(r, "kidpal_story_list_screen_next");
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && s == 1 && c == 4,
        kspv2_fm("SPV2-A2 RED: 跳屏 r=%d screen=%d cursor=%d, 期望 1/1/4（光标落该屏首项）",
                 r, s, c));

    /* tooth 6: 末屏再双击 wrap 回首屏（screen 2 -> 0, cursor 落 0）。 */
    c = -1; s = -1;
    r = kidpal_story_list_screen_next(2, 3, 4, 10, &s, &c);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && s == 0 && c == 0,
        kspv2_fm("SPV2-A2 RED: 末屏跳屏 r=%d screen=%d cursor=%d, 期望 1/0/0——"
                 "「最后一屏再双击 wrap 回首屏」缺失", r, s, c));

    /* tooth 7 (defensive): screens<=0 -> -1; NULL -> -1。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_screen_next(0, 0, 4, 10, &s, &c) == -1,
        "SPV2-A2 RED: screens<=0 未按 -1 拒收");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_cursor_step(0, 3, 4, 0, NULL, NULL) == -1,
        "SPV2-A2 RED: NULL out 未按 -1 拒收");

    /* ---- 分屏指示逐字（v1.28, 与阅读态同心智） ---- */
    /* tooth 8: 「第 1 屏 · 共 3 屏」逐字（1-based）。 */
    {
        char buf[64];
        r = kidpal_story_list_screen_label(0, 3, buf, sizeof(buf));
        kspv2_require_impl(r, "kidpal_story_list_screen_label");
        TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(buf, "第 1 屏 · 共 3 屏") == 0,
            kspv2_fm("SPV2-A2 RED: 分屏指示 = 「%s」, 期望逐字「第 1 屏 · 共 3 屏」"
                     "（v1.28 与阅读态同心智）", buf));
        TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_screen_label(0, 0, buf, sizeof(buf)) == -1,
            "SPV2-A2 RED: screens<1 未按 -1 拒收");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_screen_label(0, 3, buf, 4) == -1,
            "SPV2-A2 RED: cap 不足未按 -1 拒绝（绝不截断/溢出）");
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — SPV2-A3: 300ms 单双击消歧纯逻辑（behavioral, seam [K2]）
 * 状态门控 = 仅列表态（notes 边角②）; 死线 300ms 与板端 iot_button 双击窗 350ms
 * 关系冻结（决策点 3: 死线严格短于双击判定窗, 防重复触发）。
 * RED today: 哨兵 -9999。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC2 -- 300ms 单双击消歧仅列表态状态门控（其余态单击零延迟, notes 边角②） */
/* coverage: SPV2-A3 -- 300ms 消歧纯逻辑（状态门控/到期判/冲销判/死线 300<350 关系冻结） */
TEST_CASE("story_page_v2: 300ms 消歧纯逻辑——状态门控+死线关系 (SPV2-A3)",
          "[story_page_v2][SPV2-A3][behavioral]")
{
    /* tooth 0 (guard): 决策点 3 关系冻结——死线 300 < 板端双击窗 350（消歧窗是
     * 双击窗的真子集, 防重复触发; 数值本文件单点冻结）。 */
    TEST_ASSERT_TRUE_MESSAGE(KSPV2_DISAMBIG_MS < KSPV2_BOARD_DOUBLE_MS,
        "SPV2-A3 RED(infra): 消歧死线 300ms 必须 < 板端 iot_button 双击窗 350ms"
        "（waveshare-s3-rlcd-4.2.cc double_gap_ms=350 零触碰参照）——防重复触发的"
        "时序前提被破坏");

    /* tooth 1: 状态门控——仅 LIST 态挂起消歧窗; 其余态（阅读/录音/盖章/删除子态）
     * 恒 0 = 单击零延迟直发。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_gate(KSPV2_PAGE_LIST) == 1,
        kspv2_fm("SPV2-A3 RED: LIST 态消歧门控 = %d, 期望 1——双击语义仅存在于列表态、"
                 "单击延迟消歧只在列表态生效（§2.3 dev 注记）",
                 kidpal_story_debounce_gate(KSPV2_PAGE_LIST)));
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_debounce_gate(KSPV2_PAGE_READER) == 0 &&
        kidpal_story_debounce_gate(KSPV2_PAGE_RECORDING) == 0 &&
        kidpal_story_debounce_gate(KSPV2_PAGE_STAMP) == 0 &&
        kidpal_story_debounce_gate(KSPV2_PAGE_DELETE) == 0,
        "SPV2-A3 RED: 非 LIST 态消歧门控非 0——消歧逻辑全局生效违反状态门控"
        "（其余状态单击必须零延迟, notes 边角②）");

    /* tooth 2: 到期判——挂起 300ms 整点直发（含端）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_expired(1000u, 1299u) == 0,
        "SPV2-A3 RED: 299ms 即判到期——消歧窗被缩短, 单击会抢在双击前误发");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_expired(1000u, 1300u) == 1,
        "SPV2-A3 RED: 300ms 整点未判到期——死线含端语义漂移（单击感知延迟超 300ms "
        "或永远不发）");

    /* tooth 3: 双击到达冲销判——窗内（< 300ms）冲销挂起单击; 到点后到达的迟到双击
     * 不冲销（单击已直发）。与 tooth 2 构成 [0,300) / [300,∞) 无缝划分。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_cancel(1000u, 1299u) == 1,
        "SPV2-A3 RED: 窗内双击到达未判冲销——列表态双击会先误发单击动作");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_debounce_cancel(1000u, 1300u) == 0,
        "SPV2-A3 RED: 到点后双击仍判冲销——与到期判区间重叠（无缝划分被破坏, "
        "同一时刻既发单击又冲销 = 双触发）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — SPV2-A4: 删除确认子态决策表 + 10s idle-step（behavioral, seam [K1]/[K3]）
 * KEY 短按确认（本地即时移除/计数 -1/pending 记账）+ KEY 双·长按不响应（NONE 非
 * GLOBAL——漏网手势会被通用覆盖层列误映射 SESSION_TOGGLE, 删除确认里绝不能开会话）+
 * BOOT 族取消 + idle-step 10s 静默取消（char_window 同款签名）。
 * RED today: 哨兵 / 旧实现页值 4 走 default -> GLOBAL。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC4 -- 删除确认子态决策表（确认/不响应/取消/10s 静默取消）+ 确认本地即时移除与计数 -1（边角⑨） */
/* coverage: 契约 §四 kidpal_story_delete -- 确认后 pending 记账（发送/重发/对账 = SPV2-A5/B3 承接） */
/* coverage: SPV2-A4 -- 删除确认子态决策表 + 10s idle-step（确认本地移除/不响应 NONE/BOOT 取消/静默取消） */
TEST_CASE("story_page_v2: 删除确认子态——确认/不响应/取消/10s idle-step (SPV2-A4)",
          "[story_page_v2][SPV2-A4][behavioral]")
{
    kspv2_story_view_t v;
    kspv2_story_list_t l;
    unsigned long idle;
    int act;

    kspv2_make_list(&l, 3);
    kspv2_require_impl(kidpal_story_view_init(&v, &l), "kidpal_story_view_init");

    /* fixture: 进入删除确认子态——**单次驱动**（修订 2026-09-29: 旧版此处另有一次
     * 探针调用 kspv2_require_impl(input(BOOT,LONG),…) 先行消费同一 BOOT 长按手势进
     * DELETE 子态, 断言处二次驱动同一手势按 tooth 2 冻结决策表必返 DELETE_CANCEL(7),
     * 与期望 DELETE_ENTER(5) 对同一可观测状态互斥 = 不可满足断言; 红基线未暴露因
     * view_init 哨兵红先行 abort。探针移除后「BOOT 长按进入删除子态」语义由本次单
     * 驱动承载——进入+确认两 tooth 覆盖不缩水, require_impl 保持 RED(infra) 哨兵红
     * 归因通道）。 */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    kspv2_require_impl(act, "kidpal_story_page_input DELETE 子态入口半");
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_DELETE_ENTER && v.page == KSPV2_PAGE_DELETE,
        kspv2_fm("SPV2-A4 RED: BOOT 长按动作码 = %d page=%d, 期望 DELETE_ENTER(=%d) 且"
                 "页态 = 删除确认子态(%d)——删除确认子态入口未接（S-F2.1 AC4）",
                 act, v.page, KSPV2_ACT_DELETE_ENTER, KSPV2_PAGE_DELETE));
    v.cursor = 1; /* fixture: 光标停在第 2 行（items[1].id = 2026-08-25-b）。 */

    /* tooth 1: KEY 短按 = 确认删除——本地即时移除（count 3->2 / counter 3->2 /
     * cursor 收敛合法界）+ pending_delete_id 记账 = 被删条目 id + 动作码 6。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    kspv2_require_impl(act, "kidpal_story_page_input DELETE 半");
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_DELETE_CONFIRM,
        kspv2_fm("SPV2-A4 RED: 删除子态 KEY 短按动作码 = %d, 期望 DELETE_CONFIRM(=%d)"
                 "——「KEY 短按 = 确认删除」未进输入表（S-F2.1 AC4）",
                 act, KSPV2_ACT_DELETE_CONFIRM));
    TEST_ASSERT_TRUE_MESSAGE(v.count == 2 && v.counter == 2,
        kspv2_fm("SPV2-A4 RED: 确认后 count=%d counter=%d, 期望 2/2——条目未本地即时"
                 "移除 / 星星计数未 -1（现存篇数口径, 边角⑨; 云端重推后以 total 校准）",
                 v.count, v.counter));
    TEST_ASSERT_TRUE_MESSAGE(v.cursor <= v.count - 1,
        kspv2_fm("SPV2-A4 RED: 确认后 cursor=%d 越界（count=%d）——cursor 未收敛合法界",
                 v.cursor, v.count));
    {
        const char *pid = kidpal_story_pending_delete_id(&v);
        TEST_ASSERT_TRUE_MESSAGE(pid != NULL && strcmp(pid, "2026-08-25-b") == 0,
            kspv2_fm("SPV2-A4 RED: pending_delete_id 探针 = %s, 期望 2026-08-25-b——"
                     "确认后删除待账未记（隐式 ack 重发与对账无锚）",
                     pid != NULL ? pid : "NULL"));
    }

    /* tooth 2: 取消回列表——BOOT 任一手势（短/双/长）= DELETE_CANCEL(7) + 页态回
     * LIST。fixture 重置后逐手势验证。 */
    {
        int g;
        static const key_gesture_t gestures[] = {
            KEY_GESTURE_CLICK, KEY_GESTURE_DOUBLE_CLICK, KEY_GESTURE_LONG_PRESS
        };
        for (g = 0; g < (int)(sizeof(gestures) / sizeof(gestures[0])); g++) {
            kidpal_story_view_init(&v, &l);
            v.page = KSPV2_PAGE_DELETE;
            v.cursor = 0;
            act = kidpal_story_page_input(&v, KEY_ID_BOOT, gestures[g]);
            TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_DELETE_CANCEL && v.page == KSPV2_PAGE_LIST,
                kspv2_fm("SPV2-A4 RED: 删除子态 BOOT 手势(g=%d)动作码 = %d page=%d, "
                         "期望 DELETE_CANCEL(=%d) 且回列表——「BOOT 任一手势 = 取消回列表」"
                         "未进输入表", (int)gestures[g], act, v.page,
                         KSPV2_ACT_DELETE_CANCEL));
            TEST_ASSERT_TRUE_MESSAGE(v.count == 3 && v.counter == 3,
                "SPV2-A4 RED: 取消后条目/计数被动过——取消是零副作用回列表");
        }
    }

    /* tooth 3: 不响应 = NONE(0) 且页态保持——KEY 双击/长按在删除子态消费不动,
     * 绝不 GLOBAL（漏网手势经通用覆盖层列误映射 SESSION_TOGGLE = 删除确认里开会话,
     * §2.3 矩阵删除行「不响应」两格）。 */
    kidpal_story_view_init(&v, &l);
    v.page = KSPV2_PAGE_DELETE;
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_NONE && v.page == KSPV2_PAGE_DELETE,
        kspv2_fm("SPV2-A4 RED: 删除子态 KEY 双击动作码 = %d page=%d, 期望 NONE(=%d) 且"
                 "页态保持——不响应格缺失（GLOBAL=-2 会漏网误开会话, K1 层派发教训）",
                 act, v.page, KSPV2_ACT_NONE));
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_NONE && v.page == KSPV2_PAGE_DELETE,
        kspv2_fm("SPV2-A4 RED: 删除子态 KEY 长按动作码 = %d page=%d, 期望 NONE 且页态"
                 "保持——「其余手势不响应」未覆盖（S-F2.1 AC4 尾句）", act, v.page));

    /* tooth 4: 10s idle-step（char_window kidpal_char_window_idle_step 同款签名/
     * 语义, 时长 10s）——累计 4000+4000 未到点 0; 再 3000 到点 1。 */
    idle = 0;
    act = kidpal_story_delete_idle_step(&idle, 4000UL);
    kspv2_require_impl(act, "kidpal_story_delete_idle_step");
    TEST_ASSERT_TRUE_MESSAGE(act == 0 && idle == 4000UL,
        kspv2_fm("SPV2-A4 RED: idle 累计 4000ms 返回 %d idle=%lu, 期望 0/4000", act, idle));
    act = kidpal_story_delete_idle_step(&idle, 4000UL);
    TEST_ASSERT_TRUE_MESSAGE(act == 0 && idle == 8000UL,
        kspv2_fm("SPV2-A4 RED: idle 累计 8000ms 返回 %d idle=%lu, 期望 0/8000", act, idle));
    act = kidpal_story_delete_idle_step(&idle, 3000UL);
    TEST_ASSERT_TRUE_MESSAGE(act == 1 && idle >= KSPV2_DELETE_IDLE_MS,
        kspv2_fm("SPV2-A4 RED: idle 累计 11000ms 返回 %d idle=%lu, 期望 1 且 >= 10000"
                 "——10s 无操作静默取消未触发", act, idle));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_delete_idle_step(NULL, 1UL) == -1,
        "SPV2-A4 RED: NULL idle 未按 -1 防御");

    /* tooth 5 (guard): 决策点 3 数值冻结——idle 死线 = 10s 单点。 */
    TEST_ASSERT_TRUE_MESSAGE(KSPV2_DELETE_IDLE_MS == 10000UL,
        "SPV2-A4 RED(infra): 删除确认静默取消时长偏离 10s 冻结值（requirements "
        "「10 秒无操作静默取消」逐字）");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — SPV2-A5: 隐式 ack 重发决策 + story_list 重推对账冲销（behavioral,
 * seam [K4]/[K5]; 契约 §四 Q5）。RED today: 哨兵 -9999。
 * ------------------------------------------------------------------------ */
/* coverage: 契约 §四 kidpal_story_delete Q5 -- 隐式 ack: 5s 超时重发/上限 3 次放弃 + 重推到达对账冲销（幂等承接归 front） */
/* coverage: SPV2-A5 -- 隐式 ack 重发决策（5s/上限 3 冻结值）+ story_list 重推对账冲销 + NULL 防御 */
TEST_CASE("story_page_v2: 隐式 ack 重发+重推对账冲销 (SPV2-A5)",
          "[story_page_v2][SPV2-A5][behavioral]")
{
    kspv2_story_list_t l;
    int r;

    /* tooth 0 (guard): 决策点 3 数值冻结——重发间隔 5000ms、上限 3 次。 */
    TEST_ASSERT_TRUE_MESSAGE(KSPV2_DELETE_RESEND_MS == 5000UL &&
                             KSPV2_DELETE_RESEND_MAX == 3,
        "SPV2-A5 RED(infra): 重发决策参数偏离冻结值（5000ms / 上限 3, 与契约 §三 "
        "网关提醒重推「上限 3 次」同档）");

    /* tooth 1: 未到期（age < 5000）不重发; 到点（>= 5000）重发。 */
    r = kidpal_story_delete_resend_due(0, 4999u);
    kspv2_require_impl(r, "kidpal_story_delete_resend_due");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        kspv2_fm("SPV2-A5 RED: age=4999ms 即判重发（%d）——间隔短于冻结 5000ms", r));
    r = kidpal_story_delete_resend_due(0, 5000u);
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        kspv2_fm("SPV2-A5 RED: age=5000ms 未判重发（%d）——到点含端语义漂移", r));

    /* tooth 2: 上限——tries 达 3 后恒不重发（放弃 = 静默停发, 不新增请求-应答消息对）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_delete_resend_due(2, 5000u) == 1,
        "SPV2-A5 RED: tries=2 到点未判重发——重发上限提前收敛");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_delete_resend_due(3, 5000u) == 0 &&
                             kidpal_story_delete_resend_due(3, 999999u) == 0,
        "SPV2-A5 RED: tries=3 仍判重发——超过冻结上限 3 次, 重发无界");

    /* tooth 3: 对账冲销——重推到达、pending id 不在 items 窗口 -> 0（调用方冲销并
     * 以 total 刷新计数）; id 仍在窗口 -> 1（不冲销）。 */
    kspv2_make_list(&l, 3); /* items = a/b/c */
    r = kidpal_story_delete_reconcile_hit("2026-08-25-b", &l);
    kspv2_require_impl(r, "kidpal_story_delete_reconcile_hit");
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        kspv2_fm("SPV2-A5 RED: 窗口内 id 判 %d, 期望 1（仍存在, 不冲销）", r));
    r = kidpal_story_delete_reconcile_hit("2026-09-01-z", &l);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        kspv2_fm("SPV2-A5 RED: 窗口外 id 判 %d, 期望 0——重推对账冲销缺失, 待删账目"
                 "永不收敛（隐式 ack 死循环风险）", r));
    r = kidpal_story_delete_reconcile_hit("", &l);
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "SPV2-A5 RED: 空 pending 判非 0——无待账不应冲销任何东西");

    /* tooth 4 (defensive): NULL -> -1。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_delete_reconcile_hit(NULL, &l) == -1 &&
                             kidpal_story_delete_reconcile_hit("x", NULL) == -1,
        "SPV2-A5 RED: NULL 入参未按 -1 防御");
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — SPV2-A6: 阅读态 v1.47 决策表 + 排版/截断回收（behavioral, seam [K1]）
 * KEY 短按不响应（朗读已砍, tts_requests 零递增）/ KEY 长按退出回列表 / BOOT 短按
 * （双击同短按）翻屏 wrap 永不离页 / BOOT 长按 NONE（页循环 P3 断开）/
 * reader_slice >12 屏可达（旧 12 屏截断回收, notes 边角④与 ⑧）。
 * RED today: 哨兵 / 旧实现（KEY 短按 = TTS_READ、末屏钳制不回绕、BOOT 长按翻屏）。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.3-AC1 -- 阅读态分屏（open_reader 全文分屏 + 屏数指示逐字 + >12 屏截断回收） */
/* coverage: S-F2.3-AC2 -- 阅读态手势（KEY 短按不响应/翻屏 wrap 永不离页/KEY 长按退出回列表/BOOT 长按不离页） */
/* coverage: SPV2-A6 -- 阅读态 v1.47 决策表 + 排版/截断回收（open_reader 全文分屏/屏数指示逐字/>12 屏可达） */
TEST_CASE("story_page_v2: 阅读态决策表+截断回收 (SPV2-A6)",
          "[story_page_v2][SPV2-A6][behavioral]")
{
    kspv2_story_view_t v;
    kspv2_story_list_t l;
    kspv2_story_text_t t;
    int act, i, slen;

    kspv2_make_list(&l, 3);
    kspv2_require_impl(kidpal_story_view_init(&v, &l), "kidpal_story_view_init");

    /* fixture: 正文 24 行 -> open_reader(lps=4) = 6 屏。 */
    kspv2_require_impl(kidpal_story_text_parse(
                           "{\"id\":\"2026-08-27-c\",\"title\":\"我的大冒险\","
                           "\"text\":\"第一行\\n第二行\\n第三行\\n第四行\\n第五行\\n第六行\\n"
                           "第七行\\n第八行\\n第九行\\n第十行\\n十一行\\n十二行\\n十三行\\n"
                           "十四行\\n十五行\\n十六行\\n十七行\\n十八行\\n十九行\\n二十行\\n"
                           "廿一行\\n廿二行\\n廿三行\\n廿四行\"}",
                           &t),
                       "kidpal_story_text_parse");
    kspv2_require_impl(kidpal_story_open_reader(&v, &t, 4), "kidpal_story_open_reader");
    TEST_ASSERT_TRUE_MESSAGE(v.page == KSPV2_PAGE_READER && v.screens == 6 && v.screen == 0,
        kspv2_fm("SPV2-A6 RED(infra): open_reader 语义漂移 page=%d screens=%d, 期望 "
                 "READER/6——fixture 前提失败", v.page, v.screens));

    /* tooth 1: KEY 短按不响应（朗读已砍 v1.52）——动作码 NONE(0)、tts_requests 零
     * 递增、屏不动（旧实现 = TTS_READ(4)+tts_requests++ 双红）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    kspv2_require_impl(act, "kidpal_story_page_input READER 半");
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_NONE && v.tts_requests == 0 && v.screen == 0,
        kspv2_fm("SPV2-A6 RED: 阅读态 KEY 短按动作码 = %d tts=%d screen=%d, 期望 "
                 "NONE(=%d)/0/0——「When 按 KEY 短按, Then 不响应（朗读已砍）」未落, "
                 "旧 TTS_READ 语义残留（S-F2.3 AC2）", act, v.tts_requests, v.screen,
                 KSPV2_ACT_NONE));

    /* tooth 2: BOOT 短按翻下一屏（screen 0 -> 1, 动作码 PAGE_TURN(8)）。 */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_PAGE_TURN && v.screen == 1,
        kspv2_fm("SPV2-A6 RED: 阅读态 BOOT 短按动作码 = %d screen=%d, 期望 "
                 "PAGE_TURN(=%d)/1——「翻下一屏」短按化未落（v1.34, 旧 BOOT 长按翻屏"
                 "口径残留）", act, v.screen, KSPV2_ACT_PAGE_TURN));

    /* tooth 3: 末屏再翻 wrap 回首屏、永不离页（旧实现 = 末屏钳制 NONE）。 */
    v.screen = 5; /* 末屏（6 屏 0-based） */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_PAGE_TURN && v.screen == 0 &&
                             v.page == KSPV2_PAGE_READER,
        kspv2_fm("SPV2-A6 RED: 末屏再翻动作码 = %d screen=%d page=%d, 期望 "
                 "PAGE_TURN/0/READER——「末屏再翻 wrap 回首屏、永不离页」缺失",
                 act, v.screen, v.page));

    /* tooth 4: BOOT 双击同短按——tooth 3 已断言末屏 wrap 后 v.screen == 0, 从 0
     * 双击按 [K1] 头注「BOOT 双=PAGE_TURN」+ requirements S-F2.3 AC2「BOOT 双击同
     * 短按」= 与短按同义翻一屏, 只能到 1（修订 2026-09-29: 旧冻结期望 2 是
     * off-by-one, 与语义权威直接矛盾——纠 requirements 矛盾非 weaken: 翻一屏语义
     * 仍由 act==PAGE_TURN && screen==1 全等承载, 无任何断言被移除）。 */
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_PAGE_TURN && v.screen == 1,
        kspv2_fm("SPV2-A6 RED: 阅读态 BOOT 双击动作码 = %d screen=%d, 期望同短按 "
                 "PAGE_TURN(=%d)/1（wrap 后从 0 翻一屏）", act, v.screen,
                 KSPV2_ACT_PAGE_TURN));

    /* tooth 5: KEY 长按 = READER_EXIT(9) 退出阅读回列表（v1.36 返回上一层实例）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_READER_EXIT && v.page == KSPV2_PAGE_LIST,
        kspv2_fm("SPV2-A6 RED: 阅读态 KEY 长按动作码 = %d page=%d, 期望 "
                 "READER_EXIT(=%d)/LIST——「KEY 长按退出阅读回列表」未落",
                 act, v.page, KSPV2_ACT_READER_EXIT));

    /* tooth 6: BOOT 长按 = NONE 不离页（页循环在 P3 断开; 旧实现 = 多屏翻屏 GLOBAL）。 */
    kidpal_story_open_reader(&v, &t, 4);
    act = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_NONE && v.page == KSPV2_PAGE_READER,
        kspv2_fm("SPV2-A6 RED: 阅读态 BOOT 长按动作码 = %d page=%d, 期望 NONE 且留"
                 "阅读态——「页循环在 P3 断开（BOOT 全子态不离页）」被破坏", act, v.page));

    /* tooth 7: KEY 双击 = NONE（矩阵阅读行「—」格, 语义面外手势消费不动）。 */
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_NONE,
        kspv2_fm("SPV2-A6 RED: 阅读态 KEY 双击动作码 = %d, 期望 NONE", act));

    /* tooth 8: 阅读态屏数指示逐字（复用阅读态既有单源）。 */
    {
        char buf[64];
        v.screen = 0; v.screens = 3;
        act = kidpal_story_screen_label(&v, buf, sizeof(buf));
        kspv2_require_impl(act, "kidpal_story_screen_label");
        TEST_ASSERT_TRUE_MESSAGE(act == 0 && strcmp(buf, "第 1 屏 · 共 3 屏") == 0,
            kspv2_fm("SPV2-A6 RED: 阅读态屏数指示 = 「%s」, 期望逐字「第 1 屏 · 共 3 屏」",
                     buf));
    }

    /* tooth 9: >12 屏截断回收（notes ⑧——真实分栏、长文全篇可达; 旧渲染 12 屏
     * 显示位上限退役）。100 行正文 lps=4 -> 25 屏; 第 15 屏切片必须可达非 NULL。 */
    {
        char *big = (char *)malloc(100 * 16 + 1);
        kspv2_story_text_t bt;
        const char *slice;
        int lines;

        assert(big != NULL);
        big[0] = '\0';
        for (i = 0; i < 100; i++) {
            snprintf(big + strlen(big), 16, "L%02d\n", i);
        }
        memset(&bt, 0, sizeof(bt));
        snprintf(bt.id, sizeof(bt.id), "big");
        snprintf(bt.title, sizeof(bt.title), "长文");
        /* text 2048B 上限: 100 行 x ~5B = ~500B, 放得下; 去掉尾换行。 */
        big[strlen(big) - 1] = '\0';
        snprintf(bt.text, sizeof(bt.text), "%s", big);
        free(big);
        lines = kidpal_story_open_reader(&v, &bt, 4);
        kspv2_require_impl(lines, "kidpal_story_open_reader 长文半");
        TEST_ASSERT_TRUE_MESSAGE(v.screens == 25,
            kspv2_fm("SPV2-A6 RED: 长文分屏数 = %d, 期望 25（100 行 / 4 行屏）——"
                     "分屏数被截断回收前的旧上限截留（notes ⑧）", v.screens));
        slice = kidpal_story_reader_slice(&bt, 15, 4, &slen);
        TEST_ASSERT_TRUE_MESSAGE(slice != NULL && slen > 0,
            "SPV2-A6 RED: 第 15 屏（>12）切片 NULL——旧 12 屏截断仍在, 长文后半篇"
            "不可达（真实分栏、长文全篇可达未落）");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_story_reader_slice(&bt, 25, 4, &slen) == NULL,
            "SPV2-A6 RED(guard): 越界屏（>=screens）未按 NULL 拒收");
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — SPV2-A7: 计数双口径 guard + RECORDING/STAMP 决策表（behavioral）
 * counter 恒信 total（>16 篇窗口条数 != 计数, load-bearing）/ total 缺省回退 /
 * 16 窗口超限降级（契约 §1.3③; 均为在世行为零回归守卫, 冻结期即绿）; 录音态
 * KEY 长按同短按保存（防误触毁稿 v1.47）/ BOOT 族丢弃 / 盖章帧零语义守卫。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC1 -- 星星计数 = 现存篇数恒信 total（items.length 不顶替, 契约 §1.3③ load-bearing） */
/* coverage: S-F2.4-AC2 -- 计数口径守卫（现存篇数 = 现存篇数恒信 total）+ 录音态/盖章帧矩阵格 */
/* coverage: SPV2-A7 -- 计数双口径 guard（counter 恒信 total/total 缺省回退/16 窗口超限降级）+ 录音态/盖章帧矩阵 */
TEST_CASE("story_page_v2: 计数双口径 guard+录音态/盖章帧决策表 (SPV2-A7)",
          "[story_page_v2][SPV2-A7][behavioral]")
{
    kspv2_story_list_t l;
    kspv2_story_view_t v;
    char json[128];
    int act, n;

    /* ---- counter 恒信 total（guard: 在世行为, 零回归锚） ---- */
    memset(&l, 0, sizeof(l));
    l.total = 23;   /* >16: 窗口条数 16 != 计数 23 */
    l.count = 16;
    n = kidpal_story_counter(&l);
    kspv2_require_impl(n, "kidpal_story_counter");
    TEST_ASSERT_TRUE_MESSAGE(n == 23,
        kspv2_fm("SPV2-A7 RED: total=23/count=16 时计数 = %d——items.length 顶替了 "
                 "total（契约 §1.3③ load-bearing: >16 篇时窗口条数 != 星星计数）", n));

    l.total = KSPV2_TOTAL_ABSENT;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_counter(&l) == 16,
        "SPV2-A7 RED: total 缺省未回退 items.length——向后兼容防御缺失");

    /* ---- story_list 解析守卫（guard: 在世降级纪律零回归锚） ---- */
    snprintf(json, sizeof(json), "{\"total\":2,\"items\":[{\"id\":\"a\",\"title\":\"甲\",\"date\":\"08-20\"}]}");
    n = kidpal_story_list_parse(json, &l);
    kspv2_require_impl(n, "kidpal_story_list_parse");
    TEST_ASSERT_TRUE_MESSAGE(n == 1 && l.total == 2,
        kspv2_fm("SPV2-A7 RED: 合法载荷解析 = %d total=%d——解析底座回归", n, l.total));
    {
        kspv2_story_list_t ov;
        memset(&ov, 0, sizeof(ov));
        {
            char big[64 * KSPV2_LIST_MAX + 64];
            int k;
            big[0] = '\0';
            snprintf(big, sizeof(big), "{\"total\":20,\"items\":[");
            for (k = 0; k < 17; k++) {
                snprintf(big + strlen(big), sizeof(big) - strlen(big),
                         "%s{\"id\":\"i%d\",\"title\":\"t%d\"}", k ? "," : "", k, k);
            }
            snprintf(big + strlen(big), sizeof(big) - strlen(big), "]}");
            n = kidpal_story_list_parse(big, &ov);
            TEST_ASSERT_TRUE_MESSAGE(n == -1,
                kspv2_fm("SPV2-A7 RED: 17 条 items 解析 = %d, 期望 -1 整单降级——"
                         ">16 窗口超限降级防御缺失（契约 §1.3③ 解析超限整单降级）", n));
        }
    }

    /* ---- 录音态矩阵格（v1.47 新键位族, 全部设计态红） ---- */
    kspv2_make_list(&l, 3);
    kspv2_require_impl(kidpal_story_view_init(&v, &l), "kidpal_story_view_init");
    v.page = KSPV2_PAGE_RECORDING;
    v.rec_seconds = 47;

    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK);
    kspv2_require_impl(act, "kidpal_story_page_input RECORDING 半");
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_STOP_RECORD && v.page == KSPV2_PAGE_LIST,
        kspv2_fm("SPV2-A7 RED: 录音态 KEY 短按动作码 = %d page=%d, 期望 STOP_RECORD"
                 "(=%d) 且回列表", act, v.page, KSPV2_ACT_STOP_RECORD));

    v.page = KSPV2_PAGE_RECORDING;
    act = kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_STOP_RECORD && v.page == KSPV2_PAGE_LIST,
        kspv2_fm("SPV2-A7 RED: 录音态 KEY 长按动作码 = %d, 期望同短按 STOP_RECORD(=%d)"
                 "——「KEY 长按同短按（保存）——防长按误触毁稿」未落（旧实现 = 丢弃, "
                 "毁稿方向反了, v1.47 矩阵）", act, KSPV2_ACT_STOP_RECORD));

    {
        int g;
        static const key_gesture_t gestures[] = {
            KEY_GESTURE_CLICK, KEY_GESTURE_DOUBLE_CLICK, KEY_GESTURE_LONG_PRESS
        };
        for (g = 0; g < (int)(sizeof(gestures) / sizeof(gestures[0])); g++) {
            kidpal_story_view_init(&v, &l);
            v.page = KSPV2_PAGE_RECORDING;
            act = kidpal_story_page_input(&v, KEY_ID_BOOT, gestures[g]);
            TEST_ASSERT_TRUE_MESSAGE(act == KSPV2_ACT_DISCARD_RECORD && v.page == KSPV2_PAGE_LIST,
                kspv2_fm("SPV2-A7 RED: 录音态 BOOT 手势(g=%d)动作码 = %d page=%d, 期望 "
                         "DISCARD_RECORD(=%d) 且回列表——「BOOT = 丢弃」三手势同义未落 "
                         "（旧实现 = GLOBAL 交全局翻页, 页循环 P3 断开被破坏）",
                         (int)gestures[g], act, v.page, KSPV2_ACT_DISCARD_RECORD));
        }
    }

    /* ---- 盖章帧守卫: 页内零语义全手势 GLOBAL（在世行为零回归锚） ---- */
    kidpal_story_view_init(&v, &l);
    v.page = KSPV2_PAGE_STAMP;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK) == KSPV2_ACT_GLOBAL,
        "SPV2-A7 RED(guard): 盖章帧页内语义漂移——演出帧应零语义交全局");
}

/* -------------------------------------------------------------------------- *
 * TEST 8 — SPV2-A8: 盖章 N = 入册流水号 seq 消费（behavioral, seam [K6]/[K7];
 * S-F2.4 AC2 v1.47 拆分口径 + 契约 §1.3④ + 跨包承接 K1 seq 账本）
 * seq 探针三值决策表 / on_saved 三参改形 / stamp_serial / 同 id 幂等 / counter 零
 * 本地推 / 文案逐字二值决策表 / stamp_index 退役（行为面观察）。
 * RED today: 哨兵 / 旧 on_saved 两参签名（3 参哨兵无强符号覆盖）。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.4-AC1 -- 盖章文案逐字「星星徽章·第 N 颗已入册！」+ seq 缺省降级「星星徽章已入册！」二值决策表（契约 Q4 板端自定, 决策点 3） */
/* coverage: S-F2.4-AC2 -- N = 入册流水号只增不减（seq 唯一权威源, 契约 §1.3④）+ counter 零本地推（现存篇数恒信 total）+ 同 id 幂等 */
/* coverage: SPV2-A8 -- 盖章 N = 入册流水号 seq 消费（seq 探针三值/on_saved 三参改形/文案二值决策表/同 id 幂等） */
TEST_CASE("story_page_v2: 盖章 seq 消费+文案逐字二值决策表 (SPV2-A8)",
          "[story_page_v2][SPV2-A8][behavioral]")
{
    kspv2_story_view_t v;
    kspv2_story_list_t l;
    char buf[96];
    int r;

    /* ---- tooth 1: seq 探针三值决策表（契约 §1.3④ additive 缺省降级口径） ---- */
    r = kidpal_story_text_seq("{\"id\":\"x\",\"title\":\"t\",\"text\":\"b\",\"seq\":6}");
    kspv2_require_impl(r, "kidpal_story_text_seq");
    TEST_ASSERT_TRUE_MESSAGE(r == 6,
        kspv2_fm("SPV2-A8 RED: seq=6 探针 = %d, 期望 6——data.seq 消费缝缺失（盖章 N "
                 "唯一权威源, 契约 §1.3④）", r));
    r = kidpal_story_text_seq("{\"id\":\"x\",\"title\":\"t\",\"text\":\"b\"}");
    TEST_ASSERT_TRUE_MESSAGE(r == KSPV2_SEQ_ABSENT,
        kspv2_fm("SPV2-A8 RED: seq 缺省探针 = %d, 期望 ABSENT(=%d)——缺省降级口径缺失",
                 r, KSPV2_SEQ_ABSENT));
    r = kidpal_story_text_seq("{\"id\":\"x\",\"title\":\"t\",\"text\":\"b\",\"seq\":\"6\"}");
    TEST_ASSERT_TRUE_MESSAGE(r == KSPV2_SEQ_ABSENT,
        kspv2_fm("SPV2-A8 RED: seq 畸形（字符串）探针 = %d, 期望 ABSENT 同径降级——"
                 "seq 是 additive 装饰字段, 不为它整包拒（正文照常入阅读态）", r));

    /* ---- tooth 2: on_saved 三参改形——N = seq、counter 零本地推、同 id 幂等 ---- */
    kspv2_make_list(&l, 5); /* 现存篇数 5（counter 初值 = total 权威） */
    kspv2_require_impl(kidpal_story_view_init(&v, &l), "kidpal_story_view_init");
    r = kidpal_story_on_saved(&v, "2026-08-28-d", 6);
    kspv2_require_impl(r, "kidpal_story_on_saved 三参改形");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && kidpal_story_stamp_serial(&v) == 6,
        kspv2_fm("SPV2-A8 RED: on_saved(seq=6) 后 stamp_serial = %d, 期望 6——盖章 N "
                 "未取 seq（旧「counter+1 本地推」D3 语义残留, 与现存篇数口径混源）",
                 kidpal_story_stamp_serial(&v)));
    TEST_ASSERT_TRUE_MESSAGE(v.counter == 5,
        kspv2_fm("SPV2-A8 RED: on_saved 后 counter = %d, 期望 5 零触碰——板端不得本地"
                 "推计数（现存篇数权威恒 = story_list.total, 由重推刷新; 「第 N 颗已入册」"
                 "是历史成就、不被删除改写）", v.counter));

    /* 同 id 幂等（mcp 回执重试不重复演出）。 */
    r = kidpal_story_on_saved(&v, "2026-08-28-d", 6);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && kidpal_story_stamp_serial(&v) == 6,
        "SPV2-A8 RED: 同 saved_id 重复 ack 非幂等——盖章演出会重复触发");

    /* 下一篇: seq 只增不减（7 > 6）。 */
    r = kidpal_story_on_saved(&v, "2026-08-29-e", 7);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && kidpal_story_stamp_serial(&v) == 7,
        kspv2_fm("SPV2-A8 RED: 第二篇 on_saved(seq=7) 后 stamp_serial = %d, 期望 7——"
                 "入册流水号未随新 seq 前进", kidpal_story_stamp_serial(&v)));

    /* seq 缺省: 降级演出（serial = ABSENT, 文案不带 N）。 */
    kidpal_story_view_init(&v, &l);
    r = kidpal_story_on_saved(&v, "2026-08-30-f", KSPV2_SEQ_ABSENT);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && kidpal_story_stamp_serial(&v) == KSPV2_SEQ_ABSENT,
        kspv2_fm("SPV2-A8 RED: seq 缺省 on_saved 后 stamp_serial = %d, 期望 ABSENT——"
                 "降级演出形态缺失（契约 Q4: seq 缺省时降级演出板端自定）",
                 kidpal_story_stamp_serial(&v)));

    /* ---- tooth 3: 文案逐字二值决策表（S-F2.4 AC1 逐字 + Q4 降级形态） ---- */
    r = kidpal_story_stamp_text(6, buf, sizeof(buf));
    kspv2_require_impl(r, "kidpal_story_stamp_text");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(buf, "星星徽章·第 6 颗已入册！") == 0,
        kspv2_fm("SPV2-A8 RED: 盖章文案 = 「%s」, 期望逐字「星星徽章·第 6 颗已入册！」"
                 "——旧文案「故事徽章」残留或措辞漂移（S-F2.4 AC1 逐字 + F2 intro 对账）",
                 buf));
    r = kidpal_story_stamp_text(KSPV2_SEQ_ABSENT, buf, sizeof(buf));
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(buf, "星星徽章已入册！") == 0,
        kspv2_fm("SPV2-A8 RED: seq 缺省降级文案 = 「%s」, 期望逐字「星星徽章已入册！」"
                 "（不带 N——契约 Q4 板端自定, 决策点 3 冻结值）", buf));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_stamp_text(6, buf, 8) == -1,
        "SPV2-A8 RED: cap 不足未按 -1 拒绝（绝不截断/溢出）");
}

/* -------------------------------------------------------------------------- *
 * TEST 9 — SPV2-A9: 层值接线（behavioral + 值断言, seam [K10]; 1g 裁读 = M2-K3
 * 先例符号级插入 + 上移）。
 * RED today: 在世 kidpal_layer.h REMINDER=6 —— 与 STORY_DELETE fallback 同值,
 * 值断言（REMINDER==7）必红; pop_transients 满栈出 2 层断言同红。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC4 -- 删除确认覆盖层层值接线（STORY_DELETE=6 插入 + REMINDER/PROVISION 上移 + 提醒最高层序不变） */
/* coverage: 层栈 transient 集纳入 STORY_DELETE（pop_transients 出层; BOOT 隔离/清层语义自动成立） */
/* coverage: SPV2-A9 -- 层值接线（STORY_DELETE=6 插入 + REMINDER/PROVISION 上移 + 序不变 + pop_transients 行为） */
TEST_CASE("story_page_v2: 层值接线——STORY_DELETE 插入+上移+序不变 (SPV2-A9)",
          "[story_page_v2][SPV2-A9][behavioral]")
{
    kidpal_layer_stack_t st;
    int popped;

    /* tooth 1 (值断言): STORY_DELETE=6 / REMINDER 上移 7 / PROVISION 上移 8。
     * RED today: REMINDER 在世值 6 == fallback STORY_DELETE -> 立即红（真实缺失
     * 行为 = 层值接线未落）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        KIDPAL_LAYER_STORY_DELETE == 6 && KIDPAL_LAYER_REMINDER == 7 &&
        KIDPAL_LAYER_PROVISION == 8,
        kspv2_fm("SPV2-A9 RED: 层值接线未落 STORY_DELETE=%d REMINDER=%d PROVISION=%d, "
                 "期望 6/7/8——1g 裁读「符号级插入 + 上移」未执行（「append-only 低于 "
                 "REMINDER」双约束唯一解 = 插入 6 + 上移 7/8）",
                 KIDPAL_LAYER_STORY_DELETE, KIDPAL_LAYER_REMINDER, KIDPAL_LAYER_PROVISION));

    /* tooth 2 (序不变): 提醒最高层压删除弹窗——STORY_DELETE < REMINDER（矩阵口径:
     * 提醒弹到点必须压过删除确认覆盖层）。 */
    TEST_ASSERT_TRUE_MESSAGE(KIDPAL_LAYER_STORY_DELETE < KIDPAL_LAYER_REMINDER,
        "SPV2-A9 RED: STORY_DELETE 层值不高于 REMINDER 层值——「提醒最高层压删除弹窗」"
        "序不变约束被破坏（§2.2 覆盖层优先级）");

    /* tooth 3 (行为): 纯基层 + STORY_DELETE -> pop_transients 出 1 层回基层。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0,
        "SPV2-A9 RED(infra): kidpal_layer_push 底座回归——fixture 无法建栈");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_STORY_DELETE) == 0,
        "SPV2-A9 RED(infra): push STORY_DELETE 失败");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped >= 1 && kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        kspv2_fm("SPV2-A9 RED: 含 STORY_DELETE 栈清层出 %d 层 top=%d——删除弹窗未入 "
                 "pop_transients transient 集（取消回列表的关层路径缺失）",
                 popped, kidpal_layer_top(&st)));

    /* tooth 4 (行为, 双红判别): [LOCAL, STORY_DELETE, REMINDER] -> 出 2 层。
     * RED today: STORY_DELETE fallback 与 REMINDER 同值 6, 语义重复 -> 出 1 层红;
     * 落地后两值分离 -> 出 2 层绿。 */
    memset(&st, 0, sizeof(st));
    kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE);
    kidpal_layer_push(&st, KIDPAL_LAYER_STORY_DELETE);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0,
        "SPV2-A9 RED(infra): push REMINDER over STORY_DELETE 失败——层栈重复值拒绝");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 2 && kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        kspv2_fm("SPV2-A9 RED: [LOCAL+STORY_DELETE+REMINDER] 清层出 %d 层, 期望 2"
                 "（STORY_DELETE 6 与 REMINDER 7 值未分离 = 插入未落）", popped));
}

/* -------------------------------------------------------------------------- *
 * TEST 10 — SPV2-B1: CJK 非空白真帧探针（behavioral + 真帧, seam [K9];
 * p1_cjk_name 先例形态）。缺陷 = 现文 story_page.c 全文件不设字体 -> LVGL 默认
 * montserrat_14 纯拉丁 -> P3 全部中文面真机空白。
 * RED today: render 哨兵（story_page.c 未编入 runner）-> 「frozen contract not
 * implemented」红; 接线后若仍不接 font_puhui_14_1, 探针以「CJK 零墨」红。
 * 判定带 = 线框语义粗带（版式坐标不钉, 像素级呈现归真机轮 + design review）。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC1 -- 列表行标题 CJK 非空白真帧（星星图标+标题+日期 中的标题墨） */
/* coverage: S-F2.4-AC1 -- 盖章文案 CJK 非空白真帧（「星星徽章·第 N 颗已入册！」墨在带内） */
/* coverage: SPV2-B1 -- CJK 非空白真帧探针（LIST 标题带 + STAMP 文案带, host LVGL 软渲染 seam） */
TEST_CASE("story_page_v2: CJK 非空白真帧探针——列表标题带+盖章文案带 (SPV2-B1)",
          "[story_page_v2][SPV2-B1][pixel-probe]")
{
    kspv2_story_view_t v;
    kspv2_story_list_t l;
    kspv2_story_text_t t;
    int x, y, ink, rc;

    kspv2_make_list(&l, 3);
    memset(&t, 0, sizeof(t));
    snprintf(t.id, sizeof(t.id), "2026-08-27-c");
    snprintf(t.title, sizeof(t.title), "我的大冒险");
    snprintf(t.text, sizeof(t.text), "第一行\n第二行");
    kspv2_require_impl(kidpal_story_view_init(&v, &l), "kidpal_story_view_init");

    /* ---- 探针 1: LIST 态——CJK 标题行带（标题「小兔子找朋友」/「勇敢的小明」）。
     * 判定带 x[30..300] y[40..220]（行流区, 避开右上 ×N ASCII 计数位）; font 接线
     * 前默认字体无 CJK 字形 = 零墨红。 */
    /* seam 起手一次（修订 2026-09-29 R5: **先于被测渲染**——seam 的 render_frame
     * 每次新建整屏不透明 0x336699 fill 对象再刷新, LVGL 后建对象叠顶, render 之后
     * 调会整帧盖掉被测内容 = 墨点恒 0（对任何实现不可满足）; 时序对齐 schedule_grid
     * ksg_render_snap 先例: render_frame 起手一次 + render 后 kspv2_refr_now 直刷 +
     * 探针）。 */
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "SPV2-B1 RED(infra): host_infra_render_frame 返回值非 0——host 渲染 seam 未接"
        "（在世 seam host_infra_seam.c 成功契约: 返回 0 == 出帧, schedule_grid / "
        "包 0 smoke 同口径; 修订 2026-09-29 对齐在世契约）");
    v.page = KSPV2_PAGE_LIST;
    rc = kidpal_story_page_render(&v, &l, NULL);
    kspv2_require_impl(rc, "kidpal_story_page_render LIST 半（story_page.c 编入 runner）");
    kspv2_refr_now();
    ink = 0;
    for (y = 40; y <= 220; y++) {
        for (x = 30; x <= 300; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px >= 0 && (px & 0xFF) < 0x80) {
                ink++;
            }
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(ink >= 30,
        kspv2_fm("SPV2-B1 RED: 列表标题带 CJK 墨点 = %d, 期望 >= 30——P3 中文面空白"
                 "（story_page.c 未显式接线 font_puhui_14_1, 默认 montserrat_14 纯拉丁; "
                 "「不赌继承」= 全 label 显式接线）", ink));

    /* ---- 探针 2: STAMP 态——盖章文案带（「星星徽章·第 N 颗已入册！」中部文案带
     * x[60..340] y[170..230]; 实现落点 (110,190) 14px 字高在带内）。seam 已在探针 1
     * 起手（R5: 每半 render 后直刷, 不再重复调 render_frame——重复调用 = fill 盖内容）。 */
    v.page = KSPV2_PAGE_STAMP;
    kspv2_require_impl(kidpal_story_on_saved(&v, "2026-08-28-d", 6),
                       "kidpal_story_on_saved");
    rc = kidpal_story_page_render(&v, &l, &t);
    kspv2_require_impl(rc, "kidpal_story_page_render STAMP 半");
    kspv2_refr_now();
    ink = 0;
    for (y = 170; y <= 230; y++) {
        for (x = 60; x <= 340; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px >= 0 && (px & 0xFF) < 0x80) {
                ink++;
            }
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(ink >= 30,
        kspv2_fm("SPV2-B1 RED: 盖章文案带 CJK 墨点 = %d, 期望 >= 30——盖章文案空白 "
                 "（同一缺字体缺陷, 全 label 显式接线须覆盖盖章帧）", ink));
}

/* -------------------------------------------------------------------------- *
 * TEST 11 — SPV2-B2: 渲染面结构齿（structural, 真实工作树扫描）
 * 字体显式接线 / 盖章文案逐字双指令 / 页内无标题 / 反白退役 / 选中行 border /
 * 空态单源三处 / 静默取消零倒计时 / 阅读行高单源在世。
 * RED today: 各 needle 零命中或旧残留（负向齿按现状红）。
 * ------------------------------------------------------------------------ */
/* coverage: S-F2.1-AC1 -- 页内无标题负锚 + 选中行带边框不反白（机制级, 强度申报见头注） + 空态入口两处一致单源（边角③） */
/* coverage: S-F2.1-AC3 -- 常驻语音入口双指令「我要讲故事」+「帮我记下来」逐字（intro 对账③） */
/* coverage: S-F2.4-AC1 -- 盖章文案「星星徽章」逐字（旧「故事徽章」退役负锚） */
/* coverage: SPV2-B2 -- 渲染面结构齿（字体接线/双指令逐字/无标题/反白退役/空态单源/静默零倒计时/行高单源） */
TEST_CASE("story_page_v2: 渲染面结构齿——字体/文案/无标题/反白退役/空态单源 (SPV2-B2)",
          "[story_page_v2][SPV2-B2][structural]")
{
    char *region;

    KSPV2_REQUIRE_ROOT();

    /* tooth 1: 全 label 显式接线 font_puhui_14_1, >= 5 处（lcd_display.cc 五处先例;
     * 不赌继承）。RED today: 零命中（CJK 真机空白根因）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_count_in_file("firmware/main/display/story_page.c", "font_puhui_14_1") >= 5,
        kspv2_fm("SPV2-B2 RED: story_page.c font_puhui_14_1 仅 %d 处（期望 >= 5）——"
                 "CJK 字体未显式接线, P3 全部中文面真机空白（默认 montserrat_14 纯拉丁）",
                 kspv2_count_in_file("firmware/main/display/story_page.c", "font_puhui_14_1")));

    /* tooth 2: 盖章文案逐字——「星星徽章」在世、「故事徽章」退役。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/display/story_page.c", "星星徽章"),
        "SPV2-B2 RED: story_page.c 无「星星徽章」——盖章文案未对齐 S-F2.4 AC1 逐字");
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_count_in_file("firmware/main/display/story_page.c", "故事徽章") == 0,
        "SPV2-B2 RED: story_page.c 仍有「故事徽章」——旧文案残留（F2 改名对账未清）");

    /* tooth 3: 常驻语音入口双指令（S-F2.1 AC3 + intro 对账③: 单指令旧口径清扫,
     * 「我要讲故事」+「帮我记下来」双触发词常驻）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/display/story_page.c", "我要讲故事") &&
        kspv2_file_contains("firmware/main/display/story_page.c", "帮我记下来"),
        "SPV2-B2 RED: story_page.c 常驻语音入口无双指令——「我要讲故事」+「帮我记下来」"
        "须双触发词在世（intro 对账③; 旧单指令口径退役）");

    /* tooth 4: 页内无标题负锚（列表 = 星星记录本无页名标题, 计数右上角裸排）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_count_in_file("firmware/main/display/story_page.c", "我的故事") == 0,
        "SPV2-B2 RED: story_page.c 仍有页名「我的故事」——旧页标题未摘除（列表态无"
        "标题口径, v1.27 交互改写）");

    /* tooth 5: 选中反白退役负锚（不反白——反白真机效果差, 2026-09-14 human 裁）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_count_in_file("firmware/main/display/story_page.c", "KSY_COLOR_SEL") == 0,
        kspv2_fm("SPV2-B2 RED: story_page.c 仍有反白选中色 KSY_COLOR_SEL*（%d 处）——"
                 "「选中行带边框（不反白）」未落（S-F2.1 AC1）",
                 kspv2_count_in_file("firmware/main/display/story_page.c", "KSY_COLOR_SEL")));

    /* tooth 6: 选中行 border 机制正锚——LIST 渲染切区内含 border 样式路径。
     * （锚注: 切区锚 = case KSTORY_PAGE_LIST 枚举标签; builder 重排 case 序时
     * 锚漂移须复核本齿。） */
    region = kspv2_region_between("firmware/main/display/story_page.c",
                                  "case KSTORY_PAGE_LIST", "case KSTORY_PAGE_READER");
    TEST_ASSERT_TRUE_MESSAGE(region != NULL,
        "SPV2-B2 RED(infra): LIST 渲染切区切取失败（case 枚举锚漂移——须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(kspv2_count_in_slice(region, "border") >= 1,
        "SPV2-B2 RED: LIST 渲染切区无 border 样式路径——选中行「带边框」机制缺失"
        "（S-F2.1 AC1; 旧反白填充须替换为边框描边）");
    free(region);

    /* tooth 7: 空态文案单源三处（边角③——空态渲染入口在 render_node 非 story_page,
     * 两处同源一致）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/display/story_page.h", "KSTORY_EMPTY_HINT") &&
        kspv2_file_contains("firmware/main/display/story_page.c", "KSTORY_EMPTY_HINT") &&
        kspv2_file_contains("firmware/main/display/kidpal_render_node.cc", "KSTORY_EMPTY_HINT"),
        "SPV2-B2 RED: 空态文案未单源（KSTORY_EMPTY_HINT 须 story_page.h 定义 + "
        "story_page.c / kidpal_render_node.cc 双消费在世）——两处空态入口漂移风险");

    /* tooth 8 (guard): 10s 静默取消零倒计时负锚（矩阵注记「静默取消」）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_count_in_file("firmware/main/display/story_page.c", "倒计时") == 0,
        "SPV2-B2 RED(guard): story_page.c 出现倒计时字样——10s 取消须静默, 不显示"
        "倒计时（删除确认子态口径）");

    /* tooth 9 (guard): 阅读行高分屏单源在世（值 = builder 按 NFR-READ-1 实测重冻,
     * 本文件不锁数值——强度申报见头注）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/display/story_page.h", "KSTORY_READER_LINES_PER_SCREEN"),
        "SPV2-B2 RED(guard): story_page.h 丢失 KSTORY_READER_LINES_PER_SCREEN——"
        "阅读分屏行高单源被移除（值须按实测重冻, 不得删除单源）");

    /* tooth 10: story_page.h 旧 FROZEN OFFLINE CONTRACT 头注退役声明 + 现役权威
     * 指针迁本齿（notes 边角①; M2-K3 kidpal_layer.h「现役权威 = 冻结验收文件」
     * 同款钉法）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/display/story_page.h", "story_page_v2.acceptance.c"),
        "SPV2-B2 RED: story_page.h 头注未声明退役口径/未迁现役权威指针到 "
        "story_page_v2.acceptance.c——旧 offline 契约随 archive 验收退役的声明缺失 "
        "（notes 边角①; 重写头注 = builder 义务, 新冻结面 = 本文件）");
}

/* -------------------------------------------------------------------------- *
 * TEST 12 — SPV2-B3: 删除链路接线结构齿（structural; §四 kidpal_story_delete
 * 端到端板端半）+ stamp_index 全退役负锚。
 * RED today: 全部 needle 零命中 / stamp_index 残留（8 处）。
 * ------------------------------------------------------------------------ */
/* coverage: 契约 §四 kidpal_story_delete -- 发送原语四件套（protocol/application/mcp_server）+ 板级 P3 全手势族分流 + 层值消费三处 */
/* coverage: S-F2.4-AC2 -- 易失 stamp_index 全退役负锚（story_page.{c,h} + mcp_server.cc 零残留） */
/* coverage: SPV2-B3 -- 删除链路接线结构齿（发送原语四件套/板级全手势族分流/mcp 四针消费/层值消费/stamp_index 退役负锚） */
TEST_CASE("story_page_v2: 删除链路接线+stamp_index 退役 (SPV2-B3)",
          "[story_page_v2][SPV2-B3][structural]")
{
    char hit[KSPV2_MAX_PATH];
    int n;

    KSPV2_REQUIRE_ROOT();

    /* tooth 1: SendKidpalStoryDelete 原语 >= 3 文件消费链（protocol.h 声明 +
     * protocol.cc 定义 + mcp_server.cc 确认路径调用）。 */
    n = kspv2_scan_count("firmware/main", "SendKidpalStoryDelete", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kspv2_fm("SPV2-B3 RED: SendKidpalStoryDelete 仅 %d 文件命中（首命中 %s, 期望 "
                 ">= 3: protocol.h/.cc + mcp_server.cc）——§四 删除发送原语无端到端"
                 "消费链（逐字仿 SendAbortSpeaking 形态）", n, hit));
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/protocols/protocol.h", "SendKidpalStoryDelete") &&
        kspv2_file_contains("firmware/main/protocols/protocol.cc", "SendKidpalStoryDelete"),
        "SPV2-B3 RED: protocol.{h,cc} 缺 SendKidpalStoryDelete——发送原语未落单缝"
        "（K3 的 kidpal_session_ctx 发送同缝复用, 一次做对）");

    /* tooth 2: 板级 P3 全手势族分流——HandleStoryKey 接线 >= 5 处（KEY/BOOT 六手势;
     * 现状仅 BOOT 长按一条缝）。 */
    n = kspv2_count_in_file(KSPV2_BOARD_CC, "HandleStoryKey(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 5,
        kspv2_fm("SPV2-B3 RED: 板级 HandleStoryKey 仅 %d 处接线（期望 >= 5）——P3 全"
                 "手势族分流未落, 列表态 300ms 消歧/删除子态/阅读态手势无驱动方 "
                 "（§2.3 对账③④⑥）", n));

    /* tooth 3: mcp 消费侧新缝接线四针（对账冲销 / 重发决策 / idle-step / 消歧门控）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/mcp_server.cc", "kidpal_story_delete_reconcile_hit(") &&
        kspv2_file_contains("firmware/main/mcp_server.cc", "kidpal_story_delete_resend_due(") &&
        kspv2_file_contains("firmware/main/mcp_server.cc", "kidpal_story_delete_idle_step(") &&
        kspv2_file_contains("firmware/main/mcp_server.cc", "kidpal_story_debounce_gate("),
        "SPV2-B3 RED: mcp_server.cc 未消费删除链路/消歧新缝——纯逻辑决策表无 tick "
        "驱动方（story_list 重推到达对账 / 5s 重发 / 10s idle / 300ms 消歧门控）");

    /* tooth 4: 层值消费两处——kidpal_layer.c transient 集 + render_node front 闭集。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_file_contains("firmware/main/display/kidpal_layer.c", "KIDPAL_LAYER_STORY_DELETE") &&
        kspv2_file_contains("firmware/main/display/kidpal_render_node.cc", "KIDPAL_LAYER_STORY_DELETE"),
        "SPV2-B3 RED: KIDPAL_LAYER_STORY_DELETE 未被消费——pop_transients transient "
        "集 / front_is_overlay 闭集未纳入删除弹窗层（无页点与取消关层语义缺失）");

    /* tooth 5: stamp_index 全退役负锚（易失本地流水号退役, N 唯一权威源 = seq）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_count_in_file("firmware/main/display/story_page.c", "stamp_index") == 0 &&
        kspv2_count_in_file("firmware/main/display/story_page.h", "stamp_index") == 0 &&
        kspv2_count_in_file("firmware/main/mcp_server.cc", "stamp_index") == 0,
        kspv2_fm("SPV2-B3 RED: stamp_index 残留 story_page.c=%d story_page.h=%d "
                 "mcp_server.cc=%d——易失盖章流水号须全退役（N = story_text.data.seq "
                 "唯一权威源, 契约 §1.3④; 板端易失计数退役负锚）",
                 kspv2_count_in_file("firmware/main/display/story_page.c", "stamp_index"),
                 kspv2_count_in_file("firmware/main/display/story_page.h", "stamp_index"),
                 kspv2_count_in_file("firmware/main/mcp_server.cc", "stamp_index")));

    /* tooth 6 (guard): 扫描器负向自检——本齿 needle 不可能命中非 test 目录。 */
    n = kspv2_scan_count("firmware/main", "__kspv2_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "SPV2-B3 RED(infra): scan matched an impossible needle — 负向断言会假绿");
}

/* -------------------------------------------------------------------------- *
 * TEST 13 — SPV2-C1: boundary——注册序守卫 + 需求/契约 canary + 扫描器自检。
 * RED until builder wires（本文件 + story_page.c 进 HOST_ACCEPTANCE_SRCS）。
 * ------------------------------------------------------------------------ */
/* coverage: SPV2-C1 -- 本文件与被测实现注册进 HOST_ACCEPTANCE_SRCS（smoke 前置序守卫）+ 需求/契约面 canary + 扫描器自检 */
TEST_CASE("story_page_v2: 注册序守卫+需求契约canary+扫描器自检 (SPV2-C1)",
          "[story_page_v2][SPV2-C1][boundary]")
{
    char *mk;
    const char *mine, *impl, *smoke;

    KSPV2_REQUIRE_ROOT();

    /* tooth 1: 本文件 + 被测实现 story_page.c 注册进 HOST_ACCEPTANCE_SRCS, 且都在
     * host_infra_smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。 */
    mk = kspv2_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk, "SPV2-C1 RED(infra): host_app/main/CMakeLists.txt unreadable");
    mine = strstr(mk, "story_page_v2.acceptance.c");
    impl = strstr(mk, "display/story_page.c");
    smoke = strstr(mk, "host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
        "SPV2-C1 RED: story_page_v2.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS——"
        "host 验收构建未接本包（BUILDER DUTY: 头注 Run protocol 0) 同款接线）");
    TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
        "SPV2-C1 RED: display/story_page.c 未编入 host runner——行为齿强符号无覆盖源"
        "（全部哨兵恒红, BUILDER DUTY）");
    TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
        "SPV2-C1 RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
        "丢失——包 0 守卫被移除");
    TEST_ASSERT_TRUE_MESSAGE(mine < smoke && impl < smoke,
        "SPV2-C1 RED: 本文件/story_page.c 必须排在 host_infra_smoke.acceptance.c 之前"
        "（smoke 末位 = 最先执行, 注册序守卫）");
    free(mk);

    /* tooth 2 (canary): 需求面 F2 在世（S-F2.1 锚 + 「星星记录本」改名锚——本文件
     * 断言全部回引 requirements 现文; 需求变更走 requirement loop, 不在本包改）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_exists("docs/requirements.md") &&
        kspv2_file_contains("docs/requirements.md", "S-F2.1") &&
        kspv2_file_contains("docs/requirements.md", "星星记录本"),
        "SPV2-C1 RED(infra): docs/requirements.md F2 面丢失——story 语义权威被触碰");

    /* tooth 3 (canary): 契约面三触点在世（§1.3③④ + §四——本包零契约触碰红线）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kspv2_exists("docs/contracts/interface-contract.md") &&
        kspv2_file_contains("docs/contracts/interface-contract.md", "story_list") &&
        kspv2_file_contains("docs/contracts/interface-contract.md", "kidpal_story_delete"),
        "SPV2-C1 RED(infra): 契约面 story_list/kidpal_story_delete 触点丢失——"
        "契约面被触碰（消费现文冻结形状, 零改）");

    /* tooth 4: 扫描器自检（防假绿: 正向能见 / 负向能红 / 切区器能切已知函数）。 */
    TEST_ASSERT_TRUE_MESSAGE(kspv2_exists("firmware/main/display/story_page.c"),
        "SPV2-C1 RED(infra): scanner cannot see a known-good file");
    TEST_ASSERT_FALSE_MESSAGE(kspv2_exists("firmware/main/__kspv2_no_such__.c"),
        "SPV2-C1 RED(infra): scanner reports a nonexistent file as present");
    {
        char *region = kspv2_region_between("firmware/main/display/story_page.c",
                                            "int kidpal_story_list_parse(",
                                            "kidpal_story_counter");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "SPV2-C1 RED(infra): region slicer cannot slice a known function——"
            "story_list_parse 函数锚漂移（builder 重写 story_page.c 时须复核本齿锚）");
        TEST_ASSERT_TRUE_MESSAGE(kspv2_count_in_slice(region, "items") >= 1,
            "SPV2-C1 RED(infra): region slicer lost a known literal——切区器失灵");
        free(region);
    }
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author; SPV2 全套设计态红清单）:
 *
 * 通道②独立红 = 本文件经 C 类冻结验收实编（gcc 编译通道 error 零）+ 独立 harness
 * 实跑（unity shim, weak 哨兵全在世态）逐 TEST 产出 RED 名 verbatim（见
 * taskpackage redBaselineResult 回填）。红因归因自查 = 每条设计态红的断言失败消息
 * 与声明缺失行为逐条比对:
 *   SPV2-A1/A4/A6/A7 RECORDING/STAMP 半  <- kidpal_story_page_input 旧 v1.27 口径
 *       （KEY 短按=旧 CURSOR_NEXT、录音 KEY 长按=丢弃、BOOT 族=GLOBAL——与 v1.47
 *       矩阵逐格反向）+ DELETE 子态页值 4 无实现; fixture 自检齿（view_init 语义锚/
 *       counter 权威/parse 降级/盖章帧 GLOBAL 守卫）绿 = 底座在世证据。
 *   SPV2-A2/A3/A5/A8/A9  <- 新缝符号未实现（哨兵 -9999）: 分屏纯逻辑/消歧三缝/
 *       隐式 ack 重发/对账冲销/seq 探针/三参 on_saved/层值接线（A9 红因 = 在世
 *       REMINDER=6 与 STORY_DELETE fallback 同值, 值断言红）。
 *   SPV2-B1  <- story_page.c 未编入 runner（render 哨兵）+ 接线后若无 font_puhui_14_1
 *       则转「CJK 零墨」红（双态红因已声明）。
 *   SPV2-B2/B3/C1  <- 结构 needle 零命中或旧残留（font 零接线/「故事徽章」在世/
 *       单指令/页名/反白/stamp_index 8 处残留/SendKidpalStoryDelete 零命中/
 *       board 单缝 HandleStoryKey/CMakeLists 未注册——guard 齿（counter 权威/
 *       parse 降级/静默零倒计时/行高单源/canary/自检）冻结期绿 = 底座零回归锚。
 * 禁预钉声明: 全文件无「建连即推 story_list」时点断言（决策点 6 硬约束——现役
 * 两时点 = 归档后 + 删除后, 恢复重裁归 K3）。
 *
 * 真机证据要求（本文件不承载, deferred 挂 M4-K3 设备轮, human 在场）:
 *   WF-F2-01/03/04/05 四面 1bpp 目测 + 300ms 消歧手感 + 删除弹窗 10s 静默取消 +
 *   盖章定格 0.8s——版式/手感/观感半; host 半 = 本文件 + device build 闸。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-29  initial freeze (acceptance-author, TaskPackage
 *               260924-m4-k2-p3-star-notebook-ui; 决策点 3 数值冻结: 消歧死线
 *               300ms < 板端双击窗 350ms / 重发 5000ms 上限 3 次 / seq 缺省降级
 *               文案「星星徽章已入册！」; 决策点 6 禁预钉建连时点已守)。
 *   2026-09-29  revision 1 (correlationId 260924-m4-k2-p3-star-notebook-ui/
 *               acceptance-author/2026-09-29; K1 包 §6 tighten-only 窗口同款闭环
 *               ——builder 落地后三红三方复核确认为冻结件自身缺陷、非实现缺陷;
 *               只动三处, 其余断言零触碰零弱化):
 *     R1 SPV2-A4 fixture 双按不可满足——builder 红名 verbatim: 「SPV2-A4 RED(infra):
 *        无法经 BOOT 长按进入删除确认子态（fixture 前提失败）」。旧 fixture 探针
 *        调用 kspv2_require_impl(input(BOOT,LONG),…) 先消费 BOOT 长按进 DELETE 子
 *        态, 断言处二次驱动同一手势按 tooth 2 冻结决策表必返 DELETE_CANCEL(7), 与
 *        期望 DELETE_ENTER(5) 对同一可观测状态互斥 = 任何实现不可满足; 红基线未
 *        暴露因 view_init 哨兵红先行 abort。修订 = 探针移除, 断言处单次驱动承载
 *        「BOOT 长按进删除子态」tooth（进入+确认两 tooth 覆盖不缩水）,
 *        require_impl 哨兵红归因通道保持。
 *     R2 SPV2-A6 tooth 4 off-by-one——builder 红名 verbatim: 「SPV2-A6 RED: 阅读态
 *        BOOT 双击动作码 = 8 screen=1, 期望同短按 PAGE_TURN(=8)/2」。tooth 3 已断言
 *        末屏 wrap 后 v.screen == 0, tooth 4 从 0 双击按 [K1] 头注「BOOT 双=
 *        PAGE_TURN」+ requirements S-F2.3 AC2「BOOT 双击同短按」只能翻一屏到 1,
 *        冻结期望 2 与语义权威直接矛盾。修订 = 期望值 2 -> 1（纠 requirements 矛盾
 *        非 weaken——翻一屏语义仍由 act==PAGE_TURN && screen==1 全等承载, 无断言
 *        移除）。
 *     R3 SPV2-B1 seam 返回值契约矛盾——builder 红名 verbatim: 「SPV2-B1 RED(infra):
 *        host_infra_render_frame 未出帧——host 渲染 seam 未接」。冻结断言
 *        host_infra_render_frame() > 0 与在世 seam host_infra_seam.c 成功契约
 *        （返回 0 == 出帧; schedule_grid / 包 0 smoke / p1 诸齿均钉 == 0）矛盾 =
 *        不可满足。修订 = LIST/STAMP 两处断言 > 0 -> == 0（出帧判据不变, CJK 墨点
 *        探针断言零触碰; mutation 取证表 SPV2-A4/SPV2-B1 条目描述仍有效不动）。
 *     修订后通道②独立红复跑确认: 13 条红仍在世, A4/B1 红因更新为真实设计态义务
 *     红（A4 = 输入函数 DELETE 子态入口哨兵红; B1 = render 哨兵红）, 红条数语义
 *     不变。
 *     R4 SPV2-A4 镜像 undersize（超出来件三处清单——R1 修订验证实跑暴露的第 4 处
 *        冻结件自身缺陷, builder 红名 verbatim: 「SPV2-A4 RED: 取消后条目/计数被动
 *        过——取消是零副作用回列表」; 迭代1 被 fixture 双按缺陷 abort 遮蔽, 首次
 *        可达即红）。根因 = 测试镜像 kspv2_story_view_t 止于 last_saved_id（108B <
 *        真实 172B）, 漏镜像 seam [K5] 自己冻结的 additive 尾部字段
 *        pending_delete_id[64]——真实实现写 pending 记账即越界写破栈上相邻 fixture,
 *        冻结 [K5] 契约对任何实现不可满足（host 实证: confirm 后调用方 list.count
 *        被打爆为垃圾值）。修订 = 镜像补 pending_delete_id[64]（纯 additive fixture
 *        修正, 零断言语义变化）; 按 tighten-only「修自己 fixture 缺陷」条款闭环,
 *        如 planner 裁断超窗可整体 revert 本 hunk。
 *     R5 SPV2-B1 seam 用法时序矛盾（超出来件三处清单——R3 修订验证实跑暴露的第 5
 *        处冻结件自身缺陷, builder 红名 verbatim: 「SPV2-B1 RED: 列表标题带 CJK 墨点
 *        = 0, 期望 >= 30」; 迭代1 被 render_frame 返回值矛盾 abort 遮蔽, 首次可达
 *        即红）。根因 = B1 在被测 render 之后再调 host_infra_render_frame——seam
 *        每次新建整屏不透明 0x336699 fill 对象再刷新, LVGL 后建对象叠顶 = 刷出的
 *        帧被 fill 整帧盖掉, 墨点对任何实现恒 0。修订 = 时序对齐 schedule_grid
 *        ksg_render_snap 先例: render_frame 起手一次（== 0 断言保留, 移到 render 前
 *        作 seam 初始化判据）+ render 后经 kspv2_refr_now() 直刷（lvgl.h 经
 *        __has_include 门控, schedule-p2 先例; 独立红跑通道 render 哨兵先行 abort
 *        不到达直刷, 设计态红语义不变）+ 两处墨点 >= 30 探针逐字零触碰。R3 的
 *        STAMP 半独立 == 0 断言随重排收拢为起手单次（同一 seam 判据, 非弱化——
 *        探针强度与红通道不变）。
 *     R4/R5 修订后 host runner 全量实跑 + 通道②独立红复跑见 revision 1 尾注。
 *   2026-09-30  revision 2 (correlationId 260924-m4-k2-p3-star-notebook-ui/
 *               acceptance-author/2026-09-30; verifier round 1 acceptance-coverage
 *               gate red, fingerprint 34dfaf4bfee6——标记形态对齐, 非齿修订):
 *     R6 coverage 直配标记补钉——acceptance-coverage-check.mjs 匹配正则要求
 *        coverage token 后紧跟短 id, 本文件既有 coverage map 是 AC->SPV2 间接映射
 *        （id 与 token 之间隔 AC 引号与描述文字）正则咬不到 -> 12 条
 *        missingCoverage（SPV2-A1..A9/B1..B3; SPV2-C1 已直配过）。修订 = 12 个
 *        TEST_CASE 头注释各追加一行直配标记 `coverage: SPV2-<id> -- <作用域描述>`
 *        （纯注释追加, 零断言零语义变化; 既有 AC 面 coverage 行原样保留, 对应关系
 *        沿用本头注既有 coverage map, 不新增不缩水任何承载）。
 * --------------------------------------------------------------------------- */







