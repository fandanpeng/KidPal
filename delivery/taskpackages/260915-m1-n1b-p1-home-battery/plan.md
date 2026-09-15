# plan · 260915-m1-n1b-p1-home-battery

> workstream：m1-home-qa（M1 开工批，块②P1 新版式 + 电量 ADC）｜ workType：新建型（锚 = S-F1.1 补充规则「辅助条实现现状」差距登记）｜ 设计权威 = docs/requirements.md S-F1.1（线框 WF-F1-01 v1.4 重排序）+ arch §5.5.4

## 背景摘录（锚点逐字，权威 = docs/requirements.md S-F1.1 补充规则）

> 辅助条实现现状（2026-09-13 查证）：**联网小标链路已实现**（基座 `UpdateStatusBar` 每 10s 刷 `GetNetworkStateIcon`，真机可见）；⚠️ **电量百分比真机取不到 = Phase 1 修复项**——硬件有电池 ADC（官方 Demo `03_ADC_Test`：ADC1_CH3=GPIO4 分压，`Adc_GetBatteryLevel` 已验证），但固件三处未接线：板类 `adc1_handle`/`cali_handle`/`vbat_status` 死成员、无 `GetBatteryLevel` override（基座默认返回 false）、渲染节点无电池注入缝（`s_battery_pct` 恒 100）。修复前真机电量恒显示满电、AC3 低电闪烁不触发（P1-A4 系注入假状态验收）。

现状与差距：联网小标已通；电量链路三处断点（板级 ADC 成员 / override / 渲染注入缝）；AC3 低电闪烁、AC4 断网小标随电量/版式补齐一并钉点。

## 改法取舍

- **取舍 A（接线顺序）**：沿「板级 ADC 读取 → `GetBatteryLevel` override → 渲染节点注入缝 → 辅助条显示」自下而上接线，每层独立可测；机制齿 PH-A3 钉注入缝符号级（防渲染侧再拿假数据——P1-A4 注入假状态教训）。
- **取舍 B（数据口径不动）**：首页「接下来」日程数据仍按现口径（云端算好下发 + 三刷新触发），课表数据接首页归 M3 决策 26 链路——本包只动版式与辅助条，不触数据链路（PH-E1）。
- **取舍 C（低电信号形态）**：按 §8 已砍裁定，低电只保留「辅助条电池图标每秒闪、充电即停」最小信号（AC3），不做小伴睡姿演出、不占姿态通道。
- **取舍 D（ADC 硬件不确定性）**：真机 ADC 读数漂移/校准失败的降级预案已前置（PH-E2）：host 机制齿保住 + 真机项改读数偏差记录的结构性验收，不虚报绿——降级不亏证据。

## 阶段划分

1. 板级 ADC 接线 + GetBatteryLevel 实装（参考树 `03_ADC_Test`/`Adc_GetBatteryLevel` 只读重写进自有代码）；
2. 渲染节点电池注入缝 + 辅助条新版式/图标前置（PH-A1/A2/A3）；
3. 低电闪烁 + 断网小标钉点（PH-A4/A5）+ 全量回归（PH-A7）；
4. 真机确认轮（PH-A6，human 持板 ADC 实测）。

## 铁律约束

- `docs/contracts/**` 与 `docs/requirements.md` 只读；发现冲突走 spec-lane 逃生门。
- 参考代码只读：ADC 实现重写进自有代码，不改 `ESP32-S3-RLCD-4.2-Demo/**`。
- 电量端到端 owner = 本包独占（crossPackageBehaviors 已登记），其他包不另接电池数据。
- 已砍项零触碰：小伴睡姿演出（§8）。

## demo 证据形态复述（与 roadmap 一致）

模拟器截图（新版式 + 辅助条图标）+ host runner 输出（相对基线零新增红 + 新齿全绿）+ 真机轮（human 持板：ADC 实测电量、低电闪烁/充电停闪）。
