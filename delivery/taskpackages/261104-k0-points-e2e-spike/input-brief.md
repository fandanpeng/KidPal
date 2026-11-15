# input-brief · 261104-k0-points-e2e-spike

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-11-04T14:18:53.101Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261104-k0-points-e2e-spike/taskpackage.json` · subsystem: server points 最小写入与 front 推送 + OpenClaw 薄意图 + firmware points_home 最小消费 · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.3⑦` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§1.3⑧` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图或 front 检出后下发） | 231 | ok |
| `docs/contracts/interface-contract.md` | `§1.5` | 1.5 网关的职责（薄，零自研业务逻辑红线不变） | 250 | ok |
| `docs/contracts/interface-contract.md` | `§四-kidpal_points_page` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/261103-star-points-story-cover/review.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/261103-star-points-story-cover/task_plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261104-k0-points-e2e-spike/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/decisions/ADR.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/kidpal-ui-design-v9.html` | — | 无锚点（整文件） | — | bare |
| `docs/ui/UIUX-DESIGN-BRIEF.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/points-home.png` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `NFR-PERF-4` | NFR-PERF-4 · 记账反馈时延（本批新增） | 837 | ok |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.1-D-07` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.1-D-08` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.1-D-10` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/task_plan.md` | `K0` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/review.md` | `D-spike-first` | （未定位到标题节） | — | not-found |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/display_schema.py` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/view_router.c` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F6.3 · 源 = `docs/requirements.md` 节首行 :712 · 本包声明 AC 粒度回引：`S-F6.3-AC1`、`S-F6.3-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.3 · 设备语音记账与兑换（本批新增）

> 作为孩子，我希望直接跟小伴说「我今天扫地了，加5分」就能记账、攒够了说一声就能兑换，以便不等家长操作也能自己攒积分兑奖励。

**验收标准**
- AC1（记账句式门槛，human 2026-11-03 拍板）：Given 孩子对设备说话，When 语句是**明确完整的记账句式**——事由 + 动作词（加/扣）+ 分值（如「我今天扫地了，加5分」「我们今天去游乐场了，扣40分」），Then 触发记账；When 只提事件、不带分值动作（如随口说「我们去游乐场了」），Then **不触发**记账、走普通对话、不落任何账。
- AC2（加分）：Given 完整加分句，When 说出，Then 立即记账（加分任意金额 + 事由，自报即到账——设备不区分说话的是家长还是孩子），并自动跳转积分本页展示新余额。
- AC3（兑换资格与金额校验，human 拍板「只认目标分值」）：Given 语音兑换句，When 扣的分值 = 当前激活目标分值且余额 ≥ 该分值，Then 兑换成立（按 AC4 落账）；When 金额不匹配（如目标 40 说「扣20分」、目标已换 50 说「扣40分」），Then 温和拒答并说明当前目标要扣多少、不落账；When 余额不足激活目标分值，Then 不能兑换、温和说明还差多少。
- AC4（真实兑现后才扣）：Given 已具备兑换资格（余额 ≥ 激活目标分值 = 资格、**不自动扣分**），When 奖励实际兑现并经一次明确的兑换确认，Then 产生一条负向分录（分录形状同 S-F6.2 AC2 单行口径：兑换名目住原因位、分值 + 兑换前后余额 + 日期）；**绝不提前扣**——定好了明天去游乐场余额也不动，临时没去成什么都不用处理（从未扣分、余额保持原样，改天真去了再扣）。
- AC5（短窗去重，human 拍板）：Given 短时间内（起步 10 分钟、真机调优定数——验收经注入时间覆盖）分值相同且事由相近的重复记账句，When 再次说出，Then 不记第二条、友好回应「这条刚才记过啦」并跳积分本页；兑换侧「同一次兑换不重复扣分」同此。
- AC6（断网）：Given 断网，When 语音记账/兑换，Then 失败给可见安抚（「现在联系不上账本，等会儿再试」类，措辞归文案单源/设计稿），**不本地暂存补记**（回引 NFR-OFF-1）。

