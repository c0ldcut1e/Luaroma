#include "LuaromaFuncs.h"

#include <cmath>
#include <cstdio>
#include <stdlib.h>
#include <string>

#include <Globals.h>
#include <luaroma/types/uint64.h>
#include <system/CFile.h>
#include <system/FSUtils.h>
#include <utils/Kernel.h>
#include <utils/Logger.h>

#include <coreinit/debug.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <vpad/input.h>
#include <whb/log.h>
#include <whb/proc.h>

#include <curl/curl.h>

#define ADD_LUA_FUNC(func, name)                                               \
    lua_pushcfunction(L, func);                                                \
    lua_setfield(L, -2, name);

#define ADD_LUA_TABLE(func, name)                                              \
    func;                                                                      \
    lua_setfield(L, -2, name);

static size_t CurlCallbackWriteToString(void *contents, size_t size,
                                        size_t nmemb, void *userp) {
    std::string *str = (std::string *) userp;
    size_t totalSize = size * nmemb;
    str->append((char *) contents, totalSize);
    return totalSize;
}

static size_t CurlCallbackWriteToFile(void *contents, size_t size, size_t nmemb,
                                      void *userp) {
    CFile *file = (CFile *) userp;
    return file->write((uint8_t *) contents, size * nmemb);
}

namespace LuaromaFuncs {
    // ──────────── Logging ─────────────

    int32_t Luaroma__Write(lua_State *L) {
        WHBLogWrite(luaL_checkstring(L, 1));
        return 0;
    }

    int32_t Luaroma__Print(lua_State *L) {
        WHBLogPrint(luaL_checkstring(L, 1));
        return 0;
    }

    // ───────────── Kernel ─────────────

    int32_t Luaroma__KernelMemcpy(lua_State *L) {
        uint32_t dest = (uint32_t) luaL_checknumber(L, 1);
        uint32_t src  = (uint32_t) luaL_checknumber(L, 2);
        size_t size   = (size_t) luaL_checknumber(L, 3);
        KernelMemcpy((void *) dest, (void *) src, size);
        return 0;
    }

    int32_t Luaroma__KernelMemset(lua_State *L) {
        uint32_t dest = (uint32_t) luaL_checknumber(L, 1);
        uint8_t value = (uint8_t) luaL_checknumber(L, 2);
        size_t size   = (size_t) luaL_checknumber(L, 3);
        KernelMemset((void *) dest, value, size);
        return 0;
    }

    int32_t Luaroma__Peek8(lua_State *L) {
        uint32_t address = (uint32_t) luaL_checknumber(L, 1);
        lua_pushinteger(L, (lua_Integer) Peek8(address));
        return 1;
    }

    int32_t Luaroma__Peek16(lua_State *L) {
        uint32_t address = (uint32_t) luaL_checknumber(L, 1);
        lua_pushinteger(L, (lua_Integer) Peek16(address));
        return 1;
    }

    int32_t Luaroma__Peek32(lua_State *L) {
        uint32_t address = (uint32_t) luaL_checknumber(L, 1);
        lua_pushinteger(L, (lua_Integer) Peek32(address));
        return 1;
    }

    int32_t Luaroma__Poke8(lua_State *L) {
        uint32_t address = (uint32_t) luaL_checknumber(L, 1);
        uint8_t value    = (uint8_t) luaL_checknumber(L, 2);
        Poke8(address, value);
        return 0;
    }

    int32_t Luaroma__Poke16(lua_State *L) {
        uint32_t address = (uint32_t) luaL_checknumber(L, 1);
        uint16_t value   = (uint16_t) luaL_checknumber(L, 2);
        Poke16(address, value);
        return 0;
    }

    int32_t Luaroma__Poke32(lua_State *L) {
        uint32_t address = (uint32_t) luaL_checknumber(L, 1);
        uint32_t value   = (uint32_t) luaL_checknumber(L, 2);
        Poke32(address, value);
        return 0;
    }

    // ───────────── Input ──────────────

