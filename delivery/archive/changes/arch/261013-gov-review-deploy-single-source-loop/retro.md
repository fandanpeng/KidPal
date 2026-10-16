# retro · 261013-gov-review-deploy-single-source-loop（Track B 全环：部署单源闭环——手工匠艺 → manifest + deploy.sh + 每日机检）

> 主线程第一人称热记忆 · 2026-10-13 · 乘客：本批为部署治理批，判官 = architect 子 agent（全文 context 派发、跨轮续用），核验官 = landing-verifier 子 agent（零上下文）。跨乘客对照：**首次**由「human 反馈 OQ 偏技术」触发判官按「技术裁量 vs human 拍板」重切取舍面的批。

**【硬项 R1】** verdict 绿后同日趁热写就，第一人称，无隔场补记。

**【硬项 R2 · 计量诚实】** 主线程与判官各轮的 token 均取自 harness 通知的 subagent_tokens 累计值（判官三轮同 agent 续用：r1 = 73.7k，r2 后累计 96.4k，r3 后累计 110.6k）；主线程自身无计量，估计法 = 按 subagent 计量同量级外推，误差界 ±50%；时长取自 harness duration_ms 与会话事件顺序估计，误差 ±20%。

**【硬项 R3-R5】** 见下文各节。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 排障实证 | ECS 三件套部署实况 + 一致性 md5 对账（漂移 6 处） | 2026-10-13 上午 | 同日 | ~40 min | 主线程（未计量） |
| 起草 | PROPOSAL v1.0（六 T + 4 OQ） | 排障后 | r1 派发前 | ~30 min | 主线程（未计量） |
| 评审 R1 | RED（1 blocker + 2 major + 4 minor） | — | — | 7.6 min | 73.7k |
| 修订 + R2 | v1.1 → RED 轻量（F9–F11） | — | — | 3.5 min + 修订 | 累计 96.4k |
| 修订 + R3 | v1.2 → **GREEN** | — | — | 1.4 min | 累计 110.6k |
| FROZEN | human ack + OQ 全 (a) + r3 GREEN 齐备 | 同日下午 | — | — | — |
| landing | 20 文件落地（11 新增 + 9 修改） | FROZEN 后 | 核验派发前 | ~50 min | 主线程（未计量） |
| 核验 | landing-verifier **PASS** | — | — | 4.4 min | 48.2k |
| **合计** | 端到端约一个工作日内闭环 | | | ~3.5 h 估 | ~160k 判官+核验官实计，主线程估同量级 |

对照：与 260929 两轻量批（ui-ladder-remap / agent-def-alignment）同量级偏上——本批多出排障实证段与 11 件实码落地，red 轮数（2）在其常态带（1–2 轮）内。

## 2. Token 账

计量方式诚实声明：见 R2——subagent 数字为 harness 实计，主线程为外推估计（±50%）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（排障 + 起草 + 落地 + 簿记） | 估 ~150k（±50%） | ~49% | 含 ECS SSH 实测往返与 20 文件写作 |
| 判官（R1→R3 同 agent 续用） | 110.6k（实计累计） | ~36% | 三轮全仓独立复证 |
| 核验官 | 48.2k（实计） | ~15% | 22 tool uses 全复跑 |

花费最大处：判官 r1 的全仓独立复证（对 PROPOSAL 每条断言逐个实跑，含 md5/行号/钉值测试核）——贵但恰是 blocker 的发现面。可压缩分析：主线程起草时若把「消费者路径解析规则」自查先行（见 F1），r1 的 blocker 可前移到起草期自检，判官轮可望省 20–40k。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 主线程自查漏掉第二处路径分裂（判官 r1 blocker 接住）**：现象——我对 ECS 做了全量 md5 对账仍漏掉两个配置消费位缺文件，PROPOSAL v1.0 的 T4 甚至写下「单一写者闭合」的失实宣称 → 根因——census 按「目录枚举」做（ls 哪里有什么），不按「消费者路径解析规则」派生（env 显式 / dirname 相对 / parents[N]——`reminder_schedule_lib.py:472-474` 与 `front_app.py:672-674` 两处默认派生位 ls 永远扫不到），且两处缺位分别被「DEFAULT 同值回退」与「None 静默容错」双重掩盖（`openclaw-reminder.config.yaml:13-14` 自注同值是掩盖的直接实证）→ 教训已落 PROPOSAL §3 census 方法修正段。
- **F2 · YAML 流映射两连坑**：现象——manifest.yaml 校验两红（line 87 note 内 `prompt: 键` 半角冒号、line 142 `parents[2]` 方括号 = 流指示符）→ 根因——散文 note 写进 `{...}` 流映射时忘了流上下文禁指示符 → 证据锚 = 两次 `yaml.parser.ParserError` 实录（2026-10-13 本机 python 3.14 PyYAML）→ 教训：**散文 note 一律双引号包裹或条目转块式**。
- **F3 · Edit 转录失真两例**：现象——按记忆转录长文本做 Edit 两度 not-found（`remrush` 误写；T4 改法段凭记忆拼 old_string 不中）→ 根因——长段原文不逐字回读就转录，违反模板「byte 对账」三句纪律 → 教训：承重引文先 Read 定位行、后复制，禁目视转录。
- **near-miss（最险一刻）**：若判官 r1 没有独立复证、直接采信我「单一写者闭合」的宣称冻结落地，manifest 会上线但**仍然不管两个配置消费位**——「家长改提醒时间不生效」这个缺陷会带着「已治理」的假绿继续潜伏（同值掩盖意味着症状永远只在第一次改配置时爆发）。没出事的原因 = 判官被明确指令「不信任自述、全部亲核」且真的跑了。教训：判官简报里「逐条断言独立复跑」这句是本批保险丝的引信，值得在简报模板里固化。

