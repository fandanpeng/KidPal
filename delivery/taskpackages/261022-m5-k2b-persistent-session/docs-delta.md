# docs-delta · 261022-m5-k2b-persistent-session

- **docsSyncRequired = false**（taskpackage.json `docsSyncReason`）：架构 §8.2 会话定案取代注记 / §5.4 双历史两行 / 决策 21 注记、契约 §四:283 定案改写 + 变更记录条已由 261022 arch 批 FROZEN v1.1 landing（commit `0411e957`）同批原子落；本包只落实现面。**本包 `docs/` 全域零 diff**（含 `docs/contracts/**` 只读零触碰、spec 两文件零 diff）；本文件只登记零 diff 结论之外的声明性注记，不改任何 docs 文件。

## 声明性注记（zero-diff 结论之外如实登记；不改任何 docs 文件）

### 注记 1 · 判法目标数 281 vs 实数 292（诚实申报口径）

acceptance-author 冻结相位红基线回填的「转绿目标 281 passed」是**冻结时点值**（既有 278 绿 + 冻结件 3 条 must-stay-green 当时即绿）；冻结件 14 断言中 11 条为设计态红，builder 转绿后全量实数 = **292 passed / 0 failed / 4 deselected / 33 subtests**（281 + 11 红转绿，判据全部达成）。两个数都对，指向不同时点——final 审查以 292 为准。

### 注记 2 · unit 层口径（诚实申报）

requiredLayers 含 unit；本包 unit 层以既有套件零新增红承载（K1/P5 同款先例）：unit 面无新增测试文件，`_kidpal_turn_messages` 切片 / `_kidpal_provider_supports_user` 探针的行为级证据由冻结件 ET 断言（AST 抽取真实源切片 + 受限命名空间 exec，connection.py/openai.py 不可离线 import）承载；builder 另做四场景离线仿真旁证（真实 Dialogue + 抽取切片函数，未落文件、不计判据）。builder 申报 unit 115 passed 为既有 server/tests 全档；final bearing r2 的 unit 闸按 scope-file 求交实测 89 passed / 4 subtests（与 acceptance 闸同 7 件 pytest 文件——unit 闸口径 = 清单内可收集测试件，交集语义），以 final fact（`verifier-evidence/rounds/r2.json`）为准。

### 注记 3 · verify-deploy.sh 零改动（预期内，在册申报）

`server/deploy/verify-deploy.sh` 在 changeManifest 六件之内但本树零 diff——该脚本恒 manifest 驱动、对新资产条目（openai.py，md5/restart=engine）零特例，核验接线 = manifest 条目本身。已在新验收件头注同文申报，非漏改。

### 注记 4 · 部署轮人工生效步（live 停等声明）

新增 openai.py bind-mount 后**首次生效须 `docker compose up -d` 重建容器**（= 设备全断、人工窗口），deploy.sh 只做 `docker restart` 不自动重建（compose 行注释 + manifest 两条 note 三处同文）；connection.py 为 engine-face 重启（设备断连重连，K1/P5 同款）。生效核验锚 = docker logs 里 `kidpal persistent-session patch active`（由 provider 探针 `_kidpal_provider_supports_user` 门控；openai.py bind-mount 丢失时打 ERROR 报警而非 marker，防假绿锚）。K2-E6 live 观测项挂 human 部署轮，host 面转绿后包 status `awaiting-human:live-deploy`。
