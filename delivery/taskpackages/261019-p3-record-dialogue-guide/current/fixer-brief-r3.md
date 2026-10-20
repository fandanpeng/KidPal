# 真机轮 R3/R4 合并缺陷现场包（261019-p3-record-dialogue-guide，2026-10-20 15:50–17:40）

> fixer 四轮输入。R4 主流程已验证通过（语音收尾整理质量 ✅「游乐场的一天」、抑制 ✅、单篇干净归档 ✅）；以下为残余缺陷，按严重度排序。R3/R4 日志证据：`/c/tmp/kplog-full-r3.log`（767KB 全量）/ `/c/tmp/kplog-r4.log`（804KB）/ front journalctl / 引擎 docker logs（关键行已在下文引用）。

## D4-3（P0）幻听过滤丢弃后设备悬死思考态

- 现场：17:33:02 记录会话干净收尾（listen stop）；17:33:03 ASR 幻听「Thank you.」→ 记录语境已退、按普通问答进入——设备三点亮 + 「不等了」思考态；引擎 `kidpal asr-hallucination filter: 丢弃幻听串: Thank you.` 正确丢弃，**但无任何信号通知设备该句被丢弃**——设备悬思考态等永不来的回复，直到 17:38:50 连接超时才断。human 实测目击：列表页 + 三点 + 「不等了」键帽挂 20+ 分钟。
- 修法方向：过滤器丢弃时向设备发回合收尾信号（abort / end / 静默关 round），让设备退出思考态回 idle；落点 = engine-patch connection.py 幻听过滤分支。注意与 261014 stop-truncation 齿、记录语境守卫的交互。
- 引擎幻听本体（TECH-261014-engine-asr-hallucination-after-stop）已有三连击实害：R3 造垃圾故事、R4 造幽灵思考态；本项修「丢弃不通知」，幻听本体抑制归该 debt 条目后续。

## D4-1（P0）整理过渡页不可见（start_organizing 下发时点错）

- 现场（R3 真故事）：9630615 用户 KEY → 9630985 列表页 → **9631315~9635755 约 4.4 秒设备停在列表页（organize LLM 延迟期）** → 9635755 start_organizing 到达 → 9635815 整理页渲染 → 瞬间被 stamp 覆盖。用户观感「闪列表页→闪徽章页→回列表页」，整理页等于不存在。
- 设计意图（S-F2.2 AC4 / WF 注记③）：整理期间以思考态（或等效过渡演出）**盖住延迟**——延迟期必须显示过渡页。
- 修法方向：start_organizing 在**检出收尾沿**（listen stop / 收尾词 / 自动收尾判定时刻、LLM 回合发起前）即下发，LLM 完成后 story_text 到达离态；或给过渡页最小驻留（≥2-3s）。front_app.py 收尾单源（_organize_and_archive / _issue_finish_downlink）调序。

## D4-2（P0）幻听/空内容被归档成垃圾故事（已清现场，根因待修）

- 现场（R3）：真故事收尾后用户在列表页按 KEY（列表页 KEY = 开新记录会话，`story_key rec_session_slot page=0` 9685205）→ 秒停（9686815 listen stop，rec_seconds=0 零内容）→ pending_stop 武装 → 幻听「Thank you.」（9697955 设备屏显 `>> Thank you`）喂入累积 → organize → **归档 `2026-10-20-no-reply`（seq=9 占位、SD 落卡 bytes=8）**。主线程已手工删除该垃圾篇（/opt/kidpal/stories/，seq 账本只增不减不动）。
- 修法方向（三处可选组合，fixer 裁）：① 归档前内容守卫——累积内容经幻听串过滤 + 最小有意义内容阈值（空/纯幻听/单语气词不归档、不占用 seq、走 R1-N4 安抚句或不打扰）；② 已归档会话的 pending-stop 不再武装（幂等）；③ organize 输入装配层的幻听串过滤（E4 种子已有设计要求，这次是真机实害）。

## D4-4（P1）kplog tail 512KB 上限回归 + 轮转疑点

- KPLOG001.TXT 长至 740KB 后 `kplog tail` 返回空帧（`EmitTail` 对超 512KB 文件零输出，log_cmd_port.cc:176-181）；`cat` 分片流式不受影响（本次 R3/R4 均靠 cat 取证）。**且文件超 512KB 未轮转**（KPLOG000 在 524KB 轮转了，KPLOG001 到 740KB 未轮）——轮转阈值/时机与 tail 上限假设（「文件封顶 512KB」）自相矛盾。诊断主通道，升 P1。修法：tail 改分块流式/提限 + 轮转阈值核实。

## D4-5（P1，旧账 escalation 067bd067）帧 B「引导播报中」置位链缺失

- 全仓无路径置 rec_substate=1：引导 TTS 播放时页面不切子态（键帽仍是收尾/丢弃，BOOT 毁稿分叉敞口）。fixer r2 评估「跨端改动可能触契约」挂起。建议路径：**设备侧从既有 tts 帧在记录语境内派生**（记录语境 + tts 到达 → 置子态；tts 结束/打断 → 复零），零契约变更；若评估必须新增 wire 信号，立即停走逃生门报主线程（契约门）。

## 已验证通过（勿回退）
- 抑制全链（记录语境闭嘴回显，R4 12 句零派发）；整理质量（「游乐场的一天」忠实整理：标题/分段/原话保留）；语音收尾「说完了」+ KEY 收尾双路径归档；单篇干净归档无重复；F4 列表回退已根修（R4 未再现）。
- 部署状态：front/story.py/front_app.py/connection.py 均已最新（story.py F3 词表 17:4x 补部署——此前漏部致 R4「说完了」未命中改走 KEY 路径，部署纪律教训记一条）。

## 验收建议（归 acceptance-author 通道，fixer 不直改冻结件）
- 成功的引导/整理回合端到端 case（含 auth）——R1 暴露的缺口至今未补；
- 幻听串不进归档的机判 case（E4 种子升级）；
- 幻听丢弃 → 设备退出思考态的引擎补丁 case。
