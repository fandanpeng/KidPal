// changeId 前缀正则单源（ARQ-260904-gate-regex-uppercase-changeid，260906-gov-review-arq-governance-batch T1/T1b，FROZEN v1.0）。
// 原 8 处本地拷贝（7 脚本 + 1 钩子：closeout-check / devloop-artifact-check / dispatch-stamp / journal-append /
// loop-health-report〔原 _LHR 别名〕/ loop-metrics / taskpackage-required-check / hooks/supervisor-gate-check）收敛为具名导入（ARQ-097 模式）。
// 行为零变更：正则本体与 2026-09-04 human 批准的大写热修（[a-zA-Z0-9-] 接受混合大小写 changeId）一致，仅消灭拷贝漂移面。
// 边界声明（R1-F7）：stripPrefix / validDatePrefix 仍留各脚本本地拷贝，是否二轮收敛留后续治理批。

export const CHANGE_ID_PREFIX_RE = /^(\d{6})-[a-zA-Z0-9-]+$/;

// T1b 自检：把「门须接受混合大小写 changeId」固化为可机判断言（正反例钉死，防单源被误改回小写）。
export function runChangeIdPrefixSelfCheck() {
  const cases = [
    { name: 'mixed-case-positive', input: '260903-gateway-directionB', expect: true },   // 大写误判实例（ARQ 病灶包）
    { name: 'lowercase-positive', input: '260829-smoke-e2e', expect: true },
    { name: 'underscore-negative', input: '260903-Gateway_DirectionB', expect: false },
    { name: 'no-date-prefix-negative', input: 'gateway-directionB', expect: false },
  ];
  let pass = 0;
  const fail = [];
  for (const c of cases) {
    const got = CHANGE_ID_PREFIX_RE.test(c.input);
    if (got === c.expect) pass += 1;
    else fail.push({ ...c, got });
  }
  const result = { selfCheck: 'changeid-prefix', pass, fail: fail.length, total: cases.length, ok: fail.length === 0 };
  if (fail.length > 0) result.failures = fail;
  return result;
}

if (process.argv.includes('--self-check')) {
  process.stdout.write(`${JSON.stringify(runChangeIdPrefixSelfCheck())}\n`);
}
