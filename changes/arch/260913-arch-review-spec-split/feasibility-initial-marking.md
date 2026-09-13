# 可行性初标清单（landing 预制品 · 待 human 逐条过目）

> OQ-2 裁定流程（2026-09-13）：主线程备本清单（每条带证据行号）→ **human 逐条过目** → 过目后方可落 `docs/requirements.md` 锚行。标定规则 = 只搬运既有证据（实测留档/包闭环 → ✅；spec 标「待验证」或证据缺口 → 🟡），不造新判断；🔴 预计为零，如过目中裁出 🔴 按挂账纪律挂 ARQ/开放问题。
> 证据缩写：`spec:<行号>` = docs/spec.md 现行行号；`closed` = changes/INDEX.md 该包状态 closed（红基线转绿验收闭环）；`skill` = 本仓 .claude/skills/ 实测经验记录。

## A · Story（15 条 + 底座 1 条；S-F2.5 已砍不入）

| ID | Story | 拟标 | 证据 | 裁点说明 |
|---|---|---|---|---|
| 底座 | 语音会话生命周期与按键（§4.0） | ✅ | spec:258–283；260829-session-logic closed；260911-key-route-a1 closed（按键路由）；live 冒烟多轮（skill: kidpal-live-smoke-debug） | 状态机 + 按键双面均有包闭环 + 实机记录 |
| S-F1.1 | 首页常显（时间/日程/温湿度/电量） | ✅ | spec:288–296；260829-home-p1 closed；260911-screen-unification closed；260911-display-first-m1 closed | 三包接力闭环 |
| S-F1.2 | 课表页今天/明天 | ✅ | spec:298–306；260829-schedule-p2 closed | — |
| S-F1.3 | 语音问课表 | ✅ | spec:308–315；260829-voice-roundtrip closed（对话链路 + display 消费）；live 冒烟多轮（skill） | **裁点**：课表意图答复的端到端专项验收未见单独留档（对话链路组件各自闭环）——要求专项证据则降 🟡 |
| S-F1.4 | 日程提醒三段阶梯 | ✅ | spec:317–341；260829-schedule-reminder closed（RMD-A2~A8 全档）；cron 实测可行 spec:326「2026-09-02 实测可行」 | 红基线 30 新红→包内转绿（taskpackage 留档） |
| S-F1.5 | 家长录入课表（QQ 主通道） | 🟡 | spec:343–349（V22 已定通道）；QQ→schedule.md 写入→即时重算推送全链路 | **裁点**：QQ 录入路径的实测留档未见（直改 MD 副通道路径明确）；你若实测过则升 ✅ |
| S-F2.1 | 故事列表 | ✅ | spec:355–363；260829-story-p3 closed | — |
| S-F2.2 | 一键录故事（manual 聆听 + 网关累积） | ✅ | spec:365–374（V25/V31）；260829-story-p3「服务端扩」closed（kidpal-mvp PROPOSAL 包 8 明载 manual 聆听 + start_recording + 显式回退） | — |
| S-F2.3 | 回看/回听 | ✅ | spec:376–384；260829-story-p3 closed（列表 + 回看/回听 + 盖章） | — |
| S-F2.4 | 成就盖章 | ✅ | spec:386–392；260829-story-p3 closed | — |
| S-F3.1 | 语音查字 + 笔顺演示 | ✅ | spec:404–412；260829-char-stroke closed（CHR-A1~A9）；D3 定稿 spec:586（决策 12） | 教学字体红线 CHR-A7 已验 |
| S-F3.2 | 拼音/笔画/部首/组词 | ✅ | spec:414–420；char-stroke closed（CHR-A6 右栏速查卡） | — |
| S-F3.3 | 重播 / 查下一个字 | 🟡 | spec:422–429；重播本地重绘 = char-stroke closed（CHR-A5）；「那『星』字呢」指代解析依赖多轮会话连续性 | **裁点**：多轮连续性 spec:568 标「待部署验证」（ARQ-260902-multiturn-continuity）；live 冒烟已跑多轮对话（skill）但连续性探针结论未单独留档——live 记录里若已见两轮追问正常即可升 ✅ |
| S-F4.1 | 通用问答 | ✅ | spec:435–445；260829-voice-roundtrip closed（VRT-A7 = F4/S-F4.1 承载、VRT-A5 兜底、VRT-A4 遵循度复验） | 字号下限单列为 NFR-READ-1 |

## B · NFR（6 域 8 条）

| ID | NFR | 拟标 | 证据 | 裁点说明 |
|---|---|---|---|---|
| NFR-PERF-1 | 思考超 4 秒先开口安抚 | ✅ | spec:263；voice-roundtrip closed（VRT-A5：4s 安抚句到达即播） | — |
| NFR-PERF-2 | 查字端到端延迟 5 秒档 | 🟡 | char-stroke CHR-A9（live 口径不 auto-run，对真 ECS/OpenClaw） | **裁点**：live 档实测数字未单独留档——你有印象即裁 |
| NFR-PWR-1 | 续航阈值 | 🟡 | spec:595（决策 21 DEFER 三要素，阈值未拍） | 阈值「待定」迁入（OQ-3 裁）；触发点 = 硬件验收包续航基准 |
| NFR-SEC-1 | 传输安全与鉴权（wss + 强制 token） | ✅ | spec:564/:596（决策 22）；260903-device-ws-switch closed（WS 切换 + 证书）；260903-gateway-directionB closed | **裁点**：V25 证书轮换脚本 + 到期告警子项证据未单独留档，可拆 🟡 子项 |
| NFR-SEC-2 | 儿童数据出网记账 | ✅ | spec:593（决策 19 V29，human 2026-09-03 确认接受） | 已接受风险记账完成，非可行性问题 |
| NFR-OFF-1 | 离线边界（断网仅时间/温湿度/电量） | ✅ | spec:598（决策 24 + V4 介质禁令）；schedule-reminder closed（RMD-A7 断网无提醒） | — |
| NFR-SAFE-1 | 内容安全（system prompt 级 + ACCEPT-AS-RISK） | ✅ | spec:602（决策 28 V30，human 2026-09-03 裁） | 机械过滤层留日后合规选项（§6 留根） |
| NFR-READ-1 | 1bpp CJK 字号可读性下限 | 🟡 | spec:443/:602；ARQ-260831-small-cjk-1bpp-readability 在册待实测 | 实测前按可读字号排版、滚动承接 |
| NFR-READ-2 | 暗光可用性（无前光） | 🟡 | spec:529（「大概率无前光（待确认）」） | 环境约束声明待确认 |

## C · 待 human 过目时一并裁的三件事

1. **A 类三个 🟡**（S-F1.5 QQ 录入、S-F3.3 多轮指代、NFR-PERF-2 live 数字）——都是「你脑子里的实测记忆」能直接定案的，落文件前裁。
2. **NFR-SEC-1 是否拆子项**（核心 wss/token ✅ vs 轮换脚本/到期告警单列 🟡）。
3. **拟 ✅ 各条若你记得有翻车记录**，当场降级——本清单证据以包闭环 + skill 记录为限，不覆盖你口头知悉的例外。

> 过目完成后：终标结果随 landing 写入 requirements.md 各锚行（✅/🟡/🔴），本清单归档为本变更 landing 制品。
