//------------------------------------------------------------------------------
//  vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/vertexbufferpool.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::VertexBufferPool, 'VTBP', Core::RefCounted);

VertexBufferPool::VertexBufferPool():
	,blockSize(0)
	,poolSize(0)
	,buffer(0)
{
}

VertexBufferPool::~VertexBufferPool()
{
	Release();
}

void 
VertexBufferPool::Release()
{
	this->buffer = 0;
}

SizeT 
VertexBufferPool::GetBlockCount()const
{
    return this->blockSize;
}

void 
VertexBufferPool::Reset(DWORD vertexSize, DWORD blockVertexCount, DWORD blockCount, 
						const Util::Array<VertexComponent>& vertexComponents/*DWORD fvf, DWORD uasge, DWORD pool*/)
{
	this->blockSize = blockVertexCount * vertexSize;
	this->poolSize = this->blockSize * blockCount;

	this->buffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents,
		blockVertexCount*blockCount,	// 顶点数量
		NULL,
		blockSize,						// 缓存大小
		CoreGraphics::VertexBuffer::UsageDynamic, 
		CoreGraphics::VertexBuffer::AccessWrite);
	this->buffer->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->buffer->Load();
	this->buffer->SetLoader(0);
	n_assert(this->buffer->GetState() == VertexBuffer::Loaded);

    this->freeList.Reserve(blockCount);
    for (SizeT i = 0; i < blockCount; i++)
        this->freeList[i] = (this->freeList.Size() - 1 - i) * blockSize;
}

Ptr<VertexBuffer> 
VertexBufferPool::GetBuffer()
{
    return this->buffer;
}

bool 
VertexBufferPool::Full()const
{
    return this->freeList.IsEmpty();
}

DWORD 
VertexBufferPool::Alloc(void *data)
{
    if (this->freeList.IsEmpty())
    {
        DWORD ret = this->freeList.Back();
		this->freeList.EraseIndex(this->freeList.Size()-1);
        if(data)
        {
			void* dest = buffer->Map(CoreGraphics::VertexBuffer::MapWriteDiscard, ret, blockSize);
            if(dest)
            {
				Memory::Copy(data, dest, blockSize);
                buffer->Unmap();
            }
        }
        return ret;
    }
    return -1;
}

void 
VertexBufferPool::Free(DWORD offset)
{
	this->freeList.Append(offset);
}

void 
VertexBufferPool::FreeAll()
{
	DWORD blockCount = this->poolSize / this->blockSize;
	this->freeList.Clear();
	this->freeList.Reserve(blockCount);
	for (SizeT i = 0; i < blockCount; i++)
		this->freeList[i] = (this->freeList.Size() - 1 - i) * blockSize;
}

}

