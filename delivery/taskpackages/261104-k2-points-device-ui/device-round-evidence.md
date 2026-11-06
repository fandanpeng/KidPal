# device-round-evidence · 261104-k2-points-device-ui

> K2 真机轮（K2PTS-L1/L2/L3 + K1L-L1/L2 挂接）。2026-11-06 09:08 烧板（xiaozhi.bin @
> 53944b5b，ota_0@0x20000 保 NVS）+ 09:06 ECS 部署（front_app.py/points.py，state 迁移 +
> env 单源）。三源 = 设备 SD 日志（KPLOG）/ front journalctl / 引擎 docker logs。human 在场
> 操作，planner 取证。发现项 F1-F11 逐条钉了三源证据；修复轮复验项见 §四。

## 一、场景 PASS 台账（三源互证）

| 场景 | 证据锚 | 结论 |
|---|---|---|
| L1.1 完整句记账跳页 | ASR「加三分」(09:12:37) → front points_entry delta=+3 bal=13 rev=4 (09:12:50) → 权威帧 `{route:points_home, balance:13, state:2, progress 13/40}` → 设备 PAGE_SWITCH home→points_home takeover=1 + 看屏（★13/游乐场/进度线/最近三条） | PASS |
| L2.1 删除全链 | 设备 delete_confirm id=pt_23877cbf (09:29:01) → 恰双帧 home(bal=8,rev=5,restore=1)+ledger(total=3,reset=1) + points_delete ack + pending 清；服务端重算链逐条对上（0→3/3→5/5→8）；看屏 before→after 随删刷新 | PASS |
| L2.2 删除取消双路径 | BOOT 短按取消 + 10s 静默自动退出，分录保留；看屏整屏覆盖、页点消失、键帽「删掉/算了」逐字 | PASS（观感 F7：长原因折行溢出圆角框） |
| L2.3 满窗翻页（K1L-L2） | 开账本 `next_request home_only rev=10 limit=6` → total=8 items=6 reset=1 screens=2；续页 2 条 cached=8 has_more=0；页脚「第x屏·共y屏」 | PASS（F8：末屏再翻回绕重拉首页，轻微） |
| K1L-L1 外部写入不抢前台 | CLI 写入 ×5 → points_observe pushed rev=10 entries=8 (09:41:00) 恰一帧 restore；设备在屏不被接管 | PASS |
| L1.2 短窗重复零写入 | 09:49:30/09:49:47 两次 points_dedup hit 零落账 rev 不变；takeover home 帧照推（契约展示面兑现） | PASS（F9：缺语音「刚记过了」反馈） |
| L1.3 不完整句零写入 | 「今天好累呀」正常对话（引擎 TTS 长回复），账本零新增 | PASS |
| 种子数据清理 | 5 条测试填充逐条真删，front 5× delete consumed，账本回归 rev=15/bal=8/3 条 | PASS |

## 二、Findings（修复轮输入）

- **F1（功能）**：引擎补丁（server/deploy/engine-patch/connection.py）认领 kidpal_speak / kidpal_record_context / kidpal_schedule_summary 三类，**缺 kidpal_points_context**——front 注入的语境摘要被引擎「收到未知类型消息」丢弃 (09:12:56/09:53:02 实证)，模型收不到余额/目标/最近分录。修法沿 261022-m5-n2a 先例：补丁认领 + 会话态 + kidpal-agent-prompt.txt 动态占位。
- **F10（功能，真机实锤）**：积分页问一问语音走 `StartListening`→`HandleStartListeningEvent`（Idle 非故事分支恒 ManualStop, application.cc:1093），manual 模式 ASR 等 listen stop → 点按开启永不 stop → 无回显无回复（09:58:21 `listen start mode=manual` 后引擎零 ASR 实证）。对照课表问课程 = `HandleToggleChatEvent` 恒 auto（10:25:35 mode=auto + 识别文本实证）。修法：积分两处 VOICE_SESSION 改调 ToggleChatState（语义 = 对话开关，与问课程同路）；积分语境摘要发送点在主循环聆听处理、与入口无关、改后照发。
- **F11（功能）**：WS 建连积分初推缺失——设备重启后进积分页零请求零推送（10:29:36/10:30:04 实证），RAM 缓存掉电即失 → 停留 K0a「数据未达」占位。landed 架构积分面行明列「WS 建连」恢复推送。修法：front 建连恢复推送点（story_list 建连同点）加恰一帧 points_home restore=true + 置观察基线。
- **F2/F5（观感）**：进度线小伴标记 = 16×16 实心方块（rn_points_box, kidpal_render_node.cc:1003）；积分图标 ★ 应为金币（与故事星星视觉独立）。
- **F3（观感）**：主页「最近得到」行缺 before→after（账本页双栏已有、实证渲染正常）。
- **F4（观感）**：「详细·双击看」胶囊移入 KEY 键帽、文字压缩（human 现场意见）。
- **F6（资产）**：管线白名单 68 枚闭集无金币/小伴/场景标——V9 设计稿三件全有（.coin 24px / pt-cat 34×28 / sc-wheel 42×42），母版重生成 + 白名单 + ui_art_integration 冻结件 artt_expected 扩三枚即可。
- **F7（观感）**：删除确认覆盖层长原因折行溢出圆角框（看屏照片实证）。
- **F8（轻微行为）**：末屏 BOOT 双击回绕重拉首页——回绕 or 停住冻结件未钉，待裁（默认保留回绕）。
- **F9（行为/UX）**：dedup 缺语音反馈——修法 = front dedup 分支注入 kidpal_speak「这条刚记过啦」（ADR-003 既有音频链，引擎补丁已认领该类型）。
- （附记）09:53:17 设备 mbedtls SSL error=76 瞬断一次：front 无 traceback 无重启（pid 未变），未复现于后续，暂记环境噪音；修复轮复验语音时顺带观察。

## 三、环境限制（不计缺陷）

- 兑换正向场景：points.py CLI 无目标写入子命令（K4 归属），本轮仅可测「无激活目标拒答」半；正向归 K4 落地后补测。
- 断网 NFR-OFF-1 场景：顺延修复后复验轮（避免现场反复切网）。

## 四、修复轮复验清单（本文件未完项）

1. F10 复验：积分页点按问一问 → mode=auto + ASR 出文本 + 完整句记账仍通。
2. F11 复验：重启设备 → 进积分页数秒内出现真实余额（restore 填缓存、未达页不滞留）。
3. F9 复验：短窗重复 → 语音「刚记过啦」+ 屏 takeover 照旧。
4. F1 复验：积分页语音问「我现在多少分」→ 模型回答含正确余额（语境摘要进 prompt）。
5. F2/F3/F4/F5/F7 看屏：小伴图形 / 金币图标 / 最近三条 before→after / 键帽提示 / 长原因不溢框。
6. K2D-E4 六 V9 帧 design review 看屏对表。
7. 断网 NFR-OFF-1 + 兑换「无目标拒答」反路径。
