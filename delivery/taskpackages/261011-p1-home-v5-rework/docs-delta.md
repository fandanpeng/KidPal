# docs-delta —— 261011-p1-home-v5-rework

> blocking closeout 制品。integrator 落笔（2026-10-12 制品轮）。契约权威原文 = 包内 `CONTRACT-CHANGE-PACKET.md` §3；gate 收口回填 = 同目录 `architecture-gate-evidence.md` §11。

## 文档面改动清单（本批全部落点）

| 面 | 文件 | 变更 | 性质 / 授权 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` §1.3⑥ | 四触点（preview 单对象→多项数组 + 三态语义 / 示例值「明早」→HH:MM 数组顺修 / :155 seam 引用签名 5 参同形顺修 / 变更记录行 append） | 契约门通道（CCP §3 逐字 git apply，HUMAN_APPROVED 2026-10-11，先批后落随包同 PR）；C1 人工逐字核对留痕 = gate evidence §11（净内容 9/9 行逐字一致 + 2 处呈现性差异如实登记；CRLF 保全实证 = 工作副本 279 行全 CRLF、blob 归一存储、无整文件翻转） |
| 契约队列 | `docs/CONTRACT-DECISION-QUEUE.md` | `CDQ-261011-p1-home-v5-rework` LANDED 行 append | gate evidence §6 C3 义务。**落盘通道 pending-planner**：integrator Edit 被 enforce-scope 钩子拦截（scope-map integrator allow 面不含该文件，与派单清单冲突，机械闸优先未绕行）；行全文已交 planner 代落（见 review-packet「交 planner 代办」节） |
| ADR | `docs/ADR.md` | 追加 **ADR-009**（`data.upcoming.preview` 形状重定义为多项数组并钉三态语义——日期/决策/理由/备选与放弃原因/关联五节；备选五项自 CCP §2） | taskpackage `adrRequired=true`（CCP 收口义务④，durable semantics 变化）；append-only，不覆盖 ADR-001..008 |
| crq/tech | `docs/crq/tech/TECH-261011-spec-v5-reconcile-note-drift.md` | 新建条目（requirements v1.61 补充规则内 v1.57 ⚠️ V5 对账注与实现漂移——spine openQ q4 / gate evidence §9 Follow-up 承载，deferred，档 0 勘误批候选；frontmatter 带 `cdq: CDQ-261011-p1-home-v5-rework` 指针） | gate evidence §6 C3 义务（记 tech 侧条目 + cdq 指针） |
| crq/tech | `docs/crq/tech/ARQ-260907-contract-1-5-strip-wording.md` | append trigger 触发留痕（本批契约门触发其「下次契约门」trigger，但 §1.5 校准不在本批 CCP 批准范围，维持 deferred、顺延下次含 §1.5 触点的契约门） | append-only 记账；如实登记不扩批 |
| crq 索引 | `docs/crq/tech/INDEX.md` | `crq-render.mjs` 再生（新条目 TECH-261011-spec-v5-reconcile-note-drift 入列 :36） | 生成件，脚本再生、未手改 |
| gate evidence | `delivery/taskpackages/261011-p1-home-v5-rework/architecture-gate-evidence.md` | append §11（C1 逐字核对留痕 + C2 在场实证 + C3 收口状态 + gate 改判 CLOSED（LANDED）） | 十节骨架 closeout 回填义务 |
| spec 面·需求 | `docs/requirements.md` | **零触碰**（v1.61 已 frozen 定稿 2026-10-11，§9 v1.61 行 = 定稿载体 commit 7f68b51；本批 = 兑现 S-F1.1 AC2/AC5/AC6/AC7 + 补充规则 6c，无新增语义） | 零改申报；`git diff` 全生命周期零命中（integrator 实证） |
| spec 面·架构 | `docs/architecture.md` | **零触碰**（arch §5.2 场景① / §10 决策 26 为输入锚；零 preview 形状镜像句，triage 取证在案） | 零改申报；`git diff` 零命中 |
| 设计稿母版 | `docs/ui/kidpal-ui-design-v5.html` | 三处内容级同步（v5 第三轮修订记录追加 ⑨ 口径改判 + ⑩ 空态话术占位 / 明天区示例行「08:00 上学」置首 / rail 规格 ②③ 空态话术与全量口径改判 + 封面 cover-sub 活文本归一） | **human 直落**（pod-events `b7e2d94a` 裁 B 案主线程直落 2026-10-12——升级事件 fe6c3f46 三选一：uiux-author 不在可派集、builder 无 docs/ui 写权）；Pod 侧核对 = spec-anchor-check 复跑 ok=true（designIdCount=60 不变、errors 零，2026-10-12 integrator 实跑） |

## 代码面 doc 钉行（非本件义务、随包在案备查）

- `firmware/main/display/home_upcoming.h:14/:34` + `home_upcoming.c:410/:426`：旧单对象过渡容忍注记（C2 done，builder 落、integrator grep 取证）。
- `server/gateway/schedule.py`：producer 多项化 + 明日无项发 `[]` 不发 None（plan R1）。

## 零改申报汇总

- `docs/requirements.md` / `docs/architecture.md`：零触碰（见上表）。
- `docs/contracts/**` 其余文件（interface-contract.md 之外）：零触碰。
- `.claude/**`：零触碰。
- 核证面（front_app.py / schedule_push.py / display_schema.py / mcp_server.cc / 其余四件 server acceptance / 归档件）：零 diff（exitCriteria③；detect_changes 中 front_app 三推送点与 TECH-260928 齿为 blast radius 命中 = 既有在役消费方被影响、非被改动，两面全绿实证零回归）。

## Post-cutoff 制品纪律确认

本批 post-EVENTS_SCHEMA_CUTOFF（>= 250725）：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner `pod-retro.md`（收尾相位）。`delivery/taskpackages/261011-p1-home-v5-rework/pod-events.jsonl` 在场（advisory 检查通过，non-blocking）。
