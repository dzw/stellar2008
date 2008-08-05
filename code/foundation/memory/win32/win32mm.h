#pragma once
#ifndef MEMORY_WIN32MM_H
#define MEMORY_WIN32MM_H
//------------------------------------------------------------------------------
/**
    @file memory/win32/win32memory.h

    Low level memory functions for the Win32 platform.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/config.h"
#include "core/debug.h"

namespace Memory
{
//------------------------------------------------------------------------------
/**
    Copy a chunk of memory (note the argument order is different 
    from memcpy()!!!)
*/
__forceinline void
Copy(const void* from, void* to, size_t numBytes)
{
    if (numBytes > 0)
    {
        n_assert(0 != from);
        n_assert(0 != to);
        n_assert(from != to);
        CopyMemory(to, from, numBytes);
    }
}

//------------------------------------------------------------------------------
/**
    Overwrite a chunk of memory with 0's.
*/
__forceinline void
Clear(void* ptr, size_t numBytes)
{
    ZeroMemory(ptr, numBytes);
}

//------------------------------------------------------------------------------
/**
*/
__forceinline void
Fill(void* ptr, int c, size_t numBytes)
{
	memset(ptr, c, numBytes);
}

//------------------------------------------------------------------------------
/**
    Get the system's total current memory, this does not only include
    Nebula3's memory allocations but the memory usage of the entire system.
*/
struct MemoryStatus
{
    unsigned int totalPhysical;
    unsigned int availPhysical;
    unsigned int totalVirtual;
    unsigned int availVirtual;
};

inline MemoryStatus
GetMemoryStatus()
{
    MEMORYSTATUS stats = { NULL };
    GlobalMemoryStatus(&stats);
    MemoryStatus result;
    result.totalPhysical = (unsigned int) stats.dwTotalPhys;
    result.availPhysical = (unsigned int) stats.dwAvailPhys;
    result.totalVirtual  = (unsigned int) stats.dwTotalVirtual;
    result.availVirtual  = (unsigned int) stats.dwAvailVirtual;
    return result;
}

} // namespace Memory

//------------------------------------------------------------------------------
#endif
    