# Review Packet — 260829-smoke-e2e

> integrator 收口。合并门前的审查包。本包为**架构可行性冒烟（tracer bullet 缩域 C3）**（riskTier=high，subsystem=firmware+server），
> 目的是证「设备↔shim↔OpenClaw」三层链路串通，零业务；是下游 12 包依赖的底座（mergeOrder=1）。

## Change Size

- **Files changed**（本包落地）：
  - 固件实现（新增）：`firmware/main/protocols/shim_protocol.{cc,h}`、`firmware/main/audio/mp3_decoder.{cc,h}`
  - 固件改动：`firmware/main/audio/audio_service.{cc,h}`（`PlayMp3`）、`firmware/main/application.cc`（协议分支 + 激活门旁路）、`firmware/main/CMakeLists.txt`、`firmware/main/Kconfig.projbuild`、`firmware/main/idf_component.yml`、`firmware/.gitignore`
  - 服务端修复：`server/shim.py`（`require_device` 401 门穿透）
  - 验收（新增/冻结）：`server/acceptance/test_smoke_e2e.acceptance.py`
  - 治理热修：`.claude/scripts/acceptance-coverage-check.mjs`
  - closeout：`changes/dev/260829-smoke-e2e/*`
- **Net diff**：约 +1,100 行（设备侧 shim 协议 + mp3 解码 + 冻结验收），零删除既有产品逻辑，仅 `server/shim.py` 一处鉴权门修复。**纯新增 + 一处安全修复**。
- **Scope**：双子系统（firmware+server）。设备侧新增 shim 协议腿 + mp3 解码路径；服务端只修 `require_device` 鉴权门。
- **Split recommendation**：不拆。tracer-bullet 单包内聚，验收与实现 1:1。

## Verification Evidence

### 承重事实（verify.mjs round，唯一 blocking 机器件）

- **final r6（invocation `ddbb211c`）= ok:true / observedGateExit 0**；scoped r5（invocation `fb7d0906`）同为 ok:true。
- 承重事实文件：`.claude/taskpackages/260829-smoke-e2e/verifier-evidence/rounds/r6.json`。
- 投影·advisory：`.claude/taskpackages/current/verdict.json`（verify.mjs 程序落盘，非 agent Edit/Write）。本包未手写 `changes/dev/verdict.json` / `verification.json`（R11：投影·advisory 由机器产线程序 fs-write，integrator 只消费不落盘）。

### 逐闸状态（r6，诚实口径）

| 闸 | 态 | 要点 |
|---|---|---|
| taskpackage-required | PASS | changeId 一致 |
| firmware-build | PASS·**degraded** | idf.py 不可用，固件编译未在本机 re-verify（builder 侧已真编译 + 烧录实证——burn.log + 「Shim protocol selected」日志）。非假绿，是 re-verification 降级 |
| contract-lint | PASS | 1 条警告（contract-link-broken `../systems/kidpal/spec.md`，本仓 spec 平铺 `docs/spec.md` 的既有残链，非本包引入） |
| acceptance-coverage | PASS | SMK-A1..A4 coverage 全识别（r1 曾因 Python 断言识别缺口误拒，human 批准热修后转绿，见过程缺陷①） |
| mutation-check | PASS·skipped | human 特殊放行 c2682f8c（SMK-A2/A3 四条敏感性已手工实证、A4 豁免） |
| docs-sync | PASS | docsNoChangeReason 生效 |
| security | PASS | 三轴 + securityEvidence 实证 |
| unit / acceptance | PASS·executor-skipped | live-only on-target 验收（@pytest.mark.live 门控，ARQ-260901-g3-server-live-acceptance） |

### 冻结验收实测（结构面 pytest）—— 诚实，非「全过」

`server/acceptance/test_smoke_e2e.acceptance.py` 结构面（10 tests）实测 = **8 GREEN / 2 RED**，两红同因 `roundtrip.json`（真实声学往返证据，需麦克风+喇叭硬件）：

- **绿（8）**：a1 设备 HTTP shim 协议存在、a2 配网指向 shim 非 18789（禁词守卫）、a3 mp3 解码路径、e1 build 结构、e3 零业务守卫、boundary 空文本 400 静态接缝、security 无硬编码 token、a4 屏显占位串（screen.md）。
- **红（2）**：`test_a3_device_roundtrip_evidence`（roundtrip.json 缺）+ `test_e2_evidence_directory_structure`（EVIDENCE_FILES 缺 roundtrip.json）。**两红均为未测声学面，按 human c2682f8c 豁免，硬件到位由下游声学包 260829-voice-roundtrip 实证。**

