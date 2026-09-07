# docs-delta — TaskPackage 260829-char-stroke（subsystem: firmware + server，C1 云侧静态字库）

> 读者是人（supervisor / human / 后续 wrap-up）。本包 docsOwner = integrator，本文记录
> 「docs-sync 已完成/已确认无漂移」部分，以及「有漂移但 integrator 不直改、路由 planner /
> 主线程」的建议。
> 边界注：本包零触 `docs/contracts/**`（闸 A architect-reviewer 四分类裁定 ④ 无需改契约
> NO_CONTRACT_CHANGE，契约门未出场，预审全文见同目录 `gate-a-prereview.md`）。

## 1. 变更摘要

查生字 P4（设备面）+ C1 云侧静态字库（机制 B = OpenClaw dist 补丁）。设备侧新增 P4 查字页
（米字格 + 逐笔动画 + 速查卡）与 D3 笔顺 JSON 消费链（`char_stroke.h/.c` 纯逻辑 + `char_page`
+ mcp_server case 3 防御降级门）；云侧新增 OpenClaw 侧查表拼装（`server/deploy/openclaw-patch/`：
静态字库 LRU + dist 补丁生成/应用 + 预热 + 部署 runbook）与 system prompt v2 版本化切换
（LLM 只出意图＋字，禁复述坐标）。数据源 makemeahanzi（9574 字）。

## 2. 已改（本包 docs-sync，integrator 直接执行）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| `changes/dev/260829-char-stroke/*` | 新增 | `docs-delta.md` + `review-packet.md` + taskpackage 快照 + intake/gate-a/survey/red-baseline/pod-events 快照 + verifier-evidence rounds | 本包 closeout/audit blocking 制品（closeout 制品 SSOT 表）。 |

除此之外 docs 面**零改动**：本包无 spec/契约措辞变更需求（闸 A 判 ④），无需更新 canonical docs。

## 3. 建议 planner / 主线程（integrator 出范围不直改）

- 无 docs 漂移级建议。机制 B（OpenClaw dist 补丁）属运维面而非文档面：若 human 对「给部署态
  OpenClaw 打补丁」有异议随时叫停（intake-notes 已记此升级口）；OpenClaw 升级致锚点漂移时
  DEPLOY-RUNBOOK §0 版本漂移闸会拦，届时 runbook 需随新版重校（届时再走 docs-sync）。

## 4. 无漂移确认

- `docs/contracts/interface-contract.md` 本包零写；C1 不改跨边界 wire format（到达设备仍是
  §1.3① 全量 D3 JSON），最小块终止于 OpenClaw 回合内不越网关边界，§1.5 薄桥红线被强化
  （`test_e3_gateway_thin_bridge_untouched` / `test_gate_a_display_schema_untouched` 冻结齿在案）。
- `docs/spec.md`（docRefs §3.0.2 D3 / §4.3 S-F3.1~F3.3）与本包落地一致：笔顺三态、950ms 步进、
  歇 2s 重来、速查卡、KEY 短按重播、1024→400×300 含 Y 翻转，逐项核对无漂移。
- `.planning/kidpal-mvp/PROPOSAL.md`（FROZEN v0.4）§2 包 4 行 + §5 C1 行（human 2026-09-01
  拍板「静态字库挂 OpenClaw 侧绝不入 shim」）与本包落地一致。
- 教学字体红线（spec §5.6）由光栅生成示范字满足（标准规范字形、零 tilt），CHR-A7 齿在案。
- 无新增 `docs/` 根文件、无影子目录；本包文档产物落 `server/deploy/openclaw-patch/
  DEPLOY-RUNBOOK.md`（部署面）+ `server/deploy/openclaw-system-prompt.v2.md`（版本化 prompt，
  v1 共存）+ `changes/dev/260829-char-stroke/`，不散写。
- `docs/ARCH-REVIEW-QUEUE.md` 本包无新增 ARQ 需登记（Intent:nointent 为既有 ARQ-260907 挂账，
  与本包机制无关，见 review-packet §8.4）。
