#pragma once

#include <optional>
#include <string>

#include "WutExtra.h"
#include "system/CThread.h"

#include <lua.hpp>

extern std::optional<std::string> gLicenseStr;
extern NnAccountInfo gNnAccountInfo;

extern lua_State *gL;

extern CThread *gScriptThread;