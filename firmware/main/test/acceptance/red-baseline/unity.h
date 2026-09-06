/*
 * Minimal unity stub — RED-baseline standalone execution scaffold for
 * firmware/main/test/device_ws_switch.acceptance.c (TaskPackage 260903-device-ws-switch).
 *
 * NOT a frozen acceptance file and NOT the acceptance-gate runner. The gate runner is
 * the host_app linux-target ELF per HOST_TEST_CONVENTIONS.md [C4.4] (builder wires the
 * acceptance file into HOST_ACCEPTANCE_SRCS). This stub exists only so acceptance-author
 * can EXECUTE the real assertions for the red baseline before that wiring lands:
 *
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -I. -o /tmp/wsa_red_runner ../../device_ws_switch.acceptance.c runner.c
 *     cd ../../../../../..  (repo root)
 *     /tmp/wsa_red_runner   ; exit code = number of failed TEST_CASEs
 *
 * The stub replaces <unity.h> via -I. and implements the subset of unity macros used by
 * the acceptance file (TEST_CASE constructor registration + *_MESSAGE assert variants).
 * Nothing under firmware/main/test/ is compiled into the device build.
 */
#ifndef UNITY_STUB_H
#define UNITY_STUB_H

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

typedef void (*unity_stub_fn)(void);

#define UNITY_STUB_MAX 64
extern unity_stub_fn unity_stub_cases[UNITY_STUB_MAX];
extern const char *unity_stub_names[UNITY_STUB_MAX];
extern int unity_stub_n;
extern jmp_buf unity_stub_jmp;
extern int unity_stub_failed;

#define UNITY_STUB_CAT_(a, b) a##b
#define UNITY_STUB_CAT(a, b) UNITY_STUB_CAT_(a, b)

#define TEST_CASE(name, tags)                                                  \
    static void UNITY_STUB_CAT(unity_stub_body_, __LINE__)(void);              \
    __attribute__((constructor)) static void UNITY_STUB_CAT(unity_stub_reg_, __LINE__)(void) { \
        if (unity_stub_n < UNITY_STUB_MAX) {                                   \
            unity_stub_cases[unity_stub_n] = UNITY_STUB_CAT(unity_stub_body_, __LINE__); \
            unity_stub_names[unity_stub_n] = (name);                           \
            unity_stub_n++;                                                    \
        }                                                                      \
    }                                                                          \
    static void UNITY_STUB_CAT(unity_stub_body_, __LINE__)(void)

#define UNITY_STUB_FAIL(msg)                                                   \
    do {                                                                       \
        printf("    FAIL: %s\n", (const char *)(msg));                         \
        unity_stub_failed = 1;                                                 \
        longjmp(unity_stub_jmp, 1);                                            \
    } while (0)

#define TEST_ASSERT_TRUE_MESSAGE(c, m)  do { if (!(c)) UNITY_STUB_FAIL(m); } while (0)
#define TEST_ASSERT_FALSE_MESSAGE(c, m) do { if ((c)) UNITY_STUB_FAIL(m); } while (0)
#define TEST_ASSERT_NOT_NULL_MESSAGE(p, m) do { if ((p) == NULL) UNITY_STUB_FAIL(m); } while (0)
#define TEST_ASSERT_NULL_MESSAGE(p, m)     do { if ((p) != NULL) UNITY_STUB_FAIL(m); } while (0)
#define TEST_ASSERT_TRUE(c)  TEST_ASSERT_TRUE_MESSAGE((c), #c)
#define TEST_ASSERT_FALSE(c) TEST_ASSERT_FALSE_MESSAGE((c), #c)
#define TEST_ASSERT_NOT_NULL(c) TEST_ASSERT_NOT_NULL_MESSAGE((c), #c)
#define TEST_ASSERT_NULL(c)     TEST_ASSERT_NULL_MESSAGE((c), #c)

#endif /* UNITY_STUB_H */
