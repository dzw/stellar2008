//------------------------------------------------------------------------------
//  indexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/indexbufferpool.h"
#include "coregraphics/memoryindexbufferloader.h "

namespace CoreGraphics
{
ImplementClass(CoreGraphics::IndexBufferPool, 'IXBP', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

IndexBufferPool::IndexBufferPool():
	buffer(0),
	indexCount(0),
	position(0),
	locked(false),
	flush(true),
	flushCnt(1)
{
}

IndexBufferPool::~IndexBufferPool()
{
	Unlock();
	if (this->buffer.isvalid())
	{
		this->buffer->Unload();
		this->buffer = 0;
	}
}

void 
IndexBufferPool::Reset(SizeT indexCount, IndexType::Code type)
{
	if (type == IndexType::Index16)
		this->indexSize = 2;
	else
		this->indexSize = 4;

	this->indexCount = indexCount;
	this->position = 0;

	this->buffer = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> vbLoader = MemoryIndexBufferLoader::Create();
	vbLoader->Setup(type,
		indexCount,						// 数量
		indexCount * this->indexSize,	// 缓存大小
		CoreGraphics::IndexBuffer::UsageDynamic, 
		CoreGraphics::IndexBuffer::AccessWrite);
	this->buffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->buffer->Load();
	this->buffer->SetLoader(0);
	n_assert(this->buffer->GetState() == IndexBuffer::Loaded);
}

Ptr<IndexBuffer> 
IndexBufferPool::GetBuffer()
{
	return this->buffer;
}

void 
IndexBufferPool::FlushAtFrameStart()
{
	this->flush = true;
	this->flushCnt++;
	if (this->flushCnt > 30000)
		this->flushCnt = 1;
}

void 
IndexBufferPool::Lock(void*& indexPointer, SizeT lockCount, SizeT& startIndex)
{
	startIndex = 0;
	if (lockCount > this->indexCount)
	{
		n_assert(0);
		return;
	}

	if (this->buffer.isvalid())
	{
		IndexBuffer::MapType dwFlags = IndexBuffer::MapWriteNoOverwrite;

		if (this->flush || (lockCount + this->position) >= this->indexCount)
		{
			this->flush = false;
			this->position = 0;
			dwFlags = IndexBuffer::MapWriteDiscard;
		}
		indexPointer = (void*)this->buffer->Map(dwFlags, this->position * this->indexSize, lockCount * this->indexSize);
		n_assert(indexPointer != NULL);
		this->locked = true;
		startIndex = this->position;
		this->position += lockCount;
	}
}

void 
IndexBufferPool::Unlock()
{
	if (this->locked && this->buffer.isvalid())
	{
		this->buffer->Unmap();
		this->locked = false;
	}
}

bool 
IndexBufferPool::CheckSpace(SizeT lockCount)
{
	if ((lockCount + this->position) >= this->indexCount)
		return false;
	return true;
}

}

