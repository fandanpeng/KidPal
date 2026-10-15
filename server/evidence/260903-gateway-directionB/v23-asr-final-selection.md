# V23 ASR 终选与 VAD 切句兼容性结论（决策19）

> **_INTEGRATOR LANDING NOTE_**: staged by builder in `server/` (builder scope excludes
> `delivery/taskpackages/`). Integrator must land into `delivery/taskpackages/260903-gateway-directionB/`
> (`v23*.md` or `asr*.md`) for `test_a8_v23_asr_selection_evidence` to go green.

## 结论一句话

**ASR 采用阿里云云 ASR（云优先，决策19），结构面按可声明形状落地；因阿里云凭据不可得，live 实测为已知缺口（human 待办项），不伪造绿灯。**

## 候选评估（V23 首验点：候选流式 + VAD 切句语义兼容性）

- **候选 A —— Qwen3-ASR-Flash**（DashScope `qwen3-asr-flash`，`server/deploy/provider.config.yaml` 已声明候选）：
  - 形态：OpenAI 兼容 dashscope 端点，多语言/歌唱/噪声拒识。
  - 切句：非经典逐句（更偏整段），VAD 切句语义需 `max_sentence_silence` 类参数配合，做流式逐句上屏需验证。
- **候选 B —— Aliyun 实时流式 `aliyun_stream`**（AliyunStreamASR，NLS 实时交互，WebSocket）：
  - 形态：实时流式，业内经典 VAD 切句语义（`max_sentence_silence` 断句静音阈值，默认 800ms），**与契约§1.5「上行 Opus 帧→服务端 VAD 切句→云 ASR」最贴合**。
  - **LIVE 实测缺口**：AliyunStreamASR 需要 NLS `appkey`+`token`（或 `access_key_id/secret`）；Qwen3-ASR-Flash 需要 DashScope `api_key`。本次 ECS 现场**三者凭据均不可得** → live ASR 首验**未执行**，属 human 待办（开通阿里云智能语音交互服务 / 百炼 API key）。绝不伪造。

## 降级链（决策19，二/三级触发）

1. 阿里云流式（AliyunStreamASR）不可用 → 退 NonStream AliyunASI（AliyunASR，更便宜 0.24元/分钟逐句）。
2. 云 ASR 额度用尽/隐私变化 → 扩容 ECS 配置级切换本地 SenseVoice（`fun_local` / FunASRServer，官方服务端自带；本包 provider registry 已声明 ASR 族可插拔）。
3. heavier 更重的兜底：`provider.config.yaml` 的 `downgrade_chain` 已声明 `[aliyun_nonsream, fun_local->sensevoice]`。

## VAD 兼容性记要

- 设备本地 ESP-SR AFE 已做唤醒/AEC/VAD（spec §4.0 本地 VAD 专职喊停/插话）；服务端 VAD 切句用于「整句送云 ASR、整句送对话」（契约§1.5 第2步）。
- 用 AliyunStreamASR 时其 `max_sentence_silence` 承担服务端 VAD 切句，与本地 AFE VAD 是两级，不冲突。
- Qwen3-ASR-Flash 若引入需在网关侧补流式切句/累积语义（VAD 兼容性实测缺 steps 未执行，纳入 ASR 开通后的 live 首验项）。

## 证据 / 动作

- 本文件 = V23 终选结论落盘（A8 证据面，`test_a8_v23_asr_selection_evidence` 消费）。
- **升级项**：阿里云 ASR 凭据开通（human 待办）→ 开通后补 live 流式 + VAD 切句实测，必要时在 `delivery/taskpackages/` 附实测定案。
- 结构面已按可声明形状实现：`server/gateway/provider.py`（ASR 族 provider 注册）+ `server/deploy/provider.config.yaml`（ASR: aliyun 候选流式）+ `server/deploy/apply-xiaozhi-overlay.sh`（AliyunStreamASR 渲染，appkey/token 由 env 注入）。

---

## DashScope qwen-audio 实测（2026-09-04，human 直供凭据）

**执行对象**：模型 `qwen-audio-3.0-asr-flash`（ASR）、`qwen-audio-3.0-tts-plus`（TTS），OpenAI 兼容标准。
**凭据处置（安全纪律）**：key 仅写入 ECS `deploy/secrets.env`（chmod 600），未写入任何仓库文件、未进本报告正文；key 值 114 字符、前缀形态（横线分隔、non-standard）（非标准 DashScope/Model-Studio 账号密钥形态）。

### 实测过程与结果（阻断于端点路由，非建模算法）

1. **准备**：用 edge-tts（venv 3.11）合成一句中文「小朋友，今天天气很好，我们去公园玩吧。」→ `/tmp/kidpal_asr.mp3`（25KB）成功。
2. **ASR 请求（优先 DashScope compatible-mode）**：`POST https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions`，`model=qwen-audio-3.0-asr-flash`，user content 传 `audio_url` data-URI（base64 mp3），`stream=false` → **HTTP 401 `invalid_api_key`**（`Incorrect API key provided`），latency ~0.79s。→ 已确认是**认证拒绝**（非 payload/模型问题）。
3. **端点路由扫描**：对一组候选 OpenAI 兼容基址做 `/v1/models` 认钥匙验（key 从 ECS secrets.env 读出，transmitted 完整——deepseek 错误回显 key 尾 `****drYc` 佐证密码未被截断）：
   - `dashscope.aliyuncs.com/compatible-mode/v1` → 401 invalid_api_key
   - `api.siliconflow.cn/v1` → 401 Token is invalid
   - `api.moonshot.cn/v1` → 401 Invalid Authentication
   - `api.deepseek.com/v1` → 401 Authentication Fails
   - `api.openai.com/v1` → ECS 连不上（ConnectError）
   全部 401，无一把认出该 key。

