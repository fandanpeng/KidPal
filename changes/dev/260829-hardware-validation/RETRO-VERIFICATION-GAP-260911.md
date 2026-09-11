# 跨包系统性 Retro · 为什么 acceptance 全绿、validation 集中露馅（2026-09-11）

> 本文是 **workstream 级跨包 retro**，与 9b 包自身收口 retro（[pod-retro.md](pod-retro.md)）互补不重复：
> pod-retro 答「本包执行中哪里摩擦」，本文答「**二十个包的验收齿逐个绿过去，为什么到真机
> validation 才集中暴露十几类问题——包括与 spec 设计根本对不上的（KEY 两键设计）——是流程的
> 哪一步失守**」。证据底稿：PROGRESS-SUMMARY-260910.md §二/§四/§五/§七（14 项遗留总清单）、
> review-packet.md（F1~F7）、pod-events.jsonl（38+ 逐事件）、各包 taskpackage.json 的
> testHost/gateProfile 实测扫描（2026-09-11）、pod-retro.md F1-F4。
> 方法：把 §七 14 项遗留逐条往回追「它穿过哪些包的哪些齿而没被拦住」，归纳失守层。

## 一、总答案：三层收窄模型

「acceptance 通过」通过的命题，比「设计被实现」**窄了三层**。每层单独看都合法、都绿；
三层叠加后，「全绿」与「产品按设计工作」之间出现大片**无齿地带**。真机 validation 是第一个
走进无齿地带的验证面——所以问题不是「在 validation 阶段产生的」，而是「在 validation 阶段
**第一次被看见**的」。

```text
设计面（spec §4.0/§5.3 两键、字体、故事归档、ECS 部署……）
  ↓ 收窄① 环境层：齿只在 testHost 能观测的世界里断言
验证面A（host 纯逻辑 / host+SDL / PC 云端 mock——20 包中 16 个）
  ↓ 收窄② 范围层：齿只覆盖本包声明的边界，边界之间的缝无人断言
验证面B（包内可观测行为）
  ↓ 收窄③ 断言强度层：齿断言的性质弱于契约要求的性质
验证面C（绿信号实际证明的东西）
  ≪ 设计面 —— 差集 = 无齿地带 = validation 集中爆发的全部内容
```

## 二、三层逐层归因（附 14 项遗留的归属）

### 收窄① 环境层：验证环境 ≠ 运行环境

**事实**：2026-09-11 对 20 个 taskpackage 的 testHost 实测扫描——含真机/device 验证面的
只有 4 个（smoke-e2e、audio-bargein-hw、device-ws-switch、g-relay-fix 真机冒烟），其余 16 个
全在 host / host+SDL / PC 云端 / ecs 上验证。9b（hardware-validation）是 workstream 首个
**device 全宿主**包。

**后果**：凡 host 世界里**不存在**的东西——GPIO 接线、I2C 总线共享拓扑、任务栈尺寸
（4096B / 3584B）、DRAM 容量（480KB 静态缓冲）、LVGL 真屏活动屏、live 云配置——齿在原理上
不可能断言它们。§七 14 项中至少 6 项纯属于这一层：

| 遗留项 | host 上不可见的原因 |
|---|---|
| 9 条崩溃链（PROGRESS-SUMMARY §二 1-9） | I2C 总线共享、BCD 寄存器、栈尺寸、DRAM 上限、跨任务栈腐蚀、真屏 active 屏——全是资源与拓扑，host 抽象里没有 |
| #8 首音时延 3-6s | 真云 RTT 串行链，PC 云端 mock 无时延结构 |
| #14 物理验收 8 项 | 真按键手感 / 真屏观感 / 真电池，原理性 host 不可测 |
| #7 键位漂移（部分） | GPIO18 接线这个**物理事实**在 host 不存在（见收窄②） |

### 收窄② 范围层：包边界缝 = 无 owner 地带

**事实**：dev-loop 切包按子系统/切片切，**跨包的端到端行为没有显式 owner**。spec 设计被
切片后，落地动作掉进切片之间的缝——每个包在自己的边界内都绿。这是「压根和设计不一样」
类问题（#7 KEY、#3 故事归档、#1 ECS 漂移、换皮接线）的病灶：

