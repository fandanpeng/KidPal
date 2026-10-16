# review-r2-architect · 260915-gov-review-worktree-pool-reuse

> 判官 = architect（Track B）。评审对象 = PROPOSAL.md DRAFT v0.5（R1 修订回填）。评审日期 2026-09-15。
> R2 纪律：聚焦 R1 findings（review-r1-architect.md F1–F6 / N1–N2）的修订到位性 + v0.5 是否引入新矛盾，不做无的放矢的全量重核（R1 已全量亲核的 17 项机械断言未随修订变动，不重跑）。

## Verdict

**GREEN（可冻结）** —— R1 三条 BLOCKING 全部修复到位且语义正确，三条 SHOULD 全部落定，两条 NIT 处置得当；v0.5 修订未引入新矛盾。附两条 NIT 级备注（不阻冻结，landing 文本可顺手吸收）。

## 一、R1 findings 修订到位性（逐项）

### F1（复位目标）——到位，语义正确

T1 ② 与 T2 ② 均改为「`git reset --hard <同步后的专用 base 分支>`，非 main」，并写明原因（main..base 含未 launch 包拆包制品、字面复位 main 会抹掉致 intake fail-closed）。语义核验：复位目标 = 含下一包拆包产物的基准 ref，与 F-05（portfolio-planner.md:49 专用 base 分支）一致；「同步后的」限定词正确 encode 了前置（base 分支须先吸收 main 的合并增量，否则树内缺上批已并变更）；W1 活体试验（V8，N1b 在 K1a 树上续跑）走的是 merged→续跑路径、不涉复位，与新措辞自洽。两处（T1/T2）同文无漂移。

### F2（`--base-ref main` 三处 + V2 加严）——到位，覆盖完备

问题 3 证据补 `:38`（标注「launch 机械门实际参数，行为承重」）✅；T1 ③ 扩为三处（`:14` / `:56` 尾参 / `:38`）✅；V2 加严为 `grep -nE '当前 main|--base-ref main|-b dev/<changeId> main'` → 0 命中。覆盖核验（对照 R1 亲读的 SKILL.md 全文）：现存 main-as-ref 残留共四处形态——`:14`（base = 当前 main）、`:38`（--base-ref main）、`:56`（-b dev/<changeId> main）、`:60`（base = 当前 main HEAD）——三个 alternation 恰好全覆盖（`:60` 由「当前 main」兜住）；改后新形态（`--base-ref <专用 base ref>`、`-b dev/pool-1 <base ref>`）均不误中。真空绿反照通过：该验证能真证「F-05 对齐无残留」。

### F3（严格模式声明载体）——到位，守住了 260909 裁定

T1 ⑤ 启动卡改四要素，第四要素 = 固件严格模式声明行（`VERIFY_FW_BUILD_REQUIRED=1` 默认开、无关闭留痕即开），并引 firmware-build-check.mjs:108 读进程环境变量、未设即降级的实测语义。语义核验：与收缩前的载体机制等价——原本「默认开」的成文载体就是 launch prompt 环境段的声明行（SKILL.md:100 ①），声明随卡走、Pod 见卡即知须在跑 firmware-build 闸前设环境变量，「显式关闭须留痕理由」语义保留。260909 T3/T5 OQ1=(b) 的两个要件（默认开 + 显式关闭留痕）均在，无静默降级通道。riskTier 证成句「行为面收敛」随之恢复成立。

### F4（dev-loop-operating-system.md 同族措辞）——到位，四处同步齐

T1 新增 ⑥（`:50`「每个 ready TaskPackage: git worktree add」→「复用 ∨ 按需新建」口径）；目标文件、landing-manifest（第 14 行）、sweep 表（新行，分类 G2）、review profile（触面枚举）四处全部同步补该文件，无遗漏。

### F5（V3 词形）——到位

