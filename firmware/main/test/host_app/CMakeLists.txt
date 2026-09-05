# Standalone ESP-IDF HOST test app ("host_infra_test").
# This is a SEPARATE ESP-IDF project used ONLY for linux-target ("host") unit tests.
# It deliberately does NOT include the xiaozhi device component tree: COMPONENTS is
# pinned to `main`, so only this test app's main (+ its REQUIRES: unity, lvgl) build
# here. Device-bound components (ESP-SR AFE, LCD drivers, ML307, codecs, boards, ...)
# are NOT resolvable on the linux target (see the host test directory-convention doc
# for the INF-E2 host-vs-device compile matrix). This isolation is what makes host
# tests buildable on a PC without the hardware tree.
cmake_minimum_required(VERSION 3.16)

# Restrict the component set to this test app's main component only (plus its REQUIRES).
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(host_infra_test)