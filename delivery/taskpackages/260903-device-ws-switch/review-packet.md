# Review Packet — 260903-device-ws-switch

> integrator 收口。合并门前的审查包。本包为**方向 B 重切后的设备侧切换（tracer-bullet 设备片）**（riskTier=high，
> subsystem=firmware+server，mergeOrder=4，dependsOn=260903-gateway-directionB）：把设备从 Pod S 的同步 HTTP shim
> 形态切到「配网直写 NVS + 自签证书 pinning + WS 握手 Bearer + 原生 WS+Opus 流式」，并退役 HTTP 台阶
> （固件死代码消除 + shim.py 归档 + ECS 暴露面回收 + Pod S/TP-A 冻结验收 supersede）。

## Change Size

- **Files changed**（本包落地，working-tree diff）：
  - 固件实现（新增）：`firmware/main/provisioning/ws_provision.{cc,h}` + `ws_policy.h`、`firmware/main/certs/kidpal_ca.pem` + `ws_tls_ca.{cc,h}`
  - 固件改动：`firmware/main/protocols/websocket_protocol.cc`（NVS 读缝 + CA pinning）、`firmware/main/application.{cc,h}`（直选 WS + 激活门短路）、`firmware/main/audio/audio_service.{cc,h}`（去 PlayMp3）、`firmware/main/CMakeLists.txt`、`firmware/main/Kconfig.projbuild`、`firmware/main/idf_component.yml`、`firmware/main/test/host_app/main/CMakeLists.txt`
  - 固件删除（HTTP 台阶死代码）：`firmware/main/protocols/shim_protocol.{cc,h}`、`firmware/main/audio/mp3_decoder.{cc,h}`
  - 固件验收/单测（新增）：`firmware/main/test/device_ws_switch.acceptance.c`（954 行，WSA-A1..A7+boundary）、`firmware/main/test/ws_provision.test.cpp`
  - 服务端：`server/gateway/front/front_app.py`（websockets v17 兼容 + 设备头透传）、`server/deploy/docker-compose.yml`（network_mode host）、`server/deploy/provider.config.yaml`（base_url /v1 + TTS edge）、`server/shim.py`（ARCHIVED 墓碑）、`server/deploy/README.md`（docs-sync）
  - 验收 supersede 注记：`server/acceptance/test_cloud_link.acceptance.py`、`test_smoke_e2e.acceptance.py`（头注 +SUPERSEDED，断言零变更）
  - 治理/证据：`.claude/taskpackages/260903-device-ws-switch/*`、`changes/dev/260903-device-ws-switch/*`
- **Net diff**：tracked 面 22 文件 +304/−983（HTTP 台阶净删除为主：shim_protocol 543 行 + mp3_decoder 116 行 + 4 Kconfig 项）；另有新增固件实现/验收/证书等未入 tracked diff 的新文件。**净退役 > 净新增**。
- **Scope**：双子系统（firmware+server）。设备侧切换传输协议 + 退役台阶；服务端只做方向 B 网关的 live 适配与 shim 归档。
- **Split recommendation**：不拆。方向 B 设备片单包内聚，验收与实现 1:1。

## Verification Evidence

### 承重事实（verify.mjs round，唯一 blocking 机器件）

- **final r4（invocation `98c86540-8cb6-4f94-96ec-35cb5ee055ec`）= ok:true / observedGateExit 0**，phase=final、round=4，startedAt 2026-09-06T02:15:19Z。
- 承重事实文件：`.claude/taskpackages/260903-device-ws-switch/verifier-evidence/rounds/r4.json`（authority=bearing）。
- 投影·advisory：`.claude/taskpackages/current/verdict.json`（verify.mjs 程序落盘，非 agent Edit/Write）。本包**未手写** `changes/dev/verdict.json` / `verification.json`（R11：投影·advisory 由机器产线程序 fs-write，integrator 只消费不落盘；closeout-check 对二者为 advisory-only）。

