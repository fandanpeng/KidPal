# docs-delta — 260829-session-logic（包 3：会话状态机 + KEY/BOOT 全局按键手势解析）

> 性质：实现批（firmware 子系统，milestone-affecting，mergeOrder 6，riskTier high，testHost=host）。
> 核心结论：**无需改 canonical docs——本包纯实现落地，行为语义在 `docs/spec.md` §4.0/§5.3 未变**。
> docs-sync 闸（r4.json gate docs-sync）已 PASS；本 docs-delta 只记「无需改 + 理由」，不为写而写。

## A. docRefs 现状与无需更新理由

| docRef / contractRef | 现状 | 本包影响 | 无需更新理由 |
|---|---|---|---|
| `.planning/kidpal-mvp/PROPOSAL.md` | FROZEN v0.4（重切 15 包），本包 `origin.upstreamRef`，pin `sha1:88a5bdb83abdd867` | 只读（origin ref） | 受 `upstreamFrozen` sha1 pin 约束禁改（防 UPSTREAM_DRIFT）；本包实现与 PROPOSAL §2 包 3 定义逐条对齐（含 SES-E2 跨页端到端显式留根给包 9b） |
| `docs/spec.md#§4.0` | v0.2 FROZEN：全局按键手势表（KEY 主司会话、BOOT 主司翻页、PWR 硬件键不入状态机）+ 会话生命周期共享底座 | 只读章节锚点引用 | 本包 `session_state_machine.*` / `key_gesture_parser.*` 承载 §4.0 已定稿的手势语义（单击/双击/长按消抖、KEY 启动/结束/打断/回首页、BOOT 循环翻页），是把既有 spec 语义实现出来，零 spec 文本变更 |
| `docs/spec.md#§5.3` | v0.2 FROZEN：会话交互语义（按一下 KEY 启动、免提连续对话、60s 超时/说拜拜/再按键结束、思考超 4s 安抚） | 只读章节锚点引用 | 本包状态机转移表（LISTENING→THINKING→SPEAKING→ENDED、超时/结束意图/打断收束、4s 安抚 COMFORT 标记）按 §5.2/§5.3 已冻结行为实现，零 spec 文本变更 |
| `docs/contracts/interface-contract.md` | 契约面（笔顺 JSON / display 响应块 / 课表 MD / shim API） | 零触点 | 本包纯固件设备侧纯逻辑（按键事件入→状态转移出 + 时间戳手势解析），不涉及端云协议、不上云字段；taskpackage `contractRefs` 判定=纯消费（intake notes ③），契约面零触碰 |

**无需更新总述**：本包没有改变任何 canonical 语义。§4.0 手势表与 §5.2/§5.3 会话语义在上游 spec v0.2 冻结时已定稿（「小智默认 BOOT 做 ToggleChatState，我们改为 KEY 主司对话、BOOT 主司翻页」的改向也早已钉在 §5.3 注记），本包是把它们实现 + host 单测实证出来，不反过来改文档。唯一新增的可复用约定（host seam 头路径、HOST_ACCEPTANCE_SRCS 注册方式）已由包 0（260829-host-test-infra）的 `HOST_TEST_CONVENTIONS.md` 承载，本包是该约定的第二个消费实例，无新约定产生。

## B. 本包落地（非 canonical 文档）

| 目标 | 位置 | 说明 |
|---|---|---|
| `firmware/main/session_state_machine.{h,cc}` | firmware/ | 会话状态机纯逻辑（6 态转移表 + 非法转移拒绝 + ABORT/COMFORT side-effect 标记 + 包 8 MANUAL_RECORDING 接合面） |
| `firmware/main/key_gesture_parser.{h,cc}` | firmware/ | KEY/BOOT 手势解析纯逻辑（单击/双击/长按时间戳消抖 + `key_page_next` 四页循环 + PWR 拒收） |
| `firmware/main/CMakeLists.txt` | firmware/ | 两个 .cc 注册进主构建（SOURCES 追加 2 行） |
| `firmware/main/test/host_app/main/CMakeLists.txt` | firmware/ | `HOST_ACCEPTANCE_SRCS` 注册冻结验收文件 + 被测实现（builder 首动作，接缝约定照包 0） |
| `firmware/main/test/session_logic.acceptance.c` | firmware/ | 冻结验收（9 TEST_CASE，SES-A1~A6 + 包 8 接合面 + 尾注统一红基线逐条映射），builder/fixer 不可改 |
| `changes/dev/260829-session-logic/*` | changes/dev/ | closeout/audit 制品：本 docs-delta + review-packet（post-cutoff，制品从 taskpackage 解析，不晋升 taskpackage 本体） |

## C. 无漂移确认

- `docs/contracts/interface-contract.md`：零写（只读引用，本次 closeout 无契约暴露面变化）。
- `docs/spec.md`：零写（§4.0/§5.3 章节锚点只读引用）。
- 无新增 `docs/` 根白名单外文件、无影子目录。
- `changes/arch/**` 零改（本包无架构裁决需 ADR 归档——状态机取「纯逻辑 + 显式事件注入、无 FreeRTOS/定时器依赖」的 host 可测设计，属包 0 既有约定的延续，非新架构裁决）。
