#pragma once
#ifndef COREGRAPHICS_INDEXBUFFERPOOL_H
#define COREGRAPHICS_INDEXBUFFERPOOL_H
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::IndexBufferPool
  
    (C) 2008 cTuo
*/
#include "coregraphics/indexbuffer.h"
#include "util/array.h"
#include "core/refcounted.h"

namespace CoreGraphics
{
class IndexBufferPool : public Core::RefCounted
{
	DeclareClass(IndexBufferPool);
public:
	IndexBufferPool();
	~IndexBufferPool();

	void Reset(SizeT indexCount, IndexType::Code type);
	Ptr<IndexBuffer> GetBuffer();
	void FlushAtFrameStart();
	void Lock(void*& vertexPointer, SizeT lockCount, SizeT& startVertex);
	void Unlock();
	bool CheckSpace(SizeT lockCount);

	DWORD flushCnt;

protected:
	SizeT indexSize;
	SizeT indexCount;
	SizeT position;		

	bool  locked;
	bool  flush;

	Ptr<IndexBuffer> buffer;
};
}
//------------------------------------------------------------------------------
#endif
