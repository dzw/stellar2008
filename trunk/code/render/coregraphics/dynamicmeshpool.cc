//------------------------------------------------------------------------------
//  vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/dynamicmeshpool.h"
#include "coregraphics/renderdevice.h"

namespace CoreGraphics
{
ImplementClass(CoreGraphics::DynamicMeshPool, 'DMPL', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

DynamicMeshPool::DynamicMeshPool():
	vertexPool(0),
	indexPool(0)
{
}

DynamicMeshPool::~DynamicMeshPool()
{
	this->vertexPool = 0;
	this->indexPool = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
DynamicMeshPool::Reset(DWORD vertexSize,										// fvf,顶点大小
					   DWORD vertexCount,										// 顶点数量
					   const Util::Array<VertexComponent>& vertexComponents,
					   DWORD indexSize,										// 索引数量
					   IndexType::Code type)
{
	if (vertexSize > 0 && vertexCount > 0)
	{
		this->vertexPool = VertexBufferPool::Create();
		this->vertexPool->Reset(vertexSize, vertexCount, vertexComponents);
	}

	if (indexSize > 0 )
	{
		this->indexPool = IndexBufferPool::Create();
		this->indexPool->Reset(indexSize, type);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
DynamicMeshPool::Lock(void*& vertexPointer, SizeT lockCount, SizeT& startVertex)
{
	return this->vertexPool->Lock(vertexPointer, lockCount, startVertex);
}

//------------------------------------------------------------------------------
/**
*/
void
DynamicMeshPool::Unlock()
{
	this->vertexPool->Unlock();
}

//------------------------------------------------------------------------------
/**
	返回索引缓冲
*/
void 
DynamicMeshPool::LockIndexed(void*& indexPointer, SizeT lockCount, SizeT& startVertex)
{
	return this->indexPool->Lock(indexPointer, lockCount, startVertex);
}

//------------------------------------------------------------------------------
/**
*/
void
DynamicMeshPool::UnlockIndexed()
{
	this->indexPool->Unlock();
}

void 
DynamicMeshPool::ApplyPrimitive(const CoreGraphics::PrimitiveGroup& gp)
{
	if (this->vertexPool.isvalid())
	{
		RenderDevice::Instance()->SetVertexBuffer(this->vertexPool->GetBuffer());   
	}
	if (this->indexPool.isvalid())
	{
		RenderDevice::Instance()->SetIndexBuffer(this->indexPool->GetBuffer());
	}
	RenderDevice::Instance()->SetPrimitiveGroup(gp);
}

}

