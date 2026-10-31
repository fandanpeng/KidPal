// KidPal 261031-openclaw-cron-trigger-gating — cron trigger 薄壳（kidpal-schedule-health）
//
// OpenClaw code-mode trigger 脚本：只做「调仓内 python 判定器 → 透传判决」，判定逻辑
// 全在 schedule_health_gate.py（纯只读、host 可单测）。本文件内容内联进 job
// trigger.script；verify-deploy.sh 会把它与盘上文件逐字比对（行尾归一）。
//
// exec 返回字段实形（aggregated vs text）未经 spike 预验证，按容错写法取
// `res?.aggregated ?? res?.text`；L2 live 观测到实形后如不符按实测修正。
const res = await exec({
  command: "/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/schedule_health_gate.py",
});
const raw = String(res?.aggregated ?? res?.text ?? "");
const out = JSON.parse(raw.trim());
json({
  fire: out.fire === true,
  ...(typeof out.message === "string" && out.message ? { message: out.message } : {}),
  ...(out.state && typeof out.state === "object" ? { state: out.state } : {}),
});
