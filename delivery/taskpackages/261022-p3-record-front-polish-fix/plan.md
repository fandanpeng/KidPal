---
id: 261022-p3-record-front-polish-fix
title: front 记录会话族修复（引导节奏与「说完了」竞态 + 幻听入素材 + 列表排序与敷衍标题）
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect
date: 2026-10-22
status: pending
---

# plan · front 记录会话族修复（261022-p3-record-front-polish-fix）

> 来源：/triage-change 2026-10-22，architect 比例化 review 判 **②单包 Lite**（riskTier=medium，sourceType=regression），human 触点①已确认（两包并联、均 Lite）。四面探测触面 = 1（纯实现面），不建 spine；逃生门预查 = 干净（缺陷④带落点路由注记：引擎侧过滤候选排除——引擎 = 第三方部署树，front 侧自足）。
> **lite 判注**：「host 验收断言可闭环」——全部改动在 server/gateway，pytest 闭环（卡壳计时注入快进先例在世），无设备编译面。

## human 意图（触发链）

KPLOG005 真机轮：全天四笔记录会话，「说完了」笔笔迟到 5~14 秒成孤儿帧；13:02 笔 39 秒会话引导独播约 28 秒、末段引导与 start_organizing 同毫秒；「Okay」幻听（human 实证非本人声音）直进素材缓冲；新故事标题「我的故事」+ 同日随机 id 排序致 human 找不到刚录的篇目。三源取证落 debt 条目 `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md`（缺陷③④⑤ = 本包范围；①② = 并联包 A）。

## 范围（debt 缺陷③ + ④ + ⑤）

### 缺陷③ 引导垄断 + 收尾零缓冲，「说完了」恒成孤儿帧

**根因**：引导 TTS 独播挤压孩子说话窗口（39s 会话引导占 ~28s）；末段引导结束与 `control start_organizing` **同毫秒**下发（无应答窗）；引擎静默计时器 0.5~1s 收会话而 ASR 收尾词链路实测 5~14s——语音收尾信号结构性恒迟到，到达时 session ctx 已清 = 孤儿帧（全天四笔笔笔如此）。

**修法方向**：front 引导节奏——单段引导篇幅上限；末段引导与 start_organizing 之间留**应答窗**（延后计时起点/抑制计时，**不是**改 AC5「2 轮无回应自动收尾」轮数语义）；应答窗内到达的「说完了」经 is_finish_phrase 正常收尾。应答窗时长 = 手感参数，参考 ASR 实测 5~14s 定初值，human 真机轮终判，**数值不进 spec 文本**（卡壳阈值类「待定」值不夹带进本包 spec）。

### 缺陷④ 记录语境 ASR 幻听直进故事素材（human 实证）

**根因**：`_story_feed_from_stt` 素材旁路对引导播报期间 STT 不设防——引擎对话面抑制有效（不触发 LLM 回话），素材面与录音回显旁路全收。本次成文干净是整理 LLM 丢噪声（运气非设计）。

**修法方向**：front 素材入口对引导播报窗口 STT 丢弃或降权标记（窗口外正常进素材——验收须含反向 case 防过滤窗过宽误吞孩子接话）。**落点路由注记**：引擎侧复用 261014 幻听过滤的候选**排除**（引擎 = 第三方部署树，非自有实现面、不可版本控制），front 侧自足。④的过滤窗与③的应答窗共享「引导播报起止沿」概念——建议同一定时/状态源承载，1g 预评钉。

### 缺陷⑤ 列表排序与敷衍标题

**根因**：(a) `list_stories` 排序 = 日期倒序 + 同日按 id 字符串倒序（随机散列无时间序）——13:03 新篇排在 09:31 旧篇之下；`.seq.json` 账本 by_id 有严格递增 seq 可用。(b) 标题 = 整理 LLM 输出首行 ≤12 字，无敷衍兜底（「我的故事」实证）。

