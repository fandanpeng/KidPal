# shim.py 归档退役记录（260903-device-ws-switch · WSA-A6 齿 3 锚点件）

- 日期：2026-09-05
- 对象：`server/shim.py`（Pod S 形态的同步 HTTP shim，设备↔shim↔OpenClaw 链路的云端台阶）
- 处置：**归档（archived）**——文件保留在原路径，头部加 ARCHIVED 墓碑注记；代码逻辑零改动（`python -m py_compile` 通过）。

## 为什么归档（archiving 理由）

1. **spec §3.0.0「迁移与既有资产」台阶退役条款**：Pod S 的同步 HTTP 链路是架构验证台阶，方向 B 落地后退役；其安全发现（401 门纪律、设备 token 纪律）沿用进 V11，不复用实现。
2. **决策 18 + OQ-R4**：方向 B 冻结「唯一公网入口 = 网关」（wss，xiaozhi-esp32-server）；双通道违背冻结、且 8080 是未回收暴露面（ARQ-260902-shim-401-gate-bypass 教训）。shim 退役相位 = WS 包内同批（PROPOSAL v0.4 human 拍板 OQ-R4）。
3. **承接指针**：云端能力由方向 B 流式网关 `server/gateway/`（auth.py 强制鉴权 / display_schema.py 机械校验 / mcp_relay.py 中转 / fallback.py 兜底 / provider.py）承接；设备侧由本包 WS 切换（配网直写 NVS + 自签证书 pinning + WS 握手 Bearer）承接。受影响的冻结验收（Pod S 260829-smoke-e2e、TP-A 260831-cloud-link-bringup）已由 acceptance-author 标 superseded（断言语义零变更，spec §3.0.0 理由）。

## 处置清单与相位

| 件 | 状态 | 相位 |
|---|---|---|
| `server/shim.py` 墓碑注记（ARCHIVED） | 已落（本包 builder round 2） | host |
| 本归档记录件 | 已落（本件） | host |
| ECS shim 服务停 + 8080 安全组关闭 | 待执行 | live 相位 |
| ECS 收尾证据落盘 `ecs-teardown-evidence.md`（WSA-A6 齿 1） | 待落盘 | live 相位（证据目录 `server/acceptance/_artifacts/device-ws-switch/`，env `WSA_ARTIFACT_DIR` 可覆写） |

安全组操作超出 agent 权限面，届时上报 human 在 ECS 管理面执行并回填实测标记（8080 连接失败实测 + 安全组动作记录），杜绝「没报错就算过」。