### 逐闸状态（r4，诚实口径）

| 闸 | 态 | 要点 |
|---|---|---|
| taskpackage-required | PASS | changeId 一致 |
| firmware-build | PASS·**degraded** | idf.py 本机不可用，固件编译未在本机 re-verify（builder 侧已真编译 + 真机烧录实证——flash.log）。非假绿，是 re-verification 降级放行 |
| contract-lint | PASS | `docs/contracts/interface-contract.md` 零触碰 |
| acceptance-coverage | PASS | WSA-A1..A7 全识别（A1 structural，A2..A7 functional） |
| mutation-check | PASS | 6 条 functional acceptance 各有 mutationEvidence（turnedRed=true，sensitive=true） |
| docs-sync | PASS | docsNoChangeReason 生效（见 docs-delta B 段） |
| security | PASS | securityReviewRequired=false |
| unit / acceptance | PASS·executor-skipped | verify.mjs 侧 live-only on-target 闸（@pytest.mark.live 手动触发）+ acceptance oracle-pass（firmware 2 文件/server 3 文件结构面）——on-target 实证由下方 host ELF + live 冒烟承载 |

### Host ELF 验收实测（闸 runner，WSA-A1..A7 + boundary + 单测 + host_infra）

- 冻结验收经 builder 接线进 `host_app` linux-target ELF（`HOST_ACCEPTANCE_SRCS` + `device_ws_switch.acceptance.c` + `ws_provision.test.cpp`），WSL Ubuntu-24.04 原生执行：**15 Tests 0 Failures，exit 0**。
- 红→绿演进（红基线纪律留痕）：acceptance-author 红基线 8 Tests 6 Failures（A1..A6 各对缺失行为红、A7/boundary by-construction 绿）→ builder 实现落地后 15 Tests 2 Failures（仅剩 A4/A6 证据齿待 live 证据落盘）→ live 冒烟证据拼装后 **15/0**。

### live 方向 B 冒烟实证（2026-09-06 真机）

- 设备 ⇄ wss:8000 front ⇄ 引擎 ⇄ OpenClaw 全链串通：ASR→LLM→**流式 TTS** 下行播放 + stt 文字上屏，多轮。
- 证据拼装于 `server/acceptance/_artifacts/device-ws-switch/`（flash.log / provision.log / smoke-roundtrip.log / screen1.jpg / ecs-teardown-evidence.md）。**该目录被 `server/acceptance/.gitignore` 的 `_artifacts/` 规则忽略——本地-only、by design 不入库**，integrator 未 force-add。
- WSA-A6 live 腿：ECS shim 服务停 + 8080 安全组关闭，证据落 `ecs-teardown-evidence.md`。

### WSA-A4 证据形态修订（human 授权「验收改认已有证据」）

- 原齿 3 要字面 wire 字符串 `stt`/`tts`/`sentence_start`；引擎↔front WS 腿协商 permessage-deflate，这些帧在线上被压缩、任何日志无字面量。human 授权改认本栈真实标记（session_id 字面保留；ASR←`识别文本`/`>>`/`stt`；TTS←`语音生成成功`/`发送第一段语音`/`tts`；流式←`SentenceType`/`发送第一段语音`/`sentence_start`），**四事实仍全强制**，由真实 `smoke-roundtrip.log` 实证。
- acceptance-author 执行（`device_ws_switch.acceptance.c` 头注 2026-09-06 修订记录，源 = 真实 2026-09-06 冒烟 engine session `40024357-2e41-4bb4-9ae9-7f07327e3295`）；**证据形态改革，非严格度放松**。integrator 未触碰验收文件。

### Mutation check

