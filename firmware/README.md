# KidPal 固件（基于小智 XiaoZhi V2.1.0 二次开发）

## 基座来源

本目录 = **小智 XiaoZhi V2.1.0 整树拷贝**（ESP-IDF + C++ 工程），2026-08-29 自 vendor 面复制：

- **干净上游参考（只读，勿改）**：`ESP32-S3-RLCD-4.2-Demo/03_XiaoZhi/XiaoZhiCode_V2.1.0/`
- **我们的工作树（可改）**：本目录 `firmware/kidpal/`

需要 diff「我们改了什么 vs 上游原始」时，拿本目录对 vendor 原树做对比即可。

## 板级 overlay 主战场

- 板级配置：`main/boards/waveshare-s3-rlcd-4.2/`（官方已为本板移植好：ST7305 RLCD、ES8311 放音、ES7210 双麦录音、ESP-SR AFE=AEC+VAD+WakeNet）
- 业务改动（4 页 UI / 会话状态机 / 课表·故事·查字渲染）在 `main/` 下加自有模块，不动 vendor 原树。

## 构建

依赖全部经 **IDF Component Manager** 远程拉取（见 `main/idf_component.yml`，含 esp-sr / LVGL 9.3 / Opus 等），首次构建自动下载到 `managed_components/`（已 gitignore，可再生）。

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p <PORT> flash monitor
```

> 注意：根级 `sdkconfig` 是 vendor 自带的某板配置快照，首次 `set-target esp32s3` 后按 `sdkconfig.defaults.esp32s3` + 板级 Kconfig 重生成。

## 治理约定

- 本目录是**自有实现面**（已登记 `.claude/scope-map.json` owned 段），可写。
- vendor 面 `ESP32-S3-RLCD-4.2-Demo/` **只读**——要改它承载的行为，一律在本目录 overlay 做。
- 实现动作走 dev-loop（主 AI 不直改本面）：planner Pod 派 acceptance-author → builder → verifier↔fixer → integrator → supervisor；唯一例外 = human 明确跳过流程的一次性操作，须诚实记录。
