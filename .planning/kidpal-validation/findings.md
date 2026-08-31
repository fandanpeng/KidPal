# kidpal-validation · findings

> 调研发现、风险、偏差。追加式。

- 2026-08-31 ECS 实盘（8.137.91.16）：OpenAI 兼容端点 404 未开、shim 不存在、faster-whisper 与 edge-tts 未装（系统 Python 仅 3.6.8，whisper 需 ≥3.8）、DeepSeek 吐 ` ```display ` 块格式遵循度未实测。MVP 链路主线一块未通。
- UI 侧：260830 验证只实跑 P1 一页 8 枚静态资产 + 眨眼动画；手绘倾斜框、吉祥物其余 6 姿态、网点纹理、全图标族、成就印章、动态时间字体、站酷快乐体——方案已定未出图。
- TP-A 云端（2026-08-31→09-01）：OC-6 命门通过——DeepSeek 稳定吐 ` ```display `（12/12 可解析，失败样本 0）；但 char 查询（"勇字怎么写"）冷启动 ~109s，是 text（4.9s）/story_list（17.9s）的 6~20 倍，瓶颈在 makemeahanzi 查字+笔顺 JSON 生成，M2 须上流式/异步。
- TP-A 云端：ECS 2核/1.8GB 只能跑 faster-whisper tiny；加液/加内存前 base/small 不可行。真 OpenClaw token 只走 loopback，但 `/v1/chat/completions` 随网关 bind=lan 暴露、仅 Bearer 保护，生产须锁 loopback 或防火墙 + 降权。
