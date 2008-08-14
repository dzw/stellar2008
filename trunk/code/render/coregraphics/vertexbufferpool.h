#pragma once
#ifndef COREGRAPHICS_VERTEXBUFFERPOOL_H
#define COREGRAPHICS_VERTEXBUFFERPOOL_H
//------------------------------------------------------------------------------
/**
	动态顶点缓冲池
	
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

	void Reset(SizeT vertexSize, SizeT vertexCount, const Util::Array<VertexComponent>& vertexComponents);
	Ptr<VertexBuffer> GetBuffer();
    void FlushAtFrameStart();
	void FlushVB();
	void Lock(void*& vertexPointer, SizeT lockCount, SizeT& startVertex);
	void Unlock();
	bool CheckSpace(SizeT lockCount);
	void SetLodTime(DWORD lodTime);
	void FrameMove(DWORD frameTime);
	void EndScene();



	bool discard;
	DWORD flushCnt;

protected:
	SizeT vertexSize;
	SizeT vertexCount;
	/// 顶点数位置（不是缓冲大小位置）
	SizeT position;		
	
	bool  locked;
	bool  flush;

	DWORD updateTime;
	DWORD lodTime;

	Ptr<VertexBuffer> buffer;
};
}
#endif