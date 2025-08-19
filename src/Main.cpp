#include <cstdio>
#include <dirent.h>
#include <memory>
#include <string.h>
#include <string>

#include "Globals.h"
#include "luaroma/Luaroma.h"
#include "luaroma/LuaromaDefs.h"
#include "system/CFile.h"
#include "system/FSUtils.h"
#include "utils/Kernel.h"
#include "utils/Logger.h"
#include "utils/PluginInfo.h"

#include <coreinit/mcp.h>
#include <coreinit/title.h>
#include <nn/ac.h>
#include <nn/act.h>
#include <vpad/input.h>
#include <whb/proc.h>

#include <wups.h>
#include <wups/config/WUPSConfigItemStub.h>

#include <curl/curl.h>

WUPS_PLUGIN_NAME("Luaroma");
WUPS_PLUGIN_DESCRIPTION("Luaroma is a Wii U Project that allows people to run "
                        "Lua Scripts on Wii U instead of using "
                        "the complicated stuff.");
WUPS_PLUGIN_VERSION("v0.0.8a");
WUPS_PLUGIN_AUTHOR("c0ldcut1e");
WUPS_PLUGIN_LICENSE("Proprietary");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("luaroma");

#define LUAROMA_SCRIPTS_DIR "fs:/vol/external01/wiiu/luaroma/scripts"

extern "C" size_t curlWriteCallback(char *ptr, size_t size, size_t nmemb,
                                    void *userdata) {
    auto *response = (std::string *) userdata;
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

void generateLicense() {
    // This part of the code has been removed.
}

void verifyLicense() {
    // This part of the code has been removed.
}

WUPSConfigAPICallbackStatus
configMenuOpenendCallback(WUPSConfigCategoryHandle rootHandle) {
    // This part of the code has been removed.
}

CThread *thread = nullptr;

INITIALIZE_PLUGIN() {
    generateLicense();
    verifyLicense();

    WHBProcInit();
    initLogging();

    DEBUG_FUNCTION_LINE("%s", gLicenseStr.value().c_str());

    DEBUG_FUNCTION_LINE("Hello from %s!", getPluginName());

    WUPSConfigAPIOptionsV1 configOptions = {.name = getPluginName()};
    WUPSConfigAPI_Init(configOptions, configMenuOpenendCallback, []() {});

    Luaroma &luaroma = Luaroma::instance();

    LuaromaStatus ret = luaroma.init();
    if (ret.error != LuaromaStatus::LUAROMA_OK) {
        char buffer[0x100];
        snprintf(buffer, 0x100, "Luaroma::init() failed: %d/%s", ret.value,
                 errorToStr(ret.error).c_str());
        OSFatal(buffer);
    }

    gScriptThread = CThread::create(
            [](CThread *self, void *arg) {
                DIR *dir = opendir(LUAROMA_SCRIPTS_DIR);
                if (!dir) OSFatal("opendir() failed");

                struct dirent *entry;
                while ((entry = readdir(dir)) != nullptr) {
                    if (strcmp(entry->d_name, ".") == 0 ||
                        strcmp(entry->d_name, "..") == 0)
                        continue;

                    std::string path = std::string(LUAROMA_SCRIPTS_DIR) + "/" +
                                       entry->d_name;

                    if (entry->d_type != DT_REG) continue;

                    CFile file;
                    if (file.open(path, CFile::ReadOnly) < 0) {
                        char buffer[0x100];
                        snprintf(buffer, 0x100, "CFile::open() failed: %s",
                                 path.c_str());
                        OSFatal(buffer);
                    }

                    uint64_t size = file.size();
                    std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);
                    if (file.read(buffer.get(), (int32_t) size) !=
                        (int32_t) size) {
                        char buffer[0x100];
                        snprintf(buffer, 0x100, "CFile::read() failed: %s",
                                 path.c_str());
                        OSFatal(buffer);
                    }

                    Luaroma::instance().runScript(buffer.get(), size,
                                                  entry->d_name);
                }

                closedir(dir);
            },
            nullptr, CThread::eAttributeDetach);
    gScriptThread->resumeThread();
}

DEINITIALIZE_PLUGIN() {
    if (thread) thread->shutdownThread();

    if (gScriptThread) gScriptThread->shutdownThread();

    Luaroma::instance().deinit();

    DEBUG_FUNCTION_LINE("Goodbye from %s o/", getPluginName());

    deinitLogging();
    WHBProcShutdown();

    curl_global_cleanup();
}

ON_APPLICATION_START() {}

ON_APPLICATION_ENDS() {}