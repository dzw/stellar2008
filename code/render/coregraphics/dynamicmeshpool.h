#pragma once
#ifndef COREGRAPHICS_DYNAMICMESHPOOL_H
#define COREGRAPHICS_DYNAMICMESHPOOL_H
//------------------------------------------------------------------------------
/**
	动态mesh缓冲池，相同的索引，动态分配顶点
	
	this->meshPool = MeshPool::Create();
	this->meshPool->Reset(ResourceId("aa"), sizeof(MeshTest), vertexSize, 3, indexSize, vertexComponents);
	iii = this->meshPool->LockIndexed();
	Memory::Copy(indices, iii, indexSize*sizeof(WORD));
	this->meshPool->UnlockIndexed();

	this->curGroup = this->meshPool->Alloc(vertices);

	在用到的地方调用ApplyPrimitive设置渲染哪组缓冲（偏移位置）

	*不按固定大小分配空间。
	*缓冲满时释放整个缓冲数据（用DISCARD）

	2008 cTuo
*/    
#include "util/array.h"
#include "core/refcounted.h"
#include "coregraphics/vertexbufferpool.h"
#include "coregraphics/indexbufferpool.h"
#include "coregraphics/primitivegroup.h "

namespace CoreGraphics
{
class DynamicMeshPool : public Core::RefCounted
{
    DeclareClass(DynamicMeshPool);
public:
    DynamicMeshPool();
    ~DynamicMeshPool();

	void Reset( 
		DWORD vertexSize,										// fvf,顶点大小
		DWORD vertexCount,										// 顶点数量
		const Util::Array<VertexComponent>& vertexComponents,
		DWORD indexSize,										// 索引数量
		IndexType::Code type);									// 索引类型

	void Lock(void*& vertexPointer, SizeT lockCount, SizeT& startVertex);
	void Unlock();

	void LockIndexed(void*& indexPointer, SizeT lockCount, SizeT& startVertex);
	void UnlockIndexed();

	void ApplyPrimitive(const CoreGraphics::PrimitiveGroup& gp);

	Ptr<CoreGraphics::VertexBufferPool> GetVertexPool();
	Ptr<CoreGraphics::IndexBufferPool> GetIndexPool();
protected:
	Ptr<CoreGraphics::VertexBufferPool> vertexPool;
	Ptr<CoreGraphics::IndexBufferPool> indexPool;
};

inline Ptr<CoreGraphics::VertexBufferPool> 
DynamicMeshPool::GetVertexPool()
{
	return this->vertexPool;
}

inline Ptr<CoreGraphics::IndexBufferPool> 
DynamicMeshPool::GetIndexPool()
{
	return this->indexPool;
}

}
#endif