**线框**：无专属页——语音记账/兑换 = 会话态复用共享底座（WF-B-01/02/03）；记账成功跳转落点 = WF-F6-01。
**补充规则**：在积分本页（主页/详细账本）发起的会话带**积分语境**（浏览账本时问「我为什么得了3分」最顺手）——语境信号的技术载体与取值归架构/契约面，本 story 只钉意图。记账句式门槛以 human 2026-11-03 拍板为准（讨论前文「我们去完游乐场啦」类不完整句按门槛不落账，正负两类都进验收）。惩罚性扣分、语音删除、加分护栏（单笔分值上限）第一版不做（human 批示）——删除只走设备按键（S-F6.2）与 QQ（S-F6.4）。埋点 = 无。
**锚**：UI = 共享底座（语音轮三态） ｜ 支撑 = arch §5.1（front 组件，云）、§5.2（运行场景，云）
**可行性**：🟡（语音链路已有，薄意图→确定性账本→权威推送路径可行，句式门槛、短窗去重与真实兑现确认须 K0/真机验收）
```

### S-F6.4 · 源 = `docs/requirements.md` 节首行 :729 · 本包声明 AC 粒度回引：`S-F6.4-AC1`、`S-F6.4-AC5`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.4 · QQ 家长记账与更正（本批新增）

> 作为家长，我希望在 QQ 里就能给孩子加扣积分、查账、删除误记和兑换，以便随时掌握和更正孩子的账本。

**验收标准**
- AC1（记账同构）：Given 家长在 QQ 说（如「给小伴加 3 分，今天老师表扬了」），When 消息收到，Then 解析后记账——账面规则与设备语音完全同构（human 拍板「与语音同规则」）：加任意分 + 事由；兑换同 S-F6.3 规则（只认激活目标分值、不符回说明、真实兑现后才扣、同一次兑换不重复扣）。
- AC2（查账）：Given 家长询问当前余额、激活目标或最近记录，When 发问，Then 回答账本真实状态。
- AC3（删除）：Given 家长要求删除某条记录，When 定位明确（如「删掉刚才那条加 3 分」类稳定指认），Then 该分录删除、余额重算；When 定位含糊或指认不上，Then 用普通文字回复说明可用的定位方式、不进入追问卡死。
- AC4（一次性可判，硬约束）：Given 家长消息含糊或格式错误，When 无法唯一理解，Then 用普通文字回复说明正确说法——每条消息独立可执行或独立报错，**QQ 命令不依赖多轮澄清问答**（措辞面归实现）。
- AC5（推送不抢前台）：Given QQ 通道到账（加分/兑换/切目标/删除），When 设备正在其他页，Then 只更新数据、不抢前台；When 积分本页在屏，Then 原地刷新余额与进度线。

**线框**：无（QQ 侧无设备 UI；设备侧显示落点 = WF-F6-01 原地刷新）。
**补充规则**：通道 = 家长主通道 QQ（S-F1.5 先例同型）；惩罚性扣分的门对所有通道关闭；「一次性可判 + 普通文字错误回复」为硬设计约束（QQ 侧歧义命令宁可报错重来、不挂起等补充——多轮澄清不可依赖），具体回复格式归实现。埋点 = 无。
**锚**：UI = 无（QQ 侧无板端 UI） ｜ 支撑 = arch §5.1（服务端组件·家长通道，云）
**可行性**：🟡（QQ 与 front 在线推送先例可复用，须新 skill 共用单一 writer 并实证一次性报错和不抢前台）
```

### S-F6.2 · 源 = `docs/requirements.md` 节首行 :696 · 本包声明 AC 粒度回引：`S-F6.2-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.2 · 浏览详细账本与删除误记（本批新增）

> 作为孩子，我希望翻完整账本看每一笔加扣分的来龙去脉、记错了还能删掉那笔，以便账本始终诚实、余额算得清。

**验收标准**
- AC1：Given 在积分主页 KEY 双击（或说「打开详细账本」），When 进详细账本，Then 完整账本列表 = 时间倒序全量分录，形态与故事列表一致（分屏浏览、选中行带边框、「第 x 屏 · 共 y 屏」）；**全量可及**——任意历史分录都能翻到（分页形状由架构面钉，需求只锁「全量可及」）；页内按键与故事列表同型（§2.3）：BOOT 浏览/跳屏、BOOT 长按进删除确认、KEY 短按开语音会话（积分语境，带账本上下文）、KEY 长按回积分主页。
- AC2（分录单行呈现，human 2026-11-03 终拍裁定）：Given 浏览分录，When 看每条，Then **每条分录单行呈现 = 分值 + 变动前→变动后余额 + 原因 + 日期**（加分与兑换分录统一此形状，如「+3 18→21 主动洗碗 今天」「−40 42→2 兑换游乐场 11月3日」）；兑换分录的兑换名目在原因位承载、来龙去脉（换什么、动了多少、从多少到多少）单行可读；箭头字形与紧凑写法归设计稿/实现，需求钉「单行、变动前后余额紧跟分值、不再两行显示」。
- AC3：Given 选中某条分录按 BOOT 长按，When 删除确认弹出，Then 整屏覆盖弹窗（**复用 WF-F2-05 同族**：大字「删掉这条？」+ 待删分录，页点消失同覆盖层口径）：KEY 短按 = 确认删除、BOOT 任一手势 = 取消回账本、10 秒无操作静默取消；其余手势不响应。
- AC4（删除重算与负余额，OQ-1）：Given 确认删除某条分录，When 删除生效，Then 余额立即重算并刷新显示与进度线（如误加 50 分删掉后余额回到删除前的值）；**删除重算允许负余额**（如加 50 → 兑 40 → 删那笔 +50，余额 −40）——负余额只出现在删除重算路径；兑换动作不受影响、仍要求余额 ≥ 激活目标分值（S-F6.3 AC3）。
- AC5（断网）：Given 设备断网，When 尝试删除，Then 给可见失败提示、不本地假删——账本一切写入只走云端（回引 NFR-OFF-1）。

