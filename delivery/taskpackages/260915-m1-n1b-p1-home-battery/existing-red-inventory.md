# existing-red-inventory.md · 260915-m1-n1b-p1-home-battery

> 先存红分类清单 + 修复意图登记（governance 通道：红证据 ESCALATE 合法停止的豁免件）。对偶 `delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/existing-red-inventory.md` 格式。
> 目的：本包 final r12 verify 非 全绿，红项一律为**launch 预报在册既存红或 human 裁定销账项**，无一由本包改动引入。逐条分类、登记归属，供 human 裁。
> 原则：不伪造、不静默、不喂哨兵；「留红如案」= 有 owner/有意图登记的显式红。
> 判读权威 = `delivery/taskpackages/260915-m1-n1b-p1-home-battery/verifier-evidence/rounds/r12.json`（final，2026-09-16）+ pod-events human-decision `bba7d239`（2026-09-16）。

## 触达本包 final 闸的红（server 面，r12：unit 2F/159P/14S · acceptance 2F/122P/14S）

| # | 红项（测试/闸） | 断言语义 | 归属包 | 分类 | 修复意图登记 |
|---|---|---|---|---|---|
| R1 | `acceptance/test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4；同时绊 unit/acceptance 两闸） | `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据 | 260829-smoke-e2e | 既存证据件缺口（[evidence] 族先行红） | owner=260829-smoke-e2e；本包不代采（不是本包冒烟面）。与 K1a 包 inventory R1 / 260911-key-route-a1 inventory R1 同源同处置。 |
| R2 | `acceptance/test_schedule_reminder.acceptance.py::test_e4_contract_point3_evidenced_and_poll_form_retired`（SRE-E4） | spec 的送达语义记录被改动（「重试到确认」口径是包 7 的验收依据） | owner 归 intake 判读（K1a 包 inventory 明注：与本包板侧 KD-A4 非同一齿，勿混） | spec 面既存红（spec 送达语义记录 vs 现文漂移） | owner 归 intake 判读；非本包范围（本包板侧钉点批改走 acceptance-author REVISION LOG 已闭环，与 server 面此齿不同物）。 |

## 已销账项（human 裁定退出现役，非修绿）

| # | 红项 | 机制判读 | 处置 | 回滚方式 |
|---|---|---|---|---|
| R3 | `acceptance/test_voice_roundtrip_b.acceptance.py::test_a2_protocol_profile_twin_aligned`（VRT-A2；r6 期第三红） | MVP 验证期验收的 **server 半边**——设备半边（C 孪生 `firmware/main/test/voice_roundtrip_b.acceptance.c`）已被 544a804 归档退役，server 半边留现役纯属归档批**扫尾遗漏**（孪生对齐齿 `C_TWIN` 硬编码指旧址 :113 故红） | **human 裁定整体退出现役运行范围**（pod-events human-decision `bba7d239`，2026-09-16，decision=`vrt-a2-retire-out-of-scope`，逐字「不要跑这个了」；AskUserQuestion 留痕；按 human 明示跳过流程的一次性操作落账）。落地形态 = 模块级 skip（`test_voice_roundtrip_b.acceptance.py` +16 行 `pytestmark = pytest.mark.skip(...)`；bba7d239 初拟移入 archive/，实际改模块级 skip——gate 收集 glob 为 server/ 递归、物理移目录仍被收集且需动治理脚本，码内注释在案）。**非修绿、非验证通过**；escalation `dfda0852`（r6 期归属 pending）随之 resolved | 删除该 pytestmark 块（单文件单块，无其他耦合面） |

## 未触达本包 final 闸、但 host 面既存留红的齿

| # | 红项 | 归属/现象 | 修复意图登记 |
|---|---|---|---|
| R4 | host 通道 K1a 期 4 红（SU-E3 / KR-A6 / WSA-A4 / WSA-A6） | spec-split 归档悬空 canary（SU-E3/KR-A6 同根 docs/spec.md canary）+ 260903-device-ws-switch 证据件缺口（WSA-A4/WSA-A6） | owner 各自不变（spec-split 批次 / 260903-device-ws-switch）；随 544a804 归档已移出现役面，基线消解走独立批；非本包范围。逐条详情见 K1a 包 inventory R3-R6。 |

> 注：host 面终跑 = **13 Tests / 0 Failures / 0 Ignored**（8 基线 + 本包新齿 5 条全绿，PH-A7 板侧判据达标）。server 面 r12 = SMK-A4 / SRE-E4 两条在册红，**相对有效基线零新增**；VRT-A2（r6 期第三红）已人裁退出失败面。扫尾项：server 面可能仍有 MVP 期孪生/伴随齿未随 544a804 归档批退役（已实证一例 = VRT-A2 server 半边），留独立批 sweep——`ARQ-260916-server-mvp-teeth-sweep` 在册 `docs/ARCH-REVIEW-QUEUE.md`，不阻本包。

## 结论

- 本包自身面**全绿**：final r12 七闸全绿（含 firmware-build 真编译）；host runner 13/0；新齿 5 条全绿；mutation 10 条执行在册全敏感；PH-A6 真机确认轮 **PASS（附带条件）**（human 视觉四项 `f6132282` + 收尾裁定 `45910f60`）。
- final 红（R1/R2）与 host 面既存红（R4）均为**launch 预报先存跨包缺口**，无一由本包改动引入；R3 已人裁销账退出失败面。
- 修复意图均已登记 owner（R1 沿既往 inventory；R2 归 intake 判读；R4 各自 owner），按「留红如案」处理；本包不越权代修、不伪造证据件。
- human ruling 通道：VRT-A2 归属已由 human 裁定销账（`bba7d239`）；合并门剩余红证据 ESCALATE 豁免件即本清单。
