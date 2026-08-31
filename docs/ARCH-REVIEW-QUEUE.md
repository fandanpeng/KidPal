# ARCH-REVIEW-QUEUE · 架构 review 未结项队列

> 自 AInativecompany 迁移的净版队列（2026-08-27）。原仓历史条目不入本仓——原仓全量见 `c:\AI\AInativecompany\docs\ARCH-REVIEW-QUEUE.md`。
> 性质：工作队列（收件箱），只跟踪 `/review-arch`、`/review-governance`、`/arch-change` 产出的未结 spec / governance soundness 项：open findings、待拍板 deadlocks、follow-up、accepted-as-risk watchlist。已结项移入 `docs/archive/ARCH-REVIEW-QUEUE.closed.md`。
> 详细裁决、证据、debate log 归 `changes/arch/**/review-packet.md`、`decision-packet.md`；本文件只保留当前未结项。

## 0. AI 维护规则

| 规则 | 写法 |
|---|---|
| 新增 ARQ | 只有 unresolved / future-trigger / accepted-risk 仍需可见的项才进入 `1. 当前工作队列`。 |
| ARQ ID 格式 | 新条目用 `ARQ-<YYMMDD>-<english-kebab-slug>`：YYMMDD=创建日；slug=短英文 kebab（ASCII `[a-z0-9-]`、≤~5 词、不收尾 `-`）；人读中文标题留 title 列。子项引用 `ARQ-<YYMMDD>-<slug>-<子项>`。 |
| 已结 ARQ | human 批准且 landing 完成后即已结：从当前队列移除，在 `2. 已结索引` 加一行（ID / title / evidence 链接）。 |
| 计数口径 | 不手写计数：header 不带「（X 项）」后缀。 |
| 状态轨迹 | 治理落地状态不在本文件——由 `node .claude/scripts/changes-index.mjs --upsert <id>` 单行钉进 changes/INDEX.md arch 段。 |
| 写法约定 | 队列行散文写通顺完整的句子；内部标识符首次出现带一句实质说明；表格列结构与证据引用保持精确（见 AGENTS.md「输出标准」节）。 |

---

## 1. 当前工作队列

### 1.1 Blocker / 待拍板

| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |
|---|---|---|---|---|---|

### 1.2 Deferred / forcing-function

| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |
|---|---|---|---|---|---|
| ARQ-260827-tier1-boot-review | SHOULD | KidPal spec v0.1 + interface-contract 走首次对抗 review（spec 冻结前置门） | 项目现处 spec 草稿期，`docs/spec.md` 与 `docs/contracts/interface-contract.md` 均未经对抗 review；冻结前须过一轮（档 1 主用途即此）。review 时顺带裁定三个契约草案标记（E1 display 块、B1 课表 MD）是否达冻结级。 | human 说开始审 spec 时（建议入口 /arch-change 或 /review-arch） | 治理迁移（2026-08-27）首个登记项 |
| ARQ-260831-p4-stroke-dasharray-visual-check | COULD | P4 笔顺「未写笔画的虚线点链」dasharray 观感与契约 D3 canvas 折线渲染需视觉对齐核验 | 设计稿用 dasharray 描边实现虚线点链、允许「自然淡网格」观感；契约 D3 定的是 canvas 折线逼近贝塞尔，未定虚线渲染细则。两者之间需一次目测对齐；若确认需要契约字段，单独走契约门（不预批）。 | P4 dev-loop 包验收阶段 | 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3） |
| ARQ-260901-shim-api-contract-gap | SHOULD | shim `/v1/*` 请求/响应字段信封 schema 未入契约 §九 | TP-A 落地了 shim（POST /v1/chat `{text}`→`{audio,display}`；/v1/asr `{audio}`→`{text}`；/v1/tts `{text}`→`{audio}`；/v1/alerts 出队），但 `interface-contract.md` §九 只含 E1/B1/§三 alerts 内容、缺 shim 字段信封 schema，且 §九 原本就标注 shim API request/response 为开放问题。字段信封需走 architect-reviewer 四分类 + 人类契约门正式定稿；本验证批按冻结决策用「环境变量可覆盖 + 注释」承接，未直改契约。 | 契约门（architect-reviewer + human）定稿 shim schema 时 | 260831-cloud-link-bringup（dev-loop 收口 review-packet finding #2） |

