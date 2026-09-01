# docs-delta — 260829-smoke-e2e（架构可行性冒烟，tracer bullet 包 S）

> 性质：tracer bullet 冒烟包（subsystem=firmware+server），只证「设备↔shim↔OpenClaw」三层链路串通，零业务。
> 本文件记录 docs-sync 的 **dist（哪些已改）** 与 **处置（哪些由 planner/主线程/契约门承接，本包不动手）**。
> 边界注：integrator 的 scope-map 写面 = `firmware/** server/** docs/systems/** docs/templates/** docs/ARCH-REVIEW-QUEUE.md changes/dev/** .claude/taskpackages/** .planning/**`——`docs/notes/**`、`docs/spec.md`、`docs/contracts/**` 不在写面，故 ADR 落 `changes/dev/`（见 D 段），需迁移 `docs/notes/` 的项留给主线程。

## A. 已改（本包落地）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| 设备侧 HTTP shim 协议 | `firmware/main/protocols/shim_protocol.{cc,h}`（新增） | 最小同步 HTTP 请求/响应闭环（/v1/asr→/v1/chat→mp3 出声） | SMK-A1/A3 设备腿 |
| mp3 解码路径 | `firmware/main/audio/mp3_decoder.{cc,h}`（新增）+ `audio/audio_service.{cc,h}` 加 `PlayMp3` | Helix 定点解码（esp-libhelix-mp3） | SMK-A3 下行 TTS 直通 |
| 配网承载 | `firmware/main/Kconfig.projbuild`（新增 4 个 Kconfig 项） | `USE_SHIM_PROTOCOL` / `SHIM_BASE` / `SHIM_DEVICE_TOKEN` / `SHIM_CAPTURE_MS` | SMK-A2 |
| 协议分支 + 激活门旁路 | `firmware/main/application.cc` | `#if CONFIG_USE_SHIM_PROTOCOL` 分支 + `ActivationTask` 旁路 `CheckNewVersion()` | SMK-A2− / 启动序 |
| 构建挂接 | `firmware/main/CMakeLists.txt` + `idf_component.yml` | 加 `shim_protocol.cc`、`mp3_decoder.cc` SOURCES + `esp-libhelix-mp3` 依赖 | SMK-A1/E1 |
| 凭据不入库 | `firmware/.gitignore`（追加 `smoke_nvs.csv/bin`） | 忽略 NVS 注入凭据（含 wifi 密码 + token） | 安全轴 |
| shim 鉴权门修复 | `server/shim.py` | `require_device` `return JSONResponse` → `raise HTTPException` | 401 门穿透 |
| 冻结验收 | `server/acceptance/test_smoke_e2e.acceptance.py`（新增） | hybrid 结构/live/device-evidence 验收 | SMK-A1..E3 |
| 治理热修 | `.claude/scripts/acceptance-coverage-check.mjs` | `hasRealAssertion` 补 Python 断言识别（三支，不松门） | acceptance-coverage 门误拒 |
| closeout 制品 | `changes/dev/260829-smoke-e2e/*` | docs-delta + review-packet + ADR | 本包归档 |

## B. docs-sync 判定（docsNoChangeReason，已生效）

- docRefs 为上游/上下文引用：`.planning/kidpal-mvp/PROPOSAL.md` 受 `upstreamFrozen sha1:500df090a227865e` pin（禁改，防 UPSTREAM_DRIFT）；`docs/spec.md#§3.0.0`/`#§6.1` 为章节锚点引用，canonical spec 改动走 Track B 治理批，非本包直改面。
- 行为验收承载在冻结验收文件 `server/acceptance/test_smoke_e2e.acceptance.py`（coverage marker 已受 acceptance-coverage 闸 + 4 条 mutation 敏感性实证），非 docRefs。

## C. 笔记面检查（docs-sync note surface）

- `docs/notes/feature-inventory.md` 现有清单 §1（UI 逐点）与 §2（与 OpenClaw 对接 OC-1..15，均**云端/服务端侧**），**无固件/设备侧能力清单**。本包落地的「设备侧 HTTP shim 协议」在 feature-inventory 无对应行可补 → docs-sync 笔记面**无变更**，如实记为「无对应面」。

## D. ADR 落点说明（integrator scope 边界，非自行裁量）

- `adrRequired=true`，ADR 已落 `changes/dev/260829-smoke-e2e/adr-260829-smoke-e2e-device-http-shim.md`（内容完整，含背景/决策/实现形态 7 点/取舍/安全关联）。
- 原拟落 `docs/notes/adr-260829-smoke-e2e-device-http-shim.md`，实测 enforce-scope 拦截：`docs/notes/**` 不在 integrator 写面（报错原文：「只能写 firmware/**, server/**, docs/systems/**, docs/templates/**, docs/ARCH-REVIEW-QUEUE.md, changes/dev/**, .claude/taskpackages/**, .planning/**」），与 TP-A 先例一致。若需把 ADR 晋升到 `docs/notes/`（经验/策略类文档面），请**主线程**执行迁移，正文可整体搬用。

## E. 未测面与特殊放行（逐字引 human 裁定）

- pod-events.jsonl 事件 `c2682f8c`（human 2026-09-02）：「本包为下游多包依赖的底座，除真实声学往返（麦克风/喇叭硬件缺位）外已全部实证通过（结构面 8G、live 5/5、mutation 4 条、物理联通段全证）；未测面=真实录音往返 roundtrip.json 与 SMK-A4 回复文字/mutation，按人裁豁免、留硬件到位后由下游声学包（260829-voice-roundtrip）实证。风险接受：声学往返未经端到端验证即合并。」
- **未测面（本包如实载明，不粉饰成「全过」）**：① 真实录音往返 `roundtrip.json`（device 证据断言）② SMK-A4 回复文字形态屏显 ③ A4 mutation 演示。

## F. 无漂移确认

- `docs/contracts/interface-contract.md` 本包只读引用（`contractRefs`），零写；shim 四端点字段信封缺口已在 `ARQ-260901-shim-api-contract-gap` 登记，本包不触发、不新增字段。
- 无新增 `docs/` 根文件、无影子目录。
- 报告/证据落 `changes/dev/260829-smoke-e2e/`，不散写。