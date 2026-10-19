# retro · 261019-gov-review-docs-root-ledger-rehome（Track B 全环：docs 根瘦身——CDQ 并入 debt/contract 第四域、ADR 迁 decisions/、ARQ stub 撤销）

> 主线程第一人称热记忆 · 2026-10-19 · 乘客 #N（gov 面 Track B）· harness 形态 = typed agents（architect / architect-reviewer / landing-verifier 全 typed，判官与核验官简报降级头注不适用）。

**【硬项 R1】** verdict 绿后即写，第一人称热记忆。
**【硬项 R2】** 时长与 token 计量：子 agent 用量有 harness 报告（精确）；主线程自身与墙钟时长无计量，按会话事件序估计，误差界 ±30%。
**【硬项 R3/R4/R5】** 见 §3/§4/§5。

## 1. 时间账

| 相位 | 内容 | 时长 | tokens（子 agent，harness 实测） |
|---|---|---|---|
| 起草 | PROPOSAL v0.1（architect，429 中断后续跑成稿，359 行 + 48 锚 manifest） | ~52 min | 183k（86 tool uses） |
| 评审 R1 | BLOCK(F1–F4)：验证集排除面路径形态错 + sha256 冻结表述失实 + 2 NIT | ~13 min | 104k（45） |
| R1 修订 | architect 逐条消化 4 findings | ~6 min | 54k（18） |
| 评审 R2 | GREEN（残留 1 NIT 随 FROZEN 消化） | ~3 min | 136k（3，resume 含 R1 上下文） |
| FROZEN | 立项 ack（方案甲 + OQ1–OQ4 全 (a)，含 OQ3 五件 def 显式批准）+ R2 GREEN + 条件锚裁剪 + 锚计数修 48 | — | 主线程 |
| landing | T1–T6 主线程 packet-bound（52 件，含闸/渲染器/doctor 三脚本与 12 条目机械生成） | ~40 min（估计 ±30%） | 主线程 |
| 核验 | PASS + 2 ADVISORY（均 PROPOSAL 侧瑕疵，不阻断） | ~12 min | 121k（46） |

对照：本批触点面（48 锚）为近期 gov 批最大之一，但评审只走了 2 轮即 GREEN——R1 四条全是验证集/证据表述层，方向零争议。

## 2. Token 账

计量方式：子 agent = harness usage 报告（精确）；主线程无计量，不估绝对值。

| 载体 | tokens | 备注 |
|---|---|---|
| architect（起草 + R1 修订） | ~237k | 起草期普查实证（逐行 grep + 基线实测）占大头 |
| architect-reviewer（R1+R2） | ~240k | R1 全面亲核 45 步；R2 聚焦 3 步 |
| landing-verifier | ~121k | 48 锚逐文件人读 diff + 12 条目 byte 对账复跑 |

花费最大处 = 起草期 census 与核验期逐文件人读——这正是本批的承重面，不可压。可压缩处：无显著浪费；429 中断造成的一次会话切换损耗属环境因素。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 验证集白名单欠列「处方新文自带迁址注」**：现象 = §5.7 白名单 ①–⑯ 钉死「文件 + 内容特征」，但 PROPOSAL 自己的处方新文在 9 行（⑰–㉕）必然自带旧路径字面（处方文本逐字含旧路径，如模板行），落地与处方一致即必然「白名单外命中」→ 根因 = R1 修白名单时按「既有命中」枚举、没按「处方文本会新产生的命中」正推 → 证据锚 = landing-verdict ADVISORY-1（判 PROPOSAL 侧瑕疵、同 R1-F1 族，不阻断）；summary「验证集偏差登记」节 9 行逐行在册。
- **F2 · 状态词映射牵出 advisory 容差缺钉**：现象 = §5.3 承诺 advisory ≤195，但 T1 映射表把 CDQ-260913 钉 MAJOR（off-rubric），advisory 实测 196 → 根因 = 验证集承诺与映射表两处独立撰写、未交叉对账（PROPOSAL 内部不一致）→ 证据锚 = landing-verdict ADVISORY-2。
- **F3 · architect 起草被 429 定额中断**：现象 = 首派 183k tokens 后 API 429，靠「换账号 + SendMessage 续派」续上成稿 → 根因 = 环境定额，非流程问题；但暴露「大半成品 PROPOSAL 在盘、主线程须先验稿再续派」的断点续传姿势 → 证据锚 = 本会话中断记录。
- **F4 · 主线程落地一次自伤**：现象 = 给 TECH-261011 写追注的 python 脚本先写残片后退出（cp1252 控制台编码炸在 print 上，文件句柄已写）→ 根因 = Windows 控制台编码 + 脚本内操作顺序不当（先写后验）→ 证据锚 = summary 诚实注记 1（残片未进任何 commit，当场按 append-only 修齐）。
- **near-miss（最险一刻）**：闸读点改写第一版用 `existsSync(cdqDir) && listFiles(...)` 做目录在场判定——注入式自检里 `existsSync` 是真实 fs、注入路径永不存在，6 个新夹具会全部假红；写自检夹具时当场发现、改为 `try { listFiles } catch → 空集`（向严不变、注入可测）。没出事的理由 = 真空绿规约逼我逐例跑自检而非「看起来对」。教训：注入面与真实 fs 的边界要画在函数签名上（可注入的都进 deps），不画在函数体内。

