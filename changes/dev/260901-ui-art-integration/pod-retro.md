# Pod Retro — 260901-ui-art-integration（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 本 Pod 最有价值的教训

**planner 自己的输出也会被杂讯污染——长中文 payload 走 bash heredoc / Write 脚本时混入假条目（谕用/捫皮乱码、kidcat/corruption 假键、语法坏句）**。本 Pod crossEnd 申报连续三次写坏（两次 heredoc、一次 Write 脚本），靠「删脚本 → Read 盘上实况 → Edit 定点小修」收敛。教训固化：**结构性 JSON 编辑宁可用 Edit 工具做最小定点替换（先 Read 拿逐字 old_string），不在 bash heredoc 里拼长中文数据结构；写完必须 `node -e require()` 解析校验再进闸**。fixer 同理报过「Edit 被机械闸拦」——那不是杂讯，是 scope-map 正确执法，两件事别混。

## 卡点

- **input-brief 生成目录与闸读目录错位**：brief-inputs 产到 pod 目录，taskpackage-required-check 却在 current/ 目录找 brief——第一次 verifier scoped 相位红在这里。双份 cp 同步即解，但这是流程摩擦，值得机械收敛（闸或脚本二选一改）。
- **mutation-check 按精确串匹配**：builder 回填 mutationEvidence 时把 acceptance 条目凭记忆缩写（A5 缩词、A7 丢尾缀），闸判 missingSensitive。教训已进先例：**回填 acceptance 字段必须逐字复制 taskpackage 原文，不改写**。
- **docs-sync 闸的 docRef 弱 token 要求无人在册可写**：docRef 文档 validation-batch-conclusion.md 缺 ART-A* 锚点，fixer 被 taskpackages 外的 scope 拦、integrator 的 allow 不含 docs/notes、planner deny docs/**——三方都写不了，最后主线程上下文落盘成功（子代理被拦、主线程上下文未被拦，行为差异如实记录）。这是结构缺口：**docRef 指向 docs/notes 的包，锚点 append 该显式规划为主线程动作**，或把 docs/notes 加进 integrator allow。
- **HOST_ACCEPTANCE_SRCS 序齿的 strstr 语义**：验收 tooth 6 用 strstr 对 raw 文件判序，pre-existing 头注 comment 里的 smoke 文件名字面量会误判——builder 重排 comment 字面量化解（不涉列表顺序）。冻结验收文件内的文本匹配齿，写时要想「这个子串还会在哪些 comment 里出现」。
- **红基线口径勘误**：launch 预言「验收文件不存在 → 编译收集即失败」，实测真红 = stub 实跑 10 红（文件未接线不参与 host 收集，编译通道零错误）。判读相对基线时这个勘误进了 taskpackage redBaselineResult，避免了后续误判。

## 顺的部分

- ART-E4 前置解锁干净：V3 定稿 + 47 枚重出图 + 机判 30/30 在 launch 前完成，acceptance-author 第一步核对稿版零阻塞。
- acceptance-author 的 weak 哨兵 -2 模式（照 HP1 先例）+ 47 枚闭集白名单（真记录实测采样）让 builder 有精确施工图，一轮转绿。
- builder 一次交付：11/11 全绿、设备 build exit 0、mutation 6/6 实跑 turnedRed（含一条诚实的首探不敏感注记——A6 include 只注释不删字面量不红）。
- assets 分区共存冲突按 BUILDER DUTY 3 上报而非静默双写，planner 裁定互斥守卫默认休眠、接管归 9b——边界正确。

## 下次改进

1. planner 派发模板加一句：**mutationEvidence 的 acceptance 字段 = taskpackage 原文逐字复制**；crossEnd/长 JSON 申报用 Edit 定点修，禁 heredoc 拼长中文。
2. brief-inputs 与 taskpackage-required-check 的 brief 目录语义对齐（机械收敛项，可进 ARQ）。
3. docRef 指 docs/notes 的包，launch prompt 直接写明「锚点 append = 主线程动作」，省一轮三方被拦的往返。
4. verify_art_assets.mjs 路径常量绝对路径指主 worktree（acceptance-author 摘要 §降级 4 在案）——跨 worktree 复跑机判会踩，归 9b 前修。
5. loop-retro 阈值已到（closeout advisory：9 ≥ 3），发起责任在主线程——本包合并后应跑 /loop-retro。
