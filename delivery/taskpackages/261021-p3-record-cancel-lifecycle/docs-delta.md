# docs-delta · 261021-p3-record-cancel-lifecycle

- 日期：2026-10-21（integrator 收口相位据实申报）
- 作者：integrator
- 结论：**零新增 canonical 文档改动**。

## 申报明细

| 面 | 改动 | 依据 |
|---|---|---|
| `docs/contracts/**` | **零改动、零触碰**（本包恒只读） | 契约 §四 `kidpal_story_discard`（wire 形态 `{"session_id":"...","type":"kidpal_story_discard"}` + 发送时机 = CloseAudioChannel 之前同连接先行 + 网关按本连接 session_id 权威取消不信载荷值 + 幂等 + 审计 + 单向无应答 + 省略 = cancel-on-disconnect 兜底维持）已随上游 `/arch-change` 批 LANDED（CDQ-261021-p3-record-cancel-lifecycle，CONTRACT_CHANGE_REQUIRED R1 正式裁定；`docs/contracts/interface-contract.md:277-279` 变更记录条在盘）；§1.4 `start_organizing` 行包络核对零改（NO_CONTRACT_CHANGE）。本包纯 additive 消费零契约漂移——固件 wire 拼串与网关消费分支均逐字对齐冻结契约行，实地核对在位。 |
| `docs/architecture.md` | 零改动（本包相位） | §12 changeId 行 + §5.2 场景④ 取消链显式化一句（设备丢弃 → 契约 §四 `kidpal_story_discard` → 网关连接权威 cancel；断连 cancel 兜底）+ §5.1.2 front 行闭集枚举补一词（R1-F1 裁定随批补）已随上游架构批 landing（spine arch anchor resolved，delivery/review/261021-arch-review-p3-record-cancel-lifecycle/ FROZEN v1.1 + R2 GREEN + landing-verdict PASS）。本包为架构冻结面之下的实现收口。 |
| `docs/requirements.md` | 零改动（本包相位） | v1.66 零触（spine req anchor = N/A——需求面零命中，S-F2.2 AC4「取消、已录内容丢弃退回列表」既有语义实现对齐；S-F1.4-AC6 只作回归项；复验③重定义为实现侧验收定义，不触需求面）。 |
| `docs/decisions/ADR.md` | 零改动 | 本包无新架构裁决（`adrRequired=false`）；取消链架构权威已在 arch §5.2 场景④ 承载。 |
| `docs/debt/contract/CDQ-261021-p3-record-cancel-lifecycle.md` | 零改动 | 条目已 LANDED，本包零新契约发现（五轴 review 契约轴结论，见 review-packet.md）。 |
| `docs/debt/tech/TECH-261021-story-session-cancel-lifecycle.md` | 零改动（本包相位） | 来源 debt 条目已在册（`_archive/2026/` 归档位）；本包三症状闭环由 review-packet 与 verifier 证据承载，条目正文无需追加——如需「修复落地」注记属主线程 debt 状态机操作，非 docs-sync 写点。 |

## 实现侧文档承载（非 canonical，随代码落位）

canonical 之外的实现侧知识以代码注释与版本化制品承载，docs-sync 无需晋级：

- `firmware/main/protocols/protocol.h` / `protocol.cc`：`SendKidpalStoryDiscard` 发送原语注记（wire 逐字形状、发送时机 = CloseAudioChannel 之前、无接收者形态 = `s_kidpal_active_` 未就绪静默 no-op 离线降级——与 `SendKidpalStoryFetch` 同型）。
- `firmware/main/mcp_server.cc`：DISCARD_RECORD case 发帧先于关通道的帧序依据注记（同一 TCP 有序、front 泵先消费 discard 再处理断连；关通道保留 = cancel-on-disconnect 兜底）+ 回调上下文直接发送的线程形态注记（经 Schedule 排主任务会把帧序落到关通道之后）+ start_organizing 消费补 settle 的幂等注记（KEY 路径已 Idle 重复 settle no-op、front 侧收尾已先 cancel 零副作用）。
- `server/gateway/front/front_app.py`：`_consume_kidpal_control` discard 分支连接权威注记（载荷 session_id 仅审计、绝不作会话键）、`_story_guide_round` 双点存续守卫注记（[SC-B]）、`_session_legs_map` 三缝函数注记（[SC-D] 缝契约：登记/按 device_ws 身份摘除/纯查询返副本；提醒与课表两消费点维持全局腿不动）。
- 冻结验收件三件头注：`server/acceptance/test_record_cancel_lifecycle.acceptance.py`（[SC-A]–[SC-D] 缝契约 + 强度申报 + GAP 清单 + Coverage map）、`firmware/main/test/record_session_live.acceptance.c` / `record_dialogue_guide.acceptance.c`（REVISION LOG 2026-10-21 条）——seam 权威指向冻结件头，非 canonical。

## docs-sync 闸状态

verifier scoped r3 与 final r4（`verifier-evidence/rounds/r3.json` / `r4.json`）docs-sync 闸均 PASS（`docsNoChangeReason` 申报字段在册：契约/arch/requirements 均随上游批 landing、本包零 canonical 写点）；final r4 10/10 闸全绿（invocationId 5915a0c8-c3bb-4607-9b64-19dba531c8e1）。

## 升级评估

docs-sync 复核未发现任何暴露契约需要变化、spec 漂移或 canonical 缺口的点——实现 diff 与契约 §四 LANDED 行逐字对齐（wire 形态 / 发送时机 / 连接权威 / 幂等 / 审计五要素全落），与 arch §5.2 场景④取消链描述一致。若后续需在 architecture 某节补实现侧新符号注记（如 `_session_legs_map` 缝名），属架构面文档变更、走 `/arch-change` loop，不在本包范围内补写。**本包零 canonical 写点成立。**
