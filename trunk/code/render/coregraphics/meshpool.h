#pragma once
#ifndef COREGRAPHICS_MESHPOOL_H
#define COREGRAPHICS_MESHPOOL_H
//------------------------------------------------------------------------------
/**
	动态mesh缓冲池，相同的索引，动态分配顶点
	
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
		DWORD vertexSize,										// 顶点大小(fvf size)
		DWORD blockVertexCount,									// 块顶点数量
		DWORD blockCount,										// 块数量
		DWORD indexSize,										// 索引数量，0就不创建索引缓冲
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