---
changeId: 261010-device-reconnect-on-server-close
rigor: full
riskTier: high
sourceType: regression
reviewed-by: architect（/triage-change 分流 review；触点① human 裁定「永远不休眠」扩范围修订，2026-10-10）
date: 2026-10-10
---

# 固件断线自动重连 + 永不休眠常驻在线（TECH-261010-device-no-reconnect-on-server-close；arch §8 休眠产品决策同批落档）

## human 裁定记录（触点①，2026-10-10）

- 四档裁决确认：①Full / ②Lite / ③spine+restore。
- 本包呈报项新裁定（原话）：**「永远不休眠」**——设备常驻在线、永不休眠，替代 architect 推荐项「先修重连、休眠留账」。电池代价取舍随此裁定一并由 human 裁定（见「电池代价注记」），plan 只申报、不重开讨论。

## 触面复核（扩入「永不休眠」后）

触面仍 = 实现面单面，**不建 spine、不升 Track A**。理由：① 裁定已由 human 在治理触点（触点①）显式作出，arch 文本落档是机械转写 + 残余闭账，非新设计（作者≠判官无虞：判官 = human 已裁，本包只转写）；② 残余行（arch §3:133 / §8:354）描述的正是本包根治的行为，spec 行随实现落地同步是 dev-loop docs-sync 门既有职责；③ 先例：决策 21（引擎 24h 兜底）的 arch §6.4 更新即随 dev 包 docs-sync 落地（04c86a9 同批）；④ 契约面零触（WS wire 形状零变化）、需求面零触（提醒必达语义已钉，本包是兑现）。**备选呈报（存疑向严）**：若 human 认为该产品决策落档应另走正式 Track B 治理小批（独立 PROPOSAL + 评审循环），可拆出——推荐不拆（触点①已是显式裁定 + 落档指示）。

## 根因假设

- **子项 A（断线自动重连缺口）**：application.cc:634-641 OnAudioChannelClosed 仅 SetPowerSaveLevel(LOW_POWER) + SetDeviceState(Idle)，零重连；唯一自动建连恢复路径 = 网络重连事件（application.cc:406-410 → TryStartAutoConnect，K6）。websocket_protocol.cc:201-206 OnDisconnected → on_audio_channel_closed_ 不分本地主动关与服务端/网络侧断；IsTimeout() 120s（websocket_protocol.cc:77-79）只令 IsAudioChannelOpened() 返回 false（覆盖式）、不自发重连——机制上解释 10-10 日志的 13 分钟黑洞。
- **子项 B（休眠/离线驻留残余的物理载体，重核）**：**本板（waveshare-s3-rlcd-4.2.cc）零 power_save_timer / sleep_timer 实例化、零 OnEnterSleepMode / OnEnterLightSleepMode 接线**（实核）——「闲置约 3 分钟休眠断连 → alert_skip no_online_device」（arch §3:133 / §8:354）的可观察实质是**复合观察**：旧引擎 120s 主动关（主因，已闭环）+ 本地关通道后的**离线驻留**（每次会话收尾本地关 WS → 设备离线直到下一次交互）+ LOW_POWER modem 省电（wifi_board.cc:302-317 → WifiManager 档位）。common 层休眠入口（sleep_timer.cc:37 / power_save_timer.cc:33，NVS `sleep_mode` 默认 true）在本板编译面不可达。子项 B 落地后该残余自然闭账（见 docs-sync）。
- 既有可复用基建：TryStartAutoConnect（application.cc:477-517）= 守卫三条件（Idle && 通道未开 && 非 in-flight）+ 3 次退避 {5,10,20}s 后台 task + 每次重试前重查守卫。

## 修复方案

### 子项 A：服务端/网络侧断 → 自动重连（原范围）

- websocket_protocol 增「本地主动关」标记：CloseAudioChannel()（:81-83）置位；OnDisconnected 据此分流——本地主动关不触发 A 路径，服务端/网络侧断触发。（实现前核项：Websocket 对象 reset 析构是否回调 OnDisconnected——是则标记须覆盖析构路径。）
- OnAudioChannelClosed 在非本地关路径 Schedule 排入 TryStartAutoConnect：复用既有守卫与退避、不另起第二条重连路径、不新增退避表；3 次穷尽后静默停（不自旋）。
- 屏显不受影响口径（requirements.md:67）：重连尝试期间维持 Idle 屏显、不闪 CONNECTING、不弹错误卡——新触发点沿用「重连区零 SetDeviceState」负锚纪律。

### 子项 B：永不休眠 · 常驻在线（裁定落地的可观察语义）

