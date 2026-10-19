# pod-retro · 261018-sd-log-persist（第一人称热记忆）

> 标签：第一人称热记忆。只记我在场的过程体感与判断，不复述机器数据；token / gates / rounds / verdict / 测试数字以 machine artifacts、pod-events、verifier-evidence 为权威。

## 这个包整体给我的感觉

这是少见的一条「从架构冻结一路到真机门前」都很顺的包：plan 是 reviewed 的、决策边界清楚、验收 seam 在派单时就写死了，builder 一次就把 host 齿与设备编译都做绿了。真正让我停下来想很久的，不是实现难度，而是**跨包冻结验收的字面冲突**——一个「5 还是 8」的小数字，背后是冻结件治理的授权边界。

## 卡点与绕轮的主观判断

- **最大的卡点（friction: scan-miss）**：intake 时我只核了本包 changeManifest 与 scope-map，没有扫「既有冻结件里钉住同一实现点的字面锚」。结果 builder 首轮 host 全量才暴露 `sd_card_mount.acceptance.c` 钉 `max_files=5`、本包要 8。我的判断：这不是谁写错了，而是 intake 缺少一个「相邻冻结齿字面锚」的扫查动作。已落 friction 交 pod-retro → ARQ，值得在 1g 清单里补一条。
- **停下来问人的那一次是对的**：architect 的 10b advisory 说得很直白——plan 批了「数值变 8」，但没批「改另一个已合并包的冻结件」，这两件事在授权对象上不是一回事。我一度想过「plan 都批了，直接改吧」，但那会造出一个隐性先例（plan 提到某值 ⇒ 可改任何钉该值的冻结齿）。人类的窄口径批准 + 防先例句落进 REVISION LOG，让这次改动从「顺手」变成「有据」。
- **acceptance-author 的两轮产出质量差异**：第一轮冻结本包齿时，把 seam 契约定得非常硬（函数名、常量、weak 哨兵、双向可满足性、六项定向变异），这直接让 builder 一次落地；第二轮重锚旧齿时，它把 REVISION LOG 的 sha256 自指问题主动说清（日志写自身哈希不可自洽，改用「代码面后值 + 最终哈希随 commit 记录」），这个诚实度我很满意。
- **mutation 取证的机械坑**：mutation-check 按 raw-id 语义要求证据里出现完整 acceptance 串，我一开始只放短 id，闸红了一次。这不是闸坏，是 ARQ-097 的故意分歧；补 full-id 字段后即绿。记一笔，下次派 builder 变异时直接在派单里写明。
- **闸的提前预检很值**：security 闸要求证据文本逐字包含每个 securityAxes 串、docs-sync 闸要求 docRefs 提及验收 id 或给 no-change 理由。我在 verifier 之前先跑了一遍，省了一轮 fixer 往返。

## 别扭的 acceptance / 规则

- SLP-A6（真机四项观察）在 acceptance 里是 structural 豁免形态，但它其实是这个包**唯一能证明「真的落卡了」**的验收。host 齿证明的是逻辑与接线，真机轮才是产品事实。我在 checklist 里把它写成逐条判据，避免 human 持板时「看一眼觉得没问题」草草过。
- 「tighten-only 窗口」条文与「后续包重锚旧齿」的现实之间有一道缝：条文只写 additive，先例（261013）却是直接改既有断言的。这次用 human 窄口径裁决把缝补上，但下次还会遇到；根因还是 intake/1g 不扫相邻冻结齿。

## 帮倒忙 / 帮上忙的钩子与规则

- 帮上忙：`change-manifest-check` 的对账面定义（firmware/server）、`closeout-check` 的 requiredFiles、`verifier-evidence-check` 的 immutable fact 口径，都让「先扩单后动文件」「只认 r2 fact」这类纪律变成机械动作而不是口头承诺。
- 稍显别扭：`change-manifest-check` 因为路径首提日被 `--follow` 解析成很久以前，走了 pre-cutoff 放行（skip）。结果是「闸绿了但其实没检查」。我没有依赖它，人工做了 closed-set 对账；但这种「绿得没有信息量」的形态值得记一笔。

## 人工纠偏体感

human 的两次介入都很省：一次是窄口径批准重锚（原话直接进 REVISION LOG），一次是明确「先 integrator、再真机门」的顺序。没有推翻技术判断，只补授权与顺序，这正是我期望的人类触点。

## 下次改进

1. intake / 1g 增加一条「既有冻结件相邻字面锚」扫查（本包根因，交 ARQ）。
2. 派 builder 做 mutation 时，派单直接要求 mutationEvidence 记录携带完整 acceptance 串（避免 raw-id 语义往返）。
3. 真机 checklist 继续坚持「逐条判据 + 留痕形态」，不要退化成截图观感。

## 修复轮补记（2026-10-19，真机缺陷 fsync）

真机轮抓到一个 host 齿结构上盖不住的缺陷：fflush 只推 RAM 文件对象、FAT 目录项不落卡，硬复位丢全部已写日志。这件事给我两个体感：一，`SLP-A6 live-manual` 的价值这次是真的——它是唯一能抓到「数据是否真的活过掉电」的验收位，host 侧 210 齿全绿也替不了；二，我在 intake 时对「持久性」这个词的理解停在「fwrite 成功 = 落盘」，没有追问 FAT 语义下 fflush 与 fsync 的差别——plan 写「每 1 秒 fflush」，我照单全收，没有用架构视角复核这句技术表述。下次 intake 读 plan 里带 I/O 语义的句子时，要把「这句话在真实介质上成立吗」当一道必答题。

修复与回归齿的流程本身很顺：human 授权（b0da8396）→ fixer 最小补丁 → tighten-only additive 齿（五件套）→ r3/r4 复验全绿。冻结件治理的两条通道（窄口径重锚、additive 窗口）这次各用了一次，都比我预期的省力。
