# input-brief · 261025-kidpal-tools-profile-fix

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-25T10:32:12.743Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> regenerated-because: docRefs/contractRefs 补节锚（起包期 anchor 精化，减少整文件退化引用）（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）
> 来源 TaskPackage: `delivery/taskpackages/261025-kidpal-tools-profile-fix/taskpackage.json` · subsystem: server/deploy/261022-openclaw-kidpal-tools/（声明 change→changes 双叶键 tools.profile+tools.allow、drift_guard 升级基准换代、apply --gen-patch 双叶键、rollback 零改申报）+ server/deploy/261022-openclaw-honcho-plugin/（declare drift 现状锚四项→五项 + apply 头注换代）+ server/deploy/manifest.yaml（kidpal-tools 三条 + honcho-plugin 段陈旧字面换代）+ server/acceptance/test_261022_openclaw_kidpal_tools.acceptance.py（K1-E4 换代 + K1-E4-C 防回归锚）+ server/acceptance/test_261022_memory_isolation.acceptance.py（K3-E6 锚显式换代） · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `1.6` | 1.6 对 OpenClaw 的要求（system prompt 约束） | 189 | ok |
| `docs/requirements.md` | `S-F5.1` | S-F5.1 · 记住孩子说过的事 | 589 | ok |
| `delivery/taskpackages/261024-memory-recall-tool-enable/taskpackage.json` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/device-memory-overhaul/progress.md` | `2026-10-24 · 记忆回忆回合启用` | 2026-10-24 · 记忆回忆回合启用（261024-memory-recall-tool-enable）收口——final r2 全绿 + commit 冻结（integrator） | 264 | ok |
| `docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/device-memory-overhaul/progress.md` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F5.1 · 源 = `docs/requirements.md` 节首行 :589 · 本包声明 AC 粒度回引：`S-F5.1-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F5.1 · 记住孩子说过的事

> 作为孩子，我希望小伴记得我说过的事——我说过喜欢什么颜色、我的小狗叫什么、我同学叫什么——隔天再问它也答得上来，以便不用每次从头自我介绍，它像一个真正认识我的伙伴。

**验收标准**
- AC1（同会话回忆）：Given 同一会话里孩子说过「我最喜欢的颜色是紫色」，When 孩子追问「你还记得我最喜欢什么颜色吗」，Then 设备答出紫色——同一次说话里它听得进去、记得住。
- AC2（跨会话回忆）：Given 孩子此前的会话里讲过「我的小狗叫豆豆」，When 隔天孩子重新跟设备说话（新会话）问「我的小狗叫什么」，Then 设备仍答出豆豆——回忆不依赖同一次会话还在。
- AC3（隐私负向）：Given 孩子说出（或被诱导说出）家长姓名、联系电话、家庭安排等家长隐私信息，When 之后孩子再问「我爸爸叫什么」「我们家电话是多少」（含变着问法套问），Then 设备不记得、不复述——注入式探针 100% 不回忆（量化阈值与验证口径回引 NFR-PRIV-1）。
- AC4（记忆边界）：Given 孩子与小伴正常聊天，When 记忆形成，Then 记进画像的只是「孩子世界的事实」（喜好、宠物、同学、经历过的事）；家长隐私信息（姓名/联系方式/家庭安排）不属于记忆范围（AC3 的来源口径）。
- AC5（不入画像的回合 + 历史照记）：Given 设备自身流程触发的回合（不是孩子跟小伴真实聊天；完整枚举由架构文档定义——现知至少含：front 故事整理/卡壳引导回合、提醒到点语义回合），When 回合发生，Then 其内容不进记忆画像；Given 孩子以前聊过的历史内容（含同学姓名），When 补录进记忆，Then 照常记入——旧内容与新聊天一视同仁（孩子主动讲述的社交事实正是记忆目标）。

**线框**：无（记忆是语音与云端能力，无板端新页面/新状态——回忆经共享底座语音轮三态承接）。
**补充规则**：记忆口径 = 只记孩子世界的事实（AC4）；设备流程触发的回合不入画像、历史语料照记（含第三方姓名）见 AC5；保留 / 删除语义（存于哪、怎么删、不进日志导出与展示面）的权威口径 = NFR-PRIV-1（story 侧不重复定义）；出网路径记账 = NFR-SEC-2 ⑤。历史语料补录不区分回合性质、全量照记（含旧设备流程回合）；设备流程回合不入画像的边界自记忆子系统上线起对新增回合生效。分工边界：F2 星星记录是孩子**主动要设备记下来的内容产出**（讲完整理成篇归档、集星星），本 story 是小伴**自己记得孩子说过的事**（跨会话回忆能力），两者互不替代；回忆的回答承接 = 与通用问答同一问答框（共享底座播报态，同 S-F1.3/S-F4.1 口径）。断网时回忆不可用（回引 NFR-OFF-1，问答同款）。埋点 = 无。
**锚**：UI = 无（板端无 UI） ｜ 支撑 = arch §5.1（服务端组件，云）、§1.2（外部依赖清单）——记忆子系统落位随架构文档修订后对账
**可行性**：🟡（AC1/AC2 已实证——同会话与跨会话回忆已端到端探针验证；AC3/AC4 的抽取边界与 AC5 的机械回合隔离待架构面定形，随实现包验收）

---
```


## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 本包只改云端部署声明/脚本与验收锚：设备端零感知、固件零 diff；不新增或改变端→网关消息、帧类型、字段承载与序列化形态。

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

（无同 subsystem 的已 merged 先例包）

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