### 1.3 Accepted-risk watchlist

| ID | 严重度 | title | 保留原因 | 复议触发 | 来源 |
|---|---|---|---|---|---|
| ARQ-260827-contract-mech-gate-gap | SHOULD | pass-oracle 第 4 关（spec↔contract 机械门）暂无脚本兜底 | 原仓 `arch-spec-contract-check.mjs` 深绑多文件 Prisma contracts 结构，不适用单文件 MD 契约面，未迁。pass-oracle 第 4 关在本仓降级为人工核对，须如实记录「机械门缺失」，不得虚报已机检。将来契约面复杂化（出现第二份契约文件或字段级 drift 高发）时写一个针对 markdown 契约表的一致性检查脚本补位。 | 契约面扩到 ≥2 文件 / 或出现首次 spec↔contract 漂移实例 | 治理迁移决策记录（2026-08-27，MIGRATION-NOTES §已知缺口） |
| ARQ-260831-simulator-lvgl-version-drift | SHOULD | 模拟器↔固件 LVGL 版本错位是常驻风险（本次 9.5→9.3 已对齐，构建期校验待自动化） | TP-C（260831-simulator-lvgl-align，2026-09-01）已把模拟器 LVGL 降到 9.3.x 与固件同版（`lv_version.h` 三号一致 + MSBuild exit 0 编译通过），本次 9.5 vs 9.3 错位已消除。残余 = 「版本对齐的构建期校验」仍未做成自动门：模拟器↔固件 LVGL 版本同步眼下靠人工记得，固件每次升 LVGL 组件锁仍可能无声再漂。原风险实证保留备查（详见 ARQ-260831-i1-indexed-asset-on-device）：9.5 软件渲染的 I1 索引图像路径绕过调色板——「模拟器结论不直接当真机结论」纪律仍须双端同资产验证。建议下次 simulator 相关包落地时，把「`lv_version.h` 三号 == 固件 dependencies.lock」做成构建期断言。 | 固件 LVGL 组件锁变更 / 下次 simulator 相关 dev-loop 包立项（补构建期版本校验） | 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3 + §8.2-V1） |
| ARQ-260831-simulator-truecolor-vs-1bpp | COULD | 模拟器（真彩灰度）与真机（1bpp 阈值化）的显色差异是第二类失真 | 同一张 UI 在模拟器上细线/小字可读（FreeType 灰度抗锯齿），阈值化后可能糊掉（实测设计稿 12–16px 文字阈值化后明显糊边）。建议 simulator 包顺手做「画布快照阈值化 PNG」一键自查，验收图一律以阈值化版本为准；真机字体子集化时 16px 以下 CJK 的字号下限需实测。 | simulator 相关 dev-loop 包立项 / 真机字体子集化任务 | 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3 + §8.2-V6） |
| ARQ-260831-i1-indexed-asset-on-device | SHOULD | I1 索引格式在真机 LVGL 9.3 + SPIFFS 文件源路径下的可用性未验证 | 实战证实 9.5 软件渲染直渲路径下 I1 调色板被绕过（调色板仅 bin-decoder 对文件源走 `decode_indexed_line` 生效），故资产管线基准直出 RGB565。但 RGB565 占 Flash 是 I1 的 16 倍（P1 一套已约 96KB，大时间字块独占 55KB）；若 I1 在真机 9.3 + 文件源路径可用，可回收约 16 倍空间。功能支撑验证批 human 已裁本批不烧真机，此项留待真机联调期验证；若 TP-B（260831-ui-art-assets）的 B8 资产总量预算核算判定必须上 I1，则此项优先级升 P0。 | 真机联调期 / TP-B B8 判定必须上 I1 时 | 功能支撑验证批（2026-08-31，PROPOSAL §2 真机留根项）+ ui-implementation-strategy §6 |
| ARQ-260831-small-cjk-1bpp-readability | SHOULD | 16px 以下 CJK 文字在 1bpp 真机的可读性下限未实测，直接锁死 V3 设计稿字号下限 | 实战（V6）发现设计稿 12–16px 细字经阈值化后明显糊边——模拟器（FreeType 灰度抗锯齿）上可读的细字，真机 1bpp 未必成立。V3 设计稿（实现导向）能用多密的排版、多小的字，取决于本项实测出的字号下限。功能支撑验证批 human 已裁本批只模拟器+云端、不烧真机，故留真机字体子集化时实测。 | 真机字体子集化任务 / V3 设计稿字号定稿前 | 功能支撑验证批（2026-08-31，PROPOSAL §2 真机留根项）+ ui-implementation-strategy §6 |
| ARQ-260901-closeout-check-delivery-status-gap | SHOULD | closeout-check.mjs 依赖 delivery-status.mjs 缺失，机械 closeout 闸无法执行 | `.claude/scripts/closeout-check.mjs` 第 35 行 `import { checkStatus } from './delivery-status.mjs'`，但该脚本未从源仓迁入（`delivery-status.mjs` 不存在、`.delivery/` 目录亦不存在）。首次 dev-loop 收口（260831-cloud-link-bringup）实测触发崩溃，closeout 索引投影（`changes/INDEX.md --update-index`）无法跑；integrator 用 `taskpackage-required-check.mjs` 直接跑承重闸兜底转绿，但 closeout 机械门本身仍缺。迁移补 `delivery-status.mjs` + `.delivery/` 骨架即恢复。 | 下次 dev-loop 包收口（closeout-check 跑不了时）/ 迁移 delivery-status.mjs 时 | 260831-cloud-link-bringup integrator 收口（2026-09-01） |
| ARQ-260901-openclaw-chatcompletions-public | SHOULD | OpenClaw chatCompletions 端点为 full operator access 且随网关公网可达，仅 Bearer 保护 | TP-A 开启 OpenAI 兼容端点后实测：`POST /v1/chat/completions` = 完整 operator 权限，随 gateway bind=lan 公网可达，带 token 即可执行任意 agent 指令（无 token → 401，Bearer 已生效）。验证批接受此风险（shim 侧 OpenClaw token 仅经 loopback 转发、secrets.env 权限 600）；生产上线前须锁 127.0.0.1/防火墙拦 HTTP 路径，或 token 降权 + 设备弱权限 token 强制开启。 | 生产上线前安全加固 / 任何非验证用途对外暴露时 | 260831-cloud-link-bringup（dev-loop 收口 review-packet finding #1） |

---

## 2. 已结索引

> 已结项按统一格式追加在此（ID / title / evidence），证据链接指向 `changes/arch/**`。

| ID | title | 已结日期 | evidence |
|---|---|---|---|
| ARQ-260831-cross-surface-check-missing | landing-fidelity-check 的 cross-surface 子门子脚本未随迁出 | 2026-08-31 | human 指示直修（项目初期不走 review）：子门裁剪为「脚本在场才跑」，`landing-fidelity-check.mjs` GAR-001 段已加 existsSync 守卫与实况注记；若将来 dev-loop 面（route-fixtures / dev-loop.settings.json）迁齐，从原仓补迁 `cross-surface-check.mjs` 即自动恢复启用。 |
| ARQ-260831-docs-top-level-registry-gap | docs 平铺后 scope-map docsTopLevel 白名单未系统性对账，repo-structure.md 骨架缺失 | 2026-08-31 | human 指示直修：scope-map `allowDirs` 按 docs 实况对账（去掉不存在的 `docs/systems`，实况六面齐备）；新建 `.claude/rules/repo-structure.md` 骨架（结构单一事实源，docs 根白名单与 scope-map 双向锚定）。 |
