#pragma once

#include <stdlib.h>

#include <utils/Types.h>

#include <lua.hpp>

#define GET_LUA_UINT64(L, arg)                                                 \
    (lua_uint64 *) luaL_checkudata(L, arg, "Luaroma.uint64")
#define PUSH_LUA_UINT64(L, val)                                                \
    do {                                                                       \
        lua_uint64 *u64 =                                                      \
                (lua_uint64 *) lua_newuserdata(L, sizeof(lua_uint64));         \
        u64->value = (val);                                                    \
        luaL_getmetatable(L, "Luaroma.uint64");                                \
        lua_setmetatable(L, -2);                                               \
    } while (0);

typedef struct {
    uint64_t value;
} lua_uint64;

int lua_uint64_new(lua_State *L) {
    const char *str = luaL_checkstring(L, 1);
    uint64_t *u64   = (uint64_t *) lua_newuserdata(L, sizeof(uint64_t));
    *u64            = strtoull(str, nullptr, 0);
    luaL_getmetatable(L, "Luaroma.uint64");
    lua_setmetatable(L, -2);
    return 1;
}

int lua_uint64_toHex(lua_State *L) {
    uint64_t *u64 = (uint64_t *) luaL_checkudata(L, 1, "Luaroma.uint64");
    char buf[20];
    snprintf(buf, sizeof(buf), "0x%016llX", *u64);
    lua_pushstring(L, buf);
    return 1;
}

int lua_uint64_tonumber(lua_State *L) {
    uint64_t *u64 = (uint64_t *) luaL_checkudata(L, 1, "Luaroma.uint64");
    lua_pushnumber(L, (lua_Number) (*u64));
    return 1;
}

int lua_uint64_eq(lua_State *L) {
    uint64_t *a = (uint64_t *) luaL_checkudata(L, 1, "Luaroma.uint64");
    uint64_t *b = (uint64_t *) luaL_checkudata(L, 2, "Luaroma.uint64");
    lua_pushboolean(L, *a == *b);
    return 1;
}

void registerUint64(lua_State *L) {
    luaL_newmetatable(L, "Luaroma.uint64");
    lua_pushcfunction(L, lua_uint64_toHex);
    lua_setfield(L, -2, "hex");
    lua_pushcfunction(L, lua_uint64_tonumber);
    lua_setfield(L, -2, "num");
    lua_pushcfunction(L, lua_uint64_eq);
    lua_setfield(L, -2, "__eq");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    lua_getglobal(L, "Luaroma");
    lua_pushcfunction(L, lua_uint64_new);
    lua_setfield(L, -2, "uint64");
    lua_pop(L, 1);
}