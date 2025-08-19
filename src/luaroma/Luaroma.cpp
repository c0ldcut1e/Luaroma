#include "Luaroma.h"

#include <cstdio>

#include <Globals.h>
#include <luaroma/LuaromaFuncs.h>
#include <luaroma/scripts/LuaromaLibrary.h>
#include <utils/Logger.h>

Luaroma &Luaroma::instance() {
    static Luaroma self;
    return self;
}

LuaromaStatus Luaroma::init() {
    if (gL) return {0};

    gL = luaL_newstate();
    if (!gL) {
        DEBUG_FUNCTION_LINE_ERR("Failed to create lua state");

        return {1, LuaromaStatus::LUAROMA_FAILED};
    }

    luaL_openlibs(gL);
    LuaromaFuncs::registerAll(gL);

    mIsInit = true;

    return {2};
}

void Luaroma::deinit() {
    for (ScriptContext context : mScripts) lua_close(context.L);
    mScripts.clear();

    if (gL) {
        lua_close(gL);
        gL = nullptr;
    }

    mIsInit = false;
}

LuaromaStatus Luaroma::runScript(const uint8_t *buffer, size_t size,
                                 std::string name) {
    lua_State *L = luaL_newstate();
    if (!L) return {0, LuaromaStatus::LUAROMA_FAILED};

    WHBLogPrint("");
    DEBUG_FUNCTION_LINE("Created lua state for %s: %p", name.c_str(), L);

    luaL_openlibs(L);

    DEBUG_FUNCTION_LINE("Registering Luaroma Library communicators");
    LuaromaFuncs::registerAll(L);

    DEBUG_FUNCTION_LINE("Adding Luaroma Library to script %s", name.c_str());
    if (luaL_loadbuffer(L, (const char *) LuaromaLibraryData,
                        LuaromaLibraryLength, "library") ||
        lua_pcall(L, 0, 0, 0)) {
        char buffer[0xFF];
        snprintf(buffer, 0x100, "Luaroma Library load error: %s",
                 lua_tostring(L, -1));
        OSFatal(buffer);

        lua_close(L);
        return {1, LuaromaStatus::LUAROMA_SCRIPT_ERROR};
    }

    DEBUG_FUNCTION_LINE("Running script: %s/%d", name.c_str(), size);
    if (luaL_loadbuffer(L, (const char *) buffer, size, name.c_str()) ||
        lua_pcall(L, 0, 0, 0)) {
        char buffer[0xFF];
        snprintf(buffer, 0x100, "Luaroma script error (%s):\n\n%s",
                 name.c_str(), lua_tostring(L, -1));
        OSFatal(buffer);

        lua_close(L);

        return {2, LuaromaStatus::LUAROMA_SCRIPT_ERROR};
    }

    mScripts.push_back({L, name.c_str()});

    DEBUG_FUNCTION_LINE("Added script: %s", name.c_str());
    WHBLogPrint("");

    return {3};
}