---
workstream: kidpal-mvp 开发包拆分与并行开发
slug: kidpal-mvp
status: done
opened: 2026-08-29
closed: 2026-09-13
summary: 把 spec §4 全部 story 拆成排序带依赖的 TaskPackage，模拟优先硬件殿后，先烧板证明板↔shim↔OpenClaw 可行再做主体开发
owner: portfolio-planner
area: systems
changes:
  - 260831-cloud-link-bringup
  - 260831-simulator-lvgl-align
  - 260831-ui-art-assets
---

# kidpal-mvp · task_plan

## 目标

把 spec §4 的全部 story（F1 课表 / F2 故事 / F3 查字 + §4.0 共享底座）拆成一组排好序、带依赖、模拟优先、硬件殿后的可并行 TaskPackage；先用最小量烧一次板子证明「板↔shim↔OpenClaw」架构可行，再在电脑上完成主体开发，最后殿后烧板做声学与物理验收。

## 设计权威

拆包设计权威 = 本目录 `PROPOSAL.md`（FROZEN v0.2，frozen pin `sha1:256fa5dcf67b599b`），评审记录 `review.md`，决策覆盖 `decision-coverage.json`。

## 单一事实源

- 功能/story：[docs/spec.md](../../docs/spec.md)
- 契约（已冻结，只读）：[docs/contracts/interface-contract.md](../../docs/contracts/interface-contract.md)（E1/B1/D3/提醒三契约）

## 非目标

不改 `docs/contracts/**`、不改 vendor 面参考代码、不做 spec §9 二期间（WebSocket 流式 / WakeNet / TLS）。

## 决策

- 切分轴 = 能力竖切（每包一个可独立演示/验证的完整能力，不按技术层横切）。
- 测试策略 = 模拟优先、硬件殿后。
- 加 tracer-bullet 冒烟批先证架构可行。
- WIP limit = 3。