6 条 functional acceptance（WSA-A2..A7）各有 `mutationEvidence`、`turnedRed=true`、`sensitive=true`，红因归属正确（CSPRNG 换常量 / CA 移出树 / 证据三态 / tenclass 回填 / ecs 证据三态 / supersede 标记消失），还原 sha256 与基线一致。详见 taskpackage.mutationEvidence 与 red-baseline-summary.md。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| —（无新 `minimal:` 标记） | — | — | no-trigger |

> 本包 diff 内无新增 `minimal:` 标记（唯一的 `server/shim.py:295` 一处位于本包归档退役的 shim.py 内，随台阶一并退役，不计入新债务）。tracer-bullet 命中 minimal-implementation：复用原生 WebsocketProtocol + 官方 xiaozhi 服务端，未自建流式编排，无过早抽象、无重复实现。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 安全/凭据 | ASR 终选 `aliyun_stream`（决策 19 云优先 / V23）凭据始终未就位，live 冒烟以 `qwen3_asr_flash` + 用户 dashscope key 顶替 | changed-code hunk `provider.config.yaml` asr 段（`api_key_env: DASHSCOPE_API_KEY` / `downgrade_chain`）+ docs-delta E.2 ② | human 补办 ALIYUN_ASR 凭据，或正式裁定 `qwen3_asr_flash` 为 ASR 终选（治理/spec 面） |
| SHOULD | judgement-call | 功能/范围 | 引擎 `Intent: nointent`（工具调用关闭）——OpenClaw 拒绝 tools 数组（`invalid tool configuration`），tracer-bullet 为串通首链先关意图/工具 | changed-code hunk（引擎 data/.config 运行期 overlay）+ docs-delta E.2 ①；deviation 性质 = 缩域非缺陷 | 下游包在 OpenClaw tools 配置修复后回开 Intent/tool-calling（意图/读课表/存故事/查字为下游业务包范围） |
| SHOULD | judgement-call | docs-sync 笔记面 | `docs/notes/feature-inventory.md` OC-7 行仍描述已退役的 HTTP shim「✅ 全路径跑通」，与本包 shim 退役漂移 | docs-delta C 段（`docs/notes/**` 不在 integrator 写面） | 主线程更新 OC-7 行 + 补设备侧 WS 能力入清单 |
| FYI | judgement-call | 过程/可运维 | 假阳性教训：早前 NVS「patch version→1」脚本 Gate 在错误页魔数（`\xfe\x83` vs 真实 `\xfe\xff`）上静默 no-op，version=1 靠后续烧录才落 | 任务「must record」指令 + docs-delta E.3（已记入 ops skills） | 无代码动作；ops 纪律：闸判据须对真实载体（魔数/指纹）核验 |

> **无 BLOCKING finding。** 结构闸证据不重复包装为 finding（code-review-gate 约定）；gate 全绿仍完整执行五轴语义 review（下）。

## Five-Axis Review

