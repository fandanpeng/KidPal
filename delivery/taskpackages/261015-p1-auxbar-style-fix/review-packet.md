# Review Packet · 261015-p1-auxbar-style-fix

> integrator · 2026-10-16 · rigor=lite / riskTier=low / sourceType=human-ad-hoc · 等待 supervisor 裁断（不自行判定通过、不 push、不 merge）

## 1. 变更摘要（四观感问题 → 四修复）

human 原话「首页 最上面那一条，整体看起来很不精致」。P1 首页辅助条像素级观感修复，四观感问题对应四修复：

1. **电池/温/湿三图标缩放硬压失真** → 消费切原生直出件：`home_p1.c` 三绘制函数弃旧缩放路径（ic_batt_48 48x48 塞 22x40 非等比盒 / thermo_man 41x67 与 drop_baby 32x51 最近邻硬压 20x20），改 `ic_batt_15.bin`（15x12）、`ic_thermo_12.bin`（12x12）、`ic_drop_12.bin`（12x12）原生直出（dw/dh=原生恒等映射），半区冻结钉区（y2=29）内居中、墨带中线 y≈19 与文字同视觉级；旧资产引号字面消费销账=0。
2. **wifi 图标读作「两朵重叠」** → 14px 重设重出（winner C2 provisional）：gen_assets→gen_art_spiffs 重出链仅 `ic_wifi_14.bin` 一枚 bin 字节 diff（其余 67 枚白名单件字节不变，管线确定性自检过），`assets.c` ic_wifi_14_map 与 png 预览随链再生，消费几何 (12,12,14,14) 零重锚。
3. **日期简写大小写混排参差** → 月份/星期表串全大写（JAN..DEC / MON..SUN，Tier-2④a human 触点①拍板「直接做」），零新资产（imgfont_letter_A..Z 全套在库）。
4. **④b 冒号眨眼** → human 拍板不做出包，以负面锚守卫（AB-A6，门控零触碰、被触碰报 RED(infra)）。

**change size**：小——实现面 4 文件（home_p1.c +36/-29、screenshot 探针 +32/-30、CMakeLists 接线 +6、bin/png 二进制重出 2 枚）+ 新冻结验收件 1 件；无 server 面（crossEnd exempt）、无依赖新增、无跨层调用。

## 2. 验证证据汇总

- **红基线（acceptance-author 双通道，2026-10-15）**：独立冒烟通道 6 Tests 4 Failures，AB-A1/A2/A3/A5 四枚设计态红、红名与冻结申报逐字一致，AB-A4/A6 守卫 case 绿；全量套件 BUILD_EXIT=0 + 172 Tests / 0 Failures，与上游基线（voice 收尾态 172T/0F）持平。日志 /c/tmp/p1_auxbar_style_red_baseline_smoke.log 等 3 件。
- **mutation 四族取证（builder 第一方实跑，2026-10-15）**：m1 ic_batt_15→ic_batt_48 / m2 ic_thermo_12→thermo_man / m3 ic_drop_12→drop_baby / m4 "JAN"→"Jan"，各族 RUNNER_EXIT=1 该族单红、红名 verbatim 留档，还原复绿（终态 6 Tests 0 Failures）；豁免面 AB-A4/AB-A6/AB-A7 口径冻结于验收件头注。证据：`mutation-evidence/builder-run-2026-10-15.md`。
- **screenshot bbox 修复前后对比（builder 独立 gcc harness）**：探针窗口按 p1_home_battery.acceptance.c 冻结钉区重锚（2026-10-15）、新增 wifi 窗、bbox 扫描带收窄到 y[0..29] 排除上虚线分隔带；对比件 /c/tmp/wifi_montage.png、/c/tmp/p1shot_before/diagn_before.log、/c/tmp/p1shot_winner/diagn.log。
- **verifier round 事实**：r1 红一闸（mutation-check 形态失配——当时 AB-A4/A6 按 functional 期望取证）→ planner 形态校正（AB-A4/A6 标 kind:structural，条目原文逐字保留语义零变更）→ **r2 绿**（fact ok:true，observedGateExit=0，全闸绿：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（lite scoped 后置、final 恒跑）/ contract-lint / acceptance-coverage（AB-A1..A7，structural={A4,A6,A7}）/ mutation-check / docs-sync / security）。fact：`verifier-evidence/rounds/r1.json`、`r2.json`。
- **影响面圈定（integrator 终验记账）**：detect_changes（base=main，merge_base=27ad430）——changed_files 与 changeManifest 对账一致（账外仅簿记面 current/ 投影轮换与 .codebase-memory 常驻脏）；inbound blast radius 仅 3 符号：`kidpal_home_p1_render_frame`（渲染帧入口）、新验收模块、hop-2 归档测试模块——与「源锚齿不进调用图、图面影响极小」预期一致。受影响测试集合 = 本包新冻结件 `p1_auxbar_style.acceptance.c` ∪ impactedTestsSeed 四冻结件回归观察面（p1_home_battery / p1_cjk_name / time_sync / voice_rightcol_label，零改动全绿已由 172T/0F 全量套件覆盖）∪ screenshot 独立 harness。**未跑 final bearing verification**：planner 派单未含 final round，firmware-build 闸在本 Pod 尚未实跑（lite scoped 相位后置）——见 §8 升级事项，交 planner 决定 final 相位时机（本包 commit 冻结后 planner 侧跑 verify.mjs --phase=final 或并入合并门前核验）。

