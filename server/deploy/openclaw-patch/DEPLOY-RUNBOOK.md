# KidPal C1 — OpenClaw char-stroke 补丁部署 Runbook（供 planner 在 ECS 执行）

> 部署现实（2026-09 实测）：OpenClaw = npm 全局 2026.8.1（无源码 checkout，目标是 dist
> ESM chunk）；数据集在 `/root/.openclaw/workspace/kidpal/chardata`；`/opt/kidpal/chardata`
> 不存在需软链。补丁语义 = `char_stroke_lib.py` 冻结契约的 TS 镜像（extract 最小 char 块
> → 静态字库 LRU → 替换全量 D3）。**流式 + 非流式双出口都要覆盖**：生产引擎是流式，只补
> 非流式会让真机查字功能失效（设备侧只能防御降级）。

## 0. 前置检查（版本漂移闸 —— 对不上就停）

```bash
# 确认 OpenClaw 版本与 dist 存在
openclaw --version                    # 期望 2026.8.1
ls /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-*.js
# 确认锚点 (2026.8.1 实际回复出口形状; 任一 grep 无命中 → 停止, 不要硬 apply)
grep -l "resolveAgentResponseCommentary" \
  /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-*.js
grep -c "streamedAssistantText" \
  /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-*.js
```
**任一 grep 对不上 → 停止**，反馈 planner（dist 版本/形状漂移），勿强行 diff。

## 1. 软链数据集（部署动作，一次）

```bash
# 真实数据集目录
REAL_DATA=/root/.openclaw/workspace/kidpal/chardata
# 验收/探针默认读 /opt/kidpal/chardata —— 软链对齐
mkdir -p /opt/kidpal
ln -sfn "$REAL_DATA" /opt/kidpal/chardata
ls -la /opt/kidpal/chardata/graphics.txt /opt/kidpal/chardata/dictionary.txt
# 校验点: 各 9574 行
wc -l /opt/kidpal/chardata/graphics.txt /opt/kidpal/chardata/dictionary.txt
```

## 2. 备份（回滚必要先决）

```bash
TARGET=$(grep -l "resolveAgentResponseCommentary" \
  /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-*.js)
cp -n "$TARGET" "$TARGET.c1.bak.mainline"
echo "备份到 $TARGET.c1.bak.mainline; 也保留一份完整 dist 目录 tar 以防整体回滚"
tar -czf /root/openclaw-dist-before-c1.tgz \
  /root/.npm-global/lib/node_modules/openclaw/dist
```

## 3. 应用补丁（幂等 + 漂移闸已内置）

```bash
cd server/deploy/openclaw-patch   # repo 内
# 只推理: OPENCLAW_NM_ROOT 指向 npm 全局 openclaw; CHAR_DATA_DIR 指向成熟数据集
bash apply-openclaw-patch.sh
# 期望输出: dist chunk 定位 → backed up → patched (marker) → warm char_cache.json N 字
```
`apply-openclaw-patch.sh` 会：定位含锚点的 dist chunk → 若未打则备份到 `.c1.bak` → 调
`make_openclaw_patch.py` 注入 ESM helper + 两个出口包装点 → `warm_char_cache.py` 产
`char_cache.json`（9574 字全量）。

**注意**：`make_openclaw_patch.py` 注入 非流式 `const c1Content = k1cRewriteText(content);`
和 流式 `const c1Delta = k1cStreamStep(c1Req, content);` 两个包装点。若 dist 该处行内
`content` 是 `const` 且 `c1Req`/下游 `sendJson` 变量名与此不同，按下面 3.1 手工收尾绑定。

### 3.1 手工绑定收尾（wrap 点接续）
```bash
TARGET=$(grep -l "resolveAgentResponseCommentary" \
  /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-*.js)
grep -n "c1Content\|c1Delta" "$TARGET"        # 找到注入点行号
```
- 非流式：把该 chunk 中随后的 `sendJson(res, 200, { ... content ... })` 里的 `content`
  改为 `c1Content`（或把注入行改为 `let content = k1cRewriteText(content);` 并把原
  `const content` 改 `let content` —— 任选其一，保证下游用改写值）。
- 流式：在 delta 写入点把增量文本替换为 `k1cDelta`（复制注入点所在行，把后续写 SSE
  的内容用 `k1cDelta`）。`c1Req` 取当前请求对象（有 `id` 最好；无则用注入的单槽兜底）。

> 稳妥做法：apply 后 `sed -n` 查看注入点上下文，确认 `c1Content`/`c1Delta` 被下游实际
> 引用；若没有，手工补一处赋值引用即可。后端顺手 `node -c` 语法检查。

## 4. 校验点

