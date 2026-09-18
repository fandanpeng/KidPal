# review · m2-char-window

> workstream 级门审裁决耐久记录（append-only；行不覆盖）。

## 门 1 · 拆包切法轻量 checklist review（2026-09-17）

- **裁决**：**GREEN**——切法五项全过，workType 分流明确，可进 human 过目 → launch。
- **审者**：architect-reviewer（只读预审；本节为裁决记录，非契约/Spec 变更）。
- **被审制品**：本 workstream 四件（roadmap.md / decision-coverage.json / progress.md）+ 三包 taskpackage.json/plan.md + 上游锚 `delivery/milestones/milestones.jsonl` 行 `M2-char-window`（planningRefs 已挂接）+ `docs/requirements.md` F3 全节 / §2.2 / §2.3 / §2.4 行 186 / §8 / NFR-PERF-2 + `docs/contracts/interface-contract.md` §1.3 ①。

### 逐项结论

1. **覆盖完整性 — 过**：milestone nextAction 两块全部落包——块① = decision-coverage 第 1 条（两缺陷修复→K1，M2K1-A1–A3）+ 第 2 条（NFR-PERF-2 live 复测→K3，M2K3-A8）；块② = 第 3 条（撤 P4 三页循环→K2，M2K2-A1–A4）+ 第 4 条（页→窗改造含锚 canvas-p4→char-window 改名→K3，M2K3-A6）；第 5 条 S-F3.4 显式 dropped（§8 已砍清单，撤页连根消解，零拆包）。开放问题「语音查下一个字」v1.37 已收窄消解（requirements 行 640），无需挂账；F0 在 ARQ 面独立批，正确不混入。
2. **依赖与排序 — 过**：dependsOn 图成 DAG（K1→K2、K1→K3、K2→K3，无环）；mergeOrder 28/29/30 与依赖边一致；真机半声明清晰——M2K1-A4、M2K2-A5 均以 structural kind 显式 deferred 挂 K3 设备轮，M2K3-A7/A8 为 human 持板确认轮，launchPreconditions 第 4 条钉「K3 设备轮前 K1/K2 host 半须已 merged」。
3. **包边界 — 过**：三包为垂直切片（K1=查字链路稳定能力 / K2=三页循环能力 / K3=弹窗交互包能力），各自可独立验收；跨包共享冻结面修订通道单改不双改——schedule_reminder_b.acceptance.c（K2 独家修订、acceptance-author 留痕）、key_layer_dispatch.acceptance.c 层表断言（K3 独家修订；K2 仅作基层列等价 guard 零回归靶、不改层表），且 K2→K3 合并序串行保证 K3 在新基线上改。注：K2 的 acceptanceTestsFile 同列 key_layer_dispatch.acceptance.c，其语义已由 decision-coverage 注记 ② 钉死为「回归靶非修订面」，无歧义。
4. **story 引用纪律 — 过**：S-F3.1「语音查字并看笔顺演示」（AC1–AC2 承载面 / K3 AC1–AC4）、S-F3.2「看拼音/笔画/部首/组词」（AC1）、S-F3.3「笔顺演示重播」（AC1–AC2）均为 ID+标题+AC 号集锚点引用，且语义权威 = requirements 现文（v1.55 口径已在案）；S-F3.4 已砍零拆包、K2 无 story 回指（撤页对账项引 §2.4 行 186 / §2.2 行 69 / 行 166 ②）符合其对账性质。
5. **契约面 — 过（零契约改动裁定成立）**：逐条核过——① D3 `strokes` = makemeahanzi SVG path 自由文本（契约 §1.3 ① 样例即含 Q；契约未枚举命令词汇表，C 为 makemeahanzi 词汇表内命令），K1 补 C 解析 = 实现侧解析补全，**NO_CONTRACT_CHANGE**（本节即充任 K1 预留的 architect 快审确认，K1 无需再走一轮）；② E1 view=char 既有 display 响应块，覆盖窗显示时机是实现侧导航包装（v1.36 human 裁 §8 在案），服务端下发语义与载荷 shape 零变化；③ AC4 笔名 = OpenClaw 回复文本自由承载（v1.55 试点期），D3 `stroke_names` 扩展显式不做、留契约门回退路线注记。附带观察（不阻断、不入包）：契约 §1.3 ①「板子行为：切到 P4 查字页」一句为 v1.36 撤页前的过时描述性文字，载荷格式未变、两侧消费方理解以 requirements §2.2 行 68 为准；若日后有其它契约改动同 PR 顺带核对措辞即可，不为此单独开契约门。

### workType 分流

| 包 | workType | 说明 |
|---|---|---|
| 260917-m2-k1-char-link-defect-fix | 实现型 | 缺陷修复双管（解析补全 + 栈定容），host 机判为主 + device build 闸，真机半 deferred |
| 260917-m2-k2-retract-p4-three-page | 实现型 | 状态机/枚举/键帽条对账改造，host 机判 + device build 闸，真机半 deferred |
| 260917-m2-k3-char-window-overlay | 实现型（尾部挂 human 真机确认半轮） | 页→窗改造 + 对账三项全 host 机判；A7/A8 为 human 持板确认轮，非研究型 |

三包均 rigor=full、riskTier medium(K1)/high(K2,K3)、sourceType=delivery-story，与切法量级相称。

### 附注

- 任务清单中提及的 workstream `index.jsonl`：本仓 workstream 无此件先例（m1-home-qa 亦无），板面由 render-roadmap.mjs 自 board + taskpackage JSON 机械渲染，**缺此件不构成缺口**。
- 边界外移交三项（milestones planningRefs 挂接已由主线程完成、STATUS.md 重建、dev INDEX 随 closeout 自刷）见 progress.md 2026-09-17 节，不属本门范围。
