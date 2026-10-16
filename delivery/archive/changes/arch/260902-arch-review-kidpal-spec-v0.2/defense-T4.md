# Defense — T4 安全与儿童隐私信任边界（defender 答辩）

> defender 轮产出（2026-09-02）。主线程代落盘，内容保真。对 `findings-T4.md` 逐条答辩。

**【T4-F1】CONCEDE**｜自签证书生命周期确为 S5 死局缺口。spec §6.3:554 只说「wss + 自签长期证书（CA 嵌入固件，换证需动固件/重新烧录，轮换管理责任在自己）」，「长期」的有效期年限未量化；§6.3:553 把 OTA 应答端点「留二期」；§9:621 把「自签证书轮换管理」归入「余开放项」，无 owner / 触发条件 / 相位。charter S5（行 70-73）要求 spec「声明证书生命周期管理与换证触发，且与自建 OTA 留二期相位自洽——OTA 未建前证书到期 = 死局」，spec 未给答案。此为轴 7 DEFER（缺 forcing-function）违规，据实承认。改法方向（policy-as-code，无人工门）：落地证书有效期年限 + 到期/换证触发条件（机械轮换脚本 + 提前告警），并把「证书到期」与「OTA 建立」相位对账——若 OTA 未建前到期，须声明死局风险接受或把 OTA 提前。

**【T4-F2】CONCEDE**｜token 轮换 / 配网分发 / 强度 / 每设备-vs-共享均未定义。§9:621 把「设备 token 轮换与配网分发流程」列为开放项，即自证未定义（轴 7 同型缺口）；§6.3:553 只定义存储落点「配网直写 NVS（namespace websocket 键 url/token/version）」，未定义 token 生成通道 / 配网通道加密 / 截获防护 / 泄露轮换；§6.3:552「token 未设 = 拒绝服务」只强制存在性。challenger 引 shim.py:43-45（`SHIM_DEVICE_TOKEN` 空 = 鉴权整个关闭、单一共享 env）是准确现状基线，ARQ-260902-shim-401-gate-bypass（queue 行 49）证实该信任边界历史脆弱；spec 通篇「设备弱 token」单数（§3.0.0:75、§6.1:534），未声明每设备唯一还是全设备共享。改法方向（机械，无人工门）：声明 token 随机强度（≥128-bit 随机会话密钥，非短 PIN）+ constant-time 比对；配网走加密通道（随 wss 握手发放 / 一次性绑定码），轮换 = 自动 background 流程。「每设备唯一 vs 共享」属架构定夺项，须在冻结时裁决并记账后果（共享则提醒回执/故事归档/会话无法归属设备且单点泄露危及全部），非 human 门，但「未声明」本身可证伪。

**【T4-F3】CONCEDE**｜display JSON 无 schema 校验直入设备渲染，DNA#7 缺口命中。契约 §1.5:139 只写「shim 用正则切出 display 块」、未提校验/拒绝语义；shim.py:121-132（`split_display` 仅 `json.loads`，`JSONDecodeError`→`display=None`→回退 text view，无 view 强制、无 data shape/size 校验）；spec §6.2:546-547 / §3.0.2:168-171 只枚举 view、未命名任何校验环节。charter 行 142（DNA#7）本身就把「途中无 schema 校验环节」写为 known 缺口，challenger 精准命中。一处精度修正（不撼动实质）：view 白名单的「取值」已在契约 §1.3 ①-⑥ 与 spec §3.0.2:170 枚举（char/schedule/story_list/story_text/text/home/control），缺的不是名单本身、是**入站 enforcement**——system prompt（契约 §1.6 / shim.py:53-57）只是软约束，非机械校验。改法方向（policy-as-code，轴 11 无冲突）：在方向 B 的 push helper（spec §3.0.0:91 命名的小智服务端 push helper）落地机械 schema 校验：view ∈ 白名单、data shape/大小上限、脏 JSON 拒绝（回退 text 或丢弃），不引入任何家长逐条人工审。

**【T4-F4】CONCEDE**（承认 spec 层 operationalization 缺口，据实标注现有 backstop）｜18789 收敛未写为方向 B 落地前置。§9:621 把「安全组收敛（18789 仅回环）」列为开放项、无 phase；spec §3.0.0:77 声明小智服务端为「唯一公网入口」、§3.0.0:81「loopback + 真 token 不出内网」只是架构意图，真正把 18789 锁 127.0.0.1 / 防火墙拦 HTTP 路径的动作在 spec 里无 owner / forcing-function / 相位，冻结时裸 full-operator 暴露面可存续。ARQ-260901（queue 行 46）已在 accepted-risk 登记且「复议触发 = 生产上线前」，是 spec 外的 partial backstop——但未把「18789 收敛」绑定到方向 B landing 相位，而方向 B 落地（dev 里程碑）早于「生产上线」，中间窗口 18789 仍公网可达。改法方向（机械防火墙动作，无人工门）：把「18789 锁 127.0.0.1」写成方向 B 落地前的硬前置（policy-as-code / 部署期断言），与 ARQ-260901 复议触发合并为一个带相位的进入条件，而非悬浮在 §9。

