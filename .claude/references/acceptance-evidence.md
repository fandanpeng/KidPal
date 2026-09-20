# Acceptance Evidence Reference

验收测试定义“做完”的含义。它们必须断言真实结果，而不是表面状态或“没有报错”。

## 必要证据

- 每条验收必须绑定一个 TaskPackage acceptanceRef/sourceRef，用测试名或注释标记 `coverage: <ref>`。正式产品故事通常是 `Sx-Ay`；架构 review 修复、临时任务或 CI/drift 修复可以是 `D-A1`、`ARQ-063-C1`、`TEMP-*`、`DRIFT-*` 等稳定引用。
- API 验收必须同时断言响应 schema、关键字段、DB 副作用、领域事件；必要时断言审计/成本记录。
- UI 验收必须断言状态变化与具体内容，例如列表新增、详情跳转、错误态呈现；不能只断言页面能打开。
- AI 功能验收必须捕获工具调用、校验工具参数、校验作用域约束（如只读 scope 不得调用 mutating 工具），并确认响应没有编造工具结果。
- 错误路径至少覆盖一个边界/权限/无效输入场景。
- 版本化制品（`*.vN` 落盘物，如 `server/deploy/openclaw-system-prompt.vN.md`、engine-patch、overlay 配置）的**接线类 green-guard 必须版本无关**，三步：① 解析当前被接线 / 渲染的那一版文件名——从结构化指向读（渲染面如 `apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC`、声明面如 `provider.config.yaml` 的 `system_prompt.artifact|source|deploy_target`），不从目录猜；② 断言该文件确实落盘（防孤儿）；③ 断言其正文逐字携带本包语义锚点（防未来版丢内容）。**禁在接线类断言里硬写版本字面文件名**：前序包 forward-bump 后字面陈旧，守卫要么假红（断言字面陈旧非行为破损、需 human ruling 才放行），要么假绿（needle 在别的版本正文里偶然命中，丢正文侧静默报绿——schedule-reminder 实测先例）。**落盘 / 措辞类守卫钉死某版合理**（如断言某版基座结构、措辞收紧），撰写时以固定 token 注释标注类别 `<!-- guard-class: wiring | landing | wording -->`，与接线类区分（guard-class token = 接线 / 落盘措辞二分的类别判据）。重指母版 = `test_e3_current_prompt_wired_and_carries_char_c1`（human 裁 `c54bbcde`）；清点范围 = 全仓 grep `openclaw-system-prompt.v[0-9]` 于验收断言字面处逐条判「接线守卫（该版本无关化）vs 落盘 / 措辞守卫（钉死某版合理）」。

## UI 验收 vs 设计 review

有 UI 的切片，把 UI 语句分成两桶，避免把纯审美钉成脆测试：

**必须成为验收测试**（任务关键、可断言真实行为）：用户任务关键状态；数据可见性/内容正确性；影响用户决策的空/错误/权限/加载态；导航与 deep-link 行为；业务门/确认态；基础可访问性可操作性。

**属于设计 review、不进脆验收测试**：纯审美；微观间距/颜色；非关键动画；不改变用户任务成败的视觉打磨。这些由 integrator 的 text-only design-review 证据承载（见 dev-loop.md 合并门 Review Packet），不写成断言。

## 红基线

acceptance-author 写完验收后必须运行红基线。红基线失败原因必须对应当前 story 缺失的真实行为；环境缺依赖、测试写错、选择器失效不能算红基线。

全新页面/组件类 story 的所有测试会以同一原因（组件不存在）变红，此类统一红对逐条断言敏感性**零信息**——高风险包此时红基线结果应按条目给出映射（哪条断言对应哪个缺失行为），或显式声明依赖后续 mutation 兜底（且不得豁免 mutation）。

## 破坏性自检

verifier 或 integrator 能执行 mutation check 时，应临时破坏一个关键行为并重跑验收。验收必须因此变红；若仍绿，判定为 vacuous acceptance。

## 契约条目 ↔ 齿 的强度申报

