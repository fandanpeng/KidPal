/*
 * Standalone runner main for the WSA red baseline (see unity.h header note).
 * Runs every TEST_CASE registered by the acceptance file, prints PASS/FAIL per case,
 * exits with the number of failed TEST_CASEs.
 */
#include "unity.h"
#include <stdio.h>

unity_stub_fn unity_stub_cases[UNITY_STUB_MAX];
const char *unity_stub_names[UNITY_STUB_MAX];
int unity_stub_n = 0;
jmp_buf unity_stub_jmp;
int unity_stub_failed = 0;

int main(void)
{
    int failures = 0;
    for (int i = 0; i < unity_stub_n; i++) {
        printf("Running %s ... ", unity_stub_names[i]);
        fflush(stdout);
        unity_stub_failed = 0;
        if (setjmp(unity_stub_jmp) == 0) {
            unity_stub_cases[i]();
        }
        if (unity_stub_failed) {
            failures++;
            printf("  -> FAIL\n");
        } else {
            printf("PASS\n");
        }
    }
    printf("-----------------------\n%d Tests %d Failures\n", unity_stub_n, failures);
    return failures;
}
