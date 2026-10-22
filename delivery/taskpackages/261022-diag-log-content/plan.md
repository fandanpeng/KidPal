---
id: 261022-diag-log-content
title: 板端诊断日志内容增强（时间前缀 + 五缝语义行 + 噪声降频）
rigor: lite
riskTier: medium
sourceType: human-ad-hoc
reviewed-by: architect
date: 2026-10-22
status: pending
---

# plan · 板端诊断日志内容增强（261022-diag-log-content）

> 来源：/triage-change 2026-10-22，architect 比例化 review 判 **②单包 Lite**（riskTier=medium，sourceType=human-ad-hoc），human 触点①已确认（档位 + 墙上时间注入点 = 钩子层每行带）。四面探测触面 = 1（纯实现面），不建 spine；契约四分类 NO_CONTRACT_CHANGE（依据 docs/architecture.md:270「板端日志行格式无契约权威」+ docs/contracts/** 零触碰）。launch/verify 快路径**不适用**（设备编译面深，按 full 环境备）。

## human 意图（立项原话，逐字）

1. 「我看了一下我们设备端导出的日志，它记录的基本上完全没有什么意义……我们至少得把每个页面和每个状态的跳转都记下来。这样出了问题，你才能查得到它到底在哪一步出了问题，也能够感知到设备是怎么跑的。」
2. 「而且连时间都没记，他给了一个数字，这个数字不知道是啥呀。你至少得把当时的时间记下来吧？」

实证样本（真机 TF 卡导出）已由主线程分析：能拼出按键翻页轨迹，读不到每行墙上时间（只有 uptime 毫秒）、按键语义（action=3 密码）、PAGE_RENDER 触发归因、设备状态机动态；SystemInfo SRAM 每 10s 一条噪声占篇幅 70%。

## 建什么（一句话）

让 TF 卡导出的诊断日志能回答「设备哪一刻在哪一页、什么状态、被什么触发」——每行带墙上时间、页面跳有 from→to+trigger、状态机跳有语义行（含被拒转移）、按键有语义名、周期噪声降频。

## 改动点（changeManifest 种子，architect 锚文件:行）

1. `firmware/main/diagnostics/sd_log_fs.cc`——KidpalSdLogVprintf 写环形缓冲前加墙上时间前缀；**[C4] 三锚保持绿**（先 `s_prev_vprintf(` 链式转调、ring_write 消费、写盘任务零 ESP_LOG）；未同步时降级 `BOOT+<ms>` 标注。
2. 新增纯逻辑单源（建议 `firmware/main/diagnostics/log_semantics.{h,c}`）——global_action_t/key/gesture/page 语义名映射 + 墙上时间前缀格式化决策（已同步→`YYYY-MM-DD HH:MM:SS`，未同步→BOOT+ms）+ SystemInfo 降频间隔决策；host 同源，行为级验收测试承载。
3. `firmware/main/device_state_machine.cc:120/:125`——拒绝转移补打（`state_reject from→to`）、既有行可带 corr_id；零行为变更。
4. `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc:127/:231`——action/gesture 数字后挂语义名（`action=3(session_toggle)` 形态），corr_id 纪律保留不动。
5. `firmware/main/display/kidpal_render_node.cc`——新增 PAGE_SWITCH 一行（from→to + trigger 枚举：key/timer/net），PAGE_RENDER in/out 行不动。
6. `firmware/main/application.cc:396-399`——SystemInfo 10s→降频（建议 60s，间隔数值走纯逻辑单源，不裸改字面）。
7. host runner 登记（`firmware/main/test/host_app/` CMakeLists）+ 新验收文件。

## 验收测试建议方向

- 新纯逻辑映射/决策函数 = 行为级验收（含降级分支、节流边界）；
- 接线 = 源锚代理（仓内先例形态）；
- 回归网 = 既有冻结件全绿（`sd_log.acceptance.c` [C1]-[C5] 尤其 TEST 8 源锚、`key_layer_dispatch.acceptance.c`、`time_sync.acceptance.c`）；
- 真机轮 = 导卡逐行核五类记录面 + 串口不中断 + 无钩子递归迹象 + 噪声降频生效（对齐 SLP-A6 真机观察先例）。

## 风险

- **R1（最大）**：vprintf 钩子任意任务/ISR 上下文被调（[C1] :50 明示）——钩子内 localtime_r/snprintf 有 ISR 安全与重入风险。缓解方向（human 触点①已选钩子层方案，builder 落实）：低频任务（clock tick 或 time-sync 回调）预格式化墙上时间字符串进缓存，钩子只 memcpy 拼接；未同步走 BOOT+ms。
- **R2**：内容增强抬日志量，128KB 环形 + 16MB 封顶下的 dropped 计数需真机轮观察；噪声降频（改动点 6）正是对冲。
- **R3**：时间前缀改动不得打断 `sd_log.acceptance.c` TEST 8 的字面锚（链式转调顺序等），verify 阶段先跑回归网。

## non-goal

- kplog 轮转口径、SdLog panic 捕获（P3 批遗留、未登 debt，triage 主线程另行登记）；
- 业务日志；远程日志开关；16MB/轮转口径变更（那是架构面）。

## 触点①留痕（2026-10-22）

- 档位：human 确认 **②单包 Lite**（architect 裁决未被改判）。
- 墙上时间注入点：human 选**钩子层，每行都带**（ISR 风险经主线程当面解释后知情选择；缓解方案见 R1）。
- architect 事实修正采纳：状态机缝非新建挂点而是既有行 enrichment（device_state_machine.cc:125 已有 `State: %s -> %s` 行）+ 被拒转移补打（:120 分支）。

## 下一步

planner 跑 `/build-feature 261022-diag-log-content`（从本 plan.md 读 reviewed plan 做 intake；lite 判注：launch/verify 快路径不适用，按 full 环境备）。
