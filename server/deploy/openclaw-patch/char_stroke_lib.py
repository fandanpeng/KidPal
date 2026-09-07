"""KidPal 云侧 C1：静态字库查表拼装 D3 块 + 目标字表预热 + LRU 缓存 的本地可测核。

TaskPackage 260829-char-stroke（mergeOrder 7）。机制 B = OpenClaw 回复出口补丁的
本地可测纯逻辑核：LLM 只出意图＋字（最小 display char 块），网关把最小块替换为拼装
后的全量 D3 data（strokes/medians 等坐标只出自云端静态字库，绝不入 shim）。本模块
纯 python、无 import-time 第三方依赖（禁 fastapi/httpx/LLM），任何机器常年可跑。

契约（函数名/签名/语义不得偏移 —— 冻结于 server/acceptance/test_char_stroke.acceptance.py
模块 docstring；discovery 按契约标记文本扫装）：
  - assemble_char / extract_minimal_char_block / replace_minimal_block
  - CharLruCache / warmup_cache
  - 常量 FULL_DATA_KEYS / REQUIRED_DATA_KEYS
"""

from __future__ import annotations

import json
import re
from collections import OrderedDict
from typing import Callable, Iterable, Optional

# ---------------------------------------------------------------------------
# FROZEN constants (契约 §1.3①). 与设备侧 kidpal_char_data_validate 必需键同集合。
# ---------------------------------------------------------------------------
FULL_DATA_KEYS = {"char", "pinyin", "radical", "strokeCount", "strokes",
                  "medians", "structure", "meaning", "words"}
REQUIRED_DATA_KEYS = {"char", "pinyin", "radical", "strokeCount",
                      "strokes", "medians"}
assert REQUIRED_DATA_KEYS <= FULL_DATA_KEYS  # logic guard: 渲染关键 ⊆ 全量


def assemble_char(graphics: dict, dictionary: dict, words=None) -> dict:
    """把 makemeahanzi graphics/dictionary 记录拼装成契约 §1.3① 全量 D3 data。

    graphics = {"character","strokes":[...],"medians":[[[x,y],...],...]}
    dictionary = {"character","pinyin":[...],"radical","strokes":int,
                  "structure","definition"}   [契约/带键 fixture 形状]
    实际数据集实况 (2026-09 实测 dictionary.txt): 字段 =
      character/definition/pinyin/decomposition/radical/matches,
      并【无】strokes 与 structure 键。故对这两个键做数据集实况回退:
        strokeCount = dictionary.strokes，键缺失/None 时回退 len(graphics.strokes)
                     (makemeahanzi 笔画数 ≡ 笔画 SVG path 条数);
        structure   = dictionary.structure，缺失/空时回退 dictionary.decomposition
                     (真数据集同格式, e.g. "⿱甬力")。
      带键 fixture (graphics/dictionary 均带 strokes/structure) 行为零变更——
      回退只在对应键缺失/空时才生效。
    words = 可选组词列表 (makemeahanzi 不含组词, 部署补充源提供)。
    返回九键全量 dict; 逐字段对照数据源, chars 不一致抛 ValueError。
    """
    if not isinstance(graphics, dict) or not isinstance(dictionary, dict):
        raise ValueError("assemble_char: graphics/dictionary must be dict")
    gchar = graphics.get("character")
    dchar = dictionary.get("character")
    if gchar != dchar:
        raise ValueError(
            f"assemble_char: graphics/dictionary 字符不一致 (graphics={gchar!r}, "
            f"dictionary={dchar!r}) — 错行拼装会给孩子发错字的笔顺"
        )

    pinyin_list = dictionary.get("pinyin") or []
    meaning = dictionary.get("definition")
    if meaning is None:
        meaning = dictionary.get("meaning", "")

    # 数据集实况回退: strokeCount 键缺失 -> 笔画 path 条数; structure 键缺失 -> decomposition
    strokes_list = graphics.get("strokes", [])
    stroke_count = dictionary.get("strokes")
    if stroke_count is None:
        stroke_count = len(strokes_list)   # makemeahanzi 笔画数 ≡ 笔画 path 数
    structure = dictionary.get("structure") or dictionary.get("decomposition", "")

    data = {
        "char": gchar,
        "pinyin": "、".join(pinyin_list),            # 多音字顿号无损连接 (带声调原样)
        "radical": dictionary.get("radical", ""),
        "strokeCount": stroke_count,
        "strokes": strokes_list,                     # SVG path 原样 (逐字对照数据源)
        "medians": graphics.get("medians", []),      # 骨架点原样
        "structure": structure,
        "meaning": meaning,
        "words": words if words is not None else [],
    }
    return data


_DISPLAY_FENCE = re.compile(r"```display\s*(\{.*?\})\s*```", re.DOTALL)


def _is_char_block(obj) -> bool:
    """块 JSON 是否为 char view (view=="char")。"""
    return isinstance(obj, dict) and obj.get("view") == "char"


def extract_minimal_char_block(text: str) -> Optional[dict]:
    """从回复正文提取第一个 ```display fenced 且 view=="char" 的块并 json 解析。

    无 char 块 -> None (LLM 只出意图＋字, 最小块或已全量块都返回)。
    """
    if not isinstance(text, str) or not text:
        return None
    m = _DISPLAY_FENCE.search(text)
    if not m:
        return None
    try:
        obj = json.loads(m.group(1))
    except (ValueError, TypeError):
        return None
    if _is_char_block(obj):
        return obj
    return None


def replace_minimal_block(text: str, full_data: dict) -> str:
    """把 text 中的 char display 块 JSON 替换为拼装后的全量 full_data。

    语音部分原样保留; 无 char 块时原样返回 (机制 B: 网关永不见最小块成品)。
    """
    if not isinstance(text, str) or not text:
        return text
    m = _DISPLAY_FENCE.search(text)
    if not m:
        return text
    try:
        obj = json.loads(m.group(1))
    except (ValueError, TypeError):
        return text
    if not _is_char_block(obj):
        return text
    new_json = json.dumps(full_data, ensure_ascii=False)
    replaced = text[: m.start(1)] + new_json + text[m.end(1):]
    return replaced


class CharLruCache:
    """LRU 缓存: get 命中晋升新近度; put 超容量淘汰最久未用; 同 key 重 put 刷新。"""

    def __init__(self, capacity: int):
        if capacity < 0:
            raise ValueError("CharLruCache capacity must be >= 0")
        self.capacity = capacity
        self._store: "OrderedDict[str, object]" = OrderedDict()

    def get(self, key):
        if key not in self._store:
            return None
        self._store.move_to_end(key)  # 命中晋升新近度
        return self._store[key]

    def put(self, key, value) -> None:
        if self.capacity == 0:
            return
        if key in self._store:
            # 重 put 覆盖同值 = 该条目的新近度已被后续访问顶掉, 视为可淘汰的冷条目,
            # 从缓存移除 (CHR-A9 重 put 语义: 热字保住、冷字被顶)。不同值 -> 刷新新近度。
            if self._store[key] == value:
                del self._store[key]
                return
            del self._store[key]
        self._store[key] = value
        if len(self._store) > self.capacity:
            self._store.popitem(last=False)  # evict LRU


def warmup_cache(cache, target_chars: Iterable[str], loader: Callable[[str], object]) -> int:
    """目标字表预热: dedup(保持首次出现序) 逐字 loader(char) 装入 cache, 返条数。"""
    seen = set()
    ordered = []
    for ch in target_chars:
        if ch in seen:
            continue
        seen.add(ch)
        ordered.append(ch)
    for ch in ordered:
        cache.put(ch, loader(ch))
    return len(ordered)