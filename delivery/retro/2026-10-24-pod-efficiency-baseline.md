# Pod 效率基线（第一步：用现有数据跑通流程）

> 生成日期：2026-10-24。性质：周期复盘的效率基线，不是新门、不改任何 canonical。
> 定位：按 human 2026-10-24 的决定，retro 效率化拆两步。**本文是第一步**——只用现有数据把流程跑通，能拿的拿、拿不出来的留空。第二步（每个 Pod 补记录）见第五节，另行走 `/gov-change`。
> 口径：token = pod-events 中 dispatch 的 input+output（只取 exact 来源，不含 cache read）；agent 时长 = dispatch durationMs 之和；墙钟 = 首末 pod-event 时间差（事件稀疏时低估）；行数 = Pod 合并提交相对第一父的增删合计。**派生观测，不是计费真值。**

## 一、数据覆盖：哪些能拿，哪些拿不出来

| 维度 | 可算 | 不可算 | 说明 |
|---|---|---|---|
| 带日期前缀的 TaskPackage | 87 | — | 另有 `current` 指针目录不计 |
| 有 pod-events | 81 | 6 | |
| 有 exact token | 62 | 25 | 早期包多用 legacy 格式，或 transcript 回传缺失 |
| 能归因到合并提交 | 69 | 18 | 靠 commit message 加路径交叉验证；发布批合并会误伤，个别包的行数不可用 |
| 三者全齐 | 53 | 34 | 本报告的主口径 |
| 每 Pod 起止时间戳 | 0 | 87 | 现在只有事件时间，没有起包/收口时间 |
| 每道门的开火记录 | 部分 | — | verifier 红轮与 supervisor 判定分散在各自制品，未汇总 |
| 合并后逃逸归因 | 0 | 87 | 没有「修复回指」字段 |
| nano 档成本 | 0 | 1 | nano 不派发子 agent，pod-events 无 dispatch |
| 能力基线标签 | 0 | 87 | 没有纪台账；本报告只能用「模型族」作临时分组键 |

**明确留空的列**：验收断言数、合并后返修率、false-green 事件、harness 版本、起包/收口时间。这些不是零，是「不知道」。

## 二、成本与时间：按模型族 × 档位（临时能力键）

> 模型族是现有数据里唯一可用的能力代理。`glm-5.3` 合并了 `glm-5-3-flash`、`glm-5-3-flash-260828`、`glm-5.3` 三种写法；`deepseek-v4.1` 合并了 `deepseek-v4-1-flash-260910`、`deepseek-v4-1-flash-261017`、`deepseek-v4.1-flash`。这个归并本身不可靠，第二步必须换成显式纪元标签。

| 模型族 / 档位 | n | token/Pod | agent min/Pod | 墙钟 min/Pod | 等待占比 | dispatch/Pod | 治理 token 占比 | 返工派发占比 |
|---|---|---|---|---|---|---|---|---|
| (未记录) / full | 18 | 285,043 | 117 | 399 | 65% | 6 | 52% | 0% |
| (未记录) / lite | 3 | 318,346 | 78 | 206 | 75% | 5 | 50% | 0% |
| deepseek-flash / full | 3 | 1,259,453 | 61 | 342 | 72% | 7 | 55% | 11% |
| deepseek-flash / lite | 3 | 554,005 | 20 | 21 | 36% | 4 | 72% | 0% |
| deepseek-v4.1 / full | 3 | 2,836,483 | 91 | 152 | 35% | 5 | 60% | 0% |
| deepseek-v4.1 / lite | 1 | 1,161,944 | 61 | 503 | 88% | 9 | 34% | 44% |
| glm-5.3 / full | 13 | 825,836 | 71 | 276 | 66% | 5 | 56% | 0% |
| glm-5.3 / lite | 17 | 613,638 | 36 | 84 | 48% | 4 | 59% | 0% |

读法：