## 4. 优化建议（按 ROI 排序）

1. **「消费位派生 census」固化为部署面变更的起草期自查**：治 F1 · 载体 = 固化候选 → ARQ 收件箱（建议条目：部署/配置面变更的 census 义务 = 目录枚举 **必加** 消费者路径解析规则派生表〔env 显式 / dirname 相对 / parents[N〕三类先导〕，落地动作待 architect review）· 预期收益 = 把 r1 blocker 类发现前移到起草自检，省一轮红 + 判官复证成本。
2. **YAML manifest 书写规约入 manifest.yaml 头注**：治 F2 · 载体 = 登记不硬治（一行规约：含散文/特殊字符的 note 一律双引号；条目超三字段建议转块式——已可随下次 manifest 改动顺手补，不单开批）· 预期收益 = 消同型解析红。
3. **判官简报模板补「不信任自述、逐条亲核」引信句**：治 near-miss · 载体 = 固化候选 → ARQ 收件箱（判官派发简报的固定段）· 预期收益 = 保险丝制度化，不靠本次简报写对了。
4. **ECS bootstrap 后的 post-landing 验证留痕单**：治 post-landing 段无归属的悬空（现靠 `ecs-bootstrap.md` §5 口头承接）· 载体 = 留批次（bootstrap 执行时把 §5 第 4–11 条实测输出回填 landing-summary 附录或 pod-events 留痕）· 预期收益 = 治理闭环的最后一公里可追溯。

## 5. 一句总结

成本 = 端到端约 3.5 小时、判官+核验官实计 ~159k tokens（主线程估同量级）；触点 = human 3 次（建 repo+push / 「OQ 偏技术」反馈 / ack+OQ 终裁）；制品 = 4 件 + 评审判官记录 + TECH append + INDEX 行；对照 260929 两轻量批量级偏上、红轮数常态带内。最险一刻 = 「单一写者闭合」失实宣称险些带病冻结。**保险丝判定：接住**——判官 r1 独立复证抓出作者自查盲区（第二处路径分裂 blocker，作者全套 md5 对账未覆盖），核验官独立复跑确认落地无 scope creep，两方独立结构各自起效。最值得带走的一条：**部署面的「什么在哪」不能按目录枚举，只能按消费者的路径解析规则派生**——本批两处静默缺陷同出于此。

## 6. 本环遗留

- TECH-261012 修复项 ①（kidpal-schedule-write skill）④（front surfaced 告警）→ dev 批课表录入链路收口时对账（条目补记已钉死归属，防双做）。
- post-landing §5 第 4–11 条 → human push 含本批的 main 后按 `ecs-bootstrap.md` 执行并留痕。
- `char_stroke_lib.py` / `warm_char_cache.py` 消费点未证实 → 不进 manifest v1；处置交 dev 批或下次 cadence retro。
- front `.bak` 历史清理、front-test/ 与 shim/ 退役目录清理 → 首次实弹部署后另行小动作。
- 每日 drift 告警的消费方（谁看 `deploy-drift.json`）→ surfaced 化与 TECH-261012 ④ 同型分叉，另行裁定。
- ARQ 收件箱两条固化候选（消费位派生 census、判官简报引信句）→ 走 gov-arq 登记，不自我批准。
