# docs-delta —— 261013-lang-config-thinking-fix

> blocking closeout 制品。integrator 落笔（2026-10-14 制品轮）。本包 = Lite 单包（rigor=lite、sourceType=regression），承接 TECH-261013-lang-config-thinking-key-regen-landmine（固件 lang_config.h 生成物与 locale 源不一致——Lang::Strings::THINKING 键缺，干净构建必编译红）。docsSyncRequired=true、docsOwner=integrator（taskpackage `docsSyncReason` 在案）。

## 文档面改动清单（本批全部落点）

| 面 | 文件 | 变更 | 性质 / 授权 |
|---|---|---|---|
| crq/tech | `docs/crq/tech/TECH-261013-lang-config-thinking-key-regen-landmine.md`（status closed 后由 `crq-render.mjs` 自动归档归位 `docs/crq/tech/_archive/2026/`） | frontmatter 状态字段回写：`status: deferred` → `closed`、`closed: 2026-10-14`、`closedBy: 261013-lang-config-thinking-fix（dev-loop integrator closeout，修复随包落地）`；正文 append「修复落地 / 收口（2026-10-14 …）」节——T1+T2 正形落地逐条（LG-A1..A9 映射）+ 建议②取构建期必生成路线 + 建议③裁撤（T2 正形后自检对象不存在）+ 变异取证 6 实跑 + 3 同源承载 + verifier scoped r3 八闸全绿 + 真机轮 deferred 归 Phase-1 语音轮 + 连带第 6 条划出另开轻量 CRQ | taskpackage `docsSyncRequired=true` + exitCriteria⑥ 承载；条目正文 append-only，既有正文（frontmatter 非状态字段 + 证据/影响面/建议/风险四节）零改动 |
| crq/tech | `docs/crq/tech/TECH-261014-sdkconfig-decorative-empty-section.md` | **新建**轻量 CRQ 条目（NIT/deferred）——TECH-261013 连带第 6 条（firmware/sdkconfig 每构建被追加「Image Effects Configuration」装饰性空节、跟踪文件常驻脏）划出单独跟踪；证据 = 本收口 worktree 实测 5 行空节 diff | TECH-261013 收口段「另开轻量 CRQ」承载（integrator 对 docs/crq/ 有 scope-map 授写槽）；一条目一文件 |
| crq 索引 | `docs/crq/tech/INDEX.md` + `docs/crq/tech/_archive/2026/INDEX.md` | `crq-render.mjs` 再生：tech 域未结 19 → 19（landmine 收口 −1、连带残留新立 +1）、已结 16 → 17；`_archive/2026/INDEX.md` 增薄行（closed 2026-10-14 / closedBy 锚本包） | 生成件，脚本再生、未手改（repo-structure 纪律） |

## 零改申报

- 契约面 `docs/contracts/**`：零触碰——locale JSON 与 lang_config.h 均为板端私有资产/生成物，无 wire/契约面承载（taskpackage `crossEnd.exempt` 在案；contract-lint 闸 r3 PASS）；无 CCP / 契约队列面。
- spec 面 `docs/requirements.md` / `docs/architecture.md`：零触碰——本包为编译层/生成链修复，不新增任何需求语义或架构面叙述；plan 逃生门结论「不触契约/spec/agent 面」在案；无漂移检出、无需 architect 介入。
- ADR：零新增（taskpackage `adrRequired=false`；生成头落 build 目录 + 全依赖化 = 生成链工程正形，非契约/跨系统边界裁决，不构成 ADR 事由）。
- `.claude/**`、`server/**`、`tools/**`：零触碰。
- 实现面落点 = changeManifest 六文件逐一对账（两 locale json + main/CMakeLists.txt + scripts/gen_lang.py + firmware/.gitignore + 验收件 lang-config-gen.acceptance.md），`git status` 对账无 manifest 外源文件；`firmware/sdkconfig` M 态 = 构建期装饰性空节回写（连带第 6 条，范围外、不进本包 commit、已立 TECH-261014）。

## 代码面 doc 钉行（非本件义务、随包在案备查）

- `firmware/main/CMakeLists.txt` 两处注释 = 生成头迁 build 目录的动机注释（消灭 untracked 手补生成物形态 + 排序接线单机制说明），与 TECH-261013 收口段口径一致。
- `firmware/scripts/gen_lang.py` `--assets-dir` help 文本 = 向后兼容语义自述（未传参时旧反推逻辑保留）。

## Post-cutoff 制品纪律确认

本批 post-EVENTS_SCHEMA_CUTOFF（>= 250725）：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl` 晋升；复盘叙事归 planner `pod-retro.md`（收尾相位）。`delivery/taskpackages/261013-lang-config-thinking-fix/verify-runs.jsonl` 在场（verify.mjs dual-write 程序 fs-write）；`pod-events.jsonl` 在场（advisory 检查归 planner/supervisor、non-blocking）。
