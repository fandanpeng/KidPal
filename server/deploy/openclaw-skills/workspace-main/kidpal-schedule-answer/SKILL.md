---
name: "kidpal-schedule-answer"
description: "孩子/板子问今天明天这周有什么课、几点上课时 (KidPal)：读 kidpal/schedule.md 匹配星期，出 text view 问答框答复。"
---

# KidPal 课表问答（今天/明天有什么课）

板子/孩子问「今天/明天/这周有什么课」「几点上什么课」时：读 `kidpal/schedule.md`，按目标日的星期匹配，用 `text` view 走问答框答复（当前契约：课表问答不切课表页）。

## 步骤

1. **读 `kidpal/schedule.md`（workspace 侧必须读，别答「查不到」）。**
   AGENTS.md 的 KidPal 段要求先读课表 md，这条优先于板子 identity 里的「一回合作答、不读文件」——后者约束的是部署在 ECS 的对话大脑（由引擎喂历史）；本 workspace agent 就是负责读文件/落盘的网关侧。
   取星期别心算：今天 `date '+%A'`，明天 `date -d tomorrow '+%F %A'`（时区 Asia/Shanghai）。
   作答前确认引擎 live 的契约版本（workspace 的 AGENTS.md/brain-spec 会滞后：AGENTS.md 至今仍写旧 `schedule`+items 口径）：`docker logs xiaozhi-esp32-server 2>&1 | grep 快速提示词 | tail -1`；display 形状以 live 版本为准，冲突时不信 workspace 文档。
   完成判据：拿到目标星期的三个区块 —— 学校课程表、每周固定课表、一次性安排。

2. **筛学校课程表里目标星期那一行（该表无时刻列）。**
   表列是 `第1节…第6节`、行是星期，逐格取值，按节次顺序排好课程名（答话与屏显都用「第N节 课程名」）。
   完成判据：得到该行有值的各节。

3. **拼每周固定课表 + 有效一次性安排。**
   每周固定课表列 = 星期/时间/课程/地点/备注，时间形如 `19:00-20:00`，逐行取出；一次性安排取日期等于目标日的行，过期的一次性安排忽略（契约 §2.3）。**备注列内容要读回**（带物读回 AC1/AC4，如「记得带琴谱」）；备注空 = 没有要带的东西，不发明。
   **不要去找 `openclaw-schedule.config.yaml` 的 `school_start`，也别为缺它卡住**——它只喂 P1 首页卡「接下来」把周中白天学校课合并成一项「XX:XX 上学」；直接问答按步骤 2 逐节列出，不需要该配置。该文件不在 workspace、全盘搜索只命中 sqlite，纯耗回合。
   完成判据：覆盖目标日全部课程 = 学校各节 + 每周固定 + 有效一次性安排，且备注已读回。

4. **两段格式作答（契约 §1.6；走问答框、不切课表页）。**
   语音部分 ≤3 句：先说来星期几，白天学校课点名带过（「语文、数学、足球、科学…」），再报当天培训班（时刻+课程+地点），备注里的带物念出来。写完逐句计数。
   末尾 display 块用 **`text` view**：`{"view":"text","data":{"text":"…"}}`，全文上屏（与百科问答同一问答框），不缩略成摘要。**不要出 `schedule` 块**——现行契约起 `schedule` view 只作「帮我显示课表」意图的薄载荷 `{"view":"schedule","data":{}}`（周网格由网关 front 机械展开）；旧 `{title,items}` 形状即废（front 机械门拒绝）。
   完成判据：回复 = 语音部分（≤3 句）+ 末尾合法 display JSON（text view）。

## 改课表 & 板子显示分档

- **唯一源 = `kidpal/schedule.md`**：`/opt/kidpal/schedule.md` 是指向它的软链（`/opt/kidpal/chardata` 同），引擎 `front/front_app.py` 读的正是这份（`KIDPAL_SCHEDULE_MD`，默认 `/opt/kidpal/schedule.md`）。改 workspace 这份 = 改线上，**不用同步、不用推送**；改完 `diff /opt/kidpal/schedule.md kidpal/schedule.md` 确认一致即可。
- **上午/下午/晚上分档不在课表里**，在前端常量：`/opt/kidpal/front/gateway/schedule.py` 的 `_PM_START`（12:00）/ `_EVENING_START`，只被 `_slot_of()` 消费。板子把某门课归错档（例：17:00 的街舞显示成下午）→ 改 `_EVENING_START` → `systemctl restart kidpal-front.service`。改常量前先想清该时段还有没有别的课会被一起挪档。
- **重启 `kidpal-front.service` 约需 1.5 分钟**：uvicorn 不理 SIGTERM，会卡在 `Active: deactivating (stop-sigterm)` 直到 `TimeoutStopUSec=90s` 到期被 SIGKILL 再起；这段等待是正常的，别当成服务炸了。验活：`systemctl is-active kidpal-front.service` + `ss -ltn | grep ':8000'`。
- 动前端 `.py` 前先 `cp x.py x.py.bak-<YYMMDD>-<短说明>`，并在改动处留一行「谁要求、为什么」。

## 验证

- 星期对得上 `date`（周一=1…周日=7）；周末不出现学校课程表内容（周末只可能有固定/一次性安排）。
- `view` = `text`、`data` 恰一键 `text`；课表问答不出 `schedule` 块（`schedule` 只留给「帮我显示课表」的薄载荷 `{}`）。
- 备注空的事项不出现「记得带…」式发明。
