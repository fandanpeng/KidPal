# scope-final — 261022-diag-log-content 影响面圈定终验（scope-file）

> 本文件 = final 相位 verify.mjs `--scope-file` 消费的影响面圈定清单（TaskPackage 目录 sibling，Step 4 范围圈定制品）。**圈定失败须举手，不得默认全量、也不得缩到只剩本包档**——本包圈定走三分法①（per-Pod 专属图索引可用，正常圈定，无降级）。

## 1. 圈定可用性（三分法判定）

- per-Pod 专属图索引 `C-AI-KidPal-pod-m2` **可用**（search_graph / detect_changes 实测命中 `sd_log_fs.KidpalSdLogVprintf` 等 372 符号）→ **正常圈定**，无需降级 git 抽查路径。
- detect_changes 实测：base = HEAD `cbc8f72a`，changed_files 13（实现面 9 + taskpackage 投影 3 + verify-runs.jsonl 1），seed_symbols 36，**impacted_total 33（精确）**，全部落 `firmware/main` 单模块，零 server/ 跨面。

## 2. 影响面对账（detect_changes → 预期缝）

| blast radius 集群 | 代表符号 | 对应实现缝 | 判定 |
|---|---|---|---|
| Application 全方法族（33 中 17） | SetDeviceState / HandleStateChangedEvent / HandleStateChangedEvent 等 | esp_log 钩子全局消费面（时间前缀对全日志行生效）+ SystemInfo 降频在 Run() clock tick | 预期内 |
| 板级 InitializeButtons（aipi-lite / atk-dnesp32s3-box0 / yunliao-s3 / zhengchen 等 5 板） | InitializeButtons | key_route_dispatch 日志行挂语义名（多板共享 keya 路径） | 预期内 |
| waveshare 本板 | CustomBoard::InitializeButtons / keya_overlay_consume | 本包改动文件本体 | 预期内 |
| 渲染节点 | kidpal_render_node_init / kidpal_render_node_tick | PAGE_SWITCH 行 + trigger 标注消费点 | 预期内 |
| mcp_server 故事族 | HandleStoryKey / StoryDispatchAct / StoryTickTimerCb / KidpalStoryVadEnterRecording | 语音轮收尾页面回落 = RN_PAGE_TRIGGER_NET 标注路径 | 预期内 |
| main.cc app_main | Application 构造 | 钩子安装链 | 预期内 |

零超出预期符号集的 impact；无跨层调用、无循环依赖、零新增依赖（log_semantics.c/.h 纯 C 无芯片/FreeRTOS/LVGL 依赖，设备树与 host_app 同源）。

## 3. test-scope 清单（final verify 消费）

范围 = 本包 `acceptanceTestsFile` ∪ blast radius 映射测试 ∪ 共享 seam 相关测试 ∪ 安全余量。落定如下（**host 全量 runner 一次覆盖全集**，不做挑选式缩档）：

1. **本包验收件**：`firmware/main/test/diag_log.acceptance.c`（DLG-A1..A5 自动断言；DLG-A6 = structural 真机轮观测项，不计 runner 通过面）。
2. **冻结回归网（共享 seam 冻结件，floor）**：
   - `firmware/main/test/sd_log.acceptance.c`（落卡管线 [C1]-[C5]，[C4] 三锚：链式转调在先 / ring_write 消费 / 写盘任务零 ESP_LOG——本包动了 KidpalSdLogVprintf 体内，必须全量在跑）；
   - `firmware/main/test/key_layer_dispatch.acceptance.c`（按键路由 seam）；
   - `firmware/main/test/time_sync.acceptance.c`（K5 时间可信门 kidpal_ts_year_ok——时间前缀 synced 判定消费方）；
   - `firmware/main/test/retract_p4_three_page.acceptance.c`（M2K2-A1 冻结 `rn_render_kidpal_page_locked(kidpal_page_id_t page)` 签名文本锚——builder D2 取舍「单参保持 + trigger 旁路标注」即为保此件绿）；
   - `firmware/main/test/host_infra_smoke.acceptance.c`（包 0 boundary 齿注册序守卫——diag_log 登记行必须置于 smoke 之前）。
3. **blast radius 映射测试**：impacted 33 符号全部由 host 全量 runner（`host_infra_test.elf`，227 Tests 面）覆盖，无 runner 外独立测试需要单独跑。
4. **安全余量**：host 全量套件 = 上列 1+2 并集 + 既有全档；回归判据 = 零新增红 vs 基线 222T3F（先存红 p1_auxbar AB-A1/A2/A4，锚 5412af06），转绿后实态 227 Tests 3 Failures。
5. **设备编译闸**（DLG-X2，final 相位恒跑）+ **落卡管线口径零 diff**（DLG-X3：分区表 / sdkconfig / 16MB 封顶 / 轮转数量零改动）。

## 4. final bearing verification 执行位

- 命令形（scope-file 缺失 = 红闸，本文件即清单）：
  `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261022-diag-log-content/scope-final.md --change-id 261022-diag-log-content --round <final round> --invocation-id <uuid>`
- **本轮未执行**：前置 docs-sync 红未消（spec 面落盘交回持写权者，见 docs-delta.md），final 相位跑必红；final round / invocationId 由 planner 在 docs-sync 转绿后签发时一并跑，随后 `verifier-evidence-check.mjs --phase final` 判 fact `ok:true` 方进 closeout。
- test-scope 清单落盘路径（一行，final verifier fact / review-packet results 元数据必含）：`delivery/taskpackages/261022-diag-log-content/scope-final.md`。
