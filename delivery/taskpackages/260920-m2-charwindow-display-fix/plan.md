---
rigor: lite
riskTier: medium
sourceType: human-confirmed-plan
reviewed-by: architect
date: 2026-09-20
---

# 260920-m2-charwindow-display-fix · 查字窗展示面修复（右栏去遮挡 + 提示条让位 + 云端释义中文化）

> 来源：human 真机轮 finding②（`260920-m2-charwindow-card-fix` 落地后复验）+ `/triage-change` 比例化 review（architect，2026-09-20）。本 plan = reviewed plan 搬运 + human 触点①决策收敛，主线程只搬运不改内容。
> human 口径（判据）：**「板子端就是一个展示的地方，不用限定结构，云端给什么我们显示什么」** + **「就按语音播报的内容，除了笔画顺序不显示之外，语音播报什么我们就显示什么」**。

## 背景与根因（architect 复核 + planner 对磁盘核实）

- **遮挡真凶 = 恒显顶部状态带**（planner 原指认 voice_page 右内容区**已证伪并纠正**）：`top_bar_`（通栏 `LV_HOR_RES` × `LV_SIZE_CONTENT` ≈20–24px、`bg_opa=LV_OPA_50`）+ `status_bar_`（同区绝对定位），二者在 `KidPalReparentChatToLayer` 的 `chat_top[]` 清单内被 reparent 到 `lv_layer_top()`、按设计**任何状态恒显** → 覆盖 y≈0–24 全宽。
- 右栏 `cardlabel` 起点 `(228, 8)`（`char_page.cc:574`）→ 拼音行 y8–26 整行被压、第二行 y26–44 上半被削——与 human 现象逐位吻合。
- `voice_page`（header/echo/answer box）为 `lv_layer_top` 上**默认 HIDDEN** 对象、显隐单点 = `kidpal_chrome_mid_sync_locked`（front≠VOICE 时整页隐藏，且其 `bg_opa=COVER` 若在屏会盖掉整窗）——**非遮挡物**。
- **同族旗标**：底部提示条 `hint_label` 在 `(0,280)`（`char_page.cc:594`），`bottom_bar_` 占 y252–300 → 同族遮挡（human 已裁**本轮一并修**）。
- **释义英文根因**：`/opt/kidpal/chardata/char_cache.json` 的 `meaning` 取 makemeahanzi `definition`（英文）；D3 契约样例（`docs/contracts/interface-contract.md:58`）语义为中文「勇敢；有胆量」——**生产端偏离既有契约语义**，修复 = 向契约收敛，**零契约改动**。

## human 触点①决策（收敛后的范围，覆盖 architect 原方案的两处）

1. **档位/拆包**：② 单包 Lite **合一包**（`firmware/` + `server/` 两端，含 `crossEnd` 双向申报）；riskTier=medium（动生产 prompt + 网关出口改写，交 supervisor 机械闸复核，不取 low）。
2. **云面只改一件**：把 OpenClaw **已经生成、已经在播报**的那句中文解释写进 display 块**既有的** `meaning` 字段——**TTS 播报内容零变化、不新增字段、不改契约、不改板端渲染**。同一回合同一 LLM 写语音与 meaning ⇒ 播报与显示构造性一致。
3. **撤回组词**：human 裁「现在的播报内容就够了」→ 播报不读组词 ⇒ 显示面不凭空多出行。**不做**：不补组词数据、不让播报读组词、**不重建 char_cache**（architect 备选方案「中文词典源重建 9574 字 cache」随之作废，cache 退为 fallback 层）。
4. **空行不显示**：右栏任一行（组词/部首等）数据为空 → **该行不拼装**（不显示空行、不显示占位符）。
5. **旗标项**：**提示条一并修**（上移避开 `bottom_bar_`）；**米字格大框顶部不动**（human 未觉碍，50% 半透仍可见）。

## 改动面

### 板端（`firmware/main/display/char_page.cc`，单文件）
1. **右栏让位**：`cardlabel` 起点 `(228,8)` → **`(228, 30)`**（清出顶部状态带 + 余量）；补 `lv_obj_set_width(cardlabel, 160)`（388−228）+ `LV_LABEL_LONG_WRAP`——现 label 无宽度约束，释义改中文后变长**必然冲出右缘被裁**，同刀钉死。
2. **提示条让位**：`hint_label` 从 `(0,280)` 上移到避开 `bottom_bar_`（y≥252）的位置。
3. **空行不显示**：拼装层按字段有无选择性地拼行（组词/部首空则不拼该行）。
4. **组词多词拼装**：现只取 `words[0]`；改为数组前 N 项以「、」join（`kidpal_char_info.words` 注释本就写明「、-joined」、`card.words[96]` 容量按多词设计）——**不动 `char_stroke.c` 纯逻辑合约**（U 冻结面 + B8 守卫不触），装配在 char_page 层。
5. **不动 chrome**：让 `top_bar_`/`status_bar_` 随查字窗隐藏会扩大前台维度显隐面、横跨提醒（K4 最高层）/语音轮四态既有闭环，收益仅右上一条带 → 取内容让位（单文件局部几何改动，零 chrome 语义变更）。

