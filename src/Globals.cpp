#include "Globals.h"

std::optional<std::string> gLicenseStr = std::nullopt;
NnAccountInfo gNnAccountInfo{};

lua_State *gL = nullptr;

CThread *gScriptThread = nullptr;