- **语义**：设备在 Idle 态恒持一条活 WS 连接（常驻在线），front 推送（kidpal/alert、story_list 等）任意时刻可达；不再存在「会话收尾后离线驻留窗口」。
- **实现 = 会话收尾本地关后的短延迟常驻重建**（不动「会话结束关 audio channel」既有拓扑）：会话收尾类本地关后 T 秒（退避表首档 5s 量级，实现期定值并冻结进齿）Schedule 排入 TryStartAutoConnect 重建常驻连接——复用 A 的同款基建与守卫，触发语义独立、验收齿分开钉。
- **本地关调用点归类表（8 处，实核；builder 逐点归类留痕为验收前提）**：

| 调用点 | 归类 | 处置 |
|---|---|---|
| application.cc:422 网络断关（HandleNetworkDisconnectedEvent） | 网络断 | **零重建**（网络没了重建必刷日志；恢复走既有 K6 网络重连事件） |
| application.cc:805 Application::CloseAudioChannel（K3 [S5] 录音态丢弃面） | 会话收尾 | **B 重建**（丢弃后回 Idle → 常驻在线） |
| application.cc:849 HandleToggleChatEvent Listening 分支（会话 toggle 收尾） | 会话收尾 | **B 重建** |
| application.cc:1081 Reboot | 生命周期 | **零重建**（重启流程接管） |
| application.cc:1100 OTA 升级前关 | 生命周期 | **零重建**（升级流程接管） |
| application.cc:1189 Listening 态关（handler 内） | 会话收尾 | **B 重建**（builder 复核该 handler 语义后归类留痕） |
| application.cc:1254 AEC 模式切换关 | 功能性 | **零重建**（切换后下个会话重开通道，既有语义零偏移；builder 复核留痕） |
| application.cc:1267 ResetProtocol | 功能性 | **零重建**（protocol_.reset() 后由激活路径接管） |

- **休眠入口禁用（「永不休眠」结构面）**：本板休眠路径现状 = 零实例化（结构事实）；本包以结构齿钉死「本板永不接线休眠 timer」（needle 形态），common 层 `sleep_mode` 设置保持编译面不可达——若实现期发现任何休眠入口在接线面可达，显式禁并留痕。
- **常驻在线省电档（实现自由度，不改裁定语义）**：常驻连接的 Wi-Fi modem 档位（LOW_POWER/BALANCED 与 WS keepalive 的兼容性）真机核——若 LOW_POWER 下 ping/pong 稳定，Idle 常驻可不开 PERFORMANCE 档以缓解电池代价；前提 = keepalive 稳定，值真机验证后冻结。
- **交互核验面（零改动、回归齿覆盖）**：常驻在线改变「每会话重开 WS」既有拓扑（260928 plan 记录的拓扑事实）→ KEY 在复用通道上开会话的 seam 已按通道复用设计（application.cc:1014-1020 注释「门外 = 通道复用会话不短路漏发，notes ②③」；front ctx_push_edge 为会话基非通道基，front_app.py:212）——既有 story ctx / reminder 齿回归即可，不新增设计。

## docs-sync 落档（「永不休眠」裁定 → arch 四投影点，integrator 收口步承载）

1. `docs/architecture.md` §10 决策表 append 新决策行：「决策 29 · 设备永不休眠、常驻在线——提醒推送可达性优先于闲置功耗；板载电池续航代价随裁定接受（human 2026-10-10 触点①裁定，原话『永远不休眠』）」。
2. `docs/architecture.md:133`（§3 假设表「设备常驻在线」行）：残余注改写——「残余 = 闲置约 3 分钟休眠断连【产品决策待裁】」→「已裁：永不休眠（决策 29）；重连 + 常驻重建已落（260910 包）」。
3. `docs/architecture.md:354`（§8 风险债 3 行）：待裁项闭账（已裁 + 本包根治）。
4. `docs/architecture.md:322`（§6.4 已知残余句）：同步删除/改写休眠残余表述，补常驻在线口径。
- 本包不落 requirements.md（提醒必达语义已钉，无新增语义）；不触契约。

## 电池代价注记（诚实申报，已裁不重开）

常驻在线 = WS keepalive 常开 + Wi-Fi modem 常供电，板载 18650 电池（arch §5.3.1 电池行）**续航显著缩短**，使用形态实际转为随用随充。此取舍已由 human 以「永远不休眠」裁定一并接受（2026-10-10 触点①，原话）；本 plan 只申报不重开讨论。缓解自由度（不改裁定语义）= 上文 Idle 常驻连接 modem 档位真机调优。

