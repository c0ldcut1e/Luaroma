#include "LuaromaDefs.h"

std::string errorToStr(LuaromaStatus::Error err) {
    switch (err) {
        case LuaromaStatus::LUAROMA_OK:
            return "OK";

        case LuaromaStatus::LUAROMA_FAILED:
            return "FAILED";

        case LuaromaStatus::LUAROMA_SCRIPT_ERROR:
            return "SCRIPT_ERROR";

        case LuaromaStatus::LUAROMA_SCRIPT_NOT_FOUND:
            return "SCRIPT_NOT_FOUND";

        default:
            break;
    }

    return "";
}