### 云面（`server/deploy/**`）
1. **prompt v5**（新文件 bump，勿回改 v4）：char 段最小意图块 data 增 `meaning`（=**中文释义**，建议 ≤24 字钉进 prompt；板端 `meaning[96]` = 32 CJK 字硬上限），语义 = **同回复播报里那句中文解释的同源文本**；「严禁自生成 strokes/medians 坐标」「≤3 句」「一回合作答」逐字保留。bump 前按槽位预约规则核 v5 未被并行分支占用（ARQ-260908 教训），同步 `OPENCLAW_SYSTEM_PROMPT_CARRIER` 与 `apply-xiaozhi-overlay.sh` 渲染引用。
2. **网关 merge**（`make_openclaw_patch.py`）：`k1cRewriteText`/`k1cLookupBlock` **两条出口路径（流式 + 非流式）一致地**取最小块 `meaning` 覆盖 cache 值再 emit；缺键回落 cache 原值；HOLD-AND-REPLACE 总语义不变（最小块成品永不出网关）。
3. **python 镜像**（`char_stroke_lib.py`）：加 merge 语义，保持「TS = python 冻结语义镜像」纪律。其 docstring 所指冻结验收 `server/acceptance/test_char_stroke.acceptance.py` **已归档** → 指针改指本轮新验收件（**禁落 archive 名目录**，验收纪律 §5）。
4. **部署腿**：`apply-openclaw-patch.sh` 重跑（dist chunk 重注入 + drift-guard 先 grep 后 apply）+ overlay 渲染 v5 prompt + 服务重启，按 `DEPLOY-RUNBOOK.md`。live 腿按「probe ≠ exercise」**真跑一次查字端到端**（真发一问、板子收到中文释义块），不得只探端口。

## 关键风险

- **板端**：内容让位零 chrome 语义变更 → 对提醒最高层/语音轮四态零影响面；残余风险 = 几何手改无机器渲染验证 → 靠齿 + 真机腿。
- **云面**：① 网关**双出口路径（流式/非流式）merge 不一致** = 离线绿、真机一红（260907-g-relay-fix 同族）→ 验收齿必须两路径都打。② LLM 漏写 meaning → 回落英文：prompt 钉「必带」+ 板端空行不显示兜底；持续漏 = retro 后议。③ v5 bump 槽位冲突面 → bump 前核对。④ 既有查字流幂等性：cache miss 原样放行路径不动，merge 仅在命中后追加覆盖。

## 验收齿形态（诚实分级）

**host 可机判（钉「决策」而非「渲染」）**：
1. **板端源锚齿**（KVR 先例形态）：grep `char_page.cc` 断言 `set_pos(cardlabel` 第二参 ≥30、`set_width(cardlabel, 160)` 在场、`LV_LABEL_LONG_WRAP` 在场、`hint_label` 位置避开 bottom_bar_ 带、空行不显示逻辑在场、words 数组 join 在场（非只取 `[0]`）。**每条锚必配负向变异自证**（把值改回去 → 齿转红；MCF 已证负向齿有效、K3 A5a 正向包含齿假绿教训）。
2. **纯逻辑齿**：card_build 96B 截断 + 中文多字节边界补中文满长 case（防 UTF-8 半字截断）。
3. **云面 pytest（host-fast 真跑）**：merge 语义——LLM meaning 覆盖、缺键回落 cache、**流式与非流式两路径一致**、替换后 JSON 合法、无最小块泄漏；prompt v5 锚齿（char 段含 meaning 新约 + 「严禁」坐标约束仍在 + ≤3 句仍在），沿 a8 载体齿「解析渲染/声明两面得版本再断言」形态、避开版本字面耦合。

**只能归真机/live 腿（host 钉不了）**：① 真实遮挡消除（层序渲染结果只有上屏可证，源锚齿只是决策代理）；② 中文释义真机上屏可读、wrap 不溢出；③ 提示条新位置可见且不被键帽条压；④ 播报与屏显一致（human 判据的最终裁判）。

## human finding③（本轮追加范围，2026-09-20）：播报不得夹带非口语内容

**human 原话**：「云端返回的内容 不要带 其他无关的，上一笔播报 就有无关的，你可以查一下日志，把这个也限制一下」。

**实证（引擎日志，14:45 轮）**：OpenClaw 回复夹带 `<details><summary>来源与自检</summary>…</details>` 附录（内容含「数据入口：`kidpal/chardata/char_cache.json` 命中勇…」「笔画名称来源（仅用于口语）：bishun.hao86.com、bishun.fjnews.com…」「另注：`勇_stroke_order.svg` 只是视觉参考，C1 契约里没有 svg 字段」）；日志 `语音生成成功: 撇。<details>…</details>` 逐字在案——**整块附录进入 TTS 被念出**。网关 `display-strip` 补丁只剥 ```display 围栏，未剥该 appendix。

**修法（云面双侧，与 prompt v5 同刀）**：
1. **网关 strip 硬化**（`make_openclaw_patch.py`）：除 ```display 围栏外，**也剥 `<details>…</details>`（及同类 HTML/markdown 附录块）**——防御性兜底，**流式与非流式两出口路径一致**。
2. **prompt v5 明确**：面向设备的回复**只含口语正文 + 一个 display 块**；**禁任何自检/来源/附录/明细/HTML 结构**（内部自检若要保留，走非面向设备的日志面，不得进回复正文）。
3. **验收齿（MCD-A7）**：含 `<details>` 附录的输入经两出口路径 → TTS 文本与下发内容**均不含附录**；prompt v5 锚齿含「禁附录」新约；板端侧不新增改动（剥离在云面完成）。

**风险**：剥离过宽可能误伤正文（如孩子问「为什么」时的正常括号内容）——齿须含「正常正文不被误剥」的正向 case。

## 铁律

- **零契约改动**（D3 `meaning`/`words` 字段既有，本轮向契约样例语义收敛）、**零 spec 改动**、零 agent 定义/gate 改动。
- **落法 B（display 块通用文本通道）= 另立契约门立项，本轮不预造、不顺手夹带**（workstream progress 已登记，触发通道 `/arch-change`）。
- 冻结件修订只走 acceptance-author 留痕；builder 零写权。
- `char_stroke.c/.h` 纯逻辑合约**不动**（U 冻结面 + B8 守卫）。