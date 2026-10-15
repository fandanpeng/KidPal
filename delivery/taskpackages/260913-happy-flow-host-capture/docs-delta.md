# Docs Delta · 260913-happy-flow-host-capture

> 补制收口制品（2026-10-09）。本变更 = human 一次性授权的主线程直做操作（2026-09-13 落，
> commit 96378a7），**跳过 dev-loop 流程**（Hard Rules 例外条款，README「性质」节如实记录）。
> 落地当日即 commit 并入 main，但 closeout 目录未落本件与 review-packet.md，致 INDEX dev
> 段状态悬 open。本件如实补记，不改写任何既有记录。

## 1. 结论

- **性质**：host 渲染缝整帧截图存档轮——把 Happy Flow 涉及的 host runner 已有渲染面逐页
  真渲染落盘（10 枚 PNG，`capture/01–10`），作为当时渲染实现面的一次验证结果记录。
  非 RP-A5 真机对照轮替代品，真机观感终审归 human 持板轮。
- **代码面（固件测试基建，非业务实现面）**：
  - `firmware/main/test/host_app/main/host_infra_seam.c`：additive 落盘函数 `host_infra_dump_bmp()`；
  - `firmware/main/test/happy_flow_capture.test.c`：新增（普通 `.test.c` 非冻结件，env 门控 `KIDPAL_CAPTURE_DIR`）；
  - `firmware/main/test/host_app/main/CMakeLists.txt`：加 `-Wno-error=comment`；
  - `firmware/main/test/HOST_TEST_CONVENTIONS.md`：固化「Host 整帧截图办法（HOST CAPTURE）」配方。
- **契约面：零触碰**。**需求面 / 架构面 / ADR 面：零改**——截图轮不产生任何 spec 语义变更；
  字体部分子集等已知未收口项记录于 README 与 HOST_TEST_CONVENTIONS.md（方法文档，非 spec）。

## 2. 文档面变更清单

| 面 | 文件 | 本变更动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/**` | 零触碰 | 只读 |
| spec·需求 | `docs/requirements.md` | 零改 | 只读 |
| spec·架构 | `docs/architecture.md` | 零改 | 只读 |
| ADR | `docs/ADR.md` | 零改 | 只读 |
| 方法文档 | `firmware/main/test/HOST_TEST_CONVENTIONS.md` | 新增截图配方节（commit 96378a7 内，随批落） | 方法沉淀 |
| 存档 | `delivery/changes/dev/260913-happy-flow-host-capture/`（README + runner-run.md/.log + capture 01–10.png + 本件 + review-packet.md） | 产出 | 记录面 |

## 3. 收口时点对账（如实）

- 无 TaskPackage、无 supervisor 闸、无 pod-events——直做操作按 Hard Rules 例外条款执行，
  「跳过流程」已在 README 首节诚实记录；本件与 review-packet.md 为 INDEX 收口所需的
  closeout 制品补落，不是事后补造的 loop 制品。
- 无 workstream、milestones 不动（无阶段跳变）；无 pod-retro（非 Pod 运行，closeout-check
  对应项为 advisory，直做操作不适用）。