验收纪律管住了三件事——**有没有齿**（acceptance-coverage）、**齿敏不敏感**（mutation-check 的 `sensitive`）、**齿是否空过**（vacuity）——但**不管「齿断言的性质够不够契约要求的强度」**：一条「按下后界面变化了」的齿，与契约要求的「按下的必须是 GPIO18 那条电平路径」之间，在现有全部绿信号里没有任何差别。**强度是设计判断、不是可推导属性**，故本条以撰写约定承载（260920-gov-review-bearing-strength-align T2，FROZEN v1.0）；**不设门**——判断型申报无法机判，消费方是 acceptance-author 产齿时写、integrator 与判官按冻结件头注核。

产齿时逐条申报四件：

1. **映射**：列「契约 / 设计条目 → 承载齿 → **该齿断言的性质**」。性质分三档——**机制级**（钉具体接线 / 资产 / live 配置 / 符号路径）/ **行为级**（钉可观察行为）/ **源锚代理**（钉决策代理物，如源码文本锚——**非行为本体**，须自陈是代理）。
2. **弱于契约处显式声明**：凡承载齿的强度档低于契约要求者，逐条写明缺口与理由。
3. **行为等价假设必须显式化**（如 `BOOT≈KEY`、「同名符号即接线」）——**不写即默认不成立**。隐性等价假设正是「齿绿而契约未兑现」的主要载体：`BOOT≈KEY` 使「KEY 未接线」原理上不可见，绿信号对等价假设无免疫。
4. **落点 = 冻结验收件头注**：映射与缺口逐条写进冻结件头注。既成先例 `story_p3` 冻结头注 ⑤「当前屏正文」即该体例的**自发形态**；该先例的病灶是「头注写了强度要求、却无齿兑现、且事后无回写」——故本条要求**缺口逐条写出**，而不只是写要求。

**机制断言优先**：能钉「哪根 GPIO / 哪个资产 / 哪个 live 配置」的，不断「行为看起来对」。

## evidence 类扫描齿的 root 注入

**evidence 类扫描齿**（判定依据是仓内制品中的标记，如 VRT live 探针、WSA-A4/A6 证据门齿）**须显式接收仓根 / 由 runner 注入 root，禁依赖调用 cwd**（260920-gov-review-bearing-strength-align T2 搭车件，承 `ARQ-260908-known-red-count-no-mechanical-source` 子项②）。

**实测反例**：同一 ELF 在**文档命令位置**（`firmware/main/test/host_app/`——`firmware/main/test/HOST_TEST_CONVENTIONS.md:66` [C4.4] 运行命令约定规定的 `cd firmware/main/test/host_app && …` 位）跑出 **3** 条既有红，在**仓库根**跑出 **2** 条——同一实现两份数字，红基线与 mutation 判读全靠人肉对口径；且判读口径若错，会把既有红误记成本包回归（实测发生过一次）。

**撰写形态**：齿内取仓根走显式参数（或 runner 注入的环境变量 / 常量单源），不得用相对路径隐式依赖调用点 cwd。**为什么重要**：evidence 类齿判定不稳定 = 「同一实现两次跑出不同红名单」，与「证据可重现」的要求直接相悖；手写快照随 main 前进必然腐化、无门报警。

## 反例

```ts
expect(response.status).toBe(200);
expect(page.title()).toBeTruthy();
expect(mockFn).toHaveBeenCalled();
expect(bodyText).toContain("success");
```

这些断言只能作为辅助信号，不能单独作为验收通过依据。

**接线类守卫硬写版本字面反例（260908 批）**：

```python
# 反例：接线守卫硬写版本字面——carrier forward-bump v3 后断言字面陈旧，
# 假红（v3 逐字携带内容却断言 v2）或假绿（needle 在别的版本偶然命中）。
assert "openclaw-system-prompt.v2" in render_src or in decl_src
```

正形 = 解析结构化指向（`PROMPT_SRC` / `provider.config.yaml` 声明面）得当前版本再断言落盘 + 语义锚点（母版 `test_e3_current_prompt_wired_and_carries_char_c1`，human 裁 `c54bbcde`）；类别标注 `<!-- guard-class: wiring | landing | wording -->` token 见「必要证据」段末条。