/*
    oboskrnl/vfs/devManip/memcpy.h

    Copyright (c) 2023 Omar Berrow
*/

#pragma once

// This function contains a memcpy variant that uses vector instructions that are architecture-specific, 
// but is optional to support.

#include <int.h>
#include <export.h>

namespace obos
{
    namespace vfs
    {
        // If 64 byte vectorized instructions aren't supported on the platform, then you can use smaller
        // chunks.
        // Any states must be saved, as this could be used in syscalls, and the usermode process might
        // expect those states to be preserved.
        OBOS_WEAK void *_VectorizedMemcpy64B(void* dest, const void* src, size_t nBlocks);
    }
}