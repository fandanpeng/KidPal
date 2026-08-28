# Security Hardening Reference

当 TaskPackage 声明 `securityReviewRequired: true`，或 story 涉及用户输入、鉴权、权限、文件/网络、外部 API、AI 工具、secret、跨租户数据时，启用 security gate。

## Review axes

- Boundary validation: 所有系统边界输入用 zod/schema 校验，错误形态稳定。
- AuthN/AuthZ: 用户身份、角色、机会/租户/作用域权限被明确检查。
- Data isolation: 查询和写入不能跨租户、跨 opportunity 或越过 scope。
- Secrets: secret 只从受控环境读取，不进日志、响应、测试快照或事件 payload。
- Injection: SQL、shell、prompt、path、URL、template 输入有约束或参数化。
- Dependencies: 新增依赖有必要性说明，无明显供应链风险。
- AI tools: read-only 与 mutating 工具按 scope 隔离，危险动作需要人工门控或明确授权。

## Evidence

TaskPackage 或 test-evidence 必须记录：检查过哪些轴、对应测试/代码位置、剩余风险。缺证据时，security gate 报红。