## 4. 优化建议（按 ROI 排序）

1. **验证集白名单/容差类条目改「正推式」撰写**：凡「命中 ≡ 白名单」型验证条目，起草时从处方文本正推会产生的命中（而非只枚举现状命中）；凡计数承诺（≤N），与数据来源表交叉对账 → 治 F1/F2 · 载体 = **登记 ARQ（gov 域，architect 撰写约定增补一条）** · 预期收益 = 消掉本批两条 ADVISORY 的同族复发。
2. **注入面边界纪律进脚本撰写惯例**：可注入依赖（readText/listFiles/execFn）全走 deps 参数，函数体内不混用真实 fs 判定 → 治 near-miss · 载体 = 登记不硬治（gov-arq 收件箱一条，下次触闸类脚本时参照）· 预期收益 = 假红/假绿自查前置。
3. **429 断点续传姿势留档**：agent 中断后先验盘上制品完整度再决定续派/重派 → 治 F3 · 载体 = 留批次（本 retro §3 F3 即档案，不另立条目）。

## 5. 一句总结

成本约 62 万子 agent tokens + 主线程落地约 40 分钟（估计 ±30%）/ human 触点 2 次（立项布局裁定 + OQ 四问，同一会话内）/ 制品 4 件齐（PROPOSAL + summary + verdict + retro）· 最险一刻 = 闸读点 existsSync 注入边界（自检当场抓回）· **保险丝判定：两方独立结构接住作者偏差**——判官 R1 独立抓出验证集排除面路径形态错（F1 BLOCKING，落地前必假红）与 sha256 表述失实，核验官独立裁定 9 行白名单外命中为 PROPOSAL 瑕疵而非落地私货，两处都是主线程/起草方自己看不出的盲区 · 最值得带走的一条 = **「与处方一致的落地也会撞验证集」——验证集要从处方正推，不是从现状枚举**。

## 6. 本环遗留

- ADVISORY-1（白名单欠列 ⑰–㉕）→ 不阻断；后续批若再扫该区，可将 9 行补进白名单或立「处方新文迁址注」通配类目（随建议 1 的 ARQ 一并处置）。
- ADVISORY-2（advisory 196 > 195）→ 不阻断；CDQ-260913 的 MAJOR 档保留（迁移不造数据），归一severity 词表是账本既有留根、不属本批。
- §6 留根 1–7（PROPOSAL）：ARQ-261014 不结项（已追注）、TECH-261019 已登记（OQ2 甲案）、闸新读点首次 live 验证观察项、repo-structure 复议触发额度已消费一个（docs/decisions/）、CDQ 旧状态词停用、architect.md:52 既有漂移悬案、自律件两条——各按留根去向执行。
- post-merge 义务：主线程合并后复跑闸自检 + doctor 零红 + `--upsert` 簿记（§5 第 12 条）。
