# evidence · validate_display 样本回归（契约 diff 落地批，2026-09-24）

> 执行 = 主线程（landing 批收口义务②，M3-K1 先例形态）；验证器 = `server/gateway/display_schema.py::validate_display`（现行为现役版——**Q8 story 形状 floor 尚未实现，归 K1 builder（gate evidence C1）**，本样本 = 白名单 + data 存在性 + 大小上限面的通过证据，如实注记不夸大）。

## 命令

```
python -X utf8（sys.path.insert server/gateway; from display_schema import validate_display; 四样本逐一调用）
```

## 样本与结果（逐字输出）

| # | 样本 | 结果 |
|---|---|---|
| S1 | story_list 新语义：`{"view":"story_list","data":{"total":5,"items":[{"id":"2026-09-20-xiaotuzi","title":"我和小兔子的冒险","date":"09-20"},{"id":"2026-09-12-leizhenyu","title":"雷阵雨观察","date":"09-12"}]}}` | `ok=True reason='ok'` |
| S2 | story_text 带 seq（additive）：`{"view":"story_text","data":{"id":"2026-09-24-maoxian","title":"我的大冒险","text":"从前有一只小猫……","seq":6}}` | `ok=True reason='ok'` |
| S3 | story_text 缺 seq（缺省向后兼容）：同 S2 去 `seq` 键 | `ok=True reason='ok'` |
| S4 | **负样本**：kidpal_story_delete 伪装 display view `{"view":"kidpal_story_delete","data":{"id":"2026-09-20-xiaotuzi"}}`（须拒——§四端云消息不进 display 白名单） | `ok=False reason="view 'kidpal_story_delete' is not in whitelist ['char', 'control', 'home', 'schedule', 'story_list', 'story_text', 'text']"` |

## 判读

- S1–S3 = 新契约形状在现行验证器下可转发（additive 面零破坏）；S4 = 删除消息误入 display 面被白名单机械拒绝（「不进白名单」条款的现役执行证据）。
- 待 K1 builder 落 Q8 floor 后，本样本批由 K1 acceptance 齿（M4K1 系）在新验证器上复跑扩样（脏 total / items 缺键 / seq 非整数负样本），本文件为落地批时点快照、不回改。

## 同批回归

`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **134 passed / 4 deselected / 0 failed**（story.py 三处 docstring 钉行零行为实证；基线 = M3-K1 期 47 + K7 期 16 + perf-k1 等增量后的现役全量，零新增红）。
