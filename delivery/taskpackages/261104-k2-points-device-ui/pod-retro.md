# pod-retro · 261104-k2-points-device-ui

> Pod 收尾复盘（planner 收尾相位制品，closeout advisory 指名）。Pod 形态 = VS Code 扩展
> 会话 planner 内联（human 指示以 planner 形式在本会话跑）；base ref b96b978d；
> pod-events 40+ 事件贯穿（launch 预检五件 → intake → 验收冻结 → builder 三轮 →
> verify 两轮 → integrator 两轮 → 真机轮 + 复验轮 → 修复批四线 → round 3 红闸 → A 案 →
> round 4 绿 → 人批合并）。留档供 loop-retro 沉淀。

## 顺利面（保持）

- **launch 面五件预检 + portfolio handoff 权威消费**：changeId 直启不重归一，K1 留根三件
  逐条评估有落点（真机轮挂接 / 部署轮归主线程 / finding ② 随人批消化），零 blockingDecision。
- **真机轮三源互证纪律**：每场景设备 SD + front journalctl + 引擎 docker logs 同窗对表，
  看屏照片 human 现场供——F1-F11 全部靠三源钉死根因，无一靠猜。
- **双向对偶再次拦灾**：acceptance-author 停报两次拦住 planner 派单失准（ui_art_integration
  已归档、archive 禁改写）；integrator round 3 红闸拦住 planner 自己的数组锚违冻结契约。
  作者≠审者在 planner 落笔面上同样成立——planner 的每一次 manifest 扩单与冻结件重锚都应
  预想「如果有人审我这笔」。
- **修复批四线并行不踩脚**：固件/资产、front/引擎、冻结件重锚、skill 接线分面派单，
  文件面零重叠；manifest 扩单全部走 planner 单点。

## 踩坑与教训（供 loop-retro 提共性）

1. **planner 落笔也会违冻结契约**：drift_guard 数组锚是我为解 apply STOP 的临场改法，
   没先读 test_261017 的冻结契约节（expect 须字符串 unset）——round 3 红闸才抓住。
   **共性**：planner 改任何被冻结件消费的声明/配置件前，先读消费它的冻结件契约节，
   与 builder 改实现前读冻结验收同纪律。
2. **派发前核承载文件现役态**：派 acceptance-author 扩 artt_expected 前没核
   ui_art_integration.acceptance.c 已归档（260915），派单即死件。与 K1 教训「路由语义
   先读权威文件」同型第三次。**共性**：派单目标文件先 `git ls-files` + 读其 README/头注
   现役声明，再写派单路径。
3. **manual/auto 聆听形态是手势语义不是页面语义**：F10 根因排查走了三轮静态分析才落到
   「入口姿势决定形态」；真机日志的 `listen start mode=` 字段一次就分辨。**共性**：
   设备交互类缺陷先抓 wire 字段（mode/restore/takeover），别先读代码猜。
4. **引擎腿注入 ≠ 模型可见**：F1 以为补丁认领即达模型，实测 OpenClaw 脑的指令层靠
   skill 承载（课表先例首句即证据）。**共性**：跨进程「上下文到达」断言必须端到端验
   （模型行为），中间层日志绿不等于语义达。
5. **资产 1bpp 小尺寸需解码肉眼核**：24px 金币星形漏画（svgUse 不注 symbol）靠预览图
   一眼看出；纯尺寸/阈值参数调不出漏画的 symbol。**共性**：新资产入管线必出 8× 解码
   预览人眼过一遍，再上真机。
6. **部署通道的 bundle 纪律**：远程 push = human 边界下，git bundle + deploy.sh --ref 是
   合规单写者通路；drift STOP 的每一次都要先实证「漂移已回流」再走 --first-run，
   本包两次（K0 热修回流、skill 新 dst 首铺）都先对账后放行。

## 流程债与尾差（交主线程/human）

- F8 末屏翻屏回绕待裁（默认保留回绕）；兑换正向归 K4 目标写入器；语音首轮延迟
  NFR-PERF-4 观察项。
- tech-debt 两笔：母版 assets.c 平台复现漂移（ic_wifi_14 / p1_chip_mingzao，ruling 10
  并案，整目录重生成会改存量 bin）+ verify_art_assets.mjs 路径硬编码 C:/AI/KidPal。
- 资产闭集对账验收自 260915 归档后悬空（ruling 13）+ skill/部署登记类闭集 guard 建议
  （fixer 申报⑥）——loop-retro 候选。
- loop-retro 阈值已触（closeout advisory：26 ≥ 3），发起责任 = 主线程。
- manifest 残留已删路径 ic_coin_24_24.bin（pre-cutoff 放行，planner 收尾清理）。
