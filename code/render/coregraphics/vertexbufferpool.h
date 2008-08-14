#pragma once
#ifndef COREGRAPHICS_VERTEXBUFFERPOOL_H
#define COREGRAPHICS_VERTEXBUFFERPOOL_H
//------------------------------------------------------------------------------
/**
	��̬���㻺���
	
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
	/// ������λ�ã����ǻ����Сλ�ã�
	SizeT position;		
	
	bool  locked;
	bool  flush;

	DWORD updateTime;
	DWORD lodTime;

	Ptr<VertexBuffer> buffer;
};
}
#endif