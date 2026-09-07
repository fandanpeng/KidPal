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