```bash
# 语法校验 (node --check 对 ESM chunk 有限, 至少确认无导入语法错)
node --check <TARGET> || true   # (若输出导入/语法错误, 回滚, 见 §6)

# marker 在场
grep -c "KidPal C1" "$TARGET"

# 语支校验代码级: 非流式 + 流式锚点都在
grep -l "c1Content" "$TARGET" && grep -l "c1Delta" "$TARGET"

# char_cache.json 生成且键全集
python3 - <<'EOF'
import json
d = json.load(open('/opt/kidpal/chardata/char_cache.json', encoding='utf-8'))
print(len(d), 'chars', all(set(it)>= {'char','pinyin','radical','strokeCount','strokes','medians'} for it in d.values()))
EOF

# 重启网关
systemctl restart openclaw-gateway
systemctl status openclaw-gateway --no-pager | head -5
```

## 5. live 复验（verify 阶段跑）

```bash
RUN_LIVE=1 python -m pytest server/acceptance/test_char_stroke.acceptance.py \
  --import-mode=importlib -m live -q
# 期望: A8 live (拼装块 vs makemeahanzi 逐字对照), A9 live (端到端 ≤8s, 5 秒档)
```

## 6. 回滚

```bash
# 方式 A —— 只回补丁: 用 .c1.bak 还原该 dist chunk
TARGET=$(grep -l "resolveAgentResponseCommentary" \
  /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-*.js)
cp "$TARGET.c1.bak" "$TARGET"
# 方式 B —— 整体回滚 dist
tar -xzf /root/openclaw-dist-before-c1.tgz -C /root/.npm-global/lib/node_modules/openclaw/
systemctl restart openclaw-gateway
# 回滚后: marker 应消失
grep -c "KidPal C1" $TARGET || echo "marker cleared (回滚成功)"
```

## 7. 卫生

- `apply/openclaw-patch`、`warm/chardata` 都在 repo `server/deploy/openclaw-patch/`，
  ECS 不回写 repo（只应用制品）。所有 dist 侧文件改动以 `.c1.bak` 留档。
- 生产引擎是流式：缺流式出口 = 真机查字只读最小块 → 设备侧 `kidpal_char_data_validate`
  防御降级（友好提示、不盲渲染、不崩），不会崩，但查字不可用 —— 所以 runbook 强调双出口。
---

## 8. 包 7 提醒三档 cron（260829-schedule-reminder，本包只落载体；真跑属 live 相位人工批准）

> 覆盖指针：本节 §8.2-8.4 的部署执行由 260911-ecs-deploy-convergence 包承载（acceptance
> ECS-A4 = cron 在册 + lastRunStatus=ok + kidpal-front.service env；ECS-A5 = live 五齿
> L1-L5 证据交班，证据落 delivery/changes/dev/260829-schedule-reminder/）。

> 混合分层（裁定一 eventId acaef302-9ddc-4508-b5d3-94023e050f43）：**时刻算术 + 幂等 id +
> pending.json 原子写 = 确定性脚本** `reminder_schedule_lib.py`（本目录，`warm_char_cache.py`
> 同形态先例）由 cron 的 `--command` 载荷执行；**备注里哪部分算要带的 + 给孩子的文案 =
> OpenClaw agent 回合**按 `server/deploy/openclaw-system-prompt.v3.md` 的「包 7 提醒备注解析
> 约定」产出语义中间产物，供脚本消费。脚本对缺/坏语义输入降级继续排程（机械兜底文案）。

### 8.1 前置（planner 2026-09-08 只读实测已核）

```bash
openclaw cron list | head -40      # 原生 cron 在跑：4 条 status=ok，含 class-reminder-0529 先例
python3 -c "import gateway.schedule" 2>/dev/null || echo "需要 KIDPAL_SERVER_DIR 指向 /opt/kidpal/front"
ls /opt/kidpal/front/gateway/schedule.py   # 包 6 B1 解析器（提醒只消费、不改）
ls /root/.openclaw/workspace/kidpal/schedule.md  # 真身 549B：只有「每周固定课表」、备注全空
```
交接目录当前**不存在**（planner 实测），故 `read_pending` 缺文件安静返回空是**正常态**，
首帧由 cron 写入后才有条目。路径一律 env 可配，禁止硬编 `/root/.openclaw/**`。

### 8.2 铺脚本 + 配置（幂等，repo → ECS）

```bash
cd server/deploy && bash apply-xiaozhi-overlay.sh   # 渲染当前版 system prompt（版本见 OPENCLAW_SYSTEM_PROMPT_CARRIER）+ openclaw-reminder.config.yaml
# 脚本与配置的目标目录（与 pending.json 同机）：
export KIDPAL_OPENCLAW_SCRIPT_DIR=/opt/kidpal/openclaw-patch
export KIDPAL_ALERTS_DIR=/opt/kidpal/alerts         # cron 写、front 读的交接目录（同机）
mkdir -p "$KIDPAL_ALERTS_DIR"
```

### 8.3 注册 cron（定期重算 + 录入即时触发）