| 遗留项 | 缝在哪（证据锚） |
|---|---|
| **#7 KEY 两键设计悬空** | 包 3 session-logic 被裁定 host 纯逻辑（SES-E2/E3「只做纯逻辑」，taskpackage 零 board/GPIO/接线字样）——两键**语义**在 host 过齿；「把 GPIO18 接上 KEY」这个设备动作**不属于任何包**：`git log -S "GPIO_NUM_18"` 全固件零命中、`key_gesture_parser_feed` 设备侧零调用（仅 host 测试自命中）、板级文件只有 `boot_button_(BOOT_BUTTON_GPIO)`。9b 过渡方案（BOOT 单击=对话/双击=翻页，accept-risk 锚 pod-events d9e0f1a2）把它盖住到交互批次 |
| #3 故事归档双断链 | `StopListening()` 全固件无调用方 + manual 录音期间引擎不出 stt 帧——跨固件/front/引擎三段，**每段在自己的包里都绿**，合起来功能不可达 |
| #1 ECS 部署漂移 | `apply-xiaozhi-overlay.sh:60/62/67/73`（ASR=AliyunStreamASR / Intent=function_call / base_url 缺 /v1 / TTS type=edge）与 live（Qwen3ASRFlash / nointent）漂移——脚本属于部署包、live 配置属运维现场，**两者间无对账齿**；near-miss：任何人顺手跑脚本即断 live 语音（pod-retro near-miss #1） |
| 换皮消费接线 + 渲染节点 | `art_assets.c:10` 自注「殿后 9b」+ home_p1.c 注「不在本包范围」——殿后转手**无 owner 字段**，9b 派单时 planner 漏列，两次 human 目击返工（pod-retro F4，pod-events fd6e84d1 / ebbbcb23） |
| #10 字体资产 | 板级 `BUILTIN_TEXT_FONT=font_puhui_basic_14_1` 无人选型、「猬」缺字无人验；`font_zcool_30.bin` 是 300×52 KAP1 位图样张却被当成「设计字体已就绪」——U 包资产管线与正文消费之间无 owner |

### 收窄③ 断言强度层：齿断言的性质弱于契约要求的性质

**事实**：齿只能断言其 chosen 验证面里可观测的性质；当这个性质**弱于**契约/设计的性质时，
齿绿而契约未兑现。四处实证：

| 遗留项 | 齿断言了什么 vs 契约要求什么 |
|---|---|
| #9 故事页排版 | story_p3 8 齿采样渲染帧，断言「有渲染 / 屏间有差异 / NULL 降级」——冻结验收头注 ⑤ 写明「当前屏正文」，齿未兑现到该强度；`ksy_label`（story_page.c:1062）不设宽不 wrap，正文横向裁切一路绿到真机用户看见「小刺猬」只剩「小刺」 |
| **#7 KEY（机制 vs 行为）** | 各 device 面包写的 KEY 验收（smoke-e2e SMK-A3 / device-ws-switch / story-p3 / reminder RMD-A5 / bargein BRG-A2）断言「按键触发对话」这个**行为**——BOOT 单击与 KEY 单击行为等价，于是「KEY 未接线」在行为断言下**原理上不可见**。行为断言不锚机制（哪根 GPIO），等价假设吞掉设计 |
| #12 P2-A1 齿 | 齿 6 first-match 定位歧义（恒命中 kidpal_render_node.cc:362 而非 schedule_page.c:733-741 真定义处）——齿的**解析器**弱于齿的意图，恒红/恒绿都可能假 |
| #13 WSA-A4/A6 | 证据门齿是 evidence-based（要 flash.log / ecs-teardown 制品），但没有任何包的 pipeline 生产这些制品——齿设计时未配套证据生产链，绿不起来也不知该谁补 |

## 三、KEY 案例深挖（「压根和设计不一样」的完整失守链）

spec §4.0/§5.3 设计 KEY 主司对话 + BOOT 主司翻页两键分工。失守链五环：

1. **切包**：session-logic 包 3 把两键**纯逻辑**（key_gesture_parser）切走并裁定 host-only
   （SES-E2/E3 边界条款白纸黑字）——裁决本身合理，但**设备侧接线没有同步切给任何包**。
2. **写齿**：session-logic 的 SES-A3/A4/A6 在 host 验证语义（合法）；五个 device 面包补写的
   KEY 验收全是**行为级**（按键→对话），机制级（GPIO18 电平路径）零齿。
3. **过齿**：真机上只有 BOOT 一根线，BOOT 单击行为等价 KEY 单击 → 五个包的 KEY 齿全绿。
   **绿是等价假设的产物，不是接线的产物。**
4. **过渡方案固化漂移**：9b 因 KEY 无线可接，现场定 BOOT 单击=对话/双击=翻页的过渡方案并
   accept-risk 锚（d9e0f1a2）——处置本身诚实（human 在场裁定），但 KEY 依附的页内特例
   （回首页/光标/播笔顺/弹窗关闭）从此全部悬空，spec↔实现正式分叉。
5. **暴露**：直到本次 retro 逐包归因才把「缝在哪个包」钉死——9b 是**暴露者不是肇事者**。

** KEY 的教训一句话**：spec 设计的正确性由「语义齿（host）+ 机制齿（device）」**两半**共同
兑现；只写一半时，另一半的缺失在绿信号里完全隐形——行为等价会让它绿得理直气壮。

## 四、哪一步出了问题（dev-loop 环节定位）

