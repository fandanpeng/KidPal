# kidpal-validation · progress

> 执行日志（追加式）：改了什么、测了什么、失败/修复。

- 2026-08-31 human 口头立项（"做一个架构验证或功能支撑验证的 taskpackage"），portfolio-planner 拆 3 个独立 TaskPackage（A/B/C）。
- 2026-08-31→09-01 TP-A 260831-cloud-link-bringup 收口（integrator）：9 验收功能面全过（CLB-A1 结构 + A2~A8）；命门 OC-6 display 遵循度 12/12、JSON 可解析 12/12、失败样本 0；ECS 2核/1.8GB 判 faster-whisper 只能 tiny（base/small 不可行）；whisper 短句转写 1.92s；edge-tts 晓晓 mp3 可播；shim `/v1/chat` 一路通。冻结验收复跑 8/9——CLB-A3 char 查询冷启动 109s 踩爆 90s curl 超时（伪红，格式遵守已独立复证 12/12）。两条必载 finding：Bearer-only operator 端点暴露（验证批接受，生产锁 loopback/降权）、shim `/v1/*` 字段契约缺口（走契约门）。
- 2026-08-31→09-01 TP-C 260831-simulator-lvgl-align 收口（integrator + planner 补 contractRefs 使承重闸转绿）：模拟器 9.5→9.3 与固件同版、1bpp 阈值化快照正式化、P1 8 资产回归无差异、实验代码（kidpal_ui.c/gen_assets.mjs/vcxproj）由 dev-loop 接管。资产数据旧阈值 128 的错位 defer 到 TP-B 统一重生成。
- 2026-08-31→09-01 TP-B 260831-ui-art-assets 收口（planner 主线程跑完整 dev-loop：acceptance-author→builder→verifier↔fixer→integrator→supervisor）：本仓首个真跑完机械 verifier 承重轮 + supervisor 机械闸的包。47 枚资产全量出图（RGB565、LUM_THRESHOLD=127 单一源）+ 预算机判：599,698 字节已出 / 预计总占用 ~3.85MB vs 8M / margin 54.1% / 不触发 I1。supervisor-verdict = APPROVE。merge-readiness 暴露 server 面 live 验收假红 → human 授权快速补 G3 闸 host/live 区分（跳过流程、已记 ARQ-260901-g3-server-live-acceptance）。
- 2026-09-01 docs-sync 收尾（主线程）：feature-inventory §1.2/1.3/1.5 翻 ✅、§2.3 对接小结改批后实况；新建 docs/notes/validation-batch-conclusion.md 作批级汇总（PROPOSAL §3 要求的《V3 设计输入结论》＋《云端 MVP 链路打通证明》）。
- 2026-09-01 **本批合入 main**：feature 分支 `planning/kidpal-mvp-base` 快进 8 提交入本地 main（无 remote、非直推）。三包（TP-A/B/C）至此全部落地；workstream 归档 done。
