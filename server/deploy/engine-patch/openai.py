"""KidPal engine-side persistent-session provider patch (261022-m5-k2b, 方案 A).

部署副本补丁：经 docker-compose.yml bind-mount 覆盖镜像内
core/providers/llm/openai/openai.py。基线 = 参考树 05_XiaoZhiServer（0.9.6 同批
源码，response / response_with_functions 原样保留），本补丁面**只到**下述三点，
其余逐字不动（补丁面越薄，镜像升级时的漂移面越小，ARQ-260907 同族纪律）：

  ① 两发送腿签名新增**命名参数** `user`（keyword-only、缺省 None）——
     命名参数是 connection.py 侧 _kidpal_provider_supports_user 漂移探针的判据
     （**kwargs 吞参会把 user 静默丢弃，探针判不出来）；
  ② `user` 真值（非 None 且非空串）→ 请求体 additive 字段 `"user": user`，
     OpenClaw 按 `agent:kidpal:openai-user:<device_id>` 键维护服务端持久会话
     （对话历史唯一权威 = ADR-015）；省略 / None / 空串 → 请求体**不含** user 键，
     回落旧行为（每请求随机 session、调用方自携带历史；契约 §四定案向后兼容）；
  ③ messages 原样透传（provider 是 payload 忠实面：零历史注入、零裁剪——
     历史策略全部归 connection.py 侧）。

纪律（契约 §四 / R9，本补丁零触碰，防回退）：
  · 对 OpenClaw 的流式请求**禁带 max_tokens** —— max_tokens 维持 stock 语义
    （仅显式给出才进请求；KidPal 部署面不配置该键，故请求体恒不含它）。
  · thinking 维持关闭：stock `_apply_thinking_disabled` 钩子原样保留并照常施加
    于 request_params（R9 裁定零触碰）。

可见性：本文件不可离线 import（引擎依赖不在仓）；冻结验收以 AST 抽取两发送腿
方法体在受限命名空间 exec 行为驱动（ET 先例），payload 观察缝 =
`self.client.chat.completions.create(**request_params)`（stock 形态）。
"""

import httpx
import openai
from openai.types import CompletionUsage
from config.logger import setup_logging
from core.utils.util import check_model_key
from core.providers.llm.base import LLMProviderBase
from urllib.parse import urlparse

TAG = __name__
logger = setup_logging()

# 需要禁用思考模式的平台域名及其对应参数（默认关闭思考模式）
THINKING_DISABLED_DOMAINS = {
    "aliyuncs.com": {"enable_thinking": False},
    "deepseek.com": {"thinking": {"type": "disabled"}},
    "bigmodel.cn": {"thinking": {"type": "disabled"}},
    "moonshot.cn": {"thinking": {"type": "disabled"}},
    "volces.com": {"thinking": {"type": "disabled"}},
}


