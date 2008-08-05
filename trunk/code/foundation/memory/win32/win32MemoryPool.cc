//------------------------------------------------------------------------------
//  win32memory.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "memory/win32/win32memorypool.h"
#include "timing/time.h"
#include "math/scalar.h"
#include "threading/scopelock.h"

namespace Win32
{
using namespace Math;
using namespace Timing;
using namespace Threading;

DWORD gPageSize = 4096;

MemoryPool::MemoryPool()
	:	MemInit( 0 )
	,	OsCurrent		( 0 )
	,	OsPeak			( 0 )
	,	UsedCurrent		( 0 )
	,	UsedPeak		( 0 )
	,	CurrentAllocs	( 0 )
	,	TotalAllocs		( 0 )
	,	MemTime			( 0.0 )
	{}

// Implementation.
void 
MemoryPool::OutOfMemory()
{
	n_error("OutOfMemory");
}

MemoryPool::FPoolInfo* 
MemoryPool::CreateIndirect()
{
	FPoolInfo* Indirect = (FPoolInfo*)VirtualAlloc( NULL, 256*sizeof(FPoolInfo), MEM_COMMIT, PAGE_READWRITE );
	if( !Indirect )
		OutOfMemory();
	return Indirect;
}

void* 
MemoryPool::Malloc( size_t Size )
{
	ScopeLock slock(&memLock);

	n_assert(Size>=0);
	n_assert(MemInit);
	MEM_TIME(MemTime -= n_Seconds());
	STAT(CurrentAllocs++);
	STAT(TotalAllocs++);
	FFreeMem* Free;
	if( Size<POOL_MAX )
	{
		// Allocate from pool.
		FPoolTable* Table = MemSizeToPoolTable[Size];
		n_assert(Size<=Table->BlockSize);
		FPoolInfo* Pool = Table->FirstPool;
		if( !Pool )
		{
			// Must create a new pool.
			DWORD Blocks  = 65536 / Table->BlockSize;
			DWORD Bytes   = Blocks * Table->BlockSize;
			n_assert(Blocks>=1);
			n_assert(Blocks*Table->BlockSize<=Bytes);

			// Allocate memory.
			Free = (FFreeMem*)VirtualAlloc( NULL, Bytes, MEM_COMMIT, PAGE_READWRITE );
			if( !Free )
				OutOfMemory();

			// Create pool in the indirect table.
			FPoolInfo*& Indirect = PoolIndirect[((DWORD)Free>>24)];
			if( !Indirect )
				Indirect = CreateIndirect();
			Pool = &Indirect[((DWORD)Free>>16)&255];

			// Init pool.
			Pool->Link( Table->FirstPool );
			Pool->Mem            = (BYTE*)Free;
			Pool->Bytes		     = Bytes;
            Pool->OsBytes		 = Math::n_iAlign(Bytes,gPageSize);
			STAT(OsPeak = n_max(OsPeak,OsCurrent+=Pool->OsBytes));
			Pool->Table		     = Table;
			Pool->Taken			 = 0;
			Pool->FirstMem       = Free;

			// Create first free item.
			Free->Blocks         = Blocks;
			Free->Next           = NULL;
		}

		// Pick first available block and unlink it.
		Pool->Taken++;
		n_assert(Pool->FirstMem);
		n_assert(Pool->FirstMem->Blocks>0);
		Free = (FFreeMem*)((BYTE*)Pool->FirstMem + --Pool->FirstMem->Blocks * Table->BlockSize);
		if( Pool->FirstMem->Blocks==0 )
		{
			Pool->FirstMem = Pool->FirstMem->Next;
			if( !Pool->FirstMem )
			{
				// Move to exausted list.
				Pool->Unlink();
				Pool->Link( Table->ExaustedPool );
			}
		}
		STAT(UsedPeak = n_max(UsedPeak,UsedCurrent+=Table->BlockSize));
	}
	else
	{
		// Use OS for large allocations.
		INT AlignedSize = n_iAlign(Size,gPageSize);
		Free = (FFreeMem*)VirtualAlloc( NULL, AlignedSize, MEM_COMMIT, PAGE_READWRITE );
		if( !Free )
			OutOfMemory();
		n_assert(!((SIZE_T)Free&65535));

		// Create indirect.
		FPoolInfo*& Indirect = PoolIndirect[((DWORD)Free>>24)];
		if( !Indirect )
			Indirect = CreateIndirect();

		// Init pool.
		FPoolInfo* Pool = &Indirect[((DWORD)Free>>16)&255];
		Pool->Mem		= (BYTE*)Free;
		Pool->Bytes		= Size;
		Pool->OsBytes	= AlignedSize;
		Pool->Table		= &OsTable;
		STAT(OsPeak   = n_max(OsPeak,  OsCurrent+=AlignedSize));
		STAT(UsedPeak = n_max(UsedPeak,UsedCurrent+=Size));
	}
    MEM_TIME(MemTime += n_Seconds());
	return Free;
}

void* 
MemoryPool::Realloc( void* Ptr, size_t NewSize )
{
	ScopeLock slock(&memLock);

	n_assert(MemInit);
    MEM_TIME(MemTime -= n_Seconds());
	n_assert(NewSize>=0);
	void* NewPtr = Ptr;
	if( Ptr && NewSize )
	{
		n_assert(MemInit);
		FPoolInfo* Pool = &PoolIndirect[(DWORD)Ptr>>24][((DWORD)Ptr>>16)&255];
		if( Pool->Table!=&OsTable )
		{
			// Allocated from pool, so grow or shrink if necessary.
			if( NewSize>Pool->Table->BlockSize || MemSizeToPoolTable[NewSize]!=Pool->Table )
			{
				NewPtr = Malloc( NewSize );
				Memory::Copy( Ptr, NewPtr, n_min(NewSize,Pool->Table->BlockSize) );
				Free( Ptr );
			}
		}
		else
		{
			// Allocated from OS.
			n_assert(!((INT)Ptr&65535));
			if( NewSize>Pool->OsBytes || NewSize*3<Pool->OsBytes*2 )
			{
				// Grow or shrink.
				NewPtr = Malloc( NewSize );
				Memory::Copy( Ptr, NewPtr, n_min(NewSize,Pool->Bytes) );
				Free( Ptr );
			}
			else
			{
				// Keep as-is, reallocation isn't worth the overhead.
				Pool->Bytes = NewSize;
			}
		}
	}
	else if( Ptr == NULL )
	{
		NewPtr = Malloc( NewSize );
	}
	else
	{
		Free( Ptr );
		NewPtr = NULL;
	}
    MEM_TIME(MemTime += n_Seconds());
	return NewPtr;
}

void 
MemoryPool::Free( void* Ptr )
{
	ScopeLock slock(&memLock);

	if( !Ptr )
		return;
	n_assert(MemInit);
    MEM_TIME(MemTime -= n_Seconds());
	STAT(CurrentAllocs--);

	// Windows version.
	FPoolInfo* Pool = &PoolIndirect[(DWORD)Ptr>>24][((DWORD)Ptr>>16)&255];
	n_assert(Pool->Bytes!=0);
	if( Pool->Table!=&OsTable )
	{
		// If this pool was exausted, move to available list.
		if( !Pool->FirstMem )
		{
			Pool->Unlink();
			Pool->Link( Pool->Table->FirstPool );
		}

		// Free a pooled allocation.
		FFreeMem* Free		= (FFreeMem *)Ptr;
		Free->Blocks		= 1;
		Free->Next			= Pool->FirstMem;
		Pool->FirstMem		= Free;
		STAT(UsedCurrent   -= Pool->Table->BlockSize);

		// Free this pool.
		n_assert(Pool->Taken>=1);
		if( --Pool->Taken == 0 )
		{
			// Free the OS memory.
			Pool->Unlink();
			n_verify( VirtualFree( Pool->Mem, 0, MEM_RELEASE )!=0 );
			STAT(OsCurrent -= Pool->OsBytes);
		}
	}
	else
	{
		// Free an OS allocation.
		n_assert(!((INT)Ptr&65535));
		STAT(UsedCurrent -= Pool->Bytes);
		STAT(OsCurrent   -= Pool->OsBytes);
		n_verify( VirtualFree( Ptr, 0, MEM_RELEASE )!=0 );
	}
    MEM_TIME(MemTime += n_Seconds());
}

/**
 * Gathers memory allocations for both virtual and physical allocations.
 *
 * @param Virtual	[out] size of virtual allocations
 * @param Physical	[out] size of physical allocations	
 */
void 
MemoryPool::GetAllocationInfo( SIZE_T& Virtual, SIZE_T& Physical )
{
	Virtual		= OsCurrent;
	Physical	= 0;
}

void 
MemoryPool::DumpAllocs(bool bSummaryOnly)
{
	ScopeLock slock(&memLock);

	MemoryPool::HeapCheck();

	STAT(n_dbgout( TEXT("Memory Allocation Status") ));
	STAT(n_dbgout( TEXT("Curr Memory % 5.3fM / % 5.3fM"), UsedCurrent/1024.0/1024.0, OsCurrent/1024.0/1024.0 ));
	STAT(n_dbgout( TEXT("Peak Memory % 5.3fM / % 5.3fM"), UsedPeak   /1024.0/1024.0, OsPeak   /1024.0/1024.0 ));
	STAT(n_dbgout( TEXT("Allocs      % 6i Current / % 6i Total"), CurrentAllocs, TotalAllocs ));
	MEM_TIME(n_dbgout( "Seconds     % 5.3f", MemTime ));
	MEM_TIME(n_dbgout( "MSec/Allc   % 5.5f", 1000.0 * MemTime / MemAllocs ));

#if NEBULA3_MEMORY_STATS
	if( !bSummaryOnly )
	{
		n_dbgout( TEXT("Block Size Num Pools Cur Allocs Total Allocs Mem Used Mem Waste Efficiency") );
		n_dbgout( TEXT("---------- --------- ---------- ------------ -------- --------- ----------") );
		INT TotalPoolCount  = 0;
		INT TotalAllocCount = 0;
		INT TotalMemUsed    = 0;
		INT TotalMemWaste   = 0;
		for( INT i=0; i<POOL_COUNT; i++ )
		{
			FPoolTable* Table = &PoolTable[i];
			INT PoolCount=0;
			INT AllocCount=0;
			INT MemUsed=0;
			for( INT i=0; i<2; i++ )
			{
				for( FPoolInfo* Pool=(i?Table->FirstPool:Table->ExaustedPool); Pool; Pool=Pool->Next )
				{
					PoolCount++;
					AllocCount += Pool->Taken;
					MemUsed += Pool->Bytes;
					INT FreeCount=0;
					for( FFreeMem* Free=Pool->FirstMem; Free; Free=Free->Next )
						FreeCount += Free->Blocks;
				}
			}
			INT MemWaste = MemUsed - AllocCount*Table->BlockSize;
			n_dbgout
			(
				TEXT("% 10i % 9i % 10i % 11iK % 7iK % 8iK % 9.2f%%"),
				Table->BlockSize,
				PoolCount,
				AllocCount,
				0,
				MemUsed /1024,
				MemWaste/1024,
				MemUsed ? 100.0 * MemUsed / (MemUsed+MemWaste) : 100.0
			);
			TotalPoolCount  += PoolCount;
			TotalAllocCount += AllocCount;
			TotalMemUsed    += MemUsed;
			TotalMemWaste   += MemWaste;
		}
		n_dbgout
		(
			TEXT("BlkOverall % 9i % 10i % 11iK % 7iK % 8iK % 9.2f%%"),
			TotalPoolCount,
			TotalAllocCount,
			0,
			TotalMemUsed /1024,
			TotalMemWaste/1024,
			TotalMemUsed ? 100.0 * TotalMemUsed / (TotalMemUsed+TotalMemWaste) : 100.0
		);
	}
#endif
}

void 
MemoryPool::HeapCheck()
{
	for( INT i=0; i<POOL_COUNT; i++ )
	{
		FPoolTable* Table = &PoolTable[i];
		for( FPoolInfo** PoolPtr=&Table->FirstPool; *PoolPtr; PoolPtr=&(*PoolPtr)->Next )
		{
			FPoolInfo* Pool=*PoolPtr;
			n_assert(Pool->PrevLink==PoolPtr);
			n_assert(Pool->FirstMem);
			for( FFreeMem* Free=Pool->FirstMem; Free; Free=Free->Next )
				n_assert(Free->Blocks>0);
		}
		for( FPoolInfo** PoolPtr=&Table->ExaustedPool; *PoolPtr; PoolPtr=&(*PoolPtr)->Next )
		{
			FPoolInfo* Pool=*PoolPtr;
			n_assert(Pool->PrevLink==PoolPtr);
			n_assert(!Pool->FirstMem);
		}
	}
}

void 
MemoryPool::Init()
{
	ScopeLock slock(&memLock);

	n_assert(!MemInit);
	MemInit = 1;

	// Get OS page size.
#if __WIN32__
	SYSTEM_INFO SI;
	GetSystemInfo( &SI );
	gPageSize = SI.dwPageSize;
	n_assert(!(gPageSize&(gPageSize-1)));
#else
	gPageSize = 4096;
#endif

	// Init tables.
	OsTable.FirstPool    = NULL;
	OsTable.ExaustedPool = NULL;
	OsTable.BlockSize    = 0;

	PoolTable[0].FirstPool    = NULL;
	PoolTable[0].ExaustedPool = NULL;
	PoolTable[0].BlockSize    = 8;
	for( DWORD i=1; i<6; i++ )
	{
		PoolTable[i].FirstPool    = NULL;
		PoolTable[i].ExaustedPool = NULL;
		PoolTable[i].BlockSize    = (8<<(i>>2)) + (2<<(i-1));
	}
	for( DWORD i=6; i<POOL_COUNT; i++ )
	{
		PoolTable[i].FirstPool    = NULL;
		PoolTable[i].ExaustedPool = NULL;
		PoolTable[i].BlockSize    = (4+((i+6)&3)) << (1+((i+6)>>2));
	}
	for( DWORD i=0; i<POOL_MAX; i++ )
	{
		DWORD Index;
		for( Index=0; PoolTable[Index].BlockSize<i; Index++ );
		n_assert(Index<POOL_COUNT);
		MemSizeToPoolTable[i] = &PoolTable[Index];
	}
	for( DWORD i=0; i<256; i++ )
	{
		PoolIndirect[i] = NULL;
	}
	n_assert(POOL_MAX-1==PoolTable[POOL_COUNT-1].BlockSize);
}

} // namespace Memory


