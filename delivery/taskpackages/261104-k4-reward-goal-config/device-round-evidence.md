# device-round-evidence · 261104-k4-reward-goal-config

> K4G-L1 真机轮（2026-11-15，真实 QQ + 设备看屏）。设备固件 = `2127a19f` 构建，双分区烧录
> （`xiaozhi.bin @0x20000` + `art.bin @0x780000`，双 hash 校验通过，NVS 保留）。三源 =
> 设备 SD 日志（KPLOG）/ front journalctl / OpenClaw 会话日志 + 真实 QQ receipts。

## 一、场景 PASS 台账

| 场景 | 证据锚 | 结论 |
|---|---|---|
| 新增自定义 60 分「看海」 | QQ receipts `target:qq:20261115110400:addsea60`（asset=sc-flag） | PASS |
| 切目标 → 看海 | `target:qq:...110442:switchsea`（balance 26、revision 23）+ 设备刻度 60 | PASS |
| 删除激活中自定义被拒 | QQ 回复列「先切换」；state 未变 | PASS |
| 切回游乐场 | `target:qq:...110628:switchplay` + 设备刻度 40 | PASS |
| 删除非激活自定义成功 | `target:qq:...110807:delsea60`（revision 25） | PASS |
| 5/200 边界 | 4 分被拒、200 分接受（`target:qq:...110849:addboundary200`） | PASS |
| 预置禁删 | 删除巧克力被拒 | PASS |
| 场景标渲染 | 首轮全不画（缺陷，见 §二）；`2127a19f` 修复后复验：游乐场=摩天轮、看海=通用旗、切回摩天轮恢复 | PASS（修复后） |
| 其他页不抢前台 | 切目标时设备停留原页、积分页在屏原地刷新 | PASS |
| 真实 QQ 路由 | main agent `kidpal-points-write` ✓ ready；receipts command_id 形态正确 | PASS |

## 二、设备轮抓到的缺陷与修复

- **场景标 `.bin` 后缀缺失**：`kidpal_points_scene_asset` 按冻结契约返回符号名（`sc_wheel_42_42`），
  渲染层原样传给按文件名读 spiffs 的 `kidpal_art_load` → 加载失败被槽缓存固化，切回也不画。
  fixer 修复 `2127a19f`（槽缓存存文件名副本 + 调用点拼 `.bin`），重烧后看屏复验通过。
- 同族排查：全仓 `kidpal_art_load` 调用点逐一核对，无第二处裸符号名。

## 三、残余与未量化

- RAM 资源预算未单独量化（方案 A 三槽按名重载，RAM 零增长为设计口径；未做实测数值）。
- V9 逐帧 design review 未做像素级比对；human 看屏通过。
- 兑换正向（真实兑现后扣）属 K3 面，本包未重复。

## 四、机器消费行（前置升级闸 deviceOutcome 判据）

device-round-evidence（JSON）：{"schemaVersion":1,"changeId":"261104-k4-reward-goal-config","sourceCommit":"2127a19ffb4835df3c7c5abddb0748f5f0235837","binaryRef":"firmware/build/xiaozhi.bin","binarySha256":"35c71e1637ea632497d57379e1c3d1bf17121514619cc8f66a189423cfbfdde2","observations":[{"acceptanceRef":"K4G-L1","result":"PASS","method":"human-visual","observer":"human","observedAt":"2026-11-15T12:05:00+08:00","note":"真实 QQ 目标配置全过程（新增/切换/删激活拒绝/切回/删非激活成功/4 拒/200 接受/删预置拒）+ 设备看屏刻度 60 与 40、场景标摩天轮↔通用旗↔摩天轮（.bin 修复后复验）、其他页不抢前台；human 收口「可以了，合并吧」"}]}
