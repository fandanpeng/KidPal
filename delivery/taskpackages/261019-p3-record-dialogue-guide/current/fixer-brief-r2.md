# 真机轮 R2 缺陷现场包（261019-p3-record-dialogue-guide，2026-10-20 12:10-12:16）

> fixer 三轮输入。R1 修复（D1 auth / D2 UAF / D3 v7 渲染）已验证生效：本轮回合**零崩溃**，录音页渲染为 v7 帧 A 形态。以下为本轮新抓缺陷，human 五条真机反馈已全部定位。

## F1（BLOCKING）listen-stop（KEY 收尾）路径 = 旧 M4-K3 归档流，未接新设计

`front_app.py:909-912`：`_story_stop_on_listen_stop` → `acc.stop()` 原始转写直接归档 + story_text + story_list——**无整理 LLM 回合、无 start_organizing 下发、无出语境信号（kidpal_record_context active=false）注入、引擎会话未收尾**。对照语音收尾路径（:800-804 `_issue_finish_downlink`）= 完整新流（organize + start_organizing + 出语境信号）。

真机后果（human 现象 2/3/4 全归此）：
- 按 KEY 后**原始转写直接归档**（标题 = 首行「嗯」，口水词/断句全在，零整理）——归档文件实证：`/opt/kidpal/stories/2026-10-20-s88c57d64.md`（正文 = 逐句原始 stt，含尾行「好」=「好说完了」的残段）；
- 整理过渡页（state-organizing）**从未显示**（start_organizing 未下发）——徽章 0.8s 定格即过是设计，但前置整理页缺席；
- 引擎会话不收尾：归档后引擎仍在收 ASR（12:12:25「嗯。」+ 大模型收到），设备思考态（「不等了」键帽）挂 ~110 秒至引擎超时——设备日志 136.9s voice_look scene=1 top=4 → 247.9s 才回落。

修法方向：listen-stop 路径统一走完整收尾流（organize LLM → archive → story_text → start_organizing + active=false → 会话收尾），与语音收尾/自动收尾单源；注意 pending-stop（累积空、转写后到）真机序的兼容（SAO-A1/A2 既有冻结 oracle 保绿）。

## F2（BLOCKING）Package B 引擎侧抑制未生效——13 句叙述全部进 OpenClaw 对话

引擎日志 12:11:20-12:11:52：13 句叙述每句都有「大模型收到用户消息」= 引擎逐句派发了 LLM 回合（front 中继压制兜住未漏 TTS 到喇叭，但 token 与隐私面按 261001 human 裁定该由引擎侧堵）。引擎日志全程**零 kidpal_record_context 帧**。

两嫌疑（fixer 先判哪个，再修）：① front 注入未发/未达——`:773-777` 注释称「control/start_recording 消费沿同拍注入 active=true」，但本轮入口 = OpenClaw 意图判定 → E1 display 流 control/start_recording 经 front relay 拦截，核该收敛路径是否真触发了 `_inject_record_context_enter`（:813/:827 两入口之一可能没被这条链路踩到）；② 注入到达但引擎认领/守卫失效——connection.py `_kidpal_on_record_context` 认领与 chat() 第三守卫的执行序（「大模型收到用户消息」日志行在守卫前还是后？若守卫后无 dialogue.put 则只是日志噪音，需 OpenClaw 侧对账确认 LLM 回合是否真发生）。

## F4（BLOCKING）P3 列表新故事出现后又消失（3→2 回退）

- 12:12:25 归档后 `story_list_dispatch items=3`、设备 136.5s 列表渲染 vcount=3 lcount=3（新故事在列）；
- 用户回首页再进 P3：247.9s / 261.0s 渲染变 vcount=2——窗口内引擎**无第二条 story_list 推送**，即设备侧列表模型被本地因素回退。
- 嫌疑：设备 story_page 列表模型在 voice overlay 关闭/页重进时被旧缓存覆盖（SD 索引 story_sd_rebuild_index entries=2 是现成嫌疑）或盖章 overlay 消费与列表模型的双源。fixer 读 story_page 列表数据生命周期定位。

## F3（已裁，fixer 顺带落实现）收尾词表并入「说完了」

**human 2026-10-20 裁：并入**——需求面 v1.66 已 frozen（批 261020-finish-phrase-shuowanle，档 1，commit e724ff48；261001 q3 关闭）。实现 = `server/gateway/story.py` `_FINISH_PHRASES` 加「说完了」一词（子串扫描天然覆盖「好，说完了」前缀变体）+ 既有收尾词相关验收齿同步核对（词表 = 实现期可调常量，齿若有词表枚举断言随动）。注意 story.py 与 F1 同文件，两段改动合并冲突自查。

## F5（观感记录，非缺陷）盖章 0.8s 定格即过是设计（WF-F2-03），human 觉得「闪了一下就没了」——留观感轮评估是否延长，不在本批。

## 已验证通过（不要回退的部分）
- 零崩溃（D2 UAF 根修生效）；录音页 v7 帧 A 渲染（D3 生效）；front auth 修复后……注意本轮 KEY 路径没走 organize，**organize LLM 在 auth 修复后仍未被真机验证过**——F1 修完后 R3 轮必须同时覆盖语音收尾与 KEY 收尾两条路径的整理质量。
- S-F1.4 AC6：本轮无提醒交叠，未回归项待 R3 补。

## 日志证据
- 板端：`/c/tmp/kplog-r2-261020.log`（500 行尾，135.1s story_key stop_rec 起全链）
- front：ECS `journalctl -u kidpal-front --since '12:10'`（story_stop archived=true 12:12:24，无 llm_round 行）
- 引擎：ECS `docker logs`（识别文本 ×15 + 大模型收到 ×15 + listen stop 12:12:24 + story_text/story_list 12:12:25）
- 归档件：`/opt/kidpal/stories/2026-10-20-s88c57d64.md`（原始稿实证）
