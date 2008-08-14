//------------------------------------------------------------------------------
//  vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/vertexbufferpool.h"
#include "coregraphics/MemoryVertexBufferLoader.h"
#include "resources/ResourceLoader.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::VertexBufferPool, 'VTBP', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

VertexBufferPool::VertexBufferPool():
	vertexCount(0),
	position(0),
	locked(false),
	flush(true),
	updateTime(0),
	lodTime(0),
	flushCnt(1)
{
	
}

VertexBufferPool::~VertexBufferPool()
{
	Unlock();
	if (this->buffer.isvalid())
	{
		this->buffer->Unload();
		this->buffer = 0;
	}
}

void 
VertexBufferPool::Reset(SizeT vertexSize, SizeT vertexNum, const Util::Array<VertexComponent>& vertexComponents)
{
	this->vertexCount = vertexNum;
	this->vertexSize = vertexSize;

	this->buffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents,
		this->vertexCount,						// 顶点数量
		this->vertexCount*vertexSize,						// 缓存大小
		CoreGraphics::VertexBuffer::UsageDynamic, 
		CoreGraphics::VertexBuffer::AccessWrite);
	this->buffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->buffer->Load();
	this->buffer->SetLoader(0);
	n_assert(this->buffer->GetState() == VertexBuffer::Loaded);
}

Ptr<VertexBuffer> 
VertexBufferPool::GetBuffer()
{
    return this->buffer;
}

void 
VertexBufferPool::FlushAtFrameStart()
{
	this->flush = true;
	this->flushCnt++;
	if (this->flushCnt > 30000)
		this->flushCnt = 1;
}

void 
VertexBufferPool::FlushVB()
{
	this->flush = true;
}

//------------------------------------------------------------------------------
/**
	vertexPointer:返回缓冲地址
	lockCount:要返回的缓冲顶点个数（不是缓冲大小）
	startVertex:缓冲顶点的开始位置
*/
void 
VertexBufferPool::Lock(void*& vertexPointer, SizeT lockCount, SizeT& startVertex)
{
	startVertex = 0;

	if (lockCount > this->vertexCount)
	{
		n_assert(0);
		return;
	}

	if (this->buffer.isvalid())
	{
		VertexBuffer::MapType dwFlags = VertexBuffer::MapWriteNoOverwrite;

		if (this->flush || ((lockCount + this->position) >= this->vertexCount))
		{
			this->flush = false;
			this->position = 0;
			dwFlags = VertexBuffer::MapWriteDiscard;
		}
		vertexPointer = (void*)this->buffer->Map(dwFlags, this->position * this->vertexSize, lockCount * this->vertexSize);
		n_assert(vertexPointer != NULL);
		this->locked = true;
		startVertex = this->position;
		this->position += lockCount;
	}
}

void 
VertexBufferPool::Unlock()
{
	if (this->locked && this->buffer.isvalid())
	{
		this->buffer->Unmap();
		this->locked = false;
	}
}

bool 
VertexBufferPool::CheckSpace(SizeT lockCount)
{
	if ((lockCount + this->position) >= this->vertexCount)
		return false;
	return true;
}

void 
VertexBufferPool::SetLodTime(DWORD lodTime)
{
	this->lodTime = lodTime;
}

void 
VertexBufferPool::FrameMove(DWORD frameTime)
{
	this->updateTime = frameTime;
	if (this->updateTime >= this->lodTime)
	{
		this->updateTime -= this->lodTime;
		this->flush = true;
		this->flushCnt++;
		if (this->flushCnt > 30000)
			this->flushCnt = 1;
		this->position = 0;
		this->discard = true;
	}
}

void 
VertexBufferPool::EndScene()
{
	this->discard = false;
}

}