### 结论（V23 侧）

- **live ASR/TTS（qwen-audio 两模型）实测未能完成**：human 供的 key 在 DashScope compatible-mode 及其余候选 OpenAI 兼容基址均被拒为 invalid。**需 human 提供该 key 的正确 base_url/厂商端点**（该前缀形态非阿里云 DashScope 标准 key，疑似第三方网关/聚合服务）。
- 供 human 拍板换不换 AliyunStreamASR 所需的**实证（延迟量级、流式支持、VAD 切句语义）在拿到正确端点后补充**；本段如实记录阻断，不伪造绿。
- **V23 终选不变（AliyunStreamASR，阿里云云优先，决策19）**；TTS 主选维持 edge-tts（spec 冻结），qwen TTS 仅备案。
- **安全**：key 留于 ECS secrets.env(600)；工作区 `git status` 已核对无 key 泄漏（不回显值）。
- **升级（human 待办）**：① 确认该 key 的正确 OpenAI 兼容 base_url；② 或提供 DashScope/百炼标准 `sk-` key。

### token-plan 端点实测（2026-09-04/05，human 供正确基址后）

**基址**：`https://token-plan.cn-beijing.maas.aliyuncs.com/compatible-mode/v1`（token 套餐专用，非标准 dashscope）。同一 key（读自 ECS secrets.env，114 字符）。

1. **认证通过**：对 `/v1/chat/completions`（ASR）与 `/audio/speech`（TTS）请求均**不再 401**——错误已转为配额/节流（见下），证明该基址认得此 key。此前 dashscope 401 是端点路由错配，已修正。
2. **ASR（`qwen-audio-3.0-asr-flash`）**：edge-tts 合成 `/tmp/kidpal_asr.mp3`（25KB）以 `audio_url` data-URI 喂 `chat/completions` → **HTTP 429 `insufficient_quota`**：`Your token-plan 1-week quota has been exhausted. The quota will reset at 09-07 14:19:00 UTC.`（latency ~0.2-0.8s，认证层通过）。
3. **TTS（`qwen-audio-3.0-tts-plus`）**：`POST /audio/speech` → **HTTP 429 `Throttling.AllocationQuota`**，同上配额耗尽（端点存在、认证通过、仅配额阻断）。
4. **转写正确性 / 延迟量级 / 流式支持 / mp3/wav 输出格式**：**本轮未能实测**——token-plan 该套餐 1 周配额已耗尽，将于 **2026-09-07 14:19 UTC** 重置。配额重置后补发 live 实测（正确性/延迟/是否支持流式，供 V23/VAD 决策）。

### 结论（V23 侧，token-plan 修正后）

- **正确端点已定位**：此 key 走 token-plan 基址（dashscope 401 为错配，不再作为阻断）。**V23 实时收口只欠配额**。
- **引擎 ASR provider 可行性（供 human 拍板，不擅自定案）**：
  - 引擎自带 **`OpenaiASR`（`type: openai`）**：可配 `base_url`+`api_key`+`model_name`，以 multipart `file`+`model` 打 `${base_url}/audio/transcriptions` 形态、解析 `{"text":...}`。若 token-plan 暴露标准 `/audio/transcriptions` multipart 面，则把 `base_url` 指向 `token-plan.../compatible-mode/v1/audio/transcriptions`、`model_name: qwen-audio-3.0-asr-flash` 即可在**不改引擎代码**前提下使用——**需配额重置后 live 验该路径存在与转写正确**。
  - 引擎自带 **`Qwen3ASRFlash`（`type: qwen3_asr_flash`）**：用 DashScope **SDK**（`dashscope.MultiModalConversation.call`）直连，**无 base_url 覆写**，无法直接吃 token-plan 端点（除非加 base_url 支持——引擎改动，属 human 拍板项）。
  - 两 provider 均为 `NON_STREAM`（非流式）；token-plan 的 qwen-audio 若仅 chat/completions 音频 content 形态（非 multipart）也不能被 OpenaiASR 直接复用，则需在网关/引擎侧补一个 token-plan 适配 provider（非流式 / 潜在流式需单独评估，VAD 切句语义待实测）。
  - **引擎 boot 废证**：当前容器 overlay 选 `AliyunStreamASR`（`aliyun_stream`）因无凭据 init 必崩。换到任一云 ASR（token-plan Qwen-audio / Aliyun 流式）且配额可得时即可正常 boot；本地 SenseVoice `model.pt` 缺失则任一处兜不上。
- **V23 终选仍 AliyunStreamASR（决策19 云优先，未变更）**；TTS 主选维持 edge-tts（spec 冻结），qwen TTS 仅备案。换型/引用 token-plan 的决定权在 human（本段只报告可行性）。