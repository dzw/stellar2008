//------------------------------------------------------------------------------
//  vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/vertexchunkpool.h"
#include "coregraphics/MemoryVertexBufferLoader.h"
#include "resources/ResourceLoader.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::VertexChunkPool, 'MHPL', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

VertexChunkPool::VertexChunkPool():
	blockSize(0),
	poolSize(0),
	buffer(0)
{
}

VertexChunkPool::~VertexChunkPool()
{
	if (this->buffer.isvalid())
	{
		this->buffer->Unload();
		this->buffer = 0;
	}
}

//------------------------------------------------------------------------------
/**
	blockVertexCount:一块包含的顶点数
	blockCount:块数
*/
void 
VertexChunkPool::Reset(DWORD vertexSize, DWORD blockVertexCount, DWORD blockCount, 
						const Util::Array<VertexComponent>& vertexComponents)
{
	this->vertexSize = vertexSize;
	this->blockSize = vertexSize * blockVertexCount;
	this->poolSize = blockSize * blockCount;

	this->buffer = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents,
		blockVertexCount*blockCount,					// 顶点数量
		poolSize,										// 缓存大小
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



//------------------------------------------------------------------------------
/**
	返回某个块的内存
*/
void* 
VertexChunkPool::Lock(SizeT index)
{
	return this->buffer->Map(VertexBuffer::MapWriteNoOverwrite, index, this->blockSize);
}

//------------------------------------------------------------------------------
/**
*/
void
VertexChunkPool::Unlock()
{
	this->buffer->Unmap();
}

bool 
VertexChunkPool::Full()const
{
    return this->freeList.IsEmpty();
}

DWORD 
VertexChunkPool::Alloc(void *data)
{
    if (!this->freeList.IsEmpty())
    {
        DWORD ret = this->freeList.Back();
		this->freeList.EraseIndex(this->freeList.Size()-1);
        if(data)
        {
			void* dest = this->buffer->Map(VertexBuffer::MapWriteNoOverwrite, ret, this->blockSize);
            if(dest)
            {
				Memory::Copy(data, dest, this->blockSize);
                this->buffer->Unmap();
            }
        }
        return ret;
    }
    return -1;
}

void 
VertexChunkPool::Free(DWORD offset)
{
	n_assert(offset != -1);
	this->freeList.Append(offset);
}

void 
VertexChunkPool::FreeAll()
{
	if (this->blockSize == 0)
		return;
	DWORD blockCount = this->poolSize / this->blockSize;
	this->freeList.Clear();
	this->freeList.Reserve(blockCount);
	for (SizeT i = 0; i < blockCount; i++)
		this->freeList[i] = (this->freeList.Size() - 1 - i) * blockSize;
}

}