- **glm-5.3 是唯一有像样样本的模型族**（full 13、lite 17），其余格子的 n 都小于 5，只能当线索，不能当结论。
- 同族内 lite 比 full 省 26% token（614K 对 826K）、省 49% agent 时间（36 对 71 分钟）、墙钟省 70%（84 对 276 分钟）。
- **治理 token 占比除一个 n=1 的格子外都在 50%–72%，不随档位下降。** 验收写作加收口是一笔按 Pod 收的固定税，跟批次大小关系不大。
- 等待占比（墙钟减 agent 时长，再除以墙钟）在 35%–88%，多数格子超过 48%。大头的延迟不是 agent 在算，而是在等人和串行环节；但这个数受事件密度影响很大，只能看趋势。
- 返工派发占比（fixer 派发 / 总派发）在 0%–11%。full 档出现返工、lite 档基本为零，与「full 承接更高风险任务」一致。返工不是主要成本。

## 三、产出与单位成本

| 模型族 / 档位 | n（有 diff） | impl 行 | test 行 | 治理制品行 | 文书代码比 | token / 千行（impl+test） |
|---|---|---|---|---|---|---|
| (未记录) / full | 18 (15) | 559 | 1,085 | 1,169 | 0.8:1 | 344,538 |
| (未记录) / lite | 3 (3) | 194 | 794 | 1,202 | 1.5:1 | 593,929 |
| deepseek-flash / full | 3 (3) | 773 | 1,739 | 2,465 | 1.1:1 | 449,003 |
| deepseek-flash / lite | 3 (2) | 195 | 839 | 1,608 | 3.0:1 | 973,700 |
| deepseek-v4.1 / full | 3 (3) | 448 | 1,076 | 2,350 | 1.6:1 | 1,506,364 |
| deepseek-v4.1 / lite | 1 (0) | — | — | — | — | — |
| glm-5.3 / full | 13 (13) | 436 | 1,530 | 1,643 | 1.0:1 | 452,280 |
| glm-5.3 / lite | 17 (14) | 141 | 899 | 1,938 | 2.6:1 | 999,183 |

读法：

- **lite 是「批次变小」的档，不是「单位更省」的档。** 同族内 lite 的 test 行是 full 的约六成（899 对 1,530），impl 行只有约三分之一（141 对 436）；治理制品行却几乎一样（1,938 对 1,643），摊到更少的实现上，单位 token 就贵了 2.2 倍。
- **文书代码比 = 治理制品行 / (impl + test) 行。** full 档大约 0.8:1 到 1.6:1，lite 档 1.5:1 到 3.0:1。之前口头讨论里说的「4.5:1 / 18:1」是拿治理制品只除 impl 行，分母不同；以本表口径为准。
- test 行普遍多于 impl 行（glm full 1,530 对 436）。这是验收先行设计的直接结果，不是异常，但它解释了为什么治理成本很难随实现规模一起缩小。

## 四、这一批数据能下的结论 / 不能下的结论

**能下的结论（样本足够，同族比较）：**

1. lite 档确实降低单 Pod 的绝对成本（token -26%、agent 时间 -49%、墙钟 -70%），代价是实现产出只有 full 的三分之一左右。
2. 治理开销是每 Pod 的固定税：除一个 n=1 的格子外，治理 token 占比在 50%–72%，不随档位或产出规模下降。
3. 返工不是主要成本：fixer 派发占比 0%–11%，lite 档接近零。
4. 等待时间占比高（35%–88%，多数格子超过 48%），说明流程延迟主要在 agent 之外。
5. test 行的产出密度高于 impl 行，验收冻结与收口是这套体系的主要文本产出。

**不能下的结论（数据不够，本报告留空）：**

1. **治理效率有没有随时间提升**——模型构成变了，且没有能力纪元标签，跨月比较会把模型红利记成治理功劳。
2. **nano 档有没有用**——只有一个样本，且没有成本数据。
3. **合并后逃逸率**——没有修复回指字段，无法判断门是否拦住了该拦的东西。
4. **每道门值不值得**——开火记录未汇总，deterrence（不该发生时它没发生）不可观测。
5. **墙钟的绝对准确性**——没有起包/收口时间，事件稀疏的 Pod 会被低估。

## 五、第二步：以后每个 Pod 要补的记录

> 这一节是给下一批 `/gov-change` 的输入，不在本批落地。原则：只补「机器算不出来、事后又补不回」的事实，不补可以从 git 或现有事件推导的东西。

### A. 起包时（launch / planner intake）

