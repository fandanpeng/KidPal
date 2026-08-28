# Test Quality Antipatterns

这些反模式在 acceptance-author、verifier、fixer 中共享。verifier 发现后必须报红；fixer 修复时应补最小实现或测试支撑，而不是放宽断言。

| Antipattern | Signal | Required response |
|---|---|---|
| Status-only API test | 只断言 `status === 200` 或 `.status(200)` | 增加 schema、关键字段、DB 副作用、事件断言 |
| Page-open UI test | 只断言页面打开、标题存在、没有 console error | 增加状态变化、内容、错误态或导航断言 |
| String-contains test | 只用 `toContain("keyword")` 判断成功 | 换成精确 locator、结构化字段或 schema 断言 |
| Mock-call test | 只验证 mock 被调用 | 至少一层测试走真实服务/存储/事件路径 |
| Over-mocking | 被测模块和所有依赖都 mock | 保留真实代码路径，只 mock 外部不可控边界 |
| Happy-path only | 只测成功输入 | 增加错误输入、权限拒绝、空值或边界测试 |
| Snapshot-only | 只靠大快照 | 增加命名断言解释关键行为 |
| Source-grep test | 读实现源码 + 正则/toContain 关键字（debounce、RETRY、常量名、魔法数字如 300000）当行为证据 | 断言运行时效应：调用计数、事件发出、时间间隔行为、渲染输出；源码存在性只作辅助信号，不单独构成验收 |
| Pure-structure acceptance set | acceptance 全部是存在/渲染/编译条目，无任何决策行为条目 | 每个 source 行为决策至少一条行为断言；集合不满足则退回 planner |

## Fixer guard

修复 verifier 报出的真实缺陷后，fixer 必须补一个 guard：相关单元、集成或验收断言，能在同类回归再次发生时失败。若当前角色无权修改该测试层，升级给 planner。