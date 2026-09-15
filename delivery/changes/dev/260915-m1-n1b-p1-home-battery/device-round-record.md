# Device Round Record — 260915-m1-n1b-p1-home-battery（PH-A6 真机确认轮）

> integrator 产出 · 2026-09-16 · **状态：挂起（SUSPENDED）——PASS 裁定归 human，未裁**。本件 = 已捕获证据与两轮迭代的互证记录，**不含伪造 PASS 结论**。挂起到 2026-09-17 晨（human 口令，pod-events `372601a6`）。

## 环境与烧录

- 板：waveshare-s3-rlcd-4.2 真机，human 持板；串口 COM5；烧录口令「现在可以烧录」（human decision `1c42781c`，板就绪 COM5）。
- 烧录两轮：**首版**（scoped r2 全绿后整编产物）与**修复版**（fixer r1 字形缩放补丁 + fixer r2 图标缩放补丁后整编产物）；agent 侧烧录 + 被动串口捕获，human 持板执行动作点。
- 捕获证据（本目录）：
  - `device-round-boot-capture-20260916.log`（首版，228 行）；
  - `device-round-fix-boot-capture-20260916.log`（修复版，193 行；源 = `c:/tmp/n1b_fix_boot.log`，integrator closeout 时拷入）。

## 已捕获真机证据（PH-A3 端到端实证）

两份捕获日志中同径命中两条承重行：

1. `I (272) adc_battery_estimation: Use external ADC handle` —— 板级外部句柄构造重载生效（`adc_battery_monitor` 死成员 `adc1_handle/cali_handle/vbat_status` 接线成功，基座自建 ADC 路径已被替换）。
2. `I (1222) waveshare_rlcd_4_2: battery_inject corr_id=kdp-1222 level=95 charging=0 vbat_ok=1` —— 板级注入缝端到端：**真电量 95（非恒 100）**、未充电、vbat 有效，correlationId 链路（kdp-1222）完整。`s_battery_pct 恒 100` 现状销账的真机实证。

修复版日志另证渲染节点正常起页：`kidpal_rn: render home 400x300 render_rc=0 state_rc=0`、`show_home: LOCAL_PAGE front (layer top=2)`。

## 真机迭代记录（两轮 fixer）

- **首轮**：human 持板发现辅助条 48px 字形溢出（imgfont 真字形在辅助条小钉区超出槽位，全元素变超大字体；pod-events `1514491d` friction 在册）→ fixer r1 缩放 blit 修复（`17ae87ad`）。
- **二轮**：human 持板发现温/湿/电图标资产裁半——`thermo_man` 41×67 / `drop_baby` 32×51 / `ic_batt_48` 48×48 被硬裁进 20~22px 槽位 → fixer r2 缩放修复（`ae567f27`）。
- 两轮 host 复跑 + scoped verifier（r4/r5）全绿后整编修复版重烧，即捕获到上节修复版证据。

## human 已裁事项

- **PH-E2 降级（低电闪烁真机项）**：human 裁定 PH-A4 真机项按 PH-E2 降级预案执行 = host 机制齿 + 真机读数偏差记录承载，不虚报绿（human decision `4c10c620`，主线程 AskUserQuestion 留痕可引）。低电闪烁/充电停闪的真机可见性确认由 host 齿（PH-A4 两条 mutation 敏感齿）承载。
- **真机轮挂起**：human 口令「你先开发着吧，明天早上再帮你看」（human decision `372601a6`，2026-09-16 晚）——挂起到 2026-09-17 晨。

## 待 human 回归事项（2026-09-17 晨）

1. **看屏复验**：修复版版式（辅助条字形/图标缩放后）真机观感确认。
2. **充电捕获**：插线供捕获窗（充电数据线与 COM5 同线已拔，需重插）——验证 charging=1 路径真机读数。
3. **断网替代方案**：human 与主会话共用网络、路由不可关——候选方案 = 路由器 MAC 隔离踢下线（PH-A5 断网态真机钉点）。
4. **观感裁定与 PH-A6 PASS 裁定**：ADC 实测电量值合理性 + 整体真机确认，**PASS 归 human 裁**（合并门 blocking 项）。

## PASS 裁定空位

```
PH-A6 PASS 裁定：（待 human，2026-09-17 晨回归后填）
裁定人 / 时间 / 口令锚（pod-events eventId）：________
```

- 裁定 PASS 前，本包合并门停等（与 planner 移交口径一致）；裁定留痕入 pod-events 后由 planner 主线程走合并门流程。
