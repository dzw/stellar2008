#pragma once
#ifndef WIN32_WIN32MEMSTACK_H
#define WIN32_WIN32MEMSTACK_H
//------------------------------------------------------------------------------
/**
    ¶ÑÕ»¹ÜÀí

	2008 cTuo
*/
#include "core/debug.h "
#include "memory/win32/win32mm.h"

namespace Memory
{
// Default boundary to align memory allocations on.
enum {DEFAULT_ALIGNMENT = 8};

// Enums for specifying memory allocation type.
enum EMemZeroed {MEM_Zeroed=1};
enum EMemOned   {MEM_Oned  =1};

/*-----------------------------------------------------------------------------
MemoryStack.
-----------------------------------------------------------------------------*/

//
// Simple linear-allocation memory stack.
// Items are allocated via PushBytes() or the specialized operator new()s.
// Items are freed en masse by using MemoryMark to Pop() them.
//
class MemoryStack
{
public:
	// Get bytes.
	BYTE* PushBytes( size_t AllocSize, size_t Align )
	{
		// Debug checks.
		n_assert(AllocSize>=0);
		n_assert((Align&(Align-1))==0);
		n_assert(Top<=End);

		// Try to get memory from the current chunk.
		BYTE* Result = (BYTE *)(((INT)Top+(Align-1))&~(Align-1));
		Top = Result + AllocSize;

		// Make sure we didn't overflow.
		if( Top > End )
		{
			// We'd pass the end of the current chunk, so allocate a new one.
			AllocateNewChunk( AllocSize + Align );
			Result = (BYTE *)(((INT)Top+(Align-1))&~(Align-1));
			Top    = Result + AllocSize;
		}
		return Result;
	}

	// Main functions.
	void Init( size_t DefaultChunkSize );
	void Exit();
	void Tick();
	size_t GetByteCount();

	// Friends.
	friend class MemoryMark;
	friend void* operator new( size_t Size, MemoryStack& Mem, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT );
	friend void* operator new( size_t Size, MemoryStack& Mem, EMemZeroed Tag, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT );
	friend void* operator new( size_t Size, MemoryStack& Mem, EMemOned Tag, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT );
	friend void* operator new[]( size_t Size, MemoryStack& Mem, INT Count=1, size_t Align=DEFAULT_ALIGNMENT );
	friend void* operator new[]( size_t Size, MemoryStack& Mem, EMemZeroed Tag, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT );
	friend void* operator new[]( size_t Size, MemoryStack& Mem, EMemOned Tag, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT );

	// Types.
	struct TaggedMemory
	{
		TaggedMemory* Next;
		size_t DataSize;
		BYTE Data[1];
	};

private:
	// Constants.
	enum {MAX_CHUNKS=1024};

	// Variables.
	BYTE*			Top;				// Top of current chunk (Top<=End).
	BYTE*			End;				// End of current chunk.
	size_t				DefaultChunkSize;	// Maximum chunk size to allocate.
	TaggedMemory*	TopChunk;			// Only chunks 0..ActiveChunks-1 are valid.

	// Static.
	static TaggedMemory* UnusedChunks;

	// Functions.
	BYTE* AllocateNewChunk( size_t MinSize );
	void FreeChunks( TaggedMemory* NewTopChunk );
};



/*-----------------------------------------------------------------------------
MemoryMark.
-----------------------------------------------------------------------------*/

//
// MemoryMark marks a top-of-stack position in the memory stack.
// When the marker is constructed or initialized with a particular memory 
// stack, it saves the stack's current position. When marker is popped, it
// pops all items that were added to the stack subsequent to initialization.
//
class MemoryMark
{
public:
	// Constructors.
	MemoryMark()
	{}
	MemoryMark( MemoryStack& InMem )
	{
		Mem          = &InMem;
		Top          = Mem->Top;
		SavedChunk   = Mem->TopChunk;
	}

	~MemoryMark()
	{
		Pop();
	}

	// MemoryMark interface.
	void Pop()
	{
		// Check state.

		// Unlock any new chunks that were allocated.
		if( SavedChunk != Mem->TopChunk )
			Mem->FreeChunks( SavedChunk );

		// Restore the memory stack's state.
		Mem->Top = Top;
	}

private:
	// Implementation variables.
	MemoryStack* Mem;
	BYTE* Top;
	MemoryStack::TaggedMemory* SavedChunk;
};

} // namespace Win32
//------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
MemoryStack templates.
-----------------------------------------------------------------------------*/

// Operator new for typesafe memory stack allocation.
template <class T> inline T* New( Memory::MemoryStack& Mem, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT )
{
	return (T*)Mem.PushBytes( Count*sizeof(T), Align );
}
template <class T> inline T* NewZeroed( Memory::MemoryStack& Mem, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT )
{
	BYTE* Result = Mem.PushBytes( Count*sizeof(T), Align );
	Memory::Clear( Result, Count*sizeof(T) );
	return (T*)Result;
}
template <class T> inline T* NewOned( Memory::MemoryStack& Mem, size_t Count=1, size_t Align=DEFAULT_ALIGNMENT )
{
	BYTE* Result = Mem.PushBytes( Count*sizeof(T), Align );
	Memory::Fill( Result, 0xff, Count*sizeof(T) );
	return (T*)Result;
}

/*-----------------------------------------------------------------------------
MemoryStack operator new's.
-----------------------------------------------------------------------------*/
// Operator new for typesafe memory stack allocation.
inline void* operator new( size_t Size, Memory::MemoryStack& Mem, size_t Count, size_t Align )
{
	// Get uninitialized memory.
	return Mem.PushBytes( Size*Count, Align );
}
inline void* operator new( size_t Size, Memory::MemoryStack& Mem, Memory::EMemZeroed Tag, size_t Count, size_t Align )
{
	// Get zero-filled memory.
	BYTE* Result = Mem.PushBytes( Size*Count, Align );
	Memory::Clear( Result, Size*Count );
	return Result;
}
inline void* operator new( size_t Size, Memory::MemoryStack& Mem, Memory::EMemOned Tag, size_t Count, size_t Align )
{
	// Get one-filled memory.
	BYTE* Result = Mem.PushBytes( Size*Count, Align );
	Memory::Fill( Result, 0xff, Size*Count );
	return Result;
}
inline void* operator new[]( size_t Size, Memory::MemoryStack& Mem, size_t Count, size_t Align )
{
	// Get uninitialized memory.
	return Mem.PushBytes( Size*Count, Align );
}
inline void* operator new[]( size_t Size, Memory::MemoryStack& Mem, Memory::EMemZeroed Tag, size_t Count, size_t Align )
{
	// Get zero-filled memory.
	BYTE* Result = Mem.PushBytes( Size*Count, Align );
	Memory::Clear( Result, Size*Count );
	return Result;
}
inline void* operator new[]( size_t Size, Memory::MemoryStack& Mem, Memory::EMemOned Tag, size_t Count, size_t Align )
{
	// Get one-filled memory.
	BYTE* Result = Mem.PushBytes( Size*Count, Align );
	Memory::Fill( Result, 0xff, Size*Count );
	return Result;
}

#endif
