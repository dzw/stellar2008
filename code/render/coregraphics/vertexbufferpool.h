#pragma once
#ifndef COREGRAPHICS_VERTEXBUFFERPOOL_H
#define COREGRAPHICS_VERTEXBUFFERPOOL_H
//------------------------------------------------------------------------------
/**
	¶¯Ì¬¶¥µã»º³å³Ø
	
	cTuo
*/    
#include "coregraphics/vertexbuffer.h"
#include "util/array.h"
#include "core/refcounted.h"

namespace CoreGraphics
{
class VertexBufferPool : public Core::RefCounted
{
    DeclareClass(VertexBufferPool);
public:
    VertexBufferPool();
    ~VertexBufferPool();

    void Release();
    SizeT GetBlockCount()const;
	void Reset(DWORD vertexSize, DWORD blockVertexCount, DWORD blockCount, const Util::Array<VertexComponent>& vertexComponents);
	Ptr<VertexBuffer> GetBuffer();
    bool Full()const;
    DWORD Alloc(void *data);
    void Free(DWORD offset);
    void FreeAll();

protected:
    Util::Array<DWORD> freeList;
    DWORD blockSize;
    DWORD poolSize;
    Ptr<VertexBuffer> buffer;
};
}
#endif