//------------------------------------------------------------------------------
//  win32memstack.h
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "memory/win32/win32memstack.h"
#include "memory/win32/win32memory.h"
#include "math/scalar.h"

namespace Memory
{
MemoryStack::TaggedMemory* MemoryStack::UnusedChunks = NULL;

//------------------------------------------------------------------------------
/**
	Initialize this memory stack.
*/
void 
MemoryStack::Init(size_t InDefaultChunkSize)
{
	DefaultChunkSize = InDefaultChunkSize;
	TopChunk		 = NULL;
	End				 = NULL;
	Top				 = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryStack::Tick()
{
	n_assert(TopChunk == NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryStack::Exit()
{
	Tick();
	while (UnusedChunks)
	{
		void* Old = UnusedChunks;
		UnusedChunks = UnusedChunks->Next;
		Memory::Free(Old);
	}
}

//------------------------------------------------------------------------------
/**
*/
size_t
MemoryStack::GetByteCount()
{
	size_t Count = 0;
	for (TaggedMemory* Chunk=TopChunk; Chunk; Chunk=Chunk->Next)
	{
		if (Chunk!=TopChunk)
			Count += Chunk->DataSize;
		else
			Count += Top - Chunk->Data;
	}
	return Count;
}

//------------------------------------------------------------------------------
/**
*/
BYTE*
MemoryStack::AllocateNewChunk(size_t MinSize)
{
	TaggedMemory* Chunk=NULL;
	for (TaggedMemory** Link=&UnusedChunks; *Link; Link=&(*Link)->Next)
	{
		// Find existing chunk.
		if ((*Link)->DataSize >= MinSize)
		{
			Chunk = *Link;
			*Link = (*Link)->Next;
			break;
		}
	}
	if (!Chunk)
	{
		// Create new chunk
		size_t DataSize	= n_max(MinSize, DefaultChunkSize-(INT)sizeof(TaggedMemory));
		Chunk			= (TaggedMemory*)Memory::Alloc(DataSize + sizeof(TaggedMemory));
		Chunk->DataSize = DataSize;
	}
	Chunk->Next = TopChunk;
	TopChunk	= Chunk;
	Top			= Chunk->Data;
	End			= Top + Chunk->DataSize;
	return Top;
}

//------------------------------------------------------------------------------
/**
*/
void
MemoryStack::FreeChunks(TaggedMemory* NewTopChunk)
{
	while (TopChunk != NewTopChunk)
	{
		TaggedMemory* RemoveChunk = TopChunk;
		TopChunk				  = TopChunk->Next;
		RemoveChunk				  = UnusedChunks;
		UnusedChunks			  = RemoveChunk;
	}
	Top = NULL;
	End = NULL;
	if (TopChunk)
	{
		Top	= TopChunk->Data;
		End = Top + TopChunk->DataSize;
	}
}

} // namespace Memory
