# ecs-bootstrap — ECS deploy clone 一次性引导（human 手册）

261013-gov-review-deploy-single-source-loop 落地件（PROPOSAL T6）。本页是 **human
一次性动作**清单：把 ECS 从「手工拷贝部署」切到「GitHub 单源拉取 + manifest 铺放」。
全部步骤只在 bootstrap 时执行一次，此后部署只跑 `deploy.sh`（见 README「部署入口」节）。

前置：本批已合并 main 且 **human 已 push 到 GitHub**（remote push 铁律不变——push
即部署闸；OQ-3(a) 裁定整仓单 origin）。首推时已做 secret 扫描（ECS 实钥零命中、
长 token 唯一命中为验收测试常量，2026-10-13 实测）。

## 1. deploy key（只读）

在 ECS 上生成专用只读 key（不使用任何已有账号 key）：

```bash
ssh-keygen -t ed25519 -f /root/.ssh/kidpal_deploy_key -N "" -C "kidpal-ecs-deploy"
cat /root/.ssh/kidpal_deploy_key.pub
```

把公钥贴到 GitHub 仓 `Settings → Deploy keys → Add deploy key`，**只勾 read-only**
（不勾 write——ECS 侧永远不该能推）。

写 `/root/.ssh/config` 追加（已有文件则只追加 Host 段）：

```
Host github.com
  IdentityFile /root/.ssh/kidpal_deploy_key
  IdentitiesOnly yes
```

## 2. deploy clone

```bash
git clone --depth 1 https://github.com/fandanpeng/KidPal.git /opt/kidpal/deploy-clone
```

clone 后确认 `server/deploy/manifest.yaml` 在（部署清单 = 一切的地基）。

## 3. 首铺（--first-run 对账）

首铺按定义必红（2026-10-13 漂移基线 = PROPOSAL §2 实测 6 处漂移 + 2 处配置消费位
缺文件），所以走 `--first-run` 跳过拦截、以仓内为准对账 live：

```bash
cd /opt/kidpal/deploy-clone
bash server/deploy/deploy.sh --first-run
```

预期：front 12 件中 2 件更新（front_app.py 的 TEMP-DIAG 清零〔OQ-4(a)，不回流仓〕、
provider.py 的 OPENCLAW_MODEL 归一）、reminder lib 换新（含 `drop_refresh_marker`，
触发③ marker 链首次 live）、两个配置消费位补文件、OpenClaw 镜像面补齐。引擎面有
变更会自动跑 overlay 渲染 + `docker restart`（设备会断连重连，挑孩子不用设备的时段）。

**secrets 免疫验证**：首铺后 `git -C /opt/kidpal/deploy-clone status` 应干净——
manifest 不含 secrets、secrets.env 只在 `/opt/kidpal/xiaozhi-esp32-server/deploy/`。

## 4. 安装每日漂移 timer

```bash
cp /opt/kidpal/deploy-clone/server/deploy/kidpal-deploy-drift.{service,timer} /etc/systemd/system/
systemctl daemon-reload
systemctl enable --now kidpal-deploy-drift.timer
systemctl list-timers | grep kidpal-deploy-drift   # 应见明日的下一跳
```

（此后 timer 单元文件由 manifest 管，deploy.sh 铺放后自动 daemon-reload。）

## 5. 首轮全绿确认

```bash
bash /opt/kidpal/deploy-clone/server/deploy/verify-deploy.sh   # 期望 exit 0
cat /opt/kidpal/alerts/deploy-drift.json                        # drift_count: 0
openclaw cron show kidpal-reminder-plan | grep diagnostic       # 三档值不再依赖 DEFAULT 回退
```

PROPOSAL §5 验证集的 post-landing 条目逐条执行留痕（landing-summary 收口用）。

## 日常此后

- 部署 = `cd /opt/kidpal/deploy-clone && bash server/deploy/deploy.sh --ref origin/main`
  （human push 完成后执行；`--dry-run` 可先看变更面）；
- live 紧急热修仍允许，但下次部署被 verify 拦截直至回流仓；
- 每日 04:17 timer 自动核验，漂移写 `/opt/kidpal/alerts/deploy-drift.json`；
- 回滚：`/opt/kidpal/.deployed-commit` 记录最近一次部署的 commit 与备份目录
  （`/opt/kidpal/deploy-backups/<ts>/` 保存被覆盖的旧文件），拷回 + 按重启面重启。
