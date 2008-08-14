#pragma once
#ifndef KOK_TERRAININFO_H
#define KOK_TERRAININFO_H
//------------------------------------------------------------------------------
/**
    @class KOK::TerrainInfo
    
    (C) 2008 cTuo
*/
#include "core/refcounted.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace KOK
{

class TerrainInfo
{
public:
    /// constructor
    TerrainInfo();
    /// destructor
    virtual ~TerrainInfo();

	/// 横向的tile 数量
	unsigned int GetTileCountX() { return m_iMapWide; };
	/// 全部tile数量
	unsigned int GetTotalTileCount() { return m_iMapSize; };
	/// 横向district数量
	unsigned int GetDistrictCountX() { return m_iDistrictWide; };
	/// 全部district数量
	unsigned int GetTotalDistrictCount() { return m_iDistrictSize; };
	/// grid横向数量
	unsigned int GetGridCountX() { return m_iMapNodeWide; };
	/// grid总数量
	unsigned int GetTotalGridCount() { return m_iMapNodeSize; };

	/// set tile count X
	void SetTileCountX(unsigned int count);
	/// set map ID string 
	void SetMapIDString(char* sz_mapid) { m_szMapID=sz_mapid; };

	bool IsReady() { return m_bIsReady; };

	enum{DistrcitTileCount=8};
private:
	Util::String	m_szMapID;			// 地图ID
	unsigned int	m_iMapSize;			// 地图大小（人可站的空间）全部的格子数
	unsigned int	m_iMapWide;			// 地图宽度（人可站的空间）一行的格子数
	unsigned int	m_iMapNodeWide;		// 地表节点的宽度
	unsigned int	m_iMapNodeSize;		// 地表节点的大小
	unsigned int	m_iDistrictWide;	// 区域的宽度
	unsigned int	m_iDistrictSize;	// 区域的大小

	bool m_bIsReady;
};

} // namespace Models
//------------------------------------------------------------------------------
#endif
    