class LLMProvider(LLMProviderBase):
    def __init__(self, config):
        self.model_name = config.get("model_name")
        self.api_key = config.get("api_key")
        if "base_url" in config:
            self.base_url = config.get("base_url")
        else:
            self.base_url = config.get("url")

        timeout_config = config.get("timeout")
        if isinstance(timeout_config, dict):
            # 细粒度超时配置
            custom_timeout = httpx.Timeout(
                pool=timeout_config.get("pool", 2.0),
                connect=timeout_config.get("connect", 3.0),
                write=timeout_config.get("write", 5.0),
                read=timeout_config.get("read", 60.0)
            )
        elif isinstance(timeout_config, (int, float)) and timeout_config > 0:
            # 兼容旧的单一超时配置（整数或浮点数）
            custom_timeout = httpx.Timeout(timeout_config)
        else:
            # 未配置或配置无效，使用默认值
            custom_timeout = httpx.Timeout(300)

        param_defaults = {
            "max_tokens": int,
            "temperature": lambda x: round(float(x), 1),
            "top_p": lambda x: round(float(x), 1),
            "frequency_penalty": lambda x: round(float(x), 1),
        }

        for param, converter in param_defaults.items():
            value = config.get(param)
            try:
                setattr(
                    self,
                    param,
                    converter(value) if value not in (None, "") else None,
                )
            except (ValueError, TypeError):
                setattr(self, param, None)

        logger.debug(
            f"意图识别参数初始化: {self.temperature}, {self.max_tokens}, {self.top_p}, {self.frequency_penalty}"
        )

        model_key_msg = check_model_key("LLM", self.api_key)
        if model_key_msg:
            logger.bind(tag=TAG).error(model_key_msg)
        self.client = openai.OpenAI(api_key=self.api_key, base_url=self.base_url, timeout=custom_timeout)

    @staticmethod
    def normalize_dialogue(dialogue):
        """自动修复 dialogue 中缺失 content 的消息"""
        for msg in dialogue:
            if "role" in msg and "content" not in msg:
                msg["content"] = ""
        return dialogue

    def _apply_thinking_disabled(self, request_params: dict):
        """根据域名自动禁用思考模式"""
        parsed_url = urlparse(self.base_url)
        domain = parsed_url.netloc
        for disabled_domain, params in THINKING_DISABLED_DOMAINS.items():
            if disabled_domain in domain:
                request_params.setdefault("extra_body", {}).update(params)
                logger.bind(tag=TAG).info(f"为域名 {domain} 禁用思考模式，参数: {params}")
                break

    def response(self, session_id, dialogue, *, user=None, **kwargs):
        dialogue = self.normalize_dialogue(dialogue)

        request_params = {
            "model": self.model_name,
            "messages": dialogue,
            "stream": True,
        }

        # 添加可选参数,只有当参数不为None时才添加
        optional_params = {
            "max_tokens": kwargs.get("max_tokens", self.max_tokens),
            "temperature": kwargs.get("temperature", self.temperature),
            "top_p": kwargs.get("top_p", self.top_p),
            "frequency_penalty": kwargs.get("frequency_penalty", self.frequency_penalty),
        }

        for key, value in optional_params.items():
            if value is not None:
                request_params[key] = value

        # KidPal 持久会话标识（261022-m5-k2b）：user 真值才入请求体（additive 字段，
        # 字段名冻结 = "user"）；None / 空串 → 键缺席 = 回落旧行为（每请求随机 session）。
        if user:
            request_params["user"] = user

        # 禁用思考模式
        self._apply_thinking_disabled(request_params)

        responses = self.client.chat.completions.create(**request_params)

        is_active = True
        try:
            for chunk in responses:
                try:
                    delta = chunk.choices[0].delta if getattr(chunk, "choices", None) else None
                    content = getattr(delta, "content", "") if delta else ""
                except IndexError:
                    content = ""
                if content:
                    if "<think>" in content:
                        is_active = False
                        content = content.split("<think>")[0]
                    if "</think>" in content:
                        is_active = True
                        content = content.split("</think>")[-1]
                    if is_active:
                        yield content
        finally:
            responses.close()

    def response_with_functions(self, session_id, dialogue, functions=None, *, user=None, **kwargs):
        dialogue = self.normalize_dialogue(dialogue)

        request_params = {
            "model": self.model_name,
            "messages": dialogue,
            "stream": True,
            "tools": functions,
        }

        optional_params = {
            "max_tokens": kwargs.get("max_tokens", self.max_tokens),
            "temperature": kwargs.get("temperature", self.temperature),
            "top_p": kwargs.get("top_p", self.top_p),
            "frequency_penalty": kwargs.get("frequency_penalty", self.frequency_penalty),
        }

        for key, value in optional_params.items():
            if value is not None:
                request_params[key] = value

        # KidPal 持久会话标识（261022-m5-k2b）：与 response 腿同形同义。
        if user:
            request_params["user"] = user

        # 禁用思考模式
        self._apply_thinking_disabled(request_params)

        stream = self.client.chat.completions.create(**request_params)

        try:
            for chunk in stream:
                if getattr(chunk, "choices", None):
                    delta = chunk.choices[0].delta
                    content = getattr(delta, "content", "")
                    tool_calls = getattr(delta, "tool_calls", None)
                    yield content, tool_calls
                elif isinstance(getattr(chunk, "usage", None), CompletionUsage):
                    usage_info = getattr(chunk, "usage", None)
                    logger.bind(tag=TAG).info(
                        f"Token 消耗：输入 {getattr(usage_info, 'prompt_tokens', '未知')}，"
                        f"输出 {getattr(usage_info, 'completion_tokens', '未知')}，"
                        f"共计 {getattr(usage_info, 'total_tokens', '未知')}"
                    )
        finally:
            stream.close()
