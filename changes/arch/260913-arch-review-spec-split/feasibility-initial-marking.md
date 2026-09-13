# 可行性终标清单（human 逐条过目完毕 · 2026-09-13 · landing 制品）

> OQ-2 裁定流程（2026-09-13）已走完：主线程备清单（每条带证据行号）→ human 逐条过目终标（升降/收窄/砍均见表内「human 裁」注记）→ 终标随 landing 落 `docs/requirements.md` 锚行。标定规则 = 只搬运既有证据（实测留档/包闭环 → ✅；spec 标「待验证」或证据缺口 → 🟡），不造新判断；🔴 终标为零。
> 证据缩写：`spec:<行号>` = docs/spec.md 现行行号；`closed` = changes/INDEX.md 该包状态 closed（红基线转绿验收闭环）；`skill` = 本仓 .claude/skills/ 实测经验记录。

## A · Story（13 条活跃 + 底座 1 条；S-F2.5 已砍见 D 节）

| ID | Story | 拟标 | 证据 | 裁点说明 |
|---|---|---|---|---|
| 底座 | 语音会话生命周期与按键（§4.0） | ✅ | spec:258–283；260829-session-logic closed；260911-key-route-a1 closed（按键路由）；live 冒烟多轮（skill: kidpal-live-smoke-debug） | 状态机 + 按键双面均有包闭环 + 实机记录 |
| S-F1.1 | 首页常显（时间/日程/温湿度/电量） | ✅ | spec:288–296；260829-home-p1 closed；260911-screen-unification closed；260911-display-first-m1 closed | 三包接力闭环 |
| S-F1.2 | 课表页今天/明天 | ✅ | spec:298–306；260829-schedule-p2 closed | — |
| S-F1.3 | 语音问课表 | ✅（human 裁 2026-09-13） | spec:308–315；260829-voice-roundtrip closed（对话链路 + display 消费）；live 冒烟多轮（skill）；human 实测：能找到课表 | 显示问题（human 提到）归符合度/质量面，不影响可行性标定，转 phase 2 审计 |
| S-F1.4 | 日程提醒三段阶梯 | ✅ | spec:317–341；260829-schedule-reminder closed（RMD-A2~A8 全档）；cron 实测可行 spec:326「2026-09-02 实测可行」 | 红基线 30 新红→包内转绿（taskpackage 留档） |
| S-F1.5 | 家长录入课表（QQ 主通道） | ✅（human 裁 2026-09-13） | spec:343–349（V22 已定通道）；human 实测：QQ 录入课表链路可用 | 原拟 🟡（缺专项留档），human 实测记忆定案升 ✅ |
| S-F2.1 | 故事列表 | ✅ | spec:355–363；260829-story-p3 closed | — |
| S-F2.2 | 一键录故事（manual 聆听 + 网关累积） | ✅ | spec:365–374（V25/V31）；260829-story-p3「服务端扩」closed（kidpal-mvp PROPOSAL 包 8 明载 manual 聆听 + start_recording + 显式回退） | — |
| S-F2.3 | 回看/回听 | ✅ | spec:376–384；260829-story-p3 closed（列表 + 回看/回听 + 盖章） | — |
| S-F2.4 | 成就盖章 | ✅ | spec:386–392；260829-story-p3 closed | — |
| S-F3.1 | 语音查字 + 笔顺演示 | 🟡（human 裁 2026-09-13 降级） | spec:404–412；260829-char-stroke closed（CHR-A1~A9，host 面）；D3 定稿 spec:586（决策 12） | **human 口述：live 查字链路当前不可跑**——host 包闭环 ≠ live 端到端实证，降 🟡；phase 2 修复优先项 |
| S-F3.2 | 拼音/笔画/部首/组词 | 🟡（human 裁 2026-09-13 降级） | spec:414–420；char-stroke closed（CHR-A6 右栏速查卡，host 面） | 随 S-F3.1 同链降级：live 查字不可跑则右栏速查卡亦无 live 实证 |
| S-F3.3 | 重播（收窄后单义） | ✅（human 裁 2026-09-13 收窄） | spec:422–429；重播本地重绘 = char-stroke closed（CHR-A5，host 面） | 「语音查下一个字（指代解析）」半条移出，挂 requirements.md 需求类开放问题（多轮连续性底座能力，与 ARQ-260902-multiturn-continuity 呼应）；human 确认未试过 |
| S-F4.1 | 通用问答 | ✅ | spec:435–445；260829-voice-roundtrip closed（VRT-A7 = F4/S-F4.1 承载、VRT-A5 兜底、VRT-A4 遵循度复验） | 字号下限单列为 NFR-READ-1 |

