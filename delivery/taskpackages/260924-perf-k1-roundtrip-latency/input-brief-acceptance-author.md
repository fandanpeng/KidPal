# input-brief · acceptance-author · 260924-perf-k1-roundtrip-latency

> 消费者:acceptance-author(冻齿)。语义权威 = 本包 `plan.md`(FROZEN v1.0)验收口径节 + `taskpackage.json` acceptance[]/exitCriteria[];证据背景 = `evidence/latency-baseline-260924.md`(含 §6 勘误限定)+ `architect-review-260924.md`(Q1/Q5/§3 齿形态裁定)。

## 任务

为 PERF-A1~A4 冻结可执行验收测试,落 `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`(命名沿 K3 先例),RED 基线实跑落账。

## 齿形态裁定(评审 §3,硬性)

1. **CI 齿只断言机械面**(timing 数字不入 CI——天然 flaky):
   - 模型声明 artifact 在位 + 字段正确(model id、allowlist 条目、与 apply 脚本消费面一致);
   - apply 脚本幂等语义 + **drift-guard**(现状锚点核对不中 → exit 3 拒绝,照抄 `server/deploy/openclaw-patch/apply-openclaw-patch.sh` 的「STOP on mismatch」形态)+ 备份/一步回滚脚本存在且 dry-run 可跑;
   - 探针脚本 **v6 prompt 注入 + hash 断言**(hash 源 = 仓内 `server/deploy/openclaw-system-prompt.v6.md` 逐字,渲染面==被测面,ADR-001 精神;评审 D1 硬性修正);
   - must-stay-green:引擎 `.config.yaml` LLM 段声明面(`server/deploy/provider.config.yaml` 相关键)零改动、v6 文件 hash 零回改。
2. **live 齿**(@pytest.mark.live,不进 `-m "not live"` CI 面):探针批执行器(标准 3 题 ×N≥3、中位数、主机快照、session 计数记录)+ 门判定器(读证据文件数字对两层门:保底 6/9/14s、目标 4/6/10s;A3 公式对齐注记)。live 齿判定逻辑本身可 offline 单测(喂 fixture 证据文件 → 断言判定结果),**判定器逻辑要有齿**,时延数字本身不进 CI。
3. **mutation 打在配置接线上**(不是超时判定):删声明文件 / 改错 model id / 删 allow 条目 → 对应齿红;RED→GREEN 声明 + counterEvidence 回填 taskpackage 目录。

## 红基线

- 命令:`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/`(仓根,绝对路径调亦合法)。
- 上游已知红:按 launch-prep-record.md §基线锚 带 SHA 行引用或差异重跑(锚五要素在卡)。
- 本包新增齿 = 设计态红(实现工件未落盘前),实跑后把「N failed / M passed(既有齿零新增红)」落账 `taskpackage.json.redBaselineResult`(PENDING 占位替换)。

## 铁律

- 验收测试归你,builder/fixer 改不动;不改 `docs/contracts/**`;不改 plan.md/taskpackage.json 的 acceptance 语义(发现矛盾 → fail-loud 上报 planner,不自裁);凭证不进测试文件(live 齿从 ECS 侧配置/shell 变量取,不落仓)。
