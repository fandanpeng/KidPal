# device-round-evidence · 261104-k5-story-cover-nav

> K5S-E4 真机轮（2026-11-15 晚，四轮看屏 + 三源互证）。设备固件 = source `ca5fa9e7`（含真机轮全部修复）
> 构建，app 分区 `0x20000` + art 分区 `0x780000` 双分区烧录，双 hash 校验通过，NVS 保留。
> 三源 = 设备 SD 日志（KPLOG）/ ECS front journalctl / human 看屏。**本次轮内抓到 3 个缺陷并当轮修复重烧，
> 下表只记最终固件（`7f495a28…`）上的通过项。**

## 一、最终轮 PASS 台账（human 看屏 + KPLOG 互证）

| 场景 | 证据锚 | 结论 |
|---|---|---|
| 封皮四件（书封 + 「星星记录本」+ 星星计数 ★×19 + 键帽「打开 / 下一页」） | human 看屏第四轮确认「都 OK 了」；KPLOG `23:04:48 STORY_RENDER page=7` | PASS |
| 四点页点第 4 实心 + 覆盖层页点隐藏 | human 看屏；四点按 v1.71 版式逐轮目检 | PASS |
| BOOT 四页并集 P1→P2→P3→P4 | KPLOG `23:04:43 / :44 / :48` 三段 `PAGE_SWITCH`（home→schedule→points_home→story），落 P4 即封皮 | PASS |
| 封皮 KEY 短按进列表 / 封皮 BOOT 回 P1 / 封皮 KEY 长按回首页 | human 看屏 + KPLOG `23:05:25 PAGE_SWITCH from=story to=home trigger=key` | PASS |
| 逐层返回（阅读→列表→封皮→首页，每按一次只上一层） | human 看屏（列表长按回封皮、阅读长按回列表）；KPLOG `STORY_RENDER page=1→0→7` 序 | PASS |
| **语音「打开故事」只到封皮、不跳列表** | KPLOG `23:05:37 story_list_dispatch items=16 counter=19 degrade=false`（非 restore 导航帧）→ 同秒 `STORY_RENDER page=7` → `23:05:44 PAGE_SWITCH from=story to=story trigger=net`；human 看屏确认切到封皮 | PASS |
| 后台恢复不抢前台（restore 帧只填缓存） | KPLOG `23:05:50 story_list_gate defer_front page=7 voice_front=0 restore=1`（restore 帧被 defer，页面不跳） | PASS |
| 封皮计数 = 现存篇数、删除同步减 | human 看屏（★×19 与现存篇数一致）；计数源 = `story_list.total`（变异证据 KCN-C10 钉住） | PASS |
| 封皮观感 vs V9 母版（书封/书脊/星徽/小伴/涂鸦/★×N/标题） | human 四轮看屏，第四轮「都 OK 了」 | PASS |

## 二、本轮抓到的缺陷与修复（如实记录，均当轮修复重烧复验）

1. **front 未部署**：首轮语音「打开故事」不切页。查 ECS `/opt/kidpal/front/front/front_app.py` 无 `_expand_story_nav_intent`（旧版），薄载荷被旧门拒。→ 部署 K5 front + display_schema 并重启 `kidpal-front`（先 `.bak-20261115-k5` 备份）。
2. **导航帧被 VOICE 前台吞**（部署后仍不切页）：KPLOG `story_list_dispatch degrade=false`（restore=0）后紧跟 `story_list_gate defer_front page=7 voice_front=1 restore=0`——K3 的「VOICE 前台即 defer」判据把显式导航帧连坐吞掉。→ 修复 `mcp_server.cc`：非 restore 完整帧 = 显式导航帧，按到达时页态判 `story_nav_takeover`，不再看 voice_front；录音态页保护保留（commit `54dd605e`）+ 回归断言折进 KCN-C5 tooth 5（commit `a467bd39`）。
3. **封皮视觉与 V9 母版不一致**（三轮看屏逐步收口）：首轮缺小伴/星徽比例错/缺涂鸦；二轮书封几何偏大偏左、书脊位置与长度错、右上 ★ 过小、云压书封；三轮 ★ 与数字叠角、标题偏低偏左。→ 按 `docs/ui/V9/story-cover.png` 逐像素测量重落位（commit `54dd605e` + `ca5fa9e7`）。

## 三、残余与未量化

- **风险 A（architect 复核提出）**：归档失败离页帧（restore=False 例外）到设备按非 restore 导航帧落**封皮**而非列表——本轮机读断言只钉 restore 位；行为观感归后续观察，未在本轮构造整理失败场景。
- **风险 B**：restore 在 READER/RECORDING/STAMP 驻留时可能多一次基层重绘（页态零重置已机检），本轮未构造该时序，闪帧与否未量化。
- **风险 C（已闭环）**：封皮缺「小伴」→ 已补（pose_daily_96 等比缩放落位），human 看屏确认。
- 跨包联合收口（K5S-E5：整列车真机 + 真实 QQ 端到端）不属本包，归主线程另组织。

## 四、机器消费行（前置升级闸 deviceOutcome 判据）

device-round-evidence（JSON）：{"schemaVersion":1,"changeId":"261104-k5-story-cover-nav","sourceCommit":"ca5fa9e7e97cfee24bfcee13464f53946b899a4a","binaryRef":"firmware/build/xiaozhi.bin","binarySha256":"7f495a2862f7d7e02606024f36e0645bcbc0e7442feeb774763f0c8331cccbc6","observations":[{"acceptanceRef":"S-F2.6-AC1","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-15T23:06:00+08:00","note":"封皮四件齐（书封 207×183 / 标题「星星记录本」/ ★×19 计数 / 键帽 打开·下一页），四点第 4 实心；与 V9 母版逐项对照后 human 确认「都 OK 了」；KPLOG 23:04:48 STORY_RENDER page=7 互证"},{"acceptanceRef":"S-F2.6-AC2","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-15T23:06:00+08:00","note":"封皮 KEY 短按进列表、BOOT 回 P1、KEY 长按回首页均按 v1.71 矩阵执行；KPLOG 23:05:25 PAGE_SWITCH from=story to=home trigger=key 互证"},{"acceptanceRef":"S-F2.6-AC3","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-15T23:06:00+08:00","note":"阅读→列表→封皮→首页逐层返回，每按一次只上一层；KPLOG STORY_RENDER page=1→0→7 序互证"},{"acceptanceRef":"S-F2.6-AC4","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-15T23:06:00+08:00","note":"语音「打开故事」只到封皮不跳列表：KPLOG 23:05:37 非 restore 导航帧到达即 STORY_RENDER page=7，23:05:44 PAGE_SWITCH trigger=net 切 P4；human 看屏确认"}]}