    void registerVPadButton(lua_State *L) {
        lua_newtable(L);
        lua_pushinteger(L, VPAD_BUTTON_A);
        lua_setfield(L, -2, "A");
        lua_pushinteger(L, VPAD_BUTTON_B);
        lua_setfield(L, -2, "B");
        lua_pushinteger(L, VPAD_BUTTON_X);
        lua_setfield(L, -2, "X");
        lua_pushinteger(L, VPAD_BUTTON_Y);
        lua_setfield(L, -2, "Y");
        lua_pushinteger(L, VPAD_BUTTON_LEFT);
        lua_setfield(L, -2, "LEFT");
        lua_pushinteger(L, VPAD_BUTTON_RIGHT);
        lua_setfield(L, -2, "RIGHT");
        lua_pushinteger(L, VPAD_BUTTON_UP);
        lua_setfield(L, -2, "UP");
        lua_pushinteger(L, VPAD_BUTTON_DOWN);
        lua_setfield(L, -2, "DOWN");
        lua_pushinteger(L, VPAD_BUTTON_ZL);
        lua_setfield(L, -2, "ZL");
        lua_pushinteger(L, VPAD_BUTTON_ZR);
        lua_setfield(L, -2, "ZR");
        lua_pushinteger(L, VPAD_BUTTON_L);
        lua_setfield(L, -2, "L");
        lua_pushinteger(L, VPAD_BUTTON_R);
        lua_setfield(L, -2, "R");
        lua_pushinteger(L, VPAD_BUTTON_PLUS);
        lua_setfield(L, -2, "PLUS");
        lua_pushinteger(L, VPAD_BUTTON_MINUS);
        lua_setfield(L, -2, "MINUS");
        lua_pushinteger(L, VPAD_BUTTON_HOME);
        lua_setfield(L, -2, "HOME");
        lua_pushinteger(L, VPAD_BUTTON_SYNC);
        lua_setfield(L, -2, "SYNC");
        lua_pushinteger(L, VPAD_BUTTON_STICK_R);
        lua_setfield(L, -2, "STICK_R");
        lua_pushinteger(L, VPAD_BUTTON_STICK_L);
        lua_setfield(L, -2, "STICK_L");
        lua_pushinteger(L, VPAD_BUTTON_TV);
        lua_setfield(L, -2, "TV");
    }

    void registerVPadStick(lua_State *L) {
        lua_newtable(L);
        lua_pushinteger(L, VPAD_STICK_R_EMULATION_LEFT);
        lua_setfield(L, -2, "R_EMULATION_LEFT");
        lua_pushinteger(L, VPAD_STICK_R_EMULATION_RIGHT);
        lua_setfield(L, -2, "R_EMULATION_RIGHT");
        lua_pushinteger(L, VPAD_STICK_R_EMULATION_UP);
        lua_setfield(L, -2, "R_EMULATION_UP");
        lua_pushinteger(L, VPAD_STICK_R_EMULATION_DOWN);
        lua_setfield(L, -2, "R_EMULATION_DOWN");
        lua_pushinteger(L, VPAD_STICK_L_EMULATION_LEFT);
        lua_setfield(L, -2, "L_EMULATION_LEFT");
        lua_pushinteger(L, VPAD_STICK_L_EMULATION_RIGHT);
        lua_setfield(L, -2, "L_EMULATION_RIGHT");
        lua_pushinteger(L, VPAD_STICK_L_EMULATION_UP);
        lua_setfield(L, -2, "L_EMULATION_UP");
        lua_pushinteger(L, VPAD_STICK_L_EMULATION_DOWN);
        lua_setfield(L, -2, "L_EMULATION_DOWN");
    }

    void pushVPadStatus(lua_State *L, const VPADStatus &status) {
        lua_newtable(L);
        lua_pushinteger(L, status.hold);
        lua_setfield(L, -2, "hold");
        lua_pushinteger(L, status.trigger);
        lua_setfield(L, -2, "trigger");
        lua_pushinteger(L, status.release);
        lua_setfield(L, -2, "release");
    }

    int32_t Luaroma__VPadRead(lua_State *L) {
        VPADStatus status;
        VPADReadError err;
        VPADRead(VPAD_CHAN_0, &status, 1, &err);

        switch (err) {
            case VPAD_READ_SUCCESS:
                pushVPadStatus(L, status);
                return 1;

            case VPAD_READ_NO_SAMPLES:
                DEBUG_FUNCTION_LINE(
                        "VPadRead() failed with NO_SAMPLES, not quitting");
                lua_pushnil(L);
                return 1;

            default: {
                char buf[0x100];
                snprintf(buf, sizeof(buf), "VPADRead() failed: %d", err);
                OSFatal(buf);
                return 0;
            }
        }

        pushVPadStatus(L, status);
        return 1;
    }

    // ───────────── Time ───────────────

