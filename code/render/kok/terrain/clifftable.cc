//------------------------------------------------------------------------------
//  districtnode.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kok/terrain/CliffTable.h"
#include "memory/memory.h"

namespace KOK
{
ImplementClass(KOK::CliffTable, 'CLTE', Core::RefCounted);

using namespace Memory;
using namespace IO;
using namespace Util;

CliffTable::CliffTable()
{

}

CliffTable::~CliffTable()
{

}

void 
CliffTable::ImportFromMemory(const Ptr<Stream>& stream,int tile_tex_count)
{
	MemoryMark mm(MemStack);

	int table_size;
	stream->Read(&table_size, sizeof(int));

	m_CliffDatas.Reserve(table_size);
	char *szName = new(MemStack, MAX_PATH)char;

	for(int i=0;i<table_size;i++)
	{
		// 读入悬崖的贴图档名长度
		int length;
		
		stream->Read(&length, sizeof(int));

		if(length)
		{
			// 读入悬崖的贴图档名
			stream->Read(szName, sizeof(char)*length);
			szName[length] = '\0';
			m_CliffDatas[i].m_szTexName = szName;
		}

		// 读入悬崖的外观ID
		int id;
		stream->Read(&id, sizeof(int));
		m_CliffDatas[i].m_iCliffExterior = id;

		//m_pCliffTable[i].iTileTexIndex  = iTextLayeSize-(iCliffTableSize-i);
		//jingjie add 2005.3.25
		m_CliffDatas[i].m_iTileTexIndex  = tile_tex_count-(table_size-i);
	}
}

//------------------------------------------------------------------------------
/**
	get cliff type
*/
CliffTable::CliffType 
CliffTable::GetCliffType(unsigned int idx)
{
	if (idx>=m_CliffDatas.Size()) return Cliff_Natural;
	if (m_CliffDatas[idx].m_iCliffExterior==0) return Cliff_Natural;
	return Cliff_Artificial;
}

//------------------------------------------------------------------------------
/**
	get tex name
*/
String& 
CliffTable::GetTexName(unsigned int idx)
{
	n_assert(idx<m_CliffDatas.Size());
	return m_CliffDatas[idx].m_szTexName;
}
}