### live 验收实测（5/5 全绿）

SSH ECS 8.137.91.16 实测：healthz / a2 shim 收设备请求（journalctl 有 /v1/*）/ a3 asr→chat 一次往返（真实 edge-tts→faster-whisper→OpenClaw→mp3 下行）/ empty-text 4xx / unauthorized 401 硬闸。**device 硬件段 roll 外的 live 全绿。**

### Mutation check

`mutationRequired=false`（human 特殊放行）。SMK-A2/A3 四条敏感性已手工实证（`mutationEvidence.perAcceptance` M-A2.1/A2.2/A3.1/A3.2，turned red → 复归干净，红因归属正确）留作审计；SMK-A4 屏显敏感性依赖声学硬件真显，按人裁豁免。

## Security 三轴实证摘要（引 taskpackage.securityEvidence）

- **public-exposure-and-device-token**：临时公网暴露经 human 批准（dbc9b310），强随机 64-hex 设备 token（ECS 机生成，`/opt/kidpal/shim/secrets.env` 权限 600）门控 /v1/*；修复 require_device 门穿透后 ECS 真机复验：无 token → 401 且零 OpenClaw 转发、错 token → 401、带 token 空文本 → 400。
- **secrets-not-in-firmware**：固件源码/镜像零 token（`test_security_no_hardcoded_device_token_in_firmware` 绿 + CONFIG_SHIM_DEVICE_TOKEN 空），token 仅 NVS 分区注入（`smoke_nvs.csv/bin` gitignore 不入库）；无麦冒烟会话以真实 token 过公网 401 门（/v1/asr 200）。
- **loopback-only-openclaw-forward**：OPENCLAW_TOKEN 仅存 ECS 本机、仅 127.0.0.1:18789 loopback；固件语料零 18789/openclaw（激活门旁路后镜像死代码消除，tenclass/xiaozhi.me 字符串归零）。残余风险对偶 TP-A finding #1。

## 过程缺陷清单（发现→路由→修复，均已修复）

| # | 缺陷 | 发现 | 路由 | 修复 | 复验 |
|---|---|---|---|---|---|
| ① | acceptance-coverage 门 Python 断言识别缺口（`hasRealAssertion` 只认 JS 风格） | verify.mjs scoped r1 误拒冻结验收 | escalate `8bb97fb8` → human `0faa3a03` 批准一次性热修（跳过流程，诚实记录） | `.claude/scripts/acceptance-coverage-check.mjs` 纯追加三支（`.assert[A-Z]`/行首裸 assert/`.fail(`），门义不松 | 门转绿 + 无断言反例仍拒 + JS 回归正常 |
| ② | shim.py `require_device` 门穿透（`return JSONResponse` 不短路） | 真板冒烟 journalctl 见无 token 直达 OpenClaw 完整聊天轮 | friction `1dd01631` → fixer `a91874d2` | `raise HTTPException` | ECS 真机 401/400/零转发 |
| ③ | 启动序激活门阻塞（`ActivationTask` 重试 api.tenclass.net 先于 `InitializeProtocol`） | 真板首测未到 ShimProtocol | friction `7922efae` → builder | 编译期旁路 `#if !CONFIG_USE_SHIM_PROTOCOL` + 死代码消除 | 重烧后「Shim protocol selected」+ tenclass 字符串归零 |
| ④ | live 线束 3 缺陷（2 处 `_ssh` 三元解包写二元、`_remote_write` 大 payload 撞 Windows 命令行上限） | 冻结验收 live 首跑 | acceptance-author | 断言语义零变更修复 | live 5/5 全绿 |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `shim_protocol.cc` | `SendText` 空实现（无持久文本通道，请求/响应逐次） | M2 流式通道 | 需流式 ASR/TTS 时 |
| `shim_protocol.cc` `FinalizeSession` | 攒整段音频一次性上/下行（非流式） | M2 | 同上 |
| `shim_protocol.cc` `capture_ms_=3000` 固定默认 | 冒烟捕获窗口 | 可调 Kconfig | 真交互 pacing 时 |

> 无 `minimal:` 标记。tracer-bullet 命中 minimal-implementation：同步 HTTP + 最小 Helix mp3 + Kconfig/NVS 配网，复用 server/shim.py 未自建，无过早抽象、无重复实现。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 安全/可运维性 | ECS 公网暴露面未回收（shim `0.0.0.0:8080` + 安全组放行仍开，冒烟后须回收） | changed-code hunk `server/shim.py` + progress.md「暴露面未回收（冒烟仍需）」；对偶 TP-A finding #1 | 收口后回收暴露面到 loopback + 关安全组 |

> 无 BLOCKING finding。过程缺陷①②③④均已修复（上表），不构成 open finding。结构闸证据不重复包装为 finding（code-review-gate 约定）。

## Five-Axis Review

1. **Correctness**：SMK-A1..A4、E1..E3 语义逐条落地——设备 HTTP shim 协议（A1）、配网指向 shim 非 18789（A2 禁词守卫 + 语料零命中）、mp3 解码路径（A3 设备腿）、/v1/asr→/v1/chat 闭环（A3）、屏显占位串（A4）、零业务守卫（E3 grep 零命中）。结构面 8 GREEN 与冻结判定逐条吻合。
2. **Test quality**：冻结验收断言真实行为——协议文件在场、禁词守卫 18789/openclaw、mp3 路径、证据文件非空、边界 4xx、安全禁硬编码 token；live 断言真实 SSH 副作用（HTTP 码/JSON 可解析/非空 audio/display.view/401 硬闸）。非「没报错就算过」。
3. **Contract/boundary safety**：契约面零触碰（`contractRefs` 只读引用 `interface-contract.md`；shim 四端点字段信封缺口已登记 ARQ-260901-shim-api-contract-gap，本包不新增字段）。spec §3.0.0/§6.1 只读锚点引用。
4. **Operability/security**：三轴实证（见上）。X-Correlation-Id 贯穿设备→shim；device token 不硬编码、NVS 注入；OpenClaw token 仅 loopback。残余 = ECS 暴露面未回收（finding #1）。
5. **Maintainability/simplicity**：tracer-bullet 最小实现——同步 HTTP 换流式、Helix mp3 定点解码、Kconfig/NVS 配网、激活门编译期旁路（死代码消除使镜像归零 tenclass/xiaozhi 字符串）。复用 server/shim.py 未自建；协议选择层只落一个编织位、不侵入状态机其余部分。

## Residual Risk

- **Known limitation**：
  - 真实声学往返（`roundtrip.json` + SMK-A4 回复文字形态）未测——麦克风/喇叭硬件缺位，整语段端到端声学未验证（human c2682f8c 豁免，下游 260829-voice-roundtrip 补）。
  - 固件编译未在本机 re-verify（idf.py 不可用；builder 侧已真编译 + 烧录实证）。
  - ECS 暴露面未回收（finding #1）。
- **Follow-up recommended**：
  - 下游声学包 260829-voice-roundtrip 硬件到位后补 roundtrip.json + A4 屏显 + A4 mutation。
  - 回收 ECS 暴露面到 loopback + 关闭安全组。
  - TP-A 收口制品加真实性复核注（其「无 token → 401」实测结论被本包真机复验否定，见 ARQ-b）。
- **Human decision needed**：
  - （已裁定）声学往返豁免合并（c2682f8c）。无新增人类裁决项。

## decisionCoverage 残险（landsIn=dropped / risk-accepted）

- 本包 `decisionCoverage` 两条（PROPOSAL-§2-包S → acceptance:SMK-A3；PROPOSAL-§5-OQ3 → exitCriteria:SMK-E3）均 landsIn 到 acceptance/exitCriteria，无 `dropped`/`risk-accepted` 条目 → 无此列表。

## 未测面与特殊放行声明（逐字引）

> pod-events.jsonl 事件 `c2682f8c`（human 2026-09-02）：
> 「本包为下游多包依赖的底座，除真实声学往返（麦克风/喇叭硬件缺位）外已全部实证通过（结构面 8G、live 5/5、mutation 4 条、物理联通段全证）；未测面=真实录音往返 roundtrip.json 与 SMK-A4 回复文字/mutation，按人裁豁免、留硬件到位后由下游声学包（260829-voice-roundtrip）实证。风险接受：声学往返未经端到端验证即合并。」

- **未测面（豁免，硬件到位由下游声学包 260829-voice-roundtrip 实证）**：① 真实录音往返 `roundtrip.json`（device 证据断言）② SMK-A4 回复文字形态屏显 ③ A4 mutation 演示。
- **本包如实载明**：结构面 8 GREEN / 2 RED（两红即声学面）、live 5/5、mutation 4 条（A2/A3）+ A4 豁免。**不粉饰为「全过」。**