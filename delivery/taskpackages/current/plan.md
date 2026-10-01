---
changeId: 261001-m4-k3-story-archive-order
rigor: full
riskTier: high
sourceType: regression
reviewed-by: architect
date: 2026-10-01
---

# TaskPackage plan · 261001-m4-k3-story-archive-order（② 单包 Full）

> `/triage-change` 分流快审（architect，2026-10-01）+ human 触点①确认 ② 单包 Full 后落盘。
> 边界：本包**纯 front / story.py / 验收**，不触固件、不触契约、不触 spec 面；与 B2/B3（聆听形态 + 引擎侧抑制）分界见 §6。

## 1. 缺陷与根因

**现象**：孩子在 P3 按 KEY 停录后，录下的故事**没有被归档、彻底丢失**。

现场证据（`c:/tmp/serial-live-repair-r2.log`，261001 包 A 真机轮实录）：

```
130832  key_gesture_parser key=0 gesture=1                    ← KEY 单击
131012  MCP: story_key stop_rec archive_pend=1                ← 已走到 STOP_RECORD（包A D4a 已修好）
131062  StateMachine: State: listening -> idle                ← 关会话，上行 listen/state=stop
132772  Application: >> 今天是10/01放假了还要写作业，真不开心。说完了   ← 转写 1.7s 后才回
140512  Application: << 放假还有作业（引擎把整段当一轮回答并播报）      ← 另见 §6 残项
```

**根因（顺序缺陷，非竞态）**：

- front 在**同一条** `listen/state=stop` 上行帧上立即归档：`server/gateway/front/front_app.py:533` → `_story_stop_on_listen_stop`（`:121-139`）→ `acc.stop()`。
- 但转写只在**引擎收到该 stop 之后**才产生（manual 模式下 ASR 由 listen stop 触发），1.7 秒后才经下行 stt 到达。
- 归档瞬间 `st.accum` 必为空 → `server/gateway/story.py:561` `stop()` 返回 None，并把 routing 打回 `chat`。
- 随后到达的 stt 被 `_story_feed_from_stt`（`:104-118`，要求 routing == `accumulate`）丢弃；`_story_finish_on_phrase`（`:206-227`）同样因 routing 已回 chat 而不触发。**内容彻底丢失**。

**为什么验收没拦住**：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py` 的全部归档链齿喂数顺序都是 **stt → listen-stop**（`:709-712`、`:769-778`、`:869-871`、`:1164-1166`），与真机 **listen-stop → stt** 相反——冻结 oracle 固化了错误的时序假设。

## 2. 修法（architect 裁定：甲为主 + 乙兼容分支）

核心不变式：**归档动作恒发生在转写 stt 到达之后**；`listen/state=stop` 与 finish-phrase 都只是「该归档了」的信号。

- `listen/state=stop` 到达时：
  - `accum` **非空** → 立即归档（此分支是兼容 B2/B3 auto 模式的必要形态：auto 下录音期间 stt 已喂入 accum）；
  - `accum` **空** → 标记 **pending-stop**，**下一条 stt 到达**时 feed → 归档 → 下发 `story_text(seq)` + `story_list`（三时点②）。
- **pending 超时 = 10 秒**（实录转写延迟 1.7s，留足抖动余量）。超时后 routing 回 `chat`、释放 pending、**不写空故事**（V31 边界）。超时后迟到的 stt 因 routing 已回 chat 被自然丢弃，不污染。
- **pending 态须纳入 `StoryAccumulator` 状态机**，与 `stop()` / `cancel()` / `timed_out_sessions` 互斥协调（不双归档、不泄漏会话态）。
- **正文剔除收尾词**：归档正文剔除尾随收尾词（「我说完了」/「讲完啦」及其子串「说完了」）——实录转写尾带「说完了」，不剔除会污染故事正文。标题由模型另起（不直取正文首字），不受影响。

## 3. changeManifest

- `server/gateway/front/front_app.py`（listen-stop 挂起 + 转写到达后的归档接线 + 正文剔词）
- `server/gateway/story.py`（`StoryAccumulator` 新增 pending-stop 态与相关流转）
- `server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（归档链齿时序改真机序 + 补一条负锚齿）

**不在本包**：`server/gateway/front/front_app.py` 的 120s sweep 分支（同源缺陷，划 B2/B3，见 §6）；固件；契约 / spec 面。

## 4. 验收方案

- **时序改真机序**：全部归档链齿改为「先 `listen/state=stop` → 后到达 stt → 归档产出恰一篇」。
- **负锚齿（防复发，本缺陷的冻结 oracle）**：`listen/state=stop` 到达且 accum 为空时，`stop()` **不得**立即返回 None 打回 chat；须存在 pending-stop 态，后续 stt 到达才归档。
- **边界齿**：pending 超时（10s 无转写）→ 不写空故事、释放会话态；超时后迟到 stt 不污染；重复 stop / stop 与 cancel 交错不双归档。
- **正文齿**：归档正文不含尾随收尾词。

## 5. 风险与人门

- 改写已 merged 包的**冻结验收时序假设**（ADR-001:15 已知的人签字成本）→ 经 dev-loop 常规 human 门（supervisor riskTier=high 升级 + closeout）承载。
- 真机复验：P3 按 KEY 停录 → 列表新增一篇 + 盖章（`story_text(seq)`）；串口判据 = `story_key stop_rec archive_pend=1` 之后出现 `>>` 转写、随后 front 侧归档成功（不再 `empty_accum`）。

## 6. 与 B2/B3 的边界

- **本包交付到**：KEY 兜底收尾在当前 **manual 模式**下可用（stop → 引擎转写 → stt 到达 → 归档 → 下发）。
- **不交付**（依赖 B2/B3 的聆听形态变更）：逐句回显、说「我说完了」自动收尾、**120s 静默 sweep 复活**（manual 模式下 accum 恒空、无 stop 无 ASR 无 stt，B1 的 defer 机制救不了它）。
- **接口约定**：B1 的 pending-stop 判据含「stop 时 accum 非空 → 立即归档」分支，使 B2/B3 落地 auto 模式后 B1 机制无需返工。
- **残项（归 B3）**：收尾后引擎仍把整段当一轮回答并播报（实录 `<< 放假还有作业`），违反「收尾即盖章、AI 闭嘴」口径——属引擎侧抑制面，随 B2/B3 的 Track A。

## 7. 证据

- 真机实录：`c:/tmp/serial-live-repair-r2.log`
- 分流 review：architect（2026-10-01，同会话 return）
- 相关 spine：`delivery/changes/intake/261001-m4-k3-record-session-live-repair.md`（openQ q8 = D3 根因复核准）
