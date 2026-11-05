# device-round-evidence · 261104-k0a-four-page-skeleton（K0A-E3 真机轮）

> 结构化目击记录（pre-merge-escalation-gate deviceOutcome 子判据消费件）。目击轮 = 2026-11-05
> 三轮真机（每轮一缺陷一修复，全部闭合）；最终二进制 = 35b1958f 轮构建（其后 debt/证据/热修
> 制品提交不触固件树，31dfe585 树重编 hash 逐字一致 = ea143517…，绑定做实）。本件按闸口径记
> sourceCommit = 合并源 HEAD 31dfe585（热修 commit 在案，闸读盘时 HEAD 即此；提交版回填随
> 合并后收口补交）。
> 串口通道按 human 指示弃用（SD 卡 KPLOG 为唯一日志证据通道，2026-11-05 裁）。

device-round-evidence（JSON）：{"schemaVersion":1,"changeId":"261104-k0a-four-page-skeleton","sourceCommit":"31dfe585da472da65b92b4b89cd2281022686d0b","binaryRef":"firmware/build/xiaozhi.bin","binarySha256":"ea143517eb342de61c5d76c578983aaf01c7e139c59949eb51a131c7ccf5032c","observations":[{"acceptanceRef":"K0A-E3","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T10:47:30+08:00","note":"human 亲测四步：BOOT 逐页四页循环与回绕（页点 4 点、故事页第 4 实心）；P3 星星积分本占位（页名+等待话术、零数字零进度条）；逐页 KEY 开会话语境正确（积分本不切记录语境）；阅读态键帽「长按退出/翻屏」、KEY 短按双击零响应、长按回列表后列表语义恢复；列表 ×19 计数与 4 屏 16 条为契约 §1.3③ 窗口口径（看屏照片在案）"},{"acceptanceRef":"K0A-E3","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0a-four-page-skeleton/device-evidence/kplog-round3-35b1958f.log","excerptSha256":"89a18c6acb3ff457f1e05ee4acf789b4664e2c66fcf584339c27dcdf0bd7e4d8","segment":"KPLOG022","window":"2026-11-05 10:45:21-10:59:30"}},{"acceptanceRef":"S-F2.1","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T10:47:30+08:00","note":"星星记录本列表回归：×19 计数、行=星星+标题+日期、光标首条带边框、4 屏翻屏与跳屏、×19 为 total 全集契约口径；故事三 story 零回归"},{"acceptanceRef":"S-F2.1","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0a-four-page-skeleton/device-evidence/kplog-round3-35b1958f.log","excerptSha256":"89a18c6acb3ff457f1e05ee4acf789b4664e2c66fcf584339c27dcdf0bd7e4d8","segment":"KPLOG022","window":"2026-11-05 10:45:21-10:59:30"}},{"acceptanceRef":"S-F2.2","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T10:47:30+08:00","note":"逐页 KEY 开会话语境：首页/课表正常开会话、积分本页 KEY 开会话不切故事记录语境（无记录引导误报）；列表 KEY 短按记录双入口语义正常；阅读态短按零响应=TECH-261028 修复真机闭环"},{"acceptanceRef":"S-F2.2","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0a-four-page-skeleton/device-evidence/kplog-round3-35b1958f.log","excerptSha256":"89a18c6acb3ff457f1e05ee4acf789b4664e2c66fcf584339c27dcdf0bd7e4d8","segment":"KPLOG022","window":"2026-11-05 10:45:21-10:59:30"}},{"acceptanceRef":"S-F2.3","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-05T10:47:30+08:00","note":"阅读态：KEY 双击进阅读、键帽「长按退出/翻屏」（F1+F2 修复后）、KEY 短按与双击零响应（不开会话不出语音，S-F2.3 AC2 逐字达成）、KEY 长按回列表"},{"acceptanceRef":"S-F2.3","result":"PASS","method":"kplog","log":{"excerptRef":"delivery/taskpackages/261104-k0a-four-page-skeleton/device-evidence/kplog-round3-35b1958f.log","excerptSha256":"89a18c6acb3ff457f1e05ee4acf789b4664e2c66fcf584339c27dcdf0bd7e4d8","segment":"KPLOG022","window":"2026-11-05 10:45:21-10:59:30"}}]}

## KPLOG 摘录对账要点（excerpt = KPLOG022, 10:45:21-10:59:30）

- 四页推进：`PAGE_SWITCH from=home to=schedule` → `from=schedule to=points_home` → `from=points_home to=story`（:779/:796/:813，trigger=key，10:59 轮）。
- 阅读态键帽与归属（F1+F2 修复闭环）：进阅读后 `KEYBAR page=3 substate=1 last_known=1 left=长按退出 right=翻屏`（:331/:351/:365/:377）。
- **零会话误开**（F2 修复闭环）：摘录全窗 `idle -> listening` / `session_toggle` / `story_key rec_session_slot` 命中数 = 0——阅读态 KEY 短按/双击零响应的日志面证据。
- 首两轮缺陷现场（已修复，留档 pod-events device-round-finding 两条）：F1 = 33809aa2 前键帽 last_known=0；F2 = 33809aa2 轮 `rec_session_slot page=0` 误开录音（KPLOG round2）。

## 附带发现（不随本包闭合，human 2026-11-05 裁「记 debt 后收口」）

1. K0A-E3-F3：列表分屏指示「第 x 屏 · 共 y 屏」真机不可见（`KSTORY_LIST_SCREEN_Y=262` 撞键帽条上沿 262 被裁；story_page.c:1562 同案前科注释在案）——预存缺陷，debt 条目 TECH-261105-p3-list-screen-label-clipped。
2. story_list 16 条窗口外旧篇设备不可见 = 契约既定口径（§1.3③ Q2 丙案）；「全量可及」设计已落在积分详细账本（S-F6.2 keyset 分页，K2 承接）。故事列表若要跟进走需求变更——debt 条目 REQ-261105-story-list-beyond-window-visibility。
