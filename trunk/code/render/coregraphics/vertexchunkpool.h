#pragma once
#ifndef COREGRAPHICS_MESHPOOL_H
#define COREGRAPHICS_MESHPOOL_H
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

	*按块分配，缓冲满的时候清空未使用的，而不是释放全部缓冲数据

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
		DWORD vertexSize,										// 顶点大小(fvf size)
		DWORD blockVertexCount,									// 块顶点数量
		DWORD blockCount,										// 块数量
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