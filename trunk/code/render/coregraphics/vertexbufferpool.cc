//------------------------------------------------------------------------------
//  vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/vertexbufferpool.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::VertexBufferPool, 'VTBP', Core::RefCounted);

VertexBufferPool::VertexBufferPool()
{
}

VertexBufferPool::~VertexBufferPool()
{
}

void 
VertexBufferPool::Release()
{
}

SizeT 
VertexBufferPool::GetBlockCount()const
{
    return this->blockSize;
}

void 
VertexBufferPool::Reset(DWORD vertexSize, DWORD blockVertexCount, DWORD blockCount, DWORD fvf, DWORD uasge, DWORD pool)
{
    this->blockSize = blockVertexCount * vertexSize;
    this->poolSize = blockSize * blockCount;
    this->buffer = createVertexBuffer();
    this->freeList.SetSize(blockCount);
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
        this->freeList.ReSize(this->freeList.Size()-1);
        if(data)
        {
            void* dest = buffer->lock(ret, blockSize, 0);
            if(dest)
            {
                n_memcpy(dest, data, blockSize);
                buffer->unlock();
            }
        }
        return ret;
    }
    return -1;
}

void 
VertexBufferPool::Free(DWORD offset)
{
}

void 
VertexBufferPool::FreeAll()
{
}

}