**修法方向**：(a) 同日 tiebreak 改 seq 倒序（缺 seq 记录回退 id 倒序不崩）——契约 §1.3③ 只钉「日期倒序」，tiebreak 是实现侧自由度，零 wire 形状变更；**设备端零扰动已预核**（固件按 items 到达序解析、全树无重排消费）。(b) 标题敷衍兜底启发式（front 侧，不动 faithful-organize.v1.md 制品语义）。

## changeManifest 种子（仓根相对）

- `server/gateway/front/front_app.py`（引导节奏：单段篇幅上限 + 末段应答窗 + 收尾竞态修；`_story_feed_from_stt` 引导播报期丢弃/降权；标题敷衍兜底启发式）
- `server/gateway/story.py`（list_stories 同日 seq tiebreak + 账本读取与缺 seq 回退；StoryAccumulator 计时面如需——`STUCK_GUIDE_SECONDS`/`GUIDE_ROUND_LIMIT` 语义不动）
- `server/gateway/front/prompts/guide-round.v1.md`（以 `_load_prompt_artifact(_GUIDE_PROMPT_FILE)` 实际解析路径为准——引导制品措辞：篇幅上限指令 + 不再暗示播报期间说「说完了」有效）
- `server/acceptance/test_record_dialogue_guide.acceptance.py`、`test_record_cancel_lifecycle.acceptance.py`、`test_m4k1_story_cloud_link.acceptance.py`（回归锚 + 新验收断言）

## 验收方向

1. **③新断言**：末段引导与 start_organizing 之间注入应答窗（计时可注入快进）；应答窗内到达的「说完了」stt 走 is_finish_phrase 正常收尾（不成孤儿帧）；引导轮篇幅超上限被截断/抑制。
2. **④新断言**：引导播报窗口内 STT 不进素材缓冲、不进录音回显旁路；**窗口外正常进素材**（反向 case 必载）。
3. **⑤新断言**：同日两篇按 seq 倒序（新在上）；缺 seq 记录回退 id 倒序不崩；敷衍标题命中兜底启发式。
4. **回归锚**：上述三件 + test_m4k3_record_session_live / test_engine_stop_truncation_asr_filter（幻听过滤先例形态参照）全绿；记录会话冒烟重跑。
5. **human 真机目检**：引导不再独播垄断、说「说完了」能正常收尾归档、新篇在列表最上且标题可识别。

## 不做清单（out of scope）

- 不改 AC5「2 轮无回应自动收尾」轮数语义；
- 不改 is_finish_phrase 词表（词表 = v1.66 需求面已冻，加词走 /requirement-change）；
- 不改契约 §1.3③ 任何文本；
- 不改引擎树（含 261014 幻听串过滤的引擎侧覆盖面扩张）；
- 不做整理 LLM prompt（faithful-organize.v1.md）语义重写（标题兜底用 front 启发式）；
- 不拍 spec 里「待定」的阈值数值进 spec 文本；
- 不动固件任何文件（并联包 A 的面）。

## 风险

1. 应答窗时长 = 手感参数（ASR 5~14s 实测定初值），human 真机轮终判；
2. ④过滤窗过宽误吞孩子接话——与③应答窗同源自洽消解，验收反向 case 兜底；
3. 单向守卫声明：riskTier=medium + rigor=lite，无 security/迁移/触契约信号，不触发 riskTier-overrides-rigor。

## 冻结齿映射（intake 终版回填，acceptance-author 冻结件承载）

- **FP-A1** = 验收方向 1①（应答窗 + 窗内「说完了」正常收尾）；**FP-A2** = 验收方向 1②（引导篇幅上限）；**FP-A3** = 验收方向 1④（引导窗口素材过滤 + 窗口外反向 case）；**FP-A4** = 验收方向 1⑤a（同日 seq 倒序 + 缺 seq 回退）；**FP-A5** = 验收方向 1⑤b（标题敷衍兜底）；**FP-A6** = 验收方向 4（回归网三族 + server 全量零新增红 vs 238 passed）；**FP-A7** = 验收方向 5（live-manual 真机目检，structural 口径不入 runner 通过面，前置 = front 重部署 ECS）。
