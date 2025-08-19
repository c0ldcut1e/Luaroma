#pragma once

#include "utils/Types.h"

#include <nn/act.h>

extern "C" int32_t MCP_GetDeviceId(int32_t mcpHandle, uint32_t *outDeviceId);

typedef struct {
    char nnid[nn::act::AccountIdSize];
    uint32_t persistentId;
    uint32_t principalId;
} NnAccountInfo;