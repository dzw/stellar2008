#pragma once
#ifndef WIN32_MEMORYPOOL_H
#define WIN32_MEMORYPOOL_H
//------------------------------------------------------------------------------
/**
    cTuo
*/
#include "threading/criticalsection.h"
#include "memory/win32/win32mm.h"

#pragma warning( disable : 4311 )       // 指针截断
#pragma warning( disable : 4312 )       // 类型转换
#pragma warning( disable : 4367 )

namespace Win32
{
#define MEM_TIME(st)

#if NEBULA3_MEMORY_STATS
    #define STAT(x) x
#else
    #define STAT(x)
#endif


//
// Optimized Windows virtual memory allocator.
//
class MemoryPool
{
private:
	// Counts.
	enum {POOL_COUNT = 43     };
	enum {POOL_MAX   = 32768+1};

	// Forward declares.
	struct FFreeMem;
	struct FPoolTable;

	// Memory pool info. 32 bytes.
	struct FPoolInfo
	{
		DWORD	    Bytes;		// Bytes allocated for pool.
		DWORD		OsBytes;	// Bytes aligned to page size.
		DWORD       Taken;      // Number of allocated elements in this pool, when counts down to zero can free the entire pool.
		BYTE*       Mem;		// Memory base.
		FPoolTable* Table;		// Index of pool.
		FFreeMem*   FirstMem;   // Pointer to first free memory in this pool.
		FPoolInfo*	Next;
		FPoolInfo**	PrevLink;

		void Link( FPoolInfo*& Before )
		{
			if( Before )
				Before->PrevLink = &Next;
			Next     = Before;
			PrevLink = &Before;
			Before   = this;
		}
		void Unlink()
		{
			if( Next )
				Next->PrevLink = PrevLink;
			*PrevLink = Next;
		}
	};

	// Information about a piece of free memory. 8 bytes.
	struct FFreeMem
	{
		FFreeMem*	Next;		// Next or MemLastPool[], always in order by pool.
		DWORD		Blocks;		// Number of consecutive free blocks here, at least 1.
		FPoolInfo* GetPool()
		{
			return (FPoolInfo*)((INT)this & 0xffff0000);
		}
	};

	// Pool table.
	struct FPoolTable
	{
		FPoolInfo* FirstPool;
		FPoolInfo* ExaustedPool;
		DWORD      BlockSize;
	};

	// Variables.
	FPoolTable  PoolTable[POOL_COUNT], OsTable;
	FPoolInfo*	PoolIndirect[256];
	FPoolTable* MemSizeToPoolTable[POOL_MAX];
	INT         MemInit;
	INT			OsCurrent,OsPeak,UsedCurrent,UsedPeak,CurrentAllocs,TotalAllocs;
	DOUBLE		MemTime;
	Threading::CriticalSection memLock;

	// Implementation.
	void OutOfMemory();
	FPoolInfo* CreateIndirect();

public:
	// FMalloc interface.
	MemoryPool();
	
	void* operator new(size_t size);
	void operator delete(void* ptr);
	void* operator new[](size_t size);
	void operator delete[](void* ptr);

	void* Malloc( size_t Size );
	void* Realloc( void* Ptr, size_t NewSize );
	void Free( void* Ptr );

	/**
	 * Gathers memory allocations for both virtual and physical allocations.
	 *
	 * @param Virtual	[out] size of virtual allocations
	 * @param Physical	[out] size of physical allocations	
	 */
	void GetAllocationInfo( SIZE_T& Virtual, SIZE_T& Physical );
	void DumpAllocs(bool bSummaryOnly);
	void HeapCheck();
	void Init();
};

inline void* 
MemoryPool::operator new(size_t size)
{
	return malloc(size);
}

inline void 
MemoryPool::operator delete(void* ptr)
{
	free(ptr);
}

inline void* 
MemoryPool::operator new[](size_t size)
{
	return malloc(size);
}

inline void 
MemoryPool::operator delete[](void* ptr)
{
	free(ptr);
}

}
#endif

