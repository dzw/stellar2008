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

	*������䣬��������ʱ�����δʹ�õģ��������ͷ�ȫ����������

	2008 cTuo
*/    
#include "coregraphics/vertexbuffer.h"
#include "util/array.h"
#include "core/refcounted.h"

namespace CoreGraphics
{
class VertexChunkPool : public Core::RefCounted
{
    DeclareClass(VertexChunkPool);
public:
    VertexChunkPool();
    ~VertexChunkPool();

    SizeT GetBlockCount()const;
	void Reset( 
		DWORD vertexSize,										// �����С(fvf size)
		DWORD blockVertexCount,									// �鶥������
		DWORD blockCount,										// ������
		const Util::Array<VertexComponent>& vertexComponents);	// fvf
	const Ptr<VertexBuffer> GetBuffer()const;
    bool Full()const;
    DWORD Alloc(void *data);
    void Free(DWORD offset);
    void FreeAll();

	void* Lock(SizeT index);
	void Unlock();
	
protected:
    Util::Array<DWORD> freeList;
	DWORD vertexSize;
    DWORD blockSize;
    DWORD poolSize;
    Ptr<VertexBuffer> buffer;
};

inline SizeT 
VertexChunkPool::GetBlockCount()const
{
	return this->blockSize;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::VertexBuffer>
VertexChunkPool::GetBuffer()const
{
	return this->buffer;
}

}
#endif