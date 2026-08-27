# 架构 Review 评审轴（arch-review-rubric）

> spec 层对抗 review 的「该攻哪里」清单。challenger 照轴逐条找洞，每条 finding **必须**落到具体 spec/contract 的名字或行，不许凭感觉。scenario-author 在 charter 里冻结「本 spec 适用哪些轴」。
> 这是 dev-loop `code-review-gate.md` 的 spec 层对偶：dev-loop 审实现合规，本表审 spec 是否站得住。

## 怎么用

- scenario-author：从下表挑出适用本 spec 的轴，写进 charter；技术重的 spec（K0/K3/K9）必带 4/5/6/9/10/11，领域重的（S1/S3…）必带 0/1/2/3/8。
- challenger：按 charter 的轴逐条质询，**falsifiable-first**——先打 spec 内部矛盾、spec↔contract 漂移、跨 spec 矛盾（可证伪、可机判），再打软领域判断。
- defender：对每条 finding 反驳（引 spec/contract 原文证明已覆盖）或承认（真缺口）。
- 每条 finding 标严重度（见末尾），任何未结的 BLOCKING/SHOULD ⇒ spec 不可冻结。

## 评审轴

> 系统 spec：**轴 0 最高产**（S1 grilling 实证）。轴 10 与轴 11 **互相对立**，是 deadlock/拍板 的常客。

0. **范围 / 层级越权（系统 spec 最高产）** — 这个系统是不是把某个本该归内核（K*）或公司级关注点的能力，整条揽进自己端到端？对 spec 里**每一个能力**问：自己拥有，还是从某个内核/公司层消费？（S1 grilling：Teams 投递→K7、邮箱 OAuth/合规/分类→公司级 Inbound Router、外呼→出范围。）
1. **双向声明** — 每个 expose/喂给/只读投影 都有匹配的消费方声明；每个事件 producer ↔ 已声明的 consumers。漏一边 = 毛利缺口类断链。
2. **相位 / 依赖自洽** — 没有能力依赖一个更晚相位的系统却不写降级/defer 说明（例：产能校验需要更晚的 S7）。引用务必写明「公司 Pn」或「Sx Stage n」（蓝图 §8 相位术语）。
3. **命名 / 词汇冲突** — 公司 P0–P6 vs 系统内部 Stage/Phase;同名异义（Account vs LedgerAccount)。命名漂移会让 builder 乱接。
4. **单一事实源 / 无漂移** — 不手抄 `core.*`;图（mermaid/数据模型）反映 contracts 而不偏离;单一 client/source。spec 引用的契约名必须真实存在（见 A7 机械门 + `contracts/README.md` 铁律）。
5. **所有权 & Tier** — Tier-1（跨系统消费者 / `core.*` / events·tools·shared-types）vs Tier-2（系统私有 `@@schema`、无跨系统消费者）判定正确;数据所有权落对系统（成本在 AiRun、毛利在 S6、薪资在 S7）。
6. **门 / 编排正确性** — 门→责任系统映射显式;下游**绝不**订阅门控事件，只订后置事件;handshake 范式一致（蓝图 §5 六关卡;开票门是唯一跨系统门）。
7. **DEFER 纪律** — 每个 DEFER 都有 forcing-function + owner + 相位，不是「永不」;后置能力有触发条件，不是幽灵。
8. **完整性 / 非空过** — 每个 story/AC 可实现、可证伪（spec 层的「200=pass」反模式：别留无法判真假的承诺）。**每个集成故事必须具名 外部系统 + dedup key + 同意边界，否则即幽灵需求**（S1 E2 Teams transcript 教训）。
9. **不变量 & 失败模式** — 声明的不变量可强制;pre-mortem 的失败场景都有设计好的响应（不是「不会发生」）。
10. **安全 / 权限 / 数据隔离**（架构级） — 跨系统读权限、scope、外部不可信内容入站的信任边界、客户内容入 LLM 的处理姿态。
11. **AI-native / 自动化保全** — 设计是否让流程不「断在人这里」？任何强制**逐条人工审**的管控/同意机制都违反 AI-native，必须改成 policy-as-code（预授权 + 自动执行 + 例外队列）。**本轴与轴 10 对立**：challenger 不得无脑最大化管控;defender 替自动化方辩;取舍交拍板。锚点 = 蓝图 §5 关卡纪律（六关卡 canonical，系统不得自创逐条人工门）。

## 严重度

| 级别 | 含义 | 对冻结的影响 |
|---|---|---|
| BLOCKING | spec 自相矛盾 / 越界 / 断链 / 违反蓝图铁律 | 阻塞冻结 |
| SHOULD | 真缺口但有 workaround;不改会留债 | 阻塞冻结（除非 ACCEPT-AS-RISK 记账） |
| NIT | 表述/命名小问题 | 不阻塞 |
| OPTIONAL | 可做可不做的增强 | 不阻塞 |
| FYI | 仅提示/上下文 | 不阻塞 |

> 任何未结的 **BLOCKING/SHOULD** ⇒ spec 不可冻结。结案方式见 `arch-review-protocol.md` 的 finding outcomes。
