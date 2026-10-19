# KidPal 术语表（glossary）——项目黑话的唯一权威定义

> 双 spec（[requirements.md](requirements.md) / [architecture.md](architecture.md)）与治理文档共用；一处定义、处处引用。新词先落这里再使用。
> 治理流程类术语（Track A/B、landing、ARQ/CDQ 等）的完整机制在 [DEV-GOVERNANCE.md](DEV-GOVERNANCE.md)，本表只收一句话定义。

## 产品与角色

- **小伴**：吉祥物猫咪角色，贯穿全局演出（首页常驻 + 各状态换姿态）。
- **星星记录本**：P3 页（原「故事页」）——孩子记下来的一切（故事/日记/碎碎念），每记一篇 = 集一颗星星。
- **星星**：原「故事果」资产；计数 = 现存篇数，盖章文案的 N = 入册流水号（只增不减）。
- **盖章**：录音保存成功瞬间的全屏定格演出（-8° 微倾、0.8 秒）。
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

## 治理与验收（一句话版）

- **Track A / Track B**：变更评审两轨——A = 方向收敛红队（所属面 loop 的 Track A 段，入口 `/arch-change` / `/gov-change`；只看收敛度）；B = 常驻 Track B 七步闭环（轻路径 = 文档面瘦制品过闸）。
- **PROPOSAL / landing / landing-verifier / retro**：Track B 七步闭环的四件制品。
- **debt 账本**：未结决策项账本（`docs/debt/`，260923-gov-review-arq-ssot-restructure 起，原名 `crq`、261014-gov-review-crq-debt-rename 批改名）——四域 `gov/` `tech/` `requirement/` `contract/`，一条目一文件（frontmatter 机读状态 + append-only 正文），每域 `INDEX.md` 为生成件；已结条目自动归位 `_archive/<结项年>/`。
- **ARQ**：治理机制域条目 ID 前缀（`ARQ-<YYMMDD>-<slug>`，落 `docs/debt/gov/`；原义 ARCH-REVIEW-QUEUE 单文件队列，已重构为 debt 账本，`docs/ARCH-REVIEW-QUEUE.md` 指路 stub 已于 261019 批删除）。**TECH-** / **REQ-** = 技术域 / 需求变更请求域的条目前缀（契约决策账已独立成 contract 域，见 CDQ 行）。
- **CDQ**：契约决策账（家 = `docs/debt/contract/` 域，一条目一文件，ID 前缀 `CDQ-`；原 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列 261019 批并入并删除。契约类条目在 tech 域以 `cdq:` 指针挂接不变，**门与批准状态以 contract 域条目为唯一权威**）。
- **ADR**：架构决策记录（`docs/decisions/ADR.md`，261019 批自 docs 根迁入；append-only 五件格式：日期/决策/理由/备选与放弃原因/关联）。
- **钉点 / needle**：冻结验收测试中逐字断言的锚点；文案改动必须随 needle 一并更新，否则验收假红。
- **齿**：验收断言（acceptance teeth）；host 面 = 本机可跑，live 面 = SSH 到 ECS 真环境（`RUN_LIVE=1` 显式放行）。
- **红基线**：动工前先确认目标断言当前为红，修绿才算证据。
- **MVP / Phase 1 / 二期**：已交付基线 / 下一版（requirements.md 即其需求基线）/ 其后。
