#pragma once
#ifndef MEMORY_WIN32MEMORY_H
#define MEMORY_WIN32MEMORY_H
//------------------------------------------------------------------------------
/**
    @file memory/win32/win32memory.h

    Low level memory functions for the Win32 platform.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/config.h"
#include "core/debug.h"
#include "threading/interlocked.h"
#include "memory/win32/win32mm.h"
#include "memory/win32/win32memorypool.h"

namespace Memory
{

extern HANDLE volatile Win32ProcessHeap;
#if NEBULA3_MEMORY_STATS
	extern int volatile AllocCount;
	extern int volatile AllocSize;
#endif

#if NEBULA3_MEMORYPOOL
    extern Win32::MemoryPool *MemMalloc;
#endif

__forceinline void
CreateAllocator()
{
#if NEBULA3_MEMORYPOOL
	if (!MemMalloc)
	{
		MemMalloc = new Win32::MemoryPool;
		MemMalloc->Init();
	}
#endif
}

//------------------------------------------------------------------------------
/**
    Allocate a block of memory from the process heap.
*/
__forceinline void*
Alloc(size_t size)
{
#if NEBULA3_MEMORYPOOL
	n_assert(MemMalloc);
	return MemMalloc->Malloc(size);
#else
	if (0 == Win32ProcessHeap)
	{
		Win32ProcessHeap = GetProcessHeap();
	}
#if NEBULA3_MEMORY_STATS
	Threading::Interlocked::Increment(AllocCount);
	Threading::Interlocked::Add(AllocSize, int(size));
#endif
	return HeapAlloc(Win32ProcessHeap, HEAP_GENERATE_EXCEPTIONS, size);
#endif
}

//------------------------------------------------------------------------------
/**
    Reallocate a block of memory.
*/
__forceinline void*
Realloc(void* ptr, size_t size)
{
#if NEBULA3_MEMORYPOOL
	n_assert(MemMalloc);
	return MemMalloc->Realloc(ptr, size);
#else
	n_assert(0 != Win32ProcessHeap);
#if NEBULA3_MEMORY_STATS
	size_t curSize = HeapSize(Win32ProcessHeap, 0, ptr);
	Threading::Interlocked::Add(AllocSize, int(size - curSize));
#endif
	return HeapReAlloc(Win32ProcessHeap, HEAP_GENERATE_EXCEPTIONS, ptr, size);
#endif
}

//------------------------------------------------------------------------------
/**
    Free a chunk of memory from the process heap.
*/
__forceinline void
Free(void* ptr)
{
#if NEBULA3_MEMORYPOOL
	n_assert(MemMalloc);
	return MemMalloc->Free(ptr);
#else
	n_assert(0 != ptr);
	n_assert(0 != Win32ProcessHeap);
#if NEBULA3_MEMORY_STATS
	n_assert(0 != ptr);
	size_t size = HeapSize(Win32ProcessHeap, 0, ptr);
	Threading::Interlocked::Add(AllocSize, -int(size));
	Threading::Interlocked::Decrement(AllocCount);
#endif
	HeapFree(Win32ProcessHeap, 0, ptr);
#endif
}

//------------------------------------------------------------------------------
/**
Duplicate a 0-terminated string.
*/
__forceinline char*
DuplicateCString(const char* from)
{
	n_assert(0 != from);
	size_t len = (unsigned int) strlen(from) + 1;
	char* to = (char*) Memory::Alloc(len);
	Memory::Copy((void*)from, to, len);
	return to;
}

//------------------------------------------------------------------------------
/**
    Debug function which validates the process heap. This will NOT check
    local heaps (call Heap::ValidateAllHeaps() for this). 
    Stops the program if something is wrong. 
*/
#if NEBULA3_MEMORY_STATS
extern bool Validate();
#endif
} // namespace Memory

#ifdef new
#undef new
#endif

#ifdef delete
#undef delete
#endif

//------------------------------------------------------------------------------
/**
    Replacement global new operator.
*/
__forceinline void*
__cdecl operator new(size_t size)
{
    return Memory::Alloc(size);
}

//------------------------------------------------------------------------------
/**
    Replacement global new[] operator.
*/
__forceinline void*
__cdecl operator new[](size_t size)
{
    return Memory::Alloc(size);
}

//------------------------------------------------------------------------------
/**
    Replacement global delete operator.
*/
__forceinline void
__cdecl operator delete(void* p)
{
    Memory::Free(p);
}

//------------------------------------------------------------------------------
/**
    Replacement global delete[] operator.
*/
__forceinline void
__cdecl operator delete[](void* p)
{
    Memory::Free(p);
}


#define n_new(type) new type
#define n_new_array(type,size) new type[size]
#define n_delete(ptr) delete ptr
#define n_delete_array(ptr) delete[] ptr
//------------------------------------------------------------------------------
#endif
    