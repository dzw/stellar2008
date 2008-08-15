#pragma once
#ifndef COREGRAPHICS_MESHPOOL_H
#define COREGRAPHICS_MESHPOOL_H
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

	���̶���С���䣬DynamicMeshPool�����̶���С����

	2008 cTuo
*/    
#include "coregraphics/dynamicmesh.h"
#include "util/array.h"
#include "core/refcounted.h"

namespace CoreGraphics
{
class MeshPool : public Core::RefCounted
{
    DeclareClass(MeshPool);
public:
    MeshPool();
    ~MeshPool();

    void Release();
    SizeT GetBlockCount()const;
	void Reset(const Resources::ResourceId& name, 
		DWORD vertexSize,										// �����С(fvf size)
		DWORD blockVertexCount,									// �鶥������
		DWORD blockCount,										// ������
		DWORD indexSize,										// ����������0�Ͳ�������������
		const Util::Array<VertexComponent>& vertexComponents);	// fvf
	const Ptr<DynamicMesh> GetBuffer()const;
    bool Full()const;
    DWORD Alloc(void *data);
    void Free(DWORD offset);
    void FreeAll();

	void* Lock(SizeT index);
	void Unlock();

	ushort* LockIndexed();
	void UnlockIndexed();

	void ApplyPrimitive(SizeT index);
protected:
    Util::Array<DWORD> freeList;
	DWORD vertexSize;
    DWORD blockSize;
    DWORD poolSize;
    Ptr<DynamicMesh> meshBuffer;
};

inline void 
MeshPool::ApplyPrimitive(SizeT index)
{
	this->meshBuffer->ApplyPrimitive(index);
}

}
#endif