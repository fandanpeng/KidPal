# KidPal 术语表（glossary）——项目黑话的唯一权威定义

> 双 spec（[requirements.md](requirements.md) / [architecture.md](architecture.md)）与治理文档共用；一处定义、处处引用。新词先落这里再使用。
> 治理流程类术语（Track A/B、landing、ARQ/CDQ 等）的完整机制在 [DEV-GOVERNANCE.md](DEV-GOVERNANCE.md)，本表只收一句话定义。

## 产品与角色

- **小伴**：吉祥物猫咪角色，贯穿全局演出（首页常驻 + 各状态换姿态）。
- **星星记录本**：P3 页（原「故事页」）——孩子记下来的一切（故事/日记/碎碎念），每记一篇 = 集一颗星星。
- **星星**：原「故事果」资产；计数 = 现存篇数，盖章文案的 N = 入册流水号（只增不减）。
- **盖章**：录音保存成功瞬间的全屏定格演出（-8° 微倾、0.8 秒）。
- **记忆画像**：小伴对星星说过的事实（喜好/宠物/同学/经历）的结构化累积——跨会话回忆的依据，由云端记忆子系统自动维护；家长的隐私信息不进画像。定名承接 human 原话「事实卡」（261022-device-memory-prompt-overhaul）。
- **三段阶梯**：提醒三档时刻 = 前一天 20:30（带物）/ 当天 07:45（日程）/ 事前 15 分钟——全局常量，配在 OpenClaw 侧不进契约。
- **记录语境**：在 P3 按 KEY 发起的会话——预置引导语、无触发词；v1.65 起为**记录语境对话态**（对话式引导记录：AI 默认闭嘴、卡壳轻引导、收尾忠实整理）。
- **一刀切纯记录**：已废除口径（2026-09-14 裁定，2026-10-19 v1.65 随批 `261019-p3-record-dialogue-guide` 有意识废除）——原指该会话内说的全部累积归档、不做逐句意图切换（提问也记录）；现行语义 = 记录语境对话态，AI 引导语、孩子中途提问与 AI 回答均不进故事素材（S-F2.2）。
- **覆盖层**：盖在主页面之上的整屏内容（提醒弹窗/查字窗/语音轮演出/删除确认），层栈内置前。
- **BOOT 隔离**：覆盖层内 BOOT = 关掉顶层回原页、不翻页（v1.39 起；主屏 BOOT = 3 页循环翻页）。

## 架构与系统

