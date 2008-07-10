#pragma once
#ifndef COREGRAPHICS_VERTEXBUFFER_H
#define COREGRAPHICS_VERTEXBUFFER_H
//------------------------------------------------------------------------------
/**
    
*/    
namespace CoreGraphics
{
class VertexBufferPool : public Core::RefCount
{
    DeclareClass(VertexBufferPool);
public:
    VertexBufferPool();
    ~VertexBufferPool();

    void Release();
    SizeT GetBlockCount()const
    {
        return blockSize;
    }

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


