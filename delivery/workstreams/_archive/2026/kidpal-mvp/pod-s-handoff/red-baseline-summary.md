# Pod S 红基线判定摘要（acceptance-author 2026-09-01 实测，只读扫描得出）

红基线命令（文件落盘后可跑）：

```
cd C:/AI/KidPal-pod-smoke-e2e
python server/acceptance/test_smoke_e2e.acceptance.py          # 结构面（无 SSH/设备）
SMOKE_RUN_LIVE=1 python server/acceptance/test_smoke_e2e.acceptance.py   # + live（需 ECS+SSH）
```

## 逐条判定（结构面）

| check | 判定 | 钉住的缺失行为 |
|---|---|---|
| A1a protocols 有 shim/http 源文件 | RED | `firmware/main/protocols/` 只有 mqtt/websocket/protocol |
| A1b shim 进 CMakeLists SOURCES | RED | build 里没有设备侧 HTTP shim 协议 |
| A2+ 固件内存在 shim 配网承载 | RED | ota/kconfig/cmake/protocols 全无 `shim`/`shim_base`/`device_token` |
| A2− 固件不出现 18789/openclaw | GREEN | 守卫成立（设备不直连 OpenClaw） |
| A3 音频管线有 mp3 解码 | RED | 音频管线 Opus-only，无 mp3/helix |
| E1a 板 overlay target=esp32s3 | GREEN | 基座已在 |
| E1b 16m.csv 分区表 | GREEN | 16MB flash 布局已在 |
| E1c shim 进 build | RED | 冒烟固件尚不可对 shim 编译 |
| E3 protocols/ 零业务 | GREEN | 范围守卫成立 |
| E2/A3/A4 证据目录 | RED | `_artifacts/smoke-e2e/` 不存在（物理冒烟未跑） |
| boundary shim 空文本 400（静态接缝） | GREEN | shim.py 已拒绝 |
| shim mp3 下行接缝 | GREEN | shim 已返回 edge-tts mp3 |

每个 RED 一一对应真实缺失行为；GREEN 项是基座/范围/安全守卫，必须保持绿。

## structural 条目具体化结论

- **SMK-A1** → 两只牙：`firmware/main/protocols/` 出现 shim/http 协议源文件，且被 `firmware/main/CMakeLists.txt` SOURCES 引用。
- **SMK-E1** → 三只牙：板 overlay `config.json` target=esp32s3（绿）、`partitions/v*/16m.csv` 存在（绿）、shim 协议进 CMakeLists（红）。
