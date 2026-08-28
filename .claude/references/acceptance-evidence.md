# Acceptance Evidence Reference

验收测试定义“做完”的含义。它们必须断言真实结果，而不是表面状态或“没有报错”。

## 必要证据

- 每条验收必须绑定一个 TaskPackage acceptanceRef/sourceRef，用测试名或注释标记 `coverage: <ref>`。正式产品故事通常是 `Sx-Ay`；架构 review 修复、临时任务或 CI/drift 修复可以是 `D-A1`、`ARQ-063-C1`、`TEMP-*`、`DRIFT-*` 等稳定引用。
- API 验收必须同时断言响应 schema、关键字段、DB 副作用、领域事件；必要时断言审计/成本记录。
- UI 验收必须断言状态变化与具体内容，例如列表新增、详情跳转、错误态呈现；不能只断言页面能打开。
- AI 功能验收必须捕获工具调用、校验工具参数、校验作用域约束（如只读 scope 不得调用 mutating 工具），并确认响应没有编造工具结果。
- 错误路径至少覆盖一个边界/权限/无效输入场景。

## UI 验收 vs 设计 review

有 UI 的切片，把 UI 语句分成两桶，避免把纯审美钉成脆测试：

**必须成为验收测试**（任务关键、可断言真实行为）：用户任务关键状态；数据可见性/内容正确性；影响用户决策的空/错误/权限/加载态；导航与 deep-link 行为；业务门/确认态；基础可访问性可操作性。

**属于设计 review、不进脆验收测试**：纯审美；微观间距/颜色；非关键动画；不改变用户任务成败的视觉打磨。这些由 integrator 的 text-only design-review 证据承载（见 dev-loop.md 合并门 Review Packet），不写成断言。

## 红基线

acceptance-author 写完验收后必须运行红基线。红基线失败原因必须对应当前 story 缺失的真实行为；环境缺依赖、测试写错、选择器失效不能算红基线。

全新页面/组件类 story 的所有测试会以同一原因（组件不存在）变红，此类统一红对逐条断言敏感性**零信息**——高风险包此时红基线结果应按条目给出映射（哪条断言对应哪个缺失行为），或显式声明依赖后续 mutation 兜底（且不得豁免 mutation）。

## 破坏性自检

verifier 或 integrator 能执行 mutation check 时，应临时破坏一个关键行为并重跑验收。验收必须因此变红；若仍绿，判定为 vacuous acceptance。

## 反例

```ts
expect(response.status).toBe(200);
expect(page.title()).toBeTruthy();
expect(mockFn).toHaveBeenCalled();
expect(bodyText).toContain("success");
```

这些断言只能作为辅助信号，不能单独作为验收通过依据。