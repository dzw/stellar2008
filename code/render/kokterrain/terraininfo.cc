//------------------------------------------------------------------------------
//  terraininfo.cc
//  (C) 2008 cTuo
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "kokterrain/terraininfo.h"

namespace KOK
{
ImplementClass(KOK::TerrainInfo, 'TNIO', Core::RefCounted);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
TerrainInfo::TerrainInfo():
	m_iMapSize(0),
	m_iMapWide(0),
	m_iDistrictWide(0),
	m_iDistrictSize(0),
	m_iMapNodeWide(0),
	m_iMapNodeSize(0),
	m_bIsReady(false)
{
}

//------------------------------------------------------------------------------
/**
*/
TerrainInfo::~TerrainInfo()
{
    
}

//------------------------------------------------------------------------------
/**
	set tile count X       
*/
void TerrainInfo::SetTileCountX(unsigned int count)
{
	m_iMapWide=count;
	m_iMapSize=count*count;
	m_iMapNodeWide=count+1;
	m_iMapNodeSize=(count+1)*(count+1);
	m_iDistrictWide=count/DistrcitTileCount;
	m_iDistrictSize=m_iDistrictWide*m_iDistrictWide;
	if (count>0) m_bIsReady=true;
}

} // namespace Models