## 3. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING） | — | — | — | — | — |
| SHOULD | judgement-call | firmware/main/assets/spiffs_root | 三枚新直出件（ic_batt_15/ic_thermo_12/ic_drop_12.bin）与退役大件（ic_batt_48/thermo_man/drop_baby.bin）在 spiffs 白名单并存，退役大件本体未清（仍被 gen_art_spiffs 重烘） | changed-code hunk：home_p1.c 三消费点已切新件、销账 grep 净，但资产本体留存无消费方；heuristic: 死资产清退宜随真机轮终形确认后一批做（避免本包中途误删候选观察面） | 真机轮终形拍板后开后续小包清退退役资产本体（或 human 裁定保留） |
| NIT | judgement-call | tools/asset-pipeline | gen_art_spiffs 白名单计数头注陈旧（注释称 68 枚实为 75） | changed-code 邻域注释；builder 交付要点已申报、非本包引入 | 顺手修可并入真机轮后续包，不阻塞 |

无 hard-violation。既有冻结件（p1_home_battery / p1_cjk_name / time_sync / voice_rightcol_label）零 diff（git status 实证），回归观察面成立。

## 4. 五轴语义 review（author 五轴，gate 全绿仍完整执行）

- **正确性**：三图标消费切换与验收齿断言同源（源锚 tree-scan 只认可执行源引号字面，注释不假绿）；落位授权域推导（x0 中心±3、y0=13±2）与墨带中线 y≈19 数学自洽；mutation 四族红→还原复绿闭环证明齿的灵敏度。
- **范围**：diff 逐文件与 changeManifest 对账无越界；docs/contracts/**、docs/requirements.md、docs/architecture.md、既有冻结验收件零触碰；④b 面零触碰（AB-A6 负面锚绿）。
- **可测性**：新冻结件 6 case 形态齐（正向齿 + 销账齿 + 守卫齿 + 负面锚），红基线四红与冻结红名逐字一致；守卫 case（AB-A4 扫描器自检齿）兼咬合力自证。
- **可运维性**：探针窗口重锚至冻结钉区权威表，后续对齐轮不再拿旧坐标误判；bin 重出单枚 diff 管线确定性可复核。
- **一致性**：与 261015-voice 源锚齿形态（MCD-A1）、261011 母版同步分权（uiux-author 窄写 docs/ui）先例同构；本包零契约暴露面，无契约语义引入。

## 5. 简化债务台账（`minimal:` 标记扫描）

diff 与新验收件扫描零命中 `minimal:` 标记——无新增简化债务条目。§3 SHOULD/NIT 两项为观感邻域待办，非本包债务标记。

## 6. Residual Risk（decisionCoverage 裁决遗留）

本包 taskpackage.json 无 `decisionCoverage` 字段——无 `landsIn: dropped` 且 `reason: risk-accepted*` 条目。残余风险清单：

- **wifi 终形 provisional**：C2 为 host screenshot 自查 winner，真机墨色/观感可能与 host 渲染有差——真机轮 human 拍板前不落母版（已在 §7 门 ② 承载）。
- **真机观感未验**：本包全部验证在 host 通道（源锚齿 + screenshot harness），真机像素观感（中线、墨色浓度）由 AB-A7 human 持板门承载。

## 7. 真机轮 human 持板门清单（AB-A7，live-manual structural 豁免，停门等 human、不自行烧录）

1. **烧录方式**：app + art 双分区烧录（spiffs 资产变更，261011 先例 / flash-asset-partition-rule）——仅烧 app 分区看不到新 bin。
2. **wifi 图标终形拍板**：确认不再读作「两朵重叠」。三候选：C1（小圆点版）/ **C2（winner，provisional——三弧扇形收敛版，host screenshot bbox 与墨密度比对最优）** / C3（弧线加密版）。确认 C2 → uiux-author 落母版 C2 内容级同步 + builder 对真母版重跑 gen_assets→gen_art_spiffs；换选 → 同链重走。
3. **电池/温/湿图标与文字同视觉级**：墨高约 11-15px、中线 y≈19、无硬切感（旧 22x40/20x20 缩放路径的半截图与五官全毁不复现）。
4. **日期全大写观感确认**：如 "TUE 22 SEP"，大小写混排参差不复现。
5. **修复前后 screenshot 对比图引用**：builder 留档 /c/tmp/wifi_montage.png（wifi 三候选拼图）、/c/tmp/p1shot_before/diagn_before.log（修复前 bbox）、/c/tmp/p1shot_winner/diagn.log（修复后 bbox）——/c/tmp 为仓外临时区，**如需长期留档请转存进本包目录**（human 拍板时顺手定，未转存则随 closeout 记「留档在 /c/tmp，随系统清理可能丢失」）。

## 8. 未尽事项与升级事项

**已知预存失配二件（非本包引入、builder 已还原不修）**：

1. p1_chip_mingzao.bin 与 assets.c 预存管线失配；
2. verify_art_assets.mjs 预算对账失配。

**gen_art_spiffs 头注陈旧**：白名单计数注释写 68 实为 75（NIT，见 §3）。

**升级事项（交 planner）**：① final bearing verification 未在本 Pod 跑——r2 为 scoped 相位事实，firmware-build 闸 lite 后置到 final 恒跑但 final 相位未执行；本 commit 冻结 diff 后交 planner 决定（planner 侧跑 `verify.mjs --phase=final` 或裁定 lite 低风险包按 scoped r2 + 172T/0F 收口口径）。② 真机轮 AB-A7 停门等 human（§7）。③ 殿后两项见 docs-delta.md（requirements AC1 措辞勘误、母版 C2 终形同步）。

**集成执行回报**：detect_changes 已跑（Pod 专属图索引可用，无降级）；本树无跨包冲突、无 rebase、无 Merge-Chain 触发；无 `supervisor-verdict.json` 等机械闸 fs-write 产物待收口（本 Pod 未跑合并闸）。`.codebase-memory/artifact.json` 与 `graph.db.zst` 常驻脏不入 commit（AGENTS.md 纪律）。
