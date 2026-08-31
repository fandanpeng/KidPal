# docs-delta — 260831-cloud-link-bringup（功能支撑验证批 TP-A）

> 性质：验证/实证批。不改 story 功能语义、**不触契约面** `docs/contracts/**`、不改 canonical spec（spec 改动走治理）。
> 本文档记录 docs-sync 的 **dist（哪些已改）** 与 **处置（哪些建议 planner/治理/契约门，本批不动手）**。
> 边界注：integrator 的 scope-map 写允许面 = `firmware/** server/** docs/systems/** docs/templates/** docs/ARCH-REVIEW-QUEUE.md changes/dev/** .claude/taskpackages/** .planning/**`——`docs/notes/**` 与根 `.gitignore` **不在允许面**，故 feature-inventory §2 与根 .gitignore 的改动只能"建议 + 留给 planner/主线程"，不能由 integrator 直改。

## A. 已改（本批落地）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| `server/acceptance/.gitignore` | 新增 | 忽略 `_artifacts/` | 冻结验收运行期产物（display 合规 / whisper 转写 / ECS 算力 JSON）逐次落盘、可再生，不入版本控制。根 `.gitignore` 超出 integrator 写面，故用 scope 内的嵌套 .gitignore |
| `.planning/kidpal-validation/progress.md` | 追加 1 行 | TP-A 收口记录 | 跨切片 workstream 收口（dev-loop integrator 第 11 步） |
| `.planning/kidpal-validation/findings.md` | 追加 2 行 | 调研新发现：OC-6 display 12/12、char 冷启动 ~109s、ECS 只能 tiny | 调研归档（追加式，不覆写既有行） |
| `changes/dev/260831-cloud-link-bringup/*` | 新增 | `docs-delta.md` + `review-packet.md` + `cloud-link-validation-report.md` | 本包 closeout/audit 制品（blocking 必交 + CLB-E4 报告） |

## B. 建议 planner/主线程落地（integrator scope 边界外，未直改）

### B1. `docs/notes/feature-inventory.md` §2 —— OC 状态刷新（docs-sync 必须项）

盘点里 §2.2 的 🟡 OC-5..OC-11 已被本批实证，应翻转。精确改动如下（**integrator 不能写 docs/notes/**，请 planner/主线程按此落地）：

| 行 | 现 | 改为 |
|---|---|---|
| OC-5 | 🟡 当前 404（未开启） | ✅ 已开启，`POST /v1/chat/completions` 200 + choices[].message.content（CLB-A2） |
| OC-6 | 🟡 未实测 | ✅ 实测 12/12 出现 ` ```display ` + 12/12 可解析（CLB-A3）；char 查询冷启动 ~109s（见 findings） |
| OC-7 | 🟡 不存在 | ✅ 已建 server/shim.py（288 行薄桥，`/v1/chat /v1/asr /v1/tts /v1/alerts`）（CLB-A7） |
| OC-8 | 🟡 未装 | ✅ 已装 faster-whisper tiny（venv Py3.11.13 + 本地模型），转写正确延迟 ~1.9s（CLB-A5） |
| OC-9 | 🟡 未装 | ✅ 已装 edge-tts，晓晓 mp3 非空可播（CLB-A6） |
| OC-10 | 🟡 未核 | ✅ 实核 2核/1.8GB，whisper 只能 tiny（CLB-A4） |
| OC-11 | 🟡 契约草案已出，未实测 | ✅ 实测 schedule.md 消费正确（当天课程 + view:schedule）（CLB-A8） |

OC-12（cron 提醒出队）、OC-13/14/15（故事）**保持 🟡**——不在本批。

同步更新 §2 引言 blockquote（"shim、whisper、edge-tts、OpenAI 端点目前全都还没就位"已过时）与 §2.3 小结第 1 条（"已打通今天只有 4 条"→ 补本批 7 条）。

### B2. 根 `.gitignore`（可选，consolidation）

根 `.gitignore` 加一行 `server/acceptance/_artifacts/`，与本批的嵌套 `server/acceptance/.gitignore` 合并收敛；integrator 无根 .gitignore 写权，留给 planner。

## C. 建议走治理 / 契约门（本批未动，逐条给改法）

### C1. `docs/spec.md` ——「待装」措辞到期（Track B 常驻轻路径）

| 位置 | 现状 | 建议改为 |
|---|---|---|
| §3.0.1 A1（ASR） | `待装` | `已装（faster-whisper tiny；2核/1.8GB 实测仅撑 tiny，base/small 不可行）` |
| §3.0.1 A2（TTS） | `待装` | `已装（edge-tts 晓晓 zh-CN-XiaoxiaoNeural）` |
| §7「已定 ✅」第 6 条 | `…（faster-whisper + edge-tts，待装）` | `已装`（去"待装"） |
| §7「待办」`【待OpenClaw装】ASR/TTS` | 待装 | 删该条（已装）；小智后端对接、端云接口契约仍保留 |

> 不改原因：spec 是 canonical facts 单源，改动走 Track B 七步闭环，非 integrator 直改面。建议 planner 另起独立治理批收口。

### C2. `docs/contracts/interface-contract.md` —— 走契约门（两件事）

**(a)「待 OpenClaw 确认点」1、2 已被实证**（契约面，只读）：
- 确认点 1（display 块稳定输出）→ 实测 12/12（CLB-A3）。
- 确认点 2（课表 MD 解析/改写）→ 实测通过（CLB-A8）。
- 确认点 3（提醒 cron 可行性）→ 仍未验（OC-12，不在本批），保留。

**(b) 契约缺口（见 review-packet finding #2）**：shim `/v1/*` 请求/响应字段信封未入契约：
- `POST /v1/chat` `{text}`→`{audio(base64 mp3), display}`
- `POST /v1/asr` `{audio(base64 mp3/wav)}`→`{text}`
- `POST /v1/tts` `{text}`→`{audio(base64 mp3)}`
- `GET /v1/alerts` → alerts 出队语义

> 处置：契约面 `docs/contracts/**` 只读，走 **architect-reviewer 四分类 + 人类契约门**，建议 planner 先归档 ARQ。

## D. 无漂移确认

- `docs/contracts/interface-contract.md` 本批只读引用，零写。
- 无新增 `docs/` 根文件、无影子目录。
- 报告/证据落 `changes/dev/260831-cloud-link-bringup/`，不散写。