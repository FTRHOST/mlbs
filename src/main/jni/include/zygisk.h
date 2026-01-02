#pragma once
#include <jni.h>

#define ZYGISK_API_VERSION 4

namespace zygisk {

class Api;
class AppSpecializeInfo;

class ModuleBase {
public:
    virtual ~ModuleBase() = default;
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeInfo *info) {}
    virtual void postAppSpecialize(const AppSpecializeInfo *info) {}
};

class Api {
public:
    int connectCompanion();
    void pltHookRegister(const char *lib, const char *sym, void *new_func, void **old_func);
    void pltHookCommit();
    void pltHookExclude(const char *lib, const char *sym);
    void pltHookUnhook(const char *lib, const char *sym);
    const char *getModuleDir();
};

struct AppSpecializeInfo {
    JNIEnv *env;
    int uid;
    int gid;
    jintArray gids;
    const char *nice_name;
    const char *app_data_dir;
};

} // namespace zygisk

#define REGISTER_ZYGISK_MODULE(clazz) \
static void module_handler(zygisk::Api *api, JNIEnv *env) { \
    static clazz module; \
    module.onLoad(api, env); \
} \
extern "C" __attribute__((visibility ("default"))) \
void* zygisk_module_entry(int *version) { \
    *version = ZYGISK_API_VERSION; \
    return (void *) module_handler; \
}
