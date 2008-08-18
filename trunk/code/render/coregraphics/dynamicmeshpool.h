#pragma once
#ifndef COREGRAPHICS_DYNAMICMESHPOOL_H
#define COREGRAPHICS_DYNAMICMESHPOOL_H
//------------------------------------------------------------------------------
/**
	��̬mesh����أ���ͬ����������̬���䶥��
	
	this->meshPool = MeshPool::Create();
	this->meshPool->Reset(ResourceId("aa"), sizeof(MeshTest), vertexSize, 3, indexSize, vertexComponents);
	iii = this->meshPool->LockIndexed();
	Memory::Copy(indices, iii, indexSize*sizeof(WORD));
	this->meshPool->UnlockIndexed();

	this->curGroup = this->meshPool->Alloc(vertices);

	���õ��ĵط�����ApplyPrimitive������Ⱦ���黺�壨ƫ��λ�ã�

	*�����̶���С����ռ䡣
	*������ʱ�ͷ������������ݣ���DISCARD��

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
		DWORD vertexSize,										// fvf,�����С
		DWORD vertexCount,										// ��������
		const Util::Array<VertexComponent>& vertexComponents,
		DWORD indexSize,										// ��������
		IndexType::Code type);									// ��������

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