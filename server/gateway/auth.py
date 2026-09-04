"""Forced device-authentication gate (决策22 / V11 / V26).

Frozen acceptance GWA-A3. Pure-python, no fastapi at import time.

Frozen offline contract:
    authorize(env_token, provided_authorization) -> (ok: bool, status: int)
        env_token==""             -> (False, 401)   [token-unset = refuse-service]
        provided != "Bearer <t>"  -> (False, 401)
        correct                   -> (True,  200)
    TOKEN_MIN_CHARS  (>=22, i.e. >=160-bit-ish bearer entropy floor; 决策22 >=128-bit)
    constant-time compare (secrets.compare_digest)

History lesson ARQ-260902-shim-401-gate-bypass: a gate that merely *returns* a
rejected JSONResponse is swallowed by FastAPI dependency injection as the injected
param value and does NOT short-circuit the route. The gate must mechanically REJECT
(return (False, 401) AND be invoked by the runtime as a hard gate that raises on
(False, ...)) — no validation-mode backdoor may let an empty token through.
"""

from __future__ import annotations

import secrets

# 决策22: shared single token >=128-bit. 22 base64url chars ~= 132-bit floor; the
# runtime refuses to start unsecured unless the operator sets at least this many chars.
TOKEN_MIN_CHARS = 22


def is_bearer(provided_authorization: str) -> bool:
    """True iff the provided authorization string is a well-formed 'Bearer …' header."""
    return isinstance(provided_authorization, str) and provided_authorization.startswith("Bearer ")


def authorize(env_token, provided_authorization):
    """Decide whether a device handshake is authorised.

    Returns (ok: bool, status: int) exactly per the frozen contract. This is a pure
    decision function — the runtime wrappers it, and on an (ok==False) branch they
    mechanically short-circuit (raise / refuse the connection). It NEVER returns
    (True, ...) for an unset or wrong token.
    """
    # token-unset = refuse-service (no validation-mode backdoor).
    if not env_token:
        return (False, 401)
    if not isinstance(env_token, str) or not isinstance(provided_authorization, str):
        return (False, 401)
    expected = "Bearer " + env_token
    # constant-time compare — never leak the bearer length/prefix via timing.
    if not secrets.compare_digest(provided_authorization, expected):
        return (False, 401)
    return (True, 200)


def ensure_token_length_ok(env_token: str) -> bool:
    """Reject an operator-set token that is below the 决策22 entropy floor."""
    return len(env_token or "") >= TOKEN_MIN_CHARS