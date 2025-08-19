#pragma once

#include <cstring>

#include "Logger.h"
#include "Types.h"

#include <coreinit/cache.h>
#include <coreinit/memorymap.h>

#include <kernel/kernel.h>

inline void *KernelMemcpy(void *dest, void *src, size_t size) {
    DCFlushRange(src, size);
    ICInvalidateRange(src, size);

    uint8_t buffer[0x100];

    uint8_t *_dest = (uint8_t *) dest;
    uint8_t *_src  = (uint8_t *) src;

    size_t remaining = size;
    while (remaining > 0) {
        size_t copySize = remaining > 0x100 ? 0x100 : remaining;

        memcpy(buffer, _src, copySize);

        uint32_t physSrc  = OSEffectiveToPhysical((uint32_t) buffer);
        uint32_t physDest = OSEffectiveToPhysical((uint32_t) _dest);
        KernelCopyData(physDest, physSrc, copySize);

        _src += copySize;
        _dest += copySize;
        remaining -= copySize;
    }

    DCFlushRange(dest, size);
    ICInvalidateRange(dest, size);

    return dest;
}

inline void *KernelMemset(void *dest, uint8_t value, size_t size) {
    uint8_t buffer[0x100];
    memset(buffer, value, sizeof(buffer));

    uint8_t *_dest = (uint8_t *) dest;

    size_t remaining = size;
    while (remaining > 0) {
        size_t copySize = remaining > 0x100 ? 0x100 : remaining;

        uint32_t physSrc  = OSEffectiveToPhysical((uint32_t) buffer);
        uint32_t physDest = OSEffectiveToPhysical((uint32_t) _dest);
        KernelCopyData(physDest, physSrc, copySize);

        _dest += copySize;
        remaining -= copySize;
    }

    DCFlushRange(dest, size);
    ICInvalidateRange(dest, size);

    return dest;
}

inline uint8_t Peek8(uint32_t address) {
    uint32_t aligned = address & ~3;
    uint32_t buffer;

    DCFlushRange((void *) aligned, sizeof(uint32_t));
    ICInvalidateRange((void *) aligned, sizeof(uint32_t));

    uint32_t physSrc = OSEffectiveToPhysical(aligned);
    uint32_t physDst = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physDst, physSrc, sizeof(uint32_t));

    DCFlushRange(&buffer, sizeof(uint32_t));
    ICInvalidateRange(&buffer, sizeof(uint32_t));

    uint32_t shift = (address & 3) * 8;
    return (buffer >> shift) & 0xFF;
}

inline uint16_t Peek16(uint32_t address) {
    uint32_t aligned = address & ~3;
    uint32_t buffer;

    DCFlushRange((void *) aligned, sizeof(uint32_t));
    ICInvalidateRange((void *) aligned, sizeof(uint32_t));

    uint32_t physSrc = OSEffectiveToPhysical(aligned);
    uint32_t physDst = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physDst, physSrc, sizeof(uint32_t));

    DCFlushRange(&buffer, sizeof(uint32_t));
    ICInvalidateRange(&buffer, sizeof(uint32_t));

    uint32_t shift = (address & 2) * 8;
    return (buffer >> shift) & 0xFFFF;
}

inline uint32_t Peek32(uint32_t address) {
    uint32_t buffer;

    DCFlushRange((void *) address, sizeof(uint32_t));
    ICInvalidateRange((void *) address, sizeof(uint32_t));

    uint32_t physSrc = OSEffectiveToPhysical(address);
    uint32_t physDst = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physDst, physSrc, sizeof(uint32_t));

    DCFlushRange(&buffer, sizeof(uint32_t));
    ICInvalidateRange(&buffer, sizeof(uint32_t));

    return buffer;
}

inline void Poke8(uint32_t address, uint8_t value) {
    uint32_t aligned = address & ~3;
    uint32_t buffer;

    uint32_t physSrc = OSEffectiveToPhysical(aligned);
    uint32_t physTmp = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physTmp, physSrc, sizeof(uint32_t));

    uint32_t shift = (address & 3) * 8;
    buffer &= ~(0xFF << shift);
    buffer |= ((uint32_t) value << shift);

    DCFlushRange(&buffer, sizeof(uint32_t));
    ICInvalidateRange(&buffer, sizeof(uint32_t));

    uint32_t physDst = OSEffectiveToPhysical(aligned);
    uint32_t physBuf = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physDst, physBuf, sizeof(uint32_t));

    DCFlushRange((void *) aligned, sizeof(uint32_t));
    ICInvalidateRange((void *) aligned, sizeof(uint32_t));
}

inline void Poke16(uint32_t address, uint16_t value) {
    uint32_t aligned = address & ~3;
    uint32_t buffer;

    uint32_t physSrc = OSEffectiveToPhysical(aligned);
    uint32_t physTmp = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physTmp, physSrc, sizeof(uint32_t));

    uint32_t shift = (address & 2) * 8;
    buffer &= ~(0xFFFF << shift);
    buffer |= ((uint32_t) value << shift);

    DCFlushRange(&buffer, sizeof(uint32_t));
    ICInvalidateRange(&buffer, sizeof(uint32_t));

    uint32_t physDst = OSEffectiveToPhysical(aligned);
    uint32_t physBuf = OSEffectiveToPhysical((uint32_t) &buffer);
    KernelCopyData(physDst, physBuf, sizeof(uint32_t));

    DCFlushRange((void *) aligned, sizeof(uint32_t));
    ICInvalidateRange((void *) aligned, sizeof(uint32_t));
}

inline void Poke32(uint32_t address, uint32_t value) {
    uint32_t buffer = value;

    DCFlushRange(&buffer, sizeof(uint32_t));
    ICInvalidateRange(&buffer, sizeof(uint32_t));

    uint32_t physSrc = OSEffectiveToPhysical((uint32_t) &buffer);
    uint32_t physDst = OSEffectiveToPhysical(address);
    KernelCopyData(physDst, physSrc, sizeof(uint32_t));

    DCFlushRange((void *) address, sizeof(uint32_t));
    ICInvalidateRange((void *) address, sizeof(uint32_t));
}