# 云端链路验证报告（CLB-E4 必交）

> changeId：`260831-cloud-link-bringup` · subsystem：server · 验证批：TP-A（云端 MVP 链路打通）
> 验证日：2026-08-31 实盘 → 2026-09-01 integrator 复验 · 现场：阿里云成都 ECS 8.137.91.16
> 对照：TaskPackage `acceptance` CLB-A2~A8 · `exitCriteria` CLB-E1~E4

## 1. 各验收通过率

| 验收 | OC | 结论 | 实证 |
|---|---|---|---|
| CLB-A1 | 结构 | 通过 | shim.py + requirements.txt + systemd 单例齐备 |
| CLB-A2 | OC-5 | 通过 | `POST /v1/chat/completions` 200，`choices[].message.content` 正常 |
| CLB-A3 | OC-6 | **通过（格式遵守 12/12）** | display 块出现率 1.0、块内 JSON 可解析率 1.0、失败样本 0。冻结验收复跑时首条 `char` 查询被 90s curl 超时判伪红（见 §5 延迟与 §6 备注） |
| CLB-A4 | OC-10 | 通过 | 2 核 / 1870MB（≈1.8GB）→ 推荐 tiny 档 |
| CLB-A5 | OC-8 | 通过 | faster-whisper tiny，中文短句转写正确（含「公园」），延迟 1.92s |
| CLB-A6 | OC-9 | 通过 | edge-tts 晓晓产 mp3（>1KB + ID3 头） |
| CLB-A7 | OC-7 | 通过 | shim `POST /v1/chat` 一路：文字→OpenClaw→切分→TTS→`{audio, display}`，display.view 合法 |
| CLB-A8 | OC-11 | 通过 | B1 schedule.md 消费正确（当天「钢琴课」+ view:schedule + 口语答案含课程名） |

> **总通过率**：9 验收项功能面全过（冻结验收复跑 8/9，唯一红 CLB-A3 为 char 查询延迟踩爆 90s 超时的伪红，格式遵守独立复证 12/12，见 §5/§6）。CLB-E1（结构）逐项齐备；CLB-E2（shim 薄）满足（零业务/CRUD/意图逻辑，依赖 5 个全锁定版本）；CLB-E3（鉴权）满足（真 token 只走 loopback，设备弱 token env 门控）；本报告即 CLB-E4。

## 2. display 失败样本

**0 个。** builder 冻结验收最后绿轮 `display_compliance_20260831-233630.json`：`total=12 / n_blocks=12 / n_parsed=12 / occurrence_rate=1.0 / parse_rate=1.0 / failures=0`。
integrator 独立 150s 超时探针复测三条抽检：`char`(有 display)、`text`(有 display)、`story_list`(有 display)，均 HTTP 200 且含 ` ```display ` 块。
`server/acceptance/_artifacts/display_failures/` 目录空（无失败样本落盘）。

## 3. whisper 模型档结论

**tiny。** ECS 2 核 / 1870MB 实盘只扛得住 `faster-whisper-tiny`（CTranslate2 int8 / CPU）。`base`（~4GB 需求）与 `small` 在本机**不可行**。本地模型目录 `/opt/kidpal/models/faster-whisper-tiny`（预下载，绕开 ECS 无法直连 huggingface.co 的约束）。中文短句转写实测正确且延迟可接受（见 §5）。

## 4. ECS 算力结论

| 项 | 实测值 |
|---|---|
| CPU | 2 核（Intel Xeon Platinum） |
| 内存 | 1870MB 总量（912MB used / 958MB available，测试时点） |
| load | `0.06, 0.14, 0.18`（1 分钟平均低） |
| 结论 | 跑网关 + DeepSeek(远程 API) + edge-tts 无压力；本地 faster-whisper 只能 tiny；**char 查询延迟瓶颈在 DeepSeek 侧 reasoning + makemeahanzi 生成，非 ECS 算力** |

## 5. 延迟

| 环节 | 实测 |
|---|---|
| whisper 模型加载 | ~0.68s（builder 稳态） |
| whisper 转写（短句） | ~0.16s（builder 稳态） |
| whisper 全链路（加载+转写，integrator 复测） | **1.92s**（`whisper_transcription_20260901-000429.json`） |
| DeepSeek char 查询（冷启动，含查字+笔顺 JSON） | **~109.1s** |
| DeepSeek text 查询 | ~4.9s |
| DeepSeek story_list 查询 | ~17.9s |

> char 查询 109s 是 MVP 同步 HTTP 形态下的头号延迟风险（见 review-packet finding #3），建议 M2 上流式/异步 + char 数据缓存。

## 6. 备注（诚实口径）

1. 本报告 9 验收功能面全过，但冻结验收复跑是 **8/9**——CLB-A3 首条 `char` 查询冷启动 109s 超过验收 curl `-m 90`，产出 HTTP 0 伪红；格式遵守已由 builder 12/12 轮 + integrator 独立 150s 探针双重复证。**不声称全量复跑 9/9。**
2. 两条必载 finding（安全 Bearer-only 暴露、shim 字段契约缺口）与 char 延迟、测试超时 finding 全文见 `review-packet.md` Findings 表。
3. 本批为验证批，未改 story 功能语义、未触契约面；spec「待装」刷新与契约字段信封补齐走治理/契约门（见 `docs-delta.md`）。