    void registerTimeFormat(lua_State *L) {
        lua_newtable(L);
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "TICKS");
        lua_pushinteger(L, 1);
        lua_setfield(L, -2, "NANOSECONDS");
        lua_pushinteger(L, 2);
        lua_setfield(L, -2, "MICROSECONDS");
        lua_pushinteger(L, 3);
        lua_setfield(L, -2, "MILLISECONDS");
        lua_pushinteger(L, 4);
        lua_setfield(L, -2, "SECONDS");
    }

    int32_t Luaroma__TimeToTicks(lua_State *L) {
        lua_uint64 *value  = (lua_uint64 *) GET_LUA_UINT64(L, 1);
        int32_t timeFormat = (int32_t) luaL_checknumber(L, 2);

        if (timeFormat == 0) {
            PUSH_LUA_UINT64(L, value->value);
        } else if (timeFormat == 1) {
            PUSH_LUA_UINT64(L, OSNanosecondsToTicks(value->value));
        } else if (timeFormat == 2) {
            PUSH_LUA_UINT64(L, OSMicrosecondsToTicks(value->value));
        } else if (timeFormat == 3) {
            PUSH_LUA_UINT64(L, OSMillisecondsToTicks(value->value));
        } else if (timeFormat == 4) {
            PUSH_LUA_UINT64(L, OSSecondsToTicks(value->value));
        }

        return 1;
    }

    int32_t Luaroma__GetSystemTime(lua_State *L) {
        PUSH_LUA_UINT64(L, OSGetSystemTime());
        return 1;
    }

    // ───────────── Thread ─────────────

    int32_t Luaroma__OSSleepTicks(lua_State *L) {
        lua_uint64 *ticks = (lua_uint64 *) GET_LUA_UINT64(L, 1);
        OSSleepTicks(ticks->value);
        return 0;
    }

    // ───────────── Debug ──────────────

    int32_t Luaroma__OSFatal(lua_State *L) {
        OSFatal(luaL_checkstring(L, 1));
        return 0;
    }

    // ────────── Filesystem ────────────

    int32_t Luaroma__FileExists(lua_State *L) {
        const char *path = luaL_checkstring(L, 1);
        int32_t result   = FSUtils::CheckFile(path);
        if (result < 0) {
            OSFatal("FileExists(): FSUtils::CheckFile() failed");
        }
        lua_pushboolean(L, result == 1);
        return 1;
    }

    int32_t Luaroma__CreateDirectory(lua_State *L) {
        const char *path = luaL_checkstring(L, 1);
        int32_t result   = FSUtils::CreateSubfolder(path);
        if (result < 0) {
            OSFatal("CreateDirectory(): FSUtils::CreateSubfolder() failed");
        }
        lua_pushboolean(L, result == 1);
        return 1;
    }

    int32_t Luaroma__CopyFile(lua_State *L) {
        const char *src  = luaL_checkstring(L, 1);
        const char *dest = luaL_checkstring(L, 2);
        bool result      = FSUtils::copyFile(src, dest);
        if (!result) { OSFatal("CopyFile(): FSUtils::copyFile() failed"); }
        lua_pushboolean(L, result);
        return 1;
    }

    int32_t Luaroma__WriteToFile(lua_State *L) {
        const char *path = luaL_checkstring(L, 1);
        size_t length;
        const char *data = luaL_checklstring(L, 2, &length);

        int32_t result = FSUtils::saveBufferToFile(path, (void *) data,
                                                   (uint32_t) length);
        if (result < 0) {
            OSFatal("WriteToFile: FSUtils::saveBufferToFile() failed");
        }

        lua_pushboolean(L, result == 1);
        return 1;
    }

    int32_t Luaroma__ReadFile(lua_State *L) {
        const char *path = luaL_checkstring(L, 1);
        CFile file(path, CFile::ReadOnly);

        if (!file.isOpen()) { OSFatal("ReadFile(): Failed to open file"); }

        uint64_t size = file.size();
        if (size == 0 || size > 10 * 1024 * 1024) {
            OSFatal("ReadFile(): File is empty or too large (>10MB)");
        }

        std::string content(size, '\0');
        if (file.read((uint8_t *) content.data(), size) < 0) {
            OSFatal("ReadFile(): Failed to read file");
        }

        file.close();
        lua_pushlstring(L, content.c_str(), size);
        return 1;
    }

    // ──────────── Network ─────────────

    int32_t Luaroma__HttpGet(lua_State *L) {
        const char *url = luaL_checkstring(L, 1);
        CURL *curl      = curl_easy_init();
        if (!curl) OSFatal("HttpGet(): curl_easy_init() failed");

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         CurlCallbackWriteToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            OSFatal(("HttpGet() failed: " +
                     std::string(curl_easy_strerror(res)))
                            .c_str());
        }

        curl_easy_cleanup(curl);
        lua_pushlstring(L, response.c_str(), response.size());
        return 1;
    }

    int32_t Luaroma__HttpPost(lua_State *L) {
        const char *url  = luaL_checkstring(L, 1);
        const char *body = luaL_checkstring(L, 2);

        CURL *curl = curl_easy_init();
        if (!curl) OSFatal("HttpPost(): curl_easy_init() failed");

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         CurlCallbackWriteToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            OSFatal(("HttpPost() failed: " +
                     std::string(curl_easy_strerror(res)))
                            .c_str());
        }

        curl_easy_cleanup(curl);
        lua_pushlstring(L, response.c_str(), response.size());
        return 1;
    }

    int32_t Luaroma__DownloadToFile(lua_State *L) {
        const char *url  = luaL_checkstring(L, 1);
        const char *path = luaL_checkstring(L, 2);

        CFile file(path, CFile::WriteOnly);
        if (!file.isOpen()) {
            OSFatal(("DownloadToFile(): Failed to open file: " +
                     std::string(path))
                            .c_str());
        }

        CURL *curl = curl_easy_init();
        if (!curl) {
            file.close();
            OSFatal("DownloadToFile(): curl_easy_init failed");
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlCallbackWriteToFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        file.close();

        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            OSFatal(("DownloadToFile() failed: " +
                     std::string(curl_easy_strerror(res)))
                            .c_str());
        }

        curl_easy_cleanup(curl);
        lua_pushboolean(L, 1);
        return 1;
    }

    // ────────────── WHB ───────────────

    int32_t Luaroma__WHBProcInit(lua_State *L) {
        WHBProcInit();
        return 0;
    }

    int32_t Luaroma__WHBProcShutdown(lua_State *L) {
        WHBProcShutdown();
        return 0;
    }

    int32_t Luaroma__WHBProcIsRunning(lua_State *L) {
        lua_pushboolean(L, WHBProcIsRunning());
        return 1;
    }

    ////////////////////////////////////////////////////////////////////
    void registerAll(lua_State *L) {
        lua_newtable(L);

        ADD_LUA_FUNC(Luaroma__Write, "Write");
        ADD_LUA_FUNC(Luaroma__Print, "Print");

        ADD_LUA_FUNC(Luaroma__KernelMemcpy, "KernelMemcpy");
        ADD_LUA_FUNC(Luaroma__KernelMemset, "KernelMemset");
        ADD_LUA_FUNC(Luaroma__Peek8, "Peek8");
        ADD_LUA_FUNC(Luaroma__Peek16, "Peek16");
        ADD_LUA_FUNC(Luaroma__Peek32, "Peek32");
        ADD_LUA_FUNC(Luaroma__Poke8, "Poke8");
        ADD_LUA_FUNC(Luaroma__Poke16, "Poke16");
        ADD_LUA_FUNC(Luaroma__Poke32, "Poke32");

        ADD_LUA_FUNC(Luaroma__VPadRead, "VPadRead");

        ADD_LUA_FUNC(Luaroma__TimeToTicks, "TimeToTicks");
        ADD_LUA_FUNC(Luaroma__GetSystemTime, "GetSystemTime");

        ADD_LUA_FUNC(Luaroma__OSSleepTicks, "OSSleepTicks");

        ADD_LUA_FUNC(Luaroma__OSFatal, "OSFatal");

        ADD_LUA_FUNC(Luaroma__FileExists, "FileExists");
        ADD_LUA_FUNC(Luaroma__CreateDirectory, "CreateDirectory");
        ADD_LUA_FUNC(Luaroma__CopyFile, "CopyFile");
        ADD_LUA_FUNC(Luaroma__WriteToFile, "WriteToFile");
        ADD_LUA_FUNC(Luaroma__ReadFile, "ReadFile");

        ADD_LUA_FUNC(Luaroma__HttpGet, "HttpGet");
        ADD_LUA_FUNC(Luaroma__HttpPost, "HttpPost");
        ADD_LUA_FUNC(Luaroma__DownloadToFile, "DownloadToFile");

        ADD_LUA_FUNC(Luaroma__WHBProcInit, "WHBProcInit");
        ADD_LUA_FUNC(Luaroma__WHBProcShutdown, "WHBProcShutdown");
        ADD_LUA_FUNC(Luaroma__WHBProcIsRunning, "WHBProcIsRunning");

        ADD_LUA_TABLE(registerVPadButton(L), "VPadButton");
        ADD_LUA_TABLE(registerVPadStick(L), "VPadStick");

        ADD_LUA_TABLE(registerTimeFormat(L), "TimeFormat");

        lua_setglobal(L, "Luaroma");

        registerUint64(L);
    }
} // namespace LuaromaFuncs