**【T4-F5】CONCEDE-TO-HUMAN**（elicitation：记账准确性 + 拍板）。可证伪的记账缺失据实承认：spec §7 决策 19:581「儿童语音经云端……仅 ASR 链路」是字面记账，但架构实际出网路径另有两条——转写文本经 DeepSeek（决策 11:573「DeepSeek」+ 验证材料 §1「deepseek-v4-flash」）出网、回复文本经 edge-tts（§3.0.0:79、§3.0.1 A2:102「微软非官方接口」）出网；spec 无一处声明「转写文本出网到 DeepSeek 云」「回复文本出网到微软」，「仅 ASR 链路」措辞会对读者造成「儿童数据只有语音经阿里云出网」的误导。需 human 拍板（不替裁）：① human 是否已明知转写文本会送 DeepSeek 云、回复会送微软 edge-tts；② DeepSeek（deepseek-v4-flash）托管地境内/境外 + 数据留存政策是否落在「境内、可接受」的同一条取舍内。选项（≥2）：(a) human 确认已知且接受 → 改 spec 决策 19 措辞，把三条出网路径（阿里云 ASR / DeepSeek / edge-tts）逐条记账，消除「仅 ASR」误导；(b) human 未被告知 → 重开此取舍（可能须 LLM 选境内托管 / TTS 换本地 provider 或重新表达隐私立场）。两者都是记账/取舍题，非轴 11 人工门。

**【T4-F6】CONCEDE-TO-HUMAN**（elicitation + 轴 10↔11 拍板）。可证伪的姿态缺失据实承认：spec §4.4 S-F4.1:433 对生成端唯一约束是「system prompt 钉死：口语化、3 句以内、适合 8-12 岁」；§3.0.1 A3:103 意图理解「全包」、无内容过滤；§9:631「儿童隐私与鉴权……家长管控」开放项。spec 无任何内容安全/年龄适宜性姿态、无明确失败响应（不适龄内容会直接 TTS + 上屏）。需 human 拍板（不替裁）：「儿童产品内容安全是否需要一层 policy-as-code（机械内容过滤 / 供应商内容安全 API），还是 human 接受 system prompt 级约束即足够」是产品愿景层判断，仓库无法证伪。选项（≥2，严守轴 11 红线）：(a) 接受 system-prompt 约束 + ACCEPT-AS-RISK 记账（与 DNA#7 的 schema 校验并行，不改运行时门结构）；(b) 加机械内容过滤层（policy-as-code：供应商内容安全 API / 本地 blocklist），并配明确失败响应（超限回复替换为固定安抚/拒绝句）。**明确反对任何「家长逐条审批孩子收到的话」形态**——那是蓝图 §5「六门以外不得实现成第七个人工门」的红线；轴 11 立场由本 defender 代为辩护：若需缓解，只能走机械过滤，不可增加逐条人工审。

**【T4-F7】CONCEDE**（NIT 级，附一处术语精度修正）。「弱 token = 弱权限（仅设备面）」这一层 spec 其实已澄清——§3.0.0:75 明写「设备 token（弱权限，WS 握手 Bearer）」，「词义未定义」的说法对权限维度失准；但 token **随机强度/熵**未声明（256-bit 随机会话密钥 vs 6 位 PIN）、「强制鉴权」§6.3:552 只强制存在性（token 未设 = 拒）确是事实——token 弱随机则强制 == 无意义，此强度/熵缺口据实承认。shim.py:95 `!=` 明文比对（非 constant-time）属实，方向 B 退役 shim 后降为安全史基线、须记账。改法方向并入 T4-F2 的机械补（token 随机强度声明 + constant-time 比对），无需单独人工门。

---

## 摘要

- **verdict 计数**：REBUT ×0；CONCEDE ×4（F1/F2/F3/F4）；CONCEDE-TO-HUMAN ×2（F5/F6）；CONCEDE(NIT) ×1（F7）。共 7 条全认（其中 F7 的部分「词义未定义」术语层做了精度修正：spec §3.0.0:75 已写「弱权限」）。
- **deadlock 候选**：T4-F6 是轴 10↔11 最尖锐拍板点（challenger 主张「儿童需内容管控」、defender 主张「不加第七人工门」，正反都有据，勿在支内私裁）。T4-F5 的「是否升为实质分歧」须 human 确认出网全貌后定。二者均非「无法 REBUT 也不愿 CONCEDE」的单方僵局，而是拍板项。
- **top 论点**：
  1. T4-F5 记账不完整属实且是唯一从 SHOULD 可升实质分歧的项，须先由 human 确认出网全貌——本支最锋利点，认账不嘴硬。
  2. T4-F3 是 charter 自身预埋的 DNA#7 缺口（charter 行 142），CONCEDE 但指出白名单「取值」已枚举（契约 §1.3 / spec §3.0.2:170），真缺的是入站 enforcement。
  3. 轴 11 立场全程守住：全部改法方向均为 policy-as-code 机械手段（证书轮换脚本 / 自动 token 轮换 / schema 校验 / 防火墙断言 / 机械内容过滤），零家长逐条人工门；human 已裁的三个取舍（自签证书 / 云 ASR / 强制鉴权）本体未碰，只攻完整性 / 实现含义 / 记账准确性。