1. **Correctness**：WSA-A1..A7 语义逐条落地——WS 直选实例化（A1）、配网直写 NVS url/token/version + 短路激活（A2）、自签 CA 嵌入 + 加载路径 + pinning（A3）、方向 B 首链冒烟证据（A4）、HTTP 台阶死代码消除 + tenclass 字符串归零（A5）、ECS 暴露面回收 + shim.py 归档（A6）、Pod S/TP-A 验收 supersede（A7）。host ELF 15/0 与冻结判定逐条吻合。
2. **Test quality**：验收断言真实行为——结构齿扫真实调用点（`esp_fill_random(` 调用点级，非裸 include，闭合了 fixer mutation 暴露的 dangling-include 假绿洞）、证据齿三态取证（缺即红/满足形状即绿/破坏载体即红）、扫描器双针命中防 first-match 遮蔽。非「没报错就算过」。live 断言读真实拼装的 smoke-roundtrip.log。
3. **Contract/boundary safety**：契约面零触碰（`contractRefs` 只读 `interface-contract.md`，contract-lint PASS）；WSA-A4 证据形态修订经 human 授权、不动契约 §四 的协议事实定义（只改日志取证标记）；supersede 断言语义零变更（只改归属状态注记）。spec §3.0.0/§9 已先行冻结方向 B，无 spec 漂移。
4. **Operability/security**：设备弱 token 由配网 CSPRNG 生成（24 字节/192-bit，决策 22 ≥128-bit）写入 NVS、**不硬编码、不入库**（smoke_nvs.csv/bin gitignore）；wss 自签 CA pinning（CA 为公钥证书块，树内无私钥材料，boundary 齿守卫）；front 强制设备鉴权 + token 不入日志 + 白名单头透传；引擎网络收敛 host 模式、OpenClaw 仍 loopback-only（V28）；Pod S 的 ECS 暴露面（8080 + shim 服务）已回收。secret 扫描全树零泄漏（仅 `secrets.env.example` 占位 + `*_env` 环境变量引用）。
5. **Maintainability/simplicity**：净退役 > 净新增——删除 Pod S 同步 HTTP 台阶（shim_protocol/mp3_decoder/PlayMp3/4 Kconfig 项），复用原生 WebsocketProtocol 与官方 xiaozhi 服务端（不逐行自研流式编排）；配网直写入口单点（ws_provision.cc），证书加载单点（ws_tls_ca.cc）；无新抽象层、无投机灵活性。

## Residual Risk

- **Known limitation**：
  - live 冒烟为 tracer-bullet 串通（单/多轮 ASR→LLM→流式 TTS + 上屏），引擎 `Intent: nointent`、工具关闭——非全功能业务链（finding #2）。
  - ASR 终选 `aliyun_stream` 凭据缺口，live 以 `qwen3_asr_flash` 顶替（finding #1）。
  - firmware-build 闸 degraded（idf.py 本机不可用，固件编译未本机 re-verify；builder 侧真编译 + 烧录实证）。
  - 真实声学全链（AEC、多轮 pacing、功耗、屏显长文本）非本包范围，属下游业务包。
- **Follow-up recommended**：
  - human 补办 ALIYUN_ASR 凭据或裁定 `qwen3_asr_flash` 终选。
  - OpenClaw tools 配置修复后回开 Intent/tool-calling。
  - 主线程更新 `feature-inventory.md` OC-7 + 设备侧能力清单（finding #3）。
  - 包 2（语音回环 + display 消费）依赖本包就绪（WSA-E3，mergeOrder 5）。
- **Human decision needed**：
  - ASR 终选凭据/型号裁定（`aliyun_stream` vs `qwen3_asr_flash`）——唯一待人类拍板项。

## decisionCoverage 残险（landsIn=dropped / risk-accepted）

- 本包 `decisionCoverage` 五条（handoff-V10→WSA-A3、spec-§6.3→WSA-A2、决策22→WSA-A2、spec-§3.0.0→WSA-A5、OQ-R4→WSA-A6）均 `landsIn: acceptance:*`，**无 `dropped` / `risk-accepted` 条目** → 无此列表。

## 未测面与特殊放行声明

- **WSA-A4 证据形态修订**（human 授权「验收改认已有证据」）：字面 wire 字符串 → 本栈真实日志标记，四协议事实仍全强制（见上「Verification Evidence」专节）。非豁免、非降严格度，是证据载体形态改革。
- **firmware-build 降级放行**：idf.py 本机不可用，固件编译未本机 re-verify——builder 侧已真编译 + 真机烧录实证（flash.log），非假绿。
- **live 偏差**（nointent / qwen3_asr_flash / edge TTS / network_mode host / websockets v17）：均为串通首链的必要偏差，逐条登记 docs-delta E.2 留 retro。
- **本包如实载明**：host ELF 15/0、live 方向 B 冒烟串通、verify.mjs final r4 九闸 ok:true、mutation 6 条。**不粉饰为「全功能上线」——本包是方向 B 设备片的 tracer-bullet 串通 + HTTP 台阶退役，业务意图/工具链留下游。**
