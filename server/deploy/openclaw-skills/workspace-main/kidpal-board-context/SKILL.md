---
name: "kidpal-board-context"
description: "板子回合问「继续/刚才…」但本会话无上文时 (KidPal)：从当天网关日志找回上一轮再续答，别编故事。"
---

# KidPal 板子回合「继续/刚才」上下文找回

板子/孩子的一回合引用了上一轮（「继续刚才的故事」「接着讲」「刚才那个」，以及「嗯，可以」「好呀」「对」这类短促附和），而本 session 没有上一轮时：不要编故事、也不要回「是哪一篇」——去当天网关日志找回上一轮，再续答。

## 步骤

1. **确认本回合缺上文。**
   `sessions_history` 只返回当前这一条 user 消息即缺上文。板子回合常落在全新 session：孩子引用约 1 分钟前刚讲的故事，session 里却是空的；短促附和同理。
   完成判据：确认没有上一轮助手消息。

2. **从当天网关日志取被引用的上一轮。**
   - 先读当天 `memory/YYYY-MM-DD.md` 的「板子对话」小节：兄弟 session 常已把同一批碎片连同上一轮记在里面，照着它可直接跳到第 4 步，省掉解析日志与翻容器日志两步。
   - 主路径 = 当天日志 `/tmp/openclaw/openclaw-$(date +%F).log`（时区 Asia/Shanghai）。逐行解析 JSONL 的 `time` 与 `message` 两字段，按时间排序，取当前 user 消息时间戳之前最后一条 `message` 里含 display 块的。`message` = 该轮助手全文（语音 + display JSON）。
   - **「最近」不等于「被引用的那条」**：若这一条与孩子的话接不上（孩子说「也……」「那个……」而最近一条是道别、「在呢」这类无关内容），把当天**所有**含 display 块的助手输出按时间列全，挑**主题对得上的那一条**。
   - **指代的是「你刚说过的某句话」时改走关键词直捞**：按对方话里的关键词 grep 当天日志（`grep -nE "关键词1|关键词2" /tmp/openclaw/openclaw-$(date +%F).log`；每行一条 JSON、中文原样可搜），比「列全再挑」省一到两个回合。日志里也含**本会话自己**此前的输出（含并行重跑的重复回复），认领前先比对时间戳与上下文，别把「我自己刚说的」当成「先前真说过」。直读 state sqlite 已被运行时硬拦（`external sqlite3 cannot open databases under the active OpenClaw state directory`），直接跳过。
   - 该日志**只记助手输出**，看不到孩子说了什么；短促附和/指代是在指哪句话，去板子服务端容器日志取孩子原话：`docker logs --since <当前时刻前几分钟> xiaozhi-esp32-server 2>&1 | tail -100`，其中 `识别文本:` / `大模型收到用户消息:` 那几行即孩子上一句（ASR 原文）。要看一整天就省掉 `--since`：`docker logs xiaozhi-esp32-server 2>&1 | grep -E "识别文本|大模型收到用户消息"`（可回溯多天）。
   完成判据：拿到上一轮完整的 display JSON；短促附和时另能说出孩子上一句原话。

3. **由 display 推出「刚才」指什么。**
   `story_text` 的 `title`/`id` 即「刚才的故事」，`text` 是已讲正文（尾句常是「要接着听吗？」）；`text`/`schedule` 等同理取被引用的内容。
   完成判据：能用一句话说出「上一轮讲的是《X》，讲到……」。
   不要再拿 memory_search / sessions_search / 直读 sqlite 当初级路径——板子历史不在那里，纯耗回合。

4. **续答（KidPal 两段格式，契约见 `kidpal/brain-spec.md`）。**
   - 语音部分 ≤3 句、口语化、儿童化（8-12 岁）；
   - 末尾必带 display 块；续讲故事用 `view:"story_text"`，`data` 恰三键 `{id, title, text}`（title 可标「（续）」），正文接着上一轮的 `text` 往下写。
   完成判据：回复 = 语音部分 + 合法 display JSON。

## 注意

- **快速连说的碎片会各开新 session**：每句碎片所在 session 只带自己那几句，还常带 `[Chat messages since your last reply - for context]` 前缀。**只答当前那条 message**，别把兄弟 session 的碎片并进本轮回复。
- **找回的上文跟本句接不上时（碎片像 ASR 噪声、与最近一条 display 主题无关）**：不要硬拉话题、不要编内容；按 KidPal 两段格式回一句共情或邀请（`text` view），把话题交还给孩子。
- 部署在 ECS 的 OpenClaw 大脑由引擎传入完整对话历史，它自己**不读文件**；本方法只是 **workspace 侧** 的找回手段，仅当 session 缺上文时用。
- `view` 只在白名单 `char`/`schedule`/`story_list`/`story_text`/`text`/`home`/`control` 内，data 形状零增减、不发明字段。