验证 3 关键词改「空闲判读/常驻分支/交接卫生/按需扩容」，与 T1 ②/T2 ② 协议正文四条（空闲判读 / 一树一常驻分支 / 交接卫生 / 按需扩容）词形一致，「切新分支」v0.2 遗留形态已消。

### F6（T5 证据落耐久制品）——到位

新建 `evidence-wsl-pitfalls.md`（本 change 目录，repo-tracked）：两坑命令 + 失败输出 verbatim + 成功形态 verbatim + ELF summary，T5 改引该文件。交叉核验：证据文件所记 ELF summary「129 Tests 4 Failures 0 Ignored」与 K1a launch prompt 环境段实测记载（129 Tests / 4 Failures / 0 Ignored）一致；MSYS 改写形态（`C:/Program Files/Git/mnt/c/...`）与已知 MSYS 行为及 prompt 记载相符。硬项 1a 留痕形态达标。

### N1 / N2——处置得当

N1：Status 行 v0.5 已记「human 立项 ack 2026-09-15」+ R1 修订摘要，FROZEN 凭据（GREEN + OQ 结果）届时由主线程补——符合模板纪律（现在预填无凭据即虚记），认同不预填。N2：T3 ④ 改「启动卡基线锚行」，与 T1 ⑤ 启动卡形态统一。

## 二、v0.5 新漂移检查

- **manifest ≡ 各 T 锚清单并集**：T1 = launch-prep SKILL.md + dev-loop-operating-system.md；T2 = pod-coordination.md；T3 = SKILL.md（同 T1）；T4 = integrator.md；T5 = kidpal-idf-env SKILL.md；T6 = architecture.md。并集 6 文件 = landing-manifest 围栏块 6 行，逐一吻合 ✅（R1 时为 5，F4 补入第 6，同步无漏）。
- **结构字段**：`### T<n> ·` 标题形态六条合规；packet-id 行在场 ✅；manifest 裸路径、一行一个 ✅。
- **修订面与未动面无新矛盾**：F1 复位措辞改动局限于 T1 ②/T2 ②，未波及 merged→续跑与白名单条款；F2 证据补充未改动问题 3 根因结论；F3 第四要素与「launch 侧动作收敛为三件」并存不冲突（声明行是卡的内容、不是第四个 launch 动作）；T3 ④ 与 T1 ⑤ 措辞已统一。§0/§4/§6 未动、无漂移。
- **review profile / sweep**：G2 触面枚举五文件 + spec 面连带一项，与 manifest 一一对应；sweep 表新增行分类与说明相符，UNCLASSIFIED 仍 = 0。

## 三、NIT 级备注（不阻 GREEN，landing 文本可顺手吸收）

- **N3**：T1 ② 复位目标「**同步后的**专用 base 分支」中，「同步」（main→base 合并增量）是无主谓词——launch 侧动作清单（三件）未列该动作。建议 landing 时在 skill 换包协议里把同步显式为复位前置一步（或一句「复位前先把 main 合进 base 分支」），防操作者拿到一个未同步的 base 直接 reset。失败方向已兜底（未同步 → 树内缺上批变更 → T3 差异判停非零 → 重跑，fail-safe），故仅 NIT。
- **N4**：§5 验证集无 dev-loop-operating-system.md `:50` 对应条目（六 T 中唯该面无机械验证）。landing fidelity 核验（逐文件人读 diff vs manifest）可兜底，建议补一条 `grep -n '每个 ready TaskPackage: git worktree add' .claude/references/dev-loop-operating-system.md` → 0 命中，成本一行。

## 四、结论

R1 全部 findings 修复到位、无新矛盾、结构字段齐备。**GREEN** —— 主线程可凭「human 立项 ack（2026-09-15）+ 本 R2 GREEN + OQ 已全部收口（§4 四条均 human 已裁）」标 FROZEN，随后走 packet-bound landing 通道。N3/N4 由主线程自行裁量（不构成打回）。