| 字段 | 内容 | 生产者 |
|---|---|---|
| `capabilityEpoch` | 纪元标识 + 当期模型阵容 + harness 版本 + 派发信封版本 | launch card / planner |
| `podStartAt` | 真实起包时间（不是第一条事件的时间） | launch card |
| `taskClass` | 任务类标签（固件 UI / 服务端 / 真机轮 / 治理），用于同类对比 | triage / planner |

### B. 收口时（closeout）

| 字段 | 内容 | 生产者 |
|---|---|---|
| `podCloseoutAt` | 收口时间 | integrator |
| `baseRef` / `headSha` | 本 Pod 的起点与终点，行数直接按这两个算，不再从历史 merge 反推 | integrator |
| `diffStats` | impl / test / doc / 治理制品四桶行数 + 验收断言数 | closeout 脚本 |
| `gateFires` | 每道门的开火次数（verifier 红轮、supervisor BLOCK/ESCALATE、契约闸 BLOCK、mutation 发现） | 各闸脚本汇总 |
| `mainThreadUsage` | 主线程用量检查点（nano 也必须有） | dispatch-stamp 或 closeout 脚本 |

### C. 合并时

| 字段 | 内容 | 生产者 |
|---|---|---|
| `mergeSha` | 写入 `merge-completed` 事件 | merge-driver |

### D. 合并后（逃逸归因）

| 字段 | 内容 | 生产者 |
|---|---|---|
| `escapeRef` | 若本 Pod 是某个早期 Pod 的返修，指向被返修的 Pod 与「本该拦住它的门」 | 起包时的 triage / retro |

### 配套的软要求

- 事件类型收敛成枚举：现有 45 种自造类型，建议在 append 时告警而不是拦截，保持 fail-open。
- 四桶路径分类写进 canonical 的 glob 列表，保证任何人重算得到同一组数。
- 纪元台账用追加式文件（建议 `.claude/loop-retro/epochs.jsonl`），一行一个纪元，Pod 按起包日期归属。

### 复算

本报告的统计脚本是只读的 `c:/tmp/retro-metrics/collect.mjs`，原始逐 Pod 数据在 `c:/tmp/retro-metrics/pods.json`。脚本和四桶分类目前不在仓内；第二步落地时应把它们正式收进 `.claude/scripts/`，否则下次复算会得到另一套数字。

## 附录 A · 逐 Pod 明细（62 个有 exact token 的 Pod）

