# pod-retro · 261104-k3-qq-points-channel

> Pod 收尾复盘（planner 第一人称热记忆，closeout advisory 指名）。Pod 形态 = VS Code 扩展会话内联
> planner（human 指示在本 worktree 非 headless 启动）；base ref `45518ac2`；pod-events 贯穿
> （launch 五件 → intake → Step 1g 预评两轮 → 冻结验收 → builder 两轮 → verify scoped 两轮 →
> integrator 收口）。只记根因、摩擦、纠正与未解项，不复述 tokens/gates/rounds。

## 顺利面（保持）

- **预评前置喂事实**：第二轮 architect 派发把「points_observe 观察缝已接线、两 workspace 分界、
  points.py 执行器面、契约已 landed」直接写进 brief，要求只做校验性抽查 + 12 次工具调用硬帽，
  10 分钟收口，质量足够（编号到 front_app.py 行号）。
- **冻结验收的强度申报**：acceptance-author 把「弱于契约处」写成 GAP-K3Q-1..5、行为等价假设
  EQ-1..4、逐条 coverage 映射，integrator 的未覆盖面申报可以直接对齐，没有二次解释成本。
- **观察缝复用而非新造**：K2 真机轮已证 `points_observe` 观察 CLI 写入并推 restore 帧，K3 直接
  复用，front_app.py 最终零 diff——避免了在 2400 行文件里造第二条推送通路。
- **红基线与回归双跑**：planner 对 acceptance-author 与 builder 的关键判据都独立复跑，未凭报告采信。

## 踩坑与教训（供 loop-retro）

1. **核上一包状态只读了 progress 的冻结时点段落**：在 K3 启动卡与 progress 里误记「K2 真机轮
   待跑」，human 当场纠正（原话「K2 的真机轮跑过了呀」）。K2 的真机轮 + 复验轮其实已在
   2026-11-06 / 11-14 完成并 PASS。**共性**：核上一包交付状态要以 `device-round-evidence.md`
   与 `pod-events.jsonl` 尾部为准，progress 的某段只代表写它时的时点。
2. **只读预评会无界扩散**：architect 首轮给了 11 项必读清单 + 一条追加消息，40 分钟未收口。
   **共性**：派只读 advisory 时必须同时给「工具调用上限 + 明确产出格式 + 禁止继续扩读 +
   把已知事实前置喂入」，否则「全面」会吃掉时钟。
3. **mutationEvidence.perAcceptance 连续两包留到 verify 才补**（K2 同型，K3 复发）：
   Finalize 时只写了 note，verify.mjs 的 mutation-check 直接红。**共性**：Step 8 Finalize 就应
   要求 builder 按 `acceptance[]` 逐条给变异四字段，planner 程序化回填（禁止手抄全文串）。
4. **子 agent 对 `delivery/taskpackages/**` 的 Bash 写会被 universal.bashDenyWrite 拦**：
   acceptance-author 超预算的执行里有一次 `cat >` 被拦，改 Write 才落盘。**共性**：派发时就
   写明「包目录只准 Write/Edit，不准 Bash 重定向」，省掉一次失败往返。
5. **delete 的 command_id 重放缺口是 builder 主动申报的**：SKILL.md 已向家长承诺「同一次重试
   返首次结果」，但 delete 原本没有守卫，同 ID 重放 `--index` 会误删新分录。**共性**：凡是
   SKILL.md 对外承诺的幂等语义，必须在 CLI 侧有对应测试；文档承诺与实现不一致是静默事故源。

## 未解项 / 交接（交主线程与 human）

- **真实 QQ + 设备轮未跑**（K3Q-E4 / K3Q-L1）：加/兑/查/删往返、歧义与重复输入、`ask_user`
  不卡死、设备不抢前台/积分页原地刷新、NFR-PERF-4 实测时延，均待 human 在场执行并落
  `device-round-evidence.md`。本包不以离线绿替代。
- **ECS 部署轮未跑**：铺 `kidpal-points-write` skill + 更新后的 `points.py`，验 manifest 表驱动
  与 verify-deploy md5。归主线程。
- **合并定序需 human 裁**：先部署+真机轮再合并，还是离线合并后补 live 证据——两条都要有人拍板。
- **`command_id` 平台消息 id 可得性未核**：真实 QQ 轮第一件事就是确认平台是否给消息 id；
  拿不到则只能用 `qq:<ts>:<hash>` 降级形态，重试可辨识性下降。
- **loop-retro 阈值已触**（closeout advisory 26 ≥ 3）：发起责任 = 主线程。
- K2 遗留的 F8 末屏回绕、兑换正向归 K4、tech-debt 两笔与本包无涉，不吸收。