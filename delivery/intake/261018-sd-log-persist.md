---
changeId: 261018-sd-log-persist
sources: 2026-10-18 human 原话逐字——①「现在我们有TR 卡了，可以存日志了，把板子的日志怎么存下来，先出一个方案吧」；② 方案与容量口径对齐后「ok，那可以了，走triage」；③ 两项裁定（AskUserQuestion 2026-10-18）= 落卡内容**含对话原文（STT/LLM）**、**常开 + 有界轮转**；④ 容量口径 = **目录总量 16MB**（512KB × 32 个文件，非单文件）。关联引用 = `TECH-261016-sd-story-confidentiality`（同族威胁、非同件）；先例 = `261016-sd-card-mount`（SD 挂载能力就绪）、`261016-p3-story-body-sd-cache`（SD 写面 + ②单包 Full 同构）。查重结论：`delivery/intake/` 在册逐份按意图与触面比对——无同意图 spine；debt 三域未结条无同件（`TECH-261016` 同族非同件——同一威胁模型「儿童数据明文落可拔卡」，但数据类/模块/意图均不同），不消费、不走 §3a 晋升，仅在正文记账暴露面加宽。⑤ 2026-10-18 human 对架构面档位口径的补充原话逐字：「以前由于我们没有 TF 卡，设备的 RAM 存储有限，所以不记日志。现在有 TF 卡了，我们可以记日志了。这是在往好的方向变，并不是原来架构改了什么东西，只是原来受限，现在不受限了而已」——记为本次架构面 PROPOSAL 的立项沿革（约束解除，非架构原则变更）。
faceMap: 需求面**不动**（requirements.md:618-624 NFR-OFF-1 阈值 + v1.64 注记里的「日志」= 业务日志〔家长知情权 = OpenClaw 侧完整日志，:676/:701〕；设备诊断日志本属架构/运维面概念，§6.6 :330 已列「设备串口」为日志源）；架构面**动（必走 /arch-change，主批非轻路径）**（architecture.md:101 §2.4 原则5 / :417 §10 决策24 / :232 §5.3.1 存储行 / :266-268 §5.4 数据表 / :318 §6.2 离线边界 / :330 §6.6 可观测性——决策 24 介质白名单须增列「设备诊断日志（有界轮转、可删除）」+ 六处镜像；architect 判 = 白名单增列 + §5.4 新增数据表行，非文档面措辞级）；契约面**不动**（interface-contract.md:165-169 控制类指令 / :263-277 §四端云协议剖面 / :275 system(reboot) 留二期——无任何日志或诊断控制消息；本方案不新增远程开关，纯板端本地采集 → NO_CONTRACT_CHANGE）；治理面**不动**（零命中 `.claude/**` 机器面 / agent 定义 / gate 登记 / scope-map）；实现面**动**（main.cc:15 app_main / waveshare-s3-rlcd-4.2.cc:840 InitializeSdCard、:850 max_files=5、:865-867 挂载成功点、:971 SdCardMounted、:992 构造调用 / firmware/main/CMakeLists.txt:1-130 显式 SOURCES 清单）。触面 = 2（架构 + 实现）→ 建本 spine。
facePlan: **执行顺序 = 架构面先行、实现面后置**（architect 2026-10-18 比例化 review 判）。架构面 = `/arch-change` loop（architect 起草 PROPOSAL + architect-reviewer 四查 + Track B 后段落地；决策 24 carve-out 与六处镜像随批原子落）；需求 / 契约 / 治理面不进 loop。实现面 = 本门五档分档收尾，承载形态 **②单包 Full**（rigor=full、riskTier=high、sourceType=proposal），reviewed plan 落 `delivery/taskpackages/261018-sd-log-persist/plan.md`。**launch 硬前置 = 架构面 anchors 回填（PROPOSAL FROZEN + landing）**——change-intake §8：先于实现面的 anchors 未回填 = 上游未冻结 → 承载包不得 launch。
anchors: {req: "不动（零需求语义变更）", arch: "已回填——PROPOSAL FROZEN v1.0 + landing 完成（commit 5a37b001：docs/architecture.md 11 insertions / 5 deletions + docs/ADR.md 8 insertions / 0 deletions；landing-verdict GREEN；review INDEX c4f61d90）", contract: "不动（NO_CONTRACT_CHANGE）", imp: "已回填——TaskPackage 261018-sd-log-persist 合入本地 main（merge 3baf4981，SLP-A6 真机四项 2 PASS + 2 如实标注 + fsync 持久性加测 PASS）；姊妹包 261018-sd-log-usb-read（USB 只读取回，human-ad-hoc 无 spine）另于 d77985e1 合入"}
openQ: OQ-1（L1 主线程已裁）隐私记账落点 = `TECH-261016-sd-story-confidentiality` 正文 append 一句（暴露面加宽至 STT/LLM 对话转录）+ 架构 carve-out 措辞带「隐私接受」注记；两者都做，成本极低、防随批次遗忘。OQ-2（L1 主线程已裁）requirements.md 不动——NFR-OFF-1「日志」= 业务日志的消歧句只落架构面 §6.6 + 决策 24 carve-out。OQ-3（L0 builder 可裁）sd_log 纯逻辑 / 芯片面分离——architect 倾向分离（复用 story_cache 先例），reviewed plan 已按分离写（`sd_log.c` host 同源 + `sd_log_fs.cc` 芯片面）；builder 若坚持单文件须在包内注明纯逻辑覆盖降级为真机轮。OQ-4（L2，human 2026-10-18 已裁）确认 **②单包 Full**（riskTier=high）；触 security/隐私轴，预授权 must-回问枚举命中，未走预授权。OQ-5（L2，human 2026-10-18 已答）架构面档位口径——human 认为「约束解除、非架构原则变更」（原话见 sources⑤）；机械判档仍归 architect-reviewer（轻路径判据①「零语义变化」不满足：白名单增列 + §5.4 新增数据表行），该口径作为 PROPOSAL 立项沿革随批落。
status: closed
---

