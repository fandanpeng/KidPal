/*
 * story_sd_io.cc — story 正文 SD 只读镜像的芯片侧薄适配（TaskPackage
 * 261016-p3-story-body-sd-cache；GAP-2 真机 SD 读回 roundtrip 承载面）。
 *
 * 语义权威：docs/requirements.md v1.64 §S-F2.1 AC2/AC7 + 补充规则「正文缓存边界」；
 * 架构决策 24 窄例外（云端 story 归档 = 唯一真相源，板端 SD 只放正文的只读镜像，
 * 一切写入仍走云）；seam contract [C3] 的芯片侧半（真 fopen / FATFS 写序）。
 *
 * 设计纪律（architect 预评 ④）:
 *   - sd_mounted 显式入参注入（不碰板级私有态；调用方从 Board::SdCardMounted() 取）;
 *   - 无卡 / 坏卡短路、绝不 fopen（整层失效 = 不落盘、不崩溃）;
 *   - **先写卡成功才登记 RAM 索引**——本文件只回报 write_ok，登记由调用方经
 *     kidpal_story_cache_commit 承载;
 *   - story id 进文件名前字符集校验（[C9] kidpal_story_id_safe），禁路径拼接注入;
 *   - 只留正文（单一 <id>.txt），不留提醒 / 课表 / 日志等任何其它业务数据。
 *
 * 文件布局：/sdcard/story/<id>.txt（id = story_list items 下发过的 ASCII slug;
 * LFN 键 = CONFIG_FATFS_LFN_HEAP）。本包非目标 = 热插拔检测 / 缓存淘汰。
 */

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <dirent.h>
#include <sys/stat.h>

#include <esp_log.h>

#include "story_cache.h"  /* [C1]/[C3]/[C9] 纯逻辑面 + 本文件实现的 SD I/O 声明 */

#define TAG "StorySdIo"

namespace {

constexpr const char* kStoryDir = "/sdcard/story";
constexpr const char* kStoryDirSlash = "/sdcard/story/";
constexpr const char* kStoryExt = ".txt";

/* 建 /sdcard/story（幂等；失败只记日志、不 panic——写路径随后会自行失败降级）。 */
void EnsureStoryDir() {
    if (mkdir(kStoryDir, 0777) != 0 && errno != EEXIST) {
        ESP_LOGW(TAG, "story dir mkdir failed path=%s errno=%d", kStoryDir, errno);
    }
}

bool BuildStoryPath(const char* id, char* out, size_t cap) {
    int need = snprintf(out, cap, "%s%s%s", kStoryDirSlash, id, kStoryExt);
    return need > 0 && static_cast<size_t>(need) + 1 <= cap;
}

}  // namespace

extern "C" int kidpal_story_sd_write(const char* id, const char* text, int sd_mounted)
{
    char path[128];
    FILE* f = nullptr;
    size_t len;
    size_t written;
    int close_rc;

    if (kidpal_story_id_safe(id) != 1 || text == nullptr) {
        return -1; /* 非法 id / 无正文：绝不拼路径、绝不落盘 */
    }
    if (sd_mounted == 0) {
        return 0; /* 卡不在位 / 坏卡：整层失效，绝不 fopen */
    }
    if (!BuildStoryPath(id, path, sizeof(path))) {
        return -1;
    }
    EnsureStoryDir();
    f = fopen(path, "wb");
    if (f == nullptr) {
        ESP_LOGW(TAG, "story_sd_write FAIL step=open path=%s errno=%d", path, errno);
        return -1;
    }
    len = strlen(text);
    written = fwrite(text, 1, len, f);
    close_rc = fclose(f);
    if (written != len || close_rc != 0) {
        ESP_LOGW(TAG, "story_sd_write FAIL step=write path=%s written=%u/%u fclose=%d "
                      "errno=%d",
                 path, static_cast<unsigned>(written), static_cast<unsigned>(len),
                 close_rc, errno);
        (void)remove(path); /* 尽力清场：半截文件绝不被当成本地留存 */
        return -1;
    }
    ESP_LOGI(TAG, "story_sd_write ok path=%s bytes=%u", path,
             static_cast<unsigned>(len));
    return 1;
}

extern "C" int kidpal_story_sd_read(const char* id, char* out, size_t cap, int sd_mounted)
{
    char path[128];
    FILE* f = nullptr;
    size_t nread;

    if (kidpal_story_id_safe(id) != 1 || out == nullptr || cap == 0) {
        return -1;
    }
    if (sd_mounted == 0) {
        return 0; /* 卡不在位：绝不 fopen */
    }
    if (!BuildStoryPath(id, path, sizeof(path))) {
        return -1;
    }
    f = fopen(path, "rb");
    if (f == nullptr) {
        return 0; /* 无本地副本 = 未命中（不是错误） */
    }
    nread = fread(out, 1, cap - 1, f);
    if (ferror(f)) {
        fclose(f);
        ESP_LOGW(TAG, "story_sd_read FAIL step=read path=%s errno=%d", path, errno);
        return -1;
    }
    fclose(f);
    out[nread] = '\0';
    return 1;
}

extern "C" int kidpal_story_sd_rebuild_index(kidpal_story_cache_t* cache, int sd_mounted)
{
    DIR* dir;
    struct dirent* de;
    int n = 0;

    if (cache == nullptr) {
        return -1;
    }
    if (sd_mounted == 0) {
        return 0; /* 卡不在位：索引保持为空（整层失效） */
    }
    dir = opendir(kStoryDir);
    if (dir == nullptr) {
        /* 目录尚未建（本机从未缓存过）——不是错误。 */
        return 0;
    }
    while ((de = readdir(dir)) != nullptr) {
        char id[KSTORY_CACHE_ID_MAX];
        size_t name_len;
        size_t ext_len = strlen(kStoryExt);

        if (de->d_name[0] == '.') {
            continue; /* "." / ".." / 隐藏件 */
        }
        name_len = strlen(de->d_name);
        if (name_len <= ext_len ||
            strcmp(de->d_name + name_len - ext_len, kStoryExt) != 0) {
            continue; /* 非 <id>.txt 件：只认正文镜像, 不碰其它介质内容 */
        }
        if (name_len - ext_len + 1 > sizeof(id)) {
            continue; /* 超长 id 直接忽略（防截断撞名） */
        }
        memcpy(id, de->d_name, name_len - ext_len);
        id[name_len - ext_len] = '\0';
        if (kidpal_story_id_safe(id) != 1) {
            continue; /* 非法名不进索引（路径注入面零暴露） */
        }
        if (kidpal_story_cache_add(cache, id) == 1) {
            n++;
        }
    }
    closedir(dir);
    ESP_LOGI(TAG, "story_sd_rebuild_index ok entries=%d", n);
    return 0;
}
