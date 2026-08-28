# retro — 260829-gov-review-migration-tier23

> 档 2/3 迁移批收尾 retro（落地后趁热写，先于合并）。元级迁移批，豁免七步闭环，Status=LANDED-UNREVIEWED。

## 做了什么

把档 2（dev-loop 全链 + 测试闸）与档 3（portfolio 编排）从源仓迁入本仓，栈绑定的测试闸与
风险信号全部按本仓双实现面（firmware/ ESP-IDF C/C++ + server/ FastAPI pytest）重写或适配。

## 顺手的

- **栈绑定审计先行**：先逐脚本 grep `pnpm|vitest|prisma|tsc|apps/|packages/`，把资产分成「直搬 /
  重写 / 声明缺口」三堆再动手。栈中立的治理机（taskpackage-required / acceptance-coverage /
  mutation-check / docs-sync / security）占了相当大比例，直接拷 + 自检即绿，省了大量重写。
- **mutation-check 意外栈中立**：原以为绑栈，实则只读 acceptance-id-shared.mjs（TS 无关），
  直拷 + 自检 8/8 绿。教训落迁移指南：别假设测试闸都绑栈，先 grep。
- **互斥 Python 替换规则集**：27 个文档一次过、grep 清零。沿袭档 1 的「禁止裸 sed 连环 -e」教训。

## 踩的坑（回收进迁移指南 §6）

1. **自检电池是栈耦合重灾区，比脚本本体深**：supervisor-verdict-gate 的 69 例自检电池写死了
   apps/s1、prisma.lead.deleteMany、DROP TABLE。脚本逻辑改完不算完——电池不改则全红，改电池
   必须逐例想清楚「这一例在原栈测什么轴、在新栈对应什么」。ESP32 的 real-DB 信号（prisma.findMany）
   在固件面的对偶是「硬件副作用 + FreeRTOS 内核对象 + Unity 断言」。
2. **源仓「核心面」≠ 新仓「核心面」，core 闭集要重划**：源仓 core = `packages/core`（共享内核，
   少有人碰）；ESP32 若把 `firmware/main` 当 core 面，则它恰是开发热循环，每个自检例都落进去
   触发 high，69 例里 14 例假红。修法 = core 闭集收窄到真·承重窄面（契约/分区表/sdkconfig/boards），
   把热循环 `firmware/main` 放出 core。这是「宁滥勿漏」与「防过严」的张力在固件仓的具体形态。
3. **contract-lint 无法照搬**：TS schema lint 的「类型级一致性」在 markdown 契约面上不存在等价物。
   诚实的做法是降级为「结构完整性 lint」（节标题/围栏闭合/断链）+ 把更深的语义对账显式记为
   人工缺口（ARQ 追踪），而不是写一个假装能做语义校验的假门。
4. **实机验收的自动化天花板**：固件 on-target 验收（Unity on 实机/qemu）无法像 vitest 那样在
   合并门自动真跑。闸只能担保「有验收文件 ⇒ 有验收入口，不会静默跳过」，「实机已过」必须靠
   acceptance 相位人工/台架触发 + 证据落 verifier-evidence。这条对一切非纯软件项目成立。

## 还剩的缺口

见 PROPOSAL §五（实机验收人工触发 / spec↔contract 机械对账缺 / mypy 未启用 / firmware-build 无工具链 fail-open）。
全部诚实记账，非假绿。