- **哑终端**：设备端定位——只收音/放音/显示/按键，不理解业务、不存数据、不做智能。
- **方向 A / 方向 B**：端云通信两代形态——A = 同步 HTTP shim（四端点，已退役台阶）；B = WebSocket + Opus 流式网关（现行）。
- **端云四层链路（旧称「三腿拓扑」）**：设备端（ESP32 固件）→ 接入网关（front，唯一公网入口，TLS+鉴权权威）→ 引擎服务（小智官方容器 + engine-patch）→ 编排大脑（OpenClaw）。历史文档（决策 21、ADR-002/003）中的「设备腿 / 引擎腿 / 脑腿」即此四层的旧称谓——「腿」意指逐连接代理的一段链路。
- **脑算网推**：业务逻辑 100% 在 OpenClaw（脑）计算，front（网）只做到点投递。
- **fence 剥离**：从 LLM 流式回复中把 ```display 围栏块与播报正文实时分离的引擎部署副本状态机；闭块同步走侧通道，front 机械校验后经 mcp 下发。
- **E1 / display 块**：统一响应块 `{view, data, speech_hint?}`，7 view 白名单，契约面权威。
- **介质禁令**：板端非易失介质白名单制——仅凭证（url/token/version）+ 预栅格化资产与字体 + 固件本体；业务状态只驻 RAM。
- **overlay**：`apply-xiaozhi-overlay.sh` 以 provider.config.yaml 为源渲染引擎 `data/.config.yaml` 的部署渲染机制。
- **渲染面 == 声明面 == 被测面**：版本化制品接线不变式（ADR-001）——三处必须同指一版，守卫禁版本字面断言。
- **makemeahanzi**：开源汉字笔顺数据库（9574 字，SVG path + medians），笔顺数据源，契约 D3。
- **observation**：记忆库（Honcho）deriver 从对话中抽取的单条事实记录——记忆画像的原料；抽什么/不抽什么的边界权威 = requirements NFR-PRIV-1（261022-device-memory-prompt-overhaul）。
- **记忆子系统**：云端自托管的 Honcho 服务栈（ECS 本机五容器、端口全 loopback），承担消息 / 会话存储、observation 抽取（deriver）与画像表示（representation），在 prompt 构建前注入画像；写入范围与残余见 arch §6.7，语义权威 = requirements NFR-PRIV-1，出网记账 = NFR-SEC-2 ⑤（261022-device-memory-prompt-overhaul）。
- **平台内部系统回合**：由平台自身调度、孩子没有开口的后台回合（cron / 心跳 / 记忆巩固与叙事生成等）——内容不进记忆画像（类级边界）。与「设备流程回合」（故事整理 / 卡壳引导等，承载孩子原话、照常参与记忆）及「家长侧回合」（QQ 等，按 per-sender 归独立 peer、不污染 owner 画像）互斥不替代。完整定义与实例枚举 = arch §6.7（261025-spec-reconciliation-memory-writing-scope）。
- **持久会话**：引擎以 `user=<device_id>` 标识、只发本回合，OpenClaw 服务端按该标识维护的跨 UI 会话对话历史——对话历史的唯一权威（ADR-015）；引擎 RAM 内的副本非权威、不重放。
- **动态 context 块**：引擎每轮发往 OpenClaw 的系统侧小块（时间 / 日期、课表摘要等），位于缓存边界之下、不含静态纪律；静态语义在 workspace `AGENTS.md`（缓存边界之上，arch §5.5.5）。

## 治理与验收（一句话版）

- **Track A / Track B**：Track A = 方向收敛红队（所属面 loop 的 Track A 段，只看收敛度）；Track B = 架构+契约面既有七步闭环与轻路径，不指治理面常规后端。
- **direct-implementation**：治理面常规流程——轻设计与轻量 scope → reviewer 设计审并双向判档 → architect 直接实施 → reviewer 真实 diff 审 → 机械检查与合并；普通批不强制旧 landing 制品。
- **PROPOSAL / events.jsonl**：治理面可追加批记录与主线程实时事件流；landing-summary / landing-verdict / retro 仅旧批或条件场景，landing-verifier 为条件加强核验者。
- **debt 账本**：未结决策项账本（`docs/debt/`，260923-gov-review-arq-ssot-restructure 起，原名 `crq`、261014-gov-review-crq-debt-rename 批改名）——四域 `gov/` `tech/` `requirement/` `contract/`，一条目一文件（frontmatter 机读状态 + append-only 正文），每域 `INDEX.md` 为生成件；已结条目自动归位 `_archive/<结项年>/`。
- **ARQ**：治理机制域条目 ID 前缀（`ARQ-<YYMMDD>-<slug>`，落 `docs/debt/gov/`；原义 ARCH-REVIEW-QUEUE 单文件队列，已重构为 debt 账本，`docs/ARCH-REVIEW-QUEUE.md` 指路 stub 已于 261019 批删除）。**TECH-** / **REQ-** = 技术域 / 需求变更请求域的条目前缀（契约决策账已独立成 contract 域，见 CDQ 行）。
- **CDQ**：契约决策账（家 = `docs/debt/contract/` 域，一条目一文件，ID 前缀 `CDQ-`；原 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列 261019 批并入并删除。契约类条目在 tech 域以 `cdq:` 指针挂接不变，**门与批准状态以 contract 域条目为唯一权威**）。
- **ADR**：架构决策记录（`docs/decisions/ADR.md`，261019 批自 docs 根迁入；append-only 五件格式：日期/决策/理由/备选与放弃原因/关联）。
- **钉点 / needle**：冻结验收测试中逐字断言的锚点；文案改动必须随 needle 一并更新，否则验收假红。
- **齿**：验收断言（acceptance teeth）；host 面 = 本机可跑，live 面 = SSH 到 ECS 真环境（`RUN_LIVE=1` 显式放行）。
- **红基线**：动工前先确认目标断言当前为红，修绿才算证据。
- **MVP / Phase 1 / 二期**：已交付基线 / 下一版（requirements.md 即其需求基线）/ 其后。
