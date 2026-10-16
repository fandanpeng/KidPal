# CONTRACT-CHANGE-PACKET · 260902-arch-review-shim-api-field-envelope

> Gate: ARQ-260901-shim-api-contract-gap（260829-shim-skeleton 包 1 launch 前置闸 B）
> Date: 2026-09-02
> Status: PRECHECKED（architect-reviewer 四分类 = 独立契约变更，human 未签字）
> 变更面：docs/contracts/interface-contract.md 拟新增一节的 shim /v1/* 请求/响应字段信封 schema
> 落盘说明：本 packet 由 architect-reviewer 产出（全文如下），architect-reviewer 在 scope-map 中无写槽（deny `**`），由主线程按 .claude/references/contract-change-evidence.md 要求代为落盘，内容逐字保真。

## 1. 变更范围（Gate Scope）

In-scope（单一新增节，编号 human 定；建议 §四，勿沿用错误的「§九」）：
- 四端点 POST /v1/asr、POST /v1/chat、POST /v1/tts、GET /v1/alerts 的请求/响应 JSON 形状
- 音频编码（核心待拍点：base64 mp3 vs Opus/24kHz）
- 设备鉴权（Authorization: Bearer 设备弱 token + 401 语义 + token 未设时鉴权是否允许关闭）
- 错误信封 {error:{category,fingerprint,failingStep,message,logTail}} + X-Correlation-Id 头
- /v1/alerts 出队/清除语义（补齐契约 §三 line 218 已声明、impl 未实现的「清除已下发项」）

Explicitly out-of-scope：
- E1 display 块 / 各 view 类型 / B1 课表 MD / D3 笔顺 JSON / 提醒文件 content shape —— 全部不动
- shim 职责边界（§1.5 薄桥红线，~200 行、零业务逻辑）——本新增只定传输信封，不引入任何业务逻辑
- OpenClaw /v1/chat/completions 的调用侧形状（那是 OpenClaw 自有 API，端云边界外）

## 2. 提议 diff（proposedContractDiff —— shimApi 信封，草案）

公共：
- 鉴权：所有 /v1/* 要求 `Authorization: Bearer <SHIM_DEVICE_TOKEN>`；缺失/错误 → 401 + ErrorEnvelope。
  （待拍 A：SHIM_DEVICE_TOKEN 未设时，契约应否规定「鉴权必须强制开启」，还是保留 validation-mode 关鉴权？）
- 错误信封统一 `{error:{category,fingerprint,failingStep,message,logTail}}`，响应头带 `X-Correlation-Id`（请求可 echo）。
- 音频编码（待拍 B，见 §5 human question）：当前 impl = base64(mp3)，spec §6.1/§9 提示 Opus/24kHz。

```
POST /v1/chat   req {text:str}         -> 200 {audio:<base64>, display:{view,data,speech_hint?}}
                                       -> 400(empty text) / 502(OpenClaw|TTS fail)
POST /v1/asr    req {audio:str base64} -> 200 {text:str}
                                       -> 400(bad base64|empty) / 502(whisper fail)
POST /v1/tts    req {text:str}         -> 200 {audio:<base64>}
                                       -> 400(empty) / 502(edge-tts fail)
GET  /v1/alerts req 无体               -> 200 {alerts:[{id,fire_at,title,speech,view}]}
                                       -> 出队语义：读后清除（impl 需补齐，待拍 C：读即清 vs ack 后清 vs 幂等）
```

## 3. 四分类判定

② 独立契约变更（CONTRACT_CHANGE_REQUIRED）。

反证检查（为何不是 ④ / ③ / ①）：
- 非 ④ 无需改：不存在能表达它的现有契约字段，也塞不进 display 块 `data` 载荷（这是 HTTP 传输层，非 view 载荷）；不是设备端本地渲染细节（固件端消费方需按信封编码/解码）；无 canonical 名可映射。spec §9 line 575 明确承认「请求/响应字段」是开放问题——SSOT 缺口为真，固件（包 1 及后续所有包）需要冻结信封才能动工。
- 非 ③ spec↔contract 漂移：漂移要求「已批准 spec 与契约相互矛盾」。此处 spec §6.1 语义表与契约 §1.5 对四端点语义无矛盾（都为 audio→text / text→reply+display / text→voice / poll）；字段细节是 spec §6.3/§9 明确 defer（待定/开放）、契约缺失——「缺失 + 显式延后」≠「矛盾」。
- 非 ① 随 spec 改动携带：① 指的是「spec 正在改、契约同 PR 跟改」。此处没有任何需要先行改动的 spec 编辑来「携带」这次契约新增；将来把 spec §6.3/§9 的「音频格式/鉴权/字段」从待定移到已定，是本次契约裁决的后果而非载体。契约新增自洽独立。

## 4. 影响（Impact）

- additive/formalizing：四端点在 main 的 shim.py 已存在此形状，本新增是把 de-facto 形状正名为契约。对契约本身是纯新增、零删除。
- 唯一可能的 breaking 点：若 human 选 Opus（弃 impl 现 mp3），则需改已并入 main 的 shim.py + 冻结验收的音频格式假设——按「Landed 后 impl 对齐」处理，非契约破坏。
- 消费方：固件（编码/解码/渲染/轮询）为核心消费方，必须按此冻结信封实现；shim（实现）；OpenClaw（经 /v1/chat/completions 间接，不直接消费信封）。
- 已跑通链路影响：/v1/chat 全链路、D3 笔顺（makemeahanzi）零影响——本变更不触 E1/D3。

## 5. 给 human 的契约门提问（三条）

1. **（批准面）** 是否批准在 `docs/contracts/interface-contract.md` 新增一节，正式定义 shim `/v1/asr /v1/chat /v1/tts /v1/alerts` 的请求/响应字段信封 schema（请求/响应 JSON 形状 + 错误信封 + X-Correlation-Id + 设备鉴权 + alerts 出队语义），且除该节外契约零改动。批准 / 拒绝？
2. **（关键技术取舍）音频编码**：设备↔shim 走 **base64 mp3 over JSON**（当前 impl 已通、edge-tts 原生输出 mp3、设备端 mp3 解码已经 Pod S 实证）还是 **Opus/24kHz**（小智原协议原生格式、更小体积，但 shim 需转码、固件端需 Opus 解码，spec §6.1/§9 曾提示此方向）？三者（impl mp3 / spec §6.1 表 Opus-mp3 混写 / spec §9 待定）现在互相打架，需统一。
3. **（安全边界）设备鉴权语义**：契约是否规定「生产必须强制 `SHIM_DEVICE_TOKEN` 鉴权、不设即拒绝服务」，还是保留 impl 现「token 未设即关鉴权」的 validation mode？`/v1/alerts` 的清除语义 = 读即清 / ack 后清 / 幂等重读？这两点同时钉死设备弱 token 这一儿童隐私安全边界。

## 6. 验证计划（落地时）

- 契约 diff 逐字核对：仅新增 shim /v1/* 信封节，E1/B1/D3/§三 content 零 diff。
- JSON 样例合法性 + 对 shim.py 现形状逐字段复核（JSON Schema 或人工逐字段）。
- /v1/alerts 出队实现补丁验收（SHM-A5）+ 401 门回归（对齐 ARQ-260902-shim-401-gate-bypass）。
- spec↔contract 机械门本仓仍缺（ARQ-260827-contract-mech-gate-gap），降级人工核对并如实记录。

## 7. 消费方一览

固件（渲染/音频/轮询，主消费方）· shim（实现）· OpenClaw（间接）· 设备鉴权（安全边界消费者）

## 附：措辞纠正（预审发现）

ARQ-260901-shim-api-contract-gap 与 kidpal-mvp PROPOSAL §4/§7 写「未入契约 §九」「§九为拟新增节」系笔误——契约里没有 §九（章节止于 §三），真正的「§9 开放问题」在 `docs/spec.md` §9。落盘与 CDQ 登记统一用「拟新增契约节（建议编号 §四）」。

## 附：核心事实引证（预审核实）

- 契约缺信封：`docs/contracts/interface-contract.md`（§一 E1 / §二 B1 / §三 提醒 line 202-218 / 待确认点 line 222-226；全文无 shim /v1/* 字段信封节；§1.5 line 136-142 只描述 shim 职责流程，不定字段形状）
- spec 承认开放：`docs/spec.md` §9 line 575、§6.3 line 518-521（音频格式/鉴权标「待定」）、§3.0.0 line 79（TTS→mp3）、§6.1 line 507/509（ASR→Opus / TTS→mp3/Opus）
- 实现事实形状：`server/shim.py`（ChatIn/AudioIn/TtsIn + 四路由 + require_device 鉴权 + ErrorEnvelope；SHIM_DEVICE_TOKEN 未设时鉴权关闭「validation mode」；GET /v1/alerts line 275-287 只读不清除）
- 安全先例：ARQ-260902-shim-401-gate-bypass（401 曾穿透、已修）证明设备鉴权是安全关键面
