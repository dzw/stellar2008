//------------------------------------------------------------------------------
//  vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/meshpool.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::MeshPool, 'MHPL', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

MeshPool::MeshPool():
	blockSize(0),
	poolSize(0),
	meshBuffer(0)
{
}

MeshPool::~MeshPool()
{
	Release();
}

void 
MeshPool::Release()
{
	this->meshBuffer = 0;
}

SizeT 
MeshPool::GetBlockCount()const
{
    return this->blockSize;
}

//------------------------------------------------------------------------------
/**
	blockVertexCount:一块包含的顶点数
	blockCount:块数
*/
void 
MeshPool::Reset(const Resources::ResourceId& name, DWORD vertexSize, DWORD blockVertexCount, DWORD blockCount, DWORD indexSize, 
						const Util::Array<VertexComponent>& vertexComponents)
{
	this->vertexSize = vertexSize;
	this->blockSize = vertexSize * blockVertexCount;
	this->poolSize = blockSize * blockCount;

	this->meshBuffer = DynamicMesh::Create();
	this->meshBuffer->Initialize(name, PrimitiveTopology::TriangleList, vertexComponents, indexSize>0?true:false, blockVertexCount * blockCount, indexSize);

	Util::Array<CoreGraphics::PrimitiveGroup> primGroups;
	this->freeList.Reserve(blockCount);
	for (SizeT i = 0; i < blockCount; i++)
	{
		PrimitiveGroup primGroup;
		primGroup.SetBaseVertex(blockVertexCount*i);
		primGroup.SetNumVertices(blockVertexCount);
		primGroup.SetBaseIndex(0);
		primGroup.SetNumIndices(indexSize);
		primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

		primGroups.Append(primGroup);

		this->freeList[i] = i;
	}
	this->meshBuffer->AddPrimitiveGroup(primGroups);

    //for (SizeT i = 0; i < blockCount; i++)
    //    this->freeList[i] = (this->freeList.Size() - 1 - i) * blockSize;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<DynamicMesh>
MeshPool::GetBuffer()const
{
	return this->meshBuffer;
}

//------------------------------------------------------------------------------
/**
	返回某个块的内存
*/
void* 
MeshPool::Lock(SizeT index)
{
	void* dest;
	const CoreGraphics::PrimitiveGroup& group = this->meshBuffer->GetPrimitiveGroupAtIndex(index);
    this->meshBuffer->Begin(dest, group.GetBaseVertex()*this->vertexSize, this->blockSize);
	return dest;
}

//------------------------------------------------------------------------------
/**
*/
void
MeshPool::Unlock()
{
	this->meshBuffer->End();
}

//------------------------------------------------------------------------------
/**
	返回索引缓冲
*/
ushort* 
MeshPool::LockIndexed()
{
	ushort* dest;
	this->meshBuffer->BeginIndexed(dest);
	return dest;
}

//------------------------------------------------------------------------------
/**
*/
void
MeshPool::UnlockIndexed()
{
	this->meshBuffer->EndIndexed();
}

bool 
MeshPool::Full()const
{
    return this->freeList.IsEmpty();
}

DWORD 
MeshPool::Alloc(void *data)
{
    if (!this->freeList.IsEmpty())
    {
        DWORD ret = this->freeList.Back();
		this->freeList.EraseIndex(this->freeList.Size()-1);
        if(data)
        {
			const CoreGraphics::PrimitiveGroup& group = this->meshBuffer->GetPrimitiveGroupAtIndex(ret);
			void* dest;
			this->meshBuffer->Begin(dest, group.GetBaseVertex()*this->vertexSize, this->blockSize);
            if(dest)
            {
				Memory::Copy(data, dest, blockSize);
                this->meshBuffer->End();
            }
        }
        return ret;
    }
    return -1;
}

void 
MeshPool::Free(DWORD offset)
{
	this->freeList.Append(offset);
}

void 
MeshPool::FreeAll()
{
	DWORD blockCount = this->poolSize / this->blockSize;
	this->freeList.Clear();
	this->freeList.Reserve(blockCount);
	for (SizeT i = 0; i < blockCount; i++)
		this->freeList[i] = i;
	//	this->freeList[i] = (this->freeList.Size() - 1 - i) * blockSize;
}

}