## 影响面

- 文件：firmware/main/application.cc（OnAudioChannelClosed + 会话收尾重建触发 + 调用点归类注记）、firmware/main/protocols/websocket_protocol.{cc,h}（本地关标记）；firmware/main/boards/waveshare-s3-rlcd-4.2/（休眠入口结构 needle 如需）；server 面零改动。
- docs-sync：arch §3/§6.4/§8/§10 四投影点（见上节）。
- 需求/契约面零改动（requirements.md:67、arch §6.4 已钉语义——本包 = 兑现）。
- 触发点新增（A 服务端断 + B 会话收尾重建）如撞 boot_auto_connect.acceptance.c 既有 needle「触发点 = 2」类结构钉 → 走正常改验收通道 + human 方向锚留痕（260928 FIX-A2 先例形态）。

## acceptance 种子

impactedTests：firmware/main/test/boot_auto_connect.acceptance.c（K6 冻结件——新触发点结构核 + 既有 27 齿零破坏红线）；新增 host 纯逻辑齿（本地关标记判别 / 触发分类纯函数 / 常驻重建排程守卫）；story ctx 与 reminder 既有齿回归（复用通道 seam：application.cc:1014-1020、front ctx_push_edge 既有断言件）；固件 unit/acceptance 全量闸照跑（机械门禁全量不放松）。

**子项 A 验收齿草案**：
- RC-A1（红基线, host 纯逻辑）：非本地关（服务端断）→ 自动重连排程在场。
- RC-A2（host 纯逻辑）：服务端断路径与 B 路径触发分类互斥（同一关闭事件恰归一类、不双触发）。
- RC-A3（host 结构齿）：重连复用 TryStartAutoConnect（零第二条 task/零新退避表；新触发点区零 SetDeviceState——屏显零触碰）。
- RC-A4（真机轮, live 观测）：手动断服务端（重启 front/引擎容器）→ 退避窗内设备日志自发出现完整 hello 建连序列（三日志源对账，复刻 10-10 对账法）；期间 P1 屏显不变。

**子项 B 验收齿草案（新语义）**：
- RC-B1（host 结构齿）：「永不休眠」结构事实——本板板级文件零休眠 timer 接线（needle 钉 waveshare-s3-rlcd-4.2.cc）；common 层休眠入口编译面不可达留痕。
- RC-B2（host 纯逻辑）：会话收尾类本地关（:805/:849/:1189，按 builder 归类表）→ T 秒内恰一次常驻重建排程（复用守卫三条件）；T 值实现期定并冻结进齿。
- RC-B3（host 纯逻辑, 负锚）：网络断关（:422）/Reboot（:1081）/OTA（:1100）/AEC 切换（:1254）/ResetProtocol（:1267）零重建排程——各路径既有语义零偏移。
- RC-B4（真机轮，替代原 RC-A5）：会话结束后 T 秒内重建并**常驻在线**——静置 >3 分钟乃至跨夜后 front 发提醒可达（front/引擎日志无 `alert_skip no_online_device`，设备渲染且回执）；P1 常显不受影响。
- RC-B5（真机轮, 回归）：常驻在线下 KEY 开新会话在复用通道上正常（story ctx 发送短路防 + reminder 四态 + 引导态演出既有齿真机回归）。
- RC-B6（真机轮, 观测项）：Idle 常驻连接 modem 档位与 keepalive 稳定性观测（LOW_POWER/BALANCED 下 ping/pong 长稳则取低档，冻结结论进 docs-delta）。

## 非目标

- ~~「闲置 ~3 分钟休眠断连」产品决策~~——**已裁（永不休眠），随包落档闭账**（docs-sync 节），不再留账。
- 「会话结束关 audio channel」既有拓扑（会话生命周期语义）不改——常驻在线经收尾重建达成，不改为「会话不关通道」方案。
- 网络断路径既有行为（HandleNetworkDisconnectedEvent:417-428）零改动；server 面零改动。
- 电池/充电形态的产品面方案（如低电量策略）——不在本包，如真机观测暴露续航不可接受，升级 human 另行裁定（不在本包内自裁）。

## demo 证据形态

host 红基线先红后绿全程留痕 + 真机轮（human 在场）四场景：服务端断自发恢复（A）/ 会话收尾后常驻重建 + 跨夜静置提醒可达（B）/ 各功能性关零重建（B 负锚）/ 复用通道会话回归（B 交互面）。烧录 = 仅 app 分区（无资产变更，不触双分区烧录规则）。
