/*
 * host_app_main.c — linux-target entry for the host test app.
 *
 * On CONFIG_IDF_TARGET_LINUX the linux FreeRTOS port's `int main()` launches a task
 * that calls app_main(); it never returns on its own. So we run every registered
 * TEST_CASE (the frozen acceptance file + anything else hung into the runner), capture
 * Unity's failure count, and exit() with it so CI can observe green (0) vs red (non-0).
 *
 * NON-INTERACTIVE: unlike the interactive unity test menu, we auto-run all tests and
 * exit, which is what a headless host-test run needs.
 */
#include <stdlib.h>
#include "unity.h"
#include "sdkconfig.h"

void app_main(void)
{
    UNITY_BEGIN();
    unity_run_all_tests();
    int rc = UNITY_END();   /* returns Unity.TestFailures: 0 = all passed */
    exit(rc < 0 ? 1 : rc);
}