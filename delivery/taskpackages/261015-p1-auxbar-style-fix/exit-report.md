# 退出报告 · 261015-p1-auxbar-style-fix

> **status: awaiting-human:live-manual**（真机看屏轮 AB-A7 停门等 human 持板；不自行烧录；合并链由 human 看屏后触发——261015-voice 同款先例口径）

## 一句话

P1 辅助条四个观感问题（wifi 粘连 / 电池失真 / 温湿插画走样 / 日期小写侵蚀）全部按 reviewed plan 修复完成，host 面 + 机械闸全绿，唯真机观感验收（AB-A7 live-manual）停门等 human。

## 机械事实速览（权威在制品，此处为指针）

| 项 | 状态 | 权威制品 |
|---|---|---|
| 红基线 | 双通道实跑：新件四设计态红红名逐字 + 全量 172T/0F 持平 | taskpackage.json redBaselineResult、/c/tmp/p1_auxbar_style_*.log |
| builder 交付 | 178 Tests / 0 Failures（172+6）+ mutation 四族取证还原复绿 | mutation-evidence/builder-run-2026-10-15.md |
| verifier scoped | round 2 绿（r1 红一闸 = mutation-check 形态失配，planner 已修正：perAcceptance 逐字全文 + AB-A4/A6 标 kind:structural） | verifier-evidence/rounds/r1.json、r2.json |
| verifier final | round 4 绿（round 3 红系 planner 传错 scope 文件格式——json 当 txt 消费，fail-closed 拦截，换 txt 后过；round 4 含 firmware-build 闸 Windows idf.py 真编译通过） | verifier-evidence/rounds/r3.json、r4.json |
| supervisor 机械闸 | **APPROVE**（green-channel，provenance v2.0） | supervisor-verdict.json |
| 前置升级闸 | **PASS**（无例外命中，可交 merge-driver） | 闸输出（pod-events 无例外留痕） |
| closeout | docs-delta.md + review-packet.md 双 blocking 制品在位，closeout-check exit 0 | 同目录 |
| commit | 8003f51（分支 dev/261015-p1-auxbar-style-fix，未 push）+ 本次簿记 commit | git log |

## human 持板门清单（AB-A7，真机看屏轮）

1. **烧录 = app + art 双分区**（spiffs 资产变更——ic_wifi_14.bin 重出，261011 先例 / flash-asset-partition-rule；只烧 app 看不到新图标）。
2. **wifi 终形拍板**：修复前 14x14 双弧粘连读作「两朵重叠」→ C2「拉开双弧间隙」provisional winner（host screenshot 比对最优）。
   - **确认 C2** → uiux-author 落真母版（`docs/ui/kidpal-ui-design-v5.html:394`，C2 symbol 逐字在 review-packet §7）→ builder 重跑 gen_assets→gen_art_spiffs 链（bin 应零 diff——provisional winner 已按 C2 重出）。
   - **换选 C1（加粗）/ C3（单弧+点）** → uiux-author 重同步母版 + builder 重跑同链重出 bin → 重烧 art 分区。
   - 注：uiux-author 本 Pod 会话不可派（agent type not found，pod-events d11026fc 留痕）——母版落盘需在可派 uiux-author 的会话执行，或 human 一次性操作如实留痕。
3. **三图标与文字同视觉级**：电池/温/湿图标墨高约 11-15px、中线 y≈19、无硬切感（host 实测：电池 ink 263→58、温湿墨高 18→10）。
4. **日期全大写观感确认**（"TUE 22 SEP"，Tier-2④a human 触点①拍板项）。
5. 修复前后对比件：/c/tmp/wifi_montage.png（四联）、/c/tmp/p1shot_before/、/c/tmp/p1shot_winner/——如需长期留档请顺手定转存本包目录。

## 看屏后路径

- 全部确认 → human 触发合并链（dev INDEX 行 upsert + `merge-driver.mjs --pod 261015-p1-auxbar-style-fix`；前置升级闸已 PASS，无需重跑除非 diff 再变）。
- 有换选/观感回修 → 回 planner 派 fixer 微调 → 重验 → 再停看屏。

## 殿后项 / 遗留（不阻塞本包）

1. requirements.md S-F1.1 AC1 示例措辞「Tue 22 Sep」→ 全大写同步（human 触点①拍板殿后，档 0 勘误或 human 直改如实记录）。
2. docs/ui 母版 ic-wifi symbol 内容级同步（C2）——随看屏终形拍板落（见上门 ②）。
3. 退役大资产本体清退（ic_batt_48 / thermo_man / drop_baby 三件已在 spiffs 白名单无消费方）——SHOULD，真机终形确认后小包批处理。
4. **预存管线失配二件**（非本包引入，builder 确定性自检发现、已还原交付面）：p1_chip_mingzao.bin 与 committed assets.c 失配；verify_art_assets.mjs A9.2 预算报告对账失配（data_size 求和 729078 vs 预算 599698）——建议立 tech-debt；gen_art_spiffs 头注白名单计数陈旧（写 68 实为 75）同族顺手修。
5. loop-retro 阈值触达（14 ≥ 3，closeout-check advisory）——发起责任 = 主线程。

## 派发与循环记录

- 派发：acceptance-author（冻结 + 红基线）→ builder（实现 + mutation 取证）→ integrator（收口 + commit）。uiux-author 派发失败一次（agent type not found，已降级处置）。
- verifier：scoped r1（红，形态修正）→ r2 绿；final r3（红，scope 文件格式传错——planner 操作失误，fail-closed 拦截）→ r4 绿。
- pod-retro.md（第一人称热记忆）已落：同目录。
