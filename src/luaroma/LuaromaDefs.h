#pragma once

#include <string>

#include <utils/Types.h>

struct LuaromaStatus {
    uint8_t value;
    enum Error : int8_t {
        LUAROMA_OK               = 0,
        LUAROMA_FAILED           = -1,
        LUAROMA_SCRIPT_ERROR     = -2,
        LUAROMA_SCRIPT_NOT_FOUND = -3
    } error = LUAROMA_OK;
};

std::string errorToStr(LuaromaStatus::Error err);