# spine · 板端诊断日志落 TF 卡（261018-sd-log-persist）

> 触面 ≥2 跨面变更的路由账本（[change-intake.md](../../.claude/references/change-intake.md) 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。

## 建档前查重（change-intake.md §3）

- **既有 spine 排查**：`delivery/intake/` 在册逐份按意图与触面比对——无「日志持久化」同意图件。最近的 `261016-p3-story-body-sd-cache` 是 story 正文只读镜像（不同数据类、不同模块），不重复建档。
- **debt 三域排查**：命中 `TECH-261016-sd-story-confidentiality`（status: deferred）——**同族非同件**。同族 = 同一威胁模型「儿童数据明文落可拔 SD 卡、读卡器直读」；非同件 = 数据类不同（story 正文 vs 诊断日志含对话原文）、模块不同（`story_sd_io` vs `sd_log`）、意图不同。不消费该条目、不走 §3a 晋升；但本变更的「含对话原文」把该条目的暴露面**加宽**（全部对话转录 vs 孩子主动录的故事），须按 OQ-1 记账。

## 触面事实（查出来的，非申报）

| 面 | 动不动 | 动哪 | 依据 |
|---|---|---|---|
| 需求 | ❌ | 零改动——NFR-OFF-1 里的「日志」= 业务日志（家长知情权走 OpenClaw 侧） | requirements.md:618-624 / :676 / :701 |
| 架构 | ✅（主批，非轻路径） | 决策 24 白名单增列「设备诊断日志（有界轮转、可删除）」+ 六处镜像 | architecture.md:101 / :417 / :232 / :266-268 / :318 / :330 |
| 契约 | ❌ | 零 wire/API 变更；不采用 system command 远程开关 | interface-contract.md:165-169 / :263-277 / :275 |
| 治理 | ❌ | 零命中 `.claude/**` 机器面 / agent 定义 / gate / scope-map | — |
| 实现 | ✅ | `diagnostics/sd_log.{c,h}` + `sd_log_fs.cc`；main.cc 早期装钩子；板级挂载成功 attach + max_files 5→8；两处 CMakeLists 登记 | main.cc:15 / waveshare-s3-rlcd-4.2.cc:840,850,865-867,971,992 / firmware/main/CMakeLists.txt:1-130 |

