#pragma once
#ifndef KOK_CLIFFTABLE_H
#define KOK_CLIFFTABLE_H
//------------------------------------------------------------------------------
/**
	@class KOK::CliffTable

	(C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "util/array.h"
#include "util/string.h"
#include "io/stream.h"

namespace KOK
{

class CliffTable : public Core::RefCounted
{
	DeclareClass(CliffTable);

public:
	enum CliffType
	{
		Cliff_Natural = 0,
		Cliff_Artificial = 1,
	};

	CliffTable();
	~CliffTable();

	void ImportFromMemory(const Ptr<IO::Stream>& stream,int tile_tex_count);
	CliffType GetCliffType(unsigned int idx);
	Util::String& GetTexName(unsigned int idx); 

private:
	struct CliffData
	{
		Util::String m_szTexName;
		int      m_iCliffExterior;
		int      m_iTileTexIndex;
		CliffData() : m_iCliffExterior(0),m_iTileTexIndex(0) {};
		CliffData(const CliffData& data) : m_iCliffExterior(data.m_iCliffExterior), m_iTileTexIndex(data.m_iTileTexIndex) { m_szTexName=data.m_szTexName; };
		CliffData& operator=(const CliffData& data)
		{
			m_iCliffExterior=data.m_iCliffExterior; m_iTileTexIndex=data.m_iTileTexIndex;
			m_szTexName=data.m_szTexName; return *this;
		}
	};

	Util::Array<CliffData> m_CliffDatas;
};
}
#endif
