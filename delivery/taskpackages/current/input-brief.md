# input-brief · 261022-m5-k3-honcho-memory-production

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-23T09:02:27.325Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261022-m5-k3-honcho-memory-production/taskpackage.json` · subsystem: server/deploy/honcho（Honcho 自托管五容器 + 生产 profile 插件/hooks 授权）+ server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（v12 守则）+ server/gateway/front（一次性回合隔离，若需） · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§四` | 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B） | 268 | ok |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/device-memory-overhaul/task_plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261022-m5-k3-honcho-memory-production/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261022-openclaw-prep-tax-memory-pressure.md` | — | 无锚点（整文件） | — | bare |
| `NFR-PRIV-1` | — | （文件不存在） | — | missing |
| `NFR-SEC-2` | — | （文件不存在） | — | missing |
| `TECH-261022-device-memory-prompt-overhaul` | — | （文件不存在） | — | missing |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | `§3.1` | 3.1 方案空间与裁定 | 53 | ok |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | `T1` | T1 · 外部依赖、部署与数据视图：Honcho 记忆子系统落位 | 67 | ok |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | `T4` | T4 · 记忆写入资格门与 AC5 完整枚举（含一次性回合隔离机制裁定） | 282 | ok |
| `server/deploy/apply-xiaozhi-overlay.sh` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F5.1 · 源 = `docs/requirements.md` 节首行 :589 · 本包声明 AC 粒度回引：`S-F5.1-AC2`、`S-F5.1-AC3`、`S-F5.1-AC4`、`S-F5.1-AC5`

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

> 本包纯云端记忆子系统与 agent 守则面改动：设备端零感知、固件零 diff；不新增或改变任何端→网关消息、帧类型、字段与序列化形态（display E1 / B1 / §三提醒 / §四私有消息帧零触碰）。front 一次性回合隔离只改云端发起侧 payload（不携 user），设备无感。

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

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
| `workstream:device-memory-overhaul` | 记忆 peer 映射（device 级 peer，键 agent:kidpal:openai-user:<device_id>） | `261022-m5-k3-honcho-memory-production` | `261022-m5-k3-honcho-memory-production` | peer 语义 owner = P3；键形态源自 P2 会话键，P4 backfill 消费同一映射；多设备身份体系不做（OQ-7 留根）。 |
| `workstream:device-memory-overhaul` | openclaw.json 声明式 patch 组合（tools allowlist + Honcho 插件/hooks） | `261022-m5-k1-prompt-carrier-migration` | `261022-m5-k3-honcho-memory-production` | P1 先落 tools 声明；P3 在 P1 已应用态上追加插件/hooks 声明；两包均幂等 apply + drift_guard + rollback，openclaw.json 整文件不入仓。 |
| `workstream:device-memory-overhaul` | workspace AGENTS.md 静态语义内容（v11 迁移段 + v12 守则段） | `261022-m5-k1-prompt-carrier-migration` | `261022-m5-k3-honcho-memory-production` | P1 迁入 v11 静态语义并保持单一事实源自声明；P3 只追加 v12 记忆守则，不回改 P1 段；两包串行（P1 先）。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