## 方案要点（human 已拍板）

- 采集：`esp_log_set_vprintf()` 钩子（app_main 最早期、链式转调原 vprintf 保串口不中断）+ PSRAM 128KB 环形缓冲 + 低优先级写盘任务（1s 或 4KB 批量刷盘；写盘任务内禁调 ESP_LOG 防递归）。
- 落盘：`/sdcard/logs/KPLOGnnn.TXT`（8.3 兼容名），单文件 512KB × 32 个 ≈ **目录总量 16MB**，循环覆盖最旧。
- 内容：默认 INFO 及以上；**含对话原文（STT/LLM）**（human 2026-10-18 拍板）。
- 常开 + 有界轮转（human 2026-10-18 拍板），不做手动诊断模式。
- 边界：panic/OOPS 不落卡；bootloader 日志抓不到；热插拔不做（写失败 30s 重试）；组件内 `printf` 不经钩子；分区表与 sdkconfig 零改动。

## 档位裁定（architect 2026-10-18 比例化 review）

- **②单包 Full**，riskTier=high，sourceType=proposal，rigor=full，`reviewed-by: architect`，date=2026-10-18。
- 非 ①：改实现面行为。非 ⓪ nano：排除清单多命中（多文件面 + 验收语义净增 + 触 security/隐私轴 + 全局钩子为中枢符号 + 触架构面）。非 ③：单切片、单 owning 子系统，无跨 Pod/迁移。非 ② Lite：**触 security/隐私轴**——对话原文明文落可拔卡，与 `261016-p3-story-body-sd-cache`（riskTier=high / Full）同构且更敏感；存疑向严 → Full。
- **lite 判注义务结论**：设备编译面深（vprintf 钩子 / FATFS 写 / 板级 attach 均为 device-only seam，host_app target 盖不住）；纯逻辑（环形缓冲 / 轮转 / 文件名）可 host 齿闭环。按 Full 环境备。
- **触点①**：因「触 security 轴」不可预授权，须 human 真确认五档——**human 2026-10-18 已确认 ②单包 Full**（architect 裁决未被改判）。
- **逃生门**：architect 判定契约面 `NO_CONTRACT_CHANGE`，无需另派 architect-reviewer 四分类。

## 立项 ack

**human 2026-10-18 已确认**（触点①）。四面对齐——为什么改（设备脱离串口独立运行也要能事后查真相：课表幻觉 TECH-261018、ASR 误听等真机长尾）/ 往哪改（决策 24 白名单 + 六处镜像先行，实现面按 reviewed plan 落卡）/ 改动多大（②单包 Full，riskTier=high）。
实现面 reviewed plan 已落 [delivery/taskpackages/261018-sd-log-persist/plan.md](../taskpackages/261018-sd-log-persist/plan.md)。

**架构面档位口径（human 2026-10-18 补充）**：human 判断「原先是 TF 卡/RAM 约束导致不记日志，现在约束解除，不是架构原则变了」——原话已入 sources⑤，作为 `/arch-change` PROPOSAL 的立项沿革。轻/主判档仍由 architect-reviewer 按 SSOT 判据裁定（轻路径判据①「零语义变化」对白名单增列与 §5.4 新增数据表行不满足），human 口径不改机械判档结论，只改 PROPOSAL 的叙事与沿革定位。

**编排车道（human 2026-10-18 裁「保持现状」）**：架构面 headless 会话在 `C:/AI/KidPal-pod-m2`（分支 `dev/pod-m2`）运行；arch 面制品与 landing 随该分支收口后合并进 main，不另起 gov worktree 车道。

**架构面立项 ack（human 2026-10-18）**：human 已确认 PROPOSAL T1–T8 与「约束解除」叙事定位，architect-reviewer 四查 → FROZEN → landing → landing-verifier → retro 由 resume 的 headless 会话继续；合并本地 main 归主线程后续处理（本 worktree 不能 checkout main）。
