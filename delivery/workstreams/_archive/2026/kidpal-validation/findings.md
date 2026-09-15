# kidpal-validation · findings

> 调研发现、风险、偏差。追加式。

- 2026-08-31 ECS 实盘（8.137.91.16）：OpenAI 兼容端点 404 未开、shim 不存在、faster-whisper 与 edge-tts 未装（系统 Python 仅 3.6.8，whisper 需 ≥3.8）、DeepSeek 吐 ` ```display ` 块格式遵循度未实测。MVP 链路主线一块未通。
- UI 侧：260830 验证只实跑 P1 一页 8 枚静态资产 + 眨眼动画；手绘倾斜框、吉祥物其余 6 姿态、网点纹理、全图标族、成就印章、动态时间字体、站酷快乐体——方案已定未出图。
- TP-A 云端（2026-08-31→09-01）：OC-6 命门通过——DeepSeek 稳定吐 ` ```display `（12/12 可解析，失败样本 0）；但 char 查询（"勇字怎么写"）冷启动 ~109s，是 text（4.9s）/story_list（17.9s）的 6~20 倍，瓶颈在 makemeahanzi 查字+笔顺 JSON 生成，M2 须上流式/异步。
- TP-A 云端：ECS 2核/1.8GB 只能跑 faster-whisper tiny；加液/加内存前 base/small 不可行。真 OpenClaw token 只走 loopback，但 `/v1/chat/completions` 随网关 bind=lan 暴露、仅 Bearer 保护，生产须锁 loopback 或防火墙 + 降权。
- TP-C 模拟器（2026-09-01）：模拟器 9.5→9.3 与固件同版（`lv_version.h` 三号一致 + MSBuild exit 0），阈值化快照收编为 `kidpal_snapshot` 工具——这是 TP-B 目测结论「≈真机」可信的地基。residual：已提交 `assets.c`/`png` 仍是旧阈值 128 产物、脚本已统一 127，defer 到 TP-B 统一重生成（后 TP-B 已重出 47 枚、阈值单一源落定）。
- TP-B 资产（2026-09-01）：V2「方案已定未出图」的美术元素全量出图 + 机判转绿——**无 🔴 走不通项**；姿态族中立化 0°（倾斜回归页面层、避免换姿态+换倾斜双跳位）；图标族取舍 = 离线位图（14 枚一次性绘形），imgfont 只留给 B7 动态时间数字；站酷快乐体授权落档（SIL OFL 1.1、子集化重分发保守按改名规避 RFN 禁则）。
- TP-B 预算（2026-09-01）：已出图 47 枚真实 599,698 字节；+ P2–P4 预估后设备总占用约 3.85 MB vs SPIFFS assets 8M，**margin 54.1%、不触发 I1**。唯一逼近预算隐患 = P4 笔顺字盘（每笔每态烘整帧保守可达 ~3.1MB）→ V3 前须锁「笔形 3 态叠加复用 + 米字格单份背景」打包策略。
- 本批零触契约面；两处暴露的缺口登记走契约/治理门未直改：shim `/v1/*` 字段信封未入契约 §九（ARQ-260901-shim-api-contract-gap）、G3 闸 server 面 host/live 规范待 architect 固化（ARQ-260901-g3-server-live-acceptance）。
- 遗留待 human/真机：① 框族 255px 大半径角 24px 角片忠实性、-8° 印章内圈 2.5px 虚线断口，归 OQ-3(a) 目测裁定；② I1 真机可用性 + 16px CJK 真机可读性（定 V3 字号下限）留真机联调期。