**线框**：WF-F6-02（详细账本列表，与 WF-F2-01 同型）+ WF-F2-05 同族复用声明（删除确认弹窗不另画新框）。
**补充规则**：数据口径 = 删除 = 云端账本移除该分录并重算（设备不承载账本写入，回引 NFR-OFF-1）；删除后其余分录保留不清零；删除生效后经云端刷新设备显示（余额、进度线、账本列表一并更新）。「全量可及」为需求语义，分页/窗口形状权威归架构与契约面（不照搬故事列表最近窗口的措辞）。埋点 = 无。
**锚**：UI = `points-ledger`（新增，随批 261103-star-points-story-cover 在途） ｜ 支撑 = arch §5.5.4（UI 演出，板）、§5.1（front 组件，云）
**可行性**：🟡（故事列表与删除确认可复用，完整账本须新增分页/删除协议及云端重算，不能沿用 16 条窗口）
```


## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

| 类别 | 路径 |
|---|---|
| designRefs | `docs/ui/V9/points-home.png` |
| sharedUiRefs | `docs/ui/kidpal-ui-design-v9.html` |
| sharedUiRefs | `docs/ui/UIUX-DESIGN-BRIEF.md` |

页级 PNG（400×300）= **布局坐标定位权威**：layout 常量按图定，文字线框仅口径补充。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/display/view_router.c` | `kidpal_display_route` | :33-74 | — |
| `server/gateway/front/front_app.py` | `_push_display_payload` | :584-593 | 642, 717, 805, 847, 1330, 1436 |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

（无同 subsystem 的已 merged 先例包）

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `package:261104-k0a-four-page-skeleton` | P3 静态占位接管缝与共享构建清单串行 | `261104-k0a-four-page-skeleton` | `261104-k0-points-e2e-spike` | K0a 负责留下接管缝并先落共享 host CMake；K0 并行只开发服务端与新增 points 文件，K0a merged 后重取基线追加 firmware/main/test/host_app/main/CMakeLists.txt，其余 K0a 文件不改。该 CMake 是两包唯一共享文件，不宣称零重叠。 |
| `workstream:261103-star-points-story-cover` | P3 静态占位到 K0 最小渲染的接管缝 | `261104-k0a-four-page-skeleton` | `261104-k0-points-e2e-spike` | K0a 提供接管缝并先落共享 host CMake，K0 并行开发服务端与新增 points 文件；K0a merged 后重取基线追加 firmware/main/test/host_app/main/CMakeLists.txt，再跑 host/device build 与真机终验。该 CMake 为两包唯一共享文件，不宣称零重叠。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

## ⑥ 非阻断 advisory 明细（终端只打汇总；此处保留定位）

- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§1.3⑦`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§1.3⑦（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§1.3⑧`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§1.3⑧（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§四-kidpal_points_page`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§四-kidpal_points_page（起包期核对锚点或目标文档是否已变更）
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/review.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/review.md
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/task_plan.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/task_plan.md
- **info · whole-document** · ref=`delivery/taskpackages/261104-k0-points-e2e-spike/plan.md`：Markdown 整文件引用（不自动猜章节）：delivery/taskpackages/261104-k0-points-e2e-spike/plan.md
- **info · whole-document** · ref=`docs/requirements.md`：Markdown 整文件引用（不自动猜章节）：docs/requirements.md
- **info · whole-document** · ref=`docs/architecture.md`：Markdown 整文件引用（不自动猜章节）：docs/architecture.md
- **info · whole-document** · ref=`docs/contracts/interface-contract.md`：Markdown 整文件引用（不自动猜章节）：docs/contracts/interface-contract.md
- **info · whole-document** · ref=`docs/decisions/ADR.md`：Markdown 整文件引用（不自动猜章节）：docs/decisions/ADR.md
- **warning · bare-non-markdown** · ref=`docs/ui/kidpal-ui-design-v9.html`：ref 无锚点，清单退化为整文件引用：docs/ui/kidpal-ui-design-v9.html（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **info · whole-document** · ref=`docs/ui/UIUX-DESIGN-BRIEF.md`：Markdown 整文件引用（不自动猜章节）：docs/ui/UIUX-DESIGN-BRIEF.md
- **warning · bare-non-markdown** · ref=`docs/ui/V9/points-home.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/points-home.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-07`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-07（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-08`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-08（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-10`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-10（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/task_plan.md#K0`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/task_plan.md#K0（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/review.md#D-spike-first`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/review.md#D-spike-first（起包期核对锚点或目标文档是否已变更）
- **warning · bare-non-markdown** · ref=`server/gateway/front/front_app.py`：ref 无锚点，清单退化为整文件引用：server/gateway/front/front_app.py（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`server/gateway/display_schema.py`：ref 无锚点，清单退化为整文件引用：server/gateway/display_schema.py（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **info · whole-document** · ref=`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`：Markdown 整文件引用（不自动猜章节）：server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md
- **warning · bare-non-markdown** · ref=`firmware/main/display/view_router.c`：ref 无锚点，清单退化为整文件引用：firmware/main/display/view_router.c（非 Markdown 路径不适用文档豁免，起包期核对引用形态）

