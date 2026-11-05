# device-round-evidence · 261104-k0-points-e2e-spike（真机取证轮）

> 结构化目击记录（pre-merge-escalation-gate deviceOutcome 子判据消费件）。目击轮 = 2026-11-05
> 真机轮（human 在场操作与目击；语音记账 ×2 + QQ canary ×1 + 「打开积分本」权威帧轮）。
> 最终二进制 = 本树 device build 产物（K0a merged 后 verify firmware-build 闸真编译，app-only
> @0x20000 烧录 NVS 保全，启动 SD 日志验证）；sourceCommit = 真机轮时树 HEAD（9962e626，
> 固件面自 LP8 段后零改动，合并后恒为合并 HEAD 祖先）。
> 日志通道 = 设备 SD 卡 KPLOG（kidpal_logs.py 拉取）+ ECS front/engine 日志（ssh_helper）。
> 完整叙事 = [spike-evidence.md](spike-evidence.md)（含 K0S-E1/E2/E3 human 判据修正裁定）。

device-round-evidence（JSON）：{"schemaVersion":1,"changeId":"261104-k0-points-e2e-spike","sourceCommit":"9962e626e41bbc8fb83a1e94b29f606fe639564a","binaryRef":"firmware/build/xiaozhi.bin","binarySha256":"c21cd6434cc589509c93f9dc660a8ffef2a3cd53a53850f3e750ba48020ef007","observations":[{"acceptanceRef":"K0PTS-L1","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T16:44:00+08:00","note":"human 目击两形态：①语音「我今天扫地了，加5分」后对话页回复含「扫地加5分」（会话浮层前置期）；②页点上方残段「10」= 第二轮「浇花加2分」后最小渲染行「星星 10 分 · 今天浇花」在 chat_message_label_（bottom_bar_ 页点上方）的可见残段——即权威 balance=10 的设备显示；human 裁定该形态观感归 K2 替换（判据修正见 spike-evidence §五）"},{"acceptanceRef":"K0PTS-L1","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0-points-e2e-spike/device-evidence/kplog-k0-points-rounds.log","excerptSha256":"769ecb8b39f69742b181c1d9c865311a3f597b0b18be8f86f3b0b327b27cab7d","segment":"KPLOG 真机轮","window":"2026-11-05 16:12-16:36"}},{"acceptanceRef":"K0PTS-L3","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T16:13:00+08:00","note":"NFR-PERF-4 实测时延：语音句说完到设备消费权威帧 ≈8s（ASR 16:12:51 → display_dispatch 16:12:58，大头 LLM 轮次）；QQ 链 ≈45s（main agent LLM+工具）；只记录不拍阈值"},{"acceptanceRef":"K0PTS-L3","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0-points-e2e-spike/device-evidence/kplog-k0-points-rounds.log","excerptSha256":"769ecb8b39f69742b181c1d9c865311a3f597b0b18be8f86f3b0b327b27cab7d","segment":"KPLOG 真机轮","window":"2026-11-05 16:12-16:36"}},{"acceptanceRef":"K0S-E3","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T16:16:30+08:00","note":"QQ canary 第一笔：真实家长 QQ「给小伴加3分，CANARYQ1帮妈妈收玩具」→ main agent → 最小 add CLI 落账（pt_2dfaea0d delta=+3 balance 5→8 revision=2，与语音笔同一 state 文件 = 单源活体）；QQ 收到正常回复 ≈45s；设备前台未动 = 不抢前台。第二笔原地刷新按 2026-11-05 human 判据修正降级申报（restore=True 活调用方 K0 未接线，GAP-1 兑现，顺延 K1/K2）"},{"acceptanceRef":"K0S-E3","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0-points-e2e-spike/device-evidence/kplog-k0-points-rounds.log","excerptSha256":"769ecb8b39f69742b181c1d9c865311a3f597b0b18be8f86f3b0b327b27cab7d","segment":"KPLOG 真机轮","window":"2026-11-05 16:12-16:36"}}]}

## 摘录对账要点（excerpt = kplog-k0-points-rounds.log, 16:12–16:36）

- 三轮权威帧设备消费：`display_dispatch view=points_home balance=5 revision=1 restore=0 takeover=1`（16:12:58，语音第一笔）→ `balance=8 revision=2 restore=0 takeover=1`（16:29:36，打开积分本轮——QQ CLI 写入后的权威状态）→ `balance=10 revision=3 restore=0 takeover=1`（16:34:29，语音第二笔）。
- ECS 侧对应行（spike-evidence §二/§三/§四 引用）：front `points_intent executed` ×3（corr_id 49a00ab3/bc7ca259 + nav 展开）、`gateway.points points_entry` ×3（pt_23877cbf / pt_b35bb6ed / pt_2dfaea0d 前两为语音后一为 QQ）。
- QQ canary 的 ECS 日志（journalctl 16:15–16:17）与设备 SD 日志按 spike-evidence.md §三 时间线关联；QQ 落账在 state 文件（pt_2dfaea0d，与语音同文件）。