```bash
# 定期：每 15 分钟重算三档（已过去的档位由脚本自动跳过，幂等 id 复用不重生成）
openclaw cron add \
  --name kidpal-reminder-plan \
  --every 15m \
  --command "KIDPAL_SERVER_DIR=/opt/kidpal/front \
KIDPAL_SCHEDULE_MD=/root/.openclaw/workspace/kidpal/schedule.md \
KIDPAL_ALERTS_DIR=/opt/kidpal/alerts \
KIDPAL_SEMANTIC_JSON=/opt/kidpal/alerts/semantic.json \
python3 /opt/kidpal/openclaw-patch/reminder_schedule_lib.py"
# 即时：家长 QQ 录入 → OpenClaw 自改写 schedule.md 后，同一 --command 载荷再跑一次
#   （cron 支持 --command 载荷已在 class-reminder-0529 先例实测；语义产物由 agent 回合写
#    semantic.json，脚本按 entry_key 挂载，缺文件=机械兜底不丢提醒）
openclaw cron list   # 期望新增一条，state.lastRunStatus=ok
```

### 8.3b 注册语义回合 cron（agent 型，260925-reminder-semantic-round-not-live 补全）

> 背景：ARQ-260925-reminder-semantic-round-not-live——`kidpal-reminder-plan` 只跑脚本半边，
> 「agent 回合产 semantic.json」从未上线（semantic.json 长期缺位，提醒一直走机械兜底）。
> 本任务补 agent 生产侧：agent 型 cron 每 15 分钟唤起 main agent 一次，按
> `workspace-main/kidpal-reminder-semantic` skill 产语义产物；脚本侧零改动（消费链路
> `load_semantic`/`bring_items`/title-speech 覆盖本就完整）。

```bash
openclaw cron add \
  --name kidpal-reminder-semantic \
  --every 15m \
  --agent main \
  --no-deliver \
  --message "提醒语义回合：读 kidpal/schedule.md，按 kidpal-reminder-semantic skill 的
流程全量重写 /opt/kidpal/alerts/semantic.json。收到本消息即执行，无需确认。"
openclaw cron list   # 期望两条 kidpal-reminder-*：plan（command 型 isolated）+ semantic
#                    # （agent 型 main）；各跑一个周期后核 /opt/kidpal/alerts/semantic.json
#                    # 出现且 entry_key 与 schedule.md 行逐字对齐（live L5 证据补课）。
```

设计要点（为什么是 main agent + skill 承载）：板侧 kidpal agent 只在**设备语音请求**时被
引擎注入 system prompt（v7 提醒节的读者假设因此只覆盖语音回合），cron 唤起的 agent 回合
不走引擎注入；main agent 拥有 kidpal/ 写侧与 skill 机制，指示的家 = workspace skill
（镜像入仓 `server/deploy/openclaw-skills/workspace-main/`）。

### 8.4 前端投递侧（同机 uvicorn，无需额外进程）

`server/gateway/front/front_app.py` 在首个已鉴权设备连接建立时起**每分钟轮询**（对齐整
分钟），读 `KIDPAL_PENDING_JSON`（默认 `$KIDPAL_ALERTS_DIR/pending.json`）→ 机械校验 →
到点经 mcp `kidpal/alert` 推设备腿 + 向引擎腿注入 `kidpal_speak`（裁定二：出音走引擎既有
edge-tts + opus 下行链，front 不自建合成、不编码、不注入二进制帧）→ 等设备**渲染回执**，
60 秒未收重推、单档 3 次上限、超出窗口静默作废不补发、不做重连即重推。

```bash
systemctl show -p Environment kidpal-front.service   # 需含 KIDPAL_ALERTS_DIR / KIDPAL_SERVER_DIR
journalctl -u kidpal-front.service -o cat --since "-2 min" | grep -E "alert_poll|alert_deliver|alert_ack|alert_void"
```

### 8.5 live 验收（殿后人工批准相位，本 loop 不执行）

```bash
cd server && RUN_LIVE=1 python -m pytest -q --import-mode=importlib \
  acceptance/test_schedule_reminder.acceptance.py -k live
```
五事实（L1 cron 真在册且真跑过 / L2 pending.json 真形状 / L3 真推到设备 / L4 真渲染回执
出队 / L5 真大模型语义产物）落盘 `delivery/changes/dev/260829-schedule-reminder/`；证据内不得出现
`Invalid id` / `Unknown message type` / 推送失败 / 排程失败 字样（probe ≠ exercise）。
真实提醒音效与弹窗并发时序、屏上网点观感殿后 9b。

### 8.6 回滚

```bash
openclaw cron remove --name kidpal-reminder-plan
openclaw cron remove --name kidpal-reminder-semantic   # 8.3b 语义回合（如已注册）
rm -f /opt/kidpal/alerts/pending.json          # 投递侧下一分钟自然读空（缺文件安静）
rm -f /opt/kidpal/alerts/semantic.json         # 语义产物一并清（缺文件 = 脚本机械兜底）
# prompt 回退：apply-xiaozhi-overlay.sh 里 PROMPT_SRC 改指目标历史版一行即可（版本清单见
# OPENCLAW_SYSTEM_PROMPT_CARRIER；历史版本文件零回改、共存可 diff）
```
