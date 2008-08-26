#pragma once
#ifndef KOK_MODELDEF_H
#define KOK_MODELDEF_H
//------------------------------------------------------------------------------
/**
    @class 

    (C) 2008 cTuo
*/
#include "math/vector3.h"
#include "math/float2.h"
#include "math/ncolor.h"
#include "memory/memory.h"
#include "io/stream.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace KOK
{

#define D3DFVF_MDLVTX (D3DFVF_XYZB1|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)//

struct MDLVTX
{
	Math::vector3 p ;			// λ��
	FLOAT		  fBlend;		// The blend weight	(0.0->1.0),  Ϊ�˻�ҡ�����������Ҫ��
	Math::vector3 n ;			// ������
	DWORD	      color;		// ��ɫ
	FLOAT         tu;			// ��ͼ����
	FLOAT         tv;

	MDLVTX()
	{
		p     = Math::vector3(0.0f,0.0f,0.0f);
		fBlend = 1.0;
		n     = Math::vector3(0.0f,1.0f,0.0f);
		color = N_XRGB(255,255,255);
		tu    = 0.0f ;
		tu    = 0.0f ;
	};
};

struct VertexFVF
{
	Math::vector3 p ;			// λ��
	Math::vector3 n ;			// ������
	//Math::nColor  color;		// ��ɫ
	Math::float2  tex;			// ��ͼ����

	VertexFVF()
	{
		p     = Math::vector3(0.0f,0.0f,0.0f);
		n     = Math::vector3(0.0f,1.0f,0.0f);
		//color = Math::nColor(1.0f, 1.0f, 1.0f, 1.0f);
		tex   = Math::float2(0.0f, 0.0f);
	};
};

// ��ֱ��ʹ��DX���ͣ����Զ�������ṹ���Ӱ���ȡ����
struct AttributeRange 
{
	DWORD AttribId;
	DWORD FaceStart;
	DWORD FaceCount;
	DWORD VertexStart;
	DWORD VertexCount;
};

enum EThingSubMeshSpecialType
{
	TYPE_NORMAL = 0,
	TYPE_HD,            // "m_hd"
	TYPE_LIGHT,         // "m_light"
	TYPE_REPEAT,        // "m_repeat"
	TYPE_SELECT,        // "m_select"
	TYPE_WATER,         // "m_water"
	TYPE_EFF,           // "e_ff"
	TYPE_EFLAME,        // "e_flame"
}; 

inline void 
ReadString(const Ptr<IO::Stream>& stream, Util::String& str, int len)
{
	Memory::MemoryMark mm(Memory::MemStack);

	char* buf = new(Memory::MemStack, len+1)char;
	stream->Read(buf, len);
	buf[len] = 0;
	str = buf;
}

}  
//------------------------------------------------------------------------------
#endif

    