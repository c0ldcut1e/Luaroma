#pragma once

#include <string>
#include <vector>

#include <luaroma/LuaromaDefs.h>
#include <utils/Types.h>

#include <lua.hpp>

class Luaroma {
public:
    static Luaroma &instance();

    LuaromaStatus init();
    void deinit();

    LuaromaStatus runScript(const uint8_t *buffer, size_t size,
                            std::string name = "memchunk");

    bool isInit() const { return mIsInit; }
    size_t scriptCount() const { return mScripts.size(); };

private:
    Luaroma()  = default;
    ~Luaroma() = default;

    Luaroma(const Luaroma &)            = delete;
    Luaroma &operator=(const Luaroma &) = delete;

    bool mIsInit;

    struct ScriptContext {
        lua_State *L;
        std::string name;
    };

    std::vector<ScriptContext> mScripts;
};