| 环节 | 失守内容 | 对应收窄层 |
|---|---|---|
| **切包/派单**（portfolio-planner / planner） | 跨包端到端行为无显式 owner——KEY 接线、殿后项、字体资产、故事归档链全部掉缝 | ② |
| **acceptance-author 写齿** | 无「齿↔契约条目映射」要求、无机制断言纪律、行为等价处不申报等价假设；齿强度可合法弱于契约 | ③ |
| **verifier 判定** | verdict 只消费齿绿，无「未覆盖面申报」——没人回答「哪些设计性质没有齿」 | ②③ 的放大器 |
| **integrator 全量端到端** | 端到端跑在 host/模拟面，物理层与 live 层依旧空白；跨包缝不在其齿覆盖内 | ①② |
| **supervisor** | 只消费绿信号，盲区与 verifier 同构 | 放大器 |
| **live 状态治理** | 云配置 out-of-band 演化，repo 侧齿永远绿；repo↔live 无对账闸 | ①（环境=live 的特例） |
| **planner 知识链** | 硬件事实源（数据手册）不在 loop 内——SHTC3 0x7852 臆测烧一轮（pod-retro F1，教训已成文） | ① 特例 |

## 五、near-miss（最险三刻）

1. **ECS overlay 覆写**（pod-retro near-miss #1）：跑一次脚本即断 live 语音链——没出事纯属
   无人顺手跑脚本。批次①（260911-ecs-deploy-convergence）已把 repo↔live diff + 逐 delta
   人工裁定立为阶段 0 门，**本文将其追认为「live 状态纳入验证面」的第一个落地先例**。
2. **SetStatus 悬空指针**（pod-retro near-miss #2）：三屏双 UI 体系下 story 渲染 clean 掉
   聊天屏控件——根治钳制是被轮3 崩溃逼出来的，不是设计出来的；三屏主权架构收口已立项
   （批次②）。
3. **KEY 悬空的沉默期**：若 9b 没做过渡方案 + accept-risk 锚，两键设计悬空会**继续隐形**
   直到用户在真实交互里需要 KEY 特例（回首页/播笔顺）才发现——那将是产品级行为缺失而非
   工程问题。

## 六、保险丝判定（两方独立结构是否接住）

- **接住的**：代码实现质量类偏差——9 条崩溃链的逻辑类、BCD/命令字类全在 loop 内被
  verifier↔fixer 消化；acceptance-author 独立 + verifier 独立在这个轴上有效。
- **未接住的**：「验证面窄于设计面」这类**系统性**偏差——因为 author 与 verifier **共享同一套
  验证面假设**（同一个 testHost、同一个包边界、同一种断言语言），绿信号对双方一致。
  独立性只存在于「实现 vs 验证」轴，不存在于「设计 vs 验证面」轴——**这不是执行失误，
  是结构必然**。
- **唯一在该轴上独立的信号源是真机轮 + human 持板目视**（「界面还是老的」两次戳破派单遗漏，
  pod-retro F4）——所以问题全部在 validation 集中爆发，且未来仍会，除非把「设计 vs 验证面」
  轴的检查做成流程件（见 §七建议 1-3）。

## 七、建议（按 ROI 排序，载体已标）

1. **跨包行为显式 owner**：治 §二收窄② · 载体 = ARQ 收件箱
   （ARQ-260911-cross-package-behavior-owner-gap，本次已登记）· 预期收益：消灭「设计被切片后
   落地动作掉缝」整类问题——KEY 接线、殿后转手（并消化 pod-retro 建议 1 未登记债）、字体资产
   边界、故事归档链同型缺口。
2. **齿强度对齐契约 + 机制断言纪律**：治 §二收窄③ · 载体 = ARQ 收件箱
   （ARQ-260911-tooth-strength-below-contract，本次已登记）· 预期收益：acceptance-author 产齿
   时做「契约条目↔齿」映射，弱于契约处显式申报；行为等价处必须声明等价假设并锚机制
   （哪根 GPIO / 哪个资产文件 / 哪个 live 配置）。
3. **verdict 增「未覆盖面申报」**：治 §四放大器 · 载体 = ARQ 收件箱
   （ARQ-260911-verdict-uncovered-surface，本次已登记）· 预期收益：每次 verdict 回答
   「本次验证**未触及**哪些设计面」，supervisor/human 消费差集——把「设计 vs 验证面」轴的
   检查从「等真机轮爆」提前到「每包收口时看」。
4. **live 状态纳入验证面**：治 §四 live 行 · 载体 = 已落地先例
   （批次① S0.1 live snapshot + S0.3 逐 delta 裁定；pod-retro 建议 3 同向）· 无新动作，
   追认 + 后续部署类包沿用。
5. **device 面包真机齿最低配**：治 §二收窄①（部分）· 载体 = 登记不硬治（真机齿成本高，
   随批次②④ 的 device 轮逐步试「每 device 包 ≥1 条真按键/真外设读数齿」是否可持续）。
6. **殿后项 owner 字段**：并入建议 1（pod-retro 建议 1 的登记债本次一并结清）。

## 八、与既有制品的关系

- pod-retro.md：包内摩擦与真机执行教训（时间/token 账、F1-F4、两条 near-miss）——本文 §五
  引用不重复。
- PROGRESS-SUMMARY §七 14 项清单 + 四批次分包：本文回答「为什么会有这 14 项」，分包方案
  回答「怎么修」——两者共用 14 项编号。
- 批次② PROPOSAL（display-interaction-sovereignty）OQ4 键位路线 A（补接 KEY）：本文 §三
  是其立项证据链的流程侧补强——OQ4=A 的「补接」动作对应本文建议 1 的 owner 缺口实锤。
