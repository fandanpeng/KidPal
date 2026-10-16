# retro · 260929-gov-review-req-loop-ui-author

> Track B 闭卷复盘（趁热写、先于合并）。制品四件齐：PROPOSAL（FROZEN v1.0）+ landing-summary + landing-verdict（GREEN）+ 本件。

## 1. 本批做成了什么

需求变更 loop 接上 UI 面：档 2 触 UI 批次走「uiux-author 生成模式产设计稿与出图规格单 → 主线程代执行出图（400×300 PNG 定位权威）→ reviewer 扩五查 → human 终拍看图拍板」；下游 builder 经 brief-inputs.mjs ①c 节直接拿页级 PNG 定 layout 坐标。九文件落地、验证集 V1–V6 全绿、landing-verifier 9/9 OK + 破坏性自检全过。

## 2. 起草与评审摩擦（下次照抄 / 避开）

1. **锚行加粗病又犯了一次**（`**目标文件**：` → preflight `bold-anchor-line` 当场点名）——模板读法注第 2 条记载的 ARQ-260914 原始错误形态，本批起草者照犯。**教训**：写完 §2 先跑 `--preflight` 再写后文，别攒到最后。
2. **R1 判官抓出的最大缺口是「SSOT 改了、周边 def 没跟上」**（story-author 自持清单 / reviewer 路由信号 / planner·arch-change 悬空句）——角色 def 自持枚举与流程 SSOT 是双源，改流程必 sweep 四处：def 正文、def description（route-fixtures 必留信号）、入口命令、分工/roster 面。本批 T6 形态（单立同步批 T）可复用。
3. **tools 高敏字段的绕行方案有效**：出图执行裁「主线程代执行」（agent 产规格单、主线程跑 od_*）避免扩 agent tools 面 → 不触发 human 补 ack、不改 dispatch 白名单。同类「agent 缺工具」场景优先想职责切分而不是加工具。
4. **description 超长告警（364>320）**：双模式 def 的 description 天然膨胀，首稿把两种模式的派发细节全塞了进去；修剪后路由信号零损失。**写法定例**：两模式各留一个触发锚（Step 6b / 档 2 4b），细节留正文。

## 3. 留根交接（PROPOSAL §6 五条 + 执行测出两条）

- 首个触 UI 需求批实跑后回收：出图粒度定例（全量 vs 增量）、brief↔requirements 版本对齐机检齿、uiux-opendesign-loop 程序 SSOT 成文时机。
- L0/L1 包不强制挂图的门槛复议（T5 证据 4 到场率问题）——loop-retro cadence 关注。
- DEV-GOVERNANCE roster 行（G1 面）走独立 human-approved 批。
- 本批执行测出：landing-verifier 点名「分工表新增行是 T1 清单未明列的配套」——PROPOSAL 模板的「目标文件锚清单」粒度只到文件级，同文件内的结构面增补（表行/节）容易漏申报；候选改进 = 锚清单括注到节级（登记不硬治，交 loop-retro）。

## 4. 模型行为自评

主线程本批四轮（对齐 → 起草 → R1 修订 → 落地）：判官独立核出 4 BLOCKING 全部成立且修订映射逐条对应；landing-verifier 未发现少落。无跳步、无凭解释宣布完成。