## B · NFR（5 域 7 条；PWR-1 / READ-2 已砍入 D 节）

| ID | NFR | 拟标 | 证据 | 裁点说明 |
|---|---|---|---|---|
| NFR-PERF-1 | 思考超 4 秒先开口安抚 | ✅ | spec:263；voice-roundtrip closed（VRT-A5：4s 安抚句到达即播） | — |
| NFR-PERF-2 | 查字端到端延迟 5 秒档 | 🟡（human 裁 2026-09-13） | char-stroke CHR-A9（live 口径不 auto-run）；human 口述：live 首跑延迟长、查字链路当前不可跑 | **需求保留**（human 裁），实际效果优化归 phase 2；与 S-F3.1/3.2 降级联动 |
| NFR-SEC-1 | 传输安全与鉴权（wss + 强制 token + 证书） | ✅（human 裁 2026-09-13） | spec:564/:596（决策 22）；260903-device-ws-switch closed（WS 切换 + 证书）；260903-gateway-directionB closed；human：十年期证书已在用，视同通过 | 轮换脚本/到期告警子项不拆（human 一句话覆盖全条） |
| NFR-SEC-2 | 儿童数据出网记账 | ✅ | spec:593（决策 19 V29，human 2026-09-03 确认接受） | 已接受风险记账完成，非可行性问题 |
| NFR-OFF-1 | 离线边界（断网仅时间/温湿度/电量） | ✅ | spec:598（决策 24 + V4 介质禁令）；schedule-reminder closed（RMD-A7 断网无提醒） | — |
| NFR-SAFE-1 | 内容安全（system prompt 级 + ACCEPT-AS-RISK） | ✅ | spec:602（决策 28 V30，human 2026-09-03 裁） | 机械过滤层留日后合规选项（§6 留根） |
| NFR-READ-1 | 1bpp CJK 字号可读性下限 | 🟡（验证方式已钉） | spec:443/:602；ARQ-260831-small-cjk-1bpp-readability 在册待实测 | **验证方式（human 定 2026-09-13）**：后续技术包单独烧一个版本，单页排不同字号实测对照；实测前按可读字号排版、滚动承接 |

## D · 已砍（human 裁 2026-09-13；落 requirements.md「已砍清单」）

| ID | 需求 | 砍因 |
|---|---|---|
| S-F2.5 | 云端润色故事 | 旧 spec 已标（可选）（spec:394），本批确认不迁入 |
| NFR-PWR-1 | 续航阈值 | human：「取消掉，不管了」——spec:595 决策 21 三要素随旧 spec 归档，不迁入新 NFR 章 |
| NFR-READ-2 | 暗光可用性（无前光） | human：「不要」——spec:529 环境约束声明随旧 spec 归档，不迁入 |

## C · 过目裁定记录（2026-09-13 全部定案，无残余待定）

1. **S-F3.3 已定**：human 采纳收窄方案——S-F3.3 = 「重播」单义 ✅；「语音查下一个字（指代解析）」移出挂 requirements.md 需求类开放问题。
2. **升降定案**：S-F1.3 ✅（显示问题转 phase 2 审计）、S-F1.5 ✅、S-F3.1/3.2 降 🟡（live 查字不可跑）、NFR-PERF-2 🟡 保留待优化、NFR-SEC-1 ✅（十年证书视同通过）、NFR-READ-1 🟡（验证方式 = 专用烧板字号对照页）。
3. **落地联动**：D 节已砍三项（S-F2.5 / NFR-PWR-1 / NFR-READ-2）落 requirements.md「已砍清单」；S-F3.1/3.2 的 🟡 + NFR-PERF-2 的 🟡 写进 phase 2 立项输入（查字链路 live 修复 + 延迟优化）。

> 终标结果随 landing 写入 requirements.md 各锚行（✅/🟡/🔴），本清单归档为本变更 landing 制品。