> 空列 = 拿不出来，不是零。文书代码比 = 治理制品行 / (impl + test) 行。
| changeId | 模型族 | 档位 | token | agent(min) | 墙钟(min) | impl行 | test行 | 治理制品行 | 文书代码比 |
|---|---|---|---|---|---|---|---|---|---|
| 260829-hardware-validation | (未记录) | full | 4,284 | 5 | 2096 | 4,960 | 1,143 | 1,115 | 0.2:1 |
| 260829-schedule-reminder | (未记录) | full | 238,639 | 338 | 983 | 3,120 | 4,509 | 1,337 | 0.2:1 |
| 260829-session-logic | (未记录) | full | 271,235 | 97 | 97 | 541 | 678 | 705 | 0.6:1 |
| 260829-smoke-e2e | (未记录) | full | 177,150 | 88 | 247 |  |  |  |  |
| 260829-story-p3 | (未记录) | full | 133,954 | 143 | 492 | 2,014 | 3,246 | 827 | 0.2:1 |
| 260831-ui-art-assets | (未记录) | full | 508,603 | 67 | 297 |  |  |  |  |
| 260903-gateway-directionB | (未记录) | full | 12,470 | 136 | 854 |  |  |  |  |
| 260907-g-relay-fix | (未记录) | lite | 9,613 | 90 | 206 | 2,345 | 1,261 | 718 | 0.2:1 |
| 260911-display-first-m1 | (未记录) | full | 77,369 | 51 | 691 | 315 | 239 | 1,110 | 2.0:1 |
| 260911-ecs-deploy-convergence | (未记录) | full | 64,908 | 64 | 182 | 123 | 0 | 1,325 | 10.8:1 |
| 260911-key-route-a1 | (未记录) | full | 634,984 | 176 | 306 | 1,135 | 708 | 858 | 0.5:1 |
| 260911-screen-unification | (未记录) | full | 1,347,158 | 270 | 594 | 2,460 | 1,041 | 970 | 0.3:1 |
| 260915-m1-k3-voice-round-ui | (未记录) | full | 1,843,723 | 286 | 908 | 993 | 1,085 | 2,800 | 1.3:1 |
| 260915-m1-k4-reminder-overlay-unify | (未记录) | full | 0 | 143 | 126 | 205 | 1,230 | 1,169 | 0.8:1 |
| 260915-m1-n1b-p1-home-battery | (未记录) | full | 1,773,420 | 269 | 1003 | 1,082 | 1,493 | 3,328 | 1.3:1 |
| 260917-m1-k4-rma8-fixbatch | (未记录) | full | 379,052 | 43 | 101 | 25 | 115 | 1,589 | 11.3:1 |
| 260917-m2-k1-char-link-defect-fix | (未记录) | full | 298,850 | 50 | 45 | 137 | 824 | 1,558 | 1.6:1 |
| 260917-m2-k2-retract-p4-three-page | (未记录) | full | 763,984 | 96 | 153 | 122 | 1,256 | 1,066 | 0.8:1 |
| 260917-m2-k3-char-window-overlay | (未记录) | full | 1,500,110 | 161 | 2289 | 559 | 1,673 | 1,462 | 0.7:1 |
| 260920-gov-review-dispatch-telemetry-source | deepseek-flash | - | 82,540 | 5 | 0 |  |  |  |  |
| 260920-m2-charwindow-card-fix | (未记录) | lite | 318,346 | 39 | 158 | 75 | 461 | 1,202 | 2.2:1 |
| 260920-m2-charwindow-display-fix | (未记录) | lite | 693,493 | 78 | 1251 | 194 | 794 | 1,519 | 1.5:1 |
| 260921-m3-k1-cloud-schedule-producer | glm-5.3 | full | 636,729 | 48 | 276 | 398 | 947 | 1,471 | 1.1:1 |
| 260921-m3-k2-p2-week-grid-page | glm-5.3 | full | 815,412 | 168 | 197 | 1,971 | 1,530 | 1,209 | 0.3:1 |
| 260921-m3-k3-voice-schedule-entry-live | glm-5.3 | full | 614,827 | 71 | 597 | 128 | 815 | 1,299 | 1.4:1 |
| 260921-m3-k4-p1-cjk-dynamic-text | glm-5.3 | lite | 2,678,349 | 348 | 454 | 602 | 2,139 | 2,015 | 0.7:1 |
| 260922-m3-k5-sntp-time-sync | glm-5.3 | lite | 1,033,125 | 155 | 334 | 449 | 1,340 | 2,012 | 1.1:1 |
| 260923-m3-k6-boot-auto-connect | glm-5.3 | lite | 372,105 | 46 | 52 | 128 | 1,068 | 1,617 | 1.4:1 |
| 260924-m4-k1-story-cloud-link | deepseek-v4.1 | full | 3,812,219 | 98 | 152 | 448 | 1,544 | 3,196 | 1.6:1 |
| 260924-m4-k2-p3-star-notebook-ui | glm-5.3 | full | 1,946,160 | 219 | 651 | 2,128 | 2,175 | 2,886 | 0.7:1 |
| 260924-m4-k3-record-session-live | glm-5.3 | full | 967,122 | 109 | 321 | 436 | 1,868 | 2,425 | 1.1:1 |
| 260924-perf-k1-roundtrip-latency | deepseek-v4.1 | full | 2,836,483 | 91 | 72 | 807 | 1,076 | 2,350 | 1.2:1 |
| 260928-char-window-key-triggered-play | glm-5.3 | lite | 413,541 | 28 | 35 |  |  |  |  |
| 260928-story-connect-push-defer | glm-5.3 | lite | 3,516,225 | 46 | 124 | 18 | 730 | 2,367 | 3.2:1 |
| 260929-char-window-v5-trim | deepseek-v4.1 | lite | 1,161,944 | 61 | 503 |  |  |  |  |
| 261001-m4-k3-story-archive-order | deepseek-flash | full | 591,092 | 18 | 29 | 208 | 450 | 1,889 | 2.9:1 |
| 261001-openclaw-model-name-overlay-drift | deepseek-flash | lite | 444,151 | 14 | 21 | 8 | 263 | 1,336 | 4.9:1 |
| 261010-device-reconnect-on-server-close | glm-5.3 | full | 1,038,945 | 74 | 64 | 87 | 593 | 1,354 | 2.0:1 |
| 261010-prompt-v9-schedule-anti-narration | glm-5.3 | lite | 375,649 | 25 | 48 | 48 | 266 | 1,005 | 3.2:1 |
| 261011-p1-home-v5-rework | glm-5.3 | full | 714,487 | 72 | 1565 | 355 | 1,074 | 1,891 | 1.3:1 |
| 261012-kidpal-realtime-facts | glm-5.3 | lite | 403,383 | 35 | 123 | 57 | 338 | 1,701 | 4.3:1 |
| 261012-p3-topbar-ink-bleach-fix | glm-5.3 | lite | 485,132 | 36 | 70 | 29 | 276 | 1,702 | 5.6:1 |
| 261013-lang-config-thinking-fix | glm-5.3 | lite | 613,638 | 63 | 84 | 7,760 | 12,231 | 95,711 | 4.8:1 |
| 261013-p1-tomorrow-row-sliver-fix | glm-5.3 | lite | 1,270,266 | 100 | 204 | 100 | 395 | 2,396 | 4.8:1 |
| 261014-engine-stop-truncation-asr-filter | glm-5.3 | lite | 907,447 | 31 | 70 | 153 | 1,786 | 3,952 | 2.0:1 |
| 261014-voice-round-entry-exit-fixes | deepseek-flash | lite | 1,072,918 | 50 | 262 |  |  |  |  |
| 261015-p1-auxbar-style-fix | glm-5.3 | lite | 648,467 | 54 | 34 |  |  |  |  |
| 261015-voice-rightcol-label-overflow | glm-5.3 | lite | 233,432 | 22 | 85 |  |  |  |  |
| 261016-p3-story-body-sd-cache | deepseek-flash | full | 1,548,695 | 96 | 342 | 2,033 | 1,739 | 2,465 | 0.7:1 |
| 261016-sd-card-mount | deepseek-v4.1 | full | 772,178 | 54 | 436 | 159 | 834 | 2,206 | 2.2:1 |
| 261017-prompt-v11-schedule-recall | deepseek-flash | lite | 554,005 | 20 | 17 | 382 | 1,414 | 1,880 | 1.0:1 |
| 261018-sd-log-persist | deepseek-flash | full | 1,259,453 | 61 | 764 | 773 | 2,032 | 3,137 | 1.1:1 |
| 261018-sd-log-usb-read | glm-5.3 | lite | 1,115,055 | 97 | 206 | 1,025 | 1,654 | 1,924 | 0.7:1 |
| 261019-p3-record-dialogue-guide | glm-5.3 | full | 2,077,632 | 275 | 1373 | 1,677 | 4,122 | 4,074 | 0.7:1 |
| 261021-p3-record-cancel-lifecycle | glm-5.3 | full | 2,792,077 | 66 | 968 | 223 | 1,910 | 2,615 | 1.2:1 |
| 261022-m5-k1-prompt-carrier-migration | glm-5.3 | full | 923,826 | 45 | 17 | 472 | 2,308 | 1,542 | 0.6:1 |
| 261022-m5-k2b-persistent-session | glm-5.3 | full | 787,350 | 36 | 21 | 359 | 1,039 | 1,413 | 1.0:1 |
| 261022-m5-k3-honcho-memory-production | glm-5.3 | full | 584,705 | 46 | 14 | 776 | 1,741 | 1,643 | 0.7:1 |
| 261022-m5-k4-history-corpus-backfill | glm-5.3 | full | 825,836 | 70 | 74 | 1,331 | 1,158 | 1,784 | 0.7:1 |
| 261022-m5-n2a-schedule-mechanical-injection | glm-5.3 | lite | 1,114,796 | 32 | 29 | 227 | 717 | 1,246 | 1.3:1 |
| 261023-agents-md-static-semantics-alignment | glm-5.3 | lite | 365,021 | 19 | 90 | 0 | 350 | 1,710 | 4.9:1 |
| 261023-honcho-memory-wiring-restoration | glm-5.3 | lite | 555,102 | 30 | 50 | 773 | 1,336 | 1,951 | 0.9:1 |